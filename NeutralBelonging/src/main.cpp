bool Install(REL::VariantID id, std::size_t count, std::string compare, std::string replace) {
	REL::Relocation<const char*> target{ id };
	if (strncmp(target.get(), compare.data(), count)) return false;
	REL::safe_write(target.address(), replace.data(), count);
	return true;
}

std::size_t TweakBelonging() {
	std::size_t result = 0;
	// TESNPC
	REL::VariantID tesNPC(241895, 195857, 0x16177C8);
	result += Install(tesNPC, 7, "%s's %s", "%s - %s");
	// TESObjectCONT
	REL::VariantID tesObjectCONT(234183, 189647, 0x15D1A54);
	result += Install(tesObjectCONT, 3, "'s ", " - ");
	// done
	return result;
}

void ProcessMessage(SKSE::MessagingInterface::Message* a_msg) {
	if (a_msg->type == SKSE::MessagingInterface::kPostLoad) {
		logs::info("Plugin loaded, {} of {} tweaks applied", TweakBelonging(), 2);
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	logs::init();
	logs::info(
		"{} version {} is loading into SkyrimSE {}",
		SKSE::GetPluginName(),
		SKSE::GetPluginVersion().string("."),
		a_skse->RuntimeVersion().string(".")
	);
	const auto skseMessaging = SKSE::GetMessagingInterface();
	return (skseMessaging && skseMessaging->RegisterListener(ProcessMessage));
}