// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/RuntimeEnabledFeatures.h"

#include "wtf/Assertions.h"
#include "wtf/text/WTFString.h"

namespace blink {

void RuntimeEnabledFeatures::setStableFeaturesEnabled(bool enable)
{
    setAppBannerEnabled(enable);
    setApplicationCacheEnabled(enable);
    setAudioVideoTracksEnabled(enable);
    setCSSCompositingEnabled(enable);
    setDatabaseEnabled(enable);
    setDisplayList2dCanvasEnabled(enable);
    setEncryptedMediaEnabled(enable);
    setFileSystemEnabled(enable);
    setGlobalCacheStorageEnabled(enable);
    setImageRenderingPixelatedEnabled(enable);
    setMediaEnabled(enable);
    setMediaSourceEnabled(enable);
    setNetworkInformationEnabled(enable);
    setNotificationConstructorEnabled(enable);
    setNotificationsEnabled(enable);
    setPagePopupEnabled(enable);
    setPermissionsEnabled(enable);
    setPrefixedEncryptedMediaEnabled(enable);
    setPrefixedVideoFullscreenEnabled(enable);
    setPushMessagingEnabled(enable);
    setScreenOrientationEnabled(enable);
    setScriptedSpeechEnabled(enable);
    setSelectionForComposedTreeEnabled(enable);
    setServiceWorkerClientAttributesEnabled(enable);
    setServiceWorkerNotificationsEnabled(enable);
    setSharedWorkerEnabled(enable);
    setSlimmingPaintEnabled(enable);
    setTouchEnabled(enable);
    setWebAudioEnabled(enable);
    setXSLTEnabled(enable);
    setSVG1DOMEnabled(enable);
}

void RuntimeEnabledFeatures::setExperimentalFeaturesEnabled(bool enable)
{
    setAudioOutputDevicesEnabled(enable);
    setBackgroundSyncEnabled(enable);
    setBackgroundSyncV2Enabled(enable);
    setClientHintsEnabled(enable);
    setColumnFillEnabled(enable);
    setCompositorWorkerEnabled(enable);
    setContextMenuEnabled(enable);
    setCreateShadowRootWithParameterEnabled(enable);
    setCSS3TextEnabled(enable);
    setCSS3TextDecorationsEnabled(enable);
    setCSSAttributeCaseSensitivityEnabled(enable);
    setCSSFontSizeAdjustEnabled(enable);
    setCSSGridLayoutEnabled(enable);
    setCSSIndependentTransformPropertiesEnabled(enable);
    setCSSMaskSourceTypeEnabled(enable);
    setCSSMotionPathEnabled(enable);
    setCSSOMSmoothScrollEnabled(enable);
    setCSSScrollBlocksOnEnabled(enable);
    setCSSTouchActionPanDirectionsEnabled(enable);
    setCSSViewportEnabled(enable);
    setCustomSchemeHandlerEnabled(enable);
    setDecodeToYUVEnabled(enable);
    setDeviceLightEnabled(enable);
    setDOMConvenienceAPIEnabled(enable);
    setEnumerateDevicesEnabled(enable);
    setComputedAccessibilityInfoEnabled(enable);
    setExperimentalContentSecurityPolicyFeaturesEnabled(enable);
    setFileAPIBlobCloseEnabled(enable);
    setFormDataNewMethodsEnabled(enable);
    setFrameTimingSupportEnabled(enable);
    setGeofencingEnabled(enable);
    setIndexedDBExperimentalEnabled(enable);
    setInputDeviceEnabled(enable);
    setInputModeAttributeEnabled(enable);
    setKeyboardEventCodeEnabled(enable);
    setLinkPreconnectEnabled(enable);
    setLinkPreloadEnabled(enable);
    setLinkHeaderEnabled(enable);
    setFractionalScrollOffsetsEnabled(enable);
    setMediaControllerEnabled(enable);
    setMediaSourceExperimentalEnabled(enable);
    setMediaStreamSpeechEnabled(enable);
    setMemoryInfoInWorkersEnabled(enable);
    setNavigatorConnectEnabled(enable);
    setPathOpsSVGClippingEnabled(enable);
    setPresentationEnabled(enable);
    setPromiseRejectionEventEnabled(enable);
    setQuotaPromiseEnabled(enable);
    setSandboxBlocksModalsEnabled(enable);
    setServiceWorkerCacheAddAllEnabled(enable);
    setServiceWorkerPerformanceTimelineEnabled(enable);
    setShadowRootDelegatesFocusEnabled(enable);
    setSharedArrayBufferEnabled(enable);
    setExperimentalStreamEnabled(enable);
    setSuboriginsEnabled(enable);
    setUnsandboxedAuxiliaryEnabled(enable);
    setUserSelectAllEnabled(enable);
    setWebAnimationsAPIEnabled(enable);
    setWebAnimationsSVGEnabled(enable);
    setWebGLDraftExtensionsEnabled(enable);
    setWebVTTRegionsEnabled(enable);
    setScrollRestorationEnabled(enable);
}

void RuntimeEnabledFeatures::setTestFeaturesEnabled(bool enable)
{
    setCredentialManagerEnabled(enable);
    setCSSScrollSnapPointsEnabled(enable);
    setExecCommandInJavaScriptEnabled(enable);
    setExperimentalCanvasFeaturesEnabled(enable);
    setFullscreenUnprefixedEnabled(enable);
    setGeometryInterfacesEnabled(enable);
    setKeyboardEventKeyEnabled(enable);
    setNotificationExperimentalEnabled(enable);
    setPushMessagingDataEnabled(enable);
    setRequestAutocompleteEnabled(enable);
}


void RuntimeEnabledFeatures::setFeatureEnabledFromString(const String& name, bool isEnabled)
{
    if (equalIgnoringCase(name, "AppBanner")) {
        setAppBannerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "AlwaysUseComplexText")) {
        setAlwaysUseComplexTextEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ApplicationCache")) {
        setApplicationCacheEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "AudioOutputDevices")) {
        setAudioOutputDevicesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "AudioVideoTracks")) {
        setAudioVideoTracksEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "AuthorShadowDOMForAnyElement")) {
        setAuthorShadowDOMForAnyElementEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "BackgroundSync")) {
        setBackgroundSyncEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "BackgroundSyncV2")) {
        setBackgroundSyncV2Enabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "BleedingEdgeFastPaths")) {
        setBleedingEdgeFastPathsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "BlinkScheduler")) {
        setBlinkSchedulerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ClientHints")) {
        setClientHintsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ColumnFill")) {
        setColumnFillEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CompositedSelectionUpdate")) {
        setCompositedSelectionUpdateEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CompositorWorker")) {
        setCompositorWorkerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CompositorAnimationTimelines")) {
        setCompositorAnimationTimelinesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ContextMenu")) {
        setContextMenuEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CreateShadowRootWithParameter")) {
        setCreateShadowRootWithParameterEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CredentialManager")) {
        setCredentialManagerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSS3Text")) {
        setCSS3TextEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSS3TextDecorations")) {
        setCSS3TextDecorationsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSAttributeCaseSensitivity")) {
        setCSSAttributeCaseSensitivityEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSCompositing")) {
        setCSSCompositingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSFontSizeAdjust")) {
        setCSSFontSizeAdjustEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSGridLayout")) {
        setCSSGridLayoutEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSIndependentTransformProperties")) {
        setCSSIndependentTransformPropertiesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSMaskSourceType")) {
        setCSSMaskSourceTypeEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSMotionPath")) {
        setCSSMotionPathEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSOMSmoothScroll")) {
        setCSSOMSmoothScrollEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSScrollBlocksOn")) {
        setCSSScrollBlocksOnEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSTouchActionPanDirections")) {
        setCSSTouchActionPanDirectionsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSViewport")) {
        setCSSViewportEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CSSScrollSnapPoints")) {
        setCSSScrollSnapPointsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "CustomSchemeHandler")) {
        setCustomSchemeHandlerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Database")) {
        setDatabaseEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "DecodeToYUV")) {
        setDecodeToYUVEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "DeviceLight")) {
        setDeviceLightEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "DisplayList2dCanvas")) {
        setDisplayList2dCanvasEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "DOMConvenienceAPI")) {
        setDOMConvenienceAPIEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ForceDisplayList2dCanvas")) {
        setForceDisplayList2dCanvasEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "EncryptedMedia")) {
        setEncryptedMediaEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "EnumerateDevices")) {
        setEnumerateDevicesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ExecCommandInJavaScript")) {
        setExecCommandInJavaScriptEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ComputedAccessibilityInfo")) {
        setComputedAccessibilityInfoEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ExperimentalCanvasFeatures")) {
        setExperimentalCanvasFeaturesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ExperimentalContentSecurityPolicyFeatures")) {
        setExperimentalContentSecurityPolicyFeaturesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FastMobileScrolling")) {
        setFastMobileScrollingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FileAPIBlobClose")) {
        setFileAPIBlobCloseEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FileSystem")) {
        setFileSystemEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FormDataNewMethods")) {
        setFormDataNewMethodsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FullscreenUnprefixed")) {
        setFullscreenUnprefixedEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FrameTimingSupport")) {
        setFrameTimingSupportEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Geofencing")) {
        setGeofencingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "GeometryInterfaces")) {
        setGeometryInterfacesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "GlobalCacheStorage")) {
        setGlobalCacheStorageEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ImageColorProfiles")) {
        setImageColorProfilesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ImageRenderingPixelated")) {
        setImageRenderingPixelatedEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "IndexedDBExperimental")) {
        setIndexedDBExperimentalEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "InputDevice")) {
        setInputDeviceEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "InputModeAttribute")) {
        setInputModeAttributeEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "KeyboardEventCode")) {
        setKeyboardEventCodeEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "KeyboardEventKey")) {
        setKeyboardEventKeyEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "LangAttributeAwareFormControlUI")) {
        setLangAttributeAwareFormControlUIEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "LinkPreconnect")) {
        setLinkPreconnectEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "LinkPreload")) {
        setLinkPreloadEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "LinkHeader")) {
        setLinkHeaderEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "FractionalScrollOffsets")) {
        setFractionalScrollOffsetsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Media")) {
        setMediaEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaCapture")) {
        setMediaCaptureEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaController")) {
        setMediaControllerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaSession")) {
        setMediaSessionEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaSource")) {
        setMediaSourceEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaSourceExperimental")) {
        setMediaSourceExperimentalEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MediaStreamSpeech")) {
        setMediaStreamSpeechEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "MemoryInfoInWorkers")) {
        setMemoryInfoInWorkersEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "NavigatorConnect")) {
        setNavigatorConnectEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "NavigatorContentUtils")) {
        setNavigatorContentUtilsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "NetworkInformation")) {
        setNetworkInformationEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "NotificationConstructor")) {
        setNotificationConstructorEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "NotificationExperimental")) {
        setNotificationExperimentalEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Notifications")) {
        setNotificationsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "OrientationEvent")) {
        setOrientationEventEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "OverlayFullscreenVideo")) {
        setOverlayFullscreenVideoEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "OverlayScrollbars")) {
        setOverlayScrollbarsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PagePopup")) {
        setPagePopupEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PathOpsSVGClipping")) {
        setPathOpsSVGClippingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Permissions")) {
        setPermissionsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PointerEvent")) {
        setPointerEventEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PreciseMemoryInfo")) {
        setPreciseMemoryInfoEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PrefixedEncryptedMedia")) {
        setPrefixedEncryptedMediaEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PrefixedVideoFullscreen")) {
        setPrefixedVideoFullscreenEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Presentation")) {
        setPresentationEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PromiseRejectionEvent")) {
        setPromiseRejectionEventEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PushMessaging")) {
        setPushMessagingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "PushMessagingData")) {
        setPushMessagingDataEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "QuotaPromise")) {
        setQuotaPromiseEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ReducedReferrerGranularity")) {
        setReducedReferrerGranularityEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "RequestAutocomplete")) {
        setRequestAutocompleteEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SandboxBlocksModals")) {
        setSandboxBlocksModalsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ScreenOrientation")) {
        setScreenOrientationEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ScriptedSpeech")) {
        setScriptedSpeechEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ScrollTopLeftInterop")) {
        setScrollTopLeftInteropEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SelectionForComposedTree")) {
        setSelectionForComposedTreeEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ServiceWorkerCacheAddAll")) {
        setServiceWorkerCacheAddAllEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ServiceWorkerClientAttributes")) {
        setServiceWorkerClientAttributesEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ServiceWorkerNotifications")) {
        setServiceWorkerNotificationsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ServiceWorkerPerformanceTimeline")) {
        setServiceWorkerPerformanceTimelineEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ShadowRootDelegatesFocus")) {
        setShadowRootDelegatesFocusEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SharedArrayBuffer")) {
        setSharedArrayBufferEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SharedWorker")) {
        setSharedWorkerEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SlimmingPaint")) {
        setSlimmingPaintEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SlimmingPaintCompositorLayerization")) {
        setSlimmingPaintCompositorLayerizationEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SlimmingPaintUnderInvalidationChecking")) {
        setSlimmingPaintUnderInvalidationCheckingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SlimmingPaintStrictCullRectClipping")) {
        setSlimmingPaintStrictCullRectClippingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "StaleWhileRevalidateCacheControl")) {
        setStaleWhileRevalidateCacheControlEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ExperimentalStream")) {
        setExperimentalStreamEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Suborigins")) {
        setSuboriginsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "TextBlob")) {
        setTextBlobEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ThreadedParserDataReceiver")) {
        setThreadedParserDataReceiverEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "Touch")) {
        setTouchEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "TouchIconLoading")) {
        setTouchIconLoadingEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "UnsafeES3APIs")) {
        setUnsafeES3APIsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "UnsandboxedAuxiliary")) {
        setUnsandboxedAuxiliaryEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "UserSelectAll")) {
        setUserSelectAllEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebAnimationsAPI")) {
        setWebAnimationsAPIEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebAnimationsSVG")) {
        setWebAnimationsSVGEnabled(isEnabled);
        return;
    }
#if ENABLE(WEB_AUDIO)
    if (equalIgnoringCase(name, "WebAudio")) {
        setWebAudioEnabled(isEnabled);
        return;
    }
#endif // ENABLE(WEB_AUDIO)
    if (equalIgnoringCase(name, "WebBluetooth")) {
        setWebBluetoothEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebGLDraftExtensions")) {
        setWebGLDraftExtensionsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebGLImageChromium")) {
        setWebGLImageChromiumEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebVR")) {
        setWebVREnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "WebVTTRegions")) {
        setWebVTTRegionsEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "V8IdleTasks")) {
        setV8IdleTasksEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "XSLT")) {
        setXSLTEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "SVG1DOM")) {
        setSVG1DOMEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ScrollCustomization")) {
        setScrollCustomizationEnabled(isEnabled);
        return;
    }
    if (equalIgnoringCase(name, "ScrollRestoration")) {
        setScrollRestorationEnabled(isEnabled);
        return;
    }
    WTF_LOG_ERROR("RuntimeEnabledFeature not recognized: %s", name.ascii().data());
}

bool RuntimeEnabledFeatures::isAppBannerEnabled = true;
bool RuntimeEnabledFeatures::isAlwaysUseComplexTextEnabled = false;
bool RuntimeEnabledFeatures::isApplicationCacheEnabled = true;
bool RuntimeEnabledFeatures::isAudioOutputDevicesEnabled = false;
bool RuntimeEnabledFeatures::isAudioVideoTracksEnabled = true;
bool RuntimeEnabledFeatures::isAuthorShadowDOMForAnyElementEnabled = false;
bool RuntimeEnabledFeatures::isBackgroundSyncEnabled = false;
bool RuntimeEnabledFeatures::isBackgroundSyncV2Enabled = false;
bool RuntimeEnabledFeatures::isBleedingEdgeFastPathsEnabled = false;
bool RuntimeEnabledFeatures::isBlinkSchedulerEnabled = false;
bool RuntimeEnabledFeatures::isClientHintsEnabled = false;
bool RuntimeEnabledFeatures::isColumnFillEnabled = false;
bool RuntimeEnabledFeatures::isCompositedSelectionUpdateEnabled = false;
bool RuntimeEnabledFeatures::isCompositorWorkerEnabled = false;
bool RuntimeEnabledFeatures::isCompositorAnimationTimelinesEnabled = false;
bool RuntimeEnabledFeatures::isContextMenuEnabled = false;
bool RuntimeEnabledFeatures::isCreateShadowRootWithParameterEnabled = false;
bool RuntimeEnabledFeatures::isCredentialManagerEnabled = false;
bool RuntimeEnabledFeatures::isCSS3TextEnabled = false;
bool RuntimeEnabledFeatures::isCSS3TextDecorationsEnabled = false;
bool RuntimeEnabledFeatures::isCSSAttributeCaseSensitivityEnabled = false;
bool RuntimeEnabledFeatures::isCSSCompositingEnabled = true;
bool RuntimeEnabledFeatures::isCSSFontSizeAdjustEnabled = false;
bool RuntimeEnabledFeatures::isCSSGridLayoutEnabled = false;
bool RuntimeEnabledFeatures::isCSSIndependentTransformPropertiesEnabled = false;
bool RuntimeEnabledFeatures::isCSSMaskSourceTypeEnabled = false;
bool RuntimeEnabledFeatures::isCSSMotionPathEnabled = false;
bool RuntimeEnabledFeatures::isCSSOMSmoothScrollEnabled = false;
bool RuntimeEnabledFeatures::isCSSScrollBlocksOnEnabled = false;
bool RuntimeEnabledFeatures::isCSSTouchActionPanDirectionsEnabled = false;
bool RuntimeEnabledFeatures::isCSSViewportEnabled = false;
bool RuntimeEnabledFeatures::isCSSScrollSnapPointsEnabled = false;
bool RuntimeEnabledFeatures::isCustomSchemeHandlerEnabled = false;
bool RuntimeEnabledFeatures::isDatabaseEnabled = true;
bool RuntimeEnabledFeatures::isDecodeToYUVEnabled = false;
bool RuntimeEnabledFeatures::isDeviceLightEnabled = false;
bool RuntimeEnabledFeatures::isDisplayList2dCanvasEnabled = true;
bool RuntimeEnabledFeatures::isDOMConvenienceAPIEnabled = false;
bool RuntimeEnabledFeatures::isForceDisplayList2dCanvasEnabled = false;
bool RuntimeEnabledFeatures::isEncryptedMediaEnabled = true;
bool RuntimeEnabledFeatures::isEnumerateDevicesEnabled = false;
bool RuntimeEnabledFeatures::isExecCommandInJavaScriptEnabled = false;
bool RuntimeEnabledFeatures::isComputedAccessibilityInfoEnabled = false;
bool RuntimeEnabledFeatures::isExperimentalCanvasFeaturesEnabled = false;
bool RuntimeEnabledFeatures::isExperimentalContentSecurityPolicyFeaturesEnabled = false;
bool RuntimeEnabledFeatures::isFastMobileScrollingEnabled = false;
bool RuntimeEnabledFeatures::isFileAPIBlobCloseEnabled = false;
bool RuntimeEnabledFeatures::isFileSystemEnabled = true;
bool RuntimeEnabledFeatures::isFormDataNewMethodsEnabled = false;
bool RuntimeEnabledFeatures::isFullscreenUnprefixedEnabled = false;
bool RuntimeEnabledFeatures::isFrameTimingSupportEnabled = false;
bool RuntimeEnabledFeatures::isGeofencingEnabled = false;
bool RuntimeEnabledFeatures::isGeometryInterfacesEnabled = false;
bool RuntimeEnabledFeatures::isGlobalCacheStorageEnabled = true;
bool RuntimeEnabledFeatures::isImageColorProfilesEnabled = false;
bool RuntimeEnabledFeatures::isImageRenderingPixelatedEnabled = true;
bool RuntimeEnabledFeatures::isIndexedDBExperimentalEnabled = false;
bool RuntimeEnabledFeatures::isInputDeviceEnabled = false;
bool RuntimeEnabledFeatures::isInputModeAttributeEnabled = false;
bool RuntimeEnabledFeatures::isKeyboardEventCodeEnabled = false;
bool RuntimeEnabledFeatures::isKeyboardEventKeyEnabled = false;
bool RuntimeEnabledFeatures::isLangAttributeAwareFormControlUIEnabled = false;
bool RuntimeEnabledFeatures::isLinkPreconnectEnabled = false;
bool RuntimeEnabledFeatures::isLinkPreloadEnabled = false;
bool RuntimeEnabledFeatures::isLinkHeaderEnabled = false;
bool RuntimeEnabledFeatures::isFractionalScrollOffsetsEnabled = false;
bool RuntimeEnabledFeatures::isMediaEnabled = true;
bool RuntimeEnabledFeatures::isMediaCaptureEnabled = false;
bool RuntimeEnabledFeatures::isMediaControllerEnabled = false;
bool RuntimeEnabledFeatures::isMediaSessionEnabled = false;
bool RuntimeEnabledFeatures::isMediaSourceEnabled = true;
bool RuntimeEnabledFeatures::isMediaSourceExperimentalEnabled = false;
bool RuntimeEnabledFeatures::isMediaStreamSpeechEnabled = false;
bool RuntimeEnabledFeatures::isMemoryInfoInWorkersEnabled = false;
bool RuntimeEnabledFeatures::isNavigatorConnectEnabled = false;
bool RuntimeEnabledFeatures::isNavigatorContentUtilsEnabled = false;
bool RuntimeEnabledFeatures::isNetworkInformationEnabled = true;
bool RuntimeEnabledFeatures::isNotificationConstructorEnabled = true;
bool RuntimeEnabledFeatures::isNotificationExperimentalEnabled = false;
bool RuntimeEnabledFeatures::isNotificationsEnabled = true;
bool RuntimeEnabledFeatures::isOrientationEventEnabled = false;
bool RuntimeEnabledFeatures::isOverlayFullscreenVideoEnabled = false;
bool RuntimeEnabledFeatures::isOverlayScrollbarsEnabled = false;
bool RuntimeEnabledFeatures::isPagePopupEnabled = true;
bool RuntimeEnabledFeatures::isPathOpsSVGClippingEnabled = false;
bool RuntimeEnabledFeatures::isPermissionsEnabled = true;
bool RuntimeEnabledFeatures::isPointerEventEnabled = false;
bool RuntimeEnabledFeatures::isPreciseMemoryInfoEnabled = false;
bool RuntimeEnabledFeatures::isPrefixedEncryptedMediaEnabled = true;
bool RuntimeEnabledFeatures::isPrefixedVideoFullscreenEnabled = true;
bool RuntimeEnabledFeatures::isPresentationEnabled = false;
bool RuntimeEnabledFeatures::isPromiseRejectionEventEnabled = false;
bool RuntimeEnabledFeatures::isPushMessagingEnabled = true;
bool RuntimeEnabledFeatures::isPushMessagingDataEnabled = false;
bool RuntimeEnabledFeatures::isQuotaPromiseEnabled = false;
bool RuntimeEnabledFeatures::isReducedReferrerGranularityEnabled = false;
bool RuntimeEnabledFeatures::isRequestAutocompleteEnabled = false;
bool RuntimeEnabledFeatures::isSandboxBlocksModalsEnabled = false;
bool RuntimeEnabledFeatures::isScreenOrientationEnabled = true;
bool RuntimeEnabledFeatures::isScriptedSpeechEnabled = true;
bool RuntimeEnabledFeatures::isScrollTopLeftInteropEnabled = false;
bool RuntimeEnabledFeatures::isSelectionForComposedTreeEnabled = true;
bool RuntimeEnabledFeatures::isServiceWorkerCacheAddAllEnabled = false;
bool RuntimeEnabledFeatures::isServiceWorkerClientAttributesEnabled = true;
bool RuntimeEnabledFeatures::isServiceWorkerNotificationsEnabled = true;
bool RuntimeEnabledFeatures::isServiceWorkerPerformanceTimelineEnabled = false;
bool RuntimeEnabledFeatures::isShadowRootDelegatesFocusEnabled = false;
bool RuntimeEnabledFeatures::isSharedArrayBufferEnabled = false;
bool RuntimeEnabledFeatures::isSharedWorkerEnabled = true;
bool RuntimeEnabledFeatures::isSlimmingPaintEnabled = true;
bool RuntimeEnabledFeatures::isSlimmingPaintCompositorLayerizationEnabled = false;
bool RuntimeEnabledFeatures::isSlimmingPaintUnderInvalidationCheckingEnabled = false;
bool RuntimeEnabledFeatures::isSlimmingPaintStrictCullRectClippingEnabled = false;
bool RuntimeEnabledFeatures::isStaleWhileRevalidateCacheControlEnabled = false;
bool RuntimeEnabledFeatures::isExperimentalStreamEnabled = false;
bool RuntimeEnabledFeatures::isSuboriginsEnabled = false;
bool RuntimeEnabledFeatures::isTextBlobEnabled = false;
bool RuntimeEnabledFeatures::isThreadedParserDataReceiverEnabled = false;
bool RuntimeEnabledFeatures::isTouchEnabled = true;
bool RuntimeEnabledFeatures::isTouchIconLoadingEnabled = false;
bool RuntimeEnabledFeatures::isUnsafeES3APIsEnabled = false;
bool RuntimeEnabledFeatures::isUnsandboxedAuxiliaryEnabled = false;
bool RuntimeEnabledFeatures::isUserSelectAllEnabled = false;
bool RuntimeEnabledFeatures::isWebAnimationsAPIEnabled = false;
bool RuntimeEnabledFeatures::isWebAnimationsSVGEnabled = false;
#if ENABLE(WEB_AUDIO)
bool RuntimeEnabledFeatures::isWebAudioEnabled = true;
#endif // ENABLE(WEB_AUDIO)
bool RuntimeEnabledFeatures::isWebBluetoothEnabled = false;
bool RuntimeEnabledFeatures::isWebGLDraftExtensionsEnabled = false;
bool RuntimeEnabledFeatures::isWebGLImageChromiumEnabled = false;
bool RuntimeEnabledFeatures::isWebVREnabled = false;
bool RuntimeEnabledFeatures::isWebVTTRegionsEnabled = false;
bool RuntimeEnabledFeatures::isV8IdleTasksEnabled = false;
bool RuntimeEnabledFeatures::isXSLTEnabled = true;
bool RuntimeEnabledFeatures::isSVG1DOMEnabled = true;
bool RuntimeEnabledFeatures::isScrollCustomizationEnabled = false;
bool RuntimeEnabledFeatures::isScrollRestorationEnabled = false;

bool RuntimeEnabledFeatures::isCspCheckEnabled = false;
bool RuntimeEnabledFeatures::isMemoryCacheEnabled = false;
bool RuntimeEnabledFeatures::isNavigationToNewWindowEnabled = false;
bool RuntimeEnabledFeatures::isUpdataInOtherThreadEnabled = false;
bool RuntimeEnabledFeatures::isNpapiPluginsEnabled = false;
bool RuntimeEnabledFeatures::isHeadlessEnabled = false;
bool RuntimeEnabledFeatures::isFreshLocalFileEnabled = false;
bool RuntimeEnabledFeatures::isAlwaysIsNotSolideColor = false;
bool RuntimeEnabledFeatures::isDrawDirtyDebugLineEnabled = false;
bool RuntimeEnabledFeatures::isDrawTileLineEnabled = false;

} // namespace blink
