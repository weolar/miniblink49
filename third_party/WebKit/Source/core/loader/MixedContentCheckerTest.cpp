// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/loader/MixedContentChecker.h"

#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/RefPtr.h"

#include <base/macros.h>
#include <gtest/gtest.h>

namespace blink {

TEST(MixedContentCheckerTest, IsMixedContent)
{
    struct TestCase {
        const char* origin;
        const char* target;
        bool expectation;
    } cases[] = {
        {"http://example.com/foo", "http://example.com/foo", false},
        {"http://example.com/foo", "https://example.com/foo", false},
        {"https://example.com/foo", "https://example.com/foo", false},
        {"https://example.com/foo", "wss://example.com/foo", false},
        {"https://example.com/foo", "http://example.com/foo", true},
        {"https://example.com/foo", "http://google.com/foo", true},
        {"https://example.com/foo", "ws://example.com/foo", true},
        {"https://example.com/foo", "ws://google.com/foo", true},
    };

    for (size_t i = 0; i < arraysize(cases); ++i) {
        const char* origin = cases[i].origin;
        const char* target = cases[i].target;
        bool expectation = cases[i].expectation;

        KURL originUrl(KURL(), origin);
        RefPtr<SecurityOrigin> securityOrigin(SecurityOrigin::create(originUrl));
        KURL targetUrl(KURL(), target);
        EXPECT_EQ(expectation, MixedContentChecker::isMixedContent(securityOrigin.get(), targetUrl)) << "Origin: " << origin << ", Target: " << target << ", Expectation: " << expectation;
    }
}

} // namespace blink
