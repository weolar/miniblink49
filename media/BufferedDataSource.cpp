// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/BufferedDataSource.h"
#include "media/BufferedConstant.h"
#include "content/browser/PostTaskHelper.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/Source/wtf/Locker.h"

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

}  // namespace

namespace media {

class BufferedDataSource::ReadOperation {
public:
    ReadOperation(int64 position, int size, char* data, DataSource::ReadCB* callback);
    ~ReadOperation();

    // Runs |m_callback| with the given |result|, deleting the operation
    // afterwards.
    static void Run(std::unique_ptr<ReadOperation> readOp, int result);

    // State for the number of times this read operation has been retried.
    int retries()
    {
        return retries_;
    }
    void IncrementRetries()
    {
        ++retries_;
    }

    int64 position()
    {
        return m_position;
    }
    int size()
    {
        return m_size;
    }
    char* data()
    {
        return m_data;
    }

private:
    int retries_;

    const int64 m_position;
    const int m_size;
    char* m_data;
    wke::DataSource::ReadCB* m_callback;

    //DISALLOW_IMPLICIT_CONSTRUCTORS(ReadOperation);
};

BufferedDataSource::ReadOperation::ReadOperation(
    int64 position, int size, char* data,
    DataSource::ReadCB* callback)
    : retries_(0)
    , m_position(position)
    , m_size(size)
    , m_data(data)
    , m_callback(callback)
{
    ASSERT(m_callback);
}

BufferedDataSource::ReadOperation::~ReadOperation()
{
    ASSERT(!m_callback);
}

// static
void BufferedDataSource::ReadOperation::Run(std::unique_ptr<ReadOperation> readOp, int result)
{
    //base::ResetAndReturn(&readOp->m_callback).Run(result);
    readOp->m_callback->signalReadCompleted(result);
    readOp->m_callback = nullptr;
}

BufferedDataSource::BufferedDataSource(
    const blink::KURL& url,
    BufferedResourceLoader::CORSMode cors_mode,
    WebFrame* frame,
    MediaLog* media_log,
    BufferedDataSourceHost* host,
    DownloadingCB* downloading_cb)
    : m_url(url)
    , m_corsMode(cors_mode)
    , m_totalBytes(kPositionNotSpecified)
    , m_streaming(false)
    , m_frame(frame)
    , m_intermediateReadBuffer(kInitialReadBufferSize)
    , m_stopSignalReceived(false)
    , m_mediaHasPlayed(false)
    , m_preload(AUTO)
    , m_bitrate(0)
    , m_playbackRate(0.0)
    //, media_log_(media_log)
    , m_host(host)
    , m_downloadingCb(downloading_cb)
    , m_willDestroyed(false)
{
    ASSERT(m_host);
    ASSERT(m_downloadingCb);
    ASSERT(WTF::isMainThread());
	m_id = net::ActivatingObjCheck::inst()->genId();
	net::ActivatingObjCheck::inst()->add(m_id);
}

BufferedDataSource::~BufferedDataSource() 
{
    ASSERT(WTF::isMainThread());
    abort();
    net::ActivatingObjCheck::inst()->remove(m_id);
}

void BufferedDataSource::willDestroyed()
{
    m_willDestroyed = true;
}

void BufferedDataSource::destroy()
{
	delete this;
}

// A factory method to create BufferedResourceLoader using the read parameters.
// This method can be overridden to inject mock BufferedResourceLoader object
// for testing purpose.
BufferedResourceLoader* BufferedDataSource::CreateResourceLoader(int64 first_byte_position, int64 last_byte_position)
{
    ASSERT(WTF::isMainThread());

    BufferedResourceLoader::DeferStrategy strategy = m_preload == METADATA ?
        BufferedResourceLoader::kReadThenDefer :
        BufferedResourceLoader::kCapacityDefer;

    return new BufferedResourceLoader(m_url,
                                      m_corsMode,
                                      first_byte_position,
                                      last_byte_position,
                                      strategy,
                                      m_bitrate,
                                      m_playbackRate,
                                      /*media_log_.get()*/nullptr);
}

void BufferedDataSource::initialize(wke::DataSource::InitializeCB* init_cb)
{
    ASSERT(WTF::isMainThread());
    ASSERT(init_cb);
    ASSERT(!m_loader.get());
    if (m_willDestroyed)
        return;

    m_initCb = (init_cb);

    if (m_url.protocolIsInHTTPFamily()) {
        // Do an unbounded range request starting at the beginning.  If the server
        // responds with 200 instead of 206 we'll fall back into a streaming mode.
        m_loader.reset(CreateResourceLoader(0, kPositionNotSpecified));
    } else {
        // For all other protocols, assume they support range request. We fetch
        // the full range of the resource to obtain the instance size because
        // we won't be served HTTP headers.
        m_loader.reset(CreateResourceLoader(kPositionNotSpecified, kPositionNotSpecified));
    }

//     base::WeakPtr<BufferedDataSource> weak_this = weak_factory_.GetWeakPtr();
//     m_loader->Start(
//         base::Bind(&BufferedDataSource::StartCallback, weak_this),
//         base::Bind(&BufferedDataSource::LoadingStateChangedCallback, weak_this),
//         base::Bind(&BufferedDataSource::ProgressCallback, weak_this),
//         m_frame);
    BufferedDataSource* self = this;
    int id = m_id;
    m_loader->Start(
        [self, id](BufferedResourceLoader::Status status) {
            if (net::ActivatingObjCheck::inst()->isActivating(id))
                self->StartCallback(status);
        },
        [self, id](BufferedResourceLoader::LoadingState state) {
            if (net::ActivatingObjCheck::inst()->isActivating(id))
                self->LoadingStateChangedCallback(state);
        },
        [self, id](int64 position) {
            if (net::ActivatingObjCheck::inst()->isActivating(id))
                self->ProgressCallback(position);
        },
        m_frame
    );
}

void BufferedDataSource::setPreload(wke::DataSource::Preload preload)
{
    ASSERT(WTF::isMainThread());
    m_preload = preload;
}

bool BufferedDataSource::hasSingleOrigin()
{
    ASSERT(WTF::isMainThread());
//     DCHECK(init_cb_.is_null() && m_loader.get()) << "Initialize() must complete before calling HasSingleOrigin()";
    return m_loader->HasSingleOrigin();
}

bool BufferedDataSource::didPassCORSAccessCheck() const
{
    return m_loader.get() && m_loader->DidPassCORSAccessCheck();
}

void BufferedDataSource::abort()
{
    ASSERT(WTF::isMainThread());
    {
        WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
        StopInternal_Locked();
    }
    StopLoader();
    m_frame = NULL;
}

void BufferedDataSource::mediaPlaybackRateChanged(double playback_rate)
{
    ASSERT(WTF::isMainThread());
    ASSERT(m_loader.get());

    if (playback_rate < 0.0)
        return;

    m_playbackRate = playback_rate;
    m_loader->SetPlaybackRate(playback_rate);
}

void BufferedDataSource::mediaIsPlaying()
{
    ASSERT(WTF::isMainThread());
    m_mediaHasPlayed = true;
    UpdateDeferStrategy(false);
}

void BufferedDataSource::mediaIsPaused()
{
    ASSERT(WTF::isMainThread());
    UpdateDeferStrategy(true);
}

/////////////////////////////////////////////////////////////////////////////
// DataSource implementation.
void BufferedDataSource::stop()
{
    {
        WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
        StopInternal_Locked();
    }

    BufferedDataSource* self = this;
    int id = m_id;
    content::postTaskToMainThread(FROM_HERE, [self, id] {
        if (net::ActivatingObjCheck::inst()->isActivating(id))
            self->StopLoader();
    });
}

void BufferedDataSource::setBitrate(int bitrate)
{
    BufferedDataSource* self = this;
    int id = m_id;
    content::postTaskToMainThread(FROM_HERE, [self, bitrate, id] {
        if (net::ActivatingObjCheck::inst()->isActivating(id))
            self->SetBitrateTask(bitrate);
    });
}

void BufferedDataSource::onBufferingHaveEnough()
{
    ASSERT(WTF::isMainThread());
    if (m_loader && m_preload == METADATA && !m_mediaHasPlayed && !isStreaming())
        m_loader->CancelUponDeferral();
}

int64_t BufferedDataSource::getMemoryUsage() const
{
    ASSERT(WTF::isMainThread());
    return m_loader ? m_loader->GetMemoryUsage() : 0;
}

void BufferedDataSource::read(int64 position, int size, char* data, wke::DataSource::ReadCB* readCb)
{
//     DVLOG(1) << "Read: " << position << " offset, " << size << " bytes";
    ASSERT(readCb);

    {
        WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
        ASSERT(!m_readOp);

        if (m_stopSignalReceived) {
            readCb->signalReadCompleted(kReadError);
            return;
        }

        m_readOp.reset(new ReadOperation(position, size, data, readCb));
    }

    BufferedDataSource* self = this;
    int id = m_id;
    content::postTaskToMainThread(FROM_HERE, [self, id] {
        if (net::ActivatingObjCheck::inst()->isActivating(id))
            self->ReadTask();
    });
}

bool BufferedDataSource::getSize(int64* size_out)
{
    if (m_totalBytes != kPositionNotSpecified) {
        *size_out = m_totalBytes;
        return true;
    }
    *size_out = 0;
    return false;
}

bool BufferedDataSource::isStreaming()
{
    return m_streaming;
}

/////////////////////////////////////////////////////////////////////////////
// Render thread tasks.
void BufferedDataSource::ReadTask()
{
    ASSERT(WTF::isMainThread());
    ReadInternal();
}

void BufferedDataSource::StopInternal_Locked()
{
    //m_lock.AssertAcquired();
    //DebugBreak();
    if (m_stopSignalReceived)
        return;

    m_stopSignalReceived = true;

    // Initialize() isn't part of the DataSource interface so don't call it in
    // response to Stop().
    m_initCb = nullptr;

    if (m_readOp)
        ReadOperation::Run(std::move(m_readOp), kReadError);
}

void BufferedDataSource::StopLoader()
{
    ASSERT(WTF::isMainThread());

    if (m_loader)
        m_loader->Stop();
}

void BufferedDataSource::SetBitrateTask(int bitrate)
{
    ASSERT(WTF::isMainThread());
    ASSERT(m_loader.get());

    if (m_willDestroyed)
        return;

    m_bitrate = bitrate;
    m_loader->SetBitrate(bitrate);
}

// This method is the place where actual read happens, |m_loader| must be valid
// prior to make this method call.
void BufferedDataSource::ReadInternal()
{
    ASSERT(WTF::isMainThread());
    int64 position = 0;
    int size = 0;
    {
        WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
        if (m_stopSignalReceived)
            return;

        position = m_readOp->position();
        size = m_readOp->size();
    }

    // First we prepare the intermediate read buffer for BufferedResourceLoader
    // to write to.
    if (static_cast<int>(m_intermediateReadBuffer.size()) < size)
        m_intermediateReadBuffer.resize(size);

    // Perform the actual read with BufferedResourceLoader.
    ASSERT(!m_intermediateReadBuffer.empty());

    int id = m_id;
    BufferedDataSource* self = this;
    m_loader->Read(position, size, &m_intermediateReadBuffer[0], 
        [self, id] (BufferedResourceLoader::Status status, int bytesRead) {
            if (net::ActivatingObjCheck::inst()->isActivating(id))
                self->ReadCallback(status, bytesRead);
        });
}

/////////////////////////////////////////////////////////////////////////////
// BufferedResourceLoader callback methods.
void BufferedDataSource::StartCallback(BufferedResourceLoader::Status status)
{
    ASSERT(WTF::isMainThread());
    ASSERT(m_loader.get());
    if (m_willDestroyed)
        return;

    bool initCbIsNull = false;
    {
        WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
        initCbIsNull = !m_initCb;
    }
    if (initCbIsNull) {
        m_loader->Stop();
        return;
    }
    m_responseOriginalUrl = m_loader->response_original_url();

    // All responses must be successful. Resources that are assumed to be fully
    // buffered must have a known content length.
    bool success = status == BufferedResourceLoader::kOk && (!isAssumeFullyBuffered() || m_loader->instance_size() != kPositionNotSpecified);

    if (success) {
        m_totalBytes = m_loader->instance_size();
        m_streaming =
            !isAssumeFullyBuffered() &&
            (m_totalBytes == kPositionNotSpecified || !m_loader->range_supported());

//         media_log_->SetDoubleProperty("total_bytes", static_cast<double>(m_totalBytes));
//         media_log_->SetBooleanProperty("streaming", m_streaming);
    } else {
        m_loader->Stop();
    }

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
    if (m_stopSignalReceived)
        return;

    if (success) {
        if (m_totalBytes != kPositionNotSpecified) {
            m_host->SetTotalBytes(m_totalBytes);
            if (isAssumeFullyBuffered())
                m_host->AddBufferedByteRange(0, m_totalBytes);
        }

//         media_log_->SetBooleanProperty("single_origin", m_loader->HasSingleOrigin());
//         media_log_->SetBooleanProperty("passed_cors_access_check", m_loader->DidPassCORSAccessCheck());
//         media_log_->SetBooleanProperty("range_header_supported", m_loader->range_supported());
    }

    //render_task_runner_->PostTask(FROM_HERE, base::Bind(base::ResetAndReturn(&init_cb_), success));
	BufferedDataSource* self = this;
	int id = m_id;
    wke::DataSource::InitializeCB* int_cb = m_initCb;
    m_initCb = nullptr;
    content::postTaskToMainThread(FROM_HERE, [self, id, int_cb, success] {
        if (net::ActivatingObjCheck::inst()->isActivating(id))
        int_cb->notifyInitialize(success); // TODO
    });
}

void BufferedDataSource::PartialReadStartCallback(BufferedResourceLoader::Status status)
{
    ASSERT(WTF::isMainThread());
    ASSERT(m_loader.get());
    if (status == BufferedResourceLoader::kOk && CheckPartialResponseURL(m_loader->response_original_url())) {
        // Once the request has started successfully, we can proceed with reading from it.
        ReadInternal();
        return;
    }

    // Stop the resource loader since we have received an error.
    m_loader->Stop();

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
    if (m_stopSignalReceived)
        return;
    ReadOperation::Run(std::move(m_readOp), kReadError);
}

bool BufferedDataSource::CheckPartialResponseURL(const blink::KURL& partial_response_original_url) const
{
    // We check the redirected URL of partial responses in case malicious
    // attackers scan the bytes of other origin resources by mixing their
    // generated bytes and the target response. See http://crbug.com/489060#c32
    // for details.
    // If the origin of the new response is different from the first response we
    // deny the redirected response unless the crossorigin attribute has been set.
//     if ((m_responseOriginalUrl.GetOrigin() == partial_response_original_url.GetOrigin()) ||
//         DidPassCORSAccessCheck()) {
//         return true;
//     }
//     //MEDIA_LOG(ERROR, media_log_) << "BufferedDataSource: origin has changed";
//     return false;
    return true;
}

void BufferedDataSource::ReadCallback(BufferedResourceLoader::Status status, int bytesRead)
{
    ASSERT(WTF::isMainThread());

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
    if (m_stopSignalReceived || m_willDestroyed)
        return;

	BufferedDataSource* self = this;
	int id = m_id;
    if (status != BufferedResourceLoader::kOk) {
        // Stop the resource load if it failed.
        m_loader->Stop();

        if (m_readOp->retries() < kLoaderRetries) {
            // Allow some resiliency against sporadic network failures or intentional
            // cancellations due to a system suspend / resume.  Here we treat failed
            // reads as a cache miss so long as we haven't exceeded max retries.
            if (status == BufferedResourceLoader::kFailed) {
                content::postDelayTaskToMainThread(FROM_HERE, [self, id] {
                    if (net::ActivatingObjCheck::inst()->isActivating(id))
                        self->ReadCallback(BufferedResourceLoader::kCacheMiss, 0);
                }, kLoaderFailedRetryDelayMs);
                return;
            }

            m_readOp->IncrementRetries();

            // Recreate a loader starting from where we last left off until the
            // end of the resource.
            m_loader.reset(CreateResourceLoader(m_readOp->position(), /*kPositionNotSpecified*/kMinBufferCapacity));

            m_loader->Start(
                [self, id](BufferedResourceLoader::Status status) {
                    if (net::ActivatingObjCheck::inst()->isActivating(id))
                        self->PartialReadStartCallback(status);
                },
                [self, id](BufferedResourceLoader::LoadingState state) {
                    if (net::ActivatingObjCheck::inst()->isActivating(id))
                        self->LoadingStateChangedCallback(state);
                },
                [self, id](int64 position) {
                    if (net::ActivatingObjCheck::inst()->isActivating(id))
                        self->ProgressCallback(position);
                },
                m_frame
            );
            return;
        }

        ReadOperation::Run(std::move(m_readOp), kReadError);
        return;
    }

    if (bytesRead > 0) {
        ASSERT(!m_intermediateReadBuffer.empty());
        memcpy(m_readOp->data(), &m_intermediateReadBuffer[0], bytesRead);
    } else if (bytesRead == 0 && m_totalBytes == kPositionNotSpecified) {
        // We've reached the end of the file and we didn't know the total size
        // before. Update the total size so Read()s past the end of the file will
        // fail like they would if we had known the file size at the beginning.
        m_totalBytes = m_loader->instance_size();

        if (m_totalBytes != kPositionNotSpecified) {
            m_host->SetTotalBytes(m_totalBytes);
            m_host->AddBufferedByteRange(m_loader->first_byte_position(), m_totalBytes);
        }
    }
    ReadOperation::Run(std::move(m_readOp), bytesRead);
}

void BufferedDataSource::LoadingStateChangedCallback(BufferedResourceLoader::LoadingState state)
{
    ASSERT(WTF::isMainThread());

    if (m_willDestroyed || isAssumeFullyBuffered())
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

    m_downloadingCb->notifyDownloading(is_downloading_data);
}

void BufferedDataSource::ProgressCallback(int64 position)
{
    ASSERT(WTF::isMainThread());

    if (m_willDestroyed || isAssumeFullyBuffered())
        return;

    // TODO(scherkus): we shouldn't have to lock to signal host(), see
    // http://crbug.com/113712 for details.
    WTF::Locker<WTF::RecursiveMutex> auto_lock(m_lock);
    if (m_stopSignalReceived)
        return;

    m_host->AddBufferedByteRange(m_loader->first_byte_position(), position);
}

void BufferedDataSource::UpdateDeferStrategy(bool paused)
{
    // No need to aggressively buffer when we are assuming the resource is fully
    // buffered.
    if (isAssumeFullyBuffered()) {
        m_loader->UpdateDeferStrategy(BufferedResourceLoader::kCapacityDefer);
        return;
    }

    // If the playback has started (at which point the preload value is ignored)
    // and we're paused, then try to load as much as possible (the loader will
    // fall back to kCapacityDefer if it knows the current response won't be
    // useful from the cache in the future).
    if (m_mediaHasPlayed && paused && m_loader->range_supported()) {
        m_loader->UpdateDeferStrategy(BufferedResourceLoader::kNeverDefer);
        return;
    }

    // If media is currently playing or the page indicated preload=auto or the
    // the server does not support the byte range request or we do not want to go
    // too far ahead of the read head, use threshold strategy to enable/disable
    // deferring when the buffer is full/depleted.
    m_loader->UpdateDeferStrategy(BufferedResourceLoader::kCapacityDefer);
}

}  // namespace media
