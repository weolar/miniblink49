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

#include "modules/filesystem/FileWriterSync.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/fileapi/Blob.h"
#include "public/platform/WebFileWriter.h"
#include "public/platform/WebURL.h"

namespace blink {

void FileWriterSync::write(Blob* data, ExceptionState& exceptionState)
{
    ASSERT(writer());
    ASSERT(m_complete);
    if (!data) {
        exceptionState.throwDOMException(TypeMismatchError, FileError::typeMismatchErrorMessage);
        return;
    }

    prepareForWrite();
    writer()->write(position(), data->uuid());
    ASSERT(m_complete);
    if (m_error) {
        FileError::throwDOMException(exceptionState, m_error);
        return;
    }
    setPosition(position() + data->size());
    if (position() > length())
        setLength(position());
}

void FileWriterSync::seek(long long position, ExceptionState& exceptionState)
{
    ASSERT(writer());
    ASSERT(m_complete);
    seekInternal(position);
}

void FileWriterSync::truncate(long long offset, ExceptionState& exceptionState)
{
    ASSERT(writer());
    ASSERT(m_complete);
    if (offset < 0) {
        exceptionState.throwDOMException(InvalidStateError, FileError::invalidStateErrorMessage);
        return;
    }
    prepareForWrite();
    writer()->truncate(offset);
    ASSERT(m_complete);
    if (m_error) {
        FileError::throwDOMException(exceptionState, m_error);
        return;
    }
    if (offset < position())
        setPosition(offset);
    setLength(offset);
}

void FileWriterSync::didWrite(long long bytes, bool complete)
{
    ASSERT(m_error == FileError::OK);
    ASSERT(!m_complete);
#if ENABLE(ASSERT)
    m_complete = complete;
#else
    ASSERT_UNUSED(complete, complete);
#endif
}

void FileWriterSync::didTruncate()
{
    ASSERT(m_error == FileError::OK);
    ASSERT(!m_complete);
#if ENABLE(ASSERT)
    m_complete = true;
#endif
}

void FileWriterSync::didFail(WebFileError error)
{
    ASSERT(m_error == FileError::OK);
    m_error = static_cast<FileError::ErrorCode>(error);
    ASSERT(!m_complete);
#if ENABLE(ASSERT)
    m_complete = true;
#endif
}

FileWriterSync::FileWriterSync()
    : m_error(FileError::OK)
#if ENABLE(ASSERT)
    , m_complete(true)
#endif
{
}

void FileWriterSync::prepareForWrite()
{
    ASSERT(m_complete);
    m_error = FileError::OK;
#if ENABLE(ASSERT)
    m_complete = false;
#endif
}

FileWriterSync::~FileWriterSync()
{
}

DEFINE_TRACE(FileWriterSync)
{
    FileWriterBase::trace(visitor);
}

} // namespace blink
