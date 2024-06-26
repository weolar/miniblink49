    
#ifndef browser_api_WindowState_h
#define browser_api_WindowState_h

namespace atom {

enum WindowState {
    WindowUninited,
    WindowInited,
    WindowDestroying,
    WindowDestroyed
};

}

#endif // browser_api_WindowState_h