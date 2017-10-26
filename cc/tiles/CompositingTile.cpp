
#include "cc/tiles/CompositingTile.h"

#include "cc/tiles/TileWidthHeight.h"
#include "cc/layers/CompositingLayer.h"
#include "cc/trees/DrawProperties.h"
#include "cc/base/bdcolor.h"

#include "SkBitmap.h"
#include "SkCanvas.h"

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

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
#ifndef NDEBUG
    compositingTileCounter.increment();
#endif
    return this;
}

CompositingTile::~CompositingTile()
{
    clearBitmap();
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

SkBitmap* CompositingTile::allocBitmap(int width, int height, bool isOpaque)
{
    if (0 == width || 0 == height)
        return nullptr;

    SkBitmap* bitmap = new SkBitmap();
    SkImageInfo info = SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType, kLinear_SkColorProfileType);
    bitmap->allocPixels(info);

    SkColor color = 0x00ffffff;
//     if (!isOpaque) // TODO 
//          bitmap->eraseColor(color); // TODO: 根据是否透明窗口决定背景色
    return bitmap;
}

void CompositingTile::resizeBitmap(int dstWidth, int dstHeight)
{
    SkIRect isrc;
    m_bitmap->getBounds(&isrc);
    if (isrc.width() == dstWidth && isrc.height() == dstHeight)
        return;

    SkBitmap* dst = allocBitmap(dstWidth, dstHeight, m_compositingLayer->opaque());

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0x00ffffff);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    paint.setFilterQuality(kHigh_SkFilterQuality);

    SkCanvas canvas(*dst);
    canvas.drawBitmapRect(*m_bitmap, &isrc, SkRect::MakeIWH(isrc.width(), isrc.height()), &paint);

    clearBitmap();
    m_bitmap = dst;
}

void CompositingTile::allocBitmapIfNeeded()
{
    m_isNotInit = false;
    // 有可能在还没光栅化，就被滚动导致clearBitmap了，所以不需要ASSERT(!(!m_bitmap && 1 != getRefCnt())); 
    if (!m_compositingLayer)
        return;

    int width = m_postion.width();
    int height = m_postion.height();
    
    //m_postion = blink::IntRect(m_xIndex * kDefaultTileWidth, m_yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
    blink::IntSize bounds = m_compositingLayer->drawToCanvasProperties()->bounds;
    bool isBoundsDirty = m_layerBounds != bounds;
    m_layerBounds = bounds;
    if (0 == bounds.width() || 0 == bounds.height()) {
        clearBitmap();
        return;
    }

    
    //         if (kDefaultTileWidth >= bounds.width() && kDefaultTileHeight >= bounds.height()) {
    //             if (1 != m_compositingLayer->tilesSize()) {
    //                 WTF::String outstr = WTF::String::format("CompositingTile::allocBitmapIfNeeded %p %d %d\n", this, m_compositingLayer->id(), m_compositingLayer->tilesSize());
    //                 OutputDebugStringW(outstr.charactersWithNullTermination().data());
    // 
    //                 DebugBreak();
    //                 return;
    //             } else {
    //                 needResize = (width != bounds.width() || height != bounds.height());
    //                 width = bounds.width();
    //                 height = bounds.height();
    //                 m_postion = blink::IntRect(0, 0, width, height);
    //             }
    //         }
    
    int newWidth = bounds.width() < kDefaultTileWidth ? bounds.width() : kDefaultTileWidth;
    int newHeight = bounds.height() < kDefaultTileHeight ? bounds.height() : kDefaultTileHeight;

    bool needResize = newWidth != width || newHeight != height;
    m_postion = blink::IntRect(m_xIndex * kDefaultTileWidth, m_yIndex * kDefaultTileHeight, newWidth, newHeight);

    if (m_bitmap && needResize) {
        resizeBitmap(newWidth, newHeight);
        return;
    } else if (m_bitmap && !needResize) {
        return;
    }

    clearBitmap();
    m_bitmap = allocBitmap(newWidth, newHeight, m_compositingLayer->opaque());
}

// void CompositingTile::allocBitmapIfNeeded()
// {
//     m_isNotInit = false;
//     // 有可能在还没光栅化，就被滚动导致clearBitmap了，所以不需要ASSERT(!(!m_bitmap && 1 != getRefCnt())); 
// 
//     if (m_bitmap)
//         return;
// 
//     int width = m_postion.width();
//     int height = m_postion.height();
//     m_postion = blink::IntRect(m_xIndex * kDefaultTileWidth, m_yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
//     if (m_compositingLayer) {
//         blink::IntSize bounds = m_compositingLayer->drawToCanvasProperties()->bounds;
//         if (width >= bounds.width() && height >= bounds.height()) {
//             if (1 != m_compositingLayer->tilesSize()) {
//                 ASSERT(false);
//                 return;
//             } else {
//                 width = bounds.width();
//                 height = bounds.height();
//                 m_postion = blink::IntRect(0, 0, width, height);
//             }
//         }
//     }
//     
//     m_bitmap = new SkBitmap();
//     SkImageInfo info = SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType, kLinear_SkColorProfileType);
//     m_bitmap->allocPixels(info);
// 
//     SkColor color = 0x00ffffff;
//     m_bitmap->eraseColor(color); // 根据是否透明窗口决定背景色
// }

CompositingLayer* CompositingTile::layer() const
{
    return m_compositingLayer;
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