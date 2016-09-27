/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "modules/indexeddb/WebIDBDatabaseCallbacksImpl.h"

#include "core/dom/DOMError.h"

namespace blink {

// static
PassOwnPtr<WebIDBDatabaseCallbacksImpl> WebIDBDatabaseCallbacksImpl::create(IDBDatabaseCallbacks* callbacks)
{
    return adoptPtr(new WebIDBDatabaseCallbacksImpl(callbacks));
}

WebIDBDatabaseCallbacksImpl::WebIDBDatabaseCallbacksImpl(IDBDatabaseCallbacks* callbacks)
    : m_callbacks(callbacks)
{
}

WebIDBDatabaseCallbacksImpl::~WebIDBDatabaseCallbacksImpl()
{
}

void WebIDBDatabaseCallbacksImpl::onForcedClose()
{
    m_callbacks->onForcedClose();
}

void WebIDBDatabaseCallbacksImpl::onVersionChange(long long oldVersion, long long newVersion)
{
    m_callbacks->onVersionChange(oldVersion, newVersion);
}

void WebIDBDatabaseCallbacksImpl::onAbort(long long transactionId, const WebIDBDatabaseError& error)
{
    m_callbacks->onAbort(transactionId, DOMError::create(error.code(), error.message()));
}

void WebIDBDatabaseCallbacksImpl::onComplete(long long transactionId)
{
    m_callbacks->onComplete(transactionId);
}

} // namespace blink
