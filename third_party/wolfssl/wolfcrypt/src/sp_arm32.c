/* sp.c
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
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

/* Implementation by Sean Parkinson. */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/cpuid.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH) || \
                                    defined(WOLFSSL_HAVE_SP_ECC)

#ifdef RSA_LOW_MEM
#define SP_RSA_PRIVATE_EXP_D

#ifndef WOLFSSL_SP_SMALL
#define WOLFSSL_SP_SMALL
#endif
#endif

#include <wolfssl/wolfcrypt/sp.h>

#ifdef WOLFSSL_SP_ARM32_ASM
#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH)
#ifndef WOLFSSL_SP_NO_2048
/* Read big endian unsigned byte aray into r.
 *
 * r  A single precision integer.
 * a  Byte array.
 * n  Number of bytes in array to read.
 */
static void sp_2048_from_bin(sp_digit* r, int max, const byte* a, int n)
{
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = n-1; i >= 0; i--) {
        r[j] |= ((sp_digit)a[i]) << s;
        if (s >= 24) {
            r[j] &= 0xffffffff;
            s = 32 - s;
            if (j + 1 >= max)
                break;
            r[++j] = a[i] >> s;
            s = 8 - s;
        }
        else
            s += 8;
    }

    for (j++; j < max; j++)
        r[j] = 0;
}

/* Convert an mp_int to an array of sp_digit.
 *
 * r  A single precision integer.
 * a  A multi-precision integer.
 */
static void sp_2048_from_mp(sp_digit* r, int max, mp_int* a)
{
#if DIGIT_BIT == 32
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 32
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0xffffffff;
        s = 32 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 32 <= DIGIT_BIT) {
            s += 32;
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            if (s < DIGIT_BIT)
                r[++j] = a->dp[i] >> s;
            else
                r[++j] = 0;
        }
        s = DIGIT_BIT - s;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#else
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= ((sp_digit)a->dp[i]) << s;
        if (s + DIGIT_BIT >= 32) {
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            s = 32 - s;
            if (s == DIGIT_BIT) {
                r[++j] = 0;
                s = 0;
            }
            else {
                r[++j] = a->dp[i] >> s;
                s = DIGIT_BIT - s;
            }
        }
        else
            s += DIGIT_BIT;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#endif
}

/* Write r as big endian to byte aray.
 * Fixed length number of bytes written: 256
 *
 * r  A single precision integer.
 * a  Byte array.
 */
static void sp_2048_to_bin(sp_digit* r, byte* a)
{
    int i, j, s = 0, b;

    j = 2048 / 8 - 1;
    a[j] = 0;
    for (i=0; i<64 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 32) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 32);
        if (j >= 0)
            a[j] = 0;
        if (s != 0)
            j++;
    }
}

#ifndef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_8(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "#  A[0] * B[0]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r3, r4, r8, r9\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[tmp]]\n\t"
        "#  A[0] * B[1]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[0]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #4]\n\t"
        "#  A[0] * B[2]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[1]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[0]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #8]\n\t"
        "#  A[0] * B[3]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[2]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[1]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[0]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #12]\n\t"
        "#  A[0] * B[4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[3]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[2]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[1]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[0]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #16]\n\t"
        "#  A[0] * B[5]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[4]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[3]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[2]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[1]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[0]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #20]\n\t"
        "#  A[0] * B[6]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[5]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[4]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[3]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[2]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[1]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[0]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #24]\n\t"
        "#  A[0] * B[7]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[6]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[5]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[4]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[3]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[2]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[1]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[0]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #28]\n\t"
        "#  A[1] * B[7]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[2] * B[6]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[5]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[4]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[3]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[2]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[1]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "#  A[2] * B[7]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[3] * B[6]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[5]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[4]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[3]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[2]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "#  A[3] * B[7]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[4] * B[6]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[5]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[4]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[3]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "#  A[4] * B[7]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[5] * B[6]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[5]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[4]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "#  A[5] * B[7]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[6] * B[6]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[5]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "#  A[6] * B[7]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[7] * B[6]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "#  A[7] * B[7]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r5, [%[r], #56]\n\t"
        "str	r3, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [tmp] "r" (tmp)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_8(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "#  A[0] * A[0]\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "umull	r8, r3, r10, r10\n\t"
        "mov	r4, #0\n\t"
        "str	r8, [%[tmp]]\n\t"
        "#  A[0] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #4]\n\t"
        "#  A[0] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[1] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[tmp], #8]\n\t"
        "#  A[0] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[1] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[tmp], #12]\n\t"
        "#  A[0] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[1] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[2] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #16]\n\t"
        "#  A[0] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[tmp], #20]\n\t"
        "#  A[0] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[tmp], #24]\n\t"
        "#  A[0] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r2, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r3, r3, r5\n\t"
        "adcs	r4, r4, r6\n\t"
        "adc	r2, r2, r7\n\t"
        "str	r3, [%[tmp], #28]\n\t"
        "#  A[1] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[2] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "#  A[2] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[3] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[r], #36]\n\t"
        "#  A[3] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[4] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[5] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #40]\n\t"
        "#  A[4] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[5] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[r], #44]\n\t"
        "#  A[5] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[6] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[r], #48]\n\t"
        "#  A[6] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #52]\n\t"
        "#  A[7] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r2, r2, r9\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r2, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [tmp] "r" (tmp)
        : "memory", "r2", "r3", "r4", "r8", "r9", "r10", "r8", "r5", "r6", "r7", "r14"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_2048_sub_in_place_16(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_16(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_8(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<8; i++)
        r[i] = a[i] & m;
#else
    r[0] = a[0] & m;
    r[1] = a[1] & m;
    r[2] = a[2] & m;
    r[3] = a[3] & m;
    r[4] = a[4] & m;
    r[5] = a[5] & m;
    r[6] = a[6] & m;
    r[7] = a[7] & m;
#endif
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_16(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[16];
    sp_digit a1[8];
    sp_digit b1[8];
    sp_digit z2[16];
    sp_digit u, ca, cb;

    ca = sp_2048_add_8(a1, a, &a[8]);
    cb = sp_2048_add_8(b1, b, &b[8]);
    u  = ca & cb;
    sp_2048_mul_8(z1, a1, b1);
    sp_2048_mul_8(z2, &a[8], &b[8]);
    sp_2048_mul_8(z0, a, b);
    sp_2048_mask_8(r + 16, a1, 0 - cb);
    sp_2048_mask_8(b1, b1, 0 - ca);
    u += sp_2048_add_8(r + 16, r + 16, b1);
    u += sp_2048_sub_in_place_16(z1, z2);
    u += sp_2048_sub_in_place_16(z1, z0);
    u += sp_2048_add_16(r + 8, r + 8, z1);
    r[24] = u;
    XMEMSET(r + 24 + 1, 0, sizeof(sp_digit) * (8 - 1));
    sp_2048_add_16(r + 16, r + 16, z2);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_16(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z2[16];
    sp_digit z1[16];
    sp_digit a1[8];
    sp_digit u;

    u = sp_2048_add_8(a1, a, &a[8]);
    sp_2048_sqr_8(z1, a1);
    sp_2048_sqr_8(z2, &a[8]);
    sp_2048_sqr_8(z0, a);
    sp_2048_mask_8(r + 16, a1, 0 - u);
    u += sp_2048_add_8(r + 16, r + 16, r + 16);
    u += sp_2048_sub_in_place_16(z1, z2);
    u += sp_2048_sub_in_place_16(z1, z0);
    u += sp_2048_add_16(r + 8, r + 8, z1);
    r[24] = u;
    XMEMSET(r + 24 + 1, 0, sizeof(sp_digit) * (8 - 1));
    sp_2048_add_16(r + 16, r + 16, z2);
}

/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_2048_sub_in_place_32(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "ldr	r2, [%[a], #64]\n\t"
        "ldr	r3, [%[a], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r5, [%[a], #76]\n\t"
        "ldr	r6, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "ldr	r8, [%[b], #72]\n\t"
        "ldr	r9, [%[b], #76]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #64]\n\t"
        "str	r3, [%[a], #68]\n\t"
        "str	r4, [%[a], #72]\n\t"
        "str	r5, [%[a], #76]\n\t"
        "ldr	r2, [%[a], #80]\n\t"
        "ldr	r3, [%[a], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r5, [%[a], #92]\n\t"
        "ldr	r6, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "ldr	r8, [%[b], #88]\n\t"
        "ldr	r9, [%[b], #92]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #80]\n\t"
        "str	r3, [%[a], #84]\n\t"
        "str	r4, [%[a], #88]\n\t"
        "str	r5, [%[a], #92]\n\t"
        "ldr	r2, [%[a], #96]\n\t"
        "ldr	r3, [%[a], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r5, [%[a], #108]\n\t"
        "ldr	r6, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "ldr	r8, [%[b], #104]\n\t"
        "ldr	r9, [%[b], #108]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #96]\n\t"
        "str	r3, [%[a], #100]\n\t"
        "str	r4, [%[a], #104]\n\t"
        "str	r5, [%[a], #108]\n\t"
        "ldr	r2, [%[a], #112]\n\t"
        "ldr	r3, [%[a], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r5, [%[a], #124]\n\t"
        "ldr	r6, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "ldr	r8, [%[b], #120]\n\t"
        "ldr	r9, [%[b], #124]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #112]\n\t"
        "str	r3, [%[a], #116]\n\t"
        "str	r4, [%[a], #120]\n\t"
        "str	r5, [%[a], #124]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_32(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r5, [%[a], #68]\n\t"
        "ldr	r6, [%[a], #72]\n\t"
        "ldr	r7, [%[a], #76]\n\t"
        "ldr	r8, [%[b], #64]\n\t"
        "ldr	r9, [%[b], #68]\n\t"
        "ldr	r10, [%[b], #72]\n\t"
        "ldr	r14, [%[b], #76]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r5, [%[r], #68]\n\t"
        "str	r6, [%[r], #72]\n\t"
        "str	r7, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r5, [%[a], #84]\n\t"
        "ldr	r6, [%[a], #88]\n\t"
        "ldr	r7, [%[a], #92]\n\t"
        "ldr	r8, [%[b], #80]\n\t"
        "ldr	r9, [%[b], #84]\n\t"
        "ldr	r10, [%[b], #88]\n\t"
        "ldr	r14, [%[b], #92]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r5, [%[r], #84]\n\t"
        "str	r6, [%[r], #88]\n\t"
        "str	r7, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r5, [%[a], #100]\n\t"
        "ldr	r6, [%[a], #104]\n\t"
        "ldr	r7, [%[a], #108]\n\t"
        "ldr	r8, [%[b], #96]\n\t"
        "ldr	r9, [%[b], #100]\n\t"
        "ldr	r10, [%[b], #104]\n\t"
        "ldr	r14, [%[b], #108]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r5, [%[r], #100]\n\t"
        "str	r6, [%[r], #104]\n\t"
        "str	r7, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r5, [%[a], #116]\n\t"
        "ldr	r6, [%[a], #120]\n\t"
        "ldr	r7, [%[a], #124]\n\t"
        "ldr	r8, [%[b], #112]\n\t"
        "ldr	r9, [%[b], #116]\n\t"
        "ldr	r10, [%[b], #120]\n\t"
        "ldr	r14, [%[b], #124]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r5, [%[r], #116]\n\t"
        "str	r6, [%[r], #120]\n\t"
        "str	r7, [%[r], #124]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_16(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<16; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
#endif
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_32(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[32];
    sp_digit a1[16];
    sp_digit b1[16];
    sp_digit z2[32];
    sp_digit u, ca, cb;

    ca = sp_2048_add_16(a1, a, &a[16]);
    cb = sp_2048_add_16(b1, b, &b[16]);
    u  = ca & cb;
    sp_2048_mul_16(z1, a1, b1);
    sp_2048_mul_16(z2, &a[16], &b[16]);
    sp_2048_mul_16(z0, a, b);
    sp_2048_mask_16(r + 32, a1, 0 - cb);
    sp_2048_mask_16(b1, b1, 0 - ca);
    u += sp_2048_add_16(r + 32, r + 32, b1);
    u += sp_2048_sub_in_place_32(z1, z2);
    u += sp_2048_sub_in_place_32(z1, z0);
    u += sp_2048_add_32(r + 16, r + 16, z1);
    r[48] = u;
    XMEMSET(r + 48 + 1, 0, sizeof(sp_digit) * (16 - 1));
    sp_2048_add_32(r + 32, r + 32, z2);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_32(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z2[32];
    sp_digit z1[32];
    sp_digit a1[16];
    sp_digit u;

    u = sp_2048_add_16(a1, a, &a[16]);
    sp_2048_sqr_16(z1, a1);
    sp_2048_sqr_16(z2, &a[16]);
    sp_2048_sqr_16(z0, a);
    sp_2048_mask_16(r + 32, a1, 0 - u);
    u += sp_2048_add_16(r + 32, r + 32, r + 32);
    u += sp_2048_sub_in_place_32(z1, z2);
    u += sp_2048_sub_in_place_32(z1, z0);
    u += sp_2048_add_32(r + 16, r + 16, z1);
    r[48] = u;
    XMEMSET(r + 48 + 1, 0, sizeof(sp_digit) * (16 - 1));
    sp_2048_add_32(r + 32, r + 32, z2);
}

/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_2048_sub_in_place_64(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "ldr	r2, [%[a], #64]\n\t"
        "ldr	r3, [%[a], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r5, [%[a], #76]\n\t"
        "ldr	r6, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "ldr	r8, [%[b], #72]\n\t"
        "ldr	r9, [%[b], #76]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #64]\n\t"
        "str	r3, [%[a], #68]\n\t"
        "str	r4, [%[a], #72]\n\t"
        "str	r5, [%[a], #76]\n\t"
        "ldr	r2, [%[a], #80]\n\t"
        "ldr	r3, [%[a], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r5, [%[a], #92]\n\t"
        "ldr	r6, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "ldr	r8, [%[b], #88]\n\t"
        "ldr	r9, [%[b], #92]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #80]\n\t"
        "str	r3, [%[a], #84]\n\t"
        "str	r4, [%[a], #88]\n\t"
        "str	r5, [%[a], #92]\n\t"
        "ldr	r2, [%[a], #96]\n\t"
        "ldr	r3, [%[a], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r5, [%[a], #108]\n\t"
        "ldr	r6, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "ldr	r8, [%[b], #104]\n\t"
        "ldr	r9, [%[b], #108]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #96]\n\t"
        "str	r3, [%[a], #100]\n\t"
        "str	r4, [%[a], #104]\n\t"
        "str	r5, [%[a], #108]\n\t"
        "ldr	r2, [%[a], #112]\n\t"
        "ldr	r3, [%[a], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r5, [%[a], #124]\n\t"
        "ldr	r6, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "ldr	r8, [%[b], #120]\n\t"
        "ldr	r9, [%[b], #124]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #112]\n\t"
        "str	r3, [%[a], #116]\n\t"
        "str	r4, [%[a], #120]\n\t"
        "str	r5, [%[a], #124]\n\t"
        "ldr	r2, [%[a], #128]\n\t"
        "ldr	r3, [%[a], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r5, [%[a], #140]\n\t"
        "ldr	r6, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "ldr	r8, [%[b], #136]\n\t"
        "ldr	r9, [%[b], #140]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #128]\n\t"
        "str	r3, [%[a], #132]\n\t"
        "str	r4, [%[a], #136]\n\t"
        "str	r5, [%[a], #140]\n\t"
        "ldr	r2, [%[a], #144]\n\t"
        "ldr	r3, [%[a], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r5, [%[a], #156]\n\t"
        "ldr	r6, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "ldr	r8, [%[b], #152]\n\t"
        "ldr	r9, [%[b], #156]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #144]\n\t"
        "str	r3, [%[a], #148]\n\t"
        "str	r4, [%[a], #152]\n\t"
        "str	r5, [%[a], #156]\n\t"
        "ldr	r2, [%[a], #160]\n\t"
        "ldr	r3, [%[a], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r5, [%[a], #172]\n\t"
        "ldr	r6, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "ldr	r8, [%[b], #168]\n\t"
        "ldr	r9, [%[b], #172]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #160]\n\t"
        "str	r3, [%[a], #164]\n\t"
        "str	r4, [%[a], #168]\n\t"
        "str	r5, [%[a], #172]\n\t"
        "ldr	r2, [%[a], #176]\n\t"
        "ldr	r3, [%[a], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r5, [%[a], #188]\n\t"
        "ldr	r6, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "ldr	r8, [%[b], #184]\n\t"
        "ldr	r9, [%[b], #188]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #176]\n\t"
        "str	r3, [%[a], #180]\n\t"
        "str	r4, [%[a], #184]\n\t"
        "str	r5, [%[a], #188]\n\t"
        "ldr	r2, [%[a], #192]\n\t"
        "ldr	r3, [%[a], #196]\n\t"
        "ldr	r4, [%[a], #200]\n\t"
        "ldr	r5, [%[a], #204]\n\t"
        "ldr	r6, [%[b], #192]\n\t"
        "ldr	r7, [%[b], #196]\n\t"
        "ldr	r8, [%[b], #200]\n\t"
        "ldr	r9, [%[b], #204]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #192]\n\t"
        "str	r3, [%[a], #196]\n\t"
        "str	r4, [%[a], #200]\n\t"
        "str	r5, [%[a], #204]\n\t"
        "ldr	r2, [%[a], #208]\n\t"
        "ldr	r3, [%[a], #212]\n\t"
        "ldr	r4, [%[a], #216]\n\t"
        "ldr	r5, [%[a], #220]\n\t"
        "ldr	r6, [%[b], #208]\n\t"
        "ldr	r7, [%[b], #212]\n\t"
        "ldr	r8, [%[b], #216]\n\t"
        "ldr	r9, [%[b], #220]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #208]\n\t"
        "str	r3, [%[a], #212]\n\t"
        "str	r4, [%[a], #216]\n\t"
        "str	r5, [%[a], #220]\n\t"
        "ldr	r2, [%[a], #224]\n\t"
        "ldr	r3, [%[a], #228]\n\t"
        "ldr	r4, [%[a], #232]\n\t"
        "ldr	r5, [%[a], #236]\n\t"
        "ldr	r6, [%[b], #224]\n\t"
        "ldr	r7, [%[b], #228]\n\t"
        "ldr	r8, [%[b], #232]\n\t"
        "ldr	r9, [%[b], #236]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #224]\n\t"
        "str	r3, [%[a], #228]\n\t"
        "str	r4, [%[a], #232]\n\t"
        "str	r5, [%[a], #236]\n\t"
        "ldr	r2, [%[a], #240]\n\t"
        "ldr	r3, [%[a], #244]\n\t"
        "ldr	r4, [%[a], #248]\n\t"
        "ldr	r5, [%[a], #252]\n\t"
        "ldr	r6, [%[b], #240]\n\t"
        "ldr	r7, [%[b], #244]\n\t"
        "ldr	r8, [%[b], #248]\n\t"
        "ldr	r9, [%[b], #252]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #240]\n\t"
        "str	r3, [%[a], #244]\n\t"
        "str	r4, [%[a], #248]\n\t"
        "str	r5, [%[a], #252]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_64(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r5, [%[a], #68]\n\t"
        "ldr	r6, [%[a], #72]\n\t"
        "ldr	r7, [%[a], #76]\n\t"
        "ldr	r8, [%[b], #64]\n\t"
        "ldr	r9, [%[b], #68]\n\t"
        "ldr	r10, [%[b], #72]\n\t"
        "ldr	r14, [%[b], #76]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r5, [%[r], #68]\n\t"
        "str	r6, [%[r], #72]\n\t"
        "str	r7, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r5, [%[a], #84]\n\t"
        "ldr	r6, [%[a], #88]\n\t"
        "ldr	r7, [%[a], #92]\n\t"
        "ldr	r8, [%[b], #80]\n\t"
        "ldr	r9, [%[b], #84]\n\t"
        "ldr	r10, [%[b], #88]\n\t"
        "ldr	r14, [%[b], #92]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r5, [%[r], #84]\n\t"
        "str	r6, [%[r], #88]\n\t"
        "str	r7, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r5, [%[a], #100]\n\t"
        "ldr	r6, [%[a], #104]\n\t"
        "ldr	r7, [%[a], #108]\n\t"
        "ldr	r8, [%[b], #96]\n\t"
        "ldr	r9, [%[b], #100]\n\t"
        "ldr	r10, [%[b], #104]\n\t"
        "ldr	r14, [%[b], #108]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r5, [%[r], #100]\n\t"
        "str	r6, [%[r], #104]\n\t"
        "str	r7, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r5, [%[a], #116]\n\t"
        "ldr	r6, [%[a], #120]\n\t"
        "ldr	r7, [%[a], #124]\n\t"
        "ldr	r8, [%[b], #112]\n\t"
        "ldr	r9, [%[b], #116]\n\t"
        "ldr	r10, [%[b], #120]\n\t"
        "ldr	r14, [%[b], #124]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r5, [%[r], #116]\n\t"
        "str	r6, [%[r], #120]\n\t"
        "str	r7, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r5, [%[a], #132]\n\t"
        "ldr	r6, [%[a], #136]\n\t"
        "ldr	r7, [%[a], #140]\n\t"
        "ldr	r8, [%[b], #128]\n\t"
        "ldr	r9, [%[b], #132]\n\t"
        "ldr	r10, [%[b], #136]\n\t"
        "ldr	r14, [%[b], #140]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r5, [%[r], #132]\n\t"
        "str	r6, [%[r], #136]\n\t"
        "str	r7, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r5, [%[a], #148]\n\t"
        "ldr	r6, [%[a], #152]\n\t"
        "ldr	r7, [%[a], #156]\n\t"
        "ldr	r8, [%[b], #144]\n\t"
        "ldr	r9, [%[b], #148]\n\t"
        "ldr	r10, [%[b], #152]\n\t"
        "ldr	r14, [%[b], #156]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r5, [%[r], #148]\n\t"
        "str	r6, [%[r], #152]\n\t"
        "str	r7, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r5, [%[a], #164]\n\t"
        "ldr	r6, [%[a], #168]\n\t"
        "ldr	r7, [%[a], #172]\n\t"
        "ldr	r8, [%[b], #160]\n\t"
        "ldr	r9, [%[b], #164]\n\t"
        "ldr	r10, [%[b], #168]\n\t"
        "ldr	r14, [%[b], #172]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r5, [%[r], #164]\n\t"
        "str	r6, [%[r], #168]\n\t"
        "str	r7, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r5, [%[a], #180]\n\t"
        "ldr	r6, [%[a], #184]\n\t"
        "ldr	r7, [%[a], #188]\n\t"
        "ldr	r8, [%[b], #176]\n\t"
        "ldr	r9, [%[b], #180]\n\t"
        "ldr	r10, [%[b], #184]\n\t"
        "ldr	r14, [%[b], #188]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r5, [%[r], #180]\n\t"
        "str	r6, [%[r], #184]\n\t"
        "str	r7, [%[r], #188]\n\t"
        "ldr	r4, [%[a], #192]\n\t"
        "ldr	r5, [%[a], #196]\n\t"
        "ldr	r6, [%[a], #200]\n\t"
        "ldr	r7, [%[a], #204]\n\t"
        "ldr	r8, [%[b], #192]\n\t"
        "ldr	r9, [%[b], #196]\n\t"
        "ldr	r10, [%[b], #200]\n\t"
        "ldr	r14, [%[b], #204]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #192]\n\t"
        "str	r5, [%[r], #196]\n\t"
        "str	r6, [%[r], #200]\n\t"
        "str	r7, [%[r], #204]\n\t"
        "ldr	r4, [%[a], #208]\n\t"
        "ldr	r5, [%[a], #212]\n\t"
        "ldr	r6, [%[a], #216]\n\t"
        "ldr	r7, [%[a], #220]\n\t"
        "ldr	r8, [%[b], #208]\n\t"
        "ldr	r9, [%[b], #212]\n\t"
        "ldr	r10, [%[b], #216]\n\t"
        "ldr	r14, [%[b], #220]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #208]\n\t"
        "str	r5, [%[r], #212]\n\t"
        "str	r6, [%[r], #216]\n\t"
        "str	r7, [%[r], #220]\n\t"
        "ldr	r4, [%[a], #224]\n\t"
        "ldr	r5, [%[a], #228]\n\t"
        "ldr	r6, [%[a], #232]\n\t"
        "ldr	r7, [%[a], #236]\n\t"
        "ldr	r8, [%[b], #224]\n\t"
        "ldr	r9, [%[b], #228]\n\t"
        "ldr	r10, [%[b], #232]\n\t"
        "ldr	r14, [%[b], #236]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #224]\n\t"
        "str	r5, [%[r], #228]\n\t"
        "str	r6, [%[r], #232]\n\t"
        "str	r7, [%[r], #236]\n\t"
        "ldr	r4, [%[a], #240]\n\t"
        "ldr	r5, [%[a], #244]\n\t"
        "ldr	r6, [%[a], #248]\n\t"
        "ldr	r7, [%[a], #252]\n\t"
        "ldr	r8, [%[b], #240]\n\t"
        "ldr	r9, [%[b], #244]\n\t"
        "ldr	r10, [%[b], #248]\n\t"
        "ldr	r14, [%[b], #252]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #240]\n\t"
        "str	r5, [%[r], #244]\n\t"
        "str	r6, [%[r], #248]\n\t"
        "str	r7, [%[r], #252]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_32(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<32; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
#endif
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_64(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[64];
    sp_digit a1[32];
    sp_digit b1[32];
    sp_digit z2[64];
    sp_digit u, ca, cb;

    ca = sp_2048_add_32(a1, a, &a[32]);
    cb = sp_2048_add_32(b1, b, &b[32]);
    u  = ca & cb;
    sp_2048_mul_32(z1, a1, b1);
    sp_2048_mul_32(z2, &a[32], &b[32]);
    sp_2048_mul_32(z0, a, b);
    sp_2048_mask_32(r + 64, a1, 0 - cb);
    sp_2048_mask_32(b1, b1, 0 - ca);
    u += sp_2048_add_32(r + 64, r + 64, b1);
    u += sp_2048_sub_in_place_64(z1, z2);
    u += sp_2048_sub_in_place_64(z1, z0);
    u += sp_2048_add_64(r + 32, r + 32, z1);
    r[96] = u;
    XMEMSET(r + 96 + 1, 0, sizeof(sp_digit) * (32 - 1));
    sp_2048_add_64(r + 64, r + 64, z2);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_64(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z2[64];
    sp_digit z1[64];
    sp_digit a1[32];
    sp_digit u;

    u = sp_2048_add_32(a1, a, &a[32]);
    sp_2048_sqr_32(z1, a1);
    sp_2048_sqr_32(z2, &a[32]);
    sp_2048_sqr_32(z0, a);
    sp_2048_mask_32(r + 64, a1, 0 - u);
    u += sp_2048_add_32(r + 64, r + 64, r + 64);
    u += sp_2048_sub_in_place_64(z1, z2);
    u += sp_2048_sub_in_place_64(z1, z0);
    u += sp_2048_add_64(r + 32, r + 32, z1);
    r[96] = u;
    XMEMSET(r + 96 + 1, 0, sizeof(sp_digit) * (32 - 1));
    sp_2048_add_64(r + 64, r + 64, z2);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_64(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #256\n\t"
        "\n1:\n\t"
        "adds	%[c], %[c], #-1\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "mov	r4, #0\n\t"
        "adc	%[c], r4, #0\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_sub_in_place_64(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "add	r12, %[a], #256\n\t"
        "\n1:\n\t"
        "subs	%[c], r14, %[c]\n\t"
        "ldr	r3, [%[a]]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[a]], #4\n\t"
        "str	r4, [%[a]], #4\n\t"
        "str	r5, [%[a]], #4\n\t"
        "str	r6, [%[a]], #4\n\t"
        "sbc	%[c], r14, r14\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_64(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[128];

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #252\n\t"
        "movcc	r3, #0\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r12, [%[b], r4]\n\t"
        "umull	r9, r10, r14, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, #0\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #256\n\t"
        "beq	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #504\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_64(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[128];

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r5, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #252\n\t"
        "movcc	r3, r12\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "cmp	r4, r3\n\t"
        "beq	4f\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r9, [%[a], r4]\n\t"
        "umull	r9, r10, r14, r9\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "bal	5f\n\t"
        "\n4:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "umull	r9, r10, r14, r14\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "\n5:\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #256\n\t"
        "beq	3f\n\t"
        "cmp	r3, r4\n\t"
        "bgt	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #504\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r9", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
#ifdef WOLFSSL_SP_SMALL
/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_32(sp_digit* r, sp_digit* a, sp_digit m)
{
    int i;

    for (i=0; i<32; i++)
        r[i] = a[i] & m;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_add_32(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #128\n\t"
        "\n1:\n\t"
        "adds	%[c], %[c], #-1\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "mov	r4, #0\n\t"
        "adc	%[c], r4, #0\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_2048_sub_in_place_32(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "add	r12, %[a], #128\n\t"
        "\n1:\n\t"
        "subs	%[c], r14, %[c]\n\t"
        "ldr	r3, [%[a]]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[a]], #4\n\t"
        "str	r4, [%[a]], #4\n\t"
        "str	r5, [%[a]], #4\n\t"
        "str	r6, [%[a]], #4\n\t"
        "sbc	%[c], r14, r14\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_2048_mul_32(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[64];

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #124\n\t"
        "movcc	r3, #0\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r12, [%[b], r4]\n\t"
        "umull	r9, r10, r14, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, #0\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #128\n\t"
        "beq	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #248\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_2048_sqr_32(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[64];

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r5, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #124\n\t"
        "movcc	r3, r12\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "cmp	r4, r3\n\t"
        "beq	4f\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r9, [%[a], r4]\n\t"
        "umull	r9, r10, r14, r9\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "bal	5f\n\t"
        "\n4:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "umull	r9, r10, r14, r14\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "\n5:\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #128\n\t"
        "beq	3f\n\t"
        "cmp	r3, r4\n\t"
        "bgt	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #248\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r9", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* Caclulate the bottom digit of -1/a mod 2^n.
 *
 * a    A single precision number.
 * rho  Bottom word of inverse.
 */
static void sp_2048_mont_setup(sp_digit* a, sp_digit* rho)
{
    sp_digit x, b;

    b = a[0];
    x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**32 */

    /* rho = -1/m mod b */
    *rho = -x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 2048 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_2048_mont_norm_32(sp_digit* r, sp_digit* m)
{
    XMEMSET(r, 0, sizeof(sp_digit) * 32);

    /* r = 2^n mod m */
    sp_2048_sub_in_place_32(r, m);
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static sp_digit sp_2048_cond_sub_32(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit m)
{
    sp_digit c = 0;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r9, #0\n\t"
        "mov	r8, #0\n\t"
        "1:\n\t"
        "subs	%[c], r9, %[c]\n\t"
        "ldr	r4, [%[a], r8]\n\t"
        "ldr	r5, [%[b], r8]\n\t"
        "and	r5, r5, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbc	%[c], r9, r9\n\t"
        "str	r4, [%[r], r8]\n\t"
        "add	r8, r8, #4\n\t"
        "cmp	r8, #128\n\t"
        "blt	1b\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#else
    __asm__ __volatile__ (

        "mov	r9, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r5, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "subs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r6, [%[r], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r5, [%[b], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r5, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r6, [%[r], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r5, [%[b], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r6, [%[a], #36]\n\t"
        "ldr	r5, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r6, [%[r], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r6, [%[a], #44]\n\t"
        "ldr	r5, [%[b], #40]\n\t"
        "ldr	r7, [%[b], #44]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #40]\n\t"
        "str	r6, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r6, [%[a], #52]\n\t"
        "ldr	r5, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r6, [%[r], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r6, [%[a], #60]\n\t"
        "ldr	r5, [%[b], #56]\n\t"
        "ldr	r7, [%[b], #60]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r6, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r6, [%[a], #68]\n\t"
        "ldr	r5, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r6, [%[r], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r6, [%[a], #76]\n\t"
        "ldr	r5, [%[b], #72]\n\t"
        "ldr	r7, [%[b], #76]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #72]\n\t"
        "str	r6, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r6, [%[a], #84]\n\t"
        "ldr	r5, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r6, [%[r], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r6, [%[a], #92]\n\t"
        "ldr	r5, [%[b], #88]\n\t"
        "ldr	r7, [%[b], #92]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #88]\n\t"
        "str	r6, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r6, [%[a], #100]\n\t"
        "ldr	r5, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r6, [%[r], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r6, [%[a], #108]\n\t"
        "ldr	r5, [%[b], #104]\n\t"
        "ldr	r7, [%[b], #108]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #104]\n\t"
        "str	r6, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r6, [%[a], #116]\n\t"
        "ldr	r5, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r6, [%[r], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r6, [%[a], #124]\n\t"
        "ldr	r5, [%[b], #120]\n\t"
        "ldr	r7, [%[b], #124]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #120]\n\t"
        "str	r6, [%[r], #124]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#endif /* WOLFSSL_SP_SMALL */

    return c;
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
SP_NOINLINE static void sp_2048_mont_reduce_32(sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit ca = 0;

    __asm__ __volatile__ (
        "# i = 0\n\t"
        "mov	r12, #0\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "ldr	r14, [%[a], #4]\n\t"
        "\n1:\n\t"
        "# mu = a[i] * mp\n\t"
        "mul	r8, %[mp], r10\n\t"
        "# a[i+0] += m[0] * mu\n\t"
        "ldr	r7, [%[m], #0]\n\t"
        "ldr	r9, [%[a], #0]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r10, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "# a[i+1] += m[1] * mu\n\t"
        "ldr	r7, [%[m], #4]\n\t"
        "ldr	r9, [%[a], #4]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r14, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r10, r10, r5\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+2] += m[2] * mu\n\t"
        "ldr	r7, [%[m], #8]\n\t"
        "ldr	r14, [%[a], #8]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r14, r14, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r14, r14, r4\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+3] += m[3] * mu\n\t"
        "ldr	r7, [%[m], #12]\n\t"
        "ldr	r9, [%[a], #12]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #12]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+4] += m[4] * mu\n\t"
        "ldr	r7, [%[m], #16]\n\t"
        "ldr	r9, [%[a], #16]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #16]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+5] += m[5] * mu\n\t"
        "ldr	r7, [%[m], #20]\n\t"
        "ldr	r9, [%[a], #20]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #20]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+6] += m[6] * mu\n\t"
        "ldr	r7, [%[m], #24]\n\t"
        "ldr	r9, [%[a], #24]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #24]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+7] += m[7] * mu\n\t"
        "ldr	r7, [%[m], #28]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #28]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+8] += m[8] * mu\n\t"
        "ldr	r7, [%[m], #32]\n\t"
        "ldr	r9, [%[a], #32]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #32]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+9] += m[9] * mu\n\t"
        "ldr	r7, [%[m], #36]\n\t"
        "ldr	r9, [%[a], #36]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #36]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+10] += m[10] * mu\n\t"
        "ldr	r7, [%[m], #40]\n\t"
        "ldr	r9, [%[a], #40]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #40]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+11] += m[11] * mu\n\t"
        "ldr	r7, [%[m], #44]\n\t"
        "ldr	r9, [%[a], #44]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #44]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+12] += m[12] * mu\n\t"
        "ldr	r7, [%[m], #48]\n\t"
        "ldr	r9, [%[a], #48]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #48]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+13] += m[13] * mu\n\t"
        "ldr	r7, [%[m], #52]\n\t"
        "ldr	r9, [%[a], #52]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #52]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+14] += m[14] * mu\n\t"
        "ldr	r7, [%[m], #56]\n\t"
        "ldr	r9, [%[a], #56]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #56]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+15] += m[15] * mu\n\t"
        "ldr	r7, [%[m], #60]\n\t"
        "ldr	r9, [%[a], #60]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #60]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+16] += m[16] * mu\n\t"
        "ldr	r7, [%[m], #64]\n\t"
        "ldr	r9, [%[a], #64]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #64]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+17] += m[17] * mu\n\t"
        "ldr	r7, [%[m], #68]\n\t"
        "ldr	r9, [%[a], #68]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #68]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+18] += m[18] * mu\n\t"
        "ldr	r7, [%[m], #72]\n\t"
        "ldr	r9, [%[a], #72]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #72]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+19] += m[19] * mu\n\t"
        "ldr	r7, [%[m], #76]\n\t"
        "ldr	r9, [%[a], #76]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #76]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+20] += m[20] * mu\n\t"
        "ldr	r7, [%[m], #80]\n\t"
        "ldr	r9, [%[a], #80]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #80]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+21] += m[21] * mu\n\t"
        "ldr	r7, [%[m], #84]\n\t"
        "ldr	r9, [%[a], #84]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #84]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+22] += m[22] * mu\n\t"
        "ldr	r7, [%[m], #88]\n\t"
        "ldr	r9, [%[a], #88]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #88]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+23] += m[23] * mu\n\t"
        "ldr	r7, [%[m], #92]\n\t"
        "ldr	r9, [%[a], #92]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #92]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+24] += m[24] * mu\n\t"
        "ldr	r7, [%[m], #96]\n\t"
        "ldr	r9, [%[a], #96]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #96]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+25] += m[25] * mu\n\t"
        "ldr	r7, [%[m], #100]\n\t"
        "ldr	r9, [%[a], #100]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #100]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+26] += m[26] * mu\n\t"
        "ldr	r7, [%[m], #104]\n\t"
        "ldr	r9, [%[a], #104]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #104]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+27] += m[27] * mu\n\t"
        "ldr	r7, [%[m], #108]\n\t"
        "ldr	r9, [%[a], #108]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #108]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+28] += m[28] * mu\n\t"
        "ldr	r7, [%[m], #112]\n\t"
        "ldr	r9, [%[a], #112]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #112]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+29] += m[29] * mu\n\t"
        "ldr	r7, [%[m], #116]\n\t"
        "ldr	r9, [%[a], #116]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #116]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+30] += m[30] * mu\n\t"
        "ldr	r7, [%[m], #120]\n\t"
        "ldr	r9, [%[a], #120]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #120]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+31] += m[31] * mu\n\t"
        "ldr	r7, [%[m], #124]\n\t"
        "ldr   r9, [%[a], #124]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r7, r7, %[ca]\n\t"
        "mov	%[ca], #0\n\t"
        "adc	%[ca], %[ca], %[ca]\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #124]\n\t"
        "ldr	r9, [%[a], #128]\n\t"
        "adcs	r9, r9, r7\n\t"
        "str	r9, [%[a], #128]\n\t"
        "adc	%[ca], %[ca], #0\n\t"
        "# i += 1\n\t"
        "add	%[a], %[a], #4\n\t"
        "add	r12, r12, #4\n\t"
        "cmp	r12, #128\n\t"
        "blt	1b\n\t"
        "str	r10, [%[a], #0]\n\t"
        "str	r14, [%[a], #4]\n\t"
        : [ca] "+r" (ca), [a] "+r" (a)
        : [m] "r" (m), [mp] "r" (mp)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    sp_2048_cond_sub_32(a - 32, a, m, (sp_digit)0 - ca);
}

/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_mul_32(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_32(r, a, b);
    sp_2048_mont_reduce_32(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_32(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_32(r, a);
    sp_2048_mont_reduce_32(r, m, mp);
}

/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
static void sp_2048_mul_d_32(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r5, r3, %[b], r8\n\t"
        "mov	r4, #0\n\t"
        "str	r5, [%[r]]\n\t"
        "mov	r5, #0\n\t"
        "mov	r9, #4\n\t"
        "1:\n\t"
        "ldr	r8, [%[a], r9]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], r9]\n\t"
        "mov	r3, r4\n\t"
        "mov	r4, r5\n\t"
        "mov	r5, #0\n\t"
        "add	r9, r9, #4\n\t"
        "cmp	r9, #128\n\t"
        "blt	1b\n\t"
        "str	r3, [%[r], #128]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#else
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r3, r4, %[b], r8\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[r]]\n\t"
        "# A[1] * B\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #4]\n\t"
        "# A[2] * B\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #8]\n\t"
        "# A[3] * B\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #12]\n\t"
        "# A[4] * B\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #16]\n\t"
        "# A[5] * B\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #20]\n\t"
        "# A[6] * B\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #24]\n\t"
        "# A[7] * B\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #28]\n\t"
        "# A[8] * B\n\t"
        "ldr	r8, [%[a], #32]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "# A[9] * B\n\t"
        "ldr	r8, [%[a], #36]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "# A[10] * B\n\t"
        "ldr	r8, [%[a], #40]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "# A[11] * B\n\t"
        "ldr	r8, [%[a], #44]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "# A[12] * B\n\t"
        "ldr	r8, [%[a], #48]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "# A[13] * B\n\t"
        "ldr	r8, [%[a], #52]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "# A[14] * B\n\t"
        "ldr	r8, [%[a], #56]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #56]\n\t"
        "# A[15] * B\n\t"
        "ldr	r8, [%[a], #60]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #60]\n\t"
        "# A[16] * B\n\t"
        "ldr	r8, [%[a], #64]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #64]\n\t"
        "# A[17] * B\n\t"
        "ldr	r8, [%[a], #68]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #68]\n\t"
        "# A[18] * B\n\t"
        "ldr	r8, [%[a], #72]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #72]\n\t"
        "# A[19] * B\n\t"
        "ldr	r8, [%[a], #76]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #76]\n\t"
        "# A[20] * B\n\t"
        "ldr	r8, [%[a], #80]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #80]\n\t"
        "# A[21] * B\n\t"
        "ldr	r8, [%[a], #84]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #84]\n\t"
        "# A[22] * B\n\t"
        "ldr	r8, [%[a], #88]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #88]\n\t"
        "# A[23] * B\n\t"
        "ldr	r8, [%[a], #92]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #92]\n\t"
        "# A[24] * B\n\t"
        "ldr	r8, [%[a], #96]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #96]\n\t"
        "# A[25] * B\n\t"
        "ldr	r8, [%[a], #100]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #100]\n\t"
        "# A[26] * B\n\t"
        "ldr	r8, [%[a], #104]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #104]\n\t"
        "# A[27] * B\n\t"
        "ldr	r8, [%[a], #108]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #108]\n\t"
        "# A[28] * B\n\t"
        "ldr	r8, [%[a], #112]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #112]\n\t"
        "# A[29] * B\n\t"
        "ldr	r8, [%[a], #116]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #116]\n\t"
        "# A[30] * B\n\t"
        "ldr	r8, [%[a], #120]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #120]\n\t"
        "# A[31] * B\n\t"
        "ldr	r8, [%[a], #124]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "str	r4, [%[r], #124]\n\t"
        "str	r5, [%[r], #128]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#endif
}

/* Divide the double width number (d1|d0) by the dividend. (d1|d0 / div)
 *
 * d1   The high order half of the number to divide.
 * d0   The low order half of the number to divide.
 * div  The dividend.
 * returns the result of the division.
 *
 * Note that this is an approximate div. It may give an answer 1 larger.
 */
static sp_digit div_2048_word_32(sp_digit d1, sp_digit d0, sp_digit div)
{
    sp_digit r = 0;

    __asm__ __volatile__ (
        "lsr	r5, %[div], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[d0]\n\t"
        "mov	r7, %[d1]\n\t"
        "# Do top 32\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "# Next 30 bits\n\t"
        "mov	r4, #29\n\t"
        "1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r7, r7, r7\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "subs	r8, %[div], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r)
        : [d1] "r" (d1), [d0] "r" (d0), [div] "r" (div)
        : "r4", "r5", "r6", "r7", "r8"
    );
    return r;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static int32_t sp_2048_cmp_32(sp_digit* a, sp_digit* b)
{
    sp_digit r = -1;
    sp_digit one = 1;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "mov	r6, #124\n\t"
        "1:\n\t"
        "ldr	r4, [%[a], r6]\n\t"
        "ldr	r5, [%[b], r6]\n\t"
        "and	r4, r4, r3\n\t"
        "and	r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "sub	r6, r6, #4\n\t"
        "bcc	1b\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#else
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "ldr		r4, [%[a], #124]\n\t"
        "ldr		r5, [%[b], #124]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #120]\n\t"
        "ldr		r5, [%[b], #120]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #116]\n\t"
        "ldr		r5, [%[b], #116]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #112]\n\t"
        "ldr		r5, [%[b], #112]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #108]\n\t"
        "ldr		r5, [%[b], #108]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #104]\n\t"
        "ldr		r5, [%[b], #104]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #100]\n\t"
        "ldr		r5, [%[b], #100]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #96]\n\t"
        "ldr		r5, [%[b], #96]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #92]\n\t"
        "ldr		r5, [%[b], #92]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #88]\n\t"
        "ldr		r5, [%[b], #88]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #84]\n\t"
        "ldr		r5, [%[b], #84]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #80]\n\t"
        "ldr		r5, [%[b], #80]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #76]\n\t"
        "ldr		r5, [%[b], #76]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #72]\n\t"
        "ldr		r5, [%[b], #72]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #68]\n\t"
        "ldr		r5, [%[b], #68]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #64]\n\t"
        "ldr		r5, [%[b], #64]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #60]\n\t"
        "ldr		r5, [%[b], #60]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #56]\n\t"
        "ldr		r5, [%[b], #56]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #52]\n\t"
        "ldr		r5, [%[b], #52]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #48]\n\t"
        "ldr		r5, [%[b], #48]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #44]\n\t"
        "ldr		r5, [%[b], #44]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #40]\n\t"
        "ldr		r5, [%[b], #40]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #36]\n\t"
        "ldr		r5, [%[b], #36]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #32]\n\t"
        "ldr		r5, [%[b], #32]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #28]\n\t"
        "ldr		r5, [%[b], #28]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #24]\n\t"
        "ldr		r5, [%[b], #24]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #20]\n\t"
        "ldr		r5, [%[b], #20]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #16]\n\t"
        "ldr		r5, [%[b], #16]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #12]\n\t"
        "ldr		r5, [%[b], #12]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #8]\n\t"
        "ldr		r5, [%[b], #8]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #4]\n\t"
        "ldr		r5, [%[b], #4]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #0]\n\t"
        "ldr		r5, [%[b], #0]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#endif

    return r;
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_div_32(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[64], t2[33];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[31];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 32);
    for (i=31; i>=0; i--) {
        r1 = div_2048_word_32(t1[32 + i], t1[32 + i - 1], div);

        sp_2048_mul_d_32(t2, d, r1);
        t1[32 + i] += sp_2048_sub_in_place_32(&t1[i], t2);
        t1[32 + i] -= t2[32];
        sp_2048_mask_32(t2, d, t1[32 + i]);
        t1[32 + i] += sp_2048_add_32(&t1[i], &t1[i], t2);
        sp_2048_mask_32(t2, d, t1[32 + i]);
        t1[32 + i] += sp_2048_add_32(&t1[i], &t1[i], t2);
    }

    r1 = sp_2048_cmp_32(t1, d) >= 0;
    sp_2048_cond_sub_32(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_mod_32(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_32(a, m, NULL, r);
}

#ifdef WOLFSSL_SP_SMALL
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_32(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[16][64];
#else
    sp_digit* t[16];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 16 * 64, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<16; i++)
            t[i] = td + i * 64;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_32(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 32);
        if (reduceA) {
            err = sp_2048_mod_32(t[1] + 32, a, m);
            if (err == MP_OKAY)
                err = sp_2048_mod_32(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 32, a, sizeof(sp_digit) * 32);
            err = sp_2048_mod_32(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_32(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_32(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_32(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_32(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_32(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_32(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_32(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_32(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_32(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_32(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_32(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_32(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_32(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_32(t[15], t[ 8], t[ 7], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 28;
        n <<= 4;
        c = 28;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 32);
        for (; i>=0 || c>=4; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 28;
                n <<= 4;
                c = 28;
            }
            else if (c < 4) {
                y = n >> 28;
                n = e[i--];
                c = 4 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 28) & 0xf;
                n <<= 4;
                c -= 4;
            }

            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);

            sp_2048_mont_mul_32(r, r, t[y], m, mp);
        }

        XMEMSET(&r[32], 0, sizeof(sp_digit) * 32);
        sp_2048_mont_reduce_32(r, m, mp);

        mask = 0 - (sp_2048_cmp_32(r, m) >= 0);
        sp_2048_cond_sub_32(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#else
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_32(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][64];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 64, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 64;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_32(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 32);
        if (reduceA) {
            err = sp_2048_mod_32(t[1] + 32, a, m);
            if (err == MP_OKAY)
                err = sp_2048_mod_32(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 32, a, sizeof(sp_digit) * 32);
            err = sp_2048_mod_32(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_32(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_32(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_32(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_32(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_32(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_32(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_32(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_32(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_32(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_32(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_32(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_32(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_32(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_32(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_32(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_32(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_32(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_32(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_32(t[20], t[10], m, mp);
        sp_2048_mont_mul_32(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_32(t[22], t[11], m, mp);
        sp_2048_mont_mul_32(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_32(t[24], t[12], m, mp);
        sp_2048_mont_mul_32(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_32(t[26], t[13], m, mp);
        sp_2048_mont_mul_32(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_32(t[28], t[14], m, mp);
        sp_2048_mont_mul_32(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_32(t[30], t[15], m, mp);
        sp_2048_mont_mul_32(t[31], t[16], t[15], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 27;
        n <<= 5;
        c = 27;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 32);
        for (; i>=0 || c>=5; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 27;
                n <<= 5;
                c = 27;
            }
            else if (c < 5) {
                y = n >> 27;
                n = e[i--];
                c = 5 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 27) & 0x1f;
                n <<= 5;
                c -= 5;
            }

            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);
            sp_2048_mont_sqr_32(r, r, m, mp);

            sp_2048_mont_mul_32(r, r, t[y], m, mp);
        }
        y = e[0] & 0xf;
        sp_2048_mont_sqr_32(r, r, m, mp);
        sp_2048_mont_sqr_32(r, r, m, mp);
        sp_2048_mont_sqr_32(r, r, m, mp);
        sp_2048_mont_sqr_32(r, r, m, mp);
        sp_2048_mont_mul_32(r, r, t[y], m, mp);

        XMEMSET(&r[32], 0, sizeof(sp_digit) * 32);
        sp_2048_mont_reduce_32(r, m, mp);

        mask = 0 - (sp_2048_cmp_32(r, m) >= 0);
        sp_2048_cond_sub_32(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#endif /* WOLFSSL_SP_SMALL */

#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 2048 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_2048_mont_norm_64(sp_digit* r, sp_digit* m)
{
    XMEMSET(r, 0, sizeof(sp_digit) * 64);

    /* r = 2^n mod m */
    sp_2048_sub_in_place_64(r, m);
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static sp_digit sp_2048_cond_sub_64(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit m)
{
    sp_digit c = 0;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r9, #0\n\t"
        "mov	r8, #0\n\t"
        "1:\n\t"
        "subs	%[c], r9, %[c]\n\t"
        "ldr	r4, [%[a], r8]\n\t"
        "ldr	r5, [%[b], r8]\n\t"
        "and	r5, r5, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbc	%[c], r9, r9\n\t"
        "str	r4, [%[r], r8]\n\t"
        "add	r8, r8, #4\n\t"
        "cmp	r8, #256\n\t"
        "blt	1b\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#else
    __asm__ __volatile__ (

        "mov	r9, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r5, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "subs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r6, [%[r], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r5, [%[b], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r5, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r6, [%[r], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r5, [%[b], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r6, [%[a], #36]\n\t"
        "ldr	r5, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r6, [%[r], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r6, [%[a], #44]\n\t"
        "ldr	r5, [%[b], #40]\n\t"
        "ldr	r7, [%[b], #44]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #40]\n\t"
        "str	r6, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r6, [%[a], #52]\n\t"
        "ldr	r5, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r6, [%[r], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r6, [%[a], #60]\n\t"
        "ldr	r5, [%[b], #56]\n\t"
        "ldr	r7, [%[b], #60]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r6, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r6, [%[a], #68]\n\t"
        "ldr	r5, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r6, [%[r], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r6, [%[a], #76]\n\t"
        "ldr	r5, [%[b], #72]\n\t"
        "ldr	r7, [%[b], #76]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #72]\n\t"
        "str	r6, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r6, [%[a], #84]\n\t"
        "ldr	r5, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r6, [%[r], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r6, [%[a], #92]\n\t"
        "ldr	r5, [%[b], #88]\n\t"
        "ldr	r7, [%[b], #92]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #88]\n\t"
        "str	r6, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r6, [%[a], #100]\n\t"
        "ldr	r5, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r6, [%[r], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r6, [%[a], #108]\n\t"
        "ldr	r5, [%[b], #104]\n\t"
        "ldr	r7, [%[b], #108]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #104]\n\t"
        "str	r6, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r6, [%[a], #116]\n\t"
        "ldr	r5, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r6, [%[r], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r6, [%[a], #124]\n\t"
        "ldr	r5, [%[b], #120]\n\t"
        "ldr	r7, [%[b], #124]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #120]\n\t"
        "str	r6, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r6, [%[a], #132]\n\t"
        "ldr	r5, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r6, [%[r], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r6, [%[a], #140]\n\t"
        "ldr	r5, [%[b], #136]\n\t"
        "ldr	r7, [%[b], #140]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #136]\n\t"
        "str	r6, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r6, [%[a], #148]\n\t"
        "ldr	r5, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r6, [%[r], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r6, [%[a], #156]\n\t"
        "ldr	r5, [%[b], #152]\n\t"
        "ldr	r7, [%[b], #156]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #152]\n\t"
        "str	r6, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r6, [%[a], #164]\n\t"
        "ldr	r5, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r6, [%[r], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r6, [%[a], #172]\n\t"
        "ldr	r5, [%[b], #168]\n\t"
        "ldr	r7, [%[b], #172]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #168]\n\t"
        "str	r6, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r6, [%[a], #180]\n\t"
        "ldr	r5, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r6, [%[r], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r6, [%[a], #188]\n\t"
        "ldr	r5, [%[b], #184]\n\t"
        "ldr	r7, [%[b], #188]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #184]\n\t"
        "str	r6, [%[r], #188]\n\t"
        "ldr	r4, [%[a], #192]\n\t"
        "ldr	r6, [%[a], #196]\n\t"
        "ldr	r5, [%[b], #192]\n\t"
        "ldr	r7, [%[b], #196]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #192]\n\t"
        "str	r6, [%[r], #196]\n\t"
        "ldr	r4, [%[a], #200]\n\t"
        "ldr	r6, [%[a], #204]\n\t"
        "ldr	r5, [%[b], #200]\n\t"
        "ldr	r7, [%[b], #204]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #200]\n\t"
        "str	r6, [%[r], #204]\n\t"
        "ldr	r4, [%[a], #208]\n\t"
        "ldr	r6, [%[a], #212]\n\t"
        "ldr	r5, [%[b], #208]\n\t"
        "ldr	r7, [%[b], #212]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #208]\n\t"
        "str	r6, [%[r], #212]\n\t"
        "ldr	r4, [%[a], #216]\n\t"
        "ldr	r6, [%[a], #220]\n\t"
        "ldr	r5, [%[b], #216]\n\t"
        "ldr	r7, [%[b], #220]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #216]\n\t"
        "str	r6, [%[r], #220]\n\t"
        "ldr	r4, [%[a], #224]\n\t"
        "ldr	r6, [%[a], #228]\n\t"
        "ldr	r5, [%[b], #224]\n\t"
        "ldr	r7, [%[b], #228]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #224]\n\t"
        "str	r6, [%[r], #228]\n\t"
        "ldr	r4, [%[a], #232]\n\t"
        "ldr	r6, [%[a], #236]\n\t"
        "ldr	r5, [%[b], #232]\n\t"
        "ldr	r7, [%[b], #236]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #232]\n\t"
        "str	r6, [%[r], #236]\n\t"
        "ldr	r4, [%[a], #240]\n\t"
        "ldr	r6, [%[a], #244]\n\t"
        "ldr	r5, [%[b], #240]\n\t"
        "ldr	r7, [%[b], #244]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #240]\n\t"
        "str	r6, [%[r], #244]\n\t"
        "ldr	r4, [%[a], #248]\n\t"
        "ldr	r6, [%[a], #252]\n\t"
        "ldr	r5, [%[b], #248]\n\t"
        "ldr	r7, [%[b], #252]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #248]\n\t"
        "str	r6, [%[r], #252]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#endif /* WOLFSSL_SP_SMALL */

    return c;
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
SP_NOINLINE static void sp_2048_mont_reduce_64(sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit ca = 0;

    __asm__ __volatile__ (
        "# i = 0\n\t"
        "mov	r12, #0\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "ldr	r14, [%[a], #4]\n\t"
        "\n1:\n\t"
        "# mu = a[i] * mp\n\t"
        "mul	r8, %[mp], r10\n\t"
        "# a[i+0] += m[0] * mu\n\t"
        "ldr	r7, [%[m], #0]\n\t"
        "ldr	r9, [%[a], #0]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r10, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "# a[i+1] += m[1] * mu\n\t"
        "ldr	r7, [%[m], #4]\n\t"
        "ldr	r9, [%[a], #4]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r14, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r10, r10, r5\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+2] += m[2] * mu\n\t"
        "ldr	r7, [%[m], #8]\n\t"
        "ldr	r14, [%[a], #8]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r14, r14, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r14, r14, r4\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+3] += m[3] * mu\n\t"
        "ldr	r7, [%[m], #12]\n\t"
        "ldr	r9, [%[a], #12]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #12]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+4] += m[4] * mu\n\t"
        "ldr	r7, [%[m], #16]\n\t"
        "ldr	r9, [%[a], #16]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #16]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+5] += m[5] * mu\n\t"
        "ldr	r7, [%[m], #20]\n\t"
        "ldr	r9, [%[a], #20]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #20]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+6] += m[6] * mu\n\t"
        "ldr	r7, [%[m], #24]\n\t"
        "ldr	r9, [%[a], #24]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #24]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+7] += m[7] * mu\n\t"
        "ldr	r7, [%[m], #28]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #28]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+8] += m[8] * mu\n\t"
        "ldr	r7, [%[m], #32]\n\t"
        "ldr	r9, [%[a], #32]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #32]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+9] += m[9] * mu\n\t"
        "ldr	r7, [%[m], #36]\n\t"
        "ldr	r9, [%[a], #36]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #36]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+10] += m[10] * mu\n\t"
        "ldr	r7, [%[m], #40]\n\t"
        "ldr	r9, [%[a], #40]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #40]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+11] += m[11] * mu\n\t"
        "ldr	r7, [%[m], #44]\n\t"
        "ldr	r9, [%[a], #44]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #44]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+12] += m[12] * mu\n\t"
        "ldr	r7, [%[m], #48]\n\t"
        "ldr	r9, [%[a], #48]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #48]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+13] += m[13] * mu\n\t"
        "ldr	r7, [%[m], #52]\n\t"
        "ldr	r9, [%[a], #52]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #52]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+14] += m[14] * mu\n\t"
        "ldr	r7, [%[m], #56]\n\t"
        "ldr	r9, [%[a], #56]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #56]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+15] += m[15] * mu\n\t"
        "ldr	r7, [%[m], #60]\n\t"
        "ldr	r9, [%[a], #60]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #60]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+16] += m[16] * mu\n\t"
        "ldr	r7, [%[m], #64]\n\t"
        "ldr	r9, [%[a], #64]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #64]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+17] += m[17] * mu\n\t"
        "ldr	r7, [%[m], #68]\n\t"
        "ldr	r9, [%[a], #68]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #68]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+18] += m[18] * mu\n\t"
        "ldr	r7, [%[m], #72]\n\t"
        "ldr	r9, [%[a], #72]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #72]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+19] += m[19] * mu\n\t"
        "ldr	r7, [%[m], #76]\n\t"
        "ldr	r9, [%[a], #76]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #76]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+20] += m[20] * mu\n\t"
        "ldr	r7, [%[m], #80]\n\t"
        "ldr	r9, [%[a], #80]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #80]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+21] += m[21] * mu\n\t"
        "ldr	r7, [%[m], #84]\n\t"
        "ldr	r9, [%[a], #84]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #84]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+22] += m[22] * mu\n\t"
        "ldr	r7, [%[m], #88]\n\t"
        "ldr	r9, [%[a], #88]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #88]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+23] += m[23] * mu\n\t"
        "ldr	r7, [%[m], #92]\n\t"
        "ldr	r9, [%[a], #92]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #92]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+24] += m[24] * mu\n\t"
        "ldr	r7, [%[m], #96]\n\t"
        "ldr	r9, [%[a], #96]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #96]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+25] += m[25] * mu\n\t"
        "ldr	r7, [%[m], #100]\n\t"
        "ldr	r9, [%[a], #100]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #100]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+26] += m[26] * mu\n\t"
        "ldr	r7, [%[m], #104]\n\t"
        "ldr	r9, [%[a], #104]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #104]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+27] += m[27] * mu\n\t"
        "ldr	r7, [%[m], #108]\n\t"
        "ldr	r9, [%[a], #108]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #108]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+28] += m[28] * mu\n\t"
        "ldr	r7, [%[m], #112]\n\t"
        "ldr	r9, [%[a], #112]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #112]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+29] += m[29] * mu\n\t"
        "ldr	r7, [%[m], #116]\n\t"
        "ldr	r9, [%[a], #116]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #116]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+30] += m[30] * mu\n\t"
        "ldr	r7, [%[m], #120]\n\t"
        "ldr	r9, [%[a], #120]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #120]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+31] += m[31] * mu\n\t"
        "ldr	r7, [%[m], #124]\n\t"
        "ldr	r9, [%[a], #124]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #124]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+32] += m[32] * mu\n\t"
        "ldr	r7, [%[m], #128]\n\t"
        "ldr	r9, [%[a], #128]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #128]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+33] += m[33] * mu\n\t"
        "ldr	r7, [%[m], #132]\n\t"
        "ldr	r9, [%[a], #132]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #132]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+34] += m[34] * mu\n\t"
        "ldr	r7, [%[m], #136]\n\t"
        "ldr	r9, [%[a], #136]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #136]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+35] += m[35] * mu\n\t"
        "ldr	r7, [%[m], #140]\n\t"
        "ldr	r9, [%[a], #140]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #140]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+36] += m[36] * mu\n\t"
        "ldr	r7, [%[m], #144]\n\t"
        "ldr	r9, [%[a], #144]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #144]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+37] += m[37] * mu\n\t"
        "ldr	r7, [%[m], #148]\n\t"
        "ldr	r9, [%[a], #148]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #148]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+38] += m[38] * mu\n\t"
        "ldr	r7, [%[m], #152]\n\t"
        "ldr	r9, [%[a], #152]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #152]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+39] += m[39] * mu\n\t"
        "ldr	r7, [%[m], #156]\n\t"
        "ldr	r9, [%[a], #156]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #156]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+40] += m[40] * mu\n\t"
        "ldr	r7, [%[m], #160]\n\t"
        "ldr	r9, [%[a], #160]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #160]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+41] += m[41] * mu\n\t"
        "ldr	r7, [%[m], #164]\n\t"
        "ldr	r9, [%[a], #164]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #164]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+42] += m[42] * mu\n\t"
        "ldr	r7, [%[m], #168]\n\t"
        "ldr	r9, [%[a], #168]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #168]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+43] += m[43] * mu\n\t"
        "ldr	r7, [%[m], #172]\n\t"
        "ldr	r9, [%[a], #172]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #172]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+44] += m[44] * mu\n\t"
        "ldr	r7, [%[m], #176]\n\t"
        "ldr	r9, [%[a], #176]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #176]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+45] += m[45] * mu\n\t"
        "ldr	r7, [%[m], #180]\n\t"
        "ldr	r9, [%[a], #180]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #180]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+46] += m[46] * mu\n\t"
        "ldr	r7, [%[m], #184]\n\t"
        "ldr	r9, [%[a], #184]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #184]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+47] += m[47] * mu\n\t"
        "ldr	r7, [%[m], #188]\n\t"
        "ldr	r9, [%[a], #188]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #188]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+48] += m[48] * mu\n\t"
        "ldr	r7, [%[m], #192]\n\t"
        "ldr	r9, [%[a], #192]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #192]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+49] += m[49] * mu\n\t"
        "ldr	r7, [%[m], #196]\n\t"
        "ldr	r9, [%[a], #196]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #196]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+50] += m[50] * mu\n\t"
        "ldr	r7, [%[m], #200]\n\t"
        "ldr	r9, [%[a], #200]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #200]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+51] += m[51] * mu\n\t"
        "ldr	r7, [%[m], #204]\n\t"
        "ldr	r9, [%[a], #204]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #204]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+52] += m[52] * mu\n\t"
        "ldr	r7, [%[m], #208]\n\t"
        "ldr	r9, [%[a], #208]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #208]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+53] += m[53] * mu\n\t"
        "ldr	r7, [%[m], #212]\n\t"
        "ldr	r9, [%[a], #212]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #212]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+54] += m[54] * mu\n\t"
        "ldr	r7, [%[m], #216]\n\t"
        "ldr	r9, [%[a], #216]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #216]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+55] += m[55] * mu\n\t"
        "ldr	r7, [%[m], #220]\n\t"
        "ldr	r9, [%[a], #220]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #220]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+56] += m[56] * mu\n\t"
        "ldr	r7, [%[m], #224]\n\t"
        "ldr	r9, [%[a], #224]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #224]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+57] += m[57] * mu\n\t"
        "ldr	r7, [%[m], #228]\n\t"
        "ldr	r9, [%[a], #228]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #228]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+58] += m[58] * mu\n\t"
        "ldr	r7, [%[m], #232]\n\t"
        "ldr	r9, [%[a], #232]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #232]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+59] += m[59] * mu\n\t"
        "ldr	r7, [%[m], #236]\n\t"
        "ldr	r9, [%[a], #236]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #236]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+60] += m[60] * mu\n\t"
        "ldr	r7, [%[m], #240]\n\t"
        "ldr	r9, [%[a], #240]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #240]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+61] += m[61] * mu\n\t"
        "ldr	r7, [%[m], #244]\n\t"
        "ldr	r9, [%[a], #244]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #244]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+62] += m[62] * mu\n\t"
        "ldr	r7, [%[m], #248]\n\t"
        "ldr	r9, [%[a], #248]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #248]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+63] += m[63] * mu\n\t"
        "ldr	r7, [%[m], #252]\n\t"
        "ldr   r9, [%[a], #252]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r7, r7, %[ca]\n\t"
        "mov	%[ca], #0\n\t"
        "adc	%[ca], %[ca], %[ca]\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #252]\n\t"
        "ldr	r9, [%[a], #256]\n\t"
        "adcs	r9, r9, r7\n\t"
        "str	r9, [%[a], #256]\n\t"
        "adc	%[ca], %[ca], #0\n\t"
        "# i += 1\n\t"
        "add	%[a], %[a], #4\n\t"
        "add	r12, r12, #4\n\t"
        "cmp	r12, #256\n\t"
        "blt	1b\n\t"
        "str	r10, [%[a], #0]\n\t"
        "str	r14, [%[a], #4]\n\t"
        : [ca] "+r" (ca), [a] "+r" (a)
        : [m] "r" (m), [mp] "r" (mp)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    sp_2048_cond_sub_64(a - 64, a, m, (sp_digit)0 - ca);
}

/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_mul_64(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_64(r, a, b);
    sp_2048_mont_reduce_64(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_64(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_64(r, a);
    sp_2048_mont_reduce_64(r, m, mp);
}

/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
static void sp_2048_mul_d_64(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r5, r3, %[b], r8\n\t"
        "mov	r4, #0\n\t"
        "str	r5, [%[r]]\n\t"
        "mov	r5, #0\n\t"
        "mov	r9, #4\n\t"
        "1:\n\t"
        "ldr	r8, [%[a], r9]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], r9]\n\t"
        "mov	r3, r4\n\t"
        "mov	r4, r5\n\t"
        "mov	r5, #0\n\t"
        "add	r9, r9, #4\n\t"
        "cmp	r9, #256\n\t"
        "blt	1b\n\t"
        "str	r3, [%[r], #256]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#else
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r3, r4, %[b], r8\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[r]]\n\t"
        "# A[1] * B\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #4]\n\t"
        "# A[2] * B\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #8]\n\t"
        "# A[3] * B\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #12]\n\t"
        "# A[4] * B\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #16]\n\t"
        "# A[5] * B\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #20]\n\t"
        "# A[6] * B\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #24]\n\t"
        "# A[7] * B\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #28]\n\t"
        "# A[8] * B\n\t"
        "ldr	r8, [%[a], #32]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "# A[9] * B\n\t"
        "ldr	r8, [%[a], #36]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "# A[10] * B\n\t"
        "ldr	r8, [%[a], #40]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "# A[11] * B\n\t"
        "ldr	r8, [%[a], #44]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "# A[12] * B\n\t"
        "ldr	r8, [%[a], #48]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "# A[13] * B\n\t"
        "ldr	r8, [%[a], #52]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "# A[14] * B\n\t"
        "ldr	r8, [%[a], #56]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #56]\n\t"
        "# A[15] * B\n\t"
        "ldr	r8, [%[a], #60]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #60]\n\t"
        "# A[16] * B\n\t"
        "ldr	r8, [%[a], #64]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #64]\n\t"
        "# A[17] * B\n\t"
        "ldr	r8, [%[a], #68]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #68]\n\t"
        "# A[18] * B\n\t"
        "ldr	r8, [%[a], #72]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #72]\n\t"
        "# A[19] * B\n\t"
        "ldr	r8, [%[a], #76]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #76]\n\t"
        "# A[20] * B\n\t"
        "ldr	r8, [%[a], #80]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #80]\n\t"
        "# A[21] * B\n\t"
        "ldr	r8, [%[a], #84]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #84]\n\t"
        "# A[22] * B\n\t"
        "ldr	r8, [%[a], #88]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #88]\n\t"
        "# A[23] * B\n\t"
        "ldr	r8, [%[a], #92]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #92]\n\t"
        "# A[24] * B\n\t"
        "ldr	r8, [%[a], #96]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #96]\n\t"
        "# A[25] * B\n\t"
        "ldr	r8, [%[a], #100]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #100]\n\t"
        "# A[26] * B\n\t"
        "ldr	r8, [%[a], #104]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #104]\n\t"
        "# A[27] * B\n\t"
        "ldr	r8, [%[a], #108]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #108]\n\t"
        "# A[28] * B\n\t"
        "ldr	r8, [%[a], #112]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #112]\n\t"
        "# A[29] * B\n\t"
        "ldr	r8, [%[a], #116]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #116]\n\t"
        "# A[30] * B\n\t"
        "ldr	r8, [%[a], #120]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #120]\n\t"
        "# A[31] * B\n\t"
        "ldr	r8, [%[a], #124]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #124]\n\t"
        "# A[32] * B\n\t"
        "ldr	r8, [%[a], #128]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #128]\n\t"
        "# A[33] * B\n\t"
        "ldr	r8, [%[a], #132]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #132]\n\t"
        "# A[34] * B\n\t"
        "ldr	r8, [%[a], #136]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #136]\n\t"
        "# A[35] * B\n\t"
        "ldr	r8, [%[a], #140]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #140]\n\t"
        "# A[36] * B\n\t"
        "ldr	r8, [%[a], #144]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #144]\n\t"
        "# A[37] * B\n\t"
        "ldr	r8, [%[a], #148]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #148]\n\t"
        "# A[38] * B\n\t"
        "ldr	r8, [%[a], #152]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #152]\n\t"
        "# A[39] * B\n\t"
        "ldr	r8, [%[a], #156]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #156]\n\t"
        "# A[40] * B\n\t"
        "ldr	r8, [%[a], #160]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #160]\n\t"
        "# A[41] * B\n\t"
        "ldr	r8, [%[a], #164]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #164]\n\t"
        "# A[42] * B\n\t"
        "ldr	r8, [%[a], #168]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #168]\n\t"
        "# A[43] * B\n\t"
        "ldr	r8, [%[a], #172]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #172]\n\t"
        "# A[44] * B\n\t"
        "ldr	r8, [%[a], #176]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #176]\n\t"
        "# A[45] * B\n\t"
        "ldr	r8, [%[a], #180]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #180]\n\t"
        "# A[46] * B\n\t"
        "ldr	r8, [%[a], #184]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #184]\n\t"
        "# A[47] * B\n\t"
        "ldr	r8, [%[a], #188]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #188]\n\t"
        "# A[48] * B\n\t"
        "ldr	r8, [%[a], #192]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #192]\n\t"
        "# A[49] * B\n\t"
        "ldr	r8, [%[a], #196]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #196]\n\t"
        "# A[50] * B\n\t"
        "ldr	r8, [%[a], #200]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #200]\n\t"
        "# A[51] * B\n\t"
        "ldr	r8, [%[a], #204]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #204]\n\t"
        "# A[52] * B\n\t"
        "ldr	r8, [%[a], #208]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #208]\n\t"
        "# A[53] * B\n\t"
        "ldr	r8, [%[a], #212]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #212]\n\t"
        "# A[54] * B\n\t"
        "ldr	r8, [%[a], #216]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #216]\n\t"
        "# A[55] * B\n\t"
        "ldr	r8, [%[a], #220]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #220]\n\t"
        "# A[56] * B\n\t"
        "ldr	r8, [%[a], #224]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #224]\n\t"
        "# A[57] * B\n\t"
        "ldr	r8, [%[a], #228]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #228]\n\t"
        "# A[58] * B\n\t"
        "ldr	r8, [%[a], #232]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #232]\n\t"
        "# A[59] * B\n\t"
        "ldr	r8, [%[a], #236]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #236]\n\t"
        "# A[60] * B\n\t"
        "ldr	r8, [%[a], #240]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #240]\n\t"
        "# A[61] * B\n\t"
        "ldr	r8, [%[a], #244]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #244]\n\t"
        "# A[62] * B\n\t"
        "ldr	r8, [%[a], #248]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #248]\n\t"
        "# A[63] * B\n\t"
        "ldr	r8, [%[a], #252]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r3, [%[r], #252]\n\t"
        "str	r4, [%[r], #256]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#endif
}

/* Divide the double width number (d1|d0) by the dividend. (d1|d0 / div)
 *
 * d1   The high order half of the number to divide.
 * d0   The low order half of the number to divide.
 * div  The dividend.
 * returns the result of the division.
 *
 * Note that this is an approximate div. It may give an answer 1 larger.
 */
static sp_digit div_2048_word_64(sp_digit d1, sp_digit d0, sp_digit div)
{
    sp_digit r = 0;

    __asm__ __volatile__ (
        "lsr	r5, %[div], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[d0]\n\t"
        "mov	r7, %[d1]\n\t"
        "# Do top 32\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "# Next 30 bits\n\t"
        "mov	r4, #29\n\t"
        "1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r7, r7, r7\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "subs	r8, %[div], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r)
        : [d1] "r" (d1), [d0] "r" (d0), [div] "r" (div)
        : "r4", "r5", "r6", "r7", "r8"
    );
    return r;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_64(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<64; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
#endif
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static int32_t sp_2048_cmp_64(sp_digit* a, sp_digit* b)
{
    sp_digit r = -1;
    sp_digit one = 1;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "mov	r6, #252\n\t"
        "1:\n\t"
        "ldr	r4, [%[a], r6]\n\t"
        "ldr	r5, [%[b], r6]\n\t"
        "and	r4, r4, r3\n\t"
        "and	r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "sub	r6, r6, #4\n\t"
        "bcc	1b\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#else
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "ldr		r4, [%[a], #252]\n\t"
        "ldr		r5, [%[b], #252]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #248]\n\t"
        "ldr		r5, [%[b], #248]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #244]\n\t"
        "ldr		r5, [%[b], #244]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #240]\n\t"
        "ldr		r5, [%[b], #240]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #236]\n\t"
        "ldr		r5, [%[b], #236]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #232]\n\t"
        "ldr		r5, [%[b], #232]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #228]\n\t"
        "ldr		r5, [%[b], #228]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #224]\n\t"
        "ldr		r5, [%[b], #224]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #220]\n\t"
        "ldr		r5, [%[b], #220]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #216]\n\t"
        "ldr		r5, [%[b], #216]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #212]\n\t"
        "ldr		r5, [%[b], #212]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #208]\n\t"
        "ldr		r5, [%[b], #208]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #204]\n\t"
        "ldr		r5, [%[b], #204]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #200]\n\t"
        "ldr		r5, [%[b], #200]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #196]\n\t"
        "ldr		r5, [%[b], #196]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #192]\n\t"
        "ldr		r5, [%[b], #192]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #188]\n\t"
        "ldr		r5, [%[b], #188]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #184]\n\t"
        "ldr		r5, [%[b], #184]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #180]\n\t"
        "ldr		r5, [%[b], #180]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #176]\n\t"
        "ldr		r5, [%[b], #176]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #172]\n\t"
        "ldr		r5, [%[b], #172]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #168]\n\t"
        "ldr		r5, [%[b], #168]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #164]\n\t"
        "ldr		r5, [%[b], #164]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #160]\n\t"
        "ldr		r5, [%[b], #160]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #156]\n\t"
        "ldr		r5, [%[b], #156]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #152]\n\t"
        "ldr		r5, [%[b], #152]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #148]\n\t"
        "ldr		r5, [%[b], #148]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #144]\n\t"
        "ldr		r5, [%[b], #144]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #140]\n\t"
        "ldr		r5, [%[b], #140]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #136]\n\t"
        "ldr		r5, [%[b], #136]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #132]\n\t"
        "ldr		r5, [%[b], #132]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #128]\n\t"
        "ldr		r5, [%[b], #128]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #124]\n\t"
        "ldr		r5, [%[b], #124]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #120]\n\t"
        "ldr		r5, [%[b], #120]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #116]\n\t"
        "ldr		r5, [%[b], #116]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #112]\n\t"
        "ldr		r5, [%[b], #112]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #108]\n\t"
        "ldr		r5, [%[b], #108]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #104]\n\t"
        "ldr		r5, [%[b], #104]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #100]\n\t"
        "ldr		r5, [%[b], #100]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #96]\n\t"
        "ldr		r5, [%[b], #96]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #92]\n\t"
        "ldr		r5, [%[b], #92]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #88]\n\t"
        "ldr		r5, [%[b], #88]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #84]\n\t"
        "ldr		r5, [%[b], #84]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #80]\n\t"
        "ldr		r5, [%[b], #80]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #76]\n\t"
        "ldr		r5, [%[b], #76]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #72]\n\t"
        "ldr		r5, [%[b], #72]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #68]\n\t"
        "ldr		r5, [%[b], #68]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #64]\n\t"
        "ldr		r5, [%[b], #64]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #60]\n\t"
        "ldr		r5, [%[b], #60]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #56]\n\t"
        "ldr		r5, [%[b], #56]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #52]\n\t"
        "ldr		r5, [%[b], #52]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #48]\n\t"
        "ldr		r5, [%[b], #48]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #44]\n\t"
        "ldr		r5, [%[b], #44]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #40]\n\t"
        "ldr		r5, [%[b], #40]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #36]\n\t"
        "ldr		r5, [%[b], #36]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #32]\n\t"
        "ldr		r5, [%[b], #32]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #28]\n\t"
        "ldr		r5, [%[b], #28]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #24]\n\t"
        "ldr		r5, [%[b], #24]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #20]\n\t"
        "ldr		r5, [%[b], #20]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #16]\n\t"
        "ldr		r5, [%[b], #16]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #12]\n\t"
        "ldr		r5, [%[b], #12]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #8]\n\t"
        "ldr		r5, [%[b], #8]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #4]\n\t"
        "ldr		r5, [%[b], #4]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #0]\n\t"
        "ldr		r5, [%[b], #0]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#endif

    return r;
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_div_64(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[128], t2[65];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[63];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 64);
    for (i=63; i>=0; i--) {
        r1 = div_2048_word_64(t1[64 + i], t1[64 + i - 1], div);

        sp_2048_mul_d_64(t2, d, r1);
        t1[64 + i] += sp_2048_sub_in_place_64(&t1[i], t2);
        t1[64 + i] -= t2[64];
        sp_2048_mask_64(t2, d, t1[64 + i]);
        t1[64 + i] += sp_2048_add_64(&t1[i], &t1[i], t2);
        sp_2048_mask_64(t2, d, t1[64 + i]);
        t1[64 + i] += sp_2048_add_64(&t1[i], &t1[i], t2);
    }

    r1 = sp_2048_cmp_64(t1, d) >= 0;
    sp_2048_cond_sub_64(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_mod_64(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_64(a, m, NULL, r);
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_div_64_cond(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[128], t2[65];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[63];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 64);
    for (i=63; i>=0; i--) {
        r1 = div_2048_word_64(t1[64 + i], t1[64 + i - 1], div);

        sp_2048_mul_d_64(t2, d, r1);
        t1[64 + i] += sp_2048_sub_in_place_64(&t1[i], t2);
        t1[64 + i] -= t2[64];
        if (t1[64 + i] != 0) {
            t1[64 + i] += sp_2048_add_64(&t1[i], &t1[i], d);
            if (t1[64 + i] != 0)
                t1[64 + i] += sp_2048_add_64(&t1[i], &t1[i], d);
        }
    }

    r1 = sp_2048_cmp_64(t1, d) >= 0;
    sp_2048_cond_sub_64(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_2048_mod_64_cond(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_64_cond(a, m, NULL, r);
}

#if defined(SP_RSA_PRIVATE_EXP_D) || defined(WOLFSSL_HAVE_SP_DH)
#ifdef WOLFSSL_SP_SMALL
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_64(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[16][128];
#else
    sp_digit* t[16];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 16 * 128, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<16; i++)
            t[i] = td + i * 128;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_64(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 64);
        if (reduceA) {
            err = sp_2048_mod_64(t[1] + 64, a, m);
            if (err == MP_OKAY)
                err = sp_2048_mod_64(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 64, a, sizeof(sp_digit) * 64);
            err = sp_2048_mod_64(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_64(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_64(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_64(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_64(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_64(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_64(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_64(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_64(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_64(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_64(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_64(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_64(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_64(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_64(t[15], t[ 8], t[ 7], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 28;
        n <<= 4;
        c = 28;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 64);
        for (; i>=0 || c>=4; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 28;
                n <<= 4;
                c = 28;
            }
            else if (c < 4) {
                y = n >> 28;
                n = e[i--];
                c = 4 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 28) & 0xf;
                n <<= 4;
                c -= 4;
            }

            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);

            sp_2048_mont_mul_64(r, r, t[y], m, mp);
        }

        XMEMSET(&r[64], 0, sizeof(sp_digit) * 64);
        sp_2048_mont_reduce_64(r, m, mp);

        mask = 0 - (sp_2048_cmp_64(r, m) >= 0);
        sp_2048_cond_sub_64(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#else
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_64(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][128];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 128, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 128;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_64(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 64);
        if (reduceA) {
            err = sp_2048_mod_64(t[1] + 64, a, m);
            if (err == MP_OKAY)
                err = sp_2048_mod_64(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 64, a, sizeof(sp_digit) * 64);
            err = sp_2048_mod_64(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_64(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_64(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_64(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_64(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_64(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_64(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_64(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_64(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_64(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_64(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_64(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_64(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_64(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_64(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_64(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_64(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_64(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_64(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_64(t[20], t[10], m, mp);
        sp_2048_mont_mul_64(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_64(t[22], t[11], m, mp);
        sp_2048_mont_mul_64(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_64(t[24], t[12], m, mp);
        sp_2048_mont_mul_64(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_64(t[26], t[13], m, mp);
        sp_2048_mont_mul_64(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_64(t[28], t[14], m, mp);
        sp_2048_mont_mul_64(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_64(t[30], t[15], m, mp);
        sp_2048_mont_mul_64(t[31], t[16], t[15], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 27;
        n <<= 5;
        c = 27;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 64);
        for (; i>=0 || c>=5; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 27;
                n <<= 5;
                c = 27;
            }
            else if (c < 5) {
                y = n >> 27;
                n = e[i--];
                c = 5 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 27) & 0x1f;
                n <<= 5;
                c -= 5;
            }

            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);
            sp_2048_mont_sqr_64(r, r, m, mp);

            sp_2048_mont_mul_64(r, r, t[y], m, mp);
        }
        y = e[0] & 0x7;
        sp_2048_mont_sqr_64(r, r, m, mp);
        sp_2048_mont_sqr_64(r, r, m, mp);
        sp_2048_mont_sqr_64(r, r, m, mp);
        sp_2048_mont_mul_64(r, r, t[y], m, mp);

        XMEMSET(&r[64], 0, sizeof(sp_digit) * 64);
        sp_2048_mont_reduce_64(r, m, mp);

        mask = 0 - (sp_2048_cmp_64(r, m) >= 0);
        sp_2048_cond_sub_64(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#endif /* WOLFSSL_SP_SMALL */
#endif /* SP_RSA_PRIVATE_EXP_D || WOLFSSL_HAVE_SP_DH */

#ifdef WOLFSSL_HAVE_SP_RSA
/* RSA public key operation.
 *
 * in      Array of bytes representing the number to exponentiate, base.
 * inLen   Number of bytes in base.
 * em      Public exponent.
 * mm      Modulus.
 * out     Buffer to hold big-endian bytes of exponentiation result.
 *         Must be at least 256 bytes long.
 * outLen  Number of bytes in result.
 * returns 0 on success, MP_TO_E when the outLen is too small, MP_READ_E when
 * an array is too long and MEMORY_E when dynamic memory allocation fails.
 */
int sp_RsaPublic_2048(const byte* in, word32 inLen, mp_int* em, mp_int* mm,
    byte* out, word32* outLen)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[128], md[64], rd[128];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* a;
    sp_digit *ah;
    sp_digit* m;
    sp_digit* r;
    sp_digit e[1];
    int err = MP_OKAY;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 32 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 64 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 64 * 2;
        m = r + 64 * 2;
        ah = a + 64;
    }
#else
    a = ad;
    m = md;
    r = rd;
    ah = a + 64;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_bin(ah, 64, in, inLen);
#if DIGIT_BIT >= 32
        e[0] = em->dp[0];
#else
        e[0] = em->dp[0];
        if (em->used > 1)
            e[0] |= ((sp_digit)em->dp[1]) << DIGIT_BIT;
#endif
        if (e[0] == 0)
            err = MP_EXPTMOD_E;
    }
    if (err == MP_OKAY) {
        sp_2048_from_mp(m, 64, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_2048_sqr_64(r, ah);
                err = sp_2048_mod_64_cond(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_2048_mul_64(r, ah, r);
                err = sp_2048_mod_64_cond(r, r, m);
            }
        }
        else {
            int i;
            sp_digit mp;

            sp_2048_mont_setup(m, &mp);

            /* Convert to Montgomery form. */
            XMEMSET(a, 0, sizeof(sp_digit) * 64);
            err = sp_2048_mod_64_cond(a, a, m);

            if (err == MP_OKAY) {
                for (i=31; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 64);
                for (i--; i>=0; i--) {
                    sp_2048_mont_sqr_64(r, r, m, mp);
                    if (((e[0] >> i) & 1) == 1)
                        sp_2048_mont_mul_64(r, r, a, m, mp);
                }
                XMEMSET(&r[64], 0, sizeof(sp_digit) * 64);
                sp_2048_mont_reduce_64(r, m, mp);

                for (i = 63; i > 0; i--) {
                    if (r[i] != m[i])
                        break;
                }
                if (r[i] >= m[i])
                    sp_2048_sub_in_place_64(r, m);
            }
        }
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* RSA private key operation.
 *
 * in      Array of bytes representing the number to exponentiate, base.
 * inLen   Number of bytes in base.
 * dm      Private exponent.
 * pm      First prime.
 * qm      Second prime.
 * dpm     First prime's CRT exponent.
 * dqm     Second prime's CRT exponent.
 * qim     Inverse of second prime mod p.
 * mm      Modulus.
 * out     Buffer to hold big-endian bytes of exponentiation result.
 *         Must be at least 256 bytes long.
 * outLen  Number of bytes in result.
 * returns 0 on success, MP_TO_E when the outLen is too small, MP_READ_E when
 * an array is too long and MEMORY_E when dynamic memory allocation fails.
 */
int sp_RsaPrivate_2048(const byte* in, word32 inLen, mp_int* dm,
    mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim, mp_int* mm,
    byte* out, word32* outLen)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[64 * 2];
    sp_digit pd[32], qd[32], dpd[32];
    sp_digit tmpad[64], tmpbd[64];
#else
    sp_digit* t = NULL;
#endif
    sp_digit* a;
    sp_digit* p;
    sp_digit* q;
    sp_digit* dp;
    sp_digit* dq;
    sp_digit* qi;
    sp_digit* tmp;
    sp_digit* tmpa;
    sp_digit* tmpb;
    sp_digit* r;
    sp_digit c;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 256 || mp_count_bits(mm) != 2048))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 64 * 2;
        q = p + 32;
        qi = dq = dp = q + 32;
        tmpa = qi + 32;
        tmpb = tmpa + 64;

        tmp = t;
        r = tmp + 64;
    }
#else
    r = a = ad;
    p = pd;
    q = qd;
    qi = dq = dp = dpd;
    tmpa = tmpad;
    tmpb = tmpbd;
    tmp = a + 64;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 64, in, inLen);
        sp_2048_from_mp(p, 32, pm);
        sp_2048_from_mp(q, 32, qm);
        sp_2048_from_mp(dp, 32, dpm);

        err = sp_2048_mod_exp_32(tmpa, a, dp, 1024, p, 1);
    }
    if (err == MP_OKAY) {
        sp_2048_from_mp(dq, 32, dqm);
        err = sp_2048_mod_exp_32(tmpb, a, dq, 1024, q, 1);
    }

    if (err == MP_OKAY) {
        c = sp_2048_sub_in_place_32(tmpa, tmpb);
        sp_2048_mask_32(tmp, p, c);
        sp_2048_add_32(tmpa, tmpa, tmp);

        sp_2048_from_mp(qi, 32, qim);
        sp_2048_mul_32(tmpa, tmpa, qi);
        err = sp_2048_mod_32(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_2048_mul_32(tmpa, q, tmpa);
        XMEMSET(&tmpb[32], 0, sizeof(sp_digit) * 32);
        sp_2048_add_64(r, tmpb, tmpa);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 32 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else
    XMEMSET(tmpad, 0, sizeof(tmpad));
    XMEMSET(tmpbd, 0, sizeof(tmpbd));
    XMEMSET(pd, 0, sizeof(pd));
    XMEMSET(qd, 0, sizeof(qd));
    XMEMSET(dpd, 0, sizeof(dpd));
#endif

    return err;
}
#endif /* WOLFSSL_HAVE_SP_RSA */
#ifdef WOLFSSL_HAVE_SP_DH
/* Convert an array of sp_digit to an mp_int.
 *
 * a  A single precision integer.
 * r  A multi-precision integer.
 */
static int sp_2048_to_mp(sp_digit* a, mp_int* r)
{
    int err;

    err = mp_grow(r, (2048 + DIGIT_BIT - 1) / DIGIT_BIT);
    if (err == MP_OKAY) {
#if DIGIT_BIT == 32
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 64);
        r->used = 64;
        mp_clamp(r);
#elif DIGIT_BIT < 32
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 64; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 32) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 32 - s;
        }
        r->used = (2048 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 64; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 32 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 32 - s;
            }
            else
                s += 32;
        }
        r->used = (2048 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#endif
    }

    return err;
}

/* Perform the modular exponentiation for Diffie-Hellman.
 *
 * base  Base. MP integer.
 * exp   Exponent. MP integer.
 * mod   Modulus. MP integer.
 * res   Result. MP integer.
 * returs 0 on success, MP_READ_E if there are too many bytes in an array
 * and MEMORY_E if memory allocation fails.
 */
int sp_ModExp_2048(mp_int* base, mp_int* exp, mp_int* mod, mp_int* res)
{
    int err = MP_OKAY;
    sp_digit b[128], e[64], m[64];
    sp_digit* r = b;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 2048 || expBits > 2048 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 64, base);
        sp_2048_from_mp(e, 64, exp);
        sp_2048_from_mp(m, 64, mod);

        err = sp_2048_mod_exp_64(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_2048_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(e));

    return err;
}

/* Perform the modular exponentiation for Diffie-Hellman.
 *
 * base     Base.
 * exp      Array of bytes that is the exponent.
 * expLen   Length of data, in bytes, in exponent.
 * mod      Modulus.
 * out      Buffer to hold big-endian bytes of exponentiation result.
 *          Must be at least 256 bytes long.
 * outLen   Length, in bytes, of exponentiation result.
 * returs 0 on success, MP_READ_E if there are too many bytes in an array
 * and MEMORY_E if memory allocation fails.
 */
int sp_DhExp_2048(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen)
{
    int err = MP_OKAY;
    sp_digit b[128], e[64], m[64];
    sp_digit* r = b;
    word32 i;

    if (mp_count_bits(base) > 2048 || expLen > 256 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 64, base);
        sp_2048_from_bin(e, 64, exp, expLen);
        sp_2048_from_mp(m, 64, mod);

        err = sp_2048_mod_exp_64(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
        for (i=0; i<256 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);

    }

    XMEMSET(e, 0, sizeof(e));

    return err;
}

#endif /* WOLFSSL_HAVE_SP_DH */

#endif /* WOLFSSL_SP_NO_2048 */

#ifndef WOLFSSL_SP_NO_3072
/* Read big endian unsigned byte aray into r.
 *
 * r  A single precision integer.
 * a  Byte array.
 * n  Number of bytes in array to read.
 */
static void sp_3072_from_bin(sp_digit* r, int max, const byte* a, int n)
{
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = n-1; i >= 0; i--) {
        r[j] |= ((sp_digit)a[i]) << s;
        if (s >= 24) {
            r[j] &= 0xffffffff;
            s = 32 - s;
            if (j + 1 >= max)
                break;
            r[++j] = a[i] >> s;
            s = 8 - s;
        }
        else
            s += 8;
    }

    for (j++; j < max; j++)
        r[j] = 0;
}

/* Convert an mp_int to an array of sp_digit.
 *
 * r  A single precision integer.
 * a  A multi-precision integer.
 */
static void sp_3072_from_mp(sp_digit* r, int max, mp_int* a)
{
#if DIGIT_BIT == 32
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 32
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0xffffffff;
        s = 32 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 32 <= DIGIT_BIT) {
            s += 32;
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            if (s < DIGIT_BIT)
                r[++j] = a->dp[i] >> s;
            else
                r[++j] = 0;
        }
        s = DIGIT_BIT - s;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#else
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= ((sp_digit)a->dp[i]) << s;
        if (s + DIGIT_BIT >= 32) {
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            s = 32 - s;
            if (s == DIGIT_BIT) {
                r[++j] = 0;
                s = 0;
            }
            else {
                r[++j] = a->dp[i] >> s;
                s = DIGIT_BIT - s;
            }
        }
        else
            s += DIGIT_BIT;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#endif
}

/* Write r as big endian to byte aray.
 * Fixed length number of bytes written: 384
 *
 * r  A single precision integer.
 * a  Byte array.
 */
static void sp_3072_to_bin(sp_digit* r, byte* a)
{
    int i, j, s = 0, b;

    j = 3072 / 8 - 1;
    a[j] = 0;
    for (i=0; i<96 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 32) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 32);
        if (j >= 0)
            a[j] = 0;
        if (s != 0)
            j++;
    }
}

#ifndef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_3072_mul_8(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "#  A[0] * B[0]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r3, r4, r8, r9\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[tmp]]\n\t"
        "#  A[0] * B[1]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[0]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #4]\n\t"
        "#  A[0] * B[2]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[1]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[0]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #8]\n\t"
        "#  A[0] * B[3]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[2]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[1]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[0]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #12]\n\t"
        "#  A[0] * B[4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[3]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[2]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[1]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[0]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #16]\n\t"
        "#  A[0] * B[5]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[4]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[3]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[2]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[1]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[0]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #20]\n\t"
        "#  A[0] * B[6]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[5]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[4]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[3]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[2]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[1]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[0]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #24]\n\t"
        "#  A[0] * B[7]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[6]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[5]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[4]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[3]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[2]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[1]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[0]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #28]\n\t"
        "#  A[1] * B[7]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[2] * B[6]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[5]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[4]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[3]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[2]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[1]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "#  A[2] * B[7]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[3] * B[6]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[5]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[4]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[3]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[2]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "#  A[3] * B[7]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[4] * B[6]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[5]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[4]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[3]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "#  A[4] * B[7]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[5] * B[6]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[5]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[4]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "#  A[5] * B[7]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[6] * B[6]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[5]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "#  A[6] * B[7]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[7] * B[6]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "#  A[7] * B[7]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r5, [%[r], #56]\n\t"
        "str	r3, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [tmp] "r" (tmp)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_3072_sqr_8(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "#  A[0] * A[0]\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "umull	r8, r3, r10, r10\n\t"
        "mov	r4, #0\n\t"
        "str	r8, [%[tmp]]\n\t"
        "#  A[0] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #4]\n\t"
        "#  A[0] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[1] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[tmp], #8]\n\t"
        "#  A[0] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[1] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[tmp], #12]\n\t"
        "#  A[0] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[1] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[2] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #16]\n\t"
        "#  A[0] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[tmp], #20]\n\t"
        "#  A[0] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[tmp], #24]\n\t"
        "#  A[0] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r2, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r3, r3, r5\n\t"
        "adcs	r4, r4, r6\n\t"
        "adc	r2, r2, r7\n\t"
        "str	r3, [%[tmp], #28]\n\t"
        "#  A[1] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[2] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "#  A[2] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[3] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[r], #36]\n\t"
        "#  A[3] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[4] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[5] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #40]\n\t"
        "#  A[4] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[5] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[r], #44]\n\t"
        "#  A[5] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[6] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[r], #48]\n\t"
        "#  A[6] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #52]\n\t"
        "#  A[7] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r2, r2, r9\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r2, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [tmp] "r" (tmp)
        : "memory", "r2", "r3", "r4", "r8", "r9", "r10", "r8", "r5", "r6", "r7", "r14"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_in_place_16(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_16(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_3072_mask_8(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<8; i++)
        r[i] = a[i] & m;
#else
    r[0] = a[0] & m;
    r[1] = a[1] & m;
    r[2] = a[2] & m;
    r[3] = a[3] & m;
    r[4] = a[4] & m;
    r[5] = a[5] & m;
    r[6] = a[6] & m;
    r[7] = a[7] & m;
#endif
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_3072_mul_16(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[16];
    sp_digit a1[8];
    sp_digit b1[8];
    sp_digit z2[16];
    sp_digit u, ca, cb;

    ca = sp_3072_add_8(a1, a, &a[8]);
    cb = sp_3072_add_8(b1, b, &b[8]);
    u  = ca & cb;
    sp_3072_mul_8(z1, a1, b1);
    sp_3072_mul_8(z2, &a[8], &b[8]);
    sp_3072_mul_8(z0, a, b);
    sp_3072_mask_8(r + 16, a1, 0 - cb);
    sp_3072_mask_8(b1, b1, 0 - ca);
    u += sp_3072_add_8(r + 16, r + 16, b1);
    u += sp_3072_sub_in_place_16(z1, z2);
    u += sp_3072_sub_in_place_16(z1, z0);
    u += sp_3072_add_16(r + 8, r + 8, z1);
    r[24] = u;
    XMEMSET(r + 24 + 1, 0, sizeof(sp_digit) * (8 - 1));
    sp_3072_add_16(r + 16, r + 16, z2);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_3072_sqr_16(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z2[16];
    sp_digit z1[16];
    sp_digit a1[8];
    sp_digit u;

    u = sp_3072_add_8(a1, a, &a[8]);
    sp_3072_sqr_8(z1, a1);
    sp_3072_sqr_8(z2, &a[8]);
    sp_3072_sqr_8(z0, a);
    sp_3072_mask_8(r + 16, a1, 0 - u);
    u += sp_3072_add_8(r + 16, r + 16, r + 16);
    u += sp_3072_sub_in_place_16(z1, z2);
    u += sp_3072_sub_in_place_16(z1, z0);
    u += sp_3072_add_16(r + 8, r + 8, z1);
    r[24] = u;
    XMEMSET(r + 24 + 1, 0, sizeof(sp_digit) * (8 - 1));
    sp_3072_add_16(r + 16, r + 16, z2);
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_32(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r3, [%[a], #0]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "ldr	r8, [%[b], #4]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "ldr	r10, [%[b], #12]\n\t"
        "subs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #0]\n\t"
        "str	r4, [%[r], #4]\n\t"
        "str	r5, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r3, [%[a], #16]\n\t"
        "ldr	r4, [%[a], #20]\n\t"
        "ldr	r5, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "ldr	r8, [%[b], #20]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "ldr	r10, [%[b], #28]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #16]\n\t"
        "str	r4, [%[r], #20]\n\t"
        "str	r5, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "ldr	r3, [%[a], #32]\n\t"
        "ldr	r4, [%[a], #36]\n\t"
        "ldr	r5, [%[a], #40]\n\t"
        "ldr	r6, [%[a], #44]\n\t"
        "ldr	r7, [%[b], #32]\n\t"
        "ldr	r8, [%[b], #36]\n\t"
        "ldr	r9, [%[b], #40]\n\t"
        "ldr	r10, [%[b], #44]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #32]\n\t"
        "str	r4, [%[r], #36]\n\t"
        "str	r5, [%[r], #40]\n\t"
        "str	r6, [%[r], #44]\n\t"
        "ldr	r3, [%[a], #48]\n\t"
        "ldr	r4, [%[a], #52]\n\t"
        "ldr	r5, [%[a], #56]\n\t"
        "ldr	r6, [%[a], #60]\n\t"
        "ldr	r7, [%[b], #48]\n\t"
        "ldr	r8, [%[b], #52]\n\t"
        "ldr	r9, [%[b], #56]\n\t"
        "ldr	r10, [%[b], #60]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "str	r4, [%[r], #52]\n\t"
        "str	r5, [%[r], #56]\n\t"
        "str	r6, [%[r], #60]\n\t"
        "ldr	r3, [%[a], #64]\n\t"
        "ldr	r4, [%[a], #68]\n\t"
        "ldr	r5, [%[a], #72]\n\t"
        "ldr	r6, [%[a], #76]\n\t"
        "ldr	r7, [%[b], #64]\n\t"
        "ldr	r8, [%[b], #68]\n\t"
        "ldr	r9, [%[b], #72]\n\t"
        "ldr	r10, [%[b], #76]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #64]\n\t"
        "str	r4, [%[r], #68]\n\t"
        "str	r5, [%[r], #72]\n\t"
        "str	r6, [%[r], #76]\n\t"
        "ldr	r3, [%[a], #80]\n\t"
        "ldr	r4, [%[a], #84]\n\t"
        "ldr	r5, [%[a], #88]\n\t"
        "ldr	r6, [%[a], #92]\n\t"
        "ldr	r7, [%[b], #80]\n\t"
        "ldr	r8, [%[b], #84]\n\t"
        "ldr	r9, [%[b], #88]\n\t"
        "ldr	r10, [%[b], #92]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #80]\n\t"
        "str	r4, [%[r], #84]\n\t"
        "str	r5, [%[r], #88]\n\t"
        "str	r6, [%[r], #92]\n\t"
        "ldr	r3, [%[a], #96]\n\t"
        "ldr	r4, [%[a], #100]\n\t"
        "ldr	r5, [%[a], #104]\n\t"
        "ldr	r6, [%[a], #108]\n\t"
        "ldr	r7, [%[b], #96]\n\t"
        "ldr	r8, [%[b], #100]\n\t"
        "ldr	r9, [%[b], #104]\n\t"
        "ldr	r10, [%[b], #108]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #96]\n\t"
        "str	r4, [%[r], #100]\n\t"
        "str	r5, [%[r], #104]\n\t"
        "str	r6, [%[r], #108]\n\t"
        "ldr	r3, [%[a], #112]\n\t"
        "ldr	r4, [%[a], #116]\n\t"
        "ldr	r5, [%[a], #120]\n\t"
        "ldr	r6, [%[a], #124]\n\t"
        "ldr	r7, [%[b], #112]\n\t"
        "ldr	r8, [%[b], #116]\n\t"
        "ldr	r9, [%[b], #120]\n\t"
        "ldr	r10, [%[b], #124]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #112]\n\t"
        "str	r4, [%[r], #116]\n\t"
        "str	r5, [%[r], #120]\n\t"
        "str	r6, [%[r], #124]\n\t"
        "sbc	%[c], %[c], #0\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_32(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r5, [%[a], #68]\n\t"
        "ldr	r6, [%[a], #72]\n\t"
        "ldr	r7, [%[a], #76]\n\t"
        "ldr	r8, [%[b], #64]\n\t"
        "ldr	r9, [%[b], #68]\n\t"
        "ldr	r10, [%[b], #72]\n\t"
        "ldr	r14, [%[b], #76]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r5, [%[r], #68]\n\t"
        "str	r6, [%[r], #72]\n\t"
        "str	r7, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r5, [%[a], #84]\n\t"
        "ldr	r6, [%[a], #88]\n\t"
        "ldr	r7, [%[a], #92]\n\t"
        "ldr	r8, [%[b], #80]\n\t"
        "ldr	r9, [%[b], #84]\n\t"
        "ldr	r10, [%[b], #88]\n\t"
        "ldr	r14, [%[b], #92]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r5, [%[r], #84]\n\t"
        "str	r6, [%[r], #88]\n\t"
        "str	r7, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r5, [%[a], #100]\n\t"
        "ldr	r6, [%[a], #104]\n\t"
        "ldr	r7, [%[a], #108]\n\t"
        "ldr	r8, [%[b], #96]\n\t"
        "ldr	r9, [%[b], #100]\n\t"
        "ldr	r10, [%[b], #104]\n\t"
        "ldr	r14, [%[b], #108]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r5, [%[r], #100]\n\t"
        "str	r6, [%[r], #104]\n\t"
        "str	r7, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r5, [%[a], #116]\n\t"
        "ldr	r6, [%[a], #120]\n\t"
        "ldr	r7, [%[a], #124]\n\t"
        "ldr	r8, [%[b], #112]\n\t"
        "ldr	r9, [%[b], #116]\n\t"
        "ldr	r10, [%[b], #120]\n\t"
        "ldr	r14, [%[b], #124]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r5, [%[r], #116]\n\t"
        "str	r6, [%[r], #120]\n\t"
        "str	r7, [%[r], #124]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_48(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit p0[32];
    sp_digit p1[32];
    sp_digit p2[32];
    sp_digit p3[32];
    sp_digit p4[32];
    sp_digit p5[32];
    sp_digit t0[32];
    sp_digit t1[32];
    sp_digit t2[32];
    sp_digit a0[16];
    sp_digit a1[16];
    sp_digit a2[16];
    sp_digit b0[16];
    sp_digit b1[16];
    sp_digit b2[16];
    sp_3072_add_16(a0, a, &a[16]);
    sp_3072_add_16(b0, b, &b[16]);
    sp_3072_add_16(a1, &a[16], &a[32]);
    sp_3072_add_16(b1, &b[16], &b[32]);
    sp_3072_add_16(a2, a0, &a[32]);
    sp_3072_add_16(b2, b0, &b[32]);
    sp_3072_mul_16(p0, a, b);
    sp_3072_mul_16(p2, &a[16], &b[16]);
    sp_3072_mul_16(p4, &a[32], &b[32]);
    sp_3072_mul_16(p1, a0, b0);
    sp_3072_mul_16(p3, a1, b1);
    sp_3072_mul_16(p5, a2, b2);
    XMEMSET(r, 0, sizeof(*r)*2*48);
    sp_3072_sub_32(t0, p3, p2);
    sp_3072_sub_32(t1, p1, p2);
    sp_3072_sub_32(t2, p5, t0);
    sp_3072_sub_32(t2, t2, t1);
    sp_3072_sub_32(t0, t0, p4);
    sp_3072_sub_32(t1, t1, p0);
    sp_3072_add_32(r, r, p0);
    sp_3072_add_32(&r[16], &r[16], t1);
    sp_3072_add_32(&r[32], &r[32], t2);
    sp_3072_add_32(&r[48], &r[48], t0);
    sp_3072_add_32(&r[64], &r[64], p4);
}

/* Square a into r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_48(sp_digit* r, const sp_digit* a)
{
    sp_digit p0[32];
    sp_digit p1[32];
    sp_digit p2[32];
    sp_digit p3[32];
    sp_digit p4[32];
    sp_digit p5[32];
    sp_digit t0[32];
    sp_digit t1[32];
    sp_digit t2[32];
    sp_digit a0[16];
    sp_digit a1[16];
    sp_digit a2[16];
    sp_3072_add_16(a0, a, &a[16]);
    sp_3072_add_16(a1, &a[16], &a[32]);
    sp_3072_add_16(a2, a0, &a[32]);
    sp_3072_sqr_16(p0, a);
    sp_3072_sqr_16(p2, &a[16]);
    sp_3072_sqr_16(p4, &a[32]);
    sp_3072_sqr_16(p1, a0);
    sp_3072_sqr_16(p3, a1);
    sp_3072_sqr_16(p5, a2);
    XMEMSET(r, 0, sizeof(*r)*2*48);
    sp_3072_sub_32(t0, p3, p2);
    sp_3072_sub_32(t1, p1, p2);
    sp_3072_sub_32(t2, p5, t0);
    sp_3072_sub_32(t2, t2, t1);
    sp_3072_sub_32(t0, t0, p4);
    sp_3072_sub_32(t1, t1, p0);
    sp_3072_add_32(r, r, p0);
    sp_3072_add_32(&r[16], &r[16], t1);
    sp_3072_add_32(&r[32], &r[32], t2);
    sp_3072_add_32(&r[48], &r[48], t0);
    sp_3072_add_32(&r[64], &r[64], p4);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_48(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r5, [%[a], #68]\n\t"
        "ldr	r6, [%[a], #72]\n\t"
        "ldr	r7, [%[a], #76]\n\t"
        "ldr	r8, [%[b], #64]\n\t"
        "ldr	r9, [%[b], #68]\n\t"
        "ldr	r10, [%[b], #72]\n\t"
        "ldr	r14, [%[b], #76]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r5, [%[r], #68]\n\t"
        "str	r6, [%[r], #72]\n\t"
        "str	r7, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r5, [%[a], #84]\n\t"
        "ldr	r6, [%[a], #88]\n\t"
        "ldr	r7, [%[a], #92]\n\t"
        "ldr	r8, [%[b], #80]\n\t"
        "ldr	r9, [%[b], #84]\n\t"
        "ldr	r10, [%[b], #88]\n\t"
        "ldr	r14, [%[b], #92]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r5, [%[r], #84]\n\t"
        "str	r6, [%[r], #88]\n\t"
        "str	r7, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r5, [%[a], #100]\n\t"
        "ldr	r6, [%[a], #104]\n\t"
        "ldr	r7, [%[a], #108]\n\t"
        "ldr	r8, [%[b], #96]\n\t"
        "ldr	r9, [%[b], #100]\n\t"
        "ldr	r10, [%[b], #104]\n\t"
        "ldr	r14, [%[b], #108]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r5, [%[r], #100]\n\t"
        "str	r6, [%[r], #104]\n\t"
        "str	r7, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r5, [%[a], #116]\n\t"
        "ldr	r6, [%[a], #120]\n\t"
        "ldr	r7, [%[a], #124]\n\t"
        "ldr	r8, [%[b], #112]\n\t"
        "ldr	r9, [%[b], #116]\n\t"
        "ldr	r10, [%[b], #120]\n\t"
        "ldr	r14, [%[b], #124]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r5, [%[r], #116]\n\t"
        "str	r6, [%[r], #120]\n\t"
        "str	r7, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r5, [%[a], #132]\n\t"
        "ldr	r6, [%[a], #136]\n\t"
        "ldr	r7, [%[a], #140]\n\t"
        "ldr	r8, [%[b], #128]\n\t"
        "ldr	r9, [%[b], #132]\n\t"
        "ldr	r10, [%[b], #136]\n\t"
        "ldr	r14, [%[b], #140]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r5, [%[r], #132]\n\t"
        "str	r6, [%[r], #136]\n\t"
        "str	r7, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r5, [%[a], #148]\n\t"
        "ldr	r6, [%[a], #152]\n\t"
        "ldr	r7, [%[a], #156]\n\t"
        "ldr	r8, [%[b], #144]\n\t"
        "ldr	r9, [%[b], #148]\n\t"
        "ldr	r10, [%[b], #152]\n\t"
        "ldr	r14, [%[b], #156]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r5, [%[r], #148]\n\t"
        "str	r6, [%[r], #152]\n\t"
        "str	r7, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r5, [%[a], #164]\n\t"
        "ldr	r6, [%[a], #168]\n\t"
        "ldr	r7, [%[a], #172]\n\t"
        "ldr	r8, [%[b], #160]\n\t"
        "ldr	r9, [%[b], #164]\n\t"
        "ldr	r10, [%[b], #168]\n\t"
        "ldr	r14, [%[b], #172]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r5, [%[r], #164]\n\t"
        "str	r6, [%[r], #168]\n\t"
        "str	r7, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r5, [%[a], #180]\n\t"
        "ldr	r6, [%[a], #184]\n\t"
        "ldr	r7, [%[a], #188]\n\t"
        "ldr	r8, [%[b], #176]\n\t"
        "ldr	r9, [%[b], #180]\n\t"
        "ldr	r10, [%[b], #184]\n\t"
        "ldr	r14, [%[b], #188]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r5, [%[r], #180]\n\t"
        "str	r6, [%[r], #184]\n\t"
        "str	r7, [%[r], #188]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_in_place_96(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "ldr	r2, [%[a], #64]\n\t"
        "ldr	r3, [%[a], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r5, [%[a], #76]\n\t"
        "ldr	r6, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "ldr	r8, [%[b], #72]\n\t"
        "ldr	r9, [%[b], #76]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #64]\n\t"
        "str	r3, [%[a], #68]\n\t"
        "str	r4, [%[a], #72]\n\t"
        "str	r5, [%[a], #76]\n\t"
        "ldr	r2, [%[a], #80]\n\t"
        "ldr	r3, [%[a], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r5, [%[a], #92]\n\t"
        "ldr	r6, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "ldr	r8, [%[b], #88]\n\t"
        "ldr	r9, [%[b], #92]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #80]\n\t"
        "str	r3, [%[a], #84]\n\t"
        "str	r4, [%[a], #88]\n\t"
        "str	r5, [%[a], #92]\n\t"
        "ldr	r2, [%[a], #96]\n\t"
        "ldr	r3, [%[a], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r5, [%[a], #108]\n\t"
        "ldr	r6, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "ldr	r8, [%[b], #104]\n\t"
        "ldr	r9, [%[b], #108]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #96]\n\t"
        "str	r3, [%[a], #100]\n\t"
        "str	r4, [%[a], #104]\n\t"
        "str	r5, [%[a], #108]\n\t"
        "ldr	r2, [%[a], #112]\n\t"
        "ldr	r3, [%[a], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r5, [%[a], #124]\n\t"
        "ldr	r6, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "ldr	r8, [%[b], #120]\n\t"
        "ldr	r9, [%[b], #124]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #112]\n\t"
        "str	r3, [%[a], #116]\n\t"
        "str	r4, [%[a], #120]\n\t"
        "str	r5, [%[a], #124]\n\t"
        "ldr	r2, [%[a], #128]\n\t"
        "ldr	r3, [%[a], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r5, [%[a], #140]\n\t"
        "ldr	r6, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "ldr	r8, [%[b], #136]\n\t"
        "ldr	r9, [%[b], #140]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #128]\n\t"
        "str	r3, [%[a], #132]\n\t"
        "str	r4, [%[a], #136]\n\t"
        "str	r5, [%[a], #140]\n\t"
        "ldr	r2, [%[a], #144]\n\t"
        "ldr	r3, [%[a], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r5, [%[a], #156]\n\t"
        "ldr	r6, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "ldr	r8, [%[b], #152]\n\t"
        "ldr	r9, [%[b], #156]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #144]\n\t"
        "str	r3, [%[a], #148]\n\t"
        "str	r4, [%[a], #152]\n\t"
        "str	r5, [%[a], #156]\n\t"
        "ldr	r2, [%[a], #160]\n\t"
        "ldr	r3, [%[a], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r5, [%[a], #172]\n\t"
        "ldr	r6, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "ldr	r8, [%[b], #168]\n\t"
        "ldr	r9, [%[b], #172]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #160]\n\t"
        "str	r3, [%[a], #164]\n\t"
        "str	r4, [%[a], #168]\n\t"
        "str	r5, [%[a], #172]\n\t"
        "ldr	r2, [%[a], #176]\n\t"
        "ldr	r3, [%[a], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r5, [%[a], #188]\n\t"
        "ldr	r6, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "ldr	r8, [%[b], #184]\n\t"
        "ldr	r9, [%[b], #188]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #176]\n\t"
        "str	r3, [%[a], #180]\n\t"
        "str	r4, [%[a], #184]\n\t"
        "str	r5, [%[a], #188]\n\t"
        "ldr	r2, [%[a], #192]\n\t"
        "ldr	r3, [%[a], #196]\n\t"
        "ldr	r4, [%[a], #200]\n\t"
        "ldr	r5, [%[a], #204]\n\t"
        "ldr	r6, [%[b], #192]\n\t"
        "ldr	r7, [%[b], #196]\n\t"
        "ldr	r8, [%[b], #200]\n\t"
        "ldr	r9, [%[b], #204]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #192]\n\t"
        "str	r3, [%[a], #196]\n\t"
        "str	r4, [%[a], #200]\n\t"
        "str	r5, [%[a], #204]\n\t"
        "ldr	r2, [%[a], #208]\n\t"
        "ldr	r3, [%[a], #212]\n\t"
        "ldr	r4, [%[a], #216]\n\t"
        "ldr	r5, [%[a], #220]\n\t"
        "ldr	r6, [%[b], #208]\n\t"
        "ldr	r7, [%[b], #212]\n\t"
        "ldr	r8, [%[b], #216]\n\t"
        "ldr	r9, [%[b], #220]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #208]\n\t"
        "str	r3, [%[a], #212]\n\t"
        "str	r4, [%[a], #216]\n\t"
        "str	r5, [%[a], #220]\n\t"
        "ldr	r2, [%[a], #224]\n\t"
        "ldr	r3, [%[a], #228]\n\t"
        "ldr	r4, [%[a], #232]\n\t"
        "ldr	r5, [%[a], #236]\n\t"
        "ldr	r6, [%[b], #224]\n\t"
        "ldr	r7, [%[b], #228]\n\t"
        "ldr	r8, [%[b], #232]\n\t"
        "ldr	r9, [%[b], #236]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #224]\n\t"
        "str	r3, [%[a], #228]\n\t"
        "str	r4, [%[a], #232]\n\t"
        "str	r5, [%[a], #236]\n\t"
        "ldr	r2, [%[a], #240]\n\t"
        "ldr	r3, [%[a], #244]\n\t"
        "ldr	r4, [%[a], #248]\n\t"
        "ldr	r5, [%[a], #252]\n\t"
        "ldr	r6, [%[b], #240]\n\t"
        "ldr	r7, [%[b], #244]\n\t"
        "ldr	r8, [%[b], #248]\n\t"
        "ldr	r9, [%[b], #252]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #240]\n\t"
        "str	r3, [%[a], #244]\n\t"
        "str	r4, [%[a], #248]\n\t"
        "str	r5, [%[a], #252]\n\t"
        "ldr	r2, [%[a], #256]\n\t"
        "ldr	r3, [%[a], #260]\n\t"
        "ldr	r4, [%[a], #264]\n\t"
        "ldr	r5, [%[a], #268]\n\t"
        "ldr	r6, [%[b], #256]\n\t"
        "ldr	r7, [%[b], #260]\n\t"
        "ldr	r8, [%[b], #264]\n\t"
        "ldr	r9, [%[b], #268]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #256]\n\t"
        "str	r3, [%[a], #260]\n\t"
        "str	r4, [%[a], #264]\n\t"
        "str	r5, [%[a], #268]\n\t"
        "ldr	r2, [%[a], #272]\n\t"
        "ldr	r3, [%[a], #276]\n\t"
        "ldr	r4, [%[a], #280]\n\t"
        "ldr	r5, [%[a], #284]\n\t"
        "ldr	r6, [%[b], #272]\n\t"
        "ldr	r7, [%[b], #276]\n\t"
        "ldr	r8, [%[b], #280]\n\t"
        "ldr	r9, [%[b], #284]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #272]\n\t"
        "str	r3, [%[a], #276]\n\t"
        "str	r4, [%[a], #280]\n\t"
        "str	r5, [%[a], #284]\n\t"
        "ldr	r2, [%[a], #288]\n\t"
        "ldr	r3, [%[a], #292]\n\t"
        "ldr	r4, [%[a], #296]\n\t"
        "ldr	r5, [%[a], #300]\n\t"
        "ldr	r6, [%[b], #288]\n\t"
        "ldr	r7, [%[b], #292]\n\t"
        "ldr	r8, [%[b], #296]\n\t"
        "ldr	r9, [%[b], #300]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #288]\n\t"
        "str	r3, [%[a], #292]\n\t"
        "str	r4, [%[a], #296]\n\t"
        "str	r5, [%[a], #300]\n\t"
        "ldr	r2, [%[a], #304]\n\t"
        "ldr	r3, [%[a], #308]\n\t"
        "ldr	r4, [%[a], #312]\n\t"
        "ldr	r5, [%[a], #316]\n\t"
        "ldr	r6, [%[b], #304]\n\t"
        "ldr	r7, [%[b], #308]\n\t"
        "ldr	r8, [%[b], #312]\n\t"
        "ldr	r9, [%[b], #316]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #304]\n\t"
        "str	r3, [%[a], #308]\n\t"
        "str	r4, [%[a], #312]\n\t"
        "str	r5, [%[a], #316]\n\t"
        "ldr	r2, [%[a], #320]\n\t"
        "ldr	r3, [%[a], #324]\n\t"
        "ldr	r4, [%[a], #328]\n\t"
        "ldr	r5, [%[a], #332]\n\t"
        "ldr	r6, [%[b], #320]\n\t"
        "ldr	r7, [%[b], #324]\n\t"
        "ldr	r8, [%[b], #328]\n\t"
        "ldr	r9, [%[b], #332]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #320]\n\t"
        "str	r3, [%[a], #324]\n\t"
        "str	r4, [%[a], #328]\n\t"
        "str	r5, [%[a], #332]\n\t"
        "ldr	r2, [%[a], #336]\n\t"
        "ldr	r3, [%[a], #340]\n\t"
        "ldr	r4, [%[a], #344]\n\t"
        "ldr	r5, [%[a], #348]\n\t"
        "ldr	r6, [%[b], #336]\n\t"
        "ldr	r7, [%[b], #340]\n\t"
        "ldr	r8, [%[b], #344]\n\t"
        "ldr	r9, [%[b], #348]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #336]\n\t"
        "str	r3, [%[a], #340]\n\t"
        "str	r4, [%[a], #344]\n\t"
        "str	r5, [%[a], #348]\n\t"
        "ldr	r2, [%[a], #352]\n\t"
        "ldr	r3, [%[a], #356]\n\t"
        "ldr	r4, [%[a], #360]\n\t"
        "ldr	r5, [%[a], #364]\n\t"
        "ldr	r6, [%[b], #352]\n\t"
        "ldr	r7, [%[b], #356]\n\t"
        "ldr	r8, [%[b], #360]\n\t"
        "ldr	r9, [%[b], #364]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #352]\n\t"
        "str	r3, [%[a], #356]\n\t"
        "str	r4, [%[a], #360]\n\t"
        "str	r5, [%[a], #364]\n\t"
        "ldr	r2, [%[a], #368]\n\t"
        "ldr	r3, [%[a], #372]\n\t"
        "ldr	r4, [%[a], #376]\n\t"
        "ldr	r5, [%[a], #380]\n\t"
        "ldr	r6, [%[b], #368]\n\t"
        "ldr	r7, [%[b], #372]\n\t"
        "ldr	r8, [%[b], #376]\n\t"
        "ldr	r9, [%[b], #380]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #368]\n\t"
        "str	r3, [%[a], #372]\n\t"
        "str	r4, [%[a], #376]\n\t"
        "str	r5, [%[a], #380]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_96(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r5, [%[a], #36]\n\t"
        "ldr	r6, [%[a], #40]\n\t"
        "ldr	r7, [%[a], #44]\n\t"
        "ldr	r8, [%[b], #32]\n\t"
        "ldr	r9, [%[b], #36]\n\t"
        "ldr	r10, [%[b], #40]\n\t"
        "ldr	r14, [%[b], #44]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r5, [%[r], #36]\n\t"
        "str	r6, [%[r], #40]\n\t"
        "str	r7, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r5, [%[a], #52]\n\t"
        "ldr	r6, [%[a], #56]\n\t"
        "ldr	r7, [%[a], #60]\n\t"
        "ldr	r8, [%[b], #48]\n\t"
        "ldr	r9, [%[b], #52]\n\t"
        "ldr	r10, [%[b], #56]\n\t"
        "ldr	r14, [%[b], #60]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r5, [%[r], #52]\n\t"
        "str	r6, [%[r], #56]\n\t"
        "str	r7, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r5, [%[a], #68]\n\t"
        "ldr	r6, [%[a], #72]\n\t"
        "ldr	r7, [%[a], #76]\n\t"
        "ldr	r8, [%[b], #64]\n\t"
        "ldr	r9, [%[b], #68]\n\t"
        "ldr	r10, [%[b], #72]\n\t"
        "ldr	r14, [%[b], #76]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r5, [%[r], #68]\n\t"
        "str	r6, [%[r], #72]\n\t"
        "str	r7, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r5, [%[a], #84]\n\t"
        "ldr	r6, [%[a], #88]\n\t"
        "ldr	r7, [%[a], #92]\n\t"
        "ldr	r8, [%[b], #80]\n\t"
        "ldr	r9, [%[b], #84]\n\t"
        "ldr	r10, [%[b], #88]\n\t"
        "ldr	r14, [%[b], #92]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r5, [%[r], #84]\n\t"
        "str	r6, [%[r], #88]\n\t"
        "str	r7, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r5, [%[a], #100]\n\t"
        "ldr	r6, [%[a], #104]\n\t"
        "ldr	r7, [%[a], #108]\n\t"
        "ldr	r8, [%[b], #96]\n\t"
        "ldr	r9, [%[b], #100]\n\t"
        "ldr	r10, [%[b], #104]\n\t"
        "ldr	r14, [%[b], #108]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r5, [%[r], #100]\n\t"
        "str	r6, [%[r], #104]\n\t"
        "str	r7, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r5, [%[a], #116]\n\t"
        "ldr	r6, [%[a], #120]\n\t"
        "ldr	r7, [%[a], #124]\n\t"
        "ldr	r8, [%[b], #112]\n\t"
        "ldr	r9, [%[b], #116]\n\t"
        "ldr	r10, [%[b], #120]\n\t"
        "ldr	r14, [%[b], #124]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r5, [%[r], #116]\n\t"
        "str	r6, [%[r], #120]\n\t"
        "str	r7, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r5, [%[a], #132]\n\t"
        "ldr	r6, [%[a], #136]\n\t"
        "ldr	r7, [%[a], #140]\n\t"
        "ldr	r8, [%[b], #128]\n\t"
        "ldr	r9, [%[b], #132]\n\t"
        "ldr	r10, [%[b], #136]\n\t"
        "ldr	r14, [%[b], #140]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r5, [%[r], #132]\n\t"
        "str	r6, [%[r], #136]\n\t"
        "str	r7, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r5, [%[a], #148]\n\t"
        "ldr	r6, [%[a], #152]\n\t"
        "ldr	r7, [%[a], #156]\n\t"
        "ldr	r8, [%[b], #144]\n\t"
        "ldr	r9, [%[b], #148]\n\t"
        "ldr	r10, [%[b], #152]\n\t"
        "ldr	r14, [%[b], #156]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r5, [%[r], #148]\n\t"
        "str	r6, [%[r], #152]\n\t"
        "str	r7, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r5, [%[a], #164]\n\t"
        "ldr	r6, [%[a], #168]\n\t"
        "ldr	r7, [%[a], #172]\n\t"
        "ldr	r8, [%[b], #160]\n\t"
        "ldr	r9, [%[b], #164]\n\t"
        "ldr	r10, [%[b], #168]\n\t"
        "ldr	r14, [%[b], #172]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r5, [%[r], #164]\n\t"
        "str	r6, [%[r], #168]\n\t"
        "str	r7, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r5, [%[a], #180]\n\t"
        "ldr	r6, [%[a], #184]\n\t"
        "ldr	r7, [%[a], #188]\n\t"
        "ldr	r8, [%[b], #176]\n\t"
        "ldr	r9, [%[b], #180]\n\t"
        "ldr	r10, [%[b], #184]\n\t"
        "ldr	r14, [%[b], #188]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r5, [%[r], #180]\n\t"
        "str	r6, [%[r], #184]\n\t"
        "str	r7, [%[r], #188]\n\t"
        "ldr	r4, [%[a], #192]\n\t"
        "ldr	r5, [%[a], #196]\n\t"
        "ldr	r6, [%[a], #200]\n\t"
        "ldr	r7, [%[a], #204]\n\t"
        "ldr	r8, [%[b], #192]\n\t"
        "ldr	r9, [%[b], #196]\n\t"
        "ldr	r10, [%[b], #200]\n\t"
        "ldr	r14, [%[b], #204]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #192]\n\t"
        "str	r5, [%[r], #196]\n\t"
        "str	r6, [%[r], #200]\n\t"
        "str	r7, [%[r], #204]\n\t"
        "ldr	r4, [%[a], #208]\n\t"
        "ldr	r5, [%[a], #212]\n\t"
        "ldr	r6, [%[a], #216]\n\t"
        "ldr	r7, [%[a], #220]\n\t"
        "ldr	r8, [%[b], #208]\n\t"
        "ldr	r9, [%[b], #212]\n\t"
        "ldr	r10, [%[b], #216]\n\t"
        "ldr	r14, [%[b], #220]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #208]\n\t"
        "str	r5, [%[r], #212]\n\t"
        "str	r6, [%[r], #216]\n\t"
        "str	r7, [%[r], #220]\n\t"
        "ldr	r4, [%[a], #224]\n\t"
        "ldr	r5, [%[a], #228]\n\t"
        "ldr	r6, [%[a], #232]\n\t"
        "ldr	r7, [%[a], #236]\n\t"
        "ldr	r8, [%[b], #224]\n\t"
        "ldr	r9, [%[b], #228]\n\t"
        "ldr	r10, [%[b], #232]\n\t"
        "ldr	r14, [%[b], #236]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #224]\n\t"
        "str	r5, [%[r], #228]\n\t"
        "str	r6, [%[r], #232]\n\t"
        "str	r7, [%[r], #236]\n\t"
        "ldr	r4, [%[a], #240]\n\t"
        "ldr	r5, [%[a], #244]\n\t"
        "ldr	r6, [%[a], #248]\n\t"
        "ldr	r7, [%[a], #252]\n\t"
        "ldr	r8, [%[b], #240]\n\t"
        "ldr	r9, [%[b], #244]\n\t"
        "ldr	r10, [%[b], #248]\n\t"
        "ldr	r14, [%[b], #252]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #240]\n\t"
        "str	r5, [%[r], #244]\n\t"
        "str	r6, [%[r], #248]\n\t"
        "str	r7, [%[r], #252]\n\t"
        "ldr	r4, [%[a], #256]\n\t"
        "ldr	r5, [%[a], #260]\n\t"
        "ldr	r6, [%[a], #264]\n\t"
        "ldr	r7, [%[a], #268]\n\t"
        "ldr	r8, [%[b], #256]\n\t"
        "ldr	r9, [%[b], #260]\n\t"
        "ldr	r10, [%[b], #264]\n\t"
        "ldr	r14, [%[b], #268]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #256]\n\t"
        "str	r5, [%[r], #260]\n\t"
        "str	r6, [%[r], #264]\n\t"
        "str	r7, [%[r], #268]\n\t"
        "ldr	r4, [%[a], #272]\n\t"
        "ldr	r5, [%[a], #276]\n\t"
        "ldr	r6, [%[a], #280]\n\t"
        "ldr	r7, [%[a], #284]\n\t"
        "ldr	r8, [%[b], #272]\n\t"
        "ldr	r9, [%[b], #276]\n\t"
        "ldr	r10, [%[b], #280]\n\t"
        "ldr	r14, [%[b], #284]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #272]\n\t"
        "str	r5, [%[r], #276]\n\t"
        "str	r6, [%[r], #280]\n\t"
        "str	r7, [%[r], #284]\n\t"
        "ldr	r4, [%[a], #288]\n\t"
        "ldr	r5, [%[a], #292]\n\t"
        "ldr	r6, [%[a], #296]\n\t"
        "ldr	r7, [%[a], #300]\n\t"
        "ldr	r8, [%[b], #288]\n\t"
        "ldr	r9, [%[b], #292]\n\t"
        "ldr	r10, [%[b], #296]\n\t"
        "ldr	r14, [%[b], #300]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #288]\n\t"
        "str	r5, [%[r], #292]\n\t"
        "str	r6, [%[r], #296]\n\t"
        "str	r7, [%[r], #300]\n\t"
        "ldr	r4, [%[a], #304]\n\t"
        "ldr	r5, [%[a], #308]\n\t"
        "ldr	r6, [%[a], #312]\n\t"
        "ldr	r7, [%[a], #316]\n\t"
        "ldr	r8, [%[b], #304]\n\t"
        "ldr	r9, [%[b], #308]\n\t"
        "ldr	r10, [%[b], #312]\n\t"
        "ldr	r14, [%[b], #316]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #304]\n\t"
        "str	r5, [%[r], #308]\n\t"
        "str	r6, [%[r], #312]\n\t"
        "str	r7, [%[r], #316]\n\t"
        "ldr	r4, [%[a], #320]\n\t"
        "ldr	r5, [%[a], #324]\n\t"
        "ldr	r6, [%[a], #328]\n\t"
        "ldr	r7, [%[a], #332]\n\t"
        "ldr	r8, [%[b], #320]\n\t"
        "ldr	r9, [%[b], #324]\n\t"
        "ldr	r10, [%[b], #328]\n\t"
        "ldr	r14, [%[b], #332]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #320]\n\t"
        "str	r5, [%[r], #324]\n\t"
        "str	r6, [%[r], #328]\n\t"
        "str	r7, [%[r], #332]\n\t"
        "ldr	r4, [%[a], #336]\n\t"
        "ldr	r5, [%[a], #340]\n\t"
        "ldr	r6, [%[a], #344]\n\t"
        "ldr	r7, [%[a], #348]\n\t"
        "ldr	r8, [%[b], #336]\n\t"
        "ldr	r9, [%[b], #340]\n\t"
        "ldr	r10, [%[b], #344]\n\t"
        "ldr	r14, [%[b], #348]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #336]\n\t"
        "str	r5, [%[r], #340]\n\t"
        "str	r6, [%[r], #344]\n\t"
        "str	r7, [%[r], #348]\n\t"
        "ldr	r4, [%[a], #352]\n\t"
        "ldr	r5, [%[a], #356]\n\t"
        "ldr	r6, [%[a], #360]\n\t"
        "ldr	r7, [%[a], #364]\n\t"
        "ldr	r8, [%[b], #352]\n\t"
        "ldr	r9, [%[b], #356]\n\t"
        "ldr	r10, [%[b], #360]\n\t"
        "ldr	r14, [%[b], #364]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #352]\n\t"
        "str	r5, [%[r], #356]\n\t"
        "str	r6, [%[r], #360]\n\t"
        "str	r7, [%[r], #364]\n\t"
        "ldr	r4, [%[a], #368]\n\t"
        "ldr	r5, [%[a], #372]\n\t"
        "ldr	r6, [%[a], #376]\n\t"
        "ldr	r7, [%[a], #380]\n\t"
        "ldr	r8, [%[b], #368]\n\t"
        "ldr	r9, [%[b], #372]\n\t"
        "ldr	r10, [%[b], #376]\n\t"
        "ldr	r14, [%[b], #380]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #368]\n\t"
        "str	r5, [%[r], #372]\n\t"
        "str	r6, [%[r], #376]\n\t"
        "str	r7, [%[r], #380]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_3072_mask_48(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<48; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
#endif
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_3072_mul_96(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[96];
    sp_digit a1[48];
    sp_digit b1[48];
    sp_digit z2[96];
    sp_digit u, ca, cb;

    ca = sp_3072_add_48(a1, a, &a[48]);
    cb = sp_3072_add_48(b1, b, &b[48]);
    u  = ca & cb;
    sp_3072_mul_48(z1, a1, b1);
    sp_3072_mul_48(z2, &a[48], &b[48]);
    sp_3072_mul_48(z0, a, b);
    sp_3072_mask_48(r + 96, a1, 0 - cb);
    sp_3072_mask_48(b1, b1, 0 - ca);
    u += sp_3072_add_48(r + 96, r + 96, b1);
    u += sp_3072_sub_in_place_96(z1, z2);
    u += sp_3072_sub_in_place_96(z1, z0);
    u += sp_3072_add_96(r + 48, r + 48, z1);
    r[144] = u;
    XMEMSET(r + 144 + 1, 0, sizeof(sp_digit) * (48 - 1));
    sp_3072_add_96(r + 96, r + 96, z2);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_3072_sqr_96(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z2[96];
    sp_digit z1[96];
    sp_digit a1[48];
    sp_digit u;

    u = sp_3072_add_48(a1, a, &a[48]);
    sp_3072_sqr_48(z1, a1);
    sp_3072_sqr_48(z2, &a[48]);
    sp_3072_sqr_48(z0, a);
    sp_3072_mask_48(r + 96, a1, 0 - u);
    u += sp_3072_add_48(r + 96, r + 96, r + 96);
    u += sp_3072_sub_in_place_96(z1, z2);
    u += sp_3072_sub_in_place_96(z1, z0);
    u += sp_3072_add_96(r + 48, r + 48, z1);
    r[144] = u;
    XMEMSET(r + 144 + 1, 0, sizeof(sp_digit) * (48 - 1));
    sp_3072_add_96(r + 96, r + 96, z2);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_96(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #384\n\t"
        "\n1:\n\t"
        "adds	%[c], %[c], #-1\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "mov	r4, #0\n\t"
        "adc	%[c], r4, #0\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_in_place_96(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "add	r12, %[a], #384\n\t"
        "\n1:\n\t"
        "subs	%[c], r14, %[c]\n\t"
        "ldr	r3, [%[a]]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[a]], #4\n\t"
        "str	r4, [%[a]], #4\n\t"
        "str	r5, [%[a]], #4\n\t"
        "str	r6, [%[a]], #4\n\t"
        "sbc	%[c], r14, r14\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_3072_mul_96(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[192];

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #380\n\t"
        "movcc	r3, #0\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r12, [%[b], r4]\n\t"
        "umull	r9, r10, r14, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, #0\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #384\n\t"
        "beq	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #760\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_3072_sqr_96(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[192];

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r5, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #380\n\t"
        "movcc	r3, r12\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "cmp	r4, r3\n\t"
        "beq	4f\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r9, [%[a], r4]\n\t"
        "umull	r9, r10, r14, r9\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "bal	5f\n\t"
        "\n4:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "umull	r9, r10, r14, r14\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "\n5:\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #384\n\t"
        "beq	3f\n\t"
        "cmp	r3, r4\n\t"
        "bgt	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #760\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r9", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
#ifdef WOLFSSL_SP_SMALL
/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_3072_mask_48(sp_digit* r, sp_digit* a, sp_digit m)
{
    int i;

    for (i=0; i<48; i++)
        r[i] = a[i] & m;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_add_48(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #192\n\t"
        "\n1:\n\t"
        "adds	%[c], %[c], #-1\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "mov	r4, #0\n\t"
        "adc	%[c], r4, #0\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_3072_mul_48(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[96];

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #188\n\t"
        "movcc	r3, #0\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r12, [%[b], r4]\n\t"
        "umull	r9, r10, r14, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, #0\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #192\n\t"
        "beq	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #376\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_3072_sqr_48(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[96];

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r5, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #188\n\t"
        "movcc	r3, r12\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "cmp	r4, r3\n\t"
        "beq	4f\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r9, [%[a], r4]\n\t"
        "umull	r9, r10, r14, r9\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "bal	5f\n\t"
        "\n4:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "umull	r9, r10, r14, r14\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "\n5:\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #192\n\t"
        "beq	3f\n\t"
        "cmp	r3, r4\n\t"
        "bgt	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #376\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r9", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* Caclulate the bottom digit of -1/a mod 2^n.
 *
 * a    A single precision number.
 * rho  Bottom word of inverse.
 */
static void sp_3072_mont_setup(sp_digit* a, sp_digit* rho)
{
    sp_digit x, b;

    b = a[0];
    x = (((b + 2) & 4) << 1) + b; /* here x*a==1 mod 2**4 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**8 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**16 */
    x *= 2 - b * x;               /* here x*a==1 mod 2**32 */

    /* rho = -1/m mod b */
    *rho = -x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_in_place_48(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "add	r12, %[a], #192\n\t"
        "\n1:\n\t"
        "subs	%[c], r14, %[c]\n\t"
        "ldr	r3, [%[a]]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[a]], #4\n\t"
        "str	r4, [%[a]], #4\n\t"
        "str	r5, [%[a]], #4\n\t"
        "str	r6, [%[a]], #4\n\t"
        "sbc	%[c], r14, r14\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
    );

    return c;
}

#else
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_3072_sub_in_place_48(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "ldr	r2, [%[a], #32]\n\t"
        "ldr	r3, [%[a], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r5, [%[a], #44]\n\t"
        "ldr	r6, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "ldr	r8, [%[b], #40]\n\t"
        "ldr	r9, [%[b], #44]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #32]\n\t"
        "str	r3, [%[a], #36]\n\t"
        "str	r4, [%[a], #40]\n\t"
        "str	r5, [%[a], #44]\n\t"
        "ldr	r2, [%[a], #48]\n\t"
        "ldr	r3, [%[a], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r5, [%[a], #60]\n\t"
        "ldr	r6, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "ldr	r8, [%[b], #56]\n\t"
        "ldr	r9, [%[b], #60]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #48]\n\t"
        "str	r3, [%[a], #52]\n\t"
        "str	r4, [%[a], #56]\n\t"
        "str	r5, [%[a], #60]\n\t"
        "ldr	r2, [%[a], #64]\n\t"
        "ldr	r3, [%[a], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r5, [%[a], #76]\n\t"
        "ldr	r6, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "ldr	r8, [%[b], #72]\n\t"
        "ldr	r9, [%[b], #76]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #64]\n\t"
        "str	r3, [%[a], #68]\n\t"
        "str	r4, [%[a], #72]\n\t"
        "str	r5, [%[a], #76]\n\t"
        "ldr	r2, [%[a], #80]\n\t"
        "ldr	r3, [%[a], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r5, [%[a], #92]\n\t"
        "ldr	r6, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "ldr	r8, [%[b], #88]\n\t"
        "ldr	r9, [%[b], #92]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #80]\n\t"
        "str	r3, [%[a], #84]\n\t"
        "str	r4, [%[a], #88]\n\t"
        "str	r5, [%[a], #92]\n\t"
        "ldr	r2, [%[a], #96]\n\t"
        "ldr	r3, [%[a], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r5, [%[a], #108]\n\t"
        "ldr	r6, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "ldr	r8, [%[b], #104]\n\t"
        "ldr	r9, [%[b], #108]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #96]\n\t"
        "str	r3, [%[a], #100]\n\t"
        "str	r4, [%[a], #104]\n\t"
        "str	r5, [%[a], #108]\n\t"
        "ldr	r2, [%[a], #112]\n\t"
        "ldr	r3, [%[a], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r5, [%[a], #124]\n\t"
        "ldr	r6, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "ldr	r8, [%[b], #120]\n\t"
        "ldr	r9, [%[b], #124]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #112]\n\t"
        "str	r3, [%[a], #116]\n\t"
        "str	r4, [%[a], #120]\n\t"
        "str	r5, [%[a], #124]\n\t"
        "ldr	r2, [%[a], #128]\n\t"
        "ldr	r3, [%[a], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r5, [%[a], #140]\n\t"
        "ldr	r6, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "ldr	r8, [%[b], #136]\n\t"
        "ldr	r9, [%[b], #140]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #128]\n\t"
        "str	r3, [%[a], #132]\n\t"
        "str	r4, [%[a], #136]\n\t"
        "str	r5, [%[a], #140]\n\t"
        "ldr	r2, [%[a], #144]\n\t"
        "ldr	r3, [%[a], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r5, [%[a], #156]\n\t"
        "ldr	r6, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "ldr	r8, [%[b], #152]\n\t"
        "ldr	r9, [%[b], #156]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #144]\n\t"
        "str	r3, [%[a], #148]\n\t"
        "str	r4, [%[a], #152]\n\t"
        "str	r5, [%[a], #156]\n\t"
        "ldr	r2, [%[a], #160]\n\t"
        "ldr	r3, [%[a], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r5, [%[a], #172]\n\t"
        "ldr	r6, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "ldr	r8, [%[b], #168]\n\t"
        "ldr	r9, [%[b], #172]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #160]\n\t"
        "str	r3, [%[a], #164]\n\t"
        "str	r4, [%[a], #168]\n\t"
        "str	r5, [%[a], #172]\n\t"
        "ldr	r2, [%[a], #176]\n\t"
        "ldr	r3, [%[a], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r5, [%[a], #188]\n\t"
        "ldr	r6, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "ldr	r8, [%[b], #184]\n\t"
        "ldr	r9, [%[b], #188]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #176]\n\t"
        "str	r3, [%[a], #180]\n\t"
        "str	r4, [%[a], #184]\n\t"
        "str	r5, [%[a], #188]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 3072 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_3072_mont_norm_48(sp_digit* r, sp_digit* m)
{
    XMEMSET(r, 0, sizeof(sp_digit) * 48);

    /* r = 2^n mod m */
    sp_3072_sub_in_place_48(r, m);
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static sp_digit sp_3072_cond_sub_48(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit m)
{
    sp_digit c = 0;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r9, #0\n\t"
        "mov	r8, #0\n\t"
        "1:\n\t"
        "subs	%[c], r9, %[c]\n\t"
        "ldr	r4, [%[a], r8]\n\t"
        "ldr	r5, [%[b], r8]\n\t"
        "and	r5, r5, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbc	%[c], r9, r9\n\t"
        "str	r4, [%[r], r8]\n\t"
        "add	r8, r8, #4\n\t"
        "cmp	r8, #192\n\t"
        "blt	1b\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#else
    __asm__ __volatile__ (

        "mov	r9, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r5, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "subs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r6, [%[r], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r5, [%[b], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r5, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r6, [%[r], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r5, [%[b], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r6, [%[a], #36]\n\t"
        "ldr	r5, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r6, [%[r], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r6, [%[a], #44]\n\t"
        "ldr	r5, [%[b], #40]\n\t"
        "ldr	r7, [%[b], #44]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #40]\n\t"
        "str	r6, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r6, [%[a], #52]\n\t"
        "ldr	r5, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r6, [%[r], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r6, [%[a], #60]\n\t"
        "ldr	r5, [%[b], #56]\n\t"
        "ldr	r7, [%[b], #60]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r6, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r6, [%[a], #68]\n\t"
        "ldr	r5, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r6, [%[r], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r6, [%[a], #76]\n\t"
        "ldr	r5, [%[b], #72]\n\t"
        "ldr	r7, [%[b], #76]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #72]\n\t"
        "str	r6, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r6, [%[a], #84]\n\t"
        "ldr	r5, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r6, [%[r], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r6, [%[a], #92]\n\t"
        "ldr	r5, [%[b], #88]\n\t"
        "ldr	r7, [%[b], #92]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #88]\n\t"
        "str	r6, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r6, [%[a], #100]\n\t"
        "ldr	r5, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r6, [%[r], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r6, [%[a], #108]\n\t"
        "ldr	r5, [%[b], #104]\n\t"
        "ldr	r7, [%[b], #108]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #104]\n\t"
        "str	r6, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r6, [%[a], #116]\n\t"
        "ldr	r5, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r6, [%[r], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r6, [%[a], #124]\n\t"
        "ldr	r5, [%[b], #120]\n\t"
        "ldr	r7, [%[b], #124]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #120]\n\t"
        "str	r6, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r6, [%[a], #132]\n\t"
        "ldr	r5, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r6, [%[r], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r6, [%[a], #140]\n\t"
        "ldr	r5, [%[b], #136]\n\t"
        "ldr	r7, [%[b], #140]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #136]\n\t"
        "str	r6, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r6, [%[a], #148]\n\t"
        "ldr	r5, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r6, [%[r], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r6, [%[a], #156]\n\t"
        "ldr	r5, [%[b], #152]\n\t"
        "ldr	r7, [%[b], #156]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #152]\n\t"
        "str	r6, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r6, [%[a], #164]\n\t"
        "ldr	r5, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r6, [%[r], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r6, [%[a], #172]\n\t"
        "ldr	r5, [%[b], #168]\n\t"
        "ldr	r7, [%[b], #172]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #168]\n\t"
        "str	r6, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r6, [%[a], #180]\n\t"
        "ldr	r5, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r6, [%[r], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r6, [%[a], #188]\n\t"
        "ldr	r5, [%[b], #184]\n\t"
        "ldr	r7, [%[b], #188]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #184]\n\t"
        "str	r6, [%[r], #188]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#endif /* WOLFSSL_SP_SMALL */

    return c;
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
SP_NOINLINE static void sp_3072_mont_reduce_48(sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit ca = 0;

    __asm__ __volatile__ (
        "# i = 0\n\t"
        "mov	r12, #0\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "ldr	r14, [%[a], #4]\n\t"
        "\n1:\n\t"
        "# mu = a[i] * mp\n\t"
        "mul	r8, %[mp], r10\n\t"
        "# a[i+0] += m[0] * mu\n\t"
        "ldr	r7, [%[m], #0]\n\t"
        "ldr	r9, [%[a], #0]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r10, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "# a[i+1] += m[1] * mu\n\t"
        "ldr	r7, [%[m], #4]\n\t"
        "ldr	r9, [%[a], #4]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r14, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r10, r10, r5\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+2] += m[2] * mu\n\t"
        "ldr	r7, [%[m], #8]\n\t"
        "ldr	r14, [%[a], #8]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r14, r14, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r14, r14, r4\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+3] += m[3] * mu\n\t"
        "ldr	r7, [%[m], #12]\n\t"
        "ldr	r9, [%[a], #12]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #12]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+4] += m[4] * mu\n\t"
        "ldr	r7, [%[m], #16]\n\t"
        "ldr	r9, [%[a], #16]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #16]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+5] += m[5] * mu\n\t"
        "ldr	r7, [%[m], #20]\n\t"
        "ldr	r9, [%[a], #20]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #20]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+6] += m[6] * mu\n\t"
        "ldr	r7, [%[m], #24]\n\t"
        "ldr	r9, [%[a], #24]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #24]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+7] += m[7] * mu\n\t"
        "ldr	r7, [%[m], #28]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #28]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+8] += m[8] * mu\n\t"
        "ldr	r7, [%[m], #32]\n\t"
        "ldr	r9, [%[a], #32]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #32]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+9] += m[9] * mu\n\t"
        "ldr	r7, [%[m], #36]\n\t"
        "ldr	r9, [%[a], #36]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #36]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+10] += m[10] * mu\n\t"
        "ldr	r7, [%[m], #40]\n\t"
        "ldr	r9, [%[a], #40]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #40]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+11] += m[11] * mu\n\t"
        "ldr	r7, [%[m], #44]\n\t"
        "ldr	r9, [%[a], #44]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #44]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+12] += m[12] * mu\n\t"
        "ldr	r7, [%[m], #48]\n\t"
        "ldr	r9, [%[a], #48]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #48]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+13] += m[13] * mu\n\t"
        "ldr	r7, [%[m], #52]\n\t"
        "ldr	r9, [%[a], #52]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #52]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+14] += m[14] * mu\n\t"
        "ldr	r7, [%[m], #56]\n\t"
        "ldr	r9, [%[a], #56]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #56]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+15] += m[15] * mu\n\t"
        "ldr	r7, [%[m], #60]\n\t"
        "ldr	r9, [%[a], #60]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #60]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+16] += m[16] * mu\n\t"
        "ldr	r7, [%[m], #64]\n\t"
        "ldr	r9, [%[a], #64]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #64]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+17] += m[17] * mu\n\t"
        "ldr	r7, [%[m], #68]\n\t"
        "ldr	r9, [%[a], #68]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #68]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+18] += m[18] * mu\n\t"
        "ldr	r7, [%[m], #72]\n\t"
        "ldr	r9, [%[a], #72]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #72]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+19] += m[19] * mu\n\t"
        "ldr	r7, [%[m], #76]\n\t"
        "ldr	r9, [%[a], #76]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #76]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+20] += m[20] * mu\n\t"
        "ldr	r7, [%[m], #80]\n\t"
        "ldr	r9, [%[a], #80]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #80]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+21] += m[21] * mu\n\t"
        "ldr	r7, [%[m], #84]\n\t"
        "ldr	r9, [%[a], #84]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #84]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+22] += m[22] * mu\n\t"
        "ldr	r7, [%[m], #88]\n\t"
        "ldr	r9, [%[a], #88]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #88]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+23] += m[23] * mu\n\t"
        "ldr	r7, [%[m], #92]\n\t"
        "ldr	r9, [%[a], #92]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #92]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+24] += m[24] * mu\n\t"
        "ldr	r7, [%[m], #96]\n\t"
        "ldr	r9, [%[a], #96]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #96]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+25] += m[25] * mu\n\t"
        "ldr	r7, [%[m], #100]\n\t"
        "ldr	r9, [%[a], #100]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #100]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+26] += m[26] * mu\n\t"
        "ldr	r7, [%[m], #104]\n\t"
        "ldr	r9, [%[a], #104]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #104]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+27] += m[27] * mu\n\t"
        "ldr	r7, [%[m], #108]\n\t"
        "ldr	r9, [%[a], #108]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #108]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+28] += m[28] * mu\n\t"
        "ldr	r7, [%[m], #112]\n\t"
        "ldr	r9, [%[a], #112]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #112]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+29] += m[29] * mu\n\t"
        "ldr	r7, [%[m], #116]\n\t"
        "ldr	r9, [%[a], #116]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #116]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+30] += m[30] * mu\n\t"
        "ldr	r7, [%[m], #120]\n\t"
        "ldr	r9, [%[a], #120]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #120]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+31] += m[31] * mu\n\t"
        "ldr	r7, [%[m], #124]\n\t"
        "ldr	r9, [%[a], #124]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #124]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+32] += m[32] * mu\n\t"
        "ldr	r7, [%[m], #128]\n\t"
        "ldr	r9, [%[a], #128]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #128]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+33] += m[33] * mu\n\t"
        "ldr	r7, [%[m], #132]\n\t"
        "ldr	r9, [%[a], #132]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #132]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+34] += m[34] * mu\n\t"
        "ldr	r7, [%[m], #136]\n\t"
        "ldr	r9, [%[a], #136]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #136]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+35] += m[35] * mu\n\t"
        "ldr	r7, [%[m], #140]\n\t"
        "ldr	r9, [%[a], #140]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #140]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+36] += m[36] * mu\n\t"
        "ldr	r7, [%[m], #144]\n\t"
        "ldr	r9, [%[a], #144]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #144]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+37] += m[37] * mu\n\t"
        "ldr	r7, [%[m], #148]\n\t"
        "ldr	r9, [%[a], #148]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #148]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+38] += m[38] * mu\n\t"
        "ldr	r7, [%[m], #152]\n\t"
        "ldr	r9, [%[a], #152]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #152]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+39] += m[39] * mu\n\t"
        "ldr	r7, [%[m], #156]\n\t"
        "ldr	r9, [%[a], #156]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #156]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+40] += m[40] * mu\n\t"
        "ldr	r7, [%[m], #160]\n\t"
        "ldr	r9, [%[a], #160]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #160]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+41] += m[41] * mu\n\t"
        "ldr	r7, [%[m], #164]\n\t"
        "ldr	r9, [%[a], #164]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #164]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+42] += m[42] * mu\n\t"
        "ldr	r7, [%[m], #168]\n\t"
        "ldr	r9, [%[a], #168]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #168]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+43] += m[43] * mu\n\t"
        "ldr	r7, [%[m], #172]\n\t"
        "ldr	r9, [%[a], #172]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #172]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+44] += m[44] * mu\n\t"
        "ldr	r7, [%[m], #176]\n\t"
        "ldr	r9, [%[a], #176]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #176]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+45] += m[45] * mu\n\t"
        "ldr	r7, [%[m], #180]\n\t"
        "ldr	r9, [%[a], #180]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #180]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+46] += m[46] * mu\n\t"
        "ldr	r7, [%[m], #184]\n\t"
        "ldr	r9, [%[a], #184]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #184]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+47] += m[47] * mu\n\t"
        "ldr	r7, [%[m], #188]\n\t"
        "ldr   r9, [%[a], #188]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r7, r7, %[ca]\n\t"
        "mov	%[ca], #0\n\t"
        "adc	%[ca], %[ca], %[ca]\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #188]\n\t"
        "ldr	r9, [%[a], #192]\n\t"
        "adcs	r9, r9, r7\n\t"
        "str	r9, [%[a], #192]\n\t"
        "adc	%[ca], %[ca], #0\n\t"
        "# i += 1\n\t"
        "add	%[a], %[a], #4\n\t"
        "add	r12, r12, #4\n\t"
        "cmp	r12, #192\n\t"
        "blt	1b\n\t"
        "str	r10, [%[a], #0]\n\t"
        "str	r14, [%[a], #4]\n\t"
        : [ca] "+r" (ca), [a] "+r" (a)
        : [m] "r" (m), [mp] "r" (mp)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    sp_3072_cond_sub_48(a - 48, a, m, (sp_digit)0 - ca);
}

/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_mul_48(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_48(r, a, b);
    sp_3072_mont_reduce_48(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_48(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_48(r, a);
    sp_3072_mont_reduce_48(r, m, mp);
}

/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
static void sp_3072_mul_d_48(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r5, r3, %[b], r8\n\t"
        "mov	r4, #0\n\t"
        "str	r5, [%[r]]\n\t"
        "mov	r5, #0\n\t"
        "mov	r9, #4\n\t"
        "1:\n\t"
        "ldr	r8, [%[a], r9]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], r9]\n\t"
        "mov	r3, r4\n\t"
        "mov	r4, r5\n\t"
        "mov	r5, #0\n\t"
        "add	r9, r9, #4\n\t"
        "cmp	r9, #192\n\t"
        "blt	1b\n\t"
        "str	r3, [%[r], #192]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#else
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r3, r4, %[b], r8\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[r]]\n\t"
        "# A[1] * B\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #4]\n\t"
        "# A[2] * B\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #8]\n\t"
        "# A[3] * B\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #12]\n\t"
        "# A[4] * B\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #16]\n\t"
        "# A[5] * B\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #20]\n\t"
        "# A[6] * B\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #24]\n\t"
        "# A[7] * B\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #28]\n\t"
        "# A[8] * B\n\t"
        "ldr	r8, [%[a], #32]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "# A[9] * B\n\t"
        "ldr	r8, [%[a], #36]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "# A[10] * B\n\t"
        "ldr	r8, [%[a], #40]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "# A[11] * B\n\t"
        "ldr	r8, [%[a], #44]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "# A[12] * B\n\t"
        "ldr	r8, [%[a], #48]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "# A[13] * B\n\t"
        "ldr	r8, [%[a], #52]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "# A[14] * B\n\t"
        "ldr	r8, [%[a], #56]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #56]\n\t"
        "# A[15] * B\n\t"
        "ldr	r8, [%[a], #60]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #60]\n\t"
        "# A[16] * B\n\t"
        "ldr	r8, [%[a], #64]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #64]\n\t"
        "# A[17] * B\n\t"
        "ldr	r8, [%[a], #68]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #68]\n\t"
        "# A[18] * B\n\t"
        "ldr	r8, [%[a], #72]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #72]\n\t"
        "# A[19] * B\n\t"
        "ldr	r8, [%[a], #76]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #76]\n\t"
        "# A[20] * B\n\t"
        "ldr	r8, [%[a], #80]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #80]\n\t"
        "# A[21] * B\n\t"
        "ldr	r8, [%[a], #84]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #84]\n\t"
        "# A[22] * B\n\t"
        "ldr	r8, [%[a], #88]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #88]\n\t"
        "# A[23] * B\n\t"
        "ldr	r8, [%[a], #92]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #92]\n\t"
        "# A[24] * B\n\t"
        "ldr	r8, [%[a], #96]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #96]\n\t"
        "# A[25] * B\n\t"
        "ldr	r8, [%[a], #100]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #100]\n\t"
        "# A[26] * B\n\t"
        "ldr	r8, [%[a], #104]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #104]\n\t"
        "# A[27] * B\n\t"
        "ldr	r8, [%[a], #108]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #108]\n\t"
        "# A[28] * B\n\t"
        "ldr	r8, [%[a], #112]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #112]\n\t"
        "# A[29] * B\n\t"
        "ldr	r8, [%[a], #116]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #116]\n\t"
        "# A[30] * B\n\t"
        "ldr	r8, [%[a], #120]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #120]\n\t"
        "# A[31] * B\n\t"
        "ldr	r8, [%[a], #124]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #124]\n\t"
        "# A[32] * B\n\t"
        "ldr	r8, [%[a], #128]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #128]\n\t"
        "# A[33] * B\n\t"
        "ldr	r8, [%[a], #132]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #132]\n\t"
        "# A[34] * B\n\t"
        "ldr	r8, [%[a], #136]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #136]\n\t"
        "# A[35] * B\n\t"
        "ldr	r8, [%[a], #140]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #140]\n\t"
        "# A[36] * B\n\t"
        "ldr	r8, [%[a], #144]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #144]\n\t"
        "# A[37] * B\n\t"
        "ldr	r8, [%[a], #148]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #148]\n\t"
        "# A[38] * B\n\t"
        "ldr	r8, [%[a], #152]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #152]\n\t"
        "# A[39] * B\n\t"
        "ldr	r8, [%[a], #156]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #156]\n\t"
        "# A[40] * B\n\t"
        "ldr	r8, [%[a], #160]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #160]\n\t"
        "# A[41] * B\n\t"
        "ldr	r8, [%[a], #164]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #164]\n\t"
        "# A[42] * B\n\t"
        "ldr	r8, [%[a], #168]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #168]\n\t"
        "# A[43] * B\n\t"
        "ldr	r8, [%[a], #172]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #172]\n\t"
        "# A[44] * B\n\t"
        "ldr	r8, [%[a], #176]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #176]\n\t"
        "# A[45] * B\n\t"
        "ldr	r8, [%[a], #180]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #180]\n\t"
        "# A[46] * B\n\t"
        "ldr	r8, [%[a], #184]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #184]\n\t"
        "# A[47] * B\n\t"
        "ldr	r8, [%[a], #188]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r5, [%[r], #188]\n\t"
        "str	r3, [%[r], #192]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#endif
}

/* Divide the double width number (d1|d0) by the dividend. (d1|d0 / div)
 *
 * d1   The high order half of the number to divide.
 * d0   The low order half of the number to divide.
 * div  The dividend.
 * returns the result of the division.
 *
 * Note that this is an approximate div. It may give an answer 1 larger.
 */
static sp_digit div_3072_word_48(sp_digit d1, sp_digit d0, sp_digit div)
{
    sp_digit r = 0;

    __asm__ __volatile__ (
        "lsr	r5, %[div], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[d0]\n\t"
        "mov	r7, %[d1]\n\t"
        "# Do top 32\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "# Next 30 bits\n\t"
        "mov	r4, #29\n\t"
        "1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r7, r7, r7\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "subs	r8, %[div], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r)
        : [d1] "r" (d1), [d0] "r" (d0), [div] "r" (div)
        : "r4", "r5", "r6", "r7", "r8"
    );
    return r;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static int32_t sp_3072_cmp_48(sp_digit* a, sp_digit* b)
{
    sp_digit r = -1;
    sp_digit one = 1;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "mov	r6, #188\n\t"
        "1:\n\t"
        "ldr	r4, [%[a], r6]\n\t"
        "ldr	r5, [%[b], r6]\n\t"
        "and	r4, r4, r3\n\t"
        "and	r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "sub	r6, r6, #4\n\t"
        "bcc	1b\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#else
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "ldr		r4, [%[a], #188]\n\t"
        "ldr		r5, [%[b], #188]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #184]\n\t"
        "ldr		r5, [%[b], #184]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #180]\n\t"
        "ldr		r5, [%[b], #180]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #176]\n\t"
        "ldr		r5, [%[b], #176]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #172]\n\t"
        "ldr		r5, [%[b], #172]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #168]\n\t"
        "ldr		r5, [%[b], #168]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #164]\n\t"
        "ldr		r5, [%[b], #164]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #160]\n\t"
        "ldr		r5, [%[b], #160]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #156]\n\t"
        "ldr		r5, [%[b], #156]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #152]\n\t"
        "ldr		r5, [%[b], #152]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #148]\n\t"
        "ldr		r5, [%[b], #148]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #144]\n\t"
        "ldr		r5, [%[b], #144]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #140]\n\t"
        "ldr		r5, [%[b], #140]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #136]\n\t"
        "ldr		r5, [%[b], #136]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #132]\n\t"
        "ldr		r5, [%[b], #132]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #128]\n\t"
        "ldr		r5, [%[b], #128]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #124]\n\t"
        "ldr		r5, [%[b], #124]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #120]\n\t"
        "ldr		r5, [%[b], #120]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #116]\n\t"
        "ldr		r5, [%[b], #116]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #112]\n\t"
        "ldr		r5, [%[b], #112]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #108]\n\t"
        "ldr		r5, [%[b], #108]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #104]\n\t"
        "ldr		r5, [%[b], #104]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #100]\n\t"
        "ldr		r5, [%[b], #100]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #96]\n\t"
        "ldr		r5, [%[b], #96]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #92]\n\t"
        "ldr		r5, [%[b], #92]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #88]\n\t"
        "ldr		r5, [%[b], #88]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #84]\n\t"
        "ldr		r5, [%[b], #84]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #80]\n\t"
        "ldr		r5, [%[b], #80]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #76]\n\t"
        "ldr		r5, [%[b], #76]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #72]\n\t"
        "ldr		r5, [%[b], #72]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #68]\n\t"
        "ldr		r5, [%[b], #68]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #64]\n\t"
        "ldr		r5, [%[b], #64]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #60]\n\t"
        "ldr		r5, [%[b], #60]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #56]\n\t"
        "ldr		r5, [%[b], #56]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #52]\n\t"
        "ldr		r5, [%[b], #52]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #48]\n\t"
        "ldr		r5, [%[b], #48]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #44]\n\t"
        "ldr		r5, [%[b], #44]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #40]\n\t"
        "ldr		r5, [%[b], #40]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #36]\n\t"
        "ldr		r5, [%[b], #36]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #32]\n\t"
        "ldr		r5, [%[b], #32]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #28]\n\t"
        "ldr		r5, [%[b], #28]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #24]\n\t"
        "ldr		r5, [%[b], #24]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #20]\n\t"
        "ldr		r5, [%[b], #20]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #16]\n\t"
        "ldr		r5, [%[b], #16]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #12]\n\t"
        "ldr		r5, [%[b], #12]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #8]\n\t"
        "ldr		r5, [%[b], #8]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #4]\n\t"
        "ldr		r5, [%[b], #4]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #0]\n\t"
        "ldr		r5, [%[b], #0]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#endif

    return r;
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_div_48(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[96], t2[49];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[47];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 48);
    for (i=47; i>=0; i--) {
        r1 = div_3072_word_48(t1[48 + i], t1[48 + i - 1], div);

        sp_3072_mul_d_48(t2, d, r1);
        t1[48 + i] += sp_3072_sub_in_place_48(&t1[i], t2);
        t1[48 + i] -= t2[48];
        sp_3072_mask_48(t2, d, t1[48 + i]);
        t1[48 + i] += sp_3072_add_48(&t1[i], &t1[i], t2);
        sp_3072_mask_48(t2, d, t1[48 + i]);
        t1[48 + i] += sp_3072_add_48(&t1[i], &t1[i], t2);
    }

    r1 = sp_3072_cmp_48(t1, d) >= 0;
    sp_3072_cond_sub_48(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_mod_48(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_48(a, m, NULL, r);
}

#ifdef WOLFSSL_SP_SMALL
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_48(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[16][96];
#else
    sp_digit* t[16];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 16 * 96, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<16; i++)
            t[i] = td + i * 96;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_48(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 48);
        if (reduceA) {
            err = sp_3072_mod_48(t[1] + 48, a, m);
            if (err == MP_OKAY)
                err = sp_3072_mod_48(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 48, a, sizeof(sp_digit) * 48);
            err = sp_3072_mod_48(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_48(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_48(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_48(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_48(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_48(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_48(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_48(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_48(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_48(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_48(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_48(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_48(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_48(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_48(t[15], t[ 8], t[ 7], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 28;
        n <<= 4;
        c = 28;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 48);
        for (; i>=0 || c>=4; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 28;
                n <<= 4;
                c = 28;
            }
            else if (c < 4) {
                y = n >> 28;
                n = e[i--];
                c = 4 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 28) & 0xf;
                n <<= 4;
                c -= 4;
            }

            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);

            sp_3072_mont_mul_48(r, r, t[y], m, mp);
        }

        XMEMSET(&r[48], 0, sizeof(sp_digit) * 48);
        sp_3072_mont_reduce_48(r, m, mp);

        mask = 0 - (sp_3072_cmp_48(r, m) >= 0);
        sp_3072_cond_sub_48(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#else
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_48(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][96];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 96, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 96;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_48(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 48);
        if (reduceA) {
            err = sp_3072_mod_48(t[1] + 48, a, m);
            if (err == MP_OKAY)
                err = sp_3072_mod_48(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 48, a, sizeof(sp_digit) * 48);
            err = sp_3072_mod_48(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_48(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_48(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_48(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_48(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_48(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_48(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_48(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_48(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_48(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_48(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_48(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_48(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_48(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_48(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_48(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_48(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_48(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_48(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_48(t[20], t[10], m, mp);
        sp_3072_mont_mul_48(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_48(t[22], t[11], m, mp);
        sp_3072_mont_mul_48(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_48(t[24], t[12], m, mp);
        sp_3072_mont_mul_48(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_48(t[26], t[13], m, mp);
        sp_3072_mont_mul_48(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_48(t[28], t[14], m, mp);
        sp_3072_mont_mul_48(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_48(t[30], t[15], m, mp);
        sp_3072_mont_mul_48(t[31], t[16], t[15], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 27;
        n <<= 5;
        c = 27;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 48);
        for (; i>=0 || c>=5; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 27;
                n <<= 5;
                c = 27;
            }
            else if (c < 5) {
                y = n >> 27;
                n = e[i--];
                c = 5 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 27) & 0x1f;
                n <<= 5;
                c -= 5;
            }

            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);
            sp_3072_mont_sqr_48(r, r, m, mp);

            sp_3072_mont_mul_48(r, r, t[y], m, mp);
        }
        y = e[0] & 0x1;
        sp_3072_mont_sqr_48(r, r, m, mp);
        sp_3072_mont_mul_48(r, r, t[y], m, mp);

        XMEMSET(&r[48], 0, sizeof(sp_digit) * 48);
        sp_3072_mont_reduce_48(r, m, mp);

        mask = 0 - (sp_3072_cmp_48(r, m) >= 0);
        sp_3072_cond_sub_48(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#endif /* WOLFSSL_SP_SMALL */

#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 3072 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_3072_mont_norm_96(sp_digit* r, sp_digit* m)
{
    XMEMSET(r, 0, sizeof(sp_digit) * 96);

    /* r = 2^n mod m */
    sp_3072_sub_in_place_96(r, m);
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static sp_digit sp_3072_cond_sub_96(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit m)
{
    sp_digit c = 0;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r9, #0\n\t"
        "mov	r8, #0\n\t"
        "1:\n\t"
        "subs	%[c], r9, %[c]\n\t"
        "ldr	r4, [%[a], r8]\n\t"
        "ldr	r5, [%[b], r8]\n\t"
        "and	r5, r5, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbc	%[c], r9, r9\n\t"
        "str	r4, [%[r], r8]\n\t"
        "add	r8, r8, #4\n\t"
        "cmp	r8, #384\n\t"
        "blt	1b\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#else
    __asm__ __volatile__ (

        "mov	r9, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r5, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "subs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r6, [%[r], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r5, [%[b], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r5, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r6, [%[r], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r5, [%[b], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "ldr	r4, [%[a], #32]\n\t"
        "ldr	r6, [%[a], #36]\n\t"
        "ldr	r5, [%[b], #32]\n\t"
        "ldr	r7, [%[b], #36]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "str	r6, [%[r], #36]\n\t"
        "ldr	r4, [%[a], #40]\n\t"
        "ldr	r6, [%[a], #44]\n\t"
        "ldr	r5, [%[b], #40]\n\t"
        "ldr	r7, [%[b], #44]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #40]\n\t"
        "str	r6, [%[r], #44]\n\t"
        "ldr	r4, [%[a], #48]\n\t"
        "ldr	r6, [%[a], #52]\n\t"
        "ldr	r5, [%[b], #48]\n\t"
        "ldr	r7, [%[b], #52]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #48]\n\t"
        "str	r6, [%[r], #52]\n\t"
        "ldr	r4, [%[a], #56]\n\t"
        "ldr	r6, [%[a], #60]\n\t"
        "ldr	r5, [%[b], #56]\n\t"
        "ldr	r7, [%[b], #60]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r6, [%[r], #60]\n\t"
        "ldr	r4, [%[a], #64]\n\t"
        "ldr	r6, [%[a], #68]\n\t"
        "ldr	r5, [%[b], #64]\n\t"
        "ldr	r7, [%[b], #68]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #64]\n\t"
        "str	r6, [%[r], #68]\n\t"
        "ldr	r4, [%[a], #72]\n\t"
        "ldr	r6, [%[a], #76]\n\t"
        "ldr	r5, [%[b], #72]\n\t"
        "ldr	r7, [%[b], #76]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #72]\n\t"
        "str	r6, [%[r], #76]\n\t"
        "ldr	r4, [%[a], #80]\n\t"
        "ldr	r6, [%[a], #84]\n\t"
        "ldr	r5, [%[b], #80]\n\t"
        "ldr	r7, [%[b], #84]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #80]\n\t"
        "str	r6, [%[r], #84]\n\t"
        "ldr	r4, [%[a], #88]\n\t"
        "ldr	r6, [%[a], #92]\n\t"
        "ldr	r5, [%[b], #88]\n\t"
        "ldr	r7, [%[b], #92]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #88]\n\t"
        "str	r6, [%[r], #92]\n\t"
        "ldr	r4, [%[a], #96]\n\t"
        "ldr	r6, [%[a], #100]\n\t"
        "ldr	r5, [%[b], #96]\n\t"
        "ldr	r7, [%[b], #100]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #96]\n\t"
        "str	r6, [%[r], #100]\n\t"
        "ldr	r4, [%[a], #104]\n\t"
        "ldr	r6, [%[a], #108]\n\t"
        "ldr	r5, [%[b], #104]\n\t"
        "ldr	r7, [%[b], #108]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #104]\n\t"
        "str	r6, [%[r], #108]\n\t"
        "ldr	r4, [%[a], #112]\n\t"
        "ldr	r6, [%[a], #116]\n\t"
        "ldr	r5, [%[b], #112]\n\t"
        "ldr	r7, [%[b], #116]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #112]\n\t"
        "str	r6, [%[r], #116]\n\t"
        "ldr	r4, [%[a], #120]\n\t"
        "ldr	r6, [%[a], #124]\n\t"
        "ldr	r5, [%[b], #120]\n\t"
        "ldr	r7, [%[b], #124]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #120]\n\t"
        "str	r6, [%[r], #124]\n\t"
        "ldr	r4, [%[a], #128]\n\t"
        "ldr	r6, [%[a], #132]\n\t"
        "ldr	r5, [%[b], #128]\n\t"
        "ldr	r7, [%[b], #132]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #128]\n\t"
        "str	r6, [%[r], #132]\n\t"
        "ldr	r4, [%[a], #136]\n\t"
        "ldr	r6, [%[a], #140]\n\t"
        "ldr	r5, [%[b], #136]\n\t"
        "ldr	r7, [%[b], #140]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #136]\n\t"
        "str	r6, [%[r], #140]\n\t"
        "ldr	r4, [%[a], #144]\n\t"
        "ldr	r6, [%[a], #148]\n\t"
        "ldr	r5, [%[b], #144]\n\t"
        "ldr	r7, [%[b], #148]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #144]\n\t"
        "str	r6, [%[r], #148]\n\t"
        "ldr	r4, [%[a], #152]\n\t"
        "ldr	r6, [%[a], #156]\n\t"
        "ldr	r5, [%[b], #152]\n\t"
        "ldr	r7, [%[b], #156]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #152]\n\t"
        "str	r6, [%[r], #156]\n\t"
        "ldr	r4, [%[a], #160]\n\t"
        "ldr	r6, [%[a], #164]\n\t"
        "ldr	r5, [%[b], #160]\n\t"
        "ldr	r7, [%[b], #164]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #160]\n\t"
        "str	r6, [%[r], #164]\n\t"
        "ldr	r4, [%[a], #168]\n\t"
        "ldr	r6, [%[a], #172]\n\t"
        "ldr	r5, [%[b], #168]\n\t"
        "ldr	r7, [%[b], #172]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #168]\n\t"
        "str	r6, [%[r], #172]\n\t"
        "ldr	r4, [%[a], #176]\n\t"
        "ldr	r6, [%[a], #180]\n\t"
        "ldr	r5, [%[b], #176]\n\t"
        "ldr	r7, [%[b], #180]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #176]\n\t"
        "str	r6, [%[r], #180]\n\t"
        "ldr	r4, [%[a], #184]\n\t"
        "ldr	r6, [%[a], #188]\n\t"
        "ldr	r5, [%[b], #184]\n\t"
        "ldr	r7, [%[b], #188]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #184]\n\t"
        "str	r6, [%[r], #188]\n\t"
        "ldr	r4, [%[a], #192]\n\t"
        "ldr	r6, [%[a], #196]\n\t"
        "ldr	r5, [%[b], #192]\n\t"
        "ldr	r7, [%[b], #196]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #192]\n\t"
        "str	r6, [%[r], #196]\n\t"
        "ldr	r4, [%[a], #200]\n\t"
        "ldr	r6, [%[a], #204]\n\t"
        "ldr	r5, [%[b], #200]\n\t"
        "ldr	r7, [%[b], #204]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #200]\n\t"
        "str	r6, [%[r], #204]\n\t"
        "ldr	r4, [%[a], #208]\n\t"
        "ldr	r6, [%[a], #212]\n\t"
        "ldr	r5, [%[b], #208]\n\t"
        "ldr	r7, [%[b], #212]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #208]\n\t"
        "str	r6, [%[r], #212]\n\t"
        "ldr	r4, [%[a], #216]\n\t"
        "ldr	r6, [%[a], #220]\n\t"
        "ldr	r5, [%[b], #216]\n\t"
        "ldr	r7, [%[b], #220]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #216]\n\t"
        "str	r6, [%[r], #220]\n\t"
        "ldr	r4, [%[a], #224]\n\t"
        "ldr	r6, [%[a], #228]\n\t"
        "ldr	r5, [%[b], #224]\n\t"
        "ldr	r7, [%[b], #228]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #224]\n\t"
        "str	r6, [%[r], #228]\n\t"
        "ldr	r4, [%[a], #232]\n\t"
        "ldr	r6, [%[a], #236]\n\t"
        "ldr	r5, [%[b], #232]\n\t"
        "ldr	r7, [%[b], #236]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #232]\n\t"
        "str	r6, [%[r], #236]\n\t"
        "ldr	r4, [%[a], #240]\n\t"
        "ldr	r6, [%[a], #244]\n\t"
        "ldr	r5, [%[b], #240]\n\t"
        "ldr	r7, [%[b], #244]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #240]\n\t"
        "str	r6, [%[r], #244]\n\t"
        "ldr	r4, [%[a], #248]\n\t"
        "ldr	r6, [%[a], #252]\n\t"
        "ldr	r5, [%[b], #248]\n\t"
        "ldr	r7, [%[b], #252]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #248]\n\t"
        "str	r6, [%[r], #252]\n\t"
        "ldr	r4, [%[a], #256]\n\t"
        "ldr	r6, [%[a], #260]\n\t"
        "ldr	r5, [%[b], #256]\n\t"
        "ldr	r7, [%[b], #260]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #256]\n\t"
        "str	r6, [%[r], #260]\n\t"
        "ldr	r4, [%[a], #264]\n\t"
        "ldr	r6, [%[a], #268]\n\t"
        "ldr	r5, [%[b], #264]\n\t"
        "ldr	r7, [%[b], #268]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #264]\n\t"
        "str	r6, [%[r], #268]\n\t"
        "ldr	r4, [%[a], #272]\n\t"
        "ldr	r6, [%[a], #276]\n\t"
        "ldr	r5, [%[b], #272]\n\t"
        "ldr	r7, [%[b], #276]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #272]\n\t"
        "str	r6, [%[r], #276]\n\t"
        "ldr	r4, [%[a], #280]\n\t"
        "ldr	r6, [%[a], #284]\n\t"
        "ldr	r5, [%[b], #280]\n\t"
        "ldr	r7, [%[b], #284]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #280]\n\t"
        "str	r6, [%[r], #284]\n\t"
        "ldr	r4, [%[a], #288]\n\t"
        "ldr	r6, [%[a], #292]\n\t"
        "ldr	r5, [%[b], #288]\n\t"
        "ldr	r7, [%[b], #292]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #288]\n\t"
        "str	r6, [%[r], #292]\n\t"
        "ldr	r4, [%[a], #296]\n\t"
        "ldr	r6, [%[a], #300]\n\t"
        "ldr	r5, [%[b], #296]\n\t"
        "ldr	r7, [%[b], #300]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #296]\n\t"
        "str	r6, [%[r], #300]\n\t"
        "ldr	r4, [%[a], #304]\n\t"
        "ldr	r6, [%[a], #308]\n\t"
        "ldr	r5, [%[b], #304]\n\t"
        "ldr	r7, [%[b], #308]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #304]\n\t"
        "str	r6, [%[r], #308]\n\t"
        "ldr	r4, [%[a], #312]\n\t"
        "ldr	r6, [%[a], #316]\n\t"
        "ldr	r5, [%[b], #312]\n\t"
        "ldr	r7, [%[b], #316]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #312]\n\t"
        "str	r6, [%[r], #316]\n\t"
        "ldr	r4, [%[a], #320]\n\t"
        "ldr	r6, [%[a], #324]\n\t"
        "ldr	r5, [%[b], #320]\n\t"
        "ldr	r7, [%[b], #324]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #320]\n\t"
        "str	r6, [%[r], #324]\n\t"
        "ldr	r4, [%[a], #328]\n\t"
        "ldr	r6, [%[a], #332]\n\t"
        "ldr	r5, [%[b], #328]\n\t"
        "ldr	r7, [%[b], #332]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #328]\n\t"
        "str	r6, [%[r], #332]\n\t"
        "ldr	r4, [%[a], #336]\n\t"
        "ldr	r6, [%[a], #340]\n\t"
        "ldr	r5, [%[b], #336]\n\t"
        "ldr	r7, [%[b], #340]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #336]\n\t"
        "str	r6, [%[r], #340]\n\t"
        "ldr	r4, [%[a], #344]\n\t"
        "ldr	r6, [%[a], #348]\n\t"
        "ldr	r5, [%[b], #344]\n\t"
        "ldr	r7, [%[b], #348]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #344]\n\t"
        "str	r6, [%[r], #348]\n\t"
        "ldr	r4, [%[a], #352]\n\t"
        "ldr	r6, [%[a], #356]\n\t"
        "ldr	r5, [%[b], #352]\n\t"
        "ldr	r7, [%[b], #356]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #352]\n\t"
        "str	r6, [%[r], #356]\n\t"
        "ldr	r4, [%[a], #360]\n\t"
        "ldr	r6, [%[a], #364]\n\t"
        "ldr	r5, [%[b], #360]\n\t"
        "ldr	r7, [%[b], #364]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #360]\n\t"
        "str	r6, [%[r], #364]\n\t"
        "ldr	r4, [%[a], #368]\n\t"
        "ldr	r6, [%[a], #372]\n\t"
        "ldr	r5, [%[b], #368]\n\t"
        "ldr	r7, [%[b], #372]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #368]\n\t"
        "str	r6, [%[r], #372]\n\t"
        "ldr	r4, [%[a], #376]\n\t"
        "ldr	r6, [%[a], #380]\n\t"
        "ldr	r5, [%[b], #376]\n\t"
        "ldr	r7, [%[b], #380]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #376]\n\t"
        "str	r6, [%[r], #380]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#endif /* WOLFSSL_SP_SMALL */

    return c;
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
SP_NOINLINE static void sp_3072_mont_reduce_96(sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit ca = 0;

    __asm__ __volatile__ (
        "# i = 0\n\t"
        "mov	r12, #0\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "ldr	r14, [%[a], #4]\n\t"
        "\n1:\n\t"
        "# mu = a[i] * mp\n\t"
        "mul	r8, %[mp], r10\n\t"
        "# a[i+0] += m[0] * mu\n\t"
        "ldr	r7, [%[m], #0]\n\t"
        "ldr	r9, [%[a], #0]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r10, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "# a[i+1] += m[1] * mu\n\t"
        "ldr	r7, [%[m], #4]\n\t"
        "ldr	r9, [%[a], #4]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r14, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r10, r10, r5\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+2] += m[2] * mu\n\t"
        "ldr	r7, [%[m], #8]\n\t"
        "ldr	r14, [%[a], #8]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r14, r14, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r14, r14, r4\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+3] += m[3] * mu\n\t"
        "ldr	r7, [%[m], #12]\n\t"
        "ldr	r9, [%[a], #12]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #12]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+4] += m[4] * mu\n\t"
        "ldr	r7, [%[m], #16]\n\t"
        "ldr	r9, [%[a], #16]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #16]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+5] += m[5] * mu\n\t"
        "ldr	r7, [%[m], #20]\n\t"
        "ldr	r9, [%[a], #20]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #20]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+6] += m[6] * mu\n\t"
        "ldr	r7, [%[m], #24]\n\t"
        "ldr	r9, [%[a], #24]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #24]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+7] += m[7] * mu\n\t"
        "ldr	r7, [%[m], #28]\n\t"
        "ldr	r9, [%[a], #28]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #28]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+8] += m[8] * mu\n\t"
        "ldr	r7, [%[m], #32]\n\t"
        "ldr	r9, [%[a], #32]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #32]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+9] += m[9] * mu\n\t"
        "ldr	r7, [%[m], #36]\n\t"
        "ldr	r9, [%[a], #36]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #36]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+10] += m[10] * mu\n\t"
        "ldr	r7, [%[m], #40]\n\t"
        "ldr	r9, [%[a], #40]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #40]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+11] += m[11] * mu\n\t"
        "ldr	r7, [%[m], #44]\n\t"
        "ldr	r9, [%[a], #44]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #44]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+12] += m[12] * mu\n\t"
        "ldr	r7, [%[m], #48]\n\t"
        "ldr	r9, [%[a], #48]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #48]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+13] += m[13] * mu\n\t"
        "ldr	r7, [%[m], #52]\n\t"
        "ldr	r9, [%[a], #52]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #52]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+14] += m[14] * mu\n\t"
        "ldr	r7, [%[m], #56]\n\t"
        "ldr	r9, [%[a], #56]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #56]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+15] += m[15] * mu\n\t"
        "ldr	r7, [%[m], #60]\n\t"
        "ldr	r9, [%[a], #60]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #60]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+16] += m[16] * mu\n\t"
        "ldr	r7, [%[m], #64]\n\t"
        "ldr	r9, [%[a], #64]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #64]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+17] += m[17] * mu\n\t"
        "ldr	r7, [%[m], #68]\n\t"
        "ldr	r9, [%[a], #68]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #68]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+18] += m[18] * mu\n\t"
        "ldr	r7, [%[m], #72]\n\t"
        "ldr	r9, [%[a], #72]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #72]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+19] += m[19] * mu\n\t"
        "ldr	r7, [%[m], #76]\n\t"
        "ldr	r9, [%[a], #76]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #76]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+20] += m[20] * mu\n\t"
        "ldr	r7, [%[m], #80]\n\t"
        "ldr	r9, [%[a], #80]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #80]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+21] += m[21] * mu\n\t"
        "ldr	r7, [%[m], #84]\n\t"
        "ldr	r9, [%[a], #84]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #84]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+22] += m[22] * mu\n\t"
        "ldr	r7, [%[m], #88]\n\t"
        "ldr	r9, [%[a], #88]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #88]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+23] += m[23] * mu\n\t"
        "ldr	r7, [%[m], #92]\n\t"
        "ldr	r9, [%[a], #92]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #92]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+24] += m[24] * mu\n\t"
        "ldr	r7, [%[m], #96]\n\t"
        "ldr	r9, [%[a], #96]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #96]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+25] += m[25] * mu\n\t"
        "ldr	r7, [%[m], #100]\n\t"
        "ldr	r9, [%[a], #100]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #100]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+26] += m[26] * mu\n\t"
        "ldr	r7, [%[m], #104]\n\t"
        "ldr	r9, [%[a], #104]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #104]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+27] += m[27] * mu\n\t"
        "ldr	r7, [%[m], #108]\n\t"
        "ldr	r9, [%[a], #108]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #108]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+28] += m[28] * mu\n\t"
        "ldr	r7, [%[m], #112]\n\t"
        "ldr	r9, [%[a], #112]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #112]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+29] += m[29] * mu\n\t"
        "ldr	r7, [%[m], #116]\n\t"
        "ldr	r9, [%[a], #116]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #116]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+30] += m[30] * mu\n\t"
        "ldr	r7, [%[m], #120]\n\t"
        "ldr	r9, [%[a], #120]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #120]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+31] += m[31] * mu\n\t"
        "ldr	r7, [%[m], #124]\n\t"
        "ldr	r9, [%[a], #124]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #124]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+32] += m[32] * mu\n\t"
        "ldr	r7, [%[m], #128]\n\t"
        "ldr	r9, [%[a], #128]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #128]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+33] += m[33] * mu\n\t"
        "ldr	r7, [%[m], #132]\n\t"
        "ldr	r9, [%[a], #132]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #132]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+34] += m[34] * mu\n\t"
        "ldr	r7, [%[m], #136]\n\t"
        "ldr	r9, [%[a], #136]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #136]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+35] += m[35] * mu\n\t"
        "ldr	r7, [%[m], #140]\n\t"
        "ldr	r9, [%[a], #140]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #140]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+36] += m[36] * mu\n\t"
        "ldr	r7, [%[m], #144]\n\t"
        "ldr	r9, [%[a], #144]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #144]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+37] += m[37] * mu\n\t"
        "ldr	r7, [%[m], #148]\n\t"
        "ldr	r9, [%[a], #148]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #148]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+38] += m[38] * mu\n\t"
        "ldr	r7, [%[m], #152]\n\t"
        "ldr	r9, [%[a], #152]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #152]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+39] += m[39] * mu\n\t"
        "ldr	r7, [%[m], #156]\n\t"
        "ldr	r9, [%[a], #156]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #156]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+40] += m[40] * mu\n\t"
        "ldr	r7, [%[m], #160]\n\t"
        "ldr	r9, [%[a], #160]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #160]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+41] += m[41] * mu\n\t"
        "ldr	r7, [%[m], #164]\n\t"
        "ldr	r9, [%[a], #164]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #164]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+42] += m[42] * mu\n\t"
        "ldr	r7, [%[m], #168]\n\t"
        "ldr	r9, [%[a], #168]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #168]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+43] += m[43] * mu\n\t"
        "ldr	r7, [%[m], #172]\n\t"
        "ldr	r9, [%[a], #172]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #172]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+44] += m[44] * mu\n\t"
        "ldr	r7, [%[m], #176]\n\t"
        "ldr	r9, [%[a], #176]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #176]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+45] += m[45] * mu\n\t"
        "ldr	r7, [%[m], #180]\n\t"
        "ldr	r9, [%[a], #180]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #180]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+46] += m[46] * mu\n\t"
        "ldr	r7, [%[m], #184]\n\t"
        "ldr	r9, [%[a], #184]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #184]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+47] += m[47] * mu\n\t"
        "ldr	r7, [%[m], #188]\n\t"
        "ldr	r9, [%[a], #188]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #188]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+48] += m[48] * mu\n\t"
        "ldr	r7, [%[m], #192]\n\t"
        "ldr	r9, [%[a], #192]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #192]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+49] += m[49] * mu\n\t"
        "ldr	r7, [%[m], #196]\n\t"
        "ldr	r9, [%[a], #196]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #196]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+50] += m[50] * mu\n\t"
        "ldr	r7, [%[m], #200]\n\t"
        "ldr	r9, [%[a], #200]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #200]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+51] += m[51] * mu\n\t"
        "ldr	r7, [%[m], #204]\n\t"
        "ldr	r9, [%[a], #204]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #204]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+52] += m[52] * mu\n\t"
        "ldr	r7, [%[m], #208]\n\t"
        "ldr	r9, [%[a], #208]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #208]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+53] += m[53] * mu\n\t"
        "ldr	r7, [%[m], #212]\n\t"
        "ldr	r9, [%[a], #212]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #212]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+54] += m[54] * mu\n\t"
        "ldr	r7, [%[m], #216]\n\t"
        "ldr	r9, [%[a], #216]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #216]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+55] += m[55] * mu\n\t"
        "ldr	r7, [%[m], #220]\n\t"
        "ldr	r9, [%[a], #220]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #220]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+56] += m[56] * mu\n\t"
        "ldr	r7, [%[m], #224]\n\t"
        "ldr	r9, [%[a], #224]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #224]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+57] += m[57] * mu\n\t"
        "ldr	r7, [%[m], #228]\n\t"
        "ldr	r9, [%[a], #228]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #228]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+58] += m[58] * mu\n\t"
        "ldr	r7, [%[m], #232]\n\t"
        "ldr	r9, [%[a], #232]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #232]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+59] += m[59] * mu\n\t"
        "ldr	r7, [%[m], #236]\n\t"
        "ldr	r9, [%[a], #236]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #236]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+60] += m[60] * mu\n\t"
        "ldr	r7, [%[m], #240]\n\t"
        "ldr	r9, [%[a], #240]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #240]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+61] += m[61] * mu\n\t"
        "ldr	r7, [%[m], #244]\n\t"
        "ldr	r9, [%[a], #244]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #244]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+62] += m[62] * mu\n\t"
        "ldr	r7, [%[m], #248]\n\t"
        "ldr	r9, [%[a], #248]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #248]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+63] += m[63] * mu\n\t"
        "ldr	r7, [%[m], #252]\n\t"
        "ldr	r9, [%[a], #252]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #252]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+64] += m[64] * mu\n\t"
        "ldr	r7, [%[m], #256]\n\t"
        "ldr	r9, [%[a], #256]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #256]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+65] += m[65] * mu\n\t"
        "ldr	r7, [%[m], #260]\n\t"
        "ldr	r9, [%[a], #260]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #260]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+66] += m[66] * mu\n\t"
        "ldr	r7, [%[m], #264]\n\t"
        "ldr	r9, [%[a], #264]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #264]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+67] += m[67] * mu\n\t"
        "ldr	r7, [%[m], #268]\n\t"
        "ldr	r9, [%[a], #268]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #268]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+68] += m[68] * mu\n\t"
        "ldr	r7, [%[m], #272]\n\t"
        "ldr	r9, [%[a], #272]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #272]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+69] += m[69] * mu\n\t"
        "ldr	r7, [%[m], #276]\n\t"
        "ldr	r9, [%[a], #276]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #276]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+70] += m[70] * mu\n\t"
        "ldr	r7, [%[m], #280]\n\t"
        "ldr	r9, [%[a], #280]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #280]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+71] += m[71] * mu\n\t"
        "ldr	r7, [%[m], #284]\n\t"
        "ldr	r9, [%[a], #284]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #284]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+72] += m[72] * mu\n\t"
        "ldr	r7, [%[m], #288]\n\t"
        "ldr	r9, [%[a], #288]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #288]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+73] += m[73] * mu\n\t"
        "ldr	r7, [%[m], #292]\n\t"
        "ldr	r9, [%[a], #292]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #292]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+74] += m[74] * mu\n\t"
        "ldr	r7, [%[m], #296]\n\t"
        "ldr	r9, [%[a], #296]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #296]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+75] += m[75] * mu\n\t"
        "ldr	r7, [%[m], #300]\n\t"
        "ldr	r9, [%[a], #300]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #300]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+76] += m[76] * mu\n\t"
        "ldr	r7, [%[m], #304]\n\t"
        "ldr	r9, [%[a], #304]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #304]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+77] += m[77] * mu\n\t"
        "ldr	r7, [%[m], #308]\n\t"
        "ldr	r9, [%[a], #308]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #308]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+78] += m[78] * mu\n\t"
        "ldr	r7, [%[m], #312]\n\t"
        "ldr	r9, [%[a], #312]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #312]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+79] += m[79] * mu\n\t"
        "ldr	r7, [%[m], #316]\n\t"
        "ldr	r9, [%[a], #316]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #316]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+80] += m[80] * mu\n\t"
        "ldr	r7, [%[m], #320]\n\t"
        "ldr	r9, [%[a], #320]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #320]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+81] += m[81] * mu\n\t"
        "ldr	r7, [%[m], #324]\n\t"
        "ldr	r9, [%[a], #324]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #324]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+82] += m[82] * mu\n\t"
        "ldr	r7, [%[m], #328]\n\t"
        "ldr	r9, [%[a], #328]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #328]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+83] += m[83] * mu\n\t"
        "ldr	r7, [%[m], #332]\n\t"
        "ldr	r9, [%[a], #332]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #332]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+84] += m[84] * mu\n\t"
        "ldr	r7, [%[m], #336]\n\t"
        "ldr	r9, [%[a], #336]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #336]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+85] += m[85] * mu\n\t"
        "ldr	r7, [%[m], #340]\n\t"
        "ldr	r9, [%[a], #340]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #340]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+86] += m[86] * mu\n\t"
        "ldr	r7, [%[m], #344]\n\t"
        "ldr	r9, [%[a], #344]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #344]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+87] += m[87] * mu\n\t"
        "ldr	r7, [%[m], #348]\n\t"
        "ldr	r9, [%[a], #348]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #348]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+88] += m[88] * mu\n\t"
        "ldr	r7, [%[m], #352]\n\t"
        "ldr	r9, [%[a], #352]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #352]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+89] += m[89] * mu\n\t"
        "ldr	r7, [%[m], #356]\n\t"
        "ldr	r9, [%[a], #356]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #356]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+90] += m[90] * mu\n\t"
        "ldr	r7, [%[m], #360]\n\t"
        "ldr	r9, [%[a], #360]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #360]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+91] += m[91] * mu\n\t"
        "ldr	r7, [%[m], #364]\n\t"
        "ldr	r9, [%[a], #364]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #364]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+92] += m[92] * mu\n\t"
        "ldr	r7, [%[m], #368]\n\t"
        "ldr	r9, [%[a], #368]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #368]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+93] += m[93] * mu\n\t"
        "ldr	r7, [%[m], #372]\n\t"
        "ldr	r9, [%[a], #372]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #372]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+94] += m[94] * mu\n\t"
        "ldr	r7, [%[m], #376]\n\t"
        "ldr	r9, [%[a], #376]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #376]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+95] += m[95] * mu\n\t"
        "ldr	r7, [%[m], #380]\n\t"
        "ldr   r9, [%[a], #380]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r7, r7, %[ca]\n\t"
        "mov	%[ca], #0\n\t"
        "adc	%[ca], %[ca], %[ca]\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #380]\n\t"
        "ldr	r9, [%[a], #384]\n\t"
        "adcs	r9, r9, r7\n\t"
        "str	r9, [%[a], #384]\n\t"
        "adc	%[ca], %[ca], #0\n\t"
        "# i += 1\n\t"
        "add	%[a], %[a], #4\n\t"
        "add	r12, r12, #4\n\t"
        "cmp	r12, #384\n\t"
        "blt	1b\n\t"
        "str	r10, [%[a], #0]\n\t"
        "str	r14, [%[a], #4]\n\t"
        : [ca] "+r" (ca), [a] "+r" (a)
        : [m] "r" (m), [mp] "r" (mp)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    sp_3072_cond_sub_96(a - 96, a, m, (sp_digit)0 - ca);
}

/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_mul_96(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_96(r, a, b);
    sp_3072_mont_reduce_96(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_96(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_96(r, a);
    sp_3072_mont_reduce_96(r, m, mp);
}

/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
static void sp_3072_mul_d_96(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r5, r3, %[b], r8\n\t"
        "mov	r4, #0\n\t"
        "str	r5, [%[r]]\n\t"
        "mov	r5, #0\n\t"
        "mov	r9, #4\n\t"
        "1:\n\t"
        "ldr	r8, [%[a], r9]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], r9]\n\t"
        "mov	r3, r4\n\t"
        "mov	r4, r5\n\t"
        "mov	r5, #0\n\t"
        "add	r9, r9, #4\n\t"
        "cmp	r9, #384\n\t"
        "blt	1b\n\t"
        "str	r3, [%[r], #384]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#else
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r3, r4, %[b], r8\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[r]]\n\t"
        "# A[1] * B\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #4]\n\t"
        "# A[2] * B\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #8]\n\t"
        "# A[3] * B\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #12]\n\t"
        "# A[4] * B\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #16]\n\t"
        "# A[5] * B\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #20]\n\t"
        "# A[6] * B\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #24]\n\t"
        "# A[7] * B\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #28]\n\t"
        "# A[8] * B\n\t"
        "ldr	r8, [%[a], #32]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "# A[9] * B\n\t"
        "ldr	r8, [%[a], #36]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "# A[10] * B\n\t"
        "ldr	r8, [%[a], #40]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "# A[11] * B\n\t"
        "ldr	r8, [%[a], #44]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "# A[12] * B\n\t"
        "ldr	r8, [%[a], #48]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "# A[13] * B\n\t"
        "ldr	r8, [%[a], #52]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "# A[14] * B\n\t"
        "ldr	r8, [%[a], #56]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #56]\n\t"
        "# A[15] * B\n\t"
        "ldr	r8, [%[a], #60]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #60]\n\t"
        "# A[16] * B\n\t"
        "ldr	r8, [%[a], #64]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #64]\n\t"
        "# A[17] * B\n\t"
        "ldr	r8, [%[a], #68]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #68]\n\t"
        "# A[18] * B\n\t"
        "ldr	r8, [%[a], #72]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #72]\n\t"
        "# A[19] * B\n\t"
        "ldr	r8, [%[a], #76]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #76]\n\t"
        "# A[20] * B\n\t"
        "ldr	r8, [%[a], #80]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #80]\n\t"
        "# A[21] * B\n\t"
        "ldr	r8, [%[a], #84]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #84]\n\t"
        "# A[22] * B\n\t"
        "ldr	r8, [%[a], #88]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #88]\n\t"
        "# A[23] * B\n\t"
        "ldr	r8, [%[a], #92]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #92]\n\t"
        "# A[24] * B\n\t"
        "ldr	r8, [%[a], #96]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #96]\n\t"
        "# A[25] * B\n\t"
        "ldr	r8, [%[a], #100]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #100]\n\t"
        "# A[26] * B\n\t"
        "ldr	r8, [%[a], #104]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #104]\n\t"
        "# A[27] * B\n\t"
        "ldr	r8, [%[a], #108]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #108]\n\t"
        "# A[28] * B\n\t"
        "ldr	r8, [%[a], #112]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #112]\n\t"
        "# A[29] * B\n\t"
        "ldr	r8, [%[a], #116]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #116]\n\t"
        "# A[30] * B\n\t"
        "ldr	r8, [%[a], #120]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #120]\n\t"
        "# A[31] * B\n\t"
        "ldr	r8, [%[a], #124]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #124]\n\t"
        "# A[32] * B\n\t"
        "ldr	r8, [%[a], #128]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #128]\n\t"
        "# A[33] * B\n\t"
        "ldr	r8, [%[a], #132]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #132]\n\t"
        "# A[34] * B\n\t"
        "ldr	r8, [%[a], #136]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #136]\n\t"
        "# A[35] * B\n\t"
        "ldr	r8, [%[a], #140]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #140]\n\t"
        "# A[36] * B\n\t"
        "ldr	r8, [%[a], #144]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #144]\n\t"
        "# A[37] * B\n\t"
        "ldr	r8, [%[a], #148]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #148]\n\t"
        "# A[38] * B\n\t"
        "ldr	r8, [%[a], #152]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #152]\n\t"
        "# A[39] * B\n\t"
        "ldr	r8, [%[a], #156]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #156]\n\t"
        "# A[40] * B\n\t"
        "ldr	r8, [%[a], #160]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #160]\n\t"
        "# A[41] * B\n\t"
        "ldr	r8, [%[a], #164]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #164]\n\t"
        "# A[42] * B\n\t"
        "ldr	r8, [%[a], #168]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #168]\n\t"
        "# A[43] * B\n\t"
        "ldr	r8, [%[a], #172]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #172]\n\t"
        "# A[44] * B\n\t"
        "ldr	r8, [%[a], #176]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #176]\n\t"
        "# A[45] * B\n\t"
        "ldr	r8, [%[a], #180]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #180]\n\t"
        "# A[46] * B\n\t"
        "ldr	r8, [%[a], #184]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #184]\n\t"
        "# A[47] * B\n\t"
        "ldr	r8, [%[a], #188]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #188]\n\t"
        "# A[48] * B\n\t"
        "ldr	r8, [%[a], #192]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #192]\n\t"
        "# A[49] * B\n\t"
        "ldr	r8, [%[a], #196]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #196]\n\t"
        "# A[50] * B\n\t"
        "ldr	r8, [%[a], #200]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #200]\n\t"
        "# A[51] * B\n\t"
        "ldr	r8, [%[a], #204]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #204]\n\t"
        "# A[52] * B\n\t"
        "ldr	r8, [%[a], #208]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #208]\n\t"
        "# A[53] * B\n\t"
        "ldr	r8, [%[a], #212]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #212]\n\t"
        "# A[54] * B\n\t"
        "ldr	r8, [%[a], #216]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #216]\n\t"
        "# A[55] * B\n\t"
        "ldr	r8, [%[a], #220]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #220]\n\t"
        "# A[56] * B\n\t"
        "ldr	r8, [%[a], #224]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #224]\n\t"
        "# A[57] * B\n\t"
        "ldr	r8, [%[a], #228]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #228]\n\t"
        "# A[58] * B\n\t"
        "ldr	r8, [%[a], #232]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #232]\n\t"
        "# A[59] * B\n\t"
        "ldr	r8, [%[a], #236]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #236]\n\t"
        "# A[60] * B\n\t"
        "ldr	r8, [%[a], #240]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #240]\n\t"
        "# A[61] * B\n\t"
        "ldr	r8, [%[a], #244]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #244]\n\t"
        "# A[62] * B\n\t"
        "ldr	r8, [%[a], #248]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #248]\n\t"
        "# A[63] * B\n\t"
        "ldr	r8, [%[a], #252]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #252]\n\t"
        "# A[64] * B\n\t"
        "ldr	r8, [%[a], #256]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #256]\n\t"
        "# A[65] * B\n\t"
        "ldr	r8, [%[a], #260]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #260]\n\t"
        "# A[66] * B\n\t"
        "ldr	r8, [%[a], #264]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #264]\n\t"
        "# A[67] * B\n\t"
        "ldr	r8, [%[a], #268]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #268]\n\t"
        "# A[68] * B\n\t"
        "ldr	r8, [%[a], #272]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #272]\n\t"
        "# A[69] * B\n\t"
        "ldr	r8, [%[a], #276]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #276]\n\t"
        "# A[70] * B\n\t"
        "ldr	r8, [%[a], #280]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #280]\n\t"
        "# A[71] * B\n\t"
        "ldr	r8, [%[a], #284]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #284]\n\t"
        "# A[72] * B\n\t"
        "ldr	r8, [%[a], #288]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #288]\n\t"
        "# A[73] * B\n\t"
        "ldr	r8, [%[a], #292]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #292]\n\t"
        "# A[74] * B\n\t"
        "ldr	r8, [%[a], #296]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #296]\n\t"
        "# A[75] * B\n\t"
        "ldr	r8, [%[a], #300]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #300]\n\t"
        "# A[76] * B\n\t"
        "ldr	r8, [%[a], #304]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #304]\n\t"
        "# A[77] * B\n\t"
        "ldr	r8, [%[a], #308]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #308]\n\t"
        "# A[78] * B\n\t"
        "ldr	r8, [%[a], #312]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #312]\n\t"
        "# A[79] * B\n\t"
        "ldr	r8, [%[a], #316]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #316]\n\t"
        "# A[80] * B\n\t"
        "ldr	r8, [%[a], #320]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #320]\n\t"
        "# A[81] * B\n\t"
        "ldr	r8, [%[a], #324]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #324]\n\t"
        "# A[82] * B\n\t"
        "ldr	r8, [%[a], #328]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #328]\n\t"
        "# A[83] * B\n\t"
        "ldr	r8, [%[a], #332]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #332]\n\t"
        "# A[84] * B\n\t"
        "ldr	r8, [%[a], #336]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #336]\n\t"
        "# A[85] * B\n\t"
        "ldr	r8, [%[a], #340]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #340]\n\t"
        "# A[86] * B\n\t"
        "ldr	r8, [%[a], #344]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #344]\n\t"
        "# A[87] * B\n\t"
        "ldr	r8, [%[a], #348]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #348]\n\t"
        "# A[88] * B\n\t"
        "ldr	r8, [%[a], #352]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #352]\n\t"
        "# A[89] * B\n\t"
        "ldr	r8, [%[a], #356]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #356]\n\t"
        "# A[90] * B\n\t"
        "ldr	r8, [%[a], #360]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #360]\n\t"
        "# A[91] * B\n\t"
        "ldr	r8, [%[a], #364]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #364]\n\t"
        "# A[92] * B\n\t"
        "ldr	r8, [%[a], #368]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #368]\n\t"
        "# A[93] * B\n\t"
        "ldr	r8, [%[a], #372]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #372]\n\t"
        "# A[94] * B\n\t"
        "ldr	r8, [%[a], #376]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #376]\n\t"
        "# A[95] * B\n\t"
        "ldr	r8, [%[a], #380]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r5, [%[r], #380]\n\t"
        "str	r3, [%[r], #384]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#endif
}

/* Divide the double width number (d1|d0) by the dividend. (d1|d0 / div)
 *
 * d1   The high order half of the number to divide.
 * d0   The low order half of the number to divide.
 * div  The dividend.
 * returns the result of the division.
 *
 * Note that this is an approximate div. It may give an answer 1 larger.
 */
static sp_digit div_3072_word_96(sp_digit d1, sp_digit d0, sp_digit div)
{
    sp_digit r = 0;

    __asm__ __volatile__ (
        "lsr	r5, %[div], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[d0]\n\t"
        "mov	r7, %[d1]\n\t"
        "# Do top 32\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "# Next 30 bits\n\t"
        "mov	r4, #29\n\t"
        "1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r7, r7, r7\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "subs	r8, %[div], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r)
        : [d1] "r" (d1), [d0] "r" (d0), [div] "r" (div)
        : "r4", "r5", "r6", "r7", "r8"
    );
    return r;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_3072_mask_96(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<96; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 96; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
#endif
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static int32_t sp_3072_cmp_96(sp_digit* a, sp_digit* b)
{
    sp_digit r = -1;
    sp_digit one = 1;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "mov	r6, #380\n\t"
        "1:\n\t"
        "ldr	r4, [%[a], r6]\n\t"
        "ldr	r5, [%[b], r6]\n\t"
        "and	r4, r4, r3\n\t"
        "and	r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "sub	r6, r6, #4\n\t"
        "bcc	1b\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#else
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "ldr		r4, [%[a], #380]\n\t"
        "ldr		r5, [%[b], #380]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #376]\n\t"
        "ldr		r5, [%[b], #376]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #372]\n\t"
        "ldr		r5, [%[b], #372]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #368]\n\t"
        "ldr		r5, [%[b], #368]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #364]\n\t"
        "ldr		r5, [%[b], #364]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #360]\n\t"
        "ldr		r5, [%[b], #360]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #356]\n\t"
        "ldr		r5, [%[b], #356]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #352]\n\t"
        "ldr		r5, [%[b], #352]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #348]\n\t"
        "ldr		r5, [%[b], #348]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #344]\n\t"
        "ldr		r5, [%[b], #344]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #340]\n\t"
        "ldr		r5, [%[b], #340]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #336]\n\t"
        "ldr		r5, [%[b], #336]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #332]\n\t"
        "ldr		r5, [%[b], #332]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #328]\n\t"
        "ldr		r5, [%[b], #328]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #324]\n\t"
        "ldr		r5, [%[b], #324]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #320]\n\t"
        "ldr		r5, [%[b], #320]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #316]\n\t"
        "ldr		r5, [%[b], #316]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #312]\n\t"
        "ldr		r5, [%[b], #312]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #308]\n\t"
        "ldr		r5, [%[b], #308]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #304]\n\t"
        "ldr		r5, [%[b], #304]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #300]\n\t"
        "ldr		r5, [%[b], #300]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #296]\n\t"
        "ldr		r5, [%[b], #296]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #292]\n\t"
        "ldr		r5, [%[b], #292]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #288]\n\t"
        "ldr		r5, [%[b], #288]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #284]\n\t"
        "ldr		r5, [%[b], #284]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #280]\n\t"
        "ldr		r5, [%[b], #280]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #276]\n\t"
        "ldr		r5, [%[b], #276]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #272]\n\t"
        "ldr		r5, [%[b], #272]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #268]\n\t"
        "ldr		r5, [%[b], #268]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #264]\n\t"
        "ldr		r5, [%[b], #264]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #260]\n\t"
        "ldr		r5, [%[b], #260]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #256]\n\t"
        "ldr		r5, [%[b], #256]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #252]\n\t"
        "ldr		r5, [%[b], #252]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #248]\n\t"
        "ldr		r5, [%[b], #248]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #244]\n\t"
        "ldr		r5, [%[b], #244]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #240]\n\t"
        "ldr		r5, [%[b], #240]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #236]\n\t"
        "ldr		r5, [%[b], #236]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #232]\n\t"
        "ldr		r5, [%[b], #232]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #228]\n\t"
        "ldr		r5, [%[b], #228]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #224]\n\t"
        "ldr		r5, [%[b], #224]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #220]\n\t"
        "ldr		r5, [%[b], #220]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #216]\n\t"
        "ldr		r5, [%[b], #216]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #212]\n\t"
        "ldr		r5, [%[b], #212]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #208]\n\t"
        "ldr		r5, [%[b], #208]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #204]\n\t"
        "ldr		r5, [%[b], #204]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #200]\n\t"
        "ldr		r5, [%[b], #200]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #196]\n\t"
        "ldr		r5, [%[b], #196]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #192]\n\t"
        "ldr		r5, [%[b], #192]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #188]\n\t"
        "ldr		r5, [%[b], #188]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #184]\n\t"
        "ldr		r5, [%[b], #184]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #180]\n\t"
        "ldr		r5, [%[b], #180]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #176]\n\t"
        "ldr		r5, [%[b], #176]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #172]\n\t"
        "ldr		r5, [%[b], #172]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #168]\n\t"
        "ldr		r5, [%[b], #168]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #164]\n\t"
        "ldr		r5, [%[b], #164]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #160]\n\t"
        "ldr		r5, [%[b], #160]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #156]\n\t"
        "ldr		r5, [%[b], #156]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #152]\n\t"
        "ldr		r5, [%[b], #152]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #148]\n\t"
        "ldr		r5, [%[b], #148]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #144]\n\t"
        "ldr		r5, [%[b], #144]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #140]\n\t"
        "ldr		r5, [%[b], #140]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #136]\n\t"
        "ldr		r5, [%[b], #136]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #132]\n\t"
        "ldr		r5, [%[b], #132]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #128]\n\t"
        "ldr		r5, [%[b], #128]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #124]\n\t"
        "ldr		r5, [%[b], #124]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #120]\n\t"
        "ldr		r5, [%[b], #120]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #116]\n\t"
        "ldr		r5, [%[b], #116]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #112]\n\t"
        "ldr		r5, [%[b], #112]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #108]\n\t"
        "ldr		r5, [%[b], #108]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #104]\n\t"
        "ldr		r5, [%[b], #104]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #100]\n\t"
        "ldr		r5, [%[b], #100]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #96]\n\t"
        "ldr		r5, [%[b], #96]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #92]\n\t"
        "ldr		r5, [%[b], #92]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #88]\n\t"
        "ldr		r5, [%[b], #88]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #84]\n\t"
        "ldr		r5, [%[b], #84]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #80]\n\t"
        "ldr		r5, [%[b], #80]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #76]\n\t"
        "ldr		r5, [%[b], #76]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #72]\n\t"
        "ldr		r5, [%[b], #72]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #68]\n\t"
        "ldr		r5, [%[b], #68]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #64]\n\t"
        "ldr		r5, [%[b], #64]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #60]\n\t"
        "ldr		r5, [%[b], #60]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #56]\n\t"
        "ldr		r5, [%[b], #56]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #52]\n\t"
        "ldr		r5, [%[b], #52]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #48]\n\t"
        "ldr		r5, [%[b], #48]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #44]\n\t"
        "ldr		r5, [%[b], #44]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #40]\n\t"
        "ldr		r5, [%[b], #40]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #36]\n\t"
        "ldr		r5, [%[b], #36]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #32]\n\t"
        "ldr		r5, [%[b], #32]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #28]\n\t"
        "ldr		r5, [%[b], #28]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #24]\n\t"
        "ldr		r5, [%[b], #24]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #20]\n\t"
        "ldr		r5, [%[b], #20]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #16]\n\t"
        "ldr		r5, [%[b], #16]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #12]\n\t"
        "ldr		r5, [%[b], #12]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #8]\n\t"
        "ldr		r5, [%[b], #8]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #4]\n\t"
        "ldr		r5, [%[b], #4]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #0]\n\t"
        "ldr		r5, [%[b], #0]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#endif

    return r;
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_div_96(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[192], t2[97];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[95];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 96);
    for (i=95; i>=0; i--) {
        r1 = div_3072_word_96(t1[96 + i], t1[96 + i - 1], div);

        sp_3072_mul_d_96(t2, d, r1);
        t1[96 + i] += sp_3072_sub_in_place_96(&t1[i], t2);
        t1[96 + i] -= t2[96];
        sp_3072_mask_96(t2, d, t1[96 + i]);
        t1[96 + i] += sp_3072_add_96(&t1[i], &t1[i], t2);
        sp_3072_mask_96(t2, d, t1[96 + i]);
        t1[96 + i] += sp_3072_add_96(&t1[i], &t1[i], t2);
    }

    r1 = sp_3072_cmp_96(t1, d) >= 0;
    sp_3072_cond_sub_96(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_mod_96(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_96(a, m, NULL, r);
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_div_96_cond(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[192], t2[97];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[95];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 96);
    for (i=95; i>=0; i--) {
        r1 = div_3072_word_96(t1[96 + i], t1[96 + i - 1], div);

        sp_3072_mul_d_96(t2, d, r1);
        t1[96 + i] += sp_3072_sub_in_place_96(&t1[i], t2);
        t1[96 + i] -= t2[96];
        if (t1[96 + i] != 0) {
            t1[96 + i] += sp_3072_add_96(&t1[i], &t1[i], d);
            if (t1[96 + i] != 0)
                t1[96 + i] += sp_3072_add_96(&t1[i], &t1[i], d);
        }
    }

    r1 = sp_3072_cmp_96(t1, d) >= 0;
    sp_3072_cond_sub_96(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_3072_mod_96_cond(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_96_cond(a, m, NULL, r);
}

#if defined(SP_RSA_PRIVATE_EXP_D) || defined(WOLFSSL_HAVE_SP_DH)
#ifdef WOLFSSL_SP_SMALL
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_96(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[16][192];
#else
    sp_digit* t[16];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 16 * 192, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<16; i++)
            t[i] = td + i * 192;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_96(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 96);
        if (reduceA) {
            err = sp_3072_mod_96(t[1] + 96, a, m);
            if (err == MP_OKAY)
                err = sp_3072_mod_96(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 96, a, sizeof(sp_digit) * 96);
            err = sp_3072_mod_96(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_96(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_96(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_96(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_96(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_96(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_96(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_96(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_96(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_96(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_96(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_96(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_96(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_96(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_96(t[15], t[ 8], t[ 7], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 28;
        n <<= 4;
        c = 28;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 96);
        for (; i>=0 || c>=4; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 28;
                n <<= 4;
                c = 28;
            }
            else if (c < 4) {
                y = n >> 28;
                n = e[i--];
                c = 4 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 28) & 0xf;
                n <<= 4;
                c -= 4;
            }

            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);

            sp_3072_mont_mul_96(r, r, t[y], m, mp);
        }

        XMEMSET(&r[96], 0, sizeof(sp_digit) * 96);
        sp_3072_mont_reduce_96(r, m, mp);

        mask = 0 - (sp_3072_cmp_96(r, m) >= 0);
        sp_3072_cond_sub_96(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#else
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_96(sp_digit* r, sp_digit* a, sp_digit* e,
        int bits, sp_digit* m, int reduceA)
{
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][192];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    sp_digit mask;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 192, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 192;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_96(norm, m);

        XMEMSET(t[1], 0, sizeof(sp_digit) * 96);
        if (reduceA) {
            err = sp_3072_mod_96(t[1] + 96, a, m);
            if (err == MP_OKAY)
                err = sp_3072_mod_96(t[1], t[1], m);
        }
        else {
            XMEMCPY(t[1] + 96, a, sizeof(sp_digit) * 96);
            err = sp_3072_mod_96(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_96(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_96(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_96(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_96(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_96(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_96(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_96(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_96(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_96(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_96(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_96(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_96(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_96(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_96(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_96(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_96(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_96(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_96(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_96(t[20], t[10], m, mp);
        sp_3072_mont_mul_96(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_96(t[22], t[11], m, mp);
        sp_3072_mont_mul_96(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_96(t[24], t[12], m, mp);
        sp_3072_mont_mul_96(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_96(t[26], t[13], m, mp);
        sp_3072_mont_mul_96(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_96(t[28], t[14], m, mp);
        sp_3072_mont_mul_96(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_96(t[30], t[15], m, mp);
        sp_3072_mont_mul_96(t[31], t[16], t[15], m, mp);

        i = (bits - 1) / 32;
        n = e[i--];
        y = n >> 27;
        n <<= 5;
        c = 27;
        XMEMCPY(r, t[y], sizeof(sp_digit) * 96);
        for (; i>=0 || c>=5; ) {
            if (c == 0) {
                n = e[i--];
                y = n >> 27;
                n <<= 5;
                c = 27;
            }
            else if (c < 5) {
                y = n >> 27;
                n = e[i--];
                c = 5 - c;
                y |= n >> (32 - c);
                n <<= c;
                c = 32 - c;
            }
            else {
                y = (n >> 27) & 0x1f;
                n <<= 5;
                c -= 5;
            }

            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);
            sp_3072_mont_sqr_96(r, r, m, mp);

            sp_3072_mont_mul_96(r, r, t[y], m, mp);
        }
        y = e[0] & 0x3;
        sp_3072_mont_sqr_96(r, r, m, mp);
        sp_3072_mont_sqr_96(r, r, m, mp);
        sp_3072_mont_mul_96(r, r, t[y], m, mp);

        XMEMSET(&r[96], 0, sizeof(sp_digit) * 96);
        sp_3072_mont_reduce_96(r, m, mp);

        mask = 0 - (sp_3072_cmp_96(r, m) >= 0);
        sp_3072_cond_sub_96(r, r, m, mask);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}
#endif /* WOLFSSL_SP_SMALL */
#endif /* SP_RSA_PRIVATE_EXP_D || WOLFSSL_HAVE_SP_DH */

#ifdef WOLFSSL_HAVE_SP_RSA
/* RSA public key operation.
 *
 * in      Array of bytes representing the number to exponentiate, base.
 * inLen   Number of bytes in base.
 * em      Public exponent.
 * mm      Modulus.
 * out     Buffer to hold big-endian bytes of exponentiation result.
 *         Must be at least 384 bytes long.
 * outLen  Number of bytes in result.
 * returns 0 on success, MP_TO_E when the outLen is too small, MP_READ_E when
 * an array is too long and MEMORY_E when dynamic memory allocation fails.
 */
int sp_RsaPublic_3072(const byte* in, word32 inLen, mp_int* em, mp_int* mm,
    byte* out, word32* outLen)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[192], md[96], rd[192];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* a;
    sp_digit *ah;
    sp_digit* m;
    sp_digit* r;
    sp_digit e[1];
    int err = MP_OKAY;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 32 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 96 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 96 * 2;
        m = r + 96 * 2;
        ah = a + 96;
    }
#else
    a = ad;
    m = md;
    r = rd;
    ah = a + 96;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_bin(ah, 96, in, inLen);
#if DIGIT_BIT >= 32
        e[0] = em->dp[0];
#else
        e[0] = em->dp[0];
        if (em->used > 1)
            e[0] |= ((sp_digit)em->dp[1]) << DIGIT_BIT;
#endif
        if (e[0] == 0)
            err = MP_EXPTMOD_E;
    }
    if (err == MP_OKAY) {
        sp_3072_from_mp(m, 96, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_3072_sqr_96(r, ah);
                err = sp_3072_mod_96_cond(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_3072_mul_96(r, ah, r);
                err = sp_3072_mod_96_cond(r, r, m);
            }
        }
        else {
            int i;
            sp_digit mp;

            sp_3072_mont_setup(m, &mp);

            /* Convert to Montgomery form. */
            XMEMSET(a, 0, sizeof(sp_digit) * 96);
            err = sp_3072_mod_96_cond(a, a, m);

            if (err == MP_OKAY) {
                for (i=31; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 96);
                for (i--; i>=0; i--) {
                    sp_3072_mont_sqr_96(r, r, m, mp);
                    if (((e[0] >> i) & 1) == 1)
                        sp_3072_mont_mul_96(r, r, a, m, mp);
                }
                XMEMSET(&r[96], 0, sizeof(sp_digit) * 96);
                sp_3072_mont_reduce_96(r, m, mp);

                for (i = 95; i > 0; i--) {
                    if (r[i] != m[i])
                        break;
                }
                if (r[i] >= m[i])
                    sp_3072_sub_in_place_96(r, m);
            }
        }
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* RSA private key operation.
 *
 * in      Array of bytes representing the number to exponentiate, base.
 * inLen   Number of bytes in base.
 * dm      Private exponent.
 * pm      First prime.
 * qm      Second prime.
 * dpm     First prime's CRT exponent.
 * dqm     Second prime's CRT exponent.
 * qim     Inverse of second prime mod p.
 * mm      Modulus.
 * out     Buffer to hold big-endian bytes of exponentiation result.
 *         Must be at least 384 bytes long.
 * outLen  Number of bytes in result.
 * returns 0 on success, MP_TO_E when the outLen is too small, MP_READ_E when
 * an array is too long and MEMORY_E when dynamic memory allocation fails.
 */
int sp_RsaPrivate_3072(const byte* in, word32 inLen, mp_int* dm,
    mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim, mp_int* mm,
    byte* out, word32* outLen)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[96 * 2];
    sp_digit pd[48], qd[48], dpd[48];
    sp_digit tmpad[96], tmpbd[96];
#else
    sp_digit* t = NULL;
#endif
    sp_digit* a;
    sp_digit* p;
    sp_digit* q;
    sp_digit* dp;
    sp_digit* dq;
    sp_digit* qi;
    sp_digit* tmp;
    sp_digit* tmpa;
    sp_digit* tmpb;
    sp_digit* r;
    sp_digit c;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 384 || mp_count_bits(mm) != 3072))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 48 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 96 * 2;
        q = p + 48;
        qi = dq = dp = q + 48;
        tmpa = qi + 48;
        tmpb = tmpa + 96;

        tmp = t;
        r = tmp + 96;
    }
#else
    r = a = ad;
    p = pd;
    q = qd;
    qi = dq = dp = dpd;
    tmpa = tmpad;
    tmpb = tmpbd;
    tmp = a + 96;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 96, in, inLen);
        sp_3072_from_mp(p, 48, pm);
        sp_3072_from_mp(q, 48, qm);
        sp_3072_from_mp(dp, 48, dpm);

        err = sp_3072_mod_exp_48(tmpa, a, dp, 1536, p, 1);
    }
    if (err == MP_OKAY) {
        sp_3072_from_mp(dq, 48, dqm);
        err = sp_3072_mod_exp_48(tmpb, a, dq, 1536, q, 1);
    }

    if (err == MP_OKAY) {
        c = sp_3072_sub_in_place_48(tmpa, tmpb);
        sp_3072_mask_48(tmp, p, c);
        sp_3072_add_48(tmpa, tmpa, tmp);

        sp_3072_from_mp(qi, 48, qim);
        sp_3072_mul_48(tmpa, tmpa, qi);
        err = sp_3072_mod_48(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_3072_mul_48(tmpa, q, tmpa);
        XMEMSET(&tmpb[48], 0, sizeof(sp_digit) * 48);
        sp_3072_add_96(r, tmpb, tmpa);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 48 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else
    XMEMSET(tmpad, 0, sizeof(tmpad));
    XMEMSET(tmpbd, 0, sizeof(tmpbd));
    XMEMSET(pd, 0, sizeof(pd));
    XMEMSET(qd, 0, sizeof(qd));
    XMEMSET(dpd, 0, sizeof(dpd));
#endif

    return err;
}
#endif /* WOLFSSL_HAVE_SP_RSA */
#ifdef WOLFSSL_HAVE_SP_DH
/* Convert an array of sp_digit to an mp_int.
 *
 * a  A single precision integer.
 * r  A multi-precision integer.
 */
static int sp_3072_to_mp(sp_digit* a, mp_int* r)
{
    int err;

    err = mp_grow(r, (3072 + DIGIT_BIT - 1) / DIGIT_BIT);
    if (err == MP_OKAY) {
#if DIGIT_BIT == 32
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 96);
        r->used = 96;
        mp_clamp(r);
#elif DIGIT_BIT < 32
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 96; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 32) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 32 - s;
        }
        r->used = (3072 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 96; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 32 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 32 - s;
            }
            else
                s += 32;
        }
        r->used = (3072 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#endif
    }

    return err;
}

/* Perform the modular exponentiation for Diffie-Hellman.
 *
 * base  Base. MP integer.
 * exp   Exponent. MP integer.
 * mod   Modulus. MP integer.
 * res   Result. MP integer.
 * returs 0 on success, MP_READ_E if there are too many bytes in an array
 * and MEMORY_E if memory allocation fails.
 */
int sp_ModExp_3072(mp_int* base, mp_int* exp, mp_int* mod, mp_int* res)
{
    int err = MP_OKAY;
    sp_digit b[192], e[96], m[96];
    sp_digit* r = b;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 3072 || expBits > 3072 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 96, base);
        sp_3072_from_mp(e, 96, exp);
        sp_3072_from_mp(m, 96, mod);

        err = sp_3072_mod_exp_96(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_3072_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(e));

    return err;
}

/* Perform the modular exponentiation for Diffie-Hellman.
 *
 * base     Base.
 * exp      Array of bytes that is the exponent.
 * expLen   Length of data, in bytes, in exponent.
 * mod      Modulus.
 * out      Buffer to hold big-endian bytes of exponentiation result.
 *          Must be at least 384 bytes long.
 * outLen   Length, in bytes, of exponentiation result.
 * returs 0 on success, MP_READ_E if there are too many bytes in an array
 * and MEMORY_E if memory allocation fails.
 */
int sp_DhExp_3072(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen)
{
    int err = MP_OKAY;
    sp_digit b[192], e[96], m[96];
    sp_digit* r = b;
    word32 i;

    if (mp_count_bits(base) > 3072 || expLen > 384 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 96, base);
        sp_3072_from_bin(e, 96, exp, expLen);
        sp_3072_from_mp(m, 96, mod);

        err = sp_3072_mod_exp_96(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
        for (i=0; i<384 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);

    }

    XMEMSET(e, 0, sizeof(e));

    return err;
}

#endif /* WOLFSSL_HAVE_SP_DH */

#endif /* WOLFSSL_SP_NO_3072 */

#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH */
#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256

/* Point structure to use. */
typedef struct sp_point {
    sp_digit x[2 * 8];
    sp_digit y[2 * 8];
    sp_digit z[2 * 8];
    int infinity;
} sp_point;

/* The modulus (prime) of the curve P256. */
static sp_digit p256_mod[8] = {
    0xffffffff,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,
    0x00000001,0xffffffff
};
/* The Montogmery normalizer for modulus of the curve P256. */
static sp_digit p256_norm_mod[8] = {
    0x00000001,0x00000000,0x00000000,0xffffffff,0xffffffff,0xffffffff,
    0xfffffffe,0x00000000
};
/* The Montogmery multiplier for modulus of the curve P256. */
static sp_digit p256_mp_mod = 0x00000001;
#if defined(WOLFSSL_VALIDATE_ECC_KEYGEN) || defined(HAVE_ECC_SIGN) || \
                                            defined(HAVE_ECC_VERIFY)
/* The order of the curve P256. */
static sp_digit p256_order[8] = {
    0xfc632551,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,
    0x00000000,0xffffffff
};
#endif
/* The order of the curve P256 minus 2. */
static sp_digit p256_order2[8] = {
    0xfc63254f,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,
    0x00000000,0xffffffff
};
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery normalizer for order of the curve P256. */
static sp_digit p256_norm_order[8] = {
    0x039cdaaf,0x0c46353d,0x58e8617b,0x43190552,0x00000000,0x00000000,
    0xffffffff,0x00000000
};
#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery multiplier for order of the curve P256. */
static sp_digit p256_mp_order = 0xee00bc4f;
#endif
/* The base point of curve P256. */
static sp_point p256_base = {
    /* X ordinate */
    {
        0xd898c296,0xf4a13945,0x2deb33a0,0x77037d81,0x63a440f2,0xf8bce6e5,
        0xe12c4247,0x6b17d1f2
    },
    /* Y ordinate */
    {
        0x37bf51f5,0xcbb64068,0x6b315ece,0x2bce3357,0x7c0f9e16,0x8ee7eb4a,
        0xfe1a7f9b,0x4fe342e2
    },
    /* Z ordinate */
    {
        0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
        0x00000000,0x00000000
    },
    /* infinity */
    0
};
#if defined(HAVE_ECC_CHECK_KEY) || defined(HAVE_COMP_KEY)
static sp_digit p256_b[8] = {
    0x27d2604b,0x3bce3c3e,0xcc53b0f6,0x651d06b0,0x769886bc,0xb3ebbd55,
    0xaa3a93e7,0x5ac635d8
};
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
/* Allocate memory for point and return error. */
#define sp_ecc_point_new(heap, sp, p)                                   \
    ((p = XMALLOC(sizeof(sp_point), heap, DYNAMIC_TYPE_ECC)) == NULL) ? \
        MEMORY_E : MP_OKAY
#else
/* Set pointer to data and return no error. */
#define sp_ecc_point_new(heap, sp, p)   ((p = &sp) == NULL) ? MEMORY_E : MP_OKAY
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
/* If valid pointer then clear point data if requested and free data. */
#define sp_ecc_point_free(p, clear, heap)     \
    do {                                      \
        if (p != NULL) {                      \
            if (clear)                        \
                XMEMSET(p, 0, sizeof(*p));    \
            XFREE(p, heap, DYNAMIC_TYPE_ECC); \
        }                                     \
    }                                         \
    while (0)
#else
/* Clear point data if requested. */
#define sp_ecc_point_free(p, clear, heap) \
    do {                                  \
        if (clear)                        \
            XMEMSET(p, 0, sizeof(*p));    \
    }                                     \
    while (0)
#endif

/* Multiply a number by Montogmery normalizer mod modulus (prime).
 *
 * r  The resulting Montgomery form number.
 * a  The number to convert.
 * m  The modulus (prime).
 */
static int sp_256_mod_mul_norm_8(sp_digit* r, sp_digit* a, sp_digit* m)
{
    int64_t t[8];
    int64_t a64[8];
    int64_t o;

    (void)m;

    a64[0] = a[0];
    a64[1] = a[1];
    a64[2] = a[2];
    a64[3] = a[3];
    a64[4] = a[4];
    a64[5] = a[5];
    a64[6] = a[6];
    a64[7] = a[7];

    /*  1  1  0 -1 -1 -1 -1  0 */
    t[0] = 0 + a64[0] + a64[1] - a64[3] - a64[4] - a64[5] - a64[6];
    /*  0  1  1  0 -1 -1 -1 -1 */
    t[1] = 0 + a64[1] + a64[2] - a64[4] - a64[5] - a64[6] - a64[7];
    /*  0  0  1  1  0 -1 -1 -1 */
    t[2] = 0 + a64[2] + a64[3] - a64[5] - a64[6] - a64[7];
    /* -1 -1  0  2  2  1  0 -1 */
    t[3] = 0 - a64[0] - a64[1] + 2 * a64[3] + 2 * a64[4] + a64[5] - a64[7];
    /*  0 -1 -1  0  2  2  1  0 */
    t[4] = 0 - a64[1] - a64[2] + 2 * a64[4] + 2 * a64[5] + a64[6];
    /*  0  0 -1 -1  0  2  2  1 */
    t[5] = 0 - a64[2] - a64[3] + 2 * a64[5] + 2 * a64[6] + a64[7];
    /* -1 -1  0  0  0  1  3  2 */
    t[6] = 0 - a64[0] - a64[1] + a64[5] + 3 * a64[6] + 2 * a64[7];
    /*  1  0 -1 -1 -1 -1  0  3 */
    t[7] = 0 + a64[0] - a64[2] - a64[3] - a64[4] - a64[5] + 3 * a64[7];

    t[1] += t[0] >> 32; t[0] &= 0xffffffff;
    t[2] += t[1] >> 32; t[1] &= 0xffffffff;
    t[3] += t[2] >> 32; t[2] &= 0xffffffff;
    t[4] += t[3] >> 32; t[3] &= 0xffffffff;
    t[5] += t[4] >> 32; t[4] &= 0xffffffff;
    t[6] += t[5] >> 32; t[5] &= 0xffffffff;
    t[7] += t[6] >> 32; t[6] &= 0xffffffff;
    o     = t[7] >> 32; t[7] &= 0xffffffff;
    t[0] += o;
    t[3] -= o;
    t[6] -= o;
    t[7] += o;
    t[1] += t[0] >> 32; t[0] &= 0xffffffff;
    t[2] += t[1] >> 32; t[1] &= 0xffffffff;
    t[3] += t[2] >> 32; t[2] &= 0xffffffff;
    t[4] += t[3] >> 32; t[3] &= 0xffffffff;
    t[5] += t[4] >> 32; t[4] &= 0xffffffff;
    t[6] += t[5] >> 32; t[5] &= 0xffffffff;
    t[7] += t[6] >> 32; t[6] &= 0xffffffff;
    r[0] = t[0];
    r[1] = t[1];
    r[2] = t[2];
    r[3] = t[3];
    r[4] = t[4];
    r[5] = t[5];
    r[6] = t[6];
    r[7] = t[7];

    return MP_OKAY;
}

/* Convert an mp_int to an array of sp_digit.
 *
 * r  A single precision integer.
 * a  A multi-precision integer.
 */
static void sp_256_from_mp(sp_digit* r, int max, mp_int* a)
{
#if DIGIT_BIT == 32
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 32
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0xffffffff;
        s = 32 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 32 <= DIGIT_BIT) {
            s += 32;
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            if (s < DIGIT_BIT)
                r[++j] = a->dp[i] >> s;
            else
                r[++j] = 0;
        }
        s = DIGIT_BIT - s;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#else
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= ((sp_digit)a->dp[i]) << s;
        if (s + DIGIT_BIT >= 32) {
            r[j] &= 0xffffffff;
            if (j + 1 >= max)
                break;
            s = 32 - s;
            if (s == DIGIT_BIT) {
                r[++j] = 0;
                s = 0;
            }
            else {
                r[++j] = a->dp[i] >> s;
                s = DIGIT_BIT - s;
            }
        }
        else
            s += DIGIT_BIT;
    }

    for (j++; j < max; j++)
        r[j] = 0;
#endif
}

/* Convert a point of type ecc_point to type sp_point.
 *
 * p   Point of type sp_point (result).
 * pm  Point of type ecc_point.
 */
static void sp_256_point_from_ecc_point_8(sp_point* p, ecc_point* pm)
{
    XMEMSET(p->x, 0, sizeof(p->x));
    XMEMSET(p->y, 0, sizeof(p->y));
    XMEMSET(p->z, 0, sizeof(p->z));
    sp_256_from_mp(p->x, 8, pm->x);
    sp_256_from_mp(p->y, 8, pm->y);
    sp_256_from_mp(p->z, 8, pm->z);
    p->infinity = 0;
}

/* Convert an array of sp_digit to an mp_int.
 *
 * a  A single precision integer.
 * r  A multi-precision integer.
 */
static int sp_256_to_mp(sp_digit* a, mp_int* r)
{
    int err;

    err = mp_grow(r, (256 + DIGIT_BIT - 1) / DIGIT_BIT);
    if (err == MP_OKAY) {
#if DIGIT_BIT == 32
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 8);
        r->used = 8;
        mp_clamp(r);
#elif DIGIT_BIT < 32
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 8; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 32) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 32 - s;
        }
        r->used = (256 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 8; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 32 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 32 - s;
            }
            else
                s += 32;
        }
        r->used = (256 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#endif
    }

    return err;
}

/* Convert a point of type sp_point to type ecc_point.
 *
 * p   Point of type sp_point.
 * pm  Point of type ecc_point (result).
 * returns MEMORY_E when allocation of memory in ecc_point fails otherwise
 * MP_OKAY.
 */
static int sp_256_point_to_ecc_point_8(sp_point* p, ecc_point* pm)
{
    int err;

    err = sp_256_to_mp(p->x, pm->x);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->y, pm->y);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->z, pm->z);

    return err;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static int32_t sp_256_cmp_8(sp_digit* a, sp_digit* b)
{
    sp_digit r = -1;
    sp_digit one = 1;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "mov	r6, #28\n\t"
        "1:\n\t"
        "ldr	r4, [%[a], r6]\n\t"
        "ldr	r5, [%[b], r6]\n\t"
        "and	r4, r4, r3\n\t"
        "and	r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "sub	r6, r6, #4\n\t"
        "bcc	1b\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#else
    __asm__ __volatile__ (
        "mov	r7, #0\n\t"
        "mov	r3, #-1\n\t"
        "ldr		r4, [%[a], #28]\n\t"
        "ldr		r5, [%[b], #28]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #24]\n\t"
        "ldr		r5, [%[b], #24]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #20]\n\t"
        "ldr		r5, [%[b], #20]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #16]\n\t"
        "ldr		r5, [%[b], #16]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #12]\n\t"
        "ldr		r5, [%[b], #12]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #8]\n\t"
        "ldr		r5, [%[b], #8]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #4]\n\t"
        "ldr		r5, [%[b], #4]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "ldr		r4, [%[a], #0]\n\t"
        "ldr		r5, [%[b], #0]\n\t"
        "and		r4, r4, r3\n\t"
        "and		r5, r5, r3\n\t"
        "subs	r4, r4, r5\n\t"
        "movhi	%[r], %[one]\n\t"
        "movlo	%[r], r3\n\t"
        "movne	r3, r7\n\t"
        "eor	%[r], %[r], r3\n\t"
        : [r] "+r" (r)
        : [a] "r" (a), [b] "r" (b), [one] "r" (one)
        : "r2", "r3", "r4", "r5", "r6", "r7"
    );
#endif

    return r;
}

/* Normalize the values in each word to 32.
 *
 * a  Array of sp_digit to normalize.
 */
#define sp_256_norm_8(a)

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not copying.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static sp_digit sp_256_cond_sub_8(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit m)
{
    sp_digit c = 0;

#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r9, #0\n\t"
        "mov	r8, #0\n\t"
        "1:\n\t"
        "subs	%[c], r9, %[c]\n\t"
        "ldr	r4, [%[a], r8]\n\t"
        "ldr	r5, [%[b], r8]\n\t"
        "and	r5, r5, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbc	%[c], r9, r9\n\t"
        "str	r4, [%[r], r8]\n\t"
        "add	r8, r8, #4\n\t"
        "cmp	r8, #32\n\t"
        "blt	1b\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#else
    __asm__ __volatile__ (

        "mov	r9, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r5, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "subs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r6, [%[r], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r5, [%[b], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r5, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r6, [%[r], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r5, [%[b], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "and	r5, r5, %[m]\n\t"
        "and	r7, r7, %[m]\n\t"
        "sbcs	r4, r4, r5\n\t"
        "sbcs	r6, r6, r7\n\t"
        "str	r4, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [m] "r" (m)
        : "memory", "r4", "r6", "r5", "r7", "r8", "r9"
    );
#endif /* WOLFSSL_SP_SMALL */

    return c;
}

/* Reduce the number back to 256 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
SP_NOINLINE static void sp_256_mont_reduce_8(sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit ca = 0;

    __asm__ __volatile__ (
        "# i = 0\n\t"
        "mov	r12, #0\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "ldr	r14, [%[a], #4]\n\t"
        "\n1:\n\t"
        "# mu = a[i] * mp\n\t"
        "mul	r8, %[mp], r10\n\t"
        "# a[i+0] += m[0] * mu\n\t"
        "ldr	r7, [%[m], #0]\n\t"
        "ldr	r9, [%[a], #0]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r10, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "# a[i+1] += m[1] * mu\n\t"
        "ldr	r7, [%[m], #4]\n\t"
        "ldr	r9, [%[a], #4]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r10, r14, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r10, r10, r5\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+2] += m[2] * mu\n\t"
        "ldr	r7, [%[m], #8]\n\t"
        "ldr	r14, [%[a], #8]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r14, r14, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r14, r14, r4\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+3] += m[3] * mu\n\t"
        "ldr	r7, [%[m], #12]\n\t"
        "ldr	r9, [%[a], #12]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #12]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+4] += m[4] * mu\n\t"
        "ldr	r7, [%[m], #16]\n\t"
        "ldr	r9, [%[a], #16]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #16]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+5] += m[5] * mu\n\t"
        "ldr	r7, [%[m], #20]\n\t"
        "ldr	r9, [%[a], #20]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r4, r7, #0\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #20]\n\t"
        "adc	r4, r4, #0\n\t"
        "# a[i+6] += m[6] * mu\n\t"
        "ldr	r7, [%[m], #24]\n\t"
        "ldr	r9, [%[a], #24]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r9, r9, r6\n\t"
        "adc	r5, r7, #0\n\t"
        "adds	r9, r9, r4\n\t"
        "str	r9, [%[a], #24]\n\t"
        "adc	r5, r5, #0\n\t"
        "# a[i+7] += m[7] * mu\n\t"
        "ldr	r7, [%[m], #28]\n\t"
        "ldr   r9, [%[a], #28]\n\t"
        "umull	r6, r7, r8, r7\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r7, r7, %[ca]\n\t"
        "mov	%[ca], #0\n\t"
        "adc	%[ca], %[ca], %[ca]\n\t"
        "adds	r9, r9, r5\n\t"
        "str	r9, [%[a], #28]\n\t"
        "ldr	r9, [%[a], #32]\n\t"
        "adcs	r9, r9, r7\n\t"
        "str	r9, [%[a], #32]\n\t"
        "adc	%[ca], %[ca], #0\n\t"
        "# i += 1\n\t"
        "add	%[a], %[a], #4\n\t"
        "add	r12, r12, #4\n\t"
        "cmp	r12, #32\n\t"
        "blt	1b\n\t"
        "str	r10, [%[a], #0]\n\t"
        "str	r14, [%[a], #4]\n\t"
        : [ca] "+r" (ca), [a] "+r" (a)
        : [m] "r" (m), [mp] "r" (mp)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    sp_256_cond_sub_8(a - 8, a, m, (sp_digit)0 - ca);
}

/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
SP_NOINLINE static void sp_256_mont_mul_8(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_digit tmp[9];

    (void)mp;
    (void)m;

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "#  A[0] * B[0]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r8, r9, r6, r7\n\t"
        "str	r8, [%[tmp], #0]\n\t"
        "#  A[0] * B[1]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adc	r10, r4, #0\n\t"
        "#  A[1] * B[0]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "str	r9, [%[tmp], #4]\n\t"
        "#  A[0] * B[2]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adc	r14, r4, r14\n\t"
        "#  A[1] * B[1]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, #0\n\t"
        "#  A[2] * B[0]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "str	r10, [%[tmp], #8]\n\t"
        "#  A[0] * B[3]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "#  A[1] * B[2]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[2] * B[1]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[3] * B[0]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "str	r14, [%[tmp], #12]\n\t"
        "#  A[0] * B[4]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "#  A[1] * B[3]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[2] * B[2]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[3] * B[1]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[4] * B[0]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "#  A[0] * B[5]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "#  A[1] * B[4]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[2] * B[3]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[3] * B[2]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[4] * B[1]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[5] * B[0]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "#  A[0] * B[6]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, #0\n\t"
        "#  A[1] * B[5]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[2] * B[4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[3] * B[3]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[4] * B[2]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[5] * B[1]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[6] * B[0]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "#  A[0] * B[7]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "#  A[1] * B[6]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[2] * B[5]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[3] * B[4]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[4] * B[3]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[5] * B[2]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[6] * B[1]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[7] * B[0]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "#  A[1] * B[7]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "#  A[2] * B[6]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[3] * B[5]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[4] * B[4]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[5] * B[3]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[6] * B[2]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[7] * B[1]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "str	r8, [%[r], #0]\n\t"
        "#  A[2] * B[7]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "#  A[3] * B[6]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[4] * B[5]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[5] * B[4]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[6] * B[3]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[7] * B[2]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "str	r9, [%[r], #4]\n\t"
        "#  A[3] * B[7]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, #0\n\t"
        "#  A[4] * B[6]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[5] * B[5]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[6] * B[4]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[7] * B[3]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "str	r10, [%[r], #8]\n\t"
        "#  A[4] * B[7]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "#  A[5] * B[6]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[6] * B[5]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[7] * B[4]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "str	r14, [%[r], #12]\n\t"
        "#  A[5] * B[7]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "#  A[6] * B[6]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[7] * B[5]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[6] * B[7]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "#  A[7] * B[6]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[7] * B[7]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adc	r14, r4, r14\n\t"
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
        "str	r10, [%[r], #24]\n\t"
        "str	r14, [%[r], #28]\n\t"
        "# Start Reduction\n\t"
        "ldr	r4, [%[tmp], #0]\n\t"
        "ldr	r5, [%[tmp], #4]\n\t"
        "ldr	r6, [%[tmp], #8]\n\t"
        "ldr	r7, [%[tmp], #12]\n\t"
        "ldr	r8, [%[tmp], #16]\n\t"
        "ldr	r9, [%[tmp], #20]\n\t"
        "ldr	r10, [%[tmp], #24]\n\t"
        "ldr	r14, [%[tmp], #28]\n\t"
        "# mu = a[0]-a[7] + a[0]-a[4] << 96 + (a[0]-a[1] * 2) << 192\n\t"
        "#    - a[0] << 224\n\t"
        "#   + (a[0]-a[1] * 2) << (6 * 32)\n\t"
        "adds	r10, r10, r4\n\t"
        "adc	r14, r14, r5\n\t"
        "adds	r10, r10, r4\n\t"
        "adc	r14, r14, r5\n\t"
        "#   - a[0] << (7 * 32)\n\t"
        "sub	r14, r14, r4\n\t"
        "#   + a[0]-a[4] << (3 * 32)\n\t"
        "mov	%[a], r7\n\t"
        "mov	%[b], r8\n\t"
        "adds	r7, r7, r4\n\t"
        "adcs	r8, r8, r5\n\t"
        "adcs	r9, r9, r6\n\t"
        "adcs	r10, r10, %[a]\n\t"
        "adc	r14, r14, %[b]\n\t"
        "str	r4, [%[tmp], #0]\n\t"
        "str	r5, [%[tmp], #4]\n\t"
        "str	r6, [%[tmp], #8]\n\t"
        "str	r7, [%[tmp], #12]\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "# a += mu * m\n\t"
        "#   += mu * ((1 << 256) - (1 << 224) + (1 << 192) + (1 << 96) - 1)\n\t"
        "mov	%[a], #0\n\t"
        "# a[6] +=        t[0] + t[3]\n\t"
        "ldr	r3, [%[tmp], #24]\n\t"
        "adds	r3, r3, r4\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "# a[7] +=        t[1] + t[4]\n\t"
        "ldr	r3, [%[tmp], #28]\n\t"
        "adds	r3, r3, %[b]\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r5\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r8\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "str	r3, [%[tmp], #32]\n\t"
        "# a[8] += t[0] + t[2] + t[5]\n\t"
        "ldr	r3, [%[r], #0]\n\t"
        "adds	r3, r3, %[b]\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r4\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r6\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r9\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r3, [%[r], #0]\n\t"
        "# a[9]  += t[1] + t[3] + t[6]\n\t"
        "# a[10] += t[2] + t[4] + t[7]\n\t"
        "ldr	r3, [%[r], #4]\n\t"
        "ldr	r4, [%[r], #8]\n\t"
        "adds	r3, r3, %[b]\n\t"
        "adcs	r4, r4, #0\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r5\n\t"
        "adcs	r4, r4, r6\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adcs	r4, r4, r8\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r10\n\t"
        "adcs	r4, r4, r14\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r3, [%[r], #4]\n\t"
        "str	r4, [%[r], #8]\n\t"
        "# a[11] += t[3] + t[5]\n\t"
        "# a[12] += t[4] + t[6]\n\t"
        "# a[13] += t[5] + t[7]\n\t"
        "# a[14] += t[6]\n\t"
        "ldr	r3, [%[r], #12]\n\t"
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
        "ldr	r6, [%[r], #24]\n\t"
        "adds	r3, r3, %[b]\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adc	%[b], %[b], #0\n\t"
        "adds	r3, r3, r9\n\t"
        "adcs	r4, r4, r10\n\t"
        "adcs	r5, r5, r14\n\t"
        "adcs	r6, r6, #0\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r3, [%[r], #12]\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "# a[15] += t[7]\n\t"
        "ldr	r3, [%[r], #28]\n\t"
        "adds	r3, r3, %[b]\n\t"
        "adc	%[b], %[a], #0\n\t"
        "adds	r3, r3, r14\n\t"
        "adc	%[b], %[b], #0\n\t"
        "str	r3, [%[r], #28]\n\t"
        "ldr	r3, [%[tmp], #32]\n\t"
        "ldr	r4, [%[r], #0]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
        "ldr	r6, [%[r], #8]\n\t"
        "ldr	r8, [%[tmp], #0]\n\t"
        "ldr	r9, [%[tmp], #4]\n\t"
        "ldr	r10, [%[tmp], #8]\n\t"
        "ldr	r14, [%[tmp], #12]\n\t"
        "subs	r3, r3, r8\n\t"
        "sbcs	r4, r4, r9\n\t"
        "sbcs	r5, r5, r10\n\t"
        "sbcs	r6, r6, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "ldr	r3, [%[r], #12]\n\t"
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
        "ldr	r6, [%[r], #24]\n\t"
        "ldr	r7, [%[r], #28]\n\t"
        "ldr	r8, [%[tmp], #16]\n\t"
        "ldr	r9, [%[tmp], #20]\n\t"
        "ldr	r10, [%[tmp], #24]\n\t"
        "ldr	r14, [%[tmp], #28]\n\t"
        "sbcs	r3, r3, r8\n\t"
        "sbcs	r4, r4, r9\n\t"
        "sbcs	r5, r5, r10\n\t"
        "sbcs	r6, r6, r14\n\t"
        "sbc	r7, r7, #0\n\t"
        "str	r3, [%[r], #12]\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "# mask m and sub from result if overflow\n\t"
        "sub	%[b], %[a], %[b]\n\t"
        "and	%[a], %[b], #1\n\t"
        "ldr	r3, [%[r], #0]\n\t"
        "ldr	r4, [%[r], #4]\n\t"
        "ldr	r5, [%[r], #8]\n\t"
        "ldr	r6, [%[r], #12]\n\t"
        "ldr	r7, [%[r], #16]\n\t"
        "ldr	r8, [%[r], #20]\n\t"
        "ldr	r9, [%[r], #24]\n\t"
        "ldr	r10, [%[r], #28]\n\t"
        "subs	r3, r3, %[b]\n\t"
        "sbcs	r4, r4, %[b]\n\t"
        "sbcs	r5, r5, %[b]\n\t"
        "sbcs	r6, r6, #0\n\t"
        "sbcs	r7, r7, #0\n\t"
        "sbcs	r8, r8, #0\n\t"
        "sbcs	r9, r9, %[a]\n\t"
        "sbc	r10, r10, %[b]\n\t"
        "str	r3, [%[r], #0]\n\t"
        "str	r4, [%[r], #4]\n\t"
        "str	r5, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "str	r7, [%[r], #16]\n\t"
        "str	r8, [%[r], #20]\n\t"
        "str	r9, [%[r], #24]\n\t"
        "str	r10, [%[r], #28]\n\t"
        : [a] "+r" (a), [b] "+r" (b)
        : [r] "r" (r), [tmp] "r" (tmp)
        : "memory", "r8", "r9", "r10", "r14", "r3", "r4", "r5", "r6", "r7"
    );
}

/* Square the Montgomery form number mod the modulus (prime). (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
SP_NOINLINE static void sp_256_mont_sqr_8(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_digit tmp[16];

    (void)mp;
    (void)m;

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "#  A[0] * A[1]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #4]\n\t"
        "umull	r9, r10, r6, r7\n\t"
        "str	r9, [%[tmp], #4]\n\t"
        "#  A[0] * A[2]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adc	r14, r4, #0\n\t"
        "str	r10, [%[tmp], #8]\n\t"
        "#  A[0] * A[3]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adc	r8, r4, #0\n\t"
        "#  A[1] * A[2]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #8]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "str	r14, [%[tmp], #12]\n\t"
        "#  A[0] * A[4]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adc	r9, r4, r9\n\t"
        "#  A[1] * A[3]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "#  A[0] * A[5]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adc	r10, r4, r10\n\t"
        "#  A[1] * A[4]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "#  A[2] * A[3]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "#  A[0] * A[6]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, #0\n\t"
        "#  A[1] * A[5]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "#  A[2] * A[4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "#  A[0] * A[7]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "#  A[1] * A[6]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[2] * A[5]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "#  A[3] * A[4]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[a], #16]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "#  A[1] * A[7]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "#  A[2] * A[6]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "#  A[3] * A[5]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, r10\n\t"
        "str	r8, [%[tmp], #32]\n\t"
        "#  A[2] * A[7]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, #0\n\t"
        "#  A[3] * A[6]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "#  A[4] * A[5]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[a], #20]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adcs	r10, r4, r10\n\t"
        "adc	r14, r5, r14\n\t"
        "str	r9, [%[tmp], #36]\n\t"
        "#  A[3] * A[7]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, #0\n\t"
        "#  A[4] * A[6]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r10, r3, r10\n\t"
        "adcs	r14, r4, r14\n\t"
        "adc	r8, r5, r8\n\t"
        "str	r10, [%[tmp], #40]\n\t"
        "#  A[4] * A[7]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, #0\n\t"
        "#  A[5] * A[6]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[a], #24]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r14, r3, r14\n\t"
        "adcs	r8, r4, r8\n\t"
        "adc	r9, r5, r9\n\t"
        "str	r14, [%[tmp], #44]\n\t"
        "#  A[5] * A[7]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r8, r3, r8\n\t"
        "adcs	r9, r4, r9\n\t"
        "adc	r10, r5, #0\n\t"
        "str	r8, [%[tmp], #48]\n\t"
        "#  A[6] * A[7]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "umull	r3, r4, r6, r7\n\t"
        "adds	r9, r3, r9\n\t"
        "adc	r10, r4, r10\n\t"
        "str	r9, [%[tmp], #52]\n\t"
        "str	r10, [%[tmp], #56]\n\t"
        "# Double\n\t"
        "ldr	r4, [%[tmp], #4]\n\t"
        "ldr	r6, [%[tmp], #8]\n\t"
        "ldr	r7, [%[tmp], #12]\n\t"
        "ldr	r8, [%[tmp], #16]\n\t"
        "ldr	r9, [%[tmp], #20]\n\t"
        "ldr	r10, [%[tmp], #24]\n\t"
        "ldr	r14, [%[tmp], #28]\n\t"
        "ldr	r12, [%[tmp], #32]\n\t"
        "ldr	r3, [%[tmp], #36]\n\t"
        "adds	r4, r4, r4\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adcs	r14, r14, r14\n\t"
        "adcs	r12, r12, r12\n\t"
        "adcs	r3, r3, r3\n\t"
        "str	r4, [%[tmp], #4]\n\t"
        "str	r6, [%[tmp], #8]\n\t"
        "str	r7, [%[tmp], #12]\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "str	r12, [%[tmp], #32]\n\t"
        "str	r3, [%[tmp], #36]\n\t"
        "ldr	r4, [%[tmp], #40]\n\t"
        "ldr	r6, [%[tmp], #44]\n\t"
        "ldr	r7, [%[tmp], #48]\n\t"
        "ldr	r8, [%[tmp], #52]\n\t"
        "ldr	r9, [%[tmp], #56]\n\t"
        "adcs	r4, r4, r4\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "str	r4, [%[tmp], #40]\n\t"
        "str	r6, [%[tmp], #44]\n\t"
        "str	r7, [%[tmp], #48]\n\t"
        "str	r8, [%[tmp], #52]\n\t"
        "str	r9, [%[tmp], #56]\n\t"
        "adc	r10, r5, #0\n\t"
        "str	r10, [%[tmp], #60]\n\t"
        "ldr	r4, [%[tmp], #4]\n\t"
        "ldr	r5, [%[tmp], #8]\n\t"
        "ldr	r12, [%[tmp], #12]\n\t"
        "#  A[0] * A[0]\n\t"
        "ldr	r6, [%[a], #0]\n\t"
        "umull	r8, r9, r6, r6\n\t"
        "#  A[1] * A[1]\n\t"
        "ldr	r6, [%[a], #4]\n\t"
        "umull	r10, r14, r6, r6\n\t"
        "adds	r9, r9, r4\n\t"
        "adcs	r10, r10, r5\n\t"
        "adcs	r14, r14, r12\n\t"
        "str	r8, [%[tmp], #0]\n\t"
        "str	r9, [%[tmp], #4]\n\t"
        "str	r10, [%[tmp], #8]\n\t"
        "str	r14, [%[tmp], #12]\n\t"
        "ldr	r3, [%[tmp], #16]\n\t"
        "ldr	r4, [%[tmp], #20]\n\t"
        "ldr	r5, [%[tmp], #24]\n\t"
        "ldr	r12, [%[tmp], #28]\n\t"
        "#  A[2] * A[2]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "umull	r8, r9, r6, r6\n\t"
        "#  A[3] * A[3]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "umull	r10, r14, r6, r6\n\t"
        "adcs	r8, r8, r3\n\t"
        "adcs	r9, r9, r4\n\t"
        "adcs	r10, r10, r5\n\t"
        "adcs	r14, r14, r12\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "ldr	r3, [%[tmp], #32]\n\t"
        "ldr	r4, [%[tmp], #36]\n\t"
        "ldr	r5, [%[tmp], #40]\n\t"
        "ldr	r12, [%[tmp], #44]\n\t"
        "#  A[4] * A[4]\n\t"
        "ldr	r6, [%[a], #16]\n\t"
        "umull	r8, r9, r6, r6\n\t"
        "#  A[5] * A[5]\n\t"
        "ldr	r6, [%[a], #20]\n\t"
        "umull	r10, r14, r6, r6\n\t"
        "adcs	r8, r8, r3\n\t"
        "adcs	r9, r9, r4\n\t"
        "adcs	r10, r10, r5\n\t"
        "adcs	r14, r14, r12\n\t"
        "str	r8, [%[r], #0]\n\t"
        "str	r9, [%[r], #4]\n\t"
        "str	r10, [%[r], #8]\n\t"
        "str	r14, [%[r], #12]\n\t"
        "ldr	r3, [%[tmp], #48]\n\t"
        "ldr	r4, [%[tmp], #52]\n\t"
        "ldr	r5, [%[tmp], #56]\n\t"
        "ldr	r12, [%[tmp], #60]\n\t"
        "#  A[6] * A[6]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "umull	r8, r9, r6, r6\n\t"
        "#  A[7] * A[7]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "umull	r10, r14, r6, r6\n\t"
        "adcs	r8, r8, r3\n\t"
        "adcs	r9, r9, r4\n\t"
        "adcs	r10, r10, r5\n\t"
        "adc	r14, r14, r12\n\t"
        "str	r8, [%[r], #16]\n\t"
        "str	r9, [%[r], #20]\n\t"
        "str	r10, [%[r], #24]\n\t"
        "str	r14, [%[r], #28]\n\t"
        "# Start Reduction\n\t"
        "ldr	r4, [%[tmp], #0]\n\t"
        "ldr	r5, [%[tmp], #4]\n\t"
        "ldr	r6, [%[tmp], #8]\n\t"
        "ldr	r7, [%[tmp], #12]\n\t"
        "ldr	r8, [%[tmp], #16]\n\t"
        "ldr	r9, [%[tmp], #20]\n\t"
        "ldr	r10, [%[tmp], #24]\n\t"
        "ldr	r14, [%[tmp], #28]\n\t"
        "# mu = a[0]-a[7] + a[0]-a[4] << 96 + (a[0]-a[1] * 2) << 192\n\t"
        "#    - a[0] << 224\n\t"
        "#   + (a[0]-a[1] * 2) << (6 * 32)\n\t"
        "adds	r10, r10, r4\n\t"
        "adc	r14, r14, r5\n\t"
        "adds	r10, r10, r4\n\t"
        "adc	r14, r14, r5\n\t"
        "#   - a[0] << (7 * 32)\n\t"
        "sub	r14, r14, r4\n\t"
        "#   + a[0]-a[4] << (3 * 32)\n\t"
        "mov	%[a], r7\n\t"
        "mov	r12, r8\n\t"
        "adds	r7, r7, r4\n\t"
        "adcs	r8, r8, r5\n\t"
        "adcs	r9, r9, r6\n\t"
        "adcs	r10, r10, %[a]\n\t"
        "adc	r14, r14, r12\n\t"
        "str	r4, [%[tmp], #0]\n\t"
        "str	r5, [%[tmp], #4]\n\t"
        "str	r6, [%[tmp], #8]\n\t"
        "str	r7, [%[tmp], #12]\n\t"
        "str	r8, [%[tmp], #16]\n\t"
        "str	r9, [%[tmp], #20]\n\t"
        "# a += mu * m\n\t"
        "#   += mu * ((1 << 256) - (1 << 224) + (1 << 192) + (1 << 96) - 1)\n\t"
        "mov	%[a], #0\n\t"
        "# a[6] +=        t[0] + t[3]\n\t"
        "ldr	r3, [%[tmp], #24]\n\t"
        "adds	r3, r3, r4\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r10, [%[tmp], #24]\n\t"
        "# a[7] +=        t[1] + t[4]\n\t"
        "ldr	r3, [%[tmp], #28]\n\t"
        "adds	r3, r3, r12\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r5\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r8\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r14, [%[tmp], #28]\n\t"
        "str	r3, [%[tmp], #32]\n\t"
        "# a[8] += t[0] + t[2] + t[5]\n\t"
        "ldr	r3, [%[r], #0]\n\t"
        "adds	r3, r3, r12\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r4\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r6\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r9\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r3, [%[r], #0]\n\t"
        "# a[9]  += t[1] + t[3] + t[6]\n\t"
        "# a[10] += t[2] + t[4] + t[7]\n\t"
        "ldr	r3, [%[r], #4]\n\t"
        "ldr	r4, [%[r], #8]\n\t"
        "adds	r3, r3, r12\n\t"
        "adcs	r4, r4, #0\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r5\n\t"
        "adcs	r4, r4, r6\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adcs	r4, r4, r8\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r10\n\t"
        "adcs	r4, r4, r14\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r3, [%[r], #4]\n\t"
        "str	r4, [%[r], #8]\n\t"
        "# a[11] += t[3] + t[5]\n\t"
        "# a[12] += t[4] + t[6]\n\t"
        "# a[13] += t[5] + t[7]\n\t"
        "# a[14] += t[6]\n\t"
        "ldr	r3, [%[r], #12]\n\t"
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
        "ldr	r6, [%[r], #24]\n\t"
        "adds	r3, r3, r12\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, #0\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r7\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adc	r12, r12, #0\n\t"
        "adds	r3, r3, r9\n\t"
        "adcs	r4, r4, r10\n\t"
        "adcs	r5, r5, r14\n\t"
        "adcs	r6, r6, #0\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r3, [%[r], #12]\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "# a[15] += t[7]\n\t"
        "ldr	r3, [%[r], #28]\n\t"
        "adds	r3, r3, r12\n\t"
        "adc	r12, %[a], #0\n\t"
        "adds	r3, r3, r14\n\t"
        "adc	r12, r12, #0\n\t"
        "str	r3, [%[r], #28]\n\t"
        "ldr	r3, [%[tmp], #32]\n\t"
        "ldr	r4, [%[r], #0]\n\t"
        "ldr	r5, [%[r], #4]\n\t"
        "ldr	r6, [%[r], #8]\n\t"
        "ldr	r8, [%[tmp], #0]\n\t"
        "ldr	r9, [%[tmp], #4]\n\t"
        "ldr	r10, [%[tmp], #8]\n\t"
        "ldr	r14, [%[tmp], #12]\n\t"
        "subs	r3, r3, r8\n\t"
        "sbcs	r4, r4, r9\n\t"
        "sbcs	r5, r5, r10\n\t"
        "sbcs	r6, r6, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "ldr	r3, [%[r], #12]\n\t"
        "ldr	r4, [%[r], #16]\n\t"
        "ldr	r5, [%[r], #20]\n\t"
        "ldr	r6, [%[r], #24]\n\t"
        "ldr	r7, [%[r], #28]\n\t"
        "ldr	r8, [%[tmp], #16]\n\t"
        "ldr	r9, [%[tmp], #20]\n\t"
        "ldr	r10, [%[tmp], #24]\n\t"
        "ldr	r14, [%[tmp], #28]\n\t"
        "sbcs	r3, r3, r8\n\t"
        "sbcs	r4, r4, r9\n\t"
        "sbcs	r5, r5, r10\n\t"
        "sbcs	r6, r6, r14\n\t"
        "sbc	r7, r7, #0\n\t"
        "str	r3, [%[r], #12]\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "# mask m and sub from result if overflow\n\t"
        "sub	r12, %[a], r12\n\t"
        "and	%[a], r12, #1\n\t"
        "ldr	r3, [%[r], #0]\n\t"
        "ldr	r4, [%[r], #4]\n\t"
        "ldr	r5, [%[r], #8]\n\t"
        "ldr	r6, [%[r], #12]\n\t"
        "ldr	r7, [%[r], #16]\n\t"
        "ldr	r8, [%[r], #20]\n\t"
        "ldr	r9, [%[r], #24]\n\t"
        "ldr	r10, [%[r], #28]\n\t"
        "subs	r3, r3, r12\n\t"
        "sbcs	r4, r4, r12\n\t"
        "sbcs	r5, r5, r12\n\t"
        "sbcs	r6, r6, #0\n\t"
        "sbcs	r7, r7, #0\n\t"
        "sbcs	r8, r8, #0\n\t"
        "sbcs	r9, r9, %[a]\n\t"
        "sbc	r10, r10, r12\n\t"
        "str	r3, [%[r], #0]\n\t"
        "str	r4, [%[r], #4]\n\t"
        "str	r5, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "str	r7, [%[r], #16]\n\t"
        "str	r8, [%[r], #20]\n\t"
        "str	r9, [%[r], #24]\n\t"
        "str	r10, [%[r], #28]\n\t"
        : [a] "+r" (a)
        : [r] "r" (r), [tmp] "r" (tmp)
        : "memory", "r8", "r9", "r10", "r14", "r3", "r4", "r5", "r6", "r7", "r12"
    );
}

#ifndef WOLFSSL_SP_SMALL
/* Square the Montgomery form number a number of times. (r = a ^ n mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * n   Number of times to square.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_256_mont_sqr_n_8(sp_digit* r, sp_digit* a, int n,
        sp_digit* m, sp_digit mp)
{
    sp_256_mont_sqr_8(r, a, m, mp);
    for (; n > 1; n--)
        sp_256_mont_sqr_8(r, r, m, mp);
}

#else
/* Mod-2 for the P256 curve. */
static const uint32_t p256_mod_2[8] = {
    0xfffffffd,0xffffffff,0xffffffff,0x00000000,0x00000000,0x00000000,
    0x00000001,0xffffffff
};
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the modulus (prime) of the
 * P256 curve. (r = 1 / a mod m)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_8(sp_digit* r, sp_digit* a, sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 8);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_8(t, t, p256_mod, p256_mp_mod);
        if (p256_mod_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_8(t, t, a, p256_mod, p256_mp_mod);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 8);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 8;
    sp_digit* t3 = td + 4 * 8;

    /* t = a^2 */
    sp_256_mont_sqr_8(t, a, p256_mod, p256_mp_mod);
    /* t = a^3 = t * a */
    sp_256_mont_mul_8(t, t, a, p256_mod, p256_mp_mod);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_8(t2, t, 2, p256_mod, p256_mp_mod);
    /* t3= a^d = t2 * a */
    sp_256_mont_mul_8(t3, t2, a, p256_mod, p256_mp_mod);
    /* t = a^f = t2 * t */
    sp_256_mont_mul_8(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^f0 = t ^ 2 ^ 4 */
    sp_256_mont_sqr_n_8(t2, t, 4, p256_mod, p256_mp_mod);
    /* t3= a^fd = t2 * t3 */
    sp_256_mont_mul_8(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ff = t2 * t */
    sp_256_mont_mul_8(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_8(t2, t, 8, p256_mod, p256_mp_mod);
    /* t3= a^fffd = t2 * t3 */
    sp_256_mont_mul_8(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_8(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_8(t2, t, 16, p256_mod, p256_mp_mod);
    /* t3= a^fffffffd = t2 * t3 */
    sp_256_mont_mul_8(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_8(t, t2, t, p256_mod, p256_mp_mod);
    /* t = a^ffffffff00000000 = t ^ 2 ^ 32  */
    sp_256_mont_sqr_n_8(t2, t, 32, p256_mod, p256_mp_mod);
    /* t2= a^ffffffffffffffff = t2 * t */
    sp_256_mont_mul_8(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001 = t2 * a */
    sp_256_mont_mul_8(t2, t2, a, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff000000010000000000000000000000000000000000000000
     *   = t2 ^ 2 ^ 160 */
    sp_256_mont_sqr_n_8(t2, t2, 160, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff
     *   = t2 * t */
    sp_256_mont_mul_8(t2, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff00000000
     *   = t2 ^ 2 ^ 32 */
    sp_256_mont_sqr_n_8(t2, t2, 32, p256_mod, p256_mp_mod);
    /* r = a^ffffffff00000001000000000000000000000000fffffffffffffffffffffffd
     *   = t2 * t3 */
    sp_256_mont_mul_8(r, t2, t3, p256_mod, p256_mp_mod);
#endif /* WOLFSSL_SP_SMALL */
}

/* Map the Montgomery form projective co-ordinate point to an affine point.
 *
 * r  Resulting affine co-ordinate point.
 * p  Montgomery form projective co-ordinate point.
 * t  Temporary ordinate data.
 */
static void sp_256_map_8(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*8;
    int32_t n;

    sp_256_mont_inv_8(t1, p->z, t + 2*8);

    sp_256_mont_sqr_8(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_8(t1, t2, t1, p256_mod, p256_mp_mod);

    /* x /= z^2 */
    sp_256_mont_mul_8(r->x, p->x, t2, p256_mod, p256_mp_mod);
    XMEMSET(r->x + 8, 0, sizeof(r->x) / 2);
    sp_256_mont_reduce_8(r->x, p256_mod, p256_mp_mod);
    /* Reduce x to less than modulus */
    n = sp_256_cmp_8(r->x, p256_mod);
    sp_256_cond_sub_8(r->x, r->x, p256_mod, 0 - (n >= 0));
    sp_256_norm_8(r->x);

    /* y /= z^3 */
    sp_256_mont_mul_8(r->y, p->y, t1, p256_mod, p256_mp_mod);
    XMEMSET(r->y + 8, 0, sizeof(r->y) / 2);
    sp_256_mont_reduce_8(r->y, p256_mod, p256_mp_mod);
    /* Reduce y to less than modulus */
    n = sp_256_cmp_8(r->y, p256_mod);
    sp_256_cond_sub_8(r->y, r->y, p256_mod, 0 - (n >= 0));
    sp_256_norm_8(r->y);

    XMEMSET(r->z, 0, sizeof(r->z));
    r->z[0] = 1;

}

#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_256_add_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #32\n\t"
        "\n1:\n\t"
        "adds	%[c], %[c], #-1\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "mov	r4, #0\n\t"
        "adc	%[c], r4, #0\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#else
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_256_add_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a], #0]\n\t"
        "ldr	r5, [%[a], #4]\n\t"
        "ldr	r6, [%[a], #8]\n\t"
        "ldr	r7, [%[a], #12]\n\t"
        "ldr	r8, [%[b], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "ldr	r10, [%[b], #8]\n\t"
        "ldr	r14, [%[b], #12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #0]\n\t"
        "str	r5, [%[r], #4]\n\t"
        "str	r6, [%[r], #8]\n\t"
        "str	r7, [%[r], #12]\n\t"
        "ldr	r4, [%[a], #16]\n\t"
        "ldr	r5, [%[a], #20]\n\t"
        "ldr	r6, [%[a], #24]\n\t"
        "ldr	r7, [%[a], #28]\n\t"
        "ldr	r8, [%[b], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "ldr	r10, [%[b], #24]\n\t"
        "ldr	r14, [%[b], #28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r], #16]\n\t"
        "str	r5, [%[r], #20]\n\t"
        "str	r6, [%[r], #24]\n\t"
        "str	r7, [%[r], #28]\n\t"
        "adc	%[c], r12, r12\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
/* Add two Montgomery form numbers (r = a + b % m).
 *
 * r   Result of addition.
 * a   First number to add in Montogmery form.
 * b   Second number to add in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_add_8(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    (void)m;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a],#0]\n\t"
        "ldr	r5, [%[a],#4]\n\t"
        "ldr	r6, [%[a],#8]\n\t"
        "ldr	r7, [%[a],#12]\n\t"
        "ldr	r8, [%[b],#0]\n\t"
        "ldr	r9, [%[b],#4]\n\t"
        "ldr	r10, [%[b],#8]\n\t"
        "ldr	r14, [%[b],#12]\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "str	r4, [%[r],#0]\n\t"
        "str	r5, [%[r],#4]\n\t"
        "str	r6, [%[r],#8]\n\t"
        "str	r7, [%[r],#12]\n\t"
        "ldr	r4, [%[a],#16]\n\t"
        "ldr	r5, [%[a],#20]\n\t"
        "ldr	r6, [%[a],#24]\n\t"
        "ldr	r7, [%[a],#28]\n\t"
        "ldr	r8, [%[b],#16]\n\t"
        "ldr	r9, [%[b],#20]\n\t"
        "ldr	r10, [%[b],#24]\n\t"
        "ldr	r14, [%[b],#28]\n\t"
        "adcs	r4, r4, r8\n\t"
        "adcs	r5, r5, r9\n\t"
        "adcs	r6, r6, r10\n\t"
        "adcs	r7, r7, r14\n\t"
        "adc	r3, r12, #0\n\t"
        "sub	r3, r12, r3\n\t"
        "and	r12, r3, #1\n\t"
        "ldr	r8, [%[r],#0]\n\t"
        "ldr	r9, [%[r],#4]\n\t"
        "ldr	r10, [%[r],#8]\n\t"
        "ldr	r14, [%[r],#12]\n\t"
        "subs	r8, r8, r3\n\t"
        "sbcs	r9, r9, r3\n\t"
        "sbcs	r10, r10, r3\n\t"
        "sbcs	r14, r14, #0\n\t"
        "sbcs	r4, r4, #0\n\t"
        "sbcs	r5, r5, #0\n\t"
        "sbcs	r6, r6, r12\n\t"
        "sbc	r7, r7, r3\n\t"
        "str	r8, [%[r],#0]\n\t"
        "str	r9, [%[r],#4]\n\t"
        "str	r10, [%[r],#8]\n\t"
        "str	r14, [%[r],#12]\n\t"
        "str	r4, [%[r],#16]\n\t"
        "str	r5, [%[r],#20]\n\t"
        "str	r6, [%[r],#24]\n\t"
        "str	r7, [%[r],#28]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r3", "r12"
    );
}

/* Double a Montgomery form number (r = a + a % m).
 *
 * r   Result of doubling.
 * a   Number to double in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_dbl_8(sp_digit* r, sp_digit* a, sp_digit* m)
{
    (void)m;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a],#0]\n\t"
        "ldr	r5, [%[a],#4]\n\t"
        "ldr	r6, [%[a],#8]\n\t"
        "ldr	r7, [%[a],#12]\n\t"
        "ldr	r8, [%[a],#16]\n\t"
        "ldr	r9, [%[a],#20]\n\t"
        "ldr	r10, [%[a],#24]\n\t"
        "ldr	r14, [%[a],#28]\n\t"
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adcs	r14, r14, r14\n\t"
        "adc	r3, r12, #0\n\t"
        "sub	r3, r12, r3\n\t"
        "and	r12, r3, #1\n\t"
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, r3\n\t"
        "sbcs	r6, r6, r3\n\t"
        "sbcs	r7, r7, #0\n\t"
        "sbcs	r8, r8, #0\n\t"
        "sbcs	r9, r9, #0\n\t"
        "sbcs	r10, r10, r12\n\t"
        "sbc	r14, r14, r3\n\t"
        "str	r4, [%[r],#0]\n\t"
        "str	r5, [%[r],#4]\n\t"
        "str	r6, [%[r],#8]\n\t"
        "str	r7, [%[r],#12]\n\t"
        "str	r8, [%[r],#16]\n\t"
        "str	r9, [%[r],#20]\n\t"
        "str	r10, [%[r],#24]\n\t"
        "str	r14, [%[r],#28]\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r3", "r12"
    );
}

/* Triple a Montgomery form number (r = a + a + a % m).
 *
 * r   Result of Tripling.
 * a   Number to triple in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_tpl_8(sp_digit* r, sp_digit* a, sp_digit* m)
{
    (void)m;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a],#0]\n\t"
        "ldr	r5, [%[a],#4]\n\t"
        "ldr	r6, [%[a],#8]\n\t"
        "ldr	r7, [%[a],#12]\n\t"
        "ldr	r8, [%[a],#16]\n\t"
        "ldr	r9, [%[a],#20]\n\t"
        "ldr	r10, [%[a],#24]\n\t"
        "ldr	r14, [%[a],#28]\n\t"
        "adds	r4, r4, r4\n\t"
        "adcs	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adcs	r7, r7, r7\n\t"
        "adcs	r8, r8, r8\n\t"
        "adcs	r9, r9, r9\n\t"
        "adcs	r10, r10, r10\n\t"
        "adcs	r14, r14, r14\n\t"
        "adc	r3, r12, #0\n\t"
        "sub	r3, r12, r3\n\t"
        "and	r12, r3, #1\n\t"
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, r3\n\t"
        "sbcs	r6, r6, r3\n\t"
        "sbcs	r7, r7, #0\n\t"
        "sbcs	r8, r8, #0\n\t"
        "sbcs	r9, r9, #0\n\t"
        "sbcs	r10, r10, r12\n\t"
        "sbc	r14, r14, r3\n\t"
        "str	r8, [%[r],#16]\n\t"
        "str	r9, [%[r],#20]\n\t"
        "str	r10, [%[r],#24]\n\t"
        "str	r14, [%[r],#28]\n\t"
        "mov	r12, #0\n\t"
        "ldr	r8, [%[a],#0]\n\t"
        "ldr	r9, [%[a],#4]\n\t"
        "ldr	r10, [%[a],#8]\n\t"
        "ldr	r14, [%[a],#12]\n\t"
        "adds	r8, r8, r4\n\t"
        "adcs	r9, r9, r5\n\t"
        "adcs	r10, r10, r6\n\t"
        "adcs	r14, r14, r7\n\t"
        "str	r8, [%[r],#0]\n\t"
        "str	r9, [%[r],#4]\n\t"
        "str	r10, [%[r],#8]\n\t"
        "str	r14, [%[r],#12]\n\t"
        "ldr	r8, [%[a],#16]\n\t"
        "ldr	r9, [%[a],#20]\n\t"
        "ldr	r10, [%[a],#24]\n\t"
        "ldr	r14, [%[a],#28]\n\t"
        "ldr	r4, [%[r],#16]\n\t"
        "ldr	r5, [%[r],#20]\n\t"
        "ldr	r6, [%[r],#24]\n\t"
        "ldr	r7, [%[r],#28]\n\t"
        "adcs	r8, r8, r4\n\t"
        "adcs	r9, r9, r5\n\t"
        "adcs	r10, r10, r6\n\t"
        "adcs	r14, r14, r7\n\t"
        "adc	r3, r12, #0\n\t"
        "sub	r3, r12, r3\n\t"
        "and	r12, r3, #1\n\t"
        "ldr	r4, [%[r],#0]\n\t"
        "ldr	r5, [%[r],#4]\n\t"
        "ldr	r6, [%[r],#8]\n\t"
        "ldr	r7, [%[r],#12]\n\t"
        "subs	r4, r4, r3\n\t"
        "sbcs	r5, r5, r3\n\t"
        "sbcs	r6, r6, r3\n\t"
        "sbcs	r7, r7, #0\n\t"
        "sbcs	r8, r8, #0\n\t"
        "sbcs	r9, r9, #0\n\t"
        "sbcs	r10, r10, r12\n\t"
        "sbc	r14, r14, r3\n\t"
        "str	r4, [%[r],#0]\n\t"
        "str	r5, [%[r],#4]\n\t"
        "str	r6, [%[r],#8]\n\t"
        "str	r7, [%[r],#12]\n\t"
        "str	r8, [%[r],#16]\n\t"
        "str	r9, [%[r],#20]\n\t"
        "str	r10, [%[r],#24]\n\t"
        "str	r14, [%[r],#28]\n\t"
        :
        : [r] "r" (r), [a] "r" (a)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r3", "r12"
    );
}

/* Subtract two Montgomery form numbers (r = a - b % m).
 *
 * r   Result of subtration.
 * a   Number to subtract from in Montogmery form.
 * b   Number to subtract with in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_sub_8(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    (void)m;

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "ldr	r4, [%[a],#0]\n\t"
        "ldr	r5, [%[a],#4]\n\t"
        "ldr	r6, [%[a],#8]\n\t"
        "ldr	r7, [%[a],#12]\n\t"
        "ldr	r8, [%[b],#0]\n\t"
        "ldr	r9, [%[b],#4]\n\t"
        "ldr	r10, [%[b],#8]\n\t"
        "ldr	r14, [%[b],#12]\n\t"
        "subs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "sbcs	r7, r7, r14\n\t"
        "str	r4, [%[r],#0]\n\t"
        "str	r5, [%[r],#4]\n\t"
        "str	r6, [%[r],#8]\n\t"
        "str	r7, [%[r],#12]\n\t"
        "ldr	r4, [%[a],#16]\n\t"
        "ldr	r5, [%[a],#20]\n\t"
        "ldr	r6, [%[a],#24]\n\t"
        "ldr	r7, [%[a],#28]\n\t"
        "ldr	r8, [%[b],#16]\n\t"
        "ldr	r9, [%[b],#20]\n\t"
        "ldr	r10, [%[b],#24]\n\t"
        "ldr	r14, [%[b],#28]\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "sbcs	r7, r7, r14\n\t"
        "sbc	r3, r12, #0\n\t"
        "and	r12, r3, #1\n\t"
        "ldr	r8, [%[r],#0]\n\t"
        "ldr	r9, [%[r],#4]\n\t"
        "ldr	r10, [%[r],#8]\n\t"
        "ldr	r14, [%[r],#12]\n\t"
        "adds	r8, r8, r3\n\t"
        "adcs	r9, r9, r3\n\t"
        "adcs	r10, r10, r3\n\t"
        "adcs	r14, r14, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "adcs	r5, r5, #0\n\t"
        "adcs	r6, r6, r12\n\t"
        "adc	r7, r7, r3\n\t"
        "str	r8, [%[r],#0]\n\t"
        "str	r9, [%[r],#4]\n\t"
        "str	r10, [%[r],#8]\n\t"
        "str	r14, [%[r],#12]\n\t"
        "str	r4, [%[r],#16]\n\t"
        "str	r5, [%[r],#20]\n\t"
        "str	r6, [%[r],#24]\n\t"
        "str	r7, [%[r],#28]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r3", "r12"
    );
}

/* Divide the number by 2 mod the modulus (prime). (r = a / 2 % m)
 *
 * r  Result of division by 2.
 * a  Number to divide.
 * m  Modulus (prime).
 */
static void sp_256_div2_8(sp_digit* r, sp_digit* a, sp_digit* m)
{
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "ldr	r3, [%[a], #0]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "and	r9, r3, #1\n\t"
        "sub	r7, r10, r9\n\t"
        "and	r8, r7, #1\n\t"
        "adds	r3, r3, r7\n\t"
        "adcs	r4, r4, r7\n\t"
        "adcs	r5, r5, r7\n\t"
        "adcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #0]\n\t"
        "str	r4, [%[r], #4]\n\t"
        "str	r5, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r3, [%[a], #16]\n\t"
        "ldr	r4, [%[a], #20]\n\t"
        "ldr	r5, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "adcs	r3, r3, r10\n\t"
        "adcs	r4, r4, r10\n\t"
        "adcs	r5, r5, r8\n\t"
        "adcs	r6, r6, r7\n\t"
        "adc	r9, r10, r10\n\t"
        "lsr	r7, r3, #1\n\t"
        "and	r3, r3, #1\n\t"
        "lsr	r8, r4, #1\n\t"
        "lsr	r10, r5, #1\n\t"
        "lsr	r14, r6, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r8, r8, r5, lsl #31\n\t"
        "orr	r10, r10, r6, lsl #31\n\t"
        "orr	r14, r14, r9, lsl #31\n\t"
        "mov	r9, r3\n\t"
        "str	r7, [%[r], #16]\n\t"
        "str	r8, [%[r], #20]\n\t"
        "str	r10, [%[r], #24]\n\t"
        "str	r14, [%[r], #28]\n\t"
        "ldr	r3, [%[r], #0]\n\t"
        "ldr	r4, [%[r], #4]\n\t"
        "ldr	r5, [%[r], #8]\n\t"
        "ldr	r6, [%[r], #12]\n\t"
        "lsr	r7, r3, #1\n\t"
        "lsr	r8, r4, #1\n\t"
        "lsr	r10, r5, #1\n\t"
        "lsr	r14, r6, #1\n\t"
        "orr	r7, r7, r4, lsl #31\n\t"
        "orr	r8, r8, r5, lsl #31\n\t"
        "orr	r10, r10, r6, lsl #31\n\t"
        "orr	r14, r14, r9, lsl #31\n\t"
        "str	r7, [%[r], #0]\n\t"
        "str	r8, [%[r], #4]\n\t"
        "str	r10, [%[r], #8]\n\t"
        "str	r14, [%[r], #12]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [m] "r" (m)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r10", "r14", "r9"
    );

}

/* Double the Montgomery form projective point p.
 *
 * r  Result of doubling point.
 * p  Point to double.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_dbl_8(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*8;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    /* When infinity don't double point passed in - constant time. */
    rp[0] = r;
    rp[1] = &tp;
    x = rp[p->infinity]->x;
    y = rp[p->infinity]->y;
    z = rp[p->infinity]->z;
    /* Put point to double into result - good for infinty. */
    if (r != p) {
        for (i=0; i<8; i++)
            r->x[i] = p->x[i];
        for (i=0; i<8; i++)
            r->y[i] = p->y[i];
        for (i=0; i<8; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* T1 = Z * Z */
    sp_256_mont_sqr_8(t1, z, p256_mod, p256_mp_mod);
    /* Z = Y * Z */
    sp_256_mont_mul_8(z, y, z, p256_mod, p256_mp_mod);
    /* Z = 2Z */
    sp_256_mont_dbl_8(z, z, p256_mod);
    /* T2 = X - T1 */
    sp_256_mont_sub_8(t2, x, t1, p256_mod);
    /* T1 = X + T1 */
    sp_256_mont_add_8(t1, x, t1, p256_mod);
    /* T2 = T1 * T2 */
    sp_256_mont_mul_8(t2, t1, t2, p256_mod, p256_mp_mod);
    /* T1 = 3T2 */
    sp_256_mont_tpl_8(t1, t2, p256_mod);
    /* Y = 2Y */
    sp_256_mont_dbl_8(y, y, p256_mod);
    /* Y = Y * Y */
    sp_256_mont_sqr_8(y, y, p256_mod, p256_mp_mod);
    /* T2 = Y * Y */
    sp_256_mont_sqr_8(t2, y, p256_mod, p256_mp_mod);
    /* T2 = T2/2 */
    sp_256_div2_8(t2, t2, p256_mod);
    /* Y = Y * X */
    sp_256_mont_mul_8(y, y, x, p256_mod, p256_mp_mod);
    /* X = T1 * T1 */
    sp_256_mont_mul_8(x, t1, t1, p256_mod, p256_mp_mod);
    /* X = X - Y */
    sp_256_mont_sub_8(x, x, y, p256_mod);
    /* X = X - Y */
    sp_256_mont_sub_8(x, x, y, p256_mod);
    /* Y = Y - X */
    sp_256_mont_sub_8(y, y, x, p256_mod);
    /* Y = Y * T1 */
    sp_256_mont_mul_8(y, y, t1, p256_mod, p256_mp_mod);
    /* Y = Y - T2 */
    sp_256_mont_sub_8(y, y, t2, p256_mod);

}

#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_256_sub_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "add	r12, %[a], #32\n\t"
        "\n1:\n\t"
        "rsbs	%[c], %[c], #0\n\t"
        "ldr	r4, [%[a]], #4\n\t"
        "ldr	r5, [%[a]], #4\n\t"
        "ldr	r6, [%[a]], #4\n\t"
        "ldr	r7, [%[a]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "ldr	r14, [%[b]], #4\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "sbcs	r7, r7, r14\n\t"
        "str	r4, [%[r]], #4\n\t"
        "str	r5, [%[r]], #4\n\t"
        "str	r6, [%[r]], #4\n\t"
        "str	r7, [%[r]], #4\n\t"
        "sbc	%[c], r4, r4\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [r] "+r" (r), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    return c;
}

#else
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_256_sub_8(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r3, [%[a], #0]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b], #0]\n\t"
        "ldr	r8, [%[b], #4]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "ldr	r10, [%[b], #12]\n\t"
        "subs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #0]\n\t"
        "str	r4, [%[r], #4]\n\t"
        "str	r5, [%[r], #8]\n\t"
        "str	r6, [%[r], #12]\n\t"
        "ldr	r3, [%[a], #16]\n\t"
        "ldr	r4, [%[a], #20]\n\t"
        "ldr	r5, [%[a], #24]\n\t"
        "ldr	r6, [%[a], #28]\n\t"
        "ldr	r7, [%[b], #16]\n\t"
        "ldr	r8, [%[b], #20]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "ldr	r10, [%[b], #28]\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[r], #16]\n\t"
        "str	r4, [%[r], #20]\n\t"
        "str	r5, [%[r], #24]\n\t"
        "str	r6, [%[r], #28]\n\t"
        "sbc	%[c], %[c], #0\n\t"
        : [c] "+r" (c)
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
/* Compare two numbers to determine if they are equal.
 * Constant time implementation.
 *
 * a  First number to compare.
 * b  Second number to compare.
 * returns 1 when equal and 0 otherwise.
 */
static int sp_256_cmp_equal_8(const sp_digit* a, const sp_digit* b)
{
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ b[3]) |
            (a[4] ^ b[4]) | (a[5] ^ b[5]) | (a[6] ^ b[6]) | (a[7] ^ b[7])) == 0;
}

/* Add two Montgomery form projective points.
 *
 * r  Result of addition.
 * p  Frist point to add.
 * q  Second point to add.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_add_8(sp_point* r, sp_point* p, sp_point* q,
        sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*8;
    sp_digit* t3 = t + 4*8;
    sp_digit* t4 = t + 6*8;
    sp_digit* t5 = t + 8*8;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    /* Ensure only the first point is the same as the result. */
    if (q == r) {
        sp_point* a = p;
        p = q;
        q = a;
    }

    /* Check double */
    sp_256_sub_8(t1, p256_mod, q->y);
    sp_256_norm_8(t1);
    if (sp_256_cmp_equal_8(p->x, q->x) & sp_256_cmp_equal_8(p->z, q->z) &
        (sp_256_cmp_equal_8(p->y, q->y) | sp_256_cmp_equal_8(p->y, t1))) {
        sp_256_proj_point_dbl_8(r, p, t);
    }
    else {
        rp[0] = r;
        rp[1] = &tp;
        XMEMSET(&tp, 0, sizeof(tp));
        x = rp[p->infinity | q->infinity]->x;
        y = rp[p->infinity | q->infinity]->y;
        z = rp[p->infinity | q->infinity]->z;

        ap[0] = p;
        ap[1] = q;
        for (i=0; i<8; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<8; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<8; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U1 = X1*Z2^2 */
        sp_256_mont_sqr_8(t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t3, t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t1, t1, x, p256_mod, p256_mp_mod);
        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_8(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S1 = Y1*Z2^3 */
        sp_256_mont_mul_8(t3, t3, y, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_8(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - U1 */
        sp_256_mont_sub_8(t2, t2, t1, p256_mod);
        /* R = S2 - S1 */
        sp_256_mont_sub_8(t4, t4, t3, p256_mod);
        /* Z3 = H*Z1*Z2 */
        sp_256_mont_mul_8(z, z, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*U1*H^2 */
        sp_256_mont_sqr_8(x, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_8(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(y, t1, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_8(x, x, t5, p256_mod);
        sp_256_mont_dbl_8(t1, y, p256_mod);
        sp_256_mont_sub_8(x, x, t1, p256_mod);
        /* Y3 = R*(U1*H^2 - X3) - S1*H^3 */
        sp_256_mont_sub_8(y, y, x, p256_mod);
        sp_256_mont_mul_8(y, y, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t5, t5, t3, p256_mod, p256_mp_mod);
        sp_256_mont_sub_8(y, y, t5, p256_mod);
    }
}

/* Multiply the point by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * g     Point to multiply.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_fast_8(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td[16];
    sp_point rtd;
    sp_digit tmpd[2 * 8 * 5];
#endif
    sp_point* t;
    sp_point* rt;
    sp_digit* tmp;
    sp_digit n;
    int i;
    int c, y;
    int err;

    (void)heap;

    err = sp_ecc_point_new(heap, rtd, rt);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    t = (sp_point*)XMALLOC(sizeof(sp_point) * 16, heap, DYNAMIC_TYPE_ECC);
    if (t == NULL)
        err = MEMORY_E;
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 8 * 5, heap,
                             DYNAMIC_TYPE_ECC);
    if (tmp == NULL)
        err = MEMORY_E;
#else
    t = td;
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        /* t[0] = {0, 0, 1} * norm */
        XMEMSET(&t[0], 0, sizeof(t[0]));
        t[0].infinity = 1;
        /* t[1] = {g->x, g->y, g->z} * norm */
        sp_256_mod_mul_norm_8(t[1].x, g->x, p256_mod);
        sp_256_mod_mul_norm_8(t[1].y, g->y, p256_mod);
        sp_256_mod_mul_norm_8(t[1].z, g->z, p256_mod);
        t[1].infinity = 0;
        sp_256_proj_point_dbl_8(&t[ 2], &t[ 1], tmp);
        t[ 2].infinity = 0;
        sp_256_proj_point_add_8(&t[ 3], &t[ 2], &t[ 1], tmp);
        t[ 3].infinity = 0;
        sp_256_proj_point_dbl_8(&t[ 4], &t[ 2], tmp);
        t[ 4].infinity = 0;
        sp_256_proj_point_add_8(&t[ 5], &t[ 3], &t[ 2], tmp);
        t[ 5].infinity = 0;
        sp_256_proj_point_dbl_8(&t[ 6], &t[ 3], tmp);
        t[ 6].infinity = 0;
        sp_256_proj_point_add_8(&t[ 7], &t[ 4], &t[ 3], tmp);
        t[ 7].infinity = 0;
        sp_256_proj_point_dbl_8(&t[ 8], &t[ 4], tmp);
        t[ 8].infinity = 0;
        sp_256_proj_point_add_8(&t[ 9], &t[ 5], &t[ 4], tmp);
        t[ 9].infinity = 0;
        sp_256_proj_point_dbl_8(&t[10], &t[ 5], tmp);
        t[10].infinity = 0;
        sp_256_proj_point_add_8(&t[11], &t[ 6], &t[ 5], tmp);
        t[11].infinity = 0;
        sp_256_proj_point_dbl_8(&t[12], &t[ 6], tmp);
        t[12].infinity = 0;
        sp_256_proj_point_add_8(&t[13], &t[ 7], &t[ 6], tmp);
        t[13].infinity = 0;
        sp_256_proj_point_dbl_8(&t[14], &t[ 7], tmp);
        t[14].infinity = 0;
        sp_256_proj_point_add_8(&t[15], &t[ 8], &t[ 7], tmp);
        t[15].infinity = 0;

        i = 6;
        n = k[i+1] << 0;
        c = 28;
        y = n >> 28;
        XMEMCPY(rt, &t[y], sizeof(sp_point));
        n <<= 4;
        for (; i>=0 || c>=4; ) {
            if (c < 4) {
                n |= k[i--] << (0 - c);
                c += 32;
            }
            y = (n >> 28) & 0xf;
            n <<= 4;
            c -= 4;

            sp_256_proj_point_dbl_8(rt, rt, tmp);
            sp_256_proj_point_dbl_8(rt, rt, tmp);
            sp_256_proj_point_dbl_8(rt, rt, tmp);
            sp_256_proj_point_dbl_8(rt, rt, tmp);

            sp_256_proj_point_add_8(rt, rt, &t[y], tmp);
        }

        if (map)
            sp_256_map_8(r, rt, tmp);
        else
            XMEMCPY(r, rt, sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 8 * 5);
        XFREE(tmp, heap, DYNAMIC_TYPE_ECC);
    }
    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_point) * 16);
        XFREE(t, heap, DYNAMIC_TYPE_ECC);
    }
#else
    ForceZero(tmpd, sizeof(tmpd));
    ForceZero(td, sizeof(td));
#endif
    sp_ecc_point_free(rt, 1, heap);

    return err;
}

/* A table entry for pre-computed points. */
typedef struct sp_table_entry {
    sp_digit x[8];
    sp_digit y[8];
    byte infinity;
} sp_table_entry;

#ifdef FP_ECC
/* Double the Montgomery form projective point p a number of times.
 *
 * r  Result of repeated doubling of point.
 * p  Point to double.
 * n  Number of times to double
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_dbl_n_8(sp_point* r, sp_point* p, int n,
        sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* w = t;
    sp_digit* a = t + 2*8;
    sp_digit* b = t + 4*8;
    sp_digit* t1 = t + 6*8;
    sp_digit* t2 = t + 8*8;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    rp[0] = r;
    rp[1] = &tp;
    x = rp[p->infinity]->x;
    y = rp[p->infinity]->y;
    z = rp[p->infinity]->z;
    if (r != p) {
        for (i=0; i<8; i++)
            r->x[i] = p->x[i];
        for (i=0; i<8; i++)
            r->y[i] = p->y[i];
        for (i=0; i<8; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* Y = 2*Y */
    sp_256_mont_dbl_8(y, y, p256_mod);
    /* W = Z^4 */
    sp_256_mont_sqr_8(w, z, p256_mod, p256_mp_mod);
    sp_256_mont_sqr_8(w, w, p256_mod, p256_mp_mod);
    while (n--) {
        /* A = 3*(X^2 - W) */
        sp_256_mont_sqr_8(t1, x, p256_mod, p256_mp_mod);
        sp_256_mont_sub_8(t1, t1, w, p256_mod);
        sp_256_mont_tpl_8(a, t1, p256_mod);
        /* B = X*Y^2 */
        sp_256_mont_sqr_8(t2, y, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(b, t2, x, p256_mod, p256_mp_mod);
        /* X = A^2 - 2B */
        sp_256_mont_sqr_8(x, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_8(t1, b, p256_mod);
        sp_256_mont_sub_8(x, x, t1, p256_mod);
        /* Z = Z*Y */
        sp_256_mont_mul_8(z, z, y, p256_mod, p256_mp_mod);
        /* t2 = Y^4 */
        sp_256_mont_sqr_8(t2, t2, p256_mod, p256_mp_mod);
        if (n) {
            /* W = W*Y^4 */
            sp_256_mont_mul_8(w, w, t2, p256_mod, p256_mp_mod);
        }
        /* y = 2*A*(B - X) - Y^4 */
        sp_256_mont_sub_8(y, b, x, p256_mod);
        sp_256_mont_mul_8(y, y, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_8(y, y, p256_mod);
        sp_256_mont_sub_8(y, y, t2, p256_mod);
    }
    /* Y = Y/2 */
    sp_256_div2_8(y, y, p256_mod);
}

#endif /* FP_ECC */
/* Add two Montgomery form projective points. The second point has a q value of
 * one.
 * Only the first point can be the same pointer as the result point.
 *
 * r  Result of addition.
 * p  Frist point to add.
 * q  Second point to add.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_add_qz1_8(sp_point* r, sp_point* p,
        sp_point* q, sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*8;
    sp_digit* t3 = t + 4*8;
    sp_digit* t4 = t + 6*8;
    sp_digit* t5 = t + 8*8;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    /* Check double */
    sp_256_sub_8(t1, p256_mod, q->y);
    sp_256_norm_8(t1);
    if (sp_256_cmp_equal_8(p->x, q->x) & sp_256_cmp_equal_8(p->z, q->z) &
        (sp_256_cmp_equal_8(p->y, q->y) | sp_256_cmp_equal_8(p->y, t1))) {
        sp_256_proj_point_dbl_8(r, p, t);
    }
    else {
        rp[0] = r;
        rp[1] = &tp;
        XMEMSET(&tp, 0, sizeof(tp));
        x = rp[p->infinity | q->infinity]->x;
        y = rp[p->infinity | q->infinity]->y;
        z = rp[p->infinity | q->infinity]->z;

        ap[0] = p;
        ap[1] = q;
        for (i=0; i<8; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<8; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<8; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_8(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_8(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - X1 */
        sp_256_mont_sub_8(t2, t2, x, p256_mod);
        /* R = S2 - Y1 */
        sp_256_mont_sub_8(t4, t4, y, p256_mod);
        /* Z3 = H*Z1 */
        sp_256_mont_mul_8(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*X1*H^2 */
        sp_256_mont_sqr_8(t1, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_8(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t3, x, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_8(x, t1, t5, p256_mod);
        sp_256_mont_dbl_8(t1, t3, p256_mod);
        sp_256_mont_sub_8(x, x, t1, p256_mod);
        /* Y3 = R*(X1*H^2 - X3) - Y1*H^3 */
        sp_256_mont_sub_8(t3, t3, x, p256_mod);
        sp_256_mont_mul_8(t3, t3, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(t5, t5, y, p256_mod, p256_mp_mod);
        sp_256_mont_sub_8(y, t3, t5, p256_mod);
    }
}

#ifdef WOLFSSL_SP_SMALL
#ifdef FP_ECC
/* Convert the projective point to affine.
 * Ordinates are in Montgomery form.
 *
 * a  Point to convert.
 * t  Temprorary data.
 */
static void sp_256_proj_to_affine_8(sp_point* a, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2 * 8;
    sp_digit* tmp = t + 4 * 8;

    sp_256_mont_inv_8(t1, a->z, tmp);

    sp_256_mont_sqr_8(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_8(t1, t2, t1, p256_mod, p256_mp_mod);

    sp_256_mont_mul_8(a->x, a->x, t2, p256_mod, p256_mp_mod);
    sp_256_mont_mul_8(a->y, a->y, t1, p256_mod, p256_mp_mod);
    XMEMCPY(a->z, p256_norm_mod, sizeof(p256_norm_mod));
}

/* Generate the pre-computed table of points for the base point.
 *
 * a      The base point.
 * table  Place to store generated point data.
 * tmp    Temprorary data.
 * heap  Heap to use for allocation.
 */
static int sp_256_gen_stripe_table_8(sp_point* a,
        sp_table_entry* table, sp_digit* tmp, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td, s1d, s2d;
#endif
    sp_point* t;
    sp_point* s1 = NULL;
    sp_point* s2 = NULL;
    int i, j;
    int err;

    (void)heap;

    err = sp_ecc_point_new(heap, td, t);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, s1d, s1);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, s2d, s2);

    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->x, a->x, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->y, a->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->z, a->z, p256_mod);
    if (err == MP_OKAY) {
        t->infinity = 0;
        sp_256_proj_to_affine_8(t, tmp);

        XMEMCPY(s1->z, p256_norm_mod, sizeof(p256_norm_mod));
        s1->infinity = 0;
        XMEMCPY(s2->z, p256_norm_mod, sizeof(p256_norm_mod));
        s2->infinity = 0;

        /* table[0] = {0, 0, infinity} */
        XMEMSET(&table[0], 0, sizeof(sp_table_entry));
        table[0].infinity = 1;
        /* table[1] = Affine version of 'a' in Montgomery form */
        XMEMCPY(table[1].x, t->x, sizeof(table->x));
        XMEMCPY(table[1].y, t->y, sizeof(table->y));
        table[1].infinity = 0;

        for (i=1; i<4; i++) {
            sp_256_proj_point_dbl_n_8(t, t, 64, tmp);
            sp_256_proj_to_affine_8(t, tmp);
            XMEMCPY(table[1<<i].x, t->x, sizeof(table->x));
            XMEMCPY(table[1<<i].y, t->y, sizeof(table->y));
            table[1<<i].infinity = 0;
        }

        for (i=1; i<4; i++) {
            XMEMCPY(s1->x, table[1<<i].x, sizeof(table->x));
            XMEMCPY(s1->y, table[1<<i].y, sizeof(table->y));
            for (j=(1<<i)+1; j<(1<<(i+1)); j++) {
                XMEMCPY(s2->x, table[j-(1<<i)].x, sizeof(table->x));
                XMEMCPY(s2->y, table[j-(1<<i)].y, sizeof(table->y));
                sp_256_proj_point_add_qz1_8(t, s1, s2, tmp);
                sp_256_proj_to_affine_8(t, tmp);
                XMEMCPY(table[j].x, t->x, sizeof(table->x));
                XMEMCPY(table[j].y, t->y, sizeof(table->y));
                table[j].infinity = 0;
            }
        }
    }

    sp_ecc_point_free(s2, 0, heap);
    sp_ecc_point_free(s1, 0, heap);
    sp_ecc_point_free( t, 0, heap);

    return err;
}

#endif /* FP_ECC */
/* Multiply the point by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_stripe_8(sp_point* r, sp_point* g,
        sp_table_entry* table, sp_digit* k, int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point rtd;
    sp_point pd;
    sp_digit td[2 * 8 * 5];
#endif
    sp_point* rt;
    sp_point* p = NULL;
    sp_digit* t;
    int i, j;
    int y, x;
    int err;

    (void)g;
    (void)heap;

    err = sp_ecc_point_new(heap, rtd, rt);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 8 * 5, heap,
                           DYNAMIC_TYPE_ECC);
    if (t == NULL)
        err = MEMORY_E;
#else
    t = td;
#endif

    if (err == MP_OKAY) {
        XMEMCPY(p->z, p256_norm_mod, sizeof(p256_norm_mod));
        XMEMCPY(rt->z, p256_norm_mod, sizeof(p256_norm_mod));

        y = 0;
        for (j=0,x=63; j<4; j++,x+=32)
            y |= ((k[x / 32] >> (x % 32)) & 1) << j;
        XMEMCPY(rt->x, table[y].x, sizeof(table[y].x));
        XMEMCPY(rt->y, table[y].y, sizeof(table[y].y));
        rt->infinity = table[y].infinity;
        for (i=62; i>=0; i--) {
            y = 0;
            for (j=0,x=i; j<4; j++,x+=64)
                y |= ((k[x / 32] >> (x % 32)) & 1) << j;

            sp_256_proj_point_dbl_8(rt, rt, t);
            XMEMCPY(p->x, table[y].x, sizeof(table[y].x));
            XMEMCPY(p->y, table[y].y, sizeof(table[y].y));
            p->infinity = table[y].infinity;
            sp_256_proj_point_add_qz1_8(rt, rt, p, t);
        }

        if (map)
            sp_256_map_8(r, rt, t);
        else
            XMEMCPY(r, rt, sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (t != NULL)
        XFREE(t, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p, 0, heap);
    sp_ecc_point_free(rt, 0, heap);

    return err;
}

#ifdef FP_ECC
#ifndef FP_ENTRIES
    #define FP_ENTRIES 16
#endif

typedef struct sp_cache_t {
    sp_digit x[8];
    sp_digit y[8];
    sp_table_entry table[16];
    uint32_t cnt;
    int set;
} sp_cache_t;

static THREAD_LS_T sp_cache_t sp_cache[FP_ENTRIES];
static THREAD_LS_T int sp_cache_last = -1;
static THREAD_LS_T int sp_cache_inited = 0;

#ifndef HAVE_THREAD_LS
    static volatile int initCacheMutex = 0;
    static wolfSSL_Mutex sp_cache_lock;
#endif

static void sp_ecc_get_cache(sp_point* g, sp_cache_t** cache)
{
    int i, j;
    uint32_t least;

    if (sp_cache_inited == 0) {
        for (i=0; i<FP_ENTRIES; i++) {
            sp_cache[i].set = 0;
        }
        sp_cache_inited = 1;
    }

    /* Compare point with those in cache. */
    for (i=0; i<FP_ENTRIES; i++) {
        if (!sp_cache[i].set)
            continue;

        if (sp_256_cmp_equal_8(g->x, sp_cache[i].x) & 
                           sp_256_cmp_equal_8(g->y, sp_cache[i].y)) {
            sp_cache[i].cnt++;
            break;
        }
    }

    /* No match. */
    if (i == FP_ENTRIES) {
        /* Find empty entry. */
        i = (sp_cache_last + 1) % FP_ENTRIES;
        for (; i != sp_cache_last; i=(i+1)%FP_ENTRIES) {
            if (!sp_cache[i].set) {
                break;
            }
        }

        /* Evict least used. */
        if (i == sp_cache_last) {
            least = sp_cache[0].cnt;
            for (j=1; j<FP_ENTRIES; j++) {
                if (sp_cache[j].cnt < least) {
                    i = j;
                    least = sp_cache[i].cnt;
                }
            }
        }

        XMEMCPY(sp_cache[i].x, g->x, sizeof(sp_cache[i].x));
        XMEMCPY(sp_cache[i].y, g->y, sizeof(sp_cache[i].y));
        sp_cache[i].set = 1;
        sp_cache[i].cnt = 1;
    }

    *cache = &sp_cache[i];
    sp_cache_last = i;
}
#endif /* FP_ECC */

/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * g     Point to multiply.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_8(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#ifndef FP_ECC
    return sp_256_ecc_mulmod_fast_8(r, g, k, map, heap);
#else
    sp_digit tmp[2 * 8 * 5];
    sp_cache_t* cache;
    int err = MP_OKAY;

#ifndef HAVE_THREAD_LS
    if (initCacheMutex == 0) {
         wc_InitMutex(&sp_cache_lock);
         initCacheMutex = 1;
    }
    if (wc_LockMutex(&sp_cache_lock) != 0)
       err = BAD_MUTEX_E;
#endif /* HAVE_THREAD_LS */

    if (err == MP_OKAY) {
        sp_ecc_get_cache(g, &cache);
        if (cache->cnt == 2)
            sp_256_gen_stripe_table_8(g, cache->table, tmp, heap);

#ifndef HAVE_THREAD_LS
        wc_UnLockMutex(&sp_cache_lock);
#endif /* HAVE_THREAD_LS */

        if (cache->cnt < 2) {
            err = sp_256_ecc_mulmod_fast_8(r, g, k, map, heap);
        }
        else {
            err = sp_256_ecc_mulmod_stripe_8(r, g, cache->table, k,
                    map, heap);
        }
    }

    return err;
#endif
}

#else
#ifdef FP_ECC
/* Generate the pre-computed table of points for the base point.
 *
 * a      The base point.
 * table  Place to store generated point data.
 * tmp    Temprorary data.
 * heap  Heap to use for allocation.
 */
static int sp_256_gen_stripe_table_8(sp_point* a,
        sp_table_entry* table, sp_digit* tmp, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td, s1d, s2d;
#endif
    sp_point* t;
    sp_point* s1 = NULL;
    sp_point* s2 = NULL;
    int i, j;
    int err;

    (void)heap;

    err = sp_ecc_point_new(heap, td, t);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, s1d, s1);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, s2d, s2);

    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->x, a->x, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->y, a->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_8(t->z, a->z, p256_mod);
    if (err == MP_OKAY) {
        t->infinity = 0;
        sp_256_proj_to_affine_8(t, tmp);

        XMEMCPY(s1->z, p256_norm_mod, sizeof(p256_norm_mod));
        s1->infinity = 0;
        XMEMCPY(s2->z, p256_norm_mod, sizeof(p256_norm_mod));
        s2->infinity = 0;

        /* table[0] = {0, 0, infinity} */
        XMEMSET(&table[0], 0, sizeof(sp_table_entry));
        table[0].infinity = 1;
        /* table[1] = Affine version of 'a' in Montgomery form */
        XMEMCPY(table[1].x, t->x, sizeof(table->x));
        XMEMCPY(table[1].y, t->y, sizeof(table->y));
        table[1].infinity = 0;

        for (i=1; i<8; i++) {
            sp_256_proj_point_dbl_n_8(t, t, 32, tmp);
            sp_256_proj_to_affine_8(t, tmp);
            XMEMCPY(table[1<<i].x, t->x, sizeof(table->x));
            XMEMCPY(table[1<<i].y, t->y, sizeof(table->y));
            table[1<<i].infinity = 0;
        }

        for (i=1; i<8; i++) {
            XMEMCPY(s1->x, table[1<<i].x, sizeof(table->x));
            XMEMCPY(s1->y, table[1<<i].y, sizeof(table->y));
            for (j=(1<<i)+1; j<(1<<(i+1)); j++) {
                XMEMCPY(s2->x, table[j-(1<<i)].x, sizeof(table->x));
                XMEMCPY(s2->y, table[j-(1<<i)].y, sizeof(table->y));
                sp_256_proj_point_add_qz1_8(t, s1, s2, tmp);
                sp_256_proj_to_affine_8(t, tmp);
                XMEMCPY(table[j].x, t->x, sizeof(table->x));
                XMEMCPY(table[j].y, t->y, sizeof(table->y));
                table[j].infinity = 0;
            }
        }
    }

    sp_ecc_point_free(s2, 0, heap);
    sp_ecc_point_free(s1, 0, heap);
    sp_ecc_point_free( t, 0, heap);

    return err;
}

#endif /* FP_ECC */
/* Multiply the point by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_stripe_8(sp_point* r, sp_point* g,
        sp_table_entry* table, sp_digit* k, int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point rtd;
    sp_point pd;
    sp_digit td[2 * 8 * 5];
#endif
    sp_point* rt;
    sp_point* p = NULL;
    sp_digit* t;
    int i, j;
    int y, x;
    int err;

    (void)g;
    (void)heap;

    err = sp_ecc_point_new(heap, rtd, rt);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 8 * 5, heap,
                           DYNAMIC_TYPE_ECC);
    if (t == NULL)
        err = MEMORY_E;
#else
    t = td;
#endif

    if (err == MP_OKAY) {
        XMEMCPY(p->z, p256_norm_mod, sizeof(p256_norm_mod));
        XMEMCPY(rt->z, p256_norm_mod, sizeof(p256_norm_mod));

        y = 0;
        for (j=0,x=31; j<8; j++,x+=32)
            y |= ((k[x / 32] >> (x % 32)) & 1) << j;
        XMEMCPY(rt->x, table[y].x, sizeof(table[y].x));
        XMEMCPY(rt->y, table[y].y, sizeof(table[y].y));
        rt->infinity = table[y].infinity;
        for (i=30; i>=0; i--) {
            y = 0;
            for (j=0,x=i; j<8; j++,x+=32)
                y |= ((k[x / 32] >> (x % 32)) & 1) << j;

            sp_256_proj_point_dbl_8(rt, rt, t);
            XMEMCPY(p->x, table[y].x, sizeof(table[y].x));
            XMEMCPY(p->y, table[y].y, sizeof(table[y].y));
            p->infinity = table[y].infinity;
            sp_256_proj_point_add_qz1_8(rt, rt, p, t);
        }

        if (map)
            sp_256_map_8(r, rt, t);
        else
            XMEMCPY(r, rt, sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (t != NULL)
        XFREE(t, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p, 0, heap);
    sp_ecc_point_free(rt, 0, heap);

    return err;
}

#ifdef FP_ECC
#ifndef FP_ENTRIES
    #define FP_ENTRIES 16
#endif

typedef struct sp_cache_t {
    sp_digit x[8];
    sp_digit y[8];
    sp_table_entry table[256];
    uint32_t cnt;
    int set;
} sp_cache_t;

static THREAD_LS_T sp_cache_t sp_cache[FP_ENTRIES];
static THREAD_LS_T int sp_cache_last = -1;
static THREAD_LS_T int sp_cache_inited = 0;

#ifndef HAVE_THREAD_LS
    static volatile int initCacheMutex = 0;
    static wolfSSL_Mutex sp_cache_lock;
#endif

static void sp_ecc_get_cache(sp_point* g, sp_cache_t** cache)
{
    int i, j;
    uint32_t least;

    if (sp_cache_inited == 0) {
        for (i=0; i<FP_ENTRIES; i++) {
            sp_cache[i].set = 0;
        }
        sp_cache_inited = 1;
    }

    /* Compare point with those in cache. */
    for (i=0; i<FP_ENTRIES; i++) {
        if (!sp_cache[i].set)
            continue;

        if (sp_256_cmp_equal_8(g->x, sp_cache[i].x) & 
                           sp_256_cmp_equal_8(g->y, sp_cache[i].y)) {
            sp_cache[i].cnt++;
            break;
        }
    }

    /* No match. */
    if (i == FP_ENTRIES) {
        /* Find empty entry. */
        i = (sp_cache_last + 1) % FP_ENTRIES;
        for (; i != sp_cache_last; i=(i+1)%FP_ENTRIES) {
            if (!sp_cache[i].set) {
                break;
            }
        }

        /* Evict least used. */
        if (i == sp_cache_last) {
            least = sp_cache[0].cnt;
            for (j=1; j<FP_ENTRIES; j++) {
                if (sp_cache[j].cnt < least) {
                    i = j;
                    least = sp_cache[i].cnt;
                }
            }
        }

        XMEMCPY(sp_cache[i].x, g->x, sizeof(sp_cache[i].x));
        XMEMCPY(sp_cache[i].y, g->y, sizeof(sp_cache[i].y));
        sp_cache[i].set = 1;
        sp_cache[i].cnt = 1;
    }

    *cache = &sp_cache[i];
    sp_cache_last = i;
}
#endif /* FP_ECC */

/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * g     Point to multiply.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_8(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#ifndef FP_ECC
    return sp_256_ecc_mulmod_fast_8(r, g, k, map, heap);
#else
    sp_digit tmp[2 * 8 * 5];
    sp_cache_t* cache;
    int err = MP_OKAY;

#ifndef HAVE_THREAD_LS
    if (initCacheMutex == 0) {
         wc_InitMutex(&sp_cache_lock);
         initCacheMutex = 1;
    }
    if (wc_LockMutex(&sp_cache_lock) != 0)
       err = BAD_MUTEX_E;
#endif /* HAVE_THREAD_LS */

    if (err == MP_OKAY) {
        sp_ecc_get_cache(g, &cache);
        if (cache->cnt == 2)
            sp_256_gen_stripe_table_8(g, cache->table, tmp, heap);

#ifndef HAVE_THREAD_LS
        wc_UnLockMutex(&sp_cache_lock);
#endif /* HAVE_THREAD_LS */

        if (cache->cnt < 2) {
            err = sp_256_ecc_mulmod_fast_8(r, g, k, map, heap);
        }
        else {
            err = sp_256_ecc_mulmod_stripe_8(r, g, cache->table, k,
                    map, heap);
        }
    }

    return err;
#endif
}

#endif /* WOLFSSL_SP_SMALL */
/* Multiply the point by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * km    Scalar to multiply by.
 * p     Point to multiply.
 * r     Resulting point.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
int sp_ecc_mulmod_256(mp_int* km, ecc_point* gm, ecc_point* r, int map,
        void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point p;
    sp_digit kd[8];
#endif
    sp_point* point;
    sp_digit* k = NULL;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(heap, p, point);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        k = XMALLOC(sizeof(sp_digit) * 8, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 8, km);
        sp_256_point_from_ecc_point_8(point, gm);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_8(point, point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_8(point, point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_8(point, r);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (k != NULL)
        XFREE(k, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(point, 0, heap);

    return err;
}

#ifdef WOLFSSL_SP_SMALL
static sp_table_entry p256_table[16] = {
    /* 0 */
    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      1 },
    /* 1 */
    { { 0x18a9143c,0x79e730d4,0x5fedb601,0x75ba95fc,0x77622510,0x79fb732b,
        0xa53755c6,0x18905f76 },
      { 0xce95560a,0xddf25357,0xba19e45c,0x8b4ab8e4,0xdd21f325,0xd2e88688,
        0x25885d85,0x8571ff18 },
      0 },
    /* 2 */
    { { 0x16a0d2bb,0x4f922fc5,0x1a623499,0x0d5cc16c,0x57c62c8b,0x9241cf3a,
        0xfd1b667f,0x2f5e6961 },
      { 0xf5a01797,0x5c15c70b,0x60956192,0x3d20b44d,0x071fdb52,0x04911b37,
        0x8d6f0f7b,0xf648f916 },
      0 },
    /* 3 */
    { { 0xe137bbbc,0x9e566847,0x8a6a0bec,0xe434469e,0x79d73463,0xb1c42761,
        0x133d0015,0x5abe0285 },
      { 0xc04c7dab,0x92aa837c,0x43260c07,0x573d9f4c,0x78e6cc37,0x0c931562,
        0x6b6f7383,0x94bb725b },
      0 },
    /* 4 */
    { { 0xbfe20925,0x62a8c244,0x8fdce867,0x91c19ac3,0xdd387063,0x5a96a5d5,
        0x21d324f6,0x61d587d4 },
      { 0xa37173ea,0xe87673a2,0x53778b65,0x23848008,0x05bab43e,0x10f8441e,
        0x4621efbe,0xfa11fe12 },
      0 },
    /* 5 */
    { { 0x2cb19ffd,0x1c891f2b,0xb1923c23,0x01ba8d5b,0x8ac5ca8e,0xb6d03d67,
        0x1f13bedc,0x586eb04c },
      { 0x27e8ed09,0x0c35c6e5,0x1819ede2,0x1e81a33c,0x56c652fa,0x278fd6c0,
        0x70864f11,0x19d5ac08 },
      0 },
    /* 6 */
    { { 0xd2b533d5,0x62577734,0xa1bdddc0,0x673b8af6,0xa79ec293,0x577e7c9a,
        0xc3b266b1,0xbb6de651 },
      { 0xb65259b3,0xe7e9303a,0xd03a7480,0xd6a0afd3,0x9b3cfc27,0xc5ac83d1,
        0x5d18b99b,0x60b4619a },
      0 },
    /* 7 */
    { { 0x1ae5aa1c,0xbd6a38e1,0x49e73658,0xb8b7652b,0xee5f87ed,0x0b130014,
        0xaeebffcd,0x9d0f27b2 },
      { 0x7a730a55,0xca924631,0xddbbc83a,0x9c955b2f,0xac019a71,0x07c1dfe0,
        0x356ec48d,0x244a566d },
      0 },
    /* 8 */
    { { 0xf4f8b16a,0x56f8410e,0xc47b266a,0x97241afe,0x6d9c87c1,0x0a406b8e,
        0xcd42ab1b,0x803f3e02 },
      { 0x04dbec69,0x7f0309a8,0x3bbad05f,0xa83b85f7,0xad8e197f,0xc6097273,
        0x5067adc1,0xc097440e },
      0 },
    /* 9 */
    { { 0xc379ab34,0x846a56f2,0x841df8d1,0xa8ee068b,0x176c68ef,0x20314459,
        0x915f1f30,0xf1af32d5 },
      { 0x5d75bd50,0x99c37531,0xf72f67bc,0x837cffba,0x48d7723f,0x0613a418,
        0xe2d41c8b,0x23d0f130 },
      0 },
    /* 10 */
    { { 0xd5be5a2b,0xed93e225,0x5934f3c6,0x6fe79983,0x22626ffc,0x43140926,
        0x7990216a,0x50bbb4d9 },
      { 0xe57ec63e,0x378191c6,0x181dcdb2,0x65422c40,0x0236e0f6,0x41a8099b,
        0x01fe49c3,0x2b100118 },
      0 },
    /* 11 */
    { { 0x9b391593,0xfc68b5c5,0x598270fc,0xc385f5a2,0xd19adcbb,0x7144f3aa,
        0x83fbae0c,0xdd558999 },
      { 0x74b82ff4,0x93b88b8e,0x71e734c9,0xd2e03c40,0x43c0322a,0x9a7a9eaf,
        0x149d6041,0xe6e4c551 },
      0 },
    /* 12 */
    { { 0x80ec21fe,0x5fe14bfe,0xc255be82,0xf6ce116a,0x2f4a5d67,0x98bc5a07,
        0xdb7e63af,0xfad27148 },
      { 0x29ab05b3,0x90c0b6ac,0x4e251ae6,0x37a9a83c,0xc2aade7d,0x0a7dc875,
        0x9f0e1a84,0x77387de3 },
      0 },
    /* 13 */
    { { 0xa56c0dd7,0x1e9ecc49,0x46086c74,0xa5cffcd8,0xf505aece,0x8f7a1408,
        0xbef0c47e,0xb37b85c0 },
      { 0xcc0e6a8f,0x3596b6e4,0x6b388f23,0xfd6d4bbf,0xc39cef4e,0xaba453fa,
        0xf9f628d5,0x9c135ac8 },
      0 },
    /* 14 */
    { { 0x95c8f8be,0x0a1c7294,0x3bf362bf,0x2961c480,0xdf63d4ac,0x9e418403,
        0x91ece900,0xc109f9cb },
      { 0x58945705,0xc2d095d0,0xddeb85c0,0xb9083d96,0x7a40449b,0x84692b8d,
        0x2eee1ee1,0x9bc3344f },
      0 },
    /* 15 */
    { { 0x42913074,0x0d5ae356,0x48a542b1,0x55491b27,0xb310732a,0x469ca665,
        0x5f1a4cc1,0x29591d52 },
      { 0xb84f983f,0xe76f5b6b,0x9f5f84e1,0xbe7eef41,0x80baa189,0x1200d496,
        0x18ef332c,0x6376551f },
      0 },
};

/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_base_8(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    return sp_256_ecc_mulmod_stripe_8(r, &p256_base, p256_table,
                                      k, map, heap);
}

#else
static sp_table_entry p256_table[256] = {
    /* 0 */
    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      1 },
    /* 1 */
    { { 0x18a9143c,0x79e730d4,0x5fedb601,0x75ba95fc,0x77622510,0x79fb732b,
        0xa53755c6,0x18905f76 },
      { 0xce95560a,0xddf25357,0xba19e45c,0x8b4ab8e4,0xdd21f325,0xd2e88688,
        0x25885d85,0x8571ff18 },
      0 },
    /* 2 */
    { { 0x4147519a,0x20288602,0x26b372f0,0xd0981eac,0xa785ebc8,0xa9d4a7ca,
        0xdbdf58e9,0xd953c50d },
      { 0xfd590f8f,0x9d6361cc,0x44e6c917,0x72e9626b,0x22eb64cf,0x7fd96110,
        0x9eb288f3,0x863ebb7e },
      0 },
    /* 3 */
    { { 0x5cdb6485,0x7856b623,0x2f0a2f97,0x808f0ea2,0x4f7e300b,0x3e68d954,
        0xb5ff80a0,0x00076055 },
      { 0x838d2010,0x7634eb9b,0x3243708a,0x54014fbb,0x842a6606,0xe0e47d39,
        0x34373ee0,0x83087761 },
      0 },
    /* 4 */
    { { 0x16a0d2bb,0x4f922fc5,0x1a623499,0x0d5cc16c,0x57c62c8b,0x9241cf3a,
        0xfd1b667f,0x2f5e6961 },
      { 0xf5a01797,0x5c15c70b,0x60956192,0x3d20b44d,0x071fdb52,0x04911b37,
        0x8d6f0f7b,0xf648f916 },
      0 },
    /* 5 */
    { { 0xe137bbbc,0x9e566847,0x8a6a0bec,0xe434469e,0x79d73463,0xb1c42761,
        0x133d0015,0x5abe0285 },
      { 0xc04c7dab,0x92aa837c,0x43260c07,0x573d9f4c,0x78e6cc37,0x0c931562,
        0x6b6f7383,0x94bb725b },
      0 },
    /* 6 */
    { { 0x720f141c,0xbbf9b48f,0x2df5bc74,0x6199b3cd,0x411045c4,0xdc3f6129,
        0x2f7dc4ef,0xcdd6bbcb },
      { 0xeaf436fd,0xcca6700b,0xb99326be,0x6f647f6d,0x014f2522,0x0c0fa792,
        0x4bdae5f6,0xa361bebd },
      0 },
    /* 7 */
    { { 0x597c13c7,0x28aa2558,0x50b7c3e1,0xc38d635f,0xf3c09d1d,0x07039aec,
        0xc4b5292c,0xba12ca09 },
      { 0x59f91dfd,0x9e408fa4,0xceea07fb,0x3af43b66,0x9d780b29,0x1eceb089,
        0x701fef4b,0x53ebb99d },
      0 },
    /* 8 */
    { { 0xb0e63d34,0x4fe7ee31,0xa9e54fab,0xf4600572,0xd5e7b5a4,0xc0493334,
        0x06d54831,0x8589fb92 },
      { 0x6583553a,0xaa70f5cc,0xe25649e5,0x0879094a,0x10044652,0xcc904507,
        0x02541c4f,0xebb0696d },
      0 },
    /* 9 */
    { { 0xac1647c5,0x4616ca15,0xc4cf5799,0xb8127d47,0x764dfbac,0xdc666aa3,
        0xd1b27da3,0xeb2820cb },
      { 0x6a87e008,0x9406f8d8,0x922378f3,0xd87dfa9d,0x80ccecb2,0x56ed2e42,
        0x55a7da1d,0x1f28289b },
      0 },
    /* 10 */
    { { 0x3b89da99,0xabbaa0c0,0xb8284022,0xa6f2d79e,0xb81c05e8,0x27847862,
        0x05e54d63,0x337a4b59 },
      { 0x21f7794a,0x3c67500d,0x7d6d7f61,0x207005b7,0x04cfd6e8,0x0a5a3781,
        0xf4c2fbd6,0x0d65e0d5 },
      0 },
    /* 11 */
    { { 0xb5275d38,0xd9d09bbe,0x0be0a358,0x4268a745,0x973eb265,0xf0762ff4,
        0x52f4a232,0xc23da242 },
      { 0x0b94520c,0x5da1b84f,0xb05bd78e,0x09666763,0x94d29ea1,0x3a4dcb86,
        0xc790cff1,0x19de3b8c },
      0 },
    /* 12 */
    { { 0x26c5fe04,0x183a716c,0x3bba1bdb,0x3b28de0b,0xa4cb712c,0x7432c586,
        0x91fccbfd,0xe34dcbd4 },
      { 0xaaa58403,0xb408d46b,0x82e97a53,0x9a697486,0x36aaa8af,0x9e390127,
        0x7b4e0f7f,0xe7641f44 },
      0 },
    /* 13 */
    { { 0xdf64ba59,0x7d753941,0x0b0242fc,0xd33f10ec,0xa1581859,0x4f06dfc6,
        0x052a57bf,0x4a12df57 },
      { 0x9439dbd0,0xbfa6338f,0xbde53e1f,0xd3c24bd4,0x21f1b314,0xfd5e4ffa,
        0xbb5bea46,0x6af5aa93 },
      0 },
    /* 14 */
    { { 0x10c91999,0xda10b699,0x2a580491,0x0a24b440,0xb8cc2090,0x3e0094b4,
        0x66a44013,0x5fe3475a },
      { 0xf93e7b4b,0xb0f8cabd,0x7c23f91a,0x292b501a,0xcd1e6263,0x42e889ae,
        0xecfea916,0xb544e308 },
      0 },
    /* 15 */
    { { 0x16ddfdce,0x6478c6e9,0xf89179e6,0x2c329166,0x4d4e67e1,0x4e8d6e76,
        0xa6b0c20b,0xe0b6b2bd },
      { 0xbb7efb57,0x0d312df2,0x790c4007,0x1aac0dde,0x679bc944,0xf90336ad,
        0x25a63774,0x71c023de },
      0 },
    /* 16 */
    { { 0xbfe20925,0x62a8c244,0x8fdce867,0x91c19ac3,0xdd387063,0x5a96a5d5,
        0x21d324f6,0x61d587d4 },
      { 0xa37173ea,0xe87673a2,0x53778b65,0x23848008,0x05bab43e,0x10f8441e,
        0x4621efbe,0xfa11fe12 },
      0 },
    /* 17 */
    { { 0x2cb19ffd,0x1c891f2b,0xb1923c23,0x01ba8d5b,0x8ac5ca8e,0xb6d03d67,
        0x1f13bedc,0x586eb04c },
      { 0x27e8ed09,0x0c35c6e5,0x1819ede2,0x1e81a33c,0x56c652fa,0x278fd6c0,
        0x70864f11,0x19d5ac08 },
      0 },
    /* 18 */
    { { 0x309a4e1f,0x1e99f581,0xe9270074,0xab7de71b,0xefd28d20,0x26a5ef0b,
        0x7f9c563f,0xe7c0073f },
      { 0x0ef59f76,0x1f6d663a,0x20fcb050,0x669b3b54,0x7a6602d4,0xc08c1f7a,
        0xc65b3c0a,0xe08504fe },
      0 },
    /* 19 */
    { { 0xa031b3ca,0xf098f68d,0xe6da6d66,0x6d1cab9e,0x94f246e8,0x5bfd81fa,
        0x5b0996b4,0x78f01882 },
      { 0x3a25787f,0xb7eefde4,0x1dccac9b,0x8016f80d,0xb35bfc36,0x0cea4877,
        0x7e94747a,0x43a773b8 },
      0 },
    /* 20 */
    { { 0xd2b533d5,0x62577734,0xa1bdddc0,0x673b8af6,0xa79ec293,0x577e7c9a,
        0xc3b266b1,0xbb6de651 },
      { 0xb65259b3,0xe7e9303a,0xd03a7480,0xd6a0afd3,0x9b3cfc27,0xc5ac83d1,
        0x5d18b99b,0x60b4619a },
      0 },
    /* 21 */
    { { 0x1ae5aa1c,0xbd6a38e1,0x49e73658,0xb8b7652b,0xee5f87ed,0x0b130014,
        0xaeebffcd,0x9d0f27b2 },
      { 0x7a730a55,0xca924631,0xddbbc83a,0x9c955b2f,0xac019a71,0x07c1dfe0,
        0x356ec48d,0x244a566d },
      0 },
    /* 22 */
    { { 0xeacf1f96,0x6db0394a,0x024c271c,0x9f2122a9,0x82cbd3b9,0x2626ac1b,
        0x3581ef69,0x45e58c87 },
      { 0xa38f9dbc,0xd3ff479d,0xe888a040,0xa8aaf146,0x46e0bed7,0x945adfb2,
        0xc1e4b7a4,0xc040e21c },
      0 },
    /* 23 */
    { { 0x6f8117b6,0x847af000,0x73a35433,0x651969ff,0x1d9475eb,0x482b3576,
        0x682c6ec7,0x1cdf5c97 },
      { 0x11f04839,0x7db775b4,0x48de1698,0x7dbeacf4,0xb70b3219,0xb2921dd1,
        0xa92dff3d,0x046755f8 },
      0 },
    /* 24 */
    { { 0xbce8ffcd,0xcc8ac5d2,0x2fe61a82,0x0d53c48b,0x7202d6c7,0xf6f16172,
        0x3b83a5f3,0x046e5e11 },
      { 0xd8007f01,0xe7b8ff64,0x5af43183,0x7fb1ef12,0x35e1a03c,0x045c5ea6,
        0x303d005b,0x6e0106c3 },
      0 },
    /* 25 */
    { { 0x88dd73b1,0x48c73584,0x995ed0d9,0x7670708f,0xc56a2ab7,0x38385ea8,
        0xe901cf1f,0x442594ed },
      { 0x12d4b65b,0xf8faa2c9,0x96c90c37,0x94c2343b,0x5e978d1f,0xd326e4a1,
        0x4c2ee68e,0xa796fa51 },
      0 },
    /* 26 */
    { { 0x823addd7,0x359fb604,0xe56693b3,0x9e2a6183,0x3cbf3c80,0xf885b78e,
        0xc69766e9,0xe4ad2da9 },
      { 0x8e048a61,0x357f7f42,0xc092d9a0,0x082d198c,0xc03ed8ef,0xfc3a1af4,
        0xc37b5143,0xc5e94046 },
      0 },
    /* 27 */
    { { 0x2be75f9e,0x476a538c,0xcb123a78,0x6fd1a9e8,0xb109c04b,0xd85e4df0,
        0xdb464747,0x63283daf },
      { 0xbaf2df15,0xce728cf7,0x0ad9a7f4,0xe592c455,0xe834bcc3,0xfab226ad,
        0x1981a938,0x68bd19ab },
      0 },
    /* 28 */
    { { 0x1887d659,0xc08ead51,0xb359305a,0x3374d5f4,0xcfe74fe3,0x96986981,
        0x3c6fdfd6,0x495292f5 },
      { 0x1acec896,0x4a878c9e,0xec5b4484,0xd964b210,0x664d60a7,0x6696f7e2,
        0x26036837,0x0ec7530d },
      0 },
    /* 29 */
    { { 0xad2687bb,0x2da13a05,0xf32e21fa,0xa1f83b6a,0x1dd4607b,0x390f5ef5,
        0x64863f0b,0x0f6207a6 },
      { 0x0f138233,0xbd67e3bb,0x272aa718,0xdd66b96c,0x26ec88ae,0x8ed00407,
        0x08ed6dcf,0xff0db072 },
      0 },
    /* 30 */
    { { 0x4c95d553,0x749fa101,0x5d680a8a,0xa44052fd,0xff3b566f,0x183b4317,
        0x88740ea3,0x313b513c },
      { 0x08d11549,0xb402e2ac,0xb4dee21c,0x071ee10b,0x47f2320e,0x26b987dd,
        0x86f19f81,0x2d3abcf9 },
      0 },
    /* 31 */
    { { 0x815581a2,0x4c288501,0x632211af,0x9a0a6d56,0x0cab2e99,0x19ba7a0f,
        0xded98cdf,0xc036fa10 },
      { 0xc1fbd009,0x29ae08ba,0x06d15816,0x0b68b190,0x9b9e0d8f,0xc2eb3277,
        0xb6d40194,0xa6b2a2c4 },
      0 },
    /* 32 */
    { { 0x6d3549cf,0xd433e50f,0xfacd665e,0x6f33696f,0xce11fcb4,0x695bfdac,
        0xaf7c9860,0x810ee252 },
      { 0x7159bb2c,0x65450fe1,0x758b357b,0xf7dfbebe,0xd69fea72,0x2b057e74,
        0x92731745,0xd485717a },
      0 },
    /* 33 */
    { { 0xf0cb5a98,0x11741a8a,0x1f3110bf,0xd3da8f93,0xab382adf,0x1994e2cb,
        0x2f9a604e,0x6a6045a7 },
      { 0xa2b2411d,0x170c0d3f,0x510e96e0,0xbe0eb83e,0x8865b3cc,0x3bcc9f73,
        0xf9e15790,0xd3e45cfa },
      0 },
    /* 34 */
    { { 0xe83f7669,0xce1f69bb,0x72877d6b,0x09f8ae82,0x3244278d,0x9548ae54,
        0xe3c2c19c,0x207755de },
      { 0x6fef1945,0x87bd61d9,0xb12d28c3,0x18813cef,0x72df64aa,0x9fbcd1d6,
        0x7154b00d,0x48dc5ee5 },
      0 },
    /* 35 */
    { { 0xf7e5a199,0x123790bf,0x989ccbb7,0xe0efb8cf,0x0a519c79,0xc27a2bfe,
        0xdff6f445,0xf2fb0aed },
      { 0xf0b5025f,0x41c09575,0x40fa9f22,0x550543d7,0x380bfbd0,0x8fa3c8ad,
        0xdb28d525,0xa13e9015 },
      0 },
    /* 36 */
    { { 0xa2b65cbc,0xf9f7a350,0x2a464226,0x0b04b972,0xe23f07a1,0x265ce241,
        0x1497526f,0x2bf0d6b0 },
      { 0x4b216fb7,0xd3d4dd3f,0xfbdda26a,0xf7d7b867,0x6708505c,0xaeb7b83f,
        0x162fe89f,0x42a94a5a },
      0 },
    /* 37 */
    { { 0xeaadf191,0x5846ad0b,0x25a268d7,0x0f8a4890,0x494dc1f6,0xe8603050,
        0xc65ede3d,0x2c2dd969 },
      { 0x93849c17,0x6d02171d,0x1da250dd,0x460488ba,0x3c3a5485,0x4810c706,
        0x42c56dbc,0xf437fa1f },
      0 },
    /* 38 */
    { { 0x4a0f7dab,0x6aa0d714,0x1776e9ac,0x0f049793,0xf5f39786,0x52c0a050,
        0x54707aa8,0xaaf45b33 },
      { 0xc18d364a,0x85e37c33,0x3e497165,0xd40b9b06,0x15ec5444,0xf4171681,
        0xf4f272bc,0xcdf6310d },
      0 },
    /* 39 */
    { { 0x8ea8b7ef,0x7473c623,0x85bc2287,0x08e93518,0x2bda8e34,0x41956772,
        0xda9e2ff2,0xf0d008ba },
      { 0x2414d3b1,0x2912671d,0xb019ea76,0xb3754985,0x453bcbdb,0x5c61b96d,
        0xca887b8b,0x5bd5c2f5 },
      0 },
    /* 40 */
    { { 0xf49a3154,0xef0f469e,0x6e2b2e9a,0x3e85a595,0xaa924a9c,0x45aaec1e,
        0xa09e4719,0xaa12dfc8 },
      { 0x4df69f1d,0x26f27227,0xa2ff5e73,0xe0e4c82c,0xb7a9dd44,0xb9d8ce73,
        0xe48ca901,0x6c036e73 },
      0 },
    /* 41 */
    { { 0x0f6e3138,0x5cfae12a,0x25ad345a,0x6966ef00,0x45672bc5,0x8993c64b,
        0x96afbe24,0x292ff658 },
      { 0x5e213402,0xd5250d44,0x4392c9fe,0xf6580e27,0xda1c72e8,0x097b397f,
        0x311b7276,0x644e0c90 },
      0 },
    /* 42 */
    { { 0xa47153f0,0xe1e421e1,0x920418c9,0xb86c3b79,0x705d7672,0x93bdce87,
        0xcab79a77,0xf25ae793 },
      { 0x6d869d0c,0x1f3194a3,0x4986c264,0x9d55c882,0x096e945e,0x49fb5ea3,
        0x13db0a3e,0x39b8e653 },
      0 },
    /* 43 */
    { { 0xb6fd2e59,0x37754200,0x9255c98f,0x35e2c066,0x0e2a5739,0xd9dab21a,
        0x0f19db06,0x39122f2f },
      { 0x03cad53c,0xcfbce1e0,0xe65c17e3,0x225b2c0f,0x9aa13877,0x72baf1d2,
        0xce80ff8d,0x8de80af8 },
      0 },
    /* 44 */
    { { 0x207bbb76,0xafbea8d9,0x21782758,0x921c7e7c,0x1c0436b1,0xdfa2b74b,
        0x2e368c04,0x87194906 },
      { 0xa3993df5,0xb5f928bb,0xf3b3d26a,0x639d75b5,0x85b55050,0x011aa78a,
        0x5b74fde1,0xfc315e6a },
      0 },
    /* 45 */
    { { 0xe8d6ecfa,0x561fd41a,0x1aec7f86,0x5f8c44f6,0x4924741d,0x98452a7b,
        0xee389088,0xe6d4a7ad },
      { 0x4593c75d,0x60552ed1,0xdd271162,0x70a70da4,0x7ba2c7db,0xd2aede93,
        0x9be2ae57,0x35dfaf9a },
      0 },
    /* 46 */
    { { 0xaa736636,0x6b956fcd,0xae2cab7e,0x09f51d97,0x0f349966,0xfb10bf41,
        0x1c830d2b,0x1da5c7d7 },
      { 0x3cce6825,0x5c41e483,0xf9573c3b,0x15ad118f,0xf23036b8,0xa28552c7,
        0xdbf4b9d6,0x7077c0fd },
      0 },
    /* 47 */
    { { 0x46b9661c,0xbf63ff8d,0x0d2cfd71,0xa1dfd36b,0xa847f8f7,0x0373e140,
        0xe50efe44,0x53a8632e },
      { 0x696d8051,0x0976ff68,0xc74f468a,0xdaec0c95,0x5e4e26bd,0x62994dc3,
        0x34e1fcc1,0x028ca76d },
      0 },
    /* 48 */
    { { 0xfc9877ee,0xd11d47dc,0x801d0002,0xc8b36210,0x54c260b6,0xd002c117,
        0x6962f046,0x04c17cd8 },
      { 0xb0daddf5,0x6d9bd094,0x24ce55c0,0xbea23575,0x72da03b5,0x663356e6,
        0xfed97474,0xf7ba4de9 },
      0 },
    /* 49 */
    { { 0xebe1263f,0xd0dbfa34,0x71ae7ce6,0x55763735,0x82a6f523,0xd2440553,
        0x52131c41,0xe31f9600 },
      { 0xea6b6ec6,0xd1bb9216,0x73c2fc44,0x37a1d12e,0x89d0a294,0xc10e7eac,
        0xce34d47b,0xaa3a6259 },
      0 },
    /* 50 */
    { { 0x36f3dcd3,0xfbcf9df5,0xd2bf7360,0x6ceded50,0xdf504f5b,0x491710fa,
        0x7e79daee,0x2398dd62 },
      { 0x6d09569e,0xcf4705a3,0x5149f769,0xea0619bb,0x35f6034c,0xff9c0377,
        0x1c046210,0x5717f5b2 },
      0 },
    /* 51 */
    { { 0x21dd895e,0x9fe229c9,0x40c28451,0x8e518500,0x1d637ecd,0xfa13d239,
        0x0e3c28de,0x660a2c56 },
      { 0xd67fcbd0,0x9cca88ae,0x0ea9f096,0xc8472478,0x72e92b4d,0x32b2f481,
        0x4f522453,0x624ee54c },
      0 },
    /* 52 */
    { { 0xd897eccc,0x09549ce4,0x3f9880aa,0x4d49d1d9,0x043a7c20,0x723c2423,
        0x92bdfbc0,0x4f392afb },
      { 0x7de44fd9,0x6969f8fa,0x57b32156,0xb66cfbe4,0x368ebc3c,0xdb2fa803,
        0xccdb399c,0x8a3e7977 },
      0 },
    /* 53 */
    { { 0x06c4b125,0xdde1881f,0xf6e3ca8c,0xae34e300,0x5c7a13e9,0xef6999de,
        0x70c24404,0x3888d023 },
      { 0x44f91081,0x76280356,0x5f015504,0x3d9fcf61,0x632cd36e,0x1827edc8,
        0x18102336,0xa5e62e47 },
      0 },
    /* 54 */
    { { 0x2facd6c8,0x1a825ee3,0x54bcbc66,0x699c6354,0x98df9931,0x0ce3edf7,
        0x466a5adc,0x2c4768e6 },
      { 0x90a64bc9,0xb346ff8c,0xe4779f5c,0x630a6020,0xbc05e884,0xd949d064,
        0xf9e652a0,0x7b5e6441 },
      0 },
    /* 55 */
    { { 0x1d28444a,0x2169422c,0xbe136a39,0xe996c5d8,0xfb0c7fce,0x2387afe5,
        0x0c8d744a,0xb8af73cb },
      { 0x338b86fd,0x5fde83aa,0xa58a5cff,0xfee3f158,0x20ac9433,0xc9ee8f6f,
        0x7f3f0895,0xa036395f },
      0 },
    /* 56 */
    { { 0xa10f7770,0x8c73c6bb,0xa12a0e24,0xa6f16d81,0x51bc2b9f,0x100df682,
        0x875fb533,0x4be36b01 },
      { 0x9fb56dbb,0x9226086e,0x07e7a4f8,0x306fef8b,0x66d52f20,0xeeaccc05,
        0x1bdc00c0,0x8cbc9a87 },
      0 },
    /* 57 */
    { { 0xc0dac4ab,0xe131895c,0x712ff112,0xa874a440,0x6a1cee57,0x6332ae7c,
        0x0c0835f8,0x44e7553e },
      { 0x7734002d,0x6d503fff,0x0b34425c,0x9d35cb8b,0x0e8738b5,0x95f70276,
        0x5eb8fc18,0x470a683a },
      0 },
    /* 58 */
    { { 0x90513482,0x81b761dc,0x01e9276a,0x0287202a,0x0ce73083,0xcda441ee,
        0xc63dc6ef,0x16410690 },
      { 0x6d06a2ed,0xf5034a06,0x189b100b,0xdd4d7745,0xab8218c9,0xd914ae72,
        0x7abcbb4f,0xd73479fd },
      0 },
    /* 59 */
    { { 0x5ad4c6e5,0x7edefb16,0x5b06d04d,0x262cf08f,0x8575cb14,0x12ed5bb1,
        0x0771666b,0x816469e3 },
      { 0x561e291e,0xd7ab9d79,0xc1de1661,0xeb9daf22,0x135e0513,0xf49827eb,
        0xf0dd3f9c,0x0a36dd23 },
      0 },
    /* 60 */
    { { 0x41d5533c,0x098d32c7,0x8684628f,0x7c5f5a9e,0xe349bd11,0x39a228ad,
        0xfdbab118,0xe331dfd6 },
      { 0x6bcc6ed8,0x5100ab68,0xef7a260e,0x7160c3bd,0xbce850d7,0x9063d9a7,
        0x492e3389,0xd3b4782a },
      0 },
    /* 61 */
    { { 0xf3821f90,0xa149b6e8,0x66eb7aad,0x92edd9ed,0x1a013116,0x0bb66953,
        0x4c86a5bd,0x7281275a },
      { 0xd3ff47e5,0x503858f7,0x61016441,0x5e1616bc,0x7dfd9bb1,0x62b0f11a,
        0xce145059,0x2c062e7e },
      0 },
    /* 62 */
    { { 0x0159ac2e,0xa76f996f,0xcbdb2713,0x281e7736,0x08e46047,0x2ad6d288,
        0x2c4e7ef1,0x282a35f9 },
      { 0xc0ce5cd2,0x9c354b1e,0x1379c229,0xcf99efc9,0x3e82c11e,0x992caf38,
        0x554d2abd,0xc71cd513 },
      0 },
    /* 63 */
    { { 0x09b578f4,0x4885de9c,0xe3affa7a,0x1884e258,0x59182f1f,0x8f76b1b7,
        0xcf47f3a3,0xc50f6740 },
      { 0x374b68ea,0xa9c4adf3,0x69965fe2,0xa406f323,0x85a53050,0x2f86a222,
        0x212958dc,0xb9ecb3a7 },
      0 },
    /* 64 */
    { { 0xf4f8b16a,0x56f8410e,0xc47b266a,0x97241afe,0x6d9c87c1,0x0a406b8e,
        0xcd42ab1b,0x803f3e02 },
      { 0x04dbec69,0x7f0309a8,0x3bbad05f,0xa83b85f7,0xad8e197f,0xc6097273,
        0x5067adc1,0xc097440e },
      0 },
    /* 65 */
    { { 0xc379ab34,0x846a56f2,0x841df8d1,0xa8ee068b,0x176c68ef,0x20314459,
        0x915f1f30,0xf1af32d5 },
      { 0x5d75bd50,0x99c37531,0xf72f67bc,0x837cffba,0x48d7723f,0x0613a418,
        0xe2d41c8b,0x23d0f130 },
      0 },
    /* 66 */
    { { 0xf41500d9,0x857ab6ed,0xfcbeada8,0x0d890ae5,0x89725951,0x52fe8648,
        0xc0a3fadd,0xb0288dd6 },
      { 0x650bcb08,0x85320f30,0x695d6e16,0x71af6313,0xb989aa76,0x31f520a7,
        0xf408c8d2,0xffd3724f },
      0 },
    /* 67 */
    { { 0xb458e6cb,0x53968e64,0x317a5d28,0x992dad20,0x7aa75f56,0x3814ae0b,
        0xd78c26df,0xf5590f4a },
      { 0xcf0ba55a,0x0fc24bd3,0x0c778bae,0x0fc4724a,0x683b674a,0x1ce9864f,
        0xf6f74a20,0x18d6da54 },
      0 },
    /* 68 */
    { { 0xd5be5a2b,0xed93e225,0x5934f3c6,0x6fe79983,0x22626ffc,0x43140926,
        0x7990216a,0x50bbb4d9 },
      { 0xe57ec63e,0x378191c6,0x181dcdb2,0x65422c40,0x0236e0f6,0x41a8099b,
        0x01fe49c3,0x2b100118 },
      0 },
    /* 69 */
    { { 0x9b391593,0xfc68b5c5,0x598270fc,0xc385f5a2,0xd19adcbb,0x7144f3aa,
        0x83fbae0c,0xdd558999 },
      { 0x74b82ff4,0x93b88b8e,0x71e734c9,0xd2e03c40,0x43c0322a,0x9a7a9eaf,
        0x149d6041,0xe6e4c551 },
      0 },
    /* 70 */
    { { 0x1e9af288,0x55f655bb,0xf7ada931,0x647e1a64,0xcb2820e5,0x43697e4b,
        0x07ed56ff,0x51e00db1 },
      { 0x771c327e,0x43d169b8,0x4a96c2ad,0x29cdb20b,0x3deb4779,0xc07d51f5,
        0x49829177,0xe22f4241 },
      0 },
    /* 71 */
    { { 0x635f1abb,0xcd45e8f4,0x68538874,0x7edc0cb5,0xb5a8034d,0xc9472c1f,
        0x52dc48c9,0xf709373d },
      { 0xa8af30d6,0x401966bb,0xf137b69c,0x95bf5f4a,0x9361c47e,0x3966162a,
        0xe7275b11,0xbd52d288 },
      0 },
    /* 72 */
    { { 0x9c5fa877,0xab155c7a,0x7d3a3d48,0x17dad672,0x73d189d8,0x43f43f9e,
        0xc8aa77a6,0xa0d0f8e4 },
      { 0xcc94f92d,0x0bbeafd8,0x0c4ddb3a,0xd818c8be,0xb82eba14,0x22cc65f8,
        0x946d6a00,0xa56c78c7 },
      0 },
    /* 73 */
    { { 0x0dd09529,0x2962391b,0x3daddfcf,0x803e0ea6,0x5b5bf481,0x2c77351f,
        0x731a367a,0xd8befdf8 },
      { 0xfc0157f4,0xab919d42,0xfec8e650,0xf51caed7,0x02d48b0a,0xcdf9cb40,
        0xce9f6478,0x854a68a5 },
      0 },
    /* 74 */
    { { 0x63506ea5,0xdc35f67b,0xa4fe0d66,0x9286c489,0xfe95cd4d,0x3f101d3b,
        0x98846a95,0x5cacea0b },
      { 0x9ceac44d,0xa90df60c,0x354d1c3a,0x3db29af4,0xad5dbabe,0x08dd3de8,
        0x35e4efa9,0xe4982d12 },
      0 },
    /* 75 */
    { { 0xc34cd55e,0x23104a22,0x2680d132,0x58695bb3,0x1fa1d943,0xfb345afa,
        0x16b20499,0x8046b7f6 },
      { 0x38e7d098,0xb533581e,0xf46f0b70,0xd7f61e8d,0x44cb78c4,0x30dea9ea,
        0x9082af55,0xeb17ca7b },
      0 },
    /* 76 */
    { { 0x76a145b9,0x1751b598,0xc1bc71ec,0xa5cf6b0f,0x392715bb,0xd3e03565,
        0xfab5e131,0x097b00ba },
      { 0x565f69e1,0xaa66c8e9,0xb5be5199,0x77e8f75a,0xda4fd984,0x6033ba11,
        0xafdbcc9e,0xf95c747b },
      0 },
    /* 77 */
    { { 0xbebae45e,0x558f01d3,0xc4bc6955,0xa8ebe9f0,0xdbc64fc6,0xaeb705b1,
        0x566ed837,0x3512601e },
      { 0xfa1161cd,0x9336f1e1,0x4c65ef87,0x328ab8d5,0x724f21e5,0x4757eee2,
        0x6068ab6b,0x0ef97123 },
      0 },
    /* 78 */
    { { 0x54ca4226,0x02598cf7,0xf8642c8e,0x5eede138,0x468e1790,0x48963f74,
        0x3b4fbc95,0xfc16d933 },
      { 0xe7c800ca,0xbe96fb31,0x2678adaa,0x13806331,0x6ff3e8b5,0x3d624497,
        0xb95d7a17,0x14ca4af1 },
      0 },
    /* 79 */
    { { 0xbd2f81d5,0x7a4771ba,0x01f7d196,0x1a5f9d69,0xcad9c907,0xd898bef7,
        0xf59c231d,0x4057b063 },
      { 0x89c05c0a,0xbffd82fe,0x1dc0df85,0xe4911c6f,0xa35a16db,0x3befccae,
        0xf1330b13,0x1c3b5d64 },
      0 },
    /* 80 */
    { { 0x80ec21fe,0x5fe14bfe,0xc255be82,0xf6ce116a,0x2f4a5d67,0x98bc5a07,
        0xdb7e63af,0xfad27148 },
      { 0x29ab05b3,0x90c0b6ac,0x4e251ae6,0x37a9a83c,0xc2aade7d,0x0a7dc875,
        0x9f0e1a84,0x77387de3 },
      0 },
    /* 81 */
    { { 0xa56c0dd7,0x1e9ecc49,0x46086c74,0xa5cffcd8,0xf505aece,0x8f7a1408,
        0xbef0c47e,0xb37b85c0 },
      { 0xcc0e6a8f,0x3596b6e4,0x6b388f23,0xfd6d4bbf,0xc39cef4e,0xaba453fa,
        0xf9f628d5,0x9c135ac8 },
      0 },
    /* 82 */
    { { 0x84e35743,0x32aa3202,0x85a3cdef,0x320d6ab1,0x1df19819,0xb821b176,
        0xc433851f,0x5721361f },
      { 0x71fc9168,0x1f0db36a,0x5e5c403c,0x5f98ba73,0x37bcd8f5,0xf64ca87e,
        0xe6bb11bd,0xdcbac3c9 },
      0 },
    /* 83 */
    { { 0x4518cbe2,0xf01d9968,0x9c9eb04e,0xd242fc18,0xe47feebf,0x727663c7,
        0x2d626862,0xb8c1c89e },
      { 0xc8e1d569,0x51a58bdd,0xb7d88cd0,0x563809c8,0xf11f31eb,0x26c27fd9,
        0x2f9422d4,0x5d23bbda },
      0 },
    /* 84 */
    { { 0x95c8f8be,0x0a1c7294,0x3bf362bf,0x2961c480,0xdf63d4ac,0x9e418403,
        0x91ece900,0xc109f9cb },
      { 0x58945705,0xc2d095d0,0xddeb85c0,0xb9083d96,0x7a40449b,0x84692b8d,
        0x2eee1ee1,0x9bc3344f },
      0 },
    /* 85 */
    { { 0x42913074,0x0d5ae356,0x48a542b1,0x55491b27,0xb310732a,0x469ca665,
        0x5f1a4cc1,0x29591d52 },
      { 0xb84f983f,0xe76f5b6b,0x9f5f84e1,0xbe7eef41,0x80baa189,0x1200d496,
        0x18ef332c,0x6376551f },
      0 },
    /* 86 */
    { { 0x562976cc,0xbda5f14e,0x0ef12c38,0x22bca3e6,0x6cca9852,0xbbfa3064,
        0x08e2987a,0xbdb79dc8 },
      { 0xcb06a772,0xfd2cb5c9,0xfe536dce,0x38f475aa,0x7c2b5db8,0xc2a3e022,
        0xadd3c14a,0x8ee86001 },
      0 },
    /* 87 */
    { { 0xa4ade873,0xcbe96981,0xc4fba48c,0x7ee9aa4d,0x5a054ba5,0x2cee2899,
        0x6f77aa4b,0x92e51d7a },
      { 0x7190a34d,0x948bafa8,0xf6bd1ed1,0xd698f75b,0x0caf1144,0xd00ee6e3,
        0x0a56aaaa,0x5182f86f },
      0 },
    /* 88 */
    { { 0x7a4cc99c,0xfba6212c,0x3e6d9ca1,0xff609b68,0x5ac98c5a,0x5dbb27cb,
        0x4073a6f2,0x91dcab5d },
      { 0x5f575a70,0x01b6cc3d,0x6f8d87fa,0x0cb36139,0x89981736,0x165d4e8c,
        0x97974f2b,0x17a0cedb },
      0 },
    /* 89 */
    { { 0x076c8d3a,0x38861e2a,0x210f924b,0x701aad39,0x13a835d9,0x94d0eae4,
        0x7f4cdf41,0x2e8ce36c },
      { 0x037a862b,0x91273dab,0x60e4c8fa,0x01ba9bb7,0x33baf2dd,0xf9645388,
        0x34f668f3,0xf4ccc6cb },
      0 },
    /* 90 */
    { { 0xf1f79687,0x44ef525c,0x92efa815,0x7c595495,0xa5c78d29,0xe1231741,
        0x9a0df3c9,0xac0db488 },
      { 0xdf01747f,0x86bfc711,0xef17df13,0x592b9358,0x5ccb6bb5,0xe5880e4f,
        0x94c974a2,0x95a64a61 },
      0 },
    /* 91 */
    { { 0xc15a4c93,0x72c1efda,0x82585141,0x40269b73,0x16cb0bad,0x6a8dfb1c,
        0x29210677,0x231e54ba },
      { 0x8ae6d2dc,0xa70df917,0x39112918,0x4d6aa63f,0x5e5b7223,0xf627726b,
        0xd8a731e1,0xab0be032 },
      0 },
    /* 92 */
    { { 0x8d131f2d,0x097ad0e9,0x3b04f101,0x637f09e3,0xd5e9a748,0x1ac86196,
        0x2cf6a679,0xf1bcc880 },
      { 0xe8daacb4,0x25c69140,0x60f65009,0x3c4e4055,0x477937a6,0x591cc8fc,
        0x5aebb271,0x85169469 },
      0 },
    /* 93 */
    { { 0xf1dcf593,0xde35c143,0xb018be3b,0x78202b29,0x9bdd9d3d,0xe9cdadc2,
        0xdaad55d8,0x8f67d9d2 },
      { 0x7481ea5f,0x84111656,0xe34c590c,0xe7d2dde9,0x05053fa8,0xffdd43f4,
        0xc0728b5d,0xf84572b9 },
      0 },
    /* 94 */
    { { 0x97af71c9,0x5e1a7a71,0x7a736565,0xa1449444,0x0e1d5063,0xa1b4ae07,
        0x616b2c19,0xedee2710 },
      { 0x11734121,0xb2f034f5,0x4a25e9f0,0x1cac6e55,0xa40c2ecf,0x8dc148f3,
        0x44ebd7f4,0x9fd27e9b },
      0 },
    /* 95 */
    { { 0xf6e2cb16,0x3cc7658a,0xfe5919b6,0xe3eb7d2c,0x168d5583,0x5a8c5816,
        0x958ff387,0xa40c2fb6 },
      { 0xfedcc158,0x8c9ec560,0x55f23056,0x7ad804c6,0x9a307e12,0xd9396704,
        0x7dc6decf,0x99bc9bb8 },
      0 },
    /* 96 */
    { { 0x927dafc6,0x84a9521d,0x5c09cd19,0x52c1fb69,0xf9366dde,0x9d9581a0,
        0xa16d7e64,0x9abe210b },
      { 0x48915220,0x480af84a,0x4dd816c6,0xfa73176a,0x1681ca5a,0xc7d53987,
        0x87f344b0,0x7881c257 },
      0 },
    /* 97 */
    { { 0xe0bcf3ff,0x93399b51,0x127f74f6,0x0d02cbc5,0xdd01d968,0x8fb465a2,
        0xa30e8940,0x15e6e319 },
      { 0x3e0e05f4,0x646d6e0d,0x43588404,0xfad7bddc,0xc4f850d3,0xbe61c7d1,
        0x191172ce,0x0e55facf },
      0 },
    /* 98 */
    { { 0xf8787564,0x7e9d9806,0x31e85ce6,0x1a331721,0xb819e8d6,0x6b0158ca,
        0x6fe96577,0xd73d0976 },
      { 0x1eb7206e,0x42483425,0xc618bb42,0xa519290f,0x5e30a520,0x5dcbb859,
        0x8f15a50b,0x9250a374 },
      0 },
    /* 99 */
    { { 0xbe577410,0xcaff08f8,0x5077a8c6,0xfd408a03,0xec0a63a4,0xf1f63289,
        0xc1cc8c0b,0x77414082 },
      { 0xeb0991cd,0x05a40fa6,0x49fdc296,0xc1ca0866,0xb324fd40,0x3a68a3c7,
        0x12eb20b9,0x8cb04f4d },
      0 },
    /* 100 */
    { { 0x6906171c,0xb1c2d055,0xb0240c3f,0x9073e9cd,0xd8906841,0xdb8e6b4f,
        0x47123b51,0xe4e429ef },
      { 0x38ec36f4,0x0b8dd53c,0xff4b6a27,0xf9d2dc01,0x879a9a48,0x5d066e07,
        0x3c6e6552,0x37bca2ff },
      0 },
    /* 101 */
    { { 0xdf562470,0x4cd2e3c7,0xc0964ac9,0x44f272a2,0x80c793be,0x7c6d5df9,
        0x3002b22a,0x59913edc },
      { 0x5750592a,0x7a139a83,0xe783de02,0x99e01d80,0xea05d64f,0xcf8c0375,
        0xb013e226,0x43786e4a },
      0 },
    /* 102 */
    { { 0x9e56b5a6,0xff32b0ed,0xd9fc68f9,0x0750d9a6,0x597846a7,0xec15e845,
        0xb7e79e7a,0x8638ca98 },
      { 0x0afc24b2,0x2f5ae096,0x4dace8f2,0x05398eaf,0xaecba78f,0x3b765dd0,
        0x7b3aa6f0,0x1ecdd36a },
      0 },
    /* 103 */
    { { 0x6c5ff2f3,0x5d3acd62,0x2873a978,0xa2d516c0,0xd2110d54,0xad94c9fa,
        0xd459f32d,0xd85d0f85 },
      { 0x10b11da3,0x9f700b8d,0xa78318c4,0xd2c22c30,0x9208decd,0x556988f4,
        0xb4ed3c62,0xa04f19c3 },
      0 },
    /* 104 */
    { { 0xed7f93bd,0x087924c8,0x392f51f6,0xcb64ac5d,0x821b71af,0x7cae330a,
        0x5c0950b0,0x92b2eeea },
      { 0x85b6e235,0x85ac4c94,0x2936c0f0,0xab2ca4a9,0xe0508891,0x80faa6b3,
        0x5834276c,0x1ee78221 },
      0 },
    /* 105 */
    { { 0xe63e79f7,0xa60a2e00,0xf399d906,0xf590e7b2,0x6607c09d,0x9021054a,
        0x57a6e150,0xf3f2ced8 },
      { 0xf10d9b55,0x200510f3,0xd8642648,0x9d2fcfac,0xe8bd0e7c,0xe5631aa7,
        0x3da3e210,0x0f56a454 },
      0 },
    /* 106 */
    { { 0x1043e0df,0x5b21bffa,0x9c007e6d,0x6c74b6cc,0xd4a8517a,0x1a656ec0,
        0x1969e263,0xbd8f1741 },
      { 0xbeb7494a,0x8a9bbb86,0x45f3b838,0x1567d46f,0xa4e5a79a,0xdf7a12a7,
        0x30ccfa09,0x2d1a1c35 },
      0 },
    /* 107 */
    { { 0x506508da,0x192e3813,0xa1d795a7,0x336180c4,0x7a9944b3,0xcddb5949,
        0xb91fba46,0xa107a65e },
      { 0x0f94d639,0xe6d1d1c5,0x8a58b7d7,0x8b4af375,0xbd37ca1c,0x1a7c5584,
        0xf87a9af2,0x183d760a },
      0 },
    /* 108 */
    { { 0x0dde59a4,0x29d69711,0x0e8bef87,0xf1ad8d07,0x4f2ebe78,0x229b4963,
        0xc269d754,0x1d44179d },
      { 0x8390d30e,0xb32dc0cf,0x0de8110c,0x0a3b2753,0x2bc0339a,0x31af1dc5,
        0x9606d262,0x771f9cc2 },
      0 },
    /* 109 */
    { { 0x85040739,0x99993e77,0x8026a939,0x44539db9,0xf5f8fc26,0xcf40f6f2,
        0x0362718e,0x64427a31 },
      { 0x85428aa8,0x4f4f2d87,0xebfb49a8,0x7b7adc3f,0xf23d01ac,0x201b2c6d,
        0x6ae90d6d,0x49d9b749 },
      0 },
    /* 110 */
    { { 0x435d1099,0xcc78d8bc,0x8e8d1a08,0x2adbcd4e,0x2cb68a41,0x02c2e2a0,
        0x3f605445,0x9037d81b },
      { 0x074c7b61,0x7cdbac27,0x57bfd72e,0xfe2031ab,0x596d5352,0x61ccec96,
        0x7cc0639c,0x08c3de6a },
      0 },
    /* 111 */
    { { 0xf6d552ab,0x20fdd020,0x05cd81f1,0x56baff98,0x91351291,0x06fb7c3e,
        0x45796b2f,0xc6909442 },
      { 0x41231bd1,0x17b3ae9c,0x5cc58205,0x1eac6e87,0xf9d6a122,0x208837ab,
        0xcafe3ac0,0x3fa3db02 },
      0 },
    /* 112 */
    { { 0x05058880,0xd75a3e65,0x643943f2,0x7da365ef,0xfab24925,0x4147861c,
        0xfdb808ff,0xc5c4bdb0 },
      { 0xb272b56b,0x73513e34,0x11b9043a,0xc8327e95,0xf8844969,0xfd8ce37d,
        0x46c2b6b5,0x2d56db94 },
      0 },
    /* 113 */
    { { 0xff46ac6b,0x2461782f,0x07a2e425,0xd19f7926,0x09a48de1,0xfafea3c4,
        0xe503ba42,0x0f56bd9d },
      { 0x345cda49,0x137d4ed1,0x816f299d,0x821158fc,0xaeb43402,0xe7c6a54a,
        0x1173b5f1,0x4003bb9d },
      0 },
    /* 114 */
    { { 0xa0803387,0x3b8e8189,0x39cbd404,0xece115f5,0xd2877f21,0x4297208d,
        0xa07f2f9e,0x53765522 },
      { 0xa8a4182d,0xa4980a21,0x3219df79,0xa2bbd07a,0x1a19a2d4,0x674d0a2e,
        0x6c5d4549,0x7a056f58 },
      0 },
    /* 115 */
    { { 0x9d8a2a47,0x646b2558,0xc3df2773,0x5b582948,0xabf0d539,0x51ec000e,
        0x7a1a2675,0x77d482f1 },
      { 0x87853948,0xb8a1bd95,0x6cfbffee,0xa6f817bd,0x80681e47,0xab6ec057,
        0x2b38b0e4,0x4115012b },
      0 },
    /* 116 */
    { { 0x6de28ced,0x3c73f0f4,0x9b13ec47,0x1d5da760,0x6e5c6392,0x61b8ce9e,
        0xfbea0946,0xcdf04572 },
      { 0x6c53c3b0,0x1cb3c58b,0x447b843c,0x97fe3c10,0x2cb9780e,0xfb2b8ae1,
        0x97383109,0xee703dda },
      0 },
    /* 117 */
    { { 0xff57e43a,0x34515140,0xb1b811b8,0xd44660d3,0x8f42b986,0x2b3b5dff,
        0xa162ce21,0x2a0ad89d },
      { 0x6bc277ba,0x64e4a694,0xc141c276,0xc788c954,0xcabf6274,0x141aa64c,
        0xac2b4659,0xd62d0b67 },
      0 },
    /* 118 */
    { { 0x2c054ac4,0x39c5d87b,0xf27df788,0x57005859,0xb18128d6,0xedf7cbf3,
        0x991c2426,0xb39a23f2 },
      { 0xf0b16ae5,0x95284a15,0xa136f51b,0x0c6a05b1,0xf2700783,0x1d63c137,
        0xc0674cc5,0x04ed0092 },
      0 },
    /* 119 */
    { { 0x9ae90393,0x1f4185d1,0x4a3d64e6,0x3047b429,0x9854fc14,0xae0001a6,
        0x0177c387,0xa0a91fc1 },
      { 0xae2c831e,0xff0a3f01,0x2b727e16,0xbb76ae82,0x5a3075b4,0x8f12c8a1,
        0x9ed20c41,0x084cf988 },
      0 },
    /* 120 */
    { { 0xfca6becf,0xd98509de,0x7dffb328,0x2fceae80,0x4778e8b9,0x5d8a15c4,
        0x73abf77e,0xd57955b2 },
      { 0x31b5d4f1,0x210da79e,0x3cfa7a1c,0xaa52f04b,0xdc27c20b,0xd4d12089,
        0x02d141f1,0x8e14ea42 },
      0 },
    /* 121 */
    { { 0xf2897042,0xeed50345,0x43402c4a,0x8d05331f,0xc8bdfb21,0xc8d9c194,
        0x2aa4d158,0x597e1a37 },
      { 0xcf0bd68c,0x0327ec1a,0xab024945,0x6d4be0dc,0xc9fe3e84,0x5b9c8d7a,
        0x199b4dea,0xca3f0236 },
      0 },
    /* 122 */
    { { 0x6170bd20,0x592a10b5,0x6d3f5de7,0x0ea897f1,0x44b2ade2,0xa3363ff1,
        0x309c07e4,0xbde7fd7e },
      { 0xb8f5432c,0x516bb6d2,0xe043444b,0x210dc1cb,0xf8f95b5a,0x3db01e6f,
        0x0a7dd198,0xb623ad0e },
      0 },
    /* 123 */
    { { 0x60c7b65b,0xa75bd675,0x23a4a289,0xab8c5590,0xd7b26795,0xf8220fd0,
        0x58ec137b,0xd6aa2e46 },
      { 0x5138bb85,0x10abc00b,0xd833a95c,0x8c31d121,0x1702a32e,0xb24ff00b,
        0x2dcc513a,0x111662e0 },
      0 },
    /* 124 */
    { { 0xefb42b87,0x78114015,0x1b6c4dff,0xbd9f5d70,0xa7d7c129,0x66ecccd7,
        0x94b750f8,0xdb3ee1cb },
      { 0xf34837cf,0xb26f3db0,0xb9578d4f,0xe7eed18b,0x7c56657d,0x5d2cdf93,
        0x52206a59,0x886a6442 },
      0 },
    /* 125 */
    { { 0x65b569ea,0x3c234cfb,0xf72119c1,0x20011141,0xa15a619e,0x8badc85d,
        0x018a17bc,0xa70cf4eb },
      { 0x8c4a6a65,0x224f97ae,0x0134378f,0x36e5cf27,0x4f7e0960,0xbe3a609e,
        0xd1747b77,0xaa4772ab },
      0 },
    /* 126 */
    { { 0x7aa60cc0,0x67676131,0x0368115f,0xc7916361,0xbbc1bb5a,0xded98bb4,
        0x30faf974,0x611a6ddc },
      { 0xc15ee47a,0x30e78cbc,0x4e0d96a5,0x2e896282,0x3dd9ed88,0x36f35adf,
        0x16429c88,0x5cfffaf8 },
      0 },
    /* 127 */
    { { 0x9b7a99cd,0xc0d54cff,0x843c45a1,0x7bf3b99d,0x62c739e1,0x038a908f,
        0x7dc1994c,0x6e5a6b23 },
      { 0x0ba5db77,0xef8b454e,0xacf60d63,0xb7b8807f,0x76608378,0xe591c0c6,
        0x242dabcc,0x481a238d },
      0 },
    /* 128 */
    { { 0x35d0b34a,0xe3417bc0,0x8327c0a7,0x440b386b,0xac0362d1,0x8fb7262d,
        0xe0cdf943,0x2c41114c },
      { 0xad95a0b1,0x2ba5cef1,0x67d54362,0xc09b37a8,0x01e486c9,0x26d6cdd2,
        0x42ff9297,0x20477abf },
      0 },
    /* 129 */
    { { 0x18d65dbf,0x2f75173c,0x339edad8,0x77bf940e,0xdcf1001c,0x7022d26b,
        0xc77396b6,0xac66409a },
      { 0xc6261cc3,0x8b0bb36f,0x190e7e90,0x213f7bc9,0xa45e6c10,0x6541ceba,
        0xcc122f85,0xce8e6975 },
      0 },
    /* 130 */
    { { 0xbc0a67d2,0x0f121b41,0x444d248a,0x62d4760a,0x659b4737,0x0e044f1d,
        0x250bb4a8,0x08fde365 },
      { 0x848bf287,0xaceec3da,0xd3369d6e,0xc2a62182,0x92449482,0x3582dfdc,
        0x565d6cd7,0x2f7e2fd2 },
      0 },
    /* 131 */
    { { 0xc3770fa7,0xae4b92db,0x379043f9,0x095e8d5c,0x17761171,0x54f34e9d,
        0x907702ae,0xc65be92e },
      { 0xf6fd0a40,0x2758a303,0xbcce784b,0xe7d822e3,0x4f9767bf,0x7ae4f585,
        0xd1193b3a,0x4bff8e47 },
      0 },
    /* 132 */
    { { 0x00ff1480,0xcd41d21f,0x0754db16,0x2ab8fb7d,0xbbe0f3ea,0xac81d2ef,
        0x5772967d,0x3e4e4ae6 },
      { 0x3c5303e6,0x7e18f36d,0x92262397,0x3bd9994b,0x1324c3c0,0x9ed70e26,
        0x58ec6028,0x5388aefd },
      0 },
    /* 133 */
    { { 0x5e5d7713,0xad1317eb,0x75de49da,0x09b985ee,0xc74fb261,0x32f5bc4f,
        0x4f75be0e,0x5cf908d1 },
      { 0x8e657b12,0x76043510,0xb96ed9e6,0xbfd421a5,0x8970ccc2,0x0e29f51f,
        0x60f00ce2,0xa698ba40 },
      0 },
    /* 134 */
    { { 0xef748fec,0x73db1686,0x7e9d2cf9,0xe6e755a2,0xce265eff,0x630b6544,
        0x7aebad8d,0xb142ef8a },
      { 0x17d5770a,0xad31af9f,0x2cb3412f,0x66af3b67,0xdf3359de,0x6bd60d1b,
        0x58515075,0xd1896a96 },
      0 },
    /* 135 */
    { { 0x33c41c08,0xec5957ab,0x5468e2e1,0x87de94ac,0xac472f6c,0x18816b73,
        0x7981da39,0x267b0e0b },
      { 0x8e62b988,0x6e554e5d,0x116d21e7,0xd8ddc755,0x3d2a6f99,0x4610faf0,
        0xa1119393,0xb54e287a },
      0 },
    /* 136 */
    { { 0x178a876b,0x0a0122b5,0x085104b4,0x51ff96ff,0x14f29f76,0x050b31ab,
        0x5f87d4e6,0x84abb28b },
      { 0x8270790a,0xd5ed439f,0x85e3f46b,0x2d6cb59d,0x6c1e2212,0x75f55c1b,
        0x17655640,0xe5436f67 },
      0 },
    /* 137 */
    { { 0x2286e8d5,0x53f9025e,0x864453be,0x353c95b4,0xe408e3a0,0xd832f5bd,
        0x5b9ce99e,0x0404f68b },
      { 0xa781e8e5,0xcad33bde,0x163c2f5b,0x3cdf5018,0x0119caa3,0x57576960,
        0x0ac1c701,0x3a4263df },
      0 },
    /* 138 */
    { { 0x9aeb596d,0xc2965ecc,0x023c92b4,0x01ea03e7,0x2e013961,0x4704b4b6,
        0x905ea367,0x0ca8fd3f },
      { 0x551b2b61,0x92523a42,0x390fcd06,0x1eb7a89c,0x0392a63e,0xe7f1d2be,
        0x4ddb0c33,0x96dca264 },
      0 },
    /* 139 */
    { { 0x387510af,0x203bb43a,0xa9a36a01,0x846feaa8,0x2f950378,0xd23a5770,
        0x3aad59dc,0x4363e212 },
      { 0x40246a47,0xca43a1c7,0xe55dd24d,0xb362b8d2,0x5d8faf96,0xf9b08604,
        0xd8bb98c4,0x840e115c },
      0 },
    /* 140 */
    { { 0x1023e8a7,0xf12205e2,0xd8dc7a0b,0xc808a8cd,0x163a5ddf,0xe292a272,
        0x30ded6d4,0x5e0d6abd },
      { 0x7cfc0f64,0x07a721c2,0x0e55ed88,0x42eec01d,0x1d1f9db2,0x26a7bef9,
        0x2945a25a,0x7dea48f4 },
      0 },
    /* 141 */
    { { 0xe5060a81,0xabdf6f1c,0xf8f95615,0xe79f9c72,0x06ac268b,0xcfd36c54,
        0xebfd16d1,0xabc2a2be },
      { 0xd3e2eac7,0x8ac66f91,0xd2dd0466,0x6f10ba63,0x0282d31b,0x6790e377,
        0x6c7eefc1,0x4ea35394 },
      0 },
    /* 142 */
    { { 0x5266309d,0xed8a2f8d,0x81945a3e,0x0a51c6c0,0x578c5dc1,0xcecaf45a,
        0x1c94ffc3,0x3a76e689 },
      { 0x7d7b0d0f,0x9aace8a4,0x8f584a5f,0x963ace96,0x4e697fbe,0x51a30c72,
        0x465e6464,0x8212a10a },
      0 },
    /* 143 */
    { { 0xcfab8caa,0xef7c61c3,0x0e142390,0x18eb8e84,0x7e9733ca,0xcd1dff67,
        0x599cb164,0xaa7cab71 },
      { 0xbc837bd1,0x02fc9273,0xc36af5d7,0xc06407d0,0xf423da49,0x17621292,
        0xfe0617c3,0x40e38073 },
      0 },
    /* 144 */
    { { 0xa7bf9b7c,0xf4f80824,0x3fbe30d0,0x365d2320,0x97cf9ce3,0xbfbe5320,
        0xb3055526,0xe3604700 },
      { 0x6cc6c2c7,0x4dcb9911,0xba4cbee6,0x72683708,0x637ad9ec,0xdcded434,
        0xa3dee15f,0x6542d677 },
      0 },
    /* 145 */
    { { 0x7b6c377a,0x3f32b6d0,0x903448be,0x6cb03847,0x20da8af7,0xd6fdd3a8,
        0x09bb6f21,0xa6534aee },
      { 0x1035facf,0x30a1780d,0x9dcb47e6,0x35e55a33,0xc447f393,0x6ea50fe1,
        0xdc9aef22,0xf3cb672f },
      0 },
    /* 146 */
    { { 0x3b55fd83,0xeb3719fe,0x875ddd10,0xe0d7a46c,0x05cea784,0x33ac9fa9,
        0xaae870e7,0x7cafaa2e },
      { 0x1d53b338,0x9b814d04,0xef87e6c6,0xe0acc0a0,0x11672b0f,0xfb93d108,
        0xb9bd522e,0x0aab13c1 },
      0 },
    /* 147 */
    { { 0xd2681297,0xddcce278,0xb509546a,0xcb350eb1,0x7661aaf2,0x2dc43173,
        0x847012e9,0x4b91a602 },
      { 0x72f8ddcf,0xdcff1095,0x9a911af4,0x08ebf61e,0xc372430e,0x48f4360a,
        0x72321cab,0x49534c53 },
      0 },
    /* 148 */
    { { 0xf07b7e9d,0x83df7d71,0x13cd516f,0xa478efa3,0x6c047ee3,0x78ef264b,
        0xd65ac5ee,0xcaf46c4f },
      { 0x92aa8266,0xa04d0c77,0x913684bb,0xedf45466,0xae4b16b0,0x56e65168,
        0x04c6770f,0x14ce9e57 },
      0 },
    /* 149 */
    { { 0x965e8f91,0x99445e3e,0xcb0f2492,0xd3aca1ba,0x90c8a0a0,0xd31cc70f,
        0x3e4c9a71,0x1bb708a5 },
      { 0x558bdd7a,0xd5ca9e69,0x018a26b1,0x734a0508,0x4c9cf1ec,0xb093aa71,
        0xda300102,0xf9d126f2 },
      0 },
    /* 150 */
    { { 0xaff9563e,0x749bca7a,0xb49914a0,0xdd077afe,0xbf5f1671,0xe27a0311,
        0x729ecc69,0x807afcb9 },
      { 0xc9b08b77,0x7f8a9337,0x443c7e38,0x86c3a785,0x476fd8ba,0x85fafa59,
        0x6568cd8c,0x751adcd1 },
      0 },
    /* 151 */
    { { 0x10715c0d,0x8aea38b4,0x8f7697f7,0xd113ea71,0x93fbf06d,0x665eab14,
        0x2537743f,0x29ec4468 },
      { 0xb50bebbc,0x3d94719c,0xe4505422,0x399ee5bf,0x8d2dedb1,0x90cd5b3a,
        0x92a4077d,0xff9370e3 },
      0 },
    /* 152 */
    { { 0xc6b75b65,0x59a2d69b,0x266651c5,0x4188f8d5,0x3de9d7d2,0x28a9f33e,
        0xa2a9d01a,0x9776478b },
      { 0x929af2c7,0x8852622d,0x4e690923,0x334f5d6d,0xa89a51e9,0xce6cc7e5,
        0xac2f82fa,0x74a6313f },
      0 },
    /* 153 */
    { { 0xb75f079c,0xb2f4dfdd,0x18e36fbb,0x85b07c95,0xe7cd36dd,0x1b6cfcf0,
        0x0ff4863d,0xab75be15 },
      { 0x173fc9b7,0x81b367c0,0xd2594fd0,0xb90a7420,0xc4091236,0x15fdbf03,
        0x0b4459f6,0x4ebeac2e },
      0 },
    /* 154 */
    { { 0x5c9f2c53,0xeb6c5fe7,0x8eae9411,0xd2522011,0xf95ac5d8,0xc8887633,
        0x2c1baffc,0xdf99887b },
      { 0x850aaecb,0xbb78eed2,0x01d6a272,0x9d49181b,0xb1cdbcac,0x978dd511,
        0x779f4058,0x27b040a7 },
      0 },
    /* 155 */
    { { 0xf73b2eb2,0x90405db7,0x8e1b2118,0xe0df8508,0x5962327e,0x501b7152,
        0xe4cfa3f5,0xb393dd37 },
      { 0x3fd75165,0xa1230e7b,0xbcd33554,0xd66344c2,0x0f7b5022,0x6c36f1be,
        0xd0463419,0x09588c12 },
      0 },
    /* 156 */
    { { 0x02601c3b,0xe086093f,0xcf5c335f,0xfb0252f8,0x894aff28,0x955cf280,
        0xdb9f648b,0x81c879a9 },
      { 0xc6f56c51,0x040e687c,0x3f17618c,0xfed47169,0x9059353b,0x44f88a41,
        0x5fc11bc4,0xfa0d48f5 },
      0 },
    /* 157 */
    { { 0xe1608e4d,0xbc6e1c9d,0x3582822c,0x010dda11,0x157ec2d7,0xf6b7ddc1,
        0xb6a367d6,0x8ea0e156 },
      { 0x2383b3b4,0xa354e02f,0x3f01f53c,0x69966b94,0x2de03ca5,0x4ff6632b,
        0xfa00b5ac,0x3f5ab924 },
      0 },
    /* 158 */
    { { 0x59739efb,0x337bb0d9,0xe7ebec0d,0xc751b0f4,0x411a67d1,0x2da52dd6,
        0x2b74256e,0x8bc76887 },
      { 0x82d3d253,0xa5be3b72,0xf58d779f,0xa9f679a1,0xe16767bb,0xa1cac168,
        0x60fcf34f,0xb386f190 },
      0 },
    /* 159 */
    { { 0x2fedcfc2,0x31f3c135,0x62f8af0d,0x5396bf62,0xe57288c2,0x9a02b4ea,
        0x1b069c4d,0x4cb460f7 },
      { 0x5b8095ea,0xae67b4d3,0x6fc07603,0x92bbf859,0xb614a165,0xe1475f66,
        0x95ef5223,0x52c0d508 },
      0 },
    /* 160 */
    { { 0x15339848,0x231c210e,0x70778c8d,0xe87a28e8,0x6956e170,0x9d1de661,
        0x2bb09c0b,0x4ac3c938 },
      { 0x6998987d,0x19be0551,0xae09f4d6,0x8b2376c4,0x1a3f933d,0x1de0b765,
        0xe39705f4,0x380d94c7 },
      0 },
    /* 161 */
    { { 0x81542e75,0x01a355aa,0xee01b9b7,0x96c724a1,0x624d7087,0x6b3a2977,
        0xde2637af,0x2ce3e171 },
      { 0xf5d5bc1a,0xcfefeb49,0x2777e2b5,0xa655607e,0x9513756c,0x4feaac2f,
        0x0b624e4d,0x2e6cd852 },
      0 },
    /* 162 */
    { { 0x8c31c31d,0x3685954b,0x5bf21a0c,0x68533d00,0x75c79ec9,0x0bd7626e,
        0x42c69d54,0xca177547 },
      { 0xf6d2dbb2,0xcc6edaff,0x174a9d18,0xfd0d8cbd,0xaa4578e8,0x875e8793,
        0x9cab2ce6,0xa976a713 },
      0 },
    /* 163 */
    { { 0x93fb353d,0x0a651f1b,0x57fcfa72,0xd75cab8b,0x31b15281,0xaa88cfa7,
        0x0a1f4999,0x8720a717 },
      { 0x693e1b90,0x8c3e8d37,0x16f6dfc3,0xd345dc0b,0xb52a8742,0x8ea8d00a,
        0xc769893c,0x9719ef29 },
      0 },
    /* 164 */
    { { 0x58e35909,0x820eed8d,0x33ddc116,0x9366d8dc,0x6e205026,0xd7f999d0,
        0xe15704c1,0xa5072976 },
      { 0xc4e70b2e,0x002a37ea,0x6890aa8a,0x84dcf657,0x645b2a5c,0xcd71bf18,
        0xf7b77725,0x99389c9d },
      0 },
    /* 165 */
    { { 0x7ada7a4b,0x238c08f2,0xfd389366,0x3abe9d03,0x766f512c,0x6b672e89,
        0x202c82e4,0xa88806aa },
      { 0xd380184e,0x6602044a,0x126a8b85,0xa8cb78c4,0xad844f17,0x79d670c0,
        0x4738dcfe,0x0043bffb },
      0 },
    /* 166 */
    { { 0x36d5192e,0x8d59b5dc,0x4590b2af,0xacf885d3,0x11601781,0x83566d0a,
        0xba6c4866,0x52f3ef01 },
      { 0x0edcb64d,0x3986732a,0x8068379f,0x0a482c23,0x7040f309,0x16cbe5fa,
        0x9ef27e75,0x3296bd89 },
      0 },
    /* 167 */
    { { 0x454d81d7,0x476aba89,0x51eb9b3c,0x9eade7ef,0x81c57986,0x619a21cd,
        0xaee571e9,0x3b90febf },
      { 0x5496f7cb,0x9393023e,0x7fb51bc4,0x55be41d8,0x99beb5ce,0x03f1dd48,
        0x9f810b18,0x6e88069d },
      0 },
    /* 168 */
    { { 0xb43ea1db,0xce37ab11,0x5259d292,0x0a7ff1a9,0x8f84f186,0x851b0221,
        0xdefaad13,0xa7222bea },
      { 0x2b0a9144,0xa2ac78ec,0xf2fa59c5,0x5a024051,0x6147ce38,0x91d1eca5,
        0xbc2ac690,0xbe94d523 },
      0 },
    /* 169 */
    { { 0x0b226ce7,0x72f4945e,0x967e8b70,0xb8afd747,0x85a6c63e,0xedea46f1,
        0x9be8c766,0x7782defe },
      { 0x3db38626,0x760d2aa4,0x76f67ad1,0x460ae787,0x54499cdb,0x341b86fc,
        0xa2892e4b,0x03838567 },
      0 },
    /* 170 */
    { { 0x79ec1a0f,0x2d8daefd,0xceb39c97,0x3bbcd6fd,0x58f61a95,0xf5575ffc,
        0xadf7b420,0xdbd986c4 },
      { 0x15f39eb7,0x81aa8814,0xb98d976c,0x6ee2fcf5,0xcf2f717d,0x5465475d,
        0x6860bbd0,0x8e24d3c4 },
      0 },
    /* 171 */
    { { 0x9a587390,0x749d8e54,0x0cbec588,0x12bb194f,0xb25983c6,0x46e07da4,
        0x407bafc8,0x541a99c4 },
      { 0x624c8842,0xdb241692,0xd86c05ff,0x6044c12a,0x4f7fcf62,0xc59d14b4,
        0xf57d35d1,0xc0092c49 },
      0 },
    /* 172 */
    { { 0xdf2e61ef,0xd3cc75c3,0x2e1b35ca,0x7e8841c8,0x909f29f4,0xc62d30d1,
        0x7286944d,0x75e40634 },
      { 0xbbc237d0,0xe7d41fc5,0xec4f01c9,0xc9537bf0,0x282bd534,0x91c51a16,
        0xc7848586,0x5b7cb658 },
      0 },
    /* 173 */
    { { 0x8a28ead1,0x964a7084,0xfd3b47f6,0x802dc508,0x767e5b39,0x9ae4bfd1,
        0x8df097a1,0x7ae13eba },
      { 0xeadd384e,0xfd216ef8,0xb6b2ff06,0x0361a2d9,0x4bcdb5f3,0x204b9878,
        0xe2a8e3fd,0x787d8074 },
      0 },
    /* 174 */
    { { 0x757fbb1c,0xc5e25d6b,0xca201deb,0xe47bddb2,0x6d2233ff,0x4a55e9a3,
        0x9ef28484,0x5c222819 },
      { 0x88315250,0x773d4a85,0x827097c1,0x21b21a2b,0xdef5d33f,0xab7c4ea1,
        0xbaf0f2b0,0xe45d37ab },
      0 },
    /* 175 */
    { { 0x28511c8a,0xd2df1e34,0xbdca6cd3,0xebb229c8,0x627c39a7,0x578a71a7,
        0x84dfb9d3,0xed7bc122 },
      { 0x93dea561,0xcf22a6df,0xd48f0ed1,0x5443f18d,0x5bad23e8,0xd8b86140,
        0x45ca6d27,0xaac97cc9 },
      0 },
    /* 176 */
    { { 0xa16bd00a,0xeb54ea74,0xf5c0bcc1,0xd839e9ad,0x1f9bfc06,0x092bb7f1,
        0x1163dc4e,0x318f97b3 },
      { 0xc30d7138,0xecc0c5be,0xabc30220,0x44e8df23,0xb0223606,0x2bb7972f,
        0x9a84ff4d,0xfa41faa1 },
      0 },
    /* 177 */
    { { 0xa6642269,0x4402d974,0x9bb783bd,0xc81814ce,0x7941e60b,0x398d38e4,
        0x1d26e9e2,0x38bb6b2c },
      { 0x6a577f87,0xc64e4a25,0xdc11fe1c,0x8b52d253,0x62280728,0xff336abf,
        0xce7601a5,0x94dd0905 },
      0 },
    /* 178 */
    { { 0xde93f92a,0x156cf7dc,0x89b5f315,0xa01333cb,0xc995e750,0x02404df9,
        0xd25c2ae9,0x92077867 },
      { 0x0bf39d44,0xe2471e01,0x96bb53d7,0x5f2c9020,0x5c9c3d8f,0x4c44b7b3,
        0xd29beb51,0x81e8428b },
      0 },
    /* 179 */
    { { 0xc477199f,0x6dd9c2ba,0x6b5ecdd9,0x8cb8eeee,0xee40fd0e,0x8af7db3f,
        0xdbbfa4b1,0x1b94ab62 },
      { 0xce47f143,0x44f0d8b3,0x63f46163,0x51e623fc,0xcc599383,0xf18f270f,
        0x055590ee,0x06a38e28 },
      0 },
    /* 180 */
    { { 0xb3355b49,0x2e5b0139,0xb4ebf99b,0x20e26560,0xd269f3dc,0xc08ffa6b,
        0x83d9d4f8,0xa7b36c20 },
      { 0x1b3e8830,0x64d15c3a,0xa89f9c0b,0xd5fceae1,0xe2d16930,0xcfeee4a2,
        0xa2822a20,0xbe54c6b4 },
      0 },
    /* 181 */
    { { 0x8d91167c,0xd6cdb3df,0xe7a6625e,0x517c3f79,0x346ac7f4,0x7105648f,
        0xeae022bb,0xbf30a5ab },
      { 0x93828a68,0x8e7785be,0x7f3ef036,0x5161c332,0x592146b2,0xe11b5feb,
        0x2732d13a,0xd1c820de },
      0 },
    /* 182 */
    { { 0x9038b363,0x043e1347,0x6b05e519,0x58c11f54,0x6026cad1,0x4fe57abe,
        0x68a18da3,0xb7d17bed },
      { 0xe29c2559,0x44ca5891,0x5bfffd84,0x4f7a0376,0x74e46948,0x498de4af,
        0x6412cc64,0x3997fd5e },
      0 },
    /* 183 */
    { { 0x8bd61507,0xf2074682,0x34a64d2a,0x29e132d5,0x8a8a15e3,0xffeddfb0,
        0x3c6c13e8,0x0eeb8929 },
      { 0xa7e259f8,0xe9b69a3e,0xd13e7e67,0xce1db7e6,0xad1fa685,0x277318f6,
        0xc922b6ef,0x228916f8 },
      0 },
    /* 184 */
    { { 0x0a12ab5b,0x959ae25b,0x957bc136,0xcc11171f,0xd16e2b0c,0x8058429e,
        0x6e93097e,0xec05ad1d },
      { 0xac3f3708,0x157ba5be,0x30b59d77,0x31baf935,0x118234e5,0x47b55237,
        0x7ff11b37,0x7d314156 },
      0 },
    /* 185 */
    { { 0xf6dfefab,0x7bd9c05c,0xdcb37707,0xbe2f2268,0x3a38bb95,0xe53ead97,
        0x9bc1d7a3,0xe9ce66fc },
      { 0x6f6a02a1,0x75aa1576,0x60e600ed,0x38c087df,0x68cdc1b9,0xf8947f34,
        0x72280651,0xd9650b01 },
      0 },
    /* 186 */
    { { 0x5a057e60,0x504b4c4a,0x8def25e4,0xcbccc3be,0x17c1ccbd,0xa6353208,
        0x804eb7a2,0x14d6699a },
      { 0xdb1f411a,0x2c8a8415,0xf80d769c,0x09fbaf0b,0x1c2f77ad,0xb4deef90,
        0x0d43598a,0x6f4c6841 },
      0 },
    /* 187 */
    { { 0x96c24a96,0x8726df4e,0xfcbd99a3,0x534dbc85,0x8b2ae30a,0x3c466ef2,
        0x61189abb,0x4c4350fd },
      { 0xf855b8da,0x2967f716,0x463c38a1,0x41a42394,0xeae93343,0xc37e1413,
        0x5a3118b5,0xa726d242 },
      0 },
    /* 188 */
    { { 0x948c1086,0xdae6b3ee,0xcbd3a2e1,0xf1de503d,0x03d022f3,0x3f35ed3f,
        0xcc6cf392,0x13639e82 },
      { 0xcdafaa86,0x9ac938fb,0x2654a258,0xf45bc5fb,0x45051329,0x1963b26e,
        0xc1a335a3,0xca9365e1 },
      0 },
    /* 189 */
    { { 0x4c3b2d20,0x3615ac75,0x904e241b,0x742a5417,0xcc9d071d,0xb08521c4,
        0x970b72a5,0x9ce29c34 },
      { 0x6d3e0ad6,0x8cc81f73,0xf2f8434c,0x8060da9e,0x6ce862d9,0x35ed1d1a,
        0xab42af98,0x48c4abd7 },
      0 },
    /* 190 */
    { { 0x40c7485a,0xd221b0cc,0xe5274dbf,0xead455bb,0x9263d2e8,0x493c7698,
        0xf67b33cb,0x78017c32 },
      { 0x930cb5ee,0xb9d35769,0x0c408ed2,0xc0d14e94,0x272f1a4d,0xf8b7bf55,
        0xde5c1c04,0x53cd0454 },
      0 },
    /* 191 */
    { { 0x5d28ccac,0xbcd585fa,0x005b746e,0x5f823e56,0xcd0123aa,0x7c79f0a1,
        0xd3d7fa8f,0xeea465c1 },
      { 0x0551803b,0x7810659f,0x7ce6af70,0x6c0b599f,0x29288e70,0x4195a770,
        0x7ae69193,0x1b6e42a4 },
      0 },
    /* 192 */
    { { 0xf67d04c3,0x2e80937c,0x89eeb811,0x1e312be2,0x92594d60,0x56b5d887,
        0x187fbd3d,0x0224da14 },
      { 0x0c5fe36f,0x87abb863,0x4ef51f5f,0x580f3c60,0xb3b429ec,0x964fb1bf,
        0x42bfff33,0x60838ef0 },
      0 },
    /* 193 */
    { { 0x7e0bbe99,0x432cb2f2,0x04aa39ee,0x7bda44f3,0x9fa93903,0x5f497c7a,
        0x2d331643,0x636eb202 },
      { 0x93ae00aa,0xfcfd0e61,0x31ae6d2f,0x875a00fe,0x9f93901c,0xf43658a2,
        0x39218bac,0x8844eeb6 },
      0 },
    /* 194 */
    { { 0x6b3bae58,0x114171d2,0x17e39f3e,0x7db3df71,0x81a8eada,0xcd37bc7f,
        0x51fb789e,0x27ba83dc },
      { 0xfbf54de5,0xa7df439f,0xb5fe1a71,0x7277030b,0xdb297a48,0x42ee8e35,
        0x87f3a4ab,0xadb62d34 },
      0 },
    /* 195 */
    { { 0xa175df2a,0x9b1168a2,0x618c32e9,0x082aa04f,0x146b0916,0xc9e4f2e7,
        0x75e7c8b2,0xb990fd76 },
      { 0x4df37313,0x0829d96b,0xd0b40789,0x1c205579,0x78087711,0x66c9ae4a,
        0x4d10d18d,0x81707ef9 },
      0 },
    /* 196 */
    { { 0x03d6ff96,0x97d7cab2,0x0d843360,0x5b851bfc,0xd042db4b,0x268823c4,
        0xd5a8aa5c,0x3792daea },
      { 0x941afa0b,0x52818865,0x42d83671,0xf3e9e741,0x5be4e0a7,0x17c82527,
        0x94b001ba,0x5abd635e },
      0 },
    /* 197 */
    { { 0x0ac4927c,0x727fa84e,0xa7c8cf23,0xe3886035,0x4adca0df,0xa4bcd5ea,
        0x846ab610,0x5995bf21 },
      { 0x829dfa33,0xe90f860b,0x958fc18b,0xcaafe2ae,0x78630366,0x9b3baf44,
        0xd483411e,0x44c32ca2 },
      0 },
    /* 198 */
    { { 0xe40ed80c,0xa74a97f1,0x31d2ca82,0x5f938cb1,0x7c2d6ad9,0x53f2124b,
        0x8082a54c,0x1f2162fb },
      { 0x720b173e,0x7e467cc5,0x085f12f9,0x40e8a666,0x4c9d65dc,0x8cebc20e,
        0xc3e907c9,0x8f1d402b },
      0 },
    /* 199 */
    { { 0xfbc4058a,0x4f592f9c,0x292f5670,0xb15e14b6,0xbc1d8c57,0xc55cfe37,
        0x926edbf9,0xb1980f43 },
      { 0x32c76b09,0x98c33e09,0x33b07f78,0x1df5279d,0x863bb461,0x6f08ead4,
        0x37448e45,0x2828ad9b },
      0 },
    /* 200 */
    { { 0xc4cf4ac5,0x696722c4,0xdde64afb,0xf5ac1a3f,0xe0890832,0x0551baa2,
        0x5a14b390,0x4973f127 },
      { 0x322eac5d,0xe59d8335,0x0bd9b568,0x5e07eef5,0xa2588393,0xab36720f,
        0xdb168ac7,0x6dac8ed0 },
      0 },
    /* 201 */
    { { 0xeda835ef,0xf7b545ae,0x1d10ed51,0x4aa113d2,0x13741b09,0x035a65e0,
        0x20b9de4c,0x4b23ef59 },
      { 0x3c4c7341,0xe82bb680,0x3f58bc37,0xd457706d,0xa51e3ee8,0x73527863,
        0xddf49a4e,0x4dd71534 },
      0 },
    /* 202 */
    { { 0x95476cd9,0xbf944672,0xe31a725b,0x648d072f,0xfc4b67e0,0x1441c8b8,
        0x2f4a4dbb,0xfd317000 },
      { 0x8995d0e1,0x1cb43ff4,0x0ef729aa,0x76e695d1,0x41798982,0xe0d5f976,
        0x9569f365,0x14fac58c },
      0 },
    /* 203 */
    { { 0xf312ae18,0xad9a0065,0xfcc93fc9,0x51958dc0,0x8a7d2846,0xd9a14240,
        0x36abda50,0xed7c7651 },
      { 0x25d4abbc,0x46270f1a,0xf1a113ea,0x9b5dd8f3,0x5b51952f,0xc609b075,
        0x4d2e9f53,0xfefcb7f7 },
      0 },
    /* 204 */
    { { 0xba119185,0xbd09497a,0xaac45ba4,0xd54e8c30,0xaa521179,0x492479de,
        0x87e0d80b,0x1801a57e },
      { 0xfcafffb0,0x073d3f8d,0xae255240,0x6cf33c0b,0x5b5fdfbc,0x781d763b,
        0x1ead1064,0x9f8fc11e },
      0 },
    /* 205 */
    { { 0x5e69544c,0x1583a171,0xf04b7813,0x0eaf8567,0x278a4c32,0x1e22a8fd,
        0x3d3a69a9,0xa9d3809d },
      { 0x59a2da3b,0x936c2c2c,0x1895c847,0x38ccbcf6,0x63d50869,0x5e65244e,
        0xe1178ef7,0x3006b9ae },
      0 },
    /* 206 */
    { { 0xc9eead28,0x0bb1f2b0,0x89f4dfbc,0x7eef635d,0xb2ce8939,0x074757fd,
        0x45f8f761,0x0ab85fd7 },
      { 0x3e5b4549,0xecda7c93,0x97922f21,0x4be2bb5c,0xb43b8040,0x261a1274,
        0x11e942c2,0xb122d675 },
      0 },
    /* 207 */
    { { 0x66a5ae7a,0x3be607be,0x76adcbe3,0x01e703fa,0x4eb6e5c5,0xaf904301,
        0x097dbaec,0x9f599dc1 },
      { 0x0ff250ed,0x6d75b718,0x349a20dc,0x8eb91574,0x10b227a3,0x425605a4,
        0x8a294b78,0x7d5528e0 },
      0 },
    /* 208 */
    { { 0x20c26def,0xf0f58f66,0x582b2d1e,0x025585ea,0x01ce3881,0xfbe7d79b,
        0x303f1730,0x28ccea01 },
      { 0x79644ba5,0xd1dabcd1,0x06fff0b8,0x1fc643e8,0x66b3e17b,0xa60a76fc,
        0xa1d013bf,0xc18baf48 },
      0 },
    /* 209 */
    { { 0x5dc4216d,0x34e638c8,0x206142ac,0x00c01067,0x95f5064a,0xd453a171,
        0xb7a9596b,0x9def809d },
      { 0x67ab8d2c,0x41e8642e,0x6237a2b6,0xb4240433,0x64c4218b,0x7d506a6d,
        0x68808ce5,0x0357f8b0 },
      0 },
    /* 210 */
    { { 0x4cd2cc88,0x8e9dbe64,0xf0b8f39d,0xcc61c28d,0xcd30a0c8,0x4a309874,
        0x1b489887,0xe4a01add },
      { 0xf57cd8f9,0x2ed1eeac,0xbd594c48,0x1b767d3e,0x7bd2f787,0xa7295c71,
        0xce10cc30,0x466d7d79 },
      0 },
    /* 211 */
    { { 0x9dada2c7,0x47d31892,0x8f9aa27d,0x4fa0a6c3,0x820a59e1,0x90e4fd28,
        0x451ead1a,0xc672a522 },
      { 0x5d86b655,0x30607cc8,0xf9ad4af1,0xf0235d3b,0x571172a6,0x99a08680,
        0xf2a67513,0x5e3d64fa },
      0 },
    /* 212 */
    { { 0x9b3b4416,0xaa6410c7,0xeab26d99,0xcd8fcf85,0xdb656a74,0x5ebff74a,
        0xeb8e42fc,0x6c8a7a95 },
      { 0xb02a63bd,0x10c60ba7,0x8b8f0047,0x6b2f2303,0x312d90b0,0x8c6c3738,
        0xad82ca91,0x348ae422 },
      0 },
    /* 213 */
    { { 0x5ccda2fb,0x7f474663,0x8e0726d2,0x22accaa1,0x492b1f20,0x85adf782,
        0xd9ef2d2e,0xc1074de0 },
      { 0xae9a65b3,0xfcf3ce44,0x05d7151b,0xfd71e4ac,0xce6a9788,0xd4711f50,
        0xc9e54ffc,0xfbadfbdb },
      0 },
    /* 214 */
    { { 0x20a99363,0x1713f1cd,0x6cf22775,0xb915658f,0x24d359b2,0x968175cd,
        0x83716fcd,0xb7f976b4 },
      { 0x5d6dbf74,0x5758e24d,0x71c3af36,0x8d23bafd,0x0243dfe3,0x48f47760,
        0xcafcc805,0xf4d41b2e },
      0 },
    /* 215 */
    { { 0xfdabd48d,0x51f1cf28,0x32c078a4,0xce81be36,0x117146e9,0x6ace2974,
        0xe0160f10,0x180824ea },
      { 0x66e58358,0x0387698b,0xce6ca358,0x63568752,0x5e41e6c5,0x82380e34,
        0x83cf6d25,0x67e5f639 },
      0 },
    /* 216 */
    { { 0xcf4899ef,0xf89ccb8d,0x9ebb44c0,0x949015f0,0xb2598ec9,0x546f9276,
        0x04c11fc6,0x9fef789a },
      { 0x53d2a071,0x6d367ecf,0xa4519b09,0xb10e1a7f,0x611e2eef,0xca6b3fb0,
        0xa99c4e20,0xbc80c181 },
      0 },
    /* 217 */
    { { 0xe5eb82e6,0x972536f8,0xf56cb920,0x1a484fc7,0x50b5da5e,0xc78e2171,
        0x9f8cdf10,0x49270e62 },
      { 0xea6b50ad,0x1a39b7bb,0xa2388ffc,0x9a0284c1,0x8107197b,0x5403eb17,
        0x61372f7f,0xd2ee52f9 },
      0 },
    /* 218 */
    { { 0x88e0362a,0xd37cd285,0x8fa5d94d,0x442fa8a7,0xa434a526,0xaff836e5,
        0xe5abb733,0xdfb478be },
      { 0x673eede6,0xa91f1ce7,0x2b5b2f04,0xa5390ad4,0x5530da2f,0x5e66f7bf,
        0x08df473a,0xd9a140b4 },
      0 },
    /* 219 */
    { { 0x6e8ea498,0x0e0221b5,0x3563ee09,0x62347829,0x335d2ade,0xe06b8391,
        0x623f4b1a,0x760c058d },
      { 0xc198aa79,0x0b89b58c,0xf07aba7f,0xf74890d2,0xfde2556a,0x4e204110,
        0x8f190409,0x7141982d },
      0 },
    /* 220 */
    { { 0x4d4b0f45,0x6f0a0e33,0x392a94e1,0xd9280b38,0xb3c61d5e,0x3af324c6,
        0x89d54e47,0x3af9d1ce },
      { 0x20930371,0xfd8f7981,0x21c17097,0xeda2664c,0xdc42309b,0x0e9545dc,
        0x73957dd6,0xb1f815c3 },
      0 },
    /* 221 */
    { { 0x89fec44a,0x84faa78e,0x3caa4caf,0xc8c2ae47,0xc1b6a624,0x691c807d,
        0x1543f052,0xa41aed14 },
      { 0x7d5ffe04,0x42435399,0x625b6e20,0x8bacb2df,0x87817775,0x85d660be,
        0x86fb60ef,0xd6e9c1dd },
      0 },
    /* 222 */
    { { 0xc6853264,0x3aa2e97e,0xe2304a0b,0x771533b7,0xb8eae9be,0x1b912bb7,
        0xae9bf8c2,0x9c9c6e10 },
      { 0xe030b74c,0xa2309a59,0x6a631e90,0x4ed7494d,0xa49b79f2,0x89f44b23,
        0x40fa61b6,0x566bd596 },
      0 },
    /* 223 */
    { { 0xc18061f3,0x066c0118,0x7c83fc70,0x190b25d3,0x27273245,0xf05fc8e0,
        0xf525345e,0xcf2c7390 },
      { 0x10eb30cf,0xa09bceb4,0x0d77703a,0xcfd2ebba,0x150ff255,0xe842c43a,
        0x8aa20979,0x02f51755 },
      0 },
    /* 224 */
    { { 0xaddb7d07,0x396ef794,0x24455500,0x0b4fc742,0xc78aa3ce,0xfaff8eac,
        0xe8d4d97d,0x14e9ada5 },
      { 0x2f7079e2,0xdaa480a1,0xe4b0800e,0x45baa3cd,0x7838157d,0x01765e2d,
        0x8e9d9ae8,0xa0ad4fab },
      0 },
    /* 225 */
    { { 0x4a653618,0x0bfb7621,0x31eaaa5f,0x1872813c,0x44949d5e,0x1553e737,
        0x6e56ed1e,0xbcd530b8 },
      { 0x32e9c47b,0x169be853,0xb50059ab,0xdc2776fe,0x192bfbb4,0xcdba9761,
        0x6979341d,0x909283cf },
      0 },
    /* 226 */
    { { 0x76e81a13,0x67b00324,0x62171239,0x9bee1a99,0xd32e19d6,0x08ed361b,
        0xace1549a,0x35eeb7c9 },
      { 0x7e4e5bdc,0x1280ae5a,0xb6ceec6e,0x2dcd2cd3,0x6e266bc1,0x52e4224c,
        0x448ae864,0x9a8b2cf4 },
      0 },
    /* 227 */
    { { 0x09d03b59,0xf6471bf2,0xb65af2ab,0xc90e62a3,0xebd5eec9,0xff7ff168,
        0xd4491379,0x6bdb60f4 },
      { 0x8a55bc30,0xdadafebc,0x10097fe0,0xc79ead16,0x4c1e3bdd,0x42e19741,
        0x94ba08a9,0x01ec3cfd },
      0 },
    /* 228 */
    { { 0xdc9485c2,0xba6277eb,0x22fb10c7,0x48cc9a79,0x70a28d8a,0x4f61d60f,
        0x475464f6,0xd1acb1c0 },
      { 0x26f36612,0xd26902b1,0xe0618d8b,0x59c3a44e,0x308357ee,0x4df8a813,
        0x405626c2,0x7dcd079d },
      0 },
    /* 229 */
    { { 0xf05a4b48,0x5ce7d4d3,0x37230772,0xadcd2952,0x812a915a,0xd18f7971,
        0x377d19b8,0x0bf53589 },
      { 0x6c68ea73,0x35ecd95a,0x823a584d,0xc7f3bbca,0xf473a723,0x9fb674c6,
        0xe16686fc,0xd28be4d9 },
      0 },
    /* 230 */
    { { 0x38fa8e4b,0x5d2b9906,0x893fd8fc,0x559f186e,0x436fb6fc,0x3a6de2aa,
        0x510f88ce,0xd76007aa },
      { 0x523a4988,0x2d10aab6,0x74dd0273,0xb455cf44,0xa3407278,0x7f467082,
        0xb303bb01,0xf2b52f68 },
      0 },
    /* 231 */
    { { 0x9835b4ca,0x0d57eafa,0xbb669cbc,0x2d2232fc,0xc6643198,0x8eeeb680,
        0xcc5aed3a,0xd8dbe98e },
      { 0xc5a02709,0xcba9be3f,0xf5ba1fa8,0x30be68e5,0xf10ea852,0xfebd43cd,
        0xee559705,0xe01593a3 },
      0 },
    /* 232 */
    { { 0xea75a0a6,0xd3e5af50,0x57858033,0x512226ac,0xd0176406,0x6fe6d50f,
        0xaeb8ef06,0xafec07b1 },
      { 0x80bb0a31,0x7fb99567,0x37309aae,0x6f1af3cc,0x01abf389,0x9153a15a,
        0x6e2dbfdd,0xa71b9354 },
      0 },
    /* 233 */
    { { 0x18f593d2,0xbf8e12e0,0xa078122b,0xd1a90428,0x0ba4f2ad,0x150505db,
        0x628523d9,0x53a2005c },
      { 0xe7f2b935,0x07c8b639,0xc182961a,0x2bff975a,0x7518ca2c,0x86bceea7,
        0x3d588e3d,0xbf47d19b },
      0 },
    /* 234 */
    { { 0xdd7665d5,0x672967a7,0x2f2f4de5,0x4e303057,0x80d4903f,0x144005ae,
        0x39c9a1b6,0x001c2c7f },
      { 0x69efc6d6,0x143a8014,0x7bc7a724,0xc810bdaa,0xa78150a4,0x5f65670b,
        0x86ffb99b,0xfdadf8e7 },
      0 },
    /* 235 */
    { { 0xffc00785,0xfd38cb88,0x3b48eb67,0x77fa7591,0xbf368fbc,0x0454d055,
        0x5aa43c94,0x3a838e4d },
      { 0x3e97bb9a,0x56166329,0x441d94d9,0x9eb93363,0x0adb2a83,0x515591a6,
        0x873e1da3,0x3cdb8257 },
      0 },
    /* 236 */
    { { 0x7de77eab,0x137140a9,0x41648109,0xf7e1c50d,0xceb1d0df,0x762dcad2,
        0xf1f57fba,0x5a60cc89 },
      { 0x40d45673,0x80b36382,0x5913c655,0x1b82be19,0xdd64b741,0x057284b8,
        0xdbfd8fc0,0x922ff56f },
      0 },
    /* 237 */
    { { 0xc9a129a1,0x1b265dee,0xcc284e04,0xa5b1ce57,0xcebfbe3c,0x04380c46,
        0xf6c5cd62,0x72919a7d },
      { 0x8fb90f9a,0x298f453a,0x88e4031b,0xd719c00b,0x796f1856,0xe32c0e77,
        0x3624089a,0x5e791780 },
      0 },
    /* 238 */
    { { 0x7f63cdfb,0x5c16ec55,0xf1cae4fd,0x8e6a3571,0x560597ca,0xfce26bea,
        0xe24c2fab,0x4e0a5371 },
      { 0xa5765357,0x276a40d3,0x0d73a2b4,0x3c89af44,0x41d11a32,0xb8f370ae,
        0xd56604ee,0xf5ff7818 },
      0 },
    /* 239 */
    { { 0x1a09df21,0xfbf3e3fe,0xe66e8e47,0x26d5d28e,0x29c89015,0x2096bd0a,
        0x533f5e64,0xe41df0e9 },
      { 0xb3ba9e3f,0x305fda40,0x2604d895,0xf2340ceb,0x7f0367c7,0x0866e192,
        0xac4f155f,0x8edd7d6e },
      0 },
    /* 240 */
    { { 0x0bfc8ff3,0xc9a1dc0e,0xe936f42f,0x14efd82b,0xcca381ef,0x67016f7c,
        0xed8aee96,0x1432c1ca },
      { 0x70b23c26,0xec684829,0x0735b273,0xa64fe873,0xeaef0f5a,0xe389f6e5,
        0x5ac8d2c6,0xcaef480b },
      0 },
    /* 241 */
    { { 0x75315922,0x5245c978,0x3063cca5,0xd8295171,0xb64ef2cb,0xf3ce60d0,
        0x8efae236,0xd0ba177e },
      { 0xb1b3af60,0x53a9ae8f,0x3d2da20e,0x1a796ae5,0xdf9eef28,0x01d63605,
        0x1c54ae16,0xf31c957c },
      0 },
    /* 242 */
    { { 0x49cc4597,0xc0f58d52,0xbae0a028,0xdc5015b0,0x734a814a,0xefc5fc55,
        0x96e17c3a,0x013404cb },
      { 0xc9a824bf,0xb29e2585,0x001eaed7,0xd593185e,0x61ef68ac,0x8d6ee682,
        0x91933e6c,0x6f377c4b },
      0 },
    /* 243 */
    { { 0xa8333fd2,0x9f93bad1,0x5a2a95b8,0xa8930202,0xeaf75ace,0x211e5037,
        0xd2d09506,0x6dba3e4e },
      { 0xd04399cd,0xa48ef98c,0xe6b73ade,0x1811c66e,0xc17ecaf3,0x72f60752,
        0x3becf4a7,0xf13cf342 },
      0 },
    /* 244 */
    { { 0xa919e2eb,0xceeb9ec0,0xf62c0f68,0x83a9a195,0x7aba2299,0xcfba3bb6,
        0x274bbad3,0xc83fa9a9 },
      { 0x62fa1ce0,0x0d7d1b0b,0x3418efbf,0xe58b60f5,0x52706f04,0xbfa8ef9e,
        0x5d702683,0xb49d70f4 },
      0 },
    /* 245 */
    { { 0xfad5513b,0x914c7510,0xb1751e2d,0x05f32eec,0xd9fb9d59,0x6d850418,
        0x0c30f1cf,0x59cfadbb },
      { 0x55cb7fd6,0xe167ac23,0x820426a3,0x249367b8,0x90a78864,0xeaeec58c,
        0x354a4b67,0x5babf362 },
      0 },
    /* 246 */
    { { 0xee424865,0x37c981d1,0xf2e5577f,0x8b002878,0xb9e0c058,0x702970f1,
        0x9026c8f0,0x6188c6a7 },
      { 0xd0f244da,0x06f9a19b,0xfb080873,0x1ecced5c,0x9f213637,0x35470f9b,
        0xdf50b9d9,0x993fe475 },
      0 },
    /* 247 */
    { { 0x9b2c3609,0x68e31cdf,0x2c46d4ea,0x84eb19c0,0x9a775101,0x7ac9ec1a,
        0x4c80616b,0x81f76466 },
      { 0x75fbe978,0x1d7c2a5a,0xf183b356,0x6743fed3,0x501dd2bf,0x838d1f04,
        0x5fe9060d,0x564a812a },
      0 },
    /* 248 */
    { { 0xfa817d1d,0x7a5a64f4,0xbea82e0f,0x55f96844,0xcd57f9aa,0xb5ff5a0f,
        0x00e51d6c,0x226bf3cf },
      { 0x2f2833cf,0xd6d1a9f9,0x4f4f89a8,0x20a0a35a,0x8f3f7f77,0x11536c49,
        0xff257836,0x68779f47 },
      0 },
    /* 249 */
    { { 0x73043d08,0x79b0c1c1,0x1fc020fa,0xa5446774,0x9a6d26d0,0xd3767e28,
        0xeb092e0b,0x97bcb0d1 },
      { 0xf32ed3c3,0x2ab6eaa8,0xb281bc48,0xc8a4f151,0xbfa178f3,0x4d1bf4f3,
        0x0a784655,0xa872ffe8 },
      0 },
    /* 250 */
    { { 0xa32b2086,0xb1ab7935,0x8160f486,0xe1eb710e,0x3b6ae6be,0x9bd0cd91,
        0xb732a36a,0x02812bfc },
      { 0xcf605318,0xa63fd7ca,0xfdfd6d1d,0x646e5d50,0x2102d619,0xa1d68398,
        0xfe5396af,0x07391cc9 },
      0 },
    /* 251 */
    { { 0x8b80d02b,0xc50157f0,0x62877f7f,0x6b8333d1,0x78d542ae,0x7aca1af8,
        0x7e6d2a08,0x355d2adc },
      { 0x287386e1,0xb41f335a,0xf8e43275,0xfd272a94,0xe79989ea,0x286ca2cd,
        0x7c2a3a79,0x3dc2b1e3 },
      0 },
    /* 252 */
    { { 0x04581352,0xd689d21c,0x376782be,0x0a00c825,0x9fed701f,0x203bd590,
        0x3ccd846b,0xc4786910 },
      { 0x24c768ed,0x5dba7708,0x6841f657,0x72feea02,0x6accce0e,0x73313ed5,
        0xd5bb4d32,0xccc42968 },
      0 },
    /* 253 */
    { { 0x3d7620b9,0x94e50de1,0x5992a56a,0xd89a5c8a,0x675487c9,0xdc007640,
        0xaa4871cf,0xe147eb42 },
      { 0xacf3ae46,0x274ab4ee,0x50350fbe,0xfd4936fb,0x48c840ea,0xdf2afe47,
        0x080e96e3,0x239ac047 },
      0 },
    /* 254 */
    { { 0x2bfee8d4,0x481d1f35,0xfa7b0fec,0xce80b5cf,0x2ce9af3c,0x105c4c9e,
        0xf5f7e59d,0xc55fa1a3 },
      { 0x8257c227,0x3186f14e,0x342be00b,0xc5b1653f,0xaa904fb2,0x09afc998,
        0xd4f4b699,0x094cd99c },
      0 },
    /* 255 */
    { { 0xd703beba,0x8a981c84,0x32ceb291,0x8631d150,0xe3bd49ec,0xa445f2c9,
        0x42abad33,0xb90a30b6 },
      { 0xb4a5abf9,0xb465404f,0x75db7603,0x004750c3,0xca35d89f,0x6f9a42cc,
        0x1b7924f7,0x019f8b9a },
      0 },
};

/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_base_8(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    return sp_256_ecc_mulmod_stripe_8(r, &p256_base, p256_table,
                                      k, map, heap);
}

#endif

/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * km    Scalar to multiply by.
 * r     Resulting point.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
int sp_ecc_mulmod_base_256(mp_int* km, ecc_point* r, int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point p;
    sp_digit kd[8];
#endif
    sp_point* point;
    sp_digit* k = NULL;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(heap, p, point);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        k = XMALLOC(sizeof(sp_digit) * 8, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 8, km);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_8(point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_8(point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_8(point, r);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (k != NULL)
        XFREE(k, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(point, 0, heap);

    return err;
}

#if defined(WOLFSSL_VALIDATE_ECC_KEYGEN) || defined(HAVE_ECC_SIGN)
/* Returns 1 if the number of zero.
 * Implementation is constant time.
 *
 * a  Number to check.
 * returns 1 if the number is zero and 0 otherwise.
 */
static int sp_256_iszero_8(const sp_digit* a)
{
    return (a[0] | a[1] | a[2] | a[3] | a[4] | a[5] | a[6] | a[7]) == 0;
}

#endif /* WOLFSSL_VALIDATE_ECC_KEYGEN || HAVE_ECC_SIGN */
/* Add 1 to a. (a = a + 1)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_256_add_one_8(sp_digit* a)
{
    __asm__ __volatile__ (
        "ldr	r1, [%[a], #0]\n\t"
        "ldr	r2, [%[a], #4]\n\t"
        "ldr	r3, [%[a], #8]\n\t"
        "ldr	r4, [%[a], #12]\n\t"
        "adds	r1, r1, #1\n\t"
        "adcs	r2, r2, #0\n\t"
        "adcs	r3, r3, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "str	r1, [%[a], #0]\n\t"
        "str	r2, [%[a], #4]\n\t"
        "str	r3, [%[a], #8]\n\t"
        "str	r4, [%[a], #12]\n\t"
        "ldr	r1, [%[a], #16]\n\t"
        "ldr	r2, [%[a], #20]\n\t"
        "ldr	r3, [%[a], #24]\n\t"
        "ldr	r4, [%[a], #28]\n\t"
        "adcs	r1, r1, #0\n\t"
        "adcs	r2, r2, #0\n\t"
        "adcs	r3, r3, #0\n\t"
        "adcs	r4, r4, #0\n\t"
        "str	r1, [%[a], #16]\n\t"
        "str	r2, [%[a], #20]\n\t"
        "str	r3, [%[a], #24]\n\t"
        "str	r4, [%[a], #28]\n\t"
        :
        : [a] "r" (a)
        : "memory", "r1", "r2", "r3", "r4"
    );
}

/* Read big endian unsigned byte aray into r.
 *
 * r  A single precision integer.
 * a  Byte array.
 * n  Number of bytes in array to read.
 */
static void sp_256_from_bin(sp_digit* r, int max, const byte* a, int n)
{
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = n-1; i >= 0; i--) {
        r[j] |= ((sp_digit)a[i]) << s;
        if (s >= 24) {
            r[j] &= 0xffffffff;
            s = 32 - s;
            if (j + 1 >= max)
                break;
            r[++j] = a[i] >> s;
            s = 8 - s;
        }
        else
            s += 8;
    }

    for (j++; j < max; j++)
        r[j] = 0;
}

/* Generates a scalar that is in the range 1..order-1.
 *
 * rng  Random number generator.
 * k    Scalar value.
 * returns RNG failures, MEMORY_E when memory allocation fails and
 * MP_OKAY on success.
 */
static int sp_256_ecc_gen_k_8(WC_RNG* rng, sp_digit* k)
{
    int err;
    byte buf[32];

    do {
        err = wc_RNG_GenerateBlock(rng, buf, sizeof(buf));
        if (err == 0) {
            sp_256_from_bin(k, 8, buf, sizeof(buf));
            if (sp_256_cmp_8(k, p256_order2) < 0) {
                sp_256_add_one_8(k);
                break;
            }
        }
    }
    while (err == 0);

    return err;
}

/* Makes a random EC key pair.
 *
 * rng   Random number generator.
 * priv  Generated private value.
 * pub   Generated public point.
 * heap  Heap to use for allocation.
 * returns ECC_INF_E when the point does not have the correct order, RNG
 * failures, MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
int sp_ecc_make_key_256(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point p;
    sp_digit kd[8];
#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    sp_point inf;
#endif
#endif
    sp_point* point;
    sp_digit* k = NULL;
#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    sp_point* infinity;
#endif
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    (void)heap;

    err = sp_ecc_point_new(heap, p, point);
#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, inf, infinity);
#endif
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        k = XMALLOC(sizeof(sp_digit) * 8, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY)
        err = sp_256_ecc_gen_k_8(rng, k);
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_8(point, k, 1, NULL);
        else
#endif
            err = sp_256_ecc_mulmod_base_8(point, k, 1, NULL);
    }

#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            err = sp_256_ecc_mulmod_avx2_8(infinity, point, p256_order, 1,
                                                                          NULL);
        }
        else
#endif
            err = sp_256_ecc_mulmod_8(infinity, point, p256_order, 1, NULL);
    }
    if (err == MP_OKAY) {
        if (!sp_256_iszero_8(point->x) || !sp_256_iszero_8(point->y))
            err = ECC_INF_E;
    }
#endif

    if (err == MP_OKAY)
        err = sp_256_to_mp(k, priv);
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_8(point, pub);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (k != NULL)
        XFREE(k, heap, DYNAMIC_TYPE_ECC);
#endif
#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    sp_ecc_point_free(infinity, 1, heap);
#endif
    sp_ecc_point_free(point, 1, heap);

    return err;
}

#ifdef HAVE_ECC_DHE
/* Write r as big endian to byte aray.
 * Fixed length number of bytes written: 32
 *
 * r  A single precision integer.
 * a  Byte array.
 */
static void sp_256_to_bin(sp_digit* r, byte* a)
{
    int i, j, s = 0, b;

    j = 256 / 8 - 1;
    a[j] = 0;
    for (i=0; i<8 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 32) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 32);
        if (j >= 0)
            a[j] = 0;
        if (s != 0)
            j++;
    }
}

/* Multiply the point by the scalar and serialize the X ordinate.
 * The number is 0 padded to maximum size on output.
 *
 * priv    Scalar to multiply the point by.
 * pub     Point to multiply.
 * out     Buffer to hold X ordinate.
 * outLen  On entry, size of the buffer in bytes.
 *         On exit, length of data in buffer in bytes.
 * heap    Heap to use for allocation.
 * returns BUFFER_E if the buffer is to small for output size,
 * MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
int sp_ecc_secret_gen_256(mp_int* priv, ecc_point* pub, byte* out,
                          word32* outLen, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point p;
    sp_digit kd[8];
#endif
    sp_point* point = NULL;
    sp_digit* k = NULL;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    if (*outLen < 32)
        err = BUFFER_E;

    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, p, point);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        k = XMALLOC(sizeof(sp_digit) * 8, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(k, 8, priv);
        sp_256_point_from_ecc_point_8(point, pub);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_8(point, point, k, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_8(point, point, k, 1, heap);
    }
    if (err == MP_OKAY) {
        sp_256_to_bin(point->x, out);
        *outLen = 32;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (k != NULL)
        XFREE(k, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(point, 0, heap);

    return err;
}
#endif /* HAVE_ECC_DHE */

#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_256_mul_8(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[16];

    __asm__ __volatile__ (
        "mov	r5, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #28\n\t"
        "movcc	r3, #0\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r12, [%[b], r4]\n\t"
        "umull	r9, r10, r14, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, #0\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #32\n\t"
        "beq	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #56\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#else
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
static void sp_256_mul_8(sp_digit* r, const sp_digit* a, const sp_digit* b)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "#  A[0] * B[0]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r3, r4, r8, r9\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[tmp]]\n\t"
        "#  A[0] * B[1]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[0]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #4]\n\t"
        "#  A[0] * B[2]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[1]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[0]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #8]\n\t"
        "#  A[0] * B[3]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[2]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[1]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[0]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #12]\n\t"
        "#  A[0] * B[4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[3]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[2]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[1]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[0]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #16]\n\t"
        "#  A[0] * B[5]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[1] * B[4]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[2] * B[3]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[2]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[1]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[0]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[tmp], #20]\n\t"
        "#  A[0] * B[6]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[1] * B[5]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[2] * B[4]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[3] * B[3]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[2]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[1]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[0]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[tmp], #24]\n\t"
        "#  A[0] * B[7]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[1] * B[6]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[2] * B[5]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[3] * B[4]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[4] * B[3]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[2]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[1]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[0]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #0]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[tmp], #28]\n\t"
        "#  A[1] * B[7]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[2] * B[6]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[3] * B[5]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[4] * B[4]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[5] * B[3]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[2]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[1]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #4]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #32]\n\t"
        "#  A[2] * B[7]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[3] * B[6]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[4] * B[5]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[5] * B[4]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[6] * B[3]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[2]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #8]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #36]\n\t"
        "#  A[3] * B[7]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[4] * B[6]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[5] * B[5]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[6] * B[4]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "#  A[7] * B[3]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #40]\n\t"
        "#  A[4] * B[7]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "#  A[5] * B[6]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[6] * B[5]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "#  A[7] * B[4]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #16]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r4, r10\n\t"
        "str	r5, [%[r], #44]\n\t"
        "#  A[5] * B[7]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "#  A[6] * B[6]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "#  A[7] * B[5]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #20]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r5, r10\n\t"
        "str	r3, [%[r], #48]\n\t"
        "#  A[6] * B[7]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "#  A[7] * B[6]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #24]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r3, r10\n\t"
        "str	r4, [%[r], #52]\n\t"
        "#  A[7] * B[7]\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "umull	r6, r7, r8, r9\n\t"
        "adds	r5, r5, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r5, [%[r], #56]\n\t"
        "str	r3, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b), [tmp] "r" (tmp)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef HAVE_INTEL_AVX2
#endif /* HAVE_INTEL_AVX2 */
#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer.
 * b  A single precision integer.
 */
static sp_digit sp_256_sub_in_place_8(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "add	r12, %[a], #32\n\t"
        "\n1:\n\t"
        "subs	%[c], r14, %[c]\n\t"
        "ldr	r3, [%[a]]\n\t"
        "ldr	r4, [%[a], #4]\n\t"
        "ldr	r5, [%[a], #8]\n\t"
        "ldr	r6, [%[a], #12]\n\t"
        "ldr	r7, [%[b]], #4\n\t"
        "ldr	r8, [%[b]], #4\n\t"
        "ldr	r9, [%[b]], #4\n\t"
        "ldr	r10, [%[b]], #4\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "sbcs	r6, r6, r10\n\t"
        "str	r3, [%[a]], #4\n\t"
        "str	r4, [%[a]], #4\n\t"
        "str	r5, [%[a]], #4\n\t"
        "str	r6, [%[a]], #4\n\t"
        "sbc	%[c], r14, r14\n\t"
        "cmp	%[a], r12\n\t"
        "bne	1b\n\t"
        : [c] "+r" (c), [a] "+r" (a), [b] "+r" (b)
        :
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r12", "r14"
    );

    return c;
}

#else
/* Sub b from a into a. (a -= b)
 *
 * a  A single precision integer and result.
 * b  A single precision integer.
 */
static sp_digit sp_256_sub_in_place_8(sp_digit* a, const sp_digit* b)
{
    sp_digit c = 0;

    __asm__ __volatile__ (
        "ldr	r2, [%[a], #0]\n\t"
        "ldr	r3, [%[a], #4]\n\t"
        "ldr	r4, [%[a], #8]\n\t"
        "ldr	r5, [%[a], #12]\n\t"
        "ldr	r6, [%[b], #0]\n\t"
        "ldr	r7, [%[b], #4]\n\t"
        "ldr	r8, [%[b], #8]\n\t"
        "ldr	r9, [%[b], #12]\n\t"
        "subs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #0]\n\t"
        "str	r3, [%[a], #4]\n\t"
        "str	r4, [%[a], #8]\n\t"
        "str	r5, [%[a], #12]\n\t"
        "ldr	r2, [%[a], #16]\n\t"
        "ldr	r3, [%[a], #20]\n\t"
        "ldr	r4, [%[a], #24]\n\t"
        "ldr	r5, [%[a], #28]\n\t"
        "ldr	r6, [%[b], #16]\n\t"
        "ldr	r7, [%[b], #20]\n\t"
        "ldr	r8, [%[b], #24]\n\t"
        "ldr	r9, [%[b], #28]\n\t"
        "sbcs	r2, r2, r6\n\t"
        "sbcs	r3, r3, r7\n\t"
        "sbcs	r4, r4, r8\n\t"
        "sbcs	r5, r5, r9\n\t"
        "str	r2, [%[a], #16]\n\t"
        "str	r3, [%[a], #20]\n\t"
        "str	r4, [%[a], #24]\n\t"
        "str	r5, [%[a], #28]\n\t"
        "sbc	%[c], r9, r9\n\t"
        : [c] "+r" (c)
        : [a] "r" (a), [b] "r" (b)
        : "memory", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9"
    );

    return c;
}

#endif /* WOLFSSL_SP_SMALL */
/* Mul a by digit b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision digit.
 */
static void sp_256_mul_d_8(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r5, r3, %[b], r8\n\t"
        "mov	r4, #0\n\t"
        "str	r5, [%[r]]\n\t"
        "mov	r5, #0\n\t"
        "mov	r9, #4\n\t"
        "1:\n\t"
        "ldr	r8, [%[a], r9]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], r9]\n\t"
        "mov	r3, r4\n\t"
        "mov	r4, r5\n\t"
        "mov	r5, #0\n\t"
        "add	r9, r9, #4\n\t"
        "cmp	r9, #32\n\t"
        "blt	1b\n\t"
        "str	r3, [%[r], #32]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#else
    __asm__ __volatile__ (
        "mov	r10, #0\n\t"
        "# A[0] * B\n\t"
        "ldr	r8, [%[a]]\n\t"
        "umull	r3, r4, %[b], r8\n\t"
        "mov	r5, #0\n\t"
        "str	r3, [%[r]]\n\t"
        "# A[1] * B\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #4]\n\t"
        "# A[2] * B\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #8]\n\t"
        "# A[3] * B\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #12]\n\t"
        "# A[4] * B\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "mov	r3, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adcs	r5, r5, r7\n\t"
        "adc	r3, r10, r10\n\t"
        "str	r4, [%[r], #16]\n\t"
        "# A[5] * B\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "mov	r4, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r5, r5, r6\n\t"
        "adcs	r3, r3, r7\n\t"
        "adc	r4, r10, r10\n\t"
        "str	r5, [%[r], #20]\n\t"
        "# A[6] * B\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "mov	r5, #0\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r3, r3, r6\n\t"
        "adcs	r4, r4, r7\n\t"
        "adc	r5, r10, r10\n\t"
        "str	r3, [%[r], #24]\n\t"
        "# A[7] * B\n\t"
        "ldr	r8, [%[a], #28]\n\t"
        "umull	r6, r7, %[b], r8\n\t"
        "adds	r4, r4, r6\n\t"
        "adc	r5, r5, r7\n\t"
        "str	r4, [%[r], #28]\n\t"
        "str	r5, [%[r], #32]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [b] "r" (b)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10"
    );
#endif
}

/* Divide the double width number (d1|d0) by the dividend. (d1|d0 / div)
 *
 * d1   The high order half of the number to divide.
 * d0   The low order half of the number to divide.
 * div  The dividend.
 * returns the result of the division.
 *
 * Note that this is an approximate div. It may give an answer 1 larger.
 */
static sp_digit div_256_word_8(sp_digit d1, sp_digit d0, sp_digit div)
{
    sp_digit r = 0;

    __asm__ __volatile__ (
        "lsr	r5, %[div], #1\n\t"
        "add	r5, r5, #1\n\t"
        "mov	r6, %[d0]\n\t"
        "mov	r7, %[d1]\n\t"
        "# Do top 32\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "# Next 30 bits\n\t"
        "mov	r4, #29\n\t"
        "1:\n\t"
        "movs	r6, r6, lsl #1\n\t"
        "adc	r7, r7, r7\n\t"
        "subs	r8, r5, r7\n\t"
        "sbc	r8, r8, r8\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "sub	%[r], %[r], r8\n\t"
        "and	r8, r8, r5\n\t"
        "subs	r7, r7, r8\n\t"
        "subs	r4, r4, #1\n\t"
        "bpl	1b\n\t"
        "add	%[r], %[r], %[r]\n\t"
        "add	%[r], %[r], #1\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "umull	r4, r5, %[r], %[div]\n\t"
        "subs	r4, %[d0], r4\n\t"
        "sbc	r5, %[d1], r5\n\t"
        "add	%[r], %[r], r5\n\t"
        "subs	r8, %[div], r4\n\t"
        "sbc	r8, r8, r8\n\t"
        "sub	%[r], %[r], r8\n\t"
        : [r] "+r" (r)
        : [d1] "r" (d1), [d0] "r" (d0), [div] "r" (div)
        : "r4", "r5", "r6", "r7", "r8"
    );
    return r;
}

/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_256_mask_8(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<8; i++)
        r[i] = a[i] & m;
#else
    r[0] = a[0] & m;
    r[1] = a[1] & m;
    r[2] = a[2] & m;
    r[3] = a[3] & m;
    r[4] = a[4] & m;
    r[5] = a[5] & m;
    r[6] = a[6] & m;
    r[7] = a[7] & m;
#endif
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_256_div_8(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    sp_digit t1[16], t2[9];
    sp_digit div, r1;
    int i;

    (void)m;

    div = d[7];
    XMEMCPY(t1, a, sizeof(*t1) * 2 * 8);
    for (i=7; i>=0; i--) {
        r1 = div_256_word_8(t1[8 + i], t1[8 + i - 1], div);

        sp_256_mul_d_8(t2, d, r1);
        t1[8 + i] += sp_256_sub_in_place_8(&t1[i], t2);
        t1[8 + i] -= t2[8];
        sp_256_mask_8(t2, d, t1[8 + i]);
        t1[8 + i] += sp_256_add_8(&t1[i], &t1[i], t2);
        sp_256_mask_8(t2, d, t1[8 + i]);
        t1[8 + i] += sp_256_add_8(&t1[i], &t1[i], t2);
    }

    r1 = sp_256_cmp_8(t1, d) >= 0;
    sp_256_cond_sub_8(r, t1, t2, (sp_digit)0 - r1);

    return MP_OKAY;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MP_OKAY indicating success.
 */
static INLINE int sp_256_mod_8(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_256_div_8(a, m, NULL, r);
}

#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
#ifdef WOLFSSL_SP_SMALL
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_256_sqr_8(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[16];

    __asm__ __volatile__ (
        "mov	r12, #0\n\t"
        "mov	r6, #0\n\t"
        "mov	r7, #0\n\t"
        "mov	r8, #0\n\t"
        "mov	r5, #0\n\t"
        "\n1:\n\t"
        "subs	r3, r5, #28\n\t"
        "movcc	r3, r12\n\t"
        "sub	r4, r5, r3\n\t"
        "\n2:\n\t"
        "cmp	r4, r3\n\t"
        "beq	4f\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "ldr	r9, [%[a], r4]\n\t"
        "umull	r9, r10, r14, r9\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "bal	5f\n\t"
        "\n4:\n\t"
        "ldr	r14, [%[a], r3]\n\t"
        "umull	r9, r10, r14, r14\n\t"
        "adds	r6, r6, r9\n\t"
        "adcs	r7, r7, r10\n\t"
        "adc	r8, r8, r12\n\t"
        "\n5:\n\t"
        "add	r3, r3, #4\n\t"
        "sub	r4, r4, #4\n\t"
        "cmp	r3, #32\n\t"
        "beq	3f\n\t"
        "cmp	r3, r4\n\t"
        "bgt	3f\n\t"
        "cmp	r3, r5\n\t"
        "ble	2b\n\t"
        "\n3:\n\t"
        "str	r6, [%[r], r5]\n\t"
        "mov	r6, r7\n\t"
        "mov	r7, r8\n\t"
        "mov	r8, #0\n\t"
        "add	r5, r5, #4\n\t"
        "cmp	r5, #56\n\t"
        "ble	1b\n\t"
        "str	r6, [%[r], r5]\n\t"
        :
        : [r] "r" (tmp), [a] "r" (a)
        : "memory", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r14", "r9", "r12"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#else
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
static void sp_256_sqr_8(sp_digit* r, const sp_digit* a)
{
    sp_digit tmp[8];

    __asm__ __volatile__ (
        "mov	r14, #0\n\t"
        "#  A[0] * A[0]\n\t"
        "ldr	r10, [%[a], #0]\n\t"
        "umull	r8, r3, r10, r10\n\t"
        "mov	r4, #0\n\t"
        "str	r8, [%[tmp]]\n\t"
        "#  A[0] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #4]\n\t"
        "#  A[0] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[1] * A[1]\n\t"
        "ldr	r10, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[tmp], #8]\n\t"
        "#  A[0] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[1] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[tmp], #12]\n\t"
        "#  A[0] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[1] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[2] * A[2]\n\t"
        "ldr	r10, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[tmp], #16]\n\t"
        "#  A[0] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[tmp], #20]\n\t"
        "#  A[0] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[3]\n\t"
        "ldr	r10, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[tmp], #24]\n\t"
        "#  A[0] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #0]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r2, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[1] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[2] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r3, r3, r5\n\t"
        "adcs	r4, r4, r6\n\t"
        "adc	r2, r2, r7\n\t"
        "str	r3, [%[tmp], #28]\n\t"
        "#  A[1] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #4]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r3, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[2] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[3] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[4]\n\t"
        "ldr	r10, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r4, r4, r5\n\t"
        "adcs	r2, r2, r6\n\t"
        "adc	r3, r3, r7\n\t"
        "str	r4, [%[r], #32]\n\t"
        "#  A[2] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #8]\n\t"
        "umull	r5, r6, r10, r8\n\t"
        "mov	r4, #0\n\t"
        "mov	r7, #0\n\t"
        "#  A[3] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "#  A[4] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r5, r5, r8\n\t"
        "adcs	r6, r6, r9\n\t"
        "adc	r7, r7, r14\n\t"
        "adds	r5, r5, r5\n\t"
        "adcs	r6, r6, r6\n\t"
        "adc	r7, r7, r7\n\t"
        "adds	r2, r2, r5\n\t"
        "adcs	r3, r3, r6\n\t"
        "adc	r4, r4, r7\n\t"
        "str	r2, [%[r], #36]\n\t"
        "#  A[3] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #12]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[4] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "#  A[5] * A[5]\n\t"
        "ldr	r10, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #40]\n\t"
        "#  A[4] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #16]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r14, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "#  A[5] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "adds	r4, r4, r8\n\t"
        "adcs	r2, r2, r9\n\t"
        "adc	r3, r3, r14\n\t"
        "str	r4, [%[r], #44]\n\t"
        "#  A[5] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #20]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r14, r14\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "#  A[6] * A[6]\n\t"
        "ldr	r10, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r2, r2, r8\n\t"
        "adcs	r3, r3, r9\n\t"
        "adc	r4, r4, r14\n\t"
        "str	r2, [%[r], #48]\n\t"
        "#  A[6] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "ldr	r8, [%[a], #24]\n\t"
        "umull	r8, r9, r10, r8\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r14, r14\n\t"
        "adds	r3, r3, r8\n\t"
        "adcs	r4, r4, r9\n\t"
        "adc	r2, r2, r14\n\t"
        "str	r3, [%[r], #52]\n\t"
        "#  A[7] * A[7]\n\t"
        "ldr	r10, [%[a], #28]\n\t"
        "umull	r8, r9, r10, r10\n\t"
        "adds	r4, r4, r8\n\t"
        "adc	r2, r2, r9\n\t"
        "str	r4, [%[r], #56]\n\t"
        "str	r2, [%[r], #60]\n\t"
        :
        : [r] "r" (r), [a] "r" (a), [tmp] "r" (tmp)
        : "memory", "r2", "r3", "r4", "r8", "r9", "r10", "r8", "r5", "r6", "r7", "r14"
    );

    XMEMCPY(r, tmp, sizeof(tmp));
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Order-2 for the P256 curve. */
static const uint32_t p256_order_2[8] = {
    0xfc63254f,0xf3b9cac2,0xa7179e84,0xbce6faad,0xffffffff,0xffffffff,
    0x00000000,0xffffffff
};
#else
/* The low half of the order-2 of the P256 curve. */
static const uint32_t p256_order_low[4] = {
    0xfc63254f,0xf3b9cac2,0xa7179e84,0xbce6faad
};
#endif /* WOLFSSL_SP_SMALL */

/* Multiply two number mod the order of P256 curve. (r = a * b mod order)
 *
 * r  Result of the multiplication.
 * a  First operand of the multiplication.
 * b  Second operand of the multiplication.
 */
static void sp_256_mont_mul_order_8(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_8(r, a, b);
    sp_256_mont_reduce_8(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_8(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_8(r, a);
    sp_256_mont_reduce_8(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_8(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_8(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_8(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_8(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 8);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_8(t, t);
        if (p256_order_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_8(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 8);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 8;
    sp_digit* t3 = td + 4 * 8;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_8(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_8(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_8(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_8(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_8(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_8(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_8(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_8(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_8(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_8(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_8(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_8(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_8(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_8(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_8(t2, t2, 4);
    sp_256_mont_mul_order_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_8(t2, t2, 4);
    sp_256_mont_mul_order_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_8(t2, t2, 4);
    sp_256_mont_mul_order_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_8(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_8(r, t2, t3);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef HAVE_INTEL_AVX2
/* Multiply two number mod the order of P256 curve. (r = a * b mod order)
 *
 * r  Result of the multiplication.
 * a  First operand of the multiplication.
 * b  Second operand of the multiplication.
 */
static void sp_256_mont_mul_order_avx2_8(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_avx2_8(r, a, b);
    sp_256_mont_reduce_avx2_8(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_avx2_8(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_avx2_8(r, a);
    sp_256_mont_reduce_avx2_8(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_avx2_8(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_avx2_8(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_avx2_8(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_avx2_8(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 8);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_8(t, t);
        if (p256_order_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_8(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 8);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 8;
    sp_digit* t3 = td + 4 * 8;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_avx2_8(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_avx2_8(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_avx2_8(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_avx2_8(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_avx2_8(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_avx2_8(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_avx2_8(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_avx2_8(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_avx2_8(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_8(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_avx2_8(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_8(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_avx2_8(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_8(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_avx2_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_avx2_8(t2, t2, 4);
    sp_256_mont_mul_order_avx2_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_avx2_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_avx2_8(t2, t2, 4);
    sp_256_mont_mul_order_avx2_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_avx2_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_avx2_8(t2, t2, 4);
    sp_256_mont_mul_order_avx2_8(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_8(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_8(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_avx2_8(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_avx2_8(r, t2, t3);
#endif /* WOLFSSL_SP_SMALL */
}

#endif /* HAVE_INTEL_AVX2 */
#endif /* HAVE_ECC_SIGN || HAVE_ECC_VERIFY */
#ifdef HAVE_ECC_SIGN
#ifndef SP_ECC_MAX_SIG_GEN
#define SP_ECC_MAX_SIG_GEN  64
#endif

/* Sign the hash using the private key.
 *   e = [hash, 256 bits] from binary
 *   r = (k.G)->x mod order
 *   s = (r * x + e) / k mod order
 * The hash is truncated to the first 256 bits.
 *
 * hash     Hash to sign.
 * hashLen  Length of the hash data.
 * rng      Random number generator.
 * priv     Private part of key - scalar.
 * rm       First part of result as an mp_int.
 * sm       Sirst part of result as an mp_int.
 * heap     Heap to use for allocation.
 * returns RNG failures, MEMORY_E when memory allocation fails and
 * MP_OKAY on success.
 */
int sp_ecc_sign_256(const byte* hash, word32 hashLen, WC_RNG* rng, mp_int* priv,
                    mp_int* rm, mp_int* sm, void* heap)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d;
#else
    sp_digit ed[2*8];
    sp_digit xd[2*8];
    sp_digit kd[2*8];
    sp_digit rd[2*8];
    sp_digit td[3 * 2*8];
    sp_point p;
#endif
    sp_digit* e = NULL;
    sp_digit* x = NULL;
    sp_digit* k = NULL;
    sp_digit* r = NULL;
    sp_digit* tmp = NULL;
    sp_point* point = NULL;
    sp_digit carry;
    sp_digit* s;
    sp_digit* kInv;
    int err = MP_OKAY;
    int32_t c;
    int i;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    (void)heap;

    err = sp_ecc_point_new(heap, p, point);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = XMALLOC(sizeof(sp_digit) * 7 * 2 * 8, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            e = d + 0 * 8;
            x = d + 2 * 8;
            k = d + 4 * 8;
            r = d + 6 * 8;
            tmp = d + 8 * 8;
        }
        else
            err = MEMORY_E;
    }
#else
    e = ed;
    x = xd;
    k = kd;
    r = rd;
    tmp = td;
#endif
    s = e;
    kInv = k;

    if (err == MP_OKAY) {
        if (hashLen > 32)
            hashLen = 32;

        sp_256_from_bin(e, 8, hash, hashLen);
        sp_256_from_mp(x, 8, priv);
    }

    for (i = SP_ECC_MAX_SIG_GEN; err == MP_OKAY && i > 0; i--) {
        /* New random point. */
        err = sp_256_ecc_gen_k_8(rng, k);
        if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                err = sp_256_ecc_mulmod_base_avx2_8(point, k, 1, heap);
            else
#endif
                err = sp_256_ecc_mulmod_base_8(point, k, 1, NULL);
        }

        if (err == MP_OKAY) {
            /* r = point->x mod order */
            XMEMCPY(r, point->x, sizeof(sp_digit) * 8);
            sp_256_norm_8(r);
            c = sp_256_cmp_8(r, p256_order);
            sp_256_cond_sub_8(r, r, p256_order, 0 - (c >= 0));
            sp_256_norm_8(r);

            /* Conv k to Montgomery form (mod order) */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_8(k, k, p256_norm_order);
            else
#endif
                sp_256_mul_8(k, k, p256_norm_order);
            err = sp_256_mod_8(k, k, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_8(k);
            /* kInv = 1/k mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_inv_order_avx2_8(kInv, k, tmp);
            else
#endif
                sp_256_mont_inv_order_8(kInv, k, tmp);
            sp_256_norm_8(kInv);

            /* s = r * x + e */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_8(x, x, r);
            else
#endif
                sp_256_mul_8(x, x, r);
            err = sp_256_mod_8(x, x, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_8(x);
            carry = sp_256_add_8(s, e, x);
            sp_256_cond_sub_8(s, s, p256_order, 0 - carry);
            sp_256_norm_8(s);
            c = sp_256_cmp_8(s, p256_order);
            sp_256_cond_sub_8(s, s, p256_order, 0 - (c >= 0));
            sp_256_norm_8(s);

            /* s = s * k^-1 mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_mul_order_avx2_8(s, s, kInv);
            else
#endif
                sp_256_mont_mul_order_8(s, s, kInv);
            sp_256_norm_8(s);

            /* Check that signature is usable. */
            if (!sp_256_iszero_8(s))
                break;
        }
    }

    if (i == 0)
        err = RNG_FAILURE_E;

    if (err == MP_OKAY)
        err = sp_256_to_mp(r, rm);
    if (err == MP_OKAY)
        err = sp_256_to_mp(s, sm);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL) {
        XMEMSET(d, 0, sizeof(sp_digit) * 8 * 8);
        XFREE(d, heap, DYNAMIC_TYPE_ECC);
    }
#else
    XMEMSET(e, 0, sizeof(sp_digit) * 2 * 8);
    XMEMSET(x, 0, sizeof(sp_digit) * 2 * 8);
    XMEMSET(k, 0, sizeof(sp_digit) * 2 * 8);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 8);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 8);
    XMEMSET(tmp, 0, sizeof(sp_digit) * 3 * 2*8);
#endif
    sp_ecc_point_free(point, 1, heap);

    return err;
}
#endif /* HAVE_ECC_SIGN */

#ifdef HAVE_ECC_VERIFY
/* Verify the signature values with the hash and public key.
 *   e = Truncate(hash, 256)
 *   u1 = e/s mod order
 *   u2 = r/s mod order
 *   r == (u1.G + u2.Q)->x mod order
 * Optimization: Leave point in projective form.
 *   (x, y, 1) == (x' / z'*z', y' / z'*z'*z', z' / z')
 *   (r + n*order).z'.z' mod prime == (u1.G + u2.Q)->x'
 * The hash is truncated to the first 256 bits.
 *
 * hash     Hash to sign.
 * hashLen  Length of the hash data.
 * rng      Random number generator.
 * priv     Private part of key - scalar.
 * rm       First part of result as an mp_int.
 * sm       Sirst part of result as an mp_int.
 * heap     Heap to use for allocation.
 * returns RNG failures, MEMORY_E when memory allocation fails and
 * MP_OKAY on success.
 */
int sp_ecc_verify_256(const byte* hash, word32 hashLen, mp_int* pX,
    mp_int* pY, mp_int* pZ, mp_int* r, mp_int* sm, int* res, void* heap)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d = NULL;
#else
    sp_digit u1d[2*8];
    sp_digit u2d[2*8];
    sp_digit sd[2*8];
    sp_digit tmpd[2*8 * 5];
    sp_point p1d;
    sp_point p2d;
#endif
    sp_digit* u1;
    sp_digit* u2;
    sp_digit* s;
    sp_digit* tmp;
    sp_point* p1;
    sp_point* p2 = NULL;
    sp_digit carry;
    int32_t c;
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(heap, p1d, p1);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, p2d, p2);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = XMALLOC(sizeof(sp_digit) * 16 * 8, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            u1  = d + 0 * 8;
            u2  = d + 2 * 8;
            s   = d + 4 * 8;
            tmp = d + 6 * 8;
        }
        else
            err = MEMORY_E;
    }
#else
    u1 = u1d;
    u2 = u2d;
    s  = sd;
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        if (hashLen > 32)
            hashLen = 32;

        sp_256_from_bin(u1, 8, hash, hashLen);
        sp_256_from_mp(u2, 8, r);
        sp_256_from_mp(s, 8, sm);
        sp_256_from_mp(p2->x, 8, pX);
        sp_256_from_mp(p2->y, 8, pY);
        sp_256_from_mp(p2->z, 8, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_mul_avx2_8(s, s, p256_norm_order);
        else
#endif
            sp_256_mul_8(s, s, p256_norm_order);
        err = sp_256_mod_8(s, s, p256_order);
    }
    if (err == MP_OKAY) {
        sp_256_norm_8(s);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_inv_order_avx2_8(s, s, tmp);
            sp_256_mont_mul_order_avx2_8(u1, u1, s);
            sp_256_mont_mul_order_avx2_8(u2, u2, s);
        }
        else
#endif
        {
            sp_256_mont_inv_order_8(s, s, tmp);
            sp_256_mont_mul_order_8(u1, u1, s);
            sp_256_mont_mul_order_8(u2, u2, s);
        }

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_8(p1, u1, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_8(p1, u1, 0, heap);
    }
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_8(p2, p2, u2, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_8(p2, p2, u2, 0, heap);
    }

    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_8(p1, p1, p2, tmp);
        else
#endif
            sp_256_proj_point_add_8(p1, p1, p2, tmp);

        /* (r + n*order).z'.z' mod prime == (u1.G + u2.Q)->x' */
        /* Reload r and convert to Montgomery form. */
        sp_256_from_mp(u2, 8, r);
        err = sp_256_mod_mul_norm_8(u2, u2, p256_mod);
    }

    if (err == MP_OKAY) {
        /* u1 = r.z'.z' mod prime */
        sp_256_mont_sqr_8(p1->z, p1->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_8(u1, u2, p1->z, p256_mod, p256_mp_mod);
        *res = sp_256_cmp_8(p1->x, u1) == 0;
        if (*res == 0) {
            /* Reload r and add order. */
            sp_256_from_mp(u2, 8, r);
            carry = sp_256_add_8(u2, u2, p256_order);
            /* Carry means result is greater than mod and is not valid. */
            if (!carry) {
                sp_256_norm_8(u2);

                /* Compare with mod and if greater or equal then not valid. */
                c = sp_256_cmp_8(u2, p256_mod);
                if (c < 0) {
                    /* Convert to Montogomery form */
                    err = sp_256_mod_mul_norm_8(u2, u2, p256_mod);
                    if (err == MP_OKAY) {
                        /* u1 = (r + 1*order).z'.z' mod prime */
                        sp_256_mont_mul_8(u1, u2, p1->z, p256_mod,
                                                                  p256_mp_mod);
                        *res = sp_256_cmp_8(p1->x, u2) == 0;
                    }
                }
            }
        }
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p1, 0, heap);
    sp_ecc_point_free(p2, 0, heap);

    return err;
}
#endif /* HAVE_ECC_VERIFY */

#ifdef HAVE_ECC_CHECK_KEY
/* Check that the x and y oridinates are a valid point on the curve.
 *
 * point  EC point.
 * heap   Heap to use if dynamically allocating.
 * returns MEMORY_E if dynamic memory allocation fails, MP_VAL if the point is
 * not on the curve and MP_OKAY otherwise.
 */
static int sp_256_ecc_is_point_8(sp_point* point, void* heap)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d = NULL;
#else
    sp_digit t1d[2*8];
    sp_digit t2d[2*8];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 8 * 4, heap, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 8;
        t2 = d + 2 * 8;
    }
    else
        err = MEMORY_E;
#else
    (void)heap;

    t1 = t1d;
    t2 = t2d;
#endif

    if (err == MP_OKAY) {
        sp_256_sqr_8(t1, point->y);
        sp_256_mod_8(t1, t1, p256_mod);
        sp_256_sqr_8(t2, point->x);
        sp_256_mod_8(t2, t2, p256_mod);
        sp_256_mul_8(t2, t2, point->x);
        sp_256_mod_8(t2, t2, p256_mod);
	sp_256_sub_8(t2, p256_mod, t2);
        sp_256_mont_add_8(t1, t1, t2, p256_mod);

        sp_256_mont_add_8(t1, t1, point->x, p256_mod);
        sp_256_mont_add_8(t1, t1, point->x, p256_mod);
        sp_256_mont_add_8(t1, t1, point->x, p256_mod);

        if (sp_256_cmp_8(t1, p256_b) != 0)
            err = MP_VAL;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, heap, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

/* Check that the x and y oridinates are a valid point on the curve.
 *
 * pX  X ordinate of EC point.
 * pY  Y ordinate of EC point.
 * returns MEMORY_E if dynamic memory allocation fails, MP_VAL if the point is
 * not on the curve and MP_OKAY otherwise.
 */
int sp_ecc_is_point_256(mp_int* pX, mp_int* pY)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point pubd;
#endif
    sp_point* pub;
    byte one[1] = { 1 };
    int err;

    err = sp_ecc_point_new(NULL, pubd, pub);
    if (err == MP_OKAY) {
        sp_256_from_mp(pub->x, 8, pX);
        sp_256_from_mp(pub->y, 8, pY);
        sp_256_from_bin(pub->z, 8, one, sizeof(one));

        err = sp_256_ecc_is_point_8(pub, NULL);
    }

    sp_ecc_point_free(pub, 0, NULL);

    return err;
}

/* Check that the private scalar generates the EC point (px, py), the point is
 * on the curve and the point has the correct order.
 *
 * pX     X ordinate of EC point.
 * pY     Y ordinate of EC point.
 * privm  Private scalar that generates EC point.
 * returns MEMORY_E if dynamic memory allocation fails, MP_VAL if the point is
 * not on the curve, ECC_INF_E if the point does not have the correct order,
 * ECC_PRIV_KEY_E when the private scalar doesn't generate the EC point and
 * MP_OKAY otherwise.
 */
int sp_ecc_check_key_256(mp_int* pX, mp_int* pY, mp_int* privm, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit privd[8];
    sp_point pubd;
    sp_point pd;
#endif
    sp_digit* priv = NULL;
    sp_point* pub;
    sp_point* p = NULL;
    byte one[1] = { 1 };
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(heap, pubd, pub);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        priv = XMALLOC(sizeof(sp_digit) * 8, heap, DYNAMIC_TYPE_ECC);
        if (priv == NULL)
            err = MEMORY_E;
    }
#else
    priv = privd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(pub->x, 8, pX);
        sp_256_from_mp(pub->y, 8, pY);
        sp_256_from_bin(pub->z, 8, one, sizeof(one));
        sp_256_from_mp(priv, 8, privm);

        /* Check point at infinitiy. */
        if (sp_256_iszero_8(pub->x) &&
            sp_256_iszero_8(pub->y))
            err = ECC_INF_E;
    }

    if (err == MP_OKAY) {
        /* Check range of X and Y */
        if (sp_256_cmp_8(pub->x, p256_mod) >= 0 ||
            sp_256_cmp_8(pub->y, p256_mod) >= 0)
            err = ECC_OUT_OF_RANGE_E;
    }

    if (err == MP_OKAY) {
        /* Check point is on curve */
        err = sp_256_ecc_is_point_8(pub, heap);
    }

    if (err == MP_OKAY) {
        /* Point * order = infinity */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_8(p, pub, p256_order, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_8(p, pub, p256_order, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is infinity */
        if (!sp_256_iszero_8(p->x) ||
            !sp_256_iszero_8(p->y)) {
            err = ECC_INF_E;
        }
    }

    if (err == MP_OKAY) {
        /* Base * private = point */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_8(p, priv, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_8(p, priv, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is public key */
        if (sp_256_cmp_8(p->x, pub->x) != 0 ||
            sp_256_cmp_8(p->y, pub->y) != 0) {
            err = ECC_PRIV_KEY_E;
        }
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (priv != NULL)
        XFREE(priv, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p, 0, heap);
    sp_ecc_point_free(pub, 0, heap);

    return err;
}
#endif
#ifdef WOLFSSL_PUBLIC_ECC_ADD_DBL
/* Add two projective EC points together.
 * (pX, pY, pZ) + (qX, qY, qZ) = (rX, rY, rZ)
 *
 * pX   First EC point's X ordinate.
 * pY   First EC point's Y ordinate.
 * pZ   First EC point's Z ordinate.
 * qX   Second EC point's X ordinate.
 * qY   Second EC point's Y ordinate.
 * qZ   Second EC point's Z ordinate.
 * rX   Resultant EC point's X ordinate.
 * rY   Resultant EC point's Y ordinate.
 * rZ   Resultant EC point's Z ordinate.
 * returns MEMORY_E if dynamic memory allocation fails and MP_OKAY otherwise.
 */
int sp_ecc_proj_add_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* qX, mp_int* qY, mp_int* qZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit tmpd[2 * 8 * 5];
    sp_point pd;
    sp_point qd;
#endif
    sp_digit* tmp;
    sp_point* p;
    sp_point* q = NULL;
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(NULL, pd, p);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(NULL, qd, q);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 8 * 5, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 8, pX);
        sp_256_from_mp(p->y, 8, pY);
        sp_256_from_mp(p->z, 8, pZ);
        sp_256_from_mp(q->x, 8, qX);
        sp_256_from_mp(q->y, 8, qY);
        sp_256_from_mp(q->z, 8, qZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_8(p, p, q, tmp);
        else
#endif
            sp_256_proj_point_add_8(p, p, q, tmp);
    }

    if (err == MP_OKAY)
        err = sp_256_to_mp(p->x, rX);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->y, rY);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->z, rZ);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL)
        XFREE(tmp, NULL, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(q, 0, NULL);
    sp_ecc_point_free(p, 0, NULL);

    return err;
}

/* Double a projective EC point.
 * (pX, pY, pZ) + (pX, pY, pZ) = (rX, rY, rZ)
 *
 * pX   EC point's X ordinate.
 * pY   EC point's Y ordinate.
 * pZ   EC point's Z ordinate.
 * rX   Resultant EC point's X ordinate.
 * rY   Resultant EC point's Y ordinate.
 * rZ   Resultant EC point's Z ordinate.
 * returns MEMORY_E if dynamic memory allocation fails and MP_OKAY otherwise.
 */
int sp_ecc_proj_dbl_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit tmpd[2 * 8 * 2];
    sp_point pd;
#endif
    sp_digit* tmp;
    sp_point* p;
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(NULL, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 8 * 2, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 8, pX);
        sp_256_from_mp(p->y, 8, pY);
        sp_256_from_mp(p->z, 8, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_dbl_avx2_8(p, p, tmp);
        else
#endif
            sp_256_proj_point_dbl_8(p, p, tmp);
    }

    if (err == MP_OKAY)
        err = sp_256_to_mp(p->x, rX);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->y, rY);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->z, rZ);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL)
        XFREE(tmp, NULL, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p, 0, NULL);

    return err;
}

/* Map a projective EC point to affine in place.
 * pZ will be one.
 *
 * pX   EC point's X ordinate.
 * pY   EC point's Y ordinate.
 * pZ   EC point's Z ordinate.
 * returns MEMORY_E if dynamic memory allocation fails and MP_OKAY otherwise.
 */
int sp_ecc_map_256(mp_int* pX, mp_int* pY, mp_int* pZ)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit tmpd[2 * 8 * 4];
    sp_point pd;
#endif
    sp_digit* tmp;
    sp_point* p;
    int err;

    err = sp_ecc_point_new(NULL, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 8 * 4, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 8, pX);
        sp_256_from_mp(p->y, 8, pY);
        sp_256_from_mp(p->z, 8, pZ);

        sp_256_map_8(p, p, tmp);
    }

    if (err == MP_OKAY)
        err = sp_256_to_mp(p->x, pX);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->y, pY);
    if (err == MP_OKAY)
        err = sp_256_to_mp(p->z, pZ);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL)
        XFREE(tmp, NULL, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(p, 0, NULL);

    return err;
}
#endif /* WOLFSSL_PUBLIC_ECC_ADD_DBL */
#ifdef HAVE_COMP_KEY
/* Find the square root of a number mod the prime of the curve.
 *
 * y  The number to operate on and the result.
 * returns MEMORY_E if dynamic memory allocation fails and MP_OKAY otherwise.
 */
static int sp_256_mont_sqrt_8(sp_digit* y)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d;
#else
    sp_digit t1d[2 * 8];
    sp_digit t2d[2 * 8];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 8, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 8;
        t2 = d + 2 * 8;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            /* t2 = y ^ 0x2 */
            sp_256_mont_sqr_avx2_8(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_avx2_8(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_avx2_8(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_avx2_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_avx2_8(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_avx2_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_avx2_8(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_avx2_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_avx2_8(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_avx2_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_avx2_8(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_avx2_8(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_avx2_8(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_avx2_8(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_avx2_8(y, t1, 94, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            /* t2 = y ^ 0x2 */
            sp_256_mont_sqr_8(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_8(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_8(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_8(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_8(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_8(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_8(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_8(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_8(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_8(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_8(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_8(y, t1, 94, p256_mod, p256_mp_mod);
        }
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

/* Uncompress the point given the X ordinate.
 *
 * xm    X ordinate.
 * odd   Whether the Y ordinate is odd.
 * ym    Calculated Y ordinate.
 * returns MEMORY_E if dynamic memory allocation fails and MP_OKAY otherwise.
 */
int sp_ecc_uncompress_256(mp_int* xm, int odd, mp_int* ym)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d;
#else
    sp_digit xd[2 * 8];
    sp_digit yd[2 * 8];
#endif
    sp_digit* x;
    sp_digit* y;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 8, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        x = d + 0 * 8;
        y = d + 2 * 8;
    }
    else
        err = MEMORY_E;
#else
    x = xd;
    y = yd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(x, 8, xm);

        err = sp_256_mod_mul_norm_8(x, x, p256_mod);
    }

    if (err == MP_OKAY) {
        /* y = x^3 */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_sqr_avx2_8(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_avx2_8(y, y, x, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            sp_256_mont_sqr_8(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_8(y, y, x, p256_mod, p256_mp_mod);
        }
        /* y = x^3 - 3x */
        sp_256_mont_sub_8(y, y, x, p256_mod);
        sp_256_mont_sub_8(y, y, x, p256_mod);
        sp_256_mont_sub_8(y, y, x, p256_mod);
        /* y = x^3 - 3x + b */
        err = sp_256_mod_mul_norm_8(x, p256_b, p256_mod);
    }
    if (err == MP_OKAY) {
        sp_256_mont_add_8(y, y, x, p256_mod);
        /* y = sqrt(x^3 - 3x + b) */
        err = sp_256_mont_sqrt_8(y);
    }
    if (err == MP_OKAY) {
        XMEMSET(y + 8, 0, 8 * sizeof(sp_digit));
        sp_256_mont_reduce_8(y, p256_mod, p256_mp_mod);
        if (((y[0] ^ odd) & 1) != 0)
            sp_256_mont_sub_8(y, p256_mod, y, p256_mod);

        err = sp_256_to_mp(y, ym);
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}
#endif
#endif /* WOLFSSL_SP_NO_256 */
#endif /* WOLFSSL_HAVE_SP_ECC */
#endif /* WOLFSSL_SP_ARM32_ASM */
#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_ECC */
