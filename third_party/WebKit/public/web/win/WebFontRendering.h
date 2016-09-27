// Copyright 2014 Google Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebFontRendering_h
#define WebFontRendering_h

#include "public/platform/WebCommon.h"
#include <SkFontHost.h>

class SkTypeface;
struct IDWriteFactory;

namespace blink {

class WebFontRendering {
public:
    BLINK_EXPORT static void setUseDirectWrite(bool);
    BLINK_EXPORT static void setDirectWriteFactory(IDWriteFactory*);
    BLINK_EXPORT static void setDeviceScaleFactor(float);
    BLINK_EXPORT static void setUseSubpixelPositioning(bool);
    BLINK_EXPORT static void addSideloadedFontForTesting(SkTypeface*);
    BLINK_EXPORT static void setMenuFontMetrics(const wchar_t* familyName, int32_t fontHeight);
    BLINK_EXPORT static void setSmallCaptionFontMetrics(const wchar_t* familyName, int32_t fontHeight);
    BLINK_EXPORT static void setStatusFontMetrics(const wchar_t* familyName, int32_t fontHeight);
    BLINK_EXPORT static void setLCDOrder(SkFontHost::LCDOrder);
    BLINK_EXPORT static void setLCDOrientation(SkFontHost::LCDOrientation);
};

} // namespace blink

#endif
