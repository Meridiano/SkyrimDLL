#include "hpp/console.hpp"

void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
}

bool InitMessaging() {
	const auto messagingInterface = SKSE::GetMessagingInterface();
	if (messagingInterface && messagingInterface->RegisterListener(CamFolConsole::Register)) {
		logs::info("Trying to register console commands");
		return true;
	}
	logs::info("Could not register console commands");
	return false;
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");

	// info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	std::string pluginName{ plugin->GetName() };
	logs::info(
		"{} version {} is loading into {}",
		pluginName,
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);

	// stuff
	InitMessaging();
	CamFolUtility::ReadConfig(pluginName);
	SKSE::AllocTrampoline(64);
	CamFolHooks::UpdateDataHook::Install();

	// done
	logs::info("{} plugin was loaded", pluginName);
	return true;
}
