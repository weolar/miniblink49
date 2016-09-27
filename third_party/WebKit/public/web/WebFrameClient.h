/*
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebFrameClient_h
#define WebFrameClient_h

#include "../platform/WebColor.h"
#include "WebAXObject.h"
#include "WebDOMMessageEvent.h"
#include "WebDataSource.h"
#include "WebFrame.h"
#include "WebHistoryCommitType.h"
#include "WebHistoryItem.h"
#include "WebIconURL.h"
#include "WebNavigationPolicy.h"
#include "WebNavigationType.h"
#include "WebNavigatorContentUtilsClient.h"
#include "WebSandboxFlags.h"
#include "WebTextDirection.h"
#include "public/platform/WebCommon.h"
#include "public/platform/WebFileSystem.h"
#include "public/platform/WebFileSystemType.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebStorageQuotaCallbacks.h"
#include "public/platform/WebStorageQuotaType.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebURLRequest.h"
#include <v8.h>

namespace blink {

enum class WebTreeScopeType;
class WebApplicationCacheHost;
class WebApplicationCacheHostClient;
class WebAppBannerClient;
class WebCachedURLRequest;
class WebColorChooser;
class WebColorChooserClient;
class WebContentDecryptionModule;
class WebCookieJar;
class WebDataSource;
class WebEncryptedMediaClient;
class WebExternalPopupMenu;
class WebExternalPopupMenuClient;
class WebFormElement;
class WebGeolocationClient;
class WebMediaPlayer;
class WebMediaPlayerClient;
class WebMediaPlayerEncryptedMediaClient;
class WebMIDIClient;
class WebNotificationPermissionCallback;
class WebPermissionClient;
class WebServiceWorkerProvider;
class WebSocketHandle;
class WebPlugin;
class WebPluginPlaceholder;
class WebPresentationClient;
class WebPushClient;
class WebRTCPeerConnectionHandler;
class WebScreenOrientationClient;
class WebString;
class WebURL;
class WebURLResponse;
class WebUserMediaClient;
class WebVRClient;
class WebWorkerContentSettingsClientProxy;
struct WebColorSuggestion;
struct WebConsoleMessage;
struct WebContextMenuData;
struct WebPluginParams;
struct WebPopupMenuInfo;
struct WebRect;
struct WebURLError;

class WebFrameClient {
public:
    // Factory methods -----------------------------------------------------

    // May return null.
    virtual WebPluginPlaceholder* createPluginPlaceholder(WebLocalFrame*, const WebPluginParams&) { return 0; }

    // May return null.
    virtual WebPlugin* createPlugin(WebLocalFrame*, const WebPluginParams&) { return 0; }

    // TODO(srirama): Remove this method once chromium updated.
    virtual WebMediaPlayer* createMediaPlayer(WebLocalFrame*, const WebURL&, WebMediaPlayerClient*, WebContentDecryptionModule*) { return 0; }

    // May return null.
    // WebContentDecryptionModule* may be null if one has not yet been set.
    virtual WebMediaPlayer* createMediaPlayer(WebLocalFrame*, const WebURL&, WebMediaPlayerClient*, WebMediaPlayerEncryptedMediaClient*, WebContentDecryptionModule*) { return 0; }

    // May return null.
    virtual WebApplicationCacheHost* createApplicationCacheHost(WebLocalFrame*, WebApplicationCacheHostClient*) { return 0; }

    // May return null.
    virtual WebServiceWorkerProvider* createServiceWorkerProvider(WebLocalFrame* frame) { return 0; }

    // May return null.
    virtual WebWorkerContentSettingsClientProxy* createWorkerContentSettingsClientProxy(WebLocalFrame* frame) { return 0; }

    // Create a new WebPopupMenu. In the "createExternalPopupMenu" form, the
    // client is responsible for rendering the contents of the popup menu.
    virtual WebExternalPopupMenu* createExternalPopupMenu(
        const WebPopupMenuInfo&, WebExternalPopupMenuClient*) { return 0; }


    // Services ------------------------------------------------------------

    // A frame specific cookie jar.  May return null, in which case
    // WebKitPlatformSupport::cookieJar() will be called to access cookies.
    virtual WebCookieJar* cookieJar(WebLocalFrame*) { return 0; }


    // General notifications -----------------------------------------------

    // Indicates if creating a plugin without an associated renderer is supported.
    virtual bool canCreatePluginWithoutRenderer(const WebString& mimeType) { return false; }

    // Indicates that another page has accessed the DOM of the initial empty
    // document of a main frame. After this, it is no longer safe to show a
    // pending navigation's URL, because a URL spoof is possible.
    virtual void didAccessInitialDocument(WebLocalFrame*) { }

    // A child frame was created in this frame. This is called when the frame
    // is created and initialized. Takes the name of the new frame, the parent
    // frame and returns a new WebFrame. The WebFrame is considered in-use
    // until frameDetached() is called on it.
    // Note: If you override this, you should almost certainly be overriding
    // frameDetached().
    virtual WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType, const WebString& frameName, WebSandboxFlags sandboxFlags) { return nullptr; }

    // This frame set its opener to null, disowning it.
    // See http://html.spec.whatwg.org/#dom-opener.
    virtual void didDisownOpener(WebLocalFrame*) { }

    // Specifies the reason for the detachment.
    enum class DetachType { Remove, Swap };

    // This frame has been detached from the view, but has not been closed yet.
    virtual void frameDetached(WebFrame*, DetachType) { }

    // This frame has become focused..
    virtual void frameFocused() { }

    // This frame is about to be closed. This is called after frameDetached,
    // when the document is being unloaded, due to new one committing.
    virtual void willClose(WebFrame*) { }

    // This frame's name has changed.
    virtual void didChangeName(WebLocalFrame*, const WebString&) { }

    // The sandbox flags have changed for a child frame of this frame.
    virtual void didChangeSandboxFlags(WebFrame* childFrame, WebSandboxFlags flags) { }

    // Called when a watched CSS selector matches or stops matching.
    virtual void didMatchCSS(WebLocalFrame*, const WebVector<WebString>& newlyMatchingSelectors, const WebVector<WebString>& stoppedMatchingSelectors) { }


    // Console messages ----------------------------------------------------

    // Whether or not we should report a detailed message for the given source.
    virtual bool shouldReportDetailedMessageForSource(const WebString& source) { return false; }

    // A new message was added to the console.
    virtual void didAddMessageToConsole(const WebConsoleMessage&, const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace) { }


    // Load commands -------------------------------------------------------

    // The client should handle the navigation externally.
    virtual void loadURLExternally(
        WebLocalFrame*, const WebURLRequest&, WebNavigationPolicy, const WebString& downloadName) { }


    // Navigational queries ------------------------------------------------

    // The client may choose to alter the navigation policy.  Otherwise,
    // defaultPolicy should just be returned.

    struct NavigationPolicyInfo {
        WebLocalFrame* frame;
        WebDataSource::ExtraData* extraData;

        // Note: if browser side navigations are enabled, the client may modify
        // the urlRequest. However, should this happen, the client should change
        // the WebNavigationPolicy to WebNavigationPolicyIgnore, and the load
        // should stop in blink. In all other cases, the urlRequest should not
        // be modified.
        WebURLRequest& urlRequest;
        WebNavigationType navigationType;
        WebNavigationPolicy defaultPolicy;
        bool isRedirect;

        NavigationPolicyInfo(WebURLRequest& urlRequest)
            : frame(0)
            , extraData(0)
            , urlRequest(urlRequest)
            , navigationType(WebNavigationTypeOther)
            , defaultPolicy(WebNavigationPolicyIgnore)
            , isRedirect(false) { }
    };

    virtual WebNavigationPolicy decidePolicyForNavigation(const NavigationPolicyInfo& info)
    {
        return decidePolicyForNavigation(info.frame, info.extraData, info.urlRequest, info.navigationType, info.defaultPolicy, info.isRedirect);
    }

    // DEPRECATED
    virtual WebNavigationPolicy decidePolicyForNavigation(
        WebLocalFrame*, WebDataSource::ExtraData*, const WebURLRequest&, WebNavigationType,
        WebNavigationPolicy defaultPolicy, bool isRedirect) { return defaultPolicy; }

    // During a history navigation, we may choose to load new subframes from history as well.
    // This returns such a history item if appropriate.
    virtual WebHistoryItem historyItemForNewChildFrame(WebFrame*) { return WebHistoryItem(); }


    // Navigational notifications ------------------------------------------

    // These notifications bracket any loading that occurs in the WebFrame.
    virtual void didStartLoading(bool toDifferentDocument) { }
    virtual void didStopLoading() { }

    // Notification that some progress was made loading the current frame.
    // loadProgress is a value between 0 (nothing loaded) and 1.0 (frame fully
    // loaded).
    virtual void didChangeLoadProgress(double loadProgress) { }

    // A form submission has been requested, but the page's submit event handler
    // hasn't yet had a chance to run (and possibly alter/interrupt the submit.)
    virtual void willSendSubmitEvent(WebLocalFrame*, const WebFormElement&) { }

    // A form submission is about to occur.
    virtual void willSubmitForm(WebLocalFrame*, const WebFormElement&) { }

    // A datasource has been created for a new navigation.  The given
    // datasource will become the provisional datasource for the frame.
    virtual void didCreateDataSource(WebLocalFrame*, WebDataSource*) { }

    // A new provisional load has been started.
    virtual void didStartProvisionalLoad(WebLocalFrame* localFrame, double triggeringEventTime) { }

    // The provisional load was redirected via a HTTP 3xx response.
    virtual void didReceiveServerRedirectForProvisionalLoad(WebLocalFrame*) { }

    // The provisional load failed. The WebHistoryCommitType is the commit type
    // that would have been used had the load succeeded.
    virtual void didFailProvisionalLoad(WebLocalFrame*, const WebURLError&, WebHistoryCommitType) { }

    // The provisional datasource is now committed.  The first part of the
    // response body has been received, and the encoding of the response
    // body is known.
    virtual void didCommitProvisionalLoad(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) { }

    // The frame's document has just been initialized.
    virtual void didCreateNewDocument(WebLocalFrame* frame) { }

    // The window object for the frame has been cleared of any extra
    // properties that may have been set by script from the previously
    // loaded document.
    virtual void didClearWindowObject(WebLocalFrame* frame) { }

    // The document element has been created.
    virtual void didCreateDocumentElement(WebLocalFrame*) { }

    // The page title is available.
    virtual void didReceiveTitle(WebLocalFrame* frame, const WebString& title, WebTextDirection direction) { }

    // The icon for the page have changed.
    virtual void didChangeIcon(WebLocalFrame*, WebIconURL::Type) { }

    // The frame's document finished loading.
    virtual void didFinishDocumentLoad(WebLocalFrame*) { }

    // The 'load' event was dispatched.
    virtual void didHandleOnloadEvents(WebLocalFrame*) { }

    // The frame's document or one of its subresources failed to load. The
    // WebHistoryCommitType is the commit type that would have been used had the
    // load succeeded.
    virtual void didFailLoad(WebLocalFrame*, const WebURLError&, WebHistoryCommitType) { }

    // The frame's document and all of its subresources succeeded to load.
    virtual void didFinishLoad(WebLocalFrame*) { }

    // The navigation resulted in no change to the documents within the page.
    // For example, the navigation may have just resulted in scrolling to a
    // named anchor or a PopState event may have been dispatched.
    virtual void didNavigateWithinPage(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) { }

    // Called upon update to scroll position, document state, and other
    // non-navigational events related to the data held by WebHistoryItem.
    // WARNING: This method may be called very frequently.
    virtual void didUpdateCurrentHistoryItem(WebLocalFrame*) { }

    // The frame's manifest has changed.
    virtual void didChangeManifest(WebLocalFrame*) { }

    // The frame's presentation URL has changed.
    virtual void didChangeDefaultPresentation(WebLocalFrame*) { }

    // The frame's theme color has changed.
    virtual void didChangeThemeColor() { }

    // Called to dispatch a load event for this frame in the FrameOwner of an
    // out-of-process parent frame.
    virtual void dispatchLoad() { }

    // Web Notifications ---------------------------------------------------

    // Requests permission to display platform notifications on the origin of this frame.
    virtual void requestNotificationPermission(const WebSecurityOrigin&, WebNotificationPermissionCallback* callback) { }


    // Push API ---------------------------------------------------

    // Used to access the embedder for the Push API.
    virtual WebPushClient* pushClient() { return 0; }


    // Presentation API ----------------------------------------------------

    // Used to access the embedder for the Presentation API.
    virtual WebPresentationClient* presentationClient() { return 0; }


    // Editing -------------------------------------------------------------

    // These methods allow the client to intercept and overrule editing
    // operations.
    virtual void didChangeSelection(bool isSelectionEmpty) { }


    // Dialogs -------------------------------------------------------------

    // This method opens the color chooser and returns a new WebColorChooser
    // instance. If there is a WebColorChooser already from the last time this
    // was called, it ends the color chooser by calling endChooser, and replaces
    // it with the new one. The given list of suggestions can be used to show a
    // simple interface with a limited set of choices.

    virtual WebColorChooser* createColorChooser(
        WebColorChooserClient*,
        const WebColor&,
        const WebVector<WebColorSuggestion>&) { return 0; }

    // Displays a modal alert dialog containing the given message. Returns
    // once the user dismisses the dialog.
    virtual void runModalAlertDialog(const WebString& message) { }

    // Displays a modal confirmation dialog with the given message as
    // description and OK/Cancel choices. Returns true if the user selects
    // 'OK' or false otherwise.
    virtual bool runModalConfirmDialog(const WebString& message) { return false; }

    // Displays a modal input dialog with the given message as description
    // and OK/Cancel choices. The input field is pre-filled with
    // defaultValue. Returns true if the user selects 'OK' or false
    // otherwise. Upon returning true, actualValue contains the value of
    // the input field.
    virtual bool runModalPromptDialog(
        const WebString& message, const WebString& defaultValue,
        WebString* actualValue) { return false; }

    // Displays a modal confirmation dialog containing the given message as
    // description and OK/Cancel choices, where 'OK' means that it is okay
    // to proceed with closing the view. Returns true if the user selects
    // 'OK' or false otherwise.
    virtual bool runModalBeforeUnloadDialog(
        bool isReload, const WebString& message) { return true; }


    // UI ------------------------------------------------------------------

    // Shows a context menu with commands relevant to a specific element on
    // the given frame. Additional context data is supplied.
    virtual void showContextMenu(const WebContextMenuData&) { }

    // Called when the data attached to the currently displayed context menu is
    // invalidated. The context menu may be closed if possible.
    virtual void clearContextMenu() { }


    // Low-level resource notifications ------------------------------------

    // An element will request a resource.
    virtual void willRequestResource(WebLocalFrame*, const WebCachedURLRequest&) { }

    // A request is about to be sent out, and the client may modify it.  Request
    // is writable, and changes to the URL, for example, will change the request
    // made.  If this request is the result of a redirect, then redirectResponse
    // will be non-null and contain the response that triggered the redirect.
    virtual void willSendRequest(
        WebLocalFrame*, unsigned identifier, WebURLRequest&,
        const WebURLResponse& redirectResponse) { }

    // Response headers have been received for the resource request given
    // by identifier.
    virtual void didReceiveResponse(
        WebLocalFrame*, unsigned identifier, const WebURLResponse&) { }

    virtual void didChangeResourcePriority(
        WebLocalFrame* webFrame, unsigned identifier, const WebURLRequest::Priority& priority, int) { }

    // The resource request given by identifier succeeded.
    virtual void didFinishResourceLoad(
        WebLocalFrame*, unsigned identifier) { }

    // The specified request was satified from WebCore's memory cache.
    virtual void didLoadResourceFromMemoryCache(
        WebLocalFrame*, const WebURLRequest&, const WebURLResponse&) { }

    // This frame has displayed inactive content (such as an image) from an
    // insecure source.  Inactive content cannot spread to other frames.
    virtual void didDisplayInsecureContent(WebLocalFrame*) { }

    // The indicated security origin has run active content (such as a
    // script) from an insecure source.  Note that the insecure content can
    // spread to other frames in the same origin.
    virtual void didRunInsecureContent(WebLocalFrame*, const WebSecurityOrigin&, const WebURL& insecureURL) { }

    // A reflected XSS was encountered in the page and suppressed.
    virtual void didDetectXSS(WebLocalFrame*, const WebURL&, bool didBlockEntirePage) { }

    // A PingLoader was created, and a request dispatched to a URL.
    virtual void didDispatchPingLoader(WebLocalFrame*, const WebURL&) { }

    // The loaders in this frame have been stopped.
    virtual void didAbortLoading(WebLocalFrame*) { }


    // Script notifications ------------------------------------------------

    // Notifies that a new script context has been created for this frame.
    // This is similar to didClearWindowObject but only called once per
    // frame context.
    virtual void didCreateScriptContext(WebLocalFrame*, v8::Local<v8::Context>, int extensionGroup, int worldId) { }

    // WebKit is about to release its reference to a v8 context for a frame.
    virtual void willReleaseScriptContext(WebLocalFrame*, v8::Local<v8::Context>, int worldId) { }


    // Geometry notifications ----------------------------------------------

    // The frame's document finished the initial non-empty layout of a page.
    virtual void didFirstVisuallyNonEmptyLayout(WebLocalFrame*) { }

    // The main frame scrolled.
    virtual void didChangeScrollOffset(WebLocalFrame*) { }

    // If the frame is loading an HTML document, this will be called to
    // notify that the <body> will be attached soon.
    virtual void willInsertBody(WebLocalFrame*) { }


    // Find-in-page notifications ------------------------------------------

    // Notifies how many matches have been found so far, for a given
    // identifier.  |finalUpdate| specifies whether this is the last update
    // (all frames have completed scoping).
    virtual void reportFindInPageMatchCount(
        int identifier, int count, bool finalUpdate) { }

    // Notifies what tick-mark rect is currently selected.   The given
    // identifier lets the client know which request this message belongs
    // to, so that it can choose to ignore the message if it has moved on
    // to other things.  The selection rect is expected to have coordinates
    // relative to the top left corner of the web page area and represent
    // where on the screen the selection rect is currently located.
    virtual void reportFindInPageSelection(
        int identifier, int activeMatchOrdinal, const WebRect& selection) { }


    // Quota ---------------------------------------------------------

    // Requests a new quota size for the origin's storage.
    // |newQuotaInBytes| indicates how much storage space (in bytes) the
    // caller expects to need.
    // WebStorageQuotaCallbacks::didGrantStorageQuota will be called when
    // a new quota is granted. WebStorageQuotaCallbacks::didFail
    // is called with an error code otherwise.
    // Note that the requesting quota size may not always be granted and
    // a smaller amount of quota than requested might be returned.
    virtual void requestStorageQuota(
        WebLocalFrame*, WebStorageQuotaType,
        unsigned long long newQuotaInBytes,
        WebStorageQuotaCallbacks) { }

    // WebSocket -----------------------------------------------------

    // A WebSocket object is going to open a new WebSocket connection.
    virtual void willOpenWebSocket(WebSocketHandle*) { }


    // Geolocation ---------------------------------------------------------

    // Access the embedder API for (client-based) geolocation client .
    virtual WebGeolocationClient* geolocationClient() { return 0; }


    // MediaStream -----------------------------------------------------

    // A new WebRTCPeerConnectionHandler is created.
    virtual void willStartUsingPeerConnectionHandler(WebLocalFrame*, WebRTCPeerConnectionHandler*) { }

    virtual WebUserMediaClient* userMediaClient() { return 0; }


    // Encrypted Media -------------------------------------------------

    virtual WebEncryptedMediaClient* encryptedMediaClient() { return 0; }


    // Web MIDI -------------------------------------------------------------

    virtual WebMIDIClient* webMIDIClient() { return 0; }


    // Messages ------------------------------------------------------

    // Notifies the embedder that a postMessage was issued on this frame, and
    // gives the embedder a chance to handle it instead of WebKit. Returns true
    // if the embedder handled it.
    virtual bool willCheckAndDispatchMessageEvent(
        WebLocalFrame* sourceFrame,
        WebFrame* targetFrame,
        WebSecurityOrigin target,
        WebDOMMessageEvent event) { return false; }

    // Asks the embedder if a specific user agent should be used for the given
    // URL. Non-empty strings indicate an override should be used. Otherwise,
    // Platform::current()->userAgent() will be called to provide one.
    virtual WebString userAgentOverride(WebLocalFrame*, const WebURL& url) { return WebString(); }

    // Asks the embedder what value the network stack will send for the DNT
    // header. An empty string indicates that no DNT header will be send.
    virtual WebString doNotTrackValue(WebLocalFrame*) { return WebString(); }


    // WebGL ------------------------------------------------------

    // Asks the embedder whether WebGL is allowed for the given WebFrame.
    // This call is placed here instead of WebContentSettingsClient because this
    // class is implemented in content/, and putting it here avoids adding
    // more public content/ APIs.
    virtual bool allowWebGL(WebLocalFrame*, bool defaultValue) { return defaultValue; }

    // Notifies the client that a WebGL context was lost on this page with the
    // given reason (one of the GL_ARB_robustness status codes; see
    // Extensions3D.h in WebCore/platform/graphics).
    virtual void didLoseWebGLContext(WebLocalFrame*, int) { }

    // Send initial drawing parameters to a child frame that is being rendered out of process.
    virtual void initializeChildFrame(const WebRect& frameRect, float scaleFactor) { }


    // Screen Orientation --------------------------------------------------

    // Access the embedder API for (client-based) screen orientation client .
    virtual WebScreenOrientationClient* webScreenOrientationClient() { return 0; }


    // Accessibility -------------------------------------------------------

    // Notifies embedder about an accessibility event.
    virtual void postAccessibilityEvent(const WebAXObject&, WebAXEvent) { }

    // Provides accessibility information about a find in page result.
    virtual void handleAccessibilityFindInPageResult(
        int identifier,
        int matchIndex,
        const WebAXObject& startObject,
        int startOffset,
        const WebAXObject& endObject,
        int endOffset) { }


    // ServiceWorker -------------------------------------------------------

    // Whether the document associated with WebDataSource is controlled by the
    // ServiceWorker.
    virtual bool isControlledByServiceWorker(WebDataSource&) { return false; }

    // Returns an identifier of the service worker controlling the document
    // associated with the WebDataSource.
    virtual int64_t serviceWorkerID(WebDataSource&) { return -1; }


    // Fullscreen ----------------------------------------------------------

    // Called to enter/exit fullscreen mode.
    // After calling enterFullscreen, WebWidget::{will,Did}EnterFullScreen
    // should bound resizing the WebWidget into fullscreen mode.
    // Similarly, when exitFullScreen is called,
    // WebWidget::{will,Did}ExitFullScreen should bound resizing the WebWidget
    // out of fullscreen mode.
    // Note: the return value is ignored.
    virtual bool enterFullscreen() { return false; }
    virtual bool exitFullscreen() { return false; }


    // Sudden termination --------------------------------------------------

    // Called when elements preventing the sudden termination of the frame
    // become present or stop being present. |type| is the type of element
    // (BeforeUnload handler, Unload handler).
    enum SuddenTerminationDisablerType {
        BeforeUnloadHandler,
        UnloadHandler,
    };
    virtual void suddenTerminationDisablerChanged(bool present, SuddenTerminationDisablerType) { }


    // Permissions ---------------------------------------------------------

    // Access the embedder API for permission client.
    virtual WebPermissionClient* permissionClient() { return 0; }

    // Virtual Reality -----------------------------------------------------

    // Access the embedder API for virtual reality client.
    virtual WebVRClient* webVRClient() { return 0; }

    // App Banners ---------------------------------------------------------
    virtual WebAppBannerClient* appBannerClient() { return 0; }

    // Navigator Content Utils  --------------------------------------------

    // Registers a new URL handler for the given protocol.
    virtual void registerProtocolHandler(const WebString& scheme,
        const WebURL& url,
        const WebString& title) { }

    // Unregisters a given URL handler for the given protocol.
    virtual void unregisterProtocolHandler(const WebString& scheme, const WebURL& url) { }

    // Check if a given URL handler is registered for the given protocol.
    virtual WebCustomHandlersState isProtocolHandlerRegistered(const WebString& scheme, const WebURL& url)
    {
        return WebCustomHandlersNew;
    }

protected:
    virtual ~WebFrameClient() { }
};

} // namespace blink

#endif
