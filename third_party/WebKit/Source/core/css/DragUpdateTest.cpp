// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/Element.h"
#include "core/dom/StyleEngine.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/layout/LayoutObject.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

TEST(DragUpdateTest, AffectedByDragUpdate)
{
    // Check that when dragging the div in the document below, you only get a
    // single element style recalc.

    OwnPtr<DummyPageHolder> dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
    HTMLDocument& document = toHTMLDocument(dummyPageHolder->document());
    document.documentElement()->setInnerHTML("<style>div {width:100px;height:100px} div:-webkit-drag { background-color: green }</style>"
        "<div>"
        "<span></span>"
        "<span></span>"
        "<span></span>"
        "<span></span>"
        "</div>", ASSERT_NO_EXCEPTION);

    document.view()->updateAllLifecyclePhases();
    unsigned startCount = document.styleEngine().resolverAccessCount();

    document.documentElement()->layoutObject()->updateDragState(true);
    document.view()->updateAllLifecyclePhases();

    unsigned accessCount = document.styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(1U, accessCount);
}

TEST(DragUpdateTest, ChildrenOrSiblingsAffectedByDragUpdate)
{
    // Check that when dragging the div in the document below, you get a
    // full subtree style recalc.

    OwnPtr<DummyPageHolder> dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
    HTMLDocument& document = toHTMLDocument(dummyPageHolder->document());
    document.documentElement()->setInnerHTML("<style>div {width:100px;height:100px} div:-webkit-drag span { background-color: green }</style>"
        "<div>"
        "<span></span>"
        "<span></span>"
        "<span></span>"
        "<span></span>"
        "</div>", ASSERT_NO_EXCEPTION);

    document.updateLayout();
    unsigned startCount = document.styleEngine().resolverAccessCount();

    document.documentElement()->layoutObject()->updateDragState(true);
    document.updateLayout();

    unsigned accessCount = document.styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(5U, accessCount);
}

} // namespace blink
