/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2009 Girish Ramakrishnan <girish@forwardbias.in>
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

#ifndef WebPluginImpl_h
#define WebPluginImpl_h

#include "third_party/WebKit/public/web/WebPlugin.h"

#include "content/web_impl_win/npapi/PluginPackage.h"
#include "content/web_impl_win/npapi/PluginStream.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebPluginParams.h"
#include "third_party/WebKit/Source/core/loader/FrameLoadRequest.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/platform/heap/Handle.h"
#include "third_party/WebKit/public/platform/WebCanvas.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"
#include "third_party/WebKit/Source/wtf/Noncopyable.h"
#include "third_party/WebKit/Source/wtf/FastAllocBase.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/HashFunctions.h"
#include "third_party/npapi/bindings/npruntime.h"
#include <v8.h>
#include <windows.h>

typedef HWND PlatformWidget;
typedef PlatformWidget PlatformPluginWidget;

namespace wke {
class CWebView;
}

namespace blink {
class WebPluginContainer;
class GraphicsContext;
class WebViewClient;
class WebMouseEvent;
class WebKeyboardEvent;
}

namespace content {

enum PluginStatus {
    PluginStatusCanNotFindPlugin,
    PluginStatusCanNotLoadPlugin,
    PluginStatusLoadedSuccessfully
};

class PluginMessageThrottlerWin;

class PluginRequest {
    WTF_MAKE_NONCOPYABLE(PluginRequest); WTF_MAKE_FAST_ALLOCATED(PluginRequest);
public:
    PluginRequest(const blink::FrameLoadRequest& frameLoadRequest, bool sendNotification, void* notifyData, bool shouldAllowPopups)
        : m_frameLoadRequest(frameLoadRequest)
        , m_notifyData(notifyData)
        , m_sendNotification(sendNotification)
        , m_shouldAllowPopups(shouldAllowPopups) { }
public:
    const blink::FrameLoadRequest& frameLoadRequest() const { return m_frameLoadRequest; }
    void* notifyData() const { return m_notifyData; }
    bool sendNotification() const { return m_sendNotification; }
    bool shouldAllowPopups() const { return m_shouldAllowPopups; }
private:
    blink::FrameLoadRequest m_frameLoadRequest;
    void* m_notifyData;
    bool m_sendNotification;
    bool m_shouldAllowPopups;
};

class WebPluginImpl : public RefCounted<WebPluginImpl>, public blink::WebPlugin, private PluginStreamClient {
public:
    WebPluginImpl(blink::WebLocalFrame* parentFrame, const blink::WebPluginParams&);
    virtual ~WebPluginImpl();

    virtual bool initialize(blink::WebPluginContainer*);
    virtual void destroy() override;

    static void shutdown();

    virtual blink::WebPluginContainer* container() const override;
    virtual void containerDidDetachFromParent() override;

    virtual NPObject* scriptableObject() override;
    virtual struct _NPP* pluginNPP() override;

    // The same as scriptableObject() but allows to expose scriptable interface
    // through plain v8 object instead of NPObject.
    // If you override this function, you must return nullptr in scriptableObject().
    virtual v8::Local<v8::Object> v8ScriptableObject(v8::Isolate*) override;

    // Returns true if the form submission value is successfully obtained
    // from the plugin. The value would be associated with the name attribute
    // of the corresponding object element.
    virtual bool getFormValue(blink::WebString&) override;
    virtual bool supportsKeyboardFocus() const override;
    virtual bool supportsEditCommands() const override;
    // Returns true if this plugin supports input method, which implements
    // setComposition() and confirmComposition() below.
    virtual bool supportsInputMethod() const override { return false; }

    virtual bool canProcessDrag() const override { return false; }

    // TODO(schenney): Make these pure virtual when chromium changes land
    virtual void layoutIfNeeded() override { }
    virtual void paint(blink::WebCanvas*, const blink::WebRect&) override;

    // Coordinates are relative to the containing window.
    virtual void updateGeometry(
        const blink::WebRect& windowRect, const blink::WebRect& clipRect,
        const blink::WebRect& unobscuredRect, const blink::WebVector<blink::WebRect>& cutOutsRects,
        bool isVisible) override;

    virtual void updateFocus(bool focused, blink::WebFocusType) override;

    virtual void updateVisibility(bool) override;

    virtual bool acceptsInputEvents() override;
    virtual bool handleInputEvent(const blink::WebInputEvent&, blink::WebCursorInfo&) override;

    virtual bool handleDragStatusUpdate(blink::WebDragStatus, const blink::WebDragData&, blink::WebDragOperationsMask,
        const blink::WebPoint& position, const blink::WebPoint& screenPosition) { return false; }

    virtual void didReceiveResponse(const blink::WebURLResponse&);
    virtual void didReceiveData(const char* data, int dataLength) override;
    virtual void didFinishLoading() override;
    virtual void didFailLoading(const blink::WebURLError&) override;

    virtual void didFinishLoadingFrameRequest(const blink::WebURL&, void* notifyData) override;
    virtual void didFailLoadingFrameRequest(const blink::WebURL&, void* notifyData, const blink::WebURLError&) override;

    virtual bool supportsPaginatedPrint() { return false; }
    virtual bool isPrintScalingDisabled() { return false; }
    virtual bool getPrintPresetOptionsFromDocument(blink::WebPrintPresetOptions*) { return false; }

    virtual int printBegin(const blink::WebPrintParams& printParams) override;
    virtual void printPage(int pageNumber, blink::WebCanvas* canvas) override;
    virtual void printEnd() override;

    virtual bool hasSelection() const { return false; }
    virtual blink::WebString selectionAsText() const { return blink::WebString(); }
    virtual blink::WebString selectionAsMarkup() const { return blink::WebString(); }

    virtual bool executeEditCommand(const blink::WebString& name) { return false; }
    virtual bool executeEditCommand(const blink::WebString& name, const blink::WebString& value) { return false; }

    virtual bool setComposition(const blink::WebString& text, const blink::WebVector<blink::WebCompositionUnderline>& underlines, int selectionStart, int selectionEnd) { return false; }
    virtual bool confirmComposition(const blink::WebString& text, blink::WebWidget::ConfirmCompositionBehavior selectionBehavior) { return false; }
    virtual void extendSelectionAndDelete(int before, int after) { }
    virtual blink::WebURL linkAtPosition(const blink::WebPoint& position) const { return blink::WebURL(); }

    virtual void setZoomLevel(double level, bool textOnly) { }

    virtual bool startFind(const blink::WebString& searchText, bool caseSensitive, int identifier) { return false; }
    virtual void selectFindResult(bool forward) { }
    virtual void stopFind() { }

    virtual bool canRotateView() { return false; }
    virtual void rotateView(RotationType type) { }

    virtual bool isPlaceholder() { return true; }
    virtual bool shouldPersist() const { return false; }

    PluginPackage* plugin() const { return m_plugin.get(); }

    NPP instance() const { return m_instance; }

    void setNPWindowRect(const blink::IntRect&);
    static WebPluginImpl* currentPluginView();

    NPObject* npObject();

    //virtual PassRefPtr<JSC::Bindings::Instance> bindingInstance() override;

    PluginStatus status() const { return m_status; }

    // NPN functions
    NPError getURLNotify(const char* url, const char* target, void* notifyData);
    NPError getURL(const char* url, const char* target);
    NPError postURLNotify(const char* url, const char* target, uint32_t len, const char* but, NPBool file, void* notifyData);
    NPError postURL(const char* url, const char* target, uint32_t len, const char* but, NPBool file);
    NPError newStream(NPMIMEType type, const char* target, NPStream** stream);
    int32_t write(NPStream* stream, int32_t len, void* buffer);
    NPError destroyStream(NPStream* stream, NPReason reason);

    const char* userAgent();
    static const char* userAgentStatic();
    void status(const char* message);
    
    NPError getValue(NPNVariable variable, void* value);
    static NPError getValueStatic(NPNVariable variable, void* value);
    NPError setValue(NPPVariable variable, void* value);
    NPError getValueForURL(NPNURLVariable variable, const char* url, char** value, uint32_t* len);
    NPError setValueForURL(NPNURLVariable variable, const char* url, const char* value, uint32_t len);
    NPError getAuthenticationInfo(const char* protocol, const char* host, int32_t port, const char* scheme, const char* realm, char** username, uint32_t* ulen, char** password, uint32_t* plen);
    void invalidateRect(NPRect*);
    void invalidateRegion(NPRegion);

    void forceRedraw();
    void pushPopupsEnabledState(bool state);
    void popPopupsEnabledState();

    bool handleMouseEvent(const blink::WebMouseEvent& evt);
    bool handleKeyboardEvent(const blink::WebKeyboardEvent& evt);

    void invalidateRect(const blink::IntRect&);

    bool arePopupsAllowed() const;

    void setJavaScriptPaused(bool);

    void privateBrowsingStateChanged(bool);

    void disconnectStream(PluginStream*);
    void streamDidFinishLoading(PluginStream* stream) { disconnectStream(stream); }

    blink::LocalFrame* parentFrame() const { return m_parentFrame; }

    void focusPluginElement();

    const String& pluginsPage() const { return m_pluginsPage; }
    const String& mimeType() const { return m_mimeType; }
    const blink::KURL& url() const { return m_url; }

    static LRESULT CALLBACK PluginViewWndProc(HWND, UINT, WPARAM, LPARAM);
    LRESULT wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    WNDPROC pluginWndProc() const { return m_pluginWndProc; }

    static bool isCallingPlugin();

    bool start();

    static void keepAlive(NPP);
    static bool isAlive(NPP);

    void keepAlive();

    void setPlatformPluginWidget(PlatformPluginWidget widget)
    {
        if (widget != m_widget) {
            m_widget = widget;
        }
    }

    PlatformPluginWidget platformPluginWidget() const 
    { 
        return m_widget;
    }

    void setParentPlatformPluginWidget(PlatformWidget widget)
    {
        if (widget != m_parentWidget)
            m_parentWidget = widget;
    }

    PlatformPluginWidget parentPlatformPluginWidget() const
    {
        return m_parentWidget;
    }
    
    void setWkeWebView(wke::CWebView* wkeWebview) { m_wkeWebview = wkeWebview; }
    wke::CWebView* getWkeWebView() { return m_wkeWebview; }

    void setHwndRenderOffset(const blink::IntPoint& offset)
    {
        m_widgetOffset = offset;
    }

private:
    void findVirtualPluginByMime();

    void setParameters(const blink::WebVector<blink::WebString>& paramNames, const blink::WebVector<blink::WebString>& paramValues);
    bool startOrAddToUnstartedList();
    void init();
    bool platformStart();
    void stop();
    void platformDestroy();
    static void setCurrentPluginView(WebPluginImpl*);

    NPError load(const blink::FrameLoadRequest&, bool sendNotification, void* notifyData);
    NPError handlePost(const char* url, const char* target, uint32_t len, const char* buf, bool file, void* notifyData, bool sendNotification, bool allowHeaders);
    NPError handlePostReadFile(Vector<char>& buffer, uint32_t len, const char* buf);

    static void freeStringArray(char** stringArray, int length);
    void setCallingPlugin(bool) const;

    void invalidateWindowlessPluginRect(const blink::IntRect&);

    virtual void mediaCanStart();

    void paintWindowedPluginIntoContext(blink::GraphicsContext&, const blink::IntRect&);
    static HDC WINAPI hookedBeginPaint(HWND, PAINTSTRUCT*);
    static BOOL WINAPI hookedEndPaint(HWND, const PAINTSTRUCT*);

    static bool platformGetValueStatic(NPNVariable variable, void* value, NPError* result);
    bool platformGetValue(NPNVariable variable, void* value, NPError* result);

    blink::WebPluginContainer* m_pluginContainer;

    blink::LocalFrame* m_parentFrame;
    RefPtr<PluginPackage> m_plugin;
    bool m_isStarted;
    blink::KURL m_url;
    PluginStatus m_status;
    Vector<blink::IntRect> m_invalidRects;

    void performRequest(PluginRequest*);
    void scheduleRequest(PassOwnPtr<PluginRequest>);
    void requestTimerFired(blink::Timer<WebPluginImpl>*);
    void invalidateTimerFired(blink::Timer<WebPluginImpl>*);
    void platformStartImpl(bool isSync);
    blink::Timer<WebPluginImpl> m_requestTimer;
    blink::Timer<WebPluginImpl> m_invalidateTimer;

    class PlatformStartAsynTask : public blink::WebThread::TaskObserver {
    public:
        PlatformStartAsynTask(WebPluginImpl* parentPtr)
            : m_parentPtr(parentPtr) {}

        virtual ~PlatformStartAsynTask() override {}
        virtual void willProcessTask() override {}
        virtual void didProcessTask() override;

        void onParentDestroy() { m_parentPtr = nullptr; }

    private:
        WebPluginImpl* m_parentPtr;
    };
    PlatformStartAsynTask* m_asynStartTask;

    friend class PlatformStartAsynTask;

    void asynSetPlatformPluginWidgetVisibilityTimerFired(blink::Timer<WebPluginImpl>*);
    blink::Timer<WebPluginImpl> m_setPlatformPluginWidgetVisibilityTimer;
    
    void popPopupsStateTimerFired(blink::Timer<WebPluginImpl>*);
    blink::Timer<WebPluginImpl> m_popPopupsStateTimer;

    void lifeSupportTimerFired(blink::Timer<WebPluginImpl>*);
    blink::Timer<WebPluginImpl> m_lifeSupportTimer;

    bool dispatchNPEvent(NPEvent&);

    void updatePluginWidget(const blink::IntRect& windowRect, const blink::IntRect& clipRect);
    void paintMissingPluginIcon(blink::WebCanvas*, const blink::IntRect&);

    void paintIntoTransformedContext(HDC);
    PassRefPtr<blink::Image> snapshot();

    int m_mode;
    int m_paramCount;
    char** m_paramNames;
    char** m_paramValues;
    String m_pluginsPage;

    String m_mimeType;
    WTF::CString m_userAgent;

    NPP m_instance;
    //NPP_t m_instanceStruct;
    NPWindow m_npWindow;

    Vector<bool, 4> m_popupStateStack;

    typedef WTF::HashSet<PluginStream*> HashSetStreams;
    HashSetStreams m_streams;
    Vector<OwnPtr<PluginRequest>> m_requests;

    bool m_isWindowed;
    bool m_isTransparent;
    bool m_haveInitialized;
    bool m_isWaitingToStart;

    OwnPtr<PluginMessageThrottlerWin> m_messageThrottler;
    WNDPROC m_pluginWndProc;
    unsigned m_lastMessage;
    bool m_isCallingPluginWndProc;
    HDC m_wmPrintHDC;
    bool m_haveUpdatedPluginWidget;

private:
    blink::IntRect m_clipRect; // The clip rect to apply to a windowed plug-in
    blink::IntRect m_windowRect; // Our window rect.

    bool m_loadManually;
    PluginStream* m_manualStream;

    bool m_isJavaScriptPaused;

    bool m_haveCalledSetWindow;

    PlatformWidget m_widget;
    PlatformWidget m_parentWidget;
    blink::IntPoint m_widgetOffset;

    static WebPluginImpl* s_currentPluginView;

    SkCanvas* m_memoryCanvas;
    wke::CWebView* m_wkeWebview;
};

} // namespace content

#endif // WebPluginImpl_h
