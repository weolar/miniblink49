// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/compositorworker/CompositorWorkerManager.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8Initializer.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/WebThreadSupportingGC.h"
#include "wtf/MainThread.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

namespace {

static CompositorWorkerManager* s_instance = nullptr;

static Mutex& singletonMutex()
{
    AtomicallyInitializedStaticReference(Mutex, mutex, new Mutex);
    return mutex;
}

static void destroyThread(WebThreadSupportingGC* thread)
{
    delete thread;
}

} // namespace

void CompositorWorkerManager::initialize()
{
    MutexLocker lock(singletonMutex());
    ASSERT(!s_instance);
    s_instance = new CompositorWorkerManager();
}

void CompositorWorkerManager::shutdown()
{
    MutexLocker lock(singletonMutex());
    ASSERT(s_instance);
    delete s_instance;
    s_instance = nullptr;
}

CompositorWorkerManager* CompositorWorkerManager::instance()
{
    MutexLocker lock(singletonMutex());
    ASSERT(s_instance);
    return s_instance;
}

CompositorWorkerManager::CompositorWorkerManager()
{
}

CompositorWorkerManager::~CompositorWorkerManager()
{
}

WebThreadSupportingGC& CompositorWorkerManager::compositorWorkerThread()
{
    MutexLocker lock(m_mutex);
    if (!m_thread) {
        ASSERT(isMainThread());
        ASSERT(!m_workerCount);
        m_thread = WebThreadSupportingGC::create("CompositorWorker Thread");
    }
    return *m_thread.get();
}

void CompositorWorkerManager::initializeBackingThread()
{
    ASSERT(m_thread->isCurrentThread());
    MutexLocker lock(m_mutex);
    ++m_workerCount;
    if (m_workerCount > 1)
        return;

    m_thread->initialize();

    // Initialize the isolate at the same time.
    ASSERT(!m_isolate);
    m_isolate = V8PerIsolateData::initialize();
    V8Initializer::initializeWorker(m_isolate);

    m_interruptor = adoptPtr(new V8IsolateInterruptor(m_isolate));
    ThreadState::current()->addInterruptor(m_interruptor.get());
    ThreadState::current()->registerTraceDOMWrappers(m_isolate, V8GCController::traceDOMWrappers);
}

void CompositorWorkerManager::shutdownBackingThread()
{
    MutexLocker lock(m_mutex);
    ASSERT(m_thread->isCurrentThread());
    ASSERT(m_workerCount > 0);
    --m_workerCount;
    if (m_workerCount == 0) {
        m_thread->shutdown();
        Platform::current()->mainThread()->postTask(FROM_HERE, threadSafeBind(destroyThread, AllowCrossThreadAccess(m_thread.leakPtr())));
        m_thread = nullptr;
    }
}

v8::Isolate* CompositorWorkerManager::initializeIsolate()
{
    MutexLocker lock(m_mutex);
    ASSERT(m_thread->isCurrentThread());
    ASSERT(m_isolate);
    // It is safe to use the existing isolate even if TerminateExecution() has been
    // called on it, without calling CancelTerminateExecution().
    return m_isolate;
}

void CompositorWorkerManager::willDestroyIsolate()
{
    MutexLocker lock(m_mutex);
    ASSERT(m_thread->isCurrentThread());
    if (m_workerCount == 1) {
        V8PerIsolateData::willBeDestroyed(m_isolate);
        ThreadState::current()->removeInterruptor(m_interruptor.get());
    }
}

void CompositorWorkerManager::destroyIsolate()
{
    MutexLocker lock(m_mutex);
    if (!m_thread) {
        ASSERT(m_workerCount == 0);
        V8PerIsolateData::destroy(m_isolate);
        m_isolate = nullptr;
    }
}

void CompositorWorkerManager::terminateV8Execution()
{
    MutexLocker lock(m_mutex);
    ASSERT(isMainThread());
    if (m_workerCount > 1)
        return;

    v8::V8::TerminateExecution(m_isolate);
}

} // namespace blink
