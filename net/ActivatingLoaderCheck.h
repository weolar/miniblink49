#ifndef ActivatingLoaderCheck_h
#define ActivatingLoaderCheck_h

#include <set>
#include "third_party/WebKit/Source/wtf/HashSet.h"

namespace net {

class WebURLLoaderWinINet;

class ActivatingLoaderCheck {
public:
    ActivatingLoaderCheck();
    ~ActivatingLoaderCheck();

    static ActivatingLoaderCheck* inst();

    void shutdown();
    void destroy();

    void addActivatingLoader(WebURLLoaderWinINet* loader);
    void removeActivatingLoader(WebURLLoaderWinINet* loader);
    bool isActivating(WebURLLoaderWinINet* loader);

    void doGarbageCollected(bool forceGC);

    void testPrint();

private:
    std::set<WebURLLoaderWinINet*>* m_activatingLoaders;
    CRITICAL_SECTION m_mutex;

    static ActivatingLoaderCheck* m_inst;
};

extern ActivatingLoaderCheck* gActivatingLoaderCheck;

} // net

#endif // ActivatingLoaderCheck_h