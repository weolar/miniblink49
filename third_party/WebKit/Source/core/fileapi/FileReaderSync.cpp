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

#include "config.h"
#include "core/fileapi/FileReaderSync.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/ExceptionCode.h"
#include "core/fileapi/Blob.h"
#include "core/fileapi/FileError.h"
#include "core/fileapi/FileReaderLoader.h"
#include "wtf/PassRefPtr.h"

namespace blink {

FileReaderSync::FileReaderSync()
{
}

PassRefPtr<DOMArrayBuffer> FileReaderSync::readAsArrayBuffer(ExecutionContext* executionContext, Blob* blob, ExceptionState& exceptionState)
{
    if (!blob) {
        exceptionState.throwDOMException(NotFoundError, FileError::notFoundErrorMessage);
        return nullptr;
    }

    FileReaderLoader loader(FileReaderLoader::ReadAsArrayBuffer, nullptr);
    startLoading(executionContext, loader, *blob, exceptionState);

    return loader.arrayBufferResult();
}

String FileReaderSync::readAsBinaryString(ExecutionContext* executionContext, Blob* blob, ExceptionState& exceptionState)
{
    if (!blob) {
        exceptionState.throwDOMException(NotFoundError, FileError::notFoundErrorMessage);
        return String();
    }

    FileReaderLoader loader(FileReaderLoader::ReadAsBinaryString, 0);
    startLoading(executionContext, loader, *blob, exceptionState);
    return loader.stringResult();
}

String FileReaderSync::readAsText(ExecutionContext* executionContext, Blob* blob, const String& encoding, ExceptionState& exceptionState)
{
    if (!blob) {
        exceptionState.throwDOMException(NotFoundError, FileError::notFoundErrorMessage);
        return String();
    }

    FileReaderLoader loader(FileReaderLoader::ReadAsText, nullptr);
    loader.setEncoding(encoding);
    startLoading(executionContext, loader, *blob, exceptionState);
    return loader.stringResult();
}

String FileReaderSync::readAsDataURL(ExecutionContext* executionContext, Blob* blob, ExceptionState& exceptionState)
{
    if (!blob) {
        exceptionState.throwDOMException(NotFoundError, FileError::notFoundErrorMessage);
        return String();
    }

    FileReaderLoader loader(FileReaderLoader::ReadAsDataURL, nullptr);
    loader.setDataType(blob->type());
    startLoading(executionContext, loader, *blob, exceptionState);
    return loader.stringResult();
}

void FileReaderSync::startLoading(ExecutionContext* executionContext, FileReaderLoader& loader, const Blob& blob, ExceptionState& exceptionState)
{
    loader.start(executionContext, blob.blobDataHandle());
    if (loader.errorCode())
        FileError::throwDOMException(exceptionState, loader.errorCode());
}

} // namespace blink
