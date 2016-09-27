/*
 * Copyright (C) 2012 Google Inc.  All rights reserved.
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
#include "core/layout/LayoutThemeFontProvider.h"

#include "core/CSSValueKeywords.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/FontDescription.h"
#include "platform/win/HWndDC.h"
#include "platform/win/SystemInfo.h"
#include "wtf/text/WTFString.h"
#include <windows.h>

#define SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(structName, member) \
    offsetof(structName, member) + \
    (sizeof static_cast<structName*>(nullptr)->member)
#define NONCLIENTMETRICS_SIZE_PRE_VISTA \
    SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(NONCLIENTMETRICS, lfMessageFont)

namespace blink {

// Converts |points| to pixels. One point is 1/72 of an inch.
static float pointsToPixels(float points)
{
    static float pixelsPerInch = 0.0f;
    if (!pixelsPerInch) {
        HWndDC hdc(0); // What about printing? Is this the right DC?
        if (hdc) // Can this ever actually be 0?
            pixelsPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
        else
            pixelsPerInch = 96.0f;
    }

    static const float pointsPerInch = 72.0f;
    return points / pointsPerInch * pixelsPerInch;
}

static bool getNonClientMetrics(NONCLIENTMETRICS* metrics)
{
    static UINT size = isWindowsVistaOrGreater() ?
        sizeof(NONCLIENTMETRICS) : NONCLIENTMETRICS_SIZE_PRE_VISTA;
    metrics->cbSize = size;
    bool success = !!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, size, metrics, 0);
    ASSERT_UNUSED(success, success);
    return success;
}

// Return the height of system font |font| in pixels. We use this size by
// default for some non-form-control elements.
static float systemFontSize(const LOGFONT& font)
{
    float size = -font.lfHeight;
    if (size < 0) {
        HFONT hFont = CreateFontIndirect(&font);
        if (hFont) {
            HWndDC hdc(0); // What about printing? Is this the right DC?
            if (hdc) {
                HGDIOBJ hObject = SelectObject(hdc, hFont);
                TEXTMETRIC tm;
                GetTextMetrics(hdc, &tm);
                SelectObject(hdc, hObject);
                size = tm.tmAscent;
            }
            DeleteObject(hFont);
        }
    }

    // The "codepage 936" bit here is from Gecko; apparently this helps make
    // fonts more legible in Simplified Chinese where the default font size is
    // too small.
    //
    // FIXME: http://b/1119883 Since this is only used for "small caption",
    // "menu", and "status bar" objects, I'm not sure how much this even
    // matters. Plus the Gecko patch went in back in 2002, and maybe this
    // isn't even relevant anymore. We should investigate whether this should
    // be removed, or perhaps broadened to be "any CJK locale".
    //
    return ((size < 12.0f) && (GetACP() == 936)) ? 12.0f : size;
}

// static
void LayoutThemeFontProvider::systemFont(CSSValueID systemFontID, FontStyle& fontStyle, FontWeight& fontWeight, float& fontSize, AtomicString& fontFamily)
{
    fontStyle = FontStyleNormal;
    fontWeight = FontWeightNormal;
    fontSize = s_defaultFontSize;
    fontFamily = defaultGUIFont();

    switch (systemFontID) {
    case CSSValueSmallCaption: {
        fontSize = FontCache::smallCaptionFontHeight();
        if (fontSize) {
            fontFamily = FontCache::smallCaptionFontFamily();
        } else {
            NONCLIENTMETRICS metrics;
            if (getNonClientMetrics(&metrics)) {
                fontSize = systemFontSize(metrics.lfSmCaptionFont);
                fontFamily = AtomicString(metrics.lfSmCaptionFont.lfFaceName, wcslen(metrics.lfSmCaptionFont.lfFaceName));
            }
        }
        break;
    }
    case CSSValueMenu: {
        fontSize = FontCache::menuFontHeight();
        if (fontSize) {
            fontFamily = FontCache::menuFontFamily();
        } else {
            NONCLIENTMETRICS metrics;
            if (getNonClientMetrics(&metrics)) {
                fontSize = systemFontSize(metrics.lfMenuFont);
                fontFamily = AtomicString(metrics.lfMenuFont.lfFaceName, wcslen(metrics.lfMenuFont.lfFaceName));
            }
        }
        break;
    }
    case CSSValueStatusBar: {
        fontSize = FontCache::statusFontHeight();
        if (fontSize) {
            fontFamily = FontCache::statusFontFamily();
        } else {
            NONCLIENTMETRICS metrics;
            if (getNonClientMetrics(&metrics)) {
                fontSize = systemFontSize(metrics.lfStatusFont);
                fontFamily = metrics.lfStatusFont.lfFaceName;
            }
        }
        break;
    }
    case CSSValueWebkitMiniControl:
    case CSSValueWebkitSmallControl:
    case CSSValueWebkitControl:
        // Why 2 points smaller? Because that's what Gecko does.
        fontSize = s_defaultFontSize - pointsToPixels(2);
        fontFamily = defaultGUIFont();
        break;
    default:
        fontSize = s_defaultFontSize;
        fontFamily = defaultGUIFont();
        break;
    }
}

// static
void LayoutThemeFontProvider::setDefaultFontSize(int fontSize)
{
    s_defaultFontSize = static_cast<float>(fontSize);
}

} // namespace blink
