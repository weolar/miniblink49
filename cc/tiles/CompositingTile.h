#ifndef CompositingTile_h
#define CompositingTile_h

#include "SkRefCnt.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/skia/include/core/SkColor.h"

#include "public/platform/WebTraceLocation.h"
#include "cc/tiles/TileBase.h"

class SkBitmap;

namespace cc_blink {
class WebLayerImpl;
}

namespace cc {

class CompositingLayer;

class CompositingTile : public TileBase {
public:
    CompositingTile();
    ~CompositingTile();

    virtual TileBase* init(void* parent, int xIndex, int yIndex) override;
    virtual void ref(const blink::WebTraceLocation&) override;
    virtual void unref(const blink::WebTraceLocation&) override;
    int32_t getRefCnt() const;

    virtual int xIndex() const override { return m_xIndex; }
    virtual int yIndex() const override { return m_yIndex; }

    SkBitmap* bitmap() { return m_bitmap; }
    void clearBitmap();
    blink::IntRect postion() const;
    void eraseColor(const blink::IntRect& r, const SkColor* color);
    void allocBitmapIfNeeded(SkColor* solidColor, bool isSolidColorCoverWholeTile);

    CompositingLayer* layer() const;

    bool isNotInit() { return m_isNotInit; } // for debug

    void setSolidColor(SkColor color);
    void cancelSolidColor();
    bool isSolidColor() const { return !!m_solidColor; }
    SkColor* getSolidColor() const { return m_solidColor; }

private:
    void resizeBitmap(int dstWidth, int dstHeight, SkColor backgroundColor);
    SkBitmap* allocBitmap(int width, int height, SkColor getBackgroundColor);

    CompositingLayer* m_compositingLayer;
    mutable int32_t m_refCnt;
    bool m_isNotInit; // for debug

    int m_xIndex;
    int m_yIndex;
    blink::IntRect m_postion;
    blink::IntSize m_layerBounds;
    SkBitmap* m_bitmap;

    SkColor* m_solidColor;
};

}

#endif // CompositingTile_h