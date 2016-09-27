// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebHelperPlugin.h"

#include "platform/testing/UnitTestHelpers.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebLocalFrame.h"
#include "web/tests/FakeWebPlugin.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

namespace {

class FakePlaceholderWebPlugin : public FakeWebPlugin {
public:
    FakePlaceholderWebPlugin(WebFrame* frame, const WebPluginParams& params)
        : FakeWebPlugin(frame, params)
    {
    }
    ~FakePlaceholderWebPlugin() override {}

    bool isPlaceholder() override { return true; }
};

class WebHelperPluginFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    WebHelperPluginFrameClient() : m_createPlaceholder(false) {}
    ~WebHelperPluginFrameClient() override {}

    WebPlugin* createPlugin(WebLocalFrame* frame, const WebPluginParams& params) override
    {
        return m_createPlaceholder ? new FakePlaceholderWebPlugin(frame, params) : new FakeWebPlugin(frame, params);
    }

    void setCreatePlaceholder(bool createPlaceholder) { m_createPlaceholder = createPlaceholder; }

private:
    bool m_createPlaceholder;
};

class WebHelperPluginTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        m_helper.initializeAndLoad("about:blank", false, &m_frameClient);
    }


    void destroyHelperPlugin()
    {
        m_plugin.clear();
        // WebHelperPlugin is destroyed by a task posted to the message loop.
        testing::runPendingTasks();
    }

    FrameTestHelpers::WebViewHelper m_helper;
    WebHelperPluginFrameClient m_frameClient;
    OwnPtr<WebHelperPlugin> m_plugin;
};

TEST_F(WebHelperPluginTest, CreateAndDestroyAfterWebViewDestruction)
{
    m_plugin = adoptPtr(WebHelperPlugin::create("hello", m_helper.webView()->mainFrame()->toWebLocalFrame()));
    EXPECT_TRUE(m_plugin);
    EXPECT_TRUE(m_plugin->getPlugin());

    m_helper.reset();
    destroyHelperPlugin();
}

TEST_F(WebHelperPluginTest, CreateAndDestroyBeforeWebViewDestruction)
{
    m_plugin = adoptPtr(WebHelperPlugin::create("hello", m_helper.webView()->mainFrame()->toWebLocalFrame()));
    EXPECT_TRUE(m_plugin);
    EXPECT_TRUE(m_plugin->getPlugin());

    destroyHelperPlugin();
    m_helper.reset();
}

TEST_F(WebHelperPluginTest, CreateFailsWithPlaceholder)
{
    m_frameClient.setCreatePlaceholder(true);

    m_plugin = adoptPtr(WebHelperPlugin::create("hello", m_helper.webView()->mainFrame()->toWebLocalFrame()));
    EXPECT_EQ(0, m_plugin.get());
}

} // namespace

} // namespace
