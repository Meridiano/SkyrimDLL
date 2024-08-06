#pragma once

#include "hpp/utility.hpp"

namespace CamFolHooks {

	RE::PlayerCamera* pCamera = nullptr;
	std::uint32_t* pControls = nullptr;
	RE::UI* pUserInterface = nullptr;
	RE::PlayerCharacter* pPlayer = nullptr;

	class UpdatePCHook {
	public:
		static void Install() {
			logs::info("Installing new UpdatePC hook");
			REL::Relocation<std::uintptr_t> pcVTable{ RE::VTABLE_PlayerCharacter[0] };
			UpdatePC = pcVTable.write_vfunc(0xAD, UpdatePCMod);
			
			if (UpdatePC.address()) logs::info("Hook done!");
			else logs::info("Hook failed!");
		}
	private:
		static void UpdatePCMod(RE::PlayerCharacter* pc, float delta) {
			// call original function
			UpdatePC(pc, delta);

			// do my stuff
			if (CamFolUtility::ProcessConditions(pCamera, pControls, pUserInterface, pPlayer)) CamFolUtility::UpdateCamera(pCamera, pPlayer);
		}
		static inline REL::Relocation<decltype(UpdatePCMod)> UpdatePC;
	};

	void FillPointers() {
		// set pointers
		pCamera = RE::PlayerCamera::GetSingleton();

		bool post1130 = REL::Module::get().IsAE() && (REL::Module::get().version().patch() > 0x400);
		pControls = CamFolUtility::GetMember<std::uint32_t>(RE::ControlMap::GetSingleton(), post1130 ? 0x120 : 0x118);

		pUserInterface = RE::UI::GetSingleton();
		pPlayer = RE::PlayerCharacter::GetSingleton();

		// log pointers
		logs::info(
			"Camera = {:X}, Controls = {:X} (Post-1130 {}), UserInterface = {:X}, Player = {:X}",
			(std::uint64_t)pCamera,
			(std::uint64_t)pControls, post1130 ? "Enabled" : "Disabled",
			(std::uint64_t)pUserInterface,
			(std::uint64_t)pPlayer
		);
	}

}
