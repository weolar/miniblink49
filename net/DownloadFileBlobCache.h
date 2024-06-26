
#ifndef net_DownloadFileBlobCache_h
#define net_DownloadFileBlobCache_h

namespace net {

class DownloadFileBlobCache {
public:
    static DownloadFileBlobCache* inst();

    void shutdown();

    void DownloadFileBlobCache::appendDataToBlobCache(
        blink::WebURLLoaderClient* client,
        blink::WebURLLoader* loader,
        const String& url,
        const char* data,
        int dataLength,
        int encodedDataLength
        );

    BlobTempFileInfo* getBlobTempFileInfoByTempFilePath(const String& path);
    String createBlobTempFileInfoByUrlIfNeeded(const String& url);
    void removeBlobTempFileInfo(const String& path);

private:
    static DownloadFileBlobCache* m_inst;

    typedef WTF::HashMap<String, BlobTempFileInfo*> BlobCache;
    BlobCache m_blobCaches; // real url -> <temp, data>
};
    
}

#endif