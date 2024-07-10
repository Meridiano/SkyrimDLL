#pragma once

namespace LiAtUtility {

	static RE::NiUpdateData updateData;
	
	fs::path GetDocumentsDirectory() {
		wchar_t* docPath = NULL;
		auto resA = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &docPath);
		if (resA == S_OK) {
			std::wstring docPathWS(docPath);
			return fs::path(docPathWS);
		} else {
			auto resB = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT_PATH, NULL, &docPath);
			if (resB == S_OK) {
				std::wstring docPathWS(docPath);
				return fs::path(docPathWS);
			}
		}
		return fs::path();
	}

	std::optional<fs::path> GetLogsDirectory() {
		auto documents = GetDocumentsDirectory();
		if (documents.empty()) return std::nullopt;
		// construct path
		auto gamesDirectory = documents / "My Games";
		fs::path skyrimDirectory;
		if (REL::Module::IsVR()) {
			skyrimDirectory = gamesDirectory / "Skyrim VR";
		} else {
			auto special = fs::exists("steam_api64.dll") ? "Skyrim Special Edition" : "Skyrim Special Edition GOG";
			skyrimDirectory = gamesDirectory / special;
		}
		return (skyrimDirectory / "SKSE");
	}

	void SetUpdateData() {
		updateData.flags = RE::NiUpdateData::Flag::kDirty;
		updateData.time = 0.0F;
	}

	bool IsPC1stPerson() {
		static auto cam = RE::PlayerCamera::GetSingleton();
		static REL::Relocation<bool(*)(RE::PlayerCamera*, std::int64_t, std::int64_t, double*)> func{
			REL::VariantID(21147, 21600, 0x2ED9A0)
		};
		double val; func(cam, 0, 0, &val);
		return (val != 0.0);
	}

	void GetObjectsByName(RE::NiAVObject* obj, RE::BSFixedString name, std::set<RE::NiAVObject*>& result) {
		if (obj && obj->name == name) result.insert(obj);
		auto node = obj ? obj->AsNode() : nullptr;
		if (node)
			for (auto child : node->children)
				GetObjectsByName(child.get(), name, result);
	}

	void ProcessLight(RE::PlayerCharacter* player) {
		std::set<RE::NiAVObject*> lights;
		GetObjectsByName(player->Get3D(true), "AttachLight", lights);
		for (auto light : lights) {
			if (auto lightNode = light->AsNode(); lightNode) {
				auto& lightChildren = lightNode->children;
				if (lightChildren.size()) {
					auto lightObject = lightChildren[0].get();
					if (lightObject) {
						// get node data
						auto nodeT = lightNode->world.translate;
						auto nodeS = lightNode->world.scale;
						auto vNode = nodeT * nodeS;
						// get object data
						auto vObject = lightObject->world.translate * lightObject->world.scale;
						// check for difference
						bool varA = (vNode - vObject).Length() > 0.5F;
						bool varB = vNode.Length() + vObject.Length() < 0.5F;
						if (varA || varB) {
							// push
							lightObject->world.translate = nodeT;
							lightObject->world.scale = nodeS;
							// update
							lightObject->Update(updateData);
						}
					}
				}
			}
		}
	}

}