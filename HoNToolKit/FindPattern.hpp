#pragma once

#include <windows.h>

class FindPattern
{
	public:

		enum eType
		{
			TYPE_PATTERN,
			TYPE_EXPORT,
			TYPE_OFFSET,
		};

	private:
		static const LPSTR	m_XmlDatabase;
		DWORD_PTR			m_dwPtr;

		FindPattern();

		WORD*		MakeMask(char* szMask, size_t* pnMaxCount);
		DWORD_PTR	FindMask(HMODULE hModule, WORD* pwPattern, size_t nLen);
		bool		CompareMask(DWORD_PTR dwAddress,WORD* pwPattern, size_t nLen);

		void		Init(LPSTR lpszLib, eType Type, LPSTR lpszPattern, int nOffset);
	public:
	
		FindPattern(LPSTR lpszName);
		FindPattern(LPSTR lpszLib, eType Type, LPSTR lpszPattern, int nOffset);
		~FindPattern();

		operator DWORD_PTR() { return this->m_dwPtr; }
};