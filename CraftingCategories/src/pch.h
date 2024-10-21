#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace WinAPI = REX::W32;
namespace logger = SKSE::log;
namespace stl = SKSE::stl;
namespace fs = std::filesystem;

using namespace std::literals;

//specific
#undef min
#undef max
#include "json/json.h"
#include "include/tsl/ordered_map.h"

struct comp_str_cis {
	bool operator()(const std::string& a_lhs, const std::string& a_rhs) const {
		return ::_stricmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
	}
};

template <typename V>
using KeywordMap = std::map<std::string, V, comp_str_cis>;
using KeywordSet = std::set<std::string, comp_str_cis>;
