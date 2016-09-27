// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/InterceptingCanvas.h"

namespace blink {

void InterceptingCanvasBase::unrollDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint, SkPicture::AbortCallback* abortCallback)
{
    int saveCount = this->getSaveCount();
    if (paint) {
        SkRect newBounds = picture->cullRect();
        if (matrix)
            matrix->mapRect(&newBounds);
        this->saveLayer(&newBounds, paint);
    } else if (matrix) {
        this->save();
    }
    if (matrix)
        this->concat(*matrix);

    picture->playback(this, abortCallback);

    this->restoreToCount(saveCount);
}

} // namespace blink
