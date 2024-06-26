
#include "media/SimpleDataSource.h"

#include "media/BufferedConstant.h"
#include "content/browser/PostTaskHelper.h"
#include "net/ActivatingObjCheck.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"

#include <memory>

namespace media {

const __int32 kPosNotSpecified = -1;
static const size_t kPreloadSize = 1024 * 1024 * 3;

std::vector<char>* g_buffer = nullptr;

unsigned long lh_strhash(char* str, int len)
{
    unsigned long ret = 0;

    for (int i = 0; i < len; ++i) {
        ret += str[i];
    }
    return ret;
}

void checkBuf(int position, size_t size, const char* data)
{
    char* ptr = &g_buffer->at(0);
    char* ptr2 = ptr + position;
    if (0 != memcmp(ptr2, data, size)) {
        for (size_t i = 0; i < g_buffer->size(); ++i) {
            char* ptr3 = &g_buffer->at(i);
            if (0 == memcmp(ptr3, data, size)) {
                OutputDebugStringA("checkBuf fail\n");
                DebugBreak();
            }
        }
        DebugBreak();
    }
}

void checkFileBuf(int position, size_t size, const char* data)
{
    return;

    FILE* file = fopen("G:\\test\\web_test\\niushibang\\mov.mp4", "rb+");

    char* checkBuffer = (char*)malloc(size);
    fseek(file, position, SEEK_SET);
    fread(checkBuffer, size, 1, file);

    for (size_t i = 0; i < size; ++i) {
        size_t xx = position + i;
        if (checkBuffer[i] != *(data + i))
            DebugBreak();
    }
    free(checkBuffer);

    fclose(file);
}

bool stringToInt64(const std::string& input, int64* output);

static int s_packetCount = 0;

class SeekableBuf {
public:
    SeekableBuf()
    {
        m_totalSize = 0;
    }

    ~SeekableBuf()
    {
        clear();
    }

private:
    static const size_t kReserveaSize = 1024 * 1024 * 3;
    struct Packet {
        Packet()
        {
            pos = 0;
            s_packetCount++;
            buffer.reserve(kReserveaSize); // 3m的缓存
        }

        ~Packet()
        {
            s_packetCount--;
        }

        size_t size()
        {
            return buffer.size();
        }

        size_t rightPos()
        {
            size_t right = pos + buffer.size();
            ASSERT(m_rightPos == right);
            return right;
        }

        size_t maxPos()
        {
            return pos + kReserveaSize;
        }

        void toMaxResize()
        {
            buffer.resize(kReserveaSize);
        }

        char* data()
        {
            return buffer.data();
        }

        void resize(size_t size)
        {
            buffer.resize(size);
            m_rightPos = pos + buffer.size();
        }

        size_t pos;
        std::vector<char> buffer;

    private:
        size_t m_rightPos;
    };

private:
    // 最后个参数如果为true，表示相连也算相交
    static bool isIntersect(size_t posA, size_t lenA, size_t posB, size_t lenB, bool isEdeg)
    {
        if (isEdeg) {
            if (isJoined(posA, lenA, posB, lenB))
                return true;
        }

        // A     ----
        // B ====    ====
        if (posA + lenA <= posB || posB + lenB <= posA)
            return false;

        return true;
    }

    // 如果是相连
    static bool isJoined(size_t posA, size_t lenA, size_t posB, size_t lenB)
    {
        if (posA + lenA== posB || posB + lenB == posA)
            return true;
        return false;
    }

    static Packet* mergePacket(Packet* packet, size_t pos, const char* data, size_t len)
    {
        ASSERT(isIntersect(packet->pos, packet->size(), pos, len, true));
        size_t leftPos = packet->pos < pos ? packet->pos : pos;
        size_t rightPos = packet->rightPos();
        if (rightPos < pos + len)
            rightPos = pos + len;

        if (packet->pos > pos) {
            Packet* newPacket = new Packet();
            newPacket->pos = leftPos;
            newPacket->resize(rightPos - leftPos);
            memcpy(newPacket->data() + (packet->pos - leftPos), packet->data(), packet->size());
            memcpy(newPacket->data() + (pos - leftPos), data, len);

            delete packet;
            return newPacket;
        }
        packet->resize(rightPos - leftPos);
        memcpy(packet->data() + (pos - leftPos), data, len);
        return packet;
    }

    static Packet* mergeTwoPacket(Packet* packetA, Packet* packetB)
    {
        ASSERT(packetA->pos <= packetB->pos);
        Packet* newPacket = mergePacket(packetA, packetB->pos, packetB->data(), packetB->size());

        delete packetB;
        return newPacket;
    }
private:
    static bool isContain(size_t posA, size_t lenA, size_t posB, size_t lenB)
    {
        if (posA <= posB && posA + lenA >= posB + lenB)
            return true;
        return false;
    }

public:
    void append(const size_t pos, const char* data, const size_t len)
    {
        WTF::Locker<WTF::RecursiveMutex> autoLock(m_lock);

        size_t itPos = pos;
        const char* itData = data;
        size_t itLen = len;

        bool needLoop = false;
        bool hasIntersect = false;
        do {
            needLoop = false;
            hasIntersect = false;

            if (0 == itLen || itPos >= pos + len)
                break;

            for (size_t i = 0; i < m_buffer.size(); ++i) {
                Packet* packet = m_buffer[i];

                if (isContain(packet->pos, packet->size(), itPos, itLen)) // 如果包含关系，不用搞了
                    return;                

                if (!isIntersect(packet->pos, packet->size(), itPos, itLen, true)) { // 如果不相交
                    if (itPos + itLen <= m_buffer[i]->pos) {
                        // 如果在本packet左侧
                        insertNew(itPos, itData, itLen, i - 1);
                        checkIntersect(pos, data, len, 0);
                        return;
                    }

                    if (i != m_buffer.size() - 1) { 
                        if (!isIntersect(m_buffer[i + 1]->pos, m_buffer[i + 1]->size(), itPos, itLen, false)) { // 判断是否和下一个相交
                            if (itPos + itLen <= m_buffer[i + 1]->pos) {
                                // 如果和下一个不相交，且在下个的左侧，则new然后插入中间
                                insertNew(itPos, itData, itLen, i);
                                checkIntersect(pos, data, len, 1);
                                return;
                            } else // 如果和下一个不相交，且在下个的右侧，则下个循环处理
                                continue;                            
                        } else
                            continue; // 如果和本packet不相交，但和下一个相交，则交给下次循环处理
                    } else { // 如果没有下一个，直接new然后插到末尾
                        insertNew(itPos, itData, itLen, i);
                        checkIntersect(pos, data, len, 2);
                        return;
                    }
                    continue;
                }
                hasIntersect = true;

                if (itPos < packet->pos) { // 先处理左边超出的部分
                    size_t det = packet->pos - itPos;
                    insertNew(itPos, itData, det, i - 1);
                    itData = itData + (det);
                    itPos = packet->pos;
                    itLen = itLen - (det);

                    needLoop = true;
                    break;
                }

                if (itPos + itLen > packet->rightPos()) { // 如果右边超出了
                    size_t newSize = kReserveaSize;
                    if (i != m_buffer.size() - 1 && packet->pos + newSize > m_buffer[i + 1]->pos) { // 如果最大容量顶到下一个packet的左边缘
                        newSize = m_buffer[i + 1]->pos - packet->pos;
                    }

                    size_t oldSize = packet->size();
                    size_t oldReghtPos = packet->rightPos();
                    if (newSize > itPos + itLen - packet->pos) {
                        newSize = itPos + itLen - packet->pos;
                    }

                    if (kReserveaSize <= oldSize) { // 如果packet已经撑到最大了
                        if (itPos < oldReghtPos) { // 如果相交
                            RELEASE_ASSERT(itPos + itLen > oldReghtPos);
                            itLen -= oldReghtPos - itPos;
                            itData += oldReghtPos - itPos;
                            itPos = oldReghtPos;
                        }

                        if (i == m_buffer.size() - 1) { // 如果已经是最后一个，就直接插在末尾了
                            insertNew(itPos, itData, itLen, i);
                            checkIntersect(pos, data, len, 2);
                            return;
                        }
                        needLoop = false;
                        hasIntersect = false;
                        continue;
                    }
                    if (i != m_buffer.size() - 1 && packet->rightPos() >= m_buffer[i + 1]->pos) { // 如果本包和下个包是紧挨着，表示扩展不动了
                        ASSERT(packet->rightPos() == m_buffer[i + 1]->pos);
                        needLoop = false;
                        hasIntersect = false;
                        continue;
                    }

                    packet->resize(newSize);
                    memcpy(packet->data() + oldSize, itData + (oldReghtPos - itPos), newSize - oldSize);

                    size_t detLen = packet->rightPos() - itPos;
                    itData += detLen;
                    itPos = packet->rightPos();
                    itLen -= detLen; // newSize - oldSize

                    needLoop = (itPos < pos + len);
                    if (needLoop) {
                        ASSERT(itLen > 0);
                    }
                    break;
                }
            }
        } while (needLoop);    

        if (itPos < pos + len && !hasIntersect) { // 如果还有部分数据没和任何packet相交
            insertNew(itPos, itData, itLen, -1);
        }

        checkIntersect(pos, data, len, 3);
    }

private:
    void checkIntersect(size_t pos, const char* data, size_t len, int test)
    {
//         std::string outputStr = "SeekableBuf::append, ";
//         std::vector<char> output;
//         output.resize(0x100);
// 
//         sprintf(output.data(), "pos:%d, len:%d, right:%d, m_buffer.size:%d, %d\n", pos, len, pos + len, m_buffer.size(), test);
//         outputStr += output.data();
// 
//         for (size_t i = 0; i < m_buffer.size(); ++i) {
//             Packet* packet = m_buffer[i];
// 
//             sprintf(output.data(), "    pos:%d, rightPos:%d, size:%d\n", packet->pos, packet->rightPos(), packet->size());
//             outputStr += output.data();
//         }
// 
//         OutputDebugStringA(outputStr.c_str());

        //FILE* file = fopen("G:\\test\\web_test\\niushibang\\mov.mp4", "rb+");

        // 检查这个包和其他包有没位置重叠
        for (size_t i = 0; i < m_buffer.size(); ++i) {
            Packet* packet = m_buffer[i];
            if (packet->size() == 0)
                DebugBreak();
            for (size_t j = i + 1; j < m_buffer.size(); ++j) {
                Packet* packetB = m_buffer[j];

                if (isIntersect(packet->pos, packet->size(), packetB->pos, packetB->size(), false))
                    DebugBreak();
            }

//             char* checkBuffer = (char*)malloc(packet->size());
//             fseek(file, packet->pos, SEEK_SET);
//             fread(checkBuffer, packet->size(), 1, file);
// 
//             for (size_t j = 0; j < packet->size() && j < 10; ++j) {
//                 const char* ptr = packet->data() + j;
//                 if (checkBuffer[j] != *(ptr))
//                     DebugBreak();
//             }
//             free(checkBuffer);
        }
        //fclose(file);
    }

    // 把左边的部分剪下来，做成新的packet
    void insertNew(size_t pos, const char* data, size_t len, size_t index)
    {
        size_t itPos = pos;
        const char* itData = data;
        size_t itLen = len;

        while (itLen > 0) {
            size_t packetSize = kReserveaSize;
            if (packetSize > itLen)
                packetSize = itLen;

            Packet* newPacket = new Packet();
            newPacket->pos = itPos;
            newPacket->resize(packetSize);
            memcpy(newPacket->data(), itData, packetSize);

            ++index;
            m_buffer.insert(m_buffer.begin() + index, newPacket);

            itPos += packetSize;
            itData += packetSize;
            itLen -= packetSize;
        }
    }

public:
    void clear()
    {
        for (size_t i = 0; i < m_buffer.size(); ++i) {
            Packet* packet = m_buffer[i];
            delete packet;
        }
        m_buffer.clear();
    }

private:

    static int copyIntersect(size_t posA, size_t lenA, const char* dataA, size_t posB, size_t lenB, char* dataB, size_t* readedSize)
    {
        size_t maxA = posA + lenA;
        size_t maxB = posB + lenB;

        if (posA <= posB && maxA >= maxB) {
            //  A: ---------
            //  B:    ====
            *readedSize = lenB;
            memcpy(dataB, dataA + posB - posA, *readedSize);
            return 4;
        } else if (posA < posB && maxA >= posB && maxA <= maxB) {
            //  A:-----
            //  B:   ====
            *readedSize = lenA - (posB - posA);
            memcpy(dataB, dataA + (posB - posA), *readedSize);
            return 1;
        } else if (posA >= posB && posA <= maxB && maxA >= maxB) {
            //  A:    -----
            //  B: ======
            *readedSize = lenB - (posA - posB);
            memcpy(dataB + (posA - posB), dataA, *readedSize);
            return 2;
        } else if (posA >= posB && posA <= maxB && maxA <= maxB) {
            //  A:    -----
            //  B: ===========
            *readedSize = lenA;
            memcpy(dataB + (posA - posB), dataA, *readedSize);
            return 3;
        }
        *readedSize = 0;
        return 5;
    }

public:
    bool read(size_t pos, char* data, size_t len)
    {
        WTF::Locker<WTF::RecursiveMutex> autoLock(m_lock);
        int readCount = 0;
        size_t allReadedSize = 0;
        for (size_t i = 0; i < m_buffer.size(); ++i) {
            readCount++;
            Packet* packet = m_buffer[i];
            size_t readedSize;
            int result = copyIntersect(packet->pos, packet->size(), packet->data(), pos, len, data, &readedSize);
            allReadedSize += readedSize;
            if (1 == result) {
                if (i == m_buffer.size() - 1)
                    return false;
                else {
                    Packet* packetNext = m_buffer[i + 1];
                    if (packetNext->pos > packet->rightPos() + 1) // 如果中间有缝隙，则读取失败
                        return false;
                }
            } else if (2 == result || 3 == result) {
                if (0 == readCount)
                    return false;
            } else if (4 == result) {
                ASSERT(allReadedSize == len);
                return true;
            }
        }
        return allReadedSize == len;
    }

    void evicteMemory(size_t pos)
    {
        WTF::Locker<WTF::RecursiveMutex> autoLock(m_lock);

        if (m_buffer.size() < 5) // 太少了就不用裁剪了
            return;

        Packet* newPacket = nullptr;
        for (size_t i = 0; i < m_buffer.size(); ++i) {
            Packet* packet = m_buffer[i];
            if (!(packet->pos <= pos && packet->rightPos() > pos))
                continue;

            newPacket = packet; // 找到pos对应的包，然后下面把其他包都删了
            break;
        }

        if (!newPacket)
            return;
        for (size_t i = 0; i < m_buffer.size(); ++i) {
            Packet* packet = m_buffer[i];
            if (packet != newPacket)
                delete packet;
        }
        m_buffer.clear();
        m_buffer.push_back(newPacket); // 把其他的包都删了，只保留当前这个

//         char* output = (char*)malloc(0x100);
//         sprintf(output, "SeekableBuf ::evicteMemory: %d %d\n", newPacket->pos, newPacket->rightPos());
//         OutputDebugStringA(output);
//         free(output);
    }

private:
    static const size_t kMaxMemoryUsage = 5 * 1024 * 1014; // 5M
    static const size_t kMaxEvictedMemoryUsage = 2 * 1024 * 1014; // 被裁剪后的内存占用

    std::vector<Packet*> m_buffer;
    size_t m_totalSize;

    WTF::RecursiveMutex m_lock;
};

class LoaderWrap {
public:
    LoaderWrap(blink::WebURLLoader* loader)
        : m_loader(loader),
        m_deferred(false)
    {
    }

    ~LoaderWrap()
    {
        m_loader->cancel();
    }

    void setDeferred(bool deferred)
    {
        m_deferred = deferred;
        m_loader->setDefersLoading(deferred);
    }

    bool deferred() {
        return m_deferred;
    }

    void stop()
    {
        m_loader->cancel();
    }

private:
    std::unique_ptr<blink::WebURLLoader> m_loader;
    bool m_deferred;
};

class SimpleResourceLoader : public blink::WebURLLoaderClient {
public:
    enum DeferStrategy {
        kNeverDefer,
        kReadThenDefer,
        kCapacityDefer,
    };

    SimpleResourceLoader(
        const blink::KURL& url,
        blink::WebMediaPlayer::CORSMode corsMode,
        size_t firstBytePosition,
        size_t lastBytePosition,
        DeferStrategy strategy,
        int bitrate,
        double playbackRate,
        SimpleDataSource* dataSource
    )
    {
        m_firstBytePosition = firstBytePosition;
        m_lastBytePosition = lastBytePosition;
        m_url = url;
        m_dataSource = dataSource;
        m_corsMode = corsMode;
        m_instanceSize = kPosNotSpecified; // 暂时不知道这玩意是干嘛的
        m_contentLength = kPosNotSpecified;
        m_recvDataLength = 0;
        m_rangeSsupported = false;

//         char* output = (char*)malloc(0x100);
//         sprintf(output, "SimpleResourceLoader:: this:%p, dataSource:%p, m_firstBytePosition:%d, m_lastBytePosition:%d\n", this, dataSource, m_firstBytePosition, m_lastBytePosition);
//         OutputDebugStringA(output);
//         free(output);
    }

    ~SimpleResourceLoader()
    {

    }

    size_t getFirstBytePosition() const { return m_firstBytePosition; }
    size_t getLastBytePosition() const { return m_lastBytePosition; }

    // blink::WebURLLoaderClient implementation.
    void willSendRequest(blink::WebURLLoader* loader, blink::WebURLRequest& newRequest, const blink::WebURLResponse& redirectResponse) override
    {

    }

    void didSendData(blink::WebURLLoader* loader, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override
    {

    }

    void didReceiveResponse(blink::WebURLLoader* loader, const blink::WebURLResponse& response) override
    {
        //OutputDebugStringA("SimpleResourceLoader.didReceiveResponse\n");

        m_contentLength = response.expectedContentLength();

        if (m_url.protocolIsInHTTPFamily()) {
            bool partial_response = (response.httpStatusCode() == kHttpPartialContent);
            bool okResponse = (response.httpStatusCode() == kHttpOK);

            if (isRangeRequest()) {
                // Check to see whether the server supports byte ranges.
                std::string accept_ranges = response.httpHeaderField("Accept-Ranges").utf8();
                m_rangeSsupported = (accept_ranges.find("bytes") != std::string::npos);

                // If we have verified the partial response and it is correct, we will
                // return kOk. It's also possible for a server to support range requests
                // without advertising "Accept-Ranges: bytes".
                if (partial_response && verifyPartialResponse(response)) {
                    m_rangeSsupported = true;
                } else if (okResponse && m_firstBytePosition == 0 && m_lastBytePosition == kPosNotSpecified) {
                    // We accept a 200 response for a Range:0- request, trusting the
                    // Accept-Ranges header, because Apache thinks that's a reasonable thing
                    // to return.
                    m_instanceSize = m_contentLength;
                } else if (!okResponse) {
                    m_dataSource->onLoaderResponse(false);
                    return;
                }
            } else {
                m_instanceSize = m_contentLength;
                if (response.httpStatusCode() != kHttpOK) {
                    // We didn't request a range but server didn't reply with "200 OK".
                    m_dataSource->onLoaderResponse(false);
                    return;
                }
            }
        } else {
            ASSERT(m_instanceSize == kPosNotSpecified);
            if (m_contentLength != kPosNotSpecified) {
                if (m_firstBytePosition == kPosNotSpecified)
                    m_instanceSize = m_contentLength;
                else if (m_lastBytePosition == kPosNotSpecified)
                    m_instanceSize = m_contentLength + m_firstBytePosition;
            }

            if (0 == m_contentLength) { // 文件等协议，httpStatusCode被curl传过来会是0，所以只能依靠长度来判断是否失败
                m_dataSource->onLoaderResponse(false);
                return;
            }
        }

        m_dataSource->onLoaderResponse(true);
    }

    void didDownloadData(blink::WebURLLoader* loader, int dataLength, int encodedDataLength) override
    {

    }

    void didReceiveData(blink::WebURLLoader* loader, const char* data, int dataLength, int encodedDataLength) override
    {
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "SimpleResourceLoader.didReceiveData: m_recvDataLength:%d, dataLength:%d\n", m_recvDataLength, dataLength);
//         OutputDebugStringA(output);
//         free(output);

        m_dataSource->onLoaderDataRecv(data, dataLength);
        m_recvDataLength += dataLength;
    }

    void didReceiveCachedMetadata(blink::WebURLLoader* loader, const char* data, int dataLength) override
    {

    }

    void didFinishLoading(blink::WebURLLoader* loader, double finishTime, int64_t totalEncodedDataLength) override
    {
        //OutputDebugStringA("SimpleResourceLoader.didFinishLoading\n");
        m_dataSource->onLoaderFinish(m_recvDataLength);
    }

    void didFail(blink::WebURLLoader* loader, const blink::WebURLError&) override
    {
        //m_dataSource->onLoaderStart(false);
        // 当response失败的时候不会走到本回调
        OutputDebugStringA("SimpleResourceLoader::didFail\n");
    }

    void start(blink::WebFrame* frame)
    {
        blink::WebURLRequest request(m_url);
        request.setRequestContext(blink::WebURLRequest::RequestContextVideo); // TODO(mkwst): Split this into video/audio.

        if (isRangeRequest()) {
            std::vector<char> httpByteRange;
            httpByteRange.resize(300);

            if (m_lastBytePosition == kPosNotSpecified)
                sprintf(httpByteRange.data(), "bytes=%d-", m_firstBytePosition);
            else
                sprintf(httpByteRange.data(), "bytes=%d-%d", m_firstBytePosition, m_lastBytePosition);

            request.setHTTPHeaderField(blink::WebString::fromUTF8("Range"), blink::WebString::fromUTF8(httpByteRange.data()));

//             OutputDebugStringA("SimpleResourceLoader.start, ");
//             OutputDebugStringA(httpByteRange.data());
//             OutputDebugStringA("\n");
        } else {
            //OutputDebugStringA("SimpleResourceLoader.start, isRangeRequest fail\n");
        }

        frame->setReferrerForRequest(request, blink::WebURL());

        // Disable compression, compression for audio/video doesn't make sense...
        request.setHTTPHeaderField(blink::WebString::fromUTF8("Accept-Encoding"), blink::WebString::fromUTF8("identity;q=1, *;q=0"));

        blink::WebURLLoaderOptions options;
        if (m_corsMode == blink::WebMediaPlayer::CORSModeUnspecified) {
            options.allowCredentials = true;
            options.crossOriginRequestPolicy = blink::WebURLLoaderOptions::CrossOriginRequestPolicyAllow;
        } else {
            options.exposeAllResponseHeaders = true;
            // The author header set is empty, no preflight should go ahead.
            options.preflightPolicy = blink::WebURLLoaderOptions::PreventPreflight;
            options.crossOriginRequestPolicy = blink::WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
            if (m_corsMode == blink::WebMediaPlayer::CORSModeUseCredentials)
                options.allowCredentials = true;
        }

        blink::WebURLLoader* loader = (frame->createAssociatedURLLoader(options));
        loader->loadAsynchronously(request, this);
        m_activeLoader.reset(new LoaderWrap(loader));
        m_dataSource->loadingStateChangedCallback(SimpleDataSource::kLoading);
    }

    void stop()
    {
        m_activeLoader->stop();
    }

    bool isRangeRequest() const
    {
        return m_firstBytePosition != kPosNotSpecified;
    }

    size_t getContentLength() const
    {
        return m_contentLength;
    }

private:
    static bool parseContentRange(const std::string& content_range_str, int64* firstBytePosition,
        int64* lastBytePosition, int64* instanceSize)
    {
        const std::string kUpThroughBytesUnit = "bytes ";
        if (content_range_str.find(kUpThroughBytesUnit) != 0)
            return false;
        std::string range_spec = content_range_str.substr(kUpThroughBytesUnit.length());
        size_t dash_offset = range_spec.find("-");
        size_t slash_offset = range_spec.find("/");

        if (dash_offset == std::string::npos || slash_offset == std::string::npos ||
            slash_offset < dash_offset || slash_offset + 1 == range_spec.length()) {
            return false;
        }
        if (!stringToInt64(range_spec.substr(0, dash_offset), firstBytePosition) ||
            !stringToInt64(range_spec.substr(dash_offset + 1, slash_offset - dash_offset - 1),
                           lastBytePosition)) {
            return false;
        }
        if (slash_offset == range_spec.length() - 2 && range_spec[slash_offset + 1] == '*') {
            *instanceSize = kPosNotSpecified;
        } else {
            if (!stringToInt64(range_spec.substr(slash_offset + 1), instanceSize)) {
                return false;
            }
        }
        if (*lastBytePosition < *firstBytePosition || (*instanceSize != kPosNotSpecified && *lastBytePosition >= *instanceSize)) {
            return false;
        }

        return true;
    }

    bool verifyPartialResponse(const blink::WebURLResponse& response)
    {
        int64 firstBytePosition, lastBytePosition, instanceSize;
        if (!parseContentRange(response.httpHeaderField("Content-Range").utf8(), &firstBytePosition, &lastBytePosition, &instanceSize)) {
            return false;
        }

        if (instanceSize != kPosNotSpecified) {
            m_instanceSize = (size_t)instanceSize;
        }

        if (m_firstBytePosition != kPosNotSpecified &&
            m_firstBytePosition != firstBytePosition) {
            return false;
        }

        // TODO(hclam): I should also check |last_byte_position|, but since
        // we will never make such a request that it is ok to leave it unimplemented.
        return true;
    }

private:
    blink::WebMediaPlayer::CORSMode m_corsMode;
    SimpleDataSource* m_dataSource;
    blink::KURL m_url;

    size_t m_firstBytePosition;
    size_t m_lastBytePosition;

    size_t m_instanceSize;
    size_t m_contentLength;

    size_t m_recvDataLength; // recv数据长度

    bool m_rangeSsupported;

    std::unique_ptr<LoaderWrap> m_activeLoader;
};

//////////////////////////////////////////////////////////////////////////

SimpleDataSource::SimpleDataSource(
    const blink::KURL& url,
    blink::WebMediaPlayer::CORSMode corsMode,
    blink::WebFrame* frame,
    MediaLog* media_log,
    /*BufferedDataSourceHost*/void* host,
    wke::DataSource::DownloadingCB* downloadingCb)
{
    ASSERT(m_downloadingCb);
    m_mediaHasPlayed = false;
    m_initCb = nullptr;
    m_readCb = nullptr;
    m_url = url;
    m_playbackRate = 0.0;
    m_frame = frame;
    m_readBeginPos = 0;
    m_readEndPos = 100 * 32 * 1024; // 本来想先读他32k，但发现如果range字段写了会导致获取的content-length也是这值
    m_offset = 0;
    m_totalBytes = kPosNotSpecified;
    m_paddingReadSize = 0;
    m_paddingReadData = nullptr;
    //m_isPreloading = false;
    ASSERT(m_frame);

    m_seekableBuf.reset(new SeekableBuf());
    m_testSeekableBuf.reset(new SeekableBuf());
    m_downloadingCb = downloadingCb;

    m_id = net::ActivatingObjCheck::inst()->genId();
    net::ActivatingObjCheck::inst()->add(m_id);
}

SimpleDataSource::~SimpleDataSource()
{
    ASSERT(WTF::isMainThread());
    abort();
    net::ActivatingObjCheck::inst()->remove(m_id);
}

void SimpleDataSource::willDestroyed()
{
    m_frame = nullptr;
}

void SimpleDataSource::destroy()
{
    m_readLock.lock();
    if (m_loader.get())
        m_loader->stop();
    m_readLock.unlock();

    delete this;
}

SimpleResourceLoader* SimpleDataSource::createResourceLoader(size_t firstBytePosition, size_t lastBytePosition)
{
    ASSERT(WTF::isMainThread());

    SimpleResourceLoader::DeferStrategy strategy = m_preload == METADATA ?
        SimpleResourceLoader::DeferStrategy::kReadThenDefer :
        SimpleResourceLoader::DeferStrategy::kCapacityDefer;

    return new SimpleResourceLoader(m_url, m_corsMode, firstBytePosition, lastBytePosition,
                                    strategy, m_bitrate, m_playbackRate, this);
}

void SimpleDataSource::initialize(wke::DataSource::InitializeCB* initCb)
{
    ASSERT(WTF::isMainThread());
    ASSERT(initCb);
    ASSERT(!m_loader.get());
    if (!m_frame)
        return;

    m_initCb = initCb;

    // 先预读一次
    m_loader.reset(createResourceLoader(m_readBeginPos, kPosNotSpecified));
    m_loader->start(m_frame);
}

void SimpleDataSource::setPreload(wke::DataSource::Preload preload)
{
    ASSERT(WTF::isMainThread());
    m_preload = preload;
}

bool SimpleDataSource::hasSingleOrigin()
{
    return true;
}

bool SimpleDataSource::didPassCORSAccessCheck() const
{
    return true;
}

void SimpleDataSource::abort()
{

}

void SimpleDataSource::mediaPlaybackRateChanged(double playback_rate)
{

}
void SimpleDataSource::mediaIsPlaying()
{

}

void SimpleDataSource::mediaIsPaused()
{

}

void SimpleDataSource::onBufferingHaveEnough()
{

}

int64_t SimpleDataSource::getMemoryUsage() const
{
    return 0;
}

void SimpleDataSource::stop()
{
    if (m_loader.get())
        m_loader->stop();
}

void SimpleDataSource::notifyFinishCallback(bool ok)
{
    WTF::Locker<WTF::RecursiveMutex> autoLock(m_readLock);
    SimpleDataSource* self = this;
    int id = m_id;
    wke::DataSource::InitializeCB* initCb = m_initCb;
    m_initCb = nullptr;

    //m_isPreloading = false;
    m_loader.reset();

    if (initCb) {
        ASSERT(!m_readCb);
        content::postTaskToMainThread(FROM_HERE, [self, id, initCb, ok] {
            if (net::ActivatingObjCheck::inst()->isActivating(id))
                initCb->notifyInitialize(ok);
        });
    }

    if (!ok && m_readCb) {
        m_paddingReadData = nullptr;
        m_paddingReadSize = 0;
        m_readCb->signalReadCompleted(-1);
    }
}

void SimpleDataSource::onLoaderFinish(size_t size)
{
    WTF::Locker<WTF::RecursiveMutex> autoLock(m_readLock);
    ASSERT(WTF::isMainThread());
    notifyFinishCallback(true);

    //OutputDebugStringA("SimpleDataSource::onLoaderFinish\n");

    // 有可能onLoaderDataRecv的时候已经完成读取回调了，所以下面代码很小概率执行到
    // 但如果在recv完了后，又有外部读，此时会执行到下面
    if (m_readCb && m_paddingReadData) {
        ASSERT(m_paddingReadData && 0 != m_paddingReadSize);
        bool b = m_seekableBuf->read(m_readBeginPos, m_paddingReadData, m_paddingReadSize);
        if (!b)
            DebugBreak();
        ASSERT(b && m_readEndPos == m_readBeginPos + m_paddingReadSize - 1);

        checkFileBuf(m_readBeginPos, m_paddingReadSize, m_paddingReadData);

        size_t readSize = size > m_paddingReadSize ? m_paddingReadSize : size;
        m_paddingReadData = nullptr;
        m_paddingReadSize = 0;
        m_readCb->signalReadCompleted(b ? readSize : -1); // 有时候size比m_paddingReadSize大一点
    }

    m_seekableBuf->evicteMemory(m_readBeginPos);
    //m_testSeekableBuf->evicteMemory(m_readBeginPos);

    m_readCb = nullptr;
}

void SimpleDataSource::onLoaderDataRecv(const char* data, size_t size)
{
    WTF::Locker<WTF::RecursiveMutex> autoLock(m_readLock);
#if 0
    size_t temtSize = 546209;
    void* tempBuf = malloc(temtSize);
    m_seekableBuf->append(41178069, (const char*)tempBuf, temtSize);
    free(tempBuf);

    m_seekableBuf->clear();
#endif

//     static int s_count = 0;
//     s_count++;
//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "SimpleDataSource::onLoaderDataRecv, this:%p, m_offset:%d, size:%d, %d\n", this, m_offset, size, s_count);
//     OutputDebugStringA(output);
//     free(output);

    size_t offset = m_offset;
    
    // 这里m_offset可能会超出m_readEndPos。原因貌似是因为curl会偶尔多派发些网络数据，比range规定的大一些
    m_seekableBuf->append(m_offset, data, size);
    m_offset += size;

    // 如果是第一波网络请求，由于是发送了个获取全部文件的请求，为了避免有时候视频太大，所以主动中断网络请求
    if (m_initCb && m_offset > m_readEndPos) {
        m_loader->stop();
        notifyFinishCallback(true);
    }

    // 如果数据已经足够
    if (m_readCb && m_seekableBuf->read(m_readBeginPos, m_paddingReadData, m_paddingReadSize)) {
        ASSERT(m_readEndPos == m_readBeginPos + m_paddingReadSize - 1);

//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "SimpleDataSource::onLoaderDataRecv finish, m_readBeginPos:%d, m_readEndPos:%d, m_paddingReadSize:%d\n", m_readBeginPos, m_readEndPos, m_paddingReadSize);
//         OutputDebugStringA(output);
//         free(output);

        checkFileBuf(m_readBeginPos, m_paddingReadSize, m_paddingReadData);

        m_paddingReadData = nullptr;
        m_readCb->signalReadCompleted(m_paddingReadSize);
        m_paddingReadSize = 0;
        m_readCb = nullptr;
    }

    //m_testSeekableBuf->append(offset, data, size);
}

void SimpleDataSource::onLoaderResponse(bool isSuccess)
{
    WTF::Locker<WTF::RecursiveMutex> autoLock(m_readLock);
    if (isSuccess && kPosNotSpecified == m_totalBytes)
        m_totalBytes = m_loader->getContentLength();
    else if (!isSuccess)
        notifyFinishCallback(false);
}

void SimpleDataSource::read(int64 position, int size, char* data, wke::DataSource::ReadCB* readCb)
{
    //ASSERT(!m_loader.get()); // 必须是在预读完以后，回调init后，才能执行本函数
    ASSERT(0 == m_paddingReadSize);
    ASSERT(!m_paddingReadData);
    ASSERT(size > 0);

//     char* output = (char*)malloc(0x300);
//     sprintf_s(output, 0x299, "read--! this:%p,position:%d, size:%d\n", this, (int)position, size);
//     OutputDebugStringA(output);
//     free(output);

    WTF::Locker<WTF::RecursiveMutex> autoLock(m_readLock);

    if (position >= m_totalBytes || size <= 0) {
        DebugBreak();
        m_paddingReadSize = 0;
        m_paddingReadData = nullptr;
        readCb->signalReadCompleted(-1);
    }

    if (position >= m_totalBytes - 1) {
        m_paddingReadSize = 0;
        m_paddingReadData = nullptr;
        readCb->signalReadCompleted(-1);
        return;
    }

    int fixSize = size;
    if (position + fixSize > m_totalBytes)
        fixSize = m_totalBytes - position;
    if (fixSize <= 0) {
        m_paddingReadSize = 0;
        m_paddingReadData = nullptr;
        readCb->signalReadCompleted(-1);
        return;
    }

    if (m_seekableBuf->read((int)position, data, fixSize)) {
        //OutputDebugStringA("SimpleDataSource.read enough\n");
        checkFileBuf((int)position, fixSize, data);
        m_paddingReadSize = 0;
        m_paddingReadData = nullptr;
        readCb->signalReadCompleted(fixSize);
        return;
    }

    // 如果缓存里没有，就发送网络请求
    int netFetchSize = fixSize * 5; // 走网络请求的时候每次多读一点
    if (position + netFetchSize > m_totalBytes)
        netFetchSize = m_totalBytes - position;

    m_paddingReadSize = fixSize;
    m_paddingReadData = data;
    m_readCb = nullptr; // 放到main thread去设置，防止还没走到main thread，finish回调就来了

    size_t endPosForRead = position + fixSize - 1;
    size_t endPosForProload = position + netFetchSize - 1;

//     char* output = (char*)malloc(0x300);
//     sprintf_s(output, 0x299, 
//         "read-- this:%p, m_offset:%d, m_paddingReadSize:%d, m_readEndPos:%d, endPosForRead:%d, position:%d, fixSize:%d, endPosForRead:%d m_readBeginPos:%d\n", 
//         this, m_offset, m_paddingReadSize, m_readEndPos, endPosForRead, (int)position, fixSize, endPosForRead, m_readBeginPos);
//     OutputDebugStringA(output);
//     free(output);

    m_readBeginPos = position;
    m_readEndPos = position + fixSize - 1;
   
    // 如果上次请求网络的数据虽然完成了外部read，但网络请求还在预读…
    if (m_loader.get() && m_loader->getFirstBytePosition() <= position && endPosForRead <= m_loader->getLastBytePosition()) {
        m_readCb = readCb;

//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "SimpleDataSource::read wait for proload, getLastBytePosition:%d\n", m_loader->getLastBytePosition());
//         OutputDebugStringA(output);
//         free(output);
        return;
    }
    //OutputDebugStringA("SimpleDataSource::read net work\n");

    SimpleDataSource* self = this;
    int id = m_id;

    content::postTaskToMainThread(FROM_HERE, [self, id, endPosForProload, readCb] {
        if (!(net::ActivatingObjCheck::inst()->isActivating(id) && self->m_frame))
            return;

        WTF::Locker<WTF::RecursiveMutex> autoLock(self->m_readLock);
        self->m_readCb = readCb;
        self->m_offset = self->m_readBeginPos;
        self->m_loader.reset(self->createResourceLoader(self->m_readBeginPos, endPosForProload));
        self->m_loader->start(self->m_frame);
    });
}

bool SimpleDataSource::getSize(int64* sizeOut)
{
    if (m_totalBytes != kPosNotSpecified) {
        *sizeOut = m_totalBytes;
        return true;
    }
    *sizeOut = 0;
    return false;
}

bool SimpleDataSource::isStreaming()
{
    return false;
}

void SimpleDataSource::setBitrate(int bitrate)
{

}

void SimpleDataSource::loadingStateChangedCallback(LoadingState state)
{
    ASSERT(WTF::isMainThread());

    if (!m_frame /*|| isAssumeFullyBuffered()*/)
        return;

    bool isDownloadingData;
    switch (state) {
    case SimpleDataSource::kLoading:
        isDownloadingData = true;
        break;
    case SimpleDataSource::kLoadingDeferred:
    case SimpleDataSource::kLoadingFinished:
        isDownloadingData = false;
        break;

        // TODO(scherkus): we don't signal network activity changes when loads
        // fail to preserve existing behaviour when deferring is toggled, however
        // we should consider changing DownloadingCB to also propagate loading
        // state. For example there isn't any signal today to notify the client that
        // loading has failed (we only get errors on subsequent reads).
    case SimpleDataSource::kLoadingFailed:
        return;
    }

    m_downloadingCb->notifyDownloading(isDownloadingData);
}

}