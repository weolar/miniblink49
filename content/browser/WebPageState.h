
#ifndef content_WebPageState_h
#define content_WebPageState_h

namespace content {

enum WebPageState {
    pageUninited,
    pageInited,
    pagePreDestroy,
    pageDestroying,
    pageDestroyed
};
}

#endif // content_WebPageState_h