#pragma once

#include "hpp/regulars.hpp"

namespace SREPapyrus {
	
	std::string GetVersion(RE::StaticFunctionTag* base) {
		return SKSE::GetPluginVersion().string("-");
	}

	bool IsMatching(RE::StaticFunctionTag* base, std::string input, std::string filename, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		SREUtility::MatchStruct arguments = SREUtility::MakeMatchStruct(input, filename, lineP, lineMA, lineMB);
		return SRERegulars::RegIsMatching(arguments);
	}

	int32_t MatchCount(RE::StaticFunctionTag* base, std::string input, std::string filename, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		SREUtility::MatchStruct arguments = SREUtility::MakeMatchStruct(input, filename, lineP, lineMA, lineMB);
		return SRERegulars::RegMatchCount(arguments);
	}

	std::vector<std::int32_t> MatchInfo(RE::StaticFunctionTag* base, std::string input, std::string filename, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		SREUtility::MatchStruct arguments = SREUtility::MakeMatchStruct(input, filename, lineP, lineMA, lineMB);
		return SRERegulars::RegMatchInfo(arguments);
	}

	std::vector<std::string> MatchData(RE::StaticFunctionTag* base, std::string input, std::string filename, std::int32_t lineP, std::int32_t lineMA, std::int32_t lineMB) {
		SREUtility::MatchStruct arguments = SREUtility::MakeMatchStruct(input, filename, lineP, lineMA, lineMB);
		return SRERegulars::RegMatchData(arguments);
	}

	std::string MatchResult(RE::StaticFunctionTag* base, std::vector<std::int32_t> info, std::vector<std::string> data, std::int32_t match, std::int32_t group) {
		bool failed = false;
		std::int32_t ii = 0;
		std::int32_t di = 0;
		if (match < 1 || match > info.size()) failed = true;
		else if (group < 0) failed = true;
		else {
			for (ii = 0; ii < info.size(); ii++) {
				std::int32_t gc = info[ii];
				if (ii == match - 1) {
					if (group > gc) failed = true;
					else di += group;
					break;
				} else {
					di += gc;
				}
			}
			if (di >= data.size()) failed = true;
		}
		if (failed) {
			auto infoString = SREUtility::IntVectorToString(info);
			auto dataString = SREUtility::StringVectorToString(data);
			logs::info("MatchResult-Failed >> Begin\n{}\n{}\n{}:{}", infoString, dataString, match, group);
			logs::info("MatchResult-Failed >> End");
			return "";
		}
		return data[di];
	}

	std::string ReplaceWith(RE::StaticFunctionTag* base, std::string input, std::string filename, std::int32_t lineP, std::int32_t lineR, std::int32_t lineMA, std::int32_t lineMB) {
		SREUtility::ReplaceStruct arguments = SREUtility::MakeReplaceStruct(input, filename, lineP, lineR, lineMA, lineMB);
		return SRERegulars::RegReplaceWith(arguments);
	}

	bool Register(RE::BSScript::IVirtualMachine* a_vm) {
		auto className = SKSE::GetPluginName();

		// general
		a_vm->RegisterFunction("GetVersion", className, GetVersion);

		// match
		a_vm->RegisterFunction("IsMatching", className, IsMatching);
		a_vm->RegisterFunction("MatchCount", className, MatchCount);
		a_vm->RegisterFunction("MatchInfo", className, MatchInfo);
		a_vm->RegisterFunction("MatchData", className, MatchData);
		a_vm->RegisterFunction("MatchResult", className, MatchResult);
		
		// replace
		a_vm->RegisterFunction("ReplaceWith", className, ReplaceWith);

		return true;
	}

}