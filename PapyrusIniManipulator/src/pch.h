#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace fs = std::filesystem;
namespace logs = SKSE::log;
namespace stl = SKSE::stl;

using namespace std::literals;

// my own includes
#include "../lib/mini/ini.h"
static const std::string tab(4, ' ');
static const std::string d_tab(tab + tab);
