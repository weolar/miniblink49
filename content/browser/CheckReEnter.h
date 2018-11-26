
#ifndef content_browser_CheckReEnter_h
#define content_browser_CheckReEnter_h

namespace content {

class WebPageImpl;

class CheckReEnter {
public:
    CheckReEnter(WebPageImpl* webPageImpl);

    ~CheckReEnter();

    static void incrementEnterCount();
    static void decrementEnterCount();
    static int getEnterCount();

private:
    static int s_kEnterCount;
    WebPageImpl* m_webPageImpl;
};

#define CHECK_FOR_REENTER(self, ret) \
    if (!self->checkForRepeatEnter()) \
        return ret; \
    if (pageInited != self->m_state) \
        return ret; \
    CheckReEnter checker(self); \
    BlinkPlatformImpl::AutoDisableGC autoDisableGC;

} // content

#endif // content_browser_CheckReEnter_h