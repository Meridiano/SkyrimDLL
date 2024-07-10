#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

std::filesystem::path LogPath() {
	auto relPath = std::filesystem::path("Data\\SKSE\\Plugins");
	auto absPath = std::filesystem::absolute(relPath);
	return absPath;
}

void InitLogging()
{
	std::optional<std::filesystem::path> path = LogPath();
	if (!path) return;
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	*path /= fmt::format("{}.log", plugin->GetName());
	std::vector<spdlog::sink_ptr> sinks{
		std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true), 
		std::make_shared<spdlog::sinks::msvc_sink_mt>() 
	};
	auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);
	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
}

class PlayerPayCrimeGoldTweak {
public:
	class PlayerPayCrimeGoldHook {
	public:
		static void Install() {
			logs::info("Installing PlayerPayCrimeGold hook...");
			REL::Relocation<std::uintptr_t> playerCharacterVTable{RE::VTABLE_PlayerCharacter[0]};
			PlayerPayCrimeGold = playerCharacterVTable.write_vfunc(0xbb, PlayerPayCrimeGoldMod);
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
			SI_Error iniResult = ini.LoadFile("Data\\SKSE\\Plugins\\PlayerPayCrimeGoldTweak.ini");
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
		{
			PlayerPayCrimeGoldTweak::PlayerPayCrimeGoldHook::Install();
			logs::info("Waiting for PlayerPayCrimeGold calls");
			break;
		}
		default: break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	InitLogging();
	// show info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info("{} version {} is loading into {}", plugin->GetName(), plugin->GetVersion(), REL::Module::get().version());
	// init listener
	SKSE::Init(a_skse);
	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		logs::info("Couldn't register listener");
		return false;
	}
	// done
	return true;
}
