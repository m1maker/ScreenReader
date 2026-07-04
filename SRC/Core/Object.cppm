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
#include <bitset>
#include <expected>
#include <future>
#include <span>
#include <string_view>
#include <utility>
export module Core.Object;
import Core.KeyInfo;
import Core.Rect;
import Core.Text;
import Core.ThreadSafeQueue;

enum class EObjectFetchValue : unsigned char {
	UNKNOWN = 0,

	TYPE,
	STATES,
	CAPABILITIES,
	ANCESTORS,
	CHILDREN,
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

export using ObjectIdSpan = std::span<ObjectId>;
export using ObjectStringSpan = std::span<std::string_view>;
export using ObjectActionSpan = std::span<EObjectAction>;
export using ObjectHotkeySpan = std::span<SHotkeyInfo>;
export using ObjectRelationTypeSpan = std::span<EObjectRelationType>;

struct SObjectFetchResult final {
	ObjectId id;

	ObjectResult<EObjectType> type;
	ObjectResult<ObjectStateMask> states;
	ObjectResult<ObjectCapabilityMask> capabilities;
	ObjectResult<ObjectIdSpan> ancestors, children, selected_children;
	ObjectResult<SRect> bounds;
	ObjectResult<std::string_view> toolkit_name, toolkit_version, name, description, help_text;

	ObjectResult<size_t> cursor, text_length;
	ObjectResult<std::string_view> text, text_selection, text_by_granularity;

	ObjectResult<ObjectActionSpan> action_type;
	ObjectResult<ObjectStringSpan> action_names, action_descriptions, action_hotkey_strings;
	ObjectResult<ObjectHotkeySpan> action_hotkeys;
	ObjectResult<void> action_do;

	ObjectResult<double> value_min, value_max, value_current, value_step;
	ObjectResult<std::string_view> value_string;

	ObjectResult<ObjectRelationTypeSpan> relation_types;
	ObjectResult<ObjectIdSpan> relation_targets;
};

struct SObjectFetchRequest final {
	ObjectId id;
	ObjectFetchMask mask;

	std::promise<SObjectFetchResult> result;
};

export using ObjectFetchQueue = TThreadSafeQueue<SObjectFetchRequest>;
