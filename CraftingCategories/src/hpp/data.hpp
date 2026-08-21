#pragma once

namespace Data {
	//ConfigLoader.h
	class ConfigLoader final {
	public:
		static ConfigLoader* GetSingleton();
		~ConfigLoader() = default;
		ConfigLoader(const ConfigLoader&) = delete;
		ConfigLoader(ConfigLoader&&) = delete;
		ConfigLoader& operator=(const ConfigLoader&) = delete;
		ConfigLoader& operator=(ConfigLoader&&) = delete;
		void LoadConfigs();
		void LoadConfig(const fs::path& a_path);
	private:
		ConfigLoader() = default;
		KeywordSet GetKeywords(Json::Value a_json);
	};
	//CategoryManager.h
	class CategoryManager final {
	public:
		struct Category {
			std::string Label;
			KeywordSet Keywords;
		};
		struct Section {
			std::string Label;
			std::int32_t Priority = 50;
			KeywordSet Keywords;
			std::string IconSource = "craftingcategories/icons.swf";
			std::string IconLabel = "default";
			tsl::ordered_map<std::string, Category> Categories;
			KeywordMap<Category*> CategoryKeywords;
		};
		static CategoryManager* GetSingleton();
		~CategoryManager() = default;
		CategoryManager(const CategoryManager&) = delete;
		CategoryManager(CategoryManager&&) = delete;
		CategoryManager& operator=(const CategoryManager&) = delete;
		CategoryManager& operator=(CategoryManager&&) = delete;
		void AddSection(
			const std::string& a_label,
			std::int32_t a_priority,
			KeywordSet&& a_keywords,
			const std::string& a_iconSource,
			const std::string& a_iconLabel);
		void AddCategory(
			const std::string& a_label,
			const std::string& a_section,
			KeywordSet&& a_keywords);
		void ProcessEntry(RE::GFxValue& a_entryObject, RE::TESForm* a_form);
		void GetCategoryArgs(RE::BSTArray<RE::GFxValue>& a_result);
		void RemoveRedundantFilters();
		void ResetFlags();
	private:
		inline static constexpr auto AllLabel = "$ALL";
		inline static constexpr auto MiscLabel = "$MISC";
		inline static constexpr auto OtherLabel = "$Other";
		inline static constexpr std::uint32_t AllFlag = 0xFFFFFFFF;
		inline static constexpr std::uint32_t SectionMask = 0xFF000000;
		inline static constexpr std::uint32_t CategoryMask = 0x00FFFFFF;
		inline static constexpr std::uint32_t CategoryBits = 24;
		inline static constexpr std::uint32_t MiscSectionFlag = 0x01000000;
		inline static constexpr std::uint32_t OtherCategoryFlag = 0x00000001;
		inline static constexpr std::uint32_t StartIndex = 2;
		CategoryManager();
		struct CategoryData {
			const char* Label;
			std::int32_t Priority;
			const char* IconSource;
			const char* IconLabel;
		};
		std::string _iconSourceAll;
		std::string _iconLabelAll;
		tsl::ordered_map<std::string, Section> _sections;
		KeywordMap<Section*> _sectionKeywords;
		std::uint32_t _nextSectionFlag = StartIndex;
		std::uint32_t _nextCategoryFlag = StartIndex;
		std::map<const Section*, std::uint32_t> _sectionFlags;
		std::map<const Category*, std::uint32_t> _categoryFlags;
		std::map<std::uint32_t, CategoryData> _currentFilters;
	};
	//ConfigLoader.cpp
	ConfigLoader* ConfigLoader::GetSingleton() {
		static ConfigLoader singleton{};
		return &singleton;
	}
	RE::BSResourceNiBinaryStream& operator>>(RE::BSResourceNiBinaryStream& a_sin, Json::Value& a_root) {
		Json::CharReaderBuilder fact;
		std::unique_ptr<Json::CharReader> const reader{ fact.newCharReader() };
		auto size = a_sin.stream->totalSize;
		auto buffer = std::make_unique<char[]>(size);
		a_sin.read(buffer.get(), size);
		auto begin = buffer.get();
		auto end = begin + size;
		std::string errs;
		bool ok = reader->parse(begin, end, &a_root, &errs);
		if (!ok) {
			throw std::runtime_error(errs);
		}
		return a_sin;
	}
	void ConfigLoader::LoadConfigs() {
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) return;
		for (auto& file : dataHandler->files) {
			if (!file) continue;
			auto fileName = fs::path(file->fileName);
			fileName.replace_extension("json"sv);
			auto directory = fs::path(std::format("SKSE/Plugins/{}", SKSE::GetPluginName()));
			auto configFile = directory / fileName;
			LoadConfig(configFile);
		}
	}
	void ConfigLoader::LoadConfig(const fs::path& a_path) {
		RE::BSResourceNiBinaryStream fileStream{ a_path.string() };
		if (!fileStream.good()) return;
		Json::Value root;
		try {
			fileStream >> root;
		} catch (...) {
			logger::info("Parse errors in file: {}", a_path.filename().string());
		}
		if (!root.isObject()) return;
		auto categoryManager = CategoryManager::GetSingleton();
		Json::Value sections = root["sections"];
		if (sections.isObject()) {
			for (auto& label : sections.getMemberNames()) {
				Json::Value section = sections[label];
				std::int32_t priority = -1;
				if (section.isMember("priority")) {
					priority = std::min(std::max(section["priority"].asInt(), 0), 100);
				}
				auto keywords = GetKeywords(section["keywords"]);
				std::string iconSource;
				std::string iconLabel;
				Json::Value icon = section["icon"];
				if (icon.isObject()) {
					iconSource = icon["source"].asString();
					iconLabel = icon["label"].asString();
				}
				categoryManager->AddSection(
					label,
					priority,
					std::move(keywords),
					iconSource,
					iconLabel
				);
			}
		}
		Json::Value categories = root["categories"];
		if (categories.isObject()) {
			for (auto& label : categories.getMemberNames()) {
				Json::Value category = categories[label];
				std::string section = category["section"].asString();
				auto keywords = GetKeywords(category["keywords"]);
				categoryManager->AddCategory(label, section, std::move(keywords));
			}
		}
	}
	KeywordSet ConfigLoader::GetKeywords(Json::Value a_json) {
		KeywordSet keywords;
		if (a_json.isArray()) {
			for (auto& id : a_json) {
				keywords.insert(id.asString());
			}
		}
		return keywords;
	}
	//CategoryManager.cpp
	CategoryManager::CategoryManager() {
		auto& miscSection = _sections[""];
		miscSection.Label = MiscLabel;
		miscSection.Priority = 200;
	}
	CategoryManager* CategoryManager::GetSingleton() {
		static CategoryManager singleton{};
		return &singleton;
	}
	void CategoryManager::AddSection(
		const std::string& a_label,
		std::int32_t a_priority,
		KeywordSet&& a_keywords,
		const std::string& a_iconSource,
		const std::string& a_iconLabel) {
		if (a_label == AllLabel) {
			if (!a_iconSource.empty()) {
				_iconSourceAll = a_iconSource;
			}
			if (!a_iconLabel.empty()) {
				_iconLabelAll = a_iconLabel;
			}
			return;
		} else if (a_label == MiscLabel) {
			if (!a_iconSource.empty()) {
				_sections[""].IconSource = a_iconSource;
			}
			if (!a_iconLabel.empty()) {
				_sections[""].IconLabel = a_iconLabel;
			}
			return;
		}
		auto& section = _sections[a_label];
		section.Label = a_label;
		if (a_priority >= 0) {
			section.Priority = a_priority;
		}
		section.Keywords.merge(a_keywords);
		if (!a_iconSource.empty()) {
			section.IconSource = a_iconSource;
		}
		if (!a_iconLabel.empty()) {
			section.IconLabel = a_iconLabel;
		}
		for (auto& keyword : a_keywords) {
			if (auto i = _sectionKeywords.find(keyword); i != _sectionKeywords.end()) {
				i->second->Keywords.erase(keyword);
			}
			_sectionKeywords[keyword] = &section;
		}
	}
	void CategoryManager::AddCategory(
		const std::string& a_label,
		const std::string& a_section,
		KeywordSet&& a_keywords) {
		auto& section = _sections[a_section];
		auto& category = section.Categories[a_label];
		category.Label = a_label;
		category.Keywords.merge(a_keywords);
		for (auto& keyword : a_keywords) {
			if (auto i = section.CategoryKeywords.find(keyword);
				i != section.CategoryKeywords.end()) {
				i->second->Keywords.erase(keyword);
			}
			section.CategoryKeywords[keyword] = &category;
		}
	}
	void CategoryManager::ProcessEntry(RE::GFxValue& a_entryObject, RE::TESForm* a_form) {
		const auto keywordForm = skyrim_cast<RE::BGSKeywordForm*>(a_form);
		const Section* assignedSection = &_sections[""];
		const Category* assignedCategory = nullptr;
		std::uint32_t sectionFlag = MiscSectionFlag;
		std::uint32_t categoryFlag = OtherCategoryFlag;
		_sectionFlags.try_emplace(assignedSection, MiscSectionFlag);
		if (keywordForm) {
			auto hasKeyword = [keywordForm](std::string_view keywordString) {
				const auto keywords = std::span(keywordForm->keywords, keywordForm->numKeywords);
				return std::ranges::any_of(keywords, [&](auto&& keyword) {
					return keyword->formEditorID.size() == keywordString.size() && ::_strnicmp(keyword->formEditorID.data(), keywordString.data(), keywordString.size()) == 0;
				});
			};
			for (auto s = _sections.rbegin(); s != _sections.rend(); ++s) {
				auto& [label, section] = *s;
				if (std::ranges::any_of(section.Keywords, hasKeyword)) {
					assignedSection = &section;
					if (auto i = _sectionFlags.find(&section); i != _sectionFlags.end()) {
						sectionFlag = i->second;
					} else {
						sectionFlag = _nextSectionFlag++ << CategoryBits;
						_sectionFlags[&section] = sectionFlag;
					}
					break;
				}
			}
			for (auto c = assignedSection->Categories.rbegin(); c != assignedSection->Categories.rend(); ++c) {
				auto& [label, category] = *c;
				if (std::ranges::any_of(category.Keywords, hasKeyword)) {
					assignedCategory = &category;
					if (auto i = _categoryFlags.find(&category); i != _categoryFlags.end()) {
						categoryFlag = i->second;
					} else {
						categoryFlag = _nextCategoryFlag;
						_categoryFlags[&category] = _nextCategoryFlag++;
					}
					break;
				}
			}
		}
		_currentFilters[sectionFlag] = CategoryData{
			.Label = assignedSection->Label.data(),
			.Priority = assignedSection->Priority,
			.IconSource = assignedSection->IconSource.data(),
			.IconLabel = assignedSection->IconLabel.data(),
		};
		_currentFilters[sectionFlag | categoryFlag] = CategoryData{
			.Label = assignedCategory ? assignedCategory->Label.data() : OtherLabel,
			.Priority = assignedSection->Priority,
		};
		RE::GFxValue priority = assignedSection->Priority;
		a_entryObject.SetMember("priority", priority);
		RE::GFxValue flag = sectionFlag | categoryFlag;
		a_entryObject.SetMember("filterFlag", flag);
	}
	void CategoryManager::GetCategoryArgs(RE::BSTArray<RE::GFxValue>& a_result) {
		RemoveRedundantFilters();
		constexpr std::uint32_t stride = 5;
		const std::uint32_t numCategories = 1 + static_cast<std::uint32_t>(_currentFilters.size());
		const std::uint32_t numArgs = numCategories * stride;
		a_result.resize(numArgs);
		std::uint32_t i = 0;
		a_result[i++] = AllLabel;
		a_result[i++] = AllFlag;
		a_result[i++] = -1;
		a_result[i++] = _iconSourceAll;
		a_result[i++] = _iconLabelAll;
		for (auto& [flag, data] : _currentFilters) {
			a_result[i++] = data.Label;
			a_result[i++] = flag;
			a_result[i++] = data.Priority;
			a_result[i++] = data.IconSource;
			a_result[i++] = data.IconLabel;
		}
	}
	void CategoryManager::RemoveRedundantFilters() {
		std::vector<std::uint32_t> redundantFilters;
		for (auto& [section, flag] : _sectionFlags) {
			bool hasCategory = false;
			for (auto& [label, category] : section->Categories) {
				if (_categoryFlags.contains(&category)) {
					hasCategory = true;
					break;
				}
			}
			if (!hasCategory) {
				auto otherFilter = flag | OtherCategoryFlag;
				redundantFilters.push_back(otherFilter);
			}
		}
		for (auto& filter : redundantFilters) {
			_currentFilters.erase(filter);
		}
	}
	void CategoryManager::ResetFlags() {
		_nextSectionFlag = StartIndex;
		_nextCategoryFlag = StartIndex;
		_sectionFlags.clear();
		_categoryFlags.clear();
		_currentFilters.clear();
	}
}
