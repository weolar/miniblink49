/* types.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
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



#ifndef WOLF_CRYPT_TYPES_H
#define WOLF_CRYPT_TYPES_H

	#include <wolfssl/wolfcrypt/settings.h>
	#include <wolfssl/wolfcrypt/wc_port.h>

	#ifdef __cplusplus
	    extern "C" {
	#endif


	#if defined(WORDS_BIGENDIAN)
	    #define BIG_ENDIAN_ORDER
	#endif

	#ifndef BIG_ENDIAN_ORDER
	    #define LITTLE_ENDIAN_ORDER
	#endif

	#ifndef WOLFSSL_TYPES
	    #ifndef byte
	        typedef unsigned char  byte;
	    #endif
	    typedef unsigned short word16;
	    typedef unsigned int   word32;
	#endif


	/* try to set SIZEOF_LONG or LONG_LONG if user didn't */
	#if !defined(_MSC_VER) && !defined(__BCPLUSPLUS__)
	    #if !defined(SIZEOF_LONG_LONG) && !defined(SIZEOF_LONG)
	        #if (defined(__alpha__) || defined(__ia64__) || defined(_ARCH_PPC64) \
	                || defined(__mips64)  || defined(__x86_64__))
	            /* long should be 64bit */
	            #define SIZEOF_LONG 8
	        #elif defined(__i386__) || defined(__CORTEX_M3__)
	            /* long long should be 64bit */
	            #define SIZEOF_LONG_LONG 8
	        #endif
	    #endif
	#endif


	#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
	    #define WORD64_AVAILABLE
	    #define W64LIT(x) x##ui64
	    typedef unsigned __int64 word64;
	#elif defined(SIZEOF_LONG) && SIZEOF_LONG == 8
	    #define WORD64_AVAILABLE
	    #define W64LIT(x) x##LL
	    typedef unsigned long word64;
	#elif defined(SIZEOF_LONG_LONG) && SIZEOF_LONG_LONG == 8
	    #define WORD64_AVAILABLE
	    #define W64LIT(x) x##LL
	    typedef unsigned long long word64;
	#elif defined(__SIZEOF_LONG_LONG__) && __SIZEOF_LONG_LONG__ == 8
	    #define WORD64_AVAILABLE
	    #define W64LIT(x) x##LL
	    typedef unsigned long long word64;
	#else
	    #define MP_16BIT  /* for mp_int, mp_word needs to be twice as big as
	                         mp_digit, no 64 bit type so make mp_digit 16 bit */
	#endif


	/* These platforms have 64-bit CPU registers.  */
	#if (defined(__alpha__) || defined(__ia64__) || defined(_ARCH_PPC64) || \
	     defined(__mips64)  || defined(__x86_64__) || defined(_M_X64)) || \
         defined(__aarch64__) || defined(__sparc64__)
	    typedef word64 wolfssl_word;
        #define WC_64BIT_CPU
	#elif (defined(sun) || defined(__sun)) && \
          (defined(LP64) || defined(_LP64))
        /* LP64 with GNU GCC compiler is reserved for when long int is 64 bits
         * and int uses 32 bits. When using Solaris Studio sparc and __sparc are
         * avialable for 32 bit detection but __sparc64__ could be missed. This
         * uses LP64 for checking 64 bit CPU arch. */
	    typedef word64 wolfssl_word;
        #define WC_64BIT_CPU
    #else
	    typedef word32 wolfssl_word;
	    #ifdef WORD64_AVAILABLE
	        #define WOLFCRYPT_SLOW_WORD64
	    #endif
	#endif


	enum {
	    WOLFSSL_WORD_SIZE  = sizeof(wolfssl_word),
	    WOLFSSL_BIT_SIZE   = 8,
	    WOLFSSL_WORD_BITS  = WOLFSSL_WORD_SIZE * WOLFSSL_BIT_SIZE
	};

	#define WOLFSSL_MAX_16BIT 0xffffU

	/* use inlining if compiler allows */
	#ifndef INLINE
	#ifndef NO_INLINE
	    #ifdef _MSC_VER
	        #define INLINE __inline
	    #elif defined(__GNUC__)
               #ifdef WOLFSSL_VXWORKS
                   #define INLINE __inline__
               #else
                   #define INLINE inline
               #endif
	    #elif defined(__IAR_SYSTEMS_ICC__)
	        #define INLINE inline
	    #elif defined(THREADX)
	        #define INLINE _Inline
	    #else
	        #define INLINE
	    #endif
	#else
	    #define INLINE
	#endif
	#endif


    /* set up rotate style */
    #if (defined(_MSC_VER) || defined(__BCPLUSPLUS__)) && \
        !defined(WOLFSSL_SGX) && !defined(INTIME_RTOS)
        #define INTEL_INTRINSICS
        #define FAST_ROTATE
    #elif defined(__MWERKS__) && TARGET_CPU_PPC
        #define PPC_INTRINSICS
        #define FAST_ROTATE
    #elif defined(__GNUC__)  && (defined(__i386__) || defined(__x86_64__))
        /* GCC does peephole optimizations which should result in using rotate
           instructions  */
        #define FAST_ROTATE
    #endif


	/* set up thread local storage if available */
	#ifdef HAVE_THREAD_LS
	    #if defined(_MSC_VER)
	        #define THREAD_LS_T __declspec(thread)
	    /* Thread local storage only in FreeRTOS v8.2.1 and higher */
	    #elif defined(FREERTOS)
	        #define THREAD_LS_T
	    #else
	        #define THREAD_LS_T __thread
	    #endif
	#else
	    #define THREAD_LS_T
	#endif

    /* GCC 7 has new switch() fall-through detection */
    #if defined(__GNUC__)
        #if ((__GNUC__ > 7) || ((__GNUC__ == 7) && (__GNUC_MINOR__ >= 1)))
            #define FALL_THROUGH __attribute__ ((fallthrough));
        #endif
    #endif
    #ifndef FALL_THROUGH
        #define FALL_THROUGH
    #endif

	/* Micrium will use Visual Studio for compilation but not the Win32 API */
	#if defined(_WIN32) && !defined(MICRIUM) && !defined(FREERTOS) && \
		!defined(FREERTOS_TCP) && !defined(EBSNET) && \
        !defined(WOLFSSL_UTASKER) && !defined(INTIME_RTOS)
	    #define USE_WINDOWS_API
	#endif


	/* idea to add global alloc override by Moises Guimaraes  */
	/* default to libc stuff */
	/* XREALLOC is used once in normal math lib, not in fast math lib */
	/* XFREE on some embeded systems doesn't like free(0) so test  */
	#if defined(HAVE_IO_POOL)
		WOLFSSL_API void* XMALLOC(size_t n, void* heap, int type);
		WOLFSSL_API void* XREALLOC(void *p, size_t n, void* heap, int type);
		WOLFSSL_API void XFREE(void *p, void* heap, int type);
	#elif defined(WOLFSSL_ASYNC_CRYPT) && defined(HAVE_INTEL_QA)
        #include <wolfssl/wolfcrypt/port/intel/quickassist_mem.h>
        #undef USE_WOLFSSL_MEMORY
        #ifdef WOLFSSL_DEBUG_MEMORY
            #define XMALLOC(s, h, t)     IntelQaMalloc((s), (h), (t), __func__, __LINE__)
            #define XFREE(p, h, t)       IntelQaFree((p), (h), (t), __func__, __LINE__)
            #define XREALLOC(p, n, h, t) IntelQaRealloc((p), (n), (h), (t), __func__, __LINE__)
        #else
            #define XMALLOC(s, h, t)     IntelQaMalloc((s), (h), (t))
            #define XFREE(p, h, t)       IntelQaFree((p), (h), (t))
            #define XREALLOC(p, n, h, t) IntelQaRealloc((p), (n), (h), (t))
        #endif /* WOLFSSL_DEBUG_MEMORY */
    #elif defined(XMALLOC_USER)
	    /* prototypes for user heap override functions */
	    #include <stddef.h>  /* for size_t */
	    extern void *XMALLOC(size_t n, void* heap, int type);
	    extern void *XREALLOC(void *p, size_t n, void* heap, int type);
	    extern void XFREE(void *p, void* heap, int type);
    #elif defined(XMALLOC_OVERRIDE)
        /* override the XMALLOC, XFREE and XREALLOC macros */
	#elif defined(NO_WOLFSSL_MEMORY)
	    /* just use plain C stdlib stuff if desired */
	    #include <stdlib.h>
	    #define XMALLOC(s, h, t)     ((void)h, (void)t, malloc((s)))
	    #define XFREE(p, h, t)       {void* xp = (p); if((xp)) free((xp));}
	    #define XREALLOC(p, n, h, t) realloc((p), (n))
	#elif !defined(MICRIUM_MALLOC) && !defined(EBSNET) \
	        && !defined(WOLFSSL_SAFERTOS) && !defined(FREESCALE_MQX) \
	        && !defined(FREESCALE_KSDK_MQX) && !defined(FREESCALE_FREE_RTOS) \
            && !defined(WOLFSSL_LEANPSK) && !defined(FREERTOS) && !defined(FREERTOS_TCP)\
            && !defined(WOLFSSL_uITRON4)
	    /* default C runtime, can install different routines at runtime via cbs */
	    #include <wolfssl/wolfcrypt/memory.h>
        #ifdef WOLFSSL_STATIC_MEMORY
            #ifdef WOLFSSL_DEBUG_MEMORY
				#define XMALLOC(s, h, t)     wolfSSL_Malloc((s), (h), (t), __func__, __LINE__)
				#define XFREE(p, h, t)       {void* xp = (p); if((xp)) wolfSSL_Free((xp), (h), (t), __func__, __LINE__);}
				#define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), (h), (t), __func__, __LINE__)
            #else
	            #define XMALLOC(s, h, t)     wolfSSL_Malloc((s), (h), (t))
				#define XFREE(p, h, t)       {void* xp = (p); if((xp)) wolfSSL_Free((xp), (h), (t));}
				#define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), (h), (t))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #else
            #ifdef WOLFSSL_DEBUG_MEMORY
				#define XMALLOC(s, h, t)     ((void)h, (void)t, wolfSSL_Malloc((s), __func__, __LINE__))
				#define XFREE(p, h, t)       {void* xp = (p); if((xp)) wolfSSL_Free((xp), __func__, __LINE__);}
				#define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n), __func__, __LINE__)
            #else
	            #define XMALLOC(s, h, t)     ((void)h, (void)t, wolfSSL_Malloc((s)))
	            #define XFREE(p, h, t)       {void* xp = (p); if((xp)) wolfSSL_Free((xp));}
	            #define XREALLOC(p, n, h, t) wolfSSL_Realloc((p), (n))
            #endif /* WOLFSSL_DEBUG_MEMORY */
        #endif /* WOLFSSL_STATIC_MEMORY */
	#endif

    /* declare/free variable handling for async */
    #ifdef WOLFSSL_ASYNC_CRYPT
        #define DECLARE_VAR(VAR_NAME, VAR_TYPE, VAR_SIZE, HEAP) \
            VAR_TYPE* VAR_NAME = (VAR_TYPE*)XMALLOC(sizeof(VAR_TYPE) * VAR_SIZE, HEAP, DYNAMIC_TYPE_WOLF_BIGINT);
        #define DECLARE_VAR_INIT(VAR_NAME, VAR_TYPE, VAR_SIZE, INIT_VALUE, HEAP) \
            VAR_TYPE* VAR_NAME = ({ \
                VAR_TYPE* ptr = (VAR_TYPE*)XMALLOC(sizeof(VAR_TYPE) * VAR_SIZE, HEAP, DYNAMIC_TYPE_WOLF_BIGINT); \
                if (ptr && INIT_VALUE) { \
                    XMEMCPY(ptr, INIT_VALUE, sizeof(VAR_TYPE) * VAR_SIZE); \
                } \
                ptr; \
            })
        #define DECLARE_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            VAR_TYPE* VAR_NAME[VAR_ITEMS]; \
            int idx##VAR_NAME; \
            for (idx##VAR_NAME=0; idx##VAR_NAME<VAR_ITEMS; idx##VAR_NAME++) { \
                VAR_NAME[idx##VAR_NAME] = (VAR_TYPE*)XMALLOC(VAR_SIZE, HEAP, DYNAMIC_TYPE_WOLF_BIGINT); \
            }
        #define FREE_VAR(VAR_NAME, HEAP) \
            XFREE(VAR_NAME, HEAP, DYNAMIC_TYPE_WOLF_BIGINT);
        #define FREE_ARRAY(VAR_NAME, VAR_ITEMS, HEAP) \
            for (idx##VAR_NAME=0; idx##VAR_NAME<VAR_ITEMS; idx##VAR_NAME++) { \
                XFREE(VAR_NAME[idx##VAR_NAME], HEAP, DYNAMIC_TYPE_WOLF_BIGINT); \
            }
    #else
        #define DECLARE_VAR(VAR_NAME, VAR_TYPE, VAR_SIZE, HEAP) \
            VAR_TYPE VAR_NAME[VAR_SIZE]
        #define DECLARE_VAR_INIT(VAR_NAME, VAR_TYPE, VAR_SIZE, INIT_VALUE, HEAP) \
            VAR_TYPE* VAR_NAME = (VAR_TYPE*)INIT_VALUE
        #define DECLARE_ARRAY(VAR_NAME, VAR_TYPE, VAR_ITEMS, VAR_SIZE, HEAP) \
            VAR_TYPE VAR_NAME[VAR_ITEMS][VAR_SIZE]
        #define FREE_VAR(VAR_NAME, HEAP) /* nothing to free, its stack */
        #define FREE_ARRAY(VAR_NAME, VAR_ITEMS, HEAP)  /* nothing to free, its stack */
    #endif

    #ifndef WOLFSSL_LEANPSK
	    char* mystrnstr(const char* s1, const char* s2, unsigned int n);
    #endif

	#ifndef STRING_USER
	    #include <string.h>
	    #define XMEMCPY(d,s,l)    memcpy((d),(s),(l))
	    #define XMEMSET(b,c,l)    memset((b),(c),(l))
	    #define XMEMCMP(s1,s2,n)  memcmp((s1),(s2),(n))
	    #define XMEMMOVE(d,s,l)   memmove((d),(s),(l))

	    #define XSTRLEN(s1)       strlen((s1))
	    #define XSTRNCPY(s1,s2,n) strncpy((s1),(s2),(n))
	    /* strstr, strncmp, and strncat only used by wolfSSL proper,
         * not required for wolfCrypt only */
	    #define XSTRSTR(s1,s2)    strstr((s1),(s2))
	    #define XSTRNSTR(s1,s2,n) mystrnstr((s1),(s2),(n))
	    #define XSTRNCMP(s1,s2,n) strncmp((s1),(s2),(n))
	    #define XSTRNCAT(s1,s2,n) strncat((s1),(s2),(n))

        #ifdef MICROCHIP_PIC32
            /* XC32 does not support strncasecmp, so use case sensitive one */
            #define XSTRNCASECMP(s1,s2,n) strncmp((s1),(s2),(n))
        #elif defined(USE_WINDOWS_API)
	        #define XSTRNCASECMP(s1,s2,n) _strnicmp((s1),(s2),(n))
        #else
	        #define XSTRNCASECMP(s1,s2,n) strncasecmp((s1),(s2),(n))
	    #endif

        /* snprintf is used in asn.c for GetTimeString and PKCS7 test */
        #ifndef USE_WINDOWS_API
            #define XSNPRINTF snprintf
        #else
            #define XSNPRINTF _snprintf
        #endif

        #if defined(WOLFSSL_CERT_EXT) || defined(HAVE_ALPN)
            /* use only Thread Safe version of strtok */
            #if !defined(USE_WINDOWS_API) && !defined(INTIME_RTOS)
                #define XSTRTOK strtok_r
            #elif defined(__MINGW32__) || defined(WOLFSSL_TIRTOS) || \
                    defined(USE_WOLF_STRTOK)
                #ifndef USE_WOLF_STRTOK
                    #define USE_WOLF_STRTOK
                #endif
                #define XSTRTOK wc_strtok
            #else
                #define XSTRTOK strtok_s
            #endif
        #endif
	#endif

	#ifndef CTYPE_USER
	    #include <ctype.h>
	    #if defined(HAVE_ECC) || defined(HAVE_OCSP) || defined(WOLFSSL_KEY_GEN)
	        #define XTOUPPER(c)     toupper((c))
	        #define XISALPHA(c)     isalpha((c))
	    #endif
	    /* needed by wolfSSL_check_domain_name() */
	    #define XTOLOWER(c)      tolower((c))
	#endif


	/* memory allocation types for user hints */
	enum {
        DYNAMIC_TYPE_CA           = 1,
        DYNAMIC_TYPE_CERT         = 2,
        DYNAMIC_TYPE_KEY          = 3,
        DYNAMIC_TYPE_FILE         = 4,
        DYNAMIC_TYPE_SUBJECT_CN   = 5,
        DYNAMIC_TYPE_PUBLIC_KEY   = 6,
        DYNAMIC_TYPE_SIGNER       = 7,
        DYNAMIC_TYPE_NONE         = 8,
        DYNAMIC_TYPE_BIGINT       = 9,
        DYNAMIC_TYPE_RSA          = 10,
        DYNAMIC_TYPE_METHOD       = 11,
        DYNAMIC_TYPE_OUT_BUFFER   = 12,
        DYNAMIC_TYPE_IN_BUFFER    = 13,
        DYNAMIC_TYPE_INFO         = 14,
        DYNAMIC_TYPE_DH           = 15,
        DYNAMIC_TYPE_DOMAIN       = 16,
        DYNAMIC_TYPE_SSL          = 17,
        DYNAMIC_TYPE_CTX          = 18,
        DYNAMIC_TYPE_WRITEV       = 19,
        DYNAMIC_TYPE_OPENSSL      = 20,
        DYNAMIC_TYPE_DSA          = 21,
        DYNAMIC_TYPE_CRL          = 22,
        DYNAMIC_TYPE_REVOKED      = 23,
        DYNAMIC_TYPE_CRL_ENTRY    = 24,
        DYNAMIC_TYPE_CERT_MANAGER = 25,
        DYNAMIC_TYPE_CRL_MONITOR  = 26,
        DYNAMIC_TYPE_OCSP_STATUS  = 27,
        DYNAMIC_TYPE_OCSP_ENTRY   = 28,
        DYNAMIC_TYPE_ALTNAME      = 29,
        DYNAMIC_TYPE_SUITES       = 30,
        DYNAMIC_TYPE_CIPHER       = 31,
        DYNAMIC_TYPE_RNG          = 32,
        DYNAMIC_TYPE_ARRAYS       = 33,
        DYNAMIC_TYPE_DTLS_POOL    = 34,
        DYNAMIC_TYPE_SOCKADDR     = 35,
        DYNAMIC_TYPE_LIBZ         = 36,
        DYNAMIC_TYPE_ECC          = 37,
        DYNAMIC_TYPE_TMP_BUFFER   = 38,
        DYNAMIC_TYPE_DTLS_MSG     = 39,
        DYNAMIC_TYPE_X509         = 40,
        DYNAMIC_TYPE_TLSX         = 41,
        DYNAMIC_TYPE_OCSP         = 42,
        DYNAMIC_TYPE_SIGNATURE    = 43,
        DYNAMIC_TYPE_HASHES       = 44,
        DYNAMIC_TYPE_SRP          = 45,
        DYNAMIC_TYPE_COOKIE_PWD   = 46,
        DYNAMIC_TYPE_USER_CRYPTO  = 47,
        DYNAMIC_TYPE_OCSP_REQUEST = 48,
        DYNAMIC_TYPE_X509_EXT     = 49,
        DYNAMIC_TYPE_X509_STORE   = 50,
        DYNAMIC_TYPE_X509_CTX     = 51,
        DYNAMIC_TYPE_URL          = 52,
        DYNAMIC_TYPE_DTLS_FRAG    = 53,
        DYNAMIC_TYPE_DTLS_BUFFER  = 54,
        DYNAMIC_TYPE_SESSION_TICK = 55,
        DYNAMIC_TYPE_PKCS         = 56,
        DYNAMIC_TYPE_MUTEX        = 57,
        DYNAMIC_TYPE_PKCS7        = 58,
        DYNAMIC_TYPE_AES_BUFFER   = 59,
        DYNAMIC_TYPE_WOLF_BIGINT  = 60,
        DYNAMIC_TYPE_ASN1         = 61,
        DYNAMIC_TYPE_LOG          = 62,
        DYNAMIC_TYPE_WRITEDUP     = 63,
        DYNAMIC_TYPE_PRIVATE_KEY  = 64,
        DYNAMIC_TYPE_HMAC         = 65,
        DYNAMIC_TYPE_ASYNC        = 66,
        DYNAMIC_TYPE_ASYNC_NUMA   = 67,
        DYNAMIC_TYPE_ASYNC_NUMA64 = 68,
        DYNAMIC_TYPE_CURVE25519   = 69,
        DYNAMIC_TYPE_ED25519      = 70,
        DYNAMIC_TYPE_SECRET       = 71,
        DYNAMIC_TYPE_DIGEST       = 72,
        DYNAMIC_TYPE_RSA_BUFFER   = 73,
        DYNAMIC_TYPE_DCERT        = 74,
        DYNAMIC_TYPE_STRING       = 75,
        DYNAMIC_TYPE_PEM          = 76,
        DYNAMIC_TYPE_DER          = 77,
        DYNAMIC_TYPE_CERT_EXT     = 78,
        DYNAMIC_TYPE_ALPN         = 79,
        DYNAMIC_TYPE_ENCRYPTEDINFO= 80,
        DYNAMIC_TYPE_DIRCTX       = 81,
        DYNAMIC_TYPE_HASHCTX      = 82,
        DYNAMIC_TYPE_SEED         = 83,
        DYNAMIC_TYPE_SYMETRIC_KEY = 84,
        DYNAMIC_TYPE_ECC_BUFFER   = 85,
        DYNAMIC_TYPE_QSH          = 86,
        DYNAMIC_TYPE_SALT         = 87,
        DYNAMIC_TYPE_HASH_TMP     = 88,
	};

	/* max error buffer string size */
	enum {
	    WOLFSSL_MAX_ERROR_SZ = 80
	};

	/* stack protection */
	enum {
	    MIN_STACK_BUFFER = 8
	};



	/* settings detection for compile vs runtime math incompatibilities */
	enum {
	#if !defined(USE_FAST_MATH) && !defined(SIZEOF_LONG) && !defined(SIZEOF_LONG_LONG)
	    CTC_SETTINGS = 0x0
	#elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
	    CTC_SETTINGS = 0x1
	#elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
	    CTC_SETTINGS = 0x2
	#elif !defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 4)
	    CTC_SETTINGS = 0x4
	#elif defined(USE_FAST_MATH) && !defined(SIZEOF_LONG) && !defined(SIZEOF_LONG_LONG)
	    CTC_SETTINGS = 0x8
	#elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG) && (SIZEOF_LONG == 8)
	    CTC_SETTINGS = 0x10
	#elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 8)
	    CTC_SETTINGS = 0x20
	#elif defined(USE_FAST_MATH) && defined(SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG == 4)
	    CTC_SETTINGS = 0x40
	#else
	    #error "bad math long / long long settings"
	#endif
	};


	WOLFSSL_API word32 CheckRunTimeSettings(void);

	/* If user uses RSA, DH, DSA, or ECC math lib directly then fast math and long
	   types need to match at compile time and run time, CheckCtcSettings will
	   return 1 if a match otherwise 0 */
	#define CheckCtcSettings() (CTC_SETTINGS == CheckRunTimeSettings())

	/* invalid device id */
	#define INVALID_DEVID    -2


    /* AESNI requires alignment and ARMASM gains some performance from it */
    #if defined(WOLFSSL_AESNI) || defined(WOLFSSL_ARMASM) || defined(USE_INTEL_SPEEDUP)
        #if !defined(ALIGN16)
            #if defined(__GNUC__)
                #define ALIGN16 __attribute__ ( (aligned (16)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN16 __declspec (align (16))
            #else
                #define ALIGN16
            #endif
        #endif /* !ALIGN16 */

        #if !defined (ALIGN32)
            #if defined (__GNUC__)
                #define ALIGN32 __attribute__ ( (aligned (32)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN32 __declspec (align (32))
            #else
                #define ALIGN32
            #endif
        #endif

        #if !defined(ALIGN32)
            #if defined(__GNUC__)
                #define ALIGN32 __attribute__ ( (aligned (32)))
            #elif defined(_MSC_VER)
                /* disable align warning, we want alignment ! */
                #pragma warning(disable: 4324)
                #define ALIGN32 __declspec (align (32))
            #else
                #define ALIGN32
            #endif
        #endif /* !ALIGN32 */

        #if defined(__GNUC__)
            #define ALIGN128 __attribute__ ( (aligned (128)))
        #elif defined(_MSC_VER)
            /* disable align warning, we want alignment ! */
            #pragma warning(disable: 4324)
            #define ALIGN128 __declspec (align (128))
        #else
            #define ALIGN128
        #endif

        #if defined(__GNUC__)
            #define ALIGN256 __attribute__ ( (aligned (256)))
        #elif defined(_MSC_VER)
            /* disable align warning, we want alignment ! */
            #pragma warning(disable: 4324)
            #define ALIGN256 __declspec (align (256))
        #else
            #define ALIGN256
        #endif

    #else
        #ifndef ALIGN16
            #define ALIGN16
        #endif
        #ifndef ALIGN32
            #define ALIGN32
        #endif
        #ifndef ALIGN128
            #define ALIGN128
        #endif
        #ifndef ALIGN256
            #define ALIGN256
        #endif
    #endif /* WOLFSSL_AESNI || WOLFSSL_ARMASM */


    #ifndef TRUE
        #define TRUE  1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif


    #ifdef WOLFSSL_RIOT_OS
        #define EXIT_TEST(ret) exit(ret)
    #elif defined(HAVE_STACK_SIZE)
        #define EXIT_TEST(ret) return (void*)((size_t)(ret))
    #else
        #define EXIT_TEST(ret) return ret
    #endif

	#ifdef __cplusplus
	    }   /* extern "C" */
	#endif

#endif /* WOLF_CRYPT_TYPES_H */
