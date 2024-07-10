#include "hpp/Data.hpp"
#include "hpp/Hooks.hpp"

namespace CCUtility {

	fs::path GetDocumentsDirectory() {
		wchar_t* docPath = NULL;
		auto resA = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &docPath);
		if (resA == S_OK) {
			std::wstring docPathWS(docPath);
			return fs::path(docPathWS);
		} else {
			auto resB = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT_PATH, NULL, &docPath);
			if (resB == S_OK) {
				std::wstring docPathWS(docPath);
				return fs::path(docPathWS);
			}
		}
		return fs::path();
	}

	std::optional<fs::path> GetLogsDirectory() {
		fs::path documentsDirectory = GetDocumentsDirectory();
		if (documentsDirectory.empty()) return std::nullopt;
		// construct path
		fs::path gamesDirectory = documentsDirectory / "My Games";
		fs::path skyrimDirectory;
		if (REL::Module::IsVR()) {
			skyrimDirectory = gamesDirectory / "Skyrim VR";
		} else {
			auto special = fs::exists("steam_api64.dll") ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			skyrimDirectory = gamesDirectory / special;
		}
		return (skyrimDirectory / "SKSE");
	}

	void InitLogger() {
		auto path = GetLogsDirectory();
		if (!path) return;
		auto plugin = SKSE::PluginDeclaration::GetSingleton();
		*path /= fmt::format(FMT_STRING("{}.log"), plugin->GetName());
		std::shared_ptr<spdlog::sinks::sink> sink;
		if (WinAPI::IsDebuggerPresent()) {
			sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		} else {
			sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
		}
		auto log = std::make_shared<spdlog::logger>("global log", sink);
		log->set_level(spdlog::level::info);
		log->flush_on(spdlog::level::info);
		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	}

	void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
		switch (a_msg->type) {
			case SKSE::MessagingInterface::kDataLoaded: {
				// load translation
				Translation::ParseTranslation("CraftingCategories");
				// load settings
				Data::ConfigLoader::GetSingleton()->LoadConfigs();
				// done
				break;
			}
		}
	}

}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	CCUtility::InitLogger();
	auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logger::info("{} version {} is loading into {}", plugin->GetName(), plugin->GetVersion(), REL::Module::get().version());
	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(128);
	if (Hooks::Crafting::Install()) {
		// start listener
		SKSE::GetMessagingInterface()->RegisterListener(CCUtility::MessageListener);
		return true;
	}
	return false;
}
