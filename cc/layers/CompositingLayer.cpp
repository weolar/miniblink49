
#include "cc/layers/CompositingLayer.h"

#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkCanvas.h"

#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO
#include "platform/graphics/GraphicsContext.h" // TODO
#include "platform/graphics/BitmapImage.h" // TODO
#include "platform/RuntimeEnabledFeatures.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/core/SkBitmap.h"

#include "cc/trees/LayerTreeHost.h"
#include "cc/trees/DrawProperties.h"
#include "cc/raster/SkBitmapRefWrap.h"
#include "cc/tiles/CompositingTile.h"
#include "cc/tiles/TileWidthHeight.h"
#include "cc/tiles/TilesAddr.h"
#include "cc/playback/TileActionInfo.h"

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
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
    //m_tiles = new Vector<CompositingTile*>();
    m_tilesAddr = new TilesAddr(this);
    m_numTileX = 0;
    m_numTileY = 0;
    m_parent = nullptr;
    m_id = id;

#ifndef NDEBUG
    compositingLayerCounter.increment();
#endif
}

CompositingLayer::~CompositingLayer()
{
    delete m_prop;
    delete m_tilesAddr;

    ASSERT(!m_parent);

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

const blink::IntSize& CompositingLayer::bounds() const
{
    return m_prop->bounds;
}

float CompositingLayer::opacity() const
{
    return m_prop->opacity;
}

bool CompositingLayer::opaque() const
{
    return m_prop->opaque;
}

SkColor CompositingLayer::backgroundColor() const
{
    return m_prop->backgroundColor;
}

bool CompositingLayer::isDoubleSided() const
{
    return m_prop->isDoubleSided;
}

bool CompositingLayer::useParentBackfaceVisibility() const
{
    return m_prop->useParentBackfaceVisibility;
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
    return m_tilesAddr->getSize();
}

SkColor CompositingLayer::getBackgroundColor() const
{
    return m_prop->backgroundColor;
}

void CompositingLayer::updataTile(int newIndexNumX, int newIndexNumY, DrawToCanvasProperties* prop)
{
    TilesAddr::realloByNewXY(&m_tilesAddr, newIndexNumX, newIndexNumY);

    m_numTileX = newIndexNumX;
    m_numTileY = newIndexNumY;

    updataDrawProp(prop);
}

void CompositingLayer::cleanupUnnecessaryTile(const WTF::Vector<TileActionInfo*>& tiles)
{
    for (size_t i = 0; i < tiles.size(); ++i) {
        TileActionInfo* info = tiles[i];
        CompositingTile* tile = (CompositingTile*)m_tilesAddr->getTileByXY(info->xIndex, info->yIndex, [] { return new CompositingTile(); });
        ASSERT(tile == m_tilesAddr->getTileByIndex(info->index));        
        tile->clearBitmap();
        m_tilesAddr->remove(tile);
    }
}

static bool isBackFaceVisible(const SkMatrix44& matrix)
{
    // Compute whether a layer with a forward-facing normal of (0, 0, 1, 0)
    // would have its back face visible after applying the transform.
    if (matrix.isIdentity())
        return false;

    // This is done by transforming the normal and seeing if the resulting z
    // value is positive or negative. However, note that transforming a normal
    // actually requires using the inverse-transpose of the original transform.
    //
    // We can avoid inverting and transposing the matrix since we know we want
    // to transform only the specific normal vector (0, 0, 1, 0). In this case,
    // we only need the 3rd row, 3rd column of the inverse-transpose. We can
    // calculate only the 3rd row 3rd column element of the inverse, skipping
    // everything else.
    //
    // For more information, refer to:
    //   http://en.wikipedia.org/wiki/Invertible_matrix#Analytic_solution
    //

    double determinant = matrix.determinant();

    // If matrix was not invertible, then just assume back face is not visible.
    if (determinant == 0)
        return false;

    // Compute the cofactor of the 3rd row, 3rd column.
    double cofactorPart1 = matrix.get(0, 0) * matrix.get(1, 1) * matrix.get(3, 3);
    double cofactorPart2 = matrix.get(0, 1) * matrix.get(1, 3) * matrix.get(3, 0);
    double cofactorPart3 = matrix.get(0, 3) * matrix.get(1, 0) * matrix.get(3, 1);
    double cofactorPart4 = matrix.get(0, 0) * matrix.get(1, 3) * matrix.get(3, 1);
    double cofactorPart5 = matrix.get(0, 1) * matrix.get(1, 0) * matrix.get(3, 3);
    double cofactorPart6 = matrix.get(0, 3) * matrix.get(1, 1) * matrix.get(3, 0);

    double cofactor33 = cofactorPart1 + cofactorPart2 + cofactorPart3 - cofactorPart4 - cofactorPart5 - cofactorPart6;

    const SkMScalar kEpsilon = std::numeric_limits<float>::epsilon();

    // Technically the transformed z component is cofactor33 / determinant.  But
    // we can avoid the costly division because we only care about the resulting
    // +/- sign; we can check this equivalently by multiplication.
    return cofactor33 * determinant < -kEpsilon;
}

static bool isLayerBackFaceVisible(CompositingLayer* layer)
{
    // A layer with singular transform is not drawn. So, we can assume that its backface is not visible.
//     if (HasSingularTransform(layer, tree))
//         return false;
    // The current W3C spec on CSS transforms says that backface visibility should
    // be determined differently depending on whether the layer is in a "3d
    // rendering context" or not. For Chromium code, we can determine whether we
    // are in a 3d rendering context by checking if the parent preserves 3d.

//     if (LayerIsInExisting3DRenderingContext(layer))
//         return DrawTransformFromPropertyTrees(layer, tree).IsBackFaceVisible();

    // In this case, either the layer establishes a new 3d rendering context, or
    // is not in a 3d rendering context at all.
    return isBackFaceVisible(layer->drawToCanvasProperties()->currentTransform);
}

static bool layerShouldBeSkipped(CompositingLayer* layer, bool layerIsDrawn)
{
    // Layers can be skipped if any of these conditions are met.
    //   - is not drawn due to it or one of its ancestors being hidden (or having
    //     no copy requests).
    //   - does not draw content.
    //   - is transparent.
    //   - has empty bounds
    //   - the layer is not double-sided, but its back face is visible.
    //
    // Some additional conditions need to be computed at a later point after the
    // recursion is finished.
    //   - the intersection of render_surface content and layer clip_rect is empty
    //   - the visible_layer_rect is empty
    //
    // Note, if the layer should not have been drawn due to being fully
    // transparent, we would have skipped the entire subtree and never made it
    // into this function, so it is safe to omit this check here.

    if (!layerIsDrawn)
        return true;

    if (!layer->drawsContent() || layer->bounds().isEmpty())
        return true;

    CompositingLayer* backfaceTestLayer = layer;
    if (layer->useParentBackfaceVisibility()) {
        ASSERT(layer->parent());
        ASSERT(!layer->parent()->useParentBackfaceVisibility());
        if (layer)
            backfaceTestLayer = layer->parent();
    }

    // The layer should not be drawn if (1) it is not double-sided and (2) the back of the layer is known to be facing the screen.
    if (!backfaceTestLayer->isDoubleSided() && isLayerBackFaceVisible(backfaceTestLayer))
        return true;

    return false;
}

void CompositingLayer::drawDebugLine(SkCanvas& canvas, CompositingTile* tile)
{
    if (!blink::RuntimeEnabledFeatures::drawTileLineEnabled() || tile->isSolidColor())
        return;

    SkPaint paintTest;
    const SkColor color = 0xff000000 | (rand() % 3) * (rand() % 7) * GetTickCount();
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

    String textTest = String::format("%d %d %d, (%d %d), (%d %d)", m_id, tile->isSolidColor(), m_children.size(), tile->xIndex(), tile->yIndex(), m_prop->bounds.width(), m_prop->bounds.height());
    CString cText = textTest.utf8();
    canvas.drawText(cText.data(), cText.length(), 5, 15, paintTest);
}

void CompositingLayer::blendToTiles(TileActionInfoVector* willRasteredTiles, const SkBitmap* bitmap, const SkRect& dirtyRect, float contentScale)
{
    const Vector<TileActionInfo*>& infos = willRasteredTiles->infos();
    for (size_t i = 0; i < infos.size(); ++i) {
        TileActionInfo* info = infos[i];
        CompositingTile* tile = (CompositingTile*)m_tilesAddr->getTileByXY(info->xIndex, info->yIndex, [] { return new CompositingTile(); });
        ASSERT(tile == m_tilesAddr->getTileByIndex(info->index));
        blendToTile(tile, bitmap ? bitmap : info->m_bitmap, dirtyRect, info->m_solidColor, info->m_isSolidColorCoverWholeTile, contentScale);
    } 
}

void CompositingLayer::blendToTile(CompositingTile* tile, const SkBitmap* bitmap, const SkRect& dirtyRect, SkColor* solidColor, bool isSolidColorCoverWholeTile, float contentScale)
{
    tile->allocBitmapIfNeeded(solidColor, isSolidColorCoverWholeTile);
    if (!tile->bitmap())
        return;

    if (!solidColor && !tile->bitmap()->getPixels())
        DebugBreak();

    blink::IntRect dirtyRectInTile = (blink::IntRect)dirtyRect;
    dirtyRectInTile.move(-tile->postion().x(), -tile->postion().y());
    dirtyRectInTile.intersect(blink::IntRect(0, 0, tile->postion().width(), tile->postion().height()));
    tile->eraseColor(dirtyRectInTile, nullptr);

#if 0 // debug
    String outString = String::format("RasterTask::blendToTile:%d %d, %d %d %d %d\n",
        tile->xIndex(), tile->yIndex(), dirtyRectInTile.x(), dirtyRectInTile.y(), dirtyRectInTile.width(), dirtyRectInTile.height());
    OutputDebugStringW(outString.charactersWithNullTermination().data());
#endif

    SkPaint paint;
    paint.setAntiAlias(false);
    paint.setColor(0xFFFFFFFF);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    paint.setFilterQuality(kLow_SkFilterQuality);

    blink::IntRect postion = tile->postion();
    if (!postion.intersects((blink::IntRect)dirtyRect)) {
        postion.setWidth(kDefaultTileWidth);
        postion.setHeight(kDefaultTileHeight);
        if (!postion.intersects((blink::IntRect)dirtyRect))
            DebugBreak();
        return;
    }

    SkIRect dstI = (blink::IntRect)(dirtyRect);
    dstI = dstI.makeOffset(-tile->postion().x(), -tile->postion().y());

    //SkIRect srcRect = SkIRect::MakeXYWH(dstI.left() * contentScale, dstI.top() * contentScale, dstI.width() * contentScale, dstI.height() * contentScale);
    blink::IntRect srcRectI = (blink::IntRect)dirtyRect;
    srcRectI.intersect(tile->postion());
    srcRectI.move(/*tile->postion().x()*/ - dirtyRect.x(), /*tile->postion().y()*/ - dirtyRect.y());
    SkIRect srcRect = SkIRect::MakeXYWH(srcRectI.x() * contentScale, srcRectI.y() * contentScale, srcRectI.width() * contentScale, srcRectI.height() * contentScale);

    if (!tile->bitmap()->getPixels()) {
        ASSERT(solidColor);
        return;
    }

    SkCanvas canvas(*tile->bitmap());
    if (bitmap) {
        //canvas.scale(1 / contentScale, 1 / contentScale);
        canvas.drawBitmapRect(*bitmap, &srcRect, SkRect::MakeFromIRect(dirtyRectInTile), &paint);
        //canvas.drawBitmapRect(*bitmap, nullptr, dst, &paint);
    }
    drawDebugLine(canvas, tile);
}

class DoClipLayer {
public:
    DoClipLayer(LayerTreeHost* host, CompositingLayer* layer, blink::WebCanvas* canvas, const SkRect& clip)
    {
        m_canvas = canvas;

        bool needClip = false;
        //SkRect skClipRect = clip;
        SkRect skClipRect = SkRect::MakeIWH(layer->drawToCanvasProperties()->bounds.width(), layer->drawToCanvasProperties()->bounds.height());
        if (layer->masksToBounds()) {
//             bool isIntersect = skClipRect.intersect(SkRect::MakeIWH(layer->drawToCanvasProperties()->bounds.width(), layer->drawToCanvasProperties()->bounds.height()));
//             if (!isIntersect)
//                 skClipRect.setEmpty();
            skClipRect = SkRect::MakeIWH(layer->drawToCanvasProperties()->bounds.width(), layer->drawToCanvasProperties()->bounds.height());
            needClip = true;
        }

        m_maskLayer = nullptr;
        if (-1 != layer->m_prop->maskLayerId) {
            m_maskLayer = host->getCCLayerById(layer->m_prop->maskLayerId);
            if (m_maskLayer) {
                needClip = true;
                SkRect skMaskClipRect = SkRect::MakeXYWH(
                    m_maskLayer->m_prop->position.x(), 
                    m_maskLayer->m_prop->position.y(),
                    m_maskLayer->m_prop->bounds.width(), 
                    m_maskLayer->m_prop->bounds.height());
                if (!skClipRect.intersect(skMaskClipRect))
                    skClipRect.setEmpty();
            }
        }

        if (needClip)
            canvas->clipRect(skClipRect);
    }

    ~DoClipLayer() {
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
        m_isClipChild = false;

        if (1 != child->tilesSize() && 0 != child->tilesSize()) {
            m_isClipChild = true;
            canvas->save();
            int bugFixMagicNum = child->drawToCanvasProperties()->currentTransform.isTranslate() ? 0 : 1;
            canvas->clipRect(SkRect::MakeIWH(child->drawToCanvasProperties()->bounds.width() - bugFixMagicNum, child->drawToCanvasProperties()->bounds.height() - bugFixMagicNum));
        }
    }

    void release()
    {
        if (m_isClipChild)
            m_canvas->restore();
    }

private:
    CompositingLayer* m_child;
    blink::WebCanvas* m_canvas;
    bool m_isClipChild;
};
 
void CompositingLayer::drawToCanvasChildren(LayerTreeHost* host, SkCanvas* canvas, const SkRect& clip, int deep)
{
    for (size_t i = 0; i < children().size(); ++i) {
        CompositingLayer* child = children()[i];

        const SkMatrix44& currentTransform = child->drawToCanvasProperties()->currentTransform;
        const SkMatrix44& transformToAncestor = child->drawToCanvasProperties()->screenSpaceTransform;

        SkMatrix matrixToAncestor;
        transformToFlattenedSkMatrix(transformToAncestor, &matrixToAncestor);

        if (opacity() < 1 && opacity() > 0) {
            U8CPU opacityVal = (int)ceil(opacity() * 255);
            canvas->saveLayerAlpha(nullptr, opacityVal);
        } else
            canvas->save();

        SkPaint paint;
        paint.setAntiAlias(false);
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        paint.setFilterQuality(kLow_SkFilterQuality);

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
    U8CPU alphaVal = (int)ceil(opacity() * 255);
    if (layerShouldBeSkipped(this, true) || 0 == alphaVal)
        return;

    for (TilesAddr::iterator it = m_tilesAddr->begin(); it != m_tilesAddr->end(); ++it) {
        CompositingTile* tile = (CompositingTile*)it->value;
        if (!tile->postion().intersects(clip) || !tile->bitmap())
            continue;

        blink::IntRect tilePostion = tile->postion();
        SkRect dst = (SkRect)(tilePostion);
        // SkRect src = SkRect::MakeWH(tile->bitmap()->width(), tile->bitmap()->height());
        // dst.intersect(SkRect::MakeXYWH(clip.x(), clip.y(), clip.width(), clip.height()));
        // SkIRect src = dst.makeOffset(-tile->postion().x(), -tile->postion().y()).roundOut();
       
        SkPaint paint;
        paint.setAntiAlias(false);
        paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
        paint.setFilterQuality(kLow_SkFilterQuality);

        SkColor* color = tile->getSolidColor();
        if (color) {
            paint.setColor(*color);
            paint.setAlpha((int)ceil((alphaVal * SkColorGetA(*color)) / 255.0));
            canvas->drawRect(dst, paint);

//             SkPaint paintTest;
//             const SkColor color2 = 0xff000000 | (rand() % 3) * (rand() % 7) * GetTickCount();
//             paintTest.setColor(color2);
//             paintTest.setStrokeWidth(4);
//             paintTest.setTextSize(13);
//             paintTest.setTextEncoding(SkPaint::kUTF8_TextEncoding);
// 
//             static SkTypeface* typeface = nullptr;
//             if (!typeface)
//                 typeface = SkTypeface::RefDefault(SkTypeface::kNormal);
//             paintTest.setTypeface(typeface);
// 
//             paintTest.setStrokeWidth(1);
//             String textTest = String::format("%d %d %x", m_id, alpha, *color);
//             CString cText = textTest.utf8();
//             canvas->drawText(cText.data(), cText.length(), 5 + (int)tilePostion.x(), 15 + (int)tilePostion.y(), paintTest);
        } else {
            if (!tile->bitmap() || !tile->bitmap()->getPixels())
                DebugBreak();
            paint.setAlpha(alphaVal);
            canvas->drawBitmapRect(*tile->bitmap(), nullptr, dst, &paint);
        }    
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