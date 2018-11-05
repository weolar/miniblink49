/* wc_port.h
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/*!
    \file wolfssl/wolfcrypt/wc_port.h
*/

#ifndef WOLF_CRYPT_PORT_H
#define WOLF_CRYPT_PORT_H

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/visibility.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Detect if compiler supports C99. "NO_WOLF_C99" can be defined in
 * user_settings.h to disable checking for C99 support. */
#if !defined(WOLF_C99) && defined(__STDC_VERSION__) && \
    !defined(WOLFSSL_ARDUINO) && !defined(NO_WOLF_C99)
    #if __STDC_VERSION__ >= 199901L
        #define WOLF_C99
    #endif
#endif

#ifdef USE_WINDOWS_API
    #ifdef WOLFSSL_GAME_BUILD
        #include "system/xtl.h"
    #else
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #ifndef WOLFSSL_SGX
            #if defined(_WIN32_WCE) || defined(WIN32_LEAN_AND_MEAN)
                /* On WinCE winsock2.h must be included before windows.h */
                #include <winsock2.h>
            #endif
            #include <windows.h>
        #endif /* WOLFSSL_SGX */
    #endif
#elif defined(THREADX)
    #ifndef SINGLE_THREADED
        #ifdef NEED_THREADX_TYPES
            #include <types.h>
        #endif
        #include <tx_api.h>
    #endif
#elif defined(MICRIUM)
    /* do nothing, just don't pick Unix */
#elif defined(FREERTOS) || defined(FREERTOS_TCP) || defined(WOLFSSL_SAFERTOS)
    /* do nothing */
#elif defined(EBSNET)
    /* do nothing */
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    /* do nothing */
#elif defined(FREESCALE_FREE_RTOS)
    #include "fsl_os_abstraction.h"
#elif defined(WOLFSSL_uITRON4)
    #include "stddef.h"
    #include "kernel.h"
#elif  defined(WOLFSSL_uTKERNEL2)
    #include "tk/tkernel.h"
#elif defined(WOLFSSL_CMSIS_RTOS)
    #include "cmsis_os.h"
#elif defined(WOLFSSL_MDK_ARM)
    #if defined(WOLFSSL_MDK5)
        #include "cmsis_os.h"
    #else
        #include <rtl.h>
    #endif
#elif defined(WOLFSSL_CMSIS_RTOS)
    #include "cmsis_os.h"
#elif defined(WOLFSSL_TIRTOS)
    #include <ti/sysbios/BIOS.h>
    #include <ti/sysbios/knl/Semaphore.h>
#elif defined(WOLFSSL_FROSTED)
    #include <semaphore.h>
#elif defined(INTIME_RTOS)
    #include <rt.h>
    #include <io.h>
#else
    #ifndef SINGLE_THREADED
        #define WOLFSSL_PTHREADS
        #include <pthread.h>
    #endif
    #if defined(OPENSSL_EXTRA) || defined(GOAHEAD_WS)
        #include <unistd.h>      /* for close of BIO */
    #endif
#endif

/* For FIPS keep the function names the same */
#ifdef HAVE_FIPS
#define wc_InitMutex   InitMutex
#define wc_FreeMutex   FreeMutex
#define wc_LockMutex   LockMutex
#define wc_UnLockMutex UnLockMutex
#endif /* HAVE_FIPS */

#ifdef SINGLE_THREADED
    typedef int wolfSSL_Mutex;
#else /* MULTI_THREADED */
    /* FREERTOS comes first to enable use of FreeRTOS Windows simulator only */
    #if defined(FREERTOS)
        typedef xSemaphoreHandle wolfSSL_Mutex;
    #elif defined(FREERTOS_TCP)
        #include "FreeRTOS.h"
        #include "semphr.h"
		typedef SemaphoreHandle_t  wolfSSL_Mutex;
    #elif defined(WOLFSSL_SAFERTOS)
        typedef struct wolfSSL_Mutex {
            signed char mutexBuffer[portQUEUE_OVERHEAD_BYTES];
            xSemaphoreHandle mutex;
        } wolfSSL_Mutex;
    #elif defined(USE_WINDOWS_API)
        typedef CRITICAL_SECTION wolfSSL_Mutex;
    #elif defined(WOLFSSL_PTHREADS)
        typedef pthread_mutex_t wolfSSL_Mutex;
    #elif defined(THREADX)
        typedef TX_MUTEX wolfSSL_Mutex;
    #elif defined(MICRIUM)
        typedef OS_MUTEX wolfSSL_Mutex;
    #elif defined(EBSNET)
        typedef RTP_MUTEX wolfSSL_Mutex;
    #elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
        typedef MUTEX_STRUCT wolfSSL_Mutex;
    #elif defined(FREESCALE_FREE_RTOS)
        typedef mutex_t wolfSSL_Mutex;
    #elif defined(WOLFSSL_uITRON4)
        typedef struct wolfSSL_Mutex {
            T_CSEM sem ;
            ID     id ;
        } wolfSSL_Mutex;
    #elif defined(WOLFSSL_uTKERNEL2)
        typedef struct wolfSSL_Mutex {
            T_CSEM sem ;
            ID     id ;
        } wolfSSL_Mutex;
    #elif defined(WOLFSSL_MDK_ARM)
        #if defined(WOLFSSL_CMSIS_RTOS)
            typedef osMutexId wolfSSL_Mutex;
        #else
            typedef OS_MUT wolfSSL_Mutex;
        #endif
    #elif defined(WOLFSSL_CMSIS_RTOS)
        typedef osMutexId wolfSSL_Mutex;
    #elif defined(WOLFSSL_TIRTOS)
        typedef ti_sysbios_knl_Semaphore_Handle wolfSSL_Mutex;
    #elif defined(WOLFSSL_FROSTED)
        typedef mutex_t * wolfSSL_Mutex;
    #elif defined(INTIME_RTOS)
        typedef RTHANDLE wolfSSL_Mutex;
    #else
        #error Need a mutex type in multithreaded mode
    #endif /* USE_WINDOWS_API */
#endif /* SINGLE_THREADED */

/* Enable crypt HW mutex for Freescale MMCAU or PIC32MZ */
#if defined(FREESCALE_MMCAU) || defined(WOLFSSL_MICROCHIP_PIC32MZ)
    #ifndef WOLFSSL_CRYPT_HW_MUTEX
        #define WOLFSSL_CRYPT_HW_MUTEX  1
    #endif
#endif /* FREESCALE_MMCAU */

#ifndef WOLFSSL_CRYPT_HW_MUTEX
    #define WOLFSSL_CRYPT_HW_MUTEX  0
#endif

#if WOLFSSL_CRYPT_HW_MUTEX
    /* wolfSSL_CryptHwMutexInit is called on first wolfSSL_CryptHwMutexLock,
       however it's recommended to call this directly on Hw init to avoid possible
       race condition where two calls to wolfSSL_CryptHwMutexLock are made at
       the same time. */
    int wolfSSL_CryptHwMutexInit(void);
    int wolfSSL_CryptHwMutexLock(void);
    int wolfSSL_CryptHwMutexUnLock(void);
#else
    /* Define stubs, since HW mutex is disabled */
    #define wolfSSL_CryptHwMutexInit()      0 /* Success */
    #define wolfSSL_CryptHwMutexLock()      0 /* Success */
    #define wolfSSL_CryptHwMutexUnLock()    (void)0 /* Success */
#endif /* WOLFSSL_CRYPT_HW_MUTEX */

/* Mutex functions */
WOLFSSL_API int wc_InitMutex(wolfSSL_Mutex*);
WOLFSSL_API wolfSSL_Mutex* wc_InitAndAllocMutex(void);
WOLFSSL_API int wc_FreeMutex(wolfSSL_Mutex*);
WOLFSSL_API int wc_LockMutex(wolfSSL_Mutex*);
WOLFSSL_API int wc_UnLockMutex(wolfSSL_Mutex*);
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER)
/* dynamiclly set which mutex to use. unlock / lock is controlled by flag */
typedef void (mutex_cb)(int flag, int type, const char* file, int line);

WOLFSSL_API int wc_LockMutex_ex(int flag, int type, const char* file, int line);
WOLFSSL_API int wc_SetMutexCb(mutex_cb* cb);
#endif

/* main crypto initialization function */
WOLFSSL_API int wolfCrypt_Init(void);
WOLFSSL_API int wolfCrypt_Cleanup(void);

/* filesystem abstraction layer, used by ssl.c */
#ifndef NO_FILESYSTEM

#if defined(EBSNET)
    #include "vfapi.h"
    #include "vfile.h"

    #define XFILE                    int
    #define XFOPEN(NAME, MODE)       vf_open((const char *)NAME, VO_RDONLY, 0);
    #define XFSEEK                   vf_lseek
    #define XFTELL                   vf_tell
    #define XREWIND                  vf_rewind
    #define XFREAD(BUF, SZ, AMT, FD) vf_read(FD, BUF, SZ*AMT)
    #define XFWRITE(BUF, SZ, AMT, FD) vf_write(FD, BUF, SZ*AMT)
    #define XFCLOSE                  vf_close
    #define XSEEK_END                VSEEK_END
    #define XBADFILE                 -1
    #define XFGETS(b,s,f)            -2 /* Not ported yet */
#elif defined(LSR_FS)
    #include <fs.h>
    #define XFILE                   struct fs_file*
    #define XFOPEN(NAME, MODE)      fs_open((char*)NAME);
    #define XFSEEK(F, O, W)         (void)F
    #define XFTELL(F)               (F)->len
    #define XREWIND(F)              (void)F
    #define XFREAD(BUF, SZ, AMT, F) fs_read(F, (char*)BUF, SZ*AMT)
    #define XFWRITE(BUF, SZ, AMT, F) fs_write(F, (char*)BUF, SZ*AMT)
    #define XFCLOSE                 fs_close
    #define XSEEK_END               0
    #define XBADFILE                NULL
    #define XFGETS(b,s,f)            -2 /* Not ported yet */
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #define XFILE                   MQX_FILE_PTR
    #define XFOPEN                  fopen
    #define XFSEEK                  fseek
    #define XFTELL                  ftell
    #define XREWIND(F)              fseek(F, 0, IO_SEEK_SET)
    #define XFREAD                  fread
    #define XFWRITE                 fwrite
    #define XFCLOSE                 fclose
    #define XSEEK_END               IO_SEEK_END
    #define XBADFILE                NULL
    #define XFGETS                  fgets
#elif defined(MICRIUM)
    #include <fs_api.h>
    #define XFILE      FS_FILE*
    #define XFOPEN     fs_fopen
    #define XFSEEK     fs_fseek
    #define XFTELL     fs_ftell
    #define XREWIND    fs_rewind
    #define XFREAD     fs_fread
    #define XFWRITE    fs_fwrite
    #define XFCLOSE    fs_fclose
    #define XSEEK_END  FS_SEEK_END
    #define XBADFILE   NULL
    #define XFGETS(b,s,f) -2 /* Not ported yet */
#else
    /* stdio, default case */
    #include <stdio.h>
    #define XFILE      FILE*
    #if defined(WOLFSSL_MDK_ARM)
        extern FILE * wolfSSL_fopen(const char *name, const char *mode) ;
        #define XFOPEN     wolfSSL_fopen
    #else
        #define XFOPEN     fopen
    #endif
    #define XFSEEK     fseek
    #define XFTELL     ftell
    #define XREWIND    rewind
    #define XFREAD     fread
    #define XFWRITE    fwrite
    #define XFCLOSE    fclose
    #define XSEEK_END  SEEK_END
    #define XBADFILE   NULL
    #define XFGETS     fgets

    #if !defined(USE_WINDOWS_API) && !defined(NO_WOLFSSL_DIR)\
        && !defined(WOLFSSL_NUCLEUS)
        #include <dirent.h>
        #include <unistd.h>
        #include <sys/stat.h>
    #endif
#endif

    #ifndef MAX_FILENAME_SZ
        #define MAX_FILENAME_SZ  256 /* max file name length */
    #endif
    #ifndef MAX_PATH
        #define MAX_PATH 256
    #endif

#if !defined(NO_WOLFSSL_DIR) && !defined(WOLFSSL_NUCLEUS)
    typedef struct ReadDirCtx {
    #ifdef USE_WINDOWS_API
        WIN32_FIND_DATAA FindFileData;
        HANDLE hFind;
    #else
        struct dirent* entry;
        DIR*   dir;
        struct stat s;
    #endif
        char name[MAX_FILENAME_SZ];
    } ReadDirCtx;

    WOLFSSL_API int wc_ReadDirFirst(ReadDirCtx* ctx, const char* path, char** name);
    WOLFSSL_API int wc_ReadDirNext(ReadDirCtx* ctx, const char* path, char** name);
    WOLFSSL_API void wc_ReadDirClose(ReadDirCtx* ctx);
#endif /* !NO_WOLFSSL_DIR */

#endif /* !NO_FILESYSTEM */

/* Windows API defines its own min() macro. */
#if defined(USE_WINDOWS_API)
    #if defined(min) || defined(WOLFSSL_MYSQL_COMPATIBLE)
        #define WOLFSSL_HAVE_MIN
    #endif /* min */
    #if defined(max) || defined(WOLFSSL_MYSQL_COMPATIBLE)
        #define WOLFSSL_HAVE_MAX
    #endif /* max */
#endif /* USE_WINDOWS_API */

/* Time functions */
#ifndef NO_ASN_TIME
#if defined(USER_TIME)
    /* Use our gmtime and time_t/struct tm types.
       Only needs seconds since EPOCH using XTIME function.
       time_t XTIME(time_t * timer) {}
    */
    #define WOLFSSL_GMTIME
    #define USE_WOLF_TM
    #define USE_WOLF_TIME_T

#elif defined(TIME_OVERRIDES)
    /* Override XTIME() and XGMTIME() functionality.
       Requires user to provide these functions:
        time_t XTIME(time_t * timer) {}
        struct tm* XGMTIME(const time_t* timer, struct tm* tmp) {}
    */
    #ifndef HAVE_TIME_T_TYPE
        #define USE_WOLF_TIME_T
    #endif
    #ifndef HAVE_TM_TYPE
        #define USE_WOLF_TM
    #endif
    #define NEED_TMP_TIME

#elif defined(HAVE_RTP_SYS)
    #include "os.h"           /* dc_rtc_api needs    */
    #include "dc_rtc_api.h"   /* to get current time */

    /* uses parital <time.h> structures */
    #define XTIME(tl)       (0)
    #define XGMTIME(c, t)   rtpsys_gmtime((c))

#elif defined(MICRIUM)
    #include <clk.h>
    #include <time.h>
    #define XTIME(t1)       micrium_time((t1))
    #define WOLFSSL_GMTIME

#elif defined(MICROCHIP_TCPIP_V5) || defined(MICROCHIP_TCPIP)
    #include <time.h>
    #define XTIME(t1)       pic32_time((t1))
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #define XTIME(t1)       mqx_time((t1))
    #define HAVE_GMTIME_R

#elif defined(FREESCALE_KSDK_BM) || defined(FREESCALE_FREE_RTOS) || defined(FREESCALE_KSDK_FREERTOS)
    #include <time.h>
    #ifndef XTIME
        /*extern time_t ksdk_time(time_t* timer);*/
        #define XTIME(t1)   ksdk_time((t1))
    #endif
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(WOLFSSL_ATMEL)
    #define XTIME(t1)       atmel_get_curr_time_and_date((t1))
    #define WOLFSSL_GMTIME
    #define USE_WOLF_TM
    #define USE_WOLF_TIME_T

#elif defined(IDIRECT_DEV_TIME)
    /*Gets the timestamp from cloak software owned by VT iDirect
    in place of time() from <time.h> */
    #include <time.h>
    #define XTIME(t1)       idirect_time((t1))
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(_WIN32_WCE)
    #include <windows.h>
    #define XTIME(t1)       windows_time((t1))
    #define WOLFSSL_GMTIME

#else
    /* default */
    /* uses complete <time.h> facility */
    #include <time.h>
    #if defined(HAVE_SYS_TIME_H) || defined(WOLF_C99)
        #include <sys/time.h>
    #endif

    /* PowerPC time_t is int */
    #ifdef __PPC__
        #define TIME_T_NOT_LONG
    #endif
#endif


/* Map default time functions */
#if !defined(XTIME) && !defined(TIME_OVERRIDES) && !defined(USER_TIME)
    #define XTIME(tl)       time((tl))
#endif
#if !defined(XGMTIME) && !defined(TIME_OVERRIDES)
    #if defined(WOLFSSL_GMTIME) || !defined(HAVE_GMTIME_R) || defined(WOLF_C99)
        #define XGMTIME(c, t)   gmtime((c))
    #else
        #define XGMTIME(c, t)   gmtime_r((c), (t))
        #define NEED_TMP_TIME
    #endif
#endif
#if !defined(XVALIDATE_DATE) && !defined(HAVE_VALIDATE_DATE)
    #define USE_WOLF_VALIDDATE
    #define XVALIDATE_DATE(d, f, t) ValidateDate((d), (f), (t))
#endif

/* wolf struct tm and time_t */
#if defined(USE_WOLF_TM)
    struct tm {
        int  tm_sec;     /* seconds after the minute [0-60] */
        int  tm_min;     /* minutes after the hour [0-59] */
        int  tm_hour;    /* hours since midnight [0-23] */
        int  tm_mday;    /* day of the month [1-31] */
        int  tm_mon;     /* months since January [0-11] */
        int  tm_year;    /* years since 1900 */
        int  tm_wday;    /* days since Sunday [0-6] */
        int  tm_yday;    /* days since January 1 [0-365] */
        int  tm_isdst;   /* Daylight Savings Time flag */
        long tm_gmtoff;  /* offset from CUT in seconds */
        char *tm_zone;   /* timezone abbreviation */
    };
#endif /* USE_WOLF_TM */
#if defined(USE_WOLF_TIME_T)
    typedef long time_t;
#endif
#if defined(USE_WOLF_SUSECONDS_T)
    typedef long suseconds_t;
#endif
#if defined(USE_WOLF_TIMEVAL_T)
    struct timeval
    {
        time_t tv_sec;
        suseconds_t tv_usec;
    };
#endif

    /* forward declarations */
#if defined(USER_TIME)
    struct tm* gmtime(const time_t* timer);
    extern time_t XTIME(time_t * timer);

    #ifdef STACK_TRAP
        /* for stack trap tracking, don't call os gmtime on OS X/linux,
           uses a lot of stack spce */
        extern time_t time(time_t * timer);
        #define XTIME(tl)  time((tl))
    #endif /* STACK_TRAP */

#elif defined(TIME_OVERRIDES)
    extern time_t XTIME(time_t * timer);
    extern struct tm* XGMTIME(const time_t* timer, struct tm* tmp);
#elif defined(WOLFSSL_GMTIME)
    struct tm* gmtime(const time_t* timer);
#endif
#endif /* NO_ASN_TIME */

#ifndef WOLFSSL_LEANPSK
    char* mystrnstr(const char* s1, const char* s2, unsigned int n);
#endif

#ifndef FILE_BUFFER_SIZE
    #define FILE_BUFFER_SIZE 1024     /* default static file buffer size for input,
                                    will use dynamic buffer if not big enough */
#endif


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLF_CRYPT_PORT_H */

