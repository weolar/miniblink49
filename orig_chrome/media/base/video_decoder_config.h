// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_DECODER_CONFIG_H_
#define MEDIA_BASE_VIDEO_DECODER_CONFIG_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "media/base/media_export.h"
#include "media/base/video_codecs.h"
#include "media/base/video_types.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace media {

MEDIA_EXPORT VideoCodec
VideoCodecProfileToVideoCodec(VideoCodecProfile profile);

class MEDIA_EXPORT VideoDecoderConfig {
public:
    // Constructs an uninitialized object. Clients should call Initialize() with
    // appropriate values before using.
    VideoDecoderConfig();

    // Constructs an initialized object. It is acceptable to pass in NULL for
    // |extra_data|, otherwise the memory is copied.
    VideoDecoderConfig(VideoCodec codec,
        VideoCodecProfile profile,
        VideoPixelFormat format,
        ColorSpace color_space,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        const std::vector<uint8_t>& extra_data,
        bool is_encrypted);

    ~VideoDecoderConfig();

    // Resets the internal state of this object.
    void Initialize(VideoCodec codec,
        VideoCodecProfile profile,
        VideoPixelFormat format,
        ColorSpace color_space,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        const std::vector<uint8_t>& extra_data,
        bool is_encrypted);

    // Returns true if this object has appropriate configuration values, false
    // otherwise.
    bool IsValidConfig() const;

    // Returns true if all fields in |config| match this config.
    // Note: The contents of |extra_data_| are compared not the raw pointers.
    bool Matches(const VideoDecoderConfig& config) const;

    // Returns a human-readable string describing |*this|.  For debugging & test
    // output only.
    std::string AsHumanReadableString() const;

    std::string GetHumanReadableCodecName() const;

    VideoCodec codec() const { return codec_; }
    VideoCodecProfile profile() const { return profile_; }

    // Video format used to determine YUV buffer sizes.
    VideoPixelFormat format() const { return format_; }

    // The default color space of the decoded frames. Decoders should output
    // frames tagged with this color space unless they find a different value in
    // the bitstream.
    ColorSpace color_space() const { return color_space_; }

    // Width and height of video frame immediately post-decode. Not all pixels
    // in this region are valid.
    gfx::Size coded_size() const { return coded_size_; }

    // Region of |coded_size_| that is visible.
    gfx::Rect visible_rect() const { return visible_rect_; }

    // Final visible width and height of a video frame with aspect ratio taken
    // into account.
    gfx::Size natural_size() const { return natural_size_; }

    // Optional byte data required to initialize video decoders, such as H.264
    // AAVC data.
    const std::vector<uint8_t>& extra_data() const { return extra_data_; }

    // Whether the video stream is potentially encrypted.
    // Note that in a potentially encrypted video stream, individual buffers
    // can be encrypted or not encrypted.
    bool is_encrypted() const { return is_encrypted_; }

private:
    VideoCodec codec_;
    VideoCodecProfile profile_;

    VideoPixelFormat format_;
    ColorSpace color_space_;

    gfx::Size coded_size_;
    gfx::Rect visible_rect_;
    gfx::Size natural_size_;

    std::vector<uint8_t> extra_data_;

    bool is_encrypted_;

    // Not using DISALLOW_COPY_AND_ASSIGN here intentionally to allow the compiler
    // generated copy constructor and assignment operator. Since the extra data is
    // typically small, the performance impact is minimal.
};

} // namespace media

#endif // MEDIA_BASE_VIDEO_DECODER_CONFIG_H_
