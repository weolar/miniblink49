// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RuntimeEnabledFeatures_h
#define RuntimeEnabledFeatures_h

#include "platform/PlatformExport.h"
#include "wtf/Forward.h"

namespace blink {

// A class that stores static enablers for all experimental features.

class PLATFORM_EXPORT RuntimeEnabledFeatures {
public:
    static void setStableFeaturesEnabled(bool);
    static void setExperimentalFeaturesEnabled(bool);
    static void setTestFeaturesEnabled(bool);

    static void setFeatureEnabledFromString(const WTF::String& name, bool isEnabled);

    static void setAppBannerEnabled(bool isEnabled) { isAppBannerEnabled = isEnabled; }
    static bool appBannerEnabled() { return isAppBannerEnabled; }

    static void setAlwaysUseComplexTextEnabled(bool isEnabled) { isAlwaysUseComplexTextEnabled = isEnabled; }
    static bool alwaysUseComplexTextEnabled() { return isAlwaysUseComplexTextEnabled; }

    static void setApplicationCacheEnabled(bool isEnabled) { isApplicationCacheEnabled = isEnabled; }
    static bool applicationCacheEnabled() { return isApplicationCacheEnabled; }

    static void setAudioOutputDevicesEnabled(bool isEnabled) { isAudioOutputDevicesEnabled = isEnabled; }
    static bool audioOutputDevicesEnabled() { return isAudioOutputDevicesEnabled; }

    static void setAudioVideoTracksEnabled(bool isEnabled) { isAudioVideoTracksEnabled = isEnabled; }
    static bool audioVideoTracksEnabled() { return isAudioVideoTracksEnabled && isMediaEnabled; }

    static void setAuthorShadowDOMForAnyElementEnabled(bool isEnabled) { isAuthorShadowDOMForAnyElementEnabled = isEnabled; }
    static bool authorShadowDOMForAnyElementEnabled() { return isAuthorShadowDOMForAnyElementEnabled; }

    static void setBackgroundSyncEnabled(bool isEnabled) { isBackgroundSyncEnabled = isEnabled; }
    static bool backgroundSyncEnabled() { return isBackgroundSyncEnabled; }

    static void setBackgroundSyncV2Enabled(bool isEnabled) { isBackgroundSyncV2Enabled = isEnabled; }
    static bool backgroundSyncV2Enabled() { return isBackgroundSyncV2Enabled; }

    static void setBleedingEdgeFastPathsEnabled(bool isEnabled) { isBleedingEdgeFastPathsEnabled = isEnabled; }
    static bool bleedingEdgeFastPathsEnabled() { return isBleedingEdgeFastPathsEnabled; }

    static void setBlinkSchedulerEnabled(bool isEnabled) { isBlinkSchedulerEnabled = isEnabled; }
    static bool blinkSchedulerEnabled() { return isBlinkSchedulerEnabled; }

    static void setClientHintsEnabled(bool isEnabled) { isClientHintsEnabled = isEnabled; }
    static bool clientHintsEnabled() { return isClientHintsEnabled; }

    static void setColumnFillEnabled(bool isEnabled) { isColumnFillEnabled = isEnabled; }
    static bool columnFillEnabled() { return isColumnFillEnabled; }

    static void setCompositedSelectionUpdateEnabled(bool isEnabled) { isCompositedSelectionUpdateEnabled = isEnabled; }
    static bool compositedSelectionUpdateEnabled() { return isCompositedSelectionUpdateEnabled; }

    static void setCompositorWorkerEnabled(bool isEnabled) { isCompositorWorkerEnabled = isEnabled; }
    static bool compositorWorkerEnabled() { return isCompositorWorkerEnabled; }

    static void setCompositorAnimationTimelinesEnabled(bool isEnabled) { isCompositorAnimationTimelinesEnabled = isEnabled; }
    static bool compositorAnimationTimelinesEnabled() { return isCompositorAnimationTimelinesEnabled; }

    static void setContextMenuEnabled(bool isEnabled) { isContextMenuEnabled = isEnabled; }
    static bool contextMenuEnabled() { return isContextMenuEnabled; }

    static void setCreateShadowRootWithParameterEnabled(bool isEnabled) { isCreateShadowRootWithParameterEnabled = isEnabled; }
    static bool createShadowRootWithParameterEnabled() { return isCreateShadowRootWithParameterEnabled; }

    static void setCredentialManagerEnabled(bool isEnabled) { isCredentialManagerEnabled = isEnabled; }
    static bool credentialManagerEnabled() { return isCredentialManagerEnabled; }

    static void setCSS3TextEnabled(bool isEnabled) { isCSS3TextEnabled = isEnabled; }
    static bool css3TextEnabled() { return isCSS3TextEnabled; }

    static void setCSS3TextDecorationsEnabled(bool isEnabled) { isCSS3TextDecorationsEnabled = isEnabled; }
    static bool css3TextDecorationsEnabled() { return isCSS3TextDecorationsEnabled; }

    static void setCSSAttributeCaseSensitivityEnabled(bool isEnabled) { isCSSAttributeCaseSensitivityEnabled = isEnabled; }
    static bool cssAttributeCaseSensitivityEnabled() { return isCSSAttributeCaseSensitivityEnabled; }

    static void setCSSCompositingEnabled(bool isEnabled) { isCSSCompositingEnabled = isEnabled; }
    static bool cssCompositingEnabled() { return isCSSCompositingEnabled; }

    static void setCSSFontSizeAdjustEnabled(bool isEnabled) { isCSSFontSizeAdjustEnabled = isEnabled; }
    static bool cssFontSizeAdjustEnabled() { return isCSSFontSizeAdjustEnabled; }

    static void setCSSGridLayoutEnabled(bool isEnabled) { isCSSGridLayoutEnabled = isEnabled; }
    static bool cssGridLayoutEnabled() { return isCSSGridLayoutEnabled; }

    static void setCSSIndependentTransformPropertiesEnabled(bool isEnabled) { isCSSIndependentTransformPropertiesEnabled = isEnabled; }
    static bool cssIndependentTransformPropertiesEnabled() { return isCSSIndependentTransformPropertiesEnabled; }

    static void setCSSMaskSourceTypeEnabled(bool isEnabled) { isCSSMaskSourceTypeEnabled = isEnabled; }
    static bool cssMaskSourceTypeEnabled() { return isCSSMaskSourceTypeEnabled; }

    static void setCSSMotionPathEnabled(bool isEnabled) { isCSSMotionPathEnabled = isEnabled; }
    static bool cssMotionPathEnabled() { return isCSSMotionPathEnabled; }

    static void setCSSOMSmoothScrollEnabled(bool isEnabled) { isCSSOMSmoothScrollEnabled = isEnabled; }
    static bool cssomSmoothScrollEnabled() { return isCSSOMSmoothScrollEnabled; }

    static void setCSSScrollBlocksOnEnabled(bool isEnabled) { isCSSScrollBlocksOnEnabled = isEnabled; }
    static bool cssScrollBlocksOnEnabled() { return isCSSScrollBlocksOnEnabled; }

    static void setCSSTouchActionPanDirectionsEnabled(bool isEnabled) { isCSSTouchActionPanDirectionsEnabled = isEnabled; }
    static bool cssTouchActionPanDirectionsEnabled() { return isCSSTouchActionPanDirectionsEnabled; }

    static void setCSSViewportEnabled(bool isEnabled) { isCSSViewportEnabled = isEnabled; }
    static bool cssViewportEnabled() { return isCSSViewportEnabled; }

    static void setCSSScrollSnapPointsEnabled(bool isEnabled) { isCSSScrollSnapPointsEnabled = isEnabled; }
    static bool cssScrollSnapPointsEnabled() { return isCSSScrollSnapPointsEnabled; }

    static void setCustomSchemeHandlerEnabled(bool isEnabled) { isCustomSchemeHandlerEnabled = isEnabled; }
    static bool customSchemeHandlerEnabled() { return isCustomSchemeHandlerEnabled && isNavigatorContentUtilsEnabled; }

    static void setDatabaseEnabled(bool isEnabled) { isDatabaseEnabled = isEnabled; }
    static bool databaseEnabled() { return isDatabaseEnabled; }

    static void setDecodeToYUVEnabled(bool isEnabled) { isDecodeToYUVEnabled = isEnabled; }
    static bool decodeToYUVEnabled() { return isDecodeToYUVEnabled; }

    static void setDeviceLightEnabled(bool isEnabled) { isDeviceLightEnabled = isEnabled; }
    static bool deviceLightEnabled() { return isDeviceLightEnabled; }

    static void setDisplayList2dCanvasEnabled(bool isEnabled) { isDisplayList2dCanvasEnabled = isEnabled; }
    static bool displayList2dCanvasEnabled() { return isDisplayList2dCanvasEnabled; }

    static void setDOMConvenienceAPIEnabled(bool isEnabled) { isDOMConvenienceAPIEnabled = isEnabled; }
    static bool dOMConvenienceAPIEnabled() { return isDOMConvenienceAPIEnabled; }

    static void setForceDisplayList2dCanvasEnabled(bool isEnabled) { isForceDisplayList2dCanvasEnabled = isEnabled; }
    static bool forceDisplayList2dCanvasEnabled() { return isForceDisplayList2dCanvasEnabled; }

    static void setEncryptedMediaEnabled(bool isEnabled) { isEncryptedMediaEnabled = isEnabled; }
    static bool encryptedMediaEnabled() { return isEncryptedMediaEnabled; }

    static void setEnumerateDevicesEnabled(bool isEnabled) { isEnumerateDevicesEnabled = isEnabled; }
    static bool enumerateDevicesEnabled() { return isEnumerateDevicesEnabled; }

    static void setExecCommandInJavaScriptEnabled(bool isEnabled) { isExecCommandInJavaScriptEnabled = isEnabled; }
    static bool execCommandInJavaScriptEnabled() { return isExecCommandInJavaScriptEnabled; }

    static void setComputedAccessibilityInfoEnabled(bool isEnabled) { isComputedAccessibilityInfoEnabled = isEnabled; }
    static bool computedAccessibilityInfoEnabled() { return isComputedAccessibilityInfoEnabled; }

    static void setExperimentalCanvasFeaturesEnabled(bool isEnabled) { isExperimentalCanvasFeaturesEnabled = isEnabled; }
    static bool experimentalCanvasFeaturesEnabled() { return isExperimentalCanvasFeaturesEnabled; }

    static void setExperimentalContentSecurityPolicyFeaturesEnabled(bool isEnabled) { isExperimentalContentSecurityPolicyFeaturesEnabled = isEnabled; }
    static bool experimentalContentSecurityPolicyFeaturesEnabled() { return isExperimentalContentSecurityPolicyFeaturesEnabled; }

    static void setFastMobileScrollingEnabled(bool isEnabled) { isFastMobileScrollingEnabled = isEnabled; }
    static bool fastMobileScrollingEnabled() { return isFastMobileScrollingEnabled; }

    static void setFileAPIBlobCloseEnabled(bool isEnabled) { isFileAPIBlobCloseEnabled = isEnabled; }
    static bool fileAPIBlobCloseEnabled() { return isFileAPIBlobCloseEnabled; }

    static void setFileSystemEnabled(bool isEnabled) { isFileSystemEnabled = isEnabled; }
    static bool fileSystemEnabled() { return isFileSystemEnabled; }

    static void setFormDataNewMethodsEnabled(bool isEnabled) { isFormDataNewMethodsEnabled = isEnabled; }
    static bool formDataNewMethodsEnabled() { return isFormDataNewMethodsEnabled; }

    static void setFullscreenUnprefixedEnabled(bool isEnabled) { isFullscreenUnprefixedEnabled = isEnabled; }
    static bool fullscreenUnprefixedEnabled() { return isFullscreenUnprefixedEnabled; }

    static void setFrameTimingSupportEnabled(bool isEnabled) { isFrameTimingSupportEnabled = isEnabled; }
    static bool frameTimingSupportEnabled() { return isFrameTimingSupportEnabled; }

    static void setGeofencingEnabled(bool isEnabled) { isGeofencingEnabled = isEnabled; }
    static bool geofencingEnabled() { return isGeofencingEnabled; }

    static void setGeometryInterfacesEnabled(bool isEnabled) { isGeometryInterfacesEnabled = isEnabled; }
    static bool geometryInterfacesEnabled() { return isGeometryInterfacesEnabled; }

    static void setGlobalCacheStorageEnabled(bool isEnabled) { isGlobalCacheStorageEnabled = isEnabled; }
    static bool globalCacheStorageEnabled() { return isGlobalCacheStorageEnabled; }

    static void setImageColorProfilesEnabled(bool isEnabled) { isImageColorProfilesEnabled = isEnabled; }
    static bool imageColorProfilesEnabled() { return isImageColorProfilesEnabled; }

    static void setImageRenderingPixelatedEnabled(bool isEnabled) { isImageRenderingPixelatedEnabled = isEnabled; }
    static bool imageRenderingPixelatedEnabled() { return isImageRenderingPixelatedEnabled; }

    static void setIndexedDBExperimentalEnabled(bool isEnabled) { isIndexedDBExperimentalEnabled = isEnabled; }
    static bool indexedDBExperimentalEnabled() { return isIndexedDBExperimentalEnabled; }

    static void setInputDeviceEnabled(bool isEnabled) { isInputDeviceEnabled = isEnabled; }
    static bool inputDeviceEnabled() { return isInputDeviceEnabled; }

    static void setInputModeAttributeEnabled(bool isEnabled) { isInputModeAttributeEnabled = isEnabled; }
    static bool inputModeAttributeEnabled() { return isInputModeAttributeEnabled; }

    static void setKeyboardEventCodeEnabled(bool isEnabled) { isKeyboardEventCodeEnabled = isEnabled; }
    static bool keyboardEventCodeEnabled() { return isKeyboardEventCodeEnabled; }

    static void setKeyboardEventKeyEnabled(bool isEnabled) { isKeyboardEventKeyEnabled = isEnabled; }
    static bool keyboardEventKeyEnabled() { return isKeyboardEventKeyEnabled; }

    static void setLangAttributeAwareFormControlUIEnabled(bool isEnabled) { isLangAttributeAwareFormControlUIEnabled = isEnabled; }
    static bool langAttributeAwareFormControlUIEnabled() { return isLangAttributeAwareFormControlUIEnabled; }

    static void setLinkPreconnectEnabled(bool isEnabled) { isLinkPreconnectEnabled = isEnabled; }
    static bool linkPreconnectEnabled() { return isLinkPreconnectEnabled; }

    static void setLinkPreloadEnabled(bool isEnabled) { isLinkPreloadEnabled = isEnabled; }
    static bool linkPreloadEnabled() { return isLinkPreloadEnabled; }

    static void setLinkHeaderEnabled(bool isEnabled) { isLinkHeaderEnabled = isEnabled; }
    static bool linkHeaderEnabled() { return isLinkHeaderEnabled; }

    static void setFractionalScrollOffsetsEnabled(bool isEnabled) { isFractionalScrollOffsetsEnabled = isEnabled; }
    static bool fractionalScrollOffsetsEnabled() { return isFractionalScrollOffsetsEnabled; }

    static void setMediaEnabled(bool isEnabled) { isMediaEnabled = isEnabled; }
    static bool mediaEnabled() { return isMediaEnabled; }

    static void setMediaCaptureEnabled(bool isEnabled) { isMediaCaptureEnabled = isEnabled; }
    static bool mediaCaptureEnabled() { return isMediaCaptureEnabled; }

    static void setMediaControllerEnabled(bool isEnabled) { isMediaControllerEnabled = isEnabled; }
    static bool mediaControllerEnabled() { return isMediaControllerEnabled && isMediaEnabled; }

    static void setMediaSessionEnabled(bool isEnabled) { isMediaSessionEnabled = isEnabled; }
    static bool mediaSessionEnabled() { return isMediaSessionEnabled; }

    static void setMediaSourceEnabled(bool isEnabled) { isMediaSourceEnabled = isEnabled; }
    static bool mediaSourceEnabled() { return isMediaSourceEnabled; }

    static void setMediaSourceExperimentalEnabled(bool isEnabled) { isMediaSourceExperimentalEnabled = isEnabled; }
    static bool mediaSourceExperimentalEnabled() { return isMediaSourceExperimentalEnabled && isMediaSourceEnabled; }

    static void setMediaStreamSpeechEnabled(bool isEnabled) { isMediaStreamSpeechEnabled = isEnabled; }
    static bool mediaStreamSpeechEnabled() { return isMediaStreamSpeechEnabled; }

    static void setMemoryInfoInWorkersEnabled(bool isEnabled) { isMemoryInfoInWorkersEnabled = isEnabled; }
    static bool memoryInfoInWorkersEnabled() { return isMemoryInfoInWorkersEnabled; }

    static void setNavigatorConnectEnabled(bool isEnabled) { isNavigatorConnectEnabled = isEnabled; }
    static bool navigatorConnectEnabled() { return isNavigatorConnectEnabled; }

    static void setNavigatorContentUtilsEnabled(bool isEnabled) { isNavigatorContentUtilsEnabled = isEnabled; }
    static bool navigatorContentUtilsEnabled() { return isNavigatorContentUtilsEnabled; }

    static void setNetworkInformationEnabled(bool isEnabled) { isNetworkInformationEnabled = isEnabled; }
    static bool networkInformationEnabled() { return isNetworkInformationEnabled; }

    static void setNotificationConstructorEnabled(bool isEnabled) { isNotificationConstructorEnabled = isEnabled; }
    static bool notificationConstructorEnabled() { return isNotificationConstructorEnabled; }

    static void setNotificationExperimentalEnabled(bool isEnabled) { isNotificationExperimentalEnabled = isEnabled; }
    static bool notificationExperimentalEnabled() { return isNotificationExperimentalEnabled; }

    static void setNotificationsEnabled(bool isEnabled) { isNotificationsEnabled = isEnabled; }
    static bool notificationsEnabled() { return isNotificationsEnabled; }

    static void setOrientationEventEnabled(bool isEnabled) { isOrientationEventEnabled = isEnabled; }
    static bool orientationEventEnabled() { return isOrientationEventEnabled; }

    static void setOverlayFullscreenVideoEnabled(bool isEnabled) { isOverlayFullscreenVideoEnabled = isEnabled; }
    static bool overlayFullscreenVideoEnabled() { return isOverlayFullscreenVideoEnabled; }

    static void setOverlayScrollbarsEnabled(bool isEnabled) { isOverlayScrollbarsEnabled = isEnabled; }
    static bool overlayScrollbarsEnabled() { return isOverlayScrollbarsEnabled; }

    static void setPagePopupEnabled(bool isEnabled) { isPagePopupEnabled = isEnabled; }
    static bool pagePopupEnabled() { return isPagePopupEnabled; }

    static void setPathOpsSVGClippingEnabled(bool isEnabled) { isPathOpsSVGClippingEnabled = isEnabled; }
    static bool pathOpsSVGClippingEnabled() { return isPathOpsSVGClippingEnabled; }

    static void setPermissionsEnabled(bool isEnabled) { isPermissionsEnabled = isEnabled; }
    static bool permissionsEnabled() { return isPermissionsEnabled; }

    static void setPointerEventEnabled(bool isEnabled) { isPointerEventEnabled = isEnabled; }
    static bool pointerEventEnabled() { return isPointerEventEnabled; }

    static void setPreciseMemoryInfoEnabled(bool isEnabled) { isPreciseMemoryInfoEnabled = isEnabled; }
    static bool preciseMemoryInfoEnabled() { return isPreciseMemoryInfoEnabled; }

    static void setPrefixedEncryptedMediaEnabled(bool isEnabled) { isPrefixedEncryptedMediaEnabled = isEnabled; }
    static bool prefixedEncryptedMediaEnabled() { return isPrefixedEncryptedMediaEnabled; }

    static void setPrefixedVideoFullscreenEnabled(bool isEnabled) { isPrefixedVideoFullscreenEnabled = isEnabled; }
    static bool prefixedVideoFullscreenEnabled() { return isPrefixedVideoFullscreenEnabled; }

    static void setPresentationEnabled(bool isEnabled) { isPresentationEnabled = isEnabled; }
    static bool presentationEnabled() { return isPresentationEnabled; }

    static void setPromiseRejectionEventEnabled(bool isEnabled) { isPromiseRejectionEventEnabled = isEnabled; }
    static bool promiseRejectionEventEnabled() { return isPromiseRejectionEventEnabled; }

    static void setPushMessagingEnabled(bool isEnabled) { isPushMessagingEnabled = isEnabled; }
    static bool pushMessagingEnabled() { return isPushMessagingEnabled; }

    static void setPushMessagingDataEnabled(bool isEnabled) { isPushMessagingDataEnabled = isEnabled; }
    static bool pushMessagingDataEnabled() { return isPushMessagingDataEnabled; }

    static void setQuotaPromiseEnabled(bool isEnabled) { isQuotaPromiseEnabled = isEnabled; }
    static bool quotaPromiseEnabled() { return isQuotaPromiseEnabled; }

    static void setReducedReferrerGranularityEnabled(bool isEnabled) { isReducedReferrerGranularityEnabled = isEnabled; }
    static bool reducedReferrerGranularityEnabled() { return isReducedReferrerGranularityEnabled; }

    static void setRequestAutocompleteEnabled(bool isEnabled) { isRequestAutocompleteEnabled = isEnabled; }
    static bool requestAutocompleteEnabled() { return isRequestAutocompleteEnabled; }

    static void setSandboxBlocksModalsEnabled(bool isEnabled) { isSandboxBlocksModalsEnabled = isEnabled; }
    static bool sandboxBlocksModalsEnabled() { return isSandboxBlocksModalsEnabled; }

    static void setScreenOrientationEnabled(bool isEnabled) { isScreenOrientationEnabled = isEnabled; }
    static bool screenOrientationEnabled() { return isScreenOrientationEnabled; }

    static void setScriptedSpeechEnabled(bool isEnabled) { isScriptedSpeechEnabled = isEnabled; }
    static bool scriptedSpeechEnabled() { return isScriptedSpeechEnabled; }

    static void setScrollTopLeftInteropEnabled(bool isEnabled) { isScrollTopLeftInteropEnabled = isEnabled; }
    static bool scrollTopLeftInteropEnabled() { return isScrollTopLeftInteropEnabled; }

    static void setSelectionForComposedTreeEnabled(bool isEnabled) { isSelectionForComposedTreeEnabled = isEnabled; }
    static bool selectionForComposedTreeEnabled() { return isSelectionForComposedTreeEnabled; }

    static void setServiceWorkerCacheAddAllEnabled(bool isEnabled) { isServiceWorkerCacheAddAllEnabled = isEnabled; }
    static bool serviceWorkerCacheAddAllEnabled() { return isServiceWorkerCacheAddAllEnabled; }

    static void setServiceWorkerClientAttributesEnabled(bool isEnabled) { isServiceWorkerClientAttributesEnabled = isEnabled; }
    static bool serviceWorkerClientAttributesEnabled() { return isServiceWorkerClientAttributesEnabled; }

    static void setServiceWorkerNotificationsEnabled(bool isEnabled) { isServiceWorkerNotificationsEnabled = isEnabled; }
    static bool serviceWorkerNotificationsEnabled() { return isServiceWorkerNotificationsEnabled; }

    static void setServiceWorkerPerformanceTimelineEnabled(bool isEnabled) { isServiceWorkerPerformanceTimelineEnabled = isEnabled; }
    static bool serviceWorkerPerformanceTimelineEnabled() { return isServiceWorkerPerformanceTimelineEnabled; }

    static void setShadowRootDelegatesFocusEnabled(bool isEnabled) { isShadowRootDelegatesFocusEnabled = isEnabled; }
    static bool shadowRootDelegatesFocusEnabled() { return isShadowRootDelegatesFocusEnabled; }

    static void setSharedArrayBufferEnabled(bool isEnabled) { isSharedArrayBufferEnabled = isEnabled; }
    static bool sharedArrayBufferEnabled() { return isSharedArrayBufferEnabled; }

    static void setSharedWorkerEnabled(bool isEnabled) { isSharedWorkerEnabled = isEnabled; }
    static bool sharedWorkerEnabled() { return isSharedWorkerEnabled; }

    static void setSlimmingPaintEnabled(bool isEnabled) { isSlimmingPaintEnabled = isEnabled; }
    static bool slimmingPaintEnabled() { return isSlimmingPaintEnabled; }

    static void setSlimmingPaintCompositorLayerizationEnabled(bool isEnabled) { isSlimmingPaintCompositorLayerizationEnabled = isEnabled; }
    static bool slimmingPaintCompositorLayerizationEnabled() { return isSlimmingPaintCompositorLayerizationEnabled && isSlimmingPaintEnabled; }

    static void setSlimmingPaintUnderInvalidationCheckingEnabled(bool isEnabled) { isSlimmingPaintUnderInvalidationCheckingEnabled = isEnabled; }
    static bool slimmingPaintUnderInvalidationCheckingEnabled() { return isSlimmingPaintUnderInvalidationCheckingEnabled && isSlimmingPaintEnabled; }

    static void setSlimmingPaintStrictCullRectClippingEnabled(bool isEnabled) { isSlimmingPaintStrictCullRectClippingEnabled = isEnabled; }
    static bool slimmingPaintStrictCullRectClippingEnabled() { return isSlimmingPaintStrictCullRectClippingEnabled && isSlimmingPaintEnabled; }

    static void setStaleWhileRevalidateCacheControlEnabled(bool isEnabled) { isStaleWhileRevalidateCacheControlEnabled = isEnabled; }
    static bool staleWhileRevalidateCacheControlEnabled() { return isStaleWhileRevalidateCacheControlEnabled; }

    static void setExperimentalStreamEnabled(bool isEnabled) { isExperimentalStreamEnabled = isEnabled; }
    static bool experimentalStreamEnabled() { return isExperimentalStreamEnabled; }

    static void setSuboriginsEnabled(bool isEnabled) { isSuboriginsEnabled = isEnabled; }
    static bool suboriginsEnabled() { return isSuboriginsEnabled; }

    static void setTextBlobEnabled(bool isEnabled) { isTextBlobEnabled = isEnabled; }
    static bool textBlobEnabled() { return isTextBlobEnabled; }

    static void setThreadedParserDataReceiverEnabled(bool isEnabled) { isThreadedParserDataReceiverEnabled = isEnabled; }
    static bool threadedParserDataReceiverEnabled() { return isThreadedParserDataReceiverEnabled; }

    static void setTouchEnabled(bool isEnabled) { isTouchEnabled = isEnabled; }
    static bool touchEnabled() { return isTouchEnabled; }

    static void setTouchIconLoadingEnabled(bool isEnabled) { isTouchIconLoadingEnabled = isEnabled; }
    static bool touchIconLoadingEnabled() { return isTouchIconLoadingEnabled; }

    static void setUnsafeES3APIsEnabled(bool isEnabled) { isUnsafeES3APIsEnabled = isEnabled; }
    static bool unsafeES3APIsEnabled() { return isUnsafeES3APIsEnabled; }

    static void setUnsandboxedAuxiliaryEnabled(bool isEnabled) { isUnsandboxedAuxiliaryEnabled = isEnabled; }
    static bool unsandboxedAuxiliaryEnabled() { return isUnsandboxedAuxiliaryEnabled; }

    static void setUserSelectAllEnabled(bool isEnabled) { isUserSelectAllEnabled = isEnabled; }
    static bool userSelectAllEnabled() { return isUserSelectAllEnabled; }

    static void setWebAnimationsAPIEnabled(bool isEnabled) { isWebAnimationsAPIEnabled = isEnabled; }
    static bool webAnimationsAPIEnabled() { return isWebAnimationsAPIEnabled; }

    static void setWebAnimationsSVGEnabled(bool isEnabled) { isWebAnimationsSVGEnabled = isEnabled; }
    static bool webAnimationsSVGEnabled() { return isWebAnimationsSVGEnabled; }

#if ENABLE(WEB_AUDIO)
    static void setWebAudioEnabled(bool isEnabled) { isWebAudioEnabled = isEnabled; }
    static bool webAudioEnabled() { return isWebAudioEnabled; }
#else
    static void setWebAudioEnabled(bool) { }
    static bool webAudioEnabled() { return false; }
#endif

    static void setWebBluetoothEnabled(bool isEnabled) { isWebBluetoothEnabled = isEnabled; }
    static bool webBluetoothEnabled() { return isWebBluetoothEnabled; }

    static void setWebGLDraftExtensionsEnabled(bool isEnabled) { isWebGLDraftExtensionsEnabled = isEnabled; }
    static bool webGLDraftExtensionsEnabled() { return isWebGLDraftExtensionsEnabled; }

    static void setWebGLImageChromiumEnabled(bool isEnabled) { isWebGLImageChromiumEnabled = isEnabled; }
    static bool webGLImageChromiumEnabled() { return isWebGLImageChromiumEnabled; }

    static void setWebVREnabled(bool isEnabled) { isWebVREnabled = isEnabled; }
    static bool webVREnabled() { return isWebVREnabled && isGeometryInterfacesEnabled; }

    static void setWebVTTRegionsEnabled(bool isEnabled) { isWebVTTRegionsEnabled = isEnabled; }
    static bool webVTTRegionsEnabled() { return isWebVTTRegionsEnabled && isMediaEnabled; }

    static void setV8IdleTasksEnabled(bool isEnabled) { isV8IdleTasksEnabled = isEnabled; }
    static bool v8IdleTasksEnabled() { return isV8IdleTasksEnabled; }

    static void setXSLTEnabled(bool isEnabled) { isXSLTEnabled = isEnabled; }
    static bool xsltEnabled() { return isXSLTEnabled; }

    static void setSVG1DOMEnabled(bool isEnabled) { isSVG1DOMEnabled = isEnabled; }
    static bool svg1DOMEnabled() { return isSVG1DOMEnabled; }

    static void setScrollCustomizationEnabled(bool isEnabled) { isScrollCustomizationEnabled = isEnabled; }
    static bool scrollCustomizationEnabled() { return isScrollCustomizationEnabled; }

    static void setScrollRestorationEnabled(bool isEnabled) { isScrollRestorationEnabled = isEnabled; }
    static bool scrollRestorationEnabled() { return isScrollRestorationEnabled; }

    static void setCspCheckEnabled(bool isEnabled) { isCspCheckEnabled = isEnabled; }
    static bool cspCheckEnabled() { return isCspCheckEnabled; }

    static void setMemoryCacheEnabled(bool isEnabled) { isMemoryCacheEnabled = isEnabled; }
    static bool memoryCacheEnabled() { return isMemoryCacheEnabled; }

    static void setNavigationToNewWindowEnabled(bool isEnabled) { isNavigationToNewWindowEnabled = isEnabled; }
    static bool navigationToNewWindowEnabled() { return isNavigationToNewWindowEnabled; }
    
    static void setUpdataInOtherThreadEnabled(bool isEnabled) { isUpdataInOtherThreadEnabled = isEnabled; }
    static bool updataInOtherThreadEnabled() { return isUpdataInOtherThreadEnabled; }
    
    static void setNpapiPluginsEnabled(bool isEnabled) { isNpapiPluginsEnabled = isEnabled; }
    static bool npapiPluginsEnabled() { return isNpapiPluginsEnabled; }

    static void setHeadlessEnabled(bool isEnabled) { isHeadlessEnabled = isEnabled; }
    static bool headlessEnabled() { return isHeadlessEnabled; }

    static void setFreshLocalFileEnabled(bool isEnabled) { isFreshLocalFileEnabled = isEnabled; }
    static bool freshLocalFileEnabled() { return isFreshLocalFileEnabled; }

    static void setAlwaysIsNotSolideColorEnabled(bool isEnabled) { isAlwaysIsNotSolideColor = isEnabled; }
    static bool alwaysIsNotSolideColorEnabled() { return isAlwaysIsNotSolideColor; }

    static void setDrawDirtyDebugLineEnabled(bool isEnabled) { isDrawDirtyDebugLineEnabled = isEnabled; }
    static bool drawDirtyDebugLineEnabled() { return isDrawDirtyDebugLineEnabled; }

    static void setDrawTileLineEnabled(bool isEnabled) { isDrawTileLineEnabled = isEnabled; }
    static bool drawTileLineEnabled() { return isDrawTileLineEnabled; }

private:
    RuntimeEnabledFeatures() { }

    static bool isAppBannerEnabled;
    static bool isAlwaysUseComplexTextEnabled;
    static bool isApplicationCacheEnabled;
    static bool isAudioOutputDevicesEnabled;
    static bool isAudioVideoTracksEnabled;
    static bool isAuthorShadowDOMForAnyElementEnabled;
    static bool isBackgroundSyncEnabled;
    static bool isBackgroundSyncV2Enabled;
    static bool isBleedingEdgeFastPathsEnabled;
    static bool isBlinkSchedulerEnabled;
    static bool isClientHintsEnabled;
    static bool isColumnFillEnabled;
    static bool isCompositedSelectionUpdateEnabled;
    static bool isCompositorWorkerEnabled;
    static bool isCompositorAnimationTimelinesEnabled;
    static bool isContextMenuEnabled;
    static bool isCreateShadowRootWithParameterEnabled;
    static bool isCredentialManagerEnabled;
    static bool isCSS3TextEnabled;
    static bool isCSS3TextDecorationsEnabled;
    static bool isCSSAttributeCaseSensitivityEnabled;
    static bool isCSSCompositingEnabled;
    static bool isCSSFontSizeAdjustEnabled;
    static bool isCSSGridLayoutEnabled;
    static bool isCSSIndependentTransformPropertiesEnabled;
    static bool isCSSMaskSourceTypeEnabled;
    static bool isCSSMotionPathEnabled;
    static bool isCSSOMSmoothScrollEnabled;
    static bool isCSSScrollBlocksOnEnabled;
    static bool isCSSTouchActionPanDirectionsEnabled;
    static bool isCSSViewportEnabled;
    static bool isCSSScrollSnapPointsEnabled;
    static bool isCustomSchemeHandlerEnabled;
    static bool isDatabaseEnabled;
    static bool isDecodeToYUVEnabled;
    static bool isDeviceLightEnabled;
    static bool isDisplayList2dCanvasEnabled;
    static bool isDOMConvenienceAPIEnabled;
    static bool isForceDisplayList2dCanvasEnabled;
    static bool isEncryptedMediaEnabled;
    static bool isEnumerateDevicesEnabled;
    static bool isExecCommandInJavaScriptEnabled;
    static bool isComputedAccessibilityInfoEnabled;
    static bool isExperimentalCanvasFeaturesEnabled;
    static bool isExperimentalContentSecurityPolicyFeaturesEnabled;
    static bool isFastMobileScrollingEnabled;
    static bool isFileAPIBlobCloseEnabled;
    static bool isFileSystemEnabled;
    static bool isFormDataNewMethodsEnabled;
    static bool isFullscreenUnprefixedEnabled;
    static bool isFrameTimingSupportEnabled;
    static bool isGeofencingEnabled;
    static bool isGeometryInterfacesEnabled;
    static bool isGlobalCacheStorageEnabled;
    static bool isImageColorProfilesEnabled;
    static bool isImageRenderingPixelatedEnabled;
    static bool isIndexedDBExperimentalEnabled;
    static bool isInputDeviceEnabled;
    static bool isInputModeAttributeEnabled;
    static bool isKeyboardEventCodeEnabled;
    static bool isKeyboardEventKeyEnabled;
    static bool isLangAttributeAwareFormControlUIEnabled;
    static bool isLinkPreconnectEnabled;
    static bool isLinkPreloadEnabled;
    static bool isLinkHeaderEnabled;
    static bool isFractionalScrollOffsetsEnabled;
    static bool isMediaEnabled;
    static bool isMediaCaptureEnabled;
    static bool isMediaControllerEnabled;
    static bool isMediaSessionEnabled;
    static bool isMediaSourceEnabled;
    static bool isMediaSourceExperimentalEnabled;
    static bool isMediaStreamSpeechEnabled;
    static bool isMemoryInfoInWorkersEnabled;
    static bool isNavigatorConnectEnabled;
    static bool isNavigatorContentUtilsEnabled;
    static bool isNetworkInformationEnabled;
    static bool isNotificationConstructorEnabled;
    static bool isNotificationExperimentalEnabled;
    static bool isNotificationsEnabled;
    static bool isOrientationEventEnabled;
    static bool isOverlayFullscreenVideoEnabled;
    static bool isOverlayScrollbarsEnabled;
    static bool isPagePopupEnabled;
    static bool isPathOpsSVGClippingEnabled;
    static bool isPermissionsEnabled;
    static bool isPointerEventEnabled;
    static bool isPreciseMemoryInfoEnabled;
    static bool isPrefixedEncryptedMediaEnabled;
    static bool isPrefixedVideoFullscreenEnabled;
    static bool isPresentationEnabled;
    static bool isPromiseRejectionEventEnabled;
    static bool isPushMessagingEnabled;
    static bool isPushMessagingDataEnabled;
    static bool isQuotaPromiseEnabled;
    static bool isReducedReferrerGranularityEnabled;
    static bool isRequestAutocompleteEnabled;
    static bool isSandboxBlocksModalsEnabled;
    static bool isScreenOrientationEnabled;
    static bool isScriptedSpeechEnabled;
    static bool isScrollTopLeftInteropEnabled;
    static bool isSelectionForComposedTreeEnabled;
    static bool isServiceWorkerCacheAddAllEnabled;
    static bool isServiceWorkerClientAttributesEnabled;
    static bool isServiceWorkerNotificationsEnabled;
    static bool isServiceWorkerPerformanceTimelineEnabled;
    static bool isShadowRootDelegatesFocusEnabled;
    static bool isSharedArrayBufferEnabled;
    static bool isSharedWorkerEnabled;
    static bool isSlimmingPaintEnabled;
    static bool isSlimmingPaintCompositorLayerizationEnabled;
    static bool isSlimmingPaintUnderInvalidationCheckingEnabled;
    static bool isSlimmingPaintStrictCullRectClippingEnabled;
    static bool isStaleWhileRevalidateCacheControlEnabled;
    static bool isExperimentalStreamEnabled;
    static bool isSuboriginsEnabled;
    static bool isTextBlobEnabled;
    static bool isThreadedParserDataReceiverEnabled;
    static bool isTouchEnabled;
    static bool isTouchIconLoadingEnabled;
    static bool isUnsafeES3APIsEnabled;
    static bool isUnsandboxedAuxiliaryEnabled;
    static bool isUserSelectAllEnabled;
    static bool isWebAnimationsAPIEnabled;
    static bool isWebAnimationsSVGEnabled;
#if ENABLE(WEB_AUDIO)
    static bool isWebAudioEnabled;
#endif // ENABLE(WEB_AUDIO)
    static bool isWebBluetoothEnabled;
    static bool isWebGLDraftExtensionsEnabled;
    static bool isWebGLImageChromiumEnabled;
    static bool isWebVREnabled;
    static bool isWebVTTRegionsEnabled;
    static bool isV8IdleTasksEnabled;
    static bool isXSLTEnabled;
    static bool isSVG1DOMEnabled;
    static bool isScrollCustomizationEnabled;
    static bool isScrollRestorationEnabled;
    static bool isCspCheckEnabled;
    static bool isMemoryCacheEnabled;
    static bool isNavigationToNewWindowEnabled;
    static bool isUpdataInOtherThreadEnabled;
    static bool isNpapiPluginsEnabled;
    static bool isHeadlessEnabled;
    static bool isFreshLocalFileEnabled;
    static bool isAlwaysIsNotSolideColor;
    static bool isDrawDirtyDebugLineEnabled;
    static bool isDrawTileLineEnabled;
};

} // namespace blink

#endif // RuntimeEnabledFeatures_h
