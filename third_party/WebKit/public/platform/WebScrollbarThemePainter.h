/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebScrollbarThemePainter_h
#define WebScrollbarThemePainter_h

#include "WebCanvas.h"

namespace blink {

class ScrollbarTheme;
class Scrollbar;
class WebScrollbar;
struct WebRect;

class WebScrollbarThemePainter {
public:
    WebScrollbarThemePainter() : m_theme(0), m_scrollbar(0){}
    WebScrollbarThemePainter(const WebScrollbarThemePainter& painter) { assign(painter); }
    virtual ~WebScrollbarThemePainter() { }
    WebScrollbarThemePainter& operator=(const WebScrollbarThemePainter& painter)
    {
        assign(painter);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void assign(const WebScrollbarThemePainter&);

    BLINK_PLATFORM_EXPORT void paintScrollbarBackground(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintTrackBackground(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintBackTrackPart(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintForwardTrackPart(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintBackButtonStart(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintBackButtonEnd(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintForwardButtonStart(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintForwardButtonEnd(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintTickmarks(WebCanvas*, const WebRect&);
    BLINK_PLATFORM_EXPORT void paintThumb(WebCanvas*, const WebRect&);

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebScrollbarThemePainter(ScrollbarTheme*, Scrollbar*);
    
#ifndef MINIBLINK_NOT_IMPLEMENTED
    // blink的方式是自己实现滚动条的各种绘制，这里我们改走blink原生的
    ScrollbarTheme* theme() { return m_theme; }
    Scrollbar* scrollbar() { return m_scrollbar; }
#endif // MINIBLINK_NOT_IMPLEMENTED

#endif

private:
    // The theme is not owned by this class. It is assumed that the theme is a
    // static pointer and its lifetime is essentially infinite. The functions
    // called from the painter may not be thread-safe, so all calls must be made
    // from the same thread that it is created on.
    ScrollbarTheme* m_theme;

    // It is assumed that the constructor of this paint object is responsible
    // for the lifetime of this scrollbar. The painter has to use the real
    // scrollbar (and not a WebScrollbar wrapper) due to static_casts for
    // LayoutScrollbar and pointer-based HashMap lookups for Lion scrollbars.
    Scrollbar* m_scrollbar;
};

} // namespace blink

#endif
