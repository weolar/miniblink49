// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_CODECS_H_
#define MEDIA_BASE_VIDEO_CODECS_H_

namespace media {

enum VideoCodec {
    // These values are histogrammed over time; do not change their ordinal
    // values.  When deleting a codec replace it with a dummy value; when adding a
    // codec, do so at the bottom (and update kVideoCodecMax).
    kUnknownVideoCodec = 0,
    kCodecH264,
    kCodecVC1,
    kCodecMPEG2,
    kCodecMPEG4,
    kCodecTheora,
    kCodecVP8,
    kCodecVP9,
    kCodecHEVC,
    // DO NOT ADD RANDOM VIDEO CODECS!
    //
    // The only acceptable time to add a new codec is if there is production code
    // that uses said codec in the same CL.

    kVideoCodecMax = kCodecHEVC // Must equal the last "real" codec above.
};

// Video stream profile.  This *must* match PP_VideoDecoder_Profile.
// (enforced in webkit/plugins/ppapi/ppb_video_decoder_impl.cc) and
// gpu::VideoCodecProfile.
enum VideoCodecProfile {
    // Keep the values in this enum unique, as they imply format (h.264 vs. VP8,
    // for example), and keep the values for a particular format grouped
    // together for clarity.
    VIDEO_CODEC_PROFILE_UNKNOWN = -1,
    VIDEO_CODEC_PROFILE_MIN = VIDEO_CODEC_PROFILE_UNKNOWN,
    H264PROFILE_MIN = 0,
    H264PROFILE_BASELINE = H264PROFILE_MIN,
    H264PROFILE_MAIN = 1,
    H264PROFILE_EXTENDED = 2,
    H264PROFILE_HIGH = 3,
    H264PROFILE_HIGH10PROFILE = 4,
    H264PROFILE_HIGH422PROFILE = 5,
    H264PROFILE_HIGH444PREDICTIVEPROFILE = 6,
    H264PROFILE_SCALABLEBASELINE = 7,
    H264PROFILE_SCALABLEHIGH = 8,
    H264PROFILE_STEREOHIGH = 9,
    H264PROFILE_MULTIVIEWHIGH = 10,
    H264PROFILE_MAX = H264PROFILE_MULTIVIEWHIGH,
    VP8PROFILE_MIN = 11,
    VP8PROFILE_ANY = VP8PROFILE_MIN,
    VP8PROFILE_MAX = VP8PROFILE_ANY,
    VP9PROFILE_MIN = 12,
    VP9PROFILE_ANY = VP9PROFILE_MIN,
    VP9PROFILE_MAX = VP9PROFILE_ANY,
    VIDEO_CODEC_PROFILE_MAX = VP9PROFILE_MAX,
};

} // namespace media

#endif // MEDIA_BASE_VIDEO_CODECS_H_
