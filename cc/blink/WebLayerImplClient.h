#ifndef WebLayerImplClient_h
#define WebLayerImplClient_h

#include "third_party/WebKit/public/platform/WebCanvas.h"

namespace cc {
class RasterTaskGroup;
}

namespace blink {
class IntRect;
}

namespace cc_blink {

class WebLayerImplClient {
public:
    enum Type {
        LayerType,
        ContentLayerType,
        ScrollbarLayerType,
        ImageLayerType,
    };
    virtual void updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip) = 0;
    virtual void recordDraw(cc::RasterTaskGroup* taskGroup) = 0;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) = 0;
    virtual Type type() const = 0;
};

} // cc_blink

#endif // WebLayerImplClient_h