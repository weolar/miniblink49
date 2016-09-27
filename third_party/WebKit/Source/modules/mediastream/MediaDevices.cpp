// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/mediastream/MediaDevices.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/mediastream/NavigatorMediaStream.h"
#include "modules/mediastream/NavigatorUserMediaErrorCallback.h"
#include "modules/mediastream/NavigatorUserMediaSuccessCallback.h"
#include "modules/mediastream/UserMediaController.h"

namespace blink {

ScriptPromise MediaDevices::enumerateDevices(ScriptState* scriptState)
{
    Document* document = toDocument(scriptState->executionContext());
    UserMediaController* userMedia = UserMediaController::from(document->frame());
    if (!userMedia)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError, "No media device controller available; is this a detached window?"));

    MediaDevicesRequest* request = MediaDevicesRequest::create(scriptState, userMedia);
    return request->start();
}

namespace {

class PromiseSuccessCallback final : public NavigatorUserMediaSuccessCallback {
public:
    PromiseSuccessCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
    }

    ~PromiseSuccessCallback()
    {
    }

    void handleEvent(MediaStream* stream)
    {
        m_resolver->resolve(stream);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_resolver);
        NavigatorUserMediaSuccessCallback::trace(visitor);
    }

private:
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
};

class PromiseErrorCallback final : public NavigatorUserMediaErrorCallback {
public:
    PromiseErrorCallback(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
    }

    ~PromiseErrorCallback()
    {
    }

    void handleEvent(NavigatorUserMediaError* error)
    {
        m_resolver->reject(error);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_resolver);
        NavigatorUserMediaErrorCallback::trace(visitor);
    }

private:
    RefPtrWillBeMember<ScriptPromiseResolver> m_resolver;
};

} // namespace

ScriptPromise MediaDevices::getUserMedia(ScriptState* scriptState, const Dictionary& options, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);

    NavigatorUserMediaSuccessCallback* successCallback = new PromiseSuccessCallback(resolver);
    NavigatorUserMediaErrorCallback* errorCallback = new PromiseErrorCallback(resolver);

    Document* document = toDocument(scriptState->executionContext());
    UserMediaController* userMedia = UserMediaController::from(document->frame());
    if (!userMedia)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError, "No media device controller available; is this a detached window?"));

    UserMediaRequest* request = UserMediaRequest::create(document, userMedia, options, successCallback, errorCallback, exceptionState);
    if (!request) {
        ASSERT(exceptionState.hadException());
        return exceptionState.reject(scriptState);
    }

    request->start();
    return resolver->promise();
}

} // namespace blink
