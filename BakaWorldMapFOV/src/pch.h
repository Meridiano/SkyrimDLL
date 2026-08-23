#pragma once

#define NOMINMAX
#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

namespace logs = SKSE::log;
using namespace std::literals;
namespace fs = std::filesystem;

// custom things
#include "SimpleIni.h"
#define MapData2 map->GetRuntimeData2()
#define CameraData2 camera->GetRuntimeData2()
