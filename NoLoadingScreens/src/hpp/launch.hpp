#pragma once
#include "hpp/process.hpp"

namespace NLSLaunch {

	void InitLogging(std::string pattern) {
		logs::init();
		spdlog::set_pattern(pattern);
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