/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2010, 2012, 2014 Apple Inc. All rights reserved.
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

#ifndef AsyncFileStream_h
#define AsyncFileStream_h

#include "net/StreamReader.h"
#include <wtf/Forward.h>

namespace blink {
class WebThread;
}

namespace net {

class FileStreamClient;
class FileStream;
struct AsyncTaskInfo;

class AsyncFileStream : public AsyncStreamReader {
public:
    explicit AsyncFileStream(FileStreamClient*);
    ~AsyncFileStream();

    virtual void getSize(const String& path, double expectedModificationTime) override;
    virtual void openForRead(const String& path, long long offset, long long length) override;
    void openForWrite(const String& path);
    virtual void close() override;
    virtual void read(char* buffer, int length) override;
    void write(const String& blobURL, long long position, int length);
    void truncate(long long position);

private:
    void start();

    FileStreamClient* m_client;
    FileStream* m_stream;
    blink::WebThread* m_fileThread;
    blink::WebThread* m_blinkThread;

    AsyncTaskInfo* m_asyncTaskInfo;
};

} // namespace net

#endif // AsyncFileStream_h
