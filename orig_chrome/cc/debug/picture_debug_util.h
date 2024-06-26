// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_PICTURE_DEBUG_UTIL_H_
#define CC_DEBUG_PICTURE_DEBUG_UTIL_H_

#include <string>

class SkPicture;

namespace cc {

class PictureDebugUtil {
public:
    static void SerializeAsBase64(const SkPicture* picture, std::string* output);
};

} // namespace cc

#endif // CC_DEBUG_PICTURE_DEBUG_UTIL_H_
