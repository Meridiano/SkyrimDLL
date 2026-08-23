#pragma once

#include "hpp/internal.hpp"

namespace PIMPapyrus {

	std::string GetVersion(RE::StaticFunctionTag* base) {
		const auto version = SKSE::GetPluginVersion();
		return version.string("-");
	}

	std::string GetTranslation(RE::StaticFunctionTag* base, std::string key) {
		return PIMInternal::GetTranslation(key);
	}

	bool IniDataExists(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key) {
		return PIMInternal::IniDataExistsInternal(level, path, section, key);
	}
	
	bool ClearIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key) {
		return PIMInternal::ClearIniDataInternal(level, path, section, key);
	}
	
	bool DestroyIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key) {
		return PIMInternal::DestroyIniDataInternal(level, path, section, key);
	}
	
	std::vector<std::string> GetIniData(RE::StaticFunctionTag* base, std::int32_t level, std::string path, std::string section, std::string key) {
		return PIMInternal::GetIniDataInternal(level, path, section, key);
	}
	
	std::string PullStringFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::string defaultValue) {
		return PIMInternal::PullStringFromIniInternal(path, section, key, defaultValue);
	}
	
	bool PullBoolFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, bool defaultValue) {
		return PIMInternal::PullBoolFromIniInternal(path, section, key, defaultValue);
	}
	
	std::int32_t PullIntFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::int32_t defaultValue) {
		return PIMInternal::PullIntFromIniInternal(path, section, key, defaultValue);
	}
	
	float PullFloatFromIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, float defaultValue) {
		return PIMInternal::PullFloatFromIniInternal(path, section, key, defaultValue);
	}
	
	bool PushStringToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::string value, bool force) {
		return PIMInternal::PushStringToIniInternal(path, section, key, value, force);
	}
	
	bool PushBoolToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, bool value, bool force) {
		return PIMInternal::PushBoolToIniInternal(path, section, key, value, force);
	}
	
	bool PushIntToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, std::int32_t value, bool force) {
		return PIMInternal::PushIntToIniInternal(path, section, key, value, force);
	}
	
	bool PushFloatToIni(RE::StaticFunctionTag* base, std::string path, std::string section, std::string key, float value, bool force) {
		return PIMInternal::PushFloatToIniInternal(path, section, key, value, force);
	}

	bool Register(RE::BSScript::IVirtualMachine* a_vm) {
		const auto className = SKSE::GetPluginName();

		// general
		a_vm->RegisterFunction("GetVersion", className, GetVersion);
		a_vm->RegisterFunction("GetTranslation", className, GetTranslation);

		// basic
		a_vm->RegisterFunction("IniDataExists", className, IniDataExists);
		a_vm->RegisterFunction("ClearIniData", className, ClearIniData);
		a_vm->RegisterFunction("DestroyIniData", className, DestroyIniData);
		a_vm->RegisterFunction("GetIniData", className, GetIniData);

		// pullers
		a_vm->RegisterFunction("PullStringFromIni", className, PullStringFromIni);
		a_vm->RegisterFunction("PullBoolFromIni", className, PullBoolFromIni);
		a_vm->RegisterFunction("PullIntFromIni", className, PullIntFromIni);
		a_vm->RegisterFunction("PullFloatFromIni", className, PullFloatFromIni);

		// pushers
		a_vm->RegisterFunction("PushStringToIni", className, PushStringToIni);
		a_vm->RegisterFunction("PushBoolToIni", className, PushBoolToIni);
		a_vm->RegisterFunction("PushIntToIni", className, PushIntToIni);
		a_vm->RegisterFunction("PushFloatToIni", className, PushFloatToIni);

		return true;
	}

}
