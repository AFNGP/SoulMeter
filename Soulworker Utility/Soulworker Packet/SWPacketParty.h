#pragma once

#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#define SWPACKETPARTY_DUMMY 46 //1117

#pragma pack(push, 1)

typedef struct _SWPACKETPARTY_HEADER {
	BYTE _unknown01[4]; //0430new
	UINT32 _partyHostID;
	BYTE _unknown02[10];
	BYTE _partyPlayerCount;
}SWPACKETPARTY_HEADER;

typedef struct _SWPACKETPARTY_DATA {
	UINT32 _playerID;
	USHORT _nickSize;
}SWPACKETPARTY_DATA;

#pragma pack(pop)

class SWPacketParty : public SWPacket {
protected:
	SWPacketParty() {}

public:
	SWPacketParty(SWHEADER* swheader, BYTE* data);
	~SWPacketParty() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};