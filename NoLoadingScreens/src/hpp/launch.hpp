#pragma once
#include "hpp/process.hpp"

namespace NLSLaunch {

	void InitLogging() {
		auto path = logs::log_directory();
		if (!path) return;
		const auto plugin = SKSE::PluginDeclaration::GetSingleton();
		*path /= std::format("{}.log", plugin->GetName());
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
		if (messagingInterface && messagingInterface->RegisterListener(NLSProcess::MessageListener)) {
			logs::info("Message listener registered");
			return true;
		}
		logs::info("Could not register message listener");
		return false;
	}

}