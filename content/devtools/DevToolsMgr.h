#ifndef CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_MGR_H_
#define CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_MGR_H_

#include "third_party/WebKit/Source/wtf/HashSet.h"

namespace content {

class DevToolsMgr {
public:
    DevToolsMgr();
    static DevToolsMgr* getInst();

    void addLivedId(int id);
    void removeLivedId(int id);
    bool isLivedId(int id) const;
    int getNewestId();

private:
    static DevToolsMgr* m_inst;
    WTF::HashSet<int> m_ids;
    int m_idGen;
};

}

#endif // CONTENT_RENDERER_DEVTOOLS_DEVTOOLS_MGR_H_