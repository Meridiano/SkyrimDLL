#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace SGEStorage {

	fs::path savePath = "";
	fs::path sksePath = "";
	bool process = false;
	BYTE attempt = 0;

}

namespace SGEUtility {

	fs::path GetSavesDirectory() {
		auto skyrimDirectory = logs::log_directory().value_or("").parent_path();
		return skyrimDirectory / RE::GetINISetting("sLocalSavePath:General")->GetString();
	}

	void RenameSaveFiles(fs::path savePath, fs::path sksePath) {
		std::wstring newSaveName = stl::utf8_to_utf16(savePath.string()).value_or(L"");
		std::wstring newSkseName = stl::utf8_to_utf16(sksePath.string()).value_or(L"");
		fs::path newSavePath(newSaveName);
		fs::path newSksePath(newSkseName);
		if (!savePath.compare(newSavePath) && !sksePath.compare(newSksePath)) return;
		if (fs::exists(newSavePath)) fs::remove(newSavePath);
		if (fs::exists(newSksePath)) fs::remove(newSksePath);
		std::error_code saveError;
		std::error_code skseError;
		fs::rename(savePath, newSavePath, saveError);
		fs::rename(sksePath, newSksePath, skseError);
		if (saveError.value()) logs::info("saveError >> {}", saveError.message());
		if (skseError.value()) logs::info("skseError >> {}", skseError.message());
	}

	void EnableUpdate(std::string save, std::string skse) {
		SGEStorage::attempt = 0;
		SGEStorage::process = true;
		auto savesDirectory = GetSavesDirectory();
		SGEStorage::savePath = (savesDirectory / save);
		SGEStorage::sksePath = (savesDirectory / skse);
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
			logs::info("Save hook installed");
		}
	private:
		static const char* SaveGameSubMod(RE::BGSSaveLoadManager* a1, void* a2, const char* a3, void* a4, std::int32_t a5) {
			// do my stuff
			std::string save = std::format("{}.ess", a3);
			std::string skse = std::format("{}.skse", a3);
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
			logs::info("Update hook installed");
		}
	private:
		static void OnUpdateMod() {
			// do my stuff
			using namespace SGEStorage;
			if (process) {
				if (fs::exists(savePath) && fs::exists(sksePath)) {
					logs::info("RenameSaveFiles\n{}\n{}", savePath.string(), sksePath.string());
					SGEUtility::RenameSaveFiles(savePath, sksePath);
					SGEUtility::DisableUpdate();
				} else attempt += 1;
				if (attempt > 30) SGEUtility::DisableUpdate();
			}
			// call original function
			OnUpdate();
		}
		static inline REL::Relocation<decltype(OnUpdateMod)> OnUpdate;
	};

	void ReadMessage(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
			SaveHook::InstallHook();
			UpdateHook::InstallHook();
		}
	}

}

void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
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

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");

	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	
	SKSE::AllocTrampoline(1024);
	return InitMessaging();
}
