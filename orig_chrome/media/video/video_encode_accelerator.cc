// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/video/video_encode_accelerator.h"

namespace media {

VideoEncodeAccelerator::~VideoEncodeAccelerator() { }

VideoEncodeAccelerator::SupportedProfile::SupportedProfile()
    : profile(media::VIDEO_CODEC_PROFILE_UNKNOWN)
    , max_framerate_numerator(0)
    , max_framerate_denominator(0)
{
}

VideoEncodeAccelerator::SupportedProfile::~SupportedProfile()
{
}

} // namespace media

namespace base {

void DefaultDeleter<media::VideoEncodeAccelerator>::operator()(
    void* video_encode_accelerator) const
{
    static_cast<media::VideoEncodeAccelerator*>(video_encode_accelerator)->Destroy();
}

} // namespace base
