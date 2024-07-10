#pragma once

#include "hpp/config.hpp"

namespace ROTProcess {

	class EventManager final:
		public RE::BSTEventSink<RE::InputEvent*>,
		public RE::BSTEventSink<RE::MenuOpenCloseEvent> {
		std::int32_t dxScanCode;
		bool allowRotation;
		RE::NiPoint3 angle;
	public:
		static EventManager* GetSingleton() {
			static EventManager self;
			return std::addressof(self);
		}

		void RotatePlayer(RE::PlayerCharacter* player, float amount) {
			if (amount == 0.0F) return;
			if (auto root = player->Get3D(false); root) {
				angle.z += amount;
				root->local.rotate.SetEulerAnglesXYZ(angle);
				RE::NiUpdateData data;
				root->UpdateWorldData(&data);
			}
		}

		RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) {
			reinterpret_cast<std::uint64_t>(a_eventSource);
			if (!a_event) return RE::BSEventNotifyControl::kContinue;

			auto ui = RE::UI::GetSingleton();
			if (!ui || !ui->IsMenuOpen(RE::RaceSexMenu::MENU_NAME)) return RE::BSEventNotifyControl::kContinue;

			auto player = RE::PlayerCharacter::GetSingleton();
			if (!player || !player->Is3DLoaded()) return RE::BSEventNotifyControl::kContinue;

			for (auto event = *a_event; event; event = event->next) {
				auto inputDevice = event->GetDevice();
				switch (event->GetEventType()) {
					// iterate event
					case RE::INPUT_EVENT_TYPE::kButton:
					{
						auto buttonEvent = event->AsButtonEvent();
						if (buttonEvent && buttonEvent->IsHeld()) {
							dxScanCode = ROTUtility::ButtonEventToDXScanCode(inputDevice, buttonEvent);
						} else {
							allowRotation = false;
							continue;
						}
						// iterate device
						switch (inputDevice) {
							case RE::INPUT_DEVICE::kMouse:
							{
								if (ROTUtility::IsGamepadMode()) break;
								if (dxScanCode == ROTConfig::iRegularButton) {
									allowRotation = true;
									continue;
								}
								break;
							}
							case RE::INPUT_DEVICE::kKeyboard:
							{
								if (ROTUtility::IsGamepadMode()) break;
								if (dxScanCode == ROTConfig::iRegularButton) {
									allowRotation = true;
									continue;
								}
								break;
							}
							case RE::INPUT_DEVICE::kGamepad:
							{
								if (ROTUtility::IsGamepadMode()) {
									if (dxScanCode == ROTConfig::iGamepadButtonL) {
										// rotate left
										float amount = ROTConfig::fRotationAmount;
										RotatePlayer(player, amount);
										continue;
									} else if (dxScanCode == ROTConfig::iGamepadButtonR) {
										// rotate right
										float amount = 0.0F - ROTConfig::fRotationAmount;
										RotatePlayer(player, amount);
										continue;
									}
								}
								break;
							}
						}
					}
					continue;
					case RE::INPUT_EVENT_TYPE::kMouseMove:
					{
						if (!allowRotation) continue;

						auto mouseEvent = reinterpret_cast<RE::MouseMoveEvent*>(event->AsIDEvent());
						if (!mouseEvent) continue;

						// calculate and rotate
						auto mouseDeltaX = mouseEvent->mouseInputX;
						float amount = mouseDeltaX == 0 ? 0.0F : (mouseDeltaX > 0 ? -1 : 1) * ROTConfig::fRotationAmount;
						RotatePlayer(player, amount);
					}
					break;
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) {
			reinterpret_cast<std::uint64_t>(a_eventSource);
			if (a_event && a_event->menuName == RE::RaceSexMenu::MENU_NAME && a_event->opening) {
				auto player = RE::PlayerCharacter::GetSingleton();
				if (auto root = player->Get3D(false); root) {
					root->local.rotate.ToEulerAnglesXYZ(angle);
				}
			}
			return RE::BSEventNotifyControl::kContinue;
		}

	};

	void ProcessMessage(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kInputLoaded) {
			if (const auto input = RE::BSInputDeviceManager::GetSingleton(); input) {
				input->AddEventSink<RE::InputEvent*>(EventManager::GetSingleton());
			}
			if (const auto ui = RE::UI::GetSingleton(); ui) {
				ui->AddEventSink<RE::MenuOpenCloseEvent>(EventManager::GetSingleton());
			}
		}
	}

}