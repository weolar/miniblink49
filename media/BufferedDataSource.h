// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_
#define MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_

#include <string>
#include <vector>

#include "media/BufferedResourceLoader.h"
#include "wke/wkeMediaPlayer.h"
#include <functional>
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

namespace blink {
class WebFrame;
}

namespace media {

class BufferedDataSourceHost {
public:
    // Notify the host of the total size of the media file.
    virtual void SetTotalBytes(int64 total_bytes) = 0;

    // Notify the host that byte range [start,end] has been buffered.
    // TODO(fischman): remove this method when demuxing is push-based instead of
    // pull-based.  http://crbug.com/131444
    virtual void AddBufferedByteRange(int64 start, int64 end) = 0;

protected:
    virtual ~BufferedDataSourceHost() {}
};

// A data source capable of loading URLs and buffering the data using an
// in-memory sliding window.
//
// BufferedDataSource must be created and destroyed on the thread associated
// with the |task_runner| passed in the constructor.
class BufferedDataSource : public wke::DataSource {
public:
    class StatusCallback {
    public:
        virtual void run(int64, int64) = 0;
    };

    // |url| and |cors_mode| are passed to the object. Buffered byte range changes
    // will be reported to |host|. |downloading_cb| will be called whenever the
    // downloading/paused state of the source changes.
    BufferedDataSource(
        const blink::KURL& url,
        BufferedResourceLoader::CORSMode cors_mode,
        blink::WebFrame* frame,
        MediaLog* media_log,
        BufferedDataSourceHost* host,
        wke::DataSource::DownloadingCB* downloading_cb);
    ~BufferedDataSource();

    virtual void willDestroyed() override;
    virtual void destroy() override;

    // Executes |init_cb| with the result of initialization when it has completed.
    //
    // Method called on the render thread.
    virtual void initialize(wke::DataSource::InitializeCB* initCb) override;

    // Adjusts the buffering algorithm based on the given preload value.
    void setPreload(wke::DataSource::Preload preload);

    // Returns true if the media resource has a single origin, false otherwise.
    // Only valid to call after Initialize() has completed.
    //
    // Method called on the render thread.
    virtual bool hasSingleOrigin() override;

    // Returns true if the media resource passed a CORS access control check.
    virtual bool didPassCORSAccessCheck() const override;

    // Cancels initialization, any pending loaders, and any pending read calls
    // from the demuxer. The caller is expected to release its reference to this
    // object and never call it again.
    //
    // Method called on the render thread.
    virtual void abort() override;

    // Notifies changes in playback state for controlling media buffering
    // behavior.
    virtual void mediaPlaybackRateChanged(double playback_rate) override;
    virtual void mediaIsPlaying() override;
    virtual void mediaIsPaused() override;
    virtual bool mediaHasPlayed() const override { return m_mediaHasPlayed; }

    // Returns true if the resource is local.
    virtual bool isAssumeFullyBuffered() { return !m_url.protocolIsInHTTPFamily(); }

    // Cancels any open network connections once reaching the deferred state for
    // preload=metadata, non-streaming resources that have not started playback.
    // If already deferred, connections will be immediately closed.
    virtual void onBufferingHaveEnough() override;

    // Returns an estimate of the number of bytes held by the data source.
    virtual int64_t getMemoryUsage() const override;

    // DataSource implementation.
    // Called from demuxer thread.
    virtual void stop() override;

    virtual void read(int64 position, int size, char* data, wke::DataSource::ReadCB* read_cb) override;
    virtual bool getSize(int64* size_out) override;
    virtual bool isStreaming() override;
    virtual void setBitrate(int bitrate) override;

protected:
    // A factory method to create a BufferedResourceLoader based on the read
    // parameters. We can override this file to object a mock
    // BufferedResourceLoader for testing.
    virtual BufferedResourceLoader* CreateResourceLoader(int64 first_byte_position, int64 last_byte_position);

private:
    friend class BufferedDataSourceTest;

    // Task posted to perform actual reading on the render thread.
    void ReadTask();

    // Cancels oustanding callbacks and sets |m_stopSignalReceived|. Safe to call
    // from any thread.
    void StopInternal_Locked();

    // Stops |m_loader| if present. Used by Abort() and Stop().
    void StopLoader();

    // Tells |m_loader| the bitrate of the media.
    void SetBitrateTask(int bitrate);

    // The method that performs actual read. This method can only be executed on
    // the render thread.
    void ReadInternal();

    // BufferedResourceLoader::Start() callback for initial load.
    void StartCallback(BufferedResourceLoader::Status status);

    // BufferedResourceLoader::Start() callback for subsequent loads (i.e.,
    // when accessing ranges that are outside initial buffered region).
    void PartialReadStartCallback(BufferedResourceLoader::Status status);

    // Returns true if we can accept the new partial response.
    bool CheckPartialResponseURL(const blink::KURL& partial_response_original_url) const;

    // BufferedResourceLoader callbacks.
    void ReadCallback(BufferedResourceLoader::Status status, int bytes_read);
    void LoadingStateChangedCallback(BufferedResourceLoader::LoadingState state);
    void ProgressCallback(int64 position);

    // Update |m_loader|'s deferring strategy in response to a play/pause, or
    // change in playback rate.
    void UpdateDeferStrategy(bool paused);

    // URL of the resource requested.
    blink::KURL m_url;
    // crossorigin attribute on the corresponding HTML media element, if any.
    BufferedResourceLoader::CORSMode m_corsMode;

    // The total size of the resource. Set during StartCallback() if the size is
    // known, otherwise it will remain kPositionNotSpecified until the size is
    // determined by reaching EOF.
    int64 m_totalBytes;

    // This value will be true if this data source can only support streaming.
    // i.e. range request is not supported.
    bool m_streaming;

    // A webframe for loading.
    blink::WebFrame* m_frame;

    // A resource loader for the media resource.
    std::unique_ptr<BufferedResourceLoader> m_loader;

    // Callback method from the pipeline for initialization.
    wke::DataSource::InitializeCB* m_initCb;

    // Read parameters received from the Read() method call. Must be accessed
    // under |m_lock|.
    class ReadOperation;
    std::unique_ptr<ReadOperation> m_readOp;

    // This buffer is intermediate, we use it for BufferedResourceLoader to write
    // to. And when read in BufferedResourceLoader is done, we copy data from
    // this buffer to |read_buffer_|. The reason for an additional copy is that
    // we don't own |read_buffer_|. But since the read operation is asynchronous,
    // |read_buffer| can be destroyed at any time, so we only copy into
    // |read_buffer| in the final step when it is safe.
    // Memory is allocated for this member during initialization of this object
    // because we want buffer to be passed into BufferedResourceLoader to be
    // always non-null. And by initializing this member with a default size we can
    // avoid creating zero-sized buffered if the first read has zero size.
    std::vector<uint8> m_intermediateReadBuffer;

    // The task runner of the render thread.
    //const scoped_refptr<base::SingleThreadTaskRunner> render_task_runner_;

    // Protects |m_stopSignalReceived| and |m_readOp|.
    WTF::RecursiveMutex m_lock;

    // Whether we've been told to stop via Abort() or Stop().
    bool m_stopSignalReceived;

    // This variable is true when the user has requested the video to play at
    // least once.
    bool m_mediaHasPlayed;

    // This variable holds the value of the preload attribute for the video
    // element.
    Preload m_preload;

    // Bitrate of the content, 0 if unknown.
    int m_bitrate;

    // Current playback rate.
    double m_playbackRate;

    //scoped_refptr<MediaLog> media_log_;

    // Host object to report buffered byte range changes to.
    BufferedDataSourceHost* m_host;

    wke::DataSource::DownloadingCB* m_downloadingCb;

    // The original URL of the first response. If the request is redirected to
    // another URL it is the URL after redirected. If the response is generated in
    // a Service Worker this URL is empty. BufferedDataSource checks the original
    // URL of each successive response. If the origin URL of it is different from
    // the original URL of the first response, it is treated as an error.
    blink::KURL m_responseOriginalUrl;

    int m_id;
    bool m_willDestroyed;

    // Disallow rebinding WeakReference ownership to a different thread by keeping
    // a persistent reference. This avoids problems with the thread-safety of
    // reaching into this class from multiple threads to attain a WeakPtr.
//     base::WeakPtr<BufferedDataSource> weak_ptr_;
//     base::WeakPtrFactory<BufferedDataSource> weak_factory_;
// 
//     DISALLOW_COPY_AND_ASSIGN(BufferedDataSource);
};

}  // namespace media

#endif  // MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_
