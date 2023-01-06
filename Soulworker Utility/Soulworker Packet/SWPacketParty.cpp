#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketParty.h"

SWPacketParty::SWPacketParty(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketParty::Do() {

	SWPACKETPARTY_HEADER* party_header = (SWPACKETPARTY_HEADER*)(_data + sizeof(SWHEADER));

	BYTE* p_data = _data;
	p_data += sizeof(SWHEADER) + sizeof(SWPACKETPARTY_HEADER);

	for (int i = 0; i < party_header->_partyPlayerCount; i++) {
		SWPACKETPARTY_DATA* party_data = (SWPACKETPARTY_DATA*)p_data;
		BYTE job = *(p_data + sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + 1);

		WCHAR utf16[MAX_NAME_LEN] = {0};
		memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data + sizeof(SWPACKETPARTY_DATA), party_data->_nickSize);

		CHAR utf8[MAX_NAME_LEN] = {0};
		if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
			//Log::WriteLog(const_cast<LPTSTR>(_T("Error in SWPacketParty : UTF16toUTF8 FAILED")));
			return;
		}

		DAMAGEMETER.InsertPlayerMetadata(party_data->_playerID, utf8, job);

		p_data += sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + SWPACKETPARTY_DUMMY;
	}
}

VOID SWPacketParty::Log() {

}

VOID SWPacketParty::Debug() {

	SWPACKETPARTY_HEADER* party_header = (SWPACKETPARTY_HEADER*)(_data + sizeof(SWHEADER));

	BYTE* p_data = _data;
	p_data += sizeof(SWHEADER) + sizeof(SWPACKETPARTY_HEADER);

	for (int i = 0; i < party_header->_partyPlayerCount; i++) {
		SWPACKETPARTY_DATA* party_data = (SWPACKETPARTY_DATA*)p_data;
		BYTE job = *(p_data + sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + 1);

		WCHAR utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), p_data + sizeof(SWPACKETPARTY_DATA), party_data->_nickSize);

		CHAR utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
			return;
		}

		Log::WriteLog(const_cast<LPTSTR>(_T("[DEBUG] [PARTY DATA] [ID %08x] [NAME = %s] [NICKMEMSIZE = %d] [JOB = %d]")), party_data->_playerID, utf16, party_data->_nickSize, job);

		p_data += sizeof(SWPACKETPARTY_DATA) + party_data->_nickSize + SWPACKETPARTY_DUMMY;
	}
}