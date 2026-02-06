// Object interface.
#pragma once
#include <vector>
#include <memory>
#include <Core/Rect.h>
#include <Core/Cache.h>
#include <Core/Event.h>
#include <Core/Text.h>
#include <map>
#include <expected>
#include <memory_resource>

template<typename T>
using ObjectResult = std::expected<T, unsigned char>;

class IObject {
public:
	enum EObjectType : unsigned char {
		UNKNOWN = 0,
		BUTTON,
		TOGGLE_BUTTON,
		TEXT_FIELD,
		LABEL,
		CHECKBOX,
		RADIO_BUTTON,
		COMBO_BOX,
		LIST_BOX,
		LIST_ITEM,
		MENU,
		MENU_ITEM,
		SLIDER,
		PROGRESS_BAR,
		IMAGE,
		PANEL,
		WINDOW,
		DIALOG
	};

	enum EObjectState : unsigned long long {
		NO = 0,
		VISIBLE = 1 << 0,
		ENABLED = 1 << 1,
		FOCUSED = 1 << 2,
		SELECTED = 1 << 3,
		CHECKED = 1 << 4,
		EXPANDED = 1 << 5,
		READONLY = 1 << 6,
		MULTI_LINE = 1 << 7,
		SECURE = 1 << 8,
		REQUIRED = 1 << 9,
		INVALID = 1 << 10,
		HOVERED = 1 << 11,
		PRESSED = 1 << 12,
		DEFAULT = 1 << 13,
		LOADING = 1 << 14,
		COLLAPSED = 1 << 15,

		EDITABLE = 1 << 16,
		EXPANDABLE = 1 << 17,
		FOCUSABLE = 1 << 18,
		SELECTABLE = 1 << 19,
		MULTI_SELECTABLE = 1 << 20,
		RESIZABLE = 1 << 21,
		CHECKABLE = 1 << 22
	};

	enum EObjectError : unsigned char {
		SUCCESS = 0,
		DEFUNCT,
		NOT_SUPPORTED,
		ACCESS_DENIED,
		INVALID_ARGUMENTS,
		TIMEOUT,
		FAIL
	};

	[[nodiscard]] static constexpr auto ErrorToString(const unsigned char& error) -> std::string_view {
		switch (error) {
			case SUCCESS:
				return "No error: The operation completed successfully.";
			case DEFUNCT:
				return "Object Defunct: The target accessibility object is no longer valid.";
			case NOT_SUPPORTED:
				return "Interface Not Supported: The object does not implement the requested accessibility interface.";
			case ACCESS_DENIED:
				return "Access Denied: Permission was refused to access this object.";
			case INVALID_ARGUMENTS:
				return "Invalid Arguments: The parameters provided to the method are out of range or malformed for this specific object.";
			case TIMEOUT:
				return "Operation Timeout: The application or the accessibility registry failed to respond within the expected timeframe.";
			case FAIL:
			default:
				return "Unknown Error: An unexpected or undocumented failure occurred during the interaction with the accessibility API.";
		}
	}

	IObject() = default;
	virtual ~IObject() = default;

	[[nodiscard]] static auto GetTypeName(const EObjectType& type, bool require_all = false) -> std::string;
	[[nodiscard]] static auto GetStateNames(const EObjectType& type, const unsigned long long& states, bool require_all = false) -> std::vector<std::string>;

	[[nodiscard]] static constexpr inline auto IsValidParent(const EObjectType& type) -> bool {
		switch (type) {
			case WINDOW:
			case DIALOG:
				return true;
		}

		return false;
	}

	[[nodiscard]] virtual auto GetNativeHandle()const noexcept -> ObjectResult<void*> = 0;

	[[nodiscard]] virtual auto IsValid()const noexcept -> bool = 0;

	[[nodiscard]] virtual auto GetType()const -> ObjectResult<EObjectType> = 0;
	[[nodiscard]] virtual auto IsVisible()const -> ObjectResult<bool> = 0;
	[[nodiscard]] virtual auto IsEnabled()const -> ObjectResult<bool> = 0;

	[[nodiscard]] virtual auto GetState()const -> ObjectResult<unsigned long long> = 0;
	[[nodiscard]] virtual auto HasState(EObjectState state)const -> ObjectResult<bool> = 0;

	[[nodiscard]] virtual auto GetParent()const -> ObjectResult<std::weak_ptr<IObject>> = 0;
	[[nodiscard]] virtual auto GetChildren()const -> ObjectResult<const std::vector<std::shared_ptr<IObject>>> = 0;
	[[nodiscard]] virtual auto GetChildrenCount()const -> ObjectResult<int> = 0;

	[[nodiscard]] virtual auto GetBounds()const -> ObjectResult<struct SRect> = 0;

	[[nodiscard]] virtual auto GetIndex()const -> ObjectResult<int> = 0;

	[[nodiscard]] virtual auto GetApplicationName()const -> ObjectResult<std::string> = 0;

	[[nodiscard]] virtual auto GetName()const -> ObjectResult<std::string> = 0;
	[[nodiscard]] virtual auto GetDescription()const -> ObjectResult<std::string> = 0;

	[[nodiscard]] virtual auto GetCursor()const -> ObjectResult<int> = 0;
	[[nodiscard]] virtual auto GetText(int cursor, const ETextGranularity& granularity)const -> ObjectResult<STextRange> = 0;

	[[nodiscard]] virtual auto GetMinValue()const -> ObjectResult<double> = 0;
	[[nodiscard]] virtual auto GetMaxValue()const -> ObjectResult<double> = 0;
	[[nodiscard]] virtual auto GetCurrentValue()const -> ObjectResult<double> = 0;

	void UpdateCacheByEvent(const CEvent::EEventType& event);

protected:
	DeclareCache(EObjectType, m_type);
	DeclareCache(unsigned long long, m_states);
	DeclareCache(std::weak_ptr<IObject>, m_parent);
	DeclareCache(std::vector<std::shared_ptr<IObject>>, m_children);
	DeclareCache(int, m_childrenCount);
	DeclareCache(int, m_index);
	DeclareCache(std::string, m_name);
	DeclareCache(std::string, m_applicationName);
	DeclareCache(std::string, m_description);
	DeclareCache(int, m_cursor);
	DeclareCache(double, m_minValue);
	DeclareCache(double, m_maxValue);
	DeclareCache(double, m_currentValue);
};

template<class T, class U>
class CObjectCache final {
	std::pmr::synchronized_pool_resource m_pool;
	std::mutex m_mutex;
	std::pmr::map<T*, std::weak_ptr<U>> m_cache;
public:

	explicit CObjectCache() : m_cache(&m_pool) {}

	[[nodiscard]] auto GetOrCreate(T* native_handle) -> std::shared_ptr<U> {
		if (!native_handle) return nullptr;

		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			if (auto existing = it->second.lock()) {
				//g_object_unref(accessible); 
				return existing;
			}
			else {
				m_cache.erase(it);
			}
		}

		auto new_object = std::allocate_shared<U>(std::pmr::polymorphic_allocator<U>(&m_pool), native_handle);

		m_cache[native_handle] = new_object;

		return new_object;
	}

	void Remove(T* native_handle) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cache.erase(native_handle);
	}

	void Clear() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cache.clear();
	}
};

#define g_objectCache(T, U) CSingleton<CObjectCache<T, U>>::GetInstance()

/*
Compare two objects.
*/
[[nodiscard]] inline auto ObjectIsSame(const std::shared_ptr<IObject>& obj1, const std::shared_ptr<IObject>& obj2) -> bool {
	if (!obj1 || !obj2) {
		return !obj1 && !obj2;
	}

	if (obj1.get() == obj2.get()) {
		return true;
	}

	if (obj1->GetType() != obj2->GetType()) {
		return false;
	}

	if (obj1->GetApplicationName() != obj2->GetApplicationName()) {
		return false;
	}

	if (obj1->GetName() != obj2->GetName()) {
		return false;
	}

	if (obj1->GetDescription() != obj2->GetDescription()) {
		return false;
	}

	return true;
}

/*
Attention! GetDesktopObject is not implemented in
 Object.cpp.
This should be done by the platform implementation of IObject.
*/
[[nodiscard]] auto GetDesktopObject() -> std::shared_ptr<IObject>; // Root object.
[[nodiscard]] auto FindFocusedObject(std::shared_ptr<IObject> start_from, bool force = false) -> std::shared_ptr<IObject>;

[[nodiscard]] auto DumpObjectToString(const std::shared_ptr<IObject>& obj, int indent = 0, bool recursive = false, int max_depth = 3, int current_depth = 0) -> std::string;

