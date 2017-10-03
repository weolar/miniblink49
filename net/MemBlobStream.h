
#ifndef net_MemBlobStream_h
#define net_MemBlobStream_h

#include "net/FileStreamClient.h"

#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include <wtf/text/WTFString.h>

namespace net {

class MemBlobStream {
public:
    MemBlobStream(FileStreamClient* client, bool async) {
        m_offset = 0;
        m_length = 0;
        m_client = client;
        m_info = nullptr;
        m_async = async;
    }

    long long getSize(const String& path, double expectedModificationTime) {
        if (m_info) {
            if (!m_async)
                return  m_info->data.size();
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didGetSize, m_client, m_info->data.size()));
            return 0;
        }

        if (!path.startsWith("file:///c:/miniblink_blob_download_")) {
            DebugBreak();
            if (m_async)
                blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didGetSize, m_client, 0));
            return 0;
        }

        BlobTempFileInfo* info = WebURLLoaderManager::sharedInstance()->getBlobTempFileInfoByTempFilePath(path);
        if (!info) {
            if (m_async)
                blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didGetSize, m_client, 0));
            return 0;
        }
        m_info = info;
        ++m_info->refCount;

        if (m_async)
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didGetSize, m_client, m_info->data.size()));
        return m_info->data.size();
    }

    void init(long offset, long length) {
        ++m_info->refCount;
        m_offset = offset;
        m_length = length;

        if (m_async)
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didOpen, m_client, true));
    }

    bool openForRead(const String& path, long long offset, long long length) {
        if (m_info) {
            if (m_info->tempUrl != path || m_offset != 0 || m_length != 0) {
                DebugBreak();
                return false;
            }

            init(offset, length);
            return true;
        }

        if (!path.startsWith("file:///c:/miniblink_blob_download_") || 0 > offset || 0 > length) {
            DebugBreak();
            if (m_async)
                blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didOpen, m_client, false));
            return false;
        }
        BlobTempFileInfo* info = WebURLLoaderManager::sharedInstance()->getBlobTempFileInfoByTempFilePath(path);
        if (!info || offset + length > info->data.size()) {
            DebugBreak();
            if (m_async)
                blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didOpen, m_client, false));
            return false;
        }

        m_info = info;
        init(offset, length);

        return true;
    }

    void close() {
        if (!m_info) {
            DebugBreak();
            return;
        }

        --m_info->refCount;

        if (m_async)
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didOpen, m_client, true));
    }

    int read(char* buffer, int length) {
        if (!m_info)
            return 0;

        if (m_offset + length > m_length)
            length = m_length - m_offset;

        memcpy(buffer, m_info->data.data() + m_offset, length);
        m_offset += length;

        if (m_async)
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didRead, m_client, length));
        return length;
    }

private:
    bool m_openIsOk;
    long m_offset;
    long m_length;
    BlobTempFileInfo* m_info;
    FileStreamClient* m_client;
    bool m_async;
};

}

#endif // net_MemBlobStream_h