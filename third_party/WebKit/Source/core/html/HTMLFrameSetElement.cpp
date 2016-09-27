/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann (hausmann@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2009, 2010 Apple Inc. All rights reserved.
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
 */

#include "config.h"
#include "core/html/HTMLFrameSetElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/events/MouseEvent.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLFrameElement.h"
#include "core/layout/LayoutFrameSet.h"
#include "core/loader/FrameLoaderClient.h"

namespace blink {

using namespace HTMLNames;

inline HTMLFrameSetElement::HTMLFrameSetElement(Document& document)
    : HTMLElement(framesetTag, document)
    , m_border(6)
    , m_borderSet(false)
    , m_borderColorSet(false)
    , m_frameborder(true)
    , m_frameborderSet(false)
    , m_noresize(false)
{
    setHasCustomStyleCallbacks();
}

DEFINE_NODE_FACTORY(HTMLFrameSetElement)

bool HTMLFrameSetElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == bordercolorAttr)
        return true;
    return HTMLElement::isPresentationAttribute(name);
}

void HTMLFrameSetElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == bordercolorAttr)
        addHTMLColorToStyle(style, CSSPropertyBorderColor, value);
    else
        HTMLElement::collectStyleForPresentationAttribute(name, value, style);
}

void HTMLFrameSetElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == rowsAttr) {
        if (!value.isNull()) {
            m_rowLengths = parseListOfDimensions(value.string());
            setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(name));
        }
    } else if (name == colsAttr) {
        if (!value.isNull()) {
            m_colLengths = parseListOfDimensions(value.string());
            setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(name));
        }
    } else if (name == frameborderAttr) {
        if (!value.isNull()) {
            if (equalIgnoringCase(value, "no") || equalIgnoringCase(value, "0")) {
                m_frameborder = false;
                m_frameborderSet = true;
            } else if (equalIgnoringCase(value, "yes") || equalIgnoringCase(value, "1")) {
                m_frameborderSet = true;
            }
        } else {
            m_frameborder = false;
            m_frameborderSet = false;
        }
    } else if (name == noresizeAttr) {
        m_noresize = true;
    } else if (name == borderAttr) {
        if (!value.isNull()) {
            m_border = value.toInt();
            m_borderSet = true;
        } else {
            m_borderSet = false;
        }
    } else if (name == bordercolorAttr) {
        m_borderColorSet = !value.isEmpty();
    } else if (name == onloadAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::load, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onbeforeunloadAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::beforeunload, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onunloadAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::unload, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onpagehideAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::pagehide, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onpageshowAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::pageshow, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onblurAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::blur, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onerrorAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::error, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onfocusAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::focus, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onfocusinAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::focusin, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onfocusoutAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::focusout, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (RuntimeEnabledFeatures::orientationEventEnabled() && name == onorientationchangeAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::orientationchange, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onhashchangeAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::hashchange, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onmessageAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::message, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onresizeAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::resize, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onscrollAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::scroll, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onstorageAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::storage, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == ononlineAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::online, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onofflineAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::offline, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onpopstateAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::popstate, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == onlanguagechangeAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::languagechange, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else {
        HTMLElement::parseAttribute(name, value);
    }
}

bool HTMLFrameSetElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    // For compatibility, frames layoutObject even when display: none is set.
    // However, we delay creating a layoutObject until stylesheets have loaded.
    return style.isStyleAvailable();
}

LayoutObject* HTMLFrameSetElement::createLayoutObject(const ComputedStyle& style)
{
    if (style.hasContent())
        return LayoutObject::createObject(this, style);
    return new LayoutFrameSet(this);
}

void HTMLFrameSetElement::attach(const AttachContext& context)
{
    // Inherit default settings from parent frameset
    // FIXME: This is not dynamic.
    if (HTMLFrameSetElement* frameset = Traversal<HTMLFrameSetElement>::firstAncestor(*this)) {
        if (!m_frameborderSet)
            m_frameborder = frameset->hasFrameBorder();
        if (m_frameborder) {
            if (!m_borderSet)
                m_border = frameset->border();
            if (!m_borderColorSet)
                m_borderColorSet = frameset->hasBorderColor();
        }
        if (!m_noresize)
            m_noresize = frameset->noResize();
    }

    HTMLElement::attach(context);
}

void HTMLFrameSetElement::defaultEventHandler(Event* evt)
{
    if (evt->isMouseEvent() && !m_noresize && layoutObject() && layoutObject()->isFrameSet()) {
        if (toLayoutFrameSet(layoutObject())->userResize(toMouseEvent(evt))) {
            evt->setDefaultHandled();
            return;
        }
    }
    HTMLElement::defaultEventHandler(evt);
}

Node::InsertionNotificationRequest HTMLFrameSetElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument() && document().frame()) {
        // A document using <frameset> likely won't literally have a body, but as far as the client is concerned, the frameset is effectively the body.
        document().frame()->loader().client()->dispatchWillInsertBody();
    }
    return HTMLElement::insertedInto(insertionPoint);
}

void HTMLFrameSetElement::willRecalcStyle(StyleRecalcChange)
{
    if (needsStyleRecalc() && layoutObject()) {
        layoutObject()->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
        clearNeedsStyleRecalc();
    }
}

LocalDOMWindow* HTMLFrameSetElement::anonymousNamedGetter(const AtomicString& name)
{
    Element* frameElement = children()->namedItem(name);
    if (!isHTMLFrameElement(frameElement))
        return nullptr;
    Document* document = toHTMLFrameElement(frameElement)->contentDocument();
    if (!document || !document->frame())
        return nullptr;
    return document->domWindow();
}

} // namespace blink
