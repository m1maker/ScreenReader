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
#include <string_view>

template<typename T>
using ObjectResult = std::expected<T, unsigned char>;

class IObject : public std::enable_shared_from_this<IObject> {
public:

	enum EInterfaceMask : uint32_t {
		SUPPORTS_NOTHING = 0,
		SUPPORTS_TEXT = 1 << 0,
		SUPPORTS_SELECTION = 1 << 1,
		SUPPORTS_VALUE = 1 << 2
	};

	[[nodiscard]] virtual auto GetSupportedInterfaces() const noexcept -> uint32_t = 0;

	enum EObjectType : unsigned char {
		UNKNOWN = 0,
		ABBREVIATION,
		ALERT,
		ALERT_DIALOG,
		ANCHOR,
		ANIMATION,
		APPLICATION,
		ARTICLE,
		AUDIO,
		AUTO_COMPLETE,
		BANNER,
		BLOCKQUOTE,
		BREADCRUMB,
		BUTTON,
		CANVAS,
		CAPTION,
		CAROUSEL,
		CELL,
		CHECKBOX,
		CHART,
		CODE,
		COLOR_WELL,
		COLUMN,
		COLUMN_HEADER,
		COMBO_BOX,
		COMMENT,
		COMPLEMENTARY,
		CONTENT_DELETION,
		CONTENT_INFO,
		CONTENT_INSERTION,
		DATE_PICKER,
		DEFINITION,
		DESCRIPTION_LIST,
		DETAILS,
		DIALOG,
		DIRECTORY,
		DISCLOSURE_TRIANGLE,
		DOCUMENT,
		DRAWER,
		EMBEDDED_OBJECT,
		EMPHASIS,
		FEED,
		FIGURE,
		FIGURE_CAPTION,
		FILE_UPLOAD,
		FOOTER,
		FORM,
		GENERIC_CONTAINER,
		GRID,
		GRID_CELL,
		GROUP,
		HEADER,
		HEADING,
		IFRAME,
		IMAGE,
		IMAGE_MAP,
		INLINE_TEXT,
		INPUT_DEVICE,
		KEY,
		KEYBOARD,
		LABEL,
		LAYOUT_AREA,
		LEGEND,
		LINE,
		LINK,
		LIST,
		LIST_BOX,
		LIST_ITEM,
		LIST_MARKER,
		LOG,
		MAIN,
		MAP,
		MARK,
		MARQUEE,
		MATH,
		MATH_FRACTION,
		MATH_OPERATOR,
		MATH_ROOT,
		MATH_SUBSCRIPT,
		MATH_SUPERSCRIPT,
		MENU,
		MENU_BAR,
		MENU_ITEM,
		MENU_ITEM_CHECKBOX,
		MENU_ITEM_RADIO,
		METER,
		NAVIGATION,
		NOTE,
		PAGINATION,
		PANEL,
		PARAGRAPH,
		PASSWORD_FIELD,
		POPOVER,
		PREFORMATTED,
		PROGRESS_BAR,
		RADIO_BUTTON,
		RADIO_GROUP,
		RATING_INDICATOR,
		REGION,
		ROW,
		ROW_GROUP,
		ROW_HEADER,
		RUBY,
		SCROLL_BAR,
		SCROLL_VIEW,
		SEARCH,
		SEARCH_BOX,
		SECTION,
		SECTION_FOOTER,
		SECTION_HEADER,
		SEPARATOR,
		SHEET,
		SLIDER,
		SLIDER_THUMB,
		SPIN_BUTTON,
		SPLITTER,
		STATUS,
		STRONG,
		SUBSCRIPT,
		SUPERSCRIPT,
		SUPPLEMENTARY_CONTENT,
		SVG_ROOT,
		SWITCH,
		TAB,
		TAB_LIST,
		TAB_PANEL,
		TABLE,
		TERMINAL,
		TEXT_FIELD,
		TIME,
		TIMER,
		TITLE_BAR,
		TOGGLE_BUTTON,
		TOOLBAR,
		TOOLTIP,
		TREE,
		TREE_GRID,
		TREE_ITEM,
		VIDEO,
		VIEWPORT,
		WEB_VIEW,
		WINDOW,
	};

	enum EObjectState : unsigned long long {
		NO                = 0,

		ACTIVE              = 1ULL << 0,
		BUSY                = 1ULL << 1,
		CHECKED             = 1ULL << 2,
		COLLAPSED           = 1ULL << 3,
		DEFAULT             = 1ULL << 4,
		DRAGGING            = 1ULL << 5,
		ENABLED             = 1ULL << 6,
		EXPANDED            = 1ULL << 7,
		FOCUSED             = 1ULL << 8,
		HOVERED             = 1ULL << 9,
		INDETERMINATE       = 1ULL << 10, // For partial checkboxes
		INVALID             = 1ULL << 11,
		LINKED              = 1ULL << 12,
		LOADING             = 1ULL << 13,
		MODAL               = 1ULL << 14,
		OFFSCREEN           = 1ULL << 15,
		PRESSED             = 1ULL << 16,
		READONLY            = 1ULL << 17,
		REQUIRED            = 1ULL << 18,
		SELECTED            = 1ULL << 19,
		VISITED             = 1ULL << 20,
		VISIBLE             = 1ULL << 21,

		ANIMATED            = 1ULL << 22,
		AUTO_FILL_AVAILABLE = 1ULL << 23,
		CHECKABLE           = 1ULL << 24,
		CLICKABLE           = 1ULL << 25,
		DRAGGABLE           = 1ULL << 26,
		EDITABLE            = 1ULL << 27,
		EXPANDABLE          = 1ULL << 28,
		FOCUSABLE           = 1ULL << 29,
		HAS_POPUP           = 1ULL << 30,
		HORIZONTAL          = 1ULL << 31,
		MAPPABLE            = 1ULL << 32,
		MULTI_LINE          = 1ULL << 33,
		MULTI_SELECTABLE    = 1ULL << 34,
		MOVEABLE            = 1ULL << 35,
		PINNED              = 1ULL << 36,
		RESIZABLE           = 1ULL << 37,
		SELECTABLE          = 1ULL << 38,
		SECURE              = 1ULL << 39, // For passwords
		SORTABLE            = 1ULL << 40,
		TOUCH_OPTIMIZED     = 1ULL << 41,
		VERTICAL            = 1ULL << 42,

		CLIPPED             = 1ULL << 43,
		HAS_TOOLTIP         = 1ULL << 44,
		HIDDEN              = 1ULL << 45,
		LIVE_REGION         = 1ULL << 46, // For dynamic content updates
		PROTECTED           = 1ULL << 47,
		SENSITIVE           = 1ULL << 48,
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

protected:
	std::pmr::memory_resource* m_pool{nullptr};
public:

	explicit IObject(std::pmr::memory_resource* pool) : m_pool(pool) {}
	virtual ~IObject() = default;

	[[nodiscard]] static auto GetTypeName(const EObjectType& type, bool require_all = false) -> std::string_view;
	[[nodiscard]] static auto GetStateNames(const EObjectType& type, const unsigned long long& states, bool require_all = false) -> std::vector<std::string_view>;

	[[nodiscard]] static constexpr inline auto IsValidParent(const EObjectType& type) -> bool {
		switch (type) {
			case WINDOW:
			case DIALOG:
				return true;
		}

		return false;
	}

	template<typename Interface>
	[[nodiscard]] auto GetAs() -> std::shared_ptr<Interface>;

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

	virtual void UpdateCacheByEvent(const CEvent::EEventType& type) = 0;
};

class ITextProvider {
public:

	virtual ~ITextProvider() = default;

	[[nodiscard]] virtual auto GetCursor()const -> ObjectResult<int> = 0;
	[[nodiscard]] virtual auto GetText(int cursor, const ETextGranularity& granularity) const -> ObjectResult<STextRange<std::string>> = 0;
	[[nodiscard]] virtual auto GetSelectedRanges() const -> ObjectResult<std::vector<STextRange<void>>> = 0;
};

class ISelectionProvider {
public:

	virtual ~ISelectionProvider() = default;

	[[nodiscard]] virtual auto GetSelectedItems() const -> ObjectResult<std::vector<std::shared_ptr<IObject>>> = 0;
};

class IValueProvider {
public:

	virtual ~IValueProvider() = default;
	[[nodiscard]] virtual auto GetMinValue()const -> ObjectResult<double> = 0;
	[[nodiscard]] virtual auto GetMaxValue()const -> ObjectResult<double> = 0;
	[[nodiscard]] virtual auto GetCurrentValue()const -> ObjectResult<double> = 0;
};

template<typename T>
struct SInterfaceTrait final {
	static constexpr IObject::EInterfaceMask value = IObject::SUPPORTS_NOTHING;
};

template<> struct SInterfaceTrait<ITextProvider>      { static constexpr IObject::EInterfaceMask value = IObject::SUPPORTS_TEXT; };
template<> struct SInterfaceTrait<ISelectionProvider>      { static constexpr IObject::EInterfaceMask value = IObject::SUPPORTS_SELECTION; };
template<> struct SInterfaceTrait<IValueProvider>      { static constexpr IObject::EInterfaceMask value = IObject::SUPPORTS_VALUE; };

template<typename Interface>
[[nodiscard]] auto IObject::GetAs() -> std::shared_ptr<Interface> {
	constexpr auto required = SInterfaceTrait<Interface>::value;
	if constexpr (required == SUPPORTS_NOTHING) return nullptr;

	uint32_t mask = GetSupportedInterfaces();
	return mask & required ? std::dynamic_pointer_cast<Interface>(shared_from_this()) : nullptr;
}

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

		auto new_object = std::allocate_shared<U>(std::pmr::polymorphic_allocator<U>(&m_pool), native_handle, &m_pool);

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

