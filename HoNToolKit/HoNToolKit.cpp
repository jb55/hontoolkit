#include "HoNToolKit.hpp"

#include <stdio.h>
#include <d3d9.h>
#include <sstream>

#include "Import.hpp"
#include "Patch.hpp"
#include "HoNImports.hpp"
#include "ModuleManager.hpp"
#include "ModuleInterface.hpp"
#include "DirectX.hpp"

HoNToolKit*		g_pHoNToolKit;

HoNToolKit::HoNToolKit(HMODULE hModule) : m_hModule(hModule)
{
	// Extrace Directory from Module FileName
	GetModuleFileName(this->m_hModule, this->m_szPath, sizeof(this->m_szPath));
	*(strrchr(this->m_szPath, '\\') + 1) = '\0';
}

HoNToolKit::~HoNToolKit()
{
	delete this->m_pGamePacketReceiveHook;
	delete this->m_pGamePacketSendHook;
	delete this->m_pGameEndSceneHook;
	delete this->m_pModuleManager;
}

bool
HoNToolKit::Initialize(void)
{
	Import<DWORD_PTR>	GamePacketReceiveHook("GamePacketReceiveHook", false);
	Import<DWORD_PTR>	GamePacketSendHook("CHostClient_SendGameData", false);
	Import<DWORD_PTR>	GameEndSceneHook("DXEndScene", false);
	IDirect3DDevice9*	pIDirect3DDevice9 = NULL;


	DWORD_PTR dwPtrD3Device = *(DWORD_PTR*)(DWORD_PTR)GameEndSceneHook;
	dwPtrD3Device += 0x43;
	pIDirect3DDevice9 = **(IDirect3DDevice9***)dwPtrD3Device;

	this->m_pDirectX = new DirectX(pIDirect3DDevice9);
	this->m_pModuleManager = new ModuleManager();

	this->m_pGamePacketReceiveHook		= new PatchCall(*(DWORD_PTR*)(DWORD_PTR)GamePacketReceiveHook, (DWORD_PTR)GamePacketReceive_Interception, 5);
	this->m_pGamePacketSendHook			= new PatchCall((DWORD_PTR)(DWORD_PTR)GamePacketSendHook, (DWORD_PTR)GamePacketSend_Interception, 6);
	this->m_pGameEndSceneHook			= new PatchCall((*(DWORD_PTR*)(DWORD_PTR)GameEndSceneHook) + 0x42, (DWORD_PTR)GameEndScene_Interception, 5);
	//this->m_pGameSetActiveInterfaceHook	= new PatchIAT(GetModuleHandle("cgame.dll"), "k2.dll", "?SetActiveInterface@CUIManager@@QAEXABV?$basic_string@_WU?$char_traits@_W@std@@V?$allocator@_W@2@@std@@@Z", (DWORD_PTR)GameSetActiveInterface_Interception);

	if(this->m_pDirectX->Initialize())
	{
		return true;
	}

	return false;
}

__declspec(naked)
void
HoNToolKit::GamePacketReceive_Interception(void)
{
	static HoN::IGame* pIGame = 0;
	__asm pushad
		if(!pIGame)
			pIGame = (HoN::IGame*)HoN::s_pGame;
	__asm popad
	__asm
	{
		mov eax, pIGame
		mov ecx, [esp+0x8]
		pushad
		push ecx
		mov ecx, g_pHoNToolKit
		call HoNToolKit::GamePacketReceive
		test al, al
		popad
		jz Block
		retn
Block:
		pop eax
		mov al, 1
		retn
	}
}

__declspec(naked)
void
HoNToolKit::GamePacketSend_Interception(void)
{
	__asm
	{
		pop eax
		push ebp
		mov ebp, esp
		and esp, 0xFFFFFFF8
		push eax
		pushad
		push [ebp+0x8] // IBuffer* pBuffer
		mov ecx, g_pHoNToolKit
		call HoNToolKit::GamePacketSend
		test al, al
		popad
		jz Block
		retn
Block:
		pop eax
		pop ebp
		mov al, 1
		retn 8
	}
}

__declspec(naked)
void
HoNToolKit::GameEndScene_Interception(void)
{
	__asm
	{
		mov ecx, g_pHoNToolKit
		jmp HoNToolKit::GameEndScene
	}
}

HoN::CInterface*
__thiscall
HoNToolKit::GameSetActiveInterface_Interception(HoN::CUIManager* pUIManager, std::wstring* psInterface)
{
	return 0;	
}

bool
HoNToolKit::GamePacketReceive(HoN::CPacket *pPacket)
{
	bool bBlock = false;

	this->m_pModuleManager->Lock();
	for(ModuleInterface* pModule = this->m_pModuleManager->GetFirstModule(); pModule; pModule = this->m_pModuleManager->GetNextModule(pModule))
	{
		if(!pModule->BeforeGamePacketReceived(pPacket->m_IBuffer.pbData, pPacket->m_IBuffer.dwLength))
			bBlock = true;
	}
	this->m_pModuleManager->Unlock();

	return !bBlock;
}

bool
HoNToolKit::GamePacketSend(HoN::IBuffer* pPacket)
{
	bool bBlock = false;
	
	this->m_pModuleManager->Lock();
	for(ModuleInterface* pModule = this->m_pModuleManager->GetFirstModule(); pModule; pModule = this->m_pModuleManager->GetNextModule(pModule))
	{
		if(!pModule->BeforeGamePacketSend(pPacket->pbData, pPacket->dwLength))
			bBlock = true;
	}
	this->m_pModuleManager->Unlock();

	switch(pPacket->pbData[0])
	{
		case 0x02: // GAME_PACKET_CHAT_ALL
		case 0x03: // GAME_PACKET_CHAT_TEAM
			if(pPacket->pbData[1] == '.')
			{
				this->GameParseInput((LPSTR)pPacket->pbData + 2);
				return false;
			}
			break;
	}

	return !bBlock;
}

IDirect3DDevice9*
HoNToolKit::GameEndScene(void)
{
//	this->m_pDirectX->DrawText("HoNToolKit v0.1", 50, 50, 0xFF00FFFF);

	return this->m_pDirectX->GetDirect3DDevice();
}

void
HoNToolKit::GameParseInput(LPSTR lpszInput)
{
	std::wstringstream	wStringStream;
	int					nArgs;
	char*				szArglist[1024] = {0};
	bool				bFoundCmd = false;

	char* p = _strdup(lpszInput);

	for(nArgs = 0; p && nArgs < 16; nArgs++)
	{
		szArglist[nArgs] = p;
		p = strchr(p, ' ');

		if(p)
			*(p++) = 0;
	}
	free(p);

	if(!_strcmpi(szArglist[0], "load") && nArgs == 2)
	{
		if(this->m_pModuleManager->LoadModule(szArglist[1]))
			this->PrintText("^yHoNToolKit^w: loaded '%s' successfully!", szArglist[1]);
		else
			this->PrintText("^yHoNToolKit^w:^r An error occured while loading '%s'.", szArglist[1]);

		return;
	}
	else if(!_strcmpi(szArglist[0], "unload") && nArgs == 2)
	{
		if(this->m_pModuleManager->UnloadModule(szArglist[1]))
			this->PrintText("^yHoNToolKit^w: unloaded '%s' successfully.", szArglist[1]);
		else
			this->PrintText("^yHoNToolKit^r: An error occured while unloading '%s'.", szArglist[1]);

		return;
	}
	else if(!_strcmpi(szArglist[0], "help"))
	{
		if(nArgs == 1)
		{
			this->PrintText("^yHoNToolKit commands^w: -");
			this->PrintText("^yload");
			this->PrintText("^yunload");
			this->PrintText("^yhelp");
			this->PrintText("^ymodules");
			this->PrintText("^yinfo");
		}
		else if(nArgs == 2)
		{
			if(!_strcmpi(szArglist[1], "load"))
			{
				this->PrintText("^yload^w: Loads a module. Example: Load Sniffer");
			}
			else if(!_strcmpi(szArglist[1], "unload"))
			{
				this->PrintText("^yload^w: Unoads a module. Example: Unoad Sniffer");
			}
			else if(!_strcmpi(szArglist[1], "help"))
			{
				this->PrintText("^yhelp^w: Displays information about commands. Example: help load");
			}
			else if(!_strcmpi(szArglist[1], "modules"))
			{
				this->PrintText("^ymodules^w: Displays a list of all modules.");
			}
			else if(!_strcmpi(szArglist[1], "info"))
			{
				this->PrintText("^info^w: Displays information about a certain module. Example: info module");
			}
			else
			{
				this->PrintText("^yhelp^r: Unknown command.");
			}
		}
		
		return;
	}
	else if(!_strcmpi(szArglist[0], "modules"))
	{
		this->m_pModuleManager->Lock();
		if(this->m_pModuleManager->GetFirstModule())
		{
			for(ModuleInterface* pModule = this->m_pModuleManager->GetFirstModule(); pModule; pModule = this->m_pModuleManager->GetNextModule(pModule))
			{
				this->PrintText("^y'^w%s^y'^w, by ^y'^w%s^y'^w ^y(^w0x%.8X^y)", pModule->m_pModuleInfo->szName, pModule->m_pModuleInfo->lpszAuthor, pModule->m_pModuleInfo->hModule);
			}
		}
		else {
			this->PrintText("^yHoNToolKit^r: No modules loaded!");
		}
		this->m_pModuleManager->Unlock();
		
		return;
	}
	else if(!_strcmpi(szArglist[0], "info") && nArgs == 2)
	{
		this->m_pModuleManager->Lock();
		
		if(ModuleInterface* pModule = this->m_pModuleManager->GetModule(szArglist[1]))
		{
			this->PrintText("^y'^w%s^y'^w: Author: ^y'^w%s^y'", pModule->m_pModuleInfo->szName, pModule->m_pModuleInfo->lpszAuthor);
			this->PrintText("^y'^w%s^y'^w: Description: ^y'^w%s^y'", pModule->m_pModuleInfo->szName, pModule->m_pModuleInfo->lpszDescription);
			this->PrintText("^y'^w%s^y'^w: E-Mail: ^y'^w%s^y'", pModule->m_pModuleInfo->szName, pModule->m_pModuleInfo->lpszEMail);
			this->PrintText("^y'^w%s^y'^w: Website: ^y'^w%s^y'", pModule->m_pModuleInfo->szName, pModule->m_pModuleInfo->lpszEMail);
			this->PrintText("^y'^w%s^y'^w: Version: ^y'^w%d.%d^y'", pModule->m_pModuleInfo->szName, LOBYTE(pModule->m_pModuleInfo->wVersion), HIBYTE(pModule->m_pModuleInfo->wVersion));
		}
		else {
			this->PrintText("^yHoNToolKit^r: Module not found!");
		}

		this->m_pModuleManager->Unlock();

		return;
	}
	else if(nArgs > 1)
	{
		this->m_pModuleManager->Lock();
		for(ModuleInterface* pModule = this->m_pModuleManager->GetFirstModule(); pModule; pModule = this->m_pModuleManager->GetNextModule(pModule))
		{
			if(!_strcmpi(pModule->m_pModuleInfo->szName, szArglist[0]))
			{
				bFoundCmd = pModule->Input(szArglist + 1, nArgs - 1);
				break;
			}
		}
		this->m_pModuleManager->Unlock();

		if(bFoundCmd)
			return;
	}

	this->PrintText("^yHoNToolKit^r: Command not found.");
}

bool
HoNToolKit::GetToolKitDirectory(LPSTR lpszPath, size_t nMax)
{
	return strcpy_s(lpszPath, nMax, this->m_szPath) == 0;
}

void
HoNToolKit::SendGamePacket(byte* pbPacket, size_t nLen)
{
	HoN::IBuffer IPacket;

	(*HoN::IBuffer_Init)(&IPacket);
	IPacket.pbData = pbPacket;
	IPacket.dwLength = nLen;
	(((HoN::IGame*)HoN::s_pGame)->m_pHostClient->**HoN::CHostClient_SendGameData)(&IPacket, TRUE);
}

void
HoNToolKit::PrintText(char* lpszFormat, ...)
{
	std::wstringstream wStringStream;

	va_list vaArgs;
	char szBuffer[1024] = "";
	va_start(vaArgs, lpszFormat);
	vsprintf_s(szBuffer, sizeof(szBuffer), lpszFormat, vaArgs);
	va_end(vaArgs);

	wStringStream << szBuffer;

	((*(HoN::CChatManager**)HoN::pChatManager)->**HoN::CChatManager_AddGameChatMessage)(1, &wStringStream.str());
}