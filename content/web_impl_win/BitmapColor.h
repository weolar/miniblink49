
#include "third_party/skia/include/core/SkBitmap.h"

namespace content {

inline bool bitmapHasInvalidPremultipliedColors(const SkBitmap& bitmap)
{
    for (int x = 0; x < bitmap.width(); ++x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            uint32_t pixel = *bitmap.getAddr32(x, y);
            if (SkColorGetR(pixel) > SkColorGetA(pixel) ||
                SkColorGetG(pixel) > SkColorGetA(pixel) ||
                SkColorGetB(pixel) > SkColorGetA(pixel))
                return true;
        }
    }
    return false;
}

inline void makeBitmapOpaque(const SkBitmap& bitmap)
{
    for (int x = 0; x < bitmap.width(); ++x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            *bitmap.getAddr32(x, y) = SkColorSetA(*bitmap.getAddr32(x, y), 0xFF);
        }
    }
}

}

