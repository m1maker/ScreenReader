// AT-SPI object implementation and some inlines.
#include <Interfaces/Object.h>
#include <Interfaces/RefCountedObject.h>
#include <atspi/atspi.h>

#include <map>
#include <mutex>
#include <utility>
#include <Core/Singleton.h>

template<class T>
class CGlibRefCountedObject : public IRefCountedObject<T> {
public:
	explicit CGlibRefCountedObject(T* instance, bool take_ownership = true) 
	: IRefCountedObject<T>(instance) {
		if (instance && !take_ownership) {
			AddRef();
		}
	}

	~CGlibRefCountedObject() override {
		Release();
	}

	void AddRef() const override {
		if (this->operator T*()) {
			g_object_ref(this->operator T*());
		}
	}

	void Release() const override {
		if (this->operator T*()) {
			g_object_unref(this->operator T*());
		}
	}
};

[[nodiscard]] static constexpr inline auto GetObjectTypeFromAtspiRole(const AtspiRole& role) -> IObject::EObjectType {
	switch (role) {
		case ATSPI_ROLE_ACCELERATOR_LABEL:      return IObject::LABEL;
		case ATSPI_ROLE_ALERT:                  return IObject::ALERT;
		case ATSPI_ROLE_ANIMATION:              return IObject::ANIMATION;
		case ATSPI_ROLE_APPLICATION:            return IObject::APPLICATION;
		case ATSPI_ROLE_ARTICLE:                return IObject::ARTICLE;
		case ATSPI_ROLE_AUDIO:                  return IObject::AUDIO;
		case ATSPI_ROLE_AUTOCOMPLETE:           return IObject::AUTO_COMPLETE;
		case ATSPI_ROLE_BLOCK_QUOTE:            return IObject::BLOCKQUOTE;
		case ATSPI_ROLE_BUTTON:                 return IObject::BUTTON;
		case ATSPI_ROLE_CANVAS:                 return IObject::CANVAS;
		case ATSPI_ROLE_CAPTION:                return IObject::CAPTION;
		case ATSPI_ROLE_CHART:                  return IObject::CHART;
		case ATSPI_ROLE_CHECK_BOX:              return IObject::CHECKBOX;
		case ATSPI_ROLE_CHECK_MENU_ITEM:        return IObject::MENU_ITEM_CHECKBOX;
		case ATSPI_ROLE_COLOR_CHOOSER:          return IObject::COLOR_WELL;
		case ATSPI_ROLE_COLUMN_HEADER:          return IObject::COLUMN_HEADER;
		case ATSPI_ROLE_COMBO_BOX:              return IObject::COMBO_BOX;
		case ATSPI_ROLE_COMMENT:                return IObject::COMMENT;
		case ATSPI_ROLE_CONTENT_DELETION:       return IObject::CONTENT_DELETION;
		case ATSPI_ROLE_CONTENT_INSERTION:      return IObject::CONTENT_INSERTION;
		case ATSPI_ROLE_DEFINITION:             return IObject::DEFINITION;
		case ATSPI_ROLE_DESCRIPTION_LIST:       return IObject::DESCRIPTION_LIST;
		case ATSPI_ROLE_DIALOG:                 return IObject::DIALOG;
		case ATSPI_ROLE_DOCUMENT_FRAME:         return IObject::DOCUMENT;
		case ATSPI_ROLE_DOCUMENT_WEB:           return IObject::WEB_VIEW;
		case ATSPI_ROLE_EDITBAR:                return IObject::TOOLBAR;
		case ATSPI_ROLE_EMBEDDED:               return IObject::EMBEDDED_OBJECT;
		case ATSPI_ROLE_ENTRY:                  return IObject::TEXT_FIELD;
		case ATSPI_ROLE_FILE_CHOOSER:           return IObject::FILE_UPLOAD;
		case ATSPI_ROLE_FOOTER:                 return IObject::FOOTER;
		case ATSPI_ROLE_FORM:                   return IObject::FORM;
		case ATSPI_ROLE_GROUPING:               return IObject::GROUP;
		case ATSPI_ROLE_HEADER:                 return IObject::HEADER;
		case ATSPI_ROLE_HEADING:                return IObject::HEADING;
		case ATSPI_ROLE_IMAGE:                  return IObject::IMAGE;
		case ATSPI_ROLE_IMAGE_MAP:              return IObject::IMAGE_MAP;
		case ATSPI_ROLE_LABEL:                  return IObject::LABEL;
		case ATSPI_ROLE_LINK:                   return IObject::LINK;
		case ATSPI_ROLE_LIST:                   return IObject::LIST;
		case ATSPI_ROLE_LIST_BOX:               return IObject::LIST_BOX;
		case ATSPI_ROLE_LIST_ITEM:              return IObject::LIST_ITEM;
		case ATSPI_ROLE_LOG:                    return IObject::LOG;
		case ATSPI_ROLE_MARK:                   return IObject::MARK;
		case ATSPI_ROLE_MARQUEE:                return IObject::MARQUEE;
		case ATSPI_ROLE_MATH:                   return IObject::MATH;
		case ATSPI_ROLE_MATH_FRACTION:          return IObject::MATH_FRACTION;
		case ATSPI_ROLE_MATH_ROOT:              return IObject::MATH_ROOT;
		case ATSPI_ROLE_MENU:                   return IObject::MENU;
		case ATSPI_ROLE_MENU_BAR:               return IObject::MENU_BAR;
		case ATSPI_ROLE_MENU_ITEM:              return IObject::MENU_ITEM;
		case ATSPI_ROLE_NOTIFICATION:           return IObject::ALERT;
		case ATSPI_ROLE_PAGE_TAB:               return IObject::TAB;
		case ATSPI_ROLE_PAGE_TAB_LIST:          return IObject::TAB_LIST;
		case ATSPI_ROLE_PANEL:                  return IObject::PANEL;
		case ATSPI_ROLE_PARAGRAPH:              return IObject::PARAGRAPH;
		case ATSPI_ROLE_PASSWORD_TEXT:          return IObject::PASSWORD_FIELD;
		case ATSPI_ROLE_PROGRESS_BAR:           return IObject::PROGRESS_BAR;
		case ATSPI_ROLE_RADIO_BUTTON:           return IObject::RADIO_BUTTON;
		case ATSPI_ROLE_RADIO_MENU_ITEM:        return IObject::MENU_ITEM_RADIO;
		case ATSPI_ROLE_RATING:                 return IObject::RATING_INDICATOR;
		case ATSPI_ROLE_ROW_HEADER:             return IObject::ROW_HEADER;
		case ATSPI_ROLE_SCROLL_BAR:             return IObject::SCROLL_BAR;
		case ATSPI_ROLE_SECTION:                return IObject::SECTION;
		case ATSPI_ROLE_SEPARATOR:              return IObject::SEPARATOR;
		case ATSPI_ROLE_SLIDER:                 return IObject::SLIDER;
		case ATSPI_ROLE_SPIN_BUTTON:            return IObject::SPIN_BUTTON;
		case ATSPI_ROLE_SPLIT_PANE:             return IObject::SPLITTER;
		case ATSPI_ROLE_STATUS_BAR:             return IObject::STATUS;
		case ATSPI_ROLE_SUBSCRIPT:              return IObject::SUBSCRIPT;
		case ATSPI_ROLE_SUPERSCRIPT:            return IObject::SUPERSCRIPT;
		case ATSPI_ROLE_SWITCH:                 return IObject::SWITCH;
		case ATSPI_ROLE_TABLE:                  return IObject::TABLE;
		case ATSPI_ROLE_TABLE_CELL:             return IObject::CELL;
		case ATSPI_ROLE_TABLE_ROW:              return IObject::ROW;
		case ATSPI_ROLE_TERMINAL:               return IObject::TERMINAL;
		case ATSPI_ROLE_TEXT:                   return IObject::TEXT_FIELD;
		case ATSPI_ROLE_TIMER:                  return IObject::TIMER;
		case ATSPI_ROLE_TITLE_BAR:              return IObject::TITLE_BAR;
		case ATSPI_ROLE_TOGGLE_BUTTON:          return IObject::TOGGLE_BUTTON;
		case ATSPI_ROLE_TOOL_BAR:               return IObject::TOOLBAR;
		case ATSPI_ROLE_TOOL_TIP:               return IObject::TOOLTIP;
		case ATSPI_ROLE_TREE:                   return IObject::TREE;
		case ATSPI_ROLE_TREE_ITEM:              return IObject::TREE_ITEM;
		case ATSPI_ROLE_VIEWPORT:               return IObject::VIEWPORT;
		case ATSPI_ROLE_WINDOW:                 return IObject::WINDOW;
		default:                                return IObject::UNKNOWN;
	}
}

[[nodiscard]] static constexpr inline auto GetObjectStateFromAtspiState(const AtspiStateType& state) -> IObject::EObjectState {
	switch (state) {
		case ATSPI_STATE_ACTIVE:                return IObject::ACTIVE;
		case ATSPI_STATE_ANIMATED:              return IObject::ANIMATED;
		case ATSPI_STATE_BUSY:                  return IObject::BUSY;
		case ATSPI_STATE_CHECKABLE:             return IObject::CHECKABLE;
		case ATSPI_STATE_CHECKED:               return IObject::CHECKED;
		case ATSPI_STATE_COLLAPSED:             return IObject::COLLAPSED;
		case ATSPI_STATE_EDITABLE:              return IObject::EDITABLE;
		case ATSPI_STATE_ENABLED:               return IObject::ENABLED;
		case ATSPI_STATE_EXPANDABLE:            return IObject::EXPANDABLE;
		case ATSPI_STATE_EXPANDED:              return IObject::EXPANDED;
		case ATSPI_STATE_FOCUSABLE:             return IObject::FOCUSABLE;
		case ATSPI_STATE_FOCUSED:               return IObject::FOCUSED;
		case ATSPI_STATE_HAS_POPUP:             return IObject::HAS_POPUP;
		case ATSPI_STATE_HAS_TOOLTIP:           return IObject::HAS_TOOLTIP;
		case ATSPI_STATE_HORIZONTAL:            return IObject::HORIZONTAL;
		case ATSPI_STATE_INDETERMINATE:         return IObject::INDETERMINATE;
		case ATSPI_STATE_INVALID_ENTRY:         return IObject::INVALID;
		case ATSPI_STATE_IS_DEFAULT:            return IObject::DEFAULT;
		case ATSPI_STATE_MODAL:                 return IObject::MODAL;
		case ATSPI_STATE_MULTI_LINE:            return IObject::MULTI_LINE;
		case ATSPI_STATE_MULTISELECTABLE:       return IObject::MULTI_SELECTABLE;
		case ATSPI_STATE_PRESSED:               return IObject::PRESSED;
		case ATSPI_STATE_READ_ONLY:             return IObject::READONLY;
		case ATSPI_STATE_REQUIRED:              return IObject::REQUIRED;
		case ATSPI_STATE_RESIZABLE:             return IObject::RESIZABLE;
		case ATSPI_STATE_SELECTABLE:            return IObject::SELECTABLE;
		case ATSPI_STATE_SELECTED:              return IObject::SELECTED;
		case ATSPI_STATE_SENSITIVE:             return IObject::SENSITIVE;
		case ATSPI_STATE_VERTICAL:              return IObject::VERTICAL;
		case ATSPI_STATE_VISIBLE:               return IObject::VISIBLE;
		case ATSPI_STATE_VISITED:               return IObject::VISITED;
		default:                                return IObject::NO;
	}
}

[[nodiscard]] static constexpr inline auto GetObjectStateFromAtspiStates(const std::vector<AtspiStateType>& states) -> unsigned long long {
	unsigned long long result = IObject::NO;
	for (const auto& state : states) {
		result |= GetObjectStateFromAtspiState(state);
	}
	return result;
}

[[nodiscard]] static inline auto GetObjectStateFromAtspiStates(AtspiStateSet* states) -> unsigned long long {
	GArray* array = atspi_state_set_get_states(states);
	if (!array) [[unlikely]] {
		return IObject::NO;
	}

	std::vector<AtspiStateType> state_types;
	for (int i = 0; std::cmp_less(i , array->len); ++i) {
		gint64 state = g_array_index(array, gint64, i);
		state_types.push_back(static_cast<AtspiStateType>(state));
	}

	g_array_free(array, TRUE);
	return GetObjectStateFromAtspiStates(state_types);
}

[[nodiscard]] static constexpr inline auto GetAtspiTextGranularityFromTextGranularity(const ETextGranularity& granularity) -> AtspiTextGranularity {
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
		default: return ATSPI_TEXT_GRANULARITY_CHAR;
	}
}

/*
Wrapper class for GLib strings so that they are automatically freed.
*/
class CGlibString final {
private:
	gchar* m_pointer{nullptr};
public:

	constexpr explicit CGlibString(gchar* pointer) : m_pointer(pointer) {}

	CGlibString(const CGlibString&) = delete;
	auto operator=(const CGlibString&) -> CGlibString& = delete;

	CGlibString(CGlibString&& other)  noexcept : m_pointer(other.m_pointer) {
		other.m_pointer = nullptr;
	}

	~CGlibString() {
		if (m_pointer) {
			g_free(m_pointer);
		}
	}

	constexpr void reset(gchar* new_pointer = nullptr) {
		if (m_pointer) {
			g_free(m_pointer);
		}
		m_pointer = new_pointer;
	}

	auto operator=(CGlibString&& other)  noexcept -> CGlibString& {
		if (this != &other) {
			reset(other.m_pointer);
			other.m_pointer = nullptr;
		}
		return *this;
	}

	operator std::string() const {
		return m_pointer ? std::string(m_pointer) : std::string();
	}

	[[nodiscard]] inline constexpr auto c_str() const -> const char* { return m_pointer ? m_pointer : ""; }
	[[nodiscard]] constexpr inline auto empty() const -> bool { return !m_pointer || !*m_pointer; }
};

template<class T>
[[nodiscard]] static inline auto GetTextRangeFromAtspiRange(const T& range) -> STextRange {
	STextRange text_range;
	if constexpr (std::is_same_v<T, AtspiTextRange>) {
		CGlibString content(range.content);
		text_range.text = content;
	}

	text_range.start = range.start_offset;
	text_range.end = range.end_offset;
	return text_range;
}

class CObjectAtspi final : 
	public IObject,
	public ITextProvider,
	public IValueProvider {
	friend class CObjectCache<AtspiAccessible, CObjectAtspi>;	
	friend class CEventListenerAtspi;
	mutable std::shared_ptr<IObject> m_strongParentCache;

	mutable AtspiAccessible* m_accessible{nullptr};

	mutable AtspiAction* m_actionInterface{nullptr};
	mutable AtspiCollection* m_collectionInterface{nullptr};
	mutable AtspiComponent* m_componentInterface{nullptr};
	mutable AtspiDocument* m_documentInterface{nullptr};
	mutable AtspiEditableText* m_editableTextInterface{nullptr};
	mutable AtspiHypertext* m_hypertextInterface{nullptr};
	mutable AtspiImage* m_imageInterface{nullptr};
	mutable AtspiSelection* m_selectionInterface{nullptr};
	mutable AtspiTable* m_tableInterface{nullptr};
	mutable AtspiText* m_textInterface{nullptr};
	mutable AtspiValue* m_valueInterface{nullptr};

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

	mutable GArray* m_relations{nullptr};

	inline void ResetLastError() const noexcept {
		if (m_lastError) {
			g_error_free(m_lastError);
			m_lastError = nullptr;
		}
	}

	[[nodiscard]] auto GetRelations() const -> std::vector<AtspiRelation>;
public:
	explicit CObjectAtspi(AtspiAccessible* accessible) : m_accessible(accessible) {}
	~CObjectAtspi() override {
		if (m_relations) g_array_free(m_relations, TRUE);
		if (m_accessible) g_object_unref(m_accessible);
		if (m_actionInterface) g_object_unref(m_actionInterface);
		if (m_collectionInterface) g_object_unref(m_collectionInterface);
		if (m_componentInterface) g_object_unref(m_componentInterface);
		if (m_documentInterface) g_object_unref(m_documentInterface);
		if (m_editableTextInterface) g_object_unref(m_editableTextInterface);
		if (m_hypertextInterface) g_object_unref(m_hypertextInterface);
		if (m_imageInterface) g_object_unref(m_imageInterface);
		if (m_selectionInterface) g_object_unref(m_selectionInterface);
		if (m_tableInterface) g_object_unref(m_tableInterface);
		if (m_textInterface) g_object_unref(m_textInterface);
		if (m_valueInterface) g_object_unref(m_valueInterface);

		ResetLastError();
	}

	[[nodiscard]] auto GetNativeHandle() const noexcept -> ObjectResult<void*> override { return reinterpret_cast<void*>(m_accessible); }

	[[nodiscard]] inline auto IsValid() const noexcept -> bool override { return m_accessible != nullptr; }

	[[nodiscard]] auto GetType() const -> ObjectResult<EObjectType> override;
	[[nodiscard]] auto IsVisible() const -> ObjectResult<bool> override;
	[[nodiscard]] auto IsEnabled() const -> ObjectResult<bool> override;

	[[nodiscard]] auto GetState() const -> ObjectResult<unsigned long long> override;
	[[nodiscard]] auto HasState(EObjectState state) const -> ObjectResult<bool> override;

	[[nodiscard]] auto GetParent() const -> ObjectResult<std::weak_ptr<IObject>> override;
	[[nodiscard]] auto GetChildren() const -> ObjectResult<const std::vector<std::shared_ptr<IObject>>> override;
	[[nodiscard]] auto GetChildrenCount() const -> ObjectResult<int> override;


	[[nodiscard]] auto GetBounds() const -> ObjectResult<SRect> override;

	[[nodiscard]] auto GetIndex() const -> ObjectResult<int> override;

	[[nodiscard]] auto GetApplicationName() const -> ObjectResult<std::string> override;

	[[nodiscard]] auto GetName() const -> ObjectResult<std::string> override;
	[[nodiscard]] auto GetDescription() const -> ObjectResult<std::string> override;

	void UpdateCacheByEvent(const CEvent::EEventType& event) override;

	[[nodiscard]] auto GetCursor() const -> ObjectResult<int> override;
	[[nodiscard]] auto GetText(int cursor, const ETextGranularity& granularity) const -> ObjectResult<STextRange> override;
	[[nodiscard]] auto GetTextSelectionCount() const -> ObjectResult<int> override;
	[[nodiscard]] auto GetTextSelections() const -> ObjectResult<std::vector<STextRange>> override;

	[[nodiscard]] auto GetMinValue() const -> ObjectResult<double> override;
	[[nodiscard]] auto GetMaxValue() const -> ObjectResult<double> override;
	[[nodiscard]] auto GetCurrentValue() const -> ObjectResult<double> override;

private:
	mutable GError* m_lastError{nullptr};
};

