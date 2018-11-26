
#ifndef StreamReader_h
#define StreamReader_h

#include <wtf/text/WTFString.h>

namespace net {

class AsyncStreamReader {
public:
    virtual void getSize(const String& path, double expectedModificationTime) = 0;
    virtual void openForRead(const String& path, long long offset, long long length) = 0;
    virtual void close() = 0;
    virtual void read(char* buffer, int length) = 0;
};

}

#endif // AsyncStreamReader_h