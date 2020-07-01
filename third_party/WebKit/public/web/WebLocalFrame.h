// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebLocalFrame_h
#define WebLocalFrame_h

#include "WebFrame.h"
#include "WebFrameLoadType.h"

namespace blink {

enum class WebAppBannerPromptReply;
enum class WebSandboxFlags;
enum class WebTreeScopeType;
class WebAutofillClient;
class WebContentSettingsClient;
class WebDevToolsAgent;
class WebDevToolsAgentClient;
class WebFrameClient;
class WebNode;
class WebScriptExecutionCallback;
class WebSuspendableTask;
class WebTestInterfaceFactory;
struct WebPrintPresetOptions;

// Interface for interacting with in process frames. This contains methods that
// require interacting with a frame's document.
// FIXME: Move lots of methods from WebFrame in here.
class WebLocalFrame : public WebFrame {
public:
    // Creates a WebFrame. Delete this WebFrame by calling WebFrame::close().
    // It is valid to pass a null client pointer.
    BLINK_EXPORT static WebLocalFrame* create(WebTreeScopeType, WebFrameClient*);

    // Returns the WebFrame associated with the current V8 context. This
    // function can return 0 if the context is associated with a Document that
    // is not currently being displayed in a Frame.
    BLINK_EXPORT static WebLocalFrame* frameForCurrentContext();

    // Returns the frame corresponding to the given context. This can return 0
    // if the context is detached from the frame, or if the context doesn't
    // correspond to a frame (e.g., workers).
    BLINK_EXPORT static WebLocalFrame* frameForContext(v8::Local<v8::Context>);

    // Returns the frame inside a given frame or iframe element. Returns 0 if
    // the given element is not a frame, iframe or if the frame is empty.
    BLINK_EXPORT static WebLocalFrame* fromFrameOwnerElement(const WebElement&);

    // Initialization ---------------------------------------------------------

    // Used when we might swap from a remote frame to a local frame.
    // Creates a provisional, semi-attached frame that will be fully
    // swapped into the frame tree if it commits.
    virtual void initializeToReplaceRemoteFrame(WebRemoteFrame*, const WebString& name, WebSandboxFlags) = 0;

    virtual void setAutofillClient(WebAutofillClient*) = 0;
    virtual WebAutofillClient* autofillClient() = 0;
    virtual void setDevToolsAgentClient(WebDevToolsAgentClient*) = 0;
    virtual WebDevToolsAgent* devToolsAgent() = 0;

    // Navigation Ping --------------------------------------------------------
    virtual void sendPings(const WebNode& contextNode, const WebURL& destinationURL) = 0;

    // Navigation ----------------------------------------------------------

    // Returns a WebURLRequest corresponding to the load of the WebHistoryItem.
    virtual WebURLRequest requestFromHistoryItem(const WebHistoryItem&, WebURLRequest::CachePolicy)
        const = 0;

    // Returns a WebURLRequest corresponding to the reload of the current
    // HistoryItem.
    virtual WebURLRequest requestForReload(WebFrameLoadType,
        const WebURL& overrideURL = WebURL()) const = 0;

    // Load the given URL. For history navigations, a valid WebHistoryItem
    // should be given, as well as a WebHistoryLoadType.
    // TODO(clamy): Remove the reload, reloadWithOverrideURL, loadHistoryItem
    // loadRequest functions in WebFrame once RenderFrame only calls loadRequest.
    virtual void load(const WebURLRequest&, WebFrameLoadType = WebFrameLoadType::Standard,
        const WebHistoryItem& = WebHistoryItem(),
        WebHistoryLoadType = WebHistoryDifferentDocumentLoad) = 0;

    // Navigation State -------------------------------------------------------

    // Returns true if the current frame's load event has not completed.
    virtual bool isLoading() const = 0;

    // Returns true if any resource load is currently in progress. Exposed
    // primarily for use in layout tests. You probably want isLoading()
    // instead.
    virtual bool isResourceLoadInProgress() const = 0;

    // Override the normal rules for whether a load has successfully committed
    // in this frame. Used to propagate state when this frame has navigated
    // cross process.
    virtual void setCommittedFirstRealLoad() = 0;

    // Orientation Changes ----------------------------------------------------

    // Notify the frame that the screen orientation has changed.
    virtual void sendOrientationChangeEvent() = 0;


    // Printing ------------------------------------------------------------

    // Returns true on success and sets the out parameter to the print preset options for the document.
    virtual bool getPrintPresetOptionsForPlugin(const WebNode&, WebPrintPresetOptions*) = 0;


    // Scripting --------------------------------------------------------------
    // Executes script in the context of the current page and returns the value
    // that the script evaluated to with callback. Script execution can be
    // suspend.
    virtual void requestExecuteScriptAndReturnValue(const WebScriptSource&,
        bool userGesture, WebScriptExecutionCallback*) = 0;

    // worldID must be > 0 (as 0 represents the main world).
    // worldID must be < EmbedderWorldIdLimit, high number used internally.
    virtual void requestExecuteScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sourceIn, unsigned numSources,
        int extensionGroup, bool userGesture, WebScriptExecutionCallback*) = 0;

    // Run the task when the context of the current page is not suspended
    // otherwise run it on context resumed.
    // Method takes ownership of the passed task.
    virtual void requestRunTask(WebSuspendableTask*) const = 0;

    // Associates an isolated world with human-readable name which is useful for
    // extension debugging.
    virtual void setIsolatedWorldHumanReadableName(int worldID, const WebString&) = 0;


    // Selection --------------------------------------------------------------

    // Moves the selection extent point. This function does not allow the
    // selection to collapse. If the new extent is set to the same position as
    // the current base, this function will do nothing.
    virtual void moveRangeSelectionExtent(const WebPoint&) = 0;

    // Content Settings -------------------------------------------------------

    virtual void setContentSettingsClient(WebContentSettingsClient*) = 0;

    // App banner -------------------------------------------------------------

    // Request to show an application install banner for the given |platforms|.
    // The implementation can request the embedder to cancel the call by setting
    // |cancel| to true.
    virtual void willShowInstallBannerPrompt(int requestId, const WebVector<WebString>& platforms, WebAppBannerPromptReply*) = 0;

    // Image reload -----------------------------------------------------------

    // If the provided node is an image, reload the image bypassing the cache.
    virtual void reloadImage(const WebNode&) = 0;

    // Testing ----------------------------------------------------------------

    // Registers a test interface factory. Takes ownership of the factory.
    virtual void registerTestInterface(const WebString& name, WebTestInterfaceFactory*) = 0;

    // Iframe sandbox ---------------------------------------------------------

    // Returns the effective sandbox flags which are inherited from their parent frame.
    virtual WebSandboxFlags effectiveSandboxFlags() const = 0;

    virtual int topContentInset() const = 0;

protected:
    explicit WebLocalFrame(WebTreeScopeType scope) : WebFrame(scope) { }
};

} // namespace blink

#endif // WebLocalFrame_h
