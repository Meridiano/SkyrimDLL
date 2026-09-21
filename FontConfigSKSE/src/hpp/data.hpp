#pragma once
#include "hpp/config.hpp"

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
		float scale;
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

	class GFxFontMap : public RE::GFxState {
	public:
		// override
		virtual ~GFxFontMap();
		// members
		FontMapData** mapHolder;
	};
	static_assert(sizeof(GFxFontMap) == 0x20);

	std::string fontConfig{};
	REL::Version gameVersion{};
	std::vector<std::string> configs{};
	std::string validNameChars{};
	GFxFontMap* fontMap{};

}
