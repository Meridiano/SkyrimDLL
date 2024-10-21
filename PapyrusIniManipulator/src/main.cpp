#include "hpp/launchers.hpp"

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	// init skse and logger 
	SKSE::Init(a_skse, false);
	PIMLaunchers::InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");

	// log versions and init stuff
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	
	std::string pluginConfigPath = PIMUtility::PluginConfigPath();

	// do we need papyrus?
	bool registerPapyrusFunctionsResult = false;
	if (PIMInternal::PullBoolFromIniInternal(pluginConfigPath, "Papyrus", "bRegisterPapyrusFunctions", false)) {
		registerPapyrusFunctionsResult = PIMLaunchers::InitPapyrus();
	} else logs::info("Papyrus functions disabled");

	// do we need console?
	bool registerConsoleCommandsResult = false;
	if (PIMInternal::PullBoolFromIniInternal(pluginConfigPath, "Console", "bRegisterConsoleCommands", false)) {
		registerConsoleCommandsResult = PIMLaunchers::InitMessaging();
	} else logs::info("Console commands disabled");

	if (registerPapyrusFunctionsResult || registerConsoleCommandsResult) {
		PIMInternal::usePrettyPrint = PIMInternal::PullBoolFromIniInternal(pluginConfigPath, "General", "bUsePrettyPrint", false);
		logs::info("Pretty-print mode value is {}", PIMInternal::usePrettyPrint);
		PIMInternal::useTranslation = PIMInternal::PullBoolFromIniInternal(pluginConfigPath, "General", "bUseTranslation", false);
		logs::info("Translation mode value is {}", PIMInternal::useTranslation);
		return true;
	}

	logs::info("Both Papyrus functions and console commands are off, unload plugin");
	return false;
}
