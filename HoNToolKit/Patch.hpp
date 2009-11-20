#pragma once

class Patch;
class PatchCall;
class PatchJump;
class PatchIAT;
class PatchVTable;

class Patch
{
	public:
		Patch();
		virtual ~Patch();

		virtual bool Unhook(void);
		virtual bool Rehook(void);
		virtual bool IsHooked(void) { return this->m_bHooked; }

	protected:
		DWORD_PTR	m_dwAddress;
		DWORD_PTR	m_dwDetour;
		size_t		m_nLen;
		bool		m_bHooked;
		byte*		m_pbBackup;
		byte*		m_pbHook;
};

class PatchCall : public Patch
{
	public: 
		PatchCall(HMODULE hModule, DWORD_PTR dwOffset, DWORD_PTR dwDetour, size_t nLen);
		PatchCall(DWORD_PTR dwAddress, DWORD_PTR dwDetour, size_t nLen);
};

class PatchJump : public Patch
{
	public: 
		PatchJump(HMODULE hModule, DWORD_PTR dwOffset, DWORD_PTR dwDetour, size_t nLen);
		PatchJump(DWORD_PTR dwOffset, DWORD_PTR dwDetour, size_t nLen);
};

class PatchIAT : public Patch
{	
	public:
		PatchIAT(HMODULE hModule, LPSTR lpszModule, LPSTR lpszProcName, DWORD_PTR dwDetour);
};

class PatchVTable : public Patch
{	
	public:
		PatchVTable(DWORD_PTR dwClass, unsigned int nIndex, DWORD_PTR dwDetour);
};