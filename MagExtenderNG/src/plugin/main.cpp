namespace MENGUtility {
	std::string logLevelI = "[info]";
	std::string logLevelW = "[warning]";
	std::string logLevelE = "[error]";
	std::string logLevelF = "[fatal]";

	std::vector<std::string> IndexedAVs = {
		"Aggression",
		"Confidence",
		"Energy",
		"Morality",
		"Mood",
		"Assistance",
		"OneHanded",
		"TwoHanded",
		"Marksman",
		"Block",
		"Smithing",
		"HeavyArmor",
		"LightArmor",
		"Pickpocket",
		"LockPicking",
		"Sneak",
		"Alchemy",
		"SpeechCraft",
		"Alteration",
		"Conjuration",
		"Destruction",
		"Illusion",
		"Restoration",
		"Enchanting",
		"Health",
		"Magicka",
		"Stamina",
		"HealRate",
		"MagickaRate",
		"StaminaRate",
		"SpeedMult",
		"InventoryWeight",
		"CarryWeight",
		"CritChance",
		"MeleeDamage",
		"UnarmedDamage",
		"Mass",
		"VoicePoints",
		"VoiceRate",
		"DamageResist",
		"PoisonResist",
		"FireResist",
		"ElectricResist",
		"FrostResist",
		"MagicResist",
		"NormalWeaponsResist",
		"PerceptionCondition",
		"EnduranceCondition",
		"LeftAttackCondition",
		"RightAttackCondition",
		"LeftMobilityCondition",
		"RightMobilityCondition",
		"BrainCondition",
		"Paralysis",
		"Invisibility",
		"NightEye",
		"DetectLifeRange",
		"WaterBreathing",
		"WaterWalking",
		"IgnoreCrippleLimbs",
		"Fame",
		"Infamy",
		"JumpingBonus",
		"WardPower",
		"EquippedItemCharge",
		"ArmorPerks",
		"ShieldPerks",
		"WardDeflection",
		"Variable01",
		"Variable02",
		"Variable03",
		"Variable04",
		"Variable05",
		"Variable06",
		"Variable07",
		"Variable08",
		"Variable09",
		"Variable10",
		"BowSpeedBonus",
		"FavorActive",
		"FavorsPerDay",
		"FavorsPerDayTimer",
		"EquippedStaffCharge",
		"AbsorbChance",
		"Blindness",
		"WeaponSpeedMult",
		"ShoutRecoveryMult",
		"BowStaggerBonus",
		"Telekinesis",
		"FavorPointsBonus",
		"LastBribedIntimidated",
		"LastFlattered",
		"Muffled",
		"BypassVendorStolenCheck",
		"BypassVendorKeywordCheck",
		"WaitingForPlayer",
		"OneHandedMod",
		"TwoHandedMod",
		"MarksmanMod",
		"BlockMod",
		"SmithingMod",
		"HeavyArmorMod",
		"LightArmorMod",
		"PickPocketMod",
		"LockPickingMod",
		"SneakMod",
		"AlchemyMod",
		"SpeechcraftMod",
		"AlterationMod",
		"ConjurationMod",
		"DestructionMod",
		"IllusionMod",
		"RestorationMod",
		"EnchantingMod",
		"OneHandedSkillAdvance",
		"TwoHandedSkillAdvance",
		"MarksmanSkillAdvance",
		"BlockSkillAdvance",
		"SmithingSkillAdvance",
		"HeavyArmorSkillAdvance",
		"LightArmorSkillAdvance",
		"PickPocketSkillAdvance",
		"LockPickingSkillAdvance",
		"SneakSkillAdvance",
		"AlchemySkillAdvance",
		"SpeechcraftSkillAdvance",
		"AlterationSkillAdvance",
		"ConjurationSkillAdvance",
		"DestructionSkillAdvance",
		"IllusionSkillAdvance",
		"RestorationSkillAdvance",
		"EnchantingSkillAdvance",
		"LeftWeaponSpeedMult",
		"DragonSouls",
		"CombatHealthRegenMult",
		"OneHandedPowerMod",
		"TwoHandedPowerMod",
		"MarksmanPowerMod",
		"BlockPowerMod",
		"SmithingPowerMod",
		"HeavyArmorPowerMod",
		"LightArmorPowerMod",
		"PickPocketPowerMod",
		"LockPickingPowerMod",
		"SneakPowerMod",
		"AlchemyPowerMod",
		"SpeechcraftPowerMod",
		"AlterationPowerMod",
		"ConjurationPowerMod",
		"DestructionPowerMod",
		"IllusionPowerMod",
		"RestorationPowerMod",
		"EnchantingPowerMod",
		"DragonRend",
		"AttackDamageMult",
		"CombatHealthRegenMultMod",
		"CombatHealthRegenMultPowerMod",
		"StaminaRateMult",
		"HealRatePowerMod",
		"MagickaRateMod",
		"GrabActorOffset",
		"Grabbed",
		"AVIF162", //DEPRECATED
		"ReflectDamage"
	};

	std::vector<std::string> IndexedArchetypes = {
		"ValueMod",
		"Script",
		"Dispel",
		"CureDisease",
		"Absorb",
		"DualValueMod",
		"Calm",
		"Demoralize",
		"Frenzy",
		"Disarm",
		"CommandSummoned",
		"Invisibility",
		"Light",
		"Darkness",
		"NightEye",
		"Lock",
		"Open",
		"BoundWeapon",
		"SummonCreature",
		"DetectLife",
		"Telekinesis",
		"Paralysis",
		"Reanimate",
		"SoulTrap",
		"TurnUndead",
		"Guide",
		"WerewolfFeed",
		"CureParalysis",
		"CureAddiction",
		"CurePoison",
		"Concussion",
		"ValueAndParts",
		"AccumulateMagnitude",
		"Stagger",
		"PeakValueMod",
		"Cloak",
		"Werewolf",
		"SlowTime",
		"Rally",
		"EnhanceWeapon",
		"SpawnHazard",
		"Etherealize",
		"Banish",
		"SpawnScriptedRef",
		"Disguise",
		"GrabActor",
		"VampireLord"
	};

	int32_t GetIndex(std::vector<std::string> vec, std::string val)
	{
		auto it = std::find(vec.begin(), vec.end(), val);
		if (it != vec.end()) {
			return (int32_t)(it - vec.begin());
		}
		return -1;
	}

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

namespace MENGInternal {
	std::string MGEFStringGetter(RE::EffectSetting* theEffect, uint8_t option) {
		if (theEffect) {
			std::string result = "";
			switch (option) {
			case 1: { //GetMGEFPrimaryAV
				using AV = RE::ActorValue;
				auto primAV = theEffect->data.primaryAV;
				if (primAV != AV::kNone) {
					auto index = static_cast<std::underlying_type_t<AV>>(primAV);
					result = MENGUtility::IndexedAVs[index];
				}
				break;
			}
			case 2: { //GetMGEFSecondaryAV
				using AV = RE::ActorValue;
				auto secAV = theEffect->data.secondaryAV;
				if (secAV != AV::kNone) {
					auto index = static_cast<std::underlying_type_t<AV>>(secAV);
					result = MENGUtility::IndexedAVs[index];
				}
				break;
			}
			case 3: { //GetMGEFArchetype
				using AID = RE::EffectArchetypes::ArchetypeID;
				auto arch = theEffect->data.archetype;
				if (arch != AID::kNone) {
					auto index = static_cast<std::underlying_type_t<AID>>(arch);
					result = MENGUtility::IndexedArchetypes[index];
				}
				break;
			}
			}
			return result;
		}
		return "";
	}

	int32_t MGEFIntGetter(RE::EffectSetting* theEffect, uint8_t option) {
		if (theEffect) {
			int32_t result = -1;
			switch (option) {
			case 1: { //GetMGEFArchetypeNum
				using AID = RE::EffectArchetypes::ArchetypeID;
				auto arch = theEffect->data.archetype;
				if (arch != AID::kNone) {
					result = static_cast<std::underlying_type_t<AID>>(arch);
				}
				break;
			}
			}
			return result;
		}
		return -1;
	}

	float MGEFFloatGetter(RE::EffectSetting* theEffect, uint8_t option) {
		if (theEffect) {
			float result = 0.0F;
			switch (option) {
			case 1: //GetMGEFTaperDuration
				result = theEffect->data.taperDuration;
				break;
			case 2: //GetMGEFTaperWeight
				result = theEffect->data.taperWeight;
				break;
			case 3: //GetMGEFTaperCurve
				result = theEffect->data.taperCurve;
				break;
			case 4: //GetMGEFSecondaryAVWeight
				result = theEffect->data.secondAVWeight;
				break;
			}
			return result;
		}
		return 0.0F;
	}

	RE::TESForm* MGEFFormGetter(RE::EffectSetting* theEffect, uint8_t option) {
		if (theEffect) {
			RE::TESForm* result = nullptr;
			switch (option) {
			case 1: //GetMGEFRelatedForm
				result = theEffect->data.associatedForm;
				break;
			}
			return result;
		}
		return nullptr;
	}

	bool MGEFStringSetter(RE::EffectSetting* theEffect, uint8_t option, std::string value) {
		if (theEffect) {
			switch (option) {
			case 1: { //SetMGEFPrimaryAV
				using AV = RE::ActorValue;
				auto index = MENGUtility::GetIndex(MENGUtility::IndexedAVs, value);
				theEffect->data.primaryAV = (index < 0 ? AV::kNone : (AV)index);
				return true;
			}
			case 2: { //SetMGEFSecondaryAV
				using AV = RE::ActorValue;
				auto index = MENGUtility::GetIndex(MENGUtility::IndexedAVs, value);
				theEffect->data.secondaryAV = (index < 0 ? AV::kNone : (AV)index);
				return true;
			}
			case 3: { //SetMGEFArchetype(Native)
				using AID = RE::EffectArchetypes::ArchetypeID;
				auto index = MENGUtility::GetIndex(MENGUtility::IndexedArchetypes, value);
				theEffect->data.archetype = (index < 0 ? AID::kNone : (AID)index);
				return true;
			}
			}
			return false;
		}
		return false;
	}

	bool MGEFFloatSetter(RE::EffectSetting* theEffect, uint8_t option, float value) {
		if (theEffect) {
			switch (option) {
			case 1: //SetMGEFTaperDuration
				theEffect->data.taperDuration = (value < 0.0F ? 0.0F : value);
				return true;
			case 2: //SetMGEFTaperWeight
				theEffect->data.taperWeight = value;
				return true;
			case 3: //SetMGEFTaperCurve
				theEffect->data.taperCurve = value;
				return true;
			case 4: //SetMGEFSecondaryAVWeight
				theEffect->data.secondAVWeight = value;
				return true;
			}
			return false;
		}
		return false;
	}

	bool MGEFFormSetter(RE::EffectSetting* theEffect, uint8_t option, RE::TESForm* value) {
		if (theEffect) {
			switch (option) {
			case 1: //SetMGEFRelatedForm
				theEffect->data.associatedForm = value;
				return true;
			}
			return false;
		}
		return false;
	}

	bool AMEFloatSetter(RE::ActiveEffect* theEffect, uint8_t option, float value) {
		if (theEffect) {
			switch (option) {
			case 1: //SetActiveEffectDuration
				theEffect->duration = (value < 0.0F ? 0.0F : value);
				return true;
			case 2: //SetActiveEffectMagnitude
				theEffect->magnitude = (value < 0.0F ? 0.0F : value);
			}
			return false;
		}
		return false;
	}
}

namespace MENGPapyrus {
	//mgef-links-get
	//string
	std::string GetMGEFPrimaryAV(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFStringGetter(theEffect, 1);
	}
	std::string GetMGEFSecondaryAV(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFStringGetter(theEffect, 2);
	}
	std::string GetMGEFArchetype(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFStringGetter(theEffect, 3);
	}
	//int
	int32_t GetMGEFArchetypeNum(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFIntGetter(theEffect, 1);
	}
	//float
	float GetMGEFTaperDuration(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFFloatGetter(theEffect, 1);
	}
	float GetMGEFTaperWeight(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFFloatGetter(theEffect, 2);
	}
	float GetMGEFTaperCurve(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFFloatGetter(theEffect, 3);
	}
	float GetMGEFSecondaryAVWeight(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFFloatGetter(theEffect, 4);
	}
	//form
	RE::TESForm* GetMGEFRelatedForm(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFFormGetter(theEffect, 1);
	}

	//mgef-links-set
	//string
	bool SetMGEFPrimaryAV(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, std::string value) {
		reinterpret_cast<std::uint64_t>(base);
		return MENGInternal::MGEFStringSetter(theEffect, 1, value);
	}
	bool SetMGEFSecondaryAV(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, std::string value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFStringSetter(theEffect, 2, value);
	}
	bool SetMGEFArchetypeNative(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, std::string value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFStringSetter(theEffect, 3, value);
	}
	//float
	bool SetMGEFTaperDuration(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFFloatSetter(theEffect, 1, value);
	}
	bool SetMGEFTaperWeight(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFFloatSetter(theEffect, 2, value);
	}
	bool SetMGEFTaperCurve(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFFloatSetter(theEffect, 3, value);
	}
	bool SetMGEFSecondaryAVWeight(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFFloatSetter(theEffect, 4, value);
	}
	//form
	bool SetMGEFRelatedForm(RE::StaticFunctionTag* base, RE::EffectSetting* theEffect, RE::TESForm* value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::MGEFFormSetter(theEffect, 1, value);
	}

	//ame-links-set
	//float
	bool SetActiveEffectDuration(RE::StaticFunctionTag* base, RE::ActiveEffect* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::AMEFloatSetter(theEffect, 1, value);
	}
	bool SetActiveEffectMagnitude(RE::StaticFunctionTag* base, RE::ActiveEffect* theEffect, float value) {
		reinterpret_cast<std::uint64_t>(base); 
		return MENGInternal::AMEFloatSetter(theEffect, 2, value);
	}

	// registerer
	bool Register(RE::BSScript::IVirtualMachine* virtualMachine) {
		virtualMachine->RegisterFunction("GetMGEFPrimaryAV", "MagExtend", MENGPapyrus::GetMGEFPrimaryAV);
		virtualMachine->RegisterFunction("GetMGEFSecondaryAV", "MagExtend", MENGPapyrus::GetMGEFSecondaryAV);
		virtualMachine->RegisterFunction("GetMGEFArchetype", "MagExtend", MENGPapyrus::GetMGEFArchetype);
		virtualMachine->RegisterFunction("GetMGEFArchetypeNum", "MagExtend", MENGPapyrus::GetMGEFArchetypeNum);
		virtualMachine->RegisterFunction("GetMGEFTaperDuration", "MagExtend", MENGPapyrus::GetMGEFTaperDuration);
		virtualMachine->RegisterFunction("GetMGEFTaperWeight", "MagExtend", MENGPapyrus::GetMGEFTaperWeight);
		virtualMachine->RegisterFunction("GetMGEFTaperCurve", "MagExtend", MENGPapyrus::GetMGEFTaperCurve);
		virtualMachine->RegisterFunction("GetMGEFSecondaryAVWeight", "MagExtend", MENGPapyrus::GetMGEFSecondaryAVWeight);
		virtualMachine->RegisterFunction("GetMGEFRelatedForm", "MagExtend", MENGPapyrus::GetMGEFRelatedForm);
		virtualMachine->RegisterFunction("SetMGEFPrimaryAV", "MagExtend", MENGPapyrus::SetMGEFPrimaryAV);
		virtualMachine->RegisterFunction("SetMGEFSecondaryAV", "MagExtend", MENGPapyrus::SetMGEFSecondaryAV);
		virtualMachine->RegisterFunction("SetMGEFArchetypeNative", "MagExtend", MENGPapyrus::SetMGEFArchetypeNative);
		virtualMachine->RegisterFunction("SetMGEFTaperDuration", "MagExtend", MENGPapyrus::SetMGEFTaperDuration);
		virtualMachine->RegisterFunction("SetMGEFTaperWeight", "MagExtend", MENGPapyrus::SetMGEFTaperWeight);
		virtualMachine->RegisterFunction("SetMGEFTaperCurve", "MagExtend", MENGPapyrus::SetMGEFTaperCurve);
		virtualMachine->RegisterFunction("SetMGEFSecondaryAVWeight", "MagExtend", MENGPapyrus::SetMGEFSecondaryAVWeight);
		virtualMachine->RegisterFunction("SetMGEFRelatedForm", "MagExtend", MENGPapyrus::SetMGEFRelatedForm);
		virtualMachine->RegisterFunction("SetActiveEffectDuration", "MagExtend", MENGPapyrus::SetActiveEffectDuration);
		virtualMachine->RegisterFunction("SetActiveEffectMagnitude", "MagExtend", MENGPapyrus::SetActiveEffectMagnitude);
		return true;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	MENGUtility::InitLogger();

	auto plugin = SKSE::PluginDeclaration::GetSingleton();
	auto gameVersion = REL::Module::get().version();
	logger::info("{} {} version {} is loading into {}", MENGUtility::logLevelI, plugin->GetName(), plugin->GetVersion(), gameVersion);

	SKSE::Init(a_skse);

	auto papyrusInterface = SKSE::GetPapyrusInterface();
	if (papyrusInterface) {
		if (papyrusInterface->Register(MENGPapyrus::Register)) {
			logger::info("{} Papyrus functions registered", MENGUtility::logLevelI);
			return true;
		}
		logger::info("{} Could not register Papyrus functions", MENGUtility::logLevelF);
		return false;
	}
	logger::info("{} Papyrus interface not found", MENGUtility::logLevelF);
	return false;
}
