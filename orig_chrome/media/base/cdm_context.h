// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CDM_CONTEXT_H_
#define MEDIA_BASE_CDM_CONTEXT_H_

#include "base/callback.h"
#include "base/macros.h"
#include "media/base/media_export.h"

namespace media {

class Decryptor;

// An interface representing the context that a media pipeline needs from a
// content decryption module (CDM) to decrypt (and decode) encrypted buffers.
// Only used for implementing SetCdm().
class MEDIA_EXPORT CdmContext {
public:
    // Indicates an invalid CDM ID. See GetCdmId() for details.
    static const int kInvalidCdmId = 0;

    virtual ~CdmContext();

    // Gets the Decryptor object associated with the CDM. Returns nullptr if the
    // CDM does not support a Decryptor. Must not return nullptr if GetCdmId()
    // returns kInvalidCdmId. The returned object is only guaranteed to be valid
    // during the CDM's lifetime.
    virtual Decryptor* GetDecryptor() = 0;

    // Returns an ID that identifies a CDM, or kInvalidCdmId. The interpretation
    // is implementation-specific; current implementations use the ID to locate a
    // remote CDM in a different process. The return value will not be
    // kInvalidCdmId if GetDecryptor() returns nullptr.
    virtual int GetCdmId() const = 0;

protected:
    CdmContext();

private:
    DISALLOW_COPY_AND_ASSIGN(CdmContext);
};

// An interface for looking up CdmContext objects by the CDM ID.
class MEDIA_EXPORT CdmContextProvider {
public:
    virtual ~CdmContextProvider();

    // Returns the CdmContext corresponding to |cdm_id|. Returns nullptr if no
    // such CdmContext can be found.
    // Note: Calling GetCdmId() on the returned CdmContext returns kInvalidCdmId
    // (in all current cases) because the CDM will be local in the process where
    // GetCdmContext() is called.
    virtual CdmContext* GetCdmContext(int cdm_id) = 0;

protected:
    CdmContextProvider();

private:
    DISALLOW_COPY_AND_ASSIGN(CdmContextProvider);
};

// Callback to notify that the CdmContext has been completely attached to
// the media pipeline. Parameter indicates whether the operation succeeded.
typedef base::Callback<void(bool)> CdmAttachedCB;

// A dummy implementation of CdmAttachedCB.
MEDIA_EXPORT void IgnoreCdmAttached(bool success);

// Callback to notify that a CDM is ready. CdmAttachedCB is called when the CDM
// has been completely attached to the media pipeline.
typedef base::Callback<void(CdmContext*, const CdmAttachedCB&)> CdmReadyCB;

// Callback to set/cancel a CdmReadyCB.
// Calling this callback with a non-null callback registers CDM ready
// notification. When the CDM is ready, notification will be sent
// through the provided callback.
// Calling this callback with a null callback cancels previously registered CDM
// ready notification. Any previously provided callback will be fired
// immediately with NULL.
typedef base::Callback<void(const CdmReadyCB&)> SetCdmReadyCB;

} // namespace media

#endif // MEDIA_BASE_CDM_CONTEXT_H_
