/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "web/ExternalPopupMenu.h"

#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/html/forms/PopupMenuClient.h"
#include "core/page/Page.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/IntPoint.h"
#include "platform/text/TextDirection.h"
#include "public/platform/WebVector.h"
#include "public/web/WebExternalPopupMenu.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebMenuItemInfo.h"
#include "public/web/WebPopupMenuInfo.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

ExternalPopupMenu::ExternalPopupMenu(LocalFrame& frame, PopupMenuClient* popupMenuClient, WebViewImpl& webView)
    : m_popupMenuClient(popupMenuClient)
    , m_localFrame(frame)
    , m_webView(webView)
    , m_dispatchEventTimer(this, &ExternalPopupMenu::dispatchEvent)
    , m_webExternalPopupMenu(0)
{
}

ExternalPopupMenu::~ExternalPopupMenu()
{
}

DEFINE_TRACE(ExternalPopupMenu)
{
    visitor->trace(m_localFrame);
    PopupMenu::trace(visitor);
}

void ExternalPopupMenu::show(const FloatQuad& controlPosition, const IntSize&, int index)
{
    IntRect rect(controlPosition.enclosingBoundingBox());
    // WebCore reuses the PopupMenu of an element.
    // For simplicity, we do recreate the actual external popup everytime.
    if (m_webExternalPopupMenu) {
        m_webExternalPopupMenu->close();
        m_webExternalPopupMenu = 0;
    }

    WebPopupMenuInfo info;
    getPopupMenuInfo(info, *m_popupMenuClient);
    if (info.items.isEmpty())
        return;
    WebLocalFrameImpl* webframe = WebLocalFrameImpl::fromFrame(m_localFrame.get());
    m_webExternalPopupMenu = webframe->client()->createExternalPopupMenu(info, this);
    if (m_webExternalPopupMenu) {
        IntRect rectInViewport = m_localFrame->view()->soonToBeRemovedContentsToUnscaledViewport(rect);
        m_webExternalPopupMenu->show(rectInViewport);
#if OS(MACOSX)
        const WebInputEvent* currentEvent = WebViewImpl::currentInputEvent();
        if (currentEvent && currentEvent->type == WebInputEvent::MouseDown) {
            m_syntheticEvent = adoptPtr(new WebMouseEvent);
            *m_syntheticEvent = *static_cast<const WebMouseEvent*>(currentEvent);
            m_syntheticEvent->type = WebInputEvent::MouseUp;
            m_dispatchEventTimer.startOneShot(0, FROM_HERE);
            // FIXME: show() is asynchronous. If preparing a popup is slow and
            // a user released the mouse button before showing the popup,
            // mouseup and click events are correctly dispatched. Dispatching
            // the synthetic mouseup event is redundant in this case.
        }
#endif
    } else {
        // The client might refuse to create a popup (when there is already one pending to be shown for example).
        didCancel();
    }
}

void ExternalPopupMenu::dispatchEvent(Timer<ExternalPopupMenu>*)
{
    m_webView.handleInputEvent(*m_syntheticEvent);
    m_syntheticEvent.clear();
}

void ExternalPopupMenu::hide()
{
    if (m_popupMenuClient)
        m_popupMenuClient->popupDidHide();
    if (!m_webExternalPopupMenu)
        return;
    m_webExternalPopupMenu->close();
    m_webExternalPopupMenu = 0;
}

void ExternalPopupMenu::updateFromElement()
{
}

void ExternalPopupMenu::disconnectClient()
{
    hide();
    m_popupMenuClient = 0;
}

void ExternalPopupMenu::didChangeSelection(int index)
{
    if (m_popupMenuClient)
        m_popupMenuClient->selectionChanged(toPopupMenuItemIndex(index, *m_popupMenuClient));
}

void ExternalPopupMenu::didAcceptIndex(int index)
{
    // Calling methods on the PopupMenuClient might lead to this object being
    // derefed. This ensures it does not get deleted while we are running this
    // method.
    int popupMenuItemIndex = toPopupMenuItemIndex(index, *m_popupMenuClient);
    RefPtrWillBeRawPtr<ExternalPopupMenu> guard(this);

    if (m_popupMenuClient) {
        m_popupMenuClient->popupDidHide();
        m_popupMenuClient->valueChanged(popupMenuItemIndex);
    }
    m_webExternalPopupMenu = 0;
}

void ExternalPopupMenu::didAcceptIndices(const WebVector<int>& indices)
{
    if (!m_popupMenuClient) {
        m_webExternalPopupMenu = 0;
        return;
    }

    // Calling methods on the PopupMenuClient might lead to this object being
    // derefed. This ensures it does not get deleted while we are running this
    // method.
    RefPtrWillBeRawPtr<ExternalPopupMenu> protect(this);

    m_popupMenuClient->popupDidHide();

    if (!indices.size())
        m_popupMenuClient->valueChanged(static_cast<unsigned>(-1), true);
    else {
        for (size_t i = 0; i < indices.size(); ++i)
            m_popupMenuClient->listBoxSelectItem(toPopupMenuItemIndex(indices[i], *m_popupMenuClient), (i > 0), false, (i == indices.size() - 1));
    }

    m_webExternalPopupMenu = 0;
}

void ExternalPopupMenu::didCancel()
{
    // See comment in didAcceptIndex on why we need this.
    RefPtrWillBeRawPtr<ExternalPopupMenu> guard(this);

    if (m_popupMenuClient)
        m_popupMenuClient->popupDidHide();
    m_webExternalPopupMenu = 0;
}

void ExternalPopupMenu::getPopupMenuInfo(WebPopupMenuInfo& info, PopupMenuClient& popupMenuClient)
{
    int itemCount = popupMenuClient.listSize();
    int count = 0;
    Vector<WebMenuItemInfo> items(static_cast<size_t>(itemCount));
    for (int i = 0; i < itemCount; ++i) {
        PopupMenuStyle style = popupMenuClient.itemStyle(i);
        if (style.isDisplayNone())
            continue;

        WebMenuItemInfo& popupItem = items[count++];
        popupItem.label = popupMenuClient.itemText(i);
        popupItem.toolTip = popupMenuClient.itemToolTip(i);
        if (popupMenuClient.itemIsSeparator(i))
            popupItem.type = WebMenuItemInfo::Separator;
        else if (popupMenuClient.itemIsLabel(i))
            popupItem.type = WebMenuItemInfo::Group;
        else
            popupItem.type = WebMenuItemInfo::Option;
        popupItem.enabled = popupMenuClient.itemIsEnabled(i);
        popupItem.checked = popupMenuClient.itemIsSelected(i);
        popupItem.textDirection = toWebTextDirection(style.textDirection());
        popupItem.hasTextDirectionOverride = style.hasTextDirectionOverride();
    }

    info.itemHeight = popupMenuClient.menuStyle().font().fontMetrics().height();
    info.itemFontSize = static_cast<int>(popupMenuClient.menuStyle().font().fontDescription().computedSize());
    info.selectedIndex = toExternalPopupMenuItemIndex(popupMenuClient.selectedIndex(), popupMenuClient);
    info.rightAligned = popupMenuClient.menuStyle().textDirection() == RTL;
    info.allowMultipleSelection = popupMenuClient.multiple();
    if (count < itemCount)
        items.shrink(count);
    info.items = items;

}

int ExternalPopupMenu::toPopupMenuItemIndex(int externalPopupMenuItemIndex, PopupMenuClient& popupMenuClient)
{
    if (externalPopupMenuItemIndex < 0)
        return externalPopupMenuItemIndex;

    int itemCount = popupMenuClient.listSize();
    int indexTracker = 0;
    for (int i = 0; i < itemCount ; ++i) {
        if (popupMenuClient.itemStyle(i).isDisplayNone())
            continue;
        if (indexTracker++ == externalPopupMenuItemIndex)
            return i;
    }
    return -1;
}

int ExternalPopupMenu::toExternalPopupMenuItemIndex(int popupMenuItemIndex, PopupMenuClient& popupMenuClient)
{
    if (popupMenuItemIndex < 0)
        return popupMenuItemIndex;

    int itemCount = popupMenuClient.listSize();
    int indexTracker = 0;
    for (int i = 0; i < itemCount; ++i) {
        if (popupMenuClient.itemStyle(i).isDisplayNone())
            continue;
        if (popupMenuItemIndex == i)
            return indexTracker;
        ++indexTracker;
    }
    return -1;
}

} // namespace blink
