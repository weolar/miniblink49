/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebMediaPlayerClient_h
#define WebMediaPlayerClient_h

#include "WebMediaPlayer.h"
#include "WebMediaPlayerEncryptedMediaClient.h"

namespace blink {

class WebInbandTextTrack;
class WebLayer;
class WebMediaSource;

// TODO(srirama): Remove this inheritance when we get rid of the MediaPlayer
// and MediaPlayerClient interfaces by having HTMLMediaElement implement
// WebMediaPlayerClient interface. See crbug.com/350571.
class WebMediaPlayerClient : public WebMediaPlayerEncryptedMediaClient {
public:
    enum VideoTrackKind {
        VideoTrackKindNone,
        VideoTrackKindAlternative,
        VideoTrackKindCaptions,
        VideoTrackKindMain,
        VideoTrackKindSign,
        VideoTrackKindSubtitles,
        VideoTrackKindCommentary
    };

    enum AudioTrackKind {
        AudioTrackKindNone,
        AudioTrackKindAlternative,
        AudioTrackKindDescriptions,
        AudioTrackKindMain,
        AudioTrackKindMainDescriptions,
        AudioTrackKindTranslation,
        AudioTrackKindCommentary
    };

    virtual void networkStateChanged() = 0;
    virtual void readyStateChanged() = 0;
    virtual void timeChanged() = 0;
    virtual void repaint() = 0;
    virtual void durationChanged() = 0;
    virtual void sizeChanged() = 0;
    virtual void playbackStateChanged() = 0;
    virtual void setWebLayer(WebLayer*) = 0;
    virtual WebMediaPlayer::TrackId addAudioTrack(const WebString& id, AudioTrackKind, const WebString& label, const WebString& language, bool enabled) = 0;
    virtual void removeAudioTrack(WebMediaPlayer::TrackId) = 0;
    virtual WebMediaPlayer::TrackId addVideoTrack(const WebString& id, VideoTrackKind, const WebString& label, const WebString& language, bool selected) = 0;
    virtual void removeVideoTrack(WebMediaPlayer::TrackId) = 0;
    virtual void addTextTrack(WebInbandTextTrack*) = 0;
    virtual void removeTextTrack(WebInbandTextTrack*) = 0;
    virtual void mediaSourceOpened(WebMediaSource*) = 0;
    virtual void requestSeek(double) = 0;
    virtual void remoteRouteAvailabilityChanged(bool) = 0;
    virtual void connectedToRemoteDevice() = 0;
    virtual void disconnectedFromRemoteDevice() = 0;

protected:
    ~WebMediaPlayerClient() { }
};

} // namespace blink

#endif
