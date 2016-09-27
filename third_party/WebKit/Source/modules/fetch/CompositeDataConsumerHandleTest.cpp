// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/CompositeDataConsumerHandle.h"

#include "modules/fetch/DataConsumerHandleTestUtil.h"
#include "platform/Task.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/heap/Handle.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebTraceLocation.h"
#include "public/platform/WebWaitableEvent.h"
#include "wtf/Locker.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {

namespace {

using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;
using Checkpoint = StrictMock<::testing::MockFunction<void(int)>>;

const WebDataConsumerHandle::Result kShouldWait = WebDataConsumerHandle::ShouldWait;
const WebDataConsumerHandle::Result kOk = WebDataConsumerHandle::Ok;
const WebDataConsumerHandle::Flags kNone = WebDataConsumerHandle::FlagNone;

class MockReader : public WebDataConsumerHandle::Reader {
public:
    static PassOwnPtr<StrictMock<MockReader>> create() { return adoptPtr(new StrictMock<MockReader>); }

    using Result = WebDataConsumerHandle::Result;
    using Flags = WebDataConsumerHandle::Flags;
    MOCK_METHOD4(read, Result(void*, size_t, Flags, size_t*));
    MOCK_METHOD3(beginRead, Result(const void**, Flags, size_t*));
    MOCK_METHOD1(endRead, Result(size_t));
};

class MockHandle : public WebDataConsumerHandle {
public:
    static PassOwnPtr<StrictMock<MockHandle>> create() { return adoptPtr(new StrictMock<MockHandle>); }

    MOCK_METHOD1(obtainReaderInternal, Reader*(Client*));
};

class ThreadingRegistrationTest : public DataConsumerHandleTestUtil::ThreadingTestBase {
public:
    using Self = ThreadingRegistrationTest;
    void run()
    {
        m_context = Context::create();
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());

        postTaskAndWait(updatingThread(), FROM_HERE, new Task(threadSafeBind(&Self::createHandle, this)));
        postTaskAndWait(readingThread(), FROM_HERE, new Task(threadSafeBind(&Self::obtainReader, this)));
    }

private:
    void createHandle()
    {
        m_handle = CompositeDataConsumerHandle::create(adoptPtr(new DataConsumerHandle("handle1", m_context)));
        m_waitableEvent->signal();
    }
    void obtainReader()
    {
        m_reader = m_handle->obtainReader(&m_client);
        updatingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::update, this)));
    }
    void update()
    {
        m_handle->update(adoptPtr(new DataConsumerHandle("handle2", m_context)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::signalDone, this)));
    }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
};

class ThreadingRegistrationDeleteHandleTest : public DataConsumerHandleTestUtil::ThreadingTestBase {
public:
    using Self = ThreadingRegistrationDeleteHandleTest;
    void run()
    {
        m_context = Context::create();
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());

        postTaskAndWait(updatingThread(), FROM_HERE, new Task(threadSafeBind(&Self::createHandle, this)));
        postTaskAndWait(readingThread(), FROM_HERE, new Task(threadSafeBind(&Self::obtainReader, this)));
    }

private:
    void createHandle()
    {
        m_handle = CompositeDataConsumerHandle::create(adoptPtr(new DataConsumerHandle("handle1", m_context)));
        m_waitableEvent->signal();
    }

    void obtainReader()
    {
        m_reader = m_handle->obtainReader(&m_client);
        updatingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::update, this)));
    }
    void update()
    {
        m_handle->update(adoptPtr(new DataConsumerHandle("handle2", m_context)));
        m_handle = nullptr;
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::signalDone, this)));
    }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
};

class ThreadingRegistrationDeleteReaderTest : public DataConsumerHandleTestUtil::ThreadingTestBase {
public:
    using Self = ThreadingRegistrationDeleteReaderTest;
    void run()
    {
        m_context = Context::create();
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());

        postTaskAndWait(updatingThread(), FROM_HERE, new Task(threadSafeBind(&Self::createHandle, this)));
        postTaskAndWait(readingThread(), FROM_HERE, new Task(threadSafeBind(&Self::obtainReader, this)));
    }

private:
    void createHandle()
    {
        m_handle = CompositeDataConsumerHandle::create(adoptPtr(new DataConsumerHandle("handle1", m_context)));
        m_waitableEvent->signal();
    }

    void obtainReader()
    {
        m_reader = m_handle->obtainReader(&m_client);
        updatingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::update, this)));
    }
    void update()
    {
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        m_handle->update(adoptPtr(new DataConsumerHandle("handle2", m_context)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::signalDone, this)));
    }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
};

class ThreadingUpdatingReaderWhileUpdatingTest : public DataConsumerHandleTestUtil::ThreadingTestBase {
public:
    using Self = ThreadingUpdatingReaderWhileUpdatingTest;
    void run()
    {
        m_context = Context::create();
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());
        m_updateEvent = adoptPtr(Platform::current()->createWaitableEvent());

        postTaskAndWait(updatingThread(), FROM_HERE, new Task(threadSafeBind(&Self::createHandle, this)));
        postTaskAndWait(readingThread(), FROM_HERE, new Task(threadSafeBind(&Self::obtainReader, this)));
    }

private:
    void createHandle()
    {
        m_handle = CompositeDataConsumerHandle::create(adoptPtr(new DataConsumerHandle("handle1", m_context)));
        m_waitableEvent->signal();
    }

    void obtainReader()
    {
        m_reader = m_handle->obtainReader(&m_client);
        updatingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::update, this)));
        m_updateEvent->wait();
    }

    void update()
    {
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::reobtainReader, this)));
        m_handle->update(adoptPtr(new DataConsumerHandle("handle2", m_context)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::signalDone, this)));
        m_updateEvent->signal();
    }

    void reobtainReader()
    {
        m_reader = nullptr;
        m_reader = m_handle->obtainReader(&m_client);
    }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
    OwnPtr<WebWaitableEvent> m_updateEvent;
};

class ThreadingRegistrationUpdateTwiceAtOneTimeTest : public DataConsumerHandleTestUtil::ThreadingTestBase {
public:
    using Self = ThreadingRegistrationUpdateTwiceAtOneTimeTest;
    void run()
    {
        m_context = Context::create();
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());

        postTaskAndWait(updatingThread(), FROM_HERE, new Task(threadSafeBind(&Self::createHandle, this)));
        postTaskAndWait(readingThread(), FROM_HERE, new Task(threadSafeBind(&Self::obtainReader, this)));
    }

private:
    void createHandle()
    {
        m_handle = CompositeDataConsumerHandle::create(adoptPtr(new DataConsumerHandle("handle1", m_context)));
        m_waitableEvent->signal();
    }

    void obtainReader()
    {
        m_reader = m_handle->obtainReader(&m_client);
        updatingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::update, this)));
    }
    void update()
    {
        m_handle->update(adoptPtr(new DataConsumerHandle("handle2", m_context)));
        m_handle->update(adoptPtr(new DataConsumerHandle("handle3", m_context)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::resetReader, this)));
        readingThread()->postTask(FROM_HERE, new Task(threadSafeBind(&Self::signalDone, this)));
    }

    OwnPtr<CompositeDataConsumerHandle> m_handle;
};

TEST(CompositeDataConsumerHandleTest, Read)
{
    char buffer[20];
    size_t size = 0;
    DataConsumerHandleTestUtil::NoopClient client;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle1 = MockHandle::create();
    OwnPtr<MockHandle> handle2 = MockHandle::create();
    OwnPtr<MockReader> reader1 = MockReader::create();
    OwnPtr<MockReader> reader2 = MockReader::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(0));
    EXPECT_CALL(*handle1, obtainReaderInternal(&client)).WillOnce(Return(reader1.get()));
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*reader1, read(buffer, sizeof(buffer), kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*handle2, obtainReaderInternal(&client)).WillOnce(Return(reader2.get()));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(*reader2, read(buffer, sizeof(buffer), kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(4));

    // They are adopted by |obtainReader|.
    ASSERT_TRUE(reader1.leakPtr());
    ASSERT_TRUE(reader2.leakPtr());

    OwnPtr<CompositeDataConsumerHandle> handle = CompositeDataConsumerHandle::create(handle1.release());
    checkpoint.Call(0);
    OwnPtr<CompositeDataConsumerHandle::Reader> reader = handle->obtainReader(&client);
    checkpoint.Call(1);
    EXPECT_EQ(kOk, reader->read(buffer, sizeof(buffer), kNone, &size));
    checkpoint.Call(2);
    handle->update(handle2.release());
    checkpoint.Call(3);
    EXPECT_EQ(kOk, reader->read(buffer, sizeof(buffer), kNone, &size));
    checkpoint.Call(4);
}

TEST(CompositeDataConsumerHandleTest, TwoPhaseRead)
{
    const void* p = nullptr;
    size_t size = 0;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle1 = MockHandle::create();
    OwnPtr<MockHandle> handle2 = MockHandle::create();
    OwnPtr<MockReader> reader1 = MockReader::create();
    OwnPtr<MockReader> reader2 = MockReader::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(0));
    EXPECT_CALL(*handle1, obtainReaderInternal(nullptr)).WillOnce(Return(reader1.get()));
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*reader1, beginRead(&p, kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader1, endRead(0)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(*handle2, obtainReaderInternal(nullptr)).WillOnce(Return(reader2.get()));
    EXPECT_CALL(checkpoint, Call(4));
    EXPECT_CALL(*reader2, beginRead(&p, kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(5));
    EXPECT_CALL(*reader2, endRead(0)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(6));

    // They are adopted by |obtainReader|.
    ASSERT_TRUE(reader1.leakPtr());
    ASSERT_TRUE(reader2.leakPtr());

    OwnPtr<CompositeDataConsumerHandle> handle = CompositeDataConsumerHandle::create(handle1.release());
    checkpoint.Call(0);
    OwnPtr<CompositeDataConsumerHandle::Reader> reader = handle->obtainReader(nullptr);
    checkpoint.Call(1);
    EXPECT_EQ(kOk, reader->beginRead(&p, kNone, &size));
    checkpoint.Call(2);
    EXPECT_EQ(kOk, reader->endRead(0));
    checkpoint.Call(3);
    handle->update(handle2.release());
    checkpoint.Call(4);
    EXPECT_EQ(kOk, reader->beginRead(&p, kNone, &size));
    checkpoint.Call(5);
    EXPECT_EQ(kOk, reader->endRead(0));
    checkpoint.Call(6);
}

TEST(CompositeDataConsumerHandleTest, HangingTwoPhaseRead)
{
    const void* p = nullptr;
    size_t size = 0;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle1 = MockHandle::create();
    OwnPtr<MockHandle> handle2 = MockHandle::create();
    OwnPtr<MockHandle> handle3 = MockHandle::create();
    OwnPtr<MockReader> reader1 = MockReader::create();
    OwnPtr<MockReader> reader2 = MockReader::create();
    OwnPtr<MockReader> reader3 = MockReader::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(0));
    EXPECT_CALL(*handle1, obtainReaderInternal(nullptr)).WillOnce(Return(reader1.get()));
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*reader1, beginRead(&p, kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(*reader1, endRead(0)).WillOnce(Return(kOk));
    EXPECT_CALL(*handle2, obtainReaderInternal(nullptr)).WillOnce(Return(reader2.get()));
    EXPECT_CALL(checkpoint, Call(4));
    EXPECT_CALL(*reader2, beginRead(&p, kNone, &size)).WillOnce(Return(kShouldWait));
    EXPECT_CALL(checkpoint, Call(5));
    EXPECT_CALL(*handle3, obtainReaderInternal(nullptr)).WillOnce(Return(reader3.get()));
    EXPECT_CALL(checkpoint, Call(6));
    EXPECT_CALL(*reader3, beginRead(&p, kNone, &size)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(7));
    EXPECT_CALL(*reader3, endRead(0)).WillOnce(Return(kOk));
    EXPECT_CALL(checkpoint, Call(8));

    // They are adopted by |obtainReader|.
    ASSERT_TRUE(reader1.leakPtr());
    ASSERT_TRUE(reader2.leakPtr());
    ASSERT_TRUE(reader3.leakPtr());

    OwnPtr<CompositeDataConsumerHandle> handle = CompositeDataConsumerHandle::create(handle1.release());
    checkpoint.Call(0);
    OwnPtr<CompositeDataConsumerHandle::Reader> reader = handle->obtainReader(nullptr);
    checkpoint.Call(1);
    EXPECT_EQ(kOk, reader->beginRead(&p, kNone, &size));
    checkpoint.Call(2);
    handle->update(handle2.release());
    checkpoint.Call(3);
    EXPECT_EQ(kOk, reader->endRead(0));
    checkpoint.Call(4);
    EXPECT_EQ(kShouldWait, reader->beginRead(&p, kNone, &size));
    checkpoint.Call(5);
    handle->update(handle3.release());
    checkpoint.Call(6);
    EXPECT_EQ(kOk, reader->beginRead(&p, kNone, &size));
    checkpoint.Call(7);
    EXPECT_EQ(kOk, reader->endRead(0));
    checkpoint.Call(8);
}

TEST(CompositeDataConsumerHandleTest, RegisterClientOnDifferentThreads)
{
    ThreadingRegistrationTest test;
    test.run();

    EXPECT_EQ(
        "A reader is attached to handle1 on the reading thread.\n"
        "A reader is detached from handle1 on the reading thread.\n"
        "A reader is attached to handle2 on the reading thread.\n"
        "A reader is detached from handle2 on the reading thread.\n",
        test.result());
}

TEST(CompositeDataConsumerHandleTest, DeleteHandleWhileUpdating)
{
    ThreadingRegistrationDeleteHandleTest test;
    test.run();

    EXPECT_EQ(
        "A reader is attached to handle1 on the reading thread.\n"
        "A reader is detached from handle1 on the reading thread.\n"
        "A reader is attached to handle2 on the reading thread.\n"
        "A reader is detached from handle2 on the reading thread.\n",
        test.result());
}

TEST(CompositeDataConsumerHandleTest, DeleteReaderWhileUpdating)
{
    ThreadingRegistrationDeleteReaderTest test;
    test.run();

    EXPECT_EQ(
        "A reader is attached to handle1 on the reading thread.\n"
        "A reader is detached from handle1 on the reading thread.\n",
        test.result());
}

TEST(CompositeDataConsumerHandleTest, UpdateReaderWhileUpdating)
{
    ThreadingUpdatingReaderWhileUpdatingTest test;
    test.run();

    EXPECT_EQ(
        "A reader is attached to handle1 on the reading thread.\n"
        "A reader is detached from handle1 on the reading thread.\n"
        "A reader is attached to handle2 on the reading thread.\n"
        "A reader is detached from handle2 on the reading thread.\n",
        test.result());
}

// Disabled on Android due to flakiness (https://crbug.com/506261).
#if OS(ANDROID)
#define MAYBE_UpdateTwiceAtOnce DISABLED_UpdateTwiceAtOnce
#else
#define MAYBE_UpdateTwiceAtOnce UpdateTwiceAtOnce
#endif
TEST(CompositeDataConsumerHandleTest, MAYBE_UpdateTwiceAtOnce)
{
    ThreadingRegistrationUpdateTwiceAtOneTimeTest test;
    test.run();

    EXPECT_EQ(
        "A reader is attached to handle1 on the reading thread.\n"
        "A reader is detached from handle1 on the reading thread.\n"
        "A reader is attached to handle3 on the reading thread.\n"
        "A reader is detached from handle3 on the reading thread.\n",
        test.result());
}

TEST(CompositeDataConsumerHandleTest, DoneHandleNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNotificationTest test;
    // Test this function returns.
    test.run(CompositeDataConsumerHandle::create(createDoneDataConsumerHandle()));
}

TEST(CompositeDataConsumerHandleTest, DoneHandleNoNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNoNotificationTest test;
    // Test this function doesn't crash.
    test.run(CompositeDataConsumerHandle::create(createDoneDataConsumerHandle()));
}

} // namespace

} // namespace blink
