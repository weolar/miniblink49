/*
* Copyright (C) 2012, Google Inc. All rights reserved.
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

#include "config.h"
#include "modules/accessibility/AXNodeObject.h"

#include "core/InputTypeNames.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Text.h"
#include "core/html/HTMLDListElement.h"
#include "core/html/HTMLFieldSetElement.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLLabelElement.h"
#include "core/html/HTMLLegendElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLMeterElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/HTMLTableRowElement.h"
#include "core/html/HTMLTableSectionElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/MediaControlElements.h"
#include "core/layout/LayoutObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/UserGestureIndicator.h"
#include "wtf/text/StringBuilder.h"


namespace blink {

using namespace HTMLNames;

AXNodeObject::AXNodeObject(Node* node, AXObjectCacheImpl& axObjectCache)
    : AXObject(axObjectCache)
    , m_ariaRole(UnknownRole)
    , m_childrenDirty(false)
#if ENABLE(ASSERT)
    , m_initialized(false)
#endif
    , m_node(node)
{
}

PassRefPtrWillBeRawPtr<AXNodeObject> AXNodeObject::create(Node* node, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXNodeObject(node, axObjectCache));
}

AXNodeObject::~AXNodeObject()
{
    ASSERT(!m_node);
}

// This function implements the ARIA accessible name as described by the Mozilla
// ARIA Implementer's Guide.
static String accessibleNameForNode(Node* node)
{
    if (!node)
        return String();

    if (node->isTextNode())
        return toText(node)->data();

    if (isHTMLInputElement(*node))
        return toHTMLInputElement(*node).value();

    if (node->isHTMLElement()) {
        const AtomicString& alt = toHTMLElement(node)->getAttribute(altAttr);
        if (!alt.isEmpty())
            return alt;

        const AtomicString& title = toHTMLElement(node)->getAttribute(titleAttr);
        if (!title.isEmpty())
            return title;
    }

    return String();
}

String AXNodeObject::accessibilityDescriptionForElements(WillBeHeapVector<RawPtrWillBeMember<Element>> &elements) const
{
    StringBuilder builder;
    unsigned size = elements.size();
    for (unsigned i = 0; i < size; ++i) {
        Element* idElement = elements[i];

        builder.append(accessibleNameForNode(idElement));
        for (Node& n : NodeTraversal::descendantsOf(*idElement))
            builder.append(accessibleNameForNode(&n));

        if (i != size - 1)
            builder.append(' ');
    }
    return builder.toString();
}

void AXNodeObject::alterSliderValue(bool increase)
{
    if (roleValue() != SliderRole)
        return;

    float value = valueForRange();
    float step = stepValueForRange();

    value += increase ? step : -step;

    setValue(String::number(value));
    axObjectCache().postNotification(node(), AXObjectCacheImpl::AXValueChanged);
}

String AXNodeObject::ariaAccessibilityDescription() const
{
    String ariaLabeledBy = ariaLabeledByAttribute();
    if (!ariaLabeledBy.isEmpty())
        return ariaLabeledBy;

    const AtomicString& ariaLabel = getAttribute(aria_labelAttr);
    if (!ariaLabel.isEmpty())
        return ariaLabel;

    return String();
}


void AXNodeObject::ariaLabeledByElements(WillBeHeapVector<RawPtrWillBeMember<Element>>& elements) const
{
    elementsFromAttribute(elements, aria_labeledbyAttr);
    if (!elements.size())
        elementsFromAttribute(elements, aria_labelledbyAttr);
}

bool AXNodeObject::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
#if ENABLE(ASSERT)
    // Double-check that an AXObject is never accessed before
    // it's been initialized.
    ASSERT(m_initialized);
#endif

    // If this element is within a parent that cannot have children, it should not be exposed.
    if (isDescendantOfLeafNode()) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXAncestorIsLeafNode, leafNodeAncestor()));
        return true;
    }

    // Ignore labels that are already referenced by a control's title UI element.
    AXObject* controlObject = correspondingControlForLabelElement();
    if (controlObject && !controlObject->deprecatedExposesTitleUIElement() && controlObject->isCheckboxOrRadio()) {
        if (ignoredReasons) {
            HTMLLabelElement* label = labelElementContainer();
            if (label && !label->isSameNode(node())) {
                AXObject* labelAXObject = axObjectCache().getOrCreate(label);
                ignoredReasons->append(IgnoredReason(AXLabelContainer, labelAXObject));
            }

            ignoredReasons->append(IgnoredReason(AXLabelFor, controlObject));
        }
        return true;
    }

    Element* element = node()->isElementNode() ? toElement(node()) : node()->parentElement();
    if (!layoutObject()
        && (!element || !element->isInCanvasSubtree())
        && !equalIgnoringCase(getAttribute(aria_hiddenAttr), "false")) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXNotRendered));
        return true;
    }

    if (m_role == UnknownRole) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXUninteresting));
        return true;
    }
    return false;
}

static bool isListElement(Node* node)
{
    return isHTMLUListElement(*node) || isHTMLOListElement(*node) || isHTMLDListElement(*node);
}

static bool isPresentationalInTable(AXObject* parent, HTMLElement* currentElement)
{
    if (!currentElement)
        return false;

    Node* parentNode = parent->node();
    if (!parentNode || !parentNode->isHTMLElement())
        return false;

    // AXTable determines the role as checking isTableXXX.
    // If Table has explicit role including presentation, AXTable doesn't assign implicit Role
    // to a whole Table. That's why we should check it based on node.
    // Normal Table Tree is that
    // cell(its role)-> tr(tr role)-> tfoot, tbody, thead(ignored role) -> table(table role).
    // If table has presentation role, it will be like
    // cell(group)-> tr(unknown) -> tfoot, tbody, thead(ignored) -> table(presentation).
    if (isHTMLTableCellElement(*currentElement) && isHTMLTableRowElement(*parentNode))
        return parent->hasInheritedPresentationalRole();

    if (isHTMLTableRowElement(*currentElement) && isHTMLTableSectionElement(toHTMLElement(*parentNode))) {
        // Because TableSections have ignored role, presentation should be checked with its parent node
        AXObject* tableObject = parent->parentObject();
        Node* tableNode = tableObject ? tableObject->node() : 0;
        return isHTMLTableElement(tableNode) && tableObject->hasInheritedPresentationalRole();
    }
    return false;
}

static bool isRequiredOwnedElement(AXObject* parent, AccessibilityRole currentRole, HTMLElement* currentElement)
{
    Node* parentNode = parent->node();
    if (!parentNode || !parentNode->isHTMLElement())
        return false;

    if (currentRole == ListItemRole)
        return isListElement(parentNode);
    if (currentRole == ListMarkerRole)
        return isHTMLLIElement(*parentNode);
    if (currentRole == MenuItemCheckBoxRole || currentRole ==  MenuItemRole || currentRole ==  MenuItemRadioRole)
        return isHTMLMenuElement(*parentNode);

    if (!currentElement)
        return false;
    if (isHTMLTableCellElement(*currentElement))
        return isHTMLTableRowElement(*parentNode);
    if (isHTMLTableRowElement(*currentElement))
        return isHTMLTableSectionElement(toHTMLElement(*parentNode));

    // In case of ListboxRole and it's child, ListBoxOptionRole,
    // Inheritance of presentation role is handled in AXListBoxOption
    // Because ListBoxOption Role doesn't have any child.
    // If it's just ignored because of presentation, we can't see any AX tree related to ListBoxOption.
    return false;
}


const AXObject* AXNodeObject::inheritsPresentationalRoleFrom() const
{
    // ARIA states if an item can get focus, it should not be presentational.
    if (canSetFocusAttribute())
        return 0;

    if (isPresentational())
        return this;

    // http://www.w3.org/TR/wai-aria/complete#presentation
    // ARIA spec says that the user agent MUST apply an inherited role of presentation
    // to any owned elements that do not have an explicit role defined.
    if (ariaRoleAttribute() != UnknownRole)
        return 0;

    AXObject* parent = parentObject();
    if (!parent)
        return 0;

    HTMLElement* element = nullptr;
    if (node() && node()->isHTMLElement())
        element = toHTMLElement(node());
    if (!parent->hasInheritedPresentationalRole()) {
        if (!layoutObject() || !layoutObject()->isBoxModelObject())
            return 0;

        LayoutBoxModelObject* cssBox = toLayoutBoxModelObject(layoutObject());
        if (!cssBox->isTableCell() && !cssBox->isTableRow())
            return 0;

        if (!isPresentationalInTable(parent, element))
            return 0;
    }
    // ARIA spec says that when a parent object is presentational and this object
    // is a required owned element of that parent, then this object is also presentational.
    if (isRequiredOwnedElement(parent, roleValue(), element))
        return parent;
    return 0;
}

bool AXNodeObject::isDescendantOfElementType(const HTMLQualifiedName& tagName) const
{
    if (!node())
        return false;

    for (Element* parent = node()->parentElement(); parent; parent = parent->parentElement()) {
        if (parent->hasTagName(tagName))
            return true;
    }
    return false;
}

AccessibilityRole AXNodeObject::determineAccessibilityRoleUtil()
{
    if (!node())
        return UnknownRole;
    // HTMLAnchorElement sets isLink only when it has hrefAttr.
    // We assume that it is also LinkRole if it has event listners even though it doesn't have hrefAttr.
    if (node()->isLink() || (isHTMLAnchorElement(*node()) && isClickable()))
        return LinkRole;

    if (isHTMLButtonElement(*node()))
        return buttonRoleType();

    if (isHTMLDetailsElement(*node()))
        return DetailsRole;

    if (isHTMLSummaryElement(*node())) {
        if (node()->parentNode() && isHTMLDetailsElement(node()->parentNode()))
            return DisclosureTriangleRole;
        return UnknownRole;
    }

    if (isHTMLInputElement(*node())) {
        HTMLInputElement& input = toHTMLInputElement(*node());
        const AtomicString& type = input.type();
        if (input.dataList())
            return ComboBoxRole;
        if (type == InputTypeNames::button) {
            if ((node()->parentNode() && isHTMLMenuElement(node()->parentNode())) || (parentObject() && parentObject()->roleValue() == MenuRole))
                return MenuItemRole;
            return buttonRoleType();
        }
        if (type == InputTypeNames::checkbox) {
            if ((node()->parentNode() && isHTMLMenuElement(node()->parentNode())) || (parentObject() && parentObject()->roleValue() == MenuRole))
                return MenuItemCheckBoxRole;
            return CheckBoxRole;
        }
        if (type == InputTypeNames::date)
            return DateRole;
        if (type == InputTypeNames::datetime
            || type == InputTypeNames::datetime_local
            || type == InputTypeNames::month
            || type == InputTypeNames::week)
            return DateTimeRole;
        if (type == InputTypeNames::file)
            return ButtonRole;
        if (type == InputTypeNames::radio) {
            if ((node()->parentNode() && isHTMLMenuElement(node()->parentNode())) || (parentObject() && parentObject()->roleValue() == MenuRole))
                return MenuItemRadioRole;
            return RadioButtonRole;
        }
        if (type == InputTypeNames::number)
            return SpinButtonRole;
        if (input.isTextButton())
            return buttonRoleType();
        if (type == InputTypeNames::range)
            return SliderRole;
        if (type == InputTypeNames::color)
            return ColorWellRole;
        if (type == InputTypeNames::time)
            return InputTimeRole;
        return TextFieldRole;
    }

    if (isHTMLSelectElement(*node())) {
        HTMLSelectElement& selectElement = toHTMLSelectElement(*node());
        return selectElement.multiple() ? ListBoxRole : PopUpButtonRole;
    }

    if (isHTMLTextAreaElement(*node()))
        return TextFieldRole;

    if (headingLevel())
        return HeadingRole;

    if (isHTMLDivElement(*node()))
        return DivRole;

    if (isHTMLMeterElement(*node()))
        return MeterRole;

    if (isHTMLOutputElement(*node()))
        return StatusRole;

    if (isHTMLParagraphElement(*node()))
        return ParagraphRole;

    if (isHTMLLabelElement(*node()))
        return LabelRole;

    if (isHTMLLegendElement(*node()))
        return LegendRole;

    if (isHTMLRubyElement(*node()))
        return RubyRole;

    if (isHTMLDListElement(*node()))
        return DescriptionListRole;

    if (node()->hasTagName(ddTag))
        return DescriptionListDetailRole;

    if (node()->hasTagName(dtTag))
        return DescriptionListTermRole;

    if (node()->nodeName() == "math")
        return MathRole;

    if (node()->hasTagName(rpTag) || node()->hasTagName(rtTag))
        return AnnotationRole;

    if (isHTMLFormElement(*node()))
        return FormRole;

    if (node()->hasTagName(articleTag))
        return ArticleRole;

    if (node()->hasTagName(mainTag))
        return MainRole;

    if (node()->hasTagName(markTag))
        return MarkRole;

    if (node()->hasTagName(navTag))
        return NavigationRole;

    if (node()->hasTagName(asideTag))
        return ComplementaryRole;

    if (node()->hasTagName(preTag))
        return PreRole;

    if (node()->hasTagName(sectionTag))
        return RegionRole;

    if (node()->hasTagName(addressTag))
        return ContentInfoRole;

    if (isHTMLDialogElement(*node()))
        return DialogRole;

    // The HTML element should not be exposed as an element. That's what the LayoutView element does.
    if (isHTMLHtmlElement(*node()))
        return IgnoredRole;

    if (isHTMLIFrameElement(*node())) {
        const AtomicString& ariaRole = getAttribute(roleAttr);
        if (ariaRole == "none" || ariaRole == "presentation")
            return IframePresentationalRole;
        return IframeRole;
    }

    // There should only be one banner/contentInfo per page. If header/footer are being used within an article or section
    // then it should not be exposed as whole page's banner/contentInfo
    if (node()->hasTagName(headerTag) && !isDescendantOfElementType(articleTag) && !isDescendantOfElementType(sectionTag))
        return BannerRole;

    if (node()->hasTagName(footerTag) && !isDescendantOfElementType(articleTag) && !isDescendantOfElementType(sectionTag))
        return FooterRole;

    if (node()->hasTagName(blockquoteTag))
        return BlockquoteRole;

    if (node()->hasTagName(captionTag))
        return CaptionRole;

    if (node()->hasTagName(figcaptionTag))
        return FigcaptionRole;

    if (node()->hasTagName(figureTag))
        return FigureRole;

    if (node()->nodeName() == "TIME")
        return TimeRole;

    if (isEmbeddedObject())
        return EmbeddedObjectRole;

    return UnknownRole;
}

AccessibilityRole AXNodeObject::determineAccessibilityRole()
{
    if (!node())
        return UnknownRole;

    if ((m_ariaRole = determineAriaRoleAttribute()) != UnknownRole)
        return m_ariaRole;
    if (node()->isTextNode())
        return StaticTextRole;

    AccessibilityRole role = determineAccessibilityRoleUtil();
    if (role != UnknownRole)
        return role;
    if (node()->isElementNode()) {
        Element* element = toElement(node());
        if (element->isInCanvasSubtree() && element->isFocusable())
            return GroupRole;
    }
    return UnknownRole;
}

AccessibilityRole AXNodeObject::determineAriaRoleAttribute() const
{
    const AtomicString& ariaRole = getAttribute(roleAttr);
    if (ariaRole.isNull() || ariaRole.isEmpty())
        return UnknownRole;

    AccessibilityRole role = ariaRoleToWebCoreRole(ariaRole);

    // ARIA states if an item can get focus, it should not be presentational.
    if ((role == NoneRole || role == PresentationalRole) && canSetFocusAttribute())
        return UnknownRole;

    if (role == ButtonRole)
        role = buttonRoleType();

    role = remapAriaRoleDueToParent(role);

    if (role)
        return role;

    return UnknownRole;
}

void AXNodeObject::tokenVectorFromAttribute(Vector<String>& tokens, const QualifiedName& attribute) const
{
    Node* node = this->node();
    if (!node || !node->isElementNode())
        return;

    String attributeValue = getAttribute(attribute).string();
    if (attributeValue.isEmpty())
        return;

    attributeValue.simplifyWhiteSpace();
    attributeValue.split(' ', tokens);
}

void AXNodeObject::elementsFromAttribute(WillBeHeapVector<RawPtrWillBeMember<Element>>& elements, const QualifiedName& attribute) const
{
    Vector<String> ids;
    tokenVectorFromAttribute(ids, attribute);
    if (ids.isEmpty())
        return;

    TreeScope& scope = node()->treeScope();
    for (const auto& id : ids) {
        if (Element* idElement = scope.getElementById(AtomicString(id)))
            elements.append(idElement);
    }
}

void AXNodeObject::accessibilityChildrenFromAttribute(QualifiedName attr, AccessibilityChildrenVector& children) const
{
    WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
    elementsFromAttribute(elements, attr);

    AXObjectCacheImpl& cache = axObjectCache();
    for (const auto& element : elements) {
        if (AXObject* child = cache.getOrCreate(element))
            children.append(child);
    }
}

// This only returns true if this is the element that actually has the
// contentEditable attribute set, unlike node->hasEditableStyle() which will
// also return true if an ancestor is editable.
bool AXNodeObject::hasContentEditableAttributeSet() const
{
    if (!hasAttribute(contenteditableAttr))
        return false;
    const AtomicString& contentEditableValue = getAttribute(contenteditableAttr);
    // Both "true" (case-insensitive) and the empty string count as true.
    return contentEditableValue.isEmpty() || equalIgnoringCase(contentEditableValue, "true");
}

bool AXNodeObject::isTextControl() const
{
    if (hasContentEditableAttributeSet())
        return true;

    switch (roleValue()) {
    case TextFieldRole:
    case ComboBoxRole:
    case SearchBoxRole:
        return true;
    default:
        return false;
    }
}

bool AXNodeObject::isGenericFocusableElement() const
{
    if (!canSetFocusAttribute())
        return false;

    // If it's a control, it's not generic.
    if (isControl())
        return false;

    // If it has an aria role, it's not generic.
    if (m_ariaRole != UnknownRole)
        return false;

    // If the content editable attribute is set on this element, that's the reason
    // it's focusable, and existing logic should handle this case already - so it's not a
    // generic focusable element.

    if (hasContentEditableAttributeSet())
        return false;

    // The web area and body element are both focusable, but existing logic handles these
    // cases already, so we don't need to include them here.
    if (roleValue() == WebAreaRole)
        return false;
    if (isHTMLBodyElement(node()))
        return false;

    // An SVG root is focusable by default, but it's probably not interactive, so don't
    // include it. It can still be made accessible by giving it an ARIA role.
    if (roleValue() == SVGRootRole)
        return false;

    return true;
}

HTMLLabelElement* AXNodeObject::labelForElement(Element* element) const
{
    if (!element->isHTMLElement() || !toHTMLElement(element)->isLabelable())
        return 0;

    const AtomicString& id = element->getIdAttribute();
    if (!id.isEmpty()) {
        if (HTMLLabelElement* label = element->treeScope().labelElementForId(id))
            return label;
    }

    return Traversal<HTMLLabelElement>::firstAncestor(*element);
}

AXObject* AXNodeObject::menuButtonForMenu() const
{
    Element* menuItem = menuItemElementForMenu();

    if (menuItem) {
        // ARIA just has generic menu items. AppKit needs to know if this is a top level items like MenuBarButton or MenuBarItem
        AXObject* menuItemAX = axObjectCache().getOrCreate(menuItem);
        if (menuItemAX && menuItemAX->isMenuButton())
            return menuItemAX;
    }
    return 0;
}

static Element* siblingWithAriaRole(String role, Node* node)
{
    Node* parent = node->parentNode();
    if (!parent)
        return 0;

    for (Element* sibling = ElementTraversal::firstChild(*parent); sibling; sibling = ElementTraversal::nextSibling(*sibling)) {
        const AtomicString& siblingAriaRole = sibling->getAttribute(roleAttr);
        if (equalIgnoringCase(siblingAriaRole, role))
            return sibling;
    }

    return 0;
}

Element* AXNodeObject::menuItemElementForMenu() const
{
    if (ariaRoleAttribute() != MenuRole)
        return 0;

    return siblingWithAriaRole("menuitem", node());
}

Element* AXNodeObject::mouseButtonListener() const
{
    Node* node = this->node();
    if (!node)
        return 0;

    // check if our parent is a mouse button listener
    if (!node->isElementNode())
        node = node->parentElement();

    if (!node)
        return 0;

    // FIXME: Do the continuation search like anchorElement does
    for (Element* element = toElement(node); element; element = element->parentElement()) {
        if (element->getAttributeEventListener(EventTypeNames::click) || element->getAttributeEventListener(EventTypeNames::mousedown) || element->getAttributeEventListener(EventTypeNames::mouseup))
            return element;
    }

    return 0;
}

AccessibilityRole AXNodeObject::remapAriaRoleDueToParent(AccessibilityRole role) const
{
    // Some objects change their role based on their parent.
    // However, asking for the unignoredParent calls accessibilityIsIgnored(), which can trigger a loop.
    // While inside the call stack of creating an element, we need to avoid accessibilityIsIgnored().
    // https://bugs.webkit.org/show_bug.cgi?id=65174

    if (role != ListBoxOptionRole && role != MenuItemRole)
        return role;

    for (AXObject* parent = parentObject(); parent && !parent->accessibilityIsIgnored(); parent = parent->parentObject()) {
        AccessibilityRole parentAriaRole = parent->ariaRoleAttribute();

        // Selects and listboxes both have options as child roles, but they map to different roles within WebCore.
        if (role == ListBoxOptionRole && parentAriaRole == MenuRole)
            return MenuItemRole;
        // An aria "menuitem" may map to MenuButton or MenuItem depending on its parent.
        if (role == MenuItemRole && parentAriaRole == GroupRole)
            return MenuButtonRole;

        // If the parent had a different role, then we don't need to continue searching up the chain.
        if (parentAriaRole)
            break;
    }

    return role;
}

void AXNodeObject::init()
{
#if ENABLE(ASSERT)
    ASSERT(!m_initialized);
    m_initialized = true;
#endif
    m_role = determineAccessibilityRole();
}

void AXNodeObject::detach()
{
    AXObject::detach();
    m_node = nullptr;
}

bool AXNodeObject::isAnchor() const
{
    return !isNativeImage() && isLink();
}

bool AXNodeObject::isControl() const
{
    Node* node = this->node();
    if (!node)
        return false;

    return ((node->isElementNode() && toElement(node)->isFormControlElement())
        || AXObject::isARIAControl(ariaRoleAttribute()));
}

bool AXNodeObject::isControllingVideoElement() const
{
    Node* node = this->node();
    if (!node)
        return true;

    return isHTMLVideoElement(toParentMediaElement(node));
}

bool AXNodeObject::isEmbeddedObject() const
{
    return isHTMLPlugInElement(node());
}

bool AXNodeObject::isFieldset() const
{
    return isHTMLFieldSetElement(node());
}

bool AXNodeObject::isHeading() const
{
    return roleValue() == HeadingRole;
}

bool AXNodeObject::isHovered() const
{
    Node* node = this->node();
    if (!node)
        return false;

    return node->hovered();
}

bool AXNodeObject::isImage() const
{
    return roleValue() == ImageRole;
}

bool AXNodeObject::isImageButton() const
{
    return isNativeImage() && isButton();
}

bool AXNodeObject::isInputImage() const
{
    Node* node = this->node();
    if (roleValue() == ButtonRole && isHTMLInputElement(node))
        return toHTMLInputElement(*node).type() == InputTypeNames::image;

    return false;
}

bool AXNodeObject::isLink() const
{
    return roleValue() == LinkRole;
}

bool AXNodeObject::isMenu() const
{
    return roleValue() == MenuRole;
}

bool AXNodeObject::isMenuButton() const
{
    return roleValue() == MenuButtonRole;
}

bool AXNodeObject::isMeter() const
{
    return roleValue() == MeterRole;
}

bool AXNodeObject::isMultiSelectable() const
{
    const AtomicString& ariaMultiSelectable = getAttribute(aria_multiselectableAttr);
    if (equalIgnoringCase(ariaMultiSelectable, "true"))
        return true;
    if (equalIgnoringCase(ariaMultiSelectable, "false"))
        return false;

    return isHTMLSelectElement(node()) && toHTMLSelectElement(*node()).multiple();
}

bool AXNodeObject::isNativeCheckboxOrRadio() const
{
    Node* node = this->node();
    if (!isHTMLInputElement(node))
        return false;

    HTMLInputElement* input = toHTMLInputElement(node);
    return input->type() == InputTypeNames::checkbox || input->type() == InputTypeNames::radio;
}

bool AXNodeObject::isNativeImage() const
{
    Node* node = this->node();
    if (!node)
        return false;

    if (isHTMLImageElement(*node))
        return true;

    if (isHTMLPlugInElement(*node))
        return true;

    if (isHTMLInputElement(*node))
        return toHTMLInputElement(*node).type() == InputTypeNames::image;

    return false;
}

bool AXNodeObject::isNativeTextControl() const
{
    Node* node = this->node();
    if (!node)
        return false;

    if (isHTMLTextAreaElement(*node))
        return true;

    if (isHTMLInputElement(*node))
        return toHTMLInputElement(node)->isTextField();

    return false;
}

bool AXNodeObject::isNonNativeTextControl() const
{
    if (isNativeTextControl())
        return false;

    if (hasContentEditableAttributeSet())
        return true;

    if (isARIATextControl())
        return true;

    return false;
}

bool AXNodeObject::isPasswordField() const
{
    Node* node = this->node();
    if (!isHTMLInputElement(node))
        return false;

    AccessibilityRole ariaRole = ariaRoleAttribute();
    if (ariaRole != TextFieldRole && ariaRole != UnknownRole)
        return false;

    return toHTMLInputElement(node)->type() == InputTypeNames::password;
}

bool AXNodeObject::isProgressIndicator() const
{
    return roleValue() == ProgressIndicatorRole;
}

bool AXNodeObject::isSlider() const
{
    return roleValue() == SliderRole;
}

bool AXNodeObject::isNativeSlider() const
{
    Node* node = this->node();
    if (!node)
        return false;

    if (!isHTMLInputElement(node))
        return false;

    return toHTMLInputElement(node)->type() == InputTypeNames::range;
}

bool AXNodeObject::isChecked() const
{
    Node* node = this->node();
    if (!node)
        return false;

    // First test for native checkedness semantics
    if (isHTMLInputElement(*node))
        return toHTMLInputElement(*node).shouldAppearChecked();

    // Else, if this is an ARIA role checkbox or radio or menuitemcheckbox
    // or menuitemradio or switch, respect the aria-checked attribute
    switch (ariaRoleAttribute()) {
    case CheckBoxRole:
    case MenuItemCheckBoxRole:
    case MenuItemRadioRole:
    case RadioButtonRole:
    case SwitchRole:
        if (equalIgnoringCase(getAttribute(aria_checkedAttr), "true"))
            return true;
        return false;
    default:
        break;
    }

    // Otherwise it's not checked
    return false;
}

bool AXNodeObject::isClickable() const
{
    if (node()) {
        if (node()->isElementNode() && toElement(node())->isDisabledFormControl())
            return false;

        // Note: we can't call node()->willRespondToMouseClickEvents() because that triggers a style recalc and can delete this.
        if (node()->hasEventListeners(EventTypeNames::mouseup) || node()->hasEventListeners(EventTypeNames::mousedown) || node()->hasEventListeners(EventTypeNames::click) || node()->hasEventListeners(EventTypeNames::DOMActivate))
            return true;
    }

    return AXObject::isClickable();
}

bool AXNodeObject::isEnabled() const
{
    if (isDescendantOfDisabledNode())
        return false;

    Node* node = this->node();
    if (!node || !node->isElementNode())
        return true;

    return !toElement(node)->isDisabledFormControl();
}

AccessibilityExpanded AXNodeObject::isExpanded() const
{
    if (node() && isHTMLSummaryElement(*node())) {
        if (node()->parentNode() && isHTMLDetailsElement(node()->parentNode()))
            return toElement(node()->parentNode())->hasAttribute(openAttr) ? ExpandedExpanded : ExpandedCollapsed;
    }

    const AtomicString& expanded = getAttribute(aria_expandedAttr);
    if (equalIgnoringCase(expanded, "true"))
        return ExpandedExpanded;
    if (equalIgnoringCase(expanded, "false"))
        return ExpandedCollapsed;

    return ExpandedUndefined;
}

bool AXNodeObject::isIndeterminate() const
{
    Node* node = this->node();
    if (!isHTMLInputElement(node))
        return false;

    return toHTMLInputElement(node)->shouldAppearIndeterminate();
}

bool AXNodeObject::isPressed() const
{
    if (!isButton())
        return false;

    Node* node = this->node();
    if (!node)
        return false;

    // ARIA button with aria-pressed not undefined, then check for aria-pressed attribute rather than node()->active()
    if (ariaRoleAttribute() == ToggleButtonRole) {
        if (equalIgnoringCase(getAttribute(aria_pressedAttr), "true")
            || equalIgnoringCase(getAttribute(aria_pressedAttr), "mixed"))
            return true;
        return false;
    }

    return node->active();
}

bool AXNodeObject::isReadOnly() const
{
    Node* node = this->node();
    if (!node)
        return true;

    if (isHTMLTextAreaElement(*node))
        return toHTMLTextAreaElement(*node).isReadOnly();

    if (isHTMLInputElement(*node)) {
        HTMLInputElement& input = toHTMLInputElement(*node);
        if (input.isTextField())
            return input.isReadOnly();
    }

    return !node->hasEditableStyle();
}

bool AXNodeObject::isRequired() const
{
    Node* n = this->node();
    if (n && (n->isElementNode() && toElement(n)->isFormControlElement()) && hasAttribute(requiredAttr))
        return toHTMLFormControlElement(n)->isRequired();

    if (equalIgnoringCase(getAttribute(aria_requiredAttr), "true"))
        return true;

    return false;
}

bool AXNodeObject::canSetFocusAttribute() const
{
    Node* node = this->node();
    if (!node)
        return false;

    if (isWebArea())
        return true;

    // NOTE: It would be more accurate to ask the document whether setFocusedNode() would
    // do anything. For example, setFocusedNode() will do nothing if the current focused
    // node will not relinquish the focus.
    if (!node)
        return false;

    if (isDisabledFormControl(node))
        return false;

    return node->isElementNode() && toElement(node)->supportsFocus();
}

bool AXNodeObject::canSetValueAttribute() const
{
    if (equalIgnoringCase(getAttribute(aria_readonlyAttr), "true"))
        return false;

    if (isProgressIndicator() || isSlider())
        return true;

    if (isTextControl() && !isNativeTextControl())
        return true;

    // Any node could be contenteditable, so isReadOnly should be relied upon
    // for this information for all elements.
    return !isReadOnly();
}

bool AXNodeObject::canvasHasFallbackContent() const
{
    Node* node = this->node();
    if (!isHTMLCanvasElement(node))
        return false;

    // If it has any children that are elements, we'll assume it might be fallback
    // content. If it has no children or its only children are not elements
    // (e.g. just text nodes), it doesn't have fallback content.
    return ElementTraversal::firstChild(*node);
}

bool AXNodeObject::deprecatedExposesTitleUIElement() const
{
    if (!isControl())
        return false;

    // If this control is ignored (because it's invisible),
    // then the label needs to be exposed so it can be visible to accessibility.
    if (accessibilityIsIgnored())
        return true;

    // ARIA: section 2A, bullet #3 says if aria-labeledby or aria-label appears, it should
    // override the "label" element association.
    bool hasTextAlternative = (!ariaLabeledByAttribute().isEmpty() || !getAttribute(aria_labelAttr).isEmpty());

    // Checkboxes and radio buttons use the text of their title ui element as their own AXTitle.
    // This code controls whether the title ui element should appear in the AX tree (usually, no).
    // It should appear if the control already has a label (which will be used as the AXTitle instead).
    if (isCheckboxOrRadio())
        return hasTextAlternative;

    // When controls have their own descriptions, the title element should be ignored.
    if (hasTextAlternative)
        return false;

    return true;
}

int AXNodeObject::headingLevel() const
{
    // headings can be in block flow and non-block flow
    Node* node = this->node();
    if (!node)
        return 0;

    if (roleValue() == HeadingRole && hasAttribute(aria_levelAttr)) {
        int level = getAttribute(aria_levelAttr).toInt();
        if (level >= 1 && level <= 9)
            return level;
    }

    if (!node->isHTMLElement())
        return 0;

    HTMLElement& element = toHTMLElement(*node);
    if (element.hasTagName(h1Tag))
        return 1;

    if (element.hasTagName(h2Tag))
        return 2;

    if (element.hasTagName(h3Tag))
        return 3;

    if (element.hasTagName(h4Tag))
        return 4;

    if (element.hasTagName(h5Tag))
        return 5;

    if (element.hasTagName(h6Tag))
        return 6;

    return 0;
}

unsigned AXNodeObject::hierarchicalLevel() const
{
    Node* node = this->node();
    if (!node || !node->isElementNode())
        return 0;
    Element* element = toElement(node);
    String ariaLevel = element->getAttribute(aria_levelAttr);
    if (!ariaLevel.isEmpty())
        return ariaLevel.toInt();

    // Only tree item will calculate its level through the DOM currently.
    if (roleValue() != TreeItemRole)
        return 0;

    // Hierarchy leveling starts at 1, to match the aria-level spec.
    // We measure tree hierarchy by the number of groups that the item is within.
    unsigned level = 1;
    for (AXObject* parent = parentObject(); parent; parent = parent->parentObject()) {
        AccessibilityRole parentRole = parent->roleValue();
        if (parentRole == GroupRole)
            level++;
        else if (parentRole == TreeRole)
            break;
    }

    return level;
}

String AXNodeObject::ariaAutoComplete() const
{
    if (roleValue() != ComboBoxRole)
        return String();

    const AtomicString& ariaAutoComplete = getAttribute(aria_autocompleteAttr).lower();

    if (ariaAutoComplete == "inline" || ariaAutoComplete == "list"
        || ariaAutoComplete == "both")
        return ariaAutoComplete;

    return String();
}

String AXNodeObject::deprecatedPlaceholder() const
{
    String placeholder;
    if (node()) {
        if (isHTMLInputElement(*node())) {
            HTMLInputElement* inputElement = toHTMLInputElement(node());
            placeholder = inputElement->strippedPlaceholder();
        } else if (isHTMLTextAreaElement(*node())) {
            HTMLTextAreaElement* textAreaElement = toHTMLTextAreaElement(node());
            placeholder = textAreaElement->strippedPlaceholder();
        }
    }
    return placeholder;
}

String AXNodeObject::text() const
{
    // If this is a user defined static text, use the accessible name computation.
    if (ariaRoleAttribute() == StaticTextRole)
        return ariaAccessibilityDescription();

    if (!isTextControl())
        return String();

    Node* node = this->node();
    if (!node)
        return String();

    if (isNativeTextControl() && (isHTMLTextAreaElement(*node) || isHTMLInputElement(*node)))
        return toHTMLTextFormControlElement(*node).value();

    if (!node->isElementNode())
        return String();

    return toElement(node)->innerText();
}

AXObject* AXNodeObject::deprecatedTitleUIElement() const
{
    if (!node() || !node()->isElementNode())
        return 0;

    if (isFieldset())
        return axObjectCache().getOrCreate(toHTMLFieldSetElement(node())->legend());

    HTMLLabelElement* label = labelForElement(toElement(node()));
    if (label)
        return axObjectCache().getOrCreate(label);

    return 0;
}

AccessibilityButtonState AXNodeObject::checkboxOrRadioValue() const
{
    if (isNativeCheckboxOrRadio())
        return isChecked() ? ButtonStateOn : ButtonStateOff;

    return AXObject::checkboxOrRadioValue();
}

RGBA32 AXNodeObject::colorValue() const
{
    if (!isHTMLInputElement(node()) || !isColorWell())
        return AXObject::colorValue();

    HTMLInputElement* input = toHTMLInputElement(node());
    const AtomicString& type = input->getAttribute(typeAttr);
    if (!equalIgnoringCase(type, "color"))
        return AXObject::colorValue();

    // HTMLInputElement::value always returns a string parseable by Color.
    Color color;
    bool success = color.setFromString(input->value());
    ASSERT_UNUSED(success, success);
    return color.rgb();
}

InvalidState AXNodeObject::invalidState() const
{
    if (hasAttribute(aria_invalidAttr)) {
        const AtomicString& attributeValue = getAttribute(aria_invalidAttr);
        if (equalIgnoringCase(attributeValue, "false"))
            return InvalidStateFalse;
        if (equalIgnoringCase(attributeValue, "true"))
            return InvalidStateTrue;
        if (equalIgnoringCase(attributeValue, "spelling"))
            return InvalidStateSpelling;
        if (equalIgnoringCase(attributeValue, "grammar"))
            return InvalidStateGrammar;
        // A yet unknown value.
        if (!attributeValue.isEmpty())
            return InvalidStateOther;
    }

    if (node() && node()->isElementNode()
        && toElement(node())->isFormControlElement()) {
        HTMLFormControlElement* element = toHTMLFormControlElement(node());
        WillBeHeapVector<RefPtrWillBeMember<HTMLFormControlElement>>
            invalidControls;
        bool isInvalid = !element->checkValidity(
            &invalidControls, CheckValidityDispatchNoEvent);
        return isInvalid ? InvalidStateTrue : InvalidStateFalse;
    }

    return InvalidStateUndefined;
}

int AXNodeObject::posInSet() const
{
    if (supportsSetSizeAndPosInSet()) {
        if (hasAttribute(aria_posinsetAttr))
            return getAttribute(aria_posinsetAttr).toInt();
        return AXObject::indexInParent() + 1;
    }

    return 0;
}

int AXNodeObject::setSize() const
{
    if (supportsSetSizeAndPosInSet()) {
        if (hasAttribute(aria_setsizeAttr))
            return getAttribute(aria_setsizeAttr).toInt();

        if (parentObject()) {
            const auto& siblings = parentObject()->children();
            return siblings.size();
        }
    }

    return 0;
}

String AXNodeObject::ariaInvalidValue() const
{
    if (invalidState() == InvalidStateOther)
        return getAttribute(aria_invalidAttr);

    return String();
}

String AXNodeObject::valueDescription() const
{
    if (!supportsRangeValue())
        return String();

    return getAttribute(aria_valuetextAttr).string();
}

float AXNodeObject::valueForRange() const
{
    if (hasAttribute(aria_valuenowAttr))
        return getAttribute(aria_valuenowAttr).toFloat();

    if (isNativeSlider())
        return toHTMLInputElement(*node()).valueAsNumber();

    if (isHTMLMeterElement(node()))
        return toHTMLMeterElement(*node()).value();

    return 0.0;
}

float AXNodeObject::maxValueForRange() const
{
    if (hasAttribute(aria_valuemaxAttr))
        return getAttribute(aria_valuemaxAttr).toFloat();

    if (isNativeSlider())
        return toHTMLInputElement(*node()).maximum();

    if (isHTMLMeterElement(node()))
        return toHTMLMeterElement(*node()).max();

    return 0.0;
}

float AXNodeObject::minValueForRange() const
{
    if (hasAttribute(aria_valueminAttr))
        return getAttribute(aria_valueminAttr).toFloat();

    if (isNativeSlider())
        return toHTMLInputElement(*node()).minimum();

    if (isHTMLMeterElement(node()))
        return toHTMLMeterElement(*node()).min();

    return 0.0;
}

float AXNodeObject::stepValueForRange() const
{
    if (!isNativeSlider())
        return 0.0;

    Decimal step = toHTMLInputElement(*node()).createStepRange(RejectAny).step();
    return step.toString().toFloat();
}

String AXNodeObject::stringValue() const
{
    Node* node = this->node();
    if (!node)
        return String();

    if (ariaRoleAttribute() == StaticTextRole) {
        String staticText = text();
        if (!staticText.length())
            staticText = deprecatedTextUnderElement(TextUnderElementAll);
        return staticText;
    }

    if (node->isTextNode())
        return deprecatedTextUnderElement(TextUnderElementAll);

    if (isHTMLSelectElement(*node)) {
        HTMLSelectElement& selectElement = toHTMLSelectElement(*node);
        int selectedIndex = selectElement.selectedIndex();
        const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement.listItems();
        if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < listItems.size()) {
            const AtomicString& overriddenDescription = listItems[selectedIndex]->fastGetAttribute(aria_labelAttr);
            if (!overriddenDescription.isNull())
                return overriddenDescription;
        }
        if (!selectElement.multiple())
            return selectElement.value();
        return String();
    }

    if (isTextControl())
        return text();

    // FIXME: We might need to implement a value here for more types
    // FIXME: It would be better not to advertise a value at all for the types for which we don't implement one;
    // this would require subclassing or making accessibilityAttributeNames do something other than return a
    // single static array.
    return String();
}

String AXNodeObject::ariaDescribedByAttribute() const
{
    WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
    elementsFromAttribute(elements, aria_describedbyAttr);

    return accessibilityDescriptionForElements(elements);
}

String AXNodeObject::ariaLabeledByAttribute() const
{
    WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
    ariaLabeledByElements(elements);

    return accessibilityDescriptionForElements(elements);
}

AccessibilityRole AXNodeObject::ariaRoleAttribute() const
{
    return m_ariaRole;
}

// When building the textUnderElement for an object, determine whether or not
// we should include the inner text of this given descendant object or skip it.
static bool shouldUseAccessibilityObjectInnerText(AXObject* obj)
{
    // Consider this hypothetical example:
    // <div tabindex=0>
    //   <h2>
    //     Table of contents
    //   </h2>
    //   <a href="#start">Jump to start of book</a>
    //   <ul>
    //     <li><a href="#1">Chapter 1</a></li>
    //     <li><a href="#1">Chapter 2</a></li>
    //   </ul>
    // </div>
    //
    // The goal is to return a reasonable title for the outer container div, because
    // it's focusable - but without making its title be the full inner text, which is
    // quite long. As a heuristic, skip links, controls, and elements that are usually
    // containers with lots of children.

    // Skip hidden children
    if (obj->isInertOrAriaHidden())
        return false;

    // If something doesn't expose any children, then we can always take the inner text content.
    // This is what we want when someone puts an <a> inside a <button> for example.
    if (obj->isDescendantOfLeafNode())
        return true;

    // Skip focusable children, so we don't include the text of links and controls.
    if (obj->canSetFocusAttribute())
        return false;

    // Skip big container elements like lists, tables, etc.
    if (obj->isList() || obj->isAXTable() || obj->isTree() || obj->isCanvas())
        return false;

    return true;
}

// Returns true if |r1| and |r2| are both non-null and are contained within the
// same LayoutBox.
static bool isSameLayoutBox(LayoutObject* r1, LayoutObject* r2)
{
    if (!r1 || !r2)
        return false;
    LayoutBox* b1 = r1->enclosingBox();
    LayoutBox* b2 = r2->enclosingBox();
    return b1 && b2 && b1 == b2;
}

String AXNodeObject::deprecatedTextUnderElement(TextUnderElementMode mode) const
{
    Node* node = this->node();
    if (node && node->isTextNode())
        return toText(node)->wholeText();

    StringBuilder builder;
    AXObject* previous = nullptr;
    for (AXObject* child = firstChild(); child; child = child->nextSibling()) {
        if (!shouldUseAccessibilityObjectInnerText(child))
            continue;

        if (child->isAXNodeObject()) {
            WillBeHeapVector<OwnPtrWillBeMember<AccessibilityText>> textOrder;
            toAXNodeObject(child)->alternativeText(textOrder);
            if (textOrder.size() > 0) {
                builder.append(textOrder[0]->text());
                if (mode == TextUnderElementAny)
                    break;
                continue;
            }
        }

        // If we're going between two layoutObjects that are in separate LayoutBoxes, add
        // whitespace if it wasn't there already. Intuitively if you have
        // <span>Hello</span><span>World</span>, those are part of the same LayoutBox
        // so we should return "HelloWorld", but given <div>Hello</div><div>World</div> the
        // strings are in separate boxes so we should return "Hello World".
        if (previous && builder.length() && !isHTMLSpace(builder[builder.length() - 1])) {
            if (!isSameLayoutBox(child->layoutObject(), previous->layoutObject()))
                builder.append(' ');
        }

        builder.append(child->deprecatedTextUnderElement(mode));
        previous = child;

        if (mode == TextUnderElementAny && !builder.isEmpty())
            break;
    }

    return builder.toString();
}

AXObject* AXNodeObject::findChildWithTagName(const HTMLQualifiedName& tagName) const
{
    for (AXObject* child = firstChild(); child; child = child->nextSibling()) {
        Node* childNode = child->node();
        if (childNode && childNode->hasTagName(tagName))
            return child;
    }
    return 0;
}

String AXNodeObject::deprecatedAccessibilityDescription() const
{
    // Static text should not have a description, it should only have a stringValue.
    if (roleValue() == StaticTextRole)
        return String();

    String ariaDescription = ariaAccessibilityDescription();
    if (!ariaDescription.isEmpty())
        return ariaDescription;

    if (isImage() || isInputImage() || isNativeImage() || isCanvas()) {
        // Images should use alt as long as the attribute is present, even if empty.
        // Otherwise, it should fallback to other methods, like the title attribute.
        const AtomicString& alt = getAttribute(altAttr);
        if (!alt.isNull())
            return alt;
    }

    // An element's descriptive text is comprised of deprecatedTitle() (what's visible on the screen) and deprecatedAccessibilityDescription() (other descriptive text).
    // Both are used to generate what a screen reader speaks.
    // If this point is reached (i.e. there's no accessibilityDescription) and there's no deprecatedTitle(), we should fallback to using the title attribute.
    // The title attribute is normally used as help text (because it is a tooltip), but if there is nothing else available, this should be used (according to ARIA).
    if (deprecatedTitle(TextUnderElementAny).isEmpty())
        return getAttribute(titleAttr);

    if (roleValue() == FigureRole) {
        AXObject* figcaption = findChildWithTagName(figcaptionTag);
        if (figcaption)
            return figcaption->deprecatedAccessibilityDescription();
    }

    return String();
}

String AXNodeObject::deprecatedTitle(TextUnderElementMode mode) const
{
    Node* node = this->node();
    if (!node)
        return String();

    bool isInputElement = isHTMLInputElement(*node);
    if (isInputElement) {
        HTMLInputElement& input = toHTMLInputElement(*node);
        if (input.isTextButton())
            return input.valueWithDefault();
    }

    if (isInputElement || AXObject::isARIAInput(ariaRoleAttribute()) || isControl()) {
        HTMLLabelElement* label = labelForElement(toElement(node));
        if (label && !deprecatedExposesTitleUIElement())
            return label->innerText();
    }

    // If this node isn't laid out, there's no inner text we can extract from a select element.
    if (!isAXLayoutObject() && isHTMLSelectElement(*node))
        return String();

    switch (roleValue()) {
    case PopUpButtonRole:
        // Native popup buttons should not use their button children's text as a title. That value is retrieved through stringValue().
        if (isHTMLSelectElement(*node))
            return String();
    case ButtonRole:
    case ToggleButtonRole:
    case CheckBoxRole:
    case LineBreakRole:
    case ListBoxOptionRole:
    case ListItemRole:
    case MenuButtonRole:
    case MenuItemRole:
    case MenuItemCheckBoxRole:
    case MenuItemRadioRole:
    case RadioButtonRole:
    case SwitchRole:
    case TabRole:
        return deprecatedTextUnderElement(mode);
    // SVGRoots should not use the text under itself as a title. That could include the text of objects like <text>.
    case SVGRootRole:
        return String();
    case FigureRole: {
        AXObject* figcaption = findChildWithTagName(figcaptionTag);
        if (figcaption)
            return figcaption->deprecatedTextUnderElement();
    }
    default:
        break;
    }

    if (isHeading() || isLink())
        return deprecatedTextUnderElement(mode);

    // If it's focusable but it's not content editable or a known control type, then it will appear to
    // the user as a single atomic object, so we should use its text as the default title.
    if (isGenericFocusableElement())
        return deprecatedTextUnderElement(mode);

    return String();
}

String AXNodeObject::deprecatedHelpText() const
{
    Node* node = this->node();
    if (!node)
        return String();

    const AtomicString& ariaHelp = getAttribute(aria_helpAttr);
    if (!ariaHelp.isEmpty())
        return ariaHelp;

    String describedBy = ariaDescribedByAttribute();
    if (!describedBy.isEmpty())
        return describedBy;

    String description = deprecatedAccessibilityDescription();
    for (Node* curr = node; curr; curr = curr->parentNode()) {
        if (curr->isHTMLElement()) {
            const AtomicString& summary = toElement(curr)->getAttribute(summaryAttr);
            if (!summary.isEmpty())
                return summary;

            // The title attribute should be used as help text unless it is already being used as descriptive text.
            const AtomicString& title = toElement(curr)->getAttribute(titleAttr);
            if (!title.isEmpty() && description != title)
                return title;
        }

        // Only take help text from an ancestor element if its a group or an unknown role. If help was
        // added to those kinds of elements, it is likely it was meant for a child element.
        AXObject* axObj = axObjectCache().getOrCreate(curr);
        if (axObj) {
            AccessibilityRole role = axObj->roleValue();
            if (role != GroupRole && role != UnknownRole)
                break;
        }
    }

    return String();
}

String AXNodeObject::computedName() const
{
    String title = this->deprecatedTitle(TextUnderElementAll);

    String titleUIText;
    if (title.isEmpty()) {
        AXObject* titleUIElement = this->deprecatedTitleUIElement();
        if (titleUIElement) {
            titleUIText = titleUIElement->deprecatedTextUnderElement();
            if (!titleUIText.isEmpty())
                return titleUIText;
        }
    }

    String description = deprecatedAccessibilityDescription();
    if (!description.isEmpty())
        return description;

    if (!title.isEmpty())
        return title;

    String placeholder;
    if (isHTMLInputElement(node())) {
        HTMLInputElement* element = toHTMLInputElement(node());
        placeholder = element->strippedPlaceholder();
        if (!placeholder.isEmpty())
            return placeholder;
    }

    return String();
}

//
// New AX name calculation.
//

String AXNodeObject::textAlternative(bool recursive, bool inAriaLabelledByTraversal, WillBeHeapHashSet<RawPtrWillBeMember<AXObject>>& visited, AXNameFrom* nameFrom, WillBeHeapVector<RawPtrWillBeMember<AXObject>>* nameObjects)
{
    bool alreadyVisited = visited.contains(this);
    visited.add(this);

    if (!node() && !layoutObject())
        return String();

    // Step 2A from: http://www.w3.org/TR/accname-aam-1.1
    if (!recursive && layoutObject()
        && layoutObject()->style()->visibility() != VISIBLE
        && !equalIgnoringCase(getAttribute(aria_hiddenAttr), "false")) {
        return String();
    }

    // Step 2B from: http://www.w3.org/TR/accname-aam-1.1
    if (!inAriaLabelledByTraversal && hasAttribute(aria_labelledbyAttr) && !alreadyVisited) {
        if (nameFrom)
            *nameFrom = AXNameFromRelatedElement;
        WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
        ariaLabeledByElements(elements);
        StringBuilder accumulatedText;
        for (const auto& element : elements) {
            RefPtrWillBeRawPtr<AXObject> axElement = axObjectCache().getOrCreate(element);
            if (axElement) {
                if (nameObjects)
                    nameObjects->append(axElement.get());
                String result = axElement->textAlternative(true, true, visited, nullptr, nullptr);
                if (!result.isEmpty()) {
                    if (!accumulatedText.isEmpty())
                        accumulatedText.append(" ");
                    accumulatedText.append(result);
                }
            }
        }
        return accumulatedText.toString();
    }

    // Step 2C from: http://www.w3.org/TR/accname-aam-1.1
    if (hasAttribute(aria_labelAttr)) {
        const AtomicString& ariaLabel = getAttribute(aria_labelAttr);
        if (!ariaLabel.isEmpty()) {
            if (nameFrom)
                *nameFrom = AXNameFromAttribute;
            return ariaLabel;
        }
    }

    // Step 2F / 2G from: http://www.w3.org/TR/accname-aam-1.1
    if (recursive || nameFromContents()) {
        if (nameFrom)
            *nameFrom = AXNameFromContents;

        Node* node = this->node();
        if (node && node->isTextNode())
            return toText(node)->wholeText();

        StringBuilder accumulatedText;
        AXObject* previous = nullptr;
        for (AXObject* child = firstChild(); child; child = child->nextSibling()) {
            // If we're going between two layoutObjects that are in separate LayoutBoxes, add
            // whitespace if it wasn't there already. Intuitively if you have
            // <span>Hello</span><span>World</span>, those are part of the same LayoutBox
            // so we should return "HelloWorld", but given <div>Hello</div><div>World</div> the
            // strings are in separate boxes so we should return "Hello World".
            if (previous && accumulatedText.length() && !isHTMLSpace(accumulatedText[accumulatedText.length() - 1])) {
                if (!isSameLayoutBox(child->layoutObject(), previous->layoutObject()))
                    accumulatedText.append(' ');
            }

            String result = child->textAlternative(true, false, visited, nullptr, nullptr);
            accumulatedText.append(result);
            previous = child;
        }

        return accumulatedText.toString();
    }

    if (nameFrom)
        *nameFrom = AXNameFromAttribute;
    return String();
}

LayoutRect AXNodeObject::elementRect() const
{
    // First check if it has a custom rect, for example if this element is tied to a canvas path.
    if (!m_explicitElementRect.isEmpty())
        return m_explicitElementRect;

    // FIXME: If there are a lot of elements in the canvas, it will be inefficient.
    // We can avoid the inefficient calculations by using AXComputedObjectAttributeCache.
    if (node()->parentElement()->isInCanvasSubtree()) {
        LayoutRect rect;

        for (Node& child : NodeTraversal::childrenOf(*node())) {
            if (child.isHTMLElement()) {
                if (AXObject* obj = axObjectCache().get(&child)) {
                    if (rect.isEmpty())
                        rect = obj->elementRect();
                    else
                        rect.unite(obj->elementRect());
                }
            }
        }

        if (!rect.isEmpty())
            return rect;
    }

    // If this object doesn't have an explicit element rect or computable from its children,
    // for now, let's return the position of the ancestor that does have a position,
    // and make it the width of that parent, and about the height of a line of text, so that it's clear the object is a child of the parent.

    LayoutRect boundingBox;

    for (AXObject* positionProvider = parentObject(); positionProvider; positionProvider = positionProvider->parentObject()) {
        if (positionProvider->isAXLayoutObject()) {
            LayoutRect parentRect = positionProvider->elementRect();
            boundingBox.setSize(LayoutSize(parentRect.width(), LayoutUnit(std::min(10.0f, parentRect.height().toFloat()))));
            boundingBox.setLocation(parentRect.location());
            break;
        }
    }

    return boundingBox;
}

static Node* getParentNodeForComputeParent(Node* node)
{
    if (!node)
        return nullptr;

    Node* parentNode = nullptr;

    // Skip over <optgroup> and consider the <select> the immediate parent of an <option>.
    if (isHTMLOptionElement(node))
        parentNode = toHTMLOptionElement(node)->ownerSelectElement();

    if (!parentNode)
        parentNode = node->parentNode();

    return parentNode;
}

AXObject* AXNodeObject::computeParent() const
{
    if (Node* parentNode = getParentNodeForComputeParent(node()))
        return axObjectCache().getOrCreate(parentNode);

    return nullptr;
}

AXObject* AXNodeObject::computeParentIfExists() const
{
    if (Node* parentNode = getParentNodeForComputeParent(node()))
        return axObjectCache().get(parentNode);

    return nullptr;
}

AXObject* AXNodeObject::firstChild() const
{
    if (!node())
        return 0;

    Node* firstChild = node()->firstChild();

    if (!firstChild)
        return 0;

    return axObjectCache().getOrCreate(firstChild);
}

AXObject* AXNodeObject::nextSibling() const
{
    if (!node())
        return 0;

    Node* nextSibling = node()->nextSibling();
    if (!nextSibling)
        return 0;

    return axObjectCache().getOrCreate(nextSibling);
}

void AXNodeObject::addChildren()
{
    // If the need to add more children in addition to existing children arises,
    // childrenChanged should have been called, leaving the object with no children.
    ASSERT(!m_haveChildren);

    if (!m_node)
        return;

    m_haveChildren = true;

    // The only time we add children from the DOM tree to a node with a layoutObject is when it's a canvas.
    if (layoutObject() && !isHTMLCanvasElement(*m_node))
        return;

    Vector<AXObject*> ownedChildren;
    computeAriaOwnsChildren(ownedChildren);

    for (Node& child : NodeTraversal::childrenOf(*m_node)) {
        AXObject* childObj = axObjectCache().getOrCreate(&child);
        if (!axObjectCache().isAriaOwned(childObj))
            addChild(childObj);
    }

    for (const auto& ownedChild : ownedChildren)
        addChild(ownedChild);

    for (const auto& child : m_children)
        child->setParent(this);
}

void AXNodeObject::addChild(AXObject* child)
{
    insertChild(child, m_children.size());
}

void AXNodeObject::insertChild(AXObject* child, unsigned index)
{
    if (!child)
        return;

    // If the parent is asking for this child's children, then either it's the first time (and clearing is a no-op),
    // or its visibility has changed. In the latter case, this child may have a stale child cached.
    // This can prevent aria-hidden changes from working correctly. Hence, whenever a parent is getting children, ensure data is not stale.
    child->clearChildren();

    if (child->accessibilityIsIgnored()) {
        const auto& children = child->children();
        size_t length = children.size();
        for (size_t i = 0; i < length; ++i)
            m_children.insert(index + i, children[i]);
    } else {
        ASSERT(child->parentObject() == this);
        m_children.insert(index, child);
    }
}

bool AXNodeObject::canHaveChildren() const
{
    // If this is an AXLayoutObject, then it's okay if this object
    // doesn't have a node - there are some layoutObjects that don't have associated
    // nodes, like scroll areas and css-generated text.
    if (!node() && !isAXLayoutObject())
        return false;

    // Elements that should not have children
    switch (roleValue()) {
    case ImageRole:
    case ButtonRole:
    case PopUpButtonRole:
    case CheckBoxRole:
    case RadioButtonRole:
    case SwitchRole:
    case TabRole:
    case ToggleButtonRole:
    case ListBoxOptionRole:
    case ScrollBarRole:
        return false;
    case StaticTextRole:
        if (!axObjectCache().inlineTextBoxAccessibilityEnabled())
            return false;
    default:
        return true;
    }
}

Element* AXNodeObject::actionElement() const
{
    Node* node = this->node();
    if (!node)
        return 0;

    if (isHTMLInputElement(*node)) {
        HTMLInputElement& input = toHTMLInputElement(*node);
        if (!input.isDisabledFormControl() && (isCheckboxOrRadio() || input.isTextButton()
            || input.type() == InputTypeNames::file))
            return &input;
    } else if (isHTMLButtonElement(*node)) {
        return toElement(node);
    }

    if (AXObject::isARIAInput(ariaRoleAttribute()))
        return toElement(node);

    if (isImageButton())
        return toElement(node);

    if (isHTMLSelectElement(*node))
        return toElement(node);

    switch (roleValue()) {
    case ButtonRole:
    case PopUpButtonRole:
    case ToggleButtonRole:
    case TabRole:
    case MenuItemRole:
    case MenuItemCheckBoxRole:
    case MenuItemRadioRole:
    case ListItemRole:
        return toElement(node);
    default:
        break;
    }

    Element* elt = anchorElement();
    if (!elt)
        elt = mouseButtonListener();
    return elt;
}

Element* AXNodeObject::anchorElement() const
{
    Node* node = this->node();
    if (!node)
        return 0;

    AXObjectCacheImpl& cache = axObjectCache();

    // search up the DOM tree for an anchor element
    // NOTE: this assumes that any non-image with an anchor is an HTMLAnchorElement
    for ( ; node; node = node->parentNode()) {
        if (isHTMLAnchorElement(*node) || (node->layoutObject() && cache.getOrCreate(node->layoutObject())->isAnchor()))
            return toElement(node);
    }

    return 0;
}

Document* AXNodeObject::document() const
{
    if (!node())
        return 0;
    return &node()->document();
}

void AXNodeObject::setNode(Node* node)
{
    m_node = node;
}

AXObject* AXNodeObject::correspondingControlForLabelElement() const
{
    HTMLLabelElement* labelElement = labelElementContainer();
    if (!labelElement)
        return 0;

    HTMLElement* correspondingControl = labelElement->control();
    if (!correspondingControl)
        return 0;

    // Make sure the corresponding control isn't a descendant of this label
    // that's in the middle of being destroyed.
    if (correspondingControl->layoutObject() && !correspondingControl->layoutObject()->parent())
        return 0;

    return axObjectCache().getOrCreate(correspondingControl);
}

HTMLLabelElement* AXNodeObject::labelElementContainer() const
{
    if (!node())
        return 0;

    // the control element should not be considered part of the label
    if (isControl())
        return 0;

    // the link element should not be considered part of the label
    if (isLink())
        return 0;

    // find if this has a ancestor that is a label
    return Traversal<HTMLLabelElement>::firstAncestorOrSelf(*node());
}

void AXNodeObject::setFocused(bool on)
{
    if (!canSetFocusAttribute())
        return;

    Document* document = this->document();
    if (!on) {
        document->setFocusedElement(nullptr);
    } else {
        Node* node = this->node();
        if (node && node->isElementNode()) {
            // If this node is already the currently focused node, then calling focus() won't do anything.
            // That is a problem when focus is removed from the webpage to chrome, and then returns.
            // In these cases, we need to do what keyboard and mouse focus do, which is reset focus first.
            if (document->focusedElement() == node)
                document->setFocusedElement(nullptr);

            toElement(node)->focus();
        } else {
            document->setFocusedElement(nullptr);
        }
    }
}

void AXNodeObject::increment()
{
    UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);
    alterSliderValue(true);
}

void AXNodeObject::decrement()
{
    UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);
    alterSliderValue(false);
}

void AXNodeObject::childrenChanged()
{
    // This method is meant as a quick way of marking a portion of the accessibility tree dirty.
    if (!node() && !layoutObject())
        return;

    // If this is not part of the accessibility tree because an ancestor
    // has only presentational children, invalidate this object's children but
    // skip sending a notification and skip walking up the ancestors.
    if (ancestorForWhichThisIsAPresentationalChild()) {
        setNeedsToUpdateChildren();
        return;
    }

    axObjectCache().postNotification(this, AXObjectCacheImpl::AXChildrenChanged);

    // Go up the accessibility parent chain, but only if the element already exists. This method is
    // called during layout, minimal work should be done.
    // If AX elements are created now, they could interrogate the layout tree while it's in a funky state.
    // At the same time, process ARIA live region changes.
    for (AXObject* parent = this; parent; parent = parent->parentObjectIfExists()) {
        parent->setNeedsToUpdateChildren();

        // These notifications always need to be sent because screenreaders are reliant on them to perform.
        // In other words, they need to be sent even when the screen reader has not accessed this live region since the last update.

        // If this element supports ARIA live regions, then notify the AT of changes.
        if (parent->isLiveRegion())
            axObjectCache().postNotification(parent, AXObjectCacheImpl::AXLiveRegionChanged);

        // If this element is an ARIA text box or content editable, post a "value changed" notification on it
        // so that it behaves just like a native input element or textarea.
        if (isNonNativeTextControl())
            axObjectCache().postNotification(parent, AXObjectCacheImpl::AXValueChanged);
    }
}

void AXNodeObject::selectionChanged()
{
    // Post the selected text changed event on the first ancestor that's
    // focused (to handle form controls, ARIA text boxes and contentEditable),
    // or the web area if the selection is just in the document somewhere.
    if (isFocused() || isWebArea())
        axObjectCache().postNotification(this, AXObjectCacheImpl::AXSelectedTextChanged);
    else
        AXObject::selectionChanged(); // Calls selectionChanged on parent.
}

void AXNodeObject::textChanged()
{
    // If this element supports ARIA live regions, or is part of a region with an ARIA editable role,
    // then notify the AT of changes.
    AXObjectCacheImpl& cache = axObjectCache();
    for (Node* parentNode = node(); parentNode; parentNode = parentNode->parentNode()) {
        AXObject* parent = cache.get(parentNode);
        if (!parent)
            continue;

        if (parent->isLiveRegion())
            cache.postNotification(parentNode, AXObjectCacheImpl::AXLiveRegionChanged);

        // If this element is an ARIA text box or content editable, post a "value changed" notification on it
        // so that it behaves just like a native input element or textarea.
        if (parent->isNonNativeTextControl())
            cache.postNotification(parentNode, AXObjectCacheImpl::AXValueChanged);
    }
}

void AXNodeObject::updateAccessibilityRole()
{
    bool ignoredStatus = accessibilityIsIgnored();
    m_role = determineAccessibilityRole();

    // The AX hierarchy only needs to be updated if the ignored status of an element has changed.
    if (ignoredStatus != accessibilityIsIgnored())
        childrenChanged();
}

void AXNodeObject::computeAriaOwnsChildren(Vector<AXObject*>& ownedChildren)
{
    if (!hasAttribute(aria_ownsAttr))
        return;

    Vector<String> idVector;
    tokenVectorFromAttribute(idVector, aria_ownsAttr);

    axObjectCache().updateAriaOwns(this, idVector, ownedChildren);
}

String AXNodeObject::alternativeTextForWebArea() const
{
    // The WebArea description should follow this order:
    //     aria-label on the <html>
    //     title on the <html>
    //     <title> inside the <head> (of it was set through JS)
    //     name on the <html>
    // For iframes:
    //     aria-label on the <iframe>
    //     title on the <iframe>
    //     name on the <iframe>

    Document* document = this->document();
    if (!document)
        return String();

    // Check if the HTML element has an aria-label for the webpage.
    if (Element* documentElement = document->documentElement()) {
        const AtomicString& ariaLabel = documentElement->getAttribute(aria_labelAttr);
        if (!ariaLabel.isEmpty())
            return ariaLabel;
    }

    if (HTMLFrameOwnerElement* owner = document->ownerElement()) {
        if (isHTMLFrameElementBase(*owner)) {
            const AtomicString& title = owner->getAttribute(titleAttr);
            if (!title.isEmpty())
                return title;
        }
        return owner->getNameAttribute();
    }

    String documentTitle = document->title();
    if (!documentTitle.isEmpty())
        return documentTitle;

    if (HTMLElement* body = document->body())
        return body->getNameAttribute();

    return String();
}

void AXNodeObject::alternativeText(WillBeHeapVector<OwnPtrWillBeMember<AccessibilityText>>& textOrder) const
{
    if (isWebArea()) {
        String webAreaText = alternativeTextForWebArea();
        if (!webAreaText.isEmpty())
            textOrder.append(AccessibilityText::create(webAreaText, AlternativeText));
        return;
    }

    ariaLabeledByText(textOrder);

    const AtomicString& ariaLabel = getAttribute(aria_labelAttr);
    if (!ariaLabel.isEmpty())
        textOrder.append(AccessibilityText::create(ariaLabel, AlternativeText));

    if (isImage() || isInputImage() || isNativeImage() || isCanvas()) {
        // Images should use alt as long as the attribute is present, even if empty.
        // Otherwise, it should fallback to other methods, like the title attribute.
        const AtomicString& alt = getAttribute(altAttr);
        if (!alt.isNull())
            textOrder.append(AccessibilityText::create(alt, AlternativeText));
    }
}

void AXNodeObject::ariaLabeledByText(WillBeHeapVector<OwnPtrWillBeMember<AccessibilityText>>& textOrder) const
{
    String ariaLabeledBy = ariaLabeledByAttribute();
    if (!ariaLabeledBy.isEmpty()) {
        WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
        ariaLabeledByElements(elements);

        for (const auto& element : elements) {
            RefPtrWillBeRawPtr<AXObject> axElement = axObjectCache().getOrCreate(element);
            textOrder.append(AccessibilityText::create(ariaLabeledBy, AlternativeText, axElement));
        }
    }
}

DEFINE_TRACE(AXNodeObject)
{
    visitor->trace(m_node);
    AXObject::trace(visitor);
}

} // namespace blink
