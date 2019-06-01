// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SettingsMacros_h
#define SettingsMacros_h

#define SETTINGS_GETTERS_AND_SETTERS \
    bool DOMPasteAllowed() const { return m_DOMPasteAllowed; } \
    void setDOMPasteAllowed(bool DOMPasteAllowed); \
    bool accelerated2dCanvasEnabled() const { return m_accelerated2dCanvasEnabled; } \
    void setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled); \
    int accelerated2dCanvasMSAASampleCount() const { return m_accelerated2dCanvasMSAASampleCount; } \
    void setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount); \
    bool acceleratedCompositingEnabled() const { return m_acceleratedCompositingEnabled; } \
    void setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled); \
    bool accessibilityEnabled() const { return m_accessibilityEnabled; } \
    void setAccessibilityEnabled(bool accessibilityEnabled); \
    double accessibilityFontScaleFactor() const { return m_accessibilityFontScaleFactor; } \
    void setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor); \
    bool accessibilityPasswordValuesEnabled() const { return m_accessibilityPasswordValuesEnabled; } \
    void setAccessibilityPasswordValuesEnabled(bool accessibilityPasswordValuesEnabled); \
    bool allowCustomScrollbarInMainFrame() const { return m_allowCustomScrollbarInMainFrame; } \
    void setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame); \
    bool allowDisplayOfInsecureContent() const { return m_allowDisplayOfInsecureContent; } \
    void setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent); \
    bool allowFileAccessFromFileURLs() const { return m_allowFileAccessFromFileURLs; } \
    void setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs); \
    bool allowRunningOfInsecureContent() const { return m_allowRunningOfInsecureContent; } \
    void setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent); \
    bool allowScriptsToCloseWindows() const { return m_allowScriptsToCloseWindows; } \
    void setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows); \
    bool allowUniversalAccessFromFileURLs() const { return m_allowUniversalAccessFromFileURLs; } \
    void setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs); \
    bool antialiased2dCanvasEnabled() const { return m_antialiased2dCanvasEnabled; } \
    void setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled); \
    bool antialiasedClips2dCanvasEnabled() const { return m_antialiasedClips2dCanvasEnabled; } \
    void setAntialiasedClips2dCanvasEnabled(bool antialiasedClips2dCanvasEnabled); \
    bool asynchronousSpellCheckingEnabled() const { return m_asynchronousSpellCheckingEnabled; } \
    void setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled); \
    int availableHoverTypes() const { return m_availableHoverTypes; } \
    void setAvailableHoverTypes(int availableHoverTypes); \
    int availablePointerTypes() const { return m_availablePointerTypes; } \
    void setAvailablePointerTypes(int availablePointerTypes); \
    unsigned backgroundHtmlParserOutstandingTokenLimit() const { return m_backgroundHtmlParserOutstandingTokenLimit; } \
    void setBackgroundHtmlParserOutstandingTokenLimit(unsigned backgroundHtmlParserOutstandingTokenLimit); \
    unsigned backgroundHtmlParserPendingTokenLimit() const { return m_backgroundHtmlParserPendingTokenLimit; } \
    void setBackgroundHtmlParserPendingTokenLimit(unsigned backgroundHtmlParserPendingTokenLimit); \
    bool caretBrowsingEnabled() const { return m_caretBrowsingEnabled; } \
    void setCaretBrowsingEnabled(bool caretBrowsingEnabled); \
    bool cookieEnabled() const { return m_cookieEnabled; } \
    void setCookieEnabled(bool cookieEnabled); \
    int defaultFixedFontSize() const { return m_defaultFixedFontSize; } \
    void setDefaultFixedFontSize(int defaultFixedFontSize); \
    int defaultFontSize() const { return m_defaultFontSize; } \
    void setDefaultFontSize(int defaultFontSize); \
    const String& defaultTextEncodingName() const { return m_defaultTextEncodingName; } \
    void setDefaultTextEncodingName(const String& defaultTextEncodingName); \
    const String& defaultVideoPosterURL() const { return m_defaultVideoPosterURL; } \
    void setDefaultVideoPosterURL(const String& defaultVideoPosterURL); \
    double deviceScaleAdjustment() const { return m_deviceScaleAdjustment; } \
    void setDeviceScaleAdjustment(double deviceScaleAdjustment); \
    bool deviceSupportsMouse() const { return m_deviceSupportsMouse; } \
    void setDeviceSupportsMouse(bool deviceSupportsMouse); \
    bool deviceSupportsTouch() const { return m_deviceSupportsTouch; } \
    void setDeviceSupportsTouch(bool deviceSupportsTouch); \
    bool disableReadingFromCanvas() const { return m_disableReadingFromCanvas; } \
    void setDisableReadingFromCanvas(bool disableReadingFromCanvas); \
    WebDisplayMode displayModeOverride() const { return m_displayModeOverride; } \
    void setDisplayModeOverride(WebDisplayMode displayModeOverride); \
    bool dnsPrefetchingEnabled() const { return m_dnsPrefetchingEnabled; } \
    void setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled); \
    bool doHtmlPreloadScanning() const { return m_doHtmlPreloadScanning; } \
    void setDoHtmlPreloadScanning(bool doHtmlPreloadScanning); \
    bool downloadableBinaryFontsEnabled() const { return m_downloadableBinaryFontsEnabled; } \
    void setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled); \
    EditingBehaviorType editingBehaviorType() const { return m_editingBehaviorType; } \
    void setEditingBehaviorType(EditingBehaviorType editingBehaviorType); \
    bool forceZeroLayoutHeight() const { return m_forceZeroLayoutHeight; } \
    void setForceZeroLayoutHeight(bool forceZeroLayoutHeight); \
    bool fullscreenSupported() const { return m_fullscreenSupported; } \
    void setFullscreenSupported(bool fullscreenSupported); \
    bool hyperlinkAuditingEnabled() const { return m_hyperlinkAuditingEnabled; } \
    void setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled); \
    bool ignoreMainFrameOverflowHiddenQuirk() const { return m_ignoreMainFrameOverflowHiddenQuirk; } \
    void setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk); \
    ImageAnimationPolicy imageAnimationPolicy() const { return m_imageAnimationPolicy; } \
    void setImageAnimationPolicy(ImageAnimationPolicy imageAnimationPolicy); \
    bool imagesEnabled() const { return m_imagesEnabled; } \
    void setImagesEnabled(bool imagesEnabled); \
    bool inlineTextBoxAccessibilityEnabled() const { return m_inlineTextBoxAccessibilityEnabled; } \
    void setInlineTextBoxAccessibilityEnabled(bool inlineTextBoxAccessibilityEnabled); \
    bool invertViewportScrollOrder() const { return m_invertViewportScrollOrder; } \
    void setInvertViewportScrollOrder(bool invertViewportScrollOrder); \
    bool javaEnabled() const { return m_javaEnabled; } \
    void setJavaEnabled(bool javaEnabled); \
    bool javaScriptCanAccessClipboard() const { return m_javaScriptCanAccessClipboard; } \
    void setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard); \
    bool javaScriptCanOpenWindowsAutomatically() const { return m_javaScriptCanOpenWindowsAutomatically; } \
    void setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically); \
    bool layerSquashingEnabled() const { return m_layerSquashingEnabled; } \
    void setLayerSquashingEnabled(bool layerSquashingEnabled); \
    bool loadWithOverviewMode() const { return m_loadWithOverviewMode; } \
    void setLoadWithOverviewMode(bool loadWithOverviewMode); \
    bool loadsImagesAutomatically() const { return m_loadsImagesAutomatically; } \
    void setLoadsImagesAutomatically(bool loadsImagesAutomatically); \
    bool localStorageEnabled() const { return m_localStorageEnabled; } \
    void setLocalStorageEnabled(bool localStorageEnabled); \
    bool logDnsPrefetchAndPreconnect() const { return m_logDnsPrefetchAndPreconnect; } \
    void setLogDnsPrefetchAndPreconnect(bool logDnsPrefetchAndPreconnect); \
    bool logPreload() const { return m_logPreload; } \
    void setLogPreload(bool logPreload); \
    bool lowPriorityIframes() const { return m_lowPriorityIframes; } \
    void setLowPriorityIframes(bool lowPriorityIframes); \
    bool mainFrameClipsContent() const { return m_mainFrameClipsContent; } \
    void setMainFrameClipsContent(bool mainFrameClipsContent); \
    int maxBeaconTransmission() const { return m_maxBeaconTransmission; } \
    void setMaxBeaconTransmission(int maxBeaconTransmission); \
    int maxTouchPoints() const { return m_maxTouchPoints; } \
    void setMaxTouchPoints(int maxTouchPoints); \
    bool mediaControlsOverlayPlayButtonEnabled() const { return m_mediaControlsOverlayPlayButtonEnabled; } \
    void setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled); \
    bool mediaPlaybackRequiresUserGesture() const { return m_mediaPlaybackRequiresUserGesture; } \
    void setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture); \
    const String& mediaTypeOverride() const { return m_mediaTypeOverride; } \
    void setMediaTypeOverride(const String& mediaTypeOverride); \
    int minimumAccelerated2dCanvasSize() const { return m_minimumAccelerated2dCanvasSize; } \
    void setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize); \
    int minimumFontSize() const { return m_minimumFontSize; } \
    void setMinimumFontSize(int minimumFontSize); \
    int minimumLogicalFontSize() const { return m_minimumLogicalFontSize; } \
    void setMinimumLogicalFontSize(int minimumLogicalFontSize); \
    bool mockGestureTapHighlightsEnabled() const { return m_mockGestureTapHighlightsEnabled; } \
    void setMockGestureTapHighlightsEnabled(bool mockGestureTapHighlightsEnabled); \
    bool multiTargetTapNotificationEnabled() const { return m_multiTargetTapNotificationEnabled; } \
    void setMultiTargetTapNotificationEnabled(bool multiTargetTapNotificationEnabled); \
    bool navigateOnDragDrop() const { return m_navigateOnDragDrop; } \
    void setNavigateOnDragDrop(bool navigateOnDragDrop); \
    bool offlineWebApplicationCacheEnabled() const { return m_offlineWebApplicationCacheEnabled; } \
    void setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled); \
    double passwordEchoDurationInSeconds() const { return m_passwordEchoDurationInSeconds; } \
    void setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds); \
    bool passwordEchoEnabled() const { return m_passwordEchoEnabled; } \
    void setPasswordEchoEnabled(bool passwordEchoEnabled); \
    int pinchOverlayScrollbarThickness() const { return m_pinchOverlayScrollbarThickness; } \
    void setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness); \
    bool pluginsEnabled() const { return m_pluginsEnabled; } \
    void setPluginsEnabled(bool pluginsEnabled); \
    bool preferCompositingToLCDTextEnabled() const { return m_preferCompositingToLCDTextEnabled; } \
    void setPreferCompositingToLCDTextEnabled(bool preferCompositingToLCDTextEnabled); \
    HoverType primaryHoverType() const { return m_primaryHoverType; } \
    void setPrimaryHoverType(HoverType primaryHoverType); \
    PointerType primaryPointerType() const { return m_primaryPointerType; } \
    void setPrimaryPointerType(PointerType primaryPointerType); \
    bool reportScreenSizeInPhysicalPixelsQuirk() const { return m_reportScreenSizeInPhysicalPixelsQuirk; } \
    void setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk); \
    bool reportWheelOverscroll() const { return m_reportWheelOverscroll; } \
    void setReportWheelOverscroll(bool reportWheelOverscroll); \
    bool rootLayerScrolls() const { return m_rootLayerScrolls; } \
    void setRootLayerScrolls(bool rootLayerScrolls); \
    bool rubberBandingOnCompositorThread() const { return m_rubberBandingOnCompositorThread; } \
    void setRubberBandingOnCompositorThread(bool rubberBandingOnCompositorThread); \
    bool scriptEnabled() const { return m_scriptEnabled; } \
    void setScriptEnabled(bool scriptEnabled); \
    bool scrollAnimatorEnabled() const { return m_scrollAnimatorEnabled; } \
    void setScrollAnimatorEnabled(bool scrollAnimatorEnabled); \
    bool selectTrailingWhitespaceEnabled() const { return m_selectTrailingWhitespaceEnabled; } \
    void setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled); \
    bool selectionIncludesAltImageText() const { return m_selectionIncludesAltImageText; } \
    void setSelectionIncludesAltImageText(bool selectionIncludesAltImageText); \
    SelectionStrategy selectionStrategy() const { return m_selectionStrategy; } \
    void setSelectionStrategy(SelectionStrategy selectionStrategy); \
    bool shouldClearDocumentBackground() const { return m_shouldClearDocumentBackground; } \
    void setShouldClearDocumentBackground(bool shouldClearDocumentBackground); \
    bool shouldPrintBackgrounds() const { return m_shouldPrintBackgrounds; } \
    void setShouldPrintBackgrounds(bool shouldPrintBackgrounds); \
    bool shouldRespectImageOrientation() const { return m_shouldRespectImageOrientation; } \
    void setShouldRespectImageOrientation(bool shouldRespectImageOrientation); \
    bool showContextMenuOnMouseUp() const { return m_showContextMenuOnMouseUp; } \
    void setShowContextMenuOnMouseUp(bool showContextMenuOnMouseUp); \
    bool smartInsertDeleteEnabled() const { return m_smartInsertDeleteEnabled; } \
    void setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled); \
    bool spatialNavigationEnabled() const { return m_spatialNavigationEnabled; } \
    void setSpatialNavigationEnabled(bool spatialNavigationEnabled); \
    bool strictMixedContentChecking() const { return m_strictMixedContentChecking; } \
    void setStrictMixedContentChecking(bool strictMixedContentChecking); \
    bool strictPowerfulFeatureRestrictions() const { return m_strictPowerfulFeatureRestrictions; } \
    void setStrictPowerfulFeatureRestrictions(bool strictPowerfulFeatureRestrictions); \
    bool supportsMultipleWindows() const { return m_supportsMultipleWindows; } \
    void setSupportsMultipleWindows(bool supportsMultipleWindows); \
    bool syncXHRInDocumentsEnabled() const { return m_syncXHRInDocumentsEnabled; } \
    void setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled); \
    bool textAreasAreResizable() const { return m_textAreasAreResizable; } \
    void setTextAreasAreResizable(bool textAreasAreResizable); \
    const String& textTrackBackgroundColor() const { return m_textTrackBackgroundColor; } \
    void setTextTrackBackgroundColor(const String& textTrackBackgroundColor); \
    const String& textTrackFontFamily() const { return m_textTrackFontFamily; } \
    void setTextTrackFontFamily(const String& textTrackFontFamily); \
    const String& textTrackFontStyle() const { return m_textTrackFontStyle; } \
    void setTextTrackFontStyle(const String& textTrackFontStyle); \
    const String& textTrackFontVariant() const { return m_textTrackFontVariant; } \
    void setTextTrackFontVariant(const String& textTrackFontVariant); \
    TextTrackKindUserPreference textTrackKindUserPreference() const { return m_textTrackKindUserPreference; } \
    void setTextTrackKindUserPreference(TextTrackKindUserPreference textTrackKindUserPreference); \
    const String& textTrackTextColor() const { return m_textTrackTextColor; } \
    void setTextTrackTextColor(const String& textTrackTextColor); \
    const String& textTrackTextShadow() const { return m_textTrackTextShadow; } \
    void setTextTrackTextShadow(const String& textTrackTextShadow); \
    const String& textTrackTextSize() const { return m_textTrackTextSize; } \
    void setTextTrackTextSize(const String& textTrackTextSize); \
    bool threadedScrollingEnabled() const { return m_threadedScrollingEnabled; } \
    void setThreadedScrollingEnabled(bool threadedScrollingEnabled); \
    bool touchAdjustmentEnabled() const { return m_touchAdjustmentEnabled; } \
    void setTouchAdjustmentEnabled(bool touchAdjustmentEnabled); \
    bool touchDragDropEnabled() const { return m_touchDragDropEnabled; } \
    void setTouchDragDropEnabled(bool touchDragDropEnabled); \
    bool touchEditingEnabled() const { return m_touchEditingEnabled; } \
    void setTouchEditingEnabled(bool touchEditingEnabled); \
    bool unifiedTextCheckerEnabled() const { return m_unifiedTextCheckerEnabled; } \
    void setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled); \
    bool unsafePluginPastingEnabled() const { return m_unsafePluginPastingEnabled; } \
    void setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled); \
    bool useLegacyBackgroundSizeShorthandBehavior() const { return m_useLegacyBackgroundSizeShorthandBehavior; } \
    void setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior); \
    bool useMobileViewportStyle() const { return m_useMobileViewportStyle; } \
    void setUseMobileViewportStyle(bool useMobileViewportStyle); \
    bool useSolidColorScrollbars() const { return m_useSolidColorScrollbars; } \
    void setUseSolidColorScrollbars(bool useSolidColorScrollbars); \
    bool useWideViewport() const { return m_useWideViewport; } \
    void setUseWideViewport(bool useWideViewport); \
    bool usesEncodingDetector() const { return m_usesEncodingDetector; } \
    void setUsesEncodingDetector(bool usesEncodingDetector); \
    V8CacheOptions v8CacheOptions() const { return m_v8CacheOptions; } \
    void setV8CacheOptions(V8CacheOptions v8CacheOptions); \
    int validationMessageTimerMagnification() const { return m_validationMessageTimerMagnification; } \
    void setValidationMessageTimerMagnification(int validationMessageTimerMagnification); \
    bool viewportEnabled() const { return m_viewportEnabled; } \
    void setViewportEnabled(bool viewportEnabled); \
    bool viewportMetaEnabled() const { return m_viewportMetaEnabled; } \
    void setViewportMetaEnabled(bool viewportMetaEnabled); \
    bool viewportMetaMergeContentQuirk() const { return m_viewportMetaMergeContentQuirk; } \
    void setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk); \
    bool viewportMetaZeroValuesQuirk() const { return m_viewportMetaZeroValuesQuirk; } \
    void setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk); \
    bool webAudioEnabled() const { return m_webAudioEnabled; } \
    void setWebAudioEnabled(bool webAudioEnabled); \
    bool webGLEnabled() const { return m_webGLEnabled; } \
    void setWebGLEnabled(bool webGLEnabled); \
    bool webGLErrorsToConsoleEnabled() const { return m_webGLErrorsToConsoleEnabled; } \
    void setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled); \
    bool webSecurityEnabled() const { return m_webSecurityEnabled; } \
    void setWebSecurityEnabled(bool webSecurityEnabled); \
    bool wideViewportQuirkEnabled() const { return m_wideViewportQuirkEnabled; } \
    void setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled); \
    bool xssAuditorEnabled() const { return m_xssAuditorEnabled; } \
    void setXSSAuditorEnabled(bool xssAuditorEnabled); \
    void setFromStrings(const String& name, const String& value); \
    void setLanguage(const String& language); \
    const String& language()  const { return m_language; } \
    void setVendor(const String& vendor); \
    const String& Vendor()  const { return m_vendor; } \
    void setproductSub(const String& productSub); \
    const String& productSub()  const { return m_productSub; } \
// End of SETTINGS_GETTERS_AND_SETTERS.

#define SETTINGS_MEMBER_VARIABLES \
    int m_accelerated2dCanvasMSAASampleCount; \
    double m_accessibilityFontScaleFactor; \
    int m_availableHoverTypes; \
    int m_availablePointerTypes; \
    unsigned m_backgroundHtmlParserOutstandingTokenLimit; \
    unsigned m_backgroundHtmlParserPendingTokenLimit; \
    int m_defaultFixedFontSize; \
    int m_defaultFontSize; \
    String m_defaultTextEncodingName; \
    String m_defaultVideoPosterURL; \
    double m_deviceScaleAdjustment; \
    WebDisplayMode m_displayModeOverride; \
    EditingBehaviorType m_editingBehaviorType; \
    ImageAnimationPolicy m_imageAnimationPolicy; \
    int m_maxBeaconTransmission; \
    int m_maxTouchPoints; \
    String m_mediaTypeOverride; \
    int m_minimumAccelerated2dCanvasSize; \
    int m_minimumFontSize; \
    int m_minimumLogicalFontSize; \
    double m_passwordEchoDurationInSeconds; \
    int m_pinchOverlayScrollbarThickness; \
    HoverType m_primaryHoverType; \
    PointerType m_primaryPointerType; \
    SelectionStrategy m_selectionStrategy; \
    String m_textTrackBackgroundColor; \
    String m_textTrackFontFamily; \
    String m_textTrackFontStyle; \
    String m_textTrackFontVariant; \
    TextTrackKindUserPreference m_textTrackKindUserPreference; \
    String m_textTrackTextColor; \
    String m_textTrackTextShadow; \
    String m_textTrackTextSize; \
    V8CacheOptions m_v8CacheOptions; \
    int m_validationMessageTimerMagnification; \
    bool m_DOMPasteAllowed : 1; \
    bool m_accelerated2dCanvasEnabled : 1; \
    bool m_acceleratedCompositingEnabled : 1; \
    bool m_accessibilityEnabled : 1; \
    bool m_accessibilityPasswordValuesEnabled : 1; \
    bool m_allowCustomScrollbarInMainFrame : 1; \
    bool m_allowDisplayOfInsecureContent : 1; \
    bool m_allowFileAccessFromFileURLs : 1; \
    bool m_allowRunningOfInsecureContent : 1; \
    bool m_allowScriptsToCloseWindows : 1; \
    bool m_allowUniversalAccessFromFileURLs : 1; \
    bool m_antialiased2dCanvasEnabled : 1; \
    bool m_antialiasedClips2dCanvasEnabled : 1; \
    bool m_asynchronousSpellCheckingEnabled : 1; \
    bool m_caretBrowsingEnabled : 1; \
    bool m_cookieEnabled : 1; \
    bool m_deviceSupportsMouse : 1; \
    bool m_deviceSupportsTouch : 1; \
    bool m_disableReadingFromCanvas : 1; \
    bool m_dnsPrefetchingEnabled : 1; \
    bool m_doHtmlPreloadScanning : 1; \
    bool m_downloadableBinaryFontsEnabled : 1; \
    bool m_forceZeroLayoutHeight : 1; \
    bool m_fullscreenSupported : 1; \
    bool m_hyperlinkAuditingEnabled : 1; \
    bool m_ignoreMainFrameOverflowHiddenQuirk : 1; \
    bool m_imagesEnabled : 1; \
    bool m_inlineTextBoxAccessibilityEnabled : 1; \
    bool m_invertViewportScrollOrder : 1; \
    bool m_javaEnabled : 1; \
    bool m_javaScriptCanAccessClipboard : 1; \
    bool m_javaScriptCanOpenWindowsAutomatically : 1; \
    bool m_layerSquashingEnabled : 1; \
    bool m_loadWithOverviewMode : 1; \
    bool m_loadsImagesAutomatically : 1; \
    bool m_localStorageEnabled : 1; \
    bool m_logDnsPrefetchAndPreconnect : 1; \
    bool m_logPreload : 1; \
    bool m_lowPriorityIframes : 1; \
    bool m_mainFrameClipsContent : 1; \
    bool m_mediaControlsOverlayPlayButtonEnabled : 1; \
    bool m_mediaPlaybackRequiresUserGesture : 1; \
    bool m_mockGestureTapHighlightsEnabled : 1; \
    bool m_multiTargetTapNotificationEnabled : 1; \
    bool m_navigateOnDragDrop : 1; \
    bool m_offlineWebApplicationCacheEnabled : 1; \
    bool m_passwordEchoEnabled : 1; \
    bool m_pluginsEnabled : 1; \
    bool m_preferCompositingToLCDTextEnabled : 1; \
    bool m_reportScreenSizeInPhysicalPixelsQuirk : 1; \
    bool m_reportWheelOverscroll : 1; \
    bool m_rootLayerScrolls : 1; \
    bool m_rubberBandingOnCompositorThread : 1; \
    bool m_scriptEnabled : 1; \
    bool m_scrollAnimatorEnabled : 1; \
    bool m_selectTrailingWhitespaceEnabled : 1; \
    bool m_selectionIncludesAltImageText : 1; \
    bool m_shouldClearDocumentBackground : 1; \
    bool m_shouldPrintBackgrounds : 1; \
    bool m_shouldRespectImageOrientation : 1; \
    bool m_showContextMenuOnMouseUp : 1; \
    bool m_smartInsertDeleteEnabled : 1; \
    bool m_spatialNavigationEnabled : 1; \
    bool m_strictMixedContentChecking : 1; \
    bool m_strictPowerfulFeatureRestrictions : 1; \
    bool m_supportsMultipleWindows : 1; \
    bool m_syncXHRInDocumentsEnabled : 1; \
    bool m_textAreasAreResizable : 1; \
    bool m_threadedScrollingEnabled : 1; \
    bool m_touchAdjustmentEnabled : 1; \
    bool m_touchDragDropEnabled : 1; \
    bool m_touchEditingEnabled : 1; \
    bool m_unifiedTextCheckerEnabled : 1; \
    bool m_unsafePluginPastingEnabled : 1; \
    bool m_useLegacyBackgroundSizeShorthandBehavior : 1; \
    bool m_useMobileViewportStyle : 1; \
    bool m_useSolidColorScrollbars : 1; \
    bool m_useWideViewport : 1; \
    bool m_usesEncodingDetector : 1; \
    bool m_viewportEnabled : 1; \
    bool m_viewportMetaEnabled : 1; \
    bool m_viewportMetaMergeContentQuirk : 1; \
    bool m_viewportMetaZeroValuesQuirk : 1; \
    bool m_webAudioEnabled : 1; \
    bool m_webGLEnabled : 1; \
    bool m_webGLErrorsToConsoleEnabled : 1; \
    bool m_webSecurityEnabled : 1; \
    bool m_wideViewportQuirkEnabled : 1; \
    bool m_xssAuditorEnabled : 1; \
    String m_language; \
    String m_vendor; \
    String m_productSub; \
// End of SETTINGS_MEMBER_VARIABLES.

#define SETTINGS_INITIALIZER_LIST \
    , m_accelerated2dCanvasMSAASampleCount(0) \
    , m_accessibilityFontScaleFactor(1.0) \
    , m_availableHoverTypes(HoverTypeNone) \
    , m_availablePointerTypes(PointerTypeNone) \
    , m_backgroundHtmlParserOutstandingTokenLimit(0) \
    , m_backgroundHtmlParserPendingTokenLimit(0) \
    , m_defaultFixedFontSize(0) \
    , m_defaultFontSize(0) \
    , m_deviceScaleAdjustment(1.0) \
    , m_displayModeOverride(WebDisplayModeUndefined) \
    , m_editingBehaviorType(editingBehaviorTypeForPlatform()) \
    , m_imageAnimationPolicy(ImageAnimationPolicyAllowed) \
    , m_maxBeaconTransmission(65536) \
    , m_maxTouchPoints(0) \
    , m_mediaTypeOverride("") \
    , m_minimumAccelerated2dCanvasSize(257*256) \
    , m_minimumFontSize(0) \
    , m_minimumLogicalFontSize(0) \
    , m_passwordEchoDurationInSeconds(1) \
    , m_pinchOverlayScrollbarThickness(0) \
    , m_primaryHoverType(HoverTypeNone) \
    , m_primaryPointerType(PointerTypeNone) \
    , m_selectionStrategy(SelectionStrategy::Character) \
    , m_textTrackKindUserPreference(TextTrackKindUserPreference::Default) \
    , m_v8CacheOptions(V8CacheOptionsDefault) \
    , m_validationMessageTimerMagnification(50) \
    , m_DOMPasteAllowed(false) \
    , m_accelerated2dCanvasEnabled(false) \
    , m_acceleratedCompositingEnabled(true) \
    , m_accessibilityEnabled(false) \
    , m_accessibilityPasswordValuesEnabled(false) \
    , m_allowCustomScrollbarInMainFrame(true) \
    , m_allowDisplayOfInsecureContent(true) \
    , m_allowFileAccessFromFileURLs(true) \
    , m_allowRunningOfInsecureContent(true) \
    , m_allowScriptsToCloseWindows(false) \
    , m_allowUniversalAccessFromFileURLs(true) \
    , m_antialiased2dCanvasEnabled(true) \
    , m_antialiasedClips2dCanvasEnabled(false) \
    , m_asynchronousSpellCheckingEnabled(false) \
    , m_caretBrowsingEnabled(false) \
    , m_cookieEnabled(true) \
    , m_deviceSupportsMouse(true) \
    , m_deviceSupportsTouch(false) \
    , m_disableReadingFromCanvas(false) \
    , m_dnsPrefetchingEnabled(false) \
    , m_doHtmlPreloadScanning(true) \
    , m_downloadableBinaryFontsEnabled(true) \
    , m_forceZeroLayoutHeight(false) \
    , m_fullscreenSupported(true) \
    , m_hyperlinkAuditingEnabled(false) \
    , m_ignoreMainFrameOverflowHiddenQuirk(false) \
    , m_imagesEnabled(true) \
    , m_inlineTextBoxAccessibilityEnabled(false) \
    , m_invertViewportScrollOrder(false) \
    , m_javaEnabled(false) \
    , m_javaScriptCanAccessClipboard(false) \
    , m_javaScriptCanOpenWindowsAutomatically(false) \
    , m_layerSquashingEnabled(false) \
    , m_loadWithOverviewMode(true) \
    , m_loadsImagesAutomatically(false) \
    , m_localStorageEnabled(false) \
    , m_logDnsPrefetchAndPreconnect(false) \
    , m_logPreload(false) \
    , m_lowPriorityIframes(false) \
    , m_mainFrameClipsContent(true) \
    , m_mediaControlsOverlayPlayButtonEnabled(false) \
    , m_mediaPlaybackRequiresUserGesture(false) \
    , m_mockGestureTapHighlightsEnabled(false) \
    , m_multiTargetTapNotificationEnabled(true) \
    , m_navigateOnDragDrop(true) \
    , m_offlineWebApplicationCacheEnabled(true) \
    , m_passwordEchoEnabled(false) \
    , m_pluginsEnabled(false) \
    , m_preferCompositingToLCDTextEnabled(false) \
    , m_reportScreenSizeInPhysicalPixelsQuirk(false) \
    , m_reportWheelOverscroll(false) \
    , m_rootLayerScrolls(false) \
    , m_rubberBandingOnCompositorThread(false) \
    , m_scriptEnabled(false) \
    , m_scrollAnimatorEnabled(true) \
    , m_selectTrailingWhitespaceEnabled(defaultSelectTrailingWhitespaceEnabled) \
    , m_selectionIncludesAltImageText(false) \
    , m_shouldClearDocumentBackground(true) \
    , m_shouldPrintBackgrounds(false) \
    , m_shouldRespectImageOrientation(false) \
    , m_showContextMenuOnMouseUp(false) \
    , m_smartInsertDeleteEnabled(defaultSmartInsertDeleteEnabled) \
    , m_spatialNavigationEnabled(false) \
    , m_strictMixedContentChecking(false) \
    , m_strictPowerfulFeatureRestrictions(false) \
    , m_supportsMultipleWindows(true) \
    , m_syncXHRInDocumentsEnabled(true) \
    , m_textAreasAreResizable(false) \
    , m_threadedScrollingEnabled(true) \
    , m_touchAdjustmentEnabled(true) \
    , m_touchDragDropEnabled(false) \
    , m_touchEditingEnabled(false) \
    , m_unifiedTextCheckerEnabled(defaultUnifiedTextCheckerEnabled) \
    , m_unsafePluginPastingEnabled(false) \
    , m_useLegacyBackgroundSizeShorthandBehavior(false) \
    , m_useMobileViewportStyle(false) \
    , m_useSolidColorScrollbars(false) \
    , m_useWideViewport(true) \
    , m_usesEncodingDetector(false) \
    , m_viewportEnabled(false) \
    , m_viewportMetaEnabled(false) \
    , m_viewportMetaMergeContentQuirk(false) \
    , m_viewportMetaZeroValuesQuirk(false) \
    , m_webAudioEnabled(false) \
    , m_webGLEnabled(false) \
    , m_webGLErrorsToConsoleEnabled(true) \
    , m_webSecurityEnabled(true) \
    , m_wideViewportQuirkEnabled(false) \
    , m_xssAuditorEnabled(false) \
    , m_language("zh-cn") \
    , m_vendor("Google Inc.") \
    , m_productSub("20030107")
// End of SETTINGS_INITIALIZER_LIST.

#define SETTINGS_SETTER_BODIES \
void Settings::setDOMPasteAllowed(bool DOMPasteAllowed) \
{ \
    if (m_DOMPasteAllowed == DOMPasteAllowed) \
        return; \
    m_DOMPasteAllowed = DOMPasteAllowed; \
} \
void Settings::setAccelerated2dCanvasEnabled(bool accelerated2dCanvasEnabled) \
{ \
    if (m_accelerated2dCanvasEnabled == accelerated2dCanvasEnabled) \
        return; \
    m_accelerated2dCanvasEnabled = accelerated2dCanvasEnabled; \
} \
void Settings::setAccelerated2dCanvasMSAASampleCount(int accelerated2dCanvasMSAASampleCount) \
{ \
    if (m_accelerated2dCanvasMSAASampleCount == accelerated2dCanvasMSAASampleCount) \
        return; \
    m_accelerated2dCanvasMSAASampleCount = accelerated2dCanvasMSAASampleCount; \
} \
void Settings::setAcceleratedCompositingEnabled(bool acceleratedCompositingEnabled) \
{ \
    if (m_acceleratedCompositingEnabled == acceleratedCompositingEnabled) \
        return; \
    m_acceleratedCompositingEnabled = acceleratedCompositingEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setAccessibilityEnabled(bool accessibilityEnabled) \
{ \
    if (m_accessibilityEnabled == accessibilityEnabled) \
        return; \
    m_accessibilityEnabled = accessibilityEnabled; \
    invalidate(SettingsDelegate::AccessibilityStateChange); \
} \
void Settings::setAccessibilityFontScaleFactor(double accessibilityFontScaleFactor) \
{ \
    if (m_accessibilityFontScaleFactor == accessibilityFontScaleFactor) \
        return; \
    m_accessibilityFontScaleFactor = accessibilityFontScaleFactor; \
    invalidate(SettingsDelegate::TextAutosizingChange); \
} \
void Settings::setAccessibilityPasswordValuesEnabled(bool accessibilityPasswordValuesEnabled) \
{ \
    if (m_accessibilityPasswordValuesEnabled == accessibilityPasswordValuesEnabled) \
        return; \
    m_accessibilityPasswordValuesEnabled = accessibilityPasswordValuesEnabled; \
} \
void Settings::setAllowCustomScrollbarInMainFrame(bool allowCustomScrollbarInMainFrame) \
{ \
    if (m_allowCustomScrollbarInMainFrame == allowCustomScrollbarInMainFrame) \
        return; \
    m_allowCustomScrollbarInMainFrame = allowCustomScrollbarInMainFrame; \
} \
void Settings::setAllowDisplayOfInsecureContent(bool allowDisplayOfInsecureContent) \
{ \
    if (m_allowDisplayOfInsecureContent == allowDisplayOfInsecureContent) \
        return; \
    m_allowDisplayOfInsecureContent = allowDisplayOfInsecureContent; \
} \
void Settings::setAllowFileAccessFromFileURLs(bool allowFileAccessFromFileURLs) \
{ \
    if (m_allowFileAccessFromFileURLs == allowFileAccessFromFileURLs) \
        return; \
    m_allowFileAccessFromFileURLs = allowFileAccessFromFileURLs; \
} \
void Settings::setAllowRunningOfInsecureContent(bool allowRunningOfInsecureContent) \
{ \
    if (m_allowRunningOfInsecureContent == allowRunningOfInsecureContent) \
        return; \
    m_allowRunningOfInsecureContent = allowRunningOfInsecureContent; \
} \
void Settings::setAllowScriptsToCloseWindows(bool allowScriptsToCloseWindows) \
{ \
    if (m_allowScriptsToCloseWindows == allowScriptsToCloseWindows) \
        return; \
    m_allowScriptsToCloseWindows = allowScriptsToCloseWindows; \
} \
void Settings::setAllowUniversalAccessFromFileURLs(bool allowUniversalAccessFromFileURLs) \
{ \
    if (m_allowUniversalAccessFromFileURLs == allowUniversalAccessFromFileURLs) \
        return; \
    m_allowUniversalAccessFromFileURLs = allowUniversalAccessFromFileURLs; \
} \
void Settings::setAntialiased2dCanvasEnabled(bool antialiased2dCanvasEnabled) \
{ \
    if (m_antialiased2dCanvasEnabled == antialiased2dCanvasEnabled) \
        return; \
    m_antialiased2dCanvasEnabled = antialiased2dCanvasEnabled; \
} \
void Settings::setAntialiasedClips2dCanvasEnabled(bool antialiasedClips2dCanvasEnabled) \
{ \
    if (m_antialiasedClips2dCanvasEnabled == antialiasedClips2dCanvasEnabled) \
        return; \
    m_antialiasedClips2dCanvasEnabled = antialiasedClips2dCanvasEnabled; \
} \
void Settings::setAsynchronousSpellCheckingEnabled(bool asynchronousSpellCheckingEnabled) \
{ \
    if (m_asynchronousSpellCheckingEnabled == asynchronousSpellCheckingEnabled) \
        return; \
    m_asynchronousSpellCheckingEnabled = asynchronousSpellCheckingEnabled; \
} \
void Settings::setAvailableHoverTypes(int availableHoverTypes) \
{ \
    if (m_availableHoverTypes == availableHoverTypes) \
        return; \
    m_availableHoverTypes = availableHoverTypes; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setAvailablePointerTypes(int availablePointerTypes) \
{ \
    if (m_availablePointerTypes == availablePointerTypes) \
        return; \
    m_availablePointerTypes = availablePointerTypes; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setBackgroundHtmlParserOutstandingTokenLimit(unsigned backgroundHtmlParserOutstandingTokenLimit) \
{ \
    if (m_backgroundHtmlParserOutstandingTokenLimit == backgroundHtmlParserOutstandingTokenLimit) \
        return; \
    m_backgroundHtmlParserOutstandingTokenLimit = backgroundHtmlParserOutstandingTokenLimit; \
} \
void Settings::setBackgroundHtmlParserPendingTokenLimit(unsigned backgroundHtmlParserPendingTokenLimit) \
{ \
    if (m_backgroundHtmlParserPendingTokenLimit == backgroundHtmlParserPendingTokenLimit) \
        return; \
    m_backgroundHtmlParserPendingTokenLimit = backgroundHtmlParserPendingTokenLimit; \
} \
void Settings::setCaretBrowsingEnabled(bool caretBrowsingEnabled) \
{ \
    if (m_caretBrowsingEnabled == caretBrowsingEnabled) \
        return; \
    m_caretBrowsingEnabled = caretBrowsingEnabled; \
} \
void Settings::setCookieEnabled(bool cookieEnabled) \
{ \
    if (m_cookieEnabled == cookieEnabled) \
        return; \
    m_cookieEnabled = cookieEnabled; \
} \
void Settings::setDefaultFixedFontSize(int defaultFixedFontSize) \
{ \
    if (m_defaultFixedFontSize == defaultFixedFontSize) \
        return; \
    m_defaultFixedFontSize = defaultFixedFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setDefaultFontSize(int defaultFontSize) \
{ \
    if (m_defaultFontSize == defaultFontSize) \
        return; \
    m_defaultFontSize = defaultFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setDefaultTextEncodingName(const String& defaultTextEncodingName) \
{ \
    if (m_defaultTextEncodingName == defaultTextEncodingName) \
        return; \
    m_defaultTextEncodingName = defaultTextEncodingName; \
} \
void Settings::setDefaultVideoPosterURL(const String& defaultVideoPosterURL) \
{ \
    if (m_defaultVideoPosterURL == defaultVideoPosterURL) \
        return; \
    m_defaultVideoPosterURL = defaultVideoPosterURL; \
} \
void Settings::setDeviceScaleAdjustment(double deviceScaleAdjustment) \
{ \
    if (m_deviceScaleAdjustment == deviceScaleAdjustment) \
        return; \
    m_deviceScaleAdjustment = deviceScaleAdjustment; \
    invalidate(SettingsDelegate::TextAutosizingChange); \
} \
void Settings::setDeviceSupportsMouse(bool deviceSupportsMouse) \
{ \
    if (m_deviceSupportsMouse == deviceSupportsMouse) \
        return; \
    m_deviceSupportsMouse = deviceSupportsMouse; \
} \
void Settings::setDeviceSupportsTouch(bool deviceSupportsTouch) \
{ \
    if (m_deviceSupportsTouch == deviceSupportsTouch) \
        return; \
    m_deviceSupportsTouch = deviceSupportsTouch; \
} \
void Settings::setDisableReadingFromCanvas(bool disableReadingFromCanvas) \
{ \
    if (m_disableReadingFromCanvas == disableReadingFromCanvas) \
        return; \
    m_disableReadingFromCanvas = disableReadingFromCanvas; \
} \
void Settings::setDisplayModeOverride(WebDisplayMode displayModeOverride) \
{ \
    if (m_displayModeOverride == displayModeOverride) \
        return; \
    m_displayModeOverride = displayModeOverride; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setDNSPrefetchingEnabled(bool dnsPrefetchingEnabled) \
{ \
    if (m_dnsPrefetchingEnabled == dnsPrefetchingEnabled) \
        return; \
    m_dnsPrefetchingEnabled = dnsPrefetchingEnabled; \
    invalidate(SettingsDelegate::DNSPrefetchingChange); \
} \
void Settings::setDoHtmlPreloadScanning(bool doHtmlPreloadScanning) \
{ \
    if (m_doHtmlPreloadScanning == doHtmlPreloadScanning) \
        return; \
    m_doHtmlPreloadScanning = doHtmlPreloadScanning; \
} \
void Settings::setDownloadableBinaryFontsEnabled(bool downloadableBinaryFontsEnabled) \
{ \
    if (m_downloadableBinaryFontsEnabled == downloadableBinaryFontsEnabled) \
        return; \
    m_downloadableBinaryFontsEnabled = downloadableBinaryFontsEnabled; \
} \
void Settings::setEditingBehaviorType(EditingBehaviorType editingBehaviorType) \
{ \
    if (m_editingBehaviorType == editingBehaviorType) \
        return; \
    m_editingBehaviorType = editingBehaviorType; \
} \
void Settings::setForceZeroLayoutHeight(bool forceZeroLayoutHeight) \
{ \
    if (m_forceZeroLayoutHeight == forceZeroLayoutHeight) \
        return; \
    m_forceZeroLayoutHeight = forceZeroLayoutHeight; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setFullscreenSupported(bool fullscreenSupported) \
{ \
    if (m_fullscreenSupported == fullscreenSupported) \
        return; \
    m_fullscreenSupported = fullscreenSupported; \
} \
void Settings::setHyperlinkAuditingEnabled(bool hyperlinkAuditingEnabled) \
{ \
    if (m_hyperlinkAuditingEnabled == hyperlinkAuditingEnabled) \
        return; \
    m_hyperlinkAuditingEnabled = hyperlinkAuditingEnabled; \
} \
void Settings::setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk) \
{ \
    if (m_ignoreMainFrameOverflowHiddenQuirk == ignoreMainFrameOverflowHiddenQuirk) \
        return; \
    m_ignoreMainFrameOverflowHiddenQuirk = ignoreMainFrameOverflowHiddenQuirk; \
} \
void Settings::setImageAnimationPolicy(ImageAnimationPolicy imageAnimationPolicy) \
{ \
    if (m_imageAnimationPolicy == imageAnimationPolicy) \
        return; \
    m_imageAnimationPolicy = imageAnimationPolicy; \
} \
void Settings::setImagesEnabled(bool imagesEnabled) \
{ \
    if (m_imagesEnabled == imagesEnabled) \
        return; \
    m_imagesEnabled = imagesEnabled; \
    invalidate(SettingsDelegate::ImageLoadingChange); \
} \
void Settings::setInlineTextBoxAccessibilityEnabled(bool inlineTextBoxAccessibilityEnabled) \
{ \
    if (m_inlineTextBoxAccessibilityEnabled == inlineTextBoxAccessibilityEnabled) \
        return; \
    m_inlineTextBoxAccessibilityEnabled = inlineTextBoxAccessibilityEnabled; \
} \
void Settings::setInvertViewportScrollOrder(bool invertViewportScrollOrder) \
{ \
    if (m_invertViewportScrollOrder == invertViewportScrollOrder) \
        return; \
    m_invertViewportScrollOrder = invertViewportScrollOrder; \
} \
void Settings::setJavaEnabled(bool javaEnabled) \
{ \
    if (m_javaEnabled == javaEnabled) \
        return; \
    m_javaEnabled = javaEnabled; \
} \
void Settings::setJavaScriptCanAccessClipboard(bool javaScriptCanAccessClipboard) \
{ \
    if (m_javaScriptCanAccessClipboard == javaScriptCanAccessClipboard) \
        return; \
    m_javaScriptCanAccessClipboard = javaScriptCanAccessClipboard; \
} \
void Settings::setJavaScriptCanOpenWindowsAutomatically(bool javaScriptCanOpenWindowsAutomatically) \
{ \
    if (m_javaScriptCanOpenWindowsAutomatically == javaScriptCanOpenWindowsAutomatically) \
        return; \
    m_javaScriptCanOpenWindowsAutomatically = javaScriptCanOpenWindowsAutomatically; \
} \
void Settings::setLayerSquashingEnabled(bool layerSquashingEnabled) \
{ \
    if (m_layerSquashingEnabled == layerSquashingEnabled) \
        return; \
    m_layerSquashingEnabled = layerSquashingEnabled; \
} \
void Settings::setLoadWithOverviewMode(bool loadWithOverviewMode) \
{ \
    if (m_loadWithOverviewMode == loadWithOverviewMode) \
        return; \
    m_loadWithOverviewMode = loadWithOverviewMode; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setLoadsImagesAutomatically(bool loadsImagesAutomatically) \
{ \
    if (m_loadsImagesAutomatically == loadsImagesAutomatically) \
        return; \
    m_loadsImagesAutomatically = loadsImagesAutomatically; \
    invalidate(SettingsDelegate::ImageLoadingChange); \
} \
void Settings::setLocalStorageEnabled(bool localStorageEnabled) \
{ \
    if (m_localStorageEnabled == localStorageEnabled) \
        return; \
    m_localStorageEnabled = localStorageEnabled; \
} \
void Settings::setLogDnsPrefetchAndPreconnect(bool logDnsPrefetchAndPreconnect) \
{ \
    if (m_logDnsPrefetchAndPreconnect == logDnsPrefetchAndPreconnect) \
        return; \
    m_logDnsPrefetchAndPreconnect = logDnsPrefetchAndPreconnect; \
} \
void Settings::setLogPreload(bool logPreload) \
{ \
    if (m_logPreload == logPreload) \
        return; \
    m_logPreload = logPreload; \
} \
void Settings::setLowPriorityIframes(bool lowPriorityIframes) \
{ \
    if (m_lowPriorityIframes == lowPriorityIframes) \
        return; \
    m_lowPriorityIframes = lowPriorityIframes; \
} \
void Settings::setMainFrameClipsContent(bool mainFrameClipsContent) \
{ \
    if (m_mainFrameClipsContent == mainFrameClipsContent) \
        return; \
    m_mainFrameClipsContent = mainFrameClipsContent; \
} \
void Settings::setMaxBeaconTransmission(int maxBeaconTransmission) \
{ \
    if (m_maxBeaconTransmission == maxBeaconTransmission) \
        return; \
    m_maxBeaconTransmission = maxBeaconTransmission; \
} \
void Settings::setMaxTouchPoints(int maxTouchPoints) \
{ \
    if (m_maxTouchPoints == maxTouchPoints) \
        return; \
    m_maxTouchPoints = maxTouchPoints; \
} \
void Settings::setMediaControlsOverlayPlayButtonEnabled(bool mediaControlsOverlayPlayButtonEnabled) \
{ \
    if (m_mediaControlsOverlayPlayButtonEnabled == mediaControlsOverlayPlayButtonEnabled) \
        return; \
    m_mediaControlsOverlayPlayButtonEnabled = mediaControlsOverlayPlayButtonEnabled; \
} \
void Settings::setMediaPlaybackRequiresUserGesture(bool mediaPlaybackRequiresUserGesture) \
{ \
    if (m_mediaPlaybackRequiresUserGesture == mediaPlaybackRequiresUserGesture) \
        return; \
    m_mediaPlaybackRequiresUserGesture = mediaPlaybackRequiresUserGesture; \
} \
void Settings::setMediaTypeOverride(const String& mediaTypeOverride) \
{ \
    if (m_mediaTypeOverride == mediaTypeOverride) \
        return; \
    m_mediaTypeOverride = mediaTypeOverride; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setMinimumAccelerated2dCanvasSize(int minimumAccelerated2dCanvasSize) \
{ \
    if (m_minimumAccelerated2dCanvasSize == minimumAccelerated2dCanvasSize) \
        return; \
    m_minimumAccelerated2dCanvasSize = minimumAccelerated2dCanvasSize; \
} \
void Settings::setMinimumFontSize(int minimumFontSize) \
{ \
    if (m_minimumFontSize == minimumFontSize) \
        return; \
    m_minimumFontSize = minimumFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setMinimumLogicalFontSize(int minimumLogicalFontSize) \
{ \
    if (m_minimumLogicalFontSize == minimumLogicalFontSize) \
        return; \
    m_minimumLogicalFontSize = minimumLogicalFontSize; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setMockGestureTapHighlightsEnabled(bool mockGestureTapHighlightsEnabled) \
{ \
    if (m_mockGestureTapHighlightsEnabled == mockGestureTapHighlightsEnabled) \
        return; \
    m_mockGestureTapHighlightsEnabled = mockGestureTapHighlightsEnabled; \
} \
void Settings::setMultiTargetTapNotificationEnabled(bool multiTargetTapNotificationEnabled) \
{ \
    if (m_multiTargetTapNotificationEnabled == multiTargetTapNotificationEnabled) \
        return; \
    m_multiTargetTapNotificationEnabled = multiTargetTapNotificationEnabled; \
} \
void Settings::setNavigateOnDragDrop(bool navigateOnDragDrop) \
{ \
    if (m_navigateOnDragDrop == navigateOnDragDrop) \
        return; \
    m_navigateOnDragDrop = navigateOnDragDrop; \
} \
void Settings::setOfflineWebApplicationCacheEnabled(bool offlineWebApplicationCacheEnabled) \
{ \
    if (m_offlineWebApplicationCacheEnabled == offlineWebApplicationCacheEnabled) \
        return; \
    m_offlineWebApplicationCacheEnabled = offlineWebApplicationCacheEnabled; \
} \
void Settings::setPasswordEchoDurationInSeconds(double passwordEchoDurationInSeconds) \
{ \
    if (m_passwordEchoDurationInSeconds == passwordEchoDurationInSeconds) \
        return; \
    m_passwordEchoDurationInSeconds = passwordEchoDurationInSeconds; \
} \
void Settings::setPasswordEchoEnabled(bool passwordEchoEnabled) \
{ \
    if (m_passwordEchoEnabled == passwordEchoEnabled) \
        return; \
    m_passwordEchoEnabled = passwordEchoEnabled; \
} \
void Settings::setPinchOverlayScrollbarThickness(int pinchOverlayScrollbarThickness) \
{ \
    if (m_pinchOverlayScrollbarThickness == pinchOverlayScrollbarThickness) \
        return; \
    m_pinchOverlayScrollbarThickness = pinchOverlayScrollbarThickness; \
} \
void Settings::setPluginsEnabled(bool pluginsEnabled) \
{ \
    if (m_pluginsEnabled == pluginsEnabled) \
        return; \
    m_pluginsEnabled = pluginsEnabled; \
} \
void Settings::setPreferCompositingToLCDTextEnabled(bool preferCompositingToLCDTextEnabled) \
{ \
    if (m_preferCompositingToLCDTextEnabled == preferCompositingToLCDTextEnabled) \
        return; \
    m_preferCompositingToLCDTextEnabled = preferCompositingToLCDTextEnabled; \
    invalidate(SettingsDelegate::AcceleratedCompositingChange); \
} \
void Settings::setPrimaryHoverType(HoverType primaryHoverType) \
{ \
    if (m_primaryHoverType == primaryHoverType) \
        return; \
    m_primaryHoverType = primaryHoverType; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setPrimaryPointerType(PointerType primaryPointerType) \
{ \
    if (m_primaryPointerType == primaryPointerType) \
        return; \
    m_primaryPointerType = primaryPointerType; \
    invalidate(SettingsDelegate::MediaQueryChange); \
} \
void Settings::setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk) \
{ \
    if (m_reportScreenSizeInPhysicalPixelsQuirk == reportScreenSizeInPhysicalPixelsQuirk) \
        return; \
    m_reportScreenSizeInPhysicalPixelsQuirk = reportScreenSizeInPhysicalPixelsQuirk; \
} \
void Settings::setReportWheelOverscroll(bool reportWheelOverscroll) \
{ \
    if (m_reportWheelOverscroll == reportWheelOverscroll) \
        return; \
    m_reportWheelOverscroll = reportWheelOverscroll; \
} \
void Settings::setRootLayerScrolls(bool rootLayerScrolls) \
{ \
    if (m_rootLayerScrolls == rootLayerScrolls) \
        return; \
    m_rootLayerScrolls = rootLayerScrolls; \
} \
void Settings::setRubberBandingOnCompositorThread(bool rubberBandingOnCompositorThread) \
{ \
    if (m_rubberBandingOnCompositorThread == rubberBandingOnCompositorThread) \
        return; \
    m_rubberBandingOnCompositorThread = rubberBandingOnCompositorThread; \
} \
void Settings::setScriptEnabled(bool scriptEnabled) \
{ \
    if (m_scriptEnabled == scriptEnabled) \
        return; \
    m_scriptEnabled = scriptEnabled; \
} \
void Settings::setScrollAnimatorEnabled(bool scrollAnimatorEnabled) \
{ \
    if (m_scrollAnimatorEnabled == scrollAnimatorEnabled) \
        return; \
    m_scrollAnimatorEnabled = scrollAnimatorEnabled; \
} \
void Settings::setSelectTrailingWhitespaceEnabled(bool selectTrailingWhitespaceEnabled) \
{ \
    if (m_selectTrailingWhitespaceEnabled == selectTrailingWhitespaceEnabled) \
        return; \
    m_selectTrailingWhitespaceEnabled = selectTrailingWhitespaceEnabled; \
} \
void Settings::setSelectionIncludesAltImageText(bool selectionIncludesAltImageText) \
{ \
    if (m_selectionIncludesAltImageText == selectionIncludesAltImageText) \
        return; \
    m_selectionIncludesAltImageText = selectionIncludesAltImageText; \
} \
void Settings::setSelectionStrategy(SelectionStrategy selectionStrategy) \
{ \
    if (m_selectionStrategy == selectionStrategy) \
        return; \
    m_selectionStrategy = selectionStrategy; \
} \
void Settings::setShouldClearDocumentBackground(bool shouldClearDocumentBackground) \
{ \
    if (m_shouldClearDocumentBackground == shouldClearDocumentBackground) \
        return; \
    m_shouldClearDocumentBackground = shouldClearDocumentBackground; \
} \
void Settings::setShouldPrintBackgrounds(bool shouldPrintBackgrounds) \
{ \
    if (m_shouldPrintBackgrounds == shouldPrintBackgrounds) \
        return; \
    m_shouldPrintBackgrounds = shouldPrintBackgrounds; \
} \
void Settings::setShouldRespectImageOrientation(bool shouldRespectImageOrientation) \
{ \
    if (m_shouldRespectImageOrientation == shouldRespectImageOrientation) \
        return; \
    m_shouldRespectImageOrientation = shouldRespectImageOrientation; \
} \
void Settings::setShowContextMenuOnMouseUp(bool showContextMenuOnMouseUp) \
{ \
    if (m_showContextMenuOnMouseUp == showContextMenuOnMouseUp) \
        return; \
    m_showContextMenuOnMouseUp = showContextMenuOnMouseUp; \
} \
void Settings::setSmartInsertDeleteEnabled(bool smartInsertDeleteEnabled) \
{ \
    if (m_smartInsertDeleteEnabled == smartInsertDeleteEnabled) \
        return; \
    m_smartInsertDeleteEnabled = smartInsertDeleteEnabled; \
} \
void Settings::setSpatialNavigationEnabled(bool spatialNavigationEnabled) \
{ \
    if (m_spatialNavigationEnabled == spatialNavigationEnabled) \
        return; \
    m_spatialNavigationEnabled = spatialNavigationEnabled; \
} \
void Settings::setStrictMixedContentChecking(bool strictMixedContentChecking) \
{ \
    if (m_strictMixedContentChecking == strictMixedContentChecking) \
        return; \
    m_strictMixedContentChecking = strictMixedContentChecking; \
} \
void Settings::setStrictPowerfulFeatureRestrictions(bool strictPowerfulFeatureRestrictions) \
{ \
    if (m_strictPowerfulFeatureRestrictions == strictPowerfulFeatureRestrictions) \
        return; \
    m_strictPowerfulFeatureRestrictions = strictPowerfulFeatureRestrictions; \
} \
void Settings::setSupportsMultipleWindows(bool supportsMultipleWindows) \
{ \
    if (m_supportsMultipleWindows == supportsMultipleWindows) \
        return; \
    m_supportsMultipleWindows = supportsMultipleWindows; \
} \
void Settings::setSyncXHRInDocumentsEnabled(bool syncXHRInDocumentsEnabled) \
{ \
    if (m_syncXHRInDocumentsEnabled == syncXHRInDocumentsEnabled) \
        return; \
    m_syncXHRInDocumentsEnabled = syncXHRInDocumentsEnabled; \
} \
void Settings::setTextAreasAreResizable(bool textAreasAreResizable) \
{ \
    if (m_textAreasAreResizable == textAreasAreResizable) \
        return; \
    m_textAreasAreResizable = textAreasAreResizable; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setTextTrackBackgroundColor(const String& textTrackBackgroundColor) \
{ \
    if (m_textTrackBackgroundColor == textTrackBackgroundColor) \
        return; \
    m_textTrackBackgroundColor = textTrackBackgroundColor; \
} \
void Settings::setTextTrackFontFamily(const String& textTrackFontFamily) \
{ \
    if (m_textTrackFontFamily == textTrackFontFamily) \
        return; \
    m_textTrackFontFamily = textTrackFontFamily; \
} \
void Settings::setTextTrackFontStyle(const String& textTrackFontStyle) \
{ \
    if (m_textTrackFontStyle == textTrackFontStyle) \
        return; \
    m_textTrackFontStyle = textTrackFontStyle; \
} \
void Settings::setTextTrackFontVariant(const String& textTrackFontVariant) \
{ \
    if (m_textTrackFontVariant == textTrackFontVariant) \
        return; \
    m_textTrackFontVariant = textTrackFontVariant; \
} \
void Settings::setTextTrackKindUserPreference(TextTrackKindUserPreference textTrackKindUserPreference) \
{ \
    if (m_textTrackKindUserPreference == textTrackKindUserPreference) \
        return; \
    m_textTrackKindUserPreference = textTrackKindUserPreference; \
    invalidate(SettingsDelegate::TextTrackKindUserPreferenceChange); \
} \
void Settings::setTextTrackTextColor(const String& textTrackTextColor) \
{ \
    if (m_textTrackTextColor == textTrackTextColor) \
        return; \
    m_textTrackTextColor = textTrackTextColor; \
} \
void Settings::setTextTrackTextShadow(const String& textTrackTextShadow) \
{ \
    if (m_textTrackTextShadow == textTrackTextShadow) \
        return; \
    m_textTrackTextShadow = textTrackTextShadow; \
} \
void Settings::setTextTrackTextSize(const String& textTrackTextSize) \
{ \
    if (m_textTrackTextSize == textTrackTextSize) \
        return; \
    m_textTrackTextSize = textTrackTextSize; \
} \
void Settings::setThreadedScrollingEnabled(bool threadedScrollingEnabled) \
{ \
    if (m_threadedScrollingEnabled == threadedScrollingEnabled) \
        return; \
    m_threadedScrollingEnabled = threadedScrollingEnabled; \
    invalidate(SettingsDelegate::StyleChange); \
} \
void Settings::setTouchAdjustmentEnabled(bool touchAdjustmentEnabled) \
{ \
    if (m_touchAdjustmentEnabled == touchAdjustmentEnabled) \
        return; \
    m_touchAdjustmentEnabled = touchAdjustmentEnabled; \
} \
void Settings::setTouchDragDropEnabled(bool touchDragDropEnabled) \
{ \
    if (m_touchDragDropEnabled == touchDragDropEnabled) \
        return; \
    m_touchDragDropEnabled = touchDragDropEnabled; \
} \
void Settings::setTouchEditingEnabled(bool touchEditingEnabled) \
{ \
    if (m_touchEditingEnabled == touchEditingEnabled) \
        return; \
    m_touchEditingEnabled = touchEditingEnabled; \
} \
void Settings::setUnifiedTextCheckerEnabled(bool unifiedTextCheckerEnabled) \
{ \
    if (m_unifiedTextCheckerEnabled == unifiedTextCheckerEnabled) \
        return; \
    m_unifiedTextCheckerEnabled = unifiedTextCheckerEnabled; \
} \
void Settings::setUnsafePluginPastingEnabled(bool unsafePluginPastingEnabled) \
{ \
    if (m_unsafePluginPastingEnabled == unsafePluginPastingEnabled) \
        return; \
    m_unsafePluginPastingEnabled = unsafePluginPastingEnabled; \
} \
void Settings::setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior) \
{ \
    if (m_useLegacyBackgroundSizeShorthandBehavior == useLegacyBackgroundSizeShorthandBehavior) \
        return; \
    m_useLegacyBackgroundSizeShorthandBehavior = useLegacyBackgroundSizeShorthandBehavior; \
} \
void Settings::setUseMobileViewportStyle(bool useMobileViewportStyle) \
{ \
    if (m_useMobileViewportStyle == useMobileViewportStyle) \
        return; \
    m_useMobileViewportStyle = useMobileViewportStyle; \
    invalidate(SettingsDelegate::ViewportRuleChange); \
} \
void Settings::setUseSolidColorScrollbars(bool useSolidColorScrollbars) \
{ \
    if (m_useSolidColorScrollbars == useSolidColorScrollbars) \
        return; \
    m_useSolidColorScrollbars = useSolidColorScrollbars; \
} \
void Settings::setUseWideViewport(bool useWideViewport) \
{ \
    if (m_useWideViewport == useWideViewport) \
        return; \
    m_useWideViewport = useWideViewport; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setUsesEncodingDetector(bool usesEncodingDetector) \
{ \
    if (m_usesEncodingDetector == usesEncodingDetector) \
        return; \
    m_usesEncodingDetector = usesEncodingDetector; \
} \
void Settings::setV8CacheOptions(V8CacheOptions v8CacheOptions) \
{ \
    if (m_v8CacheOptions == v8CacheOptions) \
        return; \
    m_v8CacheOptions = v8CacheOptions; \
} \
void Settings::setValidationMessageTimerMagnification(int validationMessageTimerMagnification) \
{ \
    if (m_validationMessageTimerMagnification == validationMessageTimerMagnification) \
        return; \
    m_validationMessageTimerMagnification = validationMessageTimerMagnification; \
} \
void Settings::setViewportEnabled(bool viewportEnabled) \
{ \
    if (m_viewportEnabled == viewportEnabled) \
        return; \
    m_viewportEnabled = viewportEnabled; \
    invalidate(SettingsDelegate::ViewportDescriptionChange); \
} \
void Settings::setViewportMetaEnabled(bool viewportMetaEnabled) \
{ \
    if (m_viewportMetaEnabled == viewportMetaEnabled) \
        return; \
    m_viewportMetaEnabled = viewportMetaEnabled; \
} \
void Settings::setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk) \
{ \
    if (m_viewportMetaMergeContentQuirk == viewportMetaMergeContentQuirk) \
        return; \
    m_viewportMetaMergeContentQuirk = viewportMetaMergeContentQuirk; \
} \
void Settings::setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk) \
{ \
    if (m_viewportMetaZeroValuesQuirk == viewportMetaZeroValuesQuirk) \
        return; \
    m_viewportMetaZeroValuesQuirk = viewportMetaZeroValuesQuirk; \
} \
void Settings::setWebAudioEnabled(bool webAudioEnabled) \
{ \
    if (m_webAudioEnabled == webAudioEnabled) \
        return; \
    m_webAudioEnabled = webAudioEnabled; \
} \
void Settings::setWebGLEnabled(bool webGLEnabled) \
{ \
    if (m_webGLEnabled == webGLEnabled) \
        return; \
    m_webGLEnabled = webGLEnabled; \
} \
void Settings::setWebGLErrorsToConsoleEnabled(bool webGLErrorsToConsoleEnabled) \
{ \
    if (m_webGLErrorsToConsoleEnabled == webGLErrorsToConsoleEnabled) \
        return; \
    m_webGLErrorsToConsoleEnabled = webGLErrorsToConsoleEnabled; \
} \
void Settings::setWebSecurityEnabled(bool webSecurityEnabled) \
{ \
    if (m_webSecurityEnabled == webSecurityEnabled) \
        return; \
    m_webSecurityEnabled = webSecurityEnabled; \
} \
void Settings::setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled) \
{ \
    if (m_wideViewportQuirkEnabled == wideViewportQuirkEnabled) \
        return; \
    m_wideViewportQuirkEnabled = wideViewportQuirkEnabled; \
} \
void Settings::setXSSAuditorEnabled(bool xssAuditorEnabled) \
{ \
    if (m_xssAuditorEnabled == xssAuditorEnabled) \
        return; \
    m_xssAuditorEnabled = xssAuditorEnabled; \
} \
void Settings::setFromStrings(const String& name, const String& value) \
{ \
    if (equalIgnoringCase(name, "DOMPasteAllowed")) { \
        setDOMPasteAllowed( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "accelerated2dCanvasEnabled")) { \
        setAccelerated2dCanvasEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "accelerated2dCanvasMSAASampleCount")) { \
        setAccelerated2dCanvasMSAASampleCount( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "acceleratedCompositingEnabled")) { \
        setAcceleratedCompositingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "accessibilityEnabled")) { \
        setAccessibilityEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "accessibilityFontScaleFactor")) { \
        setAccessibilityFontScaleFactor( \
            value.toDouble() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "accessibilityPasswordValuesEnabled")) { \
        setAccessibilityPasswordValuesEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowCustomScrollbarInMainFrame")) { \
        setAllowCustomScrollbarInMainFrame( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowDisplayOfInsecureContent")) { \
        setAllowDisplayOfInsecureContent( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowFileAccessFromFileURLs")) { \
        setAllowFileAccessFromFileURLs( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowRunningOfInsecureContent")) { \
        setAllowRunningOfInsecureContent( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowScriptsToCloseWindows")) { \
        setAllowScriptsToCloseWindows( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "allowUniversalAccessFromFileURLs")) { \
        setAllowUniversalAccessFromFileURLs( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "antialiased2dCanvasEnabled")) { \
        setAntialiased2dCanvasEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "antialiasedClips2dCanvasEnabled")) { \
        setAntialiasedClips2dCanvasEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "asynchronousSpellCheckingEnabled")) { \
        setAsynchronousSpellCheckingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "availableHoverTypes")) { \
        setAvailableHoverTypes( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "availablePointerTypes")) { \
        setAvailablePointerTypes( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "backgroundHtmlParserOutstandingTokenLimit")) { \
        setBackgroundHtmlParserOutstandingTokenLimit( \
            static_cast<unsigned>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "backgroundHtmlParserPendingTokenLimit")) { \
        setBackgroundHtmlParserPendingTokenLimit( \
            static_cast<unsigned>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "caretBrowsingEnabled")) { \
        setCaretBrowsingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "cookieEnabled")) { \
        setCookieEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "defaultFixedFontSize")) { \
        setDefaultFixedFontSize( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "defaultFontSize")) { \
        setDefaultFontSize( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "defaultTextEncodingName")) { \
        setDefaultTextEncodingName( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "defaultVideoPosterURL")) { \
        setDefaultVideoPosterURL( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "deviceScaleAdjustment")) { \
        setDeviceScaleAdjustment( \
            value.toDouble() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "deviceSupportsMouse")) { \
        setDeviceSupportsMouse( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "deviceSupportsTouch")) { \
        setDeviceSupportsTouch( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "disableReadingFromCanvas")) { \
        setDisableReadingFromCanvas( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "displayModeOverride")) { \
        setDisplayModeOverride( \
            static_cast<WebDisplayMode>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "dnsPrefetchingEnabled")) { \
        setDNSPrefetchingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "doHtmlPreloadScanning")) { \
        setDoHtmlPreloadScanning( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "downloadableBinaryFontsEnabled")) { \
        setDownloadableBinaryFontsEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "editingBehaviorType")) { \
        setEditingBehaviorType( \
            static_cast<EditingBehaviorType>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "forceZeroLayoutHeight")) { \
        setForceZeroLayoutHeight( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "fullscreenSupported")) { \
        setFullscreenSupported( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "hyperlinkAuditingEnabled")) { \
        setHyperlinkAuditingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "ignoreMainFrameOverflowHiddenQuirk")) { \
        setIgnoreMainFrameOverflowHiddenQuirk( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "imageAnimationPolicy")) { \
        setImageAnimationPolicy( \
            static_cast<ImageAnimationPolicy>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "imagesEnabled")) { \
        setImagesEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "inlineTextBoxAccessibilityEnabled")) { \
        setInlineTextBoxAccessibilityEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "invertViewportScrollOrder")) { \
        setInvertViewportScrollOrder( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "javaEnabled")) { \
        setJavaEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "javaScriptCanAccessClipboard")) { \
        setJavaScriptCanAccessClipboard( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "javaScriptCanOpenWindowsAutomatically")) { \
        setJavaScriptCanOpenWindowsAutomatically( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "layerSquashingEnabled")) { \
        setLayerSquashingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "loadWithOverviewMode")) { \
        setLoadWithOverviewMode( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "loadsImagesAutomatically")) { \
        setLoadsImagesAutomatically( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "localStorageEnabled")) { \
        setLocalStorageEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "logDnsPrefetchAndPreconnect")) { \
        setLogDnsPrefetchAndPreconnect( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "logPreload")) { \
        setLogPreload( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "lowPriorityIframes")) { \
        setLowPriorityIframes( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "mainFrameClipsContent")) { \
        setMainFrameClipsContent( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "maxBeaconTransmission")) { \
        setMaxBeaconTransmission( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "maxTouchPoints")) { \
        setMaxTouchPoints( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "mediaControlsOverlayPlayButtonEnabled")) { \
        setMediaControlsOverlayPlayButtonEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "mediaPlaybackRequiresUserGesture")) { \
        setMediaPlaybackRequiresUserGesture( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "mediaTypeOverride")) { \
        setMediaTypeOverride( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "minimumAccelerated2dCanvasSize")) { \
        setMinimumAccelerated2dCanvasSize( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "minimumFontSize")) { \
        setMinimumFontSize( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "minimumLogicalFontSize")) { \
        setMinimumLogicalFontSize( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "mockGestureTapHighlightsEnabled")) { \
        setMockGestureTapHighlightsEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "multiTargetTapNotificationEnabled")) { \
        setMultiTargetTapNotificationEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "navigateOnDragDrop")) { \
        setNavigateOnDragDrop( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "offlineWebApplicationCacheEnabled")) { \
        setOfflineWebApplicationCacheEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "passwordEchoDurationInSeconds")) { \
        setPasswordEchoDurationInSeconds( \
            value.toDouble() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "passwordEchoEnabled")) { \
        setPasswordEchoEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "pinchOverlayScrollbarThickness")) { \
        setPinchOverlayScrollbarThickness( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "pluginsEnabled")) { \
        setPluginsEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "preferCompositingToLCDTextEnabled")) { \
        setPreferCompositingToLCDTextEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "primaryHoverType")) { \
        setPrimaryHoverType( \
            static_cast<HoverType>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "primaryPointerType")) { \
        setPrimaryPointerType( \
            static_cast<PointerType>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "reportScreenSizeInPhysicalPixelsQuirk")) { \
        setReportScreenSizeInPhysicalPixelsQuirk( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "reportWheelOverscroll")) { \
        setReportWheelOverscroll( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "rootLayerScrolls")) { \
        setRootLayerScrolls( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "rubberBandingOnCompositorThread")) { \
        setRubberBandingOnCompositorThread( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "scriptEnabled")) { \
        setScriptEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "scrollAnimatorEnabled")) { \
        setScrollAnimatorEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "selectTrailingWhitespaceEnabled")) { \
        setSelectTrailingWhitespaceEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "selectionIncludesAltImageText")) { \
        setSelectionIncludesAltImageText( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "selectionStrategy")) { \
        setSelectionStrategy( \
            static_cast<SelectionStrategy>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "shouldClearDocumentBackground")) { \
        setShouldClearDocumentBackground( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "shouldPrintBackgrounds")) { \
        setShouldPrintBackgrounds( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "shouldRespectImageOrientation")) { \
        setShouldRespectImageOrientation( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "showContextMenuOnMouseUp")) { \
        setShowContextMenuOnMouseUp( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "smartInsertDeleteEnabled")) { \
        setSmartInsertDeleteEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "spatialNavigationEnabled")) { \
        setSpatialNavigationEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "strictMixedContentChecking")) { \
        setStrictMixedContentChecking( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "strictPowerfulFeatureRestrictions")) { \
        setStrictPowerfulFeatureRestrictions( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "supportsMultipleWindows")) { \
        setSupportsMultipleWindows( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "syncXHRInDocumentsEnabled")) { \
        setSyncXHRInDocumentsEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textAreasAreResizable")) { \
        setTextAreasAreResizable( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackBackgroundColor")) { \
        setTextTrackBackgroundColor( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackFontFamily")) { \
        setTextTrackFontFamily( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackFontStyle")) { \
        setTextTrackFontStyle( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackFontVariant")) { \
        setTextTrackFontVariant( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackKindUserPreference")) { \
        setTextTrackKindUserPreference( \
            static_cast<TextTrackKindUserPreference>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackTextColor")) { \
        setTextTrackTextColor( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackTextShadow")) { \
        setTextTrackTextShadow( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "textTrackTextSize")) { \
        setTextTrackTextSize( \
            value \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "threadedScrollingEnabled")) { \
        setThreadedScrollingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "touchAdjustmentEnabled")) { \
        setTouchAdjustmentEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "touchDragDropEnabled")) { \
        setTouchDragDropEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "touchEditingEnabled")) { \
        setTouchEditingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "unifiedTextCheckerEnabled")) { \
        setUnifiedTextCheckerEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "unsafePluginPastingEnabled")) { \
        setUnsafePluginPastingEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "useLegacyBackgroundSizeShorthandBehavior")) { \
        setUseLegacyBackgroundSizeShorthandBehavior( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "useMobileViewportStyle")) { \
        setUseMobileViewportStyle( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "useSolidColorScrollbars")) { \
        setUseSolidColorScrollbars( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "useWideViewport")) { \
        setUseWideViewport( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "usesEncodingDetector")) { \
        setUsesEncodingDetector( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "v8CacheOptions")) { \
        setV8CacheOptions( \
            static_cast<V8CacheOptions>(value.toInt()) \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "validationMessageTimerMagnification")) { \
        setValidationMessageTimerMagnification( \
            value.toInt() \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "viewportEnabled")) { \
        setViewportEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "viewportMetaEnabled")) { \
        setViewportMetaEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "viewportMetaMergeContentQuirk")) { \
        setViewportMetaMergeContentQuirk( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "viewportMetaZeroValuesQuirk")) { \
        setViewportMetaZeroValuesQuirk( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "webAudioEnabled")) { \
        setWebAudioEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "webGLEnabled")) { \
        setWebGLEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "webGLErrorsToConsoleEnabled")) { \
        setWebGLErrorsToConsoleEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "webSecurityEnabled")) { \
        setWebSecurityEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "wideViewportQuirkEnabled")) { \
        setWideViewportQuirkEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
    if (equalIgnoringCase(name, "xssAuditorEnabled")) { \
        setXSSAuditorEnabled( \
            value.isEmpty() || equalIgnoringCase(value, "true") \
        ); \
        return; \
    } \
} \
void Settings::setLanguage(const String& language) \
{ \
    if (m_language == language) \
        return; \
    m_language = language; \
} \
void Settings::setVendor(const String& vendor) \
{ \
    if (m_vendor == vendor) \
        return; \
    m_vendor = vendor; \
} \
void Settings::setproductSub(const String& productSub) \
{ \
    if (m_productSub == productSub) \
        return; \
    m_productSub = productSub; \
} \
// End of SETTINGS_SETTER_BODIES.

#endif // SettingsMacros_h
