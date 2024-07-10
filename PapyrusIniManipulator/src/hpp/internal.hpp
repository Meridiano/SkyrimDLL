#pragma once

#include "hpp/utility.hpp"

namespace PIMInternal {

	bool usePrettyPrint = true;
	bool useTranslation = true;

	std::string GetTranslation(std::string key) {
		std::string result = "";
		bool success = SKSE::Translation::Translate(key, result);
		return (success ? result : key);
	}

	bool IniDataExistsInternal(std::int32_t level, std::string path, std::string section, std::string key) {
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			if (level == 0) {
				return true;
			} else if (level == 1) {
				return ini.has(section);
			} else if (level == 2) {
				return (ini.has(section) && ini.get(section).has(key));
			}
			return false;
		}
		return false;
	}

	bool ClearIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key) {
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			if (level == 0) {
				ini.clear();
				return file.write(ini, usePrettyPrint);
			} else if ((level == 1) && ini.has(section)) {
				ini[section].clear();
				return file.write(ini, usePrettyPrint);
			} else if ((level == 2) && ini.has(section) && ini.get(section).has(key)) {
				ini[section][key] = "";
				return file.write(ini, usePrettyPrint);
			}
			return false;
		}
		return false;
	}

	bool DestroyIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key) {
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			if (level == 0) {
				fs::path target(path);
				return fs::remove(target);
			} else if ((level == 1) && ini.has(section)) {
				ini.remove(section);
				return file.write(ini, usePrettyPrint);
			} else if ((level == 2) && ini.has(section) && ini.get(section).has(key)) {
				ini[section].remove(key);
				return file.write(ini, usePrettyPrint);
			}
			return false;
		}
		return false;
	}

	std::vector<std::string> GetIniDataInternal(std::int32_t level, std::string path, std::string section, std::string key) {
		std::vector<std::string> result;
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			if (level == 0) {
				for (std::pair iterator : ini) {
					result.push_back(iterator.first);
				}
				return result;
			} else if ((level == 1) && ini.has(section)) {
				for (std::pair iterator : ini.get(section)) {
					result.push_back(iterator.first);
				}
				return result;
			} else if ((level == 2) && ini.has(section) && ini.get(section).has(key)) {
				std::string raw = ini.get(section).get(key);
				std::string str = (useTranslation ? GetTranslation(raw) : raw);
				return PIMUtility::StringToVector(str);
			}
			return result;
		}
		return result;
	}

	std::string PullStringFromIniInternal(std::string path, std::string section, std::string key, std::string defaultValue) {
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini) && ini.has(section) && ini.get(section).has(key)) {
			std::string raw = ini.get(section).get(key);
			return (useTranslation ? GetTranslation(raw) : raw);
		}
		return defaultValue;
	}

	bool PullBoolFromIniInternal(std::string path, std::string section, std::string key, bool defaultValue) {
		std::string raw = PullStringFromIniInternal(path, section, key, PIMUtility::BoolToString(defaultValue));
		return PIMUtility::StringToBool(raw, defaultValue);
	}

	std::int32_t PullIntFromIniInternal(std::string path, std::string section, std::string key, std::int32_t defaultValue) {
		std::string raw = PullStringFromIniInternal(path, section, key, std::to_string(defaultValue));
		std::int32_t result;
		try {
			result = std::stol(raw, nullptr, 0);
		} catch (...) {
			return defaultValue;
		}
		return result;
	}

	float PullFloatFromIniInternal(std::string path, std::string section, std::string key, float defaultValue) {
		std::string raw = PullStringFromIniInternal(path, section, key, std::to_string(defaultValue));
		float result;
		try {
			result = std::stof(raw, nullptr);
		} catch (...) {
			return defaultValue;
		}
		return result;
	}

	bool PushStringToIniInternal(std::string path, std::string section, std::string key, std::string value, bool force) {
		if (!PIMUtility::FileExists(path, force)) return false;
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (!file.read(ini)) // process file
		{
			if (force) file.generate(ini, usePrettyPrint);
			else return false;
		}
		if (section.empty()) // exit if section is ""
		{
			return file.write(ini, usePrettyPrint);
		}
		if (!ini.has(section)) // process section
		{
			if (force) ini[section];
			else return false;
		}
		if (key.empty()) // exit if key is ""
		{
			return file.write(ini, usePrettyPrint);
		}
		if (!ini.get(section).has(key)) // process key
		{
			if (force) ini[section][key];
			else return false;
		}
		if (value.empty()) // exit if value is ""
		{
			return file.write(ini, usePrettyPrint);
		}
		// finally set new value
		ini[section][key] = value;
		return file.write(ini, usePrettyPrint);
	}

	bool PushBoolToIniInternal(std::string path, std::string section, std::string key, bool value, bool force) {
		return PushStringToIniInternal(path, section, key, PIMUtility::BoolToString(value), force);
	}

	bool PushIntToIniInternal(std::string path, std::string section, std::string key, std::int32_t value, bool force) {
		return PushStringToIniInternal(path, section, key, std::to_string(value), force);
	}

	bool PushFloatToIniInternal(std::string path, std::string section, std::string key, float value, bool force) {
		return PushStringToIniInternal(path, section, key, std::to_string(value), force);
	}

}
