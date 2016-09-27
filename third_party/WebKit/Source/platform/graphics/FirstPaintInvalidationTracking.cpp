// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/FirstPaintInvalidationTracking.h"

#include "platform/TraceEvent.h"

namespace blink {

static bool showPaintRectsEnabled = false;

bool firstPaintInvalidationTrackingEnabled()
{
    if (showPaintRectsEnabled)
        return true;

    bool isTracingEnabled = false;
    TRACE_EVENT_CATEGORY_GROUP_ENABLED(TRACE_DISABLED_BY_DEFAULT("blink.invalidation"), &isTracingEnabled);
    return isTracingEnabled;
}

void setFirstPaintInvalidationTrackingEnabledForShowPaintRects(bool b)
{
    showPaintRectsEnabled = b;
}

}
