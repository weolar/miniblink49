/* fe_x25519_x64.i
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
    #include <wolfssl/wolfcrypt/cpuid.h>
#endif

#ifdef HAVE_INTEL_AVX2
static void fe_mul_avx2(fe r, const fe a, const fe b);
static void fe_sq_avx2(fe r, const fe a);
static void fe_sq2_avx2(fe r, const fe a);
#endif
static void fe_mul_x64(fe r, const fe a, const fe b);
static void fe_sq_x64(fe r, const fe a);
static void fe_sq2_x64(fe r, const fe a);

static void (*fe_mul_p)(fe r, const fe a, const fe b) = fe_mul_x64;
static void (*fe_sq_p)(fe r, const fe a) = fe_sq_x64;
static void (*fe_sq2_p)(fe r, const fe a) = fe_sq2_x64;

#ifdef HAVE_INTEL_AVX2

static int cpuFlagsSet = 0;
static int intelFlags;

#endif

void fe_init()
{
#ifdef HAVE_INTEL_AVX2
    if (cpuFlagsSet)
        return;

    intelFlags = cpuid_get_flags();
    cpuFlagsSet = 1;

    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_mul_p = fe_mul_avx2;
        fe_sq_p = fe_sq_avx2;
        fe_sq2_p = fe_sq2_avx2;
    }
#endif
}

/* Convert a number represented as an array of bytes to an array of words with
 * 64-bits of data in each word.
 *
 * in   An array of bytes.
 * out  An array of words.
 */
void fe_frombytes(fe out, const unsigned char *in)
{
#ifdef LITTLE_ENDIAN_ORDER
    XMEMCPY(out, in, 32);
#else
    out[0] = (((int64_t)in[ 0]) <<  0)
           | (((int64_t)in[ 1]) <<  8)
           | (((int64_t)in[ 2]) << 16)
           | (((int64_t)in[ 3]) << 24)
           | (((int64_t)in[ 4]) << 32)
           | (((int64_t)in[ 5]) << 40)
           | (((int64_t)in[ 6]) << 48)
           | (((int64_t)in[ 7]) << 56);
    out[1] = (((int64_t)in[ 8]) <<  0)
           | (((int64_t)in[ 9]) <<  8)
           | (((int64_t)in[10]) << 16)
           | (((int64_t)in[11]) << 24)
           | (((int64_t)in[12]) << 32)
           | (((int64_t)in[13]) << 40)
           | (((int64_t)in[14]) << 48)
           | (((int64_t)in[15]) << 56);
    out[2] = (((int64_t)in[16]) <<  0)
           | (((int64_t)in[17]) <<  8)
           | (((int64_t)in[18]) << 16)
           | (((int64_t)in[19]) << 24)
           | (((int64_t)in[20]) << 32)
           | (((int64_t)in[21]) << 40)
           | (((int64_t)in[22]) << 48)
           | (((int64_t)in[23]) << 56);
    out[3] = (((int64_t)in[24]) <<  0)
           | (((int64_t)in[25]) <<  8)
           | (((int64_t)in[26]) << 16)
           | (((int64_t)in[27]) << 24)
           | (((int64_t)in[28]) << 32)
           | (((int64_t)in[29]) << 40)
           | (((int64_t)in[30]) << 48)
           | (((int64_t)in[31]) << 56);
#endif
    out[3] &= 0x7fffffffffffffff;
}

/* Convert a number represented as an array of words to an array of bytes.
 * If greater than the mod, modulo reduced by the prime 2^255 - 19.
 *
 * n    An array of words.
 * out  An array of bytes.
 */
void fe_tobytes(unsigned char *out, const fe n)
{
    __asm__ __volatile__ (
        "movq	$0x7fffffffffffffff, %%r10\n\t"
        "movq	0(%[n]), %%rax\n\t"
        "movq	8(%[n]), %%rcx\n\t"
        "addq	$19, %%rax\n\t"
        "movq	16(%[n]), %%rdx\n\t"
        "adcq	$0, %%rcx\n\t"
        "movq	24(%[n]), %%r8\n\t"
        "adcq	$0, %%rdx\n\t"
        "adcq	$0, %%r8\n\t"
        "shrq	$63, %%r8\n\t"
        "movq	0(%[n]), %%rax\n\t"
        "imulq	$19, %%r8, %%r9\n\t"
        "movq	8(%[n]), %%rcx\n\t"
        "addq	%%r9, %%rax\n\t"
        "movq	16(%[n]), %%rdx\n\t"
        "adcq	$0, %%rcx\n\t"
        "movq	24(%[n]), %%r8\n\t"
        "adcq	$0, %%rdx\n\t"
        "movq	%%rax, 0(%[n])\n\t"
        "adcq	$0, %%r8\n\t"
        "movq	%%rcx, 8(%[n])\n\t"
        "andq	%%r10, %%r8\n\t"
        "movq	%%rdx, 16(%[n])\n\t"
        "movq	%%r8, 24(%[n])\n\t"
        :
        : [n] "r" (n)
        : "memory", "rax", "rcx", "rdx", "r8", "r9", "r10"
    );
#ifdef LITTLE_ENDIAN_ORDER
    XMEMCPY(out, n, 32);
#else
    out[0] = n[0] >>  0;
    out[1] = n[0] >>  8;
    out[2] = n[0] >> 16;
    out[3] = n[0] >> 24;
    out[4] = n[0] >> 32;
    out[5] = n[0] >> 40;
    out[6] = n[0] >> 48;
    out[7] = n[0] >> 56;
    out[8] = n[1] >>  0;
    out[9] = n[1] >>  8;
    out[10] = n[1] >> 16;
    out[11] = n[1] >> 24;
    out[12] = n[1] >> 32;
    out[13] = n[1] >> 40;
    out[14] = n[1] >> 48;
    out[15] = n[1] >> 56;
    out[16] = n[2] >>  0;
    out[17] = n[2] >>  8;
    out[18] = n[2] >> 16;
    out[19] = n[2] >> 24;
    out[20] = n[2] >> 32;
    out[21] = n[2] >> 40;
    out[22] = n[2] >> 48;
    out[23] = n[2] >> 56;
    out[24] = n[3] >>  0;
    out[25] = n[3] >>  8;
    out[26] = n[3] >> 16;
    out[27] = n[3] >> 24;
    out[28] = n[3] >> 32;
    out[29] = n[3] >> 40;
    out[30] = n[3] >> 48;
    out[31] = n[3] >> 56;
#endif
}

/* Set the field element to 1.
 *
 * n  The field element number.
 */
void fe_1(fe n)
{
    n[0] = 0x0000000000000001;
    n[1] = 0x0000000000000000;
    n[2] = 0x0000000000000000;
    n[3] = 0x0000000000000000;
}

/* Set the field element to 0.
 *
 * n  The field element number.
 */
void fe_0(fe n)
{
    n[0] = 0x0000000000000000;
    n[1] = 0x0000000000000000;
    n[2] = 0x0000000000000000;
    n[3] = 0x0000000000000000;
}

/* Copy field element a into field element r.
 *
 * r  Field element to copy into.
 * a  Field element to copy.
 */
void fe_copy(fe r, const fe a)
{
    r[0] = a[0];
    r[1] = a[1];
    r[2] = a[2];
    r[3] = a[3];
}

/* Constant time, conditional swap of field elements a and b.
 *
 * a  A field element.
 * b  A field element.
 * c  If 1 then swap and if 0 then don't swap.
 */
static INLINE void fe_cswap_int(fe a, fe b, int c)
{
    __asm__ __volatile__ (
        "movslq	%[c], %%rax\n\t"
        "movq	0(%[a]), %%rcx\n\t"
        "movq	8(%[a]), %%rdx\n\t"
        "movq	16(%[a]), %%r8\n\t"
        "movq	24(%[a]), %%r9\n\t"
        "negq	%%rax\n\t"
        "xorq	0(%[b]), %%rcx\n\t"
        "xorq	8(%[b]), %%rdx\n\t"
        "xorq	16(%[b]), %%r8\n\t"
        "xorq	24(%[b]), %%r9\n\t"
        "andq	%%rax, %%rcx\n\t"
        "andq	%%rax, %%rdx\n\t"
        "andq	%%rax, %%r8\n\t"
        "andq	%%rax, %%r9\n\t"
        "xorq	%%rcx,  0(%[a])\n\t"
        "xorq	%%rdx,  8(%[a])\n\t"
        "xorq	%%r8, 16(%[a])\n\t"
        "xorq	%%r9, 24(%[a])\n\t"
        "xorq	%%rcx,  0(%[b])\n\t"
        "xorq	%%rdx,  8(%[b])\n\t"
        "xorq	%%r8, 16(%[b])\n\t"
        "xorq	%%r9, 24(%[b])\n\t"
        :
        : [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "memory", "rax", "rcx", "rdx", "r8", "r9"
    );
}

void fe_cswap(fe a, fe b, int c)
{
    fe_cswap_int(a, b, c);
}

/* Subtract b from a into r. (r = a - b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
static INLINE void fe_sub_int(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "movq	$-19, %%r11\n\t"
        "movq	0(%[a]), %%rax\n\t"
        "movq	8(%[a]), %%rdx\n\t"
        "subq	0(%[b]), %%rax\n\t"
        "movq	16(%[a]), %%r8\n\t"
        "sbbq	8(%[b]), %%rdx\n\t"
        "movq	24(%[a]), %%r9\n\t"
        "sbbq	16(%[b]), %%r8\n\t"
        "movq	$0, %%r10\n\t"
        "sbbq	24(%[b]), %%r9\n\t"
        "sbbq	$0, %%r10\n\t"
        "# Mask the modulus\n\t"
        "andq	%%r10, %%r11\n\t"
        "andq	%%r10, %%rcx\n\t"
        "# Add modulus (if underflow)\n\t"
        "addq	%%r11, %%rax\n\t"
        "adcq	%%r10, %%rdx\n\t"
        "movq	%%rax, 0(%[r])\n\t"
        "adcq	%%r10, %%r8\n\t"
        "movq	%%rdx, 8(%[r])\n\t"
        "adcq	%%rcx, %%r9\n\t"
        "movq	%%r8, 16(%[r])\n\t"
        "movq	%%r9, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "rax", "rcx", "rdx", "r8", "r9", "r10", "r11", "memory"
    );
}

void fe_sub(fe r, const fe a, const fe b)
{
    fe_sub_int(r, a, b);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
static INLINE void fe_add_int(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        "movq	0(%[a]), %%rax\n\t"
        "movq	8(%[a]), %%rdx\n\t"
        "addq	0(%[b]), %%rax\n\t"
        "movq	16(%[a]), %%r8\n\t"
        "adcq	8(%[b]), %%rdx\n\t"
        "movq	24(%[a]), %%r10\n\t"
        "adcq	16(%[b]), %%r8\n\t"
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "adcq	24(%[b]), %%r10\n\t"
        "movq	$-19, %%r11\n\t"
        "movq	%%r10, %%r9\n\t"
        "sarq	$63, %%r10\n\t"
        "# Mask the modulus\n\t"
        "andq	%%r10, %%r11\n\t"
        "andq	%%r10, %%rcx\n\t"
        "# Sub modulus (if overflow)\n\t"
        "subq	%%r11, %%rax\n\t"
        "sbbq	%%r10, %%rdx\n\t"
        "movq	%%rax, 0(%[r])\n\t"
        "sbbq	%%r10, %%r8\n\t"
        "movq	%%rdx, 8(%[r])\n\t"
        "sbbq	%%rcx, %%r9\n\t"
        "movq	%%r8, 16(%[r])\n\t"
        "movq	%%r9, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "rax", "rcx", "rdx", "r8", "r9", "r10", "r11", "memory"
    );
}

void fe_add(fe r, const fe a, const fe b)
{
    fe_add_int(r, a, b);
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_mul(fe r, const fe a, const fe b)
{
    (*fe_mul_p)(r, a, b);
}

#ifdef HAVE_INTEL_AVX2
static INLINE void fe_mul_avx2(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        "#  A[0] * B[0]\n\t"
        "movq   0(%[b]), %%rdx\n\t"
        "mulxq  0(%[a]), %%r8, %%r9\n\t"
        "#  A[2] * B[0]\n\t"
        "mulxq  16(%[a]), %%r10, %%r11\n\t"
        "#  A[1] * B[0]\n\t"
        "mulxq  8(%[a]), %%rax, %%rcx\n\t"
        "xorq   %%r15, %%r15\n\t"
        "adcxq  %%rax, %%r9\n\t"
        "#  A[1] * B[3]\n\t"
        "movq   24(%[b]), %%rdx\n\t"
        "mulxq  8(%[a]), %%r12, %%r13\n\t"
        "adcxq  %%rcx, %%r10\n\t"
        "#  A[0] * B[1]\n\t"
        "movq   8(%[b]), %%rdx\n\t"
        "mulxq  0(%[a]), %%rax, %%rcx\n\t"
        "adoxq  %%rax, %%r9\n\t"
        "#  A[2] * B[1]\n\t"
        "mulxq  16(%[a]), %%rax, %%r14\n\t"
        "adoxq  %%rcx, %%r10\n\t"
        "adcxq  %%rax, %%r11\n\t"
        "#  A[1] * B[2]\n\t"
        "movq   16(%[b]), %%rdx\n\t"
        "mulxq  8(%[a]), %%rax, %%rcx\n\t"
        "adcxq  %%r14, %%r12\n\t"
        "adoxq  %%rax, %%r11\n\t"
        "adcxq  %%r15, %%r13\n\t"
        "adoxq  %%rcx, %%r12\n\t"
        "#  A[0] * B[2]\n\t"
        "mulxq  0(%[a]), %%rax, %%rcx\n\t"
        "adoxq  %%r15, %%r13\n\t"
        "xorq   %%r14, %%r14\n\t"
        "adcxq  %%rax, %%r10\n\t"
        "#  A[1] * B[1]\n\t"
        "movq   8(%[b]), %%rdx\n\t"
        "mulxq  8(%[a]), %%rdx, %%rax\n\t"
        "adcxq  %%rcx, %%r11\n\t"
        "adoxq  %%rdx, %%r10\n\t"
        "#  A[3] * B[1]\n\t"
        "movq   8(%[b]), %%rdx\n\t"
        "adoxq  %%rax, %%r11\n\t"
        "mulxq  24(%[a]), %%rax, %%rcx\n\t"
        "adcxq  %%rax, %%r12\n\t"
        "#  A[2] * B[2]\n\t"
        "movq   16(%[b]), %%rdx\n\t"
        "mulxq  16(%[a]), %%rdx, %%rax\n\t"
        "adcxq  %%rcx, %%r13\n\t"
        "adoxq  %%rdx, %%r12\n\t"
        "#  A[3] * B[3]\n\t"
        "movq   24(%[b]), %%rdx\n\t"
        "adoxq  %%rax, %%r13\n\t"
        "mulxq  24(%[a]), %%rax, %%rcx\n\t"
        "adoxq  %%r15, %%r14\n\t"
        "adcxq  %%rax, %%r14\n\t"
        "#  A[0] * B[3]\n\t"
        "mulxq  0(%[a]), %%rdx, %%rax\n\t"
        "adcxq  %%rcx, %%r15\n\t"
        "xorq   %%rcx, %%rcx\n\t"
        "adcxq  %%rdx, %%r11\n\t"
        "#  A[3] * B[0]\n\t"
        "movq   0(%[b]), %%rdx\n\t"
        "adcxq  %%rax, %%r12\n\t"
        "mulxq  24(%[a]), %%rdx, %%rax\n\t"
        "adoxq  %%rdx, %%r11\n\t"
        "adoxq  %%rax, %%r12\n\t"
        "#  A[2] * B[3]\n\t"
        "movq   24(%[b]), %%rdx\n\t"
        "mulxq  16(%[a]), %%rdx, %%rax\n\t"
        "adcxq  %%rdx, %%r13\n\t"
        "#  A[3] * B[2]\n\t"
        "movq   16(%[b]), %%rdx\n\t"
        "adcxq  %%rax, %%r14\n\t"
        "mulxq  24(%[a]), %%rax, %%rdx\n\t"
        "adcxq  %%rcx, %%r15\n\t"
        "adoxq  %%rax, %%r13\n\t"
        "adoxq  %%rdx, %%r14\n\t"
        "adoxq  %%rcx, %%r15\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "#  Move top half into t4-t7 and remove top bit from t3\n\t"
        "shldq	$1, %%r14, %%r15\n\t"
        "shldq	$1, %%r13, %%r14\n\t"
        "shldq	$1, %%r12, %%r13\n\t"
        "shldq	$1, %%r11, %%r12\n\t"
        "andq	%%rcx, %%r11\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rdx\n\t"
        "xorq	%%rcx, %%rcx\n\t"
        "mulxq	%%r12, %%rax, %%r12\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r8\n\t"
        "adoxq	%%r12, %%r9\n\t"
        "mulxq	%%r13, %%rax, %%r13\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r9\n\t"
        "adoxq	%%r13, %%r10\n\t"
        "mulxq	%%r14, %%rax, %%r14\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r10\n\t"
        "adoxq	%%r14, %%r11\n\t"
        "mulxq	%%r15, %%r15, %%rdx\n\t"
        "adcxq	%%r15, %%r11\n\t"
        "adoxq	%%rcx, %%rdx\n\t"
        "adcxq	%%rcx, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r11, %%rdx\n\t"
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rcx, %%r11\n\t"
        "addq	%%rax, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "adcq	$0, %%r11\n\t"
        "movq	%%r8, 0(%[r])\n\t"
        "movq	%%r9, 8(%[r])\n\t"
        "movq	%%r10, 16(%[r])\n\t"
        "movq	%%r11, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "rax", "rdx", "rcx",
          "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    );
}
#endif /* HAVE_INTEL_AVX2 */

static INLINE void fe_mul_x64(fe r, const fe a, const fe b)
{
    __asm__ __volatile__ (
        "#  A[0] * B[0]\n\t"
        "movq   0(%[b]), %%rax\n\t"
        "mulq   0(%[a])\n\t"
        "movq   %%rax, %%rcx\n\t"
        "movq   %%rdx, %%r8\n\t"
        "#  A[0] * B[1]\n\t"
        "movq   8(%[b]), %%rax\n\t"
        "mulq   0(%[a])\n\t"
        "xorq   %%r9, %%r9\n\t"
        "addq   %%rax, %%r8\n\t"
        "adcq   %%rdx, %%r9\n\t"
        "#  A[1] * B[0]\n\t"
        "movq   0(%[b]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "xorq   %%r10, %%r10\n\t"
        "addq   %%rax, %%r8\n\t"
        "adcq   %%rdx, %%r9\n\t"
        "adcq   $0, %%r10\n\t"
        "#  A[0] * B[2]\n\t"
        "movq   16(%[b]), %%rax\n\t"
        "mulq   0(%[a])\n\t"
        "addq   %%rax, %%r9\n\t"
        "adcq   %%rdx, %%r10\n\t"
        "#  A[1] * B[1]\n\t"
        "movq   8(%[b]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "xorq   %%r11, %%r11\n\t"
        "addq   %%rax, %%r9\n\t"
        "adcq   %%rdx, %%r10\n\t"
        "adcq   $0, %%r11\n\t"
        "#  A[2] * B[0]\n\t"
        "movq   0(%[b]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "addq   %%rax, %%r9\n\t"
        "adcq   %%rdx, %%r10\n\t"
        "adcq   $0, %%r11\n\t"
        "#  A[0] * B[3]\n\t"
        "movq   24(%[b]), %%rax\n\t"
        "mulq   0(%[a])\n\t"
        "xorq   %%r12, %%r12\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[1] * B[2]\n\t"
        "movq   16(%[b]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[2] * B[1]\n\t"
        "movq   8(%[b]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[3] * B[0]\n\t"
        "movq   0(%[b]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[1] * B[3]\n\t"
        "movq   24(%[b]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "xorq   %%r13, %%r13\n\t"
        "addq   %%rax, %%r11\n\t"
        "adcq   %%rdx, %%r12\n\t"
        "adcq   $0, %%r13\n\t"
        "#  A[2] * B[2]\n\t"
        "movq   16(%[b]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "addq   %%rax, %%r11\n\t"
        "adcq   %%rdx, %%r12\n\t"
        "adcq   $0, %%r13\n\t"
        "#  A[3] * B[1]\n\t"
        "movq   8(%[b]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r11\n\t"
        "adcq   %%rdx, %%r12\n\t"
        "adcq   $0, %%r13\n\t"
        "#  A[2] * B[3]\n\t"
        "movq   24(%[b]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "xorq   %%r14, %%r14\n\t"
        "addq   %%rax, %%r12\n\t"
        "adcq   %%rdx, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "#  A[3] * B[2]\n\t"
        "movq   16(%[b]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r12\n\t"
        "adcq   %%rdx, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "#  A[3] * B[3]\n\t"
        "movq   24(%[b]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r13\n\t"
        "adcq   %%rdx, %%r14\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rbx\n\t"
        "#  Move top half into t4-t7 and remove top bit from t3\n\t"
        "shldq	$1, %%r13, %%r14\n\t"
        "shldq	$1, %%r12, %%r13\n\t"
        "shldq	$1, %%r11, %%r12\n\t"
        "shldq	$1, %%r10, %%r11\n\t"
        "andq %%rbx, %%r10\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rax\n\t"
        "mulq	%%r11\n\t"
        "xorq	%%r11, %%r11\n\t"
        "addq	%%rax, %%rcx\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r11\n\t"
        "mulq	%%r12\n\t"
        "xorq	%%r12, %%r12\n\t"
        "addq	%%rax, %%r8\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r12\n\t"
        "mulq	%%r13\n\t"
        "xorq	%%r13, %%r13\n\t"
        "addq	%%rax, %%r9\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r13\n\t"
        "mulq	%%r14\n\t"
        "#  Add remaining product results in\n\t"
        "addq	%%r11, %%r8\n\t"
        "adcq	%%r12, %%r9\n\t"
        "adcq	%%r13, %%r10\n\t"
        "addq	%%rax, %%r10\n\t"
        "adcq	$0, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r10, %%rdx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rbx, %%r10\n\t"
        "addq	%%rax, %%rcx\n\t"
        "adcq	$0, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "movq	%%rcx, 0(%[r])\n\t"
        "movq	%%r8, 8(%[r])\n\t"
        "movq	%%r9, 16(%[r])\n\t"
        "movq	%%r10, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "rax", "rbx", "rdx", "rcx", "r8", "r9",
          "r10", "r11", "r12", "r13", "r14"
    );
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_sq(fe r, const fe a)
{
    (*fe_sq_p)(r, a);
}

#ifdef HAVE_INTEL_AVX2
static INLINE void fe_sq_avx2(fe r, const fe a)
{
    __asm__ __volatile__ (
        "# A[0] * A[1]\n\t"
        "movq   0(%[a]), %%rdx\n\t"
        "mulxq  8(%[a]), %%r9, %%r10\n\t"
        "# A[0] * A[3]\n\t"
        "mulxq  24(%[a]), %%r11, %%r12\n\t"
        "# A[2] * A[1]\n\t"
        "movq   16(%[a]), %%rdx\n\t"
        "mulxq  8(%[a]), %%rcx, %%rbx\n\t"
        "xorq   %%r15, %%r15\n\t"
        "adoxq  %%rcx, %%r11\n\t"
        "# A[2] * A[3]\n\t"
        "mulxq  24(%[a]), %%r13, %%r14\n\t"
        "adoxq  %%rbx, %%r12\n\t"
        "# A[2] * A[0]\n\t"
        "mulxq  0(%[a]), %%rcx, %%rbx\n\t"
        "adoxq  %%r15, %%r13\n\t"
        "adcxq  %%rcx, %%r10\n\t"
        "adoxq  %%r15, %%r14\n\t"
        "# A[1] * A[3]\n\t"
        "movq   8(%[a]), %%rdx\n\t"
        "mulxq  24(%[a]), %%rax, %%r8\n\t"
        "adcxq  %%rbx, %%r11\n\t"
        "adcxq  %%rax, %%r12\n\t"
        "adcxq  %%r8, %%r13\n\t"
        "adcxq  %%r15, %%r14\n\t"
        "# Double with Carry Flag\n\t"
        "xorq   %%r15, %%r15\n\t"
        "# A[0] * A[0]\n\t"
        "movq   0(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%r8, %%rax\n\t"
        "adcxq  %%r9, %%r9\n\t"
        "# A[1] * A[1]\n\t"
        "movq   8(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rcx, %%rbx\n\t"
        "adcxq  %%r10, %%r10\n\t"
        "adoxq  %%rax, %%r9\n\t"
        "adcxq  %%r11, %%r11\n\t"
        "adoxq  %%rcx, %%r10\n\t"
        "# A[2] * A[2]\n\t"
        "movq   16(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rax, %%rcx\n\t"
        "adcxq  %%r12, %%r12\n\t"
        "adoxq  %%rbx, %%r11\n\t"
        "adcxq  %%r13, %%r13\n\t"
        "adoxq  %%rax, %%r12\n\t"
        "# A[3] * A[3]\n\t"
        "movq   24(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rax, %%rbx\n\t"
        "adcxq  %%r14, %%r14\n\t"
        "adoxq  %%rcx, %%r13\n\t"
        "adcxq  %%r15, %%r15\n\t"
        "adoxq  %%rax, %%r14\n\t"
        "adoxq  %%rbx, %%r15\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "#  Move top half into t4-t7 and remove top bit from t3\n\t"
        "shldq	$1, %%r14, %%r15\n\t"
        "shldq	$1, %%r13, %%r14\n\t"
        "shldq	$1, %%r12, %%r13\n\t"
        "shldq	$1, %%r11, %%r12\n\t"
        "andq	%%rcx, %%r11\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rdx\n\t"
        "xorq	%%rcx, %%rcx\n\t"
        "mulxq	%%r12, %%rax, %%r12\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r8\n\t"
        "adoxq	%%r12, %%r9\n\t"
        "mulxq	%%r13, %%rax, %%r13\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r9\n\t"
        "adoxq	%%r13, %%r10\n\t"
        "mulxq	%%r14, %%rax, %%r14\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r10\n\t"
        "adoxq	%%r14, %%r11\n\t"
        "mulxq	%%r15, %%r15, %%rdx\n\t"
        "adcxq	%%r15, %%r11\n\t"
        "adoxq	%%rcx, %%rdx\n\t"
        "adcxq	%%rcx, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r11, %%rdx\n\t"
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rcx, %%r11\n\t"
        "addq	%%rax, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "adcq	$0, %%r11\n\t"
        "movq	%%r8, 0(%[r])\n\t"
        "movq	%%r9, 8(%[r])\n\t"
        "movq	%%r10, 16(%[r])\n\t"
        "movq	%%r11, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rdx", "rcx", "rbx", "r8", "r9", "r10", "r11",
          "r12", "r13", "r14", "r15"
    );
}
#endif /* HAVE_INTEL_AVX2 */

static INLINE void fe_sq_x64(fe r, const fe a)
{
    __asm__ __volatile__ (
        "#  A[0] * A[1]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "movq   %%rax, %%r8\n\t"
        "movq   %%rdx, %%r9\n\t"
        "#  A[0] * A[2]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "xorq   %%r10, %%r10\n\t"
        "addq   %%rax, %%r9\n\t"
        "adcq   %%rdx, %%r10\n\t"
        "#  A[0] * A[3]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "xorq   %%r11, %%r11\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "#  A[1] * A[2]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "xorq   %%r12, %%r12\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[1] * A[3]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r11\n\t"
        "adcq   %%rdx, %%r12\n\t"
        "#  A[2] * A[3]\n\t"
        "movq   16(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "xorq   %%r13, %%r13\n\t"
        "addq   %%rax, %%r12\n\t"
        "adcq   %%rdx, %%r13\n\t"
        "# Double\n\t"
        "xorq   %%r14, %%r14\n\t"
        "addq   %%r8, %%r8\n\t"
        "adcq   %%r9, %%r9\n\t"
        "adcq   %%r10, %%r10\n\t"
        "adcq   %%r11, %%r11\n\t"
        "adcq   %%r12, %%r12\n\t"
        "adcq   %%r13, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "#  A[0] * A[0]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "movq   %%rax, %%rcx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[1] * A[1]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%r15, %%r8\n\t"
        "adcq   %%rax, %%r9\n\t"
        "adcq   $0, %%rdx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[2] * A[2]\n\t"
        "movq   16(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%r15, %%r10\n\t"
        "adcq   %%rax, %%r11\n\t"
        "adcq   $0, %%rdx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[3] * A[3]\n\t"
        "movq   24(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%rax, %%r13\n\t"
        "adcq   %%rdx, %%r14\n\t"
        "addq   %%r15, %%r12\n\t"
        "adcq   $0, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rbx\n\t"
        "#  Move top half into t4-t7 and remove top bit from t3\n\t"
        "shldq	$1, %%r13, %%r14\n\t"
        "shldq	$1, %%r12, %%r13\n\t"
        "shldq	$1, %%r11, %%r12\n\t"
        "shldq	$1, %%r10, %%r11\n\t"
        "andq %%rbx, %%r10\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rax\n\t"
        "mulq	%%r11\n\t"
        "xorq	%%r11, %%r11\n\t"
        "addq	%%rax, %%rcx\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r11\n\t"
        "mulq	%%r12\n\t"
        "xorq	%%r12, %%r12\n\t"
        "addq	%%rax, %%r8\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r12\n\t"
        "mulq	%%r13\n\t"
        "xorq	%%r13, %%r13\n\t"
        "addq	%%rax, %%r9\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r13\n\t"
        "mulq	%%r14\n\t"
        "#  Add remaining product results in\n\t"
        "addq	%%r11, %%r8\n\t"
        "adcq	%%r12, %%r9\n\t"
        "adcq	%%r13, %%r10\n\t"
        "addq	%%rax, %%r10\n\t"
        "adcq	$0, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r10, %%rdx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rbx, %%r10\n\t"
        "addq	%%rax, %%rcx\n\t"
        "adcq	$0, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "movq	%%rcx, 0(%[r])\n\t"
        "movq	%%r8, 8(%[r])\n\t"
        "movq	%%r9, 16(%[r])\n\t"
        "movq	%%r10, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rbx", "rdx", "rcx", "r8", "r9", "r10", "r11", "r12",
          "r13", "r14", "r15"
    );
}

/* Multiply a by 121666 and put result in r. (r = 121666 * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
static INLINE void fe_mul121666_int(fe r, fe a)
{
    __asm__ __volatile__ (
        "movq	$0x7fffffffffffffff, %%rcx\n\t"
        "movq	$121666, %%rax\n\t"
        "mulq	0(%[a])\n\t"
        "xorq	%%r10, %%r10\n\t"
        "movq	%%rax, %%r8\n\t"
        "movq	%%rdx, %%r9\n\t"
        "movq	$121666, %%rax\n\t"
        "mulq	8(%[a])\n\t"
        "xorq	%%r11, %%r11\n\t"
        "addq	%%rax, %%r9\n\t"
        "adcq	%%rdx, %%r10\n\t"
        "movq	$121666, %%rax\n\t"
        "mulq	16(%[a])\n\t"
        "xorq	%%r12, %%r12\n\t"
        "addq	%%rax, %%r10\n\t"
        "adcq	%%rdx, %%r11\n\t"
        "movq	$121666, %%rax\n\t"
        "mulq	24(%[a])\n\t"
        "addq	%%rax, %%r11\n\t"
        "adcq	%%rdx, %%r12\n\t"
        "shldq	$1, %%r11, %%r12\n\t"
        "andq	%%rcx, %%r11\n\t"
        "movq	$19, %%rax\n\t"
        "mulq	%%r12\n\t"
        "addq	%%rax, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "adcq	$0, %%r11\n\t"
        "movq	%%r8, 0(%[r])\n\t"
        "movq	%%r9, 8(%[r])\n\t"
        "movq	%%r10, 16(%[r])\n\t"
        "movq	%%r11, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12"
    );
}

void fe_mul121666(fe r, fe a)
{
    fe_mul121666_int(r, a);
}

/* Find the inverse of a modulo 2^255 - 1 and put result in r.
 * (r * a) mod (2^255 - 1) = 1
 * Implementation is constant time.
 *
 * r  A field element.
 * a  A field element.
 */
void fe_invert(fe r, const fe a)
{
    fe  t0, t1, t2, t3;
    int i;

    /* a ^ (2^255 - 21) */
    fe_sq(t0,  a); for (i = 1; i <   1; ++i) fe_sq(t0, t0);
    fe_sq(t1, t0); for (i = 1; i <   2; ++i) fe_sq(t1, t1); fe_mul(t1,  a, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t2, t0); for (i = 1; i <   1; ++i) fe_sq(t2, t2); fe_mul(t1, t1, t2);
    fe_sq(t2, t1); for (i = 1; i <   5; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t2, t1); for (i = 1; i <  10; ++i) fe_sq(t2, t2); fe_mul(t2, t2, t1);
    fe_sq(t3, t2); for (i = 1; i <  20; ++i) fe_sq(t3, t3); fe_mul(t2, t3, t2);
    fe_sq(t2, t2); for (i = 1; i <  10; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t2, t1); for (i = 1; i <  50; ++i) fe_sq(t2, t2); fe_mul(t2, t2, t1);
    fe_sq(t3, t2); for (i = 1; i < 100; ++i) fe_sq(t3, t3); fe_mul(t2, t3, t2);
    fe_sq(t2, t2); for (i = 1; i <  50; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <   5; ++i) fe_sq(t1, t1); fe_mul( r, t1, t0);
}

#ifdef HAVE_INTEL_AVX2
/* Find the inverse of a modulo 2^255 - 1 and put result in r.
 * (r * a) mod (2^255 - 1) = 1
 * Implementation is constant time.
 *
 * r  A field element.
 * a  A field element.
 */
static void fe_invert_avx2(fe r, const fe a)
{
    fe  t0, t1, t2, t3;
    int i;

    /* a ^ (2^255 - 21) */
    fe_sq_avx2(t0,  a); for (i = 1; i <   1; ++i) fe_sq_avx2(t0, t0);
    fe_sq_avx2(t1, t0); for (i = 1; i <   2; ++i) fe_sq_avx2(t1, t1); fe_mul_avx2(t1,  a, t1);
    fe_mul_avx2(t0, t0, t1);
    fe_sq_avx2(t2, t0); for (i = 1; i <   1; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t1, t1, t2);
    fe_sq_avx2(t2, t1); for (i = 1; i <   5; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t1, t2, t1);
    fe_sq_avx2(t2, t1); for (i = 1; i <  10; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t2, t2, t1);
    fe_sq_avx2(t3, t2); for (i = 1; i <  20; ++i) fe_sq_avx2(t3, t3); fe_mul_avx2(t2, t3, t2);
    fe_sq_avx2(t2, t2); for (i = 1; i <  10; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t1, t2, t1);
    fe_sq_avx2(t2, t1); for (i = 1; i <  50; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t2, t2, t1);
    fe_sq_avx2(t3, t2); for (i = 1; i < 100; ++i) fe_sq_avx2(t3, t3); fe_mul_avx2(t2, t3, t2);
    fe_sq_avx2(t2, t2); for (i = 1; i <  50; ++i) fe_sq_avx2(t2, t2); fe_mul_avx2(t1, t2, t1);
    fe_sq_avx2(t1, t1); for (i = 1; i <   5; ++i) fe_sq_avx2(t1, t1); fe_mul_avx2( r, t1, t0);
}
#endif

/* Find the inverse of a modulo 2^255 - 1 and put result in r.
 * (r * a) mod (2^255 - 1) = 1
 * Implementation is constant time.
 *
 * r  A field element.
 * a  A field element.
 */
static void fe_invert_x64(fe r, const fe a)
{
    fe  t0, t1, t2, t3;
    int i;

    /* a ^ (2^255 - 21) */
    fe_sq_x64(t0,  a); for (i = 1; i <   1; ++i) fe_sq_x64(t0, t0);
    fe_sq_x64(t1, t0); for (i = 1; i <   2; ++i) fe_sq_x64(t1, t1); fe_mul_x64(t1,  a, t1);
    fe_mul_x64(t0, t0, t1);
    fe_sq_x64(t2, t0); for (i = 1; i <   1; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t1, t1, t2);
    fe_sq_x64(t2, t1); for (i = 1; i <   5; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t1, t2, t1);
    fe_sq_x64(t2, t1); for (i = 1; i <  10; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t2, t2, t1);
    fe_sq_x64(t3, t2); for (i = 1; i <  20; ++i) fe_sq_x64(t3, t3); fe_mul_x64(t2, t3, t2);
    fe_sq_x64(t2, t2); for (i = 1; i <  10; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t1, t2, t1);
    fe_sq_x64(t2, t1); for (i = 1; i <  50; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t2, t2, t1);
    fe_sq_x64(t3, t2); for (i = 1; i < 100; ++i) fe_sq_x64(t3, t3); fe_mul_x64(t2, t3, t2);
    fe_sq_x64(t2, t2); for (i = 1; i <  50; ++i) fe_sq_x64(t2, t2); fe_mul_x64(t1, t2, t1);
    fe_sq_x64(t1, t1); for (i = 1; i <   5; ++i) fe_sq_x64(t1, t1); fe_mul_x64( r, t1, t0);
}

/* Scalar multiply the field element a by n using Montgomery Ladder and places
 * result in r.
 *
 * r  A field element as an array of bytes.
 * n  The scalar as an array of bytes.
 * a  A field element as an array of bytes.
 */
int curve25519(byte* r, byte* n, byte* a)
{
    fe           x1, x2, z2, x3, z3;
    fe           t0, t1;
    int          i, j;
    unsigned int swap = 0;
    unsigned int b, prev_b = 0;

    fe_frombytes(x1, a);
    fe_1(x2);
    fe_0(z2);
    fe_copy(x3, x1);
    fe_1(z3);

    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        j = 6;
        for (i = 31; i >= 0; i--) {
            while (j >= 0) {
                b = n[i] >> j;
                b &= 1;
                swap = b ^ prev_b;
                prev_b = b;
                fe_cswap_int(x2, x3, swap);
                fe_cswap_int(z2, z3, swap);

                fe_sub_int(t0, x3, z3);
                fe_sub_int(t1, x2, z2);
                fe_add_int(x2, x2, z2);
                fe_add_int(z2, x3, z3);
                fe_mul_avx2(z3, t0, x2);
                fe_mul_avx2(z2, z2, t1);
                fe_sq_avx2(t0, t1);
                fe_sq_avx2(t1, x2);
                fe_add_int(x3, z3, z2);
                fe_sub_int(z2, z3, z2);
                fe_mul_avx2(x2, t1, t0);
                fe_sub_int(t1, t1, t0);
                fe_sq_avx2(z2, z2);
                fe_mul121666_int(z3, t1);
                fe_sq_avx2(x3, x3);
                fe_add_int(t0, t0, z3);
                fe_mul_avx2(z3, x1, z2);
                fe_mul_avx2(z2, t1, t0);
                j--;
            }
            j = 7;
        }

        fe_invert_avx2(z2, z2);
        fe_mul_avx2(x2, x2, z2);
        fe_tobytes(r, x2);
    }
    else {
        j = 6;
        for (i = 31; i >= 0; i--) {
            while (j >= 0) {
                b = n[i] >> j;
                b &= 1;
                swap = b ^ prev_b;
                prev_b = b;
                fe_cswap_int(x2, x3, swap);
                fe_cswap_int(z2, z3, swap);

                fe_sub_int(t0, x3, z3);
                fe_sub_int(t1, x2, z2);
                fe_add_int(x2, x2, z2);
                fe_add_int(z2, x3, z3);
                fe_mul_x64(z3, t0, x2);
                fe_mul_x64(z2, z2, t1);
                fe_sq_x64(t0, t1);
                fe_sq_x64(t1, x2);
                fe_add_int(x3, z3, z2);
                fe_sub_int(z2, z3, z2);
                fe_mul_x64(x2, t1, t0);
                fe_sub_int(t1, t1, t0);
                fe_sq_x64(z2, z2);
                fe_mul121666_int(z3, t1);
                fe_sq_x64(x3, x3);
                fe_add_int(t0, t0, z3);
                fe_mul_x64(z3, x1, z2);
                fe_mul_x64(z2, t1, t0);
                j--;
            }
            j = 7;
        }

        fe_invert_x64(z2, z2);
        fe_mul_x64(x2, x2, z2);
        fe_tobytes(r, x2);
    }

    return 0;
}

/* The field element value 0 as an array of bytes. */
static const unsigned char zero[32] = {0};

/* Constant time check as to whether a is not 0.
 *
 * a  A field element.
 */
int fe_isnonzero(const fe a)
{
    unsigned char s[32];
    fe_tobytes(s, a);
    return ConstantCompare(s, zero, 32);
}

/* Checks whether a is negative.
 *
 * a  A field element.
 */
int fe_isnegative(const fe a)
{
    unsigned char s[32];
    fe_tobytes(s, a);
    return s[0] & 1;
}

/* Negates field element a and stores the result in r.
 *
 * r  A field element.
 * a  A field element.
 */
void fe_neg(fe r, const fe a)
{
    __asm__ __volatile__ (
        "movq	$-19, %%rax\n\t"
        "movq	$-1, %%rdx\n\t"
        "movq	$-1, %%r8\n\t"
        "movq	$0x7fffffffffffffff, %%r9\n\t"
        "subq	0(%[a]), %%rax\n\t"
        "sbbq	8(%[a]), %%rdx\n\t"
        "sbbq	16(%[a]), %%r8\n\t"
        "sbbq	24(%[a]), %%r9\n\t"
        "movq	%%rax, 0(%[r])\n\t"
        "movq	%%rdx, 8(%[r])\n\t"
        "movq	%%r8, 16(%[r])\n\t"
        "movq	%%r9, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rdx", "r8", "r9"
    );
}

/* Constant time, conditional move of b into a.
 * a is not changed if the condition is 0.
 *
 * a  A field element.
 * b  A field element.
 * c  If 1 then copy and if 0 then don't copy.
 */
void fe_cmov(fe a, const fe b, int c)
{
    __asm__ __volatile__ (
        "cmpl   $1, %[c]\n\t"
        "movq	 0(%[a]), %%rcx\n\t"
        "movq	 8(%[a]), %%rdx\n\t"
        "movq	16(%[a]), %%r8\n\t"
        "movq	24(%[a]), %%r9\n\t"
        "cmoveq	 0(%[b]), %%rcx\n\t"
        "cmoveq	 8(%[b]), %%rdx\n\t"
        "cmoveq	16(%[b]), %%r8\n\t"
        "cmoveq	24(%[b]), %%r9\n\t"
        "movq	%%rcx,  0(%[a])\n\t"
        "movq	%%rdx,  8(%[a])\n\t"
        "movq	%%r8, 16(%[a])\n\t"
        "movq	%%r9, 24(%[a])\n\t"
        :
        : [a] "r" (a), [b] "r" (b), [c] "r" (c)
        : "memory", "rax", "rcx", "rdx", "r8", "r9"
    );
}

void fe_pow22523(fe r, const fe a)
{
    fe t0, t1, t2;
    int i;

    /* a ^ (2^255 - 23) */
    fe_sq(t0,  a); for (i = 1; i <   1; ++i) fe_sq(t0, t0);
    fe_sq(t1, t0); for (i = 1; i <   2; ++i) fe_sq(t1, t1); fe_mul(t1,  a, t1);
    fe_mul(t0, t0, t1);
    fe_sq(t0, t0); for (i = 1; i <   1; ++i) fe_sq(t0, t0); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <   5; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <  10; ++i) fe_sq(t1, t1); fe_mul(t1, t1, t0);
    fe_sq(t2, t1); for (i = 1; i <  20; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <  10; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t1, t0); for (i = 1; i <  50; ++i) fe_sq(t1, t1); fe_mul(t1, t1, t0);
    fe_sq(t2, t1); for (i = 1; i < 100; ++i) fe_sq(t2, t2); fe_mul(t1, t2, t1);
    fe_sq(t1, t1); for (i = 1; i <  50; ++i) fe_sq(t1, t1); fe_mul(t0, t1, t0);
    fe_sq(t0, t0); for (i = 1; i <   2; ++i) fe_sq(t0, t0); fe_mul( r, t0, a);

    return;
}

/* Double the square of a and put result in r. (r = 2 * a * a)
 *
 * r  A field element.
 * a  A field element.
 * b  A field element.
 */
void fe_sq2(fe r, const fe a)
{
    (*fe_sq2_p)(r, a);
}

#ifdef HAVE_INTEL_AVX2
static INLINE void fe_sq2_avx2(fe r, const fe a)
{
    __asm__ __volatile__ (
        "# A[0] * A[1]\n\t"
        "movq   0(%[a]), %%rdx\n\t"
        "mulxq  8(%[a]), %%r9, %%r10\n\t"
        "# A[0] * A[3]\n\t"
        "mulxq  24(%[a]), %%r11, %%r12\n\t"
        "# A[2] * A[1]\n\t"
        "movq   16(%[a]), %%rdx\n\t"
        "mulxq  8(%[a]), %%rcx, %%rbx\n\t"
        "xorq   %%r15, %%r15\n\t"
        "adoxq  %%rcx, %%r11\n\t"
        "# A[2] * A[3]\n\t"
        "mulxq  24(%[a]), %%r13, %%r14\n\t"
        "adoxq  %%rbx, %%r12\n\t"
        "# A[2] * A[0]\n\t"
        "mulxq  0(%[a]), %%rcx, %%rbx\n\t"
        "adoxq  %%r15, %%r13\n\t"
        "adcxq  %%rcx, %%r10\n\t"
        "adoxq  %%r15, %%r14\n\t"
        "# A[1] * A[3]\n\t"
        "movq   8(%[a]), %%rdx\n\t"
        "mulxq  24(%[a]), %%rax, %%r8\n\t"
        "adcxq  %%rbx, %%r11\n\t"
        "adcxq  %%rax, %%r12\n\t"
        "adcxq  %%r8, %%r13\n\t"
        "adcxq  %%r15, %%r14\n\t"
        "# Double with Carry Flag\n\t"
        "xorq   %%r15, %%r15\n\t"
        "# A[0] * A[0]\n\t"
        "movq   0(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%r8, %%rax\n\t"
        "adcxq  %%r9, %%r9\n\t"
        "# A[1] * A[1]\n\t"
        "movq   8(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rcx, %%rbx\n\t"
        "adcxq  %%r10, %%r10\n\t"
        "adoxq  %%rax, %%r9\n\t"
        "adcxq  %%r11, %%r11\n\t"
        "adoxq  %%rcx, %%r10\n\t"
        "# A[2] * A[2]\n\t"
        "movq   16(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rax, %%rcx\n\t"
        "adcxq  %%r12, %%r12\n\t"
        "adoxq  %%rbx, %%r11\n\t"
        "adcxq  %%r13, %%r13\n\t"
        "adoxq  %%rax, %%r12\n\t"
        "# A[3] * A[3]\n\t"
        "movq   24(%[a]), %%rdx\n\t"
        "mulxq  %%rdx, %%rax, %%rbx\n\t"
        "adcxq  %%r14, %%r14\n\t"
        "adoxq  %%rcx, %%r13\n\t"
        "adcxq  %%r15, %%r15\n\t"
        "adoxq  %%rax, %%r14\n\t"
        "adoxq  %%rbx, %%r15\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rbx\n\t"
        "xorq	%%rax, %%rax\n\t"
        "#  Move top half into t4-t7, remove top bit from t3 and double\n\t"
        "shldq	$3, %%r15, %%rax\n\t"
        "shldq	$2, %%r14, %%r15\n\t"
        "shldq	$2, %%r13, %%r14\n\t"
        "shldq	$2, %%r12, %%r13\n\t"
        "shldq	$2, %%r11, %%r12\n\t"
        "shldq	$1, %%r10, %%r11\n\t"
        "shldq	$1, %%r9, %%r10\n\t"
        "shldq	$1, %%r8, %%r9\n\t"
        "shlq	$1, %%r8\n\t"
        "andq	%%rbx, %%r11\n\t"
        "# Two out left, one in right\n\t"
        "andq	%%rbx, %%r15\n\t"
        "#  Multiply top bits by 19*19\n\t"
        "imulq	$361, %%rax, %%rcx\n\t"
        "xorq	%%rbx, %%rbx\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rdx\n\t"
        "adoxq	%%rcx, %%r8\n\t"
        "mulxq	%%r12, %%rax, %%r12\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r8\n\t"
        "adoxq	%%r12, %%r9\n\t"
        "mulxq	%%r13, %%rax, %%r13\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r9\n\t"
        "adoxq	%%r13, %%r10\n\t"
        "mulxq	%%r14, %%rax, %%r14\n\t"
        "movq	$19, %%rdx\n\t"
        "adcxq	%%rax, %%r10\n\t"
        "adoxq	%%r14, %%r11\n\t"
        "mulxq	%%r15, %%r15, %%rdx\n\t"
        "adcxq	%%r15, %%r11\n\t"
        "adoxq	%%rbx, %%rdx\n\t"
        "adcxq	%%rbx, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r11, %%rdx\n\t"
        "movq	$0x7fffffffffffffff, %%rbx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rbx, %%r11\n\t"
        "addq	%%rax, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "adcq	$0, %%r11\n\t"
        "movq	%%r8, 0(%[r])\n\t"
        "movq	%%r9, 8(%[r])\n\t"
        "movq	%%r10, 16(%[r])\n\t"
        "movq	%%r11, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rdx", "rcx", "rbx", "r8", "r9", "r10", "r11",
          "r12", "r13", "r14", "r15"
    );
}
#endif /* HAVE_INTEL_AVX2 */

static INLINE void fe_sq2_x64(fe r, const fe a)
{
    __asm__ __volatile__ (
        "#  A[0] * A[1]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   8(%[a])\n\t"
        "movq   %%rax, %%r8\n\t"
        "movq   %%rdx, %%r9\n\t"
        "#  A[0] * A[2]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "xorq   %%r10, %%r10\n\t"
        "addq   %%rax, %%r9\n\t"
        "adcq   %%rdx, %%r10\n\t"
        "#  A[0] * A[3]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "xorq   %%r11, %%r11\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "#  A[1] * A[2]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   16(%[a])\n\t"
        "xorq   %%r12, %%r12\n\t"
        "addq   %%rax, %%r10\n\t"
        "adcq   %%rdx, %%r11\n\t"
        "adcq   $0, %%r12\n\t"
        "#  A[1] * A[3]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "addq   %%rax, %%r11\n\t"
        "adcq   %%rdx, %%r12\n\t"
        "#  A[2] * A[3]\n\t"
        "movq   16(%[a]), %%rax\n\t"
        "mulq   24(%[a])\n\t"
        "xorq   %%r13, %%r13\n\t"
        "addq   %%rax, %%r12\n\t"
        "adcq   %%rdx, %%r13\n\t"
        "# Double\n\t"
        "xorq   %%r14, %%r14\n\t"
        "addq   %%r8, %%r8\n\t"
        "adcq   %%r9, %%r9\n\t"
        "adcq   %%r10, %%r10\n\t"
        "adcq   %%r11, %%r11\n\t"
        "adcq   %%r12, %%r12\n\t"
        "adcq   %%r13, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "#  A[0] * A[0]\n\t"
        "movq   0(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "movq   %%rax, %%rcx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[1] * A[1]\n\t"
        "movq   8(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%r15, %%r8\n\t"
        "adcq   %%rax, %%r9\n\t"
        "adcq   $0, %%rdx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[2] * A[2]\n\t"
        "movq   16(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%r15, %%r10\n\t"
        "adcq   %%rax, %%r11\n\t"
        "adcq   $0, %%rdx\n\t"
        "movq   %%rdx, %%r15\n\t"
        "#  A[3] * A[3]\n\t"
        "movq   24(%[a]), %%rax\n\t"
        "mulq   %%rax\n\t"
        "addq   %%rax, %%r13\n\t"
        "adcq   %%rdx, %%r14\n\t"
        "addq   %%r15, %%r12\n\t"
        "adcq   $0, %%r13\n\t"
        "adcq   $0, %%r14\n\t"
        "# Reduce\n\t"
        "movq	$0x7fffffffffffffff, %%rbx\n\t"
        "xorq	%%rax, %%rax\n\t"
        "#  Move top half into t4-t7 and remove top bit from t3\n\t"
        "shldq	$3, %%r14, %%rax\n\t"
        "shldq	$2, %%r13, %%r14\n\t"
        "shldq	$2, %%r12, %%r13\n\t"
        "shldq	$2, %%r11, %%r12\n\t"
        "shldq	$2, %%r10, %%r11\n\t"
        "shldq	$1, %%r9, %%r10\n\t"
        "shldq	$1, %%r8, %%r9\n\t"
        "shldq	$1, %%rcx, %%r8\n\t"
        "shlq	$1, %%rcx\n\t"
        "andq %%rbx, %%r10\n\t"
        "# Two out left, one in right\n\t"
        "andq %%rbx, %%r14\n\t"
        "#  Multiply top bits by 19*19\n\t"
        "imulq	$361, %%rax, %%r15\n\t"
        "#  Multiply top half by 19\n\t"
        "movq	$19, %%rax\n\t"
        "mulq	%%r11\n\t"
        "xorq	%%r11, %%r11\n\t"
        "addq	%%rax, %%rcx\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r11\n\t"
        "mulq	%%r12\n\t"
        "xorq	%%r12, %%r12\n\t"
        "addq	%%rax, %%r8\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r12\n\t"
        "mulq	%%r13\n\t"
        "xorq	%%r13, %%r13\n\t"
        "addq	%%rax, %%r9\n\t"
        "movq	$19, %%rax\n\t"
        "adcq	%%rdx, %%r13\n\t"
        "mulq	%%r14\n\t"
        "#  Add remaining products back in\n\t"
        "addq	%%r15, %%rcx\n\t"
        "adcq	%%r11, %%r8\n\t"
        "adcq	%%r12, %%r9\n\t"
        "adcq	%%r13, %%r10\n\t"
        "addq	%%rax, %%r10\n\t"
        "adcq	$0, %%rdx\n\t"
        "#  Overflow\n\t"
        "shldq	$1, %%r10, %%rdx\n\t"
        "imulq	$19, %%rdx, %%rax\n\t"
        "andq	%%rbx, %%r10\n\t"
        "addq	%%rax, %%rcx\n\t"
        "adcq	$0, %%r8\n\t"
        "adcq	$0, %%r9\n\t"
        "adcq	$0, %%r10\n\t"
        "movq	%%rcx, 0(%[r])\n\t"
        "movq	%%r8, 8(%[r])\n\t"
        "movq	%%r9, 16(%[r])\n\t"
        "movq	%%r10, 24(%[r])\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "rax", "rbx", "rdx", "rcx", "r8", "r9", "r10", "r11", "r12",
          "r13", "r14", "r15"
    );
}

/* Load 3 little endian bytes into a 64-bit word.
 *
 * in  An array of bytes.
 * returns a 64-bit word.
 */
uint64_t load_3(const unsigned char *in)
{
    uint64_t result;

    result = ((((uint64_t)in[0])      ) |
              (((uint64_t)in[1]) <<  8) |
              (((uint64_t)in[2]) << 16));

    return result;
}

/* Load 4 little endian bytes into a 64-bit word.
 *
 * in  An array of bytes.
 * returns a 64-bit word.
 */
uint64_t load_4(const unsigned char *in)
{
    uint64_t result;

    result = ((((uint64_t)in[0])      ) |
              (((uint64_t)in[1]) <<  8) |
              (((uint64_t)in[2]) << 16) |
              (((uint64_t)in[3]) << 24));

    return result;
}

void fe_ge_to_p2(fe rx, fe ry, fe rz, const fe px, const fe py, const fe pz,
                 const fe pt)
{
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_mul_avx2(rx, px, pt);
        fe_mul_avx2(ry, py, pz);
        fe_mul_avx2(rz, pz, pt);
    }
    else {
        fe_mul_x64(rx, px, pt);
        fe_mul_x64(ry, py, pz);
        fe_mul_x64(rz, pz, pt);
    }
}

void fe_ge_to_p3(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
               const fe pz, const fe pt)
{
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_mul_avx2(rx, px, pt);
        fe_mul_avx2(ry, py, pz);
        fe_mul_avx2(rz, pz, pt);
        fe_mul_avx2(rt, px, py);
    }
    else {
        fe_mul_x64(rx, px, pt);
        fe_mul_x64(ry, py, pz);
        fe_mul_x64(rz, pz, pt);
        fe_mul_x64(rt, px, py);
    }
}

void fe_ge_dbl(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
               const fe pz)
{
    fe t0;
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_sq_avx2(rx,px);
        fe_sq_avx2(rz,py);
        fe_sq2_avx2(rt,pz);
        fe_add_int(ry,px,py);
        fe_sq_avx2(t0,ry);
        fe_add_int(ry,rz,rx);
        fe_sub_int(rz,rz,rx);
        fe_sub_int(rx,t0,ry);
        fe_sub_int(rt,rt,rz);
    }
    else {
        fe_sq_x64(rx,px);
        fe_sq_x64(rz,py);
        fe_sq2_x64(rt,pz);
        fe_add_int(ry,px,py);
        fe_sq_x64(t0,ry);
        fe_add_int(ry,rz,rx);
        fe_sub_int(rz,rz,rx);
        fe_sub_int(rx,t0,ry);
        fe_sub_int(rt,rt,rz);
    }
}

void fe_ge_madd(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
                const fe pz, const fe pt, const fe qxy2d, const fe qyplusx,
                const fe qyminusx)
{
    fe t0;
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_avx2(rz,rx,qyplusx);
        fe_mul_avx2(ry,ry,qyminusx);
        fe_mul_avx2(rt,qxy2d,pt);
        fe_add_int(t0,pz,pz);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_add_int(rz,t0,rt);
        fe_sub_int(rt,t0,rt);
    }
    else {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_x64(rz,rx,qyplusx);
        fe_mul_x64(ry,ry,qyminusx);
        fe_mul_x64(rt,qxy2d,pt);
        fe_add_int(t0,pz,pz);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_add_int(rz,t0,rt);
        fe_sub_int(rt,t0,rt);
    }
}

void fe_ge_msub(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
                const fe pz, const fe pt, const fe qxy2d, const fe qyplusx,
                const fe qyminusx)
{
    fe t0;
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_avx2(rz,rx,qyminusx);
        fe_mul_avx2(ry,ry,qyplusx);
        fe_mul_avx2(rt,qxy2d,pt);
        fe_add_int(t0,pz,pz);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_sub_int(rz,t0,rt);
        fe_add_int(rt,t0,rt);
    }
    else {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_x64(rz,rx,qyminusx);
        fe_mul_x64(ry,ry,qyplusx);
        fe_mul_x64(rt,qxy2d,pt);
        fe_add_int(t0,pz,pz);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_sub_int(rz,t0,rt);
        fe_add_int(rt,t0,rt);
    }
}

void fe_ge_add(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
               const fe pz, const fe pt, const fe qz, const fe qt2d,
               const fe qyplusx, const fe qyminusx)
{
    fe t0;
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_avx2(rz,rx,qyplusx);
        fe_mul_avx2(ry,ry,qyminusx);
        fe_mul_avx2(rt,qt2d,pt);
        fe_mul_avx2(rx,pz,qz);
        fe_add_int(t0,rx,rx);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_add_int(rz,t0,rt);
        fe_sub_int(rt,t0,rt);
    }
    else {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_x64(rz,rx,qyplusx);
        fe_mul_x64(ry,ry,qyminusx);
        fe_mul_x64(rt,qt2d,pt);
        fe_mul_x64(rx,pz,qz);
        fe_add_int(t0,rx,rx);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_add_int(rz,t0,rt);
        fe_sub_int(rt,t0,rt);
    }
}

void fe_ge_sub(fe rx, fe ry, fe rz, fe rt, const fe px, const fe py,
               const fe pz, const fe pt, const fe qz, const fe qt2d,
               const fe qyplusx, const fe qyminusx)
{
    fe t0;
    if (IS_INTEL_BMI2(intelFlags) && IS_INTEL_ADX(intelFlags)) {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_avx2(rz,rx,qyminusx);
        fe_mul_avx2(ry,ry,qyplusx);
        fe_mul_avx2(rt,qt2d,pt);
        fe_mul_avx2(rx,pz,qz);
        fe_add_int(t0,rx,rx);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_sub_int(rz,t0,rt);
        fe_add_int(rt,t0,rt);
    }
    else {
        fe_add_int(rx,py,px);
        fe_sub_int(ry,py,px);
        fe_mul_x64(rz,rx,qyminusx);
        fe_mul_x64(ry,ry,qyplusx);
        fe_mul_x64(rt,qt2d,pt);
        fe_mul_x64(rx,pz,qz);
        fe_add_int(t0,rx,rx);
        fe_sub_int(rx,rz,ry);
        fe_add_int(ry,rz,ry);
        fe_sub_int(rz,t0,rt);
        fe_add_int(rt,t0,rt);
    }
}

void fe_cmov_table(fe* r, fe* base, signed char b)
{
    __asm__ __volatile__ (
        "movsbq %[b], %%rax\n\t"
        "cdq\n\t"
        "xorb   %%dl, %%al\n\t"
        "subb   %%dl, %%al\n\t"
        "movb   %%al, %%r13b\n\t"

        "movq	 $1, %%rax\n\t"
        "xorq	 %%rbx, %%rbx\n\t"
        "xorq	 %%rcx, %%rcx\n\t"
        "xorq	 %%rdx, %%rdx\n\t"
        "movq	 $1, %%r8\n\t"
        "xorq	 %%r9 , %%r9\n\t"
        "xorq	 %%r10, %%r10\n\t"
        "xorq	 %%r11, %%r11\n\t"

        "cmpb   $1, %%r13b\n\t"
        "movq   (0*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (0*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (0*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (0*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (0*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (0*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (0*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (0*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $2, %%r13b\n\t"
        "movq   (1*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (1*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (1*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (1*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (1*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (1*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (1*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (1*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $3, %%r13b\n\t"
        "movq   (2*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (2*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (2*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (2*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (2*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (2*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (2*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (2*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $4, %%r13b\n\t"
        "movq   (3*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (3*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (3*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (3*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (3*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (3*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (3*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (3*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $5, %%r13b\n\t"
        "movq   (4*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (4*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (4*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (4*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (4*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (4*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (4*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (4*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $6, %%r13b\n\t"
        "movq   (5*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (5*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (5*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (5*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (5*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (5*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (5*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (5*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $7, %%r13b\n\t"
        "movq   (6*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (6*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (6*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (6*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (6*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (6*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (6*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (6*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"
        "cmpb   $8, %%r13b\n\t"
        "movq   (7*96)+0(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (7*96)+8(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (7*96)+16(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (7*96)+24(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "movq   (7*96)+32(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r8\n\t"
        "movq   (7*96)+40(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r9\n\t"
        "movq   (7*96)+48(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r10\n\t"
        "movq   (7*96)+56(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%r11\n\t"

        "cmpb   $0, %[b]\n\t"
        "movq   %%rax, %%r12\n\t"
        "cmovlq %%r8, %%rax\n\t"
        "cmovlq %%r12, %%r8\n\t"
        "movq   %%rbx, %%r12\n\t"
        "cmovlq %%r9, %%rbx\n\t"
        "cmovlq %%r12, %%r9\n\t"
        "movq   %%rcx, %%r12\n\t"
        "cmovlq %%r10, %%rcx\n\t"
        "cmovlq %%r12, %%r10\n\t"
        "movq   %%rdx, %%r12\n\t"
        "cmovlq %%r11, %%rdx\n\t"
        "cmovlq %%r12, %%r11\n\t"

        "movq   %%rax,  0(%[r])\n\t"
        "movq   %%rbx,  8(%[r])\n\t"
        "movq   %%rcx, 16(%[r])\n\t"
        "movq   %%rdx, 24(%[r])\n\t"
        "movq   %%r8 , 32(%[r])\n\t"
        "movq   %%r9 , 40(%[r])\n\t"
        "movq   %%r10, 48(%[r])\n\t"
        "movq   %%r11, 56(%[r])\n\t"

        "xorq	 %%rax, %%rax\n\t"
        "xorq	 %%rbx, %%rbx\n\t"
        "xorq	 %%rcx, %%rcx\n\t"
        "xorq	 %%rdx, %%rdx\n\t"

        "cmpb   $1, %%r13b\n\t"
        "movq   (0*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (0*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (0*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (0*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $2, %%r13b\n\t"
        "movq   (1*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (1*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (1*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (1*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $3, %%r13b\n\t"
        "movq   (2*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (2*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (2*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (2*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $4, %%r13b\n\t"
        "movq   (3*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (3*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (3*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (3*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $5, %%r13b\n\t"
        "movq   (4*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (4*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (4*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (4*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $6, %%r13b\n\t"
        "movq   (5*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (5*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (5*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (5*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $7, %%r13b\n\t"
        "movq   (6*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (6*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (6*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (6*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"
        "cmpb   $8, %%r13b\n\t"
        "movq   (7*96)+64(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rax\n\t"
        "movq   (7*96)+72(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rbx\n\t"
        "movq   (7*96)+80(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rcx\n\t"
        "movq   (7*96)+88(%[base]), %%r12\n\t"
        "cmoveq %%r12, %%rdx\n\t"

        "movq	$-19, %%r8\n\t"
        "movq	$-1, %%r9\n\t"
        "movq	$-1, %%r10\n\t"
        "movq	$0x7fffffffffffffff, %%r11\n\t"
        "subq	%%rax, %%r8\n\t"
        "sbbq	%%rbx, %%r9\n\t"
        "sbbq	%%rcx, %%r10\n\t"
        "sbbq	%%rdx, %%r11\n\t"
        "cmpb   $0, %[b]\n\t"
        "cmovlq %%r8, %%rax\n\t"
        "cmovlq %%r9, %%rbx\n\t"
        "cmovlq %%r10, %%rcx\n\t"
        "cmovlq %%r11, %%rdx\n\t"

        "movq   %%rax, 64(%[r])\n\t"
        "movq   %%rbx, 72(%[r])\n\t"
        "movq   %%rcx, 80(%[r])\n\t"
        "movq   %%rdx, 88(%[r])\n\t"
        :
        : [r] "r" (r), [base] "r" (base), [b] "r" (b)
        : "rax", "rbx", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "memory",
          "r13"
    );
}

