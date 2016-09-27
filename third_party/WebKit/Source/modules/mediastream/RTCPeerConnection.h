/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Google Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#ifndef RTCPeerConnection_h
#define RTCPeerConnection_h

#include "bindings/core/v8/Dictionary.h"
#include "core/dom/ActiveDOMObject.h"
#include "modules/EventTargetModules.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/mediastream/RTCIceCandidate.h"
#include "platform/AsyncMethodRunner.h"
#include "public/platform/WebMediaConstraints.h"
#include "public/platform/WebRTCPeerConnectionHandler.h"
#include "public/platform/WebRTCPeerConnectionHandlerClient.h"

namespace blink {

class ExceptionState;
class MediaStreamTrack;
class RTCConfiguration;
class RTCDTMFSender;
class RTCDataChannel;
class RTCErrorCallback;
class RTCOfferOptions;
class RTCSessionDescription;
class RTCSessionDescriptionCallback;
class RTCStatsCallback;
class VoidCallback;

class RTCPeerConnection final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<RTCPeerConnection>
    , public WebRTCPeerConnectionHandlerClient
    , public ActiveDOMObject {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(RTCPeerConnection);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(RTCPeerConnection);
public:
    static RTCPeerConnection* create(ExecutionContext*, const Dictionary&, const Dictionary&, ExceptionState&);
    ~RTCPeerConnection() override;

    void createOffer(RTCSessionDescriptionCallback*, RTCErrorCallback*, const Dictionary&, ExceptionState&);

    void createAnswer(RTCSessionDescriptionCallback*, RTCErrorCallback*, const Dictionary&, ExceptionState&);

    void setLocalDescription(RTCSessionDescription*, VoidCallback*, RTCErrorCallback*, ExceptionState&);
    RTCSessionDescription* localDescription(ExceptionState&);

    void setRemoteDescription(RTCSessionDescription*, VoidCallback*, RTCErrorCallback*, ExceptionState&);
    RTCSessionDescription* remoteDescription(ExceptionState&);

    String signalingState() const;

    void updateIce(const Dictionary& rtcConfiguration, const Dictionary& mediaConstraints, ExceptionState&);

    // DEPRECATED
    void addIceCandidate(RTCIceCandidate*, ExceptionState&);

    void addIceCandidate(RTCIceCandidate*, VoidCallback*, RTCErrorCallback*, ExceptionState&);

    String iceGatheringState() const;

    String iceConnectionState() const;

    MediaStreamVector getLocalStreams() const;

    MediaStreamVector getRemoteStreams() const;

    MediaStream* getStreamById(const String& streamId);

    void addStream(MediaStream*, const Dictionary& mediaConstraints, ExceptionState&);

    void removeStream(MediaStream*, ExceptionState&);

    void getStats(RTCStatsCallback* successCallback, MediaStreamTrack* selector);

    RTCDataChannel* createDataChannel(String label, const Dictionary& dataChannelDict, ExceptionState&);

    RTCDTMFSender* createDTMFSender(MediaStreamTrack*, ExceptionState&);

    void close(ExceptionState&);

    // We allow getStats after close, but not other calls or callbacks.
    bool shouldFireDefaultCallbacks() { return !m_closed && !m_stopped; }
    bool shouldFireGetStatsCallback() { return !m_stopped; }

    DEFINE_ATTRIBUTE_EVENT_LISTENER(negotiationneeded);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(icecandidate);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(signalingstatechange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(addstream);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(removestream);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(iceconnectionstatechange);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(datachannel);

    // WebRTCPeerConnectionHandlerClient
    void negotiationNeeded() override;
    void didGenerateICECandidate(const WebRTCICECandidate&) override;
    void didChangeSignalingState(SignalingState) override;
    void didChangeICEGatheringState(ICEGatheringState) override;
    void didChangeICEConnectionState(ICEConnectionState) override;
    void didAddRemoteStream(const WebMediaStream&) override;
    void didRemoveRemoteStream(const WebMediaStream&) override;
    void didAddRemoteDataChannel(WebRTCDataChannelHandler*) override;
    void releasePeerConnectionHandler() override;
    void closePeerConnection() override;

    // EventTarget
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // ActiveDOMObject
    void suspend() override;
    void resume() override;
    void stop() override;
    // We keep the this object alive until either stopped or closed.
    bool hasPendingActivity() const override
    {
        return !m_closed && !m_stopped;
    }

    // Oilpan: need to eagerly finalize m_peerHandler
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

private:
    RTCPeerConnection(ExecutionContext*, RTCConfiguration*, WebMediaConstraints, ExceptionState&);

    static RTCConfiguration* parseConfiguration(const Dictionary&, ExceptionState&);
    static RTCOfferOptions* parseOfferOptions(const Dictionary&, ExceptionState&);

    void scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event>);
    void dispatchScheduledEvent();
    bool hasLocalStreamWithTrackId(const String& trackId);

    void changeSignalingState(WebRTCPeerConnectionHandlerClient::SignalingState);
    void changeIceGatheringState(WebRTCPeerConnectionHandlerClient::ICEGatheringState);
    void changeIceConnectionState(WebRTCPeerConnectionHandlerClient::ICEConnectionState);

    void closeInternal();

    SignalingState m_signalingState;
    ICEGatheringState m_iceGatheringState;
    ICEConnectionState m_iceConnectionState;

    MediaStreamVector m_localStreams;
    MediaStreamVector m_remoteStreams;

    HeapVector<Member<RTCDataChannel>> m_dataChannels;

    OwnPtr<WebRTCPeerConnectionHandler> m_peerHandler;

    AsyncMethodRunner<RTCPeerConnection> m_dispatchScheduledEventRunner;
    WillBeHeapVector<RefPtrWillBeMember<Event>> m_scheduledEvents;

    bool m_stopped;
    bool m_closed;
};

} // namespace blink

#endif // RTCPeerConnection_h
