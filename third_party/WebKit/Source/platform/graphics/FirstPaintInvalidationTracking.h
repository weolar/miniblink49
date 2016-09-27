// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FirstPaintInvalidationTracking_h
#define FirstPaintInvalidationTracking_h

#include "platform/PlatformExport.h"

namespace blink {

PLATFORM_EXPORT bool firstPaintInvalidationTrackingEnabled();
PLATFORM_EXPORT void setFirstPaintInvalidationTrackingEnabledForShowPaintRects(bool);

} // namespace blink

#endif // FirstPaintInvalidationTracking_h
