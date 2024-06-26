// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Implementation of AudioInputStream for Windows using Windows Core Audio
// WASAPI for low latency capturing.
//
// Overview of operation:
//
// - An object of WASAPIAudioInputStream is created by the AudioManager
//   factory.
// - Next some thread will call Open(), at that point the underlying
//   Core Audio APIs are utilized to create two WASAPI interfaces called
//   IAudioClient and IAudioCaptureClient.
// - Then some thread will call Start(sink).
//   A thread called "wasapi_capture_thread" is started and this thread listens
//   on an event signal which is set periodically by the audio engine for
//   each recorded data packet. As a result, data samples will be provided
//   to the registered sink.
// - At some point, a thread will call Stop(), which stops and joins the
//   capture thread and at the same time stops audio streaming.
// - The same thread that called stop will call Close() where we cleanup
//   and notify the audio manager, which likely will destroy this object.
//
// Implementation notes:
//
// - The minimum supported client is Windows Vista.
// - This implementation is single-threaded, hence:
//    o Construction and destruction must take place from the same thread.
//    o It is recommended to call all APIs from the same thread as well.
// - It is recommended to first acquire the native sample rate of the default
//   input device and then use the same rate when creating this object. Use
//   WASAPIAudioInputStream::HardwareSampleRate() to retrieve the sample rate.
// - Calling Close() also leads to self destruction.
//
// Core Audio API details:
//
// - Utilized MMDevice interfaces:
//     o IMMDeviceEnumerator
//     o IMMDevice
// - Utilized WASAPI interfaces:
//     o IAudioClient
//     o IAudioCaptureClient
// - The stream is initialized in shared mode and the processing of the
//   audio buffer is event driven.
// - The Multimedia Class Scheduler service (MMCSS) is utilized to boost
//   the priority of the capture thread.
// - Audio applications that use the MMDevice API and WASAPI typically use
//   the ISimpleAudioVolume interface to manage stream volume levels on a
//   per-session basis. It is also possible to use of the IAudioEndpointVolume
//   interface to control the master volume level of an audio endpoint device.
//   This implementation is using the ISimpleAudioVolume interface.
//   MSDN states that "In rare cases, a specialized audio application might
//   require the use of the IAudioEndpointVolume".
//
#ifndef MEDIA_AUDIO_WIN_AUDIO_LOW_LATENCY_INPUT_WIN_H_
#define MEDIA_AUDIO_WIN_AUDIO_LOW_LATENCY_INPUT_WIN_H_

#include <Audioclient.h>
#include <MMDeviceAPI.h>

#include <string>

#include "base/compiler_specific.h"
#include "base/threading/non_thread_safe.h"
#include "base/threading/platform_thread.h"
#include "base/threading/simple_thread.h"
#include "base/win/scoped_co_mem.h"
#include "base/win/scoped_com_initializer.h"
#include "base/win/scoped_comptr.h"
#include "base/win/scoped_handle.h"
#include "media/audio/agc_audio_stream.h"
#include "media/audio/audio_parameters.h"
#include "media/base/media_export.h"

namespace media {

class AudioBus;
class AudioManagerWin;

// AudioInputStream implementation using Windows Core Audio APIs.
class MEDIA_EXPORT WASAPIAudioInputStream
    : public AgcAudioStream<AudioInputStream>,
      public base::DelegateSimpleThread::Delegate,
      NON_EXPORTED_BASE(public base::NonThreadSafe) {
public:
    // The ctor takes all the usual parameters, plus |manager| which is the
    // the audio manager who is creating this object.
    WASAPIAudioInputStream(AudioManagerWin* manager,
        const AudioParameters& params,
        const std::string& device_id);

    // The dtor is typically called by the AudioManager only and it is usually
    // triggered by calling AudioInputStream::Close().
    ~WASAPIAudioInputStream() override;

    // Implementation of AudioInputStream.
    bool Open() override;
    void Start(AudioInputCallback* callback) override;
    void Stop() override;
    void Close() override;
    double GetMaxVolume() override;
    void SetVolume(double volume) override;
    double GetVolume() override;
    bool IsMuted() override;

    bool started() const { return started_; }

private:
    // DelegateSimpleThread::Delegate implementation.
    void Run() override;

    // Issues the OnError() callback to the |sink_|.
    void HandleError(HRESULT err);

    // The Open() method is divided into these sub methods.
    HRESULT SetCaptureDevice();
    HRESULT ActivateCaptureDevice();
    HRESULT GetAudioEngineStreamFormat();
    bool DesiredFormatIsSupported();
    HRESULT InitializeAudioEngine();

    // Our creator, the audio manager needs to be notified when we close.
    AudioManagerWin* manager_;

    // Capturing is driven by this thread (which has no message loop).
    // All OnData() callbacks will be called from this thread.
    base::DelegateSimpleThread* capture_thread_;

    // Contains the desired audio format which is set up at construction.
    WAVEFORMATEX format_;

    bool opened_;
    bool started_;

    // Size in bytes of each audio frame (4 bytes for 16-bit stereo PCM)
    size_t frame_size_;

    // Size in audio frames of each audio packet where an audio packet
    // is defined as the block of data which the user received in each
    // OnData() callback.
    size_t packet_size_frames_;

    // Size in bytes of each audio packet.
    size_t packet_size_bytes_;

    // Length of the audio endpoint buffer.
    uint32 endpoint_buffer_size_frames_;

    // Contains the unique name of the selected endpoint device.
    // Note that AudioManagerBase::kDefaultDeviceId represents the default
    // device role and is not a valid ID as such.
    std::string device_id_;

    // Conversion factor used in delay-estimation calculations.
    // Converts a raw performance counter value to 100-nanosecond unit.
    double perf_count_to_100ns_units_;

    // Conversion factor used in delay-estimation calculations.
    // Converts from milliseconds to audio frames.
    double ms_to_frame_count_;

    // Pointer to the object that will receive the recorded audio samples.
    AudioInputCallback* sink_;

    // Windows Multimedia Device (MMDevice) API interfaces.

    // An IMMDevice interface which represents an audio endpoint device.
    base::win::ScopedComPtr<IMMDevice> endpoint_device_;

    // Windows Audio Session API (WASAPI) interfaces.

    // An IAudioClient interface which enables a client to create and initialize
    // an audio stream between an audio application and the audio engine.
    base::win::ScopedComPtr<IAudioClient> audio_client_;

    // Loopback IAudioClient doesn't support event-driven mode, so a separate
    // IAudioClient is needed to receive notifications when data is available in
    // the buffer. For loopback input |audio_client_| is used to receive data,
    // while |audio_render_client_for_loopback_| is used to get notifications
    // when a new buffer is ready. See comment in InitializeAudioEngine() for
    // details.
    base::win::ScopedComPtr<IAudioClient> audio_render_client_for_loopback_;

    // The IAudioCaptureClient interface enables a client to read input data
    // from a capture endpoint buffer.
    base::win::ScopedComPtr<IAudioCaptureClient> audio_capture_client_;

    // The ISimpleAudioVolume interface enables a client to control the
    // master volume level of an audio session.
    // The volume-level is a value in the range 0.0 to 1.0.
    // This interface does only work with shared-mode streams.
    base::win::ScopedComPtr<ISimpleAudioVolume> simple_audio_volume_;

    // The audio engine will signal this event each time a buffer has been
    // recorded.
    base::win::ScopedHandle audio_samples_ready_event_;

    // This event will be signaled when capturing shall stop.
    base::win::ScopedHandle stop_capture_event_;

    // Extra audio bus used for storage of deinterleaved data for the OnData
    // callback.
    scoped_ptr<media::AudioBus> audio_bus_;

    DISALLOW_COPY_AND_ASSIGN(WASAPIAudioInputStream);
};

} // namespace media

#endif // MEDIA_AUDIO_WIN_AUDIO_LOW_LATENCY_INPUT_WIN_H_
