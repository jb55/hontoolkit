#include "Loader.hpp"

BOOL
WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nShowCmd)
{
	bool bRetValue;

	g_pLoader = new Loader(hInstance);

	bRetValue = g_pLoader->Initialize();

	delete g_pLoader;

	return bRetValue;
}