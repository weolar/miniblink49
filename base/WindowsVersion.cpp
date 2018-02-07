
#include "base/WindowsVersion.h"
#include <windows.h>

namespace base {

WindowsVersion getWindowsVersion(int* major, int* minor)
{
    static bool initialized = false;
    static WindowsVersion version;
    static int majorVersion, minorVersion;

    if (initialized)
        return version;
    initialized = true;

    OSVERSIONINFOEX versionInfo = { 0 };
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&versionInfo));
    majorVersion = versionInfo.dwMajorVersion;
    minorVersion = versionInfo.dwMinorVersion;

    if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32s)
        version = Windows3_1;
    else if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
        if (!minorVersion)
            version = Windows95;
        else
            version = (minorVersion == 10) ? Windows98 : WindowsME;
    } else {
        if (majorVersion == 5) {
            if (!minorVersion)
                version = Windows2000;
            else
                version = (minorVersion == 1) ? WindowsXP : WindowsServer2003;
        } else if (majorVersion >= 6) {
            if (minorVersion >= 2)
                version = Windows8;
            else if (versionInfo.wProductType == VER_NT_WORKSTATION)
                version = (majorVersion == 6 && !minorVersion) ? WindowsVista : Windows7;
            else
                version = WindowsServer2008;
        } else
            version = (majorVersion == 4) ? WindowsNT4 : WindowsNT3;
    }

    if (major)
        *major = majorVersion;
    if (minor)
        *minor = minorVersion;
    return version;
}

}