// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_BLOCKING_URL_PROTOCOL_H_
#define MEDIA_FILTERS_BLOCKING_URL_PROTOCOL_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/synchronization/waitable_event.h"
#include "media/filters/ffmpeg_glue.h"

namespace media {

class DataSource;

// An implementation of FFmpegURLProtocol that blocks until the underlying
// asynchronous DataSource::Read() operation completes.
class MEDIA_EXPORT BlockingUrlProtocol : public FFmpegURLProtocol {
public:
    // Implements FFmpegURLProtocol using the given |data_source|. |error_cb| is
    // fired any time DataSource::Read() returns an error.
    //
    // TODO(scherkus): After all blocking operations are isolated on a separate
    // thread we should be able to eliminate |error_cb|.
    BlockingUrlProtocol(DataSource* data_source, const base::Closure& error_cb);
    virtual ~BlockingUrlProtocol();

    // Aborts any pending reads by returning a read error. After this method
    // returns all subsequent calls to Read() will immediately fail.
    void Abort();

    // FFmpegURLProtocol implementation.
    int Read(int size, uint8* data) override;
    bool GetPosition(int64* position_out) override;
    bool SetPosition(int64 position) override;
    bool GetSize(int64* size_out) override;
    bool IsStreaming() override;

private:
    // Sets |last_read_bytes_| and signals the blocked thread that the read
    // has completed.
    void SignalReadCompleted(int size);

    DataSource* data_source_;
    base::Closure error_cb_;

    // Used to unblock the thread during shutdown and when reads complete.
    base::WaitableEvent aborted_;
    base::WaitableEvent read_complete_;

    // Cached number of bytes last read from the data source.
    int last_read_bytes_;

    // Cached position within the data source.
    int64 read_position_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(BlockingUrlProtocol);
};

} // namespace media

#endif // MEDIA_FILTERS_BLOCKING_URL_PROTOCOL_H_
