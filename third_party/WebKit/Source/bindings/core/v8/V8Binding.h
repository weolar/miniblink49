/*
* Copyright (C) 2009 Google Inc. All rights reserved.
* Copyright (C) 2012 Ericsson AB. All rights reserved.
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

#ifndef V8Binding_h
#define V8Binding_h

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/NativeValueTraits.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/V8BindingMacros.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "bindings/core/v8/V8StringResource.h"
#include "bindings/core/v8/V8ThrowException.h"
#include "bindings/core/v8/V8ValueCache.h"
#include "core/CoreExport.h"
#include "platform/JSONValues.h"
#include "platform/heap/Handle.h"
#include "wtf/text/AtomicString.h"
#include <v8.h>

namespace blink {

class DOMWindow;
class EventListener;
class EventTarget;
class ExceptionState;
class ExecutionContext;
class Frame;
class LocalDOMWindow;
class LocalFrame;
class NodeFilter;
class WorkerGlobalScope;
class XPathNSResolver;

template <typename T>
struct V8TypeOf {
    // |Type| provides C++ -> V8 type conversion for DOM wrappers.
    // The Blink binding code generator will generate specialized version of
    // V8TypeOf for each wrapper class.
    typedef void Type;
};

namespace TraceEvent {
class ConvertableToTraceFormat;
}

// Helpers for throwing JavaScript TypeErrors for arity mismatches.
CORE_EXPORT void setArityTypeError(ExceptionState&, const char* valid, unsigned provided);
CORE_EXPORT v8::Local<v8::Value> createMinimumArityTypeErrorForMethod(v8::Isolate*, const char* method, const char* type, unsigned expected, unsigned provided);
v8::Local<v8::Value> createMinimumArityTypeErrorForConstructor(v8::Isolate*, const char* type, unsigned expected, unsigned provided);
CORE_EXPORT void setMinimumArityTypeError(ExceptionState&, unsigned expected, unsigned provided);

template<typename CallbackInfo, typename S>
inline void v8SetReturnValue(const CallbackInfo& info, const v8::Persistent<S>& handle)
{
    info.GetReturnValue().Set(handle);
}

template<typename CallbackInfo, typename S>
inline void v8SetReturnValue(const CallbackInfo& info, const v8::Local<S> handle)
{
    info.GetReturnValue().Set(handle);
}

template<typename CallbackInfo, typename S>
inline void v8SetReturnValue(const CallbackInfo& info, v8::MaybeLocal<S> maybe)
{
    if (LIKELY(!maybe.IsEmpty()))
        info.GetReturnValue().Set(maybe.ToLocalChecked());
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& info, bool value)
{
    info.GetReturnValue().Set(value);
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& info, double value)
{
    info.GetReturnValue().Set(value);
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& info, int32_t value)
{
    info.GetReturnValue().Set(value);
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& info, uint32_t value)
{
    info.GetReturnValue().Set(value);
}

template<typename CallbackInfo>
inline void v8SetReturnValueBool(const CallbackInfo& info, bool v)
{
    info.GetReturnValue().Set(v);
}

template<typename CallbackInfo>
inline void v8SetReturnValueInt(const CallbackInfo& info, int v)
{
    info.GetReturnValue().Set(v);
}

template<typename CallbackInfo>
inline void v8SetReturnValueUnsigned(const CallbackInfo& info, unsigned v)
{
    info.GetReturnValue().Set(v);
}

template<typename CallbackInfo>
inline void v8SetReturnValueNull(const CallbackInfo& info)
{
    info.GetReturnValue().SetNull();
}

template<typename CallbackInfo>
inline void v8SetReturnValueUndefined(const CallbackInfo& info)
{
    info.GetReturnValue().SetUndefined();
}

template<typename CallbackInfo>
inline void v8SetReturnValueEmptyString(const CallbackInfo& info)
{
    info.GetReturnValue().SetEmptyString();
}

template<typename CallbackInfo>
inline void v8SetReturnValueString(const CallbackInfo& info, const String& string, v8::Isolate* isolate)
{
    if (string.isNull()) {
        v8SetReturnValueEmptyString(info);
        return;
    }
    V8PerIsolateData::from(isolate)->stringCache()->setReturnValueFromString(info.GetReturnValue(), string.impl());
}

template<typename CallbackInfo>
inline void v8SetReturnValueStringOrNull(const CallbackInfo& info, const String& string, v8::Isolate* isolate)
{
    if (string.isNull()) {
        v8SetReturnValueNull(info);
        return;
    }
    V8PerIsolateData::from(isolate)->stringCache()->setReturnValueFromString(info.GetReturnValue(), string.impl());
}

template<typename CallbackInfo>
inline void v8SetReturnValueStringOrUndefined(const CallbackInfo& info, const String& string, v8::Isolate* isolate)
{
    if (string.isNull()) {
        v8SetReturnValueUndefined(info);
        return;
    }
    V8PerIsolateData::from(isolate)->stringCache()->setReturnValueFromString(info.GetReturnValue(), string.impl());
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, ScriptWrappable* impl, v8::Local<v8::Object> creationContext)
{
    if (UNLIKELY(!impl)) {
        v8SetReturnValueNull(callbackInfo);
        return;
    }
    if (DOMDataStore::setReturnValue(callbackInfo.GetReturnValue(), impl))
        return;
    v8::Local<v8::Object> wrapper = impl->wrap(callbackInfo.GetIsolate(), creationContext);
    v8SetReturnValue(callbackInfo, wrapper);
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, ScriptWrappable* impl)
{
    v8SetReturnValue(callbackInfo, impl, callbackInfo.Holder());
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, Node* impl)
{
    if (UNLIKELY(!impl)) {
        v8SetReturnValueNull(callbackInfo);
        return;
    }
    if (DOMDataStore::setReturnValue(callbackInfo.GetReturnValue(), impl))
        return;
    v8::Local<v8::Object> wrapper = ScriptWrappable::fromNode(impl)->wrap(callbackInfo.GetIsolate(), callbackInfo.Holder());
    v8SetReturnValue(callbackInfo, wrapper);
}

// Special versions for DOMWindow, WorkerGlobalScope and EventTarget

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, DOMWindow* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, EventTarget* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, WorkerGlobalScope* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo, typename T>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, PassRefPtr<T> impl)
{
    v8SetReturnValue(callbackInfo, impl.get());
}

template<typename CallbackInfo, typename T>
inline void v8SetReturnValue(const CallbackInfo& callbackInfo, RawPtr<T> impl)
{
    v8SetReturnValue(callbackInfo, impl.get());
}

template<typename CallbackInfo>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, ScriptWrappable* impl)
{
    ASSERT(DOMWrapperWorld::current(callbackInfo.GetIsolate()).isMainWorld());
    if (UNLIKELY(!impl)) {
        v8SetReturnValueNull(callbackInfo);
        return;
    }
    if (DOMDataStore::setReturnValueForMainWorld(callbackInfo.GetReturnValue(), impl))
        return;
    v8::Local<v8::Object> wrapper = impl->wrap(callbackInfo.GetIsolate(), callbackInfo.Holder());
    v8SetReturnValue(callbackInfo, wrapper);
}

template<typename CallbackInfo>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, Node* impl)
{
    // Since EventTarget has a special version of ToV8 and V8EventTarget.h
    // defines its own v8SetReturnValue family, which are slow, we need to
    // override them with optimized versions for Node and its subclasses.
    // Without this overload, v8SetReturnValueForMainWorld for Node would be
    // very slow.
    //
    // class hierarchy:
    //     ScriptWrappable <-- EventTarget <--+-- Node <-- ...
    //                                        +-- Window
    // overloads:
    //     v8SetReturnValueForMainWorld(ScriptWrappable*)
    //         Optimized and very fast.
    //     v8SetReturnValueForMainWorld(EventTarget*)
    //         Uses custom toV8 function and slow.
    //     v8SetReturnValueForMainWorld(Node*)
    //         Optimized and very fast.
    //     v8SetReturnValueForMainWorld(Window*)
    //         Uses custom toV8 function and slow.
    v8SetReturnValueForMainWorld(callbackInfo, ScriptWrappable::fromNode(impl));
}

// Special versions for DOMWindow, WorkerGlobalScope and EventTarget

template<typename CallbackInfo>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, DOMWindow* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, EventTarget* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, WorkerGlobalScope* impl)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo, typename T>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, PassRefPtr<T> impl)
{
    v8SetReturnValueForMainWorld(callbackInfo, impl.get());
}

template<typename CallbackInfo, typename T>
inline void v8SetReturnValueForMainWorld(const CallbackInfo& callbackInfo, RawPtr<T> impl)
{
    v8SetReturnValueForMainWorld(callbackInfo, impl.get());
}

template<typename CallbackInfo>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, ScriptWrappable* impl, const ScriptWrappable* wrappable)
{
    if (UNLIKELY(!impl)) {
        v8SetReturnValueNull(callbackInfo);
        return;
    }
    if (DOMDataStore::setReturnValueFast(callbackInfo.GetReturnValue(), impl, callbackInfo.Holder(), wrappable))
        return;
    v8::Local<v8::Object> wrapper = impl->wrap(callbackInfo.GetIsolate(), callbackInfo.Holder());
    v8SetReturnValue(callbackInfo, wrapper);
}

template<typename CallbackInfo>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, Node* impl, const ScriptWrappable* wrappable)
{
    if (UNLIKELY(!impl)) {
        v8SetReturnValueNull(callbackInfo);
        return;
    }
    if (DOMDataStore::setReturnValueFast(callbackInfo.GetReturnValue(), impl, callbackInfo.Holder(), wrappable))
        return;
    v8::Local<v8::Object> wrapper = ScriptWrappable::fromNode(impl)->wrap(callbackInfo.GetIsolate(), callbackInfo.Holder());
    v8SetReturnValue(callbackInfo, wrapper);
}

// Special versions for DOMWindow, WorkerGlobalScope and EventTarget

template<typename CallbackInfo>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, DOMWindow* impl, const ScriptWrappable*)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, EventTarget* impl, const ScriptWrappable*)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, WorkerGlobalScope* impl, const ScriptWrappable*)
{
    v8SetReturnValue(callbackInfo, toV8(impl, callbackInfo.Holder(), callbackInfo.GetIsolate()));
}

template<typename CallbackInfo, typename T, typename Wrappable>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, PassRefPtr<T> impl, const Wrappable* wrappable)
{
    v8SetReturnValueFast(callbackInfo, impl.get(), wrappable);
}

template<typename CallbackInfo, typename T, typename Wrappable>
inline void v8SetReturnValueFast(const CallbackInfo& callbackInfo, RawPtr<T> impl, const Wrappable* wrappable)
{
    v8SetReturnValueFast(callbackInfo, impl.get(), wrappable);
}

// Convert v8::String to a WTF::String. If the V8 string is not already
// an external string then it is transformed into an external string at this
// point to avoid repeated conversions.
inline String toCoreString(v8::Local<v8::String> value)
{
    return v8StringToWebCoreString<String>(value, Externalize);
}

inline String toCoreStringWithNullCheck(v8::Local<v8::String> value)
{
    if (value.IsEmpty() || value->IsNull())
        return String();
    return toCoreString(value);
}

inline String toCoreStringWithUndefinedOrNullCheck(v8::Local<v8::String> value)
{
    if (value.IsEmpty() || value->IsNull() || value->IsUndefined())
        return String();
    return toCoreString(value);
}

inline AtomicString toCoreAtomicString(v8::Local<v8::String> value)
{
    return v8StringToWebCoreString<AtomicString>(value, Externalize);
}

// This method will return a null String if the v8::Value does not contain a v8::String.
// It will not call ToString() on the v8::Value. If you want ToString() to be called,
// please use the TONATIVE_FOR_V8STRINGRESOURCE_*() macros instead.
inline String toCoreStringWithUndefinedOrNullCheck(v8::Local<v8::Value> value)
{
    if (value.IsEmpty() || !value->IsString())
        return String();
    return toCoreString(value.As<v8::String>());
}

// Convert a string to a V8 string.
// Return a V8 external string that shares the underlying buffer with the given
// WebCore string. The reference counting mechanism is used to keep the
// underlying buffer alive while the string is still live in the V8 engine.
inline v8::Local<v8::String> v8String(v8::Isolate* isolate, const String& string)
{
    if (string.isNull())
        return v8::String::Empty(isolate);
    return V8PerIsolateData::from(isolate)->stringCache()->v8ExternalString(isolate, string.impl());
}

inline v8::Local<v8::String> v8AtomicString(v8::Isolate* isolate, const char* str, int length = -1)
{
    ASSERT(isolate);
    v8::Local<v8::String> value;
    if (LIKELY(v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kInternalized, length).ToLocal(&value)))
        return value;
    // Immediately crashes when NewFromUtf8() fails because it only fails the
    // given str is too long.
    RELEASE_ASSERT_NOT_REACHED();
    return v8::String::Empty(isolate);
}

inline v8::Local<v8::Value> v8Undefined()
{
    return v8::Local<v8::Value>();
}

// Conversion flags, used in toIntXX/toUIntXX.
enum IntegerConversionConfiguration {
    NormalConversion,
    EnforceRange,
    Clamp
};

// Convert a value to a boolean.
CORE_EXPORT bool toBooleanSlow(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);
inline bool toBoolean(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    if (LIKELY(value->IsBoolean()))
        return value.As<v8::Boolean>()->Value();
    return toBooleanSlow(isolate, value, exceptionState);
}

// Convert a value to a 8-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-byte
CORE_EXPORT int8_t toInt8(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);

// Convert a value to a 8-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-octet
CORE_EXPORT uint8_t toUInt8(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);

// Convert a value to a 16-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-short
CORE_EXPORT int16_t toInt16(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);

// Convert a value to a 16-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-short
CORE_EXPORT uint16_t toUInt16(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);

// Convert a value to a 32-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-long
CORE_EXPORT int32_t toInt32Slow(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);
inline int32_t toInt32(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    // Fast case. The value is already a 32-bit integer.
    if (LIKELY(value->IsInt32()))
        return value.As<v8::Int32>()->Value();
    return toInt32Slow(isolate, value, configuration, exceptionState);
}

// Convert a value to a 32-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-long
CORE_EXPORT uint32_t toUInt32Slow(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);
inline uint32_t toUInt32(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    // Fast case. The value is already a 32-bit unsigned integer.
    if (LIKELY(value->IsUint32()))
        return value.As<v8::Uint32>()->Value();

    // Fast case. The value is a 32-bit signed integer with NormalConversion configuration.
    if (LIKELY(value->IsInt32() && configuration == NormalConversion))
        return value.As<v8::Int32>()->Value();

    return toUInt32Slow(isolate, value, configuration, exceptionState);
}

// Convert a value to a 64-bit signed integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-long-long
CORE_EXPORT int64_t toInt64Slow(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);
inline int64_t toInt64(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    // Clamping not supported for int64_t/long long int. See Source/wtf/MathExtras.h.
    ASSERT(configuration != Clamp);

    // Fast case. The value is a 32-bit integer.
    if (LIKELY(value->IsInt32()))
        return value.As<v8::Int32>()->Value();

    return toInt64Slow(isolate, value, configuration, exceptionState);
}

// Convert a value to a 64-bit unsigned integer. The conversion fails if the
// value cannot be converted to a number or the range violated per WebIDL:
// http://www.w3.org/TR/WebIDL/#es-unsigned-long-long
CORE_EXPORT uint64_t toUInt64Slow(v8::Isolate*, v8::Local<v8::Value>, IntegerConversionConfiguration, ExceptionState&);
inline uint64_t toUInt64(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
{
    // Fast case. The value is a 32-bit unsigned integer.
    if (LIKELY(value->IsUint32()))
        return value.As<v8::Uint32>()->Value();

    if (LIKELY(value->IsInt32() && configuration == NormalConversion))
        return value.As<v8::Int32>()->Value();

    return toUInt64Slow(isolate, value, configuration, exceptionState);
}

// Convert a value to a double precision float, which might fail.
CORE_EXPORT double toDoubleSlow(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);
inline double toDouble(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    if (LIKELY(value->IsNumber()))
        return value.As<v8::Number>()->Value();
    return toDoubleSlow(isolate, value, exceptionState);
}

// Convert a value to a double precision float, throwing on non-finite values.
CORE_EXPORT double toRestrictedDouble(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);

// Convert a value to a single precision float, which might fail.
inline float toFloat(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    return static_cast<float>(toDouble(isolate, value, exceptionState));
}

// Convert a value to a single precision float, throwing on non-finite values.
CORE_EXPORT float toRestrictedFloat(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);

// Converts a value to a String, throwing if any code unit is outside 0-255.
CORE_EXPORT String toByteString(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);

// Converts a value to a String, replacing unmatched UTF-16 surrogates with replacement characters.
CORE_EXPORT String toUSVString(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&);

inline v8::Local<v8::Boolean> v8Boolean(bool value, v8::Isolate* isolate)
{
    return value ? v8::True(isolate) : v8::False(isolate);
}

inline double toCoreDate(v8::Isolate* isolate, v8::Local<v8::Value> object)
{
    if (object->IsDate())
        return object.As<v8::Date>()->ValueOf();
    if (object->IsNumber())
        return object.As<v8::Number>()->Value();
    return std::numeric_limits<double>::quiet_NaN();
}

inline v8::MaybeLocal<v8::Value> v8DateOrNaN(v8::Isolate* isolate, double value)
{
    ASSERT(isolate);
    return v8::Date::New(isolate->GetCurrentContext(), std::isfinite(value) ? value : std::numeric_limits<double>::quiet_NaN());
}

// FIXME: Remove the special casing for NodeFilter and XPathNSResolver.
PassRefPtrWillBeRawPtr<NodeFilter> toNodeFilter(v8::Local<v8::Value>, v8::Local<v8::Object>, ScriptState*);
XPathNSResolver* toXPathNSResolver(ScriptState*, v8::Local<v8::Value>);

bool toV8Sequence(v8::Local<v8::Value>, uint32_t& length, v8::Isolate*, ExceptionState&);

// Converts a JavaScript value to an array as per the Web IDL specification:
// http://www.w3.org/TR/2012/CR-WebIDL-20120419/#es-array
template <typename T, typename V8T>
Vector<RefPtr<T>> toRefPtrNativeArrayUnchecked(v8::Local<v8::Value> v8Value, uint32_t length, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    Vector<RefPtr<T>> result;
    result.reserveInitialCapacity(length);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(v8Value);
    v8::TryCatch block;
    for (uint32_t i = 0; i < length; ++i) {
        v8::Local<v8::Value> element;
        if (!v8Call(object->Get(isolate->GetCurrentContext(), i), element, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return Vector<RefPtr<T>>();
        }
        if (V8T::hasInstance(element, isolate)) {
            v8::Local<v8::Object> elementObject = v8::Local<v8::Object>::Cast(element);
            result.uncheckedAppend(V8T::toImpl(elementObject));
        } else {
            exceptionState.throwTypeError("Invalid Array element type");
            return Vector<RefPtr<T>>();
        }
    }
    return result;
}

template <typename T, typename V8T>
Vector<RefPtr<T>> toRefPtrNativeArray(v8::Local<v8::Value> value, int argumentIndex, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(v8Value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notAnArrayTypeArgumentOrValue(argumentIndex));
        return Vector<RefPtr<T>>();
    }
    return toRefPtrNativeArrayUnchecked<T, V8T>(v8Value, length, isolate, exceptionState);
}

template <typename T, typename V8T>
Vector<RefPtr<T>> toRefPtrNativeArray(v8::Local<v8::Value> value, const String& propertyName, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(v8Value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notASequenceTypeProperty(propertyName));
        return Vector<RefPtr<T>>();
    }
    return toRefPtrNativeArrayUnchecked<T, V8T>(v8Value, length, isolate, exceptionState);
}

template <typename T, typename V8T>
WillBeHeapVector<RefPtrWillBeMember<T>> toRefPtrWillBeMemberNativeArray(v8::Local<v8::Value> value, int argumentIndex, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(v8Value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notAnArrayTypeArgumentOrValue(argumentIndex));
        return WillBeHeapVector<RefPtrWillBeMember<T>>();
    }

    WillBeHeapVector<RefPtrWillBeMember<T>> result;
    result.reserveInitialCapacity(length);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(v8Value);
    v8::TryCatch block(isolate);

    for (uint32_t i = 0; i < length; ++i) {
        v8::Local<v8::Value> element;
        if (!v8Call(object->Get(isolate->GetCurrentContext(), i), element, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return WillBeHeapVector<RefPtrWillBeMember<T>>();
        }
        if (V8T::hasInstance(element, isolate)) {
            v8::Local<v8::Object> elementObject = v8::Local<v8::Object>::Cast(element);
            result.uncheckedAppend(V8T::toImpl(elementObject));
        } else {
            exceptionState.throwTypeError("Invalid Array element type");
            return WillBeHeapVector<RefPtrWillBeMember<T>>();
        }
    }
    return result;
}

template <typename T, typename V8T>
WillBeHeapVector<RefPtrWillBeMember<T>> toRefPtrWillBeMemberNativeArray(v8::Local<v8::Value> value, const String& propertyName, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(v8Value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notASequenceTypeProperty(propertyName));
        return WillBeHeapVector<RefPtrWillBeMember<T>>();
    }

    WillBeHeapVector<RefPtrWillBeMember<T>> result;
    result.reserveInitialCapacity(length);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(v8Value);
    v8::TryCatch block;
    for (uint32_t i = 0; i < length; ++i) {
        v8::Local<v8::Value> element;
        if (!v8Call(object->Get(isolate->GetCurrentContext(), i), element, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return WillBeHeapVector<RefPtrWillBeMember<T>>();
        }
        if (V8T::hasInstance(element, isolate)) {
            v8::Local<v8::Object> elementObject = v8::Local<v8::Object>::Cast(element);
            result.uncheckedAppend(V8T::toImpl(elementObject));
        } else {
            exceptionState.throwTypeError("Invalid Array element type");
            return WillBeHeapVector<RefPtrWillBeMember<T>>();
        }
    }
    return result;
}

template <typename T, typename V8T>
HeapVector<Member<T>> toMemberNativeArray(v8::Local<v8::Value> value, int argumentIndex, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(v8Value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notAnArrayTypeArgumentOrValue(argumentIndex));
        return HeapVector<Member<T>>();
    }

    HeapVector<Member<T>> result;
    result.reserveInitialCapacity(length);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(v8Value);
    v8::TryCatch block(isolate);

    for (uint32_t i = 0; i < length; ++i) {
        v8::Local<v8::Value> element;
        if (!v8Call(object->Get(isolate->GetCurrentContext(), i), element, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return HeapVector<Member<T>>();
        }
        if (V8T::hasInstance(element, isolate)) {
            v8::Local<v8::Object> elementObject = v8::Local<v8::Object>::Cast(element);
            result.uncheckedAppend(V8T::toImpl(elementObject));
        } else {
            exceptionState.throwTypeError("Invalid Array element type");
            return HeapVector<Member<T>>();
        }
    }
    return result;
}

// Converts a JavaScript value to an array as per the Web IDL specification:
// http://www.w3.org/TR/2012/CR-WebIDL-20120419/#es-array
template <typename VectorType>
VectorType toImplArray(v8::Local<v8::Value> value, int argumentIndex, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    typedef typename VectorType::ValueType ValueType;
    typedef NativeValueTraits<ValueType> TraitsType;

    uint32_t length = 0;
    if (value->IsArray()) {
        length = v8::Local<v8::Array>::Cast(value)->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notAnArrayTypeArgumentOrValue(argumentIndex));
        return VectorType();
    }

    if (length > WTF::kGenericMaxDirectMapped / sizeof(ValueType)) {
        exceptionState.throwTypeError("Array length exceeds supported limit.");
        return VectorType();
    }

    VectorType result;
    result.reserveInitialCapacity(length);
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    v8::TryCatch block(isolate);

    for (uint32_t i = 0; i < length; ++i) {
        v8::Local<v8::Value> element;
        if (!v8Call(object->Get(isolate->GetCurrentContext(), i), element, block)) {
            exceptionState.rethrowV8Exception(block.Exception());
            return VectorType();
        }
        result.uncheckedAppend(TraitsType::nativeValue(isolate, element, exceptionState));
        if (exceptionState.hadException())
            return VectorType();
    }
    return result;
}

template <typename VectorType>
VectorType toImplArray(const Vector<ScriptValue>& value, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    VectorType result;
    typedef typename VectorType::ValueType ValueType;
    typedef NativeValueTraits<ValueType> TraitsType;
    result.reserveInitialCapacity(value.size());
    for (unsigned i = 0; i < value.size(); ++i) {
        result.uncheckedAppend(TraitsType::nativeValue(isolate, value[i].v8Value(), exceptionState));
        if (exceptionState.hadException())
            return VectorType();
    }
    return result;
}

template <typename VectorType>
VectorType toImplArguments(const v8::FunctionCallbackInfo<v8::Value>& info, int startIndex, ExceptionState& exceptionState)
{
    VectorType result;
    typedef typename VectorType::ValueType ValueType;
    typedef NativeValueTraits<ValueType> TraitsType;
    int length = info.Length();
    if (startIndex < length) {
        result.reserveInitialCapacity(length - startIndex);
        for (int i = startIndex; i < length; ++i) {
            result.uncheckedAppend(TraitsType::nativeValue(info.GetIsolate(), info[i], exceptionState));
            if (exceptionState.hadException())
                return VectorType();
        }
    }
    return result;
}

// Validates that the passed object is a sequence type per WebIDL spec
// http://www.w3.org/TR/2012/CR-WebIDL-20120419/#es-sequence
inline bool toV8Sequence(v8::Local<v8::Value> value, uint32_t& length, v8::Isolate* isolate, ExceptionState& exceptionState)
{
    // Attempt converting to a sequence if the value is not already an array but is
    // any kind of object except for a native Date object or a native RegExp object.
    ASSERT(!value->IsArray());
    // FIXME: Do we really need to special case Date and RegExp object?
    // https://www.w3.org/Bugs/Public/show_bug.cgi?id=22806
    if (!value->IsObject() || value->IsDate() || value->IsRegExp()) {
        // The caller is responsible for reporting a TypeError.
        return false;
    }

    v8::Local<v8::Value> v8Value(v8::Local<v8::Value>::New(isolate, value));
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(v8Value);
    v8::Local<v8::String> lengthSymbol = v8AtomicString(isolate, "length");

    // FIXME: The specification states that the length property should be used as fallback, if value
    // is not a platform object that supports indexed properties. If it supports indexed properties,
    // length should actually be one greater than value's maximum indexed property index.

    v8::TryCatch block(isolate);
    v8::Local<v8::Value> lengthValue;
    if (!v8Call(object->Get(isolate->GetCurrentContext(), lengthSymbol), lengthValue, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return false;
    }

    if (lengthValue->IsUndefined() || lengthValue->IsNull()) {
        // The caller is responsible for reporting a TypeError.
        return false;
    }

    uint32_t sequenceLength;
    if (!v8Call(lengthValue->Uint32Value(isolate->GetCurrentContext()), sequenceLength, block)) {
        exceptionState.rethrowV8Exception(block.Exception());
        return false;
    }

    length = sequenceLength;
    return true;
}

template<>
struct NativeValueTraits<String> {
    static inline String nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        V8StringResource<> stringValue(value);
        if (!stringValue.prepare(exceptionState))
            return String();
        return stringValue;
    }
};

template<>
struct NativeValueTraits<int> {
    static inline int nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return toInt32(isolate, value, NormalConversion, exceptionState);
    }
};

template<>
struct NativeValueTraits<unsigned> {
    static inline unsigned nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return toUInt32(isolate, value, NormalConversion, exceptionState);
    }
};

template<>
struct NativeValueTraits<float> {
    static inline float nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return toFloat(isolate, value, exceptionState);
    }
};

template<>
struct NativeValueTraits<double> {
    static inline double nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return toDouble(isolate, value, exceptionState);
    }
};

template<>
struct NativeValueTraits<v8::Local<v8::Value>> {
    static inline v8::Local<v8::Value> nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return value;
    }
};

template<>
struct NativeValueTraits<ScriptValue> {
    static inline ScriptValue nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return ScriptValue(ScriptState::current(isolate), value);
    }
};

template <typename T>
struct NativeValueTraits<Vector<T>> {
    static inline Vector<T> nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
    {
        return toImplArray<Vector<T>>(value, 0, isolate, exceptionState);
    }
};

using JSONValuePtr = PassRefPtr<JSONValue>;
template <>
struct NativeValueTraits<JSONValuePtr> {
    CORE_EXPORT static JSONValuePtr nativeValue(v8::Isolate*, v8::Local<v8::Value>, ExceptionState&, int maxDepth = JSONValue::maxDepth);
};

CORE_EXPORT v8::Isolate* toIsolate(ExecutionContext*);
CORE_EXPORT v8::Isolate* toIsolate(LocalFrame*);

DOMWindow* toDOMWindow(v8::Isolate*, v8::Local<v8::Value>);
DOMWindow* toDOMWindow(v8::Local<v8::Context>);
LocalDOMWindow* enteredDOMWindow(v8::Isolate*);
CORE_EXPORT LocalDOMWindow* currentDOMWindow(v8::Isolate*);
LocalDOMWindow* callingDOMWindow(v8::Isolate*);
CORE_EXPORT ExecutionContext* toExecutionContext(v8::Local<v8::Context>);
CORE_EXPORT ExecutionContext* currentExecutionContext(v8::Isolate*);
CORE_EXPORT ExecutionContext* callingExecutionContext(v8::Isolate*);

// Returns a V8 context associated with a ExecutionContext and a DOMWrapperWorld.
// This method returns an empty context if there is no frame or the frame is already detached.
CORE_EXPORT v8::Local<v8::Context> toV8Context(ExecutionContext*, DOMWrapperWorld&);
// Returns a V8 context associated with a Frame and a DOMWrapperWorld.
// This method returns an empty context if the frame is already detached.
CORE_EXPORT v8::Local<v8::Context> toV8Context(Frame*, DOMWrapperWorld&);
// Like toV8Context but also returns the context if the frame is already detached.
CORE_EXPORT v8::Local<v8::Context> toV8ContextEvenIfDetached(Frame*, DOMWrapperWorld&);

// Returns the frame object of the window object associated with
// a context, if the window is currently being displayed in a Frame.
CORE_EXPORT Frame* toFrameIfNotDetached(v8::Local<v8::Context>);

CORE_EXPORT EventTarget* toEventTarget(v8::Isolate*, v8::Local<v8::Value>);

// If the current context causes out of memory, JavaScript setting
// is disabled and it returns true.
bool handleOutOfMemory();
void crashIfV8IsDead();

inline bool isUndefinedOrNull(v8::Local<v8::Value> value)
{
    return value.IsEmpty() || value->IsNull() || value->IsUndefined();
}
v8::Local<v8::Function> getBoundFunction(v8::Local<v8::Function>);

// Attaches |environment| to |function| and returns it.
inline v8::Local<v8::Function> createClosure(v8::FunctionCallback function, v8::Local<v8::Value> environment, v8::Isolate* isolate)
{
    return v8::Function::New(isolate, function, environment);
}

// FIXME: This will be soon embedded in the generated code.
template<typename Collection> static void indexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    Collection* collection = toScriptWrappable(info.Holder())->toImpl<Collection>();
    int length = collection->length();
    v8::Local<v8::Array> properties = v8::Array::New(info.GetIsolate(), length);
    for (int i = 0; i < length; ++i) {
        v8::Local<v8::Integer> integer = v8::Integer::New(info.GetIsolate(), i);
        if (!v8CallBoolean(properties->Set(info.GetIsolate()->GetCurrentContext(), integer, integer)))
            return;
    }
    v8SetReturnValue(info, properties);
}

CORE_EXPORT bool isValidEnum(const String& value, const char** validValues, size_t length, const String& enumName, ExceptionState&);
CORE_EXPORT bool isValidEnum(const Vector<String>& values, const char** validValues, size_t length, const String& enumName, ExceptionState&);

// These methods store hidden values into an array that is stored in the internal field of a DOM wrapper.
bool addHiddenValueToArray(v8::Isolate*, v8::Local<v8::Object>, v8::Local<v8::Value>, int cacheIndex);
void removeHiddenValueFromArray(v8::Isolate*, v8::Local<v8::Object>, v8::Local<v8::Value>, int cacheIndex);
CORE_EXPORT void moveEventListenerToNewWrapper(v8::Isolate*, v8::Local<v8::Object>, EventListener* oldValue, v8::Local<v8::Value> newValue, int cacheIndex);

// Result values for platform object 'deleter' methods,
// http://www.w3.org/TR/WebIDL/#delete
enum DeleteResult {
    DeleteSuccess,
    DeleteReject,
    DeleteUnknownProperty
};

class V8IsolateInterruptor : public ThreadState::Interruptor {
public:
    explicit V8IsolateInterruptor(v8::Isolate* isolate)
        : m_isolate(isolate)
    {
    }

    static void onInterruptCallback(v8::Isolate* isolate, void* data)
    {
        V8IsolateInterruptor* interruptor = reinterpret_cast<V8IsolateInterruptor*>(data);
        interruptor->onInterrupted();
    }

    void requestInterrupt() override
    {
        m_isolate->RequestInterrupt(&onInterruptCallback, this);
    }

private:
    v8::Isolate* m_isolate;
};

class DevToolsFunctionInfo final {
public:
    explicit DevToolsFunctionInfo(v8::Local<v8::Function>& function)
        : m_scriptId(0)
        , m_lineNumber(1)
        , m_function(function)
    {
        ASSERT(!m_function.IsEmpty());
    }

    DevToolsFunctionInfo(int scriptId, const String& resourceName, int lineNumber)
        : m_scriptId(scriptId)
        , m_lineNumber(lineNumber)
        , m_resourceName(resourceName)
    {
    }

    int scriptId() const;
    int lineNumber() const;
    String resourceName() const;

private:
    void ensureInitialized() const;

    mutable int m_scriptId;
    mutable int m_lineNumber;
    mutable String m_resourceName;
    mutable v8::Local<v8::Function> m_function;
};

PassRefPtr<TraceEvent::ConvertableToTraceFormat> devToolsTraceEventData(v8::Isolate*, ExecutionContext*, v8::Local<v8::Function>);

// Callback functions used by generated code.
CORE_EXPORT void v8ConstructorAttributeGetter(v8::Local<v8::Name> propertyName, const v8::PropertyCallbackInfo<v8::Value>&);

typedef void (*InstallTemplateFunction)(v8::Local<v8::FunctionTemplate>, v8::Isolate*);

} // namespace blink

#endif // V8Binding_h
