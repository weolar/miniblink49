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

#include "config.h"
#include "modules/mediastream/RTCPeerConnection.h"

#include "bindings/core/v8/ArrayValue.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/frame/LocalFrame.h"
#include "core/html/VoidCallback.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "modules/mediastream/MediaConstraintsImpl.h"
#include "modules/mediastream/MediaStreamEvent.h"
#include "modules/mediastream/RTCDTMFSender.h"
#include "modules/mediastream/RTCDataChannel.h"
#include "modules/mediastream/RTCDataChannelEvent.h"
#include "modules/mediastream/RTCErrorCallback.h"
#include "modules/mediastream/RTCIceCandidateEvent.h"
#include "modules/mediastream/RTCSessionDescription.h"
#include "modules/mediastream/RTCSessionDescriptionCallback.h"
#include "modules/mediastream/RTCSessionDescriptionRequestImpl.h"
#include "modules/mediastream/RTCStatsCallback.h"
#include "modules/mediastream/RTCStatsRequestImpl.h"
#include "modules/mediastream/RTCVoidRequestImpl.h"
#include "platform/mediastream/RTCConfiguration.h"
#include "platform/mediastream/RTCOfferOptions.h"
#include "public/platform/Platform.h"
#include "public/platform/WebMediaStream.h"
#include "public/platform/WebRTCConfiguration.h"
#include "public/platform/WebRTCDataChannelHandler.h"
#include "public/platform/WebRTCDataChannelInit.h"
#include "public/platform/WebRTCICECandidate.h"
#include "public/platform/WebRTCOfferOptions.h"
#include "public/platform/WebRTCSessionDescription.h"
#include "public/platform/WebRTCSessionDescriptionRequest.h"
#include "public/platform/WebRTCStatsRequest.h"
#include "public/platform/WebRTCVoidRequest.h"

namespace blink {

namespace {

static bool throwExceptionIfSignalingStateClosed(RTCPeerConnection::SignalingState state, ExceptionState& exceptionState)
{
    if (state == RTCPeerConnection::SignalingStateClosed) {
        exceptionState.throwDOMException(InvalidStateError, "The RTCPeerConnection's signalingState is 'closed'.");
        return true;
    }

    return false;
}

} // namespace

RTCConfiguration* RTCPeerConnection::parseConfiguration(const Dictionary& configuration, ExceptionState& exceptionState)
{
    if (configuration.isUndefinedOrNull())
        return 0;

    RTCIceTransports iceTransports = RTCIceTransportsAll;
    String iceTransportsString;
    if (DictionaryHelper::get(configuration, "iceTransports", iceTransportsString)) {
        if (iceTransportsString == "none") {
            iceTransports = RTCIceTransportsNone;
        } else if (iceTransportsString == "relay") {
            iceTransports = RTCIceTransportsRelay;
        } else if (iceTransportsString != "all") {
            exceptionState.throwTypeError("Malformed RTCIceTransports");
            return 0;
        }
    }

    ArrayValue iceServers;
    bool ok = DictionaryHelper::get(configuration, "iceServers", iceServers);
    if (!ok || iceServers.isUndefinedOrNull()) {
        exceptionState.throwTypeError("Malformed RTCConfiguration");
        return 0;
    }

    size_t numberOfServers;
    ok = iceServers.length(numberOfServers);
    if (!ok) {
        exceptionState.throwTypeError("Malformed RTCConfiguration");
        return 0;
    }

    RTCBundlePolicy bundlePolicy = RTCBundlePolicyBalanced;
    String bundlePolicyString;
    if (DictionaryHelper::get(configuration, "bundlePolicy", bundlePolicyString)) {
        if (bundlePolicyString == "max-compat") {
            bundlePolicy = RTCBundlePolicyMaxCompat;
        } else if (bundlePolicyString == "max-bundle") {
            bundlePolicy = RTCBundlePolicyMaxBundle;
        } else if (bundlePolicyString != "balanced") {
            exceptionState.throwTypeError("Malformed RTCBundlePolicy");
            return 0;
        }
    }

    RTCRtcpMuxPolicy rtcpMuxPolicy = RTCRtcpMuxPolicyNegotiate;
    String rtcpMuxPolicyString;
    if (DictionaryHelper::get(configuration, "rtcpMuxPolicy", rtcpMuxPolicyString)) {
        if (rtcpMuxPolicyString == "require") {
            rtcpMuxPolicy = RTCRtcpMuxPolicyRequire;
        } else if (rtcpMuxPolicyString != "negotiate") {
            exceptionState.throwTypeError("Malformed RTCRtcpMuxPolicy");
            return 0;
        }
    }

    RTCConfiguration* rtcConfiguration = RTCConfiguration::create();
    rtcConfiguration->setIceTransports(iceTransports);
    rtcConfiguration->setBundlePolicy(bundlePolicy);
    rtcConfiguration->setRtcpMuxPolicy(rtcpMuxPolicy);

    for (size_t i = 0; i < numberOfServers; ++i) {
        Dictionary iceServer;
        ok = iceServers.get(i, iceServer);
        if (!ok) {
            exceptionState.throwTypeError("Malformed RTCIceServer");
            return 0;
        }

        Vector<String> names;
        iceServer.getPropertyNames(names);

        Vector<String> urlStrings;
        if (names.contains("urls")) {
            if (!DictionaryHelper::get(iceServer, "urls", urlStrings) || !urlStrings.size()) {
                String urlString;
                if (DictionaryHelper::get(iceServer, "urls", urlString)) {
                    urlStrings.append(urlString);
                } else {
                    exceptionState.throwTypeError("Malformed RTCIceServer");
                    return 0;
                }
            }
        } else if (names.contains("url")) {
            String urlString;
            if (DictionaryHelper::get(iceServer, "url", urlString)) {
                urlStrings.append(urlString);
            } else {
                exceptionState.throwTypeError("Malformed RTCIceServer");
                return 0;
            }
        } else {
            exceptionState.throwTypeError("Malformed RTCIceServer");
            return 0;
        }

        String username, credential;
        DictionaryHelper::get(iceServer, "username", username);
        DictionaryHelper::get(iceServer, "credential", credential);

        for (Vector<String>::iterator iter = urlStrings.begin(); iter != urlStrings.end(); ++iter) {
            KURL url(KURL(), *iter);
            if (!url.isValid() || !(url.protocolIs("turn") || url.protocolIs("turns") || url.protocolIs("stun"))) {
                exceptionState.throwTypeError("Malformed URL");
                return 0;
            }

            rtcConfiguration->appendServer(RTCIceServer::create(url, username, credential));
        }
    }

    return rtcConfiguration;
}

RTCOfferOptions* RTCPeerConnection::parseOfferOptions(const Dictionary& options, ExceptionState& exceptionState)
{
    if (options.isUndefinedOrNull())
        return 0;

    Vector<String> propertyNames;
    options.getPropertyNames(propertyNames);

    // Treat |options| as MediaConstraints if it is empty or has "optional" or "mandatory" properties for compatibility.
    // TODO(jiayl): remove constraints when RTCOfferOptions reaches Stable and client code is ready.
    if (propertyNames.isEmpty() || propertyNames.contains("optional") || propertyNames.contains("mandatory"))
        return 0;

    int32_t offerToReceiveVideo = -1;
    int32_t offerToReceiveAudio = -1;
    bool voiceActivityDetection = true;
    bool iceRestart = false;

    if (DictionaryHelper::get(options, "offerToReceiveVideo", offerToReceiveVideo) && offerToReceiveVideo < 0) {
        exceptionState.throwTypeError("Invalid offerToReceiveVideo");
        return 0;
    }

    if (DictionaryHelper::get(options, "offerToReceiveAudio", offerToReceiveAudio) && offerToReceiveAudio < 0) {
        exceptionState.throwTypeError("Invalid offerToReceiveAudio");
        return 0;
    }

    DictionaryHelper::get(options, "voiceActivityDetection", voiceActivityDetection);
    DictionaryHelper::get(options, "iceRestart", iceRestart);

    RTCOfferOptions* rtcOfferOptions = RTCOfferOptions::create(offerToReceiveVideo, offerToReceiveAudio, voiceActivityDetection, iceRestart);
    return rtcOfferOptions;
}

RTCPeerConnection* RTCPeerConnection::create(ExecutionContext* context, const Dictionary& rtcConfiguration, const Dictionary& mediaConstraints, ExceptionState& exceptionState)
{
    RTCConfiguration* configuration = parseConfiguration(rtcConfiguration, exceptionState);
    if (exceptionState.hadException())
        return 0;

    WebMediaConstraints constraints = MediaConstraintsImpl::create(mediaConstraints, exceptionState);
    if (exceptionState.hadException())
        return 0;

    RTCPeerConnection* peerConnection = new RTCPeerConnection(context, configuration, constraints, exceptionState);
    peerConnection->suspendIfNeeded();
    if (exceptionState.hadException())
        return 0;

    return peerConnection;
}

RTCPeerConnection::RTCPeerConnection(ExecutionContext* context, RTCConfiguration* configuration, WebMediaConstraints constraints, ExceptionState& exceptionState)
    : ActiveDOMObject(context)
    , m_signalingState(SignalingStateStable)
    , m_iceGatheringState(ICEGatheringStateNew)
    , m_iceConnectionState(ICEConnectionStateNew)
    , m_dispatchScheduledEventRunner(this, &RTCPeerConnection::dispatchScheduledEvent)
    , m_stopped(false)
    , m_closed(false)
{
    Document* document = toDocument(executionContext());

    // If we fail, set |m_closed| and |m_stopped| to true, to avoid hitting the assert in the destructor.

    if (!document->frame()) {
        m_closed = true;
        m_stopped = true;
        exceptionState.throwDOMException(NotSupportedError, "PeerConnections may not be created in detached documents.");
        return;
    }

    m_peerHandler = adoptPtr(Platform::current()->createRTCPeerConnectionHandler(this));
    if (!m_peerHandler) {
        m_closed = true;
        m_stopped = true;
        exceptionState.throwDOMException(NotSupportedError, "No PeerConnection handler can be created, perhaps WebRTC is disabled?");
        return;
    }

    document->frame()->loader().client()->dispatchWillStartUsingPeerConnectionHandler(m_peerHandler.get());

    if (!m_peerHandler->initialize(configuration, constraints)) {
        m_closed = true;
        m_stopped = true;
        exceptionState.throwDOMException(NotSupportedError, "Failed to initialize native PeerConnection.");
        return;
    }
}

RTCPeerConnection::~RTCPeerConnection()
{
    // This checks that close() or stop() is called before the destructor.
    // We are assuming that a wrapper is always created when RTCPeerConnection is created.
    ASSERT(m_closed || m_stopped);
}

void RTCPeerConnection::createOffer(RTCSessionDescriptionCallback* successCallback, RTCErrorCallback* errorCallback, const Dictionary& rtcOfferOptions, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    ASSERT(successCallback);

    RTCOfferOptions* offerOptions = parseOfferOptions(rtcOfferOptions, exceptionState);
    if (exceptionState.hadException())
        return;

    RTCSessionDescriptionRequest* request = RTCSessionDescriptionRequestImpl::create(executionContext(), this, successCallback, errorCallback);

    if (offerOptions) {
        m_peerHandler->createOffer(request, offerOptions);
    } else {
        WebMediaConstraints constraints = MediaConstraintsImpl::create(rtcOfferOptions, exceptionState);
        if (exceptionState.hadException())
            return;

        m_peerHandler->createOffer(request, constraints);
    }
}

void RTCPeerConnection::createAnswer(RTCSessionDescriptionCallback* successCallback, RTCErrorCallback* errorCallback, const Dictionary& mediaConstraints, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    ASSERT(successCallback);

    WebMediaConstraints constraints = MediaConstraintsImpl::create(mediaConstraints, exceptionState);
    if (exceptionState.hadException())
        return;

    RTCSessionDescriptionRequest* request = RTCSessionDescriptionRequestImpl::create(executionContext(), this, successCallback, errorCallback);
    m_peerHandler->createAnswer(request, constraints);
}

void RTCPeerConnection::setLocalDescription(RTCSessionDescription* sessionDescription, VoidCallback* successCallback, RTCErrorCallback* errorCallback, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!sessionDescription) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "RTCSessionDescription"));
        return;
    }

    RTCVoidRequest* request = RTCVoidRequestImpl::create(executionContext(), this, successCallback, errorCallback);
    m_peerHandler->setLocalDescription(request, sessionDescription->webSessionDescription());
}

RTCSessionDescription* RTCPeerConnection::localDescription(ExceptionState& exceptionState)
{
    WebRTCSessionDescription webSessionDescription = m_peerHandler->localDescription();
    if (webSessionDescription.isNull())
        return nullptr;

    return RTCSessionDescription::create(webSessionDescription);
}

void RTCPeerConnection::setRemoteDescription(RTCSessionDescription* sessionDescription, VoidCallback* successCallback, RTCErrorCallback* errorCallback, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!sessionDescription) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "RTCSessionDescription"));
        return;
    }

    RTCVoidRequest* request = RTCVoidRequestImpl::create(executionContext(), this, successCallback, errorCallback);
    m_peerHandler->setRemoteDescription(request, sessionDescription->webSessionDescription());
}

RTCSessionDescription* RTCPeerConnection::remoteDescription(ExceptionState& exceptionState)
{
    WebRTCSessionDescription webSessionDescription = m_peerHandler->remoteDescription();
    if (webSessionDescription.isNull())
        return nullptr;

    return RTCSessionDescription::create(webSessionDescription);
}

void RTCPeerConnection::updateIce(const Dictionary& rtcConfiguration, const Dictionary& mediaConstraints, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    RTCConfiguration* configuration = parseConfiguration(rtcConfiguration, exceptionState);
    if (exceptionState.hadException())
        return;

    WebMediaConstraints constraints = MediaConstraintsImpl::create(mediaConstraints, exceptionState);
    if (exceptionState.hadException())
        return;

    bool valid = m_peerHandler->updateICE(configuration, constraints);
    if (!valid)
        exceptionState.throwDOMException(SyntaxError, "Could not update the ICE Agent with the given configuration.");
}

void RTCPeerConnection::addIceCandidate(RTCIceCandidate* iceCandidate, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!iceCandidate) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "RTCIceCandidate"));
        return;
    }

    bool valid = m_peerHandler->addICECandidate(iceCandidate->webCandidate());
    if (!valid)
        exceptionState.throwDOMException(SyntaxError, "The ICE candidate could not be added.");
}

void RTCPeerConnection::addIceCandidate(RTCIceCandidate* iceCandidate, VoidCallback* successCallback, RTCErrorCallback* errorCallback, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!iceCandidate) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "RTCIceCandidate"));
        return;
    }
    ASSERT(successCallback);
    ASSERT(errorCallback);

    RTCVoidRequest* request = RTCVoidRequestImpl::create(executionContext(), this, successCallback, errorCallback);

    bool implemented = m_peerHandler->addICECandidate(request, iceCandidate->webCandidate());
    if (!implemented) {
        exceptionState.throwDOMException(NotSupportedError, "This method is not yet implemented.");
    }
}

String RTCPeerConnection::signalingState() const
{
    switch (m_signalingState) {
    case SignalingStateStable:
        return "stable";
    case SignalingStateHaveLocalOffer:
        return "have-local-offer";
    case SignalingStateHaveRemoteOffer:
        return "have-remote-offer";
    case SignalingStateHaveLocalPrAnswer:
        return "have-local-pranswer";
    case SignalingStateHaveRemotePrAnswer:
        return "have-remote-pranswer";
    case SignalingStateClosed:
        return "closed";
    }

    ASSERT_NOT_REACHED();
    return String();
}

String RTCPeerConnection::iceGatheringState() const
{
    switch (m_iceGatheringState) {
    case ICEGatheringStateNew:
        return "new";
    case ICEGatheringStateGathering:
        return "gathering";
    case ICEGatheringStateComplete:
        return "complete";
    }

    ASSERT_NOT_REACHED();
    return String();
}

String RTCPeerConnection::iceConnectionState() const
{
    switch (m_iceConnectionState) {
    case ICEConnectionStateNew:
        return "new";
    case ICEConnectionStateChecking:
        return "checking";
    case ICEConnectionStateConnected:
        return "connected";
    case ICEConnectionStateCompleted:
        return "completed";
    case ICEConnectionStateFailed:
        return "failed";
    case ICEConnectionStateDisconnected:
        return "disconnected";
    case ICEConnectionStateClosed:
        return "closed";
    }

    ASSERT_NOT_REACHED();
    return String();
}

void RTCPeerConnection::addStream(MediaStream* stream, const Dictionary& mediaConstraints, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!stream) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "MediaStream"));
        return;
    }

    if (m_localStreams.contains(stream))
        return;

    WebMediaConstraints constraints = MediaConstraintsImpl::create(mediaConstraints, exceptionState);
    if (exceptionState.hadException())
        return;

    m_localStreams.append(stream);

    bool valid = m_peerHandler->addStream(stream->descriptor(), constraints);
    if (!valid)
        exceptionState.throwDOMException(SyntaxError, "Unable to add the provided stream.");
}

void RTCPeerConnection::removeStream(MediaStream* stream, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    if (!stream) {
        exceptionState.throwDOMException(TypeMismatchError, ExceptionMessages::argumentNullOrIncorrectType(1, "MediaStream"));
        return;
    }

    size_t pos = m_localStreams.find(stream);
    if (pos == kNotFound)
        return;

    m_localStreams.remove(pos);

    m_peerHandler->removeStream(stream->descriptor());
}

MediaStreamVector RTCPeerConnection::getLocalStreams() const
{
    return m_localStreams;
}

MediaStreamVector RTCPeerConnection::getRemoteStreams() const
{
    return m_remoteStreams;
}

MediaStream* RTCPeerConnection::getStreamById(const String& streamId)
{
    for (MediaStreamVector::iterator iter = m_localStreams.begin(); iter != m_localStreams.end(); ++iter) {
        if ((*iter)->id() == streamId)
            return iter->get();
    }

    for (MediaStreamVector::iterator iter = m_remoteStreams.begin(); iter != m_remoteStreams.end(); ++iter) {
        if ((*iter)->id() == streamId)
            return iter->get();
    }

    return 0;
}

void RTCPeerConnection::getStats(RTCStatsCallback* successCallback, MediaStreamTrack* selector)
{
    RTCStatsRequest* statsRequest = RTCStatsRequestImpl::create(executionContext(), this, successCallback, selector);
    // FIXME: Add passing selector as part of the statsRequest.
    m_peerHandler->getStats(statsRequest);
}

RTCDataChannel* RTCPeerConnection::createDataChannel(String label, const Dictionary& options, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return nullptr;

    WebRTCDataChannelInit init;
    DictionaryHelper::get(options, "ordered", init.ordered);
    DictionaryHelper::get(options, "negotiated", init.negotiated);

    unsigned short value = 0;
    if (DictionaryHelper::get(options, "id", value))
        init.id = value;
    if (DictionaryHelper::get(options, "maxRetransmits", value))
        init.maxRetransmits = value;
    if (DictionaryHelper::get(options, "maxRetransmitTime", value))
        init.maxRetransmitTime = value;

    String protocolString;
    DictionaryHelper::get(options, "protocol", protocolString);
    init.protocol = protocolString;

    RTCDataChannel* channel = RTCDataChannel::create(executionContext(), this, m_peerHandler.get(), label, init, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    m_dataChannels.append(channel);
    RTCDataChannel::ReadyState handlerState = channel->getHandlerState();
    if (handlerState != RTCDataChannel::ReadyStateConnecting) {
        // There was an early state transition.  Don't miss it!
        channel->didChangeReadyState(handlerState);
    }
    return channel;
}

bool RTCPeerConnection::hasLocalStreamWithTrackId(const String& trackId)
{
    for (MediaStreamVector::iterator iter = m_localStreams.begin(); iter != m_localStreams.end(); ++iter) {
        if ((*iter)->getTrackById(trackId))
            return true;
    }
    return false;
}

RTCDTMFSender* RTCPeerConnection::createDTMFSender(MediaStreamTrack* track, ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return nullptr;

    if (!track) {
        exceptionState.throwTypeError(ExceptionMessages::argumentNullOrIncorrectType(1, "MediaStreamTrack"));
        return nullptr;
    }

    if (!hasLocalStreamWithTrackId(track->id())) {
        exceptionState.throwDOMException(SyntaxError, "No local stream is available for the track provided.");
        return nullptr;
    }

    RTCDTMFSender* dtmfSender = RTCDTMFSender::create(executionContext(), m_peerHandler.get(), track, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    return dtmfSender;
}

void RTCPeerConnection::close(ExceptionState& exceptionState)
{
    if (throwExceptionIfSignalingStateClosed(m_signalingState, exceptionState))
        return;

    closeInternal();
}

void RTCPeerConnection::negotiationNeeded()
{
    ASSERT(!m_closed);
    scheduleDispatchEvent(Event::create(EventTypeNames::negotiationneeded));
}

void RTCPeerConnection::didGenerateICECandidate(const WebRTCICECandidate& webCandidate)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());
    if (webCandidate.isNull())
        scheduleDispatchEvent(RTCIceCandidateEvent::create(false, false, nullptr));
    else {
        RTCIceCandidate* iceCandidate = RTCIceCandidate::create(webCandidate);
        scheduleDispatchEvent(RTCIceCandidateEvent::create(false, false, iceCandidate));
    }
}

void RTCPeerConnection::didChangeSignalingState(SignalingState newState)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());
    changeSignalingState(newState);
}

void RTCPeerConnection::didChangeICEGatheringState(ICEGatheringState newState)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());
    changeIceGatheringState(newState);
}

void RTCPeerConnection::didChangeICEConnectionState(ICEConnectionState newState)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());
    changeIceConnectionState(newState);
}

void RTCPeerConnection::didAddRemoteStream(const WebMediaStream& remoteStream)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());

    if (m_signalingState == SignalingStateClosed)
        return;

    MediaStream* stream = MediaStream::create(executionContext(), remoteStream);
    m_remoteStreams.append(stream);

    scheduleDispatchEvent(MediaStreamEvent::create(EventTypeNames::addstream, false, false, stream));
}

void RTCPeerConnection::didRemoveRemoteStream(const WebMediaStream& remoteStream)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());

    MediaStreamDescriptor* streamDescriptor = remoteStream;
    ASSERT(streamDescriptor->client());

    MediaStream* stream = static_cast<MediaStream*>(streamDescriptor->client());
    stream->streamEnded();

    if (m_signalingState == SignalingStateClosed)
        return;

    size_t pos = m_remoteStreams.find(stream);
    ASSERT(pos != kNotFound);
    m_remoteStreams.remove(pos);

    scheduleDispatchEvent(MediaStreamEvent::create(EventTypeNames::removestream, false, false, stream));
}

void RTCPeerConnection::didAddRemoteDataChannel(WebRTCDataChannelHandler* handler)
{
    ASSERT(!m_closed);
    ASSERT(executionContext()->isContextThread());

    if (m_signalingState == SignalingStateClosed)
        return;

    RTCDataChannel* channel = RTCDataChannel::create(executionContext(), this, adoptPtr(handler));
    m_dataChannels.append(channel);

    scheduleDispatchEvent(RTCDataChannelEvent::create(EventTypeNames::datachannel, false, false, channel));
}

void RTCPeerConnection::releasePeerConnectionHandler()
{
    stop();
}

void RTCPeerConnection::closePeerConnection()
{
    ASSERT(m_signalingState != RTCPeerConnection::SignalingStateClosed);
    closeInternal();
}

const AtomicString& RTCPeerConnection::interfaceName() const
{
    return EventTargetNames::RTCPeerConnection;
}

ExecutionContext* RTCPeerConnection::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

void RTCPeerConnection::suspend()
{
    m_dispatchScheduledEventRunner.suspend();
}

void RTCPeerConnection::resume()
{
    m_dispatchScheduledEventRunner.resume();
}

void RTCPeerConnection::stop()
{
    if (m_stopped)
        return;

    m_stopped = true;
    m_iceConnectionState = ICEConnectionStateClosed;
    m_signalingState = SignalingStateClosed;

    HeapVector<Member<RTCDataChannel>>::iterator i = m_dataChannels.begin();
    for (; i != m_dataChannels.end(); ++i)
        (*i)->stop();
    m_dataChannels.clear();

    m_dispatchScheduledEventRunner.stop();

    m_peerHandler.clear();
}

void RTCPeerConnection::changeSignalingState(SignalingState signalingState)
{
    if (m_signalingState != SignalingStateClosed && m_signalingState != signalingState) {
        m_signalingState = signalingState;
        scheduleDispatchEvent(Event::create(EventTypeNames::signalingstatechange));
    }
}

void RTCPeerConnection::changeIceGatheringState(ICEGatheringState iceGatheringState)
{
    m_iceGatheringState = iceGatheringState;
}

void RTCPeerConnection::changeIceConnectionState(ICEConnectionState iceConnectionState)
{
    if (m_iceConnectionState != ICEConnectionStateClosed && m_iceConnectionState != iceConnectionState) {
        m_iceConnectionState = iceConnectionState;
        scheduleDispatchEvent(Event::create(EventTypeNames::iceconnectionstatechange));
    }
}

void RTCPeerConnection::closeInternal()
{
    ASSERT(m_signalingState != RTCPeerConnection::SignalingStateClosed);
    m_peerHandler->stop();
    m_closed = true;

    changeIceConnectionState(ICEConnectionStateClosed);
    changeIceGatheringState(ICEGatheringStateComplete);
    changeSignalingState(SignalingStateClosed);
}

void RTCPeerConnection::scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    m_scheduledEvents.append(event);

    m_dispatchScheduledEventRunner.runAsync();
}

void RTCPeerConnection::dispatchScheduledEvent()
{
    if (m_stopped)
        return;

    WillBeHeapVector<RefPtrWillBeMember<Event>> events;
    events.swap(m_scheduledEvents);

    WillBeHeapVector<RefPtrWillBeMember<Event>>::iterator it = events.begin();
    for (; it != events.end(); ++it)
        dispatchEvent((*it).release());

    events.clear();
}

DEFINE_TRACE(RTCPeerConnection)
{
    visitor->trace(m_localStreams);
    visitor->trace(m_remoteStreams);
    visitor->trace(m_dataChannels);
#if ENABLE(OILPAN)
    visitor->trace(m_scheduledEvents);
#endif
    RefCountedGarbageCollectedEventTargetWithInlineData<RTCPeerConnection>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
