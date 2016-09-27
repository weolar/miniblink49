/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "config.h"
#include "public/web/WebElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Element.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/NamedNodeMap.h"
#include "core/dom/custom/CustomElementProcessingStack.h"
#include "platform/graphics/Image.h"
#include "public/platform/WebRect.h"
#include "wtf/PassRefPtr.h"

namespace blink {

bool WebElement::isFormControlElement() const
{
    return constUnwrap<Element>()->isFormControlElement();
}

bool WebElement::isTextFormControlElement() const
{
    return constUnwrap<Element>()->isTextFormControl();
}

WebString WebElement::tagName() const
{
    return constUnwrap<Element>()->tagName();
}

bool WebElement::hasHTMLTagName(const WebString& tagName) const
{
    // How to create                     class              nodeName localName
    // createElement('input')            HTMLInputElement   INPUT    input
    // createElement('INPUT')            HTMLInputElement   INPUT    input
    // createElementNS(xhtmlNS, 'input') HTMLInputElement   INPUT    input
    // createElementNS(xhtmlNS, 'INPUT') HTMLUnknownElement INPUT    INPUT
    const Element* element = constUnwrap<Element>();
    return HTMLNames::xhtmlNamespaceURI == element->namespaceURI() && element->localName() == String(tagName).lower();
}

bool WebElement::hasAttribute(const WebString& attrName) const
{
    return constUnwrap<Element>()->hasAttribute(attrName);
}

void WebElement::removeAttribute(const WebString& attrName)
{
    // TODO: Custom element callbacks need to be called on WebKit API methods that
    // mutate the DOM in any way.
    CustomElementProcessingStack::CallbackDeliveryScope deliverCustomElementCallbacks;
    unwrap<Element>()->removeAttribute(attrName);
}

WebString WebElement::getAttribute(const WebString& attrName) const
{
    return constUnwrap<Element>()->getAttribute(attrName);
}

bool WebElement::setAttribute(const WebString& attrName, const WebString& attrValue)
{
    // TODO: Custom element callbacks need to be called on WebKit API methods that
    // mutate the DOM in any way.
    CustomElementProcessingStack::CallbackDeliveryScope deliverCustomElementCallbacks;
    TrackExceptionState exceptionState;
    unwrap<Element>()->setAttribute(attrName, attrValue, exceptionState);
    return !exceptionState.hadException();
}

unsigned WebElement::attributeCount() const
{
    if (!constUnwrap<Element>()->hasAttributes())
        return 0;
    return constUnwrap<Element>()->attributes().size();
}

WebString WebElement::attributeLocalName(unsigned index) const
{
    if (index >= attributeCount())
        return WebString();
    return constUnwrap<Element>()->attributes().at(index).localName();
}

WebString WebElement::attributeValue(unsigned index) const
{
    if (index >= attributeCount())
        return WebString();
    return constUnwrap<Element>()->attributes().at(index).value();
}

WebString WebElement::textContent() const
{
    return constUnwrap<Element>()->textContent();
}

void WebElement::requestFullScreen()
{
    Element* element = unwrap<Element>();
    Fullscreen::from(element->document()).requestFullscreen(*element, Fullscreen::PrefixedRequest);
}

bool WebElement::hasNonEmptyLayoutSize() const
{
    return constUnwrap<Element>()->hasNonEmptyLayoutSize();
}

WebRect WebElement::boundsInViewportSpace()
{
    return unwrap<Element>()->boundsInViewportSpace();
}

WebImage WebElement::imageContents()
{
    if (isNull())
        return WebImage();

    Image* image = unwrap<Element>()->imageContents();
    if (!image)
        return WebImage();

    SkBitmap bitmap;
    if (!image->bitmapForCurrentFrame(&bitmap))
        return WebImage();

    return WebImage(bitmap);
}

WebElement::WebElement(const PassRefPtrWillBeRawPtr<Element>& elem)
    : WebNode(elem)
{
}

WebElement& WebElement::operator=(const PassRefPtrWillBeRawPtr<Element>& elem)
{
    m_private = elem;
    return *this;
}

WebElement::operator PassRefPtrWillBeRawPtr<Element>() const
{
    return toElement(m_private.get());
}

} // namespace blink
