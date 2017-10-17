// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef net_SharedMemoryDataConsumerHandle_h
#define net_SharedMemoryDataConsumerHandle_h

#include "net/RequestPeer.h"
#include "third_party/WebKit/public/platform/WebDataConsumerHandle.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"
#include "third_party/WebKit/Source/wtf/Functional.h"

namespace net {

// This class is a WebDataConsumerHandle that accepts RequestPeer::ReceivedData.
class SharedMemoryDataConsumerHandle final : public blink::WebDataConsumerHandle
{
private:
    class Context;

public:
    enum BackpressureMode {
        kApplyBackpressure,
        kDoNotApplyBackpressure,
    };

    class Writer final {
    public:
        Writer(const PassRefPtr<Context>& context, BackpressureMode mode);
        ~Writer();
        // Note: Writer assumes |AddData| is not called in a client's didGetReadable
        // callback. There isn't such assumption for |close| and |fail|.
        void addData(PassOwnPtr<RequestPeer::ReceivedData> data);
        void close();
        // TODO(yhirano): Consider providing error code.
        void fail();

    private:
        RefPtr<Context> m_context;
        BackpressureMode m_mode;

        DISALLOW_COPY_AND_ASSIGN(Writer);
    };

    class ReaderImpl final : public Reader {
    public:
        ReaderImpl(PassRefPtr<Context> context, Client* client);
        ~ReaderImpl() override;
        Result read(void* data,
            size_t size,
            Flags flags,
            size_t* readSize) override;
        Result beginRead(const void** buffer,
            Flags flags,
            size_t* available) override;
        Result endRead(size_t readSize) override;

    private:
        RefPtr<Context> m_context;

        DISALLOW_COPY_AND_ASSIGN(ReaderImpl);
    };

    // Creates a handle and a writer associated with the handle. The created
    // writer should be used on the calling thread.
    SharedMemoryDataConsumerHandle(BackpressureMode mode, Writer** writer);
    // |on_reader_detached| will be called aynchronously on the calling thread
    // when the reader (including the handle) is detached (i.e. both the handle
    // and the reader are destructed). The callback will be reset in the internal
    // context when the writer is detached, i.e. |close| or |Fail| is called,
    // and the callback will never be called.
    SharedMemoryDataConsumerHandle(BackpressureMode mode, PassOwnPtr<WTF::Closure> on_reader_detached, Writer** writer);
    ~SharedMemoryDataConsumerHandle() override;

    PassOwnPtr<Reader> otainReader(Client* client);

private:
    ReaderImpl* obtainReaderInternal(Client* client) override;
    const char* debugName() const override;

    RefPtr<Context> m_context;

    DISALLOW_COPY_AND_ASSIGN(SharedMemoryDataConsumerHandle);
};

}  // namespace net

#endif  // net_SharedMemoryDataConsumerHandle_h
