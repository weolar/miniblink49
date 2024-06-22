
#ifndef net_FlattenHTTPBodyElement_h
#define net_FlattenHTTPBodyElement_h

#include "content/web_impl_win/WebBlobRegistryImpl.h"
#include "third_party/WebKit/public/platform/WebHTTPBody.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/platform/FileMetadata.h"
#include "net/BlobResourceLoader.h"
#include "net/FileSystem.h"
#include "net/WebURLLoaderManager.h"
#include "net/DownloadFileBlobCache.h"
#include <vector>

namespace net {

struct FlattenHTTPBodyElement {
    enum Type {
        TypeData, TypeFile, TypeBlob
    } type;

    Vector<char> data;
    std::wstring filePath;
    long long fileStart;
    long long fileLength; // -1 means to the end of the file.
};

class FlattenHTTPBodyElementStream {
public:
    FlattenHTTPBodyElementStream(const WTF::Vector<FlattenHTTPBodyElement*>& elements, curl_off_t size)
    {
        m_elements = elements;
        m_estimateSize = size;
        m_allReadLength = 0;
        m_file = nullptr;
        reset();
    }

    ~FlattenHTTPBodyElementStream()
    {
        for (size_t i = 0; i < m_elements.size(); ++i) {
            delete m_elements[i];
        }
        reset();
    }

    void reset()
    {
        if (m_file)
            fclose(m_file);
        m_file = nullptr;
        m_elementIndex = 0;
        m_fileSize = 0;
        m_elementDataOffset = 0;
        m_elementDataLength = 0;
        m_totalFileReadLength = 0;
        m_originalDataOffset = 0;
    }
   
    size_t read(void* ptr, size_t blockSize, size_t numberOfBlocks, std::vector<char>* outBuf)
    {
        // Check for overflow.
        if (!numberOfBlocks || blockSize > std::numeric_limits<size_t>::max() / numberOfBlocks)
            return 0;
        if (m_elementIndex >= m_elements.size())
            return 0;

        const FlattenHTTPBodyElement& element = *(m_elements[m_elementIndex]);
        size_t readedLength = 0;
        
        if (FlattenHTTPBodyElement::Type::TypeFile == element.type) {
            readedLength = readFile(ptr, blockSize, numberOfBlocks, element, outBuf);
        } else {
            readedLength = readBuf(ptr, blockSize, numberOfBlocks, element, outBuf);
        }

//         static int allSize = 0;
//         allSize += readedLength;
//
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "readFile: %d, allSize:%d\n", readedLength, allSize);
//         OutputDebugStringA(output);
//         free(output);

        return readedLength;
    }

private:
    size_t readBuf(void* ptr, size_t blockSize, size_t numberOfBlocks, const FlattenHTTPBodyElement& element, std::vector<char>* outBuf)
    {
        if (-1 == numberOfBlocks)
            numberOfBlocks = element.data.size();

        const size_t maxBufferLength = blockSize * numberOfBlocks;
        size_t elementSize = element.data.size() - m_elementDataOffset;
        size_t readedLength = elementSize > maxBufferLength ? maxBufferLength : elementSize;

        if (readedLength > 0) {
            const char* realData = element.data.data() + m_elementDataOffset;
            if (!ptr) {
                outBuf->resize(readedLength);
                ptr = &outBuf->at(0);
            }
            memcpy(ptr, realData, readedLength);
            m_allReadLength += readedLength;
        }

        if (elementSize > readedLength)
            m_elementDataOffset += readedLength;
        else {
            m_elementDataOffset = 0;
            m_elementIndex++;
        }
        return readedLength;
    }

    size_t readBlobDownloadFile(void* ptr, size_t blockSize, size_t numberOfBlocks, const FlattenHTTPBodyElement& element, std::vector<char>* outBuf)
    {
        RELEASE_ASSERT(!ptr && -1 == numberOfBlocks);

        BlobTempFileInfo* info = DownloadFileBlobCache::inst()->getBlobTempFileInfoByTempFilePath(element.filePath.c_str());
        RELEASE_ASSERT(info);

        size_t fileSize = element.fileLength;
        if (-1 == fileSize || fileSize > info->data.size())
            fileSize = info->data.size();
        outBuf->resize(fileSize);

        if (0 != fileSize) 
            memcpy(&outBuf->at(0), info->data.data(), fileSize);
        
        m_allReadLength += fileSize;
        readFileFinish(nullptr, nullptr);
        return fileSize;
    }

    size_t readFile(void* ptr, size_t blockSize, size_t numberOfBlocks, const FlattenHTTPBodyElement& element, std::vector<char>* outBuf)
    {
        wchar_t blobDownloadPath[] = L"file:///c:/miniblink_blob_download_";
        std::wstring subPath = element.filePath.substr(0, sizeof(blobDownloadPath) / sizeof(wchar_t) - 1);
        if (subPath == blobDownloadPath)
            return readBlobDownloadFile(ptr, blockSize, numberOfBlocks, element, outBuf);

        if (!m_file) {
            m_file = _wfopen(element.filePath.c_str(), L"rb");

            if (!m_file) {
                readFileFinish(L"FlattenHTTPBodyElementStream._wfopen Fail:", element.filePath.c_str());
                // FIXME: show a user error?
                return 0;
            } else {
                fseek(m_file, 0L, SEEK_END);
                m_fileSize = ftell(m_file);
                fseek(m_file, 0, SEEK_SET);

                m_elementDataOffset = element.fileStart;
                m_elementDataLength = element.fileLength;

                if (0 == m_fileSize || m_elementDataOffset > m_fileSize) {
                    readFileFinish(nullptr, nullptr);
                    return 0;
                }

                if (-1 == element.fileLength || m_elementDataLength > m_fileSize)
                    m_elementDataLength = m_fileSize;
                m_totalFileReadLength = m_elementDataLength;

                if (m_elementDataOffset + m_elementDataLength > m_fileSize)
                    m_elementDataLength = m_fileSize - m_elementDataOffset;
                m_originalDataOffset = m_elementDataOffset;

                if (-1 == numberOfBlocks)
                    numberOfBlocks = m_elementDataLength;

//                 char* output = (char*)malloc(0x100);
//                 sprintf_s(output, 0x99, "m_totalFileReadLength:%d, %d %d\n", m_totalFileReadLength, (size_t)element.fileStart, (size_t)element.fileLength);
//                 OutputDebugStringA(output);
//                 free(output);
            }
        }

        const size_t maxBufferLength = blockSize * numberOfBlocks;
        size_t readedLength = 0;

        RELEASE_ASSERT(!(0 == m_fileSize || m_elementDataOffset > m_fileSize || m_elementDataOffset + m_elementDataLength > m_fileSize));

        size_t needReadLength = m_elementDataLength;

        if (needReadLength > maxBufferLength)
            needReadLength = maxBufferLength;

        fseek(m_file, m_elementDataOffset, SEEK_SET);

        if (outBuf)
            outBuf->resize(needReadLength);
        if (!ptr && 0 != needReadLength)
            ptr = &outBuf->at(0);
        
        if (ptr && 0 != needReadLength)
            readedLength = fread(ptr, 1, needReadLength, m_file);
        m_allReadLength += readedLength;
        if (!readedLength || ferror(m_file)) {
            readFileFinish(L"FlattenHTTPBodyElementStream.ferror Fail:", element.filePath.c_str());
            // FIXME: show a user error?
            return 0;
        }

        bool isEnd = feof(m_file);
        if (isEnd || m_elementDataLength == readedLength) {
            char* output = (char*)malloc(0x100);
            sprintf(output, "isEnd: %d, m_elementDataLength: %d, readedLength:%d, element.fileLength:%d\n", 
                isEnd, m_elementDataLength, readedLength, (long)element.fileLength);
            OutputDebugStringA(output);
            free(output);

            RELEASE_ASSERT(readedLength <= maxBufferLength);
            RELEASE_ASSERT(m_elementDataOffset - m_originalDataOffset + readedLength == m_totalFileReadLength);
            RELEASE_ASSERT(m_elementDataLength - readedLength == 0);
            readFileFinish(nullptr, nullptr);
        } else {
            RELEASE_ASSERT(readedLength == maxBufferLength);
            m_elementDataOffset += readedLength;
            m_elementDataLength -= readedLength;
        }

        static int allSize = 0;
        allSize += readedLength;
        return readedLength;
    }

    void readFileFinish(const wchar_t* errorText1, const wchar_t* errorText2)
    {
        size_t elementIndex = m_elementIndex;
        reset();
        m_elementIndex = elementIndex + 1;

        if (errorText1) {
            std::wstring errorText = errorText1;
            errorText += errorText2;
            errorText += L"\n";
            OutputDebugStringW(errorText.c_str());
        }
    }

public:
    bool hasMoreElements() const
    {
        bool b = m_elementIndex < m_elements.size();
        if (!b) {
            RELEASE_ASSERT(m_estimateSize == m_allReadLength);
        }
        return b;
    }

    static String getPathBySystemURL(const blink::WebURL& fileSystemURL)
    {
        blink::KURL url = fileSystemURL;
        String urlString = url.getUTF8String();
        if (urlString.startsWith("file:///"))
            urlString.remove(0, 8);
        urlString.replace("/", "\\");
        return urlString;
    }

    static void clampSliceOffsets(long long size, long long* start, long long* length)
    {
        ASSERT(size != -1);

        // Convert the negative value that is used to select from the end.
        if (*start < 0)
            *start = *start + size;
        if (*length < 0)
            *length = size;

        // Clamp the range if it exceeds the size limit.
        if (*start < 0)
            *start = 0;

        if (*start >= size) {
            *start = 0;
            *length = 0;
        }

        if (*start + *length > size)
            *length = size - *start;
    }

    static void flatten(
        const blink::WebString& blobUUID,
        curl_off_t* size,
        WTF::Vector<FlattenHTTPBodyElement*>* flattenElements,
        long long parentOffset,
        long long parentLength,
        bool useParentSize,
        int deep
        )
    {
        //RELEASE_ASSERT(deep <= 2);

        FlattenHTTPBodyElement* flattenElement = nullptr;
        content::WebBlobRegistryImpl* blobReg = (content::WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
        net::BlobDataWrap* blobData = blobReg->getBlobDataFromUUID(blobUUID);
        if (!blobData)
            return;

        long long fileSizeResult = 0;
        const Vector<blink::WebBlobData::Item*>& items = blobData->items();

//         if (2 == deep && items.size() != 1)
//             RELEASE_ASSERT(false);

        for (size_t i = 0; i < items.size(); ++i) {
            blink::WebBlobData::Item* item = items[i];
            if (blink::WebBlobData::Item::TypeData == item->type) {
                
                long long offset = parentOffset;
                long long length = parentLength;
                if (!useParentSize || -1 == length) {
                    offset = item->offset;
                    length = item->length;
                }
                clampSliceOffsets(item->data.size(), &offset, &length);

                if (length <= 0)
                    continue;

                flattenElement = new FlattenHTTPBodyElement();
                flattenElement->type = FlattenHTTPBodyElement::Type::TypeData;

                flattenElement->data.append(item->data.data() + offset, length);
                flattenElements->append(flattenElement);

                *size += length;
            } else if (blink::WebBlobData::Item::TypeFile == item->type || blink::WebBlobData::Item::TypeFileSystemURL == item->type) {
                String filePath = item->filePath;
                if (blink::WebBlobData::Item::TypeFileSystemURL == item->type) {
                    blink::KURL fileSystemURL = item->fileSystemURL;
                    filePath = getPathBySystemURL(fileSystemURL);
                }

                long long offset = parentOffset;
                long long length = parentLength;
                if (!useParentSize || -1 == length) {
                    offset = item->offset;
                    length = item->length;
                }

                if (-1 != filePath.find("miniblink_blob_download_")) {
                    BlobTempFileInfo* info = DownloadFileBlobCache::inst()->getBlobTempFileInfoByTempFilePath(filePath);
                    RELEASE_ASSERT(info);

                    fileSizeResult = info->data.size();
                    if (offset > fileSizeResult)
                        offset = fileSizeResult;

                    char* output = (char*)malloc(0x100);
                    sprintf(output, "flatten blob: fileSizeResult:%lld, offset:%lld, parentOffset:%lld, parentLength:%lld\n", 
                            fileSizeResult, offset, parentOffset, parentLength);
                    OutputDebugStringA(output);
                    free(output);

                    flattenElement = new FlattenHTTPBodyElement();
                    flattenElement->type = FlattenHTTPBodyElement::Type::TypeData;

                    long long appendSize = fileSizeResult - offset;

                    flattenElement->data.append(info->data.data() + offset, appendSize);
                    flattenElements->append(flattenElement);

                    *size += appendSize;
                } else if (getFileSize(filePath, fileSizeResult)) {
                    clampSliceOffsets(fileSizeResult, &offset, &length);
                    if (length <= 0)
                        continue;

                    *size += length;

                    flattenElement = new FlattenHTTPBodyElement();
                    flattenElement->type = FlattenHTTPBodyElement::Type::TypeFile;
                    Vector<UChar> filePathBuf = WTF::ensureUTF16UChar(filePath, true);
                    flattenElement->filePath = filePathBuf.data();
                    flattenElement->fileStart = offset; // item->offset;
                    flattenElement->fileLength = length; // item->length;
                    flattenElements->append(flattenElement);
                }
            } else if (blink::WebBlobData::Item::TypeBlob == item->type) {
                //OutputDebugStringA("FlattenHTTPBodyElementStream::flatten, TypeBlob\n");
                flatten(item->blobUUID, size, flattenElements, item->offset, item->length, true, deep + 1);
            }
        }
    }

private:
    FILE* m_file;
    size_t m_fileSize;
    size_t m_elementIndex;
    size_t m_elementDataOffset;
    size_t m_elementDataLength;
    size_t m_totalFileReadLength;
    size_t m_allReadLength;
    size_t m_originalDataOffset;
    curl_off_t m_estimateSize;
    WTF::Vector<FlattenHTTPBodyElement*> m_elements;
};

}

#endif // net_FlattenHTTPBodyElement_h