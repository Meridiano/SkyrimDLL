#pragma once

namespace FCUtility {

	RE::Setting* GetFontConfig() {
		if (auto collection = RE::INISettingCollection::GetSingleton(); collection)
			if (auto setting = collection->GetSetting("sFontConfigFile:Fonts"); setting)
				return setting;
		return nullptr;
	}

	#define AlreadyQuoted (b == '"' && e == '"')
	#define MakeDequoted arg.substr(1, s - 2)
	#define MakeQuoted ('"' + arg + '"')

	std::string Quoted(std::string arg) {
		auto s = arg.size();
		if (s < 2) return MakeQuoted;
		char b = arg[0];
		char e = arg[s - 1];
		return AlreadyQuoted ? arg : MakeQuoted;
	}

	std::string Dequoted(std::string arg) {
		auto s = arg.size();
		if (s < 2) return arg;
		char b = arg[0];
		char e = arg[s - 1];
		return AlreadyQuoted ? MakeDequoted : arg;
	}

	#undef AlreadyQuoted
	#undef MakeDequoted
	#undef MakeQuoted

	char* StoreAndReturn(std::vector<std::string>& storage, std::string item) {
		auto index = storage.size();
		storage.push_back(item);
		return storage[index].data();
	}

	bool VerifyExtension(const std::filesystem::directory_entry& entry, const wchar_t* ext) {
		if (!ext) return false;
		auto cmp = entry.path().extension().wstring();
		return (wcsicmp(cmp.data(), ext) == 0);
	}
	
	struct CaseInsensitiveHash {
		std::size_t operator()(const std::string& s) const noexcept {
			auto low = SKSE::stl::utf8_to_utf16(s).value_or(L"");
			for (auto& wch : low) { wch = std::towlower(wch); }
			static std::hash<std::wstring> hasher;
			return hasher(low);
		}
	};

	struct CaseInsensitiveEqual {
		bool operator()(const std::string& a, const std::string& b) const noexcept {
			if (a.size() != b.size()) return false;
			auto aw = SKSE::stl::utf8_to_utf16(a).value_or(L"");
			auto bw = SKSE::stl::utf8_to_utf16(b).value_or(L"");
			return std::ranges::equal(aw, bw, [](wchar_t x, wchar_t y) {
				return std::towlower(x) == std::towlower(y);
			});
		}
	};

}
