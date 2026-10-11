#pragma once
#include "hpp/data.hpp"

namespace FCProcess {

	void AdjustChars(RE::BSScaleformManager* scaleform) {
		auto oldChars = FCUtility::Dequoted(FCData::validNameChars);
		auto newChars = FCUtility::Dequoted(scaleform->validNameChars.data());
		if (oldChars != newChars) {
			newChars = FCUtility::Quoted(oldChars + newChars);
			FCData::validNameChars = scaleform->validNameChars = newChars.data();
		}
	}

	void ApplyFontScale(FCData::FontMapData** mapHolder) {
		if (mapHolder) if (auto map = *mapHolder; map) {
			logs::info("Map address = {:X}", (std::uint64_t)map);
			std::uint64_t index = 0;
			std::span<FCData::FontMapEntry> span{ &map->firstEntry, map->maxIndex + 1 };
			for (auto& entry : span) if (entry.flags != -2) {
				auto keyData = entry.key.data();
				auto tagData = entry.tag.data();
				if (keyData && tagData) {
					bool scaleChange = false;
					float oldScale = entry.scale;
					static auto mapEnd = FCConfig::fontScaleMap.end();
					if (auto mapEntry = FCConfig::fontScaleMap.find(keyData); mapEntry != mapEnd) {
						entry.scale = mapEntry->second;
						scaleChange = true;
					}
					logs::info(
						"Entry #{} / {} = {} / Scale {} {}",
						index++, FCUtility::Quoted(keyData), FCUtility::Quoted(tagData),
						scaleChange ? std::format("changed from {} to", oldScale) : "is", entry.scale
					);
				}
			}
		}
	}

	template <typename T>
	void ExtendedProcess(RE::BSScaleformManager* scaleform, REL::Relocation<T> function) {
		// save original - valid
		FCData::validNameChars = scaleform->validNameChars;
		logs::info("Original valid chars size = {}", FCData::validNameChars.size());
		// save original - config
		auto fontConfig = FCUtility::GetFontConfig();
		if (!fontConfig) FatalError("FontConfig data is missing");
		FCData::fontConfig = fontConfig->data.s;
		// iterate configs
		auto subPath = "SKSE\\Plugins\\" + std::string{ SKSE::GetPluginName() };
		if (std::filesystem::path root{ "Data\\" + subPath }; std::filesystem::is_directory(root)) {
			for (auto& entry : std::filesystem::directory_iterator(root)) {
				if (entry.is_regular_file() && FCUtility::VerifyExtension(entry, L".txt") && entry.file_size()) {
					auto newPath = FCUtility::StoreAndReturn(FCData::configs, subPath + "\\" + entry.path().filename().string());
					logs::info("Processing new config = {}", newPath);
					fontConfig->data.s = newPath;
					function(scaleform);
					AdjustChars(scaleform);
				}
			}
		}
		// final tweaks
		fontConfig->data.s = FCData::fontConfig.data();
		logs::info("New valid chars size = {}", scaleform->validNameChars.size());
		if (FCData::fontMap) ApplyFontScale(FCData::fontMap->mapHolder);
	}

}
