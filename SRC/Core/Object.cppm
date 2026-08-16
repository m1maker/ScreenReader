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
#include <atomic>
#include <cstring>
#include <bitset>
#include <expected>
#include <memory_resource>
#include <string>
#include <span>
#include <string_view>
#include <utility>
#include <vector>
export module Core.Object;
import Core.KeyInfo;
import Core.Rect;
import Core.Text;
import Core.ThreadSafeQueue;

export enum class EObjectProvider : unsigned char {
	UNKNOWN = 0,
	MAIN,
	TEXT,
	SELECTION,
	ACTION,
	VALUE,
	RELATION,
	COUNT
};

export using ObjectProviderMask = std::bitset<std::to_underlying(EObjectProvider::COUNT)>;

export enum class EObjectFetchMode : unsigned char { UNKNOWN = 0, ASYNC, AWAIT_WITH_TIMEOUT, AWAIT };

export constexpr EObjectFetchMode cObjectFetchRequestDefaultMode = EObjectFetchMode::AWAIT_WITH_TIMEOUT;
export constexpr uint64_t cObjectFetchRequestDefaultTimeoutMs = 50;

export using ObjectId = uint64_t;

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
	COUNT
};

export enum class EObjectCapability : unsigned char {
	NO = 0,

	ANIMATED,
	AUTO_FILL_AVAILABLE,
	CHECKABLE,
	CLICKABLE,
	DRAGGABLE,
	EDITABLE,
	EXPANDABLE,
	FOCUSABLE,
	HAS_POPUP,
	HORIZONTAL,
	MAPPABLE,
	MULTI_LINE,
	MULTI_SELECTABLE,
	MOVEABLE,
	PINNABLE,
	RESIZABLE,
	SELECTABLE,
	SECURE,
	SORTABLE,
	TOUCH_OPTIMIZED,
	VERTICAL,

	HAS_TOOLTIP,
	LIVE_REGION,
	PROTECTED,
	SENSITIVE,

	COUNT
};

export using ObjectCapabilityMask = std::bitset<std::to_underlying(EObjectCapability::COUNT)>;

export enum class EObjectState : unsigned char {
	NO = 0,

	ACTIVE,
	BUSY,
	CHECKED,
	COLLAPSED,
	DEFAULT,
	DEFUNCT,
	DRAGGING,
	ENABLED,
	EXPANDED,
	FOCUSED,
	HOVERED,
	INDETERMINATE,
	INVALID,
	LINKED,
	LOADING,
	MODAL,
	OFFSCREEN,
	PRESSED,
	READONLY,
	REQUIRED,
	SELECTED,
	VISITED,
	VISIBLE,

	CLIPPED,
	HIDDEN,

	COUNT
};

export using ObjectStateMask = std::bitset<std::to_underlying(EObjectState::COUNT)>;

export enum class EObjectAction : unsigned char {
	UNKNOWN = 0,
	ACTIVATE,
	FOCUS,
	TOGGLE,
	COLLAPSE,
	EXPAND,
	INCREMENT,
	DECREMENT,
	SHOW_CONTEXT_MENU,
	CUSTOM
};

export enum class EObjectError : unsigned char {
	SUCCESS = 0,
	DEFUNCT,
	FETCH_SLOT_DEFUNCT,
	MANUAL_FETCH_REQUIRED,

	BUSY,
	NOTHING_TO_FETCH,

	NOT_SUPPORTED,
	ACCESS_DENIED,
	INVALID_ARGUMENTS,
	TIMEOUT,
	FAIL
};

export template <typename T = void> using ObjectResult = std::expected<T, EObjectError>;

export enum class EObjectEventType : unsigned char {
	NONE = 0,
	FOCUS_GAINED,
	FOCUS_LOST,
	CLICKED,
	NAME_CHANGED,
	DESCRIPTION_CHANGED,
	VALUE_CHANGED,
	SELECTION_CHANGED,
	TEXT_SELECTION_CHANGED,
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
	case FETCH_SLOT_DEFUNCT:
		return "Object Fetch Slot Defunct: The target object fetch slot is no longer valid.";
	case MANUAL_FETCH_REQUIRED:
		return "Manual Fetch Required: This field cannot be obtained automatically with the 'Lazy Fetch' function "
			   "disabled, and the request for its receipt must be pushed manually.";
	case BUSY:
		return "Busy: An operation to fill a free slot or another blocking operation is underway, which depends on the "
			   "implementation of the platform-dependent accessibility provider.";
	case NOTHING_TO_FETCH:
		return "Nothing To Fetch: An attempt to change the slot without an explicit fetch request means that the slot "
			   "does not want to switch unnecessarily.";
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
		break;
	}
	return "Unknown Error: An unexpected or undocumented failure occurred during the interaction with the "
		   "accessibility API.";
}

export enum class EObjectLive : unsigned char { UNKNOWN = 0, POLITE, ASSERTIVE };

export enum class EObjectRelationType : unsigned char {
	UNKNOWN = 0,
	LABELLED_BY,
	LABEL_FOR,
	DESCRIBED_BY,
	DESCRIPTION_FOR,
	CONTROLLED_BY,
	CONTROLLER_FOR,
	MEMBER_OF,
	FLOWS_TO,
	FLOWS_FROM,
	NODE_CHILD_OF,
	NODE_PARENT_OF,
	SUBWINDOW_OF,
	PARENT_WINDOW_OF,
	EMBEDS,
	EMBEDDED_BY,
	POPUP_FOR,
	DETAILS,
	DETAILS_FOR,
	ERROR_MESSAGE,
	ERROR_FOR,
	OWNS,
	TOOLTIP_FOR,
	RADIO_GROUP,
	EXTENDED,
};

export enum class EObjectFetchValue : unsigned char {
	UNKNOWN = 0,

	TYPE,
	STATES,
	CAPABILITIES,
	PARENT,
	CHILDREN,
	INDEX,
	BOUNDS,
	TOOLKIT_NAME,
	TOOLKIT_VERSION,
	NAME,
	DESCRIPTION,
	HELP_TEXT,

	TEXT_CURSOR,
	TEXT_LENGTH,
	TEXT,
	TEXT_SELECTION,
	TEXT_SELECTION_RANGE,
	TEXT_BY_GRANULARITY,

	SELECTED_CHILDREN,

	ACTION_TYPES,
	ACTION_NAMES,
	ACTION_DESCRIPTIONS,
	ACTION_HOTKEYS,
	ACTION_HOTKEY_STRINGS,
	ACTION_DO,

	VALUE_MIN,
	VALUE_MAX,
	VALUE_CURRENT,
	VALUE_STEP,
	VALUE_STRING,

	RELATION_TYPES,
	RELATION_TARGETS,
	COUNT
};

export using ObjectFetchMask = std::bitset<std::to_underlying(EObjectFetchValue::COUNT)>;

export template <EObjectFetchValue Value = EObjectFetchValue::UNKNOWN> struct TObjectFetchValue {
	using type = void;
};

template <> struct TObjectFetchValue<EObjectFetchValue::COUNT> final : TObjectFetchValue<EObjectFetchValue::UNKNOWN> {};
template <> struct TObjectFetchValue<EObjectFetchValue::TYPE> final {
	using type = EObjectType;
};
template <> struct TObjectFetchValue<EObjectFetchValue::STATES> final {
	using type = ObjectStateMask;
};
template <> struct TObjectFetchValue<EObjectFetchValue::CAPABILITIES> final {
	using type = ObjectCapabilityMask;
};
template <> struct TObjectFetchValue<EObjectFetchValue::PARENT> final {
	using type = void*;
};
template <> struct TObjectFetchValue<EObjectFetchValue::CHILDREN> {
	using type = std::vector<void*>;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::SELECTED_CHILDREN> final : TObjectFetchValue<EObjectFetchValue::CHILDREN> {
};
template <> struct TObjectFetchValue<EObjectFetchValue::INDEX> final {
	using type = int;
};
template <> struct TObjectFetchValue<EObjectFetchValue::BOUNDS> final {
	using type = SRect;
};
template <> struct TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {
	using type = std::string_view;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::TOOLKIT_VERSION> final
	: TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::NAME> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::DESCRIPTION> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::HELP_TEXT> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <> struct TObjectFetchValue<EObjectFetchValue::TEXT_CURSOR> {
	using type = size_t;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::TEXT_LENGTH> final : TObjectFetchValue<EObjectFetchValue::TEXT_CURSOR> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::TEXT> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::TEXT_SELECTION> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {
};
template <> struct TObjectFetchValue<EObjectFetchValue::TEXT_SELECTION_RANGE> final {
	using type = STextRange;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::TEXT_BY_GRANULARITY> final
	: TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <> struct TObjectFetchValue<EObjectFetchValue::ACTION_TYPES> final {
	using type = std::vector<EObjectAction>;
};
template <> struct TObjectFetchValue<EObjectFetchValue::ACTION_NAMES> {
	using type = std::vector<std::string>;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::ACTION_DESCRIPTIONS> final
	: TObjectFetchValue<EObjectFetchValue::ACTION_NAMES> {};
template <> struct TObjectFetchValue<EObjectFetchValue::ACTION_HOTKEYS> final {
	using type = std::vector<SHotkeyInfo>;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::ACTION_HOTKEY_STRINGS> final
	: TObjectFetchValue<EObjectFetchValue::ACTION_NAMES> {
	;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::ACTION_DO> final : TObjectFetchValue<EObjectFetchValue::UNKNOWN> {
	;
};
template <> struct TObjectFetchValue<EObjectFetchValue::VALUE_MIN> {
	using type = double;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::VALUE_MAX> final : TObjectFetchValue<EObjectFetchValue::VALUE_MIN> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::VALUE_CURRENT> final : TObjectFetchValue<EObjectFetchValue::VALUE_MIN> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::VALUE_STEP> final : TObjectFetchValue<EObjectFetchValue::VALUE_MIN> {};
template <>
struct TObjectFetchValue<EObjectFetchValue::VALUE_STRING> final : TObjectFetchValue<EObjectFetchValue::TOOLKIT_NAME> {};
template <> struct TObjectFetchValue<EObjectFetchValue::RELATION_TYPES> final {
	using type = std::vector<EObjectRelationType>;
};
template <>
struct TObjectFetchValue<EObjectFetchValue::RELATION_TARGETS> final : TObjectFetchValue<EObjectFetchValue::CHILDREN> {};

export struct SObjectFetchResult final {
	std::atomic_flag busy;
	std::atomic<unsigned int> pending_requests;

	void Done() noexcept {
		if (!busy.test(std::memory_order_acquire))
			return;

		else if (pending_requests.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			busy.clear(std::memory_order_release);
			busy.notify_one();
		}
	}

	std::pmr::memory_resource* pool;

	void MakeCopy(std::string_view data, ObjectResult<std::string_view>& memory) {
		if (!pool) [[unlikely]] return;
		if (memory.has_value()) {
			pool->deallocate((void*) memory->data(), memory->size());
		}
		auto allocated = pool->allocate(data.size());
		if (!allocated) [[unlikely]] return;

		std::memcpy(allocated, data.data(), data.size());
		memory = std::string_view((const char*) allocated, data.size());
	}
	template<typename T> void MakeCopy(std::span<T> data, ObjectResult<std::span<T>>& memory) {
		if (!pool) [[unlikely]] return;
		if (memory.has_value()) {
			pool->deallocate((void*) memory->data(), memory->size());
		}
		auto allocated = pool->allocate(data.size());
		if (!allocated) [[unlikely]] return;

		std::memcpy(allocated, data.data(), data.size());
		memory = std::span<T>((T*) allocated, data.size());
	}

	ObjectFetchMask mask;

private:
	using enum EObjectFetchValue;
	template <EObjectFetchValue Value> using FetchValueType = TObjectFetchValue<Value>::type;

public:
	ObjectResult<FetchValueType<TYPE>> type;
	ObjectResult<FetchValueType<STATES>> states;
	ObjectResult<FetchValueType<CAPABILITIES>> capabilities;
	ObjectResult<FetchValueType<PARENT>> parent;
	ObjectResult<FetchValueType<CHILDREN>> children, selected_children;
	ObjectResult<FetchValueType<INDEX>> index;
	ObjectResult<FetchValueType<BOUNDS>> bounds;
	ObjectResult<FetchValueType<TOOLKIT_NAME>> toolkit_name, toolkit_version, name, description, help_text;

	ObjectResult<FetchValueType<TEXT_CURSOR>> text_cursor, text_length;
	ObjectResult<FetchValueType<TEXT_SELECTION_RANGE>> text_selection_range;
	ObjectResult<FetchValueType<TEXT>> text, text_selection, text_by_granularity;

	ObjectResult<FetchValueType<ACTION_TYPES>> action_types;
	ObjectResult<FetchValueType<ACTION_NAMES>> action_names, action_descriptions, action_hotkey_strings;
	ObjectResult<FetchValueType<ACTION_HOTKEYS>> action_hotkeys;
	ObjectResult<FetchValueType<ACTION_DO>> action_do;

	ObjectResult<FetchValueType<VALUE_MIN>> value_min, value_max, value_current, value_step;
	ObjectResult<FetchValueType<VALUE_STRING>> value_string;

	ObjectResult<FetchValueType<RELATION_TYPES>> relation_types;
	ObjectResult<FetchValueType<RELATION_TARGETS>> relation_targets;

	SObjectFetchResult(std::pmr::memory_resource* new_pool)
		: pool(new_pool) {}
	SObjectFetchResult(const SObjectFetchResult&) = delete;
	auto operator=(const SObjectFetchResult&) = delete;
	SObjectFetchResult(SObjectFetchResult&&) = delete;
	auto operator=(SObjectFetchResult&&) = delete;
};

export struct SObjectFetchRequest final {
	void* native_handle;
	SObjectFetchResult* slot;
	ObjectFetchMask mask;
};

export using ObjectFetchQueue = TThreadSafeQueue<SObjectFetchRequest>;
