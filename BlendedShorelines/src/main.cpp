namespace BSUConfig {

	mINI::INIStructure ini{ /* empty */ };
	std::string section{ "General" };
	std::uint8_t falseCount{ 0 };

	struct ConfigStruct {
		const char* sKey = nullptr;
		bool bProcess = false;
		float fValue = 0.0F;
	};

	ConfigStruct Reflections{ "fReflections", true, 1.0F };
	ConfigStruct Refraction{ "fRefraction", true, 1.0F };
	ConfigStruct Normals{ "fNormals", true, 0.2F };
	ConfigStruct SpecularLighting{ "fSpecularLighting", true, 1.0F };

	void ReadOption(ConfigStruct& item) {
		static auto map = ini.get(section);
		std::string key = item.sKey;
		if (map.has(key)) try {
			auto value = std::stof(map.get(key), nullptr);
			logs::info("{} value = {}", key, value);
			item.fValue = value;
		} catch (...) {
			logs::info("{} processing disabled / {}", key, "Conversion error");
			item.bProcess = false;
			falseCount += 1;
		} else {
			logs::info("{} processing disabled / {}", key, "Value is missing");
			item.bProcess = false;
			falseCount += 1;
		}
	}

	void ReadConfig() {
		auto pluginName = SKSE::GetPluginName();
		auto configPath = std::format("Data/SKSE/Plugins/{}.ini", pluginName);
		mINI::INIFile file(configPath);
		if (file.read(ini) && ini.has(section)) {
			ReadOption(Reflections);
			ReadOption(Refraction);
			ReadOption(Normals);
			ReadOption(SpecularLighting);
		} else logs::info("Failed to read config file, all settings are default");
	}

}

namespace BSUProcess {

	void ProcessWaterForms() {
		if (BSUConfig::falseCount == 4) {
			logs::info("All options disabled, nothing to do here");
			return;
		}
		static auto data = RE::TESDataHandler::GetSingleton();
		if (data) {
			std::size_t watersCount = 0;
			std::size_t valuesCount = 0;
			static auto& waters = data->GetFormArray<RE::TESWaterForm>();
			if (waters.size() != 0) for (auto water : waters) if (water) {
				watersCount += 1;
				#define Process(Option, Target) if (BSUConfig::Option.bProcess) { water->data.depthProperties.Target = BSUConfig::Option.fValue; valuesCount += 1; }
				Process(Reflections, reflections);
				Process(Refraction, refraction);
				Process(Normals, normals);
				Process(SpecularLighting, specularLighting);
				#undef Process
			}
			logs::info("Total {} values processed in {} water forms", valuesCount, watersCount);
		} else logs::info("TESDataHandler not found");
	}

	void ProcessMessage(SKSE::MessagingInterface::Message* a_msg) {
		if (a_msg) switch (a_msg->type) {
			case SKSE::MessagingInterface::kPostLoad:
				BSUConfig::ReadConfig();
				break;
			case SKSE::MessagingInterface::kDataLoaded:
				ProcessWaterForms();
				break;
		}
	}

}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse);
	auto& game = REL::Module::get();
	auto gameName = fs::path(game.filename()).replace_extension().string();
	auto gameVersion = game.version().string("-");
	logs::info("{} v{}", gameName, gameVersion);
	auto msg = SKSE::GetMessagingInterface();
	return msg && msg->RegisterListener(BSUProcess::ProcessMessage);
}
