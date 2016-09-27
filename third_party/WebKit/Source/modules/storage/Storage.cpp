/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/storage/Storage.h"

#include "bindings/core/v8/ExceptionState.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

Storage* Storage::create(LocalFrame* frame, StorageArea* storageArea)
{
    return new Storage(frame, storageArea);
}

Storage::Storage(LocalFrame* frame, StorageArea* storageArea)
    : DOMWindowProperty(frame)
    , m_storageArea(storageArea)
{
    ASSERT(m_frame);
    ASSERT(m_storageArea);
}

Storage::~Storage()
{
}

String Storage::anonymousIndexedGetter(unsigned index, ExceptionState& exceptionState)
{
    return anonymousNamedGetter(AtomicString::number(index), exceptionState);
}

String Storage::anonymousNamedGetter(const AtomicString& name, ExceptionState& exceptionState)
{
    bool found = contains(name, exceptionState);
    if (exceptionState.hadException() || !found)
        return String();
    String result = getItem(name, exceptionState);
    if (exceptionState.hadException())
        return String();
    return result;
}

bool Storage::anonymousNamedSetter(const AtomicString& name, const AtomicString& value, ExceptionState& exceptionState)
{
    setItem(name, value, exceptionState);
    return true;
}

bool Storage::anonymousIndexedSetter(unsigned index, const AtomicString& value, ExceptionState& exceptionState)
{
    return anonymousNamedSetter(AtomicString::number(index), value, exceptionState);
}

DeleteResult Storage::anonymousNamedDeleter(const AtomicString& name, ExceptionState& exceptionState)
{
    bool found = contains(name, exceptionState);
    if (!found)
        return DeleteUnknownProperty;
    if (exceptionState.hadException())
        return DeleteReject;
    removeItem(name, exceptionState);
    if (exceptionState.hadException())
        return DeleteReject;
    return DeleteSuccess;
}

DeleteResult Storage::anonymousIndexedDeleter(unsigned index, ExceptionState& exceptionState)
{
    DeleteResult result = anonymousNamedDeleter(AtomicString::number(index), exceptionState);
    return result == DeleteUnknownProperty ? DeleteSuccess : result;
}

void Storage::namedPropertyEnumerator(Vector<String>& names, ExceptionState& exceptionState)
{
    unsigned length = this->length(exceptionState);
    if (exceptionState.hadException())
        return;
    names.resize(length);
    for (unsigned i = 0; i < length; ++i) {
        String key = this->key(i, exceptionState);
        if (exceptionState.hadException())
            return;
        ASSERT(!key.isNull());
        String val = getItem(key, exceptionState);
        if (exceptionState.hadException())
            return;
        names[i] = key;
    }
}

bool Storage::namedPropertyQuery(const AtomicString& name, ExceptionState& exceptionState)
{
    if (name == "length")
        return false;
    bool found = contains(name, exceptionState);
    if (exceptionState.hadException() || !found)
        return false;
    return true;
}

DEFINE_TRACE(Storage)
{
    visitor->trace(m_storageArea);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
