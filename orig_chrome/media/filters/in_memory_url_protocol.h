// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_IN_MEMORY_URL_PROTOCOL_H_
#define MEDIA_FILTERS_IN_MEMORY_URL_PROTOCOL_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "media/filters/ffmpeg_glue.h"

namespace media {

// Simple FFmpegURLProtocol that reads from a buffer.
// NOTE: This object does not copy the buffer so the
//       buffer pointer passed into the constructor
//       needs to remain valid for the entire lifetime of
//       this object.
class MEDIA_EXPORT InMemoryUrlProtocol : public FFmpegURLProtocol {
public:
    InMemoryUrlProtocol(const uint8* buf, int64 size, bool streaming);
    virtual ~InMemoryUrlProtocol();

    // FFmpegURLProtocol methods.
    int Read(int size, uint8* data) override;
    bool GetPosition(int64* position_out) override;
    bool SetPosition(int64 position) override;
    bool GetSize(int64* size_out) override;
    bool IsStreaming() override;

private:
    const uint8* data_;
    int64 size_;
    int64 position_;
    bool streaming_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(InMemoryUrlProtocol);
};

} // namespace media

#endif // MEDIA_FILTERS_IN_MEMORY_URL_PROTOCOL_H_
