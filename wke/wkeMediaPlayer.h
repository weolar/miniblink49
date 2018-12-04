
#ifndef wkeMediaPlayer_h
#define wkeMediaPlayer_h

#include "wkedefine.h"

typedef struct HDC__* HDC;

namespace wke {

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

    struct MediaTimeRange {
        MediaTimeRange() : start(0), end(0) {}
        MediaTimeRange(double s, double e) : start(s), end(e) {}

        double start;
        double end;
    };

    typedef unsigned TrackId;

    ~WkeMediaPlayer() {}
    virtual void destroy() = 0;

    virtual void load(LoadType, const char*, CORSMode) = 0;

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
    virtual wkeMemBuf* seekable() const = 0;

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

protected:
    ~WkeMediaPlayerClient() {}
};

}

#endif // wkeMediaPlayer_h