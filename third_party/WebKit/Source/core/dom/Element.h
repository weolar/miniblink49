/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003-2011, 2013, 2014 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef Element_h
#define Element_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/HTMLNames.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSSelector.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Attribute.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/Document.h"
#include "core/dom/ElementData.h"
#include "core/dom/SpaceSplitString.h"
#include "core/html/CollectionType.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebFocusType.h"

namespace blink {

class ElementAnimations;
class Attr;
class Attribute;
class CSSStyleDeclaration;
class ClientRect;
class ClientRectList;
class CustomElementDefinition;
class DOMStringMap;
class DOMTokenList;
class Dictionary;
class ElementRareData;
class ElementShadow;
class ExceptionState;
class Image;
class IntSize;
class Locale;
class MutableStylePropertySet;
class PropertySetCSSStyleDeclaration;
class PseudoElement;
class ScrollState;
class ScrollToOptions;
class ShadowRoot;
class ShadowRootInit;
class StylePropertySet;

enum SpellcheckAttributeState {
    SpellcheckAttributeTrue,
    SpellcheckAttributeFalse,
    SpellcheckAttributeDefault
};

enum ElementFlags {
    TabIndexWasSetExplicitly = 1 << 0,
    StyleAffectedByEmpty = 1 << 1,
    IsInCanvasSubtree = 1 << 2,
    ContainsFullScreenElement = 1 << 3,
    IsInTopLayer = 1 << 4,
    HasPendingResources = 1 << 5,

    NumberOfElementFlags = 6, // Required size of bitfield used to store the flags.
};

typedef WillBeHeapVector<RefPtrWillBeMember<Attr>> AttrNodeList;

class CORE_EXPORT Element : public ContainerNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Element> create(const QualifiedName&, Document*);
    ~Element() override;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(beforecopy);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(beforecut);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(beforepaste);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(copy);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(cut);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(paste);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(search);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(selectstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchcancel);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchmove);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(touchstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(wheel);

    bool hasAttribute(const QualifiedName&) const;
    const AtomicString& getAttribute(const QualifiedName&) const;

    // Passing nullAtom as the second parameter removes the attribute when calling either of these set methods.
    void setAttribute(const QualifiedName&, const AtomicString& value);
    void setSynchronizedLazyAttribute(const QualifiedName&, const AtomicString& value);

    void removeAttribute(const QualifiedName&);

    // Typed getters and setters for language bindings.
    int getIntegralAttribute(const QualifiedName& attributeName) const;
    void setIntegralAttribute(const QualifiedName& attributeName, int value);
    void setUnsignedIntegralAttribute(const QualifiedName& attributeName, unsigned value);
    double getFloatingPointAttribute(const QualifiedName& attributeName, double fallbackValue = std::numeric_limits<double>::quiet_NaN()) const;
    void setFloatingPointAttribute(const QualifiedName& attributeName, double value);

    // Call this to get the value of an attribute that is known not to be the style
    // attribute or one of the SVG animatable attributes.
    bool fastHasAttribute(const QualifiedName&) const;
    const AtomicString& fastGetAttribute(const QualifiedName&) const;
#if ENABLE(ASSERT)
    bool fastAttributeLookupAllowed(const QualifiedName&) const;
#endif

#ifdef DUMP_NODE_STATISTICS
    bool hasNamedNodeMap() const;
#endif
    bool hasAttributes() const;

    bool hasAttribute(const AtomicString& name) const;
    bool hasAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName) const;

    const AtomicString& getAttribute(const AtomicString& name) const;
    const AtomicString& getAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName) const;

    void setAttribute(const AtomicString& name, const AtomicString& value, ExceptionState&);
    static bool parseAttributeName(QualifiedName&, const AtomicString& namespaceURI, const AtomicString& qualifiedName, ExceptionState&);
    void setAttributeNS(const AtomicString& namespaceURI, const AtomicString& qualifiedName, const AtomicString& value, ExceptionState&);

    const AtomicString& getIdAttribute() const;
    void setIdAttribute(const AtomicString&);

    const AtomicString& getNameAttribute() const;
    const AtomicString& getClassAttribute() const;

    bool shouldIgnoreAttributeCase() const;

    // Call this to get the value of the id attribute for style resolution purposes.
    // The value will already be lowercased if the document is in compatibility mode,
    // so this function is not suitable for non-style uses.
    const AtomicString& idForStyleResolution() const;

    // This getter takes care of synchronizing all attributes before returning the
    // AttributeCollection. If the Element has no attributes, an empty AttributeCollection
    // will be returned. This is not a trivial getter and its return value should be cached
    // for performance.
    AttributeCollection attributes() const;
    // This variant will not update the potentially invalid attributes. To be used when not interested
    // in style attribute or one of the SVG animation attributes.
    AttributeCollection attributesWithoutUpdate() const;

    void scrollIntoView(bool alignToTop = true);
    void scrollIntoViewIfNeeded(bool centerIfNeeded = true);

    void distributeScroll(ScrollState&);
    void applyScroll(ScrollState&);

    int offsetLeft();
    int offsetTop();
    int offsetWidth();
    int offsetHeight();

    Element* offsetParent();
    int clientLeft();
    int clientTop();
    int clientWidth();
    int clientHeight();
    virtual double scrollLeft();
    virtual double scrollTop();
    virtual void setScrollLeft(double);
    virtual void setScrollTop(double);
    virtual int scrollWidth();
    virtual int scrollHeight();

    void scrollBy(double x, double y);
    virtual void scrollBy(const ScrollToOptions&);
    void scrollTo(double x, double y);
    virtual void scrollTo(const ScrollToOptions&);

    IntRect boundsInViewportSpace();

    ClientRectList* getClientRects();
    ClientRect* getBoundingClientRect();

    bool hasNonEmptyLayoutSize() const;

    const AtomicString& computedRole();
    String computedName();

    // Returns the absolute bounding box translated into screen coordinates:
    IntRect screenRect() const;

    void didMoveToNewDocument(Document&) override;

    void removeAttribute(const AtomicString& name);
    void removeAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName);

    PassRefPtrWillBeRawPtr<Attr> detachAttribute(size_t index);

    PassRefPtrWillBeRawPtr<Attr> getAttributeNode(const AtomicString& name);
    PassRefPtrWillBeRawPtr<Attr> getAttributeNodeNS(const AtomicString& namespaceURI, const AtomicString& localName);
    PassRefPtrWillBeRawPtr<Attr> setAttributeNode(Attr*, ExceptionState&);
    PassRefPtrWillBeRawPtr<Attr> setAttributeNodeNS(Attr*, ExceptionState&);
    PassRefPtrWillBeRawPtr<Attr> removeAttributeNode(Attr*, ExceptionState&);

    PassRefPtrWillBeRawPtr<Attr> attrIfExists(const QualifiedName&);
    PassRefPtrWillBeRawPtr<Attr> ensureAttr(const QualifiedName&);

    AttrNodeList* attrNodeList();

    CSSStyleDeclaration* style();

    const QualifiedName& tagQName() const { return m_tagName; }
    String tagName() const { return nodeName(); }

    bool hasTagName(const QualifiedName& tagName) const { return m_tagName.matches(tagName); }
    bool hasTagName(const HTMLQualifiedName& tagName) const { return ContainerNode::hasTagName(tagName); }
    bool hasTagName(const SVGQualifiedName& tagName) const { return ContainerNode::hasTagName(tagName); }

    // Should be called only by Document::createElementNS to fix up m_tagName immediately after construction.
    void setTagNameForCreateElementNS(const QualifiedName&);

    // A fast function for checking the local name against another atomic string.
    bool hasLocalName(const AtomicString& other) const { return m_tagName.localName() == other; }

    const AtomicString& localName() const final { return m_tagName.localName(); }
    AtomicString localNameForSelectorMatching() const;
    const AtomicString& prefix() const { return m_tagName.prefix(); }
    const AtomicString& namespaceURI() const final { return m_tagName.namespaceURI(); }

    const AtomicString& locateNamespacePrefix(const AtomicString& namespaceURI) const;

    String nodeName() const override;

    PassRefPtrWillBeRawPtr<Element> cloneElementWithChildren();
    PassRefPtrWillBeRawPtr<Element> cloneElementWithoutChildren();

    void scheduleSVGFilterLayerUpdateHack();

    void setBooleanAttribute(const QualifiedName&, bool);

    virtual const StylePropertySet* additionalPresentationAttributeStyle() { return nullptr; }
    void invalidateStyleAttribute();

    const StylePropertySet* inlineStyle() const { return elementData() ? elementData()->m_inlineStyle.get() : nullptr; }

    bool setInlineStyleProperty(CSSPropertyID, CSSValueID identifier, bool important = false);
    bool setInlineStyleProperty(CSSPropertyID, double value, CSSPrimitiveValue::UnitType, bool important = false);
    bool setInlineStyleProperty(CSSPropertyID, const String& value, bool important = false);
    bool removeInlineStyleProperty(CSSPropertyID);
    void removeAllInlineStyleProperties();

    void synchronizeStyleAttributeInternal() const;

    const StylePropertySet* presentationAttributeStyle();
    virtual bool isPresentationAttribute(const QualifiedName&) const { return false; }
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) { }

    // For exposing to DOM only.
    NamedNodeMap* attributesForBindings() const;

    enum AttributeModificationReason {
        ModifiedDirectly,
        ModifiedByCloning
    };

    // This method is called whenever an attribute is added, changed or removed.
    virtual void attributeWillChange(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue) { }
    virtual void attributeChanged(const QualifiedName&, const AtomicString&, AttributeModificationReason = ModifiedDirectly);
    virtual void parseAttribute(const QualifiedName&, const AtomicString&);

    virtual bool hasLegalLinkAttribute(const QualifiedName&) const;
    virtual const QualifiedName& subResourceAttributeName() const;

    // Only called by the parser immediately after element construction.
    void parserSetAttributes(const Vector<Attribute>&);

    // Remove attributes that might introduce scripting from the vector leaving the element unchanged.
    void stripScriptingAttributes(Vector<Attribute>&) const;

    bool sharesSameElementData(const Element& other) const { return elementData() == other.elementData(); }

    // Clones attributes only.
    void cloneAttributesFromElement(const Element&);

    // Clones all attribute-derived data, including subclass specifics (through copyNonAttributeProperties.)
    void cloneDataFromElement(const Element&);

    bool hasEquivalentAttributes(const Element* other) const;

    virtual void copyNonAttributePropertiesFromElement(const Element&) { }

    void attach(const AttachContext& = AttachContext()) override;
    void detach(const AttachContext& = AttachContext()) override;

    virtual LayoutObject* createLayoutObject(const ComputedStyle&);
    virtual bool layoutObjectIsNeeded(const ComputedStyle&);
    void recalcStyle(StyleRecalcChange, Text* nextTextSibling = nullptr);
    void pseudoStateChanged(CSSSelector::PseudoType);
    void setAnimationStyleChange(bool);
    void clearAnimationStyleChange();
    void setNeedsAnimationStyleRecalc();

    void setNeedsCompositingUpdate();

    bool supportsStyleSharing() const;

    ElementShadow* shadow() const;
    ElementShadow& ensureShadow();
    PassRefPtrWillBeRawPtr<ShadowRoot> createShadowRoot(const ScriptState*, ExceptionState&);
    PassRefPtrWillBeRawPtr<ShadowRoot> createShadowRoot(const ScriptState*, const ShadowRootInit&, ExceptionState&);
    PassRefPtrWillBeRawPtr<ShadowRoot> createShadowRoot(ExceptionState&);
    ShadowRoot* shadowRoot() const;
    ShadowRoot* youngestShadowRoot() const;

    bool hasOpenShadowRoot() const { return shadowRoot(); }
    ShadowRoot* userAgentShadowRoot() const;
    ShadowRoot& ensureUserAgentShadowRoot();
    virtual void willAddFirstAuthorShadowRoot() { }

    bool isInDescendantTreeOf(const Element* shadowHost) const;

    const ComputedStyle* ensureComputedStyle(PseudoId = NOPSEUDO);

    // Methods for indicating the style is affected by dynamic updates (e.g., children changing, our position changing in our sibling list, etc.)
    bool styleAffectedByEmpty() const { return hasElementFlag(StyleAffectedByEmpty); }
    void setStyleAffectedByEmpty() { setElementFlag(StyleAffectedByEmpty); }

    void setIsInCanvasSubtree(bool value) { setElementFlag(IsInCanvasSubtree, value); }
    bool isInCanvasSubtree() const { return hasElementFlag(IsInCanvasSubtree); }

    bool isUpgradedCustomElement() { return customElementState() == Upgraded; }
    bool isUnresolvedCustomElement() { return customElementState() == WaitingForUpgrade; }

    AtomicString computeInheritedLanguage() const;
    Locale& locale() const;

    virtual void accessKeyAction(bool /*sendToAnyEvent*/) { }

    virtual bool isURLAttribute(const Attribute&) const { return false; }
    virtual bool isHTMLContentAttribute(const Attribute&) const { return false; }
    bool isJavaScriptURLAttribute(const Attribute&) const;
    virtual bool isSVGAnimationAttributeSettingJavaScriptURL(const Attribute&) const { return false; }

    virtual bool isLiveLink() const { return false; }
    KURL hrefURL() const;

    KURL getURLAttribute(const QualifiedName&) const;
    KURL getNonEmptyURLAttribute(const QualifiedName&) const;

    virtual const AtomicString imageSourceURL() const;
    virtual Image* imageContents() { return nullptr; }

    virtual void focus(bool restorePreviousSelection = true, WebFocusType = WebFocusTypeNone);
    virtual void updateFocusAppearance(bool restorePreviousSelection);
    virtual void blur();
    // Whether this element can receive focus at all. Most elements are not
    // focusable but some elements, such as form controls and links, are. Unlike
    // layoutObjectIsFocusable(), this method may be called when layout is not up to
    // date, so it must not use the layoutObject to determine focusability.
    virtual bool supportsFocus() const;
    // Whether the node can actually be focused.
    bool isFocusable() const;
    bool isFocusedElementInDocument() const;
    virtual bool isKeyboardFocusable() const;
    virtual bool isMouseFocusable() const;
    virtual void dispatchFocusEvent(Element* oldFocusedElement, WebFocusType);
    virtual void dispatchBlurEvent(Element* newFocusedElement, WebFocusType);
    virtual void dispatchFocusInEvent(const AtomicString& eventType, Element* oldFocusedElement, WebFocusType);
    void dispatchFocusOutEvent(const AtomicString& eventType, Element* newFocusedElement);

    String innerText();
    String outerText();
    String innerHTML() const;
    String outerHTML() const;
    void setInnerHTML(const String&, ExceptionState&);
    void setOuterHTML(const String&, ExceptionState&);

    Element* insertAdjacentElement(const String& where, Element* newChild, ExceptionState&);
    void insertAdjacentText(const String& where, const String& text, ExceptionState&);
    void insertAdjacentHTML(const String& where, const String& html, ExceptionState&);

    String textFromChildren();

    virtual String title() const { return String(); }

    virtual const AtomicString& shadowPseudoId() const;
    void setShadowPseudoId(const AtomicString&);

    LayoutSize minimumSizeForResizing() const;
    void setMinimumSizeForResizing(const LayoutSize&);

    virtual void didBecomeFullscreenElement() { }
    virtual void willStopBeingFullscreenElement() { }

    // Called by the parser when this element's close tag is reached,
    // signaling that all child tags have been parsed and added.
    // This is needed for <applet> and <object> elements, which can't lay themselves out
    // until they know all of their nested <param>s. [Radar 3603191, 4040848].
    // Also used for script elements and some SVG elements for similar purposes,
    // but making parsing a special case in this respect should be avoided if possible.
    virtual void finishParsingChildren();

    void beginParsingChildren() { setIsFinishedParsingChildren(false); }

    PseudoElement* pseudoElement(PseudoId) const;
    LayoutObject* pseudoElementLayoutObject(PseudoId) const;

    virtual bool matchesReadOnlyPseudoClass() const { return false; }
    virtual bool matchesReadWritePseudoClass() const { return false; }
    virtual bool matchesValidityPseudoClasses() const { return false; }
    bool matches(const String& selectors, ExceptionState&);
    Element* closest(const String& selectors, ExceptionState&);
    virtual bool shouldAppearIndeterminate() const { return false; }

    DOMTokenList& classList();

    DOMStringMap& dataset();

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    virtual bool isDateTimeEditElement() const { return false; }
    virtual bool isDateTimeFieldElement() const { return false; }
    virtual bool isPickerIndicatorElement() const { return false; }
#endif

    virtual bool isFormControlElement() const { return false; }
    virtual bool isSpinButtonElement() const { return false; }
    virtual bool isTextFormControl() const { return false; }
    virtual bool isOptionalFormControl() const { return false; }
    virtual bool isRequiredFormControl() const { return false; }
    virtual bool isDefaultButtonForForm() const { return false; }
    virtual bool willValidate() const { return false; }
    virtual bool isValidElement() { return false; }
    virtual bool isInRange() const { return false; }
    virtual bool isOutOfRange() const { return false; }
    virtual bool isClearButtonElement() const { return false; }

    bool canContainRangeEndPoint() const override { return true; }

    // Used for disabled form elements; if true, prevents mouse events from being dispatched
    // to event listeners, and prevents DOMActivate events from being sent at all.
    virtual bool isDisabledFormControl() const { return false; }

    bool hasPendingResources() const { return hasElementFlag(HasPendingResources); }
    void setHasPendingResources() { setElementFlag(HasPendingResources); }
    void clearHasPendingResources() { clearElementFlag(HasPendingResources); }
    virtual void buildPendingResource() { }

    void setCustomElementDefinition(PassRefPtrWillBeRawPtr<CustomElementDefinition>);
    CustomElementDefinition* customElementDefinition() const;

    bool containsFullScreenElement() const { return hasElementFlag(ContainsFullScreenElement); }
    void setContainsFullScreenElement(bool);
    void setContainsFullScreenElementOnAncestorsCrossingFrameBoundaries(bool);

    bool isInTopLayer() const { return hasElementFlag(IsInTopLayer); }
    void setIsInTopLayer(bool);

    void requestPointerLock();

    bool isSpellCheckingEnabled() const;

    // FIXME: public for LayoutTreeBuilder, we shouldn't expose this though.
    PassRefPtr<ComputedStyle> styleForLayoutObject();

    bool hasID() const;
    bool hasClass() const;
    const SpaceSplitString& classNames() const;

    IntSize savedLayerScrollOffset() const;
    void setSavedLayerScrollOffset(const IntSize&);

    ElementAnimations* elementAnimations() const;
    ElementAnimations& ensureElementAnimations();
    bool hasAnimations() const;

    void synchronizeAttribute(const AtomicString& localName) const;

    MutableStylePropertySet& ensureMutableInlineStyle();
    void clearMutableInlineStyleIfEmpty();

    void setTabIndex(int);
    short tabIndex() const override;

    void incrementProxyCount();
    void decrementProxyCount();

    DECLARE_VIRTUAL_TRACE();

    SpellcheckAttributeState spellcheckAttributeState() const;

protected:
    Element(const QualifiedName& tagName, Document*, ConstructionType);

    const ElementData* elementData() const { return m_elementData.get(); }
    UniqueElementData& ensureUniqueElementData();

    void addPropertyToPresentationAttributeStyle(MutableStylePropertySet*, CSSPropertyID, CSSValueID identifier);
    void addPropertyToPresentationAttributeStyle(MutableStylePropertySet*, CSSPropertyID, double value, CSSPrimitiveValue::UnitType);
    void addPropertyToPresentationAttributeStyle(MutableStylePropertySet*, CSSPropertyID, const String& value);

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;
    void childrenChanged(const ChildrenChange&) override;

    virtual void willRecalcStyle(StyleRecalcChange);
    virtual void didRecalcStyle(StyleRecalcChange);
    virtual PassRefPtr<ComputedStyle> customStyleForLayoutObject();

    virtual bool shouldRegisterAsNamedItem() const { return false; }
    virtual bool shouldRegisterAsExtraNamedItem() const { return false; }

    bool supportsSpatialNavigationFocus() const;

    void clearTabIndexExplicitlyIfNeeded();
    void setTabIndexExplicitly(short);
    // Subclasses may override this method to affect focusability. Unlike
    // supportsFocus, this method must be called on an up-to-date layout, so it
    // may use the layoutObject to reason about focusability. This method cannot be
    // moved to LayoutObject because some focusable nodes don't have layoutObjects,
    // e.g., HTMLOptionElement.
    virtual bool layoutObjectIsFocusable() const;

    // classAttributeChanged() exists to share code between
    // parseAttribute (called via setAttribute()) and
    // svgAttributeChanged (called when element.className.baseValue is set)
    void classAttributeChanged(const AtomicString& newClassString);

    static bool attributeValueIsJavaScriptURL(const Attribute&);

    PassRefPtr<ComputedStyle> originalStyleForLayoutObject();

    Node* insertAdjacent(const String& where, Node* newChild, ExceptionState&);

    virtual void parserDidSetAttributes() { }

    void scrollLayoutBoxBy(const ScrollToOptions&);
    void scrollLayoutBoxTo(const ScrollToOptions&);
    void scrollFrameBy(const ScrollToOptions&);
    void scrollFrameTo(const ScrollToOptions&);

private:
    bool hasElementFlag(ElementFlags mask) const { return hasRareData() && hasElementFlagInternal(mask); }
    void setElementFlag(ElementFlags, bool value = true);
    void clearElementFlag(ElementFlags);
    bool hasElementFlagInternal(ElementFlags) const;

    bool isElementNode() const = delete; // This will catch anyone doing an unnecessary check.
    bool isDocumentFragment() const = delete; // This will catch anyone doing an unnecessary check.
    bool isDocumentNode() const = delete; // This will catch anyone doing an unnecessary check.

    void styleAttributeChanged(const AtomicString& newStyleString, AttributeModificationReason);

    void updatePresentationAttributeStyle();

    void inlineStyleChanged();
    PropertySetCSSStyleDeclaration* inlineStyleCSSOMWrapper();
    void setInlineStyleFromString(const AtomicString&);

    StyleRecalcChange recalcOwnStyle(StyleRecalcChange);

    inline void checkForEmptyStyleChange();

    void updatePseudoElement(PseudoId, StyleRecalcChange);
    bool updateFirstLetter(Element*);

    inline void createPseudoElementIfNeeded(PseudoId);

    // FIXME: Everyone should allow author shadows.
    virtual bool areAuthorShadowsAllowed() const { return true; }
    virtual void didAddUserAgentShadowRoot(ShadowRoot&) { }
    virtual bool alwaysCreateUserAgentShadowRoot() const { return false; }

    enum SynchronizationOfLazyAttribute { NotInSynchronizationOfLazyAttribute = 0, InSynchronizationOfLazyAttribute };

    void didAddAttribute(const QualifiedName&, const AtomicString&);
    void willModifyAttribute(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue);
    void didModifyAttribute(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue);
    void didRemoveAttribute(const QualifiedName&, const AtomicString& oldValue);

    void synchronizeAllAttributes() const;
    void synchronizeAttribute(const QualifiedName&) const;

    void updateId(const AtomicString& oldId, const AtomicString& newId);
    void updateId(TreeScope&, const AtomicString& oldId, const AtomicString& newId);
    void updateName(const AtomicString& oldName, const AtomicString& newName);

    NodeType nodeType() const final;
    bool childTypeAllowed(NodeType) const final;

    void setAttributeInternal(size_t index, const QualifiedName&, const AtomicString& value, SynchronizationOfLazyAttribute);
    void appendAttributeInternal(const QualifiedName&, const AtomicString& value, SynchronizationOfLazyAttribute);
    void removeAttributeInternal(size_t index, SynchronizationOfLazyAttribute);
    void attributeChangedFromParserOrByCloning(const QualifiedName&, const AtomicString&, AttributeModificationReason);

#ifndef NDEBUG
    void formatForDebugger(char* buffer, unsigned length) const override;
#endif

    bool pseudoStyleCacheIsInvalid(const ComputedStyle* currentStyle, ComputedStyle* newStyle);

    void cancelFocusAppearanceUpdate();

    const ComputedStyle* virtualEnsureComputedStyle(PseudoId pseudoElementSpecifier = NOPSEUDO) override { return ensureComputedStyle(pseudoElementSpecifier); }

    inline void updateCallbackSelectors(const ComputedStyle* oldStyle, const ComputedStyle* newStyle);
    inline void removeCallbackSelectors();
    inline void addCallbackSelectors();

    // cloneNode is private so that non-virtual cloneElementWithChildren and cloneElementWithoutChildren
    // are used instead.
    PassRefPtrWillBeRawPtr<Node> cloneNode(bool deep) override;
    virtual PassRefPtrWillBeRawPtr<Element> cloneElementWithoutAttributesAndChildren();

    QualifiedName m_tagName;

    void updateNamedItemRegistration(const AtomicString& oldName, const AtomicString& newName);
    void updateExtraNamedItemRegistration(const AtomicString& oldName, const AtomicString& newName);

    void createUniqueElementData();

    bool shouldInvalidateDistributionWhenAttributeChanged(ElementShadow*, const QualifiedName&, const AtomicString&);

    ElementRareData* elementRareData() const;
    ElementRareData& ensureElementRareData();

    AttrNodeList& ensureAttrNodeList();
    void removeAttrNodeList();
    void detachAllAttrNodesFromElement();
    void detachAttrNodeFromElementWithValue(Attr*, const AtomicString& value);
    void detachAttrNodeAtIndex(Attr*, size_t index);

    v8::Local<v8::Object> wrapCustomElement(v8::Isolate*, v8::Local<v8::Object> creationContext);

    RefPtrWillBeMember<ElementData> m_elementData;
};

DEFINE_NODE_TYPE_CASTS(Element, isElementNode());
template <typename T> bool isElementOfType(const Node&);
template <> inline bool isElementOfType<const Element>(const Node& node) { return node.isElementNode(); }
template <typename T> inline bool isElementOfType(const Element& element) { return isElementOfType<T>(static_cast<const Node&>(element)); }
template <> inline bool isElementOfType<const Element>(const Element&) { return true; }

// Type casting.
template<typename T> inline T& toElement(Node& node)
{
    ASSERT_WITH_SECURITY_IMPLICATION(isElementOfType<const T>(node));
    return static_cast<T&>(node);
}
template<typename T> inline T* toElement(Node* node)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!node || isElementOfType<const T>(*node));
    return static_cast<T*>(node);
}
template<typename T> inline const T& toElement(const Node& node)
{
    ASSERT_WITH_SECURITY_IMPLICATION(isElementOfType<const T>(node));
    return static_cast<const T&>(node);
}
template<typename T> inline const T* toElement(const Node* node)
{
    ASSERT_WITH_SECURITY_IMPLICATION(!node || isElementOfType<const T>(*node));
    return static_cast<const T*>(node);
}
template<typename T, typename U> inline T* toElement(const RefPtr<U>& node) { return toElement<T>(node.get()); }

inline bool isDisabledFormControl(const Node* node)
{
    return node->isElementNode() && toElement(node)->isDisabledFormControl();
}

inline Element* Node::parentElement() const
{
    ContainerNode* parent = parentNode();
    return parent && parent->isElementNode() ? toElement(parent) : nullptr;
}

inline bool Element::fastHasAttribute(const QualifiedName& name) const
{
    ASSERT(fastAttributeLookupAllowed(name));
    return elementData() && elementData()->attributes().findIndex(name) != kNotFound;
}

inline const AtomicString& Element::fastGetAttribute(const QualifiedName& name) const
{
    ASSERT(fastAttributeLookupAllowed(name));
    if (elementData()) {
        if (const Attribute* attribute = elementData()->attributes().find(name))
            return attribute->value();
    }
    return nullAtom;
}

inline AttributeCollection Element::attributes() const
{
    if (!elementData())
        return AttributeCollection();
    synchronizeAllAttributes();
    return elementData()->attributes();
}

inline AttributeCollection Element::attributesWithoutUpdate() const
{
    if (!elementData())
        return AttributeCollection();
    return elementData()->attributes();
}

inline bool Element::hasAttributes() const
{
    return !attributes().isEmpty();
}

inline const AtomicString& Element::idForStyleResolution() const
{
    ASSERT(hasID());
    return elementData()->idForStyleResolution();
}

inline const AtomicString& Element::getIdAttribute() const
{
    return hasID() ? fastGetAttribute(HTMLNames::idAttr) : nullAtom;
}

inline const AtomicString& Element::getNameAttribute() const
{
    return hasName() ? fastGetAttribute(HTMLNames::nameAttr) : nullAtom;
}

inline const AtomicString& Element::getClassAttribute() const
{
    if (!hasClass())
        return nullAtom;
    if (isSVGElement())
        return getAttribute(HTMLNames::classAttr);
    return fastGetAttribute(HTMLNames::classAttr);
}

inline void Element::setIdAttribute(const AtomicString& value)
{
    setAttribute(HTMLNames::idAttr, value);
}

inline const SpaceSplitString& Element::classNames() const
{
    ASSERT(hasClass());
    ASSERT(elementData());
    return elementData()->classNames();
}

inline bool Element::hasID() const
{
    return elementData() && elementData()->hasID();
}

inline bool Element::hasClass() const
{
    return elementData() && elementData()->hasClass();
}

inline UniqueElementData& Element::ensureUniqueElementData()
{
    if (!elementData() || !elementData()->isUnique())
        createUniqueElementData();
    return toUniqueElementData(*m_elementData);
}

inline Node::InsertionNotificationRequest Node::insertedInto(ContainerNode* insertionPoint)
{
    ASSERT(!childNeedsStyleInvalidation());
    ASSERT(!needsStyleInvalidation());
    ASSERT(insertionPoint->inDocument() || isContainerNode());
    if (insertionPoint->inDocument())
        setFlag(InDocumentFlag);
    if (parentOrShadowHostNode()->isInShadowTree())
        setFlag(IsInShadowTreeFlag);
    if (childNeedsDistributionRecalc() && !insertionPoint->childNeedsDistributionRecalc())
        insertionPoint->markAncestorsWithChildNeedsDistributionRecalc();
    return InsertionDone;
}

inline void Node::removedFrom(ContainerNode* insertionPoint)
{
    ASSERT(insertionPoint->inDocument() || isContainerNode() || isInShadowTree());
    if (insertionPoint->inDocument())
        clearFlag(InDocumentFlag);
    if (isInShadowTree() && !treeScope().rootNode().isShadowRoot())
        clearFlag(IsInShadowTreeFlag);
    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->remove(this);
}

inline void Element::invalidateStyleAttribute()
{
    ASSERT(elementData());
    elementData()->m_styleAttributeIsDirty = true;
}

inline const StylePropertySet* Element::presentationAttributeStyle()
{
    if (!elementData())
        return nullptr;
    if (elementData()->m_presentationAttributeStyleIsDirty)
        updatePresentationAttributeStyle();
    // Need to call elementData() again since updatePresentationAttributeStyle()
    // might swap it with a UniqueElementData.
    return elementData()->presentationAttributeStyle();
}

inline void Element::setTagNameForCreateElementNS(const QualifiedName& tagName)
{
    // We expect this method to be called only to reset the prefix.
    ASSERT(tagName.localName() == m_tagName.localName());
    ASSERT(tagName.namespaceURI() == m_tagName.namespaceURI());
    m_tagName = tagName;
}

inline AtomicString Element::localNameForSelectorMatching() const
{
    if (isHTMLElement() || !document().isHTMLDocument())
        return localName();
    return localName().lower();
}

inline bool isShadowHost(const Node* node)
{
    return node && node->isElementNode() && toElement(node)->shadow();
}

inline bool isShadowHost(const Node& node)
{
    return node.isElementNode() && toElement(node).shadow();
}

inline bool isShadowHost(const Element* element)
{
    return element && element->shadow();
}

inline bool isShadowHost(const Element& element)
{
    return element.shadow();
}

inline bool isAtShadowBoundary(const Element* element)
{
    if (!element)
        return false;
    ContainerNode* parentNode = element->parentNode();
    return parentNode && parentNode->isShadowRoot();
}

// These macros do the same as their NODE equivalents but additionally provide a template specialization
// for isElementOfType<>() so that the Traversal<> API works for these Element types.
#define DEFINE_ELEMENT_TYPE_CASTS(thisType, predicate) \
    template <> inline bool isElementOfType<const thisType>(const Node& node) { return node.predicate; } \
    DEFINE_NODE_TYPE_CASTS(thisType, predicate)

#define DEFINE_ELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType) \
    template <> inline bool isElementOfType<const thisType>(const Node& node) { return is##thisType(node); } \
    DEFINE_NODE_TYPE_CASTS_WITH_FUNCTION(thisType)

#define DECLARE_ELEMENT_FACTORY_WITH_TAGNAME(T) \
    static PassRefPtrWillBeRawPtr<T> create(const QualifiedName&, Document&)
#define DEFINE_ELEMENT_FACTORY_WITH_TAGNAME(T) \
    PassRefPtrWillBeRawPtr<T> T::create(const QualifiedName& tagName, Document& document) \
    { \
        return adoptRefWillBeNoop(new T(tagName, document)); \
    }

} // namespace

#endif // Element_h
