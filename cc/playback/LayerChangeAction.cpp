
#include "cc/playback/LayerChangeAction.h"

#include "cc/blink/WebLayerImpl.h"
#include "cc/layers/CompositingLayer.h"
#include "cc/trees/DrawProperties.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/playback/TileActionInfo.h"
#include "cc/raster/SkBitmapRefWrap.h"

#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

namespace cc {

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, layerChangeActionCounter, ("ccLayerChangeAction"));
#endif

//WTF::Vector<LayerChangeAction*>* gTestActions = nullptr;

LayerChangeAction::LayerChangeAction(int64 actionId, ChangeType type)
	: m_actionId(actionId)
	, m_type(type)
{
#ifndef NDEBUG
    layerChangeActionCounter.increment();
#endif
//     if (!gTestActions)
//         gTestActions = new Vector<LayerChangeAction*>();
// 
//     if (WTF::kNotFound == gTestActions->find(this))
//         gTestActions->append(this);
}

LayerChangeAction::~LayerChangeAction()
{
#ifndef NDEBUG
    layerChangeActionCounter.decrement();
#endif

//     size_t pos = gTestActions->find(this);
//     gTestActions->remove(pos);
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionCreate::LayerChangeActionCreate(int64 actionId, int layerId, cc_blink::WebLayerImplClient::Type layerType)
	: LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeCreate, layerId)
	, m_layerType(layerType)
{
}

void LayerChangeActionCreate::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionCreate::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

	CompositingLayer* layer = nullptr;
	switch (m_layerType)
	{
	case cc_blink::WebLayerImplClient::LayerType:
		layer = new CompositingLayer(m_layerId);
		break;
	case cc_blink::WebLayerImplClient::ContentLayerType:
		layer = new CompositingLayer(m_layerId);
		break;
	case cc_blink::WebLayerImplClient::ScrollbarLayerType:
		layer = new CompositingLayer(m_layerId);
		break;
	case cc_blink::WebLayerImplClient::ImageLayerType:
		layer = new CompositingImageLayer(m_layerId);
		break;
	default:
		RELEASE_ASSERT(false);
		break;
	}
    host->registerCCLayer(layer);
}

#define CHECK_LAYER_EMPTY(layer) \
	if (!layer) { \
		ASSERT(host->isRootCCLayerEmpty()); \
		return; \
	}

#define CHECK_LAYER_EMPTY2(layer1, layer2) \
	if (!layer || !layer2) { \
		ASSERT(host->isRootCCLayerEmpty()); \
		return; \
	}

#define CHECK_LAYER_EMPTY3(layer1, layer2, layer3) \
	if (!layer || !layer2 || !layer3) { \
		ASSERT(host->isRootCCLayerEmpty()); \
		return; \
	}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionDestroy::LayerChangeActionDestroy(int64 actionId, int layerId)
	: LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeDestroy, layerId)
{
// 	String outString = String::format("LayerChangeActionDestroy: %d %d %d \n", m_type, layerId, (int)actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void LayerChangeActionDestroy::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionDestroy::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
	CHECK_LAYER_EMPTY(layer);

    host->unregisterCCLayer(layer);
    delete layer;
}

//////////////////////////////////////////////////////////////////////////

void LayerChangeActionInsertChild::run(LayerTreeHost* host)
{
    CompositingLayer* layer = host->getCCLayerById(m_layerId);
    CompositingLayer* child = host->getCCLayerById(m_childId);
	CHECK_LAYER_EMPTY2(layer, child);

    layer->insertChild(child, m_index);
}

//////////////////////////////////////////////////////////////////////////

void LayerChangeActionReplaceChild::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionReplaceChild::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
    CompositingLayer* referenceId = host->getCCLayerById(m_referenceId);
    CompositingLayer* newLayerId = host->getCCLayerById(m_newLayerId);
	CHECK_LAYER_EMPTY3(layer, referenceId, newLayerId);
    layer->replaceChild(referenceId, newLayerId);
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionRemoveFromParent::LayerChangeActionRemoveFromParent(int actionId, int layerId)
	: LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeRemoveFromParent, layerId)
{
// 	String outString = String::format("LayerChangeActionRemoveFromParent: %d %d %d \n", m_type, layerId, (int)actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void LayerChangeActionRemoveFromParent::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionRemoveFromParent::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
	CHECK_LAYER_EMPTY(layer);
    layer->removeFromParent();
}

//////////////////////////////////////////////////////////////////////////

void LayerChangeActionRemoveAllChildren::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionRemoveAllChildren::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
	CHECK_LAYER_EMPTY(layer);
    layer->removeAllChildren();
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionDrawPropUpdata::LayerChangeActionDrawPropUpdata()
    : LayerChangeAction(-1, LayerChangeAction::LayerChangeDrawPropUpdata)
{
    
}

LayerChangeActionDrawPropUpdata::~LayerChangeActionDrawPropUpdata()
{
    ASSERT(0 == m_props.size());
    ASSERT(0 == m_layerIds.size());
}

void LayerChangeActionDrawPropUpdata::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionDrawPropUpdata::run: %d %d \n", m_type, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    ASSERT(m_layerIds.size() == m_props.size());
    for (size_t i = 0; i < m_layerIds.size(); ++i) {
        int layerId = m_layerIds[i];
        CompositingLayer* layer = host->getCCLayerById(layerId);
		if (!layer) {
			ASSERT(host->isRootCCLayerEmpty());
			continue;
		}

        DrawToCanvasProperties* prop = m_props[i];
        layer->updataDrawProp(prop);
        delete prop;
    }
    m_layerIds.clear();
    m_props.clear();

    for (size_t i = 0; i < m_pendingInvalidateRects.size(); ++i) {
        const blink::IntRect& r = m_pendingInvalidateRects[i];
        host->requestRepaint(r);
    }
}

void LayerChangeActionDrawPropUpdata::appendDirtyLayer(cc_blink::WebLayerImpl* layer)
{
    m_layerIds.append(layer->id());

    DrawToCanvasProperties* prop = new DrawToCanvasProperties();
    prop->copyDrawProperties(*layer->drawProperties(), layer->opacity());
    prop->bounds = layer->bounds();
    prop->position = layer->position();
	prop->drawsContent = layer->drawsContent();
    prop->masksToBounds = layer->masksToBounds();
    prop->opaque = layer->opaque();
    prop->maskLayerId = layer->maskLayerId();
    prop->replicaLayerId = layer->replicaLayerId();
    m_props.append(prop);

// 	String outString = String::format("LayerChangeActionDrawPropUpdata::appendDirtyLayer:%d %d\n", prop->drawsContent, layer->id());
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void LayerChangeActionDrawPropUpdata::appendPendingInvalidateRect(const blink::IntRect& r)
{
    m_pendingInvalidateRects.append(r);
}

void LayerChangeActionDrawPropUpdata::cleanupPendingInvalidateRectIfHasAlendAction()
{
	m_pendingInvalidateRects.clear();
}

const WTF::Vector<blink::IntRect>& LayerChangeActionDrawPropUpdata::dirtyRects() const
{
    return m_pendingInvalidateRects;
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionUpdataImageLayer::LayerChangeActionUpdataImageLayer(int actionId, int layerId, SkBitmapRefWrap* bitmap)
	: LayerChangeOneLayer(actionId, LayerChangeUpdataImageLayer, layerId)
	, m_bitmap(bitmap)
{
	m_bitmap->ref();
}

LayerChangeActionUpdataImageLayer::~LayerChangeActionUpdataImageLayer()
{
	m_bitmap->deref();
}

void LayerChangeActionUpdataImageLayer::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionUpdataImageLayer::run: %d %d %d \n", m_type, m_layerId, (int)m_actionId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

	CompositingLayer* layer = (host->getCCLayerById(m_layerId));
	CHECK_LAYER_EMPTY(layer);

	if (CompositingLayer::ImageCCLayer != layer->ccType()) {
		ASSERT(false);
		return;
	}
	CompositingImageLayer* imageLayer = (CompositingImageLayer*)(layer);
	imageLayer->setImage(m_bitmap);
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionBlend::Item::~Item()
{
    delete bitmap;
    delete willRasteredTiles;
}

LayerChangeActionBlend::LayerChangeActionBlend(int actionId, int layerId, TileActionInfoVector* willRasteredTiles, const blink::IntRect& dirtyRect, SkBitmap* bitmap)
    : LayerChangeAction(actionId, LayerChangeBlend)
	, m_item(new Item(layerId, willRasteredTiles, dirtyRect, bitmap))
{
//     String outString = String::format("LayerChangeActionBlend: %d %d, %d %d\n", dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());
}

LayerChangeActionBlend::~LayerChangeActionBlend()
{
	delete m_item;
}

void LayerChangeActionBlend::setBitmap(/*size_t itemId, */SkBitmap* bitmap)
{
	m_item->bitmap = bitmap;
}

void LayerChangeActionBlend::appendPendingInvalidateRect(const blink::IntRect& r)
{
	m_pendingInvalidateRects.append(r);
}

void LayerChangeActionBlend::appendPendingInvalidateRects(const WTF::Vector<blink::IntRect>& rects)
{
    m_pendingInvalidateRects.appendVector(rects);
}

void LayerChangeActionBlend::run(LayerTreeHost* host)
{
    // 	String outString = String::format("LayerChangeActionBlend::run: layerId:%d %d \n", item->layerId, (int)m_actionId);
    // 	OutputDebugStringW(outString.charactersWithNullTermination().data());

	Item* item = m_item;
	CompositingLayer* layer = host->getCCLayerById(item->layerId);
	CHECK_LAYER_EMPTY(layer);

    if (item->bitmap)
        layer->blendToTiles(item->willRasteredTiles, *(item->bitmap), item->dirtyRect);

    for (size_t i = 0; i < m_pendingInvalidateRects.size(); ++i) {
        const blink::IntRect& r = m_pendingInvalidateRects[i];
        host->requestRepaint(r);
    }
}

//////////////////////////////////////////////////////////////////////////

LayerChangeActionUpdataTile::LayerChangeActionUpdataTile(int actionId, int layerId, int newIndexNumX, int newIndexNumY)
    : LayerChangeOneLayer(actionId, LayerChangeTileUpdata, layerId)
    , m_newIndexNumX(newIndexNumX)
    , m_newIndexNumY(newIndexNumY)
{
// 	String outString = String::format("LayerChangeActionUpdataTile::LayerChangeActionUpdataTile: actionId:%d layerId:%d, %d %d\n", actionId, layerId, newIndexNumX, newIndexNumY);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void LayerChangeActionUpdataTile::run(LayerTreeHost * host)
{
// 	String outString = String::format("LayerChangeActionUpdataTile::run  m_actionId: %d, m_layerId:%d \n", m_actionId, m_layerId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
	CHECK_LAYER_EMPTY(layer);
    layer->updataTile(m_newIndexNumX, m_newIndexNumY);
}

//////////////////////////////////////////////////////////////////////////
LayerChangeActionCleanupUnnecessaryTile::LayerChangeActionCleanupUnnecessaryTile(int layerId)
    : LayerChangeOneLayer(-1, LayerChangeTileUpdata, layerId)
{

}

LayerChangeActionCleanupUnnecessaryTile::~LayerChangeActionCleanupUnnecessaryTile()
{
	ASSERT(0 == m_tiles.size());
}

void LayerChangeActionCleanupUnnecessaryTile::appendTile(int index, int xIndex, int yIndex)
{
    m_tiles.append(new TileActionInfo(index, xIndex, yIndex));
}

void LayerChangeActionCleanupUnnecessaryTile::run(LayerTreeHost* host)
{
// 	String outString = String::format("LayerChangeActionCleanupUnnecessaryTile::run  m_actionId: %d, m_layerId:%d \n", m_actionId, m_layerId);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    CompositingLayer* layer = host->getCCLayerById(m_layerId);
	CHECK_LAYER_EMPTY(layer);
	layer->cleanupUnnecessaryTile(m_tiles);
	clear();
}

void LayerChangeActionCleanupUnnecessaryTile::clear()
{
	for (size_t i = 0; i < m_tiles.size(); ++i) {
		delete m_tiles[i];
	}
	m_tiles.clear();
}

//////////////////////////////////////////////////////////////////////////


}