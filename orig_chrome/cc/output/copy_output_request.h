// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_COPY_OUTPUT_REQUEST_H_
#define CC_OUTPUT_COPY_OUTPUT_REQUEST_H_

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/resources/single_release_callback.h"
#include "cc/resources/texture_mailbox.h"
#include "ui/gfx/geometry/rect.h"

class SkBitmap;

namespace cc {
class CopyOutputResult;

class CC_EXPORT CopyOutputRequest {
public:
    typedef base::Callback<void(scoped_ptr<CopyOutputResult> result)>
        CopyOutputRequestCallback;

    static scoped_ptr<CopyOutputRequest> CreateEmptyRequest()
    {
        return make_scoped_ptr(new CopyOutputRequest);
    }
    static scoped_ptr<CopyOutputRequest> CreateRequest(
        const CopyOutputRequestCallback& result_callback)
    {
        return make_scoped_ptr(new CopyOutputRequest(false, result_callback));
    }
    static scoped_ptr<CopyOutputRequest> CreateBitmapRequest(
        const CopyOutputRequestCallback& result_callback)
    {
        return make_scoped_ptr(new CopyOutputRequest(true, result_callback));
    }
    static scoped_ptr<CopyOutputRequest> CreateRelayRequest(
        const CopyOutputRequest& original_request,
        const CopyOutputRequestCallback& result_callback);

    ~CopyOutputRequest();

    bool IsEmpty() const { return result_callback_.is_null(); }

    // Optionally specify the source of this copy request.  If set when this copy
    // request is submitted to a layer, a prior uncommitted copy request from the
    // same |source| will be aborted.
    void set_source(void* source) { source_ = source; }
    void* source() const { return source_; }

    bool force_bitmap_result() const { return force_bitmap_result_; }

    // By default copy requests copy the entire layer's subtree output. If an
    // area is given, then the intersection of this rect (in layer space) with
    // the layer's subtree output will be returned.
    void set_area(const gfx::Rect& area)
    {
        has_area_ = true;
        area_ = area;
    }
    bool has_area() const { return has_area_; }
    gfx::Rect area() const { return area_; }

    // By default copy requests create a new TextureMailbox to return contents
    // in. This allows a client to provide a TextureMailbox, and the compositor
    // will place the result inside the TextureMailbox.
    void SetTextureMailbox(const TextureMailbox& texture_mailbox);
    bool has_texture_mailbox() const { return has_texture_mailbox_; }
    const TextureMailbox& texture_mailbox() const { return texture_mailbox_; }

    void SendEmptyResult();
    void SendBitmapResult(scoped_ptr<SkBitmap> bitmap);
    void SendTextureResult(const gfx::Size& size,
        const TextureMailbox& texture_mailbox,
        scoped_ptr<SingleReleaseCallback> release_callback);

    void SendResult(scoped_ptr<CopyOutputResult> result);

private:
    CopyOutputRequest();
    CopyOutputRequest(bool force_bitmap_result,
        const CopyOutputRequestCallback& result_callback);

    void* source_;
    bool force_bitmap_result_;
    bool has_area_;
    bool has_texture_mailbox_;
    gfx::Rect area_;
    TextureMailbox texture_mailbox_;
    CopyOutputRequestCallback result_callback_;
};

} // namespace cc

#endif // CC_OUTPUT_COPY_OUTPUT_REQUEST_H_
