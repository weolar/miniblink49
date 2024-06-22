
#ifndef wkeMediaPlayer_h
#define wkeMediaPlayer_h

#include "wkedefine.h"

typedef struct HDC__* HDC;

namespace wke {

class FFmpegUrlProtocol {
public:
    virtual void destroy() = 0;

    // Read the given amount of bytes into data, returns the number of bytes read
    // if successful, kReadError otherwise.
    virtual int read(int size, char* data) = 0;

    // Returns true and the current file position for this file, false if the
    // file position could not be retrieved.
    virtual bool getPosition(__int64* position_out) = 0;

    // Returns true if the file position could be set, false otherwise.
    virtual bool setPosition(__int64 position) = 0;

    // Returns true and the file size, false if the file size could not be
    // retrieved.
    virtual bool getSize(__int64* size_out) = 0;

    // Returns false if this protocol supports random seeking.
    virtual bool isStreaming() = 0;
};

class DataSource {
public:
    enum {
        kReadError = -1
    };

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
    virtual void willDestroyed() = 0;
    virtual void destroy() = 0;
    ~DataSource() {}

    class DownloadingCB {
    public:
        virtual void notifyDownloading(bool) = 0;
    };

    class InitializeCB {
    public:
        virtual void notifyInitialize(bool) = 0;
    };

    // Executes |init_cb| with the result of initialization when it has completed.
    // Method called on the render thread.
    virtual void initialize(InitializeCB* initCb) = 0;

    // Adjusts the buffering algorithm based on the given preload value.
    virtual void setPreload(Preload preload) = 0;

    // Returns true if the media resource has a single origin, false otherwise.
    // Only valid to call after Initialize() has completed.
    //
    // Method called on the render thread.
    virtual bool hasSingleOrigin() = 0;

    // Returns true if the media resource passed a CORS access control check.
    virtual bool didPassCORSAccessCheck() const = 0;

    // Cancels initialization, any pending loaders, and any pending read calls
    // from the demuxer. The caller is expected to release its reference to this
    // object and never call it again.
    //
    // Method called on the render thread.
    virtual void abort() = 0;

    // Notifies changes in playback state for controlling media buffering
    // behavior.
    virtual void mediaPlaybackRateChanged(double playbackRate) = 0;
    virtual void mediaIsPlaying() = 0;
    virtual void mediaIsPaused() = 0;
    virtual bool mediaHasPlayed() const = 0;

    // Returns true if the resource is local.
    virtual bool isAssumeFullyBuffered() = 0;

    // Cancels any open network connections once reaching the deferred state for
    // preload=metadata, non-streaming resources that have not started playback.
    // If already deferred, connections will be immediately closed.
    virtual void onBufferingHaveEnough() = 0;

    // Returns an estimate of the number of bytes held by the data source.
    virtual __int64 getMemoryUsage() const = 0;

    // DataSource implementation.
    // Called from demuxer thread.
    virtual void stop() = 0;

    class ReadCB {
    public:
        virtual void signalReadCompleted(int size) = 0;
    };

    virtual void read(__int64 position, int size, char* data, ReadCB* readCb) = 0;
    virtual bool getSize(__int64* size_out) = 0;
    virtual bool isStreaming() = 0;
    virtual void setBitrate(int bitrate) = 0;
};

class DataSourceHost {
public:
    virtual void destroy() = 0;
    virtual ~DataSourceHost() = 0;

    virtual void setTotalBytes(__int64 total_bytes) = 0;
    virtual void addBufferedByteRange(__int64 start, __int64 end) = 0;

    // Translate the byte ranges to time ranges and append them to the list.
    // TODO(sandersd): This is a confusing name, find something better.
    virtual  void addBufferedTimeRanges(void* buffered_time_ranges, double media_duration) = 0;

    virtual bool didLoadingProgress() = 0;
};

class WkeMediaPlayer {
public:
    enum NetworkState {
        kWkeNetworkStateEmpty,
        kWkeNetworkStateIdle,
        kWkeNetworkStateLoading,
        kWkeNetworkStateLoaded,
        kWkeNetworkStateFormatError,
        kWkeNetworkStateNetworkError,
        kWkeNetworkStateDecodeError,
    };

    enum ReadyState {
        kWkeReadyStateHaveNothing,
        kWkeReadyStateHaveMetadata,
        kWkeReadyStateHaveCurrentData,
        kWkeReadyStateHaveFutureData,
        kWkeReadyStateHaveEnoughData,
    };

    enum Preload {
        kWkePreloadNone,
        kWkePreloadMetaData,
        kWkePreloadAuto,
    };

    // Represents synchronous exceptions that can be thrown from the Encrypted
    // Media methods. This is different from the asynchronous MediaKeyError.
    enum MediaKeyException {
        kWkeMediaKeyExceptionNoError,
        kWkeMediaKeyExceptionInvalidPlayerState,
        kWkeMediaKeyExceptionKeySystemNotSupported,
        kWkeMediaKeyExceptionInvalidAccess,
    };

    enum CORSMode {
        kWkeCORSModeUnspecified,
        kWkeCORSModeAnonymous,
        kWkeCORSModeUseCredentials,
    };

    enum LoadType {
        kWkeLoadTypeURL,
        kWkeLoadTypeMediaSource,
        kWkeLoadTypeMediaStream,
    };

//     struct MediaTimeRange {
//         MediaTimeRange() : start(0), end(0) {}
//         MediaTimeRange(double s, double e) : start(s), end(e) {}
// 
//         double start;
//         double end;
//     };

    struct TimeRange {
        TimeRange() : start(0), end(0) { }
        TimeRange(double s, double e) : start(s), end(e) { }

        double start;
        double end;
    };

    class TimeRanges {
    public:
        virtual void destroy() = 0;
        virtual TimeRange* getRange(size_t i) = 0;
        virtual size_t size() = 0;
    };

    typedef unsigned TrackId;

    ~WkeMediaPlayer() {}
    virtual void destroy() = 0;

    virtual void load(LoadType, const char*, CORSMode, bool) = 0;

    // Playback controls.
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual bool supportsSave() const = 0;
    virtual void seek(double seconds) = 0;
    virtual void setRate(double) = 0;
    virtual void setVolume(double) = 0;

    virtual void requestRemotePlayback() {}
    virtual void requestRemotePlaybackControl() {}
    virtual void setPreload(Preload) {}
    virtual wkeMemBuf* buffered() const = 0;
    virtual TimeRanges* seekable() const = 0;

    // Attempts to switch the audio output device.
    // Implementations of setSinkId take ownership of the WebCallbacks
    // object, and the WebCallbacks object takes ownership of the returned
    // error value, if any.
    // Note also that setSinkId implementations must make sure that all
    // methods of the WebCallbacks object, including constructors and
    // destructors, run in the same thread where the object is created
    // (i.e., the blink thread).
    virtual void setSinkId(const char* deviceId, void*) = 0;

    // True if the loaded media has a playable video/audio track.
    virtual bool hasVideo() const = 0;
    virtual bool hasAudio() const = 0;

    // True if the media is being played on a remote device.
    virtual bool isRemote() const { return false; }

    // Dimension of the video.
    virtual wkePoint naturalSize() const = 0;

    // Getters of playback state.
    virtual bool paused() const = 0;
    virtual bool seeking() const = 0;
    virtual double duration() const = 0;
    virtual double currentTime() const = 0;

    // Internal states of loading and network.
    virtual NetworkState networkState() const = 0;
    virtual ReadyState readyState() const = 0;

    virtual bool didLoadingProgress() = 0;

    virtual bool hasSingleSecurityOrigin() const = 0;
    virtual bool didPassCORSAccessCheck() const = 0;

    virtual double mediaTimeForTimeValue(double timeValue) const = 0;

    virtual unsigned decodedFrameCount() const = 0;
    virtual unsigned droppedFrameCount() const = 0;
    virtual unsigned corruptedFrameCount() const { return 0; }
    virtual unsigned audioDecodedByteCount() const = 0;
    virtual unsigned videoDecodedByteCount() const = 0;

    virtual void paint(HDC hdc, const wkeRect& r, unsigned char alpha, int mode) = 0;
    // Do a GPU-GPU textures copy if possible.
    virtual bool copyVideoTextureToPlatformTexture(void*, unsigned texture, unsigned internalFormat, unsigned type, bool premultiplyAlpha, bool flipY) { return false; }

    virtual /*WebAudioSourceProvider*/void* audioSourceProvider() { return nullptr; }

    // Returns whether keySystem is supported. If true, the result will be
    // reported by an event.
    virtual MediaKeyException generateKeyRequest(const char* keySystem, const unsigned char* initData, unsigned initDataLength) { return kWkeMediaKeyExceptionKeySystemNotSupported; }
    virtual MediaKeyException addKey(const char* keySystem, const unsigned char* key, unsigned keyLength, const unsigned char* initData, unsigned initDataLength, const char* sessionId) { return kWkeMediaKeyExceptionKeySystemNotSupported; }
    virtual MediaKeyException cancelKeyRequest(const char* keySystem, const char* sessionId) { return kWkeMediaKeyExceptionKeySystemNotSupported; }
    virtual void setContentDecryptionModule(/*WebContentDecryptionModule*/void* cdm, /*WebContentDecryptionModuleResult*/void* result) { }

    // Sets the poster image URL.
    virtual void setPoster(const char* poster) {}

    // Instruct WebMediaPlayer to enter/exit fullscreen.
    virtual void enterFullscreen() {}

    virtual void enabledAudioTracksChanged(const void* enabledTrackIds) {}
    // |selectedTrackId| is null if no track is selected.
    virtual void selectedVideoTrackChanged(TrackId* selectedTrackId) {}
    
    virtual void setContentsToNativeWindowOffset(int x, int y) = 0;
    virtual bool handleMouseEvent(unsigned msg, unsigned wParam, unsigned lParam) = 0;
    virtual bool handleKeyboardEvent(unsigned msg, unsigned wParam, unsigned lParam) = 0;
    virtual void showMediaControls() = 0;
    virtual void hideMediaControls() = 0;
};

class WkeMediaPlayerClient {
public:
    enum VideoTrackKind {
        kWkeVideoTrackKindNone,
        kWkeVideoTrackKindAlternative,
        kWkeVideoTrackKindCaptions,
        kWkeVideoTrackKindMain,
        kWkeVideoTrackKindSign,
        kWkeVideoTrackKindSubtitles,
        kWkeVideoTrackKindCommentary
    };

    enum AudioTrackKind {
        kWkeAudioTrackKindNone,
        kWkeAudioTrackKindAlternative,
        kWkeAudioTrackKindDescriptions,
        kWkeAudioTrackKindMain,
        kWkeAudioTrackKindMainDescriptions,
        kWkeAudioTrackKindTranslation,
        kWkeAudioTrackKindCommentary
    };

    enum MediaKeyErrorCode {
        kWkeMediaKeyErrorCodeUnknown = 1,
        kWkeMediaKeyErrorCodeClient,
        kWkeMediaKeyErrorCodeService,
        kWkeMediaKeyErrorCodeOutput,
        kWkeMediaKeyErrorCodeHardwareChange,
        kWkeMediaKeyErrorCodeDomain
    };

    enum EncryptedMediaInitDataType {
        kWkeEncryptedMediaInitDataUnknown,
        kWkeEncryptedMediaInitDataCenc,
        kWkeEncryptedMediaInitDataKeyids,
        kWkeEncryptedMediaInitDataWebm,
    };

    // From https://w3c.github.io/encrypted-media/#idl-def-MediaKeySessionType
    enum EncryptedMediaSessionType {
        kWkeEncryptedMediaSessionUnknown,
        kWkeEncryptedMediaSessionTemporary,
        kWkeEncryptedMediaSessionPersistentLicense,
        kWkeEncryptedMediaSessionPersistentReleaseMessage,
    };

    virtual void keyAdded(const char* keySystem, const char* sessionId) = 0;
    virtual void keyError(const char* keySystem, const char* sessionId, MediaKeyErrorCode, unsigned short systemCode) = 0;
    virtual void keyMessage(const char* keySystem, const char* sessionId, const unsigned char* message, unsigned messageLength, const char* defaultURL) = 0;
    virtual void encrypted(EncryptedMediaInitDataType type, const unsigned char* initData, unsigned initDataLength) = 0;
    virtual void didBlockPlaybackWaitingForKey() = 0;
    virtual void didResumePlaybackBlockedForKey() = 0;

    virtual void networkStateChanged() = 0;
    virtual void readyStateChanged() = 0;
    virtual void timeChanged() = 0;
    virtual void repaint() = 0;
    virtual void durationChanged() = 0;
    virtual void sizeChanged() = 0;
    virtual void playbackStateChanged() = 0;
    virtual void setWebLayer(void*) = 0;
    virtual WkeMediaPlayer::TrackId addAudioTrack(const char* id, AudioTrackKind, const char* label, const char* language, bool enabled)
    {
        return 0;
    }
    virtual void removeAudioTrack(WkeMediaPlayer::TrackId) {};
    virtual WkeMediaPlayer::TrackId addVideoTrack(const char* id, VideoTrackKind, const char* label, const char* language, bool selected)
    {
        return 0;
    }
    virtual void removeVideoTrack(WkeMediaPlayer::TrackId) {};
    virtual void addTextTrack(void*) {};
    virtual void removeTextTrack(void*) {};
    virtual void mediaSourceOpened(void*) {};
    virtual void requestSeek(double) {};
    virtual void remoteRouteAvailabilityChanged(bool) {};
    virtual void connectedToRemoteDevice() {};
    virtual void disconnectedFromRemoteDevice() {};

    virtual void didExitFullScreen() {};

    // FFmpeg
    virtual DataSource* createDataSource(const char* url, int corsMode, wke::DataSource::DownloadingCB* downloadingCb) { return nullptr; };

protected:
    ~WkeMediaPlayerClient() {}
};

}

#endif // wkeMediaPlayer_h