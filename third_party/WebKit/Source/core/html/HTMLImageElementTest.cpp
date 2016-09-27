// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLImageElement.h"

#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

const int viewportWidth = 500;
const int viewportHeight = 600;
class HTMLImageElementTest : public testing::Test {
protected:
    HTMLImageElementTest()
        : m_dummyPageHolder(DummyPageHolder::create(IntSize(viewportWidth, viewportHeight)))
    {
    }

    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

TEST_F(HTMLImageElementTest, width)
{
    RefPtrWillBeRawPtr<HTMLImageElement> image = HTMLImageElement::create(m_dummyPageHolder->document(), nullptr, /* createdByParser */ false);
    image->setAttribute(HTMLNames::widthAttr, "400");
    // TODO(yoav): `width` does not impact resourceWidth until we resolve https://github.com/ResponsiveImagesCG/picture-element/issues/268
    EXPECT_EQ(500, image->resourceWidth().width);
    image->setAttribute(HTMLNames::sizesAttr, "100vw");
    EXPECT_EQ(500, image->resourceWidth().width);
}

TEST_F(HTMLImageElementTest, sourceSize)
{
    RefPtrWillBeRawPtr<HTMLImageElement> image = HTMLImageElement::create(m_dummyPageHolder->document(), nullptr, /* createdByParser */ false);
    image->setAttribute(HTMLNames::widthAttr, "400");
    EXPECT_EQ(viewportWidth, image->sourceSize(*image));
    image->setAttribute(HTMLNames::sizesAttr, "50vw");
    EXPECT_EQ(250, image->sourceSize(*image));
}

} // namespace blink
