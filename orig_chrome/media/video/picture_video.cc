// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/video/picture.h"

namespace media {

PictureBuffer::PictureBuffer(int32 id, gfx::Size size, uint32 texture_id)
    : id_(id)
    , size_(size)
    , texture_id_(texture_id)
    , internal_texture_id_(0)
{
}

PictureBuffer::PictureBuffer(int32 id,
    gfx::Size size,
    uint32 texture_id,
    uint32 internal_texture_id)
    : id_(id)
    , size_(size)
    , texture_id_(texture_id)
    , internal_texture_id_(internal_texture_id)
{
}

PictureBuffer::PictureBuffer(int32 id,
    gfx::Size size,
    uint32 texture_id,
    const gpu::Mailbox& texture_mailbox)
    : id_(id)
    , size_(size)
    , texture_id_(texture_id)
    , internal_texture_id_(0)
    , texture_mailbox_(texture_mailbox)
{
}

Picture::Picture(int32 picture_buffer_id,
    int32 bitstream_buffer_id,
    const gfx::Rect& visible_rect,
    bool allow_overlay)
    : picture_buffer_id_(picture_buffer_id)
    , bitstream_buffer_id_(bitstream_buffer_id)
    , visible_rect_(visible_rect)
    , allow_overlay_(allow_overlay)
{
}

} // namespace media
