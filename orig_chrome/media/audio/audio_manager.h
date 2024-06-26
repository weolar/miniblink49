// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_MANAGER_H_
#define MEDIA_AUDIO_AUDIO_MANAGER_H_

#include <string>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string16.h"
#include "media/audio/audio_device_name.h"
#include "media/audio/audio_logging.h"
#include "media/audio/audio_parameters.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

class AudioInputStream;
class AudioManagerFactory;
class AudioOutputStream;

// Manages all audio resources.  Provides some convenience functions that avoid
// the need to provide iterators over the existing streams.
class MEDIA_EXPORT AudioManager {
public:
    virtual ~AudioManager();

    // This provides an alternative to the statically linked factory method used
    // to create AudioManager. This is useful for dynamically-linked third
    // party clients seeking to provide a platform-specific implementation of
    // AudioManager. After this is called, all static AudioManager::Create*
    // methods will return an instance of AudioManager provided by |factory|. This
    // call may be made at most once per process, and before any calls to static
    // AudioManager::Create* methods. This method takes ownership of |factory|,
    // which must not be NULL.
    static void SetFactory(AudioManagerFactory* factory);

    // Construct the audio manager; only one instance is allowed.  The manager
    // will forward CreateAudioLog() calls to the provided AudioLogFactory; as
    // such |audio_log_factory| must outlive the AudioManager.
    static AudioManager* Create(AudioLogFactory* audio_log_factory);

    // Similar to Create() except also schedules a monitor on the given task
    // runner to ensure the audio thread is not stuck for more than 60 seconds; if
    // a hang is detected, the process will be crashed.  See EnableHangMonitor().
    static AudioManager* CreateWithHangTimer(
        AudioLogFactory* audio_log_factory,
        const scoped_refptr<base::SingleThreadTaskRunner>& monitor_task_runner);

    // Similar to Create() except uses a FakeAudioLogFactory for testing.
    static AudioManager* CreateForTesting();

    // Enables the hang monitor for the AudioManager once it's created.  Must be
    // called before the AudioManager is created.  CreateWithHangTimer() requires
    // either switches::kEnableAudioHangMonitor to be present or this to have been
    // called previously to start the hang monitor.  Does nothing on OSX.
    static void EnableHangMonitor();

#if defined(OS_LINUX)
    // Sets the name of the audio source as seen by external apps. Only actually
    // used with PulseAudio as of this writing.
    static void SetGlobalAppName(const std::string& app_name);

    // Returns the app name or an empty string if it is not set.
    static const std::string& GetGlobalAppName();
#endif

    // Should only be used for testing. Resets a previously-set
    // AudioManagerFactory. The instance of AudioManager is not affected.
    static void ResetFactoryForTesting();

    // Returns the pointer to the last created instance, or NULL if not yet
    // created. This is a utility method for the code outside of media directory,
    // like src/chrome.
    static AudioManager* Get();

    // Returns the localized name of the generic "default" device.
    static std::string GetDefaultDeviceName();

    // Returns the localized name of the generic default communications device.
    // This device is not supported on all platforms.
    static std::string GetCommunicationsDeviceName();

    // Returns true if the OS reports existence of audio devices. This does not
    // guarantee that the existing devices support all formats and sample rates.
    virtual bool HasAudioOutputDevices() = 0;

    // Returns true if the OS reports existence of audio recording devices. This
    // does not guarantee that the existing devices support all formats and
    // sample rates.
    virtual bool HasAudioInputDevices() = 0;

    // Returns a human readable string for the model/make of the active audio
    // input device for this computer.
    virtual base::string16 GetAudioInputDeviceModel() = 0;

    // Opens the platform default audio input settings UI.
    // Note: This could invoke an external application/preferences pane, so
    // ideally must not be called from the UI thread or other time sensitive
    // threads to avoid blocking the rest of the application.
    virtual void ShowAudioInputSettings() = 0;

    // Appends a list of available input devices to |device_names|,
    // which must initially be empty. It is not guaranteed that all the
    // devices in the list support all formats and sample rates for
    // recording.
    //
    // Not threadsafe; in production this should only be called from the
    // Audio worker thread (see GetWorkerTaskRunner()).
    virtual void GetAudioInputDeviceNames(AudioDeviceNames* device_names) = 0;

    // Appends a list of available output devices to |device_names|,
    // which must initially be empty.
    //
    // Not threadsafe; in production this should only be called from the
    // Audio worker thread (see GetWorkerTaskRunner()).
    virtual void GetAudioOutputDeviceNames(AudioDeviceNames* device_names) = 0;

    // Factory for all the supported stream formats. |params| defines parameters
    // of the audio stream to be created.
    //
    // |params.sample_per_packet| is the requested buffer allocation which the
    // audio source thinks it can usually fill without blocking. Internally two
    // or three buffers are created, one will be locked for playback and one will
    // be ready to be filled in the call to AudioSourceCallback::OnMoreData().
    //
    // To create a stream for the default output device, pass an empty string
    // for |device_id|, otherwise the specified audio device will be opened.
    //
    // Returns NULL if the combination of the parameters is not supported, or if
    // we have reached some other platform specific limit.
    //
    // |params.format| can be set to AUDIO_PCM_LOW_LATENCY and that has two
    // effects:
    // 1- Instead of triple buffered the audio will be double buffered.
    // 2- A low latency driver or alternative audio subsystem will be used when
    //    available.
    //
    // Do not free the returned AudioOutputStream. It is owned by AudioManager.
    virtual AudioOutputStream* MakeAudioOutputStream(
        const AudioParameters& params,
        const std::string& device_id)
        = 0;

    // Creates new audio output proxy. A proxy implements
    // AudioOutputStream interface, but unlike regular output stream
    // created with MakeAudioOutputStream() it opens device only when a
    // sound is actually playing.
    virtual AudioOutputStream* MakeAudioOutputStreamProxy(
        const AudioParameters& params,
        const std::string& device_id)
        = 0;

    // Factory to create audio recording streams.
    // |channels| can be 1 or 2.
    // |sample_rate| is in hertz and can be any value supported by the platform.
    // |bits_per_sample| can be any value supported by the platform.
    // |samples_per_packet| is in hertz as well and can be 0 to |sample_rate|,
    // with 0 suggesting that the implementation use a default value for that
    // platform.
    // Returns NULL if the combination of the parameters is not supported, or if
    // we have reached some other platform specific limit.
    //
    // Do not free the returned AudioInputStream. It is owned by AudioManager.
    // When you are done with it, call |Stop()| and |Close()| to release it.
    virtual AudioInputStream* MakeAudioInputStream(
        const AudioParameters& params,
        const std::string& device_id)
        = 0;

    // Returns the task runner used for audio IO.
    virtual scoped_refptr<base::SingleThreadTaskRunner> GetTaskRunner() = 0;

    // Heavyweight tasks should use GetWorkerTaskRunner() instead of
    // GetTaskRunner(). On most platforms they are the same, but some share the
    // UI loop with the audio IO loop.
    virtual scoped_refptr<base::SingleThreadTaskRunner> GetWorkerTaskRunner() = 0;

    // Allows clients to listen for device state changes; e.g. preferred sample
    // rate or channel layout changes.  The typical response to receiving this
    // callback is to recreate the stream.
    class AudioDeviceListener {
    public:
        virtual void OnDeviceChange() = 0;
    };

    virtual void AddOutputDeviceChangeListener(AudioDeviceListener* listener) = 0;
    virtual void RemoveOutputDeviceChangeListener(
        AudioDeviceListener* listener)
        = 0;

    // Returns the default output hardware audio parameters for opening output
    // streams. It is a convenience interface to
    // AudioManagerBase::GetPreferredOutputStreamParameters and each AudioManager
    // does not need their own implementation to this interface.
    // TODO(tommi): Remove this method and use GetOutputStreamParameteres instead.
    virtual AudioParameters GetDefaultOutputStreamParameters() = 0;

    // Returns the output hardware audio parameters for a specific output device.
    virtual AudioParameters GetOutputStreamParameters(
        const std::string& device_id)
        = 0;

    // Returns the input hardware audio parameters of the specific device
    // for opening input streams. Each AudioManager needs to implement their own
    // version of this interface.
    virtual AudioParameters GetInputStreamParameters(
        const std::string& device_id)
        = 0;

    // Returns the device id of an output device that belongs to the same hardware
    // as the specified input device.
    // If the hardware has only an input device (e.g. a webcam), the return value
    // will be empty (which the caller can then interpret to be the default output
    // device).  Implementations that don't yet support this feature, must return
    // an empty string. Must be called on the audio worker thread (see
    // GetWorkerTaskRunner()).
    virtual std::string GetAssociatedOutputDeviceID(
        const std::string& input_device_id)
        = 0;

    // Create a new AudioLog object for tracking the behavior for one or more
    // instances of the given component.  See AudioLogFactory for more details.
    virtual scoped_ptr<AudioLog> CreateAudioLog(
        AudioLogFactory::AudioComponent component)
        = 0;

    // Informs the audio manager that the system has support for a keyboard mic.
    // This information will be passed on in the return value of
    // GetInputStreamParameters as an effect. Only supported on ChromeOS.
    virtual void SetHasKeyboardMic() = 0;

protected:
    AudioManager();

private:
    DISALLOW_COPY_AND_ASSIGN(AudioManager);
};

} // namespace media

#endif // MEDIA_AUDIO_AUDIO_MANAGER_H_
