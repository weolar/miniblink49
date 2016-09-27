// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "skia/ext/bitmap_platform_device_mac.h"

#import <ApplicationServices/ApplicationServices.h>
#include <time.h>

#include "base/mac/mac_util.h"
#include "base/memory/ref_counted.h"
#include "skia/ext/bitmap_platform_device.h"
#include "skia/ext/platform_canvas.h"
#include "skia/ext/skia_utils_mac.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkTypes.h"
#include "third_party/skia/include/core/SkUtils.h"

namespace skia {

namespace {

static CGContextRef CGContextForData(void* data, int width, int height) {
#define HAS_ARGB_SHIFTS(a, r, g, b) \
            (SK_A32_SHIFT == (a) && SK_R32_SHIFT == (r) \
             && SK_G32_SHIFT == (g) && SK_B32_SHIFT == (b))
#if defined(SK_CPU_LENDIAN) && HAS_ARGB_SHIFTS(24, 16, 8, 0)
  // Allocate a bitmap context with 4 components per pixel (BGRA).  Apple
  // recommends these flags for improved CG performance.

  // CGBitmapContextCreate returns NULL if width/height are 0. However, our
  // callers expect to get a canvas back (which they later resize/reallocate)
  // so we pin the dimensions here.
  width = SkMax32(1, width);
  height = SkMax32(1, height);
  CGContextRef context =
      CGBitmapContextCreate(data, width, height, 8, width * 4,
                            base::mac::GetSystemColorSpace(),
                            kCGImageAlphaPremultipliedFirst |
                                kCGBitmapByteOrder32Host);
#else
#error We require that Skia's and CoreGraphics's recommended \
       image memory layout match.
#endif
#undef HAS_ARGB_SHIFTS

  if (!context)
    return NULL;

  // Change the coordinate system to match WebCore's
  CGContextTranslateCTM(context, 0, height);
  CGContextScaleCTM(context, 1.0, -1.0);

  return context;
}

}  // namespace

void BitmapPlatformDevice::ReleaseBitmapContext() {
  SkASSERT(bitmap_context_);
  CGContextRelease(bitmap_context_);
  bitmap_context_ = NULL;
}

void BitmapPlatformDevice::SetMatrixClip(
    const SkMatrix& transform,
    const SkRegion& region) {
  transform_ = transform;
  clip_region_ = region;
  config_dirty_ = true;
}

void BitmapPlatformDevice::LoadConfig() {
  if (!config_dirty_ || !bitmap_context_)
    return;  // Nothing to do.
  config_dirty_ = false;

  // We must restore and then save the state of the graphics context since the
  // calls to Load the clipping region to the context are strictly cummulative,
  // i.e., you can't replace a clip rect, other than with a save/restore.
  // But this implies that no other changes to the state are done elsewhere.
  // If we ever get to need to change this, then we must replace the clip rect
  // calls in LoadClippingRegionToCGContext() with an image mask instead.
  CGContextRestoreGState(bitmap_context_);
  CGContextSaveGState(bitmap_context_);
  LoadTransformToCGContext(bitmap_context_, transform_);
  LoadClippingRegionToCGContext(bitmap_context_, clip_region_, transform_);
}


// We use this static factory function instead of the regular constructor so
// that we can create the pixel data before calling the constructor. This is
// required so that we can call the base class' constructor with the pixel
// data.
BitmapPlatformDevice* BitmapPlatformDevice::Create(CGContextRef context,
                                                   int width,
                                                   int height,
                                                   bool is_opaque) {
  if (RasterDeviceTooBigToAllocate(width, height))
    return NULL;

  SkBitmap bitmap;
  // TODO: verify that the CG Context's pixels will have tight rowbytes or pass in the correct
  // rowbytes for the case when context != NULL.
  bitmap.setInfo(SkImageInfo::MakeN32(width, height, is_opaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType));

  void* data;
  if (context) {
    data = CGBitmapContextGetData(context);
    bitmap.setPixels(data);
  } else {
    if (!bitmap.allocPixels())
      return NULL;
    data = bitmap.getPixels();
  }

  // If we were given data, then don't clobber it!
#ifndef NDEBUG
  if (!context && is_opaque) {
    // To aid in finding bugs, we set the background color to something
    // obviously wrong so it will be noticable when it is not cleared
    bitmap.eraseARGB(255, 0, 255, 128);  // bright bluish green
  }
#endif

  if (!context) {
    context = CGContextForData(data, width, height);
    if (!context)
      return NULL;
  } else
    CGContextRetain(context);

  BitmapPlatformDevice* rv = new BitmapPlatformDevice(context, bitmap);

  // The device object took ownership of the graphics context with its own
  // CGContextRetain call.
  CGContextRelease(context);

  return rv;
}

BitmapPlatformDevice* BitmapPlatformDevice::CreateAndClear(int width,
                                                           int height,
                                                           bool is_opaque) {
  BitmapPlatformDevice* device = Create(NULL, width, height, is_opaque);
  if (!is_opaque)
    device->clear(0);
  return device;
}

BitmapPlatformDevice* BitmapPlatformDevice::CreateWithData(uint8_t* data,
                                                           int width,
                                                           int height,
                                                           bool is_opaque) {
  CGContextRef context = NULL;
  if (data)
    context = CGContextForData(data, width, height);

  BitmapPlatformDevice* rv = Create(context, width, height, is_opaque);

  // The device object took ownership of the graphics context with its own
  // CGContextRetain call.
  if (context)
    CGContextRelease(context);

  return rv;
}

// The device will own the bitmap, which corresponds to also owning the pixel
// data. Therefore, we do not transfer ownership to the SkBitmapDevice's bitmap.
BitmapPlatformDevice::BitmapPlatformDevice(
    CGContextRef context, const SkBitmap& bitmap)
    : SkBitmapDevice(bitmap),
      bitmap_context_(context),
      config_dirty_(true),  // Want to load the config next time.
      transform_(SkMatrix::I()) {
  SetPlatformDevice(this, this);
  SkASSERT(bitmap_context_);
  // Initialize the clip region to the entire bitmap.

  SkIRect rect;
  rect.set(0, 0,
           CGBitmapContextGetWidth(bitmap_context_),
           CGBitmapContextGetHeight(bitmap_context_));
  clip_region_ = SkRegion(rect);
  CGContextRetain(bitmap_context_);
  // We must save the state once so that we can use the restore/save trick
  // in LoadConfig().
  CGContextSaveGState(bitmap_context_);
}

BitmapPlatformDevice::~BitmapPlatformDevice() {
  if (bitmap_context_)
    CGContextRelease(bitmap_context_);
}

CGContextRef BitmapPlatformDevice::GetBitmapContext() {
  LoadConfig();
  return bitmap_context_;
}

void BitmapPlatformDevice::setMatrixClip(const SkMatrix& transform,
                                         const SkRegion& region,
                                         const SkClipStack&) {
  SetMatrixClip(transform, region);
}

void BitmapPlatformDevice::DrawToNativeContext(CGContextRef context, int x,
                                               int y, const CGRect* src_rect) {
  bool created_dc = false;
  if (!bitmap_context_) {
    created_dc = true;
    GetBitmapContext();
  }

  // this should not make a copy of the bits, since we're not doing
  // anything to trigger copy on write
  CGImageRef image = CGBitmapContextCreateImage(bitmap_context_);
  CGRect bounds;
  bounds.origin.x = x;
  bounds.origin.y = y;
  if (src_rect) {
    bounds.size.width = src_rect->size.width;
    bounds.size.height = src_rect->size.height;
    CGImageRef sub_image = CGImageCreateWithImageInRect(image, *src_rect);
    CGContextDrawImage(context, bounds, sub_image);
    CGImageRelease(sub_image);
  } else {
    bounds.size.width = width();
    bounds.size.height = height();
    CGContextDrawImage(context, bounds, image);
  }
  CGImageRelease(image);

  if (created_dc)
    ReleaseBitmapContext();
}

SkBaseDevice* BitmapPlatformDevice::onCreateDevice(const SkImageInfo& info,
                                                   Usage /*usage*/) {
  SkASSERT(info.colorType() == kPMColor_SkColorType);
  return BitmapPlatformDevice::CreateAndClear(info.width(), info.height(),
                                                info.isOpaque());
}

// PlatformCanvas impl

SkCanvas* CreatePlatformCanvas(CGContextRef ctx, int width, int height,
                               bool is_opaque, OnFailureType failureType) {
  skia::RefPtr<SkBaseDevice> dev = skia::AdoptRef(
      BitmapPlatformDevice::Create(ctx, width, height, is_opaque));
  return CreateCanvas(dev, failureType);
}

SkCanvas* CreatePlatformCanvas(int width, int height, bool is_opaque,
                               uint8_t* data, OnFailureType failureType) {
  skia::RefPtr<SkBaseDevice> dev = skia::AdoptRef(
      BitmapPlatformDevice::CreateWithData(data, width, height, is_opaque));
  return CreateCanvas(dev, failureType);
}

// Port of PlatformBitmap to mac

PlatformBitmap::~PlatformBitmap() {
  if (surface_)
    CGContextRelease(surface_);
}

bool PlatformBitmap::Allocate(int width, int height, bool is_opaque) {
  if (RasterDeviceTooBigToAllocate(width, height))
    return false;
    
  if (!bitmap_.allocN32Pixels(width, height, is_opaque))
    return false;

  if (!is_opaque)
    bitmap_.eraseColor(0);

  surface_ = CGContextForData(bitmap_.getPixels(), bitmap_.width(),
                              bitmap_.height());
  return true;
}

}  // namespace skia
