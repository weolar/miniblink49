/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
 * Copyright (C) 2012, 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "net/AsyncFileStream.h"

#include "net/FileStream.h"
#include "net/FileStreamClient.h"
#include "net/LambdaTask.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include <wtf/MainThread.h>
#include <wtf/text/WTFStringUtil.h>
#include <vector>
#include <functional>

namespace net {

struct AsyncTaskInfo {
    AsyncTaskInfo(FileStream* stream, blink::WebThread* blinkThread, blink::WebThread* fileThread, FileStreamClient* client) {
        this->stream = stream;
        this->client = client;
        this->blinkThread = blinkThread;
        this->fileThread = fileThread;
        destroyed = false;
    }

    ~AsyncTaskInfo()
    {
        delete stream;
    }

    FileStream* stream;
    FileStreamClient* client;
    blink::WebThread* blinkThread;
    blink::WebThread* fileThread;
    bool destroyed;
};

AsyncFileStream::AsyncFileStream(FileStreamClient* client)
    : m_client(client)
    , m_stream(new FileStream())
{
    ASSERT(isMainThread());

    content::BlinkPlatformImpl* platformImpl = (content::BlinkPlatformImpl*)blink::Platform::current();
    m_blinkThread = platformImpl->currentThread();
    m_fileThread = platformImpl->ioThread();

    m_asyncTaskInfo = new AsyncTaskInfo(m_stream, m_blinkThread, m_fileThread, m_client);
}

static void callOnFileThread(AsyncTaskInfo* info, std::function<void()>&& func)
{
    info->fileThread->postTask(FROM_HERE, new LambdaTask(std::move(func)));
}

static void callOnBlinkThread(AsyncTaskInfo* info, std::function<void()>&& func)
{
    info->blinkThread->postTask(FROM_HERE, new LambdaTask(std::move(func)));
}

AsyncFileStream::~AsyncFileStream()
{
    ASSERT(isMainThread());

    close();

    AsyncTaskInfo* asyncTaskInfo = m_asyncTaskInfo;
    // Set flag to prevent client callbacks and also prevent queued operations from starting.
    asyncTaskInfo->destroyed = true;

    // Call through file thread and back to main thread to make sure deletion happens
    // after all file thread functions and all main thread functions called from them.
    callOnFileThread(asyncTaskInfo, [asyncTaskInfo] {
        callOnBlinkThread(asyncTaskInfo, [asyncTaskInfo] {
            delete asyncTaskInfo;
        });
    });
}

static void getSizeOnFileThread(AsyncTaskInfo* info, const Vector<UChar>* path, double expectedModificationTime)
{
    if (info->destroyed) {
        delete path;
        return;
    }

    long long size = info->stream->getSize(String(path->data(), path->size()), expectedModificationTime);
    callOnBlinkThread(info, [info, size] {
        if (info->destroyed)
            return;

        info->client->didGetSize(size);
    });

    delete path;
}

void AsyncFileStream::getSize(const String& path, double expectedModificationTime)
{
//     StringCapture capturedPath(path);
//     // FIXME: Explicit return type here and in all the other cases like this below is a workaround for a deficiency
//     // in the Windows compiler at the time of this writing. Could remove it if that is resolved.
//     perform([capturedPath, expectedModificationTime](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         long long size = stream.getSize(capturedPath.string(), expectedModificationTime);
//         return [size](FileStreamClient& client) {
//             client.didGetSize(size);
//         };
//     });

    Vector<UChar>* capturedPath = new Vector<UChar>(WTF::ensureUTF16UChar(path, false));
    m_fileThread->postTask(FROM_HERE, WTF::bind(getSizeOnFileThread, m_asyncTaskInfo, capturedPath, expectedModificationTime));
}

static void openForReadOnFileThread(AsyncTaskInfo* info, const Vector<UChar>* path, long long offset, long long length)
{
    if (info->destroyed) {
        delete path;
        return;
    }

    bool success = info->stream->openForRead(String(path->data(), path->size()), offset, length);
    callOnBlinkThread(info, [info, success] {
        if (info->destroyed)
            return;

        info->client->didOpen(success);
    });

    delete path;
}

void AsyncFileStream::openForRead(const String& path, long long offset, long long length)
{
//     StringCapture capturedPath(path);
//     // FIXME: Explicit return type here is a workaround for a deficiency in the Windows compiler at the time of this writing.
//     perform([capturedPath, offset, length](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         bool success = stream.openForRead(capturedPath.string(), offset, length);
//         return [success](FileStreamClient& client) {
//             client.didOpen(success);
//         };
//     });

    Vector<UChar>* capturedPath = new Vector<UChar>(WTF::ensureUTF16UChar(path, false));
    m_fileThread->postTask(FROM_HERE, WTF::bind(openForReadOnFileThread, m_asyncTaskInfo, capturedPath, offset, length));
}

static void openForWriteOnFileThread(AsyncTaskInfo* info, const Vector<UChar>* path)
{
    if (info->destroyed) {
        delete path;
        return;
    }

    bool success = info->stream->openForWrite(String(path->data(), path->size()));
    callOnBlinkThread(info, [info, success] {
        if (info->destroyed)
            return;

        info->client->didOpen(success);
    });

    delete path;
}

void AsyncFileStream::openForWrite(const String& path)
{
//     StringCapture capturedPath(path);
//     perform([capturedPath](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         bool success = stream.openForWrite(capturedPath.string());
//         return [success](FileStreamClient& client) {
//             client.didOpen(success);
//         };
//     });

    Vector<UChar>* capturedPath = new Vector<UChar>(WTF::ensureUTF16UChar(path, false));
    m_fileThread->postTask(FROM_HERE, WTF::bind(openForWriteOnFileThread, m_asyncTaskInfo, capturedPath));
}

void AsyncFileStream::close()
{
//     auto& internals = *m_internals;
//     callOnFileThread([&internals] {
//         internals.stream.close();
//     });
    //m_fileThread->postTask(FROM_HERE, WTF::bind(&FileStream::close, m_stream));

    AsyncTaskInfo* info = m_asyncTaskInfo;
    callOnFileThread(info, [info] {
        info->stream->close();
    });
}

static void readOnFileThread(AsyncTaskInfo* info, char* buffer, int length)
{
    if (info->destroyed)
        return;

    int bytesRead = info->stream->read(buffer, length);
    callOnBlinkThread(info, [info, bytesRead] {
        if (info->destroyed)
            return;

        info->client->didRead(bytesRead);
    });
}

void AsyncFileStream::read(char* buffer, int length)
{
//     perform([buffer, length](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         int bytesRead = stream.read(buffer, length);
//         return [bytesRead](FileStreamClient& client) {
//             client.didRead(bytesRead);
//         };
//     });

    m_fileThread->postTask(FROM_HERE, WTF::bind(readOnFileThread, m_asyncTaskInfo, buffer, length));
}

static void writeOnFileThread(AsyncTaskInfo* info, Vector<UChar>* blobURL, long long position, int length) {
    if (info->destroyed) {
        delete blobURL;
        return;
    }

    int bytesWritten = info->stream->write(String(blobURL->data(), blobURL->size()), position, length);
    callOnBlinkThread(info, [info, bytesWritten] {
        if (info->destroyed)
            return;

        info->client->didWrite(bytesWritten);
    });

    delete blobURL;
}

void AsyncFileStream::write(const String& blobURL, long long position, int length)
{
//     URLCapture capturedURL(blobURL);
//     perform([capturedURL, position, length](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         int bytesWritten = stream.write(capturedURL.url(), position, length);
//         return [bytesWritten](FileStreamClient& client) {
//             client.didWrite(bytesWritten);
//         };
//     });

    Vector<UChar>* capturedBlobURL = new Vector<UChar>(WTF::ensureUTF16UChar(blobURL, false));
    m_fileThread->postTask(FROM_HERE, WTF::bind(writeOnFileThread, m_asyncTaskInfo, capturedBlobURL, position, length));
}

static void truncateOnFileThread(AsyncTaskInfo* info, long long position)
{
    if (info->destroyed)
        return;

    bool success = info->stream->truncate(position);
    callOnBlinkThread(info, [info, success] {
        if (info->destroyed)
            return;

        info->client->didTruncate(success);
    });
}

void AsyncFileStream::truncate(long long position)
{
//     perform([position](FileStream& stream) -> std::function<void(FileStreamClient&)> {
//         bool success = stream.truncate(position);
//         return [success](FileStreamClient& client) {
//             client.didTruncate(success);
//         };
//     });
    m_fileThread->postTask(FROM_HERE, WTF::bind(truncateOnFileThread, m_asyncTaskInfo, position));
}

} // namespace WebCore
