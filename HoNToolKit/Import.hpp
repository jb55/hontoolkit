#pragma once

#include "FindPattern.hpp"
#include <stdio.h>

template <class T>
class Import
{
	private:
		T		m_pImport;
		bool	m_bClassFunction;
		char	m_szPatternName[64];

		void Init(void)
		{
			FindPattern Pattern(this->m_szPatternName);

			this->m_pImport = (T) (DWORD_PTR)Pattern;
		}

	public:
		Import(LPSTR lpszPatternName, bool bClassFunction = true) : m_bClassFunction(bClassFunction)
		{
			this->m_pImport = 0;
			strcpy_s(this->m_szPatternName, sizeof(this->m_szPatternName), lpszPatternName);

		}

		~Import() { }

		operator T()
		{
			
			if(!m_pImport)
			{
				this->Init();
			}

			return m_bClassFunction ? (T)&m_pImport : m_pImport;
		}
};