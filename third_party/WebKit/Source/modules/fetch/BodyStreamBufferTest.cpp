// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/BodyStreamBuffer.h"

#include "core/testing/DummyPageHolder.h"
#include "modules/fetch/DataConsumerHandleTestUtil.h"
#include "platform/testing/UnitTestHelpers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {

namespace {

using ::testing::InSequence;
using ::testing::_;
using ::testing::Return;
using Checkpoint = ::testing::StrictMock<::testing::MockFunction<void(int)>>;
using Command = DataConsumerHandleTestUtil::Command;
using ReplayingHandle = DataConsumerHandleTestUtil::ReplayingHandle;
using MockFetchDataConsumerHandle = DataConsumerHandleTestUtil::MockFetchDataConsumerHandle;
using MockFetchDataConsumerReader = DataConsumerHandleTestUtil::MockFetchDataConsumerReader;
using MockFetchDataLoaderClient = DataConsumerHandleTestUtil::MockFetchDataLoaderClient;

const FetchDataConsumerHandle::Reader::BlobSizePolicy kAllowBlobWithInvalidSize = FetchDataConsumerHandle::Reader::AllowBlobWithInvalidSize;

class MockDrainingStreamNotificationClient : public GarbageCollectedFinalized<MockDrainingStreamNotificationClient>, public BodyStreamBuffer::DrainingStreamNotificationClient {
    USING_GARBAGE_COLLECTED_MIXIN(MockDrainingStreamNotificationClient);
public:
    static ::testing::StrictMock<MockDrainingStreamNotificationClient>* create() { return new ::testing::StrictMock<MockDrainingStreamNotificationClient>; }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        DrainingStreamNotificationClient::trace(visitor);
    }

    MOCK_METHOD0(didFetchDataLoadFinishedFromDrainingStream, void());
};

class DrainingBodyStreamBufferTest : public ::testing::Test {
public:
    DrainingBodyStreamBufferTest()
        : m_page(DummyPageHolder::create(IntSize(1, 1))) { }

    ScriptState* scriptState() { return ScriptState::forMainWorld(m_page->document().frame()); }
    ExecutionContext* executionContext() { return scriptState()->executionContext(); }

private:
    OwnPtr<DummyPageHolder> m_page;
};

TEST_F(DrainingBodyStreamBufferTest, NotifyOnDestruction)
{
    Checkpoint checkpoint;
    InSequence s;

    MockDrainingStreamNotificationClient* client = MockDrainingStreamNotificationClient::create();

    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*client, didFetchDataLoadFinishedFromDrainingStream());
    EXPECT_CALL(checkpoint, Call(2));

    BodyStreamBuffer* buffer = BodyStreamBuffer::createEmpty();
    OwnPtr<DrainingBodyStreamBuffer> drainingBuffer = DrainingBodyStreamBuffer::create(buffer, client);
    checkpoint.Call(1);
    drainingBuffer.clear();
    checkpoint.Call(2);
}

TEST_F(DrainingBodyStreamBufferTest, NotifyWhenBlobDataHandleIsDrained)
{
    Checkpoint checkpoint;
    InSequence s;

    OwnPtr<MockFetchDataConsumerHandle> src(MockFetchDataConsumerHandle::create());
    OwnPtr<MockFetchDataConsumerReader> reader(MockFetchDataConsumerReader::create());
    RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create();

    MockDrainingStreamNotificationClient* client = MockDrainingStreamNotificationClient::create();

    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*src, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kAllowBlobWithInvalidSize)).WillOnce(Return(blobDataHandle));
    EXPECT_CALL(*client, didFetchDataLoadFinishedFromDrainingStream());
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    BodyStreamBuffer* buffer = BodyStreamBuffer::create(src.release());
    checkpoint.Call(1);
    OwnPtr<DrainingBodyStreamBuffer> drainingBuffer = DrainingBodyStreamBuffer::create(buffer, client);
    checkpoint.Call(2);
    EXPECT_EQ(blobDataHandle, drainingBuffer->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
    checkpoint.Call(3);
    drainingBuffer.clear();
    checkpoint.Call(4);
}

TEST_F(DrainingBodyStreamBufferTest, DoNotNotifyWhenNullBlobDataHandleIsDrained)
{
    Checkpoint checkpoint;
    InSequence s;

    MockDrainingStreamNotificationClient* client = MockDrainingStreamNotificationClient::create();

    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*client, didFetchDataLoadFinishedFromDrainingStream());
    EXPECT_CALL(checkpoint, Call(3));

    BodyStreamBuffer* buffer = BodyStreamBuffer::createEmpty();
    OwnPtr<DrainingBodyStreamBuffer> drainingBuffer = DrainingBodyStreamBuffer::create(buffer, client);
    checkpoint.Call(1);
    EXPECT_FALSE(drainingBuffer->drainAsBlobDataHandle(kAllowBlobWithInvalidSize));
    checkpoint.Call(2);
    drainingBuffer.clear();
    checkpoint.Call(3);
}

TEST_F(DrainingBodyStreamBufferTest, DoNotNotifyWhenBufferIsLeaked)
{
    Checkpoint checkpoint;
    InSequence s;

    MockDrainingStreamNotificationClient* client = MockDrainingStreamNotificationClient::create();

    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(checkpoint, Call(3));

    BodyStreamBuffer* buffer = BodyStreamBuffer::createEmpty();

    OwnPtr<DrainingBodyStreamBuffer> drainingBuffer = DrainingBodyStreamBuffer::create(buffer, client);
    checkpoint.Call(1);
    EXPECT_EQ(buffer, drainingBuffer->leakBuffer());
    checkpoint.Call(2);
    drainingBuffer.clear();
    checkpoint.Call(3);
}

TEST_F(DrainingBodyStreamBufferTest, NotifyOnStartLoading)
{
    Checkpoint checkpoint;
    InSequence s;

    OwnPtr<ReplayingHandle> src(ReplayingHandle::create());
    src->add(Command(Command::Data, "hello, world."));
    src->add(Command(Command::Done));

    MockDrainingStreamNotificationClient* client = MockDrainingStreamNotificationClient::create();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();

    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedString(String("hello, world.")));
    EXPECT_CALL(*client, didFetchDataLoadFinishedFromDrainingStream());
    EXPECT_CALL(checkpoint, Call(5));

    Persistent<BodyStreamBuffer> buffer = BodyStreamBuffer::create(createFetchDataConsumerHandleFromWebHandle(src.release()));
    OwnPtr<DrainingBodyStreamBuffer> drainingBuffer = DrainingBodyStreamBuffer::create(buffer, client);
    checkpoint.Call(1);
    drainingBuffer->startLoading(executionContext(), fetchDataLoader, fetchDataLoaderClient);
    checkpoint.Call(2);
    drainingBuffer.clear();
    checkpoint.Call(3);
    // Loading continues as long as |buffer| is alive.
    Heap::collectAllGarbage();
    checkpoint.Call(4);
    testing::runPendingTasks();
    checkpoint.Call(5);
}

} // namespace

} // namespace blink
