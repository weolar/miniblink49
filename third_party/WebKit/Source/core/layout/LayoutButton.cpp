/**
 * Copyright (C) 2005 Apple Computer, Inc.
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
#include "core/layout/LayoutButton.h"

namespace blink {

using namespace HTMLNames;

LayoutButton::LayoutButton(Element* element)
    : LayoutFlexibleBox(element)
    , m_inner(nullptr)
{
}

LayoutButton::~LayoutButton()
{
}

void LayoutButton::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    if (!m_inner) {
        // Create an anonymous block.
        ASSERT(!firstChild());
        m_inner = createAnonymousBlock(style()->display());
        LayoutFlexibleBox::addChild(m_inner);
    }

    m_inner->addChild(newChild, beforeChild);
}

void LayoutButton::removeChild(LayoutObject* oldChild)
{
    if (oldChild == m_inner || !m_inner) {
        LayoutFlexibleBox::removeChild(oldChild);
        m_inner = 0;

    } else if (oldChild->parent() == this) {
        // We aren't the inner node, but we're getting removed from the button, this
        // can happen with things like scrollable area resizer's.
        LayoutFlexibleBox::removeChild(oldChild);

    } else {
        m_inner->removeChild(oldChild);
    }
}

void LayoutButton::updateAnonymousChildStyle(const LayoutObject& child, ComputedStyle& childStyle) const
{
    ASSERT(!m_inner || &child == m_inner);

    childStyle.setFlexGrow(1.0f);
    // min-width: 0; is needed for correct shrinking.
    childStyle.setMinWidth(Length(0, Fixed));
    // Use margin:auto instead of align-items:center to get safe centering, i.e.
    // when the content overflows, treat it the same as align-items: flex-start.
    childStyle.setMarginTop(Length());
    childStyle.setMarginBottom(Length());
    childStyle.setFlexDirection(style()->flexDirection());
    childStyle.setJustifyContent(style()->justifyContent());
    childStyle.setFlexWrap(style()->flexWrap());
    childStyle.setAlignItems(style()->alignItems());
    childStyle.setAlignContent(style()->alignContent());
}

LayoutRect LayoutButton::controlClipRect(const LayoutPoint& additionalOffset) const
{
    // Clip to the padding box to at least give content the extra padding space.
    LayoutRect rect(additionalOffset, size());
    rect.expand(borderInsets());
    return rect;
}

int LayoutButton::baselinePosition(FontBaseline baseline, bool firstLine, LineDirectionMode direction, LinePositionMode linePositionMode) const
{
    ASSERT(linePositionMode == PositionOnContainingLine);
    // We want to call the LayoutBlock version of firstLineBoxBaseline to
    // avoid LayoutFlexibleBox synthesizing a baseline that we don't want.
    // We use this check as a proxy for "are there any line boxes in this button"
    if (!hasLineIfEmpty() && LayoutBlock::firstLineBoxBaseline() == -1) {
        // To ensure that we have a consistent baseline when we have no children,
        // even when we have the anonymous LayoutBlock child, we calculate the
        // baseline for the empty case manually here.
        if (direction == HorizontalLine) {
            return marginTop() + size().height() - borderBottom() - paddingBottom() - horizontalScrollbarHeight();
        }
        return marginRight() + size().width() - borderLeft() - paddingLeft() - verticalScrollbarWidth();
    }
    return LayoutFlexibleBox::baselinePosition(baseline, firstLine, direction, linePositionMode);
}


// For compatibility with IE/FF we only clip overflow on input elements.
bool LayoutButton::hasControlClip() const
{
    return !isHTMLButtonElement(node());
}
} // namespace blink
