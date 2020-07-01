#ifndef base_windowsvc6_h
#define base_windowsvc6_h

#if USING_VC6RT == 1

#ifndef _WIN32_WINNT
#define _WIN32_WINNT (0x0501)
#endif

#include <commctrl.h>

#undef min
#undef max
#undef PURE

#ifndef _In_opt_
#define _In_opt_ 
#endif

#ifndef _In_
#define _In_
#endif

#ifndef _Inout_opt_
#define _Inout_opt_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _In_opt_z_
#define _In_opt_z_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#define WS_EX_LAYERED 0x00080000
#define SPI_GETWHEELSCROLLLINES 0x0068
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#define WHEEL_DELTA 120
#define WHEEL_PAGESCROLL (UINT_MAX)
#define CS_DROPSHADOW 0x00020000
#define TME_LEAVE 0x00000002
#define TME_HOVER 0x00000001
#define WM_MOUSEWHEEL 0x020A
#define SPI_GETWHEELSCROLLCHARS 0x006C
#define ULW_ALPHA 0x00000002
#define WM_MOUSEHWHEEL 0x020E
#define LOCALE_SSHORTTIME 0x00000079 
#define _WIN32_WINNT_VISTA 0x0600
#define SPI_GETFONTSMOOTHINGTYPE 0x200A
#define FE_FONTSMOOTHINGCLEARTYPE 0x0002

#ifdef __cplusplus
extern "C" {
#endif

WINBASEAPI
BOOL
WINAPI
InitializeCriticalSectionAndSpinCount(
    LPCRITICAL_SECTION lpCriticalSection,
    DWORD dwSpinCount
    );

WINBASEAPI
BOOL
WINAPI
TryEnterCriticalSection(
    LPCRITICAL_SECTION lpCriticalSection
    );

typedef struct tagTRACKMOUSEEVENT *LPTRACKMOUSEEVENT;

WINUSERAPI BOOL WINAPI TrackMouseEvent(LPTRACKMOUSEEVENT lpEventTrack);

WINBASEAPI HRESULT STDAPICALLTYPE CoInitializeEx(LPVOID pvReserved, DWORD dwCoInit);

WINBASEAPI BOOL WINAPI InternetSetCookieA(LPCSTR lpszUrl, LPCSTR lpszCookieName, LPCSTR lpszCookieData);

WINBASEAPI VOID WINAPI GetNativeSystemInfo(LPSYSTEM_INFO lpSystemInfo);

WINBASEAPI BOOL WINAPI QueueUserWorkItem(LPTHREAD_START_ROUTINE Function, PVOID Context, ULONG Flags);

WINBASEAPI COLORREF WINAPI SetDCBrushColor(HDC hdc, COLORREF color);

WINBASEAPI COLORREF WINAPI SetDCPenColor(HDC hdc, COLORREF color);

/*WINBASEAPI*/ BOOL WINAPI GdiAlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);

WINBASEAPI BOOL WINAPI UpdateLayeredWindow(_In_ HWND hWnd, _In_opt_ HDC hdcDst, _In_opt_ POINT* pptDst, _In_opt_ SIZE* psize, _In_opt_ HDC hdcSrc, _In_opt_ POINT* pptSrc, _In_ COLORREF crKey, _In_opt_ BLENDFUNCTION* pblend, _In_ DWORD dwFlags);

WINBASEAPI BOOL WINAPI IsDebuggerPresent(VOID);

//////////////////////////////////////////////////////////////////////////

#define INTERNET_OPEN_TYPE_PRECONFIG 0 
#define INTERNET_FLAG_ASYNC 0x10000000 
#define INTERNET_STATUS_REDIRECT 110
#define INTERNET_STATUS_REQUEST_COMPLETE 100
#define HTTP_ADDREQ_FLAG_ADD 0x20000000
#define INTERNET_OPEN_TYPE_PRECONFIG 0
#define INTERNET_SERVICE_HTTP 3
#define INTERNET_FLAG_PASSIVE 0x08000000 
#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000

#define INTERNET_FLAG_KEEP_CONNECTION 0x00400000 
#define INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS 0x00004000 
#define INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP 0x00008000 
#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000
#define INTERNET_FLAG_SECURE 0x00800000 
#define INTERNET_DEFAULT_HTTP_PORT 80 
#define INTERNET_DEFAULT_HTTPS_PORT 443 
#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000

#define HTTP_VERSIONA L"HTTP/1.0"
#define HTTP_VERSIONW L"HTTP/1.0"
#ifdef UNICODE
#define HTTP_VERSION            HTTP_VERSIONW
#else
#define HTTP_VERSION            HTTP_VERSIONA
#endif

#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000

#define INTERNET_FLAG_NO_UI 0x00000200 

#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000 
#define INTERNET_FLAG_RELOAD 0x80000000 

#define WININET_API_FLAG_ASYNC          0x00000001  // force async operation
#define WININET_API_FLAG_SYNC           0x00000004  // force sync operation
#define WININET_API_FLAG_USE_CONTEXT    0x00000008  // use value supplied in dwContext (even if 0)

#define INTERNET_FLAG_NO_CACHE_WRITE 0x04000000

#define HTTP_QUERY_STATUS_TEXT 20 
#define HTTP_QUERY_STATUS_CODE 19 
#define HTTP_QUERY_CONTENT_LENGTH 5
#define HTTP_QUERY_RAW_HEADERS_CRLF 22 
#define HTTP_QUERY_COOKIE 44
#define HTTP_QUERY_CONTENT_TYPE 1

#define INTERNET_FLAG_NO_CACHE_WRITE    0x04000000  // don't write this item to the cache
#define INTERNET_FLAG_DONT_CACHE        INTERNET_FLAG_NO_CACHE_WRITE

#define IRF_SYNC        WININET_API_FLAG_SYNC
#define IRF_NO_WAIT 0x00000008

typedef LPVOID HINTERNET;
typedef HINTERNET * LPHINTERNET;

typedef WORD INTERNET_PORT;
typedef INTERNET_PORT * LPINTERNET_PORT;

typedef struct _INTERNET_BUFFERSA {
    DWORD dwStructSize;                 // used for API versioning. Set to sizeof(INTERNET_BUFFERS)
    struct _INTERNET_BUFFERSA * Next;   // chain of buffers
    LPCSTR   lpcszHeader;               // pointer to headers (may be NULL)
    DWORD dwHeadersLength;              // length of headers if not NULL
    DWORD dwHeadersTotal;               // size of headers if not enough buffer
    LPVOID lpvBuffer;                   // pointer to data buffer (may be NULL)
    DWORD dwBufferLength;               // length of data buffer if not NULL
    DWORD dwBufferTotal;                // total size of chunk, or content-length if not chunked
    DWORD dwOffsetLow;                  // used for read-ranges (only used in HttpSendRequest2)
    DWORD dwOffsetHigh;
} INTERNET_BUFFERSA, *LPINTERNET_BUFFERSA;

typedef struct _INTERNET_BUFFERSW {
    DWORD dwStructSize;                 // used for API versioning. Set to sizeof(INTERNET_BUFFERS)
    struct _INTERNET_BUFFERSW * Next;   // chain of buffers
    LPCWSTR  lpcszHeader;               // pointer to headers (may be NULL)
    DWORD dwHeadersLength;              // length of headers if not NULL
    DWORD dwHeadersTotal;               // size of headers if not enough buffer
    LPVOID lpvBuffer;                   // pointer to data buffer (may be NULL)
    DWORD dwBufferLength;               // length of data buffer if not NULL
    DWORD dwBufferTotal;                // total size of chunk, or content-length if not chunked
    DWORD dwOffsetLow;                  // used for read-ranges (only used in HttpSendRequest2)
    DWORD dwOffsetHigh;
} INTERNET_BUFFERSW, *LPINTERNET_BUFFERSW;

BOOL WINAPI HttpSendRequestExA(
    _In_ HINTERNET hRequest,
    _In_opt_ LPINTERNET_BUFFERSA lpBuffersIn,
    _Out_opt_ LPINTERNET_BUFFERSA lpBuffersOut,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );

BOOL WINAPI InternetReadFileExA(
    _In_ HINTERNET hFile,
    _Out_ LPINTERNET_BUFFERSA lpBuffersOut,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );

HINTERNET InternetOpenW(_In_opt_ LPCWSTR lpszAgent, _In_ DWORD dwAccessType, _In_opt_ LPCWSTR lpszProxy, _In_opt_ LPCWSTR lpszProxyBypass, _In_ DWORD dwFlags);

typedef
VOID
(CALLBACK * INTERNET_STATUS_CALLBACK)(
    _In_ HINTERNET hInternet,
    _In_opt_ DWORD_PTR dwContext,
    _In_ DWORD dwInternetStatus,
    _In_opt_ LPVOID lpvStatusInformation,
    _In_ DWORD dwStatusInformationLength
    );
HINTERNET WINAPI InternetSetStatusCallback(_In_ HINTERNET hInternet, _In_opt_ INTERNET_STATUS_CALLBACK lpfnInternetCallback);

BOOL WINAPI HttpQueryInfoW(_In_ HINTERNET hRequest, _In_ DWORD dwInfoLevel, LPVOID lpBuffer, _Inout_ LPDWORD lpdwBufferLength, _Inout_opt_ LPDWORD lpdwIndex);

BOOL WINAPI HttpAddRequestHeadersA(
    _In_ HINTERNET hRequest,
    LPCSTR lpszHeaders,
    _In_ DWORD dwHeadersLength,
    _In_ DWORD dwModifiers
    );

BOOL WINAPI
InternetGetLastResponseInfoW(
    _Out_ LPDWORD lpdwError,
    LPWSTR lpszBuffer,
    _Inout_ LPDWORD lpdwBufferLength
    );

BOOL WINAPI
InternetGetCookieA(
    _In_ LPCSTR lpszUrl,
    _In_opt_ LPCSTR lpszCookieName,
    LPSTR lpszCookieData,
    _Inout_ LPDWORD lpdwSize
    );

HINTERNET WINAPI InternetOpenA(_In_opt_ LPCSTR lpszAgent, _In_ DWORD dwAccessType, _In_opt_ LPCSTR lpszProxy, _In_opt_ LPCSTR lpszProxyBypass, _In_ DWORD dwFlags);

HINTERNET WINAPI
InternetConnectW(
    _In_ HINTERNET hInternet,
    _In_ LPCWSTR lpszServerName,
    _In_ INTERNET_PORT nServerPort,
    _In_opt_ LPCWSTR lpszUserName,
    _In_opt_ LPCWSTR lpszPassword,
    _In_ DWORD dwService,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );

BOOL WINAPI InternetCloseHandle(_In_ HINTERNET hInternet);

HINTERNET WINAPI HttpOpenRequestW(
    _In_ HINTERNET hConnect,
    _In_opt_ LPCWSTR lpszVerb,
    _In_opt_ LPCWSTR lpszObjectName,
    _In_opt_ LPCWSTR lpszVersion,
    _In_opt_ LPCWSTR lpszReferrer,
    _In_opt_z_ LPCWSTR FAR * lplpszAcceptTypes,
    _In_ DWORD dwFlags,
    _In_opt_ DWORD_PTR dwContext
    );

BOOL WINAPI HttpEndRequestW(_In_ HINTERNET hRequest, _Out_opt_ LPINTERNET_BUFFERSW lpBuffersOut, _In_ DWORD dwFlags, _In_opt_ DWORD_PTR dwContext);

BOOL WINAPI InternetReadFile(_In_ HINTERNET hFile, LPVOID lpBuffer, _In_ DWORD dwNumberOfBytesToRead, _Out_ LPDWORD lpdwNumberOfBytesRead);

BOOL WINAPI HttpSendRequestW(
    _In_ HINTERNET hRequest,
    LPCWSTR lpszHeaders,
    _In_ DWORD dwHeadersLength,
    LPVOID lpOptional,
    _In_ DWORD dwOptionalLength
    );

BOOL WINAPI InternetWriteFile(_In_ HINTERNET hFile, LPCVOID lpBuffer, _In_ DWORD dwNumberOfBytesToWrite, _Out_ LPDWORD lpdwNumberOfBytesWritten);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif

#endif // base_windowsvc6_h
