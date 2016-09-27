/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2007-2008 Torch Mobile Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */


#include "config.h"
#include <windows.h>
#include <windowsx.h>
#include "PlatformMouseEventWin.h"

#include <wtf/Assertions.h>

namespace blink {

#define HIGH_BIT_MASK_SHORT 0x8000

static IntPoint positionForEvent(HWND hWnd, LPARAM lParam)
{
    POINT point = { ((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)) };
    return IntPoint(point.x, point.y);
}

static IntPoint globalPositionForEvent(HWND hWnd, LPARAM lParam)
{
    POINT point = { ((int)(short)LOWORD(lParam)), ((int)(short)HIWORD(lParam)) };
    ClientToScreen(hWnd, &point);
    return IntPoint(point.x, point.y);
}

static PlatformEvent::Type messageToEventType(UINT message)
{
    switch (message) {
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
            //MSDN docs say double click is sent on mouse down
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            return PlatformEvent::Type::MousePressed;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            return PlatformEvent::Type::MouseReleased;

#if !OS(WINCE)
        case WM_MOUSELEAVE:
#endif
        case WM_MOUSEMOVE:
            return PlatformEvent::Type::MouseMoved;

        default:
            ASSERT_NOT_REACHED();
            //Move is relatively harmless
            return PlatformEvent::Type::MouseMoved;
    }
}

PlatformMouseEventWin::PlatformMouseEventWin(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool didActivateWebView)
    : m_shiftKey(wParam & MK_SHIFT)
    , m_ctrlKey(wParam & MK_CONTROL)
    , m_altKey(GetKeyState(VK_MENU) & HIGH_BIT_MASK_SHORT)
    , m_metaKey(m_altKey) // FIXME: We'll have to test other browsers
    , m_didActivateWebView(didActivateWebView)
    , m_eventType(messageToEventType(message))
    , m_modifierFlags(wParam)
{
    m_position = (positionForEvent(hWnd, lParam));
    m_globalPosition = (globalPositionForEvent(hWnd, lParam));
    m_clickCount = 1;
    m_timestamp = ::GetTickCount()*0.001; // GetTickCount returns milliseconds
    
    switch (message) {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        m_type = MousePressed;
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        m_type = MouseReleased;
        break;
    case WM_MOUSEMOVE:
#if !OS(WINCE)
    case WM_MOUSELEAVE:
        m_type = MouseMoved;
        break;
#endif
    case WM_MOUSEHWHEEL:
        m_type = MouseScroll;
        break;
    }

    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
            m_button = LeftButton;
            break;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
            m_button = RightButton;
            break;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
            m_button = MiddleButton;
            break;
        case WM_MOUSEMOVE:
#if !OS(WINCE)
        case WM_MOUSELEAVE:
#endif
            if (wParam & MK_LBUTTON)
                m_button = LeftButton;
            else if (wParam & MK_MBUTTON)
                m_button = MiddleButton;
            else if (wParam & MK_RBUTTON)
                m_button = RightButton;
            else
                m_button = NoButton;
            break;
        default:
            ASSERT_NOT_REACHED();
    }
}

} // namespace WebCore
