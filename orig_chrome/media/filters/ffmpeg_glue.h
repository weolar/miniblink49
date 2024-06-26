// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// FFmpegGlue is an interface between FFmpeg and Chrome used to proxy FFmpeg's
// read and seek requests to Chrome's internal data structures.  The glue works
// through the AVIO interface provided by FFmpeg.
//
// AVIO works through a special AVIOContext created through avio_alloc_context()
// which is attached to the AVFormatContext used for demuxing.  The AVIO context
// is initialized with read and seek methods which FFmpeg calls when necessary.
//
// During OpenContext() FFmpegGlue will tell FFmpeg to use Chrome's AVIO context
// by passing NULL in for the filename parameter to avformat_open_input().  All
// FFmpeg operations using the configured AVFormatContext will then redirect
// reads and seeks through the glue.
//
// The glue in turn processes those read and seek requests using the
// FFmpegURLProtocol provided during construction.
//
// FFmpegGlue is also responsible for initializing FFmpeg, which is done once
// per process.  Initialization includes: turning off log messages, registering
// a lock manager, and finally registering all demuxers and codecs.

#ifndef MEDIA_FILTERS_FFMPEG_GLUE_H_
#define MEDIA_FILTERS_FFMPEG_GLUE_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "media/base/media_export.h"
#include "media/ffmpeg/ffmpeg_deleters.h"

struct AVFormatContext;
struct AVIOContext;

namespace media {

class MEDIA_EXPORT FFmpegURLProtocol {
public:
    // Read the given amount of bytes into data, returns the number of bytes read
    // if successful, kReadError otherwise.
    virtual int Read(int size, uint8* data) = 0;

    // Returns true and the current file position for this file, false if the
    // file position could not be retrieved.
    virtual bool GetPosition(int64* position_out) = 0;

    // Returns true if the file position could be set, false otherwise.
    virtual bool SetPosition(int64 position) = 0;

    // Returns true and the file size, false if the file size could not be
    // retrieved.
    virtual bool GetSize(int64* size_out) = 0;

    // Returns false if this protocol supports random seeking.
    virtual bool IsStreaming() = 0;
};

class MEDIA_EXPORT FFmpegGlue {
public:
    static void InitializeFFmpeg();

    // See file documentation for usage.  |protocol| must outlive FFmpegGlue.
    explicit FFmpegGlue(FFmpegURLProtocol* protocol);
    ~FFmpegGlue();

    // Opens an AVFormatContext specially prepared to process reads and seeks
    // through the FFmpegURLProtocol provided during construction.
    bool OpenContext();
    AVFormatContext* format_context() { return format_context_; }

private:
    bool open_called_;
    AVFormatContext* format_context_;
    scoped_ptr<AVIOContext, ScopedPtrAVFree> avio_context_;

    DISALLOW_COPY_AND_ASSIGN(FFmpegGlue);
};

} // namespace media

#endif // MEDIA_FILTERS_FFMPEG_GLUE_H_
