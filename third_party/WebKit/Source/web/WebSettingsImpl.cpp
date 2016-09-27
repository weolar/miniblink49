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

#include "config.h"
#include "web/WebSettingsImpl.h"

#include "core/frame/Settings.h"
#include "platform/graphics/DeferredImageDecoder.h"

#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "web/DevToolsEmulator.h"
#include "web/WebDevToolsAgentImpl.h"

namespace blink {

WebSettingsImpl::WebSettingsImpl(Settings* settings, DevToolsEmulator* devToolsEmulator)
    : m_settings(settings)
    , m_devToolsEmulator(devToolsEmulator)
    , m_showFPSCounter(false)
    , m_showPaintRects(false)
    , m_renderVSyncNotificationEnabled(false)
    , m_autoZoomFocusedNodeToLegibleScale(false)
    , m_supportDeprecatedTargetDensityDPI(false)
    , m_shrinksViewportContentToFit(false)
    , m_viewportMetaLayoutSizeQuirk(false)
    , m_viewportMetaNonUserScalableQuirk(false)
    , m_clobberUserAgentInitialScaleQuirk(false)
    , m_mainFrameResizesAreOrientationChanges(false)
{
    ASSERT(settings);
}

void WebSettingsImpl::setFromStrings(const WebString& name, const WebString& value)
{
    m_settings->setFromStrings(name, value);
}

void WebSettingsImpl::setStandardFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateStandard(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setFixedFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateFixed(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setReportWheelOverscroll(bool enabled)
{
    m_settings->setReportWheelOverscroll(enabled);
}

void WebSettingsImpl::setForceZeroLayoutHeight(bool enabled)
{
    m_settings->setForceZeroLayoutHeight(enabled);
}

void WebSettingsImpl::setFullscreenSupported(bool enabled)
{
    m_settings->setFullscreenSupported(enabled);
}

void WebSettingsImpl::setSerifFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateSerif(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setSansSerifFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateSansSerif(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setCursiveFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateCursive(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setFantasyFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updateFantasy(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setPictographFontFamily(const WebString& font, UScriptCode script)
{
    if (m_settings->genericFontFamilySettings().updatePictograph(font, script))
        m_settings->notifyGenericFontFamilyChange();
}

void WebSettingsImpl::setDefaultFontSize(int size)
{
    m_settings->setDefaultFontSize(size);
}

void WebSettingsImpl::setDefaultFixedFontSize(int size)
{
    m_settings->setDefaultFixedFontSize(size);
}

void WebSettingsImpl::setDefaultVideoPosterURL(const WebString& url)
{
    m_settings->setDefaultVideoPosterURL(url);
}

void WebSettingsImpl::setMinimumFontSize(int size)
{
    m_settings->setMinimumFontSize(size);
}

void WebSettingsImpl::setMinimumLogicalFontSize(int size)
{
    m_settings->setMinimumLogicalFontSize(size);
}

void WebSettingsImpl::setDeviceSupportsTouch(bool deviceSupportsTouch)
{
    m_settings->setDeviceSupportsTouch(deviceSupportsTouch);
}

void WebSettingsImpl::setDeviceSupportsMouse(bool deviceSupportsMouse)
{
    m_settings->setDeviceSupportsMouse(deviceSupportsMouse);
}

void WebSettingsImpl::setAutoZoomFocusedNodeToLegibleScale(bool autoZoomFocusedNodeToLegibleScale)
{
    m_autoZoomFocusedNodeToLegibleScale = autoZoomFocusedNodeToLegibleScale;
}

void WebSettingsImpl::setTextAutosizingEnabled(bool enabled)
{
    m_devToolsEmulator->setTextAutosizingEnabled(enabled);
}

void WebSettingsImpl::setAccessibilityFontScaleFactor(float fontScaleFactor)
{
    m_settings->setAccessibilityFontScaleFactor(fontScaleFactor);
}

void WebSettingsImpl::setAccessibilityEnabled(bool enabled)
{
    m_settings->setAccessibilityEnabled(enabled);
}

void WebSettingsImpl::setAccessibilityPasswordValuesEnabled(bool enabled)
{
    m_settings->setAccessibilityPasswordValuesEnabled(enabled);
}

void WebSettingsImpl::setInlineTextBoxAccessibilityEnabled(bool enabled)
{
    m_settings->setInlineTextBoxAccessibilityEnabled(enabled);
}

void WebSettingsImpl::setInvertViewportScrollOrder(bool enabled)
{
    m_settings->setInvertViewportScrollOrder(enabled);
}

void WebSettingsImpl::setDeviceScaleAdjustment(float deviceScaleAdjustment)
{
    m_devToolsEmulator->setDeviceScaleAdjustment(deviceScaleAdjustment);
}

void WebSettingsImpl::setDefaultTextEncodingName(const WebString& encoding)
{
    m_settings->setDefaultTextEncodingName((String)encoding);
}

void WebSettingsImpl::setJavaScriptEnabled(bool enabled)
{
    m_devToolsEmulator->setScriptEnabled(enabled);
}

void WebSettingsImpl::setWebSecurityEnabled(bool enabled)
{
    m_settings->setWebSecurityEnabled(enabled);
}

void WebSettingsImpl::setJavaScriptCanOpenWindowsAutomatically(bool canOpenWindows)
{
    m_settings->setJavaScriptCanOpenWindowsAutomatically(canOpenWindows);
}

void WebSettingsImpl::setSupportDeprecatedTargetDensityDPI(bool supportDeprecatedTargetDensityDPI)
{
    m_supportDeprecatedTargetDensityDPI = supportDeprecatedTargetDensityDPI;
}

void WebSettingsImpl::setViewportMetaLayoutSizeQuirk(bool viewportMetaLayoutSizeQuirk)
{
    m_viewportMetaLayoutSizeQuirk = viewportMetaLayoutSizeQuirk;
}

void WebSettingsImpl::setViewportMetaMergeContentQuirk(bool viewportMetaMergeContentQuirk)
{
    m_settings->setViewportMetaMergeContentQuirk(viewportMetaMergeContentQuirk);
}

void WebSettingsImpl::setViewportMetaNonUserScalableQuirk(bool viewportMetaNonUserScalableQuirk)
{
    m_viewportMetaNonUserScalableQuirk = viewportMetaNonUserScalableQuirk;
}

void WebSettingsImpl::setViewportMetaZeroValuesQuirk(bool viewportMetaZeroValuesQuirk)
{
    m_settings->setViewportMetaZeroValuesQuirk(viewportMetaZeroValuesQuirk);
}

void WebSettingsImpl::setIgnoreMainFrameOverflowHiddenQuirk(bool ignoreMainFrameOverflowHiddenQuirk)
{
    m_settings->setIgnoreMainFrameOverflowHiddenQuirk(ignoreMainFrameOverflowHiddenQuirk);
}

void WebSettingsImpl::setReportScreenSizeInPhysicalPixelsQuirk(bool reportScreenSizeInPhysicalPixelsQuirk)
{
    m_settings->setReportScreenSizeInPhysicalPixelsQuirk(reportScreenSizeInPhysicalPixelsQuirk);
}

void WebSettingsImpl::setRootLayerScrolls(bool rootLayerScrolls)
{
    m_settings->setRootLayerScrolls(rootLayerScrolls);
}

void WebSettingsImpl::setRubberBandingOnCompositorThread(bool rubberBandingOnCompositorThread)
{
}

void WebSettingsImpl::setClobberUserAgentInitialScaleQuirk(bool clobberUserAgentInitialScaleQuirk)
{
    m_clobberUserAgentInitialScaleQuirk = clobberUserAgentInitialScaleQuirk;
}

void WebSettingsImpl::setSupportsMultipleWindows(bool supportsMultipleWindows)
{
    m_settings->setSupportsMultipleWindows(supportsMultipleWindows);
}

void WebSettingsImpl::setLoadsImagesAutomatically(bool loadsImagesAutomatically)
{
    m_settings->setLoadsImagesAutomatically(loadsImagesAutomatically);
}

void WebSettingsImpl::setImageAnimationPolicy(ImageAnimationPolicy policy)
{
    m_settings->setImageAnimationPolicy(static_cast<blink::ImageAnimationPolicy>(policy));
}

void WebSettingsImpl::setImagesEnabled(bool enabled)
{
    m_settings->setImagesEnabled(enabled);
}

void WebSettingsImpl::setLoadWithOverviewMode(bool enabled)
{
    m_settings->setLoadWithOverviewMode(enabled);
}

void WebSettingsImpl::setPluginsEnabled(bool enabled)
{
    m_devToolsEmulator->setPluginsEnabled(enabled);
}

void WebSettingsImpl::setAvailablePointerTypes(int pointers)
{
    m_settings->setAvailablePointerTypes(pointers);
}

void WebSettingsImpl::setPrimaryPointerType(PointerType pointer)
{
    m_settings->setPrimaryPointerType(static_cast<blink::PointerType>(pointer));
}

void WebSettingsImpl::setAvailableHoverTypes(int types)
{
    m_settings->setAvailableHoverTypes(types);
}

void WebSettingsImpl::setPrimaryHoverType(HoverType type)
{
    m_settings->setPrimaryHoverType(static_cast<blink::HoverType>(type));
}

void WebSettingsImpl::setDOMPasteAllowed(bool enabled)
{
    m_settings->setDOMPasteAllowed(enabled);
}

void WebSettingsImpl::setShrinksViewportContentToFit(bool shrinkViewportContent)
{
    m_shrinksViewportContentToFit = shrinkViewportContent;
}

void WebSettingsImpl::setSpatialNavigationEnabled(bool enabled)
{
    m_settings->setSpatialNavigationEnabled(enabled);
}

void WebSettingsImpl::setUsesEncodingDetector(bool usesDetector)
{
    m_settings->setUsesEncodingDetector(usesDetector);
}

void WebSettingsImpl::setTextAreasAreResizable(bool areResizable)
{
    m_settings->setTextAreasAreResizable(areResizable);
}

void WebSettingsImpl::setJavaEnabled(bool enabled)
{
    m_settings->setJavaEnabled(enabled);
}

void WebSettingsImpl::setAllowScriptsToCloseWindows(bool allow)
{
    m_settings->setAllowScriptsToCloseWindows(allow);
}

void WebSettingsImpl::setUseLegacyBackgroundSizeShorthandBehavior(bool useLegacyBackgroundSizeShorthandBehavior)
{
    m_settings->setUseLegacyBackgroundSizeShorthandBehavior(useLegacyBackgroundSizeShorthandBehavior);
}

void WebSettingsImpl::setWideViewportQuirkEnabled(bool wideViewportQuirkEnabled)
{
    m_settings->setWideViewportQuirkEnabled(wideViewportQuirkEnabled);
}

void WebSettingsImpl::setUseWideViewport(bool useWideViewport)
{
    m_settings->setUseWideViewport(useWideViewport);
}

void WebSettingsImpl::setDoubleTapToZoomEnabled(bool doubleTapToZoomEnabled)
{
    m_devToolsEmulator->setDoubleTapToZoomEnabled(doubleTapToZoomEnabled);
}

void WebSettingsImpl::setDownloadableBinaryFontsEnabled(bool enabled)
{
    m_settings->setDownloadableBinaryFontsEnabled(enabled);
}

void WebSettingsImpl::setJavaScriptCanAccessClipboard(bool enabled)
{
    m_settings->setJavaScriptCanAccessClipboard(enabled);
}

void WebSettingsImpl::setXSSAuditorEnabled(bool enabled)
{
    m_settings->setXSSAuditorEnabled(enabled);
}

void WebSettingsImpl::setTextTrackKindUserPreference(TextTrackKindUserPreference preference)
{
    m_settings->setTextTrackKindUserPreference(static_cast<blink::TextTrackKindUserPreference>(preference));
}

void WebSettingsImpl::setTextTrackBackgroundColor(const WebString& color)
{
    m_settings->setTextTrackBackgroundColor(color);
}

void WebSettingsImpl::setTextTrackFontFamily(const WebString& fontFamily)
{
    m_settings->setTextTrackFontFamily(fontFamily);
}

void WebSettingsImpl::setTextTrackFontStyle(const WebString& fontStyle)
{
    m_settings->setTextTrackFontStyle(fontStyle);
}

void WebSettingsImpl::setTextTrackFontVariant(const WebString& fontVariant)
{
    m_settings->setTextTrackFontVariant(fontVariant);
}

void WebSettingsImpl::setTextTrackTextColor(const WebString& color)
{
    m_settings->setTextTrackTextColor(color);
}

void WebSettingsImpl::setTextTrackTextShadow(const WebString& shadow)
{
    m_settings->setTextTrackTextShadow(shadow);
}

void WebSettingsImpl::setTextTrackTextSize(const WebString& size)
{
    m_settings->setTextTrackTextSize(size);
}

void WebSettingsImpl::setUnsafePluginPastingEnabled(bool enabled)
{
    m_settings->setUnsafePluginPastingEnabled(enabled);
}

void WebSettingsImpl::setDNSPrefetchingEnabled(bool enabled)
{
    m_settings->setDNSPrefetchingEnabled(enabled);
}

void WebSettingsImpl::setLocalStorageEnabled(bool enabled)
{
    m_settings->setLocalStorageEnabled(enabled);
}

void WebSettingsImpl::setMainFrameClipsContent(bool enabled)
{
    m_settings->setMainFrameClipsContent(enabled);
}

void WebSettingsImpl::setMaxTouchPoints(int maxTouchPoints)
{
    m_settings->setMaxTouchPoints(maxTouchPoints);
}

void WebSettingsImpl::setAllowUniversalAccessFromFileURLs(bool allow)
{
    m_settings->setAllowUniversalAccessFromFileURLs(allow);
}

void WebSettingsImpl::setAllowFileAccessFromFileURLs(bool allow)
{
    m_settings->setAllowFileAccessFromFileURLs(allow);
}

void WebSettingsImpl::setThreadedScrollingEnabled(bool enabled)
{
    m_settings->setThreadedScrollingEnabled(enabled);
}

void WebSettingsImpl::setTouchDragDropEnabled(bool enabled)
{
    m_settings->setTouchDragDropEnabled(enabled);
}

void WebSettingsImpl::setTouchEditingEnabled(bool enabled)
{
    m_settings->setTouchEditingEnabled(enabled);
}

void WebSettingsImpl::setOfflineWebApplicationCacheEnabled(bool enabled)
{
    m_settings->setOfflineWebApplicationCacheEnabled(enabled);
}

void WebSettingsImpl::setWebAudioEnabled(bool enabled)
{
    m_settings->setWebAudioEnabled(enabled);
}

void WebSettingsImpl::setExperimentalWebGLEnabled(bool enabled)
{
    m_settings->setWebGLEnabled(enabled);
}

void WebSettingsImpl::setOpenGLMultisamplingEnabled(bool enabled)
{
    m_settings->setOpenGLMultisamplingEnabled(enabled);
}

void WebSettingsImpl::setRenderVSyncNotificationEnabled(bool enabled)
{
    m_renderVSyncNotificationEnabled = enabled;
}

void WebSettingsImpl::setWebGLErrorsToConsoleEnabled(bool enabled)
{
    m_settings->setWebGLErrorsToConsoleEnabled(enabled);
}

void WebSettingsImpl::setShowContextMenuOnMouseUp(bool enabled)
{
    m_settings->setShowContextMenuOnMouseUp(enabled);
}

void WebSettingsImpl::setShowFPSCounter(bool show)
{
    m_showFPSCounter = show;
}

void WebSettingsImpl::setShowPaintRects(bool show)
{
    m_showPaintRects = show;
}

void WebSettingsImpl::setEditingBehavior(EditingBehavior behavior)
{
    m_settings->setEditingBehaviorType(static_cast<EditingBehaviorType>(behavior));
}

void WebSettingsImpl::setAcceleratedCompositingEnabled(bool enabled)
{
    m_settings->setAcceleratedCompositingEnabled(enabled);
}

void WebSettingsImpl::setMockScrollbarsEnabled(bool enabled)
{
    m_settings->setMockScrollbarsEnabled(enabled);
}

void WebSettingsImpl::setMockGestureTapHighlightsEnabled(bool enabled)
{
    m_settings->setMockGestureTapHighlightsEnabled(enabled);
}

void WebSettingsImpl::setAccelerated2dCanvasEnabled(bool enabled)
{
    m_settings->setAccelerated2dCanvasEnabled(enabled);
}

void WebSettingsImpl::setAccelerated2dCanvasMSAASampleCount(int count)
{
    m_settings->setAccelerated2dCanvasMSAASampleCount(count);
}

void WebSettingsImpl::setAntialiased2dCanvasEnabled(bool enabled)
{
    m_settings->setAntialiased2dCanvasEnabled(enabled);
}

void WebSettingsImpl::setAntialiasedClips2dCanvasEnabled(bool enabled)
{
    m_settings->setAntialiasedClips2dCanvasEnabled(enabled);
}

void WebSettingsImpl::setPreferCompositingToLCDTextEnabled(bool enabled)
{
    m_devToolsEmulator->setPreferCompositingToLCDTextEnabled(enabled);
}

void WebSettingsImpl::setMinimumAccelerated2dCanvasSize(int numPixels)
{
    m_settings->setMinimumAccelerated2dCanvasSize(numPixels);
}

void WebSettingsImpl::setHyperlinkAuditingEnabled(bool enabled)
{
    m_settings->setHyperlinkAuditingEnabled(enabled);
}

void WebSettingsImpl::setAsynchronousSpellCheckingEnabled(bool enabled)
{
    m_settings->setAsynchronousSpellCheckingEnabled(enabled);
}

void WebSettingsImpl::setUnifiedTextCheckerEnabled(bool enabled)
{
    m_settings->setUnifiedTextCheckerEnabled(enabled);
}

void WebSettingsImpl::setCaretBrowsingEnabled(bool enabled)
{
    m_settings->setCaretBrowsingEnabled(enabled);
}

void WebSettingsImpl::setValidationMessageTimerMagnification(int newValue)
{
    m_settings->setValidationMessageTimerMagnification(newValue);
}

void WebSettingsImpl::setAllowDisplayOfInsecureContent(bool enabled)
{
    m_settings->setAllowDisplayOfInsecureContent(enabled);
}

void WebSettingsImpl::setAllowRunningOfInsecureContent(bool enabled)
{
    m_settings->setAllowRunningOfInsecureContent(enabled);
}

void WebSettingsImpl::setDisableReadingFromCanvas(bool enabled)
{
    m_settings->setDisableReadingFromCanvas(enabled);
}

void WebSettingsImpl::setStrictMixedContentChecking(bool enabled)
{
    m_settings->setStrictMixedContentChecking(enabled);
}

void WebSettingsImpl::setStrictPowerfulFeatureRestrictions(bool enabled)
{
    m_settings->setStrictPowerfulFeatureRestrictions(enabled);
}

void WebSettingsImpl::setPasswordEchoEnabled(bool flag)
{
    m_settings->setPasswordEchoEnabled(flag);
}

void WebSettingsImpl::setPasswordEchoDurationInSeconds(double durationInSeconds)
{
    m_settings->setPasswordEchoDurationInSeconds(durationInSeconds);
}

void WebSettingsImpl::setPerTilePaintingEnabled(bool enabled)
{
    m_perTilePaintingEnabled = enabled;
}

void WebSettingsImpl::setShouldPrintBackgrounds(bool enabled)
{
    m_settings->setShouldPrintBackgrounds(enabled);
}

void WebSettingsImpl::setShouldClearDocumentBackground(bool enabled)
{
    m_settings->setShouldClearDocumentBackground(enabled);
}

void WebSettingsImpl::setEnableScrollAnimator(bool enabled)
{
    m_settings->setScrollAnimatorEnabled(enabled);
}

void WebSettingsImpl::setEnableTouchAdjustment(bool enabled)
{
    m_settings->setTouchAdjustmentEnabled(enabled);
}

bool WebSettingsImpl::multiTargetTapNotificationEnabled()
{
    return m_settings->multiTargetTapNotificationEnabled();
}

void WebSettingsImpl::setMultiTargetTapNotificationEnabled(bool enabled)
{
    m_settings->setMultiTargetTapNotificationEnabled(enabled);
}

int WebSettingsImpl::availablePointerTypes() const
{
    return m_settings->availablePointerTypes();
}

WebSettings::PointerType WebSettingsImpl::primaryPointerType() const
{
    return static_cast<PointerType>(m_settings->primaryPointerType());
}

int WebSettingsImpl::availableHoverTypes() const
{
    return m_settings->availableHoverTypes();
}

WebSettings::HoverType WebSettingsImpl::primaryHoverType() const
{
    return static_cast<HoverType>(m_settings->primaryHoverType());
}

bool WebSettingsImpl::viewportEnabled() const
{
    return m_settings->viewportEnabled();
}

bool WebSettingsImpl::viewportMetaEnabled() const
{
    return m_settings->viewportMetaEnabled();
}

bool WebSettingsImpl::doubleTapToZoomEnabled() const
{
    return m_devToolsEmulator->doubleTapToZoomEnabled();
}

bool WebSettingsImpl::mockGestureTapHighlightsEnabled() const
{
    return m_settings->mockGestureTapHighlightsEnabled();
}

bool WebSettingsImpl::mainFrameResizesAreOrientationChanges() const
{
    return m_mainFrameResizesAreOrientationChanges;
}

bool WebSettingsImpl::shrinksViewportContentToFit() const
{
    return m_shrinksViewportContentToFit;
}

void WebSettingsImpl::setShouldRespectImageOrientation(bool enabled)
{
    m_settings->setShouldRespectImageOrientation(enabled);
}

void WebSettingsImpl::setMediaControlsOverlayPlayButtonEnabled(bool enabled)
{
    m_settings->setMediaControlsOverlayPlayButtonEnabled(enabled);
}

void WebSettingsImpl::setMediaPlaybackRequiresUserGesture(bool required)
{
    m_settings->setMediaPlaybackRequiresUserGesture(required);
}

void WebSettingsImpl::setViewportEnabled(bool enabled)
{
    m_settings->setViewportEnabled(enabled);
}

void WebSettingsImpl::setViewportMetaEnabled(bool enabled)
{
    m_settings->setViewportMetaEnabled(enabled);
}

void WebSettingsImpl::setSyncXHRInDocumentsEnabled(bool enabled)
{
    m_settings->setSyncXHRInDocumentsEnabled(enabled);
}

void WebSettingsImpl::setCookieEnabled(bool enabled)
{
    m_settings->setCookieEnabled(enabled);
}

void WebSettingsImpl::setNavigateOnDragDrop(bool enabled)
{
    m_settings->setNavigateOnDragDrop(enabled);
}

void WebSettingsImpl::setAllowCustomScrollbarInMainFrame(bool enabled)
{
    m_settings->setAllowCustomScrollbarInMainFrame(enabled);
}

void WebSettingsImpl::setSelectTrailingWhitespaceEnabled(bool enabled)
{
    m_settings->setSelectTrailingWhitespaceEnabled(enabled);
}

void WebSettingsImpl::setSelectionIncludesAltImageText(bool enabled)
{
    m_settings->setSelectionIncludesAltImageText(enabled);
}

void WebSettingsImpl::setSelectionStrategy(SelectionStrategyType strategy)
{
    m_settings->setSelectionStrategy(static_cast<SelectionStrategy>(strategy));
}


void WebSettingsImpl::setSmartInsertDeleteEnabled(bool enabled)
{
    m_settings->setSmartInsertDeleteEnabled(enabled);
}

void WebSettingsImpl::setPinchOverlayScrollbarThickness(int thickness)
{
    m_settings->setPinchOverlayScrollbarThickness(thickness);
}

void WebSettingsImpl::setUseSolidColorScrollbars(bool enabled)
{
    m_settings->setUseSolidColorScrollbars(enabled);
}

void WebSettingsImpl::setMainFrameResizesAreOrientationChanges(bool enabled)
{
    m_mainFrameResizesAreOrientationChanges = enabled;
}

void WebSettingsImpl::setV8CacheOptions(V8CacheOptions options)
{
    m_settings->setV8CacheOptions(static_cast<blink::V8CacheOptions>(options));
}

void WebSettingsImpl::setUseMobileViewportStyle(bool enabled)
{
    m_devToolsEmulator->setUseMobileViewportStyle(enabled);
}

} // namespace blink
