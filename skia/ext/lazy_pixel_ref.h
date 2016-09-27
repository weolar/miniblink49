// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SKIA_EXT_LAZY_PIXEL_REF_H_
#define SKIA_EXT_LAZY_PIXEL_REF_H_

#include "third_party/skia/include/core/SkPixelRef.h"
#include "third_party/skia/include/core/SkRect.h"

namespace skia {

// This class extends SkPixelRef to facilitate lazy image decoding on the impl
// thread.
class SK_API LazyPixelRef : public SkPixelRef {
 public:
  explicit LazyPixelRef(const SkImageInfo& info);
  virtual ~LazyPixelRef();

  struct PrepareParams {
    // Clipping rect for this pixel ref.
    SkIRect clip_rect;
  };

  // Request the ImageDecodingStore to prepare image decoding for the
  // given clipping rect. Returns true is succeeded, or false otherwise.
  virtual bool PrepareToDecode(const PrepareParams& params) = 0;

  // Returns true if this pixel ref is already in the ImageDecodingStore's
  // cache, false otherwise. Much cheaper than PrepareToDecode().
  virtual bool MaybeDecoded() = 0;

  // Start decoding the image.
  virtual void Decode() = 0;
};

}  // namespace skia

#endif  // SKIA_EXT_LAZY_PIXEL_REF_H_
