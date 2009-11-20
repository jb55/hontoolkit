#pragma once

#include "Import.hpp"
#include <string>
#include "HoNClasses.hpp"

namespace HoN
{
	static Import<HoN::IGame*>									s_pGame("s_pGame", false);
	static Import<HoN::CSystem*>								pK2System("K2System", false);
	static Import<HoN::CChatManager**>							pChatManager("pChatManager", false);

	static Import<void (CPacket::**)()>							CPacket_ctor("CPacket_ctor");
	static Import<void (__stdcall*)(IBuffer*)>					IBuffer_Init("IBuffer_Init", false);
	static Import<void (CHostClient::**)(IBuffer*)>				CHostClient_SendGameData("CHostClient_SendGameData");
	static Import<void (CChatManager::**)(int, std::wstring*)>	CChatManager_AddGameChatMessage("CChatManager_AddGameChatMessage");
};