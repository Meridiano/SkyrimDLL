#pragma once

#include "hpp/utility.hpp"

namespace ROTConfig {

	bool ConfigBool(mINI::INIStructure ini, std::string section, std::string key) {
		bool result = false;
		std::string raw = ini.get(section).get(key);
		auto StringToBool = [](std::string str) {
			auto low = ROTUtility::StringToLower(str);
			if (!low.compare("true") || !low.compare("1")) return true;
			if (!low.compare("false") || !low.compare("0")) return false;
			throw std::invalid_argument("non-boolean string argument");
		};
		try {
			result = StringToBool(raw);
		} catch (...) {
			logs::info("Failed to read [{}]{} ini value", section, key);
		}
		logs::info("Bool value [{}]{} is {}", section, key, result);
		return result;
	}

	std::int32_t ConfigInt(mINI::INIStructure ini, std::string section, std::string key) {
		std::int32_t result = 0;
		std::string raw = ini.get(section).get(key);
		try { result = std::stol(raw, nullptr, 0); }
		catch (...) { logs::info("Failed to read [{}]{} ini value", section, key); }
		logs::info("Int value [{}]{} is {}", section, key, result);
		return result;
	}

	float ConfigFloat(mINI::INIStructure ini, std::string section, std::string key) {
		float result = 0.0F;
		std::string raw = ini.get(section).get(key);
		try { result = std::stof(raw, nullptr); }
		catch (...) { logs::info("Failed to read [{}]{} ini value", section, key); }
		logs::info("Float value [{}]{} is {}", section, key, result);
		return result;
	}

	std::int32_t iRegularButton;
	std::int32_t iGamepadButtonL;
	std::int32_t iGamepadButtonR;
	float fRotationAmount;

	void LoadConfig() {
		std::string configPath = std::format("Data\\SKSE\\Plugins\\{}.ini", SKSE::GetPluginName());
		mINI::INIFile file(configPath);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			// general
			fRotationAmount = ConfigFloat(ini, "General", "fRotationAmount");

			// regular
			iRegularButton = ConfigInt(ini, "Regular", "iRegularButton");

			// gamepad
			iGamepadButtonL = ConfigInt(ini, "Gamepad", "iGamepadButtonL");
			iGamepadButtonR = ConfigInt(ini, "Gamepad", "iGamepadButtonR");
		} else stl::report_and_fail("Failed to read config file");
	}
}