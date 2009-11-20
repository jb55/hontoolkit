#include <windows.h>
#include "Patch.hpp"

Patch::Patch()
{

}

Patch::~Patch()
{
	DWORD dwOldProtection;

	if(this->m_bHooked)
	{
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, PAGE_EXECUTE_READWRITE, &dwOldProtection);
		memcpy((void*)this->m_dwAddress, this->m_pbBackup, this->m_nLen);
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, dwOldProtection, &dwOldProtection);
	}

	delete[] this->m_pbBackup;
	delete[] this->m_pbHook;
}

bool
Patch::Rehook(void)
{
	DWORD dwOldProtection;

	if(!this->m_bHooked)
	{
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, PAGE_EXECUTE_READWRITE, &dwOldProtection);
		memcpy((void*)this->m_dwAddress, this->m_pbHook, this->m_nLen);
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, dwOldProtection, &dwOldProtection);

		this->m_bHooked = true;

		return true;
	}

	return false;
}

bool
Patch::Unhook()
{
	DWORD dwOldProtection;

	if(this->m_bHooked)
	{
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, PAGE_EXECUTE_READWRITE, &dwOldProtection);
		memcpy((void*)this->m_dwAddress, this->m_pbBackup, this->m_nLen);
		VirtualProtect((void*)this->m_dwAddress, this->m_nLen, dwOldProtection, &dwOldProtection);

		this->m_bHooked = false;

		return true;
	}

	return false;
}

PatchCall::PatchCall(HMODULE hModule, DWORD_PTR dwOffset, DWORD_PTR dwDetour, size_t nLen)
{
	this->m_dwAddress	= reinterpret_cast<DWORD_PTR>(hModule) + dwOffset;
	this->m_dwDetour	= dwDetour;
	this->m_nLen		= nLen;
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	memset((void*)(this->m_pbHook + 5), 0x90, nLen - 5);
	this->m_pbHook[0] = 0xE8;
	*(unsigned long*)&this->m_pbHook[1] = this->m_dwDetour - (this->m_dwAddress + 5);

	memcpy(this->m_pbBackup, (void*)this->m_dwAddress, this->m_nLen);

	this->Rehook();
}

PatchCall::PatchCall(DWORD_PTR dwAddress, DWORD_PTR dwDetour, size_t nLen)
{
	this->m_dwAddress	= dwAddress;
	this->m_dwDetour	= dwDetour;
	this->m_nLen		= nLen;
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	memset((void*)(this->m_pbHook + 5), 0x90, nLen - 5);
	this->m_pbHook[0] = 0xE8;
	*(unsigned long*)&this->m_pbHook[1] = this->m_dwDetour - (this->m_dwAddress + 5);

	memcpy(this->m_pbBackup, (void*)this->m_dwAddress, this->m_nLen);

	this->Rehook();
}

PatchJump::PatchJump(HMODULE hModule, DWORD_PTR dwOffset, DWORD_PTR dwDetour, size_t nLen)
{
	this->m_dwAddress	= reinterpret_cast<DWORD_PTR>(hModule) + dwOffset;
	this->m_dwDetour	= dwDetour;
	this->m_nLen		= nLen;
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	memset((void*)(this->m_pbHook + 5), 0x90, nLen - 5);
	this->m_pbHook[0] = 0xE9;
	*(unsigned long*)&this->m_pbHook[1] = this->m_dwDetour - (this->m_dwAddress + 5);

	memcpy(this->m_pbBackup, (void*)this->m_dwAddress, this->m_nLen);

	this->Rehook();
}

PatchJump::PatchJump(DWORD_PTR dwAddress, DWORD_PTR dwDetour, size_t nLen)
{
	this->m_dwAddress	= dwAddress;
	this->m_dwDetour	= dwDetour;
	this->m_nLen		= nLen;
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	memset((void*)(this->m_pbHook + 5), 0x90, nLen - 5);
	this->m_pbHook[0] = 0xE9;
	*(unsigned long*)&this->m_pbHook[1] = this->m_dwDetour - (this->m_dwAddress + 5);

	memcpy(this->m_pbBackup, (void*)this->m_dwAddress, this->m_nLen);

	this->Rehook();
}

PatchIAT::PatchIAT(HMODULE hModule, LPSTR lpszModule, LPSTR lpszProcName, DWORD_PTR dwDetour)
{
	IMAGE_DOS_HEADER *dosHd = reinterpret_cast<IMAGE_DOS_HEADER*>(hModule);
	IMAGE_NT_HEADERS *ntHd;	
	
	DWORD_PTR dwOriginalFunction = reinterpret_cast<DWORD_PTR>(GetProcAddress(hModule, lpszProcName));

	this->m_dwDetour	= dwDetour;
	this->m_nLen		= sizeof(DWORD);
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	*(unsigned long*)this->m_pbBackup = dwOriginalFunction;
	*(unsigned long*)this->m_pbHook = dwDetour;

	if(dwOriginalFunction && dosHd && dosHd->e_magic == IMAGE_DOS_SIGNATURE)
	{
		ntHd = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<DWORD>(dosHd) + dosHd->e_lfanew);
		
		if(ntHd->Signature == IMAGE_NT_SIGNATURE)
		{
			for(IMAGE_IMPORT_DESCRIPTOR* importDesc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(reinterpret_cast<DWORD_PTR>(dosHd) + ntHd->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress); importDesc->FirstThunk; importDesc++)
			{
				if(_strcmpi(lpszModule, reinterpret_cast<char*>((importDesc->Name + reinterpret_cast<DWORD_PTR>(dosHd)))))
					continue;
			
				for(IMAGE_THUNK_DATA* thunkData = reinterpret_cast<IMAGE_THUNK_DATA*>(importDesc->FirstThunk + reinterpret_cast<DWORD_PTR>(dosHd)); thunkData->u1.Function; thunkData++)
				{
					if(thunkData->u1.Function == dwOriginalFunction)
					{
						this->m_dwAddress = thunkData->u1.Function;
						this->Rehook();
						return;
					}
				}
			}
		}
	}	
}

PatchVTable::PatchVTable(DWORD_PTR dwClass, unsigned int nIndex, DWORD_PTR dwDetour)
{
	this->m_dwAddress	= (*(DWORD_PTR*)dwClass + (nIndex * 4));
	this->m_dwDetour	= dwDetour;
	this->m_nLen		= sizeof(DWORD);
	this->m_bHooked		= false;
	this->m_pbHook		= new byte[this->m_nLen];
	this->m_pbBackup	= new byte[this->m_nLen];

	*(unsigned long*)this->m_pbHook = this->m_dwDetour;
	memcpy(this->m_pbBackup, (void*)this->m_dwAddress, this->m_nLen);

	this->Rehook();
}