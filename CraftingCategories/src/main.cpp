#include "hpp/Data.hpp"
#include "hpp/Hooks.hpp"

namespace CCUtility {

	void InitLogger(std::string pattern) {
		logger::init();
		spdlog::set_pattern(pattern);
	}

	void MessageListener(SKSE::MessagingInterface::Message* a_msg) {
		switch (a_msg->type) {
			case SKSE::MessagingInterface::kDataLoaded: {
				// load translation
				SKSE::Translation::ParseTranslation("CraftingCategories");
				// load settings
				Data::ConfigLoader::GetSingleton()->LoadConfigs();
				// done
				break;
			}
		}
	}

}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse, false);
	CCUtility::InitLogger("%d.%m.%Y %H:%M:%S [%s:%#] %v");
	auto plugin = SKSE::PluginDeclaration::GetSingleton();
	logger::info(
		"{} version {} is loading into {}",
		plugin->GetName(),
		plugin->GetVersion().string("."),
		REL::Module::get().version().string(".")
	);
	SKSE::AllocTrampoline(128);
	if (Hooks::Crafting::Install()) {
		// start listener
		SKSE::GetMessagingInterface()->RegisterListener(CCUtility::MessageListener);
		return true;
	}
	return false;
}
