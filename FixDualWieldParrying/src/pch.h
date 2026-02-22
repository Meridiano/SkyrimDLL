#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
namespace fs = std::filesystem;
using namespace std::literals;

#include "../lib/okdshin/picosha2.h"
#define FindModule(S) reinterpret_cast<std::uintptr_t>(REX::W32::GetModuleHandleW(S))
#define HashEqual(S1,S2) (S1.size() == 64) && (S1.compare(S2) == 0)
