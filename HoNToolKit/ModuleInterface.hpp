#pragma once

class	HoNToolKit;
class	ModuleInterface;
struct	ModuleInfo_t;

class ModuleInterface
{
	public:
		ModuleInterface(HoNToolKit* pToolKit, ModuleInfo_t* pModuleInfo) : m_pHoNToolKit(pToolKit), m_pModuleInfo(pModuleInfo) { }
		virtual ~ModuleInterface() { }
		virtual bool Input(char** argv, int argc) { return false; } // true = vaild command, false = invaild command
		virtual bool BeforeGamePacketReceived(byte* pbPacket, size_t nLen) { return true; } // true = pass packet, false = block packet
		virtual bool BeforeGamePacketSend(byte* pbPacket, size_t nLen) { return true; } // true = pass packet, false = block packet

		ModuleInfo_t*	m_pModuleInfo;
	protected:
		ModuleInterface() { }

		HoNToolKit*		m_pHoNToolKit;
};

struct ModuleInfo_t
{
	LPSTR	lpszAuthor;
	LPSTR	lpszEMail;
	LPSTR	lpszWebsite;
	LPSTR	lpszDescription;
	WORD	wVersion;
	
	HMODULE	hModule;
	char	szName[MAX_PATH];
};