#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#define BUILDING_wke

#include "content/browser/WebPage.h"

//cexer: 必须包含在后面，因为其中的 wke.h -> windows.h 会定义 max、min，导致 blink 内部的 max、min 出现错乱。
#include "wkeWebView.h"
#include "wkeJsBind.h"

#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/browser/WebFrameClientImpl.h"

#define PURE = 0;
#include <shlwapi.h>

namespace wke {

CWebView::CWebView()
    : m_hWnd(NULL)
    , m_transparent(false)
    , m_width(0)
    , m_height(0)
    , m_awake(true)
    , m_title("", 0)
    , m_cookie("", 0)
    , m_name("", 0)
{
    _initPage();
    _initHandler();
    _initMemoryDC();
	m_webPage->wkeHandler().isWke = true;
}

CWebView::~CWebView()
{
    m_webPage->close();
}

bool CWebView::create()
{
    return true;
}

void CWebView::destroy()
{
    delete this;
}

const utf8* CWebView::name() const
{
    return m_name.string();
}

const wchar_t* CWebView::nameW() const
{
    return m_name.stringW();
}

void CWebView::setName(const utf8* name)
{
    m_name.setString(name);
}
void CWebView::setName(const wchar_t* name)
{
    m_name.setString(name);
}

bool CWebView::isTransparent() const
{
    return m_transparent;
}

void CWebView::setTransparent(bool transparent)
{
    if (m_transparent == transparent)
        return;

//     m_transparent = transparent;
//     m_dirtyArea = blink::IntRect(0, 0, m_width, m_height);
//     setDirty(true);
// 
//     if (m_graphicsContext) {
//         delete m_graphicsContext;
//         m_graphicsContext = NULL;
//     }
// 
//     blink::Color backgroundColor = transparent ? blink::Color::transparent : blink::Color::white;
//     m_mainFrame->view()->updateBackgroundRecursively(backgroundColor, transparent);
}

void CWebView::loadPostURL(const utf8* inUrl, const char * poastData, int nLen )
{
    blink::KURL url(blink::ParsedURLString, inUrl);
	if (!url.isValid())
		url.setProtocol("http:");

	if (!url.isValid())
		return;

	if (blink::protocolIsJavaScript(url)) {
		//m_mainFrame->script()->executeIfJavaScriptURL(url);
		return;
	}

    blink::WebURLRequest request(url);
    request.setCachePolicy(blink::WebURLRequest::UseProtocolCachePolicy);
    request.setHTTPMethod(blink::WebString::fromUTF8("POST"));

    blink::WebHTTPBody body;
    body.appendData(blink::WebData(poastData, nLen));
    request.setHTTPBody(body);
    m_webPage->loadRequest(content::WebPage::kMainFrameId, request);
}

void CWebView::loadPostURL(const wchar_t * inUrl,const char * poastData,int nLen )
{
   loadPostURL(String(inUrl).utf8().data(),poastData,nLen);
}

void CWebView::loadURL(const utf8* inUrl)
{
    //cexer 必须调用String::fromUTF8显示构造第二个参数，否则String::String会把inUrl当作latin1处理。
    blink::KURL url(blink::ParsedURLString, inUrl);
    if (!url.isValid())
        url.setProtocol("http:");

    if (!url.isValid())
        return;

    if (blink::protocolIsJavaScript(url)) {
        //m_mainFrame->script()->executeIfJavaScriptURL(url);
        return;
    }

    blink::WebURLRequest request(url);
    request.setCachePolicy(blink::WebURLRequest::UseProtocolCachePolicy);
    request.setHTTPMethod(blink::WebString::fromUTF8("GET"));
    m_webPage->loadRequest(content::WebPage::kMainFrameId, request);
}

void CWebView::loadURL(const wchar_t* url)
{
    loadURL(String(url).utf8().data());
}

void CWebView::loadHTML(const utf8* html)
{
    size_t length = strlen(html);
    if (0 == length)
        return;
    String url = String::format("MemoryURL://data.com/%d", GetTickCount());
    m_webPage->loadHTMLString(content::WebPage::kMainFrameId, blink::WebData(html, length), blink::KURL(blink::ParsedURLString, url), blink::WebURL(), true);
}

void CWebView::loadHTML(const wchar_t* html)
{
    size_t length = wcslen(html);
    if (0 == length)
        return;
    String htmlUTF8((UChar*)html, length);
    Vector<char> htmlUTF8Buf = WTF::ensureStringToUTF8(htmlUTF8);

    String url = String::format("MemoryURL://data.com/%d", GetTickCount());
    m_webPage->loadHTMLString(content::WebPage::kMainFrameId, blink::WebData(htmlUTF8Buf.data(), htmlUTF8Buf.size()), blink::KURL(blink::ParsedURLString, url), blink::WebURL(), true);
}

void CWebView::loadFile(const utf8* filename)
{
    if (!filename)
        return;
    size_t length = strlen(filename);
    if (length < 4)
        return;

    String filenameUTF8(filename, length);
    loadFile(ensureUTF16UChar(filenameUTF8).data());
}

void CWebView::loadFile(const wchar_t* filename)
{
    if (!filename)
        return;
    size_t length = wcslen(filename);
    if (length < 4)
        return;

    String filenameUTF8(filename, length);

    Vector<WCHAR> filenameBuffer;
    filenameBuffer.resize(MAX_PATH + 1);
    if (filename[1] != ':') {
        ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
        ::PathRemoveFileSpecW(filenameBuffer.data());
        ::PathAppend(filenameBuffer.data(), filenameUTF8.charactersWithNullTermination().data());
    }
    loadURL(filenameBuffer.data());
}

bool CWebView::isLoading() const
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    return frameClient->isLoading();
}

bool CWebView::isLoadingSucceeded() const
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    return frameClient->isLoaded();
}

bool CWebView::isLoadingFailed() const
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    return frameClient->isLoadFailed();
}

bool CWebView::isLoadingCompleted() const
{
    return isLoadingSucceeded() || isLoadingFailed();
}

bool CWebView::isDocumentReady() const
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    return frameClient->isDocumentReady();
}

void CWebView::setUserAgent(const utf8 * useragent)
{
    content::BlinkPlatformImpl* platform = (content::BlinkPlatformImpl*)blink::Platform::current();
    platform->setUserAgent((char *)useragent);
}

void CWebView::setUserAgent(const wchar_t * useragent )
{
    setUserAgent(String(useragent).utf8().data());
}

void CWebView::stopLoading()
{
    m_webPage->mainFrame()->stopLoading();
}

void CWebView::reload()
{
    m_webPage->mainFrame()->reload();
}

const utf8* CWebView::title()
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    m_title = frameClient->title();

    return m_title.string();
}

const wchar_t* CWebView::titleW()
{
    content::WebFrameClientImpl* frameClient = m_webPage->webFrameClientImpl();
    m_title = frameClient->title();

    return m_title.stringW();
}

void CWebView::resize(int w, int h)
{
    m_webPage->fireResizeEvent(m_webPage->getHWND(), WM_SIZE, 0, MAKELONG(w, h));
}

int CWebView::width() const 
{ 
    return m_webPage->viewportSize().width();
}

int CWebView::height() const 
{ 
    return m_webPage->viewportSize().height();
}

int CWebView::contentsWidth() const
{
    return contentWidth();
}

int CWebView::contentsHeight() const
{
    return contentHeight();
}

int CWebView::contentWidth() const
{
    return m_webPage->mainFrame()->contentsSize().width;
}

int CWebView::contentHeight() const
{
    return m_webPage->mainFrame()->contentsSize().height;
}

void CWebView::setDirty(bool dirty)
{
    m_webPage->setNeedsCommit();
}

bool CWebView::isDirty() const
{
    return m_webPage->isDrawDirty();
}

void CWebView::addDirtyArea(int x, int y, int w, int h)
{
    if (w > 0 && h > 0) {
        m_dirtyArea.unite(blink::IntRect(x, y, w, h));
        setDirty(true);
    }
}

void CWebView::layoutIfNeeded()
{
    //m_mainFrame->view()->updateLayoutAndStyleIfNeededRecursive();
    m_webPage->fireTimerEvent();
}

void CWebView::repaintIfNeeded()
{
    m_webPage->fireTimerEvent();
// 	if(!m_dirty)
//      return;
// 
// 	layoutIfNeeded();
//
// 	if (m_graphicsContext == NULL)
// 	{
// 		blink::BitmapInfo bmp = blink::BitmapInfo::createBottomUp(blink::IntSize(m_width, m_height));
// 		HBITMAP hbmp = ::CreateDIBSection(0, &bmp, DIB_RGB_COLORS, &m_pixels, NULL, 0);
// 		::SelectObject(m_hdc.get(), hbmp);
// 		m_hbitmap = adoptPtr(hbmp);
// 
// 		m_graphicsContext = new blink::GraphicsContext(m_hdc.get(), m_transparent);
// 	}
// 
// 	m_graphicsContext->save();
// 
// 	if (m_transparent)
// 		m_graphicsContext->clearRect(m_dirtyArea);
// 
// 	m_graphicsContext->clip(m_dirtyArea);
// 
// 	m_mainFrame->view()->paint(m_graphicsContext, m_dirtyArea);
// 
// 	m_graphicsContext->restore();
//     ChromeClient* client = (ChromeClient*)page()->chrome()->client();
//     client->paintPopupMenu(m_pixels,  m_width*4);

//     if (m_webPage->wkeHandler().paintUpdatedCallback) {
//         blink::IntPoint pt = m_dirtyArea.location();
//         blink::IntSize sz = m_dirtyArea.size();
//         m_webPage->wkeHandler().paintUpdatedCallback(this, m_webPage->wkeHandler().paintUpdatedCallbackParam, *m_hdc.get(), pt.x(), pt.y(), sz.width(), sz.height());
// 	}
    m_dirtyArea = blink::IntRect();
    //m_dirty = false;
}

HDC CWebView::viewDC()
{
    return m_webPage->viewDC();
}

HWND CWebView::windowHandle() const
{
    return m_hWnd;
}

void CWebView::setHandle(HWND wnd)
{
	m_hWnd = wnd;
	m_webPage->setHWND(wnd);
}

void CWebView::setHandleOffset(int x, int y)
{
	m_webPage->setHWNDoffset(x, y);
}

void CWebView::paint(void* bits, int pitch)
{
    if (m_webPage->needsCommit())
        m_webPage->fireTimerEvent();
    m_webPage->paintToBit(bits, pitch);
}

void CWebView::paint(void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha)
{
//     if (m_dirty)
//         repaintIfNeeded();
// 
//     if(xSrc + w > m_width) w = m_width - xSrc;
//     if(ySrc + h > m_height) h = m_height -ySrc;
//     
//     if(xDst + w > bufWid) w =bufWid - xDst;
//     if(yDst + h > bufHei) h = bufHei - yDst;
//     
//     int pitchDst = bufWid*4;
//     int pitchSrc = m_width*4;
//     
//     unsigned char* src = (unsigned char*)m_pixels; 
//     unsigned char* dst = (unsigned char*)bits; 
//     src += pitchSrc*ySrc + xSrc*4;
//     dst += yDst*pitchDst + xDst*4;
//     
//     if(bCopyAlpha) {
//         for(int j = 0; j< h; j++) {
//             memcpy(dst,src,w*4);
//             dst += pitchDst;
//             src += pitchSrc;
//         }
//     } else {
//         for(int j = 0; j< h; j++) {
//             for(int i=0;i<w;i++) {
//                 memcpy(dst,src,3);
//                 dst += 4;
//                 src += 4;
//             }
//             dst += (bufWid - w)*4;
//             src += (m_width - w)*4;
//         }
//     }
}

bool CWebView::canGoBack() const
{
    //return page()->backForwardList()->backItem() && !page()->defersLoading();
    return m_webPage->canGoBack();
}

bool CWebView::goBack()
{
    //return page()->goBack();
    m_webPage->goBack();
    return true;
}

bool CWebView::canGoForward() const
{
    //return page()->backForwardList()->forwardItem() && !page()->defersLoading();
    return m_webPage->canGoForward();
}

bool CWebView::goForward()
{
    //return page()->goForward();
    m_webPage->goForward();
    return true;
}

void CWebView::editorSelectAll()
{
    m_webPage->mainFrame()->executeCommand("SelectAll");
}

void CWebView::editorCopy()
{
    m_webPage->mainFrame()->executeCommand("Copy");
}

void CWebView::editorCut()
{
    m_webPage->mainFrame()->executeCommand("Cut");
}

void CWebView::editorPaste()
{
    m_webPage->mainFrame()->executeCommand("Paste");
}

void CWebView::editorDelete()
{
    m_webPage->mainFrame()->executeCommand("Delete");
}

void CWebView::setCookieEnabled(bool enable)
{
    //page()->setCookieEnabled(enable);
}

//获取cookies
const wchar_t* CWebView::cookieW()
{
// 	int e = 0;
// 	m_cookie = mainFrame()->document()->cookie(e);
// 	return m_cookie.stringW();
    return L"";
}

const utf8* CWebView::cookie()
{
// 	int e = 0;
// 	m_cookie = mainFrame()->document()->cookie(e);
// 	return m_cookie.string();
    return "";
}

bool CWebView::isCookieEnabled() const
{
    //return page()->cookieEnabled();
    return true;
}

void CWebView::setMediaVolume(float volume)
{
    //page()->setMediaVolume(volume);
}

float CWebView::mediaVolume() const
{
    //return page()->mediaVolume();
    return 0;
}

bool CWebView::fireMouseEvent(unsigned int message, int x, int y, unsigned int flags)
{
    if (message == WM_CANCELMODE) {
        //mainFrame()->eventHandler()->lostMouseCapture();
        return true;
    }

    // Create our event.
    // On WM_MOUSELEAVE we need to create a mouseout event, so we force the position
    // of the event to be at (MINSHORT, MINSHORT).
    if (message == WM_MOUSELEAVE) {
        x = MINSHORT;
        y = MINSHORT;
    }
    
    BOOL handled = TRUE;
    WPARAM wParam = 0;
    LPARAM lParam = MAKELPARAM(x, y);
    if (flags & WKE_CONTROL)
        wParam |= MK_CONTROL;
    if (flags & WKE_SHIFT)
        wParam |= MK_SHIFT;

    if (flags & WKE_LBUTTON)
        wParam |= MK_LBUTTON;
    if (flags & WKE_MBUTTON)
        wParam |= MK_MBUTTON;
    if (flags & WKE_RBUTTON)
        wParam |= MK_RBUTTON;
    m_webPage->fireMouseEvent(m_webPage->getHWND(), message, wParam, lParam, &handled);
    return handled;
}

bool CWebView::fireContextMenuEvent(int x, int y, unsigned int flags)
{
//     page()->contextMenuController()->clearContextMenu();
// 
//     if (x == -1 && y == -1)
//     {   
//         blink::Frame* focusedFrame = page()->focusController()->focusedOrMainFrame();
//         return focusedFrame->eventHandler()->sendContextMenuEventForKey();
//     }
// 
//     blink::IntPoint pos(x, y);
//     blink::IntPoint globalPos(x, y);
// 
//     blink::MouseButton button = messageToButtonType(WM_RBUTTONUP, flags);
//     blink::MouseEventType eventType = messageToEventType(WM_RBUTTONUP);
// 
//     bool shift = flags & WKE_SHIFT;
//     bool ctrl = flags & WKE_CONTROL;
//     bool alt = GetKeyState(VK_MENU) & 0x8000;
//     bool meta = alt;
//     double timestamp = ::GetTickCount()*0.001;
// 
//     int clickCount = 0;
//     blink::PlatformMouseEvent mouseEvent(pos, globalPos, button, eventType, clickCount, shift, ctrl, alt, meta, timestamp);
// 
//     blink::IntPoint documentPoint(mainFrame()->view()->windowToContents(pos));
//     blink::HitTestResult result = mainFrame()->eventHandler()->hitTestResultAtPoint(documentPoint, false);
//     blink::Frame* targetFrame = result.innerNonSharedNode() ? result.innerNonSharedNode()->document()->frame() : page()->focusController()->focusedOrMainFrame();
// 
//     targetFrame->view()->setCursor(blink::pointerCursor());
//     return targetFrame->eventHandler()->sendContextMenuEvent(mouseEvent);
    return true;
}

bool CWebView::fireMouseWheelEvent(int x, int y, int wheelDelta, unsigned int flags)
{
//     if (!mainFrame()->view()->didFirstLayout())
//         return true;

    BOOL handled = TRUE;
    WPARAM wParam = 0;
    LPARAM lParam = MAKELPARAM(x, y);
    if (flags & WKE_CONTROL)
        wParam |= MK_CONTROL;
    if (flags & WKE_SHIFT)
        wParam |= MK_SHIFT;

    if (flags & WKE_LBUTTON)
        wParam |= MK_LBUTTON;
    if (flags & WKE_MBUTTON)
        wParam |= MK_MBUTTON;
    if (flags & WKE_RBUTTON)
        wParam |= MK_RBUTTON;

    wParam = MAKEWPARAM(wParam, wheelDelta);

    m_webPage->fireWheelEvent(m_webPage->getHWND(), WM_MOUSEWHEEL, wParam, lParam);
    return handled;
}

bool CWebView::fireKeyUpEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    WPARAM wParam = virtualKeyCode;
    LPARAM lParam = 0;

    if (flags & WKE_REPEAT)
        lParam |= ((KF_REPEAT) >> 16);
    if (flags & WKE_EXTENDED)
        lParam |= ((KF_EXTENDED) >> 16);

    m_webPage->fireKeyUpEvent(m_webPage->getHWND(), WM_KEYUP, wParam, lParam);
    return true;
}

bool CWebView::fireKeyDownEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    WPARAM wParam = virtualKeyCode;
    LPARAM lParam = 0;

    if (flags & WKE_REPEAT)
        lParam |= ((KF_REPEAT) >> 16);
    if (flags & WKE_EXTENDED)
        lParam |= ((KF_EXTENDED) >> 16);

    m_webPage->fireKeyDownEvent(m_webPage->getHWND(), WM_KEYDOWN, wParam, lParam);
    return true;
}

bool CWebView::fireKeyPressEvent(unsigned int charCode, unsigned int flags, bool systemKey)
{
    WPARAM wParam = charCode;
    LPARAM lParam = 0;

    if (flags & WKE_REPEAT)
        lParam |= ((KF_REPEAT) >> 16);
    if (flags & WKE_EXTENDED)
        lParam |= ((KF_EXTENDED) >> 16);

    m_webPage->fireKeyPressEvent(m_webPage->getHWND(), WM_CHAR, wParam, lParam);
    return true;
}

bool CWebView::fireWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    BOOL handle = FALSE;
    LRESULT webResult = 0;

    if (WM_SETCURSOR == message) {
        webResult = m_webPage->fireCursorEvent(hWnd, message, wParam, lParam, &handle);
    }

    if (result)
        *result = webResult;
    return handle;
}

void CWebView::setFocus()
{
    m_webPage->fireSetFocusEvent(m_webPage->getHWND(), WM_SETFOCUS, 0, 0);
}

void CWebView::killFocus()
{
    m_webPage->fireKillFocusEvent(m_webPage->getHWND(), WM_KILLFOCUS, 0, 0);
}

wkeRect CWebView::getCaret()
{
    return caretRect();
}

wkeRect CWebView::caretRect()
{
    wkeRect rect;
    rect.x = rect.y = 0;
    rect.w = rect.h = 0;
    if (!m_webPage)
        return rect;

    IntRect caret = m_webPage->caretRect();

    rect.x = caret.x();
    rect.y = caret.y();
    rect.w = caret.width();
    rect.h = caret.height();

    return rect;
}

jsValue jsUndefined()
{
    return 0;
}

jsValue CWebView::runJS(const wchar_t* script)
{
    if (!script)
        return jsUndefined();

    blink::WebString codeString;
    codeString.assign(script, wcslen(script));
    blink::WebScriptSource code(codeString);
    m_webPage->mainFrame()->executeScriptAndReturnValue(code);

    return jsUndefined();
}

jsValue CWebView::runJS(const utf8* script)
{
    if (!script)
        return jsUndefined();

    blink::WebScriptSource code(blink::WebString::fromUTF8(script));
    v8::Local<v8::Value> result = m_webPage->mainFrame()->executeScriptAndReturnValue(code);

//     v8::Local<v8::Context> context = m_webPage->mainFrame()->mainWorldScriptContext();
//     v8::Isolate* isolate = context->GetIsolate();
//     v8::HandleScope handleScope(isolate);
//     v8::Context::Scope contextScope(context);
//     
//     return createJsValueByLocalValue(isolate, context, result);
    return jsUndefined();
}

jsExecState CWebView::globalExec()
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);
    //v8::Context::Scope contextScope(isolate->GetCurrentContext());

    return wke::createTempExecStateByV8Context(m_webPage->mainFrame()->mainWorldScriptContext());
}

void CWebView::sleep()
{
    m_awake = false;
//     page()->setCanStartMedia(false);
//     page()->willMoveOffscreen();
}

void CWebView::wake()
{
    m_awake = true;
//     page()->didMoveOnscreen();
//     page()->setCanStartMedia(true);
}

bool CWebView::isAwake() const
{
    return m_awake;
}

void CWebView::setZoomFactor(float factor)
{
    //m_mainFrame->setPageZoomFactor(factor);
}

float CWebView::zoomFactor() const
{
    //return m_mainFrame->pageZoomFactor();
    return 1;
}

void CWebView::setEditable(bool editable)
{
//     if (page()->isEditable() != editable) {
//         page()->setEditable(editable);
//         page()->setTabKeyCyclesThroughElements(!editable);
//         if (editable)
//             m_mainFrame->editor()->applyEditingStyleToBodyElement();
//     }
}

void CWebView::onTitleChanged(wkeTitleChangedCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().titleChangedCallback = callback;
    m_webPage->wkeHandler().titleChangedCallbackParam = callbackParam;
}

void CWebView::onURLChanged(wkeURLChangedCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().urlChangedCallback = callback;
    m_webPage->wkeHandler().urlChangedCallbackParam = callbackParam;
}

void CWebView::onPaintUpdated(wkePaintUpdatedCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().paintUpdatedCallback = callback;
    m_webPage->wkeHandler().paintUpdatedCallbackParam = callbackParam;
}

void CWebView::onAlertBox(wkeAlertBoxCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().alertBoxCallback = callback;
    m_webPage->wkeHandler().alertBoxCallbackParam = callbackParam;
}

void CWebView::onConfirmBox(wkeConfirmBoxCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().confirmBoxCallback = callback;
    m_webPage->wkeHandler().confirmBoxCallbackParam = callbackParam;
}

void CWebView::onPromptBox(wkePromptBoxCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().promptBoxCallback = callback;
    m_webPage->wkeHandler().promptBoxCallbackParam = callbackParam;
}

void defaultRunAlertBox(wkeWebView webView, void* param, const wkeString msg)
{
    MessageBoxW(NULL, wkeGetStringW(msg), L"wke", MB_OK);
}

bool defaultRunConfirmBox(wkeWebView webView, void* param, const wkeString msg)
{
    int result = MessageBoxW(NULL, wkeGetStringW(msg), L"wke", MB_OKCANCEL);
    return result == IDOK;
}

bool defaultRunPromptBox(wkeWebView webView, void* param, const wkeString msg, const wkeString defaultResult, wkeString result)
{
    return false;
}

void CWebView::_initHandler()
{
    m_webPage->wkeHandler().alertBoxCallback = defaultRunAlertBox;
    m_webPage->wkeHandler().confirmBoxCallback = defaultRunConfirmBox;
    m_webPage->wkeHandler().promptBoxCallback = defaultRunPromptBox;
}

void CWebView::_initPage()
{
    m_webPage = new content::WebPage(nullptr);
    m_webPage->initWkeWebView(this);
    m_webPage->init(nullptr);

//     blink::Page::PageClients pageClients;
//     pageClients.chromeClient = new ChromeClient(this);
//     pageClients.contextMenuClient = new ContextMenuClient;
//     pageClients.inspectorClient = new InspectorClient;
//     pageClients.editorClient = new EditorClient;
//     pageClients.dragClient = new DragClient;
// 
//     m_page = adoptPtr(new blink::Page(pageClients));
//     blink::Settings* settings = m_page->settings();
//     settings->setMinimumFontSize(0);
//     settings->setMinimumLogicalFontSize(9);
//     settings->setDefaultFontSize(16);
//     settings->setDefaultFixedFontSize(13);
//     settings->setJavaScriptEnabled(true);
//     settings->setPluginsEnabled(true);
//     settings->setLoadsImagesAutomatically(true);
//     settings->setDefaultTextEncodingName(icuwin_getDefaultEncoding());
//     
//     settings->setStandardFontFamily("Times New Roman");
//     settings->setFixedFontFamily("Courier New");
//     settings->setSerifFontFamily("Times New Roman");
//     settings->setSansSerifFontFamily("Arial");
//     settings->setCursiveFontFamily("Comic Sans MS");
//     settings->setFantasyFontFamily("Times New Roman");
//     settings->setPictographFontFamily("Times New Roman");
// 
//     settings->setAllowUniversalAccessFromFileURLs(true);
//     settings->setAllowFileAccessFromFileURLs(true);
//     settings->setJavaScriptCanAccessClipboard(true);
//     settings->setShouldPrintBackgrounds(true);
//     settings->setTextAreasAreResizable(true);
//     settings->setLocalStorageEnabled(true);
// 
//     UChar dir[256];
//     GetCurrentDirectory(256, dir);
//     wcscat(dir, L"\\localStorage");
//     settings->setLocalStorageDatabasePath(dir);
//     blink::DatabaseTracker::tracker().setDatabaseDirectoryPath(dir);
// 
//     FrameLoaderClient* loader = new FrameLoaderClient(this, m_page.get());
//     m_mainFrame = blink::Frame::create(m_page.get(), NULL, loader).get();
//     loader->setFrame(m_mainFrame);
//     m_mainFrame->init();
// 
//     page()->focusController()->setActive(true);
}

void CWebView::_initMemoryDC()
{
    //m_hdc = adoptPtr(::CreateCompatibleDC(0));
}

void CWebView::onNavigation(wkeNavigationCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().navigationCallback = callback;
    m_webPage->wkeHandler().navigationCallbackParam = callbackParam;
}

void CWebView::onCreateView(wkeCreateViewCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().createViewCallback = callback;
    m_webPage->wkeHandler().createViewCallbackParam = callbackParam;
}

void CWebView::onLoadingFinish(wkeLoadingFinishCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().loadingFinishCallback = callback;
    m_webPage->wkeHandler().loadingFinishCallbackParam = callbackParam;
}

void CWebView::onDownload(wkeDownloadCallback callback, void* callbackParam)
{
	m_webPage->wkeHandler().downloadCallback = callback;
	m_webPage->wkeHandler().downloadCallbackParam = callbackParam;
}

void CWebView::onDocumentReady(wkeDocumentReadyCallback callback, void* callbackParam)
{
    m_webPage->wkeHandler().documentReadyCallback = callback;
    m_webPage->wkeHandler().documentReadyCallbackParam = callbackParam;
}

void CWebView::onLoadUrlBegin(wkeLoadUrlBeginCallback callback, void* callbackParam)
{
	m_webPage->wkeHandler().loadUrlBeginCallback = callback;
	m_webPage->wkeHandler().loadUrlBeginCallbackParam = callbackParam;
}

void CWebView::onLoadUrlEnd(wkeLoadUrlEndCallback callback, void* callbackParam)
{
	m_webPage->wkeHandler().loadUrlEndCallback = callback;
	m_webPage->wkeHandler().loadUrlEndCallbackParam = callbackParam;
}

void CWebView::setClientHandler(const wkeClientHandler* handler)
{
    m_webPage->wkeSetClientHandler((void*)handler);
}

const wkeClientHandler* CWebView::getClientHandler() const
{
    return (const wkeClientHandler *)m_webPage->wkeClientHandler();
}

};//namespace wke

//static Vector<wke::CWebView*> s_webViews;

wkeWebView wkeCreateWebView()
{
    wke::CWebView* webView = new wke::CWebView();
    //s_webViews.append(webView);
    return webView;
}

void wkeDestroyWebView(wkeWebView webView)
{
    //size_t pos = s_webViews.find(webView);

    //ASSERT(pos != notFound);
    //if (pos != notFound)
    //{
    //    s_webViews.remove(pos);
        delete webView;
    //}
}

#endif
