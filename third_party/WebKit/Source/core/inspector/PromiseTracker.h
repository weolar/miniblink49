// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PromiseTracker_h
#define PromiseTracker_h

#include "bindings/core/v8/V8GlobalValueMap.h"
#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/InspectorTypeBuilder.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include <v8.h>

namespace blink {

class ScriptState;
class ScriptValue;

class PromiseTracker final : public NoBaseWillBeGarbageCollectedFinalized<PromiseTracker> {
    WTF_MAKE_NONCOPYABLE(PromiseTracker);
public:
    class CORE_EXPORT Listener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~Listener() { }
        virtual void didUpdatePromise(InspectorFrontend::Debugger::EventType::Enum, PassRefPtr<TypeBuilder::Debugger::PromiseDetails>) = 0;
    };

    static PassOwnPtrWillBeRawPtr<PromiseTracker> create(Listener* listener, v8::Isolate* isolate)
    {
        return adoptPtrWillBeNoop(new PromiseTracker(listener, isolate));
    }

    ~PromiseTracker();

    bool isEnabled() const { return m_isEnabled; }
    void setEnabled(bool enabled, bool captureStacks);
    void clear();
    void didReceiveV8PromiseEvent(ScriptState*, v8::Local<v8::Object> promise, v8::Local<v8::Value> parentPromise, int status);
    ScriptValue promiseById(int promiseId);

    DECLARE_TRACE();

private:
    PromiseTracker(Listener*, v8::Isolate*);

    int circularSequentialId();
    int promiseId(v8::Local<v8::Object> promise, bool* isNewPromise);

    int m_circularSequentialId;
    bool m_isEnabled;
    bool m_captureStacks;
    RawPtrWillBeMember<Listener> m_listener;

    v8::Isolate* m_isolate;
    v8::Persistent<v8::NativeWeakMap> m_promiseToId;

    WeakPtrFactory<PromiseTracker> m_weakPtrFactory;

    class PromiseWeakCallbackData;
    class IdToPromiseMapTraits : public V8GlobalValueMapTraits<int, v8::Object, v8::kWeakWithParameter> {
    public:
        // Weak traits:
        typedef PromiseWeakCallbackData WeakCallbackDataType;
        typedef v8::GlobalValueMap<int, v8::Object, IdToPromiseMapTraits> MapType;

        static WeakCallbackDataType* WeakCallbackParameter(MapType*, int key, v8::Local<v8::Object>& value);
        static void OnWeakCallback(const v8::WeakCallbackInfo<WeakCallbackDataType>&) { }
        // This method will be called if the value is removed from the map.
        static void DisposeCallbackData(WeakCallbackDataType*);
        // This method is called if weakly referenced value is collected.
        static void DisposeWeak(const v8::WeakCallbackInfo<WeakCallbackDataType>&);

        static MapType* MapFromWeakCallbackInfo(const v8::WeakCallbackInfo<WeakCallbackDataType>&);
        static int KeyFromWeakCallbackInfo(const v8::WeakCallbackInfo<WeakCallbackDataType>&);
    };

    IdToPromiseMapTraits::MapType m_idToPromise;
};

} // namespace blink

#endif // !defined(PromiseTracker_h)
