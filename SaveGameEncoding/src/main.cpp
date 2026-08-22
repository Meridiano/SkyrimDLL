namespace SGEStorage {

	std::uint8_t attempt = 0;
	fs::path savePath = "";
	fs::path sksePath = "";
	bool process = false;

}

namespace SGEUtility {

	fs::path GetSavesDirectory() {
		auto logDir = logs::log_directory();
		if (!logDir) stl::report_and_fail("Log path not found");
		return logDir->parent_path() / RE::GetINISetting("sLocalSavePath:General")->GetString();
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
		auto savesDirectory = GetSavesDirectory();
		SGEStorage::savePath = (savesDirectory / save);
		SGEStorage::sksePath = (savesDirectory / skse);
		SGEStorage::process = true;
	}

	void DisableUpdate() {
		SGEStorage::process = false;
		SGEStorage::savePath = "";
		SGEStorage::sksePath = "";
		SGEStorage::attempt = 0;
	}

}

namespace SGEProcess {

	class SaveHook {
	public:
		static void InstallHook() {
			REL::Relocation target{
				REL::VariantID(34818, 35727, 0x58E310),
				REL::VariantOffset(0x112, 0x1CE, 0x116)
			};
			SaveGameSub = target.write_call<5>(SaveGameSubMod);
			logs::info("Save hook installed");
		}
	private:
		static const char* SaveGameSubMod(RE::BGSSaveLoadManager* a1, void* a2, const char* a3, std::int64_t a4, std::int32_t a5) {
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
			std::uint32_t aeOffset = MODULE.version().minor() == 7 ? 0xC38 : 0xC26;
			REL::Relocation target{
				REL::VariantID(35565, 36564, 0x5BAB10),
				REL::VariantOffset(0x748, aeOffset, 0x7EE)
			};
			OnUpdate = target.write_call<5>(OnUpdateMod);
			logs::info("Update hook installed");
		}
	private:
		static void OnUpdateMod() {
			// do my stuff
			if (SGEStorage::process) {
				auto& it = SGEStorage::attempt;
				auto& p1 = SGEStorage::savePath;
				auto& p2 = SGEStorage::sksePath;
				if (fs::exists(p1) && fs::exists(p2)) {
					logs::info("RenameSaveFiles\n{}\n{}", p1.string(), p2.string());
					SGEUtility::RenameSaveFiles(p1, p2);
					SGEUtility::DisableUpdate();
				} else it += 1;
				if (it > 60) SGEUtility::DisableUpdate();
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
		MODULE.version().string(".")
	);
	
	SKSE::AllocTrampoline(32);
	return InitMessaging();
}
