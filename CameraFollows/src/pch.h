#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace WinAPI = SKSE::WinAPI;
namespace logs = SKSE::log;
namespace stl = SKSE::stl;

using namespace std::literals;

// logging
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// my own includes
#include "mini/ini.h"
#include <nlohmann/json.hpp>
using JSON = nlohmann::json;
