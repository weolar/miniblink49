
#include "cc/tiles/Tile.h"

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "cc/tiles/TileGrid.h"
#include "cc/tiles/TileWidthHeight.h"
#include "cc/blink/WebLayerImpl.h" // TODO for backgroundColor

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, tileCounter, ("ccTile"));
#endif

Tile::Tile()
    : TileBase()
{

}

TileBase* Tile::init(void* parent, int xIndex, int yIndex)
{
    m_isNotInit = true;
    m_refCnt = 1;
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_priority = TilePriorityNormal;
    m_postion = blink::IntRect(xIndex * kDefaultTileWidth, yIndex * kDefaultTileHeight, kDefaultTileWidth, kDefaultTileHeight);
    m_useingRate = 0;
    m_tileGrid = (TileGrid*)parent;
    m_tileGrid->registerTile(this);
    setAllBoundDirty();
#ifndef NDEBUG
    tileCounter.increment();
#endif
    return this;
}


Tile::~Tile()
{
    for (size_t i = 0; i < m_refFrom.size(); ++i) {
        TileTraceLocation* location = m_refFrom[i];
        delete location;
    }
    //m_refFrom.clear();

    //clearBitmap();
    m_tileGrid->unregisterTile(this);

#ifndef NDEBUG
    tileCounter.decrement();
#endif
}

void Tile::ref(const blink::WebTraceLocation& location)
{
    // m_refFrom要加锁
	  //m_refFrom.append(new TileTraceLocation(location.functionName(), location.fileName(), true));
	  ASSERT(m_refCnt > 0);
	  (void)sk_atomic_fetch_add(&m_refCnt, +1, sk_memory_order_relaxed);  // No barrier required.
}

void Tile::unref(const blink::WebTraceLocation& location)
{
	  //m_refFrom.append(new TileTraceLocation(location.functionName(), location.fileName(), false));
    ASSERT(m_refCnt > 0);
    if (1 == sk_atomic_fetch_add(&m_refCnt, -1, sk_memory_order_acq_rel)) {
        delete this;
    }
}

int32_t Tile::getRefCnt() const
{
	  return m_refCnt;
}

void Tile::clearBitmap()
{
//     if (m_bitmap)
//         delete m_bitmap;
//     m_bitmap = nullptr;
//     //m_bitmap->eraseColor(0xff000000); // debug test
	  m_bitmap = false;
}

void Tile::allocBitmapIfNeeded()
{
	  m_bitmap = true;
//     WTF::MutexLocker locker(m_mutex);
//     m_isNotInit = false;
//     //ASSERT(!(!m_bitmap && 1 != getRefCnt())); // 有可能在还没光栅化，就被滚动导致clearBitmap了
// 
//     if (m_bitmap)
//         return;
// 
//     m_bitmap = new SkBitmap();
//     m_bitmap->allocN32Pixels(m_postion.width(), m_postion.height(), false);
//     SkColor color = 0x00ffffff;
//     if (m_tileGrid && m_tileGrid->layer())
//         color = 0x00ffffff & (m_tileGrid->layer()->backgroundColor());
// 	// weolar
// 	//color = 0xfff010f0;
//     m_bitmap->eraseColor(color); // 根据是否透明窗口决定背景色
}

bool Tile::isSameTileGrid(const TileGrid* tileGrid) const
{
    return m_tileGrid == tileGrid;
}

void Tile::appendDirtyRect(const blink::IntRect& r) 
{
    WTF::MutexLocker locker(m_mutex);
    m_dirtyRect.unite(r);
}

void Tile::clearDirtyRect()
{
    WTF::MutexLocker locker(m_mutex);
    m_dirtyRect = blink::IntRect();
}

cc_blink::WebLayerImpl* Tile::layer() const
{
    if (!m_tileGrid)
        return nullptr;
    return m_tileGrid->layer();
}

TileGrid* Tile::tileGrid() const
{
    return m_tileGrid;
}

blink::IntRect Tile::postion() const
{
    return m_postion; 
}

blink::IntRect Tile::dirtyRect() 
{
    WTF::MutexLocker locker(m_mutex);
    return m_dirtyRect; 
}

void Tile::setAllBoundDirty() 
{
    WTF::MutexLocker locker(m_mutex);
    m_dirtyRect = blink::IntRect(0, 0, m_postion.width(), m_postion.height()); 
}

TilePriority Tile::priority() const
{
    return m_priority; 
}

void Tile::setPriority(TilePriority priority) 
{
    m_priority = priority; 
}

void Tile::eraseColor(const blink::IntRect& r, const SkColor* color)
{
    //tile->bitmap()->eraseColor(0x00ffffff);

//     SkCanvas canvas(*bitmap());
// 
//     blink::IntRect rect = r;
//     rect.intersect(postion());
//     rect.move(-postion().x(), -postion().y());
// 
//     SkPaint clearColorPaint;
//     clearColorPaint.setXfermodeMode(SkXfermode::kClear_Mode);
//     //clearColorPaint.setColor(color ? *color : (0x00ffffff | (layer()->backgroundColor()))); // weolar
// 	   clearColorPaint.setColor(0xff10f0f0);
//     canvas.drawIRect(rect, clearColorPaint);
}

void Tile::increaseUseingRate()
{
    ++m_useingRate;
}

} // cc