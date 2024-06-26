// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_external_bitmap_impl.h"

#include "cc/resources/shared_bitmap.h"

namespace cc_blink {

namespace {

    SharedBitmapAllocationFunction g_memory_allocator;

} // namespace

void SetSharedBitmapAllocationFunction(
    SharedBitmapAllocationFunction allocator)
{
    g_memory_allocator = allocator;
}

WebExternalBitmapImpl::WebExternalBitmapImpl()
{
}

WebExternalBitmapImpl::~WebExternalBitmapImpl()
{
}

void WebExternalBitmapImpl::setSize(blink::WebSize size)
{
    if (size != size_) {
        shared_bitmap_ = g_memory_allocator(gfx::Size(size));
        size_ = size;
    }
}

blink::WebSize WebExternalBitmapImpl::size()
{
    return size_;
}

uint8* WebExternalBitmapImpl::pixels()
{
    if (!shared_bitmap_) {
        // crbug.com/520417: not sure why a non-null WebExternalBitmap is
        // being passed to prepareMailbox when the shared_bitmap_ is null.
        // Best hypothesis is that the bitmap is zero-sized.
        DCHECK(size_.isEmpty());
        return nullptr;
    }
    return shared_bitmap_->pixels();
}

} // namespace cc_blink
