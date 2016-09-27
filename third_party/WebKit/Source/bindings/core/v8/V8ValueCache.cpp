/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/V8ValueCache.h"

#include "bindings/core/v8/V8Binding.h"
#include "wtf/text/StringHash.h"

namespace blink {

StringCacheMapTraits::MapType* StringCacheMapTraits::MapFromWeakCallbackInfo(
    const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
{
    return &(V8PerIsolateData::from(data.GetIsolate())->stringCache()->m_stringCache);
}

void StringCacheMapTraits::Dispose(
    v8::Isolate* isolate, v8::Global<v8::String> value, StringImpl* key)
{
    V8PerIsolateData::from(isolate)->stringCache()->InvalidateLastString();
    key->deref();
}

void StringCacheMapTraits::DisposeWeak(const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
{
    V8PerIsolateData::from(data.GetIsolate())->stringCache()->InvalidateLastString();
    data.GetParameter()->deref();
}

void StringCacheMapTraits::OnWeakCallback(const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
{
    V8PerIsolateData::from(data.GetIsolate())->stringCache()->InvalidateLastString();
}

void StringCache::dispose()
{
    // The MapType::Dispose callback calls StringCache::InvalidateLastString,
    // which will only work while the destructor has not yet finished. Thus,
    // we need to clear the map before the destructor has completed.
    m_stringCache.Clear();
}

static v8::Local<v8::String> makeExternalString(v8::Isolate* isolate, const String& string)
{
    if (string.is8Bit()) {
        WebCoreStringResource8* stringResource = new WebCoreStringResource8(string);
        v8::Local<v8::String> newString;
        if (!v8::String::NewExternalOneByte(isolate, stringResource).ToLocal(&newString)) {
            delete stringResource;
            return v8::String::Empty(isolate);
        }
        return newString;
    }

    WebCoreStringResource16* stringResource = new WebCoreStringResource16(string);
    v8::Local<v8::String> newString;
    if (!v8::String::NewExternalTwoByte(isolate, stringResource).ToLocal(&newString)) {
        delete stringResource;
        return v8::String::Empty(isolate);
    }
    return newString;
}

v8::Local<v8::String> StringCache::v8ExternalStringSlow(v8::Isolate* isolate, StringImpl* stringImpl)
{
    if (!stringImpl->length())
        return v8::String::Empty(isolate);

    StringCacheMapTraits::MapType::PersistentValueReference cachedV8String = m_stringCache.GetReference(stringImpl);
    if (!cachedV8String.IsEmpty()) {
        m_lastStringImpl = stringImpl;
        m_lastV8String = cachedV8String;
        return m_lastV8String.NewLocal(isolate);
    }

    return createStringAndInsertIntoCache(isolate, stringImpl);
}

void StringCache::setReturnValueFromStringSlow(v8::ReturnValue<v8::Value> returnValue, StringImpl* stringImpl)
{
    if (!stringImpl->length()) {
        returnValue.SetEmptyString();
        return;
    }

    StringCacheMapTraits::MapType::PersistentValueReference cachedV8String = m_stringCache.GetReference(stringImpl);
    if (!cachedV8String.IsEmpty()) {
        m_lastStringImpl = stringImpl;
        m_lastV8String = cachedV8String;
        m_lastV8String.SetReturnValue(returnValue);
        return;
    }

    returnValue.Set(createStringAndInsertIntoCache(returnValue.GetIsolate(), stringImpl));
}

v8::Local<v8::String> StringCache::createStringAndInsertIntoCache(v8::Isolate* isolate, StringImpl* stringImpl)
{
    ASSERT(!m_stringCache.Contains(stringImpl));
    ASSERT(stringImpl->length());

    v8::Local<v8::String> newString = makeExternalString(isolate, String(stringImpl));
    ASSERT(!newString.IsEmpty());
    ASSERT(newString->Length());

    v8::UniquePersistent<v8::String> wrapper(isolate, newString);

    stringImpl->ref();
    wrapper.MarkIndependent();
    m_stringCache.Set(stringImpl, wrapper.Pass(), &m_lastV8String);
    m_lastStringImpl = stringImpl;

    return newString;
}

void StringCache::InvalidateLastString()
{
    m_lastStringImpl = nullptr;
    m_lastV8String.Reset();
}

} // namespace blink
