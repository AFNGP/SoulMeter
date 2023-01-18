#pragma once

#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"

#pragma pack(push, 1)

#pragma pack(pop)

class SWPacketSquad : public SWPacket {
protected:
	SWPacketSquad() {}

public:
	SWPacketSquad(SWHEADER* swheader, BYTE* data);
	~SWPacketSquad() {}

	VOID Do();
	VOID Log();
	VOID Debug();
};