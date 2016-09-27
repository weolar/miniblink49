// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/DataConsumerHandleUtil.h"

#include "modules/fetch/DataConsumerHandleTestUtil.h"

namespace blink {

namespace {

const WebDataConsumerHandle::Result kShouldWait = WebDataConsumerHandle::ShouldWait;
const WebDataConsumerHandle::Result kDone = WebDataConsumerHandle::Done;
const WebDataConsumerHandle::Result kUnexpectedError = WebDataConsumerHandle::UnexpectedError;
const WebDataConsumerHandle::Flags kNone = WebDataConsumerHandle::FlagNone;

TEST(DataConsumerHandleUtilTest, CreateWaitingHandle)
{
    char buffer[20];
    const void* p = nullptr;
    size_t size = 0;
    OwnPtr<WebDataConsumerHandle> handle = createWaitingDataConsumerHandle();
    OwnPtr<WebDataConsumerHandle::Reader> reader = handle->obtainReader(nullptr);

    EXPECT_EQ(kShouldWait, reader->read(buffer, sizeof(buffer), kNone, &size));
    EXPECT_EQ(kShouldWait, reader->beginRead(&p, kNone, &size));
    EXPECT_EQ(kUnexpectedError, reader->endRead(99));
}

TEST(CompositeDataConsumerHandleTest, WaitingHandleNoNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNoNotificationTest test;
    // Test this function doesn't crash.
    test.run(createWaitingDataConsumerHandle());
}

TEST(DataConsumerHandleUtilTest, CreateDoneHandle)
{
    char buffer[20];
    const void* p = nullptr;
    size_t size = 0;
    OwnPtr<WebDataConsumerHandle> handle = createDoneDataConsumerHandle();
    OwnPtr<WebDataConsumerHandle::Reader> reader = handle->obtainReader(nullptr);

    EXPECT_EQ(kDone, reader->read(buffer, sizeof(buffer), kNone, &size));
    EXPECT_EQ(kDone, reader->beginRead(&p, kNone, &size));
    EXPECT_EQ(kUnexpectedError, reader->endRead(99));
}

TEST(DataConsumerHandleUtilTest, DoneHandleNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNotificationTest test;
    // Test this function returns.
    test.run(createDoneDataConsumerHandle());
}

TEST(CompositeDataConsumerHandleTest, DoneHandleNoNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNoNotificationTest test;
    // Test this function doesn't crash.
    test.run(createDoneDataConsumerHandle());
}

TEST(DataConsumerHandleUtilTest, CreateUnexpectedErrorHandle)
{
    char buffer[20];
    const void* p = nullptr;
    size_t size = 0;
    OwnPtr<WebDataConsumerHandle> handle = createUnexpectedErrorDataConsumerHandle();
    OwnPtr<WebDataConsumerHandle::Reader> reader = handle->obtainReader(nullptr);

    EXPECT_EQ(kUnexpectedError, reader->read(buffer, sizeof(buffer), kNone, &size));
    EXPECT_EQ(kUnexpectedError, reader->beginRead(&p, kNone, &size));
    EXPECT_EQ(kUnexpectedError, reader->endRead(99));
}

TEST(DataConsumerHandleUtilTest, UnexpectedErrorHandleNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNotificationTest test;
    // Test this function returns.
    test.run(createUnexpectedErrorDataConsumerHandle());
}

TEST(CompositeDataConsumerHandleTest, UnexpectedErrorHandleNoNotification)
{
    DataConsumerHandleTestUtil::ThreadingHandleNoNotificationTest test;
    // Test this function doesn't crash.
    test.run(createUnexpectedErrorDataConsumerHandle());
}

} // namespace

} // namespace blink
