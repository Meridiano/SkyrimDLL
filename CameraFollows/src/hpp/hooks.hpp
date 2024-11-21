#pragma once

#include "hpp/utility.hpp"

namespace CamFolHooks {

	RE::PlayerCamera* pCamera = nullptr;
	std::uint32_t* pControls = nullptr;
	RE::UI* pUserInterface = nullptr;
	RE::PlayerCharacter* pPlayer = nullptr;
	RE::VATS* pVATS = nullptr;

	class UpdatePCHook {
	public:
		static void Install() {
			REL::Relocation pcVTable{ RE::VTABLE_PlayerCharacter[0] };
			UpdatePC = pcVTable.write_vfunc(0xAD, UpdatePCMod);
			logs::info("New UpdatePC hook installed");
		}
	private:
		static void UpdatePCMod(RE::PlayerCharacter* pc, float delta) {
			// call original function
			UpdatePC(pc, delta);

			// do my stuff
			if (CamFolUtility::ProcessConditions(pCamera, pControls, pUserInterface, pPlayer, pVATS)) CamFolUtility::UpdateCamera(pCamera, pPlayer);
		}
		static inline REL::Relocation<decltype(UpdatePCMod)> UpdatePC;
	};

	void FillPointers() {
		// set pointers
		pCamera = RE::PlayerCamera::GetSingleton();

		auto module = &REL::Module::get();
		bool post1130 = module && module->IsAE() && (module->version().patch() > 0x400);
		pControls = CamFolUtility::GetMember<std::uint32_t>(RE::ControlMap::GetSingleton(), post1130 ? 0x120 : 0x118);

		pUserInterface = RE::UI::GetSingleton();
		pPlayer = RE::PlayerCharacter::GetSingleton();
		pVATS = RE::VATS::GetSingleton();

		// log pointers
		logs::info(
			"Camera = {:X}, Controls = {:X} (Post-1130 {}), UserInterface = {:X}, Player = {:X}, VATS = {:X}",
			(std::uint64_t)pCamera,
			(std::uint64_t)pControls, post1130 ? "Enabled" : "Disabled",
			(std::uint64_t)pUserInterface,
			(std::uint64_t)pPlayer,
			(std::uint64_t)pVATS
		);
	}

}
