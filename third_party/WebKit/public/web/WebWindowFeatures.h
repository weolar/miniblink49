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

#ifndef WebWindowFeatures_h
#define WebWindowFeatures_h

#include "../platform/WebCommon.h"
#include "../platform/WebString.h"
#include "../platform/WebVector.h"

#if BLINK_IMPLEMENTATION
#include "core/page/WindowFeatures.h"
#endif

namespace blink {

struct WebWindowFeatures {
    float x;
    bool xSet;
    float y;
    bool ySet;
    float width;
    bool widthSet;
    float height;
    bool heightSet;

    bool menuBarVisible;
    bool statusBarVisible;
    bool toolBarVisible;
    bool locationBarVisible;
    bool scrollbarsVisible;
    bool resizable;

    bool fullscreen;
    bool dialog;
    WebVector<WebString> additionalFeatures;

    WebWindowFeatures()
        : x(0)
        , xSet(false)
        , y(0)
        , ySet(false)
        , width(0)
        , widthSet(false)
        , height(0)
        , heightSet(false)
        , menuBarVisible(true)
        , statusBarVisible(true)
        , toolBarVisible(true)
        , locationBarVisible(true)
        , scrollbarsVisible(true)
        , resizable(true)
        , fullscreen(false)
        , dialog(false)
    {
    }


#if BLINK_IMPLEMENTATION
    WebWindowFeatures(const WindowFeatures& f)
        : x(f.x)
        , xSet(f.xSet)
        , y(f.y)
        , ySet(f.ySet)
        , width(f.width)
        , widthSet(f.widthSet)
        , height(f.height)
        , heightSet(f.heightSet)
        , menuBarVisible(f.menuBarVisible)
        , statusBarVisible(f.statusBarVisible)
        , toolBarVisible(f.toolBarVisible)
        , locationBarVisible(f.locationBarVisible)
        , scrollbarsVisible(f.scrollbarsVisible)
        , resizable(f.resizable)
        , fullscreen(f.fullscreen)
        , dialog(f.dialog)
        , additionalFeatures(f.additionalFeatures)
    {
    }

    operator WindowFeatures() const
    {
        WindowFeatures result;
        result.x = x;
        result.xSet = xSet;
        result.y = y;
        result.ySet = ySet;
        result.width = width;
        result.widthSet = widthSet;
        result.height = height;
        result.heightSet = heightSet;
        result.menuBarVisible = menuBarVisible;
        result.statusBarVisible = statusBarVisible;
        result.toolBarVisible = toolBarVisible;
        result.locationBarVisible = locationBarVisible;
        result.scrollbarsVisible = scrollbarsVisible;
        result.resizable = resizable;
        result.fullscreen = fullscreen;
        result.dialog = dialog;
        for (size_t i = 0; i < additionalFeatures.size(); ++i)
            result.additionalFeatures.append(additionalFeatures[i]);
        return result;
    }
#endif
};

} // namespace blink

#endif
