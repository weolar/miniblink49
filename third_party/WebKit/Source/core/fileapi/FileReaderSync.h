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

#ifndef FileReaderSync_h
#define FileReaderSync_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Blob;
class DOMArrayBuffer;
class ExceptionState;
class ExecutionContext;
class FileReaderLoader;

class FileReaderSync final : public GarbageCollected<FileReaderSync>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static FileReaderSync* create()
    {
        return new FileReaderSync();
    }

    PassRefPtr<DOMArrayBuffer> readAsArrayBuffer(ExecutionContext*, Blob*, ExceptionState&);
    String readAsBinaryString(ExecutionContext*, Blob*, ExceptionState&);
    String readAsText(ExecutionContext* executionContext, Blob* blob, ExceptionState& ec)
    {
        return readAsText(executionContext, blob, "", ec);
    }
    String readAsText(ExecutionContext*, Blob*, const String& encoding, ExceptionState&);
    String readAsDataURL(ExecutionContext*, Blob*, ExceptionState&);

    DEFINE_INLINE_TRACE() { }

private:
    FileReaderSync();

    void startLoading(ExecutionContext*, FileReaderLoader&, const Blob&, ExceptionState&);
};

} // namespace blink

#endif // FileReaderSync_h
