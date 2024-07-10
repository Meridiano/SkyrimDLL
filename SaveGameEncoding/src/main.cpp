#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace SGEStorage {

	std::string savePath = "";
	std::string sksePath = "";
	bool process = false;
	int attempt = 0;

}

namespace SGEUtility {

	namespace fs = std::filesystem;

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
		return fs::path("");
	}

	fs::path GetSavesDirectory() {
		fs::path gamesDirectory = GetDocumentsDirectory() / "My Games";
		fs::path skyrimDirectory;
		if (REL::Module::IsVR()) {
			skyrimDirectory = gamesDirectory / "Skyrim VR";
		} else {
			auto steam = fs::path("steam_api64.dll");
			auto special = fs::exists(steam) ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			skyrimDirectory = gamesDirectory / special;
		}
		fs::path savesDirectory = skyrimDirectory / RE::GetINISetting("sLocalSavePath:General")->GetString();
		return savesDirectory;
	}

	void RenameSaveFiles(fs::path savePath, fs::path sksePath) {
		std::wstring newSaveName = stl::utf8_to_utf16(savePath.string()).value_or(L"");
		std::wstring newSkseName = stl::utf8_to_utf16(sksePath.string()).value_or(L"");
		fs::path newSavePath(newSaveName);
		fs::path newSksePath(newSkseName);
		if (savePath.compare(newSavePath) == 0 && sksePath.compare(newSksePath) == 0) return;
		if (fs::exists(newSavePath)) fs::remove(newSavePath);
		if (fs::exists(newSksePath)) fs::remove(newSksePath);
		std::error_code saveError;
		std::error_code skseError;
		fs::rename(savePath, newSavePath, saveError);
		fs::rename(sksePath, newSksePath, skseError);
		if (saveError.value() || skseError.value()) {
			logs::info("saveError >> {}", saveError.message());
			logs::info("skseError >> {}", skseError.message());
		}
	}

	void EnableUpdate(std::string save, std::string skse) {
		SGEStorage::attempt = 0;
		SGEStorage::process = true;
		fs::path savesDirectory = GetSavesDirectory();
		SGEStorage::savePath = (savesDirectory / save).string();
		SGEStorage::sksePath = (savesDirectory / skse).string();
	}

	void DisableUpdate() {
		SGEStorage::attempt = 0;
		SGEStorage::process = false;
		SGEStorage::savePath = "";
		SGEStorage::sksePath = "";
	}

}

namespace SGEProcess {

	class SaveHook {
	public:
		static void InstallHook() {
			auto address = REL::VariantID(34818, 35727, 0x58E310).address();
			auto offset = REL::VariantOffset(0x112, 0x1CE, 0x116).offset();
			SaveGameSub = SKSE::GetTrampoline().write_call<5>(address + offset, SaveGameSubMod);
			if (SaveGameSub.address()) logs::info("Save hook done >_<");
			else logs::info("Save hook failed T_T");
		}
	private:
		static const char* SaveGameSubMod(RE::BGSSaveLoadManager* a1, void* a2, const char* a3, void* a4, std::int32_t a5) {
			// do my stuff
			std::string save = fmt::format("{}.ess", a3);
			std::string skse = fmt::format("{}.skse", a3);
			SGEUtility::EnableUpdate(save, skse);
			// call original function
			return SaveGameSub(a1, a2, a3, a4, a5);
		}
		static inline REL::Relocation<decltype(SaveGameSubMod)> SaveGameSub;
	};

	class UpdateHook {
	public:
		static void InstallHook() {
			auto address = REL::VariantID(35565, 36564, 0x5BAB10).address();
			auto offset = REL::VariantOffset(0x748, 0xC26, 0x7EE).offset();
			OnUpdate = SKSE::GetTrampoline().write_call<5>(address + offset, OnUpdateMod);
			if (OnUpdate.address()) logs::info("Update hook done >_<");
			else logs::info("Update hook failed T_T");
		}
	private:
		static void OnUpdateMod() {
			// do my stuff
			if (SGEStorage::process) {
				namespace fs = SGEUtility::fs;
				fs::path save(SGEStorage::savePath);
				fs::path skse(SGEStorage::sksePath);
				if (fs::exists(save) && fs::exists(skse)) {
					logs::info("RenameSaveFiles\n{}\n{}", SGEStorage::savePath, SGEStorage::sksePath);
					SGEUtility::RenameSaveFiles(save, skse);
					SGEUtility::DisableUpdate();
				} else SGEStorage::attempt += 1;
				if (SGEStorage::attempt > 30) SGEUtility::DisableUpdate();
			}
			// call original function
			OnUpdate();
		}
		static inline REL::Relocation<decltype(OnUpdateMod)> OnUpdate;
	};

	void ReadMessage(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			SaveHook::InstallHook();
			UpdateHook::InstallHook();
		}
	}

}

std::filesystem::path LogPath() {
	auto relPath = std::filesystem::path("Data\\SKSE\\Plugins");
	auto absPath = std::filesystem::absolute(relPath);
	return absPath;
}

void InitLogging()
{
	std::optional<std::filesystem::path> path = LogPath();
	if (!path) return;
	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	*path /= fmt::format("{}.log", plugin->GetName());
	std::vector<spdlog::sink_ptr> sinks{ 
		std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true), 
		std::make_shared<spdlog::sinks::msvc_sink_mt>() 
	};
	auto logger = std::make_shared<spdlog::logger>("global", sinks.begin(), sinks.end());
	logger->set_level(spdlog::level::info);
	logger->flush_on(spdlog::level::info);
	spdlog::set_default_logger(std::move(logger));
	spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
}

bool InitMessaging() {
	const auto messagingInterface = SKSE::GetMessagingInterface();
	if (messagingInterface && messagingInterface->RegisterListener(SGEProcess::ReadMessage)) {
		logs::info("Trying to install hooks");
		return true;
	}
	logs::info("Could not install hooks");
	return false;
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	InitLogging();

	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info("{} version {} is loading into {}", plugin->GetName(), plugin->GetVersion(), REL::Module::get().version());

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1024, true);
	
	bool result = InitMessaging();
	if (result) logs::info("{} loaded", plugin->GetName());
	else logs::info("{} failed", plugin->GetName());
	
	return true;
}
