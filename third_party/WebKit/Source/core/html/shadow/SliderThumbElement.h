/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SliderThumbElement_h
#define SliderThumbElement_h

#include "core/HTMLNames.h"
#include "core/html/HTMLDivElement.h"
#include "core/layout/LayoutBlockFlow.h"
#include "wtf/Forward.h"

namespace blink {

class HTMLInputElement;
class Event;

class SliderThumbElement final : public HTMLDivElement {
public:
    static PassRefPtrWillBeRawPtr<SliderThumbElement> create(Document&);

    void setPositionFromValue();

    void dragFrom(const LayoutPoint&);
    void defaultEventHandler(Event*) override;
    bool willRespondToMouseMoveEvents() override;
    bool willRespondToMouseClickEvents() override;
    void detach(const AttachContext& = AttachContext()) override;
    const AtomicString& shadowPseudoId() const override;
    HTMLInputElement* hostInput() const;
    void setPositionFromPoint(const LayoutPoint&);
    void stopDragging();

private:
    SliderThumbElement(Document&);
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    PassRefPtrWillBeRawPtr<Element> cloneElementWithoutAttributesAndChildren() override;
    bool isDisabledFormControl() const override;
    bool matchesReadOnlyPseudoClass() const override;
    bool matchesReadWritePseudoClass() const override;
    Node* focusDelegate() override;
    void startDragging();

    bool m_inDragMode;
};

inline PassRefPtrWillBeRawPtr<Element> SliderThumbElement::cloneElementWithoutAttributesAndChildren()
{
    return create(document());
}

// FIXME: There are no ways to check if a node is a SliderThumbElement.
DEFINE_ELEMENT_TYPE_CASTS(SliderThumbElement, isHTMLElement());

class SliderContainerElement final : public HTMLDivElement {
public:
    DECLARE_NODE_FACTORY(SliderContainerElement);

private:
    explicit SliderContainerElement(Document&);
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    const AtomicString& shadowPseudoId() const override;
};

}

#endif
