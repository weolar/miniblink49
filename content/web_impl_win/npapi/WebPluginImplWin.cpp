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
#include "content/web_impl_win/npapi/WebPluginImpl.h"

#include "content/web_impl_win/npapi/PluginMessageThrottlerWin.h"
#include "content/browser/CheckReEnter.h"
#include "third_party/WebKit/Source/web/WebPluginContainerImpl.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/platform/graphics/Image.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "skia/ext/platform_canvas.h"

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

typedef HDC (WINAPI *PtrBeginPaint)(HWND, PAINTSTRUCT*);
typedef BOOL (WINAPI *PtrEndPaint)(HWND, const PAINTSTRUCT*);

#if CPU(X86_64)
//extern "C" HDC __stdcall _HBeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
//extern "C" BOOL __stdcall _HEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint);
#endif

HDC WINAPI WebPluginImpl::hookedBeginPaint(HWND hWnd, PAINTSTRUCT* lpPaint)
{
    WebPluginImpl* pluginView = reinterpret_cast<WebPluginImpl*>(GetProp(hWnd, kWebPluginViewProperty));
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

BOOL WINAPI WebPluginImpl::hookedEndPaint(HWND hWnd, const PAINTSTRUCT* lpPaint)
{
    WebPluginImpl* pluginView = reinterpret_cast<WebPluginImpl*>(GetProp(hWnd, kWebPluginViewProperty));
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

static void setUpOffscreenPaintingHooks(HDC (WINAPI*hookedBeginPaint)(HWND, PAINTSTRUCT*), BOOL (WINAPI*hookedEndPaint)(HWND, const PAINTSTRUCT*))
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
    wcex.lpfnWndProc    = DefWindowProc;
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

LRESULT CALLBACK WebPluginImpl::PluginViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WebPluginImpl* pluginView = reinterpret_cast<WebPluginImpl*>(GetProp(hWnd, kWebPluginViewProperty));

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

    IntPoint posMaxXY(rect.maxX(), rect.maxY());
    IntPoint posMaxOutXY = pluginContainer->localToRootFramePoint(posMaxXY);
    return IntRect(posOutXY.x(), posOutXY.y(), posOutXY.x() + posMaxOutXY.x(), posOutXY.y() + posMaxOutXY.y());
}

LRESULT WebPluginImpl::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

void WebPluginImpl::updatePluginWidget(const IntRect& windowRect, const IntRect& clipRect)
{
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
    if (platformPluginWidget() && (!m_haveUpdatedPluginWidget || m_windowRect != oldWindowRect || m_clipRect != oldClipRect)) {
        setCallingPlugin(true);

        // To prevent flashes while scrolling, we disable drawing during the window
        // update process by clipping the window to the zero rect.

        bool clipToZeroRect = !m_plugin->quirks().contains(PluginQuirkDontClipToZeroRectWhenScrolling);

        if (clipToZeroRect) {
            HRGN rgn = (::CreateRectRgn(0, 0, 0, 0));
            ::SetWindowRgn(platformPluginWidget(), rgn, FALSE);
            ::DeleteObject(rgn);
        } else {
            HRGN rgn = (::CreateRectRgn(m_clipRect.x(), m_clipRect.y(), m_clipRect.maxX(), m_clipRect.maxY()));
            ::SetWindowRgn(platformPluginWidget(), rgn, TRUE);
            ::DeleteObject(rgn);
        }

        if (!m_haveUpdatedPluginWidget || m_windowRect != oldWindowRect)
            ::MoveWindow(platformPluginWidget(), m_windowRect.x() + m_widgetOffset.x(), m_windowRect.y() + m_widgetOffset.y(), m_windowRect.width(), m_windowRect.height(), TRUE);

        if (clipToZeroRect) {
            HRGN rgn = (::CreateRectRgn(m_clipRect.x(), m_clipRect.y(), m_clipRect.maxX(), m_clipRect.maxY()));
            ::SetWindowRgn(platformPluginWidget(), rgn, TRUE);
            ::DeleteObject(rgn);
        }

        setCallingPlugin(false);

        m_haveUpdatedPluginWidget = true;
    }

    if (m_windowRect != oldWindowRect || m_clipRect != oldClipRect) {
        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_memoryCanvas = skia::CreatePlatformCanvas(m_windowRect.width(), m_windowRect.height(), !m_isTransparent);
    }
}

bool WebPluginImpl::dispatchNPEvent(NPEvent& npEvent)
{
    if (!m_plugin->pluginFuncs()->event)
        return true;

    bool shouldPop = false;

    if (m_plugin->pluginFuncs()->version < NPVERS_HAS_POPUPS_ENABLED_STATE && isWindowsMessageUserGesture(npEvent.event)) {
        pushPopupsEnabledState(true);
        shouldPop = true;
    }

    //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
    setCallingPlugin(true);
    bool accepted = !m_plugin->pluginFuncs()->event(m_instance, &npEvent);
    setCallingPlugin(false);

    if (shouldPop) 
        popPopupsEnabledState();

    return accepted;
}

bool WebPluginImpl::supportsKeyboardFocus() const { return true; }
bool WebPluginImpl::supportsEditCommands() const { return true; }

// void WebPluginImpl::movePluginWindowsHelper(HWND parent, const std::vector<WebPluginGeometry>& moves,
//     const blink::WebRect& windowRect, const blink::WebRect& clipRect, ) {
//     if (moves.empty())
//         return;
// 
//     bool oop_plugins = !base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kSingleProcess);
// 
//     HDWP defer_window_pos_info =
//         ::BeginDeferWindowPos(static_cast<int>(moves.size()));
// 
//     if (!defer_window_pos_info) {
//         NOTREACHED();
//         return;
//     }
// 
// #if defined(USE_AURA)
//     std::vector<RECT> invalidate_rects;
// #endif
// 
//     int failure_count = 0;
//     for (int i = 0; i < (int)moves.size(); ++i) {
//         unsigned long flags = 0;
//         const WebPluginGeometry& move = moves[i];
//         HWND window = move.window;
// 
//         // As the plugin parent window which lives on the browser UI thread is
//         // destroyed asynchronously, it is possible that we have a stale window
//         // sent in by the renderer for moving around.
//         // Note: get the parent before checking if the window is valid, to avoid a
//         // race condition where the window is destroyed after the check but before
//         // the GetParent call.
//         HWND cur_parent = ::GetParent(window);
//         if (!::IsWindow(window))
//             continue;
// 
//         if (!PluginServiceImpl::GetInstance()->IsPluginWindow(window)) {
//             // The renderer should only be trying to move plugin windows. However,
//             // this may happen as a result of a race condition (i.e. even after the
//             // check right above), so we ignore it.
//             continue;
//         }
// 
//         if (oop_plugins) {
//             if (cur_parent == GetDesktopWindow()) {
//                 // The plugin window hasn't been parented yet, add an intermediate
//                 // window that lives on this thread to speed up scrolling. Note this
//                 // only works with out of process plugins since we depend on
//                 // PluginProcessHost to destroy the intermediate HWNDs.
//                 cur_parent = ReparentWindow(window, parent);
//                 ::ShowWindow(window, SW_SHOW);  // Window was created hidden.
//             }
//             else if (!IsPluginWrapperWindow(cur_parent)) {
//                 continue;  // Race if plugin process is shutting down.
//             }
// 
//             // We move the intermediate parent window which doesn't result in cross-
//             // process synchronous Windows messages.
//             window = cur_parent;
//         }
//         else {
//             if (cur_parent == GetDesktopWindow())
//                 SetParent(window, parent);
//         }
// 
//         if (move.visible)
//             flags |= SWP_SHOWWINDOW;
//         else
//             flags |= SWP_HIDEWINDOW;
// 
// #if defined(USE_AURA)
//         if (GpuDataManagerImpl::GetInstance()->CanUseGpuBrowserCompositor()) {
//             // Without this flag, Windows repaints the parent area uncovered by this
//             // move. However when software compositing is used the clipping region is
//             // ignored. Since in Aura the browser chrome could be under the plugin, if
//             // if Windows tries to paint it synchronously inside EndDeferWindowsPos
//             // then it won't have the data and it will flash white. So instead we
//             // manually redraw the plugin.
//             // Why not do this for native Windows? Not sure if there are any
//             // performance issues with this.
//             flags |= SWP_NOREDRAW;
//         }
// #endif
// 
//         if (move.rects_valid) {
//             gfx::Rect clip_rect_in_pixel = gfx::win::DIPToScreenRect(move.clip_rect);
//             HRGN hrgn = ::CreateRectRgn(clip_rect_in_pixel.x(),
//                 clip_rect_in_pixel.y(),
//                 clip_rect_in_pixel.right(),
//                 clip_rect_in_pixel.bottom());
//             gfx::SubtractRectanglesFromRegion(hrgn, move.cutout_rects);
// 
//             // Note: System will own the hrgn after we call SetWindowRgn,
//             // so we don't need to call DeleteObject(hrgn)
//             ::SetWindowRgn(window, hrgn, !move.clip_rect.IsEmpty() && (flags & SWP_NOREDRAW) == 0);
// 
//         }
//         else {
//             flags |= SWP_NOMOVE;
//             flags |= SWP_NOSIZE;
//         }
// 
//         gfx::Rect window_rect_in_pixel = gfx::win::DIPToScreenRect(move.window_rect);
//         defer_window_pos_info = ::DeferWindowPos(defer_window_pos_info,
//             window, NULL,
//             window_rect_in_pixel.x(),
//             window_rect_in_pixel.y(),
//             window_rect_in_pixel.width(),
//             window_rect_in_pixel.height(),
//             flags);
// 
//         if (!defer_window_pos_info) {
//             DWORD lastError = GetLastError();
//             if (lastError == ERROR_INVALID_WINDOW_HANDLE) {
//                 // The window might have become invalid since the last time we checked
//                 // it (at the beginning of this loop).  In this case, let's start
//                 // again.
//                 DCHECK(!::IsWindow(window));
// 
//                 ++failure_count;
//                 int remaining = moves.size() - failure_count;
//                 if (0 == remaining) {
//                     return;
//                 }
// 
//                 defer_window_pos_info = ::BeginDeferWindowPos(remaining);
// 
//                 if (!defer_window_pos_info) {
//                     NOTREACHED();
//                     return;
//                 }
// 
//                 // Note that we set i to -1 here so that it increments up to 0 by the
//                 // for-loop.
//                 i = -1;
//                 continue;
//             }
//             DCHECK(false) << "DeferWindowPos failed, so all plugin moves ignored: " << lastError;
//             return;
//         }
//     }
// 
//     ::EndDeferWindowPos(defer_window_pos_info);
// }

void WebPluginImpl::updateGeometry(
    const blink::WebRect& windowRect, const blink::WebRect& clipRect,
    const blink::WebRect& unobscuredRect, const blink::WebVector<blink::WebRect>& cutOutsRects,
    bool isVisible)
{
    updatePluginWidget(windowRect, clipRect);
}

void WebPluginImpl::updateFocus(bool focused, blink::WebFocusType)
{
    //DebugBreak();
}

void WebPluginImpl::asynSetPlatformPluginWidgetVisibilityTimerFired(blink::Timer<WebPluginImpl>*)
{
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    bool isVisibility = container->isSelfVisible();
    if (isVisibility) {
        if (container->isParentVisible() && platformPluginWidget())
            ::ShowWindow(platformPluginWidget(), SW_SHOWNA);
    } else {
        if (container->isParentVisible() && platformPluginWidget())
            ::ShowWindow(platformPluginWidget(), SW_HIDE);
    }
}

void WebPluginImpl::updateVisibility(bool isVisibility)
{
    if (!m_setPlatformPluginWidgetVisibilityTimer.isActive())
        m_setPlatformPluginWidgetVisibilityTimer.startOneShot(0, FROM_HERE);
}

bool WebPluginImpl::acceptsInputEvents()
{
    return true;
}

bool WebPluginImpl::handleInputEvent(const blink::WebInputEvent&, blink::WebCursorInfo&)
{
    return true;
}

void WebPluginImpl::paintIntoTransformedContext(HDC hdc)
{
    if (m_isWindowed) {
        SendMessage(platformPluginWidget(), WM_PRINTCLIENT, reinterpret_cast<WPARAM>(hdc), PRF_CLIENT | PRF_CHILDREN | PRF_OWNED);
        return;
    }

    m_npWindow.type = NPWindowTypeDrawable;
    m_npWindow.window = hdc;

    WINDOWPOS windowpos = { 0, 0, 0, 0, 0, 0, 0 };

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    IntRect r = contentsToNativeWindow(m_pluginContainer, container->frameRect());

    windowpos.x = r.x();
    windowpos.y = r.y();
    windowpos.cx = r.width();
    windowpos.cy = r.height();

    NPEvent npEvent;
    npEvent.event = WM_WINDOWPOSCHANGED;
    npEvent.lParam = reinterpret_cast<uintptr_t>(&windowpos);
    npEvent.wParam = 0;

    dispatchNPEvent(npEvent);

    setNPWindowRect(container->frameRect());

    npEvent.event = WM_PAINT;
    npEvent.wParam = reinterpret_cast<uintptr_t>(hdc);

    // This is supposed to be a pointer to the dirty rect, but it seems that the Flash plugin
    // ignores it so we just pass null.
    npEvent.lParam = 0;

    dispatchNPEvent(npEvent);
}

void WebPluginImpl::paintWindowedPluginIntoContext(GraphicsContext& context, const IntRect& rect)
{
// #if !USE(WINGDI)
//     ASSERT(m_isWindowed);
//     ASSERT(context.shouldIncludeChildWindows());
// 
//     IntPoint locationInWindow = downcast<FrameView>(*parent()).convertToContainingWindow(frameRect().location());
// 
//     LocalWindowsContext windowsContext(context, frameRect(), false);
// 
// #if USE(CAIRO)
//     // Must flush drawings up to this point to the backing metafile, otherwise the
//     // plugin region will be overwritten with any clear regions specified in the
//     // cairo-controlled portions of the rendering.
//     cairo_show_page(context.platformContext()->cr());
// #endif

//     HDC hdc = windowsContext.hdc();
//     XFORM originalTransform;
//     GetWorldTransform(hdc, &originalTransform);
// 
//     // The plugin expects the DC to be in client coordinates, so we translate
//     // the DC to make that so.
//     AffineTransform ctm = context.getCTM();
//     ctm.translate(locationInWindow.x(), locationInWindow.y());
//     XFORM transform = static_cast<XFORM>(ctm.toTransformationMatrix());
// 
//     SetWorldTransform(hdc, &transform);
// 
//     paintIntoTransformedContext(hdc);
// 
//     SetWorldTransform(hdc, &originalTransform);
    DebugBreak();
//#endif
}

class BitmapDeviceForGetBitmap : public skia::BitmapPlatformDevice {
public:
    const SkBitmap& getBitmap() {
        return onAccessBitmap();
    }
};

void WebPluginImpl::paint(blink::WebCanvas* canvas, const blink::WebRect& rect)
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
    if (!m_haveCalledSetWindow)
        setNPWindowRect(container->frameRect());

    if (m_isWindowed) {
// #if !USE(WINGDI)
//          if (context.shouldIncludeChildWindows())
//              paintWindowedPluginIntoContext(context, rect);
// #endif
        return;
    }

    HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

    // On Safari/Windows without transparency layers the GraphicsContext returns the HDC
    // of the window and the plugin expects that the passed in DC has window coordinates.
    //if (!context.isInTransparencyLayer()) {
        XFORM transform;
        GetWorldTransform(hMemoryDC, &transform);
        transform.eDx = -m_windowRect.x();
        transform.eDy = -m_windowRect.y();
        SetWorldTransform(hMemoryDC, &transform);
    //}

    paintIntoTransformedContext(hMemoryDC);
    skia::EndPlatformPaint(m_memoryCanvas);

    BitmapDeviceForGetBitmap* bitmapDevice = (BitmapDeviceForGetBitmap*)skia::GetTopDevice(*m_memoryCanvas);
    const SkBitmap& bitmap = bitmapDevice->getBitmap();
    canvas->drawBitmap(bitmap, m_windowRect.x(), m_windowRect.y());
}

void WebPluginImpl::setNPWindowRect(const IntRect& rect)
{
    if (!m_isStarted)
        return;

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container->parent())
        return;

    FrameView* frameView = toFrameView(container->parent());
    if (!frameView)
        return;

    IntPoint p = container->localToRootFramePoint(rect.location());
    m_npWindow.x = rect.x(); // windowlessģʽ��ֱ�ӻ���0��0��Ķ���canvas
    m_npWindow.y = rect.y();

    m_npWindow.width = rect.width();
    m_npWindow.height = rect.height();

    m_npWindow.clipRect.right = rect.width();
    m_npWindow.clipRect.bottom = rect.height();
    m_npWindow.clipRect.left = 0;
    m_npWindow.clipRect.top = 0;

    if (m_plugin->pluginFuncs()->setwindow) {
        //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
        setCallingPlugin(true);
        m_plugin->pluginFuncs()->setwindow(m_instance, &m_npWindow);
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

NPError WebPluginImpl::handlePostReadFile(Vector<char>& buffer, uint32_t len, const char* buf)
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

bool WebPluginImpl::platformGetValueStatic(NPNVariable, void*, NPError*)
{
    return false;
}

bool WebPluginImpl::platformGetValue(NPNVariable variable, void* value, NPError* result)
{
    switch (variable) {
        case NPNVnetscapeWindow: {
            HWND* w = reinterpret_cast<HWND*>(value);
            *w = windowHandleForPageClient(platformPluginWidget());
            *result = NPERR_NO_ERROR;
            return true;
        }

        case NPNVSupportsWindowless: {
            NPBool* flag = reinterpret_cast<NPBool*>(value);
            *flag = TRUE;
            *result = NPERR_NO_ERROR;
            return true;
        }

    default:
        return false;
    }
}

void WebPluginImpl::invalidateRect(const IntRect& rect)
{
    if (m_isWindowed) {
        RECT invalidRect = { rect.x(), rect.y(), rect.maxX(), rect.maxY() };
        ::InvalidateRect(platformPluginWidget(), &invalidRect, false);
        return;
    }

    invalidateWindowlessPluginRect(rect);
}

void WebPluginImpl::invalidateRect(NPRect* rect)
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

void WebPluginImpl::invalidateRegion(NPRegion region)
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

void WebPluginImpl::forceRedraw()
{
    if (m_isWindowed)
        ::UpdateWindow(platformPluginWidget());
//      else
//         ::UpdateWindow(windowHandleForPageClient(parent() ? parent()->hostWindow()->platformPageClient() : 0));
}

void WebPluginImpl::platformStartAsyn(blink::Timer<WebPluginImpl>*)
{
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;

    if (m_isWindowed) {
        registerPluginView();
        setUpOffscreenPaintingHooks(hookedBeginPaint, hookedEndPaint);

        DWORD flags = WS_CHILD;
        if (container->isSelfVisible())
            flags |= WS_VISIBLE;

        HWND parentWindowHandle = m_parentWidget;
        HWND window = ::CreateWindowEx(0, kWebPluginViewClassName, 0, flags,
            0, 0, 0, 0, parentWindowHandle, 0, /*WebCore::instanceHandle()*/nullptr, 0);

        setPlatformWidget(window);

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
    
    updatePluginWidget(m_windowRect, m_clipRect);

    if (!m_plugin->quirks().contains(PluginQuirkDeferFirstSetWindowCall))
        setNPWindowRect(container->frameRect());
}

bool WebPluginImpl::platformStart()
{
    ASSERT(m_isStarted);
    ASSERT(m_status == PluginStatusLoadedSuccessfully);
    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return false;

    m_asynStartTimer.startOneShot(0, FROM_HERE);

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

void WebPluginImpl::platformDestroy()
{
    if (m_memoryCanvas)
        delete m_memoryCanvas;
    m_memoryCanvas = nullptr;

    PlatformPluginWidget widget = platformPluginWidget();
    if (!widget)
        return;

    blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(platformDestroyWindow, widget));

    ++CheckReEnter::s_kEnterContent;
    ::ShowWindow(widget, SW_HIDE);
    setPlatformPluginWidget(0);
    --CheckReEnter::s_kEnterContent;
}

PassRefPtr<Image> WebPluginImpl::snapshot()
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
