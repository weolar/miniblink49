
#ifndef xpapi_h
#define xpapi_h

#include <windows.h>
#include <Psapi.h>

inline BOOL GetProcessMemoryInfoXp(HANDLE Process, void* ppsmemCounters, DWORD cb)
{
    typedef BOOL(__stdcall* PFN_GetProcessMemoryInfo)(HANDLE Process, void* ppsmemCounters, DWORD cb);
    static PFN_GetProcessMemoryInfo s_GetProcessMemoryInfo = NULL;

    static BOOL s_is_init = FALSE;
    if (!s_is_init) {
        HMODULE handle = GetModuleHandle(L"Kernel32.dll");
        s_GetProcessMemoryInfo = (PFN_GetProcessMemoryInfo)GetProcAddress(handle, "GetProcessMemoryInfo");
        if (!s_GetProcessMemoryInfo) {
            handle = LoadLibraryW(L"Psapi.dll");
            s_GetProcessMemoryInfo = (PFN_GetProcessMemoryInfo)GetProcAddress(handle, "GetProcessMemoryInfo");
        }
        s_is_init = TRUE;
    }

    if (s_GetProcessMemoryInfo)
        return s_GetProcessMemoryInfo(Process, ppsmemCounters, cb);

    return FALSE;
}

#endif // xpapi_h