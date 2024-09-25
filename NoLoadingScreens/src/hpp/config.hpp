#pragma once
#include "hpp/utility.hpp"

namespace NLSConfig {

	std::string sNewStatic = "";
	std::string sNewText = "";
	std::uint8_t iMode = 0;
	std::set<std::string> Exceptions = {};
	std::set<std::string> Inclusions = {};

	bool ConfigBool(mINI::INIStructure ini, std::string section, std::string key, bool fallback) {
		bool result = fallback;
		std::string raw = ini.get(section).get(key);
		auto StringToBool = [](std::string arg) {
			auto low = NLSUtility::StringToLower(arg);
			if (!low.compare("true") || !low.compare("1")) return true;
			if (!low.compare("false") || !low.compare("0")) return false;
			EXCEPTION;
		};
		try {
			result = StringToBool(raw);
		} catch (...) {
			logs::info("Failed to read [{}]{} ini value", section, key);
		}
		logs::info("Bool value [{}]{} is {}", section, key, result);
		return result;
	}

	std::uint8_t ConfigUInt8(mINI::INIStructure ini, std::string section, std::string key, std::uint8_t fallback) {
		auto result = fallback;
		std::string raw = ini.get(section).get(key);
		try {
			result = std::stoul(raw, nullptr, 0) & 0xFF;
		} catch (...) {
			logs::info("Failed to read [{}]{} ini value", section, key);
		}
		logs::info("Integer value [{}]{} is {}", section, key, result);
		return result;
	}

	std::string ConfigString(mINI::INIStructure ini, std::string section, std::string key, std::string fallback) {
		auto result = fallback;
		if (auto map = ini.get(section); map.has(key)) result = map.get(key);
		else logs::info("Failed to read [{}]{} ini value", section, key);
		logs::info("String value [{}]{} is \"{}\"", section, key, result);
		return result;
	}

	void LoadConfig() {
		const auto plugin = SKSE::PluginDeclaration::GetSingleton();
		const auto path = std::format("Data/SKSE/Plugins/{}.ini", plugin->GetName());
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			sNewStatic = ConfigString(ini, "General", "sNewStatic", "");
			sNewText = ConfigString(ini, "General", "sNewText", "");
			iMode = ConfigUInt8(ini, "General", "iMode", 0);
			if (iMode == 1) {
				std::string section = "Exceptions";
				for (auto keyIterator : ini.get(section)) {
					auto key = keyIterator.first;
					bool value = ConfigBool(ini, section, key, false);
					if (value) Exceptions.insert(NLSUtility::StringToLower(key));
				}
			}
			if (iMode == 2) {
				std::string section = "Inclusions";
				for (auto keyIterator : ini.get(section)) {
					auto key = keyIterator.first;
					bool value = ConfigBool(ini, section, key, false);
					if (value) Inclusions.insert(NLSUtility::StringToLower(key));
				}
			}
		} else stl::report_and_fail("Failed to read config file");
	}

}