#pragma once

namespace CamFolUtility {

	// settings
	bool ProcessEnabled{ false };
	std::string TransformNode{ "" };
	std::string ConsoleCommand{ "" };

	bool EqualNoCase(const std::string one, const std::string two) {
		std::size_t length = one.length();
		if (two.length() != length) return false;
		return (strnicmp(one.data(), two.data(), length) == 0);
	}

	std::string SetProcessEnabled(std::string data) {
		bool t = EqualNoCase(data, "1") || EqualNoCase(data, "true");
		if (t) {
			ProcessEnabled = true;
			return "CamFol.Success >> Enabled";
		}
		bool f = EqualNoCase(data, "0") || EqualNoCase(data, "false");
		if (f) {
			ProcessEnabled = false;
			return "CamFol.Success >> Disabled";
		}
		ProcessEnabled = false;
		return "CamFol.Error >> Disabled";
	}

	void ReadConfig(std::string pluginName) {
		std::string data = "";
		std::string result = "";
		std::string path = std::format("Data\\SKSE\\Plugins\\{}.ini", pluginName);

		// read from ini
		mINI::INIFile file(path);
		if (mINI::INIStructure ini; file.read(ini)) {
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
	T* GetMember(const void* base, std::ptrdiff_t offset) {
		auto address = std::uintptr_t(base) + offset;
		auto reloc = REL::Relocation<T*>(address);
		return reloc.get();
	}

	template<typename Func>
	auto WriteFunctionHook(std::uint64_t id, std::size_t copyCount, Func destination) {
		const auto target = REL::ID(id).address();
		if (REL::make_pattern<"E8">().match(target)) return TRAMPOLINE.write_call<5>(target, destination);
		if (REL::make_pattern<"E9">().match(target)) return TRAMPOLINE.write_branch<5>(target, destination);
		struct XPatch: Xbyak::CodeGenerator {
			using ull = unsigned long long;
			using uch = unsigned char;
			uch workspace[64];
			XPatch(std::uintptr_t baseAddress, ull bytesCount): Xbyak::CodeGenerator(bytesCount + 14, workspace) {
				auto bytePtr = reinterpret_cast<uch*>(baseAddress);
				for (ull i = 0; i < bytesCount; i++) db(*bytePtr++);
				jmp(qword[rip]);
				dq(ull(bytePtr));
			}
		};
		XPatch patch(target, copyCount);
		patch.ready();
		auto patchSize = patch.getSize();
		TRAMPOLINE.write_branch<5>(target, destination);
		auto alloc = TRAMPOLINE.allocate(patchSize);
		std::memcpy(alloc, patch.getCode(), patchSize);
		return reinterpret_cast<std::uintptr_t>(alloc);
	}

	bool IsInFirstPerson(RE::PlayerCamera* camera) {
		using type = bool(*)(RE::PlayerCamera*, std::int64_t, std::int64_t, double*);
		static REL::Relocation<type> func{ REL::VariantID(21147, 21600, 0x2ED9A0) };
		double value = 0.0;
		func(camera, 0, 0, &value);
		return (value != 0.0);
	}

	bool ProcessConditions(RE::PlayerCamera* camera, std::uint32_t* controls, RE::UI* userInterface, RE::PlayerCharacter* player, RE::VATS* vats) {
		if (ProcessEnabled) {
			bool a = IsInFirstPerson(camera);
			bool b = !camera->IsInFreeCameraMode();
			bool c = (*controls & (std::uint32_t)RE::ControlMap::UEFlag::kMovement);
			bool d = !userInterface->IsMenuOpen(RE::MapMenu::MENU_NAME);
			bool e = !player->GetOccupiedFurniture().get().get();
			bool f = !player->IsInKillMove();
			bool g = (vats->mode != RE::VATS::VATS_MODE::kKillCam);
			return (a && b && c && d && e && f && g);
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

	RE::NiPoint3 GetHighHeelsVector(RE::PlayerCharacter* player) {
		RE::NiPoint3 result{ 0.0F, 0.0F, 0.0F };
		static RE::NiPoint3 nullResult = result;
		static auto dataOld = "HH_OFFSET";
		static auto dataNew = "SDTA";
		if (auto& biped = player->GetBiped(false); biped) {
			auto& feet = biped->objects[RE::BIPED_OBJECT::kFeet].partClone;
			if (auto feetNode = feet ? feet->AsNode() : nullptr; feetNode) {
				std::set<RE::NiAVObject*> objectSet;
				GetAllChildren(feetNode, objectSet);
				for (auto& object : objectSet) if (object) {
					if (object->HasExtraData(dataOld)) {
						if (auto hhOffset = object->GetExtraData<RE::NiFloatExtraData>(dataOld); hhOffset) {
							result.z = hhOffset->value;
						}
					} else if (object->HasExtraData(dataNew)) {
						if (auto sdta = object->GetExtraData<RE::NiStringExtraData>(dataNew); sdta) {
							JSON json = JSON::parse(sdta->value, nullptr, false, false);
							if (!json.is_discarded()) {
								for (auto& element : json) {
									if (element.contains("name") && element["name"] == "NPC" && element.contains("pos")) {
										result.x = element["pos"][0];
										result.y = element["pos"][1];
										result.z = element["pos"][2];
										break;
									}
								}
							}
						}
					}
					if (result != nullResult) break;
				}
			}
		}
		return result;
	}

	bool UpdateCamera(RE::NiAVObject* object, RE::PlayerCamera* camera, RE::PlayerCharacter* player) {
		// get camera stuff
		RE::NiNode* cameraRootNode = camera->cameraRoot.get();
		RE::NiAVObject* cameraNetImmerse = (cameraRootNode && cameraRootNode->children.size()) ? cameraRootNode->children[0].get() : nullptr;
		if (std::uint8_t process = object == cameraRootNode ? 1 : object == cameraNetImmerse ? 2 : 0; process) {
			// this will be reused
			RE::NiAVObject* tempO = nullptr;
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
			if (playerRootNode && playerCameraNode && playerTransformNode) {
				// set what we need to apply
				auto vCameraNode = playerCameraNode->local.translate * playerCameraNode->local.scale;
				if (vCameraNode.Length() > 0.95F) {
					auto vHighHeels = GetHighHeelsVector(player);
					auto vPlayerNode = playerRootNode->world.translate;
					auto vApply = (vCameraNode + vHighHeels) * playerTransformNode->world.scale + vPlayerNode;
					// push translate vectors
					switch (process) {
						case 1:
							cameraRootNode->world.translate = vApply;
							cameraRootNode->local.translate = vApply;
							return true;
						case 2:
							cameraNetImmerse->world.translate = vApply;
							return true;
					}
				}
			}
		}
		return false;
	}

}
