// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchDataLoader.h"

#include "modules/fetch/DataConsumerHandleTestUtil.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {

namespace {

using ::testing::InSequence;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::StrictMock;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::SetArgPointee;
using Checkpoint = StrictMock<::testing::MockFunction<void(int)>>;
using MockFetchDataLoaderClient = DataConsumerHandleTestUtil::MockFetchDataLoaderClient;
using MockHandle = DataConsumerHandleTestUtil::MockFetchDataConsumerHandle;
using MockReader = DataConsumerHandleTestUtil::MockFetchDataConsumerReader;

const WebDataConsumerHandle::Result kOk = WebDataConsumerHandle::Ok;
const WebDataConsumerHandle::Result kUnexpectedError = WebDataConsumerHandle::UnexpectedError;
const WebDataConsumerHandle::Result kDone = WebDataConsumerHandle::Done;
const WebDataConsumerHandle::Flags kNone = WebDataConsumerHandle::FlagNone;
const FetchDataConsumerHandle::Reader::BlobSizePolicy kDisallowBlobWithInvalidSize = FetchDataConsumerHandle::Reader::DisallowBlobWithInvalidSize;

const char kQuickBrownFox[] = "Quick brown fox";
const size_t kQuickBrownFoxLength = 15;
const size_t kQuickBrownFoxLengthWithTerminatingNull = 16;

TEST(FetchDataLoaderTest, LoadAsBlob)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle("text/test");
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();
    RefPtr<BlobDataHandle> blobDataHandle;

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kDisallowBlobWithInvalidSize)).WillOnce(Return(nullptr));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLengthWithTerminatingNull), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLengthWithTerminatingNull)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kDone));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedBlobHandleMock(_)).WillOnce(SaveArg<0>(&blobDataHandle));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);

    ASSERT_TRUE(blobDataHandle);
    EXPECT_EQ(kQuickBrownFoxLengthWithTerminatingNull, blobDataHandle->size());
    EXPECT_EQ(String("text/test"), blobDataHandle->type());
}

TEST(FetchDataLoaderTest, LoadAsBlobFailed)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle("text/test");
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kDisallowBlobWithInvalidSize)).WillOnce(Return(nullptr));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLengthWithTerminatingNull), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLengthWithTerminatingNull)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kUnexpectedError));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadFailed());
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);
}

TEST(FetchDataLoaderTest, LoadAsBlobCancel)
{
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle("text/test");
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kDisallowBlobWithInvalidSize)).WillOnce(Return(nullptr));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(checkpoint, Call(3));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    fetchDataLoader->cancel();
    checkpoint.Call(3);
}

TEST(FetchDataLoaderTest, LoadAsBlobViaDrainAsBlobDataHandleWithSameContentType)
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->appendBytes(kQuickBrownFox, kQuickBrownFoxLengthWithTerminatingNull);
    blobData->setContentType("text/test");
    RefPtr<BlobDataHandle> inputBlobDataHandle = BlobDataHandle::create(blobData.release(), kQuickBrownFoxLengthWithTerminatingNull);

    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle("text/test");
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();
    RefPtr<BlobDataHandle> blobDataHandle;

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kDisallowBlobWithInvalidSize)).WillOnce(Return(inputBlobDataHandle));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedBlobHandleMock(_)).WillOnce(SaveArg<0>(&blobDataHandle));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(checkpoint, Call(3));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    fetchDataLoader->cancel();
    checkpoint.Call(3);

    ASSERT_TRUE(blobDataHandle);
    EXPECT_EQ(inputBlobDataHandle, blobDataHandle);
    EXPECT_EQ(kQuickBrownFoxLengthWithTerminatingNull, blobDataHandle->size());
    EXPECT_EQ(String("text/test"), blobDataHandle->type());
}

TEST(FetchDataLoaderTest, LoadAsBlobViaDrainAsBlobDataHandleWithDifferentContentType)
{
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->appendBytes(kQuickBrownFox, kQuickBrownFoxLengthWithTerminatingNull);
    blobData->setContentType("text/different");
    RefPtr<BlobDataHandle> inputBlobDataHandle = BlobDataHandle::create(blobData.release(), kQuickBrownFoxLengthWithTerminatingNull);

    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsBlobHandle("text/test");
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();
    RefPtr<BlobDataHandle> blobDataHandle;

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(*reader, drainAsBlobDataHandle(kDisallowBlobWithInvalidSize)).WillOnce(Return(inputBlobDataHandle));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedBlobHandleMock(_)).WillOnce(SaveArg<0>(&blobDataHandle));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(checkpoint, Call(3));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    fetchDataLoader->cancel();
    checkpoint.Call(3);

    ASSERT_TRUE(blobDataHandle);
    EXPECT_NE(inputBlobDataHandle, blobDataHandle);
    EXPECT_EQ(kQuickBrownFoxLengthWithTerminatingNull, blobDataHandle->size());
    EXPECT_EQ(String("text/test"), blobDataHandle->type());
}

TEST(FetchDataLoaderTest, LoadAsArrayBuffer)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsArrayBuffer();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();
    RefPtr<DOMArrayBuffer> arrayBuffer;

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLengthWithTerminatingNull), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLengthWithTerminatingNull)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kDone));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedArrayBufferMock(_)).WillOnce(SaveArg<0>(&arrayBuffer));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);

    ASSERT_TRUE(arrayBuffer);
    ASSERT_EQ(kQuickBrownFoxLengthWithTerminatingNull, arrayBuffer->byteLength());
    EXPECT_STREQ(kQuickBrownFox, static_cast<const char*>(arrayBuffer->data()));
}

TEST(FetchDataLoaderTest, LoadAsArrayBufferFailed)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsArrayBuffer();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLengthWithTerminatingNull), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLengthWithTerminatingNull)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kUnexpectedError));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadFailed());
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);
}

TEST(FetchDataLoaderTest, LoadAsArrayBufferCancel)
{
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsArrayBuffer();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(checkpoint, Call(3));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    fetchDataLoader->cancel();
    checkpoint.Call(3);
}

TEST(FetchDataLoaderTest, LoadAsString)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLength), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLength)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kDone));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedString(String(kQuickBrownFox)));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);
}

TEST(FetchDataLoaderTest, LoadAsStringWithNullBytes)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>("Quick\0brown\0fox")), SetArgPointee<2>(16), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLengthWithTerminatingNull)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kDone));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadedString(String("Quick\0brown\0fox", 16)));
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);
}

TEST(FetchDataLoaderTest, LoadAsStringError)
{
    WebDataConsumerHandle::Client *client = nullptr;
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(DoAll(SaveArg<0>(&client), Return(reader.get())));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(DoAll(SetArgPointee<0>(static_cast<const void*>(kQuickBrownFox)), SetArgPointee<2>(kQuickBrownFoxLength), Return(kOk)));
    EXPECT_CALL(*reader, endRead(kQuickBrownFoxLength)).WillOnce(Return(kOk));
    EXPECT_CALL(*reader, beginRead(_, kNone, _)).WillOnce(Return(kUnexpectedError));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(*fetchDataLoaderClient, didFetchDataLoadFailed());
    EXPECT_CALL(checkpoint, Call(3));
    EXPECT_CALL(checkpoint, Call(4));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    ASSERT_TRUE(client);
    client->didGetReadable();
    checkpoint.Call(3);
    fetchDataLoader->cancel();
    checkpoint.Call(4);
}

TEST(FetchDataLoaderTest, LoadAsStringCancel)
{
    Checkpoint checkpoint;

    OwnPtr<MockHandle> handle = MockHandle::create();
    OwnPtr<MockReader> reader = MockReader::create();
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();
    MockFetchDataLoaderClient* fetchDataLoaderClient = MockFetchDataLoaderClient::create();

    InSequence s;
    EXPECT_CALL(checkpoint, Call(1));
    EXPECT_CALL(*handle, obtainReaderInternal(_)).WillOnce(Return(reader.get()));
    EXPECT_CALL(checkpoint, Call(2));
    EXPECT_CALL(*reader, destruct());
    EXPECT_CALL(checkpoint, Call(3));

    // |reader| is adopted by |obtainReader|.
    ASSERT_TRUE(reader.leakPtr());

    checkpoint.Call(1);
    fetchDataLoader->start(handle.get(), fetchDataLoaderClient);
    checkpoint.Call(2);
    fetchDataLoader->cancel();
    checkpoint.Call(3);
}

} // namespace

} // namespace blink
