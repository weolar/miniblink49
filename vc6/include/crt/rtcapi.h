/***
*rtcapi.h - declarations and definitions for RTC use
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Contains the declarations and definitions for all RunTime Check
*       support.
*
****/

#ifndef _INC_RTCAPI
#define _INC_RTCAPI


#ifdef  __cplusplus

extern "C" {

#endif

    /* General User API */

typedef enum _RTC_ErrorNumber {
    _RTC_CHKSTK = 0,
    _RTC_CVRT_LOSS_INFO,
    _RTC_CORRUPT_STACK,
    _RTC_UNINIT_LOCAL_USE,
    _RTC_ILLEGAL 
} _RTC_ErrorNumber;
 
#   define _RTC_ERRTYPE_IGNORE -1
#   define _RTC_ERRTYPE_ASK    -2

    typedef int (__cdecl *_RTC_error_fn)(int, const char *, int, const char *, const char *, ...);

    /* User API */
    int           __cdecl _RTC_NumErrors(void);
    const char *  __cdecl _RTC_GetErrDesc(_RTC_ErrorNumber errnum);
    int           __cdecl _RTC_SetErrorType(_RTC_ErrorNumber errnum, int ErrType);
    _RTC_error_fn __cdecl _RTC_SetErrorFunc(_RTC_error_fn);

    /* Power User/library API */


    /* Init functions */

    /* These functions all call _CRT_RTC_INIT */
    void __cdecl _RTC_Initialize(void);
    void __cdecl _RTC_Terminate(void);

    /*
     * If you're not using the CRT, you have to implement _CRT_RTC_INIT
     * Just return either null, or your error reporting function
     * *** Don't mess with res0/res1/res2/res3/res4 - YOU'VE BEEN WARNED! ***
     */
    _RTC_error_fn _CRT_RTC_INIT(void *res0, void **res1, int res2, int res3, int res4);
    
    /* Compiler generated calls (unlikely to be used, even by power users) */
    /* Types */
    typedef struct _RTC_vardesc {
        int addr;
        int size;
        char *name;
    } _RTC_vardesc;

    typedef struct _RTC_framedesc {
        int varCount;
        _RTC_vardesc *variables;
    } _RTC_framedesc;

    /* Shortening convert checks - name indicates src bytes to target bytes */
    /* Signedness is NOT checked */
    char   __fastcall _RTC_Check_2_to_1(short src);
    char   __fastcall _RTC_Check_4_to_1(int src);
    char   __fastcall _RTC_Check_8_to_1(__int64 src);
    short  __fastcall _RTC_Check_4_to_2(int src);
    short  __fastcall _RTC_Check_8_to_2(__int64 src);
    int    __fastcall _RTC_Check_8_to_4(__int64 src);
 
    
    /* Stack Checking Calls */
#if defined(_M_IX86)
    void   __cdecl     _RTC_CheckEsp();
#endif

    void   __fastcall  _RTC_CheckStackVars(void *esp, _RTC_framedesc *fd);

    /* Unintialized Local call */
    void   __cdecl     _RTC_UninitUse(const char *varname);


    /* Subsystem initialization stuff */
    void    __cdecl    _RTC_Shutdown(void);
    void    __cdecl    _RTC_InitBase(void);
    

#ifdef  __cplusplus

    void* _ReturnAddress();
}

#endif


#endif /* _INC_RTCAPI */

