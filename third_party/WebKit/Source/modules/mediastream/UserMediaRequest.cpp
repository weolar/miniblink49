/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
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
 * 3. Neither the name of Ericsson nor the names of its contributors
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

#include "modules/mediastream/UserMediaRequest.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/SpaceSplitString.h"
#include "modules/mediastream/MediaConstraintsImpl.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/mediastream/UserMediaController.h"
#include "platform/mediastream/MediaStreamCenter.h"
#include "platform/mediastream/MediaStreamDescriptor.h"

namespace blink {

static WebMediaConstraints parseOptions(const Dictionary& options, const String& mediaType, ExceptionState& exceptionState)
{
    WebMediaConstraints constraints;

    Dictionary constraintsDictionary;
    bool ok = options.get(mediaType, constraintsDictionary);
    if (ok && !constraintsDictionary.isUndefinedOrNull())
        constraints = MediaConstraintsImpl::create(constraintsDictionary, exceptionState);
    else {
        bool mediaRequested = false;
        DictionaryHelper::get(options, mediaType, mediaRequested);
        if (mediaRequested)
            constraints = MediaConstraintsImpl::create();
    }

    return constraints;
}

UserMediaRequest* UserMediaRequest::create(ExecutionContext* context, UserMediaController* controller, const Dictionary& options, NavigatorUserMediaSuccessCallback* successCallback, NavigatorUserMediaErrorCallback* errorCallback, ExceptionState& exceptionState)
{
    WebMediaConstraints audio = parseOptions(options, "audio", exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    WebMediaConstraints video = parseOptions(options, "video", exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    if (audio.isNull() && video.isNull()) {
        exceptionState.throwDOMException(SyntaxError, "At least one of audio and video must be requested");
        return nullptr;
    }

    return new UserMediaRequest(context, controller, audio, video, successCallback, errorCallback);
}

UserMediaRequest::UserMediaRequest(ExecutionContext* context, UserMediaController* controller, WebMediaConstraints audio, WebMediaConstraints video, NavigatorUserMediaSuccessCallback* successCallback, NavigatorUserMediaErrorCallback* errorCallback)
    : ContextLifecycleObserver(context)
    , m_audio(audio)
    , m_video(video)
    , m_controller(controller)
    , m_successCallback(successCallback)
    , m_errorCallback(errorCallback)
{
}

UserMediaRequest::~UserMediaRequest()
{
}

bool UserMediaRequest::audio() const
{
    return !m_audio.isNull();
}

bool UserMediaRequest::video() const
{
    return !m_video.isNull();
}

WebMediaConstraints UserMediaRequest::audioConstraints() const
{
    return m_audio;
}

WebMediaConstraints UserMediaRequest::videoConstraints() const
{
    return m_video;
}

Document* UserMediaRequest::ownerDocument()
{
    if (ExecutionContext* context = executionContext()) {
        return toDocument(context);
    }

    return 0;
}

void UserMediaRequest::start()
{
    if (m_controller)
        m_controller->requestUserMedia(this);
}

void UserMediaRequest::succeed(PassRefPtr<MediaStreamDescriptor> streamDescriptor)
{
    if (!executionContext())
        return;

    RefPtrWillBeRawPtr<MediaStream> stream = MediaStream::create(executionContext(), streamDescriptor);

    MediaStreamTrackVector audioTracks = stream->getAudioTracks();
    for (MediaStreamTrackVector::iterator iter = audioTracks.begin(); iter != audioTracks.end(); ++iter) {
        (*iter)->component()->source()->setConstraints(m_audio);
    }

    MediaStreamTrackVector videoTracks = stream->getVideoTracks();
    for (MediaStreamTrackVector::iterator iter = videoTracks.begin(); iter != videoTracks.end(); ++iter) {
        (*iter)->component()->source()->setConstraints(m_video);
    }

    m_successCallback->handleEvent(stream.get());
}

void UserMediaRequest::failPermissionDenied(const String& message)
{
    if (!executionContext())
        return;
    m_errorCallback->handleEvent(NavigatorUserMediaError::create(NavigatorUserMediaError::NamePermissionDenied, message, String()));
}

void UserMediaRequest::failConstraint(const String& constraintName, const String& message)
{
    ASSERT(!constraintName.isEmpty());
    if (!executionContext())
        return;
    m_errorCallback->handleEvent(NavigatorUserMediaError::create(NavigatorUserMediaError::NameConstraintNotSatisfied, message, constraintName));
}

void UserMediaRequest::failUASpecific(const String& name, const String& message, const String& constraintName)
{
    ASSERT(!name.isEmpty());
    if (!executionContext())
        return;
    m_errorCallback->handleEvent(NavigatorUserMediaError::create(name, message, constraintName));
}

void UserMediaRequest::contextDestroyed()
{
    if (m_controller) {
        m_controller->cancelUserMediaRequest(this);
        m_controller = nullptr;
    }

    ContextLifecycleObserver::contextDestroyed();
}

DEFINE_TRACE(UserMediaRequest)
{
    visitor->trace(m_controller);
    visitor->trace(m_successCallback);
    visitor->trace(m_errorCallback);
    ContextLifecycleObserver::trace(visitor);
}

} // namespace blink
