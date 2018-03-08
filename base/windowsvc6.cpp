
#if USING_VC6RT == 1

#include <windows.h>

extern "C" void __cdecl free(void *);

void __cdecl operator delete(void* p, unsigned int)
{
    free(p);
}

void __cdecl operator delete[](void * p, unsigned int)
{
    free(p);
}

typedef HINTERNET(WINAPI * PFN_InternetOpenW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
static PFN_InternetOpenW pInternetOpenW = nullptr;

typedef BOOL(WINAPI* PFN_GdiAlphaBlend)(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest,
    int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn);
static PFN_GdiAlphaBlend pGdiAlphaBlend = nullptr;

void scrt_initialize_thread_safe_statics()
{
    HMODULE hMon = LoadLibraryW(L"Wininet.dll");
    pInternetOpenW = (PFN_InternetOpenW)GetProcAddress(hMon, "InternetOpenW");

    hMon = LoadLibraryW(L"GDI32.dll");
    pGdiAlphaBlend = (PFN_GdiAlphaBlend)GetProcAddress(hMon, "GdiAlphaBlend");

    LoadLibraryW(L"Usp10.dll");

    OutputDebugStringA("scrt_initialize_thread_safe_statics\n");
}

extern "C" {

int __security_cookie = 0;     //比错误提示的名称少一个下划线
void __fastcall __security_check_cookie(unsigned int cookie)  //参数个数要正确
{
}

void * __cdecl _except_handler4(void *ExceptionRecord, void *EstablisherFrame, void *ContextRecord, void *DispatcherContext)
{
    return 0;
}

int _Init_thread_epoch = -1;
int _Init_global_epoch = -1;
//CRITICAL_SECTION Tss_mutex = { 0 };

void _Init_thread_lock()
{
    //::EnterCriticalSection(&Tss_mutex);
    DebugBreak();
}

void _Init_thread_unlock()
{
    //::LeaveCriticalSection(&Tss_mutex);
    DebugBreak();
}

void _Init_thread_wait(int a)
{
    DebugBreak();
}

void _Init_thread_notify()
{
    DebugBreak();
}

// void __cdecl _Init_thread_header(int* a1)
// {
// //     _Init_thread_lock();
// //     while (*(int*)a1)
// //     {
// //         if (*(int *)a1 != -1)
// //         {
// //             *(int *)(*(int *)(__readfsdword(44) + 4 * tls_index) + 4) = _Init_thread_epoch;
// //             return _Init_thread_unlock();
// //         }
// //         _Init_thread_wait(0x64u);
// //     }
// //     *(int*)a1 = -1;
// //     return _Init_thread_unlock();
//     *(int*)a1 = -1;
// }

// void _Init_thread_footer(int* a1)
// {
// //     _Init_thread_lock();
// //     ++_Init_global_epoch;
// //     *(int *)a1 = _Init_global_epoch;
// //     *(int *)(*(int *)(__readfsdword(44) + 4 * tls_index) + 4) = _Init_global_epoch;
// //     _Init_thread_unlock();
// //     _Init_thread_notify();
//     *(int *)a1 = _Init_global_epoch;
// }

void __CxxFrameHandler3()
{
    DebugBreak();
}

//#pragma function("ReadWriteBarrier")

HINTERNET InternetOpenW(_In_opt_ LPCWSTR lpszAgent, _In_ DWORD dwAccessType, _In_opt_ LPCWSTR lpszProxy, _In_opt_ LPCWSTR lpszProxyBypass, _In_ DWORD dwFlags)
{
    return pInternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
}

BOOL WINAPI GdiAlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
    return pGdiAlphaBlend(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

//////////////////////////////////////////////////////////////////////////
extern "C" long WINAPI _InterlockedOr(long volatile * _Value, long _Mask);
extern "C" char WINAPI _InterlockedOr8(char volatile * _Value, char _Mask);
extern "C" short WINAPI _InterlockedOr16(short volatile * _Value, short _Mask);
extern "C" short WINAPI _InterlockedExchange16(short volatile * _Target, short _Value);
extern "C" short WINAPI _InterlockedXor16(short volatile * _Value, short _Mask);
extern "C" short WINAPI _InterlockedCompareExchange16(short volatile * _Destination, short _Exchange, short _Comparand);
extern "C" char WINAPI _InterlockedCompareExchange8(char volatile * _Destination, char _Exchange, char _Comparand);
extern "C" char WINAPI _InterlockedXor8(char volatile * _Value, char _Mask);
extern "C" short WINAPI _InterlockedExchangeAdd16(short volatile * _Addend, short _Value);
extern "C" __int64 WINAPI _InterlockedCompareExchange64(__int64 volatile *Destination, __int64 ExChange, __int64 Comperand);
extern "C" long WINAPI _InterlockedXor(long volatile * _Value, long _Mask);
extern "C" short WINAPI _InterlockedAnd16(short volatile * _Value, short _Mask);
extern "C" long WINAPI _InterlockedAnd(long volatile * _Value, long _Mask);
extern "C" char WINAPI _InterlockedExchange8(char volatile * _Target, char _Value);
extern "C" char WINAPI _InterlockedAnd8(char volatile * _Value, char _Mask);
extern "C" char WINAPI _InterlockedExchangeAdd8(char volatile * _Addend, char _Value);

// #pragma function("_InterlockedOr")
// #pragma function("_InterlockedOr8")
// #pragma function("_InterlockedOr16")
// #pragma function("_InterlockedExchange16")
// #pragma function("_InterlockedXor16")
// #pragma function("_InterlockedCompareExchange16")
// #pragma function("_InterlockedCompareExchange8")
// #pragma function("_InterlockedXor8")
// #pragma function("_InterlockedExchangeAdd16")
// #pragma function("_InterlockedCompareExchange64")
// #pragma function("_InterlockedXor")
// #pragma function("_InterlockedAnd16")
// #pragma function("_InterlockedAnd")
// #pragma function("_InterlockedExchange8")
// #pragma function("_InterlockedAnd8")
// #pragma function("_InterlockedExchangeAdd8")

//////////////////////////////////////////////////////////////////////////

long WINAPI InterlockedOr(long volatile * _Value, long _Mask)
{
    return _InterlockedOr(_Value, _Mask);
}

char WINAPI InterlockedOr8(char volatile * _Value, char _Mask)
{
    return _InterlockedOr8(_Value, _Mask);
}

short WINAPI InterlockedOr16(short volatile * _Value, short _Mask)
{
    return _InterlockedOr16(_Value, _Mask);
}

short WINAPI InterlockedExchange16(short volatile * _Target, short _Value)
{
    return _InterlockedExchange16(_Target, _Value);
}

short WINAPI InterlockedXor16(short volatile * _Value, short _Mask)
{
    return _InterlockedXor16(_Value, _Mask);
}

short WINAPI InterlockedCompareExchange16(short volatile * _Destination, short _Exchange, short _Comparand)
{
    return _InterlockedCompareExchange16(_Destination, _Exchange, _Comparand);
}

char WINAPI InterlockedCompareExchange8(char volatile * _Destination, char _Exchange, char _Comparand)
{
    return _InterlockedCompareExchange8(_Destination, _Exchange, _Comparand);
}

char WINAPI InterlockedXor8(char volatile * _Value, char _Mask)
{
    return _InterlockedXor8(_Value, _Mask);
}

short WINAPI InterlockedExchangeAdd16(short volatile * _Addend, short _Value)
{
    return _InterlockedExchangeAdd16(_Addend, _Value);
}

__int64 WINAPI InterlockedCompareExchange64(__int64 volatile *Destination, __int64 ExChange, __int64 Comperand)
{
    return _InterlockedCompareExchange64(Destination, ExChange, Comperand);
}

long WINAPI InterlockedXor(long volatile * _Value, long _Mask)
{
    return _InterlockedXor(_Value, _Mask);
}

short WINAPI InterlockedAnd16(short volatile * _Value, short _Mask)
{
    return _InterlockedAnd16(_Value, _Mask);
}

long WINAPI InterlockedAnd(long volatile * _Value, long _Mask)
{
    return _InterlockedAnd(_Value, _Mask);
}

char WINAPI InterlockedExchange8(char volatile * _Target, char _Value)
{
    return _InterlockedExchange8(_Target, _Value);
}

char WINAPI InterlockedAnd8(char volatile * _Value, char _Mask)
{
    return _InterlockedAnd8(_Value, _Mask);
}

char WINAPI InterlockedExchangeAdd8(char volatile * _Addend, char _Value)
{
    return _InterlockedExchangeAdd8(_Addend, _Value);
}

};

void __stdcall _ReadWriteBarrier(void)
{
}

//////////////////////////////////////////////////////////////////////////

/* Copyright (c) Robert Walker, support@tunesmithy.co.uk
* Free source. Do what you wish with it - treat it as you would
* example code in a book on c programming.
*/

#undef time   
#undef localtime   
#undef mktime   
#undef difftime   
#undef gmtime   

#include <time.h>   
#include <assert.h>   
#undef time_t

static_assert(sizeof(time_t) == sizeof(__int64), "the value of time_t does not match with int64");

#define SECS_TO_FT_MULT 10000000   
// #ifdef _DEBUG   
// #define DEBUG_TIME_T   
// #endif   

/* From MSVC help:
* The gmtime, mktime, and localtime functions use the same single,
* statically allocated structure to hold their results. Each call to
* one of these functions destroys the result of any previous call.
* If timer represents a date before midnight, January 1, 1970,
* gmtime returns NULL. There is no error return.
*
* So here is the struct to use for our 64 bit implementation
*
* However, it may be useful to be able to make this thread safe
*/

#ifdef USE_THREAD_LOCAL_VARIABLES   
#define  TIME64_THREAD_LOCAL _declspec(thread)   
#else   
#define TIME64_THREAD_LOCAL   
#endif   

//TIME64_THREAD_LOCAL static struct tm today_ret;

static void t64ToFileTime(time_t *pt, FILETIME *pft)
{
    LARGE_INTEGER li;
    li.QuadPart = *pt*SECS_TO_FT_MULT;
    pft->dwLowDateTime = li.LowPart;
    pft->dwHighDateTime = li.HighPart;
}

static void fileTimeToT64(FILETIME *pft, time_t *pt)
{
    LARGE_INTEGER li;
    li.LowPart = pft->dwLowDateTime;
    li.HighPart = pft->dwHighDateTime;
    *pt = li.QuadPart;
    *pt /= SECS_TO_FT_MULT;
}

#define  FindTimeTBase() (time_t) 11644473600   
// calculated using   
/**
static __int64 FindTimeTBase(void)
{
// Find 1st Jan 1970 as a FILETIME
SYSTEMTIME st;
FILETIME ft;
memset(&st,0,sizeof(st));
st.wYear=1970;
st.wMonth=1;
st.wDay=1;
SystemTimeToFileTime(&st, &ft);
FileTimeToT64(&ft,&tbase);
return tbase;
}
**/

static void systemTimeToT64(SYSTEMTIME* pst, time_t* pt)
{
    FILETIME ft;
    ::SystemTimeToFileTime(pst, &ft);
    fileTimeToT64(&ft, pt);
    *pt -= FindTimeTBase();
}

static void t64ToSystemTime(const time_t* pt, SYSTEMTIME* pst)
{
    FILETIME ft;
    time_t t = *pt;
    t += FindTimeTBase();
    t64ToFileTime(&t, &ft);
    ::FileTimeToSystemTime(&ft, pst);
}


extern "C" time_t time_64(time_t* pt)
{
    time_t t;
    SYSTEMTIME st;
    ::GetSystemTime(&st);
    systemTimeToT64(&st, &t);
#ifdef DEBUG_TIME_T   
    {
        time_t t2 = time(NULL);
        if (t2 >= 0)
            assert(abs(t2 - (int)t) <= 1);
        // the <=1 here is in case the seconds get incremented   
        // betweeen the GetSystemTime(..) call and the time(..) call   
    }
#endif   
    if (pt)
        *pt = t;
    return t;
}

double difftime_64(time_t time1, time_t time0)
{
    return (double)(time1 - time0);
}

time_t mktime_64(struct tm *today)
{
    time_t t;
    SYSTEMTIME st;
    st.wDay = (WORD)today->tm_mday;
    st.wDayOfWeek = (WORD)today->tm_wday;
    st.wHour = (WORD)today->tm_hour;
    st.wMinute = (WORD)today->tm_min;
    st.wMonth = (WORD)(today->tm_mon + 1);
    st.wSecond = (WORD)today->tm_sec;
    st.wYear = (WORD)(today->tm_year + 1900);
    st.wMilliseconds = 0;
    systemTimeToT64(&st, &t);
    return t;
}

#define DAY_IN_SECS (60*60*24)

extern "C" struct tm* gmtime_64(const time_t* t)
{
    static struct tm today_ret;
    SYSTEMTIME st;
    t64ToSystemTime(t, &st);
    today_ret.tm_wday = st.wDayOfWeek;
    today_ret.tm_min = st.wMinute;
    today_ret.tm_sec = st.wSecond;
    today_ret.tm_mon = st.wMonth - 1;
    today_ret.tm_mday = st.wDay;
    today_ret.tm_hour = st.wHour;
    today_ret.tm_year = st.wYear - 1900;
    {
        SYSTEMTIME styear;
        __int64 t64Year;
        memset(&styear, 0, sizeof(styear));
        styear.wYear = st.wYear;
        styear.wMonth = 1;
        styear.wDay = 1;
        systemTimeToT64(&styear, &t64Year);
        today_ret.tm_yday = (int)((*t - t64Year) / DAY_IN_SECS);
    }
    today_ret.tm_isdst = 0;
#ifdef DEBUG_TIME_T   
    {
        struct tm today2;
        long t32 = (int)t;
        if (t32 >= 0) {
            today2 = *gmtime(&t32);
            assert(today_ret.tm_yday == today2.tm_yday);
            assert(today_ret.tm_wday == today2.tm_wday);
            assert(today_ret.tm_min == today2.tm_min);
            assert(today_ret.tm_sec == today2.tm_sec);
            assert(today_ret.tm_mon == today2.tm_mon);
            assert(today_ret.tm_mday == today2.tm_mday);
            assert(today_ret.tm_hour == today2.tm_hour);
            assert(today_ret.tm_year == today2.tm_year);
        }
    }
    {
        __int64 t2 = mktime_64(&today_ret);
        assert(t2 == t);
    }
#endif   
    return &today_ret;
}

struct tm* localtime_64(const time_t* pt)
{
    static struct tm today_ret;
    time_t t = *pt;
    FILETIME ft, ftlocal;
    t64ToFileTime(&t, &ft);
    FileTimeToLocalFileTime(&ft, &ftlocal);
    fileTimeToT64(&ftlocal, &t);
    today_ret = *gmtime_64(&t);
    
    TIME_ZONE_INFORMATION timeZoneInformation;
    switch (GetTimeZoneInformation(&timeZoneInformation)) {
    case TIME_ZONE_ID_DAYLIGHT:
        today_ret.tm_isdst = 1;
        break;
    case TIME_ZONE_ID_STANDARD:
        today_ret.tm_isdst = 0;
        break;
    case TIME_ZONE_ID_UNKNOWN:
        today_ret.tm_isdst = -1;
        break;
    }
    
    return &today_ret;
}

#endif // #if USING_VC6RT == 1