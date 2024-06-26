// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CDM_CALLBACK_PROMISE_H_
#define MEDIA_BASE_CDM_CALLBACK_PROMISE_H_

#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "media/base/cdm_promise.h"
#include "media/base/media_export.h"
#include "media/base/media_keys.h"

namespace media {

typedef base::Callback<void(MediaKeys::Exception exception_code,
    uint32 system_code,
    const std::string& error_message)>
    PromiseRejectedCB;

template <typename... T>
class MEDIA_EXPORT CdmCallbackPromise : public CdmPromiseTemplate<T...> {
public:
    CdmCallbackPromise(const base::Callback<void(const T&...)>& resolve_cb,
        const PromiseRejectedCB& reject_cb);
    virtual ~CdmCallbackPromise();

    // CdmPromiseTemplate<T> implementation.
    virtual void resolve(const T&... result) override;
    virtual void reject(MediaKeys::Exception exception_code,
        uint32 system_code,
        const std::string& error_message) override;

private:
    using CdmPromiseTemplate<T...>::MarkPromiseSettled;

    base::Callback<void(const T&...)> resolve_cb_;
    PromiseRejectedCB reject_cb_;

    DISALLOW_COPY_AND_ASSIGN(CdmCallbackPromise);
};

} // namespace media

#endif // MEDIA_BASE_CDM_CALLBACK_PROMISE_H_
