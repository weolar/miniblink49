
#ifndef media_SimpleDataSource_h
#define media_SimpleDataSource_h

#include "media/BufferedResourceLoader.h"
#include "wke/wkeMediaPlayer.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/Locker.h"
#include "third_party/WebKit/public/platform/WebMediaPlayer.h"
#include <string>
#include <vector>
#include <functional>

namespace media {

class SimpleResourceLoader;
class SeekableBuf;

class SimpleDataSource : public wke::DataSource {
public:
    class StatusCallback {
    public:
        virtual void run(int64, int64) = 0;
    };

    // |url| and |cors_mode| are passed to the object. Buffered byte range changes
    // will be reported to |host|. |downloading_cb| will be called whenever the
    // downloading/paused state of the source changes.
    SimpleDataSource(
        const blink::KURL& url,
        blink::WebMediaPlayer::CORSMode corsMode,
        blink::WebFrame* frame,
        MediaLog* media_log,
        /*BufferedDataSourceHost*/void* host,
        wke::DataSource::DownloadingCB* downloadingCb);
    ~SimpleDataSource();

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
    virtual bool mediaHasPlayed() const override {
        return m_mediaHasPlayed;
    }

    // Returns true if the resource is local.
    virtual bool isAssumeFullyBuffered() {
        return !m_url.protocolIsInHTTPFamily();
    }

    // Cancels any open network connections once reaching the deferred state for
    // preload=metadata, non-streaming resources that have not started playback.
    // If already deferred, connections will be immediately closed.
    virtual void onBufferingHaveEnough() override;

    // Returns an estimate of the number of bytes held by the data source.
    virtual int64_t getMemoryUsage() const override;

    // DataSource implementation.
    // Called from demuxer thread.
    virtual void stop() override;

    virtual void read(int64 position, int size, char* data, wke::DataSource::ReadCB* readCb) override;
    virtual bool getSize(int64* sizeOut) override;
    virtual bool isStreaming() override;
    virtual void setBitrate(int bitrate) override;

    SimpleResourceLoader* createResourceLoader(size_t firstBytePosition, size_t lastBytePosition);

    enum LoadingState {
        kLoading,  // Actively attempting to download data.
        kLoadingDeferred,  // Loading intentionally deferred.
        kLoadingFinished,  // Loading finished normally; no more data will arrive.
        kLoadingFailed,  // Loading finished abnormally; no more data will arrive.
    };
    void loadingStateChangedCallback(LoadingState state);

    // for loader
    void onLoaderResponse(bool isSuccess);
    void onLoaderDataRecv(const char* data, size_t size);
    void onLoaderFinish(size_t size);

private:
    void notifyFinishCallback(bool ok);

private:
    int m_id;
    bool m_mediaHasPlayed;

    blink::WebFrame* m_frame;
    wke::DataSource::InitializeCB* m_initCb;
    wke::DataSource::DownloadingCB* m_downloadingCb;
    wke::DataSource::ReadCB* m_readCb;

    size_t m_paddingReadSize;
    char* m_paddingReadData; // 外界调用read后，发送网络请求，然后等请求结束了，把数据拷贝给这个

    blink::KURL m_url;
    Preload m_preload;
    blink::WebMediaPlayer::CORSMode m_corsMode;
    int m_bitrate;
    double m_playbackRate;

    size_t m_readBeginPos; // 正在loader里读的开始位置
    size_t m_readEndPos;   // 正在loader里读的结束位置
    size_t m_offset;       // 接受到的网络数据在整个mp4里的偏移
    size_t m_totalBytes;   // 总大小

    std::unique_ptr<SeekableBuf> m_seekableBuf;
    std::unique_ptr<SeekableBuf> m_testSeekableBuf;
    std::unique_ptr<SimpleResourceLoader> m_loader;

    WTF::RecursiveMutex m_readLock;
};

}

#endif // media_SimpleDataSource_h