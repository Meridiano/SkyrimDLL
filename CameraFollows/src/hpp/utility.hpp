#pragma once

namespace CamFolUtility {

	bool ProcessEnabled;
	std::string TransformNode;
	std::string ConsoleCommand;

	bool IsEqual(const std::string one, const std::string two) {
		std::size_t length = one.length();
		if (two.length() != length) return false;
		return (strnicmp(one.data(), two.data(), length) == 0);
	}

	std::string SetProcessEnabled(std::string data) {
		bool t = IsEqual(data, "1") || IsEqual(data, "true");
		if (t) {
			ProcessEnabled = true;
			return "CamFol.Success >> Enabled";
		}
		bool f = IsEqual(data, "0") || IsEqual(data, "false");
		if (f) {
			ProcessEnabled = false;
			return "CamFol.Success >> Disabled";
		}
		ProcessEnabled = false;
		return "CamFol.Error >> Disabled";
	}

	void ReadConfig(const SKSE::PluginDeclaration* plugin) {
		std::string data = "";
		std::string result = "";
		std::string path = std::format("Data\\SKSE\\Plugins\\{}.ini", plugin->GetName());

		// read from ini
		mINI::INIFile file(path);
		mINI::INIStructure ini;
		if (file.read(ini)) {
			// bEnabled
			data = ini.get("General").get("bEnabled");
			result = SetProcessEnabled(data);
			logs::info("Config.General.bEnabled = {}", result);

			// sTransformNode
			data = ini.get("General").get("sTransformNode");
			if (data.length() > 0) result = std::format("CamFol.Success >> {}", data);
			else {
				data = "NPC Root [Root]";
				result = std::format("CamFol.Error >> {}", data);
			}
			TransformNode = data;
			logs::info("Config.General.sTransformNode = {}", result);

			// sConsoleCommand
			data = ini.get("General").get("sConsoleCommand");
			if (data.length() > 0) result = std::format("CamFol.Success >> {}", data);
			else {
				data = "TestFadeNodes";
				result = std::format("CamFol.Error >> {}", data);
			}
			ConsoleCommand = data;
			logs::info("Config.General.sConsoleCommand = {}", result);
		}
	}

	template<typename T>
	T* GetMember(void* base, std::ptrdiff_t offset) {
		auto address = std::uintptr_t(base) + offset;
		auto reloc = REL::Relocation<T*>(address);
		return reloc.get();
	}

	bool IsInFirstPerson(RE::PlayerCamera* camera) {
		using type = bool(*)(RE::PlayerCamera*, std::int64_t, std::int64_t, double*);
		static REL::Relocation<type> func{
			REL::VariantID(21147, 21600, 0x2ED9A0)
		};
		double value = 0.0;
		func(camera, 0, 0, &value);
		return (value != 0.0);
	}

	bool ProcessConditions(RE::PlayerCamera* camera, std::uint32_t* controls, RE::UI* userInterface, RE::PlayerCharacter* player) {
		if (ProcessEnabled) {
			bool a = IsInFirstPerson(camera);
			bool b = *controls & (int)RE::ControlMap::UEFlag::kMovement;
			bool c = !userInterface->IsMenuOpen(RE::MapMenu::MENU_NAME);
			bool d = !player->GetOccupiedFurniture().get().get();
			bool e = !player->IsInKillMove();
			return (a && b && c && d && e);
		}
		return false;
	}

	void GetAllChildren(RE::NiAVObject* object, std::set<RE::NiAVObject*>& result) {
		if (object) result.insert(object);
		auto node = object ? object->AsNode() : nullptr;
		if (node)
			for (auto& child : node->children)
				GetAllChildren(child.get(), result);
	}

	float GetHighHeelsOffset(RE::NiAVObject* object) {
		auto dataOld = "HH_OFFSET";
		auto dataNew = "SDTA";
		float result = 0.0F;
		if (object) {
			if (object->HasExtraData(dataOld)) {
				if (auto hhOffset = object->GetExtraData<RE::NiFloatExtraData>(dataOld); hhOffset) {
					result = hhOffset->value;
				}
			} else if (object->HasExtraData(dataNew)) {
				if (auto sdta = object->GetExtraData<RE::NiStringExtraData>(dataNew); sdta) {
					JSON json = JSON::parse(sdta->value, nullptr, false, false);
					if (!json.is_discarded()) {
						for (auto& element : json) {
							if (element.contains("name") && element["name"] == "NPC" && element.contains("pos")) {
								result = element["pos"][2];
								break;
							}
						}
					}
				}
			}
		}
		return result;
	}

	RE::NiPoint3 GetHighHeelsVector(RE::PlayerCharacter* player) {
		float offset = 0.0F;
		if (auto& biped = player->GetBiped(false); biped) {
			auto& bipedData = biped->objects[RE::BIPED_OBJECT::kFeet];
			if (bipedData.partClone) {
				auto feetObject = bipedData.partClone.get();
				auto feetNode = feetObject ? feetObject->AsNode() : nullptr;
				if (feetNode) {
					std::set<RE::NiAVObject*> objSet;
					GetAllChildren(feetNode, objSet);
					for (auto& obj : objSet) {
						offset = GetHighHeelsOffset(obj);
						if (offset != 0.0F) break;
					}
				}
			}
		}
		return { 0.0F, 0.0F, offset };
	}

	void UpdateCamera(RE::PlayerCamera* camera, RE::PlayerCharacter* player) {
		// this will be reused
		RE::NiNode* tempN = nullptr;
		RE::NiAVObject* tempO = nullptr;

		// get camera stuff
		RE::NiNode* cameraRootNode = nullptr;
		RE::NiAVObject* cameraNetImmerse = nullptr;

		tempN = camera->cameraRoot.get();
		cameraRootNode = tempN ? tempN->AsNode() : nullptr;
		cameraNetImmerse = (cameraRootNode && cameraRootNode->children.size()) ? cameraRootNode->children[0].get() : nullptr;

		// get player stuff
		RE::NiNode* playerRootNode = nullptr;
		RE::NiNode* playerCameraNode = nullptr;
		RE::NiNode* playerTransformNode = nullptr;

		tempO = player->Get3D(true);
		playerRootNode = tempO ? tempO->AsNode() : nullptr;
		tempO = playerRootNode ? playerRootNode->GetObjectByName("Camera1st [Cam1]") : nullptr;
		playerCameraNode = tempO ? tempO->AsNode() : nullptr;
		tempO = playerRootNode ? playerRootNode->GetObjectByName(TransformNode) : nullptr;
		playerTransformNode = tempO ? tempO->AsNode() : nullptr;

		if (cameraRootNode && cameraNetImmerse && playerRootNode && playerCameraNode && playerTransformNode) {
			// set what we need to apply
			auto vCameraNode = playerCameraNode->local.translate * playerCameraNode->local.scale;
			if (vCameraNode.Length() > 0.95F) {
				auto vHighHeels = GetHighHeelsVector(player);
				auto vPlayerNode = playerRootNode->world.translate;
				auto vApply = (vCameraNode + vHighHeels) * playerTransformNode->world.scale + vPlayerNode;

				// push translate vectors
				cameraNetImmerse->world.translate = cameraRootNode->world.translate = cameraRootNode->local.translate = vApply;

				// update them both
				RE::NiUpdateData updateData;
				updateData.flags = RE::NiUpdateData::Flag::kDirty;
				updateData.time = 0.0F;
				cameraNetImmerse->Update(updateData);
				cameraRootNode->Update(updateData);
			}
		}
	}

}
