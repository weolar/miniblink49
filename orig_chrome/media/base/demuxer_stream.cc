// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/demuxer_stream.h"

namespace media {

DemuxerStream::~DemuxerStream() { }

// Most DemuxerStream implementations don't specify liveness. Returns unknown
// liveness by default.
DemuxerStream::Liveness DemuxerStream::liveness() const
{
    return DemuxerStream::LIVENESS_UNKNOWN;
}

// Most DemuxerStream implementations don't need to convert bit stream.
// Do nothing by default.
void DemuxerStream::EnableBitstreamConverter() { }

} // namespace media
