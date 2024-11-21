template<typename Func>
auto WriteFunctionHook(std::uint64_t id, std::size_t copyCount, Func destination) {
	const auto target = REL::ID(id).address();
	struct XPatch: Xbyak::CodeGenerator {
		using ull = unsigned long long;
		using uch = unsigned char;
		uch workspace[64];
		XPatch(std::uintptr_t baseAddress, ull bytesCount): Xbyak::CodeGenerator(bytesCount + 14, workspace) {
			auto bytePtr = reinterpret_cast<uch*>(baseAddress);
			for (ull i = 0; i < bytesCount; i++) db(*bytePtr++);
			jmp(qword[rip]);
			dq(ull(bytePtr));
		}
	};
	XPatch patch(target, copyCount);
	patch.ready();
	auto patchSize = patch.getSize();
	TRAMPOLINE.write_branch<5>(target, destination);
	auto alloc = TRAMPOLINE.allocate(patchSize);
	std::memcpy(alloc, patch.getCode(), patchSize);
	return reinterpret_cast<std::uintptr_t>(alloc);
}

RE::Setting* LocateIniSetting(std::string str) {
	if (static auto base = RE::INISettingCollection::GetSingleton(); base)
		if (auto baseSetting = base->GetSetting(str); baseSetting)
			return baseSetting;
	if (static auto pref = RE::INIPrefSettingCollection::GetSingleton(); pref)
		if (auto prefSetting = pref->GetSetting(str); prefSetting)
			return prefSetting;
	return nullptr;
}

template<typename T>
void PrintIniSetting(std::string arg, T val) {
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
			if (a_scriptData && a_scriptData->numParams == 1) {
				if (auto stringChunk = a_scriptData->GetStringChunk(); stringChunk) {
					if (auto argument = stringChunk->GetString(); argument.size() > 0) {
						if (auto setting = LocateIniSetting(argument); setting) {
							switch (argument[0]) {
								case 'u':
								case 'U':
									PrintIniSetting(argument, setting->GetUInt());
									return true;
								case 's':
								case 'S':
									PrintIniSetting(argument, setting->GetString());
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
			if (a_scriptData && a_scriptData->numParams == 2) {
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
									PrintIniSetting(argument, setting->GetString());
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
		auto getAddress = REL::ID(getID).address();
		bool getMatchJump = REL::make_pattern<"E9">().match(getAddress);
		bool getMatchCustom = REL::make_pattern<"48 81 EC 48 02 00 00">().match(getAddress);
		auto getInfo = std::format("REL::ID({}) function (Get)", getID);
		if (getMatchJump) {
			// my hook is not the first
			FunctionGet::Original = TRAMPOLINE.write_branch<5>(getAddress, FunctionGet::Modified);
			logs::info("{} / Jump hook installed", getInfo);
		} else if (getMatchCustom) {
			// my hook is the first
			FunctionGet::Original = WriteFunctionHook(getID, 7, FunctionGet::Modified);
			logs::info("{} / Custom hook installed", getInfo);
		} else {
			logs::info("{} / Failed to match any pattern", getInfo);
		}
		// set
		auto setAddress = REL::ID(setID).address();
		bool setMatchJump = REL::make_pattern<"E9">().match(setAddress);
		bool setMatchCustom = REL::make_pattern<"48 81 EC 58 04 00 00">().match(setAddress);
		auto setInfo = std::format("REL::ID({}) function (Set)", setID);
		if (setMatchJump) {
			// my hook is not the first
			FunctionSet::Original = TRAMPOLINE.write_branch<5>(setAddress, FunctionSet::Modified);
			logs::info("{} / Jump hook installed", setInfo);
		} else if (setMatchCustom) {
			// my hook is the first
			FunctionSet::Original = WriteFunctionHook(setID, 7, FunctionSet::Modified);
			logs::info("{} / Custom hook installed", setInfo);
		} else {
			logs::info("{} / Failed to match any pattern", setInfo);
		}
		// done
		return (getMatchJump || getMatchCustom) && (setMatchJump || setMatchCustom);
	}
};

void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
		SKSE::AllocTrampoline(128);
		logs::info("Get-Set Hooks {}", GetSetHooks::Install() ? "Installed" : "Failed");
		logs::info("Total {} Bytes Allocated", TRAMPOLINE.allocated_size());
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
