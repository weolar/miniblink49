#ifndef ActivatingLoaderCheck_h
#define ActivatingLoaderCheck_h

#include <set>
#include <windows.h>

namespace net {

class ActivatingObjCheck {
public:
    ActivatingObjCheck();
    ~ActivatingObjCheck();

    static ActivatingObjCheck* inst();

    void shutdown();
    void destroy();

    void add(intptr_t obj);
    void remove(intptr_t obj);
    bool isActivating(intptr_t obj);

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