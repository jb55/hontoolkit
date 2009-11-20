#include "Loader.hpp"
#include "resource.h"

Loader*	g_pLoader;

Loader::Loader(HINSTANCE hInstance) : m_hInstance(hInstance)
{
	this->EnableDebugPriv();
}

Loader::~Loader()
{

}

bool
Loader::Initialize(void)
{
	return !!DialogBox(this->m_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, Loader::DialogProc__Callback);
}

bool
Loader::DialogProc(HWND hWnd, unsigned int nMessage, WPARAM wParam, LPARAM lParam)
{
	switch(nMessage)
	{
		case WM_INITDIALOG:
			this->m_hWnd = hWnd;
			return true;
		case WM_CLOSE:
			return !!EndDialog(hWnd, NULL);
		case WM_COMMAND:
				switch(wParam)
				{
					case IDC_INSTALL:
						try {
							if(this->Install())
								MessageBeep(MB_OK);
						}
						catch(std::exception e)
						{
							MessageBox(NULL, e.what(), "Install Exception", MB_OK);
						}
						break;
					case IDC_REMOVE:
						try {
							if(this->Remove())
								MessageBeep(MB_OK);
						}
						catch(std::exception e)
						{
							MessageBox(NULL, e.what(), "Remove Exception", MB_OK);
						}
						break;
				}
			break;
	}
	return false;
}

BOOL
CALLBACK
Loader::DialogProc__Callback(HWND hWnd, unsigned int nMessage, WPARAM wParam, LPARAM lParam)
{

	return !!g_pLoader->DialogProc(hWnd, nMessage, wParam, lParam);
}

void
Loader::EnableDebugPriv(void)
{
	HANDLE				hToken;
	LUID				SeDebugNameValue;
	TOKEN_PRIVILEGES	TokenPrivileges;

	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &SeDebugNameValue))
		{
			TokenPrivileges.PrivilegeCount				= 1;
			TokenPrivileges.Privileges[0].Luid			= SeDebugNameValue;
 			TokenPrivileges.Privileges[0].Attributes	= SE_PRIVILEGE_ENABLED;

			if(AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
			{
				CloseHandle(hToken);
			}
			else
			{
				CloseHandle(hToken);
				throw std::exception("Couldn't adjust token privileges!");				
			}
		}
		else
		{
			CloseHandle(hToken);
			throw std::exception("Couldn't look up privilege value!");
		}
	}
	else
	{
		throw std::exception("Couldn't open process token!");
	}
}

void
Loader::Inject(DWORD dwProcessId, LPSTR lpszDll)
{
	HANDLE			hProc, hThread;
	HMODULE			hKernel32;
	FARPROC			fpLoadLibraryA;
	LPVOID			lpRemoteString;
	DWORD			dwWritten;
	char			szPath[MAX_PATH];

	GetCurrentDirectory(sizeof(szPath), szPath);
	strcat_s(szPath, sizeof(szPath), "\\");
	strcat_s(szPath, sizeof(szPath), lpszDll);

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	if(hProc)
	{
		hKernel32 = LoadLibrary("kernel32.dll");

		if(hKernel32)
		{
			fpLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");

			if(fpLoadLibraryA)
			{
				lpRemoteString = VirtualAllocEx(hProc, NULL, sizeof(szPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

				if(lpRemoteString)
				{
					WriteProcessMemory(hProc, lpRemoteString, szPath, sizeof(szPath), &dwWritten);

					if(dwWritten == sizeof(szPath))
					{
						hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)fpLoadLibraryA, lpRemoteString, NULL, NULL);

						if(hThread)
						{
							WaitForSingleObject(hThread, INFINITE);
							VirtualFreeEx(hProc, lpRemoteString, sizeof(szPath), MEM_RELEASE);
							FreeLibrary(hKernel32);
							CloseHandle(hThread);
							CloseHandle(hProc);
						}
						else
						{
							VirtualFreeEx(hProc, lpRemoteString, sizeof(szPath), MEM_RELEASE);
							CloseHandle(hProc);
							FreeLibrary(hKernel32);
							throw std::exception("Create Remote Thread failed!");
						}
					}
					else
					{
						VirtualFreeEx(hProc, lpRemoteString, sizeof(szPath), MEM_RELEASE);
						CloseHandle(hProc);
						FreeLibrary(hKernel32);
						throw std::exception("Couldn't write string into remote process!");
					}
				}
				else
				{
					CloseHandle(hProc);
					FreeLibrary(hKernel32);
					throw std::exception("Couldn't allocate memory!");					
				}
			}
			else
			{
				CloseHandle(hProc);
				FreeLibrary(hKernel32);
				throw std::exception("Couldn't get LoadLibraryA function pointer!");
			}
		}
		else
		{
			CloseHandle(hProc);
			throw std::exception("Couldn't get Kernel32.dll Handle!");
		}
	}
	else
	{
		throw std::exception("OpenProcess failed!");
	}
}

void
Loader::Remove(DWORD dwProcessId, HMODULE hDll)
{
	HANDLE			hProc, hThread;
	HMODULE			hKernel32;
	FARPROC			fpFreeLibrary;

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);

	if(hProc)
	{
		hKernel32 = LoadLibrary("kernel32.dll");

		if(hKernel32)
		{
			fpFreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");

			if(fpFreeLibrary)
			{
				hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)fpFreeLibrary, hDll, NULL, NULL);

				if(hThread)
				{
					WaitForSingleObject(hThread, INFINITE);
					FreeLibrary(hKernel32);
					CloseHandle(hThread);
					CloseHandle(hProc);
				}
				else
				{
					CloseHandle(hProc);
					FreeLibrary(hKernel32);
					throw std::exception("Create Remote Thread failed!");
				}
			}
			else
			{
				CloseHandle(hProc);
				FreeLibrary(hKernel32);
				throw std::exception("Couldn't get FreeLibrary function pointer!");
			}
		}
		else
		{
			CloseHandle(hProc);
			throw std::exception("Couldn't get Kernel32.dll Handle!");
		}
	}
	else
	{
		throw std::exception("OpenProcess failed!");
	}
}

HMODULE
Loader::GetRemoteDll(DWORD dwProcessId, LPSTR lpszDll)
{
	HANDLE			hSnapShot;
	MODULEENTRY32	ModuleEntry32;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if(hSnapShot != INVALID_HANDLE_VALUE)
	{
		if(Module32First(hSnapShot, &ModuleEntry32))
		{
			do
			{
				if(strstr(ModuleEntry32.szModule, lpszDll))
				{
					CloseHandle(hSnapShot);
					return ModuleEntry32.hModule;
				}
			} while(Module32Next(hSnapShot, &ModuleEntry32));
		}

		CloseHandle(hSnapShot);
	}

	return NULL;
}

bool
Loader::Install(void)
{
	HANDLE			hSnapShot;
	PROCESSENTRY32	ProcessEntry32;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	if(hSnapShot != INVALID_HANDLE_VALUE)
	{
		if(Process32First(hSnapShot, &ProcessEntry32))
		{
			do
			{
				if(!_strcmpi(ProcessEntry32.szExeFile, "HoN.exe"))
				{
					HMODULE hHoNToolKit = this->GetRemoteDll(ProcessEntry32.th32ProcessID, "HoNToolKit.DLL");

					if(!hHoNToolKit)
					{
						this->Inject(ProcessEntry32.th32ProcessID, "HoNToolKit.DLL");						
						return true;
					}
				}
			} while(Process32Next(hSnapShot, &ProcessEntry32));
		}
	}

	return false;
}

bool
Loader::Remove(void)
{
	HANDLE			hSnapShot;
	PROCESSENTRY32	ProcessEntry32;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	if(hSnapShot != INVALID_HANDLE_VALUE)
	{
		if(Process32First(hSnapShot, &ProcessEntry32))
		{
			do
			{
				if(!_strcmpi(ProcessEntry32.szExeFile, "HoN.exe"))
				{
					HMODULE hHoNToolKit = this->GetRemoteDll(ProcessEntry32.th32ProcessID, "HoNToolKit.DLL");

					if(hHoNToolKit)
					{
						this->Remove(ProcessEntry32.th32ProcessID, hHoNToolKit);						
						return true;
					}
				}
			} while(Process32Next(hSnapShot, &ProcessEntry32));
		}
	}
	return false;
}