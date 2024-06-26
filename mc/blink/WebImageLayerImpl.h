
#ifndef mc_WebImageLayerImpl_h
#define mc_WebImageLayerImpl_h

#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/public/platform/WebImageLayer.h"
#include "mc/blink/WebLayerImplClient.h"

namespace blink {
class SkBitmapRefWrap;
}

namespace mc {
class SkBitmapRefWrap;
}

class SkBitmapRefWrap;

namespace mc_blink {

class WebLayerImpl;

class WebImageLayerImpl : public blink::WebImageLayer, public WebLayerImplClient {
public:
    WebImageLayerImpl();
    virtual ~WebImageLayerImpl() OVERRIDE;

    virtual blink::WebLayer* layer() OVERRIDE;
    virtual void setImageBitmap(const SkBitmap&) OVERRIDE;
    virtual void setNearestNeighbor(bool) OVERRIDE;

    // WebLayerImplClient
    virtual void updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip) OVERRIDE;
    virtual Type type() const OVERRIDE;
    virtual void recordDraw(mc::RasterTaskGroup* taskGroup) OVERRIDE;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) OVERRIDE;

private:
    WebLayerImpl* m_layer;
	mc::SkBitmapRefWrap* m_bitmap;
};

} // mc_blink

#endif // WebImageLayerImpl_h