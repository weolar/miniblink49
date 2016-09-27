/*
 * This file is part of the select element layoutObject in WebCore.
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 *               2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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

#include "config.h"
#include "core/layout/LayoutMenuList.h"

#include "core/HTMLNames.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/layout/LayoutBR.h"
#include "core/layout/LayoutScrollbar.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/page/ChromeClient.h"
#include "platform/fonts/FontCache.h"
#include "platform/geometry/IntSize.h"
#include "platform/text/PlatformLocale.h"
#include <math.h>

namespace blink {

using namespace HTMLNames;

LayoutMenuList::LayoutMenuList(Element* element)
    : LayoutFlexibleBox(element)
    , m_buttonText(nullptr)
    , m_innerBlock(nullptr)
    , m_optionsChanged(true)
    , m_isEmpty(false)
    , m_hasUpdatedActiveOption(false)
    , m_popupIsVisible(false)
    , m_optionsWidth(0)
    , m_lastActiveIndex(-1)
    , m_indexToSelectOnCancel(-1)
{
    ASSERT(isHTMLSelectElement(element));
}

LayoutMenuList::~LayoutMenuList()
{
    ASSERT(!m_popup);
}

void LayoutMenuList::willBeDestroyed()
{
    if (m_popup)
        m_popup->disconnectClient();
    m_popup = nullptr;
    LayoutFlexibleBox::willBeDestroyed();
}

// FIXME: Instead of this hack we should add a ShadowRoot to <select> with no insertion point
// to prevent children from rendering.
bool LayoutMenuList::isChildAllowed(LayoutObject* object, const ComputedStyle&) const
{
    return object->isAnonymous() && !object->isLayoutFullScreen();
}

void LayoutMenuList::createInnerBlock()
{
    if (m_innerBlock) {
        ASSERT(firstChild() == m_innerBlock);
        ASSERT(!m_innerBlock->nextSibling());
        return;
    }

    // Create an anonymous block.
    ASSERT(!firstChild());
    m_innerBlock = createAnonymousBlock();

    m_buttonText = new LayoutText(&document(), StringImpl::empty());
    // We need to set the text explicitly though it was specified in the
    // constructor because LayoutText doesn't refer to the text
    // specified in the constructor in a case of re-transforming.
    m_buttonText->setStyle(mutableStyle());
    m_innerBlock->addChild(m_buttonText);

    adjustInnerStyle();
    LayoutFlexibleBox::addChild(m_innerBlock);
}

void LayoutMenuList::adjustInnerStyle()
{
    ComputedStyle& innerStyle = m_innerBlock->mutableStyleRef();
    innerStyle.setFlexGrow(1);
    innerStyle.setFlexShrink(1);
    // min-width: 0; is needed for correct shrinking.
    innerStyle.setMinWidth(Length(0, Fixed));
    // Use margin:auto instead of align-items:center to get safe centering, i.e.
    // when the content overflows, treat it the same as align-items: flex-start.
    // But we only do that for the cases where html.css would otherwise use center.
    if (style()->alignItemsPosition() == ItemPositionCenter) {
        innerStyle.setMarginTop(Length());
        innerStyle.setMarginBottom(Length());
        innerStyle.setAlignSelfPosition(ItemPositionFlexStart);
    }

    innerStyle.setPaddingLeft(Length(LayoutTheme::theme().popupInternalPaddingLeft(styleRef()), Fixed));
    innerStyle.setPaddingRight(Length(LayoutTheme::theme().popupInternalPaddingRight(styleRef()), Fixed));
    innerStyle.setPaddingTop(Length(LayoutTheme::theme().popupInternalPaddingTop(styleRef()), Fixed));
    innerStyle.setPaddingBottom(Length(LayoutTheme::theme().popupInternalPaddingBottom(styleRef()), Fixed));

    if (m_optionStyle) {
        if ((m_optionStyle->direction() != innerStyle.direction() || m_optionStyle->unicodeBidi() != innerStyle.unicodeBidi()))
            m_innerBlock->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
        innerStyle.setTextAlign(style()->isLeftToRightDirection() ? LEFT : RIGHT);
        innerStyle.setDirection(m_optionStyle->direction());
        innerStyle.setUnicodeBidi(m_optionStyle->unicodeBidi());
    }
}

inline HTMLSelectElement* LayoutMenuList::selectElement() const
{
    return toHTMLSelectElement(node());
}

void LayoutMenuList::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    m_innerBlock->addChild(newChild, beforeChild);
    ASSERT(m_innerBlock == firstChild());

    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->childrenChanged(this);
}

void LayoutMenuList::removeChild(LayoutObject* oldChild)
{
    if (oldChild == m_innerBlock || !m_innerBlock) {
        LayoutFlexibleBox::removeChild(oldChild);
        m_innerBlock = nullptr;
    } else {
        m_innerBlock->removeChild(oldChild);
    }
}

void LayoutMenuList::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlock::styleDidChange(diff, oldStyle);

    if (!m_innerBlock)
        createInnerBlock();

    m_buttonText->setStyle(mutableStyle());
    adjustInnerStyle();

    bool fontChanged = !oldStyle || oldStyle->font() != style()->font();
    if (fontChanged)
        updateOptionsWidth();
}

void LayoutMenuList::updateOptionsWidth()
{
    float maxOptionWidth = 0;
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    int size = listItems.size();

    for (int i = 0; i < size; ++i) {
        HTMLElement* element = listItems[i];
        if (!isHTMLOptionElement(*element))
            continue;

        String text = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();
        applyTextTransform(style(), text, ' ');
        if (LayoutTheme::theme().popupOptionSupportsTextIndent()) {
            // Add in the option's text indent.  We can't calculate percentage values for now.
            float optionWidth = 0;
            if (const ComputedStyle* optionStyle = element->computedStyle())
                optionWidth += minimumValueForLength(optionStyle->textIndent(), 0);
            if (!text.isEmpty())
                optionWidth += style()->font().width(text);
            maxOptionWidth = std::max(maxOptionWidth, optionWidth);
        } else if (!text.isEmpty()) {
            maxOptionWidth = std::max(maxOptionWidth, style()->font().width(text));
        }
    }

    int width = static_cast<int>(ceilf(maxOptionWidth));
    if (m_optionsWidth == width)
        return;

    m_optionsWidth = width;
    if (parent())
        setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::MenuWidthChanged);
}

void LayoutMenuList::updateFromElement()
{
    if (m_optionsChanged) {
        updateOptionsWidth();
        m_optionsChanged = false;
    }

    if (m_popupIsVisible)
        m_popup->updateFromElement();

    updateText();
}

void LayoutMenuList::setIndexToSelectOnCancel(int listIndex)
{
    m_indexToSelectOnCancel = listIndex;
    updateText();
}

void LayoutMenuList::updateText()
{
    if (m_indexToSelectOnCancel >= 0)
        setTextFromOption(selectElement()->listToOptionIndex(m_indexToSelectOnCancel));
    else if (selectElement()->suggestedIndex() >= 0)
        setTextFromOption(selectElement()->suggestedIndex());
    else
        setTextFromOption(selectElement()->selectedIndex());
}

void LayoutMenuList::setTextFromOption(int optionIndex)
{
    HTMLSelectElement* select = selectElement();
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = select->listItems();
    const int size = listItems.size();

    String text = emptyString();
    m_optionStyle.clear();

    if (multiple()) {
        unsigned selectedCount = 0;
        int firstSelectedIndex = -1;
        for (int i = 0; i < size; ++i) {
            Element* element = listItems[i];
            if (!isHTMLOptionElement(*element))
                continue;

            if (toHTMLOptionElement(element)->selected()) {
                if (++selectedCount == 1)
                    firstSelectedIndex = i;
            }
        }

        if (selectedCount == 1) {
            ASSERT(0 <= firstSelectedIndex);
            ASSERT(firstSelectedIndex < size);
            HTMLOptionElement* selectedOptionElement = toHTMLOptionElement(listItems[firstSelectedIndex]);
            ASSERT(selectedOptionElement->selected());
            text = selectedOptionElement->textIndentedToRespectGroupLabel();
            m_optionStyle = selectedOptionElement->mutableComputedStyle();
        } else {
            Locale& locale = select->locale();
            String localizedNumberString = locale.convertToLocalizedNumber(String::number(selectedCount));
            text = locale.queryString(WebLocalizedString::SelectMenuListText, localizedNumberString);
            ASSERT(!m_optionStyle);
        }
    } else {
        const int i = select->optionToListIndex(optionIndex);
        if (i >= 0 && i < size) {
            Element* element = listItems[i];
            if (isHTMLOptionElement(*element)) {
                text = toHTMLOptionElement(element)->textIndentedToRespectGroupLabel();
                m_optionStyle = element->mutableComputedStyle();
            }
        }
    }

    setText(text.stripWhiteSpace());

    didUpdateActiveOption(optionIndex);
}

void LayoutMenuList::setText(const String& s)
{
    if (s.isEmpty()) {
        // FIXME: This is a hack. We need the select to have the same baseline positioning as
        // any surrounding text. Wihtout any content, we align the bottom of the select to the bottom
        // of the text. With content (In this case the faked " ") we correctly align the middle of
        // the select to the middle of the text. It should be possible to remove this, just set
        // s.impl() into the text and have things align correctly ...  crbug.com/485982
        m_isEmpty = true;
        m_buttonText->setText(StringImpl::create(" ", 1), true);
    } else {
        m_isEmpty = false;
        m_buttonText->setText(s.impl(), true);
    }
    adjustInnerStyle();
}

String LayoutMenuList::text() const
{
    return m_buttonText && !m_isEmpty ? m_buttonText->text() : String();
}

LayoutRect LayoutMenuList::controlClipRect(const LayoutPoint& additionalOffset) const
{
    // Clip to the intersection of the content box and the content box for the inner box
    // This will leave room for the arrows which sit in the inner box padding,
    // and if the inner box ever spills out of the outer box, that will get clipped too.
    LayoutRect outerBox = contentBoxRect();
    outerBox.moveBy(additionalOffset);

    LayoutRect innerBox(additionalOffset + m_innerBlock->location()
        + LayoutSize(m_innerBlock->paddingLeft(), m_innerBlock->paddingTop())
        , m_innerBlock->contentSize());

    return intersection(outerBox, innerBox);
}

void LayoutMenuList::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    maxLogicalWidth = std::max(m_optionsWidth, LayoutTheme::theme().minimumMenuListSize(styleRef())) + m_innerBlock->paddingLeft() + m_innerBlock->paddingRight();
    if (!style()->width().hasPercent())
        minLogicalWidth = maxLogicalWidth;
}

void LayoutMenuList::showPopup()
{
    if (m_popupIsVisible)
        return;

    if (document().frameHost()->chromeClient().hasOpenedPopup())
        return;

    if (!m_popup)
        m_popup = document().frameHost()->chromeClient().openPopupMenu(*document().frame(), this);
    m_popupIsVisible = true;

    FloatQuad quad(localToAbsoluteQuad(FloatQuad(borderBoundingBox())));
    IntSize size = pixelSnappedIntRect(frameRect()).size();
    HTMLSelectElement* select = selectElement();
    m_popup->show(quad, size, select->optionToListIndex(select->selectedIndex()));
    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->didShowMenuListPopup(this);
}

void LayoutMenuList::hidePopup()
{
    if (m_popup)
        m_popup->hide();
}

void LayoutMenuList::valueChanged(unsigned listIndex, bool fireOnChange)
{
    // Check to ensure a page navigation has not occurred while
    // the popup was up.
    Document& doc = toElement(node())->document();
    if (&doc != doc.frame()->document())
        return;

    setIndexToSelectOnCancel(-1);

    HTMLSelectElement* select = selectElement();
    select->optionSelectedByUser(select->listToOptionIndex(listIndex), fireOnChange);
}

void LayoutMenuList::listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow)
{
    selectElement()->listBoxSelectItem(listIndex, allowMultiplySelections, shift, fireOnChangeNow);
}

bool LayoutMenuList::multiple() const
{
    return selectElement()->multiple();
}

IntRect LayoutMenuList::elementRectRelativeToViewport() const
{
    // We don't use absoluteBoundingBoxRect() because it can return an IntRect
    // larger the actual size by 1px.
    return selectElement()->document().view()->contentsToViewport(roundedIntRect(absoluteBoundingBoxFloatRect()));
}

Element& LayoutMenuList::ownerElement() const
{
    return *selectElement();
}

const ComputedStyle* LayoutMenuList::computedStyleForItem(Element& element) const
{
    return element.computedStyle() ? element.computedStyle() : element.ensureComputedStyle();
}

void LayoutMenuList::didSetSelectedIndex(int listIndex)
{
    didUpdateActiveOption(selectElement()->listToOptionIndex(listIndex));
}

void LayoutMenuList::didUpdateActiveOption(int optionIndex)
{
    if (!document().existingAXObjectCache())
        return;

    if (m_lastActiveIndex == optionIndex)
        return;
    m_lastActiveIndex = optionIndex;

    HTMLSelectElement* select = selectElement();
    int listIndex = select->optionToListIndex(optionIndex);
    if (listIndex < 0 || listIndex >= static_cast<int>(select->listItems().size()))
        return;

    // We skip sending accessiblity notifications for the very first option, otherwise
    // we get extra focus and select events that are undesired.
    if (!m_hasUpdatedActiveOption) {
        m_hasUpdatedActiveOption = true;
        return;
    }

    document().existingAXObjectCache()->handleUpdateActiveMenuOption(this, optionIndex);
}

String LayoutMenuList::itemText(unsigned listIndex) const
{
    HTMLSelectElement* select = selectElement();
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = select->listItems();
    if (listIndex >= listItems.size())
        return String();

    String itemString;
    Element* element = listItems[listIndex];
    if (isHTMLOptGroupElement(*element))
        itemString = toHTMLOptGroupElement(*element).groupLabelText();
    else if (isHTMLOptionElement(*element))
        itemString = toHTMLOptionElement(*element).textIndentedToRespectGroupLabel();

    applyTextTransform(style(), itemString, ' ');
    return itemString;
}

String LayoutMenuList::itemAccessibilityText(unsigned listIndex) const
{
    // Allow the accessible name be changed if necessary.
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->fastGetAttribute(aria_labelAttr);
}

String LayoutMenuList::itemToolTip(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->title();
}

bool LayoutMenuList::itemIsEnabled(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    if (!isHTMLOptionElement(*element))
        return false;

    bool groupEnabled = true;
    if (Element* parentElement = element->parentElement()) {
        if (isHTMLOptGroupElement(*parentElement))
            groupEnabled = !parentElement->isDisabledFormControl();
    }
    if (!groupEnabled)
        return false;

    return !element->isDisabledFormControl();
}

PopupMenuStyle LayoutMenuList::itemStyle(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size()) {
        // If we are making an out of bounds access, then we want to use the style
        // of a different option element (index 0). However, if there isn't an option element
        // before at index 0, we fall back to the menu's style.
        if (!listIndex)
            return menuStyle();

        // Try to retrieve the style of an option element we know exists (index 0).
        listIndex = 0;
    }
    HTMLElement* element = listItems[listIndex];

    Color itemBackgroundColor;
    bool itemHasCustomBackgroundColor;
    getItemBackgroundColor(listIndex, itemBackgroundColor, itemHasCustomBackgroundColor);

    const ComputedStyle* style = element->computedStyle() ? element->computedStyle() : element->ensureComputedStyle();
    return style ? PopupMenuStyle(resolveColor(*style, CSSPropertyColor), itemBackgroundColor, style->font(), style->visibility() == VISIBLE,
        isHTMLOptionElement(*element) ? toHTMLOptionElement(*element).isDisplayNone() : style->display() == NONE,
        style->textIndent(), style->direction(), isOverride(style->unicodeBidi()),
        itemHasCustomBackgroundColor ? PopupMenuStyle::CustomBackgroundColor : PopupMenuStyle::DefaultBackgroundColor) : menuStyle();
}

void LayoutMenuList::getItemBackgroundColor(unsigned listIndex, Color& itemBackgroundColor, bool& itemHasCustomBackgroundColor) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size()) {
        itemBackgroundColor = resolveColor(CSSPropertyBackgroundColor);
        itemHasCustomBackgroundColor = false;
        return;
    }
    HTMLElement* element = listItems[listIndex];

    Color backgroundColor;
    if (const ComputedStyle* style = element->computedStyle())
        backgroundColor = resolveColor(*style, CSSPropertyBackgroundColor);
    itemHasCustomBackgroundColor = backgroundColor.alpha();
    // If the item has an opaque background color, return that.
    if (!backgroundColor.hasAlpha()) {
        itemBackgroundColor = backgroundColor;
        return;
    }

    // Otherwise, the item's background is overlayed on top of the menu background.
    backgroundColor = resolveColor(CSSPropertyBackgroundColor).blend(backgroundColor);
    if (!backgroundColor.hasAlpha()) {
        itemBackgroundColor = backgroundColor;
        return;
    }

    // If the menu background is not opaque, then add an opaque white background behind.
    itemBackgroundColor = Color(Color::white).blend(backgroundColor);
}

PopupMenuStyle LayoutMenuList::menuStyle() const
{
    const LayoutObject* o = m_innerBlock ? m_innerBlock : this;
    const ComputedStyle& style = o->styleRef();
    return PopupMenuStyle(o->resolveColor(CSSPropertyColor), o->resolveColor(CSSPropertyBackgroundColor), style.font(), style.visibility() == VISIBLE,
        style.display() == NONE, style.textIndent(), style.direction(), isOverride(style.unicodeBidi()));
}

LayoutUnit LayoutMenuList::clientPaddingLeft() const
{
    return paddingLeft() + m_innerBlock->paddingLeft();
}

const int endOfLinePadding = 2;
LayoutUnit LayoutMenuList::clientPaddingRight() const
{
    if (style()->appearance() == MenulistPart || style()->appearance() == MenulistButtonPart) {
        // For these appearance values, the theme applies padding to leave room for the
        // drop-down button. But leaving room for the button inside the popup menu itself
        // looks strange, so we return a small default padding to avoid having a large empty
        // space appear on the side of the popup menu.
        return endOfLinePadding;
    }

    // If the appearance isn't MenulistPart, then the select is styled (non-native), so
    // we want to return the user specified padding.
    return paddingRight() + m_innerBlock->paddingRight();
}

int LayoutMenuList::listSize() const
{
    return selectElement()->listItems().size();
}

int LayoutMenuList::selectedIndex() const
{
    HTMLSelectElement* select = selectElement();
    return select->optionToListIndex(select->selectedIndex());
}

void LayoutMenuList::popupDidHide()
{
    m_popupIsVisible = false;
    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->didHideMenuListPopup(this);
}

void LayoutMenuList::popupDidCancel()
{
    if (m_indexToSelectOnCancel >= 0)
        valueChanged(m_indexToSelectOnCancel);
}

bool LayoutMenuList::itemIsSeparator(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    return listIndex < listItems.size() && isHTMLHRElement(*listItems[listIndex]);
}

bool LayoutMenuList::itemIsLabel(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    return listIndex < listItems.size() && isHTMLOptGroupElement(*listItems[listIndex]);
}

bool LayoutMenuList::itemIsSelected(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    return isHTMLOptionElement(*element) && toHTMLOptionElement(*element).selected();
}

void LayoutMenuList::provisionalSelectionChanged(unsigned listIndex)
{
    setIndexToSelectOnCancel(listIndex);
}

} // namespace blink
