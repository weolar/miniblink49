/*
 * Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8Binding.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8AbstractEventListener.h"
#include "bindings/core/v8/V8BindingMacros.h"
#include "bindings/core/v8/V8Element.h"
#include "bindings/core/v8/V8EventTarget.h"
#include "bindings/core/v8/V8NodeFilter.h"
#include "bindings/core/v8/V8NodeFilterCondition.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "bindings/core/v8/V8Window.h"
#include "bindings/core/v8/V8WorkerGlobalScope.h"
#include "bindings/core/v8/V8XPathNSResolver.h"
#include "bindings/core/v8/WindowProxy.h"
#include "bindings/core/v8/WorkerScriptController.h"
#include "bindings/core/v8/custom/V8CustomXPathNSResolver.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/NodeFilter.h"
#include "core/dom/QualifiedName.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
//#include "core/workers/WorkerGlobalScope.h"
#include "core/xml/XPathNSResolver.h"
#include "platform/EventTracer.h"
#include "platform/JSONValues.h"
#include "wtf/MainThread.h"
#include "wtf/MathExtras.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Threading.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/CString.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/Unicode.h"
#include "wtf/text/WTFString.h"

namespace blink {

void setArityTypeError(ExceptionState& exceptionState, const char* valid, unsigned provided)
{
    exceptionState.throwTypeError(ExceptionMessages::invalidArity(valid, provided));
}

v8::Local<v8::Value> createMinimumArityTypeErrorForMethod(v8::Isolate* isolate, const char* method, const char* type, unsigned expected, unsigned provided)
{
    return V8ThrowException::createTypeError(isolate, ExceptionMessages::failedToExecute(method, type, ExceptionMessages::notEnoughArguments(expected, provided)));
}

v8::Local<v8::Value> createMinimumArityTypeErrorForConstructor(v8::Isolate* isolate, const char* type, unsigned expected, unsigned provided)
{
    return V8ThrowException::createTypeError(isolate, ExceptionMessages::failedToConstruct(type, ExceptionMessages::notEnoughArguments(expected, provided)));
}

void setMinimumArityTypeError(ExceptionState& exceptionState, unsigned expected, unsigned provided)
{
    exceptionState.throwTypeError(ExceptionMessages::notEnoughArguments(expected, provided));
}

PassRefPtrWillBeRawPtr<NodeFilter> toNodeFilter(v8::Local<v8::Value> callback, v8::Local<v8::Object> creationContext, ScriptState* scriptState)
{
    if (callback->IsNull())
        return nullptr;
    RefPtrWillBeRawPtr<NodeFilter> filter = NodeFilter::create();

    v8::Local<v8::Value> filterWrapper = toV8(filter.get(), creationContext, scriptState->isolate());
    if (filterWrapper.IsEmpty())
        return nullptr;

    RefPtrWillBeRawPtr<NodeFilterCondition> condition = V8NodeFilterCondition::create(callback, filterWrapper.As<v8::Object>(), scriptState);
    filter->setCondition(condition.release());

    return filter.release();
}

bool toBooleanSlow(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    ASSERT(!value->IsBoolean());
    v8::TryCatch block;
    bool result = false;
    if (!v8Call(value->BooleanValue(isolate->GetCurrentContext()), result, block))
        exceptionState.rethrowV8Exception(block.Exception());
    return result;
}

const int32_t kMaxInt32 = 0x7fffffff;
const int32_t kMinInt32 = -kMaxInt32 - 1;
const uint32_t kMaxUInt32 = 0xffffffff;
const int64_t kJSMaxInteger = 0x20000000000000LL - 1; // 2^53 - 1, maximum uniquely representable integer in ECMAScript.

static double enforceRange(double x, double minimum, double maximum, const char* typeName, ExceptionState& exceptionState)
{
    if (std::isnan(x) || std::isinf(x)) {
        exceptionState.throwTypeError("Value is" + String(std::isinf(x) ? " infinite and" : "") + " not of type '" + String(typeName) + "'.");
        return 0;
    }
    x = trunc(x);
    if (x < minimum || x > maximum) {
        exceptionState.throwTypeError("Value is outside the '" + String(typeName) + "' value range.");
        return 0;
    }
    return x;
}

template <typename T>
struct IntTypeLimits {
};

template <>
struct IntTypeLimits<int8_t> {
    static const int8_t minValue = -128;
    static const int8_t maxValue = 127;
    static const unsigned numberOfValues = 256; // 2^8
};

template <>
struct IntTypeLimits<uint8_t> {
    static const uint8_t maxValue = 255;
    static const unsigned numberOfValues = 256; // 2^8
};

template <>
struct IntTypeLimits<int16_t> {
    static const short minValue = -32768;
    static const short maxValue = 32767;
    static const unsigned numberOfValues = 65536; // 2^16
};

template <>
struct IntTypeLimits<uint16_t> {
    static const unsigned short maxValue = 65535;
    static const unsigned numberOfValues = 65536; // 2^16
};

template <typename T>
static inline T toSmallerInt(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, const char* typeName, ExceptionState& exceptionState)
{
    typedef IntTypeLimits<T> LimitsTrait;

    // Fast case. The value is already a 32-bit integer in the right range.
    if (value->IsInt32()) {
        int32_t result = value.As<v8::Int32>()->Value();
        if (result >= LimitsTrait::minValue && result <= LimitsTrait::maxValue)
            return static_cast<T>(result);
        if (configuration == EnforceRange) {
            exceptionState.throwTypeError("Value is outside the '" + String(typeName) + "' value range.");
            return 0;
        }
        if (configuration == Clamp)
            return clampTo<T>(result);
        result %= LimitsTrait::numberOfValues;
        return static_cast<T>(result > LimitsTrait::maxValue ? result - LimitsTrait::numberOfValues : result);
    }

    v8::Local<v8::Number> numberObject;
    if (value->IsNumber()) {
        numberObject = value.As<v8::Number>();
    } else {
        // Can the value be converted to a number?
        v8::TryCatch block(isolate);
        if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return 0;
        }
    }
    ASSERT(!numberObject.IsEmpty());

    if (configuration == EnforceRange)
        return enforceRange(numberObject->Value(), LimitsTrait::minValue, LimitsTrait::maxValue, typeName, exceptionState);

    double numberValue = numberObject->Value();
    if (std::isnan(numberValue) || !numberValue)
        return 0;

    if (configuration == Clamp)
        return clampTo<T>(numberValue);

    if (std::isinf(numberValue))
        return 0;

    numberValue = numberValue < 0 ? -floor(fabs(numberValue)) : floor(fabs(numberValue));
    numberValue = fmod(numberValue, LimitsTrait::numberOfValues);

    return static_cast<T>(numberValue > LimitsTrait::maxValue ? numberValue - LimitsTrait::numberOfValues : numberValue);
}

template <typename T>
static inline T toSmallerUInt(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, const char* typeName, ExceptionState& exceptionState)
{
    typedef IntTypeLimits<T> LimitsTrait;

    // Fast case. The value is a 32-bit signed integer - possibly positive?
    if (value->IsInt32()) {
        int32_t result = value.As<v8::Int32>()->Value();
        if (result >= 0 && result <= LimitsTrait::maxValue)
            return static_cast<T>(result);
        if (configuration == EnforceRange) {
            exceptionState.throwTypeError("Value is outside the '" + String(typeName) + "' value range.");
            return 0;
        }
        if (configuration == Clamp)
            return clampTo<T>(result);
        return static_cast<T>(result);
    }

    v8::Local<v8::Number> numberObject;
    if (value->IsNumber()) {
        numberObject = value.As<v8::Number>();
    } else {
        // Can the value be converted to a number?
        v8::TryCatch block(isolate);
        if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return 0;
        }
    }
    ASSERT(!numberObject.IsEmpty());

    if (configuration == EnforceRange)
        return enforceRange(numberObject->Value(), 0, LimitsTrait::maxValue, typeName, exceptionState);

    double numberValue = numberObject->Value();

    if (std::isnan(numberValue) || !numberValue)
        return 0;

    if (configuration == Clamp)
        return clampTo<T>(numberValue);

    if (std::isinf(numberValue))
        return 0;

    numberValue = numberValue < 0 ? -floor(fabs(numberValue)) : floor(fabs(numberValue));
    return static_cast<T>(fmod(numberValue, LimitsTrait::numberOfValues));
}

int8_t toInt8(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    return toSmallerInt<int8_t>(isolate, value, configuration, "byte", exceptionState);
}

uint8_t toUInt8(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    return toSmallerUInt<uint8_t>(isolate, value, configuration, "octet", exceptionState);
}

int16_t toInt16(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    return toSmallerInt<int16_t>(isolate, value, configuration, "short", exceptionState);
}

uint16_t toUInt16(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    return toSmallerUInt<uint16_t>(isolate, value, configuration, "unsigned short", exceptionState);
}

int32_t toInt32Slow(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    ASSERT(!value->IsInt32());
    // Can the value be converted to a number?
    v8::TryCatch block(isolate);
    v8::Local<v8::Number> numberObject;
    if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }

    ASSERT(!numberObject.IsEmpty());

    double numberValue = numberObject->Value();
    if (configuration == EnforceRange)
        return enforceRange(numberValue, kMinInt32, kMaxInt32, "long", exceptionState);

    if (std::isnan(numberValue))
        return 0;

    if (configuration == Clamp)
        return clampTo<int32_t>(numberValue);

    if (std::isinf(numberValue))
        return 0;

    int32_t result;
    if (!v8Call(numberObject->Int32Value(isolate->GetCurrentContext()), result, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    return result;
}

uint32_t toUInt32Slow(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    ASSERT(!value->IsUint32());
    if (value->IsInt32()) {
        ASSERT(configuration != NormalConversion);
        int32_t result = value.As<v8::Int32>()->Value();
        if (result >= 0)
            return result;
        if (configuration == EnforceRange) {
            exceptionState.throwTypeError("Value is outside the 'unsigned long' value range.");
            return 0;
        }
        ASSERT(configuration == Clamp);
        return clampTo<uint32_t>(result);
    }

    // Can the value be converted to a number?
    v8::TryCatch block(isolate);
    v8::Local<v8::Number> numberObject;
    if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    ASSERT(!numberObject.IsEmpty());

    if (configuration == EnforceRange)
        return enforceRange(numberObject->Value(), 0, kMaxUInt32, "unsigned long", exceptionState);

    double numberValue = numberObject->Value();

    if (std::isnan(numberValue))
        return 0;

    if (configuration == Clamp)
        return clampTo<uint32_t>(numberValue);

    if (std::isinf(numberValue))
        return 0;

    uint32_t result;
    if (!v8Call(numberObject->Uint32Value(isolate->GetCurrentContext()), result, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    return result;
}

int64_t toInt64Slow(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    ASSERT(!value->IsInt32());

    v8::Local<v8::Number> numberObject;
    // Can the value be converted to a number?
    v8::TryCatch block(isolate);
    if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    ASSERT(!numberObject.IsEmpty());

    double numberValue = numberObject->Value();

    if (configuration == EnforceRange)
        return enforceRange(numberValue, -kJSMaxInteger, kJSMaxInteger, "long long", exceptionState);

    if (std::isnan(numberValue) || std::isinf(numberValue))
        return 0;

    // NaNs and +/-Infinity should be 0, otherwise modulo 2^64.
    unsigned long long integer;
    doubleToInteger(numberValue, integer);
    return integer;
}

uint64_t toUInt64Slow(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    ASSERT(!value->IsUint32());
    if (value->IsInt32()) {
        ASSERT(configuration != NormalConversion);
        int32_t result = value.As<v8::Int32>()->Value();
        if (result >= 0)
            return result;
        if (configuration == EnforceRange) {
            exceptionState.throwTypeError("Value is outside the 'unsigned long long' value range.");
            return 0;
        }
        ASSERT(configuration == Clamp);
        return clampTo<uint64_t>(result);
    }

    v8::Local<v8::Number> numberObject;
    // Can the value be converted to a number?
    v8::TryCatch block(isolate);
    if (!v8Call(value->ToNumber(isolate->GetCurrentContext()), numberObject, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    ASSERT(!numberObject.IsEmpty());

    double numberValue = numberObject->Value();

    if (configuration == EnforceRange)
        return enforceRange(numberValue, 0, kJSMaxInteger, "unsigned long long", exceptionState);

    if (std::isnan(numberValue))
        return 0;

    if (configuration == Clamp)
        return clampTo<uint64_t>(numberValue);

    if (std::isinf(numberValue))
        return 0;

    // NaNs and +/-Infinity should be 0, otherwise modulo 2^64.
    unsigned long long integer;
    doubleToInteger(numberValue, integer);
    return integer;
}

float toRestrictedFloat(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    float numberValue = toFloat(isolate, value, exceptionState);
    if (exceptionState.hadException())
        return 0;
    if (!std::isfinite(numberValue)) {
        exceptionState.throwTypeError("The provided float value is non-finite.");
        return 0;
    }
    return numberValue;
}

double toDoubleSlow(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    ASSERT(!value->IsNumber());
    v8::TryCatch block(isolate);
    double doubleValue;
    if (!v8Call(value->NumberValue(isolate->GetCurrentContext()), doubleValue, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return 0;
    }
    return doubleValue;
}

double toRestrictedDouble(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    double numberValue = toDouble(isolate, value, exceptionState);
    if (exceptionState.hadException())
        return 0;
    if (!std::isfinite(numberValue)) {
        exceptionState.throwTypeError("The provided double value is non-finite.");
        return 0;
    }
    return numberValue;
}

String toByteString(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    // Handle null default value.
    if (value.IsEmpty())
        return String();

    // From the Web IDL spec: http://heycam.github.io/webidl/#es-ByteString
    if (value.IsEmpty())
        return String();

    // 1. Let x be ToString(v)
    v8::Local<v8::String> stringObject;
    if (value->IsString()) {
        stringObject = value.As<v8::String>();
    } else {
        v8::TryCatch block(isolate);
        if (!v8Call(value->ToString(isolate->GetCurrentContext()), stringObject, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return String();
        }
    }

    String x = toCoreString(stringObject);

    // 2. If the value of any element of x is greater than 255, then throw a TypeError.
    if (!x.containsOnlyLatin1()) {
        exceptionState.throwTypeError("Value is not a valid ByteString.");
        return String();
    }

    // 3. Return an IDL ByteString value whose length is the length of x, and where the
    // value of each element is the value of the corresponding element of x.
    // Blink: A ByteString is simply a String with a range constrained per the above, so
    // this is the identity operation.
    return x;
}

static bool hasUnmatchedSurrogates(const String& string)
{
    // By definition, 8-bit strings are confined to the Latin-1 code page and
    // have no surrogates, matched or otherwise.
    if (string.is8Bit())
        return false;

    const UChar* characters = string.characters16();
    const unsigned length = string.length();

    for (unsigned i = 0; i < length; ++i) {
        UChar c = characters[i];
        if (U16_IS_SINGLE(c))
            continue;
        if (U16_IS_TRAIL(c))
            return true;
        ASSERT(U16_IS_LEAD(c));
        if (i == length - 1)
            return true;
        UChar d = characters[i + 1];
        if (!U16_IS_TRAIL(d))
            return true;
        ++i;
    }
    return false;
}

// Replace unmatched surrogates with REPLACEMENT CHARACTER U+FFFD.
static String replaceUnmatchedSurrogates(const String& string)
{
    // This roughly implements http://heycam.github.io/webidl/#dfn-obtain-unicode
    // but since Blink strings are 16-bits internally, the output is simply
    // re-encoded to UTF-16.

    // The concept of surrogate pairs is explained at:
    // http://www.unicode.org/versions/Unicode6.2.0/ch03.pdf#G2630

    // Blink-specific optimization to avoid making an unnecessary copy.
    if (!hasUnmatchedSurrogates(string))
        return string;
    ASSERT(!string.is8Bit());

    // 1. Let S be the DOMString value.
    const UChar* s = string.characters16();

    // 2. Let n be the length of S.
    const unsigned n = string.length();

    // 3. Initialize i to 0.
    unsigned i = 0;

    // 4. Initialize U to be an empty sequence of Unicode characters.
    StringBuilder u;
    u.reserveCapacity(n);

    // 5. While i < n:
    while (i < n) {
        // 1. Let c be the code unit in S at index i.
        UChar c = s[i];
        // 2. Depending on the value of c:
        if (U16_IS_SINGLE(c)) {
            // c < 0xD800 or c > 0xDFFF
            // Append to U the Unicode character with code point c.
            u.append(c);
        } else if (U16_IS_TRAIL(c)) {
            // 0xDC00 <= c <= 0xDFFF
            // Append to U a U+FFFD REPLACEMENT CHARACTER.
            u.append(replacementCharacter);
        } else {
            // 0xD800 <= c <= 0xDBFF
            ASSERT(U16_IS_LEAD(c));
            if (i == n - 1) {
                // 1. If i = n-1, then append to U a U+FFFD REPLACEMENT CHARACTER.
                u.append(replacementCharacter);
            } else {
                // 2. Otherwise, i < n-1:
                ASSERT(i < n - 1);
                // ....1. Let d be the code unit in S at index i+1.
                UChar d = s[i + 1];
                if (U16_IS_TRAIL(d)) {
                    // 2. If 0xDC00 <= d <= 0xDFFF, then:
                    // ..1. Let a be c & 0x3FF.
                    // ..2. Let b be d & 0x3FF.
                    // ..3. Append to U the Unicode character with code point 2^16+2^10*a+b.
                    u.append(U16_GET_SUPPLEMENTARY(c, d));
                    // Blink: This is equivalent to u.append(c); u.append(d);
                    ++i;
                } else {
                    // 3. Otherwise, d < 0xDC00 or d > 0xDFFF. Append to U a U+FFFD REPLACEMENT CHARACTER.
                    u.append(replacementCharacter);
                }
            }
        }
        // 3. Set i to i+1.
        ++i;
    }

    // 6. Return U.
    ASSERT(u.length() == string.length());
    return u.toString();
}

String toUSVString(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    // http://heycam.github.io/webidl/#es-USVString
    if (value.IsEmpty())
        return String();

    v8::Local<v8::String> stringObject;
    if (value->IsString()) {
        stringObject = value.As<v8::String>();
    } else {
        v8::TryCatch block(isolate);
        if (!v8Call(value->ToString(isolate->GetCurrentContext()), stringObject, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return String();
        }
    }

    // USVString is identical to DOMString except that "convert a
    // DOMString to a sequence of Unicode characters" is used subsequently
    // when converting to an IDL value
    String x = toCoreString(stringObject);
    return replaceUnmatchedSurrogates(x);
}

XPathNSResolver* toXPathNSResolver(ScriptState* scriptState, v8::Local<v8::Value> value)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    XPathNSResolver* resolver = nullptr;
    if (V8XPathNSResolver::hasInstance(value, scriptState->isolate()))
        resolver = V8XPathNSResolver::toImpl(v8::Local<v8::Object>::Cast(value));
    else if (value->IsObject())
        resolver = V8CustomXPathNSResolver::create(scriptState, value.As<v8::Object>());
    return resolver;
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return nullptr;
}

DOMWindow* toDOMWindow(v8::Isolate* isolate, v8::Local<v8::Value> value)
{
    if (value.IsEmpty() || !value->IsObject())
        return 0;

    v8::Local<v8::Object> windowWrapper = V8Window::findInstanceInPrototypeChain(v8::Local<v8::Object>::Cast(value), isolate);
    if (!windowWrapper.IsEmpty())
        return V8Window::toImpl(windowWrapper);
    return 0;
}

DOMWindow* toDOMWindow(v8::Local<v8::Context> context)
{
    if (context.IsEmpty())
        return 0;
    return toDOMWindow(context->GetIsolate(), context->Global());
}

LocalDOMWindow* enteredDOMWindow(v8::Isolate* isolate)
{
    LocalDOMWindow* window = toLocalDOMWindow(toDOMWindow(isolate->GetEnteredContext()));
    if (!window) {
        // We don't always have an entered DOM window, for example during microtask callbacks from V8
        // (where the entered context may be the DOM-in-JS context). In that case, we fall back
        // to the current context.
        window = currentDOMWindow(isolate);
        ASSERT(window);
    }
    return window;
}

LocalDOMWindow* currentDOMWindow(v8::Isolate* isolate)
{
    return toLocalDOMWindow(toDOMWindow(isolate->GetCurrentContext()));
}

LocalDOMWindow* callingDOMWindow(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCallingContext();
    if (context.IsEmpty()) {
        // Unfortunately, when processing script from a plugin, we might not
        // have a calling context. In those cases, we fall back to the
        // entered context.
        context = isolate->GetEnteredContext();
    }
    return toLocalDOMWindow(toDOMWindow(context));
}

ExecutionContext* toExecutionContext(v8::Local<v8::Context> context)
{
    if (context.IsEmpty())
        return 0;

    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::Object> windowWrapper = V8Window::findInstanceInPrototypeChain(global, context->GetIsolate());
    if (!windowWrapper.IsEmpty())
        return V8Window::toImpl(windowWrapper)->executionContext();

#ifndef MINIBLINK_NOT_IMPLEMENTED_WEBWORKER
    v8::Local<v8::Object> workerWrapper = V8WorkerGlobalScope::findInstanceInPrototypeChain(global, context->GetIsolate());
    if (!workerWrapper.IsEmpty())
        return V8WorkerGlobalScope::toImpl(workerWrapper)->executionContext();
#endif
    // FIXME: Is this line of code reachable?
    return 0;
}

ExecutionContext* currentExecutionContext(v8::Isolate* isolate)
{
    return toExecutionContext(isolate->GetCurrentContext());
}

ExecutionContext* callingExecutionContext(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCallingContext();
    if (context.IsEmpty()) {
        // Unfortunately, when processing script from a plugin, we might not
        // have a calling context. In those cases, we fall back to the
        // entered context.
        context = isolate->GetEnteredContext();
    }
    return toExecutionContext(context);
}

Frame* toFrameIfNotDetached(v8::Local<v8::Context> context)
{
    DOMWindow* window = toDOMWindow(context);
    if (window && window->isCurrentlyDisplayedInFrame())
        return window->frame();
    // We return 0 here because |context| is detached from the Frame. If we
    // did return |frame| we could get in trouble because the frame could be
    // navigated to another security origin.
    return nullptr;
}

EventTarget* toEventTarget(v8::Isolate* isolate, v8::Local<v8::Value> value)
{
    // We need to handle a DOMWindow specially, because a DOMWindow wrapper
    // exists on a prototype chain of v8Value.
    if (DOMWindow* window = toDOMWindow(isolate, value))
        return static_cast<EventTarget*>(window);
    if (V8EventTarget::hasInstance(value, isolate)) {
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        return toWrapperTypeInfo(object)->toEventTarget(object);
    }
    return 0;
}

v8::Local<v8::Context> toV8Context(ExecutionContext* context, DOMWrapperWorld& world)
{
    ASSERT(context);
    if (context->isDocument()) {
        if (LocalFrame* frame = toDocument(context)->frame())
            return toV8Context(frame, world);
    } else if (context->isWorkerGlobalScope()) {
        if (WorkerScriptController* script = toWorkerGlobalScope(context)->script()) {
            if (script->scriptState()->contextIsValid())
                return script->scriptState()->context();
        }
    }
    return v8::Local<v8::Context>();
}

v8::Local<v8::Context> toV8Context(Frame* frame, DOMWrapperWorld& world)
{
    if (!frame)
        return v8::Local<v8::Context>();
    v8::Local<v8::Context> context = toV8ContextEvenIfDetached(frame, world);
    ScriptState* scriptState = ScriptState::from(context);
    if (scriptState->contextIsValid()) {
        ASSERT(toFrameIfNotDetached(context) == frame);
        return scriptState->context();
    }
    return v8::Local<v8::Context>();
}

v8::Local<v8::Context> toV8ContextEvenIfDetached(Frame* frame, DOMWrapperWorld& world)
{
    ASSERT(frame);
    return frame->windowProxy(world)->context();
}

void crashIfV8IsDead()
{
    if (v8::V8::IsDead()) {
        // FIXME: We temporarily deal with V8 internal error situations
        // such as out-of-memory by crashing the renderer.
        //CRASH(); // weolar
    }
}

bool isValidEnum(const String& value, const char** validValues, size_t length, const String& enumName, ExceptionState& exceptionState)
{
    for (size_t i = 0; i < length; ++i) {
        if (value == validValues[i])
            return true;
    }
    exceptionState.throwTypeError("The provided value '" + value + "' is not a valid enum value of type " + enumName + ".");
    return false;
}

bool isValidEnum(const Vector<String>& values, const char** validValues, size_t length, const String& enumName, ExceptionState& exceptionState)
{
    for (auto value : values) {
        if (!isValidEnum(value, validValues, length, enumName, exceptionState))
            return false;
    }
    return true;
}

v8::Local<v8::Function> getBoundFunction(v8::Local<v8::Function> function)
{
    v8::Local<v8::Value> boundFunction = function->GetBoundFunction();
    return boundFunction->IsFunction() ? v8::Local<v8::Function>::Cast(boundFunction) : function;
}

bool addHiddenValueToArray(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::Value> value, int arrayIndex)
{
    ASSERT(!value.IsEmpty());
    v8::Local<v8::Value> arrayValue = object->GetInternalField(arrayIndex);
    if (arrayValue->IsNull() || arrayValue->IsUndefined()) {
        arrayValue = v8::Array::New(isolate);
        object->SetInternalField(arrayIndex, arrayValue);
    }

    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(arrayValue);
    return v8CallBoolean(array->Set(isolate->GetCurrentContext(), v8::Integer::New(isolate, array->Length()), value));
}

void removeHiddenValueFromArray(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::Value> value, int arrayIndex)
{
    v8::Local<v8::Value> arrayValue = object->GetInternalField(arrayIndex);
    if (!arrayValue->IsArray())
        return;
    v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(arrayValue);
    for (int i = array->Length() - 1; i >= 0; --i) {
        v8::Local<v8::Value> item;
        if (!array->Get(isolate->GetCurrentContext(), i).ToLocal(&item))
            return;
        if (item->StrictEquals(value)) {
            array->Delete(isolate->GetCurrentContext(), i);
            return;
        }
    }
}

void moveEventListenerToNewWrapper(v8::Isolate* isolate, v8::Local<v8::Object> object, EventListener* oldValue, v8::Local<v8::Value> newValue, int arrayIndex)
{
    if (oldValue) {
        V8AbstractEventListener* oldListener = V8AbstractEventListener::cast(oldValue);
        if (oldListener) {
            v8::Local<v8::Object> oldListenerObject = oldListener->getExistingListenerObject();
            if (!oldListenerObject.IsEmpty())
                removeHiddenValueFromArray(isolate, object, oldListenerObject, arrayIndex);
        }
    }
    // Non-callable input is treated as null and ignored
    if (newValue->IsFunction())
        addHiddenValueToArray(isolate, object, newValue, arrayIndex);
}

v8::Isolate* toIsolate(ExecutionContext* context)
{
    if (context && context->isDocument())
        return V8PerIsolateData::mainThreadIsolate();
    return v8::Isolate::GetCurrent();
}

v8::Isolate* toIsolate(LocalFrame* frame)
{
    ASSERT(frame);
    return frame->script().isolate();
}

JSONValuePtr NativeValueTraits<JSONValuePtr>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState, int maxDepth)
{
    if (value.IsEmpty()) {
        ASSERT_NOT_REACHED();
        return nullptr;
    }

    if (!maxDepth)
        return nullptr;
    maxDepth--;

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    if (value->IsNull() || value->IsUndefined())
        return JSONValue::null();
    if (value->IsBoolean())
        return JSONBasicValue::create(value.As<v8::Boolean>()->Value());
    if (value->IsNumber())
        return JSONBasicValue::create(value.As<v8::Number>()->Value());
    if (value->IsString())
        return JSONString::create(toCoreString(value.As<v8::String>()));
    if (value->IsArray()) {
        v8::Local<v8::Array> array = value.As<v8::Array>();
        RefPtr<JSONArray> inspectorArray = JSONArray::create();
        uint32_t length = array->Length();
        for (uint32_t i = 0; i < length; i++) {
            v8::Local<v8::Value> value;
            if (!array->Get(context, i).ToLocal(&value))
                return nullptr;
            RefPtr<JSONValue> element = nativeValue(isolate, value, exceptionState, maxDepth);
            if (!element)
                return nullptr;
            inspectorArray->pushValue(element);
        }
        return inspectorArray;
    }
    if (value->IsObject()) {
        RefPtr<JSONObject> jsonObject = JSONObject::create();
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::Array> propertyNames;
        if (!object->GetPropertyNames(context).ToLocal(&propertyNames))
            return nullptr;
        uint32_t length = propertyNames->Length();
        for (uint32_t i = 0; i < length; i++) {
            v8::Local<v8::Value> name;
            if (!propertyNames->Get(context, i).ToLocal(&name))
                return nullptr;
            // FIXME(yurys): v8::Object should support GetOwnPropertyNames
            if (name->IsString() && !v8CallBoolean(object->HasRealNamedProperty(context, v8::Local<v8::String>::Cast(name))))
                continue;
            v8::Local<v8::Value> property;
            if (!object->Get(context, name).ToLocal(&property))
                return nullptr;
            RefPtr<JSONValue> propertyValue = nativeValue(isolate, property, exceptionState, maxDepth);
            if (!propertyValue)
                return nullptr;
            TOSTRING_DEFAULT(V8StringResource<TreatNullAsNullString>, nameString, name, nullptr);
            jsonObject->setValue(nameString, propertyValue);
        }
        return jsonObject;
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

void DevToolsFunctionInfo::ensureInitialized() const
{
    if (m_function.IsEmpty())
        return;

    v8::HandleScope scope(m_function->GetIsolate());
    v8::Local<v8::Function> originalFunction = getBoundFunction(m_function);
    m_scriptId = originalFunction->ScriptId();
    v8::ScriptOrigin origin = originalFunction->GetScriptOrigin();
    if (!origin.ResourceName().IsEmpty()) {
        V8StringResource<> stringResource(origin.ResourceName());
        stringResource.prepare();
        m_resourceName = stringResource;
        m_lineNumber = originalFunction->GetScriptLineNumber() + 1;
    }
    if (m_resourceName.isEmpty()) {
        m_resourceName = "";
        m_lineNumber = 1;
    }

    m_function.Clear();
}

int DevToolsFunctionInfo::scriptId() const
{
    ensureInitialized();
    return m_scriptId;
}

int DevToolsFunctionInfo::lineNumber() const
{
    ensureInitialized();
    return m_lineNumber;
}

String DevToolsFunctionInfo::resourceName() const
{
    ensureInitialized();
    return m_resourceName;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> devToolsTraceEventData(v8::Isolate* isolate, ExecutionContext* context, v8::Local<v8::Function> function)
{
    DevToolsFunctionInfo info(function);
    return InspectorFunctionCallEvent::data(context, info.scriptId(), info.resourceName(), info.lineNumber());
}

void v8ConstructorAttributeGetter(v8::Local<v8::Name> propertyName, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "DOMGetter");
    v8::Local<v8::Value> data = info.Data();
    ASSERT(data->IsExternal());
    V8PerContextData* perContextData = V8PerContextData::from(info.Holder()->CreationContext());
    if (!perContextData)
        return;
    v8SetReturnValue(info, perContextData->constructorForType(WrapperTypeInfo::unwrap(data)));
    TRACE_EVENT_SET_SAMPLING_STATE("v8", "V8Execution");
}

} // namespace blink
