/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "public/web/WebRuntimeFeatures.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "web/WebMediaPlayerClientImpl.h"

namespace blink {

void WebRuntimeFeatures::enableExperimentalFeatures(bool enable)
{
    RuntimeEnabledFeatures::setExperimentalFeaturesEnabled(enable);
}

void WebRuntimeFeatures::enableWebBluetooth(bool enable)
{
    RuntimeEnabledFeatures::setWebBluetoothEnabled(enable);
}

void WebRuntimeFeatures::enableFeatureFromString(const WebString& name, bool enable)
{
    RuntimeEnabledFeatures::setFeatureEnabledFromString(name, enable);
}

void WebRuntimeFeatures::enableBleedingEdgeFastPaths(bool enable)
{
    ASSERT(enable);
    RuntimeEnabledFeatures::setBleedingEdgeFastPathsEnabled(enable);
    RuntimeEnabledFeatures::setWebAnimationsAPIEnabled(enable);
}

void WebRuntimeFeatures::enableBlinkScheduler(bool enable)
{
    RuntimeEnabledFeatures::setBlinkSchedulerEnabled(enable);
}

void WebRuntimeFeatures::enableCompositorAnimationTimelines(bool enable)
{
    RuntimeEnabledFeatures::setCompositorAnimationTimelinesEnabled(enable);
}

void WebRuntimeFeatures::enableTestOnlyFeatures(bool enable)
{
    RuntimeEnabledFeatures::setTestFeaturesEnabled(enable);
}

void WebRuntimeFeatures::enableApplicationCache(bool enable)
{
    RuntimeEnabledFeatures::setApplicationCacheEnabled(enable);
}

void WebRuntimeFeatures::enableCompositedSelectionUpdate(bool enable)
{
    RuntimeEnabledFeatures::setCompositedSelectionUpdateEnabled(enable);
}

bool WebRuntimeFeatures::isCompositedSelectionUpdateEnabled()
{
    return RuntimeEnabledFeatures::compositedSelectionUpdateEnabled();
}

void WebRuntimeFeatures::enableDatabase(bool enable)
{
    RuntimeEnabledFeatures::setDatabaseEnabled(enable);
}

void WebRuntimeFeatures::enableDecodeToYUV(bool enable)
{
    RuntimeEnabledFeatures::setDecodeToYUVEnabled(enable);
}

void WebRuntimeFeatures::forceDisplayList2dCanvas(bool enable)
{
    RuntimeEnabledFeatures::setForceDisplayList2dCanvasEnabled(enable);
}

void WebRuntimeFeatures::enableDisplayList2dCanvas(bool enable)
{
    RuntimeEnabledFeatures::setDisplayList2dCanvasEnabled(enable);
}

void WebRuntimeFeatures::enableEncryptedMedia(bool enable)
{
    RuntimeEnabledFeatures::setEncryptedMediaEnabled(enable);
}

bool WebRuntimeFeatures::isEncryptedMediaEnabled()
{
    return RuntimeEnabledFeatures::encryptedMediaEnabled();
}

void WebRuntimeFeatures::enablePrefixedEncryptedMedia(bool enable)
{
    RuntimeEnabledFeatures::setPrefixedEncryptedMediaEnabled(enable);
}

bool WebRuntimeFeatures::isPrefixedEncryptedMediaEnabled()
{
    return RuntimeEnabledFeatures::prefixedEncryptedMediaEnabled();
}

void WebRuntimeFeatures::enableExperimentalCanvasFeatures(bool enable)
{
    RuntimeEnabledFeatures::setExperimentalCanvasFeaturesEnabled(enable);
}

void WebRuntimeFeatures::enableFastMobileScrolling(bool enable)
{
    RuntimeEnabledFeatures::setFastMobileScrollingEnabled(enable);
}

void WebRuntimeFeatures::enableFileSystem(bool enable)
{
    RuntimeEnabledFeatures::setFileSystemEnabled(enable);
}

void WebRuntimeFeatures::enableImageColorProfiles(bool enable)
{
    RuntimeEnabledFeatures::setImageColorProfilesEnabled(enable);
}

void WebRuntimeFeatures::enableMediaPlayer(bool enable)
{
    RuntimeEnabledFeatures::setMediaEnabled(enable);
}

void WebRuntimeFeatures::enableMediaCapture(bool enable)
{
    RuntimeEnabledFeatures::setMediaCaptureEnabled(enable);
}

void WebRuntimeFeatures::enableMediaSource(bool enable)
{
    RuntimeEnabledFeatures::setMediaSourceEnabled(enable);
}

void WebRuntimeFeatures::enableNotificationConstructor(bool enable)
{
    RuntimeEnabledFeatures::setNotificationConstructorEnabled(enable);
}

void WebRuntimeFeatures::enableNotifications(bool enable)
{
    RuntimeEnabledFeatures::setNotificationsEnabled(enable);
}

void WebRuntimeFeatures::enableNavigatorContentUtils(bool enable)
{
    RuntimeEnabledFeatures::setNavigatorContentUtilsEnabled(enable);
}

void WebRuntimeFeatures::enableNetworkInformation(bool enable)
{
    RuntimeEnabledFeatures::setNetworkInformationEnabled(enable);
}

void WebRuntimeFeatures::enableOrientationEvent(bool enable)
{
    RuntimeEnabledFeatures::setOrientationEventEnabled(enable);
}

void WebRuntimeFeatures::enablePagePopup(bool enable)
{
    RuntimeEnabledFeatures::setPagePopupEnabled(enable);
}

void WebRuntimeFeatures::enablePermissionsAPI(bool enable)
{
    RuntimeEnabledFeatures::setPermissionsEnabled(enable);
}

void WebRuntimeFeatures::enableRequestAutocomplete(bool enable)
{
    RuntimeEnabledFeatures::setRequestAutocompleteEnabled(enable);
}

void WebRuntimeFeatures::enableScreenOrientation(bool enable)
{
    RuntimeEnabledFeatures::setScreenOrientationEnabled(enable);
}

void WebRuntimeFeatures::enableScriptedSpeech(bool enable)
{
    RuntimeEnabledFeatures::setScriptedSpeechEnabled(enable);
}

void WebRuntimeFeatures::enableSlimmingPaint(bool enable)
{
    RuntimeEnabledFeatures::setSlimmingPaintEnabled(enable);
}

void WebRuntimeFeatures::enableTouch(bool enable)
{
    RuntimeEnabledFeatures::setTouchEnabled(enable);
}

void WebRuntimeFeatures::enableTouchIconLoading(bool enable)
{
    RuntimeEnabledFeatures::setTouchIconLoadingEnabled(enable);
}

void WebRuntimeFeatures::enableWebAudio(bool enable)
{
    RuntimeEnabledFeatures::setWebAudioEnabled(enable);
}

void WebRuntimeFeatures::enableWebGLDraftExtensions(bool enable)
{
    RuntimeEnabledFeatures::setWebGLDraftExtensionsEnabled(enable);
}

void WebRuntimeFeatures::enableWebGLImageChromium(bool enable)
{
    RuntimeEnabledFeatures::setWebGLImageChromiumEnabled(enable);
}

void WebRuntimeFeatures::enableXSLT(bool enable)
{
    RuntimeEnabledFeatures::setXSLTEnabled(enable);
}

void WebRuntimeFeatures::enableOverlayScrollbars(bool enable)
{
    RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(enable);
}

void WebRuntimeFeatures::enableOverlayFullscreenVideo(bool enable)
{
    RuntimeEnabledFeatures::setOverlayFullscreenVideoEnabled(enable);
}

void WebRuntimeFeatures::enableSharedWorker(bool enable)
{
    RuntimeEnabledFeatures::setSharedWorkerEnabled(enable);
}

void WebRuntimeFeatures::enablePreciseMemoryInfo(bool enable)
{
    RuntimeEnabledFeatures::setPreciseMemoryInfoEnabled(enable);
}

void WebRuntimeFeatures::enableCredentialManagerAPI(bool enable)
{
    RuntimeEnabledFeatures::setCredentialManagerEnabled(enable);
}

void WebRuntimeFeatures::enableTextBlobs(bool enable)
{
    RuntimeEnabledFeatures::setTextBlobEnabled(enable);
}

void WebRuntimeFeatures::enableCSSViewport(bool enable)
{
    RuntimeEnabledFeatures::setCSSViewportEnabled(enable);
}

void WebRuntimeFeatures::enableV8IdleTasks(bool enable)
{
    RuntimeEnabledFeatures::setV8IdleTasksEnabled(enable);
}

void WebRuntimeFeatures::enableSVG1DOM(bool enable)
{
    RuntimeEnabledFeatures::setSVG1DOMEnabled(enable);
}

void WebRuntimeFeatures::enableReducedReferrerGranularity(bool enable)
{
    RuntimeEnabledFeatures::setReducedReferrerGranularityEnabled(enable);
}

void WebRuntimeFeatures::enablePushMessaging(bool enable)
{
    RuntimeEnabledFeatures::setPushMessagingEnabled(enable);
}

void WebRuntimeFeatures::enablePushMessagingData(bool enable)
{
    RuntimeEnabledFeatures::setPushMessagingDataEnabled(enable);
}

void WebRuntimeFeatures::enableStaleWhileRevalidateCacheControl(bool enable)
{
    RuntimeEnabledFeatures::setStaleWhileRevalidateCacheControlEnabled(enable);
}

void WebRuntimeFeatures::enableUnsafeES3APIs(bool enable)
{
    RuntimeEnabledFeatures::setUnsafeES3APIsEnabled(enable);
}

void WebRuntimeFeatures::enableWebVR(bool enable)
{
    RuntimeEnabledFeatures::setWebVREnabled(enable);
}

} // namespace blink
