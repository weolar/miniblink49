
#ifndef content_browser_PlatformCursor_h
#define content_browser_PlatformCursor_h

#include <windows.h>
#include "third_party/WebKit/Source/platform/win/HWndDC.h"
#include "skia/ext/platform_canvas.h"

namespace content {

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
};

// typedef struct _OSVERSIONINFOEXW {
//     DWORD dwOSVersionInfoSize;
//     DWORD dwMajorVersion;
//     DWORD dwMinorVersion;
//     DWORD dwBuildNumber;
//     DWORD dwPlatformId;
//     WCHAR  szCSDVersion[128];     // Maintenance string for PSS usage
//     WORD   wServicePackMajor;
//     WORD   wServicePackMinor;
//     WORD   wSuiteMask;
//     BYTE  wProductType;
//     BYTE  wReserved;
// } OSVERSIONINFOEXW, *POSVERSIONINFOEXW, *LPOSVERSIONINFOEXW, RTL_OSVERSIONINFOEXW, *PRTL_OSVERSIONINFOEXW;
// 
// typedef OSVERSIONINFOEXW OSVERSIONINFOEX;

// typedef struct _OSVERSIONINFOW {
//     DWORD dwOSVersionInfoSize;
//     DWORD dwMajorVersion;
//     DWORD dwMinorVersion;
//     DWORD dwBuildNumber;
//     DWORD dwPlatformId;
//     WCHAR  szCSDVersion[128];     // Maintenance string for PSS usage
// } OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW, RTL_OSVERSIONINFOW, *PRTL_OSVERSIONINFO;

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
            if (versionInfo.wProductType == VER_NT_WORKSTATION)
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

BITMAPINFO createBitmapInfoForSize(int width, int height, int bitCount)
{
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = bitCount;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    return bitmapInfo;
}

HICON createSharedCursorImpl(const blink::WebCursorInfo& cursorInfo)
{
    const SkBitmap& img = cursorInfo.customImage.getSkBitmap();
    const blink::IntPoint& hotSpot = cursorInfo.hotSpot;
    HICON impl;

    IntPoint effectiveHotSpot = hotSpot; // determineHotSpot(img, hotSpot);
    static bool doAlpha = getWindowsVersion(nullptr, nullptr);
    HWndDC dc(0);
    HDC workingDC = /*adoptGDIObject*/(::CreateCompatibleDC(dc));
    HBITMAP hCursor = nullptr;
    HBITMAP hMask = nullptr;
    HBITMAP andMask = nullptr;
    HBITMAP xorMask = nullptr;
    HDC andMaskDC = nullptr;
    HDC xorMaskDC = nullptr;

    skia::PlatformBitmap platformBitmap;
    platformBitmap.Allocate(img.width(), img.height(), false);
    const SkBitmap& tmpSkBitmap = platformBitmap.GetBitmap();
    SkCanvas canvas(tmpSkBitmap);
    canvas.drawBitmap(img, 0, 0);
    HDC tmpSkBitmapDC = platformBitmap.GetSurface();
    hCursor = (HBITMAP)::GetCurrentObject(tmpSkBitmapDC, OBJ_BITMAP);

    if (doAlpha) {
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(workingDC, hCursor);
        ::SetBkMode(workingDC, TRANSPARENT);
        ::SelectObject(workingDC, hOldBitmap);

        Vector<unsigned char, 128> maskBits;
        maskBits.fill(0xff, (img.width() + 7) / 8 * img.height());
        hMask = /*adoptGDIObject*/(::CreateBitmap(img.width(), img.height(), 1, 1, maskBits.data()));

        ICONINFO ii;
        ii.fIcon = FALSE;
        ii.xHotspot = effectiveHotSpot.x();
        ii.yHotspot = effectiveHotSpot.y();
        ii.hbmMask = hMask;
        ii.hbmColor = hCursor;

        impl = /*SharedCursor::create*/(::CreateIconIndirect(&ii));
    } else {
        // Platform doesn't support alpha blended cursors, so we need
        // to create the mask manually
        HDC andMaskDC = /*adoptGDIObject*/(::CreateCompatibleDC(dc));
        HDC xorMaskDC = /*adoptGDIObject*/(::CreateCompatibleDC(dc));

        BITMAP cursor;
        ::GetObject(hCursor, sizeof(BITMAP), &cursor);
        andMask = /*adoptGDIObject*/(::CreateBitmap(cursor.bmWidth, cursor.bmHeight, 1, 1, 0));
        xorMask = /*adoptGDIObject*/(::CreateCompatibleBitmap(dc, cursor.bmWidth, cursor.bmHeight));
        HBITMAP oldCursor = (HBITMAP)::SelectObject(workingDC, hCursor);
        HBITMAP oldAndMask = (HBITMAP)::SelectObject(andMaskDC, andMask);
        HBITMAP oldXorMask = (HBITMAP)::SelectObject(xorMaskDC, xorMask);

        ::SetBkColor(workingDC, RGB(0, 0, 0));
        ::BitBlt(andMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, workingDC, 0, 0, SRCCOPY);

        ::SetBkColor(xorMaskDC, RGB(255, 255, 255));
        ::SetTextColor(xorMaskDC, RGB(255, 255, 255));
        ::BitBlt(xorMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, andMaskDC, 0, 0, SRCCOPY);
        ::BitBlt(xorMaskDC, 0, 0, cursor.bmWidth, cursor.bmHeight, workingDC, 0, 0, SRCAND);

        ::SelectObject(workingDC, oldCursor);
        ::SelectObject(andMaskDC, oldAndMask);
        ::SelectObject(xorMaskDC, oldXorMask);

        ICONINFO icon = { 0 };
        icon.fIcon = FALSE;
        icon.xHotspot = effectiveHotSpot.x();
        icon.yHotspot = effectiveHotSpot.y();
        icon.hbmMask = andMask;
        icon.hbmColor = xorMask;
        impl = /*SharedCursor::create*/(CreateIconIndirect(&icon));
    }

    ::DeleteObject(workingDC);
    ::DeleteObject(hCursor);
    ::DeleteObject(hMask);
    ::DeleteObject(andMask);
    ::DeleteObject(xorMask);
    ::DeleteObject(andMaskDC);
    ::DeleteObject(xorMaskDC);

    return impl/*.release()*/;
}

}

#endif // content_browser_PlatformCursor_h