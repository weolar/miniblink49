/*
 * Copyright (C) 2008, 2009, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nuanti Ltd.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AXObject_h
#define AXObject_h

#include "core/InspectorTypeBuilder.h"
#include "core/editing/VisiblePosition.h"
#include "modules/ModulesExport.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/Color.h"
#include "wtf/Forward.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class AXObject;
class AXObjectCache;
class AXObjectCacheImpl;
class Element;
class FrameView;
class IntPoint;
class Node;
class LayoutObject;
class ScrollableArea;
class Widget;

typedef unsigned AXID;

enum AccessibilityRole {
    UnknownRole = 0,
    AlertDialogRole,
    AlertRole,
    AnnotationRole, // No mapping to ARIA role
    ApplicationRole,
    ArticleRole,
    BannerRole,
    BlockquoteRole, // No mapping to ARIA role
    BusyIndicatorRole, // No mapping to ARIA role
    ButtonRole,
    CanvasRole, // No mapping to ARIA role
    CaptionRole, // No mapping to ARIA role
    CellRole,
    CheckBoxRole,
    ColorWellRole, // No mapping to ARIA role
    ColumnHeaderRole,
    ColumnRole, // No mapping to ARIA role
    ComboBoxRole,
    ComplementaryRole,
    ContentInfoRole,
    DateRole, // No mapping to ARIA role
    DateTimeRole, // No mapping to ARIA role
    DefinitionRole,
    DescriptionListDetailRole, // No mapping to ARIA role
    DescriptionListRole, // No mapping to ARIA role
    DescriptionListTermRole, // No mapping to ARIA role
    DetailsRole, // No mapping to ARIA role
    DialogRole,
    DirectoryRole,
    DisclosureTriangleRole, // No mapping to ARIA role
    DivRole, // No mapping to ARIA role
    DocumentRole,
    EmbeddedObjectRole, // No mapping to ARIA role
    FigcaptionRole, // No mapping to ARIA role
    FigureRole, // No mapping to ARIA role
    FooterRole,
    FormRole,
    GridRole,
    GroupRole,
    HeadingRole,
    IframePresentationalRole, // No mapping to ARIA role
    IframeRole, // No mapping to ARIA role
    IgnoredRole, // No mapping to ARIA role
    ImageMapLinkRole, // No mapping to ARIA role
    ImageMapRole, // No mapping to ARIA role
    ImageRole,
    InlineTextBoxRole, // No mapping to ARIA role
    InputTimeRole, // No mapping to ARIA role
    LabelRole,
    LegendRole, // No mapping to ARIA role
    LinkRole,
    ListBoxOptionRole,
    ListBoxRole,
    ListItemRole,
    ListMarkerRole, // No mapping to ARIA role
    ListRole,
    LogRole,
    MainRole,
    MarkRole, // No mapping to ARIA role
    MarqueeRole,
    MathRole,
    MenuBarRole,
    MenuButtonRole,
    MenuItemRole,
    MenuItemCheckBoxRole,
    MenuItemRadioRole,
    MenuListOptionRole,
    MenuListPopupRole,
    MenuRole,
    MeterRole,
    NavigationRole,
    NoneRole, // No mapping to ARIA role
    NoteRole,
    OutlineRole, // No mapping to ARIA role
    ParagraphRole, // No mapping to ARIA role
    PopUpButtonRole,
    PreRole, // No mapping to ARIA role
    PresentationalRole,
    ProgressIndicatorRole,
    RadioButtonRole,
    RadioGroupRole,
    RegionRole,
    RootWebAreaRole, // No mapping to ARIA role
    RowHeaderRole,
    RowRole,
    RubyRole, // No mapping to ARIA role
    RulerRole, // No mapping to ARIA role
    SVGRootRole, // No mapping to ARIA role
    ScrollAreaRole, // No mapping to ARIA role
    ScrollBarRole,
    SeamlessWebAreaRole, // No mapping to ARIA role
    SearchRole,
    SearchBoxRole,
    SliderRole,
    SliderThumbRole, // No mapping to ARIA role
    SpinButtonPartRole, // No mapping to ARIA role
    SpinButtonRole,
    SplitterRole,
    StaticTextRole, // No mapping to ARIA role
    StatusRole,
    SwitchRole,
    TabGroupRole, // No mapping to ARIA role
    TabListRole,
    TabPanelRole,
    TabRole,
    TableHeaderContainerRole, // No mapping to ARIA role
    TableRole,
    TextFieldRole,
    TimeRole, // No mapping to ARIA role
    TimerRole,
    ToggleButtonRole,
    ToolbarRole,
    TreeGridRole,
    TreeItemRole,
    TreeRole,
    UserInterfaceTooltipRole,
    WebAreaRole, // No mapping to ARIA role
    LineBreakRole, // No mapping to ARIA role
    WindowRole, // No mapping to ARIA role
    NumRoles
};

enum AccessibilityTextSource {
    AlternativeText,
    ChildrenText,
    SummaryText,
    HelpText,
    VisibleText,
    TitleTagText,
    PlaceholderText,
    LabelByElementText,
};

enum AccessibilityState {
    AXBusyState,
    AXCheckedState,
    AXEnabledState,
    AXExpandedState,
    AXFocusableState,
    AXFocusedState,
    AXHaspopupState,
    AXHoveredState,
    AXIndeterminateState,
    AXInvisibleState,
    AXLinkedState,
    AXMultilineState,
    AXMultiselectableState,
    AXOffscreenState,
    AXPressedState,
    AXProtectedState,
    AXReadonlyState,
    AXRequiredState,
    AXSelectableState,
    AXSelectedState,
    AXVerticalState,
    AXVisitedState
};

class AccessibilityText final : public NoBaseWillBeGarbageCollectedFinalized<AccessibilityText> {
public:
    static PassOwnPtrWillBeRawPtr<AccessibilityText> create(const String& text, const AccessibilityTextSource& source)
    {
        return adoptPtrWillBeNoop(new AccessibilityText(text, source, nullptr));
    }
    static PassOwnPtrWillBeRawPtr<AccessibilityText> create(const String& text, const AccessibilityTextSource& source, const RefPtrWillBeRawPtr<AXObject> element)
    {
        return adoptPtrWillBeNoop(new AccessibilityText(text, source, nullptr));
    }

    String text() const { return m_text; }
    AccessibilityTextSource textSource() const { return m_textSource; }
    AXObject* textElement() const { return m_textElement.get(); }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_textElement);
    }

private:
    AccessibilityText(const String& text, const AccessibilityTextSource& source, const RefPtrWillBeRawPtr<AXObject> element)
    : m_text(text)
    , m_textSource(source)
    , m_textElement(element)
    { }

    String m_text;
    AccessibilityTextSource m_textSource;
    RefPtrWillBeMember<AXObject> m_textElement;
};

enum AccessibilityOrientation {
    AccessibilityOrientationUndefined = 0,
    AccessibilityOrientationVertical,
    AccessibilityOrientationHorizontal,
};

enum AXObjectInclusion {
    IncludeObject,
    IgnoreObject,
    DefaultBehavior,
};

enum AccessibilityButtonState {
    ButtonStateOff = 0,
    ButtonStateOn,
    ButtonStateMixed,
};

enum AccessibilityTextDirection {
    AccessibilityTextDirectionLTR,
    AccessibilityTextDirectionRTL,
    AccessibilityTextDirectionTTB,
    AccessibilityTextDirectionBTT
};

enum SortDirection {
    SortDirectionUndefined = 0,
    SortDirectionNone,
    SortDirectionAscending,
    SortDirectionDescending,
    SortDirectionOther
};

enum AccessibilityExpanded {
    ExpandedUndefined = 0,
    ExpandedCollapsed,
    ExpandedExpanded,
};

enum AccessibilityOptionalBool {
    OptionalBoolUndefined = 0,
    OptionalBoolTrue,
    OptionalBoolFalse
};

enum InvalidState {
    InvalidStateUndefined = 0,
    InvalidStateFalse,
    InvalidStateTrue,
    InvalidStateSpelling,
    InvalidStateGrammar,
    InvalidStateOther
};

enum TextStyle {
    TextStyleNone = 0,
    TextStyleBold = 1 << 0,
    TextStyleItalic = 1 << 1,
    TextStyleUnderline = 1 << 2,
    TextStyleLineThrough = 1 << 3
};

enum TextUnderElementMode {
    TextUnderElementAll,
    TextUnderElementAny // If the text is unimportant, just whether or not it's present
};

// The source of the accessible name of an element. This is needed
// because on some platforms this determines how the accessible name
// is exposed.
enum AXNameFrom {
    AXNameFromAttribute = 0,
    AXNameFromContents,
    AXNameFromPlaceholder,
    AXNameFromRelatedElement,
};

// The source of the accessible description of an element. This is needed
// because on some platforms this determines how the accessible description
// is exposed.
enum AXDescriptionFrom {
    AXDescriptionFromPlaceholder,
    AXDescriptionFromRelatedElement
};

enum AXIgnoredReason {
    AXActiveModalDialog,
    AXAncestorDisallowsChild,
    AXAncestorIsLeafNode,
    AXAriaHidden,
    AXAriaHiddenRoot,
    AXEmptyAlt,
    AXEmptyText,
    AXInert,
    AXInheritsPresentation,
    AXLabelContainer,
    AXLabelFor,
    AXNotRendered,
    AXNotVisible,
    AXPresentationalRole,
    AXProbablyPresentational,
    AXStaticTextUsedAsNameFor,
    AXUninteresting
};

struct IgnoredReason {
    AXIgnoredReason reason;
    const AXObject* relatedObject;

    explicit IgnoredReason(AXIgnoredReason reason)
        : reason(reason)
        , relatedObject(nullptr)
    { }

    IgnoredReason(AXIgnoredReason r, const AXObject* obj)
        : reason(r)
        , relatedObject(obj)
    { }
};

class MODULES_EXPORT AXObject : public RefCountedWillBeGarbageCollectedFinalized<AXObject> {
public:
    typedef WillBeHeapVector<RefPtrWillBeMember<AXObject>> AccessibilityChildrenVector;

    struct PlainTextRange {

        unsigned start;
        unsigned length;

        PlainTextRange()
            : start(0)
            , length(0)
        { }

        PlainTextRange(unsigned s, unsigned l)
            : start(s)
            , length(l)
        { }

        bool isNull() const { return !start && !length; }
    };

protected:
    AXObject(AXObjectCacheImpl&);

public:
    virtual ~AXObject();
    DECLARE_VIRTUAL_TRACE();

    static unsigned numberOfLiveAXObjects() { return s_numberOfLiveAXObjects; }

    // After constructing an AXObject, it must be given a
    // unique ID, then added to AXObjectCacheImpl, and finally init() must
    // be called last.
    void setAXObjectID(AXID axObjectID) { m_id = axObjectID; }
    virtual void init() { }

    // When the corresponding WebCore object that this AXObject
    // wraps is deleted, it must be detached.
    virtual void detach();
    virtual bool isDetached() const;

    // If the parent of this object is known, this can be faster than using computeParent().
    virtual void setParent(AXObject* parent) { m_parent = parent; }

    // The AXObjectCacheImpl that owns this object, and its unique ID within this cache.
    AXObjectCacheImpl& axObjectCache() const
    {
        ASSERT(m_axObjectCache);
        return *m_axObjectCache;
    }

    AXID axObjectID() const { return m_id; }

    // Determine subclass type.
    virtual bool isAXNodeObject() const { return false; }
    virtual bool isAXLayoutObject() const { return false; }
    virtual bool isAXListBox() const { return false; }
    virtual bool isAXListBoxOption() const { return false; }
    virtual bool isAXScrollbar() const { return false; }
    virtual bool isAXScrollView() const { return false; }
    virtual bool isAXSVGRoot() const { return false; }

    // Check object role or purpose.
    virtual AccessibilityRole roleValue() const { return m_role; }
    bool isARIATextControl() const;
    virtual bool isARIATreeGridRow() const { return false; }
    virtual bool isAXTable() const { return false; }
    virtual bool isAnchor() const { return false; }
    virtual bool isAttachment() const { return false; }
    bool isButton() const;
    bool isCanvas() const { return roleValue() == CanvasRole; }
    bool isCheckbox() const { return roleValue() == CheckBoxRole; }
    bool isCheckboxOrRadio() const { return isCheckbox() || isRadioButton(); }
    bool isColorWell() const { return roleValue() == ColorWellRole; }
    bool isComboBox() const { return roleValue() == ComboBoxRole; }
    virtual bool isControl() const { return false; }
    virtual bool isDataTable() const { return false; }
    virtual bool isEmbeddedObject() const { return false; }
    virtual bool isFieldset() const { return false; }
    virtual bool isHeading() const { return false; }
    virtual bool isImage() const { return false; }
    virtual bool isImageMapLink() const { return false; }
    virtual bool isInputImage() const { return false; }
    bool isLandmarkRelated() const;
    virtual bool isLink() const { return false; }
    virtual bool isList() const { return false; }
    bool isListItem() const { return roleValue() == ListItemRole; }
    virtual bool isMenu() const { return false; }
    virtual bool isMenuButton() const { return false; }
    virtual bool isMenuList() const { return false; }
    virtual bool isMenuListOption() const { return false; }
    virtual bool isMenuListPopup() const { return false; }
    bool isMenuRelated() const;
    virtual bool isMeter() const { return false; }
    virtual bool isMockObject() const { return false; }
    virtual bool isNativeSpinButton() const { return false; }
    virtual bool isNativeTextControl() const { return false; } // input or textarea
    virtual bool isNonNativeTextControl() const { return false; } // contenteditable or role=textbox
    virtual bool isPasswordField() const { return false; }
    virtual bool isPasswordFieldAndShouldHideValue() const;
    bool isPresentational() const { return roleValue() == NoneRole || roleValue() == PresentationalRole; }
    virtual bool isProgressIndicator() const { return false; }
    bool isRadioButton() const { return roleValue() == RadioButtonRole; }
    bool isScrollbar() const { return roleValue() == ScrollBarRole; }
    virtual bool isSlider() const { return false; }
    virtual bool isNativeSlider() const { return false; }
    virtual bool isSpinButton() const { return roleValue() == SpinButtonRole; }
    virtual bool isSpinButtonPart() const { return false; }
    bool isTabItem() const { return roleValue() == TabRole; }
    virtual bool isTableCell() const { return false; }
    virtual bool isTableRow() const { return false; }
    virtual bool isTextControl() const { return false; }
    virtual bool isTableCol() const { return false; }
    bool isTree() const { return roleValue() == TreeRole; }
    bool isTreeItem() const { return roleValue() == TreeItemRole; }
    bool isWebArea() const { return roleValue() == WebAreaRole; }

    // Check object state.
    virtual bool isChecked() const { return false; }
    virtual bool isClickable() const;
    virtual bool isCollapsed() const { return false; }
    virtual bool isEnabled() const { return false; }
    virtual AccessibilityExpanded isExpanded() const { return ExpandedUndefined; }
    virtual bool isFocused() const { return false; }
    virtual bool isHovered() const { return false; }
    virtual bool isIndeterminate() const { return false; }
    virtual bool isLinked() const { return false; }
    virtual bool isLoaded() const { return false; }
    virtual bool isMultiSelectable() const { return false; }
    virtual bool isOffScreen() const { return false; }
    virtual bool isPressed() const { return false; }
    virtual bool isReadOnly() const { return false; }
    virtual bool isRequired() const { return false; }
    virtual bool isSelected() const { return false; }
    virtual bool isSelectedOptionActive() const { return false; }
    virtual bool isVisible() const { return true; }
    virtual bool isVisited() const { return false; }

    // Check whether certain properties can be modified.
    virtual bool canSetFocusAttribute() const { return false; }
    virtual bool canSetValueAttribute() const { return false; }
    virtual bool canSetSelectedAttribute() const { return false; }

    // Whether objects are ignored, i.e. not included in the tree.
    bool accessibilityIsIgnored() const;
    typedef Vector<IgnoredReason> IgnoredReasons;
    virtual bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const { return true; }
    bool accessibilityIsIgnoredByDefault(IgnoredReasons* = nullptr) const;
    AXObjectInclusion accessibilityPlatformIncludesObject() const;
    virtual AXObjectInclusion defaultObjectInclusion(IgnoredReasons* = nullptr) const;
    bool isInertOrAriaHidden() const;
    const AXObject* ariaHiddenRoot() const;
    bool computeIsInertOrAriaHidden(IgnoredReasons* = nullptr) const;
    bool isDescendantOfLeafNode() const;
    AXObject* leafNodeAncestor() const;
    bool isDescendantOfDisabledNode() const;
    const AXObject* disabledAncestor() const;
    bool lastKnownIsIgnoredValue();
    void setLastKnownIsIgnoredValue(bool);
    bool hasInheritedPresentationalRole() const;
    bool isPresentationalChild() const;

    //
    // Deprecated text alternative calculation API. All of these will be replaced
    // with the new API, below (under "New text alternative calculation API".
    //

    virtual bool deprecatedExposesTitleUIElement() const { return true; }
    virtual AXObject* deprecatedTitleUIElement() const { return 0; }
    virtual String deprecatedPlaceholder() const { return String(); }
    virtual void deprecatedAriaDescribedbyElements(AccessibilityChildrenVector& describedby) const { }
    virtual void deprecatedAriaLabelledbyElements(AccessibilityChildrenVector& labelledby) const { }
    virtual String deprecatedAccessibilityDescription() const { return String(); }
    virtual String deprecatedTitle(TextUnderElementMode mode = TextUnderElementAll) const { return String(); }
    virtual String deprecatedHelpText() const { return String(); }
    virtual String deprecatedTextUnderElement(TextUnderElementMode mode = TextUnderElementAll) const { return String(); }

    //
    // New text alternative calculation API (under development).
    //

    // Retrieves the accessible name of the object, an enum indicating where the name
    // was derived from, and a list of objects that were used to derive the name, if any.
    virtual String name(AXNameFrom&, WillBeHeapVector<RawPtrWillBeMember<AXObject>>& nameObjects);

    // Takes the result of nameFrom from calling |name|, above, and retrieves the
    // accessible description of the object, which is secondary to |name|, an enum indicating
    // where the description was derived from, and a list of objects that were used to
    // derive the description, if any.
    virtual String description(AXNameFrom, AXDescriptionFrom&, WillBeHeapVector<RawPtrWillBeMember<AXObject>>& descriptionObjects) { return String(); }

    // Takes the result of nameFrom and descriptionFrom from calling |name| and |description|,
    // above, and retrieves the placeholder of the object, if present and if it wasn't already
    // exposed by one of the two functions above.
    virtual String placeholder(AXNameFrom, AXDescriptionFrom) { return String(); }

    // Internal function used by name and description, above.
    virtual String textAlternative(bool recursive, bool inAriaLabelledByTraversal, WillBeHeapHashSet<RawPtrWillBeMember<AXObject>>& visited, AXNameFrom*, WillBeHeapVector<RawPtrWillBeMember<AXObject>>* nameObjects) { return String(); }

    // Returns result of Accessible Name Calculation algorithm.
    // This is a simpler high-level interface to |name| used by Inspector.
    virtual String computedName() const { return String(); }

    //
    // Properties of static elements.
    //

    virtual const AtomicString& accessKey() const { return nullAtom; }
    virtual RGBA32 backgroundColor() const { return Color::transparent; }
    virtual RGBA32 color() const { return Color::black; }
    // Used by objects of role ColorWellRole.
    virtual RGBA32 colorValue() const { return Color::transparent; }
    virtual bool canvasHasFallbackContent() const { return false; }
    // Font size is in pixels.
    virtual float fontSize() const { return 0.0f; }
    virtual int headingLevel() const { return 0; }
    // 1-based, to match the aria-level spec.
    virtual unsigned hierarchicalLevel() const { return 0; }
    virtual AccessibilityOrientation orientation() const;
    virtual String text() const { return String(); }
    virtual AccessibilityTextDirection textDirection() const { return AccessibilityTextDirectionLTR; }
    virtual int textLength() const { return 0; }
    virtual TextStyle textStyle() const { return TextStyleNone; }
    virtual KURL url() const { return KURL(); }

    // Load inline text boxes for just this node, even if
    // settings->inlineTextBoxAccessibilityEnabled() is false.
    virtual void loadInlineTextBoxes() { }

    // Walk the AXObjects on the same line. This is supported on any
    // object type but primarily intended to be used for inline text boxes.
    virtual AXObject* nextOnLine() const { return nullptr; }
    virtual AXObject* previousOnLine() const { return nullptr; }

    // For an inline text box.
    // The integer horizontal pixel offset of each character in the string; negative values for RTL.
    virtual void textCharacterOffsets(Vector<int>&) const { }
    // The start and end character offset of each word in the inline text box.
    virtual void wordBoundaries(Vector<PlainTextRange>& words) const { }

    // Properties of interactive elements.
    virtual String actionVerb() const;
    virtual AccessibilityButtonState checkboxOrRadioValue() const;
    virtual InvalidState invalidState() const { return InvalidStateUndefined; }
    // Only used when invalidState() returns InvalidStateOther.
    virtual String ariaInvalidValue() const { return String(); }
    virtual String valueDescription() const { return String(); }
    virtual float valueForRange() const { return 0.0f; }
    virtual float maxValueForRange() const { return 0.0f; }
    virtual float minValueForRange() const { return 0.0f; }
    virtual String stringValue() const { return String(); }

    // ARIA attributes.
    virtual AXObject* activeDescendant() const { return 0; }
    virtual String ariaAutoComplete() const { return String(); }
    virtual String ariaDescribedByAttribute() const { return String(); }
    virtual void ariaFlowToElements(AccessibilityChildrenVector&) const { }
    virtual void ariaControlsElements(AccessibilityChildrenVector&) const { }
    virtual void ariaOwnsElements(AccessibilityChildrenVector& owns) const { }
    virtual bool ariaHasPopup() const { return false; }
    bool isMultiline() const;
    virtual bool isRichlyEditable() const { return false; }
    virtual String ariaLabeledByAttribute() const { return String(); }
    bool ariaPressedIsPresent() const;
    virtual AccessibilityRole ariaRoleAttribute() const { return UnknownRole; }
    virtual bool ariaRoleHasPresentationalChildren() const { return false; }
    virtual AXObject* ancestorForWhichThisIsAPresentationalChild() const { return 0; }
    virtual bool shouldFocusActiveDescendant() const { return false; }
    bool supportsARIAAttributes() const;
    virtual bool supportsARIADragging() const { return false; }
    virtual bool supportsARIADropping() const { return false; }
    virtual bool supportsARIAFlowTo() const { return false; }
    virtual bool supportsARIAOwns() const { return false; }
    bool supportsRangeValue() const;
    virtual SortDirection sortDirection() const { return SortDirectionUndefined; }

    // Returns 0-based index.
    int indexInParent() const;

    // Returns 1-based position in set.
    virtual int posInSet() const { return 0; }
    virtual int setSize() const { return 0; }
    bool supportsSetSizeAndPosInSet() const;

    // ARIA trees.
    // Used by an ARIA tree to get all its rows.
    void ariaTreeRows(AccessibilityChildrenVector&);

    // ARIA live-region features.
    bool isLiveRegion() const;
    const AXObject* liveRegionRoot() const;
    virtual const AtomicString& liveRegionStatus() const { return nullAtom; }
    virtual const AtomicString& liveRegionRelevant() const { return nullAtom; }
    virtual bool liveRegionAtomic() const { return false; }
    virtual bool liveRegionBusy() const { return false; }

    const AtomicString& containerLiveRegionStatus() const;
    const AtomicString& containerLiveRegionRelevant() const;
    bool containerLiveRegionAtomic() const;
    bool containerLiveRegionBusy() const;

    // Location and click point in frame-relative coordinates.
    virtual LayoutRect elementRect() const { return m_explicitElementRect; }
    void setElementRect(LayoutRect r) { m_explicitElementRect = r; }
    virtual void markCachedElementRectDirty() const;
    virtual IntPoint clickPoint();

    // Hit testing.
    // Called on the root AX object to return the deepest available element.
    virtual AXObject* accessibilityHitTest(const IntPoint&) const { return 0; }
    // Called on the AX object after the layout tree determines which is the right AXLayoutObject.
    virtual AXObject* elementAccessibilityHitTest(const IntPoint&) const;

    // High-level accessibility tree access. Other modules should only use these functions.
    const AccessibilityChildrenVector& children();
    AXObject* parentObject() const;
    AXObject* parentObjectIfExists() const;
    virtual AXObject* computeParent() const = 0;
    virtual AXObject* computeParentIfExists() const { return 0; }
    AXObject* cachedParentObject() const { return m_parent; }
    AXObject* parentObjectUnignored() const;

    // Low-level accessibility tree exploration, only for use within the accessibility module.
    virtual AXObject* firstChild() const { return 0; }
    virtual AXObject* nextSibling() const { return 0; }
    AXObject* firstAccessibleObjectFromNode(const Node*);
    virtual void addChildren() { }
    virtual bool canHaveChildren() const { return true; }
    bool hasChildren() const { return m_haveChildren; }
    virtual void updateChildrenIfNecessary();
    virtual bool needsToUpdateChildren() const { return false; }
    virtual void setNeedsToUpdateChildren() { }
    virtual void clearChildren();
    virtual void detachFromParent() { m_parent = 0; }
    virtual AXObject* scrollBar(AccessibilityOrientation) { return 0; }

    // Properties of the object's owning document or page.
    virtual double estimatedLoadingProgress() const { return 0; }
    AXObject* focusedUIElement() const;

    // DOM and layout tree access.
    virtual Node* node() const { return 0; }
    virtual LayoutObject* layoutObject() const { return 0; }
    virtual Document* document() const;
    virtual FrameView* documentFrameView() const;
    virtual Element* anchorElement() const { return 0; }
    virtual Element* actionElement() const { return 0; }
    virtual Widget* widgetForAttachmentView() const { return 0; }
    String language() const;
    bool hasAttribute(const QualifiedName&) const;
    const AtomicString& getAttribute(const QualifiedName&) const;

    // Selected text.
    virtual PlainTextRange selectedTextRange() const { return PlainTextRange(); }

    // Scrollable containers.
    bool isScrollableContainer() const;
    IntPoint scrollOffset() const;
    IntPoint minimumScrollOffset() const;
    IntPoint maximumScrollOffset() const;
    void setScrollOffset(const IntPoint&) const;

    // If this object itself scrolls, return its ScrollableArea.
    virtual ScrollableArea* getScrollableAreaIfScrollable() const { return 0; }

    // Modify or take an action on an object.
    virtual void increment() { }
    virtual void decrement() { }
    bool performDefaultAction() const { return press(); }
    virtual bool press() const;
    // Make this object visible by scrolling as many nested scrollable views as needed.
    void scrollToMakeVisible() const;
    // Same, but if the whole object can't be made visible, try for this subrect, in local coordinates.
    void scrollToMakeVisibleWithSubFocus(const IntRect&) const;
    // Scroll this object to a given point in global coordinates of the top-level window.
    void scrollToGlobalPoint(const IntPoint&) const;
    virtual void setFocused(bool) { }
    virtual void setSelected(bool) { }
    void setSelectedText(const String&) { }
    virtual void setSelectedTextRange(const PlainTextRange&) { }
    virtual void setValue(const String&) { }
    virtual void setValue(float) { }

    // Notifications that this object may have changed.
    virtual void childrenChanged() { }
    virtual void handleActiveDescendantChanged() { }
    virtual void handleAriaExpandedChanged() { }
    void notifyIfIgnoredValueChanged();
    virtual void selectionChanged();
    virtual void textChanged() { }
    virtual void updateAccessibilityRole() { }

    // Text metrics. Most of these should be deprecated, needs major cleanup.
    virtual VisiblePosition visiblePositionForIndex(int) const { return VisiblePosition(); }
    int lineForPosition(const VisiblePosition&) const;
    virtual int index(const VisiblePosition&) const { return -1; }
    virtual void lineBreaks(Vector<int>&) const { }

    // Static helper functions.
    static bool isARIAControl(AccessibilityRole);
    static bool isARIAInput(AccessibilityRole);
    static AccessibilityRole ariaRoleToWebCoreRole(const String&);
    static IntRect boundingBoxForQuads(LayoutObject*, const Vector<FloatQuad>&);
    static const AtomicString& roleName(AccessibilityRole);
    static const AtomicString& internalRoleName(AccessibilityRole);
    static bool isInsideFocusableElementOrARIAWidget(const Node&);

protected:
    AXID m_id;
    AccessibilityChildrenVector m_children;
    mutable bool m_haveChildren;
    AccessibilityRole m_role;
    AXObjectInclusion m_lastKnownIsIgnoredValue;
    LayoutRect m_explicitElementRect;

    virtual const AXObject* inheritsPresentationalRoleFrom() const { return 0; }

    bool nameFromContents() const;

    AccessibilityRole buttonRoleType() const;

    unsigned getLengthForTextRange() const { return text().length(); }

    bool m_detached;

    mutable RawPtrWillBeMember<AXObject> m_parent;

    // The following cached attribute values (the ones starting with m_cached*)
    // are only valid if m_lastModificationCount matches AXObjectCacheImpl::modificationCount().
    mutable int m_lastModificationCount;
    mutable bool m_cachedIsIgnored : 1;
    mutable bool m_cachedIsInertOrAriaHidden : 1;
    mutable bool m_cachedIsDescendantOfLeafNode : 1;
    mutable bool m_cachedIsDescendantOfDisabledNode : 1;
    mutable bool m_cachedHasInheritedPresentationalRole : 1;
    mutable bool m_cachedIsPresentationalChild : 1;
    mutable RawPtrWillBeMember<const AXObject> m_cachedLiveRegionRoot;

    RawPtrWillBeMember<AXObjectCacheImpl> m_axObjectCache;

    // Updates the cached attribute values. This may be recursive, so to prevent deadlocks,
    // functions called here may only search up the tree (ancestors), not down.
    void updateCachedAttributeValuesIfNeeded() const;

private:
    static bool includesARIAWidgetRole(const String&);
    static bool hasInteractiveARIAAttribute(const Element&);

    static unsigned s_numberOfLiveAXObjects;
};

#define DEFINE_AX_OBJECT_TYPE_CASTS(thisType, predicate) \
    DEFINE_TYPE_CASTS(thisType, AXObject, object, object->predicate, object.predicate)

} // namespace blink

#endif // AXObject_h
