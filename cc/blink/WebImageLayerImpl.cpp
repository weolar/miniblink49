
#include "WebImageLayerImpl.h"
#include "third_party/WebKit/public/platform/WebImageLayer.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "cc/blink/WebLayerImpl.h"
#include "cc/raster/SkBitmapRefWrap.h"
#include "cc/raster/RasterTask.h"

namespace cc_blink {

WebImageLayerImpl::WebImageLayerImpl()
    : m_bitmap(new cc::SkBitmapRefWrap())
{
    m_bitmap->ref();
    m_layer = new WebLayerImpl(this);
    m_layer->setDrawsContent(true);
}

WebImageLayerImpl::~WebImageLayerImpl()
{
    m_bitmap->deref();
    m_bitmap = nullptr;

    //m_layer->parent()->replaceChild(m_layer, nullptr);
    m_layer->removeFromParent();
    m_layer->setParent(nullptr);
    delete m_layer;
}

WebLayerImplClient::Type WebImageLayerImpl::type() const
{
    return WebLayerImplClient::ImageLayerType;
}

blink::WebLayer* WebImageLayerImpl::layer()
{
    return m_layer;
}

void WebImageLayerImpl::setImageBitmap(const SkBitmap& bitmap)
{
    if (!m_bitmap || (m_bitmap->get() && bitmap.pixelRef() && bitmap.pixelRef() == m_bitmap->get()->pixelRef()))
        return;

    m_bitmap->deref();

    SkBitmap* bitmapCopy = new SkBitmap(bitmap);

    m_bitmap = new cc::SkBitmapRefWrap();
    m_bitmap->set(bitmapCopy);
    m_bitmap->ref();

    //m_layer->setNeedsFullTreeSync();
}

void WebImageLayerImpl::updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip)
{

}

void WebImageLayerImpl::recordDraw(cc::RasterTaskGroup* taskGroup)
{
    if (m_bitmap->get()) {
        taskGroup->postImageLayerAction(m_layer->id(), m_bitmap);
    }
}

void WebImageLayerImpl::drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip)
{
//     if (m_bitmap)
//         canvas->drawBitmap(*m_bitmap, 0, 0, nullptr);
}

void WebImageLayerImpl::setNearestNeighbor(bool)
{

}

} // cc_blink