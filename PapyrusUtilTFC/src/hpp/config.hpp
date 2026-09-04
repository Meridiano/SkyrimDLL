#pragma once

namespace PluginConfig {

	struct IniStruct {
		std::string section{ "" };
		std::string key{ "" };
	};

	struct BoolStruct : IniStruct {
		bool value{ false };
	};

	BoolStruct bInvertedApproach{ "General", "bInvertedApproach", false };

	const char* ConfigPath() {
		static std::string result = []() {
			std::string name{ SKSE::GetPluginName() };
			return std::format("Data/SKSE/Plugins/{}.ini", name);
		}();
		return result.data();
	}

	bool ReadBoolFromIni(std::string section, std::string key, bool fallback) {
		CSimpleIniA ini;
		ini.SetUnicode();
		if (ini.LoadFile(ConfigPath()) < 0) return fallback;
		return ini.GetBoolValue(section.data(), key.data(), fallback);
	}

	void ReadBoolStruct(BoolStruct* arg) {
		if (arg) arg->value = ReadBoolFromIni(arg->section, arg->key, arg->value);
	}

	void ReadConfig() {
		ReadBoolStruct(&bInvertedApproach);
	}

}
