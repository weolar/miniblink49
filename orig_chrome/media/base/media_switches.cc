// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/media_switches.h"

namespace switches {

// Allow users to specify a custom buffer size for debugging purpose.
const char kAudioBufferSize[] = "audio-buffer-size";

// Set number of threads to use for video decoding.
const char kVideoThreads[] = "video-threads";

#if defined(OS_ANDROID)
// Sets the MediaSource player that uses UI thread for frame processing.
const char kDisableMediaThreadForMediaPlayback[] = "disable-media-thread-for-media-playback";

// Sets the MediaSource player that uses the separate media thread
const char kEnableMediaThreadForMediaPlayback[] = "enable-media-thread-for-media-playback";

// Use WebMediaPlayerImpl instead of WebMediaPlayerAndroid. This is a temporary
// switch for experimenting with unifying the Android playback pipeline.
const char kEnableUnifiedMediaPipeline[] = "enable-unified-media-pipeline";
#endif

#if defined(OS_LINUX) || defined(OS_FREEBSD) || defined(OS_SOLARIS)
// The Alsa device to use when opening an audio input stream.
const char kAlsaInputDevice[] = "alsa-input-device";
// The Alsa device to use when opening an audio stream.
const char kAlsaOutputDevice[] = "alsa-output-device";
#endif

// Use GpuMemoryBuffers for Video Capture when this is an option for the device.
// Experimental, see http://crbug.com/503835 and http://crbug.com/440843.
const char kUseGpuMemoryBuffersForCapture[] = "use-gpu-memory-buffers-for-capture";

#if defined(OS_MACOSX)
// AVFoundation is available in versions 10.7 and onwards, and is to be used
// http://crbug.com/288562 for both audio and video device monitoring and for
// video capture. Being a dynamically loaded NSBundle and library, it hits the
// Chrome startup time (http://crbug.com/311325 and http://crbug.com/311437);
// for experimentation purposes, in particular library load time issue, the
// usage of this library can be enabled by using this flag.
const char kEnableAVFoundation[] = "enable-avfoundation";

// QTKit is the media capture API predecessor to AVFoundation, available up and
// until Mac OS X 10.9 (despite being deprecated in this last one). This flag
// is used for troubleshooting and testing, and forces QTKit in builds and
// configurations where AVFoundation would be used otherwise.
const char kForceQTKit[] = "force-qtkit";
#endif

#if defined(OS_WIN)
// Use exclusive mode audio streaming for Windows Vista and higher.
// Leads to lower latencies for audio streams which uses the
// AudioParameters::AUDIO_PCM_LOW_LATENCY audio path.
// See http://msdn.microsoft.com/en-us/library/windows/desktop/dd370844.aspx
// for details.
const char kEnableExclusiveAudio[] = "enable-exclusive-audio";

// Used to troubleshoot problems with different video capture implementations
// on Windows.  By default we use the Media Foundation API on Windows 7 and up,
// but specifying this switch will force use of DirectShow always.
// See bug: http://crbug.com/268412
const char kForceDirectShowVideoCapture[] = "force-directshow";

// Force the use of MediaFoundation for video capture. This is only supported in
// Windows 7 and above. Used, like |kForceDirectShowVideoCapture|, to
// troubleshoot problems in Windows platforms.
const char kForceMediaFoundationVideoCapture[] = "force-mediafoundation";

// Use Windows WaveOut/In audio API even if Core Audio is supported.
const char kForceWaveAudio[] = "force-wave-audio";

// Instead of always using the hardware channel layout, check if a driver
// supports the source channel layout.  Avoids outputting empty channels and
// permits drivers to enable stereo to multichannel expansion.  Kept behind a
// flag since some drivers lie about supported layouts and hang when used.  See
// http://crbug.com/259165 for more details.
const char kTrySupportedChannelLayouts[] = "try-supported-channel-layouts";

// Number of buffers to use for WaveOut.
const char kWaveOutBuffers[] = "waveout-buffers";
#endif

#if defined(USE_CRAS)
// Use CRAS, the ChromeOS audio server.
const char kUseCras[] = "use-cras";
#endif

// Enables the audio thread hang monitor.  Allows us to find users in the field
// who have stuck audio threads.  See crbug.com/422522 and crbug.com/478932.
// TODO(dalecurtis): This should be removed once those issues are resolved.
const char kEnableAudioHangMonitor[] = "enable-audio-hang-monitor";

// Use fake device for Media Stream to replace actual camera and microphone.
const char kUseFakeDeviceForMediaStream[] = "use-fake-device-for-media-stream";

// Use an .y4m file to play as the webcam. See the comments in
// media/capture/video/file_video_capture_device.h for more details.
const char kUseFileForFakeVideoCapture[] = "use-file-for-fake-video-capture";

// Play a .wav file as the microphone. Note that for WebRTC calls we'll treat
// the bits as if they came from the microphone, which means you should disable
// audio processing (lest your audio file will play back distorted). The input
// file is converted to suit Chrome's audio buses if necessary, so most sane
// .wav files should work.
const char kUseFileForFakeAudioCapture[] = "use-file-for-fake-audio-capture";

// Enables support for inband text tracks in media content.
const char kEnableInbandTextTracks[] = "enable-inband-text-tracks";

// When running tests on a system without the required hardware or libraries,
// this flag will cause the tests to fail. Otherwise, they silently succeed.
const char kRequireAudioHardwareForTesting[] = "require-audio-hardware-for-testing";

// Allows clients to override the threshold for when the media renderer will
// declare the underflow state for the video stream when audio is present.
// TODO(dalecurtis): Remove once experiments for http://crbug.com/470940 finish.
const char kVideoUnderflowThresholdMs[] = "video-underflow-threshold-ms";

// Disables the new rendering algorithm for webrtc, which is designed to improve
// the rendering smoothness.
const char kDisableRTCSmoothnessAlgorithm[] = "disable-rtc-smoothness-algorithm";

} // namespace switches
