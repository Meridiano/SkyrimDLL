#include "hpp/hooks.hpp"

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse) {
	SKSE::InitInfo info{
		.logPattern = "%d.%m.%Y %H:%M:%S [%s:%#] %v",
		.trampoline = true,
		.trampolineSize = 100
	};
	SKSE::Init(a_skse, info);
	FCData::gameVersion = a_skse->RuntimeVersion();
	auto gameName = std::filesystem::path(MODULE.filePath()).filename().replace_extension("");
	logs::info("{} v{}", gameName.string(), FCData::gameVersion.string("-"));
	FCConfig::ParseConfigList();
	FCHooks::InstallAll();
	return true;
}
