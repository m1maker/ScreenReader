#pragma once
#include "Logger.h"

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

struct SScreenReaderAppSettings final {
	bool read_list_item_count = true;

	[[nodiscard]] auto Reflect() { return std::make_tuple(ReflectField(read_list_item_count)); }
};

constexpr inline std::string_view cConfigFileName = "ScreenReader";

using ConfigValueVariant = std::variant<int, double, std::string, bool>;

class IConfig {
public:
	virtual ~IConfig() = default;

	virtual void OpenSection(std::string_view section) = 0;
	virtual void CloseSection() = 0;
	[[nodiscard]] virtual auto GetSectionsOpened() const -> std::vector<std::string_view>;
	virtual void CloseAllSections() = 0;

	virtual void SetValue(std::string_view key, const ConfigValueVariant& value) = 0;
	[[nodiscard]] virtual auto GetValue(std::string_view key) const -> ConfigValueVariant = 0;

	virtual void Save() = 0;
	virtual void Load() = 0;
};

template <typename T>
concept Reflectable = requires(T t) {
	{ t.Reflect() };
};

class CConfigSerializer final {
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
			g_logger.Log(CLogger::WARNING, "Config", "Type mismatch for key: " + std::string(name));
		}
	}
};
