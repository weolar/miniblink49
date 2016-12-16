/*
* Copyright (C) 2011 Apple Inc. All rights reserved.
* Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#ifndef PopupMenuWin_h
#define PopupMenuWin_h

#include "third_party/WebKit/public/web/WebTextDirection.h"
#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/public/web/WebPopupType.h"
#include "skia/ext/platform_canvas.h"

namespace blink {
class WebPagePopupImpl;
class WebFrameClient;
class WebViewImpl;
class WebPagePopup;
}

namespace cc {
class LayerTreeHost;
}

namespace content {

class PlatformEventHandler;

class PopupMenuWin : public blink::WebViewClient {
public:
    static blink::WebWidget* create(HWND hWnd, blink::IntPoint offset, blink::WebViewImpl* webViewImpl, blink::WebPopupType type, PopupMenuWin** result);
    virtual void PopupMenuWin::closeWidgetSoon() override;

    HWND popupHandle() const { return m_popup; }

    // WebWidgetClient
    virtual void didInvalidateRect(const blink::WebRect&) override;
    virtual void didAutoResize(const blink::WebSize& newSize) override;
    virtual void didUpdateLayoutSize(const blink::WebSize& newSize) override;
    virtual void scheduleAnimation() override;
    virtual void setWindowRect(const blink::WebRect&) override;
    virtual blink::WebLayerTreeView* layerTreeView() override;
    virtual void show(blink::WebNavigationPolicy) override;

protected:
    PopupMenuWin(HWND hWnd, blink::IntPoint offset, blink::WebViewImpl* webViewImpl);
    blink::WebWidget* PopupMenuWin::createWnd();
    void updataSize();
    void updataPaint();
    void initialize();
    bool initSetting();
    
    void registerClass();
    void paint(HDC hdc, RECT rcPaint);
    void beginMainFrame();
    static LRESULT CALLBACK PopupMenuWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    HWND m_popup;
    skia::PlatformCanvas* m_memoryCanvas;
    blink::IntRect m_rect;
    bool m_needsCommit;
    bool m_hasResize;
    bool m_needResize;
    bool m_initialize;
    double m_lastFrameTimeMonotonic;
    blink::WebPagePopup* m_popupImpl;
    cc::LayerTreeHost* m_layerTreeHost;
    blink::WebFrameClient* m_webFrameClient;
    blink::WebViewImpl* m_webViewImpl;
    HWND m_hParentWnd;
	blink::IntPoint m_offset;
    PlatformEventHandler* m_platformEventHandler;
};

}

#endif // PopupMenuWin_h