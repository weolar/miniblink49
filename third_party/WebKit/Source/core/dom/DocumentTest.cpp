/*
 * Copyright (c) 2014, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/Document.h"

#include "core/dom/DocumentVisibilityObserver.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {

class DocumentTest : public ::testing::Test {
protected:
    void SetUp() override;

#if ENABLE(OILPAN)
    void TearDown() override
    {
        Heap::collectAllGarbage();
    }
#endif

    Document& document() const { return m_dummyPageHolder->document(); }
    Page& page() const { return m_dummyPageHolder->page(); }

    void setHtmlInnerHTML(const char*);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

void DocumentTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
}

void DocumentTest::setHtmlInnerHTML(const char* htmlContent)
{
    document().documentElement()->setInnerHTML(String::fromUTF8(htmlContent), ASSERT_NO_EXCEPTION);
    document().view()->updateAllLifecyclePhases();
}

class MockDocumentVisibilityObserver
    : public NoBaseWillBeGarbageCollectedFinalized<MockDocumentVisibilityObserver>
    , public DocumentVisibilityObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MockDocumentVisibilityObserver);
public:
    static PassOwnPtrWillBeRawPtr<MockDocumentVisibilityObserver> create(Document& document)
    {
        return adoptPtrWillBeNoop(new MockDocumentVisibilityObserver(document));
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        DocumentVisibilityObserver::trace(visitor);
    }

    MOCK_METHOD1(didChangeVisibilityState, void(PageVisibilityState));

private:
    MockDocumentVisibilityObserver(Document& document) : DocumentVisibilityObserver(document) { }
};

TEST_F(DocumentTest, VisibilityOberver)
{
    page().setVisibilityState(PageVisibilityStateVisible, true); // initial state
    OwnPtrWillBeRawPtr<MockDocumentVisibilityObserver> observer1 = MockDocumentVisibilityObserver::create(document());

    {
        OwnPtrWillBeRawPtr<MockDocumentVisibilityObserver> observer2 = MockDocumentVisibilityObserver::create(document());
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        ::testing::Mock::VerifyAndClearExpectations(observer1.get());
        ::testing::Mock::VerifyAndClearExpectations(observer2.get());

        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(1);
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateHidden)).Times(1);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        page().setVisibilityState(PageVisibilityStateHidden, false);
        ::testing::Mock::VerifyAndClearExpectations(observer1.get());
        ::testing::Mock::VerifyAndClearExpectations(observer2.get());

        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        page().setVisibilityState(PageVisibilityStateHidden, false);
        ::testing::Mock::VerifyAndClearExpectations(observer1.get());
        ::testing::Mock::VerifyAndClearExpectations(observer2.get());

        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(1);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        OwnPtr<DummyPageHolder> alternatePage = DummyPageHolder::create(IntSize(800, 600));
        Document& alternateDocument = alternatePage->document();
        observer2->setObservedDocument(alternateDocument);
        page().setVisibilityState(PageVisibilityStateVisible, false);
        ::testing::Mock::VerifyAndClearExpectations(observer1.get());
        ::testing::Mock::VerifyAndClearExpectations(observer2.get());

        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(1);
        EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateHidden)).Times(1);
        EXPECT_CALL(*observer2, didChangeVisibilityState(PageVisibilityStateVisible)).Times(0);
        observer2->setObservedDocument(document());
        page().setVisibilityState(PageVisibilityStateHidden, false);
        ::testing::Mock::VerifyAndClearExpectations(observer1.get());
        ::testing::Mock::VerifyAndClearExpectations(observer2.get());
    }

    // observer2 destroyed
    EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateHidden)).Times(0);
    EXPECT_CALL(*observer1, didChangeVisibilityState(PageVisibilityStateVisible)).Times(1);
    page().setVisibilityState(PageVisibilityStateVisible, false);
}

// This tests that we properly resize and re-layout pages for printing in the presence of
// media queries effecting elements in a subtree layout boundary
TEST_F(DocumentTest, PrintRelayout)
{
    setHtmlInnerHTML(
        "<style>"
        "    div {"
        "        width: 100px;"
        "        height: 100px;"
        "        overflow: hidden;"
        "    }"
        "    span {"
        "        width: 50px;"
        "        height: 50px;"
        "    }"
        "    @media screen {"
        "        span {"
        "            width: 20px;"
        "        }"
        "    }"
        "</style>"
        "<p><div><span></span></div></p>");
    FloatSize pageSize(400, 400);
    float maximumShrinkRatio = 1.6;

    document().frame()->setPrinting(true, pageSize, pageSize, maximumShrinkRatio);
    EXPECT_EQ(document().documentElement()->offsetWidth(), 400);
    document().frame()->setPrinting(false, FloatSize(), FloatSize(), 0);
    EXPECT_EQ(document().documentElement()->offsetWidth(), 800);

}

// This test checks that Documunt::linkManifest() returns a value conform to the specification.
TEST_F(DocumentTest, LinkManifest)
{
    // Test the default result.
    EXPECT_EQ(0, document().linkManifest());

    // Check that we use the first manifest with <link rel=manifest>
    RefPtrWillBeRawPtr<HTMLLinkElement> link = HTMLLinkElement::create(document(), false);
    link->setAttribute(blink::HTMLNames::relAttr, "manifest");
    link->setAttribute(blink::HTMLNames::hrefAttr, "foo.json");
    document().head()->appendChild(link);
    EXPECT_EQ(link, document().linkManifest());

    RefPtrWillBeRawPtr<HTMLLinkElement> link2 = HTMLLinkElement::create(document(), false);
    link2->setAttribute(blink::HTMLNames::relAttr, "manifest");
    link2->setAttribute(blink::HTMLNames::hrefAttr, "bar.json");
    document().head()->insertBefore(link2, link.get());
    EXPECT_EQ(link2, document().linkManifest());
    document().head()->appendChild(link2);
    EXPECT_EQ(link, document().linkManifest());

    // Check that crazy URLs are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "http:foo.json");
    EXPECT_EQ(link, document().linkManifest());

    // Check that empty URLs are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "");
    EXPECT_EQ(link, document().linkManifest());

    // Check that URLs from different origins are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://example.org/manifest.json");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://foo.example.org/manifest.json");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://foo.bar/manifest.json");
    EXPECT_EQ(link, document().linkManifest());

    // More than one token in @rel is accepted.
    link->setAttribute(blink::HTMLNames::relAttr, "foo bar manifest");
    EXPECT_EQ(link, document().linkManifest());

    // Such as spaces around the token.
    link->setAttribute(blink::HTMLNames::relAttr, " manifest ");
    EXPECT_EQ(link, document().linkManifest());

    // Check that rel=manifest actually matters.
    link->setAttribute(blink::HTMLNames::relAttr, "");
    EXPECT_EQ(link2, document().linkManifest());
    link->setAttribute(blink::HTMLNames::relAttr, "manifest");

    // Check that link outside of the <head> are ignored.
    document().head()->removeChild(link.get(), ASSERT_NO_EXCEPTION);
    document().head()->removeChild(link2.get(), ASSERT_NO_EXCEPTION);
    EXPECT_EQ(0, document().linkManifest());
    document().body()->appendChild(link);
    EXPECT_EQ(0, document().linkManifest());
    document().head()->appendChild(link);
    document().head()->appendChild(link2);

    // Check that some attribute values do not have an effect.
    link->setAttribute(blink::HTMLNames::crossoriginAttr, "use-credentials");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::hreflangAttr, "klingon");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::typeAttr, "image/gif");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::sizesAttr, "16x16");
    EXPECT_EQ(link, document().linkManifest());
    link->setAttribute(blink::HTMLNames::mediaAttr, "print");
    EXPECT_EQ(link, document().linkManifest());
}

// This test checks that Documunt::linkDefaultPresentation() returns a value conform to the specification.
TEST_F(DocumentTest, linkDefaultPresentation)
{
    // Test the default result.
    EXPECT_EQ(0, document().linkDefaultPresentation());

    // Check that we use the first element with <link rel='default-presentation'>
    RefPtrWillBeRawPtr<HTMLLinkElement> link = HTMLLinkElement::create(document(), false);
    link->setAttribute(blink::HTMLNames::relAttr, "default-presentation");
    link->setAttribute(blink::HTMLNames::hrefAttr, "presentation.html");
    document().head()->appendChild(link);
    EXPECT_EQ(link, document().linkDefaultPresentation());

    RefPtrWillBeRawPtr<HTMLLinkElement> link2 = HTMLLinkElement::create(document(), false);
    link2->setAttribute(blink::HTMLNames::relAttr, "default-presentation");
    link2->setAttribute(blink::HTMLNames::hrefAttr, "presentation.html");
    document().head()->insertBefore(link2, link.get());
    EXPECT_EQ(link2, document().linkDefaultPresentation());
    document().head()->appendChild(link2);
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // Check that crazy URLs are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "far:foo.bar");
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // Check that empty URLs are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "");
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // Check that URLs from different origins are accepted.
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://example.org/presentation.html");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://foo.example.org/presentation.html");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::hrefAttr, "http://foo.bar/presentation.html");
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // More than one token in @rel is accepted.
    link->setAttribute(blink::HTMLNames::relAttr, "foo bar default-presentation");
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // Such as spaces around the token.
    link->setAttribute(blink::HTMLNames::relAttr, " default-presentation ");
    EXPECT_EQ(link, document().linkDefaultPresentation());

    // Check that rel=default-presentation actually matters.
    link->setAttribute(blink::HTMLNames::relAttr, "");
    EXPECT_EQ(link2, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::relAttr, "default-presentation");

    // Check that links outside of the <head> are ignored.
    document().head()->removeChild(link.get(), ASSERT_NO_EXCEPTION);
    document().head()->removeChild(link2.get(), ASSERT_NO_EXCEPTION);
    EXPECT_EQ(0, document().linkDefaultPresentation());
    document().body()->appendChild(link);
    EXPECT_EQ(0, document().linkDefaultPresentation());
    document().head()->appendChild(link);
    document().head()->appendChild(link2);

    // Check that some attribute values do not have an effect.
    link->setAttribute(blink::HTMLNames::crossoriginAttr, "use-credentials");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::hreflangAttr, "klingon");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::typeAttr, "image/gif");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::sizesAttr, "16x16");
    EXPECT_EQ(link, document().linkDefaultPresentation());
    link->setAttribute(blink::HTMLNames::mediaAttr, "print");
    EXPECT_EQ(link, document().linkDefaultPresentation());
}

TEST_F(DocumentTest, referrerPolicyParsing)
{
    EXPECT_EQ(ReferrerPolicyDefault, document().referrerPolicy());

    struct TestCase {
        const char* policy;
        ReferrerPolicy expected;
    } tests[] = {
        { "always", ReferrerPolicyAlways },
        { "default", ReferrerPolicyNoReferrerWhenDowngrade },
        { "never", ReferrerPolicyNever },
        { "no-referrer", ReferrerPolicyNever },
        { "no-referrer-when-downgrade", ReferrerPolicyNoReferrerWhenDowngrade },
        { "not-a-real-policy", ReferrerPolicyDefault },
        { "origin", ReferrerPolicyOrigin },
        { "origin-when-crossorigin", ReferrerPolicyOriginWhenCrossOrigin },
        { "origin-when-cross-origin", ReferrerPolicyOriginWhenCrossOrigin },
        { "unsafe-url", ReferrerPolicyAlways },
    };

    for (auto test : tests) {
        document().setReferrerPolicy(ReferrerPolicyDefault);

        document().processReferrerPolicy(test.policy);
        EXPECT_EQ(test.expected, document().referrerPolicy()) << test.policy;
    }
}

} // namespace blink
