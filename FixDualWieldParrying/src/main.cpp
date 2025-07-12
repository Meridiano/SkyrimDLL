std::string CalculateHash(fs::path path) {
	if (fs::exists(path) && fs::is_regular_file(path)) {
		std::ifstream stream(path, std::ios::in + std::ios::binary);
		std::vector<std::uint8_t> hash(picosha2::k_digest_size);
		picosha2::hash256(stream, hash.begin(), hash.end());
		return picosha2::bytes_to_hex_string(hash);
	}
	return "";
}

template <typename T>
void WriteValue(REL::Relocation<std::uintptr_t> reloc, T value) {
	auto target = reloc.address();
	auto source = std::addressof(value);
	REL::safe_write(target, source, sizeof(T));
}

void MessageListener(SKSE::MessagingInterface::Message* msg) {
	if (msg->type == SKSE::MessagingInterface::kPostLoad) {
		bool ngPatched = false;
		bool ogPatched = false;
		// case 1 - ng version
		auto ngModule = FindModule(DualWieldParryingNG);
		if (ngModule) {
			auto sha256 = CalculateHash("Data/SKSE/Plugins/DualWieldParryingNG.dll");
			bool match = HashEqual(sha256, "4a401832814a043009f9ca7cc724da0531674a10556c3f4aa98292df19dc6ed2");
			if (match) {
				WriteValue<std::uint8_t>(REL::Relocation{ ngModule + 0x4F343 }, 0xF);
				ngPatched = true;
			} else logs::info("NG version found but no match");
		} else logs::info("NG version not found");
		// case 2 - og version
		auto ogModule = FindModule(DualWieldParryingSKSE);
		if (ogModule) {
			auto sha256 = CalculateHash("Data/SKSE/Plugins/DualWieldParryingSKSE.dll");
			bool match = HashEqual(sha256, "584f6f32d5a8bc941220486c5d64653cd1a193d2903b04e9ba6e40506289964a");
			if (match) {
				WriteValue<std::uint32_t>(REL::Relocation{ ogModule + 0x5AB2 }, 0x100);
				ogPatched = true;
			} else logs::info("OG version found but no match");
		} else logs::info("OG version not found");
		// done
		logs::info("NG version {} / OG version {}", ngPatched ? "patched" : "skipped", ogPatched ? "patched" : "skipped");
	}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
	SKSE::Init(a_skse);
	// info
	auto gameModule = &REL::Module::get();
	auto gameVersion = gameModule ? gameModule->version().string("-") : "Unknown";
	logs::info("Skyrim.x64 version {}", gameVersion);
	// register
	auto skseMessaging = SKSE::GetMessagingInterface();
	return skseMessaging ? skseMessaging->RegisterListener(MessageListener) : false;
}
