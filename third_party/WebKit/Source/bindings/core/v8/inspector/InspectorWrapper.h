// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorWrapper_h
#define InspectorWrapper_h

#include "bindings/core/v8/V8HiddenValue.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include <v8.h>

namespace blink {

class InspectorWrapperBase {
public:
    struct V8MethodConfiguration {
        const char* name;
        v8::FunctionCallback callback;
    };

    struct V8AttributeConfiguration {
        const char* name;
        v8::AccessorNameGetterCallback callback;
    };

    static v8::Local<v8::FunctionTemplate> createWrapperTemplate(v8::Isolate*, const char* className, const Vector<V8MethodConfiguration>& methods, const Vector<V8AttributeConfiguration>& attributes);

protected:
    static v8::Local<v8::Object> createWrapper(v8::Local<v8::FunctionTemplate>, v8::Local<v8::Context>);
    static void* unwrap(v8::Local<v8::Object>, const char* name);

    static v8::Local<v8::String> v8InternalizedString(v8::Isolate*, const char* name);
};

template<typename T, bool = IsGarbageCollectedType<T>::value>
class InspectorWrapperTypeTrait {
public:
    using PassType = PassRefPtr<T>;
    using Type = RefPtr<T>;
};

template<typename T>
class InspectorWrapperTypeTrait<T, true> {
public:
    using PassType = PassRefPtrWillBeRawPtr<T>;
    using Type = RefPtrWillBeRawPtr<T>;
};

template<class T, char* const hiddenPropertyName, char* const className>
class InspectorWrapper final : public InspectorWrapperBase {
public:
    class WeakCallbackData final {
    public:
        WeakCallbackData(v8::Isolate* isolate, typename InspectorWrapperTypeTrait<T>::PassType impl, v8::Local<v8::Object> wrapper)
            : m_impl(impl)
            , m_persistent(isolate, wrapper)
        {
            m_persistent.SetWeak(this, &WeakCallbackData::weakCallback, v8::WeakCallbackType::kParameter);
        }

        typename InspectorWrapperTypeTrait<T>::Type m_impl;

    private:
        static void weakCallback(const v8::WeakCallbackInfo<WeakCallbackData>& info)
        {
            delete info.GetParameter();
        }

        v8::Global<v8::Object> m_persistent;
    };

    static v8::Local<v8::FunctionTemplate> createWrapperTemplate(v8::Isolate* isolate, const Vector<V8MethodConfiguration>& methods, const Vector<V8AttributeConfiguration>& attributes)
    {
        return InspectorWrapperBase::createWrapperTemplate(isolate, className, methods, attributes);
    }

    static v8::Local<v8::Object> wrap(v8::Local<v8::FunctionTemplate> constructorTemplate, v8::Local<v8::Context> context, typename blink::InspectorWrapperTypeTrait<T>::PassType object)
    {
        v8::Context::Scope contextScope(context);
        v8::Local<v8::Object> result = InspectorWrapperBase::createWrapper(constructorTemplate, context);
        if (result.IsEmpty())
            return v8::Local<v8::Object>();
        typename blink::InspectorWrapperTypeTrait<T>::Type impl(object);
        v8::Isolate* isolate = context->GetIsolate();
        v8::Local<v8::External> objectReference = v8::External::New(isolate, new WeakCallbackData(isolate, impl, result));
#if V8_MINOR_VERSION == 7
        blink::V8HiddenValue::setHiddenValue(isolate, result, v8InternalizedString(isolate, hiddenPropertyName), objectReference);
#else
        result->SetHiddenValue(v8InternalizedString(isolate, hiddenPropertyName), objectReference);
#endif
        return result;
    }
    static T* unwrap(v8::Local<v8::Object> object)
    {
        void* data = InspectorWrapperBase::unwrap(object, hiddenPropertyName);
        if (!data)
            return nullptr;
        return reinterpret_cast<WeakCallbackData*>(data)->m_impl.get();
    }
};

} // namespace blink

#endif // InspectorWrapper_h
