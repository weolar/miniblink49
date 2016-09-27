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
#include "bindings/core/v8/V8HTMLOptionsCollection.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8HTMLOptionElement.h"
#include "bindings/core/v8/V8Node.h"
#include "bindings/core/v8/V8NodeList.h"
#include "core/dom/ExceptionCode.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLOptionsCollection.h"
#include "core/html/HTMLSelectElement.h"

namespace blink {

void V8HTMLOptionsCollection::lengthAttributeSetterCustom(v8::Local<v8::Value> value, const v8::FunctionCallbackInfo<v8::Value>& info)
{
    HTMLOptionsCollection* impl = V8HTMLOptionsCollection::toImpl(info.Holder());
    unsigned newLength = 0;
    double currentLength;
    if (!v8Call(value->NumberValue(info.GetIsolate()->GetCurrentContext()), currentLength))
        return;
    ExceptionState exceptionState(ExceptionState::SetterContext, "length", "HTMLOptionsCollection", info.Holder(), info.GetIsolate());
    if (!std::isnan(currentLength) && !std::isinf(currentLength)) {
        if (currentLength < 0.0)
            exceptionState.throwDOMException(IndexSizeError, "The value provided (" + String::number(currentLength) + ") is negative. Lengths must be greater than or equal to 0.");
        else if (currentLength > static_cast<double>(UINT_MAX))
            newLength = UINT_MAX;
        else
            newLength = static_cast<unsigned>(currentLength);
    }

    if (exceptionState.throwIfNeeded())
        return;

    impl->setLength(newLength, exceptionState);
}

} // namespace blink
