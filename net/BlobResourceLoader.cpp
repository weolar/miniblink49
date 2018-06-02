/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#include "net/BlobResourceLoader.h"
#include "net/WebURLLoaderManager.h"
#include "net/AsyncFileStream.h"
#include "net/FileStream.h"
#include "net/MemBlobStream.h"
#include "content/web_impl_win/WebBlobRegistryImpl.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include <wtf/MainThread.h>

namespace net {

static const unsigned bufferSize = 512 * 1024;
static const long long positionNotSpecified = -1;

static const int httpOK = 200;
static const int httpPartialContent = 206;
static const int httpNotAllowed = 403;
static const int httpNotFound = 404;
static const int httpRequestedRangeNotSatisfiable = 416;
static const int httpInternalError = 500;
static const char* httpOKText = "OK";
static const char* httpPartialContentText = "Partial Content";
static const char* httpNotAllowedText = "Not Allowed";
static const char* httpNotFoundText = "Not Found";
static const char* httpRequestedRangeNotSatisfiableText = "Requested Range Not Satisfiable";
static const char* httpInternalErrorText = "Internal Server Error";

static const char* const webKitBlobResourceDomain = "WebKitBlobResource";
enum {
    notFoundError = 1,
    securityError = 2,
    rangeError = 3,
    notReadableError = 4,
    methodNotAllowed = 5
};

BlobDataWrap::BlobDataWrap()
{

}

BlobDataWrap::~BlobDataWrap()
{

}

///////////////////////////////////////////////////////////////////////////////
// BlobResourceSynchronousLoader

namespace {

class BlobResourceSynchronousLoader : public blink::WebURLLoaderClient {
public:
    BlobResourceSynchronousLoader(blink::WebURLError&, blink::WebURLResponse&, Vector<char>&);

    virtual void didReceiveResponse(blink::WebURLLoader*, const blink::WebURLResponse&) override;
    virtual void didReceiveData(blink::WebURLLoader*, const char* data, int dataLength, int encodedDataLength) override;
    virtual void didFinishLoading(blink::WebURLLoader*, double finishTime, int64_t totalEncodedDataLength) override;
    virtual void didFail(blink::WebURLLoader*, const blink::WebURLError&) override;

private:
    blink::WebURLError m_error;
    blink::WebURLResponse m_response;
    Vector<char>& m_data;
};

BlobResourceSynchronousLoader::BlobResourceSynchronousLoader(blink::WebURLError& error, blink::WebURLResponse& response, Vector<char>& data)
    : m_error(error)
    , m_response(response)
    , m_data(data)
{
}

void BlobResourceSynchronousLoader::didReceiveResponse(blink::WebURLLoader* handle, const blink::WebURLResponse& response)
{
    // We cannot handle the size that is more than maximum integer.
    if (response.expectedContentLength() > INT_MAX) {
        m_error.domain = blink::WebString::fromUTF8(webKitBlobResourceDomain);
        m_error.reason = notReadableError;
        m_error.unreachableURL = response.url();
        m_error.localizedDescription = blink::WebString::fromUTF8("File is too large");
        return;
    }

    m_response = response;

    // Read all the data.
    m_data.resize(static_cast<size_t>(response.expectedContentLength()));
    //static_cast<BlobResourceLoader*>(handle)->readSync(m_data.data(), static_cast<int>(m_data.size()));
    // TODO
}

void BlobResourceSynchronousLoader::didReceiveData(blink::WebURLLoader*, const char* data, int dataLength, int)
{
    m_data.append(data, dataLength);
}

void BlobResourceSynchronousLoader::didFinishLoading(blink::WebURLLoader*, double, int64_t)
{
}

void BlobResourceSynchronousLoader::didFail(blink::WebURLLoader*, const blink::WebURLError& error)
{
    m_error = error;
}

}

///////////////////////////////////////////////////////////////////////////////
// BlobResourceLoader

BlobResourceLoader* BlobResourceLoader::createAsync(BlobDataWrap* blobData, const blink::WebURLRequest& request, blink::WebURLLoaderClient* client, blink::WebURLLoader* loader)
{
    // FIXME: Should probably call didFail() instead of blocking the load without explanation.
    if (!equalIgnoringCase((String)request.httpMethod(), "GET"))
        return nullptr;

    return /*adoptRef*/(new BlobResourceLoader(blobData, request, client, loader, true));
}

void BlobResourceLoader::loadResourceSynchronously(BlobDataWrap* blobData, const blink::WebURLRequest& request, blink::WebURLLoader* loader,
    blink::WebURLError& error, blink::WebURLResponse& response, Vector<char>& data)
{
    if (!equalIgnoringCase((String)request.httpMethod(), "GET")) {
        error.domain = blink::WebString::fromUTF8(webKitBlobResourceDomain);
        error.reason = methodNotAllowed;
        error.unreachableURL = response.url();
        error.localizedDescription = blink::WebString::fromUTF8("Request method must be GET");
        return;
    }

    BlobResourceSynchronousLoader client(error, response, data);
    BlobResourceLoader* blobLoader = new BlobResourceLoader(blobData, request, &client, loader, false);
    blobLoader->start();
}

class StreamWrap {
public:
    static const long long kIsAsynSize = -2;

    StreamWrap(FileStreamClient* client, bool isAsyn)
    {
        m_blob = nullptr;
        m_fileAsyn = nullptr;
        m_fileSyn = nullptr;
        m_isBlob = false;
        m_isAsyn = isAsyn;
        m_client = client;
    }

    ~StreamWrap()
    {
        m_blob = nullptr;
        m_fileAsyn = nullptr;
        m_fileSyn = nullptr;
    }

    bool initCheck(const String& path)
    {
        bool isBlob = path.startsWith("file:///c:/miniblink_blob_download_");
        if (isBlob) {
            if (!m_blob)
                m_blob.reset(new MemBlobStream(m_client, true));
        } else {
            if (m_isAsyn && !m_fileAsyn)
                m_fileAsyn.reset(new AsyncFileStream(m_client));
            if (!m_isAsyn && !m_fileSyn)
                m_fileSyn.reset(new FileStream());
        }
        return isBlob;
    }

    long long getSize(const String& path, double expectedModificationTime)
    {
        bool isBlob = initCheck(path);

        if (isBlob)
            return m_blob->getSize(path, expectedModificationTime);
        else if (!m_isAsyn)
            return m_fileSyn->getSize(path, expectedModificationTime);
        else if (m_isAsyn)
            m_fileAsyn->getSize(path, expectedModificationTime);

        return kIsAsynSize;
    }

    bool openForRead(const String& path, long long offset, long long length)
    {
        m_isBlob = initCheck(path);
        if (m_isBlob)
            return m_blob->openForRead(path, offset, length);
        else if (!m_isAsyn)
            return m_fileSyn->openForRead(path, offset, length);
        else if (m_isAsyn)
            m_fileAsyn->openForRead(path, offset, length);
        return true;
    }

    void close()
    {
        if (m_isBlob && m_blob)
            m_blob->close();
        else if (!m_isAsyn && m_fileSyn)
            m_fileSyn->close();
        else if (m_isAsyn && m_fileAsyn)
            m_fileAsyn->close();
    }

    int read(char* buffer, int length)
    {
        if (m_isBlob)
            return m_blob->read(buffer, length);
        else if (!m_isAsyn)
            return m_fileSyn->read(buffer, length);
        else if (m_isAsyn)
            m_fileAsyn->read(buffer, length);

        return 0;
    }

private:
    std::unique_ptr<MemBlobStream> m_blob;
    std::unique_ptr<AsyncFileStream> m_fileAsyn;
    std::unique_ptr<FileStream> m_fileSyn;
    bool m_isBlob;
    bool m_isAsyn;
    FileStreamClient* m_client;
};

class LoaderWrap {
public:
    LoaderWrap(BlobResourceLoader* client, BlobResourceLoader::RunType runType)
    {
        m_client = client;
        m_runType = runType;
    }

    static void doStartInMainThread(void* param)
    {
        LoaderWrap* self = (LoaderWrap*)param;

        if (self->m_client) {
            self->m_client->asynTaskFinish(self->m_runType);
            self->m_client->doStart();
        }

        self->m_client = nullptr;
        delete self;
    }

    static void doNotifyFinishOnMainThread(void* param)
    {
        LoaderWrap* self = (LoaderWrap*)param;

        if (self->m_client) {
            self->m_client->asynTaskFinish(self->m_runType);
            self->m_client->doNotifyFinish();
        }

        self->m_client = nullptr;
        delete self;
    }

    void cancel() { m_client = nullptr; }

private:
    BlobResourceLoader* m_client;
    BlobResourceLoader::RunType m_runType;
};

BlobResourceLoader::BlobResourceLoader(BlobDataWrap* blobData, const blink::WebURLRequest& request, blink::WebURLLoaderClient* client, blink::WebURLLoader* loader, bool async)
    : m_request(request)
    , m_client(client)
    , m_loader(loader)
    , m_blobData(blobData)
    , m_async(async)
    , m_errorCode(0)
    , m_aborted(false)
    , m_rangeOffset(positionNotSpecified)
    , m_rangeEnd(positionNotSpecified)
    , m_rangeSuffixLength(positionNotSpecified)
    , m_totalRemainingSize(0)
    , m_currentItemReadSize(0)
    , m_sizeItemCount(0)
    , m_readItemCount(0)
    , m_fileOpened(false)
    , m_doNotifyFinishAsynTaskWeakPtr(nullptr)
    , m_doStartAsynTaskWeakPtr(nullptr)
    , m_isDestroied(nullptr)
{
    m_streamWrap = std::unique_ptr<StreamWrap>(new StreamWrap(this, m_async));
}

BlobResourceLoader::~BlobResourceLoader()
{
    cancel();
}

void BlobResourceLoader::cancel()
{
    m_streamWrap = nullptr;
    m_aborted = true;

    if (m_doStartAsynTaskWeakPtr)
        m_doStartAsynTaskWeakPtr->cancel();
    m_doStartAsynTaskWeakPtr = nullptr;

    if (m_doNotifyFinishAsynTaskWeakPtr)
        m_doNotifyFinishAsynTaskWeakPtr->cancel();
    m_doNotifyFinishAsynTaskWeakPtr = nullptr;

    if (m_isDestroied)
        *m_isDestroied = true;

    //ResourceHandle::cancel();
}

void BlobResourceLoader::continueDidReceiveResponse()
{
    // BlobResourceLoader doesn't wait for didReceiveResponse, and it currently cannot be used for downloading.
}

void BlobResourceLoader::asynTaskFinish(RunType runType)
{
    switch (runType) {
    case BlobResourceLoader::kStart:
        m_doStartAsynTaskWeakPtr = nullptr;
        break;
    case BlobResourceLoader::kNotifyFinish:
        m_doNotifyFinishAsynTaskWeakPtr = nullptr;
        break;
    }
}

void BlobResourceLoader::start()
{
    if (!m_async) {
        doStart();
        return;
    }

    //RefPtr<BlobResourceLoader> handle(this);

    // Finish this async call quickly and return.
    if (!m_doStartAsynTaskWeakPtr) {
        m_doStartAsynTaskWeakPtr = new LoaderWrap(this, BlobResourceLoader::kStart);
        WTF::internal::callOnMainThread(LoaderWrap::doStartInMainThread, m_doStartAsynTaskWeakPtr);
    }
}

void BlobResourceLoader::doStart()
{
    ASSERT(isMainThread());

    // Do not continue if the request is aborted or an error occurs.
    if (m_aborted || m_errorCode)
        return;

    // If the blob data is not found, fail now.
    if (!m_blobData) {
        m_errorCode = notFoundError;
        notifyResponse();
        return;
    }

    // Parse the "Range" header we care about.
    String range = m_request.httpHeaderField("Range");
    if (!range.isEmpty() && !blink::parseRange(range, m_rangeOffset, m_rangeEnd, m_rangeSuffixLength)) {
        m_errorCode = rangeError;
        notifyResponse();
        return;
    }

    if (m_async)
        getSizeForNext();
    else {
        //Ref<BlobResourceLoader> protect(*this); // getSizeForNext calls the client
        bool isDestroied = false;
        m_isDestroied = &isDestroied;

        for (size_t i = 0; i < m_blobData->items().size() && !m_aborted && !m_errorCode; ++i) {
            getSizeForNext();
            if (isDestroied)
                return;
        }

        m_isDestroied = nullptr;
        notifyResponse();
    }
}

static String getPathBySystemURL(const blink::WebURL& fileSystemURL)
{
    blink::KURL url = fileSystemURL;
    String urlString = url.getUTF8String();
    if (urlString.startsWith("file:///"))
        urlString.remove(0, 8);
    return urlString;
}

void BlobResourceLoader::getSizeForNext()
{
    ASSERT(isMainThread());

    // Do we finish validating and counting size for all items?
    if (m_sizeItemCount >= m_blobData->items().size()) {
        seek();

        // Start reading if in asynchronous mode.
        if (m_async) {
            //Ref<BlobResourceLoader> protect(*this);
            bool isDestroied = false;
            m_isDestroied = &isDestroied;
            notifyResponse();
            if (isDestroied)
                return;
            m_isDestroied = nullptr;
            m_buffer.resize(bufferSize);
            readAsync();
        }
        return;
    }

    long long size = 0;
    String filePath;
    blink::WebBlobData::Item* item = m_blobData->items().at(m_sizeItemCount);
    switch (item->type) {
    case blink::WebBlobData::Item::TypeData:
        didGetSize(item->data.size());
        break;
    case blink::WebBlobData::Item::TypeFileSystemURL:
        item->filePath = getPathBySystemURL(item->fileSystemURL); // no break
    case blink::WebBlobData::Item::TypeFile:
        // Files know their sizes, but asking the stream to verify that the file wasn't modified.
        size = m_streamWrap->getSize(item->filePath, item->expectedModificationTime);
        if (StreamWrap::kIsAsynSize != size) // 有可能是miniblink_blob_download类型的blob文件名
            didGetSize(size);
        break;
    case blink::WebBlobData::Item::TypeBlob: {
        content::WebBlobRegistryImpl* blobReg = (content::WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
        net::BlobDataWrap* blobData = blobReg->getBlobDataFromUUID(item->blobUUID);
        
        const Vector<blink::WebBlobData::Item*>& items = blobData->items();
        for (size_t i = 0; i < items.size(); ++i) {
            blink::WebBlobData::Item* it = items[i];
            switch (it->type) {
            case blink::WebBlobData::Item::TypeData:
                didGetSize(it->data.size());
                break;
            case blink::WebBlobData::Item::TypeFileSystemURL:
                filePath = getPathBySystemURL(item->fileSystemURL); // no break
                break;
            case blink::WebBlobData::Item::TypeFile:
                size = m_streamWrap->getSize(item->filePath, item->expectedModificationTime);
                if (StreamWrap::kIsAsynSize != size)
                    didGetSize(size);
                break;
            case blink::WebBlobData::Item::TypeBlob:
                DebugBreak();
                break;
            }
        }

        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
}

// WebBlobData::Item::length的长度表示需要取多长，如果是-1，表示全部取。并且
// WebBlobRegistryImpl::setBlobDataLengthByTempPath里会对blob型设置个真实长度
void BlobResourceLoader::didGetSize(long long size)
{
    ASSERT(isMainThread());

    // Do not continue if the request is aborted or an error occurs.
    if (m_aborted || m_errorCode)
        return;

    // If the size is -1, it means the file has been moved or changed. Fail now.
    if (size == -1) {
        OutputDebugStringA("BlobResourceLoader::didGetSize fail\n");

        m_errorCode = notFoundError;
        notifyResponse();
        return;
    }

    // The size passed back is the size of the whole file. If the underlying item is a sliced file, we need to use the slice length.
    const blink::WebBlobData::Item* item = m_blobData->items().at(m_sizeItemCount);
    RELEASE_ASSERT(item->length != 0);

    if (-1 != item->length && item->length < size)
        size = item->length;

    // Cache the size.
    m_itemLengthList.append(size);

    // Count the size.
    m_totalRemainingSize += size;
    m_sizeItemCount++;

    // Continue with the next item.
    getSizeForNext();
}

void BlobResourceLoader::seek()
{
    ASSERT(isMainThread());

    // Convert from the suffix length to the range.
    if (m_rangeSuffixLength != positionNotSpecified) {
        m_rangeOffset = m_totalRemainingSize - m_rangeSuffixLength;
        m_rangeEnd = m_rangeOffset + m_rangeSuffixLength - 1;
    }

    // Bail out if the range is not provided.
    if (m_rangeOffset == positionNotSpecified)
        return;

    // Skip the initial items that are not in the range.
    long long offset = m_rangeOffset;
    for (m_readItemCount = 0; m_readItemCount < m_blobData->items().size() && offset >= m_itemLengthList[m_readItemCount]; ++m_readItemCount)
        offset -= m_itemLengthList[m_readItemCount];

    // Set the offset that need to jump to for the first item in the range.
    m_currentItemReadSize = offset;

    // Adjust the total remaining size in order not to go beyond the range.
    if (m_rangeEnd != positionNotSpecified) {
        long long rangeSize = m_rangeEnd - m_rangeOffset + 1;
        if (m_totalRemainingSize > rangeSize)
            m_totalRemainingSize = rangeSize;
    } else
        m_totalRemainingSize -= m_rangeOffset;
}

int BlobResourceLoader::readSync(char* buf, int length)
{
    ASSERT(isMainThread());

    ASSERT(!m_async);
    //Ref<BlobResourceLoader> protect(*this);

    int offset = 0;
    int remaining = length;
    while (remaining) {
        // Do not continue if the request is aborted or an error occurs.
        if (m_aborted || m_errorCode)
            break;

        // If there is no more remaining data to read, we are done.
        if (!m_totalRemainingSize || m_readItemCount >= m_blobData->items().size())
            break;
        
        const blink::WebBlobData::Item* item = m_blobData->items().at(m_readItemCount);
        int bytesRead = 0;
        if (item->type == blink::WebBlobData::Item::TypeData)
            bytesRead = readDataSync(*item, buf + offset, remaining);
        else if (item->type == blink::WebBlobData::Item::TypeFile || item->type == blink::WebBlobData::Item::TypeFileSystemURL)
            bytesRead = readFileSync(*item, buf + offset, remaining);
        else
            ASSERT_NOT_REACHED();

        if (bytesRead > 0) {
            offset += bytesRead;
            remaining -= bytesRead;
        }
    }

    int result;
    if (m_aborted || m_errorCode)
        result = -1;
    else
        result = length - remaining;

    bool isDestroied = false;
    m_isDestroied = &isDestroied;

    if (result > 0)
        notifyReceiveData(buf, result);

    m_isDestroied = nullptr;
    if (!result && !isDestroied)
        notifyFinish();

    return result;
}

int BlobResourceLoader::readDataSync(const blink::WebBlobData::Item& item, char* buf, int length)
{
    ASSERT(isMainThread());

    ASSERT(!m_async);

    int itemLength = item.length;
    if (blink::WebBlobData::Item::TypeData == item.type)
        itemLength = item.data.size();

    long long remaining = itemLength - m_currentItemReadSize;
    int bytesToRead = (length > remaining) ? static_cast<int>(remaining) : length;
    if (bytesToRead > m_totalRemainingSize)
        bytesToRead = static_cast<int>(m_totalRemainingSize);
    memcpy(buf, item.data.data() + item.offset + m_currentItemReadSize, bytesToRead);
    m_totalRemainingSize -= bytesToRead;

    m_currentItemReadSize += bytesToRead;
    if (m_currentItemReadSize == itemLength) {
        m_readItemCount++;
        m_currentItemReadSize = 0;
    }

    return bytesToRead;
}

int BlobResourceLoader::readFileSync(const blink::WebBlobData::Item& item, char* buf, int length)
{
    ASSERT(isMainThread());

    ASSERT(!m_async);

    if (!m_fileOpened) {
        long long bytesToRead = m_itemLengthList[m_readItemCount] - m_currentItemReadSize;
        if (bytesToRead > m_totalRemainingSize)
            bytesToRead = m_totalRemainingSize;
        bool success = m_streamWrap->openForRead(item.filePath, item.offset + m_currentItemReadSize, bytesToRead);
        m_currentItemReadSize = 0;
        if (!success) {
            m_errorCode = notReadableError;
            return 0;
        }

        m_fileOpened = true;
    }

    int bytesRead = m_streamWrap->read(buf, length);
    if (bytesRead < 0) {
        m_errorCode = notReadableError;
        return 0;
    }
    if (!bytesRead) {
        m_streamWrap->close();
        m_fileOpened = false;
        m_readItemCount++;
    } else
        m_totalRemainingSize -= bytesRead;
    
    return bytesRead;

    DebugBreak();
    return 0;
}

void BlobResourceLoader::readAsync()
{
    ASSERT(isMainThread());
    ASSERT(m_async);

    // Do not continue if the request is aborted or an error occurs.
    if (m_aborted || m_errorCode)
        return;

    // If there is no more remaining data to read, we are done.
    if (!m_totalRemainingSize || m_readItemCount >= m_blobData->items().size()) {
        notifyFinish();
        return;
    }

    const blink::WebBlobData::Item* item = m_blobData->items().at(m_readItemCount);
    if (item->type == blink::WebBlobData::Item::TypeData)
        readDataAsync(*item);
    else if (item->type == blink::WebBlobData::Item::TypeFile || item->type == blink::WebBlobData::Item::TypeFileSystemURL)
        readFileAsync(*item);
    else
        ASSERT_NOT_REACHED();
}

void BlobResourceLoader::readDataAsync(const blink::WebBlobData::Item& item)
{
    ASSERT(isMainThread());
    ASSERT(m_async);
    //Ref<BlobResourceLoader> protect(*this);

    int itemLength = item.length;
    if (blink::WebBlobData::Item::TypeData == item.type)
        itemLength = item.data.size();

    long long bytesToRead = itemLength - m_currentItemReadSize;
    if (bytesToRead > m_totalRemainingSize)
        bytesToRead = m_totalRemainingSize;
    consumeData(item.data.data() + item.offset + m_currentItemReadSize, static_cast<int>(bytesToRead));
    m_currentItemReadSize = 0;
}

void BlobResourceLoader::readFileAsync(const blink::WebBlobData::Item& item)
{
    ASSERT(isMainThread());
    ASSERT(m_async);

    if (m_fileOpened) {
        m_streamWrap->read(m_buffer.data(), m_buffer.size());
        return;
    }

    long long bytesToRead = m_itemLengthList[m_readItemCount] - m_currentItemReadSize;
    if (bytesToRead > m_totalRemainingSize)
        bytesToRead = static_cast<int>(m_totalRemainingSize);
    m_streamWrap->openForRead(item.filePath, item.offset + m_currentItemReadSize, bytesToRead);
    m_fileOpened = true;
    m_currentItemReadSize = 0;
}

void BlobResourceLoader::didOpen(bool success)
{
    ASSERT(m_async);

    if (!success) {
        failed(notReadableError);
        return;
    }

    // Continue the reading.
    readAsync();
}

void BlobResourceLoader::didRead(int bytesRead)
{
    if (bytesRead < 0) {
        failed(notReadableError);
        return;
    }

    consumeData(m_buffer.data(), bytesRead);
}

void BlobResourceLoader::consumeData(const char* data, int bytesRead)
{
    ASSERT(m_async);
    //Ref<BlobResourceLoader> protect(*this);

    m_totalRemainingSize -= bytesRead;

    // Notify the client.
    if (bytesRead)
        notifyReceiveData(data, bytesRead);

    if (m_fileOpened) {
        // When the current item is a file item, the reading is completed only if bytesRead is 0.
        if (!bytesRead) {
            // Close the file.
            m_fileOpened = false;
            m_streamWrap->close();

            // Move to the next item.
            m_readItemCount++;
        }
    } else {
        // Otherwise, we read the current text item as a whole and move to the next item.
        m_readItemCount++;
    }

    // Continue the reading.
    readAsync();
}

void BlobResourceLoader::failed(int errorCode)
{
    ASSERT(m_async);
    //Ref<BlobResourceLoader> protect(*this);

    // Notify the client.
    bool isDestroied = false;
    m_isDestroied = &isDestroied;
    notifyFail(errorCode);
    if (m_isDestroied)
        return;
    m_isDestroied = nullptr;

    // Close the file if needed.
    if (m_fileOpened) {
        m_fileOpened = false;
        m_streamWrap->close();
    }
}

void BlobResourceLoader::notifyResponse()
{
    if (!m_client)
        return;

    if (m_errorCode) {
        //Ref<BlobResourceLoader> protect(*this);
        bool isDestroied = false;
        m_isDestroied = &isDestroied;
        notifyResponseOnError();

        if (!isDestroied) {
            m_isDestroied = nullptr;
            notifyFinish();
        }
    } else
        notifyResponseOnSuccess();
}

void BlobResourceLoader::notifyResponseOnSuccess()
{
    ASSERT(isMainThread());

    bool isRangeRequest = m_rangeOffset != positionNotSpecified;
    blink::WebURLResponse response;
    response.initialize();
    response.setURL(m_request.url());
    response.setMIMEType(m_blobData->m_contentType);
    response.setExpectedContentLength(m_totalRemainingSize);
    response.setHTTPStatusCode(isRangeRequest ? httpPartialContent : httpOK);
    response.setHTTPStatusText(blink::WebString::fromUTF8(isRangeRequest ? httpPartialContentText : httpOKText));
    // FIXME: If a resource identified with a blob: URL is a File object, user agents must use that file's name attribute,
    // as if the response had a Content-Disposition header with the filename parameter set to the File's name attribute.
    // Notably, this will affect a name suggested in "File Save As".

    // BlobResourceLoader cannot be used with downloading, and doesn't even wait for continueDidReceiveResponse.
    // It's currently client's responsibility to know that didReceiveResponseAsync cannot be used to convert a
    // load into a download or blobs.
//     if (usesAsyncCallbacks())
//         m_client->didReceiveResponseAsync(this, response);
//     else
        m_client->didReceiveResponse(m_loader, response);
}

void BlobResourceLoader::notifyResponseOnError()
{
    ASSERT(m_errorCode);

    blink::WebURLResponse response;
    response.initialize();
    response.setURL(m_request.url());
    response.setMIMEType(blink::WebString::fromUTF8("text/plain"));

    switch (m_errorCode) {
    case rangeError:
        response.setHTTPStatusCode(httpRequestedRangeNotSatisfiable);
        response.setHTTPStatusText(blink::WebString::fromUTF8(httpRequestedRangeNotSatisfiableText));
        break;
    case notFoundError:
        response.setHTTPStatusCode(httpNotFound);
        response.setHTTPStatusText(blink::WebString::fromUTF8(httpNotFoundText));
        break;
    case securityError:
        response.setHTTPStatusCode(httpNotAllowed);
        response.setHTTPStatusText(blink::WebString::fromUTF8(httpNotAllowedText));
        break;
    default:
        response.setHTTPStatusCode(httpInternalError);
        response.setHTTPStatusText(blink::WebString::fromUTF8(httpInternalErrorText));
        break;
    }

    // Note that we don't wait for continueDidReceiveResponse when using didReceiveResponseAsync.
    // This is not formally correct, but the client has to be a no-op anyway, because blobs can't be downloaded.
//     if (usesAsyncCallbacks())
//         client()->didReceiveResponseAsync(this, response);
//     else
        m_client->didReceiveResponse(m_loader, response);
}

void BlobResourceLoader::notifyReceiveData(const char* data, int bytesRead)
{
    if (m_client)
        m_client->didReceiveData(m_loader, data, bytesRead, 0);
}

void BlobResourceLoader::notifyFail(int errorCode)
{
    blink::WebURLError error;
    error.domain = blink::WebString::fromUTF8(webKitBlobResourceDomain);
    error.reason = errorCode;
    error.unreachableURL = m_request.url();

    if (m_client)
        m_client->didFail(m_loader, error);
}

void BlobResourceLoader::doNotifyFinish()
{
    if (aborted())
        return;

    m_streamWrap->close();
    m_fileOpened = false;

    if (!m_client)
        return;
    m_client->didFinishLoading(m_loader, 0, 0);
}

void BlobResourceLoader::notifyFinish()
{
    if (!m_async) {
        doNotifyFinish();
        return;
    }

    // Schedule to notify the client from a standalone function because the client might dispose the handle immediately from the callback function
    // while we still have BlobResourceLoader calls in the stack.
    if (!m_doNotifyFinishAsynTaskWeakPtr) {
        m_doNotifyFinishAsynTaskWeakPtr = new LoaderWrap(this, BlobResourceLoader::kNotifyFinish);
        WTF::internal::callOnMainThread(LoaderWrap::doNotifyFinishOnMainThread, m_doNotifyFinishAsynTaskWeakPtr);
    }
}

} // namespace WebCore
