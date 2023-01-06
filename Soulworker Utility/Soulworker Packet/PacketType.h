#pragma once

#include ".\Soulworker Packet\SWPacket.h"
/* 0x01 */
#include ".\Soulworker Packet\SWPacketHeartbeat.h"

/* 0x03 */
#include ".\Soulworker Packet\SWPacketStatChange.h"
#include ".\Soulworker Packet\SWPacketDead.h"
#include ".\Soulworker Packet\SWPacketCharacterUpdateSpecialOptionList.h"

/* 0x04 */
#include ".\Soulworker Packet\SWPacketWorldChange.h"
#include ".\Soulworker Packet\SWPacketMazeStart.h"
#include ".\Soulworker Packet\SWPacketSpawnedCharInfo.h"
#include ".\Soulworker Packet\SWPacketInInfoMonster.h"
#include ".\Soulworker Packet\SWPacketOtherInfosMonster.h"

/* 0x05 */
#include ".\Soulworker Packet\SWPacketStartMove.h"
// stopMove
// jump
// cancelwithmove
#include ".\Soulworker Packet\SWPacketEnterAnimation.h"

/* 0x06 */
// DODGE
#include ".\Soulworker Packet\SWPacketUseSkill.h"
#include ".\Soulworker Packet\SWPacketOtherUseSkill.h"
#include ".\Soulworker Packet\SWPacketDamage.h"
#include ".\Soulworker Packet\SWPacketBuffIn.h"
#include ".\Soulworker Packet\SWPacketBuffOut.h"
// projectile
#include ".\Soulworker Packet\SWPacketAkasic.h"
#include ".\Soulworker Packet\SWPacketCooldown.h"


/* 0x07 */
#include ".\Soulworker Packet\SWPacketChat.h"
// chat_whisper

/* 0x11 Maze */
#include ".\Soulworker Packet\SWPacketMazeEnd.h"
#include ".\Soulworker Packet\SWPacketMazeUpdateState.h"

/* 0x12 */
#include ".\Soulworker Packet\SWPacketParty.h"
#include ".\Soulworker Packet\SWPacketPartyListInfo.h"

/* 0x17 Monster */
#include ".\Soulworker Packet\SWPacketMonsterStatUpdate.h"
#include ".\Soulworker Packet\SWPacketMonsterKilled.h"
#include ".\Soulworker Packet\SWPacketAggroChanged.h"

/* 0x23 Gesture */
#include ".\Soulworker Packet\SWPacketGestureUsed.h"

/* 0x2e */
#include ".\Soulworker Packet\SWPacketBigParty.h"

// SWSPacket
#include ".\Soulworker Packet\SWSPacket.h"
#include ".\Soulworker Packet\SWSPacketMySkillUsed.h"
#include ".\Soulworker Packet\SWSPacketMyDodgeUsed.h"
#include ".\Soulworker Packet\SWSPacketHeartbeat.h"


// opcode0106: 01=mainCMD, 06=subCMD
enum RecvOPcode {
	HEARTBEAT = 0x0106,

	/* 0x03 */
	STATCHANGE = 0x0334,
	DEAD = 0x0341,
	CHARACTER_UPDATE_SPECIAL_OPTION_LIST = 0x0347,

	/* 0x04 */
	WORLDCHANGE = 0x0402,
	MAZESTART = 0x0408,
	SPAWNED_CHARINFO = 0x0411,
	IN_INFO_MONSTER = 0x0415,	
	OTHER_INFOS_MONSTER = 0x0423,

	/* 0x05 */
	STARTMOVE = 0x0502,
	STOPMOVE = 0x0504,
	JUMP = 0x0506,
	CANCEL_WITHMOVE = 0x0511,
	ENTER_ANIMATION = 0x050d,
 
	/* 0x06 */
	DODGE = 0x0607,
	USESKILL = 0x0609,
	OTHER_USESKILL = 0x0610,
	DAMAGE = 0x0613,
	BUFFIN = 0x0614,
	BUFFOUT = 0x0615,
	PROJECTILE = 0x0632,
	AKASIC = 0x067b,
	COOLDOWN = 0x067c,

	/* 0x07 */
	CHAT = 0x0701,
	CHAT_WHISPER = 0x0702,

	/* 0x11 Maze */
	MAZEEND = 0x1175,
	MAZE_UPDATE_STATE = 0x1121,

	/* 0x12 Party */
	PARTY = 0x1209,
	PARTY_LIST_INFO = 0x1236,

	/* 0x17 Monster */
	MONSTER_STAT_UPDATE = 0x1712,
	MONSTER_KILLED = 0x1711,
	AGGRO_CHANGED = 0x1722,

	/* 0x23 */
	GESTURE_USED = 0x2301,

	/* 0x2e */
	BIG_PARTY = 0x2e09,
};

enum class SendOPCode
{
	/* 0x01 */
	HEARTBEAT = 0x0106,

	/* 0x06 */
	DODGE_USE = 0x0606,
	SKILL_USE = 0x0608
};