// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/DataConsumerTee.h"

#include "core/testing/DummyPageHolder.h"
#include "core/testing/NullExecutionContext.h"
#include "modules/fetch/DataConsumerHandleTestUtil.h"
#include "platform/Task.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/WebThreadSupportingGC.h"
#include "public/platform/Platform.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebTraceLocation.h"
#include "public/platform/WebWaitableEvent.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

#include <gtest/gtest.h>
#include <string.h>
#include <v8.h>

namespace blink {
namespace {

using ::testing::InSequence;
using ::testing::Return;
using ::testing::StrictMock;
using ::testing::_;
using Checkpoint = StrictMock<::testing::MockFunction<void(int)>>;

using Result = WebDataConsumerHandle::Result;
using Thread = DataConsumerHandleTestUtil::Thread;
const Result kDone = WebDataConsumerHandle::Done;
const Result kUnexpectedError = WebDataConsumerHandle::UnexpectedError;
const FetchDataConsumerHandle::Reader::BlobSizePolicy kDisallowBlobWithInvalidSize = FetchDataConsumerHandle::Reader::DisallowBlobWithInvalidSize;
const FetchDataConsumerHandle::Reader::BlobSizePolicy kAllowBlobWithInvalidSize = FetchDataConsumerHandle::Reader::AllowBlobWithInvalidSize;

using Command = DataConsumerHandleTestUtil::Command;
using Handle = DataConsumerHandleTestUtil::ReplayingHandle;
using HandleReader = DataConsumerHandleTestUtil::HandleReader;
using HandleTwoPhaseReader = DataConsumerHandleTestUtil::HandleTwoPhaseReader;
using HandleReadResult = DataConsumerHandleTestUtil::HandleReadResult;
using MockFetchDataConsumerHandle = DataConsumerHandleTestUtil::MockFetchDataConsumerHandle;
using MockFetchDataConsumerReader = DataConsumerHandleTestUtil::MockFetchDataConsumerReader;
template <typename T>
using HandleReaderRunner = DataConsumerHandleTestUtil::HandleReaderRunner<T>;

String toString(const Vector<char>& v)
{
    return String(v.data(), v.size());
}

template<typename Handle>
class TeeCreationThread {
public:
    void run(PassOwnPtr<Handle> src, OwnPtr<Handle>* dest1, OwnPtr<Handle>* dest2)
    {
        m_thread = adoptPtr(new Thread("src thread", Thread::WithExecutionContext));
        m_waitableEvent = adoptPtr(Platform::current()->createWaitableEvent());
        m_thread->thread()->postTask(FROM_HERE, new Task(threadSafeBind(&TeeCreationThread<Handle>::runInternal, AllowCrossThreadAccess(this), src, AllowCrossThreadAccess(dest1), AllowCrossThreadAccess(dest2))));
        m_waitableEvent->wait();
    }

    Thread* thread() { return m_thread.get(); }

private:
    void runInternal(PassOwnPtr<Handle> src, OwnPtr<Handle>* dest1, OwnPtr<Handle>* dest2)
    {
        DataConsumerTee::create(m_thread->executionContext(), src, dest1, dest2);
        m_waitableEvent->signal();
    }

    OwnPtr<Thread> m_thread;
    OwnPtr<WebWaitableEvent> m_waitableEvent;
};

TEST(DataConsumerTeeTest, CreateDone)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release()), r2(dest2.release());

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res1->result());
    EXPECT_EQ(0u, res1->data().size());
    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ(0u, res2->data().size());
}

TEST(DataConsumerTeeTest, Read)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Wait));
    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release());
    HandleReaderRunner<HandleReader> r2(dest2.release());

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res1->result());
    EXPECT_EQ("hello, world", toString(res1->data()));

    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ("hello, world", toString(res2->data()));
}

TEST(DataConsumerTeeTest, TwoPhaseRead)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Wait));
    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Wait));
    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleTwoPhaseReader> r1(dest1.release());
    HandleReaderRunner<HandleTwoPhaseReader> r2(dest2.release());

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res1->result());
    EXPECT_EQ("hello, world", toString(res1->data()));

    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ("hello, world", toString(res2->data()));
}

TEST(DataConsumerTeeTest, Error)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Error));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release());
    HandleReaderRunner<HandleReader> r2(dest2.release());

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kUnexpectedError, res1->result());
    EXPECT_EQ(kUnexpectedError, res2->result());
}

void postStop(Thread* thread)
{
    thread->executionContext()->stopActiveDOMObjects();
}

TEST(DataConsumerTeeTest, StopSource)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release());
    HandleReaderRunner<HandleReader> r2(dest2.release());

    // We can pass a raw pointer because the subsequent |wait| calls ensure
    // t->thread() is alive.
    t->thread()->thread()->postTask(FROM_HERE, new Task(threadSafeBind(postStop, AllowCrossThreadAccess(t->thread()))));

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kUnexpectedError, res1->result());
    EXPECT_EQ(kUnexpectedError, res2->result());
}

TEST(DataConsumerTeeTest, DetachSource)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release());
    HandleReaderRunner<HandleReader> r2(dest2.release());

    t = nullptr;

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kUnexpectedError, res1->result());
    EXPECT_EQ(kUnexpectedError, res2->result());
}

TEST(DataConsumerTeeTest, DetachSourceAfterReadingDone)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    HandleReaderRunner<HandleReader> r1(dest1.release());
    OwnPtr<HandleReadResult> res1 = r1.wait();

    EXPECT_EQ(kDone, res1->result());
    EXPECT_EQ("hello, world", toString(res1->data()));

    t = nullptr;

    HandleReaderRunner<HandleReader> r2(dest2.release());
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ("hello, world", toString(res2->data()));
}

TEST(DataConsumerTeeTest, DetachOneDestination)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    dest1 = nullptr;

    HandleReaderRunner<HandleReader> r2(dest2.release());
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ("hello, world", toString(res2->data()));
}

TEST(DataConsumerTeeTest, DetachBothDestinationsShouldStopSourceReader)
{
    OwnPtr<Handle> src(Handle::create());
    RefPtr<Handle::Context> context(src->context());
    OwnPtr<WebDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Data, "hello, "));
    src->add(Command(Command::Data, "world"));

    OwnPtr<TeeCreationThread<WebDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<WebDataConsumerHandle>());
    t->run(src.release(), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    dest1 = nullptr;
    dest2 = nullptr;

    // Collect garbage to finalize the source reader.
    Heap::collectAllGarbage();
    context->detached()->wait();
}

TEST(FetchDataConsumerTeeTest, Create)
{
    RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create();
    OwnPtr<MockFetchDataConsumerHandle> src(MockFetchDataConsumerHandle::create());
    OwnPtr<MockFetchDataConsumerReader> reader(MockFetchDataConsumerReader::create());

    Checkpoint checkpoint;
    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*src, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kAllowBlobWithInvalidSize)).WillOnce(Return(blobDataHandle));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(checkpoint, Call(2));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    OwnPtr<FetchDataConsumerHandle> dest1, dest2;
    OwnPtr<TeeCreationThread<FetchDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<FetchDataConsumerHandle>());

    checkpoint.Call(1);
    t->run(src.release(), &dest1, &dest2);
    checkpoint.Call(2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);
    EXPECT_EQ(blobDataHandle, dest1->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
    EXPECT_EQ(blobDataHandle, dest2->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
}

TEST(FetchDataConsumerTeeTest, CreateFromBlobWithInvalidSize)
{
    RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create(BlobData::create(), -1);
    OwnPtr<MockFetchDataConsumerHandle> src(MockFetchDataConsumerHandle::create());
    OwnPtr<MockFetchDataConsumerReader> reader(MockFetchDataConsumerReader::create());

    Checkpoint checkpoint;
    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*src, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kAllowBlobWithInvalidSize)).WillOnce(Return(blobDataHandle));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(checkpoint, Call(2));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    OwnPtr<FetchDataConsumerHandle> dest1, dest2;
    OwnPtr<TeeCreationThread<FetchDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<FetchDataConsumerHandle>());

    checkpoint.Call(1);
    t->run(src.release(), &dest1, &dest2);
    checkpoint.Call(2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);
    EXPECT_FALSE(dest1->obtainReader(nullptr)->drainAsBlobDataHandle(kDisallowBlobWithInvalidSize));
    EXPECT_EQ(blobDataHandle, dest1->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
    EXPECT_FALSE(dest2->obtainReader(nullptr)->drainAsBlobDataHandle(kDisallowBlobWithInvalidSize));
    EXPECT_EQ(blobDataHandle, dest2->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
}

TEST(FetchDataConsumerTeeTest, CreateDone)
{
    OwnPtr<Handle> src(Handle::create());
    OwnPtr<FetchDataConsumerHandle> dest1, dest2;

    src->add(Command(Command::Done));

    OwnPtr<TeeCreationThread<FetchDataConsumerHandle>> t = adoptPtr(new TeeCreationThread<FetchDataConsumerHandle>());
    t->run(createFetchDataConsumerHandleFromWebHandle(src.release()), &dest1, &dest2);

    ASSERT_TRUE(dest1);
    ASSERT_TRUE(dest2);

    EXPECT_FALSE(dest1->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
    EXPECT_FALSE(dest2->obtainReader(nullptr)->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));

    HandleReaderRunner<HandleReader> r1(dest1.release()), r2(dest2.release());

    OwnPtr<HandleReadResult> res1 = r1.wait();
    OwnPtr<HandleReadResult> res2 = r2.wait();

    EXPECT_EQ(kDone, res1->result());
    EXPECT_EQ(0u, res1->data().size());
    EXPECT_EQ(kDone, res2->result());
    EXPECT_EQ(0u, res2->data().size());
}

} // namespace
} // namespace blink
