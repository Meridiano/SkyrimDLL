bool ReadBoolFromIni(std::string section, std::string key) {
	char buffer[32];
	std::string path = std::format("Data\\SKSE\\Plugins\\{}.ini", SKSE::GetPluginName());
	GetPrivateProfileString(
		section.data(),
		key.data(),
		"false",
		buffer,
		sizeof(buffer),
		std::filesystem::absolute(path).string().data()
	);
	return (strnicmp(std::string(buffer).data(), "true", 4) == 0);
}

void TweakShadowLights(bool procSpot) {
	if (auto tesDH = RE::TESDataHandler::GetSingleton(); tesDH) {
		auto list = tesDH->GetFormArray<RE::TESObjectLIGH>();
		if (list.size() > 0) for (auto form : list) {
			if (form) {
				auto oldFlags = form->data.flags;
				auto newFlags = oldFlags;
				newFlags.reset(LF::kHemiShadow);
				newFlags.reset(LF::kOmniShadow);
				if (procSpot) if (oldFlags & LF::kSpotShadow) {
					newFlags.reset(LF::kSpotShadow);
					newFlags.set(LF::kSpotlight);
				}
				if (oldFlags != newFlags) {
					form->data.flags = newFlags;
					logs::info(
						"Light {:08X} flags changed from {:04X} to {:04X}",
						form->formID, oldFlags.underlying(), newFlags.underlying()
					);
				}
			}
		}
	}
}

void ProcessMessage(SKSE::MessagingInterface::Message* a_msg) {
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		bool procSpot = ReadBoolFromIni("General", "bProcessSpotlights");
		logs::info("Data loaded, bProcessSpotlights = {}", procSpot);
		TweakShadowLights(procSpot);
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	logs::init();
	logs::info(
		"{} version {} is loading into SkyrimSE {}",
		SKSE::GetPluginName(),
		SKSE::GetPluginVersion().string("."),
		a_skse->RuntimeVersion().string(".")
	);
	const auto skseMessaging = SKSE::GetMessagingInterface();
	return (skseMessaging && skseMessaging->RegisterListener(ProcessMessage));
}
