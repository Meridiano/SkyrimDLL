#include "hpp/console.hpp"

std::filesystem::path LogPath() {
	auto relPath = std::filesystem::path("Data\\SKSE\\Plugins");
	auto absPath = std::filesystem::absolute(relPath);
	return absPath;
}

void InitLogging() {
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
	InitLogging();

	// info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);

	// skse
	SKSE::Init(a_skse);

	// stuff
	InitMessaging();
	CamFolUtility::ReadConfig(plugin);
	CamFolHooks::UpdatePCHook::Install();

	// done
	logs::info("{} plugin was loaded", plugin->GetName());
	return true;
}
