// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/byte_queue.h"

#include "base/logging.h"

namespace media {

// Default starting size for the queue.
enum { kDefaultQueueSize = 1024 };

ByteQueue::ByteQueue()
    : buffer_(new uint8[kDefaultQueueSize])
    , size_(kDefaultQueueSize)
    , offset_(0)
    , used_(0)
{
}

ByteQueue::~ByteQueue() { }

void ByteQueue::Reset()
{
    offset_ = 0;
    used_ = 0;
}

void ByteQueue::Push(const uint8* data, int size)
{
    DCHECK(data);
    DCHECK_GT(size, 0);

    size_t size_needed = used_ + size;

    // Check to see if we need a bigger buffer.
    if (size_needed > size_) {
        size_t new_size = 2 * size_;
        while (size_needed > new_size && new_size > size_)
            new_size *= 2;

        // Sanity check to make sure we didn't overflow.
        CHECK_GT(new_size, size_);

        scoped_ptr<uint8[]> new_buffer(new uint8[new_size]);

        // Copy the data from the old buffer to the start of the new one.
        if (used_ > 0)
            memcpy(new_buffer.get(), front(), used_);

        buffer_.reset(new_buffer.release());
        size_ = new_size;
        offset_ = 0;
    } else if ((offset_ + used_ + size) > size_) {
        // The buffer is big enough, but we need to move the data in the queue.
        memmove(buffer_.get(), front(), used_);
        offset_ = 0;
    }

    memcpy(front() + used_, data, size);
    used_ += size;
}

void ByteQueue::Peek(const uint8** data, int* size) const
{
    DCHECK(data);
    DCHECK(size);
    *data = front();
    *size = used_;
}

void ByteQueue::Pop(int count)
{
    DCHECK_LE(count, used_);

    offset_ += count;
    used_ -= count;

    // Move the offset back to 0 if we have reached the end of the buffer.
    if (offset_ == size_) {
        DCHECK_EQ(used_, 0);
        offset_ = 0;
    }
}

uint8* ByteQueue::front() const { return buffer_.get() + offset_; }

} // namespace media
