/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/shadow/ClearButtonElement.h"

#include "core/events/MouseEvent.h"
#include "core/frame/LocalFrame.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutView.h"

namespace blink {

using namespace HTMLNames;

inline ClearButtonElement::ClearButtonElement(Document& document, ClearButtonOwner& clearButtonOwner)
    : HTMLDivElement(document)
    , m_clearButtonOwner(&clearButtonOwner)
{
}

PassRefPtrWillBeRawPtr<ClearButtonElement> ClearButtonElement::create(Document& document, ClearButtonOwner& clearButtonOwner)
{
    RefPtrWillBeRawPtr<ClearButtonElement> element = adoptRefWillBeNoop(new ClearButtonElement(document, clearButtonOwner));
    element->setShadowPseudoId(AtomicString("-webkit-clear-button", AtomicString::ConstructFromLiteral));
    element->setAttribute(idAttr, ShadowElementNames::clearButton());
    return element.release();
}

void ClearButtonElement::detach(const AttachContext& context)
{
    HTMLDivElement::detach(context);
}

void ClearButtonElement::defaultEventHandler(Event* event)
{
    if (!m_clearButtonOwner) {
        if (!event->defaultHandled())
            HTMLDivElement::defaultEventHandler(event);
        return;
    }

    if (!m_clearButtonOwner->shouldClearButtonRespondToMouseEvents()) {
        if (!event->defaultHandled())
            HTMLDivElement::defaultEventHandler(event);
        return;
    }

    if (event->type() == EventTypeNames::click) {
        if (layoutObject() && layoutObject()->visibleToHitTesting()) {
            m_clearButtonOwner->focusAndSelectClearButtonOwner();
            m_clearButtonOwner->clearValue();
            event->setDefaultHandled();
        }
    }

    if (!event->defaultHandled())
        HTMLDivElement::defaultEventHandler(event);
}

bool ClearButtonElement::isClearButtonElement() const
{
    return true;
}

DEFINE_TRACE(ClearButtonElement)
{
    visitor->trace(m_clearButtonOwner);
    HTMLDivElement::trace(visitor);
}

}
