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

#ifndef WebRuntimeFeatures_h
#define WebRuntimeFeatures_h

#include "../platform/WebCommon.h"
#include "../platform/WebString.h"

namespace blink {

// This class is used to enable runtime features of Blink.
// Stable features are enabled by default.
class WebRuntimeFeatures {
public:
    BLINK_EXPORT static void enableExperimentalFeatures(bool);

    BLINK_EXPORT static void enableWebBluetooth(bool);

    BLINK_EXPORT static void enableTestOnlyFeatures(bool);

    // Enables a feature by its string identifier from
    // RuntimeEnabledFeatures.in.
    BLINK_EXPORT static void enableFeatureFromString(const WebString& name, bool enable);

    BLINK_EXPORT static void enableApplicationCache(bool);

    BLINK_EXPORT static void enableDatabase(bool);

    BLINK_EXPORT static void enableCompositedSelectionUpdate(bool);
    BLINK_EXPORT static bool isCompositedSelectionUpdateEnabled();

    BLINK_EXPORT static void enableDecodeToYUV(bool);

    BLINK_EXPORT static void enableDisplayList2dCanvas(bool);
    BLINK_EXPORT static void forceDisplayList2dCanvas(bool);

    BLINK_EXPORT static void enableEncryptedMedia(bool);
    BLINK_EXPORT static bool isEncryptedMediaEnabled();

    BLINK_EXPORT static void enablePrefixedEncryptedMedia(bool);
    BLINK_EXPORT static bool isPrefixedEncryptedMediaEnabled();

    BLINK_EXPORT static void enableBleedingEdgeFastPaths(bool);

    BLINK_EXPORT static void enableBlinkScheduler(bool);

    BLINK_EXPORT static void enableCompositorAnimationTimelines(bool);

    BLINK_EXPORT static void enableExperimentalCanvasFeatures(bool);

    BLINK_EXPORT static void enableFastMobileScrolling(bool);

    BLINK_EXPORT static void enableFileSystem(bool);

    BLINK_EXPORT static void enableImageColorProfiles(bool);

    BLINK_EXPORT static void enableMediaPlayer(bool);

    BLINK_EXPORT static void enableSubpixelFontScaling(bool);

    BLINK_EXPORT static void enableMediaCapture(bool);

    BLINK_EXPORT static void enableMediaSource(bool);

    BLINK_EXPORT static void enableNotificationConstructor(bool);

    BLINK_EXPORT static void enableNotifications(bool);

    BLINK_EXPORT static void enableNavigatorContentUtils(bool);

    BLINK_EXPORT static void enableNetworkInformation(bool);

    BLINK_EXPORT static void enableOrientationEvent(bool);

    BLINK_EXPORT static void enablePagePopup(bool);

    BLINK_EXPORT static void enablePermissionsAPI(bool);

    BLINK_EXPORT static void enableRequestAutocomplete(bool);

    BLINK_EXPORT static void enableScreenOrientation(bool);

    BLINK_EXPORT static void enableScriptedSpeech(bool);

    BLINK_EXPORT static void enableSlimmingPaint(bool);

    BLINK_EXPORT static void enableTouch(bool);

    BLINK_EXPORT static void enableTouchIconLoading(bool);

    BLINK_EXPORT static void enableWebAudio(bool);

    BLINK_EXPORT static void enableWebGLDraftExtensions(bool);

    BLINK_EXPORT static void enableWebGLImageChromium(bool);

    BLINK_EXPORT static void enableWebMIDI(bool);

    BLINK_EXPORT static void enableXSLT(bool);

    BLINK_EXPORT static void enableOverlayScrollbars(bool);

    BLINK_EXPORT static void enableOverlayFullscreenVideo(bool);

    BLINK_EXPORT static void enableSharedWorker(bool);

    BLINK_EXPORT static void enablePreciseMemoryInfo(bool);

    BLINK_EXPORT static void enableLayerSquashing(bool) { }

    BLINK_EXPORT static void enableCredentialManagerAPI(bool);

    BLINK_EXPORT static void enableTextBlobs(bool);

    BLINK_EXPORT static void enableCSSViewport(bool);

    BLINK_EXPORT static void enableV8IdleTasks(bool);

    BLINK_EXPORT static void enableSVG1DOM(bool);

    BLINK_EXPORT static void enableReducedReferrerGranularity(bool);

    BLINK_EXPORT static void enablePushMessaging(bool);

    BLINK_EXPORT static void enablePushMessagingData(bool);

    BLINK_EXPORT static void enablePushMessagingHasPermission(bool);

    BLINK_EXPORT static void enableStaleWhileRevalidateCacheControl(bool);

    BLINK_EXPORT static void enableUnsafeES3APIs(bool);

    BLINK_EXPORT static void enableWebVR(bool);
private:
    WebRuntimeFeatures();
};

} // namespace blink

#endif
