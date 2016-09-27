// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/Timing.h"

namespace blink {

String Timing::fillModeString(FillMode fillMode)
{
    switch (fillMode) {
    case Timing::FillModeNone:
        return "none";
    case Timing::FillModeForwards:
        return "forwards";
    case Timing::FillModeBackwards:
        return "backwards";
    case Timing::FillModeBoth:
        return "both";
    case Timing::FillModeAuto:
        return "auto";
    }
    ASSERT_NOT_REACHED();
    return "none";
}

String Timing::playbackDirectionString(PlaybackDirection playbackDirection)
{
    switch (playbackDirection) {
    case Timing::PlaybackDirectionNormal:
        return "normal";
    case Timing::PlaybackDirectionReverse:
        return "reverse";
    case Timing::PlaybackDirectionAlternate:
        return "alternate";
    case Timing::PlaybackDirectionAlternateReverse:
        return "alternate-reverse";
    }
    ASSERT_NOT_REACHED();
    return "normal";
}


} // namespace
