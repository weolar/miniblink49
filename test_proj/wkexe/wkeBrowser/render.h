#ifndef RENDER_H
#define RENDER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wke.h>

class CRender
{
public:
    enum RenderType
    {
        GDI_RENDER,
        D3D_RENDER,
    };
    static CRender* create(RenderType type);

    virtual bool init(HWND hView) = 0;
    virtual void destroy() = 0;
    virtual void resize(unsigned int w, unsigned int h) = 0;
    virtual void render(wkeWebView webView) = 0;
    virtual HDC dc() { return 0; }
    virtual unsigned int width() { return 0; }
    virtual unsigned int height() { return 0; }
};

#endif