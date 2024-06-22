// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InternalRuntimeFlags_h
#define InternalRuntimeFlags_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class InternalRuntimeFlags : public GarbageCollected<InternalRuntimeFlags>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static InternalRuntimeFlags* create()
    {
        return new InternalRuntimeFlags;
    }

    bool appBannerEnabled() { return RuntimeEnabledFeatures::appBannerEnabled(); }
    bool alwaysUseComplexTextEnabled() { return RuntimeEnabledFeatures::alwaysUseComplexTextEnabled(); }
    bool applicationCacheEnabled() { return RuntimeEnabledFeatures::applicationCacheEnabled(); }
    bool audioOutputDevicesEnabled() { return RuntimeEnabledFeatures::audioOutputDevicesEnabled(); }
    bool audioVideoTracksEnabled() { return RuntimeEnabledFeatures::audioVideoTracksEnabled(); }
    bool authorShadowDOMForAnyElementEnabled() { return RuntimeEnabledFeatures::authorShadowDOMForAnyElementEnabled(); }
    bool backgroundSyncEnabled() { return RuntimeEnabledFeatures::backgroundSyncEnabled(); }
    bool backgroundSyncV2Enabled() { return RuntimeEnabledFeatures::backgroundSyncV2Enabled(); }
    bool bleedingEdgeFastPathsEnabled() { return RuntimeEnabledFeatures::bleedingEdgeFastPathsEnabled(); }
    bool blinkSchedulerEnabled() { return RuntimeEnabledFeatures::blinkSchedulerEnabled(); }
    bool clientHintsEnabled() { return RuntimeEnabledFeatures::clientHintsEnabled(); }
    bool columnFillEnabled() { return RuntimeEnabledFeatures::columnFillEnabled(); }
    bool compositedSelectionUpdateEnabled() { return RuntimeEnabledFeatures::compositedSelectionUpdateEnabled(); }
    bool compositorWorkerEnabled() { return RuntimeEnabledFeatures::compositorWorkerEnabled(); }
    bool compositorAnimationTimelinesEnabled() { return RuntimeEnabledFeatures::compositorAnimationTimelinesEnabled(); }
    bool contextMenuEnabled() { return RuntimeEnabledFeatures::contextMenuEnabled(); }
    bool createShadowRootWithParameterEnabled() { return RuntimeEnabledFeatures::createShadowRootWithParameterEnabled(); }
    bool credentialManagerEnabled() { return RuntimeEnabledFeatures::credentialManagerEnabled(); }
    bool css3TextEnabled() { return RuntimeEnabledFeatures::css3TextEnabled(); }
    bool css3TextDecorationsEnabled() { return RuntimeEnabledFeatures::css3TextDecorationsEnabled(); }
    bool cssAttributeCaseSensitivityEnabled() { return RuntimeEnabledFeatures::cssAttributeCaseSensitivityEnabled(); }
    bool cssCompositingEnabled() { return RuntimeEnabledFeatures::cssCompositingEnabled(); }
    bool cssFontSizeAdjustEnabled() { return RuntimeEnabledFeatures::cssFontSizeAdjustEnabled(); }
    bool cssGridLayoutEnabled() { return RuntimeEnabledFeatures::cssGridLayoutEnabled(); }
    bool cssIndependentTransformPropertiesEnabled() { return RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled(); }
    bool cssMaskSourceTypeEnabled() { return RuntimeEnabledFeatures::cssMaskSourceTypeEnabled(); }
    bool cssMotionPathEnabled() { return RuntimeEnabledFeatures::cssMotionPathEnabled(); }
    bool cssomSmoothScrollEnabled() { return RuntimeEnabledFeatures::cssomSmoothScrollEnabled(); }
    bool cssScrollBlocksOnEnabled() { return RuntimeEnabledFeatures::cssScrollBlocksOnEnabled(); }
    bool cssTouchActionPanDirectionsEnabled() { return RuntimeEnabledFeatures::cssTouchActionPanDirectionsEnabled(); }
    bool cssViewportEnabled() { return RuntimeEnabledFeatures::cssViewportEnabled(); }
    bool cssScrollSnapPointsEnabled() { return RuntimeEnabledFeatures::cssScrollSnapPointsEnabled(); }
    bool customSchemeHandlerEnabled() { return RuntimeEnabledFeatures::customSchemeHandlerEnabled(); }
    bool databaseEnabled() { return RuntimeEnabledFeatures::databaseEnabled(); }
    bool decodeToYUVEnabled() { return RuntimeEnabledFeatures::decodeToYUVEnabled(); }
    bool deviceLightEnabled() { return RuntimeEnabledFeatures::deviceLightEnabled(); }
    bool displayList2dCanvasEnabled() { return RuntimeEnabledFeatures::displayList2dCanvasEnabled(); }
    bool dOMConvenienceAPIEnabled() { return RuntimeEnabledFeatures::dOMConvenienceAPIEnabled(); }
    bool forceDisplayList2dCanvasEnabled() { return RuntimeEnabledFeatures::forceDisplayList2dCanvasEnabled(); }
    bool encryptedMediaEnabled() { return RuntimeEnabledFeatures::encryptedMediaEnabled(); }
    bool enumerateDevicesEnabled() { return RuntimeEnabledFeatures::enumerateDevicesEnabled(); }
    bool execCommandInJavaScriptEnabled() { return RuntimeEnabledFeatures::execCommandInJavaScriptEnabled(); }
    bool computedAccessibilityInfoEnabled() { return RuntimeEnabledFeatures::computedAccessibilityInfoEnabled(); }
    bool experimentalCanvasFeaturesEnabled() { return RuntimeEnabledFeatures::experimentalCanvasFeaturesEnabled(); }
    bool experimentalContentSecurityPolicyFeaturesEnabled() { return RuntimeEnabledFeatures::experimentalContentSecurityPolicyFeaturesEnabled(); }
    bool fastMobileScrollingEnabled() { return RuntimeEnabledFeatures::fastMobileScrollingEnabled(); }
    bool fileAPIBlobCloseEnabled() { return RuntimeEnabledFeatures::fileAPIBlobCloseEnabled(); }
    bool fileSystemEnabled() { return RuntimeEnabledFeatures::fileSystemEnabled(); }
    bool formDataNewMethodsEnabled() { return RuntimeEnabledFeatures::formDataNewMethodsEnabled(); }
    bool fullscreenUnprefixedEnabled() { return RuntimeEnabledFeatures::fullscreenUnprefixedEnabled(); }
    bool frameTimingSupportEnabled() { return RuntimeEnabledFeatures::frameTimingSupportEnabled(); }
    bool geofencingEnabled() { return RuntimeEnabledFeatures::geofencingEnabled(); }
    bool geometryInterfacesEnabled() { return RuntimeEnabledFeatures::geometryInterfacesEnabled(); }
    bool globalCacheStorageEnabled() { return RuntimeEnabledFeatures::globalCacheStorageEnabled(); }
    bool imageColorProfilesEnabled() { return RuntimeEnabledFeatures::imageColorProfilesEnabled(); }
    bool imageRenderingPixelatedEnabled() { return RuntimeEnabledFeatures::imageRenderingPixelatedEnabled(); }
    bool indexedDBExperimentalEnabled() { return RuntimeEnabledFeatures::indexedDBExperimentalEnabled(); }
    bool inputDeviceEnabled() { return RuntimeEnabledFeatures::inputDeviceEnabled(); }
    bool inputModeAttributeEnabled() { return RuntimeEnabledFeatures::inputModeAttributeEnabled(); }
    bool keyboardEventCodeEnabled() { return RuntimeEnabledFeatures::keyboardEventCodeEnabled(); }
    bool keyboardEventKeyEnabled() { return RuntimeEnabledFeatures::keyboardEventKeyEnabled(); }
    bool langAttributeAwareFormControlUIEnabled() { return RuntimeEnabledFeatures::langAttributeAwareFormControlUIEnabled(); }
    bool linkPreconnectEnabled() { return RuntimeEnabledFeatures::linkPreconnectEnabled(); }
    bool linkPreloadEnabled() { return RuntimeEnabledFeatures::linkPreloadEnabled(); }
    bool linkHeaderEnabled() { return RuntimeEnabledFeatures::linkHeaderEnabled(); }
    bool fractionalScrollOffsetsEnabled() { return RuntimeEnabledFeatures::fractionalScrollOffsetsEnabled(); }
    bool mediaEnabled() { return RuntimeEnabledFeatures::mediaEnabled(); }
    bool mediaCaptureEnabled() { return RuntimeEnabledFeatures::mediaCaptureEnabled(); }
    bool mediaControllerEnabled() { return RuntimeEnabledFeatures::mediaControllerEnabled(); }
    bool mediaSessionEnabled() { return RuntimeEnabledFeatures::mediaSessionEnabled(); }
    bool mediaSourceEnabled() { return RuntimeEnabledFeatures::mediaSourceEnabled(); }
    bool mediaSourceExperimentalEnabled() { return RuntimeEnabledFeatures::mediaSourceExperimentalEnabled(); }
    bool mediaStreamSpeechEnabled() { return RuntimeEnabledFeatures::mediaStreamSpeechEnabled(); }
    bool memoryInfoInWorkersEnabled() { return RuntimeEnabledFeatures::memoryInfoInWorkersEnabled(); }
    bool navigatorConnectEnabled() { return RuntimeEnabledFeatures::navigatorConnectEnabled(); }
    bool navigatorContentUtilsEnabled() { return RuntimeEnabledFeatures::navigatorContentUtilsEnabled(); }
    bool networkInformationEnabled() { return RuntimeEnabledFeatures::networkInformationEnabled(); }
    bool notificationConstructorEnabled() { return RuntimeEnabledFeatures::notificationConstructorEnabled(); }
    bool notificationExperimentalEnabled() { return RuntimeEnabledFeatures::notificationExperimentalEnabled(); }
    bool notificationsEnabled() { return RuntimeEnabledFeatures::notificationsEnabled(); }
    bool orientationEventEnabled() { return RuntimeEnabledFeatures::orientationEventEnabled(); }
    bool overlayFullscreenVideoEnabled() { return RuntimeEnabledFeatures::overlayFullscreenVideoEnabled(); }
    bool overlayScrollbarsEnabled() { return RuntimeEnabledFeatures::overlayScrollbarsEnabled(); }
    bool pagePopupEnabled() { return RuntimeEnabledFeatures::pagePopupEnabled(); }
    bool pathOpsSVGClippingEnabled() { return RuntimeEnabledFeatures::pathOpsSVGClippingEnabled(); }
    bool permissionsEnabled() { return RuntimeEnabledFeatures::permissionsEnabled(); }
    bool pointerEventEnabled() { return RuntimeEnabledFeatures::pointerEventEnabled(); }
    bool preciseMemoryInfoEnabled() { return RuntimeEnabledFeatures::preciseMemoryInfoEnabled(); }
    bool prefixedEncryptedMediaEnabled() { return RuntimeEnabledFeatures::prefixedEncryptedMediaEnabled(); }
    bool prefixedVideoFullscreenEnabled() { return RuntimeEnabledFeatures::prefixedVideoFullscreenEnabled(); }
    bool presentationEnabled() { return RuntimeEnabledFeatures::presentationEnabled(); }
    bool promiseRejectionEventEnabled() { return RuntimeEnabledFeatures::promiseRejectionEventEnabled(); }
    bool pushMessagingEnabled() { return RuntimeEnabledFeatures::pushMessagingEnabled(); }
    bool pushMessagingDataEnabled() { return RuntimeEnabledFeatures::pushMessagingDataEnabled(); }
    bool quotaPromiseEnabled() { return RuntimeEnabledFeatures::quotaPromiseEnabled(); }
    bool reducedReferrerGranularityEnabled() { return RuntimeEnabledFeatures::reducedReferrerGranularityEnabled(); }
    bool requestAutocompleteEnabled() { return RuntimeEnabledFeatures::requestAutocompleteEnabled(); }
    bool sandboxBlocksModalsEnabled() { return RuntimeEnabledFeatures::sandboxBlocksModalsEnabled(); }
    bool screenOrientationEnabled() { return RuntimeEnabledFeatures::screenOrientationEnabled(); }
    bool scriptedSpeechEnabled() { return RuntimeEnabledFeatures::scriptedSpeechEnabled(); }
    bool scrollTopLeftInteropEnabled() { return RuntimeEnabledFeatures::scrollTopLeftInteropEnabled(); }
    bool selectionForComposedTreeEnabled() { return RuntimeEnabledFeatures::selectionForComposedTreeEnabled(); }
    bool serviceWorkerCacheAddAllEnabled() { return RuntimeEnabledFeatures::serviceWorkerCacheAddAllEnabled(); }
    bool serviceWorkerClientAttributesEnabled() { return RuntimeEnabledFeatures::serviceWorkerClientAttributesEnabled(); }
    bool serviceWorkerNotificationsEnabled() { return RuntimeEnabledFeatures::serviceWorkerNotificationsEnabled(); }
    bool serviceWorkerPerformanceTimelineEnabled() { return RuntimeEnabledFeatures::serviceWorkerPerformanceTimelineEnabled(); }
    bool shadowRootDelegatesFocusEnabled() { return RuntimeEnabledFeatures::shadowRootDelegatesFocusEnabled(); }
    bool sharedArrayBufferEnabled() { return RuntimeEnabledFeatures::sharedArrayBufferEnabled(); }
    bool sharedWorkerEnabled() { return RuntimeEnabledFeatures::sharedWorkerEnabled(); }
    bool slimmingPaintEnabled() { return RuntimeEnabledFeatures::slimmingPaintEnabled(); }
    bool slimmingPaintCompositorLayerizationEnabled() { return RuntimeEnabledFeatures::slimmingPaintCompositorLayerizationEnabled(); }
    bool slimmingPaintUnderInvalidationCheckingEnabled() { return RuntimeEnabledFeatures::slimmingPaintUnderInvalidationCheckingEnabled(); }
    bool slimmingPaintStrictCullRectClippingEnabled() { return RuntimeEnabledFeatures::slimmingPaintStrictCullRectClippingEnabled(); }
    bool staleWhileRevalidateCacheControlEnabled() { return RuntimeEnabledFeatures::staleWhileRevalidateCacheControlEnabled(); }
    bool experimentalStreamEnabled() { return RuntimeEnabledFeatures::experimentalStreamEnabled(); }
    bool suboriginsEnabled() { return RuntimeEnabledFeatures::suboriginsEnabled(); }
    bool textBlobEnabled() { return RuntimeEnabledFeatures::textBlobEnabled(); }
    bool threadedParserDataReceiverEnabled() { return RuntimeEnabledFeatures::threadedParserDataReceiverEnabled(); }
    bool touchEnabled() { return RuntimeEnabledFeatures::touchEnabled(); }
    bool touchIconLoadingEnabled() { return RuntimeEnabledFeatures::touchIconLoadingEnabled(); }
    bool unsafeES3APIsEnabled() { return RuntimeEnabledFeatures::unsafeES3APIsEnabled(); }
    bool unsandboxedAuxiliaryEnabled() { return RuntimeEnabledFeatures::unsandboxedAuxiliaryEnabled(); }
    bool userSelectAllEnabled() { return RuntimeEnabledFeatures::userSelectAllEnabled(); }
    bool webAnimationsAPIEnabled() { return RuntimeEnabledFeatures::webAnimationsAPIEnabled(); }
    bool webAnimationsSVGEnabled() { return RuntimeEnabledFeatures::webAnimationsSVGEnabled(); }
#if ENABLE(WEB_AUDIO)
    bool webAudioEnabled() { return RuntimeEnabledFeatures::webAudioEnabled(); }
#endif // ENABLE(WEB_AUDIO)
    bool webBluetoothEnabled() { return RuntimeEnabledFeatures::webBluetoothEnabled(); }
    bool webGLDraftExtensionsEnabled() { return RuntimeEnabledFeatures::webGLDraftExtensionsEnabled(); }
    bool webGLImageChromiumEnabled() { return RuntimeEnabledFeatures::webGLImageChromiumEnabled(); }
    bool webVREnabled() { return RuntimeEnabledFeatures::webVREnabled(); }
    bool webVTTRegionsEnabled() { return RuntimeEnabledFeatures::webVTTRegionsEnabled(); }
    bool v8IdleTasksEnabled() { return RuntimeEnabledFeatures::v8IdleTasksEnabled(); }
    bool xsltEnabled() { return RuntimeEnabledFeatures::xsltEnabled(); }
    bool svg1DOMEnabled() { return RuntimeEnabledFeatures::svg1DOMEnabled(); }
    bool scrollCustomizationEnabled() { return RuntimeEnabledFeatures::scrollCustomizationEnabled(); }
    bool scrollRestorationEnabled() { return RuntimeEnabledFeatures::scrollRestorationEnabled(); }

    DEFINE_INLINE_TRACE() { }

private:
    InternalRuntimeFlags() { }
};

} // namespace blink

#endif // InternalRuntimeFlags_h
