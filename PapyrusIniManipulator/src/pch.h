#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace WinAPI = REX::W32;
namespace fs = std::filesystem;
namespace logs = SKSE::log;
namespace stl = SKSE::stl;

using namespace std::literals;

// my own includes
#include "mini/ini.h"

// logger
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
