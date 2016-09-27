/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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
#include "core/html/ImageData.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8Uint8ClampedArray.h"
#include "core/dom/ExceptionCode.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

ImageData* ImageData::create(const IntSize& size)
{
    Checked<int, RecordOverflow> dataSize = 4;
    dataSize *= size.width();
    dataSize *= size.height();
    if (dataSize.hasOverflowed())
        return nullptr;

    return new ImageData(size);
}

ImageData* ImageData::create(const IntSize& size, PassRefPtr<DOMUint8ClampedArray> byteArray)
{
    Checked<int, RecordOverflow> dataSize = 4;
    dataSize *= size.width();
    dataSize *= size.height();
    if (dataSize.hasOverflowed())
        return nullptr;

    if (dataSize.unsafeGet() < 0
        || static_cast<unsigned>(dataSize.unsafeGet()) > byteArray->length())
        return nullptr;

    return new ImageData(size, byteArray);
}

ImageData* ImageData::create(unsigned width, unsigned height, ExceptionState& exceptionState)
{
    if (!width || !height) {
        exceptionState.throwDOMException(IndexSizeError, String::format("The source %s is zero or not a number.", width ? "height" : "width"));
        return nullptr;
    }

    Checked<unsigned, RecordOverflow> dataSize = 4;
    dataSize *= width;
    dataSize *= height;
    if (dataSize.hasOverflowed()) {
        exceptionState.throwDOMException(IndexSizeError, "The requested image size exceeds the supported range.");
        return nullptr;
    }

    return new ImageData(IntSize(width, height));
}

bool ImageData::validateConstructorArguments(DOMUint8ClampedArray* data, unsigned width, unsigned& lengthInPixels, ExceptionState& exceptionState)
{
    if (!width) {
        exceptionState.throwDOMException(IndexSizeError, "The source width is zero or not a number.");
        return false;
    }
    ASSERT(data);
    unsigned length = data->length();
    if (!length) {
        exceptionState.throwDOMException(IndexSizeError, "The input data has a zero byte length.");
        return false;
    }
    if (length % 4) {
        exceptionState.throwDOMException(IndexSizeError, "The input data byte length is not a multiple of 4.");
        return false;
    }
    length /= 4;
    if (length % width) {
        exceptionState.throwDOMException(IndexSizeError, "The input data byte length is not a multiple of (4 * width).");
        return false;
    }
    lengthInPixels = length;
    return true;
}

ImageData* ImageData::create(DOMUint8ClampedArray* data, unsigned width, ExceptionState& exceptionState)
{
    unsigned lengthInPixels = 0;
    if (!validateConstructorArguments(data, width, lengthInPixels, exceptionState)) {
        ASSERT(exceptionState.hadException());
        return nullptr;
    }
    ASSERT(lengthInPixels && width);
    unsigned height = lengthInPixels / width;
    return new ImageData(IntSize(width, height), data);
}

ImageData* ImageData::create(DOMUint8ClampedArray* data, unsigned width, unsigned height, ExceptionState& exceptionState)
{
    unsigned lengthInPixels = 0;
    if (!validateConstructorArguments(data, width, lengthInPixels, exceptionState)) {
        ASSERT(exceptionState.hadException());
        return nullptr;
    }
    ASSERT(lengthInPixels && width);
    if (height != lengthInPixels / width) {
        exceptionState.throwDOMException(IndexSizeError, "The input data byte length is not equal to (4 * width * height).");
        return nullptr;
    }
    return new ImageData(IntSize(width, height), data);
}

v8::Local<v8::Object> ImageData::associateWithWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperType, v8::Local<v8::Object> wrapper)
{
    wrapper = ScriptWrappable::associateWithWrapper(isolate, wrapperType, wrapper);

    if (!wrapper.IsEmpty() && m_data.get()) {
        // Create a V8 Uint8ClampedArray object and set the "data" property
        // of the ImageData object to the created v8 object, eliminating the
        // C++ callback when accessing the "data" property.
        v8::Local<v8::Value> pixelArray = toV8(m_data.get(), wrapper, isolate);
        if (pixelArray.IsEmpty() || !v8CallBoolean(wrapper->DefineOwnProperty(isolate->GetCurrentContext(), v8AtomicString(isolate, "data"), pixelArray, v8::ReadOnly)))
            return v8::Local<v8::Object>();
    }
    return wrapper;
}

ImageData::ImageData(const IntSize& size)
    : m_size(size)
    , m_data(DOMUint8ClampedArray::create(size.width() * size.height() * 4))
{
}

ImageData::ImageData(const IntSize& size, PassRefPtr<DOMUint8ClampedArray> byteArray)
    : m_size(size)
    , m_data(byteArray)
{
    ASSERT_WITH_SECURITY_IMPLICATION(static_cast<unsigned>(size.width() * size.height() * 4) <= m_data->length());
}

void ImageData::dispose()
{
    m_data.clear();
}

} // namespace blink
