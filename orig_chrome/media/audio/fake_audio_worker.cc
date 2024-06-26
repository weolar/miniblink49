// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/fake_audio_worker.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/cancelable_callback.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "media/audio/audio_parameters.h"

namespace media {

class FakeAudioWorker::Worker
    : public base::RefCountedThreadSafe<FakeAudioWorker::Worker> {
public:
    Worker(const scoped_refptr<base::SingleThreadTaskRunner>& worker_task_runner,
        const AudioParameters& params);

    bool IsStopped();
    void Start(const base::Closure& worker_cb);
    void Stop();

private:
    friend class base::RefCountedThreadSafe<Worker>;
    ~Worker();

    // Initialize and start regular calls to DoRead() on the worker thread.
    void DoStart();

    // Cancel any delayed callbacks to DoRead() in the worker loop's queue.
    void DoCancel();

    // Task that regularly calls |worker_cb_| according to the playback rate as
    // determined by the audio parameters given during construction.  Runs on
    // the worker loop.
    void DoRead();

    const scoped_refptr<base::SingleThreadTaskRunner> worker_task_runner_;
    const base::TimeDelta buffer_duration_;

    base::Lock worker_cb_lock_; // Held while mutating or running |worker_cb_|.
    base::Closure worker_cb_;
    base::TimeTicks next_read_time_;

    // Used to cancel any delayed tasks still inside the worker loop's queue.
    base::CancelableClosure worker_task_cb_;

    base::ThreadChecker thread_checker_;

    DISALLOW_COPY_AND_ASSIGN(Worker);
};

FakeAudioWorker::FakeAudioWorker(
    const scoped_refptr<base::SingleThreadTaskRunner>& worker_task_runner,
    const AudioParameters& params)
    : worker_(new Worker(worker_task_runner, params))
{
}

FakeAudioWorker::~FakeAudioWorker()
{
    DCHECK(worker_->IsStopped());
}

void FakeAudioWorker::Start(const base::Closure& worker_cb)
{
    DCHECK(worker_->IsStopped());
    worker_->Start(worker_cb);
}

void FakeAudioWorker::Stop()
{
    worker_->Stop();
}

FakeAudioWorker::Worker::Worker(
    const scoped_refptr<base::SingleThreadTaskRunner>& worker_task_runner,
    const AudioParameters& params)
    : worker_task_runner_(worker_task_runner)
    , buffer_duration_(base::TimeDelta::FromMicroseconds(
          params.frames_per_buffer() * base::Time::kMicrosecondsPerSecond / static_cast<float>(params.sample_rate())))
{
    // Worker can be constructed on any thread, but will DCHECK that its
    // Start/Stop methods are called from the same thread.
    thread_checker_.DetachFromThread();
}

FakeAudioWorker::Worker::~Worker()
{
    DCHECK(worker_cb_.is_null());
}

bool FakeAudioWorker::Worker::IsStopped()
{
    base::AutoLock scoped_lock(worker_cb_lock_);
    return worker_cb_.is_null();
}

void FakeAudioWorker::Worker::Start(const base::Closure& worker_cb)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!worker_cb.is_null());
    {
        base::AutoLock scoped_lock(worker_cb_lock_);
        DCHECK(worker_cb_.is_null());
        worker_cb_ = worker_cb;
    }
    worker_task_runner_->PostTask(FROM_HERE, base::Bind(&Worker::DoStart, this));
}

void FakeAudioWorker::Worker::DoStart()
{
    DCHECK(worker_task_runner_->BelongsToCurrentThread());
    next_read_time_ = base::TimeTicks::Now();
    worker_task_cb_.Reset(base::Bind(&Worker::DoRead, this));
    worker_task_cb_.callback().Run();
}

void FakeAudioWorker::Worker::Stop()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    {
        base::AutoLock scoped_lock(worker_cb_lock_);
        if (worker_cb_.is_null())
            return;
        worker_cb_.Reset();
    }
    worker_task_runner_->PostTask(FROM_HERE, base::Bind(&Worker::DoCancel, this));
}

void FakeAudioWorker::Worker::DoCancel()
{
    DCHECK(worker_task_runner_->BelongsToCurrentThread());
    worker_task_cb_.Cancel();
}

void FakeAudioWorker::Worker::DoRead()
{
    DCHECK(worker_task_runner_->BelongsToCurrentThread());

    {
        base::AutoLock scoped_lock(worker_cb_lock_);
        if (!worker_cb_.is_null())
            worker_cb_.Run();
    }

    // Need to account for time spent here due to the cost of |worker_cb| as well
    // as the imprecision of PostDelayedTask().
    const base::TimeTicks now = base::TimeTicks::Now();
    base::TimeDelta delay = next_read_time_ + buffer_duration_ - now;

    // If we're behind, find the next nearest ontime interval.
    if (delay < base::TimeDelta())
        delay += buffer_duration_ * (-delay / buffer_duration_ + 1);
    next_read_time_ = now + delay;

    worker_task_runner_->PostDelayedTask(
        FROM_HERE, worker_task_cb_.callback(), delay);
}

} // namespace media
