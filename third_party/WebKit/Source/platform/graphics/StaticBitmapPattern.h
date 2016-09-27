// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StaticBitmapPattern_h
#define StaticBitmapPattern_h

#include "platform/graphics/BitmapPatternBase.h"

namespace blink {

class PLATFORM_EXPORT StaticBitmapPattern : public BitmapPatternBase {
public:
    static PassRefPtr<Pattern> create(PassRefPtr<Image> tileImage, RepeatMode);

    ~StaticBitmapPattern() override;

protected:
    PassRefPtr<SkShader> createShader() override;

    SkImageInfo getBitmapInfo() override;
    void drawBitmapToCanvas(SkCanvas&, SkPaint&) override;

private:
    StaticBitmapPattern(PassRefPtr<SkImage>, RepeatMode);

    RefPtr<SkImage> m_tileImage;
};

} // namespace

#endif
