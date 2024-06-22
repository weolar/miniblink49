
#ifndef IntRectWin_h
#define IntRectWin_h

#include "third_party/WebKit/Source/platform/geometry/IntRect.h"

namespace blink {

inline IntRect winRectToIntRect(const RECT& rt)
{
    return IntRect(rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top);
}

inline RECT intRectToWinRect(const IntRect& rt)
{
    RECT r = { rt.x(), rt.y(), rt.maxX(), rt.maxY() };
    return r;
}

inline int intRectArea(const IntRect* r)
{
    return (r->width()) * (r->height());
}

inline IntRect intUnionRect(const IntRect& a, const IntRect& b)
{
    IntRect c = a;
    c.unite(b);
    return c;
}

inline int intUnionArea(IntRect* r1, IntRect* r2)
{
    IntRect area;
    area = intUnionRect(*r1, *r2);
    return intRectArea(&area);
}

} // namespace blink

#endif // IntRectWin_h