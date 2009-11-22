#include "Module.hpp"
#include "Sniffer.hpp"

Sniffer::Sniffer(HoNToolKit* pHoNToolKit) : m_pHoNToolKit(pHoNToolKit)
{
	
}

Sniffer::~Sniffer()
{
	for(std::map<byte, LPSTR>::iterator entry = this->m_PacketSendNames.begin(); entry != this->m_PacketSendNames.end(); entry++)
	{
		delete (*entry).second;
	}

	for(std::map<byte, LPSTR>::iterator entry = this->m_PacketRecvNames.begin(); entry != this->m_PacketRecvNames.end(); entry++)
	{
		delete (*entry).second;
	}
}

bool
Sniffer::SetPacketName(byte bPacket, bool bReceive, LPSTR lpszName)
{
	LPSTR lpszNewName = new char[strlen(lpszName) + 1];

	strcpy_s(lpszNewName, strlen(lpszName) + 1, lpszName);

	if(bReceive)
	{
		if(this->m_PacketRecvNames[bPacket])
			delete this->m_PacketRecvNames[bPacket];

		this->m_PacketRecvNames[bPacket] = lpszNewName;
	}
	else
	{
		if(this->m_PacketSendNames[bPacket])
			delete this->m_PacketSendNames[bPacket];

		this->m_PacketSendNames[bPacket] = lpszNewName;
	}

	return true;
}

const LPSTR
Sniffer::GetPacketName(byte bPacket, bool bReceive)
{
	return bReceive ? this->m_PacketRecvNames[bPacket] : this->m_PacketSendNames[bPacket];
}

bool
Sniffer::ShowPacket(byte bPacket, bool bReceive)
{
	if(bReceive)
	{
		this->m_PacketRecvList[bPacket] = true;
	}
	else
	{
		this->m_PacketSendList[bPacket] = true;
	}

	return true;
}

bool
Sniffer::HidePacket(byte bPacket, bool bReceive)
{
	if(bReceive)
	{
		this->m_PacketRecvList[bPacket] = false;
	}
	else
	{
		this->m_PacketSendList[bPacket] = false;
	}

	return true;
}

void
Sniffer::ShowPacketList(void)
{
	char szSend[1024] = "^ySniffer^w: ^yRecv^w: ";
	char szRecv[1024] = "^ySniffer^w: ^ySend^w: ";

	this->m_pHoNToolKit->PrintText("^ySniffer^w: Packets that are currently visible");

	for(std::map<byte, bool>::iterator entry = this->m_PacketSendList.begin(); entry != this->m_PacketSendList.end(); entry++)
	{
		if(entry->second)
		{
			char szBuffer[64] = "";
			sprintf_s(szBuffer, sizeof(szBuffer), "%.2X ", entry->first);

			strcat_s(szSend, sizeof(szSend), szBuffer);
		}
	}

	for(std::map<byte, bool>::iterator entry = this->m_PacketRecvList.begin(); entry != this->m_PacketRecvList.end(); entry++)
	{
		if(entry->second)
		{
			char szBuffer[64] = "";
			sprintf_s(szBuffer, sizeof(szBuffer), "%.2X ", entry->first);

			strcat_s(szRecv, sizeof(szRecv), szBuffer);
		}
	}

	if(strlen(szSend) > 23)
		this->m_pHoNToolKit->PrintText(szSend);

	if(strlen(szRecv) > 23)
		this->m_pHoNToolKit->PrintText(szRecv);
}

void
Sniffer::ClearPacketList(void)
{
	for(std::map<byte, bool>::iterator entry = this->m_PacketSendList.begin(); entry != this->m_PacketSendList.end(); entry++)
	{
		entry->second = false;
	}

	for(std::map<byte, bool>::iterator entry = this->m_PacketRecvList.begin(); entry != this->m_PacketRecvList.end(); entry++)
	{
		entry->second = false;
	}
}

void
Sniffer::OnGamePacketReceive(byte *pbPacket, size_t nLen)
{
	if(this->m_PacketRecvList[pbPacket[0]])
	{
		char szPacket[1024] = "";

		for(unsigned int i = 0; i < nLen; i++)
		{
			char szBuffer[64] = "";
			
			sprintf_s(szBuffer, sizeof(szBuffer), "%.2x ", pbPacket[i]);
			strcat_s(szPacket, sizeof(szPacket), szBuffer);
		}

		if(this->GetPacketName(pbPacket[0], true))
			this->m_pHoNToolKit->PrintText("^yRecv^w: %s %s", this->GetPacketName(pbPacket[0], true), szPacket);
		else
			this->m_pHoNToolKit->PrintText("^yRecv^w: %s", szPacket);
	}
}

void
Sniffer::OnGamePacketSend(byte *pbPacket, size_t nLen)
{
	if(this->m_PacketSendList[pbPacket[0]])
	{
		char szPacket[1024] = "";

		for(unsigned int i = 0; i < nLen; i++)
		{
			char szBuffer[64] = "";
			
			sprintf_s(szBuffer, sizeof(szBuffer), "%.2x ", pbPacket[i]);
			strcat_s(szPacket, sizeof(szPacket), szBuffer);
		}

		if(this->GetPacketName(pbPacket[0], false))
			this->m_pHoNToolKit->PrintText("^ySend^w: %s %s", this->GetPacketName(pbPacket[0], false), szPacket);
		else
			this->m_pHoNToolKit->PrintText("^ySend^w: %s", szPacket);
	}
}