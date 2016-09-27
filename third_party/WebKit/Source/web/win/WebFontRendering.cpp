// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/win/WebFontRendering.h"

#include "platform/fonts/FontCache.h"

namespace blink {

// static
void WebFontRendering::setUseDirectWrite(bool useDirectWrite)
{
    FontCache::setUseDirectWrite(useDirectWrite);
}

// static
void WebFontRendering::setDirectWriteFactory(IDWriteFactory* factory)
{
    FontCache::setDirectWriteFactory(factory);
}

// static
void WebFontRendering::setDeviceScaleFactor(float deviceScaleFactor)
{
    FontCache::setDeviceScaleFactor(deviceScaleFactor);
}

// static
void WebFontRendering::setUseSubpixelPositioning(bool useSubpixelPositioning)
{
    FontCache::setUseSubpixelPositioning(useSubpixelPositioning);
}

// static
void WebFontRendering::addSideloadedFontForTesting(SkTypeface* typeface)
{
    FontCache::addSideloadedFontForTesting(typeface);
}

// static
void WebFontRendering::setMenuFontMetrics(const wchar_t* familyName, int32_t fontHeight)
{
    FontCache::setMenuFontMetrics(familyName, fontHeight);
}

// static
void WebFontRendering::setSmallCaptionFontMetrics(const wchar_t* familyName, int32_t fontHeight)
{
    FontCache::setSmallCaptionFontMetrics(familyName, fontHeight);
}

// static
void WebFontRendering::setStatusFontMetrics(const wchar_t* familyName, int32_t fontHeight)
{
    FontCache::setStatusFontMetrics(familyName, fontHeight);
}

// static
void WebFontRendering::setLCDOrder(SkFontHost::LCDOrder order)
{
    SkFontHost::SetSubpixelOrder(order);
}

// static
void WebFontRendering::setLCDOrientation(SkFontHost::LCDOrientation orientation)
{
    SkFontHost::SetSubpixelOrientation(orientation);
}

} // namespace blink
