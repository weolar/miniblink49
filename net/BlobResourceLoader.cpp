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
//#include "net/AsyncFileStream.h"
#include "net/FileStream.h"
#include "net/MemBlobStream.h"
#include "net/FlattenHTTPBodyElement.h"
#include "net/DownloadFileBlobCache.h"
#include "net/ActivatingObjCheck.h"
#include "net/RequestExtraData.h"
#include "content/web_impl_win/WebBlobRegistryImpl.h"
#include "content/browser/WebPage.h"
#include "content/browser/PostTaskHelper.h"
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
    webviewId = 0;
}

BlobDataWrap::~BlobDataWrap()
{
    for (size_t i = 0; i < m_items.size(); i++) {
        blink::WebBlobData::Item* it = m_items[i];
        if (blink::WebBlobData::Item::TypeFile == it->type) {
            BlobTempFileInfo* blobTempFileInfo = DownloadFileBlobCache::inst()->getBlobTempFileInfoByTempFilePath(it->filePath);
            if (blobTempFileInfo) // 拖拽一张本地图片，也会走到这
                blobTempFileInfo->deref();
        }
        delete it;
    }
}

void BlobTempFileInfo::ref()
{
    ++m_refCount;
}

void BlobTempFileInfo::deref()
{
    m_refCount--;
    if (0 != m_refCount)
        return;

    DownloadFileBlobCache::inst()->removeBlobTempFileInfo(this->url);
    delete this;
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

// xhr.open(URL.createObjectURL(inputFile))的时候会走到这里。
// fileReader.readAsArrayBuffer(blob);的时候也会走到这里。
// 下面是个经典的场景，会出现两次BlobResourceLoader：
// function objectURLToBlob(url, callback) {
//   var xhr = new XMLHttpRequest();
//   xhr.open("GET", url, true);
//   xhr.responseType = "blob";
//   xhr.onload = function(e) {
//     if (this.status == 200 || this.status == = 0) {
//       callback(this.response);
//     }
//   };
//   xhr.send();
// }
//
// var fileReader = new FileReader();
// fileReader.onload = function(e) {
//   handleBinaryFile(e.target.result);
// };
// var img = new Image();
// img.src = URL.createObjectURL(file);
// objectURLToBlob(img.src, function(blob) {
//   fileReader.readAsArrayBuffer(blob);
// }
// https://newimage.chinahuanong.com.cn:8001/SunECM/ecm-view/image/webScan/js/uploadify/AjaxUpload.js
// https://newimage.chinahuanong.com.cn:8001/SunECM/ecm-view/image/webScan/js/uploadify/exif.js
// https://newimage.chinahuanong.com.cn:8001/SunECM/ecm-view/image/webScan/js/uploadify/lrz.js

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
    //, m_fileOpened(false)
    , m_isNotifyFinishing(false)
    , m_isNotifyStarting(nullptr)
    , m_isDestroied(nullptr)
{
    //m_streamWrap = std::unique_ptr<StreamWrap>(new StreamWrap(this, m_async));

    m_id = ActivatingObjCheck::inst()->genId();
    ActivatingObjCheck::inst()->add(m_id);

    int webviewId = 0;
    RequestExtraData* requestExtraData = reinterpret_cast<RequestExtraData*>(m_request.extraData());
    if (requestExtraData) {
        content::WebPage* page = requestExtraData->page;
        if (page) {
            webviewId = page->getId();
        }
    }

    ++blobData->m_ref;
    if (0 != blobData->webviewId && 0 != webviewId) {
        ASSERT(blobData->webviewId == webviewId);
    }
    blobData->webviewId = webviewId;
    m_webviewId = webviewId;
}

BlobResourceLoader::~BlobResourceLoader()
{
    cancel();
    content::WebBlobRegistryImpl* blobReg = (content::WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
    if (blobReg)
        blobReg->derefBlobDataWrap(m_blobData);
    ActivatingObjCheck::inst()->remove(m_id);
}

void BlobResourceLoader::cancel()
{
    m_aborted = true;

    if (m_isDestroied)
        *m_isDestroied = true;
}

void BlobResourceLoader::continueDidReceiveResponse()
{
    // BlobResourceLoader doesn't wait for didReceiveResponse, and it currently cannot be used for downloading.
}

void BlobResourceLoader::start()
{
    if (!m_async) {
        doStart();
        return;
    }

    //RefPtr<BlobResourceLoader> handle(this);

    // Finish this async call quickly and return.
    if (m_isNotifyStarting)
        return;
    m_isNotifyStarting = true;

    BlobResourceLoader* self = this;
    int id = m_id;
    content::postTaskToMainThread(FROM_HERE, [self, id] {
        if (!ActivatingObjCheck::inst()->isActivating(id))
            return;
        self->m_isNotifyStarting = false;
        self->doStart();
    });
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

    //////////////////////////////////////////////////////////////////////////
    RELEASE_ASSERT(0 == m_totalRemainingSize);
    String filePath;
    long long fileSizeResult = 0;

    WTF::Vector<FlattenHTTPBodyElement *> flattenElements;
    for (size_t i = 0; i < m_blobData->items().size(); ++i) {
        blink::WebBlobData::Item* item = m_blobData->items().at(i);

        FlattenHTTPBodyElement* flattenElement = nullptr;
        if (blink::WebBlobData::Item::TypeFileSystemURL == item->type)
            filePath = FlattenHTTPBodyElementStream::getPathBySystemURL(item->fileSystemURL);
        else if (blink::WebBlobData::Item::TypeFile == item->type)
            filePath = item->filePath;

        long long offset = item->offset;
        long long length = item->length;
        
        if (blink::WebBlobData::Item::TypeFile == item->type || blink::WebBlobData::Item::TypeFileSystemURL == item->type) {
            if (filePath.startsWith("file:///c:/miniblink_blob_download_")) {
                BlobTempFileInfo* info = DownloadFileBlobCache::inst()->getBlobTempFileInfoByTempFilePath(filePath);
                if (!info)
                    continue;
                fileSizeResult = info->data.size();
            } else {
                if (!getFileSize((String)(filePath), fileSizeResult))
                    continue;
            }

            FlattenHTTPBodyElementStream::clampSliceOffsets(fileSizeResult, &offset, &length);
            m_totalRemainingSize += length;

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = FlattenHTTPBodyElement::Type::TypeFile;
            Vector<UChar> filePathBuf = WTF::ensureUTF16UChar(filePath, true);
            flattenElement->filePath = filePathBuf.data();
            flattenElement->fileStart = item->offset;
            flattenElement->fileLength = item->length;
            flattenElements.append(flattenElement);
        } else if (blink::WebBlobData::Item::TypeData == item->type) {
            FlattenHTTPBodyElementStream::clampSliceOffsets(item->data.size(), &offset, &length);
            m_totalRemainingSize += length;

            flattenElement = new FlattenHTTPBodyElement();
            flattenElement->type = FlattenHTTPBodyElement::Type::TypeData;
            flattenElement->data.append(item->data.data() + (size_t)offset, (size_t)length);
            flattenElements.append(flattenElement);
        } else if (blink::WebBlobData::Item::TypeBlob == item->type) {
            FlattenHTTPBodyElementStream::flatten(item->blobUUID, &m_totalRemainingSize, &flattenElements, item->offset, item->length, true, 1);
        }
    }
    notifyResponse();

    FlattenHTTPBodyElementStream bodyStream(flattenElements, m_totalRemainingSize);
    while (true) {
        if (!bodyStream.hasMoreElements())
            break;

        std::vector<char> buf;
        size_t sent = bodyStream.read(nullptr, 1, -1, &buf);
        RELEASE_ASSERT(sent == buf.size());
        if (0 != sent)
            notifyReceiveData(&buf[0], buf.size());
    }
    notifyFinish();
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
}

void BlobResourceLoader::notifyResponse()
{
    if (!m_client)
        return;

    if (m_errorCode) {
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

void BlobResourceLoader::notifyReceiveData(const char* data, int bytesRead)
{
    if (!m_client)
        return;

    if (m_request.downloadToFile() && !m_request.useStreamOnResponse()) {
        DownloadFileBlobCache::inst()->appendDataToBlobCache(m_client, m_loader, m_request.url().string(), data, bytesRead, 0);
    } else
        m_client->didReceiveData(m_loader, data, bytesRead, 0);
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

    WebURLLoaderManager* manager = WebURLLoaderManager::sharedInstance();
    if (!manager)
        return;

    // xhr.open(URL.createObjectURL(inputFile))
    // xhr.responseType = "blob";的时候downloadToFile为true。
    if (m_request.downloadToFile()) {
        String path = DownloadFileBlobCache::inst()->createBlobTempFileInfoByUrlIfNeeded(m_request.url().string());
        if (!path.isNull() && !path.isEmpty())
            response.setDownloadFilePath(path);
    }

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
    if (m_isNotifyFinishing)
        return;
    m_isNotifyFinishing = true;

    BlobResourceLoader* self = this;
    int id = m_id;
    content::postTaskToMainThread(FROM_HERE, [self, id] {
        if (!ActivatingObjCheck::inst()->isActivating(id))
            return;
        self->m_isNotifyFinishing = false;
        self->doNotifyFinish();
    });
}

} // namespace net
