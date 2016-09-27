// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/VideoPainter.h"

#include "core/dom/Document.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLVideoElement.h"
#include "core/layout/LayoutVideo.h"
#include "core/paint/ImagePainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/geometry/LayoutPoint.h"

namespace blink {

void VideoPainter::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    WebMediaPlayer* mediaPlayer = m_layoutVideo.mediaElement()->webMediaPlayer();
    bool displayingPoster = m_layoutVideo.videoElement()->shouldDisplayPosterImage();
    if (!displayingPoster && !mediaPlayer)
        return;

    LayoutRect rect(m_layoutVideo.videoBox());
    if (rect.isEmpty())
        return;
    rect.moveBy(paintOffset);

    GraphicsContext* context = paintInfo.context;
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, m_layoutVideo, paintInfo.phase))
        return;

    LayoutRect contentRect = m_layoutVideo.contentBoxRect();
    contentRect.moveBy(paintOffset);
    LayoutObjectDrawingRecorder drawingRecorder(*context, m_layoutVideo, paintInfo.phase, contentRect);

    bool clip = !contentRect.contains(rect);
    if (clip) {
        context->save();
        context->clip(contentRect);
    }

    if (displayingPoster) {
        ImagePainter(m_layoutVideo).paintIntoRect(context, rect);
    } else if ((m_layoutVideo.document().view() && m_layoutVideo.document().view()->paintBehavior() & PaintBehaviorFlattenCompositingLayers) || !m_layoutVideo.acceleratedRenderingInUse()) {
        SkPaint videoPaint = context->fillPaint();
        videoPaint.setColor(SK_ColorBLACK);
        m_layoutVideo.videoElement()->paintCurrentFrame(context->canvas(), pixelSnappedIntRect(rect), &videoPaint);
    }
    if (clip)
        context->restore();
}

} // namespace blink
