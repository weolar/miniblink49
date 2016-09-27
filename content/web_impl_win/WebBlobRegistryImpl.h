
#ifndef WebBlobRegistryImpl_h
#define WebBlobRegistryImpl_h

#include "third_party\WebKit\public\platform\WebBlobRegistry.h"

using namespace blink;

namespace content {

class WebBlobRegistryImpl : public blink::WebBlobRegistry {
public:
    class BuilderImpl : public blink::WebBlobRegistry::Builder {
    public:
        BuilderImpl();
        virtual ~BuilderImpl() override;
        virtual void appendData(const WebThreadSafeData&)override;
        virtual void appendFile(const WebString& path, uint64_t offset, uint64_t length, double expectedModificationTime) override;
        virtual void appendBlob(const WebString& uuid, uint64_t offset, uint64_t length) override;
        virtual void appendFileSystemURL(const WebURL&, uint64_t offset, uint64_t length, double expectedModificationTime) override;
        virtual void build() override;
    };

    virtual ~WebBlobRegistryImpl() override;

    virtual void registerBlobData(const WebString& uuid, const WebBlobData&) override;

    virtual Builder* createBuilder(const WebString& uuid, const WebString& contentType) override;

    virtual void addBlobDataRef(const WebString& uuid) override;
    virtual void removeBlobDataRef(const WebString& uuid) override;
    virtual void registerPublicBlobURL(const WebURL&, const WebString& uuid) override;
    virtual void revokePublicBlobURL(const WebURL&) override;

    virtual void registerStreamURL(const WebURL&, const WebString&) override;

    virtual void registerStreamURL(const WebURL&, const WebURL& srcURL) override;

    virtual void addDataToStream(const WebURL&, const char* data, size_t length) override;

    virtual void flushStream(const WebURL&) override;

    virtual void finalizeStream(const WebURL&) override;

    virtual void abortStream(const WebURL&) override;

    virtual void unregisterStreamURL(const WebURL&) override;
};

} // namespace content

#endif // WebBlobRegistryImpl_h
