/*
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#include "core/html/shadow/MediaControlElementTypes.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/css/StylePropertySet.h"
#include "core/events/MouseEvent.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/shadow/MediaControls.h"

namespace blink {

using namespace HTMLNames;

class Event;

HTMLMediaElement* toParentMediaElement(Node* node)
{
    if (!node)
        return nullptr;
    Node* mediaNode = node->shadowHost();
    if (!mediaNode)
        return nullptr;
    if (!isHTMLMediaElement(mediaNode))
        return nullptr;

    return toHTMLMediaElement(mediaNode);
}

MediaControlElementType mediaControlElementType(Node* node)
{
    ASSERT_WITH_SECURITY_IMPLICATION(node->isMediaControlElement());
    HTMLElement* element = toHTMLElement(node);
    if (isHTMLInputElement(*element))
        return static_cast<MediaControlInputElement*>(element)->displayType();
    return static_cast<MediaControlDivElement*>(element)->displayType();
}

MediaControlElement::MediaControlElement(MediaControls& mediaControls, MediaControlElementType displayType, HTMLElement* element)
    : m_mediaControls(mediaControls)
    , m_displayType(displayType)
    , m_element(element)
{
}

HTMLMediaElement& MediaControlElement::mediaElement() const
{
    return mediaControls().mediaElement();
}

void MediaControlElement::hide()
{
    m_element->setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
}

void MediaControlElement::show()
{
    m_element->removeInlineStyleProperty(CSSPropertyDisplay);
}

void MediaControlElement::setDisplayType(MediaControlElementType displayType)
{
    if (displayType == m_displayType)
        return;

    m_displayType = displayType;
    if (LayoutObject* object = m_element->layoutObject())
        object->setShouldDoFullPaintInvalidation();
}

DEFINE_TRACE(MediaControlElement)
{
    visitor->trace(m_element);
}

// ----------------------------

MediaControlDivElement::MediaControlDivElement(MediaControls& mediaControls, MediaControlElementType displayType)
    : HTMLDivElement(mediaControls.document())
    , MediaControlElement(mediaControls, displayType, this)
{
}

DEFINE_TRACE(MediaControlDivElement)
{
    MediaControlElement::trace(visitor);
    HTMLDivElement::trace(visitor);
}

// ----------------------------

MediaControlInputElement::MediaControlInputElement(MediaControls& mediaControls, MediaControlElementType displayType)
    : HTMLInputElement(mediaControls.document(), 0, false)
    , MediaControlElement(mediaControls, displayType, this)
{
}

bool MediaControlInputElement::isMouseFocusable() const
{
    return false;
}

DEFINE_TRACE(MediaControlInputElement)
{
    MediaControlElement::trace(visitor);
    HTMLInputElement::trace(visitor);
}

// ----------------------------

MediaControlTimeDisplayElement::MediaControlTimeDisplayElement(MediaControls& mediaControls, MediaControlElementType displayType)
    : MediaControlDivElement(mediaControls, displayType)
    , m_currentValue(0)
{
}

void MediaControlTimeDisplayElement::setCurrentValue(double time)
{
    m_currentValue = time;
}

} // namespace blink
