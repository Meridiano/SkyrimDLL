#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
}

class PlayerPayCrimeGoldTweak {
public:
	class PlayerPayCrimeGoldHook {
	public:
		static void Install() {
			logs::info("Installing PlayerPayCrimeGold hook...");
			REL::Relocation playerCharacterVTable{ RE::VTABLE_PlayerCharacter[0] };
			PlayerPayCrimeGold = playerCharacterVTable.write_vfunc(0xBB, PlayerPayCrimeGoldMod);
			logs::info("Done!");
		}
	private:
		static void PlayerPayCrimeGoldMod(RE::PlayerCharacter* a_this, RE::TESFaction* a_faction, bool a_goToJail, bool a_removeStolenItems) {
			// init default values
			bool goToJail = a_goToJail;
			bool removeStolenItems = a_removeStolenItems;
			bool payCrimeGold = true;
			// check faction form id
			bool factionIsVanilla = ((a_faction->GetFormID() >> 24) <= 0x4);
			logs::info("PlayerPayCrimeGold called, faction is vanilla: {}, processing", factionIsVanilla);
			// load ini
			CSimpleIniA ini;
			ini.SetUnicode();
			auto iniPath = std::format("Data\\SKSE\\Plugins\\{}.ini", SKSE::GetPluginName());
			SI_Error iniResult = ini.LoadFile(iniPath.data());
			// read ini
			if (iniResult < 0) {
				logs::info("Settings file not found, input values: {} / {} / {}", goToJail, removeStolenItems, payCrimeGold);
			} else {
				// get settings
				int goToJailMode = ini.GetLongValue("Modes", "GoToJailMode", 0);
				bool goToJailValue = ini.GetBoolValue("Values", "GoToJailValue", false);
				int removeStolenItemsMode = ini.GetLongValue("Modes", "RemoveStolenItemsMode", 0);
				bool removeStolenItemsValue = ini.GetBoolValue("Values", "RemoveStolenItemsValue", false);
				int payCrimeGoldMode = ini.GetLongValue("Modes", "PayCrimeGoldMode", 0);
				bool payCrimeGoldValue = ini.GetBoolValue("Values", "PayCrimeGoldValue", false);
				// process jail
				if (goToJailMode == 1 && factionIsVanilla) { // replace vanilla
					goToJail = goToJailValue;
				} else if (goToJailMode == 2 && !factionIsVanilla) { // replace non-vanilla
					goToJail = goToJailValue;
				} else if (goToJailMode == 3) { // replace any
					goToJail = goToJailValue;
				}
				// process items
				if (removeStolenItemsMode == 1 && factionIsVanilla) { // replace vanilla
					removeStolenItems = removeStolenItemsValue;
				} else if (removeStolenItemsMode == 2 && !factionIsVanilla) { // replace non-vanilla
					removeStolenItems = removeStolenItemsValue;
				} else if (removeStolenItemsMode == 3) { // replace any
					removeStolenItems = removeStolenItemsValue;
				}
				// process gold
				if (payCrimeGoldMode == 1 && factionIsVanilla) { // replace vanilla
					payCrimeGold = payCrimeGoldValue;
				} else if (payCrimeGoldMode == 2 && !factionIsVanilla) { // replace non-vanilla
					payCrimeGold = payCrimeGoldValue;
				} else if (payCrimeGoldMode == 3) { // replace any
					payCrimeGold = payCrimeGoldValue;
				}
				// show result
				logs::info("Settings file parsed, result values: {} / {} / {}", goToJail, removeStolenItems, payCrimeGold);
			}
			// if false > keep player's gold
			if (!payCrimeGold) {
				a_faction->SetCrimeGold(0);
				a_faction->SetCrimeGoldViolent(0);
			}
			// call original function
			PlayerPayCrimeGold(a_this, a_faction, goToJail, removeStolenItems);
		}
		static inline REL::Relocation<decltype(PlayerPayCrimeGoldMod)> PlayerPayCrimeGold;
	};
};

void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {
	switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			PlayerPayCrimeGoldTweak::PlayerPayCrimeGoldHook::Install();
			logs::info("Waiting for PlayerPayCrimeGold calls");
			break;
		default:
			break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	// show info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	// init listener
	const auto messaging = SKSE::GetMessagingInterface();
	if (messaging && messaging->RegisterListener(MessageHandler)) return true;
	logs::info("Couldn't register listener");
	return false;
}
