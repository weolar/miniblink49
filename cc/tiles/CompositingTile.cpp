
#include "cc/tiles/CompositingTile.h"

#include "SkBitmap.h"
#include "SkCanvas.h"

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, compositingTileCounter, ("compositingTileCounter"));
#endif

CompositingTile::CompositingTile(CompositingLayer* compositingLayer, int xIndex, int yIndex)
{
    m_compositingLayer = compositingLayer;
    m_isNotInit = true;
	m_refCnt = 1;
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_postion = blink::IntRect(xIndex * kDefaultTileWidth, yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
    m_bitmap = nullptr;

#ifndef NDEBUG
    compositingTileCounter.increment();
#endif
}

CompositingTile::~CompositingTile()
{
    clearBitmap();
    //m_tileGrid->unregisterTile(this);

#ifndef NDEBUG
    compositingTileCounter.decrement();
#endif
}

void CompositingTile::ref()
{
	ASSERT(m_refCnt > 0);
	(void)sk_atomic_fetch_add(&m_refCnt, +1, sk_memory_order_relaxed);  // No barrier required.
}

void CompositingTile::unref()
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

void CompositingTile::allocBitmapIfNeeded()
{
    m_isNotInit = false;
    //ASSERT(!(!m_bitmap && 1 != getRefCnt())); // 有可能在还没光栅化，就被滚动导致clearBitmap了

    if (m_bitmap)
        return;
	
     m_bitmap = new SkBitmap();
//     m_bitmap->allocN32Pixels(m_postion.width(), m_postion.height(), false);
	 SkImageInfo info = SkImageInfo::Make(m_postion.width(), m_postion.height(), kN32_SkColorType, kPremul_SkAlphaType, kLinear_SkColorProfileType);
	 m_bitmap->allocPixels(info);

    SkColor color = 0x00ffffff;
	//color = 0xfff010f0;
    m_bitmap->eraseColor(color); // 根据是否透明窗口决定背景色
}

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
    //clearColorPaint.setColor(color ? *color : (0x00ffffff | (layer()->backgroundColor()))); // weolar
    clearColorPaint.setColor(0x00ffffff);
    canvas.drawIRect(rect, clearColorPaint);
}

} // cc