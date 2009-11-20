#include "Module.hpp"

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