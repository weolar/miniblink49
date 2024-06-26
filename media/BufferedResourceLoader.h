// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_BUFFERED_RESOURCE_LOADER_H_
#define MEDIA_BLINK_BUFFERED_RESOURCE_LOADER_H_

#include "media/SeekableBuffer.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/public/platform/WebURLLoader.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/web/WebFrame.h"

#include <string>
#include <memory>

namespace media {
class MediaLog;
class SeekableBuffer;

const int64 kPositionNotSpecified = -1;

class ActiveLoader {
public:
    // Creates an ActiveLoader with the given loader. It is assumed that the
    // initial state of |loader| is loading and not deferred.
    ActiveLoader::ActiveLoader(blink::WebURLLoader* loader)
        : loader_(loader),
        deferred_(false) {
    }

    ActiveLoader::~ActiveLoader() {
        loader_->cancel();
    }

    // Starts or stops deferring the resource load.
    void ActiveLoader::SetDeferred(bool deferred) {
        deferred_ = deferred;
        loader_->setDefersLoading(deferred);
    }
    bool deferred() {
        return deferred_;
    }

private:
    friend class BufferedDataSourceTest;

    std::unique_ptr<blink::WebURLLoader> loader_;
    bool deferred_;

    //DISALLOW_IMPLICIT_CONSTRUCTORS(ActiveLoader);
};

// BufferedResourceLoader is single threaded and must be accessed on the
// render thread. It wraps a WebURLLoader and does in-memory buffering,
// pausing resource loading when the in-memory buffer is full and resuming
// resource loading when there is available capacity.
class BufferedResourceLoader : public blink::WebURLLoaderClient {
public:
    // kNeverDefer - Aggresively buffer; never defer loading while paused.
    // kReadThenDefer - Request only enough data to fulfill read requests.
    // kCapacityDefer - Try to keep amount of buffered data at capacity.
    enum DeferStrategy {
        kNeverDefer,
        kReadThenDefer,
        kCapacityDefer,
    };

    // Status codes for start/read operations on BufferedResourceLoader.
    enum Status {
        // Everything went as planned.
        kOk,

        // The operation failed, which may have been due to:
        //   - Page navigation
        //   - Server replied 4xx/5xx
        //   - The response was invalid
        //   - Connection was terminated
        //
        // At this point you should delete the loader.
        kFailed,

        // The loader will never be able to satisfy the read request. Please stop,
        // delete, create a new loader, and try again.
        kCacheMiss,
    };

    // Keep in sync with WebMediaPlayer::CORSMode.
    enum CORSMode {
        kUnspecified, kAnonymous, kUseCredentials
    };

    enum LoadingState {
        kLoading,  // Actively attempting to download data.
        kLoadingDeferred,  // Loading intentionally deferred.
        kLoadingFinished,  // Loading finished normally; no more data will arrive.
        kLoadingFailed,  // Loading finished abnormally; no more data will arrive.
    };

    // |url| - URL for the resource to be loaded.
    // |cors_mode| - HTML media element's crossorigin attribute.
    // |first_byte_position| - First byte to start loading from,
    // |kPositionNotSpecified| for not specified.
    // |last_byte_position| - Last byte to be loaded,
    // |kPositionNotSpecified| for not specified.
    // |strategy| is the initial loading strategy to use.
    // |bitrate| is the bitrate of the media, 0 if unknown.
    // |playback_rate| is the current playback rate of the media.
    BufferedResourceLoader(
        const blink::KURL& url,
        CORSMode cors_mode,
        int64 first_byte_position,
        int64 last_byte_position,
        DeferStrategy strategy,
        int bitrate,
        double playback_rate,
        MediaLog* media_log);
    ~BufferedResourceLoader() override;

    // Start the resource loading with the specified URL and range.
    //
    // |loading_cb| is executed when the loading state has changed.
    // |progress_cb| is executed when additional data has arrived.
    typedef std::function<void(Status)> StartCB;
    typedef std::function<void(LoadingState)> LoadingStateChangedCB;
    typedef std::function<void(int64)> ProgressCB;
    void Start(StartCB&& start_cb, LoadingStateChangedCB&& loading_cb, ProgressCB&& progress_cb, blink::WebFrame* frame);

    // Stops everything associated with this loader, including active URL loads
    // and pending callbacks.
    //
    // It is safe to delete a BufferedResourceLoader after calling Stop().
    void Stop();

    // Copies |read_size| bytes from |position| into |buffer|, executing |read_cb|
    // when the operation has completed.
    //
    // The callback will contain the number of bytes read iff the status is kOk,
    // zero otherwise.
    //
    // If necessary will temporarily increase forward capacity of buffer to
    // accomodate an unusually large read.
    typedef std::function<void(Status, int)> ReadCB;
    void Read(int64 position, int read_size, uint8* buffer, ReadCB&& read_cb);

    // Gets the content length in bytes of the instance after this loader has been
    // started. If this value is |kPositionNotSpecified|, then content length is
    // unknown.
    int64 content_length();

    // Gets the original size of the file requested. If this value is
    // |kPositionNotSpecified|, then the size is unknown.
    int64 instance_size();

    // Returns true if the server supports byte range requests.
    bool range_supported();

    // blink::WebURLLoaderClient implementation.
    void willSendRequest(
        blink::WebURLLoader* loader,
        blink::WebURLRequest& newRequest,
        const blink::WebURLResponse& redirectResponse) override;
    void didSendData(
        blink::WebURLLoader* loader,
        unsigned long long bytesSent,
        unsigned long long totalBytesToBeSent) override;
    void didReceiveResponse(
        blink::WebURLLoader* loader,
        const blink::WebURLResponse& response) override;
    void didDownloadData(
        blink::WebURLLoader* loader,
        int data_length,
        int encoded_data_length) override;
    void didReceiveData(
        blink::WebURLLoader* loader,
        const char* data,
        int data_length,
        int encoded_data_length) override;
    void didReceiveCachedMetadata(
        blink::WebURLLoader* loader,
        const char* data, int dataLength) override;
    void didFinishLoading(
        blink::WebURLLoader* loader,
        double finishTime,
        int64_t total_encoded_data_length) override;
    void didFail(
        blink::WebURLLoader* loader,
        const blink::WebURLError&) override;

    // Returns true if the media resource has a single origin, false otherwise.
    // Only valid to call after Start() has completed.
    bool HasSingleOrigin() const;

    // Returns true if the media resource passed a CORS access control check.
    // Only valid to call after Start() has completed.
    bool DidPassCORSAccessCheck() const;

    // Sets the defer strategy to the given value unless it seems unwise.
    // Specifically downgrade kNeverDefer to kCapacityDefer if we know the
    // current response will not be used to satisfy future requests (the cache
    // won't help us).
    void UpdateDeferStrategy(DeferStrategy strategy);

    // Sets the playback rate to the given value and updates buffer window
    // accordingly.
    void SetPlaybackRate(double playback_rate);

    // Sets the bitrate to the given value and updates buffer window
    // accordingly.
    void SetBitrate(int bitrate);

    // Return the |first_byte_position| passed into the ctor.
    int64 first_byte_position() const;

    // Parse a Content-Range header into its component pieces and return true if
    // each of the expected elements was found & parsed correctly.
    // |*instance_size| may be set to kPositionNotSpecified if the range ends in
    // "/*".
    // NOTE: only public for testing!  This is an implementation detail of
    // VerifyPartialResponse (a private method).
    static bool ParseContentRange(
        const std::string& content_range_str, int64* first_byte_position,
        int64* last_byte_position, int64* instance_size);

    // Cancels and closes any outstanding deferred ActiveLoader instances. Does
    // not report a failed state, so subsequent read calls to cache may still
    // complete okay. If the ActiveLoader is not deferred it will be canceled once
    // it is unless playback starts before then (as determined by the reported
    // playback rate).
    void CancelUponDeferral();

    // Returns the original URL of the response. If the request is redirected to
    // another URL it is the URL after redirected. If the response is generated in
    // a Service Worker it is empty.
    const blink::KURL response_original_url() const {
        return m_responseOriginalUrl;
    }

    // Returns an estimate of the amount of memory owned by the resource loader.
    int64_t GetMemoryUsage() const;

private:
    friend class BufferedDataSourceTest;
    friend class BufferedResourceLoaderTest;
    friend class MockBufferedDataSource;

    // Updates the |m_buffer|'s forward and backward capacities.
    void UpdateBufferWindow();

    // Updates deferring behavior based on current buffering scheme.
    void UpdateDeferBehavior();

    // Sets |m_activeLoader|'s defer state and fires |m_loadingCb| if the state
    // changed.
    void SetDeferred(bool deferred);

    // Returns true if we should defer resource loading based on the current
    // buffering scheme.
    bool ShouldDefer() const;

    // Returns true if the current read request can be fulfilled by what is in
    // the buffer.
    bool CanFulfillRead() const;

    // Returns true if the current read request will be fulfilled in the future.
    bool WillFulfillRead() const;

    // Method that does the actual read and calls the |m_readCb|, assuming the
    // request range is in |m_buffer|.
    void ReadInternal();

    // If we have made a range request, verify the response from the server.
    bool VerifyPartialResponse(const blink::WebURLResponse& response);

    // Done with read. Invokes the read callback and reset parameters for the
    // read request.
    void DoneRead(Status status, int bytes_read);

    // Done with start. Invokes the start callback and reset it.
    void DoneStart(Status status);

    bool HasPendingRead() {
        return !!m_readCb.get();
    }

    // Helper function that returns true if a range request was specified.
    bool IsRangeRequest() const;

    // Log everything interesting to |media_log_|.
    void Log();

    // A sliding window of buffer.
    SeekableBuffer m_buffer;

    // Keeps track of an active WebURLLoader and associated state.
    std::unique_ptr<ActiveLoader> m_activeLoader;

    // Tracks if |m_activeLoader| failed. If so, then all calls to Read() will
    // fail.
    bool m_loaderFailed;

    // Current buffering algorithm in place for resource loading.
    DeferStrategy m_deferStrategy;

    // True if the currently-reading response might be used to satisfy a future
    // request from the cache.
    bool m_mightBeReusedFromCacheIinFuture;

    // True if Range header is supported.
    bool m_rangeSsupported;

    // Forward capacity to reset to after an extension.
    int m_savedForwardCapacity;

    blink::KURL m_url;
    CORSMode m_corsMmode;
    const int64 m_firstBytePosition;
    const int64 m_lastBytePosition;
    bool m_singleOrigin;

    // Executed whenever the state of resource loading has changed.
    std::unique_ptr<LoadingStateChangedCB> m_loadingCb;

    // Executed whenever additional data has been downloaded and reports the
    // zero-indexed file offset of the furthest buffered byte.
    std::unique_ptr<ProgressCB> m_progressCb;

    // Members used during request start.
    std::unique_ptr<StartCB> m_startCb;
    int64 m_offset;
    int64 m_contentLength;
    int64 m_instanceSize;

    // Members used during a read operation. They should be reset after each
    // read has completed or failed.
    std::unique_ptr<ReadCB> m_readCb;
    int64 m_readPosition;
    int m_readSize;
    uint8* m_readBuffer;

    // Offsets of the requested first byte and last byte in |m_buffer|. They are
    // written by Read().
    int m_firstOffset;
    int m_lastOffset;

    // Injected WebURLLoader instance for testing purposes.
    std::unique_ptr<blink::WebURLLoader> m_testLoader;

    // Bitrate of the media. Set to 0 if unknown.
    int m_bitrate;

    // Playback rate of the media.
    double m_playbackRate;

    blink::KURL m_responseOriginalUrl;

    //scoped_refptr<MediaLog> media_log_;

    bool m_cancelUponDeferral;

    int m_id;

    DISALLOW_COPY_AND_ASSIGN(BufferedResourceLoader);
};

}  // namespace media

#endif  // MEDIA_BLINK_BUFFERED_RESOURCE_LOADER_H_
