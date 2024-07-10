#pragma once

namespace RE {
	namespace Offset {
		namespace CraftingMenu {
			inline static constexpr auto Ctor = REL::VariantID(
				50272,   //SE
				51361,   //AE
				0x88DE80 //VR
			);
			inline static constexpr auto CtorOffset = REL::VariantOffset(
				0x5B, //SE
				0xA9, //AE
				0x70  //VR
			);
		}
		namespace CraftingSubMenus {
			namespace ConstructibleObjectMenu {
				inline static constexpr auto UpdateItemList = REL::VariantID(
					50510,   //SE
					51401,   //AE
					0x89B630 //VR
				);
				inline static constexpr auto UpdateItemListOffset = REL::VariantOffset(
					0x22D, //SE
					0x2C8, //AE
					0x22D  //VR
				);
			}
		}
		namespace TESObjectREFR {
			inline static constexpr auto ActivateCraftingWorkBench = REL::VariantID(
				52056,   //SE
				52941,   //AE
				0x90B2D0 //VR
			);
			inline static constexpr auto ActivateCraftingWorkBenchOffset = REL::VariantOffset(
				0xE, //SE
				0xE, //AE
				0xE  //VR
			);
		}
	}
}
