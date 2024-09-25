#pragma once

namespace NLSUtility {

	std::vector<std::string> StringSplit(std::string arg, char del) {
		std::vector<std::string> out;
		std::istringstream iss(arg);
		std::string str;
		while (std::getline(iss, str, del)) out.push_back(str);
		return out;
	}

	std::string StringToLower(std::string in) {
		auto out = in;
		std::transform(
			in.begin(), in.end(), out.begin(),
			[](unsigned char uch) { return std::tolower(uch); }
		);
		return out;
	};

	std::string SourceFilenameLower(RE::TESForm* form, RE::TESDataHandler* data) {
		if (form && data) {
			std::uint8_t space = (form->formID >> 24);
			if (space == 0xFE) {
				std::uint16_t light = (form->formID >> 12) & 0xFFF;
				auto file = data->LookupLoadedLightModByIndex(light);
				if (file) {
					auto name = std::string(file->fileName);
					return StringToLower(name);
				}
			} else if (space != 0xFF) {
				auto file = data->LookupLoadedModByIndex(space);
				if (file) {
					auto name = std::string(file->fileName);
					return StringToLower(name);
				}
			}
		}
		return "";
	}

	RE::TESForm* RecordFromText(RE::TESDataHandler* data, std::string text) {
		std::vector<std::string> args = StringSplit(text, ':');
		try {
			if (args.size() == 2) {
				auto plugin = args[0];
				auto id = std::stoul(args[1], nullptr, 0);
				auto form = data->LookupForm(id, plugin);
				if (form) return form;
			}
			EXCEPTION;
		} catch (...) {
			logs::info("Failed to lookup \"{}\" form", text);
		}
		return nullptr;
	}

}