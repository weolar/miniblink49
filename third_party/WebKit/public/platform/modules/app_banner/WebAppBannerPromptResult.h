// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebAppBannerPromptResult_h
#define WebAppBannerPromptResult_h

#include "public/platform/WebString.h"

namespace blink {

struct WebAppBannerPromptResult {
    enum class Outcome { Accepted, Dismissed };

    WebAppBannerPromptResult(const WebString& platform, Outcome outcome)
        : platform(platform)
        , outcome(outcome)
    {
    }

    WebString platform;
    Outcome outcome;
};

} // namespace blink

#endif // WebAppBannerPromptResult_h
