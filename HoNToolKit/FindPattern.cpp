#include "HoNToolKit.hpp"
#include <Tlhelp32.h>
#include "FindPattern.hpp"
#include "tinyxml.h"

const LPSTR	FindPattern::m_XmlDatabase = "HoNToolKit.xml";

// This should never happen ..
FindPattern::FindPattern()
{
	this->m_dwPtr = 0;
}

// Empty destructor
FindPattern::~FindPattern()
{

}

// Grab pattern from XML file
FindPattern::FindPattern(LPSTR lpszName)
{
	char			szXmlPath[MAX_PATH];
	eType			Type;
	const char		*pszType = NULL, *pszLibrary = NULL, *pszPattern = NULL, *pszOffset = NULL, *pszExport = NULL;
	char *x			= "";
	DWORD			dwOffset;

	if(g_pHoNToolKit->GetToolKitDirectory(szXmlPath, sizeof(szXmlPath)))
	{
		if(!strcat_s(szXmlPath, sizeof(szXmlPath), FindPattern::m_XmlDatabase))
		{
			TiXmlDocument XmlDoc(szXmlPath);

			if(XmlDoc.LoadFile())
			{
				TiXmlElement* pPatterns = XmlDoc.FirstChildElement("Patterns");

				if(pPatterns)
				{
					for(TiXmlElement* pPattern = pPatterns->FirstChildElement(); pPattern; pPattern = pPattern->NextSiblingElement())
					{
						if(const char* pszName = pPattern->Attribute("name"))
							if(!_strcmpi(pszName, lpszName))
							{
								pszType = pPattern->Attribute("type");
								pszLibrary = pPattern->Attribute("library");
								pszOffset = pPattern->Attribute("offset");

								if(pszType && pszLibrary)
								{
									if(!_strcmpi(pszType, "pattern"))
										Type = this->TYPE_PATTERN;
									else if(!_strcmpi(pszType, "export"))
										Type = this->TYPE_EXPORT;
									else if(!_strcmpi(pszType, "offset"))
										Type = this->TYPE_OFFSET;
									else return;
							
									switch(Type)
									{
										case this->TYPE_PATTERN:
											pszPattern = pPattern->Attribute("pattern");

											if(pszPattern)
												Init((LPSTR)pszLibrary, Type, (LPSTR)pszPattern, pszOffset ? atoi(pszOffset) : 0);
											break;

										case this->TYPE_EXPORT:
											pszExport = pPattern->Attribute("symbol");

											if(pszExport)
												Init((LPSTR)pszLibrary, Type, (LPSTR)pszExport, pszOffset ? atoi(pszOffset) : 0 );
											break;

										case this->TYPE_OFFSET:
											dwOffset = strtoul(pszOffset, &x, 0x10);

											if(dwOffset)
												Init((LPSTR)pszLibrary, Type, NULL, dwOffset);
											break;
										default:
											break;
									}
								}
							}
					}
				}
			}
		}
	}
}

FindPattern::FindPattern(LPSTR lpszLib, eType Type, LPSTR lpszPattern, int nOffset)
{
	this->Init(lpszLib, Type, lpszPattern, nOffset);
}

void
FindPattern::Init(LPSTR lpszLib, eType Type, LPSTR lpszPattern, int nOffset)
{
	HMODULE	hModule;
	WORD*	pwPattern;
	size_t	nPatternSize;

	hModule = GetModuleHandle(lpszLib);

	switch(Type)
	{
		case this->TYPE_OFFSET:
			if(hModule)
			{
				this->m_dwPtr = (DWORD_PTR)hModule + (DWORD_PTR)nOffset;
			}
			break;
		case this->TYPE_EXPORT:
			if(hModule)
			{
				this->m_dwPtr = (DWORD_PTR)GetProcAddress(hModule, lpszPattern);

				if(this->m_dwPtr && nOffset)
				{
					this->m_dwPtr += nOffset;
				}
			}
			break;
		case this->TYPE_PATTERN:
			if(hModule)
			{
				pwPattern = this->MakeMask(lpszPattern, &nPatternSize);

				if(pwPattern)
				{
					this->m_dwPtr = this->FindMask(hModule, pwPattern, nPatternSize);
					
					if(this->m_dwPtr)
						this->m_dwPtr = (int)this->m_dwPtr + nOffset;

					delete[] pwPattern;
				}
			}
			break;
		default:
			break;
	}
}

WORD*
FindPattern::MakeMask(char* szMask, size_t* pnMaxCount)
{
	char*	pszFixedMask;
	WORD*	pwPattern;

	pszFixedMask = new char[strlen(szMask) + 1 ];
	memset(pszFixedMask, NULL, strlen(szMask) + 1);

	for(unsigned int i = 0, j = 0; i < strlen(szMask); i++)
	{
		if(szMask[i] == ' ')
			continue;

		if(szMask[i] == '?')
		{
			pszFixedMask[j++] = 'x';
			continue;
		}
		
		pszFixedMask[j++] = szMask[i];
	}

	pwPattern = new WORD[strlen(szMask) / 2];
	memset(pwPattern, NULL, strlen(szMask));

	for(unsigned int i = 0; i < strlen(pszFixedMask) / 2; i++)
	{
		char* x = "";
		char szBuffer[3] = "";

		memcpy(szBuffer, pszFixedMask + i*2, 2);

		unsigned long ulValue = strtoul(szBuffer, &x, 0x10);

		if(strlen(x))
			pwPattern[i] = 0x00;
		else pwPattern[i] = MAKEWORD(ulValue, 0xFF);
	}

	*pnMaxCount = strlen(pszFixedMask) / 2;

	return pwPattern;
}

DWORD_PTR
FindPattern::FindMask(HMODULE hModule, WORD *pwPattern, unsigned int nLen)
{
	HANDLE			hSnapshot		= CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	DWORD			dwModuleSize	= 0, dwRead = 0, dwOldProtection = 0;
	byte*			pbModule;
	MODULEENTRY32	Module;

	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		Module.dwSize = sizeof(MODULEENTRY32);

		if(Module32First(hSnapshot, &Module))
		{
			do
			{
				if(Module.hModule == hModule)
				{
					dwModuleSize = Module.modBaseSize;
					break;
				}
			} while(Module32Next(hSnapshot, &Module));
		}

		if(dwModuleSize)
		{
			pbModule = new byte[dwModuleSize];

			if(pbModule)
			{
				ReadProcessMemory(GetCurrentProcess(), hModule, pbModule, dwModuleSize, &dwRead);

				VirtualProtect(pbModule, dwModuleSize, PAGE_READWRITE, &dwOldProtection);

				if(dwRead == dwModuleSize)
				{
					for(DWORD_PTR dwAddress = (DWORD_PTR)hModule; dwAddress + nLen < dwAddress + dwModuleSize; dwAddress++)
					{
						if(this->CompareMask((DWORD_PTR)pbModule + (dwAddress - (DWORD_PTR)hModule), pwPattern, nLen))
							return dwAddress;
					}
				}

				VirtualProtect(pbModule, dwModuleSize, dwOldProtection, &dwOldProtection);

				delete[] pbModule;
			}
		}
	}

	return NULL;
}

bool
FindPattern::CompareMask(DWORD_PTR dwAddress, WORD* pwPattern, size_t nLen)
{
	unsigned int j = 0;

	for(DWORD_PTR i = dwAddress; i < dwAddress + nLen && j < nLen; i++)
	{
		if(HIBYTE(pwPattern[j]) != 0xFF)
		{
			j++;
			continue;
		}

		if(LOBYTE(pwPattern[j++]) != *(byte*)i)
			return false;
	}

	return true;
}