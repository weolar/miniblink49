/* sha512.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_SHA512
#include <wolfssl/wolfcrypt/sha512.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/cpuid.h>

/* deprecated USE_SLOW_SHA2 (replaced with USE_SLOW_SHA512) */
#if defined(USE_SLOW_SHA2) && !defined(USE_SLOW_SHA512)
    #define USE_SLOW_SHA512
#endif

/* fips wrapper calls, user can call direct */
#ifdef HAVE_FIPS
    int wc_InitSha512(wc_Sha512* sha)
    {
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }

        return InitSha512_fips(sha);
    }
    int wc_InitSha512_ex(wc_Sha512* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha512_fips(sha);
    }
    int wc_Sha512Update(wc_Sha512* sha, const byte* data, word32 len)
    {
        if (sha == NULL || (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }

        return Sha512Update_fips(sha, data, len);
    }
    int wc_Sha512Final(wc_Sha512* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }

        return Sha512Final_fips(sha, out);
    }
    void wc_Sha512Free(wc_Sha512* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

    #if defined(WOLFSSL_SHA384) || defined(HAVE_AESGCM)
        int wc_InitSha384(wc_Sha384* sha)
        {
            if (sha == NULL) {
                return BAD_FUNC_ARG;
            }
            return InitSha384_fips(sha);
        }
        int wc_InitSha384_ex(wc_Sha384* sha, void* heap, int devId)
        {
            (void)heap;
            (void)devId;
            if (sha == NULL) {
                return BAD_FUNC_ARG;
            }
            return InitSha384_fips(sha);
        }
        int wc_Sha384Update(wc_Sha384* sha, const byte* data, word32 len)
        {
            if (sha == NULL || (data == NULL && len > 0)) {
                return BAD_FUNC_ARG;
            }
            return Sha384Update_fips(sha, data, len);
        }
        int wc_Sha384Final(wc_Sha384* sha, byte* out)
        {
            if (sha == NULL || out == NULL) {
                return BAD_FUNC_ARG;
            }
            return Sha384Final_fips(sha, out);
        }
        void wc_Sha384Free(wc_Sha384* sha)
        {
            (void)sha;
            /* Not supported in FIPS */
        }
    #endif /* WOLFSSL_SHA384 || HAVE_AESGCM */

#else /* else build without using fips */

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
#endif

#if defined(HAVE_INTEL_AVX1)
    /* #define DEBUG_XMM  */
#endif

#if defined(HAVE_INTEL_AVX2)
    #define HAVE_INTEL_RORX
    /* #define DEBUG_YMM  */
#endif

#if defined(HAVE_BYTEREVERSE64) && \
        !defined(HAVE_INTEL_AVX1) && !defined(HAVE_INTEL_AVX2)
    #define ByteReverseWords64(out, in, size) ByteReverseWords64_1(out, size)
    #define ByteReverseWords64_1(buf, size) \
        { unsigned int i ;\
            for(i=0; i< size/sizeof(word64); i++){\
                __asm__ volatile("bswapq %0":"+r"(buf[i])::) ;\
            }\
        }
#endif

#if defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha.c */
#else

static int InitSha512(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return BAD_FUNC_ARG;

    sha512->digest[0] = W64LIT(0x6a09e667f3bcc908);
    sha512->digest[1] = W64LIT(0xbb67ae8584caa73b);
    sha512->digest[2] = W64LIT(0x3c6ef372fe94f82b);
    sha512->digest[3] = W64LIT(0xa54ff53a5f1d36f1);
    sha512->digest[4] = W64LIT(0x510e527fade682d1);
    sha512->digest[5] = W64LIT(0x9b05688c2b3e6c1f);
    sha512->digest[6] = W64LIT(0x1f83d9abfb41bd6b);
    sha512->digest[7] = W64LIT(0x5be0cd19137e2179);

    sha512->buffLen = 0;
    sha512->loLen   = 0;
    sha512->hiLen   = 0;

    return 0;
}


/* Hardware Acceleration */
#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)

    /*****
    Intel AVX1/AVX2 Macro Control Structure

    #if defined(HAVE_INteL_SPEEDUP)
        #define HAVE_INTEL_AVX1
        #define HAVE_INTEL_AVX2
    #endif

    int InitSha512(wc_Sha512* sha512) {
         Save/Recover XMM, YMM
         ...

         Check Intel AVX cpuid flags
    }

    #if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
      Transform_Sha512_AVX1(); # Function prototype
      Transform_Sha512_AVX2(); #
    #endif

      _Transform_Sha512() {     # Native Transform Function body

      }

      int Sha512Update() {
         Save/Recover XMM, YMM
         ...
      }

      int Sha512Final() {
         Save/Recover XMM, YMM
         ...
      }


    #if defined(HAVE_INTEL_AVX1)

       XMM Instructions/INLINE asm Definitions

    #endif

    #if defined(HAVE_INTEL_AVX2)

       YMM Instructions/INLINE asm Definitions

    #endif

    #if defnied(HAVE_INTEL_AVX1)

      int Transform_Sha512_AVX1() {
          Stitched Message Sched/Round
      }

    #endif

    #if defnied(HAVE_INTEL_AVX2)

      int Transform_Sha512_AVX2() {
          Stitched Message Sched/Round
      }
    #endif

    */


    /* Each platform needs to query info type 1 from cpuid to see if aesni is
     * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
     */

    #if defined(HAVE_INTEL_AVX1)
        static int Transform_Sha512_AVX1(wc_Sha512 *sha512);
        static int Transform_Sha512_AVX1_Len(wc_Sha512 *sha512, word32 len);
    #endif
    #if defined(HAVE_INTEL_AVX2)
        static int Transform_Sha512_AVX2(wc_Sha512 *sha512);
        static int Transform_Sha512_AVX2_Len(wc_Sha512 *sha512, word32 len);
        #if defined(HAVE_INTEL_RORX)
            static int Transform_Sha512_AVX1_RORX(wc_Sha512 *sha512);
            static int Transform_Sha512_AVX1_RORX_Len(wc_Sha512 *sha512,
                                                      word32 len);
            static int Transform_Sha512_AVX2_RORX(wc_Sha512 *sha512);
            static int Transform_Sha512_AVX2_RORX_Len(wc_Sha512 *sha512,
                                                      word32 len);
        #endif
    #endif
    static int _Transform_Sha512(wc_Sha512 *sha512);
    static int (*Transform_Sha512_p)(wc_Sha512* sha512) = _Transform_Sha512;
    static int (*Transform_Sha512_Len_p)(wc_Sha512* sha512, word32 len) = NULL;
    static int transform_check = 0;
    static int intel_flags;
    #define Transform_Sha512(sha512)     (*Transform_Sha512_p)(sha512)
    #define Transform_Sha512_Len(sha512, len) \
        (*Transform_Sha512_Len_p)(sha512, len)

    static void Sha512_SetTransform()
    {
        if (transform_check)
            return;

        intel_flags = cpuid_get_flags();

    #if defined(HAVE_INTEL_AVX2)
        if (IS_INTEL_AVX2(intel_flags)) {
        #ifdef HAVE_INTEL_RORX
            if (IS_INTEL_BMI2(intel_flags)) {
                Transform_Sha512_p = Transform_Sha512_AVX2_RORX;
                Transform_Sha512_Len_p = Transform_Sha512_AVX2_RORX_Len;
            }
            else
        #endif
            if (1) {
                Transform_Sha512_p = Transform_Sha512_AVX2;
                Transform_Sha512_Len_p = Transform_Sha512_AVX2_Len;
            }
        #ifdef HAVE_INTEL_RORX
            else {
                Transform_Sha512_p = Transform_Sha512_AVX1_RORX;
                Transform_Sha512_Len_p = Transform_Sha512_AVX1_RORX_Len;
            }
        #endif
        }
        else
    #endif
    #if defined(HAVE_INTEL_AVX1)
        if (IS_INTEL_AVX1(intel_flags)) {
            Transform_Sha512_p = Transform_Sha512_AVX1;
            Transform_Sha512_Len_p = Transform_Sha512_AVX1_Len;
        }
        else
    #endif
            Transform_Sha512_p = _Transform_Sha512;

        transform_check = 1;
    }

    int wc_InitSha512_ex(wc_Sha512* sha512, void* heap, int devId)
    {
        int ret = InitSha512(sha512);

        (void)heap;
        (void)devId;

        Sha512_SetTransform();

        return ret;
    }

#else
    #define Transform_Sha512(sha512) _Transform_Sha512(sha512)

    int wc_InitSha512_ex(wc_Sha512* sha512, void* heap, int devId)
    {
        int ret = 0;

        if (sha512 == NULL)
            return BAD_FUNC_ARG;

        sha512->heap = heap;

        ret = InitSha512(sha512);
        if (ret != 0)
            return ret;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA512)
        ret = wolfAsync_DevCtxInit(&sha512->asyncDev,
                            WOLFSSL_ASYNC_MARKER_SHA512, sha512->heap, devId);
    #else
        (void)devId;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        return ret;
    }

#endif /* Hardware Acceleration */

static const word64 K512[80] = {
    W64LIT(0x428a2f98d728ae22), W64LIT(0x7137449123ef65cd),
    W64LIT(0xb5c0fbcfec4d3b2f), W64LIT(0xe9b5dba58189dbbc),
    W64LIT(0x3956c25bf348b538), W64LIT(0x59f111f1b605d019),
    W64LIT(0x923f82a4af194f9b), W64LIT(0xab1c5ed5da6d8118),
    W64LIT(0xd807aa98a3030242), W64LIT(0x12835b0145706fbe),
    W64LIT(0x243185be4ee4b28c), W64LIT(0x550c7dc3d5ffb4e2),
    W64LIT(0x72be5d74f27b896f), W64LIT(0x80deb1fe3b1696b1),
    W64LIT(0x9bdc06a725c71235), W64LIT(0xc19bf174cf692694),
    W64LIT(0xe49b69c19ef14ad2), W64LIT(0xefbe4786384f25e3),
    W64LIT(0x0fc19dc68b8cd5b5), W64LIT(0x240ca1cc77ac9c65),
    W64LIT(0x2de92c6f592b0275), W64LIT(0x4a7484aa6ea6e483),
    W64LIT(0x5cb0a9dcbd41fbd4), W64LIT(0x76f988da831153b5),
    W64LIT(0x983e5152ee66dfab), W64LIT(0xa831c66d2db43210),
    W64LIT(0xb00327c898fb213f), W64LIT(0xbf597fc7beef0ee4),
    W64LIT(0xc6e00bf33da88fc2), W64LIT(0xd5a79147930aa725),
    W64LIT(0x06ca6351e003826f), W64LIT(0x142929670a0e6e70),
    W64LIT(0x27b70a8546d22ffc), W64LIT(0x2e1b21385c26c926),
    W64LIT(0x4d2c6dfc5ac42aed), W64LIT(0x53380d139d95b3df),
    W64LIT(0x650a73548baf63de), W64LIT(0x766a0abb3c77b2a8),
    W64LIT(0x81c2c92e47edaee6), W64LIT(0x92722c851482353b),
    W64LIT(0xa2bfe8a14cf10364), W64LIT(0xa81a664bbc423001),
    W64LIT(0xc24b8b70d0f89791), W64LIT(0xc76c51a30654be30),
    W64LIT(0xd192e819d6ef5218), W64LIT(0xd69906245565a910),
    W64LIT(0xf40e35855771202a), W64LIT(0x106aa07032bbd1b8),
    W64LIT(0x19a4c116b8d2d0c8), W64LIT(0x1e376c085141ab53),
    W64LIT(0x2748774cdf8eeb99), W64LIT(0x34b0bcb5e19b48a8),
    W64LIT(0x391c0cb3c5c95a63), W64LIT(0x4ed8aa4ae3418acb),
    W64LIT(0x5b9cca4f7763e373), W64LIT(0x682e6ff3d6b2b8a3),
    W64LIT(0x748f82ee5defb2fc), W64LIT(0x78a5636f43172f60),
    W64LIT(0x84c87814a1f0ab72), W64LIT(0x8cc702081a6439ec),
    W64LIT(0x90befffa23631e28), W64LIT(0xa4506cebde82bde9),
    W64LIT(0xbef9a3f7b2c67915), W64LIT(0xc67178f2e372532b),
    W64LIT(0xca273eceea26619c), W64LIT(0xd186b8c721c0c207),
    W64LIT(0xeada7dd6cde0eb1e), W64LIT(0xf57d4f7fee6ed178),
    W64LIT(0x06f067aa72176fba), W64LIT(0x0a637dc5a2c898a6),
    W64LIT(0x113f9804bef90dae), W64LIT(0x1b710b35131c471b),
    W64LIT(0x28db77f523047d84), W64LIT(0x32caab7b40c72493),
    W64LIT(0x3c9ebe0a15c9bebc), W64LIT(0x431d67c49c100d4c),
    W64LIT(0x4cc5d4becb3e42b6), W64LIT(0x597f299cfc657e2a),
    W64LIT(0x5fcb6fab3ad6faec), W64LIT(0x6c44198c4a475817)
};

#define blk0(i) (W[i] = sha512->buffer[i])

#define blk2(i) (\
               W[ i     & 15] += \
            s1(W[(i-2)  & 15])+ \
               W[(i-7)  & 15] + \
            s0(W[(i-15) & 15])  \
        )

#define Ch(x,y,z)  (z ^ (x & (y ^ z)))
#define Maj(x,y,z) ((x & y) | (z & (x | y)))

#define a(i) T[(0-i) & 7]
#define b(i) T[(1-i) & 7]
#define c(i) T[(2-i) & 7]
#define d(i) T[(3-i) & 7]
#define e(i) T[(4-i) & 7]
#define f(i) T[(5-i) & 7]
#define g(i) T[(6-i) & 7]
#define h(i) T[(7-i) & 7]

#define S0(x) (rotrFixed64(x,28) ^ rotrFixed64(x,34) ^ rotrFixed64(x,39))
#define S1(x) (rotrFixed64(x,14) ^ rotrFixed64(x,18) ^ rotrFixed64(x,41))
#define s0(x) (rotrFixed64(x,1)  ^ rotrFixed64(x,8)  ^ (x>>7))
#define s1(x) (rotrFixed64(x,19) ^ rotrFixed64(x,61) ^ (x>>6))

#define R(i) \
    h(i) += S1(e(i)) + Ch(e(i),f(i),g(i)) + K[i+j] + (j ? blk2(i) : blk0(i)); \
    d(i) += h(i); \
    h(i) += S0(a(i)) + Maj(a(i),b(i),c(i))

static int _Transform_Sha512(wc_Sha512* sha512)
{
    const word64* K = K512;
    word32 j;
    word64 T[8];

#ifdef WOLFSSL_SMALL_STACK
    word64* W;
    W = (word64*) XMALLOC(sizeof(word64) * 16, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (W == NULL)
        return MEMORY_E;
#else
    word64 W[16];
#endif

    /* Copy digest to working vars */
    XMEMCPY(T, sha512->digest, sizeof(T));

#ifdef USE_SLOW_SHA512
    /* over twice as small, but 50% slower */
    /* 80 operations, not unrolled */
    for (j = 0; j < 80; j += 16) {
        int m;
        for (m = 0; m < 16; m++) { /* braces needed here for macros {} */
            R(m);
        }
    }
#else
    /* 80 operations, partially loop unrolled */
    for (j = 0; j < 80; j += 16) {
        R( 0); R( 1); R( 2); R( 3);
        R( 4); R( 5); R( 6); R( 7);
        R( 8); R( 9); R(10); R(11);
        R(12); R(13); R(14); R(15);
    }
#endif /* USE_SLOW_SHA512 */

    /* Add the working vars back into digest */
    sha512->digest[0] += a(0);
    sha512->digest[1] += b(0);
    sha512->digest[2] += c(0);
    sha512->digest[3] += d(0);
    sha512->digest[4] += e(0);
    sha512->digest[5] += f(0);
    sha512->digest[6] += g(0);
    sha512->digest[7] += h(0);

    /* Wipe variables */
    ForceZero(W, sizeof(word64) * 16);
    ForceZero(T, sizeof(T));

#ifdef WOLFSSL_SMALL_STACK
    XFREE(W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}


static INLINE void AddLength(wc_Sha512* sha512, word32 len)
{
    word64 tmp = sha512->loLen;
    if ( (sha512->loLen += len) < tmp)
        sha512->hiLen++;                       /* carry low to high */
}

static INLINE int Sha512Update(wc_Sha512* sha512, const byte* data, word32 len)
{
    int ret = 0;
    /* do block size increments */
    byte* local = (byte*)sha512->buffer;

    /* check that internal buffLen is valid */
    if (sha512->buffLen >= WC_SHA512_BLOCK_SIZE)
        return BUFFER_E;

    if (sha512->buffLen > 0) {
        word32 add = min(len, WC_SHA512_BLOCK_SIZE - sha512->buffLen);
        XMEMCPY(&local[sha512->buffLen], data, add);

        sha512->buffLen += add;
        data            += add;
        len             -= add;

        if (sha512->buffLen == WC_SHA512_BLOCK_SIZE) {
    #if defined(LITTLE_ENDIAN_ORDER)
        #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
            if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
        #endif
            {
                ByteReverseWords64(sha512->buffer, sha512->buffer,
                                                          WC_SHA512_BLOCK_SIZE);
            }
    #endif
            ret = Transform_Sha512(sha512);
            if (ret == 0) {
                AddLength(sha512, WC_SHA512_BLOCK_SIZE);
                sha512->buffLen = 0;
            }
            else
                len = 0;
        }
    }

#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
    if (Transform_Sha512_Len_p != NULL) {
        word32 blocksLen = len & ~(WC_SHA512_BLOCK_SIZE-1);

        if (blocksLen > 0) {
            AddLength(sha512, blocksLen);
            sha512->data = data;
            /* Byte reversal performed in function if required. */
            Transform_Sha512_Len(sha512, blocksLen);
            data += blocksLen;
            len  -= blocksLen;
        }
    }
    else
#endif
#if !defined(LITTLE_ENDIAN_ORDER) || defined(FREESCALE_MMCAU_SHA) || \
                            defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
    {
        word32 blocksLen = len & ~(WC_SHA512_BLOCK_SIZE-1);

        AddLength(sha512, blocksLen);
        while (len >= WC_SHA512_BLOCK_SIZE) {
            XMEMCPY(local, data, WC_SHA512_BLOCK_SIZE);

            data += WC_SHA512_BLOCK_SIZE;
            len  -= WC_SHA512_BLOCK_SIZE;

            /* Byte reversal performed in function if required. */
            ret = Transform_Sha512(sha512);
            if (ret != 0)
                break;
        }
    }
#else
    {
        word32 blocksLen = len & ~(WC_SHA512_BLOCK_SIZE-1);

        AddLength(sha512, blocksLen);
        while (len >= WC_SHA512_BLOCK_SIZE) {
            XMEMCPY(local, data, WC_SHA512_BLOCK_SIZE);

            data += WC_SHA512_BLOCK_SIZE;
            len  -= WC_SHA512_BLOCK_SIZE;

            ByteReverseWords64(sha512->buffer, sha512->buffer,
                                                          WC_SHA512_BLOCK_SIZE);
            ret = Transform_Sha512(sha512);
            if (ret != 0)
                break;
        }
    }
#endif

    if (len > 0) {
        XMEMCPY(local, data, len);
        sha512->buffLen = len;
    }

    return ret;
}

int wc_Sha512Update(wc_Sha512* sha512, const byte* data, word32 len)
{
    if (sha512 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA512)
    if (sha512->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA512) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha512(&sha512->asyncDev, NULL, data, len);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    return Sha512Update(sha512, data, len);
}
#endif /* WOLFSSL_IMX6_CAAM */

static INLINE int Sha512Final(wc_Sha512* sha512)
{
    byte* local = (byte*)sha512->buffer;
    int ret;

    if (sha512 == NULL) {
        return BAD_FUNC_ARG;
    }

    AddLength(sha512, sha512->buffLen);               /* before adding pads */

    local[sha512->buffLen++] = 0x80;  /* add 1 */

    /* pad with zeros */
    if (sha512->buffLen > WC_SHA512_PAD_SIZE) {
        XMEMSET(&local[sha512->buffLen], 0, WC_SHA512_BLOCK_SIZE - sha512->buffLen);
        sha512->buffLen += WC_SHA512_BLOCK_SIZE - sha512->buffLen;
#if defined(LITTLE_ENDIAN_ORDER)
    #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
    #endif
        {
            ByteReverseWords64(sha512->buffer,sha512->buffer,
                                                             WC_SHA512_BLOCK_SIZE);
        }
#endif /* LITTLE_ENDIAN_ORDER */
        ret = Transform_Sha512(sha512);
        if (ret != 0)
            return ret;

        sha512->buffLen = 0;
    }
    XMEMSET(&local[sha512->buffLen], 0, WC_SHA512_PAD_SIZE - sha512->buffLen);

    /* put lengths in bits */
    sha512->hiLen = (sha512->loLen >> (8 * sizeof(sha512->loLen) - 3)) +
                                                         (sha512->hiLen << 3);
    sha512->loLen = sha512->loLen << 3;

    /* store lengths */
#if defined(LITTLE_ENDIAN_ORDER)
    #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        if (!IS_INTEL_AVX1(intel_flags) && !IS_INTEL_AVX2(intel_flags))
    #endif
            ByteReverseWords64(sha512->buffer, sha512->buffer, WC_SHA512_PAD_SIZE);
#endif
    /* ! length ordering dependent on digest endian type ! */

    sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2] = sha512->hiLen;
    sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 1] = sha512->loLen;
#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
    if (IS_INTEL_AVX1(intel_flags) || IS_INTEL_AVX2(intel_flags))
        ByteReverseWords64(&(sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2]),
                           &(sha512->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2]),
                           WC_SHA512_BLOCK_SIZE - WC_SHA512_PAD_SIZE);
#endif
    ret = Transform_Sha512(sha512);
    if (ret != 0)
        return ret;

    #ifdef LITTLE_ENDIAN_ORDER
        ByteReverseWords64(sha512->digest, sha512->digest, WC_SHA512_DIGEST_SIZE);
    #endif

    return 0;
}

int wc_Sha512FinalRaw(wc_Sha512* sha512, byte* hash)
{
#ifdef LITTLE_ENDIAN_ORDER
    word64 digest[WC_SHA512_DIGEST_SIZE / sizeof(word64)];
#endif

    if (sha512 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef LITTLE_ENDIAN_ORDER
    ByteReverseWords64((word64*)digest, (word64*)sha512->digest,
                                                         WC_SHA512_DIGEST_SIZE);
    XMEMCPY(hash, digest, WC_SHA512_DIGEST_SIZE);
#else
    XMEMCPY(hash, sha512->digest, WC_SHA512_DIGEST_SIZE);
#endif

    return 0;
}

int wc_Sha512Final(wc_Sha512* sha512, byte* hash)
{
    int ret;

    if (sha512 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA512)
    if (sha512->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA512) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha512(&sha512->asyncDev, hash, NULL,
                                            WC_SHA512_DIGEST_SIZE);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha512Final(sha512);
    if (ret != 0)
        return ret;

    XMEMCPY(hash, sha512->digest, WC_SHA512_DIGEST_SIZE);

    return InitSha512(sha512);  /* reset state */
}


int wc_InitSha512(wc_Sha512* sha512)
{
    return wc_InitSha512_ex(sha512, NULL, INVALID_DEVID);
}

void wc_Sha512Free(wc_Sha512* sha512)
{
    if (sha512 == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA512)
    wolfAsync_DevCtxFree(&sha512->asyncDev, WOLFSSL_ASYNC_MARKER_SHA512);
#endif /* WOLFSSL_ASYNC_CRYPT */
}


#if defined(HAVE_INTEL_AVX1)

static word64 mBYTE_FLIP_MASK[] =  { 0x0001020304050607, 0x08090a0b0c0d0e0f };

#define W_0     xmm0
#define W_2     xmm1
#define W_4     xmm2
#define W_6     xmm3
#define W_8     xmm4
#define W_10    xmm5
#define W_12    xmm6
#define W_14    xmm7

#define W_M15   xmm12
#define W_M7    xmm13
#define MASK    xmm14

#define XTMP1   xmm8
#define XTMP2   xmm9
#define XTMP3   xmm10
#define XTMP4   xmm11

#define XMM_REGS \
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",       \
    "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"

#define _VPALIGNR(dest, src1, src2, bits)                               \
    "vpalignr	$" #bits ", %%" #src2 ", %%" #src1 ", %%" #dest "\n\t"
#define VPALIGNR(dest, src1, src2, bits) \
       _VPALIGNR(dest, src1, src2, bits)

#define _V_SHIFT_R(dest, src, bits)                             \
    "vpsrlq	$" #bits ", %%" #src ", %%" #dest "\n\t"
#define V_SHIFT_R(dest, src, bits) \
       _V_SHIFT_R(dest, src, bits)

#define _V_SHIFT_L(dest, src, bits)                             \
    "vpsllq	$" #bits ", %%" #src ", %%" #dest "\n\t"
#define V_SHIFT_L(dest, src, bits) \
       _V_SHIFT_L(dest, src, bits)

#define _V_ADD(dest, src1, src2)                                \
    "vpaddq	%%" #src1 ", %%" #src2 ", %%" #dest "\n\t"
#define V_ADD(dest, src1, src2) \
       _V_ADD(dest, src1, src2)

#define _V_XOR(dest, src1, src2)                                \
    "vpxor	%%" #src1 ", %%" #src2 ", %%" #dest "\n\t"
#define V_XOR(dest, src1, src2) \
       _V_XOR(dest, src1, src2)

#define _V_OR(dest, src1, src2)                                 \
    "vpor	%%" #src1 ", %%" #src2 ", %%" #dest "\n\t"
#define V_OR(dest, src1, src2) \
       _V_OR(dest, src1, src2)

#define RA  %%r8
#define RB  %%r9
#define RC  %%r10
#define RD  %%r11
#define RE  %%r12
#define RF  %%r13
#define RG  %%r14
#define RH  %%r15

#define STATE_REGS "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"

#define L1  "%%rax"
#define L2  "%%rcx"
#define L3  "%%rdx"
#define L4  "%%rbx"
#define WX  "%%rsp"

#define WORK_REGS "rax", "rbx", "rcx", "rdx"

#define RND_0_1(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = e >>> 23 */                              \
    "rorq	 $23, " L1 "\n\t"                    \

#define RND_0_2(a,b,c,d,e,f,g,h,i)                   \
    /* L3 = a */                                     \
    "movq	"#a", " L3 "\n\t"                    \
    /* L2 = f */                                     \
    "movq	"#f", " L2 "\n\t"                    \
    /* h += W_X[i] */                                \
    "addq	("#i")*8(" WX "), "#h"\n\t"          \
    /* L2 = f ^ g */                                 \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_0_2_A(a,b,c,d,e,f,g,h,i)                 \
    /* L3 = a */                                     \
    "movq	"#a", " L3 "\n\t"                    \
    /* L2 = f */                                     \
    "movq	"#f", " L2 "\n\t"                    \

#define RND_0_2_B(a,b,c,d,e,f,g,h,i)                 \
    /* h += W_X[i] */                                \
    "addq	("#i")*8(" WX "), "#h"\n\t"          \
    /* L2 = f ^ g */                                 \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_0_3(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = (e >>> 23) ^ e */                        \
    "xorq	"#e", " L1 "\n\t"                    \
    /* L2 = (f ^ g) & e */                           \
    "andq	"#e", " L2 "\n\t"                    \

#define RND_0_4(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = ((e >>> 23) ^ e) >>> 4 */                \
    "rorq	 $4, " L1 "\n\t"                     \
    /* L2 = ((f ^ g) & e) ^ g */                     \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_0_5(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = (((e >>> 23) ^ e) >>> 4) ^ e */          \
    "xorq	"#e", " L1 "\n\t"                    \
    /* h += Ch(e,f,g) */                             \
    "addq	" L2 ", "#h"\n\t"                    \

#define RND_0_6(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = ((((e >>> 23) ^ e) >>> 4) ^ e) >>> 14 */ \
    "rorq	$14, " L1 "\n\t"                     \
    /* L3 = a ^ b */                                 \
    "xorq	"#b", " L3 "\n\t"                    \

#define RND_0_7(a,b,c,d,e,f,g,h,i)                   \
    /* h += Sigma1(e) */                             \
    "addq	" L1 ", "#h"\n\t"                    \
    /* L2 = a */                                     \
    "movq	"#a", " L2 "\n\t"                    \

#define RND_0_8(a,b,c,d,e,f,g,h,i)                   \
    /* L4 = (a ^ b) & (b ^ c) */                     \
    "andq	" L3 ", " L4 "\n\t"                  \
    /* L2 = a >>> 5 */                               \
    "rorq	$5, " L2 "\n\t"                      \

#define RND_0_9(a,b,c,d,e,f,g,h,i)                   \
    /* L2 = (a >>> 5) ^ a */                         \
    "xorq	"#a", " L2 "\n\t"                    \
    /* L4 = ((a ^ b) & (b ^ c) ^ b */                \
    "xorq	"#b", " L4 "\n\t"                    \

#define RND_0_10(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = ((a >>> 5) ^ a) >>> 6 */                 \
    "rorq	 $6, " L2 "\n\t"                     \
    /* d += h */                                     \
    "addq	"#h", "#d"\n\t"                      \

#define RND_0_11(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = (((a >>> 5) ^ a) >>> 6) ^ a */           \
    "xorq	"#a", " L2 "\n\t"                    \
    /* h += Sigma0(a) */                             \
    "addq	" L4 ", "#h"\n\t"                    \

#define RND_0_12(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = ((((a >>> 5) ^ a) >>> 6) ^ a) >>> 28 */  \
    "rorq	$28, " L2 "\n\t"                     \
    /* d (= e next RND) */                           \
    "movq	"#d", " L1 "\n\t"                    \
    /* h += Maj(a,b,c) */                            \
    "addq	" L2 ", "#h"\n\t"                    \

#define RND_1_1(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = e >>> 23 */                              \
    "rorq	 $23, " L1 "\n\t"                    \

#define RND_1_2(a,b,c,d,e,f,g,h,i)                   \
    /* L4 = a */                                     \
    "movq	"#a", " L4 "\n\t"                    \
    /* L2 = f */                                     \
    "movq	"#f", " L2 "\n\t"                    \
    /* h += W_X[i] */                                \
    "addq	("#i")*8(" WX "), "#h"\n\t"          \
    /* L2 = f ^ g */                                 \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_1_2_A(a,b,c,d,e,f,g,h,i)                 \
    /* L4 = a */                                     \
    "movq	"#a", " L4 "\n\t"                    \
    /* L2 = f */                                     \
    "movq	"#f", " L2 "\n\t"                    \

#define RND_1_2_B(a,b,c,d,e,f,g,h,i)                 \
    /* h += W_X[i] */                                \
    "addq	("#i")*8(" WX "), "#h"\n\t"          \
    /* L2 = f ^ g */                                 \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_1_3(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = (e >>> 23) ^ e */                        \
    "xorq	"#e", " L1 "\n\t"                    \
    /* L2 = (f ^ g) & e */                           \
    "andq	"#e", " L2 "\n\t"                    \

#define RND_1_4(a,b,c,d,e,f,g,h,i)                   \
    /* ((e >>> 23) ^ e) >>> 4 */                     \
    "rorq	 $4, " L1 "\n\t"                     \
    /* ((f ^ g) & e) ^ g */                          \
    "xorq	"#g", " L2 "\n\t"                    \

#define RND_1_5(a,b,c,d,e,f,g,h,i)                   \
    /* (((e >>> 23) ^ e) >>> 4) ^ e */               \
    "xorq	"#e", " L1 "\n\t"                    \
    /* h += Ch(e,f,g) */                             \
    "addq	" L2 ", "#h"\n\t"                    \

#define RND_1_6(a,b,c,d,e,f,g,h,i)                   \
    /* L1 = ((((e >>> 23) ^ e) >>> 4) ^ e) >>> 14 */ \
    "rorq	$14, " L1 "\n\t"                     \
    /* L4 = a ^ b */                                 \
    "xorq	"#b", " L4 "\n\t"                    \

#define RND_1_7(a,b,c,d,e,f,g,h,i)                   \
    /* h += Sigma1(e) */                             \
    "addq	" L1 ", "#h"\n\t"                    \
    /* L2 = a */                                     \
    "movq	"#a", " L2 "\n\t"                    \
 
#define RND_1_8(a,b,c,d,e,f,g,h,i)                   \
    /* L3 = (a ^ b) & (b ^ c) */                     \
    "andq	" L4 ", " L3 "\n\t"                  \
    /* L2 = a >>> 5 */                               \
    "rorq	$5, " L2 "\n\t"                      \

#define RND_1_9(a,b,c,d,e,f,g,h,i)                   \
    /* L2 = (a >>> 5) ^ a */                         \
    "xorq	"#a", " L2 "\n\t"                    \
    /* L3 = ((a ^ b) & (b ^ c) ^ b */                \
    "xorq	"#b", " L3 "\n\t"                    \

#define RND_1_10(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = ((a >>> 5) ^ a) >>> 6 */                 \
    "rorq	 $6, " L2 "\n\t"                     \
    /* d += h */                                     \
    "addq	"#h", "#d"\n\t"                      \

#define RND_1_11(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = (((a >>> 5) ^ a) >>> 6) ^ a */           \
    "xorq	"#a", " L2 "\n\t"                    \
    /* h += Sigma0(a) */                             \
    "addq	" L3 ", "#h"\n\t"                    \

#define RND_1_12(a,b,c,d,e,f,g,h,i)                  \
    /* L2 = ((((a >>> 5) ^ a) >>> 6) ^ a) >>> 28 */  \
    "rorq	$28, " L2 "\n\t"                     \
    /* d (= e next RND) */                           \
    "movq	"#d", " L1 "\n\t"                    \
    /* h += Maj(a,b,c) */                            \
    "addq	" L2 ", "#h"\n\t"                    \


#define MsgSched2(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,a,b,c,d,e,f,g,h,i) \
            RND_0_1(a,b,c,d,e,f,g,h,i)                                  \
    VPALIGNR(W_M15, W_2, W_0, 8)                                        \
    VPALIGNR(W_M7, W_10, W_8, 8)                                        \
            RND_0_2(a,b,c,d,e,f,g,h,i)                                  \
    V_SHIFT_R(XTMP1, W_M15, 1)                                          \
    V_SHIFT_L(XTMP2, W_M15, 63)                                         \
            RND_0_3(a,b,c,d,e,f,g,h,i)                                  \
            RND_0_4(a,b,c,d,e,f,g,h,i)                                  \
    V_SHIFT_R(XTMP3, W_M15, 8)                                          \
    V_SHIFT_L(XTMP4, W_M15, 56)                                         \
            RND_0_5(a,b,c,d,e,f,g,h,i)                                  \
            RND_0_6(a,b,c,d,e,f,g,h,i)                                  \
    V_OR(XTMP1, XTMP2, XTMP1)                                           \
    V_OR(XTMP3, XTMP4, XTMP3)                                           \
            RND_0_7(a,b,c,d,e,f,g,h,i)                                  \
            RND_0_8(a,b,c,d,e,f,g,h,i)                                  \
    V_SHIFT_R(XTMP4, W_M15, 7)                                          \
    V_XOR(XTMP1, XTMP3, XTMP1)                                          \
            RND_0_9(a,b,c,d,e,f,g,h,i)                                  \
            RND_0_10(a,b,c,d,e,f,g,h,i)                                 \
    V_XOR(XTMP1, XTMP4, XTMP1)                                          \
    V_ADD(W_0, W_0, W_M7)                                               \
            RND_0_11(a,b,c,d,e,f,g,h,i)                                 \
            RND_0_12(a,b,c,d,e,f,g,h,i)                                 \
            RND_1_1(h,a,b,c,d,e,f,g,i+1)                                \
    V_ADD(W_0, W_0, XTMP1)                                              \
            RND_1_2(h,a,b,c,d,e,f,g,i+1)                                \
    V_SHIFT_R(XTMP1, W_14, 19)                                          \
    V_SHIFT_L(XTMP2, W_14, 45)                                          \
            RND_1_3(h,a,b,c,d,e,f,g,i+1)                                \
            RND_1_4(h,a,b,c,d,e,f,g,i+1)                                \
    V_SHIFT_R(XTMP3, W_14, 61)                                          \
    V_SHIFT_L(XTMP4, W_14, 3)                                           \
            RND_1_5(h,a,b,c,d,e,f,g,i+1)                                \
            RND_1_6(h,a,b,c,d,e,f,g,i+1)                                \
            RND_1_7(h,a,b,c,d,e,f,g,i+1)                                \
    V_OR(XTMP1, XTMP2, XTMP1)                                           \
    V_OR(XTMP3, XTMP4, XTMP3)                                           \
            RND_1_8(h,a,b,c,d,e,f,g,i+1)                                \
            RND_1_9(h,a,b,c,d,e,f,g,i+1)                                \
    V_XOR(XTMP1, XTMP3, XTMP1)                                          \
    V_SHIFT_R(XTMP4, W_14, 6)                                           \
            RND_1_10(h,a,b,c,d,e,f,g,i+1)                               \
            RND_1_11(h,a,b,c,d,e,f,g,i+1)                               \
    V_XOR(XTMP1, XTMP4, XTMP1)                                          \
            RND_1_12(h,a,b,c,d,e,f,g,i+1)                               \
    V_ADD(W_0, W_0, XTMP1)                                              \

#define RND_ALL_2(a, b, c, d, e, f, g, h, i) \
    RND_0_1 (a, b, c, d, e, f, g, h, i )     \
    RND_0_2 (a, b, c, d, e, f, g, h, i )     \
    RND_0_3 (a, b, c, d, e, f, g, h, i )     \
    RND_0_4 (a, b, c, d, e, f, g, h, i )     \
    RND_0_5 (a, b, c, d, e, f, g, h, i )     \
    RND_0_6 (a, b, c, d, e, f, g, h, i )     \
    RND_0_7 (a, b, c, d, e, f, g, h, i )     \
    RND_0_8 (a, b, c, d, e, f, g, h, i )     \
    RND_0_9 (a, b, c, d, e, f, g, h, i )     \
    RND_0_10(a, b, c, d, e, f, g, h, i )     \
    RND_0_11(a, b, c, d, e, f, g, h, i )     \
    RND_0_12(a, b, c, d, e, f, g, h, i )     \
    RND_1_1 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_2 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_3 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_4 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_5 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_6 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_7 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_8 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_9 (h, a, b, c, d, e, f, g, i+1)    \
    RND_1_10(h, a, b, c, d, e, f, g, i+1)    \
    RND_1_11(h, a, b, c, d, e, f, g, i+1)    \
    RND_1_12(h, a, b, c, d, e, f, g, i+1)


#if defined(HAVE_INTEL_RORX)

#define RND_RORX_0_1(a, b, c, d, e, f, g, h, i) \
    /* L1 = e>>>14 */                           \
    "rorxq	$14, "#e", " L1 "\n\t"          \
    /* L2 = e>>>18 */                           \
    "rorxq	$18, "#e", " L2 "\n\t"          \
    /* Prev RND: h += Maj(a,b,c) */             \
    "addq	" L3 ", "#a"\n\t"               \

#define RND_RORX_0_2(a, b, c, d, e, f, g, h, i) \
    /* h += w_k */                              \
    "addq	("#i")*8(" WX "), "#h"\n\t"     \
    /* L3 = f */                                \
    "movq	"#f", " L3 "\n\t"               \
    /* L2 = (e>>>14) ^ (e>>>18) */              \
    "xorq	" L1 ", " L2 "\n\t"             \

#define RND_RORX_0_3(a, b, c, d, e, f, g, h, i) \
    /* L3 = f ^ g */                            \
    "xorq	"#g", " L3 "\n\t"               \
    /* L1 = e>>>41 */                           \
    "rorxq	$41, "#e", " L1 "\n\t"          \
    /* L1 = Sigma1(e) */                        \
    "xorq	" L2 ", " L1 "\n\t"             \

#define RND_RORX_0_4(a, b, c, d, e, f, g, h, i) \
    /* L3 = (f ^ g) & e */                      \
    "andq	"#e", " L3 "\n\t"               \
    /* h += Sigma1(e) */                        \
    "addq	" L1 ", "#h"\n\t"               \
    /* L1 = a>>>28 */                           \
    "rorxq	$28, "#a", " L1 "\n\t"          \

#define RND_RORX_0_5(a, b, c, d, e, f, g, h, i) \
    /* L2 = a>>>34 */                           \
    "rorxq	$34, "#a", " L2 "\n\t"          \
    /* L3 = Ch(e,f,g) */                        \
    "xorq	"#g", " L3 "\n\t"               \
    /* L2 = (a>>>28) ^ (a>>>34) */              \
    "xorq	" L1 ", " L2 "\n\t"             \

#define RND_RORX_0_6(a, b, c, d, e, f, g, h, i) \
    /* L1 = a>>>39 */                           \
    "rorxq	$39, "#a", " L1 "\n\t"          \
    /* h += Ch(e,f,g) */                        \
    "addq	" L3 ", "#h"\n\t"               \
    /* L1 = Sigma0(a) */                        \
    "xorq	" L2 ", " L1 "\n\t"             \

#define RND_RORX_0_7(a, b, c, d, e, f, g, h, i) \
    /* L3 = b */                                \
    "movq	"#b", " L3 "\n\t"               \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */  \
    "addq	"#h", "#d"\n\t"                 \
    /* L3 = a ^ b */                            \
    "xorq	"#a", " L3 "\n\t"               \

#define RND_RORX_0_8(a, b, c, d, e, f, g, h, i) \
    /* L4 = (a ^ b) & (b ^ c) */                \
    "andq	" L3 ", " L4 "\n\t"             \
    /* h += Sigma0(a) */                        \
    "addq	" L1 ", "#h"\n\t"               \
    /* L4 = Maj(a,b,c) */                       \
    "xorq	"#b", " L4 "\n\t"               \

#define RND_RORX_1_1(a, b, c, d, e, f, g, h, i) \
    /* L1 = e>>>14 */                           \
    "rorxq	$14, "#e", " L1 "\n\t"          \
    /* L2 = e>>>18 */                           \
    "rorxq	$18, "#e", " L2 "\n\t"          \
    /* Prev RND: h += Maj(a,b,c) */             \
    "addq	" L4 ", "#a"\n\t"               \

#define RND_RORX_1_2(a, b, c, d, e, f, g, h, i) \
    /* h += w_k */                              \
    "addq	("#i")*8(" WX "), "#h"\n\t"     \
    /* L4 = f */                                \
    "movq	"#f", " L4 "\n\t"               \
    /* L2 = (e>>>14) ^ (e>>>18) */              \
    "xorq	" L1 ", " L2 "\n\t"             \

#define RND_RORX_1_3(a, b, c, d, e, f, g, h, i) \
    /* L4 = f ^ g */                            \
    "xorq	"#g", " L4 "\n\t"               \
    /* L1 = e>>>41 */                           \
    "rorxq	$41, "#e", " L1 "\n\t"          \
    /* L1 = Sigma1(e) */                        \
    "xorq	" L2 ", " L1 "\n\t"             \

#define RND_RORX_1_4(a, b, c, d, e, f, g, h, i) \
    /* L4 = (f ^ g) & e */                      \
    "andq	"#e", " L4 "\n\t"               \
    /* h += Sigma1(e) */                        \
    "addq	" L1 ", "#h"\n\t"               \
    /* L1 = a>>>28 */                           \
    "rorxq	$28, "#a", " L1 "\n\t"          \

#define RND_RORX_1_5(a, b, c, d, e, f, g, h, i) \
    /* L2 = a>>>34 */                           \
    "rorxq	$34, "#a", " L2 "\n\t"          \
    /* L4 = Ch(e,f,g) */                        \
    "xorq	"#g", " L4 "\n\t"               \
    /* L2 = (a>>>28) ^ (a>>>34) */              \
    "xorq	" L1 ", " L2 "\n\t"             \

#define RND_RORX_1_6(a, b, c, d, e, f, g, h, i) \
    /* L1 = a>>>39 */                           \
    "rorxq	$39, "#a", " L1 "\n\t"          \
    /* h += Ch(e,f,g) */                        \
    "addq	" L4 ", "#h"\n\t"               \
    /* L1 = Sigma0(a) */                        \
    "xorq	" L2 ", " L1 "\n\t"             \

#define RND_RORX_1_7(a, b, c, d, e, f, g, h, i) \
    /* L4 = b */                                \
    "movq	"#b", " L4 "\n\t"               \
    /* d += h + w_k + Sigma1(e) + Ch(e,f,g) */  \
    "addq	"#h", "#d"\n\t"                 \
    /* L4 = a ^ b */                            \
    "xorq	"#a", " L4 "\n\t"               \

#define RND_RORX_1_8(a, b, c, d, e, f, g, h, i) \
    /* L2 = (a ^ b) & (b ^ c) */                \
    "andq	" L4 ", " L3 "\n\t"             \
    /* h += Sigma0(a) */                        \
    "addq	" L1 ", "#h"\n\t"               \
    /* L3 = Maj(a,b,c) */                       \
    "xorq	"#b", " L3 "\n\t"               \

#define RND_RORX_ALL_2(a, b, c, d, e, f, g, h, i) \
    RND_RORX_0_1(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_2(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_3(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_4(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_5(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_6(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_7(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_0_8(a, b, c, d, e, f, g, h, i+0)     \
    RND_RORX_1_1(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_2(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_3(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_4(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_5(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_6(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_7(h, a, b, c, d, e, f, g, i+1)     \
    RND_RORX_1_8(h, a, b, c, d, e, f, g, i+1)     \

#define RND_RORX_ALL_4(a, b, c, d, e, f, g, h, i) \
    RND_RORX_ALL_2(a, b, c, d, e, f, g, h, i+0)   \
    RND_RORX_ALL_2(g, h, a, b, c, d, e, f, i+2)

#define MsgSched_RORX(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,a,b,c,d,e,f,g,h,i) \
            RND_RORX_0_1(a,b,c,d,e,f,g,h,i)                                 \
    VPALIGNR(W_M15, W_2, W_0, 8)                                            \
    VPALIGNR(W_M7, W_10, W_8, 8)                                            \
            RND_RORX_0_2(a,b,c,d,e,f,g,h,i)                                 \
    V_SHIFT_R(XTMP1, W_M15, 1)                                              \
    V_SHIFT_L(XTMP2, W_M15, 63)                                             \
            RND_RORX_0_3(a,b,c,d,e,f,g,h,i)                                 \
    V_SHIFT_R(XTMP3, W_M15, 8)                                              \
    V_SHIFT_L(XTMP4, W_M15, 56)                                             \
            RND_RORX_0_4(a,b,c,d,e,f,g,h,i)                                 \
    V_OR(XTMP1, XTMP2, XTMP1)                                               \
    V_OR(XTMP3, XTMP4, XTMP3)                                               \
            RND_RORX_0_5(a,b,c,d,e,f,g,h,i)                                 \
    V_SHIFT_R(XTMP4, W_M15, 7)                                              \
    V_XOR(XTMP1, XTMP3, XTMP1)                                              \
            RND_RORX_0_6(a,b,c,d,e,f,g,h,i)                                 \
    V_XOR(XTMP1, XTMP4, XTMP1)                                              \
    V_ADD(W_0, W_0, W_M7)                                                   \
            RND_RORX_0_7(a,b,c,d,e,f,g,h,i)                                 \
            RND_RORX_0_8(a,b,c,d,e,f,g,h,i)                                 \
    V_ADD(W_0, W_0, XTMP1)                                                  \
            RND_RORX_1_1(h,a,b,c,d,e,f,g,i+1)                               \
    V_SHIFT_R(XTMP1, W_14, 19)                                              \
    V_SHIFT_L(XTMP2, W_14, 45)                                              \
            RND_RORX_1_2(h,a,b,c,d,e,f,g,i+1)                               \
    V_SHIFT_R(XTMP3, W_14, 61)                                              \
    V_SHIFT_L(XTMP4, W_14, 3)                                               \
            RND_RORX_1_3(h,a,b,c,d,e,f,g,i+1)                               \
    V_OR(XTMP1, XTMP2, XTMP1)                                               \
    V_OR(XTMP3, XTMP4, XTMP3)                                               \
            RND_RORX_1_4(h,a,b,c,d,e,f,g,i+1)                               \
            RND_RORX_1_5(h,a,b,c,d,e,f,g,i+1)                               \
    V_XOR(XTMP1, XTMP3, XTMP1)                                              \
    V_SHIFT_R(XTMP4, W_14, 6)                                               \
            RND_RORX_1_6(h,a,b,c,d,e,f,g,i+1)                               \
            RND_RORX_1_7(h,a,b,c,d,e,f,g,i+1)                               \
    V_XOR(XTMP1, XTMP4, XTMP1)                                              \
            RND_RORX_1_8(h,a,b,c,d,e,f,g,i+1)                               \
    V_ADD(W_0, W_0, XTMP1)                                                  \

#endif

#define _INIT_MASK(mask) \
    "vmovdqu %[mask], %%" #mask "\n\t"
#define INIT_MASK(mask) \
       _INIT_MASK(mask)

#define _LOAD_W_2(i1, i2, xmm1, xmm2, mask, reg)           \
    "vmovdqu	" #i1 "*16(%%" #reg "), %%" #xmm1 "\n\t"   \
    "vmovdqu	" #i2 "*16(%%" #reg "), %%" #xmm2 "\n\t"   \
    "vpshufb	%%" #mask ", %%" #xmm1 ", %%" #xmm1 "\n\t" \
    "vpshufb	%%" #mask ", %%" #xmm2 ", %%" #xmm2 "\n\t"
#define LOAD_W_2(i1, i2, xmm1, xmm2, mask, reg) \
       _LOAD_W_2(i1, i2, xmm1, xmm2, mask, reg)

#define LOAD_W(mask, reg)                           \
    /* X0..3(xmm4..7), W[0..15] = buffer[0.15];  */ \
    LOAD_W_2(0, 1, W_0 , W_2 , mask, reg)           \
    LOAD_W_2(2, 3, W_4 , W_6 , mask, reg)           \
    LOAD_W_2(4, 5, W_8 , W_10, mask, reg)           \
    LOAD_W_2(6, 7, W_12, W_14, mask, reg)

#define _SET_W_X_2(xmm0, xmm1, reg, i)                          \
    "vpaddq	" #i "+ 0(%%" #reg "), %%" #xmm0 ", %%xmm8\n\t" \
    "vpaddq	" #i "+16(%%" #reg "), %%" #xmm1 ", %%xmm9\n\t" \
    "vmovdqu	%%xmm8, " #i "+ 0(" WX ")\n\t"                  \
    "vmovdqu	%%xmm9, " #i "+16(" WX ")\n\t"                  \

#define SET_W_X_2(xmm0, xmm1, reg, i) \
       _SET_W_X_2(xmm0, xmm1, reg, i)

#define SET_W_X(reg)                \
    SET_W_X_2(W_0 , W_2 , reg,  0)  \
    SET_W_X_2(W_4 , W_6 , reg, 32)  \
    SET_W_X_2(W_8 , W_10, reg, 64)  \
    SET_W_X_2(W_12, W_14, reg, 96)

#define LOAD_DIGEST()                     \
    "movq	  (%[sha512]), %%r8 \n\t" \
    "movq	 8(%[sha512]), %%r9 \n\t" \
    "movq	16(%[sha512]), %%r10\n\t" \
    "movq	24(%[sha512]), %%r11\n\t" \
    "movq	32(%[sha512]), %%r12\n\t" \
    "movq	40(%[sha512]), %%r13\n\t" \
    "movq	48(%[sha512]), %%r14\n\t" \
    "movq	56(%[sha512]), %%r15\n\t"

#define STORE_ADD_DIGEST()                \
    "addq	 %%r8,   (%[sha512])\n\t" \
    "addq	 %%r9,  8(%[sha512])\n\t" \
    "addq	%%r10, 16(%[sha512])\n\t" \
    "addq	%%r11, 24(%[sha512])\n\t" \
    "addq	%%r12, 32(%[sha512])\n\t" \
    "addq	%%r13, 40(%[sha512])\n\t" \
    "addq	%%r14, 48(%[sha512])\n\t" \
    "addq	%%r15, 56(%[sha512])\n\t"

#define ADD_DIGEST()                      \
    "addq	  (%[sha512]), %%r8 \n\t" \
    "addq	 8(%[sha512]), %%r9 \n\t" \
    "addq	16(%[sha512]), %%r10\n\t" \
    "addq	24(%[sha512]), %%r11\n\t" \
    "addq	32(%[sha512]), %%r12\n\t" \
    "addq	40(%[sha512]), %%r13\n\t" \
    "addq	48(%[sha512]), %%r14\n\t" \
    "addq	56(%[sha512]), %%r15\n\t"

#define STORE_DIGEST()                    \
    "movq	 %%r8,   (%[sha512])\n\t" \
    "movq	 %%r9,  8(%[sha512])\n\t" \
    "movq	%%r10, 16(%[sha512])\n\t" \
    "movq	%%r11, 24(%[sha512])\n\t" \
    "movq	%%r12, 32(%[sha512])\n\t" \
    "movq	%%r13, 40(%[sha512])\n\t" \
    "movq	%%r14, 48(%[sha512])\n\t" \
    "movq	%%r15, 56(%[sha512])\n\t"

#endif /* HAVE_INTEL_AVX1 */


/***  Transform Body ***/
#if defined(HAVE_INTEL_AVX1)
static int Transform_Sha512_AVX1(wc_Sha512* sha512)
{
    __asm__ __volatile__ (

        /* 16 Ws plus loop counter. */
        "subq	$136, %%rsp\n\t"
        "leaq	64(%[sha512]), %%rax\n\t"

    INIT_MASK(MASK)
    LOAD_DIGEST()

    LOAD_W(MASK, rax)

        "movl	$4, 16*8(" WX ")\n\t"
        "leaq	%[K512], %%rsi\n\t"
        /* b */
        "movq	%%r9, " L4 "\n\t"
        /* e */
        "movq	%%r12, " L1 "\n\t"
        /* b ^ c */
        "xorq	%%r10, " L4 "\n\t"

        "# Start of 16 rounds\n"
        "1:\n\t"

    SET_W_X(rsi)

        "addq	$128, %%rsi\n\t"

    MsgSched2(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched2(W_2,W_4,W_6,W_8,W_10,W_12,W_14,W_0,RG,RH,RA,RB,RC,RD,RE,RF, 2)
    MsgSched2(W_4,W_6,W_8,W_10,W_12,W_14,W_0,W_2,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched2(W_6,W_8,W_10,W_12,W_14,W_0,W_2,W_4,RC,RD,RE,RF,RG,RH,RA,RB, 6)
    MsgSched2(W_8,W_10,W_12,W_14,W_0,W_2,W_4,W_6,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched2(W_10,W_12,W_14,W_0,W_2,W_4,W_6,W_8,RG,RH,RA,RB,RC,RD,RE,RF,10)
    MsgSched2(W_12,W_14,W_0,W_2,W_4,W_6,W_8,W_10,RE,RF,RG,RH,RA,RB,RC,RD,12)
    MsgSched2(W_14,W_0,W_2,W_4,W_6,W_8,W_10,W_12,RC,RD,RE,RF,RG,RH,RA,RB,14)

        "subl	$1, 16*8(" WX ")\n\t"
        "jne	1b\n\t"

    SET_W_X(rsi)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 2)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB, 6)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,10)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,12)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

    STORE_ADD_DIGEST()

        "addq	$136, %%rsp\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory", "rsi"
    );

    return 0;
}

static int Transform_Sha512_AVX1_Len(wc_Sha512* sha512, word32 len)
{
    __asm__ __volatile__ (

        "movq	224(%[sha512]), %%rsi\n\t"
        "leaq	%[K512], %%rdx\n\t"

    INIT_MASK(MASK)
    LOAD_DIGEST()

        "# Start of processing a block\n"
        "2:\n\t"

        /* 16 Ws plus loop counter and K512. len goes into -4(%rsp).
         * Debug needs more stack space. */
        "subq	$256, %%rsp\n\t"

    LOAD_W(MASK, rsi)

        "movl	$4, 16*8(" WX ")\n\t"
        /* b */
        "movq	%%r9, " L4 "\n\t"
        /* e */
        "movq	%%r12, " L1 "\n\t"
        /* b ^ c */
        "xorq	%%r10, " L4 "\n\t"

    SET_W_X(rdx)

        "# Start of 16 rounds\n"
        "1:\n\t"

        "addq	$128, %%rdx\n\t"
        "movq	%%rdx, 17*8(%%rsp)\n\t"

    MsgSched2(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched2(W_2,W_4,W_6,W_8,W_10,W_12,W_14,W_0,RG,RH,RA,RB,RC,RD,RE,RF, 2)
    MsgSched2(W_4,W_6,W_8,W_10,W_12,W_14,W_0,W_2,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched2(W_6,W_8,W_10,W_12,W_14,W_0,W_2,W_4,RC,RD,RE,RF,RG,RH,RA,RB, 6)
    MsgSched2(W_8,W_10,W_12,W_14,W_0,W_2,W_4,W_6,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched2(W_10,W_12,W_14,W_0,W_2,W_4,W_6,W_8,RG,RH,RA,RB,RC,RD,RE,RF,10)
    MsgSched2(W_12,W_14,W_0,W_2,W_4,W_6,W_8,W_10,RE,RF,RG,RH,RA,RB,RC,RD,12)
    MsgSched2(W_14,W_0,W_2,W_4,W_6,W_8,W_10,W_12,RC,RD,RE,RF,RG,RH,RA,RB,14)

        "movq	17*8(%%rsp), %%rdx\n\t"

    SET_W_X(rdx)

        "subl	$1, 16*8(" WX ")\n\t"
        "jne	1b\n\t"

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 2)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB, 6)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,10)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,12)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

    ADD_DIGEST()

        "addq	$256, %%rsp\n\t"
        "leaq	%[K512], %%rdx\n\t"
        "addq	$128, %%rsi\n\t"
        "subl	$128, %[len]\n\t"

    STORE_DIGEST()

        "jnz	2b\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK),
          [len]    "m" (len),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory", "rsi"
    );

    return 0;
}
#endif /* HAVE_INTEL_AVX1 */

#if defined(HAVE_INTEL_AVX2) && defined(HAVE_INTEL_RORX)
static int Transform_Sha512_AVX1_RORX(wc_Sha512* sha512)
{
    __asm__ __volatile__ (

        /* 16 Ws plus loop counter and K512. */
        "subq	$144, %%rsp\n\t"
        "leaq	64(%[sha512]), %%rax\n\t"

    INIT_MASK(MASK)
    LOAD_DIGEST()

    LOAD_W(MASK, rax)

        "movl	$4, 16*8(" WX ")\n\t"
        "leaq	%[K512], %%rsi\n\t"
        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* L3 = 0 (add to prev h) */
        "xorq	" L3 ", " L3 "\n\t"
        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"

    SET_W_X(rsi)

        "# Start of 16 rounds\n"
        "1:\n\t"

        "addq	$128, %%rsi\n\t"

    MsgSched_RORX(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched_RORX(W_2,W_4,W_6,W_8,W_10,W_12,W_14,W_0,RG,RH,RA,RB,RC,RD,RE,RF, 2)
    MsgSched_RORX(W_4,W_6,W_8,W_10,W_12,W_14,W_0,W_2,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched_RORX(W_6,W_8,W_10,W_12,W_14,W_0,W_2,W_4,RC,RD,RE,RF,RG,RH,RA,RB, 6)
    MsgSched_RORX(W_8,W_10,W_12,W_14,W_0,W_2,W_4,W_6,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched_RORX(W_10,W_12,W_14,W_0,W_2,W_4,W_6,W_8,RG,RH,RA,RB,RC,RD,RE,RF,10)
    MsgSched_RORX(W_12,W_14,W_0,W_2,W_4,W_6,W_8,W_10,RE,RF,RG,RH,RA,RB,RC,RD,12)
    MsgSched_RORX(W_14,W_0,W_2,W_4,W_6,W_8,W_10,W_12,RC,RD,RE,RF,RG,RH,RA,RB,14)

    SET_W_X(rsi)

        "subl	$1, 16*8(" WX ")\n\t"
        "jne	1b\n\t"

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 2)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB, 6)

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,10)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,12)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

        /* Prev RND: h += Maj(a,b,c) */
        "addq	" L3 ", %%r8\n\t"
        "addq	$144, %%rsp\n\t"

    STORE_ADD_DIGEST()

        :
        : [mask]   "m" (mBYTE_FLIP_MASK),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory", "rsi"
    );

    return 0;
}

static int Transform_Sha512_AVX1_RORX_Len(wc_Sha512* sha512, word32 len)
{
    __asm__ __volatile__ (

        "movq	224(%[sha512]), %%rsi\n\t"
        "leaq	%[K512], %%rcx\n\t"

    INIT_MASK(MASK)
    LOAD_DIGEST()

        "# Start of processing a block\n"
        "2:\n\t"

        /* 16 Ws plus loop counter and K512. len goes into -4(%rsp).
         * Debug needs more stack space. */
        "subq	$256, %%rsp\n\t"

    LOAD_W(MASK, rsi)

        "movl	$4, 16*8(" WX ")\n\t"
        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* L3 = 0 (add to prev h) */
        "xorq	" L3 ", " L3 "\n\t"
        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"

    SET_W_X(rcx)

        "# Start of 16 rounds\n"
        "1:\n\t"

        "addq	$128, %%rcx\n\t"
        "movq	%%rcx, 17*8(%%rsp)\n\t"

    MsgSched_RORX(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched_RORX(W_2,W_4,W_6,W_8,W_10,W_12,W_14,W_0,RG,RH,RA,RB,RC,RD,RE,RF, 2)
    MsgSched_RORX(W_4,W_6,W_8,W_10,W_12,W_14,W_0,W_2,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched_RORX(W_6,W_8,W_10,W_12,W_14,W_0,W_2,W_4,RC,RD,RE,RF,RG,RH,RA,RB, 6)
    MsgSched_RORX(W_8,W_10,W_12,W_14,W_0,W_2,W_4,W_6,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched_RORX(W_10,W_12,W_14,W_0,W_2,W_4,W_6,W_8,RG,RH,RA,RB,RC,RD,RE,RF,10)
    MsgSched_RORX(W_12,W_14,W_0,W_2,W_4,W_6,W_8,W_10,RE,RF,RG,RH,RA,RB,RC,RD,12)
    MsgSched_RORX(W_14,W_0,W_2,W_4,W_6,W_8,W_10,W_12,RC,RD,RE,RF,RG,RH,RA,RB,14)

        "movq	17*8(%%rsp), %%rcx\n\t"

    SET_W_X(rcx)

        "subl	$1, 16*8(" WX ")\n\t"
        "jne	1b\n\t"

    SET_W_X(rcx)

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 2)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB, 6)

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,10)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,12)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

        /* Prev RND: h += Maj(a,b,c) */
        "addq	" L3 ", %%r8\n\t"
        "addq	$256, %%rsp\n\t"

    ADD_DIGEST()

        "leaq	%[K512], %%rcx\n\t"
        "addq	$128, %%rsi\n\t"
        "subl	$128, %[len]\n\t"

    STORE_DIGEST()

        "jnz	2b\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK),
          [len]    "m" (len),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, XMM_REGS, "memory", "rsi"
    );

    return 0;
}
#endif /* HAVE_INTEL_AVX2 && HAVE_INTEL_RORX */

#if defined(HAVE_INTEL_AVX2)
static const unsigned long mBYTE_FLIP_MASK_Y[] =
   { 0x0001020304050607, 0x08090a0b0c0d0e0f,
     0x0001020304050607, 0x08090a0b0c0d0e0f };

#define W_Y_0       ymm0
#define W_Y_4       ymm1
#define W_Y_8       ymm2
#define W_Y_12      ymm3

#define X0       xmm0
#define X1       xmm1
#define X2       xmm2
#define X3       xmm3
#define X4       xmm4
#define X5       xmm5
#define X6       xmm6
#define X7       xmm7
#define X8       xmm8
#define X9       xmm9
#define Y0       ymm0
#define Y1       ymm1
#define Y2       ymm2
#define Y3       ymm3
#define Y4       ymm4
#define Y5       ymm5
#define Y6       ymm6
#define Y7       ymm7

#define W_Y_M15     ymm12
#define W_Y_M7      ymm13
#define W_Y_M2      ymm14
#define MASK_Y      ymm15

#define YTMP1       ymm8
#define YTMP2       ymm9
#define YTMP3       ymm10
#define YTMP4       ymm11

#define YMM_REGS \
    "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7",       \
    "xmm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15"

#define _VPERM2I128(dest, src1, src2, sel)                             \
    "vperm2I128	$" #sel ", %%" #src2 ", %%" #src1 ", %%" #dest "\n\t"
#define VPERM2I128(dest, src1, src2, sel) \
       _VPERM2I128(dest, src1, src2, sel)

#define _VPERMQ(dest, src, sel)                                        \
    "vpermq	$" #sel ", %%" #src ", %%" #dest "\n\t"
#define VPERMQ(dest, src, sel) \
       _VPERMQ(dest, src, sel)

#define _VPBLENDD(dest, src1, src2, sel)                               \
    "vpblendd	$" #sel ", %%" #src2 ", %%" #src1 ", %%" #dest "\n\t"
#define VPBLENDD(dest, src1, src2, sel) \
       _VPBLENDD(dest, src1, src2, sel)

#define _V_ADD_I(dest, src1, addr, i)                                  \
    "vpaddq	 "#i"*8(%%" #addr "), %%" #src1 ", %%" #dest "\n\t"
#define V_ADD_I(dest, src1, addr, i) \
       _V_ADD_I(dest, src1, addr, i)

#define _VMOVDQU_I(addr, i, src)                                       \
    "vmovdqu	 %%" #src ", " #i "*8(%%" #addr ")\n\t"
#define VMOVDQU_I(addr, i, src) \
       _VMOVDQU_I(addr, i, src)

#define MsgSched4_AVX2(W_Y_0,W_Y_4,W_Y_8,W_Y_12,a,b,c,d,e,f,g,h,i) \
            RND_0_1(a,b,c,d,e,f,g,h,i)                             \
    /* W[-13]..W[-15], W[-12] */                                   \
    VPBLENDD(W_Y_M15, W_Y_0, W_Y_4, 0x03)                          \
    /* W[-5]..W[-7], W[-4] */                                      \
    VPBLENDD(W_Y_M7, W_Y_8, W_Y_12, 0x03)                          \
            RND_0_2(a,b,c,d,e,f,g,h,i)                             \
            RND_0_3(a,b,c,d,e,f,g,h,i)                             \
    /* W_Y_M15 = W[-12]..W[-15] */                                 \
    VPERMQ(W_Y_M15, W_Y_M15, 0x39)                                 \
            RND_0_4(a,b,c,d,e,f,g,h,i)                             \
    /* W_Y_M7 = W[-4]..W[-7] */                                    \
    VPERMQ(W_Y_M7, W_Y_M7, 0x39)                                   \
            RND_0_5(a,b,c,d,e,f,g,h,i)                             \
            RND_0_6(a,b,c,d,e,f,g,h,i)                             \
    /* W[-15] >>  1 */                                             \
    V_SHIFT_R(YTMP1, W_Y_M15, 1)                                   \
            RND_0_7(a,b,c,d,e,f,g,h,i)                             \
    /* W[-15] << 63 */                                             \
    V_SHIFT_L(YTMP2, W_Y_M15, 63)                                  \
            RND_0_8(a,b,c,d,e,f,g,h,i)                             \
    /* W[-15] >>  8 */                                             \
    V_SHIFT_R(YTMP3, W_Y_M15, 8)                                   \
            RND_0_9(a,b,c,d,e,f,g,h,i)                             \
    /* W[-15] << 56 */                                             \
    V_SHIFT_L(YTMP4, W_Y_M15, 56)                                  \
            RND_0_10(a,b,c,d,e,f,g,h,i)                            \
    /* W[-15] >>> 1 */                                             \
    V_OR(YTMP1, YTMP2, YTMP1)                                      \
            RND_0_11(a,b,c,d,e,f,g,h,i)                            \
    /* W[-15] >>> 8 */                                             \
    V_OR(YTMP3, YTMP4, YTMP3)                                      \
            RND_0_12(a,b,c,d,e,f,g,h,i)                            \
            RND_1_1(h,a,b,c,d,e,f,g,i+1)                           \
    /* W[-15] >> 7 */                                              \
    V_SHIFT_R(YTMP4, W_Y_M15, 7)                                   \
            RND_1_2_A(h,a,b,c,d,e,f,g,i+1)                         \
    /* (W[-15] >>> 1) ^ (W[-15] >>> 8) */                          \
    V_XOR(YTMP1, YTMP3, YTMP1)                                     \
            RND_1_2_B(h,a,b,c,d,e,f,g,i+1)                         \
    /* (W[-15] >>> 1) ^ (W[-15] >>> 8) ^ (W[-15] >> 7) */          \
    V_XOR(YTMP1, YTMP4, YTMP1)                                     \
            RND_1_3(h,a,b,c,d,e,f,g,i+1)                           \
    /* W[0] = W[-16] + W[-7] */                                    \
    V_ADD(W_Y_0, W_Y_0, W_Y_M7)                                    \
            RND_1_4(h,a,b,c,d,e,f,g,i+1)                           \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) */                       \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                     \
            RND_1_5(h,a,b,c,d,e,f,g,i+1)                           \
    /* 0, 0, W[-1], W[-2] */                                       \
    VPERM2I128(W_Y_M2, W_Y_12, W_Y_12, 0x81)                       \
            RND_1_6(h,a,b,c,d,e,f,g,i+1)                           \
            RND_1_7(h,a,b,c,d,e,f,g,i+1)                           \
            RND_1_8(h,a,b,c,d,e,f,g,i+1)                           \
    /* W[-2] >> 19 */                                              \
    V_SHIFT_R(YTMP1, W_Y_M2, 19)                                   \
            RND_1_9(h,a,b,c,d,e,f,g,i+1)                           \
    /* W[-2] << 45 */                                              \
    V_SHIFT_L(YTMP2, W_Y_M2, 45)                                   \
            RND_1_10(h,a,b,c,d,e,f,g,i+1)                          \
    /* W[-2] >> 61 */                                              \
    V_SHIFT_R(YTMP3, W_Y_M2, 61)                                   \
            RND_1_11(h,a,b,c,d,e,f,g,i+1)                          \
    /* W[-2] <<  3 */                                              \
    V_SHIFT_L(YTMP4, W_Y_M2, 3)                                    \
            RND_1_12(h,a,b,c,d,e,f,g,i+1)                          \
            RND_0_1(g,h,a,b,c,d,e,f,i+2)                           \
    /* W[-2] >>> 19 */                                             \
    V_OR(YTMP1, YTMP2, YTMP1)                                      \
            RND_0_2(g,h,a,b,c,d,e,f,i+2)                           \
    /* W[-2] >>> 61 */                                             \
    V_OR(YTMP3, YTMP4, YTMP3)                                      \
            RND_0_3(g,h,a,b,c,d,e,f,i+2)                           \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) */                          \
    V_XOR(YTMP1, YTMP3, YTMP1)                                     \
            RND_0_4(g,h,a,b,c,d,e,f,i+2)                           \
    /* W[-2] >>  6 */                                              \
    V_SHIFT_R(YTMP4, W_Y_M2, 6)                                    \
            RND_0_5(g,h,a,b,c,d,e,f,i+2)                           \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) ^ (W[-2] >> 6) */           \
    V_XOR(YTMP1, YTMP4, YTMP1)                                     \
            RND_0_6(g,h,a,b,c,d,e,f,i+2)                           \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) + s1(W[-2]) */           \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                     \
            RND_0_7(g,h,a,b,c,d,e,f,i+2)                           \
            RND_0_8(g,h,a,b,c,d,e,f,i+2)                           \
    /* W[1], W[0], 0, 0 */                                         \
    VPERM2I128(W_Y_M2, W_Y_0, W_Y_0, 0x08)                         \
            RND_0_9(g,h,a,b,c,d,e,f,i+2)                           \
            RND_0_10(g,h,a,b,c,d,e,f,i+2)                          \
    /* W[-2] >> 19 */                                              \
    V_SHIFT_R(YTMP1, W_Y_M2, 19)                                   \
            RND_0_11(g,h,a,b,c,d,e,f,i+2)                          \
    /* W[-2] << 45 */                                              \
    V_SHIFT_L(YTMP2, W_Y_M2, 45)                                   \
            RND_0_12(g,h,a,b,c,d,e,f,i+2)                          \
            RND_1_1(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[-2] >> 61 */                                              \
    V_SHIFT_R(YTMP3, W_Y_M2, 61)                                   \
            RND_1_2(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[-2] <<  3 */                                              \
    V_SHIFT_L(YTMP4, W_Y_M2, 3)                                    \
            RND_1_3(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[-2] >>> 19 */                                             \
    V_OR(YTMP1, YTMP2, YTMP1)                                      \
            RND_1_4(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[-2] >>> 61 */                                             \
    V_OR(YTMP3, YTMP4, YTMP3)                                      \
            RND_1_5(f,g,h,a,b,c,d,e,i+3)                           \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) */                          \
    V_XOR(YTMP1, YTMP3, YTMP1)                                     \
            RND_1_6(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[-2] >>  6 */                                              \
    V_SHIFT_R(YTMP4, W_Y_M2, 6)                                    \
            RND_1_7(f,g,h,a,b,c,d,e,i+3)                           \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) ^ (W[-2] >> 6) */           \
    V_XOR(YTMP1, YTMP4, YTMP1)                                     \
            RND_1_8(f,g,h,a,b,c,d,e,i+3)                           \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) + s1(W[-2]) */           \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                     \
            RND_1_9(f,g,h,a,b,c,d,e,i+3)                           \
            RND_1_10(f,g,h,a,b,c,d,e,i+3)                          \
            RND_1_11(f,g,h,a,b,c,d,e,i+3)                          \
            RND_1_12(f,g,h,a,b,c,d,e,i+3)                          \

#define MsgSched2_AVX2(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,a,b,c,d,e,f,g,h,i) \
            RND_0_1(a,b,c,d,e,f,g,h,i)                                       \
    VPALIGNR(W_Y_M15, W_2, W_0, 8)                                           \
    VPALIGNR(W_Y_M7, W_10, W_8, 8)                                           \
            RND_0_2(a,b,c,d,e,f,g,h,i)                                       \
    V_SHIFT_R(YTMP1, W_Y_M15, 1)                                             \
    V_SHIFT_L(YTMP2, W_Y_M15, 63)                                            \
            RND_0_3(a,b,c,d,e,f,g,h,i)                                       \
            RND_0_4(a,b,c,d,e,f,g,h,i)                                       \
    V_SHIFT_R(YTMP3, W_Y_M15, 8)                                             \
    V_SHIFT_L(YTMP4, W_Y_M15, 56)                                            \
            RND_0_5(a,b,c,d,e,f,g,h,i)                                       \
            RND_0_6(a,b,c,d,e,f,g,h,i)                                       \
    V_OR(YTMP1, YTMP2, YTMP1)                                                \
    V_OR(YTMP3, YTMP4, YTMP3)                                                \
            RND_0_7(a,b,c,d,e,f,g,h,i)                                       \
            RND_0_8(a,b,c,d,e,f,g,h,i)                                       \
    V_SHIFT_R(YTMP4, W_Y_M15, 7)                                             \
    V_XOR(YTMP1, YTMP3, YTMP1)                                               \
            RND_0_9(a,b,c,d,e,f,g,h,i)                                       \
            RND_0_10(a,b,c,d,e,f,g,h,i)                                      \
    V_XOR(YTMP1, YTMP4, YTMP1)                                               \
    V_ADD(W_0, W_0, W_Y_M7)                                                  \
            RND_0_11(a,b,c,d,e,f,g,h,i)                                      \
            RND_0_12(a,b,c,d,e,f,g,h,i)                                      \
            RND_1_1(h,a,b,c,d,e,f,g,i+1)                                     \
    V_ADD(W_0, W_0, YTMP1)                                                   \
            RND_1_2(h,a,b,c,d,e,f,g,i+1)                                     \
    V_SHIFT_R(YTMP1, W_14, 19)                                               \
    V_SHIFT_L(YTMP2, W_14, 45)                                               \
            RND_1_3(h,a,b,c,d,e,f,g,i+1)                                     \
            RND_1_4(h,a,b,c,d,e,f,g,i+1)                                     \
    V_SHIFT_R(YTMP3, W_14, 61)                                               \
    V_SHIFT_L(YTMP4, W_14, 3)                                                \
            RND_1_5(h,a,b,c,d,e,f,g,i+1)                                     \
            RND_1_6(h,a,b,c,d,e,f,g,i+1)                                     \
            RND_1_7(h,a,b,c,d,e,f,g,i+1)                                     \
    V_OR(YTMP1, YTMP2, YTMP1)                                                \
    V_OR(YTMP3, YTMP4, YTMP3)                                                \
            RND_1_8(h,a,b,c,d,e,f,g,i+1)                                     \
            RND_1_9(h,a,b,c,d,e,f,g,i+1)                                     \
    V_XOR(YTMP1, YTMP3, YTMP1)                                               \
    V_SHIFT_R(YTMP4, W_14, 6)                                                \
            RND_1_10(h,a,b,c,d,e,f,g,i+1)                                    \
            RND_1_11(h,a,b,c,d,e,f,g,i+1)                                    \
    V_XOR(YTMP1, YTMP4, YTMP1)                                               \
            RND_1_12(h,a,b,c,d,e,f,g,i+1)                                    \
    V_ADD(W_0, W_0, YTMP1)                                                   \

#define MsgSched4_AVX2_RORX_SET(W_Y_0,W_Y_4,W_Y_8,W_Y_12,a,b,c,d,e,f,g,h,i) \
            RND_RORX_0_1(a,b,c,d,e,f,g,h,i)                                 \
    /* W[-13]..W[-15], W[-12] */                                            \
    VPBLENDD(W_Y_M15, W_Y_0, W_Y_4, 0x03)                                   \
    /* W[-5]..W[-7], W[-4] */                                               \
    VPBLENDD(W_Y_M7, W_Y_8, W_Y_12, 0x03)                                   \
            RND_RORX_0_2(a,b,c,d,e,f,g,h,i)                                 \
    /* W_Y_M15 = W[-12]..W[-15] */                                          \
    VPERMQ(W_Y_M15, W_Y_M15, 0x39)                                          \
            RND_RORX_0_3(a,b,c,d,e,f,g,h,i)                                 \
    /* W_Y_M7 = W[-4]..W[-7] */                                             \
    VPERMQ(W_Y_M7, W_Y_M7, 0x39)                                            \
            RND_RORX_0_4(a,b,c,d,e,f,g,h,i)                                 \
    /* W[-15] >>  1 */                                                      \
    V_SHIFT_R(YTMP1, W_Y_M15, 1)                                            \
    /* W[-15] << 63 */                                                      \
    V_SHIFT_L(YTMP2, W_Y_M15, 63)                                           \
            RND_RORX_0_5(a,b,c,d,e,f,g,h,i)                                 \
    /* W[-15] >>  8 */                                                      \
    V_SHIFT_R(YTMP3, W_Y_M15, 8)                                            \
    /* W[-15] << 56 */                                                      \
    V_SHIFT_L(YTMP4, W_Y_M15, 56)                                           \
    /* W[-15] >>> 1 */                                                      \
    V_OR(YTMP1, YTMP2, YTMP1)                                               \
    /* W[-15] >>> 8 */                                                      \
    V_OR(YTMP3, YTMP4, YTMP3)                                               \
            RND_RORX_0_6(a,b,c,d,e,f,g,h,i)                                 \
    /* W[-15] >> 7 */                                                       \
    V_SHIFT_R(YTMP4, W_Y_M15, 7)                                            \
            RND_RORX_0_7(a,b,c,d,e,f,g,h,i)                                 \
    /* 0, 0, W[-1], W[-2] */                                                \
    VPERM2I128(W_Y_M2, W_Y_12, W_Y_12, 0x81)                                \
            RND_RORX_0_8(a,b,c,d,e,f,g,h,i)                                 \
            RND_RORX_1_1(h,a,b,c,d,e,f,g,i+1)                               \
    /* (W[-15] >>> 1) ^ (W[-15] >>> 8) */                                   \
    V_XOR(YTMP1, YTMP3, YTMP1)                                              \
            RND_RORX_1_2(h,a,b,c,d,e,f,g,i+1)                               \
    /* (W[-15] >>> 1) ^ (W[-15] >>> 8) ^ (W[-15] >> 7) */                   \
    V_XOR(YTMP1, YTMP4, YTMP1)                                              \
            RND_RORX_1_3(h,a,b,c,d,e,f,g,i+1)                               \
    /* W[0] = W[-16] + W[-7] */                                             \
    V_ADD(W_Y_0, W_Y_0, W_Y_M7)                                             \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) */                                \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                              \
            RND_RORX_1_4(h,a,b,c,d,e,f,g,i+1)                               \
    /* W[-2] >> 19 */                                                       \
    V_SHIFT_R(YTMP1, W_Y_M2, 19)                                            \
    /* W[-2] << 45 */                                                       \
    V_SHIFT_L(YTMP2, W_Y_M2, 45)                                            \
            RND_RORX_1_5(h,a,b,c,d,e,f,g,i+1)                               \
    /* W[-2] >> 61 */                                                       \
    V_SHIFT_R(YTMP3, W_Y_M2, 61)                                            \
    /* W[-2] <<  3 */                                                       \
    V_SHIFT_L(YTMP4, W_Y_M2, 3)                                             \
    /* W[-2] >>> 19 */                                                      \
    V_OR(YTMP1, YTMP2, YTMP1)                                               \
            RND_RORX_1_6(h,a,b,c,d,e,f,g,i+1)                               \
    /* W[-2] >>> 61 */                                                      \
    V_OR(YTMP3, YTMP4, YTMP3)                                               \
            RND_RORX_1_7(h,a,b,c,d,e,f,g,i+1)                               \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) */                                   \
    V_XOR(YTMP1, YTMP3, YTMP1)                                              \
            RND_RORX_1_8(h,a,b,c,d,e,f,g,i+1)                               \
    /* W[-2] >>  6 */                                                       \
    V_SHIFT_R(YTMP4, W_Y_M2, 6)                                             \
            RND_RORX_0_1(g,h,a,b,c,d,e,f,i+2)                               \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) ^ (W[-2] >> 6) */                    \
    V_XOR(YTMP1, YTMP4, YTMP1)                                              \
            RND_RORX_0_2(g,h,a,b,c,d,e,f,i+2)                               \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) + s1(W[-2]) */                    \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                              \
            RND_RORX_0_3(g,h,a,b,c,d,e,f,i+2)                               \
    /* W[1], W[0], 0, 0 */                                                  \
    VPERM2I128(W_Y_M2, W_Y_0, W_Y_0, 0x08)                                  \
            RND_RORX_0_4(g,h,a,b,c,d,e,f,i+2)                               \
            RND_RORX_0_5(g,h,a,b,c,d,e,f,i+2)                               \
    /* W[-2] >> 19 */                                                       \
    V_SHIFT_R(YTMP1, W_Y_M2, 19)                                            \
    /* W[-2] << 45 */                                                       \
    V_SHIFT_L(YTMP2, W_Y_M2, 45)                                            \
            RND_RORX_0_6(g,h,a,b,c,d,e,f,i+2)                               \
    /* W[-2] >> 61 */                                                       \
    V_SHIFT_R(YTMP3, W_Y_M2, 61)                                            \
    /* W[-2] <<  3 */                                                       \
    V_SHIFT_L(YTMP4, W_Y_M2, 3)                                             \
    /* W[-2] >>> 19 */                                                      \
    V_OR(YTMP1, YTMP2, YTMP1)                                               \
            RND_RORX_0_7(g,h,a,b,c,d,e,f,i+2)                               \
    /* W[-2] >>> 61 */                                                      \
    V_OR(YTMP3, YTMP4, YTMP3)                                               \
            RND_RORX_0_8(g,h,a,b,c,d,e,f,i+2)                               \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) */                                   \
    V_XOR(YTMP1, YTMP3, YTMP1)                                              \
            RND_RORX_1_1(f,g,h,a,b,c,d,e,i+3)                               \
    /* W[-2] >>  6 */                                                       \
    V_SHIFT_R(YTMP4, W_Y_M2, 6)                                             \
            RND_RORX_1_2(f,g,h,a,b,c,d,e,i+3)                               \
            RND_RORX_1_3(f,g,h,a,b,c,d,e,i+3)                               \
    /* (W[-2] >>> 19) ^ (W[-2] >>> 61) ^ (W[-2] >> 6) */                    \
    V_XOR(YTMP1, YTMP4, YTMP1)                                              \
            RND_RORX_1_4(f,g,h,a,b,c,d,e,i+3)                               \
            RND_RORX_1_5(f,g,h,a,b,c,d,e,i+3)                               \
    /* W[0] = W[-16] + W[-7] + s0(W[-15]) + s1(W[-2]) */                    \
    V_ADD(W_Y_0, W_Y_0, YTMP1)                                              \
            RND_RORX_1_6(f,g,h,a,b,c,d,e,i+3)                               \
    V_ADD_I(YTMP1, W_Y_0, rsi, i)                                           \
            RND_RORX_1_7(f,g,h,a,b,c,d,e,i+3)                               \
            RND_RORX_1_8(f,g,h,a,b,c,d,e,i+3)                               \
    VMOVDQU_I(rsp, i, YTMP1)                                                \

#define MsgSched2_AVX2_RORX(W_0,W_2,W_4,W_6,W_8,W_10,W_12,W_14,a,b,c,d,e,  \
                            f,g,h,i)                                       \
            RND_RORX_0_1(a,b,c,d,e,f,g,h,i)                                \
    VPALIGNR(W_Y_M15, W_2, W_0, 8)                                         \
    VPALIGNR(W_Y_M7, W_10, W_8, 8)                                         \
            RND_RORX_0_2(a,b,c,d,e,f,g,h,i)                                \
    V_SHIFT_R(YTMP1, W_Y_M15, 1)                                           \
    V_SHIFT_L(YTMP2, W_Y_M15, 63)                                          \
            RND_RORX_0_3(a,b,c,d,e,f,g,h,i)                                \
    V_SHIFT_R(YTMP3, W_Y_M15, 8)                                           \
    V_SHIFT_L(YTMP4, W_Y_M15, 56)                                          \
            RND_RORX_0_4(a,b,c,d,e,f,g,h,i)                                \
    V_OR(YTMP1, YTMP2, YTMP1)                                              \
    V_OR(YTMP3, YTMP4, YTMP3)                                              \
            RND_RORX_0_5(a,b,c,d,e,f,g,h,i)                                \
    V_SHIFT_R(YTMP4, W_Y_M15, 7)                                           \
    V_XOR(YTMP1, YTMP3, YTMP1)                                             \
            RND_RORX_0_6(a,b,c,d,e,f,g,h,i)                                \
    V_XOR(YTMP1, YTMP4, YTMP1)                                             \
    V_ADD(W_0, W_0, W_Y_M7)                                                \
            RND_RORX_0_7(a,b,c,d,e,f,g,h,i)                                \
            RND_RORX_0_8(a,b,c,d,e,f,g,h,i)                                \
    V_ADD(W_0, W_0, YTMP1)                                                 \
            RND_RORX_1_1(h,a,b,c,d,e,f,g,i+1)                              \
    V_SHIFT_R(YTMP1, W_14, 19)                                             \
    V_SHIFT_L(YTMP2, W_14, 45)                                             \
            RND_RORX_1_2(h,a,b,c,d,e,f,g,i+1)                              \
    V_SHIFT_R(YTMP3, W_14, 61)                                             \
    V_SHIFT_L(YTMP4, W_14, 3)                                              \
            RND_RORX_1_3(h,a,b,c,d,e,f,g,i+1)                              \
    V_OR(YTMP1, YTMP2, YTMP1)                                              \
    V_OR(YTMP3, YTMP4, YTMP3)                                              \
            RND_RORX_1_4(h,a,b,c,d,e,f,g,i+1)                              \
            RND_RORX_1_5(h,a,b,c,d,e,f,g,i+1)                              \
    V_XOR(YTMP1, YTMP3, YTMP1)                                             \
    V_SHIFT_R(YTMP4, W_14, 6)                                              \
            RND_RORX_1_6(h,a,b,c,d,e,f,g,i+1)                              \
            RND_RORX_1_7(h,a,b,c,d,e,f,g,i+1)                              \
    V_XOR(YTMP1, YTMP4, YTMP1)                                             \
            RND_RORX_1_8(h,a,b,c,d,e,f,g,i+1)                              \
    V_ADD(W_0, W_0, YTMP1)                                                 \


#define _INIT_MASK_Y(mask)            \
    "vmovdqu %[mask], %%"#mask"\n\t"
#define INIT_MASK_Y(mask) \
       _INIT_MASK_Y(mask)

/* Load into YMM registers and swap endian. */
#define _LOAD_BLOCK_W_Y_2(mask, ymm0, ymm1, reg, i)           \
    /* buffer[0..15] => ymm0..ymm3;  */                       \
    "vmovdqu	" #i "+ 0(%%" #reg "), %%" #ymm0 "\n\t"       \
    "vmovdqu	" #i "+32(%%" #reg "), %%" #ymm1 "\n\t"       \
    "vpshufb	%%" #mask ", %%" #ymm0 ", %%" #ymm0 "\n\t"    \
    "vpshufb	%%" #mask ", %%" #ymm1 ", %%" #ymm1 "\n\t"

#define LOAD_BLOCK_W_Y_2(mask, ymm1, ymm2, reg, i) \
       _LOAD_BLOCK_W_Y_2(mask, ymm1, ymm2, reg, i)

#define LOAD_BLOCK_W_Y(mask, reg)                  \
    LOAD_BLOCK_W_Y_2(mask, W_Y_0, W_Y_4 , reg,  0) \
    LOAD_BLOCK_W_Y_2(mask, W_Y_8, W_Y_12, reg, 64)

#define _SET_W_Y_2(ymm0, ymm1, ymm2, ymm3, reg, i)                    \
    "vpaddq	" #i "+ 0(%%" #reg "), %%" #ymm0 ", %%" #ymm2 "\n\t"  \
    "vpaddq	" #i "+32(%%" #reg "), %%" #ymm1 ", %%" #ymm3 "\n\t"  \
    "vmovdqu	%%" #ymm2 ", " #i "+ 0(" WX ")\n\t"                   \
    "vmovdqu	%%" #ymm3 ", " #i "+32(" WX ")\n\t"

#define SET_W_Y_2(ymm0, ymm1, ymm2, ymm3, reg, i) \
       _SET_W_Y_2(ymm0, ymm1, ymm2, ymm3, reg, i)

#define SET_BLOCK_W_Y(reg)                          \
    SET_W_Y_2(W_Y_0, W_Y_4 , YTMP1, YTMP2, reg,  0) \
    SET_W_Y_2(W_Y_8, W_Y_12, YTMP1, YTMP2, reg, 64)

/* Load into YMM registers and swap endian. */
#define _LOAD_BLOCK2_W_Y_2(mask, Y0, Y1, X0, X1, X8, X9, reg, i)   \
    "vmovdqu	" #i "+  0(%%" #reg "), %%" #X0 "\n\t"                   \
    "vmovdqu	" #i "+ 16(%%" #reg "), %%" #X1 "\n\t"                   \
    "vmovdqu	" #i "+128(%%" #reg "), %%" #X8 "\n\t"                   \
    "vmovdqu	" #i "+144(%%" #reg "), %%" #X9 "\n\t"                   \
    "vinserti128	$1, %%" #X8 ", %%" #Y0 ", %%" #Y0 "\n\t"         \
    "vinserti128	$1, %%" #X9 ", %%" #Y1 ", %%" #Y1 "\n\t"         \
    "vpshufb	%%" #mask ", %%" #Y0 ", %%" #Y0 "\n\t"                   \
    "vpshufb	%%" #mask ", %%" #Y1 ", %%" #Y1 "\n\t"

#define LOAD_BLOCK2_W_Y_2(mask, Y0, Y1, X0, X1, X8, X9, reg, i) \
       _LOAD_BLOCK2_W_Y_2(mask, Y0, Y1, X0, X1, X8, X9, reg, i)

#define LOAD_BLOCK2_W_Y(mask, reg)                           \
    LOAD_BLOCK2_W_Y_2(mask, Y0, Y1, X0, X1, X8, X9, reg,  0) \
    LOAD_BLOCK2_W_Y_2(mask, Y2, Y3, X2, X3, X8, X9, reg, 32) \
    LOAD_BLOCK2_W_Y_2(mask, Y4, Y5, X4, X5, X8, X9, reg, 64) \
    LOAD_BLOCK2_W_Y_2(mask, Y6, Y7, X6, X7, X8, X9, reg, 96) \

#define SET_BLOCK2_W_Y(reg)                   \
    SET_W_Y_2(Y0, Y1, YTMP1, YTMP2, reg,   0) \
    SET_W_Y_2(Y2, Y3, YTMP1, YTMP2, reg,  64) \
    SET_W_Y_2(Y4, Y5, YTMP1, YTMP2, reg, 128) \
    SET_W_Y_2(Y6, Y7, YTMP1, YTMP2, reg, 192)

static const word64 K512_AVX2[160] = {
    W64LIT(0x428a2f98d728ae22), W64LIT(0x7137449123ef65cd),
    W64LIT(0x428a2f98d728ae22), W64LIT(0x7137449123ef65cd),
    W64LIT(0xb5c0fbcfec4d3b2f), W64LIT(0xe9b5dba58189dbbc),
    W64LIT(0xb5c0fbcfec4d3b2f), W64LIT(0xe9b5dba58189dbbc),
    W64LIT(0x3956c25bf348b538), W64LIT(0x59f111f1b605d019),
    W64LIT(0x3956c25bf348b538), W64LIT(0x59f111f1b605d019),
    W64LIT(0x923f82a4af194f9b), W64LIT(0xab1c5ed5da6d8118),
    W64LIT(0x923f82a4af194f9b), W64LIT(0xab1c5ed5da6d8118),
    W64LIT(0xd807aa98a3030242), W64LIT(0x12835b0145706fbe),
    W64LIT(0xd807aa98a3030242), W64LIT(0x12835b0145706fbe),
    W64LIT(0x243185be4ee4b28c), W64LIT(0x550c7dc3d5ffb4e2),
    W64LIT(0x243185be4ee4b28c), W64LIT(0x550c7dc3d5ffb4e2),
    W64LIT(0x72be5d74f27b896f), W64LIT(0x80deb1fe3b1696b1),
    W64LIT(0x72be5d74f27b896f), W64LIT(0x80deb1fe3b1696b1),
    W64LIT(0x9bdc06a725c71235), W64LIT(0xc19bf174cf692694),
    W64LIT(0x9bdc06a725c71235), W64LIT(0xc19bf174cf692694),
    W64LIT(0xe49b69c19ef14ad2), W64LIT(0xefbe4786384f25e3),
    W64LIT(0xe49b69c19ef14ad2), W64LIT(0xefbe4786384f25e3),
    W64LIT(0x0fc19dc68b8cd5b5), W64LIT(0x240ca1cc77ac9c65),
    W64LIT(0x0fc19dc68b8cd5b5), W64LIT(0x240ca1cc77ac9c65),
    W64LIT(0x2de92c6f592b0275), W64LIT(0x4a7484aa6ea6e483),
    W64LIT(0x2de92c6f592b0275), W64LIT(0x4a7484aa6ea6e483),
    W64LIT(0x5cb0a9dcbd41fbd4), W64LIT(0x76f988da831153b5),
    W64LIT(0x5cb0a9dcbd41fbd4), W64LIT(0x76f988da831153b5),
    W64LIT(0x983e5152ee66dfab), W64LIT(0xa831c66d2db43210),
    W64LIT(0x983e5152ee66dfab), W64LIT(0xa831c66d2db43210),
    W64LIT(0xb00327c898fb213f), W64LIT(0xbf597fc7beef0ee4),
    W64LIT(0xb00327c898fb213f), W64LIT(0xbf597fc7beef0ee4),
    W64LIT(0xc6e00bf33da88fc2), W64LIT(0xd5a79147930aa725),
    W64LIT(0xc6e00bf33da88fc2), W64LIT(0xd5a79147930aa725),
    W64LIT(0x06ca6351e003826f), W64LIT(0x142929670a0e6e70),
    W64LIT(0x06ca6351e003826f), W64LIT(0x142929670a0e6e70),
    W64LIT(0x27b70a8546d22ffc), W64LIT(0x2e1b21385c26c926),
    W64LIT(0x27b70a8546d22ffc), W64LIT(0x2e1b21385c26c926),
    W64LIT(0x4d2c6dfc5ac42aed), W64LIT(0x53380d139d95b3df),
    W64LIT(0x4d2c6dfc5ac42aed), W64LIT(0x53380d139d95b3df),
    W64LIT(0x650a73548baf63de), W64LIT(0x766a0abb3c77b2a8),
    W64LIT(0x650a73548baf63de), W64LIT(0x766a0abb3c77b2a8),
    W64LIT(0x81c2c92e47edaee6), W64LIT(0x92722c851482353b),
    W64LIT(0x81c2c92e47edaee6), W64LIT(0x92722c851482353b),
    W64LIT(0xa2bfe8a14cf10364), W64LIT(0xa81a664bbc423001),
    W64LIT(0xa2bfe8a14cf10364), W64LIT(0xa81a664bbc423001),
    W64LIT(0xc24b8b70d0f89791), W64LIT(0xc76c51a30654be30),
    W64LIT(0xc24b8b70d0f89791), W64LIT(0xc76c51a30654be30),
    W64LIT(0xd192e819d6ef5218), W64LIT(0xd69906245565a910),
    W64LIT(0xd192e819d6ef5218), W64LIT(0xd69906245565a910),
    W64LIT(0xf40e35855771202a), W64LIT(0x106aa07032bbd1b8),
    W64LIT(0xf40e35855771202a), W64LIT(0x106aa07032bbd1b8),
    W64LIT(0x19a4c116b8d2d0c8), W64LIT(0x1e376c085141ab53),
    W64LIT(0x19a4c116b8d2d0c8), W64LIT(0x1e376c085141ab53),
    W64LIT(0x2748774cdf8eeb99), W64LIT(0x34b0bcb5e19b48a8),
    W64LIT(0x2748774cdf8eeb99), W64LIT(0x34b0bcb5e19b48a8),
    W64LIT(0x391c0cb3c5c95a63), W64LIT(0x4ed8aa4ae3418acb),
    W64LIT(0x391c0cb3c5c95a63), W64LIT(0x4ed8aa4ae3418acb),
    W64LIT(0x5b9cca4f7763e373), W64LIT(0x682e6ff3d6b2b8a3),
    W64LIT(0x5b9cca4f7763e373), W64LIT(0x682e6ff3d6b2b8a3),
    W64LIT(0x748f82ee5defb2fc), W64LIT(0x78a5636f43172f60),
    W64LIT(0x748f82ee5defb2fc), W64LIT(0x78a5636f43172f60),
    W64LIT(0x84c87814a1f0ab72), W64LIT(0x8cc702081a6439ec),
    W64LIT(0x84c87814a1f0ab72), W64LIT(0x8cc702081a6439ec),
    W64LIT(0x90befffa23631e28), W64LIT(0xa4506cebde82bde9),
    W64LIT(0x90befffa23631e28), W64LIT(0xa4506cebde82bde9),
    W64LIT(0xbef9a3f7b2c67915), W64LIT(0xc67178f2e372532b),
    W64LIT(0xbef9a3f7b2c67915), W64LIT(0xc67178f2e372532b),
    W64LIT(0xca273eceea26619c), W64LIT(0xd186b8c721c0c207),
    W64LIT(0xca273eceea26619c), W64LIT(0xd186b8c721c0c207),
    W64LIT(0xeada7dd6cde0eb1e), W64LIT(0xf57d4f7fee6ed178),
    W64LIT(0xeada7dd6cde0eb1e), W64LIT(0xf57d4f7fee6ed178),
    W64LIT(0x06f067aa72176fba), W64LIT(0x0a637dc5a2c898a6),
    W64LIT(0x06f067aa72176fba), W64LIT(0x0a637dc5a2c898a6),
    W64LIT(0x113f9804bef90dae), W64LIT(0x1b710b35131c471b),
    W64LIT(0x113f9804bef90dae), W64LIT(0x1b710b35131c471b),
    W64LIT(0x28db77f523047d84), W64LIT(0x32caab7b40c72493),
    W64LIT(0x28db77f523047d84), W64LIT(0x32caab7b40c72493),
    W64LIT(0x3c9ebe0a15c9bebc), W64LIT(0x431d67c49c100d4c),
    W64LIT(0x3c9ebe0a15c9bebc), W64LIT(0x431d67c49c100d4c),
    W64LIT(0x4cc5d4becb3e42b6), W64LIT(0x597f299cfc657e2a),
    W64LIT(0x4cc5d4becb3e42b6), W64LIT(0x597f299cfc657e2a),
    W64LIT(0x5fcb6fab3ad6faec), W64LIT(0x6c44198c4a475817),
    W64LIT(0x5fcb6fab3ad6faec), W64LIT(0x6c44198c4a475817)
};
static const word64* K512_AVX2_END = &K512_AVX2[128];

static int Transform_Sha512_AVX2(wc_Sha512* sha512)
{
    __asm__ __volatile__ (

        /* 16 Ws plus loop counter and K512. */
        "subq	$136, %%rsp\n\t"
        "leaq	64(%[sha512]), %%rax\n\t"

    INIT_MASK(MASK_Y)
    LOAD_DIGEST()

    LOAD_BLOCK_W_Y(MASK_Y, rax)

        "movl	$4, 16*8(" WX ")\n\t"
        "leaq	%[K512], %%rsi\n\t"
        /* b */
        "movq	%%r9, " L4 "\n\t"
        /* e */
        "movq	%%r12, " L1 "\n\t"
        /* b ^ c */
        "xorq	%%r10, " L4 "\n\t"

    SET_BLOCK_W_Y(rsi)

        "# Start of 16 rounds\n"
        "1:\n\t"

        "addq	$128, %%rsi\n\t"

    MsgSched4_AVX2(W_Y_0,W_Y_4,W_Y_8,W_Y_12,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched4_AVX2(W_Y_4,W_Y_8,W_Y_12,W_Y_0,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched4_AVX2(W_Y_8,W_Y_12,W_Y_0,W_Y_4,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched4_AVX2(W_Y_12,W_Y_0,W_Y_4,W_Y_8,RE,RF,RG,RH,RA,RB,RC,RD,12)

    SET_BLOCK_W_Y(rsi)

        "subl	$1, 16*8(" WX ")\n\t"
        "jne	1b\n\t"

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 2)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB, 6)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,10)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,12)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

    STORE_ADD_DIGEST()

        "addq	$136, %%rsp\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK_Y),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory", "rsi"
    );

    return 0;
}

static int Transform_Sha512_AVX2_Len(wc_Sha512* sha512, word32 len)
{
    if ((len & WC_SHA512_BLOCK_SIZE) != 0) {
        XMEMCPY(sha512->buffer, sha512->data, WC_SHA512_BLOCK_SIZE);
        Transform_Sha512_AVX2(sha512);
        sha512->data += WC_SHA512_BLOCK_SIZE;
        len -= WC_SHA512_BLOCK_SIZE;
        if (len == 0)
            return 0;
    }

    __asm__ __volatile__ (

        "movq	224(%[sha512]), %%rcx\n\t"

    INIT_MASK(MASK_Y)
    LOAD_DIGEST()

        "# Start of processing two blocks\n"
        "2:\n\t"

        "subq	$1344, %%rsp\n\t"
        "leaq	%[K512], %%rsi\n\t"

        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* e */
        "movq	%%r12, " L1 "\n\t"

    LOAD_BLOCK2_W_Y(MASK_Y, rcx)

        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"
        "\n"
        "1:\n\t"
    SET_BLOCK2_W_Y(rsi)
    MsgSched2_AVX2(Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched2_AVX2(Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y0,RG,RH,RA,RB,RC,RD,RE,RF, 4)
    MsgSched2_AVX2(Y2,Y3,Y4,Y5,Y6,Y7,Y0,Y1,RE,RF,RG,RH,RA,RB,RC,RD, 8)
    MsgSched2_AVX2(Y3,Y4,Y5,Y6,Y7,Y0,Y1,Y2,RC,RD,RE,RF,RG,RH,RA,RB,12)
    MsgSched2_AVX2(Y4,Y5,Y6,Y7,Y0,Y1,Y2,Y3,RA,RB,RC,RD,RE,RF,RG,RH,16)
    MsgSched2_AVX2(Y5,Y6,Y7,Y0,Y1,Y2,Y3,Y4,RG,RH,RA,RB,RC,RD,RE,RF,20)
    MsgSched2_AVX2(Y6,Y7,Y0,Y1,Y2,Y3,Y4,Y5,RE,RF,RG,RH,RA,RB,RC,RD,24)
    MsgSched2_AVX2(Y7,Y0,Y1,Y2,Y3,Y4,Y5,Y6,RC,RD,RE,RF,RG,RH,RA,RB,28)
        "addq	$256, %%rsi\n\t"
        "addq	$256, %%rsp\n\t"
        "cmpq	%[K512_END], %%rsi\n\t"
        "jne	1b\n\t"

    SET_BLOCK2_W_Y(rsi)
    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 4)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 8)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,12)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH,16)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,20)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,24)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,28)
        "subq	$1024, %%rsp\n\t"

    ADD_DIGEST()
    STORE_DIGEST()

        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* e */
        "movq	%%r12, " L1 "\n\t"
        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"

        "movq	$5, %%rsi\n\t"
        "\n"
        "3:\n\t"
    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 2)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 6)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,10)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

    RND_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH,18)
    RND_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,22)
    RND_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,26)
    RND_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,30)
        "addq	$256, %%rsp\n\t"
        "subq	$1, %%rsi\n\t"
        "jnz	3b\n\t"

    ADD_DIGEST()

        "movq	224(%[sha512]), %%rcx\n\t"
        "addq	$64, %%rsp\n\t"
        "addq	$256, %%rcx\n\t"
        "subl	$256, %[len]\n\t"
        "movq	%%rcx, 224(%[sha512])\n\t"

    STORE_DIGEST()

        "jnz	2b\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK_Y),
          [len]    "m" (len),
          [sha512] "r" (sha512),
          [K512]   "m" (K512_AVX2),
          [K512_END]   "m" (K512_AVX2_END)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory", "rsi"
    );

    return 0;
}

#ifdef HAVE_INTEL_RORX
static int Transform_Sha512_AVX2_RORX(wc_Sha512* sha512)
{
    __asm__ __volatile__ (

        /* 16 Ws plus loop counter. */
        "subq	$136, %%rsp\n\t"
        "leaq	64(%[sha512]), " L2 "\n\t"

    INIT_MASK(MASK_Y)
    LOAD_DIGEST()

    LOAD_BLOCK_W_Y(MASK_Y, rcx)

        "movl	$4, 16*8(" WX ")\n\t"
        "leaq	%[K512], %%rsi\n\t"
        /* b */
        "movq	%%r9, " L4 "\n\t"
        /* L3 = 0 (add to prev h) */
        "xorq	" L3 ", " L3 "\n\t"
        /* b ^ c */
        "xorq	%%r10, " L4 "\n\t"

    SET_BLOCK_W_Y(rsi)

        "# Start of 16 rounds\n"
        "1:\n\t"

        "addq	$128, %%rsi\n\t"

    MsgSched4_AVX2_RORX_SET(W_Y_0,W_Y_4,W_Y_8,W_Y_12,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched4_AVX2_RORX_SET(W_Y_4,W_Y_8,W_Y_12,W_Y_0,RE,RF,RG,RH,RA,RB,RC,RD, 4)
    MsgSched4_AVX2_RORX_SET(W_Y_8,W_Y_12,W_Y_0,W_Y_4,RA,RB,RC,RD,RE,RF,RG,RH, 8)
    MsgSched4_AVX2_RORX_SET(W_Y_12,W_Y_0,W_Y_4,W_Y_8,RE,RF,RG,RH,RA,RB,RC,RD,12)

        "subl	$1, 16*8(%%rsp)\n\t"
        "jnz	1b\n\t"

    RND_RORX_ALL_4(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_RORX_ALL_4(RE,RF,RG,RH,RA,RB,RC,RD, 4)
    RND_RORX_ALL_4(RA,RB,RC,RD,RE,RF,RG,RH, 8)
    RND_RORX_ALL_4(RE,RF,RG,RH,RA,RB,RC,RD,12)
        /* Prev RND: h += Maj(a,b,c) */
        "addq	" L3 ", %%r8\n\t"
        "addq	$136, %%rsp\n\t"

    STORE_ADD_DIGEST()

        :
        : [mask]   "m" (mBYTE_FLIP_MASK_Y),
          [sha512] "r" (sha512),
          [K512]   "m" (K512)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory", "rsi"
    );

    return 0;
}

static int Transform_Sha512_AVX2_RORX_Len(wc_Sha512* sha512, word32 len)
{
    if ((len & WC_SHA512_BLOCK_SIZE) != 0) {
        XMEMCPY(sha512->buffer, sha512->data, WC_SHA512_BLOCK_SIZE);
        Transform_Sha512_AVX2_RORX(sha512);
        sha512->data += WC_SHA512_BLOCK_SIZE;
        len -= WC_SHA512_BLOCK_SIZE;
        if (len == 0)
            return 0;
    }

    __asm__ __volatile__ (

        "movq	224(%[sha512]), %%rax\n\t"

    INIT_MASK(MASK_Y)
    LOAD_DIGEST()

        "# Start of processing two blocks\n"
        "2:\n\t"

        "subq	$1344, %%rsp\n\t"
        "leaq	%[K512], %%rsi\n\t"

        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* L3 = 0 (add to prev h) */
        "xorq	" L3 ", " L3 "\n\t"

    LOAD_BLOCK2_W_Y(MASK_Y, rax)

        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"
        "\n"
        "1:\n\t"
    SET_BLOCK2_W_Y(rsi)
    MsgSched2_AVX2_RORX(Y0,Y1,Y2,Y3,Y4,Y5,Y6,Y7,RA,RB,RC,RD,RE,RF,RG,RH, 0)
    MsgSched2_AVX2_RORX(Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y0,RG,RH,RA,RB,RC,RD,RE,RF, 4)
    MsgSched2_AVX2_RORX(Y2,Y3,Y4,Y5,Y6,Y7,Y0,Y1,RE,RF,RG,RH,RA,RB,RC,RD, 8)
    MsgSched2_AVX2_RORX(Y3,Y4,Y5,Y6,Y7,Y0,Y1,Y2,RC,RD,RE,RF,RG,RH,RA,RB,12)
    MsgSched2_AVX2_RORX(Y4,Y5,Y6,Y7,Y0,Y1,Y2,Y3,RA,RB,RC,RD,RE,RF,RG,RH,16)
    MsgSched2_AVX2_RORX(Y5,Y6,Y7,Y0,Y1,Y2,Y3,Y4,RG,RH,RA,RB,RC,RD,RE,RF,20)
    MsgSched2_AVX2_RORX(Y6,Y7,Y0,Y1,Y2,Y3,Y4,Y5,RE,RF,RG,RH,RA,RB,RC,RD,24)
    MsgSched2_AVX2_RORX(Y7,Y0,Y1,Y2,Y3,Y4,Y5,Y6,RC,RD,RE,RF,RG,RH,RA,RB,28)
        "addq	$256, %%rsi\n\t"
        "addq	$256, %%rsp\n\t"
        "cmpq	%[K512_END], %%rsi\n\t"
        "jne	1b\n\t"

    SET_BLOCK2_W_Y(rsi)
    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 0)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 4)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD, 8)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,12)

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH,16)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,20)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,24)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,28)
        "addq	" L3 ", %%r8\n\t"
        "subq	$1024, %%rsp\n\t"

    ADD_DIGEST()
    STORE_DIGEST()

        /* L4 = b */
        "movq	%%r9, " L4 "\n\t"
        /* L3 = 0 (add to prev h) */
        "xorq	" L3 ", " L3 "\n\t"
        /* L4 = b ^ c */
        "xorq	%%r10, " L4 "\n\t"

        "movq	$5, %%rsi\n\t"
        "\n"
        "3:\n\t"
    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH, 2)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF, 6)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,10)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,14)

    RND_RORX_ALL_2(RA,RB,RC,RD,RE,RF,RG,RH,18)
    RND_RORX_ALL_2(RG,RH,RA,RB,RC,RD,RE,RF,22)
    RND_RORX_ALL_2(RE,RF,RG,RH,RA,RB,RC,RD,26)
    RND_RORX_ALL_2(RC,RD,RE,RF,RG,RH,RA,RB,30)
        "addq	$256, %%rsp\n\t"
        "subq	$1, %%rsi\n\t"
        "jnz	3b\n\t"

        "addq	" L3 ", %%r8\n\t"

    ADD_DIGEST()

        "movq	224(%[sha512]), %%rax\n\t"
        "addq	$64, %%rsp\n\t"
        "addq	$256, %%rax\n\t"
        "subl	$256, %[len]\n\t"
        "movq	%%rax, 224(%[sha512])\n\t"

    STORE_DIGEST()

        "jnz	2b\n\t"

        :
        : [mask]   "m" (mBYTE_FLIP_MASK_Y),
          [len]    "m" (len),
          [sha512] "r" (sha512),
          [K512]   "m" (K512_AVX2),
          [K512_END]   "m" (K512_AVX2_END)
        : WORK_REGS, STATE_REGS, YMM_REGS, "memory", "rsi"
    );

    return 0;
}
#endif /* HAVE_INTEL_RORX */
#endif /* HAVE_INTEL_AVX2 */



/* -------------------------------------------------------------------------- */
/* SHA384 */
/* -------------------------------------------------------------------------- */
#ifdef WOLFSSL_SHA384

#if defined(WOLFSSL_IMX6_CAAM) && !defined(NO_IMX6_CAAM_HASH)
    /* functions defined in wolfcrypt/src/port/caam/caam_sha.c */
#else

static int InitSha384(wc_Sha384* sha384)
{
    if (sha384 == NULL) {
        return BAD_FUNC_ARG;
    }

    sha384->digest[0] = W64LIT(0xcbbb9d5dc1059ed8);
    sha384->digest[1] = W64LIT(0x629a292a367cd507);
    sha384->digest[2] = W64LIT(0x9159015a3070dd17);
    sha384->digest[3] = W64LIT(0x152fecd8f70e5939);
    sha384->digest[4] = W64LIT(0x67332667ffc00b31);
    sha384->digest[5] = W64LIT(0x8eb44a8768581511);
    sha384->digest[6] = W64LIT(0xdb0c2e0d64f98fa7);
    sha384->digest[7] = W64LIT(0x47b5481dbefa4fa4);

    sha384->buffLen = 0;
    sha384->loLen   = 0;
    sha384->hiLen   = 0;

    return 0;
}

int wc_Sha384Update(wc_Sha384* sha384, const byte* data, word32 len)
{
    if (sha384 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA384)
    if (sha384->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA384) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha384(&sha384->asyncDev, NULL, data, len);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    return Sha512Update((wc_Sha512*)sha384, data, len);
}


int wc_Sha384FinalRaw(wc_Sha384* sha384, byte* hash)
{
#ifdef LITTLE_ENDIAN_ORDER
    word64 digest[WC_SHA384_DIGEST_SIZE / sizeof(word64)];
#endif

    if (sha384 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef LITTLE_ENDIAN_ORDER
    ByteReverseWords64((word64*)digest, (word64*)sha384->digest,
                                                         WC_SHA384_DIGEST_SIZE);
    XMEMCPY(hash, digest, WC_SHA384_DIGEST_SIZE);
#else
    XMEMCPY(hash, sha384->digest, WC_SHA384_DIGEST_SIZE);
#endif

    return 0;
}

int wc_Sha384Final(wc_Sha384* sha384, byte* hash)
{
    int ret;

    if (sha384 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA384)
    if (sha384->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA384) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha384(&sha384->asyncDev, hash, NULL,
                                            WC_SHA384_DIGEST_SIZE);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha512Final((wc_Sha512*)sha384);
    if (ret != 0)
        return ret;

    XMEMCPY(hash, sha384->digest, WC_SHA384_DIGEST_SIZE);

    return InitSha384(sha384);  /* reset state */
}


/* Hardware Acceleration */
#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
    int wc_InitSha384_ex(wc_Sha384* sha384, void* heap, int devId)
    {
        int ret = InitSha384(sha384);

        (void)heap;
        (void)devId;

        Sha512_SetTransform();

        return ret;
    }
#else
int wc_InitSha384_ex(wc_Sha384* sha384, void* heap, int devId)
{
    int ret;

    if (sha384 == NULL) {
        return BAD_FUNC_ARG;
    }

    sha384->heap = heap;
    ret = InitSha384(sha384);
    if (ret != 0)
        return ret;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA384)
    ret = wolfAsync_DevCtxInit(&sha384->asyncDev, WOLFSSL_ASYNC_MARKER_SHA384,
                                                           sha384->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}
#endif
#endif /* WOLFSSL_IMX6_CAAM */

int wc_InitSha384(wc_Sha384* sha384)
{
    return wc_InitSha384_ex(sha384, NULL, INVALID_DEVID);
}

void wc_Sha384Free(wc_Sha384* sha384)
{
    if (sha384 == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA384)
    wolfAsync_DevCtxFree(&sha384->asyncDev, WOLFSSL_ASYNC_MARKER_SHA384);
#endif /* WOLFSSL_ASYNC_CRYPT */
}

#endif /* WOLFSSL_SHA384 */

#endif /* HAVE_FIPS */


int wc_Sha512GetHash(wc_Sha512* sha512, byte* hash)
{
    int ret;
    wc_Sha512 tmpSha512;

    if (sha512 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha512Copy(sha512, &tmpSha512);
    if (ret == 0) {
        ret = wc_Sha512Final(&tmpSha512, hash);
    }
    return ret;
}

int wc_Sha512Copy(wc_Sha512* src, wc_Sha512* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha512));

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif

    return ret;
}

#ifdef WOLFSSL_SHA384
int wc_Sha384GetHash(wc_Sha384* sha384, byte* hash)
{
    int ret;
    wc_Sha384 tmpSha384;

    if (sha384 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha384Copy(sha384, &tmpSha384);
    if (ret == 0) {
        ret = wc_Sha384Final(&tmpSha384, hash);
    }
    return ret;
}
int wc_Sha384Copy(wc_Sha384* src, wc_Sha384* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(wc_Sha384));

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif

    return ret;
}
#endif /* WOLFSSL_SHA384 */

#endif /* WOLFSSL_SHA512 */
