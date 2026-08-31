#pragma once

#include "hpp/utility.hpp"

namespace CamFolHooks {

	RE::PlayerCamera* pCamera = nullptr;
	std::uint32_t* pControls = nullptr;
	RE::UI* pUserInterface = nullptr;
	RE::PlayerCharacter* pPlayer = nullptr;
	RE::VATS* pVATS = nullptr;

	class UpdateDataHook {
	private:
		static std::size_t UpdateDataMod(RE::NiAVObject* object, RE::NiUpdateData* data) {
			if (object && data) {
				if (pCamera && pControls && pUserInterface && pPlayer && pVATS)
					if (CamFolUtility::ProcessConditions(pCamera, pControls, pUserInterface, pPlayer, pVATS))
						if (CamFolUtility::UpdateCamera(object, pCamera, pPlayer)) {
							data->flags = RE::NiUpdateData::Flag::kDirty;
							data->time = 0.0F;
						}
			}
			return UpdateData(object, data);
		}
		static inline REL::Relocation<decltype(UpdateDataMod)> UpdateData;
	public:
		static void Install() {
			auto id = REL::RelocationID(68900, 70251, 68900).id();
			// push rbx = 40 53
			// sub rsp, 20h = 48 83 EC 20
			UpdateData = CamFolUtility::WriteFunctionHook(id, 6, UpdateDataMod);
			logs::info("New UpdateData hook installed");
		}
	};

	void FillPointers() {
		// set pointers
		pCamera = RE::PlayerCamera::GetSingleton();

		bool newest = MODULE.IsAE() && (MODULE.version().minor() >= 7 || MODULE.version().patch() >= 1130);
		pControls = CamFolUtility::GetMember<std::uint32_t>(RE::ControlMap::GetSingleton(), newest ? 0x120 : 0x118);

		pUserInterface = RE::UI::GetSingleton();
		pPlayer = RE::PlayerCharacter::GetSingleton();
		pVATS = RE::VATS::GetSingleton();

		// log pointers
		logs::info(
			"Camera = {:X}, Controls = {:X} (Offset {}), UserInterface = {:X}, Player = {:X}, VATS = {:X}",
			(std::uint64_t)pCamera,
			(std::uint64_t)pControls, newest ? "V2" : "V1",
			(std::uint64_t)pUserInterface,
			(std::uint64_t)pPlayer,
			(std::uint64_t)pVATS
		);
	}

}
