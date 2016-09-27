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

#ifndef WebScrollbarThemeGeometryNative_h
#define WebScrollbarThemeGeometryNative_h

#include "platform/PlatformExport.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebScrollbarThemeGeometry.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ScrollbarTheme;
class WebScrollbar;

class PLATFORM_EXPORT WebScrollbarThemeGeometryNative : public WebScrollbarThemeGeometry {
public:
    static PassOwnPtr<WebScrollbarThemeGeometryNative> create(ScrollbarTheme*);

    // WebScrollbarThemeGeometry overrides
    WebScrollbarThemeGeometryNative* clone() const override;
    int thumbPosition(WebScrollbar*) override;
    int thumbLength(WebScrollbar*) override;
    int trackPosition(WebScrollbar*) override;
    int trackLength(WebScrollbar*) override;
    bool hasButtons(WebScrollbar*) override;
    bool hasThumb(WebScrollbar*) override;
    WebRect trackRect(WebScrollbar*) override;
    WebRect thumbRect(WebScrollbar*) override;
    int minimumThumbLength(WebScrollbar*) override;
    int scrollbarThickness(WebScrollbar*) override;
    WebRect backButtonStartRect(WebScrollbar*) override;
    WebRect backButtonEndRect(WebScrollbar*) override;
    WebRect forwardButtonStartRect(WebScrollbar*) override;
    WebRect forwardButtonEndRect(WebScrollbar*) override;
    WebRect constrainTrackRectToTrackPieces(WebScrollbar*, const WebRect&) override;
    void splitTrack(WebScrollbar*, const WebRect& track, WebRect& startTrack, WebRect& thumb, WebRect& endTrack) override;

private:
    explicit WebScrollbarThemeGeometryNative(ScrollbarTheme*);

    // The theme is not owned by this class. It is assumed that the theme is a
    // static pointer and its lifetime is essentially infinite. Only thread-safe
    // functions on the theme can be called by this theme.
    ScrollbarTheme* m_theme;
};

} // namespace blink

#endif
