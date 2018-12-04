#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)

#include "libcef/browser/CefFrameHostImpl.h"

#include "v8.h"
#include "include/cef_browser.h"
#include "include/cef_v8.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebView.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/renderer/CefV8Impl.h"
#include "libcef/common/CefMessages.h"
#include "libcef/common/StringUtil.h"
#include "libcef/common/CefTaskImpl.h"
#include "content/browser/WebPage.h"

int64 CefFrameHostImpl::GetFrameIdByBlinkFrame(const blink::WebFrame* frame) {
    if (!frame)
        return content::WebPage::kInvalidFrameId;

    blink::Frame* blinkFrame = blink::toCoreFrame(frame);
    if (!blinkFrame)
        return content::WebPage::kInvalidFrameId;
    return blinkFrame->frameID();
}

int64 CefFrameHostImpl::GetFrameIdByBlinkFrame(const blink::WebLocalFrame* frame) {
    if (!frame)
        return content::WebPage::kInvalidFrameId;

    blink::Frame* blinkFrame = blink::toCoreFrame((blink::WebFrame*)frame);
    if (!blinkFrame)
        return content::WebPage::kInvalidFrameId;
    return blinkFrame->frameID();
}

WTF::String CefFrameHostImpl::GetFrameNameByBlinkFrame(const blink::WebLocalFrame* frame) {
    if (!frame || !frame->view())
        return "";

    return frame->uniqueName();
}

CefFrameHostImpl::CefFrameHostImpl(CefBrowserHostImpl* browser,
    int64 frameId,
    bool isMainFrame,
    const CefString& url,
    const CefString& name,
    int64 parentFrameId) {
    m_isMainFrame = isMainFrame;
    m_url = url;
    m_names = name;
    m_browser = browser;
    m_frameId = frameId;
    m_isFocused = false;
}

// void CefFrameHostImpl::Init(CefBrowserHostImpl* browser, blink::WebLocalFrame* frame, /*const CefString& url,*/ const CefString& name) {
//     ASSERT(frame);
//     blink::Frame* blinkFrame = blink::toCoreFrame((blink::WebFrame*)frame);
//     if (!blinkFrame)
//         return;
// 
//     m_isMainFrame = blinkFrame->isMainFrame();
//     m_frame = frame;
//     // m_url = url;
//     m_names = name;
//     m_browser = browser;
// 
//     m_frameId = blinkFrame->frameID();
// }

bool CefFrameHostImpl::IsValid() { return m_browser != nullptr; }
void CefFrameHostImpl::Undo() {
    SendCommand("Undo", nullptr);
}

void CefFrameHostImpl::Redo() {
    SendCommand("Redo", nullptr);
}

void CefFrameHostImpl::Cut() {
    SendCommand("Cut", nullptr);
}

void CefFrameHostImpl::Copy() {
    SendCommand("Copy", nullptr);
}

void CefFrameHostImpl::Paste() {
    SendCommand("Paste", nullptr);
}

void CefFrameHostImpl::Delete() {
    SendCommand("Delete", nullptr);
}

void CefFrameHostImpl::SelectAll() {
    SendCommand("SelectAll", nullptr);
}

void CefFrameHostImpl::ViewSource() {
//     Cef_Response_Params response;
//     SendCommand("GetSource", &response);
// 
//     CefRefPtr<CefBrowser> browser = frame_->GetBrowser();
//     if (browser.get()) {
//         static_cast<CefBrowserHostImpl*>(browser.get())->ViewText(params.response);
//     }
}

void CefFrameHostImpl::GetSource(CefRefPtr<CefStringVisitor> visitor) {
    Cef_Response_Params response;
    SendCommand("GetSource", &response);
    if (!response.success || response.response.isNull() || response.response.isEmpty())
        return;
    CefString string;
    cef::WTFStringToCefString(response.response, string);

    visitor->Visit(string);
}

void CefFrameHostImpl::GetText(CefRefPtr<CefStringVisitor> visitor) {
    Cef_Response_Params response;
    SendCommand("GetText", &response);
    if (!response.success || response.response.isNull() || response.response.isEmpty())
        return;
    CefString string;
    cef::WTFStringToCefString(response.response, string);

    visitor->Visit(string);
}

void CefFrameHostImpl::LoadRequest(CefRefPtr<CefRequest> request) {
//     if (!browser_)
//         return;
// 
//     CefMsg_LoadRequest_Params params;
//     params.url = GURL(std::string(request->GetURL()));
//     params.method = request->GetMethod();
//     params.frame_id = frame_id_;
//     params.first_party_for_cookies = GURL(std::string(request->GetFirstPartyForCookies()));
// 
//     CefRequest::HeaderMap headerMap;
//     request->GetHeaderMap(headerMap);
//     if (!headerMap.empty())
//         params.headers = HttpHeaderUtils::GenerateHeaders(headerMap);
// 
//     CefRefPtr<CefPostData> postData = request->GetPostData();
//     if (postData.get()) {
//         CefPostDataImpl* impl = static_cast<CefPostDataImpl*>(postData.get());
//         params.upload_data = new net::UploadData();
//         impl->Get(*params.upload_data.get());
//     }
// 
//     params.load_flags = request->GetFlags();
// 
//     browser_->LoadRequest(params);
    DebugBreak();
}

void CefFrameHostImpl::LoadURL(const CefString& url) {
    if (m_browser)
        m_browser->LoadURL(m_frameId, url, blink::Referrer(), CefString());
}

void CefFrameHostImpl::LoadString(const CefString& string, const CefString& url) {
    if (m_browser)
        m_browser->LoadString(m_frameId, string, url);
}

void CefFrameHostImpl::ExecuteJavaScript(const CefString& jsCode, const CefString& scriptUrl, int startLine) {
    if (jsCode.empty())
        return;
    
    if (!CEF_CURRENTLY_ON_UIT()) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefFrameHostImpl::ExecuteJavaScript, this, jsCode, scriptUrl, startLine));
        return;
    }

    blink::WebFrame* frame = m_browser->webPage()->getWebFrameFromFrameId(m_frameId);
    if (!frame)
        return;

    if (startLine < 0)
        startLine = 0;

    blink::KURL kurl = blink::KURL(blink::ParsedURLString, scriptUrl.ToString().c_str());

    base::string16 code16 = jsCode.ToString16();
    blink::WebString webCode(code16.c_str(), code16.size());
    frame->executeScript(blink::WebScriptSource(webCode, kurl, startLine));
}

bool CefFrameHostImpl::IsMain() {
//     if (m_frame)
//         return (m_frame->parent() == NULL);
//     return false;
    return m_isMainFrame;
}

bool CefFrameHostImpl::IsFocused() {
//     if (m_frame && m_frame->view())
//         return (m_frame->view()->focusedFrame() == m_frame);
//     return false;
    MutexLocker locker(m_stateLock);
    return m_isFocused;
}

CefString CefFrameHostImpl::GetName() {
//     CefString name;
//     if (!m_frame)
//         return name;
// 
//     cef::WebStringToCefString(m_frame->uniqueName(), name);
//     return name;
    MutexLocker locker(m_stateLock);
    return m_names;
}

int64 CefFrameHostImpl::GetIdentifier() { return m_parentFrameId; }

CefRefPtr<CefFrame> CefFrameHostImpl::GetParent() {
//     if (m_frame) {
//         blink::WebFrame* parent = m_frame->parent();
//         if (parent)
//             return m_frame->GetWebFrameImpl(parent).get();
//     }

    return NULL;
}

CefString CefFrameHostImpl::GetURL() {
//     CefString url;
//     if (!m_frame)
//         return url;
// 
//     blink::KURL kurl = m_frame->document().url();
//     cef::WTFStringToCefString(kurl.string(), url);
//     
//     return url;
    return m_url;
}

CefRefPtr<CefBrowser> CefFrameHostImpl::GetBrowser() { return m_browser; }

CefRefPtr<CefV8Context> CefFrameHostImpl::GetV8Context() {
    if (!CEF_CURRENTLY_ON_UIT())
        return nullptr;

    blink::WebFrame* frame = m_browser->webPage()->getWebFrameFromFrameId(m_frameId);
    if (!frame)
        return nullptr;

    v8::Isolate* isolate = blink::mainThreadIsolate();
    v8::HandleScope handleScope(isolate);
    return new CefV8ContextImpl(isolate, frame->mainWorldScriptContext());
}

void CefFrameHostImpl::VisitDOM(CefRefPtr<CefDOMVisitor> visitor) {
//     blink::WebFrame* frame = m_browser->webPage()->getWebFrameFromFrameId(m_frameId);
//     if (!frame)
//         return;
    DebugBreak();
}

void CefFrameHostImpl::SetFocused(bool focused) {
    MutexLocker locker(m_stateLock);
    m_isFocused = focused;
}

void CefFrameHostImpl::SetAttributes(const CefString& url, const CefString& name, int64 parentFrameId) {
    MutexLocker locker(m_stateLock);
    if (!url.empty() && url != m_url)
        m_url = url;
    if (!name.empty() && name != m_names)
        m_names = name;
    if (m_parentFrameId != content::WebPage::kUnspecifiedFrameId)
        m_parentFrameId = parentFrameId;
}

// Avoids unnecessary string type conversions.
void CefFrameHostImpl::SendJavaScript(const std::string& jsCode, const std::string& scriptUrl, int startLine) {
}

void CefFrameHostImpl::SendCommand(const WTF::String& commandName, Cef_Response_Params* response) {
    if (!m_browser || 0 == commandName.length())
        return;

    Cef_Request_Params* request = new Cef_Request_Params();
    request->name = "execute-command";

    Vector<char> commandNameVector = WTF::ensureStringToUTF8(commandName, true);
    std::string commandNameString = commandNameVector.data();

    request->arguments.AppendString(commandNameString);
    request->frameId = m_frameId;
    m_browser->SendCommand(request, response);
}


#endif
