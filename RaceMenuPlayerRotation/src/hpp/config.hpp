#pragma once

#include "hpp/utility.hpp"

namespace ROTConfig {

	bool ConfigBool(mINI::INIStructure ini, std::string section, std::string key) {
		bool result = false;
		std::string raw = ini.get(section).get(key);
		std::string lower = ROTUtility::StringToLower(raw);
		if (lower.compare("true") == 0 || lower.compare("1") == 0) result = true;
		else if (lower.compare("false") == 0 || lower.compare("0") == 0) result = false;
		else logs::info("Failed to read [{}]{} ini value", section, key);
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
		const auto plugin = SKSE::PluginDeclaration::GetSingleton();
		std::string configPath = fmt::format("Data\\SKSE\\Plugins\\{}.ini", plugin->GetName());
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