/* Copyright (c) 2001-2005, Microsoft Corp. All rights reserved. */

#if _MSC_VER > 1000
#pragma once
#endif

#if defined(__cplusplus)
extern "C" {
#endif


#if !defined(RC_INVOKED) /* RC complains about long symbols in #ifs */
#if defined(ISOLATION_AWARE_ENABLED) && (ISOLATION_AWARE_ENABLED != 0)


#if !defined(ISOLATION_AWARE_USE_STATIC_LIBRARY)
#define ISOLATION_AWARE_USE_STATIC_LIBRARY 0
#endif

#if !defined(ISOLATION_AWARE_BUILD_STATIC_LIBRARY)
#define ISOLATION_AWARE_BUILD_STATIC_LIBRARY 0
#endif

#if !defined(ISOLATION_AWARE_INLINE)
#if ISOLATION_AWARE_BUILD_STATIC_LIBRARY
#define ISOLATION_AWARE_INLINE /* nothing */
#else
#if defined(__cplusplus)
#define ISOLATION_AWARE_INLINE inline
#else
#define ISOLATION_AWARE_INLINE __inline
#endif
#endif
#endif

#if !ISOLATION_AWARE_USE_STATIC_LIBRARY


/* These wrappers prevent warnings about taking the addresses of __declspec(dllimport) functions. */
ISOLATION_AWARE_INLINE HMODULE WINAPI IsolationAwarePrivatezlybNQyVOeNeln(LPCSTR s) { return LoadLibraryA(s); }
ISOLATION_AWARE_INLINE HMODULE WINAPI IsolationAwarePrivatezlybNQyVOeNelJ(LPCWSTR s) { return LoadLibraryW(s); }
ISOLATION_AWARE_INLINE HMODULE WINAPI IsolationAwarePrivatezltRgzbQhYRuNaQYRn(LPCSTR s) { return GetModuleHandleA(s); }
ISOLATION_AWARE_INLINE HMODULE WINAPI IsolationAwarePrivatezltRgzbQhYRuNaQYRJ(LPCWSTR s) { return GetModuleHandleW(s); }

/* temporary support for out of sync headers */
#define IsolationAwarePrivateG_FqbjaLEiEL IsolationAwarePrivateT_SqbjaYRiRY
#define IsolationAwarePrivatenCgIiAgEzlnCgpgk IsolationAwarePrivatenPgViNgRzlnPgpgk
#define WinbaseIsolationAwarePrivateG_HnCgpgk WinbaseIsolationAwarePrivateT_UnPgpgk
#define IsolationAwarePrivatezlybADyIBeAeln IsolationAwarePrivatezlybNQyVOeNeln
#define IsolationAwarePrivatezlybADyIBeAelJ IsolationAwarePrivatezlybNQyVOeNelJ
#define IsolationAwarePrivatezltEgCebCnDDeEff IsolationAwarePrivatezltRgCebPnQQeRff

BOOL WINAPI IsolationAwarePrivatenPgViNgRzlnPgpgk(ULONG_PTR* pulpCookie);

/*
These are private.
*/
__declspec(selectany) HANDLE WinbaseIsolationAwarePrivateT_UnPgpgk = INVALID_HANDLE_VALUE;
__declspec(selectany) BOOL   IsolationAwarePrivateT_SqbjaYRiRY = FALSE;
__declspec(selectany) BOOL   WinbaseIsolationAwarePrivateT_SpeRNgRQnPgpgk = FALSE;
__declspec(selectany) BOOL   WinbaseIsolationAwarePrivateT_SpYRNahcpNYYRQ = FALSE;

FARPROC WINAPI WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY(LPCSTR pszProcName);

#endif /* ISOLATION_AWARE_USE_STATIC_LIBRARY */
__out HMODULE WINAPI IsolationAwareLoadLibraryA(__in LPCSTR lpLibFileName);
__out HMODULE WINAPI IsolationAwareLoadLibraryW(__in LPCWSTR lpLibFileName);
__out HMODULE WINAPI IsolationAwareLoadLibraryExA(__in LPCSTR lpLibFileName,__reserved HANDLE hFile,__in DWORD dwFlags);
__out HMODULE WINAPI IsolationAwareLoadLibraryExW(__in LPCWSTR lpLibFileName,__reserved HANDLE hFile,__in DWORD dwFlags);
__out HANDLE WINAPI IsolationAwareCreateActCtxW(__in PCACTCTXW pActCtx);
void WINAPI IsolationAwareReleaseActCtx(__inout HANDLE hActCtx);
BOOL WINAPI IsolationAwareActivateActCtx(__inout HANDLE hActCtx,__out ULONG_PTR*lpCookie);
BOOL WINAPI IsolationAwareDeactivateActCtx(__in DWORD dwFlags,__in ULONG_PTR ulCookie);
BOOL WINAPI IsolationAwareFindActCtxSectionStringW(__in DWORD dwFlags,__reserved const GUID*lpExtensionGuid,__in ULONG ulSectionId,__in LPCWSTR lpStringToFind,__out PACTCTX_SECTION_KEYED_DATA ReturnedData);
BOOL WINAPI IsolationAwareQueryActCtxW(__in DWORD dwFlags,__in HANDLE hActCtx,__in_opt PVOID pvSubInstance,__in ULONG ulInfoClass,__out_bcount_part_opt(cbBuffer,*pcbWrittenOrRequired) PVOID pvBuffer,__in SIZE_T cbBuffer,__out_opt SIZE_T*pcbWrittenOrRequired);

#if defined(UNICODE)

#define IsolationAwareLoadLibrary IsolationAwareLoadLibraryW
#define IsolationAwareLoadLibraryEx IsolationAwareLoadLibraryExW

#else /* UNICODE */

#define IsolationAwareLoadLibrary IsolationAwareLoadLibraryA
#define IsolationAwareLoadLibraryEx IsolationAwareLoadLibraryExA

#endif /* UNICODE */

#if !ISOLATION_AWARE_USE_STATIC_LIBRARY
ISOLATION_AWARE_INLINE __out HMODULE WINAPI IsolationAwareLoadLibraryA(__in LPCSTR lpLibFileName)
{
    __out HMODULE result = NULL;
    ULONG_PTR  ulpCookie = 0;
    const BOOL fActivateActCtxSuccess = IsolationAwarePrivateT_SqbjaYRiRY || IsolationAwarePrivatenPgViNgRzlnPgpgk(&ulpCookie);
    if (!fActivateActCtxSuccess)
        return result;
    __try
    {
        result = LoadLibraryA(lpLibFileName);
    }
    __finally
    {
        if (!IsolationAwarePrivateT_SqbjaYRiRY)
        {
            const BOOL fPreserveLastError = (result == NULL);
            const DWORD dwLastError = fPreserveLastError ? GetLastError() : NO_ERROR;
            (void)IsolationAwareDeactivateActCtx(0, ulpCookie);
            if (fPreserveLastError)
                SetLastError(dwLastError);
        }
    }
    return result;
}

ISOLATION_AWARE_INLINE __out HMODULE WINAPI IsolationAwareLoadLibraryW(__in LPCWSTR lpLibFileName)
{
    __out HMODULE result = NULL;
    ULONG_PTR  ulpCookie = 0;
    const BOOL fActivateActCtxSuccess = IsolationAwarePrivateT_SqbjaYRiRY || IsolationAwarePrivatenPgViNgRzlnPgpgk(&ulpCookie);
    if (!fActivateActCtxSuccess)
        return result;
    __try
    {
        result = LoadLibraryW(lpLibFileName);
    }
    __finally
    {
        if (!IsolationAwarePrivateT_SqbjaYRiRY)
        {
            const BOOL fPreserveLastError = (result == NULL);
            const DWORD dwLastError = fPreserveLastError ? GetLastError() : NO_ERROR;
            (void)IsolationAwareDeactivateActCtx(0, ulpCookie);
            if (fPreserveLastError)
                SetLastError(dwLastError);
        }
    }
    return result;
}

ISOLATION_AWARE_INLINE __out HMODULE WINAPI IsolationAwareLoadLibraryExA(__in LPCSTR lpLibFileName,__reserved HANDLE hFile,__in DWORD dwFlags)
{
    __out HMODULE result = NULL;
    ULONG_PTR  ulpCookie = 0;
    const BOOL fActivateActCtxSuccess = IsolationAwarePrivateT_SqbjaYRiRY || IsolationAwarePrivatenPgViNgRzlnPgpgk(&ulpCookie);
    if (!fActivateActCtxSuccess)
        return result;
    __try
    {
        result = LoadLibraryExA(lpLibFileName,hFile,dwFlags);
    }
    __finally
    {
        if (!IsolationAwarePrivateT_SqbjaYRiRY)
        {
            const BOOL fPreserveLastError = (result == NULL);
            const DWORD dwLastError = fPreserveLastError ? GetLastError() : NO_ERROR;
            (void)IsolationAwareDeactivateActCtx(0, ulpCookie);
            if (fPreserveLastError)
                SetLastError(dwLastError);
        }
    }
    return result;
}

ISOLATION_AWARE_INLINE __out HMODULE WINAPI IsolationAwareLoadLibraryExW(__in LPCWSTR lpLibFileName,__reserved HANDLE hFile,__in DWORD dwFlags)
{
    __out HMODULE result = NULL;
    ULONG_PTR  ulpCookie = 0;
    const BOOL fActivateActCtxSuccess = IsolationAwarePrivateT_SqbjaYRiRY || IsolationAwarePrivatenPgViNgRzlnPgpgk(&ulpCookie);
    if (!fActivateActCtxSuccess)
        return result;
    __try
    {
        result = LoadLibraryExW(lpLibFileName,hFile,dwFlags);
    }
    __finally
    {
        if (!IsolationAwarePrivateT_SqbjaYRiRY)
        {
            const BOOL fPreserveLastError = (result == NULL);
            const DWORD dwLastError = fPreserveLastError ? GetLastError() : NO_ERROR;
            (void)IsolationAwareDeactivateActCtx(0, ulpCookie);
            if (fPreserveLastError)
                SetLastError(dwLastError);
        }
    }
    return result;
}

ISOLATION_AWARE_INLINE __out HANDLE WINAPI IsolationAwareCreateActCtxW(__in PCACTCTXW pActCtx)
{
    __out HANDLE result = INVALID_HANDLE_VALUE;
    typedef __out HANDLE (WINAPI* PFN)(__in PCACTCTXW pActCtx);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("CreateActCtxW");
        if (s_pfn == NULL)
            return result;
    }
    result = s_pfn(pActCtx);
    return result;
}

ISOLATION_AWARE_INLINE void WINAPI IsolationAwareReleaseActCtx(__inout HANDLE hActCtx)
{
    typedef void (WINAPI* PFN)(__inout HANDLE hActCtx);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("ReleaseActCtx");
        if (s_pfn == NULL)
            return;
    }
    s_pfn(hActCtx);
    return;
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwareActivateActCtx(__inout HANDLE hActCtx,__out ULONG_PTR*lpCookie)
{
    BOOL fResult = FALSE;
    typedef BOOL (WINAPI* PFN)(__inout HANDLE hActCtx,__out ULONG_PTR*lpCookie);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("ActivateActCtx");
        if (s_pfn == NULL)
            return fResult;
    }
    fResult = s_pfn(hActCtx,lpCookie);
    return fResult;
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwareDeactivateActCtx(__in DWORD dwFlags,__in ULONG_PTR ulCookie)
{
    BOOL fResult = FALSE;
    typedef BOOL (WINAPI* PFN)(__in DWORD dwFlags,__in ULONG_PTR ulCookie);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("DeactivateActCtx");
        if (s_pfn == NULL)
            return fResult;
    }
    fResult = s_pfn(dwFlags,ulCookie);
    return fResult;
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwareFindActCtxSectionStringW(__in DWORD dwFlags,__reserved const GUID*lpExtensionGuid,__in ULONG ulSectionId,__in LPCWSTR lpStringToFind,__out PACTCTX_SECTION_KEYED_DATA ReturnedData)
{
    BOOL fResult = FALSE;
    typedef BOOL (WINAPI* PFN)(__in DWORD dwFlags,__reserved const GUID*lpExtensionGuid,__in ULONG ulSectionId,__in LPCWSTR lpStringToFind,__out PACTCTX_SECTION_KEYED_DATA ReturnedData);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("FindActCtxSectionStringW");
        if (s_pfn == NULL)
            return fResult;
    }
    fResult = s_pfn(dwFlags,lpExtensionGuid,ulSectionId,lpStringToFind,ReturnedData);
    return fResult;
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwareQueryActCtxW(__in DWORD dwFlags,__in HANDLE hActCtx,__in_opt PVOID pvSubInstance,__in ULONG ulInfoClass,__out_bcount_part_opt(cbBuffer,*pcbWrittenOrRequired) PVOID pvBuffer,__in SIZE_T cbBuffer,__out_opt SIZE_T*pcbWrittenOrRequired)
{
    BOOL fResult = FALSE;
    typedef BOOL (WINAPI* PFN)(__in DWORD dwFlags,__in HANDLE hActCtx,__in_opt PVOID pvSubInstance,__in ULONG ulInfoClass,__out_bcount_part_opt(cbBuffer,*pcbWrittenOrRequired) PVOID pvBuffer,__in SIZE_T cbBuffer,__out_opt SIZE_T*pcbWrittenOrRequired);
    static PFN s_pfn;
    if (s_pfn == NULL)
    {
        s_pfn = (PFN)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("QueryActCtxW");
        if (s_pfn == NULL)
            return fResult;
    }
    fResult = s_pfn(dwFlags,hActCtx,pvSubInstance,ulInfoClass,pvBuffer,cbBuffer,pcbWrittenOrRequired);
    return fResult;
}



#define WINBASE_NUMBER_OF(x) (sizeof(x) / sizeof((x)[0]))

typedef struct IsolationAwarePrivate_pBAFGnAG_zBqHyr_vAsB {
    HMODULE (WINAPI * WinbaseIsolationAwarePrivateybNQn)(LPCSTR a);
    HMODULE (WINAPI * WinbaseIsolationAwarePrivateybNQJ)(LPCWSTR w);
    PCSTR  WinbaseIsolationAwarePrivateANZRn;
    PCWSTR WinbaseIsolationAwarePrivateANZRJ;
} IsolationAwarePrivatepBAFGnAG_zBqHyr_vAsB;
typedef const IsolationAwarePrivatepBAFGnAG_zBqHyr_vAsB *IsolationAwarePrivateCpBAFGnAG_zBqHyr_vAsB;

typedef struct IsolationAwarePrivate_zHGnoyr_zBqHyr_vAsB {
    HMODULE WinbaseIsolationAwarePrivateybNQRQzbQhYR;
} IsolationAwarePrivatezHGnoyr_zBqHyr_vAsB, *IsolationAwarePrivateCzHGnoyr_zBqHyr_vAsB;

ISOLATION_AWARE_INLINE FARPROC WINAPI
IsolationAwarePrivatezltRgCebPnQQeRff(
    IsolationAwarePrivateCpBAFGnAG_zBqHyr_vAsB c,
    IsolationAwarePrivateCzHGnoyr_zBqHyr_vAsB m,
    LPCSTR ProcName
    )
{
    static HMODULE s_moduleUnicows;
    static BOOL s_fUnicowsInitialized;
    FARPROC Proc = NULL;
    HMODULE hModule;

    /*
       get unicows.dll loaded on-demand
    */
    if (!s_fUnicowsInitialized)
    {
        if ((GetVersion() & 0x80000000) != 0)
        {
            GetFileAttributesW(L"???.???");
            s_moduleUnicows = GetModuleHandleA("Unicows.dll");
        }
        s_fUnicowsInitialized = TRUE;
    }

    /*
       always call GetProcAddress(unicows) before the usual .dll
    */
    if (s_moduleUnicows != NULL)
    {
        Proc = GetProcAddress(s_moduleUnicows, ProcName);
        if (Proc != NULL)
            goto Exit;
    }

    hModule = m->WinbaseIsolationAwarePrivateybNQRQzbQhYR;
    if (hModule == NULL)
    {
        hModule = (((GetVersion() & 0x80000000) != 0) ? (*c->WinbaseIsolationAwarePrivateybNQn)(c->WinbaseIsolationAwarePrivateANZRn) : (*c->WinbaseIsolationAwarePrivateybNQJ)(c->WinbaseIsolationAwarePrivateANZRJ));
        if (hModule == NULL)
            goto Exit;
        m->WinbaseIsolationAwarePrivateybNQRQzbQhYR = hModule;
    }
    Proc = GetProcAddress(hModule, ProcName);
Exit:
    return Proc;
}

ISOLATION_AWARE_INLINE BOOL WINAPI WinbaseIsolationAwarePrivatetRgzlnPgpgk(void)
/*
The correctness of this function depends on it being statically
linked into its clients.

This function is private to functions present in this header.
Do not use it.
*/
{
    BOOL fResult = FALSE;
    ACTIVATION_CONTEXT_BASIC_INFORMATION actCtxBasicInfo;
    ULONG_PTR ulpCookie = 0;

    if (IsolationAwarePrivateT_SqbjaYRiRY)
    {
        fResult = TRUE;
        goto Exit;
    }

    if (WinbaseIsolationAwarePrivateT_UnPgpgk != INVALID_HANDLE_VALUE)
    {
        fResult = TRUE;
        goto Exit;
    }

    if (!IsolationAwareQueryActCtxW(
        QUERY_ACTCTX_FLAG_ACTCTX_IS_ADDRESS
        | QUERY_ACTCTX_FLAG_NO_ADDREF,
        &WinbaseIsolationAwarePrivateT_UnPgpgk,
        NULL,
        ActivationContextBasicInformation,
        &actCtxBasicInfo,
        sizeof(actCtxBasicInfo),
        NULL
        ))
        goto Exit;

    /*
    If QueryActCtxW returns NULL, try CreateActCtx(3).
    */
    if (actCtxBasicInfo.hActCtx == NULL)
    {
        ACTCTXW actCtx;
        WCHAR rgchFullModulePath[MAX_PATH + 2];
        DWORD dw;
        HMODULE hmodSelf;
        PGET_MODULE_HANDLE_EXW pfnGetModuleHandleExW;

        pfnGetModuleHandleExW = (PGET_MODULE_HANDLE_EXW)WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY("GetModuleHandleExW");
        if (pfnGetModuleHandleExW == NULL)
            goto Exit;

        if (!(*pfnGetModuleHandleExW)(
                  GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT
                | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                (LPCWSTR)&WinbaseIsolationAwarePrivateT_UnPgpgk,
                &hmodSelf
                ))
            goto Exit;

        rgchFullModulePath[WINBASE_NUMBER_OF(rgchFullModulePath) - 1] = 0;
        rgchFullModulePath[WINBASE_NUMBER_OF(rgchFullModulePath) - 2] = 0;
        dw = GetModuleFileNameW(hmodSelf, rgchFullModulePath, WINBASE_NUMBER_OF(rgchFullModulePath)-1);
        if (dw == 0)
            goto Exit;
        if (rgchFullModulePath[WINBASE_NUMBER_OF(rgchFullModulePath) - 2] != 0)
        {
            SetLastError(ERROR_BUFFER_OVERFLOW);
            goto Exit;
        }

        actCtx.cbSize = sizeof(actCtx);
        actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
        actCtx.lpSource = rgchFullModulePath;
        actCtx.lpResourceName = (LPCWSTR)(ULONG_PTR)3;
        actCtx.hModule = hmodSelf;
        actCtxBasicInfo.hActCtx = IsolationAwareCreateActCtxW(&actCtx);
        if (actCtxBasicInfo.hActCtx == INVALID_HANDLE_VALUE)
        {
            const DWORD dwLastError = GetLastError();
            if ((dwLastError != ERROR_RESOURCE_DATA_NOT_FOUND) &&
                (dwLastError != ERROR_RESOURCE_TYPE_NOT_FOUND) &&
                (dwLastError != ERROR_RESOURCE_LANG_NOT_FOUND) &&
                (dwLastError != ERROR_RESOURCE_NAME_NOT_FOUND))
                goto Exit;

            actCtxBasicInfo.hActCtx = NULL;
        }

        WinbaseIsolationAwarePrivateT_SpeRNgRQnPgpgk = TRUE;
    }

    WinbaseIsolationAwarePrivateT_UnPgpgk = actCtxBasicInfo.hActCtx;

#define ACTIVATION_CONTEXT_SECTION_DLL_REDIRECTION              (2)

    if (IsolationAwareActivateActCtx(actCtxBasicInfo.hActCtx, &ulpCookie))
    {
        __try
        {
            ACTCTX_SECTION_KEYED_DATA actCtxSectionKeyedData;

            actCtxSectionKeyedData.cbSize = sizeof(actCtxSectionKeyedData);
            if (IsolationAwareFindActCtxSectionStringW(0, NULL, ACTIVATION_CONTEXT_SECTION_DLL_REDIRECTION, L"Comctl32.dll", &actCtxSectionKeyedData))
            {
                /* get button, edit, etc. registered */
                LoadLibraryW(L"Comctl32.dll");
            }
        }
        __finally
        {
            IsolationAwareDeactivateActCtx(0, ulpCookie);
        }
    }

    fResult = TRUE;
Exit:
    return fResult;
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwareInit(void)
/*
The correctness of this function depends on it being statically
linked into its clients.

Call this from DllMain(DLL_PROCESS_ATTACH) if you use id 3 and wish to avoid a race condition that
    can cause an hActCtx leak.
Call this from your .exe's initialization if you use id 3 and wish to avoid a race condition that
    can cause an hActCtx leak.
If you use id 2, this function fetches data from your .dll
    that you do not need to worry about cleaning up.
*/
{
    return WinbaseIsolationAwarePrivatetRgzlnPgpgk();
}

ISOLATION_AWARE_INLINE void WINAPI IsolationAwareCleanup(void)
/*
Call this from DllMain(DLL_PROCESS_DETACH), if you use id 3, to avoid a leak.
Call this from your .exe's cleanup to possibly avoid apparent (but not actual) leaks, if use id 3.
This function does nothing, safely, if you use id 2.
*/
{
    HANDLE hActCtx;

    if (WinbaseIsolationAwarePrivateT_SpYRNahcpNYYRQ)
        return;

    /* IsolationAware* calls made from here on out will OutputDebugString
       and use the process default activation context instead of id 3 or will
       continue to successfully use id 2 (but still OutputDebugString).
    */
    WinbaseIsolationAwarePrivateT_SpYRNahcpNYYRQ = TRUE;
    
    /* There is no cleanup to do if we did not CreateActCtx but only called QueryActCtx.
    */
    if (!WinbaseIsolationAwarePrivateT_SpeRNgRQnPgpgk)
        return;

    hActCtx = WinbaseIsolationAwarePrivateT_UnPgpgk;
    WinbaseIsolationAwarePrivateT_UnPgpgk = NULL; /* process default */

    if (hActCtx == INVALID_HANDLE_VALUE)
        return;
    if (hActCtx == NULL)
        return;
    IsolationAwareReleaseActCtx(hActCtx);
}

ISOLATION_AWARE_INLINE BOOL WINAPI IsolationAwarePrivatenPgViNgRzlnPgpgk(ULONG_PTR* pulpCookie)
/*
This function is private to functions present in this header and other headers.
*/
{
    BOOL fResult = FALSE;

    if (WinbaseIsolationAwarePrivateT_SpYRNahcpNYYRQ)
    {
        const static char debugString[] = "IsolationAware function called after IsolationAwareCleanup\n";
        OutputDebugStringA(debugString);
    }

    if (IsolationAwarePrivateT_SqbjaYRiRY)
    {
        fResult = TRUE;
        goto Exit;
    }

    /* Do not call Init if Cleanup has been called. */
    if (!WinbaseIsolationAwarePrivateT_SpYRNahcpNYYRQ)
    {
        if (!WinbaseIsolationAwarePrivatetRgzlnPgpgk())
            goto Exit;
    }
    /* If Cleanup has been called and id3 was in use, this will activate NULL. */
    if (!IsolationAwareActivateActCtx(WinbaseIsolationAwarePrivateT_UnPgpgk, pulpCookie))
        goto Exit;

    fResult = TRUE;
Exit:
    if (!fResult)
    {
        const DWORD dwLastError = GetLastError();
        if (dwLastError == ERROR_PROC_NOT_FOUND
            || dwLastError == ERROR_MOD_NOT_FOUND
            || dwLastError == ERROR_CALL_NOT_IMPLEMENTED
            )
        {
            IsolationAwarePrivateT_SqbjaYRiRY = TRUE;
            fResult = TRUE;
        }
    }
    return fResult;
}

#undef WINBASE_NUMBER_OF

ISOLATION_AWARE_INLINE FARPROC WINAPI WinbaseIsolationAwarePrivatetRgCebPnQQeRff_xReaRYQP_QYY(LPCSTR pszProcName)
/* This function is shared by the other stubs in this header. */
{
    static HMODULE s_module;
    /* Use GetModuleHandle instead of LoadLibrary on kernel32.dll because */
    /* we already necessarily have a reference on kernel32.dll. */
    const static IsolationAwarePrivatepBAFGnAG_zBqHyr_vAsB
        c = { IsolationAwarePrivatezltRgzbQhYRuNaQYRn, IsolationAwarePrivatezltRgzbQhYRuNaQYRJ, "Kernel32.dll", L"Kernel32.dll" };
    static IsolationAwarePrivatezHGnoyr_zBqHyr_vAsB m;

    return IsolationAwarePrivatezltRgCebPnQQeRff(&c, &m, pszProcName);
}

#endif /* ISOLATION_AWARE_USE_STATIC_LIBRARY */

#define ActivateActCtx IsolationAwareActivateActCtx
#define CreateActCtxW IsolationAwareCreateActCtxW
#define DeactivateActCtx IsolationAwareDeactivateActCtx
#define FindActCtxSectionStringW IsolationAwareFindActCtxSectionStringW
#define LoadLibraryA IsolationAwareLoadLibraryA
#define LoadLibraryExA IsolationAwareLoadLibraryExA
#define LoadLibraryExW IsolationAwareLoadLibraryExW
#define LoadLibraryW IsolationAwareLoadLibraryW
#define QueryActCtxW IsolationAwareQueryActCtxW
#define ReleaseActCtx IsolationAwareReleaseActCtx

#endif /* ISOLATION_AWARE_ENABLED */
#endif /* RC */


#if defined(__cplusplus)
} /* __cplusplus */
#endif

