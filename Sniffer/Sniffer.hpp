#pragma once

#include <map>

class Sniffer
{
	private:
		HoNToolKit*				m_pHoNToolKit;
		std::map<byte, LPSTR>	m_PacketSendNames;
		std::map<byte, LPSTR>	m_PacketRecvNames;
		std::map<byte, bool>	m_PacketSendList;
		std::map<byte, bool>	m_PacketRecvList;
		bool					m_bShowAll;

	public:
		Sniffer(HoNToolKit* pHoNToolKit);
		~Sniffer();

		bool		SetPacketName(byte bPacket, bool bReceive, LPSTR lpszName);
		const LPSTR GetPacketName(byte bPacket, bool bReceive);

		bool		ShowPacket(byte bPacket, bool bReceive);
		bool		HidePacket(byte bPacket, bool bReceive);
		void		ShowAll(bool bFlag);
		void		ShowPacketList(void);
		void		ClearPacketList(void);

		void		OnGamePacketReceive(byte* pbPacket, size_t nLen);
		void		OnGamePacketSend(byte* pbPacket, size_t nLen);
};