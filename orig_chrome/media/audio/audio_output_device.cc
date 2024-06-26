// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_output_device.h"

#include "base/callback_helpers.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/audio/audio_output_controller.h"
#include "media/base/limits.h"

namespace media {

// Takes care of invoking the render callback on the audio thread.
// An instance of this class is created for each capture stream in
// OnStreamCreated().
class AudioOutputDevice::AudioThreadCallback
    : public AudioDeviceThread::Callback {
public:
    AudioThreadCallback(const AudioParameters& audio_parameters,
        base::SharedMemoryHandle memory,
        int memory_length,
        AudioRendererSink::RenderCallback* render_callback);
    ~AudioThreadCallback() override;

    void MapSharedMemory() override;

    // Called whenever we receive notifications about pending data.
    void Process(uint32 pending_data) override;

private:
    AudioRendererSink::RenderCallback* render_callback_;
    scoped_ptr<AudioBus> output_bus_;
    uint64 callback_num_;
    DISALLOW_COPY_AND_ASSIGN(AudioThreadCallback);
};

AudioOutputDevice::AudioOutputDevice(
    scoped_ptr<AudioOutputIPC> ipc,
    const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner,
    int session_id,
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin)
    : ScopedTaskRunnerObserver(io_task_runner)
    , callback_(NULL)
    , ipc_(ipc.Pass())
    , state_(IDLE)
    , start_on_authorized_(false)
    , play_on_start_(true)
    , session_id_(session_id)
    , device_id_(device_id)
    , security_origin_(security_origin)
    , stopping_hack_(false)
    , did_receive_auth_(true, false)
    , device_status_(OUTPUT_DEVICE_STATUS_ERROR_INTERNAL)
{
    CHECK(ipc_);

    // The correctness of the code depends on the relative values assigned in the
    // State enum.
    static_assert(IPC_CLOSED < IDLE, "invalid enum value assignment 0");
    static_assert(IDLE < AUTHORIZING, "invalid enum value assignment 1");
    static_assert(AUTHORIZING < AUTHORIZED, "invalid enum value assignment 2");
    static_assert(AUTHORIZED < CREATING_STREAM,
        "invalid enum value assignment 3");
    static_assert(CREATING_STREAM < PAUSED, "invalid enum value assignment 4");
    static_assert(PAUSED < PLAYING, "invalid enum value assignment 5");
}

void AudioOutputDevice::Initialize(const AudioParameters& params,
    RenderCallback* callback)
{
    DCHECK(!callback_) << "Calling Initialize() twice?";
    DCHECK(params.IsValid());
    audio_parameters_ = params;
    callback_ = callback;
}

AudioOutputDevice::~AudioOutputDevice()
{
    // The current design requires that the user calls Stop() before deleting
    // this class.
    DCHECK(audio_thread_.IsStopped());
}

void AudioOutputDevice::RequestDeviceAuthorization()
{
    task_runner()->PostTask(
        FROM_HERE,
        base::Bind(&AudioOutputDevice::RequestDeviceAuthorizationOnIOThread,
            this));
}

void AudioOutputDevice::Start()
{
    DCHECK(callback_) << "Initialize hasn't been called";
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioOutputDevice::CreateStreamOnIOThread, this,
            audio_parameters_));
}

void AudioOutputDevice::Stop()
{
    {
        base::AutoLock auto_lock(audio_thread_lock_);
        audio_thread_.Stop(base::MessageLoop::current());
        stopping_hack_ = true;
    }

    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioOutputDevice::ShutDownOnIOThread, this));
}

void AudioOutputDevice::Play()
{
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioOutputDevice::PlayOnIOThread, this));
}

void AudioOutputDevice::Pause()
{
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioOutputDevice::PauseOnIOThread, this));
}

bool AudioOutputDevice::SetVolume(double volume)
{
    if (volume < 0 || volume > 1.0)
        return false;

    if (!task_runner()->PostTask(FROM_HERE,
            base::Bind(&AudioOutputDevice::SetVolumeOnIOThread, this, volume))) {
        return false;
    }

    return true;
}

OutputDevice* AudioOutputDevice::GetOutputDevice()
{
    return this;
}

void AudioOutputDevice::SwitchOutputDevice(
    const std::string& device_id,
    const /*url::Origin*/ std::string& security_origin,
    const SwitchOutputDeviceCB& callback)
{
    NOTREACHED();
}

AudioParameters AudioOutputDevice::GetOutputParameters()
{
    CHECK(!task_runner()->BelongsToCurrentThread());
    did_receive_auth_.Wait();
    return output_params_;
}

OutputDeviceStatus AudioOutputDevice::GetDeviceStatus()
{
    CHECK(!task_runner()->BelongsToCurrentThread());
    did_receive_auth_.Wait();
    return device_status_;
}

void AudioOutputDevice::RequestDeviceAuthorizationOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    DCHECK_EQ(state_, IDLE);
    state_ = AUTHORIZING;
    ipc_->RequestDeviceAuthorization(this, session_id_, device_id_,
        security_origin_);
}

void AudioOutputDevice::CreateStreamOnIOThread(const AudioParameters& params)
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    switch (state_) {
    case IPC_CLOSED:
        if (callback_)
            callback_->OnRenderError();
        break;

    case IDLE:
        DebugBreak();
        if (did_receive_auth_.IsSignaled() && device_id_.empty() && security_origin_./*unique*/ c_str()) {
            state_ = CREATING_STREAM;
            ipc_->CreateStream(this, params);
        } else {
            RequestDeviceAuthorizationOnIOThread();
            start_on_authorized_ = true;
        }
        break;

    case AUTHORIZING:
        start_on_authorized_ = true;
        break;

    case AUTHORIZED:
        state_ = CREATING_STREAM;
        ipc_->CreateStream(this, params);
        start_on_authorized_ = false;
        break;

    case CREATING_STREAM:
    case PAUSED:
    case PLAYING:
        NOTREACHED();
        break;
    }
}

void AudioOutputDevice::PlayOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    if (state_ == PAUSED) {
        TRACE_EVENT_ASYNC_BEGIN0(
            "audio", "StartingPlayback", audio_callback_.get());
        ipc_->PlayStream();
        state_ = PLAYING;
        play_on_start_ = false;
    } else {
        play_on_start_ = true;
    }
}

void AudioOutputDevice::PauseOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    if (state_ == PLAYING) {
        TRACE_EVENT_ASYNC_END0(
            "audio", "StartingPlayback", audio_callback_.get());
        ipc_->PauseStream();
        state_ = PAUSED;
    }
    play_on_start_ = false;
}

void AudioOutputDevice::ShutDownOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    // Close the stream, if we haven't already.
    if (state_ >= AUTHORIZING) {
        ipc_->CloseStream();
        state_ = IDLE;
    }
    start_on_authorized_ = false;

    // We can run into an issue where ShutDownOnIOThread is called right after
    // OnStreamCreated is called in cases where Start/Stop are called before we
    // get the OnStreamCreated callback.  To handle that corner case, we call
    // Stop(). In most cases, the thread will already be stopped.
    //
    // Another situation is when the IO thread goes away before Stop() is called
    // in which case, we cannot use the message loop to close the thread handle
    // and can't rely on the main thread existing either.
    base::AutoLock auto_lock_(audio_thread_lock_);
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    audio_thread_.Stop(NULL);
    audio_callback_.reset();
    stopping_hack_ = false;
}

void AudioOutputDevice::SetVolumeOnIOThread(double volume)
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    if (state_ >= CREATING_STREAM)
        ipc_->SetVolume(volume);
}

void AudioOutputDevice::OnStateChanged(AudioOutputIPCDelegateState state)
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    // Do nothing if the stream has been closed.
    if (state_ < CREATING_STREAM)
        return;

    // TODO(miu): Clean-up inconsistent and incomplete handling here.
    // http://crbug.com/180640
    switch (state) {
    case AUDIO_OUTPUT_IPC_DELEGATE_STATE_PLAYING:
    case AUDIO_OUTPUT_IPC_DELEGATE_STATE_PAUSED:
        break;
    case AUDIO_OUTPUT_IPC_DELEGATE_STATE_ERROR:
        DLOG(WARNING) << "AudioOutputDevice::OnStateChanged(ERROR)";
        // Don't dereference the callback object if the audio thread
        // is stopped or stopping.  That could mean that the callback
        // object has been deleted.
        // TODO(tommi): Add an explicit contract for clearing the callback
        // object.  Possibly require calling Initialize again or provide
        // a callback object via Start() and clear it in Stop().
        if (!audio_thread_.IsStopped())
            callback_->OnRenderError();
        break;
    default:
        NOTREACHED();
        break;
    }
}

void AudioOutputDevice::OnDeviceAuthorized(
    OutputDeviceStatus device_status,
    const media::AudioParameters& output_params)
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    DCHECK_EQ(state_, AUTHORIZING);

    // It may happen that a second authorization is received as a result to a
    // call to Start() after Stop(). If the status for the second authorization
    // differs from the first, it will not be reflected in |device_status_|
    // to avoid a race.
    // This scenario is unlikely. If it occurs, the new value will be
    // different from OUTPUT_DEVICE_STATUS_OK, so the AudioOutputDevice
    // will enter the IPC_CLOSED state anyway, which is the safe thing to do.
    // This is preferable to holding a lock.
    if (!did_receive_auth_.IsSignaled())
        device_status_ = device_status;

    if (device_status == OUTPUT_DEVICE_STATUS_OK) {
        state_ = AUTHORIZED;
        if (!did_receive_auth_.IsSignaled()) {
            output_params_ = output_params;
            did_receive_auth_.Signal();
        }
        if (start_on_authorized_)
            CreateStreamOnIOThread(audio_parameters_);
    } else {
        // Closing IPC forces a Signal(), so no clients are locked waiting
        // indefinitely after this method returns.
        ipc_->CloseStream();
        OnIPCClosed();
        if (callback_)
            callback_->OnRenderError();
    }
}

void AudioOutputDevice::OnStreamCreated(
    base::SharedMemoryHandle handle,
    base::SyncSocket::Handle socket_handle,
    int length)
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    DCHECK(base::SharedMemory::IsHandleValid(handle));
#if defined(OS_WIN)
    DCHECK(socket_handle);
#else
    DCHECK_GE(socket_handle, 0);
#endif
    DCHECK_GT(length, 0);

    if (state_ != CREATING_STREAM)
        return;

    // We can receive OnStreamCreated() on the IO thread after the client has
    // called Stop() but before ShutDownOnIOThread() is processed. In such a
    // situation |callback_| might point to freed memory. Instead of starting
    // |audio_thread_| do nothing and wait for ShutDownOnIOThread() to get called.
    //
    // TODO(scherkus): The real fix is to have sane ownership semantics. The fact
    // that |callback_| (which should own and outlive this object!) can point to
    // freed memory is a mess. AudioRendererSink should be non-refcounted so that
    // owners (WebRtcAudioDeviceImpl, AudioRendererImpl, etc...) can Stop() and
    // delete as they see fit. AudioOutputDevice should internally use WeakPtr
    // to handle teardown and thread hopping. See http://crbug.com/151051 for
    // details.
    {
        base::AutoLock auto_lock(audio_thread_lock_);
        if (stopping_hack_)
            return;

        DCHECK(audio_thread_.IsStopped());
        audio_callback_.reset(new AudioOutputDevice::AudioThreadCallback(
            audio_parameters_, handle, length, callback_));
        audio_thread_.Start(audio_callback_.get(), socket_handle,
            "AudioOutputDevice", true);
        state_ = PAUSED;

        // We handle the case where Play() and/or Pause() may have been called
        // multiple times before OnStreamCreated() gets called.
        if (play_on_start_)
            PlayOnIOThread();
    }
}

void AudioOutputDevice::OnIPCClosed()
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    state_ = IPC_CLOSED;
    ipc_.reset();

    // Signal to unblock any blocked threads waiting for parameters
    did_receive_auth_.Signal();
}

void AudioOutputDevice::WillDestroyCurrentMessageLoop()
{
    LOG(ERROR) << "IO loop going away before the audio device has been stopped";
    ShutDownOnIOThread();
}

// AudioOutputDevice::AudioThreadCallback

AudioOutputDevice::AudioThreadCallback::AudioThreadCallback(
    const AudioParameters& audio_parameters,
    base::SharedMemoryHandle memory,
    int memory_length,
    AudioRendererSink::RenderCallback* render_callback)
    : AudioDeviceThread::Callback(audio_parameters, memory, memory_length, 1)
    , render_callback_(render_callback)
    , callback_num_(0)
{
}

AudioOutputDevice::AudioThreadCallback::~AudioThreadCallback()
{
}

void AudioOutputDevice::AudioThreadCallback::MapSharedMemory()
{
    CHECK_EQ(total_segments_, 1);
    CHECK(shared_memory_.Map(memory_length_));
    size_t audio_parameters_size = AudioBus::CalculateMemorySize(audio_parameters_);
    DCHECK_EQ(memory_length_, audio_parameters_size);

    output_bus_ = AudioBus::WrapMemory(audio_parameters_, shared_memory_.memory());
}

// Called whenever we receive notifications about pending data.
void AudioOutputDevice::AudioThreadCallback::Process(uint32 pending_data)
{
    // Convert the number of pending bytes in the render buffer into milliseconds.
    int audio_delay_milliseconds = pending_data / bytes_per_ms_;

    callback_num_++;
    TRACE_EVENT1("audio", "AudioOutputDevice::FireRenderCallback",
        "callback_num", callback_num_);

    // When playback starts, we get an immediate callback to Process to make sure
    // that we have some data, we'll get another one after the device is awake and
    // ingesting data, which is what we want to track with this trace.
    if (callback_num_ == 2) {
        TRACE_EVENT_ASYNC_END0("audio", "StartingPlayback", this);
    }

    // Update the audio-delay measurement then ask client to render audio.  Since
    // |output_bus_| is wrapping the shared memory the Render() call is writing
    // directly into the shared memory.
    render_callback_->Render(output_bus_.get(), audio_delay_milliseconds);
}

} // namespace media.
