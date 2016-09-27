/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "public/platform/WebCryptoKey.h"

#include "public/platform/WebCryptoAlgorithm.h"
#include "public/platform/WebCryptoAlgorithmParams.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"
#include "wtf/OwnPtr.h"
#include "wtf/ThreadSafeRefCounted.h"

namespace blink {

class WebCryptoKeyPrivate : public ThreadSafeRefCounted<WebCryptoKeyPrivate> {
public:
    WebCryptoKeyPrivate(PassOwnPtr<WebCryptoKeyHandle> handle, WebCryptoKeyType type, bool extractable, const WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyUsageMask usages)
        : handle(handle)
        , type(type)
        , extractable(extractable)
        , algorithm(algorithm)
        , usages(usages)
    {
        ASSERT(!algorithm.isNull());
    }

    const OwnPtr<WebCryptoKeyHandle> handle;
    const WebCryptoKeyType type;
    const bool extractable;
    const WebCryptoKeyAlgorithm algorithm;
    const WebCryptoKeyUsageMask usages;
};

WebCryptoKey WebCryptoKey::create(WebCryptoKeyHandle* handle, WebCryptoKeyType type, bool extractable, const WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyUsageMask usages)
{
    WebCryptoKey key;
    key.m_private = adoptRef(new WebCryptoKeyPrivate(adoptPtr(handle), type, extractable, algorithm, usages));
    return key;
}

WebCryptoKey WebCryptoKey::createNull()
{
    return WebCryptoKey();
}

WebCryptoKeyHandle* WebCryptoKey::handle() const
{
    ASSERT(!isNull());
    return m_private->handle.get();
}

WebCryptoKeyType WebCryptoKey::type() const
{
    ASSERT(!isNull());
    return m_private->type;
}

bool WebCryptoKey::extractable() const
{
    ASSERT(!isNull());
    return m_private->extractable;
}

const WebCryptoKeyAlgorithm& WebCryptoKey::algorithm() const
{
    ASSERT(!isNull());
    return m_private->algorithm;
}

WebCryptoKeyUsageMask WebCryptoKey::usages() const
{
    ASSERT(!isNull());
    return m_private->usages;
}

bool WebCryptoKey::isNull() const
{
    return m_private.isNull();
}

void WebCryptoKey::assign(const WebCryptoKey& other)
{
    m_private = other.m_private;
}

void WebCryptoKey::reset()
{
    m_private.reset();
}

} // namespace blink
