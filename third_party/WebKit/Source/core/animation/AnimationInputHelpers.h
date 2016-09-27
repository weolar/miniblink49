// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AnimationInputHelpers_h
#define AnimationInputHelpers_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "wtf/Forward.h"

namespace blink {

class TimingFunction;

class CORE_EXPORT AnimationInputHelpers {
public:
    static CSSPropertyID keyframeAttributeToCSSPropertyID(const String&);
    static PassRefPtr<TimingFunction> parseTimingFunction(const String&);
};

} // namespace blink

#endif // AnimationInputHelpers_h
