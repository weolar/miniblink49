// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/source_buffer_platform.h"

namespace media {

// 12MB: approximately 5 minutes of 320Kbps content.
// 150MB: approximately 5 minutes of 4Mbps content.
const size_t kSourceBufferAudioMemoryLimit = 12 * 1024 * 1024;
const size_t kSourceBufferVideoMemoryLimit = 150 * 1024 * 1024 / 2;

} // namespace media
