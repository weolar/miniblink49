// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_
#define MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_

#include <string>
#include <vector>

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/lock.h"
#include "media/base/data_source.h"
#include "media/base/ranges.h"
#include "media/blink/buffered_resource_loader.h"
#include "media/blink/media_blink_export.h"
//#include "url/gurl.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {
class MediaLog;

class MEDIA_BLINK_EXPORT BufferedDataSourceHost {
public:
    // Notify the host of the total size of the media file.
    virtual void SetTotalBytes(int64 total_bytes) = 0;

    // Notify the host that byte range [start,end] has been buffered.
    // TODO(fischman): remove this method when demuxing is push-based instead of
    // pull-based.  http://crbug.com/131444
    virtual void AddBufferedByteRange(int64 start, int64 end) = 0;

protected:
    virtual ~BufferedDataSourceHost() { }
};

// A data source capable of loading URLs and buffering the data using an
// in-memory sliding window.
//
// BufferedDataSource must be created and destroyed on the thread associated
// with the |task_runner| passed in the constructor.
class MEDIA_BLINK_EXPORT BufferedDataSource : public DataSource {
public:
    // Used to specify video preload states. They are "hints" to the browser about
    // how aggressively the browser should load and buffer data.
    // Please see the HTML5 spec for the descriptions of these values:
    // http://www.w3.org/TR/html5/video.html#attr-media-preload
    //
    // Enum values must match the values in blink::WebMediaPlayer::Preload and
    // there will be assertions at compile time if they do not match.
    enum Preload {
        NONE,
        METADATA,
        AUTO,
    };
    typedef base::Callback<void(bool)> DownloadingCB;

    // |url| and |cors_mode| are passed to the object. Buffered byte range changes
    // will be reported to |host|. |downloading_cb| will be called whenever the
    // downloading/paused state of the source changes.
    BufferedDataSource(
        const blink::WebURL& url,
        BufferedResourceLoader::CORSMode cors_mode,
        const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
        blink::WebFrame* frame,
        MediaLog* media_log,
        BufferedDataSourceHost* host,
        const DownloadingCB& downloading_cb);
    ~BufferedDataSource() override;

    // Executes |init_cb| with the result of initialization when it has completed.
    //
    // Method called on the render thread.
    typedef base::Callback<void(bool)> InitializeCB;
    void Initialize(const InitializeCB& init_cb);

    // Adjusts the buffering algorithm based on the given preload value.
    void SetPreload(Preload preload);

    // Returns true if the media resource has a single origin, false otherwise.
    // Only valid to call after Initialize() has completed.
    //
    // Method called on the render thread.
    bool HasSingleOrigin();

    // Returns true if the media resource passed a CORS access control check.
    bool DidPassCORSAccessCheck() const;

    // Cancels initialization, any pending loaders, and any pending read calls
    // from the demuxer. The caller is expected to release its reference to this
    // object and never call it again.
    //
    // Method called on the render thread.
    void Abort();

    // Notifies changes in playback state for controlling media buffering
    // behavior.
    void MediaPlaybackRateChanged(double playback_rate);
    void MediaIsPlaying();
    void MediaIsPaused();
    bool media_has_played() const { return media_has_played_; }

    // Returns true if the resource is local.
    bool assume_fully_buffered() { return !url_.schemeIsHTTPOrHTTPS(); }

    // Cancels any open network connections once reaching the deferred state for
    // preload=metadata, non-streaming resources that have not started playback.
    // If already deferred, connections will be immediately closed.
    void OnBufferingHaveEnough();

    // Returns an estimate of the number of bytes held by the data source.
    int64_t GetMemoryUsage() const;

    // DataSource implementation.
    // Called from demuxer thread.
    void Stop() override;

    void Read(int64 position,
        int size,
        uint8* data,
        const DataSource::ReadCB& read_cb) override;
    bool GetSize(int64* size_out) override;
    bool IsStreaming() override;
    void SetBitrate(int bitrate) override;

protected:
    // A factory method to create a BufferedResourceLoader based on the read
    // parameters. We can override this file to object a mock
    // BufferedResourceLoader for testing.
    virtual BufferedResourceLoader* CreateResourceLoader(
        int64 first_byte_position, int64 last_byte_position);

private:
    friend class BufferedDataSourceTest;

    // Task posted to perform actual reading on the render thread.
    void ReadTask();

    // Cancels oustanding callbacks and sets |stop_signal_received_|. Safe to call
    // from any thread.
    void StopInternal_Locked();

    // Stops |loader_| if present. Used by Abort() and Stop().
    void StopLoader();

    // Tells |loader_| the bitrate of the media.
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
    bool CheckPartialResponseURL(const blink::WebURL& partial_response_original_url) const;

    // BufferedResourceLoader callbacks.
    void ReadCallback(BufferedResourceLoader::Status status, int bytes_read, int need_read);
    void LoadingStateChangedCallback(BufferedResourceLoader::LoadingState state);
    void ProgressCallback(int64 position);

    // Update |loader_|'s deferring strategy in response to a play/pause, or
    // change in playback rate.
    void UpdateDeferStrategy(bool paused);

    // URL of the resource requested.
    blink::WebURL url_;
    // crossorigin attribute on the corresponding HTML media element, if any.
    BufferedResourceLoader::CORSMode cors_mode_;

    // The total size of the resource. Set during StartCallback() if the size is
    // known, otherwise it will remain kPositionNotSpecified until the size is
    // determined by reaching EOF.
    int64 total_bytes_;

    // This value will be true if this data source can only support streaming.
    // i.e. range request is not supported.
    bool streaming_;

    // A webframe for loading.
    blink::WebFrame* frame_;

    // A resource loader for the media resource.
    scoped_ptr<BufferedResourceLoader> loader_;

    // Callback method from the pipeline for initialization.
    InitializeCB init_cb_;

    // Read parameters received from the Read() method call. Must be accessed
    // under |lock_|.
    class ReadOperation;
    scoped_ptr<ReadOperation> read_op_;

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
    std::vector<uint8> intermediate_read_buffer_;

    // The task runner of the render thread.
    const scoped_refptr<base::SingleThreadTaskRunner> render_task_runner_;

    // Protects |stop_signal_received_| and |read_op_|.
    base::Lock lock_;

    // Whether we've been told to stop via Abort() or Stop().
    bool stop_signal_received_;

    // This variable is true when the user has requested the video to play at
    // least once.
    bool media_has_played_;

    // This variable holds the value of the preload attribute for the video
    // element.
    Preload preload_;

    // Bitrate of the content, 0 if unknown.
    int bitrate_;

    // Current playback rate.
    double playback_rate_;

    scoped_refptr<MediaLog> media_log_;

    // Host object to report buffered byte range changes to.
    BufferedDataSourceHost* host_;

    DownloadingCB downloading_cb_;

    // The original URL of the first response. If the request is redirected to
    // another URL it is the URL after redirected. If the response is generated in
    // a Service Worker this URL is empty. BufferedDataSource checks the original
    // URL of each successive response. If the origin URL of it is different from
    // the original URL of the first response, it is treated as an error.
    blink::WebURL response_original_url_;

    // Disallow rebinding WeakReference ownership to a different thread by keeping
    // a persistent reference. This avoids problems with the thread-safety of
    // reaching into this class from multiple threads to attain a WeakPtr.
    base::WeakPtr<BufferedDataSource> weak_ptr_;
    base::WeakPtrFactory<BufferedDataSource> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(BufferedDataSource);
};

} // namespace media

#endif // MEDIA_BLINK_BUFFERED_DATA_SOURCE_H_
