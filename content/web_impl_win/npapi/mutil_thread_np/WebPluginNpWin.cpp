/*
 * Copyright (C) 2006, 2007, 2008, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2008-2009 Torch Mobile, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "content/web_impl_win/npapi/WebPluginNp.h"
#include "content/web_impl_win/npapi/WebPluginImeWin.h"
#include "content/web_impl_win/npapi/PluginMessageThrottlerWin.h"
#include "content/web_impl_win/npapi/WebPluginUtil.h"
#include "content/web_impl_win/BitmapColor.h"
#include "content/browser/CheckReEnter.h"
#include "third_party/WebKit/Source/web/WebPluginContainerImpl.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/platform/graphics/Image.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "skia/ext/platform_canvas.h"
#include "wke/wkeGlobalVar.h"
#include "wtf/text/WTFStringUtil.h"

static inline HWND windowHandleForPageClient(HWND client)
{
    return client;
}

using std::min;
using namespace WTF;
using namespace blink;

namespace content {

const LPCWSTR kWebPluginViewClassName = L"WebPluginView";
const LPCWSTR kWebPluginViewProperty = L"WebPluginViewProperty";

// The code used to hook BeginPaint/EndPaint originally came from
// <http://www.fengyuan.com/article/wmprint.html>.
// Copyright (C) 2000 by Feng Yuan (www.fengyuan.com).

static unsigned beginPaintSysCall;
static BYTE* beginPaint;

static unsigned endPaintSysCall;
static BYTE* endPaint;

typedef HDC (__stdcall *PtrBeginPaint)(HWND, PAINTSTRUCT*);
typedef BOOL (__stdcall *PtrEndPaint)(HWND, const PAINTSTRUCT*);

HDC __stdcall WebPluginNp::hookedBeginPaint(HWND hWnd, PAINTSTRUCT* lpPaint)
{
    WebPluginNp* pluginView = reinterpret_cast<WebPluginNp*>(::GetPropW(hWnd, kWebPluginViewProperty));
    if (pluginView && pluginView->m_wmPrintHDC) {
        // We're secretly handling WM_PRINTCLIENT, so set up the PAINTSTRUCT so
        // that the plugin will paint into the HDC we provide.
        memset(lpPaint, 0, sizeof(PAINTSTRUCT));
        lpPaint->hdc = pluginView->m_wmPrintHDC;
        GetClientRect(hWnd, &lpPaint->rcPaint);
        return pluginView->m_wmPrintHDC;
    }

#if defined(_M_IX86)
    // Call through to the original BeginPaint.
    __asm   mov     eax, beginPaintSysCall
    __asm   push    lpPaint
    __asm   push    hWnd
    __asm   call    beginPaint
#else
    return BeginPaint(hWnd, lpPaint);
#endif
}

BOOL __stdcall WebPluginNp::hookedEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint)
{
    WebPluginNp* pluginView = reinterpret_cast<WebPluginNp*>(::GetPropW(hWnd, kWebPluginViewProperty));
    if (pluginView && pluginView->m_wmPrintHDC) {
        // We're secretly handling WM_PRINTCLIENT, so we don't have to do any
        // cleanup.
        return TRUE;
    }

#if defined (_M_IX86)
    // Call through to the original EndPaint.
    __asm   mov     eax, endPaintSysCall
    __asm   push    lpPaint
    __asm   push    hWnd
    __asm   call    endPaint
#else
    return EndPaint(hWnd, lpPaint);
#endif
}

static void hook(const char* module, const char* proc, unsigned& sysCallID, BYTE*& pProc, const void* pNewProc)
{
    // See <http://www.fengyuan.com/article/wmprint.html> for an explanation of
    // how this function works.

    HINSTANCE hMod = GetModuleHandleA(module);

    pProc = reinterpret_cast<BYTE*>(reinterpret_cast<ptrdiff_t>(GetProcAddress(hMod, proc)));

#if defined(_M_IX86)
    if (pProc[0] != 0xB8)
        return;

    // FIXME: Should we be reading the bytes one-by-one instead of doing an
    // unaligned read?
    sysCallID = *reinterpret_cast<unsigned*>(pProc + 1);

    DWORD flOldProtect;
    if (!VirtualProtect(pProc, 5, PAGE_EXECUTE_READWRITE, &flOldProtect))
        return;

    pProc[0] = 0xE9;
    *reinterpret_cast<unsigned*>(pProc + 1) = reinterpret_cast<intptr_t>(pNewProc) - reinterpret_cast<intptr_t>(pProc + 5);

    pProc += 5;
#else
    /* Disassembly of BeginPaint()
    00000000779FC5B0 4C 8B D1         mov         r10,rcx
    00000000779FC5B3 B8 17 10 00 00   mov         eax,1017h
    00000000779FC5B8 0F 05            syscall
    00000000779FC5BA C3               ret
    00000000779FC5BB 90               nop
    00000000779FC5BC 90               nop
    00000000779FC5BD 90               nop
    00000000779FC5BE 90               nop
    00000000779FC5BF 90               nop
    00000000779FC5C0 90               nop
    00000000779FC5C1 90               nop
    00000000779FC5C2 90               nop
    00000000779FC5C3 90               nop
    */
    // Check for the signature as in the above disassembly
    DWORD guard = 0xB8D18B4C;
    if (*reinterpret_cast<DWORD*>(pProc) != guard)
        return;

    DWORD flOldProtect;
    VirtualProtect(pProc, 12, PAGE_EXECUTE_READWRITE, & flOldProtect);
    pProc[0] = 0x48;    // mov rax, this
    pProc[1] = 0xb8;
    *(__int64*)(pProc+2) = (__int64)pNewProc;
    pProc[10] = 0xff;   // jmp rax
    pProc[11] = 0xe0;
#endif
}

static void setUpOffscreenPaintingHooks(HDC (__stdcall*hookedBeginPaint)(HWND, PAINTSTRUCT*), BOOL (__stdcall*hookedEndPaint)(HWND, const PAINTSTRUCT*))
{
    static bool haveHooked = false;
    if (haveHooked)
        return;
    haveHooked = true;

    // Most (all?) windowed plugins don't seem to respond to WM_PRINTCLIENT, so
    // we hook into BeginPaint/EndPaint to allow their normal WM_PAINT handling
    // to draw into a given HDC. Note that this hooking affects the entire
    // process.
    hook("user32.dll", "BeginPaint", beginPaintSysCall, beginPaint, reinterpret_cast<const void *>(reinterpret_cast<ptrdiff_t>(hookedBeginPaint)));
    hook("user32.dll", "EndPaint", endPaintSysCall, endPaint, reinterpret_cast<const void *>(reinterpret_cast<ptrdiff_t>(hookedEndPaint)));
}

static bool registerPluginView()
{
    static bool haveRegisteredWindowClass = false;
    if (haveRegisteredWindowClass)
        return true;

    haveRegisteredWindowClass = true;

    //ASSERT(WebCore::instanceHandle());

    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.hIconSm        = 0;
    wcex.style          = CS_DBLCLKS;
    wcex.lpfnWndProc    = ::DefWindowProcA;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = nullptr; // WebCore::instanceHandle();
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)COLOR_WINDOW;
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = kWebPluginViewClassName;

    return !!RegisterClassEx(&wcex);
}

LRESULT CALLBACK WebPluginNp::PluginViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WebPluginNp* pluginView = reinterpret_cast<WebPluginNp*>(GetProp(hWnd, kWebPluginViewProperty));
    return pluginView->wndProc(hWnd, message, wParam, lParam);
}

static bool isWindowsMessageUserGesture(UINT message)
{
    switch (message) {
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_KEYUP:
            return true;
        default:
            return false;
    }
}

static inline IntPoint contentsToNativeWindow(WebPluginContainer* pluginContainer, const IntPoint& point)
{
    return pluginContainer->localToRootFramePoint(point);
}

static inline IntRect contentsToNativeWindow(WebPluginContainer* pluginContainer, const IntRect& rect)
{
    IntPoint posXY(rect.x(), rect.y());
    IntPoint posOutXY = pluginContainer->localToRootFramePoint(posXY);
    return IntRect(posOutXY.x(), posOutXY.y(), rect.width(), rect.height());
}

LRESULT WebPluginNp::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // <rdar://5711136> Sometimes Flash will call SetCapture before creating
    // a full-screen window and will not release it, which causes the
    // full-screen window to never receive mouse events. We set/release capture
    // on mouse down/up before sending the event to the plug-in to prevent that.
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
            ::SetCapture(hWnd);
            break;
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            ::ReleaseCapture();
            break;
    }

    if (message == m_lastMessage &&
        m_plugin->quirks().contains(PluginQuirkDontCallWndProcForSameMessageRecursively) && 
        m_isCallingPluginWndProc)
        return 1;

    if (message == WM_USER + 1 &&
        m_plugin->quirks().contains(PluginQuirkThrottleWMUserPlusOneMessages)) {
        if (!m_messageThrottler)
            m_messageThrottler = adoptPtr(new PluginMessageThrottlerWin(this));

        m_messageThrottler->appendMessage(hWnd, message, wParam, lParam);
        return 0;
    }

    m_lastMessage = message;
    m_isCallingPluginWndProc = true;

    // If the plug-in doesn't explicitly support changing the pop-up state, we enable
    // popups for all user gestures.
    // Note that we need to pop the state in a timer, because the Flash plug-in 
    // pops up windows in response to a posted message.
    if (m_plugin->pluginFuncs()->version < NPVERS_HAS_POPUPS_ENABLED_STATE &&
        isWindowsMessageUserGesture(message) && !m_popPopupsStateTimer.isActive()) {

        pushPopupsEnabledState(true);

        m_popPopupsStateTimer.startOneShot(0, FROM_HERE);
    }

    if (message == WM_PRINTCLIENT) {
        // Most (all?) windowed plugins don't respond to WM_PRINTCLIENT, so we
        // change the message to WM_PAINT and rely on our hooked versions of
        // BeginPaint/EndPaint to make the plugin draw into the given HDC.
        message = WM_PAINT;
        m_wmPrintHDC = reinterpret_cast<HDC>(wParam);
    }

    // Call the plug-in's window proc.
    LRESULT result = ::CallWindowProc(m_pluginWndProc, hWnd, message, wParam, lParam);

    m_wmPrintHDC = 0;

    m_isCallingPluginWndProc = false;

    return result;
}

static void subtractRectanglesFromRegion(HRGN hrgn, const blink::WebVector<blink::WebRect>& cutouts)
{
    if (0 == cutouts.size())
        return;

    HRGN cutout = ::CreateRectRgn(0, 0, 0, 0);
    for (size_t i = 0; i < cutouts.size(); i++) {
        blink::IntRect r = cutouts[i];
        ::SetRectRgn(cutout, r.x(), r.y(), r.maxX(), r.maxY());
        ::CombineRgn(hrgn, hrgn, cutout, RGN_DIFF);
    }
    ::DeleteObject(cutout);
}

void WebPluginNp::setRgnIfNeeded(const blink::IntRect& windowRect, const blink::IntRect& oldWindowRect, const blink::IntRect& oldClipRect)
{
    if (!m_plugin)
        return;

    if (!m_cutOutsRectsDirty && (m_haveUpdatedPluginWidget && m_windowRect == oldWindowRect && m_clipRect == oldClipRect))
        return;

    m_cutOutsRectsDirty = false;
    setCallingPlugin(true);

    // To prevent flashes while scrolling, we disable drawing during the window
    // update process by clipping the window to the zero rect.
    bool clipToZeroRect = !m_plugin->quirks().contains(PluginQuirkDontClipToZeroRectWhenScrolling);

    if (platformPluginWidget()) {
        if (clipToZeroRect) {
            HRGN rgn = ::CreateRectRgn(0, 0, 0, 0);
            ::SetWindowRgn(platformPluginWidget(), rgn, FALSE);
        } else {
            HRGN rgn = ::CreateRectRgn(m_clipRect.x(), m_clipRect.y(), m_clipRect.maxX(), m_clipRect.maxY());
            ::SetWindowRgn(platformPluginWidget(), rgn, TRUE);
        }

        if (!m_haveUpdatedPluginWidget || m_windowRect != oldWindowRect)
            ::MoveWindow(platformPluginWidget(), m_windowRect.x() + m_widgetOffset.x(), m_windowRect.y() + m_widgetOffset.y(), m_windowRect.width(), m_windowRect.height(), TRUE);

        if (clipToZeroRect) {
            HRGN rgn = ::CreateRectRgn(m_clipRect.x(), m_clipRect.y(), m_clipRect.maxX(), m_clipRect.maxY());
            if (wke::g_cutOutsNpapiRectsEnable)
                subtractRectanglesFromRegion(rgn, m_cutOutsRects);
            ::SetWindowRgn(platformPluginWidget(), rgn, TRUE);
        }
    }

    setNPWindowRect(windowRect);
    setCallingPlugin(false);
    m_haveUpdatedPluginWidget = true;
}

void WebPluginNp::updatePluginWidget(const IntRect& windowRect, const IntRect& clipRect, bool isVisible)
{
    CheckReEnter enterContent(nullptr);

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container->parent())
        return;

    FrameView* frameView = toFrameView(container->parent());
    if (!frameView)
        return;

    IntRect oldWindowRect = m_windowRect;
    IntRect oldClipRect = m_clipRect;

#if 0
    m_windowRect = IntRect(frameView->contentsToRootFrame(frameView->frameRect().location()), frameView->frameRect().size());
    m_clipRect = frameView->windowClipRect();
    m_clipRect.move(-m_windowRect.x(), -m_windowRect.y());
#else    
    m_windowRect = windowRect;
    m_clipRect = clipRect;
#endif

    setRgnIfNeeded(windowRect, oldWindowRect, oldClipRect);
    
    ::ShowWindow(platformPluginWidget(), isVisible ? SW_SHOWNA : SW_HIDE);

    if (m_windowRect != oldWindowRect || m_clipRect != oldClipRect) {
        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_memoryCanvas = skia::CreatePlatformCanvas(m_windowRect.width(), m_windowRect.height(), !m_isTransparent);
    }
}

bool WebPluginNp::dispatchNPEvent(NPEvent& npEvent)
{
    if (!m_plugin->pluginFuncs()->event)
        return true;

    bool shouldPop = false;

    if (m_plugin->pluginFuncs()->version < NPVERS_HAS_POPUPS_ENABLED_STATE && isWindowsMessageUserGesture(npEvent.event)) {
        pushPopupsEnabledState(true);
        shouldPop = true;
    }

    setCallingPlugin(true);

    WebPluginNp* self = this;
    bool accepted = true;

    NPEvent* npEventCopy = new NPEvent();
    *npEventCopy = npEvent;
    int id = m_id;
    WebPluginUtil::get()->postTaskToNpThread(FROM_HERE, [self, id, npEventCopy] {
        if (WebPluginUtil::isLive(id))
            self->m_plugin->pluginFuncs()->event(self->m_instance, npEventCopy);
        delete npEventCopy;
    });

    setCallingPlugin(false);

    if (shouldPop) 
        popPopupsEnabledState();

    return accepted;
}

bool WebPluginNp::supportsKeyboardFocus() const
{ 
    return true; 
}

bool WebPluginNp::supportsEditCommands() const
{
    return true; 
}

void WebPluginNp::updateGeometry(
    const blink::WebRect& windowRect, const blink::WebRect& clipRect,
    const blink::WebRect& unobscuredRect, const blink::WebVector<blink::WebRect>& cutOutsRects,
    bool isVisible)
{
    m_cutOutsRects.clear();
    for (size_t i = 0; i < cutOutsRects.size(); ++i) {
        m_cutOutsRects.append(cutOutsRects[i]);
    }
    
    m_cutOutsRectsDirty = true;
    updatePluginWidget(windowRect, clipRect, isVisible);
}

void WebPluginNp::updateFocus(bool focused, blink::WebFocusType)
{
    if (!m_plugin)
        return;

    NPEvent* focusEvent = new NPEvent();
    focusEvent->event = focused ? WM_SETFOCUS : WM_KILLFOCUS;
    focusEvent->wParam = 0;
    focusEvent->lParam = 0;

    if (!m_pluginIme && m_plugin->quirks().contains(PluginQuirkEmulateIme))
        m_pluginIme = new WebPluginIMEWin();

    WebPluginNp* self = this;
    int id = m_id;
    WebPluginUtil::get()->postTaskToNpThread(FROM_HERE, [self, id, focusEvent] {
        if (WebPluginUtil::isLive(id))
            self->m_plugin->pluginFuncs()->event(self->m_instance, focusEvent);
        delete focusEvent;
    });
}

void WebPluginNp::asynSetPlatformPluginWidgetVisibilityTimerFired(blink::Timer<WebPluginNp>*)
{
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    bool isVisibility = container->isVisible();
    if (isVisibility) {
        if (platformPluginWidget())
            ::ShowWindow(platformPluginWidget(), SW_SHOWNA);
    } else {
        if (platformPluginWidget())
            ::ShowWindow(platformPluginWidget(), SW_HIDE);
    }
}

void WebPluginNp::updateVisibility(bool isVisibility)
{
    if (!m_setPlatformPluginWidgetVisibilityTimer.isActive())
        m_setPlatformPluginWidgetVisibilityTimer.startOneShot(0, FROM_HERE);
}

bool WebPluginNp::acceptsInputEvents()
{
    return true;
}

bool WebPluginNp::handleMouseEvent(const blink::WebMouseEvent& evt)
{
    ASSERT(m_plugin && !m_isWindowed);

    bool isDefaultHandled = false;
    NPEvent npEvent;

    //blink::IntPoint p = contentsToNativeWindow(m_pluginContainer, blink::IntPoint(evt.x, evt.y));
    blink::IntPoint documentScrollOffsetRelativeToViewOrigin;// = contentsToNativeWindow(m_pluginContainer, blink::IntPoint());
    blink::IntPoint p(evt.windowX - documentScrollOffsetRelativeToViewOrigin.x(), evt.windowY - documentScrollOffsetRelativeToViewOrigin.y());

    npEvent.lParam = MAKELPARAM(p.x(), p.y());
    npEvent.wParam = 0;

    if (evt.modifiers & WebInputEvent::ControlKey)
        npEvent.wParam |= MK_CONTROL;
    if (evt.modifiers & WebInputEvent::ShiftKey)
        npEvent.wParam |= MK_SHIFT;

    if (evt.type == blink::WebInputEvent::Type::MouseMove
        || evt.type == blink::WebInputEvent::Type::MouseLeave
        || evt.type == blink::WebInputEvent::Type::MouseEnter) {
        npEvent.event = WM_MOUSEMOVE;
        if (evt.button != blink::WebMouseEvent::Button::ButtonNone) {
            switch (evt.button) {
            case blink::WebMouseEvent::Button::ButtonLeft:
                npEvent.wParam |= MK_LBUTTON;
                break;
            case blink::WebMouseEvent::Button::ButtonMiddle:
                npEvent.wParam |= MK_MBUTTON;
                break;
            case blink::WebMouseEvent::Button::ButtonRight:
                npEvent.wParam |= MK_RBUTTON;
                break;
            case blink::WebMouseEvent::Button::ButtonNone:
                break;
            }
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseDown) {
        focusPluginElement();

        switch (evt.button) {
        case blink::WebMouseEvent::Button::ButtonLeft:
            npEvent.event = WM_LBUTTONDOWN;
            break;
        case blink::WebMouseEvent::Button::ButtonMiddle:
            npEvent.event = WM_MBUTTONDOWN;
            break;
        case blink::WebMouseEvent::Button::ButtonRight:
            npEvent.event = WM_RBUTTONDOWN;
            break;
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseUp) {
        switch (evt.button) {
        case blink::WebMouseEvent::Button::ButtonLeft:
            npEvent.event = WM_LBUTTONUP;
            break;
        case blink::WebMouseEvent::Button::ButtonMiddle:
            npEvent.event = WM_MBUTTONUP;
            break;
        case blink::WebMouseEvent::Button::ButtonRight:
            npEvent.event = WM_RBUTTONUP;
            break;
        }
    } else if (evt.type == blink::WebInputEvent::Type::MouseWheel) {
        const blink::WebMouseWheelEvent& wheelEvt = static_cast<const blink::WebMouseWheelEvent&>(evt);
        npEvent.event = WM_MOUSEWHEEL;
        npEvent.wParam = MAKEWPARAM(wheelEvt.deltaX, wheelEvt.deltaY);
    } else
        return isDefaultHandled;

    // FIXME: Consider back porting the http://webkit.org/b/58108 fix here.
    if (dispatchNPEvent(npEvent))
        isDefaultHandled = true;
    
    // Currently, Widget::setCursor is always called after this function in EventHandler.cpp
    // and since we don't want that we set ignoreNextSetCursor to true here to prevent that.
    return isDefaultHandled;
}

bool WebPluginNp::handleKeyboardCharEventForEmulateIme(int windowsKeyCode)
{
    if (!m_pluginIme && m_plugin->quirks().contains(PluginQuirkEmulateIme))
        m_pluginIme = new WebPluginIMEWin();
    if (!m_pluginIme)
        return false;

    NPEvent npEvent;
    npEvent.wParam = windowsKeyCode;

    npEvent.event = WM_SETFOCUS;
    npEvent.wParam = 0;
    npEvent.lParam = 0;
    dispatchNPEvent(npEvent);

    wchar_t c = (wchar_t)windowsKeyCode;
    wchar_t text[2] = { c, 0 };
    std::vector<int> clauses;
    std::vector<int> target;
    clauses.push_back(0);
    target.push_back(0);
    m_pluginIme->compositionUpdated(text, clauses, target, 0);
    m_pluginIme->compositionCompleted(text);
    m_pluginIme->sendEvents(m_id, m_instance, m_plugin->pluginFuncs()->event);

    return true;
}

bool WebPluginNp::handleKeyboardEvent(const blink::WebKeyboardEvent& evt)
{
    if (!m_pluginIme && m_plugin->quirks().contains(PluginQuirkEmulateIme))
        m_pluginIme = new WebPluginIMEWin();
    WebPluginIMEWin::ScopedLock lock(m_pluginIme);

    ASSERT(m_plugin && !m_isWindowed);
    bool isDefaultHandled = false;
    NPEvent npEvent;

    npEvent.wParam = evt.windowsKeyCode;

    if (evt.type == blink::WebInputEvent::Type::KeyDown) {
        npEvent.event = WM_KEYDOWN;
        npEvent.lParam = 0;
    } else if (evt.type == blink::WebInputEvent::Type::Char) {
        if (!m_plugin->quirks().contains(PluginQuirkEmulateIme)) {
            npEvent.event = WM_CHAR;
            npEvent.lParam = 1;
            npEvent.wParam = evt.windowsKeyCode;
            dispatchNPEvent(npEvent);
        }
        isDefaultHandled = true;
    } else if (evt.type == blink::WebInputEvent::Type::KeyUp) {
        npEvent.event = WM_KEYUP;
        npEvent.lParam = 0x8000;
    } else
        return isDefaultHandled;

    if (dispatchNPEvent(npEvent))
        return true;
    return isDefaultHandled;
}

bool WebPluginNp::handleInputEvent(const blink::WebInputEvent& evt, blink::WebCursorInfo&)
{
    if (m_isWindowed)
        return false;

    if (blink::WebInputEvent::isMouseEventType(evt.type) || blink::WebInputEvent::MouseWheel == evt.type)
        return handleMouseEvent(static_cast<const blink::WebMouseEvent&>(evt));

    if (blink::WebInputEvent::isKeyboardEventType(evt.type))
        return handleKeyboardEvent(static_cast<const blink::WebKeyboardEvent&>(evt));

    return false;
}

bool WebPluginNp::getImeStatus(int* inputType, blink::IntRect* caretRect)
{
    if (!m_pluginIme)
        return false;
    return m_pluginIme->getStatus(inputType, caretRect);
}

static void captureWindow(HWND hWnd, HDC hdcDest)
{
    HDC hdcSrc = ::GetWindowDC(hWnd);

    RECT windowRect = { 0 };
    ::GetWindowRect(hWnd, &windowRect);

    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    ::BitBlt(hdcDest, 0, 0, width, height, hdcSrc, 0, 0, SRCCOPY);

    ::ReleaseDC(hWnd, hdcSrc);
}

void WebPluginNp::paintIntoTransformedContext(HDC hdc)
{
    if (m_isWindowed) {
        //captureWindow(platformPluginWidget(), hdc);
        //SendMessage(platformPluginWidget(), WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN | PRF_OWNED);
        return;
    }

    m_npWindow.type = NPWindowTypeDrawable;
    m_npWindow.window = hdc;

    WINDOWPOS windowpos = { 0, 0, 0, 0, 0, 0, 0 };

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    //IntRect r = contentsToNativeWindow(m_pluginContainer, container->frameRect());
    blink::IntPoint documentScrollOffsetRelativeToViewOrigin = contentsToNativeWindow(m_pluginContainer, blink::IntPoint());
    blink::IntRect r = container->frameRect();
    blink::IntRect frameRect = container->frameRect();

    // WM_WINDOWPOSCHANGED 存滚动的位置，setNPWindowRect设置原始位置
    // WM_WINDOWPOSCHANGED是给鼠标键盘消息定位用的，表示flash在原生窗口中的位置
    // setNPWindowRect表示绘制的目标在HDC的哪个坐标开始
    r.setX(documentScrollOffsetRelativeToViewOrigin.x());
    r.setY(documentScrollOffsetRelativeToViewOrigin.y());

    if (r.isEmpty())
        r = blink::IntRect(0, 0, 1, 1);

    windowpos.x = r.x();
    windowpos.y = r.y();
    
    windowpos.cx = r.width();
    windowpos.cy = r.height();

    NPEvent npEvent;
    npEvent.event = WM_WINDOWPOSCHANGED;
    npEvent.lParam = reinterpret_cast<uintptr_t>(&windowpos);
    npEvent.wParam = 0;

    dispatchNPEvent(npEvent);

    setNPWindowRect(frameRect);

    npEvent.event = WM_PAINT;
    npEvent.wParam = reinterpret_cast<uintptr_t>(hdc);

    // This is supposed to be a pointer to the dirty rect, but it seems that the Flash plugin
    // ignores it so we just pass null.
    npEvent.lParam = 0;

    dispatchNPEvent(npEvent);
}

void WebPluginNp::paintWindowedPluginIntoContext(GraphicsContext& context, const IntRect& rect)
{
    DebugBreak();
}

void WebPluginNp::paint(blink::WebCanvas* canvas, const blink::WebRect& rect)
{
    if (!m_isStarted) {
        // Draw the "missing plugin" image
        paintMissingPluginIcon(canvas, rect);
        return;
    }

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    // Ensure that we have called SetWindow before we try to paint.
    if (!m_haveCalledSetWindow) {
        blink::IntRect r = container->frameRect();
        if (r.isEmpty())
            r = rect;
        setNPWindowRect(r);
    }

    if (m_isWindowed)
        return;

    if (!m_memoryCanvas || !canvas) // start()里有可能这两其中一个为nullptr，看打开哪个宏
        return;

    SkPaint clearPaint;
    clearPaint.setARGB(0xff, 0xFF, 0xFF, 0xFF);
    clearPaint.setXfermodeMode(SkXfermode::kClear_Mode);

    SkRect skrc;
    blink::IntRect r = container->frameRect();
    skrc.set(0, 0, r.width(), r.height());
    m_memoryCanvas->drawRect(skrc, clearPaint);

    HDC hMemoryDC = skia::BeginPlatformPaint(m_parentWidget, m_memoryCanvas);

    // On Safari/Windows without transparency layers the GraphicsContext returns the HDC
    // of the window and the plugin expects that the passed in DC has window coordinates.
    XFORM originalTransform;
    XFORM transform;

    if (!m_isWindowed) {
        ::GetWorldTransform(hMemoryDC, &originalTransform);
        transform = originalTransform;
        transform.eDx = -r.x();
        transform.eDy = -r.y();
        ::SetWorldTransform(hMemoryDC, &transform);
    }

    paintIntoTransformedContext(hMemoryDC);

    if (!m_isWindowed)
        ::SetWorldTransform(hMemoryDC, &originalTransform);
    skia::EndPlatformPaint(m_memoryCanvas);

    SkBaseDevice* bitmapDevice = skia::GetTopDevice(*m_memoryCanvas);
    const SkBitmap& bitmap = bitmapDevice->accessBitmap(false);
    
    if (canvas != m_memoryCanvas)
        canvas->drawBitmap(bitmap, m_windowRect.x(), m_windowRect.y());
}

void WebPluginNp::setNPWindowRect(const IntRect& rect)
{
    if (!m_isStarted)
        return;

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    IntPoint p = container->localToRootFramePoint(rect.location());
    // windowless模式是直接画在rect.x，rect.y点的独立canvas
    m_npWindow.x = rect.x();
    m_npWindow.y = rect.y();

    m_npWindow.width = rect.width();
    m_npWindow.height = rect.height();

    m_npWindow.clipRect.right = rect.width();
    m_npWindow.clipRect.bottom = rect.height();
    m_npWindow.clipRect.left = 0;
    m_npWindow.clipRect.top = 0;

    if (m_plugin->pluginFuncs()->setwindow) {
        setCallingPlugin(true);

        WebPluginNp* self = this;
        int id = m_id;
        NPWindow* npWindow = new NPWindow();
        *npWindow = m_npWindow;
        WebPluginUtil::get()->postTaskToNpThread(FROM_HERE, [self, id, npWindow] {
            if (WebPluginUtil::isLive(id))
                self->m_plugin->pluginFuncs()->setwindow(self->m_instance, npWindow);
            delete npWindow;
        });

        setCallingPlugin(false);

        m_haveCalledSetWindow = true;

        if (!m_isWindowed)
            return;

        if (platformPluginWidget()) {
            WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtrW(platformPluginWidget(), GWLP_WNDPROC);
            if (currentWndProc != PluginViewWndProc)
                m_pluginWndProc = (WNDPROC)SetWindowLongPtrW(platformPluginWidget(), GWLP_WNDPROC, (LONG_PTR)PluginViewWndProc);
        }
    }
}

NPError WebPluginNp::handlePostReadFile(Vector<char>& buffer, uint32_t len, const char* buf)
{
    String filename(buf, len);

    if (filename.startsWith("file:///"))
        filename = filename.substring(8);

    // Get file info
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (GetFileAttributesExW(filename.charactersWithNullTermination().data(), GetFileExInfoStandard, &attrs) == 0)
        return NPERR_FILE_NOT_FOUND;

    if (attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return NPERR_FILE_NOT_FOUND;

    HANDLE fileHandle = CreateFileW(filename.charactersWithNullTermination().data(), FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    
    if (fileHandle == INVALID_HANDLE_VALUE)
        return NPERR_FILE_NOT_FOUND;

    buffer.resize(attrs.nFileSizeLow);

    DWORD bytesRead;
    int retval = ReadFile(fileHandle, buffer.data(), attrs.nFileSizeLow, &bytesRead, 0);

    CloseHandle(fileHandle);

    if (retval == 0 || bytesRead != attrs.nFileSizeLow)
        return NPERR_FILE_NOT_FOUND;

    return NPERR_NO_ERROR;
}

bool WebPluginNp::platformGetValueStatic(NPNVariable, void*, NPError*)
{
    return false;
}

bool WebPluginNp::platformGetValue(NPNVariable variable, void* value, NPError* result)
{
    switch (variable) {
        case NPNVnetscapeWindow: {
            HWND* w = reinterpret_cast<HWND*>(value);
            *w = windowHandleForPageClient(platformPluginWidget() ? platformPluginWidget() : parentPlatformPluginWidget());
            *result = NPERR_NO_ERROR;
            return true;
        }

        case NPNVSupportsWindowless: {
            NPBool* flag = reinterpret_cast<NPBool*>(value);
            *flag = TRUE; // weolar
            *result = NPERR_NO_ERROR;
            return true;
        }

    default:
        return false;
    }
}

void WebPluginNp::invalidateRect(const IntRect& rect)
{
    if (m_isWindowed) {
        RECT invalidRect = { rect.x(), rect.y(), rect.maxX(), rect.maxY() };
        ::InvalidateRect(platformPluginWidget(), &invalidRect, false);
        return;
    }

    invalidateWindowlessPluginRect(rect);
}

void WebPluginNp::invalidateRect(NPRect* rect)
{
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    if (!rect) {
        invalidateRect(container->boundsRect());
        return;
    }

    IntRect r(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);

    if (m_isWindowed) {
        RECT invalidRect = { r.x(), r.y(), r.maxX(), r.maxY() };
        ::InvalidateRect(platformPluginWidget(), &invalidRect, FALSE);
    } else {
        if (m_plugin->quirks().contains(PluginQuirkThrottleInvalidate)) {
            m_invalidRects.append(r);
            if (!m_invalidateTimer.isActive())
                m_invalidateTimer.startOneShot(0.001, FROM_HERE);
        } else
            invalidateRect(r);
    }
}

void WebPluginNp::invalidateRegion(NPRegion region)
{
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (m_isWindowed || !container)
        return;

    RECT r;

    if (GetRgnBox(region, &r) == 0) {
        invalidateRect(container->boundsRect());
        return;
    }

    IntRect rect(IntPoint(r.left, r.top), IntSize(r.right-r.left, r.bottom-r.top));
    invalidateRect(rect);
}

void WebPluginNp::forceRedraw()
{
    if (m_isWindowed)
        ::UpdateWindow(platformPluginWidget());
#if 0
     else
        ::UpdateWindow(windowHandleForPageClient(parent() ? parent()->hostWindow()->platformPageClient() : 0));
#endif
}

void WebPluginNp::platformStartImpl()
{
    if (m_asynStartTask)
        m_asynStartTask = nullptr;

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    if (m_isWindowed) {
        registerPluginView();
        setUpOffscreenPaintingHooks(hookedBeginPaint, hookedEndPaint);

        DWORD flags = WS_CHILD;
        if (container->isSelfVisible())
            flags |= WS_VISIBLE;

        flags |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

        HWND window = ::CreateWindowEx(0, kWebPluginViewClassName, 0, flags,
            0, 0, 0, 0, m_parentWidget, 0, nullptr, 0);

        setPlatformPluginWidget(window);

        // Calling SetWindowLongPtrA here makes the window proc ASCII, which is required by at least
        // the Shockwave Director plug-in.
#if CPU(X86_64)
        ::SetWindowLongPtrA(platformPluginWidget(), GWLP_WNDPROC, (LONG_PTR)DefWindowProcA);
#else
        ::SetWindowLongPtrA(platformPluginWidget(), GWL_WNDPROC, (LONG)DefWindowProcA);
#endif
        ::SetProp(platformPluginWidget(), kWebPluginViewProperty, this);

        m_npWindow.type = NPWindowTypeWindow;
        m_npWindow.window = platformPluginWidget();
    } else {
        m_npWindow.type = NPWindowTypeDrawable;
        m_npWindow.window = 0;
    }

    blink::IntRect r(0, 0, 1, 1);
    if (m_windowRect.isEmpty() && m_clipRect.isEmpty()) {
        updatePluginWidget(r, r, true);
        paint(m_memoryCanvas, r);
    } else    
        updatePluginWidget(m_windowRect, m_clipRect, true);
}

#define USING_ASYNC_START 0

void WebPluginNp::PlatformStartAsynTask::didProcessTask()
{
#if USING_ASYNC_START
    if (m_parentPtr)
        m_parentPtr->platformStartImpl(false);
#else
    if (m_parentPtr) {
        IntRect r(0, 0, 1, 1);
        m_parentPtr->paint(m_parentPtr->m_memoryCanvas, r);
    }
#endif
    blink::Platform::current()->currentThread()->removeTaskObserver(this);
    delete this;
}

bool WebPluginNp::platformStart()
{
    ASSERT(m_isStarted);
    ASSERT(m_status == PluginStatusLoadedSuccessfully);

    // 淘宝npaliedit控件需要同步调用，否则会因为setwindow没被调用到而在namedPropertyGetterCustom里崩溃
    WebPluginNp* self = this;
    sendTaskToNpThread(FROM_HERE, [self] {
        self->platformStartImpl();
    });

    return true;
}

static LRESULT CALLBACK platformDestroyNullWndProc(HWND, UINT, WPARAM, LPARAM)
{
    return 0;
}

static void platformDestroyWindow(PlatformPluginWidget widget)
{
    bool isUnicode = ::IsWindowUnicode(widget);
    if (!isUnicode)
        ::SetWindowLongPtrA(widget, GWLP_WNDPROC, (LONG_PTR)&platformDestroyNullWndProc);
    else
        ::SetWindowLongPtrW(widget, GWLP_WNDPROC, (LONG_PTR)&platformDestroyNullWndProc);
    
    ::DestroyWindow(widget);
}

void WebPluginNp::platformDestroy()
{
    if (m_memoryCanvas)
        delete m_memoryCanvas;
    m_memoryCanvas = nullptr;

    PlatformPluginWidget widget = platformPluginWidget();
    if (!widget)
        return;

    blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(platformDestroyWindow, widget));

    CheckReEnter::incrementEnterCount();
    ::ShowWindow(widget, SW_HIDE);
    setPlatformPluginWidget(0);
    CheckReEnter::decrementEnterCount();
}

bool WebPluginNp::setComposition(const blink::WebString& text, const blink::WebVector<blink::WebCompositionUnderline>& underlines, int selectionStart, int selectionEnd)
{
    if (!m_pluginIme && m_plugin->quirks().contains(PluginQuirkEmulateIme))
        m_pluginIme = new WebPluginIMEWin();
    if (!m_pluginIme)
        return false;

    std::vector<int> clauses;
    std::vector<int> target;
    for (size_t i = 0; i < underlines.size(); ++i) {
        clauses.push_back(underlines[i].startOffset);
        clauses.push_back(underlines[i].endOffset);
        if (underlines[i].thick) {
            target.clear();
            target.push_back(underlines[i].startOffset);
            target.push_back(underlines[i].endOffset);
        }
    }

    String textStr(text);
    Vector<UChar> str = WTF::ensureUTF16UChar(textStr, false);
    String textW(str.data(), str.size());

    m_pluginIme->compositionUpdated(textW, clauses, target, selectionEnd);
    m_pluginIme->sendEvents(m_id, m_instance, m_plugin->pluginFuncs()->event);

    return true;
}

bool WebPluginNp::confirmComposition(const blink::WebString& text, blink::WebWidget::ConfirmCompositionBehavior selectionBehavior)
{
    if (!m_pluginIme && m_plugin->quirks().contains(PluginQuirkEmulateIme))
        m_pluginIme = new WebPluginIMEWin();
    if (!m_pluginIme)
        return false;

    String textStr(text);
    Vector<UChar> str = WTF::ensureUTF16UChar(textStr, false);
    String textW(str.data(), str.size());
    m_pluginIme->compositionCompleted(textW);
    m_pluginIme->sendEvents(m_id, m_instance, m_plugin->pluginFuncs()->event);

    return true;
}

PassRefPtr<Image> WebPluginNp::snapshot()
{
// #if !USE(WINGDI)
//     auto hdc = adoptGDIObject(::CreateCompatibleDC(0));
// 
//     if (!m_isWindowed) {
//         // Enable world transforms.
//         SetGraphicsMode(hdc.get(), GM_ADVANCED);
// 
//         XFORM transform;
//         GetWorldTransform(hdc.get(), &transform);
// 
//         // Windowless plug-ins assume that they're drawing onto the view's DC.
//         // Translate the context so that the plug-in draws at (0, 0).
//         IntPoint position = downcast<FrameView>(*parent()).contentsToWindow(frameRect()).location();
//         transform.eDx = -position.x();
//         transform.eDy = -position.y();
//         SetWorldTransform(hdc.get(), &transform);
//     }
// 
//     void* bits;
//     BitmapInfo bmp = BitmapInfo::createBottomUp(frameRect().size());
//     auto hbmp = adoptGDIObject(::CreateDIBSection(0, &bmp, DIB_RGB_COLORS, &bits, 0, 0));
// 
//     HBITMAP hbmpOld = static_cast<HBITMAP>(SelectObject(hdc.get(), hbmp.get()));
// 
//     paintIntoTransformedContext(hdc.get());
// 
//     SelectObject(hdc.get(), hbmpOld);
// 
//     return BitmapImage::create(hbmp.get());
// #else
     return nullptr;
// #endif
}

} // namespace WebCore
