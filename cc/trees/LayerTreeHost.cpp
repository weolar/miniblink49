
#include "cc/trees/LayerTreeHost.h"

#include "SkCanvas.h"
#include "skia/ext/bitmap_platform_device_win.h"
#include "skia/ext/platform_canvas.h"

#include "cc/blink/WebLayerImpl.h"
#include "cc/blink/WebLayerImplClient.h"
#include "cc/trees/DrawProperties.h"
#include "cc/trees/LayerSorter.h"
#include "cc/trees/ActionsFrameGroup.h"
#include "cc/trees/LayerTreeHostClient.h"
#include "cc/tiles/Tile.h"
#include "cc/tiles/TileGrid.h"
#include "cc/raster/RasterTask.h"
#include "cc/layers/CompositingLayer.h"
#include "cc/playback/LayerChangeAction.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/public/platform/WebFloatSize.h"
#include "third_party/WebKit/public/platform/WebGestureCurveTarget.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/Source/platform/WebThreadSupportingGC.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h" // TODO
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "wke/wkeWebView.h"

extern DWORD g_nowTime;

using namespace blink;

namespace cc {

LayerTreeHost* gLayerTreeHost = nullptr;

static void initializeCompositeThread(blink::WebThreadSupportingGC* webThreadSupportingGC)
{
    webThreadSupportingGC->initialize();
}

LayerTreeHost::LayerTreeHost(LayerTreeHostClent* hostClient, LayerTreeHostUiThreadClient* uiThreadClient)
{
    m_rootLayer = nullptr;
    m_rootCCLayer = nullptr;
    m_deviceScaleFactor = 1.0f;
    m_backgroundColor = 0xff00ffff;
    m_hasTransparentBackground = false;
    m_visible = true;
    m_pageScaleFactor = 1.0f;
    m_minimum = 1.0f;
    m_maximum = 1.0f;
    m_hostClient = hostClient;
    m_uiThreadClient = uiThreadClient;
    //m_needsFullTreeSync = true;
    m_needTileRender = true;
    m_layerTreeDirty = true;
    m_3dNodesCount = 0;
    m_tilesMutex = new WTF::Mutex();
    m_rasterNotifMutex = new WTF::Mutex();
    m_newestDrawingIndex = 0;
    m_drawingIndex = 1;
    m_actionsFrameGroup = new ActionsFrameGroup(this);
    m_isDestroying = false;
    
    m_memoryCanvas = nullptr;
    m_paintToMemoryCanvasInUiThreadTaskCount = 0;
    m_drawMinInterval = 0.003;

    m_isDrawDirty = true;
    m_lastCompositeTime = 0.0;
    m_lastPaintTime = 0.0;
    m_lastRecordTime = 0.0;
    m_drawFrameCount = 0;
    m_drawFrameFinishCount = 0;
    m_requestApplyActionsCount = 0;
    m_requestApplyActionsFinishCount = 0;
    m_postpaintMessageCount = 0;
    m_hasResize = false;
    m_compositeThread = nullptr;
    if (m_uiThreadClient) {
        m_compositeThread = blink::WebThreadSupportingGC::create("CompositeThread");
        m_compositeThread->platformThread().postTask(FROM_HERE, WTF::bind(&initializeCompositeThread, m_compositeThread.get()));
    }
    gLayerTreeHost = this;
}

static void shutdownCompositeThread(blink::WebThreadSupportingGC* webThread, int* waitCount)
{
    webThread->shutdown();
    atomicDecrement(waitCount);
}

LayerTreeHost::~LayerTreeHost()
{
    m_isDestroying = true;

    while (0 != RasterTaskWorkerThreadPool::shared()->getPendingRasterTaskNum()) { ::Sleep(20); }
    requestApplyActionsToRunIntoCompositeThread(true);

    m_compositeMutex.lock();
    for (auto it : m_wrapSelfForUiThreads) {
        it->m_host = nullptr;
    }
    m_wrapSelfForUiThreads.clear();
    m_compositeMutex.unlock();

    //while (0 != m_paintToMemoryCanvasInUiThreadTaskCount) { ::Sleep(20); }
    waitForApplyActions();

    ASSERT(0 == m_actionsFrameGroup->getFramesSize());
    if (m_compositeThread) {
        int waitCount = 1;
        blink::WebThreadSupportingGC* webThread = m_compositeThread.leakPtr();
        webThread->platformThread().postTask(FROM_HERE, WTF::bind(&shutdownCompositeThread, webThread, &waitCount));
        while (waitCount) {
            ::Sleep(100);
        }
        delete webThread;
    }

    if (m_memoryCanvas)
        delete m_memoryCanvas;
    m_memoryCanvas = nullptr;
    
    for (WTF::HashMap<int, cc_blink::WebLayerImpl*>::iterator it = m_liveLayers.begin(); m_liveLayers.end() != it; ++it) {
        cc_blink::WebLayerImpl* layer = it->value;
        layer->setLayerTreeHost(nullptr);
    }
    m_liveLayers.clear();

    ASSERT(0 == m_actionsFrameGroup->getFramesSize());

    for (WTF::HashMap<int, CompositingLayer*>::iterator it = m_liveCCLayers.begin(); m_liveCCLayers.end() != it; ++it) {
        CompositingLayer* ccLayer = it->value;
        ccLayer->setParent(nullptr);
        delete ccLayer;
    }
    m_liveCCLayers.clear();

    ASSERT(0 == m_actionsFrameGroup->getFramesSize());
    
    for (size_t i = 0; i < m_dirtyLayersGroup.size(); ++i) {
        DirtyLayers* dirtyLayers = m_dirtyLayersGroup[i];
        delete dirtyLayers;
    }
    m_dirtyLayersGroup.clear();

    ASSERT(0 == m_actionsFrameGroup->getFramesSize());

    delete m_rasterNotifMutex;
    m_rasterNotifMutex = nullptr;

    ASSERT(0 == m_actionsFrameGroup->getFramesSize());

    delete m_tilesMutex;
    m_tilesMutex = nullptr;

    waitForApplyActions();
    ASSERT(0 == m_actionsFrameGroup->getFramesSize());

    delete m_actionsFrameGroup;
    m_actionsFrameGroup = nullptr;
}

void LayerTreeHost::waitForDrawFrame()
{
    int finishCount = 0;
    do {
        m_compositeMutex.lock();
        finishCount = m_drawFrameFinishCount;
        m_compositeMutex.unlock();
        Sleep(20);
    } while (0 != finishCount);
}

void LayerTreeHost::waitForApplyActions()
{
    waitForDrawFrame();

    int finishCount = 0;
    do {
        m_compositeMutex.lock();
        finishCount = m_requestApplyActionsFinishCount;
        m_compositeMutex.unlock();
        Sleep(20);
    } while (0 != finishCount);
}

bool LayerTreeHost::isDestroying() const
{
    return m_isDestroying;
}

void LayerTreeHost::registerLayer(cc_blink::WebLayerImpl* layer)
{
    layer->setBackgroundColor(getRealColor(m_hasTransparentBackground, m_backgroundColor));
    m_liveLayers.add(layer->id(), layer);
}

void LayerTreeHost::unregisterLayer(cc_blink::WebLayerImpl* layer)
{
    m_liveLayers.remove(layer->id());
}

void LayerTreeHost::registerCCLayer(CompositingLayer* layer)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    m_liveCCLayers.add(layer->id(), layer);
}

void LayerTreeHost::unregisterCCLayer(CompositingLayer* layer)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    m_liveCCLayers.remove(layer->id());
}

cc_blink::WebLayerImpl* LayerTreeHost::getLayerById(int id)
{
    WTF::HashMap<int, cc_blink::WebLayerImpl*>::iterator it = m_liveLayers.find(id);
    if (m_liveLayers.end() != it)
        return it->value;

    return nullptr;
}

CompositingLayer* LayerTreeHost::getCCLayerById(int id)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    WTF::HashMap<int, CompositingLayer*>::iterator it = m_liveCCLayers.find(id);
    if (m_liveCCLayers.end() != it)
        return it->value;

    return nullptr;
}

void LayerTreeHost::gc()
{
    for (auto it = m_liveLayers.begin(); it != m_liveLayers.end(); ++it) {
        cc_blink::WebLayerImpl* layer = it->value;
        layer->gc();
    }
}

void LayerTreeHost::increaseNodesCount()
{
    ++m_3dNodesCount;
}

void LayerTreeHost::decreaseNodesCount()
{
    --m_3dNodesCount;
}

bool LayerTreeHost::has3dNodes()
{
    return m_3dNodesCount > 0;
}

WTF::Mutex* LayerTreeHost::tilesMutex()
{
    return m_tilesMutex;
}

int64 LayerTreeHost::createDrawingIndex()
{
    ++m_newestDrawingIndex;
    return m_newestDrawingIndex;
}

int64 LayerTreeHost::drawingIndex()
{
    return m_drawingIndex;
}

void LayerTreeHost::addRasteringIndex(int64 index)
{
    WTF::MutexLocker locker(*m_tilesMutex);
    m_rasteringIndexs.append(index);
}

int64 LayerTreeHost::frontRasteringIndex()
{
    WTF::MutexLocker locker(*m_tilesMutex);
    ASSERT(0 != m_rasteringIndexs.size());
    if (0 == m_rasteringIndexs.size())
        return -1;
    return m_rasteringIndexs[0];
}

void LayerTreeHost::popRasteringIndex()
{
    WTF::MutexLocker locker(*m_tilesMutex);
    if (0 < m_rasteringIndexs.size())
        m_rasteringIndexs.remove(0);
}

void LayerTreeHost::setWebGestureCurveTarget(blink::WebGestureCurveTarget* webGestureCurveTarget)
{
    m_webGestureCurveTarget = webGestureCurveTarget;
}

// void LayerTreeHost::setNeedsCommit() // 暂时被废弃，由setLayerTreeDirty代替
// {
//     // 由光栅化线程来提起脏区域，所以这里直接指定需要开始下一帧，光删化完毕后由光栅化线程通过requestRepaint发起重绘
//     m_hostClient->scheduleAnimation();
// }

void LayerTreeHost::setLayerTreeDirty()
{
    m_layerTreeDirty = true;
    m_hostClient->onLayerTreeDirty();
}

bool LayerTreeHost::isLayerTreeDirty() const
{
    return m_layerTreeDirty;
}

// void LayerTreeHost::setNeedsFullTreeSync()
// {
//     m_needsFullTreeSync = true;
//     //m_hostClient->scheduleAnimation();
// }

void LayerTreeHost::requestRepaint(const blink::IntRect& repaintRect)
{
    m_hostClient->onLayerTreeInvalidateRect(blink::WebRect(repaintRect));
}

void LayerTreeHost::requestDrawFrameLocked(DirtyLayers* dirtyLayers, Vector<Tile*>* tilesToUIThreadRelease)
{
    DebugBreak();
}

static bool compareDirtyLayer(DirtyLayers*& left, DirtyLayers*& right)
{
    return left->drawingIndex() < right->drawingIndex();
}

static bool compareAction(LayerChangeAction*& left, LayerChangeAction*& right)
{
    return left->actionId() < right->actionId();
}

bool LayerTreeHost::canRecordActions() const
{
    if (!m_actionsFrameGroup->containComefromMainframeLocked())
        return true;

    if (RasterTaskWorkerThreadPool::shared()->getPendingRasterTaskNum() > 10)
        return false;

    if (!m_actionsFrameGroup || m_actionsFrameGroup->getFramesSize() > 30)
        return false;
    
    double lastRecordTime = WTF::monotonicallyIncreasingTime();
    double detTime = lastRecordTime - m_lastRecordTime;
    if (detTime < m_drawMinInterval)
        return false;
    m_lastRecordTime = lastRecordTime;

    return true;
}

void LayerTreeHost::beginRecordActions(bool isComefromMainframe)
{
    if (m_isDestroying)
        return;
    m_actionsFrameGroup->beginRecordActions(isComefromMainframe);
}

void LayerTreeHost::endRecordActions()
{
    if (m_isDestroying)
        return;
    m_actionsFrameGroup->endRecordActions();
}

void LayerTreeHost::appendLayerChangeAction(LayerChangeAction* action)
{
    m_actionsFrameGroup->saveLayerChangeAction(action);

    setLayerTreeDirty();
}

int64 LayerTreeHost::genActionId()
{
    return m_actionsFrameGroup->genActionId();
}

bool LayerTreeHost::preDrawFrame()
{
    return applyActions(m_isDestroying);
}

bool LayerTreeHost::applyActions(bool needCheck)
{
    ASSERT(!m_compositeThread || !isMainThread());
    return m_actionsFrameGroup->applyActions(needCheck);
}

void LayerTreeHost::postDrawFrame()
{
    ASSERT(!m_compositeThread || !isMainThread());
}

void LayerTreeHost::scrollBy(const blink::WebFloatSize& delta, const blink::WebFloatSize& velocity)
{

}

blink::IntSize LayerTreeHost::canDrawSize()
{
    blink::IntSize outSize = deviceViewportSize();
    outSize.scale(1.2);
    return outSize;
}

static blink::WebDoublePoint getEffectiveTotalScrollOffset(cc_blink::WebLayerImpl* layer) {
    //     if (layer->DrawIgnoreScrollOffset())
    //         return gfx::Vector2dF();
    blink::WebDoublePoint offset = layer->scrollPositionDouble();
    // The scroll parent's total scroll offset (scroll offset + scroll delta)
    // can't be used because its scroll offset has already been applied to the
    // scroll children's positions by the main thread layer positioning code.
    //     if (layer->scrollParent())
    //         offset += layer->scroll_parent()->ScrollDelta();
    return offset;
}

// static void updateLayer(cc_blink::WebLayerImpl* layer, SkCanvas* canvas, const blink::IntRect& clip)
// {
//     blink::WebFloatPoint position = layer->position();
//     blink::WebSize size = layer->bounds();
//     blink::IntRect childClip(0, 0, size.width, size.height);
// 
// //     childClip.intersect(clip);
// //     if (childClip.isEmpty())
// //         return;
//     layer->updataAndPaintContents(canvas, childClip);
// }
//
// static void updateLayerChildren(cc_blink::WebLayerImpl* layer, SkCanvas* canvas, const blink::IntRect& clip, bool needsFullTreeSync)
// {
//     blink::WebFloatPoint currentLayerPosition = layer->position();
//     blink::WebDoublePoint effectiveTotalScrollOffset = getEffectiveTotalScrollOffset(layer);
//     blink::WebFloatPoint currentLayerPositionScrolled(currentLayerPosition.x - effectiveTotalScrollOffset.x, currentLayerPosition.y - effectiveTotalScrollOffset.y);
// 
//     blink::WebFloatPoint3D transformOrigin = layer->transformOrigin();
// 
//     SkMatrix44 combinedTransform(SkMatrix44::kIdentity_Constructor);
//     combinedTransform.preTranslate(currentLayerPositionScrolled.x + transformOrigin.x, currentLayerPositionScrolled.y + transformOrigin.y, currentLayerPosition.x);
//     combinedTransform.preConcat(layer->transform());
//     combinedTransform.preTranslate(-transformOrigin.x, -transformOrigin.y, -transformOrigin.z);
// 
//     if (!combinedTransform.isIdentity()) {
//         canvas->save();
//         canvas->concat(combinedTransform);
//     }
// 
//     blink::WebSize size = layer->bounds();
//     blink::IntRect layerRect(0, 0, size.width, size.height);
// 
//     bool layerClip = 0 != layerRect.width() && 0 != layerRect.height() && layer->masksToBounds();
//     if (layerClip) {
//         canvas->save();
//         canvas->clipRect(layerRect, SkRegion::kIntersect_Op, false);
//     }
// 
//     if (needsFullTreeSync)
//         updateLayer(layer, canvas, clip);
// 
//     blink::IntRect clipInChildCoordinate = clip;
//     clipInChildCoordinate.move(-currentLayerPositionScrolled.x, -currentLayerPositionScrolled.y);
// 
//     WTF::Vector<cc_blink::WebLayerImpl*>& children = layer->children();
//     for (size_t i = 0; i < children.size(); ++i) {
//         cc_blink::WebLayerImpl* child = children[i];
// #if 1
//         if (!(child->dirty() || child->childrenDirty() || needsFullTreeSync))
//             continue;
// #endif
// 
//         child->clearChildrenDirty();
//         updateLayerChildren(child, canvas, clipInChildCoordinate, needsFullTreeSync);
//     }
// 
//     if (layerClip)
//         canvas->restore();
// 
//     if (!combinedTransform.isIdentity())
//         canvas->restore();
// }
// 
// void LayerTreeHost::updateLayers(SkCanvas* canvas, const blink::IntRect& clip, bool needsFullTreeSync)
// {
//     if (!m_rootLayer)
//         return;
// 
//     updateLayerChildren(m_rootLayer, canvas, clip, m_needsFullTreeSync || needsFullTreeSync);
//     m_needsFullTreeSync = false;
// }

static void flattenTo2d(SkMatrix44& matrix)
{
    matrix.set(2, 0, 0.0);
    matrix.set(2, 1, 0.0);
    matrix.set(0, 2, 0.0);
    matrix.set(1, 2, 0.0);
    matrix.set(2, 2, 1.0);
    matrix.set(3, 2, 0.0);
    matrix.set(2, 3, 0.0);
}

void LayerTreeHost::recordDraw()
{
    if (blink::RuntimeEnabledFeatures::headlessEnabled())
        return;
    if (!m_rootLayer)
        return;

    updateLayersDrawProperties();

    cc::RasterTaskGroup* taskGroup = RasterTaskWorkerThreadPool::shared()->beginPostRasterTask(this);

    for (size_t i = 0; i < m_pendingRepaintRectsInRootLayerCoordinate.size(); ++i) {
        SkRect r = m_pendingRepaintRectsInRootLayerCoordinate[i];
        if (!r.isEmpty())
            taskGroup->appendPendingInvalidateRect(r);
    }

    m_pendingRepaintRectsInRootLayerCoordinate.clear();

    m_rootLayer->recordDrawChildren(taskGroup, 0);
    taskGroup->endPostRasterTask();
}

void printTrans(const SkMatrix44& transform, int deep)
{
    SkMScalar total = 0.0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            total += transform.get(i, j);
        }
    }
    
    String outString = String::format("LayerTreeHost::printTrans:%d, %f \n", deep, total);
    OutputDebugStringW(outString.charactersWithNullTermination().data());
}

bool LayerTreeHost::drawToCanvas(SkCanvas* canvas, const SkRect& dirtyRect)
{
    if (dirtyRect.isEmpty())
        return false;

    canvas->save();
    canvas->clipRect(dirtyRect);
       
    SkPaint clearColorPaint;
    clearColorPaint.setColor(getRealColor(m_hasTransparentBackground, m_backgroundColor));

    // http://blog.csdn.net/to_be_designer/article/details/48530921
    clearColorPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode); // SkXfermode::kSrcOver_Mode
    canvas->drawRect((SkRect)dirtyRect, clearColorPaint);

    bool b = false;
    WTF::Locker<WTF::Mutex> locker(m_rootCCLayerMutex);
    if (getRootCCLayer())
        b = m_rootCCLayer->drawToCanvasChildren(this, canvas, dirtyRect, 1, 0);

    canvas->restore();

    return b;
}

static bool isIdentityOrIntegerTranslation(const SkMatrix44& transform)
{
    if (!transform.isTranslate())
        return false;

    bool noFractionalTranslation =
        static_cast<int>(transform.get(0, 3)) == transform.get(0, 3) &&
        static_cast<int>(transform.get(1, 3)) == transform.get(1, 3) &&
        static_cast<int>(transform.get(2, 3)) == transform.get(2, 3);

    return noFractionalTranslation;
}

struct DrawPropertiesFromAncestor {
    DrawPropertiesFromAncestor()
    {
        transform = SkMatrix44(SkMatrix44::kIdentity_Constructor);
        opacity = 1.0;
        subtreeCanUseLcdText = true;
    }

    SkMatrix44 transform;
    float opacity;
    bool subtreeCanUseLcdText;
};

static void updateChildLayersDrawProperties(cc_blink::WebLayerImpl* layer, LayerSorter& layerSorter, const DrawPropertiesFromAncestor& propFromAncestor, int deep)
{
    WTF::Vector<cc_blink::WebLayerImpl*>& children = layer->children();
    
    for (size_t i = 0; i < children.size(); ++i) {
        cc_blink::WebLayerImpl* child = children[i];
        DrawProps* drawProperties = child->drawProperties();

        blink::WebFloatPoint currentLayerPosition = child->position();
        blink::WebDoublePoint effectiveTotalScrollOffset = getEffectiveTotalScrollOffset(child);
        blink::WebFloatPoint currentLayerPositionScrolled(currentLayerPosition.x - effectiveTotalScrollOffset.x, currentLayerPosition.y - effectiveTotalScrollOffset.y);
        blink::WebFloatPoint3D transformOrigin = child->transformOrigin();

        SkMatrix44 currentTransform(SkMatrix44::kIdentity_Constructor);
        SkMatrix44 childTransform = child->transform();

        if (!childTransform.isIdentity()) {
            currentTransform.preTranslate(currentLayerPositionScrolled.x + transformOrigin.x, currentLayerPositionScrolled.y + transformOrigin.y, transformOrigin.z);
            currentTransform.preConcat(childTransform);
            currentTransform.preTranslate(-transformOrigin.x, -transformOrigin.y, -transformOrigin.z);
        } else {
            currentTransform.preTranslate(currentLayerPositionScrolled.x, currentLayerPositionScrolled.y, 0);
        }

        // Flatten to 2D if the layer doesn't preserve 3D.
        SkMatrix44 combinedTransform = propFromAncestor.transform;
        combinedTransform.preConcat(currentTransform);

        SkMatrix44 transformToAncestorIfFlatten = combinedTransform;
        if (child->shouldFlattenTransform())
            flattenTo2d(transformToAncestorIfFlatten);

        drawProperties->screenSpaceTransform = combinedTransform;
        drawProperties->targetSpaceTransform = combinedTransform;
        drawProperties->currentTransform = currentTransform;

        bool layerCanUseLcdText = true;
        bool subtreeCanUseLcdText = true;

        DrawPropertiesFromAncestor prop;
        prop.transform = transformToAncestorIfFlatten;
        prop.opacity *= child->opacity();

        // To avoid color fringing, LCD text should only be used on opaque layers with just integral translation.
        subtreeCanUseLcdText = /*propFromAncestor.subtreeCanUseLcdText &&*/ prop.opacity == 1.f && isIdentityOrIntegerTranslation(propFromAncestor.transform);
        // Also disable LCD text locally for non-opaque content.
        layerCanUseLcdText = subtreeCanUseLcdText && child->opaque();
        drawProperties->layerCanUseLcdText = layerCanUseLcdText;

        prop.subtreeCanUseLcdText = subtreeCanUseLcdText;
        updateChildLayersDrawProperties(child, layerSorter, prop, deep + 1);
    }

    if (children.size() && layer->layerTreeHost()->has3dNodes() && layer->is3dSorted()) {
        layerSorter.Sort(children.begin(), children.end());
    }
}

void LayerTreeHost::updateLayersDrawProperties()
{
    LayerSorter layerSorter;
    SkMatrix44 transform(SkMatrix44::kIdentity_Constructor);

    DrawPropertiesFromAncestor prop;
    updateChildLayersDrawProperties(m_rootLayer, layerSorter, prop, 0);
}

inline SkRect unionRect(const SkRect& a, const SkRect& b)
{
    SkRect c = a;
    c.join(b);
    return c;
}

static SkScalar rectArea(const SkRect& r)
{
    return (r.width()) * (r.height());
}

static SkScalar unionArea(const SkRect& r1, const SkRect& r2)
{
    SkRect area;
    area = unionRect(r1, r2);
    return rectArea(area);
}

static void mergeDirty(Vector<SkRect>* dirtyRects)
{
    do {
        int nDirty = (int)dirtyRects->size();
        if (nDirty < 1)
            break;

        SkScalar bestDelta = 0;
        int mergeA = 0;
        int mergeB = 0;
        for (int i = 0; i < nDirty - 1; i++) {
            for (int j = i + 1; j < nDirty; j++) {
                SkScalar delta = unionArea(dirtyRects->at(i), dirtyRects->at(j)) - rectArea(dirtyRects->at(i)) - rectArea(dirtyRects->at(j));
                if (bestDelta >= delta) {
                    mergeA = i;
                    mergeB = j;
                    bestDelta = delta;
                }
            }
        }

        if (mergeA == mergeB)
            break;

        dirtyRects->at(mergeA).join(dirtyRects->at(mergeB));
        for (int i = mergeB + 1; i < nDirty; i++)
            dirtyRects->at(i - 1) = dirtyRects->at(i);

        dirtyRects->removeLast();
    } while (true);
}

static void addAndMergeDirty(Vector<SkRect>* dirtyRects, const SkRect& r)
{
    size_t dirtySize = dirtyRects->size();
    if (0 == dirtySize) {
        dirtyRects->append(r);
        return;
    }

    SkScalar bestDelta = 0;
    size_t mergePos = (size_t)-1;
    for (size_t i = 0; i < dirtySize; i++) {
        SkScalar delta = unionArea(dirtyRects->at(i), r) - rectArea(dirtyRects->at(i)) - rectArea(r);
        if (delta > bestDelta)
            continue;
        bestDelta = delta;
        mergePos = i;
    }

    if (mergePos == (size_t)-1)
        dirtyRects->append(r);
    else
        dirtyRects->at(mergePos).join(r);
}

void LayerTreeHost::appendPendingRepaintRect(const SkRect& r)
{
    addAndMergeDirty(&m_pendingRepaintRectsInRootLayerCoordinate, r);
}

static void showDebugChildren(cc_blink::WebLayerImpl* layer, int deep)
{
    Vector<LChar> blankSpaceString;
    blankSpaceString.resize(deep);
    blankSpaceString.fill(' ');

    WTF::Vector<cc_blink::WebLayerImpl*>& children = layer->children();
    for (size_t i = 0; i < children.size(); ++i) {
        cc_blink::WebLayerImpl* child = children[i];

        blink::WebFloatPoint position = child->position();
        blink::WebSize bounds = child->bounds();
        
        String msg = String::format("%p, %d %d %d %d - %d, (%f %f)\n", child,
            (int)position.x, (int)position.y, bounds.width, bounds.height, child->id(), (child->transform().get(0, 3)), (child->transform().get(1, 3)));
        msg.insert(blankSpaceString.data(), blankSpaceString.size(), 0);
        OutputDebugStringA(msg.utf8().data());

        showDebugChildren(child, deep + 1);
    }
}

void LayerTreeHost::showDebug()
{
    if (!m_rootLayer)
        return;
    showDebugChildren(m_rootLayer, 1);
}

// Sets the root of the tree. The root is set by way of the constructor.
void LayerTreeHost::setRootLayer(const blink::WebLayer& layer)
{
    m_rootLayer = (cc_blink::WebLayerImpl*)&layer;
    m_rootLayer->setLayerTreeHost(this);

    requestApplyActionsToRunIntoCompositeThread(false);

    getRootCCLayer();

    //setNeedsFullTreeSync();
}

CompositingLayer* LayerTreeHost::getRootCCLayer()
{
    WTF::Locker<WTF::Mutex> locker(m_rootCCLayerMutex);
    if (m_rootCCLayer)
        return m_rootCCLayer;
    if (!m_rootLayer)
        return nullptr;

    m_rootCCLayer = getCCLayerById(m_rootLayer->id());
    return m_rootCCLayer;
}

void LayerTreeHost::clearRootLayer()
{
    m_rootLayer = nullptr;

    while (0 != RasterTaskWorkerThreadPool::shared()->getPendingRasterTaskNum()) { ::Sleep(20); }

    requestApplyActionsToRunIntoCompositeThread(false);

    m_rootCCLayerMutex.lock();
    m_rootCCLayer = nullptr;
    m_rootCCLayerMutex.unlock();
}

void LayerTreeHost::setViewportSize(const blink::WebSize& deviceViewportSize)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    m_deviceViewportSize = deviceViewportSize;
    m_clientRect = blink::IntRect(blink::IntPoint(), m_deviceViewportSize);
    m_hasResize = true;
}

// Gives the viewport size in physical device pixels.
blink::WebSize LayerTreeHost::deviceViewportSize() const
{
    return blink::WebSize(m_deviceViewportSize.width(), m_deviceViewportSize.height());
}

void LayerTreeHost::setDeviceScaleFactor(float scale)
{
    m_deviceScaleFactor = scale;
}

float LayerTreeHost::deviceScaleFactor() const
{
    return m_deviceScaleFactor;
}

static void recursiveSetColor(WTF::HashMap<int, cc_blink::WebLayerImpl*>* layers, bool hasTransparentBackground, SkColor backgroundColor)
{
    for (auto it = layers->begin(); it != layers->end(); ++it) {
        cc_blink::WebLayerImpl* layer = it->value;
        layer->setBackgroundColor(getRealColor(hasTransparentBackground, backgroundColor));
    }
}

void LayerTreeHost::setBackgroundColor(blink::WebColor color)
{
    m_backgroundColor = color;
    recursiveSetColor(&m_liveLayers, m_hasTransparentBackground, m_backgroundColor);
}

blink::WebColor LayerTreeHost::getBackgroundColor() const
{
    return m_backgroundColor;
}

void LayerTreeHost::setHasTransparentBackground(bool b)
{
    if (m_hasTransparentBackground == b)
        return;
    m_hasTransparentBackground = b;
    recursiveSetColor(&m_liveLayers, m_hasTransparentBackground, m_backgroundColor);
}

bool LayerTreeHost::getHasTransparentBackground() const
{ 
    return m_hasTransparentBackground;
}

void LayerTreeHost::registerForAnimations(blink::WebLayer* layer)
{
    // 不能在这里设置LayerTreeHost，因为popup 类型的会把新窗口的layer调用本接口到老的host来。
//     cc_blink::WebLayerImpl* layerImpl = (cc_blink::WebLayerImpl*)layer;
//     layerImpl->setLayerTreeHost(this);
}

// Sets whether this view is visible. In threaded mode, a view that is not visible will not
// composite or trigger updateAnimations() or layout() calls until it becomes visible.
void LayerTreeHost::setVisible(bool visible)
{
    m_visible = visible;
}

// Sets the current page scale factor and minimum / maximum limits. Both limits are initially 1 (no page scale allowed).
void LayerTreeHost::setPageScaleFactorAndLimits(float pageScaleFactor, float minimum, float maximum)
{
    m_pageScaleFactor = pageScaleFactor;
    m_minimum = minimum;
    m_maximum = maximum;
}

// Starts an animation of the page scale to a target scale factor and scroll offset.
// If useAnchor is true, destination is a point on the screen that will remain fixed for the duration of the animation.
// If useAnchor is false, destination is the final top-left scroll position.
void LayerTreeHost::startPageScaleAnimation(const blink::WebPoint& destination, bool useAnchor, float newPageScale, double durationSec)
{

}

void LayerTreeHost::setNeedsAnimate()
{
    m_hostClient->onLayerTreeSetNeedsCommit();
}

void LayerTreeHost::finishAllRendering()
{
    OutputDebugStringW(L"LayerTreeHost::finishAllRendering");
}

//////////////////////////////////////////////////////////////////////////

blink::IntRect LayerTreeHost::getClientRect()
{
    blink::IntRect clientRect;
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    clientRect = m_clientRect;
    return clientRect;
}

void LayerTreeHost::requestDrawFrameToRunIntoCompositeThread()
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    if (!m_compositeThread) {
        RELEASE_ASSERT(!m_uiThreadClient);
        return;
    }
    if (0 != m_drawFrameCount)
        return;

    atomicIncrement(&m_drawFrameCount);
    atomicIncrement(&m_drawFrameFinishCount);
    m_compositeThread->platformThread().postTask(FROM_HERE, WTF::bind(&LayerTreeHost::drawFrameInCompositeThread, this));
}

void LayerTreeHost::requestApplyActionsToRunIntoCompositeThread(bool needCheck)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    if (!m_compositeThread) {
        RELEASE_ASSERT(!m_uiThreadClient);
        return;
    }
    if (0 != m_requestApplyActionsCount && !needCheck) // 如果needCheck==true，则表示是退出流程，必须执行一次onApply
        return;

    atomicIncrement(&m_requestApplyActionsCount);
    atomicIncrement(&m_requestApplyActionsFinishCount);
    m_compositeThread->platformThread().postTask(FROM_HERE, WTF::bind(&LayerTreeHost::onApplyActionsInCompositeThread, this, needCheck));
}

void LayerTreeHost::onApplyActionsInCompositeThread(bool needCheck)
{
    atomicDecrement(&m_requestApplyActionsCount);
    applyActions(needCheck);
    atomicDecrement(&m_requestApplyActionsFinishCount);
}

void LayerTreeHost::clearCanvas(SkCanvas* canvas, const SkRect& rect, bool useLayeredBuffer)
{
    // When using transparency mode clear the rectangle before painting.
    SkPaint clearPaint;
    if (useLayeredBuffer) {
        clearPaint.setARGB(0, 0xFF, 0xFF, 0xFF);
        clearPaint.setXfermodeMode(SkXfermode::kClear_Mode);
    } else {
        clearPaint.setARGB(0xFF, 0xFF, 0xFF, 0xFF);
        clearPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
    }

    SkRect skrc;
    skrc.set(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());
    canvas->drawRect(skrc, clearPaint);
}

void LayerTreeHost::setDrawMinInterval(double drawMinInterval)
{
    m_drawMinInterval = drawMinInterval;
}

void LayerTreeHost::postPaintMessage(const SkRect& paintRect)
{
    SkRect dirtyRect = paintRect;

    m_compositeMutex.lock();

    SkRect clientRect = SkRect::MakeXYWH(m_clientRect.x(), m_clientRect.y(), m_clientRect.width(), m_clientRect.height());
    if (paintRect.isEmpty() || !dirtyRect.intersect(clientRect)) {
        m_compositeMutex.unlock();
        return;
    }

//     m_dirtyRectsForComposite.append(dirtyRect);
//     mergeDirty(&m_dirtyRectsForComposite);
    addAndMergeDirty(&m_dirtyRectsForComposite, dirtyRect);
    m_compositeMutex.unlock();
}

void LayerTreeHost::firePaintEvent(HDC hdc, const RECT* paintRect)
{
    m_compositeMutex.lock();
    if (!m_memoryCanvas || m_clientRect.isEmpty()) {
        m_compositeMutex.unlock();
        return;
    }
    m_compositeMutex.unlock();

#if 0
    HPEN hpen = CreatePen(PS_SOLID, 10, RGB(11, 22, 33));
    HBRUSH hbrush = CreateSolidBrush(RGB(0xf3, 22, 33));

    SelectObject(hdc, hpen);
    SelectObject(hdc, hbrush);

    Rectangle(hdc, paintRect->left, paintRect->top, paintRect->right, paintRect->bottom);

    DeleteObject(hpen);
    DeleteObject(hbrush);
#endif

    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);

    if (!m_hasTransparentBackground)
        skia::DrawToNativeContext(m_memoryCanvas, hdc, paintRect->left, paintRect->top, paintRect);
    else {
        RECT rc = blink::intRectToWinRect(m_clientRect);
        skia::DrawToNativeLayeredContext(m_memoryCanvas, hdc, paintRect, &rc);
    }
}

void LayerTreeHost::drawFrameInCompositeThread()
{
    m_compositeMutex.lock();
    if (1 != m_drawFrameCount)
        DebugBreak();
    atomicDecrement(&m_drawFrameCount);
    m_compositeMutex.unlock();

    double lastCompositeTime = WTF::monotonicallyIncreasingTime();
    double detTime = lastCompositeTime - m_lastCompositeTime;
    if (detTime < m_drawMinInterval && !m_isDestroying) { // 如果刷新频率太快，缓缓再画
        requestDrawFrameToRunIntoCompositeThread();
        atomicDecrement(&m_drawFrameFinishCount);
        return;
    }
    m_lastCompositeTime = lastCompositeTime;

#if 0
    LARGE_INTEGER performanceCount = { 0 };
    QueryPerformanceCounter(&performanceCount);
    static DWORD gLastCount = 0;
    WTF::String outstr = String::format("LayerTreeHost::drawFrameInCompositeThread:[%d]\n", performanceCount.LowPart - gLastCount);
    OutputDebugStringA(outstr.utf8().data());
    gLastCount = performanceCount.LowPart;
#endif

    bool frameReady = preDrawFrame(); // 这里也会发起Commit
    if (!frameReady) {
        ASSERT(!m_isDestroying);
        requestDrawFrameToRunIntoCompositeThread();
        atomicDecrement(&m_drawFrameFinishCount);
        return;
    }

    m_compositeMutex.lock();
    Vector<SkRect> dirtyRects = m_dirtyRectsForComposite;
    m_dirtyRectsForComposite.clear();
    m_compositeMutex.unlock();

    for (size_t i = 0; i < dirtyRects.size() && !m_isDestroying; ++i) {
        const SkRect& r = dirtyRects[i];
        paintToMemoryCanvasInCompositeThread(r);
    }

    postDrawFrame();
    atomicDecrement(&m_drawFrameFinishCount);
}

void LayerTreeHost::paintToMemoryCanvasInUiThread(const SkRect& paintRect)
{
    if (!m_uiThreadClient)
        return;

//     DWORD nowTime = (DWORD)(WTF::currentTimeMS() * 100);
//     DWORD detTime = nowTime - g_nowTime;
//     InterlockedExchange((LONG*)&g_nowTime, nowTime);

//     String out = String::format("LayerTreeHost.paintToMemoryCanvasInUiThread: %d\n", detTime);
//     OutputDebugStringA(out.utf8().data());

    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    blink::IntRect intPaintRect(paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height());
    m_uiThreadClient->paintToMemoryCanvasInUiThread(m_memoryCanvas, intPaintRect);
}

void LayerTreeHost::WrapSelfForUiThread::endPaint() {
    m_host->m_compositeMutex.lock();
    m_host->m_wrapSelfForUiThreads.remove(this);
    m_host->m_compositeMutex.unlock();

    int* paintToMemoryCanvasInUiThreadTaskCount = &m_host->m_paintToMemoryCanvasInUiThreadTaskCount;

    delete this;

    atomicDecrement(paintToMemoryCanvasInUiThreadTaskCount);
}

void LayerTreeHost::WrapSelfForUiThread::paintInUiThread()
{
    if (!m_host) {
        delete this;
        return;
    }

    double lastPaintTime = WTF::monotonicallyIncreasingTime();
    double detTime = lastPaintTime - m_host->m_lastPaintTime;
    if (detTime < m_host->m_drawMinInterval) {
        m_host->requestPaintToMemoryCanvasToUiThread(IntRect());
        endPaint();
        return;
    }
    m_host->m_lastPaintTime = lastPaintTime;

    m_host->m_compositeMutex.lock();
    Vector<SkRect> dirtyRectsForUi = m_host->m_dirtyRectsForUi;
    m_host->m_dirtyRectsForUi.clear();
    m_host->m_compositeMutex.unlock();

    for (size_t i = 0; i < dirtyRectsForUi.size(); ++i) {
        m_host->paintToMemoryCanvasInUiThread(dirtyRectsForUi[i]);
    }

    endPaint();
}

LayerTreeHost::WrapSelfForUiThread::~WrapSelfForUiThread()
{
    
}

void LayerTreeHost::WrapSelfForUiThread::willProcessTask()
{
    Platform::current()->mainThread()->removeTaskObserver(this);
    paintInUiThread();
}

void LayerTreeHost::WrapSelfForUiThread::didProcessTask()
{
    Platform::current()->mainThread()->removeTaskObserver(this);
    paintInUiThread();
}

void LayerTreeHost::requestPaintToMemoryCanvasToUiThread(const SkRect& r)
{
//     char* output = (char*)malloc(0x100);
//     sprintf(output, "LayerTreeHost::requestPaintToMemoryCanvasToUiThread: %f %f\n", r.x(), r.y());
//     OutputDebugStringA(output);
//     free(output);

    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);

    if (r.isEmpty() && m_paintToMemoryCanvasInUiThreadTaskCount > 1)
        return;

    SkRect dirtyRect(r);
    SkRect clientRect = SkRect::MakeXYWH(m_clientRect.x(), m_clientRect.y(), m_clientRect.width(), m_clientRect.height());
    if (!dirtyRect.intersect(clientRect))
        return;

    addAndMergeDirty(&m_dirtyRectsForUi, dirtyRect);

    if (m_paintToMemoryCanvasInUiThreadTaskCount > 30)
        return;

    WrapSelfForUiThread* wrap = new WrapSelfForUiThread(this);
    m_wrapSelfForUiThreads.add(wrap);
    atomicIncrement(&m_paintToMemoryCanvasInUiThreadTaskCount);
    Platform::current()->mainThread()->postTask(FROM_HERE, WTF::bind(&LayerTreeHost::WrapSelfForUiThread::paintInUiThread, wrap)); //addTaskObserver(wrap);
}

void LayerTreeHost::paintToMemoryCanvasInCompositeThread(const SkRect& r)
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    SkRect paintRect = r;

    if ((!m_memoryCanvas || m_hasResize) && !m_clientRect.isEmpty()) {
        m_hasResize = false;
        paintRect = m_clientRect;

        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_memoryCanvas = skia::CreatePlatformCanvas(m_clientRect.width(), m_clientRect.height(), !m_hasTransparentBackground);
        clearCanvas(m_memoryCanvas, m_clientRect, m_hasTransparentBackground);
    }

    SkRect clientRect = SkRect::MakeXYWH(m_clientRect.x(), m_clientRect.y(), m_clientRect.width(), m_clientRect.height());
    if (!paintRect.intersect(clientRect))
        return;

    if (!m_memoryCanvas) {
        ASSERT(false);
        return;
    }

    m_isDrawDirty = true;
    if (m_hasTransparentBackground)
        clearCanvas(m_memoryCanvas, paintRect, m_hasTransparentBackground);

    bool needNotifUi = drawToCanvas(m_memoryCanvas, paintRect); // 绘制脏矩形

    if (!needNotifUi)
        return;

#if ENABLE_WKE == 1
    if (blink::RuntimeEnabledFeatures::updataInOtherThreadEnabled()) {
        blink::IntRect intPaintRect(paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height());
        m_uiThreadClient->paintToMemoryCanvasInUiThread(m_memoryCanvas, intPaintRect);
        return;
    }
#endif

    requestPaintToMemoryCanvasToUiThread(paintRect);
}

SkCanvas* LayerTreeHost::getMemoryCanvasLocked()
{
    m_compositeMutex.lock();
    return m_memoryCanvas;
}

void LayerTreeHost::releaseMemoryCanvasLocked()
{
    m_compositeMutex.unlock();
}

bool LayerTreeHost::isDrawDirty()
{
    bool isDrawDirty;
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    isDrawDirty = m_isDrawDirty;
    return isDrawDirty;
}

void LayerTreeHost::disablePaint()
{
    m_hostClient->disablePaint();
}

void LayerTreeHost::enablePaint()
{
    m_hostClient->enablePaint();
}

LayerTreeHost::BitInfo* LayerTreeHost::getBitBegin()
{
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    if (!m_memoryCanvas)
        return nullptr;

    int width = m_clientRect.width();
    int height = m_clientRect.height();

    DWORD cBytes = width * height * 4;
    SkBaseDevice* device = (SkBaseDevice*)m_memoryCanvas->getTopDevice();
    if (!device)
        return nullptr;

    const SkBitmap& bitmap = device->accessBitmap(false);
    SkCanvas* tempCanvas = nullptr;
    uint32_t* pixels = nullptr;

    if (bitmap.info().width() != width || bitmap.info().height() != height) {
        tempCanvas = skia::CreatePlatformCanvas(width, height, !m_hasTransparentBackground);
        clearCanvas(tempCanvas, m_clientRect, m_hasTransparentBackground);
        tempCanvas->drawBitmap(bitmap, 0, 0, nullptr);
        device = (SkBaseDevice*)tempCanvas->getTopDevice();
        if (!device) {
            delete tempCanvas;
            return nullptr;
        }
        const SkBitmap& tempBitmap = device->accessBitmap(false);
        pixels = tempBitmap.getAddr32(0, 0);
    } else
        pixels = bitmap.getAddr32(0, 0);

    m_compositeMutex.lock();

    BitInfo* bitInfo = new BitInfo();
    bitInfo->pixels = pixels;
    bitInfo->tempCanvas = tempCanvas;
    bitInfo->width = width;
    bitInfo->height = height;
    return bitInfo;
}

void LayerTreeHost::getBitEnd(const BitInfo* bitInfo)
{
    m_isDrawDirty = false;

    if (bitInfo->tempCanvas) {
        delete m_memoryCanvas;
        m_memoryCanvas = bitInfo->tempCanvas;
    }
    m_compositeMutex.unlock();

    delete bitInfo;
}

void LayerTreeHost::paintToBit(void* bits, int pitch)
{
#if 1
    BitInfo* bitInfo = getBitBegin();
    if (!bitInfo)
        return;
    uint32_t* pixels = bitInfo->pixels;;
    int width = bitInfo->width;
    int height = bitInfo->height;

    if (pitch == 0 || pitch == width * 4) {
        memcpy(bits, pixels, width * height * 4);
    } else {
        unsigned char* src = (unsigned char*)pixels;
        unsigned char* dst = (unsigned char*)bits;
        for (int i = 0; i < height; ++i) {
            memcpy(dst, src, width * 4);
            src += width * 4;
            dst += pitch;
        }
    }
    getBitEnd(bitInfo);
#else
    WTF::Locker<WTF::Mutex> locker(m_compositeMutex);
    if (!m_memoryCanvas)
        return;

    int width = m_clientRect.width();
    int height = m_clientRect.height();

    DWORD cBytes = width * height * 4;
    SkBaseDevice* device = (SkBaseDevice*)m_memoryCanvas->getTopDevice();
    if (!device)
        return;

    const SkBitmap& bitmap = device->accessBitmap(false);
    SkCanvas* tempCanvas = nullptr;
    uint32_t* pixels = nullptr;

    if (bitmap.info().width() != width || bitmap.info().height() != height) {
        tempCanvas = skia::CreatePlatformCanvas(width, height, !m_hasTransparentBackground);
        clearCanvas(tempCanvas, m_clientRect, m_hasTransparentBackground);
        tempCanvas->drawBitmap(bitmap, 0, 0, nullptr);
        device = (SkBaseDevice*)tempCanvas->getTopDevice();
        if (!device)
            return;
        const SkBitmap& tempBitmap = device->accessBitmap(false);
        pixels = tempBitmap.getAddr32(0, 0);
    } else
        pixels = bitmap.getAddr32(0, 0);

    if (pitch == 0 || pitch == width * 4) {
        memcpy(bits, pixels, width * height * 4);
    } else {
        unsigned char* src = (unsigned char*)pixels;
        unsigned char* dst = (unsigned char*)bits;
        for (int i = 0; i < height; ++i) {
            memcpy(dst, src, width * 4);
            src += width * 4;
            dst += pitch;
        }
    }

    m_isDrawDirty = false;

    if (tempCanvas) {
        delete m_memoryCanvas;
        m_memoryCanvas = tempCanvas;
    }

#endif
}

} // cc