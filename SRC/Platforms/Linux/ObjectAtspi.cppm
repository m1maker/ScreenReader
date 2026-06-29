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

// AT-SPI object implementation and some inlines.
module;
#include <atspi/atspi.h>
#include <expected>
#include <map>
#include <memory_resource>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
export module Platforms.Linux.Object;
import Core.Object;
import Core.Rect;
import Core.Text;
import Traits.RefCountedObject;

template <typename T> struct LifecycleTrait<T, std::enable_if_t<std::is_convertible_v<T*, GObject*>>> final {
	static void AddRef(T* p) {
		if (p)
			g_object_ref(p);
	}
	static void Release(T* p) {
		if (p)
			g_object_unref(p);
	}
};

export [[nodiscard]] constexpr inline auto GetObjectTypeFromAtspiRole(AtspiRole role) -> EObjectType {
	using enum EObjectType;
	switch (role) {
	case ATSPI_ROLE_INVALID:
	case ATSPI_ROLE_UNKNOWN:
	case ATSPI_ROLE_LAST_DEFINED:
		return UNKNOWN;
	case ATSPI_ROLE_ACCELERATOR_LABEL:
		return LABEL;
	case ATSPI_ROLE_ALERT:
		return ALERT;
	case ATSPI_ROLE_ANIMATION:
		return ANIMATION;
	case ATSPI_ROLE_APPLICATION:
		return APPLICATION;
	case ATSPI_ROLE_ARTICLE:
		return ARTICLE;
	case ATSPI_ROLE_AUDIO:
		return AUDIO;
	case ATSPI_ROLE_AUTOCOMPLETE:
		return AUTO_COMPLETE;
	case ATSPI_ROLE_BLOCK_QUOTE:
		return BLOCKQUOTE;
	case ATSPI_ROLE_BUTTON:
		return BUTTON;
	case ATSPI_ROLE_CANVAS:
		return CANVAS;
	case ATSPI_ROLE_CAPTION:
		return CAPTION;
	case ATSPI_ROLE_CHART:
		return CHART;
	case ATSPI_ROLE_CHECK_BOX:
		return CHECKBOX;
	case ATSPI_ROLE_CHECK_MENU_ITEM:
		return MENU_ITEM_CHECKBOX;
	case ATSPI_ROLE_COLOR_CHOOSER:
		return COLOR_WELL;
	case ATSPI_ROLE_COLUMN_HEADER:
		return COLUMN_HEADER;
	case ATSPI_ROLE_COMBO_BOX:
		return COMBO_BOX;
	case ATSPI_ROLE_COMMENT:
		return COMMENT;
	case ATSPI_ROLE_CONTENT_DELETION:
		return CONTENT_DELETION;
	case ATSPI_ROLE_CONTENT_INSERTION:
		return CONTENT_INSERTION;
	case ATSPI_ROLE_DEFINITION:
		return DEFINITION;
	case ATSPI_ROLE_DESCRIPTION_LIST:
		return DESCRIPTION_LIST;
	case ATSPI_ROLE_DIALOG:
		return DIALOG;
	case ATSPI_ROLE_DOCUMENT_FRAME:
		return DOCUMENT;
	case ATSPI_ROLE_DOCUMENT_WEB:
		return WEB_VIEW;
	case ATSPI_ROLE_EDITBAR:
		return TOOLBAR;
	case ATSPI_ROLE_EMBEDDED:
		return EMBEDDED_OBJECT;
	case ATSPI_ROLE_ENTRY:
		return TEXT_FIELD;
	case ATSPI_ROLE_FILE_CHOOSER:
		return FILE_UPLOAD;
	case ATSPI_ROLE_FOOTER:
		return FOOTER;
	case ATSPI_ROLE_FORM:
		return FORM;
	case ATSPI_ROLE_GROUPING:
		return GROUP;
	case ATSPI_ROLE_HEADER:
		return HEADER;
	case ATSPI_ROLE_HEADING:
		return HEADING;
	case ATSPI_ROLE_IMAGE:
		return IMAGE;
	case ATSPI_ROLE_IMAGE_MAP:
		return IMAGE_MAP;
	case ATSPI_ROLE_LABEL:
		return LABEL;
	case ATSPI_ROLE_LINK:
		return LINK;
	case ATSPI_ROLE_LIST:
		return LIST;
	case ATSPI_ROLE_LIST_BOX:
		return LIST_BOX;
	case ATSPI_ROLE_LIST_ITEM:
		return LIST_ITEM;
	case ATSPI_ROLE_LOG:
		return LOG;
	case ATSPI_ROLE_MARK:
		return MARK;
	case ATSPI_ROLE_MARQUEE:
		return MARQUEE;
	case ATSPI_ROLE_MATH:
		return MATH;
	case ATSPI_ROLE_MATH_FRACTION:
		return MATH_FRACTION;
	case ATSPI_ROLE_MATH_ROOT:
		return MATH_ROOT;
	case ATSPI_ROLE_MENU:
		return MENU;
	case ATSPI_ROLE_MENU_BAR:
		return MENU_BAR;
	case ATSPI_ROLE_MENU_ITEM:
		return MENU_ITEM;
	case ATSPI_ROLE_NOTIFICATION:
		return ALERT;
	case ATSPI_ROLE_PAGE_TAB:
		return TAB;
	case ATSPI_ROLE_PAGE_TAB_LIST:
		return TAB_LIST;
	case ATSPI_ROLE_PANEL:
		return PANEL;
	case ATSPI_ROLE_PARAGRAPH:
		return PARAGRAPH;
	case ATSPI_ROLE_PASSWORD_TEXT:
		return PASSWORD_FIELD;
	case ATSPI_ROLE_PROGRESS_BAR:
		return PROGRESS_BAR;
	case ATSPI_ROLE_RADIO_BUTTON:
		return RADIO_BUTTON;
	case ATSPI_ROLE_RADIO_MENU_ITEM:
		return MENU_ITEM_RADIO;
	case ATSPI_ROLE_RATING:
		return RATING_INDICATOR;
	case ATSPI_ROLE_ROW_HEADER:
		return ROW_HEADER;
	case ATSPI_ROLE_SCROLL_BAR:
		return SCROLL_BAR;
	case ATSPI_ROLE_SECTION:
		return SECTION;
	case ATSPI_ROLE_SEPARATOR:
		return SEPARATOR;
	case ATSPI_ROLE_SLIDER:
		return SLIDER;
	case ATSPI_ROLE_SPIN_BUTTON:
		return SPIN_BUTTON;
	case ATSPI_ROLE_SPLIT_PANE:
		return SPLITTER;
	case ATSPI_ROLE_STATUS_BAR:
		return STATUS;
	case ATSPI_ROLE_SUBSCRIPT:
		return SUBSCRIPT;
	case ATSPI_ROLE_SUPERSCRIPT:
		return SUPERSCRIPT;
	case ATSPI_ROLE_SWITCH:
		return SWITCH;
	case ATSPI_ROLE_TABLE:
		return TABLE;
	case ATSPI_ROLE_TABLE_CELL:
		return CELL;
	case ATSPI_ROLE_TABLE_ROW:
		return ROW;
	case ATSPI_ROLE_TERMINAL:
		return TERMINAL;
	case ATSPI_ROLE_TEXT:
		return TEXT_FIELD;
	case ATSPI_ROLE_TIMER:
		return TIMER;
	case ATSPI_ROLE_TITLE_BAR:
		return TITLE_BAR;
	case ATSPI_ROLE_TOGGLE_BUTTON:
		return TOGGLE_BUTTON;
	case ATSPI_ROLE_TOOL_BAR:
		return TOOLBAR;
	case ATSPI_ROLE_TOOL_TIP:
		return TOOLTIP;
	case ATSPI_ROLE_TREE:
		return TREE;
	case ATSPI_ROLE_TREE_ITEM:
		return TREE_ITEM;
	case ATSPI_ROLE_VIEWPORT:
		return VIEWPORT;
	case ATSPI_ROLE_WINDOW:
		return WINDOW;
	case ATSPI_ROLE_ARROW:
		return UNKNOWN;
	case ATSPI_ROLE_CALENDAR:
		return DATE_PICKER;
	case ATSPI_ROLE_DATE_EDITOR:
		return DATE_PICKER;
	case ATSPI_ROLE_DESKTOP_ICON:
		return UNKNOWN;
	case ATSPI_ROLE_DESKTOP_FRAME:
		return WINDOW;
	case ATSPI_ROLE_DIAL:
		return SPIN_BUTTON;
	case ATSPI_ROLE_DIRECTORY_PANE:
		return LIST;
	case ATSPI_ROLE_DRAWING_AREA:
		return CANVAS;
	case ATSPI_ROLE_FILLER:
		return SEPARATOR;
	case ATSPI_ROLE_FOCUS_TRAVERSABLE:
		return UNKNOWN;
	case ATSPI_ROLE_FONT_CHOOSER:
		return COLOR_WELL;
	case ATSPI_ROLE_FRAME:
		return WINDOW;
	case ATSPI_ROLE_GLASS_PANE:
		return PANEL;
	case ATSPI_ROLE_HTML_CONTAINER:
		return EMBEDDED_OBJECT;
	case ATSPI_ROLE_ICON:
		return IMAGE;
	case ATSPI_ROLE_INTERNAL_FRAME:
		return WINDOW;
	case ATSPI_ROLE_LAYERED_PANE:
		return PANEL;
	case ATSPI_ROLE_OPTION_PANE:
		return DIALOG;
	case ATSPI_ROLE_POPUP_MENU:
		return MENU;
	case ATSPI_ROLE_ROOT_PANE:
		return PANEL;
	case ATSPI_ROLE_SCROLL_PANE:
		return SCROLL_VIEW;
	case ATSPI_ROLE_TABLE_COLUMN_HEADER:
		return COLUMN_HEADER;
	case ATSPI_ROLE_TABLE_ROW_HEADER:
		return ROW_HEADER;
	case ATSPI_ROLE_TEAROFF_MENU_ITEM:
		return MENU_ITEM;
	case ATSPI_ROLE_TREE_TABLE:
		return TREE_GRID;
	case ATSPI_ROLE_EXTENDED:
		return UNKNOWN;
	case ATSPI_ROLE_RULER:
		return UNKNOWN;
	case ATSPI_ROLE_PAGE:
		return TAB;
	case ATSPI_ROLE_REDUNDANT_OBJECT:
		return UNKNOWN;
	case ATSPI_ROLE_INPUT_METHOD_WINDOW:
		return WINDOW;
	case ATSPI_ROLE_DOCUMENT_SPREADSHEET:
		return TABLE;
	case ATSPI_ROLE_DOCUMENT_PRESENTATION:
		return DOCUMENT;
	case ATSPI_ROLE_DOCUMENT_TEXT:
		return DOCUMENT;
	case ATSPI_ROLE_DOCUMENT_EMAIL:
		return DOCUMENT;
	case ATSPI_ROLE_INFO_BAR:
		return STATUS;
	case ATSPI_ROLE_LEVEL_BAR:
		return PROGRESS_BAR;
	case ATSPI_ROLE_VIDEO:
		return VIDEO;
	case ATSPI_ROLE_LANDMARK:
		return REGION;
	case ATSPI_ROLE_STATIC:
		return LABEL;
	case ATSPI_ROLE_DESCRIPTION_TERM:
		return DEFINITION;
	case ATSPI_ROLE_DESCRIPTION_VALUE:
		return DEFINITION;
	case ATSPI_ROLE_FOOTNOTE:
		return NOTE;
	case ATSPI_ROLE_SUGGESTION:
		return CONTENT_INSERTION;
	case ATSPI_ROLE_PUSH_BUTTON_MENU:
		return BUTTON;
	}
	return UNKNOWN;
}

/*
AT-SPI has a problem: it mixes the capabilities of an object and its states into one entity (enum, function).
In order not to call the same function a bunch of times, first to convert it to capabilities, and then states, we will
create a variant for holding both types and a structure for merging (when we get it through array).
*/

using StateVariant = std::variant<std::monostate, EObjectState, EObjectCapability>;

struct SMergedState final {
	ObjectStateMask states;
	ObjectCapabilityMask capabilities;
};

export [[nodiscard]] constexpr inline auto GetStateVariantFromAtspiState(AtspiStateType state) -> StateVariant {
	switch (state) {
	case ATSPI_STATE_INVALID:
	case ATSPI_STATE_SHOWING:
	case ATSPI_STATE_SINGLE_LINE:
	case ATSPI_STATE_STALE:
	case ATSPI_STATE_LAST_DEFINED:
	case ATSPI_STATE_ICONIFIED:
	case ATSPI_STATE_MANAGES_DESCENDANTS:
		return {};
	case ATSPI_STATE_ACTIVE:
		return EObjectState::ACTIVE;
	case ATSPI_STATE_ARMED:
		return EObjectState::HOVERED;
	case ATSPI_STATE_ANIMATED:
		return EObjectCapability::ANIMATED;
	case ATSPI_STATE_SUPPORTS_AUTOCOMPLETION:
		return EObjectCapability::AUTO_FILL_AVAILABLE;
	case ATSPI_STATE_BUSY:
	case ATSPI_STATE_TRANSIENT:
		return EObjectState::BUSY;
	case ATSPI_STATE_CHECKABLE:
		return EObjectCapability::CHECKABLE;
	case ATSPI_STATE_CHECKED:
		return EObjectState::CHECKED;
	case ATSPI_STATE_COLLAPSED:
		return EObjectState::COLLAPSED;
	case ATSPI_STATE_DEFUNCT:
		return EObjectState::DEFUNCT;
	case ATSPI_STATE_EDITABLE:
		return EObjectCapability::EDITABLE;
	case ATSPI_STATE_ENABLED:
		return EObjectState::ENABLED;
	case ATSPI_STATE_EXPANDABLE:
		return EObjectCapability::EXPANDABLE;
	case ATSPI_STATE_EXPANDED:
		return EObjectState::EXPANDED;
	case ATSPI_STATE_FOCUSABLE:
		return EObjectCapability::FOCUSABLE;
	case ATSPI_STATE_FOCUSED:
		return EObjectState::FOCUSED;
	case ATSPI_STATE_OPAQUE:
		return EObjectState::HIDDEN;
	case ATSPI_STATE_HAS_POPUP:
		return EObjectCapability::HAS_POPUP;
	case ATSPI_STATE_HAS_TOOLTIP:
		return EObjectCapability::HAS_TOOLTIP;
	case ATSPI_STATE_HORIZONTAL:
		return EObjectCapability::HORIZONTAL;
	case ATSPI_STATE_INDETERMINATE:
		return EObjectState::INDETERMINATE;
	case ATSPI_STATE_INVALID_ENTRY:
	case ATSPI_STATE_TRUNCATED:
		return EObjectState::INVALID;
	case ATSPI_STATE_IS_DEFAULT:
		return EObjectState::DEFAULT;
	case ATSPI_STATE_MODAL:
		return EObjectState::MODAL;
	case ATSPI_STATE_MULTI_LINE:
		return EObjectCapability::MULTI_LINE;
	case ATSPI_STATE_MULTISELECTABLE:
		return EObjectCapability::MULTI_SELECTABLE;
	case ATSPI_STATE_PRESSED:
		return EObjectState::PRESSED;
	case ATSPI_STATE_READ_ONLY:
		return EObjectState::READONLY;
	case ATSPI_STATE_REQUIRED:
		return EObjectState::REQUIRED;
	case ATSPI_STATE_RESIZABLE:
		return EObjectCapability::RESIZABLE;
	case ATSPI_STATE_SELECTABLE:
	case ATSPI_STATE_SELECTABLE_TEXT:
		return EObjectCapability::SELECTABLE;
	case ATSPI_STATE_SELECTED:
		return EObjectState::SELECTED;
	case ATSPI_STATE_SENSITIVE:
		return EObjectCapability::SENSITIVE;
	case ATSPI_STATE_VERTICAL:
		return EObjectCapability::VERTICAL;
	case ATSPI_STATE_VISIBLE:
		return EObjectState::VISIBLE;
	case ATSPI_STATE_VISITED:
		return EObjectState::VISITED;
	}
	return {};
}

export [[nodiscard]] inline auto GetMergedObjectStateFromAtspiStates(AtspiStateSet* state_set) -> SMergedState {
	SMergedState state{};
	for (int i = ATSPI_STATE_INVALID; i < ATSPI_STATE_LAST_DEFINED; ++i) {
		auto atspi_state = static_cast<AtspiStateType>(i);
		auto enabled = atspi_state_set_contains(state_set, atspi_state);
		if (!enabled)
			continue;
		auto state_variant = GetStateVariantFromAtspiState(atspi_state);
		std::visit(
			[&state](auto&& value) {
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same_v<T, EObjectState>)
					state.states[std::to_underlying(value)] = true;
				else if constexpr (std::is_same_v<T, EObjectCapability>)
					state.capabilities[std::to_underlying(value)] = true;
			},
			state_variant);
	}

	return state;
}

export [[nodiscard]] constexpr inline auto GetAtspiTextGranularityFromTextGranularity(ETextGranularity granularity)
	-> AtspiTextGranularity {
	switch (granularity) {
	case ETextGranularity::CHARACTER:
		return ATSPI_TEXT_GRANULARITY_CHAR;
	case ETextGranularity::WORD:
		return ATSPI_TEXT_GRANULARITY_WORD;
	case ETextGranularity::SENTENCE:
		return ATSPI_TEXT_GRANULARITY_SENTENCE;
	case ETextGranularity::LINE:
		return ATSPI_TEXT_GRANULARITY_LINE;
	case ETextGranularity::PARAGRAPH:
		return ATSPI_TEXT_GRANULARITY_PARAGRAPH;
	default:
		return ATSPI_TEXT_GRANULARITY_CHAR;
	}
}

export template <class T> [[nodiscard]] inline auto GetTextRangeFromAtspiRange(const T& range) -> STextRange {
	STextRange text_range;
	if constexpr (std::is_same_v<T, AtspiTextRange>) {
		text_range.text = range.content;
	}

	text_range.start = range.start_offset;
	text_range.end = range.end_offset;
	return text_range;
}

export template <typename T> struct SAtspiIface final {
	T* pointer{nullptr};
	explicit SAtspiIface(T* p) noexcept : pointer(p) {}
	~SAtspiIface() noexcept {
		if (pointer)
			g_object_unref(pointer);
	}
	operator T*() const noexcept { return pointer; }
};

export class CObjectAtspi final {
	std::pmr::memory_resource* m_pool{nullptr};

	mutable AtspiAccessible* m_accessible{nullptr};

	mutable struct SObjectAtspiData* m_data{nullptr};

public:
	using NativeHandle = AtspiAccessible*;
	CObjectAtspi() = default;
	explicit CObjectAtspi(AtspiAccessible* accessible, struct SObjectAtspiData* data, std::pmr::memory_resource* pool);

	auto operator==(const CObjectAtspi& other) const noexcept { return m_accessible == other.m_accessible; }

	//[[nodiscard]] auto GetSupportedInterfaces() const noexcept -> uint32_t ;

	[[nodiscard]] auto GetNativeHandle() const noexcept -> ObjectResult<NativeHandle> { return m_accessible; }

	[[nodiscard]] inline auto IsValid() const noexcept -> bool {
		return m_accessible != nullptr && m_data != nullptr && m_pool != nullptr;
	}

	[[nodiscard]] auto GetType() const -> ObjectResult<EObjectType>;

	[[nodiscard]] auto GetState() const -> ObjectResult<ObjectStateMask>;
	[[nodiscard]] auto GetCapabilities() const -> ObjectResult<ObjectCapabilityMask>;

	[[nodiscard]] auto GetParent() const -> ObjectResult<CObjectAtspi>;
	[[nodiscard]] auto GetChildren() const -> ObjectResult<std::vector<CObjectAtspi>>;
	[[nodiscard]] auto GetChildAt(int index) const -> ObjectResult<CObjectAtspi>;

	[[nodiscard]] auto GetChildrenCount() const -> ObjectResult<int>;

	[[nodiscard]] auto GetBounds() const -> ObjectResult<SRect>;

	[[nodiscard]] auto GetIndex() const -> ObjectResult<int>;

	[[nodiscard]] auto GetApplicationName() const -> ObjectResult<std::string_view>;

	[[nodiscard]] auto GetName() const -> ObjectResult<std::string_view>;
	[[nodiscard]] auto GetDescription() const -> ObjectResult<std::string_view>;

	void UpdateCacheByEvent(EObjectEventType event);

	[[nodiscard]] auto GetCursor() const -> ObjectResult<int>;
	[[nodiscard]] auto GetText(int cursor, const ETextGranularity& granularity) const -> ObjectResult<STextRange>;
	[[nodiscard]] auto GetSelectedText() const -> ObjectResult<STextRange>;

	[[nodiscard]] auto GetSelectedChildAt(int index) const -> ObjectResult<CObjectAtspi>;
	[[nodiscard]] auto GetSelectedChildrenCount() const -> ObjectResult<int>;

	[[nodiscard]] auto GetMinValue() const -> ObjectResult<double>;
	[[nodiscard]] auto GetMaxValue() const -> ObjectResult<double>;
	[[nodiscard]] auto GetCurrentValue() const -> ObjectResult<double>;

	[[nodiscard]] auto GetActionCount() const -> ObjectResult<int>;

	[[nodiscard]] auto GetActionType(int number) const -> ObjectResult<EObjectAction>;
	[[nodiscard]] auto GetActionName(int number) const -> ObjectResult<std::string_view>;
	[[nodiscard]] auto DoAction(int number) -> ObjectResult<>;
};

export struct SObjectAtspiData final {
	gchar* app_name{nullptr};
	gchar* name{nullptr};
	gchar* description{nullptr};
	gchar* last_text{nullptr};
	gchar* action_name{nullptr};

	mutable GError* last_error{nullptr};
	uint32_t interfaces_mask{0};

	inline void ResetLastError() const noexcept {
		if (last_error) {
			g_error_free(last_error);
			last_error = nullptr;
		}
	}

	~SObjectAtspiData() {
		ResetLastError();
		if (app_name)
			g_free(app_name);
		if (name)
			g_free(name);
		if (description)
			g_free(description);
		if (last_text)
			g_free(last_text);
		if (action_name)
			g_free(action_name);
		app_name = nullptr;
		name = nullptr;
		description = nullptr;
		last_text = nullptr;
		action_name = nullptr;
	}
};
