
#ifndef browser_api_WindowInterface_h
#define browser_api_WindowInterface_h

#include <v8.h>

namespace atom {

class WebContents;

class WindowInterface {
public:
    virtual bool isClosed() = 0;
    virtual void close() = 0;
    virtual v8::Local<v8::Object> getWrapper() = 0;
    virtual int getId() const = 0;
    virtual WebContents* getWebContents() const = 0;
};

} // atom

#endif // browser_api_WindowInterface_h