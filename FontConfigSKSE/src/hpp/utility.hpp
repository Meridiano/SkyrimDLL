#pragma once

namespace FCUtility {

	RE::Setting* GetFontConfig() {
		if (auto collection = RE::INISettingCollection::GetSingleton(); collection)
			if (auto setting = collection->GetSetting("sFontConfigFile:Fonts"); setting)
				return setting;
		return nullptr;
	}

	std::string Quoted(std::string arg) {
		auto s = arg.size();
		char b = arg[0];
		char e = arg[s - 1];
		return (b != '"' && e != '"') ? ('"' + arg + '"') : arg;
	}

	std::string Dequoted(std::string arg) {
		auto s = arg.size();
		char b = arg[0];
		char e = arg[s - 1];
		return (b == '"' && e == '"') ? arg.substr(1, s - 2) : arg;
	}

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

}
