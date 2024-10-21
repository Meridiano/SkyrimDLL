#pragma once

#include "hpp/hooks.hpp"

namespace CamFolConsole {

	void ConsolePrint(std::string text) {
		auto console = RE::ConsoleLog::GetSingleton();
		if (console && console->IsConsoleMode()) {
			console->Print(text.data());
		}
	}

	bool ExecuteCF(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&) {
		int size = a_scriptData->numParams;
		std::vector<std::string> arguments(1, "");
		auto stringChunk = a_scriptData->GetStringChunk();
		for (int index = 0; index < size; index++) {
			arguments[index] = stringChunk->GetString();
			stringChunk = stringChunk->GetNext()->AsString();
		}
		std::string data = arguments[0];
		std::string info = CamFolUtility::SetProcessEnabled(data);
		ConsolePrint(info);
		return true;
	}

	bool RegisterCF() {
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand(CamFolUtility::ConsoleCommand);
		if (info) {
			// define parameters
			using Type = RE::SCRIPT_PARAM_TYPE;
			static RE::SCRIPT_PARAMETER parameters[] = {
				{ "Toggle (Boolean)", Type::kChar, true }
			};

			// replace info
			info->functionName = "CameraFollows";
			info->shortName = "CamFol";
			info->helpString = "Camera Follows Console Command";
			info->referenceFunction = false;
			info->SetParameters(parameters); // set both "numParams" and "params"
			info->executeFunction = ExecuteCF;

			/* leave the same
			info->compileFunction = info->compileFunction; */

			info->conditionFunction = nullptr;
			info->editorFilter = false;
			info->invalidatesCellList = false;

			// done
			logs::info("CameraFollows and CamFol console commands registered");
			return true;
		}
		logs::info("CameraFollows and CamFol console commands not registered");
		return false;
	}

	void Register(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			bool com = RegisterCF();
			logs::info("{}", com ? "Console commands registered" : "Could not register console commands");
			CamFolHooks::FillPointers();
		}
	}

}
