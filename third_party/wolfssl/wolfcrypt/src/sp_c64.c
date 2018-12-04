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

#ifndef WOLFSSL_SP_ASM
#if SP_WORD_SIZE == 64
#if defined(WOLFSSL_SP_CACHE_RESISTANT) || defined(WOLFSSL_SP_SMALL)
/* Mask for address to obfuscate which of the two address will be used. */
static const size_t addr_mask[2] = { 0, (size_t)-1 };
#endif

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
        if (s >= 49) {
            r[j] &= 0x1ffffffffffffffl;
            s = 57 - s;
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
#if DIGIT_BIT == 57
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 57
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0x1ffffffffffffffl;
        s = 57 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 57 <= DIGIT_BIT) {
            s += 57;
            r[j] &= 0x1ffffffffffffffl;
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
        if (s + DIGIT_BIT >= 57) {
            r[j] &= 0x1ffffffffffffffl;
            if (j + 1 >= max)
                break;
            s = 57 - s;
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

    for (i=0; i<35; i++) {
        r[i+1] += r[i] >> 57;
        r[i] &= 0x1ffffffffffffffl;
    }
    j = 2048 / 8 - 1;
    a[j] = 0;
    for (i=0; i<36 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 57) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 57);
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
SP_NOINLINE static void sp_2048_mul_9(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int128_t t0   = ((int128_t)a[ 0]) * b[ 0];
    int128_t t1   = ((int128_t)a[ 0]) * b[ 1]
                 + ((int128_t)a[ 1]) * b[ 0];
    int128_t t2   = ((int128_t)a[ 0]) * b[ 2]
                 + ((int128_t)a[ 1]) * b[ 1]
                 + ((int128_t)a[ 2]) * b[ 0];
    int128_t t3   = ((int128_t)a[ 0]) * b[ 3]
                 + ((int128_t)a[ 1]) * b[ 2]
                 + ((int128_t)a[ 2]) * b[ 1]
                 + ((int128_t)a[ 3]) * b[ 0];
    int128_t t4   = ((int128_t)a[ 0]) * b[ 4]
                 + ((int128_t)a[ 1]) * b[ 3]
                 + ((int128_t)a[ 2]) * b[ 2]
                 + ((int128_t)a[ 3]) * b[ 1]
                 + ((int128_t)a[ 4]) * b[ 0];
    int128_t t5   = ((int128_t)a[ 0]) * b[ 5]
                 + ((int128_t)a[ 1]) * b[ 4]
                 + ((int128_t)a[ 2]) * b[ 3]
                 + ((int128_t)a[ 3]) * b[ 2]
                 + ((int128_t)a[ 4]) * b[ 1]
                 + ((int128_t)a[ 5]) * b[ 0];
    int128_t t6   = ((int128_t)a[ 0]) * b[ 6]
                 + ((int128_t)a[ 1]) * b[ 5]
                 + ((int128_t)a[ 2]) * b[ 4]
                 + ((int128_t)a[ 3]) * b[ 3]
                 + ((int128_t)a[ 4]) * b[ 2]
                 + ((int128_t)a[ 5]) * b[ 1]
                 + ((int128_t)a[ 6]) * b[ 0];
    int128_t t7   = ((int128_t)a[ 0]) * b[ 7]
                 + ((int128_t)a[ 1]) * b[ 6]
                 + ((int128_t)a[ 2]) * b[ 5]
                 + ((int128_t)a[ 3]) * b[ 4]
                 + ((int128_t)a[ 4]) * b[ 3]
                 + ((int128_t)a[ 5]) * b[ 2]
                 + ((int128_t)a[ 6]) * b[ 1]
                 + ((int128_t)a[ 7]) * b[ 0];
    int128_t t8   = ((int128_t)a[ 0]) * b[ 8]
                 + ((int128_t)a[ 1]) * b[ 7]
                 + ((int128_t)a[ 2]) * b[ 6]
                 + ((int128_t)a[ 3]) * b[ 5]
                 + ((int128_t)a[ 4]) * b[ 4]
                 + ((int128_t)a[ 5]) * b[ 3]
                 + ((int128_t)a[ 6]) * b[ 2]
                 + ((int128_t)a[ 7]) * b[ 1]
                 + ((int128_t)a[ 8]) * b[ 0];
    int128_t t9   = ((int128_t)a[ 1]) * b[ 8]
                 + ((int128_t)a[ 2]) * b[ 7]
                 + ((int128_t)a[ 3]) * b[ 6]
                 + ((int128_t)a[ 4]) * b[ 5]
                 + ((int128_t)a[ 5]) * b[ 4]
                 + ((int128_t)a[ 6]) * b[ 3]
                 + ((int128_t)a[ 7]) * b[ 2]
                 + ((int128_t)a[ 8]) * b[ 1];
    int128_t t10  = ((int128_t)a[ 2]) * b[ 8]
                 + ((int128_t)a[ 3]) * b[ 7]
                 + ((int128_t)a[ 4]) * b[ 6]
                 + ((int128_t)a[ 5]) * b[ 5]
                 + ((int128_t)a[ 6]) * b[ 4]
                 + ((int128_t)a[ 7]) * b[ 3]
                 + ((int128_t)a[ 8]) * b[ 2];
    int128_t t11  = ((int128_t)a[ 3]) * b[ 8]
                 + ((int128_t)a[ 4]) * b[ 7]
                 + ((int128_t)a[ 5]) * b[ 6]
                 + ((int128_t)a[ 6]) * b[ 5]
                 + ((int128_t)a[ 7]) * b[ 4]
                 + ((int128_t)a[ 8]) * b[ 3];
    int128_t t12  = ((int128_t)a[ 4]) * b[ 8]
                 + ((int128_t)a[ 5]) * b[ 7]
                 + ((int128_t)a[ 6]) * b[ 6]
                 + ((int128_t)a[ 7]) * b[ 5]
                 + ((int128_t)a[ 8]) * b[ 4];
    int128_t t13  = ((int128_t)a[ 5]) * b[ 8]
                 + ((int128_t)a[ 6]) * b[ 7]
                 + ((int128_t)a[ 7]) * b[ 6]
                 + ((int128_t)a[ 8]) * b[ 5];
    int128_t t14  = ((int128_t)a[ 6]) * b[ 8]
                 + ((int128_t)a[ 7]) * b[ 7]
                 + ((int128_t)a[ 8]) * b[ 6];
    int128_t t15  = ((int128_t)a[ 7]) * b[ 8]
                 + ((int128_t)a[ 8]) * b[ 7];
    int128_t t16  = ((int128_t)a[ 8]) * b[ 8];

    t1   += t0  >> 57; r[ 0] = t0  & 0x1ffffffffffffffl;
    t2   += t1  >> 57; r[ 1] = t1  & 0x1ffffffffffffffl;
    t3   += t2  >> 57; r[ 2] = t2  & 0x1ffffffffffffffl;
    t4   += t3  >> 57; r[ 3] = t3  & 0x1ffffffffffffffl;
    t5   += t4  >> 57; r[ 4] = t4  & 0x1ffffffffffffffl;
    t6   += t5  >> 57; r[ 5] = t5  & 0x1ffffffffffffffl;
    t7   += t6  >> 57; r[ 6] = t6  & 0x1ffffffffffffffl;
    t8   += t7  >> 57; r[ 7] = t7  & 0x1ffffffffffffffl;
    t9   += t8  >> 57; r[ 8] = t8  & 0x1ffffffffffffffl;
    t10  += t9  >> 57; r[ 9] = t9  & 0x1ffffffffffffffl;
    t11  += t10 >> 57; r[10] = t10 & 0x1ffffffffffffffl;
    t12  += t11 >> 57; r[11] = t11 & 0x1ffffffffffffffl;
    t13  += t12 >> 57; r[12] = t12 & 0x1ffffffffffffffl;
    t14  += t13 >> 57; r[13] = t13 & 0x1ffffffffffffffl;
    t15  += t14 >> 57; r[14] = t14 & 0x1ffffffffffffffl;
    t16  += t15 >> 57; r[15] = t15 & 0x1ffffffffffffffl;
    r[17] = (sp_digit)(t16 >> 57);
                       r[16] = t16 & 0x1ffffffffffffffl;
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_9(sp_digit* r, const sp_digit* a)
{
    int128_t t0   =  ((int128_t)a[ 0]) * a[ 0];
    int128_t t1   = (((int128_t)a[ 0]) * a[ 1]) * 2;
    int128_t t2   = (((int128_t)a[ 0]) * a[ 2]) * 2
                 +  ((int128_t)a[ 1]) * a[ 1];
    int128_t t3   = (((int128_t)a[ 0]) * a[ 3]
                 +  ((int128_t)a[ 1]) * a[ 2]) * 2;
    int128_t t4   = (((int128_t)a[ 0]) * a[ 4]
                 +  ((int128_t)a[ 1]) * a[ 3]) * 2
                 +  ((int128_t)a[ 2]) * a[ 2];
    int128_t t5   = (((int128_t)a[ 0]) * a[ 5]
                 +  ((int128_t)a[ 1]) * a[ 4]
                 +  ((int128_t)a[ 2]) * a[ 3]) * 2;
    int128_t t6   = (((int128_t)a[ 0]) * a[ 6]
                 +  ((int128_t)a[ 1]) * a[ 5]
                 +  ((int128_t)a[ 2]) * a[ 4]) * 2
                 +  ((int128_t)a[ 3]) * a[ 3];
    int128_t t7   = (((int128_t)a[ 0]) * a[ 7]
                 +  ((int128_t)a[ 1]) * a[ 6]
                 +  ((int128_t)a[ 2]) * a[ 5]
                 +  ((int128_t)a[ 3]) * a[ 4]) * 2;
    int128_t t8   = (((int128_t)a[ 0]) * a[ 8]
                 +  ((int128_t)a[ 1]) * a[ 7]
                 +  ((int128_t)a[ 2]) * a[ 6]
                 +  ((int128_t)a[ 3]) * a[ 5]) * 2
                 +  ((int128_t)a[ 4]) * a[ 4];
    int128_t t9   = (((int128_t)a[ 1]) * a[ 8]
                 +  ((int128_t)a[ 2]) * a[ 7]
                 +  ((int128_t)a[ 3]) * a[ 6]
                 +  ((int128_t)a[ 4]) * a[ 5]) * 2;
    int128_t t10  = (((int128_t)a[ 2]) * a[ 8]
                 +  ((int128_t)a[ 3]) * a[ 7]
                 +  ((int128_t)a[ 4]) * a[ 6]) * 2
                 +  ((int128_t)a[ 5]) * a[ 5];
    int128_t t11  = (((int128_t)a[ 3]) * a[ 8]
                 +  ((int128_t)a[ 4]) * a[ 7]
                 +  ((int128_t)a[ 5]) * a[ 6]) * 2;
    int128_t t12  = (((int128_t)a[ 4]) * a[ 8]
                 +  ((int128_t)a[ 5]) * a[ 7]) * 2
                 +  ((int128_t)a[ 6]) * a[ 6];
    int128_t t13  = (((int128_t)a[ 5]) * a[ 8]
                 +  ((int128_t)a[ 6]) * a[ 7]) * 2;
    int128_t t14  = (((int128_t)a[ 6]) * a[ 8]) * 2
                 +  ((int128_t)a[ 7]) * a[ 7];
    int128_t t15  = (((int128_t)a[ 7]) * a[ 8]) * 2;
    int128_t t16  =  ((int128_t)a[ 8]) * a[ 8];

    t1   += t0  >> 57; r[ 0] = t0  & 0x1ffffffffffffffl;
    t2   += t1  >> 57; r[ 1] = t1  & 0x1ffffffffffffffl;
    t3   += t2  >> 57; r[ 2] = t2  & 0x1ffffffffffffffl;
    t4   += t3  >> 57; r[ 3] = t3  & 0x1ffffffffffffffl;
    t5   += t4  >> 57; r[ 4] = t4  & 0x1ffffffffffffffl;
    t6   += t5  >> 57; r[ 5] = t5  & 0x1ffffffffffffffl;
    t7   += t6  >> 57; r[ 6] = t6  & 0x1ffffffffffffffl;
    t8   += t7  >> 57; r[ 7] = t7  & 0x1ffffffffffffffl;
    t9   += t8  >> 57; r[ 8] = t8  & 0x1ffffffffffffffl;
    t10  += t9  >> 57; r[ 9] = t9  & 0x1ffffffffffffffl;
    t11  += t10 >> 57; r[10] = t10 & 0x1ffffffffffffffl;
    t12  += t11 >> 57; r[11] = t11 & 0x1ffffffffffffffl;
    t13  += t12 >> 57; r[12] = t12 & 0x1ffffffffffffffl;
    t14  += t13 >> 57; r[13] = t13 & 0x1ffffffffffffffl;
    t15  += t14 >> 57; r[14] = t14 & 0x1ffffffffffffffl;
    t16  += t15 >> 57; r[15] = t15 & 0x1ffffffffffffffl;
    r[17] = (sp_digit)(t16 >> 57);
                       r[16] = t16 & 0x1ffffffffffffffl;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_9(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    r[ 0] = a[ 0] + b[ 0];
    r[ 1] = a[ 1] + b[ 1];
    r[ 2] = a[ 2] + b[ 2];
    r[ 3] = a[ 3] + b[ 3];
    r[ 4] = a[ 4] + b[ 4];
    r[ 5] = a[ 5] + b[ 5];
    r[ 6] = a[ 6] + b[ 6];
    r[ 7] = a[ 7] + b[ 7];
    r[ 8] = a[ 8] + b[ 8];

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[16] = a[16] + b[16];
    r[17] = a[17] + b[17];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[16] = a[16] - b[16];
    r[17] = a[17] - b[17];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_18(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[18];
    sp_digit* a1 = z1;
    sp_digit b1[9];
    sp_digit* z2 = r + 18;
    sp_2048_add_9(a1, a, &a[9]);
    sp_2048_add_9(b1, b, &b[9]);
    sp_2048_mul_9(z2, &a[9], &b[9]);
    sp_2048_mul_9(z0, a, b);
    sp_2048_mul_9(z1, a1, b1);
    sp_2048_sub_18(z1, z1, z2);
    sp_2048_sub_18(z1, z1, z0);
    sp_2048_add_18(r + 9, r + 9, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_18(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[18];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 18;
    sp_2048_add_9(a1, a, &a[9]);
    sp_2048_sqr_9(z2, &a[9]);
    sp_2048_sqr_9(z0, a);
    sp_2048_sqr_9(z1, a1);
    sp_2048_sub_18(z1, z1, z2);
    sp_2048_sub_18(z1, z1, z0);
    sp_2048_add_18(r + 9, r + 9, z1);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[32] = a[32] + b[32];
    r[33] = a[33] + b[33];
    r[34] = a[34] + b[34];
    r[35] = a[35] + b[35];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[32] = a[32] - b[32];
    r[33] = a[33] - b[33];
    r[34] = a[34] - b[34];
    r[35] = a[35] - b[35];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_36(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[36];
    sp_digit* a1 = z1;
    sp_digit b1[18];
    sp_digit* z2 = r + 36;
    sp_2048_add_18(a1, a, &a[18]);
    sp_2048_add_18(b1, b, &b[18]);
    sp_2048_mul_18(z2, &a[18], &b[18]);
    sp_2048_mul_18(z0, a, b);
    sp_2048_mul_18(z1, a1, b1);
    sp_2048_sub_36(z1, z1, z2);
    sp_2048_sub_36(z1, z1, z0);
    sp_2048_add_36(r + 18, r + 18, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_36(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[36];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 36;
    sp_2048_add_18(a1, a, &a[18]);
    sp_2048_sqr_18(z2, &a[18]);
    sp_2048_sqr_18(z0, a);
    sp_2048_sqr_18(z1, a1);
    sp_2048_sub_36(z1, z1, z2);
    sp_2048_sub_36(z1, z1, z0);
    sp_2048_add_36(r + 18, r + 18, z1);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_36(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[35]) * b[35];
    r[71] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 69; k >= 0; k--) {
        for (i = 35; i >= 0; i--) {
            j = k - i;
            if (j >= 36)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_36(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[35]) * a[35];
    r[71] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 69; k >= 0; k--) {
        for (i = 35; i >= 0; i--) {
            j = k - i;
            if (j >= 36 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int128_t)a[i]) * a[i];

        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

#endif /* WOLFSSL_SP_SMALL */
#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_18(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[17]) * b[17];
    r[35] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 33; k >= 0; k--) {
        for (i = 17; i >= 0; i--) {
            j = k - i;
            if (j >= 18)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_18(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[17]) * a[17];
    r[35] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 33; k >= 0; k--) {
        for (i = 17; i >= 0; i--) {
            j = k - i;
            if (j >= 18 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int128_t)a[i]) * a[i];

        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
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
    x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
    x &= 0x1ffffffffffffffl;

    /* rho = -1/m mod b */
    *rho = (1L << 57) - x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 2048 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_2048_mont_norm_18(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<17; i++)
        r[i] = 0x1ffffffffffffffl;
#else
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = 0x1ffffffffffffffl;
        r[i + 1] = 0x1ffffffffffffffl;
        r[i + 2] = 0x1ffffffffffffffl;
        r[i + 3] = 0x1ffffffffffffffl;
        r[i + 4] = 0x1ffffffffffffffl;
        r[i + 5] = 0x1ffffffffffffffl;
        r[i + 6] = 0x1ffffffffffffffl;
        r[i + 7] = 0x1ffffffffffffffl;
    }
    r[16] = 0x1ffffffffffffffl;
#endif
    r[17] = 0x7fffffffffffffl;

    /* r = (2^n - 1) mod n */
    sp_2048_sub_18(r, r, m);

    /* Add one so r = 2^n mod m */
    r[0] += 1;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static sp_digit sp_2048_cmp_18(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=17; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[17] - b[17]) & (0 - !r);
    r |= (a[16] - b[16]) & (0 - !r);
    for (i = 8; i >= 0; i -= 8) {
        r |= (a[i + 7] - b[i + 7]) & (0 - !r);
        r |= (a[i + 6] - b[i + 6]) & (0 - !r);
        r |= (a[i + 5] - b[i + 5]) & (0 - !r);
        r |= (a[i + 4] - b[i + 4]) & (0 - !r);
        r |= (a[i + 3] - b[i + 3]) & (0 - !r);
        r |= (a[i + 2] - b[i + 2]) & (0 - !r);
        r |= (a[i + 1] - b[i + 1]) & (0 - !r);
        r |= (a[i + 0] - b[i + 0]) & (0 - !r);
    }
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static void sp_2048_cond_sub_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[16] = a[16] - (b[16] & m);
    r[17] = a[17] - (b[17] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 18; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[18] += t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 16; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[17]; r[17] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    r[18] +=  t[1] >> 57;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 57.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_2048_norm_18(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 17; i++) {
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
#else
    int i;
    for (i = 0; i < 16; i += 8) {
        a[i+1] += a[i+0] >> 57; a[i+0] &= 0x1ffffffffffffffl;
        a[i+2] += a[i+1] >> 57; a[i+1] &= 0x1ffffffffffffffl;
        a[i+3] += a[i+2] >> 57; a[i+2] &= 0x1ffffffffffffffl;
        a[i+4] += a[i+3] >> 57; a[i+3] &= 0x1ffffffffffffffl;
        a[i+5] += a[i+4] >> 57; a[i+4] &= 0x1ffffffffffffffl;
        a[i+6] += a[i+5] >> 57; a[i+5] &= 0x1ffffffffffffffl;
        a[i+7] += a[i+6] >> 57; a[i+6] &= 0x1ffffffffffffffl;
        a[i+8] += a[i+7] >> 57; a[i+7] &= 0x1ffffffffffffffl;
        a[i+9] += a[i+8] >> 57; a[i+8] &= 0x1ffffffffffffffl;
    }
    a[16+1] += a[16] >> 57;
    a[16] &= 0x1ffffffffffffffl;
#endif
}

/* Shift the result in the high 1024 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_2048_mont_shift_18(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    word64 n;

    n = a[17] >> 55;
    for (i = 0; i < 17; i++) {
        n += a[18 + i] << 2;
        r[i] = n & 0x1ffffffffffffffl;
        n >>= 57;
    }
    n += a[35] << 2;
    r[17] = n;
#else
    word64 n;
    int i;

    n  = a[17] >> 55;
    for (i = 0; i < 16; i += 8) {
        n += a[i+18] << 2; r[i+0] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+19] << 2; r[i+1] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+20] << 2; r[i+2] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+21] << 2; r[i+3] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+22] << 2; r[i+4] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+23] << 2; r[i+5] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+24] << 2; r[i+6] = n & 0x1ffffffffffffffl; n >>= 57;
        n += a[i+25] << 2; r[i+7] = n & 0x1ffffffffffffffl; n >>= 57;
    }
    n += a[34] << 2; r[16] = n & 0x1ffffffffffffffl; n >>= 57;
    n += a[35] << 2; r[17] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[18], 0, sizeof(*r) * 18);
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_2048_mont_reduce_18(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    for (i=0; i<17; i++) {
        mu = (a[i] * mp) & 0x1ffffffffffffffl;
        sp_2048_mul_add_18(a+i, m, mu);
        a[i+1] += a[i] >> 57;
    }
    mu = (a[i] * mp) & 0x7fffffffffffffl;
    sp_2048_mul_add_18(a+i, m, mu);
    a[i+1] += a[i] >> 57;
    a[i] &= 0x1ffffffffffffffl;

    sp_2048_mont_shift_18(a, a);
    sp_2048_cond_sub_18(a, a, m, 0 - ((a[17] >> 55) > 0));
    sp_2048_norm_18(a);
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
static void sp_2048_mont_mul_18(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_18(r, a, b);
    sp_2048_mont_reduce_18(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_18(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_18(r, a);
    sp_2048_mont_reduce_18(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_d_18(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 18; i++) {
        t += tb * a[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[18] = (sp_digit)t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 16; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[17];
    r[17] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    r[18] =  (sp_digit)(t[1] >> 57);
#endif /* WOLFSSL_SP_SMALL */
}

/* Conditionally add a and b using the mask m.
 * m is -1 to add and 0 when not.
 *
 * r  A single precision number representing conditional add result.
 * a  A single precision number to add with.
 * b  A single precision number to add.
 * m  Mask value to apply.
 */
static void sp_2048_cond_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[16] = a[16] + (b[16] & m);
    r[17] = a[17] + (b[17] & m);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#endif
#ifdef WOLFSSL_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 18; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif
/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_2048_div_18(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int128_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[36], t2d[18 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 18 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 18;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[17];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 18);
        for (i=17; i>=0; i--) {
            t1[18 + i] += t1[18 + i - 1] >> 57;
            t1[18 + i - 1] &= 0x1ffffffffffffffl;
            d1 = t1[18 + i];
            d1 <<= 57;
            d1 += t1[18 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_2048_mul_d_18(t2, d, r1);
            sp_2048_sub_18(&t1[i], &t1[i], t2);
            t1[18 + i] -= t2[18];
            t1[18 + i] += t1[18 + i - 1] >> 57;
            t1[18 + i - 1] &= 0x1ffffffffffffffl;
            r1 = (((-t1[18 + i]) << 57) - t1[18 + i - 1]) / div;
            r1++;
            sp_2048_mul_d_18(t2, d, r1);
            sp_2048_add_18(&t1[i], &t1[i], t2);
            t1[18 + i] += t1[18 + i - 1] >> 57;
            t1[18 + i - 1] &= 0x1ffffffffffffffl;
        }
        t1[18 - 1] += t1[18 - 2] >> 57;
        t1[18 - 2] &= 0x1ffffffffffffffl;
        d1 = t1[18 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_2048_mul_d_18(t2, d, r1);
        sp_2048_sub_18(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 18);
        for (i=0; i<16; i++) {
            r[i+1] += r[i] >> 57;
            r[i] &= 0x1ffffffffffffffl;
        }
        sp_2048_cond_add_18(r, r, d, 0 - (r[17] < 0));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_2048_mod_18(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_18(a, m, NULL, r);
}

/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_18(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
    sp_digit* m, int reduceA)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* td;
    sp_digit* t[3];
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 18 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 18 * 2);

        norm = t[0] = td;
        t[1] = &td[18 * 2];
        t[2] = &td[2 * 18 * 2];

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_18(norm, m);

        if (reduceA)
            err = sp_2048_mod_18(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 18);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_18(t[1], t[1], norm);
        err = sp_2048_mod_18(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_2048_mont_mul_18(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 18 * 2);
            sp_2048_mont_sqr_18(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 18 * 2);
        }

        sp_2048_mont_reduce_18(t[0], m, mp);
        n = sp_2048_cmp_18(t[0], m);
        sp_2048_cond_sub_18(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 18 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][36];
#else
    sp_digit* td;
    sp_digit* t[3];
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 18 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[18 * 2];
        t[2] = &td[2 * 18 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_18(norm, m);

        if (reduceA) {
            err = sp_2048_mod_18(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_18(t[1], t[1], norm);
                err = sp_2048_mod_18(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_18(t[1], a, norm);
            err = sp_2048_mod_18(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_2048_mont_mul_18(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_2048_mont_sqr_18(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_2048_mont_reduce_18(t[0], m, mp);
        n = sp_2048_cmp_18(t[0], m);
        sp_2048_cond_sub_18(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][36];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[36];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 36, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 36;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_18(norm, m);

        if (reduceA) {
            err = sp_2048_mod_18(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_18(t[1], t[1], norm);
                err = sp_2048_mod_18(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_18(t[1], a, norm);
            err = sp_2048_mod_18(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_18(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_18(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_18(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_18(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_18(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_18(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_18(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_18(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_18(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_18(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_18(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_18(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_18(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_18(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_18(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_18(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_18(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_18(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_18(t[20], t[10], m, mp);
        sp_2048_mont_mul_18(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_18(t[22], t[11], m, mp);
        sp_2048_mont_mul_18(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_18(t[24], t[12], m, mp);
        sp_2048_mont_mul_18(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_18(t[26], t[13], m, mp);
        sp_2048_mont_mul_18(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_18(t[28], t[14], m, mp);
        sp_2048_mont_mul_18(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_18(t[30], t[15], m, mp);
        sp_2048_mont_mul_18(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 56) / 57) - 1;
        c = bits % 57;
        if (c == 0)
            c = 57;
        if (i < 18)
            n = e[i--] << (64 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (7 - c);
            c += 57;
        }
        y = n >> 59;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (7 - c);
                c += 57;
            }
            y = (n >> 59) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_2048_mont_sqr_18(rt, rt, m, mp);
            sp_2048_mont_sqr_18(rt, rt, m, mp);
            sp_2048_mont_sqr_18(rt, rt, m, mp);
            sp_2048_mont_sqr_18(rt, rt, m, mp);
            sp_2048_mont_sqr_18(rt, rt, m, mp);

            sp_2048_mont_mul_18(rt, rt, t[y], m, mp);
        }

        sp_2048_mont_reduce_18(rt, m, mp);
        n = sp_2048_cmp_18(rt, m);
        sp_2048_cond_sub_18(rt, rt, m, (n < 0) - 1);
        XMEMCPY(r, rt, sizeof(rt));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
}

#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 2048 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_2048_mont_norm_36(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<35; i++)
        r[i] = 0x1ffffffffffffffl;
#else
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = 0x1ffffffffffffffl;
        r[i + 1] = 0x1ffffffffffffffl;
        r[i + 2] = 0x1ffffffffffffffl;
        r[i + 3] = 0x1ffffffffffffffl;
        r[i + 4] = 0x1ffffffffffffffl;
        r[i + 5] = 0x1ffffffffffffffl;
        r[i + 6] = 0x1ffffffffffffffl;
        r[i + 7] = 0x1ffffffffffffffl;
    }
    r[32] = 0x1ffffffffffffffl;
    r[33] = 0x1ffffffffffffffl;
    r[34] = 0x1ffffffffffffffl;
#endif
    r[35] = 0x1fffffffffffffl;

    /* r = (2^n - 1) mod n */
    sp_2048_sub_36(r, r, m);

    /* Add one so r = 2^n mod m */
    r[0] += 1;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static sp_digit sp_2048_cmp_36(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=35; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[35] - b[35]) & (0 - !r);
    r |= (a[34] - b[34]) & (0 - !r);
    r |= (a[33] - b[33]) & (0 - !r);
    r |= (a[32] - b[32]) & (0 - !r);
    for (i = 24; i >= 0; i -= 8) {
        r |= (a[i + 7] - b[i + 7]) & (0 - !r);
        r |= (a[i + 6] - b[i + 6]) & (0 - !r);
        r |= (a[i + 5] - b[i + 5]) & (0 - !r);
        r |= (a[i + 4] - b[i + 4]) & (0 - !r);
        r |= (a[i + 3] - b[i + 3]) & (0 - !r);
        r |= (a[i + 2] - b[i + 2]) & (0 - !r);
        r |= (a[i + 1] - b[i + 1]) & (0 - !r);
        r |= (a[i + 0] - b[i + 0]) & (0 - !r);
    }
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static void sp_2048_cond_sub_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[32] = a[32] - (b[32] & m);
    r[33] = a[33] - (b[33] & m);
    r[34] = a[34] - (b[34] & m);
    r[35] = a[35] - (b[35] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 36; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[36] += t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 32; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[33]; r[33] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[34]; r[34] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    t[3] = tb * a[35]; r[35] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
    r[36] +=  t[3] >> 57;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 57.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_2048_norm_36(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 35; i++) {
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
#else
    int i;
    for (i = 0; i < 32; i += 8) {
        a[i+1] += a[i+0] >> 57; a[i+0] &= 0x1ffffffffffffffl;
        a[i+2] += a[i+1] >> 57; a[i+1] &= 0x1ffffffffffffffl;
        a[i+3] += a[i+2] >> 57; a[i+2] &= 0x1ffffffffffffffl;
        a[i+4] += a[i+3] >> 57; a[i+3] &= 0x1ffffffffffffffl;
        a[i+5] += a[i+4] >> 57; a[i+4] &= 0x1ffffffffffffffl;
        a[i+6] += a[i+5] >> 57; a[i+5] &= 0x1ffffffffffffffl;
        a[i+7] += a[i+6] >> 57; a[i+6] &= 0x1ffffffffffffffl;
        a[i+8] += a[i+7] >> 57; a[i+7] &= 0x1ffffffffffffffl;
        a[i+9] += a[i+8] >> 57; a[i+8] &= 0x1ffffffffffffffl;
    }
    a[32+1] += a[32] >> 57;
    a[32] &= 0x1ffffffffffffffl;
    a[33+1] += a[33] >> 57;
    a[33] &= 0x1ffffffffffffffl;
    a[34+1] += a[34] >> 57;
    a[34] &= 0x1ffffffffffffffl;
#endif
}

/* Shift the result in the high 2048 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_2048_mont_shift_36(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    sp_digit n, s;

    s = a[36];
    n = a[35] >> 53;
    for (i = 0; i < 35; i++) {
        n += (s & 0x1ffffffffffffffl) << 4;
        r[i] = n & 0x1ffffffffffffffl;
        n >>= 57;
        s = a[37 + i] + (s >> 57);
    }
    n += s << 4;
    r[35] = n;
#else
    sp_digit n, s;
    int i;

    s = a[36]; n = a[35] >> 53;
    for (i = 0; i < 32; i += 8) {
        n += (s & 0x1ffffffffffffffl) << 4; r[i+0] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+37] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+1] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+38] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+2] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+39] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+3] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+40] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+4] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+41] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+5] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+42] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+6] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+43] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 4; r[i+7] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+44] + (s >> 57);
    }
    n += (s & 0x1ffffffffffffffl) << 4; r[32] = n & 0x1ffffffffffffffl;
    n >>= 57; s = a[69] + (s >> 57);
    n += (s & 0x1ffffffffffffffl) << 4; r[33] = n & 0x1ffffffffffffffl;
    n >>= 57; s = a[70] + (s >> 57);
    n += (s & 0x1ffffffffffffffl) << 4; r[34] = n & 0x1ffffffffffffffl;
    n >>= 57; s = a[71] + (s >> 57);
    n += s << 4;              r[35] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[36], 0, sizeof(*r) * 36);
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_2048_mont_reduce_36(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<35; i++) {
            mu = (a[i] * mp) & 0x1ffffffffffffffl;
            sp_2048_mul_add_36(a+i, m, mu);
            a[i+1] += a[i] >> 57;
        }
        mu = (a[i] * mp) & 0x1fffffffffffffl;
        sp_2048_mul_add_36(a+i, m, mu);
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
    else {
        for (i=0; i<35; i++) {
            mu = a[i] & 0x1ffffffffffffffl;
            sp_2048_mul_add_36(a+i, m, mu);
            a[i+1] += a[i] >> 57;
        }
        mu = a[i] & 0x1fffffffffffffl;
        sp_2048_mul_add_36(a+i, m, mu);
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }

    sp_2048_mont_shift_36(a, a);
    sp_2048_cond_sub_36(a, a, m, 0 - ((a[35] >> 53) > 0));
    sp_2048_norm_36(a);
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
static void sp_2048_mont_mul_36(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_36(r, a, b);
    sp_2048_mont_reduce_36(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_36(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_36(r, a);
    sp_2048_mont_reduce_36(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_d_36(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 36; i++) {
        t += tb * a[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[36] = (sp_digit)t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 32; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[33];
    r[33] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[34];
    r[34] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    t[3] = tb * a[35];
    r[35] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
    r[36] =  (sp_digit)(t[3] >> 57);
#endif /* WOLFSSL_SP_SMALL */
}

/* Conditionally add a and b using the mask m.
 * m is -1 to add and 0 when not.
 *
 * r  A single precision number representing conditional add result.
 * a  A single precision number to add with.
 * b  A single precision number to add.
 * m  Mask value to apply.
 */
static void sp_2048_cond_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[32] = a[32] + (b[32] & m);
    r[33] = a[33] + (b[33] & m);
    r[34] = a[34] + (b[34] & m);
    r[35] = a[35] + (b[35] & m);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#endif
#ifdef WOLFSSL_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 36; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif
/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_2048_div_36(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int128_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[72], t2d[36 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 36 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 36;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[35];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 36);
        for (i=35; i>=0; i--) {
            t1[36 + i] += t1[36 + i - 1] >> 57;
            t1[36 + i - 1] &= 0x1ffffffffffffffl;
            d1 = t1[36 + i];
            d1 <<= 57;
            d1 += t1[36 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_2048_mul_d_36(t2, d, r1);
            sp_2048_sub_36(&t1[i], &t1[i], t2);
            t1[36 + i] -= t2[36];
            t1[36 + i] += t1[36 + i - 1] >> 57;
            t1[36 + i - 1] &= 0x1ffffffffffffffl;
            r1 = (((-t1[36 + i]) << 57) - t1[36 + i - 1]) / div;
            r1++;
            sp_2048_mul_d_36(t2, d, r1);
            sp_2048_add_36(&t1[i], &t1[i], t2);
            t1[36 + i] += t1[36 + i - 1] >> 57;
            t1[36 + i - 1] &= 0x1ffffffffffffffl;
        }
        t1[36 - 1] += t1[36 - 2] >> 57;
        t1[36 - 2] &= 0x1ffffffffffffffl;
        d1 = t1[36 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_2048_mul_d_36(t2, d, r1);
        sp_2048_sub_36(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 36);
        for (i=0; i<34; i++) {
            r[i+1] += r[i] >> 57;
            r[i] &= 0x1ffffffffffffffl;
        }
        sp_2048_cond_add_36(r, r, d, 0 - (r[35] < 0));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_2048_mod_36(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_36(a, m, NULL, r);
}

#if defined(SP_RSA_PRIVATE_EXP_D) || defined(WOLFSSL_HAVE_SP_DH)
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_2048_mod_exp_36(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
    sp_digit* m, int reduceA)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* td;
    sp_digit* t[3];
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 36 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 36 * 2);

        norm = t[0] = td;
        t[1] = &td[36 * 2];
        t[2] = &td[2 * 36 * 2];

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_36(norm, m);

        if (reduceA)
            err = sp_2048_mod_36(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 36);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_36(t[1], t[1], norm);
        err = sp_2048_mod_36(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_2048_mont_mul_36(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 36 * 2);
            sp_2048_mont_sqr_36(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 36 * 2);
        }

        sp_2048_mont_reduce_36(t[0], m, mp);
        n = sp_2048_cmp_36(t[0], m);
        sp_2048_cond_sub_36(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 36 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][72];
#else
    sp_digit* td;
    sp_digit* t[3];
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 36 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[36 * 2];
        t[2] = &td[2 * 36 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_36(norm, m);

        if (reduceA) {
            err = sp_2048_mod_36(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_36(t[1], t[1], norm);
                err = sp_2048_mod_36(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_36(t[1], a, norm);
            err = sp_2048_mod_36(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_2048_mont_mul_36(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_2048_mont_sqr_36(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_2048_mont_reduce_36(t[0], m, mp);
        n = sp_2048_cmp_36(t[0], m);
        sp_2048_cond_sub_36(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][72];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[72];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 72, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 72;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_36(norm, m);

        if (reduceA) {
            err = sp_2048_mod_36(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_36(t[1], t[1], norm);
                err = sp_2048_mod_36(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_36(t[1], a, norm);
            err = sp_2048_mod_36(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_36(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_36(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_36(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_36(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_36(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_36(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_36(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_36(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_36(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_36(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_36(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_36(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_36(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_36(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_36(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_36(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_36(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_36(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_36(t[20], t[10], m, mp);
        sp_2048_mont_mul_36(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_36(t[22], t[11], m, mp);
        sp_2048_mont_mul_36(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_36(t[24], t[12], m, mp);
        sp_2048_mont_mul_36(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_36(t[26], t[13], m, mp);
        sp_2048_mont_mul_36(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_36(t[28], t[14], m, mp);
        sp_2048_mont_mul_36(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_36(t[30], t[15], m, mp);
        sp_2048_mont_mul_36(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 56) / 57) - 1;
        c = bits % 57;
        if (c == 0)
            c = 57;
        if (i < 36)
            n = e[i--] << (64 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (7 - c);
            c += 57;
        }
        y = n >> 59;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (7 - c);
                c += 57;
            }
            y = (n >> 59) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_2048_mont_sqr_36(rt, rt, m, mp);
            sp_2048_mont_sqr_36(rt, rt, m, mp);
            sp_2048_mont_sqr_36(rt, rt, m, mp);
            sp_2048_mont_sqr_36(rt, rt, m, mp);
            sp_2048_mont_sqr_36(rt, rt, m, mp);

            sp_2048_mont_mul_36(rt, rt, t[y], m, mp);
        }

        sp_2048_mont_reduce_36(rt, m, mp);
        n = sp_2048_cmp_36(rt, m);
        sp_2048_cond_sub_36(rt, rt, m, (n < 0) - 1);
        XMEMCPY(r, rt, sizeof(rt));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
}
#endif /* SP_RSA_PRIVATE_EXP_D || WOLFSSL_HAVE_SP_DH */

#if defined(WOLFSSL_HAVE_SP_RSA) && !defined(SP_RSA_PRIVATE_EXP_D)
/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_2048_mask_18(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<18; i++)
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
    r[16] = a[16] & m;
    r[17] = a[17] & m;
#endif
}

#endif
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
#ifdef WOLFSSL_SP_SMALL
    sp_digit* d = NULL;
    sp_digit* a;
    sp_digit* m;
    sp_digit* r;
    sp_digit* norm;
    sp_digit e[1];
    sp_digit mp;
    int i;
    int err = MP_OKAY;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 57 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 36 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 36 * 2;
        m = r + 36 * 2;
        norm = r;

        sp_2048_from_bin(a, 36, in, inLen);
#if DIGIT_BIT >= 57
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
        sp_2048_from_mp(m, 36, mm);

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_36(norm, m);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_36(a, a, norm);
        err = sp_2048_mod_36(a, a, m);
    }
    if (err == MP_OKAY) {
        for (i=56; i>=0; i--)
            if (e[0] >> i)
                break;

        XMEMCPY(r, a, sizeof(sp_digit) * 36 * 2);
        for (i--; i>=0; i--) {
            sp_2048_mont_sqr_36(r, r, m, mp);

            if (((e[0] >> i) & 1) == 1)
                sp_2048_mont_mul_36(r, r, a, m, mp);
        }
        sp_2048_mont_reduce_36(r, m, mp);
        mp = sp_2048_cmp_36(r, m);
        sp_2048_cond_sub_36(r, r, m, (mp < 0) - 1);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#else
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[72], md[36], rd[72];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* a;
    sp_digit* m;
    sp_digit* r;
    sp_digit e[1];
    int err = MP_OKAY;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 57 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 36 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 36 * 2;
        m = r + 36 * 2;
    }
#else
    a = ad;
    m = md;
    r = rd;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 36, in, inLen);
#if DIGIT_BIT >= 57
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
        sp_2048_from_mp(m, 36, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_2048_sqr_36(r, a);
                err = sp_2048_mod_36(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_2048_mul_36(r, a, r);
                err = sp_2048_mod_36(r, r, m);
            }
        }
        else {
            sp_digit* norm = r;
            int i;
            sp_digit mp;

            sp_2048_mont_setup(m, &mp);
            sp_2048_mont_norm_36(norm, m);

            if (err == MP_OKAY) {
                sp_2048_mul_36(a, a, norm);
                err = sp_2048_mod_36(a, a, m);
            }

            if (err == MP_OKAY) {
                for (i=56; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 72);
                for (i--; i>=0; i--) {
                    sp_2048_mont_sqr_36(r, r, m, mp);

                    if (((e[0] >> i) & 1) == 1)
                        sp_2048_mont_mul_36(r, r, a, m, mp);
                }
                sp_2048_mont_reduce_36(r, m, mp);
                mp = sp_2048_cmp_36(r, m);
                sp_2048_cond_sub_36(r, r, m, (mp < 0) - 1);
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
#endif /* WOLFSSL_SP_SMALL */
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
#ifdef SP_RSA_PRIVATE_EXP_D
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* a;
    sp_digit* d = NULL;
    sp_digit* m;
    sp_digit* r;
    int err = MP_OKAY;

    (void)pm;
    (void)qm;
    (void)dpm;
    (void)dqm;
    (void)qim;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(dm) > 2048 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 36 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = d + 36;
        m = a + 36;
        r = a;

        sp_2048_from_bin(a, 36, in, inLen);
        sp_2048_from_mp(d, 36, dm);
        sp_2048_from_mp(m, 36, mm);
        err = sp_2048_mod_exp_36(r, a, d, 2048, m, 0);
    }
    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (d != NULL) {
        XMEMSET(d, 0, sizeof(sp_digit) * 36);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[72], d[36], m[36];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)pm;
    (void)qm;
    (void)dpm;
    (void)dqm;
    (void)qim;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(dm) > 2048 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 36, in, inLen);
        sp_2048_from_mp(d, 36, dm);
        sp_2048_from_mp(m, 36, mm);
        err = sp_2048_mod_exp_36(r, a, d, 2048, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    XMEMSET(d, 0, sizeof(sp_digit) * 36);

    return err;
#endif /* WOLFSSL_SP_SMALL || defined(WOLFSSL_SMALL_STACK) */
#else
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* t = NULL;
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
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 256 || mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 18 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 36 * 2;
        q = p + 18;
        qi = dq = dp = q + 18;
        tmpa = qi + 18;
        tmpb = tmpa + 36;

        tmp = t;
        r = tmp + 36;

        sp_2048_from_bin(a, 36, in, inLen);
        sp_2048_from_mp(p, 18, pm);
        sp_2048_from_mp(q, 18, qm);
        sp_2048_from_mp(dp, 18, dpm);
        err = sp_2048_mod_exp_18(tmpa, a, dp, 1024, p, 1);
    }
    if (err == MP_OKAY) {
        sp_2048_from_mp(dq, 18, dqm);
        err = sp_2048_mod_exp_18(tmpb, a, dq, 1024, q, 1);
    }
    if (err == MP_OKAY) {
        sp_2048_sub_18(tmpa, tmpa, tmpb);
        sp_2048_mask_18(tmp, p, tmpa[17] >> 63);
        sp_2048_add_18(tmpa, tmpa, tmp);

        sp_2048_from_mp(qi, 18, qim);
        sp_2048_mul_18(tmpa, tmpa, qi);
        err = sp_2048_mod_18(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_2048_mul_18(tmpa, q, tmpa);
        sp_2048_add_36(r, tmpb, tmpa);
        sp_2048_norm_36(r);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 18 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[36 * 2];
    sp_digit p[18], q[18], dp[18], dq[18], qi[18];
    sp_digit tmp[36], tmpa[36], tmpb[36];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 256 || mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 36, in, inLen);
        sp_2048_from_mp(p, 18, pm);
        sp_2048_from_mp(q, 18, qm);
        sp_2048_from_mp(dp, 18, dpm);
        sp_2048_from_mp(dq, 18, dqm);
        sp_2048_from_mp(qi, 18, qim);

        err = sp_2048_mod_exp_18(tmpa, a, dp, 1024, p, 1);
    }
    if (err == MP_OKAY)
        err = sp_2048_mod_exp_18(tmpb, a, dq, 1024, q, 1);

    if (err == MP_OKAY) {
        sp_2048_sub_18(tmpa, tmpa, tmpb);
        sp_2048_mask_18(tmp, p, tmpa[17] >> 63);
        sp_2048_add_18(tmpa, tmpa, tmp);
        sp_2048_mul_18(tmpa, tmpa, qi);
        err = sp_2048_mod_18(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_2048_mul_18(tmpa, tmpa, q);
        sp_2048_add_36(r, tmpb, tmpa);
        sp_2048_norm_36(r);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    XMEMSET(tmpa, 0, sizeof(tmpa));
    XMEMSET(tmpb, 0, sizeof(tmpb));
    XMEMSET(p, 0, sizeof(p));
    XMEMSET(q, 0, sizeof(q));
    XMEMSET(dp, 0, sizeof(dp));
    XMEMSET(dq, 0, sizeof(dq));
    XMEMSET(qi, 0, sizeof(qi));

    return err;
#endif /* WOLFSSL_SP_SMALL || defined(WOLFSSL_SMALL_STACK) */
#endif /* SP_RSA_PRIVATE_EXP_D */
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
#if DIGIT_BIT == 57
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 36);
        r->used = 36;
        mp_clamp(r);
#elif DIGIT_BIT < 57
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 36; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 57) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 57 - s;
        }
        r->used = (2048 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 36; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 57 >= DIGIT_BIT) {
    #if DIGIT_BIT < 64
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 57 - s;
            }
            else
                s += 57;
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
#ifdef WOLFSSL_SP_SMALL
    int err = MP_OKAY;
    sp_digit* d = NULL;
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 2048 || expBits > 2048 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 36 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 36 * 2;
        m = e + 36;
        r = b;

        sp_2048_from_mp(b, 36, base);
        sp_2048_from_mp(e, 36, exp);
        sp_2048_from_mp(m, 36, mod);

        err = sp_2048_mod_exp_36(r, b, e, mp_count_bits(exp), m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_2048_to_mp(r, res);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 36);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[72], ed[36], md[36];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    int err = MP_OKAY;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 2048 || expBits > 2048 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 36 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 36 * 2;
        m = e + 36;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 36, base);
        sp_2048_from_mp(e, 36, exp);
        sp_2048_from_mp(m, 36, mod);

        err = sp_2048_mod_exp_36(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_2048_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 36);

#ifdef WOLFSSL_SMALL_STACK
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
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
#ifdef WOLFSSL_SP_SMALL
    int err = MP_OKAY;
    sp_digit* d = NULL;
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    word32 i;

    if (mp_count_bits(base) > 2048 || expLen > 256 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 36 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 36 * 2;
        m = e + 36;
        r = b;

        sp_2048_from_mp(b, 36, base);
        sp_2048_from_bin(e, 36, exp, expLen);
        sp_2048_from_mp(m, 36, mod);

        err = sp_2048_mod_exp_36(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
        for (i=0; i<256 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 36);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[72], ed[36], md[36];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    word32 i;
    int err = MP_OKAY;

    if (mp_count_bits(base) > 2048 || expLen > 256 ||
                                                   mp_count_bits(mod) != 2048) {
        err = MP_READ_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 36 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 36 * 2;
        m = e + 36;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 36, base);
        sp_2048_from_bin(e, 36, exp, expLen);
        sp_2048_from_mp(m, 36, mod);

        err = sp_2048_mod_exp_36(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
        for (i=0; i<256 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 36);

#ifdef WOLFSSL_SMALL_STACK
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
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
        if (s >= 49) {
            r[j] &= 0x1ffffffffffffffl;
            s = 57 - s;
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
#if DIGIT_BIT == 57
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 57
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0x1ffffffffffffffl;
        s = 57 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 57 <= DIGIT_BIT) {
            s += 57;
            r[j] &= 0x1ffffffffffffffl;
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
        if (s + DIGIT_BIT >= 57) {
            r[j] &= 0x1ffffffffffffffl;
            if (j + 1 >= max)
                break;
            s = 57 - s;
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

    for (i=0; i<53; i++) {
        r[i+1] += r[i] >> 57;
        r[i] &= 0x1ffffffffffffffl;
    }
    j = 3072 / 8 - 1;
    a[j] = 0;
    for (i=0; i<54 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 57) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 57);
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
SP_NOINLINE static void sp_3072_mul_9(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int128_t t0   = ((int128_t)a[ 0]) * b[ 0];
    int128_t t1   = ((int128_t)a[ 0]) * b[ 1]
                 + ((int128_t)a[ 1]) * b[ 0];
    int128_t t2   = ((int128_t)a[ 0]) * b[ 2]
                 + ((int128_t)a[ 1]) * b[ 1]
                 + ((int128_t)a[ 2]) * b[ 0];
    int128_t t3   = ((int128_t)a[ 0]) * b[ 3]
                 + ((int128_t)a[ 1]) * b[ 2]
                 + ((int128_t)a[ 2]) * b[ 1]
                 + ((int128_t)a[ 3]) * b[ 0];
    int128_t t4   = ((int128_t)a[ 0]) * b[ 4]
                 + ((int128_t)a[ 1]) * b[ 3]
                 + ((int128_t)a[ 2]) * b[ 2]
                 + ((int128_t)a[ 3]) * b[ 1]
                 + ((int128_t)a[ 4]) * b[ 0];
    int128_t t5   = ((int128_t)a[ 0]) * b[ 5]
                 + ((int128_t)a[ 1]) * b[ 4]
                 + ((int128_t)a[ 2]) * b[ 3]
                 + ((int128_t)a[ 3]) * b[ 2]
                 + ((int128_t)a[ 4]) * b[ 1]
                 + ((int128_t)a[ 5]) * b[ 0];
    int128_t t6   = ((int128_t)a[ 0]) * b[ 6]
                 + ((int128_t)a[ 1]) * b[ 5]
                 + ((int128_t)a[ 2]) * b[ 4]
                 + ((int128_t)a[ 3]) * b[ 3]
                 + ((int128_t)a[ 4]) * b[ 2]
                 + ((int128_t)a[ 5]) * b[ 1]
                 + ((int128_t)a[ 6]) * b[ 0];
    int128_t t7   = ((int128_t)a[ 0]) * b[ 7]
                 + ((int128_t)a[ 1]) * b[ 6]
                 + ((int128_t)a[ 2]) * b[ 5]
                 + ((int128_t)a[ 3]) * b[ 4]
                 + ((int128_t)a[ 4]) * b[ 3]
                 + ((int128_t)a[ 5]) * b[ 2]
                 + ((int128_t)a[ 6]) * b[ 1]
                 + ((int128_t)a[ 7]) * b[ 0];
    int128_t t8   = ((int128_t)a[ 0]) * b[ 8]
                 + ((int128_t)a[ 1]) * b[ 7]
                 + ((int128_t)a[ 2]) * b[ 6]
                 + ((int128_t)a[ 3]) * b[ 5]
                 + ((int128_t)a[ 4]) * b[ 4]
                 + ((int128_t)a[ 5]) * b[ 3]
                 + ((int128_t)a[ 6]) * b[ 2]
                 + ((int128_t)a[ 7]) * b[ 1]
                 + ((int128_t)a[ 8]) * b[ 0];
    int128_t t9   = ((int128_t)a[ 1]) * b[ 8]
                 + ((int128_t)a[ 2]) * b[ 7]
                 + ((int128_t)a[ 3]) * b[ 6]
                 + ((int128_t)a[ 4]) * b[ 5]
                 + ((int128_t)a[ 5]) * b[ 4]
                 + ((int128_t)a[ 6]) * b[ 3]
                 + ((int128_t)a[ 7]) * b[ 2]
                 + ((int128_t)a[ 8]) * b[ 1];
    int128_t t10  = ((int128_t)a[ 2]) * b[ 8]
                 + ((int128_t)a[ 3]) * b[ 7]
                 + ((int128_t)a[ 4]) * b[ 6]
                 + ((int128_t)a[ 5]) * b[ 5]
                 + ((int128_t)a[ 6]) * b[ 4]
                 + ((int128_t)a[ 7]) * b[ 3]
                 + ((int128_t)a[ 8]) * b[ 2];
    int128_t t11  = ((int128_t)a[ 3]) * b[ 8]
                 + ((int128_t)a[ 4]) * b[ 7]
                 + ((int128_t)a[ 5]) * b[ 6]
                 + ((int128_t)a[ 6]) * b[ 5]
                 + ((int128_t)a[ 7]) * b[ 4]
                 + ((int128_t)a[ 8]) * b[ 3];
    int128_t t12  = ((int128_t)a[ 4]) * b[ 8]
                 + ((int128_t)a[ 5]) * b[ 7]
                 + ((int128_t)a[ 6]) * b[ 6]
                 + ((int128_t)a[ 7]) * b[ 5]
                 + ((int128_t)a[ 8]) * b[ 4];
    int128_t t13  = ((int128_t)a[ 5]) * b[ 8]
                 + ((int128_t)a[ 6]) * b[ 7]
                 + ((int128_t)a[ 7]) * b[ 6]
                 + ((int128_t)a[ 8]) * b[ 5];
    int128_t t14  = ((int128_t)a[ 6]) * b[ 8]
                 + ((int128_t)a[ 7]) * b[ 7]
                 + ((int128_t)a[ 8]) * b[ 6];
    int128_t t15  = ((int128_t)a[ 7]) * b[ 8]
                 + ((int128_t)a[ 8]) * b[ 7];
    int128_t t16  = ((int128_t)a[ 8]) * b[ 8];

    t1   += t0  >> 57; r[ 0] = t0  & 0x1ffffffffffffffl;
    t2   += t1  >> 57; r[ 1] = t1  & 0x1ffffffffffffffl;
    t3   += t2  >> 57; r[ 2] = t2  & 0x1ffffffffffffffl;
    t4   += t3  >> 57; r[ 3] = t3  & 0x1ffffffffffffffl;
    t5   += t4  >> 57; r[ 4] = t4  & 0x1ffffffffffffffl;
    t6   += t5  >> 57; r[ 5] = t5  & 0x1ffffffffffffffl;
    t7   += t6  >> 57; r[ 6] = t6  & 0x1ffffffffffffffl;
    t8   += t7  >> 57; r[ 7] = t7  & 0x1ffffffffffffffl;
    t9   += t8  >> 57; r[ 8] = t8  & 0x1ffffffffffffffl;
    t10  += t9  >> 57; r[ 9] = t9  & 0x1ffffffffffffffl;
    t11  += t10 >> 57; r[10] = t10 & 0x1ffffffffffffffl;
    t12  += t11 >> 57; r[11] = t11 & 0x1ffffffffffffffl;
    t13  += t12 >> 57; r[12] = t12 & 0x1ffffffffffffffl;
    t14  += t13 >> 57; r[13] = t13 & 0x1ffffffffffffffl;
    t15  += t14 >> 57; r[14] = t14 & 0x1ffffffffffffffl;
    t16  += t15 >> 57; r[15] = t15 & 0x1ffffffffffffffl;
    r[17] = (sp_digit)(t16 >> 57);
                       r[16] = t16 & 0x1ffffffffffffffl;
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_9(sp_digit* r, const sp_digit* a)
{
    int128_t t0   =  ((int128_t)a[ 0]) * a[ 0];
    int128_t t1   = (((int128_t)a[ 0]) * a[ 1]) * 2;
    int128_t t2   = (((int128_t)a[ 0]) * a[ 2]) * 2
                 +  ((int128_t)a[ 1]) * a[ 1];
    int128_t t3   = (((int128_t)a[ 0]) * a[ 3]
                 +  ((int128_t)a[ 1]) * a[ 2]) * 2;
    int128_t t4   = (((int128_t)a[ 0]) * a[ 4]
                 +  ((int128_t)a[ 1]) * a[ 3]) * 2
                 +  ((int128_t)a[ 2]) * a[ 2];
    int128_t t5   = (((int128_t)a[ 0]) * a[ 5]
                 +  ((int128_t)a[ 1]) * a[ 4]
                 +  ((int128_t)a[ 2]) * a[ 3]) * 2;
    int128_t t6   = (((int128_t)a[ 0]) * a[ 6]
                 +  ((int128_t)a[ 1]) * a[ 5]
                 +  ((int128_t)a[ 2]) * a[ 4]) * 2
                 +  ((int128_t)a[ 3]) * a[ 3];
    int128_t t7   = (((int128_t)a[ 0]) * a[ 7]
                 +  ((int128_t)a[ 1]) * a[ 6]
                 +  ((int128_t)a[ 2]) * a[ 5]
                 +  ((int128_t)a[ 3]) * a[ 4]) * 2;
    int128_t t8   = (((int128_t)a[ 0]) * a[ 8]
                 +  ((int128_t)a[ 1]) * a[ 7]
                 +  ((int128_t)a[ 2]) * a[ 6]
                 +  ((int128_t)a[ 3]) * a[ 5]) * 2
                 +  ((int128_t)a[ 4]) * a[ 4];
    int128_t t9   = (((int128_t)a[ 1]) * a[ 8]
                 +  ((int128_t)a[ 2]) * a[ 7]
                 +  ((int128_t)a[ 3]) * a[ 6]
                 +  ((int128_t)a[ 4]) * a[ 5]) * 2;
    int128_t t10  = (((int128_t)a[ 2]) * a[ 8]
                 +  ((int128_t)a[ 3]) * a[ 7]
                 +  ((int128_t)a[ 4]) * a[ 6]) * 2
                 +  ((int128_t)a[ 5]) * a[ 5];
    int128_t t11  = (((int128_t)a[ 3]) * a[ 8]
                 +  ((int128_t)a[ 4]) * a[ 7]
                 +  ((int128_t)a[ 5]) * a[ 6]) * 2;
    int128_t t12  = (((int128_t)a[ 4]) * a[ 8]
                 +  ((int128_t)a[ 5]) * a[ 7]) * 2
                 +  ((int128_t)a[ 6]) * a[ 6];
    int128_t t13  = (((int128_t)a[ 5]) * a[ 8]
                 +  ((int128_t)a[ 6]) * a[ 7]) * 2;
    int128_t t14  = (((int128_t)a[ 6]) * a[ 8]) * 2
                 +  ((int128_t)a[ 7]) * a[ 7];
    int128_t t15  = (((int128_t)a[ 7]) * a[ 8]) * 2;
    int128_t t16  =  ((int128_t)a[ 8]) * a[ 8];

    t1   += t0  >> 57; r[ 0] = t0  & 0x1ffffffffffffffl;
    t2   += t1  >> 57; r[ 1] = t1  & 0x1ffffffffffffffl;
    t3   += t2  >> 57; r[ 2] = t2  & 0x1ffffffffffffffl;
    t4   += t3  >> 57; r[ 3] = t3  & 0x1ffffffffffffffl;
    t5   += t4  >> 57; r[ 4] = t4  & 0x1ffffffffffffffl;
    t6   += t5  >> 57; r[ 5] = t5  & 0x1ffffffffffffffl;
    t7   += t6  >> 57; r[ 6] = t6  & 0x1ffffffffffffffl;
    t8   += t7  >> 57; r[ 7] = t7  & 0x1ffffffffffffffl;
    t9   += t8  >> 57; r[ 8] = t8  & 0x1ffffffffffffffl;
    t10  += t9  >> 57; r[ 9] = t9  & 0x1ffffffffffffffl;
    t11  += t10 >> 57; r[10] = t10 & 0x1ffffffffffffffl;
    t12  += t11 >> 57; r[11] = t11 & 0x1ffffffffffffffl;
    t13  += t12 >> 57; r[12] = t12 & 0x1ffffffffffffffl;
    t14  += t13 >> 57; r[13] = t13 & 0x1ffffffffffffffl;
    t15  += t14 >> 57; r[14] = t14 & 0x1ffffffffffffffl;
    t16  += t15 >> 57; r[15] = t15 & 0x1ffffffffffffffl;
    r[17] = (sp_digit)(t16 >> 57);
                       r[16] = t16 & 0x1ffffffffffffffl;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_9(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    r[ 0] = a[ 0] + b[ 0];
    r[ 1] = a[ 1] + b[ 1];
    r[ 2] = a[ 2] + b[ 2];
    r[ 3] = a[ 3] + b[ 3];
    r[ 4] = a[ 4] + b[ 4];
    r[ 5] = a[ 5] + b[ 5];
    r[ 6] = a[ 6] + b[ 6];
    r[ 7] = a[ 7] + b[ 7];
    r[ 8] = a[ 8] + b[ 8];

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[16] = a[16] + b[16];
    r[17] = a[17] + b[17];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_18(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 16; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[16] = a[16] - b[16];
    r[17] = a[17] - b[17];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_18(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[18];
    sp_digit* a1 = z1;
    sp_digit b1[9];
    sp_digit* z2 = r + 18;
    sp_3072_add_9(a1, a, &a[9]);
    sp_3072_add_9(b1, b, &b[9]);
    sp_3072_mul_9(z2, &a[9], &b[9]);
    sp_3072_mul_9(z0, a, b);
    sp_3072_mul_9(z1, a1, b1);
    sp_3072_sub_18(z1, z1, z2);
    sp_3072_sub_18(z1, z1, z0);
    sp_3072_add_18(r + 9, r + 9, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_18(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[18];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 18;
    sp_3072_add_9(a1, a, &a[9]);
    sp_3072_sqr_9(z2, &a[9]);
    sp_3072_sqr_9(z0, a);
    sp_3072_sqr_9(z1, a1);
    sp_3072_sub_18(z1, z1, z2);
    sp_3072_sub_18(z1, z1, z0);
    sp_3072_add_18(r + 9, r + 9, z1);
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[32] = a[32] - b[32];
    r[33] = a[33] - b[33];
    r[34] = a[34] - b[34];
    r[35] = a[35] - b[35];

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_36(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 32; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[32] = a[32] + b[32];
    r[33] = a[33] + b[33];
    r[34] = a[34] + b[34];
    r[35] = a[35] + b[35];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_54(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit p0[36];
    sp_digit p1[36];
    sp_digit p2[36];
    sp_digit p3[36];
    sp_digit p4[36];
    sp_digit p5[36];
    sp_digit t0[36];
    sp_digit t1[36];
    sp_digit t2[36];
    sp_digit a0[18];
    sp_digit a1[18];
    sp_digit a2[18];
    sp_digit b0[18];
    sp_digit b1[18];
    sp_digit b2[18];
    sp_3072_add_18(a0, a, &a[18]);
    sp_3072_add_18(b0, b, &b[18]);
    sp_3072_add_18(a1, &a[18], &a[36]);
    sp_3072_add_18(b1, &b[18], &b[36]);
    sp_3072_add_18(a2, a0, &a[36]);
    sp_3072_add_18(b2, b0, &b[36]);
    sp_3072_mul_18(p0, a, b);
    sp_3072_mul_18(p2, &a[18], &b[18]);
    sp_3072_mul_18(p4, &a[36], &b[36]);
    sp_3072_mul_18(p1, a0, b0);
    sp_3072_mul_18(p3, a1, b1);
    sp_3072_mul_18(p5, a2, b2);
    XMEMSET(r, 0, sizeof(*r)*2*54);
    sp_3072_sub_36(t0, p3, p2);
    sp_3072_sub_36(t1, p1, p2);
    sp_3072_sub_36(t2, p5, t0);
    sp_3072_sub_36(t2, t2, t1);
    sp_3072_sub_36(t0, t0, p4);
    sp_3072_sub_36(t1, t1, p0);
    sp_3072_add_36(r, r, p0);
    sp_3072_add_36(&r[18], &r[18], t1);
    sp_3072_add_36(&r[36], &r[36], t2);
    sp_3072_add_36(&r[54], &r[54], t0);
    sp_3072_add_36(&r[72], &r[72], p4);
}

/* Square a into r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_54(sp_digit* r, const sp_digit* a)
{
    sp_digit p0[36];
    sp_digit p1[36];
    sp_digit p2[36];
    sp_digit p3[36];
    sp_digit p4[36];
    sp_digit p5[36];
    sp_digit t0[36];
    sp_digit t1[36];
    sp_digit t2[36];
    sp_digit a0[18];
    sp_digit a1[18];
    sp_digit a2[18];
    sp_3072_add_18(a0, a, &a[18]);
    sp_3072_add_18(a1, &a[18], &a[36]);
    sp_3072_add_18(a2, a0, &a[36]);
    sp_3072_sqr_18(p0, a);
    sp_3072_sqr_18(p2, &a[18]);
    sp_3072_sqr_18(p4, &a[36]);
    sp_3072_sqr_18(p1, a0);
    sp_3072_sqr_18(p3, a1);
    sp_3072_sqr_18(p5, a2);
    XMEMSET(r, 0, sizeof(*r)*2*54);
    sp_3072_sub_36(t0, p3, p2);
    sp_3072_sub_36(t1, p1, p2);
    sp_3072_sub_36(t2, p5, t0);
    sp_3072_sub_36(t2, t2, t1);
    sp_3072_sub_36(t0, t0, p4);
    sp_3072_sub_36(t1, t1, p0);
    sp_3072_add_36(r, r, p0);
    sp_3072_add_36(&r[18], &r[18], t1);
    sp_3072_add_36(&r[36], &r[36], t2);
    sp_3072_add_36(&r[54], &r[54], t0);
    sp_3072_add_36(&r[72], &r[72], p4);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 54; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#else
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[48] = a[48] + b[48];
    r[49] = a[49] + b[49];
    r[50] = a[50] + b[50];
    r[51] = a[51] + b[51];
    r[52] = a[52] + b[52];
    r[53] = a[53] + b[53];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 54; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#else
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[48] = a[48] - b[48];
    r[49] = a[49] - b[49];
    r[50] = a[50] - b[50];
    r[51] = a[51] - b[51];
    r[52] = a[52] - b[52];
    r[53] = a[53] - b[53];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_54(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[53]) * b[53];
    r[107] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 105; k >= 0; k--) {
        for (i = 53; i >= 0; i--) {
            j = k - i;
            if (j >= 54)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_54(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[53]) * a[53];
    r[107] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 105; k >= 0; k--) {
        for (i = 53; i >= 0; i--) {
            j = k - i;
            if (j >= 54 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int128_t)a[i]) * a[i];

        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

#endif /* WOLFSSL_SP_SMALL */
#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 27; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#else
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[24] = a[24] + b[24];
    r[25] = a[25] + b[25];
    r[26] = a[26] + b[26];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 27; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#else
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[24] = a[24] - b[24];
    r[25] = a[25] - b[25];
    r[26] = a[26] - b[26];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_27(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[26]) * b[26];
    r[53] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 51; k >= 0; k--) {
        for (i = 26; i >= 0; i--) {
            j = k - i;
            if (j >= 27)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

#else
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_27(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j;
    int128_t t[54];

    XMEMSET(t, 0, sizeof(t));
    for (i=0; i<27; i++) {
        for (j=0; j<27; j++)
            t[i+j] += ((int128_t)a[i]) * b[j];
    }
    for (i=0; i<53; i++) {
        r[i] = t[i] & 0x1ffffffffffffffl;
        t[i+1] += t[i] >> 57;
    }
    r[53] = (sp_digit)t[53];
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_27(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[26]) * a[26];
    r[53] = (sp_digit)(c >> 57);
    c = (c & 0x1ffffffffffffffl) << 57;
    for (k = 51; k >= 0; k--) {
        for (i = 26; i >= 0; i--) {
            j = k - i;
            if (j >= 27 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int128_t)a[i]) * a[i];

        r[k + 2] += c >> 114;
        r[k + 1] = (c >> 57) & 0x1ffffffffffffffl;
        c = (c & 0x1ffffffffffffffl) << 57;
    }
    r[0] = (sp_digit)(c >> 57);
}

#else
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_27(sp_digit* r, const sp_digit* a)
{
    int i, j;
    int128_t t[54];

    XMEMSET(t, 0, sizeof(t));
    for (i=0; i<27; i++) {
        for (j=0; j<i; j++)
            t[i+j] += (((int128_t)a[i]) * a[j]) * 2;
        t[i+i] += ((int128_t)a[i]) * a[i];
    }
    for (i=0; i<53; i++) {
        r[i] = t[i] & 0x1ffffffffffffffl;
        t[i+1] += t[i] >> 57;
    }
    r[53] = (sp_digit)t[53];
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
    x *= 2 - b * x;               /* here x*a==1 mod 2**64 */
    x &= 0x1ffffffffffffffl;

    /* rho = -1/m mod b */
    *rho = (1L << 57) - x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 3072 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_3072_mont_norm_27(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<26; i++)
        r[i] = 0x1ffffffffffffffl;
#else
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i + 0] = 0x1ffffffffffffffl;
        r[i + 1] = 0x1ffffffffffffffl;
        r[i + 2] = 0x1ffffffffffffffl;
        r[i + 3] = 0x1ffffffffffffffl;
        r[i + 4] = 0x1ffffffffffffffl;
        r[i + 5] = 0x1ffffffffffffffl;
        r[i + 6] = 0x1ffffffffffffffl;
        r[i + 7] = 0x1ffffffffffffffl;
    }
    r[24] = 0x1ffffffffffffffl;
    r[25] = 0x1ffffffffffffffl;
#endif
    r[26] = 0x3fffffffffffffl;

    /* r = (2^n - 1) mod n */
    sp_3072_sub_27(r, r, m);

    /* Add one so r = 2^n mod m */
    r[0] += 1;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static sp_digit sp_3072_cmp_27(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=26; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[26] - b[26]) & (0 - !r);
    r |= (a[25] - b[25]) & (0 - !r);
    r |= (a[24] - b[24]) & (0 - !r);
    for (i = 16; i >= 0; i -= 8) {
        r |= (a[i + 7] - b[i + 7]) & (0 - !r);
        r |= (a[i + 6] - b[i + 6]) & (0 - !r);
        r |= (a[i + 5] - b[i + 5]) & (0 - !r);
        r |= (a[i + 4] - b[i + 4]) & (0 - !r);
        r |= (a[i + 3] - b[i + 3]) & (0 - !r);
        r |= (a[i + 2] - b[i + 2]) & (0 - !r);
        r |= (a[i + 1] - b[i + 1]) & (0 - !r);
        r |= (a[i + 0] - b[i + 0]) & (0 - !r);
    }
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static void sp_3072_cond_sub_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 27; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[24] = a[24] - (b[24] & m);
    r[25] = a[25] - (b[25] & m);
    r[26] = a[26] - (b[26] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_add_27(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 27; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[27] += t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 24; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[25]; r[25] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[26]; r[26] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    r[27] +=  t[2] >> 57;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 57.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_3072_norm_27(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 26; i++) {
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
#else
    int i;
    for (i = 0; i < 24; i += 8) {
        a[i+1] += a[i+0] >> 57; a[i+0] &= 0x1ffffffffffffffl;
        a[i+2] += a[i+1] >> 57; a[i+1] &= 0x1ffffffffffffffl;
        a[i+3] += a[i+2] >> 57; a[i+2] &= 0x1ffffffffffffffl;
        a[i+4] += a[i+3] >> 57; a[i+3] &= 0x1ffffffffffffffl;
        a[i+5] += a[i+4] >> 57; a[i+4] &= 0x1ffffffffffffffl;
        a[i+6] += a[i+5] >> 57; a[i+5] &= 0x1ffffffffffffffl;
        a[i+7] += a[i+6] >> 57; a[i+6] &= 0x1ffffffffffffffl;
        a[i+8] += a[i+7] >> 57; a[i+7] &= 0x1ffffffffffffffl;
        a[i+9] += a[i+8] >> 57; a[i+8] &= 0x1ffffffffffffffl;
    }
    a[24+1] += a[24] >> 57;
    a[24] &= 0x1ffffffffffffffl;
    a[25+1] += a[25] >> 57;
    a[25] &= 0x1ffffffffffffffl;
#endif
}

/* Shift the result in the high 1536 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_3072_mont_shift_27(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    sp_digit n, s;

    s = a[27];
    n = a[26] >> 54;
    for (i = 0; i < 26; i++) {
        n += (s & 0x1ffffffffffffffl) << 3;
        r[i] = n & 0x1ffffffffffffffl;
        n >>= 57;
        s = a[28 + i] + (s >> 57);
    }
    n += s << 3;
    r[26] = n;
#else
    sp_digit n, s;
    int i;

    s = a[27]; n = a[26] >> 54;
    for (i = 0; i < 24; i += 8) {
        n += (s & 0x1ffffffffffffffl) << 3; r[i+0] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+28] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+1] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+29] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+2] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+30] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+3] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+31] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+4] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+32] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+5] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+33] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+6] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+34] + (s >> 57);
        n += (s & 0x1ffffffffffffffl) << 3; r[i+7] = n & 0x1ffffffffffffffl;
        n >>= 57; s = a[i+35] + (s >> 57);
    }
    n += (s & 0x1ffffffffffffffl) << 3; r[24] = n & 0x1ffffffffffffffl;
    n >>= 57; s = a[52] + (s >> 57);
    n += (s & 0x1ffffffffffffffl) << 3; r[25] = n & 0x1ffffffffffffffl;
    n >>= 57; s = a[53] + (s >> 57);
    n += s << 3;              r[26] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[27], 0, sizeof(*r) * 27);
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_3072_mont_reduce_27(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    for (i=0; i<26; i++) {
        mu = (a[i] * mp) & 0x1ffffffffffffffl;
        sp_3072_mul_add_27(a+i, m, mu);
        a[i+1] += a[i] >> 57;
    }
    mu = (a[i] * mp) & 0x3fffffffffffffl;
    sp_3072_mul_add_27(a+i, m, mu);
    a[i+1] += a[i] >> 57;
    a[i] &= 0x1ffffffffffffffl;

    sp_3072_mont_shift_27(a, a);
    sp_3072_cond_sub_27(a, a, m, 0 - ((a[26] >> 54) > 0));
    sp_3072_norm_27(a);
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
static void sp_3072_mont_mul_27(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_27(r, a, b);
    sp_3072_mont_reduce_27(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_27(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_27(r, a);
    sp_3072_mont_reduce_27(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_d_27(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 27; i++) {
        t += tb * a[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[27] = (sp_digit)t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 24; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[25];
    r[25] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[26];
    r[26] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    r[27] =  (sp_digit)(t[2] >> 57);
#endif /* WOLFSSL_SP_SMALL */
}

/* Conditionally add a and b using the mask m.
 * m is -1 to add and 0 when not.
 *
 * r  A single precision number representing conditional add result.
 * a  A single precision number to add with.
 * b  A single precision number to add.
 * m  Mask value to apply.
 */
static void sp_3072_cond_add_27(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 27; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[24] = a[24] + (b[24] & m);
    r[25] = a[25] + (b[25] & m);
    r[26] = a[26] + (b[26] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_3072_div_27(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int128_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[54], t2d[27 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 27 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 27;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[26];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 27);
        for (i=26; i>=0; i--) {
            t1[27 + i] += t1[27 + i - 1] >> 57;
            t1[27 + i - 1] &= 0x1ffffffffffffffl;
            d1 = t1[27 + i];
            d1 <<= 57;
            d1 += t1[27 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_3072_mul_d_27(t2, d, r1);
            sp_3072_sub_27(&t1[i], &t1[i], t2);
            t1[27 + i] -= t2[27];
            t1[27 + i] += t1[27 + i - 1] >> 57;
            t1[27 + i - 1] &= 0x1ffffffffffffffl;
            r1 = (((-t1[27 + i]) << 57) - t1[27 + i - 1]) / div;
            r1++;
            sp_3072_mul_d_27(t2, d, r1);
            sp_3072_add_27(&t1[i], &t1[i], t2);
            t1[27 + i] += t1[27 + i - 1] >> 57;
            t1[27 + i - 1] &= 0x1ffffffffffffffl;
        }
        t1[27 - 1] += t1[27 - 2] >> 57;
        t1[27 - 2] &= 0x1ffffffffffffffl;
        d1 = t1[27 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_3072_mul_d_27(t2, d, r1);
        sp_3072_sub_27(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 27);
        for (i=0; i<25; i++) {
            r[i+1] += r[i] >> 57;
            r[i] &= 0x1ffffffffffffffl;
        }
        sp_3072_cond_add_27(r, r, d, 0 - (r[26] < 0));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_3072_mod_27(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_27(a, m, NULL, r);
}

/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_27(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
    sp_digit* m, int reduceA)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* td;
    sp_digit* t[3];
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 27 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 27 * 2);

        norm = t[0] = td;
        t[1] = &td[27 * 2];
        t[2] = &td[2 * 27 * 2];

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_27(norm, m);

        if (reduceA)
            err = sp_3072_mod_27(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 27);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_27(t[1], t[1], norm);
        err = sp_3072_mod_27(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_3072_mont_mul_27(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 27 * 2);
            sp_3072_mont_sqr_27(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 27 * 2);
        }

        sp_3072_mont_reduce_27(t[0], m, mp);
        n = sp_3072_cmp_27(t[0], m);
        sp_3072_cond_sub_27(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 27 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][54];
#else
    sp_digit* td;
    sp_digit* t[3];
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 27 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[27 * 2];
        t[2] = &td[2 * 27 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_27(norm, m);

        if (reduceA) {
            err = sp_3072_mod_27(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_27(t[1], t[1], norm);
                err = sp_3072_mod_27(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_27(t[1], a, norm);
            err = sp_3072_mod_27(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_3072_mont_mul_27(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_3072_mont_sqr_27(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_3072_mont_reduce_27(t[0], m, mp);
        n = sp_3072_cmp_27(t[0], m);
        sp_3072_cond_sub_27(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][54];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[54];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 54, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 54;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_27(norm, m);

        if (reduceA) {
            err = sp_3072_mod_27(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_27(t[1], t[1], norm);
                err = sp_3072_mod_27(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_27(t[1], a, norm);
            err = sp_3072_mod_27(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_27(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_27(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_27(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_27(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_27(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_27(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_27(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_27(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_27(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_27(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_27(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_27(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_27(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_27(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_27(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_27(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_27(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_27(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_27(t[20], t[10], m, mp);
        sp_3072_mont_mul_27(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_27(t[22], t[11], m, mp);
        sp_3072_mont_mul_27(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_27(t[24], t[12], m, mp);
        sp_3072_mont_mul_27(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_27(t[26], t[13], m, mp);
        sp_3072_mont_mul_27(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_27(t[28], t[14], m, mp);
        sp_3072_mont_mul_27(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_27(t[30], t[15], m, mp);
        sp_3072_mont_mul_27(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 56) / 57) - 1;
        c = bits % 57;
        if (c == 0)
            c = 57;
        if (i < 27)
            n = e[i--] << (64 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (7 - c);
            c += 57;
        }
        y = n >> 59;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (7 - c);
                c += 57;
            }
            y = (n >> 59) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_3072_mont_sqr_27(rt, rt, m, mp);
            sp_3072_mont_sqr_27(rt, rt, m, mp);
            sp_3072_mont_sqr_27(rt, rt, m, mp);
            sp_3072_mont_sqr_27(rt, rt, m, mp);
            sp_3072_mont_sqr_27(rt, rt, m, mp);

            sp_3072_mont_mul_27(rt, rt, t[y], m, mp);
        }

        sp_3072_mont_reduce_27(rt, m, mp);
        n = sp_3072_cmp_27(rt, m);
        sp_3072_cond_sub_27(rt, rt, m, (n < 0) - 1);
        XMEMCPY(r, rt, sizeof(rt));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
}

#endif /* !SP_RSA_PRIVATE_EXP_D && WOLFSSL_HAVE_SP_RSA */

/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 3072 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_3072_mont_norm_54(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<53; i++)
        r[i] = 0x1ffffffffffffffl;
#else
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i + 0] = 0x1ffffffffffffffl;
        r[i + 1] = 0x1ffffffffffffffl;
        r[i + 2] = 0x1ffffffffffffffl;
        r[i + 3] = 0x1ffffffffffffffl;
        r[i + 4] = 0x1ffffffffffffffl;
        r[i + 5] = 0x1ffffffffffffffl;
        r[i + 6] = 0x1ffffffffffffffl;
        r[i + 7] = 0x1ffffffffffffffl;
    }
    r[48] = 0x1ffffffffffffffl;
    r[49] = 0x1ffffffffffffffl;
    r[50] = 0x1ffffffffffffffl;
    r[51] = 0x1ffffffffffffffl;
    r[52] = 0x1ffffffffffffffl;
#endif
    r[53] = 0x7ffffffffffffl;

    /* r = (2^n - 1) mod n */
    sp_3072_sub_54(r, r, m);

    /* Add one so r = 2^n mod m */
    r[0] += 1;
}

/* Compare a with b in constant time.
 *
 * a  A single precision integer.
 * b  A single precision integer.
 * return -ve, 0 or +ve if a is less than, equal to or greater than b
 * respectively.
 */
static sp_digit sp_3072_cmp_54(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=53; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[53] - b[53]) & (0 - !r);
    r |= (a[52] - b[52]) & (0 - !r);
    r |= (a[51] - b[51]) & (0 - !r);
    r |= (a[50] - b[50]) & (0 - !r);
    r |= (a[49] - b[49]) & (0 - !r);
    r |= (a[48] - b[48]) & (0 - !r);
    for (i = 40; i >= 0; i -= 8) {
        r |= (a[i + 7] - b[i + 7]) & (0 - !r);
        r |= (a[i + 6] - b[i + 6]) & (0 - !r);
        r |= (a[i + 5] - b[i + 5]) & (0 - !r);
        r |= (a[i + 4] - b[i + 4]) & (0 - !r);
        r |= (a[i + 3] - b[i + 3]) & (0 - !r);
        r |= (a[i + 2] - b[i + 2]) & (0 - !r);
        r |= (a[i + 1] - b[i + 1]) & (0 - !r);
        r |= (a[i + 0] - b[i + 0]) & (0 - !r);
    }
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static void sp_3072_cond_sub_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 54; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[48] = a[48] - (b[48] & m);
    r[49] = a[49] - (b[49] & m);
    r[50] = a[50] - (b[50] & m);
    r[51] = a[51] - (b[51] & m);
    r[52] = a[52] - (b[52] & m);
    r[53] = a[53] - (b[53] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_add_54(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 54; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[54] += t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 48; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[49]; r[49] += (t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[50]; r[50] += (t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    t[3] = tb * a[51]; r[51] += (t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
    t[4] = tb * a[52]; r[52] += (t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
    t[5] = tb * a[53]; r[53] += (t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
    r[54] +=  t[5] >> 57;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 57.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_3072_norm_54(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 53; i++) {
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
#else
    int i;
    for (i = 0; i < 48; i += 8) {
        a[i+1] += a[i+0] >> 57; a[i+0] &= 0x1ffffffffffffffl;
        a[i+2] += a[i+1] >> 57; a[i+1] &= 0x1ffffffffffffffl;
        a[i+3] += a[i+2] >> 57; a[i+2] &= 0x1ffffffffffffffl;
        a[i+4] += a[i+3] >> 57; a[i+3] &= 0x1ffffffffffffffl;
        a[i+5] += a[i+4] >> 57; a[i+4] &= 0x1ffffffffffffffl;
        a[i+6] += a[i+5] >> 57; a[i+5] &= 0x1ffffffffffffffl;
        a[i+7] += a[i+6] >> 57; a[i+6] &= 0x1ffffffffffffffl;
        a[i+8] += a[i+7] >> 57; a[i+7] &= 0x1ffffffffffffffl;
        a[i+9] += a[i+8] >> 57; a[i+8] &= 0x1ffffffffffffffl;
    }
    a[48+1] += a[48] >> 57;
    a[48] &= 0x1ffffffffffffffl;
    a[49+1] += a[49] >> 57;
    a[49] &= 0x1ffffffffffffffl;
    a[50+1] += a[50] >> 57;
    a[50] &= 0x1ffffffffffffffl;
    a[51+1] += a[51] >> 57;
    a[51] &= 0x1ffffffffffffffl;
    a[52+1] += a[52] >> 57;
    a[52] &= 0x1ffffffffffffffl;
#endif
}

/* Shift the result in the high 3072 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_3072_mont_shift_54(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    int128_t n = a[53] >> 51;
    n += ((int128_t)a[54]) << 6;

    for (i = 0; i < 53; i++) {
        r[i] = n & 0x1ffffffffffffffl;
        n >>= 57;
        n += ((int128_t)a[55 + i]) << 6;
    }
    r[53] = (sp_digit)n;
#else
    int i;
    int128_t n = a[53] >> 51;
    n += ((int128_t)a[54]) << 6;
    for (i = 0; i < 48; i += 8) {
        r[i + 0] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 55]) << 6;
        r[i + 1] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 56]) << 6;
        r[i + 2] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 57]) << 6;
        r[i + 3] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 58]) << 6;
        r[i + 4] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 59]) << 6;
        r[i + 5] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 60]) << 6;
        r[i + 6] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 61]) << 6;
        r[i + 7] = n & 0x1ffffffffffffffl;
        n >>= 57; n += ((int128_t)a[i + 62]) << 6;
    }
    r[48] = n & 0x1ffffffffffffffl; n >>= 57; n += ((int128_t)a[103]) << 6;
    r[49] = n & 0x1ffffffffffffffl; n >>= 57; n += ((int128_t)a[104]) << 6;
    r[50] = n & 0x1ffffffffffffffl; n >>= 57; n += ((int128_t)a[105]) << 6;
    r[51] = n & 0x1ffffffffffffffl; n >>= 57; n += ((int128_t)a[106]) << 6;
    r[52] = n & 0x1ffffffffffffffl; n >>= 57; n += ((int128_t)a[107]) << 6;
    r[53] = (sp_digit)n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[54], 0, sizeof(*r) * 54);
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_3072_mont_reduce_54(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<53; i++) {
            mu = (a[i] * mp) & 0x1ffffffffffffffl;
            sp_3072_mul_add_54(a+i, m, mu);
            a[i+1] += a[i] >> 57;
        }
        mu = (a[i] * mp) & 0x7ffffffffffffl;
        sp_3072_mul_add_54(a+i, m, mu);
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }
    else {
        for (i=0; i<53; i++) {
            mu = a[i] & 0x1ffffffffffffffl;
            sp_3072_mul_add_54(a+i, m, mu);
            a[i+1] += a[i] >> 57;
        }
        mu = a[i] & 0x7ffffffffffffl;
        sp_3072_mul_add_54(a+i, m, mu);
        a[i+1] += a[i] >> 57;
        a[i] &= 0x1ffffffffffffffl;
    }

    sp_3072_mont_shift_54(a, a);
    sp_3072_cond_sub_54(a, a, m, 0 - ((a[53] >> 51) > 0));
    sp_3072_norm_54(a);
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
static void sp_3072_mont_mul_54(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_54(r, a, b);
    sp_3072_mont_reduce_54(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_54(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_54(r, a);
    sp_3072_mont_reduce_54(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_d_54(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 54; i++) {
        t += tb * a[i];
        r[i] = t & 0x1ffffffffffffffl;
        t >>= 57;
    }
    r[54] = (sp_digit)t;
#else
    int128_t tb = b;
    int128_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x1ffffffffffffffl;
    for (i = 0; i < 48; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 57) + (t[6] & 0x1ffffffffffffffl);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 57) + (t[7] & 0x1ffffffffffffffl);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 57) + (t[0] & 0x1ffffffffffffffl);
    }
    t[1] = tb * a[49];
    r[49] = (sp_digit)(t[0] >> 57) + (t[1] & 0x1ffffffffffffffl);
    t[2] = tb * a[50];
    r[50] = (sp_digit)(t[1] >> 57) + (t[2] & 0x1ffffffffffffffl);
    t[3] = tb * a[51];
    r[51] = (sp_digit)(t[2] >> 57) + (t[3] & 0x1ffffffffffffffl);
    t[4] = tb * a[52];
    r[52] = (sp_digit)(t[3] >> 57) + (t[4] & 0x1ffffffffffffffl);
    t[5] = tb * a[53];
    r[53] = (sp_digit)(t[4] >> 57) + (t[5] & 0x1ffffffffffffffl);
    r[54] =  (sp_digit)(t[5] >> 57);
#endif /* WOLFSSL_SP_SMALL */
}

/* Conditionally add a and b using the mask m.
 * m is -1 to add and 0 when not.
 *
 * r  A single precision number representing conditional add result.
 * a  A single precision number to add with.
 * b  A single precision number to add.
 * m  Mask value to apply.
 */
static void sp_3072_cond_add_54(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 54; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 48; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[48] = a[48] + (b[48] & m);
    r[49] = a[49] + (b[49] & m);
    r[50] = a[50] + (b[50] & m);
    r[51] = a[51] + (b[51] & m);
    r[52] = a[52] + (b[52] & m);
    r[53] = a[53] + (b[53] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_3072_div_54(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int128_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[108], t2d[54 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 54 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 54;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[53];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 54);
        for (i=53; i>=0; i--) {
            t1[54 + i] += t1[54 + i - 1] >> 57;
            t1[54 + i - 1] &= 0x1ffffffffffffffl;
            d1 = t1[54 + i];
            d1 <<= 57;
            d1 += t1[54 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_3072_mul_d_54(t2, d, r1);
            sp_3072_sub_54(&t1[i], &t1[i], t2);
            t1[54 + i] -= t2[54];
            t1[54 + i] += t1[54 + i - 1] >> 57;
            t1[54 + i - 1] &= 0x1ffffffffffffffl;
            r1 = (((-t1[54 + i]) << 57) - t1[54 + i - 1]) / div;
            r1++;
            sp_3072_mul_d_54(t2, d, r1);
            sp_3072_add_54(&t1[i], &t1[i], t2);
            t1[54 + i] += t1[54 + i - 1] >> 57;
            t1[54 + i - 1] &= 0x1ffffffffffffffl;
        }
        t1[54 - 1] += t1[54 - 2] >> 57;
        t1[54 - 2] &= 0x1ffffffffffffffl;
        d1 = t1[54 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_3072_mul_d_54(t2, d, r1);
        sp_3072_sub_54(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 54);
        for (i=0; i<52; i++) {
            r[i+1] += r[i] >> 57;
            r[i] &= 0x1ffffffffffffffl;
        }
        sp_3072_cond_add_54(r, r, d, 0 - (r[53] < 0));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_3072_mod_54(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_54(a, m, NULL, r);
}

#if defined(SP_RSA_PRIVATE_EXP_D) || defined(WOLFSSL_HAVE_SP_DH)
/* Modular exponentiate a to the e mod m. (r = a^e mod m)
 *
 * r     A single precision number that is the result of the operation.
 * a     A single precision number being exponentiated.
 * e     A single precision number that is the exponent.
 * bits  The number of bits in the exponent.
 * m     A single precision number that is the modulus.
 * returns 0 on success and MEMORY_E on dynamic memory allocation failure.
 */
static int sp_3072_mod_exp_54(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
    sp_digit* m, int reduceA)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* td;
    sp_digit* t[3];
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 54 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 54 * 2);

        norm = t[0] = td;
        t[1] = &td[54 * 2];
        t[2] = &td[2 * 54 * 2];

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_54(norm, m);

        if (reduceA)
            err = sp_3072_mod_54(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 54);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_54(t[1], t[1], norm);
        err = sp_3072_mod_54(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_3072_mont_mul_54(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 54 * 2);
            sp_3072_mont_sqr_54(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 54 * 2);
        }

        sp_3072_mont_reduce_54(t[0], m, mp);
        n = sp_3072_cmp_54(t[0], m);
        sp_3072_cond_sub_54(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 54 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][108];
#else
    sp_digit* td;
    sp_digit* t[3];
#endif
    sp_digit* norm;
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 54 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[54 * 2];
        t[2] = &td[2 * 54 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_54(norm, m);

        if (reduceA) {
            err = sp_3072_mod_54(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_54(t[1], t[1], norm);
                err = sp_3072_mod_54(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_54(t[1], a, norm);
            err = sp_3072_mod_54(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 57;
        c = bits % 57;
        n = e[i--] << (57 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 57;
            }

            y = (n >> 56) & 1;
            n <<= 1;

            sp_3072_mont_mul_54(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_3072_mont_sqr_54(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_3072_mont_reduce_54(t[0], m, mp);
        n = sp_3072_cmp_54(t[0], m);
        sp_3072_cond_sub_54(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][108];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[108];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 108, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 108;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_54(norm, m);

        if (reduceA) {
            err = sp_3072_mod_54(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_54(t[1], t[1], norm);
                err = sp_3072_mod_54(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_54(t[1], a, norm);
            err = sp_3072_mod_54(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_54(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_54(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_54(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_54(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_54(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_54(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_54(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_54(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_54(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_54(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_54(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_54(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_54(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_54(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_54(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_54(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_54(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_54(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_54(t[20], t[10], m, mp);
        sp_3072_mont_mul_54(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_54(t[22], t[11], m, mp);
        sp_3072_mont_mul_54(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_54(t[24], t[12], m, mp);
        sp_3072_mont_mul_54(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_54(t[26], t[13], m, mp);
        sp_3072_mont_mul_54(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_54(t[28], t[14], m, mp);
        sp_3072_mont_mul_54(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_54(t[30], t[15], m, mp);
        sp_3072_mont_mul_54(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 56) / 57) - 1;
        c = bits % 57;
        if (c == 0)
            c = 57;
        if (i < 54)
            n = e[i--] << (64 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (7 - c);
            c += 57;
        }
        y = n >> 59;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (7 - c);
                c += 57;
            }
            y = (n >> 59) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_3072_mont_sqr_54(rt, rt, m, mp);
            sp_3072_mont_sqr_54(rt, rt, m, mp);
            sp_3072_mont_sqr_54(rt, rt, m, mp);
            sp_3072_mont_sqr_54(rt, rt, m, mp);
            sp_3072_mont_sqr_54(rt, rt, m, mp);

            sp_3072_mont_mul_54(rt, rt, t[y], m, mp);
        }

        sp_3072_mont_reduce_54(rt, m, mp);
        n = sp_3072_cmp_54(rt, m);
        sp_3072_cond_sub_54(rt, rt, m, (n < 0) - 1);
        XMEMCPY(r, rt, sizeof(rt));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
}
#endif /* SP_RSA_PRIVATE_EXP_D || WOLFSSL_HAVE_SP_DH */

#if defined(WOLFSSL_HAVE_SP_RSA) && !defined(SP_RSA_PRIVATE_EXP_D)
/* AND m into each word of a and store in r.
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * m  Mask to AND against each digit.
 */
static void sp_3072_mask_27(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<27; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 24; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
    r[24] = a[24] & m;
    r[25] = a[25] & m;
    r[26] = a[26] & m;
#endif
}

#endif
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
#ifdef WOLFSSL_SP_SMALL
    sp_digit* d = NULL;
    sp_digit* a;
    sp_digit* m;
    sp_digit* r;
    sp_digit* norm;
    sp_digit e[1];
    sp_digit mp;
    int i;
    int err = MP_OKAY;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 57 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 54 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 54 * 2;
        m = r + 54 * 2;
        norm = r;

        sp_3072_from_bin(a, 54, in, inLen);
#if DIGIT_BIT >= 57
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
        sp_3072_from_mp(m, 54, mm);

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_54(norm, m);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_54(a, a, norm);
        err = sp_3072_mod_54(a, a, m);
    }
    if (err == MP_OKAY) {
        for (i=56; i>=0; i--)
            if (e[0] >> i)
                break;

        XMEMCPY(r, a, sizeof(sp_digit) * 54 * 2);
        for (i--; i>=0; i--) {
            sp_3072_mont_sqr_54(r, r, m, mp);

            if (((e[0] >> i) & 1) == 1)
                sp_3072_mont_mul_54(r, r, a, m, mp);
        }
        sp_3072_mont_reduce_54(r, m, mp);
        mp = sp_3072_cmp_54(r, m);
        sp_3072_cond_sub_54(r, r, m, (mp < 0) - 1);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#else
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[108], md[54], rd[108];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* a;
    sp_digit* m;
    sp_digit* r;
    sp_digit e[1];
    int err = MP_OKAY;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(em) > 57 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 54 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 54 * 2;
        m = r + 54 * 2;
    }
#else
    a = ad;
    m = md;
    r = rd;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 54, in, inLen);
#if DIGIT_BIT >= 57
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
        sp_3072_from_mp(m, 54, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_3072_sqr_54(r, a);
                err = sp_3072_mod_54(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_3072_mul_54(r, a, r);
                err = sp_3072_mod_54(r, r, m);
            }
        }
        else {
            sp_digit* norm = r;
            int i;
            sp_digit mp;

            sp_3072_mont_setup(m, &mp);
            sp_3072_mont_norm_54(norm, m);

            if (err == MP_OKAY) {
                sp_3072_mul_54(a, a, norm);
                err = sp_3072_mod_54(a, a, m);
            }

            if (err == MP_OKAY) {
                for (i=56; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 108);
                for (i--; i>=0; i--) {
                    sp_3072_mont_sqr_54(r, r, m, mp);

                    if (((e[0] >> i) & 1) == 1)
                        sp_3072_mont_mul_54(r, r, a, m, mp);
                }
                sp_3072_mont_reduce_54(r, m, mp);
                mp = sp_3072_cmp_54(r, m);
                sp_3072_cond_sub_54(r, r, m, (mp < 0) - 1);
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
#endif /* WOLFSSL_SP_SMALL */
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
#ifdef SP_RSA_PRIVATE_EXP_D
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* a;
    sp_digit* d = NULL;
    sp_digit* m;
    sp_digit* r;
    int err = MP_OKAY;

    (void)pm;
    (void)qm;
    (void)dpm;
    (void)dqm;
    (void)qim;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(dm) > 3072 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 54 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = d + 54;
        m = a + 54;
        r = a;

        sp_3072_from_bin(a, 54, in, inLen);
        sp_3072_from_mp(d, 54, dm);
        sp_3072_from_mp(m, 54, mm);
        err = sp_3072_mod_exp_54(r, a, d, 3072, m, 0);
    }
    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (d != NULL) {
        XMEMSET(d, 0, sizeof(sp_digit) * 54);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[108], d[54], m[54];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)pm;
    (void)qm;
    (void)dpm;
    (void)dqm;
    (void)qim;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (mp_count_bits(dm) > 3072 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 54, in, inLen);
        sp_3072_from_mp(d, 54, dm);
        sp_3072_from_mp(m, 54, mm);
        err = sp_3072_mod_exp_54(r, a, d, 3072, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    XMEMSET(d, 0, sizeof(sp_digit) * 54);

    return err;
#endif /* WOLFSSL_SP_SMALL || defined(WOLFSSL_SMALL_STACK) */
#else
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* t = NULL;
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
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 384 || mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 27 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 54 * 2;
        q = p + 27;
        qi = dq = dp = q + 27;
        tmpa = qi + 27;
        tmpb = tmpa + 54;

        tmp = t;
        r = tmp + 54;

        sp_3072_from_bin(a, 54, in, inLen);
        sp_3072_from_mp(p, 27, pm);
        sp_3072_from_mp(q, 27, qm);
        sp_3072_from_mp(dp, 27, dpm);
        err = sp_3072_mod_exp_27(tmpa, a, dp, 1536, p, 1);
    }
    if (err == MP_OKAY) {
        sp_3072_from_mp(dq, 27, dqm);
        err = sp_3072_mod_exp_27(tmpb, a, dq, 1536, q, 1);
    }
    if (err == MP_OKAY) {
        sp_3072_sub_27(tmpa, tmpa, tmpb);
        sp_3072_mask_27(tmp, p, tmpa[26] >> 63);
        sp_3072_add_27(tmpa, tmpa, tmp);

        sp_3072_from_mp(qi, 27, qim);
        sp_3072_mul_27(tmpa, tmpa, qi);
        err = sp_3072_mod_27(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_3072_mul_27(tmpa, q, tmpa);
        sp_3072_add_54(r, tmpb, tmpa);
        sp_3072_norm_54(r);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 27 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[54 * 2];
    sp_digit p[27], q[27], dp[27], dq[27], qi[27];
    sp_digit tmp[54], tmpa[54], tmpb[54];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 384 || mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 54, in, inLen);
        sp_3072_from_mp(p, 27, pm);
        sp_3072_from_mp(q, 27, qm);
        sp_3072_from_mp(dp, 27, dpm);
        sp_3072_from_mp(dq, 27, dqm);
        sp_3072_from_mp(qi, 27, qim);

        err = sp_3072_mod_exp_27(tmpa, a, dp, 1536, p, 1);
    }
    if (err == MP_OKAY)
        err = sp_3072_mod_exp_27(tmpb, a, dq, 1536, q, 1);

    if (err == MP_OKAY) {
        sp_3072_sub_27(tmpa, tmpa, tmpb);
        sp_3072_mask_27(tmp, p, tmpa[26] >> 63);
        sp_3072_add_27(tmpa, tmpa, tmp);
        sp_3072_mul_27(tmpa, tmpa, qi);
        err = sp_3072_mod_27(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_3072_mul_27(tmpa, tmpa, q);
        sp_3072_add_54(r, tmpb, tmpa);
        sp_3072_norm_54(r);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    XMEMSET(tmpa, 0, sizeof(tmpa));
    XMEMSET(tmpb, 0, sizeof(tmpb));
    XMEMSET(p, 0, sizeof(p));
    XMEMSET(q, 0, sizeof(q));
    XMEMSET(dp, 0, sizeof(dp));
    XMEMSET(dq, 0, sizeof(dq));
    XMEMSET(qi, 0, sizeof(qi));

    return err;
#endif /* WOLFSSL_SP_SMALL || defined(WOLFSSL_SMALL_STACK) */
#endif /* SP_RSA_PRIVATE_EXP_D */
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
#if DIGIT_BIT == 57
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 54);
        r->used = 54;
        mp_clamp(r);
#elif DIGIT_BIT < 57
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 54; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 57) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 57 - s;
        }
        r->used = (3072 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 54; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 57 >= DIGIT_BIT) {
    #if DIGIT_BIT < 64
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 57 - s;
            }
            else
                s += 57;
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
#ifdef WOLFSSL_SP_SMALL
    int err = MP_OKAY;
    sp_digit* d = NULL;
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 3072 || expBits > 3072 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 54 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 54 * 2;
        m = e + 54;
        r = b;

        sp_3072_from_mp(b, 54, base);
        sp_3072_from_mp(e, 54, exp);
        sp_3072_from_mp(m, 54, mod);

        err = sp_3072_mod_exp_54(r, b, e, mp_count_bits(exp), m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_3072_to_mp(r, res);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 54);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[108], ed[54], md[54];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    int err = MP_OKAY;
    int expBits = mp_count_bits(exp);

    if (mp_count_bits(base) > 3072 || expBits > 3072 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 54 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 54 * 2;
        m = e + 54;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 54, base);
        sp_3072_from_mp(e, 54, exp);
        sp_3072_from_mp(m, 54, mod);

        err = sp_3072_mod_exp_54(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_3072_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 54);

#ifdef WOLFSSL_SMALL_STACK
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
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
#ifdef WOLFSSL_SP_SMALL
    int err = MP_OKAY;
    sp_digit* d = NULL;
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    word32 i;

    if (mp_count_bits(base) > 3072 || expLen > 384 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 54 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 54 * 2;
        m = e + 54;
        r = b;

        sp_3072_from_mp(b, 54, base);
        sp_3072_from_bin(e, 54, exp, expLen);
        sp_3072_from_mp(m, 54, mod);

        err = sp_3072_mod_exp_54(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
        for (i=0; i<384 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 54);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[108], ed[54], md[54];
#else
    sp_digit* d = NULL;
#endif
    sp_digit* b;
    sp_digit* e;
    sp_digit* m;
    sp_digit* r;
    word32 i;
    int err = MP_OKAY;

    if (mp_count_bits(base) > 3072 || expLen > 384 ||
                                                   mp_count_bits(mod) != 3072) {
        err = MP_READ_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(*d) * 54 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 54 * 2;
        m = e + 54;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 54, base);
        sp_3072_from_bin(e, 54, exp, expLen);
        sp_3072_from_mp(m, 54, mod);

        err = sp_3072_mod_exp_54(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
        for (i=0; i<384 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 54);

#ifdef WOLFSSL_SMALL_STACK
    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#endif
}

#endif /* WOLFSSL_HAVE_SP_DH */

#endif /* WOLFSSL_SP_NO_3072 */

#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH */
#ifdef WOLFSSL_HAVE_SP_ECC
#ifndef WOLFSSL_SP_NO_256

/* Point structure to use. */
typedef struct sp_point {
    sp_digit x[2 * 5];
    sp_digit y[2 * 5];
    sp_digit z[2 * 5];
    int infinity;
} sp_point;

/* The modulus (prime) of the curve P256. */
static sp_digit p256_mod[5] = {
    0xfffffffffffffl,0x00fffffffffffl,0x0000000000000l,0x0001000000000l,
    0x0ffffffff0000l
};
#ifndef WOLFSSL_SP_SMALL
/* The Montogmery normalizer for modulus of the curve P256. */
static sp_digit p256_norm_mod[5] = {
    0x0000000000001l,0xff00000000000l,0xfffffffffffffl,0xfffefffffffffl,
    0x000000000ffffl
};
#endif /* WOLFSSL_SP_SMALL */
/* The Montogmery multiplier for modulus of the curve P256. */
static sp_digit p256_mp_mod = 0x0000000000001;
#if defined(WOLFSSL_VALIDATE_ECC_KEYGEN) || defined(HAVE_ECC_SIGN) || \
                                            defined(HAVE_ECC_VERIFY)
/* The order of the curve P256. */
static sp_digit p256_order[5] = {
    0x9cac2fc632551l,0xada7179e84f3bl,0xfffffffbce6fal,0x0000fffffffffl,
    0x0ffffffff0000l
};
#endif
/* The order of the curve P256 minus 2. */
static sp_digit p256_order2[5] = {
    0x9cac2fc63254fl,0xada7179e84f3bl,0xfffffffbce6fal,0x0000fffffffffl,
    0x0ffffffff0000l
};
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery normalizer for order of the curve P256. */
static sp_digit p256_norm_order[5] = {
    0x6353d039cdaafl,0x5258e8617b0c4l,0x0000000431905l,0xffff000000000l,
    0x000000000ffffl
};
#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery multiplier for order of the curve P256. */
static sp_digit p256_mp_order = 0x1c8aaee00bc4fl;
#endif
/* The base point of curve P256. */
static sp_point p256_base = {
    /* X ordinate */
    {
        0x13945d898c296l,0x812deb33a0f4al,0x3a440f277037dl,0x4247f8bce6e56l,
        0x06b17d1f2e12cl
    },
    /* Y ordinate */
    {
        0x6406837bf51f5l,0x576b315ececbbl,0xc0f9e162bce33l,0x7f9b8ee7eb4a7l,
        0x04fe342e2fe1al
    },
    /* Z ordinate */
    {
        0x0000000000001l,0x0000000000000l,0x0000000000000l,0x0000000000000l,
        0x0000000000000l
    },
    /* infinity */
    0
};
#if defined(HAVE_ECC_CHECK_KEY) || defined(HAVE_COMP_KEY)
static sp_digit p256_b[5] = {
    0xe3c3e27d2604bl,0xb0cc53b0f63bcl,0x69886bc651d06l,0x93e7b3ebbd557l,
    0x05ac635d8aa3al
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
 * returns MEMORY_E when memory allocation fails and MP_OKAY otherwise.
 */
static int sp_256_mod_mul_norm_5(sp_digit* r, sp_digit* a, sp_digit* m)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    int64_t* td;
#else
    int64_t td[8];
    int64_t a32d[8];
#endif
    int64_t* t;
    int64_t* a32;
    int64_t o;
    int err = MP_OKAY;

    (void)m;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(int64_t) * 2 * 8, NULL, DYNAMIC_TYPE_ECC);
    if (td != NULL) {
        t = td;
        a32 = td + 8;
    }
    else
        err = MEMORY_E;
#else
    t = td;
    a32 = a32d;
#endif

    if (err == MP_OKAY) {
        a32[0] = (sp_digit)(a[0]) & 0xffffffff;
        a32[1] = (sp_digit)(a[0] >> 32);
        a32[1] |= a[1] << 20;
        a32[1] &= 0xffffffff;
        a32[2] = (sp_digit)(a[1] >> 12) & 0xffffffff;
        a32[3] = (sp_digit)(a[1] >> 44);
        a32[3] |= a[2] << 8;
        a32[3] &= 0xffffffff;
        a32[4] = (sp_digit)(a[2] >> 24);
        a32[4] |= a[3] << 28;
        a32[4] &= 0xffffffff;
        a32[5] = (sp_digit)(a[3] >> 4) & 0xffffffff;
        a32[6] = (sp_digit)(a[3] >> 36);
        a32[6] |= a[4] << 16;
        a32[6] &= 0xffffffff;
        a32[7] = (sp_digit)(a[4] >> 16) & 0xffffffff;

        /*  1  1  0 -1 -1 -1 -1  0 */
        t[0] = 0 + a32[0] + a32[1] - a32[3] - a32[4] - a32[5] - a32[6];
        /*  0  1  1  0 -1 -1 -1 -1 */
        t[1] = 0 + a32[1] + a32[2] - a32[4] - a32[5] - a32[6] - a32[7];
        /*  0  0  1  1  0 -1 -1 -1 */
        t[2] = 0 + a32[2] + a32[3] - a32[5] - a32[6] - a32[7];
        /* -1 -1  0  2  2  1  0 -1 */
        t[3] = 0 - a32[0] - a32[1] + 2 * a32[3] + 2 * a32[4] + a32[5] - a32[7];
        /*  0 -1 -1  0  2  2  1  0 */
        t[4] = 0 - a32[1] - a32[2] + 2 * a32[4] + 2 * a32[5] + a32[6];
        /*  0  0 -1 -1  0  2  2  1 */
        t[5] = 0 - a32[2] - a32[3] + 2 * a32[5] + 2 * a32[6] + a32[7];
        /* -1 -1  0  0  0  1  3  2 */
        t[6] = 0 - a32[0] - a32[1] + a32[5] + 3 * a32[6] + 2 * a32[7];
        /*  1  0 -1 -1 -1 -1  0  3 */
        t[7] = 0 + a32[0] - a32[2] - a32[3] - a32[4] - a32[5] + 3 * a32[7];

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
        r[0] |= t[1] << 32;
        r[0] &= 0xfffffffffffffl;
        r[1] = (sp_digit)(t[1] >> 20);
        r[1] |= t[2] << 12;
        r[1] |= t[3] << 44;
        r[1] &= 0xfffffffffffffl;
        r[2] = (sp_digit)(t[3] >> 8);
        r[2] |= t[4] << 24;
        r[2] &= 0xfffffffffffffl;
        r[3] = (sp_digit)(t[4] >> 28);
        r[3] |= t[5] << 4;
        r[3] |= t[6] << 36;
        r[3] &= 0xfffffffffffffl;
        r[4] = (sp_digit)(t[6] >> 16);
        r[4] |= t[7] << 16;
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_ECC);
#endif

    return err;
}

/* Convert an mp_int to an array of sp_digit.
 *
 * r  A single precision integer.
 * a  A multi-precision integer.
 */
static void sp_256_from_mp(sp_digit* r, int max, mp_int* a)
{
#if DIGIT_BIT == 52
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 52
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0xfffffffffffffl;
        s = 52 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 52 <= DIGIT_BIT) {
            s += 52;
            r[j] &= 0xfffffffffffffl;
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
        if (s + DIGIT_BIT >= 52) {
            r[j] &= 0xfffffffffffffl;
            if (j + 1 >= max)
                break;
            s = 52 - s;
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
static void sp_256_point_from_ecc_point_5(sp_point* p, ecc_point* pm)
{
    XMEMSET(p->x, 0, sizeof(p->x));
    XMEMSET(p->y, 0, sizeof(p->y));
    XMEMSET(p->z, 0, sizeof(p->z));
    sp_256_from_mp(p->x, 5, pm->x);
    sp_256_from_mp(p->y, 5, pm->y);
    sp_256_from_mp(p->z, 5, pm->z);
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
#if DIGIT_BIT == 52
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 5);
        r->used = 5;
        mp_clamp(r);
#elif DIGIT_BIT < 52
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 5; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 52) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 52 - s;
        }
        r->used = (256 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 5; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 52 >= DIGIT_BIT) {
    #if DIGIT_BIT < 64
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 52 - s;
            }
            else
                s += 52;
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
static int sp_256_point_to_ecc_point_5(sp_point* p, ecc_point* pm)
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
static sp_digit sp_256_cmp_5(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=4; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    r |= (a[ 4] - b[ 4]) & (0 - !r);
    r |= (a[ 3] - b[ 3]) & (0 - !r);
    r |= (a[ 2] - b[ 2]) & (0 - !r);
    r |= (a[ 1] - b[ 1]) & (0 - !r);
    r |= (a[ 0] - b[ 0]) & (0 - !r);
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Normalize the values in each word to 52.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_256_norm_5(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 4; i++) {
        a[i+1] += a[i] >> 52;
        a[i] &= 0xfffffffffffffl;
    }
#else
    a[1] += a[0] >> 52; a[0] &= 0xfffffffffffffl;
    a[2] += a[1] >> 52; a[1] &= 0xfffffffffffffl;
    a[3] += a[2] >> 52; a[2] &= 0xfffffffffffffl;
    a[4] += a[3] >> 52; a[3] &= 0xfffffffffffffl;
#endif
}

/* Conditionally subtract b from a using the mask m.
 * m is -1 to subtract and 0 when not.
 *
 * r  A single precision number representing condition subtract result.
 * a  A single precision number to subtract from.
 * b  A single precision number to subtract.
 * m  Mask value to apply.
 */
static void sp_256_cond_sub_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 5; i++)
        r[i] = a[i] - (b[i] & m);
#else
    r[ 0] = a[ 0] - (b[ 0] & m);
    r[ 1] = a[ 1] - (b[ 1] & m);
    r[ 2] = a[ 2] - (b[ 2] & m);
    r[ 3] = a[ 3] - (b[ 3] & m);
    r[ 4] = a[ 4] - (b[ 4] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_256_mul_add_5(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 5; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0xfffffffffffffl;
        t >>= 52;
    }
    r[5] += t;
#else
    int128_t tb = b;
    int128_t t[5];

    t[ 0] = tb * a[ 0];
    t[ 1] = tb * a[ 1];
    t[ 2] = tb * a[ 2];
    t[ 3] = tb * a[ 3];
    t[ 4] = tb * a[ 4];
    r[ 0] +=                 (t[ 0] & 0xfffffffffffffl);
    r[ 1] += (t[ 0] >> 52) + (t[ 1] & 0xfffffffffffffl);
    r[ 2] += (t[ 1] >> 52) + (t[ 2] & 0xfffffffffffffl);
    r[ 3] += (t[ 2] >> 52) + (t[ 3] & 0xfffffffffffffl);
    r[ 4] += (t[ 3] >> 52) + (t[ 4] & 0xfffffffffffffl);
    r[ 5] +=  t[ 4] >> 52;
#endif /* WOLFSSL_SP_SMALL */
}

/* Shift the result in the high 256 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_256_mont_shift_5(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    word64 n;

    n = a[4] >> 48;
    for (i = 0; i < 4; i++) {
        n += a[5 + i] << 4;
        r[i] = n & 0xfffffffffffffl;
        n >>= 52;
    }
    n += a[9] << 4;
    r[4] = n;
#else
    word64 n;

    n  = a[4] >> 48;
    n += a[ 5] << 4; r[ 0] = n & 0xfffffffffffffl; n >>= 52;
    n += a[ 6] << 4; r[ 1] = n & 0xfffffffffffffl; n >>= 52;
    n += a[ 7] << 4; r[ 2] = n & 0xfffffffffffffl; n >>= 52;
    n += a[ 8] << 4; r[ 3] = n & 0xfffffffffffffl; n >>= 52;
    n += a[ 9] << 4; r[ 4] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[5], 0, sizeof(*r) * 5);
}

/* Reduce the number back to 256 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_256_mont_reduce_5(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<4; i++) {
            mu = (a[i] * mp) & 0xfffffffffffffl;
            sp_256_mul_add_5(a+i, m, mu);
            a[i+1] += a[i] >> 52;
        }
        mu = (a[i] * mp) & 0xffffffffffffl;
        sp_256_mul_add_5(a+i, m, mu);
        a[i+1] += a[i] >> 52;
        a[i] &= 0xfffffffffffffl;
    }
    else {
        for (i=0; i<4; i++) {
            mu = a[i] & 0xfffffffffffffl;
            sp_256_mul_add_5(a+i, p256_mod, mu);
            a[i+1] += a[i] >> 52;
        }
        mu = a[i] & 0xffffffffffffl;
        sp_256_mul_add_5(a+i, p256_mod, mu);
        a[i+1] += a[i] >> 52;
        a[i] &= 0xfffffffffffffl;
    }

    sp_256_mont_shift_5(a, a);
    sp_256_cond_sub_5(a, a, m, 0 - ((a[4] >> 48) > 0));
    sp_256_norm_5(a);
}

#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_256_mul_5(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[4]) * b[4];
    r[9] = (sp_digit)(c >> 52);
    c = (c & 0xfffffffffffffl) << 52;
    for (k = 7; k >= 0; k--) {
        for (i = 4; i >= 0; i--) {
            j = k - i;
            if (j >= 5)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 104;
        r[k + 1] = (c >> 52) & 0xfffffffffffffl;
        c = (c & 0xfffffffffffffl) << 52;
    }
    r[0] = (sp_digit)(c >> 52);
}

#else
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_256_mul_5(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int128_t t0   = ((int128_t)a[ 0]) * b[ 0];
    int128_t t1   = ((int128_t)a[ 0]) * b[ 1]
                 + ((int128_t)a[ 1]) * b[ 0];
    int128_t t2   = ((int128_t)a[ 0]) * b[ 2]
                 + ((int128_t)a[ 1]) * b[ 1]
                 + ((int128_t)a[ 2]) * b[ 0];
    int128_t t3   = ((int128_t)a[ 0]) * b[ 3]
                 + ((int128_t)a[ 1]) * b[ 2]
                 + ((int128_t)a[ 2]) * b[ 1]
                 + ((int128_t)a[ 3]) * b[ 0];
    int128_t t4   = ((int128_t)a[ 0]) * b[ 4]
                 + ((int128_t)a[ 1]) * b[ 3]
                 + ((int128_t)a[ 2]) * b[ 2]
                 + ((int128_t)a[ 3]) * b[ 1]
                 + ((int128_t)a[ 4]) * b[ 0];
    int128_t t5   = ((int128_t)a[ 1]) * b[ 4]
                 + ((int128_t)a[ 2]) * b[ 3]
                 + ((int128_t)a[ 3]) * b[ 2]
                 + ((int128_t)a[ 4]) * b[ 1];
    int128_t t6   = ((int128_t)a[ 2]) * b[ 4]
                 + ((int128_t)a[ 3]) * b[ 3]
                 + ((int128_t)a[ 4]) * b[ 2];
    int128_t t7   = ((int128_t)a[ 3]) * b[ 4]
                 + ((int128_t)a[ 4]) * b[ 3];
    int128_t t8   = ((int128_t)a[ 4]) * b[ 4];

    t1   += t0  >> 52; r[ 0] = t0  & 0xfffffffffffffl;
    t2   += t1  >> 52; r[ 1] = t1  & 0xfffffffffffffl;
    t3   += t2  >> 52; r[ 2] = t2  & 0xfffffffffffffl;
    t4   += t3  >> 52; r[ 3] = t3  & 0xfffffffffffffl;
    t5   += t4  >> 52; r[ 4] = t4  & 0xfffffffffffffl;
    t6   += t5  >> 52; r[ 5] = t5  & 0xfffffffffffffl;
    t7   += t6  >> 52; r[ 6] = t6  & 0xfffffffffffffl;
    t8   += t7  >> 52; r[ 7] = t7  & 0xfffffffffffffl;
    r[9] = (sp_digit)(t8 >> 52);
                       r[8] = t8 & 0xfffffffffffffl;
}

#endif /* WOLFSSL_SP_SMALL */
/* Multiply two Montogmery form numbers mod the modulus (prime).
 * (r = a * b mod m)
 *
 * r   Result of multiplication.
 * a   First number to multiply in Montogmery form.
 * b   Second number to multiply in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_256_mont_mul_5(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_256_mul_5(r, a, b);
    sp_256_mont_reduce_5(r, m, mp);
}

#ifdef WOLFSSL_SP_SMALL
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_sqr_5(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int128_t c;

    c = ((int128_t)a[4]) * a[4];
    r[9] = (sp_digit)(c >> 52);
    c = (c & 0xfffffffffffffl) << 52;
    for (k = 7; k >= 0; k--) {
        for (i = 4; i >= 0; i--) {
            j = k - i;
            if (j >= 5 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int128_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int128_t)a[i]) * a[i];

        r[k + 2] += c >> 104;
        r[k + 1] = (c >> 52) & 0xfffffffffffffl;
        c = (c & 0xfffffffffffffl) << 52;
    }
    r[0] = (sp_digit)(c >> 52);
}

#else
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_sqr_5(sp_digit* r, const sp_digit* a)
{
    int128_t t0   =  ((int128_t)a[ 0]) * a[ 0];
    int128_t t1   = (((int128_t)a[ 0]) * a[ 1]) * 2;
    int128_t t2   = (((int128_t)a[ 0]) * a[ 2]) * 2
                 +  ((int128_t)a[ 1]) * a[ 1];
    int128_t t3   = (((int128_t)a[ 0]) * a[ 3]
                 +  ((int128_t)a[ 1]) * a[ 2]) * 2;
    int128_t t4   = (((int128_t)a[ 0]) * a[ 4]
                 +  ((int128_t)a[ 1]) * a[ 3]) * 2
                 +  ((int128_t)a[ 2]) * a[ 2];
    int128_t t5   = (((int128_t)a[ 1]) * a[ 4]
                 +  ((int128_t)a[ 2]) * a[ 3]) * 2;
    int128_t t6   = (((int128_t)a[ 2]) * a[ 4]) * 2
                 +  ((int128_t)a[ 3]) * a[ 3];
    int128_t t7   = (((int128_t)a[ 3]) * a[ 4]) * 2;
    int128_t t8   =  ((int128_t)a[ 4]) * a[ 4];

    t1   += t0  >> 52; r[ 0] = t0  & 0xfffffffffffffl;
    t2   += t1  >> 52; r[ 1] = t1  & 0xfffffffffffffl;
    t3   += t2  >> 52; r[ 2] = t2  & 0xfffffffffffffl;
    t4   += t3  >> 52; r[ 3] = t3  & 0xfffffffffffffl;
    t5   += t4  >> 52; r[ 4] = t4  & 0xfffffffffffffl;
    t6   += t5  >> 52; r[ 5] = t5  & 0xfffffffffffffl;
    t7   += t6  >> 52; r[ 6] = t6  & 0xfffffffffffffl;
    t8   += t7  >> 52; r[ 7] = t7  & 0xfffffffffffffl;
    r[9] = (sp_digit)(t8 >> 52);
                       r[8] = t8 & 0xfffffffffffffl;
}

#endif /* WOLFSSL_SP_SMALL */
/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_256_mont_sqr_5(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_256_sqr_5(r, a);
    sp_256_mont_reduce_5(r, m, mp);
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
static void sp_256_mont_sqr_n_5(sp_digit* r, sp_digit* a, int n,
        sp_digit* m, sp_digit mp)
{
    sp_256_mont_sqr_5(r, a, m, mp);
    for (; n > 1; n--)
        sp_256_mont_sqr_5(r, r, m, mp);
}

#else
/* Mod-2 for the P256 curve. */
static const uint64_t p256_mod_2[4] = {
    0xfffffffffffffffd,0x00000000ffffffff,0x0000000000000000,
    0xffffffff00000001
};
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the modulus (prime) of the
 * P256 curve. (r = 1 / a mod m)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_5(sp_digit* r, sp_digit* a, sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 5);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_5(t, t, p256_mod, p256_mp_mod);
        if (p256_mod_2[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_5(t, t, a, p256_mod, p256_mp_mod);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 5);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 5;
    sp_digit* t3 = td + 4 * 5;

    /* t = a^2 */
    sp_256_mont_sqr_5(t, a, p256_mod, p256_mp_mod);
    /* t = a^3 = t * a */
    sp_256_mont_mul_5(t, t, a, p256_mod, p256_mp_mod);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_5(t2, t, 2, p256_mod, p256_mp_mod);
    /* t3= a^d = t2 * a */
    sp_256_mont_mul_5(t3, t2, a, p256_mod, p256_mp_mod);
    /* t = a^f = t2 * t */
    sp_256_mont_mul_5(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^f0 = t ^ 2 ^ 4 */
    sp_256_mont_sqr_n_5(t2, t, 4, p256_mod, p256_mp_mod);
    /* t3= a^fd = t2 * t3 */
    sp_256_mont_mul_5(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ff = t2 * t */
    sp_256_mont_mul_5(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_5(t2, t, 8, p256_mod, p256_mp_mod);
    /* t3= a^fffd = t2 * t3 */
    sp_256_mont_mul_5(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_5(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_5(t2, t, 16, p256_mod, p256_mp_mod);
    /* t3= a^fffffffd = t2 * t3 */
    sp_256_mont_mul_5(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_5(t, t2, t, p256_mod, p256_mp_mod);
    /* t = a^ffffffff00000000 = t ^ 2 ^ 32  */
    sp_256_mont_sqr_n_5(t2, t, 32, p256_mod, p256_mp_mod);
    /* t2= a^ffffffffffffffff = t2 * t */
    sp_256_mont_mul_5(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001 = t2 * a */
    sp_256_mont_mul_5(t2, t2, a, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff000000010000000000000000000000000000000000000000
     *   = t2 ^ 2 ^ 160 */
    sp_256_mont_sqr_n_5(t2, t2, 160, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff
     *   = t2 * t */
    sp_256_mont_mul_5(t2, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff00000000
     *   = t2 ^ 2 ^ 32 */
    sp_256_mont_sqr_n_5(t2, t2, 32, p256_mod, p256_mp_mod);
    /* r = a^ffffffff00000001000000000000000000000000fffffffffffffffffffffffd
     *   = t2 * t3 */
    sp_256_mont_mul_5(r, t2, t3, p256_mod, p256_mp_mod);
#endif /* WOLFSSL_SP_SMALL */
}

/* Map the Montgomery form projective co-ordinate point to an affine point.
 *
 * r  Resulting affine co-ordinate point.
 * p  Montgomery form projective co-ordinate point.
 * t  Temporary ordinate data.
 */
static void sp_256_map_5(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*5;
    int64_t n;

    sp_256_mont_inv_5(t1, p->z, t + 2*5);

    sp_256_mont_sqr_5(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_5(t1, t2, t1, p256_mod, p256_mp_mod);

    /* x /= z^2 */
    sp_256_mont_mul_5(r->x, p->x, t2, p256_mod, p256_mp_mod);
    XMEMSET(r->x + 5, 0, sizeof(r->x) / 2);
    sp_256_mont_reduce_5(r->x, p256_mod, p256_mp_mod);
    /* Reduce x to less than modulus */
    n = sp_256_cmp_5(r->x, p256_mod);
    sp_256_cond_sub_5(r->x, r->x, p256_mod, 0 - (n >= 0));
    sp_256_norm_5(r->x);

    /* y /= z^3 */
    sp_256_mont_mul_5(r->y, p->y, t1, p256_mod, p256_mp_mod);
    XMEMSET(r->y + 5, 0, sizeof(r->y) / 2);
    sp_256_mont_reduce_5(r->y, p256_mod, p256_mp_mod);
    /* Reduce y to less than modulus */
    n = sp_256_cmp_5(r->y, p256_mod);
    sp_256_cond_sub_5(r->y, r->y, p256_mod, 0 - (n >= 0));
    sp_256_norm_5(r->y);

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
SP_NOINLINE static int sp_256_add_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 5; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#else
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_256_add_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    r[ 0] = a[ 0] + b[ 0];
    r[ 1] = a[ 1] + b[ 1];
    r[ 2] = a[ 2] + b[ 2];
    r[ 3] = a[ 3] + b[ 3];
    r[ 4] = a[ 4] + b[ 4];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
/* Add two Montgomery form numbers (r = a + b % m).
 *
 * r   Result of addition.
 * a   First number to add in Montogmery form.
 * b   Second number to add in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_add_5(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    sp_256_add_5(r, a, b);
    sp_256_norm_5(r);
    sp_256_cond_sub_5(r, r, m, 0 - ((r[4] >> 48) > 0));
    sp_256_norm_5(r);
}

/* Double a Montgomery form number (r = a + a % m).
 *
 * r   Result of doubling.
 * a   Number to double in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_dbl_5(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_add_5(r, a, a);
    sp_256_norm_5(r);
    sp_256_cond_sub_5(r, r, m, 0 - ((r[4] >> 48) > 0));
    sp_256_norm_5(r);
}

/* Triple a Montgomery form number (r = a + a + a % m).
 *
 * r   Result of Tripling.
 * a   Number to triple in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_tpl_5(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_add_5(r, a, a);
    sp_256_norm_5(r);
    sp_256_cond_sub_5(r, r, m, 0 - ((r[4] >> 48) > 0));
    sp_256_norm_5(r);
    sp_256_add_5(r, r, a);
    sp_256_norm_5(r);
    sp_256_cond_sub_5(r, r, m, 0 - ((r[4] >> 48) > 0));
    sp_256_norm_5(r);
}

#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_256_sub_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 5; i++)
        r[i] = a[i] - b[i];

    return 0;
}

#else
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_256_sub_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    r[ 0] = a[ 0] - b[ 0];
    r[ 1] = a[ 1] - b[ 1];
    r[ 2] = a[ 2] - b[ 2];
    r[ 3] = a[ 3] - b[ 3];
    r[ 4] = a[ 4] - b[ 4];

    return 0;
}

#endif /* WOLFSSL_SP_SMALL */
/* Conditionally add a and b using the mask m.
 * m is -1 to add and 0 when not.
 *
 * r  A single precision number representing conditional add result.
 * a  A single precision number to add with.
 * b  A single precision number to add.
 * m  Mask value to apply.
 */
static void sp_256_cond_add_5(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 5; i++)
        r[i] = a[i] + (b[i] & m);
#else
    r[ 0] = a[ 0] + (b[ 0] & m);
    r[ 1] = a[ 1] + (b[ 1] & m);
    r[ 2] = a[ 2] + (b[ 2] & m);
    r[ 3] = a[ 3] + (b[ 3] & m);
    r[ 4] = a[ 4] + (b[ 4] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Subtract two Montgomery form numbers (r = a - b % m).
 *
 * r   Result of subtration.
 * a   Number to subtract from in Montogmery form.
 * b   Number to subtract with in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_sub_5(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    sp_256_sub_5(r, a, b);
    sp_256_cond_add_5(r, r, m, r[4] >> 48);
    sp_256_norm_5(r);
}

/* Shift number left one bit.
 * Bottom bit is lost.
 *
 * r  Result of shift.
 * a  Number to shift.
 */
SP_NOINLINE static void sp_256_rshift1_5(sp_digit* r, sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<4; i++)
        r[i] = ((a[i] >> 1) | (a[i + 1] << 51)) & 0xfffffffffffffl;
#else
    r[0] = ((a[0] >> 1) | (a[1] << 51)) & 0xfffffffffffffl;
    r[1] = ((a[1] >> 1) | (a[2] << 51)) & 0xfffffffffffffl;
    r[2] = ((a[2] >> 1) | (a[3] << 51)) & 0xfffffffffffffl;
    r[3] = ((a[3] >> 1) | (a[4] << 51)) & 0xfffffffffffffl;
#endif
    r[4] = a[4] >> 1;
}

/* Divide the number by 2 mod the modulus (prime). (r = a / 2 % m)
 *
 * r  Result of division by 2.
 * a  Number to divide.
 * m  Modulus (prime).
 */
static void sp_256_div2_5(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_cond_add_5(r, a, m, 0 - (a[0] & 1));
    sp_256_norm_5(r);
    sp_256_rshift1_5(r, r);
}

/* Double the Montgomery form projective point p.
 *
 * r  Result of doubling point.
 * p  Point to double.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_dbl_5(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*5;
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
        for (i=0; i<5; i++)
            r->x[i] = p->x[i];
        for (i=0; i<5; i++)
            r->y[i] = p->y[i];
        for (i=0; i<5; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* T1 = Z * Z */
    sp_256_mont_sqr_5(t1, z, p256_mod, p256_mp_mod);
    /* Z = Y * Z */
    sp_256_mont_mul_5(z, y, z, p256_mod, p256_mp_mod);
    /* Z = 2Z */
    sp_256_mont_dbl_5(z, z, p256_mod);
    /* T2 = X - T1 */
    sp_256_mont_sub_5(t2, x, t1, p256_mod);
    /* T1 = X + T1 */
    sp_256_mont_add_5(t1, x, t1, p256_mod);
    /* T2 = T1 * T2 */
    sp_256_mont_mul_5(t2, t1, t2, p256_mod, p256_mp_mod);
    /* T1 = 3T2 */
    sp_256_mont_tpl_5(t1, t2, p256_mod);
    /* Y = 2Y */
    sp_256_mont_dbl_5(y, y, p256_mod);
    /* Y = Y * Y */
    sp_256_mont_sqr_5(y, y, p256_mod, p256_mp_mod);
    /* T2 = Y * Y */
    sp_256_mont_sqr_5(t2, y, p256_mod, p256_mp_mod);
    /* T2 = T2/2 */
    sp_256_div2_5(t2, t2, p256_mod);
    /* Y = Y * X */
    sp_256_mont_mul_5(y, y, x, p256_mod, p256_mp_mod);
    /* X = T1 * T1 */
    sp_256_mont_mul_5(x, t1, t1, p256_mod, p256_mp_mod);
    /* X = X - Y */
    sp_256_mont_sub_5(x, x, y, p256_mod);
    /* X = X - Y */
    sp_256_mont_sub_5(x, x, y, p256_mod);
    /* Y = Y - X */
    sp_256_mont_sub_5(y, y, x, p256_mod);
    /* Y = Y * T1 */
    sp_256_mont_mul_5(y, y, t1, p256_mod, p256_mp_mod);
    /* Y = Y - T2 */
    sp_256_mont_sub_5(y, y, t2, p256_mod);

}

/* Compare two numbers to determine if they are equal.
 * Constant time implementation.
 *
 * a  First number to compare.
 * b  Second number to compare.
 * returns 1 when equal and 0 otherwise.
 */
static int sp_256_cmp_equal_5(const sp_digit* a, const sp_digit* b)
{
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ b[3]) |
            (a[4] ^ b[4])) == 0;
}

/* Add two Montgomery form projective points.
 *
 * r  Result of addition.
 * p  Frist point to add.
 * q  Second point to add.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_add_5(sp_point* r, sp_point* p, sp_point* q,
        sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*5;
    sp_digit* t3 = t + 4*5;
    sp_digit* t4 = t + 6*5;
    sp_digit* t5 = t + 8*5;
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
    sp_256_sub_5(t1, p256_mod, q->y);
    sp_256_norm_5(t1);
    if (sp_256_cmp_equal_5(p->x, q->x) & sp_256_cmp_equal_5(p->z, q->z) &
        (sp_256_cmp_equal_5(p->y, q->y) | sp_256_cmp_equal_5(p->y, t1))) {
        sp_256_proj_point_dbl_5(r, p, t);
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
        for (i=0; i<5; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<5; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<5; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U1 = X1*Z2^2 */
        sp_256_mont_sqr_5(t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t3, t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t1, t1, x, p256_mod, p256_mp_mod);
        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_5(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S1 = Y1*Z2^3 */
        sp_256_mont_mul_5(t3, t3, y, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_5(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - U1 */
        sp_256_mont_sub_5(t2, t2, t1, p256_mod);
        /* R = S2 - S1 */
        sp_256_mont_sub_5(t4, t4, t3, p256_mod);
        /* Z3 = H*Z1*Z2 */
        sp_256_mont_mul_5(z, z, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*U1*H^2 */
        sp_256_mont_sqr_5(x, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_5(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(y, t1, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_5(x, x, t5, p256_mod);
        sp_256_mont_dbl_5(t1, y, p256_mod);
        sp_256_mont_sub_5(x, x, t1, p256_mod);
        /* Y3 = R*(U1*H^2 - X3) - S1*H^3 */
        sp_256_mont_sub_5(y, y, x, p256_mod);
        sp_256_mont_mul_5(y, y, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t5, t5, t3, p256_mod, p256_mp_mod);
        sp_256_mont_sub_5(y, y, t5, p256_mod);
    }
}

#ifdef WOLFSSL_SP_SMALL
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
static int sp_256_ecc_mulmod_5(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
    sp_point* td;
    sp_point* t[3];
    sp_digit* tmp;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    (void)heap;

    td = (sp_point*)XMALLOC(sizeof(sp_point) * 3, heap, DYNAMIC_TYPE_ECC);
    if (td == NULL)
        err = MEMORY_E;
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 5 * 5, heap,
                             DYNAMIC_TYPE_ECC);
    if (tmp == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3);

        t[0] = &td[0];
        t[1] = &td[1];
        t[2] = &td[2];

        /* t[0] = {0, 0, 1} * norm */
        t[0]->infinity = 1;
        /* t[1] = {g->x, g->y, g->z} * norm */
        err = sp_256_mod_mul_norm_5(t[1]->x, g->x, p256_mod);
    }
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t[1]->y, g->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t[1]->z, g->z, p256_mod);

    if (err == MP_OKAY) {
        i = 4;
        c = 48;
        n = k[i--] << (52 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = k[i--];
                c = 52;
            }

            y = (n >> 51) & 1;
            n <<= 1;

            sp_256_proj_point_add_5(t[y^1], t[0], t[1], tmp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(sp_point));
            sp_256_proj_point_dbl_5(t[2], t[2], tmp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(sp_point));
        }

        if (map)
            sp_256_map_5(r, t[0], tmp);
        else
            XMEMCPY(r, t[0], sizeof(sp_point));
    }

    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 5 * 5);
        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (td != NULL) {
        XMEMSET(td, 0, sizeof(sp_point) * 3);
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
}

#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
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
static int sp_256_ecc_mulmod_5(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td[3];
    sp_digit tmpd[2 * 5 * 5];
#endif
    sp_point* t;
    sp_digit* tmp;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

    (void)heap;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_point td[3];
    t = (sp_point*)XMALLOC(sizeof(*td) * 3, heap, DYNAMIC_TYPE_ECC);
    if (t == NULL)
        err = MEMORY_E;
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 5 * 5, heap,
                             DYNAMIC_TYPE_ECC);
    if (tmp == NULL)
        err = MEMORY_E;
#else
    t = td;
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        t[0] = &td[0];
        t[1] = &td[1];
        t[2] = &td[2];

        /* t[0] = {0, 0, 1} * norm */
        XMEMSET(&t[0], 0, sizeof(t[0]));
        t[0].infinity = 1;
        /* t[1] = {g->x, g->y, g->z} * norm */
        err = sp_256_mod_mul_norm_5(t[1].x, g->x, p256_mod);
    }
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t[1].y, g->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t[1].z, g->z, p256_mod);

    if (err == MP_OKAY) {
        i = 4;
        c = 48;
        n = k[i--] << (52 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = k[i--];
                c = 52;
            }

            y = (n >> 51) & 1;
            n <<= 1;

            sp_256_proj_point_add_5(&t[y^1], &t[0], &t[1], tmp);

            XMEMCPY(&t[2], (void*)(((size_t)&t[0] & addr_mask[y^1]) +
                                 ((size_t)&t[1] & addr_mask[y])), sizeof(t[2]));
            sp_256_proj_point_dbl_5(&t[2], &t[2], tmp);
            XMEMCPY((void*)(((size_t)&t[0] & addr_mask[y^1]) +
                           ((size_t)&t[1] & addr_mask[y])), &t[2], sizeof(t[2]));
        }

        if (map)
            sp_256_map_5(r, &t[0], tmp);
        else
            XMEMCPY(r, &t[0], sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 5 * 5);
        XFREE(tmp, heap, DYNAMIC_TYPE_ECC);
    }
    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_point) * 3);
        XFREE(t, heap, DYNAMIC_TYPE_ECC);
    }
#else
    ForceZero(tmpd, sizeof(tmpd));
    ForceZero(td, sizeof(td));
#endif

    return err;
}

#else
/* A table entry for pre-computed points. */
typedef struct sp_table_entry {
    sp_digit x[5];
    sp_digit y[5];
    byte infinity;
} sp_table_entry;

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
static int sp_256_ecc_mulmod_fast_5(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td[16];
    sp_point rtd;
    sp_digit tmpd[2 * 5 * 5];
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
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 5 * 5, heap,
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
        sp_256_mod_mul_norm_5(t[1].x, g->x, p256_mod);
        sp_256_mod_mul_norm_5(t[1].y, g->y, p256_mod);
        sp_256_mod_mul_norm_5(t[1].z, g->z, p256_mod);
        t[1].infinity = 0;
        sp_256_proj_point_dbl_5(&t[ 2], &t[ 1], tmp);
        t[ 2].infinity = 0;
        sp_256_proj_point_add_5(&t[ 3], &t[ 2], &t[ 1], tmp);
        t[ 3].infinity = 0;
        sp_256_proj_point_dbl_5(&t[ 4], &t[ 2], tmp);
        t[ 4].infinity = 0;
        sp_256_proj_point_add_5(&t[ 5], &t[ 3], &t[ 2], tmp);
        t[ 5].infinity = 0;
        sp_256_proj_point_dbl_5(&t[ 6], &t[ 3], tmp);
        t[ 6].infinity = 0;
        sp_256_proj_point_add_5(&t[ 7], &t[ 4], &t[ 3], tmp);
        t[ 7].infinity = 0;
        sp_256_proj_point_dbl_5(&t[ 8], &t[ 4], tmp);
        t[ 8].infinity = 0;
        sp_256_proj_point_add_5(&t[ 9], &t[ 5], &t[ 4], tmp);
        t[ 9].infinity = 0;
        sp_256_proj_point_dbl_5(&t[10], &t[ 5], tmp);
        t[10].infinity = 0;
        sp_256_proj_point_add_5(&t[11], &t[ 6], &t[ 5], tmp);
        t[11].infinity = 0;
        sp_256_proj_point_dbl_5(&t[12], &t[ 6], tmp);
        t[12].infinity = 0;
        sp_256_proj_point_add_5(&t[13], &t[ 7], &t[ 6], tmp);
        t[13].infinity = 0;
        sp_256_proj_point_dbl_5(&t[14], &t[ 7], tmp);
        t[14].infinity = 0;
        sp_256_proj_point_add_5(&t[15], &t[ 8], &t[ 7], tmp);
        t[15].infinity = 0;

        i = 3;
        n = k[i+1] << 12;
        c = 44;
        y = n >> 56;
        XMEMCPY(rt, &t[y], sizeof(sp_point));
        n <<= 8;
        for (; i>=0 || c>=4; ) {
            if (c < 4) {
                n |= k[i--] << (12 - c);
                c += 52;
            }
            y = (n >> 60) & 0xf;
            n <<= 4;
            c -= 4;

            sp_256_proj_point_dbl_5(rt, rt, tmp);
            sp_256_proj_point_dbl_5(rt, rt, tmp);
            sp_256_proj_point_dbl_5(rt, rt, tmp);
            sp_256_proj_point_dbl_5(rt, rt, tmp);

            sp_256_proj_point_add_5(rt, rt, &t[y], tmp);
        }

        if (map)
            sp_256_map_5(r, rt, tmp);
        else
            XMEMCPY(r, rt, sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 5 * 5);
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

#ifdef FP_ECC
/* Double the Montgomery form projective point p a number of times.
 *
 * r  Result of repeated doubling of point.
 * p  Point to double.
 * n  Number of times to double
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_dbl_n_5(sp_point* r, sp_point* p, int n,
        sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* w = t;
    sp_digit* a = t + 2*5;
    sp_digit* b = t + 4*5;
    sp_digit* t1 = t + 6*5;
    sp_digit* t2 = t + 8*5;
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
        for (i=0; i<5; i++)
            r->x[i] = p->x[i];
        for (i=0; i<5; i++)
            r->y[i] = p->y[i];
        for (i=0; i<5; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* Y = 2*Y */
    sp_256_mont_dbl_5(y, y, p256_mod);
    /* W = Z^4 */
    sp_256_mont_sqr_5(w, z, p256_mod, p256_mp_mod);
    sp_256_mont_sqr_5(w, w, p256_mod, p256_mp_mod);
    while (n--) {
        /* A = 3*(X^2 - W) */
        sp_256_mont_sqr_5(t1, x, p256_mod, p256_mp_mod);
        sp_256_mont_sub_5(t1, t1, w, p256_mod);
        sp_256_mont_tpl_5(a, t1, p256_mod);
        /* B = X*Y^2 */
        sp_256_mont_sqr_5(t2, y, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(b, t2, x, p256_mod, p256_mp_mod);
        /* X = A^2 - 2B */
        sp_256_mont_sqr_5(x, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_5(t1, b, p256_mod);
        sp_256_mont_sub_5(x, x, t1, p256_mod);
        /* Z = Z*Y */
        sp_256_mont_mul_5(z, z, y, p256_mod, p256_mp_mod);
        /* t2 = Y^4 */
        sp_256_mont_sqr_5(t2, t2, p256_mod, p256_mp_mod);
        if (n) {
            /* W = W*Y^4 */
            sp_256_mont_mul_5(w, w, t2, p256_mod, p256_mp_mod);
        }
        /* y = 2*A*(B - X) - Y^4 */
        sp_256_mont_sub_5(y, b, x, p256_mod);
        sp_256_mont_mul_5(y, y, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_5(y, y, p256_mod);
        sp_256_mont_sub_5(y, y, t2, p256_mod);
    }
    /* Y = Y/2 */
    sp_256_div2_5(y, y, p256_mod);
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
static void sp_256_proj_point_add_qz1_5(sp_point* r, sp_point* p,
        sp_point* q, sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*5;
    sp_digit* t3 = t + 4*5;
    sp_digit* t4 = t + 6*5;
    sp_digit* t5 = t + 8*5;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    /* Check double */
    sp_256_sub_5(t1, p256_mod, q->y);
    sp_256_norm_5(t1);
    if (sp_256_cmp_equal_5(p->x, q->x) & sp_256_cmp_equal_5(p->z, q->z) &
        (sp_256_cmp_equal_5(p->y, q->y) | sp_256_cmp_equal_5(p->y, t1))) {
        sp_256_proj_point_dbl_5(r, p, t);
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
        for (i=0; i<5; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<5; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<5; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_5(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_5(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - X1 */
        sp_256_mont_sub_5(t2, t2, x, p256_mod);
        /* R = S2 - Y1 */
        sp_256_mont_sub_5(t4, t4, y, p256_mod);
        /* Z3 = H*Z1 */
        sp_256_mont_mul_5(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*X1*H^2 */
        sp_256_mont_sqr_5(t1, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_5(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t3, x, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_5(x, t1, t5, p256_mod);
        sp_256_mont_dbl_5(t1, t3, p256_mod);
        sp_256_mont_sub_5(x, x, t1, p256_mod);
        /* Y3 = R*(X1*H^2 - X3) - Y1*H^3 */
        sp_256_mont_sub_5(t3, t3, x, p256_mod);
        sp_256_mont_mul_5(t3, t3, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(t5, t5, y, p256_mod, p256_mp_mod);
        sp_256_mont_sub_5(y, t3, t5, p256_mod);
    }
}

#ifdef FP_ECC
/* Convert the projective point to affine.
 * Ordinates are in Montgomery form.
 *
 * a  Point to convert.
 * t  Temprorary data.
 */
static void sp_256_proj_to_affine_5(sp_point* a, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2 * 5;
    sp_digit* tmp = t + 4 * 5;

    sp_256_mont_inv_5(t1, a->z, tmp);

    sp_256_mont_sqr_5(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_5(t1, t2, t1, p256_mod, p256_mp_mod);

    sp_256_mont_mul_5(a->x, a->x, t2, p256_mod, p256_mp_mod);
    sp_256_mont_mul_5(a->y, a->y, t1, p256_mod, p256_mp_mod);
    XMEMCPY(a->z, p256_norm_mod, sizeof(p256_norm_mod));
}

/* Generate the pre-computed table of points for the base point.
 *
 * a      The base point.
 * table  Place to store generated point data.
 * tmp    Temprorary data.
 * heap  Heap to use for allocation.
 */
static int sp_256_gen_stripe_table_5(sp_point* a,
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
        err = sp_256_mod_mul_norm_5(t->x, a->x, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t->y, a->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_5(t->z, a->z, p256_mod);
    if (err == MP_OKAY) {
        t->infinity = 0;
        sp_256_proj_to_affine_5(t, tmp);

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
            sp_256_proj_point_dbl_n_5(t, t, 32, tmp);
            sp_256_proj_to_affine_5(t, tmp);
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
                sp_256_proj_point_add_qz1_5(t, s1, s2, tmp);
                sp_256_proj_to_affine_5(t, tmp);
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
static int sp_256_ecc_mulmod_stripe_5(sp_point* r, sp_point* g,
        sp_table_entry* table, sp_digit* k, int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point rtd;
    sp_point pd;
    sp_digit td[2 * 5 * 5];
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
    t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 5 * 5, heap,
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
            y |= ((k[x / 52] >> (x % 52)) & 1) << j;
        XMEMCPY(rt->x, table[y].x, sizeof(table[y].x));
        XMEMCPY(rt->y, table[y].y, sizeof(table[y].y));
        rt->infinity = table[y].infinity;
        for (i=30; i>=0; i--) {
            y = 0;
            for (j=0,x=i; j<8; j++,x+=32)
                y |= ((k[x / 52] >> (x % 52)) & 1) << j;

            sp_256_proj_point_dbl_5(rt, rt, t);
            XMEMCPY(p->x, table[y].x, sizeof(table[y].x));
            XMEMCPY(p->y, table[y].y, sizeof(table[y].y));
            p->infinity = table[y].infinity;
            sp_256_proj_point_add_qz1_5(rt, rt, p, t);
        }

        if (map)
            sp_256_map_5(r, rt, t);
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
    sp_digit x[5];
    sp_digit y[5];
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

        if (sp_256_cmp_equal_5(g->x, sp_cache[i].x) & 
                           sp_256_cmp_equal_5(g->y, sp_cache[i].y)) {
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
static int sp_256_ecc_mulmod_5(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#ifndef FP_ECC
    return sp_256_ecc_mulmod_fast_5(r, g, k, map, heap);
#else
    sp_digit tmp[2 * 5 * 5];
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
            sp_256_gen_stripe_table_5(g, cache->table, tmp, heap);

#ifndef HAVE_THREAD_LS
        wc_UnLockMutex(&sp_cache_lock);
#endif /* HAVE_THREAD_LS */

        if (cache->cnt < 2) {
            err = sp_256_ecc_mulmod_fast_5(r, g, k, map, heap);
        }
        else {
            err = sp_256_ecc_mulmod_stripe_5(r, g, cache->table, k,
                    map, heap);
        }
    }

    return err;
#endif
}

#endif
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
    sp_digit kd[5];
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
        k = XMALLOC(sizeof(sp_digit) * 5, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 5, km);
        sp_256_point_from_ecc_point_5(point, gm);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_5(point, point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_5(point, point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_5(point, r);

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (k != NULL)
        XFREE(k, heap, DYNAMIC_TYPE_ECC);
#endif
    sp_ecc_point_free(point, 0, heap);

    return err;
}

#ifdef WOLFSSL_SP_SMALL
/* Multiply the base point of P256 by the scalar and return the result.
 * If map is true then convert result to affine co-ordinates.
 *
 * r     Resulting point.
 * k     Scalar to multiply by.
 * map   Indicates whether to convert result to affine.
 * heap  Heap to use for allocation.
 * returns MEMORY_E when memory allocation fails and MP_OKAY on success.
 */
static int sp_256_ecc_mulmod_base_5(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    /* No pre-computed values. */
    return sp_256_ecc_mulmod_5(r, &p256_base, k, map, heap);
}

#else
static sp_table_entry p256_table[256] = {
    /* 0 */
    { { 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00 },
      1 },
    /* 1 */
    { { 0x730d418a9143cl,0xfc5fedb60179el,0x762251075ba95l,0x55c679fb732b7l,
        0x018905f76a537l },
      { 0x25357ce95560al,0xe4ba19e45cddfl,0xd21f3258b4ab8l,0x5d85d2e88688dl,
        0x08571ff182588l },
      0 },
    /* 2 */
    { { 0x886024147519al,0xac26b372f0202l,0x785ebc8d0981el,0x58e9a9d4a7caal,
        0x0d953c50ddbdfl },
      { 0x361ccfd590f8fl,0x6b44e6c9179d6l,0x2eb64cf72e962l,0x88f37fd961102l,
        0x0863ebb7e9eb2l },
      0 },
    /* 3 */
    { { 0x6b6235cdb6485l,0xa22f0a2f97785l,0xf7e300b808f0el,0x80a03e68d9544l,
        0x000076055b5ffl },
      { 0x4eb9b838d2010l,0xbb3243708a763l,0x42a660654014fl,0x3ee0e0e47d398l,
        0x0830877613437l },
      0 },
    /* 4 */
    { { 0x22fc516a0d2bbl,0x6c1a6234994f9l,0x7c62c8b0d5cc1l,0x667f9241cf3a5l,
        0x02f5e6961fd1bl },
      { 0x5c70bf5a01797l,0x4d609561925c1l,0x71fdb523d20b4l,0x0f7b04911b370l,
        0x0f648f9168d6fl },
      0 },
    /* 5 */
    { { 0x66847e137bbbcl,0x9e8a6a0bec9e5l,0x9d73463e43446l,0x0015b1c427617l,
        0x05abe0285133dl },
      { 0xa837cc04c7dabl,0x4c43260c0792al,0x8e6cc37573d9fl,0x73830c9315627l,
        0x094bb725b6b6fl },
      0 },
    /* 6 */
    { { 0x9b48f720f141cl,0xcd2df5bc74bbfl,0x11045c46199b3l,0xc4efdc3f61294l,
        0x0cdd6bbcb2f7dl },
      { 0x6700beaf436fdl,0x6db99326beccal,0x14f25226f647fl,0xe5f60c0fa7920l,
        0x0a361bebd4bdal },
      0 },
    /* 7 */
    { { 0xa2558597c13c7l,0x5f50b7c3e128al,0x3c09d1dc38d63l,0x292c07039aecfl,
        0x0ba12ca09c4b5l },
      { 0x08fa459f91dfdl,0x66ceea07fb9e4l,0xd780b293af43bl,0xef4b1eceb0899l,
        0x053ebb99d701fl },
      0 },
    /* 8 */
    { { 0x7ee31b0e63d34l,0x72a9e54fab4fel,0x5e7b5a4f46005l,0x4831c0493334dl,
        0x08589fb9206d5l },
      { 0x0f5cc6583553al,0x4ae25649e5aa7l,0x0044652087909l,0x1c4fcc9045071l,
        0x0ebb0696d0254l },
      0 },
    /* 9 */
    { { 0x6ca15ac1647c5l,0x47c4cf5799461l,0x64dfbacb8127dl,0x7da3dc666aa37l,
        0x0eb2820cbd1b2l },
      { 0x6f8d86a87e008l,0x9d922378f3940l,0x0ccecb2d87dfal,0xda1d56ed2e428l,
        0x01f28289b55a7l },
      0 },
    /* 10 */
    { { 0xaa0c03b89da99l,0x9eb8284022abbl,0x81c05e8a6f2d7l,0x4d6327847862bl,
        0x0337a4b5905e5l },
      { 0x7500d21f7794al,0xb77d6d7f613c6l,0x4cfd6e8207005l,0xfbd60a5a37810l,
        0x00d65e0d5f4c2l },
      0 },
    /* 11 */
    { { 0x09bbeb5275d38l,0x450be0a358d9dl,0x73eb2654268a7l,0xa232f0762ff49l,
        0x0c23da24252f4l },
      { 0x1b84f0b94520cl,0x63b05bd78e5dal,0x4d29ea1096667l,0xcff13a4dcb869l,
        0x019de3b8cc790l },
      0 },
    /* 12 */
    { { 0xa716c26c5fe04l,0x0b3bba1bdb183l,0x4cb712c3b28del,0xcbfd7432c586al,
        0x0e34dcbd491fcl },
      { 0x8d46baaa58403l,0x8682e97a53b40l,0x6aaa8af9a6974l,0x0f7f9e3901273l,
        0x0e7641f447b4el },
      0 },
    /* 13 */
    { { 0x53941df64ba59l,0xec0b0242fc7d7l,0x1581859d33f10l,0x57bf4f06dfc6al,
        0x04a12df57052al },
      { 0x6338f9439dbd0l,0xd4bde53e1fbfal,0x1f1b314d3c24bl,0xea46fd5e4ffa2l,
        0x06af5aa93bb5bl },
      0 },
    /* 14 */
    { { 0x0b69910c91999l,0x402a580491da1l,0x8cc20900a24b4l,0x40133e0094b4bl,
        0x05fe3475a66a4l },
      { 0x8cabdf93e7b4bl,0x1a7c23f91ab0fl,0xd1e6263292b50l,0xa91642e889aecl,
        0x0b544e308ecfel },
      0 },
    /* 15 */
    { { 0x8c6e916ddfdcel,0x66f89179e6647l,0xd4e67e12c3291l,0xc20b4e8d6e764l,
        0x0e0b6b2bda6b0l },
      { 0x12df2bb7efb57l,0xde790c40070d3l,0x79bc9441aac0dl,0x3774f90336ad6l,
        0x071c023de25a6l },
      0 },
    /* 16 */
    { { 0x8c244bfe20925l,0xc38fdce86762al,0xd38706391c19al,0x24f65a96a5d5dl,
        0x061d587d421d3l },
      { 0x673a2a37173eal,0x0853778b65e87l,0x5bab43e238480l,0xefbe10f8441e0l,
        0x0fa11fe124621l },
      0 },
    /* 17 */
    { { 0x91f2b2cb19ffdl,0x5bb1923c231c8l,0xac5ca8e01ba8dl,0xbedcb6d03d678l,
        0x0586eb04c1f13l },
      { 0x5c6e527e8ed09l,0x3c1819ede20c3l,0x6c652fa1e81a3l,0x4f11278fd6c05l,
        0x019d5ac087086l },
      0 },
    /* 18 */
    { { 0x9f581309a4e1fl,0x1be92700741e9l,0xfd28d20ab7de7l,0x563f26a5ef0bel,
        0x0e7c0073f7f9cl },
      { 0xd663a0ef59f76l,0x5420fcb0501f6l,0xa6602d4669b3bl,0x3c0ac08c1f7a7l,
        0x0e08504fec65bl },
      0 },
    /* 19 */
    { { 0x8f68da031b3cal,0x9ee6da6d66f09l,0x4f246e86d1cabl,0x96b45bfd81fa9l,
        0x078f018825b09l },
      { 0xefde43a25787fl,0x0d1dccac9bb7el,0x35bfc368016f8l,0x747a0cea4877bl,
        0x043a773b87e94l },
      0 },
    /* 20 */
    { { 0x77734d2b533d5l,0xf6a1bdddc0625l,0x79ec293673b8al,0x66b1577e7c9aal,
        0x0bb6de651c3b2l },
      { 0x9303ab65259b3l,0xd3d03a7480e7el,0xb3cfc27d6a0afl,0xb99bc5ac83d19l,
        0x060b4619a5d18l },
      0 },
    /* 21 */
    { { 0xa38e11ae5aa1cl,0x2b49e73658bd6l,0xe5f87edb8b765l,0xffcd0b130014el,
        0x09d0f27b2aeebl },
      { 0x246317a730a55l,0x2fddbbc83aca9l,0xc019a719c955bl,0xc48d07c1dfe0al,
        0x0244a566d356el },
      0 },
    /* 22 */
    { { 0x0394aeacf1f96l,0xa9024c271c6dbl,0x2cbd3b99f2122l,0xef692626ac1b8l,
        0x045e58c873581l },
      { 0xf479da38f9dbcl,0x46e888a040d3fl,0x6e0bed7a8aaf1l,0xb7a4945adfb24l,
        0x0c040e21cc1e4l },
      0 },
    /* 23 */
    { { 0xaf0006f8117b6l,0xff73a35433847l,0xd9475eb651969l,0x6ec7482b35761l,
        0x01cdf5c97682cl },
      { 0x775b411f04839l,0xf448de16987dbl,0x70b32197dbeacl,0xff3db2921dd1bl,
        0x0046755f8a92dl },
      0 },
    /* 24 */
    { { 0xac5d2bce8ffcdl,0x8b2fe61a82cc8l,0x202d6c70d53c4l,0xa5f3f6f161727l,
        0x0046e5e113b83l },
      { 0x8ff64d8007f01l,0x125af43183e7bl,0x5e1a03c7fb1efl,0x005b045c5ea63l,
        0x06e0106c3303dl },
      0 },
    /* 25 */
    { { 0x7358488dd73b1l,0x8f995ed0d948cl,0x56a2ab7767070l,0xcf1f38385ea8cl,
        0x0442594ede901l },
      { 0xaa2c912d4b65bl,0x3b96c90c37f8fl,0xe978d1f94c234l,0xe68ed326e4a15l,
        0x0a796fa514c2el },
      0 },
    /* 26 */
    { { 0xfb604823addd7l,0x83e56693b3359l,0xcbf3c809e2a61l,0x66e9f885b78e3l,
        0x0e4ad2da9c697l },
      { 0xf7f428e048a61l,0x8cc092d9a0357l,0x03ed8ef082d19l,0x5143fc3a1af4cl,
        0x0c5e94046c37bl },
      0 },
    /* 27 */
    { { 0xa538c2be75f9el,0xe8cb123a78476l,0x109c04b6fd1a9l,0x4747d85e4df0bl,
        0x063283dafdb46l },
      { 0x28cf7baf2df15l,0x550ad9a7f4ce7l,0x834bcc3e592c4l,0xa938fab226adel,
        0x068bd19ab1981l },
      0 },
    /* 28 */
    { { 0xead511887d659l,0xf4b359305ac08l,0xfe74fe33374d5l,0xdfd696986981cl,
        0x0495292f53c6fl },
      { 0x78c9e1acec896l,0x10ec5b44844a8l,0x64d60a7d964b2l,0x68376696f7e26l,
        0x00ec7530d2603l },
      0 },
    /* 29 */
    { { 0x13a05ad2687bbl,0x6af32e21fa2dal,0xdd4607ba1f83bl,0x3f0b390f5ef51l,
        0x00f6207a66486l },
      { 0x7e3bb0f138233l,0x6c272aa718bd6l,0x6ec88aedd66b9l,0x6dcf8ed004072l,
        0x0ff0db07208edl },
      0 },
    /* 30 */
    { { 0xfa1014c95d553l,0xfd5d680a8a749l,0xf3b566fa44052l,0x0ea3183b4317fl,
        0x0313b513c8874l },
      { 0x2e2ac08d11549l,0x0bb4dee21cb40l,0x7f2320e071ee1l,0x9f8126b987dd4l,
        0x02d3abcf986f1l },
      0 },
    /* 31 */
    { { 0x88501815581a2l,0x56632211af4c2l,0xcab2e999a0a6dl,0x8cdf19ba7a0f0l,
        0x0c036fa10ded9l },
      { 0xe08bac1fbd009l,0x9006d1581629al,0xb9e0d8f0b68b1l,0x0194c2eb32779l,
        0x0a6b2a2c4b6d4l },
      0 },
    /* 32 */
    { { 0x3e50f6d3549cfl,0x6ffacd665ed43l,0xe11fcb46f3369l,0x9860695bfdaccl,
        0x0810ee252af7cl },
      { 0x50fe17159bb2cl,0xbe758b357b654l,0x69fea72f7dfbel,0x17452b057e74dl,
        0x0d485717a9273l },
      0 },
    /* 33 */
    { { 0x41a8af0cb5a98l,0x931f3110bf117l,0xb382adfd3da8fl,0x604e1994e2cbal,
        0x06a6045a72f9al },
      { 0xc0d3fa2b2411dl,0x3e510e96e0170l,0x865b3ccbe0eb8l,0x57903bcc9f738l,
        0x0d3e45cfaf9e1l },
      0 },
    /* 34 */
    { { 0xf69bbe83f7669l,0x8272877d6bce1l,0x244278d09f8ael,0xc19c9548ae543l,
        0x0207755dee3c2l },
      { 0xd61d96fef1945l,0xefb12d28c387bl,0x2df64aa18813cl,0xb00d9fbcd1d67l,
        0x048dc5ee57154l },
      0 },
    /* 35 */
    { { 0x790bff7e5a199l,0xcf989ccbb7123l,0xa519c79e0efb8l,0xf445c27a2bfe0l,
        0x0f2fb0aeddff6l },
      { 0x09575f0b5025fl,0xd740fa9f2241cl,0x80bfbd0550543l,0xd5258fa3c8ad3l,
        0x0a13e9015db28l },
      0 },
    /* 36 */
    { { 0x7a350a2b65cbcl,0x722a464226f9fl,0x23f07a10b04b9l,0x526f265ce241el,
        0x02bf0d6b01497l },
      { 0x4dd3f4b216fb7l,0x67fbdda26ad3dl,0x708505cf7d7b8l,0xe89faeb7b83f6l,
        0x042a94a5a162fl },
      0 },
    /* 37 */
    { { 0x6ad0beaadf191l,0x9025a268d7584l,0x94dc1f60f8a48l,0xde3de86030504l,
        0x02c2dd969c65el },
      { 0x2171d93849c17l,0xba1da250dd6d0l,0xc3a5485460488l,0x6dbc4810c7063l,
        0x0f437fa1f42c5l },
      0 },
    /* 38 */
    { { 0x0d7144a0f7dabl,0x931776e9ac6aal,0x5f397860f0497l,0x7aa852c0a050fl,
        0x0aaf45b335470l },
      { 0x37c33c18d364al,0x063e49716585el,0x5ec5444d40b9bl,0x72bcf41716811l,
        0x0cdf6310df4f2l },
      0 },
    /* 39 */
    { { 0x3c6238ea8b7efl,0x1885bc2287747l,0xbda8e3408e935l,0x2ff2419567722l,
        0x0f0d008bada9el },
      { 0x2671d2414d3b1l,0x85b019ea76291l,0x53bcbdbb37549l,0x7b8b5c61b96d4l,
        0x05bd5c2f5ca88l },
      0 },
    /* 40 */
    { { 0xf469ef49a3154l,0x956e2b2e9aef0l,0xa924a9c3e85a5l,0x471945aaec1eal,
        0x0aa12dfc8a09el },
      { 0x272274df69f1dl,0x2ca2ff5e7326fl,0x7a9dd44e0e4c8l,0xa901b9d8ce73bl,
        0x06c036e73e48cl },
      0 },
    /* 41 */
    { { 0xae12a0f6e3138l,0x0025ad345a5cfl,0x5672bc56966efl,0xbe248993c64b4l,
        0x0292ff65896afl },
      { 0x50d445e213402l,0x274392c9fed52l,0xa1c72e8f6580el,0x7276097b397fdl,
        0x0644e0c90311bl },
      0 },
    /* 42 */
    { { 0x421e1a47153f0l,0x79920418c9e1el,0x05d7672b86c3bl,0x9a7793bdce877l,
        0x0f25ae793cab7l },
      { 0x194a36d869d0cl,0x824986c2641f3l,0x96e945e9d55c8l,0x0a3e49fb5ea30l,
        0x039b8e65313dbl },
      0 },
    /* 43 */
    { { 0x54200b6fd2e59l,0x669255c98f377l,0xe2a573935e2c0l,0xdb06d9dab21a0l,
        0x039122f2f0f19l },
      { 0xce1e003cad53cl,0x0fe65c17e3cfbl,0xaa13877225b2cl,0xff8d72baf1d29l,
        0x08de80af8ce80l },
      0 },
    /* 44 */
    { { 0xea8d9207bbb76l,0x7c21782758afbl,0xc0436b1921c7el,0x8c04dfa2b74b1l,
        0x0871949062e36l },
      { 0x928bba3993df5l,0xb5f3b3d26ab5fl,0x5b55050639d75l,0xfde1011aa78a8l,
        0x0fc315e6a5b74l },
      0 },
    /* 45 */
    { { 0xfd41ae8d6ecfal,0xf61aec7f86561l,0x924741d5f8c44l,0x908898452a7b4l,
        0x0e6d4a7adee38l },
      { 0x52ed14593c75dl,0xa4dd271162605l,0xba2c7db70a70dl,0xae57d2aede937l,
        0x035dfaf9a9be2l },
      0 },
    /* 46 */
    { { 0x56fcdaa736636l,0x97ae2cab7e6b9l,0xf34996609f51dl,0x0d2bfb10bf410l,
        0x01da5c7d71c83l },
      { 0x1e4833cce6825l,0x8ff9573c3b5c4l,0x23036b815ad11l,0xb9d6a28552c7fl,
        0x07077c0fddbf4l },
      0 },
    /* 47 */
    { { 0x3ff8d46b9661cl,0x6b0d2cfd71bf6l,0x847f8f7a1dfd3l,0xfe440373e140al,
        0x053a8632ee50el },
      { 0x6ff68696d8051l,0x95c74f468a097l,0xe4e26bddaec0cl,0xfcc162994dc35l,
        0x0028ca76d34e1l },
      0 },
    /* 48 */
    { { 0xd47dcfc9877eel,0x10801d0002d11l,0x4c260b6c8b362l,0xf046d002c1175l,
        0x004c17cd86962l },
      { 0xbd094b0daddf5l,0x7524ce55c06d9l,0x2da03b5bea235l,0x7474663356e67l,
        0x0f7ba4de9fed9l },
      0 },
    /* 49 */
    { { 0xbfa34ebe1263fl,0x3571ae7ce6d0dl,0x2a6f523557637l,0x1c41d24405538l,
        0x0e31f96005213l },
      { 0xb9216ea6b6ec6l,0x2e73c2fc44d1bl,0x9d0a29437a1d1l,0xd47bc10e7eac8l,
        0x0aa3a6259ce34l },
      0 },
    /* 50 */
    { { 0xf9df536f3dcd3l,0x50d2bf7360fbcl,0xf504f5b6cededl,0xdaee491710fadl,
        0x02398dd627e79l },
      { 0x705a36d09569el,0xbb5149f769cf4l,0x5f6034cea0619l,0x6210ff9c03773l,
        0x05717f5b21c04l },
      0 },
    /* 51 */
    { { 0x229c921dd895el,0x0040c284519fel,0xd637ecd8e5185l,0x28defa13d2391l,
        0x0660a2c560e3cl },
      { 0xa88aed67fcbd0l,0x780ea9f0969ccl,0x2e92b4dc84724l,0x245332b2f4817l,
        0x0624ee54c4f52l },
      0 },
    /* 52 */
    { { 0x49ce4d897ecccl,0xd93f9880aa095l,0x43a7c204d49d1l,0xfbc0723c24230l,
        0x04f392afb92bdl },
      { 0x9f8fa7de44fd9l,0xe457b32156696l,0x68ebc3cb66cfbl,0x399cdb2fa8033l,
        0x08a3e7977ccdbl },
      0 },
    /* 53 */
    { { 0x1881f06c4b125l,0x00f6e3ca8cddel,0xc7a13e9ae34e3l,0x4404ef6999de5l,
        0x03888d02370c2l },
      { 0x8035644f91081l,0x615f015504762l,0x32cd36e3d9fcfl,0x23361827edc86l,
        0x0a5e62e471810l },
      0 },
    /* 54 */
    { { 0x25ee32facd6c8l,0x5454bcbc661a8l,0x8df9931699c63l,0x5adc0ce3edf79l,
        0x02c4768e6466al },
      { 0x6ff8c90a64bc9l,0x20e4779f5cb34l,0xc05e884630a60l,0x52a0d949d064bl,
        0x07b5e6441f9e6l },
      0 },
    /* 55 */
    { { 0x9422c1d28444al,0xd8be136a39216l,0xb0c7fcee996c5l,0x744a2387afe5fl,
        0x0b8af73cb0c8dl },
      { 0xe83aa338b86fdl,0x58a58a5cff5fdl,0x0ac9433fee3f1l,0x0895c9ee8f6f2l,
        0x0a036395f7f3fl },
      0 },
    /* 56 */
    { { 0x3c6bba10f7770l,0x81a12a0e248c7l,0x1bc2b9fa6f16dl,0xb533100df6825l,
        0x04be36b01875fl },
      { 0x6086e9fb56dbbl,0x8b07e7a4f8922l,0x6d52f20306fefl,0x00c0eeaccc056l,
        0x08cbc9a871bdcl },
      0 },
    /* 57 */
    { { 0x1895cc0dac4abl,0x40712ff112e13l,0xa1cee57a874a4l,0x35f86332ae7c6l,
        0x044e7553e0c08l },
      { 0x03fff7734002dl,0x8b0b34425c6d5l,0xe8738b59d35cbl,0xfc1895f702760l,
        0x0470a683a5eb8l },
      0 },
    /* 58 */
    { { 0x761dc90513482l,0x2a01e9276a81bl,0xce73083028720l,0xc6efcda441ee0l,
        0x016410690c63dl },
      { 0x34a066d06a2edl,0x45189b100bf50l,0xb8218c9dd4d77l,0xbb4fd914ae72al,
        0x0d73479fd7abcl },
      0 },
    /* 59 */
    { { 0xefb165ad4c6e5l,0x8f5b06d04d7edl,0x575cb14262cf0l,0x666b12ed5bb18l,
        0x0816469e30771l },
      { 0xb9d79561e291el,0x22c1de1661d7al,0x35e0513eb9dafl,0x3f9cf49827eb1l,
        0x00a36dd23f0ddl },
      0 },
    /* 60 */
    { { 0xd32c741d5533cl,0x9e8684628f098l,0x349bd117c5f5al,0xb11839a228adel,
        0x0e331dfd6fdbal },
      { 0x0ab686bcc6ed8l,0xbdef7a260e510l,0xce850d77160c3l,0x33899063d9a7bl,
        0x0d3b4782a492el },
      0 },
    /* 61 */
    { { 0x9b6e8f3821f90l,0xed66eb7aada14l,0xa01311692edd9l,0xa5bd0bb669531l,
        0x07281275a4c86l },
      { 0x858f7d3ff47e5l,0xbc61016441503l,0xdfd9bb15e1616l,0x505962b0f11a7l,
        0x02c062e7ece14l },
      0 },
    /* 62 */
    { { 0xf996f0159ac2el,0x36cbdb2713a76l,0x8e46047281e77l,0x7ef12ad6d2880l,
        0x0282a35f92c4el },
      { 0x54b1ec0ce5cd2l,0xc91379c2299c3l,0xe82c11ecf99efl,0x2abd992caf383l,
        0x0c71cd513554dl },
      0 },
    /* 63 */
    { { 0x5de9c09b578f4l,0x58e3affa7a488l,0x9182f1f1884e2l,0xf3a38f76b1b75l,
        0x0c50f6740cf47l },
      { 0x4adf3374b68eal,0x2369965fe2a9cl,0x5a53050a406f3l,0x58dc2f86a2228l,
        0x0b9ecb3a72129l },
      0 },
    /* 64 */
    { { 0x8410ef4f8b16al,0xfec47b266a56fl,0xd9c87c197241al,0xab1b0a406b8e6l,
        0x0803f3e02cd42l },
      { 0x309a804dbec69l,0xf73bbad05f7f0l,0xd8e197fa83b85l,0xadc1c6097273al,
        0x0c097440e5067l },
      0 },
    /* 65 */
    { { 0xa56f2c379ab34l,0x8b841df8d1846l,0x76c68efa8ee06l,0x1f30203144591l,
        0x0f1af32d5915fl },
      { 0x375315d75bd50l,0xbaf72f67bc99cl,0x8d7723f837cffl,0x1c8b0613a4184l,
        0x023d0f130e2d4l },
      0 },
    /* 66 */
    { { 0xab6edf41500d9l,0xe5fcbeada8857l,0x97259510d890al,0xfadd52fe86488l,
        0x0b0288dd6c0a3l },
      { 0x20f30650bcb08l,0x13695d6e16853l,0x989aa7671af63l,0xc8d231f520a7bl,
        0x0ffd3724ff408l },
      0 },
    /* 67 */
    { { 0x68e64b458e6cbl,0x20317a5d28539l,0xaa75f56992dadl,0x26df3814ae0b7l,
        0x0f5590f4ad78cl },
      { 0x24bd3cf0ba55al,0x4a0c778bae0fcl,0x83b674a0fc472l,0x4a201ce9864f6l,
        0x018d6da54f6f7l },
      0 },
    /* 68 */
    { { 0x3e225d5be5a2bl,0x835934f3c6ed9l,0x2626ffc6fe799l,0x216a431409262l,
        0x050bbb4d97990l },
      { 0x191c6e57ec63el,0x40181dcdb2378l,0x236e0f665422cl,0x49c341a8099b0l,
        0x02b10011801fel },
      0 },
    /* 69 */
    { { 0x8b5c59b391593l,0xa2598270fcfc6l,0x19adcbbc385f5l,0xae0c7144f3aadl,
        0x0dd55899983fbl },
      { 0x88b8e74b82ff4l,0x4071e734c993bl,0x3c0322ad2e03cl,0x60419a7a9eaf4l,
        0x0e6e4c551149dl },
      0 },
    /* 70 */
    { { 0x655bb1e9af288l,0x64f7ada93155fl,0xb2820e5647e1al,0x56ff43697e4bcl,
        0x051e00db107edl },
      { 0x169b8771c327el,0x0b4a96c2ad43dl,0xdeb477929cdb2l,0x9177c07d51f53l,
        0x0e22f42414982l },
      0 },
    /* 71 */
    { { 0x5e8f4635f1abbl,0xb568538874cd4l,0x5a8034d7edc0cl,0x48c9c9472c1fbl,
        0x0f709373d52dcl },
      { 0x966bba8af30d6l,0x4af137b69c401l,0x361c47e95bf5fl,0x5b113966162a9l,
        0x0bd52d288e727l },
      0 },
    /* 72 */
    { { 0x55c7a9c5fa877l,0x727d3a3d48ab1l,0x3d189d817dad6l,0x77a643f43f9e7l,
        0x0a0d0f8e4c8aal },
      { 0xeafd8cc94f92dl,0xbe0c4ddb3a0bbl,0x82eba14d818c8l,0x6a0022cc65f8bl,
        0x0a56c78c7946dl },
      0 },
    /* 73 */
    { { 0x2391b0dd09529l,0xa63daddfcf296l,0xb5bf481803e0el,0x367a2c77351f5l,
        0x0d8befdf8731al },
      { 0x19d42fc0157f4l,0xd7fec8e650ab9l,0x2d48b0af51cael,0x6478cdf9cb400l,
        0x0854a68a5ce9fl },
      0 },
    /* 74 */
    { { 0x5f67b63506ea5l,0x89a4fe0d66dc3l,0xe95cd4d9286c4l,0x6a953f101d3bfl,
        0x05cacea0b9884l },
      { 0xdf60c9ceac44dl,0xf4354d1c3aa90l,0xd5dbabe3db29al,0xefa908dd3de8al,
        0x0e4982d1235e4l },
      0 },
    /* 75 */
    { { 0x04a22c34cd55el,0xb32680d132231l,0xfa1d94358695bl,0x0499fb345afa1l,
        0x08046b7f616b2l },
      { 0x3581e38e7d098l,0x8df46f0b70b53l,0x4cb78c4d7f61el,0xaf5530dea9ea4l,
        0x0eb17ca7b9082l },
      0 },
    /* 76 */
    { { 0x1b59876a145b9l,0x0fc1bc71ec175l,0x92715bba5cf6bl,0xe131d3e035653l,
        0x0097b00bafab5l },
      { 0x6c8e9565f69e1l,0x5ab5be5199aa6l,0xa4fd98477e8f7l,0xcc9e6033ba11dl,
        0x0f95c747bafdbl },
      0 },
    /* 77 */
    { { 0xf01d3bebae45el,0xf0c4bc6955558l,0xbc64fc6a8ebe9l,0xd837aeb705b1dl,
        0x03512601e566el },
      { 0x6f1e1fa1161cdl,0xd54c65ef87933l,0x24f21e5328ab8l,0xab6b4757eee27l,
        0x00ef971236068l },
      0 },
    /* 78 */
    { { 0x98cf754ca4226l,0x38f8642c8e025l,0x68e17905eede1l,0xbc9548963f744l,
        0x0fc16d9333b4fl },
      { 0x6fb31e7c800cal,0x312678adaabe9l,0xff3e8b5138063l,0x7a173d6244976l,
        0x014ca4af1b95dl },
      0 },
    /* 79 */
    { { 0x771babd2f81d5l,0x6901f7d1967a4l,0xad9c9071a5f9dl,0x231dd898bef7cl,
        0x04057b063f59cl },
      { 0xd82fe89c05c0al,0x6f1dc0df85bffl,0x35a16dbe4911cl,0x0b133befccaeal,
        0x01c3b5d64f133l },
      0 },
    /* 80 */
    { { 0x14bfe80ec21fel,0x6ac255be825fel,0xf4a5d67f6ce11l,0x63af98bc5a072l,
        0x0fad27148db7el },
      { 0x0b6ac29ab05b3l,0x3c4e251ae690cl,0x2aade7d37a9a8l,0x1a840a7dc875cl,
        0x077387de39f0el },
      0 },
    /* 81 */
    { { 0xecc49a56c0dd7l,0xd846086c741e9l,0x505aecea5cffcl,0xc47e8f7a1408fl,
        0x0b37b85c0bef0l },
      { 0x6b6e4cc0e6a8fl,0xbf6b388f23359l,0x39cef4efd6d4bl,0x28d5aba453facl,
        0x09c135ac8f9f6l },
      0 },
    /* 82 */
    { { 0xa320284e35743l,0xb185a3cdef32al,0xdf19819320d6al,0x851fb821b1761l,
        0x05721361fc433l },
      { 0xdb36a71fc9168l,0x735e5c403c1f0l,0x7bcd8f55f98bal,0x11bdf64ca87e3l,
        0x0dcbac3c9e6bbl },
      0 },
    /* 83 */
    { { 0xd99684518cbe2l,0x189c9eb04ef01l,0x47feebfd242fcl,0x6862727663c7el,
        0x0b8c1c89e2d62l },
      { 0x58bddc8e1d569l,0xc8b7d88cd051al,0x11f31eb563809l,0x22d426c27fd9fl,
        0x05d23bbda2f94l },
      0 },
    /* 84 */
    { { 0xc729495c8f8bel,0x803bf362bf0a1l,0xf63d4ac2961c4l,0xe9009e418403dl,
        0x0c109f9cb91ecl },
      { 0x095d058945705l,0x96ddeb85c0c2dl,0xa40449bb9083dl,0x1ee184692b8d7l,
        0x09bc3344f2eeel },
      0 },
    /* 85 */
    { { 0xae35642913074l,0x2748a542b10d5l,0x310732a55491bl,0x4cc1469ca665bl,
        0x029591d525f1al },
      { 0xf5b6bb84f983fl,0x419f5f84e1e76l,0x0baa189be7eefl,0x332c1200d4968l,
        0x06376551f18efl },
      0 },
    /* 86 */
    { { 0x5f14e562976ccl,0xe60ef12c38bdal,0xcca985222bca3l,0x987abbfa30646l,
        0x0bdb79dc808e2l },
      { 0xcb5c9cb06a772l,0xaafe536dcefd2l,0xc2b5db838f475l,0xc14ac2a3e0227l,
        0x08ee86001add3l },
      0 },
    /* 87 */
    { { 0x96981a4ade873l,0x4dc4fba48ccbel,0xa054ba57ee9aal,0xaa4b2cee28995l,
        0x092e51d7a6f77l },
      { 0xbafa87190a34dl,0x5bf6bd1ed1948l,0xcaf1144d698f7l,0xaaaad00ee6e30l,
        0x05182f86f0a56l },
      0 },
    /* 88 */
    { { 0x6212c7a4cc99cl,0x683e6d9ca1fbal,0xac98c5aff609bl,0xa6f25dbb27cb5l,
        0x091dcab5d4073l },
      { 0x6cc3d5f575a70l,0x396f8d87fa01bl,0x99817360cb361l,0x4f2b165d4e8c8l,
        0x017a0cedb9797l },
      0 },
    /* 89 */
    { { 0x61e2a076c8d3al,0x39210f924b388l,0x3a835d9701aadl,0xdf4194d0eae41l,
        0x02e8ce36c7f4cl },
      { 0x73dab037a862bl,0xb760e4c8fa912l,0x3baf2dd01ba9bl,0x68f3f96453883l,
        0x0f4ccc6cb34f6l },
      0 },
    /* 90 */
    { { 0xf525cf1f79687l,0x9592efa81544el,0x5c78d297c5954l,0xf3c9e1231741al,
        0x0ac0db4889a0dl },
      { 0xfc711df01747fl,0x58ef17df1386bl,0xccb6bb5592b93l,0x74a2e5880e4f5l,
        0x095a64a6194c9l },
      0 },
    /* 91 */
    { { 0x1efdac15a4c93l,0x738258514172cl,0x6cb0bad40269bl,0x06776a8dfb1c1l,
        0x0231e54ba2921l },
      { 0xdf9178ae6d2dcl,0x3f39112918a70l,0xe5b72234d6aa6l,0x31e1f627726b5l,
        0x0ab0be032d8a7l },
      0 },
    /* 92 */
    { { 0xad0e98d131f2dl,0xe33b04f101097l,0x5e9a748637f09l,0xa6791ac86196dl,
        0x0f1bcc8802cf6l },
      { 0x69140e8daacb4l,0x5560f6500925cl,0x77937a63c4e40l,0xb271591cc8fc4l,
        0x0851694695aebl },
      0 },
    /* 93 */
    { { 0x5c143f1dcf593l,0x29b018be3bde3l,0xbdd9d3d78202bl,0x55d8e9cdadc29l,
        0x08f67d9d2daadl },
      { 0x116567481ea5fl,0xe9e34c590c841l,0x5053fa8e7d2ddl,0x8b5dffdd43f40l,
        0x0f84572b9c072l },
      0 },
    /* 94 */
    { { 0xa7a7197af71c9l,0x447a7365655e1l,0xe1d5063a14494l,0x2c19a1b4ae070l,
        0x0edee2710616bl },
      { 0x034f511734121l,0x554a25e9f0b2fl,0x40c2ecf1cac6el,0xd7f48dc148f3al,
        0x09fd27e9b44ebl },
      0 },
    /* 95 */
    { { 0x7658af6e2cb16l,0x2cfe5919b63ccl,0x68d5583e3eb7dl,0xf3875a8c58161l,
        0x0a40c2fb6958fl },
      { 0xec560fedcc158l,0xc655f230568c9l,0xa307e127ad804l,0xdecfd93967049l,
        0x099bc9bb87dc6l },
      0 },
    /* 96 */
    { { 0x9521d927dafc6l,0x695c09cd1984al,0x9366dde52c1fbl,0x7e649d9581a0fl,
        0x09abe210ba16dl },
      { 0xaf84a48915220l,0x6a4dd816c6480l,0x681ca5afa7317l,0x44b0c7d539871l,
        0x07881c25787f3l },
      0 },
    /* 97 */
    { { 0x99b51e0bcf3ffl,0xc5127f74f6933l,0xd01d9680d02cbl,0x89408fb465a2dl,
        0x015e6e319a30el },
      { 0xd6e0d3e0e05f4l,0xdc43588404646l,0x4f850d3fad7bdl,0x72cebe61c7d1cl,
        0x00e55facf1911l },
      0 },
    /* 98 */
    { { 0xd9806f8787564l,0x2131e85ce67e9l,0x819e8d61a3317l,0x65776b0158cabl,
        0x0d73d09766fe9l },
      { 0x834251eb7206el,0x0fc618bb42424l,0xe30a520a51929l,0xa50b5dcbb8595l,
        0x09250a3748f15l },
      0 },
    /* 99 */
    { { 0xf08f8be577410l,0x035077a8c6cafl,0xc0a63a4fd408al,0x8c0bf1f63289el,
        0x077414082c1ccl },
      { 0x40fa6eb0991cdl,0x6649fdc29605al,0x324fd40c1ca08l,0x20b93a68a3c7bl,
        0x08cb04f4d12ebl },
      0 },
    /* 100 */
    { { 0x2d0556906171cl,0xcdb0240c3fb1cl,0x89068419073e9l,0x3b51db8e6b4fdl,
        0x0e4e429ef4712l },
      { 0xdd53c38ec36f4l,0x01ff4b6a270b8l,0x79a9a48f9d2dcl,0x65525d066e078l,
        0x037bca2ff3c6el },
      0 },
    /* 101 */
    { { 0x2e3c7df562470l,0xa2c0964ac94cdl,0x0c793be44f272l,0xb22a7c6d5df98l,
        0x059913edc3002l },
      { 0x39a835750592al,0x80e783de027a1l,0xa05d64f99e01dl,0xe226cf8c0375el,
        0x043786e4ab013l },
      0 },
    /* 102 */
    { { 0x2b0ed9e56b5a6l,0xa6d9fc68f9ff3l,0x97846a70750d9l,0x9e7aec15e8455l,
        0x08638ca98b7e7l },
      { 0xae0960afc24b2l,0xaf4dace8f22f5l,0xecba78f05398el,0xa6f03b765dd0al,
        0x01ecdd36a7b3al },
      0 },
    /* 103 */
    { { 0xacd626c5ff2f3l,0xc02873a9785d3l,0x2110d54a2d516l,0xf32dad94c9fadl,
        0x0d85d0f85d459l },
      { 0x00b8d10b11da3l,0x30a78318c49f7l,0x208decdd2c22cl,0x3c62556988f49l,
        0x0a04f19c3b4edl },
      0 },
    /* 104 */
    { { 0x924c8ed7f93bdl,0x5d392f51f6087l,0x21b71afcb64acl,0x50b07cae330a8l,
        0x092b2eeea5c09l },
      { 0xc4c9485b6e235l,0xa92936c0f085al,0x0508891ab2ca4l,0x276c80faa6b3el,
        0x01ee782215834l },
      0 },
    /* 105 */
    { { 0xa2e00e63e79f7l,0xb2f399d906a60l,0x607c09df590e7l,0xe1509021054a6l,
        0x0f3f2ced857a6l },
      { 0x510f3f10d9b55l,0xacd8642648200l,0x8bd0e7c9d2fcfl,0xe210e5631aa7el,
        0x00f56a4543da3l },
      0 },
    /* 106 */
    { { 0x1bffa1043e0dfl,0xcc9c007e6d5b2l,0x4a8517a6c74b6l,0xe2631a656ec0dl,
        0x0bd8f17411969l },
      { 0xbbb86beb7494al,0x6f45f3b8388a9l,0x4e5a79a1567d4l,0xfa09df7a12a7al,
        0x02d1a1c3530ccl },
      0 },
    /* 107 */
    { { 0xe3813506508dal,0xc4a1d795a7192l,0xa9944b3336180l,0xba46cddb59497l,
        0x0a107a65eb91fl },
      { 0x1d1c50f94d639l,0x758a58b7d7e6dl,0xd37ca1c8b4af3l,0x9af21a7c5584bl,
        0x0183d760af87al },
      0 },
    /* 108 */
    { { 0x697110dde59a4l,0x070e8bef8729dl,0xf2ebe78f1ad8dl,0xd754229b49634l,
        0x01d44179dc269l },
      { 0xdc0cf8390d30el,0x530de8110cb32l,0xbc0339a0a3b27l,0xd26231af1dc52l,
        0x0771f9cc29606l },
      0 },
    /* 109 */
    { { 0x93e7785040739l,0xb98026a939999l,0x5f8fc2644539dl,0x718ecf40f6f2fl,
        0x064427a310362l },
      { 0xf2d8785428aa8l,0x3febfb49a84f4l,0x23d01ac7b7adcl,0x0d6d201b2c6dfl,
        0x049d9b7496ae9l },
      0 },
    /* 110 */
    { { 0x8d8bc435d1099l,0x4e8e8d1a08cc7l,0xcb68a412adbcdl,0x544502c2e2a02l,
        0x09037d81b3f60l },
      { 0xbac27074c7b61l,0xab57bfd72e7cdl,0x96d5352fe2031l,0x639c61ccec965l,
        0x008c3de6a7cc0l },
      0 },
    /* 111 */
    { { 0xdd020f6d552abl,0x9805cd81f120fl,0x135129156baffl,0x6b2f06fb7c3e9l,
        0x0c69094424579l },
      { 0x3ae9c41231bd1l,0x875cc5820517bl,0x9d6a1221eac6el,0x3ac0208837abfl,
        0x03fa3db02cafel },
      0 },
    /* 112 */
    { { 0xa3e6505058880l,0xef643943f2d75l,0xab249257da365l,0x08ff4147861cfl,
        0x0c5c4bdb0fdb8l },
      { 0x13e34b272b56bl,0x9511b9043a735l,0x8844969c8327el,0xb6b5fd8ce37dfl,
        0x02d56db9446c2l },
      0 },
    /* 113 */
    { { 0x1782fff46ac6bl,0x2607a2e425246l,0x9a48de1d19f79l,0xba42fafea3c40l,
        0x00f56bd9de503l },
      { 0xd4ed1345cda49l,0xfc816f299d137l,0xeb43402821158l,0xb5f1e7c6a54aal,
        0x04003bb9d1173l },
      0 },
    /* 114 */
    { { 0xe8189a0803387l,0xf539cbd4043b8l,0x2877f21ece115l,0x2f9e4297208ddl,
        0x053765522a07fl },
      { 0x80a21a8a4182dl,0x7a3219df79a49l,0xa19a2d4a2bbd0l,0x4549674d0a2e1l,
        0x07a056f586c5dl },
      0 },
    /* 115 */
    { { 0xb25589d8a2a47l,0x48c3df2773646l,0xbf0d5395b5829l,0x267551ec000eal,
        0x077d482f17a1al },
      { 0x1bd9587853948l,0xbd6cfbffeeb8al,0x0681e47a6f817l,0xb0e4ab6ec0578l,
        0x04115012b2b38l },
      0 },
    /* 116 */
    { { 0x3f0f46de28cedl,0x609b13ec473c7l,0xe5c63921d5da7l,0x094661b8ce9e6l,
        0x0cdf04572fbeal },
      { 0x3c58b6c53c3b0l,0x10447b843c1cbl,0xcb9780e97fe3cl,0x3109fb2b8ae12l,
        0x0ee703dda9738l },
      0 },
    /* 117 */
    { { 0x15140ff57e43al,0xd3b1b811b8345l,0xf42b986d44660l,0xce212b3b5dff8l,
        0x02a0ad89da162l },
      { 0x4a6946bc277bal,0x54c141c27664el,0xabf6274c788c9l,0x4659141aa64ccl,
        0x0d62d0b67ac2bl },
      0 },
    /* 118 */
    { { 0x5d87b2c054ac4l,0x59f27df78839cl,0x18128d6570058l,0x2426edf7cbf3bl,
        0x0b39a23f2991cl },
      { 0x84a15f0b16ae5l,0xb1a136f51b952l,0x27007830c6a05l,0x4cc51d63c137fl,
        0x004ed0092c067l },
      0 },
    /* 119 */
    { { 0x185d19ae90393l,0x294a3d64e61f4l,0x854fc143047b4l,0xc387ae0001a69l,
        0x0a0a91fc10177l },
      { 0xa3f01ae2c831el,0x822b727e16ff0l,0xa3075b4bb76ael,0x0c418f12c8a15l,
        0x0084cf9889ed2l },
      0 },
    /* 120 */
    { { 0x509defca6becfl,0x807dffb328d98l,0x778e8b92fceael,0xf77e5d8a15c44l,
        0x0d57955b273abl },
      { 0xda79e31b5d4f1l,0x4b3cfa7a1c210l,0xc27c20baa52f0l,0x41f1d4d12089dl,
        0x08e14ea4202d1l },
      0 },
    /* 121 */
    { { 0x50345f2897042l,0x1f43402c4aeedl,0x8bdfb218d0533l,0xd158c8d9c194cl,
        0x0597e1a372aa4l },
      { 0x7ec1acf0bd68cl,0xdcab024945032l,0x9fe3e846d4be0l,0x4dea5b9c8d7acl,
        0x0ca3f0236199bl },
      0 },
    /* 122 */
    { { 0xa10b56170bd20l,0xf16d3f5de7592l,0x4b2ade20ea897l,0x07e4a3363ff14l,
        0x0bde7fd7e309cl },
      { 0xbb6d2b8f5432cl,0xcbe043444b516l,0x8f95b5a210dc1l,0xd1983db01e6ffl,
        0x0b623ad0e0a7dl },
      0 },
    /* 123 */
    { { 0xbd67560c7b65bl,0x9023a4a289a75l,0x7b26795ab8c55l,0x137bf8220fd0dl,
        0x0d6aa2e4658ecl },
      { 0xbc00b5138bb85l,0x21d833a95c10al,0x702a32e8c31d1l,0x513ab24ff00b1l,
        0x0111662e02dccl },
      0 },
    /* 124 */
    { { 0x14015efb42b87l,0x701b6c4dff781l,0x7d7c129bd9f5dl,0x50f866ecccd7al,
        0x0db3ee1cb94b7l },
      { 0xf3db0f34837cfl,0x8bb9578d4fb26l,0xc56657de7eed1l,0x6a595d2cdf937l,
        0x0886a64425220l },
      0 },
    /* 125 */
    { { 0x34cfb65b569eal,0x41f72119c13c2l,0x15a619e200111l,0x17bc8badc85dal,
        0x0a70cf4eb018al },
      { 0xf97ae8c4a6a65l,0x270134378f224l,0xf7e096036e5cfl,0x7b77be3a609e4l,
        0x0aa4772abd174l },
      0 },
    /* 126 */
    { { 0x761317aa60cc0l,0x610368115f676l,0xbc1bb5ac79163l,0xf974ded98bb4bl,
        0x0611a6ddc30fal },
      { 0x78cbcc15ee47al,0x824e0d96a530el,0xdd9ed882e8962l,0x9c8836f35adf3l,
        0x05cfffaf81642l },
      0 },
    /* 127 */
    { { 0x54cff9b7a99cdl,0x9d843c45a1c0dl,0x2c739e17bf3b9l,0x994c038a908f6l,
        0x06e5a6b237dc1l },
      { 0xb454e0ba5db77l,0x7facf60d63ef8l,0x6608378b7b880l,0xabcce591c0c67l,
        0x0481a238d242dl },
      0 },
    /* 128 */
    { { 0x17bc035d0b34al,0x6b8327c0a7e34l,0xc0362d1440b38l,0xf9438fb7262dal,
        0x02c41114ce0cdl },
      { 0x5cef1ad95a0b1l,0xa867d543622bal,0x1e486c9c09b37l,0x929726d6cdd20l,
        0x020477abf42ffl },
      0 },
    /* 129 */
    { { 0x5173c18d65dbfl,0x0e339edad82f7l,0xcf1001c77bf94l,0x96b67022d26bdl,
        0x0ac66409ac773l },
      { 0xbb36fc6261cc3l,0xc9190e7e908b0l,0x45e6c10213f7bl,0x2f856541cebaal,
        0x0ce8e6975cc12l },
      0 },
    /* 130 */
    { { 0x21b41bc0a67d2l,0x0a444d248a0f1l,0x59b473762d476l,0xb4a80e044f1d6l,
        0x008fde365250bl },
      { 0xec3da848bf287l,0x82d3369d6eacel,0x2449482c2a621l,0x6cd73582dfdc9l,
        0x02f7e2fd2565dl },
      0 },
    /* 131 */
    { { 0xb92dbc3770fa7l,0x5c379043f9ae4l,0x7761171095e8dl,0x02ae54f34e9d1l,
        0x0c65be92e9077l },
      { 0x8a303f6fd0a40l,0xe3bcce784b275l,0xf9767bfe7d822l,0x3b3a7ae4f5854l,
        0x04bff8e47d119l },
      0 },
    /* 132 */
    { { 0x1d21f00ff1480l,0x7d0754db16cd4l,0xbe0f3ea2ab8fbl,0x967dac81d2efbl,
        0x03e4e4ae65772l },
      { 0x8f36d3c5303e6l,0x4b922623977e1l,0x324c3c03bd999l,0x60289ed70e261l,
        0x05388aefd58ecl },
      0 },
    /* 133 */
    { { 0x317eb5e5d7713l,0xee75de49daad1l,0x74fb26109b985l,0xbe0e32f5bc4fcl,
        0x05cf908d14f75l },
      { 0x435108e657b12l,0xa5b96ed9e6760l,0x970ccc2bfd421l,0x0ce20e29f51f8l,
        0x0a698ba4060f0l },
      0 },
    /* 134 */
    { { 0xb1686ef748fecl,0xa27e9d2cf973dl,0xe265effe6e755l,0xad8d630b6544cl,
        0x0b142ef8a7aebl },
      { 0x1af9f17d5770al,0x672cb3412fad3l,0xf3359de66af3bl,0x50756bd60d1bdl,
        0x0d1896a965851l },
      0 },
    /* 135 */
    { { 0x957ab33c41c08l,0xac5468e2e1ec5l,0xc472f6c87de94l,0xda3918816b73al,
        0x0267b0e0b7981l },
      { 0x54e5d8e62b988l,0x55116d21e76e5l,0xd2a6f99d8ddc7l,0x93934610faf03l,
        0x0b54e287aa111l },
      0 },
    /* 136 */
    { { 0x122b5178a876bl,0xff085104b40a0l,0x4f29f7651ff96l,0xd4e6050b31ab1l,
        0x084abb28b5f87l },
      { 0xd439f8270790al,0x9d85e3f46bd5el,0xc1e22122d6cb5l,0x564075f55c1b6l,
        0x0e5436f671765l },
      0 },
    /* 137 */
    { { 0x9025e2286e8d5l,0xb4864453be53fl,0x408e3a0353c95l,0xe99ed832f5bdel,
        0x00404f68b5b9cl },
      { 0x33bdea781e8e5l,0x18163c2f5bcadl,0x119caa33cdf50l,0xc701575769600l,
        0x03a4263df0ac1l },
      0 },
    /* 138 */
    { { 0x65ecc9aeb596dl,0xe7023c92b4c29l,0xe01396101ea03l,0xa3674704b4b62l,
        0x00ca8fd3f905el },
      { 0x23a42551b2b61l,0x9c390fcd06925l,0x392a63e1eb7a8l,0x0c33e7f1d2be0l,
        0x096dca2644ddbl },
      0 },
    /* 139 */
    { { 0xbb43a387510afl,0xa8a9a36a01203l,0xf950378846feal,0x59dcd23a57702l,
        0x04363e2123aadl },
      { 0x3a1c740246a47l,0xd2e55dd24dca4l,0xd8faf96b362b8l,0x98c4f9b086045l,
        0x0840e115cd8bbl },
      0 },
    /* 140 */
    { { 0x205e21023e8a7l,0xcdd8dc7a0bf12l,0x63a5ddfc808a8l,0xd6d4e292a2721l,
        0x05e0d6abd30del },
      { 0x721c27cfc0f64l,0x1d0e55ed8807al,0xd1f9db242eec0l,0xa25a26a7bef91l,
        0x07dea48f42945l },
      0 },
    /* 141 */
    { { 0xf6f1ce5060a81l,0x72f8f95615abdl,0x6ac268be79f9cl,0x16d1cfd36c540l,
        0x0abc2a2beebfdl },
      { 0x66f91d3e2eac7l,0x63d2dd04668acl,0x282d31b6f10bal,0xefc16790e3770l,
        0x04ea353946c7el },
      0 },
    /* 142 */
    { { 0xa2f8d5266309dl,0xc081945a3eed8l,0x78c5dc10a51c6l,0xffc3cecaf45a5l,
        0x03a76e6891c94l },
      { 0xce8a47d7b0d0fl,0x968f584a5f9aal,0xe697fbe963acel,0x646451a30c724l,
        0x08212a10a465el },
      0 },
    /* 143 */
    { { 0xc61c3cfab8caal,0x840e142390ef7l,0xe9733ca18eb8el,0xb164cd1dff677l,
        0x0aa7cab71599cl },
      { 0xc9273bc837bd1l,0xd0c36af5d702fl,0x423da49c06407l,0x17c317621292fl,
        0x040e38073fe06l },
      0 },
    /* 144 */
    { { 0x80824a7bf9b7cl,0x203fbe30d0f4fl,0x7cf9ce3365d23l,0x5526bfbe53209l,
        0x0e3604700b305l },
      { 0xb99116cc6c2c7l,0x08ba4cbee64dcl,0x37ad9ec726837l,0xe15fdcded4346l,
        0x06542d677a3del },
      0 },
    /* 145 */
    { { 0x2b6d07b6c377al,0x47903448be3f3l,0x0da8af76cb038l,0x6f21d6fdd3a82l,
        0x0a6534aee09bbl },
      { 0x1780d1035facfl,0x339dcb47e630al,0x447f39335e55al,0xef226ea50fe1cl,
        0x0f3cb672fdc9al },
      0 },
    /* 146 */
    { { 0x719fe3b55fd83l,0x6c875ddd10eb3l,0x5cea784e0d7a4l,0x70e733ac9fa90l,
        0x07cafaa2eaae8l },
      { 0x14d041d53b338l,0xa0ef87e6c69b8l,0x1672b0fe0acc0l,0x522efb93d1081l,
        0x00aab13c1b9bdl },
      0 },
    /* 147 */
    { { 0xce278d2681297l,0xb1b509546addcl,0x661aaf2cb350el,0x12e92dc431737l,
        0x04b91a6028470l },
      { 0xf109572f8ddcfl,0x1e9a911af4dcfl,0x372430e08ebf6l,0x1cab48f4360acl,
        0x049534c537232l },
      0 },
    /* 148 */
    { { 0xf7d71f07b7e9dl,0xa313cd516f83dl,0xc047ee3a478efl,0xc5ee78ef264b6l,
        0x0caf46c4fd65al },
      { 0xd0c7792aa8266l,0x66913684bba04l,0xe4b16b0edf454l,0x770f56e65168al,
        0x014ce9e5704c6l },
      0 },
    /* 149 */
    { { 0x45e3e965e8f91l,0xbacb0f2492994l,0x0c8a0a0d3aca1l,0x9a71d31cc70f9l,
        0x01bb708a53e4cl },
      { 0xa9e69558bdd7al,0x08018a26b1d5cl,0xc9cf1ec734a05l,0x0102b093aa714l,
        0x0f9d126f2da30l },
      0 },
    /* 150 */
    { { 0xbca7aaff9563el,0xfeb49914a0749l,0xf5f1671dd077al,0xcc69e27a0311bl,
        0x0807afcb9729el },
      { 0xa9337c9b08b77l,0x85443c7e387f8l,0x76fd8ba86c3a7l,0xcd8c85fafa594l,
        0x0751adcd16568l },
      0 },
    /* 151 */
    { { 0xa38b410715c0dl,0x718f7697f78ael,0x3fbf06dd113eal,0x743f665eab149l,
        0x029ec44682537l },
      { 0x4719cb50bebbcl,0xbfe45054223d9l,0xd2dedb1399ee5l,0x077d90cd5b3a8l,
        0x0ff9370e392a4l },
      0 },
    /* 152 */
    { { 0x2d69bc6b75b65l,0xd5266651c559al,0xde9d7d24188f8l,0xd01a28a9f33e3l,
        0x09776478ba2a9l },
      { 0x2622d929af2c7l,0x6d4e690923885l,0x89a51e9334f5dl,0x82face6cc7e5al,
        0x074a6313fac2fl },
      0 },
    /* 153 */
    { { 0x4dfddb75f079cl,0x9518e36fbbb2fl,0x7cd36dd85b07cl,0x863d1b6cfcf0el,
        0x0ab75be150ff4l },
      { 0x367c0173fc9b7l,0x20d2594fd081bl,0x4091236b90a74l,0x59f615fdbf03cl,
        0x04ebeac2e0b44l },
      0 },
    /* 154 */
    { { 0xc5fe75c9f2c53l,0x118eae9411eb6l,0x95ac5d8d25220l,0xaffcc8887633fl,
        0x0df99887b2c1bl },
      { 0x8eed2850aaecbl,0x1b01d6a272bb7l,0x1cdbcac9d4918l,0x4058978dd511bl,
        0x027b040a7779fl },
      0 },
    /* 155 */
    { { 0x05db7f73b2eb2l,0x088e1b2118904l,0x962327ee0df85l,0xa3f5501b71525l,
        0x0b393dd37e4cfl },
      { 0x30e7b3fd75165l,0xc2bcd33554a12l,0xf7b5022d66344l,0x34196c36f1be0l,
        0x009588c12d046l },
      0 },
    /* 156 */
    { { 0x6093f02601c3bl,0xf8cf5c335fe08l,0x94aff28fb0252l,0x648b955cf2808l,
        0x081c879a9db9fl },
      { 0xe687cc6f56c51l,0x693f17618c040l,0x059353bfed471l,0x1bc444f88a419l,
        0x0fa0d48f55fc1l },
      0 },
    /* 157 */
    { { 0xe1c9de1608e4dl,0x113582822cbc6l,0x57ec2d7010ddal,0x67d6f6b7ddc11l,
        0x08ea0e156b6a3l },
      { 0x4e02f2383b3b4l,0x943f01f53ca35l,0xde03ca569966bl,0xb5ac4ff6632b2l,
        0x03f5ab924fa00l },
      0 },
    /* 158 */
    { { 0xbb0d959739efbl,0xf4e7ebec0d337l,0x11a67d1c751b0l,0x256e2da52dd64l,
        0x08bc768872b74l },
      { 0xe3b7282d3d253l,0xa1f58d779fa5bl,0x16767bba9f679l,0xf34fa1cac168el,
        0x0b386f19060fcl },
      0 },
    /* 159 */
    { { 0x3c1352fedcfc2l,0x6262f8af0d31fl,0x57288c25396bfl,0x9c4d9a02b4eael,
        0x04cb460f71b06l },
      { 0x7b4d35b8095eal,0x596fc07603ae6l,0x614a16592bbf8l,0x5223e1475f66bl,
        0x052c0d50895efl },
      0 },
    /* 160 */
    { { 0xc210e15339848l,0xe870778c8d231l,0x956e170e87a28l,0x9c0b9d1de6616l,
        0x04ac3c9382bb0l },
      { 0xe05516998987dl,0xc4ae09f4d619bl,0xa3f933d8b2376l,0x05f41de0b7651l,
        0x0380d94c7e397l },
      0 },
    /* 161 */
    { { 0x355aa81542e75l,0xa1ee01b9b701al,0x24d708796c724l,0x37af6b3a29776l,
        0x02ce3e171de26l },
      { 0xfeb49f5d5bc1al,0x7e2777e2b5cfel,0x513756ca65560l,0x4e4d4feaac2f9l,
        0x02e6cd8520b62l },
      0 },
    /* 162 */
    { { 0x5954b8c31c31dl,0x005bf21a0c368l,0x5c79ec968533dl,0x9d540bd7626e7l,
        0x0ca17754742c6l },
      { 0xedafff6d2dbb2l,0xbd174a9d18cc6l,0xa4578e8fd0d8cl,0x2ce6875e8793al,
        0x0a976a7139cabl },
      0 },
    /* 163 */
    { { 0x51f1b93fb353dl,0x8b57fcfa720a6l,0x1b15281d75cabl,0x4999aa88cfa73l,
        0x08720a7170a1fl },
      { 0xe8d37693e1b90l,0x0b16f6dfc38c3l,0x52a8742d345dcl,0x893c8ea8d00abl,
        0x09719ef29c769l },
      0 },
    /* 164 */
    { { 0xeed8d58e35909l,0xdc33ddc116820l,0xe2050269366d8l,0x04c1d7f999d06l,
        0x0a5072976e157l },
      { 0xa37eac4e70b2el,0x576890aa8a002l,0x45b2a5c84dcf6l,0x7725cd71bf186l,
        0x099389c9df7b7l },
      0 },
    /* 165 */
    { { 0xc08f27ada7a4bl,0x03fd389366238l,0x66f512c3abe9dl,0x82e46b672e897l,
        0x0a88806aa202cl },
      { 0x2044ad380184el,0xc4126a8b85660l,0xd844f17a8cb78l,0xdcfe79d670c0al,
        0x00043bffb4738l },
      0 },
    /* 166 */
    { { 0x9b5dc36d5192el,0xd34590b2af8d5l,0x1601781acf885l,0x486683566d0a1l,
        0x052f3ef01ba6cl },
      { 0x6732a0edcb64dl,0x238068379f398l,0x040f3090a482cl,0x7e7516cbe5fa7l,
        0x03296bd899ef2l },
      0 },
    /* 167 */
    { { 0xaba89454d81d7l,0xef51eb9b3c476l,0x1c579869eade7l,0x71e9619a21cd8l,
        0x03b90febfaee5l },
      { 0x3023e5496f7cbl,0xd87fb51bc4939l,0x9beb5ce55be41l,0x0b1803f1dd489l,
        0x06e88069d9f81l },
      0 },
    /* 168 */
    { { 0x7ab11b43ea1dbl,0xa95259d292ce3l,0xf84f1860a7ff1l,0xad13851b02218l,
        0x0a7222beadefal },
      { 0xc78ec2b0a9144l,0x51f2fa59c5a2al,0x147ce385a0240l,0xc69091d1eca56l,
        0x0be94d523bc2al },
      0 },
    /* 169 */
    { { 0x4945e0b226ce7l,0x47967e8b7072fl,0x5a6c63eb8afd7l,0xc766edea46f18l,
        0x07782defe9be8l },
      { 0xd2aa43db38626l,0x8776f67ad1760l,0x4499cdb460ae7l,0x2e4b341b86fc5l,
        0x003838567a289l },
      0 },
    /* 170 */
    { { 0xdaefd79ec1a0fl,0xfdceb39c972d8l,0x8f61a953bbcd6l,0xb420f5575ffc5l,
        0x0dbd986c4adf7l },
      { 0xa881415f39eb7l,0xf5b98d976c81al,0xf2f717d6ee2fcl,0xbbd05465475dcl,
        0x08e24d3c46860l },
      0 },
    /* 171 */
    { { 0xd8e549a587390l,0x4f0cbec588749l,0x25983c612bb19l,0xafc846e07da4bl,
        0x0541a99c4407bl },
      { 0x41692624c8842l,0x2ad86c05ffdb2l,0xf7fcf626044c1l,0x35d1c59d14b44l,
        0x0c0092c49f57dl },
      0 },
    /* 172 */
    { { 0xc75c3df2e61efl,0xc82e1b35cad3cl,0x09f29f47e8841l,0x944dc62d30d19l,
        0x075e406347286l },
      { 0x41fc5bbc237d0l,0xf0ec4f01c9e7dl,0x82bd534c9537bl,0x858691c51a162l,
        0x05b7cb658c784l },
      0 },
    /* 173 */
    { { 0xa70848a28ead1l,0x08fd3b47f6964l,0x67e5b39802dc5l,0x97a19ae4bfd17l,
        0x07ae13eba8df0l },
      { 0x16ef8eadd384el,0xd9b6b2ff06fd2l,0xbcdb5f30361a2l,0xe3fd204b98784l,
        0x0787d8074e2a8l },
      0 },
    /* 174 */
    { { 0x25d6b757fbb1cl,0xb2ca201debc5el,0xd2233ffe47bddl,0x84844a55e9a36l,
        0x05c2228199ef2l },
      { 0xd4a8588315250l,0x2b827097c1773l,0xef5d33f21b21al,0xf2b0ab7c4ea1dl,
        0x0e45d37abbaf0l },
      0 },
    /* 175 */
    { { 0xf1e3428511c8al,0xc8bdca6cd3d2dl,0x27c39a7ebb229l,0xb9d3578a71a76l,
        0x0ed7bc12284dfl },
      { 0x2a6df93dea561l,0x8dd48f0ed1cf2l,0xbad23e85443f1l,0x6d27d8b861405l,
        0x0aac97cc945cal },
      0 },
    /* 176 */
    { { 0x4ea74a16bd00al,0xadf5c0bcc1eb5l,0xf9bfc06d839e9l,0xdc4e092bb7f11l,
        0x0318f97b31163l },
      { 0x0c5bec30d7138l,0x23abc30220eccl,0x022360644e8dfl,0xff4d2bb7972fbl,
        0x0fa41faa19a84l },
      0 },
    /* 177 */
    { { 0x2d974a6642269l,0xce9bb783bd440l,0x941e60bc81814l,0xe9e2398d38e47l,
        0x038bb6b2c1d26l },
      { 0xe4a256a577f87l,0x53dc11fe1cc64l,0x22807288b52d2l,0x01a5ff336abf6l,
        0x094dd0905ce76l },
      0 },
    /* 178 */
    { { 0xcf7dcde93f92al,0xcb89b5f315156l,0x995e750a01333l,0x2ae902404df9cl,
        0x092077867d25cl },
      { 0x71e010bf39d44l,0x2096bb53d7e24l,0xc9c3d8f5f2c90l,0xeb514c44b7b35l,
        0x081e8428bd29bl },
      0 },
    /* 179 */
    { { 0x9c2bac477199fl,0xee6b5ecdd96ddl,0xe40fd0e8cb8eel,0xa4b18af7db3fel,
        0x01b94ab62dbbfl },
      { 0x0d8b3ce47f143l,0xfc63f4616344fl,0xc59938351e623l,0x90eef18f270fcl,
        0x006a38e280555l },
      0 },
    /* 180 */
    { { 0xb0139b3355b49l,0x60b4ebf99b2e5l,0x269f3dc20e265l,0xd4f8c08ffa6bdl,
        0x0a7b36c2083d9l },
      { 0x15c3a1b3e8830l,0xe1a89f9c0b64dl,0x2d16930d5fceal,0x2a20cfeee4a2el,
        0x0be54c6b4a282l },
      0 },
    /* 181 */
    { { 0xdb3df8d91167cl,0x79e7a6625ed6cl,0x46ac7f4517c3fl,0x22bb7105648f3l,
        0x0bf30a5abeae0l },
      { 0x785be93828a68l,0x327f3ef0368e7l,0x92146b25161c3l,0xd13ae11b5feb5l,
        0x0d1c820de2732l },
      0 },
    /* 182 */
    { { 0xe13479038b363l,0x546b05e519043l,0x026cad158c11fl,0x8da34fe57abe6l,
        0x0b7d17bed68a1l },
      { 0xa5891e29c2559l,0x765bfffd8444cl,0x4e469484f7a03l,0xcc64498de4af7l,
        0x03997fd5e6412l },
      0 },
    /* 183 */
    { { 0x746828bd61507l,0xd534a64d2af20l,0xa8a15e329e132l,0x13e8ffeddfb08l,
        0x00eeb89293c6cl },
      { 0x69a3ea7e259f8l,0xe6d13e7e67e9bl,0xd1fa685ce1db7l,0xb6ef277318f6al,
        0x0228916f8c922l },
      0 },
    /* 184 */
    { { 0xae25b0a12ab5bl,0x1f957bc136959l,0x16e2b0ccc1117l,0x097e8058429edl,
        0x0ec05ad1d6e93l },
      { 0xba5beac3f3708l,0x3530b59d77157l,0x18234e531baf9l,0x1b3747b552371l,
        0x07d3141567ff1l },
      0 },
    /* 185 */
    { { 0x9c05cf6dfefabl,0x68dcb377077bdl,0xa38bb95be2f22l,0xd7a3e53ead973l,
        0x0e9ce66fc9bc1l },
      { 0xa15766f6a02a1l,0xdf60e600ed75al,0x8cdc1b938c087l,0x0651f8947f346l,
        0x0d9650b017228l },
      0 },
    /* 186 */
    { { 0xb4c4a5a057e60l,0xbe8def25e4504l,0x7c1ccbdcbccc3l,0xb7a2a63532081l,
        0x014d6699a804el },
      { 0xa8415db1f411al,0x0bf80d769c2c8l,0xc2f77ad09fbafl,0x598ab4deef901l,
        0x06f4c68410d43l },
      0 },
    /* 187 */
    { { 0x6df4e96c24a96l,0x85fcbd99a3872l,0xb2ae30a534dbcl,0x9abb3c466ef28l,
        0x04c4350fd6118l },
      { 0x7f716f855b8dal,0x94463c38a1296l,0xae9334341a423l,0x18b5c37e1413el,
        0x0a726d2425a31l },
      0 },
    /* 188 */
    { { 0x6b3ee948c1086l,0x3dcbd3a2e1dael,0x3d022f3f1de50l,0xf3923f35ed3f0l,
        0x013639e82cc6cl },
      { 0x938fbcdafaa86l,0xfb2654a2589acl,0x5051329f45bc5l,0x35a31963b26e4l,
        0x0ca9365e1c1a3l },
      0 },
    /* 189 */
    { { 0x5ac754c3b2d20l,0x17904e241b361l,0xc9d071d742a54l,0x72a5b08521c4cl,
        0x09ce29c34970bl },
      { 0x81f736d3e0ad6l,0x9ef2f8434c8ccl,0xce862d98060dal,0xaf9835ed1d1a6l,
        0x048c4abd7ab42l },
      0 },
    /* 190 */
    { { 0x1b0cc40c7485al,0xbbe5274dbfd22l,0x263d2e8ead455l,0x33cb493c76989l,
        0x078017c32f67bl },
      { 0x35769930cb5eel,0x940c408ed2b9dl,0x72f1a4dc0d14el,0x1c04f8b7bf552l,
        0x053cd0454de5cl },
      0 },
    /* 191 */
    { { 0x585fa5d28ccacl,0x56005b746ebcdl,0xd0123aa5f823el,0xfa8f7c79f0a1cl,
        0x0eea465c1d3d7l },
      { 0x0659f0551803bl,0x9f7ce6af70781l,0x9288e706c0b59l,0x91934195a7702l,
        0x01b6e42a47ae6l },
      0 },
    /* 192 */
    { { 0x0937cf67d04c3l,0xe289eeb8112e8l,0x2594d601e312bl,0xbd3d56b5d8879l,
        0x00224da14187fl },
      { 0xbb8630c5fe36fl,0x604ef51f5f87al,0x3b429ec580f3cl,0xff33964fb1bfbl,
        0x060838ef042bfl },
      0 },
    /* 193 */
    { { 0xcb2f27e0bbe99l,0xf304aa39ee432l,0xfa939037bda44l,0x16435f497c7a9l,
        0x0636eb2022d33l },
      { 0xd0e6193ae00aal,0xfe31ae6d2ffcfl,0xf93901c875a00l,0x8bacf43658a29l,
        0x08844eeb63921l },
      0 },
    /* 194 */
    { { 0x171d26b3bae58l,0x7117e39f3e114l,0x1a8eada7db3dfl,0x789ecd37bc7f8l,
        0x027ba83dc51fbl },
      { 0xf439ffbf54de5l,0x0bb5fe1a71a7dl,0xb297a48727703l,0xa4ab42ee8e35dl,
        0x0adb62d3487f3l },
      0 },
    /* 195 */
    { { 0x168a2a175df2al,0x4f618c32e99b1l,0x46b0916082aa0l,0xc8b2c9e4f2e71l,
        0x0b990fd7675e7l },
      { 0x9d96b4df37313l,0x79d0b40789082l,0x80877111c2055l,0xd18d66c9ae4a7l,
        0x081707ef94d10l },
      0 },
    /* 196 */
    { { 0x7cab203d6ff96l,0xfc0d84336097dl,0x042db4b5b851bl,0xaa5c268823c4dl,
        0x03792daead5a8l },
      { 0x18865941afa0bl,0x4142d83671528l,0xbe4e0a7f3e9e7l,0x01ba17c825275l,
        0x05abd635e94b0l },
      0 },
    /* 197 */
    { { 0xfa84e0ac4927cl,0x35a7c8cf23727l,0xadca0dfe38860l,0xb610a4bcd5ea4l,
        0x05995bf21846al },
      { 0xf860b829dfa33l,0xae958fc18be90l,0x8630366caafe2l,0x411e9b3baf447l,
        0x044c32ca2d483l },
      0 },
    /* 198 */
    { { 0xa97f1e40ed80cl,0xb131d2ca82a74l,0xc2d6ad95f938cl,0xa54c53f2124b7l,
        0x01f2162fb8082l },
      { 0x67cc5720b173el,0x66085f12f97e4l,0xc9d65dc40e8a6l,0x07c98cebc20e4l,
        0x08f1d402bc3e9l },
      0 },
    /* 199 */
    { { 0x92f9cfbc4058al,0xb6292f56704f5l,0xc1d8c57b15e14l,0xdbf9c55cfe37bl,
        0x0b1980f43926el },
      { 0x33e0932c76b09l,0x9d33b07f7898cl,0x63bb4611df527l,0x8e456f08ead48l,
        0x02828ad9b3744l },
      0 },
    /* 200 */
    { { 0x722c4c4cf4ac5l,0x3fdde64afb696l,0x0890832f5ac1al,0xb3900551baa2el,
        0x04973f1275a14l },
      { 0xd8335322eac5dl,0xf50bd9b568e59l,0x25883935e07eel,0x8ac7ab36720fal,
        0x06dac8ed0db16l },
      0 },
    /* 201 */
    { { 0x545aeeda835efl,0xd21d10ed51f7bl,0x3741b094aa113l,0xde4c035a65e01l,
        0x04b23ef5920b9l },
      { 0xbb6803c4c7341l,0x6d3f58bc37e82l,0x51e3ee8d45770l,0x9a4e73527863al,
        0x04dd71534ddf4l },
      0 },
    /* 202 */
    { { 0x4467295476cd9l,0x2fe31a725bbf9l,0xc4b67e0648d07l,0x4dbb1441c8b8fl,
        0x0fd3170002f4al },
      { 0x43ff48995d0e1l,0xd10ef729aa1cbl,0x179898276e695l,0xf365e0d5f9764l,
        0x014fac58c9569l },
      0 },
    /* 203 */
    { { 0xa0065f312ae18l,0xc0fcc93fc9ad9l,0xa7d284651958dl,0xda50d9a142408l,
        0x0ed7c765136abl },
      { 0x70f1a25d4abbcl,0xf3f1a113ea462l,0xb51952f9b5dd8l,0x9f53c609b0755l,
        0x0fefcb7f74d2el },
      0 },
    /* 204 */
    { { 0x9497aba119185l,0x30aac45ba4bd0l,0xa521179d54e8cl,0xd80b492479deal,
        0x01801a57e87e0l },
      { 0xd3f8dfcafffb0l,0x0bae255240073l,0xb5fdfbc6cf33cl,0x1064781d763b5l,
        0x09f8fc11e1eadl },
      0 },
    /* 205 */
    { { 0x3a1715e69544cl,0x67f04b7813158l,0x78a4c320eaf85l,0x69a91e22a8fd2l,
        0x0a9d3809d3d3al },
      { 0xc2c2c59a2da3bl,0xf61895c847936l,0x3d5086938ccbcl,0x8ef75e65244e6l,
        0x03006b9aee117l },
      0 },
    /* 206 */
    { { 0x1f2b0c9eead28l,0x5d89f4dfbc0bbl,0x2ce89397eef63l,0xf761074757fdbl,
        0x00ab85fd745f8l },
      { 0xa7c933e5b4549l,0x5c97922f21ecdl,0x43b80404be2bbl,0x42c2261a1274bl,
        0x0b122d67511e9l },
      0 },
    /* 207 */
    { { 0x607be66a5ae7al,0xfa76adcbe33bel,0xeb6e5c501e703l,0xbaecaf9043014l,
        0x09f599dc1097dl },
      { 0x5b7180ff250edl,0x74349a20dc6d7l,0x0b227a38eb915l,0x4b78425605a41l,
        0x07d5528e08a29l },
      0 },
    /* 208 */
    { { 0x58f6620c26defl,0xea582b2d1ef0fl,0x1ce3881025585l,0x1730fbe7d79b0l,
        0x028ccea01303fl },
      { 0xabcd179644ba5l,0xe806fff0b8d1dl,0x6b3e17b1fc643l,0x13bfa60a76fc6l,
        0x0c18baf48a1d0l },
      0 },
    /* 209 */
    { { 0x638c85dc4216dl,0x67206142ac34el,0x5f5064a00c010l,0x596bd453a1719l,
        0x09def809db7a9l },
      { 0x8642e67ab8d2cl,0x336237a2b641el,0x4c4218bb42404l,0x8ce57d506a6d6l,
        0x00357f8b06880l },
      0 },
    /* 210 */
    { { 0xdbe644cd2cc88l,0x8df0b8f39d8e9l,0xd30a0c8cc61c2l,0x98874a309874cl,
        0x0e4a01add1b48l },
      { 0x1eeacf57cd8f9l,0x3ebd594c482edl,0xbd2f7871b767dl,0xcc30a7295c717l,
        0x0466d7d79ce10l },
      0 },
    /* 211 */
    { { 0x318929dada2c7l,0xc38f9aa27d47dl,0x20a59e14fa0a6l,0xad1a90e4fd288l,
        0x0c672a522451el },
      { 0x07cc85d86b655l,0x3bf9ad4af1306l,0x71172a6f0235dl,0x751399a086805l,
        0x05e3d64faf2a6l },
      0 },
    /* 212 */
    { { 0x410c79b3b4416l,0x85eab26d99aa6l,0xb656a74cd8fcfl,0x42fc5ebff74adl,
        0x06c8a7a95eb8el },
      { 0x60ba7b02a63bdl,0x038b8f004710cl,0x12d90b06b2f23l,0xca918c6c37383l,
        0x0348ae422ad82l },
      0 },
    /* 213 */
    { { 0x746635ccda2fbl,0xa18e0726d27f4l,0x92b1f2022accal,0x2d2e85adf7824l,
        0x0c1074de0d9efl },
      { 0x3ce44ae9a65b3l,0xac05d7151bfcfl,0xe6a9788fd71e4l,0x4ffcd4711f50cl,
        0x0fbadfbdbc9e5l },
      0 },
    /* 214 */
    { { 0x3f1cd20a99363l,0x8f6cf22775171l,0x4d359b2b91565l,0x6fcd968175cd2l,
        0x0b7f976b48371l },
      { 0x8e24d5d6dbf74l,0xfd71c3af36575l,0x243dfe38d23bal,0xc80548f477600l,
        0x0f4d41b2ecafcl },
      0 },
    /* 215 */
    { { 0x1cf28fdabd48dl,0x3632c078a451fl,0x17146e9ce81bel,0x0f106ace29741l,
        0x0180824eae016l },
      { 0x7698b66e58358l,0x52ce6ca358038l,0xe41e6c5635687l,0x6d2582380e345l,
        0x067e5f63983cfl },
      0 },
    /* 216 */
    { { 0xccb8dcf4899efl,0xf09ebb44c0f89l,0x2598ec9949015l,0x1fc6546f9276bl,
        0x09fef789a04c1l },
      { 0x67ecf53d2a071l,0x7fa4519b096d3l,0x11e2eefb10e1al,0x4e20ca6b3fb06l,
        0x0bc80c181a99cl },
      0 },
    /* 217 */
    { { 0x536f8e5eb82e6l,0xc7f56cb920972l,0x0b5da5e1a484fl,0xdf10c78e21715l,
        0x049270e629f8cl },
      { 0x9b7bbea6b50adl,0xc1a2388ffc1a3l,0x107197b9a0284l,0x2f7f5403eb178l,
        0x0d2ee52f96137l },
      0 },
    /* 218 */
    { { 0xcd28588e0362al,0xa78fa5d94dd37l,0x434a526442fa8l,0xb733aff836e5al,
        0x0dfb478bee5abl },
      { 0xf1ce7673eede6l,0xd42b5b2f04a91l,0x530da2fa5390al,0x473a5e66f7bf5l,
        0x0d9a140b408dfl },
      0 },
    /* 219 */
    { { 0x221b56e8ea498l,0x293563ee090e0l,0x35d2ade623478l,0x4b1ae06b83913l,
        0x0760c058d623fl },
      { 0x9b58cc198aa79l,0xd2f07aba7f0b8l,0xde2556af74890l,0x04094e204110fl,
        0x07141982d8f19l },
      0 },
    /* 220 */
    { { 0xa0e334d4b0f45l,0x38392a94e16f0l,0x3c61d5ed9280bl,0x4e473af324c6bl,
        0x03af9d1ce89d5l },
      { 0xf798120930371l,0x4c21c17097fd8l,0xc42309beda266l,0x7dd60e9545dcdl,
        0x0b1f815c37395l },
      0 },
    /* 221 */
    { { 0xaa78e89fec44al,0x473caa4caf84fl,0x1b6a624c8c2ael,0xf052691c807dcl,
        0x0a41aed141543l },
      { 0x353997d5ffe04l,0xdf625b6e20424l,0x78177758bacb2l,0x60ef85d660be8l,
        0x0d6e9c1dd86fbl },
      0 },
    /* 222 */
    { { 0x2e97ec6853264l,0xb7e2304a0b3aal,0x8eae9be771533l,0xf8c21b912bb7bl,
        0x09c9c6e10ae9bl },
      { 0x09a59e030b74cl,0x4d6a631e90a23l,0x49b79f24ed749l,0x61b689f44b23al,
        0x0566bd59640fal },
      0 },
    /* 223 */
    { { 0xc0118c18061f3l,0xd37c83fc70066l,0x7273245190b25l,0x345ef05fc8e02l,
        0x0cf2c7390f525l },
      { 0xbceb410eb30cfl,0xba0d77703aa09l,0x50ff255cfd2ebl,0x0979e842c43a1l,
        0x002f517558aa2l },
      0 },
    /* 224 */
    { { 0xef794addb7d07l,0x4224455500396l,0x78aa3ce0b4fc7l,0xd97dfaff8eaccl,
        0x014e9ada5e8d4l },
      { 0x480a12f7079e2l,0xcde4b0800edaal,0x838157d45baa3l,0x9ae801765e2d7l,
        0x0a0ad4fab8e9dl },
      0 },
    /* 225 */
    { { 0xb76214a653618l,0x3c31eaaa5f0bfl,0x4949d5e187281l,0xed1e1553e7374l,
        0x0bcd530b86e56l },
      { 0xbe85332e9c47bl,0xfeb50059ab169l,0x92bfbb4dc2776l,0x341dcdba97611l,
        0x0909283cf6979l },
      0 },
    /* 226 */
    { { 0x0032476e81a13l,0x996217123967bl,0x32e19d69bee1al,0x549a08ed361bdl,
        0x035eeb7c9ace1l },
      { 0x0ae5a7e4e5bdcl,0xd3b6ceec6e128l,0xe266bc12dcd2cl,0xe86452e4224c6l,
        0x09a8b2cf4448al },
      0 },
    /* 227 */
    { { 0x71bf209d03b59l,0xa3b65af2abf64l,0xbd5eec9c90e62l,0x1379ff7ff168el,
        0x06bdb60f4d449l },
      { 0xafebc8a55bc30l,0x1610097fe0dadl,0xc1e3bddc79eadl,0x08a942e197414l,
        0x001ec3cfd94bal },
      0 },
    /* 228 */
    { { 0x277ebdc9485c2l,0x7922fb10c7ba6l,0x0a28d8a48cc9al,0x64f64f61d60f7l,
        0x0d1acb1c04754l },
      { 0x902b126f36612l,0x4ee0618d8bd26l,0x08357ee59c3a4l,0x26c24df8a8133l,
        0x07dcd079d4056l },
      0 },
    /* 229 */
    { { 0x7d4d3f05a4b48l,0x52372307725cel,0x12a915aadcd29l,0x19b8d18f79718l,
        0x00bf53589377dl },
      { 0xcd95a6c68ea73l,0xca823a584d35el,0x473a723c7f3bbl,0x86fc9fb674c6fl,
        0x0d28be4d9e166l },
      0 },
    /* 230 */
    { { 0xb990638fa8e4bl,0x6e893fd8fc5d2l,0x36fb6fc559f18l,0x88ce3a6de2aa4l,
        0x0d76007aa510fl },
      { 0x0aab6523a4988l,0x4474dd02732d1l,0x3407278b455cfl,0xbb017f467082al,
        0x0f2b52f68b303l },
      0 },
    /* 231 */
    { { 0x7eafa9835b4cal,0xfcbb669cbc0d5l,0x66431982d2232l,0xed3a8eeeb680cl,
        0x0d8dbe98ecc5al },
      { 0x9be3fc5a02709l,0xe5f5ba1fa8cbal,0x10ea85230be68l,0x9705febd43cdfl,
        0x0e01593a3ee55l },
      0 },
    /* 232 */
    { { 0x5af50ea75a0a6l,0xac57858033d3el,0x0176406512226l,0xef066fe6d50fdl,
        0x0afec07b1aeb8l },
      { 0x9956780bb0a31l,0xcc37309aae7fbl,0x1abf3896f1af3l,0xbfdd9153a15a0l,
        0x0a71b93546e2dl },
      0 },
    /* 233 */
    { { 0xe12e018f593d2l,0x28a078122bbf8l,0xba4f2add1a904l,0x23d9150505db0l,
        0x053a2005c6285l },
      { 0x8b639e7f2b935l,0x5ac182961a07cl,0x518ca2c2bff97l,0x8e3d86bceea77l,
        0x0bf47d19b3d58l },
      0 },
    /* 234 */
    { { 0x967a7dd7665d5l,0x572f2f4de5672l,0x0d4903f4e3030l,0xa1b6144005ae8l,
        0x0001c2c7f39c9l },
      { 0xa801469efc6d6l,0xaa7bc7a724143l,0x78150a4c810bdl,0xb99b5f65670bal,
        0x0fdadf8e786ffl },
      0 },
    /* 235 */
    { { 0x8cb88ffc00785l,0x913b48eb67fd3l,0xf368fbc77fa75l,0x3c940454d055bl,
        0x03a838e4d5aa4l },
      { 0x663293e97bb9al,0x63441d94d9561l,0xadb2a839eb933l,0x1da3515591a60l,
        0x03cdb8257873el },
      0 },
    /* 236 */
    { { 0x140a97de77eabl,0x0d41648109137l,0xeb1d0dff7e1c5l,0x7fba762dcad2cl,
        0x05a60cc89f1f5l },
      { 0x3638240d45673l,0x195913c65580bl,0xd64b7411b82bel,0x8fc0057284b8dl,
        0x0922ff56fdbfdl },
      0 },
    /* 237 */
    { { 0x65deec9a129a1l,0x57cc284e041b2l,0xebfbe3ca5b1cel,0xcd6204380c46cl,
        0x072919a7df6c5l },
      { 0xf453a8fb90f9al,0x0b88e4031b298l,0x96f1856d719c0l,0x089ae32c0e777l,
        0x05e7917803624l },
      0 },
    /* 238 */
    { { 0x6ec557f63cdfbl,0x71f1cae4fd5c1l,0x60597ca8e6a35l,0x2fabfce26bea5l,
        0x04e0a5371e24cl },
      { 0xa40d3a5765357l,0x440d73a2b4276l,0x1d11a323c89afl,0x04eeb8f370ae4l,
        0x0f5ff7818d566l },
      0 },
    /* 239 */
    { { 0x3e3fe1a09df21l,0x8ee66e8e47fbfl,0x9c8901526d5d2l,0x5e642096bd0a2l,
        0x0e41df0e9533fl },
      { 0xfda40b3ba9e3fl,0xeb2604d895305l,0xf0367c7f2340cl,0x155f0866e1927l,
        0x08edd7d6eac4fl },
      0 },
    /* 240 */
    { { 0x1dc0e0bfc8ff3l,0x2be936f42fc9al,0xca381ef14efd8l,0xee9667016f7ccl,
        0x01432c1caed8al },
      { 0x8482970b23c26l,0x730735b273ec6l,0xaef0f5aa64fe8l,0xd2c6e389f6e5el,
        0x0caef480b5ac8l },
      0 },
    /* 241 */
    { { 0x5c97875315922l,0x713063cca5524l,0x64ef2cbd82951l,0xe236f3ce60d0bl,
        0x0d0ba177e8efal },
      { 0x9ae8fb1b3af60l,0xe53d2da20e53al,0xf9eef281a796al,0xae1601d63605dl,
        0x0f31c957c1c54l },
      0 },
    /* 242 */
    { { 0x58d5249cc4597l,0xb0bae0a028c0fl,0x34a814adc5015l,0x7c3aefc5fc557l,
        0x0013404cb96e1l },
      { 0xe2585c9a824bfl,0x5e001eaed7b29l,0x1ef68acd59318l,0x3e6c8d6ee6826l,
        0x06f377c4b9193l },
      0 },
    /* 243 */
    { { 0x3bad1a8333fd2l,0x025a2a95b89f9l,0xaf75acea89302l,0x9506211e5037el,
        0x06dba3e4ed2d0l },
      { 0xef98cd04399cdl,0x6ee6b73adea48l,0x17ecaf31811c6l,0xf4a772f60752cl,
        0x0f13cf3423becl },
      0 },
    /* 244 */
    { { 0xb9ec0a919e2ebl,0x95f62c0f68ceel,0xaba229983a9a1l,0xbad3cfba3bb67l,
        0x0c83fa9a9274bl },
      { 0xd1b0b62fa1ce0l,0xf53418efbf0d7l,0x2706f04e58b60l,0x2683bfa8ef9e5l,
        0x0b49d70f45d70l },
      0 },
    /* 245 */
    { { 0xc7510fad5513bl,0xecb1751e2d914l,0x9fb9d5905f32el,0xf1cf6d850418dl,
        0x059cfadbb0c30l },
      { 0x7ac2355cb7fd6l,0xb8820426a3e16l,0x0a78864249367l,0x4b67eaeec58c9l,
        0x05babf362354al },
      0 },
    /* 246 */
    { { 0x981d1ee424865l,0x78f2e5577f37cl,0x9e0c0588b0028l,0xc8f0702970f1bl,
        0x06188c6a79026l },
      { 0x9a19bd0f244dal,0x5cfb08087306fl,0xf2136371eccedl,0xb9d935470f9b9l,
        0x0993fe475df50l },
      0 },
    /* 247 */
    { { 0x31cdf9b2c3609l,0xc02c46d4ea68el,0xa77510184eb19l,0x616b7ac9ec1a9l,
        0x081f764664c80l },
      { 0xc2a5a75fbe978l,0xd3f183b3561d7l,0x01dd2bf6743fel,0x060d838d1f045l,
        0x0564a812a5fe9l },
      0 },
    /* 248 */
    { { 0xa64f4fa817d1dl,0x44bea82e0f7a5l,0xd57f9aa55f968l,0x1d6cb5ff5a0fcl,
        0x0226bf3cf00e5l },
      { 0x1a9f92f2833cfl,0x5a4f4f89a8d6dl,0xf3f7f7720a0a3l,0x783611536c498l,
        0x068779f47ff25l },
      0 },
    /* 249 */
    { { 0x0c1c173043d08l,0x741fc020fa79bl,0xa6d26d0a54467l,0x2e0bd3767e289l,
        0x097bcb0d1eb09l },
      { 0x6eaa8f32ed3c3l,0x51b281bc482abl,0xfa178f3c8a4f1l,0x46554d1bf4f3bl,
        0x0a872ffe80a78l },
      0 },
    /* 250 */
    { { 0xb7935a32b2086l,0x0e8160f486b1al,0xb6ae6bee1eb71l,0xa36a9bd0cd913l,
        0x002812bfcb732l },
      { 0xfd7cacf605318l,0x50fdfd6d1da63l,0x102d619646e5dl,0x96afa1d683982l,
        0x007391cc9fe53l },
      0 },
    /* 251 */
    { { 0x157f08b80d02bl,0xd162877f7fc50l,0x8d542ae6b8333l,0x2a087aca1af87l,
        0x0355d2adc7e6dl },
      { 0xf335a287386e1l,0x94f8e43275b41l,0x79989eafd272al,0x3a79286ca2cdel,
        0x03dc2b1e37c2al },
      0 },
    /* 252 */
    { { 0x9d21c04581352l,0x25376782bed68l,0xfed701f0a00c8l,0x846b203bd5909l,
        0x0c47869103ccdl },
      { 0xa770824c768edl,0x026841f6575dbl,0xaccce0e72feeal,0x4d3273313ed56l,
        0x0ccc42968d5bbl },
      0 },
    /* 253 */
    { { 0x50de13d7620b9l,0x8a5992a56a94el,0x75487c9d89a5cl,0x71cfdc0076406l,
        0x0e147eb42aa48l },
      { 0xab4eeacf3ae46l,0xfb50350fbe274l,0x8c840eafd4936l,0x96e3df2afe474l,
        0x0239ac047080el },
      0 },
    /* 254 */
    { { 0xd1f352bfee8d4l,0xcffa7b0fec481l,0xce9af3cce80b5l,0xe59d105c4c9e2l,
        0x0c55fa1a3f5f7l },
      { 0x6f14e8257c227l,0x3f342be00b318l,0xa904fb2c5b165l,0xb69909afc998al,
        0x0094cd99cd4f4l },
      0 },
    /* 255 */
    { { 0x81c84d703bebal,0x5032ceb2918a9l,0x3bd49ec8631d1l,0xad33a445f2c9el,
        0x0b90a30b642abl },
      { 0x5404fb4a5abf9l,0xc375db7603b46l,0xa35d89f004750l,0x24f76f9a42cccl,
        0x0019f8b9a1b79l },
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
static int sp_256_ecc_mulmod_base_5(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    return sp_256_ecc_mulmod_stripe_5(r, &p256_base, p256_table,
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
    sp_digit kd[5];
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
        k = XMALLOC(sizeof(sp_digit) * 5, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 5, km);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_5(point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_5(point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_5(point, r);

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
static int sp_256_iszero_5(const sp_digit* a)
{
    return (a[0] | a[1] | a[2] | a[3] | a[4]) == 0;
}

#endif /* WOLFSSL_VALIDATE_ECC_KEYGEN || HAVE_ECC_SIGN */
/* Add 1 to a. (a = a + 1)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_add_one_5(sp_digit* a)
{
    a[0]++;
    sp_256_norm_5(a);
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
        if (s >= 44) {
            r[j] &= 0xfffffffffffffl;
            s = 52 - s;
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
static int sp_256_ecc_gen_k_5(WC_RNG* rng, sp_digit* k)
{
    int err;
    byte buf[32];

    do {
        err = wc_RNG_GenerateBlock(rng, buf, sizeof(buf));
        if (err == 0) {
            sp_256_from_bin(k, 5, buf, sizeof(buf));
            if (sp_256_cmp_5(k, p256_order2) < 0) {
                sp_256_add_one_5(k);
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
    sp_digit kd[5];
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
        k = XMALLOC(sizeof(sp_digit) * 5, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY)
        err = sp_256_ecc_gen_k_5(rng, k);
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_5(point, k, 1, NULL);
        else
#endif
            err = sp_256_ecc_mulmod_base_5(point, k, 1, NULL);
    }

#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            err = sp_256_ecc_mulmod_avx2_5(infinity, point, p256_order, 1,
                                                                          NULL);
        }
        else
#endif
            err = sp_256_ecc_mulmod_5(infinity, point, p256_order, 1, NULL);
    }
    if (err == MP_OKAY) {
        if (!sp_256_iszero_5(point->x) || !sp_256_iszero_5(point->y))
            err = ECC_INF_E;
    }
#endif

    if (err == MP_OKAY)
        err = sp_256_to_mp(k, priv);
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_5(point, pub);

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

    for (i=0; i<4; i++) {
        r[i+1] += r[i] >> 52;
        r[i] &= 0xfffffffffffffl;
    }
    j = 256 / 8 - 1;
    a[j] = 0;
    for (i=0; i<5 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 52) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 52);
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
    sp_digit kd[5];
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
        k = XMALLOC(sizeof(sp_digit) * 5, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(k, 5, priv);
        sp_256_point_from_ecc_point_5(point, pub);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_5(point, point, k, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_5(point, point, k, 1, heap);
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
#ifdef HAVE_INTEL_AVX2
#endif /* HAVE_INTEL_AVX2 */
#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_256_mul_d_5(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int128_t tb = b;
    int128_t t = 0;
    int i;

    for (i = 0; i < 5; i++) {
        t += tb * a[i];
        r[i] = t & 0xfffffffffffffl;
        t >>= 52;
    }
    r[5] = (sp_digit)t;
#else
    int128_t tb = b;
    int128_t t[5];

    t[ 0] = tb * a[ 0];
    t[ 1] = tb * a[ 1];
    t[ 2] = tb * a[ 2];
    t[ 3] = tb * a[ 3];
    t[ 4] = tb * a[ 4];
    r[ 0] =                           (t[ 0] & 0xfffffffffffffl);
    r[ 1] = (sp_digit)(t[ 0] >> 52) + (t[ 1] & 0xfffffffffffffl);
    r[ 2] = (sp_digit)(t[ 1] >> 52) + (t[ 2] & 0xfffffffffffffl);
    r[ 3] = (sp_digit)(t[ 2] >> 52) + (t[ 3] & 0xfffffffffffffl);
    r[ 4] = (sp_digit)(t[ 3] >> 52) + (t[ 4] & 0xfffffffffffffl);
    r[ 5] = (sp_digit)(t[ 4] >> 52);
#endif /* WOLFSSL_SP_SMALL */
}

/* Divide d in a and put remainder into r (m*d + r = a)
 * m is not calculated as it is not needed at this time.
 *
 * a  Nmber to be divided.
 * d  Number to divide with.
 * m  Multiplier result.
 * r  Remainder from the division.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_256_div_5(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int128_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[10], t2d[5 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 5 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 5;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[4];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 5);
        for (i=4; i>=0; i--) {
            t1[5 + i] += t1[5 + i - 1] >> 52;
            t1[5 + i - 1] &= 0xfffffffffffffl;
            d1 = t1[5 + i];
            d1 <<= 52;
            d1 += t1[5 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_256_mul_d_5(t2, d, r1);
            sp_256_sub_5(&t1[i], &t1[i], t2);
            t1[5 + i] -= t2[5];
            t1[5 + i] += t1[5 + i - 1] >> 52;
            t1[5 + i - 1] &= 0xfffffffffffffl;
            r1 = (((-t1[5 + i]) << 52) - t1[5 + i - 1]) / div;
            r1++;
            sp_256_mul_d_5(t2, d, r1);
            sp_256_add_5(&t1[i], &t1[i], t2);
            t1[5 + i] += t1[5 + i - 1] >> 52;
            t1[5 + i - 1] &= 0xfffffffffffffl;
        }
        t1[5 - 1] += t1[5 - 2] >> 52;
        t1[5 - 2] &= 0xfffffffffffffl;
        d1 = t1[5 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_256_mul_d_5(t2, d, r1);
        sp_256_sub_5(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 5);
        for (i=0; i<3; i++) {
            r[i+1] += r[i] >> 52;
            r[i] &= 0xfffffffffffffl;
        }
        sp_256_cond_add_5(r, r, d, 0 - (r[4] < 0));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
}

/* Reduce a modulo m into r. (r = a mod m)
 *
 * r  A single precision number that is the reduced result.
 * a  A single precision number that is to be reduced.
 * m  A single precision number that is the modulus to reduce with.
 * returns MEMORY_E when unable to allocate memory and MP_OKAY otherwise.
 */
static int sp_256_mod_5(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_256_div_5(a, m, NULL, r);
}

#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
#ifdef WOLFSSL_SP_SMALL
/* Order-2 for the P256 curve. */
static const uint64_t p256_order_2[4] = {
    0xf3b9cac2fc63254f,0xbce6faada7179e84,0xffffffffffffffff,
    0xffffffff00000000
};
#else
/* The low half of the order-2 of the P256 curve. */
static const uint64_t p256_order_low[2] = {
    0xf3b9cac2fc63254f,0xbce6faada7179e84
};
#endif /* WOLFSSL_SP_SMALL */

/* Multiply two number mod the order of P256 curve. (r = a * b mod order)
 *
 * r  Result of the multiplication.
 * a  First operand of the multiplication.
 * b  Second operand of the multiplication.
 */
static void sp_256_mont_mul_order_5(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_5(r, a, b);
    sp_256_mont_reduce_5(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_5(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_5(r, a);
    sp_256_mont_reduce_5(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_5(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_5(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_5(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_5(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 5);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_5(t, t);
        if (p256_order_2[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_5(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 5);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 5;
    sp_digit* t3 = td + 4 * 5;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_5(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_5(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_5(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_5(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_5(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_5(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_5(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_5(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_5(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_5(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_5(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_5(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_5(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_5(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_5(t2, t2, 4);
    sp_256_mont_mul_order_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_5(t2, t2, 4);
    sp_256_mont_mul_order_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_5(t2, t2, 4);
    sp_256_mont_mul_order_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_5(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_5(r, t2, t3);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef HAVE_INTEL_AVX2
/* Multiply two number mod the order of P256 curve. (r = a * b mod order)
 *
 * r  Result of the multiplication.
 * a  First operand of the multiplication.
 * b  Second operand of the multiplication.
 */
static void sp_256_mont_mul_order_avx2_5(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_avx2_5(r, a, b);
    sp_256_mont_reduce_avx2_5(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_avx2_5(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_avx2_5(r, a);
    sp_256_mont_reduce_avx2_5(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_avx2_5(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_avx2_5(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_avx2_5(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_avx2_5(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 5);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_5(t, t);
        if (p256_order_2[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_avx2_5(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 5);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 5;
    sp_digit* t3 = td + 4 * 5;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_avx2_5(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_avx2_5(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_avx2_5(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_avx2_5(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_avx2_5(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_avx2_5(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_avx2_5(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_avx2_5(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_avx2_5(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_5(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_avx2_5(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_5(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_avx2_5(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_5(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_avx2_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_avx2_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_avx2_5(t2, t2, 4);
    sp_256_mont_mul_order_avx2_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_avx2_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_avx2_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_avx2_5(t2, t2, 4);
    sp_256_mont_mul_order_avx2_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_avx2_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_avx2_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_avx2_5(t2, t2, 4);
    sp_256_mont_mul_order_avx2_5(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_5(t2, t2);
        if (p256_order_low[i / 64] & ((sp_digit)1 << (i % 64)))
            sp_256_mont_mul_order_avx2_5(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_avx2_5(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_avx2_5(r, t2, t3);
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
    sp_digit ed[2*5];
    sp_digit xd[2*5];
    sp_digit kd[2*5];
    sp_digit rd[2*5];
    sp_digit td[3 * 2*5];
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
    int64_t c;
    int i;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    (void)heap;

    err = sp_ecc_point_new(heap, p, point);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = XMALLOC(sizeof(sp_digit) * 7 * 2 * 5, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            e = d + 0 * 5;
            x = d + 2 * 5;
            k = d + 4 * 5;
            r = d + 6 * 5;
            tmp = d + 8 * 5;
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

        sp_256_from_bin(e, 5, hash, hashLen);
        sp_256_from_mp(x, 5, priv);
    }

    for (i = SP_ECC_MAX_SIG_GEN; err == MP_OKAY && i > 0; i--) {
        /* New random point. */
        err = sp_256_ecc_gen_k_5(rng, k);
        if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                err = sp_256_ecc_mulmod_base_avx2_5(point, k, 1, heap);
            else
#endif
                err = sp_256_ecc_mulmod_base_5(point, k, 1, NULL);
        }

        if (err == MP_OKAY) {
            /* r = point->x mod order */
            XMEMCPY(r, point->x, sizeof(sp_digit) * 5);
            sp_256_norm_5(r);
            c = sp_256_cmp_5(r, p256_order);
            sp_256_cond_sub_5(r, r, p256_order, 0 - (c >= 0));
            sp_256_norm_5(r);

            /* Conv k to Montgomery form (mod order) */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_5(k, k, p256_norm_order);
            else
#endif
                sp_256_mul_5(k, k, p256_norm_order);
            err = sp_256_mod_5(k, k, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_5(k);
            /* kInv = 1/k mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_inv_order_avx2_5(kInv, k, tmp);
            else
#endif
                sp_256_mont_inv_order_5(kInv, k, tmp);
            sp_256_norm_5(kInv);

            /* s = r * x + e */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_5(x, x, r);
            else
#endif
                sp_256_mul_5(x, x, r);
            err = sp_256_mod_5(x, x, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_5(x);
            carry = sp_256_add_5(s, e, x);
            sp_256_cond_sub_5(s, s, p256_order, 0 - carry);
            sp_256_norm_5(s);
            c = sp_256_cmp_5(s, p256_order);
            sp_256_cond_sub_5(s, s, p256_order, 0 - (c >= 0));
            sp_256_norm_5(s);

            /* s = s * k^-1 mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_mul_order_avx2_5(s, s, kInv);
            else
#endif
                sp_256_mont_mul_order_5(s, s, kInv);
            sp_256_norm_5(s);

            /* Check that signature is usable. */
            if (!sp_256_iszero_5(s))
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
        XMEMSET(d, 0, sizeof(sp_digit) * 8 * 5);
        XFREE(d, heap, DYNAMIC_TYPE_ECC);
    }
#else
    XMEMSET(e, 0, sizeof(sp_digit) * 2 * 5);
    XMEMSET(x, 0, sizeof(sp_digit) * 2 * 5);
    XMEMSET(k, 0, sizeof(sp_digit) * 2 * 5);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 5);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 5);
    XMEMSET(tmp, 0, sizeof(sp_digit) * 3 * 2*5);
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
    sp_digit u1d[2*5];
    sp_digit u2d[2*5];
    sp_digit sd[2*5];
    sp_digit tmpd[2*5 * 5];
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
    int64_t c;
    int err;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

    err = sp_ecc_point_new(heap, p1d, p1);
    if (err == MP_OKAY)
        err = sp_ecc_point_new(heap, p2d, p2);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = XMALLOC(sizeof(sp_digit) * 16 * 5, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            u1  = d + 0 * 5;
            u2  = d + 2 * 5;
            s   = d + 4 * 5;
            tmp = d + 6 * 5;
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

        sp_256_from_bin(u1, 5, hash, hashLen);
        sp_256_from_mp(u2, 5, r);
        sp_256_from_mp(s, 5, sm);
        sp_256_from_mp(p2->x, 5, pX);
        sp_256_from_mp(p2->y, 5, pY);
        sp_256_from_mp(p2->z, 5, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_mul_avx2_5(s, s, p256_norm_order);
        else
#endif
            sp_256_mul_5(s, s, p256_norm_order);
        err = sp_256_mod_5(s, s, p256_order);
    }
    if (err == MP_OKAY) {
        sp_256_norm_5(s);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_inv_order_avx2_5(s, s, tmp);
            sp_256_mont_mul_order_avx2_5(u1, u1, s);
            sp_256_mont_mul_order_avx2_5(u2, u2, s);
        }
        else
#endif
        {
            sp_256_mont_inv_order_5(s, s, tmp);
            sp_256_mont_mul_order_5(u1, u1, s);
            sp_256_mont_mul_order_5(u2, u2, s);
        }

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_5(p1, u1, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_5(p1, u1, 0, heap);
    }
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_5(p2, p2, u2, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_5(p2, p2, u2, 0, heap);
    }

    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_5(p1, p1, p2, tmp);
        else
#endif
            sp_256_proj_point_add_5(p1, p1, p2, tmp);

        /* (r + n*order).z'.z' mod prime == (u1.G + u2.Q)->x' */
        /* Reload r and convert to Montgomery form. */
        sp_256_from_mp(u2, 5, r);
        err = sp_256_mod_mul_norm_5(u2, u2, p256_mod);
    }

    if (err == MP_OKAY) {
        /* u1 = r.z'.z' mod prime */
        sp_256_mont_sqr_5(p1->z, p1->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_5(u1, u2, p1->z, p256_mod, p256_mp_mod);
        *res = sp_256_cmp_5(p1->x, u1) == 0;
        if (*res == 0) {
            /* Reload r and add order. */
            sp_256_from_mp(u2, 5, r);
            carry = sp_256_add_5(u2, u2, p256_order);
            /* Carry means result is greater than mod and is not valid. */
            if (!carry) {
                sp_256_norm_5(u2);

                /* Compare with mod and if greater or equal then not valid. */
                c = sp_256_cmp_5(u2, p256_mod);
                if (c < 0) {
                    /* Convert to Montogomery form */
                    err = sp_256_mod_mul_norm_5(u2, u2, p256_mod);
                    if (err == MP_OKAY) {
                        /* u1 = (r + 1*order).z'.z' mod prime */
                        sp_256_mont_mul_5(u1, u2, p1->z, p256_mod,
                                                                  p256_mp_mod);
                        *res = sp_256_cmp_5(p1->x, u2) == 0;
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
static int sp_256_ecc_is_point_5(sp_point* point, void* heap)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d = NULL;
#else
    sp_digit t1d[2*5];
    sp_digit t2d[2*5];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 5 * 4, heap, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 5;
        t2 = d + 2 * 5;
    }
    else
        err = MEMORY_E;
#else
    (void)heap;

    t1 = t1d;
    t2 = t2d;
#endif

    if (err == MP_OKAY) {
        sp_256_sqr_5(t1, point->y);
        sp_256_mod_5(t1, t1, p256_mod);
        sp_256_sqr_5(t2, point->x);
        sp_256_mod_5(t2, t2, p256_mod);
        sp_256_mul_5(t2, t2, point->x);
        sp_256_mod_5(t2, t2, p256_mod);
	sp_256_sub_5(t2, p256_mod, t2);
        sp_256_mont_add_5(t1, t1, t2, p256_mod);

        sp_256_mont_add_5(t1, t1, point->x, p256_mod);
        sp_256_mont_add_5(t1, t1, point->x, p256_mod);
        sp_256_mont_add_5(t1, t1, point->x, p256_mod);

        if (sp_256_cmp_5(t1, p256_b) != 0)
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
        sp_256_from_mp(pub->x, 5, pX);
        sp_256_from_mp(pub->y, 5, pY);
        sp_256_from_bin(pub->z, 5, one, sizeof(one));

        err = sp_256_ecc_is_point_5(pub, NULL);
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
    sp_digit privd[5];
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
        priv = XMALLOC(sizeof(sp_digit) * 5, heap, DYNAMIC_TYPE_ECC);
        if (priv == NULL)
            err = MEMORY_E;
    }
#else
    priv = privd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(pub->x, 5, pX);
        sp_256_from_mp(pub->y, 5, pY);
        sp_256_from_bin(pub->z, 5, one, sizeof(one));
        sp_256_from_mp(priv, 5, privm);

        /* Check point at infinitiy. */
        if (sp_256_iszero_5(pub->x) &&
            sp_256_iszero_5(pub->y))
            err = ECC_INF_E;
    }

    if (err == MP_OKAY) {
        /* Check range of X and Y */
        if (sp_256_cmp_5(pub->x, p256_mod) >= 0 ||
            sp_256_cmp_5(pub->y, p256_mod) >= 0)
            err = ECC_OUT_OF_RANGE_E;
    }

    if (err == MP_OKAY) {
        /* Check point is on curve */
        err = sp_256_ecc_is_point_5(pub, heap);
    }

    if (err == MP_OKAY) {
        /* Point * order = infinity */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_5(p, pub, p256_order, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_5(p, pub, p256_order, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is infinity */
        if (!sp_256_iszero_5(p->x) ||
            !sp_256_iszero_5(p->y)) {
            err = ECC_INF_E;
        }
    }

    if (err == MP_OKAY) {
        /* Base * private = point */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_5(p, priv, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_5(p, priv, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is public key */
        if (sp_256_cmp_5(p->x, pub->x) != 0 ||
            sp_256_cmp_5(p->y, pub->y) != 0) {
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
    sp_digit tmpd[2 * 5 * 5];
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
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 5 * 5, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 5, pX);
        sp_256_from_mp(p->y, 5, pY);
        sp_256_from_mp(p->z, 5, pZ);
        sp_256_from_mp(q->x, 5, qX);
        sp_256_from_mp(q->y, 5, qY);
        sp_256_from_mp(q->z, 5, qZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_5(p, p, q, tmp);
        else
#endif
            sp_256_proj_point_add_5(p, p, q, tmp);
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
    sp_digit tmpd[2 * 5 * 2];
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
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 5 * 2, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 5, pX);
        sp_256_from_mp(p->y, 5, pY);
        sp_256_from_mp(p->z, 5, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_dbl_avx2_5(p, p, tmp);
        else
#endif
            sp_256_proj_point_dbl_5(p, p, tmp);
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
    sp_digit tmpd[2 * 5 * 4];
    sp_point pd;
#endif
    sp_digit* tmp;
    sp_point* p;
    int err;

    err = sp_ecc_point_new(NULL, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 5 * 4, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 5, pX);
        sp_256_from_mp(p->y, 5, pY);
        sp_256_from_mp(p->z, 5, pZ);

        sp_256_map_5(p, p, tmp);
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
static int sp_256_mont_sqrt_5(sp_digit* y)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d;
#else
    sp_digit t1d[2 * 5];
    sp_digit t2d[2 * 5];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 5, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 5;
        t2 = d + 2 * 5;
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
            sp_256_mont_sqr_avx2_5(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_avx2_5(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_avx2_5(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_avx2_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_avx2_5(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_avx2_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_avx2_5(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_avx2_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_avx2_5(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_avx2_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_avx2_5(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_avx2_5(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_avx2_5(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_avx2_5(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_avx2_5(y, t1, 94, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            /* t2 = y ^ 0x2 */
            sp_256_mont_sqr_5(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_5(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_5(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_5(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_5(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_5(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_5(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_5(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_5(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_5(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_5(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_5(y, t1, 94, p256_mod, p256_mp_mod);
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
    sp_digit xd[2 * 5];
    sp_digit yd[2 * 5];
#endif
    sp_digit* x;
    sp_digit* y;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 5, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        x = d + 0 * 5;
        y = d + 2 * 5;
    }
    else
        err = MEMORY_E;
#else
    x = xd;
    y = yd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(x, 5, xm);

        err = sp_256_mod_mul_norm_5(x, x, p256_mod);
    }

    if (err == MP_OKAY) {
        /* y = x^3 */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_sqr_avx2_5(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_avx2_5(y, y, x, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            sp_256_mont_sqr_5(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_5(y, y, x, p256_mod, p256_mp_mod);
        }
        /* y = x^3 - 3x */
        sp_256_mont_sub_5(y, y, x, p256_mod);
        sp_256_mont_sub_5(y, y, x, p256_mod);
        sp_256_mont_sub_5(y, y, x, p256_mod);
        /* y = x^3 - 3x + b */
        err = sp_256_mod_mul_norm_5(x, p256_b, p256_mod);
    }
    if (err == MP_OKAY) {
        sp_256_mont_add_5(y, y, x, p256_mod);
        /* y = sqrt(x^3 - 3x + b) */
        err = sp_256_mont_sqrt_5(y);
    }
    if (err == MP_OKAY) {
        XMEMSET(y + 5, 0, 5 * sizeof(sp_digit));
        sp_256_mont_reduce_5(y, p256_mod, p256_mp_mod);
        if (((y[0] ^ odd) & 1) != 0)
            sp_256_mont_sub_5(y, p256_mod, y, p256_mod);

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
#endif /* SP_WORD_SIZE == 64 */
#endif /* !WOLFSSL_SP_ASM */
#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_ECC */
