#pragma once

namespace ROTUtility {

	bool IsGamepadMode() {
		auto inputManager = RE::BSInputDeviceManager::GetSingleton();
		auto gamepad = (inputManager ? inputManager->IsGamepadEnabled() : false);
		return gamepad;
	}

	std::int32_t ButtonEventToDXScanCode(RE::INPUT_DEVICE device, RE::ButtonEvent* event) {
		switch (device) {
			case RE::INPUT_DEVICE::kKeyboard: return std::int32_t(event->GetIDCode());
			case RE::INPUT_DEVICE::kMouse: return std::int32_t(event->GetIDCode() + 256);
			case RE::INPUT_DEVICE::kGamepad: {
				using GB = RE::BSWin32GamepadDevice::Key;
				const auto button = static_cast<GB>(event->GetIDCode());
				switch (button) {
					case GB::kUp: return 266;
					case GB::kDown: return 267;
					case GB::kLeft: return 268;
					case GB::kRight: return 269;
					case GB::kStart: return 270;
					case GB::kBack: return 271;
					case GB::kLeftThumb: return 272;
					case GB::kRightThumb: return 273;
					case GB::kLeftShoulder: return 274;
					case GB::kRightShoulder: return 275;
					case GB::kA: return 276;
					case GB::kB: return 277;
					case GB::kX: return 278;
					case GB::kY: return 279;
					case GB::kLeftTrigger: return 280;
					case GB::kRightTrigger: return 281;
				}
			}
		}
		return 0;
	}

	unsigned char CharToLower(unsigned char ch) {
		return (unsigned char)(std::tolower(ch));
	}

	std::string StringToLower(std::string toConvert) {
		std::string lower = toConvert;
		std::transform(toConvert.begin(), toConvert.end(), lower.begin(), CharToLower);
		return lower;
	}

}