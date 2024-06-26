// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_TIMESTAMP_CONSTANTS_H_
#define MEDIA_BASE_TIMESTAMP_CONSTANTS_H_

#include "base/time/time.h"
#include "media/base/media_export.h"

namespace media {

// Indicates an invalid or missing timestamp.
MEDIA_EXPORT inline base::TimeDelta kNoTimestamp()
{
    return base::TimeDelta::FromMicroseconds(kint64min);
}

// Represents an infinite stream duration.
MEDIA_EXPORT inline base::TimeDelta kInfiniteDuration()
{
    return base::TimeDelta::Max();
}

} // namespace media

#endif // MEDIA_BASE_TIMESTAMP_CONSTANTS_H_
