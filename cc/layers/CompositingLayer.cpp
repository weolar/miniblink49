
#include "cc/layers/CompositingLayer.h"

#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO
#include "platform/graphics/GraphicsContext.h" // TODO
#include "platform/graphics/BitmapImage.h" // TODO
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/core/SkBitmap.h"

#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/DrawProperties.h"
#include "cc/raster/SkBitmapRefWrap.h"
#include "cc/tiles/CompositingTile.h"
#include "cc/tiles/TileWidthHeight.h"
#include "cc/playback/TileActionInfo.h"

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
}

namespace cc_blink {
    extern int debugMaskLayerId;
}

static void transformToFlattenedSkMatrix(const SkMatrix44& transform, SkMatrix* flattened)
{
    // Convert from 4x4 to 3x3 by dropping the third row and column.
    flattened->set(0, SkMScalarToScalar(transform.get(0, 0)));
    flattened->set(1, SkMScalarToScalar(transform.get(0, 1)));
    flattened->set(2, SkMScalarToScalar(transform.get(0, 3)));
    flattened->set(3, SkMScalarToScalar(transform.get(1, 0)));
    flattened->set(4, SkMScalarToScalar(transform.get(1, 1)));
    flattened->set(5, SkMScalarToScalar(transform.get(1, 3)));
    flattened->set(6, SkMScalarToScalar(transform.get(3, 0)));
    flattened->set(7, SkMScalarToScalar(transform.get(3, 1)));
    flattened->set(8, SkMScalarToScalar(transform.get(3, 3)));
}

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, compositingLayerCounter, ("ccCompositingLayer"));
#endif
   
CompositingLayer::CompositingLayer(int id)
{
    m_prop = new DrawToCanvasProperties();
    m_tiles = new Vector<CompositingTile*>();
	m_numTileX = 0;
	m_numTileY = 0;
	m_parent = nullptr;
	m_id = id;

// 	String outString = String::format("CompositingLayer::CompositingLayer:%p %d \n", this, m_id);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

#ifndef NDEBUG
	compositingLayerCounter.increment();
#endif
}

CompositingLayer::~CompositingLayer()
{
    delete m_prop;
    for (size_t i = 0; i < m_tiles->size(); ++i) {
        CompositingTile* tile = m_tiles->at(i);
        tile->unref();
    }
    delete m_tiles;
	ASSERT(!m_parent);

// 	String outString = String::format("CompositingLayer::~~~~~~~~CompositingLayer:%p %d \n", this, m_id);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
#ifndef NDEBUG
	compositingLayerCounter.decrement();
#endif
}

int CompositingLayer::id() const
{
    return m_id;
}

bool CompositingLayer::masksToBounds() const 
{
    return m_prop->masksToBounds;
}

bool CompositingLayer::drawsContent() const 
{
    return m_prop->drawsContent;
}

float CompositingLayer::opacity() const
{
    return m_prop->opacity;
}

bool CompositingLayer::opaque() const
{
    return m_prop->opaque;
}

void CompositingLayer::insertChild(CompositingLayer* child, size_t index)
{
    CompositingLayer* childOfImpl = static_cast<CompositingLayer*>(child);
    childOfImpl->removeFromParent();
    childOfImpl->setParent(this);

    index = std::min(index, m_children.size());
    m_children.insert(index, childOfImpl);   
}

void CompositingLayer::setParent(CompositingLayer* parent)
{
    m_parent = parent;
}

void CompositingLayer::removeAllChildren()
{
    while (m_children.size()) {
        CompositingLayer* layer = m_children[0];
        ASSERT(this == layer->parent());
        layer->removeFromParent();
    }
}

void CompositingLayer::replaceChild(CompositingLayer* reference, CompositingLayer* newLayer)
{
    ASSERT(reference);
    ASSERT(reference->parent());

    if (reference == newLayer)
        return;

    int referenceIndex = indexOfChild(reference);
    if (referenceIndex == -1) {
        RELEASE_ASSERT(false);
        return;
    }

    reference->removeFromParent();

    if (newLayer) {
        newLayer->removeFromParent();
        insertChild(newLayer, referenceIndex);
    }
}

int CompositingLayer::indexOfChild(CompositingLayer* child) const
{
    for (size_t i = 0; i < m_children.size(); ++i) {
        if (m_children[i] == child)
            return i;
    }
    return -1;
}

void CompositingLayer::removeFromParent()
{
    if (m_parent)
        m_parent->removeChildOrDependent(this);
}

void CompositingLayer::removeChildOrDependent(CompositingLayer* child)
{
    for (size_t iter = 0; iter != m_children.size(); ++iter) {
        if (m_children[iter] != child)
            continue;

        child->setParent(NULL);
        m_children.remove(iter);
        return;
    }
}

void CompositingLayer::updataDrawProp(DrawToCanvasProperties* prop)
{
    m_prop->copy(*prop);
}

size_t CompositingLayer::tilesSize() const
{
    if (!m_tiles)
        return 0;
    return m_tiles->size();
}

CompositingTile* CompositingLayer::getTileByXY(int xIndex, int yIndex)
{
    if (m_numTileX <= xIndex || m_numTileY <= yIndex)
        return nullptr;
    return *(m_tiles->data() + m_numTileX * yIndex + xIndex);
}

void CompositingLayer::updataTile(int newIndexNumX, int newIndexNumY)
{
    Vector<CompositingTile*>* newTiles = new Vector<CompositingTile*>;
    newTiles->resize(newIndexNumX * newIndexNumY);
    int index = 0;
    for (int y = 0; y < newIndexNumY; ++y) {
        for (int x = 0; x < newIndexNumX; ++x) {
            CompositingTile* tile = getTileByXY(x, y);
            if (!tile)
                tile = new CompositingTile(this, x, y);
            else {
                ASSERT(x == tile->xIndex() && y == tile->yIndex());
                tile->ref();
            }

            newTiles->at(index) = tile;
            ++index;
        }
    }

    for (size_t i = 0; i < m_tiles->size(); ++i) {
        CompositingTile* tile = m_tiles->at(i);
        tile->unref();
    }

    delete m_tiles;
	m_tiles = newTiles;
	m_numTileX = newIndexNumX;
	m_numTileY = newIndexNumY;

// 	String outString = String::format("CompositingLayer::updataTile: %d \n", m_id);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void CompositingLayer::cleanupUnnecessaryTile(const WTF::Vector<TileActionInfo*>& tiles)
{
    for (size_t i = 0; i < tiles.size(); ++i) {
        TileActionInfo* info = tiles[i];
        CompositingTile* tile = getTileByXY(info->xIndex, info->yIndex);
        ASSERT(tile == m_tiles->at(info->index));
        
        tile->clearBitmap();
    }
}

void CompositingLayer::blendToTiles(TileActionInfoVector* willRasteredTiles, const SkBitmap& bitmap, const blink::IntRect& dirtyRect)
{
    const Vector<TileActionInfo*>& infos = willRasteredTiles->infos();
    for (size_t i = 0; i < infos.size(); ++i) {
        TileActionInfo* info = infos[i];
        CompositingTile* tile = getTileByXY(info->xIndex, info->yIndex);
        ASSERT(tile == m_tiles->at(info->index));

        blendToTile(tile, bitmap, dirtyRect);
    }
}

void CompositingLayer::blendToTile(CompositingTile* tile, const SkBitmap& bitmap, blink::IntRect dirtyRect)
{
    tile->allocBitmapIfNeeded();
    if (!tile->bitmap())
        return;

    blink::IntRect dirtyRectInTile = dirtyRect;
    dirtyRectInTile.move(-tile->postion().x(), -tile->postion().y());
    dirtyRectInTile.intersect(blink::IntRect(0, 0, tile->postion().width(), tile->postion().height()));
    tile->eraseColor(dirtyRectInTile, nullptr);

#if 0 // debug
    String outString = String::format("RasterTask::blendToTile:%d %d, %d %d %d %d\n",
        tile->xIndex(), tile->yIndex(), dirtyRectInTile.x(), dirtyRectInTile.y(), dirtyRectInTile.width(), dirtyRectInTile.height());
    OutputDebugStringW(outString.charactersWithNullTermination().data());
#endif

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0xFFFFFFFF);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    paint.setFilterQuality(kHigh_SkFilterQuality);

    blink::IntRect postion = tile->postion();
    if (!postion.intersects(dirtyRect)) {
//         if (postion.width() == 1 && postion.height() == 1 && dirtyRect.width() == 1 && dirtyRect.height() == 1)
//             return;
        DebugBreak();
        return;
    }

    SkIRect dst = (dirtyRect);
    dst = dst.makeOffset(-tile->postion().x(), -tile->postion().y());

    SkCanvas canvas(*tile->bitmap());
    canvas.drawBitmapRect(bitmap, nullptr, SkRect::MakeFromIRect(dst), &paint);

#if 0 // debug
    static bool s_startDump = false;
    if (s_startDump) {
        s_startDump = true;

        Vector<unsigned char> output;
        blink::GDIPlusImageEncoder::encode(bitmap, blink::GDIPlusImageEncoder::PNG, &output);
        blink::saveDumpFile("", (char*)output.data(), output.size());
    }
#endif

#if 0 // debug
    SkPaint paintTest;
    const SkColor color = 0xff000000 | (rand() % 3) * (rand() % 7) * GetTickCount();
	//const SkColor color = 0x11FFFFFF;
    paintTest.setColor(color);
    paintTest.setStrokeWidth(4);
    paintTest.setTextSize(13);
    paintTest.setTextEncoding(SkPaint::kUTF8_TextEncoding);

    static SkTypeface* typeface = nullptr;
    if (!typeface)
        typeface = SkTypeface::RefDefault(SkTypeface::kNormal);
    paintTest.setTypeface(typeface);

    paintTest.setStrokeWidth(1);
    canvas.drawLine(0, 0, tile->postion().width(), tile->postion().height(), paintTest);
    canvas.drawLine(tile->postion().width(), 0, 0, tile->postion().height(), paintTest);

    String textTest = String::format("id:%d, %d %d", m_id, tile->xIndex(), tile->yIndex());
    CString cText = textTest.utf8();
    canvas.drawText(cText.data(), cText.length(), 5, 15, paintTest);
#endif
}

class DoClipLayer {
public:
    DoClipLayer(LayerTreeHost* host, CompositingLayer* layer, blink::WebCanvas* canvas, SkRect clip)
    {
        m_canvas = canvas;

        SkRect skClipRect = clip;
        if (layer->masksToBounds()) {
            bool isIntersect = skClipRect.intersect(SkRect::MakeIWH(layer->drawToCanvasProperties()->bounds.width(), layer->drawToCanvasProperties()->bounds.height()));
            if (!isIntersect)
                skClipRect.setEmpty();
        }

        m_maskLayer = nullptr;
        if (-1 != layer->m_prop->maskLayerId) {
            m_maskLayer = host->getCCLayerById(layer->m_prop->maskLayerId);
            if (m_maskLayer) {
                SkRect skMaskClipRect = SkRect::MakeXYWH(
                    m_maskLayer->m_prop->position.x(), 
                    m_maskLayer->m_prop->position.y(),
                    m_maskLayer->m_prop->bounds.width(), 
                    m_maskLayer->m_prop->bounds.height());
                if (!skClipRect.intersect(skMaskClipRect))
                    skClipRect.setEmpty();
            }
        }

        canvas->clipRect(skClipRect);
    }

private:
    CompositingLayer* m_maskLayer;
    blink::WebCanvas* m_canvas;
};

class DoClipChileLayer {
public:
    DoClipChileLayer(CompositingLayer* child, blink::WebCanvas* canvas)
    {
        m_canvas = canvas;
        m_child = child;
        m_clipChild = false;

        if (1 != child->tilesSize() && 0 != child->tilesSize()) {
            m_clipChild = true;
            canvas->save();
            canvas->clipRect(SkRect::MakeIWH(child->drawToCanvasProperties()->bounds.width(), child->drawToCanvasProperties()->bounds.height())); // weolar
        }
    }

    void release()
    {
        if (m_clipChild)
            m_canvas->restore();
    }

private:
    CompositingLayer* m_child;
    blink::WebCanvas* m_canvas;
    bool m_clipChild;
};
 
void CompositingLayer::drawToCanvasChildren(LayerTreeHost* host, SkCanvas* canvas, const blink::IntRect& clip, int deep)
{
    for (size_t i = 0; i < children().size(); ++i) {
        CompositingLayer* child = children()[i];

        const SkMatrix44& currentTransform = child->drawToCanvasProperties()->currentTransform;
        const SkMatrix44& transformToAncestor = child->drawToCanvasProperties()->screenSpaceTransform;

        SkMatrix matrixToAncestor;
        transformToFlattenedSkMatrix(transformToAncestor, &matrixToAncestor);

        SkMatrix matrixToCurrent;
        transformToFlattenedSkMatrix(currentTransform, &matrixToCurrent);

        canvas->save();
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        paint.setFilterQuality(kHigh_SkFilterQuality);

        canvas->setMatrix(matrixToAncestor);

        SkRect clipInLayerdCoordinate = SkRect::MakeXYWH(clip.x(), clip.y(), clip.width(), clip.height());
        SkMatrix44 transformToAncestorInverse;
        transformToAncestor.invert(&transformToAncestorInverse);
        ((SkMatrix)transformToAncestorInverse).mapRect(&clipInLayerdCoordinate);

        blink::IntRect clipInLayerdCoordinateInt(SkScalarTruncToInt(clipInLayerdCoordinate.x()), SkScalarTruncToInt(clipInLayerdCoordinate.y()),
            SkScalarTruncToInt(clipInLayerdCoordinate.width()), SkScalarTruncToInt(clipInLayerdCoordinate.height()));

        DoClipLayer doClipLayer(host, child, canvas, clipInLayerdCoordinate);

        DoClipChileLayer doClipChileLayer(child, canvas);
        child->drawToCanvas(host, canvas, clipInLayerdCoordinateInt);

        doClipChileLayer.release();

        if (!child->opaque() || !child->masksToBounds() || !child->drawsContent())
            child->drawToCanvasChildren(host, canvas, clip, deep + 1);

        canvas->resetMatrix();
        canvas->restore();
    }
}

void CompositingLayer::drawToCanvas(LayerTreeHost* host, blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    int alpha = (int)(255 * opacity());
    if (!drawsContent())
        return;

    for (size_t i = 0; i < m_tiles->size(); ++i) {
        CompositingTile* tile = m_tiles->at(i);
        if (!tile->postion().intersects(clip) || !tile->bitmap())
            continue;

        blink::IntRect tilePostion = tile->postion();
        SkIRect dst = (SkIRect)(tilePostion);
        SkIRect src = SkIRect::MakeWH(tile->bitmap()->width(), tile->bitmap()->height());

        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setAlpha(alpha);
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        //paint.setColor(0xffffffff);
        paint.setFilterQuality(kHigh_SkFilterQuality);

#if 0 // debug
        OwnPtr<blink::GraphicsContext> context = blink::GraphicsContext::deprecatedCreateWithCanvas(canvas, blink::GraphicsContext::NothingDisabled);
        context->setStrokeStyle(blink::SolidStroke);
        context->setStrokeThickness(1);
        context->setStrokeColor(0xff000000 | (::GetTickCount() + rand()));
        //context->drawLine(blink::IntPoint(tilePostion.x(), tilePostion.y()), blink::IntPoint(tilePostion.maxX(), tilePostion.maxY()));
        //context->drawLine(blink::IntPoint(tilePostion.maxX(), tilePostion.y()), blink::IntPoint(tilePostion.x(), tilePostion.maxY()));
        context->strokeRect(tilePostion, 1);
        context->fillRect(tilePostion, 0x00000000 | (::GetTickCount() + rand()));
#endif
        canvas->drawBitmapRect(*tile->bitmap(), nullptr, SkRect::MakeFromIRect(dst), &paint); // weolar
    }
}

//////////////////////////////////////////////////////////////////////////

CompositingImageLayer::CompositingImageLayer(int id)
	: CompositingLayer(id)
	, m_bitmap(nullptr)
{

}

CompositingImageLayer::~CompositingImageLayer()
{
    if (m_bitmap)
	    m_bitmap->deref();
}

void CompositingImageLayer::drawToCanvas(LayerTreeHost* host, blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (!drawsContent() || !m_bitmap || !m_bitmap->get())
        return;

    SkRect dst = SkRect::MakeWH(m_prop->bounds.width(), m_prop->bounds.height());
    canvas->drawBitmapRect(*m_bitmap->get(), dst, nullptr);
}

void CompositingImageLayer::setImage(SkBitmapRefWrap* bitmap) 
{
    if (m_bitmap)
        m_bitmap->deref();
    m_bitmap = bitmap; 
    m_bitmap->ref();
}

}