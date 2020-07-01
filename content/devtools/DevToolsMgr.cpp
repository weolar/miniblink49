
#include "content/devtools/DevToolsMgr.h"

namespace content {

DevToolsMgr* DevToolsMgr::m_inst = nullptr;

DevToolsMgr::DevToolsMgr()
{
    m_idGen = 0;
}

DevToolsMgr* DevToolsMgr::getInst()
{
    if (!m_inst)
        m_inst = new DevToolsMgr();
    return m_inst;
}

void DevToolsMgr::addLivedId(int id)
{
    m_ids.add(id);
}

void DevToolsMgr::removeLivedId(int id)
{
    m_ids.remove(id);
}

bool DevToolsMgr::isLivedId(int id) const
{
    return m_ids.contains(id);
}

int DevToolsMgr::getNewestId()
{
    return ++m_idGen;
}


}