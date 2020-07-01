
#ifndef net_StorageMgr_h
#define net_StorageMgr_h

#include "net/StorageDef.h"

#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/StringHash.h"

namespace net {

class WebStorageAreaImpl;
class WebStorageNamespaceImpl;

class StorageMgr {
public:
    static StorageMgr* getInst()
    {
        if (!m_inst)
            m_inst = new StorageMgr();
        return m_inst;
    }

    DOMStorageMap* createOrGet(const String& fullPath);

private:
    static StorageMgr* m_inst;
    int64 m_storageNamespaceIdCount;
    HashMap<String, DOMStorageMap*> m_pathToStorageNamespace;
};

}

#endif // net_StorageMgr_h