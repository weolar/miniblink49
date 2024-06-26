// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Contains code that should be used for initializing, or querying the state
// of the media library as a whole.

#ifndef MEDIA_BASE_MEDIA_H_
#define MEDIA_BASE_MEDIA_H_

#include "media/base/media_export.h"

namespace base {
class FilePath;
}

namespace media {

// Initializes media libraries (e.g. ffmpeg) as well as CPU specific media
// features.
MEDIA_EXPORT void InitializeMediaLibrary();

} // namespace media

#endif // MEDIA_BASE_MEDIA_H_
