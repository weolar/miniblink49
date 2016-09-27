// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AppBannerController_h
#define AppBannerController_h

#include "modules/ModulesExport.h"
#include "wtf/Noncopyable.h"

namespace blink {

enum class WebAppBannerPromptReply;
class LocalFrame;
class WebAppBannerClient;
class WebString;
template <typename T> class WebVector;

// FIXME: unless userChoice ends up implemented, this class should not exist and
// a regular static method could be used instead.
class MODULES_EXPORT AppBannerController final {
    WTF_MAKE_NONCOPYABLE(AppBannerController);
public:
    static void willShowInstallBannerPrompt(int requestId, WebAppBannerClient*, LocalFrame*, const WebVector<WebString>& platforms, WebAppBannerPromptReply*);

private:
    AppBannerController() = delete;
    ~AppBannerController() = delete;
};

} // namespace blink

#endif // AppBannerController_h
