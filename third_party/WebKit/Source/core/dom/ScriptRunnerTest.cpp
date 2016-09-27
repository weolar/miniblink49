// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/ScriptRunner.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ScriptLoader.h"
#include "public/platform/Platform.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Invoke;
using ::testing::ElementsAre;
using ::testing::Return;

namespace blink {

class MockScriptLoader final : public ScriptLoader {
public:
    static PassOwnPtrWillBeRawPtr<MockScriptLoader> create(Element* element)
    {
        return adoptPtrWillBeNoop(new MockScriptLoader(element));
    }
    ~MockScriptLoader() override { }

    MOCK_METHOD0(execute, void());
    MOCK_CONST_METHOD0(isReady, bool());
private:
    explicit MockScriptLoader(Element* element) : ScriptLoader(element, false, false)
    {
    }
};

class MockWebThread : public WebThread {
public:
    explicit MockWebThread(WebScheduler* webScheduler) : m_webScheduler(webScheduler) { }
    ~MockWebThread() override { }

    void postTask(const WebTraceLocation&, Task*) override { ASSERT_NOT_REACHED(); }
    void postDelayedTask(const WebTraceLocation&, Task*, long long) override { ASSERT_NOT_REACHED(); }

    bool isCurrentThread() const override
    {
        ASSERT_NOT_REACHED();
        return false;
    }

    PlatformThreadId threadId() const override
    {
        ASSERT_NOT_REACHED();
        return 0;
    }

    void addTaskObserver(TaskObserver*) override { ASSERT_NOT_REACHED(); }
    void removeTaskObserver(TaskObserver*) override { ASSERT_NOT_REACHED(); }

    WebScheduler* scheduler() const override { return m_webScheduler; }

private:
    WebScheduler* m_webScheduler;
};

class MockPlatform : public Platform, private WebScheduler {
public:
    MockPlatform() : m_mockWebThread(this), m_shouldYield(false), m_shouldYieldEveryOtherTime(false) { }

    void postLoadingTask(const WebTraceLocation&, WebThread::Task* task) override
    {
        m_tasks.append(adoptPtr(task));
    }

    void cryptographicallyRandomValues(unsigned char* buffer, size_t length) override { }

    WebThread* currentThread() override { return &m_mockWebThread; }

    void runSingleTask()
    {
        if (m_tasks.isEmpty())
            return;
        m_tasks.takeFirst()->run();
    }

    void runAllTasks()
    {
        while (!m_tasks.isEmpty())
            m_tasks.takeFirst()->run();
    }

    bool shouldYieldForHighPriorityWork() override
    {
        if (m_shouldYieldEveryOtherTime)
            m_shouldYield = !m_shouldYield;
        return m_shouldYield;
    }

    void setShouldYield(bool shouldYield)
    {
        m_shouldYield = shouldYield;
    }

    // NOTE if we yield 100% of the time, nothing will get run.
    void setShouldYieldEveryOtherTime(bool shouldYieldEveryOtherTime)
    {
        m_shouldYieldEveryOtherTime = shouldYieldEveryOtherTime;
    }

private:
    MockWebThread m_mockWebThread;
    Deque<OwnPtr<WebThread::Task>> m_tasks;
    bool m_shouldYield;
    bool m_shouldYieldEveryOtherTime;
};

class ScriptRunnerTest : public testing::Test {
public:
    void SetUp() override
    {
        m_document = Document::create();
        m_element = m_document->createElement("foo", ASSERT_NO_EXCEPTION);

        m_scriptRunner = ScriptRunner::create(m_document.get());
        m_oldPlatform = Platform::current();

        // Force Platform::initialize to create a new one pointing at MockPlatform.
        Platform::initialize(&m_platform);
        m_platform.setShouldYield(false);
        m_platform.setShouldYieldEveryOtherTime(false);
    }

    void TearDown() override
    {
        m_scriptRunner.release();
        Platform::initialize(m_oldPlatform);
    }

    RefPtrWillBePersistent<Document> m_document;
    RefPtrWillBePersistent<Element> m_element;
    OwnPtrWillBePersistent<ScriptRunner> m_scriptRunner;
    std::vector<int> m_order; // gmock matchers don't work nicely with WTF::Vector
    MockPlatform m_platform;
    Platform* m_oldPlatform; // NOT OWNED
};

TEST_F(ScriptRunnerTest, QueueSingleScript_Async)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader = MockScriptLoader::create(m_element.get());
    m_scriptRunner->queueScriptForExecution(scriptLoader.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader.get(), ScriptRunner::ASYNC_EXECUTION);

    EXPECT_CALL(*scriptLoader, execute());
    m_platform.runAllTasks();
}

TEST_F(ScriptRunnerTest, QueueSingleScript_InOrder)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader = MockScriptLoader::create(m_element.get());
    m_scriptRunner->queueScriptForExecution(scriptLoader.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->resume();

    EXPECT_CALL(*scriptLoader, isReady()).WillOnce(Return(true));
    EXPECT_CALL(*scriptLoader, execute());
    m_platform.runAllTasks();
}

TEST_F(ScriptRunnerTest, QueueMultipleScripts_InOrder)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::IN_ORDER_EXECUTION);

    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([this] {
        m_order.push_back(1);
    }));
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([this] {
        m_order.push_back(2);
    }));
    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));

    // Make the scripts become ready in reverse order.
    bool isReady[] = { false, false, false };
    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Invoke([&isReady] {
        return isReady[0];
    }));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Invoke([&isReady] {
        return isReady[1];
    }));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Invoke([&isReady] {
        return isReady[2];
    }));

    for (int i = 2; i >= 0; i--) {
        isReady[i] = true;
        m_scriptRunner->resume();
        m_platform.runAllTasks();
    }

    // But ensure the scripts were run in the expected order.
    EXPECT_THAT(m_order, ElementsAre(1, 2, 3));
}

TEST_F(ScriptRunnerTest, QueueMixedScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader4 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader5 = MockScriptLoader::create(m_element.get());

    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader4.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader5.get(), ScriptRunner::ASYNC_EXECUTION);

    m_scriptRunner->notifyScriptReady(scriptLoader4.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader5.get(), ScriptRunner::ASYNC_EXECUTION);

    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([this] {
        m_order.push_back(1);
    }));
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([this] {
        m_order.push_back(2);
    }));
    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));
    EXPECT_CALL(*scriptLoader4, execute()).WillOnce(Invoke([this] {
        m_order.push_back(4);
    }));
    EXPECT_CALL(*scriptLoader5, execute()).WillOnce(Invoke([this] {
        m_order.push_back(5);
    }));

    m_platform.runAllTasks();

    // Make sure the async scripts were run before the in-order ones.
    EXPECT_THAT(m_order, ElementsAre(4, 5, 1, 2, 3));
}

TEST_F(ScriptRunnerTest, QueueMixedScripts_YieldAfterEveryExecution)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader4 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader5 = MockScriptLoader::create(m_element.get());

    m_platform.setShouldYieldEveryOtherTime(true);

    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader4.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader5.get(), ScriptRunner::ASYNC_EXECUTION);

    m_scriptRunner->notifyScriptReady(scriptLoader4.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader5.get(), ScriptRunner::ASYNC_EXECUTION);

    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([this] {
        m_order.push_back(1);
    }));
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([this] {
        m_order.push_back(2);
    }));
    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));
    EXPECT_CALL(*scriptLoader4, execute()).WillOnce(Invoke([this] {
        m_order.push_back(4);
    }));
    EXPECT_CALL(*scriptLoader5, execute()).WillOnce(Invoke([this] {
        m_order.push_back(5);
    }));

    m_platform.runAllTasks();

    // Make sure the async scripts were run before the in-order ones.
    EXPECT_THAT(m_order, ElementsAre(4, 5, 1, 2, 3));
}

TEST_F(ScriptRunnerTest, QueueReentrantScript_Async)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);

    MockScriptLoader* scriptLoader = scriptLoader2.get();
    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([scriptLoader, this] {
        m_order.push_back(1);
        m_scriptRunner->notifyScriptReady(scriptLoader, ScriptRunner::ASYNC_EXECUTION);
    }));

    scriptLoader = scriptLoader3.get();
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([scriptLoader, this] {
        m_order.push_back(2);
        m_scriptRunner->notifyScriptReady(scriptLoader, ScriptRunner::ASYNC_EXECUTION);
    }));

    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));

    // Make sure that re-entrant calls to notifyScriptReady don't cause ScriptRunner::execute to do
    // more work than expected.
    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2, 3));
}

TEST_F(ScriptRunnerTest, QueueReentrantScript_InOrder)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->resume();

    MockScriptLoader* scriptLoader = scriptLoader2.get();
    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([scriptLoader, this] {
        m_order.push_back(1);
        m_scriptRunner->queueScriptForExecution(scriptLoader, ScriptRunner::IN_ORDER_EXECUTION);
        m_scriptRunner->resume();
    }));

    scriptLoader = scriptLoader3.get();
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([scriptLoader, this] {
        m_order.push_back(2);
        m_scriptRunner->queueScriptForExecution(scriptLoader, ScriptRunner::IN_ORDER_EXECUTION);
        m_scriptRunner->resume();
    }));

    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));

    // Make sure that re-entrant calls to queueScriptForExecution don't cause ScriptRunner::execute to do
    // more work than expected.
    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2, 3));
}

TEST_F(ScriptRunnerTest, ShouldYield_AsyncScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader2.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader3.get(), ScriptRunner::ASYNC_EXECUTION);

    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([this] {
        m_order.push_back(1);
        m_platform.setShouldYield(true);
    }));
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([this] {
        m_order.push_back(2);
    }));
    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1));

    // Make sure the interrupted tasks are executed next 'tick'.
    m_platform.setShouldYield(false);
    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2, 3));
}

TEST_F(ScriptRunnerTest, QueueReentrantScript_ManyAsyncScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoaders[20];
    for (int i = 0; i < 20; i++)
        scriptLoaders[i] = nullptr;

    for (int i = 0; i < 20; i++) {
        scriptLoaders[i] = MockScriptLoader::create(m_element.get());
        EXPECT_CALL(*scriptLoaders[i], isReady()).WillRepeatedly(Return(true));

        m_scriptRunner->queueScriptForExecution(scriptLoaders[i].get(), ScriptRunner::ASYNC_EXECUTION);

        if (i > 0) {
            EXPECT_CALL(*scriptLoaders[i], execute()).WillOnce(Invoke([this, i] {
                m_order.push_back(i);
            }));
        }
    }

    m_scriptRunner->notifyScriptReady(scriptLoaders[0].get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoaders[1].get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->resume();

    EXPECT_CALL(*scriptLoaders[0], execute()).WillOnce(Invoke([&scriptLoaders, this] {
        for (int i = 2; i < 20; i++)
            m_scriptRunner->notifyScriptReady(scriptLoaders[i].get(), ScriptRunner::ASYNC_EXECUTION);
        m_scriptRunner->resume();
        m_order.push_back(0);
    }));

    m_platform.runAllTasks();

    int expected[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
    };

    EXPECT_THAT(m_order, testing::ElementsAreArray(expected));
}

TEST_F(ScriptRunnerTest, ShouldYield_InOrderScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->resume();

    EXPECT_CALL(*scriptLoader1, execute()).WillOnce(Invoke([this] {
        m_order.push_back(1);
        m_platform.setShouldYield(true);
    }));
    EXPECT_CALL(*scriptLoader2, execute()).WillOnce(Invoke([this] {
        m_order.push_back(2);
    }));
    EXPECT_CALL(*scriptLoader3, execute()).WillOnce(Invoke([this] {
        m_order.push_back(3);
    }));

    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1));

    // Make sure the interrupted tasks are executed next 'tick'.
    m_platform.setShouldYield(false);
    m_platform.runSingleTask();
    EXPECT_THAT(m_order, ElementsAre(1, 2, 3));
}

TEST_F(ScriptRunnerTest, ShouldYield_RunsAtLastOneTask_AsyncScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader1.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader2.get(), ScriptRunner::ASYNC_EXECUTION);
    m_scriptRunner->notifyScriptReady(scriptLoader3.get(), ScriptRunner::ASYNC_EXECUTION);

    m_platform.setShouldYield(true);
    EXPECT_CALL(*scriptLoader1, execute()).Times(1);
    EXPECT_CALL(*scriptLoader2, execute()).Times(0);
    EXPECT_CALL(*scriptLoader3, execute()).Times(0);

    m_platform.runSingleTask();

    // We can't safely distruct ScriptRunner with unexecuted MockScriptLoaders (real ScriptLoader is fine) so drain them.
    testing::Mock::VerifyAndClear(scriptLoader2.get());
    testing::Mock::VerifyAndClear(scriptLoader3.get());
    EXPECT_CALL(*scriptLoader2, execute()).Times(1);
    EXPECT_CALL(*scriptLoader3, execute()).Times(1);

    m_platform.runAllTasks();
}

TEST_F(ScriptRunnerTest, ShouldYield_RunsAtLastOneTask_InOrderScripts)
{
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader1 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader2 = MockScriptLoader::create(m_element.get());
    OwnPtrWillBeRawPtr<MockScriptLoader> scriptLoader3 = MockScriptLoader::create(m_element.get());

    EXPECT_CALL(*scriptLoader1, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));

    m_scriptRunner->queueScriptForExecution(scriptLoader1.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader2.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->queueScriptForExecution(scriptLoader3.get(), ScriptRunner::IN_ORDER_EXECUTION);
    m_scriptRunner->resume();

    m_platform.setShouldYield(true);
    EXPECT_CALL(*scriptLoader1, execute()).Times(1);
    EXPECT_CALL(*scriptLoader2, execute()).Times(0);
    EXPECT_CALL(*scriptLoader3, execute()).Times(0);

    m_platform.runSingleTask();

    // We can't safely distruct ScriptRunner with unexecuted MockScriptLoaders (real ScriptLoader is fine) so drain them.
    testing::Mock::VerifyAndClear(scriptLoader2.get());
    testing::Mock::VerifyAndClear(scriptLoader3.get());
    EXPECT_CALL(*scriptLoader2, execute()).Times(1);
    EXPECT_CALL(*scriptLoader3, execute()).Times(1);
    EXPECT_CALL(*scriptLoader2, isReady()).WillRepeatedly(Return(true));
    EXPECT_CALL(*scriptLoader3, isReady()).WillRepeatedly(Return(true));
    m_platform.runAllTasks();
}

} // namespace blink
