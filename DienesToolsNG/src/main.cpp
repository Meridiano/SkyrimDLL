namespace DTNGUtility {
	std::string logLevelI = "[info]";
	std::string logLevelW = "[warning]";
	std::string logLevelE = "[error]";
	std::string logLevelF = "[fatal]";

	void InitLogger(std::string pattern) {
		logger::init();
		spdlog::set_pattern(pattern);
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
	SKSE::Init(a_skse, false);
	DTNGUtility::InitLogger("%d.%m.%Y %H:%M:%S [%s:%#] %v");

	auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logger::info(
		"{} {} version {} is loading into {}", DTNGUtility::logLevelI,
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);

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
