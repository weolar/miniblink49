
#include "config.h"
#include <windows.h>
#if USING_VC6RT != 1
#include <wininet.h>
#endif
#include "net/WebURLLoaderWinINet.h"
#include "net/ActivatingObjCheck.h"
#include "content/web_impl_win/WebURLLoaderImpl.h"
#include "content/web_impl_win/CurrentTimeImpl.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/Source/platform/network/FormData.h"
#include "third_party/WebKit/Source/platform/network/ResourceRequest.h"
#include "third_party/WebKit/Source/platform/network/HTTPHeaderMap.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/MIMETypeRegistry.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/wtf/TemporaryChange.h"
#include "third_party/zlib/zlib.h"
#include "net/DataURL.h"
#include "net/BlinkSynchronousLoader.h"
#include "net/cookies/WebCookieJarCurlImpl.h"

#if USING_VC6RT == 1
#define PURE                    = 0
#endif

#include <Shlwapi.h>

using namespace blink;

namespace net {

static inline HINTERNET createInternetHandle(const blink::WebString& userAgent, bool asynchronous)
{
    WTF::String userAgentString = userAgent;
    userAgentString = String::fromUTF8(userAgentString.utf8());
    HINTERNET internetHandle = InternetOpenW((LPCWSTR)userAgentString.charactersWithNullTermination().data(),
        INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, asynchronous ? INTERNET_FLAG_ASYNC : 0);

    if (asynchronous)
        InternetSetStatusCallback(internetHandle, &WebURLLoaderWinINet::internetStatusCallback);

    return internetHandle;
}

static int httpGzipDecompress(Byte *zdata, uLong nzdata, Byte *data, uLong *ndata)
{
    int err = 0;
    z_stream d_stream = { 0 }; /* decompression stream */
    static char dummy_head[2] = {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    if (inflateInit2(&d_stream, 47) != Z_OK)
        return -1;

    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if ((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if (err != Z_OK) {
            if (err == Z_DATA_ERROR) {
                d_stream.next_in = (Bytef*)dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if ((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK)
                    return -1;
            } else
                return -1;
        }
    }
    if (inflateEnd(&d_stream) != Z_OK)
        return -1;
    *ndata = d_stream.total_out;
    return 0;
}

//////////////////////////////////////////////////////////////////////////

class LoadFileAsyncTask : public blink::WebThread::Task {
public:
    LoadFileAsyncTask(WebURLLoaderWinINet* loader)
        : m_loader(loader) {}

    virtual ~LoadFileAsyncTask() OVERRIDE
    {
    }

    void cancel()
    {
        m_loader = nullptr;
    }

    virtual void run() OVERRIDE
    {
        if (!m_loader || !m_loader->firstRequest())
            return;

        WebURLLoaderWinINet* loader = m_loader; // fileLoadImpl里会调用cancel，把m_loader清空
        blink::KURL url = (blink::KURL)loader->firstRequest()->url();
        loader->fileLoadImpl(url);
        delete loader;
    }

private:
    WebURLLoaderWinINet* m_loader;
};

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderWinINetCounter, ("WebURLLoaderWinINetCounter"));
#endif

void WebURLLoaderWinINet::init(bool asynchronously)
{
    m_request = nullptr;
    m_client = nullptr;
    m_requestCanceled = false;
    m_canDestroy = true;

    if (asynchronously)
        m_internetHandle = nullptr;
    m_connectHandle = nullptr;
    m_requestHandle = nullptr;
    m_sentEndRequest = false;
    m_formData.clear();
    m_bytesRemainingToWrite = 0;
    m_hasReceivedResponse = false;
    m_totalEncodedDataLength = 0;
    m_hadDestroied = nullptr;
    m_loadSynchronously = false;
    m_debugRedirectPath = nullptr;
    m_gzipDecompressData.clear();
    m_fileAsyncTask = nullptr;
}

WebURLLoaderWinINet::WebURLLoaderWinINet(content::WebURLLoaderImpl* loader)
    : m_loader(loader)
{
    ASSERT(isMainThread());
    ActivatingObjCheck::inst()->add((intptr_t)this);

    String outString = String::format("WebURLLoaderWinINet::WebURLLoaderWinINet: %p\n", this);
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    init(true);

#ifndef NDEBUG
    webURLLoaderWinINetCounter.increment();
#endif
}

WebURLLoaderWinINet::~WebURLLoaderWinINet()
{
    String outString = String::format("WebURLLoaderWinINet::~WebURLLoaderWinINet: %p %p\n", this, m_loader);
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    ASSERT(isMainThread());
    if (!ActivatingObjCheck::inst()->isActivating((intptr_t)this))
        DebugBreak();

    ASSERT(!m_internetHandle);

    if (m_fileAsyncTask)
        m_fileAsyncTask->cancel();

    if (m_hadDestroied)
        *m_hadDestroied = true;
    m_hadDestroied = nullptr;

    if (m_request)
        delete m_request;
    m_request = nullptr;

    if (m_debugRedirectPath)
        delete m_debugRedirectPath;
    m_debugRedirectPath = nullptr;

    ActivatingObjCheck::inst()->remove((intptr_t)this);

    if (ActivatingObjCheck::inst()->isActivating((intptr_t)this))
        DebugBreak();

    if (m_loader)
        m_loader->onWinINetWillBeDelete();
    m_loader = nullptr;

#ifndef NDEBUG
    webURLLoaderWinINetCounter.decrement();
#endif
}

void WebURLLoaderWinINet::onLoaderWillBeDelete()
{
    m_loader = nullptr;
}

static void callOnRedirect(void* context)
{
    ASSERT(isMainThread());

    WebURLLoaderWinINet* handle = static_cast<WebURLLoaderWinINet*>(context);
    if (!ActivatingObjCheck::inst()->isActivating((intptr_t)handle))
        return;

    handle->onRedirect();
}

static void callOnRequestComplete(void* context)
{
    ASSERT(isMainThread());
    WebURLLoaderWinINet* handle = static_cast<WebURLLoaderWinINet*>(context);
    if (!ActivatingObjCheck::inst()->isActivating((intptr_t)handle))
        return;

    bool b = handle->onRequestComplete();
    if (!b)
        delete handle;
}

void WebURLLoaderWinINet::onRedirect()
{
    if (nullptr == m_requestHandle || m_redirectUrl.isNull() || m_redirectUrl.isEmpty())
        return;

    blink::WebURLRequest newRequest(*firstRequest());
    newRequest.setURL(blink::KURL(blink::ParsedURLString, m_redirectUrl));

    String redirectUrl = (String)firstRequest()->url().string();
    blink::WebURLResponse response(firstRequest()->url());

    if (m_client && m_loader)
        m_client->willSendRequest(m_loader, newRequest, response);
}

static String queryHTTPHeader(HINTERNET requestHandle, DWORD infoLevel)
{
    DWORD bufferSize = 0;
    HttpQueryInfoW(requestHandle, infoLevel, 0, &bufferSize, 0);
    if (0 == bufferSize)
        return String();

    Vector<UChar> characters((bufferSize + 1) / sizeof(UChar));
    if (!HttpQueryInfoW(requestHandle, infoLevel, characters.data(), &bufferSize, 0))
        return String();

    if (0 == bufferSize || (bufferSize / sizeof(UChar)) > characters.size() - 1)
        return String();

    if (0 == characters.at(bufferSize / sizeof(UChar) - 1))
        characters.removeLast(); // Remove NullTermination.
    return String(characters.data(), bufferSize / sizeof(UChar));
}

void WebURLLoaderWinINet::internetStatusCallback(HINTERNET internetHandle, DWORD_PTR context, DWORD internetStatus,
    LPVOID statusInformation, DWORD statusInformationLength)
{
    WebURLLoaderWinINet* handle = reinterpret_cast<WebURLLoaderWinINet*>(context);
    if (!ActivatingObjCheck::inst()->isActivating((intptr_t)handle))
        return;

    switch (internetStatus) {
    case INTERNET_STATUS_REDIRECT:
    {
        String redirectUrl(static_cast<UChar*>(statusInformation), statusInformationLength);
        ASSERT(redirectUrl.containsOnlyASCII());
        if (redirectUrl.containsOnlyASCII()) {
            handle->m_redirectUrl = String::fromUTF8(redirectUrl.utf8());
            WTF::internal::callOnMainThread(callOnRedirect, handle);
        }
    }
    break;

    case INTERNET_STATUS_REQUEST_COMPLETE:
        WTF::internal::callOnMainThread(callOnRequestComplete, handle);
        break;

    default:
        break;
    }
}

static HINTERNET asynchronousInternetHandle(const blink::WebString& userAgent)
{
    /*static*/ HINTERNET internetHandle = createInternetHandle(userAgent, true);
    return internetHandle;
}

static void respondError(WebURLLoader* loader, blink::WebURLLoaderClient* client, const Vector<UChar>& host)
{
    blink::WebURLError error;
    blink::WebString domain((const blink::WebUChar*)host.data(), host.size());
    error.reason = GetLastError();
    if (0 == error.reason)
        error.reason = -1;
    error.domain = domain;
    client->didFail(loader, error);
}

static void appendCharToVector(Vector<char>& buffer, const char* characters)
{
    buffer.append(characters, strlen(characters));
}

class HeaderFlattenerForWinINet : public blink::WebHTTPHeaderVisitor {
public:
    explicit HeaderFlattenerForWinINet() : m_hasAcceptHeader(false) {}

    virtual void visitHeader(const blink::WebString& name, const blink::WebString& value) override
    {
        String nameString = (String)name;
        // Headers are latin1.
        CString nameLatin1 = ((String)name).latin1();
        CString valueLatin1 = ((String)value).latin1();

        // Skip over referrer headers found in the header map because we already
        // pulled it out as a separate parameter.
        // 但挪到这里后，没有单独的参数写referr，干脆也加到headBuffer里
        //         if (equalIgnoringCase(nameString, "referer"))
        //             return;

        if (equalIgnoringCase(nameString, "accept"))
            m_hasAcceptHeader = true;

        //         if (!m_buffer.isEmpty())
        //             appendCharToVector(m_buffer, "\r\n");

        Vector<char> headBuffer;
        headBuffer.append(nameLatin1.data(), nameLatin1.length());
        appendCharToVector(headBuffer, ": ");
        headBuffer.append(valueLatin1.data(), valueLatin1.length());
        appendCharToVector(headBuffer, "\r\n");

        m_buffer.append(headBuffer.data(), headBuffer.size());

        ::HttpAddRequestHeadersA(m_requestHandle, headBuffer.data(), headBuffer.size(), HTTP_ADDREQ_FLAG_ADD);
    }

    const Vector<char>& getBuffer()
    {
        // In some cases, WebKit doesn't add an Accept header, but not having the
        // header confuses some web servers.  See bug 808613.
        if (!m_hasAcceptHeader) {
            if (!m_buffer.isEmpty())
                appendCharToVector(m_buffer, "\r\n");
            appendCharToVector(m_buffer, "Accept: */*\r\n");
            m_hasAcceptHeader = true;
        }
        DebugBreak();

        //         String cookie = content::WebCookieJarImpl::inst()->cookies(m_url, blink::KURL());
        //         
        //         if (!cookie.isNull() && !cookie.isEmpty()) {
        //             appendCharToVector(m_buffer, "Cookie: ");
        //             appendCharToVector(m_buffer, cookie.utf8().data());
        //             appendCharToVector(m_buffer, "\r\n");
        //         }
        //         appendCharToVector(m_buffer, "\r\n");

        //////////////////////////////////////////////////////////////////////////
        if (0 && m_debugTest) {
            m_buffer.clear();
            appendCharToVector(m_buffer, "Accept: application/json\r\n");
            appendCharToVector(m_buffer, "Accept-Encoding: gzip, deflate, sdch\r\n");
            appendCharToVector(m_buffer, "Accept-Language: zh-CN,zh;q=0.8\r\n");
            appendCharToVector(m_buffer, "Cache-Control: max-age=0\r\n");
            appendCharToVector(m_buffer, "Connection: keep-alive\r\n");
            appendCharToVector(m_buffer, "Content-type: application/x-www-form-urlencoded\r\n");
            appendCharToVector(m_buffer, "Cookie: xmuuid=XMGUEST-FC6BCE40-1587-11E5-9022-0B280AD5AED4; _newdevuuid=s1DeZNMiYmxqQrVycDEzIWzF6RBj3RV508lG0RVCsUloulkN0LSna5AaIYA9ZpQCcrP78bV3eisBam9mH2oyqMYIDuvwyp0cKfsbR7iumcfZExrCNo945PecPxbXz4r605K18k5c4IaLzmFgp2ot7b66B4wR9hbXT8IjaJ6yzkA2zUlhQ1v9tpGwoewiRMbWCzYDgoUI; _mmsid=1464599910378.2537; mstz=||1375463365.1|||; mstuid=1434610951973_2745; xm_vistor=1434610951973_2745_1464599910410-1464599910410; Hm_lvt_4982d57ea12df95a2b24715fb6440726=1464599910; Hm_lpvt_4982d57ea12df95a2b24715fb6440726=1464599910; MAEInstance=0e526c69015e323a49aa55ed7c91f0a7bdf1d210de13ca58f9f3f48ffb6cfabd\r\n");
            appendCharToVector(m_buffer, "Host: m.mi.com\r\n");
            appendCharToVector(m_buffer, "Origin: http://m.mi.com\r\n");
            appendCharToVector(m_buffer, "Referer: http://m.mi.com/1/\r\n");
            appendCharToVector(m_buffer, "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.99 Safari/537.36 LBBROWSER\r\n");
            appendCharToVector(m_buffer, "X-Requested-With:XMLHttpRequest\r\n");
            appendCharToVector(m_buffer, "\r\n");
            ::HttpAddRequestHeadersA(m_requestHandle, m_buffer.data(), m_buffer.size(), HTTP_ADDREQ_FLAG_ADD);
        }


        //////////////////////////////////////////////////////////////////////////
        return m_buffer;
    }

    HINTERNET m_requestHandle;
    KURL m_url;

    bool m_debugTest;
private:
    Vector<char> m_buffer;
    bool m_hasAcceptHeader;
};

bool getCookiesFromHost(const KURL& url, Vector<char>* cookies)
{
    DWORD cookie_size = 0;
    bool success = true;
    std::string cookie_string;

#ifndef COOKIEACTION_READ
#define COOKIEACTION_READ 0x00000020
#endif
    int32 cookie_action = COOKIEACTION_READ;
#undef COOKIEACTION_READ
    // TODO string utf8 -> UTF16
    BOOL result = InternetGetCookieA(url.string().utf8().data(), NULL, NULL, &cookie_size);
    DWORD error = 0;
    if (!result || 0 == cookie_size) {
        success = false;
        error = GetLastError();
        return false;
    }

    cookies->resize(cookie_size + 1);
    if (!InternetGetCookieA(url.string().utf8().data(), NULL, cookies->data(), &cookie_size)) {
        success = false;
        error = GetLastError();
        return false;
    }

    cookies->at(cookie_size) = '\0';
    return true;
}

bool testHttpDownload(KURL url, Vector<char>& buffer)
{
    HINTERNET hInternet;
    hInternet = InternetOpenW(L"", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet == NULL)
        return false;

    // 建立连接
    HINTERNET hConnect = nullptr;;
    hConnect = InternetConnectW(hInternet, L"x5.imtt.qq.com", 80, L"", L"", INTERNET_SERVICE_HTTP, INTERNET_FLAG_PASSIVE, 0);

    if (hInternet == NULL) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET httpFile;
    httpFile = HttpOpenRequestW(hConnect, L"GET", L"0072811/web_oa/opendoc.xml", HTTP_VERSION, NULL, 0, INTERNET_FLAG_NO_UI | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD, 1);
    if (httpFile == NULL) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    HttpSendRequestW(httpFile, NULL, NULL, 0, 0);

    DWORD bytesWritten = 4;
    const char* writeData = "aaaa";
    BOOL b = InternetWriteFile(httpFile, writeData, 4, &bytesWritten);

    //HINTERNET httpFile = InternetOpenUrl(hInternet, L"http://x5.imtt.qq.com/0072811/web_oa/opendoc.xml", NULL, 0, INTERNET_FLAG_RELOAD, 0);

    const DWORD bufLen = 1024;
    DWORD bufRead = 0;

    Vector<char> tempBuffer;
    tempBuffer.resize(bufLen);
    while (true) {
        BOOL b = InternetReadFile(httpFile, tempBuffer.data(), bufLen, &bufRead);
        if (bufRead == 0 /* buffers.dwBufferLength == 0*/)
            break;
        buffer.append(tempBuffer.data(), bufRead);
    }

    InternetCloseHandle(httpFile);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return true;
}

// WebURLLoader methods:
void WebURLLoaderWinINet::loadSynchronously(const blink::WebURLRequest& request, blink::WebURLResponse& response,
    blink::WebURLError& error, blink::WebData& data)
{
    Vector<char> buffer;

    BlinkSynchronousLoader loader(error, response, buffer);
    m_internetHandle = createInternetHandle(blink::Platform::current()->userAgent(), false);

    bool b = start(request, &loader, false); // 执行完后本类被析构了
    ASSERT(!b);

    data.assign(buffer.data(), buffer.size());
    m_canDestroy = true;
    cancel();
}

void WebURLLoaderWinINet::loadAsynchronously(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client)
{
#if 0
    Vector<char> buffer;
    blink::WebURLResponse response;
    testHttpDownload(request.url(), buffer);
    response.setURL(request.url());
    response.setHTTPStatusCode(200);
    response.setMIMEType(WebString::fromUTF8("text/xml; charset=UTF-8"));
    response.setHTTPStatusText(WebString::fromUTF8("ok"));
#endif

    bool b = start(request, client, true);
    if (!b)
        delete this;
}

// false 表示中断了，true表示还需要继续接收回调数据
bool WebURLLoaderWinINet::start(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client, bool asynchronously)
{
    init(asynchronously);
    m_loadSynchronously = !asynchronously;
    m_startTime = WTF::currentTimeMS();
    m_request = new blink::WebURLRequest(request);
    m_client = client;

    blink::KURL url = (blink::KURL)request.url();
    Vector<UChar> host = WTF::ensureUTF16UChar(url.host(), true);

    if (!url.isValid() || !url.protocolIsData()) {
        WTF::String outstr = String::format("WebURLLoaderWinINet.loadAsynchronously: %p %ws\n", this, WTF::ensureUTF16UChar(url.string(), true).data());
        OutputDebugStringW(outstr.charactersWithNullTermination().data());
    }

    if (url.isLocalFile() || url.protocolIsData()) {
        if (m_loadSynchronously) {
            fileLoadImpl(url);
            return false;
        } else {
            m_fileAsyncTask = new LoadFileAsyncTask(this);
            blink::Platform::current()->currentThread()->postTask(FROM_HERE, m_fileAsyncTask);
        }
        return true;
    }

    if (!m_internetHandle)
        m_internetHandle = asynchronousInternetHandle(blink::Platform::current()->userAgent());
    if (!m_internetHandle) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        respondError(m_loader, client, host);
        return false;
    }

    DWORD flags = INTERNET_FLAG_KEEP_CONNECTION
        //| INTERNET_FLAG_NO_COOKIES // TODO
        | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
        | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP
        | INTERNET_FLAG_DONT_CACHE
        | INTERNET_FLAG_RELOAD;
    if (url.protocolIs("https"))
        flags |= INTERNET_FLAG_SECURE;

    int port = url.port();
    if (0 == port) {
        if (url.protocolIs("http"))
            port = INTERNET_DEFAULT_HTTP_PORT;
        else if (url.protocolIs("https"))
            port = INTERNET_DEFAULT_HTTPS_PORT;
    }

    m_connectHandle = InternetConnectW(m_internetHandle, host.data(), port, 0, 0, INTERNET_SERVICE_HTTP, flags, reinterpret_cast<DWORD_PTR>(this));
    if (!m_connectHandle) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        respondError(m_loader, client, host);
        return false;
    }

    String urlStr = url.path();
    String urlQuery = url.query();

    if (!urlQuery.isEmpty()) {
        urlStr.append('?');
        urlStr.append(urlQuery);
    }

    Vector<UChar> httpMethod = WTF::ensureUTF16UChar(request.httpMethod(), true);
    Vector<UChar> httpReferrer = WTF::ensureUTF16UChar(request.httpHeaderField(blink::WebString::fromUTF8("Referer")), true);
    Vector<UChar> httpAcceptField = WTF::ensureUTF16UChar(request.httpHeaderField(blink::WebString::fromUTF8("Accept")), true);

    LPCWSTR httpAccept[] = { httpAcceptField.data(), 0 };

    m_requestHandle = HttpOpenRequestW(m_connectHandle, (LPCWSTR)httpMethod.data(), (LPCWSTR)WTF::ensureUTF16UChar(urlStr, true).data(),
        0, (LPCWSTR)httpReferrer.data(), httpAccept, flags, reinterpret_cast<DWORD_PTR>(this));

    if (!m_requestHandle) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        InternetCloseHandle(m_connectHandle);
        m_connectHandle = 0;
        respondError(m_loader, client, host);
        return false;
    }

    if (!request.httpBody().isNull()) {
        WTF::PassRefPtr<blink::FormData> formData = request.httpBody();
        if (formData) {
            formData->flatten(m_formData);
            m_bytesRemainingToWrite = m_formData.size();
        }
    }

    HeaderFlattenerForWinINet flattener;
    flattener.m_url = url;
    flattener.m_debugTest = WTF::kNotFound != url.string().find("http://m.mi.com/v1/product/view");
    flattener.m_requestHandle = m_requestHandle;
    request.visitHTTPHeaderFields(&flattener);
    Vector<char> httpHeaders = flattener.getBuffer();

    INTERNET_BUFFERSA internetBuffers = { 0 };
    internetBuffers.dwStructSize = sizeof(internetBuffers);
    internetBuffers.lpcszHeader = (LPCSTR)httpHeaders.data();
    internetBuffers.dwHeadersLength = httpHeaders.size();
    internetBuffers.dwBufferTotal = m_bytesRemainingToWrite;
    internetBuffers.lpvBuffer = m_formData.data();

    BOOL b = HttpSendRequestExA(m_requestHandle, &internetBuffers, 0, 0, reinterpret_cast<DWORD_PTR>(this));

    if (m_loadSynchronously) {
        while (onRequestComplete()) {
            // Loop until finished.
        }
        return false;
    }
    return true;
}

bool WebURLLoaderWinINet::onRequestComplete()
{
//     if (!m_internetHandle || (!m_requestHandle && !m_connectHandle))
//         return false;
// 
//     if (m_bytesRemainingToWrite) {
//         DWORD bytesWritten;
//         const char* writeData = m_formData.data() + (m_formData.size() - m_bytesRemainingToWrite);
//         BOOL b = InternetWriteFile(m_requestHandle, writeData, m_bytesRemainingToWrite, &bytesWritten);
//         if (!b) {
//             DWORD dwError;
//             Vector<wchar_t> errorBuf;
//             DWORD dwBufferLength = 200;
//             errorBuf.resize(dwBufferLength);
//             b = ::InternetGetLastResponseInfoW(&dwError, errorBuf.data(), &dwBufferLength);
//             OutputDebugStringW(errorBuf.data());
//             OutputDebugStringA("\n");
//         }
//         m_bytesRemainingToWrite -= bytesWritten;
//         //         if (m_bytesRemainingToWrite)
//         //             return true;
//         m_formData.clear();
//     }
// 
//     if (!m_sentEndRequest) {
//         HttpEndRequestW(m_requestHandle, 0, 0, reinterpret_cast<DWORD_PTR>(this));
//         m_sentEndRequest = true;
//         return true;
//     }
// 
//     static const int bufferSize = 32768;
//     WTF::Vector<char> buffer;
//     buffer.resize(bufferSize + 2);
//     INTERNET_BUFFERSA buffers = { 0 };
//     buffers.dwStructSize = sizeof(INTERNET_BUFFERSA);
//     buffers.lpvBuffer = buffer.data();
//     buffers.dwBufferLength = bufferSize;
// 
//     bool needGzip = false;
// 
//     BOOL ok = FALSE;
//     while ((ok = InternetReadFileExA(m_requestHandle, &buffers, m_loadSynchronously ? IRF_SYNC : IRF_NO_WAIT, reinterpret_cast<DWORD_PTR>(this))) && buffers.dwBufferLength) {
//         ASSERT(buffers.dwBufferLength <= bufferSize);
// 
//         if (!m_hasReceivedResponse) {
//             m_hasReceivedResponse = true;
// 
//             blink::WebURLResponse response(firstRequest()->url());
// 
//             WTF::String httpStatusText = queryHTTPHeader(m_requestHandle, HTTP_QUERY_STATUS_TEXT);
//             if (!httpStatusText.isNull())
//                 response.setHTTPStatusText(httpStatusText);
// 
//             WTF::String httpStatusCode = queryHTTPHeader(m_requestHandle, HTTP_QUERY_STATUS_CODE);
//             if (!httpStatusCode.isNull())
//                 response.setHTTPStatusCode(httpStatusCode.toInt());
// 
//             WTF::String httpContentLength = queryHTTPHeader(m_requestHandle, HTTP_QUERY_CONTENT_LENGTH);
//             if (!httpContentLength.isNull())
//                 response.setExpectedContentLength(httpContentLength.toInt());
// 
//             WTF::String httpContentType = queryHTTPHeader(m_requestHandle, HTTP_QUERY_CONTENT_TYPE);
//             if (!httpContentType.isNull()) {
//                 response.setMIMEType(blink::extractMIMETypeFromMediaType(WTF::AtomicString(httpContentType)));
//                 response.setTextEncodingName(blink::extractCharsetFromMediaType(WTF::AtomicString(httpContentType)));
// 
//                 blink::WebUChar kContentType[] = L"Content-Type";
//                 blink::WebString kContentTypeString(kContentType, sizeof(kContentType) / sizeof(blink::WebUChar) - 1);
//                 response.setHTTPHeaderField(kContentTypeString, blink::WebString(httpContentType));
//             }
// 
//             WTF::String rawHeadersCrlf = queryHTTPHeader(m_requestHandle, HTTP_QUERY_RAW_HEADERS_CRLF);
//             content::WebCookieJarImpl::inst()->setToRecordFromRawHeads(m_request->url(), rawHeadersCrlf);
// 
//             needGzip = (WTF::kNotFound != rawHeadersCrlf.find("Content-Encoding: gzip"));
// 
//             WTF::String cookies = queryHTTPHeader(m_requestHandle, HTTP_QUERY_COOKIE);
// 
//             if (m_client && m_loader) {
//                 WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
//                 m_client->didReceiveResponse(m_loader, response);
//                 if (!ActivatingObjCheck::inst()->isActivating((intptr_t)this)) // 有可能在didReceiveResponse把整个类析构掉
//                     return false;
//             }
//         }
// 
//         m_totalEncodedDataLength += buffers.dwBufferLength;
// 
//         // FIXME: https://bugs.webkit.org/show_bug.cgi?id=19793
//         // -1 means we do not provide any data about transfer size to inspector so it would use
//         // Content-Length headers or content size to show transfer size.
//         if (m_client && m_loader && !m_debugRedirectPath && !needGzip) {
//             WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
//             m_client->didReceiveData(m_loader, buffer.data(), buffers.dwBufferLength, -1);
//         } else
//             m_gzipDecompressData.append(buffer.data(), buffers.dwBufferLength);
// 
//         if (!ActivatingObjCheck::inst()->isActivating((intptr_t)this))
//             return false;
// 
//         buffers.dwBufferLength = bufferSize;
//     }
// 
//     if (!ok && GetLastError() == ERROR_IO_PENDING) {
//         //         String urlTest = m_request->url().string();
//         //         String out = String::format("WebURLLoaderWinINet::onRequestComplete not complete:");
//         //         out.append(urlTest);
//         //         out.append("\n");
//         //         OutputDebugStringW(out.charactersWithNullTermination().data());
//         return true;
//     }
// 
//     Vector<char> cookies;
//     if (getCookiesFromHost(m_request->url(), &cookies))
//         content::WebCookieJarImpl::inst()->setCookieFromWinINet(m_request->url(), cookies);
// 
//     if (m_debugRedirectPath)
//         fileLoadImpl(*m_debugRedirectPath);
// 
//     ::InternetCloseHandle(m_requestHandle);
//     ::InternetCloseHandle(m_connectHandle);
//     m_requestHandle = 0;
//     m_connectHandle = 0;
// 
//     if (m_client && m_loader) {
//         if (0 == m_totalEncodedDataLength || !m_hasReceivedResponse) {
//             blink::WebURLError error;
//             error.reason = GetLastError();
//             if (0 == error.reason)
//                 error.reason = -1;
//             error.domain = blink::WebString(m_request->url().string());
//             error.localizedDescription = blink::WebString::fromUTF8("Url request fail, data length is null\n");
//             WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
//             m_client->didFail(m_loader, error);
//             return false;
//         }
// 
//         //         String urlTest = m_request->url().string();
//         //         String out = String::format("WebURLLoaderWinINet::onRequestComplete:");
//         //         out.append(urlTest);
//         //         out.append("\n");
//         //         OutputDebugStringW(out.charactersWithNullTermination().data());
// 
//         if (needGzip && m_gzipDecompressData.size() > 0) {
//             Vector<unsigned char> decompressData;
//             decompressData.resize(m_gzipDecompressData.size() * 10);
//             uLong ndata = decompressData.size();
//             if (0 == httpGzipDecompress(m_gzipDecompressData.data(), m_gzipDecompressData.size(), decompressData.data(), &ndata))
//                 m_client->didReceiveData(m_loader, (const char *)decompressData.data(), ndata, -1);
//         }
// 
//         WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
//         m_client->didFinishLoading(m_loader, WTF::currentTime(), m_totalEncodedDataLength);
//     }
    DebugBreak();
    return false;
}

void WebURLLoaderWinINet::onTimeout()
{
    if (m_client) {
        blink::WebURLError error;
        error.domain = blink::WebString(m_request->url().string());
        error.localizedDescription = blink::WebString::fromUTF8("Url request time out\n");
        m_client->didFail(m_loader, error);
    }
}

void WebURLLoaderWinINet::fileLoadImpl(const blink::KURL& url)
{
    if (requestCanceled())
        return;

    if (url.protocolIsData()) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        if (m_client && m_loader)
            net::handleDataURL(m_loader, m_client, url);
        return;
    }

    Vector<UChar> fileNameVec = WTF::ensureUTF16UChar(url.fileSystemPath(), false);
    String fileName(fileNameVec.data(), fileNameVec.size());
    if (L'/' == fileName[0])
        fileName.remove(0);
    if (WTF::kNotFound == fileName.find(":/") && WTF::kNotFound == fileName.find(":\\")) {
        WTF::Vector<WCHAR> moduleName;
        moduleName.resize(MAX_PATH + 3);
        moduleName[MAX_PATH] = 0;
        ::GetModuleFileNameW(NULL, moduleName.data(), MAX_PATH);
        ::PathRemoveFileSpecW(moduleName.data());
        size_t moduleNameLen = wcslen(moduleName.data());
        if (moduleName[moduleNameLen - 1] != L'\\') {
            moduleName[moduleNameLen] = L'\\';
            ++moduleNameLen;
        }

        unsigned insertPos = 0;
        if (fileName.startsWith("file:///"))
            insertPos = 8;
        fileName.insert(moduleName.data(), moduleNameLen, insertPos);
        fileName.replace("/", "\\");
    }

    HANDLE fileHandle = ::CreateFileW((LPCWSTR)fileName.charactersWithNullTermination().data(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        blink::WebURLError error;
        error.reason = GetLastError();
        if (0 == error.reason)
            error.reason = -1;
        error.domain = blink::WebString(fileNameVec.data(), fileNameVec.size());
        error.localizedDescription = blink::WebString::fromUTF8("fileLoadImpl -> CreateFileW fail\n");
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        if (m_client && m_loader)
            m_client->didFail(m_loader, error);

        String errorOut("WebURLLoaderWinINet::fileLoadImpl fail:");
        errorOut.append(fileName);
        errorOut.append("\n");
        OutputDebugStringW(errorOut.charactersWithNullTermination().data());
        return;
    }

    blink::WebURLResponse response;
    response.initialize();
    response.setURL(blink::WebURL(url));
    response.setHTTPStatusCode(200);
    response.setHTTPStatusText(WebString::fromUTF8("ok"));

    int dotPos = fileName.reverseFind('.');
    int slashPos = fileName.reverseFind('/');

    if (slashPos < dotPos && dotPos != -1) {
        String ext = fileName.substring(dotPos + 1);
        response.setMIMEType(blink::MIMETypeRegistry::getMIMETypeForExtension(ext));
    }

    if (m_client && m_loader && !m_debugRedirectPath) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        m_client->didReceiveResponse(m_loader, response);
    }

    bool result = false;
    DWORD bytesRead = 0;
    int64_t totalEncodedDataLength = 0;

    do {
        const int bufferSize = 8192;
        WTF::Vector<char> buffer;
        buffer.resize(bufferSize);
        result = ::ReadFile(fileHandle, buffer.data(), bufferSize, &bytesRead, 0);
        // FIXME: https://bugs.webkit.org/show_bug.cgi?id=19793
        // -1 means we do not provide any data about transfer size to inspector so it would use
        // Content-Length headers or content size to show transfer size.
        if (result && bytesRead) {
            if (m_client && m_loader)
                m_client->didReceiveData(m_loader, buffer.data(), bytesRead, -1);
            totalEncodedDataLength += bytesRead;
        }
        // Check for end of file.
    } while (result && bytesRead);

    ::CloseHandle(fileHandle);

    if (m_client && m_loader && !m_debugRedirectPath) {
        WTF::TemporaryChange<bool> cannotDestroy(m_canDestroy, false);
        m_client->didFinishLoading(m_loader, WTF::currentTime(), totalEncodedDataLength);
    }
}

void WebURLLoaderWinINet::cancel()
{
    String outString = String::format("WebURLLoaderWinINet::cancel: %p %d %p\n", this, m_canDestroy, m_internetHandle);
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (m_internetHandle) {
        InternetSetStatusCallback(m_internetHandle, nullptr);
        InternetCloseHandle(m_internetHandle);
        m_internetHandle = nullptr;
    }

    if (m_requestHandle) {
        InternetCloseHandle(m_requestHandle);
        InternetCloseHandle(m_connectHandle);

        m_requestHandle = nullptr;
        m_connectHandle = nullptr;

        // 调用InternetCloseHandle，系统不会调用oncomplete，所以手动析构
    } else
        m_requestCanceled = true;

    m_client = nullptr;
    if (m_fileAsyncTask)
        m_fileAsyncTask->cancel();

    if (m_canDestroy)
        delete this;
}

void WebURLLoaderWinINet::setDefersLoading(bool value)
{

}

void WebURLLoaderWinINet::didChangePriority(blink::WebURLRequest::Priority new_priority,
    int intra_priority_value)
{

}

bool WebURLLoaderWinINet::attachThreadedDataReceiver(
    blink::WebThreadedDataReceiver* threaded_data_receiver)
{
    DebugBreak();
    return false;
}

}  // namespace content

namespace blink {

bool saveDumpFile(const String& url, char* buffer, unsigned int size)
{
    HANDLE hFile = NULL;
    bool   bRet = false;

    if (1) {
        static int i = 0;
        ++i;
        String savePath;
        savePath = String::format("C:\\Users\\weo\\Desktop\\mantan\\DumpFile_%d.png", i);

        String output;
        output = String::format("saveDumpFile:%d ", i);
        output.append(url);
        output.append("\n");
        //OutputDebugStringW(output.charactersWithNullTermination().data());

        hFile = CreateFileW(savePath.charactersWithNullTermination().data(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (!hFile || INVALID_HANDLE_VALUE == hFile)
            return false;

        DWORD numberOfBytesWritten = 0;
        ::WriteFile(hFile, buffer, size, &numberOfBytesWritten, NULL);
        ::CloseHandle(hFile);
        bRet = true;
    }

    return bRet;
}

void readScript(const WCHAR* path, Vector<char>& buffer)
{
    HANDLE hFile = NULL;

    hFile = CreateFileW(path,
        GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD nNumberOfBytesToRead = 0;
    DWORD dwFileSizeHigh = 0;
    nNumberOfBytesToRead = ::GetFileSize(hFile, &dwFileSizeHigh);

    buffer.resize(nNumberOfBytesToRead);

    DWORD nNumberOfBytesRead = 0;
    ::ReadFile(hFile, buffer.data(), nNumberOfBytesToRead, &nNumberOfBytesRead, nullptr);

    ::CloseHandle(hFile);
}

}