// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/websourcebuffer_impl.h"

#include <cmath>
#include <limits>

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "media/base/timestamp_constants.h"
#include "media/filters/chunk_demuxer.h"
#include "third_party/WebKit/public/platform/WebSourceBufferClient.h"

namespace media {

static base::TimeDelta DoubleToTimeDelta(double time)
{
    DCHECK(!std::isnan(time));
    DCHECK_NE(time, -std::numeric_limits<double>::infinity());

    if (time == std::numeric_limits<double>::infinity())
        return kInfiniteDuration();

    // Don't use base::TimeDelta::Max() here, as we want the largest finite time
    // delta.
    base::TimeDelta max_time = base::TimeDelta::FromInternalValue(kint64max - 1);
    double max_time_in_seconds = max_time.InSecondsF();

    if (time >= max_time_in_seconds)
        return max_time;

    return base::TimeDelta::FromMicroseconds(time * base::Time::kMicrosecondsPerSecond);
}

WebSourceBufferImpl::WebSourceBufferImpl(
    const std::string& id, ChunkDemuxer* demuxer)
    : id_(id)
    , demuxer_(demuxer)
    , client_(NULL)
    , append_window_end_(kInfiniteDuration())
{
    DCHECK(demuxer_);
}

#if 0
static bool SaveMediaFile(const std::string& url, const unsigned char* buffer, unsigned int size)
{
    HANDLE hFile = NULL;
    bool   bRet = false;

    hFile = CreateFileA(url.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return false;

    DWORD numberOfBytesWritten = 0;
    ::WriteFile(hFile, buffer, size, &numberOfBytesWritten, NULL);
    ::CloseHandle(hFile);
    bRet = true;

    return bRet;
}
#endif

WebSourceBufferImpl::~WebSourceBufferImpl()
{
    DCHECK(!demuxer_) << "Object destroyed w/o removedFromMediaSource() call";
    DCHECK(!client_);
#if 0
    char* output = (char*)malloc(0x300);
    sprintf(output, "P:\\mediaio\\testdata\\data_%p_%d.mp4", this, buffer_test_count_);
    SaveMediaFile(output, (const unsigned char*)buffer_test_.data(), buffer_test_.size());
    free(output);
#endif
}

void WebSourceBufferImpl::setClient(blink::WebSourceBufferClient* client)
{
    DCHECK(client);
    DCHECK(!client_);
    client_ = client;
}

bool WebSourceBufferImpl::setMode(WebSourceBuffer::AppendMode mode)
{
    if (demuxer_->IsParsingMediaSegment(id_))
        return false;

    switch (mode) {
    case WebSourceBuffer::AppendModeSegments:
        demuxer_->SetSequenceMode(id_, false);
        return true;
    case WebSourceBuffer::AppendModeSequence:
        demuxer_->SetSequenceMode(id_, true);
        return true;
    }

    NOTREACHED();
    return false;
}

blink::WebTimeRanges WebSourceBufferImpl::buffered()
{
    Ranges<base::TimeDelta> ranges = demuxer_->GetBufferedRanges(id_);
    blink::WebTimeRanges result(ranges.size());
    for (size_t i = 0; i < ranges.size(); i++) {
        result[i].start = ranges.start(i).InSecondsF();
        result[i].end = ranges.end(i).InSecondsF();
    }
    return result;
}

bool WebSourceBufferImpl::evictCodedFrames(double currentPlaybackTime, size_t newDataSize)
{
    return demuxer_->EvictCodedFrames(id_, base::TimeDelta::FromSecondsD(currentPlaybackTime), newDataSize);
}

void WebSourceBufferImpl::append(const unsigned char* data, unsigned length, double* timestamp_offset)
{
#if 0
    if (buffer_test_.size() > 500 * 1024 * 1024) {
        char* output = (char*)malloc(0x300);
        sprintf(output, "P:\\mediaio\\testdata\\data_%p_%d.mp4", this, buffer_test_count_);
        SaveMediaFile(output, (const unsigned char *)buffer_test_.data(), buffer_test_.size());
        free(output);
        buffer_test_count_++;
        buffer_test_.clear();
    }

    if (0 != length) {
        size_t old_size = buffer_test_.size();
        buffer_test_.resize(buffer_test_.size() + length);
        memcpy(buffer_test_.data() + old_size, data, length);
    }
#endif

    base::TimeDelta old_offset = timestamp_offset_;
    demuxer_->AppendData(id_, data, length, append_window_start_, append_window_end_, &timestamp_offset_,
        base::Bind(&WebSourceBufferImpl::InitSegmentReceived, base::Unretained(this)));

    // Coded frame processing may update the timestamp offset. If the caller
    // provides a non-NULL |timestamp_offset| and frame processing changes the
    // timestamp offset, report the new offset to the caller. Do not update the
    // caller's offset otherwise, to preserve any pre-existing value that may have
    // more than microsecond precision.
    if (timestamp_offset && old_offset != timestamp_offset_)
        *timestamp_offset = timestamp_offset_.InSecondsF();
}

void WebSourceBufferImpl::resetParserState()
{
    demuxer_->ResetParserState(id_, append_window_start_, append_window_end_, &timestamp_offset_);

    // TODO(wolenetz): resetParserState should be able to modify the caller
    // timestamp offset (just like WebSourceBufferImpl::append).
    // See http://crbug.com/370229 for further details.
}

void WebSourceBufferImpl::abort()
{
    resetParserState();
}

void WebSourceBufferImpl::remove(double start, double end)
{
    DCHECK_GE(start, 0);
    DCHECK_GE(end, 0);
    demuxer_->Remove(id_, DoubleToTimeDelta(start), DoubleToTimeDelta(end));
}

bool WebSourceBufferImpl::setTimestampOffset(double offset)
{
    if (demuxer_->IsParsingMediaSegment(id_))
        return false;

    timestamp_offset_ = DoubleToTimeDelta(offset);

    // http://www.w3.org/TR/media-source/#widl-SourceBuffer-timestampOffset
    // Step 6: If the mode attribute equals "sequence", then set the group start
    // timestamp to new timestamp offset.
    demuxer_->SetGroupStartTimestampIfInSequenceMode(id_, timestamp_offset_);
    return true;
}

void WebSourceBufferImpl::setAppendWindowStart(double start)
{
    DCHECK_GE(start, 0);
    append_window_start_ = DoubleToTimeDelta(start);
}

void WebSourceBufferImpl::setAppendWindowEnd(double end)
{
    DCHECK_GE(end, 0);
    append_window_end_ = DoubleToTimeDelta(end);
}

void WebSourceBufferImpl::removedFromMediaSource()
{
    demuxer_->RemoveId(id_);
    demuxer_ = NULL;
    client_ = NULL;
}

void WebSourceBufferImpl::InitSegmentReceived()
{
    DVLOG(1) << __FUNCTION__;
    client_->initializationSegmentReceived();
}

} // namespace media
