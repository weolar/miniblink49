// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutPart.h"

#include "core/html/HTMLElement.h"
#include "core/layout/ImageQualityController.h"
#include "core/layout/LayoutTestHelper.h"
#include <gtest/gtest.h>

namespace blink {

class LayoutPartTest : public RenderingTest {
};

class OverriddenLayoutPart : public LayoutPart {
public:
    explicit OverriddenLayoutPart(Element* element) : LayoutPart(element) { }

    virtual const char* name() const override { return "OverriddenLayoutPart"; }
};

TEST_F(LayoutPartTest, DestroyUpdatesImageQualityController)
{
    RefPtrWillBeRawPtr<Element> element = HTMLElement::create(HTMLNames::divTag, document());
    LayoutObject* part = new OverriddenLayoutPart(element.get());
    // The third and forth arguments are not important in this test.
    ImageQualityController::imageQualityController()->set(part, 0, this, LayoutSize(1, 1));
    EXPECT_TRUE(ImageQualityController::has(part));
    part->destroy();
    EXPECT_FALSE(ImageQualityController::has(part));
}

}
