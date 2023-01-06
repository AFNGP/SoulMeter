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
			opcode = "heartbeat";
			swpacket = new SWPacketHeartbeat(swheader, data, packet->_ts);
			break;

			/*0x03*/
		case RecvOPcode::STATCHANGE: //0430
			opcode = "statchange";
			swpacket = new SWPacketStatChange(swheader, data);
			break;
		case RecvOPcode::DEAD:
			opcode = "dead";
			swpacket = new SWPacketDead(swheader, data);
			break;
		case RecvOPcode::CHARACTER_UPDATE_SPECIAL_OPTION_LIST:
			opcode = "character update special option list";
			swpacket = new SWPacketCharacterUpdateSpecialOptionList(swheader, data);
			break;

			/*0x04*/
			// eSUB_CMD_WORLD_ENTER_RES
		case RecvOPcode::WORLDCHANGE: //0430
			opcode = "world change";
			swpacket = new SWPacketWorldChange(swheader, data);
			break;
		case RecvOPcode::MAZESTART:
			opcode = "maze start";
			swpacket = new SWPacketMazeStart(swheader, data);
			break;
		case RecvOPcode::SPAWNED_CHARINFO:
			opcode = "spawned char info";
			//swpacket = new SWPacketSpawnedCharInfo(swheader, data);
			break;
		case RecvOPcode::IN_INFO_MONSTER: //0605
			opcode = "in info monster";
			swpacket = new SWPacketInInfoMonster(swheader, data);
			break;
		case RecvOPcode::OTHER_INFOS_MONSTER:
			opcode = "other infos monster";
			swpacket = new SWPacketOtherInfosMonster(swheader, data);
			break;

			/*0x05*/
		case RecvOPcode::STARTMOVE:
			opcode = "startmove";
			//swpacket = new SWPacketStartMove(swheader, data);
			break;
		case RecvOPcode::STOPMOVE:
			opcode = "stopmove";
			break;
		case RecvOPcode::JUMP:
			opcode = "jump";
			break;
		case RecvOPcode::CANCEL_WITHMOVE:
			opcode = "cancel withmove";
			break;
		case RecvOPcode::ENTER_ANIMATION:
			opcode = "enter animation";
			swpacket = new SWPacketEnterAnimation(swheader, data);
			break;

			/*0x06 Combat*/
		//case RecvOPcode::EVADE:
		//	break;
		case RecvOPcode::USESKILL:
			opcode = "useskill";
			//swpacket = new SWPacketUseSkill(swheader, data);
			break;
		case RecvOPcode::OTHER_USESKILL: //0430
			opcode = "other_useskill";
			swpacket = new SWPacketOtherUseSkill(swheader, data);
			break;
		case RecvOPcode::DAMAGE: //0430
			opcode = "damage";
			swpacket = new SWPacketDamage(swheader, data);
			break;
		case RecvOPcode::BUFFIN: //0430
			opcode = "buffin";
			swpacket = new SWPacketBuffIn(swheader, data);
			break;
		case RecvOPcode::BUFFOUT: //0430
			opcode = "buffout";
			swpacket = new SWPacketBuffOut(swheader, data);
			break;
		case RecvOPcode::PROJECTILE:
			opcode = "projectile";
			break;
		case RecvOPcode::AKASIC:
			opcode = "akasic";
			swpacket = new SWPacketAkasic(swheader, data);
			break;
		case RecvOPcode::COOLDOWN: //0430
			opcode = "cooldown";
			swpacket = new SWPacketCooldown(swheader, data);
			break;

			/*0x07*/
		case RecvOPcode::CHAT:
			opcode = "chat";
			//swpacket = new SWPacketChat(swheader, data);
			break;

			/* 0x11 Maze*/
		case RecvOPcode::MAZEEND:
			opcode = "mazeend";
			swpacket = new SWPacketMazeEnd(swheader, data);
			break;
		case RecvOPcode::MAZE_UPDATE_STATE:
			opcode = "maze update state";
			swpacket = new SWPacketMazeUpdateState(swheader, data);
			break;

			/* 0x12 Party */
		case RecvOPcode::PARTY: //1117
			opcode = "party";
			swpacket = new SWPacketParty(swheader, data);
			break;
		case RecvOPcode::PARTY_LIST_INFO:
			opcode = "part list info";
			//swpacket = new SWPacketPartyListInfo(swheader, data);
			break;

			/* 0x17 Monster */
		case RecvOPcode::MONSTER_STAT_UPDATE:
			opcode = "monster stat update";
			swpacket = new SWPacketMonsterStatUpdate(swheader, data);
			break;
		case RecvOPcode::MONSTER_KILLED: //0430
			opcode = "monster killed";
			swpacket = new SWPacketMonsterKilled(swheader, data);
			break;
		case RecvOPcode::AGGRO_CHANGED: //0430
			opcode = "aggro changed";
			swpacket = new SWPacketAggroChanged(swheader, data);
			break;

			/* 0x23 Gesture*/
		case RecvOPcode::GESTURE_USED:
			opcode = "gesture used";
			//swpacket = new SWPacketGestureUsed(swheader, data);
			break;

			/* 0x2e Force*/
			// 8 players party
		case RecvOPcode::BIG_PARTY: //1117
			opcode = "big party";
			swpacket = new SWPacketBigParty(swheader, data);
			break;




		default:
/*#if DEBUG_RECV_DISPLAYPKT == 1
			printf("Found opcode %s\n", opcode);
			Log::WriteLogA("OP : %04x\tsize : %04x", swheader->_op, swheader->_size);
			for (int i = 0; i < swheader->_size; i++)
				Log::WriteLogNoDate(L"%02x ", data[i]);
			Log::WriteLogNoDate(L"\n\n");
#endif*/
			break;
		}

		if (swpacket != nullptr) {
#if DEBUG_RECV_CREATEPACKET == 1
			swpacket->Debug();
#endif
			// Todo
			swpacket->Do();

			delete swpacket;
		}
	}
	DAMAGEMETER.FreeLock();

	PACKETPARSER.ResizePacket(packet, TRUE);
}