#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "hpp/process.hpp"

void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
}

void InitMessaging() {
	logs::info("Initializing message listener...");
	const auto msgInterface = SKSE::GetMessagingInterface();
	if (msgInterface && msgInterface->RegisterListener(ROTProcess::ProcessMessage)) {
		logs::info("Message listener successfully initialized");
	} else {
		stl::report_and_fail("Failed to initialize message listener");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	// // init skse and logs
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

	// done
	InitMessaging();
	ROTConfig::LoadConfig();
	return true;
}
