// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositorWorkerManager_h
#define CompositorWorkerManager_h

#include "modules/ModulesExport.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/ThreadingPrimitives.h"
#include <v8.h>

namespace blink {

class V8IsolateInterruptor;
class WebThreadSupportingGC;

class MODULES_EXPORT CompositorWorkerManager final {
public:
    static void initialize();
    static void shutdown();

    static CompositorWorkerManager* instance();

    // Returns the thread used for compositor workers. This creates a new thread if a
    // thread doesn't already exist.
    WebThreadSupportingGC& compositorWorkerThread();

    // Attempts to initialize/shutdown a thread if necessary. Does nothing if the thread
    // is already initialized, or if the thread has more than one active workers at the
    // time of shutdown.
    void initializeBackingThread();
    void shutdownBackingThread();

    v8::Isolate* initializeIsolate();
    void willDestroyIsolate();
    void destroyIsolate();
    void terminateV8Execution();

private:
    friend class CompositorWorkerManagerTest;

    CompositorWorkerManager();
    ~CompositorWorkerManager();

    Mutex m_mutex;
    OwnPtr<WebThreadSupportingGC> m_thread;
    int m_workerCount = 0;
    v8::Isolate* m_isolate = nullptr;
    OwnPtr<V8IsolateInterruptor> m_interruptor;
};

} // namespace blink

#endif // CompositorWorkerManager_h
