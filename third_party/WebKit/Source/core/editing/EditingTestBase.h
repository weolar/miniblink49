// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EditingTestBase_h
#define EditingTestBase_h

#include "core/dom/Position.h"
#include "core/testing/CoreTestHelpers.h"
#include "wtf/Forward.h"
#include <gtest/gtest.h>

namespace blink {

class DummyPageHolder;

class EditingTestBase : public ::testing::Test {
protected:
    EditingTestBase();
    ~EditingTestBase() override;

    void SetUp() override;

    Document& document() const;

    static PassRefPtrWillBeRawPtr<ShadowRoot> createShadowRootForElementWithIDAndSetInnerHTML(TreeScope&, const char* hostElementID, const char* shadowRootContent);

    void setBodyContent(const char*);
    PassRefPtrWillBeRawPtr<ShadowRoot> setShadowContent(const char* shadowContent, const char* host = "host");
    void updateLayoutAndStyleForPainting();

    static Position positionInDOMTree(Node& anchor, int offset);
    static PositionInComposedTree positionInComposedTree(Node& anchor, int offset);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

} // namespace blink

#endif // EditingTestBase_h
