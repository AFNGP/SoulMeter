#include "pch.h"
#include ".\Soulworker Packet\SWPacket.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\SWPacketSquad.h"

SWPacketSquad::SWPacketSquad(SWHEADER* swheader, BYTE* data) : SWPacket(swheader, data) {

}

VOID SWPacketSquad::Do() {

	size_t offset = 32; //unk01

	UINT16 squadLeaderNickSize = 0;
	memcpy(&squadLeaderNickSize, _data + offset, 2);

	offset += 2; //SQUADLEADERNICKSIZE

	offset += squadLeaderNickSize; //SQUADLEADERNICK

	offset += 4; //unk02

	UINT8 squadPlayerCount = 0;
	memcpy(&squadPlayerCount, _data + offset, 1);

	offset += 1; //SQUADPLAYERCOUNT

	offset += 11; //unk03

	for (int i = 0; i < squadPlayerCount; i++) {
		UINT32 playerId = 0;
		memcpy(&playerId, _data + offset, 4);

		offset += 4; //PLAYERID

		UINT16 playerNickSize = 0;
		memcpy(&playerNickSize, _data + offset, 2);

		offset += 2; //PLAYERNICKSIZE

		WCHAR utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), _data + offset, playerNickSize);

		CHAR utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
			// Log::WriteLogA("Error in SWPacketSquad : UTF16toUTF8 FAILED");
			return;
		}

		offset += playerNickSize; //PLAYERNICK

		offset += 1; //D_unk01
		UINT8 playerJob = 0;
		memcpy(&playerJob, _data + offset, 1);

		DAMAGEMETER.InsertPlayerMetadata(playerId, utf8, playerJob);

		offset += 1; //PLAYERJOB

		offset += 50; //D_unk02
	}
}

VOID SWPacketSquad::Log() {

}

VOID SWPacketSquad::Debug() {
	
	Log::WriteLogA("Skipping past unk01 (32 bytes)");

	size_t offset = 32; //unk01
	Log::WriteLogA("At offset %zu", offset);

	UINT16 squadLeaderNickSize = 0;
	memcpy(&squadLeaderNickSize, _data + offset, 2);
	Log::WriteLogA("Found leader nick size of %16x (2 bytes)", squadLeaderNickSize);

	offset += 2; //SQUADLEADERNICKSIZE

	Log::WriteLogA("At offset %zu", offset);
	WCHAR utf16[MAX_NAME_LEN] = { 0 };
	memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), _data + offset, squadLeaderNickSize);
	CHAR utf8[MAX_NAME_LEN] = { 0 };
	if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
		Log::WriteLogA("Failed to convert squad leader name");
	}
	else {
		Log::WriteLogA("Squad leader name is %s (%16x bytes)", utf8, squadLeaderNickSize);
	}

	offset += squadLeaderNickSize; //SQUADLEADERNICK

	Log::WriteLogA("At offset %zu", offset);
	Log::WriteLogA("Skipping past unk02 (4 bytes)");
	offset += 4; //unk02

	Log::WriteLogA("At offset %zu", offset);
	UINT8 squadPlayerCount = 0;
	memcpy(&squadPlayerCount, _data + offset, 1);
	Log::WriteLogA("Squad player count is %8x (1 byte)", squadPlayerCount);

	offset += 1; //SQUADPLAYERCOUNT

	Log::WriteLogA("At offset %zu skipping past unk03 (10 bytes)", offset);

	offset += 11; //unk03

	for (int i = 0; i < squadPlayerCount; i++) {
		UINT32 playerId = 0;
		memcpy(&playerId, _data + offset, 4);
		Log::WriteLogA("Found player ID %32x (4 bytes)", playerId);

		offset += 4; //PLAYERID
		Log::WriteLogA("At offset %zu", offset);

		UINT16 playerNickSize = 0;
		memcpy(&playerNickSize, _data + offset, 2);
		Log::WriteLogA("Found player nick size of %16x (2 bytes)", playerNickSize);

		offset += 2; //PLAYERNICKSIZE
		Log::WriteLogA("At offset %zu", offset);

		WCHAR utf16[MAX_NAME_LEN] = { 0 };
		memcpy_s(utf16, MAX_NAME_LEN * sizeof(WCHAR), _data + offset, playerNickSize);

		CHAR utf8[MAX_NAME_LEN] = { 0 };
		if (!UTF16toUTF8(utf16, utf8, MAX_NAME_LEN)) {
			Log::WriteLogA("Failed to convert player name");
		}
		else {
			Log::WriteLogA("Player name is %s (%16x bytes)", utf8, playerNickSize);
		}

		offset += playerNickSize; //PLAYERNICK

		Log::WriteLogA("At offset %zu skipping past D_unk01 (1 byte)", offset);

		offset += 1; //D_unk01

		Log::WriteLogA("At offset %zu", offset);
		UINT8 playerJob = 0;
		memcpy(&playerJob, _data + offset, 1);
		Log::WriteLogA("Player job is %8x (1 byte)", playerJob);

		offset += 1; //PLAYERJOB

		Log::WriteLogA("Skipping past D_unk02 (50 bytes)");
		
		offset += 50; //D_unk02
	}

}