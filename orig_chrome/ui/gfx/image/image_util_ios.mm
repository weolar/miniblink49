// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <UIKit/UIKit.h>

#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_util.h"

#include "base/logging.h"

namespace gfx {

bool JPEG1xEncodedDataFromImage(const Image& image,
                                int quality,
                                std::vector<unsigned char>* dst) {
  NSData* data = UIImageJPEGRepresentation(image.ToUIImage(), quality / 100.0);

  if ([data length] == 0)
    return false;

  dst->resize([data length]);
  [data getBytes:&dst->at(0) length:[data length]];
  return true;
}

}  // end namespace gfx
