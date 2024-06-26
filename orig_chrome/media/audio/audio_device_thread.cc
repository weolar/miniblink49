// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/audio_device_thread.h"

#include <algorithm>

#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/aligned_memory.h"
#include "base/message_loop/message_loop.h"
#include "base/numerics/safe_conversions.h"
#include "base/threading/platform_thread.h"
#include "base/threading/thread_restrictions.h"
#include "media/base/audio_bus.h"

using base::PlatformThread;

namespace media {

// The actual worker thread implementation.  It's very bare bones and much
// simpler than SimpleThread (no synchronization in Start, etc) and supports
// joining the thread handle asynchronously via a provided message loop even
// after the Thread object itself has been deleted.
class AudioDeviceThread::Thread
    : public PlatformThread::Delegate,
      public base::RefCountedThreadSafe<AudioDeviceThread::Thread> {
public:
    Thread(AudioDeviceThread::Callback* callback,
        base::SyncSocket::Handle socket,
        const char* thread_name,
        bool synchronized_buffers);

    void Start();

    // Stops the thread.  If |loop_for_join| is non-NULL, the function posts
    // a task to join (close) the thread handle later instead of waiting for
    // the thread.  If loop_for_join is NULL, then the function waits
    // synchronously for the thread to terminate.
    void Stop(base::MessageLoop* loop_for_join);

private:
    friend class base::RefCountedThreadSafe<AudioDeviceThread::Thread>;
    ~Thread() override;

    // Overrides from PlatformThread::Delegate.
    void ThreadMain() override;

    // Runs the loop that reads from the socket.
    void Run();

private:
    base::PlatformThreadHandle thread_;
    AudioDeviceThread::Callback* callback_;
    base::CancelableSyncSocket socket_;
    base::Lock callback_lock_;
    const char* thread_name_;
    const bool synchronized_buffers_;

    DISALLOW_COPY_AND_ASSIGN(Thread);
};

// AudioDeviceThread implementation

AudioDeviceThread::AudioDeviceThread()
{
}

AudioDeviceThread::~AudioDeviceThread() { DCHECK(!thread_.get()); }

void AudioDeviceThread::Start(AudioDeviceThread::Callback* callback,
    base::SyncSocket::Handle socket,
    const char* thread_name,
    bool synchronized_buffers)
{
    base::AutoLock auto_lock(thread_lock_);
    CHECK(!thread_.get());
    thread_ = new AudioDeviceThread::Thread(callback, socket, thread_name, synchronized_buffers);
    thread_->Start();
}

void AudioDeviceThread::Stop(base::MessageLoop* loop_for_join)
{
    base::AutoLock auto_lock(thread_lock_);
    if (thread_.get()) {
        thread_->Stop(loop_for_join);
        thread_ = NULL;
    }
}

bool AudioDeviceThread::IsStopped()
{
    base::AutoLock auto_lock(thread_lock_);
    return !thread_.get();
}

// AudioDeviceThread::Thread implementation
AudioDeviceThread::Thread::Thread(AudioDeviceThread::Callback* callback,
    base::SyncSocket::Handle socket,
    const char* thread_name,
    bool synchronized_buffers)
    : thread_()
    , callback_(callback)
    , socket_(socket)
    , thread_name_(thread_name)
    , synchronized_buffers_(synchronized_buffers)
{
}

AudioDeviceThread::Thread::~Thread()
{
    DCHECK(thread_.is_null());
}

void AudioDeviceThread::Thread::Start()
{
    base::AutoLock auto_lock(callback_lock_);
    DCHECK(thread_.is_null());
    // This reference will be released when the thread exists.
    AddRef();

    PlatformThread::CreateWithPriority(0, this, &thread_,
        base::ThreadPriority::REALTIME_AUDIO);
    CHECK(!thread_.is_null());
}

void AudioDeviceThread::Thread::Stop(base::MessageLoop* loop_for_join)
{
    socket_.Shutdown();

    base::PlatformThreadHandle thread = base::PlatformThreadHandle();

    { // NOLINT
        base::AutoLock auto_lock(callback_lock_);
        callback_ = NULL;
        std::swap(thread, thread_);
    }

    if (!thread.is_null()) {
        if (loop_for_join) {
            loop_for_join->PostTask(FROM_HERE,
                base::Bind(&base::PlatformThread::Join, thread));
        } else {
            base::PlatformThread::Join(thread);
        }
    }
}

void AudioDeviceThread::Thread::ThreadMain()
{
    PlatformThread::SetName(thread_name_);

    // Singleton access is safe from this thread as long as callback is non-NULL.
    // The callback is the only point where the thread calls out to 'unknown' code
    // that might touch singletons and the lifetime of the callback is controlled
    // by another thread on which singleton access is OK as well.
    base::ThreadRestrictions::SetSingletonAllowed(true);

    { // NOLINT
        base::AutoLock auto_lock(callback_lock_);
        if (callback_)
            callback_->InitializeOnAudioThread();
    }

    Run();

    // Release the reference for the thread. Note that after this, the Thread
    // instance will most likely be deleted.
    Release();
}

void AudioDeviceThread::Thread::Run()
{
    uint32 buffer_index = 0;
    while (true) {
        uint32 pending_data = 0;
        size_t bytes_read = socket_.Receive(&pending_data, sizeof(pending_data));
        if (bytes_read != sizeof(pending_data))
            break;

        // kuint32max is a special signal which is returned after the browser
        // stops the output device in response to a renderer side request.
        //
        // Avoid running Process() for the paused signal, we still need to update
        // the buffer index if |synchronized_buffers_| is true though.
        //
        // See comments in AudioOutputController::DoPause() for details on why.
        if (pending_data != kuint32max) {
            base::AutoLock auto_lock(callback_lock_);
            if (callback_)
                callback_->Process(pending_data);
        }

        // The usage of |synchronized_buffers_| differs between input and output
        // cases.
        // Input:
        // Let the other end know that we have read data, so that it can verify
        // it doesn't overwrite any data before read. The |buffer_index| value is
        // not used. For more details, see AudioInputSyncWriter::Write().
        // Output:
        // Let the other end know which buffer we just filled. The |buffer_index| is
        // used to ensure the other end is getting the buffer it expects. For more
        // details on how this works see AudioSyncReader::WaitUntilDataIsReady().
        if (synchronized_buffers_) {
            ++buffer_index;
            size_t bytes_sent = socket_.Send(&buffer_index, sizeof(buffer_index));
            if (bytes_sent != sizeof(buffer_index))
                break;
        }
    }
}

// AudioDeviceThread::Callback implementation

AudioDeviceThread::Callback::Callback(
    const AudioParameters& audio_parameters,
    base::SharedMemoryHandle memory,
    int memory_length,
    int total_segments)
    : audio_parameters_(audio_parameters)
    , samples_per_ms_(audio_parameters.sample_rate() / 1000)
    , bytes_per_ms_(audio_parameters.channels() * (audio_parameters_.bits_per_sample() / 8) * samples_per_ms_)
    , shared_memory_(memory, false)
    , memory_length_(memory_length)
    , total_segments_(total_segments)
{
    CHECK_NE(bytes_per_ms_, 0); // Catch division by zero early.
    CHECK_NE(samples_per_ms_, 0);
    CHECK_GT(total_segments_, 0);
    CHECK_EQ(memory_length_ % total_segments_, 0);
    segment_length_ = memory_length_ / total_segments_;
}

AudioDeviceThread::Callback::~Callback() { }

void AudioDeviceThread::Callback::InitializeOnAudioThread()
{
    MapSharedMemory();
    CHECK(shared_memory_.memory());
}

} // namespace media.
