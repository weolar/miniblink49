
#ifndef WebFileUtilitiesImpl_h
#define WebFileUtilitiesImpl_h

#include "third_party/WebKit/public/platform/WebFileUtilities.h"

namespace content {

class WebFileUtilitiesImpl : public blink::WebFileUtilities {
public:
    WebFileUtilitiesImpl();

    // blink::WebFileUtilities
    virtual bool getFileInfo(const blink::WebString& path, blink::WebFileInfo& result) override;
    virtual blink::WebString directoryName(const blink::WebString& path) override;
    virtual blink::WebString baseName(const blink::WebString& path) override;
    virtual bool isDirectory(const blink::WebString& path) override;
    virtual blink::WebURL filePathToURL(const blink::WebString& path) override;
};

String pathGetFileName(const String& path);
bool fileExists(const String& path);
String openTemporaryFile(const String&, HANDLE& handle);
String pathByAppendingComponent(const String& path, const String& component);
int writeToFile(HANDLE handle, const char* data, int length);

}

#endif // WebFileUtilitiesImpl_h