#ifndef COMPONENTS_WEBCRYPTO_WEBCRYPTO_IMPL_H_
#define COMPONENTS_WEBCRYPTO_WEBCRYPTO_IMPL_H_

#include "third_party/WebKit/public/platform/WebCrypto.h"
#include "wtf/Vector.h"

namespace content {

class WebCryptoDigestorImpl : public blink::WebCryptoDigestor {
public:
    virtual ~WebCryptoDigestorImpl() override;
    virtual bool consume(const unsigned char* data, unsigned dataSize) override;
    virtual bool finish(unsigned char*& resultData, unsigned& resultDataSize) override;

private:
    Vector<unsigned char> m_data;
};
    
class WebCryptoImpl : public blink::WebCrypto {
public:
    WebCryptoImpl();
    virtual blink::WebCryptoDigestor* createDigestor(blink::WebCryptoAlgorithmId algorithmId) override;

private:
    WebCryptoDigestorImpl* m_webCryptoDigestorImpl;
};

}

#endif