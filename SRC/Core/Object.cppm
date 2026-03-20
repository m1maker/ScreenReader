module;
#include <Core/EnumUtils.h>
#include <expected>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>
export module Core.Object;
import Core.Rect;
import Core.Text;
import Traits.RefCountedObject;

export enum class EObjectInterfaceMask : uint32_t {
	SUPPORTS_NOTHING = 0,
	SUPPORTS_TEXT = 1 << 0,
	SUPPORTS_SELECTION = 1 << 1,
	SUPPORTS_VALUE = 1 << 2
};
EnableBitwiseEnum(EObjectInterfaceMask);

export enum class EObjectType : unsigned char {
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

export enum class EObjectState : unsigned long long {
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
EnableBitwiseEnum(EObjectState);

export enum class EObjectError : unsigned char {
	SUCCESS = 0,
	DEFUNCT,
	NOT_SUPPORTED,
	ACCESS_DENIED,
	INVALID_ARGUMENTS,
	TIMEOUT,
	FAIL
};

export template <typename T> using ObjectResult = std::expected<T, EObjectError>;

export enum class EObjectEventType : unsigned char {
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

export [[nodiscard]] constexpr auto ObjectErrorToString(EObjectError error) -> std::string_view {
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

export [[nodiscard]] auto GetObjectTypeName(EObjectType type, bool require_all = false) -> std::string_view;
export [[nodiscard]] auto GetObjectStateNames(EObjectType type, unsigned long long states, bool require_all = false)
	-> std::vector<std::string_view>;

export [[nodiscard]] constexpr inline auto IsObjectParent(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::WINDOW:
	case EObjectType::DIALOG:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectContainer(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::GENERIC_CONTAINER:
	case EObjectType::GROUP:
	case EObjectType::SECTION:
	case EObjectType::PANEL:
	case EObjectType::DRAWER:
	case EObjectType::SHEET:
	case EObjectType::ARTICLE:
	case EObjectType::REGION:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectDataView(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::TABLE:
	case EObjectType::GRID:
	case EObjectType::TREE_GRID:
	case EObjectType::LIST_BOX:
	case EObjectType::DIRECTORY:
	case EObjectType::FEED:
	case EObjectType::TREE:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectDataElement(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::CELL:
	case EObjectType::GRID_CELL:
	case EObjectType::ROW:
	case EObjectType::COLUMN:
	case EObjectType::ROW_GROUP:
	case EObjectType::LIST_ITEM:
	case EObjectType::TREE_ITEM:
	case EObjectType::COLUMN_HEADER:
	case EObjectType::ROW_HEADER:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectMath(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::MATH:
	case EObjectType::MATH_FRACTION:
	case EObjectType::MATH_OPERATOR:
	case EObjectType::MATH_ROOT:
	case EObjectType::MATH_SUBSCRIPT:
	case EObjectType::MATH_SUPERSCRIPT:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectNavigation(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::NAVIGATION:
	case EObjectType::BREADCRUMB:
	case EObjectType::MENU_BAR:
	case EObjectType::TAB_LIST:
	case EObjectType::PAGINATION:
	case EObjectType::ANCHOR:
	case EObjectType::LINK:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectMedia(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::IMAGE:
	case EObjectType::VIDEO:
	case EObjectType::AUDIO:
	case EObjectType::CANVAS:
	case EObjectType::ANIMATION:
	case EObjectType::SVG_ROOT:
	case EObjectType::FIGURE:
	case EObjectType::CHART:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectWeb(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::WEB_VIEW:
	case EObjectType::IFRAME:
	case EObjectType::DOCUMENT:
	case EObjectType::IMAGE_MAP:
	case EObjectType::MAP:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectInput(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::BUTTON:
	case EObjectType::CHECKBOX:
	case EObjectType::COLOR_WELL:
	case EObjectType::COMBO_BOX:
	case EObjectType::DATE_PICKER:
	case EObjectType::FILE_UPLOAD:
	case EObjectType::RADIO_BUTTON:
	case EObjectType::SEARCH_BOX:
	case EObjectType::SLIDER:
	case EObjectType::SPIN_BUTTON:
	case EObjectType::SWITCH:
	case EObjectType::TEXT_FIELD:
	case EObjectType::TOGGLE_BUTTON:
	case EObjectType::PASSWORD_FIELD:
		return true;
	default:
		return false;
	}
}

export [[nodiscard]] constexpr inline auto IsObjectFeedback(EObjectType type) -> bool {
	switch (type) {
	case EObjectType::ALERT:
	case EObjectType::ALERT_DIALOG:
	case EObjectType::METER:
	case EObjectType::PROGRESS_BAR:
	case EObjectType::STATUS:
	case EObjectType::TIMER:
	case EObjectType::TOOLTIP:
	case EObjectType::LOG:
		return true;
	default:
		return false;
	}
}

export template <class NativeHandle, typename ObjectData> class CObjectCache final {
	std::pmr::unsynchronized_pool_resource m_pool;
	std::pmr::unordered_map<NativeHandle*, ObjectData*> m_cache;
	explicit CObjectCache() : m_cache(&m_pool) {}

public:
	// template<class NativeHandle, typename ObjectData>
	static auto& GetInstance() {
		static CObjectCache<NativeHandle, ObjectData> instance;
		return instance;
	}

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
