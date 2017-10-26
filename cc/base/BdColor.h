#ifndef cc_BgColor_h
#define cc_BgColor_h

#include "third_party/skia/include/core/SkColor.h"

namespace cc {
#ifndef NDEBUG
const SkColor s_kBgColor = 0xffff00ff;
#else
const SkColor s_kBgColor = 0xffffffff;
#endif

inline SkColor getRealColor(bool hasTransparentBackground, SkColor c)
{
    return (hasTransparentBackground ? (0x00ffffff & c) : (0xff000000 | c));
}

}

#endif // cc_BgColor_h