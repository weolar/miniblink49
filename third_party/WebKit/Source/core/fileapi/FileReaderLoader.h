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

#ifndef FileReaderLoader_h
#define FileReaderLoader_h

#include "core/CoreExport.h"
#include "core/fileapi/FileError.h"
#include "core/loader/ThreadableLoaderClient.h"
#include "platform/weborigin/KURL.h"
#include "wtf/ArrayBufferBuilder.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/TextEncoding.h"
#include "wtf/text/WTFString.h"

namespace blink {

class BlobDataHandle;
class DOMArrayBuffer;
class ExecutionContext;
class FileReaderLoaderClient;
class Stream;
class TextResourceDecoder;
class ThreadableLoader;

class CORE_EXPORT FileReaderLoader final : public ThreadableLoaderClient {
    WTF_MAKE_FAST_ALLOCATED(FileReaderLoader);
public:
    enum ReadType {
        ReadAsArrayBuffer,
        ReadAsBinaryString,
        ReadAsText,
        ReadAsDataURL,
        ReadByClient
    };

    // If client is given, do the loading asynchronously. Otherwise, load synchronously.
    static PassOwnPtr<FileReaderLoader> create(ReadType readType, FileReaderLoaderClient* client)
    {
        return adoptPtr(new FileReaderLoader(readType, client));
    }

    FileReaderLoader(ReadType, FileReaderLoaderClient*);
    virtual ~FileReaderLoader();

    void start(ExecutionContext*, PassRefPtr<BlobDataHandle>);
    void start(ExecutionContext*, const Stream&, unsigned readSize);
    void cancel();

    // ThreadableLoaderClient
    virtual void didReceiveResponse(unsigned long, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    virtual void didReceiveData(const char*, unsigned) override;
    virtual void didFinishLoading(unsigned long, double) override;
    virtual void didFail(const ResourceError&) override;

    String stringResult();
    PassRefPtr<DOMArrayBuffer> arrayBufferResult() const;

    // Returns the total bytes received. Bytes ignored by m_rawData won't be
    // counted.
    //
    // This value doesn't grow more than numeric_limits<unsigned> when
    // m_readType is not set to ReadByClient.
    long long bytesLoaded() const { return m_bytesLoaded; }

    // Before didReceiveResponse() is called: Returns -1.
    // After didReceiveResponse() is called:
    // - If the size of the resource is known (from
    //   m_response.expectedContentLength() or once didFinishLoading() is
    //   called), returns it.
    // - Otherwise, returns -1.
    long long totalBytes() const { return m_totalBytes; }

    FileError::ErrorCode errorCode() const { return m_errorCode; }

    void setEncoding(const String&);
    void setDataType(const String& dataType) { m_dataType = dataType; }

private:
    void startInternal(ExecutionContext&, const Stream*, PassRefPtr<BlobDataHandle>);
    void terminate();
    void cleanup();

    void failed(FileError::ErrorCode);
    void convertToText();
    void convertToDataURL();

    static FileError::ErrorCode httpStatusCodeToErrorCode(int);

    ReadType m_readType;
    FileReaderLoaderClient* m_client;
    WTF::TextEncoding m_encoding;
    String m_dataType;

    KURL m_urlForReading;
    bool m_urlForReadingIsStream;
    RefPtr<ThreadableLoader> m_loader;

    OwnPtr<ArrayBufferBuilder> m_rawData;
    bool m_isRawDataConverted;

    String m_stringResult;

    // The decoder used to decode the text data.
    OwnPtr<TextResourceDecoder> m_decoder;

    bool m_finishedLoading;
    long long m_bytesLoaded;
    // If the total size of the resource is unknown, m_totalBytes is set to -1
    // until completion of loading, and the buffer for receiving data is set to
    // dynamically grow. Otherwise, m_totalBytes is set to the total size and
    // the buffer for receiving data of m_totalBytes is allocated and never grow
    // even when extra data is appeneded.
    long long m_totalBytes;

    bool m_hasRange;
    unsigned m_rangeStart;
    unsigned m_rangeEnd;

    FileError::ErrorCode m_errorCode;
};

} // namespace blink

#endif // FileReaderLoader_h
