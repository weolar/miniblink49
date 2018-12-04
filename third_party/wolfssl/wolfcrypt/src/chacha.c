/* chacha.c
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
 *
 *  based from
 *  chacha-ref.c version 20080118
 *  D. J. Bernstein
 *  Public domain.
 */



#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_CHACHA

#include <wolfssl/wolfcrypt/chacha.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef CHACHA_AEAD_TEST
    #include <stdio.h>
#endif

#ifdef USE_INTEL_CHACHA_SPEEDUP
    #include <emmintrin.h>
    #include <immintrin.h>

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

    #ifndef NO_AVX2_SUPPORT
        #define HAVE_INTEL_AVX2
    #endif

    #if defined(_MSC_VER)
        #define CHACHA20_NOINLINE __declspec(noinline)
    #elif defined(__GNUC__)
        #define CHACHA20_NOINLINE __attribute__((noinline))
    #else
        #define CHACHA20_NOINLINE
    #endif

    static int cpuidFlagsSet = 0;
    static int cpuidFlags = 0;
#endif

#ifdef BIG_ENDIAN_ORDER
    #define LITTLE32(x) ByteReverseWord32(x)
#else
    #define LITTLE32(x) (x)
#endif

/* Number of rounds */
#define ROUNDS  20

#define U32C(v) (v##U)
#define U32V(v) ((word32)(v) & U32C(0xFFFFFFFF))
#define U8TO32_LITTLE(p) LITTLE32(((word32*)(p))[0])

#define ROTATE(v,c) rotlFixed(v, c)
#define XOR(v,w)    ((v) ^ (w))
#define PLUS(v,w)   (U32V((v) + (w)))
#define PLUSONE(v)  (PLUS((v),1))

#define QUARTERROUND(a,b,c,d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);



#define QUARTERROUND_INTEL_ASM(a0,b0,c0,d0,   \
                               a1,b1,c1,d1,   \
                               a2,b2,c2,d2,   \
                               a3,b3,c3,d3,   \
                               t1,o1)         \
       "vpaddd	"#b0", "#a0", "#a0"\n\t"      \
       "vpxor	"#a0", "#d0", "#d0"\n\t"      \
       "vmovdqa	"#o1"(%[x]), "#c3"\n\t"       \
       "vpshufb	%[rotl16], "#d0", "#d0"\n\t"  \
       "vpaddd	"#d0", "#c0", "#c0"\n\t"      \
       "vpxor	"#c0", "#b0", "#b0"\n\t"      \
       "vpaddd	"#b1", "#a1", "#a1"\n\t"      \
       "vpxor	"#a1", "#d1", "#d1"\n\t"      \
       "vpshufb	%[rotl16], "#d1", "#d1"\n\t"  \
       "vpaddd	"#d1", "#c1", "#c1"\n\t"      \
       "vpxor	"#c1", "#b1", "#b1"\n\t"      \
       "vpaddd	"#b2", "#a2", "#a2"\n\t"      \
       "vpxor	"#a2", "#d2", "#d2"\n\t"      \
       "vpshufb	%[rotl16], "#d2", "#d2"\n\t"  \
       "vpaddd	"#d2", "#c2", "#c2"\n\t"      \
       "vpxor	"#c2", "#b2", "#b2"\n\t"      \
       "vpaddd	"#b3", "#a3", "#a3"\n\t"      \
       "vpxor	"#a3", "#d3", "#d3"\n\t"      \
       "vpshufb	%[rotl16], "#d3", "#d3"\n\t"  \
       "vpaddd  "#d3", "#c3", "#c3"\n\t"      \
       "vpxor	"#c3", "#b3", "#b3"\n\t"      \
       "vmovdqa	"#c3", "#o1"(%[x])\n\t"       \
       "vpsrld	$20, "#b0", "#t1"\n\t"        \
       "vpslld	$12, "#b0", "#b0"\n\t"        \
       "vpxor	"#t1", "#b0", "#b0"\n\t"      \
       "vpsrld	$20, "#b1", "#t1"\n\t"        \
       "vpslld	$12, "#b1", "#b1"\n\t"        \
       "vpxor	"#t1", "#b1", "#b1"\n\t"      \
       "vpsrld	$20, "#b2", "#t1"\n\t"        \
       "vpslld	$12, "#b2", "#b2"\n\t"        \
       "vpxor	"#t1", "#b2", "#b2"\n\t"      \
       "vpsrld	$20, "#b3", "#t1"\n\t"        \
       "vpslld	$12, "#b3", "#b3"\n\t"        \
       "vpxor	"#t1", "#b3", "#b3"\n\t"      \
       "vpaddd	"#b0", "#a0", "#a0"\n\t"      \
       "vpxor	"#a0", "#d0", "#d0"\n\t"      \
       "vmovdqa	"#o1"(%[x]), "#c3"\n\t"       \
       "vpshufb	%[rotl8], "#d0", "#d0"\n\t"   \
       "vpaddd	"#d0", "#c0", "#c0"\n\t"      \
       "vpxor	"#c0", "#b0", "#b0"\n\t"      \
       "vpaddd	"#b1", "#a1", "#a1"\n\t"      \
       "vpxor	"#a1", "#d1", "#d1"\n\t"      \
       "vpshufb	%[rotl8], "#d1", "#d1"\n\t"   \
       "vpaddd	"#d1", "#c1", "#c1"\n\t"      \
       "vpxor	"#c1", "#b1", "#b1"\n\t"      \
       "vpaddd	"#b2", "#a2", "#a2"\n\t"      \
       "vpxor	"#a2", "#d2", "#d2"\n\t"      \
       "vpshufb	%[rotl8], "#d2", "#d2"\n\t"   \
       "vpaddd	"#d2", "#c2", "#c2"\n\t"      \
       "vpxor	"#c2", "#b2", "#b2"\n\t"      \
       "vpaddd	"#b3", "#a3", "#a3"\n\t"      \
       "vpxor	"#a3", "#d3", "#d3"\n\t"      \
       "vpshufb	%[rotl8], "#d3", "#d3"\n\t"   \
       "vpaddd	"#d3", "#c3", "#c3"\n\t"      \
       "vpxor	"#c3", "#b3", "#b3"\n\t"      \
       "vmovdqa	"#c3", "#o1"(%[x])\n\t"       \
       "vpsrld	$25, "#b0", "#t1"\n\t"        \
       "vpslld	 $7, "#b0", "#b0"\n\t"        \
       "vpxor	"#t1", "#b0", "#b0"\n\t"      \
       "vpsrld	$25, "#b1", "#t1"\n\t"        \
       "vpslld	 $7, "#b1", "#b1"\n\t"        \
       "vpxor	"#t1", "#b1", "#b1"\n\t"      \
       "vpsrld	$25, "#b2", "#t1"\n\t"        \
       "vpslld	 $7, "#b2", "#b2"\n\t"        \
       "vpxor	"#t1", "#b2", "#b2"\n\t"      \
       "vpsrld	$25, "#b3", "#t1"\n\t"        \
       "vpslld	 $7, "#b3", "#b3"\n\t"        \
       "vpxor	"#t1", "#b3", "#b3"\n\t"

#define QUARTERROUND_INTEL_ASM_2(a0,b0,c0,d0, \
                                 a1,b1,c1,d1, \
                                 a2,b2,c2,d2, \
                                 a3,b3,c3,d3, \
                                 t1,o1)       \
       "vpaddd	"#b0", "#a0", "#a0"\n\t"      \
       "vpxor	"#a0", "#d0", "#d0"\n\t"      \
       "vmovdqa	"#o1"(%[x]), "#c1"\n\t"       \
       "vpshufb	%[rotl16], "#d0", "#d0"\n\t"  \
       "vpaddd	"#d0", "#c0", "#c0"\n\t"      \
       "vpxor	"#c0", "#b0", "#b0"\n\t"      \
       "vpaddd	"#b1", "#a1", "#a1"\n\t"      \
       "vpxor	"#a1", "#d1", "#d1"\n\t"      \
       "vpshufb	%[rotl16], "#d1", "#d1"\n\t"  \
       "vpaddd	"#d1", "#c1", "#c1"\n\t"      \
       "vpxor	"#c1", "#b1", "#b1"\n\t"      \
       "vpaddd	"#b2", "#a2", "#a2"\n\t"      \
       "vpxor	"#a2", "#d2", "#d2"\n\t"      \
       "vpshufb	%[rotl16], "#d2", "#d2"\n\t"  \
       "vpaddd	"#d2", "#c2", "#c2"\n\t"      \
       "vpxor	"#c2", "#b2", "#b2"\n\t"      \
       "vpaddd	"#b3", "#a3", "#a3"\n\t"      \
       "vpxor	"#a3", "#d3", "#d3"\n\t"      \
       "vpshufb	%[rotl16], "#d3", "#d3"\n\t"  \
       "vpaddd	"#d3", "#c3", "#c3"\n\t"      \
       "vpxor	"#c3", "#b3", "#b3"\n\t"      \
       "vmovdqa	"#c1", "#o1"(%[x])\n\t"       \
       "vpsrld	$20, "#b0", "#t1"\n\t"        \
       "vpslld	$12, "#b0", "#b0"\n\t"        \
       "vpxor	"#t1", "#b0", "#b0"\n\t"      \
       "vpsrld	$20, "#b1", "#t1"\n\t"        \
       "vpslld	$12, "#b1", "#b1"\n\t"        \
       "vpxor	"#t1", "#b1", "#b1"\n\t"      \
       "vpsrld	$20, "#b2", "#t1"\n\t"        \
       "vpslld	$12, "#b2", "#b2"\n\t"        \
       "vpxor	"#t1", "#b2", "#b2"\n\t"      \
       "vpsrld	$20, "#b3", "#t1"\n\t"        \
       "vpslld	$12, "#b3", "#b3"\n\t"        \
       "vpxor	"#t1", "#b3", "#b3"\n\t"      \
       "vpaddd	"#b0", "#a0", "#a0"\n\t"      \
       "vpxor	"#a0", "#d0", "#d0"\n\t"      \
       "vmovdqa	"#o1"(%[x]), "#c1"\n\t"       \
       "vpshufb	%[rotl8], "#d0", "#d0"\n\t"   \
       "vpaddd	"#d0", "#c0", "#c0"\n\t"      \
       "vpxor	"#c0", "#b0", "#b0"\n\t"      \
       "vpaddd	"#b1", "#a1", "#a1"\n\t"      \
       "vpxor	"#a1", "#d1", "#d1"\n\t"      \
       "vpshufb	%[rotl8], "#d1", "#d1"\n\t"   \
       "vpaddd	"#d1", "#c1", "#c1"\n\t"      \
       "vpxor	"#c1", "#b1", "#b1"\n\t"      \
       "vpaddd	"#b2", "#a2", "#a2"\n\t"      \
       "vpxor	"#a2", "#d2", "#d2"\n\t"      \
       "vpshufb	%[rotl8], "#d2", "#d2"\n\t"   \
       "vpaddd	"#d2", "#c2", "#c2"\n\t"      \
       "vpxor	"#c2", "#b2", "#b2"\n\t"      \
       "vpaddd	"#b3", "#a3", "#a3"\n\t"      \
       "vpxor	"#a3", "#d3", "#d3"\n\t"      \
       "vpshufb	%[rotl8], "#d3", "#d3"\n\t"   \
       "vpaddd	"#d3", "#c3", "#c3"\n\t"      \
       "vpxor	"#c3", "#b3", "#b3"\n\t"      \
       "vmovdqa	"#c1", "#o1"(%[x])\n\t"       \
       "vpsrld	$25, "#b0", "#t1"\n\t"        \
       "vpslld	 $7, "#b0", "#b0"\n\t"        \
       "vpxor	"#t1", "#b0", "#b0"\n\t"      \
       "vpsrld	$25, "#b1", "#t1"\n\t"        \
       "vpslld	 $7, "#b1", "#b1"\n\t"        \
       "vpxor	"#t1", "#b1", "#b1"\n\t"      \
       "vpsrld	$25, "#b2", "#t1"\n\t"        \
       "vpslld	 $7, "#b2", "#b2"\n\t"        \
       "vpxor	"#t1", "#b2", "#b2"\n\t"      \
       "vpsrld	$25, "#b3", "#t1"\n\t"        \
       "vpslld	 $7, "#b3", "#b3"\n\t"        \
       "vpxor	"#t1", "#b3", "#b3"\n\t"


#define QUARTERROUND_XMM()                                      \
        QUARTERROUND_INTEL_ASM(%%xmm0,%%xmm4,%%xmm8,%%xmm12,    \
                               %%xmm1,%%xmm5,%%xmm9,%%xmm13,    \
                               %%xmm2,%%xmm6,%%xmm10,%%xmm14,   \
                               %%xmm3,%%xmm7,%%xmm11,%%xmm15,   \
                               %%xmm11,48)
#define QUARTERROUND_XMM_2()                                    \
        QUARTERROUND_INTEL_ASM_2(%%xmm0,%%xmm5,%%xmm10,%%xmm15, \
                                 %%xmm1,%%xmm6,%%xmm11,%%xmm12, \
                                 %%xmm2,%%xmm7,%%xmm8,%%xmm13,  \
                                 %%xmm3,%%xmm4,%%xmm9,%%xmm14,  \
                                 %%xmm11,48)

#define QUARTERROUND_YMM()                                      \
        QUARTERROUND_INTEL_ASM(%%ymm0,%%ymm4,%%ymm8,%%ymm12,    \
                               %%ymm1,%%ymm5,%%ymm9,%%ymm13,    \
                               %%ymm2,%%ymm6,%%ymm10,%%ymm14,   \
                               %%ymm3,%%ymm7,%%ymm11,%%ymm15,   \
                               %%ymm11,96)
#define QUARTERROUND_YMM_2()                                    \
        QUARTERROUND_INTEL_ASM_2(%%ymm0,%%ymm5,%%ymm10,%%ymm15, \
                                 %%ymm1,%%ymm6,%%ymm11,%%ymm12, \
                                 %%ymm2,%%ymm7,%%ymm8,%%ymm13,  \
                                 %%ymm3,%%ymm4,%%ymm9,%%ymm14,  \
                                 %%ymm11,96)

/**
  * Set up iv(nonce). Earlier versions used 64 bits instead of 96, this version
  * uses the typical AEAD 96 bit nonce and can do record sizes of 256 GB.
  */
int wc_Chacha_SetIV(ChaCha* ctx, const byte* inIv, word32 counter)
{
    word32 temp[CHACHA_IV_WORDS];/* used for alignment of memory */

#ifdef CHACHA_AEAD_TEST
    word32 i;
    printf("NONCE : ");
    for (i = 0; i < CHACHA_IV_BYTES; i++) {
        printf("%02x", inIv[i]);
    }
    printf("\n\n");
#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(temp, inIv, CHACHA_IV_BYTES);

    ctx->X[CHACHA_IV_BYTES+0] = counter;           /* block counter */
    ctx->X[CHACHA_IV_BYTES+1] = LITTLE32(temp[0]); /* fixed variable from nonce */
    ctx->X[CHACHA_IV_BYTES+2] = LITTLE32(temp[1]); /* counter from nonce */
    ctx->X[CHACHA_IV_BYTES+3] = LITTLE32(temp[2]); /* counter from nonce */

    return 0;
}

/* "expand 32-byte k" as unsigned 32 byte */
static const word32 sigma[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
/* "expand 16-byte k" as unsigned 16 byte */
static const word32 tau[4] = {0x61707865, 0x3120646e, 0x79622d36, 0x6b206574};

/**
  * Key setup. 8 word iv (nonce)
  */
int wc_Chacha_SetKey(ChaCha* ctx, const byte* key, word32 keySz)
{
    const word32* constants;
    const byte*   k;

#ifdef XSTREAM_ALIGN
    word32 alignKey[8];
#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    if (keySz != (CHACHA_MAX_KEY_SZ/2) && keySz != CHACHA_MAX_KEY_SZ)
        return BAD_FUNC_ARG;

#ifdef XSTREAM_ALIGN
    if ((wolfssl_word)key % 4) {
        WOLFSSL_MSG("wc_ChachaSetKey unaligned key");
        XMEMCPY(alignKey, key, keySz);
        k = (byte*)alignKey;
    }
    else {
        k = key;
    }
#else
    k = key;
#endif /* XSTREAM_ALIGN */

#ifdef CHACHA_AEAD_TEST
    word32 i;
    printf("ChaCha key used :\n");
    for (i = 0; i < keySz; i++) {
        printf("%02x", key[i]);
        if ((i + 1) % 8 == 0)
           printf("\n");
    }
    printf("\n\n");
#endif

    ctx->X[4] = U8TO32_LITTLE(k +  0);
    ctx->X[5] = U8TO32_LITTLE(k +  4);
    ctx->X[6] = U8TO32_LITTLE(k +  8);
    ctx->X[7] = U8TO32_LITTLE(k + 12);
    if (keySz == CHACHA_MAX_KEY_SZ) {
        k += 16;
        constants = sigma;
    }
    else {
        constants = tau;
    }
    ctx->X[ 8] = U8TO32_LITTLE(k +  0);
    ctx->X[ 9] = U8TO32_LITTLE(k +  4);
    ctx->X[10] = U8TO32_LITTLE(k +  8);
    ctx->X[11] = U8TO32_LITTLE(k + 12);
    ctx->X[ 0] = constants[0];
    ctx->X[ 1] = constants[1];
    ctx->X[ 2] = constants[2];
    ctx->X[ 3] = constants[3];

    return 0;
}

/**
  * Converts word into bytes with rotations having been done.
  */
static INLINE void wc_Chacha_wordtobyte(word32 output[CHACHA_CHUNK_WORDS],
    const word32 input[CHACHA_CHUNK_WORDS])
{
    word32 x[CHACHA_CHUNK_WORDS];
    word32 i;

    for (i = 0; i < CHACHA_CHUNK_WORDS; i++) {
        x[i] = input[i];
    }

    for (i = (ROUNDS); i > 0; i -= 2) {
        QUARTERROUND(0, 4,  8, 12)
        QUARTERROUND(1, 5,  9, 13)
        QUARTERROUND(2, 6, 10, 14)
        QUARTERROUND(3, 7, 11, 15)
        QUARTERROUND(0, 5, 10, 15)
        QUARTERROUND(1, 6, 11, 12)
        QUARTERROUND(2, 7,  8, 13)
        QUARTERROUND(3, 4,  9, 14)
    }

    for (i = 0; i < CHACHA_CHUNK_WORDS; i++) {
        x[i] = PLUS(x[i], input[i]);
    }

    for (i = 0; i < CHACHA_CHUNK_WORDS; i++) {
        output[i] = LITTLE32(x[i]);
    }
}


#ifdef USE_INTEL_CHACHA_SPEEDUP

#define QUARTERROUND_2_X64(r11, r12, r13, r14, r21, r22, r23, r24) \
        "addl	"#r12", "#r11"\n\t"                                \
        "addl	"#r22", "#r21"\n\t"                                \
        "xorl	"#r11", "#r14"\n\t"                                \
        "xorl	"#r21", "#r24"\n\t"                                \
        "roll	$16, "#r14"\n\t"                                   \
        "roll	$16, "#r24"\n\t"                                   \
        "addl	"#r14", "#r13"\n\t"                                \
        "addl	"#r24", "#r23"\n\t"                                \
        "xorl	"#r13", "#r12"\n\t"                                \
        "xorl	"#r23", "#r22"\n\t"                                \
        "roll	$12, "#r12"\n\t"                                   \
        "roll	$12, "#r22"\n\t"                                   \
        "addl	"#r12", "#r11"\n\t"                                \
        "addl	"#r22", "#r21"\n\t"                                \
        "xorl	"#r11", "#r14"\n\t"                                \
        "xorl	"#r21", "#r24"\n\t"                                \
        "roll	$8, "#r14"\n\t"                                    \
        "roll	$8, "#r24"\n\t"                                    \
        "addl	"#r14", "#r13"\n\t"                                \
        "addl	"#r24", "#r23"\n\t"                                \
        "xorl	"#r13", "#r12"\n\t"                                \
        "xorl	"#r23", "#r22"\n\t"                                \
        "roll	$7, "#r12"\n\t"                                    \
        "roll	$7, "#r22"\n\t"                                    \

#define CHACHA_CRYPT_X64()                                                     \
        "subq	$40, %%rsp\n\t"                                                \
        "movq	32(%[input]), %%rax\n\t"                                       \
        "movq	40(%[input]), %%rdx\n\t"                                       \
        "movq	%%rax,  8(%%rsp)\n\t"                                          \
        "movq	%%rdx, 16(%%rsp)\n\t"                                          \
        "movl	 0(%[input]), %%eax\n\t"                                       \
        "movl	 4(%[input]), %%ebx\n\t"                                       \
        "movl	 8(%[input]), %%ecx\n\t"                                       \
        "movl	12(%[input]), %%edx\n\t"                                       \
        "movl	16(%[input]), %%r8d\n\t"                                       \
        "movl	20(%[input]), %%r9d\n\t"                                       \
        "movl	24(%[input]), %%r10d\n\t"                                      \
        "movl	28(%[input]), %%r11d\n\t"                                      \
        "movl	48(%[input]), %%r12d\n\t"                                      \
        "movl	52(%[input]), %%r13d\n\t"                                      \
        "movl	56(%[input]), %%r14d\n\t"                                      \
        "movl	60(%[input]), %%r15d\n\t"                                      \
        "movb	$10, (%%rsp)\n\t"                                              \
        "movq	%%rsi, 32(%%rsp)\n\t"                                          \
        "movq	%%rdi, 24(%%rsp)\n\t"                                          \
        "movl	 8(%%rsp), %%esi\n\t"                                          \
        "movl	12(%%rsp), %%edi\n\t"                                          \
        "\n"                                                                   \
        "1:\n\t"                                                               \
        QUARTERROUND_2_X64(%%eax,  %%r8d, %%esi, %%r12d,                       \
                           %%ebx,  %%r9d, %%edi, %%r13d)                       \
        "movl	%%esi,  8(%%rsp)\n\t"                                          \
        "movl	%%edi, 12(%%rsp)\n\t"                                          \
        "movl	16(%%rsp), %%esi\n\t"                                          \
        "movl	20(%%rsp), %%edi\n\t"                                          \
        QUARTERROUND_2_X64(%%ecx, %%r10d, %%esi, %%r14d,                       \
                           %%edx, %%r11d, %%edi, %%r15d)                       \
        QUARTERROUND_2_X64(%%eax,  %%r9d, %%esi, %%r15d,                       \
                           %%ebx, %%r10d, %%edi, %%r12d)                       \
        "movl	%%esi, 16(%%rsp)\n\t"                                          \
        "movl	%%edi, 20(%%rsp)\n\t"                                          \
        "movl	 8(%%rsp), %%esi\n\t"                                          \
        "movl	12(%%rsp), %%edi\n\t"                                          \
        QUARTERROUND_2_X64(%%ecx, %%r11d, %%esi, %%r13d,                       \
                           %%edx,  %%r8d, %%edi, %%r14d)                       \
        "decb	(%%rsp)\n\t"                                                   \
        "jnz	1b\n\t"                                                        \
        "movl	%%esi,  8(%%rsp)\n\t"                                          \
        "movl	%%edi, 12(%%rsp)\n\t"                                          \
        "movq	32(%%rsp), %%rsi\n\t"                                          \
        "movq	24(%%rsp), %%rdi\n\t"                                          \
        "addl	 0(%[input]), %%eax\n\t"                                       \
        "addl	 4(%[input]), %%ebx\n\t"                                       \
        "addl	 8(%[input]), %%ecx\n\t"                                       \
        "addl	12(%[input]), %%edx\n\t"                                       \
        "addl	16(%[input]), %%r8d\n\t"                                       \
        "addl	20(%[input]), %%r9d\n\t"                                       \
        "addl	24(%[input]), %%r10d\n\t"                                      \
        "addl	28(%[input]), %%r11d\n\t"                                      \
        "addl	48(%[input]), %%r12d\n\t"                                      \
        "addl	52(%[input]), %%r13d\n\t"                                      \
        "addl	56(%[input]), %%r14d\n\t"                                      \
        "addl	60(%[input]), %%r15d\n\t"                                      \

#define CHACHA_PARTIAL_CHUNK_X64()                                             \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_X64()                                                     \
        "movl	%%eax ,  0(%[c])\n\t"                                          \
        "movl	%%ebx ,  4(%[c])\n\t"                                          \
        "movl	%%ecx ,  8(%[c])\n\t"                                          \
        "movl	%%edx , 12(%[c])\n\t"                                          \
        "movl	%%r8d , 16(%[c])\n\t"                                          \
        "movl	%%r9d , 20(%[c])\n\t"                                          \
        "movl	%%r10d, 24(%[c])\n\t"                                          \
        "movl	%%r11d, 28(%[c])\n\t"                                          \
        "movl	%%r12d, 48(%[c])\n\t"                                          \
        "movl	%%r13d, 52(%[c])\n\t"                                          \
        "movl	%%r14d, 56(%[c])\n\t"                                          \
        "movl	%%r15d, 60(%[c])\n\t"                                          \
        "movl	 8(%%rsp), %%eax\n\t"                                          \
        "movl	12(%%rsp), %%ebx\n\t"                                          \
        "movl	16(%%rsp), %%ecx\n\t"                                          \
        "movl	20(%%rsp), %%edx\n\t"                                          \
        "addl	32(%[input]), %%eax\n\t"                                       \
        "addl	36(%[input]), %%ebx\n\t"                                       \
        "addl	40(%[input]), %%ecx\n\t"                                       \
        "addl	44(%[input]), %%edx\n\t"                                       \
        "movl	%%eax , 32(%[c])\n\t"                                          \
        "movl	%%ebx , 36(%[c])\n\t"                                          \
        "movl	%%ecx , 40(%[c])\n\t"                                          \
        "movl	%%edx , 44(%[c])\n\t"                                          \
        "addl	$1, 48(%[input])\n\t"                                          \
        "addq	$40, %%rsp\n\t"                                                \
        "movq	%[output], %%rax\n\t"                                          \
        "movq	%[m], %%rbx\n\t"                                               \
        "movl	%[bytes], %%r8d\n\t"                                           \
        "xorq	%%rdx, %%rdx\n\t"                                              \
        "movl	%%r8d, %%r9d\n\t"                                              \
        "andl	$7, %%r9d\n\t"                                                 \
        "jz	4f\n\t"                                                        \
        "\n"                                                                   \
        "2:\n\t"                                                               \
        "movzbl	(%[c],%%rdx,1), %%ecx\n\t"                                     \
        "xorb	(%%rbx,%%rdx,1), %%cl\n\t"                                     \
        "movb	%%cl, (%%rax,%%rdx,1)\n\t"                                     \
        "incl	%%edx\n\t"                                                     \
        "cmpl	%%r9d, %%edx\n\t"                                              \
        "jne	2b\n\t"                                                        \
        "je	3f\n\t"                                                        \
        "\n"                                                                   \
        "4:\n\t"                                                               \
        "movq	(%[c],%%rdx,1), %%rcx\n\t"                                     \
        "xorq	(%%rbx,%%rdx,1), %%rcx\n\t"                                    \
        "movq	%%rcx, (%%rax,%%rdx,1)\n\t"                                    \
        "addl	$8, %%edx\n\t"                                                 \
        "\n"                                                                   \
        "3:\n\t"                                                               \
        "cmpl	%%r8d, %%edx\n\t"                                              \
        "jne	4b\n\t"                                                        \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (x),                                   \
          [output] "m" (c), [bytes] "m" (bytes), [m] "m" (m)                   \
        : "eax", "ebx", "ecx", "edx", "r8", "r9", "r10", "r11", "r12", "r13",  \
          "r14", "r15", "memory"                                               \
    )


#define CHACHA_CHUNK_X64()                                                     \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_X64()                                                     \
        "movq	%%rsi, 32(%%rsp)\n\t"                                          \
        "addq	$40, %%rsp\n\t"                                                \
        "movq	%[m], %%rsi\n\t"                                               \
        "subq	$40, %%rsp\n\t"                                                \
        "xorl	 0(%%rsi), %%eax\n\t"                                          \
        "xorl	 4(%%rsi), %%ebx\n\t"                                          \
        "xorl	 8(%%rsi), %%ecx\n\t"                                          \
        "xorl	12(%%rsi), %%edx\n\t"                                          \
        "xorl	16(%%rsi), %%r8d\n\t"                                          \
        "xorl	20(%%rsi), %%r9d\n\t"                                          \
        "xorl	24(%%rsi), %%r10d\n\t"                                         \
        "xorl	28(%%rsi), %%r11d\n\t"                                         \
        "xorl	48(%%rsi), %%r12d\n\t"                                         \
        "xorl	52(%%rsi), %%r13d\n\t"                                         \
        "xorl	56(%%rsi), %%r14d\n\t"                                         \
        "xorl	60(%%rsi), %%r15d\n\t"                                         \
        "movq	32(%%rsp), %%rsi\n\t"                                          \
        "movl	%%eax ,  0(%[c])\n\t"                                          \
        "movl	%%ebx ,  4(%[c])\n\t"                                          \
        "movl	%%ecx ,  8(%[c])\n\t"                                          \
        "movl	%%edx , 12(%[c])\n\t"                                          \
        "movl	%%r8d , 16(%[c])\n\t"                                          \
        "movl	%%r9d , 20(%[c])\n\t"                                          \
        "movl	%%r10d, 24(%[c])\n\t"                                          \
        "movl	%%r11d, 28(%[c])\n\t"                                          \
        "movl	%%r12d, 48(%[c])\n\t"                                          \
        "movl	%%r13d, 52(%[c])\n\t"                                          \
        "movl	%%r14d, 56(%[c])\n\t"                                          \
        "movl	%%r15d, 60(%[c])\n\t"                                          \
        "addq	$40, %%rsp\n\t"                                                \
        "movq	%[m], %%r8\n\t"                                                \
        "subq	$40, %%rsp\n\t"                                                \
        "movl	 8(%%rsp), %%eax\n\t"                                          \
        "movl	12(%%rsp), %%ebx\n\t"                                          \
        "movl	16(%%rsp), %%ecx\n\t"                                          \
        "movl	20(%%rsp), %%edx\n\t"                                          \
        "addl	32(%[input]), %%eax\n\t"                                       \
        "addl	36(%[input]), %%ebx\n\t"                                       \
        "addl	40(%[input]), %%ecx\n\t"                                       \
        "addl	44(%[input]), %%edx\n\t"                                       \
        "xorl	32(%%r8), %%eax\n\t"                                           \
        "xorl	36(%%r8), %%ebx\n\t"                                           \
        "xorl	40(%%r8), %%ecx\n\t"                                           \
        "xorl	44(%%r8), %%edx\n\t"                                           \
        "movl	%%eax , 32(%[c])\n\t"                                          \
        "movl	%%ebx , 36(%[c])\n\t"                                          \
        "movl	%%ecx , 40(%[c])\n\t"                                          \
        "movl	%%edx , 44(%[c])\n\t"                                          \
        "addl	$1, 48(%[input])\n\t"                                          \
        "addq	$40, %%rsp\n\t"                                                \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (c), [m] "m" (m)                       \
        : "eax", "ebx", "ecx", "edx", "r8", "r9", "r10", "r11", "r12", "r13",  \
          "r14", "r15", "memory"                                               \
    )


static void chacha_encrypt_x64(ChaCha* ctx, const byte* m, byte* c,
                               word32 bytes)
{
    word32 x[CHACHA_CHUNK_WORDS];

    if (bytes == 0)
        return;

    for (; bytes >= CHACHA_CHUNK_BYTES;) {
        CHACHA_CHUNK_X64();
        bytes -= CHACHA_CHUNK_BYTES;
        c += CHACHA_CHUNK_BYTES;
        m += CHACHA_CHUNK_BYTES;
    }
    if (bytes > 0) {
        CHACHA_PARTIAL_CHUNK_X64();
    }
}

#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
static const __m128i rotl8 =  { 0x0605040702010003UL,0x0e0d0c0f0a09080bUL };
static const __m128i rotl16 = { 0x0504070601000302UL,0x0d0c0f0e09080b0aUL };
#endif /* HAVE_INTEL_AVX1 || HAVE_INTEL_AVX2 */

#ifdef HAVE_INTEL_AVX1
#define QUARTERROUND_2_AVX()               \
        "paddd	%%xmm1, %%xmm0\n\t"        \
        "pxor	%%xmm0, %%xmm3\n\t"        \
        "pshufb	%[rotl16], %%xmm3\n\t"     \
        "paddd	%%xmm3, %%xmm2\n\t"        \
        "pxor	%%xmm2, %%xmm1\n\t"        \
        "movdqa	%%xmm1, %%xmm4\n\t"        \
        "pslld	$12, %%xmm1\n\t"           \
        "psrld	$20, %%xmm4\n\t"           \
        "pxor	%%xmm4, %%xmm1\n\t"        \
        "paddd	%%xmm1, %%xmm0\n\t"        \
        "pxor	%%xmm0, %%xmm3\n\t"        \
        "pshufb	%[rotl8], %%xmm3\n\t"      \
        "paddd	%%xmm3, %%xmm2\n\t"        \
        "pxor	%%xmm2, %%xmm1\n\t"        \
        "movdqa	%%xmm1, %%xmm4\n\t"        \
        "pslld	$7, %%xmm1\n\t"            \
        "psrld	$25, %%xmm4\n\t"           \
        "pxor	%%xmm4, %%xmm1\n\t"        \
        "# Swap words for next round\n\t"  \
        "pshufd	$0x39, %%xmm1, %%xmm1\n\t" \
        "pshufd	$0x4e, %%xmm2, %%xmm2\n\t" \
        "pshufd	$0x93, %%xmm3, %%xmm3\n\t" \
        "paddd	%%xmm1, %%xmm0\n\t"        \
        "pxor	%%xmm0, %%xmm3\n\t"        \
        "pshufb	%[rotl16], %%xmm3\n\t"     \
        "paddd	%%xmm3, %%xmm2\n\t"        \
        "pxor	%%xmm2, %%xmm1\n\t"        \
        "movdqa	%%xmm1, %%xmm4\n\t"        \
        "pslld	$12, %%xmm1\n\t"           \
        "psrld	$20, %%xmm4\n\t"           \
        "pxor	%%xmm4, %%xmm1\n\t"        \
        "paddd	%%xmm1, %%xmm0\n\t"        \
        "pxor	%%xmm0, %%xmm3\n\t"        \
        "pshufb	%[rotl8], %%xmm3\n\t"      \
        "paddd	%%xmm3, %%xmm2\n\t"        \
        "pxor	%%xmm2, %%xmm1\n\t"        \
        "movdqa	%%xmm1, %%xmm4\n\t"        \
        "pslld	$7, %%xmm1\n\t"            \
        "psrld	$25, %%xmm4\n\t"           \
        "pxor	%%xmm4, %%xmm1\n\t"        \
        "# Swap words back\n\t"            \
        "pshufd	$0x93, %%xmm1, %%xmm1\n\t" \
        "pshufd	$0x4e, %%xmm2, %%xmm2\n\t" \
        "pshufd	$0x39, %%xmm3, %%xmm3\n\t" \

#define CHACHA_CRYPT_AVX()                                                     \
        "movdqu	 0(%[input]), %%xmm0\n\t"                                      \
        "movdqu	16(%[input]), %%xmm1\n\t"                                      \
        "movdqu	32(%[input]), %%xmm2\n\t"                                      \
        "movdqu	48(%[input]), %%xmm3\n\t"                                      \
        "movb	$10, %%al\n\t"                                                 \
        "\n"                                                                   \
        "1:\n\t"                                                               \
        QUARTERROUND_2_AVX()                                                   \
        "decb	%%al\n\t"                                                      \
        "jnz	1b\n\t"                                                        \
        "movdqu	 0(%[input]), %%xmm4\n\t"                                      \
        "movdqu	16(%[input]), %%xmm5\n\t"                                      \
        "movdqu	32(%[input]), %%xmm6\n\t"                                      \
        "movdqu	48(%[input]), %%xmm7\n\t"                                      \
        "paddd	%%xmm4, %%xmm0\n\t"                                            \
        "paddd	%%xmm5, %%xmm1\n\t"                                            \
        "paddd	%%xmm6, %%xmm2\n\t"                                            \
        "paddd	%%xmm7, %%xmm3\n\t"                                            \

#define CHACHA_PARTIAL_CHUNK_AVX()                                             \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_AVX()                                                     \
        "movdqu	%%xmm0,  0(%[c])\n\t"                                          \
        "movdqu	%%xmm1, 16(%[c])\n\t"                                          \
        "movdqu	%%xmm2, 32(%[c])\n\t"                                          \
        "movdqu	%%xmm3, 48(%[c])\n\t"                                          \
        "addl	$1, 48(%[input])\n\t"                                          \
        "movl	%[bytes], %%r8d\n\t"                                           \
        "xorq	%%rdx, %%rdx\n\t"                                              \
        "movl	%%r8d, %%r9d\n\t"                                              \
        "andl	$7, %%r9d\n\t"                                                 \
        "jz	4f\n\t"                                                        \
        "\n"                                                                   \
        "2:\n\t"                                                               \
        "movzbl	(%[c],%%rdx,1), %%ecx\n\t"                                     \
        "xorb	(%[m],%%rdx,1), %%cl\n\t"                                      \
        "movb	%%cl, (%[output],%%rdx,1)\n\t"                                 \
        "incl	%%edx\n\t"                                                     \
        "cmpl	%%r9d, %%edx\n\t"                                              \
        "jne	2b\n\t"                                                        \
        "je	3f\n\t"                                                        \
        "\n"                                                                   \
        "4:\n\t"                                                               \
        "movq	(%[c],%%rdx,1), %%rcx\n\t"                                     \
        "xorq	(%[m],%%rdx,1), %%rcx\n\t"                                     \
        "movq	%%rcx, (%[output],%%rdx,1)\n\t"                                \
        "addl	$8, %%edx\n\t"                                                 \
        "\n"                                                                   \
        "3:\n\t"                                                               \
        "cmpl	%%r8d, %%edx\n\t"                                              \
        "jne	4b\n\t"                                                        \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (x),                                   \
          [output] "r" (c), [bytes] "r" (bytes), [m] "r" (m),                  \
          [rotl8] "xrm" (rotl8), [rotl16] "xrm" (rotl16)                       \
        : "eax", "ecx", "edx", "r8", "r9", "memory",                           \
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"       \
    )


#define CHACHA_CHUNK_AVX()                                                     \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_AVX()                                                     \
        "movdqu	 0(%[m]), %%xmm4\n\t"                                          \
        "movdqu	16(%[m]), %%xmm5\n\t"                                          \
        "movdqu	32(%[m]), %%xmm6\n\t"                                          \
        "movdqu	48(%[m]), %%xmm7\n\t"                                          \
        "pxor	%%xmm4, %%xmm0\n\t"                                            \
        "pxor	%%xmm5, %%xmm1\n\t"                                            \
        "pxor	%%xmm6, %%xmm2\n\t"                                            \
        "pxor	%%xmm7, %%xmm3\n\t"                                            \
        "movdqu	%%xmm0,  0(%[c])\n\t"                                          \
        "movdqu	%%xmm1, 16(%[c])\n\t"                                          \
        "movdqu	%%xmm2, 32(%[c])\n\t"                                          \
        "movdqu	%%xmm3, 48(%[c])\n\t"                                          \
        "addl	$1, 48(%[input])\n\t"                                          \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (c), [m] "r" (m),                      \
          [rotl8] "xrm" (rotl8), [rotl16] "xrm" (rotl16)                       \
        : "rax", "memory",                                                     \
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"       \
    )

CHACHA20_NOINLINE static void chacha_encrypt_avx(ChaCha* ctx, const byte* m,
                                                 byte* c, word32 bytes)
{
    ALIGN128 word32 X[4*CHACHA_CHUNK_WORDS]; /* used to make sure aligned */
    ALIGN128 word32 x[2*CHACHA_CHUNK_WORDS]; /* used to make sure aligned */
    word32 cnt = 0;
    static const __m128i add =    { 0x0000000100000000UL,0x0000000300000002UL };
    static const __m128i four =   { 0x0000000400000004UL,0x0000000400000004UL };

    if (bytes == 0)
        return;

    __asm__ __volatile__ (
       "movl	%[bytes], %[cnt]\n\t"
       "shrl	$8, %[cnt]\n\t"
       "jz      L_end128\n\t"

       "vpshufd	$0,   (%[key]), %%xmm0\n\t"
       "vpshufd	$0,  4(%[key]), %%xmm1\n\t"
       "vpshufd	$0,  8(%[key]), %%xmm2\n\t"
       "vpshufd	$0, 12(%[key]), %%xmm3\n\t"
       "vpshufd	$0, 16(%[key]), %%xmm4\n\t"
       "vpshufd	$0, 20(%[key]), %%xmm5\n\t"
       "vpshufd	$0, 24(%[key]), %%xmm6\n\t"
       "vpshufd	$0, 28(%[key]), %%xmm7\n\t"
       "vpshufd	$0, 32(%[key]), %%xmm8\n\t"
       "vpshufd	$0, 36(%[key]), %%xmm9\n\t"
       "vpshufd	$0, 40(%[key]), %%xmm10\n\t"
       "vpshufd	$0, 44(%[key]), %%xmm11\n\t"
       "vpshufd	$0, 48(%[key]), %%xmm12\n\t"
       "vpshufd	$0, 52(%[key]), %%xmm13\n\t"
       "vpshufd	$0, 56(%[key]), %%xmm14\n\t"
       "vpshufd	$0, 60(%[key]), %%xmm15\n\t"

       "vpaddd	%[add], %%xmm12, %%xmm12\n\t"

       "vmovdqa	%%xmm0,     (%[X])\n\t"
       "vmovdqa	%%xmm1,   16(%[X])\n\t"
       "vmovdqa	%%xmm2,   32(%[X])\n\t"
       "vmovdqa	%%xmm3,   48(%[X])\n\t"
       "vmovdqa	%%xmm4,   64(%[X])\n\t"
       "vmovdqa	%%xmm5,   80(%[X])\n\t"
       "vmovdqa	%%xmm6,   96(%[X])\n\t"
       "vmovdqa	%%xmm7,  112(%[X])\n\t"
       "vmovdqa	%%xmm8,  128(%[X])\n\t"
       "vmovdqa	%%xmm9,  144(%[X])\n\t"
       "vmovdqa	%%xmm10, 160(%[X])\n\t"
       "vmovdqa	%%xmm11, 176(%[X])\n\t"
       "vmovdqa	%%xmm12, 192(%[X])\n\t"
       "vmovdqa	%%xmm13, 208(%[X])\n\t"
       "vmovdqa	%%xmm14, 224(%[X])\n\t"
       "vmovdqa	%%xmm15, 240(%[X])\n\t"
       "\n"
   "L_enc128_loop:\n\t"
       "vmovdqa	%%xmm11, 48(%[x])\n\t"
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       QUARTERROUND_XMM()
       QUARTERROUND_XMM_2()
       "vmovdqa	48(%[x]), %%xmm11\n\t"

       "vpaddd	   (%[X]), %%xmm0,  %%xmm0\n\t"
       "vpaddd	 16(%[X]), %%xmm1,  %%xmm1\n\t"
       "vpaddd	 32(%[X]), %%xmm2,  %%xmm2\n\t"
       "vpaddd	 48(%[X]), %%xmm3,  %%xmm3\n\t"
       "vpaddd	 64(%[X]), %%xmm4,  %%xmm4\n\t"
       "vpaddd	 80(%[X]), %%xmm5,  %%xmm5\n\t"
       "vpaddd	 96(%[X]), %%xmm6,  %%xmm6\n\t"
       "vpaddd	112(%[X]), %%xmm7,  %%xmm7\n\t"
       "vpaddd	128(%[X]), %%xmm8,  %%xmm8\n\t"
       "vpaddd	144(%[X]), %%xmm9,  %%xmm9\n\t"
       "vpaddd	160(%[X]), %%xmm10, %%xmm10\n\t"
       "vpaddd	176(%[X]), %%xmm11, %%xmm11\n\t"
       "vpaddd	192(%[X]), %%xmm12, %%xmm12\n\t"
       "vpaddd	208(%[X]), %%xmm13, %%xmm13\n\t"
       "vpaddd	224(%[X]), %%xmm14, %%xmm14\n\t"
       "vpaddd	240(%[X]), %%xmm15, %%xmm15\n\t"

       "vmovdqa	%%xmm8,     (%[x])\n\t"
       "vmovdqa	%%xmm9,   16(%[x])\n\t"
       "vmovdqa	%%xmm10,  32(%[x])\n\t"
       "vmovdqa	%%xmm11,  48(%[x])\n\t"
       "vmovdqa	%%xmm12,  64(%[x])\n\t"
       "vmovdqa	%%xmm13,  80(%[x])\n\t"
       "vmovdqa	%%xmm14,  96(%[x])\n\t"
       "vmovdqa	%%xmm15, 112(%[x])\n\t"

       "vpunpckldq %%xmm1, %%xmm0, %%xmm8\n\t"
       "vpunpckldq %%xmm3, %%xmm2, %%xmm9\n\t"
       "vpunpckhdq %%xmm1, %%xmm0, %%xmm12\n\t"
       "vpunpckhdq %%xmm3, %%xmm2, %%xmm13\n\t"
       "vpunpckldq %%xmm5, %%xmm4, %%xmm10\n\t"
       "vpunpckldq %%xmm7, %%xmm6, %%xmm11\n\t"
       "vpunpckhdq %%xmm5, %%xmm4, %%xmm14\n\t"
       "vpunpckhdq %%xmm7, %%xmm6, %%xmm15\n\t"
       "vpunpcklqdq %%xmm9,  %%xmm8,  %%xmm0\n\t"
       "vpunpcklqdq %%xmm11, %%xmm10, %%xmm1\n\t"
       "vpunpckhqdq %%xmm9,  %%xmm8,  %%xmm2\n\t"
       "vpunpckhqdq %%xmm11, %%xmm10, %%xmm3\n\t"
       "vpunpcklqdq %%xmm13, %%xmm12, %%xmm4\n\t"
       "vpunpcklqdq %%xmm15, %%xmm14, %%xmm5\n\t"
       "vpunpckhqdq %%xmm13, %%xmm12, %%xmm6\n\t"
       "vpunpckhqdq %%xmm15, %%xmm14, %%xmm7\n\t"
       "vmovdqu	   (%[in]), %%xmm8\n\t"
       "vmovdqu	 16(%[in]), %%xmm9\n\t"
       "vmovdqu	 64(%[in]), %%xmm10\n\t"
       "vmovdqu	 80(%[in]), %%xmm11\n\t"
       "vmovdqu	128(%[in]), %%xmm12\n\t"
       "vmovdqu	144(%[in]), %%xmm13\n\t"
       "vmovdqu	192(%[in]), %%xmm14\n\t"
       "vmovdqu	208(%[in]), %%xmm15\n\t"
       "vpxor	%%xmm8,  %%xmm0, %%xmm0\n\t"
       "vpxor	%%xmm9,  %%xmm1, %%xmm1\n\t"
       "vpxor	%%xmm10, %%xmm2, %%xmm2\n\t"
       "vpxor	%%xmm11, %%xmm3, %%xmm3\n\t"
       "vpxor	%%xmm12, %%xmm4, %%xmm4\n\t"
       "vpxor	%%xmm13, %%xmm5, %%xmm5\n\t"
       "vpxor	%%xmm14, %%xmm6, %%xmm6\n\t"
       "vpxor	%%xmm15, %%xmm7, %%xmm7\n\t"
       "vmovdqu	%%xmm0,    (%[out])\n\t"
       "vmovdqu	%%xmm1,  16(%[out])\n\t"
       "vmovdqu	%%xmm2,  64(%[out])\n\t"
       "vmovdqu	%%xmm3,  80(%[out])\n\t"
       "vmovdqu	%%xmm4, 128(%[out])\n\t"
       "vmovdqu	%%xmm5, 144(%[out])\n\t"
       "vmovdqu	%%xmm6, 192(%[out])\n\t"
       "vmovdqu	%%xmm7, 208(%[out])\n\t"

       "vmovdqa	   (%[x]), %%xmm0\n\t"
       "vmovdqa	 16(%[x]), %%xmm1\n\t"
       "vmovdqa	 32(%[x]), %%xmm2\n\t"
       "vmovdqa	 48(%[x]), %%xmm3\n\t"
       "vmovdqa	 64(%[x]), %%xmm4\n\t"
       "vmovdqa	 80(%[x]), %%xmm5\n\t"
       "vmovdqa	 96(%[x]), %%xmm6\n\t"
       "vmovdqa	112(%[x]), %%xmm7\n\t"

       "vpunpckldq %%xmm1, %%xmm0, %%xmm8\n\t"
       "vpunpckldq %%xmm3, %%xmm2, %%xmm9\n\t"
       "vpunpckhdq %%xmm1, %%xmm0, %%xmm12\n\t"
       "vpunpckhdq %%xmm3, %%xmm2, %%xmm13\n\t"
       "vpunpckldq %%xmm5, %%xmm4, %%xmm10\n\t"
       "vpunpckldq %%xmm7, %%xmm6, %%xmm11\n\t"
       "vpunpckhdq %%xmm5, %%xmm4, %%xmm14\n\t"
       "vpunpckhdq %%xmm7, %%xmm6, %%xmm15\n\t"
       "vpunpcklqdq %%xmm9,  %%xmm8,  %%xmm0\n\t"
       "vpunpcklqdq %%xmm11, %%xmm10, %%xmm1\n\t"
       "vpunpckhqdq %%xmm9,  %%xmm8,  %%xmm2\n\t"
       "vpunpckhqdq %%xmm11, %%xmm10, %%xmm3\n\t"
       "vpunpcklqdq %%xmm13, %%xmm12, %%xmm4\n\t"
       "vpunpcklqdq %%xmm15, %%xmm14, %%xmm5\n\t"
       "vpunpckhqdq %%xmm13, %%xmm12, %%xmm6\n\t"
       "vpunpckhqdq %%xmm15, %%xmm14, %%xmm7\n\t"
       "vmovdqu	 32(%[in]), %%xmm8\n\t"
       "vmovdqu	 48(%[in]), %%xmm9\n\t"
       "vmovdqu	 96(%[in]), %%xmm10\n\t"
       "vmovdqu	112(%[in]), %%xmm11\n\t"
       "vmovdqu	160(%[in]), %%xmm12\n\t"
       "vmovdqu	176(%[in]), %%xmm13\n\t"
       "vmovdqu	224(%[in]), %%xmm14\n\t"
       "vmovdqu	240(%[in]), %%xmm15\n\t"
       "vpxor	%%xmm8,  %%xmm0, %%xmm0\n\t"
       "vpxor	%%xmm9,  %%xmm1, %%xmm1\n\t"
       "vpxor	%%xmm10, %%xmm2, %%xmm2\n\t"
       "vpxor	%%xmm11, %%xmm3, %%xmm3\n\t"
       "vpxor	%%xmm12, %%xmm4, %%xmm4\n\t"
       "vpxor	%%xmm13, %%xmm5, %%xmm5\n\t"
       "vpxor	%%xmm14, %%xmm6, %%xmm6\n\t"
       "vpxor	%%xmm15, %%xmm7, %%xmm7\n\t"
       "vmovdqu	%%xmm0,  32(%[out])\n\t"
       "vmovdqu	%%xmm1,  48(%[out])\n\t"
       "vmovdqu	%%xmm2,  96(%[out])\n\t"
       "vmovdqu	%%xmm3, 112(%[out])\n\t"
       "vmovdqu	%%xmm4, 160(%[out])\n\t"
       "vmovdqu	%%xmm5, 176(%[out])\n\t"
       "vmovdqu	%%xmm6, 224(%[out])\n\t"
       "vmovdqu	%%xmm7, 240(%[out])\n\t"

       "vmovdqa	192(%[X]), %%xmm12\n\t"
       "add	$256, %[in]\n\t"
       "add	$256, %[out]\n\t"
       "vpaddd	%[four], %%xmm12, %%xmm12\n\t"
       "sub	$256, %[bytes]\n\t"
       "vmovdqa	%%xmm12, 192(%[X])\n\t"
       "cmp	$256, %[bytes]\n\t"
       "jl	L_done\n\t"

       "vmovdqa	   (%[X]), %%xmm0\n\t"
       "vmovdqa	 16(%[X]), %%xmm1\n\t"
       "vmovdqa	 32(%[X]), %%xmm2\n\t"
       "vmovdqa	 48(%[X]), %%xmm3\n\t"
       "vmovdqa	 64(%[X]), %%xmm4\n\t"
       "vmovdqa	 80(%[X]), %%xmm5\n\t"
       "vmovdqa	 96(%[X]), %%xmm6\n\t"
       "vmovdqa	112(%[X]), %%xmm7\n\t"
       "vmovdqa	128(%[X]), %%xmm8\n\t"
       "vmovdqa	144(%[X]), %%xmm9\n\t"
       "vmovdqa	160(%[X]), %%xmm10\n\t"
       "vmovdqa	176(%[X]), %%xmm11\n\t"
       "vmovdqa	192(%[X]), %%xmm12\n\t"
       "vmovdqa	208(%[X]), %%xmm13\n\t"
       "vmovdqa	224(%[X]), %%xmm14\n\t"
       "vmovdqa	240(%[X]), %%xmm15\n\t"
       "jmp	L_enc128_loop\n\t"

       "\n"
   "L_done:\n\t"

       "shl	$2, %[cnt]\n\t"
       "add	48(%[key]), %[cnt]\n\t"
       "movl	%[cnt], 48(%[key])\n\t"
       "\n"
   "L_end128:\n\t"
       : [bytes] "+r" (bytes), [cnt] "+r" (cnt),
         [in] "+r" (m), [out] "+r" (c)
       : [X] "r" (X), [x] "r" (x), [key] "r" (ctx->X),
         [add] "xrm" (add), [four] "xrm" (four),
         [rotl8] "xrm" (rotl8), [rotl16] "xrm" (rotl16)
       : "xmm0", "xmm1", "xmm2", "xmm3",
         "xmm4", "xmm5", "xmm6", "xmm7",
         "xmm8", "xmm9", "xmm10", "xmm11",
         "xmm12", "xmm13", "xmm14", "xmm15", "memory"
    );

    for (; bytes >= CHACHA_CHUNK_BYTES;) {
        CHACHA_CHUNK_AVX();
        bytes -= CHACHA_CHUNK_BYTES;
        c += CHACHA_CHUNK_BYTES;
        m += CHACHA_CHUNK_BYTES;
    }
    if (bytes > 0) {
        CHACHA_PARTIAL_CHUNK_AVX();
    }
}
#endif /* HAVE_INTEL_AVX1 */

#ifdef HAVE_INTEL_AVX2
#define QUARTERROUND_2_AVX2()                          \
        "vpaddd		%%xmm1, %%xmm0, %%xmm0\n\t"    \
        "vpxor		%%xmm0, %%xmm3, %%xmm3\n\t"    \
        "vpshufb	%[rotl16], %%xmm3, %%xmm3\n\t" \
        "vpaddd		%%xmm3, %%xmm2, %%xmm2\n\t"    \
        "vpxor		%%xmm2, %%xmm1, %%xmm1\n\t"    \
        "vpsrld		$20, %%xmm1, %%xmm4\n\t"       \
        "vpslld		$12, %%xmm1, %%xmm1\n\t"       \
        "vpxor		%%xmm4, %%xmm1, %%xmm1\n\t"    \
        "vpaddd		%%xmm1, %%xmm0, %%xmm0\n\t"    \
        "vpxor		%%xmm0, %%xmm3, %%xmm3\n\t"    \
        "vpshufb	%[rotl8], %%xmm3, %%xmm3\n\t"  \
        "vpaddd		%%xmm3, %%xmm2, %%xmm2\n\t"    \
        "vpxor		%%xmm2, %%xmm1, %%xmm1\n\t"    \
        "vpsrld		$25, %%xmm1, %%xmm4\n\t"       \
        "vpslld		$7, %%xmm1, %%xmm1\n\t"        \
        "vpxor		%%xmm4, %%xmm1, %%xmm1\n\t"    \
        "# Swap words for next round\n\t"              \
        "vpshufd	$0x39, %%xmm1, %%xmm1\n\t"     \
        "vpshufd	$0x4e, %%xmm2, %%xmm2\n\t"     \
        "vpshufd	$0x93, %%xmm3, %%xmm3\n\t"     \
        "vpaddd		%%xmm1, %%xmm0, %%xmm0\n\t"    \
        "vpxor		%%xmm0, %%xmm3, %%xmm3\n\t"    \
        "vpshufb	%[rotl16], %%xmm3, %%xmm3\n\t" \
        "vpaddd		%%xmm3, %%xmm2, %%xmm2\n\t"    \
        "vpxor		%%xmm2, %%xmm1, %%xmm1\n\t"    \
        "vpsrld		$20, %%xmm1, %%xmm4\n\t"       \
        "vpslld		$12, %%xmm1, %%xmm1\n\t"       \
        "vpxor		%%xmm4, %%xmm1, %%xmm1\n\t"    \
        "vpaddd		%%xmm1, %%xmm0, %%xmm0\n\t"    \
        "vpxor		%%xmm0, %%xmm3, %%xmm3\n\t"    \
        "vpshufb	%[rotl8], %%xmm3, %%xmm3\n\t"  \
        "vpaddd		%%xmm3, %%xmm2, %%xmm2\n\t"    \
        "vpxor		%%xmm2, %%xmm1, %%xmm1\n\t"    \
        "vpsrld		$25, %%Xmm1, %%xmm4\n\t"       \
        "vpslld		$7, %%xmm1, %%xmm1\n\t"        \
        "vpxor		%%xmm4, %%xmm1, %%xmm1\n\t"    \
        "# Swap words back\n\t"                        \
        "vpshufd	$0x93, %%xmm1, %%xmm1\n\t"     \
        "vpshufd	$0x4e, %%xmm2, %%xmm2\n\t"     \
        "vpshufd	$0x39, %%xmm3, %%xmm3\n\t"     \

#define CHACHA_CRYPT_AVX2()                                                    \
        "vmovdqu	 0(%[input]), %%xmm8\n\t"                              \
        "vmovdqu	16(%[input]), %%xmm9\n\t"                              \
        "vmovdqu	32(%[input]), %%xmm10\n\t"                             \
        "vmovdqu	48(%[input]), %%xmm11\n\t"                             \
        "vmovdqu	%%xmm8, %%xmm0\n\t"                                    \
        "vmovdqu	%%xmm9, %%xmm1\n\t"                                    \
        "vmovdqu	%%xmm10, %%xmm2\n\t"                                   \
        "vmovdqu	%%xmm11, %%xmm3\n\t"                                   \
        "movb		$10, %%al\n\t"                                         \
        "\n"                                                                   \
        "1:\n\t"                                                               \
        QUARTERROUND_2_AVX2()                                                  \
        "decb		%%al\n\t"                                              \
        "jnz		1b\n\t"                                                \
        "vpaddd		%%xmm8, %%xmm0, %%xmm0\n\t"                            \
        "vpaddd		%%xmm9, %%xmm1, %%xmm1\n\t"                            \
        "vpaddd		%%xmm10, %%xmm2, %%xmm2\n\t"                           \
        "vpaddd		%%xmm11, %%xmm3, %%xmm3\n\t"                           \

#define CHACHA_PARTIAL_CHUNK_AVX2()                                            \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_AVX2()                                                    \
        "vmovdqu	%%xmm0,  0(%[c])\n\t"                                  \
        "vmovdqu	%%xmm1, 16(%[c])\n\t"                                  \
        "vmovdqu	%%xmm2, 32(%[c])\n\t"                                  \
        "vmovdqu	%%xmm3, 48(%[c])\n\t"                                  \
        "addl		$1, 48(%[input])\n\t"                                  \
        "movl		%[bytes], %%r8d\n\t"                                   \
        "xorq		%%rdx, %%rdx\n\t"                                      \
        "movl		%%r8d, %%r9d\n\t"                                      \
        "andl		$7, %%r9d\n\t"                                         \
        "jz		4f\n\t"                                                \
        "\n"                                                                   \
        "2:\n\t"                                                               \
        "movzbl		(%[c],%%rdx,1), %%ecx\n\t"                             \
        "xorb		(%[m],%%rdx,1), %%cl\n\t"                              \
        "movb		%%cl, (%[output],%%rdx,1)\n\t"                         \
        "incl		%%edx\n\t"                                             \
        "cmpl		%%r9d, %%edx\n\t"                                      \
        "jne		2b\n\t"                                                \
        "je		3f\n\t"                                                \
        "\n"                                                                   \
        "4:\n\t"                                                               \
        "movq		(%[c],%%rdx,1), %%rcx\n\t"                             \
        "xorq		(%[m],%%rdx,1), %%rcx\n\t"                             \
        "movq		%%rcx, (%[output],%%rdx,1)\n\t"                        \
        "addl		$8, %%edx\n\t"                                         \
        "\n"                                                                   \
        "3:\n\t"                                                               \
        "cmpl		%%r8d, %%edx\n\t"                                      \
        "jne		4b\n\t"                                                \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (x),                                   \
          [output] "r" (c), [bytes] "r" (bytes), [m] "r" (m),                  \
          [rotl8] "xrm" (rotl8), [rotl16] "xrm" (rotl16)                       \
        : "eax", "ecx", "edx", "r8", "r9", "memory",                           \
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",      \
          "xmm8", "xmm9", "xmm10", "xmm11"                                     \
    )


#define CHACHA_CHUNK_AVX2()                                                    \
    __asm__ __volatile__ (                                                     \
        CHACHA_CRYPT_AVX2()                                                    \
        "vmovdqu	 0(%[m]), %%xmm4\n\t"                                  \
        "vmovdqu	16(%[m]), %%xmm5\n\t"                                  \
        "vmovdqu	32(%[m]), %%xmm6\n\t"                                  \
        "vmovdqu	48(%[m]), %%xmm7\n\t"                                  \
        "vpxor		%%xmm4, %%xmm0, %%xmm0\n\t"                            \
        "vpxor		%%xmm5, %%xmm1, %%xmm1\n\t"                            \
        "vpxor		%%xmm6, %%xmm2, %%xmm2\n\t"                            \
        "vpxor		%%xmm7, %%xmm3, %%xmm3\n\t"                            \
        "vmovdqu	%%xmm0,  0(%[c])\n\t"                                  \
        "vmovdqu	%%xmm1, 16(%[c])\n\t"                                  \
        "vmovdqu	%%xmm2, 32(%[c])\n\t"                                  \
        "vmovdqu	%%xmm3, 48(%[c])\n\t"                                  \
        "addl		$1, 48(%[input])\n\t"                                  \
        :                                                                      \
        : [input] "r" (ctx->X), [c] "r" (c), [m] "r" (m),                      \
          [rotl8] "xrm" (rotl8), [rotl16] "xrm" (rotl16)                       \
        : "rax", "memory",                                                     \
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",      \
          "xmm8", "xmm9", "xmm10", "xmm11"                                     \
    )


static void chacha_encrypt_avx2(ChaCha* ctx, const byte* m, byte* c,
                                 word32 bytes)
{
    ALIGN256 word32 X[8*CHACHA_CHUNK_WORDS]; /* used to make sure aligned */
    ALIGN256 word32 x[4*CHACHA_CHUNK_WORDS]; /* used to make sure aligned */
    word32 cnt = 0;
    static const __m256i add    = { 0x0000000100000000UL,0x0000000300000002UL,
                                    0x0000000500000004UL,0x0000000700000006UL };
    static const __m256i eight  = { 0x0000000800000008UL,0x0000000800000008UL,
                                    0x0000000800000008UL,0x0000000800000008UL };
    static const __m256i rotl8_256  =
                                  { 0x0605040702010003UL,0x0e0d0c0f0a09080bUL,
                                    0x0605040702010003UL,0x0e0d0c0f0a09080bUL };
    static const __m256i rotl16_256 =
                                  { 0x0504070601000302UL,0x0d0c0f0e09080b0aUL,
                                    0x0504070601000302UL,0x0d0c0f0e09080b0aUL };

    if (bytes == 0)
        return;

    __asm__ __volatile__ (
       "movl	%[bytes], %[cnt]\n\t"
       "shrl	$9, %[cnt]\n\t"
       "jz      L_end256\n\t"

       "vpbroadcastd	  (%[key]), %%ymm0\n\t"
       "vpbroadcastd	 4(%[key]), %%ymm1\n\t"
       "vpbroadcastd	 8(%[key]), %%ymm2\n\t"
       "vpbroadcastd	12(%[key]), %%ymm3\n\t"
       "vpbroadcastd	16(%[key]), %%ymm4\n\t"
       "vpbroadcastd	20(%[key]), %%ymm5\n\t"
       "vpbroadcastd	24(%[key]), %%ymm6\n\t"
       "vpbroadcastd	28(%[key]), %%ymm7\n\t"
       "vpbroadcastd	32(%[key]), %%ymm8\n\t"
       "vpbroadcastd	36(%[key]), %%ymm9\n\t"
       "vpbroadcastd	40(%[key]), %%ymm10\n\t"
       "vpbroadcastd	44(%[key]), %%ymm11\n\t"
       "vpbroadcastd	48(%[key]), %%ymm12\n\t"
       "vpbroadcastd	52(%[key]), %%ymm13\n\t"
       "vpbroadcastd	56(%[key]), %%ymm14\n\t"
       "vpbroadcastd	60(%[key]), %%ymm15\n\t"

       "vpaddd	%[add], %%ymm12, %%ymm12\n\t"

       "vmovdqa	%%ymm0,     (%[X])\n\t"
       "vmovdqa	%%ymm1,   32(%[X])\n\t"
       "vmovdqa	%%ymm2,   64(%[X])\n\t"
       "vmovdqa	%%ymm3,   96(%[X])\n\t"
       "vmovdqa	%%ymm4,  128(%[X])\n\t"
       "vmovdqa	%%ymm5,  160(%[X])\n\t"
       "vmovdqa	%%ymm6,  192(%[X])\n\t"
       "vmovdqa	%%ymm7,  224(%[X])\n\t"
       "vmovdqa	%%ymm8,  256(%[X])\n\t"
       "vmovdqa	%%ymm9,  288(%[X])\n\t"
       "vmovdqa	%%ymm10, 320(%[X])\n\t"
       "vmovdqa	%%ymm11, 352(%[X])\n\t"
       "vmovdqa	%%ymm12, 384(%[X])\n\t"
       "vmovdqa	%%ymm13, 416(%[X])\n\t"
       "vmovdqa	%%ymm14, 448(%[X])\n\t"
       "vmovdqa	%%ymm15, 480(%[X])\n\t"
       "\n"
   "L_enc256_loop:\n\t"
       "vmovdqa	%%ymm11, 96(%[x])\n\t"
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       QUARTERROUND_YMM()
       QUARTERROUND_YMM_2()
       "vmovdqa	96(%[x]), %%ymm11\n\t"

       "vpaddd	   (%[X]), %%ymm0,  %%ymm0\n\t"
       "vpaddd	 32(%[X]), %%ymm1,  %%ymm1\n\t"
       "vpaddd	 64(%[X]), %%ymm2,  %%ymm2\n\t"
       "vpaddd	 96(%[X]), %%ymm3,  %%ymm3\n\t"
       "vpaddd	128(%[X]), %%ymm4,  %%ymm4\n\t"
       "vpaddd	160(%[X]), %%ymm5,  %%ymm5\n\t"
       "vpaddd	192(%[X]), %%ymm6,  %%ymm6\n\t"
       "vpaddd	224(%[X]), %%ymm7,  %%ymm7\n\t"
       "vpaddd	256(%[X]), %%ymm8,  %%ymm8\n\t"
       "vpaddd	288(%[X]), %%ymm9,  %%ymm9\n\t"
       "vpaddd	320(%[X]), %%ymm10, %%ymm10\n\t"
       "vpaddd	352(%[X]), %%ymm11, %%ymm11\n\t"
       "vpaddd	384(%[X]), %%ymm12, %%ymm12\n\t"
       "vpaddd	416(%[X]), %%ymm13, %%ymm13\n\t"
       "vpaddd	448(%[X]), %%ymm14, %%ymm14\n\t"
       "vpaddd	480(%[X]), %%ymm15, %%ymm15\n\t"

       "vmovdqa	%%ymm8,     (%[x])\n\t"
       "vmovdqa	%%ymm9,   32(%[x])\n\t"
       "vmovdqa	%%ymm10,  64(%[x])\n\t"
       "vmovdqa	%%ymm11,  96(%[x])\n\t"
       "vmovdqa	%%ymm12, 128(%[x])\n\t"
       "vmovdqa	%%ymm13, 160(%[x])\n\t"
       "vmovdqa	%%ymm14, 192(%[x])\n\t"
       "vmovdqa	%%ymm15, 224(%[x])\n\t"

       "vpunpckldq	%%ymm1, %%ymm0, %%ymm8\n\t"
       "vpunpckldq	%%ymm3, %%ymm2, %%ymm9\n\t"
       "vpunpckhdq	%%ymm1, %%ymm0, %%ymm12\n\t"
       "vpunpckhdq	%%ymm3, %%ymm2, %%ymm13\n\t"
       "vpunpckldq	%%ymm5, %%ymm4, %%ymm10\n\t"
       "vpunpckldq	%%ymm7, %%ymm6, %%ymm11\n\t"
       "vpunpckhdq	%%ymm5, %%ymm4, %%ymm14\n\t"
       "vpunpckhdq	%%ymm7, %%ymm6, %%ymm15\n\t"
       "vpunpcklqdq	%%ymm9,  %%ymm8,  %%ymm0\n\t"
       "vpunpcklqdq	%%ymm11, %%ymm10, %%ymm1\n\t"
       "vpunpckhqdq	%%ymm9,  %%ymm8,  %%ymm2\n\t"
       "vpunpckhqdq	%%ymm11, %%ymm10, %%ymm3\n\t"
       "vpunpcklqdq	%%ymm13, %%ymm12, %%ymm4\n\t"
       "vpunpcklqdq	%%ymm15, %%ymm14, %%ymm5\n\t"
       "vpunpckhqdq	%%ymm13, %%ymm12, %%ymm6\n\t"
       "vpunpckhqdq	%%ymm15, %%ymm14, %%ymm7\n\t"
       "vperm2i128	$0x20, %%ymm1, %%ymm0, %%ymm8\n\t"
       "vperm2i128	$0x20, %%ymm3, %%ymm2, %%ymm9\n\t"
       "vperm2i128	$0x31, %%ymm1, %%ymm0, %%ymm12\n\t"
       "vperm2i128	$0x31, %%ymm3, %%ymm2, %%ymm13\n\t"
       "vperm2i128	$0x20, %%ymm5, %%ymm4, %%ymm10\n\t"
       "vperm2i128	$0x20, %%ymm7, %%ymm6, %%ymm11\n\t"
       "vperm2i128	$0x31, %%ymm5, %%ymm4, %%ymm14\n\t"
       "vperm2i128	$0x31, %%ymm7, %%ymm6, %%ymm15\n\t"

       "vmovdqu	   (%[in]), %%ymm0\n\t"
       "vmovdqu	 64(%[in]), %%ymm1\n\t"
       "vmovdqu	128(%[in]), %%ymm2\n\t"
       "vmovdqu	192(%[in]), %%ymm3\n\t"
       "vmovdqu	256(%[in]), %%ymm4\n\t"
       "vmovdqu	320(%[in]), %%ymm5\n\t"
       "vmovdqu	384(%[in]), %%ymm6\n\t"
       "vmovdqu	448(%[in]), %%ymm7\n\t"
       "vpxor	%%ymm0, %%ymm8,  %%ymm8\n\t"
       "vpxor	%%ymm1, %%ymm9,  %%ymm9\n\t"
       "vpxor	%%ymm2, %%ymm10, %%ymm10\n\t"
       "vpxor	%%ymm3, %%ymm11, %%ymm11\n\t"
       "vpxor	%%ymm4, %%ymm12, %%ymm12\n\t"
       "vpxor	%%ymm5, %%ymm13, %%ymm13\n\t"
       "vpxor	%%ymm6, %%ymm14, %%ymm14\n\t"
       "vpxor	%%ymm7, %%ymm15, %%ymm15\n\t"
       "vmovdqu	%%ymm8,     (%[out])\n\t"
       "vmovdqu	%%ymm9,   64(%[out])\n\t"
       "vmovdqu	%%ymm10, 128(%[out])\n\t"
       "vmovdqu	%%ymm11, 192(%[out])\n\t"
       "vmovdqu	%%ymm12, 256(%[out])\n\t"
       "vmovdqu	%%ymm13, 320(%[out])\n\t"
       "vmovdqu	%%ymm14, 384(%[out])\n\t"
       "vmovdqu	%%ymm15, 448(%[out])\n\t"

       "vmovdqa	   (%[x]), %%ymm0\n\t"
       "vmovdqa	 32(%[x]), %%ymm1\n\t"
       "vmovdqa	 64(%[x]), %%ymm2\n\t"
       "vmovdqa	 96(%[x]), %%ymm3\n\t"
       "vmovdqa	128(%[x]), %%ymm4\n\t"
       "vmovdqa	160(%[x]), %%ymm5\n\t"
       "vmovdqa	192(%[x]), %%ymm6\n\t"
       "vmovdqa	224(%[x]), %%ymm7\n\t"

       "vpunpckldq	%%ymm1, %%ymm0, %%ymm8\n\t"
       "vpunpckldq	%%ymm3, %%ymm2, %%ymm9\n\t"
       "vpunpckhdq	%%ymm1, %%ymm0, %%ymm12\n\t"
       "vpunpckhdq	%%ymm3, %%ymm2, %%ymm13\n\t"
       "vpunpckldq	%%ymm5, %%ymm4, %%ymm10\n\t"
       "vpunpckldq	%%ymm7, %%ymm6, %%ymm11\n\t"
       "vpunpckhdq	%%ymm5, %%ymm4, %%ymm14\n\t"
       "vpunpckhdq	%%ymm7, %%ymm6, %%ymm15\n\t"
       "vpunpcklqdq	%%ymm9,  %%ymm8,  %%ymm0\n\t"
       "vpunpcklqdq	%%ymm11, %%ymm10, %%ymm1\n\t"
       "vpunpckhqdq	%%ymm9 , %%ymm8,  %%ymm2\n\t"
       "vpunpckhqdq	%%ymm11, %%ymm10, %%ymm3\n\t"
       "vpunpcklqdq	%%ymm13, %%ymm12, %%ymm4\n\t"
       "vpunpcklqdq	%%ymm15, %%ymm14, %%ymm5\n\t"
       "vpunpckhqdq	%%ymm13, %%ymm12, %%ymm6\n\t"
       "vpunpckhqdq	%%ymm15, %%ymm14, %%ymm7\n\t"
       "vperm2i128	$0x20, %%ymm1, %%ymm0, %%ymm8\n\t"
       "vperm2i128	$0x20, %%ymm3, %%ymm2, %%ymm9\n\t"
       "vperm2i128	$0x31, %%ymm1, %%ymm0, %%ymm12\n\t"
       "vperm2i128	$0x31, %%ymm3, %%ymm2, %%ymm13\n\t"
       "vperm2i128	$0x20, %%ymm5, %%ymm4, %%ymm10\n\t"
       "vperm2i128	$0x20, %%ymm7, %%ymm6, %%ymm11\n\t"
       "vperm2i128	$0x31, %%ymm5, %%ymm4, %%ymm14\n\t"
       "vperm2i128	$0x31, %%ymm7, %%ymm6, %%ymm15\n\t"

       "vmovdqu	 32(%[in]), %%ymm0\n\t"
       "vmovdqu	 96(%[in]), %%ymm1\n\t"
       "vmovdqu	160(%[in]), %%ymm2\n\t"
       "vmovdqu	224(%[in]), %%ymm3\n\t"
       "vmovdqu	288(%[in]), %%ymm4\n\t"
       "vmovdqu	352(%[in]), %%ymm5\n\t"
       "vmovdqu	416(%[in]), %%ymm6\n\t"
       "vmovdqu	480(%[in]), %%ymm7\n\t"
       "vpxor	%%ymm0, %%ymm8,  %%ymm8\n\t"
       "vpxor	%%ymm1, %%ymm9,  %%ymm9\n\t"
       "vpxor	%%ymm2, %%ymm10, %%ymm10\n\t"
       "vpxor	%%ymm3, %%ymm11, %%ymm11\n\t"
       "vpxor	%%ymm4, %%ymm12, %%ymm12\n\t"
       "vpxor	%%ymm5, %%ymm13, %%ymm13\n\t"
       "vpxor	%%ymm6, %%ymm14, %%ymm14\n\t"
       "vpxor	%%ymm7, %%ymm15, %%ymm15\n\t"
       "vmovdqu	%%ymm8,   32(%[out])\n\t"
       "vmovdqu	%%ymm9,   96(%[out])\n\t"
       "vmovdqu	%%ymm10, 160(%[out])\n\t"
       "vmovdqu	%%ymm11, 224(%[out])\n\t"
       "vmovdqu	%%ymm12, 288(%[out])\n\t"
       "vmovdqu	%%ymm13, 352(%[out])\n\t"
       "vmovdqu	%%ymm14, 416(%[out])\n\t"
       "vmovdqu	%%ymm15, 480(%[out])\n\t"

       "vmovdqa	384(%[X]), %%ymm12\n\t"
       "add	$512, %[in]\n\t"
       "add	$512, %[out]\n\t"
       "vpaddd	%[eight], %%ymm12, %%ymm12\n\t"
       "sub	$512, %[bytes]\n\t"
       "vmovdqa	%%ymm12, 384(%[X])\n\t"
       "cmp	$512, %[bytes]\n\t"
       "jl	L_done256\n\t"

       "vmovdqa	   (%[X]), %%ymm0\n\t"
       "vmovdqa	 32(%[X]), %%ymm1\n\t"
       "vmovdqa	 64(%[X]), %%ymm2\n\t"
       "vmovdqa	 96(%[X]), %%ymm3\n\t"
       "vmovdqa	128(%[X]), %%ymm4\n\t"
       "vmovdqa	160(%[X]), %%ymm5\n\t"
       "vmovdqa	192(%[X]), %%ymm6\n\t"
       "vmovdqa	224(%[X]), %%ymm7\n\t"
       "vmovdqa	256(%[X]), %%ymm8\n\t"
       "vmovdqa	288(%[X]), %%ymm9\n\t"
       "vmovdqa	320(%[X]), %%ymm10\n\t"
       "vmovdqa	352(%[X]), %%ymm11\n\t"
       "vmovdqa	384(%[X]), %%ymm12\n\t"
       "vmovdqa	416(%[X]), %%ymm13\n\t"
       "vmovdqa	448(%[X]), %%ymm14\n\t"
       "vmovdqa	480(%[X]), %%ymm15\n\t"
       "jmp	L_enc256_loop\n\t"
       "\n"
   "L_done256:\n\t"
       "shl	$3, %[cnt]\n\t"
       "add	48(%[key]), %[cnt]\n\t"
       "movl	%[cnt], 48(%[key])\n\t"
       "\n"
   "L_end256:\n\t"
       : [bytes] "+r" (bytes), [cnt] "+r" (cnt),
         [in] "+r" (m), [out] "+r" (c)
       : [X] "r" (X), [x] "r" (x), [key] "r" (ctx->X),
         [add] "m" (add), [eight] "m" (eight),
         [rotl8] "m" (rotl8_256), [rotl16] "m" (rotl16_256)
       : "ymm0", "ymm1", "ymm2", "ymm3",
         "ymm4", "ymm5", "ymm6", "ymm7",
         "ymm8", "ymm9", "ymm10", "ymm11",
         "ymm12", "ymm13", "ymm14", "ymm15", "memory"
    );

    /* AVX code optimised for multiples of 256 bytes. */
    if (bytes == 256) {
        chacha_encrypt_avx(ctx, m, c, bytes);
        bytes -= 256;
    }

    for (; bytes >= CHACHA_CHUNK_BYTES;) {
        CHACHA_CHUNK_AVX2();
        bytes -= CHACHA_CHUNK_BYTES;
        c += CHACHA_CHUNK_BYTES;
        m += CHACHA_CHUNK_BYTES;
    }
    if (bytes > 0) {
        CHACHA_PARTIAL_CHUNK_AVX2();
    }
}
#endif /* HAVE_INTEL_AVX2 */
#endif /* USE_INTEL_CHACHA_SPEEDUP */

/**
  * Encrypt a stream of bytes
  */
static void wc_Chacha_encrypt_bytes(ChaCha* ctx, const byte* m, byte* c,
                                    word32 bytes)
{
    byte*  output;
    word32 temp[CHACHA_CHUNK_WORDS]; /* used to make sure aligned */
    word32 i;

    output = (byte*)temp;

    for (; bytes > 0;) {
        wc_Chacha_wordtobyte(temp, ctx->X);
        ctx->X[CHACHA_IV_BYTES] = PLUSONE(ctx->X[CHACHA_IV_BYTES]);
        if (bytes <= CHACHA_CHUNK_BYTES) {
            for (i = 0; i < bytes; ++i) {
                c[i] = m[i] ^ output[i];
            }
            return;
        }
        for (i = 0; i < CHACHA_CHUNK_BYTES; ++i) {
            c[i] = m[i] ^ output[i];
        }
        bytes -= CHACHA_CHUNK_BYTES;
        c += CHACHA_CHUNK_BYTES;
        m += CHACHA_CHUNK_BYTES;
    }
}

/**
  * API to encrypt/decrypt a message of any size.
  */
int wc_Chacha_Process(ChaCha* ctx, byte* output, const byte* input,
                      word32 msglen)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef USE_INTEL_CHACHA_SPEEDUP
    if (!cpuidFlagsSet) {
        cpuidFlags = cpuid_get_flags();
        cpuidFlagsSet = 1;
    }

    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(cpuidFlags)) {
        chacha_encrypt_avx2(ctx, input, output, msglen);
        return 0;
    }
    #endif
    if (IS_INTEL_AVX1(cpuidFlags)) {
        chacha_encrypt_avx(ctx, input, output, msglen);
        return 0;
    }
    else {
        chacha_encrypt_x64(ctx, input, output, msglen);
        return 0;
    }
#endif
    wc_Chacha_encrypt_bytes(ctx, input, output, msglen);

    return 0;
}

#endif /* HAVE_CHACHA*/

