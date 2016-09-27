// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutTestHelper.h"

#include "core/loader/EmptyClients.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/GraphicsLayerFactory.h"

namespace blink {

class FakeGraphicsLayerFactory : public GraphicsLayerFactory {
public:
    virtual PassOwnPtr<GraphicsLayer> createGraphicsLayer(GraphicsLayerClient* client) override
    {
        return adoptPtr(new GraphicsLayer(client));
    }
};

class FakeChromeClient : public EmptyChromeClient {
public:
    virtual GraphicsLayerFactory* graphicsLayerFactory() const
    {
        static FakeGraphicsLayerFactory* factory = adoptPtr(new FakeGraphicsLayerFactory).leakPtr();
        return factory;
    }
};

void RenderingTest::SetUp()
{
    fillWithEmptyClients(m_pageClients);
    static FakeChromeClient* chromeClient = adoptPtr(new FakeChromeClient).leakPtr();
    m_pageClients.chromeClient = chromeClient;
    m_pageHolder = DummyPageHolder::create(IntSize(800, 600), &m_pageClients);

    // This ensures that the minimal DOM tree gets attached
    // correctly for tests that don't call setBodyInnerHTML.
    document().view()->updateAllLifecyclePhases();
}


} // namespace blink
