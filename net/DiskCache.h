
#ifndef net_DiskCache_h
#define net_DiskCache_h

#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "net/FileSystem.h"
#include <vector>

namespace blink {
class KURL;
}

namespace base {
class ListValue;
}

namespace net {

struct DiskCacheItem {
    std::vector<char> content;
    String mime;
};

class DiskCache {
public:
    DiskCache();
    ~DiskCache();

    void saveMemoryCache();
    void initFromJsonFile();
    DiskCacheItem* getCacheUrlItem(const blink::KURL& kurl);

private:
    base::ListValue* m_cacheList;
};

}

#endif // net_DiskCache_h