// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_WEBM_WEBM_VIDEO_CLIENT_H_
#define MEDIA_FORMATS_WEBM_WEBM_VIDEO_CLIENT_H_

#include <string>
#include <vector>

#include "media/base/media_log.h"
#include "media/formats/webm/webm_parser.h"

namespace media {
class VideoDecoderConfig;

// Helper class used to parse a Video element inside a TrackEntry element.
class WebMVideoClient : public WebMParserClient {
public:
    explicit WebMVideoClient(const scoped_refptr<MediaLog>& media_log);
    ~WebMVideoClient() override;

    // Reset this object's state so it can process a new video track element.
    void Reset();

    // Initialize |config| with the data in |codec_id|, |codec_private|,
    // |is_encrypted| and the fields parsed from the last video track element this
    // object was used to parse.
    // Returns true if |config| was successfully initialized.
    // Returns false if there was unexpected values in the provided parameters or
    // video track element fields. The contents of |config| are undefined in this
    // case and should not be relied upon.
    bool InitializeConfig(const std::string& codec_id,
        const std::vector<uint8>& codec_private,
        bool is_encrypted,
        VideoDecoderConfig* config);

private:
    // WebMParserClient implementation.
    bool OnUInt(int id, int64 val) override;
    bool OnBinary(int id, const uint8* data, int size) override;
    bool OnFloat(int id, double val) override;

    scoped_refptr<MediaLog> media_log_;
    int64 pixel_width_;
    int64 pixel_height_;
    int64 crop_bottom_;
    int64 crop_top_;
    int64 crop_left_;
    int64 crop_right_;
    int64 display_width_;
    int64 display_height_;
    int64 display_unit_;
    int64 alpha_mode_;

    DISALLOW_COPY_AND_ASSIGN(WebMVideoClient);
};

} // namespace media

#endif // MEDIA_FORMATS_WEBM_WEBM_VIDEO_CLIENT_H_
