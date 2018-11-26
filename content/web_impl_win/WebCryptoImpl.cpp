
#include "content/web_impl_win/WebCryptoImpl.h"

namespace content {

WebCryptoDigestorImpl::~WebCryptoDigestorImpl() {

}

bool WebCryptoDigestorImpl::consume(const unsigned char* data, unsigned dataSize) {
    m_data.append(data, dataSize);
    return true;
}

bool WebCryptoDigestorImpl::finish(unsigned char*& resultData, unsigned& resultDataSize) {
    resultData = m_data.data();
    resultDataSize = m_data.size();
    return true;
}
    
WebCryptoImpl::WebCryptoImpl() {

}

blink::WebCryptoDigestor* WebCryptoImpl::createDigestor(blink::WebCryptoAlgorithmId algorithmId) {
    return new WebCryptoDigestorImpl();
}

}

namespace blink {
  
void WebCryptoResult::completeWithError(WebCryptoErrorType, const WebString&) 
{
    DebugBreak();
}

void WebCryptoResult::reset()
{
    DebugBreak();
}

}