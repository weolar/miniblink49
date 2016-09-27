/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
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
#include "public/web/WebElementCollection.h"

#include "core/dom/Element.h"
#include "core/html/HTMLCollection.h"
#include "public/web/WebElement.h"
#include "wtf/PassRefPtr.h"

namespace blink {

void WebElementCollection::reset()
{
    m_private.reset();
}

void WebElementCollection::assign(const WebElementCollection& other)
{
    m_private = other.m_private;
}

WebElementCollection::WebElementCollection(const PassRefPtrWillBeRawPtr<HTMLCollection>& col)
    : m_private(col)
{
}

WebElementCollection& WebElementCollection::operator=(const PassRefPtrWillBeRawPtr<HTMLCollection>& col)
{
    m_private = col;
    return *this;
}

unsigned WebElementCollection::length() const
{
    return m_private->length();
}

WebElement WebElementCollection::nextItem() const
{
    Element* element = m_private->item(m_current);
    if (element)
        m_current++;
    return WebElement(element);
}

WebElement WebElementCollection::firstItem() const
{
    m_current = 0;
    return nextItem();
}

} // namespace blink
