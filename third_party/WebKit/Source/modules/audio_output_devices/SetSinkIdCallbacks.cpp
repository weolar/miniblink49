// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/audio_output_devices/SetSinkIdCallbacks.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "modules/audio_output_devices/HTMLMediaElementAudioOutputDevice.h"
#include "platform/Logging.h"
#include "public/platform/WebSetSinkIdError.h"

namespace blink {

SetSinkIdCallbacks::SetSinkIdCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver, HTMLMediaElement& element, const String& sinkId)
    : m_resolver(resolver)
    , m_element(element)
    , m_sinkId(sinkId)
{
    ASSERT(m_resolver);
    WTF_LOG(Media, __FUNCTION__);
}

SetSinkIdCallbacks::~SetSinkIdCallbacks()
{
    WTF_LOG(Media, __FUNCTION__);
}

void SetSinkIdCallbacks::onSuccess()
{
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;

    HTMLMediaElementAudioOutputDevice& aodElement = HTMLMediaElementAudioOutputDevice::from(*m_element);
    aodElement.setSinkId(m_sinkId);
    m_resolver->resolve();
}

void SetSinkIdCallbacks::onError(WebSetSinkIdError* rawError)
{
    ASSERT(rawError);
    OwnPtr<WebSetSinkIdError> error = adoptPtr(rawError);
    if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
        return;

    switch (error->errorType) {
    case WebSetSinkIdError::ErrorTypeNotFound:
        m_resolver->reject(DOMException::create(NotFoundError, error->message));
        break;
    case WebSetSinkIdError::ErrorTypeSecurity:
        m_resolver->reject(DOMException::create(SecurityError, error->message));
        break;
    case WebSetSinkIdError::ErrorTypeNotSupported:
        m_resolver->reject(DOMException::create(NotSupportedError, error->message));
        break;
    case WebSetSinkIdError::ErrorTypeAbort:
        m_resolver->reject(DOMException::create(AbortError, error->message));
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

} // namespace blink
