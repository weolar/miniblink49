
#ifndef WebImageLayerImpl_h
#define WebImageLayerImpl_h
#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/public/platform/WebImageLayer.h"
#include "cc/blink/WebLayerImplClient.h"

namespace blink {

}

namespace blink {
class SkBitmapRefWrap;
}

namespace cc {
class SkBitmapRefWrap;
}

class SkBitmapRefWrap;

namespace cc_blink {

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
    virtual void recordDraw(cc::RasterTaskGroup* taskGroup) OVERRIDE;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) OVERRIDE;

private:
    WebLayerImpl* m_layer;
	cc::SkBitmapRefWrap* m_bitmap;
	//SkBitmap* m_bitmap;
};

} // cc_blink

#endif // WebImageLayerImpl_h