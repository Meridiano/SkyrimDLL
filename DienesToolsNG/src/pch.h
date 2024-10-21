#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace WinAPI = REX::W32;
namespace logger = SKSE::log;
namespace fs = std::filesystem;
namespace stl = SKSE::stl;

using namespace std::literals;

#include <shlobj_core.h>
#undef AddForm
