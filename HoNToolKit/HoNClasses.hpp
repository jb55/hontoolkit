#pragma once

namespace HoN
{
	class CPacket;
	class IBuffer;
	class IHeroEntity;
	class CHostClient;
	class IGame;
	class CWorldEntity;
	class CWorldEntityList;
	class CPlayer;
	class IUnitEntity;
	class CChatManager;

	class IBuffer
	{
		private:
			virtual void Function0() { };
			virtual void Function1() { };
			virtual void Function2() { };
			virtual void Function3() { };
			virtual void Function4() { };
			virtual void Function5() { };
			virtual void Function6() { };
			virtual void Function7() { };
			virtual void Function8() { };
			virtual void Function9() { };
			virtual void Function10() { };
			virtual void Function11() { };
			virtual void Function12() { };
			virtual void Function13() { };
			virtual void Function14() { };
			virtual void Function15() { };
			virtual void Function16() { };
			virtual void Function17() { };
			virtual void Function18() { };
			virtual void Function19() { };
			virtual void Function20() { };
			virtual void Function21() { };
			virtual void Function22() { };

		public:
			BYTE* pbData;//0x04
			DWORD _1;//0x08
			DWORD dwLength;//0x0C
		private:
			DWORD _2;//0x10
			DWORD _3;//0x14
		public:
			BYTE bData[8192];//0x18
	};

	class CPacket
	{
		private:
			DWORD _1[2];//0x00
			/*
			WORD _1;//0x00
			BYTE _2;//0x02
			DWORD _3;//0x03
			BYTE _4;//0x07*/
		public:
			IBuffer m_IBuffer;//+0x08
	};

	class IGame
	{
		public:
			class {
				private:
					BYTE _1[0x238];//0x00
				public:
					CPlayer* m_pPlayer;//0x238
			} *m_pUnkClass;
		private:
			BYTE _1[0xC];//+0x04
		public:
			class {
				private:
					BYTE _1[0x1B4];//0x00
				public:
					CWorldEntityList* m_pWorldEntityList;//0x1B4
			} *m_GameClasses;//0x10

			class {
				public:
					virtual void			Function0(void) { };//0x00
					virtual void			Function1(void) { };//0x04
					virtual IUnitEntity*	FindEntityById(DWORD dwUnitId) { return 0; }; //0x08
			}* m_pUnkClass2;
		private:
			BYTE _2[0x88];//0x14
		public:
			CHostClient* m_pHostClient;//0xA0
		private:
			BYTE _3[0x178];//0xA4
	};
	/* iterate over units
0D60F6D5  |. 8B0D 1C24660D  MOV ECX,DWORD PTR DS:[<&game_shared.?s_pGame@IGame@@0PAV1@A>]          ;  game_sha.?s_pGame@IGame@@0PAV1@A
0D60F6DB  |. 8B09           MOV ECX,DWORD PTR DS:[ECX]
0D60F6DD  |. 8B49 14        MOV ECX,DWORD PTR DS:[ECX+14]
0D60F6E0  |. 8B11           MOV EDX,DWORD PTR DS:[ECX]
0D60F6E2  |. 83EC 14        SUB ESP,14
0D60F6E5  |. 53             PUSH EBX
0D60F6E6  |. 55             PUSH EBP
0D60F6E7  |. 8B6C24 2C      MOV EBP,DWORD PTR SS:[ESP+2C]
0D60F6EB  |. 8B85 38020000  MOV EAX,DWORD PTR SS:[EBP+238]
0D60F6F1  |. 8B40 78        MOV EAX,DWORD PTR DS:[EAX+78]                                          ;  cPlayer::+0x78 -> UnitId
0D60F6F4  |. 56             PUSH ESI
0D60F6F5  |. 57             PUSH EDI
0D60F6F6  |. 50             PUSH EAX
0D60F6F7  |. 8B42 08        MOV EAX,DWORD PTR DS:[EDX+8]
0D60F6FA  |. FFD0           CALL EAX
0D60F6FC  |. 85C0           TEST EAX,EAX
	*/

	class CHostClient
	{

	};

	class IHeroEntity
	{

	};

	class CWorldEntity
	{
		public:
			DWORD m_dwEntityId;//0x00
	};

	class CWorldEntityList
	{

	};

	class CSystem
	{
		private:
			BYTE _1[0xD0];//0x00
		public:
			HWND m_hWnd;//0xD0
			void* m_fpWNDPROC;//0xD4
	};

	class CPlayer
	{
		private:
			BYTE _1[0x78];//0x00
		public:
			DWORD m_dwHeroEntityId;//0x78
	};
	
	class IUnitEntity
	{

	};

	class CChatManager
	{
		private:
			DWORD _1;
	};

	class CConsole
	{

	};

	class CUIManager
	{

	};


	class CInterface
	{

	};
};