#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <exception>

class Loader;

extern Loader* g_pLoader;

class Loader
{
	private:
		HINSTANCE	m_hInstance;
		HWND		m_hWnd;

		static BOOL CALLBACK DialogProc__Callback(HWND hWnd, unsigned int nMessage, WPARAM wParam, LPARAM lParam);

		void	EnableDebugPriv(void);
		void	Inject(DWORD dwProcessId, LPSTR lpszDll);
		void	Remove(DWORD dwProcessId, HMODULE hDll);
		HMODULE	GetRemoteDll(DWORD dwProcessId, LPSTR lpszDll);

		bool	Install(void);
		bool	Remove(void);

	public:
		Loader(HINSTANCE hInstance);
		~Loader();

		bool Initialize(void);
		bool DialogProc(HWND hWnd, unsigned int nMessage, WPARAM wParam, LPARAM lParam);
};