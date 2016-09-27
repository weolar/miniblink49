/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/html/HTMLDialogElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/NodeTraversal.h"
#include "core/events/Event.h"
#include "core/frame/FrameView.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLFormControlElement.h"
#include "core/layout/LayoutBlock.h"
#include "core/style/ComputedStyle.h"

namespace blink {

using namespace HTMLNames;

// This function chooses the focused element when showModal() is invoked, as described in the spec for showModal().
static void setFocusForModalDialog(HTMLDialogElement* dialog)
{
    Element* focusableDescendant = 0;
    Node* next = 0;
    for (Node* node = dialog->firstChild(); node; node = next) {
        if (isHTMLDialogElement(*node))
            next = NodeTraversal::nextSkippingChildren(*node, dialog);
        else
            next = NodeTraversal::next(*node, dialog);

        if (!node->isElementNode())
            continue;
        Element* element = toElement(node);
        if (element->isFormControlElement()) {
            HTMLFormControlElement* control = toHTMLFormControlElement(node);
            if (control->isAutofocusable()) {
                control->focus();
                return;
            }
        }
        if (!focusableDescendant && element->isFocusable())
            focusableDescendant = element;
    }

    if (focusableDescendant) {
        focusableDescendant->focus();
        return;
    }

    if (dialog->isFocusable()) {
        dialog->focus();
        return;
    }

    dialog->document().setFocusedElement(nullptr);
}

static void inertSubtreesChanged(Document& document)
{
    // When a modal dialog opens or closes, nodes all over the accessibility
    // tree can change inertness which means they must be added or removed from
    // the tree. The most foolproof way is to clear the entire tree and rebuild
    // it, though a more clever way is probably possible.
    Document& topDocument = document.topDocument();
    topDocument.clearAXObjectCache();
    if (AXObjectCache* cache = topDocument.axObjectCache())
        cache->childrenChanged(&topDocument);
}

inline HTMLDialogElement::HTMLDialogElement(Document& document)
    : HTMLElement(dialogTag, document)
    , m_centeringMode(NotCentered)
    , m_centeredPosition(0)
    , m_returnValue("")
{
    UseCounter::count(document, UseCounter::DialogElement);
}

DEFINE_NODE_FACTORY(HTMLDialogElement)

void HTMLDialogElement::close(const String& returnValue, ExceptionState& exceptionState)
{
    if (!fastHasAttribute(openAttr)) {
        exceptionState.throwDOMException(InvalidStateError, "The element does not have an 'open' attribute, and therefore cannot be closed.");
        return;
    }
    closeDialog(returnValue);
}

void HTMLDialogElement::closeDialog(const String& returnValue)
{
    if (!fastHasAttribute(openAttr))
        return;
    setBooleanAttribute(openAttr, false);

    HTMLDialogElement* activeModalDialog = document().activeModalDialog();
    document().removeFromTopLayer(this);
    if (activeModalDialog == this)
        inertSubtreesChanged(document());

    if (!returnValue.isNull())
        m_returnValue = returnValue;

    dispatchScopedEvent(Event::create(EventTypeNames::close));
}

void HTMLDialogElement::forceLayoutForCentering()
{
    m_centeringMode = NeedsCentering;
    document().updateLayoutIgnorePendingStylesheets();
    if (m_centeringMode == NeedsCentering)
        setNotCentered();
}

void HTMLDialogElement::show()
{
    if (fastHasAttribute(openAttr))
        return;
    setBooleanAttribute(openAttr, true);
}

void HTMLDialogElement::showModal(ExceptionState& exceptionState)
{
    if (fastHasAttribute(openAttr)) {
        exceptionState.throwDOMException(InvalidStateError, "The element already has an 'open' attribute, and therefore cannot be opened modally.");
        return;
    }
    if (!inDocument()) {
        exceptionState.throwDOMException(InvalidStateError, "The element is not in a Document.");
        return;
    }

    document().addToTopLayer(this);
    setBooleanAttribute(openAttr, true);

    // Throw away the AX cache first, so the subsequent steps don't have a chance of queuing up
    // AX events on objects that would be invalidated when the cache is thrown away.
    inertSubtreesChanged(document());

    forceLayoutForCentering();
    setFocusForModalDialog(this);
}

void HTMLDialogElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLElement::removedFrom(insertionPoint);
    setNotCentered();
    // FIXME: We should call inertSubtreesChanged() here.
}

void HTMLDialogElement::setCentered(LayoutUnit centeredPosition)
{
    ASSERT(m_centeringMode == NeedsCentering);
    m_centeredPosition = centeredPosition;
    m_centeringMode = Centered;
}

void HTMLDialogElement::setNotCentered()
{
    m_centeringMode = NotCentered;
}

bool HTMLDialogElement::isPresentationAttribute(const QualifiedName& name) const
{
    // FIXME: Workaround for <https://bugs.webkit.org/show_bug.cgi?id=91058>: modifying an attribute for which there is an attribute selector
    // in html.css sometimes does not trigger a style recalc.
    if (name == openAttr)
        return true;

    return HTMLElement::isPresentationAttribute(name);
}

void HTMLDialogElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::cancel) {
        closeDialog();
        event->setDefaultHandled();
        return;
    }
    HTMLElement::defaultEventHandler(event);
}

} // namespace blink
