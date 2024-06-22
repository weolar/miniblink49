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

#include "net/CurlCacheManager.h"

#include "net/FileSystem.h"
#include "third_party/WebKit/Source/platform/network/HTTPHeaderMap.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include <wtf/HashMap.h>
#include <wtf/text/CString.h>
#include <wke/wkeGlobalVar.h>
#define IO_BUFFERSIZE 4096

namespace net {

static bool needCache(const WebURLLoaderInternal& job)
{
    KURL url = job.firstRequest()->url();
    if (!url.protocolIsInHTTPFamily()) // 非http协议的不缓存了
        return false;

    const String& urlStr = url.string();
    if (WTF::kNotFound == urlStr.find(".mp4")) // 非视频也不缓存
        return false;
    return true;
}

CurlCacheManager* CurlCacheManager::m_instance = nullptr;

CurlCacheManager* CurlCacheManager::getInstance()
{
    if (!m_instance) {
        m_instance = new CurlCacheManager;
    }
    return m_instance;
}

CurlCacheManager::CurlCacheManager()
    : m_disabled(true)
    , m_isCallingdReadCachedData(false)
    , m_currentStorageSize(0)
    , m_storageSizeLimit(1024 * 1024 * 100) // 50 * 1024 * 1024 bytes
    , m_storageSizeLimitDisk(1024 * 1024 * 100)
    , m_cacheLevel(0)
    , m_cacheDir("")
    , m_delaySaveTimer(this, &CurlCacheManager::delaySaveTimerFired)
{
    // Call setCacheDirectory() to enable the Cache Manager
    m_delaySaveTimer.startRepeating(3.5, FROM_HERE);
}

CurlCacheManager::~CurlCacheManager()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    saveIndex();
}

void CurlCacheManager::delaySaveTimerFired(blink::Timer<CurlCacheManager>*)
{
    saveIndex();
}

void CurlCacheManager::setCacheDirectory(const String& directory)
{
    if (!wke::g_diskCacheEnable)
        return;
    m_cacheDir = directory;

    if (m_cacheDir.isEmpty()) {
        //LOG(Network, "Cache Error: Cache location is not set! CacheManager disabled.\n");
        m_disabled = true;
        return;
    }

    if (!fileExists(m_cacheDir)) {
        if (!makeAllDirectories(m_cacheDir)) {
            //LOG(Network, "Cache Error: Could not open or create cache directory! CacheManager disabled.\n");
            m_disabled = true;
            return;
        }
    }

    m_cacheDir.append("/");

    m_disabled = false;
    loadIndex();
}

void CurlCacheManager::setStorageSizeLimit(size_t sizeLimit)
{
    m_storageSizeLimit = sizeLimit;
}

void CurlCacheManager::setStorageSizeLimitDisk(size_t sizeLimit)
{
    m_storageSizeLimitDisk = sizeLimit;
}

void CurlCacheManager::loadIndex()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    if (m_cacheLevel == 2) {
        DebugBreak();
//         String indexFilePath(m_cacheDir);
//         indexFilePath.append("index_m.dat");
// 
//         PlatformFileHandle indexFile = openFile(indexFilePath, OpenForRead);
//         if (!isHandleValid(indexFile)) {
//             //LOG(Network, "Cache Warning: Could not open %s for read\n", indexFilePath.latin1().data());
//             return;
//         }
// 
//         uint32_t size = 0;
//         Vector<char> buffer;
//         while (readFromFile(indexFile, (char *)&size, sizeof(uint32_t)) == 4) {
//             buffer.resize(size);
//             if (readFromFile(indexFile, buffer.data(), size) != size)// 读url
//                 break;
// 
//             String url(buffer.data(), buffer.size());
//             CurlCacheEntry* cacheEntry = new CurlCacheEntry(url, nullptr, m_cacheDir, m_cacheLevel);
//             
//             if (readFromFile(indexFile, (char*)&size, sizeof(uint32_t)) != 4) { // 读头
//                 delete cacheEntry;
//                 break;
//             }
// 
//             buffer.resize(size);
//             if (readFromFile(indexFile, buffer.data(), size) != size) {
//                 delete cacheEntry;
//                 break;
//             }
//             cacheEntry->loadResponseHeaders(buffer);
//             
//             if (readFromFile(indexFile, (char*)&size, sizeof(uint32_t)) != 4) { // 读数据
//                 delete cacheEntry;
//                 break;
//             }
// 
//             buffer.resize(size);
//             if (readFromFile(indexFile, buffer.data(), size) != size) {
//                 delete cacheEntry;
//                 break;
//             }
//             
//             cacheEntry->saveCachedData(buffer.data(), buffer.size());
//             cacheEntry->didFinishLoading();
// 
//             if (cacheEntry->isCached() && cacheEntry->entrySize() < m_storageSizeLimit) {
//                 m_currentStorageSize += cacheEntry->entrySize();
//                 makeRoomForNewEntry();
//                 m_LRUEntryList.prependOrMoveToFirst(url);
//                 m_index.set(url, cacheEntry);
//             } else {
//                 cacheEntry->invalidate();
//                 delete cacheEntry;
//             }
//         }
//         closeFile(indexFile);
//         return;
    }

    String indexFilePath(m_cacheDir);
    indexFilePath.append("index.dat");

    PlatformFileHandle indexFile = openFile(indexFilePath, OpenForRead);
    if (!isHandleValid(indexFile)) {
        //LOG(Network, "Cache Warning: Could not open %s for read\n", indexFilePath.latin1().data());
        return;
    }

    long long filesize = -1;
    if (!getFileSize(indexFilePath, filesize)) {
        //LOG(Network, "Cache Error: Could not get file size of %s\n", indexFilePath.latin1().data());
        return;
    }

    // Load the file content into buffer
    Vector<char> buffer;
    buffer.resize(filesize);
    int bufferPosition = 0;
    int bufferReadSize = IO_BUFFERSIZE;
    while (filesize > bufferPosition) {
        if (filesize - bufferPosition < bufferReadSize)
            bufferReadSize = filesize - bufferPosition;

        readFromFile(indexFile, buffer.data() + bufferPosition, bufferReadSize);
        bufferPosition += bufferReadSize;
    }
    closeFile(indexFile);

    // Create strings from buffer
    String headerContent = String(buffer.data(), buffer.size());
    Vector<String> indexURLs;
    headerContent.split('\n', indexURLs);
    buffer.clear();

    // Add entries to index
    Vector<String>::const_iterator it = indexURLs.begin();
    Vector<String>::const_iterator end = indexURLs.end();
    if (indexURLs.size() > 1)
        --end; // Last line is empty

    while (it != end) {
        String url = it->stripWhiteSpace();
        CurlCacheEntry* cacheEntry = new CurlCacheEntry(url, nullptr, m_cacheDir, m_cacheLevel);

        if (cacheEntry->isCached() && cacheEntry->entrySize() < m_storageSizeLimit) {
            m_currentStorageSize += cacheEntry->entrySize();
            makeRoomForNewEntry();
            m_LRUEntryList.prependOrMoveToFirst(url);
            m_index.set(url, cacheEntry);
        } else {
            cacheEntry->invalidate();
            delete cacheEntry;
        }

        ++it;
    }
}

void CurlCacheManager::saveIndexForLevel2()
{
    makeRoomForNewEntryDisk(); // 淘汰部分数据

    String indexFilePath(m_cacheDir);
    indexFilePath.append("index_m.dat");

    deleteFile(indexFilePath);
    PlatformFileHandle indexFile = openFile(indexFilePath, OpenForWrite);
    if (!isHandleValid(indexFile)) {
        //LOG(Network, "Cache Error: Could not open %s for write\n", indexFilePath.latin1().data());
        return;
    }

    ListHashSet<String>::iterator it = m_LRUEntryList.begin();
    const ListHashSet<String>::iterator& end = m_LRUEntryList.end();
    while (it != end) {
        const CString& urlLatin1 = it->latin1();
        HashMap<String, CurlCacheEntry *>::iterator  ic = m_index.find(String(it->utf8().data(), it->utf8().length()));
        if (ic != m_index.end()) {

            uint32_t size = urlLatin1.length();
            writeToFile(indexFile, (const char*)& size, sizeof(uint32_t)); // url长度

            writeToFile(indexFile, urlLatin1.data(), urlLatin1.length()); // url

            Vector<char>* h = ic->value->headerBuffer();
            size = h->size();
            writeToFile(indexFile, (const char*)& size, sizeof(uint32_t)); // 头长度

            writeToFile(indexFile, h->data(), h->size()); // 头

            Vector<char>* c = ic->value->contentBuffer();
            size = c->size();
            writeToFile(indexFile, (const char*)& size, sizeof(uint32_t)); // 数据长度

            writeToFile(indexFile, c->data(), c->size()); // 头
            ic->value->saveHeadersToFile();
        }
        ++it;
    }

    closeFile(indexFile);
}

void CurlCacheManager::saveIndex()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    if (m_cacheLevel == 2)
        return saveIndexForLevel2();

    String indexFilePath(m_cacheDir);
    indexFilePath.append("index.dat");

    deleteFile(indexFilePath);
    PlatformFileHandle indexFile = openFile(indexFilePath, OpenForWrite);
    if (!isHandleValid(indexFile)) {
        //LOG(Network, "Cache Error: Could not open %s for write\n", indexFilePath.latin1().data());
        return;
    }

    ListHashSet<String>::const_iterator it = m_LRUEntryList.begin();
    ListHashSet<String>::const_iterator end = m_LRUEntryList.end();
    while (it != end) {
        const CString& urlLatin1 = it->latin1();
        writeToFile(indexFile, urlLatin1.data(), urlLatin1.length());
        writeToFile(indexFile, "\n", 1);

        HashMap<String, CurlCacheEntry *>::iterator ic = m_index.find(String(it->utf8().data(), it->utf8().length()));
        if (ic != m_index.end())
            ic->value->saveHeadersToFile();

        ++it;
    }

    closeFile(indexFile);
}

void CurlCacheManager::makeRoomForNewEntry()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    while ((m_currentStorageSize > m_storageSizeLimit) && m_LRUEntryList.size() > 0) {
        ASSERT(m_index.find(m_LRUEntryList.last()) != m_index.end());
        invalidateCacheEntry(m_LRUEntryList.last());
    }
}

void CurlCacheManager::makeRoomForNewEntryDisk()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    while ((m_currentStorageSize > m_storageSizeLimitDisk) && m_LRUEntryList.size() > 0) {
        ASSERT(m_index.find(m_LRUEntryList.last()) != m_index.end());
        invalidateCacheEntry(m_LRUEntryList.last());
    }
}

void CurlCacheManager::addCacheEntry(const String& url, WebURLLoaderInternal& job, const ResourceResponse& response)
{
    CurlCacheEntry* cacheEntry = new CurlCacheEntry(url, &job, m_cacheDir, m_cacheLevel);
    bool cacheable = cacheEntry->parseResponseHeaders(response);
    if (cacheable) {
        m_LRUEntryList.prependOrMoveToFirst(url);
        m_index.set(url, cacheEntry);
        saveResponseHeaders(url, response);
    } else
        delete cacheEntry;
}

void CurlCacheManager::didReceiveResponse(WebURLLoaderInternal& job, ResourceResponse& response)
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    if (job.isCancelled())
        return;

    if (!needCache(job))
        return;

    String range1 = response.httpHeaderField(WebString::fromUTF8("Content-Range"));
    String range2 = job.firstRequest()->httpHeaderField(WebString::fromUTF8("Range"));
    String output = "CurlCacheManager.didReceiveResponse:[";
    output.append(range1);
    output.append("], [");
    output.append(range2);
    output.append("], ");
    output.append(String::format("%d\n", response.httpStatusCode()));
    OutputDebugStringA(output.utf8().data());

    const String& url = job.firstRequest()->url().string();

    if (response.httpStatusCode() == 304) {
        readCachedData(url, &job, response);
        m_LRUEntryList.prependOrMoveToFirst(url);
    } else if (response.httpStatusCode() == 200) {


        HashMap<String, CurlCacheEntry*>::iterator it = m_index.find(url);
        if (it != m_index.end() && it->value->isLoading())
            return;

        invalidateCacheEntry(url); // Invalidate existing entry on 200
        addCacheEntry(url, job, response);
    } else if (response.httpStatusCode() == 206) { // Partial Content
        HashMap<String, CurlCacheEntry*>::iterator it = m_index.find(url);
        if (it != m_index.end()) {
            // todo: 通知CacheEntry，把range记下来，并且调整文件写入的offset，等下didReceiveData要用到
            //it->value->onPartialContentResponse(response);
        } else
            addCacheEntry(url, job, response);
    } else
        invalidateCacheEntry(url);
}

void CurlCacheManager::didReceiveData(WebURLLoaderInternal& job, const char* data, size_t size)
{
    if (m_disabled || !wke::g_diskCacheEnable || m_isCallingdReadCachedData)
        return;

    const String& url = job.firstRequest()->url().string();

    HashMap<String, CurlCacheEntry*>::iterator it = m_index.find(url);
    if (it != m_index.end()) {
#if 0
        if (it->value->getJob() != &job)
            return;
#endif
        if (!it->value->saveCachedData(job, data, size)) {
            invalidateCacheEntry(url);
        } else {
            m_currentStorageSize += size;
            m_LRUEntryList.prependOrMoveToFirst(url);
            makeRoomForNewEntry();
        }
    }
}

void CurlCacheManager::didFinishLoading(WebURLLoaderInternal& job)
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    const String& url = job.firstRequest()->url().string();

    HashMap<String, CurlCacheEntry *>::iterator it = m_index.find(url);
    if (it != m_index.end())
        it->value->didFinishLoading();
}

bool CurlCacheManager::isCached(WebURLLoaderInternal* job) const
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return false;

    if (!needCache(*job))
        return false;

    KURL url = job->firstRequest()->url();
    const String& urlStr = url.string();
    HashMap<String, CurlCacheEntry *>::const_iterator it = m_index.find(urlStr);
    if (it == m_index.end())
        return false;

    // 如果是partial请求，要检查是否请求的range在缓存里面
    // 23.3.7更新：对于range请求，要所有内容都下载完才算被缓存
//     String range = job->firstRequest()->httpHeaderField("Range");
//     if (range.startsWith("bytes=")) {
//         return it->value->isRequstInRange(*job);
//     }

    return it->value->isCached() && !it->value->isLoading();    
}

HTTPHeaderMap& CurlCacheManager::requestHeaders(WebURLLoaderInternal* job)
{
    ASSERT(isCached(job));
    const String& url = job->firstRequest()->url().string();
    return m_index.find(url)->value->requestHeaders();
}

bool CurlCacheManager::getCachedResponse(const String& url,ResourceResponse& response)
{
    HashMap<String, CurlCacheEntry *>::iterator it = m_index.find(url);
    if (it != m_index.end()) {
        it->value->setResponseFromCachedHeaders(response);
        return true;
    }
    return false;
}

void CurlCacheManager::saveResponseHeaders(const String& url, const ResourceResponse& response)
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    HashMap<String, CurlCacheEntry *>::iterator it = m_index.find(url);
    if (it != m_index.end()) {
        if (!it->value->saveResponseHeaders(response))
            invalidateCacheEntry(url);
    }
}

void CurlCacheManager::invalidateCacheEntry(const String& url)
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    HashMap<String, CurlCacheEntry *>::iterator it = m_index.find(url);
    if (it != m_index.end()) {
        if (m_currentStorageSize < it->value->entrySize())
            m_currentStorageSize = 0;
        else
            m_currentStorageSize -= it->value->entrySize();

        it->value->invalidate();
        delete it->value;
        m_index.remove(url);
    }
    m_LRUEntryList.remove(url);
}

void CurlCacheManager::didFail(WebURLLoaderInternal& job)
{
    const String& url = job.firstRequest()->url().string();
    invalidateCacheEntry(url);
}

void CurlCacheManager::shutdown()
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;
    saveIndex(); 
}

void CurlCacheManager::readCachedData(const String& url, WebURLLoaderInternal* job, ResourceResponse& response)
{
    if (m_disabled || !wke::g_diskCacheEnable)
        return;

    m_isCallingdReadCachedData = true;

    String range = job->m_response.httpHeaderField(WebString::fromUTF8("Content-Range"));
    String range2 = job->firstRequest()->httpHeaderField(WebString::fromUTF8("Range"));

    HashMap<String, CurlCacheEntry *>::iterator it = m_index.find(url);
    if (it != m_index.end()) {
        it->value->setResponseFromCachedHeaders(response);
        m_LRUEntryList.prependOrMoveToFirst(url);
        if (!it->value->readCachedData(job))
            invalidateCacheEntry(url);
    }

    m_isCallingdReadCachedData = false;
}

}

