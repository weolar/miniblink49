// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/cdm_promise_adapter.h"

#include "media/base/media_keys.h"

namespace media {

CdmPromiseAdapter::CdmPromiseAdapter()
    : next_promise_id_(1)
{
}

CdmPromiseAdapter::~CdmPromiseAdapter()
{
    DCHECK(promises_.empty());
    DCHECK(thread_checker_.CalledOnValidThread());
    Clear();
}

uint32_t CdmPromiseAdapter::SavePromise(scoped_ptr<CdmPromise> promise)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    uint32_t promise_id = next_promise_id_++;
    promises_.add(promise_id, promise.Pass());
    return promise_id;
}

template <typename... T>
void CdmPromiseAdapter::ResolvePromise(uint32_t promise_id,
    const T&... result)
{
    scoped_ptr<CdmPromise> promise = TakePromise(promise_id);
    if (!promise) {
        NOTREACHED() << "Promise not found for " << promise_id;
        return;
    }

    // Sanity check the type before we do static_cast.
    CdmPromise::ResolveParameterType type = promise->GetResolveParameterType();
    CdmPromise::ResolveParameterType expected = CdmPromiseTraits<T...>::kType;
    if (type != expected) {
        NOTREACHED() << "Promise type mismatch: " << type << " vs " << expected;
        return;
    }

    static_cast<CdmPromiseTemplate<T...>*>(promise.get())->resolve(result...);
}

void CdmPromiseAdapter::RejectPromise(uint32_t promise_id,
    MediaKeys::Exception exception_code,
    uint32 system_code,
    const std::string& error_message)
{
    scoped_ptr<CdmPromise> promise = TakePromise(promise_id);
    if (!promise) {
        NOTREACHED() << "No promise found for promise_id " << promise_id;
        return;
    }

    promise->reject(exception_code, system_code, error_message);
}

void CdmPromiseAdapter::Clear()
{
    // Reject all outstanding promises.
    DCHECK(thread_checker_.CalledOnValidThread());
    for (auto& promise : promises_)
        promise.second->reject(MediaKeys::UNKNOWN_ERROR, 0, "Operation aborted.");
    promises_.clear();
}

scoped_ptr<CdmPromise> CdmPromiseAdapter::TakePromise(uint32_t promise_id)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    PromiseMap::iterator it = promises_.find(promise_id);
    if (it == promises_.end())
        return nullptr;
    return promises_.take_and_erase(it);
}

// Explicit instantiation of function templates.
template MEDIA_EXPORT void CdmPromiseAdapter::ResolvePromise(uint32_t);
template MEDIA_EXPORT void CdmPromiseAdapter::ResolvePromise(
    uint32_t,
    const std::string&);

} // namespace media
