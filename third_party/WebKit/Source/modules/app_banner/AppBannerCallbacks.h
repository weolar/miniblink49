// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AppBannerCallbacks_h
#define AppBannerCallbacks_h

#include "modules/app_banner/BeforeInstallPromptEvent.h"
#include "public/platform/modules/app_banner/WebAppBannerClient.h"

namespace blink {

class AppBannerCallbacks : public WebAppBannerCallbacks {
public:
    explicit AppBannerCallbacks(UserChoiceProperty*);
    ~AppBannerCallbacks() override = default;

    // WebAppBannerCallbacks overrides.
    void onSuccess(WebAppBannerPromptResult*) override;

private:
    Persistent<UserChoiceProperty> m_userChoice;
};

} // namespace blink

#endif // AppBannerCallbacks_h
