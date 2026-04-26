module;
#include <array>
#include <bitset>
#include <string_view>
#include <utility>
export module Core.ObjectMeta;
import Core.Object;

export enum class EObjectGroup : unsigned char {
	UNKNOWN = 0,
	PARENT,
	CONTAINER,
	DATA_VIEW,
	DATA_ELIMENT,
	INPUT,
	MATH,
	NAVIGATION,
	WEB,
	VALUE,
	FEEDBACK,
	COUNT
};

using ObjectGroupMask = std::bitset<std::to_underlying(EObjectGroup::COUNT)>;

struct SObjectMeta final {
	std::string_view speech_name;
	ObjectGroupMask group_flags;
};

[[nodiscard]] static consteval auto InitializeMeta(EObjectType type) -> SObjectMeta {
	SObjectMeta meta;

	switch (type) {
	case EObjectType::UNKNOWN:
		meta.speech_name = "unknown";
		meta.group_flags.set(std::to_underlying(EObjectGroup::UNKNOWN));
		break;

	case EObjectType::ABBREVIATION:
		meta.speech_name = "abbreviation";
		break;

	case EObjectType::ANIMATION:
		meta.speech_name = "animation";
		break;

	case EObjectType::AUDIO:
		meta.speech_name = "audio";
		break;

	case EObjectType::AUTO_COMPLETE:
		meta.speech_name = "auto complete";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		break;

	case EObjectType::BANNER:
		meta.speech_name = "banner";
		break;

	case EObjectType::BLOCKQUOTE:
		meta.speech_name = "blockquote";
		break;

	case EObjectType::CANVAS:
		meta.speech_name = "canvas";
		break;

	case EObjectType::CAPTION:
		meta.speech_name = "caption";
		break;

	case EObjectType::CODE:
		meta.speech_name = "code";
		break;

	case EObjectType::COMMENT:
		meta.speech_name = "comment";
		break;

	case EObjectType::COMPLEMENTARY:
		meta.speech_name = "complementary";
		break;

	case EObjectType::CONTENT_DELETION:
		meta.speech_name = "content deletion";
		break;

	case EObjectType::CONTENT_INFO:
		meta.speech_name = "content info";
		break;

	case EObjectType::CONTENT_INSERTION:
		meta.speech_name = "content insertion";
		break;

	case EObjectType::DEFINITION:
		meta.speech_name = "definition";
		break;

	case EObjectType::DESCRIPTION_LIST:
		meta.speech_name = "description list";
		break;

	case EObjectType::DETAILS:
		meta.speech_name = "details";
		break;

	case EObjectType::DIRECTORY:
		meta.speech_name = "directory";
		break;

	case EObjectType::DISCLOSURE_TRIANGLE:
		meta.speech_name = "disclosure triangle";
		break;

	case EObjectType::EMBEDDED_OBJECT:
		meta.speech_name = "embedded object";
		break;

	case EObjectType::EMPHASIS:
		meta.speech_name = "emphasis";
		break;

	case EObjectType::FEED:
		meta.speech_name = "feed";
		break;

	case EObjectType::FIGURE:
		meta.speech_name = "figure";
		break;

	case EObjectType::FIGURE_CAPTION:
		meta.speech_name = "figure caption";
		break;

	case EObjectType::FILE_UPLOAD:
		meta.speech_name = "file upload";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		break;

	case EObjectType::GENERIC_CONTAINER:
		meta.speech_name = "generic container";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::HEADING:
		meta.speech_name = "heading";
		break;

	case EObjectType::IFRAME:
		meta.speech_name = "iframe";
		break;

	case EObjectType::IMAGE:
		meta.speech_name = "image";
		break;

	case EObjectType::IMAGE_MAP:
		meta.speech_name = "image map";
		break;

	case EObjectType::INLINE_TEXT:
		meta.speech_name = "inline text";
		break;

	case EObjectType::INPUT_DEVICE:
		meta.speech_name = "input device";
		break;

	case EObjectType::KEY:
		meta.speech_name = "key";
		break;

	case EObjectType::KEYBOARD:
		meta.speech_name = "keyboard";
		break;

	case EObjectType::LABEL:
		meta.speech_name = "label";
		break;

	case EObjectType::LAYOUT_AREA:
		meta.speech_name = "layout area";
		break;

	case EObjectType::LEGEND:
		meta.speech_name = "legend";
		break;

	case EObjectType::LINE:
		meta.speech_name = "line";
		break;

	case EObjectType::LIST_BOX:
		meta.speech_name = "list box";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::LIST_MARKER:
		meta.speech_name = "list marker";
		break;

	case EObjectType::MAIN:
		meta.speech_name = "main";
		break;

	case EObjectType::MARK:
		meta.speech_name = "mark";
		break;

	case EObjectType::MARQUEE:
		meta.speech_name = "marquee";
		break;

	case EObjectType::NOTE:
		meta.speech_name = "note";
		break;

	case EObjectType::PARAGRAPH:
		meta.speech_name = "paragraph";
		break;

	case EObjectType::PREFORMATTED:
		meta.speech_name = "preformatted";
		break;

	case EObjectType::REGION:
		meta.speech_name = "region";
		break;

	case EObjectType::ROW_GROUP:
		meta.speech_name = "row group";
		break;

	case EObjectType::RUBY:
		meta.speech_name = "ruby";
		break;

	case EObjectType::SLIDER_THUMB:
		meta.speech_name = "slider thumb";
		break;

	case EObjectType::SPLITTER:
		meta.speech_name = "splitter";
		break;

	case EObjectType::STRONG:
		meta.speech_name = "strong";
		break;

	case EObjectType::SUBSCRIPT:
		meta.speech_name = "subscript";
		break;

	case EObjectType::SUPERSCRIPT:
		meta.speech_name = "superscript";
		break;

	case EObjectType::SUPPLEMENTARY_CONTENT:
		meta.speech_name = "supplementary content";
		break;

	case EObjectType::SVG_ROOT:
		meta.speech_name = "svg root";
		break;

	case EObjectType::TAB_PANEL:
		meta.speech_name = "tab panel";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::TERMINAL:
		meta.speech_name = "terminal";
		break;

	case EObjectType::COUNT:
		break;

	case EObjectType::SEARCH:
		meta.speech_name = "search";
		meta.group_flags.set(std::to_underlying(EObjectGroup::NAVIGATION));
		break;

	case EObjectType::SECTION_FOOTER:
		meta.speech_name = "section footer";
		break;

	case EObjectType::SECTION_HEADER:
		meta.speech_name = "section header";
		break;

	case EObjectType::SEPARATOR:
		meta.speech_name = "separator";
		break;
	case EObjectType::TITLE_BAR:
		meta.speech_name = "title bar";
		break;

	case EObjectType::TREE:
		meta.speech_name = "tree";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_VIEW));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::VIDEO:
		meta.speech_name = "video";
		break;
	case EObjectType::APPLICATION:
		meta.speech_name = "application";
		meta.group_flags.set(std::to_underlying(EObjectGroup::PARENT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::DIALOG:
	case EObjectType::ALERT_DIALOG:
		meta.speech_name = (type == EObjectType::DIALOG) ? "dialog" : "alert dialog";
		meta.group_flags.set(std::to_underlying(EObjectGroup::PARENT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		meta.group_flags.set(std::to_underlying(EObjectGroup::FEEDBACK));
		break;

	case EObjectType::WINDOW:
	case EObjectType::SHEET:
	case EObjectType::DRAWER:
		meta.speech_name = (type == EObjectType::WINDOW) ? "window" : (type == EObjectType::SHEET) ? "sheet" : "drawer";
		meta.group_flags.set(std::to_underlying(EObjectGroup::PARENT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::GROUP:
	case EObjectType::PANEL:
	case EObjectType::SCROLL_VIEW:
	case EObjectType::VIEWPORT:
	case EObjectType::WEB_VIEW:
		meta.speech_name = (type == EObjectType::GROUP) ? "group"
			: (type == EObjectType::PANEL)				? "panel"
			: (type == EObjectType::SCROLL_VIEW)		? "scroll view"
			: (type == EObjectType::VIEWPORT)			? "viewport"
														: "web view";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::FORM:
	case EObjectType::MENU_BAR:
	case EObjectType::TOOLBAR:
	case EObjectType::TAB_LIST:
		meta.speech_name = (type == EObjectType::FORM) ? "form"
			: (type == EObjectType::MENU_BAR)		   ? "menu bar"
			: (type == EObjectType::TOOLBAR)		   ? "toolbar"
													   : "tab list";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::LIST:
	case EObjectType::MENU:
	case EObjectType::RADIO_GROUP:
		meta.speech_name = (type == EObjectType::LIST) ? "list" : (type == EObjectType::MENU) ? "menu" : "radio group";
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::TABLE:
	case EObjectType::GRID:
	case EObjectType::TREE_GRID:
		meta.speech_name = (type == EObjectType::TABLE) ? "table" : (type == EObjectType::GRID) ? "grid" : "tree grid";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_VIEW));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::CAROUSEL:
	case EObjectType::PAGINATION:
		meta.speech_name = (type == EObjectType::CAROUSEL) ? "carousel" : "pagination";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_VIEW));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::CHART:
	case EObjectType::MAP:
		meta.speech_name = (type == EObjectType::CHART) ? "chart" : "map";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_VIEW));
		break;

	case EObjectType::CELL:
	case EObjectType::GRID_CELL:
	case EObjectType::ROW:
	case EObjectType::ROW_HEADER:
	case EObjectType::COLUMN:
	case EObjectType::COLUMN_HEADER:
		meta.speech_name = (type == EObjectType::CELL) ? "cell"
			: (type == EObjectType::GRID_CELL)		   ? "grid cell"
			: (type == EObjectType::ROW)			   ? "row"
			: (type == EObjectType::ROW_HEADER)		   ? "row header"
			: (type == EObjectType::COLUMN)			   ? "column"
													   : "column header";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_ELIMENT));
		break;

	case EObjectType::LIST_ITEM:
	case EObjectType::TREE_ITEM:
	case EObjectType::MENU_ITEM:
		meta.speech_name = (type == EObjectType::LIST_ITEM) ? "list item"
			: (type == EObjectType::TREE_ITEM)				? "tree item"
															: "menu item";
		meta.group_flags.set(std::to_underlying(EObjectGroup::DATA_ELIMENT));
		break;

	case EObjectType::BUTTON:
	case EObjectType::TOGGLE_BUTTON:
	case EObjectType::MENU_ITEM_CHECKBOX:
	case EObjectType::MENU_ITEM_RADIO:
		meta.speech_name = (type == EObjectType::BUTTON) ? "button"
			: (type == EObjectType::TOGGLE_BUTTON)		 ? "toggle button"
			: (type == EObjectType::MENU_ITEM_CHECKBOX)	 ? "menu item checkbox"
														 : "menu item radio";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		break;

	case EObjectType::CHECKBOX:
	case EObjectType::RADIO_BUTTON:
	case EObjectType::SWITCH:
		meta.speech_name = (type == EObjectType::CHECKBOX) ? "checkbox"
			: (type == EObjectType::RADIO_BUTTON)		   ? "radio button"
														   : "switch";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::VALUE));
		break;

	case EObjectType::TEXT_FIELD:
	case EObjectType::PASSWORD_FIELD:
	case EObjectType::SEARCH_BOX:
	case EObjectType::COMBO_BOX:
		meta.speech_name = (type == EObjectType::TEXT_FIELD) ? "text field"
			: (type == EObjectType::PASSWORD_FIELD)			 ? "password field"
			: (type == EObjectType::SEARCH_BOX)				 ? "search box"
															 : "combo box";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::VALUE));
		break;

	case EObjectType::SLIDER:
	case EObjectType::SCROLL_BAR:
	case EObjectType::SPIN_BUTTON:
	case EObjectType::DATE_PICKER:
	case EObjectType::COLOR_WELL:
		meta.speech_name = (type == EObjectType::SLIDER) ? "slider"
			: (type == EObjectType::SCROLL_BAR)			 ? "scroll bar"
			: (type == EObjectType::SPIN_BUTTON)		 ? "spin button"
			: (type == EObjectType::DATE_PICKER)		 ? "date picker"
														 : "color well";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::VALUE));
		break;

	case EObjectType::PROGRESS_BAR:
	case EObjectType::METER:
		meta.speech_name = (type == EObjectType::PROGRESS_BAR) ? "progress bar" : "meter";
		meta.group_flags.set(std::to_underlying(EObjectGroup::INPUT));
		meta.group_flags.set(std::to_underlying(EObjectGroup::FEEDBACK));
		meta.group_flags.set(std::to_underlying(EObjectGroup::VALUE));
		break;

	case EObjectType::MATH:
	case EObjectType::MATH_FRACTION:
	case EObjectType::MATH_OPERATOR:
	case EObjectType::MATH_ROOT:
	case EObjectType::MATH_SUBSCRIPT:
	case EObjectType::MATH_SUPERSCRIPT:
		meta.speech_name = (type == EObjectType::MATH) ? "math"
			: (type == EObjectType::MATH_FRACTION)	   ? "math fraction"
			: (type == EObjectType::MATH_OPERATOR)	   ? "math operator"
			: (type == EObjectType::MATH_ROOT)		   ? "math root"
			: (type == EObjectType::MATH_SUBSCRIPT)	   ? "math subscript"
													   : "math superscript";
		meta.group_flags.set(std::to_underlying(EObjectGroup::MATH));
		break;

	case EObjectType::NAVIGATION:
	case EObjectType::BREADCRUMB:
	case EObjectType::ANCHOR:
	case EObjectType::LINK:
	case EObjectType::TAB:
		meta.speech_name = (type == EObjectType::NAVIGATION) ? "navigation"
			: (type == EObjectType::BREADCRUMB)				 ? "breadcrumb"
			: (type == EObjectType::ANCHOR)					 ? "anchor"
			: (type == EObjectType::LINK)					 ? "link"
															 : "tab";
		meta.group_flags.set(std::to_underlying(EObjectGroup::NAVIGATION));
		break;

	case EObjectType::DOCUMENT:
	case EObjectType::ARTICLE:
	case EObjectType::SECTION:
	case EObjectType::HEADER:
	case EObjectType::FOOTER:
		meta.speech_name = (type == EObjectType::DOCUMENT) ? "document"
			: (type == EObjectType::ARTICLE)			   ? "article"
			: (type == EObjectType::SECTION)			   ? "section"
			: (type == EObjectType::HEADER)				   ? "header"
														   : "footer";
		meta.group_flags.set(std::to_underlying(EObjectGroup::WEB));
		meta.group_flags.set(std::to_underlying(EObjectGroup::CONTAINER));
		break;

	case EObjectType::RATING_INDICATOR:
	case EObjectType::STATUS:
	case EObjectType::TIME:
	case EObjectType::TIMER:
		meta.speech_name = (type == EObjectType::RATING_INDICATOR) ? "rating indicator"
			: (type == EObjectType::STATUS)						   ? "status"
			: (type == EObjectType::TIME)						   ? "time"
																   : "timer";
		meta.group_flags.set(std::to_underlying(EObjectGroup::VALUE));
		meta.group_flags.set(std::to_underlying(EObjectGroup::FEEDBACK));
		break;

	case EObjectType::ALERT:
	case EObjectType::LOG:
	case EObjectType::TOOLTIP:
	case EObjectType::POPOVER:
		meta.speech_name = (type == EObjectType::ALERT) ? "alert"
			: (type == EObjectType::LOG)				? "log"
			: (type == EObjectType::TOOLTIP)			? "tooltip"
														: "popover";
		meta.group_flags.set(std::to_underlying(EObjectGroup::FEEDBACK));
		break;
	}

	return meta;
}

using ObjectMetaArray = std::array<SObjectMeta, std::to_underlying(EObjectType::COUNT)>;

[[nodiscard]] static consteval auto InitializeMetaArray() -> ObjectMetaArray {
	ObjectMetaArray array;
	for (size_t i = 0; i < static_cast<size_t>(EObjectType::COUNT); ++i) {
		auto meta = InitializeMeta(static_cast<EObjectType>(i));
		array[i] = meta;
	}
	return array;
}

static constexpr ObjectMetaArray cObjectMetadata = InitializeMetaArray();

export [[nodiscard]] constexpr auto GetObjectTypeName(EObjectType type) -> std::string_view {
	auto index = static_cast<size_t>(type);
	if (index < 0 || index > cObjectMetadata.size()) [[unlikely]]
		return "unknown";

	return cObjectMetadata[index].speech_name;
}

export [[nodiscard]] constexpr auto IsObjectInGroup(EObjectType type, EObjectGroup group) -> bool {
	auto index = static_cast<size_t>(type);
	if (index < 0 || index > cObjectMetadata.size()) [[unlikely]]
		return false;

	return cObjectMetadata[index].group_flags.test(std::to_underlying(group));
}
