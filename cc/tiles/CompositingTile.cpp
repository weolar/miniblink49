
#include "cc/tiles/CompositingTile.h"

#include "cc/tiles/TileWidthHeight.h"
#include "cc/layers/CompositingLayer.h"
#include "cc/trees/DrawProperties.h"
#include "cc/base/bdcolor.h"

#include "SkBitmap.h"
#include "SkCanvas.h"

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"
#include "WTF/text/WTFString.h"

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, compositingTileCounter, ("compositingTileCounter"));
#endif

CompositingTile::CompositingTile()
    : TileBase()
{

}

TileBase* CompositingTile::init(void* parent, int xIndex, int yIndex)
{
    m_compositingLayer = (CompositingLayer*)parent;
    m_isNotInit = true;
    m_refCnt = 1;
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_postion = blink::IntRect(xIndex * kDefaultTileWidth, yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
    m_bitmap = nullptr;
    m_solidColor = new SkColor(0xffffffff);
#ifndef NDEBUG
    compositingTileCounter.increment();
#endif
    return this;
}

CompositingTile::~CompositingTile()
{
    clearBitmap();

    if (m_solidColor)
        delete m_solidColor;
#ifndef NDEBUG
    compositingTileCounter.decrement();
#endif
}

void CompositingTile::ref(const blink::WebTraceLocation&)
{
	  ASSERT(m_refCnt > 0);
	  (void)sk_atomic_fetch_add(&m_refCnt, +1, sk_memory_order_relaxed);  // No barrier required.
}

void CompositingTile::unref(const blink::WebTraceLocation&)
{
    ASSERT(m_refCnt > 0);
    if (1 == sk_atomic_fetch_add(&m_refCnt, -1, sk_memory_order_acq_rel)) {
        delete this;
	  }
}

int32_t CompositingTile::getRefCnt() const
{
	  return m_refCnt;
}

void CompositingTile::clearBitmap()
{
    if (m_bitmap)
        delete m_bitmap;
    m_bitmap = nullptr;
}

SkBitmap* CompositingTile::allocBitmap(int width, int height, SkColor backgroundColor)
{
    if (0 == width || 0 == height)
        return nullptr;

    SkBitmap* bitmap = new SkBitmap();
    SkImageInfo info = SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType, kLinear_SkColorProfileType);
    if (!m_solidColor)
        bitmap->allocPixels(info);

    //backgroundColor = 0xff1811ff;
    bitmap->eraseColor(backgroundColor);
    return bitmap;
}

void CompositingTile::resizeBitmap(int dstWidth, int dstHeight, SkColor backgroundColor)
{
    SkIRect isrc;
    m_bitmap->getBounds(&isrc);
    if (isrc.width() == dstWidth && isrc.height() == dstHeight)
        return;

    SkBitmap* dst = allocBitmap(dstWidth, dstHeight, backgroundColor);

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x00ffffff);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    paint.setFilterQuality(kHigh_SkFilterQuality);

    SkCanvas canvas(*dst);
    if (!m_solidColor || blink::RuntimeEnabledFeatures::drawTileLineEnabled())
        canvas.drawBitmapRect(*m_bitmap, &isrc, SkRect::MakeIWH(isrc.width(), isrc.height()), &paint);

    clearBitmap();
    m_bitmap = dst;
}

void CompositingTile::allocBitmapIfNeeded(SkColor* solidColor, bool isSolidColorCoverWholeTile)
{
    //solidColor = nullptr; // todo weolar

    SkColor backgroundColor = m_compositingLayer->getBackgroundColor();
    if (m_solidColor)
        backgroundColor = *m_solidColor;

    if (m_solidColor && !solidColor) { // 纯色 -> bitmap
        clearBitmap();
        cancelSolidColor();
    } else if (!m_solidColor && solidColor) { // bitmap -> 纯色
        if (isSolidColorCoverWholeTile) {
            clearBitmap();
            setSolidColor(*solidColor);
        }
    } else if (!m_solidColor && !solidColor) { // 都不是纯色
        if (m_bitmap)
            ASSERT(m_bitmap->getPixels());
    } else if (m_solidColor && solidColor) { // 都是纯色
        cancelSolidColor();
        clearBitmap();
        if (isSolidColorCoverWholeTile) {
            setSolidColor(*solidColor);
            if (m_bitmap)
                ASSERT(!m_bitmap->getPixels());
        }
    }

    m_isNotInit = false;
    // 有可能在还没光栅化，就被滚动导致clearBitmap了，所以不需要ASSERT(!(!m_bitmap && 1 != getRefCnt())); 
    if (!m_compositingLayer)
        return;

    int width = m_postion.width();
    int height = m_postion.height();
    
    blink::IntSize bounds = m_compositingLayer->drawToCanvasProperties()->bounds;
    bool isBoundsDirty = m_layerBounds != bounds;
    m_layerBounds = bounds;
    if (0 == bounds.width() || 0 == bounds.height()) {
        clearBitmap();
        return;
    }

    int newWidth = bounds.width() < kDefaultTileWidth ? bounds.width() : kDefaultTileWidth;
    int newHeight = bounds.height() < kDefaultTileHeight ? bounds.height() : kDefaultTileHeight;
    newWidth = newWidth <= 0 ? 1 : newWidth;
    newHeight = newHeight <= 0 ? 1 : newHeight;

    bool needResize = newWidth != width || newHeight != height;
    m_postion = blink::IntRect(m_xIndex * kDefaultTileWidth, m_yIndex * kDefaultTileHeight, newWidth, newHeight);

    if (m_bitmap && needResize) {
        if (!m_solidColor)
            ASSERT(m_bitmap->getPixels());
        resizeBitmap(newWidth, newHeight, backgroundColor);
        return;
    } else if (m_bitmap && !needResize) {
        if (!m_solidColor)
            ASSERT(m_bitmap->getPixels());
        return;
    }

    clearBitmap();
    m_bitmap = allocBitmap(newWidth, newHeight, backgroundColor);
}

CompositingLayer* CompositingTile::layer() const
{
    return m_compositingLayer;
}

void CompositingTile::setSolidColor(SkColor color)
{
    m_solidColor = new SkColor();
    *m_solidColor = color;
}

void CompositingTile::cancelSolidColor()
{
    delete m_solidColor;
    m_solidColor = nullptr;
}

blink::IntRect CompositingTile::postion() const
{
    return m_postion; 
}

void CompositingTile::eraseColor(const blink::IntRect& r, const SkColor* color)
{
    SkCanvas canvas(*bitmap());

    blink::IntRect rect = r;
    rect.intersect(postion());
    rect.move(-postion().x(), -postion().y());

    SkPaint clearColorPaint;
    clearColorPaint.setXfermodeMode(SkXfermode::kClear_Mode);
    clearColorPaint.setColor(color ? *color : layer()->backgroundColor());
    canvas.drawIRect(rect, clearColorPaint);
}

} // cc