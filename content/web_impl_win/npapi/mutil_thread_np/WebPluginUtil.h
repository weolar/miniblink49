
#ifndef content_web_impl_win_npapi_WebPluginUtil_h
#define content_web_impl_win_npapi_WebPluginUtil_h

#include <set>

namespace blink {
class WebThreadSupportingGC;
}

namespace content {

class WebPluginUtil {
public:
    void postTaskToNpThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure);
    void postTaskToBlinkThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure);
    void sendTaskToNpThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure);
    void sendTaskToBlinkThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure);

    int genId();

    bool isLive(int id);
    void addId(int id);
    void removeId(int id);

    static WebPluginUtil* get()
    {
        if (!m_inst)
            m_inst = new WebPluginUtil();
        return m_inst;
    }

private:
    WebPluginUtil();

    int m_idGen;
    std::set<int> m_liveSet;
    blink::WebThreadSupportingGC* m_thread; // np插件运行在独立线程
    CRITICAL_SECTION m_lock;

    static WebPluginUtil* m_inst;
};

}

#endif // content_web_impl_win_npapi_WebPluginUtil_h