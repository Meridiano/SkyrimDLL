#pragma once

namespace Translation {

	std::uint32_t ReadLineW(RE::BSResourceNiBinaryStream* a_this, wchar_t* a_dst, std::uint32_t a_dstLen, std::uint32_t a_terminator) {
		wchar_t* iter = a_dst;
		if (a_dstLen == 0) return 0;
		for (std::uint32_t i = 0; i < a_dstLen - 1; i++) {
			wchar_t data;
			std::uint64_t read;
			a_this->stream->DoRead(&data, sizeof(data), read);
			if (read != sizeof(data)) break;
			if (data == a_terminator) break;
			*iter++ = data;
		}
		// null terminate
		*iter = 0;
		return static_cast<std::uint32_t>(iter - a_dst);
	}

	void GetCachedString(wchar_t** a_pOut, wchar_t* a_bufIn, std::uint32_t a_unused) {
		bool post1130 = REL::Module::IsAE() && (REL::Module::get().version().patch() > 0x400);
		using func_t = decltype(&GetCachedString);
		REL::Relocation<func_t> func{
			REL::RelocationID(67844, post1130 ? 443410 : 69188, 67844)
		};
		return func(a_pOut, a_bufIn, a_unused);
	};

	void ParseTranslation(const std::string& a_name) {
		const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
		const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
		const auto translator = loader ? loader->GetStateAddRef<RE::GFxTranslator>(RE::GFxState::StateType::kTranslator) : nullptr;
		const auto scaleformTranslator = skyrim_cast<RE::BSScaleformTranslator*>(translator);
		if (!scaleformTranslator) {
			logger::info("Failed to import translation for {}", a_name);
			return;
		}
		const auto iniSettingCollection = RE::INISettingCollection::GetSingleton();
		auto setting = iniSettingCollection ? iniSettingCollection->GetSetting("sLanguage:General") : nullptr;
		// construct translation filename
		std::string language = (setting && setting->GetType() == RE::Setting::Type::kString) ? setting->data.s : "ENGLISH";
		std::string path = std::format("Interface\\Translations\\{}_{}.txt", a_name, language);
		RE::BSResourceNiBinaryStream fileStream{path};
		if (!fileStream.good()) {
			return;
		} else {
			logger::info("Reading translations from {}", path);
		}
		// check if file is empty, check if the BOM is UTF-16
		std::uint16_t bom = 0;
		std::uint64_t read;
		fileStream.stream->DoRead(&bom, sizeof(std::uint16_t), read);
		if (read == 0) {
			logger::info("Empty translation file");
			return;
		}
		if (bom != 0xFEFF) {
			logger::info("BOM error, file must be encoded in UTF-16 LE BOM");
			return;
		}
		while (true) {
			wchar_t buf[512];
			std::uint32_t len = ReadLineW(&fileStream, buf, sizeof(buf) / sizeof(buf[0]), '\n');
			if (len == 0) return; // end of file
			if (len < 4 || buf[0] != '$') continue; // at least $ + wchar_t + \t + wchar_t
			wchar_t last = buf[len - 1];
			if (last == '\r') len--;
			// null terminate
			buf[len] = 0;
			std::uint32_t delimIdx = 0;
			for (std::uint32_t i = 0; i < len; i++) if (buf[i] == '\t') delimIdx = i;
			if (delimIdx < 2) continue; // at least $ + wchar_t
			buf[delimIdx] = 0; // replace \t by \0
			wchar_t* key = nullptr;
			wchar_t* translation = nullptr;
			GetCachedString(&key, buf, 0);
			GetCachedString(&translation, &buf[delimIdx + 1], 0);
			scaleformTranslator->translator.translationMap.emplace(key, translation);
		}
	}

}
