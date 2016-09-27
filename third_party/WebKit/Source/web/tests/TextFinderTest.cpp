// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/TextFinder.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/Document.h"
#include "core/dom/NodeList.h"
#include "core/dom/Range.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLElement.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/web/WebDocument.h"
#include "web/FindInPageCoordinates.h"
#include "web/WebLocalFrameImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include "wtf/OwnPtr.h"
#include <gtest/gtest.h>

using blink::testing::runPendingTasks;

namespace blink {

class TextFinderTest : public ::testing::Test {
protected:
    void SetUp() override;

    Document& document() const;
    TextFinder& textFinder() const;

    static WebFloatRect findInPageRect(Node* startContainer, int startOffset, Node* endContainer, int endOffset);

private:
    FrameTestHelpers::WebViewHelper m_webViewHelper;
    RefPtrWillBePersistent<Document> m_document;
    TextFinder* m_textFinder;
};

void TextFinderTest::SetUp()
{
    m_webViewHelper.initialize();
    WebLocalFrameImpl& frameImpl = *m_webViewHelper.webViewImpl()->mainFrameImpl();
    frameImpl.viewImpl()->resize(WebSize(640, 480));
    frameImpl.viewImpl()->layout();
    m_document = PassRefPtrWillBeRawPtr<Document>(frameImpl.document());
    m_textFinder = &frameImpl.ensureTextFinder();
}

Document& TextFinderTest::document() const
{
    return *m_document;
}

TextFinder& TextFinderTest::textFinder() const
{
    return *m_textFinder;
}

WebFloatRect TextFinderTest::findInPageRect(Node* startContainer, int startOffset, Node* endContainer, int endOffset)
{
    RefPtrWillBeRawPtr<Range> range = Range::create(startContainer->document(), startContainer, startOffset, endContainer, endOffset);
    return WebFloatRect(findInPageRectFromRange(range.get()));
}

TEST_F(TextFinderTest, FindTextSimple)
{
    document().body()->setInnerHTML("XXXXFindMeYYYYfindmeZZZZ", ASSERT_NO_EXCEPTION);
    Node* textNode = document().body()->firstChild();

    int identifier = 0;
    WebString searchText(String("FindMe"));
    WebFindOptions findOptions; // Default.
    bool wrapWithinFrame = true;
    WebRect* selectionRect = nullptr;

    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    Range* activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(4, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(10, activeMatch->endOffset());

    findOptions.findNext = true;
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(14, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(20, activeMatch->endOffset());

    // Should wrap to the first match.
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(4, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(10, activeMatch->endOffset());

    // Search in the reverse order.
    identifier = 1;
    findOptions = WebFindOptions();
    findOptions.forward = false;

    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(14, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(20, activeMatch->endOffset());

    findOptions.findNext = true;
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(4, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(10, activeMatch->endOffset());

    // Wrap to the first match (last occurence in the document).
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textNode, activeMatch->startContainer());
    EXPECT_EQ(14, activeMatch->startOffset());
    EXPECT_EQ(textNode, activeMatch->endContainer());
    EXPECT_EQ(20, activeMatch->endOffset());
}

TEST_F(TextFinderTest, FindTextNotFound)
{
    document().body()->setInnerHTML("XXXXFindMeYYYYfindmeZZZZ", ASSERT_NO_EXCEPTION);

    int identifier = 0;
    WebString searchText(String("Boo"));
    WebFindOptions findOptions; // Default.
    bool wrapWithinFrame = true;
    WebRect* selectionRect = nullptr;

    EXPECT_FALSE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    EXPECT_FALSE(textFinder().activeMatch());
}

TEST_F(TextFinderTest, FindTextInShadowDOM)
{
    document().body()->setInnerHTML("<b>FOO</b><i>foo</i>", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = document().body()->createShadowRoot(ASSERT_NO_EXCEPTION);
    shadowRoot->setInnerHTML("<content select=\"i\"></content><u>Foo</u><content></content>", ASSERT_NO_EXCEPTION);
    Node* textInBElement = document().body()->firstChild()->firstChild();
    Node* textInIElement = document().body()->lastChild()->firstChild();
    Node* textInUElement = shadowRoot->childNodes()->item(1)->firstChild();

    int identifier = 0;
    WebString searchText(String("foo"));
    WebFindOptions findOptions; // Default.
    bool wrapWithinFrame = true;
    WebRect* selectionRect = nullptr;

    // TextIterator currently returns the matches in the document order, instead of the visual order. It visits
    // the shadow roots first, so in this case the matches will be returned in the order of <u> -> <b> -> <i>.
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    Range* activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInUElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInUElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    findOptions.findNext = true;
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInBElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInBElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInIElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInIElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    // Should wrap to the first match.
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInUElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInUElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    // Fresh search in the reverse order.
    identifier = 1;
    findOptions = WebFindOptions();
    findOptions.forward = false;

    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInIElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInIElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    findOptions.findNext = true;
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInBElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInBElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInUElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInUElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());

    // And wrap.
    ASSERT_TRUE(textFinder().find(identifier, searchText, findOptions, wrapWithinFrame, selectionRect));
    activeMatch = textFinder().activeMatch();
    ASSERT_TRUE(activeMatch);
    EXPECT_EQ(textInIElement, activeMatch->startContainer());
    EXPECT_EQ(0, activeMatch->startOffset());
    EXPECT_EQ(textInIElement, activeMatch->endContainer());
    EXPECT_EQ(3, activeMatch->endOffset());
}

TEST_F(TextFinderTest, ScopeTextMatchesSimple)
{
    document().body()->setInnerHTML("XXXXFindMeYYYYfindmeZZZZ", ASSERT_NO_EXCEPTION);
    Node* textNode = document().body()->firstChild();

    int identifier = 0;
    WebString searchText(String("FindMe"));
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();
    textFinder().scopeStringMatches(identifier, searchText, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    EXPECT_EQ(2, textFinder().totalMatchCount());
    WebVector<WebFloatRect> matchRects;
    textFinder().findMatchRects(matchRects);
    ASSERT_EQ(2u, matchRects.size());
    EXPECT_EQ(findInPageRect(textNode, 4, textNode, 10), matchRects[0]);
    EXPECT_EQ(findInPageRect(textNode, 14, textNode, 20), matchRects[1]);
}

TEST_F(TextFinderTest, ScopeTextMatchesWithShadowDOM)
{
    document().body()->setInnerHTML("<b>FOO</b><i>foo</i>", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = document().body()->createShadowRoot(ASSERT_NO_EXCEPTION);
    shadowRoot->setInnerHTML("<content select=\"i\"></content><u>Foo</u><content></content>", ASSERT_NO_EXCEPTION);
    Node* textInBElement = document().body()->firstChild()->firstChild();
    Node* textInIElement = document().body()->lastChild()->firstChild();
    Node* textInUElement = shadowRoot->childNodes()->item(1)->firstChild();

    int identifier = 0;
    WebString searchText(String("fOO"));
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();
    textFinder().scopeStringMatches(identifier, searchText, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    // TextIterator currently returns the matches in the document order, instead of the visual order. It visits
    // the shadow roots first, so in this case the matches will be returned in the order of <u> -> <b> -> <i>.
    EXPECT_EQ(3, textFinder().totalMatchCount());
    WebVector<WebFloatRect> matchRects;
    textFinder().findMatchRects(matchRects);
    ASSERT_EQ(3u, matchRects.size());
    EXPECT_EQ(findInPageRect(textInUElement, 0, textInUElement, 3), matchRects[0]);
    EXPECT_EQ(findInPageRect(textInBElement, 0, textInBElement, 3), matchRects[1]);
    EXPECT_EQ(findInPageRect(textInIElement, 0, textInIElement, 3), matchRects[2]);
}

TEST_F(TextFinderTest, ScopeRepeatPatternTextMatches)
{
    document().body()->setInnerHTML("ab ab ab ab ab", ASSERT_NO_EXCEPTION);
    Node* textNode = document().body()->firstChild();

    int identifier = 0;
    WebString searchText(String("ab ab"));
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();
    textFinder().scopeStringMatches(identifier, searchText, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    EXPECT_EQ(2, textFinder().totalMatchCount());
    WebVector<WebFloatRect> matchRects;
    textFinder().findMatchRects(matchRects);
    ASSERT_EQ(2u, matchRects.size());
    EXPECT_EQ(findInPageRect(textNode, 0, textNode, 5), matchRects[0]);
    EXPECT_EQ(findInPageRect(textNode, 6, textNode, 11), matchRects[1]);
}

TEST_F(TextFinderTest, OverlappingMatches)
{
    document().body()->setInnerHTML("aababaa", ASSERT_NO_EXCEPTION);
    Node* textNode = document().body()->firstChild();

    int identifier = 0;
    WebString searchText(String("aba"));
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();
    textFinder().scopeStringMatches(identifier, searchText, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    // We shouldn't find overlapped matches.
    EXPECT_EQ(1, textFinder().totalMatchCount());
    WebVector<WebFloatRect> matchRects;
    textFinder().findMatchRects(matchRects);
    ASSERT_EQ(1u, matchRects.size());
    EXPECT_EQ(findInPageRect(textNode, 1, textNode, 4), matchRects[0]);
}

TEST_F(TextFinderTest, SequentialMatches)
{
    document().body()->setInnerHTML("ababab", ASSERT_NO_EXCEPTION);
    Node* textNode = document().body()->firstChild();

    int identifier = 0;
    WebString searchText(String("ab"));
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();
    textFinder().scopeStringMatches(identifier, searchText, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    EXPECT_EQ(3, textFinder().totalMatchCount());
    WebVector<WebFloatRect> matchRects;
    textFinder().findMatchRects(matchRects);
    ASSERT_EQ(3u, matchRects.size());
    EXPECT_EQ(findInPageRect(textNode, 0, textNode, 2), matchRects[0]);
    EXPECT_EQ(findInPageRect(textNode, 2, textNode, 4), matchRects[1]);
    EXPECT_EQ(findInPageRect(textNode, 4, textNode, 6), matchRects[2]);
}

class TextFinderFakeTimerTest : public TextFinderTest {
protected:
    void SetUp() override;
    void TearDown() override;

    // A simple platform that mocks out the clock.
    class TimeProxyPlatform : public Platform {
    public:
        TimeProxyPlatform()
            : m_timeCounter(0.)
            , m_fallbackPlatform(0)
        { }

        void install()
        {
            // Check that the proxy wasn't installed yet.
            ASSERT_NE(Platform::current(), this);
            m_fallbackPlatform = Platform::current();
            m_timeCounter = m_fallbackPlatform->currentTime();
            Platform::initialize(this);
            ASSERT_EQ(Platform::current(), this);
        }

        void remove()
        {
            // Check that the proxy was installed.
            ASSERT_EQ(Platform::current(), this);
            Platform::initialize(m_fallbackPlatform);
            ASSERT_EQ(Platform::current(), m_fallbackPlatform);
            m_fallbackPlatform = 0;
        }

    private:
        Platform& ensureFallback()
        {
            ASSERT(m_fallbackPlatform);
            return *m_fallbackPlatform;
        }

        // From blink::Platform:
        double currentTime() override
        {
            return ++m_timeCounter;
        }

        // These blink::Platform methods must be overriden to make a usable object.
        void cryptographicallyRandomValues(unsigned char* buffer, size_t length) override
        {
            ensureFallback().cryptographicallyRandomValues(buffer, length);
        }

        const unsigned char* getTraceCategoryEnabledFlag(const char* categoryName) override
        {
            return ensureFallback().getTraceCategoryEnabledFlag(categoryName);
        }

        // These two methods allow timers to work correctly.
        double monotonicallyIncreasingTime() override
        {
            return ensureFallback().monotonicallyIncreasingTime();
        }

        WebThread* currentThread() override { return ensureFallback().currentThread(); }
        WebUnitTestSupport* unitTestSupport() override { return ensureFallback().unitTestSupport(); }
        WebString defaultLocale() override { return ensureFallback().defaultLocale(); }
        WebCompositorSupport* compositorSupport() override { return ensureFallback().compositorSupport(); }

        double m_timeCounter;
        Platform* m_fallbackPlatform;
    };

    TimeProxyPlatform m_proxyTimePlatform;
};

void TextFinderFakeTimerTest::SetUp()
{
    TextFinderTest::SetUp();
    m_proxyTimePlatform.install();
}

void TextFinderFakeTimerTest::TearDown()
{
    m_proxyTimePlatform.remove();
    TextFinderTest::TearDown();
}

TEST_F(TextFinderFakeTimerTest, ScopeWithTimeouts)
{
    // Make a long string.
    String text(Vector<UChar>(100));
    text.fill('a');
    String searchPattern("abc");
    // Make 4 substrings "abc" in text.
    text.insert(searchPattern, 1);
    text.insert(searchPattern, 10);
    text.insert(searchPattern, 50);
    text.insert(searchPattern, 90);

    document().body()->setInnerHTML(text, ASSERT_NO_EXCEPTION);

    int identifier = 0;
    WebFindOptions findOptions; // Default.

    textFinder().resetMatchCount();

    // There will be only one iteration before timeout, because increment
    // of the TimeProxyPlatform timer is greater than timeout threshold.
    textFinder().scopeStringMatches(identifier, searchPattern, findOptions, true);
    while (textFinder().scopingInProgress())
        runPendingTasks();

    EXPECT_EQ(4, textFinder().totalMatchCount());
}

} // namespace blink
