// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_NEW_SESSION_CDM_RESULT_PROMISE_H_
#define MEDIA_BLINK_NEW_SESSION_CDM_RESULT_PROMISE_H_

#include <string>

#include "base/basictypes.h"
#include "media/base/cdm_promise.h"
#include "media/base/media_keys.h"
#include "media/blink/media_blink_export.h"
#include "third_party/WebKit/public/platform/WebContentDecryptionModuleResult.h"

namespace media {

enum class SessionInitStatus {
    // Error creating the session.
    UNKNOWN_STATUS,

    // New session has been initialized.
    NEW_SESSION,

    // CDM could not find the requested session.
    SESSION_NOT_FOUND,

    // CDM already has a non-closed session that matches the provided
    // parameters.
    SESSION_ALREADY_EXISTS
};

typedef base::Callback<void(const std::string& session_id,
    SessionInitStatus* status)>
    SessionInitializedCB;

// Special class for resolving a new session promise. Resolving a new session
// promise returns the session ID (as a string), but the blink promise needs
// to get passed a SessionStatus. This class converts the session id to a
// SessionStatus by calling |new_session_created_cb|.
class MEDIA_BLINK_EXPORT NewSessionCdmResultPromise
    : public CdmPromiseTemplate<std::string> {
public:
    NewSessionCdmResultPromise(
        const blink::WebContentDecryptionModuleResult& result,
        const std::string& uma_name,
        const SessionInitializedCB& new_session_created_cb);
    ~NewSessionCdmResultPromise() override;

    // CdmPromiseTemplate<T> implementation.
    void resolve(const std::string& session_id) override;
    void reject(MediaKeys::Exception exception_code,
        uint32 system_code,
        const std::string& error_message) override;

private:
    blink::WebContentDecryptionModuleResult web_cdm_result_;

    // UMA name to report result to.
    std::string uma_name_;

    // Called on resolve() to convert the session ID into a SessionInitStatus to
    // be reported to blink.
    SessionInitializedCB new_session_created_cb_;

    DISALLOW_COPY_AND_ASSIGN(NewSessionCdmResultPromise);
};

} // namespace media

#endif // MEDIA_BLINK_NEW_SESSION_CDM_RESULT_PROMISE_H_
