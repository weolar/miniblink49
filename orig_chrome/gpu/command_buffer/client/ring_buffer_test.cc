// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the tests for the RingBuffer class.

#include "gpu/command_buffer/client/ring_buffer.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "gpu/command_buffer/client/cmd_buffer_helper.h"
#include "gpu/command_buffer/service/cmd_buffer_engine.h"
#include "gpu/command_buffer/service/command_buffer_service.h"
#include "gpu/command_buffer/service/gpu_scheduler.h"
#include "gpu/command_buffer/service/mocks.h"
#include "gpu/command_buffer/service/transfer_buffer_manager.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

using testing::_;
using testing::DoAll;
using testing::Invoke;
using testing::Mock;
using testing::Return;
using testing::Sequence;
using testing::Truly;

class BaseRingBufferTest : public testing::Test {
protected:
    static const unsigned int kBaseOffset = 128;
    static const unsigned int kBufferSize = 1024;
    static const unsigned int kAlignment = 4;

    void RunPendingSetToken()
    {
        for (std::vector<const void*>::iterator it = set_token_arguments_.begin();
             it != set_token_arguments_.end();
             ++it) {
            api_mock_->SetToken(cmd::kSetToken, 1, *it);
        }
        set_token_arguments_.clear();
        delay_set_token_ = false;
    }

    void SetToken(unsigned int command,
        unsigned int arg_count,
        const void* _args)
    {
        EXPECT_EQ(static_cast<unsigned int>(cmd::kSetToken), command);
        EXPECT_EQ(1u, arg_count);
        if (delay_set_token_)
            set_token_arguments_.push_back(_args);
        else
            api_mock_->SetToken(cmd::kSetToken, 1, _args);
    }

    void SetUp() override
    {
        delay_set_token_ = false;
        api_mock_.reset(new AsyncAPIMock(true));
        // ignore noops in the mock - we don't want to inspect the internals of the
        // helper.
        EXPECT_CALL(*api_mock_, DoCommand(cmd::kNoop, 0, _))
            .WillRepeatedly(Return(error::kNoError));
        // Forward the SetToken calls to the engine
        EXPECT_CALL(*api_mock_.get(), DoCommand(cmd::kSetToken, 1, _))
            .WillRepeatedly(DoAll(Invoke(this, &BaseRingBufferTest::SetToken),
                Return(error::kNoError)));

        {
            TransferBufferManager* manager = new TransferBufferManager(nullptr);
            transfer_buffer_manager_ = manager;
            EXPECT_TRUE(manager->Initialize());
        }
        command_buffer_.reset(
            new CommandBufferService(transfer_buffer_manager_.get()));
        EXPECT_TRUE(command_buffer_->Initialize());

        gpu_scheduler_.reset(new GpuScheduler(
            command_buffer_.get(), api_mock_.get(), NULL));
        command_buffer_->SetPutOffsetChangeCallback(base::Bind(
            &GpuScheduler::PutChanged, base::Unretained(gpu_scheduler_.get())));
        command_buffer_->SetGetBufferChangeCallback(base::Bind(
            &GpuScheduler::SetGetBuffer, base::Unretained(gpu_scheduler_.get())));

        api_mock_->set_engine(gpu_scheduler_.get());

        helper_.reset(new CommandBufferHelper(command_buffer_.get()));
        helper_->Initialize(kBufferSize);
    }

    int32 GetToken()
    {
        return command_buffer_->GetLastState().token;
    }

    scoped_ptr<AsyncAPIMock> api_mock_;
    scoped_refptr<TransferBufferManagerInterface> transfer_buffer_manager_;
    scoped_ptr<CommandBufferService> command_buffer_;
    scoped_ptr<GpuScheduler> gpu_scheduler_;
    scoped_ptr<CommandBufferHelper> helper_;
    std::vector<const void*> set_token_arguments_;
    bool delay_set_token_;

    scoped_ptr<int8[]> buffer_;
    int8* buffer_start_;
};

#ifndef _MSC_VER
const unsigned int BaseRingBufferTest::kBaseOffset;
const unsigned int BaseRingBufferTest::kBufferSize;
#endif

// Test fixture for RingBuffer test - Creates a RingBuffer, using a
// CommandBufferHelper with a mock AsyncAPIInterface for its interface (calling
// it directly, not through the RPC mechanism), making sure Noops are ignored
// and SetToken are properly forwarded to the engine.
class RingBufferTest : public BaseRingBufferTest {
protected:
    void SetUp() override
    {
        BaseRingBufferTest::SetUp();

        buffer_.reset(new int8[kBufferSize + kBaseOffset]);
        buffer_start_ = buffer_.get() + kBaseOffset;
        allocator_.reset(new RingBuffer(kAlignment, kBaseOffset, kBufferSize,
            helper_.get(), buffer_start_));
    }

    void TearDown() override
    {
        // If the GpuScheduler posts any tasks, this forces them to run.
        base::MessageLoop::current()->RunUntilIdle();

        BaseRingBufferTest::TearDown();
    }

    scoped_ptr<RingBuffer> allocator_;
};

// Checks basic alloc and free.
TEST_F(RingBufferTest, TestBasic)
{
    const unsigned int kSize = 16;
    EXPECT_EQ(kBufferSize, allocator_->GetLargestFreeOrPendingSize());
    EXPECT_EQ(kBufferSize, allocator_->GetLargestFreeSizeNoWaiting());
    void* pointer = allocator_->Alloc(kSize);
    EXPECT_GE(kBufferSize, allocator_->GetOffset(pointer) - kBaseOffset + kSize);
    EXPECT_EQ(kBufferSize, allocator_->GetLargestFreeOrPendingSize());
    EXPECT_EQ(kBufferSize - kSize, allocator_->GetLargestFreeSizeNoWaiting());
    int32 token = helper_->InsertToken();
    allocator_->FreePendingToken(pointer, token);
}

// Checks the free-pending-token mechanism.
TEST_F(RingBufferTest, TestFreePendingToken)
{
    const unsigned int kSize = 16;
    const unsigned int kAllocCount = kBufferSize / kSize;
    CHECK(kAllocCount * kSize == kBufferSize);

    delay_set_token_ = true;
    // Allocate several buffers to fill in the memory.
    int32 tokens[kAllocCount];
    for (unsigned int ii = 0; ii < kAllocCount; ++ii) {
        void* pointer = allocator_->Alloc(kSize);
        EXPECT_GE(kBufferSize,
            allocator_->GetOffset(pointer) - kBaseOffset + kSize);
        tokens[ii] = helper_->InsertToken();
        allocator_->FreePendingToken(pointer, tokens[ii]);
    }

    EXPECT_EQ(kBufferSize - (kSize * kAllocCount),
        allocator_->GetLargestFreeSizeNoWaiting());

    RunPendingSetToken();

    // This allocation will need to reclaim the space freed above, so that should
    // process the commands until a token is passed.
    void* pointer1 = allocator_->Alloc(kSize);
    EXPECT_EQ(kBaseOffset, allocator_->GetOffset(pointer1));

    // Check that the token has indeed passed.
    EXPECT_LE(tokens[0], GetToken());

    allocator_->FreePendingToken(pointer1, helper_->InsertToken());
}

// Tests GetLargestFreeSizeNoWaiting
TEST_F(RingBufferTest, TestGetLargestFreeSizeNoWaiting)
{
    EXPECT_EQ(kBufferSize, allocator_->GetLargestFreeSizeNoWaiting());

    void* pointer = allocator_->Alloc(kBufferSize);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(pointer, helper_->InsertToken());
}

TEST_F(RingBufferTest, TestFreeBug)
{
    // The first and second allocations must not match.
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 20;
    void* pointer = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(pointer, helper_.get()->InsertToken());
    pointer = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(pointer, helper_.get()->InsertToken());
    pointer = allocator_->Alloc(kBufferSize);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(pointer, helper_.get()->InsertToken());
}

// Test that discarding a single allocation clears the block.
TEST_F(RingBufferTest, DiscardTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    void* ptr = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->DiscardBlock(ptr);
    EXPECT_EQ(kBufferSize, allocator_->GetLargestFreeSizeNoWaiting());
}

// Test that discarding front of the buffer effectively frees the block.
TEST_F(RingBufferTest, DiscardFrontTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discard first block should free it up upon GetLargestFreeSizeNoWaiting().
    allocator_->DiscardBlock(ptr1);
    EXPECT_EQ(kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr3, helper_.get()->InsertToken());
}

// Test that discarding middle of the buffer merely marks it as padding.
TEST_F(RingBufferTest, DiscardMiddleTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discard middle block should just set it as padding.
    allocator_->DiscardBlock(ptr2);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr3, helper_.get()->InsertToken());
}

// Test that discarding end of the buffer frees it for no waiting.
TEST_F(RingBufferTest, DiscardEndTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discard end block should discard it.
    allocator_->DiscardBlock(ptr3);
    EXPECT_EQ(kAlloc3, allocator_->GetLargestFreeSizeNoWaiting());
}

// Test discard end of the buffer that has looped around.
TEST_F(RingBufferTest, DiscardLoopedEndTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr3, helper_.get()->InsertToken());

    // This allocation should be at the beginning again, we need to utilize
    // DiscardBlock here to discard the first item so that we can allocate
    // at the beginning without the FreeOldestBlock() getting called and freeing
    // the whole ring buffer.
    allocator_->DiscardBlock(ptr1);
    void* ptr4 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(ptr1, ptr4);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discard end block should work properly still.
    allocator_->DiscardBlock(ptr4);
    EXPECT_EQ(kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
}

// Test discard end of the buffer that has looped around with padding.
TEST_F(RingBufferTest, DiscardEndWithPaddingTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kPadding = kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2 - kPadding;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(kPadding, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr3, helper_.get()->InsertToken());

    // Cause it to loop around with padding at the end of ptr3.
    allocator_->DiscardBlock(ptr1);
    void* ptr4 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(ptr1, ptr4);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discard end block should also discard the padding.
    allocator_->DiscardBlock(ptr4);
    EXPECT_EQ(kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());

    // We can test that there is padding by attempting to allocate the padding.
    void* padding = allocator_->Alloc(kPadding);
    EXPECT_EQ(kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(padding, helper_.get()->InsertToken());
}

// Test that discard will effectively remove all padding at the end.
TEST_F(RingBufferTest, DiscardAllPaddingFromEndTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discarding the middle allocation should turn it into padding.
    allocator_->DiscardBlock(ptr2);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discarding the last allocation should discard the middle padding as well.
    allocator_->DiscardBlock(ptr3);
    EXPECT_EQ(kAlloc2 + kAlloc3, allocator_->GetLargestFreeSizeNoWaiting());
}

// Test that discard will effectively remove all padding from the beginning.
TEST_F(RingBufferTest, DiscardAllPaddingFromBeginningTest)
{
    const unsigned int kAlloc1 = 3 * kAlignment;
    const unsigned int kAlloc2 = 2 * kAlignment;
    const unsigned int kAlloc3 = kBufferSize - kAlloc1 - kAlloc2;
    void* ptr1 = allocator_->Alloc(kAlloc1);
    EXPECT_EQ(kBufferSize - kAlloc1, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr1, helper_.get()->InsertToken());

    void* ptr2 = allocator_->Alloc(kAlloc2);
    EXPECT_EQ(static_cast<uint8_t*>(ptr1) + kAlloc1,
        static_cast<uint8_t*>(ptr2));
    EXPECT_EQ(kBufferSize - kAlloc1 - kAlloc2,
        allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr2, helper_.get()->InsertToken());

    void* ptr3 = allocator_->Alloc(kAlloc3);
    EXPECT_EQ(static_cast<uint8_t*>(ptr2) + kAlloc2,
        static_cast<uint8_t*>(ptr3));
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());
    allocator_->FreePendingToken(ptr3, helper_.get()->InsertToken());

    // Discarding the middle allocation should turn it into padding.
    allocator_->DiscardBlock(ptr2);
    EXPECT_EQ(0u, allocator_->GetLargestFreeSizeNoWaiting());

    // Discarding the first allocation should discard the middle padding as well.
    allocator_->DiscardBlock(ptr1);
    EXPECT_EQ(kAlloc1 + kAlloc2, allocator_->GetLargestFreeSizeNoWaiting());
}

} // namespace gpu
