#include "Module.hpp"
#include "Sniffer.hpp"

extern "C" __declspec(dllexport) ModuleInterface* __stdcall CreateModule(HoNToolKit* pHoNToolKit) 
{
	return new Module(pHoNToolKit, &g_ModuleInfo);
}

Module::Module(HoNToolKit* pHoNToolKit, ModuleInfo_t* pModuleInfo)
{
	this->m_pHoNToolKit = pHoNToolKit;
	this->m_pModuleInfo = pModuleInfo;

	this->m_pSniffer	= new Sniffer(m_pHoNToolKit);
}

Module::~Module()
{
	delete this->m_pSniffer;
}

bool
Module::Input(char** argv, int argc)
{
	if((!_strcmpi(argv[0], "show") || !_strcmpi(argv[0], "hide")) && argc >= 3)
	{
		bool	bShow		= !_strcmpi(argv[0], "show") ? true : false;
		bool	bReceive	= !_strcmpi(argv[1], "r") ? true : false;

		for(int i = 2; i < argc; i++)
		{
			char*	x = "";
			byte	bPacket	= (byte)strtoul(argv[i], &x, 0x10);

			if(bShow)
				this->m_pSniffer->ShowPacket(bPacket, bReceive);
			else
				this->m_pSniffer->HidePacket(bPacket, bReceive);
		}

		return true;
	}
	else if(!_strcmpi(argv[0], "list"))
	{
		this->m_pSniffer->ShowPacketList();

		return true;
	}
	else if(!_strcmpi(argv[0], "clear"))
	{
		this->m_pSniffer->ClearPacketList();

		return true;
	}

	return false;
}


bool
Module::BeforeGamePacketReceived(byte* pbPacket, size_t nLen)
{
	m_pSniffer->OnGamePacketReceive(pbPacket, nLen);

	return true;
}

bool
Module::BeforeGamePacketSend(byte* pbPacket, size_t nLen)
{
	m_pSniffer->OnGamePacketSend(pbPacket, nLen);

	return true;
}