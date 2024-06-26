// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Low-latency audio capturing class utilizing audio input stream provided
// by a server (browser) process by use of an IPC interface.
//
// Relationship of classes:
//
//  AudioInputController                 AudioInputDevice
//           ^                                  ^
//           |                                  |
//           v                  IPC             v
// AudioInputRendererHost  <----------->  AudioInputIPC
//           ^                            (AudioInputMessageFilter)
//           |
//           v
// AudioInputDeviceManager
//
// Transportation of audio samples from the browser to the render process
// is done by using shared memory in combination with a SyncSocket.
// The AudioInputDevice user registers an AudioInputDevice::CaptureCallback by
// calling Initialize().  The callback will be called with recorded audio from
// the underlying audio layers.
// The session ID is used by the AudioInputRendererHost to start the device
// referenced by this ID.
//
// State sequences:
//
// Start -> InitializeOnIOThread -> CreateStream ->
//       <- OnStreamCreated <-
//       -> StartOnIOThread -> PlayStream ->
//
//
// AudioInputDevice::Capture => low latency audio transport on audio thread =>
//                               |
// Stop --> ShutDownOnIOThread ------>  CloseStream -> Close
//
// This class depends on two threads to function:
//
// 1. An IO thread.
//    This thread is used to asynchronously process Start/Stop etc operations
//    that are available via the public interface.  The public methods are
//    asynchronous and simply post a task to the IO thread to actually perform
//    the work.
// 2. Audio transport thread.
//    Responsible for calling the CaptureCallback and feed audio samples from
//    the server side audio layer using a socket and shared memory.
//
// Implementation notes:
// - The user must call Stop() before deleting the class instance.

#ifndef MEDIA_AUDIO_AUDIO_INPUT_DEVICE_H_
#define MEDIA_AUDIO_AUDIO_INPUT_DEVICE_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "media/audio/audio_device_thread.h"
#include "media/audio/audio_input_ipc.h"
#include "media/audio/audio_parameters.h"
#include "media/audio/scoped_task_runner_observer.h"
#include "media/base/audio_capturer_source.h"
#include "media/base/media_export.h"

namespace media {

// TODO(henrika): This class is based on the AudioOutputDevice class and it has
// many components in common. Investigate potential for re-factoring.
// See http://crbug.com/179597.
// TODO(henrika): Add support for event handling (e.g. OnStateChanged,
// OnCaptureStopped etc.) and ensure that we can deliver these notifications
// to any clients using this class.
class MEDIA_EXPORT AudioInputDevice
    : NON_EXPORTED_BASE(public AudioCapturerSource),
      NON_EXPORTED_BASE(public AudioInputIPCDelegate),
      NON_EXPORTED_BASE(public ScopedTaskRunnerObserver) {
public:
    // NOTE: Clients must call Initialize() before using.
    AudioInputDevice(
        scoped_ptr<AudioInputIPC> ipc,
        const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner);

    // AudioCapturerSource implementation.
    void Initialize(const AudioParameters& params,
        CaptureCallback* callback,
        int session_id) override;
    void Start() override;
    void Stop() override;
    void SetVolume(double volume) override;
    void SetAutomaticGainControl(bool enabled) override;

protected:
    friend class base::RefCountedThreadSafe<AudioInputDevice>;
    ~AudioInputDevice() override;

    // Methods called on IO thread ----------------------------------------------
    // AudioInputIPCDelegate implementation.
    void OnStreamCreated(base::SharedMemoryHandle handle,
        base::SyncSocket::Handle socket_handle,
        int length,
        int total_segments) override;
    void OnVolume(double volume) override;
    void OnStateChanged(AudioInputIPCDelegateState state) override;
    void OnIPCClosed() override;

private:
    // Note: The ordering of members in this enum is critical to correct behavior!
    enum State {
        IPC_CLOSED, // No more IPCs can take place.
        IDLE, // Not started.
        CREATING_STREAM, // Waiting for OnStreamCreated() to be called back.
        RECORDING, // Receiving audio data.
    };

    // Methods called on IO thread ----------------------------------------------
    // The following methods are tasks posted on the IO thread that needs to
    // be executed on that thread. They interact with AudioInputMessageFilter and
    // sends IPC messages on that thread.
    void StartUpOnIOThread();
    void ShutDownOnIOThread();
    void SetVolumeOnIOThread(double volume);
    void SetAutomaticGainControlOnIOThread(bool enabled);

    // base::MessageLoop::DestructionObserver implementation for the IO loop.
    // If the IO loop dies before we do, we shut down the audio thread from here.
    void WillDestroyCurrentMessageLoop() override;

    AudioParameters audio_parameters_;

    CaptureCallback* callback_;

    // A pointer to the IPC layer that takes care of sending requests over to
    // the AudioInputRendererHost.  Only valid when state_ != IPC_CLOSED and must
    // only be accessed on the IO thread.
    scoped_ptr<AudioInputIPC> ipc_;

    // Current state (must only be accessed from the IO thread).  See comments for
    // State enum above.
    State state_;

    // The media session ID used to identify which input device to be started.
    // Only modified in Initialize() and ShutDownOnIOThread().
    int session_id_;

    // Stores the Automatic Gain Control state. Default is false.
    // Only modified on the IO thread.
    bool agc_is_enabled_;

    // Our audio thread callback class.  See source file for details.
    class AudioThreadCallback;

    // In order to avoid a race between OnStreamCreated and Stop(), we use this
    // guard to control stopping and starting the audio thread.
    base::Lock audio_thread_lock_;
    AudioDeviceThread audio_thread_;
    scoped_ptr<AudioInputDevice::AudioThreadCallback> audio_callback_;

    // Temporary hack to ignore OnStreamCreated() due to the user calling Stop()
    // so we don't start the audio thread pointing to a potentially freed
    // |callback_|.
    //
    // TODO(miu): Replace this by changing AudioCapturerSource to accept the
    // callback via Start(). See http://crbug.com/151051 for details.
    bool stopping_hack_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(AudioInputDevice);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_INPUT_DEVICE_H_
