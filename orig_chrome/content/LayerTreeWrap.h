
#ifndef orig_chrome_content_LayerTreeWrap_h
#define orig_chrome_content_LayerTreeWrap_h

#include "content/WebPageOcBridge.h"

namespace blink {
class WebLayerTreeView;
}

namespace content {

class RenderWidgetCompositor;

class LayerTreeWrap {
public:
    LayerTreeWrap(WebPageOcBridge* webPageOcBridge, bool isUiThreadIsolate);
    ~LayerTreeWrap();

    void setHWND(HWND hWnd);
    void onHostResized(int width, int height);
    void firePaintEvent(HDC hdc, const RECT& paintRect);
    HDC getHdcLocked();
    void releaseHdc();
    void initializeLayerTreeView();
    blink::WebLayerTreeView* layerTreeView();

private:
    RenderWidgetCompositor* m_renderWidgetCompositor;
};

}

#endif // orig_chrome_content_LayerTreeWrap_h