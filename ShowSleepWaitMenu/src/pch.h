#pragma once

// #pragma warning(push) // optional

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include <xbyak/xbyak.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// #pragma warning(pop) // optional

#include "Plugin.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

#define DLLEXPORT __declspec(dllexport)

using namespace std::literals;

namespace stl {

	using namespace SKSE::stl;

	template <class T>
	void write_thunk_call_5(std::uintptr_t a_src) {
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_call<5>(a_src, T::thunk);
	}

	template <class T>
	void write_thunk_call_6(std::uintptr_t a_src) {
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		T::func = *(std::uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
	}

	template <class F, std::size_t index, class T>
	void write_vfunc() {
		REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[index] };
		T::func = vtbl.write_vfunc(T::size, T::thunk);
	}

	template <std::size_t idx, class T>
	void write_vfunc(REL::VariantID id) {
		REL::Relocation<std::uintptr_t> vtbl{ id };
		T::func = vtbl.write_vfunc(idx, T::thunk);
	}

	template <class F, class T>
	void write_vfunc() {
		write_vfunc<F, 0, T>();
	}

	template <class T>
	void write_thunk_jmp(std::uintptr_t a_src) {
		SKSE::AllocTrampoline(14);
		auto& trampoline = SKSE::GetTrampoline();
		T::func = trampoline.write_branch<5>(a_src, T::thunk);
	}

}

namespace logger = SKSE::log;
namespace fs = std::filesystem;

// project-specific
#include <magic_enum.hpp>
