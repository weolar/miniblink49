/*
 * Copyright (C) 2009, 2010, 2011, 2012 Google Inc. All rights reserved.
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

#ifndef WebView_h
#define WebView_h

#include "../platform/WebColor.h"
#include "../platform/WebDisplayMode.h"
#include "../platform/WebPageVisibilityState.h"
#include "../platform/WebString.h"
#include "../platform/WebVector.h"
#include "WebDragOperation.h"
#include "WebHistoryCommitType.h"
#include "WebHistoryItem.h"
#include "WebWidget.h"

namespace blink {

class WebAXObject;
class WebAutofillClient;
class WebCredentialManagerClient;
class WebDragData;
class WebFrame;
class WebHitTestResult;
class WebPageOverlay;
class WebPrerendererClient;
class WebSettings;
class WebSpellCheckClient;
class WebString;
class WebViewClient;
struct WebActiveWheelFlingParameters;
struct WebDeviceEmulationParams;
struct WebMediaPlayerAction;
struct WebPluginAction;
struct WebPoint;
struct WebFloatPoint;
struct WebWindowFeatures;

class WebView : public WebWidget {
public:
    BLINK_EXPORT static const double textSizeMultiplierRatio;
    BLINK_EXPORT static const double minTextSizeMultiplier;
    BLINK_EXPORT static const double maxTextSizeMultiplier;

    enum StyleInjectionTarget {
        InjectStyleInAllFrames,
        InjectStyleInTopFrameOnly
    };


    // Initialization ------------------------------------------------------

    // Creates a WebView that is NOT yet initialized. You will need to
    // call setMainFrame to finish the initialization. It is valid
    // to pass a null client pointer.
    BLINK_EXPORT static WebView* create(WebViewClient*);

    // After creating a WebView, you should immediately call this method.
    // You can optionally modify the settings before calling this method.
    // This WebFrame will receive events for the main frame and must not
    // be null.
    virtual void setMainFrame(WebFrame*) = 0;

    // Initializes the various client interfaces.
    virtual void setCredentialManagerClient(WebCredentialManagerClient*) = 0;
    virtual void setPrerendererClient(WebPrerendererClient*) = 0;
    virtual void setSpellCheckClient(WebSpellCheckClient*) = 0;

    // Options -------------------------------------------------------------

    // The returned pointer is valid for the lifetime of the WebView.
    virtual WebSettings* settings() = 0;

    // Corresponds to the encoding of the main frame.  Setting the page
    // encoding may cause the main frame to reload.
    virtual WebString pageEncoding() const = 0;
    virtual void setPageEncoding(const WebString&) = 0;

    // Makes the WebView transparent.  This is useful if you want to have
    // some custom background rendered behind it.
    virtual bool isTransparent() const = 0;
    virtual void setIsTransparent(bool) = 0;

    // Sets the base color used for this WebView's background. This is in effect
    // the default background color used for pages with no background-color
    // style in effect, or used as the alpha-blended basis for any pages with
    // translucent background-color style. (For pages with opaque
    // background-color style, this property is effectively ignored).
    // Setting this takes effect for the currently loaded page, if any, and
    // persists across subsequent navigations. Defaults to white prior to the
    // first call to this method.
    virtual void setBaseBackgroundColor(WebColor) = 0;

    // Controls whether pressing Tab key advances focus to links.
    virtual bool tabsToLinks() const = 0;
    virtual void setTabsToLinks(bool) = 0;

    // Method that controls whether pressing Tab key cycles through page
    // elements or inserts a '\t' char in the focused text area.
    virtual bool tabKeyCyclesThroughElements() const = 0;
    virtual void setTabKeyCyclesThroughElements(bool) = 0;

    // Controls the WebView's active state, which may affect the rendering
    // of elements on the page (i.e., tinting of input elements).
    virtual bool isActive() const = 0;
    virtual void setIsActive(bool) = 0;

    // Allows disabling domain relaxation.
    virtual void setDomainRelaxationForbidden(bool, const WebString& scheme) = 0;

    // Allows setting the state of the various bars exposed via BarProp
    // properties on the window object. The size related fields of
    // WebWindowFeatures are ignored.
    virtual void setWindowFeatures(const WebWindowFeatures&) = 0;

    // Marks the WebView as being opened by a DOM call. This is relevant
    // for whether window.close() may be called.
    virtual void setOpenedByDOM() = 0;


    // Frames --------------------------------------------------------------

    virtual WebFrame* mainFrame() = 0;

    // Returns the frame identified by the given name.  This method
    // supports pseudo-names like _self, _top, and _blank.  It traverses
    // the entire frame tree containing this tree looking for a frame that
    // matches the given name.  If the optional relativeToFrame parameter
    // is specified, then the search begins with the given frame and its
    // children.
    virtual WebFrame* findFrameByName(
        const WebString& name, WebFrame* relativeToFrame = 0) = 0;


    // Focus ---------------------------------------------------------------

    virtual WebFrame* focusedFrame() = 0;
    virtual void setFocusedFrame(WebFrame*) = 0;

    // Focus the first (last if reverse is true) focusable node.
    virtual void setInitialFocus(bool reverse) = 0;

    // Clears the focused element (and selection if a text field is focused)
    // to ensure that a text field on the page is not eating keystrokes we
    // send it.
    virtual void clearFocusedElement() = 0;

    // Scrolls the node currently in focus into |rect|, where |rect| is in
    // viewport space. Returns true if an animation was started.
    virtual bool scrollFocusedNodeIntoRect(const WebRect&) { return false; }

    // Advance the focus of the WebView forward to the next element or to the
    // previous element in the tab sequence (if reverse is true).
    virtual void advanceFocus(bool reverse) { }

    // Animate a scale into the specified rect where multiple targets were
    // found from previous tap gesture.
    // Returns false if it doesn't do any zooming.
    virtual bool zoomToMultipleTargetsRect(const WebRect&) = 0;


    // Zoom ----------------------------------------------------------------

    // Returns the current zoom level.  0 is "original size", and each increment
    // above or below represents zooming 20% larger or smaller to default limits
    // of 300% and 50% of original size, respectively.  Only plugins use
    // non whole-numbers, since they might choose to have specific zoom level so
    // that fixed-width content is fit-to-page-width, for example.
    virtual double zoomLevel() = 0;

    // Changes the zoom level to the specified level, clamping at the limits
    // noted above, and returns the current zoom level after applying the
    // change.
    virtual double setZoomLevel(double) = 0;

    // Updates the zoom limits for this view.
    virtual void zoomLimitsChanged(double minimumZoomLevel,
                                   double maximumZoomLevel) = 0;

    // Helper functions to convert between zoom level and zoom factor.  zoom
    // factor is zoom percent / 100, so 300% = 3.0.
    BLINK_EXPORT static double zoomLevelToZoomFactor(double zoomLevel);
    BLINK_EXPORT static double zoomFactorToZoomLevel(double factor);

    // Returns the current text zoom factor, where 1.0 is the normal size, > 1.0
    // is scaled up and < 1.0 is scaled down.
    virtual float textZoomFactor() = 0;

    // Scales the text in the page by a factor of textZoomFactor.
    // Note: this has no effect on plugins.
    virtual float setTextZoomFactor(float) = 0;

    // Gets the scale factor of the page, where 1.0 is the normal size, > 1.0
    // is scaled up, < 1.0 is scaled down.
    virtual float pageScaleFactor() const = 0;

    // TODO: Obsolete, the origin parameter is ambiguous with two viewports. Remove
    // once Chromium side users are removed.
    // Scales a page by a factor of scaleFactor and then sets a scroll position to (x, y).
    // setPageScaleFactor() magnifies and shrinks a page without affecting layout.
    // On the other hand, zooming affects layout of the page.
    virtual void setPageScaleFactor(float scaleFactor, const WebPoint& origin) { setPageScaleFactor(scaleFactor); }

    // Scales the page without affecting layout by using the pinch-to-zoom viewport.
    virtual void setPageScaleFactor(float) = 0;

    // Sets the offset of the pinch-to-zoom viewport within the main frame, in
    // partial CSS pixels. The offset will be clamped so the pinch viewport
    // stays within the frame's bounds.
    virtual void setPinchViewportOffset(const WebFloatPoint&) = 0;

    // Gets the pinch viewport's current offset within the page's main frame,
    // in partial CSS pixels.
    virtual WebFloatPoint pinchViewportOffset() const = 0;

    // Sets the default minimum, and maximum page scale. These will be overridden
    // by the page or by the overrides below if they are set.
    virtual void setDefaultPageScaleLimits(
        float minScale,
        float maxScale) = 0;

    // Sets the initial page scale to the given factor. This scale setting overrides
    // page scale set in the page's viewport meta tag.
    virtual void setInitialPageScaleOverride(float) = 0;

    // Sets the maximum page scale considered to be legible. Automatic zooms (e.g, double-tap
    // or find in page) will have the page scale limited to this value times the font scale
    // factor. Manual pinch zoom will not be affected by this limit.
    virtual void setMaximumLegibleScale(float) = 0;

    // Reset any saved values for the scroll and scale state.
    virtual void resetScrollAndScaleState() = 0;

    // Prevent the web page from setting min/max scale via the viewport meta
    // tag. This is an accessibility feature that lets folks zoom in to web
    // pages even if the web page tries to block scaling.
    virtual void setIgnoreViewportTagScaleLimits(bool) = 0;

    // Returns the "preferred" contents size, defined as the preferred minimum width of the main document's contents
    // and the minimum height required to display the main document without scrollbars.
    // The returned size has the page zoom factor applied.
    virtual WebSize contentsPreferredMinimumSize() = 0;

    // Sets the display mode of the web app.
    virtual void setDisplayMode(WebDisplayMode) = 0;

    // The ratio of the current device's screen DPI to the target device's screen DPI.
    virtual float deviceScaleFactor() const = 0;

    // Sets the ratio as computed by computePageScaleConstraints.
    virtual void setDeviceScaleFactor(float) = 0;

    // Set and reset the device color profile.
    virtual void setDeviceColorProfile(const WebVector<char>&) = 0;
    virtual void resetDeviceColorProfile() = 0;

    // Auto-Resize -----------------------------------------------------------

    // In auto-resize mode, the view is automatically adjusted to fit the html
    // content within the given bounds.
    virtual void enableAutoResizeMode(
        const WebSize& minSize,
        const WebSize& maxSize) = 0;

    // Turn off auto-resize.
    virtual void disableAutoResizeMode() = 0;

    // Media ---------------------------------------------------------------

    // Performs the specified media player action on the node at the given location.
    virtual void performMediaPlayerAction(
        const WebMediaPlayerAction&, const WebPoint& location) = 0;

    // Performs the specified plugin action on the node at the given location.
    virtual void performPluginAction(
        const WebPluginAction&, const WebPoint& location) = 0;


    // Data exchange -------------------------------------------------------

    // Do a hit test at given point and return the HitTestResult.
    virtual WebHitTestResult hitTestResultAt(const WebPoint&) = 0;

    // Do a hit test equivalent to what would be done for a GestureTap event
    // that has width/height corresponding to the supplied |tapArea|.
    virtual WebHitTestResult hitTestResultForTap(const WebPoint& tapPoint,
        const WebSize& tapArea) = 0;

    // Copy to the clipboard the image located at a particular point in the
    // WebView (if there is such an image)
    virtual void copyImageAt(const WebPoint&) = 0;

    // Save as the image located at a particular point in the
    // WebView (if there is such an image)
    virtual void saveImageAt(const WebPoint&) = 0;

    // Notifies the WebView that a drag has terminated.
    virtual void dragSourceEndedAt(
        const WebPoint& clientPoint, const WebPoint& screenPoint,
        WebDragOperation operation) = 0;

    // Notfies the WebView that the system drag and drop operation has ended.
    virtual void dragSourceSystemDragEnded() = 0;

    // Callback methods when a drag-and-drop operation is trying to drop
    // something on the WebView.
    virtual WebDragOperation dragTargetDragEnter(
        const WebDragData&,
        const WebPoint& clientPoint, const WebPoint& screenPoint,
        WebDragOperationsMask operationsAllowed,
        int modifiers) = 0;
    virtual WebDragOperation dragTargetDragOver(
        const WebPoint& clientPoint, const WebPoint& screenPoint,
        WebDragOperationsMask operationsAllowed,
        int modifiers) = 0;
    virtual void dragTargetDragLeave() = 0;
    virtual void dragTargetDrop(
        const WebPoint& clientPoint, const WebPoint& screenPoint,
        int modifiers) = 0;

    // Retrieves a list of spelling markers.
    virtual void spellingMarkers(WebVector<uint32_t>* markers) = 0;
    virtual void removeSpellingMarkersUnderWords(const WebVector<WebString>& words) = 0;

    // Support for resource loading initiated by plugins -------------------

    // Returns next unused request identifier which is unique within the
    // parent Page.
    virtual unsigned long createUniqueIdentifierForRequest() = 0;


    // Developer tools -----------------------------------------------------

    // Enables device emulation as specified in params.
    virtual void enableDeviceEmulation(const WebDeviceEmulationParams&) = 0;

    // Cancel emulation started via |enableDeviceEmulation| call.
    virtual void disableDeviceEmulation() = 0;

#ifdef IMPLEMENTED_NEWEST_BLINK
    // Accessibility -------------------------------------------------------

    // Returns the accessibility object for this view.
    virtual WebAXObject accessibilityObject() = 0;
#endif // IMPLEMENTED_NEWEST_BLINK

    // Context menu --------------------------------------------------------

    virtual void performCustomContextMenuAction(unsigned action) = 0;

    // Shows a context menu for the currently focused element.
    virtual void showContextMenu() = 0;


    // SmartClip support ---------------------------------------------------
    virtual void extractSmartClipData(WebRect initRect, WebString& text, WebString& html, WebRect& resultRect) = 0;


    // Popup menu ----------------------------------------------------------

    // Sets whether select popup menus should be rendered by the browser.
    BLINK_EXPORT static void setUseExternalPopupMenus(bool);

    // Hides any popup (suggestions, selects...) that might be showing.
    virtual void hidePopups() = 0;


    // Visited link state --------------------------------------------------

    // Tells all WebView instances to update the visited link state for the
    // specified hash.
    BLINK_EXPORT static void updateVisitedLinkState(unsigned long long hash);

    // Tells all WebView instances to update the visited state for all
    // their links.
    BLINK_EXPORT static void resetVisitedLinkState();


    // Custom colors -------------------------------------------------------

    virtual void setSelectionColors(unsigned activeBackgroundColor,
                                    unsigned activeForegroundColor,
                                    unsigned inactiveBackgroundColor,
                                    unsigned inactiveForegroundColor) = 0;

    // Modal dialog support ------------------------------------------------

    // Call these methods before and after running a nested, modal event loop
    // to suspend script callbacks and resource loads.
    BLINK_EXPORT static void willEnterModalLoop();
    BLINK_EXPORT static void didExitModalLoop();

    // Called to inform the WebView that a wheel fling animation was started externally (for instance
    // by the compositor) but must be completed by the WebView.
    virtual void transferActiveWheelFlingAnimation(const WebActiveWheelFlingParameters&) = 0;

    // Cancels an active fling, returning true if a fling was active.
    virtual bool endActiveFlingAnimation() = 0;

    virtual void setShowPaintRects(bool) = 0;
    virtual void setShowFPSCounter(bool) = 0;
    virtual void setContinuousPaintingEnabled(bool) = 0;
    virtual void setShowScrollBottleneckRects(bool) = 0;

    // Visibility -----------------------------------------------------------

    // Sets the visibility of the WebView.
    virtual void setVisibilityState(WebPageVisibilityState visibilityState,
                                    bool isInitialState) { }

    // PageOverlay ----------------------------------------------------------

    // Adds/removes page overlay to this WebView. These functions change the
    // graphical appearance of the WebView. WebPageOverlay paints the
    // contents of the page overlay. It also provides an z-order number for
    // the page overlay. The z-order number defines the paint order the page
    // overlays. Page overlays with larger z-order number will be painted after
    // page overlays with smaller z-order number. That is, they appear above
    // the page overlays with smaller z-order number. If two page overlays have
    // the same z-order number, the later added one will be on top.
    virtual void addPageOverlay(WebPageOverlay*, int /*z-order*/) = 0;
    virtual void removePageOverlay(WebPageOverlay*) = 0;


    // i18n -----------------------------------------------------------------

    // Inform the WebView that the accept languages have changed.
    // If the WebView wants to get the accept languages value, it will have
    // to call the WebViewClient::acceptLanguages().
    virtual void acceptLanguagesChanged() = 0;

    // Testing functionality for TestRunner ---------------------------------

    // Force the webgl context to fail so that webglcontextcreationerror
    // event gets generated/tested.
    virtual void forceNextWebGLContextCreationToFail() = 0;

    // Force the drawing buffer used by webgl contexts to fail so that the webgl
    // context's ability to deal with that failure gracefully can be tested.
    virtual void forceNextDrawingBufferCreationToFail() = 0;

protected:
    ~WebView() {}

   public:
    virtual void setMainFrameTopContentInset(int offset) = 0;
};

} // namespace blink

#endif
