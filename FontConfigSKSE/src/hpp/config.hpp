#pragma once
#include "hpp/utility.hpp"

namespace FCConfig {

	std::unordered_map<std::string, float, FCUtility::CaseInsensitiveHash, FCUtility::CaseInsensitiveEqual> fontScaleMap{};

    void ParseConfig(std::string path) {
        auto [root, data] = std::pair{ glz::generic(), std::string() };
        if (auto error = glz::read_file_json(root, path, data); error || !root.is_object()) {
            logs::info("Invalid JSON = {}", path);
            return;
        }
        if (static std::string fontScale{ "FontScale" }; root.contains(fontScale)) {
            if (auto& section = root.at(fontScale); section.is_object()) {
                for (auto& [key, value] : section.get_object()) if (value.is_number()) {
                    float result = value.as<float>();
                    logs::info("New config option [{}] {} = {}", fontScale, FCUtility::Quoted(key), result);
                    fontScaleMap.insert_or_assign(key, result);
                }
            }
        }
    }

    void ParseConfigList() {
        auto stringPath = "Data\\SKSE\\Plugins\\" + std::string{ SKSE::GetPluginName() };
        if (std::filesystem::path root{ stringPath }; std::filesystem::is_directory(root)) {
            for (auto& entry : std::filesystem::directory_iterator(root)) {
                if (entry.is_regular_file() && FCUtility::VerifyExtension(entry, L".json") && entry.file_size()) {
                    auto newPath = entry.path().string();
                    logs::info("Processing new JSON = {}", newPath);
                    ParseConfig(newPath);
                }
            }
        }
    }

}
