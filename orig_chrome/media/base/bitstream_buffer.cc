// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/bitstream_buffer.h"

namespace media {

BitstreamBuffer::BitstreamBuffer(int32 id,
    base::SharedMemoryHandle handle,
    size_t size)
    : id_(id)
    , handle_(handle)
    , size_(size)
    , presentation_timestamp_(kNoTimestamp())
{
}

BitstreamBuffer::BitstreamBuffer(int32 id,
    base::SharedMemoryHandle handle,
    size_t size,
    base::TimeDelta presentation_timestamp)
    : id_(id)
    , handle_(handle)
    , size_(size)
    , presentation_timestamp_(presentation_timestamp)
{
}

BitstreamBuffer::~BitstreamBuffer() { }

void BitstreamBuffer::SetDecryptConfig(const DecryptConfig& decrypt_config)
{
    key_id_ = decrypt_config.key_id();
    iv_ = decrypt_config.iv();
    subsamples_ = decrypt_config.subsamples();
}

} // namespace media
