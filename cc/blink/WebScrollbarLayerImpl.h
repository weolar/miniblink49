
#ifndef WebScrollbarLayerImpl_h
#define WebScrollbarLayerImpl_h

#include "WebKit/public/platform/WebScrollbarLayer.h"
#include "WebKit/public/platform/WebScrollbar.h"
#include "WebKit/public/platform/WebScrollbarThemePainter.h"
#include "WebKit/public/platform/WebScrollbarThemeGeometry.h"
#include "cc/blink/WebLayerImplClient.h"

namespace cc {
class RecordDrawHelp;
}

namespace cc_blink {
class WebLayerImpl;
class WebScrollbarContentLayerClient;

class WebScrollbarLayerImpl : public blink::WebScrollbarLayer, public cc_blink::WebLayerImplClient {
public:
    WebScrollbarLayerImpl(
        blink::WebScrollbar* scrollbar,
        blink::WebScrollbarThemePainter painter,
        blink::WebScrollbarThemeGeometry* geometry);
    WebScrollbarLayerImpl(
        blink::WebScrollbar::Orientation orientation,
        int thumb_thickness,
        int track_start,
        bool is_left_side_vertical_scrollbar);
    ~WebScrollbarLayerImpl() override;

    // blink::WebScrollbarLayer implementation.
    blink::WebLayer* layer() override;
    virtual void setScrollLayer(blink::WebLayer* layer) override;
    virtual void setClipLayer(blink::WebLayer*) override;

    // WebLayerImplClient
    virtual void updataAndPaintContents(blink::WebCanvas* canvas, const blink::IntRect& clip) override;
    virtual WebLayerImplClient::Type type() const override{ return ScrollbarLayerType; }
    virtual void recordDraw(cc::RasterTaskGroup* taskGroup) override;
    virtual void drawToCanvas(blink::WebCanvas* canvas, const blink::IntRect& clip) override;
private:
    bool isPosChange();

    WebLayerImpl* m_layer;
    int m_scrollLayerId;
    int m_clipLayerId;

    blink::WebScrollbar* m_scrollbar;
    blink::WebScrollbarThemePainter m_painter;
    blink::WebScrollbarThemeGeometry* m_geometry;

    int m_visibleSize;
    int m_value;

    cc::RecordDrawHelp* m_recordDrawHelp;

    blink::IntRect m_scrollbarPos;

	WebScrollbarContentLayerClient* m_client;

    DISALLOW_COPY_AND_ASSIGN(WebScrollbarLayerImpl);
};
} // cc_blink

#endif // WebScrollbarLayerImpl_h