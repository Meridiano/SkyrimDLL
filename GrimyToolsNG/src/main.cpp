void InitLogging(std::string pattern) {
	logs::init();
	spdlog::set_pattern(pattern);
}

namespace GPNGUtility {

	std::string logLevelI = "[info]";
	std::string logLevelW = "[warning]";
	std::string logLevelE = "[error]";
	std::string logLevelF = "[fatal]";
	
	bool IsInRange(int64_t value, int64_t min, int64_t max) {
		if (value < min) return false;
		if (value > max) return false;
		return true;
	}
	int64_t RestrictInt(int64_t value, int64_t min, int64_t max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

}

namespace GPNGInternal {
	// enchantments
	bool IsArmorEnch(RE::EnchantmentItem* theEnch) {
		bool constant = (theEnch->GetCastingType() == RE::MagicSystem::CastingType::kConstantEffect);
		bool self = (theEnch->GetDelivery() == RE::MagicSystem::Delivery::kSelf);
		return (constant && self);
	}
	uint32_t GetWeaponEnchCount() {
		uint32_t result = 0;
		auto theDataHandler = RE::TESDataHandler::GetSingleton();
		auto allEnch = theDataHandler->GetFormArray<RE::EnchantmentItem>();
		for (auto theEnch : allEnch) {
			result += theEnch ? (IsArmorEnch(theEnch) ? 0 : 1) : 0;
		}
		return result;
	}
	uint32_t GetArmorEnchCount() {
		uint32_t result = 0;
		auto theDataHandler = RE::TESDataHandler::GetSingleton();
		auto allEnch = theDataHandler->GetFormArray<RE::EnchantmentItem>();
		for (auto theEnch : allEnch) {
			result += theEnch ? (IsArmorEnch(theEnch) ? 1 : 0) : 0;
		}
		return result;
	}

	RE::TESObjectARMO* GetArmorTemplate(RE::TESObjectARMO* theArmor) {
		if (theArmor) return theArmor->templateArmor;
		return nullptr;
	}

	// light duration get-set
	int32_t GetLightDuration(RE::TESObjectLIGH* theLight) {
		return (theLight ? theLight->data.time : 0);
	}
	void SetLightDuration(RE::TESObjectLIGH* theLight, int32_t theDuration) {
		if (theLight) {
			theLight->data.time = (theDuration > 0 ? theDuration : -1);
		}
	}

	// movement get-set
	float GetMovementType(RE::TESForm* theForm, int32_t indexOne, int32_t indexTwo) {
		if (theForm) {
			auto theMovement = skyrim_cast<RE::BGSMovementType*>(theForm);
			if (theMovement && GPNGUtility::IsInRange(indexOne, 0, 4) && GPNGUtility::IsInRange(indexTwo, 0, 1)) {
				return theMovement->movementTypeData.defaultData.speeds[indexOne][indexTwo];
			}
		}
		return 0.0F;
	}
	void SetMovementType(RE::TESForm* theForm, int32_t indexOne, int32_t indexTwo, float value) {
		if (theForm) {
			auto theMovement = skyrim_cast<RE::BGSMovementType*>(theForm);
			if (theMovement && GPNGUtility::IsInRange(indexOne, 0, 4) && GPNGUtility::IsInRange(indexTwo, 0, 1) && value >= 0.0F) {
				theMovement->movementTypeData.defaultData.speeds[indexOne][indexTwo] = value;
			}
		}
	}

	// weather get-set
	uint32_t GetWeatherInt(RE::TESWeather* theWeather, uint32_t index) {
		if (theWeather) {
			uint8_t result8 = 0;
			uint32_t result32 = 0;
			bool convert = true;
			switch (index) {
				case 0:
					result8 = theWeather->data.windSpeed;
					break;
				case 1:
					result8 = theWeather->data.unk01;
					break;
				case 2:
					result8 = theWeather->data.unk02;
					break;
				case 3:
					result8 = theWeather->data.transDelta;
					break;
				case 4:
					result8 = theWeather->data.sunGlare;
					break;
				case 5:
					result8 = theWeather->data.sunDamage;
					break;
				case 6:
					result8 = theWeather->data.precipitationBeginFadeIn;
					break;
				case 7:
					result8 = theWeather->data.precipitationEndFadeOut;
					break;
				case 8:
					result8 = theWeather->data.thunderLightningBeginFadeIn;
					break;
				case 9:
					result8 = theWeather->data.thunderLightningEndFadeOut;
					break;
				case 10:
					result8 = theWeather->data.thunderLightningFrequency;
					break;
				case 11: {
					result8 = theWeather->data.flags.underlying();
					break;
				}
				case 12: {
					auto r = theWeather->data.lightningColor.red;
					auto g = theWeather->data.lightningColor.green;
					auto b = theWeather->data.lightningColor.blue;
					result32 = ((r < 0 ? 255 : r) * 65536 + (g < 0 ? 255 : g) * 256 + (b < 0 ? 255 : b) * 1);
					convert = false;
					break;
				}
				case 13:
					result8 = theWeather->data.visualEffectBegin;
					break;
				case 14:
					result8 = theWeather->data.visualEffectEnd;
					break;
				case 15:
					result8 = theWeather->data.windDirection;
					break;
				case 16:
					result8 = theWeather->data.windDirectionRange;
					break;
				case 17:
					result8 = theWeather->data.unk13;
					break;
			}
			if (convert) result32 = result8;
			return result32;
		}
		return 0;
	}
	void SetWeatherInt(RE::TESWeather* theWeather, uint32_t index, uint32_t value32) {
		if (theWeather) {
			auto value8 = (uint8_t)GPNGUtility::RestrictInt(value32, 0, 255);
			switch (index) {
				case 0:
					theWeather->data.windSpeed = value8;
					break;
				case 1:
					theWeather->data.unk01 = value8;
					break;
				case 2:
					theWeather->data.unk02 = value8;
					break;
				case 3:
					theWeather->data.transDelta = value8;
					break;
				case 4:
					theWeather->data.sunGlare = value8;
					break;
				case 5:
					theWeather->data.sunDamage = value8;
					break;
				case 6:
					theWeather->data.precipitationBeginFadeIn = value8;
					break;
				case 7:
					theWeather->data.precipitationEndFadeOut = value8;
					break;
				case 8:
					theWeather->data.thunderLightningBeginFadeIn = value8;
					break;
				case 9:
					theWeather->data.thunderLightningEndFadeOut = value8;
					break;
				case 10:
					theWeather->data.thunderLightningFrequency = value8;
					break;
				case 11: {
					using WDF = RE::TESWeather::WeatherDataFlag;
					theWeather->data.flags = WDF::kNone;
					bool bPleasant = value8 & (uint8_t)WDF::kPleasant;
					bool bCloudy = value8 & (uint8_t)WDF::kCloudy;
					bool bRainy = value8 & (uint8_t)WDF::kRainy;
					bool bSnow = value8 & (uint8_t)WDF::kSnow;
					bool bPermAurora = value8 & (uint8_t)WDF::kPermAurora;
					bool bAuroraFollowsSun = value8 & (uint8_t)WDF::kAuroraFollowsSun;
					theWeather->data.flags.set(
						(bPleasant ? WDF::kPleasant : WDF::kNone),
						(bCloudy ? WDF::kCloudy : WDF::kNone),
						(bRainy ? WDF::kRainy : WDF::kNone),
						(bSnow ? WDF::kSnow : WDF::kNone),
						(bPermAurora ? WDF::kPermAurora : WDF::kNone),
						(bAuroraFollowsSun ? WDF::kAuroraFollowsSun : WDF::kNone)
					);
					break;
				}
				case 12: {
					value32 = (uint32_t)GPNGUtility::RestrictInt(value32, 0, 0xFFFFFF);
					theWeather->data.lightningColor.red = (uint8_t)((value32 & 0xFF0000) >> 16);
					theWeather->data.lightningColor.green = (uint8_t)((value32 & 0x00FF00) >> 8);
					theWeather->data.lightningColor.blue = (uint8_t)((value32 & 0x0000FF) >> 0);
					break;
				}
				case 13:
					theWeather->data.visualEffectBegin = value8;
					break;
				case 14:
					theWeather->data.visualEffectEnd = value8;
					break;
				case 15:
					theWeather->data.windDirection = value8;
					break;
				case 16:
					theWeather->data.windDirectionRange = value8;
					break;
				case 17:
					theWeather->data.unk13 = value8;
					break;
			}
		}
	}

	void SetSpellNthMagicEffect(RE::SpellItem* theMagic, RE::EffectSetting* theEffect, uint32_t index) {
		if (theMagic && theEffect) {
			uint32_t maxIndex = theMagic->effects.size() - 1;
			if (GPNGUtility::IsInRange(index, 0, maxIndex)) {
				theMagic->effects[index]->baseEffect = theEffect;
			}
		}
	}

	// spell - getters
	int32_t GetSpellType(RE::SpellItem* theMagic) {
		if (theMagic) {
			auto spellType = theMagic->GetSpellType();
			return static_cast<std::underlying_type_t<RE::MagicSystem::SpellType>>(spellType);
		}
		return -1;
	}
	int32_t GetSpellCastType(RE::SpellItem* theMagic) {
		if (theMagic) {
			auto spellCastType = theMagic->GetCastingType();
			return static_cast<std::underlying_type_t<RE::MagicSystem::CastingType>>(spellCastType);
		}
		return -1;
	}
	int32_t GetSpellDelivery(RE::SpellItem* theMagic) {
		if (theMagic) {
			auto spellDelivery = theMagic->GetDelivery();
			return static_cast<std::underlying_type_t<RE::MagicSystem::Delivery>>(spellDelivery);
		}
		return -1;
	}
	float GetSpellChargeTime(RE::SpellItem* theMagic) {
		if (theMagic) {
			return theMagic->GetChargeTime();
		}
		return 0.0F;
	}
	float GetSpellRange(RE::SpellItem* theMagic) {
		if (theMagic) {
			return theMagic->GetRange();
		}
		return 0.0F;
	}

	// spell - setters
	void SetSpellType(RE::SpellItem* theMagic, uint32_t spellType) {
		if (theMagic && GPNGUtility::IsInRange(spellType, 0, 13)) {
			theMagic->data.spellType = RE::MagicSystem::SpellType(spellType);
		}
	}
	void SetSpellCastType(RE::SpellItem* theMagic, uint32_t spellCastType) {
		if (theMagic && GPNGUtility::IsInRange(spellCastType, 0, 3)) {
			theMagic->data.castingType = RE::MagicSystem::CastingType(spellCastType);
		}
	}
	void SetSpellCastTime(RE::SpellItem* theMagic, float spellCastTime) {
		if (theMagic && spellCastTime >= 0.0F) {
			theMagic->data.castDuration = spellCastTime;
		}
	}
	void SetSpellChargeTime(RE::SpellItem* theMagic, float spellChargeTime) {
		if (theMagic && spellChargeTime >= 0.0F) {
			theMagic->data.chargeTime = spellChargeTime;
		}
	}
	void SetSpellDelivery(RE::SpellItem* theMagic, uint32_t spellDelivery) {
		if (theMagic && GPNGUtility::IsInRange(spellDelivery, 0, 4)) {
			theMagic->data.delivery = RE::MagicSystem::Delivery(spellDelivery);
		}
	}
	void SetSpellPerk(RE::SpellItem* theMagic, RE::BGSPerk* thePerk) {
		if (theMagic && thePerk) {
			theMagic->data.castingPerk = thePerk;
		}
	}
	void SetMagickaCost(RE::SpellItem* theMagic, int32_t magickaCost) {
		if (theMagic && magickaCost >= 0) {
			theMagic->data.costOverride = magickaCost;
		}
	}

	// spell - merger
	int32_t MergeSpells(RE::SpellItem* outputSpell, std::vector<RE::SpellItem*> inputSpells, std::vector<float> magMults, std::vector<float> durMults, std::vector<float> costMults, float timeMult) {
		// check for sizes
		auto isCount = inputSpells.size();
		auto mmCount = magMults.size();
		auto dmCount = durMults.size();
		auto cmCount = costMults.size();
		if (isCount == 0 || mmCount == 0 || dmCount == 0 || cmCount == 0) return -4;
		if (isCount != mmCount || mmCount != dmCount || dmCount != cmCount) return -4;
		// check for input effects count
		if (outputSpell) {
			size_t maxCount = outputSpell->effects.size();
			size_t inputCount = 0;
			for (size_t index = 0; index < isCount; index++) {
				inputCount += inputSpells[index]->effects.size();
				if (inputCount > maxCount) return -3;
			}
		} else return -3;
		// check for delivery type
		auto compDelivery = inputSpells[0]->data.delivery;
		for (size_t index = 1; index < isCount; index++) {
			if (inputSpells[index]->data.delivery != compDelivery) return -2;
		}
		// check for cast type
		auto compCast = inputSpells[0]->data.castingType;
		for (size_t index = 1; index < isCount; index++) {
			if (inputSpells[index]->data.castingType != compCast) return -1;
		}
		// merging
		RE::BSTArrayBase::size_type outIndex = 0;
		float mgefMult = (compCast == RE::MagicSystem::CastingType::kConstantEffect) ? 1.0F : timeMult;
		for (size_t inIndex = 0; inIndex < isCount; inIndex++) {
			auto inSpell = inputSpells[inIndex];
			auto magMult = magMults[inIndex];
			auto durMult = durMults[inIndex];
			auto costMult = costMults[inIndex];
			for (auto inMGEF : inSpell->effects) {
				// get target effect
				auto outMGEF = outputSpell->effects[outIndex];
				// calculate target values
				auto mag = inMGEF->GetMagnitude() * magMult * mgefMult;
				auto dur = uint32_t(inMGEF->GetDuration() * durMult);
				auto cost = inMGEF->cost * costMult * mgefMult;
				// set
				outMGEF->baseEffect = inMGEF->baseEffect;
				outMGEF->effectItem.magnitude = mag;
				outMGEF->effectItem.duration = dur;
				outMGEF->cost = cost;
				// increase iterator
				outIndex += 1;
			}
		}
		// additions
		outputSpell->data.delivery = compDelivery;
		outputSpell->data.castingType = compCast;
		SetSpellChargeTime(outputSpell, outputSpell->data.chargeTime * timeMult);
		// check magic school
		for (auto outMGEF : outputSpell->effects) {
			if (outMGEF->baseEffect->data.associatedSkill != RE::ActorValue::kNone) return 0;
		}
		return 1;
	}
}

namespace GPNGPapyrus {
	// enchantments
	uint32_t GetWeaponEnchCount(RE::StaticFunctionTag* base) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetWeaponEnchCount();
	}
	uint32_t GetArmorEnchCount(RE::StaticFunctionTag* base) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetArmorEnchCount();
	}

	RE::TESObjectARMO* GetArmorTemplate(RE::StaticFunctionTag* base, RE::TESObjectARMO* theArmor) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetArmorTemplate(theArmor);
	}

	// light
	int32_t GetLightDuration(RE::StaticFunctionTag* base, RE::TESObjectLIGH* theLight) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetLightDuration(theLight);
	}
	void SetLightDuration(RE::StaticFunctionTag* base, RE::TESObjectLIGH* theLight, int32_t theDuration) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetLightDuration(theLight, theDuration);
	}

	// movement
	float GetMovementType(RE::StaticFunctionTag* base, RE::TESForm* theForm, int32_t indexOne, int32_t indexTwo) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetMovementType(theForm, indexOne, indexTwo);
	}
	void SetMovementType(RE::StaticFunctionTag* base, RE::TESForm* theForm, int32_t indexOne, int32_t indexTwo, float value) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetMovementType(theForm, indexOne, indexTwo, value);
	}

	// weather
	uint32_t GetWeatherInt(RE::StaticFunctionTag* base, RE::TESWeather* theWeather, uint32_t index) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetWeatherInt(theWeather, index);
	}
	void SetWeatherInt(RE::StaticFunctionTag* base, RE::TESWeather* theWeather, uint32_t index, uint32_t value) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetWeatherInt(theWeather, index, value);
	}

	void SetSpellNthMagicEffect(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, RE::EffectSetting* theEffect, uint32_t index) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellNthMagicEffect(theMagic, theEffect, index);
	}

	// spell-get
	int32_t GetSpellType(RE::StaticFunctionTag* base, RE::SpellItem* theMagic) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetSpellType(theMagic);
	}
	int32_t GetSpellCastType(RE::StaticFunctionTag* base, RE::SpellItem* theMagic) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetSpellCastType(theMagic);
	}
	int32_t GetSpellDelivery(RE::StaticFunctionTag* base, RE::SpellItem* theMagic) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetSpellDelivery(theMagic);
	}
	float GetSpellChargeTime(RE::StaticFunctionTag* base, RE::SpellItem* theMagic) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetSpellChargeTime(theMagic);
	}
	float GetSpellRange(RE::StaticFunctionTag* base, RE::SpellItem* theMagic) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::GetSpellRange(theMagic);
	}

	// spell-set
	void SetSpellType(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, uint32_t spellType) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellType(theMagic, spellType);
	}
	void SetSpellCastType(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, uint32_t spellCastType) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellCastType(theMagic, spellCastType);
	}
	void SetSpellCastTime(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, float spellCastTime) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellCastTime(theMagic, spellCastTime);
	}
	void SetSpellChargeTime(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, float spellChargeTime) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellChargeTime(theMagic, spellChargeTime);
	}
	void SetSpellDelivery(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, uint32_t spellDelivery) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellDelivery(theMagic, spellDelivery);
	}
	void SetSpellPerk(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, RE::BGSPerk* thePerk) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetSpellPerk(theMagic, thePerk);
	}
	void SetMagickaCost(RE::StaticFunctionTag* base, RE::SpellItem* theMagic, int32_t magickaCost) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::SetMagickaCost(theMagic, magickaCost);
	}

	// spell-merge
	int32_t MergeSpells(RE::StaticFunctionTag* base, RE::SpellItem* outputSpell, std::vector<RE::SpellItem*> inputSpells, std::vector<float> magMults, std::vector<float> durMults, std::vector<float> costMults, float timeMult) {
		reinterpret_cast<std::uint64_t>(base);
		return GPNGInternal::MergeSpells(outputSpell, inputSpells, magMults, durMults, costMults, timeMult);
	}

	// registerer
	bool Register(RE::BSScript::IVirtualMachine* virtualMachine) {
		std::string className = "GrimyToolsPluginScript";
		virtualMachine->RegisterFunction("GetWeaponEnchCount", className, GPNGPapyrus::GetWeaponEnchCount);
		virtualMachine->RegisterFunction("GetArmorEnchCount", className, GPNGPapyrus::GetArmorEnchCount);
		virtualMachine->RegisterFunction("GetArmorTemplate", className, GPNGPapyrus::GetArmorTemplate);
		virtualMachine->RegisterFunction("GetLightDuration", className, GPNGPapyrus::GetLightDuration);
		virtualMachine->RegisterFunction("SetLightDuration", className, GPNGPapyrus::SetLightDuration);
		virtualMachine->RegisterFunction("GetMovementType", className, GPNGPapyrus::GetMovementType);
		virtualMachine->RegisterFunction("SetMovementType", className, GPNGPapyrus::SetMovementType);
		virtualMachine->RegisterFunction("GetWeatherInt", className, GPNGPapyrus::GetWeatherInt);
		virtualMachine->RegisterFunction("SetWeatherInt", className, GPNGPapyrus::SetWeatherInt);
		virtualMachine->RegisterFunction("SetSpellNthMagicEffect", className, GPNGPapyrus::SetSpellNthMagicEffect);
		virtualMachine->RegisterFunction("GetSpellType", className, GPNGPapyrus::GetSpellType);
		virtualMachine->RegisterFunction("GetSpellCastType", className, GPNGPapyrus::GetSpellCastType);
		virtualMachine->RegisterFunction("GetSpellDelivery", className, GPNGPapyrus::GetSpellDelivery);
		virtualMachine->RegisterFunction("GetSpellChargeTime", className, GPNGPapyrus::GetSpellChargeTime);
		virtualMachine->RegisterFunction("GetSpellRange", className, GPNGPapyrus::GetSpellRange);
		virtualMachine->RegisterFunction("SetSpellType", className, GPNGPapyrus::SetSpellType);
		virtualMachine->RegisterFunction("SetSpellCastType", className, GPNGPapyrus::SetSpellCastType);
		virtualMachine->RegisterFunction("SetSpellCastTime", className, GPNGPapyrus::SetSpellCastTime);
		virtualMachine->RegisterFunction("SetSpellChargeTime", className, GPNGPapyrus::SetSpellChargeTime);
		virtualMachine->RegisterFunction("SetSpellDelivery", className, GPNGPapyrus::SetSpellDelivery);
		virtualMachine->RegisterFunction("SetSpellPerk", className, GPNGPapyrus::SetSpellPerk);
		virtualMachine->RegisterFunction("SetMagickaCost", className, GPNGPapyrus::SetMagickaCost);
		virtualMachine->RegisterFunction("MergeSpells", className, GPNGPapyrus::MergeSpells);
		return true;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	InitLogging("%d.%m.%Y %H:%M:%S [%s:%#] %v");

	const auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logs::info(
		"{} {} version {} is loading into {}", GPNGUtility::logLevelI,
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);

	const auto papyrusInterface = SKSE::GetPapyrusInterface();
	if (papyrusInterface) {
		if (papyrusInterface->Register(GPNGPapyrus::Register)) {
			logs::info("{} Papyrus functions registered", GPNGUtility::logLevelI);
			return true;
		}
		logs::info("{} Could not register Papyrus functions", GPNGUtility::logLevelF);
		return false;
	}
	logs::info("{} Papyrus interface not found", GPNGUtility::logLevelF);
	return false;
}
