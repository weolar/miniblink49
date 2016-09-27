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

#ifndef WebSettings_h
#define WebSettings_h

#include "../platform/WebCommon.h"
#include "../platform/WebSize.h"
#include <unicode/uscript.h>

namespace blink {

class WebString;

// WebSettings is owned by the WebView and allows code to modify the settings for
// the WebView's page without any knowledge of WebCore itself.  For the most part,
// these functions have a 1:1 mapping with the methods in WebCore/page/Settings.h.
class WebSettings {
public:
    enum ImageAnimationPolicy {
        ImageAnimationPolicyAllowed,
        ImageAnimationPolicyAnimateOnce,
        ImageAnimationPolicyNoAnimation
    };

    enum EditingBehavior {
        EditingBehaviorMac,
        EditingBehaviorWin,
        EditingBehaviorUnix,
        EditingBehaviorAndroid
    };

    enum V8CacheOptions {
        V8CacheOptionsDefault,
        V8CacheOptionsNone,
        V8CacheOptionsParse,
        V8CacheOptionsCode,
    };

    // Bit field values indicating available pointer types. Identical to
    // blink::PointerType enums, enforced by compile-time assertions in
    // AssertMatchingEnums.cpp.
    // TODO(mustaq): Move this into public/platform, like WebBlendMode.
    enum PointerType {
        PointerTypeNone = 1 << 0,
        PointerTypeCoarse = 1 << 1,
        PointerTypeFine = 1 << 2
    };

    // Bit field values indicating available hover types. Identical to
    // blink::HoverType enums, enforced by compile-time assertions in
    // AssertMatchingEnums.cpp.
    enum HoverType {
        HoverTypeNone = 1 << 0,
        // Indicates that the primary pointing system can hover, but it requires
        // a significant action on the user's part. e.g. hover on "long press".
        HoverTypeOnDemand = 1 << 1,
        HoverTypeHover = 1 << 2
    };

    // Selection strategy defines how the selection granularity changes when the
    // selection extent is moved.
    enum class SelectionStrategyType {
        // Always uses character granularity.
        Character,
        // "Expand by word, shrink by character" selection strategy.
        // Uses character granularity when selection is shrinking. If the
        // selection is expanding, granularity doesn't change until a word
        // boundary is passed, after which the granularity switches to "word".
        Direction
    };

    // Defines user preference for text track kind.
    enum class TextTrackKindUserPreference {
        // Display only tracks marked as default
        Default,
        // If available, display captions track in preferred language, else display subtitles.
        Captions,
        // If available, display subtitles track in preferred language, else display captions.
        Subtitles
    };

    // Sets value of a setting by its string identifier from Settings.in and
    // string representation of value. An enum's string representation is the
    // string representation of the integer value of the enum.
    virtual void setFromStrings(const WebString& name, const WebString& value) = 0;

    virtual bool mainFrameResizesAreOrientationChanges() const = 0;
    virtual int availablePointerTypes() const = 0;
    virtual PointerType primaryPointerType() const = 0;
    virtual int availableHoverTypes() const = 0;
    virtual HoverType primaryHoverType() const = 0;
    virtual bool shrinksViewportContentToFit() const = 0;
    virtual bool viewportEnabled() const = 0;
    virtual void setAccelerated2dCanvasEnabled(bool) = 0;
    virtual void setAccelerated2dCanvasMSAASampleCount(int) = 0;
    virtual void setAcceleratedCompositingEnabled(bool) = 0;
    virtual void setPreferCompositingToLCDTextEnabled(bool) = 0;
    // Not implemented yet, see http://crbug.com/178119
    virtual void setAcceleratedCompositingForTransitionEnabled(bool) { }
    // If set to true, allows frames with an https origin to display passive
    // contents at an insecure URL. Otherwise, disallows it. The
    // FrameLoaderClient set to the frame may override the value set by this
    // method.
    virtual void setAccessibilityEnabled(bool) = 0;
    virtual void setAccessibilityPasswordValuesEnabled(bool) = 0;
    virtual void setAllowDisplayOfInsecureContent(bool) = 0;
    virtual void setAllowFileAccessFromFileURLs(bool) = 0;
    virtual void setAllowCustomScrollbarInMainFrame(bool) = 0;
    // If set to true, allows frames with an https origin to run active
    // contents at an insecure URL. This includes WebSockets. Otherwise,
    // disallows it. The FrameLoaderClient set to the frame may override the
    // value set by this method.
    virtual void setAllowRunningOfInsecureContent(bool) = 0;
    virtual void setAllowScriptsToCloseWindows(bool) = 0;
    virtual void setAllowUniversalAccessFromFileURLs(bool) = 0;
    virtual void setAntialiased2dCanvasEnabled(bool) = 0;
    virtual void setAntialiasedClips2dCanvasEnabled(bool) = 0;
    virtual void setAsynchronousSpellCheckingEnabled(bool) = 0;
    virtual void setAutoZoomFocusedNodeToLegibleScale(bool) = 0;
    virtual void setCaretBrowsingEnabled(bool) = 0;
    virtual void setClobberUserAgentInitialScaleQuirk(bool) = 0;
    virtual void setCookieEnabled(bool) = 0;
    virtual void setNavigateOnDragDrop(bool) = 0;
    virtual void setCursiveFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setDNSPrefetchingEnabled(bool) = 0;
    virtual void setDOMPasteAllowed(bool) = 0;
    virtual void setDefaultFixedFontSize(int) = 0;
    virtual void setDefaultFontSize(int) = 0;
    virtual void setDefaultTextEncodingName(const WebString&) = 0;
    virtual void setDefaultVideoPosterURL(const WebString&) = 0;
    void setDeferred2dCanvasEnabled(bool) { } // temporary stub
    virtual void setDeviceScaleAdjustment(float) = 0;
    virtual void setDeviceSupportsMouse(bool) = 0;
    virtual void setDeviceSupportsTouch(bool) = 0;
    virtual void setDisableReadingFromCanvas(bool) = 0;
    virtual void setDoubleTapToZoomEnabled(bool) = 0;
    virtual void setDownloadableBinaryFontsEnabled(bool) = 0;
    virtual void setEditingBehavior(EditingBehavior) = 0;
    virtual void setEnableScrollAnimator(bool) = 0;
    virtual void setEnableTouchAdjustment(bool) = 0;
    virtual bool multiTargetTapNotificationEnabled() = 0;
    virtual void setMultiTargetTapNotificationEnabled(bool) = 0;
    virtual void setExperimentalWebGLEnabled(bool) = 0;
    virtual void setFantasyFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setFixedFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setReportWheelOverscroll(bool) = 0;
    virtual void setForceZeroLayoutHeight(bool) = 0;
    virtual void setFullscreenSupported(bool) = 0;
    virtual void setHyperlinkAuditingEnabled(bool) = 0;
    virtual void setIgnoreMainFrameOverflowHiddenQuirk(bool) = 0;
    virtual void setImageAnimationPolicy(ImageAnimationPolicy) = 0;
    virtual void setImagesEnabled(bool) = 0;
    virtual void setInlineTextBoxAccessibilityEnabled(bool) = 0;
    virtual void setInvertViewportScrollOrder(bool) = 0;
    virtual void setJavaEnabled(bool) = 0;
    virtual void setJavaScriptCanAccessClipboard(bool) = 0;
    virtual void setJavaScriptCanOpenWindowsAutomatically(bool) = 0;
    virtual void setJavaScriptEnabled(bool) = 0;
    void setLayerSquashingEnabled(bool) { }
    virtual void setLoadsImagesAutomatically(bool) = 0;
    virtual void setLoadWithOverviewMode(bool) = 0;
    virtual void setLocalStorageEnabled(bool) = 0;
    virtual void setMainFrameClipsContent(bool) = 0;
    virtual void setMainFrameResizesAreOrientationChanges(bool) = 0;
    virtual void setMaxTouchPoints(int) = 0;
    virtual void setMediaControlsOverlayPlayButtonEnabled(bool) = 0;
    virtual void setMediaPlaybackRequiresUserGesture(bool) = 0;
    virtual void setMinimumAccelerated2dCanvasSize(int) = 0;
    virtual void setMinimumFontSize(int) = 0;
    virtual void setMinimumLogicalFontSize(int) = 0;
    virtual void setMockScrollbarsEnabled(bool) = 0;
    virtual void setOfflineWebApplicationCacheEnabled(bool) = 0;
    virtual void setOpenGLMultisamplingEnabled(bool) = 0;
    virtual void setPasswordEchoDurationInSeconds(double) = 0;
    virtual void setPasswordEchoEnabled(bool) = 0;
    virtual void setPerTilePaintingEnabled(bool) = 0;
    virtual void setPictographFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setPinchOverlayScrollbarThickness(int) = 0;
    virtual void setPluginsEnabled(bool) = 0;
    virtual void setAvailablePointerTypes(int) = 0;
    virtual void setPrimaryPointerType(PointerType) = 0;
    virtual void setAvailableHoverTypes(int) = 0;
    virtual void setPrimaryHoverType(HoverType) = 0;
    virtual void setRenderVSyncNotificationEnabled(bool) = 0;
    virtual void setReportScreenSizeInPhysicalPixelsQuirk(bool) = 0;
    virtual void setRootLayerScrolls(bool) = 0;
    virtual void setRubberBandingOnCompositorThread(bool) = 0;
    virtual void setSansSerifFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setSelectTrailingWhitespaceEnabled(bool) = 0;
    virtual void setSelectionIncludesAltImageText(bool) = 0;
    virtual void setSelectionStrategy(SelectionStrategyType) = 0;
    virtual void setSerifFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setShouldPrintBackgrounds(bool) = 0;
    virtual void setShouldClearDocumentBackground(bool) = 0;
    virtual void setShouldRespectImageOrientation(bool) = 0;
    virtual void setShowContextMenuOnMouseUp(bool) = 0;
    virtual void setShowFPSCounter(bool) = 0;
    virtual void setShowPaintRects(bool) = 0;
    virtual void setShrinksViewportContentToFit(bool) = 0;
    virtual void setSmartInsertDeleteEnabled(bool) = 0;
    // Spatial navigation feature, when enabled, improves the experience
    // of keyboard-controlling the web pages which originally were not designed
    // for keyboard navigation. It allows to use arrow keys to move focus between
    // the adjacent HTML elements. As a side effect, it extends the criteria for
    // elements to be focusable to include any element which has click or keyboard
    // event handlers specified. User can also trigger click handlers for such
    // elements using SPACE or ENTER keys.
    virtual void setSpatialNavigationEnabled(bool) = 0;
    virtual void setStandardFontFamily(const WebString&, UScriptCode = USCRIPT_COMMON) = 0;
    virtual void setStrictMixedContentChecking(bool) = 0;
    virtual void setStrictPowerfulFeatureRestrictions(bool) = 0;
    virtual void setSupportDeprecatedTargetDensityDPI(bool) = 0;
    virtual void setSupportsMultipleWindows(bool) = 0;
    virtual void setSyncXHRInDocumentsEnabled(bool) = 0;
    virtual void setTextAreasAreResizable(bool) = 0;
    virtual void setTextAutosizingEnabled(bool) = 0;
    virtual void setAccessibilityFontScaleFactor(float) = 0;
    virtual void setTextTrackKindUserPreference(TextTrackKindUserPreference) = 0;
    virtual void setTextTrackBackgroundColor(const WebString&) = 0;
    virtual void setTextTrackFontFamily(const WebString&) = 0;
    virtual void setTextTrackFontStyle(const WebString&) = 0;
    virtual void setTextTrackFontVariant(const WebString&) = 0;
    virtual void setTextTrackTextColor(const WebString&) = 0;
    virtual void setTextTrackTextShadow(const WebString&) = 0;
    virtual void setTextTrackTextSize(const WebString&) = 0;
    virtual void setThreadedScrollingEnabled(bool) = 0;
    virtual void setTouchDragDropEnabled(bool) = 0;
    virtual void setTouchEditingEnabled(bool) = 0;
    virtual void setUnifiedTextCheckerEnabled(bool) = 0;
    virtual void setUnsafePluginPastingEnabled(bool) = 0;
    virtual void setUseLegacyBackgroundSizeShorthandBehavior(bool) = 0;
    virtual void setUseMobileViewportStyle(bool) = 0;
    virtual void setUseSolidColorScrollbars(bool) = 0;
    virtual void setUseWideViewport(bool) = 0;
    virtual void setUsesEncodingDetector(bool) = 0;
    virtual void setV8CacheOptions(V8CacheOptions) = 0;
    virtual void setValidationMessageTimerMagnification(int) = 0;
    virtual void setViewportEnabled(bool) = 0;
    virtual void setViewportMetaEnabled(bool) = 0;
    virtual void setViewportMetaLayoutSizeQuirk(bool) = 0;
    virtual void setViewportMetaMergeContentQuirk(bool) = 0;
    virtual void setViewportMetaNonUserScalableQuirk(bool) = 0;
    virtual void setViewportMetaZeroValuesQuirk(bool) = 0;
    virtual void setWebAudioEnabled(bool) = 0;
    virtual void setWebGLErrorsToConsoleEnabled(bool) = 0;
    virtual void setWebSecurityEnabled(bool) = 0;
    virtual void setWideViewportQuirkEnabled(bool) = 0;
    virtual void setXSSAuditorEnabled(bool) = 0;

protected:
    ~WebSettings() { }
};

} // namespace blink

#endif
