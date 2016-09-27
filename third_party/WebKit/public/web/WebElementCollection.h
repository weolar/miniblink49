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

#ifndef WebElementCollection_h
#define WebElementCollection_h

#include "../platform/WebCommon.h"
#include "../platform/WebPrivatePtr.h"

#if BLINK_IMPLEMENTATION
#include "platform/heap/Handle.h"
namespace WTF { template <typename T> class PassRefPtr; }
#endif

namespace blink {

class HTMLCollection;
class WebElement;

// Provides readonly access to some properties of a DOM node.
class WebElementCollection {
public:
    ~WebElementCollection() { reset(); }

    WebElementCollection() : m_current(0) { }
    WebElementCollection(const WebElementCollection& n) { assign(n); }
    WebElementCollection& operator=(const WebElementCollection& n)
    {
        assign(n);
        return *this;
    }

    bool isNull() const { return m_private.isNull(); }

    BLINK_EXPORT void reset();
    BLINK_EXPORT void assign(const WebElementCollection&);

    BLINK_EXPORT unsigned length() const;
    BLINK_EXPORT WebElement nextItem() const;
    BLINK_EXPORT WebElement firstItem() const;

#if BLINK_IMPLEMENTATION
    WebElementCollection(const PassRefPtrWillBeRawPtr<HTMLCollection>&);
    WebElementCollection& operator=(const PassRefPtrWillBeRawPtr<HTMLCollection>&);
#endif

private:
    WebPrivatePtr<HTMLCollection> m_private;
    mutable unsigned m_current;
};

} // namespace blink

#endif
