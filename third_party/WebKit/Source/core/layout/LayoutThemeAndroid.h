/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef LayoutThemeAndroid_h
#define LayoutThemeAndroid_h

#include "core/layout/LayoutThemeDefault.h"

namespace blink {

class LayoutThemeAndroid final : public LayoutThemeDefault {
public:
    static PassRefPtr<LayoutTheme> create();
    virtual String extraDefaultStyleSheet() override;

    virtual void adjustInnerSpinButtonStyle(ComputedStyle&, Element*) const override;

    virtual bool delegatesMenuListRendering() const override { return true; }

    virtual String extraMediaControlsStyleSheet() override;

    virtual Color platformTapHighlightColor() const override
    {
        return LayoutThemeAndroid::defaultTapHighlightColor;
    }

    virtual Color platformActiveSelectionBackgroundColor() const override
    {
        return LayoutThemeAndroid::defaultActiveSelectionBackgroundColor;
    }

protected:
    virtual int menuListArrowPadding() const override;

private:
    virtual ~LayoutThemeAndroid();

    static const RGBA32 defaultTapHighlightColor = 0x6633b5e5;
    static const RGBA32 defaultActiveSelectionBackgroundColor = 0x6633b5e5;
};

} // namespace blink

#endif // LayoutThemeAndroid_h
