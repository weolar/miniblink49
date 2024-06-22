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

#ifndef CurlCacheManager_h
#define CurlCacheManager_h

#include "net/CancelledReason.h"
#include "net/ProxyType.h"
#include "net/WebURLLoaderInternal.h"
#include "net/CurlCacheEntry.h"

#include "third_party/libcurl/include/curl/curl.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/CString.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/platform/network/HTTPHeaderMap.h"
#include "platform/network/ResourceResponse.h"
#include <map>

#include <wtf/HashMap.h>
#include <wtf/ListHashSet.h>
#include <wtf/text/WTFString.h>

namespace blink {

class WebURLRequest;
class WebURLResponse;
struct WebURLError;
class WebURLLoaderClient;
class WebURLLoader;

}

namespace net {

class CurlCacheManager {

public:
    static CurlCacheManager* getInstance();

    // 设置缓存路径
    void setCacheDirectory(const String&);
    void setCacheLevel(int Level) { m_cacheLevel = Level; };
    const String& cacheDirectory() { return m_cacheDir; }
    void setStorageSizeLimit(size_t);
    void setStorageSizeLimitDisk(size_t);

    void delaySaveTimerFired(blink::Timer<CurlCacheManager>*);

    bool isCached(WebURLLoaderInternal* job) const;
    HTTPHeaderMap& requestHeaders(WebURLLoaderInternal* job); // Load headers
    bool getCachedResponse(const String& url,ResourceResponse&);

    void didReceiveResponse(WebURLLoaderInternal&, ResourceResponse&);
    void didReceiveData(WebURLLoaderInternal&, const char*, size_t); // Save data
    void didFinishLoading(WebURLLoaderInternal&);
    void didFail(WebURLLoaderInternal&);

    void shutdown();
    void save() { saveIndex(); }

private:
    CurlCacheManager();
    ~CurlCacheManager();
    CurlCacheManager(CurlCacheManager const&);

    void addCacheEntry(const String& url, WebURLLoaderInternal& job, const ResourceResponse& response);
    
    static CurlCacheManager *m_instance;

    int m_cacheLevel; // 缓存等级
    bool m_disabled; // 禁用缓存
    String m_cacheDir;
    HashMap<String, CurlCacheEntry*> m_index;

    bool m_isCallingdReadCachedData; // 防止重入

    ListHashSet<String> m_LRUEntryList;
    size_t m_currentStorageSize;
    size_t m_storageSizeLimit;
    size_t m_storageSizeLimitDisk;

    blink::Timer<CurlCacheManager> m_delaySaveTimer;

    void saveIndex();
    void saveIndexForLevel2();
    void loadIndex();
    void makeRoomForNewEntry();
    void makeRoomForNewEntryDisk();

    void saveResponseHeaders(const String&, const ResourceResponse&);
    void invalidateCacheEntry(const String&);
    void readCachedData(const String&, WebURLLoaderInternal*, ResourceResponse&);
};

}

#endif // CurlCacheManager_h
