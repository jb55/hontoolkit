#include "Module.hpp"

extern "C" __declspec(dllexport) ModuleInterface* __stdcall CreateModule(HoNToolKit* pHoNToolKit) 
{
	return new Module(pHoNToolKit, &g_ModuleInfo);
}

Module::Module(HoNToolKit* pHoNToolKit, ModuleInfo_t* pModuleInfo)
{
	this->m_pHoNToolKit = pHoNToolKit;
	this->m_pModuleInfo = pModuleInfo;
}

Module::~Module()
{

}

bool
Module::Input(char** argv, int argc)
{

	return false;
}


bool
Module::BeforeGamePacketReceived(byte* pbPacket, size_t nLen)
{

	return true;
}

bool
Module::BeforeGamePacketSend(byte* pbPacket, size_t nLen)
{

	return true;
}