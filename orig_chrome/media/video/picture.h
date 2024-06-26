// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_VIDEO_PICTURE_H_
#define MEDIA_VIDEO_PICTURE_H_

#include "base/basictypes.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "media/base/media_export.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace media {

// A picture buffer that is composed of a GLES2 texture.
// This is the media-namespace equivalent of PP_PictureBuffer_Dev.
class MEDIA_EXPORT PictureBuffer {
public:
    PictureBuffer(int32 id, gfx::Size size, uint32 texture_id);
    PictureBuffer(int32 id,
        gfx::Size size,
        uint32 texture_id,
        uint32 internal_texture_id);
    PictureBuffer(int32 id,
        gfx::Size size,
        uint32 texture_id,
        const gpu::Mailbox& texture_mailbox);

    // Returns the client-specified id of the buffer.
    int32 id() const
    {
        return id_;
    }

    // Returns the size of the buffer.
    gfx::Size size() const
    {
        return size_;
    }

    // Returns the id of the texture.
    // NOTE: The texture id in the renderer process corresponds to a different
    // texture id in the GPU process.
    uint32 texture_id() const
    {
        return texture_id_;
    }

    uint32 internal_texture_id() const { return internal_texture_id_; }

    const gpu::Mailbox& texture_mailbox() const
    {
        return texture_mailbox_;
    }

private:
    int32 id_;
    gfx::Size size_;
    uint32 texture_id_;
    uint32 internal_texture_id_;
    gpu::Mailbox texture_mailbox_;
};

// A decoded picture frame.
// This is the media-namespace equivalent of PP_Picture_Dev.
class MEDIA_EXPORT Picture {
public:
    Picture(int32 picture_buffer_id,
        int32 bitstream_buffer_id,
        const gfx::Rect& visible_rect,
        bool allow_overlay);

    // Returns the id of the picture buffer where this picture is contained.
    int32 picture_buffer_id() const
    {
        return picture_buffer_id_;
    }

    // Returns the id of the bitstream buffer from which this frame was decoded.
    int32 bitstream_buffer_id() const
    {
        return bitstream_buffer_id_;
    }

    void set_bitstream_buffer_id(int32 bitstream_buffer_id)
    {
        bitstream_buffer_id_ = bitstream_buffer_id;
    }

    // Returns the visible rectangle of the picture. Its size may be smaller
    // than the size of the PictureBuffer, as it is the only visible part of the
    // Picture contained in the PictureBuffer.
    gfx::Rect visible_rect() const { return visible_rect_; }

    bool allow_overlay() const { return allow_overlay_; }

private:
    int32 picture_buffer_id_;
    int32 bitstream_buffer_id_;
    gfx::Rect visible_rect_;
    bool allow_overlay_;
};

} // namespace media

#endif // MEDIA_VIDEO_PICTURE_H_
