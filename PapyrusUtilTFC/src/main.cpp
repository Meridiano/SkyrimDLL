#include "hpp/config.hpp"
#include "hpp/papyrus.hpp"
#include "hpp/utility.hpp"

void InvertedPatch() {
	std::uint8_t result = 0;

	// up to 1.6.659 GOG, AE ID is 22911
	// up to 1.6.659 GOG, offsets are 0xB4 and 0xC0, even in SE and VR
	// starting from 1.6.1130, AE ID is 441593
	// starting from 1.6.1130, offsets are 0xC9 and 0xD0

	bool newModule = MODULE.version() > SKSE::RUNTIME_SSE_1_6_659;
	REL::VariantID id(22436, newModule ? 441593 : 22911, 0x326280);
	std::size_t value_a = newModule ? 0xC9 : 0xB4;
	std::size_t value_b = newModule ? 0xD0 : 0xC0;
	REL::VariantOffset offset_a(value_a, value_a, value_a);
	REL::VariantOffset offset_b(value_b, value_b, value_b);
	result += PluginUtility::NopCall(id, offset_a);
	result += PluginUtility::NopCall(id, offset_b);
	logs::info("Inverted approach {} result = {} / {}", newModule ? "V2" : "V1", result, 2);
}

void PapyrusPatch() {
	auto papyrusUtil = FIND_MODULE("PapyrusUtil");
	if (papyrusUtil) {
		const auto papInterface = SKSE::GetPapyrusInterface();
		if (papInterface) papInterface->Register(PapyrusFixed::PapyrusOverride);
		else SKSE::stl::report_and_fail("Script interface not found");
	} else SKSE::stl::report_and_fail("PapyrusUtil module not found");
	logs::info("Papyrus override registered");
}

void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
	switch (a_msg->type) {
		case SKSE::MessagingInterface::kPostLoad:
			if (PluginConfig::bInvertedApproach.value) InvertedPatch();
			break;
		case SKSE::MessagingInterface::kPostPostLoad:
			if (PluginConfig::bInvertedApproach.value) {
				logs::info("Inverted approach is used, Papyrus patch disabled");
				break;
			}
			PapyrusPatch();
			break;
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, true);

	auto moduleName = fs::path(MODULE.filename()).replace_extension("").string();
	auto moduleVersion = MODULE.version().string("-");
	logs::info("{} v{}", moduleName, moduleVersion);

	PluginConfig::ReadConfig();

	const auto msgInterface = SKSE::GetMessagingInterface();
	return (msgInterface ? msgInterface->RegisterListener(MessageListener) : false);
}
