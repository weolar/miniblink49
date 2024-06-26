// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_input_device.h"

#include "base/bind.h"
#include "base/memory/scoped_vector.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/time/time.h"
#include "media/audio/audio_manager_base.h"
#include "media/base/audio_bus.h"

namespace media {

// The number of shared memory buffer segments indicated to browser process
// in order to avoid data overwriting. This number can be any positive number,
// dependent how fast the renderer process can pick up captured data from
// shared memory.
static const int kRequestedSharedMemoryCount = 10;

// Takes care of invoking the capture callback on the audio thread.
// An instance of this class is created for each capture stream in
// OnLowLatencyCreated().
class AudioInputDevice::AudioThreadCallback
    : public AudioDeviceThread::Callback {
public:
    AudioThreadCallback(const AudioParameters& audio_parameters,
        base::SharedMemoryHandle memory,
        int memory_length,
        int total_segments,
        CaptureCallback* capture_callback);
    ~AudioThreadCallback() override;

    void MapSharedMemory() override;

    // Called whenever we receive notifications about pending data.
    void Process(uint32 pending_data) override;

private:
    int current_segment_id_;
    uint32 last_buffer_id_;
    ScopedVector<media::AudioBus> audio_buses_;
    CaptureCallback* capture_callback_;

    DISALLOW_COPY_AND_ASSIGN(AudioThreadCallback);
};

AudioInputDevice::AudioInputDevice(
    scoped_ptr<AudioInputIPC> ipc,
    const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner)
    : ScopedTaskRunnerObserver(io_task_runner)
    , callback_(NULL)
    , ipc_(ipc.Pass())
    , state_(IDLE)
    , session_id_(0)
    , agc_is_enabled_(false)
    , stopping_hack_(false)
{
    CHECK(ipc_);

    // The correctness of the code depends on the relative values assigned in the
    // State enum.
    static_assert(IPC_CLOSED < IDLE, "invalid enum value assignment 0");
    static_assert(IDLE < CREATING_STREAM, "invalid enum value assignment 1");
    static_assert(CREATING_STREAM < RECORDING, "invalid enum value assignment 2");
}

void AudioInputDevice::Initialize(const AudioParameters& params,
    CaptureCallback* callback,
    int session_id)
{
    DCHECK(params.IsValid());
    DCHECK(!callback_);
    DCHECK_EQ(0, session_id_);
    audio_parameters_ = params;
    callback_ = callback;
    session_id_ = session_id;
}

void AudioInputDevice::Start()
{
    DCHECK(callback_) << "Initialize hasn't been called";
    DVLOG(1) << "Start()";
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioInputDevice::StartUpOnIOThread, this));
}

void AudioInputDevice::Stop()
{
    DVLOG(1) << "Stop()";

    {
        base::AutoLock auto_lock(audio_thread_lock_);
        audio_thread_.Stop(base::MessageLoop::current());
        stopping_hack_ = true;
    }

    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioInputDevice::ShutDownOnIOThread, this));
}

void AudioInputDevice::SetVolume(double volume)
{
    if (volume < 0 || volume > 1.0) {
        DLOG(ERROR) << "Invalid volume value specified";
        return;
    }

    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioInputDevice::SetVolumeOnIOThread, this, volume));
}

void AudioInputDevice::SetAutomaticGainControl(bool enabled)
{
    DVLOG(1) << "SetAutomaticGainControl(enabled=" << enabled << ")";
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&AudioInputDevice::SetAutomaticGainControlOnIOThread,
            this, enabled));
}

void AudioInputDevice::OnStreamCreated(
    base::SharedMemoryHandle handle,
    base::SyncSocket::Handle socket_handle,
    int length,
    int total_segments)
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

    base::AutoLock auto_lock(audio_thread_lock_);
    // TODO(miu): See TODO in OnStreamCreated method for AudioOutputDevice.
    // Interface changes need to be made; likely, after AudioInputDevice is merged
    // into AudioOutputDevice (http://crbug.com/179597).
    if (stopping_hack_)
        return;

    DCHECK(audio_thread_.IsStopped());
    audio_callback_.reset(new AudioInputDevice::AudioThreadCallback(
        audio_parameters_, handle, length, total_segments, callback_));
    audio_thread_.Start(
        audio_callback_.get(), socket_handle, "AudioInputDevice", true);

    state_ = RECORDING;
    ipc_->RecordStream();
}

void AudioInputDevice::OnVolume(double volume)
{
    NOTIMPLEMENTED();
}

void AudioInputDevice::OnStateChanged(
    AudioInputIPCDelegateState state)
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    // Do nothing if the stream has been closed.
    if (state_ < CREATING_STREAM)
        return;

    // TODO(miu): Clean-up inconsistent and incomplete handling here.
    // http://crbug.com/180640
    switch (state) {
    case AUDIO_INPUT_IPC_DELEGATE_STATE_STOPPED:
        ShutDownOnIOThread();
        break;
    case AUDIO_INPUT_IPC_DELEGATE_STATE_RECORDING:
        NOTIMPLEMENTED();
        break;
    case AUDIO_INPUT_IPC_DELEGATE_STATE_ERROR:
        DLOG(WARNING) << "AudioInputDevice::OnStateChanged(ERROR)";
        // Don't dereference the callback object if the audio thread
        // is stopped or stopping.  That could mean that the callback
        // object has been deleted.
        // TODO(tommi): Add an explicit contract for clearing the callback
        // object.  Possibly require calling Initialize again or provide
        // a callback object via Start() and clear it in Stop().
        if (!audio_thread_.IsStopped())
            callback_->OnCaptureError(
                "AudioInputDevice::OnStateChanged - audio thread still running");
        break;
    default:
        NOTREACHED();
        break;
    }
}

void AudioInputDevice::OnIPCClosed()
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    state_ = IPC_CLOSED;
    ipc_.reset();
}

AudioInputDevice::~AudioInputDevice()
{
    // TODO(henrika): The current design requires that the user calls
    // Stop before deleting this class.
    DCHECK(audio_thread_.IsStopped());
}

void AudioInputDevice::StartUpOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    // Make sure we don't call Start() more than once.
    if (state_ != IDLE)
        return;

    if (session_id_ <= 0) {
        DLOG(WARNING) << "Invalid session id for the input stream " << session_id_;
        return;
    }

    state_ = CREATING_STREAM;
    ipc_->CreateStream(this, session_id_, audio_parameters_,
        agc_is_enabled_, kRequestedSharedMemoryCount);
}

void AudioInputDevice::ShutDownOnIOThread()
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    // Close the stream, if we haven't already.
    if (state_ >= CREATING_STREAM) {
        ipc_->CloseStream();
        state_ = IDLE;
        agc_is_enabled_ = false;
    }

    // We can run into an issue where ShutDownOnIOThread is called right after
    // OnStreamCreated is called in cases where Start/Stop are called before we
    // get the OnStreamCreated callback.  To handle that corner case, we call
    // Stop(). In most cases, the thread will already be stopped.
    //
    // Another situation is when the IO thread goes away before Stop() is called
    // in which case, we cannot use the message loop to close the thread handle
    // and can't not rely on the main thread existing either.
    base::AutoLock auto_lock_(audio_thread_lock_);
    base::ThreadRestrictions::ScopedAllowIO allow_io;
    audio_thread_.Stop(NULL);
    audio_callback_.reset();
    stopping_hack_ = false;
}

void AudioInputDevice::SetVolumeOnIOThread(double volume)
{
    DCHECK(task_runner()->BelongsToCurrentThread());
    if (state_ >= CREATING_STREAM)
        ipc_->SetVolume(volume);
}

void AudioInputDevice::SetAutomaticGainControlOnIOThread(bool enabled)
{
    DCHECK(task_runner()->BelongsToCurrentThread());

    if (state_ >= CREATING_STREAM) {
        DLOG(WARNING) << "The AGC state can not be modified after starting.";
        return;
    }

    // We simply store the new AGC setting here. This value will be used when
    // a new stream is initialized and by GetAutomaticGainControl().
    agc_is_enabled_ = enabled;
}

void AudioInputDevice::WillDestroyCurrentMessageLoop()
{
    LOG(ERROR) << "IO loop going away before the input device has been stopped";
    ShutDownOnIOThread();
}

// AudioInputDevice::AudioThreadCallback
AudioInputDevice::AudioThreadCallback::AudioThreadCallback(
    const AudioParameters& audio_parameters,
    base::SharedMemoryHandle memory,
    int memory_length,
    int total_segments,
    CaptureCallback* capture_callback)
    : AudioDeviceThread::Callback(audio_parameters, memory, memory_length,
        total_segments)
    , current_segment_id_(0)
    , last_buffer_id_(UINT32_MAX)
    , capture_callback_(capture_callback)
{
}

AudioInputDevice::AudioThreadCallback::~AudioThreadCallback()
{
}

void AudioInputDevice::AudioThreadCallback::MapSharedMemory()
{
    shared_memory_.Map(memory_length_);

    // Create vector of audio buses by wrapping existing blocks of memory.
    uint8* ptr = static_cast<uint8*>(shared_memory_.memory());
    for (int i = 0; i < total_segments_; ++i) {
        media::AudioInputBuffer* buffer = reinterpret_cast<media::AudioInputBuffer*>(ptr);
        scoped_ptr<media::AudioBus> audio_bus = media::AudioBus::WrapMemory(audio_parameters_, buffer->audio);
        audio_buses_.push_back(audio_bus.Pass());
        ptr += segment_length_;
    }
}

void AudioInputDevice::AudioThreadCallback::Process(uint32 pending_data)
{
    // The shared memory represents parameters, size of the data buffer and the
    // actual data buffer containing audio data. Map the memory into this
    // structure and parse out parameters and the data area.
    uint8* ptr = static_cast<uint8*>(shared_memory_.memory());
    ptr += current_segment_id_ * segment_length_;
    AudioInputBuffer* buffer = reinterpret_cast<AudioInputBuffer*>(ptr);

    // Usually this will be equal but in the case of low sample rate (e.g. 8kHz,
    // the buffer may be bigger (on mac at least)).
    DCHECK_GE(buffer->params.size,
        segment_length_ - sizeof(AudioInputBufferParameters));

    // Verify correct sequence.
    if (buffer->params.id != last_buffer_id_ + 1) {
        std::string message = base::StringPrintf(
            "Incorrect buffer sequence. Expected = %u. Actual = %u.",
            last_buffer_id_ + 1, buffer->params.id);
        LOG(ERROR) << message;
        capture_callback_->OnCaptureError(message);
    }
    if (current_segment_id_ != static_cast<int>(pending_data)) {
        std::string message = base::StringPrintf(
            "Segment id not matching. Remote = %u. Local = %d.",
            pending_data, current_segment_id_);
        LOG(ERROR) << message;
        capture_callback_->OnCaptureError(message);
    }
    last_buffer_id_ = buffer->params.id;

    // Use pre-allocated audio bus wrapping existing block of shared memory.
    media::AudioBus* audio_bus = audio_buses_[current_segment_id_];

    // Deliver captured data to the client in floating point format and update
    // the audio delay measurement.
    capture_callback_->Capture(
        audio_bus,
        buffer->params.hardware_delay_bytes / bytes_per_ms_, // Delay in ms
        buffer->params.volume,
        buffer->params.key_pressed);

    if (++current_segment_id_ >= total_segments_)
        current_segment_id_ = 0;
}

} // namespace media
