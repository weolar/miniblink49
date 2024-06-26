// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_COPY_OUTPUT_RESULT_H_
#define CC_OUTPUT_COPY_OUTPUT_RESULT_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/resources/single_release_callback.h"
#include "cc/resources/texture_mailbox.h"
#include "ui/gfx/geometry/size.h"

class SkBitmap;

namespace cc {
class TextureMailbox;

class CC_EXPORT CopyOutputResult {
public:
    static scoped_ptr<CopyOutputResult> CreateEmptyResult()
    {
        return make_scoped_ptr(new CopyOutputResult);
    }
    static scoped_ptr<CopyOutputResult> CreateBitmapResult(
        scoped_ptr<SkBitmap> bitmap)
    {
        return make_scoped_ptr(new CopyOutputResult(bitmap.Pass()));
    }
    static scoped_ptr<CopyOutputResult> CreateTextureResult(
        const gfx::Size& size,
        const TextureMailbox& texture_mailbox,
        scoped_ptr<SingleReleaseCallback> release_callback)
    {
        return make_scoped_ptr(
            new CopyOutputResult(size, texture_mailbox, release_callback.Pass()));
    }

    ~CopyOutputResult();

    bool IsEmpty() const { return !HasBitmap() && !HasTexture(); }
    bool HasBitmap() const { return !!bitmap_; }
    bool HasTexture() const { return texture_mailbox_.IsValid(); }

    gfx::Size size() const { return size_; }
    scoped_ptr<SkBitmap> TakeBitmap();
    void TakeTexture(TextureMailbox* texture_mailbox,
        scoped_ptr<SingleReleaseCallback>* release_callback);

private:
    CopyOutputResult();
    explicit CopyOutputResult(scoped_ptr<SkBitmap> bitmap);
    explicit CopyOutputResult(const gfx::Size& size,
        const TextureMailbox& texture_mailbox,
        scoped_ptr<SingleReleaseCallback> release_callback);

    gfx::Size size_;
    scoped_ptr<SkBitmap> bitmap_;
    TextureMailbox texture_mailbox_;
    scoped_ptr<SingleReleaseCallback> release_callback_;
};

} // namespace cc

#endif // CC_OUTPUT_COPY_OUTPUT_RESULT_H_
