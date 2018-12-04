/* sha256.c
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


/* code submitted by raphael.huck@efixo.com */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if !defined(NO_SHA256) && !defined(WOLFSSL_ARMASM)

#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/cpuid.h>

/* fips wrapper calls, user can call direct */
#ifdef HAVE_FIPS

    int wc_InitSha256(wc_Sha256* sha)
    {
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha256_fips(sha);
    }
    int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha256_fips(sha);
    }
    int wc_Sha256Update(wc_Sha256* sha, const byte* data, word32 len)
    {
        if (sha == NULL ||  (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        if (data == NULL && len == 0) {
            /* valid, but do nothing */
            return 0;
        }

        return Sha256Update_fips(sha, data, len);
    }
    int wc_Sha256Final(wc_Sha256* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }
        return Sha256Final_fips(sha, out);
    }
    void wc_Sha256Free(wc_Sha256* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

#else /* else build without fips */


#if defined(WOLFSSL_TI_HASH)
    /* #include <wolfcrypt/src/port/ti/ti-hash.c> included by wc_port.c */
#else

#include <wolfssl/wolfcrypt/logging.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#if defined(USE_INTEL_SPEEDUP)
    #define HAVE_INTEL_AVX1

    #if defined(__GNUC__) && ((__GNUC__ < 4) || \
                              (__GNUC__ == 4 && __GNUC_MINOR__ <= 8))
        #define NO_AVX2_SUPPORT
    #endif
    #if defined(__clang__) && ((__clang_major__ < 3) || \
                               (__clang_major__ == 3 && __clang_minor__ <= 5))
        #define NO_AVX2_SUPPORT
    #elif defined(__clang__) && defined(NO_AVX2_SUPPORT)
        #undef NO_AVX2_SUPPORT
    #endif

    #define HAVE_INTEL_AVX1
    #ifndef NO_AVX2_SUPPORT
        #define HAVE_INTEL_AVX2
    #endif
#endif /* USE_INTEL_SPEEDUP */

#if defined(HAVE_INTEL_AVX2)
    #define HAVE_INTEL_RORX
#endif


#if !defined(WOLFSSL_PIC32MZ_HASH) && !defined(STM32_HASH_SHA2) && \
    (!defined(WOLFSSL_IMX6_CAAM) || defined(NO_IMX6_CAAM_HASH))
static int InitSha256(wc_Sha256* sha256)
{
    int ret = 0;

    if (sha256 == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(sha256->digest, 0, sizeof(sha256->digest));
    sha256->digest[0] = 0x6A09E667L;
    sha256->digest[1] = 0xBB67AE85L;
    sha256->digest[2] = 0x3C6EF372L;
    sha256->digest[3] = 0xA54FF53AL;
    sha256->digest[4] = 0x510E527FL;
    sha256->digest[5] = 0x9B05688CL;
    sha256->digest[6] = 0x1F83D9ABL;
    sha256->digest[7] = 0x5BE0CD19L;

    sha256->buffLen = 0;
    sha256->loLen   = 0;
    sha256->hiLen   = 0;

    return ret;
}
#endif


/* Hardware Acceleration */
#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)

    /* in case intel instructions aren't available, plus we need the K[] global */
    #define NEED_SOFT_SHA256

    /*****
    Intel AVX1/AVX2 Macro Control Structure

    #define HAVE_INTEL_AVX1
    #define HAVE_INTEL_AVX2

    #define HAVE_INTEL_RORX


    int InitSha256(wc_Sha256* sha256) {
         Save/Recover XMM, YMM
         ...
    }

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
      Transform_Sha256(); Function prototype
    #else
      Transform_Sha256() {   }
      int Sha256Final() {
         Save/Recover XMM, YMM
         ...
      }
    #endif

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
        #if defined(HAVE_INTEL_RORX
             #define RND with rorx instuction
        #else
            #define RND
        #endif
    #endif

    #if defined(HAVE_INTEL_AVX1)

       #define XMM Instructions/inline asm

       int Transform_Sha256() {
           Stitched Message Sched/Round
        }

    #elif defined(HAVE_INTEL_AVX2)

      #define YMM Instructions/inline asm

      int Transform_Sha256() {
          More granural Stitched Message Sched/Round
      }

    #endif

    */

    /* Each platform needs to query info type 1 from cpuid to see if aesni is
     * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
     */

    /* #if defined(HAVE_INTEL_AVX1/2) at the tail of sha256 */
    static int Transform_Sha256(wc_Sha256* sha256);
    #if defined(HAVE_INTEL_AVX1)
        static int Transform_Sha256_AVX1(wc_Sha256 *sha256);
        static int Transform_Sha256_AVX1_Len(wc_Sha256* sha256, word32 len);
    #endif
    #if defined(HAVE_INTEL_AVX2)
        static int Transform_Sha256_AVX2(wc_Sha256 *sha256);
        static int Transform_Sha256_AVX2_Len(wc_Sha256* sha256, word32 len);
        #ifdef HAVE_INTEL_RORX
        static int Transform_Sha256_AVX1_RORX(wc_Sha256 *sha256);
        static int Transform_Sha256_AVX1_RORX_Len(wc_Sha256* sha256, word32 len);
        static int Transform_Sha256_AVX2_RORX(wc_Sha256 *sha256);
        static int Transform_Sha256_AVX2_RORX_Len(wc_Sha256* sha256, word32 len);
        #endif
    #endif
    static int (*Transform_Sha256_p)(wc_Sha256* sha256);
                                                       /* = _Transform_Sha256 */
    static int (*Transform_Sha256_Len_p)(wc_Sha256* sha256, word32 len);
                                                                    /* = NULL */
    static int transform_check = 0;
    static word32 intel_flags;
    #define XTRANSFORM(S)         (*Transform_Sha256_p)((S))
    #define XTRANSFORM_LEN(S, L)  (*Transform_Sha256_Len_p)((S),(L))

    static void Sha256_SetTransform(void)
    {

        if (transform_check)
            return;

        intel_flags = cpuid_get_flags();

    #ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_AVX2(intel_flags)) {
        #ifdef HAVE_INTEL_RORX
            if (IS_INTEL_BMI2(intel_flags)) {
                Transform_Sha256_p = Transform_Sha256_AVX2_RORX;
                Transform_Sha256_Len_p = Transform_Sha256_AVX2_RORX_Len;
            }
            else
        #endif
            if (1)
            {
                Transform_Sha256_p = Transform_Sha256_AVX2;
                Transform_Sha256_Len_p = Transform_Sha256_AVX2_Len;
            }
        #ifdef HAVE_INTEL_RORX
            else {
                Transform_Sha256_p = Transform_Sha256_AVX1_RORX;
                Transform_Sha256_Len_p = Transform_Sha256_AVX1_RORX_Len;
            }
        #endif
        }
        else
    #endif
    #ifdef HAVE_INTEL_AVX1
        if (IS_INTEL_AVX1(intel_flags)) {
            Transform_Sha256_p = Transform_Sha256_AVX1;
            Transform_Sha256_Len_p = Transform_Sha256_AVX1_Len;
        }
        else
    #endif
        {
            Transform_Sha256_p = Transform_Sha256;
            Transform_Sha256_Len_p = NULL;
        }

        transform_check = 1;
    }

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        sha256->heap = heap;

        ret = InitSha256(sha256);
        if (ret != 0)
            return ret;

        /* choose best Transform function under this runtime environment */
        Sha256_SetTransform();

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        ret = wolfAsync_DevCtxInit(&sha256->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA256, sha256->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return ret;
    }

#elif defined(FREESCALE_LTC_SHA)
    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        (void)heap;
        (void)devId;

        LTC_HASH_Init(LTC_BASE, &sha256->ctx, kLTC_Sha256, NULL, 0);

        return 0;
    }

#elif defined(FREESCALE_MMCAU_SHA)

    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
        #include "cau_api.h"
    #else
        #include "fsl_mmcau.h"
    #endif

    #define XTRANSFORM(S)        Transform_Sha256((S))
    #define XTRANSFORM_LEN(S,L)  Transform_Sha256_Len((S),(L))

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;

        (void)heap;
        (void)devId;

        ret = wolfSSL_CryptHwMutexLock();
        if (ret != 0) {
            return ret;
        }
    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
        cau_sha256_initialize_output(sha256->digest);
    #else
        MMCAU_SHA256_InitializeOutput((uint32_t*)sha256->digest);
    #endif
        wolfSSL_CryptHwMutexUnLock();

        sha256->buffLen = 0;
        sha256->loLen   = 0;
        sha256->hiLen   = 0;

        return ret;
    }

    static int Transform_Sha256(wc_Sha256* sha256)
    {
        int ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
    #ifdef FREESCALE_MMCAU_CLASSIC_SHA
            cau_sha256_hash_n((byte*)sha256->buffer, 1, sha256->digest);
    #else
            MMCAU_SHA256_HashN((byte*)sha256->buffer, 1, sha256->digest);
    #endif
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

#elif defined(WOLFSSL_PIC32MZ_HASH)
    #include <wolfssl/wolfcrypt/port/pic32/pic32mz-crypt.h>

#elif defined(STM32_HASH_SHA2)

    /* Supports CubeMX HAL or Standard Peripheral Library */

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        (void)devId;
        (void)heap;

        wc_Stm32_Hash_Init(&sha256->stmCtx);
        return 0;
    }

    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        int ret = 0;

        if (sha256 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Update(&sha256->stmCtx,
                HASH_AlgoSelection_SHA256, data, len);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret = 0;

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Final(&sha256->stmCtx,
                HASH_AlgoSelection_SHA256, hash, WC_SHA256_DIGEST_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }

        (void)wc_InitSha256(sha256); /* reset state */

        return ret;
    }

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha256.c */
#else
    #define NEED_SOFT_SHA256

    int wc_InitSha256_ex(wc_Sha256* sha256, void* heap, int devId)
    {
        int ret = 0;
        if (sha256 == NULL)
            return BAD_FUNC_ARG;

        sha256->heap = heap;

        ret = InitSha256(sha256);
        if (ret != 0)
            return ret;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        ret = wolfAsync_DevCtxInit(&sha256->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA256, sha256->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return ret;
    }
#endif /* End Hardware Acceleration */

#ifdef NEED_SOFT_SHA256

    static const ALIGN32 word32 K[64] = {
        0x428A2F98L, 0x71374491L, 0xB5C0FBCFL, 0xE9B5DBA5L, 0x3956C25BL,
        0x59F111F1L, 0x923F82A4L, 0xAB1C5ED5L, 0xD807AA98L, 0x12835B01L,
        0x243185BEL, 0x550C7DC3L, 0x72BE5D74L, 0x80DEB1FEL, 0x9BDC06A7L,
        0xC19BF174L, 0xE49B69C1L, 0xEFBE4786L, 0x0FC19DC6L, 0x240CA1CCL,
        0x2DE92C6FL, 0x4A7484AAL, 0x5CB0A9DCL, 0x76F988DAL, 0x983E5152L,
        0xA831C66DL, 0xB00327C8L, 0xBF597FC7L, 0xC6E00BF3L, 0xD5A79147L,
        0x06CA6351L, 0x14292967L, 0x27B70A85L, 0x2E1B2138L, 0x4D2C6DFCL,
        0x53380D13L, 0x650A7354L, 0x766A0ABBL, 0x81C2C92EL, 0x92722C85L,
        0xA2BFE8A1L, 0xA81A664BL, 0xC24B8B70L, 0xC76C51A3L, 0xD192E819L,
        0xD6990624L, 0xF40E3585L, 0x106AA070L, 0x19A4C116L, 0x1E376C08L,
        0x2748774CL, 0x34B0BCB5L, 0x391C0CB3L, 0x4ED8AA4AL, 0x5B9CCA4FL,
        0x682E6FF3L, 0x748F82EEL, 0x78A5636FL, 0x84C87814L, 0x8CC70208L,
        0x90BEFFFAL, 0xA4506CEBL, 0xBEF9A3F7L, 0xC67178F2L
    };

    #define Ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
    #define Maj(x,y,z)      ((((x) | (y)) & (z)) | ((x) & (y)))
    #define R(x, n)         (((x) & 0xFFFFFFFFU) >> (n))

    #define S(x, n)         rotrFixed(x, n)
    #define Sigma0(x)       (S(x, 2)  ^ S(x, 13) ^ S(x, 22))
    #define Sigma1(x)       (S(x, 6)  ^ S(x, 11) ^ S(x, 25))
    #define Gamma0(x)       (S(x, 7)  ^ S(x, 18) ^ R(x, 3))
    #define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

    #define a(i) S[(0-i) & 7]
    #define b(i) S[(1-i) & 7]
    #define c(i) S[(2-i) & 7]
    #define d(i) S[(3-i) & 7]
    #define e(i) S[(4-i) & 7]
    #define f(i) S[(5-i) & 7]
    #define g(i) S[(6-i) & 7]
    #define h(i) S[(7-i) & 7]

    #define RND(j) \
         t0 = h(j) + Sigma1(e(j)) + Ch(e(j), f(j), g(j)) + K[i+j] + W[i+j]; \
         t1 = Sigma0(a(j)) + Maj(a(j), b(j), c(j)); \
         d(j) += t0; \
         h(j)  = t0 + t1

    #ifndef XTRANSFORM
         #define XTRANSFORM(S)        Transform_Sha256((S))
         #define XTRANSFORM_LEN(S,L)  Transform_Sha256_Len((S),(L))
    #endif

    static int Transform_Sha256(wc_Sha256* sha256)
    {
        word32 S[8], t0, t1;
        int i;

    #ifdef WOLFSSL_SMALL_STACK
        word32* W;

        W = (word32*)XMALLOC(sizeof(word32) * WC_SHA256_BLOCK_SIZE, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (W == NULL)
            return MEMORY_E;
    #else
        word32 W[WC_SHA256_BLOCK_SIZE];
    #endif

        /* Copy context->state[] to working vars */
        for (i = 0; i < 8; i++)
            S[i] = sha256->digest[i];

        for (i = 0; i < 16; i++)
            W[i] = sha256->buffer[i];

        for (i = 16; i < WC_SHA256_BLOCK_SIZE; i++)
            W[i] = Gamma1(W[i-2]) + W[i-7] + Gamma0(W[i-15]) + W[i-16];

    #ifdef USE_SLOW_SHA256
        /* not unrolled - ~2k smaller and ~25% slower */
        for (i = 0; i < WC_SHA256_BLOCK_SIZE; i += 8) {
            int j;
            for (j = 0; j < 8; j++) { /* braces needed here for macros {} */
                RND(j);
            }
        }
    #else
        /* partially loop unrolled */
        for (i = 0; i < WC_SHA256_BLOCK_SIZE; i += 8) {
            RND(0); RND(1); RND(2); RND(3);
            RND(4); RND(5); RND(6); RND(7);
        }
    #endif /* USE_SLOW_SHA256 */

        /* Add the working vars back into digest state[] */
        for (i = 0; i < 8; i++) {
            sha256->digest[i] += S[i];
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return 0;
    }
#endif
/* End wc_ software implementation */


#ifdef XTRANSFORM

    static INLINE void AddLength(wc_Sha256* sha256, word32 len)
    {
        word32 tmp = sha256->loLen;
        if ((sha256->loLen += len) < tmp)
            sha256->hiLen++;                       /* carry low to high */
    }

    static INLINE int Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        int ret = 0;
        byte* local;

        if (sha256 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        if (data == NULL && len == 0) {
            /* valid, but do nothing */
            return 0;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        if (sha256->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA256) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha256(&sha256->asyncDev, NULL, data, len);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        /* do block size increments */
        local = (byte*)sha256->buffer;

        /* check that internal buffLen is valid */
        if (sha256->buffLen >= WC_SHA256_BLOCK_SIZE)
            return BUFFER_E;

        if (sha256->buffLen > 0) {
            word32 add = min(len, WC_SHA256_BLOCK_SIZE - sha256->buffLen);
            XMEMCPY(&local[sha256->buffLen], data, add);

            sha256->buffLen += add;
            data            += add;
            len             -= add;

            if (sha256->buffLen == WC_SHA256_BLOCK_SIZE) {
        #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
            #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
                if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
            #endif
                {
                    ByteReverseWords(sha256->buffer, sha256->buffer,
                                                          WC_SHA256_BLOCK_SIZE);
                }
        #endif
                ret = XTRANSFORM(sha256);
                if (ret == 0) {
                    AddLength(sha256, WC_SHA256_BLOCK_SIZE);
                    sha256->buffLen = 0;
                }
                else
                    len = 0;
            }
        }

    #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        if (Transform_Sha256_Len_p != NULL) {
            word32 blocksLen = len & ~(WC_SHA256_BLOCK_SIZE-1);

            if (blocksLen > 0) {
                AddLength(sha256, blocksLen);
                sha256->data = data;
                /* Byte reversal performed in function if required. */
                XTRANSFORM_LEN(sha256, blocksLen);
                data += blocksLen;
                len  -= blocksLen;
            }
        }
        else
    #endif
    #if !defined(LITTLE_ENDIAN_ORDER) || defined(FREESCALE_MMCAU_SHA) || \
                            defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        {
            word32 blocksLen = len & ~(WC_SHA256_BLOCK_SIZE-1);

            AddLength(sha256, blocksLen);
            while (len >= WC_SHA256_BLOCK_SIZE) {
                XMEMCPY(local, data, WC_SHA256_BLOCK_SIZE);

                data += WC_SHA256_BLOCK_SIZE;
                len  -= WC_SHA256_BLOCK_SIZE;

                /* Byte reversal performed in function if required. */
                ret = XTRANSFORM(sha256);
                if (ret != 0)
                    break;
            }
        }
    #else
        {
            word32 blocksLen = len & ~(WC_SHA256_BLOCK_SIZE-1);

            AddLength(sha256, blocksLen);
            while (len >= WC_SHA256_BLOCK_SIZE) {
                XMEMCPY(local, data, WC_SHA256_BLOCK_SIZE);

                data += WC_SHA256_BLOCK_SIZE;
                len  -= WC_SHA256_BLOCK_SIZE;

                ByteReverseWords(sha256->buffer, sha256->buffer,
                                                          WC_SHA256_BLOCK_SIZE);
                ret = XTRANSFORM(sha256);
                if (ret != 0)
                    break;
            }
        }
    #endif

        if (len > 0) {
            XMEMCPY(local, data, len);
            sha256->buffLen = len;
        }

        return ret;
    }

    int wc_Sha256Update(wc_Sha256* sha256, const byte* data, word32 len)
    {
        return Sha256Update(sha256, data, len);
    }

    static INLINE int Sha256Final(wc_Sha256* sha256)
    {

        int ret;
        byte* local = (byte*)sha256->buffer;

        if (sha256 == NULL) {
            return BAD_FUNC_ARG;
        }

        AddLength(sha256, sha256->buffLen);  /* before adding pads */
        local[sha256->buffLen++] = 0x80;     /* add 1 */

        /* pad with zeros */
        if (sha256->buffLen > WC_SHA256_PAD_SIZE) {
            XMEMSET(&local[sha256->buffLen], 0,
                WC_SHA256_BLOCK_SIZE - sha256->buffLen);
            sha256->buffLen += WC_SHA256_BLOCK_SIZE - sha256->buffLen;

            {
        #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
            #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
                if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
            #endif
                {
                    ByteReverseWords(sha256->buffer, sha256->buffer,
                                                          WC_SHA256_BLOCK_SIZE);
                }
        #endif
            }

            ret = XTRANSFORM(sha256);
            if (ret != 0)
                return ret;

            sha256->buffLen = 0;
        }
        XMEMSET(&local[sha256->buffLen], 0, WC_SHA256_PAD_SIZE - sha256->buffLen);

        /* put lengths in bits */
        sha256->hiLen = (sha256->loLen >> (8 * sizeof(sha256->loLen) - 3)) +
                                                         (sha256->hiLen << 3);
        sha256->loLen = sha256->loLen << 3;

        /* store lengths */
    #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU_SHA)
        #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
            if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
        #endif
            {
                ByteReverseWords(sha256->buffer, sha256->buffer,
                    WC_SHA256_BLOCK_SIZE);
            }
    #endif
        /* ! length ordering dependent on digest endian type ! */
        XMEMCPY(&local[WC_SHA256_PAD_SIZE], &sha256->hiLen, sizeof(word32));
        XMEMCPY(&local[WC_SHA256_PAD_SIZE + sizeof(word32)], &sha256->loLen,
                sizeof(word32));

    #if defined(FREESCALE_MMCAU_SHA) || defined(HAVE_INTEL_AVX1) || \
        defined(HAVE_INTEL_AVX2)
        /* Kinetis requires only these bytes reversed */
        #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
            if (IS_INTEL_AVX1(intel_flags) || IS_INTEL_AVX2(intel_flags))
        #endif
            {
                ByteReverseWords(
                    &sha256->buffer[WC_SHA256_PAD_SIZE / sizeof(word32)],
                    &sha256->buffer[WC_SHA256_PAD_SIZE / sizeof(word32)],
                    2 * sizeof(word32));
            }
    #endif

        return XTRANSFORM(sha256);
    }

    int wc_Sha256FinalRaw(wc_Sha256* sha256, byte* hash)
    {
    #ifdef LITTLE_ENDIAN_ORDER
        word32 digest[WC_SHA256_DIGEST_SIZE / sizeof(word32)];
    #endif

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords((word32*)digest, (word32*)sha256->digest,
                                                         WC_SHA256_DIGEST_SIZE);
        XMEMCPY(hash, digest, WC_SHA256_DIGEST_SIZE);
    #else
        XMEMCPY(hash, sha256->digest, WC_SHA256_DIGEST_SIZE);
    #endif

        return 0;
    }

    int wc_Sha256Final(wc_Sha256* sha256, byte* hash)
    {
        int ret;

        if (sha256 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
        if (sha256->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA256) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha256(&sha256->asyncDev, hash, NULL,
                                            WC_SHA256_DIGEST_SIZE);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        ret = Sha256Final(sha256);
        if (ret != 0)
            return ret;

    #if defined(LITTLE_ENDIAN_ORDER)
        ByteReverseWords(sha256->digest, sha256->digest, WC_SHA256_DIGEST_SIZE);
    #endif
        XMEMCPY(hash, sha256->digest, WC_SHA256_DIGEST_SIZE);

        return InitSha256(sha256);  /* reset state */
    }

#endif /* XTRANSFORM */


#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)

#define _LOAD_DIGEST()                     \
    "movl	  (%[sha256]), %%r8d \n\t" \
    "movl	 4(%[sha256]), %%r9d \n\t" \
    "movl	 8(%[sha256]), %%r10d\n\t" \
    "movl	12(%[sha256]), %%r11d\n\t" \
    "movl	16(%[sha256]), %%r12d\n\t" \
    "movl	20(%[sha256]), %%r13d\n\t" \
    "movl	24(%[sha256]), %%r14d\n\t" \
    "movl	28(%[sha256]), %%r15d\n\t"

#define _STORE_ADD_DIGEST()                \
    "addl	%%r8d ,   (%[sha256])\n\t" \
    "addl	%%r9d ,  4(%[sha256])\n\t" \
    "addl	%%r10d,  8(%[sha256])\n\t" \
    "addl	%%r11d, 12(%[sha256])\n\t" \
    "addl	%%r12d, 16(%[sha256])\n\t" \
    "addl	%%r13d, 20(%[sha256])\n\t" \
    "addl	%%r14d, 24(%[sha256])\n\t" \
    "addl	%%r15d, 28(%[sha256])\n\t"

#define _ADD_DIGEST()                      \
    "addl	  (%[sha256]), %%r8d \n\t" \
    "addl	 4(%[sha256]), %%r9d \n\t" \
    "addl	 8(%[sha256]), %%r10d\n\t" \
    "addl	12(%[sha256]), %%r11d\n\t" \
    "addl	16(%[sha256]), %%r12d\n\t" \
    "addl	20(%[sha256]), %%r13d\n\t" \
    "addl	24(%[sha256]), %%r14d\n\t" \
    "addl	28(%[sha256]), %%r15d\n\t"

#define _STORE_DIGEST()                    \
    "movl	%%r8d ,   (%[sha256])\n\t" \
    "movl	%%r9d ,  4(%[sha256])\n\t" \
    "movl	%%r10d,  8(%[sha256])\n\t" \
    "movl	%%r11d, 12(%[sha256])\n\t" \
    "movl	%%r12d, 16(%[sha256])\n\t" \
    "movl	%%r13d, 20(%[sha256])\n\t" \
    "movl	%%r14d, 24(%[sha256])\n\t" \
    "movl	%%r15d, 28(%[sha256])\n\t"

#define LOAD_DIGEST() \
    _LOAD_DIGEST()

#define STORE_ADD_DIGEST() \
    _STORE_ADD_DIGEST()

#define ADD_DIGEST() \
    _ADD_DIGEST()

#define STORE_DIGEST() \
    _STORE_DIGEST()


#define S_0 %r8d
#define S_1 %r9d
#define S_2 %r10d
#define S_3 %r11d
#define S_4 %r12d
#define S_5 %r13d
#define S_6 %r14d
#define S_7 %r15d

#define L1  "%%edx"
#define L2  "%%ecx"
#define L3  "%%eax"
#define L4  "%%ebx"
#define WK  "%%rsp"

#define WORK_REGS  "eax", "ebx", "ecx", "edx"
#define STATE_REGS "r8","r9","r10","r11","r12","r13","r14","r15"
#define XMM_REGS   "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",    \
                   "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13"

#if defined(HAVE_INTEL_RORX)
#define RND_STEP_RORX_0_1(a, b, c, d, e, f, g, h, i) \
    /* L3 = f */                                     \
    "movl	%" #f ", " L3 "\n\t"                 \
    /* L2 = e>>>11 */                                \
    "rorx	$11, %" #e ", " L2 "\n\t"            \
    /* h += w_k */                                   \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"     \

#define RND_STEP_RORX_0_2(a, b, c, d, e, f, g, h, i) \
    /* L2 = (e>>>6) ^ (e>>>11) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L3 = f ^ g */                                 \
    "xorl	%" #g ", " L3 "\n\t"                 \
    /* L1 = e>>>25 */                                \
    "rorx	$25, %" #e ", " L1 "\n\t"            \

#define RND_STEP_RORX_0_3(a, b, c, d, e, f, g, h, i) \
    /* L3 = (f ^ g) & e */                           \
    "andl	%" #e ", " L3 "\n\t"                 \
    /* L1 = Sigma1(e) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L2 = a>>>13 */                                \
    "rorx	$13, %" #a ", " L2 "\n\t"            \

#define RND_STEP_RORX_0_4(a, b, c, d, e, f, g, h, i) \
    /* h += Sigma1(e) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L1 = a>>>2 */                                 \
    "rorx	$2, %" #a ", " L1 "\n\t"             \
    /* L3 = Ch(e,f,g) */                             \
    "xorl	%" #g ", " L3 "\n\t"                 \

#define RND_STEP_RORX_0_5(a, b, c, d, e, f, g, h, i) \
    /* L2 = (a>>>2) ^ (a>>>13) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L1 = a>>>22 */                                \
    "rorx	$22, %" #a ", " L1 "\n\t"            \
    /* h += Ch(e,f,g) */                             \
    "addl	" L3 ", %" #h "\n\t"                 \

#define RND_STEP_RORX_0_6(a, b, c, d, e, f, g, h, i) \
    /* L1 = Sigma0(a) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L3 = b */                                     \
    "movl	%" #b ", " L3 "\n\t"                 \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */       \
    "addl	%" #h ", %" #d "\n\t"                \

#define RND_STEP_RORX_0_7(a, b, c, d, e, f, g, h, i) \
    /* L3 = a ^ b */                                 \
    "xorl	%" #a ", " L3 "\n\t"                 \
    /* h += Sigma0(a) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L4 = (a ^ b) & (b ^ c) */                     \
    "andl	" L3 ", " L4 "\n\t"                  \

#define RND_STEP_RORX_0_8(a, b, c, d, e, f, g, h, i) \
    /* L4 = Maj(a,b,c) */                            \
    "xorl	%" #b ", " L4 "\n\t"                 \
    /* L1 = d>>>6 (= e>>>6 next RND) */              \
    "rorx	$6, %" #d ", " L1 "\n\t"             \
    /* h += Maj(a,b,c) */                            \
    "addl	" L4 ", %" #h "\n\t"                 \

#define RND_STEP_RORX_1_1(a, b, c, d, e, f, g, h, i) \
    /* L4 = f */                                     \
    "movl	%" #f ", " L4 "\n\t"                 \
    /* L2 = e>>>11 */                                \
    "rorx	$11, %" #e ", " L2 "\n\t"            \
    /* h += w_k */                                   \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"     \

#define RND_STEP_RORX_1_2(a, b, c, d, e, f, g, h, i) \
    /* L2 = (e>>>6) ^ (e>>>11) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L4 = f ^ g */                                 \
    "xorl	%" #g ", " L4 "\n\t"                 \
    /* L1 = e>>>25 */                                \
    "rorx	$25, %" #e ", " L1 "\n\t"            \

#define RND_STEP_RORX_1_3(a, b, c, d, e, f, g, h, i) \
    /* L4 = (f ^ g) & e */                           \
    "andl	%" #e ", " L4 "\n\t"                 \
    /* L1 = Sigma1(e) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L2 = a>>>13 */                                \
    "rorx	$13, %" #a ", " L2 "\n\t"            \

#define RND_STEP_RORX_1_4(a, b, c, d, e, f, g, h, i) \
    /* h += Sigma1(e) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L1 = a>>>2 */                                 \
    "rorx	$2, %" #a ", " L1 "\n\t"             \
    /* L4 = Ch(e,f,g) */                             \
    "xorl	%" #g ", " L4 "\n\t"                 \

#define RND_STEP_RORX_1_5(a, b, c, d, e, f, g, h, i) \
    /* L2 = (a>>>2) ^ (a>>>13) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L1 = a>>>22 */                                \
    "rorx	$22, %" #a ", " L1 "\n\t"            \
    /* h += Ch(e,f,g) */                             \
    "addl	" L4 ", %" #h "\n\t"                 \

#define RND_STEP_RORX_1_6(a, b, c, d, e, f, g, h, i) \
    /* L1 = Sigma0(a) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L4 = b */                                     \
    "movl	%" #b ", " L4 "\n\t"                 \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */       \
    "addl	%" #h ", %" #d "\n\t"                \

#define RND_STEP_RORX_1_7(a, b, c, d, e, f, g, h, i) \
    /* L4 = a ^ b */                                 \
    "xorl	%" #a ", " L4 "\n\t"                 \
    /* h += Sigma0(a) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L3 = (a ^ b) & (b ^ c) */                     \
    "andl	" L4 ", " L3 "\n\t"                  \

#define RND_STEP_RORX_1_8(a, b, c, d, e, f, g, h, i) \
    /* L3 = Maj(a,b,c) */                            \
    "xorl	%" #b ", " L3 "\n\t"                 \
    /* L1 = d>>>6 (= e>>>6 next RND) */              \
    "rorx	$6, %" #d ", " L1 "\n\t"             \
    /* h += Maj(a,b,c) */                            \
    "addl	" L3 ", %" #h "\n\t"                 \

#define _RND_RORX_X_0(a, b, c, d, e, f, g, h, i)     \
    /* L1 = e>>>6 */                                 \
    "rorx	$6, %" #e ", " L1 "\n\t"             \
    /* L2 = e>>>11 */                                \
    "rorx	$11, %" #e ", " L2 "\n\t"            \
    /* Prev RND: h += Maj(a,b,c) */                  \
    "addl	" L3 ", %" #a "\n\t"                 \
    /* h += w_k */                                   \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"     \
    /* L3 = f */                                     \
    "movl	%" #f ", " L3 "\n\t"                 \
    /* L2 = (e>>>6) ^ (e>>>11) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L3 = f ^ g */                                 \
    "xorl	%" #g ", " L3 "\n\t"                 \
    /* L1 = e>>>25 */                                \
    "rorx	$25, %" #e ", " L1 "\n\t"            \
    /* L1 = Sigma1(e) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L3 = (f ^ g) & e */                           \
    "andl	%" #e ", " L3 "\n\t"                 \
    /* h += Sigma1(e) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L1 = a>>>2 */                                 \
    "rorx	$2, %" #a ", " L1 "\n\t"             \
    /* L2 = a>>>13 */                                \
    "rorx	$13, %" #a ", " L2 "\n\t"            \
    /* L3 = Ch(e,f,g) */                             \
    "xorl	%" #g ", " L3 "\n\t"                 \
    /* L2 = (a>>>2) ^ (a>>>13) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L1 = a>>>22 */                                \
    "rorx	$22, %" #a ", " L1 "\n\t"            \
    /* h += Ch(e,f,g) */                             \
    "addl	" L3 ", %" #h "\n\t"                 \
    /* L1 = Sigma0(a) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L3 = b */                                     \
    "movl	%" #b ", " L3 "\n\t"                 \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */       \
    "addl	%" #h ", %" #d "\n\t"                \
    /* L3 = a ^ b */                                 \
    "xorl	%" #a ", " L3 "\n\t"                 \
    /* L4 = (a ^ b) & (b ^ c) */                     \
    "andl	" L3 ", " L4 "\n\t"                  \
    /* h += Sigma0(a) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L4 = Maj(a,b,c) */                            \
    "xorl	%" #b ", " L4 "\n\t"                 \

#define _RND_RORX_X_1(a, b, c, d, e, f, g, h, i)     \
    /* L1 = e>>>6 */                                 \
    "rorx	$6, %" #e ", " L1 "\n\t"             \
    /* L2 = e>>>11 */                                \
    "rorx	$11, %" #e ", " L2 "\n\t"            \
    /* Prev RND: h += Maj(a,b,c) */                  \
    "addl	" L4 ", %" #a "\n\t"                 \
    /* h += w_k */                                   \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"     \
    /* L4 = f */                                     \
    "movl	%" #f ", " L4 "\n\t"                 \
    /* L2 = (e>>>6) ^ (e>>>11) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L4 = f ^ g */                                 \
    "xorl	%" #g ", " L4 "\n\t"                 \
    /* L1 = e>>>25 */                                \
    "rorx	$25, %" #e ", " L1 "\n\t"            \
    /* L1 = Sigma1(e) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L4 = (f ^ g) & e */                           \
    "andl	%" #e ", " L4 "\n\t"                 \
    /* h += Sigma1(e) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L1 = a>>>2 */                                 \
    "rorx	$2, %" #a ", " L1 "\n\t"             \
    /* L2 = a>>>13 */                                \
    "rorx	$13, %" #a ", " L2 "\n\t"            \
    /* L4 = Ch(e,f,g) */                             \
    "xorl	%" #g ", " L4 "\n\t"                 \
    /* L2 = (a>>>2) ^ (a>>>13) */                    \
    "xorl	" L1 ", " L2 "\n\t"                  \
    /* L1 = a>>>22 */                                \
    "rorx	$22, %" #a ", " L1 "\n\t"            \
    /* h += Ch(e,f,g) */                             \
    "addl	" L4 ", %" #h "\n\t"                 \
    /* L1 = Sigma0(a) */                             \
    "xorl	" L2 ", " L1 "\n\t"                  \
    /* L4 = b */                                     \
    "movl	%" #b ", " L4 "\n\t"                 \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */       \
    "addl	%" #h ", %" #d "\n\t"                \
    /* L4 = a ^ b */                                 \
    "xorl	%" #a ", " L4 "\n\t"                 \
    /* L2 = (a ^ b) & (b ^ c) */                     \
    "andl	" L4 ", " L3 "\n\t"                  \
    /* h += Sigma0(a) */                             \
    "addl	" L1 ", %" #h "\n\t"                 \
    /* L3 = Maj(a,b,c) */                            \
    "xorl	%" #b ", " L3 "\n\t"                 \


#define RND_RORX_X_0(a,b,c,d,e,f,g,h,i) \
       _RND_RORX_X_0(a,b,c,d,e,f,g,h,i)
#define RND_RORX_X_1(a,b,c,d,e,f,g,h,i) \
       _RND_RORX_X_1(a,b,c,d,e,f,g,h,i)

#define RND_RORX_X4(a,b,c,d,e,f,g,h,i)    \
        RND_RORX_X_0(a,b,c,d,e,f,g,h,i+0) \
        RND_RORX_X_1(h,a,b,c,d,e,f,g,i+1) \
        RND_RORX_X_0(g,h,a,b,c,d,e,f,i+2) \
        RND_RORX_X_1(f,g,h,a,b,c,d,e,i+3)

#endif /* HAVE_INTEL_RORX */

#define RND_STEP_0_1(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = e>>>14 */                                                 \
    "rorl	$14, " L1 "\n\t"                                      \

#define RND_STEP_0_2(a,b,c,d,e,f,g,h,i)                               \
    /* L3 = b */                                                      \
    "movl	%" #b ", " L3 "\n\t"                                  \
    /* L2 = f */                                                      \
    "movl	%" #f ", " L2 "\n\t"                                  \
    /* h += w_k */                                                    \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"                      \
    /* L2 = f ^ g */                                                  \
    "xorl	%" #g ", " L2 "\n\t"                                  \

#define RND_STEP_0_3(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = (e>>>14) ^ e */                                           \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L2 = (f ^ g) & e */                                            \
    "andl	%" #e ", " L2 "\n\t"                                  \
 
#define RND_STEP_0_4(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = ((e>>>14) ^ e) >>> 5 */                                   \
    "rorl	$5, " L1 "\n\t"                                       \
    /* L2 = Ch(e,f,g) */                                              \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = (((e>>>14) ^ e) >>> 5) ^ e */                             \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* h += Ch(e,f,g) */                                              \
    "addl	" L2 ", %" #h "\n\t"                                  \

#define RND_STEP_0_5(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = ((((e>>>14) ^ e) >>> 5) ^ e) >>> 6 */                     \
    "rorl	$6, " L1 "\n\t"                                       \
    /* L3 = a ^ b (= b ^ c of next RND) */                            \
    "xorl	%" #a ", " L3 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) */                                     \
    "addl	" L1 ", %" #h "\n\t"                                  \
    /* L2 = a */                                                      \
    "movl	%" #a ", " L2 "\n\t"                                  \

#define RND_STEP_0_6(a,b,c,d,e,f,g,h,i)                               \
    /* L3 = (a ^ b) & (b ^ c) */                                      \
    "andl	" L3 ", " L4 "\n\t"                                   \
    /* L2 = a>>>9 */                                                  \
    "rorl	$9, " L2 "\n\t"                                       \
    /* L2 = (a>>>9) ^ a */                                            \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* L1 = Maj(a,b,c) */                                             \
    "xorl	%" #b ", " L4 "\n\t"                                  \

#define RND_STEP_0_7(a,b,c,d,e,f,g,h,i)                               \
    /* L2 = ((a>>>9) ^ a) >>> 11 */                                   \
    "rorl	$11, " L2 "\n\t"                                      \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */                        \
    "addl	%" #h ", %" #d "\n\t"                                 \
    /* L2 = (((a>>>9) ^ a) >>> 11) ^ a */                             \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) + Ch(e,f,g) + Maj(a,b,c) */            \
    "addl	" L4 ", %" #h "\n\t"                                  \

#define RND_STEP_0_8(a,b,c,d,e,f,g,h,i)                               \
    /* L2 = ((((a>>>9) ^ a) >>> 11) ^ a) >>> 2 */                     \
    "rorl	$2, " L2 "\n\t"                                       \
    /* L1 = d (e of next RND) */                                      \
    "movl	%" #d ", " L1 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) Sigma0(a) + Ch(e,f,g) + Maj(a,b,c) */  \
    "addl	" L2 ", %" #h "\n\t"                                  \

#define RND_STEP_1_1(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = e>>>14 */                                                 \
    "rorl	$14, " L1 "\n\t"                                      \
 
#define RND_STEP_1_2(a,b,c,d,e,f,g,h,i)                               \
    /* L3 = b */                                                      \
    "movl	%" #b ", " L4 "\n\t"                                  \
    /* L2 = f */                                                      \
    "movl	%" #f ", " L2 "\n\t"                                  \
    /* h += w_k */                                                    \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"                      \
    /* L2 = f ^ g */                                                  \
    "xorl	%" #g ", " L2 "\n\t"                                  \
 
#define RND_STEP_1_3(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = (e>>>14) ^ e */                                           \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L2 = (f ^ g) & e */                                            \
    "andl	%" #e ", " L2 "\n\t"                                  \
 
#define RND_STEP_1_4(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = ((e>>>14) ^ e) >>> 5 */                                   \
    "rorl	$5, " L1 "\n\t"                                       \
    /* L2 = Ch(e,f,g) */                                              \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = (((e>>>14) ^ e) >>> 5) ^ e */                             \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* h += Ch(e,f,g) */                                              \
    "addl	" L2 ", %" #h "\n\t"                                  \

#define RND_STEP_1_5(a,b,c,d,e,f,g,h,i)                               \
    /* L1 = ((((e>>>14) ^ e) >>> 5) ^ e) >>> 6 */                     \
    "rorl	$6, " L1 "\n\t"                                       \
    /* L4 = a ^ b (= b ^ c of next RND) */                            \
    "xorl	%" #a ", " L4 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) */                                     \
    "addl	" L1 ", %" #h "\n\t"                                  \
    /* L2 = a */                                                      \
    "movl	%" #a ", " L2 "\n\t"                                  \

#define RND_STEP_1_6(a,b,c,d,e,f,g,h,i)                               \
    /* L3 = (a ^ b) & (b ^ c)  */                                     \
    "andl	" L4 ", " L3 "\n\t"                                   \
    /* L2 = a>>>9 */                                                  \
    "rorl	$9, " L2 "\n\t"                                       \
    /* L2 = (a>>>9) ^ a */                                            \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* L1 = Maj(a,b,c) */                                             \
    "xorl	%" #b ", " L3 "\n\t"                                  \

#define RND_STEP_1_7(a,b,c,d,e,f,g,h,i)                               \
    /* L2 = ((a>>>9) ^ a) >>> 11 */                                   \
    "rorl	$11, " L2 "\n\t"                                      \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */                        \
    "addl	%" #h ", %" #d "\n\t"                                 \
    /* L2 = (((a>>>9) ^ a) >>> 11) ^ a */                             \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) + Ch(e,f,g) + Maj(a,b,c) */            \
    "addl	" L3 ", %" #h "\n\t"                                  \

#define RND_STEP_1_8(a,b,c,d,e,f,g,h,i)                               \
    /* L2 = ((((a>>>9) ^ a) >>> 11) ^ a) >>> 2 */                     \
    "rorl	$2, " L2 "\n\t"                                       \
    /* L1 = d (e of next RND) */                                      \
    "movl	%" #d ", " L1 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) Sigma0(a) + Ch(e,f,g) + Maj(a,b,c) */  \
    "addl	" L2 ", %" #h "\n\t"                                  \

#define _RND_ALL_0(a,b,c,d,e,f,g,h,i)                                 \
    /* h += w_k */                                                    \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"                      \
    /* L2 = f */                                                      \
    "movl	%" #f ", " L2 "\n\t"                                  \
    /* L3 = b */                                                      \
    "movl	%" #b ", " L3 "\n\t"                                  \
    /* L2 = f ^ g */                                                  \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = e>>>14 */                                                 \
    "rorl	$14, " L1 "\n\t"                                      \
    /* L2 = (f ^ g) & e */                                            \
    "andl	%" #e ", " L2 "\n\t"                                  \
    /* L1 = (e>>>14) ^ e */                                           \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L2 = Ch(e,f,g) */                                              \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = ((e>>>14) ^ e) >>> 5 */                                   \
    "rorl	$5, " L1 "\n\t"                                       \
    /* h += Ch(e,f,g) */                                              \
    "addl	" L2 ", %" #h "\n\t"                                  \
    /* L1 = (((e>>>14) ^ e) >>> 5) ^ e */                             \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L3 = a ^ b */                                                  \
    "xorl	%" #a ", " L3 "\n\t"                                  \
    /* L1 = ((((e>>>14) ^ e) >>> 5) ^ e) >>> 6 */                     \
    "rorl	$6, " L1 "\n\t"                                       \
    /* L2 = a */                                                      \
    "movl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) */                                     \
    "addl	" L1 ", %" #h "\n\t"                                  \
    /* L2 = a>>>9 */                                                  \
    "rorl	$9, " L2 "\n\t"                                       \
    /* L3 = (a ^ b) & (b ^ c) */                                      \
    "andl	" L3 ", " L4 "\n\t"                                   \
    /* L2 = (a>>>9) ^ a */                                            \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* L1 = Maj(a,b,c) */                                             \
    "xorl	%" #b ", " L4 "\n\t"                                  \
    /* L2 = ((a>>>9) ^ a) >>> 11 */                                   \
    "rorl	$11, " L2 "\n\t"                                      \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */                        \
    "addl	%" #h ", %" #d "\n\t"                                 \
    /* L2 = (((a>>>9) ^ a) >>> 11) ^ a */                             \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) + Ch(e,f,g) + Maj(a,b,c) */            \
    "addl	" L4 ", %" #h "\n\t"                                  \
    /* L2 = ((((a>>>9) ^ a) >>> 11) ^ a) >>> 2 */                     \
    "rorl	$2, " L2 "\n\t"                                       \
    /* L1 = d (e of next RND) */                                      \
    "movl	%" #d ", " L1 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) Sigma0(a) + Ch(e,f,g) + Maj(a,b,c) */  \
    "addl	" L2 ", %" #h "\n\t"                                  \

#define _RND_ALL_1(a,b,c,d,e,f,g,h,i)                                 \
    /* h += w_k */                                                    \
    "addl	(" #i ")*4(" WK "), %" #h "\n\t"                      \
    /* L2 = f */                                                      \
    "movl	%" #f ", " L2 "\n\t"                                  \
    /* L3 = b */                                                      \
    "movl	%" #b ", " L4 "\n\t"                                  \
    /* L2 = f ^ g */                                                  \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = e>>>14 */                                                 \
    "rorl	$14, " L1 "\n\t"                                      \
    /* L2 = (f ^ g) & e */                                            \
    "andl	%" #e ", " L2 "\n\t"                                  \
    /* L1 = (e>>>14) ^ e */                                           \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L2 = Ch(e,f,g) */                                              \
    "xorl	%" #g ", " L2 "\n\t"                                  \
    /* L1 = ((e>>>14) ^ e) >>> 5 */                                   \
    "rorl	$5, " L1 "\n\t"                                       \
    /* h += Ch(e,f,g) */                                              \
    "addl	" L2 ", %" #h "\n\t"                                  \
    /* L1 = (((e>>>14) ^ e) >>> 5) ^ e */                             \
    "xorl	%" #e ", " L1 "\n\t"                                  \
    /* L3 = a ^ b */                                                  \
    "xorl	%" #a ", " L4 "\n\t"                                  \
    /* L1 = ((((e>>>14) ^ e) >>> 5) ^ e) >>> 6 */                     \
    "rorl	$6, " L1 "\n\t"                                       \
    /* L2 = a */                                                      \
    "movl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) */                                     \
    "addl	" L1 ", %" #h "\n\t"                                  \
    /* L2 = a>>>9 */                                                  \
    "rorl	$9, " L2 "\n\t"                                       \
    /* L3 = (a ^ b) & (b ^ c)  */                                     \
    "andl	" L4 ", " L3 "\n\t"                                   \
    /* L2 = (a>>>9) ^ a */                                            \
    "xorl	%" #a", " L2 "\n\t"                                   \
    /* L1 = Maj(a,b,c) */                                             \
    "xorl	%" #b ", " L3 "\n\t"                                  \
    /* L2 = ((a>>>9) ^ a) >>> 11 */                                   \
    "rorl	$11, " L2 "\n\t"                                      \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */                        \
    "addl	%" #h ", %" #d "\n\t"                                 \
    /* L2 = (((a>>>9) ^ a) >>> 11) ^ a */                             \
    "xorl	%" #a ", " L2 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) + Ch(e,f,g) + Maj(a,b,c) */            \
    "addl	" L3 ", %" #h "\n\t"                                  \
    /* L2 = ((((a>>>9) ^ a) >>> 11) ^ a) >>> 2 */                     \
    "rorl	$2, " L2 "\n\t"                                       \
    /* L1 = d (e of next RND) */                                      \
    "movl	%" #d ", " L1 "\n\t"                                  \
    /* h = h + w_k + Sigma1(e) Sigma0(a) + Ch(e,f,g) + Maj(a,b,c) */  \
    "addl	" L2 ", %" #h "\n\t"                                  \


#define RND_ALL_0(a, b, c, d, e, f, g, h, i) \
       _RND_ALL_0(a, b, c, d, e, f, g, h, i)
#define RND_ALL_1(a, b, c, d, e, f, g, h, i) \
       _RND_ALL_1(a, b, c, d, e, f, g, h, i)

#define RND_ALL_4(a, b, c, d, e, f, g, h, i)   \
        RND_ALL_0(a, b, c, d, e, f, g, h, i+0) \
        RND_ALL_1(h, a, b, c, d, e, f, g, i+1) \
        RND_ALL_0(g, h, a, b, c, d, e, f, i+2) \
        RND_ALL_1(f, g, h, a, b, c, d, e, i+3)

#endif  /* defined(HAVE_INTEL_AVX1) ||  defined(HAVE_INTEL_AVX2) */

#if defined(HAVE_INTEL_AVX1) /* inline Assember for Intel AVX1 instructions */

#define _VPALIGNR(op1, op2, op3, op4)                    \
    "vpalignr	$" #op4", %" #op3", %" #op2", %" #op1"\n\t"
#define VPALIGNR(op1, op2, op3, op4)                     \
        _VPALIGNR(op1, op2, op3, op4)
#define _VPADDD(op1, op2, op3)                           \
    "vpaddd	%" #op3", %" #op2", %" #op1"\n\t"
#define VPADDD(op1, op2, op3)                            \
       _VPADDD(op1, op2, op3)
#define _VPSRLD(op1, op2, op3)                           \
    "vpsrld	$" #op3", %" #op2", %" #op1"\n\t"
#define VPSRLD(op1, op2, op3)        \
       _VPSRLD(op1, op2, op3)
#define _VPSRLQ(op1, op2, op3)                           \
    "vpsrlq	$" #op3", %" #op2", %" #op1"\n\t"
#define VPSRLQ(op1,op2,op3)        \
       _VPSRLQ(op1,op2,op3)
#define _VPSLLD(op1,op2,op3)                             \
    "vpslld	$" #op3", %" #op2", %" #op1"\n\t"
#define VPSLLD(op1,op2,op3)        \
       _VPSLLD(op1,op2,op3)
#define _VPOR(op1,op2,op3)                               \
    "vpor	%" #op3", %" #op2", %" #op1"\n\t"
#define VPOR(op1,op2,op3)          \
       _VPOR(op1,op2,op3)
#define _VPXOR(op1,op2,op3)                              \
    "vpxor	%" #op3", %" #op2", %" #op1"\n\t"
#define VPXOR(op1,op2,op3)         \
       _VPXOR(op1,op2,op3)
#define _VPSHUFD(op1,op2,op3)                            \
    "vpshufd	$" #op3", %" #op2", %" #op1"\n\t"
#define VPSHUFD(op1,op2,op3)       \
       _VPSHUFD(op1,op2,op3)
#define _VPSHUFB(op1,op2,op3)                            \
    "vpshufb	%" #op3", %" #op2", %" #op1"\n\t"
#define VPSHUFB(op1,op2,op3)       \
       _VPSHUFB(op1,op2,op3)
#define _VPSLLDQ(op1,op2,op3)                            \
    "vpslldq	$" #op3", %" #op2", %" #op1"\n\t"
#define VPSLLDQ(op1,op2,op3)       \
       _VPSLLDQ(op1,op2,op3)

#define MsgSched(X0,X1,X2,X3,a,b,c,d,e,f,g,h,_i)                           \
            RND_STEP_0_1(a,b,c,d,e,f,g,h,_i)                               \
    VPALIGNR (XTMP1, X1, X0, 4)    /* XTMP1 = W[-15] */                    \
    VPALIGNR (XTMP0, X3, X2, 4)    /* XTMP0 = W[-7] */                     \
            RND_STEP_0_2(a,b,c,d,e,f,g,h,_i)                               \
            RND_STEP_0_3(a,b,c,d,e,f,g,h,_i)                               \
    VPSRLD   (XTMP2, XTMP1, 7)     /* XTMP2 = W[-15] >> 7 */               \
    VPSLLD   (XTMP3, XTMP1, 25)    /* XTEMP3 = W[-15] << (32-7) */         \
            RND_STEP_0_4(a,b,c,d,e,f,g,h,_i)                               \
            RND_STEP_0_5(a,b,c,d,e,f,g,h,_i)                               \
    VPSRLD   (XTMP4, XTMP1, 18)    /* XTEMP4 = W[-15] >> 18 */             \
    VPSLLD   (XTMP5, XTMP1, 14)    /* XTEMP5 = W[-15] << (32-18) */        \
            RND_STEP_0_6(a,b,c,d,e,f,g,h,_i)                               \
            RND_STEP_0_7(a,b,c,d,e,f,g,h,_i)                               \
    VPOR     (XTMP2, XTMP3, XTMP2) /* XTMP2 = W[-15] >>> 7 */              \
    VPOR     (XTMP4, XTMP5, XTMP4) /* XTMP4 = W[-15] >>> 18 */             \
            RND_STEP_0_8(a,b,c,d,e,f,g,h,_i)                               \
            RND_STEP_1_1(h,a,b,c,d,e,f,g,_i+1)                             \
            RND_STEP_1_2(h,a,b,c,d,e,f,g,_i+1)                             \
    VPSRLD   (XTMP5, XTMP1, 3)     /* XTMP4 = W[-15] >> 3 */               \
    VPXOR    (XTMP2, XTMP4, XTMP2)                                         \
                          /* XTMP2 = W[-15] MY_ROR 7 ^ W[-15] MY_ROR 18 */ \
            RND_STEP_1_3(h,a,b,c,d,e,f,g,_i+1)                             \
            RND_STEP_1_4(h,a,b,c,d,e,f,g,_i+1)                             \
    VPXOR    (XTMP1, XTMP5, XTMP2)  /* XTMP1 = s0 */                       \
    VPSHUFD  (XTMP2, X3, 0b11111010)  /* XTMP2 = W[-2] {BBAA}*/            \
            RND_STEP_1_5(h,a,b,c,d,e,f,g,_i+1)                             \
            RND_STEP_1_6(h,a,b,c,d,e,f,g,_i+1)                             \
    VPSRLD   (XTMP4, XTMP2, 10)      /* XTMP4 = W[-2] >> 10 {BBAA} */      \
    VPSRLQ   (XTMP3, XTMP2, 19)      /* XTMP3 = W[-2] MY_ROR 19 {xBxA} */  \
            RND_STEP_1_7(h,a,b,c,d,e,f,g,_i+1)                             \
            RND_STEP_1_8(h,a,b,c,d,e,f,g,_i+1)                             \
            RND_STEP_0_1(g,h,a,b,c,d,e,f,_i+2)                             \
    VPSRLQ   (XTMP2, XTMP2, 17)      /* XTMP2 = W[-2] MY_ROR 17 {xBxA} */  \
    VPADDD   (XTMP0, XTMP0, X0)                                            \
            RND_STEP_0_2(g,h,a,b,c,d,e,f,_i+2)                             \
            RND_STEP_0_3(g,h,a,b,c,d,e,f,_i+2)                             \
            RND_STEP_0_4(g,h,a,b,c,d,e,f,_i+2)                             \
    VPXOR    (XTMP2, XTMP3, XTMP2)                                         \
    VPADDD   (XTMP0, XTMP0, XTMP1)  /* XTMP0 = W[-16] + W[-7] + s0 */      \
            RND_STEP_0_5(g,h,a,b,c,d,e,f,_i+2)                             \
    VPXOR    (XTMP4, XTMP4, XTMP2)   /* XTMP4 = s1 {xBxA} */               \
            RND_STEP_0_6(g,h,a,b,c,d,e,f,_i+2)                             \
    VPSHUFB  (XTMP4, XTMP4, SHUF_00BA)  /* XTMP4 = s1 {00BA} */            \
            RND_STEP_0_7(g,h,a,b,c,d,e,f,_i+2)                             \
    VPADDD   (XTMP0, XTMP0, XTMP4)  /* XTMP0 = {..., ..., W[1], W[0]} */   \
            RND_STEP_0_8(g,h,a,b,c,d,e,f,_i+2)                             \
            RND_STEP_1_1(f,g,h,a,b,c,d,e,_i+3)                             \
    VPSHUFD  (XTMP2, XTMP0, 0b01010000) /* XTMP2 = W[-2] {DDCC} */         \
            RND_STEP_1_2(f,g,h,a,b,c,d,e,_i+3)                             \
    VPSRLQ   (XTMP4, XTMP2, 17)      /* XTMP4 = W[-2] MY_ROR 17 {xDxC} */  \
    VPSRLQ   (XTMP3, XTMP2, 19)       /* XTMP3 = W[-2] MY_ROR 19 {xDxC} */ \
            RND_STEP_1_3(f,g,h,a,b,c,d,e,_i+3)                             \
            RND_STEP_1_4(f,g,h,a,b,c,d,e,_i+3)                             \
    VPSRLD   (XTMP5, XTMP2, 10)       /* XTMP5 = W[-2] >> 10 {DDCC} */     \
    VPXOR    (XTMP4, XTMP3, XTMP4)                                         \
            RND_STEP_1_5(f,g,h,a,b,c,d,e,_i+3)                             \
            RND_STEP_1_6(f,g,h,a,b,c,d,e,_i+3)                             \
    VPXOR    (XTMP5, XTMP4, XTMP5)   /* XTMP5 = s1 {xDxC} */               \
            RND_STEP_1_7(f,g,h,a,b,c,d,e,_i+3)                             \
    VPSHUFB  (XTMP5, XTMP5, SHUF_DC00) /* XTMP5 = s1 {DC00} */             \
            RND_STEP_1_8(f,g,h,a,b,c,d,e,_i+3)                             \
    VPADDD   (X0, XTMP5, XTMP0)      /* X0 = {W[3], W[2], W[1], W[0]} */

#if defined(HAVE_INTEL_RORX)

#define MsgSched_RORX(X0,X1,X2,X3,a,b,c,d,e,f,g,h,_i)                      \
            RND_STEP_RORX_0_1(a,b,c,d,e,f,g,h,_i)                          \
    VPALIGNR (XTMP0, X3, X2, 4)                                            \
    VPALIGNR (XTMP1, X1, X0, 4)   /* XTMP1 = W[-15] */                     \
            RND_STEP_RORX_0_2(a,b,c,d,e,f,g,h,_i)                          \
            RND_STEP_RORX_0_3(a,b,c,d,e,f,g,h,_i)                          \
    VPSRLD   (XTMP2, XTMP1, 7)                                             \
    VPSLLD   (XTMP3, XTMP1, 25) /* VPSLLD   (XTMP3, XTMP1, (32-7)) */      \
            RND_STEP_RORX_0_4(a,b,c,d,e,f,g,h,_i)                          \
            RND_STEP_RORX_0_5(a,b,c,d,e,f,g,h,_i)                          \
    VPSRLD   (XTMP4, XTMP1, 3)  /* XTMP4 = W[-15] >> 3 */                  \
    VPOR     (XTMP3, XTMP3, XTMP2)  /* XTMP1 = W[-15] MY_ROR 7 */          \
            RND_STEP_RORX_0_6(a,b,c,d,e,f,g,h,_i)                          \
            RND_STEP_RORX_0_7(a,b,c,d,e,f,g,h,_i)                          \
            RND_STEP_RORX_0_8(a,b,c,d,e,f,g,h,_i)                          \
                                                                           \
            RND_STEP_RORX_1_1(h,a,b,c,d,e,f,g,_i+1)                        \
    VPSRLD   (XTMP2, XTMP1,18)                                             \
            RND_STEP_RORX_1_2(h,a,b,c,d,e,f,g,_i+1)                        \
    VPSLLD   (XTMP1, XTMP1, 14) /* VPSLLD   (XTMP1, XTMP1, (32-18)) */     \
            RND_STEP_RORX_1_3(h,a,b,c,d,e,f,g,_i+1)                        \
    VPXOR    (XTMP3, XTMP3, XTMP1)                                         \
            RND_STEP_RORX_1_4(h,a,b,c,d,e,f,g,_i+1)                        \
    VPXOR    (XTMP3, XTMP3, XTMP2)                                         \
                          /* XTMP1 = W[-15] MY_ROR 7 ^ W[-15] MY_ROR 18 */ \
            RND_STEP_RORX_1_5(h,a,b,c,d,e,f,g,_i+1)                        \
    VPSHUFD  (XTMP2, X3, 0b11111010)  /* XTMP2 = W[-2] {BBAA}*/            \
            RND_STEP_RORX_1_6(h,a,b,c,d,e,f,g,_i+1)                        \
    VPXOR    (XTMP1, XTMP3, XTMP4)  /* XTMP1 = s0 */                       \
            RND_STEP_RORX_1_7(h,a,b,c,d,e,f,g,_i+1)                        \
    VPSRLD   (XTMP4, XTMP2, 10)      /* XTMP4 = W[-2] >> 10 {BBAA} */      \
            RND_STEP_RORX_1_8(h,a,b,c,d,e,f,g,_i+1)                        \
                                                                           \
            RND_STEP_RORX_0_1(g,h,a,b,c,d,e,f,_i+2)                        \
    VPSRLQ   (XTMP3, XTMP2, 19)      /* XTMP3 = W[-2] MY_ROR 19 {xBxA} */  \
            RND_STEP_RORX_0_2(g,h,a,b,c,d,e,f,_i+2)                        \
    VPSRLQ   (XTMP2, XTMP2, 17)      /* XTMP2 = W[-2] MY_ROR 17 {xBxA} */  \
    VPADDD   (XTMP0, XTMP0, X0)                                            \
            RND_STEP_RORX_0_3(g,h,a,b,c,d,e,f,_i+2)                        \
    VPADDD   (XTMP0, XTMP0, XTMP1)  /* XTMP0 = W[-16] + W[-7] + s0 */      \
            RND_STEP_RORX_0_4(g,h,a,b,c,d,e,f,_i+2)                        \
    VPXOR    (XTMP2, XTMP2, XTMP3)                                         \
            RND_STEP_RORX_0_5(g,h,a,b,c,d,e,f,_i+2)                        \
    VPXOR    (XTMP4, XTMP4, XTMP2)   /* XTMP4 = s1 {xBxA} */               \
            RND_STEP_RORX_0_6(g,h,a,b,c,d,e,f,_i+2)                        \
    VPSHUFB  (XTMP4, XTMP4, SHUF_00BA)  /* XTMP4 = s1 {00BA} */            \
            RND_STEP_RORX_0_7(g,h,a,b,c,d,e,f,_i+2)                        \
    VPADDD   (XTMP0, XTMP0, XTMP4)  /* XTMP0 = {..., ..., W[1], W[0]} */   \
            RND_STEP_RORX_0_8(g,h,a,b,c,d,e,f,_i+2)                        \
                                                                           \
            RND_STEP_RORX_1_1(f,g,h,a,b,c,d,e,_i+3)                        \
    VPSHUFD  (XTMP2, XTMP0, 0b01010000) /* XTMP2 = W[-2] {DDCC} */         \
            RND_STEP_RORX_1_2(f,g,h,a,b,c,d,e,_i+3)                        \
    VPSRLD   (XTMP5, XTMP2, 10)       /* XTMP5 = W[-2] >> 10 {DDCC} */     \
            RND_STEP_RORX_1_3(f,g,h,a,b,c,d,e,_i+3)                        \
    VPSRLQ   (XTMP3, XTMP2, 19)       /* XTMP3 = W[-2] MY_ROR 19 {xDxC} */ \
            RND_STEP_RORX_1_4(f,g,h,a,b,c,d,e,_i+3)                        \
    VPSRLQ   (XTMP2, XTMP2, 17)      /* XTMP2 = W[-2] MY_ROR 17 {xDxC} */  \
            RND_STEP_RORX_1_5(f,g,h,a,b,c,d,e,_i+3)                        \
    VPXOR    (XTMP2, XTMP2, XTMP3)                                         \
            RND_STEP_RORX_1_6(f,g,h,a,b,c,d,e,_i+3)                        \
    VPXOR    (XTMP5, XTMP5, XTMP2)   /* XTMP5 = s1 {xDxC} */               \
            RND_STEP_RORX_1_7(f,g,h,a,b,c,d,e,_i+3)                        \
    VPSHUFB  (XTMP5, XTMP5, SHUF_DC00) /* XTMP5 = s1 {DC00} */             \
            RND_STEP_RORX_1_8(f,g,h,a,b,c,d,e,_i+3)                        \
    VPADDD   (X0, XTMP5, XTMP0)      /* X0 = {W[3], W[2], W[1], W[0]} */

#endif /* HAVE_INTEL_RORX */


#define _W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK) \
    "# X0, X1, X2, X3 = W[0..15]\n\t"                  \
    "vmovdqu	  (%%rax), %" #X0 "\n\t"               \
    "vmovdqu	16(%%rax), %" #X1 "\n\t"               \
    VPSHUFB(X0, X0, BYTE_FLIP_MASK)                    \
    VPSHUFB(X1, X1, BYTE_FLIP_MASK)                    \
    "vmovdqu	32(%%rax), %" #X2 "\n\t"               \
    "vmovdqu	48(%%rax), %" #X3 "\n\t"               \
    VPSHUFB(X2, X2, BYTE_FLIP_MASK)                    \
    VPSHUFB(X3, X3, BYTE_FLIP_MASK)

#define W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK) \
       _W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK)


#define _SET_W_K_XFER_4(i) \
    "vpaddd	(" #i "*4)+ 0+%[K], %%xmm0, %%xmm4\n\t"  \
    "vpaddd	(" #i "*4)+16+%[K], %%xmm1, %%xmm5\n\t"  \
    "vmovdqu	%%xmm4,   (" WK ")\n\t"                  \
    "vmovdqu	%%xmm5, 16(" WK ")\n\t"                  \
    "vpaddd	(" #i "*4)+32+%[K], %%xmm2, %%xmm6\n\t"  \
    "vpaddd	(" #i "*4)+48+%[K], %%xmm3, %%xmm7\n\t"  \
    "vmovdqu	%%xmm6, 32(" WK ")\n\t"                  \
    "vmovdqu	%%xmm7, 48(" WK ")\n\t"

#define SET_W_K_XFER_4(i) \
       _SET_W_K_XFER_4(i)


static const ALIGN32 word64 mSHUF_00BA[] =
    { 0x0b0a090803020100, 0xFFFFFFFFFFFFFFFF }; /* shuffle xBxA -> 00BA */
static const ALIGN32 word64 mSHUF_DC00[] =
    { 0xFFFFFFFFFFFFFFFF, 0x0b0a090803020100 }; /* shuffle xDxC -> DC00 */
static const ALIGN32 word64 mBYTE_FLIP_MASK[] =
    { 0x0405060700010203, 0x0c0d0e0f08090a0b };

#define _Init_Masks(mask1, mask2, mask3)       \
    "vmovdqa	%[FLIP], %" #mask1 "\n\t"      \
    "vmovdqa	%[SHUF00BA], %" #mask2 "\n\t"  \
    "vmovdqa	%[SHUFDC00], %" #mask3 "\n\t"

#define Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00) \
       _Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)

#define X0 %xmm0
#define X1 %xmm1
#define X2 %xmm2
#define X3 %xmm3

#define XTMP0 %xmm4
#define XTMP1 %xmm5
#define XTMP2 %xmm6
#define XTMP3 %xmm7
#define XTMP4 %xmm8
#define XTMP5 %xmm9
#define XFER  %xmm10

#define SHUF_00BA   %xmm11 /* shuffle xBxA -> 00BA */
#define SHUF_DC00   %xmm12 /* shuffle xDxC -> DC00 */
#define BYTE_FLIP_MASK  %xmm13


SHA256_NOINLINE static int Transform_Sha256_AVX1(wc_Sha256* sha256)
{
    __asm__ __volatile__ (

        "subq	$64, %%rsp\n\t"

        "leaq	32(%[sha256]), %%rax\n\t"
    Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)
    LOAD_DIGEST()

    W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK)

        "movl	%%r9d, " L4 "\n\t"
        "movl	%%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_K_XFER_4(0)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(16)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(32)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(48)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    STORE_ADD_DIGEST()

        "addq	$64, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_00BA[0]),
          [SHUFDC00] "m" (mSHUF_DC00[0]),
          [sha256]   "r" (sha256),
          [K]        "m" (K)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory"
    );

    return 0;
}

SHA256_NOINLINE static int Transform_Sha256_AVX1_Len(wc_Sha256* sha256,
                                                     word32 len)
{
    __asm__ __volatile__ (

        "subq	$64, %%rsp\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)
    LOAD_DIGEST()

        "# Start of loop processing a block\n"
        "1:\n\t"

    W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK)

        "movl	%%r9d, " L4 "\n\t"
        "movl	%%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_K_XFER_4(0)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(16)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(32)
    MsgSched(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(48)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)
        "movq	120(%[sha256]), %%rax\n\t"

    ADD_DIGEST()

        "addq	$64, %%rax\n\t"
        "subl	$64, %[len]\n\t"

    STORE_DIGEST()

        "movq	%%rax, 120(%[sha256])\n\t"
        "jnz	1b\n\t"

        "addq	$64, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_00BA[0]),
          [SHUFDC00] "m" (mSHUF_DC00[0]),
          [sha256]   "r" (sha256),
          [len]      "r" (len),
          [K]        "m" (K)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory"
    );

    return 0;
}
#endif  /* HAVE_INTEL_AVX1 */

#if defined(HAVE_INTEL_AVX2) && defined(HAVE_INTEL_RORX)
SHA256_NOINLINE static int Transform_Sha256_AVX1_RORX(wc_Sha256* sha256)
{
    __asm__ __volatile__ (

        "subq	$64, %%rsp\n\t"

    Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)
        "leaq	32(%[sha256]), %%rax\n\t"
    W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK)

    LOAD_DIGEST()

    SET_W_K_XFER_4(0)
        "movl	%%r9d, " L4 "\n\t"
        "rorx	$6, %%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(16)
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(32)
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(48)
        "xorl	" L3 ", " L3 "\n\t"
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)
        /* Prev RND: h += Maj(a,b,c) */
        "addl	" L3 ", %%r8d\n\t"

    STORE_ADD_DIGEST()

        "addq	$64, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_00BA[0]),
          [SHUFDC00] "m" (mSHUF_DC00[0]),
          [sha256]   "r" (sha256),
          [K]        "m" (K)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory"
    );

    return 0;
}

SHA256_NOINLINE static int Transform_Sha256_AVX1_RORX_Len(wc_Sha256* sha256,
                                                          word32 len)
{
    __asm__ __volatile__ (

        "subq	$64, %%rsp\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    Init_Masks(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)
    LOAD_DIGEST()

        "# Start of loop processing a block\n"
        "1:\n\t"

    W_K_from_buff(X0, X1, X2, X3, BYTE_FLIP_MASK)

    SET_W_K_XFER_4(0)
        "movl	%%r9d, " L4 "\n\t"
        "rorx	$6, %%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(16)
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(32)
    MsgSched_RORX(X0, X1, X2, X3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_RORX(X1, X2, X3, X0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    MsgSched_RORX(X2, X3, X0, X1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    MsgSched_RORX(X3, X0, X1, X2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)

    SET_W_K_XFER_4(48)
        "xorl	" L3 ", " L3 "\n\t"
        "xorl	" L2 ", " L2 "\n\t"
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  4)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  8)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 12)
        /* Prev RND: h += Maj(a,b,c) */
        "addl	" L3 ", %%r8d\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    ADD_DIGEST()

        "addq	$64, %%rax\n\t"
        "subl	$64, %[len]\n\t"

    STORE_DIGEST()

        "movq	%%rax, 120(%[sha256])\n\t"
        "jnz	1b\n\t"

        "addq	$64, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_00BA[0]),
          [SHUFDC00] "m" (mSHUF_DC00[0]),
          [sha256]   "r" (sha256),
          [len]      "r" (len),
          [K]        "m" (K)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory"
    );

    return 0;
}
#endif /* HAVE_INTEL_AVX2 && HAVE_INTEL_RORX */


#if defined(HAVE_INTEL_AVX2)
#define Y0 %ymm0
#define Y1 %ymm1
#define Y2 %ymm2
#define Y3 %ymm3

#define YTMP0 %ymm4
#define YTMP1 %ymm5
#define YTMP2 %ymm6
#define YTMP3 %ymm7
#define YTMP4 %ymm8
#define YTMP5 %ymm9
#define YXFER %ymm10

#define SHUF_Y_00BA       %ymm11 /* shuffle xBxA -> 00BA */
#define SHUF_Y_DC00       %ymm12 /* shuffle xDxC -> DC00 */
#define BYTE_FLIP_Y_MASK  %ymm13

#define YMM_REGS "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", \
                 "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13"

#define MsgSched_Y(Y0,Y1,Y2,Y3,a,b,c,d,e,f,g,h,_i)                            \
            RND_STEP_0_1(a,b,c,d,e,f,g,h,_i)                                  \
    VPALIGNR (YTMP1, Y1, Y0, 4)    /* YTMP1 = W[-15] */                       \
    VPALIGNR (YTMP0, Y3, Y2, 4)    /* YTMP0 = W[-7] */                        \
            RND_STEP_0_2(a,b,c,d,e,f,g,h,_i)                                  \
            RND_STEP_0_3(a,b,c,d,e,f,g,h,_i)                                  \
    VPSRLD   (YTMP2, YTMP1, 7)     /* YTMP2 = W[-15] >> 7 */                  \
    VPSLLD   (YTMP3, YTMP1, 25)    /* YTEMP3 = W[-15] << (32-7) */            \
            RND_STEP_0_4(a,b,c,d,e,f,g,h,_i)                                  \
            RND_STEP_0_5(a,b,c,d,e,f,g,h,_i)                                  \
    VPSRLD   (YTMP4, YTMP1, 18)    /* YTEMP4 = W[-15] >> 18 */                \
    VPSLLD   (YTMP5, YTMP1, 14)    /* YTEMP5 = W[-15] << (32-18) */           \
            RND_STEP_0_6(a,b,c,d,e,f,g,h,_i)                                  \
            RND_STEP_0_7(a,b,c,d,e,f,g,h,_i)                                  \
    VPOR     (YTMP2, YTMP3, YTMP2) /* YTMP2 = W[-15] >>> 7 */                 \
    VPOR     (YTMP4, YTMP5, YTMP4) /* YTMP4 = W[-15] >>> 18 */                \
            RND_STEP_0_8(a,b,c,d,e,f,g,h,_i)                                  \
            RND_STEP_1_1(h,a,b,c,d,e,f,g,_i+1)                                \
            RND_STEP_1_2(h,a,b,c,d,e,f,g,_i+1)                                \
    VPSRLD   (YTMP5, YTMP1, 3)     /* YTMP4 = W[-15] >> 3 */                  \
    VPXOR    (YTMP2, YTMP4, YTMP2) /* YTMP2 = W[-15] >>> 7 ^ W[-15] >>> 18 */ \
            RND_STEP_1_3(h,a,b,c,d,e,f,g,_i+1)                                \
            RND_STEP_1_4(h,a,b,c,d,e,f,g,_i+1)                                \
    VPXOR    (YTMP1, YTMP5, YTMP2)  /* YTMP1 = s0 */                          \
    VPSHUFD  (YTMP2, Y3, 0b11111010)  /* YTMP2 = W[-2] {BBAA}*/               \
            RND_STEP_1_5(h,a,b,c,d,e,f,g,_i+1)                                \
            RND_STEP_1_6(h,a,b,c,d,e,f,g,_i+1)                                \
    VPSRLD   (YTMP4, YTMP2, 10)      /* YTMP4 = W[-2] >> 10 {BBAA} */         \
    VPSRLQ   (YTMP3, YTMP2, 19)      /* YTMP3 = W[-2] MY_ROR 19 {xBxA} */     \
            RND_STEP_1_7(h,a,b,c,d,e,f,g,_i+1)                                \
            RND_STEP_1_8(h,a,b,c,d,e,f,g,_i+1)                                \
            RND_STEP_0_1(g,h,a,b,c,d,e,f,_i+2)                                \
    VPSRLQ   (YTMP2, YTMP2, 17)      /* YTMP2 = W[-2] MY_ROR 17 {xBxA} */     \
    VPADDD   (YTMP0, YTMP0, Y0)                                               \
            RND_STEP_0_2(g,h,a,b,c,d,e,f,_i+2)                                \
            RND_STEP_0_3(g,h,a,b,c,d,e,f,_i+2)                                \
            RND_STEP_0_4(g,h,a,b,c,d,e,f,_i+2)                                \
    VPXOR    (YTMP2, YTMP3, YTMP2)                                            \
    VPADDD   (YTMP0, YTMP0, YTMP1)  /* YTMP0 = W[-16] + W[-7] + s0 */         \
            RND_STEP_0_5(g,h,a,b,c,d,e,f,_i+2)                                \
    VPXOR    (YTMP4, YTMP4, YTMP2)   /* YTMP4 = s1 {xBxA} */                  \
            RND_STEP_0_6(g,h,a,b,c,d,e,f,_i+2)                                \
    VPSHUFB  (YTMP4, YTMP4, SHUF_Y_00BA)  /* YTMP4 = s1 {00BA} */             \
            RND_STEP_0_7(g,h,a,b,c,d,e,f,_i+2)                                \
    VPADDD   (YTMP0, YTMP0, YTMP4)  /* YTMP0 = {..., ..., W[1], W[0]} */      \
            RND_STEP_0_8(g,h,a,b,c,d,e,f,_i+2)                                \
            RND_STEP_1_1(f,g,h,a,b,c,d,e,_i+3)                                \
    VPSHUFD  (YTMP2, YTMP0, 0b01010000) /* YTMP2 = W[-2] {DDCC} */            \
            RND_STEP_1_2(f,g,h,a,b,c,d,e,_i+3)                                \
    VPSRLQ   (YTMP4, YTMP2, 17)      /* YTMP4 = W[-2] MY_ROR 17 {xDxC} */     \
    VPSRLQ   (YTMP3, YTMP2, 19)       /* YTMP3 = W[-2] MY_ROR 19 {xDxC} */    \
            RND_STEP_1_3(f,g,h,a,b,c,d,e,_i+3)                                \
            RND_STEP_1_4(f,g,h,a,b,c,d,e,_i+3)                                \
    VPSRLD   (YTMP5, YTMP2, 10)       /* YTMP5 = W[-2] >> 10 {DDCC} */        \
    VPXOR    (YTMP4, YTMP3, YTMP4)                                            \
            RND_STEP_1_5(f,g,h,a,b,c,d,e,_i+3)                                \
            RND_STEP_1_6(f,g,h,a,b,c,d,e,_i+3)                                \
    VPXOR    (YTMP5, YTMP4, YTMP5)   /* YTMP5 = s1 {xDxC} */                  \
            RND_STEP_1_7(f,g,h,a,b,c,d,e,_i+3)                                \
    VPSHUFB  (YTMP5, YTMP5, SHUF_Y_DC00) /* YTMP5 = s1 {DC00} */              \
            RND_STEP_1_8(f,g,h,a,b,c,d,e,_i+3)                                \
    VPADDD   (Y0, YTMP5, YTMP0)      /* Y0 = {W[3], W[2], W[1], W[0]} */

#if defined(HAVE_INTEL_RORX)

#define MsgSched_Y_RORX(Y0,Y1,Y2,Y3,a,b,c,d,e,f,g,h,_i)                       \
            RND_STEP_RORX_0_1(a,b,c,d,e,f,g,h,_i)                             \
    VPALIGNR (YTMP1, Y1, Y0, 4)    /* YTMP1 = W[-15] */                       \
            RND_STEP_RORX_0_2(a,b,c,d,e,f,g,h,_i)                             \
    VPALIGNR (YTMP0, Y3, Y2, 4)    /* YTMP0 = W[-7] */                        \
            RND_STEP_RORX_0_3(a,b,c,d,e,f,g,h,_i)                             \
    VPSRLD   (YTMP2, YTMP1, 7)     /* YTMP2 = W[-15] >> 7 */                  \
            RND_STEP_RORX_0_4(a,b,c,d,e,f,g,h,_i)                             \
    VPSLLD   (YTMP3, YTMP1, 25)    /* YTEMP3 = W[-15] << (32-7) */            \
            RND_STEP_RORX_0_5(a,b,c,d,e,f,g,h,_i)                             \
    VPSRLD   (YTMP4, YTMP1, 18)    /* YTEMP4 = W[-15] >> 18 */                \
            RND_STEP_RORX_0_6(a,b,c,d,e,f,g,h,_i)                             \
    VPSLLD   (YTMP5, YTMP1, 14)    /* YTEMP5 = W[-15] << (32-18) */           \
            RND_STEP_RORX_0_7(a,b,c,d,e,f,g,h,_i)                             \
    VPOR     (YTMP2, YTMP2, YTMP3) /* YTMP2 = W[-15] >>> 7 */                 \
            RND_STEP_RORX_0_8(a,b,c,d,e,f,g,h,_i)                             \
    VPOR     (YTMP4, YTMP4, YTMP5) /* YTMP4 = W[-15] >>> 18 */                \
            RND_STEP_RORX_1_1(h,a,b,c,d,e,f,g,_i+1)                           \
    VPSRLD   (YTMP5, YTMP1, 3)     /* YTMP4 = W[-15] >> 3 */                  \
            RND_STEP_RORX_1_2(h,a,b,c,d,e,f,g,_i+1)                           \
    VPXOR    (YTMP2, YTMP2, YTMP4) /* YTMP2 = W[-15] >>> 7 ^ W[-15] >>> 18 */ \
            RND_STEP_RORX_1_3(h,a,b,c,d,e,f,g,_i+1)                           \
    VPSHUFD  (YTMP3, Y3, 0b11111010)  /* YTMP2 = W[-2] {BBAA}*/               \
            RND_STEP_RORX_1_4(h,a,b,c,d,e,f,g,_i+1)                           \
    VPXOR    (YTMP1, YTMP5, YTMP2)  /* YTMP1 = s0 */                          \
            RND_STEP_RORX_1_5(h,a,b,c,d,e,f,g,_i+1)                           \
    VPSRLD   (YTMP4, YTMP3, 10)      /* YTMP4 = W[-2] >> 10 {BBAA} */         \
            RND_STEP_RORX_1_6(h,a,b,c,d,e,f,g,_i+1)                           \
    VPSRLQ   (YTMP2, YTMP3, 19)      /* YTMP3 = W[-2] MY_ROR 19 {xBxA} */     \
            RND_STEP_RORX_1_7(h,a,b,c,d,e,f,g,_i+1)                           \
    VPSRLQ   (YTMP3, YTMP3, 17)      /* YTMP2 = W[-2] MY_ROR 17 {xBxA} */     \
            RND_STEP_RORX_1_8(h,a,b,c,d,e,f,g,_i+1)                           \
    VPADDD   (YTMP0, YTMP0, Y0)                                               \
            RND_STEP_RORX_0_1(g,h,a,b,c,d,e,f,_i+2)                           \
    VPXOR    (YTMP2, YTMP2, YTMP3)                                            \
            RND_STEP_RORX_0_2(g,h,a,b,c,d,e,f,_i+2)                           \
    VPXOR    (YTMP4, YTMP4, YTMP2)   /* YTMP4 = s1 {xBxA} */                  \
            RND_STEP_RORX_0_3(g,h,a,b,c,d,e,f,_i+2)                           \
    VPADDD   (YTMP0, YTMP0, YTMP1)  /* YTMP0 = W[-16] + W[-7] + s0 */         \
            RND_STEP_RORX_0_4(g,h,a,b,c,d,e,f,_i+2)                           \
    VPSHUFB  (YTMP4, YTMP4, SHUF_Y_00BA)  /* YTMP4 = s1 {00BA} */             \
            RND_STEP_RORX_0_5(g,h,a,b,c,d,e,f,_i+2)                           \
    VPADDD   (YTMP0, YTMP0, YTMP4)  /* YTMP0 = {..., ..., W[1], W[0]} */      \
            RND_STEP_RORX_0_6(g,h,a,b,c,d,e,f,_i+2)                           \
    VPSHUFD  (YTMP2, YTMP0, 0b01010000) /* YTMP2 = W[-2] {DDCC} */            \
            RND_STEP_RORX_0_7(g,h,a,b,c,d,e,f,_i+2)                           \
            RND_STEP_RORX_0_8(g,h,a,b,c,d,e,f,_i+2)                           \
    VPSRLQ   (YTMP4, YTMP2, 17)      /* YTMP4 = W[-2] MY_ROR 17 {xDxC} */     \
            RND_STEP_RORX_1_1(f,g,h,a,b,c,d,e,_i+3)                           \
    VPSRLQ   (YTMP3, YTMP2, 19)       /* YTMP3 = W[-2] MY_ROR 19 {xDxC} */    \
            RND_STEP_RORX_1_2(f,g,h,a,b,c,d,e,_i+3)                           \
    VPSRLD   (YTMP5, YTMP2, 10)       /* YTMP5 = W[-2] >> 10 {DDCC} */        \
            RND_STEP_RORX_1_3(f,g,h,a,b,c,d,e,_i+3)                           \
    VPXOR    (YTMP4, YTMP4, YTMP3)                                            \
            RND_STEP_RORX_1_4(f,g,h,a,b,c,d,e,_i+3)                           \
    VPXOR    (YTMP5, YTMP5, YTMP4)   /* YTMP5 = s1 {xDxC} */                  \
            RND_STEP_RORX_1_5(f,g,h,a,b,c,d,e,_i+3)                           \
            RND_STEP_RORX_1_6(f,g,h,a,b,c,d,e,_i+3)                           \
    VPSHUFB  (YTMP5, YTMP5, SHUF_Y_DC00) /* YTMP5 = s1 {DC00} */              \
            RND_STEP_RORX_1_7(f,g,h,a,b,c,d,e,_i+3)                           \
            RND_STEP_RORX_1_8(f,g,h,a,b,c,d,e,_i+3)                           \
    VPADDD   (Y0, YTMP5, YTMP0)      /* Y0 = {W[3], W[2], W[1], W[0]} */      \

#endif /* HAVE_INTEL_RORX */

#define _VINSERTI128(op1,op2,op3,op4) \
    "vinserti128	$" #op4 ", %" #op3 ", %" #op2 ", %" #op1 "\n\t"
#define VINSERTI128(op1,op2,op3,op4)  \
       _VINSERTI128(op1,op2,op3,op4)


#define _LOAD_W_K_LOW(BYTE_FLIP_MASK, reg)   \
    "# X0, X1, X2, X3 = W[0..15]\n\t"        \
    "vmovdqu	  (%%" #reg "), %%xmm0\n\t"  \
    "vmovdqu	16(%%" #reg "), %%xmm1\n\t"  \
    VPSHUFB(X0, X0, BYTE_FLIP_MASK)          \
    VPSHUFB(X1, X1, BYTE_FLIP_MASK)          \
    "vmovdqu	32(%%" #reg "), %%xmm2\n\t"  \
    "vmovdqu	48(%%" #reg "), %%xmm3\n\t"  \
    VPSHUFB(X2, X2, BYTE_FLIP_MASK)          \
    VPSHUFB(X3, X3, BYTE_FLIP_MASK)

#define LOAD_W_K_LOW(BYTE_FLIP_MASK, reg) \
       _LOAD_W_K_LOW(BYTE_FLIP_MASK, reg)


#define _LOAD_W_K(BYTE_FLIP_Y_MASK, reg)      \
    "# X0, X1, X2, X3 = W[0..15]\n\t"         \
    "vmovdqu	   (%%" #reg "), %%xmm0\n\t"  \
    "vmovdqu	 16(%%" #reg "), %%xmm1\n\t"  \
    "vmovdqu	 64(%%" #reg "), %%xmm4\n\t"  \
    "vmovdqu	 80(%%" #reg "), %%xmm5\n\t"  \
    VINSERTI128(Y0, Y0, XTMP0, 1)             \
    VINSERTI128(Y1, Y1, XTMP1, 1)             \
    VPSHUFB(Y0, Y0, BYTE_FLIP_Y_MASK)         \
    VPSHUFB(Y1, Y1, BYTE_FLIP_Y_MASK)         \
    "vmovdqu	 32(%%" #reg "), %%xmm2\n\t"  \
    "vmovdqu	 48(%%" #reg "), %%xmm3\n\t"  \
    "vmovdqu	 96(%%" #reg "), %%xmm6\n\t"  \
    "vmovdqu	112(%%" #reg "), %%xmm7\n\t"  \
    VINSERTI128(Y2, Y2, XTMP2, 1)             \
    VINSERTI128(Y3, Y3, XTMP3, 1)             \
    VPSHUFB(Y2, Y2, BYTE_FLIP_Y_MASK)         \
    VPSHUFB(Y3, Y3, BYTE_FLIP_Y_MASK)

#define LOAD_W_K(BYTE_FLIP_Y_MASK, reg) \
       _LOAD_W_K(BYTE_FLIP_Y_MASK, reg)


#define _SET_W_Y_4(i)  \
    "vpaddd	(" #i "*8)+ 0+%[K], %%ymm0, %%ymm4\n\t" \
    "vpaddd	(" #i "*8)+32+%[K], %%ymm1, %%ymm5\n\t" \
    "vmovdqu	%%ymm4, (" #i "*8)+ 0(" WK ")\n\t"      \
    "vmovdqu	%%ymm5, (" #i "*8)+32(" WK ")\n\t"      \
    "vpaddd	(" #i "*8)+64+%[K], %%ymm2, %%ymm4\n\t" \
    "vpaddd	(" #i "*8)+96+%[K], %%ymm3, %%ymm5\n\t" \
    "vmovdqu	%%ymm4, (" #i "*8)+64(" WK ")\n\t"      \
    "vmovdqu	%%ymm5, (" #i "*8)+96(" WK ")\n\t"

#define SET_W_Y_4(i) \
       _SET_W_Y_4(i)


static const ALIGN32 word64 mSHUF_Y_00BA[] =
    { 0x0b0a090803020100, 0xFFFFFFFFFFFFFFFF,
      0x0b0a090803020100, 0xFFFFFFFFFFFFFFFF }; /* shuffle xBxA -> 00BA */
static const ALIGN32 word64 mSHUF_Y_DC00[] =
    { 0xFFFFFFFFFFFFFFFF, 0x0b0a090803020100,
      0xFFFFFFFFFFFFFFFF, 0x0b0a090803020100 }; /* shuffle xDxC -> DC00 */
static const ALIGN32 word64 mBYTE_FLIP_Y_MASK[] =
    { 0x0405060700010203, 0x0c0d0e0f08090a0b,
      0x0405060700010203, 0x0c0d0e0f08090a0b };

#define _INIT_MASKS_Y(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00) \
    "vmovdqa	%[FLIP], %" #BYTE_FLIP_MASK "\n\t"          \
    "vmovdqa	%[SHUF00BA], %" #SHUF_00BA "\n\t"           \
    "vmovdqa	%[SHUFDC00], %" #SHUF_DC00 "\n\t"

#define INIT_MASKS_Y(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00) \
       _INIT_MASKS_Y(BYTE_FLIP_MASK, SHUF_00BA, SHUF_DC00)

static const ALIGN32 word32 K256[128] = {
    0x428A2F98L, 0x71374491L, 0xB5C0FBCFL, 0xE9B5DBA5L,
    0x428A2F98L, 0x71374491L, 0xB5C0FBCFL, 0xE9B5DBA5L,
    0x3956C25BL, 0x59F111F1L, 0x923F82A4L, 0xAB1C5ED5L,
    0x3956C25BL, 0x59F111F1L, 0x923F82A4L, 0xAB1C5ED5L,
    0xD807AA98L, 0x12835B01L, 0x243185BEL, 0x550C7DC3L,
    0xD807AA98L, 0x12835B01L, 0x243185BEL, 0x550C7DC3L,
    0x72BE5D74L, 0x80DEB1FEL, 0x9BDC06A7L, 0xC19BF174L,
    0x72BE5D74L, 0x80DEB1FEL, 0x9BDC06A7L, 0xC19BF174L,
    0xE49B69C1L, 0xEFBE4786L, 0x0FC19DC6L, 0x240CA1CCL,
    0xE49B69C1L, 0xEFBE4786L, 0x0FC19DC6L, 0x240CA1CCL,
    0x2DE92C6FL, 0x4A7484AAL, 0x5CB0A9DCL, 0x76F988DAL,
    0x2DE92C6FL, 0x4A7484AAL, 0x5CB0A9DCL, 0x76F988DAL,
    0x983E5152L, 0xA831C66DL, 0xB00327C8L, 0xBF597FC7L,
    0x983E5152L, 0xA831C66DL, 0xB00327C8L, 0xBF597FC7L,
    0xC6E00BF3L, 0xD5A79147L, 0x06CA6351L, 0x14292967L,
    0xC6E00BF3L, 0xD5A79147L, 0x06CA6351L, 0x14292967L,
    0x27B70A85L, 0x2E1B2138L, 0x4D2C6DFCL, 0x53380D13L,
    0x27B70A85L, 0x2E1B2138L, 0x4D2C6DFCL, 0x53380D13L,
    0x650A7354L, 0x766A0ABBL, 0x81C2C92EL, 0x92722C85L,
    0x650A7354L, 0x766A0ABBL, 0x81C2C92EL, 0x92722C85L,
    0xA2BFE8A1L, 0xA81A664BL, 0xC24B8B70L, 0xC76C51A3L,
    0xA2BFE8A1L, 0xA81A664BL, 0xC24B8B70L, 0xC76C51A3L,
    0xD192E819L, 0xD6990624L, 0xF40E3585L, 0x106AA070L,
    0xD192E819L, 0xD6990624L, 0xF40E3585L, 0x106AA070L,
    0x19A4C116L, 0x1E376C08L, 0x2748774CL, 0x34B0BCB5L,
    0x19A4C116L, 0x1E376C08L, 0x2748774CL, 0x34B0BCB5L,
    0x391C0CB3L, 0x4ED8AA4AL, 0x5B9CCA4FL, 0x682E6FF3L,
    0x391C0CB3L, 0x4ED8AA4AL, 0x5B9CCA4FL, 0x682E6FF3L,
    0x748F82EEL, 0x78A5636FL, 0x84C87814L, 0x8CC70208L,
    0x748F82EEL, 0x78A5636FL, 0x84C87814L, 0x8CC70208L,
    0x90BEFFFAL, 0xA4506CEBL, 0xBEF9A3F7L, 0xC67178F2L,
    0x90BEFFFAL, 0xA4506CEBL, 0xBEF9A3F7L, 0xC67178F2L
};

SHA256_NOINLINE static int Transform_Sha256_AVX2(wc_Sha256* sha256)
{
    __asm__ __volatile__ (

        "subq	$512, %%rsp\n\t"
        "leaq	32(%[sha256]), %%rax\n\t"

    INIT_MASKS_Y(BYTE_FLIP_MASK, SHUF_Y_00BA, SHUF_Y_DC00)
    LOAD_DIGEST()

    LOAD_W_K_LOW(BYTE_FLIP_MASK, rax)

        "movl	%%r9d, " L4 "\n\t"
        "movl	%%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_Y_4(0)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  8)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 16)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 24)

    SET_W_Y_4(16)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 32)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 40)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 48)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 56)

    SET_W_Y_4(32)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 64)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 72)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 80)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 88)

    SET_W_Y_4(48)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  96)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 104)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 112)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 120)

    STORE_ADD_DIGEST()

        "addq	$512, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_Y_00BA[0]),
          [SHUFDC00] "m" (mSHUF_Y_DC00[0]),
          [sha256]   "r" (sha256),
          [K]        "m" (K256)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory"
    );

    return 0;
}

SHA256_NOINLINE static int Transform_Sha256_AVX2_Len(wc_Sha256* sha256,
                                                     word32 len)
{
    if ((len & WC_SHA256_BLOCK_SIZE) != 0) {
        XMEMCPY(sha256->buffer, sha256->data, WC_SHA256_BLOCK_SIZE);
        Transform_Sha256_AVX2(sha256);
        sha256->data += WC_SHA256_BLOCK_SIZE;
        len -= WC_SHA256_BLOCK_SIZE;
        if (len == 0)
            return 0;
    }

    __asm__ __volatile__ (

        "subq	$512, %%rsp\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    INIT_MASKS_Y(BYTE_FLIP_Y_MASK, SHUF_Y_00BA, SHUF_Y_DC00)
    LOAD_DIGEST()

        "# Start of loop processing two blocks\n"
        "1:\n\t"

    LOAD_W_K(BYTE_FLIP_Y_MASK, rax)

        "movl	%%r9d, " L4 "\n\t"
        "movl	%%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_Y_4(0)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  8)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 16)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 24)

    SET_W_Y_4(16)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 32)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 40)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 48)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 56)

    SET_W_Y_4(32)
    MsgSched_Y(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 64)
    MsgSched_Y(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 72)
    MsgSched_Y(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 80)
    MsgSched_Y(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 88)

    SET_W_Y_4(48)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  96)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 104)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 112)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 120)

    ADD_DIGEST()
    STORE_DIGEST()

        "movl	%%r9d, " L4 "\n\t"
        "movl	%%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,   4)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  12)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  20)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  28)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  36)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  44)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  52)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  60)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  68)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  76)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  84)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  92)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 100)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 108)
    RND_ALL_4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 116)
    RND_ALL_4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 124)

    ADD_DIGEST()

        "movq	120(%[sha256]), %%rax\n\t"
        "addq	$128, %%rax\n\t"
        "subl	$128, %[len]\n\t"

    STORE_DIGEST()

        "movq	%%rax, 120(%[sha256])\n\t"
        "jnz	1b\n\t"

        "addq	$512, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_Y_MASK[0]),
          [SHUF00BA] "m" (mSHUF_Y_00BA[0]),
          [SHUFDC00] "m" (mSHUF_Y_DC00[0]),
          [sha256]   "r" (sha256),
          [len]      "r" (len),
          [K]        "m" (K256)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory"
    );

    return 0;
}

#if defined(HAVE_INTEL_RORX)
SHA256_NOINLINE static int Transform_Sha256_AVX2_RORX(wc_Sha256* sha256)
{
    __asm__ __volatile__ (

        "subq	$512, %%rsp\n\t"
        "leaq	32(%[sha256]), %%rax\n\t"

    INIT_MASKS_Y(BYTE_FLIP_MASK, SHUF_Y_00BA, SHUF_Y_DC00)
    LOAD_W_K_LOW(BYTE_FLIP_MASK, rax)

    LOAD_DIGEST()

        "movl	%%r9d, " L4 "\n\t"
        "rorx	$6, %%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_Y_4(0)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  8)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 16)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 24)

    SET_W_Y_4(16)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 32)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 40)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 48)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 56)

    SET_W_Y_4(32)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 64)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 72)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 80)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 88)

    SET_W_Y_4(48)
        "xorl	" L3 ", " L3 "\n\t"
        "xorl	" L2 ", " L2 "\n\t"
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  96)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 104)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 112)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 120)
        /* Prev RND: h += Maj(a,b,c) */
        "addl	" L3 ", %%r8d\n\t"

    STORE_ADD_DIGEST()

        "addq	$512, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_MASK[0]),
          [SHUF00BA] "m" (mSHUF_Y_00BA[0]),
          [SHUFDC00] "m" (mSHUF_Y_DC00[0]),
          [sha256]   "r" (sha256),
          [K]        "m" (K256)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory"
    );

    return 0;
}

SHA256_NOINLINE static int Transform_Sha256_AVX2_RORX_Len(wc_Sha256* sha256,
                                                          word32 len)
{
    if ((len & WC_SHA256_BLOCK_SIZE) != 0) {
        XMEMCPY(sha256->buffer, sha256->data, WC_SHA256_BLOCK_SIZE);
        Transform_Sha256_AVX2_RORX(sha256);
        sha256->data += WC_SHA256_BLOCK_SIZE;
        len -= WC_SHA256_BLOCK_SIZE;
        if (len == 0)
            return 0;
    }

    __asm__ __volatile__ (

        "subq	$512, %%rsp\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    INIT_MASKS_Y(BYTE_FLIP_Y_MASK, SHUF_Y_00BA, SHUF_Y_DC00)
    LOAD_DIGEST()

        "# Start of loop processing two blocks\n"
        "1:\n\t"

    LOAD_W_K(BYTE_FLIP_Y_MASK, rax)

        "movl	%%r9d, " L4 "\n\t"
        "rorx	$6, %%r12d, " L1 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    SET_W_Y_4(0)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  0)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  8)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 16)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 24)

    SET_W_Y_4(16)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 32)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 40)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 48)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 56)

    SET_W_Y_4(32)
    MsgSched_Y_RORX(Y0, Y1, Y2, Y3, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 64)
    MsgSched_Y_RORX(Y1, Y2, Y3, Y0, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 72)
    MsgSched_Y_RORX(Y2, Y3, Y0, Y1, S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 80)
    MsgSched_Y_RORX(Y3, Y0, Y1, Y2, S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 88)

    SET_W_Y_4(48)
        "xorl	" L3 ", " L3 "\n\t"
        "xorl	" L2 ", " L2 "\n\t"
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  96)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 104)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 112)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 120)
        /* Prev RND: h += Maj(a,b,c) */
        "addl	" L3 ", %%r8d\n\t"
        "xorl	" L2 ", " L2 "\n\t"

    ADD_DIGEST()
    STORE_DIGEST()

        "movl	%%r9d, " L4 "\n\t"
        "xorl	" L3 ", " L3 "\n\t"
        "xorl	%%r10d, " L4 "\n\t"

    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,   4)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  12)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  20)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  28)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  36)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  44)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  52)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  60)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  68)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  76)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7,  84)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3,  92)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 100)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 108)
    RND_RORX_X4(S_0, S_1, S_2, S_3, S_4, S_5, S_6, S_7, 116)
    RND_RORX_X4(S_4, S_5, S_6, S_7, S_0, S_1, S_2, S_3, 124)
        /* Prev RND: h += Maj(a,b,c) */
        "addl	" L3 ", %%r8d\n\t"
        "movq	120(%[sha256]), %%rax\n\t"

    ADD_DIGEST()

        "addq	$128, %%rax\n\t"
        "subl	$128, %[len]\n\t"

    STORE_DIGEST()

        "movq	%%rax, 120(%[sha256])\n\t"
        "jnz	1b\n\t"

        "addq	$512, %%rsp\n\t"

        :
        : [FLIP]     "m" (mBYTE_FLIP_Y_MASK[0]),
          [SHUF00BA] "m" (mSHUF_Y_00BA[0]),
          [SHUFDC00] "m" (mSHUF_Y_DC00[0]),
          [sha256]   "r" (sha256),
          [len]      "r" (len),
          [K]        "m" (K256)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory"
    );

    return 0;
}
#endif  /* HAVE_INTEL_RORX */
#endif  /* HAVE_INTEL_AVX2 */


#ifdef WOLFSSL_SHA224

#ifdef STM32_HASH_SHA2

    /* Supports CubeMX HAL or Standard Peripheral Library */

    int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId)
    {
        if (sha224 == NULL)
            return BAD_FUNC_ARG;

        (void)devId;
        (void)heap;

        wc_Stm32_Hash_Init(&sha224->stmCtx);
        return 0;
    }

    int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len)
    {
        int ret = 0;

        if (sha224 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Update(&sha224->stmCtx,
                HASH_AlgoSelection_SHA224, data, len);
            wolfSSL_CryptHwMutexUnLock();
        }
        return ret;
    }

    int wc_Sha224Final(wc_Sha224* sha224, byte* hash)
    {
        int ret = 0;

        if (sha224 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

        ret = wolfSSL_CryptHwMutexLock();
        if (ret == 0) {
            ret = wc_Stm32_Hash_Final(&sha224->stmCtx,
                HASH_AlgoSelection_SHA224, hash, WC_SHA224_DIGEST_SIZE);
            wolfSSL_CryptHwMutexUnLock();
        }

        (void)wc_InitSha224(sha224); /* reset state */

        return ret;
    }

#elif defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha256.c */
#else

    #define NEED_SOFT_SHA224


    static int InitSha224(wc_Sha224* sha224)
    {
        int ret = 0;

        if (sha224 == NULL) {
            return BAD_FUNC_ARG;
        }

        sha224->digest[0] = 0xc1059ed8;
        sha224->digest[1] = 0x367cd507;
        sha224->digest[2] = 0x3070dd17;
        sha224->digest[3] = 0xf70e5939;
        sha224->digest[4] = 0xffc00b31;
        sha224->digest[5] = 0x68581511;
        sha224->digest[6] = 0x64f98fa7;
        sha224->digest[7] = 0xbefa4fa4;

        sha224->buffLen = 0;
        sha224->loLen   = 0;
        sha224->hiLen   = 0;

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
        /* choose best Transform function under this runtime environment */
        Sha256_SetTransform();
    #endif

        return ret;
    }

#endif

#ifdef NEED_SOFT_SHA224
    int wc_InitSha224_ex(wc_Sha224* sha224, void* heap, int devId)
    {
        int ret = 0;

        if (sha224 == NULL)
            return BAD_FUNC_ARG;

        sha224->heap = heap;

        ret = InitSha224(sha224);
        if (ret != 0)
            return ret;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        ret = wolfAsync_DevCtxInit(&sha224->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA224, sha224->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return ret;
    }

    int wc_Sha224Update(wc_Sha224* sha224, const byte* data, word32 len)
    {
        int ret;

        if (sha224 == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        if (sha224->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA224) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha224(&sha224->asyncDev, NULL, data, len);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        ret = Sha256Update((wc_Sha256*)sha224, data, len);

        return ret;
    }

    int wc_Sha224Final(wc_Sha224* sha224, byte* hash)
    {
        int ret;

        if (sha224 == NULL || hash == NULL) {
            return BAD_FUNC_ARG;
        }

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        if (sha224->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA224) {
        #if defined(HAVE_INTEL_QA)
            return IntelQaSymSha224(&sha224->asyncDev, hash, NULL,
                                            WC_SHA224_DIGEST_SIZE);
        #endif
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        ret = Sha256Final((wc_Sha256*)sha224);
        if (ret != 0)
            return ret;

    #if defined(LITTLE_ENDIAN_ORDER)
        ByteReverseWords(sha224->digest, sha224->digest, WC_SHA224_DIGEST_SIZE);
    #endif
        XMEMCPY(hash, sha224->digest, WC_SHA224_DIGEST_SIZE);

        return InitSha224(sha224);  /* reset state */
    }
#endif /* end of SHA224 software implementation */

    int wc_InitSha224(wc_Sha224* sha224)
    {
        return wc_InitSha224_ex(sha224, NULL, INVALID_DEVID);
    }

    void wc_Sha224Free(wc_Sha224* sha224)
    {
        if (sha224 == NULL)
            return;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA224)
        wolfAsync_DevCtxFree(&sha224->asyncDev, WOLFSSL_ASYNC_MARKER_SHA224);
    #endif /* WOLFSSL_ASYNC_CRYPT */
    }
#endif /* WOLFSSL_SHA224 */


int wc_InitSha256(wc_Sha256* sha256)
{
    return wc_InitSha256_ex(sha256, NULL, INVALID_DEVID);
}

void wc_Sha256Free(wc_Sha256* sha256)
{
    if (sha256 == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA256)
    wolfAsync_DevCtxFree(&sha256->asyncDev, WOLFSSL_ASYNC_MARKER_SHA256);
#endif /* WOLFSSL_ASYNC_CRYPT */
}

#endif /* !WOLFSSL_TI_HASH */
#endif /* HAVE_FIPS */


#ifndef WOLFSSL_TI_HASH
#ifdef WOLFSSL_SHA224
    int wc_Sha224GetHash(wc_Sha224* sha224, byte* hash)
    {
        int ret;
        wc_Sha224 tmpSha224;

        if (sha224 == NULL || hash == NULL)
            return BAD_FUNC_ARG;

        ret = wc_Sha224Copy(sha224, &tmpSha224);
        if (ret == 0) {
            ret = wc_Sha224Final(&tmpSha224, hash);
        }
        return ret;
    }
    int wc_Sha224Copy(wc_Sha224* src, wc_Sha224* dst)
    {
        int ret = 0;

        if (src == NULL || dst == NULL)
            return BAD_FUNC_ARG;

        XMEMCPY(dst, src, sizeof(wc_Sha224));

    #ifdef WOLFSSL_ASYNC_CRYPT
        ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
    #endif

        return ret;
    }
#endif /* WOLFSSL_SHA224 */

int wc_Sha256GetHash(wc_Sha256* sha256, byte* hash)
{
    int ret;
    wc_Sha256 tmpSha256;

    if (sha256 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha256Copy(sha256, &tmpSha256);
    if (ret == 0) {
        ret = wc_Sha256Final(&tmpSha256, hash);
    }
    return ret;
}
int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha256));

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif
#ifdef WOLFSSL_PIC32MZ_HASH
    ret = wc_Pic32HashCopy(&src->cache, &dst->cache);
#endif

    return ret;
}
#endif /* !WOLFSSL_TI_HASH */

#endif /* NO_SHA256 */
