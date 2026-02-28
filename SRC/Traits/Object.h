// Object trait.
#pragma once
#include <Core/Cache.h>
#include <Core/EnumUtils.h>
#include <Core/Rect.h>
#include <Core/StaticInterface.h>
#include <Core/Text.h>
#include <Traits/RefCountedObject.h>
#include <expected>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class EObjectInterfaceMask : uint32_t {
	SUPPORTS_NOTHING = 0,
	SUPPORTS_TEXT = 1 << 0,
	SUPPORTS_SELECTION = 1 << 1,
	SUPPORTS_VALUE = 1 << 2
};
EnableBitwiseEnum(EObjectInterfaceMask)

	enum class EObjectType : unsigned char {
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

enum class EObjectState : unsigned long long {
	NO = 0,

	ACTIVE = 1ULL << 0,
	BUSY = 1ULL << 1,
	CHECKED = 1ULL << 2,
	COLLAPSED = 1ULL << 3,
	DEFAULT = 1ULL << 4,
	DRAGGING = 1ULL << 5,
	ENABLED = 1ULL << 6,
	EXPANDED = 1ULL << 7,
	FOCUSED = 1ULL << 8,
	HOVERED = 1ULL << 9,
	INDETERMINATE = 1ULL << 10, // For partial checkboxes
	INVALID = 1ULL << 11,
	LINKED = 1ULL << 12,
	LOADING = 1ULL << 13,
	MODAL = 1ULL << 14,
	OFFSCREEN = 1ULL << 15,
	PRESSED = 1ULL << 16,
	READONLY = 1ULL << 17,
	REQUIRED = 1ULL << 18,
	SELECTED = 1ULL << 19,
	VISITED = 1ULL << 20,
	VISIBLE = 1ULL << 21,

	ANIMATED = 1ULL << 22,
	AUTO_FILL_AVAILABLE = 1ULL << 23,
	CHECKABLE = 1ULL << 24,
	CLICKABLE = 1ULL << 25,
	DRAGGABLE = 1ULL << 26,
	EDITABLE = 1ULL << 27,
	EXPANDABLE = 1ULL << 28,
	FOCUSABLE = 1ULL << 29,
	HAS_POPUP = 1ULL << 30,
	HORIZONTAL = 1ULL << 31,
	MAPPABLE = 1ULL << 32,
	MULTI_LINE = 1ULL << 33,
	MULTI_SELECTABLE = 1ULL << 34,
	MOVEABLE = 1ULL << 35,
	PINNED = 1ULL << 36,
	RESIZABLE = 1ULL << 37,
	SELECTABLE = 1ULL << 38,
	SECURE = 1ULL << 39, // For passwords
	SORTABLE = 1ULL << 40,
	TOUCH_OPTIMIZED = 1ULL << 41,
	VERTICAL = 1ULL << 42,

	CLIPPED = 1ULL << 43,
	HAS_TOOLTIP = 1ULL << 44,
	HIDDEN = 1ULL << 45,
	LIVE_REGION = 1ULL << 46, // For dynamic content updates
	PROTECTED = 1ULL << 47,
	SENSITIVE = 1ULL << 48,
};
EnableBitwiseEnum(EObjectState)

	enum class EObjectError : unsigned char {
		SUCCESS = 0,
		DEFUNCT,
		NOT_SUPPORTED,
		ACCESS_DENIED,
		INVALID_ARGUMENTS,
		TIMEOUT,
		FAIL
	};

template <typename T> using ObjectResult = std::expected<T, EObjectError>;

enum class EObjectEventType : unsigned char {
	NONE = 0,
	FOCUS_GAINED,
	FOCUS_LOST,
	CLICKED,
	VALUE_CHANGED,
	SELECTION_CHANGED,
	STATE_CHANGED,
	VISIBILITY_CHANGED,
	ENABLED_CHANGED,
	TEXT_CHANGED,
	CURSOR_MOVED,
	CHILD_ADDED,
	CHILD_REMOVED,
	PARENT_UPDATED,
	LAYOUT_UPDATED
};

[[nodiscard]] static constexpr auto ObjectErrorToString(EObjectError error) -> std::string_view {
	using enum EObjectError;
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
		return "Invalid Arguments: The parameters provided to the method are out of range or malformed for this "
			   "specific object.";
	case TIMEOUT:
		return "Operation Timeout: The application or the accessibility registry failed to respond within the "
			   "expected timeframe.";
	case FAIL:
	default:
		return "Unknown Error: An unexpected or undocumented failure occurred during the interaction with the "
			   "accessibility API.";
	}
}

[[nodiscard]] auto GetObjectTypeName(EObjectType type, bool require_all = false) -> std::string_view;
[[nodiscard]] auto GetObjectStateNames(EObjectType type, unsigned long long states, bool require_all = false)
	-> std::vector<std::string_view>;

[[nodiscard]] static constexpr inline auto IsValidObjectParent(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::WINDOW:
	case EObjectType::DIALOG:
		return true;
	default:
		return false;
	}

	return false;
}

template <typename Derived> class TTextProvider {
BindStaticInterface(Derived) public : [[nodiscard]] auto GetCursor() const { return Impl().do_GetCursor(); }
	[[nodiscard]] auto GetText(int cursor, const ETextGranularity& granularity) const {
		return Impl().do_GetText(cursor, granularity);
	}
	[[nodiscard]] auto GetSelectedRanges() const { return Impl().do_GetSelectedRanges(); }
};

template <typename Derived> class TSelectionProvider {
BindStaticInterface(Derived) public : [[nodiscard]] auto GetSelectedItems() const {
		return Impl().do_GetSelectedItems();
	}
};

template <typename Derived> class TValueProvider {
BindStaticInterface(Derived) public : [[nodiscard]] auto GetMinValue() const { return Impl().do_GetMinValue(); }
	[[nodiscard]] auto GetMaxValue() const { return Impl().do_GetMaxValue(); }
	[[nodiscard]] auto GetCurrentValue() const { return Impl().do_GetCurrentValue(); }
};

template <typename Derived> class TObject {
	// static_assert(std::is_trivially_copyable_v<Derived>, "Object handles must be trivial!");
BindStaticInterface(Derived)

	protected : std::pmr::memory_resource* m_pool {
		nullptr
	};

public:
	explicit TObject(std::pmr::memory_resource* pool) : m_pool(pool) {}

	[[nodiscard]] auto GetSupportedInterfaces() const noexcept -> uint32_t {
		return Impl().do_GetSupportedInterfaces();
	}
	[[nodiscard]] auto GetNativeHandle() const noexcept -> ObjectResult<void*> { return Impl().do_GetNativeHandle(); }
	[[nodiscard]] auto IsValid() const noexcept -> bool { return Impl().do_IsValid(); }

	[[nodiscard]] auto GetType() const { return Impl().do_GetType(); }
	[[nodiscard]] auto IsVisible() const { return Impl().do_IsVisible(); }
	[[nodiscard]] auto IsEnabled() const { return Impl().do_IsEnabled(); }

	[[nodiscard]] auto GetState() const { return Impl().do_GetState(); }
	[[nodiscard]] auto HasState(EObjectState state) const { return Impl().do_HasState(state); }

	[[nodiscard]] auto GetParent() const -> ObjectResult<Derived> { return Impl().do_GetParent(); }
	[[nodiscard]] auto GetChildren() const -> ObjectResult<std::vector<Derived>> { return Impl().do_GetChildren(); }
	[[nodiscard]] auto GetChildrenCount() const { return Impl().do_GetChildrenCount(); }

	[[nodiscard]] auto GetBounds() const { return Impl().do_GetBounds(); }
	[[nodiscard]] auto GetIndex() const { return Impl().do_GetIndex(); }

	[[nodiscard]] auto GetApplicationName() const { return Impl().do_GetApplicationName(); }
	[[nodiscard]] auto GetName() const { return Impl().do_GetName(); }
	[[nodiscard]] auto GetDescription() const { return Impl().do_GetDescription(); }

	void UpdateCacheByEvent(EObjectEventType type) { return Impl().do_UpdateCacheByEvent(type); }
};

template <class NativeHandle, typename ObjectData> class CObjectCache final {
	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle*, ObjectData*> m_cache;

public:
	explicit CObjectCache() : m_cache(&m_pool) {}

	template <typename PlatformObject> [[nodiscard]] auto GetOrCreate(NativeHandle* native_handle) -> PlatformObject {
		if (!native_handle)
			return PlatformObject();

		auto it = m_cache.find(native_handle);
		if (it != m_cache.end()) {
			if (auto existing = it->second) {
				LifecycleTrait<NativeHandle>::Release(native_handle);
				return PlatformObject(native_handle, existing, &m_pool);
			}
			Remove(native_handle);
		}

		auto raw = m_pool.allocate(sizeof(ObjectData));
		auto object_data = new (raw) ObjectData();

		auto new_object = PlatformObject(native_handle, object_data, &m_pool);
		m_cache[native_handle] = object_data;

		return new_object;
	}

	void Remove(NativeHandle* native_handle) {
		auto it = m_cache.find(native_handle);
		if (it == m_cache.end()) [[unlikely]]
			return;

		LifecycleTrait<NativeHandle>::Release(native_handle);
		it->second->~ObjectData();
		m_pool.deallocate(it->second, sizeof(ObjectData));
		m_cache.erase(it);
	}

	void Clear() {
		for (auto [handle, data] : m_cache) {
			Remove(handle);
		}

		m_cache.clear();
	}
};

#define g_objectCache(T, U) CSingleton<CObjectCache<T, U>>::GetInstance()
