#pragma once

#include <vector>

class ModuleInterface;

template<class _Ty> 
class SafeVector : public std::vector<_Ty>
{
	public:
		SafeVector()
		{
			InitializeCriticalSection(&m_CriticalSection);
		}

		~SafeVector()
		{
			DeleteCriticalSection(&m_CriticalSection);
		}

		void Lock(void)
		{
			EnterCriticalSection(&m_CriticalSection);
		}

		void Unlock(void)
		{
			LeaveCriticalSection(&m_CriticalSection);
		}

	private:
		CRITICAL_SECTION	m_CriticalSection;
};

class ModuleManager
{
	private:
		SafeVector<ModuleInterface*> m_ModuleList;

	public:
		ModuleManager();
		~ModuleManager();

		virtual bool				LoadModule(LPSTR lpszName);
		virtual bool				UnloadModule(LPSTR lpszName);
		virtual bool				UnloadModule(ModuleInterface* pModuleInterface);

		virtual ModuleInterface*	GetModule(LPSTR lpszName);

		virtual ModuleInterface*	GetFirstModule(void);
		virtual ModuleInterface*	GetNextModule(ModuleInterface* pModuleInterface);

		void						Lock(void) { this->m_ModuleList.Lock(); }
		void						Unlock(void) { this->m_ModuleList.Unlock(); }
};