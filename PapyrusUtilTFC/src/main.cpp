namespace MiscUtilFixed {

	void FixInputContext(RE::PlayerCamera* camera) {
		static auto controls = RE::ControlMap::GetSingleton();
		if (controls) {
			auto stateA = camera->currentState.get();
			auto stateB = camera->GetRuntimeData().cameraStates[3].get();
			static auto context = RE::ControlMap::InputContextID::kTFCMode;
			if (stateA == stateB) {
				controls->PushInputContext(context);
			} else {
				controls->PopInputContext(context);
			}
		}
	}

	void SetFreeCameraSpeed(float speed) {
		static auto collection = RE::INISettingCollection::GetSingleton();
		if (collection) {
			static auto setting = collection->GetSetting("fFreeCameraTranslationSpeed:Camera");
			if (setting) {
				setting->data.f = speed;
			}
		}
	}

	void ToggleFreeCamera(RE::StaticFunctionTag* base, bool stopTime) {
		(void)base; // unused
		logs::info("ToggleFreeCamera:{}", stopTime);
		static auto camera = RE::PlayerCamera::GetSingleton();
		if (camera) {
			camera->ToggleFreeCameraMode(stopTime);
			FixInputContext(camera);
		}
	}

	void SetFreeCameraState(RE::StaticFunctionTag* base, bool enable, float speed) {
		(void)base; // unused
		logs::info("SetFreeCameraState:{}:{}", enable, speed);
		static auto camera = RE::PlayerCamera::GetSingleton();
		if (camera) {
			auto cameraState = camera->currentState.get();
			if (cameraState) {
				bool freeCamera = cameraState->id == RE::CameraState::kFree;
				if (freeCamera != enable) {
					if (enable) SetFreeCameraSpeed(speed);
					camera->ToggleFreeCameraMode(false);
					FixInputContext(camera);
				}
			}
		}
	}

}

bool PapyrusOverride(RE::BSScript::IVirtualMachine* a_vm) {
	logs::info("PapyrusOverride:{:X}", (std::uint64_t)a_vm);
	if (a_vm) {
		auto className = "MiscUtil";
		a_vm->RegisterFunction("ToggleFreeCamera", className, MiscUtilFixed::ToggleFreeCamera);
		a_vm->RegisterFunction("SetFreeCameraState", className, MiscUtilFixed::SetFreeCameraState);
	}
	return true;
}

void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
	if (a_msg->type == SKSE::MessagingInterface::kPostPostLoad) {
		auto papyrusUtil = FindModule(PapyrusUtil);
		if (papyrusUtil) {
			const auto papInterface = SKSE::GetPapyrusInterface();
			if (papInterface) papInterface->Register(PapyrusOverride);
			else SKSE::stl::report_and_fail("Script interface not found");
		} else SKSE::stl::report_and_fail("PapyrusUtil module not found");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, true);

	auto module = &REL::Module::get();
	auto moduleName = fs::path(module->filename()).replace_extension("").string();
	auto moduleVersion = module->version().string("-");
	logs::info("{} v{}", moduleName, moduleVersion);

	const auto msgInterface = SKSE::GetMessagingInterface();
	return (msgInterface ? msgInterface->RegisterListener(MessageListener) : false);
}
