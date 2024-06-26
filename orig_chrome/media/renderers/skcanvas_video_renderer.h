// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_RENDERERS_SKCANVAS_VIDEO_RENDERER_H_
#define MEDIA_RENDERERS_SKCANVAS_VIDEO_RENDERER_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "media/base/media_export.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_frame.h"
#include "media/base/video_rotation.h"
#include "media/filters/context_3d.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkXfermode.h"

class SkCanvas;
class SkImage;

namespace gfx {
class RectF;
}

namespace media {
class VideoImageGenerator;

// Handles rendering of VideoFrames to SkCanvases.
class MEDIA_EXPORT SkCanvasVideoRenderer {
public:
    SkCanvasVideoRenderer();
    ~SkCanvasVideoRenderer();

    // Paints |video_frame| on |canvas|, scaling and rotating the result to fit
    // dimensions specified by |dest_rect|.
    // If the format of |video_frame| is PIXEL_FORMAT_NATIVE_TEXTURE, |context_3d|
    // must be provided.
    //
    // Black will be painted on |canvas| if |video_frame| is null.
    void Paint(const scoped_refptr<VideoFrame>& video_frame,
        SkCanvas* canvas,
        const gfx::RectF& dest_rect,
        uint8 alpha,
        SkXfermode::Mode mode,
        VideoRotation video_rotation,
        const Context3D& context_3d);

    // Copy |video_frame| on |canvas|.
    // If the format of |video_frame| is PIXEL_FORMAT_NATIVE_TEXTURE, |context_3d|
    // must be provided.
    void Copy(const scoped_refptr<VideoFrame>& video_frame,
        SkCanvas* canvas,
        const Context3D& context_3d);

    // Convert the contents of |video_frame| to raw RGB pixels. |rgb_pixels|
    // should point into a buffer large enough to hold as many 32 bit RGBA pixels
    // as are in the visible_rect() area of the frame.
    static void ConvertVideoFrameToRGBPixels(const media::VideoFrame* video_frame,
        void* rgb_pixels,
        size_t row_bytes);

    // Copy the contents of texture of |video_frame| to texture |texture|.
    // |level|, |internal_format|, |type| specify target texture |texture|.
    // The format of |video_frame| must be VideoFrame::NATIVE_TEXTURE.
    static void CopyVideoFrameSingleTextureToGLTexture(
        gpu::gles2::GLES2Interface* gl,
        VideoFrame* video_frame,
        unsigned int texture,
        unsigned int internal_format,
        unsigned int type,
        bool premultiply_alpha,
        bool flip_y);

    // In general, We hold the most recently painted frame to increase the
    // performance for the case that the same frame needs to be painted
    // repeatedly. Call this function if you are sure the most recent frame will
    // never be painted again, so we can release the resource.
    void ResetCache();

private:
    // Last image used to draw to the canvas.
    skia::RefPtr<SkImage> last_image_;
    // Timestamp of the videoframe used to generate |last_image_|.
    base::TimeDelta last_timestamp_ = media::kNoTimestamp();
    // If |last_image_| is not used for a while, it's deleted to save memory.
    base::DelayTimer last_image_deleting_timer_;

    // Used for DCHECKs to ensure method calls executed in the correct thread.
    base::ThreadChecker thread_checker_;

    DISALLOW_COPY_AND_ASSIGN(SkCanvasVideoRenderer);
};

} // namespace media

#endif // MEDIA_RENDERERS_SKCANVAS_VIDEO_RENDERER_H_
