#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
using namespace std::literals;
namespace fs = std::filesystem;

#define FindModule(S) reinterpret_cast<std::uintptr_t>(REX::W32::GetModuleHandleW(L#S))
#undef min
#undef max
