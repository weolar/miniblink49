// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_CONTROLLER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_CONTROLLER_H_

//#include "base/macros.h"
#include "third_party/WebKit/Source/platform/PlatformExport.h"
#include "third_party/WebKit/Source/platform/heap/ThreadState.h"
#include "v8.h"

#if V8_MAJOR_VERSION >= 7

namespace blink {

class ThreadState;

// UnifiedHeapController ties V8's garbage collector to Oilpan for performing a
// garbage collection across both managed heaps.
//
// Unified heap garbage collections are triggered by V8 and mark the full
// transitive closure of V8 and Blink (Oilpan) objects. The garbage collection
// is initially triggered by V8. Both collecters report live references using
// the EmbedderHeapTracer APIs. V8 and Blink both run separate incremental
// marking steps to compute their live closures, respectively. The final atomic
// pause is then initiated by V8 and triggers a fixed-point computation between
// V8 and Blink where both GCs report live references to each other and drain
// their marking work lists until they are empty and no new references are
// found.
//
// Oilpan does not consider references from DOM wrappers (JavaScript objects on
// V8's heap) as roots for such garbage collections.
class PLATFORM_EXPORT UnifiedHeapController final : public v8::EmbedderHeapTracer {
    //DISALLOW_IMPLICIT_CONSTRUCTORS(UnifiedHeapController);
public:
    UnifiedHeapController(ThreadState* threadState);
    ~UnifiedHeapController();

    void attachIsolate(v8::Isolate* isolate)
    {
        isolate_ = isolate;
    }

    void detachIsolate(v8::Isolate* isolate)
    {
        isolate_ = nullptr;
    }

    std::vector<std::pair<void*, void*>>* leakV8References();

    // v8::EmbedderHeapTracer implementation.
    void TracePrologue() final;
    void TraceEpilogue() final;
    void EnterFinalPause(EmbedderStackState) final;
    void RegisterV8References(const std::vector<std::pair<void*, void*>>&) final;
    bool AdvanceTracing(double) final;
    bool IsTracingDone() final;
    bool IsRootForNonTracingGC(const v8::TracedGlobal<v8::Value>&) final;

    ThreadState* thread_state() const { return m_threadState; }

private:
    static bool IsRootForNonTracingGCInternal(const v8::TracedGlobal<v8::Value>& handle);

    ThreadState* const m_threadState;

    // Returns whether the Blink heap has been fully processed.
    bool m_isTracingDone = false;

    WTF::HashMap<void*, void*> m_v8References;
};

}  // namespace blink

#endif
#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_HEAP_UNIFIED_HEAP_CONTROLLER_H_
