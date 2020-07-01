#include "config.h"

#include "content/web_impl_win/WebURLLoaderImplCurl.h"
#include "content/web_impl_win/WebBlobRegistryImpl.h"

#include "net/WebURLLoaderManager.h"
#include "net/WebURLLoaderInternal.h"
#include "net/BlinkSynchronousLoader.h"
#include "net/BlobResourceLoader.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

#include <windows.h>

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderImplCurlCount, ("WebURLLoaderImplCurl"));
#endif

namespace content {

WebURLLoaderImplCurl::WebURLLoaderImplCurl()
{
    m_jobIds = 0;
    m_blobLoader = nullptr;
#ifndef NDEBUG
    webURLLoaderImplCurlCount.increment();
#endif
}

WebURLLoaderImplCurl::~WebURLLoaderImplCurl()
{
#ifndef NDEBUG
    webURLLoaderImplCurlCount.decrement();
#endif
}

void WebURLLoaderImplCurl::init()
{
    m_hadDestroied = false;
    m_jobIds = 0;
    m_blobLoader = nullptr;
}

static bool shouldContentSniffURL(const KURL& url)
{
    // We shouldn't content sniff file URLs as their MIME type should be established via their extension.
    return !url.protocolIs("file");
}

void WebURLLoaderImplCurl::loadSynchronously(
    const blink::WebURLRequest& request,
    blink::WebURLResponse& response,
    blink::WebURLError& error,
    blink::WebData& data)
{
    if (!net::WebURLLoaderManager::sharedInstance())
        return;

    init();

    WebURLRequest requestNew = request;

    Vector<char> buffer;
    net::BlinkSynchronousLoader syncLoader(error, response, buffer);
    net::WebURLLoaderInternal* job = new net::WebURLLoaderInternal(this, requestNew, &syncLoader, false, shouldContentSniffURL(request.url()));
    net::WebURLLoaderManager::sharedInstance()->dispatchSynchronousJob(job);

    data.assign(buffer.data(), buffer.size());
}

void WebURLLoaderImplCurl::loadAsynchronously(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client)
{
    if (!net::WebURLLoaderManager::sharedInstance())
        return;

    init();

    KURL url = request.url();
    if (url.protocol() == "blob") {
        WebBlobRegistryImpl* blolRegistry = (WebBlobRegistryImpl*)blink::Platform::current()->blobRegistry();
        net::BlobDataWrap* blogData = blolRegistry->getBlobDataFromUUID(url.string());
        if (!blogData)
            return;

        blolRegistry->abortStream(WebURL());

        m_blobLoader = net::BlobResourceLoader::createAsync(blogData, request, client, this);
        m_blobLoader->start();
        return;
    }

    WebURLRequest requestNew = request;
    net::WebURLLoaderInternal* job = new net::WebURLLoaderInternal(this, requestNew, client, false, shouldContentSniffURL(request.url()));
    int jobIds = net::WebURLLoaderManager::sharedInstance()->addAsynchronousJob(job);
    if (0 == jobIds)
        return;
    m_jobIds = jobIds;

    // 执行完add后，this可能被销毁，当dataurl的时候
#if 0
    blink::KURL url = (blink::KURL)requestNew.url();
    Vector<UChar> host = WTF::ensureUTF16UChar(url.host());

    if (!url.isValid() || !url.protocolIsData()) {
        WTF::String outstr = String::format("WebURLLoaderImpl.loadAsynchronously: %p %ws\n", this, WTF::ensureUTF16UChar(url.string()).data());
        OutputDebugStringW(outstr.charactersWithNullTermination().data());
    }
#endif
}

void WebURLLoaderImplCurl::cancel()
{
    if (m_blobLoader) {
        m_blobLoader->cancel();
        delete m_blobLoader;
        m_blobLoader = nullptr;
        return;
    }

    if (0 != m_jobIds && net::WebURLLoaderManager::sharedInstance())
        net::WebURLLoaderManager::sharedInstance()->cancel(m_jobIds);
    m_jobIds = 0;
}

void WebURLLoaderImplCurl::fileLoadImpl(const blink::KURL& url)
{

}

void WebURLLoaderImplCurl::setDefersLoading(bool value)
{
    //notImplemented();
}

void WebURLLoaderImplCurl::didChangePriority(blink::WebURLRequest::Priority new_priority, int intra_priority_value)
{
    //notImplemented();
}

bool WebURLLoaderImplCurl::attachThreadedDataReceiver(blink::WebThreadedDataReceiver* threaded_data_receiver)
{
    notImplemented();
    return false;
}

} // content