
#ifndef net_MemBlobStream_h
#define net_MemBlobStream_h

#include "net/FileStreamClient.h"
#include "net/LambdaTask.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include <wtf/text/WTFString.h>

namespace net {

class MemBlobStream {
private:
    struct SelfWrap {
        SelfWrap(FileStreamClient* client) {
            isDestroied = false;
            m_client = client;
        }
        bool isDestroied;
        FileStreamClient* m_client;
    };
public:
    MemBlobStream(FileStreamClient* client, bool isAsync) {
        m_offset = 0;
        m_length = 0;
        m_client = client;
        m_info = nullptr;
        m_isAsync = isAsync;
        m_selfWrap = new SelfWrap(client);
    }

    ~MemBlobStream() {
        if (m_isAsync)
            return;

        SelfWrap* selfWrap = m_selfWrap;
        selfWrap->isDestroied = true;
        LambdaTask::asyncCall([selfWrap] {
            delete selfWrap;
        });
    }

    long long getSize(const String& path, double expectedModificationTime) {
        if (m_info)
            return m_info->data.size();
        
        if (!path.startsWith("file:///c:/miniblink_blob_download_")) {
            DebugBreak();              
            return -1;
        }

        BlobTempFileInfo* info = WebURLLoaderManager::sharedInstance()->getBlobTempFileInfoByTempFilePath(path);
        if (!info)
            return -1;
        
        m_info = info;
        ++m_info->refCount;

        return m_info->data.size();
    }

    void init(long offset, long length) {
        ++m_info->refCount;
        m_offset = offset;
        m_length = length;
        SelfWrap* selfWrap = m_selfWrap;
        if (m_isAsync) {
            LambdaTask::asyncCall([selfWrap] {
                if (selfWrap->isDestroied)
                    return;
                selfWrap->m_client->didOpen(true);
            });
        }
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
            return false;
        }

        BlobTempFileInfo* info = WebURLLoaderManager::sharedInstance()->getBlobTempFileInfoByTempFilePath(path);
        if (!info || offset + length > info->data.size()) {
            DebugBreak();
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
    }

    int read(char* buffer, int length) {
        if (!m_info)
            return 0;

        if (m_offset + length > m_length)
            length = m_length - m_offset;

        memcpy(buffer, m_info->data.data() + m_offset, length);
        m_offset += length;

        if (m_isAsync) {
            //blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&FileStreamClient::didRead, m_client, length));
            SelfWrap* selfWrap = m_selfWrap;
            LambdaTask::asyncCall([selfWrap, length] {
                if (selfWrap->isDestroied)
                    return;
                selfWrap->m_client->didRead(length);
            });
        }
        return length;
    }

private:
    bool m_openIsOk;
    long m_offset;
    long m_length;
    BlobTempFileInfo* m_info;
    FileStreamClient* m_client;
    bool m_isAsync;
    SelfWrap* m_selfWrap;
};

}

#endif // net_MemBlobStream_h