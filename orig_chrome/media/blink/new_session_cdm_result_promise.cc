// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/new_session_cdm_result_promise.h"

#include "base/logging.h"
#include "media/blink/cdm_result_promise_helper.h"
#include "third_party/WebKit/public/platform/WebString.h"

namespace media {

static blink::WebContentDecryptionModuleResult::SessionStatus ConvertStatus(
    SessionInitStatus status)
{
    switch (status) {
    case SessionInitStatus::UNKNOWN_STATUS:
        break;
    case SessionInitStatus::NEW_SESSION:
        return blink::WebContentDecryptionModuleResult::NewSession;
    case SessionInitStatus::SESSION_NOT_FOUND:
        return blink::WebContentDecryptionModuleResult::SessionNotFound;
    case SessionInitStatus::SESSION_ALREADY_EXISTS:
        return blink::WebContentDecryptionModuleResult::SessionAlreadyExists;
    }
    NOTREACHED();
    return blink::WebContentDecryptionModuleResult::SessionNotFound;
}

NewSessionCdmResultPromise::NewSessionCdmResultPromise(
    const blink::WebContentDecryptionModuleResult& result,
    const std::string& uma_name,
    const SessionInitializedCB& new_session_created_cb)
    : web_cdm_result_(result)
    , uma_name_(uma_name)
    , new_session_created_cb_(new_session_created_cb)
{
}

NewSessionCdmResultPromise::~NewSessionCdmResultPromise()
{
}

void NewSessionCdmResultPromise::resolve(const std::string& session_id)
{
    // |new_session_created_cb_| uses a WeakPtr<> and may not do anything
    // if the session object has been destroyed.
    SessionInitStatus status = SessionInitStatus::UNKNOWN_STATUS;
    new_session_created_cb_.Run(session_id, &status);

    if (status == SessionInitStatus::UNKNOWN_STATUS) {
        reject(MediaKeys::INVALID_STATE_ERROR, 0,
            "Cannot finish session initialization");
        return;
    }

    MarkPromiseSettled();
    ReportCdmResultUMA(uma_name_, SUCCESS);
    web_cdm_result_.completeWithSession(ConvertStatus(status));
}

void NewSessionCdmResultPromise::reject(MediaKeys::Exception exception_code,
    uint32 system_code,
    const std::string& error_message)
{
    MarkPromiseSettled();
    ReportCdmResultUMA(uma_name_,
        ConvertCdmExceptionToResultForUMA(exception_code));
    web_cdm_result_.completeWithError(ConvertCdmException(exception_code),
        system_code,
        blink::WebString::fromUTF8(error_message));
}

} // namespace media
