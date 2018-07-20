#ifndef ActivatingLoaderCheck_h
#define ActivatingLoaderCheck_h

#include <set>
#include "third_party/WebKit/Source/wtf/HashSet.h"

namespace net {

class ActivatingObjCheck {
public:
    ActivatingObjCheck();
    ~ActivatingObjCheck();

    static ActivatingObjCheck* inst();

    void shutdown();
    void destroy();

    void add(intptr_t loader);
    void remove(intptr_t loader);
    bool isActivating(intptr_t loader);

    void doGarbageCollected(bool forceGC);

    void testPrint();

    int genId();

private:
    std::set<intptr_t>* m_activatingObjs;
    CRITICAL_SECTION m_mutex;
    int m_newestId;

    static ActivatingObjCheck* m_inst;
};

} // net

#endif // ActivatingLoaderCheck_h