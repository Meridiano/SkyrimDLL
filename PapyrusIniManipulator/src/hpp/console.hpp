#pragma once

#include "hpp/internal.hpp"

namespace PIMConsole {

	void ConsolePrint(std::string text) {
		auto console = RE::ConsoleLog::GetSingleton();
		if (console && console->IsConsoleMode()) {
			console->Print(text.data());
		}
	}

	#define ConLog(fmt, ...) ConsolePrint(std::format(fmt, __VA_ARGS__))

	bool ExecutePVFI(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&) {
		std::vector<std::string> arguments(3, "");
		auto stringChunk = a_scriptData->GetStringChunk();
		for (int index = 0; index < a_scriptData->numParams; index++) {
			arguments[index] = stringChunk->GetString();
			stringChunk = stringChunk->GetNext()->AsString();
		}
		std::string path = arguments[0];
		std::string section = arguments[1];
		std::string key = arguments[2];
		ConLog("{}Path = {}\n{}Section = {}\n{}Key = {}", tab, path, tab, section, tab, key);
		if (PIMInternal::IniDataExistsInternal(2, path, section, key)) {
			ConLog("{}Value = {}", d_tab, PIMInternal::PullStringFromIniInternal(path, section, key, ""));
			return true;
		}
		ConLog("{}Could not find this path/section/key.", d_tab);
		return false;
	}

	bool ExecutePVTI(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR*, RE::TESObjectREFR*, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&) {
		std::vector<std::string> arguments(5, "");
		auto stringChunk = a_scriptData->GetStringChunk();
		for (int index = 0; index < a_scriptData->numParams; index++) {
			arguments[index] = stringChunk->GetString();
			stringChunk = stringChunk->GetNext()->AsString();
		}
		std::string path = arguments[0];
		std::string section = arguments[1];
		std::string key = arguments[2];
		std::string value = arguments[3];
		bool force = PIMUtility::StringToBool(arguments[4], false);
		ConLog("{}Path = {}\n{}Section = {}\n{}Key = {}\n{}Value = {}\n{}Force = {}", tab, path, tab, section, tab, key, tab, value, tab, force);
		if (PIMInternal::IniDataExistsInternal(2, path, section, key) || force) {
			bool result = PIMInternal::PushStringToIniInternal(path, section, key, value, force);
			if (result) {
				ConLog("{}Value has been pushed successfully.", d_tab);
				return true;
			}
			ConLog("{}Could not push value to this path/section/key.", d_tab);
			return false;
		}
		ConLog("{}Could not find this path/section/key.\n{}Push without force canceled.", d_tab, d_tab);
		return false;
	}

	#undef ConLog

	bool RegisterPVFI() {
		std::string toSteal = PIMInternal::PullStringFromIniInternal(PIMUtility::PluginConfigPath(), "Console", "sConsoleCommandToStealA", "StartTrackPlayerDoors");
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand(toSteal);
		if (info) {
			using Type = RE::SCRIPT_PARAM_TYPE;
			static RE::SCRIPT_PARAMETER params[] = {
				{ "Path (String)", Type::kChar, false },
				{ "Section (String)", Type::kChar, true },
				{ "Key (String)", Type::kChar, true }
			};
			info->functionName = "PullValueFromIni";
			info->shortName = "PVFI";
			info->helpString = "Pulls value from ini file.";
			info->referenceFunction = false;
			info->SetParameters(params);
			info->executeFunction = ExecutePVFI;
			info->conditionFunction = nullptr;
			info->editorFilter = false;
			info->invalidatesCellList = false;
			logs::info("PullValueFromIni and PVFI console commands registered");
			return true;
		}
		logs::info("PullValueFromIni and PVFI console commands not registered");
		return false;
	}

	bool RegisterPVTI() {
		std::string toSteal = PIMInternal::PullStringFromIniInternal(PIMUtility::PluginConfigPath(), "Console", "sConsoleCommandToStealB", "StopTrackPlayerDoors");
		auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand(toSteal);
		if (info) {
			using Type = RE::SCRIPT_PARAM_TYPE;
			static RE::SCRIPT_PARAMETER params[] = {
				{ "Path (String)", Type::kChar, false },
				{ "Section (String)", Type::kChar, true },
				{ "Key (String)", Type::kChar, true },
				{ "Value (String)", Type::kChar, true },
				{ "Force (Boolean)", Type::kChar, true }
			};
			info->functionName = "PushValueToIni";
			info->shortName = "PVTI";
			info->helpString = "Pushes value to ini file.";
			info->referenceFunction = false;
			info->SetParameters(params);
			info->executeFunction = ExecutePVTI;
			info->conditionFunction = nullptr;
			info->editorFilter = false;
			info->invalidatesCellList = false;
			logs::info("PushValueToIni and PVTI console commands registered");
			return true;
		}
		logs::info("PushValueToIni and PVTI console commands not registered");
		return false;
	}

	void Register(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			if (RegisterPVFI() && RegisterPVTI()) {
				logs::info("Console commands registered");
			} else {
				logs::info("Could not register console commands");
			}
		}
	}

}
