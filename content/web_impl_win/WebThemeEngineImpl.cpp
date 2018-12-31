#include "config.h"
#include <windows.h>

#include "platform/geometry/IntRect.h"
#include "ui/gfx/win/dpi.h"
#include "WebThemeEngineImpl.h"
#if 0
#include "NativeThemeWin.h"
#endif
#include "content/ui/CustomTheme.h"

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
    return m_theme->GetPartSize(part);
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