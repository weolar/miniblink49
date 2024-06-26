// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Audio rendering unit utilizing audio output stream provided by browser
// process through IPC.
//
// Relationship of classes.
//
//  AudioOutputController                AudioOutputDevice
//           ^                                ^
//           |                                |
//           v               IPC              v
//    AudioRendererHost  <---------> AudioOutputIPC (AudioMessageFilter)
//
// Transportation of audio samples from the render to the browser process
// is done by using shared memory in combination with a sync socket pair
// to generate a low latency transport. The AudioOutputDevice user registers an
// AudioOutputDevice::RenderCallback at construction and will be polled by the
// AudioOutputDevice for audio to be played out by the underlying audio layers.
//
// State sequences.
//
//            Task [IO thread]                  IPC [IO thread]
// RequestDeviceAuthorization -> RequestDeviceAuthorizationOnIOThread ------>
// RequestDeviceAuthorization ->
//             <- OnDeviceAuthorized <- AudioMsg_NotifyDeviceAuthorized <-
//
// Start -> CreateStreamOnIOThread -----> CreateStream ------>
//       <- OnStreamCreated <- AudioMsg_NotifyStreamCreated <-
//       ---> PlayOnIOThread -----------> PlayStream -------->
//
// Optionally Play() / Pause() sequences may occur:
// Play -> PlayOnIOThread --------------> PlayStream --------->
// Pause -> PauseOnIOThread ------------> PauseStream -------->
// (note that Play() / Pause() sequences before
// OnStreamCreated are deferred until OnStreamCreated, with the last valid
// state being used)
//
// AudioOutputDevice::Render => audio transport on audio thread =>
//                               |
// Stop --> ShutDownOnIOThread -------->  CloseStream -> Close
//
// This class utilizes several threads during its lifetime, namely:
// 1. Creating thread.
//    Must be the main render thread.
// 2. Control thread (may be the main render thread or another thread).
//    The methods: Start(), Stop(), Play(), Pause(), SetVolume()
//    must be called on the same thread.
// 3. IO thread (internal implementation detail - not exposed to public API)
//    The thread within which this class receives all the IPC messages and
//    IPC communications can only happen in this thread.
// 4. Audio transport thread (See AudioDeviceThread).
//    Responsible for calling the AudioThreadCallback implementation that in
//    turn calls AudioRendererSink::RenderCallback which feeds audio samples to
//    the audio layer in the browser process using sync sockets and shared
//    memory.
//
// Implementation notes:
// - The user must call Stop() before deleting the class instance.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_DEVICE_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_DEVICE_H_

#include <string>

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "base/synchronization/waitable_event.h"
#include "media/audio/audio_device_thread.h"
#include "media/audio/audio_output_ipc.h"
#include "media/audio/audio_parameters.h"
#include "media/audio/scoped_task_runner_observer.h"
#include "media/base/audio_renderer_sink.h"
#include "media/base/media_export.h"
#include "media/base/output_device.h"

namespace media {

class MEDIA_EXPORT AudioOutputDevice
    : NON_EXPORTED_BASE(public AudioRendererSink),
      NON_EXPORTED_BASE(public AudioOutputIPCDelegate),
      NON_EXPORTED_BASE(public OutputDevice),
      NON_EXPORTED_BASE(public ScopedTaskRunnerObserver) {
public:
    // NOTE: Clients must call Initialize() before using.
    AudioOutputDevice(
        scoped_ptr<AudioOutputIPC> ipc,
        const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner,
        int session_id,
        const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin);

    // Request authorization to use the device specified in the constructor.
    void RequestDeviceAuthorization();

    // AudioRendererSink implementation.
    void Initialize(const AudioParameters& params,
        RenderCallback* callback) override;
    void Start() override;
    void Stop() override;
    void Play() override;
    void Pause() override;
    bool SetVolume(double volume) override;
    OutputDevice* GetOutputDevice() override;

    // OutputDevice implementation
    AudioParameters GetOutputParameters() override;
    OutputDeviceStatus GetDeviceStatus() override;

    // Methods called on IO thread ----------------------------------------------
    // AudioOutputIPCDelegate methods.
    void OnStateChanged(AudioOutputIPCDelegateState state) override;
    void OnDeviceAuthorized(OutputDeviceStatus device_status,
        const media::AudioParameters& output_params) override;
    void OnStreamCreated(base::SharedMemoryHandle handle,
        base::SyncSocket::Handle socket_handle,
        int length) override;
    void OnIPCClosed() override;

protected:
    // Magic required by ref_counted.h to avoid any code deleting the object
    // accidentally while there are references to it.
    friend class base::RefCountedThreadSafe<AudioOutputDevice>;
    ~AudioOutputDevice() override;

private:
    // Note: The ordering of members in this enum is critical to correct behavior!
    enum State {
        IPC_CLOSED, // No more IPCs can take place.
        IDLE, // Not started.
        AUTHORIZING, // Sent device authorization request, waiting for reply.
        AUTHORIZED, // Successful device authorization received.
        CREATING_STREAM, // Waiting for OnStreamCreated() to be called back.
        PAUSED, // Paused.  OnStreamCreated() has been called.  Can Play()/Stop().
        PLAYING, // Playing back.  Can Pause()/Stop().
    };

    // Unsupported OutputDevice implementation
    void SwitchOutputDevice(const std::string& device_id,
        const /*url::Origin*/ std::string& security_origin,
        const SwitchOutputDeviceCB& callback) override;

    // Methods called on IO thread ----------------------------------------------
    // The following methods are tasks posted on the IO thread that need to
    // be executed on that thread.  They use AudioOutputIPC to send IPC messages
    // upon state changes.
    void RequestDeviceAuthorizationOnIOThread();
    void CreateStreamOnIOThread(const AudioParameters& params);
    void PlayOnIOThread();
    void PauseOnIOThread();
    void ShutDownOnIOThread();
    void SetVolumeOnIOThread(double volume);

    // base::MessageLoop::DestructionObserver implementation for the IO loop.
    // If the IO loop dies before we do, we shut down the audio thread from here.
    void WillDestroyCurrentMessageLoop() override;

    AudioParameters audio_parameters_;

    RenderCallback* callback_;

    // A pointer to the IPC layer that takes care of sending requests over to
    // the AudioRendererHost.  Only valid when state_ != IPC_CLOSED and must only
    // be accessed on the IO thread.
    scoped_ptr<AudioOutputIPC> ipc_;

    // Current state (must only be accessed from the IO thread).  See comments for
    // State enum above.
    State state_;

    // State of Start() calls before OnDeviceAuthorized() is called.
    bool start_on_authorized_;

    // State of Play() / Pause() calls before OnStreamCreated() is called.
    bool play_on_start_;

    // The media session ID used to identify which input device to be started.
    // Only used by Unified IO.
    int session_id_;

    // ID of hardware output device to be used (provided session_id_ is zero)
    const std::string device_id_;
    const /*url::Origin*/ std::string security_origin_;

    // Our audio thread callback class.  See source file for details.
    class AudioThreadCallback;

    // In order to avoid a race between OnStreamCreated and Stop(), we use this
    // guard to control stopping and starting the audio thread.
    base::Lock audio_thread_lock_;
    AudioDeviceThread audio_thread_;
    scoped_ptr<AudioOutputDevice::AudioThreadCallback> audio_callback_;

    // Temporary hack to ignore OnStreamCreated() due to the user calling Stop()
    // so we don't start the audio thread pointing to a potentially freed
    // |callback_|.
    //
    // TODO(scherkus): Replace this by changing AudioRendererSink to either accept
    // the callback via Start(). See http://crbug.com/151051 for details.
    bool stopping_hack_;

    base::WaitableEvent did_receive_auth_;
    media::AudioParameters output_params_;
    OutputDeviceStatus device_status_;

    DISALLOW_COPY_AND_ASSIGN(AudioOutputDevice);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_DEVICE_H_
