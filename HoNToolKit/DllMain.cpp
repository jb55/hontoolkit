#include "HoNToolKit.hpp"

BOOL
WINAPI
DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			g_pHoNToolKit = new HoNToolKit(hModule);
			return g_pHoNToolKit->Initialize();
		case DLL_PROCESS_DETACH:
			delete g_pHoNToolKit;
			break;
	}
	return true;
}