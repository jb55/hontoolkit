#include <windows.h>
#include "HoNToolKit.hpp"
#include "ModuleManager.hpp"
#include "ModuleInterface.hpp"

ModuleManager::ModuleManager()
{

}

ModuleManager::~ModuleManager()
{
	this->m_ModuleList.Lock();

	for(unsigned int i = 0; i < this->m_ModuleList.size(); i++)
	{
		this->UnloadModule(this->m_ModuleList[i]);
	}

	this->m_ModuleList.clear();
	this->m_ModuleList.Unlock();
}

bool
ModuleManager::LoadModule(LPSTR lpszName)
{
	typedef ModuleInterface* (__stdcall *fpCreateModule)(HoNToolKit* pHoNToolKit);

	HMODULE				hLibrary;
	char				szModulePath[MAX_PATH];
	char				szModuleName[MAX_PATH];
	ModuleInterface*	pModuleInterface;
	fpCreateModule		fnCreateModule;

	g_pHoNToolKit->GetToolKitDirectory(szModulePath, sizeof(szModulePath));
	strcat_s(szModulePath, sizeof(szModulePath), lpszName);

	strcpy_s(szModuleName, sizeof(szModuleName), lpszName);
	if(strchr(szModuleName, '.'))
		*strchr(szModuleName, '.') = '\0';

	this->m_ModuleList.Lock();

	if(!this->GetModule(szModuleName))
	{
		hLibrary = LoadLibrary(szModulePath);

		if(hLibrary)
		{
			fnCreateModule = (fpCreateModule)GetProcAddress(hLibrary, "_CreateModule@4");

			if(fnCreateModule)
			{
				pModuleInterface = fnCreateModule(g_pHoNToolKit);

				if(pModuleInterface)
				{
					pModuleInterface->m_pModuleInfo->hModule = hLibrary;
					strcpy_s(pModuleInterface->m_pModuleInfo->szName, sizeof(pModuleInterface->m_pModuleInfo->szName), szModuleName);
					this->m_ModuleList.push_back(pModuleInterface);
					this->m_ModuleList.Unlock();

					return true;
				}
			}
		}

		FreeLibrary(hLibrary);
	}
	
	this->m_ModuleList.Unlock();

	return false;
}

bool
ModuleManager::UnloadModule(LPSTR lpszName)
{
	bool				bRetValue = false;
	ModuleInterface*	pModuleInterface;

	this->m_ModuleList.Lock();

	pModuleInterface = this->GetModule(lpszName);

	if(pModuleInterface)
		bRetValue = this->UnloadModule(pModuleInterface);

	this->m_ModuleList.Unlock();

	return bRetValue;
}

bool
ModuleManager::UnloadModule(ModuleInterface* pModuleInterface)
{
	HMODULE hModule;

	if(pModuleInterface)
	{
		this->m_ModuleList.Lock();
		if(!this->m_ModuleList.empty())
		{
			for(unsigned int i = 0; i < this->m_ModuleList.size(); i++)
			{
				if(this->m_ModuleList[i] == pModuleInterface)
				{
					hModule = pModuleInterface->m_pModuleInfo->hModule;
					this->m_ModuleList.erase(this->m_ModuleList.begin() + i);
					delete pModuleInterface;
					FreeLibrary(hModule);
					this->m_ModuleList.Unlock();

					return true;
				}
			}
		}
		this->m_ModuleList.Unlock();
	}
	return false;
}

ModuleInterface*
ModuleManager::GetModule(LPSTR lpszName)
{
	ModuleInterface* pModule = NULL;

	this->m_ModuleList.Lock();

	for(unsigned int i = 0; i < this->m_ModuleList.size(); i++)
	{
		if(!_strcmpi(this->m_ModuleList[i]->m_pModuleInfo->szName, lpszName))
		{
			pModule = this->m_ModuleList[i];
			break;
		}
	}

	this->m_ModuleList.Unlock();

	return pModule;
}

ModuleInterface*
ModuleManager::GetFirstModule(void)
{
	ModuleInterface* pModule = NULL;

	this->m_ModuleList.Lock();
	if(!this->m_ModuleList.empty())
		pModule = this->m_ModuleList[0];
	this->m_ModuleList.Unlock();

	return pModule;
}

ModuleInterface*
ModuleManager::GetNextModule(ModuleInterface *pModuleInterface)
{
	ModuleInterface* pModule = NULL;

	this->m_ModuleList.Lock();

	if(!this->m_ModuleList.empty())
	{
		for(unsigned int i = 0; i < this->m_ModuleList.size(); i++)
		{
			if(i > 0 && this->m_ModuleList[i - 1] == pModuleInterface)
			{
				pModule = this->m_ModuleList[i];
				break;
			}
		}
	}

	this->m_ModuleList.Unlock();

	return pModule;
}