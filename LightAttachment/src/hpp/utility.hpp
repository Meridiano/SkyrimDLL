#pragma once

namespace LiAtUtility {

	static RE::NiUpdateData updateData{ 0.0F, RE::NiUpdateData::Flag::kDirty };

	bool IsPC1stPerson() {
		static auto camera = RE::PlayerCamera::GetSingleton();
		static auto id = REL::VariantID(21147, 21600, 0x2ED9A0);
		static REL::Relocation<bool(*)(RE::PlayerCamera*, std::int64_t, std::int64_t, double*)> function{ id };
		double value = 0.0;
		function(camera, 0, 0, &value);
		return (value != 0.0);
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
				if (auto& lightChildren = lightNode->children; lightChildren.size()) {
					if (auto lightObject = lightChildren[0].get(); lightObject) {
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