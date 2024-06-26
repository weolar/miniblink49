// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_ID_PROVIDER_H_
#define CC_ANIMATION_ANIMATION_ID_PROVIDER_H_

#include "base/basictypes.h"
#include "cc/base/cc_export.h"

namespace cc {

class CC_EXPORT AnimationIdProvider {
public:
    // These functions each return monotonically increasing values.
    static int NextAnimationId();
    static int NextGroupId();
    static int NextTimelineId();
    static int NextPlayerId();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(AnimationIdProvider);
};

} // namespace cc

#endif // CC_ANIMATION_ANIMATION_ID_PROVIDER_H_
