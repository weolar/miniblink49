
#ifndef orig_chrome_content_compositor_SoftwareOutputDevice_h
#define orig_chrome_content_compositor_SoftwareOutputDevice_h

#include "cc/output/software_output_device.h"

class SkCanvas;

namespace content {

class WebPageOcBridge;

class SoftwareOutputDevice : public cc::SoftwareOutputDevice {
public:
    SoftwareOutputDevice(WebPageOcBridge* webPageOcBridge);
    ~SoftwareOutputDevice();

    void setHWND(HWND hWnd);
    HDC getHdcLocked();
    void releaseHdc();
    void firePaintEvent(HDC hdc, const RECT& paintRect);

    virtual void Resize(const gfx::Size& viewportPixelSize, float scalefactor) override;
    virtual SkCanvas* BeginPaint(const gfx::Rect& damage_rect) override;
    virtual void EndPaint() override;

private:
    WebPageOcBridge* m_webPageOcBridge;
    SkCanvas* m_memoryCanvas;
    HWND m_hWnd;
    CRITICAL_SECTION m_memoryCanvasLock;
};

}

#endif // orig_chrome_content_compositor_BrowserSoftwareOutputDevice_h