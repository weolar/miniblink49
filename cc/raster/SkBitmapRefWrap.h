#ifndef SkBitmapRefWrap_h
#define SkBitmapRefWrap_h

#include "third_party/skia/include/core/SkAtomics.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace cc {

class SkBitmapRefWrap {
public:
    SkBitmapRefWrap()
    {
        m_bitmap = nullptr;
        m_ref = 0;
    }

    ~SkBitmapRefWrap()
    {
        if (m_bitmap)
            delete m_bitmap;
    }

    SkBitmapRefWrap* ref()
    {
        sk_atomic_inc(&m_ref);
        return this;
    }

    void deref()
    {
        sk_atomic_dec(&m_ref);
        if (m_ref <= 0)
            delete this;
    }

    int getRef() { return m_ref; }

    SkBitmap* get() { return m_bitmap; }

    void set(SkBitmap* bitmap)
    {
        ASSERT(!m_bitmap);
        m_bitmap = bitmap;
    }

private:
    SkBitmap* m_bitmap;
    int m_ref;
};

} // cc

#endif // SkBitmapRefWrap_h