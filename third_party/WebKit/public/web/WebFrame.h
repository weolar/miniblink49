/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef WebFrame_h
#define WebFrame_h

#include "WebCompositionUnderline.h"
#include "WebHistoryItem.h"
#include "WebIconURL.h"
#include "WebNode.h"
#include "WebURLLoaderOptions.h"
#include "public/platform/WebCanvas.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebPrivateOwnPtr.h"
#include "public/platform/WebReferrerPolicy.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebTreeScopeType.h"

struct NPObject;

namespace v8 {
class Context;
class Function;
class Object;
class Value;
template <class T> class Local;
}

namespace blink {

class Frame;
class OpenedFrameTracker;
class Visitor;
class WebData;
class WebDataSource;
class WebDocument;
class WebElement;
class WebLayer;
class WebLocalFrame;
class WebPerformance;
class WebRange;
class WebRemoteFrame;
class WebSecurityOrigin;
class WebSharedWorkerRepositoryClient;
class WebString;
class WebURL;
class WebURLLoader;
class WebURLRequest;
class WebView;
enum class WebSandboxFlags;
struct WebConsoleMessage;
struct WebFindOptions;
struct WebFloatPoint;
struct WebFloatRect;
struct WebPoint;
struct WebPrintParams;
struct WebRect;
struct WebScriptSource;
struct WebSize;
struct WebURLLoaderOptions;

template <typename T> class WebVector;

// Frames may be rendered in process ('local') or out of process ('remote').
// A remote frame is always cross-site; a local frame may be either same-site or
// cross-site.
// WebFrame is the base class for both WebLocalFrame and WebRemoteFrame and
// contains methods that are valid on both local and remote frames, such as
// getting a frame's parent or its opener.
class WebFrame {
public:
    // Control of layoutTreeAsText output
    enum LayoutAsTextControl {
        LayoutAsTextNormal = 0,
        LayoutAsTextDebug = 1 << 0,
        LayoutAsTextPrinting = 1 << 1,
        LayoutAsTextWithLineTrees = 1 << 2
    };
    typedef unsigned LayoutAsTextControls;

    // FIXME: We already have blink::TextGranularity. For now we support only
    // a part of blink::TextGranularity.
    // Ideally it seems blink::TextGranularity should be broken up into
    // blink::TextGranularity and perhaps blink::TextBoundary and then
    // TextGranularity enum could be moved somewhere to public/, and we could
    // just use it here directly without introducing a new enum.
    enum TextGranularity {
        CharacterGranularity = 0,
        WordGranularity,
        TextGranularityLast = WordGranularity,
    };

    // Returns the number of live WebFrame objects, used for leak checking.
    BLINK_EXPORT static int instanceCount();

    virtual bool isWebLocalFrame() const = 0;
    virtual WebLocalFrame* toWebLocalFrame() = 0;
    virtual bool isWebRemoteFrame() const = 0;
    virtual WebRemoteFrame* toWebRemoteFrame() = 0;

    BLINK_EXPORT bool swap(WebFrame*);

    // This method closes and deletes the WebFrame. This is typically called by
    // the embedder in response to a frame detached callback to the WebFrame
    // client.
    virtual void close() = 0;

    // Called by the embedder when it needs to detach the subtree rooted at this
    // frame.
    BLINK_EXPORT void detach();

    // Basic properties ---------------------------------------------------

    // The unique name of this frame.
    virtual WebString uniqueName() const = 0;

    // The name of this frame. If no name is given, empty string is returned.
    virtual WebString assignedName() const = 0;

    // Sets the name of this frame. For child frames (frames that are not a
    // top-most frame) the actual name may have a suffix appended to make the
    // frame name unique within the hierarchy.
    virtual void setName(const WebString&) = 0;

    // The urls of the given combination types of favicon (if any) specified by
    // the document loaded in this frame. The iconTypesMask is a bit-mask of
    // WebIconURL::Type values, used to select from the available set of icon
    // URLs
    virtual WebVector<WebIconURL> iconURLs(int iconTypesMask) const = 0;

    // For a WebFrame with contents being rendered in another process, this
    // sets a layer for use by the in-process compositor. WebLayer should be
    // null if the content is being rendered in the current process.
    virtual void setRemoteWebLayer(WebLayer*) = 0;

    // Initializes the various client interfaces.
    virtual void setSharedWorkerRepositoryClient(WebSharedWorkerRepositoryClient*) = 0;

    // The security origin of this frame.
    BLINK_EXPORT WebSecurityOrigin securityOrigin() const;

    // Updates the sandbox flags in the frame's FrameOwner.  This is used when
    // this frame's parent is in another process and it dynamically updates
    // this frame's sandbox flags.  The flags won't take effect until the next
    // navigation.
    BLINK_EXPORT void setFrameOwnerSandboxFlags(WebSandboxFlags);

    // Geometry -----------------------------------------------------------

    // NOTE: These routines do not force page layout so their results may
    // not be accurate if the page layout is out-of-date.

    // If set to false, do not draw scrollbars on this frame's view.
    virtual void setCanHaveScrollbars(bool) = 0;

    // The scroll offset from the top-left corner of the frame in pixels.
    virtual WebSize scrollOffset() const = 0;
    virtual void setScrollOffset(const WebSize&) = 0;

    // The minimum and maxium scroll positions in pixels.
    virtual WebSize minimumScrollOffset() const = 0;
    virtual WebSize maximumScrollOffset() const = 0;

    // The size of the contents area.
    virtual WebSize contentsSize() const = 0;

    // Returns true if the contents (minus scrollbars) has non-zero area.
    virtual bool hasVisibleContent() const = 0;

    // Returns the visible content rect (minus scrollbars, in absolute coordinate)
    virtual WebRect visibleContentRect() const = 0;

    virtual bool hasHorizontalScrollbar() const = 0;
    virtual bool hasVerticalScrollbar() const = 0;

    // Hierarchy ----------------------------------------------------------

    // Returns the containing view.
    virtual WebView* view() const = 0;

    // Returns the frame that opened this frame or 0 if there is none.
    BLINK_EXPORT WebFrame* opener() const;

    // Sets the frame that opened this one or 0 if there is none.
    virtual void setOpener(WebFrame*);

    // Reset the frame that opened this frame to 0.
    // This is executed between layout tests runs
    void clearOpener() { setOpener(0); }

    // Inserts the given frame as a child of this frame, so that it is the next
    // child after |previousSibling|, or first child if |previousSibling| is null.
    BLINK_EXPORT void insertAfter(WebFrame* child, WebFrame* previousSibling);

    // Adds the given frame as a child of this frame.
    BLINK_EXPORT void appendChild(WebFrame*);

    // Removes the given child from this frame.
    virtual void removeChild(WebFrame*);

    // Returns the parent frame or 0 if this is a top-most frame.
    BLINK_EXPORT WebFrame* parent() const;

    // Returns the top-most frame in the hierarchy containing this frame.
    BLINK_EXPORT WebFrame* top() const;

    // Returns the first/last child frame.
    BLINK_EXPORT WebFrame* firstChild() const;
    BLINK_EXPORT WebFrame* lastChild() const;

    // Returns the previous/next sibling frame.
    BLINK_EXPORT WebFrame* previousSibling() const;
    BLINK_EXPORT WebFrame* nextSibling() const;

    // Returns the previous/next frame in "frame traversal order",
    // optionally wrapping around.
    BLINK_EXPORT WebFrame* traversePrevious(bool wrap) const;
    BLINK_EXPORT WebFrame* traverseNext(bool wrap) const;

    // Returns the child frame identified by the given name.
    BLINK_EXPORT WebFrame* findChildByName(const WebString& name) const;


    // Content ------------------------------------------------------------

    virtual WebDocument document() const = 0;
#ifdef IMPLEMENTED_NEWEST_BLINK
    virtual WebPerformance performance() const = 0;
#endif // IMPLEMENTED_NEWEST_BLINK

    // Closing -------------------------------------------------------------

    // Runs beforeunload handlers for this frame, returning false if a
    // handler suppressed unloading.
    virtual bool dispatchBeforeUnloadEvent() = 0;

    // Runs unload handlers for this frame.
    virtual void dispatchUnloadEvent() = 0;


    // Scripting ----------------------------------------------------------

    // Returns a NPObject corresponding to this frame's DOMWindow.
    virtual NPObject* windowObject() const = 0;

    // Binds a NPObject as a property of this frame's DOMWindow.
    virtual void bindToWindowObject(const WebString& name, NPObject*) = 0;
    virtual void bindToWindowObject(
        const WebString& name, NPObject*, void*) = 0;

    // Executes script in the context of the current page.
    virtual void executeScript(const WebScriptSource&) = 0;

    // Executes JavaScript in a new world associated with the web frame.
    // The script gets its own global scope and its own prototypes for
    // intrinsic JavaScript objects (String, Array, and so-on). It also
    // gets its own wrappers for all DOM nodes and DOM constructors.
    // extensionGroup is an embedder-provided specifier that controls which
    // v8 extensions are loaded into the new context - see
    // blink::registerExtension for the corresponding specifier.
    //
    // worldID must be > 0 (as 0 represents the main world).
    // worldID must be < EmbedderWorldIdLimit, high number used internally.
    virtual void executeScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sources, unsigned numSources,
        int extensionGroup) = 0;

    // Associates an isolated world (see above for description) with a security
    // origin. XMLHttpRequest instances used in that world will be considered
    // to come from that origin, not the frame's.
    virtual void setIsolatedWorldSecurityOrigin(
        int worldID, const WebSecurityOrigin&) = 0;

    // Associates a content security policy with an isolated world. This policy
    // should be used when evaluating script in the isolated world, and should
    // also replace a protected resource's CSP when evaluating resources
    // injected into the DOM.
    //
    // FIXME: Setting this simply bypasses the protected resource's CSP. It
    //     doesn't yet restrict the isolated world to the provided policy.
    virtual void setIsolatedWorldContentSecurityPolicy(
        int worldID, const WebString&) = 0;

    // Logs to the console associated with this frame.
    virtual void addMessageToConsole(const WebConsoleMessage&) = 0;

    // Calls window.gc() if it is defined.
    virtual void collectGarbage() = 0;

    // Check if the scripting URL represents a mixed content condition relative
    // to this frame.
    virtual bool checkIfRunInsecureContent(const WebURL&) const = 0;

    // Executes script in the context of the current page and returns the value
    // that the script evaluated to.
    // DEPRECATED: Use WebLocalFrame::requestExecuteScriptAndReturnValue.
    virtual v8::Local<v8::Value> executeScriptAndReturnValue(
        const WebScriptSource&) = 0;

    // worldID must be > 0 (as 0 represents the main world).
    // worldID must be < EmbedderWorldIdLimit, high number used internally.
    // DEPRECATED: Use WebLocalFrame::requestExecuteScriptInIsolatedWorld.
    virtual void executeScriptInIsolatedWorld(
        int worldID, const WebScriptSource* sourcesIn, unsigned numSources,
        int extensionGroup, WebVector<v8::Local<v8::Value> >* results) = 0;

    // Call the function with the given receiver and arguments, bypassing
    // canExecute().
    virtual v8::Local<v8::Value> callFunctionEvenIfScriptDisabled(
        v8::Local<v8::Function>,
        v8::Local<v8::Value>,
        int argc,
        v8::Local<v8::Value> argv[]) = 0;

    // Returns the V8 context for associated with the main world and this
    // frame. There can be many V8 contexts associated with this frame, one for
    // each isolated world and one for the main world. If you don't know what
    // the "main world" or an "isolated world" is, then you probably shouldn't
    // be calling this API.
    virtual v8::Local<v8::Context> mainWorldScriptContext() const = 0;


    // Returns true if the WebFrame currently executing JavaScript has access
    // to the given WebFrame, or false otherwise.
    BLINK_EXPORT static bool scriptCanAccess(WebFrame*);


    // Navigation ----------------------------------------------------------

    // Reload the current document.
    // True |ignoreCache| explicitly bypasses caches.
    // False |ignoreCache| revalidates any existing cache entries.
    virtual void reload(bool ignoreCache = false) = 0;

    // This is used for situations where we want to reload a different URL because of a redirect.
    virtual void reloadWithOverrideURL(const WebURL& overrideUrl, bool ignoreCache = false) = 0;

    // Load the given URL.
    virtual void loadRequest(const WebURLRequest&) = 0;

    // Load the given history state, corresponding to a back/forward
    // navigation of a frame. Multiple frames may be navigated via separate calls.
    virtual void loadHistoryItem(
        const WebHistoryItem&,
        WebHistoryLoadType,
        WebURLRequest::CachePolicy = WebURLRequest::UseProtocolCachePolicy) = 0;

    // Loads the given data with specific mime type and optional text
    // encoding.  For HTML data, baseURL indicates the security origin of
    // the document and is used to resolve links.  If specified,
    // unreachableURL is reported via WebDataSource::unreachableURL.  If
    // replace is false, then this data will be loaded as a normal
    // navigation.  Otherwise, the current history item will be replaced.
    virtual void loadData(const WebData& data,
                          const WebString& mimeType,
                          const WebString& textEncoding,
                          const WebURL& baseURL,
                          const WebURL& unreachableURL = WebURL(),
                          bool replace = false) = 0;

    // This method is short-hand for calling LoadData, where mime_type is
    // "text/html" and text_encoding is "UTF-8".
    virtual void loadHTMLString(const WebData& html,
                                const WebURL& baseURL,
                                const WebURL& unreachableURL = WebURL(),
                                bool replace = false) = 0;

    // Stops any pending loads on the frame and its children.
    virtual void stopLoading() = 0;

    // Returns the data source that is currently loading.  May be null.
    virtual WebDataSource* provisionalDataSource() const = 0;

    // Returns the data source that is currently loaded.
    virtual WebDataSource* dataSource() const = 0;

    // View-source rendering mode.  Set this before loading an URL to cause
    // it to be rendered in view-source mode.
    virtual void enableViewSourceMode(bool) = 0;
    virtual bool isViewSourceModeEnabled() const = 0;

    // Sets the referrer for the given request to be the specified URL or
    // if that is null, then it sets the referrer to the referrer that the
    // frame would use for subresources.  NOTE: This method also filters
    // out invalid referrers (e.g., it is invalid to send a HTTPS URL as
    // the referrer for a HTTP request).
    virtual void setReferrerForRequest(WebURLRequest&, const WebURL&) = 0;

    // Called to associate the WebURLRequest with this frame.  The request
    // will be modified to inherit parameters that allow it to be loaded.
    // This method ends up triggering WebFrameClient::willSendRequest.
    // DEPRECATED: Please use createAssociatedURLLoader instead.
    virtual void dispatchWillSendRequest(WebURLRequest&) = 0;

    // Returns a WebURLLoader that is associated with this frame.  The loader
    // will, for example, be cancelled when WebFrame::stopLoading is called.
    // FIXME: stopLoading does not yet cancel an associated loader!!
    virtual WebURLLoader* createAssociatedURLLoader(const WebURLLoaderOptions& = WebURLLoaderOptions()) = 0;

    // Returns the number of registered unload listeners.
    virtual unsigned unloadListenerCount() const = 0;


    // Editing -------------------------------------------------------------

    // Replaces the selection with the given text.
    virtual void replaceSelection(const WebString& text) = 0;

    virtual void insertText(const WebString& text) = 0;

    virtual void setMarkedText(const WebString& text, unsigned location, unsigned length) = 0;
    virtual void unmarkText() = 0;
    virtual bool hasMarkedText() const = 0;

    virtual WebRange markedRange() const = 0;

    // Returns the text range rectangle in the viepwort coordinate space.
    virtual bool firstRectForCharacterRange(unsigned location, unsigned length, WebRect&) const = 0;

    // Returns the index of a character in the Frame's text stream at the given
    // point. The point is in the viewport coordinate space. Will return
    // WTF::notFound if the point is invalid.
    virtual size_t characterIndexForPoint(const WebPoint&) const = 0;

    // Supports commands like Undo, Redo, Cut, Copy, Paste, SelectAll,
    // Unselect, etc. See EditorCommand.cpp for the full list of supported
    // commands.
    virtual bool executeCommand(const WebString&, const WebNode& = WebNode()) = 0;
    virtual bool executeCommand(const WebString&, const WebString& value, const WebNode& = WebNode()) = 0;
    virtual bool isCommandEnabled(const WebString&) const = 0;

    // Spell-checking support.
    virtual void enableContinuousSpellChecking(bool) = 0;
    virtual bool isContinuousSpellCheckingEnabled() const = 0;
    virtual void requestTextChecking(const WebElement&) = 0;
    virtual void replaceMisspelledRange(const WebString&) = 0;
    virtual void removeSpellingMarkers() = 0;

    // Selection -----------------------------------------------------------

    virtual bool hasSelection() const = 0;

    virtual WebRange selectionRange() const = 0;

    virtual WebString selectionAsText() const = 0;
    virtual WebString selectionAsMarkup() const = 0;

    // Expands the selection to a word around the caret and returns
    // true. Does nothing and returns false if there is no caret or
    // there is ranged selection.
    virtual bool selectWordAroundCaret() = 0;

    // DEPRECATED: Use moveRangeSelection.
    virtual void selectRange(const WebPoint& base, const WebPoint& extent) = 0;

    virtual void selectRange(const WebRange&) = 0;

    // Move the current selection to the provided viewport point/points. If the
    // current selection is editable, the new selection will be restricted to
    // the root editable element.
    // |TextGranularity| represents character wrapping granularity. If
    // WordGranularity is set, WebFrame extends selection to wrap word.
    virtual void moveRangeSelection(const WebPoint& base, const WebPoint& extent, WebFrame::TextGranularity = CharacterGranularity) = 0;
    virtual void moveCaretSelection(const WebPoint&) = 0;

    virtual bool setEditableSelectionOffsets(int start, int end) = 0;
    virtual bool setCompositionFromExistingText(int compositionStart, int compositionEnd, const WebVector<WebCompositionUnderline>& underlines) = 0;
    virtual void extendSelectionAndDelete(int before, int after) = 0;

    virtual void setCaretVisible(bool) = 0;

    // Printing ------------------------------------------------------------

    // Reformats the WebFrame for printing. WebPrintParams specifies the printable
    // content size, paper size, printable area size, printer DPI and print
    // scaling option. If constrainToNode node is specified, then only the given node
    // is printed (for now only plugins are supported), instead of the entire frame.
    // Returns the number of pages that can be printed at the given
    // page size.
    virtual int printBegin(const WebPrintParams&, const WebNode& constrainToNode = WebNode()) = 0;

    // Returns the page shrinking factor calculated by webkit (usually
    // between 1/1.25 and 1/2). Returns 0 if the page number is invalid or
    // not in printing mode.
    virtual float getPrintPageShrink(int page) = 0;

    // Prints one page, and returns the calculated page shrinking factor
    // (usually between 1/1.25 and 1/2).  Returns 0 if the page number is
    // invalid or not in printing mode.
    virtual float printPage(int pageToPrint, WebCanvas*) = 0;

    // Reformats the WebFrame for screen display.
    virtual void printEnd() = 0;

    // If the frame contains a full-frame plugin or the given node refers to a
    // plugin whose content indicates that printed output should not be scaled,
    // return true, otherwise return false.
    virtual bool isPrintScalingDisabledForPlugin(const WebNode& = WebNode()) = 0;

    // CSS3 Paged Media ----------------------------------------------------

    // Returns true if page box (margin boxes and page borders) is visible.
    virtual bool isPageBoxVisible(int pageIndex) = 0;

    // Returns true if the page style has custom size information.
    virtual bool hasCustomPageSizeStyle(int pageIndex) = 0;

    // Returns the preferred page size and margins in pixels, assuming 96
    // pixels per inch. pageSize, marginTop, marginRight, marginBottom,
    // marginLeft must be initialized to the default values that are used if
    // auto is specified.
    virtual void pageSizeAndMarginsInPixels(int pageIndex,
                                            WebSize& pageSize,
                                            int& marginTop,
                                            int& marginRight,
                                            int& marginBottom,
                                            int& marginLeft) = 0;

    // Returns the value for a page property that is only defined when printing.
    // printBegin must have been called before this method.
    virtual WebString pageProperty(const WebString& propertyName, int pageIndex) = 0;

    // Find-in-page --------------------------------------------------------

    // Searches a frame for a given string.
    //
    // If a match is found, this function will select it (scrolling down to
    // make it visible if needed) and fill in selectionRect with the
    // location of where the match was found (in window coordinates).
    //
    // If no match is found, this function clears all tickmarks and
    // highlighting.
    //
    // Returns true if the search string was found, false otherwise.
    virtual bool find(int identifier,
                      const WebString& searchText,
                      const WebFindOptions& options,
                      bool wrapWithinFrame,
                      WebRect* selectionRect) = 0;

    // Notifies the frame that we are no longer interested in searching.
    // This will abort any asynchronous scoping effort already under way
    // (see the function scopeStringMatches for details) and erase all
    // tick-marks and highlighting from the previous search.  If
    // clearSelection is true, it will also make sure the end state for the
    // find operation does not leave a selection.  This can occur when the
    // user clears the search string but does not close the find box.
    virtual void stopFinding(bool clearSelection) = 0;

    // Counts how many times a particular string occurs within the frame.
    // It also retrieves the location of the string and updates a vector in
    // the frame so that tick-marks and highlighting can be drawn.  This
    // function does its work asynchronously, by running for a certain
    // time-slice and then scheduling itself (co-operative multitasking) to
    // be invoked later (repeating the process until all matches have been
    // found).  This allows multiple frames to be searched at the same time
    // and provides a way to cancel at any time (see
    // cancelPendingScopingEffort).  The parameter searchText specifies
    // what to look for and |reset| signals whether this is a brand new
    // request or a continuation of the last scoping effort.
    virtual void scopeStringMatches(int identifier,
                                    const WebString& searchText,
                                    const WebFindOptions& options,
                                    bool reset) = 0;

    // Cancels any outstanding requests for scoping string matches on a frame.
    virtual void cancelPendingScopingEffort() = 0;

    // This function is called on the main frame during the scoping effort
    // to keep a running tally of the accumulated total match-count for all
    // frames.  After updating the count it will notify the WebViewClient
    // about the new count.
    virtual void increaseMatchCount(int count, int identifier) = 0;

    // This function is called on the main frame to reset the total number
    // of matches found during the scoping effort.
    virtual void resetMatchCount() = 0;

    // Returns a counter that is incremented when the find-in-page markers are
    // changed on any frame. Switching the active marker doesn't change the
    // current version. Should be called only on the main frame.
    virtual int findMatchMarkersVersion() const = 0;

    // Returns the bounding box of the active find-in-page match marker or an
    // empty rect if no such marker exists. The rect is returned in find-in-page
    // coordinates whatever frame the active marker is.
    // Should be called only on the main frame.
    virtual WebFloatRect activeFindMatchRect() = 0;

    // Swaps the contents of the provided vector with the bounding boxes of the
    // find-in-page match markers from all frames. The bounding boxes are returned
    // in find-in-page coordinates. This method should be called only on the main frame.
    virtual void findMatchRects(WebVector<WebFloatRect>&) = 0;

    // Selects the find-in-page match in the appropriate frame closest to the
    // provided point in find-in-page coordinates. Returns the ordinal of such
    // match or -1 if none could be found. If not null, selectionRect is set to
    // the bounding box of the selected match in window coordinates.
    // This method should be called only on the main frame.
    virtual int selectNearestFindMatch(const WebFloatPoint&,
                                       WebRect* selectionRect) = 0;


    // Set the tickmarks for the frame. This will override the default tickmarks
    // generated by find results. If this is called with an empty array, the
    // default behavior will be restored.
    virtual void setTickmarks(const WebVector<WebRect>&) = 0;

    // Events --------------------------------------------------------------

    // Dispatches a message event on the current DOMWindow in this WebFrame.
    virtual void dispatchMessageEventWithOriginCheck(
        const WebSecurityOrigin& intendedTargetOrigin,
        const WebDOMEvent&) = 0;


    // Utility -------------------------------------------------------------

    // Returns the contents of this frame as a string.  If the text is
    // longer than maxChars, it will be clipped to that length.  WARNING:
    // This function may be slow depending on the number of characters
    // retrieved and page complexity.  For a typically sized page, expect
    // it to take on the order of milliseconds.
    //
    // If there is room, subframe text will be recursively appended. Each
    // frame will be separated by an empty line.
    virtual WebString contentAsText(size_t maxChars) const = 0;

    // Returns HTML text for the contents of this frame.  This is generated
    // from the DOM.
    virtual WebString contentAsMarkup() const = 0;

    // Returns a text representation of the render tree.  This method is used
    // to support layout tests.
    virtual WebString layoutTreeAsText(LayoutAsTextControls toShow = LayoutAsTextNormal) const = 0;

    // Draws the contents of the web frame at the specified region onto the
    // specified canvas
    virtual void drawInCanvas(const WebRect& rect, const WebString& customCSS, WebCanvas* canvas) const {};

    // Calls markerTextForListItem() defined in core/layout/LayoutTreeAsText.h.
    virtual WebString markerTextForListItem(const WebElement&) const = 0;

    // Prints all of the pages into the canvas, with page boundaries drawn as
    // one pixel wide blue lines. This method exists to support layout tests.
    virtual void printPagesWithBoundaries(WebCanvas*, const WebSize&) = 0;

    // Returns the bounds rect for current selection. If selection is performed
    // on transformed text, the rect will still bound the selection but will
    // not be transformed itself. If no selection is present, the rect will be
    // empty ((0,0), (0,0)).
    virtual WebRect selectionBoundsRect() const = 0;

    // Only for testing purpose:
    // Returns true if selection.anchorNode has a marker on range from |from| with |length|.
    virtual bool selectionStartHasSpellingMarkerFor(int from, int length) const = 0;

    // Dumps the layer tree, used by the accelerated compositor, in
    // text form. This is used only by layout tests.
    virtual WebString layerTreeAsText(bool showDebugInfo = false) const = 0;

    // Returns the frame inside a given frame or iframe element. Returns 0 if
    // the given element is not a frame, iframe or if the frame is empty.
    BLINK_EXPORT static WebFrame* fromFrameOwnerElement(const WebElement&);

#if BLINK_IMPLEMENTATION
    static WebFrame* fromFrame(Frame*);

    bool inShadowTree() const { return m_scope == WebTreeScopeType::Shadow; }

#if ENABLE(OILPAN)
    static void traceFrames(Visitor*, WebFrame*);
    static void traceFrames(InlinedGlobalMarkingVisitor, WebFrame*);
    void clearWeakFrames(Visitor*);
    void clearWeakFrames(InlinedGlobalMarkingVisitor);
#endif
#endif

protected:
    explicit WebFrame(WebTreeScopeType);
    virtual ~WebFrame();

    // Sets the parent WITHOUT fulling adding it to the frame tree.
    // Used to lie to a local frame that is replacing a remote frame,
    // so it can properly start a navigation but wait to swap until
    // commit-time.
    void setParent(WebFrame*);

private:
    friend class OpenedFrameTracker;

#if BLINK_IMPLEMENTATION
#if ENABLE(OILPAN)
    static void traceFrame(Visitor*, WebFrame*);
    static void traceFrame(InlinedGlobalMarkingVisitor, WebFrame*);
    static bool isFrameAlive(const WebFrame*);

    template <typename VisitorDispatcher>
    static void traceFramesImpl(VisitorDispatcher, WebFrame*);
    template <typename VisitorDispatcher>
    void clearWeakFramesImpl(VisitorDispatcher);
    template <typename VisitorDispatcher>
    static void traceFrameImpl(VisitorDispatcher, WebFrame*);
#endif
#endif

    const WebTreeScopeType m_scope;

    WebFrame* m_parent;
    WebFrame* m_previousSibling;
    WebFrame* m_nextSibling;
    WebFrame* m_firstChild;
    WebFrame* m_lastChild;

    WebFrame* m_opener;
    WebPrivateOwnPtr<OpenedFrameTracker> m_openedFrameTracker;
};

#if BLINK_IMPLEMENTATION
Frame* toCoreFrame(const WebFrame*);
#endif

} // namespace blink

#endif
