#pragma once

#include <windows.h>
#include "../HoNToolKit/HoNToolKit.hpp"
#include "../HoNToolKit/ModuleInterface.hpp"

static ModuleInfo_t g_ModuleInfo = {
	"delzz",
	"rolle3k@gmail.com",
	"",
	"Empty sample project!",
	MAKEWORD(1, 0),
};

class Module : public ModuleInterface
{
	public:
		Module(HoNToolKit* pHoNToolKit, ModuleInfo_t* pModuleInfo);
		virtual ~Module();
		virtual bool Input(char** argv, int argc);
		virtual bool BeforeGamePacketReceived(byte* pbPacket, size_t nLen);
		virtual bool BeforeGamePacketSend(byte* pbPacket, size_t nLen);
};
