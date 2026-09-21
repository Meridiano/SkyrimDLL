#pragma once
#include "hpp/process.hpp"

namespace FCHooks {

	// the game calls this to parse the fontconfig file
	class ParseConfigHook {
	private:
		static inline std::string name{ "1AF0D603" };
		struct Call {
			static void NEW(RE::BSScaleformManager* scaleform) {
				OLD(scaleform);
				logs::info("{} triggered, BSScaleformManager = {:X}", name, (std::uint64_t)scaleform);
				if (scaleform) FCProcess::ExtendedProcess(scaleform, OLD);
			}
			static inline REL::Relocation<void(*)(RE::BSScaleformManager*)> OLD;
		};
	public:
		static void Install() {
			bool newAE = FCData::gameVersion > REL::Version(1, 6, 1179, 0);
			static REL::Relocation target{
				REL::VariantID(80300, 82323, 0xF2ADC0),
				REL::VariantOffset(0x3B1, newAE ? 0x430 : 0x434, 0x3B1)
			};
			if (!SignMatch(target, "E8")) FatalError("{} target is invalid", name);
			Call::OLD = target.write_call<5>(Call::NEW);
			logs::info("{} installed", name);
		}
	};

	// the game calls this to insert a font into GFxFontMap
	class MapFontHook {
	private:
		static inline std::string name{ "3153A5AB" };
		template <std::uint8_t variant>
		struct Call {
			static bool NEW(FCData::GFxFontMap* map, char* key, char* tag, FCData::FontType type, float scale, std::int32_t unusedA, std::int32_t unusedB) {
				auto info = std::format("{} input = {} + {} + {}", name, FCUtility::Quoted(key), FCUtility::Quoted(tag), (std::uint32_t)type);
				bool result = OLD(map, key, tag, type, scale, unusedA, unusedB);
				logs::info("{} / result = {:X}", info, result);
				FCData::fontMap = map;
				return result;
			}
			static inline REL::Relocation<decltype(NEW)> OLD;
		};
	public:
		static void Install(std::uint8_t variant) {
			switch (variant) {
				case 0: { // SE-VR
					static auto id = REL::VariantID(80310, NULL, 0xF2BD00);
					static std::uint64_t offsets[5] = { 0x326, 0x382, 0x3DE, 0x437, 0x452 };
					// make
					static REL::Relocation targetA{ id, REL::Offset(offsets[0]) };
					static REL::Relocation targetB{ id, REL::Offset(offsets[1]) };
					static REL::Relocation targetC{ id, REL::Offset(offsets[2]) };
					static REL::Relocation targetD{ id, REL::Offset(offsets[3]) };
					static REL::Relocation targetE{ id, REL::Offset(offsets[4]) };
					// check
					if (!SignMatch(targetA, "E8")) FatalError("{}:{:X} target is invalid", name, offsets[0]);
					if (!SignMatch(targetB, "E8")) FatalError("{}:{:X} target is invalid", name, offsets[1]);
					if (!SignMatch(targetC, "E8")) FatalError("{}:{:X} target is invalid", name, offsets[2]);
					if (!SignMatch(targetD, "E8")) FatalError("{}:{:X} target is invalid", name, offsets[3]);
					if (!SignMatch(targetE, "E8")) FatalError("{}:{:X} target is invalid", name, offsets[4]);
					// install
					Call<0>::OLD = targetA.write_call<5>(Call<0>::NEW);
					Call<1>::OLD = targetB.write_call<5>(Call<1>::NEW);
					Call<2>::OLD = targetC.write_call<5>(Call<2>::NEW);
					Call<3>::OLD = targetD.write_call<5>(Call<3>::NEW);
					Call<4>::OLD = targetE.write_call<5>(Call<4>::NEW);
					// done
					logs::info("{} installed", name);
				}   break;
				case 1: { // AE
					bool newAE = FCData::gameVersion > REL::Version(1, 6, 1179, 0);
					static REL::Relocation target{ REL::ID(82334), REL::Offset(newAE ? 0x12C4 : 0x1284) };
					if (!SignMatch(target, "E8")) FatalError("{} target is invalid", name);
					Call<0>::OLD = target.write_call<5>(Call<0>::NEW);
					logs::info("{} installed", name);
				}   break;
			}
		}
	};

	// the game calls this to find a string inside the map
	class GetStringHook {
	private:
		static inline std::string name{ "02752D51" };
		struct Call {
			static RE::GString* NEW(FCData::FontMapData** mapHolder, RE::GString* str) {
				auto result = OLD(mapHolder, str);
				if (str && result) {
					auto data = str->data();
					logs::info("{} duplication of {} in address {:X}", name, FCUtility::Quoted(data ? data : ""), (std::uint64_t)result);
					result = nullptr;
				}
				return result;
			}
			static inline REL::Relocation<decltype(NEW)> OLD;
		};
	public:
		static void Install(std::uint8_t variant) {
			switch (variant) {
				case 0: { // SE-VR
					static REL::Relocation target{ REL::VariantID(82537, NULL, 0xFA9210), REL::Offset(0x51) };
					if (SignMatch(target, "74 69") || SignMatch(target, "EB 69")) {
						REL::safe_fill(target.address(), 0xEB, 1);
						logs::info("{} installed", name);
					} else FatalError("{} target is invalid", name);
				}   break;
				case 1: { // AE
					static REL::Relocation target{ REL::ID(84623), REL::Offset(0x44) };
					if (!SignMatch(target, "E8")) FatalError("{} target is invalid", name);
					Call::OLD = target.write_call<5>(Call::NEW);
					logs::info("{} installed", name);
				}   break;
			}
		}
	};

	void InstallAll() {
		std::uint8_t variant = FCData::gameVersion > REL::Version(1, 5, 97, 0) ? 1 : 0;
		logs::info("Using hooks variant {}", variant);
		ParseConfigHook::Install();
		MapFontHook::Install(variant);
		GetStringHook::Install(variant);
	}

}
