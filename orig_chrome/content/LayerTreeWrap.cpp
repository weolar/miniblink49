
#include "content/LayerTreeWrap.h"
#include "content/RenderWidgetCompositor.h"

namespace content {

LayerTreeWrap::LayerTreeWrap(WebPageOcBridge* webPageOcBridge, bool isUiThreadIsolate)
{
    m_renderWidgetCompositor = new RenderWidgetCompositor(webPageOcBridge, isUiThreadIsolate);
}

LayerTreeWrap::~LayerTreeWrap()
{
    delete m_renderWidgetCompositor;
}

void LayerTreeWrap::setHWND(HWND hWnd)
{
    m_renderWidgetCompositor->setHWND(hWnd);
}

void LayerTreeWrap::onHostResized(int width, int height)
{
    m_renderWidgetCompositor->onHostResized(width, height);
}

void LayerTreeWrap::firePaintEvent(HDC hdc, const RECT& paintRect)
{
    m_renderWidgetCompositor->firePaintEvent(hdc, paintRect);
}

HDC LayerTreeWrap::getHdcLocked()
{
    return m_renderWidgetCompositor->getHdcLocked();
}

void LayerTreeWrap::releaseHdc()
{
    m_renderWidgetCompositor->releaseHdc();
}

void LayerTreeWrap::initializeLayerTreeView()
{
    m_renderWidgetCompositor->initializeLayerTreeView();
}

blink::WebLayerTreeView* LayerTreeWrap::layerTreeView()
{
    return m_renderWidgetCompositor;
}

}