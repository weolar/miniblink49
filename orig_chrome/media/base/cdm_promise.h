// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CDM_PROMISE_H_
#define MEDIA_BASE_CDM_PROMISE_H_

#include <string>

#include "base/basictypes.h"
#include "base/logging.h"
#include "media/base/media_export.h"
#include "media/base/media_keys.h"

namespace media {

// Interface for promises being resolved/rejected in response to various
// session actions. These may be called synchronously or asynchronously.
// The promise must be resolved or rejected exactly once. It is expected that
// the caller free the promise once it is resolved/rejected.

// These classes are almost generic, except for the parameters to reject(). If
// a generic class for promises is available, this could be changed to use the
// generic class as long as the parameters to reject() can be set appropriately.

// The base class only has a reject() method and GetResolveParameterType() that
// indicates the type of CdmPromiseTemplate. CdmPromiseTemplate<T> adds the
// resolve(T) method that is dependent on the type of promise. This base class
// is specified so that the promises can be easily saved before passing across
// the pepper interface.
class MEDIA_EXPORT CdmPromise {
public:
    enum ResolveParameterType {
        VOID_TYPE,
        INT_TYPE,
        STRING_TYPE,
        KEY_IDS_VECTOR_TYPE
    };

    CdmPromise();
    virtual ~CdmPromise();

    // Used to indicate that the operation failed. |exception_code| must be
    // specified. |system_code| is a Key System-specific value for the error
    // that occurred, or 0 if there is no associated status code or such status
    // codes are not supported by the Key System. |error_message| is optional.
    virtual void reject(MediaKeys::Exception exception_code,
        uint32 system_code,
        const std::string& error_message)
        = 0;

    // Used to determine the template type of CdmPromiseTemplate<T> so that
    // saved CdmPromise objects can be cast to the correct templated version.
    virtual ResolveParameterType GetResolveParameterType() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(CdmPromise);
};

// For some reason the Windows compiler is not happy with the implementation
// of CdmPromiseTemplate being in the .cc file, so moving it here.
template <typename... T>
struct CdmPromiseTraits {
};

template <>
struct CdmPromiseTraits<> {
    static const CdmPromise::ResolveParameterType kType = CdmPromise::VOID_TYPE;
};

template <>
struct CdmPromiseTraits<int> {
    static const CdmPromise::ResolveParameterType kType = CdmPromise::INT_TYPE;
};

template <>
struct CdmPromiseTraits<std::string> {
    static const CdmPromise::ResolveParameterType kType = CdmPromise::STRING_TYPE;
};

// This class adds the resolve(T) method. This class is still an interface, and
// is used as the type of promise that gets passed around.
template <typename... T>
class MEDIA_EXPORT CdmPromiseTemplate : public CdmPromise {
public:
    CdmPromiseTemplate()
        : is_settled_(false)
    {
    }

    virtual ~CdmPromiseTemplate() { DCHECK(is_settled_); }

    virtual void resolve(const T&... result) = 0;

    // CdmPromise implementation.
    virtual void reject(MediaKeys::Exception exception_code,
        uint32 system_code,
        const std::string& error_message)
        = 0;

    ResolveParameterType GetResolveParameterType() const override
    {
        return CdmPromiseTraits<T...>::kType;
    }

protected:
    // All implementations must call this method in resolve() and reject() methods
    // to indicate that the promise has been settled.
    void MarkPromiseSettled()
    {
        // Promise can only be settled once.
        DCHECK(!is_settled_);
        is_settled_ = true;
    }

private:
    // Keep track of whether the promise has been resolved or rejected yet.
    bool is_settled_;

    DISALLOW_COPY_AND_ASSIGN(CdmPromiseTemplate);
};

} // namespace media

#endif // MEDIA_BASE_CDM_PROMISE_H_
