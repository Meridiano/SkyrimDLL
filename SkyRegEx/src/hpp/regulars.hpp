#pragma once

#include "hpp/utility.hpp"

namespace SRERegulars {

	bool RegIsMatching(SREUtility::MatchStruct arguments) {
		bool result = false;
		jp::Regex re;
		re.setPattern(arguments.pattern).setModifier(arguments.modA).compile();
		if (re) {
			result = (re.match(arguments.input, arguments.modB, 0) > 0);
		} else {
			std::string info = stl::utf16_to_utf8(arguments.input + L"\n" + arguments.pattern).value_or("");
			logs::info("IsMatching-Failed >> Begin\n{}\n{}\n{}", info, arguments.modA, arguments.modB);
			logs::info("IsMatching-Failed >> End");
		}
		return result;
	}

	std::int32_t RegMatchCount(SREUtility::MatchStruct arguments) {
		std::int32_t result = 0;
		jp::Regex re;
		re.setPattern(arguments.pattern).setModifier(arguments.modA).compile();
		if (re) {
			result = re.match(arguments.input, arguments.modB, 0);
		} else {
			std::string info = stl::utf16_to_utf8(arguments.input + L"\n" + arguments.pattern).value_or("");
			logs::info("MatchCount-Failed >> Begin\n{}\n{}\n{}", info, arguments.modA, arguments.modB);
			logs::info("MatchCount-Failed >> End");
		}
		return result;
	}

	std::vector<std::int32_t> RegMatchInfo(SREUtility::MatchStruct arguments) {
		bool fail = false;
		jp::Regex re;
		jp::VecNum vn;
		jp::RegexMatch rm;
		re.setPattern(arguments.pattern).setModifier(arguments.modA).compile();
		if (re) {
			std::int32_t count = rm.setRegexObject(&re).setSubject(&arguments.input).setModifier(arguments.modB).setNumberedSubstringVector(&vn).match();
			if (count > 0) {
				std::vector<std::int32_t> result;
				for (auto match : vn) {
					std::int32_t element = match.size();
					result.push_back(element);
				}
				return result;
			} else fail = true;
		} else fail = true;
		if (fail) {
			std::string info = stl::utf16_to_utf8(arguments.input + L"\n" + arguments.pattern).value_or("");
			logs::info("MatchInfo-Failed >> Begin\n{}\n{}\n{}", info, arguments.modA, arguments.modB);
			logs::info("MatchInfo-Failed >> End");
		}
		return {};
	}

	std::vector<std::string> RegMatchData(SREUtility::MatchStruct arguments) {
		bool fail = false;
		jp::Regex re;
		jp::VecNum vn;
		jp::RegexMatch rm;
		re.setPattern(arguments.pattern).setModifier(arguments.modA).compile();
		if (re) {
			std::int32_t count = rm.setRegexObject(&re).setSubject(&arguments.input).setModifier(arguments.modB).setNumberedSubstringVector(&vn).match();
			if (count > 0) {
				std::vector<std::string> result;
				for (auto match : vn) {
					for (auto group : match) {
						std::string element = stl::utf16_to_utf8(group).value_or("");
						result.push_back(element);
					}
				}
				return result;
			} else fail = true;
		} else fail = true;
		if (fail) {
			std::string info = stl::utf16_to_utf8(arguments.input + L"\n" + arguments.pattern).value_or("");
			logs::info("MatchData-Failed >> Begin\n{}\n{}\n{}", info, arguments.modA, arguments.modB);
			logs::info("MatchData-Failed >> End");
		}
		return {};
	}

	std::string RegReplaceWith(SREUtility::ReplaceStruct arguments) {
		jp::Regex re;
		jp::RegexReplace rr;
		re.setPattern(arguments.pattern).setModifier(arguments.modA).compile();
		if (re) {
			std::wstring result = rr.setRegexObject(&re).setSubject(&arguments.input).setReplaceWith(&arguments.replace).setModifier(arguments.modB).replace();
			return stl::utf16_to_utf8(result).value_or("");
		} else {
			std::string info = stl::utf16_to_utf8(arguments.input + L"\n" + arguments.pattern + L"\n" + arguments.replace).value_or("");
			logs::info("ReplaceWith-Failed >> Begin\n{}\n{}\n{}", info, arguments.modA, arguments.modB);
			logs::info("ReplaceWith-Failed >> End");
		}
		return {};
	}

}