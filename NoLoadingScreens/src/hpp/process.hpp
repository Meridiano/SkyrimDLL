#pragma once
#include "hpp/config.hpp"

namespace NLSProcess {

	void ProcessLoadScreens(RE::TESDataHandler* tesDH, std::uint8_t mode) {
		auto list = tesDH->GetFormArray<RE::TESLoadScreen>();
		auto stat = NLSUtility::RecordFromText(tesDH, NLSConfig::sNewStatic);
		auto text = NLSConfig::sNewText;
		if (list.size() > 0) for (auto form : list) {
			auto filename = NLSUtility::SourceFilenameLower(form, tesDH);
			bool process = !filename.empty();
			switch (mode) {
				case 0:
					process = true;
					break;
				case 1:
					process = !NLSConfig::Exceptions.contains(filename);
					break;
				case 2:
					process = NLSConfig::Inclusions.contains(filename);
					break;
				default:
					process = false;
					break;
			}
			if (form && process) {
				if (auto data = form->loadNIFData; data) {
					data->loadNIF = reinterpret_cast<RE::TESBoundObject*>(stat);
				}
				form->loadingText = text;
			};
		}
	}

	void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
			NLSConfig::LoadConfig();
		} else if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			const auto tesDH = RE::TESDataHandler::GetSingleton();
			if (tesDH) logs::info("TESDataHandler = {:X}", (std::uint64_t)tesDH);
			else stl::report_and_fail("Error: TESDataHandler not found");
			ProcessLoadScreens(tesDH, NLSConfig::iMode);
		} else return;
	}

}