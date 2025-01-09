float GetConfigValue(std::string world) {
	// resolve config
	std::string config = "Data/SKSE/Plugins/BakaWorldMapFOV";
	auto configCustom = std::format("{}.{}.ini", config, world);
	if (fs::exists(configCustom)) config = configCustom;
	else config = std::format("{}.ini", config);
	// resolve value
	CSimpleIniA ini;
	ini.SetUnicode();
	SI_Error iniResult = ini.LoadFile(config.data());
	return static_cast<float>(
		iniResult < 0 ? 80.0 : ini.GetDoubleValue("General", "fWorldMapFOV", 80.0)
	);
}

class EventHandler:
	public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
private:
	inline static auto defaultWorldFOV = 80.0F;
	inline static auto defaultFirstFOV = 80.0F;
public:
	static EventHandler* GetSingleton() {
		static EventHandler singleton;
		return std::addressof(singleton);
	}
	static void Install() {
		if (auto handler = GetSingleton(); handler) {
			if (auto ui = RE::UI::GetSingleton(); ui) {
				ui->AddEventSink<RE::MenuOpenCloseEvent>(handler);
				logs::info("Added MenuOpenCloseEvent Handler");
			}
		}
	}
	RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override {
		if (a_event && a_event->menuName == "MapMenu") {
			if (auto camera = RE::PlayerCamera::GetSingleton(); camera) {
				if (a_event->opening) {
					std::string edid = "DefaultWorld";
					if (auto pc = RE::PlayerCharacter::GetSingleton(); pc)
						if (auto world = pc->GetWorldspace(); world)
							edid = world->GetFormEditorID();
					// get
					auto fWorldMapFOV = GetConfigValue(edid);
					// save
					defaultWorldFOV = CameraData2.worldFOV;
					defaultFirstFOV = CameraData2.firstPersonFOV;
					// set
					CameraData2.worldFOV = fWorldMapFOV;
					CameraData2.firstPersonFOV = fWorldMapFOV;
					// log
					logs::info("{} : {} {} > {}", edid, defaultWorldFOV, defaultFirstFOV, fWorldMapFOV);
				} else {
					// restore
					CameraData2.worldFOV = defaultWorldFOV;
					CameraData2.firstPersonFOV = defaultFirstFOV;
					// log
					logs::info("Restore : {} {}", defaultWorldFOV, defaultFirstFOV);
				}
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}
};

void MessageCallback(SKSE::MessagingInterface::Message* a_msg) noexcept {
	switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			EventHandler::Install();
			break;
		default:
			break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_SKSE) {
	SKSE::Init(a_SKSE, true);
	auto msg = SKSE::GetMessagingInterface();
	return (msg && msg->RegisterListener(MessageCallback));
}
