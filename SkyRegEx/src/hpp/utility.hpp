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
		std::vector<std::wstring> result;
		std::string path = std::format("Data\\SKSE\\Plugins\\SkyRegEx\\{}.txt", filename);
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
			result += std::format("{}, ", element);
		}
		result += std::format("size = {} ]", data.size());
		return result;
	}

	std::string StringVectorToString(std::vector<std::string> data) {
		std::string result = "[ ";
		for (std::string element : data) {
			result += std::format("\"{}\", ", element);
		}
		result += std::format("size = {} ]", data.size());
		return result;
	}

}