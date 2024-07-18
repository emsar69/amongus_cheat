#include "game.h"
#include "logs.h"
#include <string>

std::string WStringToString(const std::wstring& wstr)
{
	std::string str;
	size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

void game::init()
{
	game::memory = Memory();
}

const char* game::getRoleName(RoleTypes roleType) {
	switch (roleType)
	{
	case RoleTypes::Crewmate:
		return "Crewmate";
	case RoleTypes::Impostor:
		return "Impostor";
	case RoleTypes::Scientist:
		return "Scientist";
	case RoleTypes::Engineer:
		return "Engineer";
	case RoleTypes::GuardianAngel:
		return "GuardianAngel";
	case RoleTypes::Shapeshifter:
		return "ShapeShifter";
	case RoleTypes::CrewmateGhost:
		return "CrewmateGhost";
	case RoleTypes::ImpostorGhost:
		return "ImpostorGhost";
	case RoleTypes::Noisemaker:
		return "NoiseMaker";
	case RoleTypes::Phantom:
		return "Phantom";
	case RoleTypes::Tracker:
		return "Tracker";
	default:
		return "Unknown Role";
	}
}

uintptr_t game::getAssembly() {
	return (uintptr_t)GetModuleHandleA("GameAssembly.dll");
}

Player* game::getLocalPlayer() {
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t addr = game::memory.ReadByPointers(GameAssembly + 0x022A55AC, {0x5C, 0x0, 0x0});
	return reinterpret_cast<Player*>(addr);
}

std::vector<Player*> game::getPlayerList() {
	uintptr_t GameAssembly = game::getAssembly();
	std::vector<Player*> result;
	for (int a = 0; a < 15; a ++) {
		uintptr_t i = 0x4 * (uintptr_t)a + 0x10;
		uintptr_t* addr = (uintptr_t*)(game::memory.ReadByPointers(GameAssembly + 0x022A55AC, { 0x5C, 0x4, 0x8, i }));

		if (addr == nullptr) break;
		if (*addr == 0) break;

		Player* plr = reinterpret_cast<Player*>(*addr);

		result.push_back(plr);
	}
	return result;
}

uintptr_t* game::getPlayerInfo(Player* player) {
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t get_DataAddr = GameAssembly + 0x006E7800;
	uintptr_t* playeraddr = (uintptr_t*)player;

	typedef uintptr_t* (__cdecl* get_Data_Sign)(uintptr_t* player_control, const uintptr_t* methodInfo);
	get_Data_Sign get_Data = reinterpret_cast<get_Data_Sign>(get_DataAddr);
	
	uintptr_t* playerInfo = get_Data(playeraddr, NULL);

	return playerInfo;
}

std::string game::getPlayerName(Player* player) {
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t addr = GameAssembly + 0x0087C210;

	typedef uintptr_t* (__cdecl* getNameSign)(uintptr_t* info, const uintptr_t* methodInfo);
	getNameSign get_name = reinterpret_cast<getNameSign>(addr);

	uintptr_t* info = game::getPlayerInfo(player);
	if (info == nullptr || *info == 0) return "";
	uintptr_t* res = get_name(info, NULL);

	class str_fields {
	public:
		char pad[8];
		size_t m_stringLength;
		wchar_t m_firstChar[];
	};

	str_fields* str = reinterpret_cast<str_fields*>(res);
	if (str == nullptr) return "";
	if (str->m_firstChar == NULL) return "";
	std::wstring wstr(str->m_firstChar);
	
	std::string playerName = WStringToString(wstr);

	logger::Write(playerName);
	logger::Write(res);
	
	return playerName;
}

AmongUsClient* game::getAmongUsClient(){
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t client = game::memory.ReadByPointers(GameAssembly + 0x02292578, {0x5C, 0x0, 0x0});
	return reinterpret_cast<AmongUsClient*>(client);
}

uintptr_t game::getRoleManagerAddr() {
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t Manager = game::memory.ReadByPointers(GameAssembly + 0x0226C778, {0x5C, 0x8, 0x0});
	return Manager;
}

uintptr_t* game::getHudManagerAddr() {
	uintptr_t GameAssembly = game::getAssembly();
	uintptr_t Manager = game::memory.ReadByPointers(GameAssembly + 0x0225038C, { 0x5C, 0x0, 0x0 });
	return (uintptr_t*)Manager;
}

void game::setRole(Player* target, RoleTypes role){
	uintptr_t assembly = game::getAssembly();
	uintptr_t setRoleAddr = assembly + 0x00961F00;

	uintptr_t RoleManager = game::getRoleManagerAddr();
	int rint = (int)role;
	uintptr_t trg = reinterpret_cast<uintptr_t>(target);

	logger::Write(RoleManager);
	logger::Write((uintptr_t*)target);

	//NOT WORKING CURRENTLY

	asm volatile (
        "mov %0, %%eax\n"
        "mov %1, %%ecx\n"
        "mov %2, %%edi\n"
        "call *%3\n"
        :
        : "r"(RoleManager), "r"(rint), "r"(trg), "r"(setRoleAddr)
        : "eax","ecx","edi"
    );

	// NOT WORKING CURRENTLY

	logger::Write("Set role.");
}
