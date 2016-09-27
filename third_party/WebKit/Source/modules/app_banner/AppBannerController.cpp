// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/app_banner/AppBannerController.h"

#include "core/EventTypeNames.h"
#include "core/dom/Document.h"
#include "core/frame/DOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "modules/app_banner/BeforeInstallPromptEvent.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/app_banner/WebAppBannerClient.h"
#include "public/platform/modules/app_banner/WebAppBannerPromptReply.h"

namespace blink {

// static
void AppBannerController::willShowInstallBannerPrompt(int requestId, WebAppBannerClient* client, LocalFrame* frame, const WebVector<WebString>& platforms, WebAppBannerPromptReply* reply)
{
    ASSERT(RuntimeEnabledFeatures::appBannerEnabled());

    Vector<String> wtfPlatforms;
    for (const WebString& platform : platforms)
        wtfPlatforms.append(platform);

    // dispatchEvent() returns whether the default behavior can happen. In other
    // words, it returns false if preventDefault() was called.
    *reply = frame->domWindow()->dispatchEvent(BeforeInstallPromptEvent::create(EventTypeNames::beforeinstallprompt, frame->document(), wtfPlatforms, requestId, client))
        ? WebAppBannerPromptReply::None : WebAppBannerPromptReply::Cancel;
}

} // namespace blink
