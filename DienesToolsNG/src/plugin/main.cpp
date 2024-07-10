namespace DTNGUtility {
	std::string logLevelI = "[info]";
	std::string logLevelW = "[warning]";
	std::string logLevelE = "[error]";
	std::string logLevelF = "[fatal]";

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

	fs::path GetLogsDirectory() {
		fs::path gamesDirectory = GetDocumentsDirectory() / "My Games";
		fs::path skyrimDirectory;
		if (REL::Module::IsVR()) {
			skyrimDirectory = gamesDirectory / "Skyrim VR";
		} else {
			auto steam = fs::path("steam_api64.dll");
			auto special = fs::exists(steam) ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			skyrimDirectory = gamesDirectory / special;
		}
		fs::path logsDirectory = skyrimDirectory / "SKSE";
		return logsDirectory;
	}

	void InitLogger() {
		std::optional<fs::path> path = GetLogsDirectory();
		if (!path) return;
		auto plugin = SKSE::PluginDeclaration::GetSingleton();
		*path /= fmt::format("{}.log", plugin->GetName());
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
}

namespace DTNGInternal {
	void GetAllCOBJThatYieldForm(RE::TESForm* theForm, RE::BGSListForm* theFormList) {
		if (theForm && theFormList) {
			auto theFormID = theForm->GetFormID();
			auto theDataHandler = RE::TESDataHandler::GetSingleton();
			RE::BSTArray<RE::BGSConstructibleObject*> theCOBJs = theDataHandler->GetFormArray<RE::BGSConstructibleObject>();
			for (auto theCOBJ : theCOBJs) {
				auto theCreated = theCOBJ->createdItem;
				if (theCreated && theCreated->GetFormID() == theFormID) {
					theFormList->AddForm(theCOBJ);
				}
			}
		} else {
			logger::info("{} NULL TESForm or NULL BGSListForm", DTNGUtility::logLevelE);
		}
	}
	RE::BGSConstructibleObject* TempCOBJ(RE::BGSConstructibleObject* toCopy) {
		RE::BGSConstructibleObject* result = nullptr;
		if (toCopy) {
			auto factory = RE::IFormFactory::GetFormFactoryByType(toCopy->GetFormType());
			if (factory) {
				result = (RE::BGSConstructibleObject*)factory->Create();
				if (result) {
					result->InitItem();
					// copy all members
					result->requiredItems.CopyComponent(&toCopy->requiredItems); // 20
					result->conditions = toCopy->conditions;                     // 38
					result->createdItem = toCopy->createdItem;                   // 40 - CNAM
					result->benchKeyword = toCopy->benchKeyword;                 // 48 - BNAM
					result->data = toCopy->data;                                 // 50
					result->pad52 = toCopy->pad52;                               // 52
					result->unk54 = toCopy->unk54;                               // 54
				} else {
					logger::info("{} Factory for type {} failed", DTNGUtility::logLevelE, toCopy->GetFormType());
				}
			} else {
				logger::info("{} Tried to clone form {} (type {}), no factory found", DTNGUtility::logLevelE, toCopy->GetFormID(), toCopy->GetFormType());
			}
		}
		return result;
	}
	void ReplaceKYWD(RE::TESForm* theForm, RE::BGSKeyword* toRemove, RE::BGSKeyword* toAdd) {
		if (theForm && toRemove && toAdd) {
			RE::BGSKeywordForm* pKeywords = skyrim_cast<RE::BGSKeywordForm*>(theForm);
			if (pKeywords && pKeywords->HasKeyword(toRemove) && !pKeywords->HasKeyword(toAdd)) {
				auto index = pKeywords->GetKeywordIndex(toRemove).value();
				pKeywords->keywords[index] = toAdd;
			}
		} else {
			logger::info("{} NULL TESForm or NULL BGSKeyword", DTNGUtility::logLevelE);
		}
	}
}

namespace DTNGPapyrus {
	// links
	void GetAllCOBJThatYieldForm(RE::StaticFunctionTag* base, RE::TESForm* theForm, RE::BGSListForm* theFormList) {
		reinterpret_cast<std::uintptr_t>(base);
		return DTNGInternal::GetAllCOBJThatYieldForm(theForm, theFormList);
	}
	RE::BGSConstructibleObject* TempCOBJ(RE::StaticFunctionTag* base, RE::BGSConstructibleObject* toCopy) {
		reinterpret_cast<std::uintptr_t>(base);
		return DTNGInternal::TempCOBJ(toCopy);
	}
	void ReplaceKYWD(RE::StaticFunctionTag* base, RE::TESForm* theForm, RE::BGSKeyword* toRemove, RE::BGSKeyword* toAdd) {
		reinterpret_cast<std::uintptr_t>(base);
		return DTNGInternal::ReplaceKYWD(theForm, toRemove, toAdd);
	}
	// registerer
	bool Register(RE::BSScript::IVirtualMachine* virtualMachine) {
		virtualMachine->RegisterFunction("GetAllCOBJThatYieldForm", "DienesToolsPluginScript", DTNGPapyrus::GetAllCOBJThatYieldForm);
		virtualMachine->RegisterFunction("TempCOBJ", "DienesToolsPluginScript", DTNGPapyrus::TempCOBJ);
		virtualMachine->RegisterFunction("ReplaceKYWD", "DienesToolsPluginScript", DTNGPapyrus::ReplaceKYWD);
		return true;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	DTNGUtility::InitLogger();

	auto plugin = SKSE::PluginDeclaration::GetSingleton();
	auto gameVersion = REL::Module::get().version();
	logger::info("{} {} version {} is loading into {}", DTNGUtility::logLevelI, plugin->GetName(), plugin->GetVersion(), gameVersion);

	SKSE::Init(a_skse);

	auto papyrusInterface = SKSE::GetPapyrusInterface();
	if (papyrusInterface) {
		if (papyrusInterface->Register(DTNGPapyrus::Register)) {
			logger::info("{} Papyrus functions registered", DTNGUtility::logLevelI);
			return true;
		}
		logger::info("{} Could not register Papyrus functions", DTNGUtility::logLevelF);
		return false;
	}
	logger::info("{} Papyrus interface not found", DTNGUtility::logLevelF);
	return false;
}
