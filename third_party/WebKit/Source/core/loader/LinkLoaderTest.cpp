// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/loader/LinkLoader.h"

#include "core/fetch/ResourceFetcher.h"
#include "core/html/LinkRelAttribute.h"
#include "core/loader/LinkLoaderClient.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/network/ResourceLoadPriority.h"

#include <base/macros.h>
#include <gtest/gtest.h>

namespace blink {

class MockLinkLoaderClient : public LinkLoaderClient {
public:
    MockLinkLoaderClient(bool shouldLoad)
        : m_shouldLoad(shouldLoad)
    {
    }

    bool shouldLoadLink() override
    {
        return m_shouldLoad;
    }

    void linkLoaded() override {}
    void linkLoadingErrored() override {}
    void didStartLinkPrerender() override {}
    void didStopLinkPrerender() override {}
    void didSendLoadForLinkPrerender() override {}
    void didSendDOMContentLoadedForLinkPrerender() override {}

private:
    bool m_shouldLoad;
};

TEST(LinkLoaderTest, Preload)
{
    struct TestCase {
        const char* href;
        const char* as;
        const ResourceLoadPriority priority;
        const bool shouldLoad;
    } cases[] = {
        {"data://example.com/cat.jpg", "image", ResourceLoadPriorityVeryLow, true},
        {"data://example.com/cat.jpg", "script", ResourceLoadPriorityMedium, true},
        {"data://example.com/cat.jpg", "stylesheet", ResourceLoadPriorityHigh, true},
        {"data://example.com/cat.jpg", "blabla", ResourceLoadPriorityUnresolved, true},
        {"data://example.com/cat.jpg", "image", ResourceLoadPriorityUnresolved, false},
    };

    // Test the cases with a single header
    for (const auto& testCase : cases) {
        OwnPtr<DummyPageHolder> dummyPageHolder = DummyPageHolder::create(IntSize(500, 500));
        MockLinkLoaderClient loaderClient(testCase.shouldLoad);
        LinkLoader loader(&loaderClient);
        KURL hrefURL = KURL(KURL(), testCase.href);
        loader.loadLink(LinkRelAttribute("preload"),
            AtomicString(),
            String(),
            testCase.as,
            hrefURL,
            dummyPageHolder->document());
        if (testCase.priority == ResourceLoadPriorityUnresolved) {
            ASSERT(!loader.resource());
        } else {
            ASSERT(loader.resource());
            ASSERT_EQ(testCase.priority, loader.resource()->resourceRequest().priority());
        }
    }
}

} // namespace blink
