
#ifndef browser_api_MenuUitl_h
#define browser_api_MenuUitl_h

#include <windows.h>

namespace atom {

class MenuItem;

class MenuUitl {
public:
    static void onMenuCommon(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

}

#endif 