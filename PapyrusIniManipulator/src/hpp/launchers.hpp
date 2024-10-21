#pragma once

#include "hpp/papyrus.hpp"
#include "hpp/console.hpp"

namespace PIMLaunchers {

	void InitLogging(std::string pattern) {
		logs::init();
		spdlog::set_pattern(pattern);
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
