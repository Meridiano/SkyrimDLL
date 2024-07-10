#pragma once

#include "hpp/RE.hpp"

namespace Hooks {
	//Crafting.h
	class Crafting {
	public:
		using WorkBenchType = RE::TESFurniture::WorkBenchData::BenchType;
		Crafting() = delete;
		static bool Install();
		static bool LoadMoviePatch();
		static bool CustomCategoryPatch();
	private:
		static bool CheckFurniture(RE::TESObjectREFR* a_refr);
		static bool LoadMovie(
			RE::BSScaleformManager* a_scaleformManager,
			RE::IMenu* a_menu,
			RE::GPtr<RE::GFxMovieView>& a_viewOut,
			const char* a_fileName,
			RE::GFxMovieView::ScaleModeType a_mode,
			float a_backgroundAlpha);
		static void SetItemEntryData(
			RE::CraftingSubMenus::ConstructibleObjectMenu* a_menu,
			RE::BSTArray<RE::CraftingSubMenus::ConstructibleObjectMenu::ItemEntry>& a_entries);
		inline static WorkBenchType _currentWorkBenchType = WorkBenchType::kNone;
		inline static REL::Relocation<decltype(&CheckFurniture)> _IsFurniture;
		inline static REL::Relocation<decltype(&LoadMovie)> _LoadMovie;
		inline static REL::Relocation<decltype(&SetItemEntryData)> _SetItemEntryData;
	};
	//Crafting.cpp
	bool Crafting::Install() {
		return (LoadMoviePatch() && CustomCategoryPatch());
	}
	bool Crafting::LoadMoviePatch() {
		static const auto hook1 = REL::Relocation<std::uintptr_t>(
			RE::Offset::TESObjectREFR::ActivateCraftingWorkBench,
			RE::Offset::TESObjectREFR::ActivateCraftingWorkBenchOffset
		);
		static const auto hook2 = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingMenu::Ctor,
			RE::Offset::CraftingMenu::CtorOffset
		);
		if (!REL::make_pattern<"E8">().match(hook1.address()) || !REL::make_pattern<"E8">().match(hook2.address())) {
			logger::info("Failed to install Crafting::LoadMoviePatch");
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_IsFurniture = trampoline.write_call<5>(hook1.address(), &Crafting::CheckFurniture);
		_LoadMovie = trampoline.write_call<5>(hook2.address(), &Crafting::LoadMovie);
		return true;
	}
	bool Crafting::CustomCategoryPatch() {
		static const auto hook = REL::Relocation<std::uintptr_t>(
			RE::Offset::CraftingSubMenus::ConstructibleObjectMenu::UpdateItemList,
			RE::Offset::CraftingSubMenus::ConstructibleObjectMenu::UpdateItemListOffset
		);
		if (!REL::make_pattern<"E8">().match(hook.address())) {
			logger::info("Failed to install Crafting::CustomCategoryPatch");
			return false;
		}
		auto& trampoline = SKSE::GetTrampoline();
		_SetItemEntryData = trampoline.write_call<5>(hook.address(), &Crafting::SetItemEntryData);
		return true;
	}
	bool Crafting::CheckFurniture(RE::TESObjectREFR* a_refr) {
		auto isFurniture = _IsFurniture(a_refr);
		if (isFurniture) {
			auto furniture = static_cast<RE::TESFurniture*>(a_refr->data.objectReference);
			_currentWorkBenchType = furniture->workBenchData.benchType.get();
		}
		return isFurniture;
	}
	bool Crafting::LoadMovie(
		RE::BSScaleformManager* a_scaleformManager, RE::IMenu* a_menu,
		RE::GPtr<RE::GFxMovieView>& a_viewOut, const char* a_fileName,
		RE::GFxMovieView::ScaleModeType a_mode, float a_backgroundAlpha) {
		auto fileName = a_fileName;
		switch (_currentWorkBenchType) {
			case WorkBenchType::kCreateObject:
				fileName = "ConstructibleObjectMenu";
				break;
			default: break;
		}
		_currentWorkBenchType = WorkBenchType::kNone;
		return _LoadMovie(a_scaleformManager, a_menu, a_viewOut, fileName, a_mode, a_backgroundAlpha);
	}
	void Crafting::SetItemEntryData(RE::CraftingSubMenus::ConstructibleObjectMenu* a_menu, RE::BSTArray<RE::CraftingSubMenus::ConstructibleObjectMenu::ItemEntry>& a_entries) {
		_SetItemEntryData(a_menu, a_entries);
		auto& entryList = a_menu->entryList;
		assert(entryList.GetArraySize() == a_entries.size());
		auto categoryManager = Data::CategoryManager::GetSingleton();
		categoryManager->ResetFlags();
		for (std::uint32_t i = 0; i < a_entries.size(); i++) {
			RE::GFxValue entryObject;
			entryList.GetElement(i, &entryObject);
			categoryManager->ProcessEntry(
				entryObject,
				a_entries[i].constructibleObject->createdItem);
		}
		RE::BSTArray<RE::GFxValue> categoryArgs;
		categoryManager->GetCategoryArgs(categoryArgs);
		a_menu->view->Invoke(
			"Menu.InventoryLists.SetCustomConstructCategories",
			nullptr,
			categoryArgs.data(),
			categoryArgs.size()
		);
	}
}
