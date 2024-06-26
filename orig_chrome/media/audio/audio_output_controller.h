// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_CONTROLLER_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_CONTROLLER_H_

#include "base/atomic_ref_count.h"
#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/timer/timer.h"
#include "build/build_config.h"
#include "media/audio/audio_io.h"
#include "media/audio/audio_manager.h"
#include "media/audio/audio_power_monitor.h"
#include "media/audio/audio_source_diverter.h"
#include "media/audio/simple_sources.h"
#include "media/base/media_export.h"

// An AudioOutputController controls an AudioOutputStream and provides data
// to this output stream. It has an important function that it executes
// audio operations like play, pause, stop, etc. on a separate thread,
// namely the audio manager thread.
//
// All the public methods of AudioOutputController are non-blocking.
// The actual operations are performed on the audio manager thread.
//
// Here is a state transition diagram for the AudioOutputController:
//
//   *[ Empty ]  -->  [ Created ]  -->  [ Playing ]  -------.
//        |                |               |    ^           |
//        |                |               |    |           |
//        |                |               |    |           v
//        |                |               |    `-----  [ Paused ]
//        |                |               |                |
//        |                v               v                |
//        `----------->  [      Closed       ]  <-----------'
//
// * Initial state
//
// At any time after reaching the Created state but before Closed, the
// AudioOutputController may be notified of a device change via
// OnDeviceChange().  As the OnDeviceChange() is processed, state transitions
// will occur, ultimately ending up in an equivalent pre-call state.  E.g., if
// the state was Paused, the new state will be Created, since these states are
// all functionally equivalent and require a Play() call to continue to the next
// state.
//
// The AudioOutputStream can request data from the AudioOutputController via the
// AudioSourceCallback interface. AudioOutputController uses the SyncReader
// passed to it via construction to synchronously fulfill this read request.
//

namespace media {

class MEDIA_EXPORT AudioOutputController
    : public base::RefCountedThreadSafe<AudioOutputController>,
      public AudioOutputStream::AudioSourceCallback,
      public AudioSourceDiverter,
      NON_EXPORTED_BASE(public AudioManager::AudioDeviceListener) {
public:
    // An event handler that receives events from the AudioOutputController. The
    // following methods are called on the audio manager thread.
    class MEDIA_EXPORT EventHandler {
    public:
        virtual void OnCreated() = 0;
        virtual void OnPlaying() = 0;
        virtual void OnPaused() = 0;
        virtual void OnError() = 0;

    protected:
        virtual ~EventHandler() { }
    };

    // A synchronous reader interface used by AudioOutputController for
    // synchronous reading.
    // TODO(crogers): find a better name for this class and the Read() method
    // now that it can handle synchronized I/O.
    class SyncReader {
    public:
        virtual ~SyncReader() { }

        // Notify the synchronous reader the number of bytes in the
        // AudioOutputController not yet played. This is used by SyncReader to
        // prepare more data and perform synchronization.
        virtual void UpdatePendingBytes(uint32 bytes) = 0;

        // Attempts to completely fill |dest|, zeroing |dest| if the request can not
        // be fulfilled (due to timeout).
        virtual void Read(AudioBus* dest) = 0;

        // Close this synchronous reader.
        virtual void Close() = 0;
    };

    // Factory method for creating an AudioOutputController.
    // This also creates and opens an AudioOutputStream on the audio manager
    // thread, and if this is successful, the |event_handler| will receive an
    // OnCreated() call from the same audio manager thread.  |audio_manager| must
    // outlive AudioOutputController.
    // The |output_device_id| can be either empty (default device) or specify a
    // specific hardware device for audio output.
    static scoped_refptr<AudioOutputController> Create(
        AudioManager* audio_manager, EventHandler* event_handler,
        const AudioParameters& params, const std::string& output_device_id,
        SyncReader* sync_reader);

    // Indicates whether audio power level analysis will be performed.  If false,
    // ReadCurrentPowerAndClip() can not be called.
    static bool will_monitor_audio_levels()
    {
#if defined(OS_ANDROID) || defined(OS_IOS)
        return false;
#else
        return true;
#endif
    }

    // Methods to control playback of the stream.

    // Starts the playback of this audio output stream.
    void Play();

    // Pause this audio output stream.
    void Pause();

    // Closes the audio output stream. The state is changed and the resources
    // are freed on the audio manager thread. closed_task is executed after that.
    // Callbacks (EventHandler and SyncReader) must exist until closed_task is
    // called.
    //
    // It is safe to call this method more than once. Calls after the first one
    // will have no effect.
    void Close(const base::Closure& closed_task);

    // Sets the volume of the audio output stream.
    void SetVolume(double volume);

    // Calls |callback| (on the caller's thread) with the current output
    // device ID.
    void GetOutputDeviceId(
        base::Callback<void(const std::string&)> callback) const;

    // Changes which output device to use. If desired, you can provide a
    // callback that will be notified (on the thread you called from)
    // when the function has completed execution.
    //
    // Changing the output device causes the controller to go through
    // the same state transition back to the current state as a call to
    // OnDeviceChange (unless it is currently diverting, see
    // Start/StopDiverting below, in which case the state transition
    // will happen when StopDiverting is called).
    void SwitchOutputDevice(const std::string& output_device_id,
        const base::Closure& callback);

    // AudioSourceCallback implementation.
    int OnMoreData(AudioBus* dest, uint32 total_bytes_delay) override;
    void OnError(AudioOutputStream* stream) override;

    // AudioDeviceListener implementation.  When called AudioOutputController will
    // shutdown the existing |stream_|, transition to the kRecreating state,
    // create a new stream, and then transition back to an equivalent state prior
    // to being called.
    void OnDeviceChange() override;

    // AudioSourceDiverter implementation.
    const AudioParameters& GetAudioParameters() override;
    void StartDiverting(AudioOutputStream* to_stream) override;
    void StopDiverting() override;

    // Accessor for AudioPowerMonitor::ReadCurrentPowerAndClip().  See comments in
    // audio_power_monitor.h for usage.  This may be called on any thread.
    std::pair<float, bool> ReadCurrentPowerAndClip();

protected:
    // Internal state of the source.
    enum State {
        kEmpty,
        kCreated,
        kPlaying,
        kPaused,
        kClosed,
        kError,
    };

    // Time constant for AudioPowerMonitor.  See AudioPowerMonitor ctor comments
    // for semantics.  This value was arbitrarily chosen, but seems to work well.
    enum { kPowerMeasurementTimeConstantMillis = 10 };

    friend class base::RefCountedThreadSafe<AudioOutputController>;
    ~AudioOutputController() override;

private:
    AudioOutputController(AudioManager* audio_manager, EventHandler* handler,
        const AudioParameters& params,
        const std::string& output_device_id,
        SyncReader* sync_reader);

    // The following methods are executed on the audio manager thread.
    void DoCreate(bool is_for_device_change);
    void DoPlay();
    void DoPause();
    void DoClose();
    void DoSetVolume(double volume);
    std::string DoGetOutputDeviceId() const;
    void DoSwitchOutputDevice(const std::string& output_device_id);
    void DoReportError();
    void DoStartDiverting(AudioOutputStream* to_stream);
    void DoStopDiverting();

    // Helper method that stops the physical stream.
    void StopStream();

    // Helper method that stops, closes, and NULLs |*stream_|.
    void DoStopCloseAndClearStream();

    // Checks if a stream was started successfully but never calls OnMoreData().
    void WedgeCheck();

    AudioManager* const audio_manager_;
    const AudioParameters params_;
    EventHandler* const handler_;

    // Specifies the device id of the output device to open or empty for the
    // default output device.
    std::string output_device_id_;

    AudioOutputStream* stream_;

    // When non-NULL, audio is being diverted to this stream.
    AudioOutputStream* diverting_to_stream_;

    // The current volume of the audio stream.
    double volume_;

    // |state_| may only be used on the audio manager thread.
    State state_;

    // SyncReader is used only in low latency mode for synchronous reading.
    SyncReader* const sync_reader_;

    // The message loop of audio manager thread that this object runs on.
    const scoped_refptr<base::SingleThreadTaskRunner> message_loop_;

    // Scans audio samples from OnMoreData() as input to compute power levels.
    AudioPowerMonitor power_monitor_;

    // Flags when we've asked for a stream to start but it never did.
    base::AtomicRefCount on_more_io_data_called_;
    scoped_ptr<base::OneShotTimer> wedge_timer_;

    // Flag which indicates errors received during Stop/Close should be ignored.
    // These errors are generally harmless since a fresh stream is about to be
    // recreated, but if forwarded, renderer side clients may consider them
    // catastrophic and abort their operations.
    //
    // If |stream_| is started then |ignore_errors_during_stop_close_| must only
    // be accessed while |error_lock_| is held.
    bool ignore_errors_during_stop_close_;
    base::Lock error_lock_;

    DISALLOW_COPY_AND_ASSIGN(AudioOutputController);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_CONTROLLER_H_
