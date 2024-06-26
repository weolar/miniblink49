// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_EXTERNAL_BITMAP_IMPL_H_
#define CC_BLINK_WEB_EXTERNAL_BITMAP_IMPL_H_

#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebExternalBitmap.h"

namespace cc {
class SharedBitmap;
}

namespace cc_blink {

typedef scoped_ptr<cc::SharedBitmap> (*SharedBitmapAllocationFunction)(
    const gfx::Size& size);

// Sets the function that this will use to allocate shared memory.
CC_BLINK_EXPORT void SetSharedBitmapAllocationFunction(
    SharedBitmapAllocationFunction);

class WebExternalBitmapImpl : public blink::WebExternalBitmap {
public:
    CC_BLINK_EXPORT explicit WebExternalBitmapImpl();
    virtual ~WebExternalBitmapImpl();

    // blink::WebExternalBitmap implementation.
    blink::WebSize size() override;
    void setSize(blink::WebSize size) override;
    uint8* pixels() override;

    cc::SharedBitmap* shared_bitmap() { return shared_bitmap_.get(); }

private:
    scoped_ptr<cc::SharedBitmap> shared_bitmap_;
    blink::WebSize size_;

    DISALLOW_COPY_AND_ASSIGN(WebExternalBitmapImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_EXTERNAL_BITMAP_IMPL_H_
