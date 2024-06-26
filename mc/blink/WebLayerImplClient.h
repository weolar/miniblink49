#ifndef mc_WebLayerImplClient_h
#define mc_WebLayerImplClient_h

#include "third_party/WebKit/public/platform/WebCanvas.h"

namespace mc {
class RasterTaskGroup;
}

namespace blink {
class IntRect;
}

namespace mc_blink {

class WebLayerImplClient {
public:
    enum Type {
        LayerType,
        ContentLayerType,
        ScrollbarLayerType,
        ImageLayerType,
    };
    virtual void updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip) = 0;
    virtual void recordDraw(mc::RasterTaskGroup* taskGroup) = 0;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) = 0;
    virtual Type type() const = 0;
};

} // mc_blink

#endif // WebLayerImplClient_h