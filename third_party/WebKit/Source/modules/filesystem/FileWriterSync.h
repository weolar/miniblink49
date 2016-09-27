/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
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

#ifndef FileWriterSync_h
#define FileWriterSync_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/fileapi/FileError.h"
#include "modules/filesystem/FileWriterBase.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebFileWriterClient.h"

namespace blink {

class Blob;
class ExceptionState;

class FileWriterSync final
#if ENABLE(OILPAN)
    : public GarbageCollectedFinalized<FileWriterSync>
    , public FileWriterBase
#else
    : public FileWriterBase
#endif
    , public ScriptWrappable
    , public WebFileWriterClient {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(FileWriterSync);
public:
    static FileWriterSync* create()
    {
        return new FileWriterSync();
    }
    ~FileWriterSync() override;
    DECLARE_VIRTUAL_TRACE();

    // FileWriterBase
    void write(Blob*, ExceptionState&);
    void seek(long long position, ExceptionState&);
    void truncate(long long length, ExceptionState&);

    // WebFileWriterClient, via FileWriterBase
    void didWrite(long long bytes, bool complete) override;
    void didTruncate() override;
    void didFail(WebFileError) override;

private:
    FileWriterSync();
    void prepareForWrite();

    FileError::ErrorCode m_error;
#if ENABLE(ASSERT)
    bool m_complete;
#endif
};

} // namespace blink

#endif // FileWriterSync_h
