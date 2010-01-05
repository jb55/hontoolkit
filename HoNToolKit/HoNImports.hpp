#pragma once

#include "Import.hpp"
#include <string>
#include "HoNClasses.hpp"

namespace HoN
{
	static Import<HoN::IGame*>									s_pGame("s_pGame", false);
	static Import<HoN::CSystem*>								pK2System("K2System", false);
	static Import<HoN::CChatManager**>							pChatManager("pChatManager", false);
	static Import<HoN::CConsole*>								pConsole("g_pConsole", false);

	static Import<void (CPacket::**)()>							CPacket_ctor("CPacket_ctor");
	static Import<void (__stdcall*)(IBuffer*)>					IBuffer_Init("IBuffer_Init", false);
	static Import<void (CHostClient::**)(IBuffer*, BOOL bUnk)>	CHostClient_SendGameData("CHostClient_SendGameData");
	static Import<void (CChatManager::**)(int, std::wstring*)>	CChatManager_AddGameChatMessage("CChatManager_AddGameChatMessage");
	static Import<void (CConsole::**)(std::wstring*)>			CConsole_Execute("CConsole_Execute");
	static Import<CUIManager* (CUIManager::**)(std::wstring*)>	CUIManager_SetActiveInterface("k2.dll");
	static Import<CInterface* (CUIManager::**)(void)>			CUIManager_GetActiveInterface("k2.dll");
};