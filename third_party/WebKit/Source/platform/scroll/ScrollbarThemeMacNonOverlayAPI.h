/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef ScrollbarThemeMacNonOverlayAPI_h
#define ScrollbarThemeMacNonOverlayAPI_h

#include "platform/scroll/ScrollbarThemeMacCommon.h"

namespace blink {

class PLATFORM_EXPORT ScrollbarThemeMacNonOverlayAPI : public ScrollbarThemeMacCommon {
public:
    int scrollbarThickness(ScrollbarControlSize = RegularScrollbar) override;
    bool usesOverlayScrollbars() const override { return false; }
    ScrollbarButtonsPlacement buttonsPlacement() const override;

    bool paint(ScrollbarThemeClient*, GraphicsContext*, const IntRect& damageRect) override;

protected:
    IntRect trackRect(ScrollbarThemeClient*, bool painting = false) override;
    IntRect backButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) override;
    IntRect forwardButtonRect(ScrollbarThemeClient*, ScrollbarPart, bool painting = false) override;

    void updateButtonPlacement() override;

    bool hasButtons(ScrollbarThemeClient*) override;
    bool hasThumb(ScrollbarThemeClient*) override;

    int minimumThumbLength(ScrollbarThemeClient*) override;
};

}

#endif
