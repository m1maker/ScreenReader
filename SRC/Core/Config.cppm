/*
* This file is part of the Screen Reader project.
*
* Copyright (C) 2025-2026  Georgiy Bondarenko (M_maker / m1maker) <georgijbondarenko248@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

module;

#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#define ReflectField(x)                                                                                                \
	std::pair {                                                                                                        \
		#x, std::ref(x)                                                                                                \
	}

export module Core.Config;
import Core.Speech;
import Core.Utterance;

export struct SScreenReaderAppSettings final {
	struct {
		bool read_item_count = true;
		bool read_unfocused_object_changes = true;
	} object_presentation;
	struct SSpeechParameters final {
		unsigned char rate = cSpeechEngineNormalValue, volume = cSpeechEngineMaxValue, pitch = cSpeechEngineNormalValue,
					  pitch_range = cSpeechEngineNormalValue;
		bool ssml = true;
		struct SUtteranceParameters final {
			std::string_view rate = CUtterance::cRateDefault, pitch = CUtterance::cPitchDefault,
							 volume = CUtterance::cVolumeDefault, pause_before = "", pause_after = "";
		} name, description = {.pause_before = "150ms"},
				role = {.pitch = CUtterance::cPitchXlow, .pause_before = "5ms"},
				state = {.rate = CUtterance::cRateXfast, .pitch = CUtterance::cPitchXlow, .pause_before = "15ms"},
				text = {.pitch = CUtterance::cPitchXhigh, .pause_before = "60ms"};

	} speech;
};

export using SpeechParameters = SScreenReaderAppSettings::SSpeechParameters;
export using UtteranceParameters = SpeechParameters::SUtteranceParameters;

constexpr inline std::string_view cConfigFileName = "ScreenReader";

using ConfigValueVariant = std::variant<int, double, std::string, bool>;

export class IConfig {
public:
	virtual ~IConfig() = default;

	virtual void OpenSection(std::string_view section) = 0;
	virtual void CloseSection() = 0;
	[[nodiscard]] virtual auto GetSectionsOpened() const -> std::vector<std::string_view> = 0;
	virtual void CloseAllSections() = 0;

	virtual void SetValue(std::string_view key, const ConfigValueVariant& value) = 0;
	[[nodiscard]] virtual auto GetValue(std::string_view key) const -> ConfigValueVariant = 0;

	virtual void Save() = 0;
	virtual void Load() = 0;
};

export template <typename T>
concept Reflectable = requires(T t) {
	{ t.Reflect() };
};

export class CConfigSerializer final {
	IConfig& m_backend;

public:
	explicit CConfigSerializer(IConfig& backend) : m_backend(backend) {}

	template <Reflectable T> void Write(const T& obj) {
		auto fields = obj.Reflect();

		std::apply([this](auto&&... args) -> auto { ((this->WriteField(args.first, args.second)), ...); }, fields);

		m_backend.Save();
	}

	template <Reflectable T> void Read(T& obj) {
		m_backend.Load();

		auto fields = obj.Reflect();

		std::apply([this](auto&&... args) -> auto { ((this->ReadField(args.first, args.second)), ...); }, fields);
	}

private:
	template <typename V> void WriteField(const char* name, const V& value) { m_backend.SetValue(name, value); }

	template <typename Wrapper> void ReadField(const char* name, Wrapper field_wrapper) {
		using FieldType = typename Wrapper::type;

		ConfigValueVariant val = m_backend.GetValue(name);

		if (val.valueless_by_exception())
			return;

		if (std::holds_alternative<FieldType>(val)) {
			field_wrapper.get() = std::get<FieldType>(val);
		}
		else {
		}
	}
};
