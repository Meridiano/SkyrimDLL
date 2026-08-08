#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
using namespace std::literals;
namespace fs = std::filesystem;

// custom things
#define NOMINMAX
#include "SimpleIni.h"
#define CameraData2 camera->GetRuntimeData2()
