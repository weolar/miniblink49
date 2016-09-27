/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#include "modules/accessibility/AXSpinButton.h"

#include "core/layout/LayoutObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

PassRefPtrWillBeRawPtr<AXSpinButton> AXSpinButton::create(AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXSpinButton(axObjectCache));
}

AXSpinButton::AXSpinButton(AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache)
    , m_spinButtonElement(nullptr)
{
}

AXSpinButton::~AXSpinButton()
{
    ASSERT(!m_spinButtonElement);
}

DEFINE_TRACE(AXSpinButton)
{
    visitor->trace(m_spinButtonElement);
    AXMockObject::trace(visitor);
}

LayoutRect AXSpinButton::elementRect() const
{
    if (!m_spinButtonElement || !m_spinButtonElement->layoutObject())
        return LayoutRect();

    return LayoutRect(m_spinButtonElement->layoutObject()->absoluteFocusRingBoundingBoxRect());
}

void AXSpinButton::detach()
{
    AXObject::detach();
    m_spinButtonElement = nullptr;
}

void AXSpinButton::detachFromParent()
{
    AXObject::detachFromParent();
    m_spinButtonElement = nullptr;
}

AccessibilityRole AXSpinButton::roleValue() const
{
    return m_spinButtonElement ? SpinButtonRole : UnknownRole;
}

void AXSpinButton::addChildren()
{
    m_haveChildren = true;

    AXSpinButtonPart* incrementor = toAXSpinButtonPart(axObjectCache().getOrCreate(SpinButtonPartRole));
    incrementor->setIsIncrementor(true);
    incrementor->setParent(this);
    m_children.append(incrementor);

    AXSpinButtonPart* decrementor = toAXSpinButtonPart(axObjectCache().getOrCreate(SpinButtonPartRole));
    decrementor->setIsIncrementor(false);
    decrementor->setParent(this);
    m_children.append(decrementor);
}

void AXSpinButton::step(int amount)
{
    ASSERT(m_spinButtonElement);
    if (!m_spinButtonElement)
        return;

    m_spinButtonElement->step(amount);
}

// AXSpinButtonPart

AXSpinButtonPart::AXSpinButtonPart(AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache)
    , m_isIncrementor(false)
{
}

PassRefPtrWillBeRawPtr<AXSpinButtonPart> AXSpinButtonPart::create(AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXSpinButtonPart(axObjectCache));
}

LayoutRect AXSpinButtonPart::elementRect() const
{
    // FIXME: This logic should exist in the layout tree or elsewhere, but there is no
    // relationship that exists that can be queried.

    LayoutRect parentRect = parentObject()->elementRect();
    if (m_isIncrementor) {
        parentRect.setHeight(parentRect.height() / 2);
    } else {
        parentRect.setY(parentRect.y() + parentRect.height() / 2);
        parentRect.setHeight(parentRect.height() / 2);
    }

    return parentRect;
}

bool AXSpinButtonPart::press() const
{
    if (!m_parent || !m_parent->isSpinButton())
        return false;

    AXSpinButton* spinButton = toAXSpinButton(parentObject());
    if (m_isIncrementor)
        spinButton->step(1);
    else
        spinButton->step(-1);

    return true;
}

} // namespace blink
