// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLFormControlElement.h"

#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLInputElement.h"
#include "core/layout/LayoutObject.h"
#include "core/loader/EmptyClients.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

class HTMLFormControlElementTest : public ::testing::Test {
protected:
    void SetUp() override;

    DummyPageHolder& page() const { return *m_dummyPageHolder; }
    HTMLDocument& document() const { return *m_document; }

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
    RefPtrWillBePersistent<HTMLDocument> m_document;
};

void HTMLFormControlElementTest::SetUp()
{
    Page::PageClients pageClients;
    fillWithEmptyClients(pageClients);
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600), &pageClients);

    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    m_document->setMimeType("text/html");
}

TEST_F(HTMLFormControlElementTest, customValidationMessageTextDirection)
{
    document().documentElement()->setInnerHTML("<body><input required id=input></body>", ASSERT_NO_EXCEPTION);
    document().view()->updateAllLifecyclePhases();

    HTMLInputElement* input = toHTMLInputElement(document().getElementById("input"));
    input->setCustomValidity(String::fromUTF8("\xD8\xB9\xD8\xB1\xD8\xA8\xD9\x89"));
    input->setAttribute(HTMLNames::titleAttr, AtomicString::fromUTF8("\xD8\xB9\xD8\xB1\xD8\xA8\xD9\x89"));

    String message = input->validationMessage().stripWhiteSpace();
    String subMessage = String();
    TextDirection messageDir = RTL;
    TextDirection subMessageDir = LTR;

    input->findCustomValidationMessageTextDirection(message, messageDir, subMessage, subMessageDir);
    EXPECT_EQ(RTL, messageDir);
    EXPECT_EQ(LTR, subMessageDir);

    input->layoutObject()->mutableStyleRef().setDirection(RTL);
    input->findCustomValidationMessageTextDirection(message, messageDir, subMessage, subMessageDir);
    EXPECT_EQ(RTL, messageDir);
    EXPECT_EQ(RTL, subMessageDir);

    input->setCustomValidity(String::fromUTF8("Main message."));
    message = input->validationMessage().stripWhiteSpace();
    input->findCustomValidationMessageTextDirection(message, messageDir, subMessage, subMessageDir);
    EXPECT_EQ(LTR, messageDir);
    EXPECT_EQ(RTL, subMessageDir);
}

} // namespace blink
