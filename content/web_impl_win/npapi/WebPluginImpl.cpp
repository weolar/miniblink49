/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2010 Girish Ramakrishnan <girish@forwardbias.in>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"

#include "content/web_impl_win/WebCookieJarCurlImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"
#include "content/web_impl_win/npapi/PluginPackage.h"
#include "content/web_impl_win/npapi/PluginMainThreadScheduler.h"
#include "content/web_impl_win/npapi/PluginMessageThrottlerWin.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/web/WebPluginContainerImpl.h"
#include "third_party/WebKit/Source/platform/network/ResourceRequest.h"
#include "third_party/WebKit/Source/platform/graphics/Image.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"
#include "third_party/WebKit/Source/platform/UserGestureIndicator.h"
#include "third_party/WebKit/Source/core/loader/DocumentLoader.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/page/ChromeClient.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/bindings/core/v8/ScriptController.h"
#include "third_party/WebKit/Source/bindings/core/v8/npruntime_impl.h"
#include "third_party/WebKit/Source/bindings/core/v8/ScriptSourceCode.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/web/WebPluginContainer.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebViewClient.h"
#include "third_party/WebKit/Source/wtf/ASCIICType.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/npapi/bindings/npapi.h"
#include "gen/blink/core/HTMLNames.h"
#include "wtf/text/WTFStringUtil.h"

using std::min;

using namespace WTF;
using namespace blink;

namespace content {

using namespace HTMLNames;

static int s_callingPlugin;

typedef HashMap<NPP, WebPluginImpl*> InstanceMap;

static InstanceMap& instanceMap()
{
    static InstanceMap& map = *new InstanceMap;
    return map;
}

static String scriptStringIfJavaScriptURL(const KURL& url)
{
    if (!protocolIsJavaScript(url))
        return String();

    // This returns an unescaped string
    return WTF::ensureStringToUTF8String(decodeURLEscapeSequences(url.string().substring(11)));
}

static void buildResourceRequest(FrameLoadRequest* frameLoadRequest, blink::LocalFrame* parentFrame, const KURL& url, const char* target)
{
    if (target)
        frameLoadRequest->setFrameName(target);

    ResourceRequest* request = &frameLoadRequest->resourceRequest();

    request->setHTTPMethod("GET");
    request->setURL(url);
    //request->setHTTPHeaderField("x-requested-with", AtomicString("ShockwaveFlash/17.0.0.171"));
    //request->setHTTPHeaderField("accept-encoding", AtomicString("gzip, deflate, sdch"));
    //request->setHTTPHeaderField("accept", AtomicString("*/*"));
    request->setHTTPHeaderField("user-agent", AtomicString(String(blink::Platform::current()->userAgent())));

    if (parentFrame->document())
        request->setHTTPHeaderField("referer", AtomicString(parentFrame->document()->baseURL().string()));
}

RefPtr<Image> s_nullPluginImage;
WebPluginImpl* WebPluginImpl::s_currentPluginView = 0;

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webPluginImplCount, ("WebPluginImplCount"));
#endif

WebPluginImpl::WebPluginImpl(WebLocalFrame* parentFrame, const blink::WebPluginParams& params)
    : m_parentFrame(toWebLocalFrameImpl(parentFrame)->frame())
    , m_plugin(nullptr)
    , m_widget(nullptr)
    , m_parentWidget(nullptr)
    , m_isStarted(false)
    , m_url(params.url)
    , m_status(PluginStatusLoadedSuccessfully)
    , m_requestTimer(this, &WebPluginImpl::requestTimerFired)
    , m_invalidateTimer(this, &WebPluginImpl::invalidateTimerFired)
    , m_popPopupsStateTimer(this, &WebPluginImpl::popPopupsStateTimerFired)
    , m_lifeSupportTimer(this, &WebPluginImpl::lifeSupportTimerFired)
    , m_asynStartTask(nullptr)
    , m_setPlatformPluginWidgetVisibilityTimer(this, &WebPluginImpl::asynSetPlatformPluginWidgetVisibilityTimerFired)
    , m_mode(params.loadManually ? NP_FULL : NP_EMBED)
    , m_paramNames(0)
    , m_paramValues(0)
    , m_mimeType(params.mimeType)
    , m_instance(0)
    , m_isWindowed(true)
    , m_isTransparent(false)
    , m_haveInitialized(false)
    , m_isWaitingToStart(false)
    , m_pluginWndProc(0)
    , m_lastMessage(0)
    , m_isCallingPluginWndProc(false)
    , m_wmPrintHDC(0)
    , m_haveUpdatedPluginWidget(false)
    , m_loadManually(params.loadManually)
    , m_manualStream(nullptr)
    , m_isJavaScriptPaused(false)
    , m_haveCalledSetWindow(false)
    , m_memoryCanvas(nullptr)
    , m_wkeWebview(nullptr)
{
#ifndef NDEBUG
    webPluginImplCount.increment();
#endif
    if (!m_parentFrame)
        return;

    // if we fail to find a plugin for this MIME type, findPlugin will search for
    // a plugin by the file extension and update the MIME type, so pass a mutable String
    m_plugin = PluginDatabase::installedPlugins()->findPlugin(m_url, m_mimeType);

    // No plugin was found, try refreshing the database and searching again
    if (!m_plugin && PluginDatabase::installedPlugins()->refresh())
        m_plugin = PluginDatabase::installedPlugins()->findPlugin(m_url, m_mimeType);

    if (!m_plugin) {
        m_status = PluginStatusCanNotFindPlugin;
        return;
    }

    m_instance = new NPP_t();
    m_instance->ndata = this;
    m_instance->pdata = 0;

    instanceMap().add(m_instance, this);
    memset(&m_npWindow, 0, sizeof(m_npWindow));
    setParameters(params.attributeNames, params.attributeValues);

    //resize(size);
}

WebPluginImpl::~WebPluginImpl()
{
    ASSERT(!m_lifeSupportTimer.isActive());

    if (m_asynStartTask)
        m_asynStartTask->onParentDestroy();

    // If we failed to find the plug-in, we'll return early in our constructor, and
    // m_instance will be 0.
    if (m_instance)
        instanceMap().remove(m_instance);

    //     if (m_isWaitingToStart)
    //         m_parentFrame->document()->removeMediaCanStartListener(this);

    stop();

    freeStringArray(m_paramNames, m_paramCount);
    freeStringArray(m_paramValues, m_paramCount);

    platformDestroy();

    m_pluginContainer->clearScriptObjects();

//     if (m_plugin && !(m_plugin->quirks().contains(PluginQuirkDontUnloadPlugin)))
//         m_plugin->unload(); // 不卸载了，卸载容易出各种问题

#ifndef NDEBUG
    webPluginImplCount.decrement();
#endif
}

void WebPluginImpl::popPopupsStateTimerFired(blink::Timer<WebPluginImpl>*)
{
    popPopupsEnabledState();
}

void WebPluginImpl::init()
{
    if (m_haveInitialized)
        return;

    m_haveInitialized = true;

//     // if we fail to find a plugin for this MIME type, findPlugin will search for
//     // a plugin by the file extension and update the MIME type, so pass a mutable String
//     m_plugin = PluginDatabase::installedPlugins()->findPlugin(m_url, m_mimeType);

    // No plugin was found, try refreshing the database and searching again
    if (!m_plugin && PluginDatabase::installedPlugins()->refresh())
        m_plugin = PluginDatabase::installedPlugins()->findPlugin(m_url, m_mimeType);

    if (!m_plugin) {
        m_status = PluginStatusCanNotLoadPlugin;
        return;
    }

    //WTF_LOG(Plugins, "WebPluginImpl::init(): Initializing plug-in '%s'", m_plugin->name().utf8().data());
    if (!m_plugin->load()) {
        m_plugin = nullptr;
        m_status = PluginStatusCanNotLoadPlugin;
        return;
    }

    if (!startOrAddToUnstartedList()) {
        m_status = PluginStatusCanNotLoadPlugin;
        return;
    }

    m_status = PluginStatusLoadedSuccessfully;
}

bool WebPluginImpl::startOrAddToUnstartedList()
{
    if (!m_parentFrame->page())
        return false;

    // We only delay starting the plug-in if we're going to kick off the load
    // ourselves. Otherwise, the loader will try to deliver data before we've
    // started the plug-in.
//     if (!m_loadManually && !m_parentFrame->page()->canStartMedia()) {
//         m_parentFrame->document()->addMediaCanStartListener(this);
//         m_isWaitingToStart = true;
//         return true;
//     }

    return start();
}

bool WebPluginImpl::start()
{
    if (m_isStarted)
        return false;

    m_isWaitingToStart = false;

    PluginMainThreadScheduler::scheduler().registerPlugin(m_instance);

    ASSERT(m_plugin);
    ASSERT(m_plugin->pluginFuncs()->newp);

    NPError npErr;
    {
        WebPluginImpl::setCurrentPluginView(this);
        //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
        setCallingPlugin(true);
        npErr = m_plugin->pluginFuncs()->newp((NPMIMEType)m_mimeType.utf8().data(), m_instance, m_mode, m_paramCount, m_paramNames, m_paramValues, NULL);
        setCallingPlugin(false);
        //LOG_NPERROR(npErr);
        WebPluginImpl::setCurrentPluginView(0);
    }

    if (npErr != NPERR_NO_ERROR) {
        m_status = PluginStatusCanNotLoadPlugin;
        PluginMainThreadScheduler::scheduler().unregisterPlugin(m_instance);
        return false;
    }

    m_isStarted = true;

    if (!m_url.isEmpty() && !m_loadManually) {
        FrameLoadRequest frameLoadRequest(m_parentFrame->document());
        buildResourceRequest(&frameLoadRequest, m_parentFrame, m_url, nullptr);

        load(frameLoadRequest, false, 0);
    }

    m_status = PluginStatusLoadedSuccessfully;

    if (!platformStart())
        m_status = PluginStatusCanNotLoadPlugin;

    if (m_status != PluginStatusLoadedSuccessfully)
        return false;

    return true;
}

void WebPluginImpl::mediaCanStart()
{
    ASSERT(!m_isStarted);
//     if (!start())
//         parentFrame()->loader().client().dispatchDidFailToStartPlugin(this);
}

class DestroyNpTask : public blink::WebThread::TaskObserver {
public:
    DestroyNpTask(NPP_DestroyProcPtr destroyFunc, NPP instance)
    {
        m_destroyFunc = destroyFunc;
        m_instance = instance;
    }

    virtual ~DestroyNpTask() override
    {
        delete m_instance;
    }

    virtual void willProcessTask() override
    {
    }

    virtual void didProcessTask() override
    {
        String out = String::format("DestroyNpTask: %p, m_instance: %p, pdata: %p\n", this, m_instance, m_instance->pdata);
        OutputDebugStringA(out.utf8().data());

        NPSavedData* savedData = 0;
        //WebPluginImpl::setCurrentPluginView(this);
        //SetCallingPlugin(true);
        NPError npErr = m_destroyFunc(m_instance, &savedData);
        //setCallingPlugin(false);
        //WebPluginImpl::setCurrentPluginView(0);

        if (savedData) {
            // TODO: Actually save this data instead of just discarding it
            if (savedData->buf)
                NPN_MemFree(savedData->buf);
            NPN_MemFree(savedData);
        }

        m_instance->pdata = 0;

        blink::Platform::current()->currentThread()->removeTaskObserver(this);
        delete this;
    }

private:
    NPP_DestroyProcPtr m_destroyFunc;
    NPP m_instance;
};

void WebPluginImpl::stop()
{
    if (!m_isStarted)
        return;

    HashSetStreams streams = m_streams;
    HashSetStreams::iterator end = streams.end();
    for (HashSetStreams::iterator it = streams.begin(); it != end; ++it) {
        PluginStream* stream = *it;
        stream->stop();
        disconnectStream(stream);
    }

    ASSERT(m_streams.isEmpty());

    m_isStarted = false;
    
    // Unsubclass the window
    if (m_isWindowed) {
        WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtr(platformPluginWidget(), GWLP_WNDPROC);

        if (currentWndProc == PluginViewWndProc)
            SetWindowLongPtr(platformPluginWidget(), GWLP_WNDPROC, (LONG_PTR)m_pluginWndProc);
    }

    // Clear the window
    m_npWindow.window = 0;

    if (m_plugin->pluginFuncs()->setwindow && !m_plugin->quirks().contains(PluginQuirkDontSetNullWindowHandleOnDestroy)) {
        WebPluginImpl::setCurrentPluginView(this);
        setCallingPlugin(true);
        m_plugin->pluginFuncs()->setwindow(m_instance, &m_npWindow);
        setCallingPlugin(false);
        WebPluginImpl::setCurrentPluginView(0);
    }

    PluginMainThreadScheduler::scheduler().unregisterPlugin(m_instance);

    // 这里调用destroy会有问题，如果是在_NPN_Evaluate走到这里的话。例子：http://music.yule.sohu.com/20170926/n514522612.shtml
    blink::Platform::current()->currentThread()->addTaskObserver(new DestroyNpTask(m_plugin->pluginFuncs()->destroy, m_instance));
}

void WebPluginImpl::setCurrentPluginView(WebPluginImpl* pluginView)
{
    s_currentPluginView = pluginView;
}

WebPluginImpl* WebPluginImpl::currentPluginView()
{
    return s_currentPluginView;
}

static char* createUTF8String(const String& str)
{
    Vector<char> cstr = WTF::ensureStringToUTF8(str, false);
    const size_t cstrLength = cstr.size();
    char* result = reinterpret_cast<char*>(fastMalloc(cstrLength + 1));

    memcpy(result, cstr.data(), cstrLength);
    result[cstrLength] = '\0';

    return result;
}

void WebPluginImpl::performRequest(PluginRequest* request)
{
    if (!m_isStarted)
        return;

    // don't let a plugin start any loads if it is no longer part of a document that is being 
    // displayed unless the loads are in the same frame as the plugin.
    const AtomicString& targetFrameName = request->frameLoadRequest().frameName();
//     if (m_parentFrame->loader().documentLoader() != m_parentFrame->loader().activeDocumentLoader() && (targetFrameName.isNull() || m_parentFrame->tree().find(targetFrameName) != m_parentFrame))
//         return;

    KURL requestURL = request->frameLoadRequest().resourceRequest().url();
    String jsString = scriptStringIfJavaScriptURL(requestURL);

    Vector<char> requestUrlBuf = ensureStringToUTF8(requestURL.string(), true);
    
    UserGestureIndicator gestureIndicator(request->shouldAllowPopups() ? DefinitelyProcessingUserGesture : PossiblyProcessingUserGesture);

    if (jsString.isNull()) {
        // if this is not a targeted request, create a stream for it. otherwise,
        // just pass it off to the loader
        if (targetFrameName.isEmpty()) {
            PluginStream* stream = PluginStream::create(this, m_parentFrame, request->frameLoadRequest().resourceRequest(), request->sendNotification(), request->notifyData(), plugin()->pluginFuncs(), instance(), m_plugin->quirks());            
            m_streams.add((stream));
            stream->start();
        } else {
            // If the target frame is our frame, we could destroy the
            // WebPluginImpl, so we protect it. <rdar://problem/6991251>
            RefPtr<WebPluginImpl> protect(this);

            FrameLoadRequest frameRequest(m_parentFrame->document(), request->frameLoadRequest().resourceRequest());
            frameRequest.setFrameName(targetFrameName);
            //frameRequest.setShouldCheckNewWindowPolicy(true);
            m_parentFrame->loader().load(frameRequest);

            // FIXME: <rdar://problem/4807469> This should be sent when the document has finished loading
            if (request->sendNotification()) {
                WebPluginImpl::setCurrentPluginView(this);
                //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
                setCallingPlugin(true);
                m_plugin->pluginFuncs()->urlnotify(m_instance, requestUrlBuf.data(), NPRES_DONE, request->notifyData());
                setCallingPlugin(false);
                WebPluginImpl::setCurrentPluginView(0);
            }
        }
        return;
    }

    // Targeted JavaScript requests are only allowed on the frame that contains the JavaScript plugin
    // and this has been made sure in ::load.
    ASSERT(targetFrameName.isEmpty() || m_parentFrame->tree().find(targetFrameName) == m_parentFrame);
    
    // Executing a script can cause the plugin view to be destroyed, so we keep a reference to it.
    RefPtr<WebPluginImpl> protector(this);
    v8::HandleScope handleScope(toIsolate(m_parentFrame));
    blink::ScriptSourceCode jsCode(jsString);
    v8::Local<v8::Value> result = m_parentFrame->script().executeScriptInMainWorldAndReturnValue(jsCode);

    if (targetFrameName.isNull()) {
        String resultString;

        CString cstr;        
        if (result->IsString()) {
            v8::Local<v8::String> v8String = result->ToString();
            resultString = v8StringToWebCoreString<String>(v8String, blink::Externalize);
            cstr = resultString.utf8();
        }

        PluginStream* stream = PluginStream::create(this, m_parentFrame, request->frameLoadRequest().resourceRequest(), request->sendNotification(), request->notifyData(), plugin()->pluginFuncs(), instance(), m_plugin->quirks());
        m_streams.add(stream);
        stream->sendJavaScriptStream(requestURL, cstr);
    }
}

void WebPluginImpl::requestTimerFired(blink::Timer<WebPluginImpl>*)
{
    ASSERT(!m_requests.isEmpty());
    ASSERT(!m_isJavaScriptPaused);

    PassOwnPtr<PluginRequest> request = m_requests[0].release();
    m_requests.remove(0);
    
    // Schedule a new request before calling performRequest since the call to
    // performRequest can cause the plugin view to be deleted.
    if (!m_requests.isEmpty())
        m_requestTimer.startOneShot(0, FROM_HERE);

    performRequest(request.get());
}

void WebPluginImpl::scheduleRequest(PassOwnPtr<PluginRequest> request)
{
    m_requests.append(request);

    if (!m_isJavaScriptPaused)
        m_requestTimer.startOneShot(0, FROM_HERE);
}

NPError WebPluginImpl::load(const FrameLoadRequest& frameLoadRequest, bool sendNotification, void* notifyData)
{
    ASSERT(frameLoadRequest.resourceRequest().httpMethod() == "GET" || frameLoadRequest.resourceRequest().httpMethod() == "POST");

    KURL url = frameLoadRequest.resourceRequest().url();
    
    if (url.isEmpty())
        return NPERR_INVALID_URL;

    // Don't allow requests to be made when the document loader is stopping all loaders.
    DocumentLoader* loader = m_parentFrame->loader().documentLoader();
    if (!loader /*|| loader->isStopping()*/) // TODO
        return NPERR_GENERIC_ERROR;

    const AtomicString& targetFrameName = frameLoadRequest.frameName();
    String jsString = scriptStringIfJavaScriptURL(url);

    if (!jsString.isNull()) {
        // Return NPERR_GENERIC_ERROR if JS is disabled. This is what Mozilla does.
        if (!m_parentFrame->script().canExecuteScripts(NotAboutToExecuteScript))
            return NPERR_GENERIC_ERROR;

        // For security reasons, only allow JS requests to be made on the frame that contains the plug-in.
        if (!targetFrameName.isNull() && m_parentFrame->tree().find(targetFrameName) != m_parentFrame)
            return NPERR_INVALID_PARAM;
    } else if (!m_parentFrame->document()->securityOrigin()->canDisplay(url))
        return NPERR_GENERIC_ERROR;

    scheduleRequest(adoptPtr<PluginRequest>(new PluginRequest(frameLoadRequest, sendNotification, notifyData, arePopupsAllowed())));

    return NPERR_NO_ERROR;
}

static KURL makeURL(const KURL& baseURL, const char* relativeURLString)
{
    String urlString = relativeURLString;

    // Strip return characters.
    urlString.replaceWithLiteral('\n', "");
    urlString.replaceWithLiteral('\r', "");

    return KURL(baseURL, urlString);
}

NPError WebPluginImpl::getURLNotify(const char* url, const char* target, void* notifyData)
{
    FrameLoadRequest frameLoadRequest(m_parentFrame->document());

//     frameLoadRequest.setFrameName(target);
//     frameLoadRequest.resourceRequest().setHTTPMethod("GET");
//     frameLoadRequest.resourceRequest().setURL(makeURL(m_parentFrame->document()->baseURL(), url));
    buildResourceRequest(&frameLoadRequest, m_parentFrame, makeURL(m_parentFrame->document()->baseURL(), url), target);

    return load(frameLoadRequest, true, notifyData);
}

NPError WebPluginImpl::getURL(const char* url, const char* target)
{
    FrameLoadRequest frameLoadRequest(m_parentFrame->document());

    buildResourceRequest(&frameLoadRequest, m_parentFrame, makeURL(m_parentFrame->document()->baseURL(), url), target);

    return load(frameLoadRequest, false, 0);
}

NPError WebPluginImpl::postURLNotify(const char* url, const char* target, uint32_t len, const char* buf, NPBool file, void* notifyData)
{
    return handlePost(url, target, len, buf, file, notifyData, true, true);
}

NPError WebPluginImpl::postURL(const char* url, const char* target, uint32_t len, const char* buf, NPBool file)
{
    // As documented, only allow headers to be specified via NPP_PostURL when using a file.
    return handlePost(url, target, len, buf, file, 0, false, file);
}

NPError WebPluginImpl::newStream(NPMIMEType, const char* /* target */, NPStream**)
{
    notImplemented();
    // Unsupported
    return NPERR_GENERIC_ERROR;
}

int32_t WebPluginImpl::write(NPStream*, int32_t /* len */, void* /* buffer */)
{
    notImplemented();
    // Unsupported
    return -1;
}

NPError WebPluginImpl::destroyStream(NPStream* stream, NPReason reason)
{
    if (!stream || PluginStream::ownerForStream(stream) != m_instance)
        return NPERR_INVALID_INSTANCE_ERROR;

    PluginStream* browserStream = static_cast<PluginStream*>(stream->ndata);
    browserStream->cancelAndDestroyStream(reason);

    return NPERR_NO_ERROR;
}

void WebPluginImpl::status(const char* message)
{
    if (Page* page = m_parentFrame->page())
        page->chromeClient().setStatusbarText(String::fromUTF8(message));
}

NPError WebPluginImpl::setValue(NPPVariable variable, void* value)
{
    //LOG(Plugins, "WebPluginImpl::setValue(%s): ", prettyNameForNPPVariable(variable, value).data());
    switch (variable) {
    case NPPVpluginWindowBool:
        m_isWindowed = value;
        //m_isWindowed = false; // weolar

        return NPERR_NO_ERROR;
    case NPPVpluginTransparentBool:
        m_isTransparent = value;
        return NPERR_NO_ERROR;
    default:
        notImplemented();
        return NPERR_GENERIC_ERROR;
    }
}

void WebPluginImpl::invalidateTimerFired(blink::Timer<WebPluginImpl>*)
{
    for (unsigned i = 0; i < m_invalidRects.size(); i++)
        invalidateRect(m_invalidRects[i]);
    m_invalidRects.clear();
}

void WebPluginImpl::pushPopupsEnabledState(bool state)
{
    m_popupStateStack.append(state);
}
 
void WebPluginImpl::popPopupsEnabledState()
{
    m_popupStateStack.removeLast();
}

bool WebPluginImpl::arePopupsAllowed() const
{
    if (!m_popupStateStack.isEmpty())
        return m_popupStateStack.last();

    return false;
}

void WebPluginImpl::setJavaScriptPaused(bool paused)
{
    if (m_isJavaScriptPaused == paused)
        return;
    m_isJavaScriptPaused = paused;

    if (m_isJavaScriptPaused)
        m_requestTimer.stop();
    else if (!m_requests.isEmpty())
        m_requestTimer.startOneShot(0, FROM_HERE);
}

NPObject* WebPluginImpl::npObject()
{
    NPObject* object = 0;

    if (!m_isStarted || !m_plugin || !m_plugin->pluginFuncs()->getvalue)
        return 0;

    // On Windows, calling Java's NPN_GetValue can allow the message loop to
    // run, allowing loading to take place or JavaScript to run. Protect the
    // WebPluginImpl from destruction. <rdar://problem/6978804>
    RefPtr<WebPluginImpl> protect(this);

    NPError npErr;
    {
        WebPluginImpl::setCurrentPluginView(this);
        //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
        setCallingPlugin(true);
        npErr = m_plugin->pluginFuncs()->getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
        setCallingPlugin(false);
        WebPluginImpl::setCurrentPluginView(0);
    }

    if (npErr != NPERR_NO_ERROR)
        return 0;

    return object;
}

// PassRefPtr<JSC::Bindings::Instance> WebPluginImpl::bindingInstance()
// {
// #if ENABLE(NETSCAPE_PLUGIN_API)
//     NPObject* object = npObject();
//     if (!object)
//         return 0;
// 
//     if (hasOneRef()) {
//         // The renderer for the WebPluginImpl was destroyed during the above call, and
//         // the WebPluginImpl will be destroyed when this function returns, so we
//         // return null.
//         return 0;
//     }
// 
//     RefPtr<JSC::Bindings::RootObject> root = m_parentFrame->script().createRootObject(this);
//     RefPtr<JSC::Bindings::Instance> instance = JSC::Bindings::CInstance::create(object, root.release());
// 
//     _NPN_ReleaseObject(object);
// 
//     return instance.release();
// #else
//     return 0;
// #endif
// }

void WebPluginImpl::disconnectStream(PluginStream* stream)
{
    ASSERT(m_streams.contains(stream));

    m_streams.remove(stream);
}

void WebPluginImpl::setParameters(const WebVector<WebString>& paramNames, const WebVector<WebString>& paramValues)
{
    ASSERT(paramNames.size() == paramValues.size());

    unsigned size = paramNames.size();
    unsigned paramCount = 0;

    m_paramNames = reinterpret_cast<char**>(fastMalloc(sizeof(char*) * size));
    m_paramValues = reinterpret_cast<char**>(fastMalloc(sizeof(char*) * size));

    for (unsigned i = 0; i < size; i++) {
        if (m_plugin->quirks().contains(PluginQuirkRemoveWindowlessVideoParam) && equalIgnoringCase(String(paramNames[i]), "windowlessvideo"))
            continue;

        if (String(paramNames[i]) == "pluginspage")
            m_pluginsPage = paramValues[i];

        m_paramNames[paramCount] = createUTF8String(paramNames[i]);
        m_paramValues[paramCount] = createUTF8String(paramValues[i]);

        paramCount++;
    }

    m_paramCount = paramCount;
}

void WebPluginImpl::focusPluginElement()
{
//     WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
//     if (!container)
//         return;
//     container->focusPlugin();
}

void WebPluginImpl::setCallingPlugin(bool b) const
{
    if (!m_plugin->quirks().contains(PluginQuirkHasModalMessageLoop))
        return;

    if (b)
        ++s_callingPlugin;
    else
        --s_callingPlugin;

    ASSERT(s_callingPlugin >= 0);
}

bool WebPluginImpl::isCallingPlugin()
{
    return s_callingPlugin > 0;
}

void WebPluginImpl::freeStringArray(char** stringArray, int length)
{
    if (!stringArray)
        return;

    for (int i = 0; i < length; i++)
        fastFree(stringArray[i]);

    fastFree(stringArray);
}

static inline bool startsWithBlankLine(const Vector<char>& buffer)
{
    return buffer.size() > 0 && buffer[0] == '\n';
}

static inline int locationAfterFirstBlankLine(const Vector<char>& buffer)
{
    const char* bytes = buffer.data();
    unsigned length = buffer.size();

    for (unsigned i = 0; i < length - 4; i++) {
        // Support for Acrobat. It sends "\n\n".
        if (bytes[i] == '\n' && bytes[i + 1] == '\n')
            return i + 2;
        
        // Returns the position after 2 CRLF's or 1 CRLF if it is the first line.
        if (bytes[i] == '\r' && bytes[i + 1] == '\n') {
            i += 2;
            if (i == 2)
                return i;
            else if (bytes[i] == '\n')
                // Support for Director. It sends "\r\n\n" (3880387).
                return i + 1;
            else if (bytes[i] == '\r' && bytes[i + 1] == '\n')
                // Support for Flash. It sends "\r\n\r\n" (3758113).
                return i + 2;
        }
    }

    return -1;
}

static inline const char* findEOL(const char* bytes, unsigned length)
{
    // According to the HTTP specification EOL is defined as
    // a CRLF pair. Unfortunately, some servers will use LF
    // instead. Worse yet, some servers will use a combination
    // of both (e.g. <header>CRLFLF<body>), so findEOL needs
    // to be more forgiving. It will now accept CRLF, LF or
    // CR.
    //
    // It returns NULL if EOLF is not found or it will return
    // a pointer to the first terminating character.
    for (unsigned i = 0; i < length; i++) {
        if (bytes[i] == '\n')
            return bytes + i;
        if (bytes[i] == '\r') {
            // Check to see if spanning buffer bounds
            // (CRLF is across reads). If so, wait for
            // next read.
            if (i + 1 == length)
                break;

            return bytes + i;
        }
    }

    return 0;
}

static inline AtomicString capitalizeRFC822HeaderFieldName(const String& name)
{
    bool capitalizeCharacter = true;
    String result;

    for (unsigned i = 0; i < name.length(); i++) {
        UChar c;

        if (capitalizeCharacter && name[i] >= 'a' && name[i] <= 'z')
            c = toASCIIUpper(name[i]);
        else if (!capitalizeCharacter && name[i] >= 'A' && name[i] <= 'Z')
            c = toASCIILower(name[i]);
        else
            c = name[i];

        if (name[i] == '-')
            capitalizeCharacter = true;
        else
            capitalizeCharacter = false;

        result.append(c);
    }

    return AtomicString(result);
}

static inline HTTPHeaderMap parseRFC822HeaderFields(const Vector<char>& buffer, unsigned length)
{
    const char* bytes = buffer.data();
    const char* eol;
    AtomicString lastKey;
    HTTPHeaderMap headerFields;

    // Loop ove rlines until we're past the header, or we can't find any more end-of-lines
    while ((eol = findEOL(bytes, length))) {
        const char* line = bytes;
        int lineLength = eol - bytes;
        
        // Move bytes to the character after the terminator as returned by findEOL.
        bytes = eol + 1;
        if ((*eol == '\r') && (*bytes == '\n'))
            bytes++; // Safe since findEOL won't return a spanning CRLF.

        length -= (bytes - line);
        if (lineLength == 0)
            // Blank line; we're at the end of the header
            break;
        else if (*line == ' ' || *line == '\t') {
            // Continuation of the previous header
            if (lastKey.isNull()) {
                // malformed header; ignore it and continue
                continue;
            } else {
                // Merge the continuation of the previous header
                AtomicString currentValue = headerFields.get(lastKey);
                String newValue(line, lineLength);

                headerFields.set(lastKey, currentValue + newValue);
            }
        } else {
            // Brand new header
            const char* colon;
            for (colon = line; *colon != ':' && colon != eol; colon++) {
                // empty loop
            }
            if (colon == eol) 
                // malformed header; ignore it and continue
                continue;
            else {
                lastKey = capitalizeRFC822HeaderFieldName(String(line, colon - line));
                AtomicString value;

                for (colon++; colon != eol; colon++) {
                    if (*colon != ' ' && *colon != '\t')
                        break;
                }
                if (colon == eol)
                    value = "";
                else
                    value = AtomicString::fromUTF8(colon, eol - colon);

                String oldValue = headerFields.get(lastKey);
                if (!oldValue.isNull())
                    value = oldValue + ", " + value;

                headerFields.set(lastKey, value);
            }
        }
    }

    return headerFields;
}

NPError WebPluginImpl::handlePost(const char* url, const char* target, uint32_t len, const char* buf, bool file, void* notifyData, bool sendNotification, bool allowHeaders)
{
    if (!url || !len || !buf)
        return NPERR_INVALID_PARAM;

    FrameLoadRequest frameLoadRequest(m_parentFrame->document());

    HTTPHeaderMap headerFields;
    Vector<char> buffer;
    
    if (file) {
        NPError readResult = handlePostReadFile(buffer, len, buf);
        if(readResult != NPERR_NO_ERROR)
            return readResult;
    } else {
        buffer.resize(len);
        memcpy(buffer.data(), buf, len);
    }

    const char* postData = buffer.data();
    int postDataLength = buffer.size();

    if (allowHeaders) {
        if (startsWithBlankLine(buffer)) {
            postData++;
            postDataLength--;
        } else {
            int location = locationAfterFirstBlankLine(buffer);
            if (location != -1) {
                // If the blank line is somewhere in the middle of the buffer, everything before is the header
                headerFields = parseRFC822HeaderFields(buffer, location);
                unsigned dataLength = buffer.size() - location;

                // Sometimes plugins like to set Content-Length themselves when they post,
                // but WebFoundation does not like that. So we will remove the header
                // and instead truncate the data to the requested length.
                String contentLength = headerFields.get("Content-Length");

                if (!contentLength.isNull())
                    dataLength = min(contentLength.toInt(), (int)dataLength);
                headerFields.remove("Content-Length");

                postData += location;
                postDataLength = dataLength;
            }
        }
    }

    frameLoadRequest.resourceRequest().setHTTPMethod("POST");
    frameLoadRequest.resourceRequest().setURL(makeURL(m_parentFrame->document()->baseURL(), url));
    frameLoadRequest.resourceRequest().addHTTPHeaderFields(headerFields);
    frameLoadRequest.resourceRequest().setHTTPBody(FormData::create(postData, postDataLength));
    frameLoadRequest.setFrameName(target);

    return load(frameLoadRequest, sendNotification, notifyData);
}

void WebPluginImpl::invalidateWindowlessPluginRect(const IntRect& rect)
{
//     if (!isVisible())
//         return;
//     
//     if (!m_element->renderer())
//         return;
//     auto& renderer = downcast<RenderBox>(*m_element->renderer());
//     
//     IntRect dirtyRect = rect;
//     dirtyRect.move(renderer.borderLeft() + renderer.paddingLeft(), renderer.borderTop() + renderer.paddingTop());
//     renderer.repaintRectangle(dirtyRect);

    m_pluginContainer->invalidateRect(rect);
}

void WebPluginImpl::paintMissingPluginIcon(blink::WebCanvas* canvas, const IntRect& rect)
{
    if (!s_nullPluginImage)
        s_nullPluginImage = Image::loadPlatformResource("nullPlugin");

    WebPluginContainerImpl* container = (WebPluginContainerImpl*)m_pluginContainer;
    if (!container)
        return;
    OwnPtr<GraphicsContext> context = GraphicsContext::deprecatedCreateWithCanvas(canvas, GraphicsContext::NothingDisabled);

    IntRect imageRect(container->frameRect().x(), container->frameRect().y(), s_nullPluginImage->width(), s_nullPluginImage->height());

    int xOffset = (container->frameRect().width() - imageRect.width()) / 2;
    int yOffset = (container->frameRect().height() - imageRect.height()) / 2;

    imageRect.move(xOffset, yOffset);

    if (!rect.intersects(imageRect))
        return;

    context->save();
    context->clip(toFrameView(container->parent())->windowClipRect());
    context->drawImage(s_nullPluginImage.get(), imageRect);
    context->restore();
}

static const char* MozillaUserAgent = "Mozilla/5.0 ("
        "Windows; U; Windows NT 5.1;"
        " en-US; rv:1.8.1) Gecko/20061010 Firefox/2.0";

static const char* const ChromeUserAgent = "Mozilla/5.0 ("
    "Windows; U; Windows NT 5.1;"
    " AppleWebKit/534.34 (KHTML, like Gecko) Chrome/19.0.1055.1 Safari/534.34";

const char* WebPluginImpl::userAgent()
{
    if (m_plugin->quirks().contains(PluginQuirkWantsMozillaUserAgent))
        return MozillaUserAgent;
    else if (m_plugin->quirks().contains(PluginQuirkWantsChromeUserAgent))
        return ChromeUserAgent;
    if (m_userAgent.isNull())
        m_userAgent = m_parentFrame->loader().userAgent(m_url).utf8();

    return m_userAgent.data();
}

const char* WebPluginImpl::userAgentStatic()
{
    return MozillaUserAgent;
}

void WebPluginImpl::lifeSupportTimerFired(blink::Timer<WebPluginImpl>*)
{
    deref();
}

void WebPluginImpl::keepAlive()
{
    if (m_lifeSupportTimer.isActive())
        return;

    ref();
    m_lifeSupportTimer.startOneShot(0, FROM_HERE);
}

void WebPluginImpl::keepAlive(NPP instance)
{
    WebPluginImpl* view = instanceMap().get(instance);
    if (!view)
        return;

    view->keepAlive();
}

bool WebPluginImpl::isAlive(NPP instance)
{
    return !!instanceMap().get(instance);
}

NPError WebPluginImpl::getValueStatic(NPNVariable variable, void* value)
{
    //LOG(Plugins, "WebPluginImpl::getValueStatic(%s)", prettyNameForNPNVariable(variable).data());

    NPError result;
    if (platformGetValueStatic(variable, value, &result))
        return result;

    return NPERR_GENERIC_ERROR;
}

NPError WebPluginImpl::getValue(NPNVariable variable, void* value)
{
    // LOG(Plugins, "WebPluginImpl::getValue(%s)", prettyNameForNPNVariable(variable).data());

    NPError result;
    if (platformGetValue(variable, value, &result))
        return result;

    if (platformGetValueStatic(variable, value, &result))
        return result;

    switch (variable) {
    case NPNVWindowNPObject: {
        if (m_isJavaScriptPaused)
            return NPERR_GENERIC_ERROR;

        NPObject* windowScriptObject = m_parentFrame->script().windowScriptNPObject();

        // Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
        if (windowScriptObject)
            _NPN_RetainObject(windowScriptObject);

        void** v = (void**)value;
        *v = windowScriptObject;

        return NPERR_NO_ERROR;
    }

    case NPNVPluginElementNPObject: {
        if (m_isJavaScriptPaused || !m_pluginContainer)
            return NPERR_GENERIC_ERROR;

        NPObject* pluginScriptObject = 0;
        WebElement element = m_pluginContainer->element();
        if (element.hasHTMLTagName(WebString::fromLatin1("applet")) ||
            element.hasHTMLTagName(WebString::fromLatin1("embed")) ||
            element.hasHTMLTagName(WebString::fromLatin1("object"))) {
            pluginScriptObject = m_pluginContainer->scriptableObjectForElement();
        }

        // Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
        if (pluginScriptObject)
            _NPN_RetainObject(pluginScriptObject);

        void** v = (void**)value;
        *v = pluginScriptObject;

        return NPERR_NO_ERROR;
    }

    case NPNVprivateModeBool: {
        Page* page = m_parentFrame->page();
        if (!page)
            return NPERR_GENERIC_ERROR;
        //*((NPBool*)value) = page->usesEphemeralSession();
        *((NPBool*)value) = false; // 是否处于私人模式
        return NPERR_NO_ERROR;
    }

    default:
        return NPERR_GENERIC_ERROR;
    }
}

// static Frame* getFrame(Frame* parentFrame, Element* element)
// {
//     if (parentFrame)
//         return parentFrame;
//     
//     return element->document().frame();
// }

NPError WebPluginImpl::getValueForURL(NPNURLVariable variable, const char* url, char** value, uint32_t* len)
{
    //LOG(Plugins, "WebPluginImpl::getValueForURL(%s)", prettyNameForNPNURLVariable(variable).data());

    NPError result = NPERR_NO_ERROR;

    switch (variable) {
    case NPNURLVCookie: {
        KURL u(m_parentFrame->document()->baseURL(), url);
        if (u.isValid()) {
            //Frame* frame = getFrame(parentFrame(), m_element);
            LocalFrame* frame = parentFrame();
            if (frame) {
                const CString cookieStr(WebCookieJarImpl::inst()->cookies(u, WebURL()).utf8().c_str());
                if (!cookieStr.isNull()) {
                    const int size = cookieStr.length();
                    *value = static_cast<char*>(NPN_MemAlloc(size+1));
                    if (*value) {
                        memset(*value, 0, size+1);
                        memcpy(*value, cookieStr.data(), size+1);
                        if (len)
                            *len = size;
                    } else
                        result = NPERR_OUT_OF_MEMORY_ERROR;
                }
            }
        } else
            result = NPERR_INVALID_URL;
        break;
    }
    case NPNURLVProxy: {
        KURL u(m_parentFrame->document()->baseURL(), url);
        if (u.isValid()) {
//             Frame* frame = getFrame(parentFrame(), m_element);
//             const FrameLoader* frameLoader = frame ? &frame->loader() : 0;
//             const NetworkingContext* context = frameLoader ? frameLoader->networkingContext() : 0;
//             const CString proxyStr = toString(proxyServersForURL(u, context)).utf8();
//             if (!proxyStr.isNull()) {
//                 const int size = proxyStr.length();
//                 *value = static_cast<char*>(NPN_MemAlloc(size+1));
//                 if (*value) {
//                     memset(*value, 0, size+1);
//                     memcpy(*value, proxyStr.data(), size+1);
//                     if (len)
//                         *len = size;
//                 } else
//                     result = NPERR_OUT_OF_MEMORY_ERROR;
//             }
            DebugBreak();
        } else
            result = NPERR_INVALID_URL;
        break;
    }
    default:
        result = NPERR_GENERIC_ERROR;
        //LOG(Plugins, "WebPluginImpl::getValueForURL: %s", prettyNameForNPNURLVariable(variable).data());
        break;
    }

    return result;
}


NPError WebPluginImpl::setValueForURL(NPNURLVariable variable, const char* url, const char* value, uint32_t len)
{
    //LOG(Plugins, "WebPluginImpl::setValueForURL(%s)", prettyNameForNPNURLVariable(variable).data());

    NPError result = NPERR_NO_ERROR;

    switch (variable) {
    case NPNURLVCookie: {
        KURL u(m_parentFrame->document()->baseURL(), url);
        if (u.isValid()) {
            const String cookieStr = String::fromUTF8(value, len);
            //Frame* frame = getFrame(parentFrame(), m_element);
            if (!cookieStr.isEmpty())
                WebCookieJarImpl::inst()->setCookie(u, WebURL(), cookieStr);
        } else
            result = NPERR_INVALID_URL;
        break;
    }
    case NPNURLVProxy:
        //LOG(Plugins, "WebPluginImpl::setValueForURL(%s): Plugins are NOT allowed to set proxy information.", prettyNameForNPNURLVariable(variable).data());
        result = NPERR_GENERIC_ERROR;
        break;
    default:
        //LOG(Plugins, "WebPluginImpl::setValueForURL: %s", prettyNameForNPNURLVariable(variable).data());
        result = NPERR_GENERIC_ERROR;
        break;
    }

    return result;
}

NPError WebPluginImpl::getAuthenticationInfo(const char* protocol, const char* host, int32_t port, const char* /* scheme */, const char* /* realm */, char**  /* username */, uint32_t* /* ulen */, char** /* password */, uint32_t* /* plen */)
{
// #if LOG_DISABLED
//     UNUSED_PARAM(protocol);
//     UNUSED_PARAM(host);
//     UNUSED_PARAM(port);
// #endif
    //LOG(Plugins, "WebPluginImpl::getAuthenticationInfo: protocol=%s, host=%s, port=%d", protocol, host, port);
    notImplemented();
    return NPERR_GENERIC_ERROR;
}

void WebPluginImpl::privateBrowsingStateChanged(bool privateBrowsingEnabled)
{
    NPP_SetValueProcPtr setValue = m_plugin->pluginFuncs()->setvalue;
    if (!setValue)
        return;

    WebPluginImpl::setCurrentPluginView(this);
    //JSC::JSLock::DropAllLocks dropAllLocks(JSDOMWindowBase::commonVM());
    setCallingPlugin(true);
    NPBool value = privateBrowsingEnabled;
    setValue(m_instance, NPNVprivateModeBool, &value);
    setCallingPlugin(false);
    WebPluginImpl::setCurrentPluginView(0);
}

//////////////////////////////////////////////////////////////////////////

bool WebPluginImpl::initialize(blink::WebPluginContainer* pluginContainer)
{
    m_pluginContainer = pluginContainer;
    init();
    return true;
}

void WebPluginImpl::destroy()
{
    ASSERT(1 == refCount());
    deref();
}

void WebPluginImpl::shutdown()
{
    s_nullPluginImage.clear();
}

WebPluginContainer* WebPluginImpl::container() const 
{
    return m_pluginContainer;
}

void WebPluginImpl::containerDidDetachFromParent() { }

NPObject* WebPluginImpl::scriptableObject()
{ 
    return npObject();
}

struct _NPP* WebPluginImpl::pluginNPP()
{
    return m_instance;
}

v8::Local<v8::Object> WebPluginImpl::v8ScriptableObject(v8::Isolate*) 
{
//     if (!m_pluginContainer)
         return v8::Local<v8::Object>();
// 
//     return m_pluginContainer->v8ObjectForElement();
}

bool WebPluginImpl::getFormValue(WebString&)
{
    return false;
}

void WebPluginImpl::didReceiveResponse(const WebURLResponse& response)
{
    if (m_status != PluginStatusLoadedSuccessfully)
        return;

    ASSERT(m_loadManually);
    ASSERT(!m_manualStream);

    m_manualStream = PluginStream::create(this, m_parentFrame, m_parentFrame->loader().documentLoader()->request(), false, 0, plugin()->pluginFuncs(), instance(), m_plugin->quirks());
    m_manualStream->setLoadManually(true);

    m_manualStream->didReceiveResponse(0, response);
}

void WebPluginImpl::didReceiveData(const char* data, int length)
{
    if (m_status != PluginStatusLoadedSuccessfully)
        return;

    ASSERT(m_loadManually);
    ASSERT(m_manualStream);

    m_manualStream->didReceiveData(0, data, length, 0);
}

void WebPluginImpl::didFinishLoading()
{
    if (m_status != PluginStatusLoadedSuccessfully)
        return;

    ASSERT(m_loadManually);
    ASSERT(m_manualStream);

    m_manualStream->didFinishLoading(0, 0, 0);
}

void WebPluginImpl::didFailLoading(const WebURLError& error)
{
    if (m_status != PluginStatusLoadedSuccessfully)
        return;

    ASSERT(m_loadManually);

    if (m_manualStream)
        m_manualStream->didFail(nullptr, error);
}

void WebPluginImpl::didFinishLoadingFrameRequest(const WebURL&, void* notifyData)
{
    DebugBreak();
}

void WebPluginImpl::didFailLoadingFrameRequest(const WebURL&, void* notifyData, const WebURLError&)
{
    DebugBreak();
}

} // namespace content
