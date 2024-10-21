#pragma once

#include "hpp/utility.hpp"

namespace LiAtHooks {

	static bool oldState = false;
	static RE::PlayerCharacter* player = nullptr;

	class UpdateHook {
	public:
		static void Install() {
			auto address = REL::VariantID(35565, 36564, 0x5BAB10).address();
			auto offset = REL::VariantOffset(0x748, 0xC26, 0x7EE).offset();
			OnUpdate = SKSE::GetTrampoline().write_call<5>(address + offset, OnUpdateMod);
			logs::info("Update hook installed");
		}
	private:
		static void OnUpdateMod() {
			// call original function
			OnUpdate();
			// do my stuff
			auto newState = LiAtUtility::IsPC1stPerson();
			if (oldState != newState) {
				logs::info("First-person camera toggle / {} >> {}", oldState, newState);
				oldState = newState;
			}
			if (player && newState) LiAtUtility::ProcessLight(player);
		}
		static inline REL::Relocation<decltype(OnUpdateMod)> OnUpdate;
	};

	void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
			UpdateHook::Install();
		} else if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
			player = RE::PlayerCharacter::GetSingleton();
			if (player) logs::info("PlayerCharacter = {:X}", (std::uint64_t)player);
			else stl::report_and_fail("Error: PlayerCharacter not found");
			LiAtUtility::SetUpdateData();
		} else return;
	}

}
