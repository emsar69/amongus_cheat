#pragma once
#include <memory.h>
#include <cstdint>
#include <string>
#include <vector>

enum class GameStates : int32_t {
	NotJoined = 0x00000000,
	Joined = 0x00000001,
	Started = 0x00000002,
	Ended = 0x00000003,
};

enum class RoleTypes : uint16_t {
	Crewmate = 0x0000,
    Impostor = 0x0001,
    Scientist = 0x0002,
    Engineer = 0x0003,
    GuardianAngel = 0x0004,
    Shapeshifter = 0x0005,
    CrewmateGhost = 0x0006,
    ImpostorGhost = 0x0007,
    Noisemaker = 0x0008,
    Phantom = 0x0009,
    Tracker = 0x000a
};

class AmongUsClient
{
public:
	char pad_0000[136]; //0x0000
	GameStates GameState; //0x0088
};

class PlayerPhysics
{
public:
	char pad_0000[52]; //0x0000
	float PlayerSpeed; //0x0034
	float GhostSpeed; //0x0038
};

class Role
{
public:
	char pad_0000[12]; //0x0000
	RoleTypes roleType; //0x000C
	char pad_0010[44]; //0x0010
	bool TaskCountTowardProgress; //0x003C
	bool CanUseKillButton; //0x003D
	bool CanBeKilled; //0x003E
	bool CanVent; //0x003F
	int32_t AffectedByLightAffectors; //0x0040
	int32_t MaxCount; //0x0044
	int32_t TeamType; //0x0048
}; //Size: 0x0840

class CachedData
{
public:
	char pad_0000[72]; //0x0000
	Role* Role; //0x0048
	char pad_004C[4]; //0x004C
	bool IsDead; //0x0050
}; //Size: 0x0440

class Player
{
public:
	char pad_0000[88]; //0x0000
	CachedData* cacheddata; //0x0058
	char pad_005C[36]; //0x005C
	int KillTimer; //0x0080
	char pad_0084[16]; //0x0084
	PlayerPhysics* Physics; //0x0094
}; //Size: 0x0840

namespace game {

	inline Memory memory;

	void init();
	const char* getRoleName(RoleTypes roleType);
	uintptr_t getAssembly();
	Player* getLocalPlayer();
	std::vector<Player*> getPlayerList();
	uintptr_t* getPlayerInfo(Player* player);
	std::string getPlayerName(Player* player);
	AmongUsClient* getAmongUsClient();
	uintptr_t getRoleManagerAddr();
	uintptr_t* getHudManagerAddr();
	void setRole(Player* target, RoleTypes role);
}