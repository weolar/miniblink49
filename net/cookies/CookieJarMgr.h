
#ifndef net_CookieJarMgr_h
#define net_CookieJarMgr_h

#include <string>
#include <map>

namespace net {

class WebCookieJarImpl;

class CookieJarMgr {
public:
    static CookieJarMgr* getInst()
    {
        if (!m_inst)
            m_inst = new CookieJarMgr();
        return m_inst;
    }

    WebCookieJarImpl* createOrGet(const std::string& fullPath);

private:
    static CookieJarMgr* m_inst;
    std::map<std::string, WebCookieJarImpl*> m_pathToCookies;
};

}

#endif // net_CookieJarMgr_h