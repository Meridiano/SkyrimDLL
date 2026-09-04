#pragma once

#define NOMINMAX
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
using namespace std::literals;
namespace fs = std::filesystem;

#include "SimpleIni.h"
#define MODULE REL::Module::get()
#define GET_VALUE(RET,PTR) *std::bit_cast<RET*>(PTR)
#define FIND_MODULE(S) reinterpret_cast<std::uintptr_t>(REX::W32::GetModuleHandleW(L##S))
