/*
 * Copyright (C) 2013, Google Inc. All rights reserved.
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
#include "modules/accessibility/AXInlineTextBox.h"

#include "core/dom/Range.h"
#include "core/layout/LayoutText.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/LayoutUnit.h"


namespace blink {

using namespace HTMLNames;

AXInlineTextBox::AXInlineTextBox(PassRefPtr<AbstractInlineTextBox> inlineTextBox, AXObjectCacheImpl& axObjectCache)
    : AXObject(axObjectCache)
    , m_inlineTextBox(inlineTextBox)
{
}

PassRefPtrWillBeRawPtr<AXInlineTextBox> AXInlineTextBox::create(PassRefPtr<AbstractInlineTextBox> inlineTextBox, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXInlineTextBox(inlineTextBox, axObjectCache));
}

void AXInlineTextBox::init()
{
}

void AXInlineTextBox::detach()
{
    AXObject::detach();
    m_inlineTextBox = nullptr;
}

LayoutRect AXInlineTextBox::elementRect() const
{
    if (!m_inlineTextBox)
        return LayoutRect();

    return m_inlineTextBox->bounds();
}

bool AXInlineTextBox::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    AXObject* parent = parentObject();
    if (!parent)
        return false;

    if (!parent->accessibilityIsIgnored())
        return false;

    if (ignoredReasons)
        parent->computeAccessibilityIsIgnored(ignoredReasons);

    return true;
}

void AXInlineTextBox::textCharacterOffsets(Vector<int>& offsets) const
{
    if (!m_inlineTextBox)
        return;

    unsigned len = m_inlineTextBox->len();
    Vector<float> widths;
    m_inlineTextBox->characterWidths(widths);
    ASSERT(widths.size() == len);
    offsets.resize(len);

    float widthSoFar = 0;
    for (unsigned i = 0; i < len; i++) {
        widthSoFar += widths[i];
        offsets[i] = roundf(widthSoFar);
    }
}

void AXInlineTextBox::wordBoundaries(Vector<PlainTextRange>& words) const
{
    if (!m_inlineTextBox)
        return;

    Vector<AbstractInlineTextBox::WordBoundaries> wordBoundaries;
    m_inlineTextBox->wordBoundaries(wordBoundaries);
    words.resize(wordBoundaries.size());
    for (unsigned i = 0; i < wordBoundaries.size(); i++)
        words[i] = PlainTextRange(wordBoundaries[i].startIndex, wordBoundaries[i].endIndex - wordBoundaries[i].startIndex);
}

String AXInlineTextBox::stringValue() const
{
    if (!m_inlineTextBox)
        return String();

    return m_inlineTextBox->text();
}

AXObject* AXInlineTextBox::computeParent() const
{
    if (!m_inlineTextBox || !m_axObjectCache)
        return 0;

    LayoutText* layoutText = m_inlineTextBox->layoutText();
    return m_axObjectCache->getOrCreate(layoutText);
}

// In addition to LTR and RTL direction, edit fields also support
// top to bottom and bottom to top via the CSS writing-mode property.
AccessibilityTextDirection AXInlineTextBox::textDirection() const
{
    if (!m_inlineTextBox)
        return AXObject::textDirection();

    switch (m_inlineTextBox->direction()) {
    case AbstractInlineTextBox::LeftToRight:
        return AccessibilityTextDirectionLTR;
    case AbstractInlineTextBox::RightToLeft:
        return AccessibilityTextDirectionRTL;
    case AbstractInlineTextBox::TopToBottom:
        return AccessibilityTextDirectionTTB;
    case AbstractInlineTextBox::BottomToTop:
        return AccessibilityTextDirectionBTT;
    }

    return AXObject::textDirection();
}

AXObject* AXInlineTextBox::nextOnLine() const
{
    RefPtr<AbstractInlineTextBox> nextOnLine = m_inlineTextBox->nextOnLine();
    if (nextOnLine)
        return m_axObjectCache->getOrCreate(nextOnLine.get());

    if (!m_inlineTextBox->isLast())
        return 0;

    return parentObject()->nextOnLine();
}

AXObject* AXInlineTextBox::previousOnLine() const
{
    RefPtr<AbstractInlineTextBox> previousOnLine = m_inlineTextBox->previousOnLine();
    if (previousOnLine)
        return m_axObjectCache->getOrCreate(previousOnLine.get());

    if (!m_inlineTextBox->isFirst())
        return 0;

    return parentObject()->previousOnLine();
}

} // namespace blink
