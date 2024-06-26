/*
 * Copyright (C) 2013 University of Szeged
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "net/CurlCacheEntry.h"

#include "net/FileSystem.h"
#include "net/WebURLLoaderManager.h"
#include "net/WebURLLoaderInternal.h"
#include "net/MergeVec.h"
#include "third_party/WebKit/Source/platform/network/HTTPHeaderMap.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/skia/src/utils/SkSHA1.h"
#include <wtf/CurrentTime.h>
#include <wtf/DateMath.h>
#include <wtf/HexNumber.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFStringUtil.h>

namespace net {

CurlCacheEntry::CurlCacheEntry(const String& url, WebURLLoaderInternal* job, const String& cacheDir, int Level)
    : m_headerFilename(cacheDir)
    , m_contentFilename(cacheDir)
    , m_contentTempFilename(cacheDir)
    , m_contentTempFile(invalidPlatformFileHandle)
    , m_entrySize(0)
    , m_expireDate(-1)
    , m_headerParsed(false)
    , m_job(job)
    , m_headerMem(false)
    , m_contentMem(false)
    , m_fileTime(0)
    , m_instanceSize(-1)
    , m_curPartialPos(-1)
    , m_cacheLevel(Level)
    , m_loading(false)
{
    if (m_cacheLevel == 2) {
        m_headerMem = true;
        m_contentMem = true;
        return;
    }
    generateBaseFilename(url.latin1());

    m_headerFilename.append(m_basename);
    m_headerFilename.append(".header");

    m_contentFilename.append(m_basename);
    m_contentFilename.append(".content");

    m_contentTempFilename = m_contentFilename;
    m_contentTempFilename.append(".temp");
}

CurlCacheEntry::~CurlCacheEntry()
{
    if (m_cacheLevel == 2) {
        m_loading = false;
        return;
    }
    closeContentFile(&m_contentTempFile);
}

bool CurlCacheEntry::isLoading() const
{
    if (m_cacheLevel == 2)
        return m_loading;
    return isHandleValid(m_contentTempFile);
}

// bytes 301679059-301875667/302920716
bool parseContentRange(const std::string& content_range_str, int64* first_byte_position, int64* last_byte_position, int64* instance_size)
{
    const std::string kUpThroughBytesUnit = "bytes ";
    if (content_range_str.find(kUpThroughBytesUnit) != 0)
        return false;
    std::string range_spec = content_range_str.substr(kUpThroughBytesUnit.length());
    size_t dash_offset = range_spec.find("-");
    size_t slash_offset = range_spec.find("/");

    if (dash_offset == std::string::npos || slash_offset == std::string::npos || slash_offset < dash_offset || slash_offset + 1 == range_spec.length()) {
        return false;
    }

    bool ok;
    std::string temp = range_spec.substr(0, dash_offset);
    *first_byte_position = WTF::charactersToUInt64((const LChar*)temp.c_str(), temp.size(), &ok);
    if (!ok)
        return false;

    temp = range_spec.substr(dash_offset + 1, slash_offset - dash_offset - 1);
    *last_byte_position = WTF::charactersToUInt64((const LChar*)temp.c_str(), temp.size(), &ok);
    if (!ok)
        return false;
    
    if (slash_offset == range_spec.length() - 2 && range_spec[slash_offset + 1] == '*') {
        *instance_size = -1;
    } else {
        std::string temp = range_spec.substr(slash_offset + 1);
        *instance_size = WTF::charactersToUInt64((const LChar*)temp.c_str(), temp.size(), &ok);
        if (!ok)
            return false;        
    }
    if (*last_byte_position < *first_byte_position || (*instance_size != -1 && *last_byte_position >= *instance_size))
        return false;    

    return true;
}

bool getRequestContentRange(const WebURLLoaderInternal& job, int64* firstBytePosition, int64* lastBytePosition)
{
    // "bytes=301679059-301875667"
    // "bytes 0-"
    String range = job.firstRequest()->httpHeaderField(WebString::fromUTF8("Range"));

    std::string contentRange = range.utf8().data();
    if (contentRange.size() < 7)
        return false;

    contentRange[5] = ' '; // 模仿出response时候的样子
    bool lastPosIsInfinite = contentRange[contentRange.size() - 1] == '-';
    if (lastPosIsInfinite)
        contentRange += "99999999998/99999999999";
    else
        contentRange += "/99999999998";

    int64 instanceSize = 0;
    if (!parseContentRange(contentRange, firstBytePosition, lastBytePosition, &instanceSize))
        return false;

    if (*lastBytePosition == 99999999998)
        *lastBytePosition = -1;
    return true;
}

bool CurlCacheEntry::isRequstInRange(const WebURLLoaderInternal& job) const
{
    int64 firstBytePosition;
    int64 lastBytePosition;
    if (!getRequestContentRange(job, &firstBytePosition, &lastBytePosition))
        return false;

    for (size_t i = 0; i < m_rangs.size(); ++i) {
        int64_t it = m_rangs[i];
        int begin = it >> 32;
        int end = it & 0xffffffff;
        if (begin <= firstBytePosition && firstBytePosition <= end) {
            if (lastBytePosition < 0) {
                if (m_instanceSize != -1) {
                    // 检查是否所有都填了
                    return end == m_instanceSize - 1;
                } else
                    return true;
            } else {
                return (begin <= lastBytePosition && lastBytePosition <= end);
            }
        }

    }
    return false;
}

bool CurlCacheEntry::isInRange(const WebURLLoaderInternal& job) const
{
    DebugBreak();
    String range = job.m_response.httpHeaderField(WebString::fromUTF8("Content-Range"));
    if (range.startsWith("bytes "))
        range = range.substring(sizeof("bytes ") - 1);

    if (job.m_response.httpStatusCode() != 206)
        DebugBreak();
    // bytes 301679059-301875667/302920716
    std::string contentRange = range.utf8().data();
    int64 firstBytePosition;
    int64 lastBytePosition;
    int64 instanceSize;
    if (parseContentRange(contentRange, &firstBytePosition, &lastBytePosition, &instanceSize))
        return false;

    for (size_t i = 0; i < m_rangs.size(); ++i) {
        int64_t it = m_rangs[i];
        int begin = it >> 32;
        int end = it & 0xffffffff;
        if (begin <= firstBytePosition && firstBytePosition <= end &&
            begin <= lastBytePosition && lastBytePosition <= end) {
            return true;
        }

    }
    return true;
}

// Cache manager should invalidate the entry on false
bool CurlCacheEntry::isCached()
{
    if (!m_headerMem || !m_contentMem) {
        if (!fileExists(m_contentFilename) || !fileExists(m_headerFilename))
            return false;
    }

    if (!m_headerParsed) {
        if (!loadResponseHeaders())
            return false;
    }

    if (m_expireDate < currentTimeMS()) {
        m_headerParsed = false;
        return false;
    }

    if (!entrySize())
        return false;

    long long headerFileSize = 0;
    if (!getFileSize(m_contentFilename, headerFileSize))
        return false;

//     if (m_rangs.size() != 1)
//         return false;
// 
//     int begin = m_rangs[0] >> 32;
//     int end = m_rangs[0] & 0xffffffff;
//     if (begin != 0 || end != m_instanceSize)
//         return false;

    return true;
}

int getMaxRangesSize(const std::vector<int64_t>& ranges)
{
    if (ranges.size() == 0)
        return 0;
    return ((ranges[ranges.size() - 1]) & 0xffffffff) + 1;
}

bool CurlCacheEntry::onPartialContentResponse(WebURLResponse& response)
{
    // bytes 301679059-301875667/302920716
    String range = response.httpHeaderField(WebString::fromUTF8("Content-Range"));
    if (range.isEmpty()) {
        m_curPartialPos = 0;
        return true;
    }
    std::string contentRange = range.utf8().data();
    int64 firstBytePosition;
    int64 lastBytePosition;
    int64 instanceSize;
    if (!parseContentRange(contentRange, &firstBytePosition, &lastBytePosition, &instanceSize))
        return false;

    m_instanceSize = instanceSize;
    const int offset = (int)firstBytePosition;
    int fileSize = getMaxRangesSize(m_rangs);

    if (m_curPartialPos == offset)
        return true;
    m_curPartialPos = offset;

    long long fileSize64 = -1;
    if (!getFileSize(m_contentTempFile, fileSize64))
        return false;
    
    if (fileSize64 != fileSize)
        DebugBreak();

    if (offset > fileSize) {
        if (!openContentTempFile())
            return false;

        const int kBufSize = 0x1000;
        Vector<char> buf;

        int writeOffset = fileSize;
        int remainSize = offset - fileSize; // 每次写入的剩余大小
        for (; remainSize > 0; ) {
            int writeSize = kBufSize;
            if (remainSize < kBufSize)
                writeSize = remainSize;
            remainSize -= writeSize;

            buf.resize(writeSize);
            memset(buf.data(), 0xcc, buf.size());
            
            seekFile(m_contentTempFile, (long long)0, SeekFromEnd);
            writeToFile(m_contentTempFile, buf.data(), buf.size());
            writeOffset += writeSize;
        }
    }
    
//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "onPartialContentResponse: MaxRange: %d, m_curPartialPos: %d\n", getMaxRangesSize(m_rangs), m_curPartialPos);
//     OutputDebugStringA(output);
//     free(output);

    return true;
}

static bool isSaveFinish(const std::vector<int64_t>& rangs, int filesize)
{
    if (rangs.size() != 1)
        return false;
    int begin = rangs[0] >> 32;
    int end = (rangs[0]) & 0xffffffff;
    return (begin == 0 && end == filesize - 1);
}

void CurlCacheEntry::onCachedDataFinish()
{
    closeContentFile(&m_contentTempFile);

    Vector<UChar> contentFilename = WTF::ensureUTF16UChar(m_contentFilename, true);
    Vector<UChar> contentTempFilename = WTF::ensureUTF16UChar(m_contentTempFilename, true);
    ::MoveFileEx(contentTempFilename.data(), contentFilename.data(), MOVEFILE_REPLACE_EXISTING);
}

bool CurlCacheEntry::saveCachedData(WebURLLoaderInternal& job, const char* data, size_t size)
{
    if (m_contentMem) {
        m_contentBuffer.append(data, size);
        m_contentMem = true;
    }

    if (m_cacheLevel == 2) {
        m_loading = true;
        return true;
    }

    if (!openContentTempFile())
        return false;

    onPartialContentResponse(job.m_response);
    RELEASE_ASSERT(job.m_dataLength >= size);

    int curPos = m_curPartialPos + job.m_dataLength - size;
    seekFile(m_contentTempFile, (long long)curPos, SeekFromBeginning);
    writeToFile(m_contentTempFile, data, size);

    mergeVec(&m_rangs, curPos, curPos + size - 1);

    if (isSaveFinish(m_rangs, m_instanceSize))
        onCachedDataFinish();

    return true;
}

bool CurlCacheEntry::readCachedData(WebURLLoaderInternal* job)
{
    ASSERT(job->client());
    if (m_contentMem) {
        WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, m_contentBuffer.data(), m_contentBuffer.size(), 0);
        return true;
    }

    int64 firstBytePosition;
    int64 lastBytePosition;
    if (!getRequestContentRange(*job, &firstBytePosition, &lastBytePosition)) {
        firstBytePosition = 0;
        lastBytePosition = -1;
    }

    Vector<char> buffer;
//     if (!loadFileToBuffer(m_contentFilename, buffer))
//         return false;
//     WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, buffer.data(), buffer.size(), 0);   

    PlatformFileHandle inputFile = openFile(m_contentFilename, OpenForRead);
    if (!isHandleValid(inputFile)) {
        //LOG(Network, "Cache Error: Could not open %s for read\n", filepath.latin1().data());
        return false;
    }

    long long filesize = -1;
    if (!getFileSize(m_contentFilename, filesize)) {
        //LOG(Network, "Cache Error: Could not get file size of %s\n", filepath.latin1().data());
        closeFile(inputFile);
        return false;
    }

    if (0 <= lastBytePosition && lastBytePosition <= filesize - 1)
        filesize = lastBytePosition + 1;

    int bufferPosition = firstBytePosition;
    int bufferReadSize = 4096;
    int bytesRead = 0;
    bool ok = true;

    while (filesize > bufferPosition) {
        if (filesize - bufferPosition < bufferReadSize)
            bufferReadSize = filesize - bufferPosition;

//         if (bufferPosition > 4 * 1000 * 1000) {
//             blink::WebURLError error;
//             WebURLLoaderManager::sharedInstance()->handleDidFail(job, error);
//             ok = false;
//             break;
//         }

        buffer.resize(bufferReadSize);

        seekFile(inputFile, (long long)bufferPosition, SeekFromBeginning);
        bytesRead = readFromFile(inputFile, buffer.data() /*+ bufferPosition*/, bufferReadSize);
        if (bytesRead != bufferReadSize) {
            //LOG(Network, "Cache Error: Could not read from %s\n", filepath.latin1().data());
            ok = false;
            break;
        }
        WebURLLoaderManager::sharedInstance()->didReceiveDataOrDownload(job, buffer.data(), buffer.size(), 0);

        bufferPosition += bufferReadSize;
    }
    closeFile(inputFile);

    return ok;
}

bool CurlCacheEntry::saveHeadersToFile()
{
    if (m_cacheLevel == 2)
        return true;
    PlatformFileHandle headerFile = openFile(m_headerFilename, OpenForWrite);
    if (!isHandleValid(headerFile)) {
        //LOG(Network, "Cache Error: Could not open %s for write\n", m_headerFilename.latin1().data());
        return false;
    }

    writeToFile(headerFile, m_headerBuffer.data(), m_headerBuffer.size());

    closeFile(headerFile);

    return true;
}

bool CurlCacheEntry::saveResponseHeaders(const ResourceResponse& response)
{
    if (m_headerMem) {
        m_fileTime = currentTimeMS(); // GMT
        m_headerBuffer.clear();

        // Headers
        blink::HTTPHeaderMap::const_iterator it = response.httpHeaderFields().begin();
        blink::HTTPHeaderMap::const_iterator end = response.httpHeaderFields().end();
        while (it != end) {
            String headerField = it->key;
            headerField.append(": ");
            headerField.append(it->value);
            headerField.append("\n");
            CString headerFieldLatin1 = headerField.latin1();
            m_headerBuffer.append(headerFieldLatin1.data(), headerFieldLatin1.length());
            m_cachedResponse.setHTTPHeaderField(it->key, it->value);
            ++it;
        }
    } else {
        PlatformFileHandle headerFile = openFile(m_headerFilename, OpenForWrite);
        if (!isHandleValid(headerFile)) {
            //LOG(Network, "Cache Error: Could not open %s for write\n", m_headerFilename.latin1().data());
            return false;
        }

        // Headers
        blink::HTTPHeaderMap::const_iterator it = response.httpHeaderFields().begin();
        blink::HTTPHeaderMap::const_iterator end = response.httpHeaderFields().end();
        while (it != end) {
            String headerField = it->key;
            headerField.append(": ");
            headerField.append(it->value);
            headerField.append("\n");
            CString headerFieldLatin1 = headerField.latin1();
            writeToFile(headerFile, headerFieldLatin1.data(), headerFieldLatin1.length());
            
            m_headerBuffer.append(headerFieldLatin1.data(), headerFieldLatin1.length());
            m_cachedResponse.setHTTPHeaderField(it->key, it->value);
            ++it;
        }

        closeFile(headerFile);

        m_headerMem = true;
    }
    return true;
}

bool CurlCacheEntry::loadResponseHeaders()
{
    //
    if (!m_headerMem) {
        if (!loadFileToBuffer(m_headerFilename, m_headerBuffer))
            return false;
        m_headerMem = true;
    }
    String headerContent = String(m_headerBuffer.data(), m_headerBuffer.size());
    Vector<String> headerFields;
    headerContent.split('\n', headerFields);

    Vector<String>::const_iterator it = headerFields.begin();
    Vector<String>::const_iterator end = headerFields.end();
    while (it != end) {
        size_t splitPosition = it->find(":");
        if (splitPosition != kNotFound)
            m_cachedResponse.setHTTPHeaderField(AtomicString(it->left(splitPosition)), AtomicString(it->substring(splitPosition+1).stripWhiteSpace()));
        ++it;
    }

    return parseResponseHeaders(m_cachedResponse);
}

// Set response headers from memory
void CurlCacheEntry::setResponseFromCachedHeaders(ResourceResponse& response)
{
    response.setHTTPStatusCode(304);
    response.setWasCached(true);

    // Integrate the headers in the response with the cached ones.
    blink::HTTPHeaderMap::const_iterator it = m_cachedResponse.httpHeaderFields().begin();
    blink::HTTPHeaderMap::const_iterator end = m_cachedResponse.httpHeaderFields().end();
    while (it != end) {
        if (response.httpHeaderField(it->key).isNull())
            response.setHTTPHeaderField(it->key, it->value);
        ++it;
    }

    // Try to parse expected content length
    long long contentLength = -1;
    if (!response.httpHeaderField("Content-Length").isNull()) {
        bool success = false;
        long long parsedContentLength = response.httpHeaderField("Content-Length").toInt64(&success);
        if (success)
            contentLength = parsedContentLength;
    }
    response.setExpectedContentLength(contentLength); // -1 on parse error or null

    response.setMimeType(extractMIMETypeFromMediaType(response.httpHeaderField("Content-Type")));
    response.setTextEncodingName(AtomicString(extractCharsetFromMediaType(response.httpHeaderField("Content-Type"))));
}

void CurlCacheEntry::didFail()
{
    // The cache manager will call invalidate()
    if (m_cacheLevel == 2) {
        m_loading = false;
        return;
    }
    closeContentFile(&m_contentTempFile);
}

void CurlCacheEntry::didFinishLoading()
{
    if (m_cacheLevel == 2) {
        m_loading = false;
        return;
    }
    closeContentFile(&m_contentTempFile);
}

void CurlCacheEntry::generateBaseFilename(const CString& url)
{
    SkSHA1 sha1;

    sha1.write(reinterpret_cast<const uint8_t*>(url.data()), url.length());
    SkSHA1::Digest hash;
    sha1.finish(hash);

    uint8_t* rawdata = hash.data;

    for (size_t i = 0; i < 20; i++)
        appendByteAsHex(rawdata[i], m_basename, Lowercase);
}

bool CurlCacheEntry::loadFileToBuffer(const String& filepath, Vector<char>& buffer)
{
    if (m_cacheLevel == 2)
        return true;
    // Open the file
    PlatformFileHandle inputFile = openFile(filepath, OpenForRead);
    if (!isHandleValid(inputFile)) {
        //LOG(Network, "Cache Error: Could not open %s for read\n", filepath.latin1().data());
        return false;
    }

    long long filesize = -1;
    if (!getFileSize(filepath, filesize)) {
        //LOG(Network, "Cache Error: Could not get file size of %s\n", filepath.latin1().data());
        closeFile(inputFile);
        return false;
    }

    // Load the file content into buffer
    buffer.resize(filesize);
    int bufferPosition = 0;
    int bufferReadSize = 4096;
    int bytesRead = 0;
    while (filesize > bufferPosition) {
        if (filesize - bufferPosition < bufferReadSize)
            bufferReadSize = filesize - bufferPosition;

        bytesRead = readFromFile(inputFile, buffer.data() + bufferPosition, bufferReadSize);
        if (bytesRead != bufferReadSize) {
            //LOG(Network, "Cache Error: Could not read from %s\n", filepath.latin1().data());
            closeFile(inputFile);
            return false;
        }

        bufferPosition += bufferReadSize;
    }
    closeFile(inputFile);
    return true;
}

void CurlCacheEntry::invalidate()
{
    if (m_contentMem)
        m_contentBuffer.clear();
    if (m_headerMem)
        m_contentBuffer.clear();
    if (m_cacheLevel == 2) {
        m_loading = false;
        return;
    }
    closeContentFile(&m_contentTempFile);
    deleteFile(m_headerFilename);
    deleteFile(m_contentFilename);
    deleteFile(m_contentTempFilename);
    //LOG(Network, "Cache: invalidated %s\n", m_basename.latin1().data());
}

bool CurlCacheEntry::parseResponseHeaders(const ResourceResponse& response)
{
    time_t fileModificationDate;

    if (m_fileTime == 0) {
        if (getFileModificationTime(m_headerFilename, fileModificationDate)) {
            m_fileTime = (double)(uint64_t)(fileModificationDate);
            m_fileTime *= 1000.0;
        }
        else
            m_fileTime = currentTimeMS(); // GMT
    }

    //if (response.cacheControlContainsNoCache() || response.cacheControlContainsNoStore())
    //    return false;

    double maxAge = 0;
    bool maxAgeIsValid = false;

    //if (response.cacheControlContainsMustRevalidate())
    //    maxAge = 0;
    //else {
    //    maxAge = response.cacheControlMaxAge();
    //    if (std::isnan(maxAge))
    //        maxAge = 0;
    //    else
    //        maxAgeIsValid = true;
    //}

    if (!response.hasCacheValidatorFields())
        return false;

    double lastModificationDate = 0;
    double responseDate = 0;
    double expirationDate = 0;

    lastModificationDate = response.lastModified();
    if (std::isnan(lastModificationDate))
        lastModificationDate = 0;

    responseDate = response.date();
    if (std::isnan(responseDate))
        responseDate = 0;

    expirationDate = response.expires();
    if (std::isnan(expirationDate))
        expirationDate = 0;


    if (maxAgeIsValid) {
        // When both the cache entry and the response contain max-age, the lesser one takes priority
        double expires = m_fileTime + maxAge * 1000;
        if (m_expireDate == -1 || m_expireDate > expires)
            m_expireDate = expires;
    } else if (responseDate > 0 && expirationDate >= responseDate)
        m_expireDate = m_fileTime + (expirationDate - responseDate);

    // If there is no lifetime information
    if (m_expireDate == -1) {
        if (lastModificationDate > 0)
            m_expireDate = m_fileTime + (m_fileTime - lastModificationDate) * 0.1;
        else
            m_expireDate = 0;
    }

    AtomicString etag = response.httpHeaderField("ETag");
    if (!etag.isNull())
        m_requestHeaders.set("If-None-Match", etag);

    AtomicString lastModified = response.httpHeaderField("Last-Modified");
    if (!lastModified.isNull())
        m_requestHeaders.set("If-Modified-Since", lastModified);

    if (etag.isNull() && lastModified.isNull())
        return false;

    m_headerParsed = true;
    return true;
}

bool CurlCacheEntry::loadResponseHeaders(Vector<char>& header)
{
    m_headerBuffer = header;
    return loadResponseHeaders();
}

size_t CurlCacheEntry::entrySize()
{
    if (!m_entrySize) {
        long long headerFileSize;
        long long contentFileSize;
        
        if (m_headerMem) {
            headerFileSize = m_headerBuffer.size();
        } else if (!getFileSize(m_headerFilename, headerFileSize)) {
            //LOG(Network, "Cache Error: Could not get file size of %s\n", m_headerFilename.latin1().data());
            return m_entrySize;
        }
        
        if (m_contentMem) {
            contentFileSize = m_contentBuffer.size();
        } else if (!getFileSize(m_contentFilename, contentFileSize)) {
            //LOG(Network, "Cache Error: Could not get file size of %s\n", m_contentFilename.latin1().data());
            return m_entrySize;
        }

        m_entrySize = headerFileSize + contentFileSize;
    }

    return m_entrySize;
}

bool CurlCacheEntry::openContentTempFile()
{
    if (isHandleValid(m_contentTempFile))
        return true;
    
    m_contentTempFile = openFileEx(m_contentTempFilename, OpenForWrite, kOpenAlways);
    if (isHandleValid(m_contentTempFile))
        return true;
    
    //LOG(Network, "Cache Error: Could not open %s for write\n", m_contentFilename.latin1().data());
    return false;
}

bool CurlCacheEntry::closeContentFile(PlatformFileHandle* contentTempFile)
{
    if (!isHandleValid(*contentTempFile))
        return true;

    closeFile(*contentTempFile);
    *contentTempFile = invalidPlatformFileHandle;

    return true;
}

}
