#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
namespace stl = SKSE::stl;
using namespace std::literals;

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include "mini/ini.h"

#define EXCEPTION throw "NLS_ERROR"