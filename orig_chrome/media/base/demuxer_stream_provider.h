// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_DEMUXER_STREAM_PROVIDER_H_
#define MEDIA_BASE_DEMUXER_STREAM_PROVIDER_H_

#include "media/base/demuxer_stream.h"
#include "media/base/media_export.h"

namespace media {

class MEDIA_EXPORT DemuxerStreamProvider {
public:
    DemuxerStreamProvider();
    virtual ~DemuxerStreamProvider();

    // Returns the first stream of the given stream type (which is not allowed
    // to be DemuxerStream::TEXT), or NULL if that type of stream is not present.
    virtual DemuxerStream* GetStream(DemuxerStream::Type type) = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(DemuxerStreamProvider);
};

} // namespace media

#endif // MEDIA_BASE_DEMUXER_STREAM_PROVIDER_H_
