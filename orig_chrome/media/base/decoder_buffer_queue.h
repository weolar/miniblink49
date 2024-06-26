// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_DECODER_BUFFER_QUEUE_H_
#define MEDIA_BASE_DECODER_BUFFER_QUEUE_H_

#include <deque>

#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "media/base/media_export.h"

namespace media {

class DecoderBuffer;

// Maintains a queue of DecoderBuffers in increasing timestamp order.
//
// Individual buffer durations are ignored when calculating the duration of the
// queue i.e., the queue must have at least 2 in-order buffers to calculate
// duration.
//
// Not thread safe: access must be externally synchronized.
class MEDIA_EXPORT DecoderBufferQueue {
public:
    DecoderBufferQueue();
    ~DecoderBufferQueue();

    // Push |buffer| to the end of the queue. If |buffer| is queued out of order
    // it will be excluded from duration calculations.
    //
    // It is invalid to push an end-of-stream |buffer|.
    void Push(const scoped_refptr<DecoderBuffer>& buffer);

    // Pops a DecoderBuffer from the front of the queue.
    //
    // It is invalid to call Pop() on an empty queue.
    scoped_refptr<DecoderBuffer> Pop();

    // Removes all queued buffers.
    void Clear();

    // Returns true if this queue is empty.
    bool IsEmpty();

    // Returns the duration of encoded data stored in this queue as measured by
    // the timestamps of the earliest and latest buffers, ignoring out of order
    // buffers.
    //
    // Returns zero if the queue is empty.
    base::TimeDelta Duration();

    // Returns the total size of buffers inside the queue.
    size_t data_size() const { return data_size_; }

private:
    typedef std::deque<scoped_refptr<DecoderBuffer>> Queue;
    Queue queue_;

    // A subset of |queue_| that contains buffers that are in strictly
    // increasing timestamp order. Used to calculate Duration() while ignoring
    // out-of-order buffers.
    Queue in_order_queue_;

    base::TimeDelta earliest_valid_timestamp_;

    // Total size in bytes of buffers in the queue.
    size_t data_size_;

    DISALLOW_COPY_AND_ASSIGN(DecoderBufferQueue);
};

} // namespace media

#endif // MEDIA_BASE_DECODER_BUFFER_QUEUE_H_
