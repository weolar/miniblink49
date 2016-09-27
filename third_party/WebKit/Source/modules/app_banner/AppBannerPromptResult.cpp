// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/app_banner/AppBannerPromptResult.h"

#include "public/platform/modules/app_banner/WebAppBannerPromptResult.h"

namespace blink {

// static
AppBannerPromptResult* AppBannerPromptResult::take(PassOwnPtr<WebAppBannerPromptResult> webInstance)
{
    return AppBannerPromptResult::create(webInstance->platform, webInstance->outcome);
}

AppBannerPromptResult::~AppBannerPromptResult()
{
}

String AppBannerPromptResult::outcome() const
{
    switch (m_outcome) {
    case WebAppBannerPromptResult::Outcome::Accepted:
        return "accepted";

    case WebAppBannerPromptResult::Outcome::Dismissed:
        return "dismissed";
    }

    ASSERT_NOT_REACHED();
    return "";
}

AppBannerPromptResult::AppBannerPromptResult(const AtomicString& platform, WebAppBannerPromptResult::Outcome outcome)
    : m_platform(platform)
    , m_outcome(outcome)
{
}

} // namespace blink
