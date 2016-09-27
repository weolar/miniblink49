// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebDataConsumerHandle_h
#define WebDataConsumerHandle_h

#include <stddef.h>

#if INSIDE_BLINK
#include "wtf/PassOwnPtr.h"
#endif

#include "public/platform/WebCommon.h"

namespace blink {

// WebDataConsumerHandle represents the "consumer" side of a data pipe. A user
// can read data from it.
//
// A WebDataConsumerHandle is a thread-safe object. A user can call
// |obtainReader| or destruct the object on any thread.
// A WebDataConsumerHandle having a reader is called "locked". A
// WebDataConsumerHandle or its reader are called "waiting" when reading from
// the handle or reader returns ShouldWait.
//
// WebDataConsumerHandle can be created / used / destructed only on
// Oilpan-enabled threads.
// TODO(yhirano): Remove this restriction.
class BLINK_PLATFORM_EXPORT WebDataConsumerHandle {
public:
    using Flags = unsigned;
    static const Flags FlagNone = 0;

    enum Result {
        Ok,
        Done,
        Busy,
        ShouldWait,
        ResourceExhausted,
        UnexpectedError,
    };

    // Client gets notification from the pipe.
    class Client {
    public:
        virtual ~Client() { }
        // The associated handle gets readable. This function will be called
        // when the associated reader was waiting but is not waiting any more.
        // This means this function can be called when handle gets errored or
        // closed. This also means that this function will not be called even
        // when some data arrives if the handle already has non-empty readable
        // data.
        // It is not guaranteed that the handle is not waiting when this
        // function is called, i.e. it can be called more than needed.
        // One can use / destruct the associated reader in this function.
        virtual void didGetReadable() = 0;
    };

    // This class provides a means to read data from the associated handle. A
    // Reader object is bound to the thread on which |obtainReader| is called.
    // Any functions including the destructor should be called on the thread.
    // A reader can outlive the associated handle. In such a case, the handle
    // destruction will not affect the reader functionality.
    // Reading functions may success (i.e. return Ok) or fail (otherwise), and
    // the behavior which is not specified is unspecified.
    class Reader {
    public:
        // Destructing a reader means it is released and a user can get another
        // Reader by calling |obtainReader| on any thread again.
        virtual ~Reader() { }

        // Reads data into |data| up to |size| bytes. The actual read size will
        // be stored in |*readSize|. This function cannot be called when a
        // two-phase read is in progress.
        // Returns Done when it reaches to the end of the data.
        // Returns ShouldWait when the handle does not have data to read but
        // it is not closed or errored.
        virtual Result read(void* data, size_t /* size */, Flags, size_t* readSize)
        {
            BLINK_ASSERT_NOT_REACHED();
            return UnexpectedError;
        }

        // Begins a two-phase read. On success, the function stores a buffer
        // that contains the read data of length |*available| into |*buffer|.
        // Returns Done when it reaches to the end of the data.
        // Returns ShouldWait when the handle does not have data to read but
        // it is not closed or errored.
        // On fail, you don't have to (and should not) call endRead, because the
        // read session implicitly ends in that case.
        virtual Result beginRead(const void** buffer, Flags, size_t* available)
        {
            BLINK_ASSERT_NOT_REACHED();
            return UnexpectedError;
        }

        // Ends a two-phase read.
        // |readSize| indicates the actual read size.
        virtual Result endRead(size_t readSize)
        {
            BLINK_ASSERT_NOT_REACHED();
            return UnexpectedError;
        }
    };

    WebDataConsumerHandle();
    virtual ~WebDataConsumerHandle();

    // Returns a non-null reader. This function can be called only when this
    // handle is not locked. |client| can be null. Otherwise, |*client| must be
    // valid as long as the reader is valid. The returned reader is bound to
    // the calling thread and client notification will be called on the thread
    // if |client| is not null.
    // If |client| is not null and the handle is not waiting, client
    // notification is called asynchronously.
#if INSIDE_BLINK
    PassOwnPtr<Reader> obtainReader(Client*);
#endif

    // Returns a string literal (e.g. class name) for debugging only.
    virtual const char* debugName() const { return "WebDataConsumerHandle"; }

private:
    // The caller takes ownership of the returned object.
    virtual Reader* obtainReaderInternal(Client* client)
    {
        BLINK_ASSERT_NOT_REACHED();
        return nullptr;
    }
};

} // namespace blink

#endif // WebDataConsumerHandle_h
