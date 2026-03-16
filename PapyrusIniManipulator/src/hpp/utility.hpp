#pragma once

namespace PIMUtility {
	
	std::string PluginConfigPath() {
		const auto pluginName = SKSE::GetPluginName();
		return std::format("Data\\SKSE\\Plugins\\{}.ini", pluginName);
	}

	bool EqualStrings(std::string a, std::string b, bool noCase) {
		auto length = a.length();
		if (b.length() == length) {
			if (noCase) return (strnicmp(a.data(), b.data(), length) == 0);
			else return (strncmp(a.data(), b.data(), length) == 0);
		}
		return false;
	}

	std::string BoolToString(bool value) {
		return (value ? "true" : "false");
	}
	
	std::vector<std::string> StringToVector(std::string value) {
		std::vector<std::string> result;
		std::wstring ws = stl::utf8_to_utf16(value).value_or(L"");
		for (wchar_t wc : ws) {
			std::wstring element(1, wc);
			result.push_back(stl::utf16_to_utf8(element).value_or(""));
		}
		return result;
	}

	bool StringToBool(std::string value, bool fallback) {
		bool t = EqualStrings(value, "true", true) || EqualStrings(value, "1", true);
		if (t) return true;
		bool f = EqualStrings(value, "false", true) || EqualStrings(value, "0", true);
		if (f) return false;
		return fallback;
	}

	bool FileExists(std::string path, bool create) {
		fs::path target(path);
		if (fs::exists(target)) return true;
		if (create) {
			fs::path targetRoot = target.root_path();
			fs::path targetParent = target.parent_path();
			if (targetRoot.compare(targetParent) != 0) {
				// need to check folders
				if (!targetRoot.string().empty() && !fs::exists(targetRoot)) {
					// path is absolute but local drive not found
					return false;
				}
				if (!fs::exists(targetParent) && !fs::create_directories(targetParent)) {
					// failed to create required folders
					return false;
				}
			}
			std::ofstream file(target);
			file << "";
			file.close();
			return fs::exists(target);
		}
		return false;
	}

}
