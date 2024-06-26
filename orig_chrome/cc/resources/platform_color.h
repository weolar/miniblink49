// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_PLATFORM_COLOR_H_
#define CC_RESOURCES_PLATFORM_COLOR_H_

#include "base/basictypes.h"
#include "base/logging.h"
#include "cc/resources/resource_format.h"
#include "third_party/skia/include/core/SkTypes.h"

namespace cc {

class PlatformColor {
public:
    enum SourceDataFormat {
        SOURCE_FORMAT_RGBA8,
        SOURCE_FORMAT_BGRA8
    };

    static SourceDataFormat Format()
    {
        return SK_B32_SHIFT ? SOURCE_FORMAT_RGBA8 : SOURCE_FORMAT_BGRA8;
    }

    // Returns the most efficient texture format for this platform.
    static ResourceFormat BestTextureFormat(bool supports_bgra8888)
    {
        switch (Format()) {
        case SOURCE_FORMAT_BGRA8:
            return (supports_bgra8888) ? BGRA_8888 : RGBA_8888;
        case SOURCE_FORMAT_RGBA8:
            return RGBA_8888;
        }
        NOTREACHED();
        return RGBA_8888;
    }

    // Return true if the given texture format has the same component order
    // as the color on this platform.
    static bool SameComponentOrder(ResourceFormat format)
    {
        switch (Format()) {
        case SOURCE_FORMAT_RGBA8:
            return format == RGBA_8888 || format == RGBA_4444;
        case SOURCE_FORMAT_BGRA8:
            return format == BGRA_8888 || format == RGBA_4444;
        }
        NOTREACHED();
        return false;
    }

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(PlatformColor);
};

} // namespace cc

#endif // CC_RESOURCES_PLATFORM_COLOR_H_
