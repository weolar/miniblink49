// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchDataConsumerHandle_h
#define FetchDataConsumerHandle_h

#include "platform/blob/BlobData.h"
#include "public/platform/WebDataConsumerHandle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"

namespace blink {

// This is an interface class that adds Reader's blobDataHandle() to
// WebDataConsumerHandle.
// This class works not very well with Oilpan: As this class is not garbage
// collected while many clients or related objects may be, it is very easy
// to create a reference cycle. When an client is garbage collected, making
// the client own the handle is the right way.
class FetchDataConsumerHandle : public WebDataConsumerHandle {
public:
    class Reader : public WebDataConsumerHandle::Reader {
    public:
        enum BlobSizePolicy {
            // The returned blob must have a valid size (i.e. != kuint64max).
            DisallowBlobWithInvalidSize,
            // The returned blob can have an invalid size.
            AllowBlobWithInvalidSize
        };

        // Drains the data as a BlobDataHandle.
        // If this function returns non-null BlobDataHandle:
        // - The bytes that will be read from the returned BlobDataHandle
        //   must be idential to the bytes that would be read through
        //   WebDataConsumerHandle::Reader APIs without calling this function.
        // - Subsequent calls to read() / beginRead() return |Done|.
        // This function can return |nullptr|, and in such cases this
        // function is no-op.
        // This function returns |nullptr| when called during two-phase read.
        virtual PassRefPtr<BlobDataHandle> drainAsBlobDataHandle(BlobSizePolicy = DisallowBlobWithInvalidSize) { return nullptr; }
    };

    // TODO(yhirano): obtainReader() is currently non-virtual override, and
    // will be changed into virtual override when we can use scoped_ptr /
    // unique_ptr in both Blink and Chromium.
    PassOwnPtr<Reader> obtainReader(Client* client) { return adoptPtr(obtainReaderInternal(client)); }

private:
    Reader* obtainReaderInternal(Client*) override = 0;
};

} // namespace blink

#endif // FetchDataConsumerHandle_h
