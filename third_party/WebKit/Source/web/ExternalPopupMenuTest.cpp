// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/ExternalPopupMenu.h"

#include "core/HTMLNames.h"
#include "core/frame/FrameHost.h"
#include "core/frame/PinchViewport.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/forms/PopupMenuClient.h"
#include "core/layout/LayoutMenuList.h"
#include "core/page/Page.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/PopupMenu.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebExternalPopupMenu.h"
#include "public/web/WebPopupMenuInfo.h"
#include "public/web/WebSettings.h"
#include "web/WebLocalFrameImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

const size_t kListSize = 7;

class TestPopupMenuClient : public PopupMenuClient {
public:
    TestPopupMenuClient() : m_listSize(0) { }
    ~TestPopupMenuClient() override { }

    void valueChanged(unsigned listIndex, bool fireEvents = true) override { }
    void selectionChanged(unsigned listIndex, bool fireEvents = true) override { }
    void selectionCleared() override { }

    String itemText(unsigned listIndex) const override { return emptyString(); }
    String itemToolTip(unsigned listIndex) const override { return emptyString(); }
    String itemAccessibilityText(unsigned listIndex) const override { return emptyString(); }
    bool itemIsEnabled(unsigned listIndex) const override { return true; }
    PopupMenuStyle itemStyle(unsigned listIndex) const override
    {
        FontDescription fontDescription;
        fontDescription.setComputedSize(12.0);
        Font font(fontDescription);
        font.update(nullptr);
        bool displayNone = m_displayNoneIndexSet.find(listIndex) != m_displayNoneIndexSet.end();
        return PopupMenuStyle(Color::black, Color::white, font, true, displayNone, Length(), TextDirection(), false);
    }
    PopupMenuStyle menuStyle() const override { return itemStyle(0); }
    LayoutUnit clientPaddingLeft() const override { return 0; }
    LayoutUnit clientPaddingRight() const override { return 0; }
    int listSize() const override { return m_listSize; }
    int selectedIndex() const override { return 0; }
    void popupDidHide() override { }
    void popupDidCancel() override { }
    bool itemIsSeparator(unsigned listIndex) const override { return false;}
    bool itemIsLabel(unsigned listIndex) const override { return false; }
    bool itemIsSelected(unsigned listIndex) const override { return listIndex == 0;}
    void provisionalSelectionChanged(unsigned listIndex) override { }
    bool multiple() const override { return false; }
    IntRect elementRectRelativeToViewport() const override { return IntRect(); }
    Element& ownerElement() const override { return *m_ownerElement; }
    const ComputedStyle* computedStyleForItem(Element& element) const override { return nullptr; }

    void setListSize(size_t size) { m_listSize = size; }
    void setDisplayNoneIndex(unsigned index) { m_displayNoneIndexSet.insert(index); }
    void setOwnerElement(PassRefPtrWillBeRawPtr<Element> element) { m_ownerElement = element; }

private:
    size_t m_listSize;
    std::set<unsigned> m_displayNoneIndexSet;
    RefPtrWillBePersistent<Element> m_ownerElement;
};

class ExternalPopupMenuDisplayNoneItemsTest : public testing::Test {
public:
    ExternalPopupMenuDisplayNoneItemsTest() { }

protected:
    void SetUp() override
    {
        m_popupMenuClient.setListSize(kListSize);

        // Set the 4th an 5th items to have "display: none" property
        m_popupMenuClient.setDisplayNoneIndex(3);
        m_popupMenuClient.setDisplayNoneIndex(4);

        OwnPtr<DummyPageHolder> dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
        m_popupMenuClient.setOwnerElement(HTMLSelectElement::create(dummyPageHolder->document()));
    }

    TestPopupMenuClient m_popupMenuClient;
};

TEST_F(ExternalPopupMenuDisplayNoneItemsTest, PopupMenuInfoSizeTest)
{
    WebPopupMenuInfo info;
    ExternalPopupMenu::getPopupMenuInfo(info, m_popupMenuClient);
    EXPECT_EQ(5U, info.items.size());
}

TEST_F(ExternalPopupMenuDisplayNoneItemsTest, IndexMappingTest)
{
    // 6th indexed item in popupmenu would be the 4th item in ExternalPopupMenu,
    // and vice-versa.
    EXPECT_EQ(4, ExternalPopupMenu::toExternalPopupMenuItemIndex(6, m_popupMenuClient));
    EXPECT_EQ(6, ExternalPopupMenu::toPopupMenuItemIndex(4, m_popupMenuClient));

    // Invalid index, methods should return -1.
    EXPECT_EQ(-1, ExternalPopupMenu::toExternalPopupMenuItemIndex(8, m_popupMenuClient));
    EXPECT_EQ(-1, ExternalPopupMenu::toPopupMenuItemIndex(8, m_popupMenuClient));
}

class ExternalPopupMenuWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    WebExternalPopupMenu* createExternalPopupMenu(const WebPopupMenuInfo&, WebExternalPopupMenuClient*) override
    {
        return &m_mockWebExternalPopupMenu;
    }
    WebRect shownBounds() const
    {
        return m_mockWebExternalPopupMenu.shownBounds();
    }
private:
    class MockWebExternalPopupMenu : public WebExternalPopupMenu {
        void show(const WebRect& bounds) override
        {
            m_shownBounds = bounds;
        }
        void close() override { }

    public:
        WebRect shownBounds() const
        {
            return m_shownBounds;
        }

    private:
        WebRect m_shownBounds;
    };
    WebRect m_shownBounds;
    MockWebExternalPopupMenu m_mockWebExternalPopupMenu;
};

class ExternalPopupMenuTest : public testing::Test {
public:
    ExternalPopupMenuTest() : m_baseURL("http://www.test.com") { }

protected:
    void SetUp() override
    {
        m_helper.initialize(false, &m_webFrameClient, &m_webViewClient);
        webView()->setUseExternalPopupMenus(true);
    }
    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    void registerMockedURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLLoad(URLTestHelpers::toKURL(m_baseURL + fileName), WebString::fromUTF8(fileName.c_str()), WebString::fromUTF8("popup/"), WebString::fromUTF8("text/html"));
    }

    void loadFrame(const std::string& fileName)
    {
        FrameTestHelpers::loadFrame(mainFrame(), m_baseURL + fileName);
    }

    WebViewImpl* webView() const { return m_helper.webViewImpl(); }
    const ExternalPopupMenuWebFrameClient& client() const { return m_webFrameClient; }
    WebLocalFrameImpl* mainFrame() const { return m_helper.webViewImpl()->mainFrameImpl(); }

private:
    std::string m_baseURL;
    FrameTestHelpers::TestWebViewClient m_webViewClient;
    ExternalPopupMenuWebFrameClient m_webFrameClient;
    FrameTestHelpers::WebViewHelper m_helper;
};

TEST_F(ExternalPopupMenuTest, PopupAccountsForPinchViewportOffset)
{
    registerMockedURLLoad("select_mid_screen.html");
    loadFrame("select_mid_screen.html");

    webView()->resize(WebSize(100, 100));
    webView()->layout();

    HTMLSelectElement* select = toHTMLSelectElement(mainFrame()->frame()->document()->getElementById("select"));
    LayoutMenuList* menuList = toLayoutMenuList(select->layoutObject());
    ASSERT_TRUE(menuList);

    PinchViewport& pinchViewport = webView()->page()->frameHost().pinchViewport();

    IntRect rectInDocument = menuList->absoluteBoundingBoxRect();

    webView()->setPageScaleFactor(2);
    IntPoint scrollDelta(20, 30);
    pinchViewport.move(scrollDelta);

    menuList->showPopup();

    EXPECT_EQ(rectInDocument.x() - scrollDelta.x(), client().shownBounds().x);
    EXPECT_EQ(rectInDocument.y() - scrollDelta.y(), client().shownBounds().y);
}

TEST_F(ExternalPopupMenuTest, DidAcceptIndex)
{
    registerMockedURLLoad("select.html");
    loadFrame("select.html");

    HTMLSelectElement* select = toHTMLSelectElement(mainFrame()->frame()->document()->getElementById("select"));
    LayoutMenuList* menuList = toLayoutMenuList(select->layoutObject());
    ASSERT_TRUE(menuList);

    menuList->showPopup();
    ASSERT_TRUE(menuList->popupIsVisible());

    WebExternalPopupMenuClient* client = static_cast<ExternalPopupMenu*>(menuList->popup());
    client->didAcceptIndex(2);
    EXPECT_FALSE(menuList->popupIsVisible());
    ASSERT_STREQ("2", menuList->text().utf8().data());
    EXPECT_EQ(2, select->selectedIndex());
}

TEST_F(ExternalPopupMenuTest, DidAcceptIndices)
{
    registerMockedURLLoad("select.html");
    loadFrame("select.html");

    HTMLSelectElement* select = toHTMLSelectElement(mainFrame()->frame()->document()->getElementById("select"));
    LayoutMenuList* menuList = toLayoutMenuList(select->layoutObject());
    ASSERT_TRUE(menuList);

    menuList->showPopup();
    ASSERT_TRUE(menuList->popupIsVisible());

    WebExternalPopupMenuClient* client = static_cast<ExternalPopupMenu*>(menuList->popup());
    int indices[] = { 2 };
    WebVector<int> indicesVector(indices, 1);
    client->didAcceptIndices(indicesVector);
    EXPECT_FALSE(menuList->popupIsVisible());
    EXPECT_STREQ("2", menuList->text().utf8().data());
    EXPECT_EQ(2, select->selectedIndex());
}

TEST_F(ExternalPopupMenuTest, DidAcceptIndicesClearSelect)
{
    registerMockedURLLoad("select.html");
    loadFrame("select.html");

    HTMLSelectElement* select = toHTMLSelectElement(mainFrame()->frame()->document()->getElementById("select"));
    LayoutMenuList* menuList = toLayoutMenuList(select->layoutObject());
    ASSERT_TRUE(menuList);

    menuList->showPopup();
    ASSERT_TRUE(menuList->popupIsVisible());

    WebExternalPopupMenuClient* client = static_cast<ExternalPopupMenu*>(menuList->popup());
    WebVector<int> indices;
    client->didAcceptIndices(indices);
    EXPECT_FALSE(menuList->popupIsVisible());
    EXPECT_EQ(-1, select->selectedIndex());
}

} // namespace blink
