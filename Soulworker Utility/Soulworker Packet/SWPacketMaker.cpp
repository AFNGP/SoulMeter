#include "pch.h"
#include ".\Soulworker Packet\PacketType.h"
#include ".\Soulworker Packet\SWPacketMaker.h"
#include ".\Packet Capture\PacketParser.h"
#include ".\UI\PlayerTable.h"
#include ".\SWCrypt\SWCryptDLL.h"
#include "SWConfig.h"

SWHEADER* SWPacketMaker::GetSWHeader(IPv4Packet* packet) {

	if (packet == nullptr || packet->_datalength < sizeof(SWHEADER)) {
		return nullptr;
	}

	SWHEADER* swheader = (SWHEADER*)(packet->_data);

	if (swheader->_magic != SWMAGIC || (swheader->_const_value01 != SWCONSTVALUE_RECV && swheader->_const_value01 != SWCONSTVALUE_SEND)) {
		return nullptr;
	}

	return swheader;
}

BYTE* SWPacketMaker::GetSWData(IPv4Packet* packet) {

	return (BYTE*)(packet->_data);
}

VOID SWPacketMaker::Decrypt(BYTE* data, const UINT size, const UINT start, const INT32 keyIndex) {

	if (data == nullptr || size < 0 || start < 0)
		return;

#if USE_XOR == 1
	UINT32 _size = size;
	_size -= sizeof(SWHEADER) + 3;
	for (UINT i = 0; i < _size; i++) {
		data[i + start] ^= _keyTable[16 * (keyIndex % 16) + (i & 0xF)];
	}
#else
	SWCRYPT.SWDecrypt(data + start, size - start, keyIndex, TRUE);
#endif
}

VOID SWPacketMaker::CreateSWPacket(IPv4Packet* packet) {

	SWHEADER* swheader = GetSWHeader(packet);
	BYTE* data = GetSWData(packet);

	if (swheader == nullptr || data == nullptr)
	{
		return;
	}

#if DEBUG_RECV_DISPLAY_ALL_PKT == 1
	Log::WriteLogA("OP : %04x\tsize : %04x", swheader->_op, swheader->_size);
	for (int i = 0; i < swheader->_size; i++)
		Log::WriteLogNoDate(L"%02x ", data[i]);
	Log::WriteLogNoDate(L"\n\n");
#endif

	Decrypt(data, swheader->_size, sizeof(SWHEADER) - 3, swheader->key);


	SWPacket* swpacket = nullptr;
	char* opcode = "NO_OPCODE_FOUND";
	DAMAGEMETER.GetLock();
	{
		switch (_byteswap_ushort(swheader->_op)) {
			/* 0x01*/
		case RecvOPcode::HEARTBEAT: 
			opcode = "HEARTBEAT";
			swpacket = new SWPacketHeartbeat(swheader, data, packet->_ts);
			break;

			/*0x03*/
		case RecvOPcode::STATCHANGE: //0430
			opcode = "STATCHANGE";
			swpacket = new SWPacketStatChange(swheader, data);
			break;
		case RecvOPcode::DEAD:
			opcode = "DEAD";
			swpacket = new SWPacketDead(swheader, data);
			break;
		case RecvOPcode::CHARACTER_UPDATE_SPECIAL_OPTION_LIST:
			opcode = "character update special option list";
			swpacket = new SWPacketCharacterUpdateSpecialOptionList(swheader, data);
			break;

			/*0x04*/
			// eSUB_CMD_WORLD_ENTER_RES
		case RecvOPcode::WORLDCHANGE: //0430
			opcode = "WORLDCHANGE";
			swpacket = new SWPacketWorldChange(swheader, data);
			break;
		case RecvOPcode::MAZESTART:
			opcode = "MAZESTART";
			swpacket = new SWPacketMazeStart(swheader, data);
			break;
		case RecvOPcode::SPAWNED_CHARINFO:
			opcode = "SPAWNED_CHARINFO";
			//swpacket = new SWPacketSpawnedCharInfo(swheader, data);
			break;
		case RecvOPcode::IN_INFO_MONSTER: //0605
			opcode = "IN_INFO_MONSTER";
			swpacket = new SWPacketInInfoMonster(swheader, data);
			break;
		case RecvOPcode::OTHER_INFOS_MONSTER:
			opcode = "OTHER_INFOS_MONSTER";
			swpacket = new SWPacketOtherInfosMonster(swheader, data);
			break;

			/*0x05*/
		case RecvOPcode::STARTMOVE:
			opcode = "STARTMOVE";
			//swpacket = new SWPacketStartMove(swheader, data);
			break;
		case RecvOPcode::STOPMOVE:
			opcode = "STOPMOVE";
			break;
		case RecvOPcode::JUMP:
			opcode = "JUMP";
			break;
		case RecvOPcode::CANCEL_WITHMOVE:
			opcode = "CANCEL_WITHMOVE";
			break;
		case RecvOPcode::ENTER_ANIMATION:
			opcode = "ENTER_ANIMATION";
			swpacket = new SWPacketEnterAnimation(swheader, data);
			break;

			/*0x06 Combat*/
		//case RecvOPcode::EVADE:
		//	break;
		case RecvOPcode::USESKILL:
			opcode = "USESKIL:";
			//swpacket = new SWPacketUseSkill(swheader, data);
			break;
		case RecvOPcode::OTHER_USESKILL: //0430
			opcode = "OTHER_USESKILL";
			swpacket = new SWPacketOtherUseSkill(swheader, data);
			break;
		case RecvOPcode::DAMAGE: //0430
			opcode = "DAMAGE";
			swpacket = new SWPacketDamage(swheader, data);
			break;
		case RecvOPcode::BUFFIN: //0430
			opcode = "BUFFIN";
			swpacket = new SWPacketBuffIn(swheader, data);
			break;
		case RecvOPcode::BUFFOUT: //0430
			opcode = "BUFFOUT";
			swpacket = new SWPacketBuffOut(swheader, data);
			break;
		case RecvOPcode::PROJECTILE:
			opcode = "PROJECTILE";
			break;
		case RecvOPcode::AKASIC:
			opcode = "AKASIC";
			swpacket = new SWPacketAkasic(swheader, data);
			break;
		case RecvOPcode::COOLDOWN: //0430
			opcode = "COOLDOWN";
			swpacket = new SWPacketCooldown(swheader, data);
			break;

			/*0x07*/
		case RecvOPcode::CHAT:
			opcode = "CHAT";
			//swpacket = new SWPacketChat(swheader, data);
			break;

			/* 0x11 Maze*/
		case RecvOPcode::MAZEEND:
			opcode = "MAZEEND";
			swpacket = new SWPacketMazeEnd(swheader, data);
			break;
		case RecvOPcode::MAZE_UPDATE_STATE:
			opcode = "maze MAZE_UPDATE_STATE state";
			swpacket = new SWPacketMazeUpdateState(swheader, data);
			break;

			/* 0x12 Party */
		case RecvOPcode::PARTY: //1117
			opcode = "PARTY";
			swpacket = new SWPacketParty(swheader, data);
			break;
		case RecvOPcode::PARTY_LIST_INFO:
			opcode = "PARTY_LIST_INFO";
			//swpacket = new SWPacketPartyListInfo(swheader, data);
			break;

			/* 0x17 Monster */
		case RecvOPcode::MONSTER_STAT_UPDATE:
			opcode = "MONSTER_STAT_UPDATE";
			swpacket = new SWPacketMonsterStatUpdate(swheader, data);
			break;
		case RecvOPcode::MONSTER_KILLED: //0430
			opcode = "MONSTER_KILLED";
			swpacket = new SWPacketMonsterKilled(swheader, data);
			break;
		case RecvOPcode::AGGRO_CHANGED: //0430
			opcode = "AGGRO_CHANGED";
			swpacket = new SWPacketAggroChanged(swheader, data);
			break;

			/* 0x23 Gesture*/
		case RecvOPcode::GESTURE_USED:
			opcode = "GESTURE_USED";
			//swpacket = new SWPacketGestureUsed(swheader, data);
			break;

			/* 0x2e Force*/
			// 8 players party
		case RecvOPcode::BIG_PARTY: //1117
			opcode = "BIG_PARTY";
			swpacket = new SWPacketBigParty(swheader, data);
			break;
		
		
		case RecvOPcode::SQUAD:
			opcode = "SQUAD";
			swpacket = new SWPacketSquad(swheader, data);
			break;
		

		default:
			break;
		}

		



#if DEBUG_RECV_DISPLAYPKT == 1
		Log::WriteLogA("Found opcode %s\n", opcode);
		Log::WriteLogA("OP : %04x\tsize : %04x", swheader->_op, swheader->_size);
		for (int i = 0; i < swheader->_size; i++)
			Log::WriteLogNoDate(L"%02x ", data[i]);
		Log::WriteLogNoDate(L"\n\n");
#endif

		if (swpacket != nullptr) {
#if DEBUG_RECV_CREATEPACKET == 1
			swpacket->Debug();
#endif
			if (opcode == "SQUAD") {
				swpacket->Debug();
			}
			// Todo
			swpacket->Do();

			delete swpacket;
		}
	}
	DAMAGEMETER.FreeLock();

	PACKETPARSER.ResizePacket(packet, TRUE);
}