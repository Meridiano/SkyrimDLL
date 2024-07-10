#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "hpp/papyrus.hpp"

void InitLogging() {
	std::optional<std::filesystem::path> path = SREUtility::GetLogsDirectory();
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
	InitLogging();
	
	// info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info("{} version {} is loading into {}", plugin->GetName(), plugin->GetVersion(), REL::Module::get().version());

	// init
	SKSE::Init(a_skse);
	bool papyrus = InitPapyrus();

	// finalize
	if (papyrus) {
		logs::info("Plugin loading done");
		return true;
	}
	logs::info("Plugin loading failed");
	return false;
}
