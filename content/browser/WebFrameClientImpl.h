
#ifndef WebFrameClientImpl_h
#define WebFrameClientImpl_h

#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebConsoleMessage.h"

namespace cef {
class BrowserHostImpl;
class BrowserImpl;
}

using namespace blink;

namespace content {

class WebPage;
class ContextMenu;

class WebFrameClientImpl : public WebFrameClient {
public:
    WebFrameClientImpl();
    ~WebFrameClientImpl();

    virtual void didAddMessageToConsole(const WebConsoleMessage& message, const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace) override;
    virtual bool shouldReportDetailedMessageForSource(const WebString& source) override { return true; };

    virtual WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType, const WebString& frameName, WebSandboxFlags sandboxFlags) override;

    virtual void frameDetached(WebFrame* child, DetachType) override;

    void loadURLExternally(WebLocalFrame*, const WebURLRequest&, WebNavigationPolicy, const WebString& downloadName) 
    {
        OutputDebugStringA(__FUNCTION__);
    }

    // Factory methods -----------------------------------------------------

    // May return null.
    virtual WebPluginPlaceholder* createPluginPlaceholder(WebLocalFrame*, const WebPluginParams&) override;

    // May return null.
    virtual WebPlugin* createPlugin(WebLocalFrame*, const WebPluginParams&) override;

    // TODO(srirama): Remove this method once chromium updated.
    virtual WebMediaPlayer* createMediaPlayer(WebLocalFrame*, const WebURL&, WebMediaPlayerClient*, WebContentDecryptionModule*) override;

    // May return null.
    // WebContentDecryptionModule* may be null if one has not yet been set.
    virtual WebMediaPlayer* createMediaPlayer(WebLocalFrame*, const WebURL&, WebMediaPlayerClient*, WebMediaPlayerEncryptedMediaClient*, WebContentDecryptionModule*) override;

    // May return null.
    virtual WebApplicationCacheHost* createApplicationCacheHost(WebLocalFrame*, WebApplicationCacheHostClient*) override;

    // May return null.
    virtual WebServiceWorkerProvider* createServiceWorkerProvider(WebLocalFrame* frame) override;

    // May return null.
    virtual WebWorkerContentSettingsClientProxy* createWorkerContentSettingsClientProxy(WebLocalFrame* frame) override;

    // Create a new WebPopupMenu. In the "createExternalPopupMenu" form, the
    // client is responsible for rendering the contents of the popup menu.
    virtual WebExternalPopupMenu* createExternalPopupMenu(const WebPopupMenuInfo&, WebExternalPopupMenuClient*) override;

    // Navigational notifications ------------------------------------------
    // These notifications bracket any loading that occurs in the WebFrame.
    virtual void didStartLoading(bool toDifferentDocument) override;
    virtual void didStopLoading() override;

    // Notification that some progress was made loading the current frame.
    // loadProgress is a value between 0 (nothing loaded) and 1.0 (frame fully
    // loaded).
    virtual void didChangeLoadProgress(double loadProgress) override;

    // A form submission has been requested, but the page's submit event handler
    // hasn't yet had a chance to run (and possibly alter/interrupt the submit.)
    virtual void willSendSubmitEvent(WebLocalFrame*, const WebFormElement&) override;

    // A form submission is about to occur.
    virtual void willSubmitForm(WebLocalFrame*, const WebFormElement&) override;

    // A datasource has been created for a new navigation.  The given
    // datasource will become the provisional datasource for the frame.
    virtual void didCreateDataSource(WebLocalFrame*, WebDataSource*) override;

    // A new provisional load has been started.
    virtual void didStartProvisionalLoad(WebLocalFrame* localFrame, double triggeringEventTime) override;

    // The provisional load was redirected via a HTTP 3xx response.
    virtual void didReceiveServerRedirectForProvisionalLoad(WebLocalFrame*) override;

    // The provisional load failed. The WebHistoryCommitType is the commit type
    // that would have been used had the load succeeded.
    virtual void didFailProvisionalLoad(WebLocalFrame*, const WebURLError&, WebHistoryCommitType) override;

    // The provisional datasource is now committed.  The first part of the
    // response body has been received, and the encoding of the response
    // body is known.
    virtual void didCommitProvisionalLoad(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) override;

    // The frame's document has just been initialized.
    virtual void didCreateNewDocument(WebLocalFrame* frame) override;

    // The window object for the frame has been cleared of any extra
    // properties that may have been set by script from the previously
    // loaded document.
    virtual void didClearWindowObject(WebLocalFrame* frame) override;

    // The document element has been created.
    virtual void didCreateDocumentElement(WebLocalFrame*) override;

    // The page title is available.
    virtual void didReceiveTitle(WebLocalFrame* frame, const WebString& title, WebTextDirection direction) override;

    // The icon for the page have changed.
    virtual void didChangeIcon(WebLocalFrame*, WebIconURL::Type) override;

    // The frame's document finished loading.
    virtual void didFinishDocumentLoad(WebLocalFrame*) override;

    // The 'load' event was dispatched.
    virtual void didHandleOnloadEvents(WebLocalFrame*) override;

    // The frame's document or one of its subresources failed to load. The
    // WebHistoryCommitType is the commit type that would have been used had the
    // load succeeded.
    virtual void didFailLoad(WebLocalFrame*, const WebURLError&, WebHistoryCommitType) override;

    // The frame's document and all of its subresources succeeded to load.
    virtual void didFinishLoad(WebLocalFrame*) override;

    // The navigation resulted in no change to the documents within the page.
    // For example, the navigation may have just resulted in scrolling to a
    // named anchor or a PopState event may have been dispatched.
    virtual void didNavigateWithinPage(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) override;

    // Called upon update to scroll position, document state, and other
    // non-navigational events related to the data held by WebHistoryItem.
    // WARNING: This method may be called very frequently.
    virtual void didUpdateCurrentHistoryItem(WebLocalFrame*) override;

    // The frame's manifest has changed.
    virtual void didChangeManifest(WebLocalFrame*) override;

    // The frame's presentation URL has changed.
    virtual void didChangeDefaultPresentation(WebLocalFrame*) override;

    // The frame's theme color has changed.
    virtual void didChangeThemeColor() override;

    // Called to dispatch a load event for this frame in the FrameOwner of an
    // out-of-process parent frame.
    virtual void dispatchLoad() override;

    // Low-level resource notifications ------------------------------------

    // An element will request a resource.
    virtual void willRequestResource(WebLocalFrame*, const WebCachedURLRequest&) override;

    // A request is about to be sent out, and the client may modify it.  Request
    // is writable, and changes to the URL, for example, will change the request
    // made.  If this request is the result of a redirect, then redirectResponse
    // will be non-null and contain the response that triggered the redirect.
    virtual void willSendRequest(
        WebLocalFrame*, unsigned identifier, WebURLRequest&,
        const WebURLResponse& redirectResponse) override;

    // Response headers have been received for the resource request given
    // by identifier.
    virtual void didReceiveResponse(WebLocalFrame* webFrame, unsigned identifier, const WebURLResponse&) override;

    virtual void didChangeResourcePriority(WebLocalFrame* webFrame, unsigned identifier, const WebURLRequest::Priority& priority, int) override;

    virtual void didDispatchPingLoader(WebLocalFrame* webFrame, const WebURL& url) override;

    // Navigational queries ------------------------------------------------
    virtual WebNavigationPolicy decidePolicyForNavigation(const NavigationPolicyInfo& info) override;

    // Services ------------------------------------------------------------

    // A frame specific cookie jar.  May return null, in which case
    // WebKitPlatformSupport::cookieJar() will be called to access cookies.
    virtual WebCookieJar* cookieJar(WebLocalFrame*) override;

    // Dialogs -------------------------------------------------------------

    virtual void runModalAlertDialog(const WebString& message) override;

    // Displays a modal confirmation dialog with the given message as
    // description and OK/Cancel choices. Returns true if the user selects
    // 'OK' or false otherwise.
    virtual bool runModalConfirmDialog(const WebString& message) override;

    // Displays a modal input dialog with the given message as description
    // and OK/Cancel choices. The input field is pre-filled with
    // defaultValue. Returns true if the user selects 'OK' or false
    // otherwise. Upon returning true, actualValue contains the value of
    // the input field.
    virtual bool runModalPromptDialog(const WebString& message, const WebString& defaultValue, WebString* actualValue) override;

    // Displays a modal confirmation dialog containing the given message as
    // description and OK/Cancel choices, where 'OK' means that it is okay
    // to proceed with closing the view. Returns true if the user selects
    // 'OK' or false otherwise.
    virtual bool runModalBeforeUnloadDialog(bool isReload, const WebString& message) override;

    // UI ------------------------------------------------------------------

    // Shows a context menu with commands relevant to a specific element on
    // the given frame. Additional context data is supplied.
    virtual void showContextMenu(const WebContextMenuData&) override;

    // Called when the data attached to the currently displayed context menu is
    // invalidated. The context menu may be closed if possible.
    virtual void clearContextMenu() override;

    // Script notifications ------------------------------------------------

    // Notifies that a new script context has been created for this frame.
    // This is similar to didClearWindowObject but only called once per
    // frame context.
    virtual void didCreateScriptContext(WebLocalFrame*, v8::Local<v8::Context>, int extensionGroup, int worldId) override;

    // WebKit is about to release its reference to a v8 context for a frame.
    virtual void willReleaseScriptContext(WebLocalFrame*, v8::Local<v8::Context>, int worldId) override;

    //////////////////////////////////////////////////////////////////////////
    void setWebPage(WebPage* webPage);
    WebPage* webPage();
    //////////////////////////////////////////////////////////////////////////

    bool isLoading() const { return m_loading; }
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    // for wke
    bool isLoaded() const { return m_loaded; }
    bool isLoadFailed() const { return m_loadFailed; }
    bool isDocumentReady() const { return m_documentReady; }

    String title() const { return m_title; }
#endif
private:
    void resetLoadState();
    void onLoadingStateChange(bool isLoading, bool toDifferentDocument);

    WebPage* m_webPage;
    bool m_loading;

    bool m_loadFailed;
    bool m_loaded;
    bool m_documentReady;
    String m_title;
    WTF::Vector<WebFrame*> m_unusedFrames;

    ContextMenu* m_menu;
};

} // namespace blink

#endif // WebFrameClientImpl_h