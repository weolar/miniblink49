// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/buffered_data_source.h"

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "media/base/media_log.h"
//#include "net/base/net_errors.h"

using blink::WebFrame;

namespace {

// BufferedDataSource has an intermediate buffer, this value governs the initial
// size of that buffer. It is set to 32KB because this is a typical read size
// of FFmpeg.
const int kInitialReadBufferSize = 32768;

// Number of cache misses or read failures we allow for a single Read() before
// signaling an error.
const int kLoaderRetries = 3;

// The number of milliseconds to wait before retrying a failed load.
const int kLoaderFailedRetryDelayMs = 250;

} // namespace

namespace media {

class BufferedDataSource::ReadOperation {
public:
    ReadOperation(int64 position, int size, uint8* data,
        const DataSource::ReadCB& callback);
    ~ReadOperation();

    // Runs |callback_| with the given |result|, deleting the operation
    // afterwards.
    static void Run(scoped_ptr<ReadOperation> read_op, int result);

    // State for the number of times this read operation has been retried.
    int retries() { return retries_; }
    void IncrementRetries() { ++retries_; }

    int64 position() { return position_; }
    int size() { return size_; }
    uint8* data() { return data_; }

private:
    int retries_;

    const int64 position_;
    const int size_;
    uint8* data_;
    DataSource::ReadCB callback_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(ReadOperation);
};

BufferedDataSource::ReadOperation::ReadOperation(
    int64 position, int size, uint8* data,
    const DataSource::ReadCB& callback)
    : retries_(0)
    , position_(position)
    , size_(size)
    , data_(data)
    , callback_(callback)
{
    DCHECK(!callback_.is_null());
}

BufferedDataSource::ReadOperation::~ReadOperation()
{
    DCHECK(callback_.is_null());
}

// static
void BufferedDataSource::ReadOperation::Run(
    scoped_ptr<ReadOperation> read_op, int result)
{
    base::ResetAndReturn(&read_op->callback_).Run(result);
}

BufferedDataSource::BufferedDataSource(
    const blink::WebURL& url,
    BufferedResourceLoader::CORSMode cors_mode,
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    WebFrame* frame,
    MediaLog* media_log,
    BufferedDataSourceHost* host,
    const DownloadingCB& downloading_cb)
    : url_(url)
    , cors_mode_(cors_mode)
    , total_bytes_(kPositionNotSpecified)
    , streaming_(false)
    , frame_(frame)
    , intermediate_read_buffer_(kInitialReadBufferSize)
    , render_task_runner_(task_runner)
    , stop_signal_received_(false)
    , media_has_played_(false)
    , preload_(AUTO)
    , bitrate_(0)
    , playback_rate_(0.0)
    , media_log_(media_log)
    , host_(host)
    , downloading_cb_(downloading_cb)
    , weak_factory_(this)
{
    weak_ptr_ = weak_factory_.GetWeakPtr();
    DCHECK(host_);
    DCHECK(!downloading_cb_.is_null());
    DCHECK(render_task_runner_->BelongsToCurrentThread());
}

BufferedDataSource::~BufferedDataSource()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
}

// A factory method to create BufferedResourceLoader using the read parameters.
// This method can be overridden to inject mock BufferedResourceLoader object
// for testing purpose.
BufferedResourceLoader* BufferedDataSource::CreateResourceLoader(
    int64 first_byte_position, int64 last_byte_position)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());

    BufferedResourceLoader::DeferStrategy strategy = preload_ == METADATA ? BufferedResourceLoader::kReadThenDefer : BufferedResourceLoader::kCapacityDefer;

    return new BufferedResourceLoader(url_,
        cors_mode_,
        first_byte_position,
        last_byte_position,
        strategy,
        bitrate_,
        playback_rate_,
        media_log_.get());
}

void BufferedDataSource::Initialize(const InitializeCB& init_cb)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(!init_cb.is_null());
    DCHECK(!loader_.get());

    init_cb_ = init_cb;

    if (url_.schemeIsHTTPOrHTTPS()) {
        // Do an unbounded range request starting at the beginning.  If the server
        // responds with 200 instead of 206 we'll fall back into a streaming mode.
        loader_.reset(CreateResourceLoader(0, kPositionNotSpecified));
    } else {
        // For all other protocols, assume they support range request. We fetch
        // the full range of the resource to obtain the instance size because
        // we won't be served HTTP headers.
        loader_.reset(CreateResourceLoader(kPositionNotSpecified, kPositionNotSpecified));
    }

    base::WeakPtr<BufferedDataSource> weak_this = weak_factory_.GetWeakPtr();
    loader_->Start(
        base::Bind(&BufferedDataSource::StartCallback, weak_this),
        base::Bind(&BufferedDataSource::LoadingStateChangedCallback, weak_this),
        base::Bind(&BufferedDataSource::ProgressCallback, weak_this),
        frame_);
}

void BufferedDataSource::SetPreload(Preload preload)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    preload_ = preload;
}

bool BufferedDataSource::HasSingleOrigin()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(init_cb_.is_null() && loader_.get())
        << "Initialize() must complete before calling HasSingleOrigin()";
    return loader_->HasSingleOrigin();
}

bool BufferedDataSource::DidPassCORSAccessCheck() const
{
    return loader_.get() && loader_->DidPassCORSAccessCheck();
}

void BufferedDataSource::Abort()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    {
        base::AutoLock auto_lock(lock_);
        StopInternal_Locked();
    }
    StopLoader();
    frame_ = NULL;
}

void BufferedDataSource::MediaPlaybackRateChanged(double playback_rate)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(loader_.get());

    if (playback_rate < 0.0)
        return;

    playback_rate_ = playback_rate;
    loader_->SetPlaybackRate(playback_rate);
}

void BufferedDataSource::MediaIsPlaying()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    media_has_played_ = true;
    UpdateDeferStrategy(false);
}

void BufferedDataSource::MediaIsPaused()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    UpdateDeferStrategy(true);
}

/////////////////////////////////////////////////////////////////////////////
// DataSource implementation.
void BufferedDataSource::Stop()
{
    {
        base::AutoLock auto_lock(lock_);
        StopInternal_Locked();
    }

    render_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&BufferedDataSource::StopLoader, weak_factory_.GetWeakPtr()));
}

void BufferedDataSource::SetBitrate(int bitrate)
{
    render_task_runner_->PostTask(FROM_HERE,
        base::Bind(&BufferedDataSource::SetBitrateTask,
            weak_factory_.GetWeakPtr(),
            bitrate));
}

void BufferedDataSource::OnBufferingHaveEnough()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    if (loader_ && preload_ == METADATA && !media_has_played_ && !IsStreaming())
        loader_->CancelUponDeferral();
}

int64_t BufferedDataSource::GetMemoryUsage() const
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    return loader_ ? loader_->GetMemoryUsage() : 0;
}

void BufferedDataSource::Read(
    int64 position, int size, uint8* data,
    const DataSource::ReadCB& read_cb)
{
    DVLOG(1) << "Read: " << position << " offset, " << size << " bytes";
    DCHECK(!read_cb.is_null());

    {
        base::AutoLock auto_lock(lock_);
        DCHECK(!read_op_);

        if (stop_signal_received_) {
            read_cb.Run(kReadError);
            return;
        }

        read_op_.reset(new ReadOperation(position, size, data, read_cb));
    }

    render_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&BufferedDataSource::ReadTask, weak_factory_.GetWeakPtr()));
}

bool BufferedDataSource::GetSize(int64* size_out)
{
    if (total_bytes_ != kPositionNotSpecified) {
        *size_out = total_bytes_;
        return true;
    }
    *size_out = 0;
    return false;
}

bool BufferedDataSource::IsStreaming()
{
    return streaming_;
}

/////////////////////////////////////////////////////////////////////////////
// Render thread tasks.
void BufferedDataSource::ReadTask()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    ReadInternal();
}

void BufferedDataSource::StopInternal_Locked()
{
    lock_.AssertAcquired();
    if (stop_signal_received_)
        return;

    stop_signal_received_ = true;

    // Initialize() isn't part of the DataSource interface so don't call it in
    // response to Stop().
    init_cb_.Reset();

    if (read_op_)
        ReadOperation::Run(read_op_.Pass(), kReadError);
}

void BufferedDataSource::StopLoader()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());

    if (loader_)
        loader_->Stop();
}

void BufferedDataSource::SetBitrateTask(int bitrate)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(loader_.get());

    bitrate_ = bitrate;
    loader_->SetBitrate(bitrate);
}

// This method is the place where actual read happens, |loader_| must be valid
// prior to make this method call.
void BufferedDataSource::ReadInternal()
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    int64 position = 0;
    int size = 0;
    {
        base::AutoLock auto_lock(lock_);
        if (stop_signal_received_)
            return;

        position = read_op_->position();
        size = read_op_->size();
    }

    // First we prepare the intermediate read buffer for BufferedResourceLoader
    // to write to.
    if (static_cast<int>(intermediate_read_buffer_.size()) < size)
        intermediate_read_buffer_.resize(size);

    // Perform the actual read with BufferedResourceLoader.
    DCHECK(!intermediate_read_buffer_.empty());
    loader_->Read(position, size, &intermediate_read_buffer_[0], base::Bind(&BufferedDataSource::ReadCallback, weak_factory_.GetWeakPtr()));
}

/////////////////////////////////////////////////////////////////////////////
// BufferedResourceLoader callback methods.
void BufferedDataSource::StartCallback(
    BufferedResourceLoader::Status status)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(loader_.get());

    bool init_cb_is_null = false;
    {
        base::AutoLock auto_lock(lock_);
        init_cb_is_null = init_cb_.is_null();
    }
    if (init_cb_is_null) {
        loader_->Stop();
        return;
    }
    response_original_url_ = loader_->response_original_url();

    // All responses must be successful. Resources that are assumed to be fully
    // buffered must have a known content length.
    bool success = status == BufferedResourceLoader::kOk && (!assume_fully_buffered() || loader_->instance_size() != kPositionNotSpecified);

    if (success) {
        total_bytes_ = loader_->instance_size();
        streaming_ = !assume_fully_buffered() && (total_bytes_ == kPositionNotSpecified || !loader_->range_supported());

        media_log_->SetDoubleProperty("total_bytes", static_cast<double>(total_bytes_));
        media_log_->SetBooleanProperty("streaming", streaming_);
    } else {
        loader_->Stop();
    }

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    base::AutoLock auto_lock(lock_);
    if (stop_signal_received_)
        return;

    if (success) {
        if (total_bytes_ != kPositionNotSpecified) {
            host_->SetTotalBytes(total_bytes_);
            if (assume_fully_buffered())
                host_->AddBufferedByteRange(0, total_bytes_);
        }

        media_log_->SetBooleanProperty("single_origin", loader_->HasSingleOrigin());
        media_log_->SetBooleanProperty("passed_cors_access_check", loader_->DidPassCORSAccessCheck());
        media_log_->SetBooleanProperty("range_header_supported", loader_->range_supported());
    }

    render_task_runner_->PostTask(FROM_HERE, base::Bind(base::ResetAndReturn(&init_cb_), success));
}

void BufferedDataSource::PartialReadStartCallback(
    BufferedResourceLoader::Status status)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());
    DCHECK(loader_.get());
    if (status == BufferedResourceLoader::kOk && CheckPartialResponseURL(loader_->response_original_url())) {
        // Once the request has started successfully, we can proceed with
        // reading from it.
        ReadInternal();
        return;
    }

    // Stop the resource loader since we have received an error.
    loader_->Stop();

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    base::AutoLock auto_lock(lock_);
    if (stop_signal_received_)
        return;
    ReadOperation::Run(read_op_.Pass(), kReadError);
}

bool BufferedDataSource::CheckPartialResponseURL(const blink::WebURL& partial_response_original_url) const
{
    // We check the redirected URL of partial responses in case malicious
    // attackers scan the bytes of other origin resources by mixing their
    // generated bytes and the target response. See http://crbug.com/489060#c32
    // for details.
    // If the origin of the new response is different from the first response we
    // deny the redirected response unless the crossorigin attribute has been set.
    if ((response_original_url_.getOrigin() == partial_response_original_url.getOrigin()) || DidPassCORSAccessCheck()) {
        return true;
    }

    MEDIA_LOG(ERROR, media_log_) << "BufferedDataSource: origin has changed";
    return false;
}

void BufferedDataSource::ReadCallback(BufferedResourceLoader::Status status, int bytes_read, int need_read)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    base::AutoLock auto_lock(lock_);
    if (stop_signal_received_)
        return;

    if (status != BufferedResourceLoader::kOk) {
        // Stop the resource load if it failed.
        loader_->Stop();

        if (read_op_->retries() < kLoaderRetries) {
            // Allow some resiliency against sporadic network failures or intentional
            // cancellations due to a system suspend / resume.  Here we treat failed
            // reads as a cache miss so long as we haven't exceeded max retries.
            if (status == BufferedResourceLoader::kFailed) {
                render_task_runner_->PostDelayedTask(
                    FROM_HERE, base::Bind(&BufferedDataSource::ReadCallback, weak_factory_.GetWeakPtr(), BufferedResourceLoader::kCacheMiss, 0, need_read),
                    base::TimeDelta::FromMilliseconds(kLoaderFailedRetryDelayMs));
                return;
            }

            read_op_->IncrementRetries();

            // Recreate a loader starting from where we last left off until the end of the resource.
            int64 last_byte_position = kPositionNotSpecified;
            if (need_read != 0)
                last_byte_position = read_op_->position() + need_read * 6;
            if (total_bytes_ <= 0 || last_byte_position > total_bytes_)
                last_byte_position = kPositionNotSpecified;

            if (!url_.schemeIsHTTPOrHTTPS()) // 如果是文件协议，直接为-1了。不然BufferedResourceLoader的content_length_会不对（为意外为last_byte_position）。
                last_byte_position = kPositionNotSpecified;

            loader_.reset(CreateResourceLoader(read_op_->position(), /*kPositionNotSpecified*/last_byte_position));

            base::WeakPtr<BufferedDataSource> weak_this = weak_factory_.GetWeakPtr();
            loader_->Start(
                base::Bind(&BufferedDataSource::PartialReadStartCallback, weak_this),
                base::Bind(&BufferedDataSource::LoadingStateChangedCallback, weak_this),
                base::Bind(&BufferedDataSource::ProgressCallback, weak_this),
                frame_);
            return;
        }

        ReadOperation::Run(read_op_.Pass(), kReadError);
        return;
    }

    if (bytes_read > 0) {
        DCHECK(!intermediate_read_buffer_.empty());
        memcpy(read_op_->data(), &intermediate_read_buffer_[0], bytes_read);
    } else if (bytes_read == 0 && total_bytes_ == kPositionNotSpecified) {
        // We've reached the end of the file and we didn't know the total size
        // before. Update the total size so Read()s past the end of the file will
        // fail like they would if we had known the file size at the beginning.
        total_bytes_ = loader_->instance_size();

        if (total_bytes_ != kPositionNotSpecified) {
            host_->SetTotalBytes(total_bytes_);
            host_->AddBufferedByteRange(loader_->first_byte_position(),
                total_bytes_);
        }
    }
    ReadOperation::Run(read_op_.Pass(), bytes_read);
}

void BufferedDataSource::LoadingStateChangedCallback(
    BufferedResourceLoader::LoadingState state)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());

    if (assume_fully_buffered())
        return;

    bool is_downloading_data;
    switch (state) {
    case BufferedResourceLoader::kLoading:
        is_downloading_data = true;
        break;
    case BufferedResourceLoader::kLoadingDeferred:
    case BufferedResourceLoader::kLoadingFinished:
        is_downloading_data = false;
        break;

    // TODO(scherkus): we don't signal network activity changes when loads
    // fail to preserve existing behaviour when deferring is toggled, however
    // we should consider changing DownloadingCB to also propagate loading
    // state. For example there isn't any signal today to notify the client that
    // loading has failed (we only get errors on subsequent reads).
    case BufferedResourceLoader::kLoadingFailed:
        return;
    }

    downloading_cb_.Run(is_downloading_data);
}

void BufferedDataSource::ProgressCallback(int64 position)
{
    DCHECK(render_task_runner_->BelongsToCurrentThread());

    if (assume_fully_buffered())
        return;

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    base::AutoLock auto_lock(lock_);
    if (stop_signal_received_)
        return;

    host_->AddBufferedByteRange(loader_->first_byte_position(), position);
}

void BufferedDataSource::UpdateDeferStrategy(bool paused)
{
    // No need to aggressively buffer when we are assuming the resource is fully
    // buffered.
    if (assume_fully_buffered()) {
        loader_->UpdateDeferStrategy(BufferedResourceLoader::kCapacityDefer);
        return;
    }

    // If the playback has started (at which point the preload value is ignored)
    // and we're paused, then try to load as much as possible (the loader will
    // fall back to kCapacityDefer if it knows the current response won't be
    // useful from the cache in the future).
    if (media_has_played_ && paused && loader_->range_supported()) {
        loader_->UpdateDeferStrategy(BufferedResourceLoader::kNeverDefer);
        return;
    }

    // If media is currently playing or the page indicated preload=auto or the
    // the server does not support the byte range request or we do not want to go
    // too far ahead of the read head, use threshold strategy to enable/disable
    // deferring when the buffer is full/depleted.
    loader_->UpdateDeferStrategy(BufferedResourceLoader::kCapacityDefer);
}

} // namespace media
