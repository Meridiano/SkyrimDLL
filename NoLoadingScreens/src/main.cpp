#include "hpp/launch.hpp"

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	NLSLaunch::InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	return NLSLaunch::InitMessaging();
}
