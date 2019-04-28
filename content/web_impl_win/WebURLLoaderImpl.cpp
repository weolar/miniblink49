
#include "config.h"
#include <windows.h>
#if USING_VC6RT != 1
#include <wininet.h>
#endif
#include "content/web_impl_win/WebURLLoaderImpl.h"
#include "net/WebURLLoaderWinINet.h"

namespace content {

WebURLLoaderImpl::WebURLLoaderImpl()
{
    String outString = String::format("WebURLLoaderImpl::WebURLLoaderImpl: %p\n", this);
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    m_loaderWinINet = nullptr;
}

WebURLLoaderImpl::~WebURLLoaderImpl()
{
    String outString = String::format("WebURLLoaderImpl::~WebURLLoaderImpl: %p %p\n", this, m_loaderWinINet);
    OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (m_loaderWinINet)
        m_loaderWinINet->onLoaderWillBeDelete();
    m_loaderWinINet = nullptr;
}

// WebURLLoader methods:
void WebURLLoaderImpl::loadSynchronously(
    const blink::WebURLRequest& request,
    blink::WebURLResponse& response,
    blink::WebURLError& error,
    blink::WebData& data)
{
    m_loaderWinINet = new net::WebURLLoaderWinINet(this);
    m_loaderWinINet->loadSynchronously(request, response, error, data);
}

void WebURLLoaderImpl::loadAsynchronously(
    const blink::WebURLRequest& request,
    blink::WebURLLoaderClient* client)
{
    bool canLoad = false;

    m_loaderWinINet = new net::WebURLLoaderWinINet(this);
    m_loaderWinINet->loadAsynchronously(request, client);
}

void WebURLLoaderImpl::cancel()
{
    ASSERT(m_loaderWinINet);
    m_loaderWinINet->cancel();
}

void WebURLLoaderImpl::setDefersLoading(bool value)
{
    m_loaderWinINet->setDefersLoading(value);
}

void WebURLLoaderImpl::didChangePriority(blink::WebURLRequest::Priority newPriority, int intraPriorityValue)
{

}

bool WebURLLoaderImpl::attachThreadedDataReceiver(blink::WebThreadedDataReceiver* threadedDataReceiver)
{
    DebugBreak();
    return false;
}

void WebURLLoaderImpl::onWinINetWillBeDelete()
{
    m_loaderWinINet = nullptr;
}

}  // namespace content