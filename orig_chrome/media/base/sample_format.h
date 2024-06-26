// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_SAMPLE_FORMAT_H
#define MEDIA_BASE_SAMPLE_FORMAT_H

#include "media/base/media_export.h"

namespace media {

enum SampleFormat {
    // These values are histogrammed over time; do not change their ordinal
    // values.  When deleting a sample format replace it with a dummy value; when
    // adding a sample format, do so at the bottom before kSampleFormatMax, and
    // update the value of kSampleFormatMax.
    kUnknownSampleFormat = 0,
    kSampleFormatU8, // Unsigned 8-bit w/ bias of 128.
    kSampleFormatS16, // Signed 16-bit.
    kSampleFormatS32, // Signed 32-bit.
    kSampleFormatF32, // Float 32-bit.
    kSampleFormatPlanarS16, // Signed 16-bit planar.
    kSampleFormatPlanarF32, // Float 32-bit planar.
    kSampleFormatPlanarS32, // Signed 32-bit planar.

    // Must always be equal to largest value ever logged.
    kSampleFormatMax = kSampleFormatPlanarS32,
};

// Returns the number of bytes used per channel for the specified
// |sample_format|.
MEDIA_EXPORT int SampleFormatToBytesPerChannel(SampleFormat sample_format);

// Returns the name of the sample format as a string
MEDIA_EXPORT const char* SampleFormatToString(SampleFormat sample_format);

} // namespace media

#endif // MEDIA_BASE_SAMPLE_FORMAT_H
