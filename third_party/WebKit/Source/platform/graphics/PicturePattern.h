// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PicturePattern_h
#define PicturePattern_h

#include "platform/graphics/Pattern.h"

namespace blink {

class PLATFORM_EXPORT PicturePattern : public Pattern {
public:
    static PassRefPtr<PicturePattern> create(PassRefPtr<const SkPicture>, RepeatMode);

    ~PicturePattern() override;

protected:
    PassRefPtr<SkShader> createShader() override;

private:
    PicturePattern(PassRefPtr<const SkPicture>, RepeatMode);

    RefPtr<const SkPicture> m_tilePicture;
};

} // namespace

#endif
