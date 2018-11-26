
#ifndef base_WindowsVersion_h
#define base_WindowsVersion_h

namespace base {

// NOTE: Keep these in order so callers can do things like
// "if (windowsVersion() >= WindowsVista) ...". It's OK to change or add values,
// though.
enum WindowsVersion {
    // CE-based versions
    WindowsCE1 = 0,
    WindowsCE2,
    WindowsCE3,
    WindowsCE4,
    WindowsCE5,
    WindowsCE6,
    WindowsCE7,
    // 3.x-based versions
    Windows3_1,
    // 9x-based versions
    Windows95,
    Windows98,
    WindowsME,
    // NT-based versions
    WindowsNT3,
    WindowsNT4,
    Windows2000,
    WindowsXP,
    WindowsServer2003,
    WindowsVista,
    WindowsServer2008,
    Windows7,
    Windows8
};

WindowsVersion getWindowsVersion(int* major, int* minor);
 
}

#endif