
#ifndef content_browser_TouchStruct_h
#define content_browser_TouchStruct_h

#ifndef WM_TOUCH
#define WM_TOUCH 0x0240

#define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004
#define TOUCHEVENTF_INRANGE         0x0008
#define TOUCHEVENTF_PRIMARY         0x0010
#define TOUCHEVENTF_NOCOALESCE      0x0020
#define TOUCHEVENTF_PEN             0x0040
#define TOUCHEVENTF_PALM            0x0080

/*
* Touch input mask values (TOUCHINPUT.dwMask)
*/
#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001  // the dwTime field contains a system generated value
#define TOUCHINPUTMASKF_EXTRAINFO       0x0002  // the dwExtraInfo field is valid
#define TOUCHINPUTMASKF_CONTACTAREA     0x0004  // the cxContact and cyContact fields are valid

typedef HANDLE HTOUCHINPUT;

typedef struct tagTOUCHINPUT {
    LONG x;
    LONG y;
    HANDLE hSource;
    DWORD dwID;
    DWORD dwFlags;
    DWORD dwMask;
    DWORD dwTime;
    ULONG_PTR dwExtraInfo;
    DWORD cxContact;
    DWORD cyContact;
} TOUCHINPUT, *PTOUCHINPUT;
typedef TOUCHINPUT const * PCTOUCHINPUT;

#endif

inline BOOL RegisterTouchWindowXp(HWND hwnd, ULONG ulFlags)
{
    typedef BOOL(__stdcall* PFN_RegisterTouchWindow)(HWND hwnd, ULONG ulFlags);
    static PFN_RegisterTouchWindow s_RegisterTouchWindow = NULL;
    static BOOL s_isInit = FALSE;
    if (!s_isInit) {
        HMODULE handle = GetModuleHandle(L"User32.dll");
        s_RegisterTouchWindow = (PFN_RegisterTouchWindow)GetProcAddress(handle, "RegisterTouchWindow");
        s_isInit = TRUE;
    }

    if (s_RegisterTouchWindow)
        return s_RegisterTouchWindow(hwnd, ulFlags);

    return FALSE;
}

inline BOOL GetTouchInputInfoXp(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize)
{
    typedef BOOL(__stdcall* PFN_GetTouchInputInfo)(HTOUCHINPUT hTouchInput, UINT cInputs, PTOUCHINPUT pInputs, int cbSize);
    static PFN_GetTouchInputInfo s_GetTouchInputInfo = NULL;
    static BOOL s_isInit = FALSE;
    if (!s_isInit) {
        HMODULE handle = GetModuleHandle(L"User32.dll");
        s_GetTouchInputInfo = (PFN_GetTouchInputInfo)GetProcAddress(handle, "GetTouchInputInfo");
        s_isInit = TRUE;
    }

    if (s_GetTouchInputInfo)
        return s_GetTouchInputInfo(hTouchInput, cInputs, pInputs, cbSize);

    return FALSE;
}

inline BOOL CloseTouchInputHandleXp(HTOUCHINPUT hTouchInput)
{
    typedef BOOL(__stdcall* PFN_CloseTouchInputHandle)(HTOUCHINPUT hTouchInput);
    static PFN_CloseTouchInputHandle s_CloseTouchInputHandle = NULL;
    static BOOL s_isInit = FALSE;
    if (!s_isInit) {
        HMODULE handle = GetModuleHandle(L"User32.dll");
        s_CloseTouchInputHandle = (PFN_CloseTouchInputHandle)GetProcAddress(handle, "CloseTouchInputHandle");
        s_isInit = TRUE;
    }

    if (s_CloseTouchInputHandle)
        return s_CloseTouchInputHandle(hTouchInput);

    return FALSE;
}

#endif // content_browser_TouchStruct_h
