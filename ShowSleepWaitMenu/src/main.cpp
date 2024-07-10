void InitializeLog(spdlog::level::level_enum a_level) {
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory");
	}
	*path /= std::format("{}.log", Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
	const auto level = a_level;
	auto log = std::make_shared<spdlog::logger>("global log", std::move(sink));
	log->set_level(level);
	log->flush_on(spdlog::level::info);
	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%d.%m.%Y %H:%M:%S [%s:%#] %v");
}

namespace PapyrusNS {

	void ShowSleepWaitMenu(RE::StaticFunctionTag* base, bool sleep) {
		reinterpret_cast<std::uint64_t>(base);
		using func_t = void(*)(bool);
		REL::Relocation<func_t> func{
			REL::VariantID(
				51618,   // SE
				52490,   // AE
				0x8EAD70 // VR
			)
		};
		return func(sleep);
	}

	std::string GetPluginVersion(RE::StaticFunctionTag* base) {
		reinterpret_cast<std::uint64_t>(base);
		return Plugin::VERSION.string(".");
	}

	bool ProcessVirtualMachine(RE::BSScript::IVirtualMachine* virtualMachine) {
		if (virtualMachine) {
			virtualMachine->RegisterFunction("ShowSleepWaitMenu", "ShowSleepWaitMenuScript", ShowSleepWaitMenu);
			virtualMachine->RegisterFunction("GetPluginVersion", "ShowSleepWaitMenuScript", GetPluginVersion);
			return true;
		}
		return false;
	}

	bool ProcessPapyrusInterface() {
		auto papyrusInterface = SKSE::GetPapyrusInterface();
		if (papyrusInterface && papyrusInterface->Register(ProcessVirtualMachine)) {
			logger::info("Papyrus functions registered");
			return true;
		}
		logger::info("Could not register Papyrus functions");
		return false;
	}

}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse) {
	InitializeLog(spdlog::level::info);
	logger::info(
		"{} version {} is loading into {}",
		Plugin::NAME,
		Plugin::VERSION.string("."),
		REL::Module::get().version()
	);
	SKSE::Init(a_skse);
	return PapyrusNS::ProcessPapyrusInterface();
}

/* ------------------------------------------------------------------------------------------------------------
---------------------------------- THESE AREN'T THE LINES YOU'RE LOOKING FOR ----------------------------------
------------------------------------------------------------------------------------------------------------ */

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME);
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo* pluginInfo) {
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}
