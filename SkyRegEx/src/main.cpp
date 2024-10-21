#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "hpp/papyrus.hpp"

void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
}

bool InitPapyrus() {
	const auto papyrusInterface = SKSE::GetPapyrusInterface();
	if (papyrusInterface && papyrusInterface->Register(SREPapyrus::Register)) {
		logs::info("Papyrus functions registered");
		return true;
	}
	logs::info("Could not register Papyrus functions");
	return false;
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	// init
	SKSE::Init(a_skse, false);
	InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	
	// info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);

	return InitPapyrus();
}
