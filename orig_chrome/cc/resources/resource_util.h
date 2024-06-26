// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RESOURCE_UTIL_H_
#define CC_RESOURCES_RESOURCE_UTIL_H_

#include <limits>

#include "base/numerics/safe_math.h"
#include "cc/base/cc_export.h"
#include "cc/base/math_util.h"
#include "cc/resources/resource_format.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT ResourceUtil {
public:
    // Returns true if the width is valid and fits in bytes, false otherwise.
    template <typename T>
    static bool VerifyWidthInBytes(int width, ResourceFormat format);
    // Returns true if the size is valid and fits in bytes, false otherwise.
    template <typename T>
    static bool VerifySizeInBytes(const gfx::Size& size, ResourceFormat format);

    // Dies with a CRASH() if the width can not be represented as a positive
    // number of bytes.
    template <typename T>
    static T CheckedWidthInBytes(int width, ResourceFormat format);
    // Dies with a CRASH() if the size can not be represented as a positive
    // number of bytes.
    template <typename T>
    static T CheckedSizeInBytes(const gfx::Size& size, ResourceFormat format);

    // Returns the width in bytes but may overflow or return 0. Only do this for
    // computing widths for sizes that have already been checked.
    template <typename T>
    static T UncheckedWidthInBytes(int width, ResourceFormat format);
    // Returns the size in bytes but may overflow or return 0. Only do this for
    // sizes that have already been checked.
    template <typename T>
    static T UncheckedSizeInBytes(const gfx::Size& size, ResourceFormat format);
    // Returns the width in bytes aligned but may overflow or return 0. Only do
    // this for computing widths for sizes that have already been checked.
    template <typename T>
    static T UncheckedWidthInBytesAligned(int width, ResourceFormat format);
    // Returns the size in bytes aligned but may overflow or return 0. Only do
    // this for sizes that have already been checked.
    template <typename T>
    static T UncheckedSizeInBytesAligned(const gfx::Size& size,
        ResourceFormat format);

private:
    // TODO(prashant.n): Replace IsSameType with std::is_same once C++11 is used
    // on all platforms.
    template <typename T, typename U>
    struct IsSameType {
        static const bool value = false;
    };

    template <typename T>
    struct IsSameType<T, T> {
        static const bool value = true;
    };

    template <typename T>
    static inline void VerifyType();

    template <typename T>
    static bool VerifyFitsInBytesInternal(int width,
        int height,
        ResourceFormat format,
        bool verify_size,
        bool aligned);

    template <typename T>
    static T BytesInternal(int width,
        int height,
        ResourceFormat format,
        bool verify_size,
        bool aligned);

    DISALLOW_COPY_AND_ASSIGN(ResourceUtil);
};

template <typename T>
bool ResourceUtil::VerifyWidthInBytes(int width, ResourceFormat format)
{
    VerifyType<T>();
    return VerifyFitsInBytesInternal<T>(width, 0, format, false, false);
}

template <typename T>
bool ResourceUtil::VerifySizeInBytes(const gfx::Size& size,
    ResourceFormat format)
{
    VerifyType<T>();
    return VerifyFitsInBytesInternal<T>(size.width(), size.height(), format, true,
        false);
}

template <typename T>
T ResourceUtil::CheckedWidthInBytes(int width, ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(width, 0, format, false, false));
    base::CheckedNumeric<T> checked_value = BitsPerPixel(format);
    checked_value *= width;
    checked_value = MathUtil::CheckedRoundUp<T>(checked_value.ValueOrDie(), 8);
    checked_value /= 8;
    return checked_value.ValueOrDie();
}

template <typename T>
T ResourceUtil::CheckedSizeInBytes(const gfx::Size& size,
    ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(size.width(), size.height(), format, true,
        false));
    base::CheckedNumeric<T> checked_value = BitsPerPixel(format);
    checked_value *= size.width();
    checked_value = MathUtil::CheckedRoundUp<T>(checked_value.ValueOrDie(), 8);
    checked_value /= 8;
    checked_value *= size.height();
    return checked_value.ValueOrDie();
}

template <typename T>
T ResourceUtil::UncheckedWidthInBytes(int width, ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(width, 0, format, false, false));
    return BytesInternal<T>(width, 0, format, false, false);
}

template <typename T>
T ResourceUtil::UncheckedSizeInBytes(const gfx::Size& size,
    ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(size.width(), size.height(), format, true,
        false));
    return BytesInternal<T>(size.width(), size.height(), format, true, false);
}

template <typename T>
T ResourceUtil::UncheckedWidthInBytesAligned(int width, ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(width, 0, format, false, true));
    return BytesInternal<T>(width, 0, format, false, true);
}

template <typename T>
T ResourceUtil::UncheckedSizeInBytesAligned(const gfx::Size& size,
    ResourceFormat format)
{
    VerifyType<T>();
    DCHECK(VerifyFitsInBytesInternal<T>(size.width(), size.height(), format, true,
        true));
    return BytesInternal<T>(size.width(), size.height(), format, true, true);
}

template <typename T>
void ResourceUtil::VerifyType()
{
    static_assert(
        std::numeric_limits<T>::is_integer && !IsSameType<T, bool>::value,
        "T must be non-bool integer type. Preferred type is size_t.");
}

template <typename T>
bool ResourceUtil::VerifyFitsInBytesInternal(int width,
    int height,
    ResourceFormat format,
    bool verify_size,
    bool aligned)
{
    base::CheckedNumeric<T> checked_value = BitsPerPixel(format);
    checked_value *= width;
    if (!checked_value.IsValid())
        return false;

    // Roundup bits to byte (8 bits) boundary. If width is 3 and BitsPerPixel is
    // 4, then it should return 16, so that row pixels do not get truncated.
    checked_value = MathUtil::UncheckedRoundUp<T>(checked_value.ValueOrDie(), 8);

    // If aligned is true, bytes are aligned on 4-bytes boundaries for upload
    // performance, assuming that GL_PACK_ALIGNMENT or GL_UNPACK_ALIGNMENT have
    // not changed from default.
    if (aligned) {
        checked_value /= 8;
        if (!checked_value.IsValid())
            return false;
        checked_value = MathUtil::UncheckedRoundUp<T>(checked_value.ValueOrDie(), 4);
        checked_value *= 8;
    }

    if (verify_size)
        checked_value *= height;
    if (!checked_value.IsValid())
        return false;
    T value = checked_value.ValueOrDie();
    if ((value % 8) != 0)
        return false;
    return true;
}

template <typename T>
T ResourceUtil::BytesInternal(int width,
    int height,
    ResourceFormat format,
    bool verify_size,
    bool aligned)
{
    T bytes = BitsPerPixel(format);
    bytes *= width;
    bytes = MathUtil::UncheckedRoundUp<T>(bytes, 8);
    bytes /= 8;
    if (aligned)
        bytes = MathUtil::UncheckedRoundUp<T>(bytes, 4);
    if (verify_size)
        bytes *= height;

    return bytes;
}

} // namespace cc

#endif // CC_RESOURCES_RESOURCE_UTIL_H_
