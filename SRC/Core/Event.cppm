// A variant for abstracting event types. Categories are declared here.
module;
#include <memory>
#include <memory_resource>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
export module Core.Event;
import Core.Environment;
import Core.KeyInfo;
import Core.Object;
import Proxies.Object;

/*
The object event category.
Events such as changes to an object's focus, state, name, description, value, etc., always dispatch a CObjectEvent.
*/
export class CObjectEvent final {
public:
	EObjectEventType type;
	CObjectProxy object;
};

/*
The keyboard event category.
Key pressed and released events. always dispatch a CKeyboardEvent.
*/
export class CKeyboardEvent final {
public:
	enum eKeyboardEventType : unsigned char { NONE = 0, KEY_PRESSED, KEY_RELEASED } type;

	EKeycode keycode;
};

namespace std {
export template <> struct hash<SHotkeyInfo> {
	auto operator()(const SHotkeyInfo& k) const noexcept -> std::size_t { return std::hash<uint32_t>{}(k.Pack()); }
};
} // namespace std

using EventVariant = std::variant<std::monostate, CObjectEvent, CKeyboardEvent>;

export class CEvent final {
	EventVariant m_variant;

public:
	using allocator_type = std::pmr::polymorphic_allocator<>;

	enum EEventType : unsigned char { NONE = 0, OBJECT, KEYBOARD };

private:
	EEventType m_type{NONE};

public:
	~CEvent() = default;
	CEvent(CObjectEvent&& object_event, allocator_type alloc = {})
		: m_variant(std::move(object_event)), m_type(OBJECT) {}

	CEvent(CKeyboardEvent&& keyboard_event, allocator_type alloc = {})
		: m_variant(std::move(keyboard_event)), m_type(KEYBOARD) {}

	CEvent(CEvent&& other, allocator_type alloc) : m_variant(std::move(other.m_variant)), m_type(other.m_type) {}

	CEvent(CEvent&&) = default;
	auto operator=(CEvent&&) -> CEvent& = default;

	[[nodiscard]] auto GetType() const -> EEventType { return m_type; }

	template <typename T>
	auto GetAs(this auto&& self)
		-> std::optional<std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const T, T>> {
		if (auto* ptr = std::get_if<T>(&self.m_variant)) {
			return *ptr;
		}
		return std::nullopt;
	}

	template <typename Visitor> auto Visit(Visitor&& visitor) -> decltype(auto) {
		return std::visit(std::forward<Visitor>(visitor), m_variant);
	}

	template <typename Visitor> auto Visit(Visitor&& visitor) const -> decltype(auto) {
		return std::visit(std::forward<Visitor>(visitor), m_variant);
	}
};
