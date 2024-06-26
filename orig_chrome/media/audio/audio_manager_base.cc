// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_manager_base.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/command_line.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "base/thread_task_runner_handle.h"
#include "build/build_config.h"
#include "media/audio/audio_output_dispatcher_impl.h"
#include "media/audio/audio_output_proxy.h"
#include "media/audio/audio_output_resampler.h"
#include "media/audio/fake_audio_input_stream.h"
#include "media/audio/fake_audio_output_stream.h"
#include "media/base/media_switches.h"

namespace media {
namespace {

    const int kStreamCloseDelaySeconds = 5;

    // Default maximum number of output streams that can be open simultaneously
    // for all platforms.
    const int kDefaultMaxOutputStreams = 16;

    // Default maximum number of input streams that can be open simultaneously
    // for all platforms.
    const int kDefaultMaxInputStreams = 16;

    const int kMaxInputChannels = 3;

} // namespace

const char AudioManagerBase::kDefaultDeviceId[] = "default";
const char AudioManagerBase::kCommunicationsDeviceId[] = "communications";
const char AudioManagerBase::kLoopbackInputDeviceId[] = "loopback";

struct AudioManagerBase::DispatcherParams {
    DispatcherParams(const AudioParameters& input,
        const AudioParameters& output,
        const std::string& output_device_id)
        : input_params(input)
        , output_params(output)
        , output_device_id(output_device_id)
    {
    }
    ~DispatcherParams() { }

    const AudioParameters input_params;
    const AudioParameters output_params;
    const std::string output_device_id;
    scoped_refptr<AudioOutputDispatcher> dispatcher;

private:
    DISALLOW_COPY_AND_ASSIGN(DispatcherParams);
};

class AudioManagerBase::CompareByParams {
public:
    explicit CompareByParams(const DispatcherParams* dispatcher)
        : dispatcher_(dispatcher)
    {
    }
    bool operator()(DispatcherParams* dispatcher_in) const
    {
        // We will reuse the existing dispatcher when:
        // 1) Unified IO is not used, input_params and output_params of the
        //    existing dispatcher are the same as the requested dispatcher.
        // 2) Unified IO is used, input_params and output_params of the existing
        //    dispatcher are the same as the request dispatcher.
        return (dispatcher_->input_params.Equals(dispatcher_in->input_params) && dispatcher_->output_params.Equals(dispatcher_in->output_params) && dispatcher_->output_device_id == dispatcher_in->output_device_id);
    }

private:
    const DispatcherParams* dispatcher_;
};

static bool IsDefaultDeviceId(const std::string& device_id)
{
    return device_id.empty() || device_id == AudioManagerBase::kDefaultDeviceId;
}

AudioManagerBase::AudioManagerBase(AudioLogFactory* audio_log_factory)
    : max_num_output_streams_(kDefaultMaxOutputStreams)
    , max_num_input_streams_(kDefaultMaxInputStreams)
    , num_output_streams_(0)
    , num_input_streams_(0)
    ,
    // TODO(dalecurtis): Switch this to an base::ObserverListThreadSafe, so we
    // don't
    // block the UI thread when swapping devices.
    output_listeners_(
        base::ObserverList<AudioDeviceListener>::NOTIFY_EXISTING_ONLY)
    , audio_thread_("AudioThread")
    , audio_log_factory_(audio_log_factory)
{
#if defined(OS_WIN)
    audio_thread_.init_com_with_mta(true);
#elif defined(OS_MACOSX)
    // CoreAudio calls must occur on the main thread of the process, which in our
    // case is sadly the browser UI thread.  Failure to execute calls on the right
    // thread leads to crashes and odd behavior.  See http://crbug.com/158170.
    // TODO(dalecurtis): We should require the message loop to be passed in.
    if (base::MessageLoopForUI::IsCurrent()) {
        task_runner_ = base::ThreadTaskRunnerHandle::Get();
        return;
    }
#endif

    CHECK(audio_thread_.Start());
    task_runner_ = audio_thread_.task_runner();
}

AudioManagerBase::~AudioManagerBase()
{
    // The platform specific AudioManager implementation must have already
    // stopped the audio thread. Otherwise, we may destroy audio streams before
    // stopping the thread, resulting an unexpected behavior.
    // This way we make sure activities of the audio streams are all stopped
    // before we destroy them.
    CHECK(!audio_thread_.IsRunning());
    // All the output streams should have been deleted.
    DCHECK_EQ(0, num_output_streams_);
    // All the input streams should have been deleted.
    DCHECK_EQ(0, num_input_streams_);
}

base::string16 AudioManagerBase::GetAudioInputDeviceModel()
{
    return base::string16();
}

scoped_refptr<base::SingleThreadTaskRunner> AudioManagerBase::GetTaskRunner()
{
    return task_runner_;
}

scoped_refptr<base::SingleThreadTaskRunner>
AudioManagerBase::GetWorkerTaskRunner()
{
    // Lazily start the worker thread.
    if (!audio_thread_.IsRunning())
        CHECK(audio_thread_.Start());

    return audio_thread_.task_runner();
}

AudioOutputStream* AudioManagerBase::MakeAudioOutputStream(
    const AudioParameters& params,
    const std::string& device_id)
{
    // TODO(miu): Fix ~50 call points across several unit test modules to call
    // this method on the audio thread, then uncomment the following:
    // DCHECK(task_runner_->BelongsToCurrentThread());

    if (!params.IsValid()) {
        DLOG(ERROR) << "Audio parameters are invalid";
        return NULL;
    }

    // Limit the number of audio streams opened. This is to prevent using
    // excessive resources for a large number of audio streams. More
    // importantly it prevents instability on certain systems.
    // See bug: http://crbug.com/30242.
    if (num_output_streams_ >= max_num_output_streams_) {
        DLOG(ERROR) << "Number of opened output audio streams "
                    << num_output_streams_
                    << " exceed the max allowed number "
                    << max_num_output_streams_;
        return NULL;
    }

    AudioOutputStream* stream;
    switch (params.format()) {
    case AudioParameters::AUDIO_PCM_LINEAR:
        DCHECK(IsDefaultDeviceId(device_id))
            << "AUDIO_PCM_LINEAR supports only the default device.";
        stream = MakeLinearOutputStream(params);
        break;
    case AudioParameters::AUDIO_PCM_LOW_LATENCY:
        stream = MakeLowLatencyOutputStream(params, device_id);
        break;
    case AudioParameters::AUDIO_FAKE:
        stream = FakeAudioOutputStream::MakeFakeStream(this, params);
        break;
    default:
        stream = NULL;
        break;
    }

    if (stream) {
        ++num_output_streams_;
    }

    return stream;
}

AudioInputStream* AudioManagerBase::MakeAudioInputStream(
    const AudioParameters& params,
    const std::string& device_id)
{
    // TODO(miu): Fix ~20 call points across several unit test modules to call
    // this method on the audio thread, then uncomment the following:
    // DCHECK(task_runner_->BelongsToCurrentThread());

    if (!params.IsValid() || (params.channels() > kMaxInputChannels) || device_id.empty()) {
        DLOG(ERROR) << "Audio parameters are invalid for device " << device_id;
        return NULL;
    }

    if (num_input_streams_ >= max_num_input_streams_) {
        DLOG(ERROR) << "Number of opened input audio streams "
                    << num_input_streams_
                    << " exceed the max allowed number " << max_num_input_streams_;
        return NULL;
    }

    DVLOG(2) << "Creating a new AudioInputStream with buffer size = "
             << params.frames_per_buffer();

    AudioInputStream* stream;
    switch (params.format()) {
    case AudioParameters::AUDIO_PCM_LINEAR:
        stream = MakeLinearInputStream(params, device_id);
        break;
    case AudioParameters::AUDIO_PCM_LOW_LATENCY:
        stream = MakeLowLatencyInputStream(params, device_id);
        break;
    case AudioParameters::AUDIO_FAKE:
        stream = FakeAudioInputStream::MakeFakeStream(this, params);
        break;
    default:
        stream = NULL;
        break;
    }

    if (stream) {
        ++num_input_streams_;
    }

    return stream;
}

AudioOutputStream* AudioManagerBase::MakeAudioOutputStreamProxy(
    const AudioParameters& params,
    const std::string& device_id)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // If the caller supplied an empty device id to select the default device,
    // we fetch the actual device id of the default device so that the lookup
    // will find the correct device regardless of whether it was opened as
    // "default" or via the specific id.
    // NOTE: Implementations that don't yet support opening non-default output
    // devices may return an empty string from GetDefaultOutputDeviceID().
    std::string output_device_id = IsDefaultDeviceId(device_id) ? GetDefaultOutputDeviceID() : device_id;

    // If we're not using AudioOutputResampler our output parameters are the same
    // as our input parameters.
    AudioParameters output_params = params;
    if (params.format() == AudioParameters::AUDIO_PCM_LOW_LATENCY) {
        output_params = GetPreferredOutputStreamParameters(output_device_id, params);

        // Ensure we only pass on valid output parameters.
        if (!output_params.IsValid()) {
            // We've received invalid audio output parameters, so switch to a mock
            // output device based on the input parameters.  This may happen if the OS
            // provided us junk values for the hardware configuration.
            LOG(ERROR) << "Invalid audio output parameters received; using fake "
                       << "audio path. Channels: " << output_params.channels() << ", "
                       << "Sample Rate: " << output_params.sample_rate() << ", "
                       << "Bits Per Sample: " << output_params.bits_per_sample()
                       << ", Frames Per Buffer: "
                       << output_params.frames_per_buffer();

            // Tell the AudioManager to create a fake output device.
            output_params = params;
            output_params.set_format(AudioParameters::AUDIO_FAKE);
        } else if (params.effects() != output_params.effects()) {
            // Turn off effects that weren't requested.
            output_params.set_effects(params.effects() & output_params.effects());
        }
    }

    DispatcherParams* dispatcher_params = new DispatcherParams(params, output_params, output_device_id);

    AudioOutputDispatchers::iterator it = std::find_if(output_dispatchers_.begin(), output_dispatchers_.end(),
        CompareByParams(dispatcher_params));
    if (it != output_dispatchers_.end()) {
        delete dispatcher_params;
        return new AudioOutputProxy((*it)->dispatcher.get());
    }

    const base::TimeDelta kCloseDelay = base::TimeDelta::FromSeconds(kStreamCloseDelaySeconds);
    scoped_refptr<AudioOutputDispatcher> dispatcher;
    if (output_params.format() != AudioParameters::AUDIO_FAKE) {
        dispatcher = new AudioOutputResampler(this, params, output_params,
            output_device_id,
            kCloseDelay);
    } else {
        dispatcher = new AudioOutputDispatcherImpl(this, output_params,
            output_device_id,
            kCloseDelay);
    }

    dispatcher_params->dispatcher = dispatcher;
    output_dispatchers_.push_back(dispatcher_params);
    return new AudioOutputProxy(dispatcher.get());
}

void AudioManagerBase::ShowAudioInputSettings()
{
}

void AudioManagerBase::GetAudioInputDeviceNames(
    AudioDeviceNames* device_names)
{
}

void AudioManagerBase::GetAudioOutputDeviceNames(
    AudioDeviceNames* device_names)
{
}

void AudioManagerBase::ReleaseOutputStream(AudioOutputStream* stream)
{
    DCHECK(stream);
    // TODO(xians) : Have a clearer destruction path for the AudioOutputStream.
    // For example, pass the ownership to AudioManager so it can delete the
    // streams.
    --num_output_streams_;
    delete stream;
}

void AudioManagerBase::ReleaseInputStream(AudioInputStream* stream)
{
    DCHECK(stream);
    // TODO(xians) : Have a clearer destruction path for the AudioInputStream.
    --num_input_streams_;
    delete stream;
}

void AudioManagerBase::Shutdown()
{
    // Only true when we're sharing the UI message loop with the browser.  The UI
    // loop is no longer running at this time and browser destruction is imminent.
    if (task_runner_->BelongsToCurrentThread()) {
        ShutdownOnAudioThread();
    } else {
        task_runner_->PostTask(FROM_HERE, base::Bind(&AudioManagerBase::ShutdownOnAudioThread, base::Unretained(this)));
    }

    // Stop() will wait for any posted messages to be processed first.
    audio_thread_.Stop();
}

void AudioManagerBase::ShutdownOnAudioThread()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    while (!output_dispatchers_.empty()) {
        output_dispatchers_.back()->dispatcher->Shutdown();
        output_dispatchers_.pop_back();
    }
}

void AudioManagerBase::AddOutputDeviceChangeListener(
    AudioDeviceListener* listener)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    output_listeners_.AddObserver(listener);
}

void AudioManagerBase::RemoveOutputDeviceChangeListener(
    AudioDeviceListener* listener)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    output_listeners_.RemoveObserver(listener);
}

void AudioManagerBase::NotifyAllOutputDeviceChangeListeners()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DVLOG(1) << "Firing OnDeviceChange() notifications.";
    FOR_EACH_OBSERVER(AudioDeviceListener, output_listeners_, OnDeviceChange());
}

AudioParameters AudioManagerBase::GetDefaultOutputStreamParameters()
{
    return GetPreferredOutputStreamParameters(GetDefaultOutputDeviceID(),
        AudioParameters());
}

AudioParameters AudioManagerBase::GetOutputStreamParameters(
    const std::string& device_id)
{
    return GetPreferredOutputStreamParameters(device_id,
        AudioParameters());
}

AudioParameters AudioManagerBase::GetInputStreamParameters(
    const std::string& device_id)
{
    NOTREACHED();
    return AudioParameters();
}

std::string AudioManagerBase::GetAssociatedOutputDeviceID(
    const std::string& input_device_id)
{
    return "";
}

std::string AudioManagerBase::GetDefaultOutputDeviceID()
{
    return "";
}

int AudioManagerBase::GetUserBufferSize()
{
    const base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
    int buffer_size = 0;
    std::string buffer_size_str(cmd_line->GetSwitchValueASCII(
        switches::kAudioBufferSize));
    if (base::StringToInt(buffer_size_str, &buffer_size) && buffer_size > 0)
        return buffer_size;

    return 0;
}

scoped_ptr<AudioLog> AudioManagerBase::CreateAudioLog(
    AudioLogFactory::AudioComponent component)
{
    return audio_log_factory_->CreateAudioLog(component);
}

void AudioManagerBase::SetHasKeyboardMic()
{
    NOTREACHED();
}

} // namespace media
