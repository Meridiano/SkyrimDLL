#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace fs = std::filesystem;
namespace logs = SKSE::log;
namespace stl = SKSE::stl;
using namespace std::literals;

// my own includes
#include "../lib/jpcre2/jpcre2.hpp"
using jp = jpcre2::select<wchar_t>;
