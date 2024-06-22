// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalSettingsGenerated_h
#define InternalSettingsGenerated_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/RefCountedSupplement.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Page;

class InternalSettingsGenerated : public RefCountedWillBeGarbageCollectedFinalized<InternalSettingsGenerated>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    explicit InternalSettingsGenerated(Page*);
    virtual ~InternalSettingsGenerated();
    void resetToConsistentState();
    void setDOMPasteAllowed(bool DOMPasteAllowed);
    void setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled);
    void setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount);
    void setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled);
    void setAccessibilityEnabled(bool accessibilityEnabled);
    void setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor);
    void setAccessibilityPasswordValuesEnabled(bool accessibilityPasswordValuesEnabled);
    void setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame);
    void setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent);
    void setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs);
    void setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent);
    void setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows);
    void setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs);
    void setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled);
    void setAntialiasedClips2dCanvasEnabled(bool antialiasedClips2dCanvasEnabled);
    void setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled);
    void setAvailableHoverTypes(int availableHoverTypes);
    void setAvailablePointerTypes(int availablePointerTypes);
    void setBackgroundHtmlParserOutstandingTokenLimit(unsigned backgroundHtmlParserOutstandingTokenLimit);
    void setBackgroundHtmlParserPendingTokenLimit(unsigned backgroundHtmlParserPendingTokenLimit);
    void setCaretBrowsingEnabled(bool caretBrowsingEnabled);
    void setCookieEnabled(bool cookieEnabled);
    void setDefaultFixedFontSize(int defaultFixedFontSize);
    void setDefaultFontSize(int defaultFontSize);
    void setDefaultTextEncodingName(const String& defaultTextEncodingName);
    void setDefaultVideoPosterURL(const String& defaultVideoPosterURL);
    void setDeviceScaleAdjustment(double deviceScaleAdjustment);
    void setDeviceSupportsMouse(bool deviceSupportsMouse);
    void setDeviceSupportsTouch(bool deviceSupportsTouch);
    void setDisableReadingFromCanvas(bool disableReadingFromCanvas);
    void setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled);
    void setDoHtmlPreloadScanning(bool doHtmlPreloadScanning);
    void setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled);
    void setForceZeroLayoutHeight(bool forceZeroLayoutHeight);
    void setFullscreenSupported(bool fullscreenSupported);
    void setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled);
    void setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk);
    void setImagesEnabled(bool imagesEnabled);
    void setInlineTextBoxAccessibilityEnabled(bool inlineTextBoxAccessibilityEnabled);
    void setInvertViewportScrollOrder(bool invertViewportScrollOrder);
    void setJavaEnabled(bool javaEnabled);
    void setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard);
    void setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically);
    void setLayerSquashingEnabled(bool layerSquashingEnabled);
    void setLoadWithOverviewMode(bool loadWithOverviewMode);
    void setLoadsImagesAutomatically(bool loadsImagesAutomatically);
    void setLocalStorageEnabled(bool localStorageEnabled);
    void setLogDnsPrefetchAndPreconnect(bool logDnsPrefetchAndPreconnect);
    void setLogPreload(bool logPreload);
    void setLowPriorityIframes(bool lowPriorityIframes);
    void setMainFrameClipsContent(bool mainFrameClipsContent);
    void setMaxBeaconTransmission(int maxBeaconTransmission);
    void setMaxTouchPoints(int maxTouchPoints);
    void setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled);
    void setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture);
    void setMediaTypeOverride(const String& mediaTypeOverride);
    void setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize);
    void setMinimumFontSize(int minimumFontSize);
    void setMinimumLogicalFontSize(int minimumLogicalFontSize);
    void setMockGestureTapHighlightsEnabled(bool mockGestureTapHighlightsEnabled);
    void setMultiTargetTapNotificationEnabled(bool multiTargetTapNotificationEnabled);
    void setNavigateOnDragDrop(bool navigateOnDragDrop);
    void setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled);
    void setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds);
    void setPasswordEchoEnabled(bool passwordEchoEnabled);
    void setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness);
    void setPluginsEnabled(bool pluginsEnabled);
    void setPreferCompositingToLCDTextEnabled(bool preferCompositingToLCDTextEnabled);
    void setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk);
    void setReportWheelOverscroll(bool reportWheelOverscroll);
    void setRootLayerScrolls(bool rootLayerScrolls);
    void setRubberBandingOnCompositorThread(bool rubberBandingOnCompositorThread);
    void setScriptEnabled(bool scriptEnabled);
    void setScrollAnimatorEnabled(bool scrollAnimatorEnabled);
    void setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled);
    void setSelectionIncludesAltImageText(bool selectionIncludesAltImageText);
    void setShouldClearDocumentBackground(bool shouldClearDocumentBackground);
    void setShouldPrintBackgrounds(bool shouldPrintBackgrounds);
    void setShouldRespectImageOrientation(bool shouldRespectImageOrientation);
    void setShowContextMenuOnMouseUp(bool showContextMenuOnMouseUp);
    void setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled);
    void setSpatialNavigationEnabled(bool spatialNavigationEnabled);
    void setStrictMixedContentChecking(bool strictMixedContentChecking);
    void setStrictPowerfulFeatureRestrictions(bool strictPowerfulFeatureRestrictions);
    void setSupportsMultipleWindows(bool supportsMultipleWindows);
    void setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled);
    void setTextAreasAreResizable(bool textAreasAreResizable);
    void setTextTrackBackgroundColor(const String& textTrackBackgroundColor);
    void setTextTrackFontFamily(const String& textTrackFontFamily);
    void setTextTrackFontStyle(const String& textTrackFontStyle);
    void setTextTrackFontVariant(const String& textTrackFontVariant);
    void setTextTrackTextColor(const String& textTrackTextColor);
    void setTextTrackTextShadow(const String& textTrackTextShadow);
    void setTextTrackTextSize(const String& textTrackTextSize);
    void setThreadedScrollingEnabled(bool threadedScrollingEnabled);
    void setTouchAdjustmentEnabled(bool touchAdjustmentEnabled);
    void setTouchDragDropEnabled(bool touchDragDropEnabled);
    void setTouchEditingEnabled(bool touchEditingEnabled);
    void setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled);
    void setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled);
    void setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior);
    void setUseMobileViewportStyle(bool useMobileViewportStyle);
    void setUseSolidColorScrollbars(bool useSolidColorScrollbars);
    void setUseWideViewport(bool useWideViewport);
    void setUsesEncodingDetector(bool usesEncodingDetector);
    void setValidationMessageTimerMagnification(int validationMessageTimerMagnification);
    void setViewportEnabled(bool viewportEnabled);
    void setViewportMetaEnabled(bool viewportMetaEnabled);
    void setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk);
    void setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk);
    void setWebAudioEnabled(bool webAudioEnabled);
    void setWebGLEnabled(bool webGLEnabled);
    void setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled);
    void setWebSecurityEnabled(bool webSecurityEnabled);
    void setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled);
    void setXSSAuditorEnabled(bool xssAuditorEnabled);

    DEFINE_INLINE_VIRTUAL_TRACE() { }

private:
    Page* m_page;

    bool m_DOMPasteAllowed;
    bool m_accelerated2dCanvasEnabled;
    int m_accelerated2dCanvasMSAASampleCount;
    bool m_acceleratedCompositingEnabled;
    bool m_accessibilityEnabled;
    double m_accessibilityFontScaleFactor;
    bool m_accessibilityPasswordValuesEnabled;
    bool m_allowCustomScrollbarInMainFrame;
    bool m_allowDisplayOfInsecureContent;
    bool m_allowFileAccessFromFileURLs;
    bool m_allowRunningOfInsecureContent;
    bool m_allowScriptsToCloseWindows;
    bool m_allowUniversalAccessFromFileURLs;
    bool m_antialiased2dCanvasEnabled;
    bool m_antialiasedClips2dCanvasEnabled;
    bool m_asynchronousSpellCheckingEnabled;
    int m_availableHoverTypes;
    int m_availablePointerTypes;
    unsigned m_backgroundHtmlParserOutstandingTokenLimit;
    unsigned m_backgroundHtmlParserPendingTokenLimit;
    bool m_caretBrowsingEnabled;
    bool m_cookieEnabled;
    int m_defaultFixedFontSize;
    int m_defaultFontSize;
    String m_defaultTextEncodingName;
    String m_defaultVideoPosterURL;
    double m_deviceScaleAdjustment;
    bool m_deviceSupportsMouse;
    bool m_deviceSupportsTouch;
    bool m_disableReadingFromCanvas;
    bool m_dnsPrefetchingEnabled;
    bool m_doHtmlPreloadScanning;
    bool m_downloadableBinaryFontsEnabled;
    bool m_forceZeroLayoutHeight;
    bool m_fullscreenSupported;
    bool m_hyperlinkAuditingEnabled;
    bool m_ignoreMainFrameOverflowHiddenQuirk;
    bool m_imagesEnabled;
    bool m_inlineTextBoxAccessibilityEnabled;
    bool m_invertViewportScrollOrder;
    bool m_javaEnabled;
    bool m_javaScriptCanAccessClipboard;
    bool m_javaScriptCanOpenWindowsAutomatically;
    bool m_layerSquashingEnabled;
    bool m_loadWithOverviewMode;
    bool m_loadsImagesAutomatically;
    bool m_localStorageEnabled;
    bool m_logDnsPrefetchAndPreconnect;
    bool m_logPreload;
    bool m_lowPriorityIframes;
    bool m_mainFrameClipsContent;
    int m_maxBeaconTransmission;
    int m_maxTouchPoints;
    bool m_mediaControlsOverlayPlayButtonEnabled;
    bool m_mediaPlaybackRequiresUserGesture;
    String m_mediaTypeOverride;
    int m_minimumAccelerated2dCanvasSize;
    int m_minimumFontSize;
    int m_minimumLogicalFontSize;
    bool m_mockGestureTapHighlightsEnabled;
    bool m_multiTargetTapNotificationEnabled;
    bool m_navigateOnDragDrop;
    bool m_offlineWebApplicationCacheEnabled;
    double m_passwordEchoDurationInSeconds;
    bool m_passwordEchoEnabled;
    int m_pinchOverlayScrollbarThickness;
    bool m_pluginsEnabled;
    bool m_preferCompositingToLCDTextEnabled;
    bool m_reportScreenSizeInPhysicalPixelsQuirk;
    bool m_reportWheelOverscroll;
    bool m_rootLayerScrolls;
    bool m_rubberBandingOnCompositorThread;
    bool m_scriptEnabled;
    bool m_scrollAnimatorEnabled;
    bool m_selectTrailingWhitespaceEnabled;
    bool m_selectionIncludesAltImageText;
    bool m_shouldClearDocumentBackground;
    bool m_shouldPrintBackgrounds;
    bool m_shouldRespectImageOrientation;
    bool m_showContextMenuOnMouseUp;
    bool m_smartInsertDeleteEnabled;
    bool m_spatialNavigationEnabled;
    bool m_strictMixedContentChecking;
    bool m_strictPowerfulFeatureRestrictions;
    bool m_supportsMultipleWindows;
    bool m_syncXHRInDocumentsEnabled;
    bool m_textAreasAreResizable;
    String m_textTrackBackgroundColor;
    String m_textTrackFontFamily;
    String m_textTrackFontStyle;
    String m_textTrackFontVariant;
    String m_textTrackTextColor;
    String m_textTrackTextShadow;
    String m_textTrackTextSize;
    bool m_threadedScrollingEnabled;
    bool m_touchAdjustmentEnabled;
    bool m_touchDragDropEnabled;
    bool m_touchEditingEnabled;
    bool m_unifiedTextCheckerEnabled;
    bool m_unsafePluginPastingEnabled;
    bool m_useLegacyBackgroundSizeShorthandBehavior;
    bool m_useMobileViewportStyle;
    bool m_useSolidColorScrollbars;
    bool m_useWideViewport;
    bool m_usesEncodingDetector;
    int m_validationMessageTimerMagnification;
    bool m_viewportEnabled;
    bool m_viewportMetaEnabled;
    bool m_viewportMetaMergeContentQuirk;
    bool m_viewportMetaZeroValuesQuirk;
    bool m_webAudioEnabled;
    bool m_webGLEnabled;
    bool m_webGLErrorsToConsoleEnabled;
    bool m_webSecurityEnabled;
    bool m_wideViewportQuirkEnabled;
    bool m_xssAuditorEnabled;
};

} // namespace blink

#endif // InternalSettingsGenerated_h
