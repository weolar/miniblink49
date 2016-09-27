/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
#include "modules/accessibility/AXScrollbar.h"

#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/scroll/Scrollbar.h"

namespace blink {

AXScrollbar::AXScrollbar(Scrollbar* scrollbar, AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache)
    , m_scrollbar(scrollbar)
{
    ASSERT(scrollbar);
}

AXScrollbar::~AXScrollbar()
{
    ASSERT(!m_scrollbar);
}

void AXScrollbar::detachFromParent()
{
    m_scrollbar = nullptr;
    AXMockObject::detachFromParent();
}

PassRefPtrWillBeRawPtr<AXScrollbar> AXScrollbar::create(Scrollbar* scrollbar, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXScrollbar(scrollbar, axObjectCache));
}

LayoutRect AXScrollbar::elementRect() const
{
    if (!m_scrollbar)
        return LayoutRect();

    return LayoutRect(m_scrollbar->frameRect());
}

Document* AXScrollbar::document() const
{
    AXObject* parent = parentObject();
    if (!parent)
        return 0;
    return parent->document();
}

AccessibilityOrientation AXScrollbar::orientation() const
{
    if (!m_scrollbar)
        return AccessibilityOrientationHorizontal;

    if (m_scrollbar->orientation() == HorizontalScrollbar)
        return AccessibilityOrientationHorizontal;
    if (m_scrollbar->orientation() == VerticalScrollbar)
        return AccessibilityOrientationVertical;

    return AccessibilityOrientationVertical;
}

bool AXScrollbar::isEnabled() const
{
    if (!m_scrollbar)
        return false;
    return m_scrollbar->enabled();
}

float AXScrollbar::valueForRange() const
{
    if (!m_scrollbar)
        return 0;

    return m_scrollbar->currentPos() / m_scrollbar->maximum();
}

void AXScrollbar::setValue(float value)
{
    if (!m_scrollbar)
        return;

    if (!m_scrollbar->scrollableArea())
        return;

    float newValue = value * m_scrollbar->maximum();

    // TODO(bokan): This should potentially be a UserScroll.
    m_scrollbar->scrollableArea()->setScrollPositionSingleAxis(m_scrollbar->orientation(), newValue, ProgrammaticScroll);
}

DEFINE_TRACE(AXScrollbar)
{
    visitor->trace(m_scrollbar);
    AXMockObject::trace(visitor);
}

} // namespace blink
