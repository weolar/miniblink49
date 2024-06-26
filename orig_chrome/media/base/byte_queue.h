// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_BYTE_QUEUE_H_
#define MEDIA_BASE_BYTE_QUEUE_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"

namespace media {

// Represents a queue of bytes.
// Data is added to the end of the queue via an Push() call and removed via
// Pop(). The contents of the queue can be observed via the Peek() method.
// This class manages the underlying storage of the queue and tries to minimize
// the number of buffer copies when data is appended and removed.
class MEDIA_EXPORT ByteQueue {
public:
    ByteQueue();
    ~ByteQueue();

    // Reset the queue to the empty state.
    void Reset();

    // Appends new bytes onto the end of the queue.
    void Push(const uint8* data, int size);

    // Get a pointer to the front of the queue and the queue size.
    // These values are only valid until the next Push() or
    // Pop() call.
    void Peek(const uint8** data, int* size) const;

    // Remove |count| bytes from the front of the queue.
    void Pop(int count);

private:
    // Returns a pointer to the front of the queue.
    uint8* front() const;

    scoped_ptr<uint8[]> buffer_;

    // Size of |buffer_|.
    size_t size_;

    // Offset from the start of |buffer_| that marks the front of the queue.
    size_t offset_;

    // Number of bytes stored in the queue.
    int used_;

    DISALLOW_COPY_AND_ASSIGN(ByteQueue);
};

} // namespace media

#endif // MEDIA_BASE_BYTE_QUEUE_H_
