/* poly1305.c
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

/*
 * Based off the public domain implementations by Andrew Moon
 * and Daniel J. Bernstein
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_POLY1305
#include <wolfssl/wolfcrypt/poly1305.h>
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

#ifdef _MSC_VER
    /* 4127 warning constant while(1)  */
    #pragma warning(disable: 4127)
#endif

#ifdef USE_INTEL_SPEEDUP
    #include <emmintrin.h>
    #include <immintrin.h>

    #if defined(__GNUC__) && ((__GNUC__ < 4) || \
                              (__GNUC__ == 4 && __GNUC_MINOR__ <= 8))
        #define NO_AVX2_SUPPORT
    #endif
    #if defined(__clang__) && ((__clang_major__ < 3) || \
                               (__clang_major__ == 3 && __clang_minor__ <= 5))
        #define NO_AVX2_SUPPORT
    #endif

    #define HAVE_INTEL_AVX1
    #ifndef NO_AVX2_SUPPORT
        #define HAVE_INTEL_AVX2
    #endif
#endif

#ifdef USE_INTEL_SPEEDUP
static word32 intel_flags = 0;
static word32 cpu_flags_set = 0;
#endif

#if defined(USE_INTEL_SPEEDUP) || defined(POLY130564)
    #if defined(_MSC_VER)
        #define POLY1305_NOINLINE __declspec(noinline)
    #elif defined(__GNUC__)
        #define POLY1305_NOINLINE __attribute__((noinline))
    #else
        #define POLY1305_NOINLINE
    #endif

    #if defined(_MSC_VER)
        #include <intrin.h>

        typedef struct word128 {
            word64 lo;
            word64 hi;
        } word128;

        #define MUL(out, x, y) out.lo = _umul128((x), (y), &out.hi)
        #define ADD(out, in) { word64 t = out.lo; out.lo += in.lo; \
                               out.hi += (out.lo < t) + in.hi; }
        #define ADDLO(out, in) { word64 t = out.lo; out.lo += in; \
                                 out.hi += (out.lo < t); }
        #define SHR(in, shift) (__shiftright128(in.lo, in.hi, (shift)))
        #define LO(in) (in.lo)

    #elif defined(__GNUC__)
        #if defined(__SIZEOF_INT128__)
            typedef unsigned __int128 word128;
        #else
            typedef unsigned word128 __attribute__((mode(TI)));
        #endif

        #define MUL(out, x, y) out = ((word128)x * y)
        #define ADD(out, in) out += in
        #define ADDLO(out, in) out += in
        #define SHR(in, shift) (word64)(in >> (shift))
        #define LO(in) (word64)(in)
    #endif
#endif

#ifdef USE_INTEL_SPEEDUP
#ifdef HAVE_INTEL_AVX1
static void poly1305_block_avx(Poly1305* ctx, const unsigned char *m)
{
        __asm__ __volatile__ (
            "movq	(%[ctx]), %%r15\n\t"
            "movq	24(%[ctx]), %%r8\n\t"
            "movq	32(%[ctx]), %%r9\n\t"
            "movq	40(%[ctx]), %%r10\n\t"
            "xorq	%%rbx, %%rbx\n\t"
            "movb	%[nfin], %%bl\n\t"
            "# h += m\n\t"
            "movq	 (%[m]), %%r11\n\t"
            "movq	8(%[m]), %%r12\n\t"
            "addq	%%r11, %%r8\n\t"
            "adcq	%%r12, %%r9\n\t"
            "movq	8(%[ctx]), %%rax\n\t"
            "adcq	%%rbx, %%r10\n\t"
            "# r[1] * h[0] => rdx, rax ==> t2, t1\n\t"
            "mulq	%%r8\n\t"
            "movq	%%rax, %%r12\n\t"
            "movq	%%rdx, %%r13\n\t"
            "# r[0] * h[1] => rdx, rax ++> t2, t1\n\t"
            "movq	%%r15, %%rax\n\t"
            "mulq	%%r9\n\t"
            "addq	%%rax, %%r12\n\t"
            "movq	%%r15, %%rax\n\t"
            "adcq	%%rdx, %%r13\n\t"
            "# r[0] * h[0] => rdx, rax +=> t1, t0\n\t"
            "mulq	%%r8\n\t"
            "movq	%%rdx, %%r8\n\t"
            "movq	%%rax, %%r11\n\t"
            "# r[1] * h[1] => rdx, rax =+> t3, t2\n\t"
            "movq	8(%[ctx]), %%rax\n\t"
            "mulq	%%r9\n\t"
            "#   r[0] * h[2] +> t2\n\t"
            "addq	64(%[ctx],%%r10,8), %%r13\n\t"
            "movq	%%rdx, %%r14\n\t"
            "addq	%%r8, %%r12\n\t"
            "adcq	%%rax, %%r13\n\t"
            "#   r[1] * h[2] +> t3\n\t"
            "adcq	120(%[ctx],%%r10,8), %%r14\n\t"
            "# r * h in r14, r13, r12, r11 \n\t"
            "# h = (r * h) mod 2^130 - 5\n\t"
            "movq	%%r13, %%r10\n\t"
            "andq	  $-4, %%r13\n\t"
            "andq	   $3, %%r10\n\t"
            "addq	%%r13, %%r11\n\t"
            "movq	%%r13, %%r8\n\t"
            "adcq	%%r14, %%r12\n\t"
            "adcq	   $0, %%r10\n\t"
            "shrdq	   $2, %%r14, %%r8\n\t"
            "shrq	   $2, %%r14\n\t"
            "addq	%%r11, %%r8\n\t"
            "adcq	%%r14, %%r12\n\t"
            "movq	%%r12, %%r9\n\t"
            "adcq	   $0, %%r10\n\t"
            "# h in r10, r9, r8 \n\t"
            "# Store h to ctx\n\t"
            "movq       %%r8, 24(%[ctx])\n\t"
            "movq       %%r9, 32(%[ctx])\n\t"
            "movq       %%r10, 40(%[ctx])\n\t"
            :
            : [m] "r" (m), [ctx] "r" (ctx), [nfin] "m" (ctx->finished)
            : "rax", "rdx", "r11", "r12", "r13", "r14", "r15", "rbx",
              "r8", "r9", "r10", "memory"
        );
}

POLY1305_NOINLINE static void poly1305_blocks_avx(Poly1305* ctx,
                                           const unsigned char* m, size_t bytes)
{
        __asm__ __volatile__ (
            "movq	(%[ctx]), %%r15\n\t"
            "movq	24(%[ctx]), %%r8\n\t"
            "movq	32(%[ctx]), %%r9\n\t"
            "movq	40(%[ctx]), %%r10\n"
        "L_avx_start:\n\t"
            "# h += m\n\t"
            "movq	 (%[m]), %%r11\n\t"
            "movq	8(%[m]), %%r12\n\t"
            "addq	%%r11, %%r8\n\t"
            "adcq	%%r12, %%r9\n\t"
            "movq	8(%[ctx]), %%rax\n\t"
            "adcq	$0, %%r10\n\t"
            "# r[1] * h[0] => rdx, rax ==> t2, t1\n\t"
            "mulq	%%r8\n\t"
            "movq	%%rax, %%r12\n\t"
            "movq	%%rdx, %%r13\n\t"
            "# r[0] * h[1] => rdx, rax ++> t2, t1\n\t"
            "movq	%%r15, %%rax\n\t"
            "mulq	%%r9\n\t"
            "addq	%%rax, %%r12\n\t"
            "movq	%%r15, %%rax\n\t"
            "adcq	%%rdx, %%r13\n\t"
            "# r[0] * h[0] => rdx, rax +=> t1, t0\n\t"
            "mulq	%%r8\n\t"
            "movq	%%rdx, %%r8\n\t"
            "movq	%%rax, %%r11\n\t"
            "# r[1] * h[1] => rdx, rax =+> t3, t2\n\t"
            "movq	8(%[ctx]), %%rax\n\t"
            "mulq	%%r9\n\t"
            "#   r[0] * h[2] +> t2\n\t"
            "addq	72(%[ctx],%%r10,8), %%r13\n\t"
            "movq	%%rdx, %%r14\n\t"
            "addq	%%r8, %%r12\n\t"
            "adcq	%%rax, %%r13\n\t"
            "#   r[1] * h[2] +> t3\n\t"
            "adcq	128(%[ctx],%%r10,8), %%r14\n\t"
            "# r * h in r14, r13, r12, r11 \n\t"
            "# h = (r * h) mod 2^130 - 5\n\t"
            "movq	%%r13, %%r10\n\t"
            "andq	  $-4, %%r13\n\t"
            "andq	   $3, %%r10\n\t"
            "addq	%%r13, %%r11\n\t"
            "movq	%%r13, %%r8\n\t"
            "adcq	%%r14, %%r12\n\t"
            "adcq	   $0, %%r10\n\t"
            "shrdq	   $2, %%r14, %%r8\n\t"
            "shrq	   $2, %%r14\n\t"
            "addq	%%r11, %%r8\n\t"
            "adcq	%%r14, %%r12\n\t"
            "movq	%%r12, %%r9\n\t"
            "adcq	   $0, %%r10\n\t"
            "# h in r10, r9, r8 \n\t"
            "# Next block from message\n\t"
            "addq	$16, %[m]\n\t"
            "subq	$16, %[bytes]\n\t"
            "cmp        $16, %[bytes]\n\t"
            "jge	L_avx_start\n\t"
            "# Store h to ctx\n\t"
            "movq	%%r8, 24(%[ctx])\n\t"
            "movq	%%r9, 32(%[ctx])\n\t"
            "movq	%%r10, 40(%[ctx])\n\t"
            : [m] "+r" (m), [bytes] "+r" (bytes)
            : [ctx] "r" (ctx)
            : "rax", "rdx", "r11", "r12", "r13", "r14", "r15",
              "r8", "r9", "r10", "memory"
        );
}

static void poly1305_setkey_avx(Poly1305* ctx, const byte* key)
{
    int i;

    ctx->r[0] = *(word64*)(key + 0) & 0x0ffffffc0fffffffL;
    ctx->r[1] = *(word64*)(key + 8) & 0x0ffffffc0ffffffcL;

    for (i=0; i<7; i++) {
        ctx->hh[i + 0] = ctx->r[0] * i;
        ctx->hh[i + 7] = ctx->r[1] * i;
    }

    /* h (accumulator) = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;

    /* save pad for later */
    ctx->pad[0] = *(word64*)(key + 16);
    ctx->pad[1] = *(word64*)(key + 24);

    ctx->leftover = 0;
    ctx->finished = 1;
}

static void poly1305_final_avx(Poly1305* ctx, byte* mac)
{
    word64 h0, h1, h2;

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i] = 1;
        for (i = i + 1; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 0;
        poly1305_block_avx(ctx, ctx->buffer);
    }

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

    /* h %= p */
    /* h = (h + pad) */
    __asm__ __volatile__ (
        "# mod 2^130 - 5\n\t"
        "movq	%[h2],  %%r13\n\t"
        "andq	 $0x3, %[h2]\n\t"
        "shrq	 $0x2, %%r13\n\t"
        "leaq	(%%r13, %%r13, 4), %%r13\n\t"
        "add	 %%r13, %[h0]\n\t"
        "adc	   $0, %[h1]\n\t"
        "adc	   $0, %[h2]\n\t"
        "# Fixup when between (1 << 130) - 1 and (1 << 130) - 5\n\t"
        "movq	%[h0], %%r13\n\t"
        "movq	%[h1], %%r14\n\t"
        "movq	%[h2], %%r15\n\t"
        "addq	$5, %%r13\n\t"
        "adcq	$0, %%r14\n\t"
        "adcq	$0, %%r15\n\t"
        "movq	%%r15, %%r12\n\t"
        "andq	$3, %%r15\n\t"
        "cmpq   $4, %%r12\n\t"
        "cmove	%%r13, %[h0]\n\t"
        "cmove	%%r14, %[h1]\n\t"
        "cmove	%%r15, %[h2]\n\t"
        "# h += pad\n\t"
        "add	%[p0], %[h0]\n\t"
        "adc	%[p1], %[h1]\n\t"
        "movq	%[h0], (%[m])\n\t"
        "movq	%[h1], 8(%[m])\n\t"
        : [h0] "+r" (h0), [h1] "+r" (h1), [h2] "+r" (h2),
          [p0] "+r" (ctx->pad[0]), [p1] "+r" (ctx->pad[1])
        : [m] "r" (mac)
        : "memory", "r15", "r14", "r13", "r12"
    );

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;
}
#endif

#ifdef HAVE_INTEL_AVX2
#if defined(_MSC_VER)
    #define POLY1305_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
    #define POLY1305_NOINLINE __attribute__((noinline))
#else
    #define POLY1305_NOINLINE
#endif

/* Load H into five 256-bit registers.
 *
 * h is the memory location of the data - 26 bits in 32.
 * h0-h4 the 4 H values with 26 bits stored in 64 for multiply.
 * z is zero.
 */
#define LOAD_H(h, h0, h1, h2, h3, h4, z)  \
    "vmovdqu      ("#h"), "#h1"\n\t"      \
    "vmovdqu    32("#h"), "#h3"\n\t"      \
    "vmovdqu    64("#h"), "#h4"\n\t"      \
    "vpermq	$0xd8, "#h1", "#h1"\n\t"  \
    "vpermq	$0xd8, "#h3", "#h3"\n\t"  \
    "vpermq	$0xd8, "#h4", "#h4"\n\t"  \
    "vpunpckldq	 "#z", "#h1", "#h0"\n\t"  \
    "vpunpckhdq	 "#z", "#h1", "#h1"\n\t"  \
    "vpunpckldq	 "#z", "#h3", "#h2"\n\t"  \
    "vpunpckhdq	 "#z", "#h3", "#h3"\n\t"  \
    "vpunpckldq	 "#z", "#h4", "#h4"\n\t"

/* Store H, five 256-bit registers, packed.
 *
 * h is the memory location of the data - 26 bits in 32.
 * h0-h4 the 4 H values with 26 bits stored in 64.
 * x4 is the xmm register of h4.
 */
#define STORE_H(h, h0, h1, h2, h3, h4, x4)      \
    "vpshufd	$0x08, "#h0", "#h0"\n\t"        \
    "vpshufd	$0x08, "#h1", "#h1"\n\t"        \
    "vpshufd	$0x08, "#h2", "#h2"\n\t"        \
    "vpshufd	$0x08, "#h3", "#h3"\n\t"        \
    "vpshufd	$0x08, "#h4", "#h4"\n\t"        \
    "vpermq	$0x08, "#h0", "#h0"\n\t"        \
    "vpermq	$0x08, "#h1", "#h1"\n\t"        \
    "vpermq	$0x08, "#h2", "#h2"\n\t"        \
    "vpermq	$0x08, "#h3", "#h3"\n\t"        \
    "vpermq	$0x08, "#h4", "#h4"\n\t"        \
    "vperm2i128	$0x20, "#h1", "#h0", "#h0"\n\t" \
    "vperm2i128	$0x20, "#h3", "#h2", "#h2"\n\t" \
    "vmovdqu	 "#h0",   ("#h")\n\t"           \
    "vmovdqu	 "#h2", 32("#h")\n\t"           \
    "vmovdqu	 "#x4", 64("#h")\n\t"

/* Load four powers of r into position to be multiplied by the 4 H values.
 *
 * rp0-rp3 are the register holding pointers to the values of the powers of r.
 * r0-r4 holds the loaded values with 26 bits store in 64 for multiply.
 * t0-t3 are temporary registers.
 */
#define LOAD_Rx4(rp0, rp1, rp2, rp3,                     \
                 r0, r1, r2, r3, r4,                     \
                 t0, t1, t2, t3)                         \
    "vmovdqu		("#rp0"), "#r0"\n\t"             \
    "vmovdqu		("#rp1"), "#r1"\n\t"             \
    "vmovdqu		("#rp2"), "#r2"\n\t"             \
    "vmovdqu		("#rp3"), "#r3"\n\t"             \
    "vpermq		$0xd8, "#r0", "#r0"\n\t"         \
    "vpermq		$0xd8, "#r1", "#r1"\n\t"         \
    "vpermq		$0xd8, "#r2", "#r2"\n\t"         \
    "vpermq		$0xd8, "#r3", "#r3"\n\t"         \
    "vpunpcklqdq	"#r1", "#r0", "#t0"\n\t"         \
    "vpunpckhqdq	"#r1", "#r0", "#t1"\n\t"         \
    "vpunpcklqdq	"#r3", "#r2", "#t2"\n\t"         \
    "vpunpckhqdq	"#r3", "#r2", "#t3"\n\t"         \
    "vperm2i128		$0x20, "#t2", "#t0", "#r0"\n\t"  \
    "vperm2i128		$0x31, "#t2", "#t0", "#r2"\n\t"  \
    "vperm2i128		$0x20, "#t3", "#t1", "#r4"\n\t"  \
    "vpsrlq		  $32, "#r0", "#r1"\n\t"         \
    "vpsrlq		  $32, "#r2", "#r3"\n\t"

/* Load the r^4 value into position to be multiplied by all 4 H values.
 *
 * r4 holds r^4 as five 26 bits each in 32.
 * r0-r4 holds the loaded values with 26 bits store in 64 for multiply.
 * t0-t1 are temporary registers.
 */
#define LOAD_R4(r4, r40, r41, r42, r43, r44, \
                t0, t1)                      \
    "vmovdqu	"#r4", "#t0"\n\t"            \
    "vpsrlq	  $32, "#t0", "#t1"\n\t"     \
    "vpermq	 $0x0, "#t0", "#r40"\n\t"    \
    "vpermq	 $0x0, "#t1", "#r41"\n\t"    \
    "vpermq	$0x55, "#t0", "#r42"\n\t"    \
    "vpermq	$0x55, "#t1", "#r43"\n\t"    \
    "vpermq	$0xaa, "#t0", "#r44"\n\t"

/* Multiply the top 4 26-bit values in 64 bits of each H by 5 for reduction in
 * multiply.
 *
 * s1-s4 are each 64 bit value in r1-r4 multiplied by 5.
 * r1-r4 are the top 4
 */
#define MUL5(s1, s2, s3, s4, r1, r2, r3, r4) \
    "vpslld	   $2, "#r1", "#s1"\n\t"     \
    "vpslld	   $2, "#r2", "#s2"\n\t"     \
    "vpslld	   $2, "#r3", "#s3"\n\t"     \
    "vpslld	   $2, "#r4", "#s4"\n\t"     \
    "vpaddq	"#s1", "#r1", "#s1"\n\t"     \
    "vpaddq	"#s2", "#r2", "#s2"\n\t"     \
    "vpaddq	"#s3", "#r3", "#s3"\n\t"     \
    "vpaddq	"#s4", "#r4", "#s4"\n\t"

/* Add the 4 H values together.
 * Each 64 bits in a register is 26 bits of one of the H values.
 *
 * h0-h4 contains the 4 H values.
 * t1-t4 are temporary registers.
 */
#define FINALIZE_H(h0, h1, h2, h3, h4,    \
                   t0, t1, t2, t3, t4)    \
    "vpermq	$0xf5, "#h0", "#t0"\n\t"  \
    "vpermq	$0xf5, "#h1", "#t1"\n\t"  \
    "vpermq	$0xf5, "#h2", "#t2"\n\t"  \
    "vpermq	$0xf5, "#h3", "#t3"\n\t"  \
    "vpermq	$0xf5, "#h4", "#t4"\n\t"  \
    "vpaddq	"#h0", "#t0", "#h0"\n\t"  \
    "vpaddq	"#h1", "#t1", "#h1"\n\t"  \
    "vpaddq	"#h2", "#t2", "#h2"\n\t"  \
    "vpaddq	"#h3", "#t3", "#h3"\n\t"  \
    "vpaddq	"#h4", "#t4", "#h4"\n\t"  \
    "vpermq	$0xaa, "#h0", "#t0"\n\t"  \
    "vpermq	$0xaa, "#h1", "#t1"\n\t"  \
    "vpermq	$0xaa, "#h2", "#t2"\n\t"  \
    "vpermq	$0xaa, "#h3", "#t3"\n\t"  \
    "vpermq	$0xaa, "#h4", "#t4"\n\t"  \
    "vpaddq	"#h0", "#t0", "#h0"\n\t"  \
    "vpaddq	"#h1", "#t1", "#h1"\n\t"  \
    "vpaddq	"#h2", "#t2", "#h2"\n\t"  \
    "vpaddq	"#h3", "#t3", "#h3"\n\t"  \
    "vpaddq	"#h4", "#t4", "#h4"\n\t"

/* Move 32 bits from each xmm register to a 32 bit register.
 *
 * x0-x4 are the xmm version of the ymm registers used.
 * t0-t4 are the 32-bit registers to store data in.
 */
#define MOVE_TO_32(x0, x1, x2, x3, x4,  \
                   t0, t1, t2, t3, t4)  \
    "vmovd	"#x0", "#t0"\n\t"       \
    "vmovd	"#x1", "#t1"\n\t"       \
    "vmovd	"#x2", "#t2"\n\t"       \
    "vmovd	"#x3", "#t3"\n\t"       \
    "vmovd	"#x4", "#t4"\n\t"

/* Multiply using AVX2 instructions.
 * Each register contains up to 32 bits of data in 64 bits.
 * This is a 4 way parallel multiply.
 *
 * h0-h4 contain 4 H values with the 32 bits of each per register.
 * r0-r4 contain the 4 powers of r.
 * s1-s4 contain r1-r4 times 5.
 * t0-t4 and v0-v3 are temporary registers.
 */
#define MUL_AVX2(h0, h1, h2, h3, h4,        \
                 r0, r1, r2, r3, r4,        \
                 s1, s2, s3, s4,            \
                 t0, t1, t2, t3, t4,        \
                 v0, v1, v2, v3)            \
    "vpmuludq	"#s1", "#h4", "#t0"\n\t"    \
    "vpmuludq	"#s2", "#h3", "#v0"\n\t"    \
    "vpmuludq	"#s2", "#h4", "#t1"\n\t"    \
    "vpmuludq	"#s3", "#h3", "#v1"\n\t"    \
    "vpmuludq	"#s3", "#h4", "#t2"\n\t"    \
    "vpaddq	"#t0", "#v0", "#t0"\n\t"    \
    "vpmuludq	"#s3", "#h2", "#v2"\n\t"    \
    "vpmuludq	"#s4", "#h4", "#t3"\n\t"    \
    "vpaddq	"#t1", "#v1", "#t1"\n\t"    \
    "vpmuludq	"#s4", "#h1", "#v3"\n\t"    \
    "vpmuludq	"#s4", "#h2", "#v0"\n\t"    \
    "vpaddq	"#t0", "#v2", "#t0"\n\t"    \
    "vpmuludq	"#s4", "#h3", "#v1"\n\t"    \
    "vpmuludq	"#r0", "#h3", "#v2"\n\t"    \
    "vpaddq	"#t0", "#v3", "#t0"\n\t"    \
    "vpmuludq	"#r0", "#h4", "#t4"\n\t"    \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"    \
    "vpmuludq	"#r0", "#h0", "#v3"\n\t"    \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"    \
    "vpmuludq	"#r0", "#h1", "#v0"\n\t"    \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"    \
    "vpmuludq	"#r0", "#h2", "#v1"\n\t"    \
    "vpmuludq	"#r1", "#h2", "#v2"\n\t"    \
    "vpaddq	"#t0", "#v3", "#t0"\n\t"    \
    "vpmuludq	"#r1", "#h3", "#v3"\n\t"    \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"    \
    "vpmuludq	"#r1", "#h0", "#v0"\n\t"    \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"    \
    "vpmuludq	"#r1", "#h1", "#v1"\n\t"    \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"    \
    "vpmuludq	"#r2", "#h1", "#v2"\n\t"    \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"    \
    "vpmuludq	"#r2", "#h2", "#v3"\n\t"    \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"    \
    "vpmuludq	"#r2", "#h0", "#v0"\n\t"    \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"    \
    "vpmuludq	"#r3", "#h0", "#v1"\n\t"    \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"    \
    "vpmuludq	"#r3", "#h1", "#v2"\n\t"    \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"    \
    "vpmuludq	"#r4", "#h0", "#v3"\n\t"    \
    "vpaddq	"#t2", "#v0", "#t2"\n\t"    \
    "vpaddq	"#t3", "#v1", "#t3"\n\t"    \
    "vpaddq	"#t4", "#v2", "#t4"\n\t"    \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"

/* Load the 4 blocks of the message.
 *
 * m the address of the message to load.
 * m0-m4 is the loaded message with 32 bits in 64. Loaded so data is parallel.
 * hi is the high bits of the 4 m (1<< 128 if not final block).
 * z is zero.
 */
#define LOAD_M(m, m0, m1, m2, m3, m4, hi, z)     \
    "vmovdqu      (%[m]), "#m0"\n\t"             \
    "vmovdqu    32(%[m]), "#m1"\n\t"             \
    "vperm2i128	$0x20, "#m1", "#m0", "#m2"\n\t"  \
    "vperm2i128	$0x31, "#m1", "#m0", "#m0"\n\t"  \
    "vpunpckldq	"#m0", "#m2", "#m1"\n\t"         \
    "vpunpckhdq	"#m0", "#m2", "#m3"\n\t"         \
    "vpunpckldq	 "#z", "#m1", "#m0"\n\t"         \
    "vpunpckhdq	 "#z", "#m1", "#m1"\n\t"         \
    "vpunpckldq	 "#z", "#m3", "#m2"\n\t"         \
    "vpunpckhdq	 "#z", "#m3", "#m3"\n\t"         \
    "vmovdqu	"#hi", "#m4"\n\t"                \
    "vpsllq	   $6, "#m1", "#m1"\n\t"         \
    "vpsllq	  $12, "#m2", "#m2"\n\t"         \
    "vpsllq	  $18, "#m3", "#m3"\n\t"


/* Multiply using AVX2 instructions - adding with message.
 * Each register contains up to 32 bits of data in 64 bits.
 * This is a 4 way parallel multiply.
 * The message data is loaded first and the multiplication adds into it.
 *
 * h0-h4 contain 4 H values with the 32 bits of each per register.
 * r0-r4 contain the 4 powers of r.
 * s1-s4 contain r1-r4 times 5.
 * t0-t4 and v0-v3 are temporary registers.
 * hi is the high bits of the 4 m (1<< 128 if not final block).
 * z is zero.
 */
#define MUL_ADD_AVX2(h0, h1, h2, h3, h4,         \
                     r0, r1, r2, r3, r4,         \
                     s1, s2, s3, s4,             \
                     t0, t1, t2, t3, t4,         \
                     v0, v1, v2, v3,             \
                     hi, z)                      \
    "vmovdqu      (%[m]), "#t0"\n\t"             \
    "vmovdqu    32(%[m]), "#t1"\n\t"             \
    "vperm2i128	$0x20, "#t1", "#t0", "#t2"\n\t"  \
    "vperm2i128	$0x31, "#t1", "#t0", "#t0"\n\t"  \
    "vpunpckldq	"#t0", "#t2", "#t1"\n\t"         \
    "vpunpckhdq	"#t0", "#t2", "#t3"\n\t"         \
    "vpunpckldq	 "#z", "#t1", "#t0"\n\t"         \
    "vpunpckhdq	 "#z", "#t1", "#t1"\n\t"         \
    "vpunpckldq	 "#z", "#t3", "#t2"\n\t"         \
    "vpunpckhdq	 "#z", "#t3", "#t3"\n\t"         \
    "vmovdqu	"#hi", "#t4"\n\t"                \
    "vpsllq	   $6, "#t1", "#t1"\n\t"         \
    "vpsllq	  $12, "#t2", "#t2"\n\t"         \
    "vpsllq	  $18, "#t3", "#t3"\n\t"         \
    "vpmuludq	"#s1", "#h4", "#v0"\n\t"         \
    "vpaddq     "#t0", "#v0", "#t0"\n\t"         \
    "vpmuludq	"#s2", "#h3", "#v0"\n\t"         \
    "vpmuludq	"#s2", "#h4", "#v1"\n\t"         \
    "vpaddq     "#t1", "#v1", "#t1"\n\t"         \
    "vpmuludq	"#s3", "#h3", "#v1"\n\t"         \
    "vpmuludq	"#s3", "#h4", "#v2"\n\t"         \
    "vpaddq     "#t2", "#v2", "#t2"\n\t"         \
    "vpaddq	"#t0", "#v0", "#t0"\n\t"         \
    "vpmuludq	"#s3", "#h2", "#v2"\n\t"         \
    "vpmuludq	"#s4", "#h4", "#v3"\n\t"         \
    "vpaddq     "#t3", "#v3", "#t3"\n\t"         \
    "vpaddq	"#t1", "#v1", "#t1"\n\t"         \
    "vpmuludq	"#s4", "#h1", "#v3"\n\t"         \
    "vpmuludq	"#s4", "#h2", "#v0"\n\t"         \
    "vpaddq	"#t0", "#v2", "#t0"\n\t"         \
    "vpmuludq	"#s4", "#h3", "#v1"\n\t"         \
    "vpmuludq	"#r0", "#h3", "#v2"\n\t"         \
    "vpaddq	"#t0", "#v3", "#t0"\n\t"         \
    "vpmuludq	"#r0", "#h4", "#v3"\n\t"         \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"         \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"         \
    "vpmuludq	"#r0", "#h0", "#v3"\n\t"         \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"         \
    "vpmuludq	"#r0", "#h1", "#v0"\n\t"         \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"         \
    "vpmuludq	"#r0", "#h2", "#v1"\n\t"         \
    "vpmuludq	"#r1", "#h2", "#v2"\n\t"         \
    "vpaddq	"#t0", "#v3", "#t0"\n\t"         \
    "vpmuludq	"#r1", "#h3", "#v3"\n\t"         \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"         \
    "vpmuludq	"#r1", "#h0", "#v0"\n\t"         \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"         \
    "vpmuludq	"#r1", "#h1", "#v1"\n\t"         \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"         \
    "vpmuludq	"#r2", "#h1", "#v2"\n\t"         \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"         \
    "vpmuludq	"#r2", "#h2", "#v3"\n\t"         \
    "vpaddq	"#t1", "#v0", "#t1"\n\t"         \
    "vpmuludq	"#r2", "#h0", "#v0"\n\t"         \
    "vpaddq	"#t2", "#v1", "#t2"\n\t"         \
    "vpmuludq	"#r3", "#h0", "#v1"\n\t"         \
    "vpaddq	"#t3", "#v2", "#t3"\n\t"         \
    "vpmuludq	"#r3", "#h1", "#v2"\n\t"         \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"         \
    "vpmuludq	"#r4", "#h0", "#v3"\n\t"         \
    "vpaddq	"#t2", "#v0", "#t2"\n\t"         \
    "vpaddq	"#t3", "#v1", "#t3"\n\t"         \
    "vpaddq	"#t4", "#v2", "#t4"\n\t"         \
    "vpaddq	"#t4", "#v3", "#t4"\n\t"

/* Reduce, in place, the 64 bits of data to 26 bits.
 *
 * h0-h4 contain the 4 H values to reduce.
 * t0-t2 are temporaries.
 * mask contains the 26-bit mask for each 64 bit value in the 256 bit register.
 */
#define REDUCE_IN(h0, h1, h2, h3, h4,       \
                  t0, t1, t2, mask)         \
    "vpsrlq	    $26, "#h0", "#t0"\n\t"  \
    "vpsrlq	    $26, "#h3", "#t1"\n\t"  \
    "vpand	"#mask", "#h0", "#h0"\n\t"  \
    "vpand	"#mask", "#h3", "#h3"\n\t"  \
    "vpaddq	  "#h1", "#t0", "#h1"\n\t"  \
    "vpaddq	  "#h4", "#t1", "#h4"\n\t"  \
                                            \
    "vpsrlq	    $26, "#h1", "#t0"\n\t"  \
    "vpsrlq	    $26, "#h4", "#t1"\n\t"  \
    "vpand	"#mask", "#h1", "#h1"\n\t"  \
    "vpand	"#mask", "#h4", "#h4"\n\t"  \
    "vpaddq	  "#h2", "#t0", "#h2"\n\t"  \
    "vpslld	     $2, "#t1", "#t2"\n\t"  \
    "vpaddd	  "#t2", "#t1", "#t2"\n\t"  \
    "vpaddq	  "#h0", "#t2", "#h0"\n\t"  \
                                            \
    "vpsrlq	    $26, "#h2", "#t0"\n\t"  \
    "vpsrlq	    $26, "#h0", "#t1"\n\t"  \
    "vpand	"#mask", "#h2", "#h2"\n\t"  \
    "vpand	"#mask", "#h0", "#h0"\n\t"  \
    "vpaddq	  "#h3", "#t0", "#h3"\n\t"  \
    "vpaddq	  "#h1", "#t1", "#h1"\n\t"  \
                                            \
    "vpsrlq	    $26, "#h3", "#t0"\n\t"  \
    "vpand	"#mask", "#h3", "#h3"\n\t"  \
    "vpaddq	  "#h4", "#t0", "#h4"\n\t"

/* Reduce the 64 bits of data to 26 bits.
 *
 * h0-h4 contain the reduced H values.
 * m0-m4 contain the 4 H values to reduce.
 * t0-t2 are temporaries.
 * mask contains the 26-bit mask for each 64 bit value in the 256 bit register.
 */
#define REDUCE(h0, h1, h2, h3, h4,          \
               m0, m1, m2, m3, m4,          \
               t0, t1, t2, mask)            \
    "vpsrlq	    $26, "#m0", "#t0"\n\t"  \
    "vpsrlq	    $26, "#m3", "#t1"\n\t"  \
    "vpand	"#mask", "#m0", "#m0"\n\t"  \
    "vpand	"#mask", "#m3", "#m3"\n\t"  \
    "vpaddq	  "#m1", "#t0", "#m1"\n\t"  \
    "vpaddq	  "#m4", "#t1", "#m4"\n\t"  \
                                            \
    "vpsrlq	    $26, "#m1", "#t0"\n\t"  \
    "vpsrlq	    $26, "#m4", "#t1"\n\t"  \
    "vpand	"#mask", "#m1", "#h1"\n\t"  \
    "vpand	"#mask", "#m4", "#h4"\n\t"  \
    "vpaddq	  "#m2", "#t0", "#m2"\n\t"  \
    "vpslld	     $2, "#t1", "#t2"\n\t"  \
    "vpaddd	  "#t2", "#t1", "#t2"\n\t"  \
    "vpaddq	  "#m0", "#t2", "#m0"\n\t"  \
                                            \
    "vpsrlq	    $26, "#m2", "#t0"\n\t"  \
    "vpsrlq	    $26, "#m0", "#t1"\n\t"  \
    "vpand	"#mask", "#m2", "#h2"\n\t"  \
    "vpand	"#mask", "#m0", "#h0"\n\t"  \
    "vpaddq	  "#m3", "#t0", "#m3"\n\t"  \
    "vpaddq	  "#h1", "#t1", "#h1"\n\t"  \
                                            \
    "vpsrlq	    $26, "#m3", "#t0"\n\t"  \
    "vpand	"#mask", "#m3", "#h3"\n\t"  \
    "vpaddq	  "#h4", "#t0", "#h4"\n\t"


POLY1305_NOINLINE static void poly1305_blocks_avx2(Poly1305* ctx,
                                           const unsigned char* m, size_t bytes)
{
    ALIGN256 word64 r4[5][4];
    ALIGN256 word64 s[4][4];
    register word32 t0 asm("r8");
    register word32 t1 asm("r9");
    register word32 t2 asm("r10");
    register word32 t3 asm("r11");
    register word32 t4 asm("r12");
    static const word64 mask[4] = { 0x0000000003ffffff, 0x0000000003ffffff,
                                    0x0000000003ffffff, 0x0000000003ffffff };

    __asm__ __volatile__ (
        "vpxor		%%ymm15, %%ymm15, %%ymm15\n\t"
        "cmpb		$0x0, %[started]\n\t"
        "jne		L_begin\n\t"
        "# Load the message data\n\t"
        LOAD_M(m, %%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4, %[hibit], %%ymm15)
        "vmovdqu	%[mask], %%ymm14\n\t"
        "# Reduce, in place, the message data\n\t"
        REDUCE_IN(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
                  %%ymm10, %%ymm11, %%ymm12, %%ymm14)
        "addq		$64, %[m]\n\t"
        "subq		$64, %[bytes]\n\t"
        "jz		L_store\n\t"
        "\n"
    "L_begin:\n\t"
        "# Load the H values.\n\t"
        LOAD_H(%[h], %%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4, %%ymm15)
        "movq		336(%[ctx]), %%r8\n\t"
        "# Check if there is a power of r to load - otherwise use r^4.\n\t"
        "cmpq		$0x0, %%r8\n\t"
        "je		L_load_r4\n\t"
        "\n\t"
        "movq		344(%[ctx]), %%r9\n\t"
        "movq		352(%[ctx]), %%r10\n\t"
        "movq		360(%[ctx]), %%r11\n\t"
        "# Load the 4 powers of r.\n\t"
        LOAD_Rx4(%%r8, %%r9, %%r10, %%r11, \
                 %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
                 %%ymm10, %%ymm11, %%ymm12, %%ymm13)
        "jmp		L_mul_5\n\t"
        "\n"
     "L_load_r4:\n\t"
        "# Load r^4 into all four positions.\n\t"
        LOAD_R4(304(%[ctx]), %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
                %%ymm13, %%ymm14)
        "\n"
    "L_mul_5:\n\t"
        "# Multiply top 4 26-bit values of all four H by 5\n\t"
        MUL5(%%ymm10, %%ymm11, %%ymm12, %%ymm13, %%ymm6, %%ymm7, %%ymm8, %%ymm9)
        "# Store powers of r and multiple of 5 for use in multiply.\n\t"
        "vmovdqa	%%ymm10,    (%[s])\n\t"
        "vmovdqa	%%ymm11,  32(%[s])\n\t"
        "vmovdqa	%%ymm12,  64(%[s])\n\t"
        "vmovdqa	%%ymm13,  96(%[s])\n\t"
        "vmovdqa	 %%ymm5,    (%[r4])\n\t"
        "vmovdqa	 %%ymm6,  32(%[r4])\n\t"
        "vmovdqa	 %%ymm7,  64(%[r4])\n\t"
        "vmovdqa	 %%ymm8,  96(%[r4])\n\t"
        "vmovdqa	 %%ymm9, 128(%[r4])\n\t"
        "vmovdqu	%[mask], %%ymm14\n\t"
        "\n"
        "# If not finished then loop over data\n\t"
        "cmpb		$0x1, %[fin]\n\t"
        "jne		L_start\n\t"
        "# Do last multiply, reduce, add the four H together and move to\n\t"
        "# 32-bit registers\n\t"
        MUL_AVX2(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
                 (%[r4]), 32(%[r4]), 64(%[r4]), 96(%[r4]), 128(%[r4]),
                 (%[s]), 32(%[s]), 64(%[s]), 96(%[s]),
                 %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
                 %%ymm10, %%ymm11, %%ymm12, %%ymm13)
        REDUCE(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
               %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
               %%ymm10, %%ymm11, %%ymm12, %%ymm14)
        FINALIZE_H(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
                   %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9)
        MOVE_TO_32(%%xmm0, %%xmm1, %%xmm2, %%xmm3, %%xmm4,
                   %[t0], %[t1], %[t2], %[t3], %[t4])
        "jmp		L_end\n\t"
        "\n"
    "L_start:\n\t"
        MUL_ADD_AVX2(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
                     (%[r4]), 32(%[r4]), 64(%[r4]), 96(%[r4]), 128(%[r4]),
                     (%[s]), 32(%[s]), 64(%[s]), 96(%[s]),
                     %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
                     %%ymm10, %%ymm11, %%ymm12, %%ymm13,
                     %[hibit], %%ymm15)
        REDUCE(%%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4,
               %%ymm5, %%ymm6, %%ymm7, %%ymm8, %%ymm9,
               %%ymm10, %%ymm11, %%ymm12, %%ymm14)
        "addq		$64, %[m]\n\t"
        "subq		$64, %[bytes]\n\t"
        "jnz		L_start\n\t"
        "\n"
    "L_store:\n\t"
        "# Store four H values - state\n\t"
        STORE_H(%[h], %%ymm0, %%ymm1, %%ymm2, %%ymm3, %%ymm4, %%xmm4)
        "\n"
    "L_end:\n\t"
        : [m] "+r" (m), [bytes] "+r" (bytes),
          [t0] "+r" (t0), [t1] "+r" (t1), [t2] "+r" (t2),
          [t3] "+r" (t3), [t4] "+r" (t4)
        : [ctx] "r" (ctx), [h] "r" (ctx->hh),
          [r4] "r" (r4), [s] "r" (s),
          [fin] "m" (ctx->finished), [started] "m" (ctx->started),
          [mask] "m" (mask), [hibit] "m" (ctx->hibit)
        : "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7",
          "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15",
          "memory"
    );

    if (ctx->finished)
    {
        word64 h0, h1, h2, g0, g1, g2, c;

        /* Convert to 64 bit form. */
        h0 = (((word64)(t1 & 0x3FFFF)) << 26) +  t0;
        h1 = (((word64)(t3 &   0x3FF)) << 34) +
             (((word64) t2           ) <<  8) + (t1 >> 18);
        h2 = (((word64) t4           ) << 16) + (t3 >> 10);

        /* Perform modulur reduction. */
                     c = (h1 >> 44); h1 &= 0xfffffffffff;
        h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
        h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
        h1 += c;     c = (h1 >> 44); h1 &= 0xfffffffffff;
        h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
        h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
        h1 += c;

        /* compute h + -p */
        g0 = h0 + 5; c = (g0 >> 44); g0 &= 0xfffffffffff;
        g1 = h1 + c; c = (g1 >> 44); g1 &= 0xfffffffffff;
        g2 = h2 + c - ((word64)1 << 42);

        /* select h if h < p, or h + -p if h >= p */
        c = (g2 >> ((sizeof(word64) * 8) - 1)) - 1;
        g0 &= c;
        g1 &= c;
        g2 &= c;
        c = ~c;
        h0 = (h0 & c) | g0;
        h1 = (h1 & c) | g1;
        h2 = (h2 & c) | g2;

        /* Store for return */
        ctx->h[0] = h0;
        ctx->h[1] = h1;
        ctx->h[2] = h2;
    }

    ctx->started = 1;
}

/* Multiply two 130-bit numbers in 64-bit registers and reduce.
 *
 * r0-r2 are the first operand and the result.
 * a0-a2 are the second operand.
 */
#define MUL_64(r0, r1, r2, a0, a1, a2)                                       \
    s1 = a1 * (5 << 2);                                                      \
    s2 = a2 * (5 << 2);                                                      \
    MUL(d0, r0, a0); MUL(d, r1, s2); ADD(d0, d); MUL(d, r2, s1); ADD(d0, d); \
    MUL(d1, r0, a1); MUL(d, r1, a0); ADD(d1, d); MUL(d, r2, s2); ADD(d1, d); \
    MUL(d2, r0, a2); MUL(d, r1, a1); ADD(d2, d); MUL(d, r2, a0); ADD(d2, d); \
                                                                             \
                  c = SHR(d0, 44); r0 = LO(d0) & 0xfffffffffff;              \
    ADDLO(d1, c); c = SHR(d1, 44); r1 = LO(d1) & 0xfffffffffff;              \
    ADDLO(d2, c); c = SHR(d2, 42); r2 = LO(d2) & 0x3ffffffffff;              \
    r0  += c * 5; c = (r0 >> 44);  r0 =    r0  & 0xfffffffffff;              \
    r1  += c

/* Store the 130-bit number in 64-bit registers as 26-bit values in 32 bits.
 *
 * r0-r2 contains the 130-bit number in 64-bit registers.
 * r is the address of where to store the 26 bits in 32 result.
 */
#define CONV_64_TO_32(r0, r1, r2, r)                      \
    r[0] = (word32)( r0                    ) & 0x3ffffff; \
    r[1] = (word32)((r0 >> 26) | (r1 << 18)) & 0x3ffffff; \
    r[2] = (word32)( r1 >> 8               ) & 0x3ffffff; \
    r[3] = (word32)((r1 >> 34) | (r2 << 10)) & 0x3ffffff; \
    r[4] = (word32)( r2 >> 16              )
  

static void poly1305_setkey_avx2(Poly1305* ctx, const byte* key)
{
    word64 r0, r1, r2, t0, t1, c;
    word64 r20, r21, r22;
    word64 r30, r31, r32;
    word64 r40, r41, r42;
    word64 s1, s2;
    word128 d0, d1, d2, d;

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    t0 = ((word64*)key)[0];
    t1 = ((word64*)key)[1];
    r0 = ( t0                    ) & 0xffc0fffffff;
    r1 = ((t0 >> 44) | (t1 << 20)) & 0xfffffc0ffff;
    r2 = ((t1 >> 24)             ) & 0x00ffffffc0f;

    __asm__ __volatile__ (
        "vpxor		%%ymm0, %%ymm0, %%ymm0\n\t"
        "vmovdqu	%%ymm0,   (%[h])\n\t"
        "vmovdqu	%%ymm0, 32(%[h])\n\t"
        "vmovdqu	%%ymm0, 64(%[h])\n\t"
        "vmovdqu	%%ymm0,   (%[r0])\n\t"
        "vmovdqu	%%ymm0,   (%[r1])\n\t"
        "vmovdqu	%%ymm0,   (%[r2])\n\t"
        "vmovdqu	%%ymm0,   (%[r3])\n\t"
        "vmovdqu	%%ymm0,   (%[r4])\n\t"
        :
        : [h] "r" (ctx->hh), [r0] "r" (ctx->r0), [r1] "r" (ctx->r1),
          [r2] "r" (ctx->r2), [r3] "r" (ctx->r3), [r4] "r" (ctx->r4)
        : "memory", "ymm0"
    );
    /* h = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;

    /* save pad for later */
    ctx->pad[0] = ((word64*)key)[2];
    ctx->pad[1] = ((word64*)key)[3];

    /* Set 1 for r^0 */
    ctx->r0[0] = 1;

    /* Store r^1 */
    CONV_64_TO_32(r0, r1, r2, ctx->r1);

    /* Calc and store r^2 */
    r20 = r0; r21 = r1; r22 = r2;
    MUL_64(r20, r21, r22, r0, r1, r2);
    CONV_64_TO_32(r20, r21, r22, ctx->r2);

    /* Calc and store r^3 */
    r30 = r20; r31 = r21; r32 = r22;
    MUL_64(r30, r31, r32, r0, r1, r2);
    CONV_64_TO_32(r30, r31, r32, ctx->r3);

    /* Calc and store r^4 */
    r40 = r20; r41 = r21; r42 = r22;
    MUL_64(r40, r41, r42, r20, r21, r22);
    CONV_64_TO_32(r40, r41, r42, ctx->r4);

    /* NULL means use [r^4, r^4, r^4, r^4] */
    ctx->rp[0] = ctx->rp[1] = ctx->rp[2] = ctx->rp[3] = NULL;

    /* Message high bits set unless last partial block. */
    ctx->hibit[0] = ctx->hibit[1] = ctx->hibit[2] = ctx->hibit[3] = 0x1000000;

    ctx->leftover = 0;
    ctx->finished = 0;
    ctx->started = 0;
}

static void poly1305_final_avx2(Poly1305* ctx, byte* mac)
{
    word64 h0, h1, h2, t0, t1, c;

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;

        if (i & 15)
            ctx->buffer[i++] = 1;
        for (; i < POLY1305_BLOCK_SIZE * 4; i++)
            ctx->buffer[i] = 0;

        ctx->hibit[3] = 0;
        if (ctx->leftover < 48)
            ctx->hibit[2] = 0;
        if (ctx->leftover < 32)
            ctx->hibit[1] = 0;
        if (ctx->leftover < 16)
            ctx->hibit[0] = 0;

        if (ctx->started) {
            if (ctx->leftover <= 16) {
                ctx->rp[0] = ctx->r4;
                ctx->rp[1] = ctx->r4;
                ctx->rp[2] = ctx->r3;
                ctx->rp[3] = ctx->r2;
            }
            else if (ctx->leftover <= 32) {
                ctx->rp[0] = ctx->r4;
                ctx->rp[1] = ctx->r4;
                ctx->rp[2] = ctx->r4;
                ctx->rp[3] = ctx->r3;
            }
        }

        poly1305_blocks_avx2(ctx, ctx->buffer, POLY1305_BLOCK_SIZE * 4);
    }
    if (ctx->started) {
        if (ctx->leftover == 0 || ctx->leftover > 48) {
            ctx->rp[0] = ctx->r4;
            ctx->rp[1] = ctx->r3;
            ctx->rp[2] = ctx->r2;
            ctx->rp[3] = ctx->r1;
        }
        else if (ctx->leftover > 32) {
            ctx->rp[0] = ctx->r3;
            ctx->rp[1] = ctx->r2;
            ctx->rp[2] = ctx->r1;
            ctx->rp[3] = ctx->r0;
        }
        else if (ctx->leftover > 16) {
            ctx->rp[0] = ctx->r2;
            ctx->rp[1] = ctx->r1;
            ctx->rp[2] = ctx->r0;
            ctx->rp[3] = ctx->r0;
        }
        else {
            ctx->rp[0] = ctx->r1;
            ctx->rp[1] = ctx->r0;
            ctx->rp[2] = ctx->r0;
            ctx->rp[3] = ctx->r0;
        }
        ctx->finished = 1;
        poly1305_blocks_avx2(ctx, ctx->buffer, POLY1305_BLOCK_SIZE * 4);
    }

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

    /* h = (h + pad) */
    t0 = ctx->pad[0];
    t1 = ctx->pad[1];

    h0 += (( t0                    ) & 0xfffffffffff)    ;
    c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff) + c;
    c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += (((t1 >> 24)             ) & 0x3ffffffffff) + c;
    h2 &= 0x3ffffffffff;

    /* mac = h % (2^128) */
    h0 = ((h0      ) | (h1 << 44));
    h1 = ((h1 >> 20) | (h2 << 24));

    ((word64*)mac)[0] = h0;
    ((word64*)mac)[1] = h1;

    /* zero out the state */
    __asm__ __volatile__ (
        "vpxor		%%ymm0, %%ymm0, %%ymm0\n\t"
        "vmovdqu	%%ymm0,   (%[h])\n\t"
        "vmovdqu	%%ymm0, 32(%[h])\n\t"
        "vmovdqu	%%ymm0, 64(%[h])\n\t"
        "vmovdqu	%%ymm0,   (%[r1])\n\t"
        "vmovdqu	%%ymm0,   (%[r2])\n\t"
        "vmovdqu	%%ymm0,   (%[r3])\n\t"
        "vmovdqu	%%ymm0,   (%[r4])\n\t"
        :
        : [h] "r" (ctx->hh), [r1] "r" (ctx->r1), [r2] "r" (ctx->r2),
          [r3] "r" (ctx->r3), [r4] "r" (ctx->r4)
        : "memory", "ymm0"
    );
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;

    ctx->finished = 0;
    ctx->started = 0;
}
#endif

#elif defined(POLY130564)

    static word64 U8TO64(const byte* p)
    {
        return
            (((word64)(p[0] & 0xff)      ) |
             ((word64)(p[1] & 0xff) <<  8) |
             ((word64)(p[2] & 0xff) << 16) |
             ((word64)(p[3] & 0xff) << 24) |
             ((word64)(p[4] & 0xff) << 32) |
             ((word64)(p[5] & 0xff) << 40) |
             ((word64)(p[6] & 0xff) << 48) |
             ((word64)(p[7] & 0xff) << 56));
    }

    static void U64TO8(byte* p, word64 v) {
        p[0] = (v      ) & 0xff;
        p[1] = (v >>  8) & 0xff;
        p[2] = (v >> 16) & 0xff;
        p[3] = (v >> 24) & 0xff;
        p[4] = (v >> 32) & 0xff;
        p[5] = (v >> 40) & 0xff;
        p[6] = (v >> 48) & 0xff;
        p[7] = (v >> 56) & 0xff;
    }

#else /* if not 64 bit then use 32 bit */

    static word32 U8TO32(const byte *p)
    {
        return
            (((word32)(p[0] & 0xff)      ) |
             ((word32)(p[1] & 0xff) <<  8) |
             ((word32)(p[2] & 0xff) << 16) |
             ((word32)(p[3] & 0xff) << 24));
    }

    static void U32TO8(byte *p, word32 v) {
        p[0] = (v      ) & 0xff;
        p[1] = (v >>  8) & 0xff;
        p[2] = (v >> 16) & 0xff;
        p[3] = (v >> 24) & 0xff;
    }
#endif


static void U32TO64(word32 v, byte* p)
{
    XMEMSET(p, 0, 8);
    p[0] = (v & 0xFF);
    p[1] = (v >>  8) & 0xFF;
    p[2] = (v >> 16) & 0xFF;
    p[3] = (v >> 24) & 0xFF;
}

static void poly1305_blocks(Poly1305* ctx, const unsigned char *m,
                            size_t bytes)
{
#ifdef USE_INTEL_SPEEDUP
    /* AVX2 is handled in wc_Poly1305Update. */
    poly1305_blocks_avx(ctx, m, bytes);
#elif defined(POLY130564)
    const word64 hibit = (ctx->finished) ? 0 : ((word64)1 << 40); /* 1 << 128 */
    word64 r0,r1,r2;
    word64 s1,s2;
    word64 h0,h1,h2;
    word64 c;
    word128 d0,d1,d2,d;

    r0 = ctx->r[0];
    r1 = ctx->r[1];
    r2 = ctx->r[2];

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

    s1 = r1 * (5 << 2);
    s2 = r2 * (5 << 2);

    while (bytes >= POLY1305_BLOCK_SIZE) {
        word64 t0,t1;

        /* h += m[i] */
        t0 = U8TO64(&m[0]);
        t1 = U8TO64(&m[8]);

        h0 += (( t0                    ) & 0xfffffffffff);
        h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff);
        h2 += (((t1 >> 24)             ) & 0x3ffffffffff) | hibit;

        /* h *= r */
        MUL(d0, h0, r0); MUL(d, h1, s2); ADD(d0, d); MUL(d, h2, s1); ADD(d0, d);
        MUL(d1, h0, r1); MUL(d, h1, r0); ADD(d1, d); MUL(d, h2, s2); ADD(d1, d);
        MUL(d2, h0, r2); MUL(d, h1, r1); ADD(d2, d); MUL(d, h2, r0); ADD(d2, d);

        /* (partial) h %= p */
                      c = SHR(d0, 44); h0 = LO(d0) & 0xfffffffffff;
        ADDLO(d1, c); c = SHR(d1, 44); h1 = LO(d1) & 0xfffffffffff;
        ADDLO(d2, c); c = SHR(d2, 42); h2 = LO(d2) & 0x3ffffffffff;
        h0  += c * 5; c = (h0 >> 44);  h0 =    h0  & 0xfffffffffff;
        h1  += c;

        m += POLY1305_BLOCK_SIZE;
        bytes -= POLY1305_BLOCK_SIZE;
    }

    ctx->h[0] = h0;
    ctx->h[1] = h1;
    ctx->h[2] = h2;

#else /* if not 64 bit then use 32 bit */
    const word32 hibit = (ctx->finished) ? 0 : (1 << 24); /* 1 << 128 */
    word32 r0,r1,r2,r3,r4;
    word32 s1,s2,s3,s4;
    word32 h0,h1,h2,h3,h4;
    word64 d0,d1,d2,d3,d4;
    word32 c;


    r0 = ctx->r[0];
    r1 = ctx->r[1];
    r2 = ctx->r[2];
    r3 = ctx->r[3];
    r4 = ctx->r[4];

    s1 = r1 * 5;
    s2 = r2 * 5;
    s3 = r3 * 5;
    s4 = r4 * 5;

    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

    while (bytes >= POLY1305_BLOCK_SIZE) {
        /* h += m[i] */
        h0 += (U8TO32(m+ 0)     ) & 0x3ffffff;
        h1 += (U8TO32(m+ 3) >> 2) & 0x3ffffff;
        h2 += (U8TO32(m+ 6) >> 4) & 0x3ffffff;
        h3 += (U8TO32(m+ 9) >> 6) & 0x3ffffff;
        h4 += (U8TO32(m+12) >> 8) | hibit;

        /* h *= r */
        d0 = ((word64)h0 * r0) + ((word64)h1 * s4) + ((word64)h2 * s3) +
             ((word64)h3 * s2) + ((word64)h4 * s1);
        d1 = ((word64)h0 * r1) + ((word64)h1 * r0) + ((word64)h2 * s4) +
             ((word64)h3 * s3) + ((word64)h4 * s2);
        d2 = ((word64)h0 * r2) + ((word64)h1 * r1) + ((word64)h2 * r0) +
             ((word64)h3 * s4) + ((word64)h4 * s3);
        d3 = ((word64)h0 * r3) + ((word64)h1 * r2) + ((word64)h2 * r1) +
             ((word64)h3 * r0) + ((word64)h4 * s4);
        d4 = ((word64)h0 * r4) + ((word64)h1 * r3) + ((word64)h2 * r2) +
             ((word64)h3 * r1) + ((word64)h4 * r0);

        /* (partial) h %= p */
                      c = (word32)(d0 >> 26); h0 = (word32)d0 & 0x3ffffff;
        d1 += c;      c = (word32)(d1 >> 26); h1 = (word32)d1 & 0x3ffffff;
        d2 += c;      c = (word32)(d2 >> 26); h2 = (word32)d2 & 0x3ffffff;
        d3 += c;      c = (word32)(d3 >> 26); h3 = (word32)d3 & 0x3ffffff;
        d4 += c;      c = (word32)(d4 >> 26); h4 = (word32)d4 & 0x3ffffff;
        h0 += c * 5;  c =  (h0 >> 26); h0 =                h0 & 0x3ffffff;
        h1 += c;

        m += POLY1305_BLOCK_SIZE;
        bytes -= POLY1305_BLOCK_SIZE;
    }

    ctx->h[0] = h0;
    ctx->h[1] = h1;
    ctx->h[2] = h2;
    ctx->h[3] = h3;
    ctx->h[4] = h4;

#endif /* end of 64 bit cpu blocks or 32 bit cpu */
}

static void poly1305_block(Poly1305* ctx, const unsigned char *m)
{
#ifdef USE_INTEL_SPEEDUP
    /* AVX2 does 4 blocks at a time - this func not used. */
    poly1305_block_avx(ctx, m);
#else
    poly1305_blocks(ctx, m, POLY1305_BLOCK_SIZE);
#endif
}


int wc_Poly1305SetKey(Poly1305* ctx, const byte* key, word32 keySz)
{
#if defined(POLY130564)
    word64 t0,t1;
#endif

#ifdef CHACHA_AEAD_TEST
    word32 k;
    printf("Poly key used:\n");
    for (k = 0; k < keySz; k++) {
        printf("%02x", key[k]);
        if ((k+1) % 8 == 0)
            printf("\n");
    }
    printf("\n");
#endif

    if (keySz != 32 || ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef USE_INTEL_SPEEDUP
    if (!cpu_flags_set) {
        intel_flags = cpuid_get_flags();
        cpu_flags_set = 1;
    }
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags))
        poly1305_setkey_avx2(ctx, key);
    else
    #endif
        poly1305_setkey_avx(ctx, key);
#elif defined(POLY130564)

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    t0 = U8TO64(key + 0);
    t1 = U8TO64(key + 8);

    ctx->r[0] = ( t0                    ) & 0xffc0fffffff;
    ctx->r[1] = ((t0 >> 44) | (t1 << 20)) & 0xfffffc0ffff;
    ctx->r[2] = ((t1 >> 24)             ) & 0x00ffffffc0f;

    /* h (accumulator) = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;

    /* save pad for later */
    ctx->pad[0] = U8TO64(key + 16);
    ctx->pad[1] = U8TO64(key + 24);

    ctx->leftover = 0;
    ctx->finished = 0;

#else /* if not 64 bit then use 32 bit */

    /* r &= 0xffffffc0ffffffc0ffffffc0fffffff */
    ctx->r[0] = (U8TO32(key +  0)     ) & 0x3ffffff;
    ctx->r[1] = (U8TO32(key +  3) >> 2) & 0x3ffff03;
    ctx->r[2] = (U8TO32(key +  6) >> 4) & 0x3ffc0ff;
    ctx->r[3] = (U8TO32(key +  9) >> 6) & 0x3f03fff;
    ctx->r[4] = (U8TO32(key + 12) >> 8) & 0x00fffff;

    /* h = 0 */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;

    /* save pad for later */
    ctx->pad[0] = U8TO32(key + 16);
    ctx->pad[1] = U8TO32(key + 20);
    ctx->pad[2] = U8TO32(key + 24);
    ctx->pad[3] = U8TO32(key + 28);

    ctx->leftover = 0;
    ctx->finished = 0;

#endif

    return 0;
}


int wc_Poly1305Final(Poly1305* ctx, byte* mac)
{
#ifdef USE_INTEL_SPEEDUP
#elif defined(POLY130564)

    word64 h0,h1,h2,c;
    word64 g0,g1,g2;
    word64 t0,t1;

#else

    word32 h0,h1,h2,h3,h4,c;
    word32 g0,g1,g2,g3,g4;
    word64 f;
    word32 mask;

#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef USE_INTEL_SPEEDUP
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags))
        poly1305_final_avx2(ctx, mac);
    else
    #endif
        poly1305_final_avx(ctx, mac);
#elif defined(POLY130564)

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i] = 1;
        for (i = i + 1; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 1;
        poly1305_block(ctx, ctx->buffer);
    }

    /* fully carry h */
    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];

                 c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
    h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += c;     c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += c;     c = (h2 >> 42); h2 &= 0x3ffffffffff;
    h0 += c * 5; c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += c;

    /* compute h + -p */
    g0 = h0 + 5; c = (g0 >> 44); g0 &= 0xfffffffffff;
    g1 = h1 + c; c = (g1 >> 44); g1 &= 0xfffffffffff;
    g2 = h2 + c - ((word64)1 << 42);

    /* select h if h < p, or h + -p if h >= p */
    c = (g2 >> ((sizeof(word64) * 8) - 1)) - 1;
    g0 &= c;
    g1 &= c;
    g2 &= c;
    c = ~c;
    h0 = (h0 & c) | g0;
    h1 = (h1 & c) | g1;
    h2 = (h2 & c) | g2;

    /* h = (h + pad) */
    t0 = ctx->pad[0];
    t1 = ctx->pad[1];

    h0 += (( t0                    ) & 0xfffffffffff)    ;
    c = (h0 >> 44); h0 &= 0xfffffffffff;
    h1 += (((t0 >> 44) | (t1 << 20)) & 0xfffffffffff) + c;
    c = (h1 >> 44); h1 &= 0xfffffffffff;
    h2 += (((t1 >> 24)             ) & 0x3ffffffffff) + c;
    h2 &= 0x3ffffffffff;

    /* mac = h % (2^128) */
    h0 = ((h0      ) | (h1 << 44));
    h1 = ((h1 >> 20) | (h2 << 24));

    U64TO8(mac + 0, h0);
    U64TO8(mac + 8, h1);

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;

#else /* if not 64 bit then use 32 bit */

    /* process the remaining block */
    if (ctx->leftover) {
        size_t i = ctx->leftover;
        ctx->buffer[i++] = 1;
        for (; i < POLY1305_BLOCK_SIZE; i++)
            ctx->buffer[i] = 0;
        ctx->finished = 1;
        poly1305_block(ctx, ctx->buffer);
    }

    /* fully carry h */
    h0 = ctx->h[0];
    h1 = ctx->h[1];
    h2 = ctx->h[2];
    h3 = ctx->h[3];
    h4 = ctx->h[4];

                 c = h1 >> 26; h1 = h1 & 0x3ffffff;
    h2 +=     c; c = h2 >> 26; h2 = h2 & 0x3ffffff;
    h3 +=     c; c = h3 >> 26; h3 = h3 & 0x3ffffff;
    h4 +=     c; c = h4 >> 26; h4 = h4 & 0x3ffffff;
    h0 += c * 5; c = h0 >> 26; h0 = h0 & 0x3ffffff;
    h1 +=     c;

    /* compute h + -p */
    g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
    g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
    g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
    g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
    g4 = h4 + c - (1 << 26);

    /* select h if h < p, or h + -p if h >= p */
    mask = (g4 >> ((sizeof(word32) * 8) - 1)) - 1;
    g0 &= mask;
    g1 &= mask;
    g2 &= mask;
    g3 &= mask;
    g4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;
    h2 = (h2 & mask) | g2;
    h3 = (h3 & mask) | g3;
    h4 = (h4 & mask) | g4;

    /* h = h % (2^128) */
    h0 = ((h0      ) | (h1 << 26)) & 0xffffffff;
    h1 = ((h1 >>  6) | (h2 << 20)) & 0xffffffff;
    h2 = ((h2 >> 12) | (h3 << 14)) & 0xffffffff;
    h3 = ((h3 >> 18) | (h4 <<  8)) & 0xffffffff;

    /* mac = (h + pad) % (2^128) */
    f = (word64)h0 + ctx->pad[0]            ; h0 = (word32)f;
    f = (word64)h1 + ctx->pad[1] + (f >> 32); h1 = (word32)f;
    f = (word64)h2 + ctx->pad[2] + (f >> 32); h2 = (word32)f;
    f = (word64)h3 + ctx->pad[3] + (f >> 32); h3 = (word32)f;

    U32TO8(mac + 0, h0);
    U32TO8(mac + 4, h1);
    U32TO8(mac + 8, h2);
    U32TO8(mac + 12, h3);

    /* zero out the state */
    ctx->h[0] = 0;
    ctx->h[1] = 0;
    ctx->h[2] = 0;
    ctx->h[3] = 0;
    ctx->h[4] = 0;
    ctx->r[0] = 0;
    ctx->r[1] = 0;
    ctx->r[2] = 0;
    ctx->r[3] = 0;
    ctx->r[4] = 0;
    ctx->pad[0] = 0;
    ctx->pad[1] = 0;
    ctx->pad[2] = 0;
    ctx->pad[3] = 0;

#endif

    return 0;
}


int wc_Poly1305Update(Poly1305* ctx, const byte* m, word32 bytes)
{
    size_t i;

#ifdef CHACHA_AEAD_TEST
    word32 k;
    printf("Raw input to poly:\n");
    for (k = 0; k < bytes; k++) {
        printf("%02x", m[k]);
        if ((k+1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
#endif

    if (ctx == NULL)
        return BAD_FUNC_ARG;

#ifdef USE_INTEL_SPEEDUP
    #ifdef HAVE_INTEL_AVX2
    if (IS_INTEL_AVX2(intel_flags)) {
        /* handle leftover */
        if (ctx->leftover) {
            size_t want = (4 * POLY1305_BLOCK_SIZE - ctx->leftover);
            if (want > bytes)
                want = bytes;
            for (i = 0; i < want; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            bytes -= (word32)want;
            m += want;
            ctx->leftover += want;
            if (ctx->leftover < 4 * POLY1305_BLOCK_SIZE)
                return 0;
            poly1305_blocks_avx2(ctx, ctx->buffer, 4 * POLY1305_BLOCK_SIZE);
            ctx->leftover = 0;
        }

        /* process full blocks */
        if (bytes >= 4 * POLY1305_BLOCK_SIZE) {
            size_t want = (bytes & ~(4 * POLY1305_BLOCK_SIZE - 1));
            poly1305_blocks_avx2(ctx, m, want);
            m += want;
            bytes -= (word32)want;
        }

        /* store leftover */
        if (bytes) {
            for (i = 0; i < bytes; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            ctx->leftover += bytes;
        }
    }
    else
    #endif
#endif
    {
        /* handle leftover */
        if (ctx->leftover) {
            size_t want = (POLY1305_BLOCK_SIZE - ctx->leftover);
            if (want > bytes)
                want = bytes;
            for (i = 0; i < want; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            bytes -= (word32)want;
            m += want;
            ctx->leftover += want;
            if (ctx->leftover < POLY1305_BLOCK_SIZE)
                return 0;
            poly1305_block(ctx, ctx->buffer);
            ctx->leftover = 0;
        }

        /* process full blocks */
        if (bytes >= POLY1305_BLOCK_SIZE) {
            size_t want = (bytes & ~(POLY1305_BLOCK_SIZE - 1));
            poly1305_blocks(ctx, m, want);
            m += want;
            bytes -= (word32)want;
        }

        /* store leftover */
        if (bytes) {
            for (i = 0; i < bytes; i++)
                ctx->buffer[ctx->leftover + i] = m[i];
            ctx->leftover += bytes;
        }
    }

    return 0;
}


/*  Takes in an initialized Poly1305 struct that has a key loaded and creates
    a MAC (tag) using recent TLS AEAD padding scheme.
    ctx        : Initialized Poly1305 struct to use
    additional : Additional data to use
    addSz      : Size of additional buffer
    input      : Input buffer to create tag from
    sz         : Size of input buffer
    tag        : Buffer to hold created tag
    tagSz      : Size of input tag buffer (must be at least
                 WC_POLY1305_MAC_SZ(16))
 */
int wc_Poly1305_MAC(Poly1305* ctx, byte* additional, word32 addSz,
                    byte* input, word32 sz, byte* tag, word32 tagSz)
{
    int ret;
    byte padding[WC_POLY1305_PAD_SZ - 1];
    word32 paddingLen;
    byte little64[8];

    XMEMSET(padding, 0, sizeof(padding));

    /* sanity check on arguments */
    if (ctx == NULL || input == NULL || tag == NULL ||
                                                   tagSz < WC_POLY1305_MAC_SZ) {
        return BAD_FUNC_ARG;
    }

    /* additional allowed to be 0 */
    if (addSz > 0) {
        if (additional == NULL)
            return BAD_FUNC_ARG;

        /* additional data plus padding */
        if ((ret = wc_Poly1305Update(ctx, additional, addSz)) != 0) {
            return ret;
        }
        paddingLen = -((int)addSz) & (WC_POLY1305_PAD_SZ - 1);
        if (paddingLen) {
            if ((ret = wc_Poly1305Update(ctx, padding, paddingLen)) != 0) {
                return ret;
            }
        }
    }

    /* input plus padding */
    if ((ret = wc_Poly1305Update(ctx, input, sz)) != 0) {
        return ret;
    }
    paddingLen = -((int)sz) & (WC_POLY1305_PAD_SZ - 1);
    if (paddingLen) {
        if ((ret = wc_Poly1305Update(ctx, padding, paddingLen)) != 0) {
            return ret;
        }
    }

    /* size of additional data and input as little endian 64 bit types */
    U32TO64(addSz, little64);
    ret = wc_Poly1305Update(ctx, little64, sizeof(little64));
    if (ret)
    {
        return ret;
    }

    U32TO64(sz, little64);
    ret = wc_Poly1305Update(ctx, little64, sizeof(little64));
    if (ret)
    {
        return ret;
    }

    /* Finalize the auth tag */
    ret = wc_Poly1305Final(ctx, tag);

    return ret;

}
#endif /* HAVE_POLY1305 */

