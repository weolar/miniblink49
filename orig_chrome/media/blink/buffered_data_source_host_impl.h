// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_BUFFERED_DATA_SOURCE_HOST_IMPL_H_
#define MEDIA_BLINK_BUFFERED_DATA_SOURCE_HOST_IMPL_H_

#include "base/time/time.h"
#include "media/base/ranges.h"
#include "media/blink/buffered_data_source.h"
#include "media/blink/media_blink_export.h"

namespace media {

// Provides an implementation of BufferedDataSourceHost that translates the
// buffered byte ranges into estimated time ranges.
class MEDIA_BLINK_EXPORT BufferedDataSourceHostImpl
    : public BufferedDataSourceHost {
public:
    BufferedDataSourceHostImpl();
    ~BufferedDataSourceHostImpl() override;

    // BufferedDataSourceHost implementation.
    void SetTotalBytes(int64 total_bytes) override;
    void AddBufferedByteRange(int64 start, int64 end) override;

    // Translate the byte ranges to time ranges and append them to the list.
    // TODO(sandersd): This is a confusing name, find something better.
    void AddBufferedTimeRanges(
        Ranges<base::TimeDelta>* buffered_time_ranges,
        base::TimeDelta media_duration) const;

    bool DidLoadingProgress();

private:
    // Total size of the data source.
    int64 total_bytes_;

    // List of buffered byte ranges for estimating buffered time.
    Ranges<int64> buffered_byte_ranges_;

    // True when AddBufferedByteRange() has been called more recently than
    // DidLoadingProgress().
    bool did_loading_progress_;

    DISALLOW_COPY_AND_ASSIGN(BufferedDataSourceHostImpl);
};

} // namespace media

#endif // MEDIA_BLINK_BUFFERED_DATA_SOURCE_HOST_IMPL_H_
