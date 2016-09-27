// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutTestHelper_h
#define LayoutTestHelper_h

#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLElement.h"
#include "core/testing/DummyPageHolder.h"
#include "wtf/OwnPtr.h"
#include <gtest/gtest.h>

namespace blink {

class RenderingTest : public testing::Test {
protected:
    virtual void SetUp() override;

    Document& document() const { return m_pageHolder->document(); }

    void setBodyInnerHTML(const String& htmlContent)
    {
        document().body()->setInnerHTML(htmlContent, ASSERT_NO_EXCEPTION);
        document().view()->updateAllLifecyclePhases();
    }

    void enableCompositing()
    {
        m_pageHolder->page().settings().setAcceleratedCompositingEnabled(true);
        document().view()->updateAllLifecyclePhases();
    }

private:
    Page::PageClients m_pageClients;
    OwnPtr<DummyPageHolder> m_pageHolder;
};

} // namespace blink

#endif // LayoutTestHelper_h
