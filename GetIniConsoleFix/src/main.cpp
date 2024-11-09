template<typename Func>
auto WriteFunctionHook(std::uint64_t id, std::size_t byteCopyCount, Func destination) {
	const REL::Relocation target{ REL::ID(id) };
	struct XPatch: Xbyak::CodeGenerator {
		using ull = unsigned long long;
		using uch = unsigned char;
		XPatch(std::uintptr_t originalFuncAddr, ull originalByteLength, ull newByteLength):
		Xbyak::CodeGenerator(originalByteLength + newByteLength, TRAMPOLINE.allocate(originalByteLength + newByteLength)) {
			auto byteAddr = reinterpret_cast<uch*>(originalFuncAddr);
			for (ull i = 0; i < originalByteLength; i++) db(*byteAddr++);
			jmp(qword[rip]);
			dq(ull(byteAddr));
		}
	};
	XPatch patch(target.address(), byteCopyCount, 14);
	patch.ready();
	auto patchSize = patch.getSize();
	TRAMPOLINE.write_branch<5>(target.address(), destination);
	auto alloc = TRAMPOLINE.allocate(patchSize);
	std::memcpy(alloc, patch.getCode(), patchSize);
	return reinterpret_cast<std::uintptr_t>(alloc);
}

RE::Setting* LocateIniSetting(std::string str) {
	if (static auto base = RE::INISettingCollection::GetSingleton(); base) {
		if (auto baseSetting = base->GetSetting(str); baseSetting) return baseSetting;
		else if (static auto pref = RE::INIPrefSettingCollection::GetSingleton(); pref) {
			if (auto prefSetting = pref->GetSetting(str); prefSetting) return prefSetting;
		}
	}
	return nullptr;
}

template<typename T>
void PrintSetting(std::string arg, T val) {
	if (static auto conLog = RE::ConsoleLog::GetSingleton(); conLog) {
		auto msg = std::format("INISetting {} >> {}", arg, val);
		conLog->Print(msg.data());
	}
}

class GetSetHooks {
	static const std::uint64_t getID = 22033;
	static const std::uint64_t setID = 22034;
	std::vector<std::string> dataHolder;
private:
	static decltype(dataHolder)& GetStorage() {
		static GetSetHooks self;
		return self.dataHolder;
	}
	struct FunctionGet {
		static bool Modified(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj, RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr) {
			if (a_scriptData->numParams == 1) {
				if (auto stringChunk = a_scriptData->GetStringChunk(); stringChunk) {
					if (auto argument = stringChunk->GetString(); argument.size() > 0) {
						if (auto setting = LocateIniSetting(argument); setting) {
							switch (argument[0]) {
								case 'u':
								case 'U':
									PrintSetting(argument, setting->GetUInt());
									return true;
								case 's':
								case 'S':
									PrintSetting(argument, setting->GetString());
									return true;
							}
						}
					}
				}
			}
			return Original(a_paramInfo, a_scriptData, a_thisObj, a_containingObj, a_scriptObj, a_locals, a_result, a_opcodeOffsetPtr);
		}
		static inline REL::Relocation<decltype(Modified)> Original;
	};
	struct FunctionSet {
		static bool Modified(const RE::SCRIPT_PARAMETER* a_paramInfo, RE::SCRIPT_FUNCTION::ScriptData* a_scriptData, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* a_containingObj, RE::Script* a_scriptObj, RE::ScriptLocals* a_locals, double& a_result, std::uint32_t& a_opcodeOffsetPtr) {
			if (a_scriptData->numParams == 2) {
				if (auto stringChunk = a_scriptData->GetStringChunk(); stringChunk) {
					if (auto argument = stringChunk->GetString(); argument.size() > 0) {
						if (auto setting = LocateIniSetting(argument); setting) {
							switch (argument[0]) {
								case 's':
								case 'S':
									auto StoreNextValue = [](RE::SCRIPT_FUNCTION::StringChunk* chunk) {
										std::string data = std::string();
										try { data = chunk->GetNext()->AsString()->GetString(); }
										catch (...) { /* lmao, okay */ }
										GetStorage().push_back(data);
										return (GetStorage().size() - 1);
									};
									setting->data.s = GetStorage()[StoreNextValue(stringChunk)].data();
									PrintSetting(argument, setting->GetString());
									return true;
							}
						}
					}
				}
			}
			return Original(a_paramInfo, a_scriptData, a_thisObj, a_containingObj, a_scriptObj, a_locals, a_result, a_opcodeOffsetPtr);
		}
		static inline REL::Relocation<decltype(Modified)> Original;
	};
public:
	static bool Install() {
		// get
		bool getMatch = REL::make_pattern<"48 81 EC 48 02 00 00">().match(REL::ID(getID).address());
		if (getMatch) FunctionGet::Original = WriteFunctionHook(getID, 7, FunctionGet::Modified);
		else logs::info("REL::ID({}) pattern doesn't match (Get)", getID);
		// set
		bool setMatch = REL::make_pattern<"48 81 EC 58 04 00 00">().match(REL::ID(setID).address());
		if (setMatch) FunctionSet::Original = WriteFunctionHook(setID, 7, FunctionSet::Modified);
		else logs::info("REL::ID({}) pattern doesn't match (Set)", setID);
		// done
		return (getMatch && setMatch);
	}
};

void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
		SKSE::AllocTrampoline(128);
		logs::info("Get-Set Hooks {}", GetSetHooks::Install() ? "Installed" : "Failed");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, true);

	const auto gameVersion = a_skse->RuntimeVersion();
	logs::info(
		"Skyrim SE/AE v{}",
		gameVersion.string("-")
	);

	if (gameVersion.patch() < 1130) {
		SKSE::stl::report_and_fail("Patch version 1130 or higher is required");
	}

	const auto messagingInterface = SKSE::GetMessagingInterface();
	if (messagingInterface && messagingInterface->RegisterListener(MessageListener)) {
		logs::info("Message listener registered");
		return true;
	}
	logs::info("Could not register message listener");
	return false;
}
