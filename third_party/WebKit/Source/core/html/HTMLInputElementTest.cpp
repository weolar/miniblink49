// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLInputElement.h"

#include "core/dom/Document.h"
#include <gtest/gtest.h>

namespace blink {

TEST(HTMLInputElementTest, create)
{
    const RefPtrWillBeRawPtr<Document> document = Document::create();
    RefPtrWillBeRawPtr<HTMLInputElement> input = HTMLInputElement::create(*document, nullptr, /* createdByParser */ false);
    EXPECT_NE(nullptr, input->userAgentShadowRoot());

    input = HTMLInputElement::create(*document, nullptr, /* createdByParser */ true);
    EXPECT_EQ(nullptr, input->userAgentShadowRoot());
    input->parserSetAttributes(Vector<Attribute>());
    EXPECT_NE(nullptr, input->userAgentShadowRoot());
}

} // namespace blink
