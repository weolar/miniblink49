
#if USING_VC6RT == 1

#include <windowsvc6.h>

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

#endif // #if USING_VC6RT == 1