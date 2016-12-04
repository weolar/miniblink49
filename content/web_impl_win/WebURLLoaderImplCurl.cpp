#include "config.h"

#include "content/web_impl_win/WebURLLoaderImplCurl.h"

#include <windows.h>
//#include <wininet.h>

#include "net/WebURLLoaderManager.h"
#include "net/WebURLLoaderInternal.h"
#include "net/BlinkSynchronousLoader.h"

#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webURLLoaderImplCurlCount, ("WebURLLoaderImplCurl"));
#endif

namespace content {

WebURLLoaderImplCurl::WebURLLoaderImplCurl()
{
    m_webURLLoaderInternal = nullptr;
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
    m_webURLLoaderInternal = nullptr;
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
    init();

    WebURLRequest requestNew = request;

    Vector<char> buffer;
    net::BlinkSynchronousLoader syncLoader(error, response, buffer);
    net::WebURLLoaderInternal* job = new net::WebURLLoaderInternal(this, requestNew, &syncLoader, false, shouldContentSniffURL(request.url()));

    net::WebURLLoaderManager::sharedInstance()->dispatchSynchronousJob(job);

    data.assign(buffer.data(), buffer.size());

    delete job;
}

void WebURLLoaderImplCurl::loadAsynchronously(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client)
{
    init();

    WebURLRequest requestNew = request;
    m_webURLLoaderInternal = new net::WebURLLoaderInternal(this, requestNew, client, false, shouldContentSniffURL(request.url()));
    net::WebURLLoaderManager::sharedInstance()->add(m_webURLLoaderInternal);

    blink::KURL url = (blink::KURL)requestNew.url();
    Vector<UChar> host = WTF::ensureUTF16UChar(url.host());

#if 0
    if (!url.isValid() || !url.protocolIsData()) {
        WTF::String outstr = String::format("WebURLLoaderImpl.loadAsynchronously: %p %ws\n", this, WTF::ensureUTF16UChar(url.string()).data());
        OutputDebugStringW(outstr.charactersWithNullTermination().data());
    }
#endif

    return;
}

void WebURLLoaderImplCurl::cancel()
{
    net::WebURLLoaderManager::sharedInstance()->cancel(m_webURLLoaderInternal);
    m_webURLLoaderInternal = nullptr;
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