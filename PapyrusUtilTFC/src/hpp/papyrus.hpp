#pragma once

namespace PapyrusFixed {

	void FixInputContext(RE::PlayerCamera* camera) {
		static auto controls = RE::ControlMap::GetSingleton();
		if (controls) {
			auto stateA = camera->currentState.get();
			auto stateB = camera->GetRuntimeData().cameraStates[RE::CameraState::kFree].get();
			if (stateA && stateB) {
				static auto context = RE::ControlMap::InputContextID::kTFCMode;
				if (stateA == stateB) {
					controls->PushInputContext(context);
				} else {
					controls->PopInputContext(context);
				}
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
				if (enable) SetFreeCameraSpeed(speed);
				if (freeCamera != enable) {
					camera->ToggleFreeCameraMode(false);
					FixInputContext(camera);
				}
			}
		}
	}

	bool PapyrusOverride(RE::BSScript::IVirtualMachine* a_vm) {
		logs::info("PapyrusOverride:{:X}", (std::uint64_t)a_vm);
		if (a_vm) {
			auto className = "MiscUtil";
			a_vm->RegisterFunction("ToggleFreeCamera", className, ToggleFreeCamera);
			a_vm->RegisterFunction("SetFreeCameraState", className, SetFreeCameraState);
		}
		return true;
	}

}
