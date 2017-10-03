
#ifndef content_browser_CheckReEnter_h
#define content_browser_CheckReEnter_h

namespace content {

class WebPageImpl;

class CheckReEnter {
public:
    CheckReEnter(WebPageImpl* webPageImpl);

    ~CheckReEnter();

    static int s_kEnterContent;

private:
    WebPageImpl* m_webPageImpl;
};

#define CHECK_FOR_REENTER(ret) \
    if (!checkForRepeatEnter()) \
        return ret; \
    if (pageInited != m_state) \
        return ret; \
    CheckReEnter checker(this);

#define CHECK_FOR_REENTER0() \
    if (!checkForRepeatEnter()) \
        return; \
    if (pageInited != m_state) \
        return; \
    CheckReEnter checker(this);

} // content

#endif // content_browser_CheckReEnter_h