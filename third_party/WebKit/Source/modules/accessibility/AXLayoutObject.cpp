/*
* Copyright (C) 2008 Apple Inc. All rights reserved.
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
#include "modules/accessibility/AXLayoutObject.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSPropertyNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/RenderedPosition.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/CharacterIterator.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLLabelElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutFieldset.h"
#include "core/layout/LayoutFileUploadControl.h"
#include "core/layout/LayoutHTMLCanvas.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutMenuList.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutTextControlSingleLine.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/layout/LayoutView.h"
#include "core/loader/ProgressTracker.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/graphics/SVGImage.h"
#include "modules/accessibility/AXImageMapLink.h"
#include "modules/accessibility/AXInlineTextBox.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/accessibility/AXSVGRoot.h"
#include "modules/accessibility/AXSpinButton.h"
#include "modules/accessibility/AXTable.h"
#include "platform/fonts/FontTraits.h"
#include "platform/text/PlatformLocale.h"
#include "platform/text/TextDirection.h"
#include "wtf/StdLibExtras.h"

using blink::WebLocalizedString;

namespace blink {

using namespace HTMLNames;

static inline LayoutObject* firstChildInContinuation(const LayoutInline& layoutObject)
{
    LayoutBoxModelObject* r = layoutObject.continuation();

    while (r) {
        if (r->isLayoutBlock())
            return r;
        if (LayoutObject* child = r->slowFirstChild())
            return child;
        r = toLayoutInline(r)->continuation();
    }

    return 0;
}

static inline bool isInlineWithContinuation(LayoutObject* object)
{
    if (!object->isBoxModelObject())
        return false;

    LayoutBoxModelObject* layoutObject = toLayoutBoxModelObject(object);
    if (!layoutObject->isLayoutInline())
        return false;

    return toLayoutInline(layoutObject)->continuation();
}

static inline LayoutObject* firstChildConsideringContinuation(LayoutObject* layoutObject)
{
    LayoutObject* firstChild = layoutObject->slowFirstChild();

    if (!firstChild && isInlineWithContinuation(layoutObject))
        firstChild = firstChildInContinuation(toLayoutInline(*layoutObject));

    return firstChild;
}

static inline LayoutInline* startOfContinuations(LayoutObject* r)
{
    if (r->isInlineElementContinuation()) {
        return toLayoutInline(r->node()->layoutObject());
    }

    // Blocks with a previous continuation always have a next continuation
    if (r->isLayoutBlock() && toLayoutBlock(r)->inlineElementContinuation())
        return toLayoutInline(toLayoutBlock(r)->inlineElementContinuation()->node()->layoutObject());

    return 0;
}

static inline LayoutObject* endOfContinuations(LayoutObject* layoutObject)
{
    LayoutObject* prev = layoutObject;
    LayoutObject* cur = layoutObject;

    if (!cur->isLayoutInline() && !cur->isLayoutBlock())
        return layoutObject;

    while (cur) {
        prev = cur;
        if (cur->isLayoutInline()) {
            cur = toLayoutInline(cur)->inlineElementContinuation();
            ASSERT(cur || !toLayoutInline(prev)->continuation());
        } else {
            cur = toLayoutBlock(cur)->inlineElementContinuation();
        }
    }

    return prev;
}

static inline bool lastChildHasContinuation(LayoutObject* layoutObject)
{
    LayoutObject* lastChild = layoutObject->slowLastChild();
    return lastChild && isInlineWithContinuation(lastChild);
}

static LayoutBoxModelObject* nextContinuation(LayoutObject* layoutObject)
{
    ASSERT(layoutObject);
    if (layoutObject->isLayoutInline() && !layoutObject->isReplaced())
        return toLayoutInline(layoutObject)->continuation();
    if (layoutObject->isLayoutBlock())
        return toLayoutBlock(layoutObject)->inlineElementContinuation();
    return 0;
}

AXLayoutObject::AXLayoutObject(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXNodeObject(layoutObject->node(), axObjectCache)
    , m_layoutObject(layoutObject)
    , m_cachedElementRectDirty(true)
{
#if ENABLE(ASSERT)
    m_layoutObject->setHasAXObject(true);
#endif
}

PassRefPtrWillBeRawPtr<AXLayoutObject> AXLayoutObject::create(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXLayoutObject(layoutObject, axObjectCache));
}

AXLayoutObject::~AXLayoutObject()
{
    ASSERT(isDetached());
}

LayoutRect AXLayoutObject::elementRect() const
{
    if (!m_explicitElementRect.isEmpty())
        return m_explicitElementRect;
    if (!m_layoutObject)
        return LayoutRect();
    if (!m_layoutObject->isBox())
        return computeElementRect();

    for (const AXObject* obj = this; obj; obj = obj->parentObject()) {
        if (obj->isAXLayoutObject())
            toAXLayoutObject(obj)->checkCachedElementRect();
    }
    for (const AXObject* obj = this; obj; obj = obj->parentObject()) {
        if (obj->isAXLayoutObject())
            toAXLayoutObject(obj)->updateCachedElementRect();
    }

    return m_cachedElementRect;
}

void AXLayoutObject::setLayoutObject(LayoutObject* layoutObject)
{
    m_layoutObject = layoutObject;
    setNode(layoutObject->node());
}

LayoutBoxModelObject* AXLayoutObject::layoutBoxModelObject() const
{
    if (!m_layoutObject || !m_layoutObject->isBoxModelObject())
        return 0;
    return toLayoutBoxModelObject(m_layoutObject);
}

Document* AXLayoutObject::topDocument() const
{
    if (!document())
        return 0;
    return &document()->topDocument();
}

bool AXLayoutObject::shouldNotifyActiveDescendant() const
{
    // We want to notify that the combo box has changed its active descendant,
    // but we do not want to change the focus, because focus should remain with the combo box.
    if (isComboBox())
        return true;

    return shouldFocusActiveDescendant();
}

ScrollableArea* AXLayoutObject::getScrollableAreaIfScrollable() const
{
    // FIXME(dmazzoni): the plan is to get rid of AXScrollView, but until
    // this is done, a WebArea delegates its scrolling to its parent scroll view.
    // http://crbug.com/484878
    if (parentObject() && parentObject()->isAXScrollView())
        return parentObject()->getScrollableAreaIfScrollable();

    if (!m_layoutObject || !m_layoutObject->isBox())
        return 0;

    LayoutBox* box = toLayoutBox(m_layoutObject);
    if (!box->canBeScrolledAndHasScrollableArea())
        return 0;

    return box->scrollableArea();
}

static bool isImageOrAltText(LayoutBoxModelObject* box, Node* node)
{
    if (box && box->isImage())
        return true;
    if (isHTMLImageElement(node))
        return true;
    if (isHTMLInputElement(node) && toHTMLInputElement(node)->hasFallbackContent())
        return true;
    return false;
}

AccessibilityRole AXLayoutObject::determineAccessibilityRole()
{
    if (!m_layoutObject)
        return UnknownRole;

    if ((m_ariaRole = determineAriaRoleAttribute()) != UnknownRole)
        return m_ariaRole;

    Node* node = m_layoutObject->node();
    LayoutBoxModelObject* cssBox = layoutBoxModelObject();

    if ((cssBox && cssBox->isListItem()) || isHTMLLIElement(node))
        return ListItemRole;
    if (m_layoutObject->isListMarker())
        return ListMarkerRole;
    if (m_layoutObject->isBR())
        return LineBreakRole;
    if (m_layoutObject->isText())
        return StaticTextRole;
    if (cssBox && isImageOrAltText(cssBox, node)) {
        if (node && node->isLink())
            return ImageMapRole;
        if (isHTMLInputElement(node))
            return ariaHasPopup() ? PopUpButtonRole : ButtonRole;
        if (isSVGImage())
            return SVGRootRole;
        return ImageRole;
    }
    // Note: if JavaScript is disabled, the layoutObject won't be a LayoutHTMLCanvas.
    if (isHTMLCanvasElement(node) && m_layoutObject->isCanvas())
        return CanvasRole;

    if (cssBox && cssBox->isLayoutView())
        return WebAreaRole;

    if (m_layoutObject->isSVGImage())
        return ImageRole;
    if (m_layoutObject->isSVGRoot())
        return SVGRootRole;

    // Table sections should be ignored.
    if (m_layoutObject->isTableSection())
        return IgnoredRole;

    if (m_layoutObject->isHR())
        return SplitterRole;

    AccessibilityRole role = AXNodeObject::determineAccessibilityRoleUtil();
    if (role != UnknownRole)
        return role;

    if (m_layoutObject->isLayoutBlockFlow())
        return GroupRole;

    // If the element does not have role, but it has ARIA attributes, accessibility should fallback to exposing it as a group.
    if (supportsARIAAttributes())
        return GroupRole;

    return UnknownRole;
}

void AXLayoutObject::init()
{
    AXNodeObject::init();
}

void AXLayoutObject::detach()
{
    AXNodeObject::detach();

    detachRemoteSVGRoot();

#if ENABLE(ASSERT)
    if (m_layoutObject)
        m_layoutObject->setHasAXObject(false);
#endif
    m_layoutObject = 0;
}

//
// Check object role or purpose.
//

bool AXLayoutObject::isAttachment() const
{
    LayoutBoxModelObject* layoutObject = layoutBoxModelObject();
    if (!layoutObject)
        return false;
    // Widgets are the replaced elements that we represent to AX as attachments
    bool isLayoutPart = layoutObject->isLayoutPart();
    ASSERT(!isLayoutPart || (layoutObject->isReplaced() && !isImage()));
    return isLayoutPart;
}

static bool isLinkable(const AXObject& object)
{
    if (!object.layoutObject())
        return false;

    // See https://wiki.mozilla.org/Accessibility/AT-Windows-API for the elements
    // Mozilla considers linkable.
    return object.isLink() || object.isImage() || object.layoutObject()->isText();
}

// Requires layoutObject to be present because it relies on style
// user-modify. Don't move this logic to AXNodeObject.
// TODO(nektar): Implement support in AXNodeObject for aria-hidden and canvas.
bool AXLayoutObject::isRichlyEditable() const
{
    if (node() && node()->isContentRichlyEditable())
        return true;

    if (isWebArea()) {
        Document& document = m_layoutObject->document();
        HTMLElement* body = document.body();
        if (body && body->isContentRichlyEditable())
            return true;

        return document.isContentRichlyEditable();
    }

    return false;
}

bool AXLayoutObject::isLinked() const
{
    if (!isLinkable(*this))
        return false;

    Element* anchor = anchorElement();
    if (!isHTMLAnchorElement(anchor))
        return false;

    return !toHTMLAnchorElement(*anchor).href().isEmpty();
}

bool AXLayoutObject::isLoaded() const
{
    return !m_layoutObject->document().parser();
}

bool AXLayoutObject::isOffScreen() const
{
    ASSERT(m_layoutObject);
    IntRect contentRect = pixelSnappedIntRect(m_layoutObject->absoluteClippedOverflowRect());
    FrameView* view = m_layoutObject->frame()->view();
    IntRect viewRect = view->visibleContentRect();
    viewRect.intersect(contentRect);
    return viewRect.isEmpty();
}

bool AXLayoutObject::isReadOnly() const
{
    ASSERT(m_layoutObject);

    if (isWebArea()) {
        Document& document = m_layoutObject->document();
        HTMLElement* body = document.body();
        if (body && body->hasEditableStyle())
            return false;

        return !document.hasEditableStyle();
    }

    return AXNodeObject::isReadOnly();
}

bool AXLayoutObject::isVisited() const
{
    // FIXME: Is it a privacy violation to expose visited information to accessibility APIs?
    return m_layoutObject->style()->isLink() && m_layoutObject->style()->insideLink() == InsideVisitedLink;
}

//
// Check object state.
//

bool AXLayoutObject::isFocused() const
{
    if (!m_layoutObject)
        return false;

    Document& document = m_layoutObject->document();
    Element* focusedElement = document.focusedElement();
    if (!focusedElement)
        return false;

    // A web area is represented by the Document node in the DOM tree, which isn't focusable.
    // Check instead if the frame's selection controller is focused
    if (focusedElement == m_layoutObject->node()
        || (roleValue() == WebAreaRole && document.frame()->selection().isFocusedAndActive()))
        return true;

    return false;
}

bool AXLayoutObject::isSelected() const
{
    if (!m_layoutObject)
        return false;

    Node* node = m_layoutObject->node();
    if (!node)
        return false;

    const AtomicString& ariaSelected = getAttribute(aria_selectedAttr);
    if (equalIgnoringCase(ariaSelected, "true"))
        return true;

    if (isTabItem() && isTabItemSelected())
        return true;

    return false;
}

//
// Whether objects are ignored, i.e. not included in the tree.
//

AXObjectInclusion AXLayoutObject::defaultObjectInclusion(IgnoredReasons* ignoredReasons) const
{
    // The following cases can apply to any element that's a subclass of AXLayoutObject.

    if (!m_layoutObject) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXNotRendered));
        return IgnoreObject;
    }

    if (m_layoutObject->style()->visibility() != VISIBLE) {
        // aria-hidden is meant to override visibility as the determinant in AX hierarchy inclusion.
        if (equalIgnoringCase(getAttribute(aria_hiddenAttr), "false"))
            return DefaultBehavior;

        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXNotVisible));
        return IgnoreObject;
    }

    return AXObject::defaultObjectInclusion(ignoredReasons);
}

bool AXLayoutObject::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
#if ENABLE(ASSERT)
    ASSERT(m_initialized);
#endif

    // Check first if any of the common reasons cause this element to be ignored.
    // Then process other use cases that need to be applied to all the various roles
    // that AXLayoutObjects take on.
    AXObjectInclusion decision = defaultObjectInclusion(ignoredReasons);
    if (decision == IncludeObject)
        return false;
    if (decision == IgnoreObject)
        return true;

    // If this element is within a parent that cannot have children, it should not be exposed
    if (isDescendantOfLeafNode()) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXAncestorIsLeafNode, leafNodeAncestor()));
        return true;
    }

    if (roleValue() == IgnoredRole) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXUninteresting));
        return true;
    }

    if (hasInheritedPresentationalRole()) {
        if (ignoredReasons) {
            const AXObject* inheritsFrom = inheritsPresentationalRoleFrom();
            if (inheritsFrom == this)
                ignoredReasons->append(IgnoredReason(AXPresentationalRole));
            else
                ignoredReasons->append(IgnoredReason(AXInheritsPresentation, inheritsFrom));
        }
        return true;
    }

    // An ARIA tree can only have tree items and static text as children.
    if (AXObject* treeAncestor = treeAncestorDisallowingChild()) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXAncestorDisallowsChild, treeAncestor));
        return true;
    }

    // TODO: we should refactor this - but right now this is necessary to make
    // sure scroll areas stay in the tree.
    if (isAttachment())
        return false;

    // find out if this element is inside of a label element.
    // if so, it may be ignored because it's the label for a checkbox or radio button
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

    if (m_layoutObject->isBR())
        return false;

    if (m_layoutObject->isText()) {
        // static text beneath MenuItems and MenuButtons are just reported along with the menu item, so it's ignored on an individual level
        AXObject* parent = parentObjectUnignored();
        if (parent && (parent->ariaRoleAttribute() == MenuItemRole || parent->ariaRoleAttribute() == MenuButtonRole)) {
            if (ignoredReasons)
                ignoredReasons->append(IgnoredReason(AXStaticTextUsedAsNameFor, parent));
            return true;
        }
        LayoutText* layoutText = toLayoutText(m_layoutObject);
        if (!layoutText->firstTextBox()) {
            if (ignoredReasons)
                ignoredReasons->append(IgnoredReason(AXEmptyText));
            return true;
        }

        // Don't ignore static text in editable text controls.
        for (AXObject* parent = parentObject(); parent; parent = parent->parentObject()) {
            if (parent->roleValue() == TextFieldRole)
                return false;
        }

        // text elements that are just empty whitespace should not be returned
        // FIXME(dmazzoni): we probably shouldn't ignore this if the style is 'pre', or similar...
        if (layoutText->text().impl()->containsOnlyWhitespace()) {
            if (ignoredReasons)
                ignoredReasons->append(IgnoredReason(AXEmptyText));
            return true;
        }
        return false;
    }
    if (isHeading())
        return false;

    if (isLandmarkRelated())
        return false;

    if (isLink())
        return false;

    // all controls are accessible
    if (isControl())
        return false;

    if (ariaRoleAttribute() != UnknownRole)
        return false;

    // don't ignore labels, because they serve as TitleUIElements
    Node* node = m_layoutObject->node();
    if (isHTMLLabelElement(node))
        return false;

    // Anything that is content editable should not be ignored.
    // However, one cannot just call node->hasEditableStyle() since that will ask if its parents
    // are also editable. Only the top level content editable region should be exposed.
    if (hasContentEditableAttributeSet())
        return false;

    // List items play an important role in defining the structure of lists. They should not be ignored.
    if (roleValue() == ListItemRole)
        return false;

    if (roleValue() == BlockquoteRole)
        return false;

    if (roleValue() == DialogRole)
        return false;

    if (roleValue() == FigcaptionRole)
        return false;

    if (roleValue() == FigureRole)
        return false;

    if (roleValue() == DetailsRole)
        return false;

    if (roleValue() == MathRole)
        return false;

    if (roleValue() == MeterRole)
        return false;

    if (roleValue() == RubyRole)
        return false;

    if (roleValue() == TimeRole)
        return false;

    if (roleValue() == MarkRole)
        return false;

    // if this element has aria attributes on it, it should not be ignored.
    if (supportsARIAAttributes())
        return false;

    // <span> tags are inline tags and not meant to convey information if they have no other aria
    // information on them. If we don't ignore them, they may emit signals expected to come from
    // their parent. In addition, because included spans are GroupRole objects, and GroupRole
    // objects are often containers with meaningful information, the inclusion of a span can have
    // the side effect of causing the immediate parent accessible to be ignored. This is especially
    // problematic for platforms which have distinct roles for textual block elements.
    if (isHTMLSpanElement(node)) {
        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXUninteresting));
        return true;
    }

    if (m_layoutObject->isLayoutBlockFlow() && m_layoutObject->childrenInline() && !canSetFocusAttribute()) {
        if (toLayoutBlockFlow(m_layoutObject)->firstLineBox() || mouseButtonListener())
            return false;

        if (ignoredReasons)
            ignoredReasons->append(IgnoredReason(AXUninteresting));
        return true;
    }

    // ignore images seemingly used as spacers
    if (isImage()) {
        // If the image can take focus, it should not be ignored, lest the user not be able to interact with something important.
        if (canSetFocusAttribute())
            return false;

        if (node && node->isElementNode()) {
            Element* elt = toElement(node);
            const AtomicString& alt = elt->getAttribute(altAttr);
            // don't ignore an image that has an alt tag
            if (!alt.string().containsOnlyWhitespace())
                return false;
            // informal standard is to ignore images with zero-length alt strings
            if (!alt.isNull()) {
                if (ignoredReasons)
                    ignoredReasons->append(IgnoredReason(AXEmptyAlt));
                return true;
            }
        }

        if (isNativeImage() && m_layoutObject->isImage()) {
            // check for one-dimensional image
            LayoutImage* image = toLayoutImage(m_layoutObject);
            if (image->size().height() <= 1 || image->size().width() <= 1) {
                if (ignoredReasons)
                    ignoredReasons->append(IgnoredReason(AXProbablyPresentational));
                return true;
            }

            // check whether laid out image was stretched from one-dimensional file image
            if (image->cachedImage()) {
                LayoutSize imageSize = image->cachedImage()->imageSizeForLayoutObject(m_layoutObject, image->view()->zoomFactor());
                if (imageSize.height() <= 1 || imageSize.width() <= 1) {
                    if (ignoredReasons)
                        ignoredReasons->append(IgnoredReason(AXProbablyPresentational));
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    if (isCanvas()) {
        if (canvasHasFallbackContent())
            return false;
        LayoutHTMLCanvas* canvas = toLayoutHTMLCanvas(m_layoutObject);
        if (canvas->size().height() <= 1 || canvas->size().width() <= 1) {
            if (ignoredReasons)
                ignoredReasons->append(IgnoredReason(AXProbablyPresentational));
            return true;
        }
        // Otherwise fall through; use presence of help text, title, or description to decide.
    }

    if (isWebArea() || m_layoutObject->isListMarker())
        return false;

    // Using the help text, title or accessibility description (so we
    // check if there's some kind of accessible name for the element)
    // to decide an element's visibility is not as definitive as
    // previous checks, so this should remain as one of the last.
    //
    // These checks are simplified in the interest of execution speed;
    // for example, any element having an alt attribute will make it
    // not ignored, rather than just images.
    if (!getAttribute(aria_helpAttr).isEmpty() || !getAttribute(aria_describedbyAttr).isEmpty() || !getAttribute(altAttr).isEmpty() || !getAttribute(titleAttr).isEmpty())
        return false;

    // Don't ignore generic focusable elements like <div tabindex=0>
    // unless they're completely empty, with no children.
    if (isGenericFocusableElement() && node->hasChildren())
        return false;

    if (!ariaAccessibilityDescription().isEmpty())
        return false;

    if (isScrollableContainer())
        return false;

    // By default, objects should be ignored so that the AX hierarchy is not
    // filled with unnecessary items.
    if (ignoredReasons)
        ignoredReasons->append(IgnoredReason(AXUninteresting));
    return true;
}

//
// Properties of static elements.
//

const AtomicString& AXLayoutObject::accessKey() const
{
    Node* node = m_layoutObject->node();
    if (!node)
        return nullAtom;
    if (!node->isElementNode())
        return nullAtom;
    return toElement(node)->getAttribute(accesskeyAttr);
}

RGBA32 AXLayoutObject::backgroundColor() const
{
    if (!m_layoutObject)
        return AXNodeObject::backgroundColor();

    const ComputedStyle* style = m_layoutObject->style();
    if (!style || !style->hasBackground())
        return AXNodeObject::backgroundColor();

    Color color = style->visitedDependentColor(CSSPropertyBackgroundColor);
    return color.rgb();
}

RGBA32 AXLayoutObject::color() const
{
    if (!m_layoutObject || isColorWell())
        return AXNodeObject::color();

    const ComputedStyle* style = m_layoutObject->style();
    if (!style)
        return AXNodeObject::color();

    Color color = style->visitedDependentColor(CSSPropertyColor);
    return color.rgb();
}

// Font size is in pixels.
float AXLayoutObject::fontSize() const
{
    if (!m_layoutObject)
        return AXNodeObject::fontSize();

    const ComputedStyle* style = m_layoutObject->style();
    if (!style)
        return AXNodeObject::fontSize();

    return style->computedFontSize();
}

AccessibilityOrientation AXLayoutObject::orientation() const
{
    const AtomicString& ariaOrientation = getAttribute(aria_orientationAttr);
    AccessibilityOrientation axorientation = AccessibilityOrientationUndefined;

    // For TreeGridRole, roleValue() can't be compared because its overridden
    // in AXTable::roleValue()
    if (ariaRoleAttribute() == TreeGridRole) {
        if (equalIgnoringCase(ariaOrientation, "horizontal"))
            axorientation = AccessibilityOrientationHorizontal;
        if (equalIgnoringCase(ariaOrientation, "vertical"))
            axorientation = AccessibilityOrientationVertical;
        return axorientation;
    }

    switch (roleValue()) {
    case ComboBoxRole:
    case ListBoxRole:
    case MenuRole:
    case ScrollBarRole:
    case TreeRole:
        axorientation = AccessibilityOrientationVertical;
        break;
    case MenuBarRole:
    case SliderRole:
    case SplitterRole:
    case TabListRole:
    case ToolbarRole:
        axorientation = AccessibilityOrientationHorizontal;
        break;
    case RadioGroupRole:
        break;
    default:
        return AXObject::orientation();
    }

    if (equalIgnoringCase(ariaOrientation, "horizontal"))
        axorientation = AccessibilityOrientationHorizontal;
    if (equalIgnoringCase(ariaOrientation, "vertical"))
        axorientation = AccessibilityOrientationVertical;
    return axorientation;
}

String AXLayoutObject::text() const
{
    if (isPasswordFieldAndShouldHideValue()) {
        if (!m_layoutObject)
            return String();

        const ComputedStyle* style = m_layoutObject->style();
        if (!style)
            return String();

        unsigned unmaskedTextLength = AXNodeObject::text().length();
        if (!unmaskedTextLength)
            return String();

        UChar maskCharacter = 0;
        switch (style->textSecurity()) {
        case TSNONE:
            break; // Fall through to the non-password branch.
        case TSDISC:
            maskCharacter = bulletCharacter;
            break;
        case TSCIRCLE:
            maskCharacter = whiteBulletCharacter;
            break;
        case TSSQUARE:
            maskCharacter = blackSquareCharacter;
            break;
        }
        if (maskCharacter) {
            StringBuilder maskedText;
            maskedText.reserveCapacity(unmaskedTextLength);
            for (unsigned i = 0; i < unmaskedTextLength; ++i)
                maskedText.append(maskCharacter);
            return maskedText.toString();
        }
    }

    return AXNodeObject::text();
}

AccessibilityTextDirection AXLayoutObject::textDirection() const
{
    if (!m_layoutObject)
        return AXNodeObject::textDirection();

    const ComputedStyle* style = m_layoutObject->style();
    if (!style)
        return AXNodeObject::textDirection();

    if (style->isHorizontalWritingMode()) {
        switch (style->direction()) {
        case LTR:
            return AccessibilityTextDirectionLTR;
        case RTL:
            return AccessibilityTextDirectionRTL;
        }
    } else {
        switch (style->direction()) {
        case LTR:
            return AccessibilityTextDirectionTTB;
        case RTL:
            return AccessibilityTextDirectionBTT;
        }
    }

    return AXNodeObject::textDirection();
}

int AXLayoutObject::textLength() const
{
    if (!isTextControl())
        return -1;

    return text().length();
}

TextStyle AXLayoutObject::textStyle() const
{
    if (!m_layoutObject)
        return AXNodeObject::textStyle();

    const ComputedStyle* style = m_layoutObject->style();
    if (!style)
        return AXNodeObject::textStyle();

    unsigned textStyle = TextStyleNone;
    if (style->fontWeight() == FontWeightBold)
        textStyle |= TextStyleBold;
    if (style->fontDescription().style() == FontStyleItalic)
        textStyle |= TextStyleItalic;
    if (style->textDecoration() == TextDecorationUnderline)
        textStyle |= TextStyleUnderline;
    if (style->textDecoration() == TextDecorationLineThrough)
        textStyle |= TextStyleLineThrough;

    return static_cast<TextStyle>(textStyle);
}

KURL AXLayoutObject::url() const
{
    if (isAnchor() && isHTMLAnchorElement(m_layoutObject->node())) {
        if (HTMLAnchorElement* anchor = toHTMLAnchorElement(anchorElement()))
            return anchor->href();
    }

    if (isWebArea())
        return m_layoutObject->document().url();

    if (isImage() && isHTMLImageElement(m_layoutObject->node()))
        return toHTMLImageElement(*m_layoutObject->node()).src();

    if (isInputImage())
        return toHTMLInputElement(m_layoutObject->node())->src();

    return KURL();
}

//
// Inline text boxes.
//

void AXLayoutObject::loadInlineTextBoxes()
{
    if (!layoutObject() || !layoutObject()->isText())
        return;

    clearChildren();
    addInlineTextBoxChildren(true);
}

AXObject* AXLayoutObject::nextOnLine() const
{
    if (!m_layoutObject)
        return 0;

    InlineBox* inlineBox;
    if (m_layoutObject->isLayoutInline())
        inlineBox = toLayoutInline(m_layoutObject)->lastLineBox();
    else if (m_layoutObject->isText())
        inlineBox = toLayoutText(m_layoutObject)->lastTextBox();
    else
        return 0;

    AXObject* result = 0;
    for (InlineBox* next = inlineBox->nextOnLine(); next; next = next->nextOnLine()) {
        LayoutObject* layoutObject = &next->layoutObject();
        result = axObjectCache().getOrCreate(layoutObject);
        if (result)
            break;
    }

    // A static text node might span multiple lines. Try to return the first inline
    // text box within that static text if possible.
    if (result && result->roleValue() == StaticTextRole && result->children().size())
        result = result->children()[0].get();

    return result;
}

AXObject* AXLayoutObject::previousOnLine() const
{
    if (!m_layoutObject)
        return 0;

    InlineBox* inlineBox;
    if (m_layoutObject->isLayoutInline())
        inlineBox = toLayoutInline(m_layoutObject)->firstLineBox();
    else if (m_layoutObject->isText())
        inlineBox = toLayoutText(m_layoutObject)->firstTextBox();
    else
        return 0;

    AXObject* result = 0;
    for (InlineBox* prev = inlineBox->prevOnLine(); prev; prev = prev->prevOnLine()) {
        LayoutObject* layoutObject = &prev->layoutObject();
        result = axObjectCache().getOrCreate(layoutObject);
        if (result)
            break;
    }

    // A static text node might span multiple lines. Try to return the last inline
    // text box within that static text if possible.
    if (result && result->roleValue() == StaticTextRole && result->children().size())
        result = result->children()[result->children().size() - 1].get();

    return result;
}

//
// Properties of interactive elements.
//

static String queryString(WebLocalizedString::Name name)
{
    return Locale::defaultLocale().queryString(name);
}

String AXLayoutObject::actionVerb() const
{
    switch (roleValue()) {
    case ButtonRole:
    case ToggleButtonRole:
        return queryString(WebLocalizedString::AXButtonActionVerb);
    case TextFieldRole:
        return queryString(WebLocalizedString::AXTextFieldActionVerb);
    case RadioButtonRole:
        return queryString(WebLocalizedString::AXRadioButtonActionVerb);
    case CheckBoxRole:
    case SwitchRole:
        return queryString(isChecked() ? WebLocalizedString::AXCheckedCheckBoxActionVerb : WebLocalizedString::AXUncheckedCheckBoxActionVerb);
    case LinkRole:
        return queryString(WebLocalizedString::AXLinkActionVerb);
    default:
        return emptyString();
    }
}

String AXLayoutObject::stringValue() const
{
    if (!m_layoutObject)
        return String();

    LayoutBoxModelObject* cssBox = layoutBoxModelObject();

    if (ariaRoleAttribute() == StaticTextRole) {
        String staticText = text();
        if (!staticText.length())
            staticText = deprecatedTextUnderElement(TextUnderElementAll);
        return staticText;
    }

    if (m_layoutObject->isText())
        return deprecatedTextUnderElement(TextUnderElementAll);

    if (cssBox && cssBox->isMenuList()) {
        // LayoutMenuList will go straight to the text() of its selected item.
        // This has to be overridden in the case where the selected item has an ARIA label.
        HTMLSelectElement* selectElement = toHTMLSelectElement(m_layoutObject->node());
        int selectedIndex = selectElement->selectedIndex();
        const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement->listItems();
        if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < listItems.size()) {
            const AtomicString& overriddenDescription = listItems[selectedIndex]->fastGetAttribute(aria_labelAttr);
            if (!overriddenDescription.isNull())
                return overriddenDescription;
        }
        return toLayoutMenuList(m_layoutObject)->text();
    }

    if (m_layoutObject->isListMarker())
        return toLayoutListMarker(m_layoutObject)->text();

    if (isWebArea()) {
        // FIXME: Why would a layoutObject exist when the Document isn't attached to a frame?
        if (m_layoutObject->frame())
            return String();

        ASSERT_NOT_REACHED();
    }

    if (isTextControl())
        return text();

    if (m_layoutObject->isFileUploadControl())
        return toLayoutFileUploadControl(m_layoutObject)->fileTextValue();

    // Handle other HTML input elements that aren't text controls, like date and time
    // controls, by returning the string value, with the exception of checkboxes
    // and radio buttons (which would return "on").
    if (node() && isHTMLInputElement(node())) {
        HTMLInputElement* input = toHTMLInputElement(node());
        if (input->type() != InputTypeNames::checkbox && input->type() != InputTypeNames::radio)
            return input->value();
    }

    // FIXME: We might need to implement a value here for more types
    // FIXME: It would be better not to advertise a value at all for the types for which we don't implement one;
    // this would require subclassing or making accessibilityAttributeNames do something other than return a
    // single static array.
    return String();
}

//
// ARIA attributes.
//

AXObject* AXLayoutObject::activeDescendant() const
{
    if (!m_layoutObject)
        return 0;

    if (m_layoutObject->node() && !m_layoutObject->node()->isElementNode())
        return 0;

    Element* element = toElement(m_layoutObject->node());
    if (!element)
        return 0;

    const AtomicString& activeDescendantAttrStr = element->getAttribute(aria_activedescendantAttr);
    if (activeDescendantAttrStr.isNull() || activeDescendantAttrStr.isEmpty())
        return 0;

    Element* target = element->treeScope().getElementById(activeDescendantAttrStr);
    if (!target)
        return 0;

    AXObject* obj = axObjectCache().getOrCreate(target);

    // An activedescendant is only useful if it has a layoutObject, because that's what's needed to post the notification.
    if (obj && obj->isAXLayoutObject())
        return obj;

    return 0;
}

void AXLayoutObject::ariaFlowToElements(AccessibilityChildrenVector& flowTo) const
{
    accessibilityChildrenFromAttribute(aria_flowtoAttr, flowTo);
}

void AXLayoutObject::ariaControlsElements(AccessibilityChildrenVector& controls) const
{
    accessibilityChildrenFromAttribute(aria_controlsAttr, controls);
}

void AXLayoutObject::deprecatedAriaDescribedbyElements(AccessibilityChildrenVector& describedby) const
{
    accessibilityChildrenFromAttribute(aria_describedbyAttr, describedby);
}

void AXLayoutObject::deprecatedAriaLabelledbyElements(AccessibilityChildrenVector& labelledby) const
{
    accessibilityChildrenFromAttribute(aria_labelledbyAttr, labelledby);
}

void AXLayoutObject::ariaOwnsElements(AccessibilityChildrenVector& owns) const
{
    accessibilityChildrenFromAttribute(aria_ownsAttr, owns);
}

bool AXLayoutObject::ariaHasPopup() const
{
    return elementAttributeValue(aria_haspopupAttr);
}

bool AXLayoutObject::ariaRoleHasPresentationalChildren() const
{
    switch (m_ariaRole) {
    case ButtonRole:
    case SliderRole:
    case ImageRole:
    case ProgressIndicatorRole:
    case SpinButtonRole:
    // case SeparatorRole:
        return true;
    default:
        return false;
    }
}

AXObject* AXLayoutObject::ancestorForWhichThisIsAPresentationalChild() const
{
    // Walk the parent chain looking for a parent that has presentational children
    AXObject* parent = parentObject();
    while (parent) {
        if (parent->ariaRoleHasPresentationalChildren())
            break;

        // The descendants of a AXMenuList that are AXLayoutObjects are all
        // presentational. (The real descendants are a AXMenuListPopup and
        // AXMenuListOptions, which are not AXLayoutObjects.)
        if (parent->isMenuList())
            break;

        parent = parent->parentObjectIfExists();
    }

    return parent;
}

bool AXLayoutObject::shouldFocusActiveDescendant() const
{
    switch (ariaRoleAttribute()) {
    case ComboBoxRole:
    case GridRole:
    case GroupRole:
    case ListBoxRole:
    case MenuRole:
    case MenuBarRole:
    case OutlineRole:
    case PopUpButtonRole:
    case ProgressIndicatorRole:
    case RadioGroupRole:
    case RowRole:
    case TabListRole:
    case ToolbarRole:
    case TreeRole:
    case TreeGridRole:
        return true;
    default:
        return false;
    }
}

bool AXLayoutObject::supportsARIADragging() const
{
    const AtomicString& grabbed = getAttribute(aria_grabbedAttr);
    return equalIgnoringCase(grabbed, "true") || equalIgnoringCase(grabbed, "false");
}

bool AXLayoutObject::supportsARIADropping() const
{
    const AtomicString& dropEffect = getAttribute(aria_dropeffectAttr);
    return !dropEffect.isEmpty();
}

bool AXLayoutObject::supportsARIAFlowTo() const
{
    return !getAttribute(aria_flowtoAttr).isEmpty();
}

bool AXLayoutObject::supportsARIAOwns() const
{
    if (!m_layoutObject)
        return false;
    const AtomicString& ariaOwns = getAttribute(aria_ownsAttr);

    return !ariaOwns.isEmpty();
}

//
// ARIA live-region features.
//

const AtomicString& AXLayoutObject::liveRegionStatus() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, liveRegionStatusAssertive, ("assertive", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, liveRegionStatusPolite, ("polite", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, liveRegionStatusOff, ("off", AtomicString::ConstructFromLiteral));

    const AtomicString& liveRegionStatus = getAttribute(aria_liveAttr);
    // These roles have implicit live region status.
    if (liveRegionStatus.isEmpty()) {
        switch (roleValue()) {
        case AlertDialogRole:
        case AlertRole:
            return liveRegionStatusAssertive;
        case LogRole:
        case StatusRole:
            return liveRegionStatusPolite;
        case TimerRole:
        case MarqueeRole:
            return liveRegionStatusOff;
        default:
            break;
        }
    }

    return liveRegionStatus;
}

const AtomicString& AXLayoutObject::liveRegionRelevant() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, defaultLiveRegionRelevant, ("additions text", AtomicString::ConstructFromLiteral));
    const AtomicString& relevant = getAttribute(aria_relevantAttr);

    // Default aria-relevant = "additions text".
    if (relevant.isEmpty())
        return defaultLiveRegionRelevant;

    return relevant;
}

bool AXLayoutObject::liveRegionAtomic() const
{
    // ARIA role status should have implicit aria-atomic value of true.
    if (getAttribute(aria_atomicAttr).isEmpty() && roleValue() == StatusRole)
        return true;
    return elementAttributeValue(aria_atomicAttr);
}

bool AXLayoutObject::liveRegionBusy() const
{
    return elementAttributeValue(aria_busyAttr);
}

//
// Accessibility Text.
//

String AXLayoutObject::deprecatedTextUnderElement(TextUnderElementMode mode) const
{
    if (!m_layoutObject)
        return String();

    if (m_layoutObject->isBR())
        return String("\n");

    if (m_layoutObject->isFileUploadControl())
        return toLayoutFileUploadControl(m_layoutObject)->buttonValue();

    if (m_layoutObject->isText()) {
        LayoutText* layoutText = toLayoutText(m_layoutObject);
        String result = layoutText->plainText();
        if (!result.isEmpty() || layoutText->isAllCollapsibleWhitespace())
            return result;
        return layoutText->text();
    }

    return AXNodeObject::deprecatedTextUnderElement(mode);
}

//
// Accessibility Text - (To be deprecated).
//

String AXLayoutObject::deprecatedHelpText() const
{
    if (!m_layoutObject)
        return String();

    const AtomicString& ariaHelp = getAttribute(aria_helpAttr);
    if (!ariaHelp.isEmpty())
        return ariaHelp;

    String describedBy = ariaDescribedByAttribute();
    if (!describedBy.isEmpty())
        return describedBy;

    String description = deprecatedAccessibilityDescription();
    for (LayoutObject* curr = m_layoutObject; curr; curr = curr->parent()) {
        if (curr->node() && curr->node()->isHTMLElement()) {
            const AtomicString& summary = toElement(curr->node())->getAttribute(summaryAttr);
            if (!summary.isEmpty())
                return summary;

            // The title attribute should be used as help text unless it is already being used as descriptive text.
            const AtomicString& title = toElement(curr->node())->getAttribute(titleAttr);
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

//
// Position and size.
//

void AXLayoutObject::checkCachedElementRect() const
{
    if (m_cachedElementRectDirty)
        return;

    if (!m_layoutObject)
        return;

    if (!m_layoutObject->isBox())
        return;

    bool dirty = false;
    LayoutBox* box = toLayoutBox(m_layoutObject);
    if (box->frameRect() != m_cachedFrameRect)
        dirty = true;

    if (box->canBeScrolledAndHasScrollableArea()) {
        ScrollableArea* scrollableArea = box->scrollableArea();
        if (scrollableArea && scrollableArea->scrollPosition() != m_cachedScrollPosition)
            dirty = true;
    }

    if (dirty)
        markCachedElementRectDirty();
}

void AXLayoutObject::updateCachedElementRect() const
{
    if (!m_cachedElementRectDirty)
        return;

    if (!m_layoutObject)
        return;

    if (!m_layoutObject->isBox())
        return;

    LayoutBox* box = toLayoutBox(m_layoutObject);
    m_cachedFrameRect = box->frameRect();

    if (box->canBeScrolledAndHasScrollableArea()) {
        ScrollableArea* scrollableArea = box->scrollableArea();
        if (scrollableArea)
            m_cachedScrollPosition = scrollableArea->scrollPosition();
    }

    m_cachedElementRect = computeElementRect();
    m_cachedElementRectDirty = false;
}

void AXLayoutObject::markCachedElementRectDirty() const
{
    if (m_cachedElementRectDirty)
        return;

    // Marks children recursively, if this element changed.
    m_cachedElementRectDirty = true;
    for (AXObject* child = firstChild(); child; child = child->nextSibling())
        child->markCachedElementRectDirty();
}

IntPoint AXLayoutObject::clickPoint()
{
    // Headings are usually much wider than their textual content. If the mid point is used, often it can be wrong.
    if (isHeading() && children().size() == 1)
        return children()[0]->clickPoint();

    // use the default position unless this is an editable web area, in which case we use the selection bounds.
    if (!isWebArea() || isReadOnly())
        return AXObject::clickPoint();

    IntRect bounds = pixelSnappedIntRect(elementRect());
    return IntPoint(bounds.x() + (bounds.width() / 2), bounds.y() - (bounds.height() / 2));
}

//
// Hit testing.
//

AXObject* AXLayoutObject::accessibilityHitTest(const IntPoint& point) const
{
    if (!m_layoutObject || !m_layoutObject->hasLayer())
        return 0;

    DeprecatedPaintLayer* layer = toLayoutBox(m_layoutObject)->layer();

    HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active);
    HitTestResult hitTestResult = HitTestResult(request, point);
    layer->hitTest(hitTestResult);
    if (!hitTestResult.innerNode())
        return 0;

    Node* node = hitTestResult.innerNode();

    // Allow the hit test to return media control buttons.
    if (node->isInShadowTree() && (!isHTMLInputElement(*node) || !node->isMediaControlElement()))
        node = node->shadowHost();

    if (isHTMLAreaElement(node))
        return accessibilityImageMapHitTest(toHTMLAreaElement(node), point);

    if (isHTMLOptionElement(node))
        node = toHTMLOptionElement(*node).ownerSelectElement();

    LayoutObject* obj = node->layoutObject();
    if (!obj)
        return 0;

    AXObject* result = axObjectCache().getOrCreate(obj);
    result->updateChildrenIfNecessary();

    // Allow the element to perform any hit-testing it might need to do to reach non-layout children.
    result = result->elementAccessibilityHitTest(point);

    if (result && result->accessibilityIsIgnored()) {
        // If this element is the label of a control, a hit test should return the control.
        if (result->isAXLayoutObject()) {
            AXObject* controlObject = toAXLayoutObject(result)->correspondingControlForLabelElement();
            if (controlObject && !controlObject->deprecatedExposesTitleUIElement())
                return controlObject;
        }

        result = result->parentObjectUnignored();
    }

    return result;
}

AXObject* AXLayoutObject::elementAccessibilityHitTest(const IntPoint& point) const
{
    if (isSVGImage())
        return remoteSVGElementHitTest(point);

    return AXObject::elementAccessibilityHitTest(point);
}

//
// High-level accessibility tree access.
//

AXObject* AXLayoutObject::computeParent() const
{
    if (!m_layoutObject)
        return 0;

    if (ariaRoleAttribute() == MenuBarRole)
        return axObjectCache().getOrCreate(m_layoutObject->parent());

    // menuButton and its corresponding menu are DOM siblings, but Accessibility needs them to be parent/child
    if (ariaRoleAttribute() == MenuRole) {
        AXObject* parent = menuButtonForMenu();
        if (parent)
            return parent;
    }

    LayoutObject* parentObj = layoutParentObject();
    if (parentObj)
        return axObjectCache().getOrCreate(parentObj);

    // WebArea's parent should be the scroll view containing it.
    if (isWebArea())
        return axObjectCache().getOrCreate(m_layoutObject->frame()->view());

    return 0;
}

AXObject* AXLayoutObject::computeParentIfExists() const
{
    if (!m_layoutObject)
        return 0;

    if (ariaRoleAttribute() == MenuBarRole)
        return axObjectCache().get(m_layoutObject->parent());

    // menuButton and its corresponding menu are DOM siblings, but Accessibility needs them to be parent/child
    if (ariaRoleAttribute() == MenuRole) {
        AXObject* parent = menuButtonForMenu();
        if (parent)
            return parent;
    }

    LayoutObject* parentObj = layoutParentObject();
    if (parentObj)
        return axObjectCache().get(parentObj);

    // WebArea's parent should be the scroll view containing it.
    if (isWebArea())
        return axObjectCache().get(m_layoutObject->frame()->view());

    return 0;
}

//
// Low-level accessibility tree exploration, only for use within the accessibility module.
//

AXObject* AXLayoutObject::firstChild() const
{
    if (!m_layoutObject)
        return 0;

    LayoutObject* firstChild = firstChildConsideringContinuation(m_layoutObject);

    if (!firstChild)
        return 0;

    return axObjectCache().getOrCreate(firstChild);
}

AXObject* AXLayoutObject::nextSibling() const
{
    if (!m_layoutObject)
        return 0;

    LayoutObject* nextSibling = 0;

    LayoutInline* inlineContinuation = m_layoutObject->isLayoutBlock() ? toLayoutBlock(m_layoutObject)->inlineElementContinuation() : 0;
    if (inlineContinuation) {
        // Case 1: node is a block and has an inline continuation. Next sibling is the inline continuation's first child.
        nextSibling = firstChildConsideringContinuation(inlineContinuation);
    } else if (m_layoutObject->isAnonymousBlock() && lastChildHasContinuation(m_layoutObject)) {
        // Case 2: Anonymous block parent of the start of a continuation - skip all the way to
        // after the parent of the end, since everything in between will be linked up via the continuation.
        LayoutObject* lastParent = endOfContinuations(toLayoutBlock(m_layoutObject)->lastChild())->parent();
        while (lastChildHasContinuation(lastParent))
            lastParent = endOfContinuations(lastParent->slowLastChild())->parent();
        nextSibling = lastParent->nextSibling();
    } else if (LayoutObject* ns = m_layoutObject->nextSibling()) {
        // Case 3: node has an actual next sibling
        nextSibling = ns;
    } else if (isInlineWithContinuation(m_layoutObject)) {
        // Case 4: node is an inline with a continuation. Next sibling is the next sibling of the end
        // of the continuation chain.
        nextSibling = endOfContinuations(m_layoutObject)->nextSibling();
    } else if (isInlineWithContinuation(m_layoutObject->parent())) {
        // Case 5: node has no next sibling, and its parent is an inline with a continuation.
        LayoutObject* continuation = toLayoutInline(m_layoutObject->parent())->continuation();

        if (continuation->isLayoutBlock()) {
            // Case 5a: continuation is a block - in this case the block itself is the next sibling.
            nextSibling = continuation;
        } else {
            // Case 5b: continuation is an inline - in this case the inline's first child is the next sibling.
            nextSibling = firstChildConsideringContinuation(continuation);
        }
    }

    if (!nextSibling)
        return 0;

    return axObjectCache().getOrCreate(nextSibling);
}

void AXLayoutObject::addChildren()
{
    // If the need to add more children in addition to existing children arises,
    // childrenChanged should have been called, leaving the object with no children.
    ASSERT(!m_haveChildren);

    m_haveChildren = true;

    if (!canHaveChildren())
        return;

    Vector<AXObject*> ownedChildren;
    computeAriaOwnsChildren(ownedChildren);

    for (RefPtrWillBeRawPtr<AXObject> obj = firstChild(); obj; obj = obj->nextSibling()) {
        if (!axObjectCache().isAriaOwned(obj.get()))
            addChild(obj.get());
    }

    addHiddenChildren();
    addAttachmentChildren();
    addPopupChildren();
    addImageMapChildren();
    addTextFieldChildren();
    addCanvasChildren();
    addRemoteSVGChildren();
    addInlineTextBoxChildren(false);

    for (const auto& child : m_children) {
        if (!child->cachedParentObject())
            child->setParent(this);
    }

    for (const auto& ownedChild : ownedChildren)
        addChild(ownedChild);
}

bool AXLayoutObject::canHaveChildren() const
{
    if (!m_layoutObject)
        return false;

    return AXNodeObject::canHaveChildren();
}

void AXLayoutObject::updateChildrenIfNecessary()
{
    if (needsToUpdateChildren())
        clearChildren();

    AXObject::updateChildrenIfNecessary();
}

void AXLayoutObject::clearChildren()
{
    AXObject::clearChildren();
    m_childrenDirty = false;
}

//
// Properties of the object's owning document or page.
//

double AXLayoutObject::estimatedLoadingProgress() const
{
    if (!m_layoutObject)
        return 0;

    if (isLoaded())
        return 1.0;

    if (LocalFrame* frame = m_layoutObject->document().frame())
        return frame->loader().progress().estimatedProgress();
    return 0;
}

//
// DOM and layout tree access.
//

Node* AXLayoutObject::node() const
{
    return m_layoutObject ? m_layoutObject->node() : 0;
}

Document* AXLayoutObject::document() const
{
    if (!m_layoutObject)
        return 0;
    return &m_layoutObject->document();
}

FrameView* AXLayoutObject::documentFrameView() const
{
    if (!m_layoutObject)
        return 0;

    // this is the LayoutObject's Document's LocalFrame's FrameView
    return m_layoutObject->document().view();
}

Element* AXLayoutObject::anchorElement() const
{
    if (!m_layoutObject)
        return 0;

    AXObjectCacheImpl& cache = axObjectCache();
    LayoutObject* currLayoutObject;

    // Search up the layout tree for a LayoutObject with a DOM node. Defer to an earlier continuation, though.
    for (currLayoutObject = m_layoutObject; currLayoutObject && !currLayoutObject->node(); currLayoutObject = currLayoutObject->parent()) {
        if (currLayoutObject->isAnonymousBlock()) {
            LayoutObject* continuation = toLayoutBlock(currLayoutObject)->continuation();
            if (continuation)
                return cache.getOrCreate(continuation)->anchorElement();
        }
    }

    // bail if none found
    if (!currLayoutObject)
        return 0;

    // search up the DOM tree for an anchor element
    // NOTE: this assumes that any non-image with an anchor is an HTMLAnchorElement
    Node* node = currLayoutObject->node();
    for ( ; node; node = node->parentNode()) {
        if (isHTMLAnchorElement(*node) || (node->layoutObject() && cache.getOrCreate(node->layoutObject())->isAnchor()))
            return toElement(node);
    }

    return 0;
}

Widget* AXLayoutObject::widgetForAttachmentView() const
{
    if (!isAttachment())
        return 0;
    return toLayoutPart(m_layoutObject)->widget();
}

//
// Selected text.
//

AXObject::PlainTextRange AXLayoutObject::selectedTextRange() const
{
    if (!isTextControl())
        return PlainTextRange();

    if (m_layoutObject->isTextControl()) {
        HTMLTextFormControlElement* textControl = toLayoutTextControl(m_layoutObject)->textFormControlElement();
        return PlainTextRange(textControl->selectionStart(), textControl->selectionEnd() - textControl->selectionStart());
    }

    return visibleSelectionUnderObject();
}

VisibleSelection AXLayoutObject::selection() const
{
    return m_layoutObject->frame()->selection().selection();
}

//
// Modify or take an action on an object.
//

void AXLayoutObject::setSelectedTextRange(const PlainTextRange& range)
{
    if (m_layoutObject->isTextControl()) {
        HTMLTextFormControlElement* textControl = toLayoutTextControl(m_layoutObject)->textFormControlElement();
        textControl->setSelectionRange(range.start, range.start + range.length, SelectionHasNoDirection, NotDispatchSelectEvent);
        return;
    }

    Document& document = m_layoutObject->document();
    LocalFrame* frame = document.frame();
    if (!frame)
        return;
    Node* node = m_layoutObject->node();
    frame->selection().setSelection(VisibleSelection(Position(node, range.start),
        Position(node, range.start + range.length), DOWNSTREAM));
}

void AXLayoutObject::setValue(const String& string)
{
    if (!node() || !node()->isElementNode())
        return;
    if (!m_layoutObject || !m_layoutObject->isBoxModelObject())
        return;

    LayoutBoxModelObject* layoutObject = toLayoutBoxModelObject(m_layoutObject);
    if (layoutObject->isTextField() && isHTMLInputElement(*node()))
        toHTMLInputElement(*node()).setValue(string);
    else if (layoutObject->isTextArea() && isHTMLTextAreaElement(*node()))
        toHTMLTextAreaElement(*node()).setValue(string);
}

//
// Notifications that this object may have changed.
//

void AXLayoutObject::handleActiveDescendantChanged()
{
    Element* element = toElement(layoutObject()->node());
    if (!element)
        return;
    Document& doc = layoutObject()->document();
    if (!doc.frame()->selection().isFocusedAndActive() || doc.focusedElement() != element)
        return;
    AXLayoutObject* activedescendant = toAXLayoutObject(activeDescendant());

    if (activedescendant && shouldNotifyActiveDescendant())
        toAXObjectCacheImpl(doc.axObjectCache())->postNotification(m_layoutObject, AXObjectCacheImpl::AXActiveDescendantChanged);
}

void AXLayoutObject::handleAriaExpandedChanged()
{
    // Find if a parent of this object should handle aria-expanded changes.
    AXObject* containerParent = this->parentObject();
    while (containerParent) {
        bool foundParent = false;

        switch (containerParent->roleValue()) {
        case TreeRole:
        case TreeGridRole:
        case GridRole:
        case TableRole:
            foundParent = true;
            break;
        default:
            break;
        }

        if (foundParent)
            break;

        containerParent = containerParent->parentObject();
    }

    // Post that the row count changed.
    if (containerParent)
        axObjectCache().postNotification(containerParent, AXObjectCacheImpl::AXRowCountChanged);

    // Post that the specific row either collapsed or expanded.
    AccessibilityExpanded expanded = isExpanded();
    if (!expanded)
        return;

    if (roleValue() == RowRole || roleValue() == TreeItemRole) {
        AXObjectCacheImpl::AXNotification notification = AXObjectCacheImpl::AXRowExpanded;
        if (expanded == ExpandedCollapsed)
            notification = AXObjectCacheImpl::AXRowCollapsed;

        axObjectCache().postNotification(this, notification);
    }
}

void AXLayoutObject::textChanged()
{
    if (!m_layoutObject)
        return;

    Settings* settings = document()->settings();
    if (settings && settings->inlineTextBoxAccessibilityEnabled() && roleValue() == StaticTextRole)
        childrenChanged();

    // Do this last - AXNodeObject::textChanged posts live region announcements,
    // and we should update the inline text boxes first.
    AXNodeObject::textChanged();
}

//
// Text metrics. Most of these should be deprecated, needs major cleanup.
//

// NOTE: Consider providing this utility method as AX API
int AXLayoutObject::index(const VisiblePosition& position) const
{
    if (position.isNull() || !isTextControl())
        return -1;

    if (layoutObjectContainsPosition(m_layoutObject, position.deepEquivalent()))
        return indexForVisiblePosition(position);

    return -1;
}

VisiblePosition AXLayoutObject::visiblePositionForIndex(int index) const
{
    if (!m_layoutObject)
        return VisiblePosition();

    if (m_layoutObject->isTextControl())
        return toLayoutTextControl(m_layoutObject)->textFormControlElement()->visiblePositionForIndex(index);

    if (!allowsTextRanges() && !m_layoutObject->isText())
        return VisiblePosition();

    Node* node = m_layoutObject->node();
    if (!node)
        return VisiblePosition();

    if (index <= 0)
        return VisiblePosition(firstPositionInOrBeforeNode(node), DOWNSTREAM);

    Position start, end;
    bool selected = Range::selectNodeContents(node, start, end);
    if (!selected)
        return VisiblePosition();

    CharacterIterator it(start, end);
    it.advance(index - 1);
    return VisiblePosition(Position(it.currentContainer(), it.endOffset()), UPSTREAM);
}

int AXLayoutObject::indexForVisiblePosition(const VisiblePosition& pos) const
{
    if (m_layoutObject->isTextControl()) {
        HTMLTextFormControlElement* textControl = toLayoutTextControl(m_layoutObject)->textFormControlElement();
        return textControl->indexForVisiblePosition(pos);
    }

    if (!isTextControl())
        return 0;

    Node* node = m_layoutObject->node();
    if (!node)
        return 0;

    Position indexPosition = pos.deepEquivalent();
    if (indexPosition.isNull()
        || (highestEditableRoot(indexPosition) != node
        && highestEditableRoot(indexPosition, HasEditableAXRole) != node))
        return 0;

    RefPtrWillBeRawPtr<Range> range = Range::create(m_layoutObject->document());
    range->setStart(node, 0, IGNORE_EXCEPTION);
    range->setEnd(indexPosition, IGNORE_EXCEPTION);

    return TextIterator::rangeLength(range->startPosition(), range->endPosition());
}

void AXLayoutObject::addInlineTextBoxChildren(bool force)
{
    Settings* settings = document()->settings();
    if (!force && (!settings || !settings->inlineTextBoxAccessibilityEnabled()))
        return;

    if (!layoutObject() || !layoutObject()->isText())
        return;

    if (layoutObject()->needsLayout()) {
        // If a LayoutText needs layout, its inline text boxes are either
        // nonexistent or invalid, so defer until the layout happens and
        // the layoutObject calls AXObjectCacheImpl::inlineTextBoxesUpdated.
        return;
    }

    LayoutText* layoutText = toLayoutText(layoutObject());
    for (RefPtr<AbstractInlineTextBox> box = layoutText->firstAbstractInlineTextBox(); box.get(); box = box->nextInlineTextBox()) {
        AXObject* axObject = axObjectCache().getOrCreate(box.get());
        if (!axObject->accessibilityIsIgnored())
            m_children.append(axObject);
    }
}

void AXLayoutObject::lineBreaks(Vector<int>& lineBreaks) const
{
    if (!isTextControl())
        return;

    VisiblePosition visiblePos = visiblePositionForIndex(0);
    VisiblePosition prevVisiblePos = visiblePos;
    visiblePos = nextLinePosition(visiblePos, 0, HasEditableAXRole);
    // nextLinePosition moves to the end of the current line when there are
    // no more lines.
    while (visiblePos.isNotNull() && !inSameLine(prevVisiblePos, visiblePos)) {
        lineBreaks.append(indexForVisiblePosition(visiblePos));
        prevVisiblePos = visiblePos;
        visiblePos = nextLinePosition(visiblePos, 0, HasEditableAXRole);
    }
}

//
// Private.
//

AXObject* AXLayoutObject::treeAncestorDisallowingChild() const
{
    // Determine if this is in a tree. If so, we apply special behavior to make it work like an AXOutline.
    AXObject* axObj = parentObject();
    AXObject* treeAncestor = 0;
    while (axObj) {
        if (axObj->isTree()) {
            treeAncestor = axObj;
            break;
        }
        axObj = axObj->parentObject();
    }

    // If the object is in a tree, only tree items should be exposed (and the children of tree items).
    if (treeAncestor) {
        AccessibilityRole role = roleValue();
        if (role != TreeItemRole && role != StaticTextRole)
            return treeAncestor;
    }
    return 0;
}

void AXLayoutObject::ariaListboxSelectedChildren(AccessibilityChildrenVector& result)
{
    bool isMulti = isMultiSelectable();

    for (const auto& child : children()) {
        // Every child should have aria-role option, and if so, check for selected attribute/state.
        if (child->isSelected() && child->ariaRoleAttribute() == ListBoxOptionRole) {
            result.append(child);
            if (!isMulti)
                return;
        }
    }
}

AXObject::PlainTextRange AXLayoutObject::visibleSelectionUnderObject() const
{
    Node* node = m_layoutObject->node();
    if (!node)
        return PlainTextRange();

    VisibleSelection visibleSelection = selection();
    RefPtrWillBeRawPtr<Range> currentSelectionRange = visibleSelection.toNormalizedRange();
    if (!currentSelectionRange || !currentSelectionRange->intersectsNode(node, IGNORE_EXCEPTION))
        return PlainTextRange();

    int start = indexForVisiblePosition(visibleSelection.visibleStart());
    int end = indexForVisiblePosition(visibleSelection.visibleEnd());

    return PlainTextRange(start, end - start);
}

bool AXLayoutObject::nodeIsTextControl(const Node* node) const
{
    if (!node)
        return false;

    const AXObject* axObjectForNode = axObjectCache().getOrCreate(const_cast<Node*>(node));
    if (!axObjectForNode)
        return false;

    return axObjectForNode->isTextControl();
}

bool AXLayoutObject::isTabItemSelected() const
{
    if (!isTabItem() || !m_layoutObject)
        return false;

    Node* node = m_layoutObject->node();
    if (!node || !node->isElementNode())
        return false;

    // The ARIA spec says a tab item can also be selected if it is aria-labeled by a tabpanel
    // that has keyboard focus inside of it, or if a tabpanel in its aria-controls list has KB
    // focus inside of it.
    AXObject* focusedElement = focusedUIElement();
    if (!focusedElement)
        return false;

    WillBeHeapVector<RawPtrWillBeMember<Element>> elements;
    elementsFromAttribute(elements, aria_controlsAttr);

    for (const auto& element : elements) {
        AXObject* tabPanel = axObjectCache().getOrCreate(element);

        // A tab item should only control tab panels.
        if (!tabPanel || tabPanel->roleValue() != TabPanelRole)
            continue;

        AXObject* checkFocusElement = focusedElement;
        // Check if the focused element is a descendant of the element controlled by the tab item.
        while (checkFocusElement) {
            if (tabPanel == checkFocusElement)
                return true;
            checkFocusElement = checkFocusElement->parentObject();
        }
    }

    return false;
}

AXObject* AXLayoutObject::accessibilityImageMapHitTest(HTMLAreaElement* area, const IntPoint& point) const
{
    if (!area)
        return 0;

    AXObject* parent = axObjectCache().getOrCreate(area->imageElement());
    if (!parent)
        return 0;

    for (const auto& child : parent->children()) {
        if (child->elementRect().contains(point))
            return child.get();
    }

    return 0;
}

LayoutObject* AXLayoutObject::layoutParentObject() const
{
    if (!m_layoutObject)
        return 0;

    LayoutObject* startOfConts = m_layoutObject->isLayoutBlock() ? startOfContinuations(m_layoutObject) : 0;
    if (startOfConts) {
        // Case 1: node is a block and is an inline's continuation. Parent
        // is the start of the continuation chain.
        return startOfConts;
    }

    LayoutObject* parent = m_layoutObject->parent();
    startOfConts = parent && parent->isLayoutInline() ? startOfContinuations(parent) : 0;
    if (startOfConts) {
        // Case 2: node's parent is an inline which is some node's continuation; parent is
        // the earliest node in the continuation chain.
        return startOfConts;
    }

    LayoutObject* firstChild = parent ? parent->slowFirstChild() : 0;
    if (firstChild && firstChild->node()) {
        // Case 3: The first sibling is the beginning of a continuation chain. Find the origin of that continuation.
        // Get the node's layoutObject and follow that continuation chain until the first child is found.
        for (LayoutObject* nodeLayoutFirstChild = firstChild->node()->layoutObject(); nodeLayoutFirstChild != firstChild; nodeLayoutFirstChild = firstChild->node()->layoutObject()) {
            for (LayoutObject* contsTest = nodeLayoutFirstChild; contsTest; contsTest = nextContinuation(contsTest)) {
                if (contsTest == firstChild) {
                    parent = nodeLayoutFirstChild->parent();
                    break;
                }
            }
            LayoutObject* newFirstChild = parent->slowFirstChild();
            if (firstChild == newFirstChild)
                break;
            firstChild = newFirstChild;
            if (!firstChild->node())
                break;
        }
    }

    return parent;
}

bool AXLayoutObject::isSVGImage() const
{
    return remoteSVGRootElement();
}

void AXLayoutObject::detachRemoteSVGRoot()
{
    if (AXSVGRoot* root = remoteSVGRootElement())
        root->setParent(0);
}

AXSVGRoot* AXLayoutObject::remoteSVGRootElement() const
{
    // FIXME(dmazzoni): none of this code properly handled multiple references to the same
    // remote SVG document. I'm disabling this support until it can be fixed properly.
    return 0;
}

AXObject* AXLayoutObject::remoteSVGElementHitTest(const IntPoint& point) const
{
    AXObject* remote = remoteSVGRootElement();
    if (!remote)
        return 0;

    IntSize offset = point - roundedIntPoint(elementRect().location());
    return remote->accessibilityHitTest(IntPoint(offset));
}

// The boundingBox for elements within the remote SVG element needs to be offset by its position
// within the parent page, otherwise they are in relative coordinates only.
void AXLayoutObject::offsetBoundingBoxForRemoteSVGElement(LayoutRect& rect) const
{
    for (AXObject* parent = parentObject(); parent; parent = parent->parentObject()) {
        if (parent->isAXSVGRoot()) {
            rect.moveBy(parent->parentObject()->elementRect().location());
            break;
        }
    }
}

// Hidden children are those that are not laid out or visible, but are specifically marked as aria-hidden=false,
// meaning that they should be exposed to the AX hierarchy.
void AXLayoutObject::addHiddenChildren()
{
    Node* node = this->node();
    if (!node)
        return;

    // First do a quick run through to determine if we have any hidden nodes (most often we will not).
    // If we do have hidden nodes, we need to determine where to insert them so they match DOM order as close as possible.
    bool shouldInsertHiddenNodes = false;
    for (Node& child : NodeTraversal::childrenOf(*node)) {
        if (!child.layoutObject() && isNodeAriaVisible(&child)) {
            shouldInsertHiddenNodes = true;
            break;
        }
    }

    if (!shouldInsertHiddenNodes)
        return;

    // Iterate through all of the children, including those that may have already been added, and
    // try to insert hidden nodes in the correct place in the DOM order.
    unsigned insertionIndex = 0;
    for (Node& child : NodeTraversal::childrenOf(*node)) {
        if (child.layoutObject()) {
            // Find out where the last layout sibling is located within m_children.
            if (AXObject* childObject = axObjectCache().get(child.layoutObject())) {
                if (childObject->accessibilityIsIgnored()) {
                    const auto& children = childObject->children();
                    childObject = children.size() ? children.last().get() : 0;
                }
                if (childObject)
                    insertionIndex = m_children.find(childObject) + 1;
                continue;
            }
        }

        if (!isNodeAriaVisible(&child))
            continue;

        unsigned previousSize = m_children.size();
        if (insertionIndex > previousSize)
            insertionIndex = previousSize;

        insertChild(axObjectCache().getOrCreate(&child), insertionIndex);
        insertionIndex += (m_children.size() - previousSize);
    }
}

void AXLayoutObject::addTextFieldChildren()
{
    Node* node = this->node();
    if (!isHTMLInputElement(node))
        return;

    HTMLInputElement& input = toHTMLInputElement(*node);
    Element* spinButtonElement = input.userAgentShadowRoot()->getElementById(ShadowElementNames::spinButton());
    if (!spinButtonElement || !spinButtonElement->isSpinButtonElement())
        return;

    AXSpinButton* axSpinButton = toAXSpinButton(axObjectCache().getOrCreate(SpinButtonRole));
    axSpinButton->setSpinButtonElement(toSpinButtonElement(spinButtonElement));
    axSpinButton->setParent(this);
    m_children.append(axSpinButton);
}

void AXLayoutObject::addImageMapChildren()
{
    LayoutBoxModelObject* cssBox = layoutBoxModelObject();
    if (!cssBox || !cssBox->isLayoutImage())
        return;

    HTMLMapElement* map = toLayoutImage(cssBox)->imageMap();
    if (!map)
        return;

    for (HTMLAreaElement& area : Traversal<HTMLAreaElement>::descendantsOf(*map)) {
        // add an <area> element for this child if it has a link
        if (area.isLink()) {
            AXImageMapLink* areaObject = toAXImageMapLink(axObjectCache().getOrCreate(ImageMapLinkRole));
            areaObject->setHTMLAreaElement(&area);
            areaObject->setHTMLMapElement(map);
            areaObject->setParent(this);
            ASSERT(areaObject->axObjectID() != 0);
            if (!areaObject->accessibilityIsIgnored())
                m_children.append(areaObject);
            else
                axObjectCache().remove(areaObject->axObjectID());
        }
    }
}

void AXLayoutObject::addCanvasChildren()
{
    if (!isHTMLCanvasElement(node()))
        return;

    // If it's a canvas, it won't have laid out children, but it might have accessible fallback content.
    // Clear m_haveChildren because AXNodeObject::addChildren will expect it to be false.
    ASSERT(!m_children.size());
    m_haveChildren = false;
    AXNodeObject::addChildren();
}

void AXLayoutObject::addAttachmentChildren()
{
    if (!isAttachment())
        return;

    // FrameView's need to be inserted into the AX hierarchy when encountered.
    Widget* widget = widgetForAttachmentView();
    if (!widget || !widget->isFrameView())
        return;

    AXObject* axWidget = axObjectCache().getOrCreate(widget);
    if (!axWidget->accessibilityIsIgnored())
        m_children.append(axWidget);
}

void AXLayoutObject::addPopupChildren()
{
    if (!isHTMLInputElement(node()))
        return;
    if (AXObject* axPopup = toHTMLInputElement(node())->popupRootAXObject())
        m_children.append(axPopup);
}

void AXLayoutObject::addRemoteSVGChildren()
{
    AXSVGRoot* root = remoteSVGRootElement();
    if (!root)
        return;

    root->setParent(this);

    if (root->accessibilityIsIgnored()) {
        for (const auto& child : root->children())
            m_children.append(child);
    } else {
        m_children.append(root);
    }
}

void AXLayoutObject::ariaSelectedRows(AccessibilityChildrenVector& result)
{
    // Get all the rows.
    AccessibilityChildrenVector allRows;
    if (isTree())
        ariaTreeRows(allRows);
    else if (isAXTable() && toAXTable(this)->supportsSelectedRows())
        allRows = toAXTable(this)->rows();

    // Determine which rows are selected.
    bool isMulti = isMultiSelectable();

    // Prefer active descendant over aria-selected.
    AXObject* activeDesc = activeDescendant();
    if (activeDesc && (activeDesc->isTreeItem() || activeDesc->isTableRow())) {
        result.append(activeDesc);
        if (!isMulti)
            return;
    }

    for (const auto& row : allRows) {
        if (row->isSelected()) {
            result.append(row);
            if (!isMulti)
                break;
        }
    }
}

bool AXLayoutObject::elementAttributeValue(const QualifiedName& attributeName) const
{
    if (!m_layoutObject)
        return false;

    return equalIgnoringCase(getAttribute(attributeName), "true");
}

LayoutRect AXLayoutObject::computeElementRect() const
{
    LayoutObject* obj = m_layoutObject;

    if (!obj)
        return LayoutRect();

    if (obj->node()) // If we are a continuation, we want to make sure to use the primary layoutObject.
        obj = obj->node()->layoutObject();

    // absoluteFocusRingBoundingBox will query the hierarchy below this element, which for large webpages can be very slow.
    // For a web area, which will have the most elements of any element, absoluteQuads should be used.
    // We should also use absoluteQuads for SVG elements, otherwise transforms won't be applied.

    LayoutRect result;
    if (obj->isText()) {
        Vector<FloatQuad> quads;
        toLayoutText(obj)->absoluteQuads(quads, 0, LayoutText::ClipToEllipsis);
        result = LayoutRect(boundingBoxForQuads(obj, quads));
    } else if (isWebArea() || obj->isSVGRoot()) {
        result = LayoutRect(obj->absoluteBoundingBoxRect());
    } else {
        result = LayoutRect(obj->absoluteFocusRingBoundingBoxRect());
    }

    Document* document = this->document();
    if (document && document->isSVGDocument())
        offsetBoundingBoxForRemoteSVGElement(result);
    if (document && document->frame() && document->frame()->pagePopupOwner()) {
        IntPoint popupOrigin = document->view()->contentsToScreen(IntRect()).location();
        IntPoint mainOrigin = axObjectCache().rootObject()->documentFrameView()->contentsToScreen(IntRect()).location();
        result.moveBy(IntPoint(popupOrigin - mainOrigin));
    }

    // The size of the web area should be the content size, not the clipped size.
    if (isWebArea() && obj->frame()->view())
        result.setSize(LayoutSize(obj->frame()->view()->contentsSize()));

    // Checkboxes and radio buttons include their label as part of their rect.
    if (isCheckboxOrRadio()) {
        HTMLLabelElement* label = labelForElement(toElement(m_layoutObject->node()));
        if (label && label->layoutObject()) {
            LayoutRect labelRect = axObjectCache().getOrCreate(label)->elementRect();
            result.unite(labelRect);
        }
    }

    return result;
}

} // namespace blink
