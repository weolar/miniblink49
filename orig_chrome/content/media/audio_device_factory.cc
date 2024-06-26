// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/media/audio_device_factory.h"

#include "base/logging.h"
// #include "content/renderer/media/audio_input_message_filter.h"
// #include "content/renderer/media/audio_message_filter.h"
#include "media/audio/audio_input_device.h"
#include "media/audio/audio_manager_base.h"
#include "media/audio/audio_output_controller.h"
#include "media/audio/audio_output_device.h"
#include "media/base/audio_bus.h"
#include "media/base/output_device.h"
//#include "url/origin.h"
#include "base/memory/shared_memory.h"
#include "base/process/process.h"
#include "base/threading/thread.h"
#include "content/OrigChromeMgr.h"

namespace content {

namespace {
    const int kStreamIDNotSet = -1;
}

class AudioEntry;

class AudioOutputIPCImpl
    : public NON_EXPORTED_BASE(media::AudioOutputIPC) {
public:
    AudioOutputIPCImpl(/*const scoped_refptr<AudioMessageFilter>&filter, int render_frame_id*/);
    ~AudioOutputIPCImpl() override;

    // media::AudioOutputIPC implementation.
    void RequestDeviceAuthorization(media::AudioOutputIPCDelegate* delegate, int session_id, const std::string& device_id, const std::string& security_origin) override;
    void CreateStream(media::AudioOutputIPCDelegate* delegate, const media::AudioParameters& params) override;
    void PlayStream() override;
    void PauseStream() override;
    void CloseStream() override;
    void SetVolume(double volume) override;

private:
    //const scoped_refptr<AudioMessageFilter> filter_;
    //const int render_frame_id_;
    int stream_id_;
    bool stream_created_;

    AudioEntry* entry_;
};

AudioOutputIPCImpl::AudioOutputIPCImpl(/*const scoped_refptr<AudioMessageFilter>& filter, int render_frame_id*/)
    //     : filter_(filter),
    //     render_frame_id_(render_frame_id),
    : stream_id_(kStreamIDNotSet)
    , stream_created_(false)
    , entry_(nullptr)
{
}

// AudioOutputDeviceInfo GetDefaultDeviceInfoOnDeviceThread(media::AudioManager* audio_manager)
// {
//     DCHECK(audio_manager->GetWorkerTaskRunner()->BelongsToCurrentThread());
//     AudioOutputDeviceInfo default_device_info = {
//         media::AudioManagerBase::kDefaultDeviceId,
//         audio_manager->GetDefaultDeviceName(),
//         audio_manager->GetDefaultOutputStreamParameters() };
//
//     return default_device_info;
// }

void AudioOutputIPCImpl::RequestDeviceAuthorization(
    media::AudioOutputIPCDelegate* delegate,
    int session_id,
    const std::string& device_id,
    const std::string& security_origin)
{
    //     DCHECK(filter_->io_task_runner_->BelongsToCurrentThread());
    //     DCHECK(delegate);
    //     DCHECK_EQ(stream_id_, kStreamIDNotSet);
    //     DCHECK(!stream_created_);
    //
    //     stream_id_ = filter_->delegates_.Add(delegate);
    //     filter_->Send(new AudioHostMsg_RequestDeviceAuthorization(
    //         stream_id_, render_frame_id_, session_id, device_id,
    //         url::Origin(security_origin)));

    media::AudioParameters outputParams = OrigChromeMgr::getInst()->getAudioManager()->GetDefaultOutputStreamParameters();
    delegate->OnDeviceAuthorized(media::OUTPUT_DEVICE_STATUS_OK, outputParams);
}

//////////////////////////////////////////////////////////////////////////

class AudioEntry : public media::AudioOutputController::EventHandler {
public:
    AudioEntry(int stream_id,
        int render_frame_id,
        const media::AudioParameters& params,
        const std::string& output_device_id,
        scoped_ptr<base::SharedMemory> shared_memory,
        scoped_ptr<media::AudioOutputController::SyncReader> reader)
        : stream_id_(stream_id)
        , render_frame_id_(render_frame_id)
        , shared_memory_(shared_memory.Pass())
        , reader_(reader.Pass())
        , controller_(media::AudioOutputController::Create(OrigChromeMgr::getInst()->getAudioManager(), this, params, output_device_id, reader_.get()))
        , playing_(false)
    {
        DCHECK(controller_.get());
    }
    ~AudioEntry() override
    {
    }

    int stream_id() const
    {
        return stream_id_;
    }

    int render_frame_id() const { return render_frame_id_; }

    media::AudioOutputController* controller() const { return controller_.get(); }

    base::SharedMemory* shared_memory()
    {
        return shared_memory_.get();
    }

    media::AudioOutputController::SyncReader* reader() const
    {
        return reader_.get();
    }

    bool playing() const { return playing_; }
    void set_playing(bool playing) { playing_ = playing; }

private:
    // media::AudioOutputController::EventHandler implementation.
    void OnCreated() override
    {
        //         BrowserThread::PostTask(
        //             BrowserThread::IO,
        //             FROM_HERE,
        //             base::Bind(&AudioRendererHost::DoCompleteCreation, host_, stream_id_));
        //DebugBreak();
    }

    void OnPlaying() override
    {
        //         BrowserThread::PostTask(
        //             BrowserThread::IO,
        //             FROM_HERE,
        //             base::Bind(&AudioRendererHost::DoNotifyStreamStateChanged,
        //                 host_,
        //                 stream_id_,
        //                 true));
        //UpdateNumPlayingStreams(entry, is_playing);
        set_playing(true);
    }

    void OnPaused() override
    {
        //         BrowserThread::PostTask(
        //             BrowserThread::IO,
        //             FROM_HERE,
        //             base::Bind(&AudioRendererHost::DoNotifyStreamStateChanged,
        //                 host_,
        //                 stream_id_,
        //                 false));
        set_playing(false);
    }

    void OnError() override
    {
        //         BrowserThread::PostTask(
        //             BrowserThread::IO,
        //             FROM_HERE,
        //             base::Bind(&AudioRendererHost::ReportErrorAndClose, host_, stream_id_));
        ///CloseStream();
        DebugBreak();
    }

    const int stream_id_;

    // The routing ID of the source RenderFrame.
    const int render_frame_id_;

    // Shared memory for transmission of the audio data.  Used by |reader_|.
    const scoped_ptr<base::SharedMemory> shared_memory_;

    // The synchronous reader to be used by |controller_|.
    const scoped_ptr<media::AudioOutputController::SyncReader> reader_;

    // The AudioOutputController that manages the audio stream.
    const scoped_refptr<media::AudioOutputController> controller_;

    bool playing_;
};

// A AudioOutputController::SyncReader implementation using SyncSocket. This
// is used by AudioOutputController to provide a low latency data source for
// transmitting audio packets between the browser process and the renderer
// process.
class AudioSyncReader : public media::AudioOutputController::SyncReader {
public:
    AudioSyncReader::AudioSyncReader(base::SharedMemory* shared_memory, const media::AudioParameters& params)
        : shared_memory_(shared_memory)
        , mute_audio_(false /*base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kMuteAudio)*/)
        , packet_size_(shared_memory_->requested_size())
        , renderer_callback_count_(0)
        , renderer_missed_callback_count_(0)
        ,
#if defined(OS_MACOSX)
        maximum_wait_time_(params.GetBufferDuration() / 2)
        ,
#else
        // TODO(dalecurtis): Investigate if we can reduce this on all platforms.
        maximum_wait_time_(base::TimeDelta::FromMilliseconds(20))
        ,
#endif
        buffer_index_(0)
    {
        DCHECK_EQ(packet_size_, media::AudioBus::CalculateMemorySize(params));
        output_bus_ = media::AudioBus::WrapMemory(params, shared_memory->memory());
        output_bus_->Zero();
    }

    ~AudioSyncReader() override
    {
        //         if (!renderer_callback_count_)
        //             return;
        //
        //         // Recording the percentage of deadline misses gives us a rough overview of
        //         // how many users might be running into audio glitches.
        //         int percentage_missed = 100.0 * renderer_missed_callback_count_ / renderer_callback_count_;
        //         UMA_HISTOGRAM_PERCENTAGE("Media.AudioRendererMissedDeadline", percentage_missed);
        //
        //         // Add more detailed information regarding detected audio glitches where
        //         // a non-zero value of |renderer_missed_callback_count_| is added to the
        //         // AUDIO_RENDERER_AUDIO_GLITCHES bin.
        //         renderer_missed_callback_count_ > 0 ?
        //             LogAudioGlitchResult(AUDIO_RENDERER_AUDIO_GLITCHES) :
        //             LogAudioGlitchResult(AUDIO_RENDERER_NO_AUDIO_GLITCHES);
        //         std::string log_string = base::StringPrintf("ASR: number of detected audio glitches=%d", static_cast<int>(renderer_missed_callback_count_));
        //         MediaStreamManager::SendMessageToNativeLog(log_string);
        //         DVLOG(1) << log_string;
    }

    // media::AudioOutputController::SyncReader implementations.
    void UpdatePendingBytes(uint32 bytes) override
    {
        // Zero out the entire output buffer to avoid stuttering/repeating-buffers
        // in the anomalous case if the renderer is unable to keep up with real-time.
        output_bus_->Zero();
        socket_->Send(&bytes, sizeof(bytes));
        ++buffer_index_;
    }

    void Read(media::AudioBus* dest) override
    {
        ++renderer_callback_count_;
        if (!WaitUntilDataIsReady()) {
            ++renderer_missed_callback_count_;
            //             if (renderer_missed_callback_count_ <= 100) {
            //                 LOG(WARNING) << "AudioSyncReader::Read timed out, audio glitch count=" << renderer_missed_callback_count_;
            //                 if (renderer_missed_callback_count_ == 100)
            //                     LOG(WARNING) << "(log cap reached, suppressing further logs)";
            //             }
            dest->Zero();
            return;
        }

        if (mute_audio_)
            dest->Zero();
        else
            output_bus_->CopyTo(dest);
    }

    void Close() override
    {
        socket_->Close();
    }

    bool Init()
    {
        socket_.reset(new base::CancelableSyncSocket());
        foreign_socket_.reset(new base::CancelableSyncSocket());
        return base::CancelableSyncSocket::CreatePair(socket_.get(), foreign_socket_.get());
    }

    bool PrepareForeignSocket(base::ProcessHandle process_handle, base::SyncSocket::TransitDescriptor* descriptor)
    {
        return foreign_socket_->PrepareTransitDescriptor(process_handle, descriptor);
    }

private:
    // Blocks until data is ready for reading or a timeout expires.  Returns false
    // if an error or timeout occurs.
    bool WaitUntilDataIsReady()
    {
        base::TimeDelta timeout = maximum_wait_time_;
        const base::TimeTicks start_time = base::TimeTicks::Now();
        const base::TimeTicks finish_time = start_time + timeout;

        // Check if data is ready and if not, wait a reasonable amount of time for it.
        //
        // Data readiness is achieved via parallel counters, one on the renderer side
        // and one here.  Every time a buffer is requested via UpdatePendingBytes(),
        // |buffer_index_| is incremented.  Subsequently every time the renderer has a
        // buffer ready it increments its counter and sends the counter value over the
        // SyncSocket.  Data is ready when |buffer_index_| matches the counter value
        // received from the renderer.
        //
        // The counter values may temporarily become out of sync if the renderer is
        // unable to deliver audio fast enough.  It's assumed that the renderer will
        // catch up at some point, which means discarding counter values read from the
        // SyncSocket which don't match our current buffer index.
        size_t bytes_received = 0;
        uint32 renderer_buffer_index = 0;
        while (timeout.InMicroseconds() > 0) {
            bytes_received = socket_->ReceiveWithTimeout(&renderer_buffer_index, sizeof(renderer_buffer_index), timeout);
            if (bytes_received != sizeof(renderer_buffer_index)) {
                bytes_received = 0;
                break;
            }

            if (renderer_buffer_index == buffer_index_)
                break;

            // Reduce the timeout value as receives succeed, but aren't the right index.
            timeout = finish_time - base::TimeTicks::Now();
        }

        // Receive timed out or another error occurred.  Receive can timeout if the
        // renderer is unable to deliver audio data within the allotted time.
        if (!bytes_received || renderer_buffer_index != buffer_index_) {
            //             DVLOG(2) << "AudioSyncReader::WaitUntilDataIsReady() timed out.";
            //
            //             base::TimeDelta time_since_start = base::TimeTicks::Now() - start_time;
            //             UMA_HISTOGRAM_CUSTOM_TIMES("Media.AudioOutputControllerDataNotReady",
            //                 time_since_start,
            //                 base::TimeDelta::FromMilliseconds(1),
            //                 base::TimeDelta::FromMilliseconds(1000),
            //                 50);
            return false;
        }

        return true;
    }

    const base::SharedMemory* const shared_memory_;

    // Mutes all incoming samples. This is used to prevent audible sound
    // during automated testing.
    const bool mute_audio_;

    // Socket for transmitting audio data.
    scoped_ptr<base::CancelableSyncSocket> socket_;

    // Socket to be used by the renderer. The reference is released after
    // PrepareForeignSocketHandle() is called and ran successfully.
    scoped_ptr<base::CancelableSyncSocket> foreign_socket_;

    // Shared memory wrapper used for transferring audio data to Read() callers.
    scoped_ptr<media::AudioBus> output_bus_;

    // Maximum amount of audio data which can be transferred in one Read() call.
    const int packet_size_;

    // Track the number of times the renderer missed its real-time deadline and
    // report a UMA stat during destruction.
    size_t renderer_callback_count_;
    size_t renderer_missed_callback_count_;

    // The maximum amount of time to wait for data from the renderer.  Calculated
    // from the parameters given at construction.
    const base::TimeDelta maximum_wait_time_;

    // The index of the audio buffer we're expecting to be sent from the renderer;
    // used to block with timeout for audio data.
    uint32 buffer_index_;

    DISALLOW_COPY_AND_ASSIGN(AudioSyncReader);
};

void AudioOutputIPCImpl::CreateStream(media::AudioOutputIPCDelegate* delegate, const media::AudioParameters& params)
{
    //     DCHECK(filter_->io_task_runner_->BelongsToCurrentThread());
    //     DCHECK(!stream_created_);
    //     if (stream_id_ == kStreamIDNotSet)
    //         stream_id_ = filter_->delegates_.Add(delegate);
    //
    //     filter_->Send(new AudioHostMsg_CreateStream(stream_id_, render_frame_id_, params));
    static const char kDefaultDeviceId[] = "default";

    // Create the shared memory and share with the renderer process.
    uint32 shared_memory_size = media::AudioBus::CalculateMemorySize(params);
    scoped_ptr<base::SharedMemory> shared_memory(new base::SharedMemory());
    if (!shared_memory->CreateAndMapAnonymous(shared_memory_size)) {
        DebugBreak();
        return;
    }

    scoped_ptr<AudioSyncReader> reader(new AudioSyncReader(shared_memory.get(), params));
    if (!reader->Init()) {
        DebugBreak();
        return;
    }

    base::SyncSocket::TransitDescriptor socket_descriptor;
    // If we failed to prepare the sync socket for the renderer then we fail the construction of audio stream.
    if (!reader->PrepareForeignSocket(base::Process::Current().Handle(), &socket_descriptor)) {
        DebugBreak();
        return;
    }

    entry_ = new AudioEntry(/*stream_id*/ 0, /*render_frame_id*/ 0, params, kDefaultDeviceId, shared_memory.Pass(), reader.Pass());

    base::SharedMemoryHandle foreign_memory_handle;
    if (!entry_->shared_memory()->ShareToProcess(base::Process::Current().Handle(), &foreign_memory_handle)) {
        // If we failed to map and share the shared memory then close the audio
        // stream and send an error message.
        DebugBreak();
        return;
    }

    base::SyncSocket::Handle socket_handle = base::SyncSocket::UnwrapHandle(socket_descriptor);
    delegate->OnStreamCreated(foreign_memory_handle, socket_handle, entry_->shared_memory()->requested_size());

    stream_created_ = true;
}

void AudioOutputIPCImpl::PlayStream()
{
    //     DCHECK(stream_created_);
    //     filter_->Send(new AudioHostMsg_PlayStream(stream_id_));
    if (entry_)
        entry_->controller()->Play();
}

void AudioOutputIPCImpl::PauseStream()
{
    //     DCHECK(stream_created_);
    //     filter_->Send(new AudioHostMsg_PauseStream(stream_id_));
    if (entry_)
        entry_->controller()->Pause();
}

void OnControllerClose(AudioEntry* entry)
{
    if (entry)
        delete entry;
}

void AudioOutputIPCImpl::CloseStream()
{
    //     DCHECK(filter_->io_task_runner_->BelongsToCurrentThread());
    //     DCHECK_NE(stream_id_, kStreamIDNotSet);
    //     filter_->Send(new AudioHostMsg_CloseStream(stream_id_));
    //     filter_->delegates_.Remove(stream_id_);

    if (entry_) {
        media::AudioOutputController* controller = entry_->controller();
        controller->Close(base::Bind(OnControllerClose, entry_));
    }
    entry_ = nullptr;

    stream_id_ = kStreamIDNotSet;
    stream_created_ = false;
}

void AudioOutputIPCImpl::SetVolume(double volume)
{
    //     DCHECK(stream_created_);
    //     filter_->Send(new AudioHostMsg_SetVolume(stream_id_, volume));
    if (entry_)
        entry_->controller()->SetVolume(volume);
}

// static
AudioDeviceFactory* AudioDeviceFactory::factory_ = NULL;

// static
scoped_refptr<media::AudioOutputDevice> AudioDeviceFactory::NewOutputDevice(
    int render_frame_id,
    int session_id,
    const std::string& device_id,
    const std::string& security_origin)
{
    if (factory_) {
        media::AudioOutputDevice* const device = factory_->CreateOutputDevice(
            render_frame_id, session_id, device_id, security_origin);
        if (device)
            return device;
    }

    //AudioMessageFilter* const filter = AudioMessageFilter::Get();
    scoped_refptr<media::AudioOutputDevice> device = new media::AudioOutputDevice(
        scoped_ptr<media::AudioOutputIPC>(new AudioOutputIPCImpl()), // filter->CreateAudioOutputIPC(render_frame_id),
        OrigChromeMgr::getInst()->getMediaIoThread()->task_runner(), // filter->io_task_runner(),
        session_id, device_id, security_origin);
    device->RequestDeviceAuthorization();
    return device;
}

// static
scoped_refptr<media::AudioInputDevice> AudioDeviceFactory::NewInputDevice(int render_frame_id)
{
    //     if (factory_) {
    //         media::AudioInputDevice* const device =
    //             factory_->CreateInputDevice(render_frame_id);
    //         if (device)
    //             return device;
    //     }
    //
    //     AudioInputMessageFilter* const filter = AudioInputMessageFilter::Get();
    //     return new media::AudioInputDevice(
    //         filter->CreateAudioInputIPC(render_frame_id), filter->io_task_runner());
    DebugBreak();
    return nullptr;
}

AudioDeviceFactory::AudioDeviceFactory()
{
    DCHECK(!factory_) << "Can't register two factories at once.";
    factory_ = this;
}

AudioDeviceFactory::~AudioDeviceFactory()
{
    factory_ = NULL;
}

AudioOutputIPCImpl::~AudioOutputIPCImpl()
{
}

} // namespace content
