#ifndef mc_BgColor_h
#define mc_BgColor_h

#include "third_party/skia/include/core/SkColor.h"

namespace mc {
#ifndef NDEBUG
const SkColor s_kBgColor = 0xffffffff;
#else
const SkColor s_kBgColor = 0xffffffff;
#endif

inline SkColor getRealColor(bool hasTransparentBackground, SkColor c)
{
    return (hasTransparentBackground ? (0x00ffffff & c) : (0xff000000 | c));
}

}

#endif // mc_BgColor_h