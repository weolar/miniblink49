// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/workers/WorkerThread.h"

#include "core/inspector/ConsoleMessage.h"
#include "core/workers/WorkerReportingProxy.h"
#include "core/workers/WorkerThreadStartupData.h"
#include "platform/NotImplemented.h"
#include "public/platform/WebScheduler.h"
#include "public/platform/WebWaitableEvent.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::AtMost;
using testing::Invoke;
using testing::Return;
using testing::Mock;

namespace blink {

namespace {
class MockWorkerLoaderProxyProvider : public WorkerLoaderProxyProvider {
public:
    MockWorkerLoaderProxyProvider() { }
    ~MockWorkerLoaderProxyProvider() override { }

    void postTaskToLoader(PassOwnPtr<ExecutionContextTask>) override
    {
        notImplemented();
    }

    bool postTaskToWorkerGlobalScope(PassOwnPtr<ExecutionContextTask>) override
    {
        notImplemented();
        return false;
    }
};

class MockWorkerReportingProxy : public WorkerReportingProxy {
public:
    MockWorkerReportingProxy() { }
    ~MockWorkerReportingProxy() override { }

    MOCK_METHOD5(reportException, void(const String& errorMessage, int lineNumber, int columnNumber, const String& sourceURL, int exceptionId));
    MOCK_METHOD1(reportConsoleMessage, void(PassRefPtrWillBeRawPtr<ConsoleMessage>));
    MOCK_METHOD1(postMessageToPageInspector, void(const String&));
    MOCK_METHOD0(postWorkerConsoleAgentEnabled, void());
    MOCK_METHOD1(didEvaluateWorkerScript, void(bool success));
    MOCK_METHOD1(workerGlobalScopeStarted, void(WorkerGlobalScope*));
    MOCK_METHOD0(workerGlobalScopeClosed, void());
    MOCK_METHOD0(workerThreadTerminated, void());
    MOCK_METHOD0(willDestroyWorkerGlobalScope, void());
};

class FakeWorkerGlobalScope : public WorkerGlobalScope {
public:
    typedef WorkerGlobalScope Base;

    FakeWorkerGlobalScope(const KURL& url, const String& userAgent, WorkerThread* thread, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients)
        : WorkerGlobalScope(url, userAgent, thread, monotonicallyIncreasingTime(), starterOrigin, workerClients)
    {
    }

    ~FakeWorkerGlobalScope() override
    {
    }

    // EventTarget
    const AtomicString& interfaceName() const override
    {
        return EventTargetNames::DedicatedWorkerGlobalScope;
    }

    void logExceptionToConsole(const String&, int, const String&, int, int, PassRefPtrWillBeRawPtr<ScriptCallStack>) override
    {
    }
};

class WorkerThreadForTest : public WorkerThread {
public:
    WorkerThreadForTest(
        WorkerLoaderProxyProvider* mockWorkerLoaderProxyProvider,
        WorkerReportingProxy& mockWorkerReportingProxy)
        : WorkerThread(WorkerLoaderProxy::create(mockWorkerLoaderProxyProvider), mockWorkerReportingProxy)
        , m_thread(WebThreadSupportingGC::create("Test thread"))
    {
    }

    ~WorkerThreadForTest() override { }

    // WorkerThread implementation:
    WebThreadSupportingGC& backingThread() override
    {
        return *m_thread;
    }

    MOCK_METHOD1(doIdleGc, bool(double deadlineSeconds));

    PassRefPtrWillBeRawPtr<WorkerGlobalScope> createWorkerGlobalScope(PassOwnPtr<WorkerThreadStartupData> startupData) override
    {
        return adoptRefWillBeNoop(new FakeWorkerGlobalScope(startupData->m_scriptURL, startupData->m_userAgent, this, startupData->m_starterOrigin, startupData->m_workerClients.release()));
    }

private:
    OwnPtr<WebThreadSupportingGC> m_thread;
};

class WakeupTask : public WebThread::Task {
public:
    WakeupTask() { }

    ~WakeupTask() override { }

    void run() override { }
};

class PostDelayedWakeupTask : public WebThread::Task {
public:
    PostDelayedWakeupTask(WebScheduler* scheduler, long long delay) : m_scheduler(scheduler), m_delay(delay) { }

    ~PostDelayedWakeupTask() override { }

    void run() override
    {
        m_scheduler->postTimerTask(FROM_HERE, new WakeupTask(), m_delay);
    }

    WebScheduler* m_scheduler; // Not owned.
    long long m_delay;
};

class SignalTask : public WebThread::Task {
public:
    SignalTask(WebWaitableEvent* completionEvent) : m_completionEvent(completionEvent) { }

    ~SignalTask() override { }

    void run() override
    {
        m_completionEvent->signal();
    }

private:
    WebWaitableEvent* m_completionEvent; // Not owned.
};

} // namespace

class WorkerThreadTest : public testing::Test {
public:
    void SetUp() override
    {
        m_mockWorkerLoaderProxyProvider = adoptPtr(new MockWorkerLoaderProxyProvider());
        m_mockWorkerReportingProxy = adoptPtr(new MockWorkerReportingProxy());
        m_securityOrigin = SecurityOrigin::create(KURL(ParsedURLString, "http://fake.url/"));
        m_workerThread = adoptRef(new WorkerThreadForTest(
            m_mockWorkerLoaderProxyProvider.get(),
            *m_mockWorkerReportingProxy));
    }

    void TearDown() override
    {
        m_workerThread->workerLoaderProxy()->detachProvider(m_mockWorkerLoaderProxyProvider.get());
    }

    void start()
    {
        OwnPtr<Vector<CSPHeaderAndType>> headers = adoptPtr(new Vector<CSPHeaderAndType>());
        CSPHeaderAndType headerAndType("contentSecurityPolicy", ContentSecurityPolicyHeaderTypeReport);
        headers->append(headerAndType);

        OwnPtrWillBeRawPtr<WorkerClients> clients = nullptr;

        m_workerThread->start(WorkerThreadStartupData::create(
            KURL(ParsedURLString, "http://fake.url/"),
            "fake user agent",
            "//fake source code",
            nullptr,
            DontPauseWorkerGlobalScopeOnStart,
            headers.release(),
            m_securityOrigin.get(),
            clients.release(),
            V8CacheOptionsDefault));
    }

    void waitForInit()
    {
        OwnPtr<WebWaitableEvent> completionEvent = adoptPtr(Platform::current()->createWaitableEvent());
        m_workerThread->backingThread().postTask(FROM_HERE, new SignalTask(completionEvent.get()));
        completionEvent->wait();
    }

    void postWakeUpTask(long long waitMs)
    {
        WebScheduler* scheduler = m_workerThread->backingThread().platformThread().scheduler();

        // The idle task will get posted on an after wake up queue, so we need another task
        // posted at the right time to wake the system up.  We don't know the right delay here
        // since the thread can take a variable length of time to be responsive, however this
        // isn't a problem when posting a delayed task from within a task on the worker thread.
        scheduler->postLoadingTask(FROM_HERE, new PostDelayedWakeupTask(scheduler, waitMs));
    }

protected:
    void expectWorkerLifetimeReportingCalls()
    {
        EXPECT_CALL(*m_mockWorkerReportingProxy, workerGlobalScopeStarted(_)).Times(1);
        EXPECT_CALL(*m_mockWorkerReportingProxy, didEvaluateWorkerScript(true)).Times(1);
        EXPECT_CALL(*m_mockWorkerReportingProxy, workerThreadTerminated()).Times(1);
        EXPECT_CALL(*m_mockWorkerReportingProxy, willDestroyWorkerGlobalScope()).Times(1);
    }

    RefPtr<SecurityOrigin> m_securityOrigin;
    OwnPtr<MockWorkerLoaderProxyProvider> m_mockWorkerLoaderProxyProvider;
    OwnPtr<MockWorkerReportingProxy> m_mockWorkerReportingProxy;
    RefPtr<WorkerThreadForTest> m_workerThread;
};

TEST_F(WorkerThreadTest, StartAndStop)
{
    expectWorkerLifetimeReportingCalls();
    start();
    waitForInit();
    m_workerThread->terminateAndWait();
}

TEST_F(WorkerThreadTest, StartAndStopImmediately)
{
    EXPECT_CALL(*m_mockWorkerReportingProxy, workerGlobalScopeStarted(_)).Times(AtMost(1));
    EXPECT_CALL(*m_mockWorkerReportingProxy, didEvaluateWorkerScript(true)).Times(AtMost(1));
    EXPECT_CALL(*m_mockWorkerReportingProxy, workerThreadTerminated()).Times(AtMost(1));
    EXPECT_CALL(*m_mockWorkerReportingProxy, willDestroyWorkerGlobalScope()).Times(AtMost(1));
    start();
    m_workerThread->terminateAndWait();
}

TEST_F(WorkerThreadTest, GcOccursWhileIdle)
{
    OwnPtr<WebWaitableEvent> gcDone = adoptPtr(Platform::current()->createWaitableEvent());

    ON_CALL(*m_workerThread, doIdleGc(_)).WillByDefault(Invoke(
        [&gcDone](double)
        {
            gcDone->signal();
            return false;
        }));

    EXPECT_CALL(*m_workerThread, doIdleGc(_)).Times(testing::AtLeast(1));

    expectWorkerLifetimeReportingCalls();
    start();
    waitForInit();
    postWakeUpTask(500ul);

    gcDone->wait();
    m_workerThread->terminateAndWait();
};

class RepeatingTask : public WebThread::Task {
public:
    RepeatingTask(WebScheduler* scheduler, WebWaitableEvent* completion)
        : RepeatingTask(scheduler, completion, 0) { }

    ~RepeatingTask() override { }

    void run() override
    {
        m_taskCount++;
        if (m_taskCount == 10)
            m_completion->signal();

        m_scheduler->postTimerTask(
            FROM_HERE, new RepeatingTask(m_scheduler, m_completion, m_taskCount), 0ul);
        m_scheduler->postLoadingTask(FROM_HERE, new WakeupTask());

    }

private:
    RepeatingTask(WebScheduler* scheduler, WebWaitableEvent* completion, int taskCount)
        : m_scheduler(scheduler)
        , m_completion(completion)
        , m_taskCount(taskCount)
        { }

    WebScheduler* m_scheduler; // Not owned.
    WebWaitableEvent* m_completion;
    int m_taskCount;
};

TEST_F(WorkerThreadTest, GcDoesNotOccurWhenNotIdle)
{
    OwnPtr<WebWaitableEvent> completion = adoptPtr(Platform::current()->createWaitableEvent());

    EXPECT_CALL(*m_workerThread, doIdleGc(_)).Times(0);

    expectWorkerLifetimeReportingCalls();
    start();
    waitForInit();

    WebScheduler* scheduler = m_workerThread->backingThread().platformThread().scheduler();

    // Post a repeating task that should prevent any GC from happening.
    scheduler->postLoadingTask(FROM_HERE, new RepeatingTask(scheduler, completion.get()));

    completion->wait();

    // Make sure doIdleGc has not been called by this stage.
    Mock::VerifyAndClearExpectations(m_workerThread.get());

    m_workerThread->terminateAndWait();
}

} // namespace blink
