// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/compositorworker/CompositorWorkerManager.h"

#include "bindings/core/v8/ScriptSourceCode.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/testing/DummyPageHolder.h"
#include "core/workers/WorkerLoaderProxy.h"
#include "core/workers/WorkerObjectProxy.h"
#include "core/workers/WorkerThreadStartupData.h"
#include "modules/compositorworker/CompositorWorkerThread.h"
#include "platform/NotImplemented.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebWaitableEvent.h"
#include <gtest/gtest.h>

namespace blink {
namespace {

class TestCompositorWorkerThread : public CompositorWorkerThread {
public:
    TestCompositorWorkerThread(WorkerLoaderProxyProvider* loaderProxyProvider, WorkerObjectProxy& objectProxy, double timeOrigin, WebWaitableEvent* startEvent)
        : CompositorWorkerThread(WorkerLoaderProxy::create(loaderProxyProvider), objectProxy, timeOrigin)
        , m_startEvent(startEvent)
    {
    }

    ~TestCompositorWorkerThread() override { }

    void setCallbackAfterV8Termination(PassOwnPtr<Function<void()>> callback)
    {
        m_v8TerminationCallback = callback;
    }

private:
    // WorkerThread:
    void didStartRunLoop() override
    {
        m_startEvent->signal();
    }
    void terminateV8Execution() override
    {
        CompositorWorkerThread::terminateV8Execution();
        if (m_v8TerminationCallback)
            (*m_v8TerminationCallback)();
    }

    WebWaitableEvent* m_startEvent;
    OwnPtr<Function<void()>> m_v8TerminationCallback;
};

// A null WorkerObjectProxy, supplied when creating CompositorWorkerThreads.
class TestCompositorWorkerObjectProxy : public WorkerObjectProxy {
public:
    static PassOwnPtr<TestCompositorWorkerObjectProxy> create(ExecutionContext* context)
    {
        return adoptPtr(new TestCompositorWorkerObjectProxy(context));
    }

    // (Empty) WorkerReportingProxy implementation:
    virtual void reportException(const String& errorMessage, int lineNumber, int columnNumber, const String& sourceURL, int exceptionId) { }
    void reportConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) override { }
    void postMessageToPageInspector(const String&) override { }
    void postWorkerConsoleAgentEnabled() override { }

    void didEvaluateWorkerScript(bool success) override { }
    void workerGlobalScopeStarted(WorkerGlobalScope*) override { }
    void workerGlobalScopeClosed() override { }
    void workerThreadTerminated() override { }
    void willDestroyWorkerGlobalScope() override { }
private:
    TestCompositorWorkerObjectProxy(ExecutionContext* context)
        : WorkerObjectProxy(context, nullptr)
    {
    }
};

} // namespace

class CompositorWorkerManagerTest : public ::testing::Test {
public:
    void SetUp() override
    {
        m_page = DummyPageHolder::create();
        m_objectProxy = TestCompositorWorkerObjectProxy::create(&m_page->document());
        m_securityOrigin = SecurityOrigin::create(KURL(ParsedURLString, "http://fake.url/"));
    }

    void TearDown() override
    {
        ASSERT(!managerHasThread());
        ASSERT(!managerHasIsolate());
        m_page.clear();
    }

    PassRefPtr<TestCompositorWorkerThread> createCompositorWorker(WebWaitableEvent* startEvent)
    {
        TestCompositorWorkerThread* workerThread = new TestCompositorWorkerThread(nullptr, *m_objectProxy, 0, startEvent);
        OwnPtrWillBeRawPtr<WorkerClients> clients = nullptr;
        workerThread->start(WorkerThreadStartupData::create(
            KURL(ParsedURLString, "http://fake.url/"),
            "fake user agent",
            "//fake source code",
            nullptr,
            DontPauseWorkerGlobalScopeOnStart,
            adoptPtr(new Vector<CSPHeaderAndType>()),
            m_securityOrigin.get(),
            clients.release(),
            V8CacheOptionsDefault));
        return adoptRef(workerThread);
    }

    void createWorkerAdapter(RefPtr<CompositorWorkerThread>* workerThread, WebWaitableEvent* creationEvent)
    {
        *workerThread = createCompositorWorker(creationEvent);
    }

    // Attempts to run some simple script for |worker|.
    void checkWorkerCanExecuteScript(WorkerThread* worker)
    {
        OwnPtr<WebWaitableEvent> waitEvent = adoptPtr(Platform::current()->createWaitableEvent());
        worker->backingThread().platformThread().postTask(FROM_HERE, threadSafeBind(&CompositorWorkerManagerTest::executeScriptInWorker, AllowCrossThreadAccess(this),
            AllowCrossThreadAccess(worker), AllowCrossThreadAccess(waitEvent.get())));
        waitEvent->wait();
    }

    void waitForWaitableEventAfterIteratingCurrentLoop(WebWaitableEvent* waitEvent)
    {
        testing::runPendingTasks();
        waitEvent->wait();
    }

    bool managerHasThread() const
    {
        return CompositorWorkerManager::instance()->m_thread;
    }

    bool managerHasIsolate() const
    {
        return CompositorWorkerManager::instance()->m_isolate;
    }

private:
    void executeScriptInWorker(WorkerThread* worker, WebWaitableEvent* waitEvent)
    {
        WorkerScriptController* scriptController = worker->workerGlobalScope()->script();
        bool evaluateResult = scriptController->evaluate(ScriptSourceCode("var counter = 0; ++counter;"));
        ASSERT_UNUSED(evaluateResult, evaluateResult);
        waitEvent->signal();
    }

    OwnPtr<DummyPageHolder> m_page;
    RefPtr<SecurityOrigin> m_securityOrigin;
    OwnPtr<WorkerObjectProxy> m_objectProxy;
};

TEST_F(CompositorWorkerManagerTest, Basic)
{
    OwnPtr<WebWaitableEvent> creationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<CompositorWorkerThread> compositorWorker = createCompositorWorker(creationEvent.get());
    waitForWaitableEventAfterIteratingCurrentLoop(creationEvent.get());
    checkWorkerCanExecuteScript(compositorWorker.get());
    compositorWorker->terminateAndWait();
}

// Tests that the same WebThread is used for new workers if the WebThread is still alive.
TEST_F(CompositorWorkerManagerTest, CreateSecondAndTerminateFirst)
{
    // Create the first worker and wait until it is initialized.
    OwnPtr<WebWaitableEvent> firstCreationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<CompositorWorkerThread> firstWorker = createCompositorWorker(firstCreationEvent.get());
    WebThreadSupportingGC* firstThread = &CompositorWorkerManager::instance()->compositorWorkerThread();
    ASSERT(firstThread);
    waitForWaitableEventAfterIteratingCurrentLoop(firstCreationEvent.get());
    v8::Isolate* firstIsolate = firstWorker->isolate();
    ASSERT(firstIsolate);

    // Create the second worker and immediately destroy the first worker.
    OwnPtr<WebWaitableEvent> secondCreationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<CompositorWorkerThread> secondWorker = createCompositorWorker(secondCreationEvent.get());
    firstWorker->terminateAndWait();

    // Wait until the second worker is initialized. Verify that the second worker is using the same
    // thread and Isolate as the first worker.
    WebThreadSupportingGC* secondThread = &CompositorWorkerManager::instance()->compositorWorkerThread();
    ASSERT(secondThread);
    waitForWaitableEventAfterIteratingCurrentLoop(secondCreationEvent.get());
    EXPECT_EQ(firstThread, secondThread);

    v8::Isolate* secondIsolate = secondWorker->isolate();
    ASSERT(secondIsolate);
    EXPECT_EQ(firstIsolate, secondIsolate);

    // Verify that the worker can still successfully execute script.
    checkWorkerCanExecuteScript(secondWorker.get());

    secondWorker->terminateAndWait();
}

static void checkCurrentIsolate(v8::Isolate* isolate, WebWaitableEvent* event)
{
    EXPECT_EQ(v8::Isolate::GetCurrent(), isolate);
    event->signal();
}

// Tests that a new WebThread is created if all existing workers are terminated before a new worker is created.
TEST_F(CompositorWorkerManagerTest, TerminateFirstAndCreateSecond)
{
    // Create the first worker, wait until it is initialized, and terminate it.
    OwnPtr<WebWaitableEvent> creationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<CompositorWorkerThread> compositorWorker = createCompositorWorker(creationEvent.get());
    WebThreadSupportingGC* firstThread = &CompositorWorkerManager::instance()->compositorWorkerThread();
    waitForWaitableEventAfterIteratingCurrentLoop(creationEvent.get());
    ASSERT(compositorWorker->isolate());
    compositorWorker->terminateAndWait();

    // Create the second worker. Verify that the second worker lives in a different WebThread since the first
    // thread will have been destroyed after destroying the first worker.
    creationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    compositorWorker = createCompositorWorker(creationEvent.get());
    WebThreadSupportingGC* secondThread = &CompositorWorkerManager::instance()->compositorWorkerThread();
    EXPECT_NE(firstThread, secondThread);
    waitForWaitableEventAfterIteratingCurrentLoop(creationEvent.get());

    // Jump over to the worker's thread to verify that the Isolate is set up correctly and execute script.
    OwnPtr<WebWaitableEvent> checkEvent = adoptPtr(Platform::current()->createWaitableEvent());
    secondThread->platformThread().postTask(FROM_HERE, threadSafeBind(&checkCurrentIsolate, AllowCrossThreadAccess(compositorWorker->isolate()), AllowCrossThreadAccess(checkEvent.get())));
    waitForWaitableEventAfterIteratingCurrentLoop(checkEvent.get());
    checkWorkerCanExecuteScript(compositorWorker.get());

    compositorWorker->terminateAndWait();
}

// Tests that v8::Isolate and WebThread are correctly set-up if a worker is created while another is terminating.
TEST_F(CompositorWorkerManagerTest, CreatingSecondDuringTerminationOfFirst)
{
    OwnPtr<WebWaitableEvent> firstCreationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<TestCompositorWorkerThread> firstWorker = createCompositorWorker(firstCreationEvent.get());
    waitForWaitableEventAfterIteratingCurrentLoop(firstCreationEvent.get());
    v8::Isolate* firstIsolate = firstWorker->isolate();
    ASSERT(firstIsolate);

    // Request termination of the first worker, and set-up to make sure the second worker is created right as
    // the first worker terminates its isolate.
    OwnPtr<WebWaitableEvent> secondCreationEvent = adoptPtr(Platform::current()->createWaitableEvent());
    RefPtr<CompositorWorkerThread> secondWorker;
    firstWorker->setCallbackAfterV8Termination(bind(&CompositorWorkerManagerTest::createWorkerAdapter, this, &secondWorker, secondCreationEvent.get()));
    firstWorker->terminateAndWait();
    ASSERT(secondWorker);

    waitForWaitableEventAfterIteratingCurrentLoop(secondCreationEvent.get());
    v8::Isolate* secondIsolate = secondWorker->isolate();
    ASSERT(secondIsolate);
    EXPECT_EQ(firstIsolate, secondIsolate);

    // Verify that the isolate can run some scripts correctly in the second worker.
    checkWorkerCanExecuteScript(secondWorker.get());
    secondWorker->terminateAndWait();
}

} // namespace blink
