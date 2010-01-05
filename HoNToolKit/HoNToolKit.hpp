#pragma once

#include <windows.h>
#include <string>
#include "HoNClasses.hpp"

class	HoNToolKit;
class	ModuleManager;
class	Patch;
class	PatchCall;
class	PatchJump;
class	PatchIAT;
class	PatchVTable;
class	DirectX;
struct	IDirect3DDevice9;

extern HoNToolKit* g_pHoNToolKit;

class HoNToolKit
{
	private:
		HMODULE			m_hModule;
		char			m_szPath[MAX_PATH];

		DirectX*		m_pDirectX;

		PatchCall*		m_pGamePacketReceiveHook;
		PatchCall*		m_pGamePacketSendHook;
		PatchCall*		m_pGameEndSceneHook;
		PatchIAT*		m_pGameSetActiveInterfaceHook;

		ModuleManager*	m_pModuleManager;

		static		void						GamePacketReceive_Interception(void);
		static		void						GamePacketSend_Interception(void);
		static		void						GameEndScene_Interception(void);
		static		HoN::CInterface* __thiscall	GameSetActiveInterface_Interception(HoN::CUIManager* pUIManager, std::wstring* psInterface);

	public:
		HoNToolKit(HMODULE hModule);
		~HoNToolKit();

		bool Initialize(void);

		// ** Game Interceptions ** //
		bool				GamePacketReceive(HoN::CPacket* pPacket);
		bool				GamePacketSend(HoN::IBuffer* pPacket);
		IDirect3DDevice9*	GameEndScene(void);
		HoN::CInterface*	GameSetActiveInterface(std::wstring* psInterface);
		void				GameParseInput(LPSTR lpszInput);

		// ** Exports ** //
		virtual bool		GetToolKitDirectory(LPSTR lpszPath, size_t nMaxCount);
		virtual void		SendGamePacket(byte* pbPacket, size_t nLen);
		virtual void		PrintText(char* lpszFormat, ...);
};