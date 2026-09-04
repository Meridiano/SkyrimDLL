#pragma once

namespace PluginUtility {

	bool NopCall(REL::VariantID id, REL::VariantOffset offset) {
		REL::Relocation target{ id, offset };
		if (auto address = target.address(); GET_VALUE(std::uint8_t, address) == 0xE8) {
			REL::safe_write(address, REL::NOP5, 5);
			return true;
		}
		return false;
	}

}
