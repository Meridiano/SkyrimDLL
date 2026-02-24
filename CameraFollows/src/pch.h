#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
namespace stl = SKSE::stl;
using namespace std::literals;

// my own includes
#include "../lib/mini/ini.h"
#include "../lib/nlohmann/json.hpp"
using JSON = nlohmann::json;
#include "xbyak/xbyak.h"
#define TRAMPOLINE SKSE::GetTrampoline()