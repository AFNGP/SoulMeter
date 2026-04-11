# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SoulMeter is a Windows DPS (Damage Per Second) meter for the MMORPG Soulworker. It captures and decrypts game network packets using WinDivert (kernel-level packet interception) to display real-time damage statistics via an ImGui/DirectX 11 overlay.

Multi-branch repository: `main` = JP server, `KRSW` = KR server, `GLBSW` = Global server.

## Build

- **IDE:** Visual Studio 2022 (v143 toolset), Windows 10 SDK
- **Solution:** `Soulworker Utility.sln`
- **Primary config:** Release|x64 → outputs `SoulMeter <version>.exe`
- **Version:** defined in `/VERSION` and `SWConfig.h` (`APP_VERSION`)
- No CLI build commands — open the solution in Visual Studio and build from there.

Debug builds use a console subsystem and expose conditional logging. Some defines are set in project settings; others (`DEBUG_CAPTURE_*`) are toggled directly in `Packet Capture/PacketCapture.h` under `#ifdef _DEBUG`:
- `DEBUG_CAPTURE_ALL`, `DEBUG_CAPTURE_IP`, `DEBUG_CAPTURE_TCP`, `DEBUG_CAPTURE_DATA`, `DEBUG_CAPTURE_SORT`, `DEBUG_CAPTURE_QUEUE`
- `DEBUG_DAMAGEMETER_OWNER_ID`, `DEBUG_DAMAGEMETER_DB`, `DEBUG_DAMAGEMETER_WORLD`
- `DEBUG_RECV_CREATEPACKET`, `DEBUG_RECV_DISPLAYPKT`

No automated test framework exists.

## Architecture

### Data Flow

```
WinDivert (kernel) → PacketCapture → PacketParser → SWPacketMaker (decrypt)
                                                           ↓
                                              SWPacket* handlers (80+ types)
                                                           ↓
                                    DamageMeter / CombatMeter / BuffMeter
                                                           ↓
                                              ImGui UI (PlayerTable, PlotWindow, etc.)
```

Packets are intercepted on TCP port 10200. `SWPacketMaker` decrypts server→client packets; `SWSPacketMaker` handles client→server. Both use a hardcoded 256-byte key table. Decrypted packets are routed by type to individual `SWPacket*.cpp` / `SWSPacket*.cpp` handlers.

Language is auto-detected at startup from the system code page (932=JP, 949=KR, 936/950=ZH-TW, default=EN).

### Key Design Patterns

- **Singleton** (`Util/Singleton.h`) — all major subsystems are global singletons: `PacketCapture`, `DamageMeter`, `UIWindow`, `CombatMeter`, `BuffMeter`, `Language`
- **Memory Pool** — `SWDamagePlayer` uses `MemoryPool` for allocation performance
- **State Machine** — `Timer` has run/suspend/stop states; `CombatMeter` tracks combat phases
- **Multithreading** — packet parsing runs on separate threads; `CriticalSection`/`MultiThreadSync` protect shared state

### Major Subsystems

| Directory            | Purpose                                                                    |
| -------------------- | -------------------------------------------------------------------------- |
| `Packet Capture/`    | WinDivert wrapper; raw TCP capture and initial routing                     |
| `Soulworker Packet/` | Packet decryption (`SWPacketMaker`) and 80+ typed packet handlers          |
| `Damage Meter/`      | Core DPS tracking — players, monsters, skills, history, SQLite persistence |
| `Combat Meter/`      | Combat state tracking                                                      |
| `Buff Meter/`        | Active buff tracking per player                                            |
| `UI/`                | ImGui + DirectX 11 rendering; `Option.cpp` (~35KB) handles all settings    |
| `Language/`          | JSON-based localization (EN, JP, KR, zh_TW)                                |
| `Util/`              | Singleton, MemoryPool, CriticalSection, logging, crash dump                |
| `SWCrypt/`           | Interface to external crypto DLL                                           |
| `Third Party/`       | ImGui, SQLite, WinDivert, OpenSSL, FlatBuffers, tinyxml2, cpp-httplib      |

### Key Files

- `SWConfig.h` — app-wide constants: `APP_VERSION`, `SWMAGIC`, protocol flags
- `pch.h` — precompiled header; all major includes live here
- `main.cpp` — initialization order for all subsystems
- `Soulworker Packet/PacketType.h` — enumeration of all packet types
- `Soulworker Packet/PacketInfo.h` — stat types and protocol definitions
- `Damage Meter/Damage Meter.h/.cpp` — central meter singleton
- `SWDB.db` — SQLite database containing skill, monster, and map data (~2.4MB)

### Runtime Files (not committed)

- `option.xml` — user settings (ImGui window state, preferences)
- `imgui.ini` — ImGui layout
- Required DLLs alongside the exe: `WinDivert.dll`, `WinDivert64.sys`, `SWCrypt.dll`, `libssl-1_1-x64.dll`, `libcrypto-1_1-x64.dll`, `sqlite3.dll`, `pthreadVC2.dll`

## Adding/Updating Packet Handlers

1. Add the packet type to `Soulworker Packet/PacketType.h`
2. Create `SWPacket<Name>.h/.cpp` (server→client recv) or `SWSPacket<Name>.h/.cpp` (client→server send) in `Soulworker Packet/`
3. Register it in `Packet Capture/PacketParser.cpp`
4. Update `SWDB.db` if new skill/monster data is needed

## Updating the Database

`SWDB.db` is the SQLite database for skill/monster/map lookups. Update it directly with a SQLite editor when game content changes. The `Damage Meter/MySQLite.h/.cpp` wrapper handles all queries.

## Versioning

Update version in two places when releasing:
1. `/VERSION` file
2. `SWConfig.h` → `APP_VERSION`

The output executable name also reflects the version (configured in the `.vcxproj`).
