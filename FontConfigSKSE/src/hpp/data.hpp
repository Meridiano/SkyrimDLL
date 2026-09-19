#pragma once
#include "hpp/utility.hpp"

namespace FCData {

	ScopedEnum(FontType, std::uint32_t,
		kNormal = 0,
		kItalic,
		kBold,
		kBoldItalic,
		kUnknown = 16
	);

	struct FontMapEntry {
		std::int64_t flags;
		std::uint64_t index;
		RE::GString key;
		RE::GString tag;
		float special;
		std::int32_t unusedA;
		std::int32_t unusedB;
		FontType type;
	};
	static_assert(sizeof(FontMapEntry) == 0x30);

	struct FontMapData {
		std::uint64_t count;
		std::uint64_t maxIndex;
		FontMapEntry firstEntry;
	};
	static_assert(sizeof(FontMapData) == 0x40);

	class GFxFontMap {
		virtual ~GFxFontMap() = 0;
		// members
		std::byte unkownData[16];
		FontMapData** mapHolder;
	public:
		void LogEntries() {
			if (mapHolder) if (auto map = *mapHolder; map) {
				logs::info("Map address = {:X}", (std::uint64_t)map);
				std::uint64_t index = 0;
				std::span<FontMapEntry> span{ &map->firstEntry, map->maxIndex + 1 };
				for (auto& entry : span) if (entry.flags != -2) {
					auto keyData = entry.key.data();
					auto tagData = entry.tag.data();
					if (keyData && tagData) logs::info("Entry #{} / {} = {}", index++, FCUtility::Quoted(keyData), FCUtility::Quoted(tagData));
				}
			}
		}
	};

	std::string fontConfig{};
	REL::Version gameVersion{};
	std::vector<std::string> configs{};
	std::string validNameChars{};
	GFxFontMap* fontMap{};

}
