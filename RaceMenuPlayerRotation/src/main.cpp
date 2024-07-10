#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "hpp/process.hpp"

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
	std::shared_ptr<spdlog::sinks::sink> sink;
	if (WinAPI::IsDebuggerPresent()) sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
	else sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	auto logger = std::make_shared<spdlog::logger>("global", sink);
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);
	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
}

void InitMessaging()
{
	logs::info("Initializing message listener...");
	const auto interface = SKSE::GetMessagingInterface();
	if (interface->RegisterListener(ROTProcess::ProcessMessage)) {
		logs::info("Message listener successfully initialized");
	} else {
		stl::report_and_fail("Failed to initialize message listener");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	// init logs
	InitLogging();

	// show info
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info("{} version {} is loading into {}", plugin->GetName(), plugin->GetVersion(), REL::Module::get().version());
	
	// init skse and stuff
	SKSE::Init(a_skse);
	InitMessaging();
	ROTConfig::LoadConfig();

	// done
	logs::info("{} loading done", plugin->GetName());
	return true;
}
