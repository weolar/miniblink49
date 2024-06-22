#include "config.h"
#include <windows.h>

#include "content/web_impl_win/WebThemeEngineImpl.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#if 0
#include "content/web_impl_win/NativeThemeWin.h"
#endif
#include "content/ui/CustomTheme.h"
#include "platform/geometry/IntRect.h"
#include "ui/gfx/win/dpi.h"

namespace content {

WebThemeEngineImpl::WebThemeEngineImpl()
{
    //m_nativeThemeWin = new NativeThemeWin();
    m_theme = new CustomTheme();
}

WebThemeEngineImpl::~WebThemeEngineImpl()
{
    //delete m_nativeThemeWin;
    delete m_theme;
}

blink::WebSize WebThemeEngineImpl::getSize(blink::WebThemeEngine::Part part)
{
    //return m_nativeThemeWin->getSize(part);
    blink::IntSize size = m_theme->GetPartSize(part);

    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    float zoom = platform->getZoom();
    if (zoom > 0 && zoom < 3)
        size.scale(zoom);
    return size;
}

void WebThemeEngineImpl::paint(
    blink::WebCanvas* canvas, 
    blink::WebThemeEngine::Part part, 
    blink::WebThemeEngine::State state, 
    const blink::WebRect& rect, 
    const blink::WebThemeEngine::ExtraParams* extra)
{
    //m_nativeThemeWin->PaintDirect(canvas, part, state, rect, extra);
    m_theme->Paint(canvas, part, state, rect, *extra);
}


} // namespace content