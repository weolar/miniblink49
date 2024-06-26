
#ifndef orig_chrome_content_WebPageOcBridge_h
#define orig_chrome_content_WebPageOcBridge_h

#include <windows.h>

namespace content {

class WebPageOcBridge {
public:
    virtual void onBeginPaint(HDC hdc, const RECT& damageRect) = 0;
    virtual bool onEndPaintStep1(HDC hdc, const RECT& damageRect) = 0;
    virtual void onEndPaintStep2(HDC hdc, const RECT& damageRect) = 0;
    virtual void onLayout() = 0;
    virtual void onBeginMainFrame() = 0;
    virtual COLORREF getBackgroundColor() = 0;
};
}

#endif