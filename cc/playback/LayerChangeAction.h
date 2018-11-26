#ifndef cc_LayerChangeAction_h
#define cc_LayerChangeAction_h

#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/skia/include/core/SkRect.h"
#include "cc/blink/WebLayerImplClient.h"

class SkBitmap;

namespace cc_blink {
class WebLayerImpl;
}

namespace cc {

struct DrawToCanvasProperties;
class TileActionInfoVector;
struct TileActionInfo;
class LayerTreeHost;
class SkBitmapRefWrap;

class LayerChangeAction {
public:
    enum ChangeType {
        LayerChangeCreate = 0,
        LayerChangeDestroy = 1,
        LayerChangeAddChild,
        LayerChangeInsertChild,
        LayerChangeReplaceChild,
        LayerChangeRemoveFromParent = 5,
        LayerChangeRemoveAllChildren = 6,
        LayerChangeSortChildren, // TODO
        LayerChangeDrawPropUpdata,
        LayerChangeUpdataImageLayer,
        LayerChangeBlend,
        LayerChangeTileUpdata,
    };

    LayerChangeAction(int64 actionId, ChangeType type);
    virtual ~LayerChangeAction();
    virtual void run(LayerTreeHost* host) = 0;

    void setActionId(int actionId) { m_actionId = actionId; }
    int64 actionId() const { return m_actionId; }
    bool isActionIdEmpty() const { return -1 == m_actionId; }
    ChangeType type() const { return m_type; }

protected:
    int64 m_actionId;
    ChangeType m_type;
};

class LayerChangeOneLayer : public LayerChangeAction {
public:
    LayerChangeOneLayer(int64 actionId, ChangeType type, int layerId)
        : LayerChangeAction(actionId, type)
        , m_layerId(layerId)
    {
    }
    int layerId() const { return m_layerId; }

protected:
    int m_layerId;
};

class LayerChangeActionCreate : public LayerChangeOneLayer {
public:
    LayerChangeActionCreate(int64 actionId, int layerId, cc_blink::WebLayerImplClient::Type layerType);

    virtual void run(LayerTreeHost* host) override;
private:
    cc_blink::WebLayerImplClient::Type m_layerType;
};

class LayerChangeActionDestroy : public LayerChangeOneLayer {
public:
    LayerChangeActionDestroy(int64 actionId, int layerId);

    virtual void run(LayerTreeHost* host) override;
private:
};

class LayerChangeActionAddChild : public LayerChangeOneLayer {
public:
    LayerChangeActionAddChild(int actionId, int layerId, int childId)
        : LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeAddChild, layerId)
    {}

    virtual void run(LayerTreeHost* host) override { DebugBreak(); }

    int childId() const { return m_childId; }
private:
    int m_childId;
};

class LayerChangeActionInsertChild : public LayerChangeOneLayer {
public:
    LayerChangeActionInsertChild(int actionId, int layerId, int childId, size_t index)
        : LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeInsertChild, layerId)
        , m_childId(childId)
        , m_index(index)
    {}

    virtual void run(LayerTreeHost* host) override;

    int childId() const { return m_childId; }
    int index() const { return m_index; }
private:
    int m_childId;
    int m_index;
};

class LayerChangeActionReplaceChild : public LayerChangeOneLayer {
public:
    LayerChangeActionReplaceChild(int actionId, int layerId, int referenceId, size_t newLayerId)
        : LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeReplaceChild, layerId)
        , m_referenceId(referenceId)
        , m_newLayerId(newLayerId)
    {}

    virtual void run(LayerTreeHost* host) override;

    int referenceId() const { return m_referenceId; }
    int newLayerId() const { return m_newLayerId; }
private:
    int m_referenceId;
    int m_newLayerId;
};

class LayerChangeActionRemoveFromParent : public LayerChangeOneLayer {
public:
    LayerChangeActionRemoveFromParent(int actionId, int layerId);

    virtual void run(LayerTreeHost* host) override;
};

class LayerChangeActionRemoveAllChildren : public LayerChangeOneLayer {
public:
    LayerChangeActionRemoveAllChildren(int actionId, int layerId)
        : LayerChangeOneLayer(actionId, LayerChangeAction::LayerChangeRemoveAllChildren, layerId)
    {}

    void run(LayerTreeHost* host);
};

class LayerChangeActionDrawPropUpdata : public LayerChangeAction {
public:
    LayerChangeActionDrawPropUpdata();
    virtual ~LayerChangeActionDrawPropUpdata() override;

    void appendDirtyLayer(cc_blink::WebLayerImpl* layer);
    void appendPendingInvalidateRect(const SkRect& r);
    void cleanupPendingInvalidateRectIfHasAlendAction();
    const WTF::Vector<SkRect>& dirtyRects() const;

    bool isDirtyLayerEmpty() const { return 0 == m_layerIds.size(); }

    void run(LayerTreeHost* host);

private:
    WTF::Vector<int> m_layerIds;
    WTF::Vector<DrawToCanvasProperties*> m_props;
    WTF::Vector<SkRect> m_pendingInvalidateRects;
};

class LayerChangeActionUpdataImageLayer : public LayerChangeOneLayer {
public:
    LayerChangeActionUpdataImageLayer(int actionId, int layerId, SkBitmapRefWrap* bitmap);
    virtual ~LayerChangeActionUpdataImageLayer() override;

    void run(LayerTreeHost* host);

private:
    SkBitmapRefWrap* m_bitmap;
};

class LayerChangeActionBlend : public LayerChangeAction {
public:
    LayerChangeActionBlend(int actionId, int layerId, TileActionInfoVector* willRasteredTiles, const SkRect& dirtyRect);
    virtual ~LayerChangeActionBlend() override;

    void setContentScale(float contentScale) { m_contentScale = contentScale; }
    float getContentScale() const { return m_contentScale; }

    void run(LayerTreeHost* host);
    void setDirtyRectBitmap(SkBitmap* bitmap);
    void appendPendingInvalidateRect(const SkRect& r);
    void appendPendingInvalidateRects(const WTF::Vector<SkRect>& rects);

    int getLayerId() const { return m_layerId; }

    const TileActionInfoVector* getWillRasteredTiles() const { return m_willRasteredTiles; }

private:
    int m_layerId;
    TileActionInfoVector* m_willRasteredTiles;
    SkRect m_dirtyRect;
    SkBitmap* m_dirtyRectBitmap;
    float m_contentScale;

    WTF::Vector<SkRect> m_pendingInvalidateRects;
};

class LayerChangeActionUpdataTile : public LayerChangeOneLayer {
public:
    LayerChangeActionUpdataTile(int actionId, int layerId, int newIndexNumX, int newIndexNumY, DrawToCanvasProperties* prop);
    virtual void run(LayerTreeHost* host) override;

    int newIndexNumX() { return m_newIndexNumX; }
    int newIndexNumY() { return m_newIndexNumY; }

private:
    int m_newIndexNumX;
    int m_newIndexNumY;
    DrawToCanvasProperties* m_prop;
};

class LayerChangeActionCleanupUnnecessaryTile : public LayerChangeOneLayer {
public:
    LayerChangeActionCleanupUnnecessaryTile(int layerId);
    ~LayerChangeActionCleanupUnnecessaryTile();

    void appendTile(int index, int xIndex, int yIndex);

    virtual void run(LayerTreeHost* host) override;

    bool isEmpty() const { return m_tiles.isEmpty(); }

private:
    void clear();

    WTF::Vector<TileActionInfo*> m_tiles;
};

}

#endif // cc_LayerChangeAction_h
