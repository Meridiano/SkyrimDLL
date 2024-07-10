#pragma once

namespace SREUtility {

	struct MatchStruct {
		std::wstring input;
		std::wstring pattern;
		std::string modA;
		std::string modB;
	};

	struct ReplaceStruct {
		std::wstring input;
		std::wstring pattern;
		std::wstring replace;
		std::string modA;
		std::string modB;
	};

	std::vector<std::wstring> ReadFile(std::string filename) {
		namespace fs = std::filesystem;
		std::vector<std::wstring> result;
		std::string path = fmt::format("Data\\SKSE\\Plugins\\SkyRegEx\\{}.txt", filename);
		fs::path target(path);
		if (fs::exists(target)) {
			std::ifstream input(target);
			std::string strN;
			while (std::getline(input, strN)) {
				std::wstring strW = stl::utf8_to_utf16(strN).value_or(L"");
				result.push_back(strW);
			}
			input.close();
		}
		std::wstring size = std::to_wstring(result.size());
		result.insert(result.begin(), size);
		return result;
	}

	bool IsValidLine(std::int32_t line, std::int32_t max) {
		if (line < 1 || line > max) return false;
		return true;
	}

	bool IsValidMatch(std::vector<std::wstring> data, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		std::int32_t max;
		try {
			max = std::stoi(data[0]);
		} catch (...) {
			max = 0;
		}
		bool p = IsValidLine(lineP, max);
		bool ma = IsValidLine(lineMA, max);
		bool mb = IsValidLine(lineMB, max);
		return (p && ma && mb);
	}

	bool IsValidReplace(std::vector<std::wstring> data, std::int32_t lineP, std::int32_t lineR, std::int32_t lineMA, std::int32_t lineMB) {
		std::int32_t max;
		try {
			max = std::stoi(data[0]);
		} catch (...) {
			max = 0;
		}
		bool p = IsValidLine(lineP, max);
		bool r = IsValidLine(lineR, max);
		bool ma = IsValidLine(lineMA, max);
		bool mb = IsValidLine(lineMB, max);
		return (p && r && ma && mb);
	}

	MatchStruct MakeMatchStruct(std::string input, std::string filename, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		MatchStruct result;
		std::vector<std::wstring> data = ReadFile(filename);
		if (IsValidMatch(data, lineP, lineMA, lineMB)) {
			result.input = stl::utf8_to_utf16(input).value_or(L"");
			result.pattern = data[lineP];
			result.modA = stl::utf16_to_utf8(data[lineMA]).value_or("");
			result.modB = stl::utf16_to_utf8(data[lineMB]).value_or("");
		}
		return result;
	}

	ReplaceStruct MakeReplaceStruct(std::string input, std::string filename, std::int32_t lineP, std::int32_t lineR, std::int32_t lineMA, std::int32_t lineMB) {
		ReplaceStruct result;
		std::vector<std::wstring> data = ReadFile(filename);
		if (IsValidReplace(data, lineP, lineR, lineMA, lineMB)) {
			result.input = stl::utf8_to_utf16(input).value_or(L"");
			result.pattern = data[lineP];
			result.replace = data[lineR];
			result.modA = stl::utf16_to_utf8(data[lineMA]).value_or("");
			result.modB = stl::utf16_to_utf8(data[lineMB]).value_or("");
		}
		return result;
	}

	std::string IntVectorToString(std::vector<std::int32_t> data) {
		std::string result = "[ ";
		for (std::int32_t element : data) {
			result += fmt::format("{}, ", element);
		}
		result += fmt::format("size = {} ]", data.size());
		return result;
	}

	std::string StringVectorToString(std::vector<std::string> data) {
		std::string result = "[ ";
		for (std::string element : data) {
			result += fmt::format("\"{}\", ", element);
		}
		result += fmt::format("size = {} ]", data.size());
		return result;
	}

	std::filesystem::path GetDocumentsDirectory() {
		wchar_t* docPath = NULL;
		auto resA = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &docPath);
		if (resA == S_OK) {
			std::wstring docPathWS(docPath);
			return std::filesystem::path(docPathWS);
		} else {
			auto resB = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT_PATH, NULL, &docPath);
			if (resB == S_OK) {
				std::wstring docPathWS(docPath);
				return std::filesystem::path(docPathWS);
			}
		}
		return std::filesystem::path("");
	}

	std::filesystem::path GetLogsDirectory() {
		std::filesystem::path gamesDirectory = GetDocumentsDirectory() / "My Games";
		std::filesystem::path skyrimDirectory;
		if (REL::Module::IsVR()) {
			skyrimDirectory = gamesDirectory / "Skyrim VR";
		} else {
			auto steam = std::filesystem::path("steam_api64.dll");
			auto special = std::filesystem::exists(steam) ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			skyrimDirectory = gamesDirectory / special;
		}
		std::filesystem::path logsDirectory = skyrimDirectory / "SKSE";
		return logsDirectory;
	}

}