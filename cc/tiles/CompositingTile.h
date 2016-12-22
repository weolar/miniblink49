#ifndef CompositingTile_h
#define CompositingTile_h

#include "SkRefCnt.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/skia/include/core/SkColor.h"

#include "public/platform/WebTraceLocation.h" // TODO

class SkBitmap;

namespace cc_blink {
class WebLayerImpl;
}

namespace cc {

class CompositingLayer;

class CompositingTile {
public:
    CompositingTile(CompositingLayer* compositingLayer, int xIndex, int yIndex);
    ~CompositingTile();

	void ref();
	void unref();
	int32_t getRefCnt() const;

    int xIndex() const { return m_xIndex; }
    int yIndex() const { return m_yIndex; }

    SkBitmap* bitmap() { return m_bitmap; }
    void clearBitmap();
    blink::IntRect postion() const;
    void eraseColor(const blink::IntRect& r, const SkColor* color);
    void allocBitmapIfNeeded();

    CompositingLayer* layer() const;

    bool isNotInit() { return m_isNotInit; } // for debug

private:
    CompositingLayer* m_compositingLayer;
	mutable int32_t m_refCnt;
    bool m_isNotInit; // for debug

    int m_xIndex;
    int m_yIndex;
    blink::IntRect m_postion;
    SkBitmap* m_bitmap;
};

}

#endif // CompositingTile_h