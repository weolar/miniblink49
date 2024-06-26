// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_REGION_H_
#define CC_BASE_REGION_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/skia_util.h"

namespace base {
class Value;
namespace trace_event {
    class TracedValue;
}
}

namespace cc {
class SimpleEnclosedRegion;

class CC_EXPORT Region {
public:
    Region();
    Region(const Region& region);
    Region(const gfx::Rect& rect); // NOLINT(runtime/explicit)
    ~Region();

    const Region& operator=(const gfx::Rect& rect);
    const Region& operator=(const Region& region);

    void Swap(Region* region);
    void Clear();
    bool IsEmpty() const;
    int GetRegionComplexity() const;

    bool Contains(const gfx::Point& point) const;
    bool Contains(const gfx::Rect& rect) const;
    bool Contains(const Region& region) const;

    bool Intersects(const gfx::Rect& rect) const;
    bool Intersects(const Region& region) const;

    void Subtract(const gfx::Rect& rect);
    void Subtract(const Region& region);
    void Subtract(const SimpleEnclosedRegion& region);
    void Union(const gfx::Rect& rect);
    void Union(const Region& region);
    void Intersect(const gfx::Rect& rect);
    void Intersect(const Region& region);

    bool Equals(const Region& other) const
    {
        return skregion_ == other.skregion_;
    }

    gfx::Rect bounds() const
    {
        return gfx::SkIRectToRect(skregion_.getBounds());
    }

    std::string ToString() const;
    scoped_ptr<base::Value> AsValue() const;
    void AsValueInto(base::trace_event::TracedValue* array) const;

    class CC_EXPORT Iterator {
    public:
        Iterator();
        explicit Iterator(const Region& region);
        ~Iterator();

        gfx::Rect rect() const
        {
            return gfx::SkIRectToRect(it_.rect());
        }

        void next()
        {
            it_.next();
        }

        bool has_rect() const
        {
            return !it_.done();
        }

    private:
        SkRegion::Iterator it_;
    };

private:
    SkRegion skregion_;
};

inline bool operator==(const Region& a, const Region& b)
{
    return a.Equals(b);
}

inline bool operator!=(const Region& a, const Region& b)
{
    return !(a == b);
}

inline Region SubtractRegions(const Region& a, const Region& b)
{
    Region result = a;
    result.Subtract(b);
    return result;
}

inline Region SubtractRegions(const Region& a, const gfx::Rect& b)
{
    Region result = a;
    result.Subtract(b);
    return result;
}

inline Region IntersectRegions(const Region& a, const Region& b)
{
    Region result = a;
    result.Intersect(b);
    return result;
}

inline Region IntersectRegions(const Region& a, const gfx::Rect& b)
{
    Region result = a;
    result.Intersect(b);
    return result;
}

inline Region UnionRegions(const Region& a, const Region& b)
{
    Region result = a;
    result.Union(b);
    return result;
}

inline Region UnionRegions(const Region& a, const gfx::Rect& b)
{
    Region result = a;
    result.Union(b);
    return result;
}

} // namespace cc

#endif // CC_BASE_REGION_H_
