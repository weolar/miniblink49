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

#include "core/fileapi/FileReaderLoader.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/ExecutionContext.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fileapi/Blob.h"
#include "core/fileapi/FileReaderLoaderClient.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/loader/ThreadableLoader.h"
#include "core/streams/Stream.h"
#include "platform/blob/BlobRegistry.h"
#include "platform/blob/BlobURL.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/Base64.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

FileReaderLoader::FileReaderLoader(ReadType readType, FileReaderLoaderClient* client)
    : m_readType(readType)
    , m_client(client)
    , m_urlForReadingIsStream(false)
    , m_isRawDataConverted(false)
    , m_stringResult("")
    , m_finishedLoading(false)
    , m_bytesLoaded(0)
    , m_totalBytes(-1)
    , m_hasRange(false)
    , m_rangeStart(0)
    , m_rangeEnd(0)
    , m_errorCode(FileError::OK)
{
}

FileReaderLoader::~FileReaderLoader()
{
    terminate();
    if (!m_urlForReading.isEmpty()) {
        if (m_urlForReadingIsStream)
            BlobRegistry::unregisterStreamURL(m_urlForReading);
        else
            BlobRegistry::revokePublicBlobURL(m_urlForReading);
    }
}

void FileReaderLoader::startInternal(ExecutionContext& executionContext, const Stream* stream, PassRefPtr<BlobDataHandle> blobData)
{
    // The blob is read by routing through the request handling layer given a temporary public url.
    m_urlForReading = BlobURL::createPublicURL(executionContext.securityOrigin());
    if (m_urlForReading.isEmpty()) {
        failed(FileError::SECURITY_ERR);
        return;
    }

    if (blobData) {
        ASSERT(!stream);
        BlobRegistry::registerPublicBlobURL(executionContext.securityOrigin(), m_urlForReading, blobData);
    } else {
        ASSERT(stream);
        BlobRegistry::registerStreamURL(executionContext.securityOrigin(), m_urlForReading, stream->url());
    }

    // Construct and load the request.
    ResourceRequest request(m_urlForReading);

    // FIXME: Should this really be 'internal'? Do we know anything about the actual request that generated this fetch?
    request.setRequestContext(WebURLRequest::RequestContextInternal);

    request.setHTTPMethod("GET");
    if (m_hasRange)
        request.setHTTPHeaderField("Range", AtomicString(String::format("bytes=%d-%d", m_rangeStart, m_rangeEnd)));

    ThreadableLoaderOptions options;
    options.preflightPolicy = ConsiderPreflight;
    options.crossOriginRequestPolicy = DenyCrossOriginRequests;
    // FIXME: Is there a directive to which this load should be subject?
    options.contentSecurityPolicyEnforcement = DoNotEnforceContentSecurityPolicy;
    // Use special initiator to hide the request from the inspector.
    options.initiator = FetchInitiatorTypeNames::internal;

    ResourceLoaderOptions resourceLoaderOptions;
    resourceLoaderOptions.allowCredentials = AllowStoredCredentials;

    if (m_client)
        m_loader = ThreadableLoader::create(executionContext, this, request, options, resourceLoaderOptions);
    else
        ThreadableLoader::loadResourceSynchronously(executionContext, request, *this, options, resourceLoaderOptions);
}

void FileReaderLoader::start(ExecutionContext* executionContext, PassRefPtr<BlobDataHandle> blobData)
{
    ASSERT(executionContext);
    m_urlForReadingIsStream = false;
    startInternal(*executionContext, 0, blobData);
}

void FileReaderLoader::start(ExecutionContext* executionContext, const Stream& stream, unsigned readSize)
{
    ASSERT(executionContext);
    if (readSize > 0) {
        m_hasRange = true;
        m_rangeStart = 0;
        m_rangeEnd = readSize - 1; // End is inclusive so (0,0) is a 1-byte read.
    }

    m_urlForReadingIsStream = true;
    startInternal(*executionContext, &stream, nullptr);
}

void FileReaderLoader::cancel()
{
    m_errorCode = FileError::ABORT_ERR;
    terminate();
}

void FileReaderLoader::terminate()
{
    if (m_loader) {
        m_loader->cancel();
        cleanup();
    }
}

void FileReaderLoader::cleanup()
{
    m_loader = nullptr;

    // If we get any error, we do not need to keep a buffer around.
    if (m_errorCode) {
        m_rawData.clear();
        m_stringResult = "";
        m_isRawDataConverted = true;
        m_decoder.clear();
    }
}

void FileReaderLoader::didReceiveResponse(unsigned long, const ResourceResponse& response, PassOwnPtr<WebDataConsumerHandle> handle)
{
    ASSERT_UNUSED(handle, !handle);
    if (response.httpStatusCode() != 200) {
        failed(httpStatusCodeToErrorCode(response.httpStatusCode()));
        return;
    }

    // A negative value means that the content length wasn't specified.
    m_totalBytes = response.expectedContentLength();

    long long initialBufferLength = -1;

    if (m_totalBytes >= 0) {
        initialBufferLength = m_totalBytes;
    } else if (m_hasRange) {
        // Set m_totalBytes and allocate a buffer based on the specified range.
        m_totalBytes = 1LL + m_rangeEnd - m_rangeStart;
        initialBufferLength = m_totalBytes;
    } else {
        // Nothing is known about the size of the resource. Normalize
        // m_totalBytes to -1 and initialize the buffer for receiving with the
        // default size.
        m_totalBytes = -1;
    }

    ASSERT(!m_rawData);

    if (m_readType != ReadByClient) {
        // Check that we can cast to unsigned since we have to do
        // so to call ArrayBuffer's create function.
        // FIXME: Support reading more than the current size limit of ArrayBuffer.
        if (initialBufferLength > std::numeric_limits<unsigned>::max()) {
            failed(FileError::NOT_READABLE_ERR);
            return;
        }

        if (initialBufferLength < 0)
            m_rawData = adoptPtr(new ArrayBufferBuilder());
        else
            m_rawData = adoptPtr(new ArrayBufferBuilder(static_cast<unsigned>(initialBufferLength)));

        if (!m_rawData || !m_rawData->isValid()) {
            failed(FileError::NOT_READABLE_ERR);
            return;
        }

        if (initialBufferLength >= 0) {
            // Total size is known. Set m_rawData to ignore overflowed data.
            m_rawData->setVariableCapacity(false);
        }
    }

    if (m_client)
        m_client->didStartLoading();
}

void FileReaderLoader::didReceiveData(const char* data, unsigned dataLength)
{
    ASSERT(data);

    // Bail out if we already encountered an error.
    if (m_errorCode)
        return;

    if (m_readType == ReadByClient) {
        m_bytesLoaded += dataLength;

        if (m_client)
            m_client->didReceiveDataForClient(data, dataLength);
        return;
    }

    unsigned bytesAppended = m_rawData->append(data, dataLength);
    if (!bytesAppended) {
        m_rawData.clear();
        m_bytesLoaded = 0;
        failed(FileError::NOT_READABLE_ERR);
        return;
    }
    m_bytesLoaded += bytesAppended;
    m_isRawDataConverted = false;

    if (m_client)
        m_client->didReceiveData();
}

void FileReaderLoader::didFinishLoading(unsigned long, double)
{
    if (m_readType != ReadByClient && m_rawData) {
        m_rawData->shrinkToFit();
        m_isRawDataConverted = false;
    }

    if (m_totalBytes == -1) {
        // Update m_totalBytes only when in dynamic buffer grow mode.
        m_totalBytes = m_bytesLoaded;
    }

    m_finishedLoading = true;

    cleanup();
    if (m_client)
        m_client->didFinishLoading();
}

void FileReaderLoader::didFail(const ResourceError&)
{
    // If we're aborting, do not proceed with normal error handling since it is covered in aborting code.
    if (m_errorCode == FileError::ABORT_ERR)
        return;

    failed(FileError::NOT_READABLE_ERR);
}

void FileReaderLoader::failed(FileError::ErrorCode errorCode)
{
    m_errorCode = errorCode;
    cleanup();
    if (m_client)
        m_client->didFail(m_errorCode);
}

FileError::ErrorCode FileReaderLoader::httpStatusCodeToErrorCode(int httpStatusCode)
{
    switch (httpStatusCode) {
    case 403:
        return FileError::SECURITY_ERR;
    case 404:
        return FileError::NOT_FOUND_ERR;
    default:
        return FileError::NOT_READABLE_ERR;
    }
}

PassRefPtr<DOMArrayBuffer> FileReaderLoader::arrayBufferResult() const
{
    ASSERT(m_readType == ReadAsArrayBuffer);

    // If the loading is not started or an error occurs, return an empty result.
    if (!m_rawData || m_errorCode)
        return nullptr;

    return DOMArrayBuffer::create(m_rawData->toArrayBuffer());
}

String FileReaderLoader::stringResult()
{
    ASSERT(m_readType != ReadAsArrayBuffer && m_readType != ReadByClient);

    // If the loading is not started or an error occurs, return an empty result.
    if (!m_rawData || m_errorCode)
        return m_stringResult;

    // If already converted from the raw data, return the result now.
    if (m_isRawDataConverted)
        return m_stringResult;

    switch (m_readType) {
    case ReadAsArrayBuffer:
        // No conversion is needed.
        break;
    case ReadAsBinaryString:
        m_stringResult = m_rawData->toString();
        m_isRawDataConverted = true;
        break;
    case ReadAsText:
        convertToText();
        break;
    case ReadAsDataURL:
        // Partial data is not supported when reading as data URL.
        if (m_finishedLoading)
            convertToDataURL();
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    return m_stringResult;
}

void FileReaderLoader::convertToText()
{
    m_isRawDataConverted = true;

    if (!m_bytesLoaded) {
        m_stringResult = "";
        return;
    }

    // Decode the data.
    // The File API spec says that we should use the supplied encoding if it is valid. However, we choose to ignore this
    // requirement in order to be consistent with how WebKit decodes the web content: always has the BOM override the
    // provided encoding.
    // FIXME: consider supporting incremental decoding to improve the perf.
    StringBuilder builder;
    if (!m_decoder)
        m_decoder = TextResourceDecoder::create("text/plain", m_encoding.isValid() ? m_encoding : UTF8Encoding());
    builder.append(m_decoder->decode(static_cast<const char*>(m_rawData->data()), m_rawData->byteLength()));

    if (m_finishedLoading)
        builder.append(m_decoder->flush());

    m_stringResult = builder.toString();
}

void FileReaderLoader::convertToDataURL()
{
    m_isRawDataConverted = true;

    StringBuilder builder;
    builder.appendLiteral("data:");

    if (!m_bytesLoaded) {
        m_stringResult = builder.toString();
        return;
    }

    builder.append(m_dataType);
    builder.appendLiteral(";base64,");

    Vector<char> out;
    base64Encode(static_cast<const char*>(m_rawData->data()), m_rawData->byteLength(), out);
    out.append('\0');
    builder.append(out.data());

    m_stringResult = builder.toString();
}

void FileReaderLoader::setEncoding(const String& encoding)
{
    if (!encoding.isEmpty())
        m_encoding = WTF::TextEncoding(encoding);
}

} // namespace blink
