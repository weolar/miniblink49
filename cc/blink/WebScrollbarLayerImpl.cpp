
#include "cc/blink/WebScrollbarLayerImpl.h"
#include "cc/blink/WebLayerImpl.h"

#include "cc/raster/RasterResouce.h"
#include "cc/trees/LayerTreeHost.h"
#include "cc/tiles/TileGrid.h"

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"
#include "third_party/WebKit/public/platform/WebFloatSize.h"
#include "third_party/WebKit/Source/platform/scroll/ScrollbarTheme.h"
#include "third_party/WebKit/Source/platform/scroll/Scrollbar.h"

namespace cc_blink {

#ifdef _DEBUG
static int gWebScrollbarLayerImplCount = 0;
#endif

class WebScrollbarContentLayerClient : public blink::WebContentLayerClient {
public:
    WebScrollbarContentLayerClient(WebScrollbarLayerImpl* owner)
        : m_owner(owner)
    {
    }

    virtual void paintContents(blink::WebCanvas* canvas, const blink::WebRect& clip, blink::WebContentLayerClient::PaintingControlSetting) override
    {
        m_owner->drawToCanvas(canvas, clip);
    }

    virtual void paintContents(blink::WebDisplayItemList*, const blink::WebRect& clip, blink::WebContentLayerClient::PaintingControlSetting) override
    {
    }

    virtual ~WebScrollbarContentLayerClient() override { }

protected:
    
    WebScrollbarLayerImpl* m_owner;
};

WebScrollbarLayerImpl::WebScrollbarLayerImpl(
    blink::WebScrollbar* scrollbar,
    blink::WebScrollbarThemePainter painter,
    blink::WebScrollbarThemeGeometry* geometry)
    : m_layer(new WebLayerImpl(this))
    , m_scrollLayerId(-1)
    , m_clipLayerId(-1)
    , m_client(new WebScrollbarContentLayerClient(this))
{
    m_scrollbar = scrollbar;
    m_painter = painter;
    m_geometry = geometry;
    m_recordDrawHelp = nullptr;
    m_visibleSize = 0;
    m_value = 0;
}

WebScrollbarLayerImpl::WebScrollbarLayerImpl(
    blink::WebScrollbar::Orientation orientation,
    int thumb_thickness,
    int track_start,
    bool is_left_side_vertical_scrollbar)
    : m_layer(new WebLayerImpl(this))
    , m_scrollLayerId(-1)
    , m_client(new WebScrollbarContentLayerClient(this))
{
    m_visibleSize = 0;
    m_value = 0;
    m_recordDrawHelp = nullptr;
    m_scrollbar = nullptr;
    m_geometry = nullptr;
}

WebScrollbarLayerImpl::~WebScrollbarLayerImpl()
{
    m_layer->removeFromParent();
    delete m_layer;
    delete m_client;

    if (m_scrollbar)
        delete m_scrollbar;
    m_scrollbar = nullptr;

    if (m_geometry)
        delete m_geometry;
    m_geometry = nullptr;
}

blink::WebLayer* WebScrollbarLayerImpl::layer() 
{
    return m_layer;
}

bool WebScrollbarLayerImpl::isPosChange()
{
    const blink::Scrollbar* scrollbar = m_painter.scrollbar();
    if (!scrollbar)
        return false;
    int x = scrollbar->x();
    int y = scrollbar->y();

    int width = scrollbar->width();
    int height = scrollbar->height();
    blink::IntRect scrollbarPos(x, y, width, height);
    return m_scrollbarPos != scrollbarPos;
}

void WebScrollbarLayerImpl::setScrollLayer(blink::WebLayer* layer) 
{
    if (isPosChange())
        m_layer->requestBoundRepaint(false);

    if (m_scrollLayerId == layer->id())
        return;
    m_scrollLayerId = layer->id();
}

void WebScrollbarLayerImpl::setClipLayer(blink::WebLayer* layer)
{
    if (isPosChange())
        m_layer->requestBoundRepaint(false);

    if (m_clipLayerId == layer->id())
        return;
    m_clipLayerId = layer->id();
}

void WebScrollbarLayerImpl::updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (!m_painter.theme())
        return;

    m_value = m_scrollbar->value();

    int scrollbarX = m_painter.scrollbar()->x();
    int scrollbarY = m_painter.scrollbar()->y();

    int scrollbarWidth = m_painter.scrollbar()->width();
    int scrollbarHeight = m_painter.scrollbar()->height();

    canvas->save();
    //canvas->clipRect((SkRect)clip); // add
    canvas->translate(-scrollbarX, -scrollbarY);

    blink::IntRect clipInRootCoordinate = clip;
    clipInRootCoordinate.move(scrollbarX, scrollbarY);
    clipInRootCoordinate.setWidth(scrollbarWidth);
    clipInRootCoordinate.setHeight(scrollbarHeight);

    SkPaint paint;
    paint.setColor(0xffffffff);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas->drawRect(clipInRootCoordinate, paint);

    PassOwnPtr<blink::GraphicsContext> graphicsContext = blink::GraphicsContext::deprecatedCreateWithCanvas(canvas);
    m_painter.theme()->paint(m_painter.scrollbar(), graphicsContext.get(), clipInRootCoordinate);

    canvas->restore();
}

void WebScrollbarLayerImpl::recordDraw(cc::RasterTaskGroup* taskGroup)
{
    if (!m_layer->tileGrid())
        return;

    cc::LayerTreeHost* layerTreeHost = m_layer->layerTreeHost();
    if (layerTreeHost && !layerTreeHost->needTileRender())
        return;

    blink::IntRect screenRect = blink::IntRect(blink::IntPoint(), layerTreeHost->deviceViewportSize());
    screenRect = m_layer->mapRectFromRootLayerCoordinateToCurrentLayer(screenRect);

    blink::IntSize bounds = m_layer->bounds();
    m_layer->tileGrid()->update(m_client, taskGroup, bounds, screenRect);
}

void WebScrollbarLayerImpl::drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
    if (!m_painter.theme())
        return;

    m_visibleSize = m_painter.scrollbar()->visibleSize();
    int value = m_scrollbar->value();

    m_value = value;

    int scrollbarX = m_painter.scrollbar()->x();
    int scrollbarY = m_painter.scrollbar()->y();
    int scrollbarWidth = m_painter.scrollbar()->width();
    int scrollbarHeight = m_painter.scrollbar()->height();

    canvas->save();
    canvas->translate(-scrollbarX, -scrollbarY);

    blink::IntRect clipInRootCoordinate;
    clipInRootCoordinate.setX(scrollbarX);
    clipInRootCoordinate.setY(scrollbarY);
    //clipInRootCoordinate.move(scrollbarX, scrollbarY);
    clipInRootCoordinate.setWidth(scrollbarWidth);
    clipInRootCoordinate.setHeight(scrollbarHeight);

    SkPaint paint;
    paint.setColor(0xffffffff);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas->drawRect(clipInRootCoordinate, paint);

//     Vector<WCHAR> msg;
//     msg.resize(100);
//     wsprintfW(msg.data(), L"WebScrollbarLayerImpl::updataAndPaintContents:%p %p, %d %d %d %d - %d %d %d %d\n",
//         this, m_layer, clipInRootCoordinate.x(), clipInRootCoordinate.y(), clipInRootCoordinate.width(), clipInRootCoordinate.height(),
//         scrollbarX, scrollbarY, clip.x(), clip.y());
//     OutputDebugStringW(msg.data());

    // theme::paint里因为调用了DC的东西，不能被canvas所translate，所以手动绘制到根坐标系，也就是窗口的最边上
    
    PassOwnPtr<blink::GraphicsContext> graphicsContext = blink::GraphicsContext::deprecatedCreateWithCanvas(canvas);
    m_painter.theme()->paint(m_painter.scrollbar(), graphicsContext.get(), clipInRootCoordinate);

    canvas->restore();
}

} // cc_blink