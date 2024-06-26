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

#ifndef CurlCacheEntry_h
#define CurlCacheEntry_h

#include "net/CancelledReason.h"
#include "net/ProxyType.h"
#include "net/WebURLLoaderInternal.h"
#include "net/FileSystem.h"

#include "third_party/libcurl/include/curl/curl.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/platform/network/HTTPHeaderMap.h"
#include "third_party/WebKit/Source/platform/network/ResourceResponse.h"
#include <wtf/HashMap.h>
#include <wtf/ListHashSet.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>
#include <wtf/text/WTFString.h>
#include <map>
#include <vector>

namespace net {

class CurlCacheEntry {
public:    
    CurlCacheEntry(const String& url, WebURLLoaderInternal* job, const String& cacheDir, int Level);
    ~CurlCacheEntry();

    bool isCached();
    bool isLoading() const;

    bool isInRange(const WebURLLoaderInternal& job) const;
    bool isRequstInRange(const WebURLLoaderInternal& job) const;

    size_t entrySize();
    blink::HTTPHeaderMap& requestHeaders() { return m_requestHeaders; }

    bool onPartialContentResponse(WebURLResponse& response);

    bool saveCachedData(WebURLLoaderInternal& job, const char* data, size_t);
    bool readCachedData(WebURLLoaderInternal*);

    bool saveHeadersToFile();
    bool saveResponseHeaders(const ResourceResponse&);
    void setResponseFromCachedHeaders(ResourceResponse&);

    void invalidate();
    void didFail();
    void didFinishLoading();

    bool parseResponseHeaders(const ResourceResponse&);

    const WebURLLoaderInternal* getJob() const { return m_job; }

    Vector<char>* headerBuffer() { return &m_headerBuffer; };
    Vector<char>* contentBuffer() { return &m_contentBuffer; };
    bool loadResponseHeaders(Vector<char> &header);

private:
    String m_basename;
    String m_headerFilename;
    String m_contentFilename;
    String m_contentTempFilename;

    PlatformFileHandle m_contentTempFile;

    size_t m_entrySize;
    double m_expireDate;
    bool m_headerParsed;

    // 内存优化
    int m_cacheLevel; // 优化等级
    bool m_loading;
    bool m_headerMem;
    bool m_contentMem;
    Vector<char> m_headerBuffer;
    Vector<char> m_contentBuffer;
    double m_fileTime;

    
    std::vector<int64_t> m_rangs;
    int m_instanceSize;// 总共的大小，-1表示还没获取到总大小
    int m_curPartialPos;

    ResourceResponse m_cachedResponse;
    blink::HTTPHeaderMap m_requestHeaders;

    WebURLLoaderInternal* m_job;

    void generateBaseFilename(const CString& url);
    bool loadFileToBuffer(const String& filepath, Vector<char>& buffer);
    bool loadResponseHeaders();

    bool openContentTempFile();
    bool closeContentFile(PlatformFileHandle* contentFile);

    void onCachedDataFinish();
};

}

#endif // CurlCacheEntry_h