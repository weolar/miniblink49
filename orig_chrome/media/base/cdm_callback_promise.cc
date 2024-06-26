// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/cdm_callback_promise.h"

#include "base/logging.h"

namespace media {

template <typename... T>
CdmCallbackPromise<T...>::CdmCallbackPromise(
    const base::Callback<void(const T&...)>& resolve_cb,
    const PromiseRejectedCB& reject_cb)
    : resolve_cb_(resolve_cb)
    , reject_cb_(reject_cb)
{
    DCHECK(!resolve_cb_.is_null());
    DCHECK(!reject_cb_.is_null());
}

template <typename... T>
CdmCallbackPromise<T...>::~CdmCallbackPromise()
{
}

template <typename... T>
void CdmCallbackPromise<T...>::resolve(const T&... result)
{
    MarkPromiseSettled();
    resolve_cb_.Run(result...);
}

template <typename... T>
void CdmCallbackPromise<T...>::reject(MediaKeys::Exception exception_code,
    uint32 system_code,
    const std::string& error_message)
{
    MarkPromiseSettled();
    reject_cb_.Run(exception_code, system_code, error_message);
}

// Explicit template instantiation for the Promises needed.
template class MEDIA_EXPORT CdmCallbackPromise<>;
template class MEDIA_EXPORT CdmCallbackPromise<std::string>;

} // namespace media
