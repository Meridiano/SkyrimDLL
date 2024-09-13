#pragma once

#include "hpp/papyrus.hpp"
#include "hpp/console.hpp"

namespace PIMLaunchers {

	void InitLogging() {
		auto path = PIMUtility::PluginLogPath();
		const auto plugin = SKSE::PluginDeclaration::GetSingleton();
		path /= std::format("{}.log", plugin->GetName());
		std::shared_ptr<spdlog::sinks::sink> sink;
		if (WinAPI::IsDebuggerPresent()) {
			sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		} else {
			sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true);
		}
		auto logger = std::make_shared<spdlog::logger>("global", sink);
		logger->set_level(spdlog::level::info);
		logger->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(logger));
		spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	}
	
	bool InitPapyrus() {
		const auto papyrusInterface = SKSE::GetPapyrusInterface();
		if (papyrusInterface && papyrusInterface->Register(PIMPapyrus::Register)) {
			logs::info("Papyrus functions registered");
			return true;
		}
		logs::info("Could not register Papyrus functions");
		return false;
	}

	bool InitMessaging() {
		const auto messagingInterface = SKSE::GetMessagingInterface();
		if (messagingInterface && messagingInterface->RegisterListener(PIMConsole::Register)) {
			logs::info("Trying to register console commands");
			return true;
		}
		logs::info("Could not register console commands");
		return false;
	}
	
}
