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

#ifndef BlobResourceHandle_h
#define BlobResourceHandle_h

#include "net/FileStreamClient.h"
#include "net/StreamReader.h"
#include "third_party/WebKit/public/platform/WebBlobData.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include <wtf/PassRefPtr.h>
#include <wtf/Vector.h>
#include <wtf/text/WTFString.h>
#include <memory>

namespace blink {
class WebURLRequest;
class WebURLResponse;
class WebURLLoaderClient;
class WebURLLoader;
struct WebURLError;
}

namespace net {

class BlobDataWrap {
public:
    BlobDataWrap();
    ~BlobDataWrap();

    const Vector<blink::WebBlobData::Item*>& items() const { return m_items; }
    void appendItem(blink::WebBlobData::Item* item) { m_items.append(item); }

    int m_ref;
    String m_contentType;
private:
    Vector<blink::WebBlobData::Item*> m_items;
};

struct BlobTempFileInfo {
    String tempUrl;
    Vector<char> data;
    int refCount;
};

class StreamWrap;

class BlobResourceLoader : public FileStreamClient {
public:
    static BlobResourceLoader* createAsync(BlobDataWrap*, const blink::WebURLRequest&, blink::WebURLLoaderClient*, blink::WebURLLoader*);

    static void loadResourceSynchronously(BlobDataWrap*, const blink::WebURLRequest&, blink::WebURLLoader*, blink::WebURLError&, blink::WebURLResponse&, Vector<char>& data);

    void start();
    int readSync(char*, int);

    bool aborted() const { return m_aborted; }

    // ResourceHandle methods.
    /*virtual*/ void cancel() /*override*/;
    /*virtual*/ void continueDidReceiveResponse() /*override*/;

    ~BlobResourceLoader();

private:
    BlobResourceLoader(BlobDataWrap*, const blink::WebURLRequest&, blink::WebURLLoaderClient*, blink::WebURLLoader* loader, bool async);
    
    // FileStreamClient methods.
    virtual void didGetSize(long long) override;
    virtual void didOpen(bool) override;
    virtual void didRead(int) override;

    void doNotifyFinish();
    void doStart();
    void getSizeForNext();
    void seek();
    void consumeData(const char* data, int bytesRead);
    void failed(int errorCode);

    void readAsync();
    void readDataAsync(const blink::WebBlobData::Item&);
    void readFileAsync(const blink::WebBlobData::Item&);

    int readDataSync(const blink::WebBlobData::Item&, char*, int);
    int readFileSync(const blink::WebBlobData::Item&, char*, int);

    void notifyResponse();
    void notifyResponseOnSuccess();
    void notifyResponseOnError();
    void notifyReceiveData(const char*, int);
    void notifyFail(int errorCode);
    void notifyFinish();

    blink::WebURLRequest m_request;
    blink::WebURLLoader* m_loader;
    blink::WebURLLoaderClient* m_client;

    BlobDataWrap* m_blobData;
    bool m_async;
    std::unique_ptr<StreamWrap> m_streamWrap;
    Vector<char> m_buffer;
    Vector<long long> m_itemLengthList;
    int m_errorCode;
    bool m_aborted;
    long long m_rangeOffset;
    long long m_rangeEnd;
    long long m_rangeSuffixLength;
    long long m_totalRemainingSize;
    long long m_currentItemReadSize;
    unsigned m_sizeItemCount;
    unsigned m_readItemCount;
    bool m_fileOpened;

    friend class LoaderWrap;
    enum RunType {
        kStart,
        kNotifyFinish
    };
    LoaderWrap* m_doNotifyFinishAsynTaskWeakPtr;
    LoaderWrap* m_doStartAsynTaskWeakPtr;
    void asynTaskFinish(RunType runType);

    bool* m_isDestroied;
};

} // namespace WebCore

#endif // BlobResourceHandle_h
