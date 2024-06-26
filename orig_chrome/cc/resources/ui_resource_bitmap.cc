// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/ui_resource_bitmap.h"

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkMallocPixelRef.h"
#include "third_party/skia/include/core/SkPixelRef.h"

namespace cc {
namespace {

    UIResourceBitmap::UIResourceFormat SkColorTypeToUIResourceFormat(
        SkColorType sk_type)
    {
        UIResourceBitmap::UIResourceFormat format = UIResourceBitmap::RGBA8;
        switch (sk_type) {
        case kN32_SkColorType:
            format = UIResourceBitmap::RGBA8;
            break;
        case kAlpha_8_SkColorType:
            format = UIResourceBitmap::ALPHA_8;
            break;
        default:
            NOTREACHED() << "Invalid SkColorType for UIResourceBitmap: " << sk_type;
            break;
        }
        return format;
    }

} // namespace

void UIResourceBitmap::Create(const skia::RefPtr<SkPixelRef>& pixel_ref,
    const gfx::Size& size,
    UIResourceFormat format)
{
    DCHECK(size.width());
    DCHECK(size.height());
    DCHECK(pixel_ref);
    DCHECK(pixel_ref->isImmutable());
    format_ = format;
    size_ = size;
    pixel_ref_ = pixel_ref;

    // Default values for secondary parameters.
    wrap_mode_ = CLAMP_TO_EDGE;
    opaque_ = (format == ETC1);
}

UIResourceBitmap::UIResourceBitmap(const SkBitmap& skbitmap)
{
    DCHECK_EQ(skbitmap.width(), skbitmap.rowBytesAsPixels());
    DCHECK(skbitmap.isImmutable());

    skia::RefPtr<SkPixelRef> pixel_ref = skia::SharePtr(skbitmap.pixelRef());
    const SkImageInfo& info = pixel_ref->info();
    Create(pixel_ref, gfx::Size(info.width(), info.height()),
        SkColorTypeToUIResourceFormat(skbitmap.colorType()));

    SetOpaque(skbitmap.isOpaque());
}

UIResourceBitmap::UIResourceBitmap(const gfx::Size& size, bool is_opaque)
{
    SkAlphaType alphaType = is_opaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType;
    SkImageInfo info = SkImageInfo::MakeN32(size.width(), size.height(), alphaType);
    skia::RefPtr<SkPixelRef> pixel_ref = skia::AdoptRef(
        SkMallocPixelRef::NewAllocate(info, info.minRowBytes(), NULL));
    pixel_ref->setImmutable();
    Create(pixel_ref, size, UIResourceBitmap::RGBA8);
    SetOpaque(is_opaque);
}

UIResourceBitmap::UIResourceBitmap(const skia::RefPtr<SkPixelRef>& pixel_ref,
    const gfx::Size& size)
{
    Create(pixel_ref, size, UIResourceBitmap::ETC1);
}

UIResourceBitmap::~UIResourceBitmap() { }

AutoLockUIResourceBitmap::AutoLockUIResourceBitmap(
    const UIResourceBitmap& bitmap)
    : bitmap_(bitmap)
{
    bitmap_.pixel_ref_->lockPixels();
}

AutoLockUIResourceBitmap::~AutoLockUIResourceBitmap()
{
    bitmap_.pixel_ref_->unlockPixels();
}

const uint8_t* AutoLockUIResourceBitmap::GetPixels() const
{
    return static_cast<const uint8_t*>(bitmap_.pixel_ref_->pixels());
}

} // namespace cc
