#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
using namespace std::literals;

#include "glaze/json.hpp"

#define MODULE REL::Module::get()
#define SignMatch(LOC,SIG) REL::make_pattern<SIG>().match(LOC.address())
#define FatalError(...) SKSE::stl::report_and_fail(std::format(__VA_ARGS__))
#define ScopedEnum(N,T,...) struct N##Scope { enum N##Enum : T { __VA_ARGS__ }; }; using N = N##Scope::N##Enum
