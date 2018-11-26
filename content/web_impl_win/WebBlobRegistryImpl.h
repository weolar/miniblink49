
#ifndef WebBlobRegistryImpl_h
#define WebBlobRegistryImpl_h

#include "third_party/WebKit/public/platform/WebBlobRegistry.h"
#include "third_party/WebKit/public/platform/WebBlobData.h"
#include "third_party/WebKit/Source/wtf/HashMap.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/WebKit/Source/wtf/text/StringHash.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"

namespace blink {
class BlobData;
}

namespace net {
class BlobDataWrap;
}

namespace content {

class WebBlobRegistryImpl : public blink::WebBlobRegistry {
public:
    WebBlobRegistryImpl();

    class BuilderImpl : public blink::WebBlobRegistry::Builder {
    public:
        BuilderImpl();
        virtual ~BuilderImpl() override;
        virtual void appendData(const blink::WebThreadSafeData&)override;
        virtual void appendFile(const blink::WebString& path, uint64_t offset, uint64_t length, double expectedModificationTime) override;
        virtual void appendBlob(const blink::WebString& uuid, uint64_t offset, uint64_t length) override;
        virtual void appendFileSystemURL(const blink::WebURL&, uint64_t offset, uint64_t length, double expectedModificationTime) override;
        virtual void build() override;
    };

    virtual ~WebBlobRegistryImpl() override;

    void setBlobDataLengthByTempPath(const WTF::String& url, size_t length) const;
    net::BlobDataWrap* getBlobDataFromUUID(const String& url) const;

    virtual void registerBlobData(const blink::WebString& uuid, const blink::WebBlobData& data) override;

    virtual Builder* createBuilder(const blink::WebString& uuid, const blink::WebString& contentType) override;

    virtual void addBlobDataRef(const blink::WebString& uuid) override;
    virtual void removeBlobDataRef(const blink::WebString& uuid) override;
    virtual void registerPublicBlobURL(const blink::WebURL& url, const blink::WebString& uuid) override;
    virtual void revokePublicBlobURL(const blink::WebURL& url) override;

    virtual void registerStreamURL(const blink::WebURL&, const blink::WebString&) override;
    virtual void registerStreamURL(const blink::WebURL&, const blink::WebURL& srcURL) override;
    virtual void addDataToStream(const blink::WebURL&, const char* data, size_t length) override;
    virtual void flushStream(const blink::WebURL&) override;
    virtual void finalizeStream(const blink::WebURL&) override;
    virtual void abortStream(const blink::WebURL&) override;
    virtual void unregisterStreamURL(const blink::WebURL&) override;

    void check() const;

private:
    HashMap<String, net::BlobDataWrap*> m_datasSet;
    mutable RecursiveMutex m_lock;
};

} // namespace content

#endif // WebBlobRegistryImpl_h
