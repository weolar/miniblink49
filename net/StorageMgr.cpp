
#include "net/StorageMgr.h"

namespace net {

StorageMgr* StorageMgr::m_inst = nullptr;

DOMStorageMap* StorageMgr::createOrGet(const String& fullPath)
{
    RELEASE_ASSERT(!fullPath.is8Bit());
    WebStorageNamespaceImpl* storageArea = nullptr;
    HashMap<String, DOMStorageMap*>::iterator it = m_pathToStorageNamespace.find(fullPath);
    if (m_pathToStorageNamespace.end() != it)
        return it->value;
    
    DOMStorageMap* storageMap = new DOMStorageMap();
    m_pathToStorageNamespace.add(fullPath, storageMap);
    return storageMap;
}
    
}