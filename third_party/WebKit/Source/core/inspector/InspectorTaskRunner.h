// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorTaskRunner_h
#define InspectorTaskRunner_h

#include "core/CoreExport.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include <v8.h>

namespace blink {

class CORE_EXPORT InspectorTaskRunner final {
    WTF_MAKE_NONCOPYABLE(InspectorTaskRunner);
public:
    explicit InspectorTaskRunner(v8::Isolate*);
    ~InspectorTaskRunner();

    class Task {
    public:
        virtual ~Task() { }
        virtual void run() = 0;
    };
    // This method can be called on any thread. It is caller's responsibility to make sure that
    // this V8Debugger and corresponding v8::Isolate exist while this method is running.
    void interruptAndRun(PassOwnPtr<Task>);
    void runPendingTasks();

    class CORE_EXPORT IgnoreInterruptsScope final {
    public:
        explicit IgnoreInterruptsScope(InspectorTaskRunner*);
        ~IgnoreInterruptsScope();

    private:
        bool m_wasIgnoring;
        InspectorTaskRunner* m_taskRunner;
    };

private:
    static void v8InterruptCallback(v8::Isolate*, void* data);

    v8::Isolate* m_isolate;
    class ThreadSafeTaskQueue;
    OwnPtr<ThreadSafeTaskQueue> m_taskQueue;
    bool m_ignoreInterrupts;
};

} // namespace blink


#endif // !defined(InspectorTaskRunner_h)
