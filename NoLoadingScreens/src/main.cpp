#include "hpp/launch.hpp"

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	NLSLaunch::InitLogging();
	
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	SKSE::Init(a_skse);
	return NLSLaunch::InitMessaging();
}
