
#ifndef browser_api_WindowInterface_h
#define browser_api_WindowInterface_h

namespace atom {

class WindowInterface {
public:
    virtual bool isClosed() = 0;
    virtual void close() = 0;
};

} // atom

#endif // browser_api_WindowInterface_h