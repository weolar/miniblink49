// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AppBannerPromptResult_h
#define AppBannerPromptResult_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "public/platform/modules/app_banner/WebAppBannerPromptResult.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ScriptPromiseResolver;

class AppBannerPromptResult final : public GarbageCollectedFinalized<AppBannerPromptResult>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(AppBannerPromptResult);
public:
    static AppBannerPromptResult* take(PassOwnPtr<WebAppBannerPromptResult> webInstance);

    static AppBannerPromptResult* create(const AtomicString& platform, WebAppBannerPromptResult::Outcome outcome)
    {
        return new AppBannerPromptResult(platform, outcome);
    }

    virtual ~AppBannerPromptResult();

    String platform() const { return m_platform; }
    String outcome() const;

    DEFINE_INLINE_VIRTUAL_TRACE() { }

private:
    AppBannerPromptResult(const AtomicString& platform, WebAppBannerPromptResult::Outcome);

    String m_platform;
    WebAppBannerPromptResult::Outcome m_outcome;
};

} // namespace blink

#endif // AppBannerPromptResult_h
