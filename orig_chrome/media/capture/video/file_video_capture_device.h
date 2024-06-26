// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_H_
#define MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_H_

#include <string>

#include "base/files/file.h"
#include "base/files/memory_mapped_file.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread.h"
#include "base/threading/thread_checker.h"
#include "media/capture/video/video_capture_device.h"

namespace media {

class VideoFileParser;

// Implementation of a VideoCaptureDevice class that reads from a file. Used for
// testing the video capture pipeline when no real hardware is available. The
// supported file formats are YUV4MPEG2 (a.k.a. Y4M) and MJPEG/JPEG. YUV4MPEG2
// is a minimal container with a series of uncompressed video only frames, see
// the link http://wiki.multimedia.cx/index.php?title=YUV4MPEG2 for more
// information on the file format. Several restrictions and notes apply, see the
// implementation file.
// Example Y4M videos can be found in http://media.xiph.org/video/derf.
// Example MJPEG videos can be found in media/data/test/bear.mjpeg.
// Restrictions: Y4M videos should have .y4m file extension and MJPEG videos
// should have .mjpeg file extension.
class MEDIA_EXPORT FileVideoCaptureDevice : public VideoCaptureDevice {
public:
    // Reads and parses the header of a |file_path|, returning the collected
    // pixel format in |video_format|. Returns true on file parsed successfully,
    // or false.
    // Restrictions: Only trivial Y4M per-frame headers and MJPEG are supported.
    static bool GetVideoCaptureFormat(const base::FilePath& file_path,
        media::VideoCaptureFormat* video_format);

    // Constructor of the class, with a fully qualified file path as input, which
    // represents the Y4M or MJPEG file to stream repeatedly.
    explicit FileVideoCaptureDevice(const base::FilePath& file_path);

    // VideoCaptureDevice implementation, class methods.
    ~FileVideoCaptureDevice() override;
    void AllocateAndStart(const VideoCaptureParams& params,
        scoped_ptr<VideoCaptureDevice::Client> client) override;
    void StopAndDeAllocate() override;

private:
    // Opens a given file |file_path| for reading, and stores collected format
    // information in |video_format|. Returns the parsed file to the
    // caller, who is responsible for closing it.
    static scoped_ptr<VideoFileParser> GetVideoFileParser(
        const base::FilePath& file_path,
        media::VideoCaptureFormat* video_format);

    // Called on the |capture_thread_|.
    void OnAllocateAndStart(const VideoCaptureParams& params,
        scoped_ptr<Client> client);
    void OnStopAndDeAllocate();
    const uint8_t* GetNextFrame();
    void OnCaptureTask();

    // |thread_checker_| is used to check that destructor, AllocateAndStart() and
    // StopAndDeAllocate() are called in the correct thread that owns the object.
    base::ThreadChecker thread_checker_;

    // |capture_thread_| is used for internal operations via posting tasks to it.
    // It is active between OnAllocateAndStart() and OnStopAndDeAllocate().
    base::Thread capture_thread_;
    // The following members belong to |capture_thread_|.
    scoped_ptr<VideoCaptureDevice::Client> client_;
    const base::FilePath file_path_;
    scoped_ptr<VideoFileParser> file_parser_;
    VideoCaptureFormat capture_format_;
    // Target time for the next frame.
    base::TimeTicks next_frame_time_;

    DISALLOW_COPY_AND_ASSIGN(FileVideoCaptureDevice);
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_FILE_VIDEO_CAPTURE_DEVICE_H_
