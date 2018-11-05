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
#if SP_WORD_SIZE == 32
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
        if (s >= 15) {
            r[j] &= 0x7fffff;
            s = 23 - s;
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
#if DIGIT_BIT == 23
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 23
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0x7fffff;
        s = 23 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 23 <= DIGIT_BIT) {
            s += 23;
            r[j] &= 0x7fffff;
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
        if (s + DIGIT_BIT >= 23) {
            r[j] &= 0x7fffff;
            if (j + 1 >= max)
                break;
            s = 23 - s;
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

    for (i=0; i<89; i++) {
        r[i+1] += r[i] >> 23;
        r[i] &= 0x7fffff;
    }
    j = 2048 / 8 - 1;
    a[j] = 0;
    for (i=0; i<90 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 23) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 23);
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
SP_NOINLINE static void sp_2048_mul_15(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int64_t t0   = ((int64_t)a[ 0]) * b[ 0];
    int64_t t1   = ((int64_t)a[ 0]) * b[ 1]
                 + ((int64_t)a[ 1]) * b[ 0];
    int64_t t2   = ((int64_t)a[ 0]) * b[ 2]
                 + ((int64_t)a[ 1]) * b[ 1]
                 + ((int64_t)a[ 2]) * b[ 0];
    int64_t t3   = ((int64_t)a[ 0]) * b[ 3]
                 + ((int64_t)a[ 1]) * b[ 2]
                 + ((int64_t)a[ 2]) * b[ 1]
                 + ((int64_t)a[ 3]) * b[ 0];
    int64_t t4   = ((int64_t)a[ 0]) * b[ 4]
                 + ((int64_t)a[ 1]) * b[ 3]
                 + ((int64_t)a[ 2]) * b[ 2]
                 + ((int64_t)a[ 3]) * b[ 1]
                 + ((int64_t)a[ 4]) * b[ 0];
    int64_t t5   = ((int64_t)a[ 0]) * b[ 5]
                 + ((int64_t)a[ 1]) * b[ 4]
                 + ((int64_t)a[ 2]) * b[ 3]
                 + ((int64_t)a[ 3]) * b[ 2]
                 + ((int64_t)a[ 4]) * b[ 1]
                 + ((int64_t)a[ 5]) * b[ 0];
    int64_t t6   = ((int64_t)a[ 0]) * b[ 6]
                 + ((int64_t)a[ 1]) * b[ 5]
                 + ((int64_t)a[ 2]) * b[ 4]
                 + ((int64_t)a[ 3]) * b[ 3]
                 + ((int64_t)a[ 4]) * b[ 2]
                 + ((int64_t)a[ 5]) * b[ 1]
                 + ((int64_t)a[ 6]) * b[ 0];
    int64_t t7   = ((int64_t)a[ 0]) * b[ 7]
                 + ((int64_t)a[ 1]) * b[ 6]
                 + ((int64_t)a[ 2]) * b[ 5]
                 + ((int64_t)a[ 3]) * b[ 4]
                 + ((int64_t)a[ 4]) * b[ 3]
                 + ((int64_t)a[ 5]) * b[ 2]
                 + ((int64_t)a[ 6]) * b[ 1]
                 + ((int64_t)a[ 7]) * b[ 0];
    int64_t t8   = ((int64_t)a[ 0]) * b[ 8]
                 + ((int64_t)a[ 1]) * b[ 7]
                 + ((int64_t)a[ 2]) * b[ 6]
                 + ((int64_t)a[ 3]) * b[ 5]
                 + ((int64_t)a[ 4]) * b[ 4]
                 + ((int64_t)a[ 5]) * b[ 3]
                 + ((int64_t)a[ 6]) * b[ 2]
                 + ((int64_t)a[ 7]) * b[ 1]
                 + ((int64_t)a[ 8]) * b[ 0];
    int64_t t9   = ((int64_t)a[ 0]) * b[ 9]
                 + ((int64_t)a[ 1]) * b[ 8]
                 + ((int64_t)a[ 2]) * b[ 7]
                 + ((int64_t)a[ 3]) * b[ 6]
                 + ((int64_t)a[ 4]) * b[ 5]
                 + ((int64_t)a[ 5]) * b[ 4]
                 + ((int64_t)a[ 6]) * b[ 3]
                 + ((int64_t)a[ 7]) * b[ 2]
                 + ((int64_t)a[ 8]) * b[ 1]
                 + ((int64_t)a[ 9]) * b[ 0];
    int64_t t10  = ((int64_t)a[ 0]) * b[10]
                 + ((int64_t)a[ 1]) * b[ 9]
                 + ((int64_t)a[ 2]) * b[ 8]
                 + ((int64_t)a[ 3]) * b[ 7]
                 + ((int64_t)a[ 4]) * b[ 6]
                 + ((int64_t)a[ 5]) * b[ 5]
                 + ((int64_t)a[ 6]) * b[ 4]
                 + ((int64_t)a[ 7]) * b[ 3]
                 + ((int64_t)a[ 8]) * b[ 2]
                 + ((int64_t)a[ 9]) * b[ 1]
                 + ((int64_t)a[10]) * b[ 0];
    int64_t t11  = ((int64_t)a[ 0]) * b[11]
                 + ((int64_t)a[ 1]) * b[10]
                 + ((int64_t)a[ 2]) * b[ 9]
                 + ((int64_t)a[ 3]) * b[ 8]
                 + ((int64_t)a[ 4]) * b[ 7]
                 + ((int64_t)a[ 5]) * b[ 6]
                 + ((int64_t)a[ 6]) * b[ 5]
                 + ((int64_t)a[ 7]) * b[ 4]
                 + ((int64_t)a[ 8]) * b[ 3]
                 + ((int64_t)a[ 9]) * b[ 2]
                 + ((int64_t)a[10]) * b[ 1]
                 + ((int64_t)a[11]) * b[ 0];
    int64_t t12  = ((int64_t)a[ 0]) * b[12]
                 + ((int64_t)a[ 1]) * b[11]
                 + ((int64_t)a[ 2]) * b[10]
                 + ((int64_t)a[ 3]) * b[ 9]
                 + ((int64_t)a[ 4]) * b[ 8]
                 + ((int64_t)a[ 5]) * b[ 7]
                 + ((int64_t)a[ 6]) * b[ 6]
                 + ((int64_t)a[ 7]) * b[ 5]
                 + ((int64_t)a[ 8]) * b[ 4]
                 + ((int64_t)a[ 9]) * b[ 3]
                 + ((int64_t)a[10]) * b[ 2]
                 + ((int64_t)a[11]) * b[ 1]
                 + ((int64_t)a[12]) * b[ 0];
    int64_t t13  = ((int64_t)a[ 0]) * b[13]
                 + ((int64_t)a[ 1]) * b[12]
                 + ((int64_t)a[ 2]) * b[11]
                 + ((int64_t)a[ 3]) * b[10]
                 + ((int64_t)a[ 4]) * b[ 9]
                 + ((int64_t)a[ 5]) * b[ 8]
                 + ((int64_t)a[ 6]) * b[ 7]
                 + ((int64_t)a[ 7]) * b[ 6]
                 + ((int64_t)a[ 8]) * b[ 5]
                 + ((int64_t)a[ 9]) * b[ 4]
                 + ((int64_t)a[10]) * b[ 3]
                 + ((int64_t)a[11]) * b[ 2]
                 + ((int64_t)a[12]) * b[ 1]
                 + ((int64_t)a[13]) * b[ 0];
    int64_t t14  = ((int64_t)a[ 0]) * b[14]
                 + ((int64_t)a[ 1]) * b[13]
                 + ((int64_t)a[ 2]) * b[12]
                 + ((int64_t)a[ 3]) * b[11]
                 + ((int64_t)a[ 4]) * b[10]
                 + ((int64_t)a[ 5]) * b[ 9]
                 + ((int64_t)a[ 6]) * b[ 8]
                 + ((int64_t)a[ 7]) * b[ 7]
                 + ((int64_t)a[ 8]) * b[ 6]
                 + ((int64_t)a[ 9]) * b[ 5]
                 + ((int64_t)a[10]) * b[ 4]
                 + ((int64_t)a[11]) * b[ 3]
                 + ((int64_t)a[12]) * b[ 2]
                 + ((int64_t)a[13]) * b[ 1]
                 + ((int64_t)a[14]) * b[ 0];
    int64_t t15  = ((int64_t)a[ 1]) * b[14]
                 + ((int64_t)a[ 2]) * b[13]
                 + ((int64_t)a[ 3]) * b[12]
                 + ((int64_t)a[ 4]) * b[11]
                 + ((int64_t)a[ 5]) * b[10]
                 + ((int64_t)a[ 6]) * b[ 9]
                 + ((int64_t)a[ 7]) * b[ 8]
                 + ((int64_t)a[ 8]) * b[ 7]
                 + ((int64_t)a[ 9]) * b[ 6]
                 + ((int64_t)a[10]) * b[ 5]
                 + ((int64_t)a[11]) * b[ 4]
                 + ((int64_t)a[12]) * b[ 3]
                 + ((int64_t)a[13]) * b[ 2]
                 + ((int64_t)a[14]) * b[ 1];
    int64_t t16  = ((int64_t)a[ 2]) * b[14]
                 + ((int64_t)a[ 3]) * b[13]
                 + ((int64_t)a[ 4]) * b[12]
                 + ((int64_t)a[ 5]) * b[11]
                 + ((int64_t)a[ 6]) * b[10]
                 + ((int64_t)a[ 7]) * b[ 9]
                 + ((int64_t)a[ 8]) * b[ 8]
                 + ((int64_t)a[ 9]) * b[ 7]
                 + ((int64_t)a[10]) * b[ 6]
                 + ((int64_t)a[11]) * b[ 5]
                 + ((int64_t)a[12]) * b[ 4]
                 + ((int64_t)a[13]) * b[ 3]
                 + ((int64_t)a[14]) * b[ 2];
    int64_t t17  = ((int64_t)a[ 3]) * b[14]
                 + ((int64_t)a[ 4]) * b[13]
                 + ((int64_t)a[ 5]) * b[12]
                 + ((int64_t)a[ 6]) * b[11]
                 + ((int64_t)a[ 7]) * b[10]
                 + ((int64_t)a[ 8]) * b[ 9]
                 + ((int64_t)a[ 9]) * b[ 8]
                 + ((int64_t)a[10]) * b[ 7]
                 + ((int64_t)a[11]) * b[ 6]
                 + ((int64_t)a[12]) * b[ 5]
                 + ((int64_t)a[13]) * b[ 4]
                 + ((int64_t)a[14]) * b[ 3];
    int64_t t18  = ((int64_t)a[ 4]) * b[14]
                 + ((int64_t)a[ 5]) * b[13]
                 + ((int64_t)a[ 6]) * b[12]
                 + ((int64_t)a[ 7]) * b[11]
                 + ((int64_t)a[ 8]) * b[10]
                 + ((int64_t)a[ 9]) * b[ 9]
                 + ((int64_t)a[10]) * b[ 8]
                 + ((int64_t)a[11]) * b[ 7]
                 + ((int64_t)a[12]) * b[ 6]
                 + ((int64_t)a[13]) * b[ 5]
                 + ((int64_t)a[14]) * b[ 4];
    int64_t t19  = ((int64_t)a[ 5]) * b[14]
                 + ((int64_t)a[ 6]) * b[13]
                 + ((int64_t)a[ 7]) * b[12]
                 + ((int64_t)a[ 8]) * b[11]
                 + ((int64_t)a[ 9]) * b[10]
                 + ((int64_t)a[10]) * b[ 9]
                 + ((int64_t)a[11]) * b[ 8]
                 + ((int64_t)a[12]) * b[ 7]
                 + ((int64_t)a[13]) * b[ 6]
                 + ((int64_t)a[14]) * b[ 5];
    int64_t t20  = ((int64_t)a[ 6]) * b[14]
                 + ((int64_t)a[ 7]) * b[13]
                 + ((int64_t)a[ 8]) * b[12]
                 + ((int64_t)a[ 9]) * b[11]
                 + ((int64_t)a[10]) * b[10]
                 + ((int64_t)a[11]) * b[ 9]
                 + ((int64_t)a[12]) * b[ 8]
                 + ((int64_t)a[13]) * b[ 7]
                 + ((int64_t)a[14]) * b[ 6];
    int64_t t21  = ((int64_t)a[ 7]) * b[14]
                 + ((int64_t)a[ 8]) * b[13]
                 + ((int64_t)a[ 9]) * b[12]
                 + ((int64_t)a[10]) * b[11]
                 + ((int64_t)a[11]) * b[10]
                 + ((int64_t)a[12]) * b[ 9]
                 + ((int64_t)a[13]) * b[ 8]
                 + ((int64_t)a[14]) * b[ 7];
    int64_t t22  = ((int64_t)a[ 8]) * b[14]
                 + ((int64_t)a[ 9]) * b[13]
                 + ((int64_t)a[10]) * b[12]
                 + ((int64_t)a[11]) * b[11]
                 + ((int64_t)a[12]) * b[10]
                 + ((int64_t)a[13]) * b[ 9]
                 + ((int64_t)a[14]) * b[ 8];
    int64_t t23  = ((int64_t)a[ 9]) * b[14]
                 + ((int64_t)a[10]) * b[13]
                 + ((int64_t)a[11]) * b[12]
                 + ((int64_t)a[12]) * b[11]
                 + ((int64_t)a[13]) * b[10]
                 + ((int64_t)a[14]) * b[ 9];
    int64_t t24  = ((int64_t)a[10]) * b[14]
                 + ((int64_t)a[11]) * b[13]
                 + ((int64_t)a[12]) * b[12]
                 + ((int64_t)a[13]) * b[11]
                 + ((int64_t)a[14]) * b[10];
    int64_t t25  = ((int64_t)a[11]) * b[14]
                 + ((int64_t)a[12]) * b[13]
                 + ((int64_t)a[13]) * b[12]
                 + ((int64_t)a[14]) * b[11];
    int64_t t26  = ((int64_t)a[12]) * b[14]
                 + ((int64_t)a[13]) * b[13]
                 + ((int64_t)a[14]) * b[12];
    int64_t t27  = ((int64_t)a[13]) * b[14]
                 + ((int64_t)a[14]) * b[13];
    int64_t t28  = ((int64_t)a[14]) * b[14];

    t1   += t0  >> 23; r[ 0] = t0  & 0x7fffff;
    t2   += t1  >> 23; r[ 1] = t1  & 0x7fffff;
    t3   += t2  >> 23; r[ 2] = t2  & 0x7fffff;
    t4   += t3  >> 23; r[ 3] = t3  & 0x7fffff;
    t5   += t4  >> 23; r[ 4] = t4  & 0x7fffff;
    t6   += t5  >> 23; r[ 5] = t5  & 0x7fffff;
    t7   += t6  >> 23; r[ 6] = t6  & 0x7fffff;
    t8   += t7  >> 23; r[ 7] = t7  & 0x7fffff;
    t9   += t8  >> 23; r[ 8] = t8  & 0x7fffff;
    t10  += t9  >> 23; r[ 9] = t9  & 0x7fffff;
    t11  += t10 >> 23; r[10] = t10 & 0x7fffff;
    t12  += t11 >> 23; r[11] = t11 & 0x7fffff;
    t13  += t12 >> 23; r[12] = t12 & 0x7fffff;
    t14  += t13 >> 23; r[13] = t13 & 0x7fffff;
    t15  += t14 >> 23; r[14] = t14 & 0x7fffff;
    t16  += t15 >> 23; r[15] = t15 & 0x7fffff;
    t17  += t16 >> 23; r[16] = t16 & 0x7fffff;
    t18  += t17 >> 23; r[17] = t17 & 0x7fffff;
    t19  += t18 >> 23; r[18] = t18 & 0x7fffff;
    t20  += t19 >> 23; r[19] = t19 & 0x7fffff;
    t21  += t20 >> 23; r[20] = t20 & 0x7fffff;
    t22  += t21 >> 23; r[21] = t21 & 0x7fffff;
    t23  += t22 >> 23; r[22] = t22 & 0x7fffff;
    t24  += t23 >> 23; r[23] = t23 & 0x7fffff;
    t25  += t24 >> 23; r[24] = t24 & 0x7fffff;
    t26  += t25 >> 23; r[25] = t25 & 0x7fffff;
    t27  += t26 >> 23; r[26] = t26 & 0x7fffff;
    t28  += t27 >> 23; r[27] = t27 & 0x7fffff;
    r[29] = (sp_digit)(t28 >> 23);
                       r[28] = t28 & 0x7fffff;
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_15(sp_digit* r, const sp_digit* a)
{
    int64_t t0   =  ((int64_t)a[ 0]) * a[ 0];
    int64_t t1   = (((int64_t)a[ 0]) * a[ 1]) * 2;
    int64_t t2   = (((int64_t)a[ 0]) * a[ 2]) * 2
                 +  ((int64_t)a[ 1]) * a[ 1];
    int64_t t3   = (((int64_t)a[ 0]) * a[ 3]
                 +  ((int64_t)a[ 1]) * a[ 2]) * 2;
    int64_t t4   = (((int64_t)a[ 0]) * a[ 4]
                 +  ((int64_t)a[ 1]) * a[ 3]) * 2
                 +  ((int64_t)a[ 2]) * a[ 2];
    int64_t t5   = (((int64_t)a[ 0]) * a[ 5]
                 +  ((int64_t)a[ 1]) * a[ 4]
                 +  ((int64_t)a[ 2]) * a[ 3]) * 2;
    int64_t t6   = (((int64_t)a[ 0]) * a[ 6]
                 +  ((int64_t)a[ 1]) * a[ 5]
                 +  ((int64_t)a[ 2]) * a[ 4]) * 2
                 +  ((int64_t)a[ 3]) * a[ 3];
    int64_t t7   = (((int64_t)a[ 0]) * a[ 7]
                 +  ((int64_t)a[ 1]) * a[ 6]
                 +  ((int64_t)a[ 2]) * a[ 5]
                 +  ((int64_t)a[ 3]) * a[ 4]) * 2;
    int64_t t8   = (((int64_t)a[ 0]) * a[ 8]
                 +  ((int64_t)a[ 1]) * a[ 7]
                 +  ((int64_t)a[ 2]) * a[ 6]
                 +  ((int64_t)a[ 3]) * a[ 5]) * 2
                 +  ((int64_t)a[ 4]) * a[ 4];
    int64_t t9   = (((int64_t)a[ 0]) * a[ 9]
                 +  ((int64_t)a[ 1]) * a[ 8]
                 +  ((int64_t)a[ 2]) * a[ 7]
                 +  ((int64_t)a[ 3]) * a[ 6]
                 +  ((int64_t)a[ 4]) * a[ 5]) * 2;
    int64_t t10  = (((int64_t)a[ 0]) * a[10]
                 +  ((int64_t)a[ 1]) * a[ 9]
                 +  ((int64_t)a[ 2]) * a[ 8]
                 +  ((int64_t)a[ 3]) * a[ 7]
                 +  ((int64_t)a[ 4]) * a[ 6]) * 2
                 +  ((int64_t)a[ 5]) * a[ 5];
    int64_t t11  = (((int64_t)a[ 0]) * a[11]
                 +  ((int64_t)a[ 1]) * a[10]
                 +  ((int64_t)a[ 2]) * a[ 9]
                 +  ((int64_t)a[ 3]) * a[ 8]
                 +  ((int64_t)a[ 4]) * a[ 7]
                 +  ((int64_t)a[ 5]) * a[ 6]) * 2;
    int64_t t12  = (((int64_t)a[ 0]) * a[12]
                 +  ((int64_t)a[ 1]) * a[11]
                 +  ((int64_t)a[ 2]) * a[10]
                 +  ((int64_t)a[ 3]) * a[ 9]
                 +  ((int64_t)a[ 4]) * a[ 8]
                 +  ((int64_t)a[ 5]) * a[ 7]) * 2
                 +  ((int64_t)a[ 6]) * a[ 6];
    int64_t t13  = (((int64_t)a[ 0]) * a[13]
                 +  ((int64_t)a[ 1]) * a[12]
                 +  ((int64_t)a[ 2]) * a[11]
                 +  ((int64_t)a[ 3]) * a[10]
                 +  ((int64_t)a[ 4]) * a[ 9]
                 +  ((int64_t)a[ 5]) * a[ 8]
                 +  ((int64_t)a[ 6]) * a[ 7]) * 2;
    int64_t t14  = (((int64_t)a[ 0]) * a[14]
                 +  ((int64_t)a[ 1]) * a[13]
                 +  ((int64_t)a[ 2]) * a[12]
                 +  ((int64_t)a[ 3]) * a[11]
                 +  ((int64_t)a[ 4]) * a[10]
                 +  ((int64_t)a[ 5]) * a[ 9]
                 +  ((int64_t)a[ 6]) * a[ 8]) * 2
                 +  ((int64_t)a[ 7]) * a[ 7];
    int64_t t15  = (((int64_t)a[ 1]) * a[14]
                 +  ((int64_t)a[ 2]) * a[13]
                 +  ((int64_t)a[ 3]) * a[12]
                 +  ((int64_t)a[ 4]) * a[11]
                 +  ((int64_t)a[ 5]) * a[10]
                 +  ((int64_t)a[ 6]) * a[ 9]
                 +  ((int64_t)a[ 7]) * a[ 8]) * 2;
    int64_t t16  = (((int64_t)a[ 2]) * a[14]
                 +  ((int64_t)a[ 3]) * a[13]
                 +  ((int64_t)a[ 4]) * a[12]
                 +  ((int64_t)a[ 5]) * a[11]
                 +  ((int64_t)a[ 6]) * a[10]
                 +  ((int64_t)a[ 7]) * a[ 9]) * 2
                 +  ((int64_t)a[ 8]) * a[ 8];
    int64_t t17  = (((int64_t)a[ 3]) * a[14]
                 +  ((int64_t)a[ 4]) * a[13]
                 +  ((int64_t)a[ 5]) * a[12]
                 +  ((int64_t)a[ 6]) * a[11]
                 +  ((int64_t)a[ 7]) * a[10]
                 +  ((int64_t)a[ 8]) * a[ 9]) * 2;
    int64_t t18  = (((int64_t)a[ 4]) * a[14]
                 +  ((int64_t)a[ 5]) * a[13]
                 +  ((int64_t)a[ 6]) * a[12]
                 +  ((int64_t)a[ 7]) * a[11]
                 +  ((int64_t)a[ 8]) * a[10]) * 2
                 +  ((int64_t)a[ 9]) * a[ 9];
    int64_t t19  = (((int64_t)a[ 5]) * a[14]
                 +  ((int64_t)a[ 6]) * a[13]
                 +  ((int64_t)a[ 7]) * a[12]
                 +  ((int64_t)a[ 8]) * a[11]
                 +  ((int64_t)a[ 9]) * a[10]) * 2;
    int64_t t20  = (((int64_t)a[ 6]) * a[14]
                 +  ((int64_t)a[ 7]) * a[13]
                 +  ((int64_t)a[ 8]) * a[12]
                 +  ((int64_t)a[ 9]) * a[11]) * 2
                 +  ((int64_t)a[10]) * a[10];
    int64_t t21  = (((int64_t)a[ 7]) * a[14]
                 +  ((int64_t)a[ 8]) * a[13]
                 +  ((int64_t)a[ 9]) * a[12]
                 +  ((int64_t)a[10]) * a[11]) * 2;
    int64_t t22  = (((int64_t)a[ 8]) * a[14]
                 +  ((int64_t)a[ 9]) * a[13]
                 +  ((int64_t)a[10]) * a[12]) * 2
                 +  ((int64_t)a[11]) * a[11];
    int64_t t23  = (((int64_t)a[ 9]) * a[14]
                 +  ((int64_t)a[10]) * a[13]
                 +  ((int64_t)a[11]) * a[12]) * 2;
    int64_t t24  = (((int64_t)a[10]) * a[14]
                 +  ((int64_t)a[11]) * a[13]) * 2
                 +  ((int64_t)a[12]) * a[12];
    int64_t t25  = (((int64_t)a[11]) * a[14]
                 +  ((int64_t)a[12]) * a[13]) * 2;
    int64_t t26  = (((int64_t)a[12]) * a[14]) * 2
                 +  ((int64_t)a[13]) * a[13];
    int64_t t27  = (((int64_t)a[13]) * a[14]) * 2;
    int64_t t28  =  ((int64_t)a[14]) * a[14];

    t1   += t0  >> 23; r[ 0] = t0  & 0x7fffff;
    t2   += t1  >> 23; r[ 1] = t1  & 0x7fffff;
    t3   += t2  >> 23; r[ 2] = t2  & 0x7fffff;
    t4   += t3  >> 23; r[ 3] = t3  & 0x7fffff;
    t5   += t4  >> 23; r[ 4] = t4  & 0x7fffff;
    t6   += t5  >> 23; r[ 5] = t5  & 0x7fffff;
    t7   += t6  >> 23; r[ 6] = t6  & 0x7fffff;
    t8   += t7  >> 23; r[ 7] = t7  & 0x7fffff;
    t9   += t8  >> 23; r[ 8] = t8  & 0x7fffff;
    t10  += t9  >> 23; r[ 9] = t9  & 0x7fffff;
    t11  += t10 >> 23; r[10] = t10 & 0x7fffff;
    t12  += t11 >> 23; r[11] = t11 & 0x7fffff;
    t13  += t12 >> 23; r[12] = t12 & 0x7fffff;
    t14  += t13 >> 23; r[13] = t13 & 0x7fffff;
    t15  += t14 >> 23; r[14] = t14 & 0x7fffff;
    t16  += t15 >> 23; r[15] = t15 & 0x7fffff;
    t17  += t16 >> 23; r[16] = t16 & 0x7fffff;
    t18  += t17 >> 23; r[17] = t17 & 0x7fffff;
    t19  += t18 >> 23; r[18] = t18 & 0x7fffff;
    t20  += t19 >> 23; r[19] = t19 & 0x7fffff;
    t21  += t20 >> 23; r[20] = t20 & 0x7fffff;
    t22  += t21 >> 23; r[21] = t21 & 0x7fffff;
    t23  += t22 >> 23; r[22] = t22 & 0x7fffff;
    t24  += t23 >> 23; r[23] = t23 & 0x7fffff;
    t25  += t24 >> 23; r[24] = t24 & 0x7fffff;
    t26  += t25 >> 23; r[25] = t25 & 0x7fffff;
    t27  += t26 >> 23; r[26] = t26 & 0x7fffff;
    t28  += t27 >> 23; r[27] = t27 & 0x7fffff;
    r[29] = (sp_digit)(t28 >> 23);
                       r[28] = t28 & 0x7fffff;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_15(sp_digit* r, const sp_digit* a,
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
    r[ 9] = a[ 9] + b[ 9];
    r[10] = a[10] + b[10];
    r[11] = a[11] + b[11];
    r[12] = a[12] + b[12];
    r[13] = a[13] + b[13];
    r[14] = a[14] + b[14];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_30(sp_digit* r, const sp_digit* a,
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
    r[27] = a[27] - b[27];
    r[28] = a[28] - b[28];
    r[29] = a[29] - b[29];

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_30(sp_digit* r, const sp_digit* a,
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
    r[27] = a[27] + b[27];
    r[28] = a[28] + b[28];
    r[29] = a[29] + b[29];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_45(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit p0[30];
    sp_digit p1[30];
    sp_digit p2[30];
    sp_digit p3[30];
    sp_digit p4[30];
    sp_digit p5[30];
    sp_digit t0[30];
    sp_digit t1[30];
    sp_digit t2[30];
    sp_digit a0[15];
    sp_digit a1[15];
    sp_digit a2[15];
    sp_digit b0[15];
    sp_digit b1[15];
    sp_digit b2[15];
    sp_2048_add_15(a0, a, &a[15]);
    sp_2048_add_15(b0, b, &b[15]);
    sp_2048_add_15(a1, &a[15], &a[30]);
    sp_2048_add_15(b1, &b[15], &b[30]);
    sp_2048_add_15(a2, a0, &a[30]);
    sp_2048_add_15(b2, b0, &b[30]);
    sp_2048_mul_15(p0, a, b);
    sp_2048_mul_15(p2, &a[15], &b[15]);
    sp_2048_mul_15(p4, &a[30], &b[30]);
    sp_2048_mul_15(p1, a0, b0);
    sp_2048_mul_15(p3, a1, b1);
    sp_2048_mul_15(p5, a2, b2);
    XMEMSET(r, 0, sizeof(*r)*2*45);
    sp_2048_sub_30(t0, p3, p2);
    sp_2048_sub_30(t1, p1, p2);
    sp_2048_sub_30(t2, p5, t0);
    sp_2048_sub_30(t2, t2, t1);
    sp_2048_sub_30(t0, t0, p4);
    sp_2048_sub_30(t1, t1, p0);
    sp_2048_add_30(r, r, p0);
    sp_2048_add_30(&r[15], &r[15], t1);
    sp_2048_add_30(&r[30], &r[30], t2);
    sp_2048_add_30(&r[45], &r[45], t0);
    sp_2048_add_30(&r[60], &r[60], p4);
}

/* Square a into r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_45(sp_digit* r, const sp_digit* a)
{
    sp_digit p0[30];
    sp_digit p1[30];
    sp_digit p2[30];
    sp_digit p3[30];
    sp_digit p4[30];
    sp_digit p5[30];
    sp_digit t0[30];
    sp_digit t1[30];
    sp_digit t2[30];
    sp_digit a0[15];
    sp_digit a1[15];
    sp_digit a2[15];
    sp_2048_add_15(a0, a, &a[15]);
    sp_2048_add_15(a1, &a[15], &a[30]);
    sp_2048_add_15(a2, a0, &a[30]);
    sp_2048_sqr_15(p0, a);
    sp_2048_sqr_15(p2, &a[15]);
    sp_2048_sqr_15(p4, &a[30]);
    sp_2048_sqr_15(p1, a0);
    sp_2048_sqr_15(p3, a1);
    sp_2048_sqr_15(p5, a2);
    XMEMSET(r, 0, sizeof(*r)*2*45);
    sp_2048_sub_30(t0, p3, p2);
    sp_2048_sub_30(t1, p1, p2);
    sp_2048_sub_30(t2, p5, t0);
    sp_2048_sub_30(t2, t2, t1);
    sp_2048_sub_30(t0, t0, p4);
    sp_2048_sub_30(t1, t1, p0);
    sp_2048_add_30(r, r, p0);
    sp_2048_add_30(&r[15], &r[15], t1);
    sp_2048_add_30(&r[30], &r[30], t2);
    sp_2048_add_30(&r[45], &r[45], t0);
    sp_2048_add_30(&r[60], &r[60], p4);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[40] = a[40] + b[40];
    r[41] = a[41] + b[41];
    r[42] = a[42] + b[42];
    r[43] = a[43] + b[43];
    r[44] = a[44] + b[44];

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 88; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[88] = a[88] + b[88];
    r[89] = a[89] + b[89];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 88; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[88] = a[88] - b[88];
    r[89] = a[89] - b[89];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_2048_mul_90(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[90];
    sp_digit* a1 = z1;
    sp_digit b1[45];
    sp_digit* z2 = r + 90;
    sp_2048_add_45(a1, a, &a[45]);
    sp_2048_add_45(b1, b, &b[45]);
    sp_2048_mul_45(z2, &a[45], &b[45]);
    sp_2048_mul_45(z0, a, b);
    sp_2048_mul_45(z1, a1, b1);
    sp_2048_sub_90(z1, z1, z2);
    sp_2048_sub_90(z1, z1, z0);
    sp_2048_add_90(r + 45, r + 45, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_90(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[90];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 90;
    sp_2048_add_45(a1, a, &a[45]);
    sp_2048_sqr_45(z2, &a[45]);
    sp_2048_sqr_45(z0, a);
    sp_2048_sqr_45(z1, a1);
    sp_2048_sub_90(z1, z1, z2);
    sp_2048_sub_90(z1, z1, z0);
    sp_2048_add_90(r + 45, r + 45, z1);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 90; i++)
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
SP_NOINLINE static int sp_2048_sub_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 90; i++)
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
SP_NOINLINE static void sp_2048_mul_90(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[89]) * b[89];
    r[179] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 177; k >= 0; k--) {
        for (i = 89; i >= 0; i--) {
            j = k - i;
            if (j >= 90)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_90(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[89]) * a[89];
    r[179] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 177; k >= 0; k--) {
        for (i = 89; i >= 0; i--) {
            j = k - i;
            if (j >= 90 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int64_t)a[i]) * a[i];

        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
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
SP_NOINLINE static int sp_2048_add_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 45; i++)
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
SP_NOINLINE static int sp_2048_sub_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 45; i++)
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
SP_NOINLINE static int sp_2048_sub_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[40] = a[40] - b[40];
    r[41] = a[41] - b[41];
    r[42] = a[42] - b[42];
    r[43] = a[43] - b[43];
    r[44] = a[44] - b[44];

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
SP_NOINLINE static void sp_2048_mul_45(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[44]) * b[44];
    r[89] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 87; k >= 0; k--) {
        for (i = 44; i >= 0; i--) {
            j = k - i;
            if (j >= 45)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_2048_sqr_45(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[44]) * a[44];
    r[89] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 87; k >= 0; k--) {
        for (i = 44; i >= 0; i--) {
            j = k - i;
            if (j >= 45 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int64_t)a[i]) * a[i];

        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
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
    x &= 0x7fffff;

    /* rho = -1/m mod b */
    *rho = (1L << 23) - x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 2048 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_2048_mont_norm_45(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<44; i++)
        r[i] = 0x7fffff;
#else
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i + 0] = 0x7fffff;
        r[i + 1] = 0x7fffff;
        r[i + 2] = 0x7fffff;
        r[i + 3] = 0x7fffff;
        r[i + 4] = 0x7fffff;
        r[i + 5] = 0x7fffff;
        r[i + 6] = 0x7fffff;
        r[i + 7] = 0x7fffff;
    }
    r[40] = 0x7fffff;
    r[41] = 0x7fffff;
    r[42] = 0x7fffff;
    r[43] = 0x7fffff;
#endif
    r[44] = 0xfffl;

    /* r = (2^n - 1) mod n */
    sp_2048_sub_45(r, r, m);

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
static sp_digit sp_2048_cmp_45(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=44; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[44] - b[44]) & (0 - !r);
    r |= (a[43] - b[43]) & (0 - !r);
    r |= (a[42] - b[42]) & (0 - !r);
    r |= (a[41] - b[41]) & (0 - !r);
    r |= (a[40] - b[40]) & (0 - !r);
    for (i = 32; i >= 0; i -= 8) {
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
static void sp_2048_cond_sub_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 45; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[40] = a[40] - (b[40] & m);
    r[41] = a[41] - (b[41] & m);
    r[42] = a[42] - (b[42] & m);
    r[43] = a[43] - (b[43] & m);
    r[44] = a[44] - (b[44] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_add_45(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 45; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[45] += t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x7fffff;
    for (i = 0; i < 40; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[41]; r[41] += (t[0] >> 23) + (t[1] & 0x7fffff);
    t[2] = tb * a[42]; r[42] += (t[1] >> 23) + (t[2] & 0x7fffff);
    t[3] = tb * a[43]; r[43] += (t[2] >> 23) + (t[3] & 0x7fffff);
    t[4] = tb * a[44]; r[44] += (t[3] >> 23) + (t[4] & 0x7fffff);
    r[45] +=  t[4] >> 23;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 23.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_2048_norm_45(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 44; i++) {
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
#else
    int i;
    for (i = 0; i < 40; i += 8) {
        a[i+1] += a[i+0] >> 23; a[i+0] &= 0x7fffff;
        a[i+2] += a[i+1] >> 23; a[i+1] &= 0x7fffff;
        a[i+3] += a[i+2] >> 23; a[i+2] &= 0x7fffff;
        a[i+4] += a[i+3] >> 23; a[i+3] &= 0x7fffff;
        a[i+5] += a[i+4] >> 23; a[i+4] &= 0x7fffff;
        a[i+6] += a[i+5] >> 23; a[i+5] &= 0x7fffff;
        a[i+7] += a[i+6] >> 23; a[i+6] &= 0x7fffff;
        a[i+8] += a[i+7] >> 23; a[i+7] &= 0x7fffff;
        a[i+9] += a[i+8] >> 23; a[i+8] &= 0x7fffff;
    }
    a[40+1] += a[40] >> 23;
    a[40] &= 0x7fffff;
    a[41+1] += a[41] >> 23;
    a[41] &= 0x7fffff;
    a[42+1] += a[42] >> 23;
    a[42] &= 0x7fffff;
    a[43+1] += a[43] >> 23;
    a[43] &= 0x7fffff;
#endif
}

/* Shift the result in the high 1024 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_2048_mont_shift_45(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    int64_t n = a[44] >> 12;
    n += ((int64_t)a[45]) << 11;

    for (i = 0; i < 44; i++) {
        r[i] = n & 0x7fffff;
        n >>= 23;
        n += ((int64_t)a[46 + i]) << 11;
    }
    r[44] = (sp_digit)n;
#else
    int i;
    int64_t n = a[44] >> 12;
    n += ((int64_t)a[45]) << 11;
    for (i = 0; i < 40; i += 8) {
        r[i + 0] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 46]) << 11;
        r[i + 1] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 47]) << 11;
        r[i + 2] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 48]) << 11;
        r[i + 3] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 49]) << 11;
        r[i + 4] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 50]) << 11;
        r[i + 5] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 51]) << 11;
        r[i + 6] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 52]) << 11;
        r[i + 7] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 53]) << 11;
    }
    r[40] = n & 0x7fffff; n >>= 23; n += ((int64_t)a[86]) << 11;
    r[41] = n & 0x7fffff; n >>= 23; n += ((int64_t)a[87]) << 11;
    r[42] = n & 0x7fffff; n >>= 23; n += ((int64_t)a[88]) << 11;
    r[43] = n & 0x7fffff; n >>= 23; n += ((int64_t)a[89]) << 11;
    r[44] = (sp_digit)n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[45], 0, sizeof(*r) * 45);
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_2048_mont_reduce_45(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    for (i=0; i<44; i++) {
        mu = (a[i] * mp) & 0x7fffff;
        sp_2048_mul_add_45(a+i, m, mu);
        a[i+1] += a[i] >> 23;
    }
    mu = (a[i] * mp) & 0xfffl;
    sp_2048_mul_add_45(a+i, m, mu);
    a[i+1] += a[i] >> 23;
    a[i] &= 0x7fffff;

    sp_2048_mont_shift_45(a, a);
    sp_2048_cond_sub_45(a, a, m, 0 - ((a[44] >> 12) > 0));
    sp_2048_norm_45(a);
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
static void sp_2048_mont_mul_45(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_45(r, a, b);
    sp_2048_mont_reduce_45(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_45(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_45(r, a);
    sp_2048_mont_reduce_45(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_d_45(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 45; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[45] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 40; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[41];
    r[41] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
    t[2] = tb * a[42];
    r[42] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
    t[3] = tb * a[43];
    r[43] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
    t[4] = tb * a[44];
    r[44] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
    r[45] =  (sp_digit)(t[4] >> 23);
#endif /* WOLFSSL_SP_SMALL */
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_d_90(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 90; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[90] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 88; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[89];
    r[89] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
    r[90] =  (sp_digit)(t[1] >> 23);
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
static void sp_2048_cond_add_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 45; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[40] = a[40] + (b[40] & m);
    r[41] = a[41] + (b[41] & m);
    r[42] = a[42] + (b[42] & m);
    r[43] = a[43] + (b[43] & m);
    r[44] = a[44] + (b[44] & m);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_add_45(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 45; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif
SP_NOINLINE static void sp_2048_rshift_45(sp_digit* r, sp_digit* a, byte n)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<44; i++)
        r[i] = ((a[i] >> n) | (a[i + 1] << (23 - n))) & 0x7fffff;
#else
    r[0] = ((a[0] >> n) | (a[1] << (23 - n))) & 0x7fffff;
    r[1] = ((a[1] >> n) | (a[2] << (23 - n))) & 0x7fffff;
    r[2] = ((a[2] >> n) | (a[3] << (23 - n))) & 0x7fffff;
    r[3] = ((a[3] >> n) | (a[4] << (23 - n))) & 0x7fffff;
    r[4] = ((a[4] >> n) | (a[5] << (23 - n))) & 0x7fffff;
    r[5] = ((a[5] >> n) | (a[6] << (23 - n))) & 0x7fffff;
    r[6] = ((a[6] >> n) | (a[7] << (23 - n))) & 0x7fffff;
    r[7] = ((a[7] >> n) | (a[8] << (23 - n))) & 0x7fffff;
    r[8] = ((a[8] >> n) | (a[9] << (23 - n))) & 0x7fffff;
    r[9] = ((a[9] >> n) | (a[10] << (23 - n))) & 0x7fffff;
    r[10] = ((a[10] >> n) | (a[11] << (23 - n))) & 0x7fffff;
    r[11] = ((a[11] >> n) | (a[12] << (23 - n))) & 0x7fffff;
    r[12] = ((a[12] >> n) | (a[13] << (23 - n))) & 0x7fffff;
    r[13] = ((a[13] >> n) | (a[14] << (23 - n))) & 0x7fffff;
    r[14] = ((a[14] >> n) | (a[15] << (23 - n))) & 0x7fffff;
    r[15] = ((a[15] >> n) | (a[16] << (23 - n))) & 0x7fffff;
    r[16] = ((a[16] >> n) | (a[17] << (23 - n))) & 0x7fffff;
    r[17] = ((a[17] >> n) | (a[18] << (23 - n))) & 0x7fffff;
    r[18] = ((a[18] >> n) | (a[19] << (23 - n))) & 0x7fffff;
    r[19] = ((a[19] >> n) | (a[20] << (23 - n))) & 0x7fffff;
    r[20] = ((a[20] >> n) | (a[21] << (23 - n))) & 0x7fffff;
    r[21] = ((a[21] >> n) | (a[22] << (23 - n))) & 0x7fffff;
    r[22] = ((a[22] >> n) | (a[23] << (23 - n))) & 0x7fffff;
    r[23] = ((a[23] >> n) | (a[24] << (23 - n))) & 0x7fffff;
    r[24] = ((a[24] >> n) | (a[25] << (23 - n))) & 0x7fffff;
    r[25] = ((a[25] >> n) | (a[26] << (23 - n))) & 0x7fffff;
    r[26] = ((a[26] >> n) | (a[27] << (23 - n))) & 0x7fffff;
    r[27] = ((a[27] >> n) | (a[28] << (23 - n))) & 0x7fffff;
    r[28] = ((a[28] >> n) | (a[29] << (23 - n))) & 0x7fffff;
    r[29] = ((a[29] >> n) | (a[30] << (23 - n))) & 0x7fffff;
    r[30] = ((a[30] >> n) | (a[31] << (23 - n))) & 0x7fffff;
    r[31] = ((a[31] >> n) | (a[32] << (23 - n))) & 0x7fffff;
    r[32] = ((a[32] >> n) | (a[33] << (23 - n))) & 0x7fffff;
    r[33] = ((a[33] >> n) | (a[34] << (23 - n))) & 0x7fffff;
    r[34] = ((a[34] >> n) | (a[35] << (23 - n))) & 0x7fffff;
    r[35] = ((a[35] >> n) | (a[36] << (23 - n))) & 0x7fffff;
    r[36] = ((a[36] >> n) | (a[37] << (23 - n))) & 0x7fffff;
    r[37] = ((a[37] >> n) | (a[38] << (23 - n))) & 0x7fffff;
    r[38] = ((a[38] >> n) | (a[39] << (23 - n))) & 0x7fffff;
    r[39] = ((a[39] >> n) | (a[40] << (23 - n))) & 0x7fffff;
    r[40] = ((a[40] >> n) | (a[41] << (23 - n))) & 0x7fffff;
    r[41] = ((a[41] >> n) | (a[42] << (23 - n))) & 0x7fffff;
    r[42] = ((a[42] >> n) | (a[43] << (23 - n))) & 0x7fffff;
    r[43] = ((a[43] >> n) | (a[44] << (23 - n))) & 0x7fffff;
#endif
    r[44] = a[44] >> n;
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
static int sp_2048_div_45(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int64_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[90 + 1], t2d[45 + 1], sdd[45 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    sp_digit* sd;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (4 * 45 + 3), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 90 + 1;
        sd = t2 + 45 + 1;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
    sd = sdd;
#endif

    (void)m;

    if (err == MP_OKAY) {
        sp_2048_mul_d_45(sd, d, 1 << 11);
        sp_2048_mul_d_90(t1, a, 1 << 11);
        div = sd[44];
        for (i=45; i>=0; i--) {
            t1[45 + i] += t1[45 + i - 1] >> 23;
            t1[45 + i - 1] &= 0x7fffff;
            d1 = t1[45 + i];
            d1 <<= 23;
            d1 += t1[45 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_2048_mul_d_45(t2, sd, r1);
            sp_2048_sub_45(&t1[i], &t1[i], t2);
            t1[45 + i] -= t2[45];
            t1[45 + i] += t1[45 + i - 1] >> 23;
            t1[45 + i - 1] &= 0x7fffff;
            r1 = (((-t1[45 + i]) << 23) - t1[45 + i - 1]) / div;
            r1 -= t1[45 + i];
            sp_2048_mul_d_45(t2, sd, r1);
            sp_2048_add_45(&t1[i], &t1[i], t2);
            t1[45 + i] += t1[45 + i - 1] >> 23;
            t1[45 + i - 1] &= 0x7fffff;
        }
        t1[45 - 1] += t1[45 - 2] >> 23;
        t1[45 - 2] &= 0x7fffff;
        d1 = t1[45 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_2048_mul_d_45(t2, sd, r1);
        sp_2048_sub_45(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 45);
        for (i=0; i<43; i++) {
            r[i+1] += r[i] >> 23;
            r[i] &= 0x7fffff;
        }
        sp_2048_cond_add_45(r, r, sd, 0 - (r[44] < 0));
    }

    sp_2048_rshift_45(r, r, 11);

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
static int sp_2048_mod_45(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_45(a, m, NULL, r);
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
static int sp_2048_mod_exp_45(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
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

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 45 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 45 * 2);

        norm = t[0] = td;
        t[1] = &td[45 * 2];
        t[2] = &td[2 * 45 * 2];

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_45(norm, m);

        if (reduceA)
            err = sp_2048_mod_45(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 45);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_45(t[1], t[1], norm);
        err = sp_2048_mod_45(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_2048_mont_mul_45(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 45 * 2);
            sp_2048_mont_sqr_45(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 45 * 2);
        }

        sp_2048_mont_reduce_45(t[0], m, mp);
        n = sp_2048_cmp_45(t[0], m);
        sp_2048_cond_sub_45(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 45 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][90];
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
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 45 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[45 * 2];
        t[2] = &td[2 * 45 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_45(norm, m);

        if (reduceA) {
            err = sp_2048_mod_45(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_45(t[1], t[1], norm);
                err = sp_2048_mod_45(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_45(t[1], a, norm);
            err = sp_2048_mod_45(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_2048_mont_mul_45(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_2048_mont_sqr_45(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_2048_mont_reduce_45(t[0], m, mp);
        n = sp_2048_cmp_45(t[0], m);
        sp_2048_cond_sub_45(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][90];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[90];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 90, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 90;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_45(norm, m);

        if (reduceA) {
            err = sp_2048_mod_45(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_45(t[1], t[1], norm);
                err = sp_2048_mod_45(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_45(t[1], a, norm);
            err = sp_2048_mod_45(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_45(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_45(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_45(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_45(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_45(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_45(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_45(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_45(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_45(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_45(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_45(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_45(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_45(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_45(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_45(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_45(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_45(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_45(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_45(t[20], t[10], m, mp);
        sp_2048_mont_mul_45(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_45(t[22], t[11], m, mp);
        sp_2048_mont_mul_45(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_45(t[24], t[12], m, mp);
        sp_2048_mont_mul_45(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_45(t[26], t[13], m, mp);
        sp_2048_mont_mul_45(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_45(t[28], t[14], m, mp);
        sp_2048_mont_mul_45(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_45(t[30], t[15], m, mp);
        sp_2048_mont_mul_45(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 22) / 23) - 1;
        c = bits % 23;
        if (c == 0)
            c = 23;
        if (i < 45)
            n = e[i--] << (32 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (9 - c);
            c += 23;
        }
        y = n >> 27;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (9 - c);
                c += 23;
            }
            y = (n >> 27) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_2048_mont_sqr_45(rt, rt, m, mp);
            sp_2048_mont_sqr_45(rt, rt, m, mp);
            sp_2048_mont_sqr_45(rt, rt, m, mp);
            sp_2048_mont_sqr_45(rt, rt, m, mp);
            sp_2048_mont_sqr_45(rt, rt, m, mp);

            sp_2048_mont_mul_45(rt, rt, t[y], m, mp);
        }

        sp_2048_mont_reduce_45(rt, m, mp);
        n = sp_2048_cmp_45(rt, m);
        sp_2048_cond_sub_45(rt, rt, m, (n < 0) - 1);
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
static void sp_2048_mont_norm_90(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<89; i++)
        r[i] = 0x7fffff;
#else
    int i;

    for (i = 0; i < 88; i += 8) {
        r[i + 0] = 0x7fffff;
        r[i + 1] = 0x7fffff;
        r[i + 2] = 0x7fffff;
        r[i + 3] = 0x7fffff;
        r[i + 4] = 0x7fffff;
        r[i + 5] = 0x7fffff;
        r[i + 6] = 0x7fffff;
        r[i + 7] = 0x7fffff;
    }
    r[88] = 0x7fffff;
#endif
    r[89] = 0x1l;

    /* r = (2^n - 1) mod n */
    sp_2048_sub_90(r, r, m);

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
static sp_digit sp_2048_cmp_90(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=89; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[89] - b[89]) & (0 - !r);
    r |= (a[88] - b[88]) & (0 - !r);
    for (i = 80; i >= 0; i -= 8) {
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
static void sp_2048_cond_sub_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 90; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 88; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[88] = a[88] - (b[88] & m);
    r[89] = a[89] - (b[89] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_add_90(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 90; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[90] += t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x7fffff;
    for (i = 0; i < 88; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[89]; r[89] += (t[0] >> 23) + (t[1] & 0x7fffff);
    r[90] +=  t[1] >> 23;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 23.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_2048_norm_90(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 89; i++) {
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
#else
    int i;
    for (i = 0; i < 88; i += 8) {
        a[i+1] += a[i+0] >> 23; a[i+0] &= 0x7fffff;
        a[i+2] += a[i+1] >> 23; a[i+1] &= 0x7fffff;
        a[i+3] += a[i+2] >> 23; a[i+2] &= 0x7fffff;
        a[i+4] += a[i+3] >> 23; a[i+3] &= 0x7fffff;
        a[i+5] += a[i+4] >> 23; a[i+4] &= 0x7fffff;
        a[i+6] += a[i+5] >> 23; a[i+5] &= 0x7fffff;
        a[i+7] += a[i+6] >> 23; a[i+6] &= 0x7fffff;
        a[i+8] += a[i+7] >> 23; a[i+7] &= 0x7fffff;
        a[i+9] += a[i+8] >> 23; a[i+8] &= 0x7fffff;
    }
    a[88+1] += a[88] >> 23;
    a[88] &= 0x7fffff;
#endif
}

/* Shift the result in the high 2048 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_2048_mont_shift_90(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    int64_t n = a[89] >> 1;
    n += ((int64_t)a[90]) << 22;

    for (i = 0; i < 89; i++) {
        r[i] = n & 0x7fffff;
        n >>= 23;
        n += ((int64_t)a[91 + i]) << 22;
    }
    r[89] = (sp_digit)n;
#else
    int i;
    int64_t n = a[89] >> 1;
    n += ((int64_t)a[90]) << 22;
    for (i = 0; i < 88; i += 8) {
        r[i + 0] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 91]) << 22;
        r[i + 1] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 92]) << 22;
        r[i + 2] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 93]) << 22;
        r[i + 3] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 94]) << 22;
        r[i + 4] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 95]) << 22;
        r[i + 5] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 96]) << 22;
        r[i + 6] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 97]) << 22;
        r[i + 7] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 98]) << 22;
    }
    r[88] = n & 0x7fffff; n >>= 23; n += ((int64_t)a[179]) << 22;
    r[89] = (sp_digit)n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[90], 0, sizeof(*r) * 90);
}

/* Reduce the number back to 2048 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_2048_mont_reduce_90(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<89; i++) {
            mu = (a[i] * mp) & 0x7fffff;
            sp_2048_mul_add_90(a+i, m, mu);
            a[i+1] += a[i] >> 23;
        }
        mu = (a[i] * mp) & 0x1l;
        sp_2048_mul_add_90(a+i, m, mu);
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
    else {
        for (i=0; i<89; i++) {
            mu = a[i] & 0x7fffff;
            sp_2048_mul_add_90(a+i, m, mu);
            a[i+1] += a[i] >> 23;
        }
        mu = a[i] & 0x1l;
        sp_2048_mul_add_90(a+i, m, mu);
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }

    sp_2048_mont_shift_90(a, a);
    sp_2048_cond_sub_90(a, a, m, 0 - ((a[89] >> 1) > 0));
    sp_2048_norm_90(a);
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
static void sp_2048_mont_mul_90(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_2048_mul_90(r, a, b);
    sp_2048_mont_reduce_90(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_2048_mont_sqr_90(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_2048_sqr_90(r, a);
    sp_2048_mont_reduce_90(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_2048_mul_d_180(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 180; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[180] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 176; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[177];
    r[177] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
    t[2] = tb * a[178];
    r[178] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
    t[3] = tb * a[179];
    r[179] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
    r[180] =  (sp_digit)(t[3] >> 23);
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
static void sp_2048_cond_add_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 90; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 88; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[88] = a[88] + (b[88] & m);
    r[89] = a[89] + (b[89] & m);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_2048_sub_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 90; i++)
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
SP_NOINLINE static int sp_2048_add_90(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 90; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif
SP_NOINLINE static void sp_2048_rshift_90(sp_digit* r, sp_digit* a, byte n)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<89; i++)
        r[i] = ((a[i] >> n) | (a[i + 1] << (23 - n))) & 0x7fffff;
#else
    r[0] = ((a[0] >> n) | (a[1] << (23 - n))) & 0x7fffff;
    r[1] = ((a[1] >> n) | (a[2] << (23 - n))) & 0x7fffff;
    r[2] = ((a[2] >> n) | (a[3] << (23 - n))) & 0x7fffff;
    r[3] = ((a[3] >> n) | (a[4] << (23 - n))) & 0x7fffff;
    r[4] = ((a[4] >> n) | (a[5] << (23 - n))) & 0x7fffff;
    r[5] = ((a[5] >> n) | (a[6] << (23 - n))) & 0x7fffff;
    r[6] = ((a[6] >> n) | (a[7] << (23 - n))) & 0x7fffff;
    r[7] = ((a[7] >> n) | (a[8] << (23 - n))) & 0x7fffff;
    r[8] = ((a[8] >> n) | (a[9] << (23 - n))) & 0x7fffff;
    r[9] = ((a[9] >> n) | (a[10] << (23 - n))) & 0x7fffff;
    r[10] = ((a[10] >> n) | (a[11] << (23 - n))) & 0x7fffff;
    r[11] = ((a[11] >> n) | (a[12] << (23 - n))) & 0x7fffff;
    r[12] = ((a[12] >> n) | (a[13] << (23 - n))) & 0x7fffff;
    r[13] = ((a[13] >> n) | (a[14] << (23 - n))) & 0x7fffff;
    r[14] = ((a[14] >> n) | (a[15] << (23 - n))) & 0x7fffff;
    r[15] = ((a[15] >> n) | (a[16] << (23 - n))) & 0x7fffff;
    r[16] = ((a[16] >> n) | (a[17] << (23 - n))) & 0x7fffff;
    r[17] = ((a[17] >> n) | (a[18] << (23 - n))) & 0x7fffff;
    r[18] = ((a[18] >> n) | (a[19] << (23 - n))) & 0x7fffff;
    r[19] = ((a[19] >> n) | (a[20] << (23 - n))) & 0x7fffff;
    r[20] = ((a[20] >> n) | (a[21] << (23 - n))) & 0x7fffff;
    r[21] = ((a[21] >> n) | (a[22] << (23 - n))) & 0x7fffff;
    r[22] = ((a[22] >> n) | (a[23] << (23 - n))) & 0x7fffff;
    r[23] = ((a[23] >> n) | (a[24] << (23 - n))) & 0x7fffff;
    r[24] = ((a[24] >> n) | (a[25] << (23 - n))) & 0x7fffff;
    r[25] = ((a[25] >> n) | (a[26] << (23 - n))) & 0x7fffff;
    r[26] = ((a[26] >> n) | (a[27] << (23 - n))) & 0x7fffff;
    r[27] = ((a[27] >> n) | (a[28] << (23 - n))) & 0x7fffff;
    r[28] = ((a[28] >> n) | (a[29] << (23 - n))) & 0x7fffff;
    r[29] = ((a[29] >> n) | (a[30] << (23 - n))) & 0x7fffff;
    r[30] = ((a[30] >> n) | (a[31] << (23 - n))) & 0x7fffff;
    r[31] = ((a[31] >> n) | (a[32] << (23 - n))) & 0x7fffff;
    r[32] = ((a[32] >> n) | (a[33] << (23 - n))) & 0x7fffff;
    r[33] = ((a[33] >> n) | (a[34] << (23 - n))) & 0x7fffff;
    r[34] = ((a[34] >> n) | (a[35] << (23 - n))) & 0x7fffff;
    r[35] = ((a[35] >> n) | (a[36] << (23 - n))) & 0x7fffff;
    r[36] = ((a[36] >> n) | (a[37] << (23 - n))) & 0x7fffff;
    r[37] = ((a[37] >> n) | (a[38] << (23 - n))) & 0x7fffff;
    r[38] = ((a[38] >> n) | (a[39] << (23 - n))) & 0x7fffff;
    r[39] = ((a[39] >> n) | (a[40] << (23 - n))) & 0x7fffff;
    r[40] = ((a[40] >> n) | (a[41] << (23 - n))) & 0x7fffff;
    r[41] = ((a[41] >> n) | (a[42] << (23 - n))) & 0x7fffff;
    r[42] = ((a[42] >> n) | (a[43] << (23 - n))) & 0x7fffff;
    r[43] = ((a[43] >> n) | (a[44] << (23 - n))) & 0x7fffff;
    r[44] = ((a[44] >> n) | (a[45] << (23 - n))) & 0x7fffff;
    r[45] = ((a[45] >> n) | (a[46] << (23 - n))) & 0x7fffff;
    r[46] = ((a[46] >> n) | (a[47] << (23 - n))) & 0x7fffff;
    r[47] = ((a[47] >> n) | (a[48] << (23 - n))) & 0x7fffff;
    r[48] = ((a[48] >> n) | (a[49] << (23 - n))) & 0x7fffff;
    r[49] = ((a[49] >> n) | (a[50] << (23 - n))) & 0x7fffff;
    r[50] = ((a[50] >> n) | (a[51] << (23 - n))) & 0x7fffff;
    r[51] = ((a[51] >> n) | (a[52] << (23 - n))) & 0x7fffff;
    r[52] = ((a[52] >> n) | (a[53] << (23 - n))) & 0x7fffff;
    r[53] = ((a[53] >> n) | (a[54] << (23 - n))) & 0x7fffff;
    r[54] = ((a[54] >> n) | (a[55] << (23 - n))) & 0x7fffff;
    r[55] = ((a[55] >> n) | (a[56] << (23 - n))) & 0x7fffff;
    r[56] = ((a[56] >> n) | (a[57] << (23 - n))) & 0x7fffff;
    r[57] = ((a[57] >> n) | (a[58] << (23 - n))) & 0x7fffff;
    r[58] = ((a[58] >> n) | (a[59] << (23 - n))) & 0x7fffff;
    r[59] = ((a[59] >> n) | (a[60] << (23 - n))) & 0x7fffff;
    r[60] = ((a[60] >> n) | (a[61] << (23 - n))) & 0x7fffff;
    r[61] = ((a[61] >> n) | (a[62] << (23 - n))) & 0x7fffff;
    r[62] = ((a[62] >> n) | (a[63] << (23 - n))) & 0x7fffff;
    r[63] = ((a[63] >> n) | (a[64] << (23 - n))) & 0x7fffff;
    r[64] = ((a[64] >> n) | (a[65] << (23 - n))) & 0x7fffff;
    r[65] = ((a[65] >> n) | (a[66] << (23 - n))) & 0x7fffff;
    r[66] = ((a[66] >> n) | (a[67] << (23 - n))) & 0x7fffff;
    r[67] = ((a[67] >> n) | (a[68] << (23 - n))) & 0x7fffff;
    r[68] = ((a[68] >> n) | (a[69] << (23 - n))) & 0x7fffff;
    r[69] = ((a[69] >> n) | (a[70] << (23 - n))) & 0x7fffff;
    r[70] = ((a[70] >> n) | (a[71] << (23 - n))) & 0x7fffff;
    r[71] = ((a[71] >> n) | (a[72] << (23 - n))) & 0x7fffff;
    r[72] = ((a[72] >> n) | (a[73] << (23 - n))) & 0x7fffff;
    r[73] = ((a[73] >> n) | (a[74] << (23 - n))) & 0x7fffff;
    r[74] = ((a[74] >> n) | (a[75] << (23 - n))) & 0x7fffff;
    r[75] = ((a[75] >> n) | (a[76] << (23 - n))) & 0x7fffff;
    r[76] = ((a[76] >> n) | (a[77] << (23 - n))) & 0x7fffff;
    r[77] = ((a[77] >> n) | (a[78] << (23 - n))) & 0x7fffff;
    r[78] = ((a[78] >> n) | (a[79] << (23 - n))) & 0x7fffff;
    r[79] = ((a[79] >> n) | (a[80] << (23 - n))) & 0x7fffff;
    r[80] = ((a[80] >> n) | (a[81] << (23 - n))) & 0x7fffff;
    r[81] = ((a[81] >> n) | (a[82] << (23 - n))) & 0x7fffff;
    r[82] = ((a[82] >> n) | (a[83] << (23 - n))) & 0x7fffff;
    r[83] = ((a[83] >> n) | (a[84] << (23 - n))) & 0x7fffff;
    r[84] = ((a[84] >> n) | (a[85] << (23 - n))) & 0x7fffff;
    r[85] = ((a[85] >> n) | (a[86] << (23 - n))) & 0x7fffff;
    r[86] = ((a[86] >> n) | (a[87] << (23 - n))) & 0x7fffff;
    r[87] = ((a[87] >> n) | (a[88] << (23 - n))) & 0x7fffff;
    r[88] = ((a[88] >> n) | (a[89] << (23 - n))) & 0x7fffff;
#endif
    r[89] = a[89] >> n;
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
static int sp_2048_div_90(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int64_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[180 + 1], t2d[90 + 1], sdd[90 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    sp_digit* sd;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (4 * 90 + 3), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 180 + 1;
        sd = t2 + 90 + 1;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
    sd = sdd;
#endif

    (void)m;

    if (err == MP_OKAY) {
        sp_2048_mul_d_90(sd, d, 1 << 22);
        sp_2048_mul_d_180(t1, a, 1 << 22);
        div = sd[89];
        for (i=90; i>=0; i--) {
            t1[90 + i] += t1[90 + i - 1] >> 23;
            t1[90 + i - 1] &= 0x7fffff;
            d1 = t1[90 + i];
            d1 <<= 23;
            d1 += t1[90 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_2048_mul_d_90(t2, sd, r1);
            sp_2048_sub_90(&t1[i], &t1[i], t2);
            t1[90 + i] -= t2[90];
            t1[90 + i] += t1[90 + i - 1] >> 23;
            t1[90 + i - 1] &= 0x7fffff;
            r1 = (((-t1[90 + i]) << 23) - t1[90 + i - 1]) / div;
            r1 -= t1[90 + i];
            sp_2048_mul_d_90(t2, sd, r1);
            sp_2048_add_90(&t1[i], &t1[i], t2);
            t1[90 + i] += t1[90 + i - 1] >> 23;
            t1[90 + i - 1] &= 0x7fffff;
        }
        t1[90 - 1] += t1[90 - 2] >> 23;
        t1[90 - 2] &= 0x7fffff;
        d1 = t1[90 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_2048_mul_d_90(t2, sd, r1);
        sp_2048_sub_90(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 90);
        for (i=0; i<88; i++) {
            r[i+1] += r[i] >> 23;
            r[i] &= 0x7fffff;
        }
        sp_2048_cond_add_90(r, r, sd, 0 - (r[89] < 0));
    }

    sp_2048_rshift_90(r, r, 22);

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
static int sp_2048_mod_90(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_2048_div_90(a, m, NULL, r);
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
static int sp_2048_mod_exp_90(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
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

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 90 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 90 * 2);

        norm = t[0] = td;
        t[1] = &td[90 * 2];
        t[2] = &td[2 * 90 * 2];

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_90(norm, m);

        if (reduceA)
            err = sp_2048_mod_90(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 90);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_90(t[1], t[1], norm);
        err = sp_2048_mod_90(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_2048_mont_mul_90(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 90 * 2);
            sp_2048_mont_sqr_90(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 90 * 2);
        }

        sp_2048_mont_reduce_90(t[0], m, mp);
        n = sp_2048_cmp_90(t[0], m);
        sp_2048_cond_sub_90(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 90 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][180];
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
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 90 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[90 * 2];
        t[2] = &td[2 * 90 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_90(norm, m);

        if (reduceA) {
            err = sp_2048_mod_90(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_90(t[1], t[1], norm);
                err = sp_2048_mod_90(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_90(t[1], a, norm);
            err = sp_2048_mod_90(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_2048_mont_mul_90(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_2048_mont_sqr_90(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_2048_mont_reduce_90(t[0], m, mp);
        n = sp_2048_cmp_90(t[0], m);
        sp_2048_cond_sub_90(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][180];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[180];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 180, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 180;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_90(norm, m);

        if (reduceA) {
            err = sp_2048_mod_90(t[1], a, m);
            if (err == MP_OKAY) {
                sp_2048_mul_90(t[1], t[1], norm);
                err = sp_2048_mod_90(t[1], t[1], m);
            }
        }
        else {
            sp_2048_mul_90(t[1], a, norm);
            err = sp_2048_mod_90(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_2048_mont_sqr_90(t[ 2], t[ 1], m, mp);
        sp_2048_mont_mul_90(t[ 3], t[ 2], t[ 1], m, mp);
        sp_2048_mont_sqr_90(t[ 4], t[ 2], m, mp);
        sp_2048_mont_mul_90(t[ 5], t[ 3], t[ 2], m, mp);
        sp_2048_mont_sqr_90(t[ 6], t[ 3], m, mp);
        sp_2048_mont_mul_90(t[ 7], t[ 4], t[ 3], m, mp);
        sp_2048_mont_sqr_90(t[ 8], t[ 4], m, mp);
        sp_2048_mont_mul_90(t[ 9], t[ 5], t[ 4], m, mp);
        sp_2048_mont_sqr_90(t[10], t[ 5], m, mp);
        sp_2048_mont_mul_90(t[11], t[ 6], t[ 5], m, mp);
        sp_2048_mont_sqr_90(t[12], t[ 6], m, mp);
        sp_2048_mont_mul_90(t[13], t[ 7], t[ 6], m, mp);
        sp_2048_mont_sqr_90(t[14], t[ 7], m, mp);
        sp_2048_mont_mul_90(t[15], t[ 8], t[ 7], m, mp);
        sp_2048_mont_sqr_90(t[16], t[ 8], m, mp);
        sp_2048_mont_mul_90(t[17], t[ 9], t[ 8], m, mp);
        sp_2048_mont_sqr_90(t[18], t[ 9], m, mp);
        sp_2048_mont_mul_90(t[19], t[10], t[ 9], m, mp);
        sp_2048_mont_sqr_90(t[20], t[10], m, mp);
        sp_2048_mont_mul_90(t[21], t[11], t[10], m, mp);
        sp_2048_mont_sqr_90(t[22], t[11], m, mp);
        sp_2048_mont_mul_90(t[23], t[12], t[11], m, mp);
        sp_2048_mont_sqr_90(t[24], t[12], m, mp);
        sp_2048_mont_mul_90(t[25], t[13], t[12], m, mp);
        sp_2048_mont_sqr_90(t[26], t[13], m, mp);
        sp_2048_mont_mul_90(t[27], t[14], t[13], m, mp);
        sp_2048_mont_sqr_90(t[28], t[14], m, mp);
        sp_2048_mont_mul_90(t[29], t[15], t[14], m, mp);
        sp_2048_mont_sqr_90(t[30], t[15], m, mp);
        sp_2048_mont_mul_90(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 22) / 23) - 1;
        c = bits % 23;
        if (c == 0)
            c = 23;
        if (i < 90)
            n = e[i--] << (32 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (9 - c);
            c += 23;
        }
        y = n >> 27;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (9 - c);
                c += 23;
            }
            y = (n >> 27) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_2048_mont_sqr_90(rt, rt, m, mp);
            sp_2048_mont_sqr_90(rt, rt, m, mp);
            sp_2048_mont_sqr_90(rt, rt, m, mp);
            sp_2048_mont_sqr_90(rt, rt, m, mp);
            sp_2048_mont_sqr_90(rt, rt, m, mp);

            sp_2048_mont_mul_90(rt, rt, t[y], m, mp);
        }

        sp_2048_mont_reduce_90(rt, m, mp);
        n = sp_2048_cmp_90(rt, m);
        sp_2048_cond_sub_90(rt, rt, m, (n < 0) - 1);
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
static void sp_2048_mask_45(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<45; i++)
        r[i] = a[i] & m;
#else
    int i;

    for (i = 0; i < 40; i += 8) {
        r[i+0] = a[i+0] & m;
        r[i+1] = a[i+1] & m;
        r[i+2] = a[i+2] & m;
        r[i+3] = a[i+3] & m;
        r[i+4] = a[i+4] & m;
        r[i+5] = a[i+5] & m;
        r[i+6] = a[i+6] & m;
        r[i+7] = a[i+7] & m;
    }
    r[40] = a[40] & m;
    r[41] = a[41] & m;
    r[42] = a[42] & m;
    r[43] = a[43] & m;
    r[44] = a[44] & m;
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
    if (err == MP_OKAY && (mp_count_bits(em) > 23 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 90 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 90 * 2;
        m = r + 90 * 2;
        norm = r;

        sp_2048_from_bin(a, 90, in, inLen);
#if DIGIT_BIT >= 23
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
        sp_2048_from_mp(m, 90, mm);

        sp_2048_mont_setup(m, &mp);
        sp_2048_mont_norm_90(norm, m);
    }
    if (err == MP_OKAY) {
        sp_2048_mul_90(a, a, norm);
        err = sp_2048_mod_90(a, a, m);
    }
    if (err == MP_OKAY) {
        for (i=22; i>=0; i--)
            if (e[0] >> i)
                break;

        XMEMCPY(r, a, sizeof(sp_digit) * 90 * 2);
        for (i--; i>=0; i--) {
            sp_2048_mont_sqr_90(r, r, m, mp);

            if (((e[0] >> i) & 1) == 1)
                sp_2048_mont_mul_90(r, r, a, m, mp);
        }
        sp_2048_mont_reduce_90(r, m, mp);
        mp = sp_2048_cmp_90(r, m);
        sp_2048_cond_sub_90(r, r, m, (mp < 0) - 1);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#else
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[180], md[90], rd[180];
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
    if (err == MP_OKAY && (mp_count_bits(em) > 23 || inLen > 256 ||
                                                     mp_count_bits(mm) != 2048))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 90 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 90 * 2;
        m = r + 90 * 2;
    }
#else
    a = ad;
    m = md;
    r = rd;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 90, in, inLen);
#if DIGIT_BIT >= 23
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
        sp_2048_from_mp(m, 90, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_2048_sqr_90(r, a);
                err = sp_2048_mod_90(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_2048_mul_90(r, a, r);
                err = sp_2048_mod_90(r, r, m);
            }
        }
        else {
            sp_digit* norm = r;
            int i;
            sp_digit mp;

            sp_2048_mont_setup(m, &mp);
            sp_2048_mont_norm_90(norm, m);

            if (err == MP_OKAY) {
                sp_2048_mul_90(a, a, norm);
                err = sp_2048_mod_90(a, a, m);
            }

            if (err == MP_OKAY) {
                for (i=22; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 180);
                for (i--; i>=0; i--) {
                    sp_2048_mont_sqr_90(r, r, m, mp);

                    if (((e[0] >> i) & 1) == 1)
                        sp_2048_mont_mul_90(r, r, a, m, mp);
                }
                sp_2048_mont_reduce_90(r, m, mp);
                mp = sp_2048_cmp_90(r, m);
                sp_2048_cond_sub_90(r, r, m, (mp < 0) - 1);
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
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 90 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = d + 90;
        m = a + 90;
        r = a;

        sp_2048_from_bin(a, 90, in, inLen);
        sp_2048_from_mp(d, 90, dm);
        sp_2048_from_mp(m, 90, mm);
        err = sp_2048_mod_exp_90(r, a, d, 2048, m, 0);
    }
    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (d != NULL) {
        XMEMSET(d, 0, sizeof(sp_digit) * 90);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[180], d[90], m[90];
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
        sp_2048_from_bin(a, 90, in, inLen);
        sp_2048_from_mp(d, 90, dm);
        sp_2048_from_mp(m, 90, mm);
        err = sp_2048_mod_exp_90(r, a, d, 2048, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    XMEMSET(d, 0, sizeof(sp_digit) * 90);

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
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 45 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 90 * 2;
        q = p + 45;
        qi = dq = dp = q + 45;
        tmpa = qi + 45;
        tmpb = tmpa + 90;

        tmp = t;
        r = tmp + 90;

        sp_2048_from_bin(a, 90, in, inLen);
        sp_2048_from_mp(p, 45, pm);
        sp_2048_from_mp(q, 45, qm);
        sp_2048_from_mp(dp, 45, dpm);
        err = sp_2048_mod_exp_45(tmpa, a, dp, 1024, p, 1);
    }
    if (err == MP_OKAY) {
        sp_2048_from_mp(dq, 45, dqm);
        err = sp_2048_mod_exp_45(tmpb, a, dq, 1024, q, 1);
    }
    if (err == MP_OKAY) {
        sp_2048_sub_45(tmpa, tmpa, tmpb);
        sp_2048_mask_45(tmp, p, tmpa[44] >> 31);
        sp_2048_add_45(tmpa, tmpa, tmp);

        sp_2048_from_mp(qi, 45, qim);
        sp_2048_mul_45(tmpa, tmpa, qi);
        err = sp_2048_mod_45(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_2048_mul_45(tmpa, q, tmpa);
        sp_2048_add_90(r, tmpb, tmpa);
        sp_2048_norm_90(r);

        sp_2048_to_bin(r, out);
        *outLen = 256;
    }

    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 45 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[90 * 2];
    sp_digit p[45], q[45], dp[45], dq[45], qi[45];
    sp_digit tmp[90], tmpa[90], tmpb[90];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 256)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 256 || mp_count_bits(mm) != 2048))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_2048_from_bin(a, 90, in, inLen);
        sp_2048_from_mp(p, 45, pm);
        sp_2048_from_mp(q, 45, qm);
        sp_2048_from_mp(dp, 45, dpm);
        sp_2048_from_mp(dq, 45, dqm);
        sp_2048_from_mp(qi, 45, qim);

        err = sp_2048_mod_exp_45(tmpa, a, dp, 1024, p, 1);
    }
    if (err == MP_OKAY)
        err = sp_2048_mod_exp_45(tmpb, a, dq, 1024, q, 1);

    if (err == MP_OKAY) {
        sp_2048_sub_45(tmpa, tmpa, tmpb);
        sp_2048_mask_45(tmp, p, tmpa[44] >> 31);
        sp_2048_add_45(tmpa, tmpa, tmp);
        sp_2048_mul_45(tmpa, tmpa, qi);
        err = sp_2048_mod_45(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_2048_mul_45(tmpa, tmpa, q);
        sp_2048_add_90(r, tmpb, tmpa);
        sp_2048_norm_90(r);

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
#if DIGIT_BIT == 23
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 90);
        r->used = 90;
        mp_clamp(r);
#elif DIGIT_BIT < 23
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 90; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 23) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 23 - s;
        }
        r->used = (2048 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 90; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 23 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 23 - s;
            }
            else
                s += 23;
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 90 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 90 * 2;
        m = e + 90;
        r = b;

        sp_2048_from_mp(b, 90, base);
        sp_2048_from_mp(e, 90, exp);
        sp_2048_from_mp(m, 90, mod);

        err = sp_2048_mod_exp_90(r, b, e, mp_count_bits(exp), m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_2048_to_mp(r, res);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 90);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[180], ed[90], md[90];
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 90 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 90 * 2;
        m = e + 90;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 90, base);
        sp_2048_from_mp(e, 90, exp);
        sp_2048_from_mp(m, 90, mod);

        err = sp_2048_mod_exp_90(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_2048_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 90);

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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 90 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 90 * 2;
        m = e + 90;
        r = b;

        sp_2048_from_mp(b, 90, base);
        sp_2048_from_bin(e, 90, exp, expLen);
        sp_2048_from_mp(m, 90, mod);

        err = sp_2048_mod_exp_90(r, b, e, expLen * 8, m, 0);
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
        XMEMSET(e, 0, sizeof(sp_digit) * 90);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[180], ed[90], md[90];
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 90 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 90 * 2;
        m = e + 90;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_2048_from_mp(b, 90, base);
        sp_2048_from_bin(e, 90, exp, expLen);
        sp_2048_from_mp(m, 90, mod);

        err = sp_2048_mod_exp_90(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_2048_to_bin(r, out);
        *outLen = 256;
        for (i=0; i<256 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 90);

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
        if (s >= 15) {
            r[j] &= 0x7fffff;
            s = 23 - s;
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
#if DIGIT_BIT == 23
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 23
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0x7fffff;
        s = 23 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 23 <= DIGIT_BIT) {
            s += 23;
            r[j] &= 0x7fffff;
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
        if (s + DIGIT_BIT >= 23) {
            r[j] &= 0x7fffff;
            if (j + 1 >= max)
                break;
            s = 23 - s;
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

    for (i=0; i<135; i++) {
        r[i+1] += r[i] >> 23;
        r[i] &= 0x7fffff;
    }
    j = 3072 / 8 - 1;
    a[j] = 0;
    for (i=0; i<136 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 23) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 23);
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
SP_NOINLINE static void sp_3072_mul_17(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j;
    int64_t t[34];

    XMEMSET(t, 0, sizeof(t));
    for (i=0; i<17; i++) {
        for (j=0; j<17; j++)
            t[i+j] += ((int64_t)a[i]) * b[j];
    }
    for (i=0; i<33; i++) {
        r[i] = t[i] & 0x7fffff;
        t[i+1] += t[i] >> 23;
    }
    r[33] = (sp_digit)t[33];
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_17(sp_digit* r, const sp_digit* a)
{
    int i, j;
    int64_t t[34];

    XMEMSET(t, 0, sizeof(t));
    for (i=0; i<17; i++) {
        for (j=0; j<i; j++)
            t[i+j] += (((int64_t)a[i]) * a[j]) * 2;
        t[i+i] += ((int64_t)a[i]) * a[i];
    }
    for (i=0; i<33; i++) {
        r[i] = t[i] & 0x7fffff;
        t[i+1] += t[i] >> 23;
    }
    r[33] = (sp_digit)t[33];
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_17(sp_digit* r, const sp_digit* a,
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

    return 0;
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_34(sp_digit* r, const sp_digit* a,
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

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_34(sp_digit* r, const sp_digit* a,
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

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_34(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[34];
    sp_digit* a1 = z1;
    sp_digit b1[17];
    sp_digit* z2 = r + 34;
    sp_3072_add_17(a1, a, &a[17]);
    sp_3072_add_17(b1, b, &b[17]);
    sp_3072_mul_17(z2, &a[17], &b[17]);
    sp_3072_mul_17(z0, a, b);
    sp_3072_mul_17(z1, a1, b1);
    sp_3072_sub_34(z1, z1, z2);
    sp_3072_sub_34(z1, z1, z0);
    sp_3072_add_34(r + 17, r + 17, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_34(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[34];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 34;
    sp_3072_add_17(a1, a, &a[17]);
    sp_3072_sqr_17(z2, &a[17]);
    sp_3072_sqr_17(z0, a);
    sp_3072_sqr_17(z1, a1);
    sp_3072_sub_34(z1, z1, z2);
    sp_3072_sub_34(z1, z1, z0);
    sp_3072_add_34(r + 17, r + 17, z1);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }
    r[64] = a[64] + b[64];
    r[65] = a[65] + b[65];
    r[66] = a[66] + b[66];
    r[67] = a[67] + b[67];

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }
    r[64] = a[64] - b[64];
    r[65] = a[65] - b[65];
    r[66] = a[66] - b[66];
    r[67] = a[67] - b[67];

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_68(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[68];
    sp_digit* a1 = z1;
    sp_digit b1[34];
    sp_digit* z2 = r + 68;
    sp_3072_add_34(a1, a, &a[34]);
    sp_3072_add_34(b1, b, &b[34]);
    sp_3072_mul_34(z2, &a[34], &b[34]);
    sp_3072_mul_34(z0, a, b);
    sp_3072_mul_34(z1, a1, b1);
    sp_3072_sub_68(z1, z1, z2);
    sp_3072_sub_68(z1, z1, z0);
    sp_3072_add_68(r + 34, r + 34, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_68(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[68];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 68;
    sp_3072_add_34(a1, a, &a[34]);
    sp_3072_sqr_34(z2, &a[34]);
    sp_3072_sqr_34(z0, a);
    sp_3072_sqr_34(z1, a1);
    sp_3072_sub_68(z1, z1, z2);
    sp_3072_sub_68(z1, z1, z0);
    sp_3072_add_68(r + 34, r + 34, z1);
}

/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i += 8) {
        r[i + 0] = a[i + 0] + b[i + 0];
        r[i + 1] = a[i + 1] + b[i + 1];
        r[i + 2] = a[i + 2] + b[i + 2];
        r[i + 3] = a[i + 3] + b[i + 3];
        r[i + 4] = a[i + 4] + b[i + 4];
        r[i + 5] = a[i + 5] + b[i + 5];
        r[i + 6] = a[i + 6] + b[i + 6];
        r[i + 7] = a[i + 7] + b[i + 7];
    }

    return 0;
}

/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i += 8) {
        r[i + 0] = a[i + 0] - b[i + 0];
        r[i + 1] = a[i + 1] - b[i + 1];
        r[i + 2] = a[i + 2] - b[i + 2];
        r[i + 3] = a[i + 3] - b[i + 3];
        r[i + 4] = a[i + 4] - b[i + 4];
        r[i + 5] = a[i + 5] - b[i + 5];
        r[i + 6] = a[i + 6] - b[i + 6];
        r[i + 7] = a[i + 7] - b[i + 7];
    }

    return 0;
}

/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_3072_mul_136(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    sp_digit* z0 = r;
    sp_digit z1[136];
    sp_digit* a1 = z1;
    sp_digit b1[68];
    sp_digit* z2 = r + 136;
    sp_3072_add_68(a1, a, &a[68]);
    sp_3072_add_68(b1, b, &b[68]);
    sp_3072_mul_68(z2, &a[68], &b[68]);
    sp_3072_mul_68(z0, a, b);
    sp_3072_mul_68(z1, a1, b1);
    sp_3072_sub_136(z1, z1, z2);
    sp_3072_sub_136(z1, z1, z0);
    sp_3072_add_136(r + 68, r + 68, z1);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_136(sp_digit* r, const sp_digit* a)
{
    sp_digit* z0 = r;
    sp_digit z1[136];
    sp_digit* a1 = z1;
    sp_digit* z2 = r + 136;
    sp_3072_add_68(a1, a, &a[68]);
    sp_3072_sqr_68(z2, &a[68]);
    sp_3072_sqr_68(z0, a);
    sp_3072_sqr_68(z1, a1);
    sp_3072_sub_136(z1, z1, z2);
    sp_3072_sub_136(z1, z1, z0);
    sp_3072_add_136(r + 68, r + 68, z1);
}

#endif /* WOLFSSL_SP_SMALL */
#ifdef WOLFSSL_SP_SMALL
/* Add b to a into r. (r = a + b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_add_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i++)
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
SP_NOINLINE static int sp_3072_sub_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i++)
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
SP_NOINLINE static void sp_3072_mul_136(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[135]) * b[135];
    r[271] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 269; k >= 0; k--) {
        for (i = 135; i >= 0; i--) {
            j = k - i;
            if (j >= 136)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_136(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[135]) * a[135];
    r[271] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 269; k >= 0; k--) {
        for (i = 135; i >= 0; i--) {
            j = k - i;
            if (j >= 136 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int64_t)a[i]) * a[i];

        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
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
SP_NOINLINE static int sp_3072_add_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 68; i++)
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
SP_NOINLINE static int sp_3072_sub_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 68; i++)
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
SP_NOINLINE static void sp_3072_mul_68(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[67]) * b[67];
    r[135] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 133; k >= 0; k--) {
        for (i = 67; i >= 0; i--) {
            j = k - i;
            if (j >= 68)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
}

/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_3072_sqr_68(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[67]) * a[67];
    r[135] = (sp_digit)(c >> 23);
    c = (c & 0x7fffff) << 23;
    for (k = 133; k >= 0; k--) {
        for (i = 67; i >= 0; i--) {
            j = k - i;
            if (j >= 68 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int64_t)a[i]) * a[i];

        r[k + 2] += c >> 46;
        r[k + 1] = (c >> 23) & 0x7fffff;
        c = (c & 0x7fffff) << 23;
    }
    r[0] = (sp_digit)(c >> 23);
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
    x &= 0x7fffff;

    /* rho = -1/m mod b */
    *rho = (1L << 23) - x;
}

#if !defined(SP_RSA_PRIVATE_EXP_D) && defined(WOLFSSL_HAVE_SP_RSA)
/* r = 2^n mod m where n is the number of bits to reduce by.
 * Given m must be 3072 bits, just need to subtract.
 *
 * r  A single precision number.
 * m  A signle precision number.
 */
static void sp_3072_mont_norm_68(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<67; i++)
        r[i] = 0x7fffff;
#else
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i + 0] = 0x7fffff;
        r[i + 1] = 0x7fffff;
        r[i + 2] = 0x7fffff;
        r[i + 3] = 0x7fffff;
        r[i + 4] = 0x7fffff;
        r[i + 5] = 0x7fffff;
        r[i + 6] = 0x7fffff;
        r[i + 7] = 0x7fffff;
    }
    r[64] = 0x7fffff;
    r[65] = 0x7fffff;
    r[66] = 0x7fffff;
#endif
    r[67] = 0x3ffffl;

    /* r = (2^n - 1) mod n */
    sp_3072_sub_68(r, r, m);

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
static sp_digit sp_3072_cmp_68(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=67; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    r |= (a[67] - b[67]) & (0 - !r);
    r |= (a[66] - b[66]) & (0 - !r);
    r |= (a[65] - b[65]) & (0 - !r);
    r |= (a[64] - b[64]) & (0 - !r);
    for (i = 56; i >= 0; i -= 8) {
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
static void sp_3072_cond_sub_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 68; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
    r[64] = a[64] - (b[64] & m);
    r[65] = a[65] - (b[65] & m);
    r[66] = a[66] - (b[66] & m);
    r[67] = a[67] - (b[67] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_add_68(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 68; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[68] += t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x7fffff;
    for (i = 0; i < 64; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[65]; r[65] += (t[0] >> 23) + (t[1] & 0x7fffff);
    t[2] = tb * a[66]; r[66] += (t[1] >> 23) + (t[2] & 0x7fffff);
    t[3] = tb * a[67]; r[67] += (t[2] >> 23) + (t[3] & 0x7fffff);
    r[68] +=  t[3] >> 23;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 23.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_3072_norm_68(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 67; i++) {
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
#else
    int i;
    for (i = 0; i < 64; i += 8) {
        a[i+1] += a[i+0] >> 23; a[i+0] &= 0x7fffff;
        a[i+2] += a[i+1] >> 23; a[i+1] &= 0x7fffff;
        a[i+3] += a[i+2] >> 23; a[i+2] &= 0x7fffff;
        a[i+4] += a[i+3] >> 23; a[i+3] &= 0x7fffff;
        a[i+5] += a[i+4] >> 23; a[i+4] &= 0x7fffff;
        a[i+6] += a[i+5] >> 23; a[i+5] &= 0x7fffff;
        a[i+7] += a[i+6] >> 23; a[i+6] &= 0x7fffff;
        a[i+8] += a[i+7] >> 23; a[i+7] &= 0x7fffff;
        a[i+9] += a[i+8] >> 23; a[i+8] &= 0x7fffff;
    }
    a[64+1] += a[64] >> 23;
    a[64] &= 0x7fffff;
    a[65+1] += a[65] >> 23;
    a[65] &= 0x7fffff;
    a[66+1] += a[66] >> 23;
    a[66] &= 0x7fffff;
#endif
}

/* Shift the result in the high 1536 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_3072_mont_shift_68(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    sp_digit n, s;

    s = a[68];
    n = a[67] >> 18;
    for (i = 0; i < 67; i++) {
        n += (s & 0x7fffff) << 5;
        r[i] = n & 0x7fffff;
        n >>= 23;
        s = a[69 + i] + (s >> 23);
    }
    n += s << 5;
    r[67] = n;
#else
    sp_digit n, s;
    int i;

    s = a[68]; n = a[67] >> 18;
    for (i = 0; i < 64; i += 8) {
        n += (s & 0x7fffff) << 5; r[i+0] = n & 0x7fffff;
        n >>= 23; s = a[i+69] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+1] = n & 0x7fffff;
        n >>= 23; s = a[i+70] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+2] = n & 0x7fffff;
        n >>= 23; s = a[i+71] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+3] = n & 0x7fffff;
        n >>= 23; s = a[i+72] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+4] = n & 0x7fffff;
        n >>= 23; s = a[i+73] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+5] = n & 0x7fffff;
        n >>= 23; s = a[i+74] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+6] = n & 0x7fffff;
        n >>= 23; s = a[i+75] + (s >> 23);
        n += (s & 0x7fffff) << 5; r[i+7] = n & 0x7fffff;
        n >>= 23; s = a[i+76] + (s >> 23);
    }
    n += (s & 0x7fffff) << 5; r[64] = n & 0x7fffff;
    n >>= 23; s = a[133] + (s >> 23);
    n += (s & 0x7fffff) << 5; r[65] = n & 0x7fffff;
    n >>= 23; s = a[134] + (s >> 23);
    n += (s & 0x7fffff) << 5; r[66] = n & 0x7fffff;
    n >>= 23; s = a[135] + (s >> 23);
    n += s << 5;              r[67] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[68], 0, sizeof(*r) * 68);
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_3072_mont_reduce_68(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    for (i=0; i<67; i++) {
        mu = (a[i] * mp) & 0x7fffff;
        sp_3072_mul_add_68(a+i, m, mu);
        a[i+1] += a[i] >> 23;
    }
    mu = (a[i] * mp) & 0x3ffffl;
    sp_3072_mul_add_68(a+i, m, mu);
    a[i+1] += a[i] >> 23;
    a[i] &= 0x7fffff;

    sp_3072_mont_shift_68(a, a);
    sp_3072_cond_sub_68(a, a, m, 0 - ((a[67] >> 18) > 0));
    sp_3072_norm_68(a);
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
static void sp_3072_mont_mul_68(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_68(r, a, b);
    sp_3072_mont_reduce_68(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_68(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_68(r, a);
    sp_3072_mont_reduce_68(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_d_68(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 68; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[68] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 64; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    t[1] = tb * a[65];
    r[65] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
    t[2] = tb * a[66];
    r[66] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
    t[3] = tb * a[67];
    r[67] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
    r[68] =  (sp_digit)(t[3] >> 23);
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
static void sp_3072_cond_add_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 68; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 64; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
    r[64] = a[64] + (b[64] & m);
    r[65] = a[65] + (b[65] & m);
    r[66] = a[66] + (b[66] & m);
    r[67] = a[67] + (b[67] & m);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 68; i++)
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
SP_NOINLINE static int sp_3072_add_68(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 68; i++)
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
static int sp_3072_div_68(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int64_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[136], t2d[68 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 68 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 68;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[67];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 68);
        for (i=67; i>=0; i--) {
            t1[68 + i] += t1[68 + i - 1] >> 23;
            t1[68 + i - 1] &= 0x7fffff;
            d1 = t1[68 + i];
            d1 <<= 23;
            d1 += t1[68 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_3072_mul_d_68(t2, d, r1);
            sp_3072_sub_68(&t1[i], &t1[i], t2);
            t1[68 + i] -= t2[68];
            t1[68 + i] += t1[68 + i - 1] >> 23;
            t1[68 + i - 1] &= 0x7fffff;
            r1 = (((-t1[68 + i]) << 23) - t1[68 + i - 1]) / div;
            r1++;
            sp_3072_mul_d_68(t2, d, r1);
            sp_3072_add_68(&t1[i], &t1[i], t2);
            t1[68 + i] += t1[68 + i - 1] >> 23;
            t1[68 + i - 1] &= 0x7fffff;
        }
        t1[68 - 1] += t1[68 - 2] >> 23;
        t1[68 - 2] &= 0x7fffff;
        d1 = t1[68 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_3072_mul_d_68(t2, d, r1);
        sp_3072_sub_68(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 68);
        for (i=0; i<66; i++) {
            r[i+1] += r[i] >> 23;
            r[i] &= 0x7fffff;
        }
        sp_3072_cond_add_68(r, r, d, 0 - (r[67] < 0));
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
static int sp_3072_mod_68(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_68(a, m, NULL, r);
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
static int sp_3072_mod_exp_68(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
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

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 68 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 68 * 2);

        norm = t[0] = td;
        t[1] = &td[68 * 2];
        t[2] = &td[2 * 68 * 2];

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_68(norm, m);

        if (reduceA)
            err = sp_3072_mod_68(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 68);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_68(t[1], t[1], norm);
        err = sp_3072_mod_68(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_3072_mont_mul_68(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 68 * 2);
            sp_3072_mont_sqr_68(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 68 * 2);
        }

        sp_3072_mont_reduce_68(t[0], m, mp);
        n = sp_3072_cmp_68(t[0], m);
        sp_3072_cond_sub_68(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 68 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][136];
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
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 68 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[68 * 2];
        t[2] = &td[2 * 68 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_68(norm, m);

        if (reduceA) {
            err = sp_3072_mod_68(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_68(t[1], t[1], norm);
                err = sp_3072_mod_68(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_68(t[1], a, norm);
            err = sp_3072_mod_68(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_3072_mont_mul_68(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_3072_mont_sqr_68(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_3072_mont_reduce_68(t[0], m, mp);
        n = sp_3072_cmp_68(t[0], m);
        sp_3072_cond_sub_68(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][136];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[136];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 136, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 136;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_68(norm, m);

        if (reduceA) {
            err = sp_3072_mod_68(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_68(t[1], t[1], norm);
                err = sp_3072_mod_68(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_68(t[1], a, norm);
            err = sp_3072_mod_68(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_68(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_68(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_68(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_68(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_68(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_68(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_68(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_68(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_68(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_68(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_68(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_68(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_68(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_68(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_68(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_68(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_68(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_68(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_68(t[20], t[10], m, mp);
        sp_3072_mont_mul_68(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_68(t[22], t[11], m, mp);
        sp_3072_mont_mul_68(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_68(t[24], t[12], m, mp);
        sp_3072_mont_mul_68(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_68(t[26], t[13], m, mp);
        sp_3072_mont_mul_68(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_68(t[28], t[14], m, mp);
        sp_3072_mont_mul_68(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_68(t[30], t[15], m, mp);
        sp_3072_mont_mul_68(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 22) / 23) - 1;
        c = bits % 23;
        if (c == 0)
            c = 23;
        if (i < 68)
            n = e[i--] << (32 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (9 - c);
            c += 23;
        }
        y = n >> 27;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (9 - c);
                c += 23;
            }
            y = (n >> 27) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_3072_mont_sqr_68(rt, rt, m, mp);
            sp_3072_mont_sqr_68(rt, rt, m, mp);
            sp_3072_mont_sqr_68(rt, rt, m, mp);
            sp_3072_mont_sqr_68(rt, rt, m, mp);
            sp_3072_mont_sqr_68(rt, rt, m, mp);

            sp_3072_mont_mul_68(rt, rt, t[y], m, mp);
        }

        sp_3072_mont_reduce_68(rt, m, mp);
        n = sp_3072_cmp_68(rt, m);
        sp_3072_cond_sub_68(rt, rt, m, (n < 0) - 1);
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
static void sp_3072_mont_norm_136(sp_digit* r, sp_digit* m)
{
    /* Set r = 2^n - 1. */
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<135; i++)
        r[i] = 0x7fffff;
#else
    int i;

    for (i = 0; i < 136; i += 8) {
        r[i + 0] = 0x7fffff;
        r[i + 1] = 0x7fffff;
        r[i + 2] = 0x7fffff;
        r[i + 3] = 0x7fffff;
        r[i + 4] = 0x7fffff;
        r[i + 5] = 0x7fffff;
        r[i + 6] = 0x7fffff;
        r[i + 7] = 0x7fffff;
    }
#endif
    r[135] = 0x1fffl;

    /* r = (2^n - 1) mod n */
    sp_3072_sub_136(r, r, m);

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
static sp_digit sp_3072_cmp_136(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=135; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    int i;

    for (i = 128; i >= 0; i -= 8) {
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
static void sp_3072_cond_sub_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 136; i++)
        r[i] = a[i] - (b[i] & m);
#else
    int i;

    for (i = 0; i < 136; i += 8) {
        r[i + 0] = a[i + 0] - (b[i + 0] & m);
        r[i + 1] = a[i + 1] - (b[i + 1] & m);
        r[i + 2] = a[i + 2] - (b[i + 2] & m);
        r[i + 3] = a[i + 3] - (b[i + 3] & m);
        r[i + 4] = a[i + 4] - (b[i + 4] & m);
        r[i + 5] = a[i + 5] - (b[i + 5] & m);
        r[i + 6] = a[i + 6] - (b[i + 6] & m);
        r[i + 7] = a[i + 7] - (b[i + 7] & m);
    }
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_add_136(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 136; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[136] += t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] += t[0] & 0x7fffff;
    for (i = 0; i < 136; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] += (t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] += (t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] += (t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] += (t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] += (t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] += (t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] += (t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] += (t[7] >> 23) + (t[0] & 0x7fffff);
    }
    r[136] +=  t[7] >> 23;
#endif /* WOLFSSL_SP_SMALL */
}

/* Normalize the values in each word to 23.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_3072_norm_136(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 135; i++) {
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
#else
    int i;
    for (i = 0; i < 128; i += 8) {
        a[i+1] += a[i+0] >> 23; a[i+0] &= 0x7fffff;
        a[i+2] += a[i+1] >> 23; a[i+1] &= 0x7fffff;
        a[i+3] += a[i+2] >> 23; a[i+2] &= 0x7fffff;
        a[i+4] += a[i+3] >> 23; a[i+3] &= 0x7fffff;
        a[i+5] += a[i+4] >> 23; a[i+4] &= 0x7fffff;
        a[i+6] += a[i+5] >> 23; a[i+5] &= 0x7fffff;
        a[i+7] += a[i+6] >> 23; a[i+6] &= 0x7fffff;
        a[i+8] += a[i+7] >> 23; a[i+7] &= 0x7fffff;
        a[i+9] += a[i+8] >> 23; a[i+8] &= 0x7fffff;
    }
    a[128+1] += a[128] >> 23;
    a[128] &= 0x7fffff;
    a[129+1] += a[129] >> 23;
    a[129] &= 0x7fffff;
    a[130+1] += a[130] >> 23;
    a[130] &= 0x7fffff;
    a[131+1] += a[131] >> 23;
    a[131] &= 0x7fffff;
    a[132+1] += a[132] >> 23;
    a[132] &= 0x7fffff;
    a[133+1] += a[133] >> 23;
    a[133] &= 0x7fffff;
    a[134+1] += a[134] >> 23;
    a[134] &= 0x7fffff;
#endif
}

/* Shift the result in the high 3072 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_3072_mont_shift_136(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    int64_t n = a[135] >> 13;
    n += ((int64_t)a[136]) << 10;

    for (i = 0; i < 135; i++) {
        r[i] = n & 0x7fffff;
        n >>= 23;
        n += ((int64_t)a[137 + i]) << 10;
    }
    r[135] = (sp_digit)n;
#else
    int i;
    int64_t n = a[135] >> 13;
    n += ((int64_t)a[136]) << 10;
    for (i = 0; i < 136; i += 8) {
        r[i + 0] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 137]) << 10;
        r[i + 1] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 138]) << 10;
        r[i + 2] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 139]) << 10;
        r[i + 3] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 140]) << 10;
        r[i + 4] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 141]) << 10;
        r[i + 5] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 142]) << 10;
        r[i + 6] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 143]) << 10;
        r[i + 7] = n & 0x7fffff;
        n >>= 23; n += ((int64_t)a[i + 144]) << 10;
    }
    r[135] = (sp_digit)n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[136], 0, sizeof(*r) * 136);
}

/* Reduce the number back to 3072 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_3072_mont_reduce_136(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<135; i++) {
            mu = (a[i] * mp) & 0x7fffff;
            sp_3072_mul_add_136(a+i, m, mu);
            a[i+1] += a[i] >> 23;
        }
        mu = (a[i] * mp) & 0x1fffl;
        sp_3072_mul_add_136(a+i, m, mu);
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }
    else {
        for (i=0; i<135; i++) {
            mu = a[i] & 0x7fffff;
            sp_3072_mul_add_136(a+i, m, mu);
            a[i+1] += a[i] >> 23;
        }
        mu = a[i] & 0x1fffl;
        sp_3072_mul_add_136(a+i, m, mu);
        a[i+1] += a[i] >> 23;
        a[i] &= 0x7fffff;
    }

    sp_3072_mont_shift_136(a, a);
    sp_3072_cond_sub_136(a, a, m, 0 - ((a[135] >> 13) > 0));
    sp_3072_norm_136(a);
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
static void sp_3072_mont_mul_136(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_3072_mul_136(r, a, b);
    sp_3072_mont_reduce_136(r, m, mp);
}

/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_3072_mont_sqr_136(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_3072_sqr_136(r, a);
    sp_3072_mont_reduce_136(r, m, mp);
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_d_136(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 136; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[136] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 136; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    r[136] =  (sp_digit)(t[7] >> 23);
#endif /* WOLFSSL_SP_SMALL */
}

/* Multiply a by scalar b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_3072_mul_d_272(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 272; i++) {
        t += tb * a[i];
        r[i] = t & 0x7fffff;
        t >>= 23;
    }
    r[272] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[8];
    int i;

    t[0] = tb * a[0]; r[0] = t[0] & 0x7fffff;
    for (i = 0; i < 272; i += 8) {
        t[1] = tb * a[i+1];
        r[i+1] = (sp_digit)(t[0] >> 23) + (t[1] & 0x7fffff);
        t[2] = tb * a[i+2];
        r[i+2] = (sp_digit)(t[1] >> 23) + (t[2] & 0x7fffff);
        t[3] = tb * a[i+3];
        r[i+3] = (sp_digit)(t[2] >> 23) + (t[3] & 0x7fffff);
        t[4] = tb * a[i+4];
        r[i+4] = (sp_digit)(t[3] >> 23) + (t[4] & 0x7fffff);
        t[5] = tb * a[i+5];
        r[i+5] = (sp_digit)(t[4] >> 23) + (t[5] & 0x7fffff);
        t[6] = tb * a[i+6];
        r[i+6] = (sp_digit)(t[5] >> 23) + (t[6] & 0x7fffff);
        t[7] = tb * a[i+7];
        r[i+7] = (sp_digit)(t[6] >> 23) + (t[7] & 0x7fffff);
        t[0] = tb * a[i+8];
        r[i+8] = (sp_digit)(t[7] >> 23) + (t[0] & 0x7fffff);
    }
    r[272] =  (sp_digit)(t[7] >> 23);
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
static void sp_3072_cond_add_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 136; i++)
        r[i] = a[i] + (b[i] & m);
#else
    int i;

    for (i = 0; i < 136; i += 8) {
        r[i + 0] = a[i + 0] + (b[i + 0] & m);
        r[i + 1] = a[i + 1] + (b[i + 1] & m);
        r[i + 2] = a[i + 2] + (b[i + 2] & m);
        r[i + 3] = a[i + 3] + (b[i + 3] & m);
        r[i + 4] = a[i + 4] + (b[i + 4] & m);
        r[i + 5] = a[i + 5] + (b[i + 5] & m);
        r[i + 6] = a[i + 6] + (b[i + 6] & m);
        r[i + 7] = a[i + 7] + (b[i + 7] & m);
    }
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef WOLFSSL_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_3072_sub_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i++)
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
SP_NOINLINE static int sp_3072_add_136(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 136; i++)
        r[i] = a[i] + b[i];

    return 0;
}
#endif
SP_NOINLINE static void sp_3072_rshift_136(sp_digit* r, sp_digit* a, byte n)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<135; i++)
        r[i] = ((a[i] >> n) | (a[i + 1] << (23 - n))) & 0x7fffff;
#else
    r[0] = ((a[0] >> n) | (a[1] << (23 - n))) & 0x7fffff;
    r[1] = ((a[1] >> n) | (a[2] << (23 - n))) & 0x7fffff;
    r[2] = ((a[2] >> n) | (a[3] << (23 - n))) & 0x7fffff;
    r[3] = ((a[3] >> n) | (a[4] << (23 - n))) & 0x7fffff;
    r[4] = ((a[4] >> n) | (a[5] << (23 - n))) & 0x7fffff;
    r[5] = ((a[5] >> n) | (a[6] << (23 - n))) & 0x7fffff;
    r[6] = ((a[6] >> n) | (a[7] << (23 - n))) & 0x7fffff;
    r[7] = ((a[7] >> n) | (a[8] << (23 - n))) & 0x7fffff;
    r[8] = ((a[8] >> n) | (a[9] << (23 - n))) & 0x7fffff;
    r[9] = ((a[9] >> n) | (a[10] << (23 - n))) & 0x7fffff;
    r[10] = ((a[10] >> n) | (a[11] << (23 - n))) & 0x7fffff;
    r[11] = ((a[11] >> n) | (a[12] << (23 - n))) & 0x7fffff;
    r[12] = ((a[12] >> n) | (a[13] << (23 - n))) & 0x7fffff;
    r[13] = ((a[13] >> n) | (a[14] << (23 - n))) & 0x7fffff;
    r[14] = ((a[14] >> n) | (a[15] << (23 - n))) & 0x7fffff;
    r[15] = ((a[15] >> n) | (a[16] << (23 - n))) & 0x7fffff;
    r[16] = ((a[16] >> n) | (a[17] << (23 - n))) & 0x7fffff;
    r[17] = ((a[17] >> n) | (a[18] << (23 - n))) & 0x7fffff;
    r[18] = ((a[18] >> n) | (a[19] << (23 - n))) & 0x7fffff;
    r[19] = ((a[19] >> n) | (a[20] << (23 - n))) & 0x7fffff;
    r[20] = ((a[20] >> n) | (a[21] << (23 - n))) & 0x7fffff;
    r[21] = ((a[21] >> n) | (a[22] << (23 - n))) & 0x7fffff;
    r[22] = ((a[22] >> n) | (a[23] << (23 - n))) & 0x7fffff;
    r[23] = ((a[23] >> n) | (a[24] << (23 - n))) & 0x7fffff;
    r[24] = ((a[24] >> n) | (a[25] << (23 - n))) & 0x7fffff;
    r[25] = ((a[25] >> n) | (a[26] << (23 - n))) & 0x7fffff;
    r[26] = ((a[26] >> n) | (a[27] << (23 - n))) & 0x7fffff;
    r[27] = ((a[27] >> n) | (a[28] << (23 - n))) & 0x7fffff;
    r[28] = ((a[28] >> n) | (a[29] << (23 - n))) & 0x7fffff;
    r[29] = ((a[29] >> n) | (a[30] << (23 - n))) & 0x7fffff;
    r[30] = ((a[30] >> n) | (a[31] << (23 - n))) & 0x7fffff;
    r[31] = ((a[31] >> n) | (a[32] << (23 - n))) & 0x7fffff;
    r[32] = ((a[32] >> n) | (a[33] << (23 - n))) & 0x7fffff;
    r[33] = ((a[33] >> n) | (a[34] << (23 - n))) & 0x7fffff;
    r[34] = ((a[34] >> n) | (a[35] << (23 - n))) & 0x7fffff;
    r[35] = ((a[35] >> n) | (a[36] << (23 - n))) & 0x7fffff;
    r[36] = ((a[36] >> n) | (a[37] << (23 - n))) & 0x7fffff;
    r[37] = ((a[37] >> n) | (a[38] << (23 - n))) & 0x7fffff;
    r[38] = ((a[38] >> n) | (a[39] << (23 - n))) & 0x7fffff;
    r[39] = ((a[39] >> n) | (a[40] << (23 - n))) & 0x7fffff;
    r[40] = ((a[40] >> n) | (a[41] << (23 - n))) & 0x7fffff;
    r[41] = ((a[41] >> n) | (a[42] << (23 - n))) & 0x7fffff;
    r[42] = ((a[42] >> n) | (a[43] << (23 - n))) & 0x7fffff;
    r[43] = ((a[43] >> n) | (a[44] << (23 - n))) & 0x7fffff;
    r[44] = ((a[44] >> n) | (a[45] << (23 - n))) & 0x7fffff;
    r[45] = ((a[45] >> n) | (a[46] << (23 - n))) & 0x7fffff;
    r[46] = ((a[46] >> n) | (a[47] << (23 - n))) & 0x7fffff;
    r[47] = ((a[47] >> n) | (a[48] << (23 - n))) & 0x7fffff;
    r[48] = ((a[48] >> n) | (a[49] << (23 - n))) & 0x7fffff;
    r[49] = ((a[49] >> n) | (a[50] << (23 - n))) & 0x7fffff;
    r[50] = ((a[50] >> n) | (a[51] << (23 - n))) & 0x7fffff;
    r[51] = ((a[51] >> n) | (a[52] << (23 - n))) & 0x7fffff;
    r[52] = ((a[52] >> n) | (a[53] << (23 - n))) & 0x7fffff;
    r[53] = ((a[53] >> n) | (a[54] << (23 - n))) & 0x7fffff;
    r[54] = ((a[54] >> n) | (a[55] << (23 - n))) & 0x7fffff;
    r[55] = ((a[55] >> n) | (a[56] << (23 - n))) & 0x7fffff;
    r[56] = ((a[56] >> n) | (a[57] << (23 - n))) & 0x7fffff;
    r[57] = ((a[57] >> n) | (a[58] << (23 - n))) & 0x7fffff;
    r[58] = ((a[58] >> n) | (a[59] << (23 - n))) & 0x7fffff;
    r[59] = ((a[59] >> n) | (a[60] << (23 - n))) & 0x7fffff;
    r[60] = ((a[60] >> n) | (a[61] << (23 - n))) & 0x7fffff;
    r[61] = ((a[61] >> n) | (a[62] << (23 - n))) & 0x7fffff;
    r[62] = ((a[62] >> n) | (a[63] << (23 - n))) & 0x7fffff;
    r[63] = ((a[63] >> n) | (a[64] << (23 - n))) & 0x7fffff;
    r[64] = ((a[64] >> n) | (a[65] << (23 - n))) & 0x7fffff;
    r[65] = ((a[65] >> n) | (a[66] << (23 - n))) & 0x7fffff;
    r[66] = ((a[66] >> n) | (a[67] << (23 - n))) & 0x7fffff;
    r[67] = ((a[67] >> n) | (a[68] << (23 - n))) & 0x7fffff;
    r[68] = ((a[68] >> n) | (a[69] << (23 - n))) & 0x7fffff;
    r[69] = ((a[69] >> n) | (a[70] << (23 - n))) & 0x7fffff;
    r[70] = ((a[70] >> n) | (a[71] << (23 - n))) & 0x7fffff;
    r[71] = ((a[71] >> n) | (a[72] << (23 - n))) & 0x7fffff;
    r[72] = ((a[72] >> n) | (a[73] << (23 - n))) & 0x7fffff;
    r[73] = ((a[73] >> n) | (a[74] << (23 - n))) & 0x7fffff;
    r[74] = ((a[74] >> n) | (a[75] << (23 - n))) & 0x7fffff;
    r[75] = ((a[75] >> n) | (a[76] << (23 - n))) & 0x7fffff;
    r[76] = ((a[76] >> n) | (a[77] << (23 - n))) & 0x7fffff;
    r[77] = ((a[77] >> n) | (a[78] << (23 - n))) & 0x7fffff;
    r[78] = ((a[78] >> n) | (a[79] << (23 - n))) & 0x7fffff;
    r[79] = ((a[79] >> n) | (a[80] << (23 - n))) & 0x7fffff;
    r[80] = ((a[80] >> n) | (a[81] << (23 - n))) & 0x7fffff;
    r[81] = ((a[81] >> n) | (a[82] << (23 - n))) & 0x7fffff;
    r[82] = ((a[82] >> n) | (a[83] << (23 - n))) & 0x7fffff;
    r[83] = ((a[83] >> n) | (a[84] << (23 - n))) & 0x7fffff;
    r[84] = ((a[84] >> n) | (a[85] << (23 - n))) & 0x7fffff;
    r[85] = ((a[85] >> n) | (a[86] << (23 - n))) & 0x7fffff;
    r[86] = ((a[86] >> n) | (a[87] << (23 - n))) & 0x7fffff;
    r[87] = ((a[87] >> n) | (a[88] << (23 - n))) & 0x7fffff;
    r[88] = ((a[88] >> n) | (a[89] << (23 - n))) & 0x7fffff;
    r[89] = ((a[89] >> n) | (a[90] << (23 - n))) & 0x7fffff;
    r[90] = ((a[90] >> n) | (a[91] << (23 - n))) & 0x7fffff;
    r[91] = ((a[91] >> n) | (a[92] << (23 - n))) & 0x7fffff;
    r[92] = ((a[92] >> n) | (a[93] << (23 - n))) & 0x7fffff;
    r[93] = ((a[93] >> n) | (a[94] << (23 - n))) & 0x7fffff;
    r[94] = ((a[94] >> n) | (a[95] << (23 - n))) & 0x7fffff;
    r[95] = ((a[95] >> n) | (a[96] << (23 - n))) & 0x7fffff;
    r[96] = ((a[96] >> n) | (a[97] << (23 - n))) & 0x7fffff;
    r[97] = ((a[97] >> n) | (a[98] << (23 - n))) & 0x7fffff;
    r[98] = ((a[98] >> n) | (a[99] << (23 - n))) & 0x7fffff;
    r[99] = ((a[99] >> n) | (a[100] << (23 - n))) & 0x7fffff;
    r[100] = ((a[100] >> n) | (a[101] << (23 - n))) & 0x7fffff;
    r[101] = ((a[101] >> n) | (a[102] << (23 - n))) & 0x7fffff;
    r[102] = ((a[102] >> n) | (a[103] << (23 - n))) & 0x7fffff;
    r[103] = ((a[103] >> n) | (a[104] << (23 - n))) & 0x7fffff;
    r[104] = ((a[104] >> n) | (a[105] << (23 - n))) & 0x7fffff;
    r[105] = ((a[105] >> n) | (a[106] << (23 - n))) & 0x7fffff;
    r[106] = ((a[106] >> n) | (a[107] << (23 - n))) & 0x7fffff;
    r[107] = ((a[107] >> n) | (a[108] << (23 - n))) & 0x7fffff;
    r[108] = ((a[108] >> n) | (a[109] << (23 - n))) & 0x7fffff;
    r[109] = ((a[109] >> n) | (a[110] << (23 - n))) & 0x7fffff;
    r[110] = ((a[110] >> n) | (a[111] << (23 - n))) & 0x7fffff;
    r[111] = ((a[111] >> n) | (a[112] << (23 - n))) & 0x7fffff;
    r[112] = ((a[112] >> n) | (a[113] << (23 - n))) & 0x7fffff;
    r[113] = ((a[113] >> n) | (a[114] << (23 - n))) & 0x7fffff;
    r[114] = ((a[114] >> n) | (a[115] << (23 - n))) & 0x7fffff;
    r[115] = ((a[115] >> n) | (a[116] << (23 - n))) & 0x7fffff;
    r[116] = ((a[116] >> n) | (a[117] << (23 - n))) & 0x7fffff;
    r[117] = ((a[117] >> n) | (a[118] << (23 - n))) & 0x7fffff;
    r[118] = ((a[118] >> n) | (a[119] << (23 - n))) & 0x7fffff;
    r[119] = ((a[119] >> n) | (a[120] << (23 - n))) & 0x7fffff;
    r[120] = ((a[120] >> n) | (a[121] << (23 - n))) & 0x7fffff;
    r[121] = ((a[121] >> n) | (a[122] << (23 - n))) & 0x7fffff;
    r[122] = ((a[122] >> n) | (a[123] << (23 - n))) & 0x7fffff;
    r[123] = ((a[123] >> n) | (a[124] << (23 - n))) & 0x7fffff;
    r[124] = ((a[124] >> n) | (a[125] << (23 - n))) & 0x7fffff;
    r[125] = ((a[125] >> n) | (a[126] << (23 - n))) & 0x7fffff;
    r[126] = ((a[126] >> n) | (a[127] << (23 - n))) & 0x7fffff;
    r[127] = ((a[127] >> n) | (a[128] << (23 - n))) & 0x7fffff;
    r[128] = ((a[128] >> n) | (a[129] << (23 - n))) & 0x7fffff;
    r[129] = ((a[129] >> n) | (a[130] << (23 - n))) & 0x7fffff;
    r[130] = ((a[130] >> n) | (a[131] << (23 - n))) & 0x7fffff;
    r[131] = ((a[131] >> n) | (a[132] << (23 - n))) & 0x7fffff;
    r[132] = ((a[132] >> n) | (a[133] << (23 - n))) & 0x7fffff;
    r[133] = ((a[133] >> n) | (a[134] << (23 - n))) & 0x7fffff;
    r[134] = ((a[134] >> n) | (a[135] << (23 - n))) & 0x7fffff;
#endif
    r[135] = a[135] >> n;
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
static int sp_3072_div_136(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int64_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[272 + 1], t2d[136 + 1], sdd[136 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    sp_digit* sd;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (4 * 136 + 3), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 272 + 1;
        sd = t2 + 136 + 1;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
    sd = sdd;
#endif

    (void)m;

    if (err == MP_OKAY) {
        sp_3072_mul_d_136(sd, d, 1 << 10);
        sp_3072_mul_d_272(t1, a, 1 << 10);
        div = sd[135];
        for (i=136; i>=0; i--) {
            t1[136 + i] += t1[136 + i - 1] >> 23;
            t1[136 + i - 1] &= 0x7fffff;
            d1 = t1[136 + i];
            d1 <<= 23;
            d1 += t1[136 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_3072_mul_d_136(t2, sd, r1);
            sp_3072_sub_136(&t1[i], &t1[i], t2);
            t1[136 + i] -= t2[136];
            t1[136 + i] += t1[136 + i - 1] >> 23;
            t1[136 + i - 1] &= 0x7fffff;
            r1 = (((-t1[136 + i]) << 23) - t1[136 + i - 1]) / div;
            r1 -= t1[136 + i];
            sp_3072_mul_d_136(t2, sd, r1);
            sp_3072_add_136(&t1[i], &t1[i], t2);
            t1[136 + i] += t1[136 + i - 1] >> 23;
            t1[136 + i - 1] &= 0x7fffff;
        }
        t1[136 - 1] += t1[136 - 2] >> 23;
        t1[136 - 2] &= 0x7fffff;
        d1 = t1[136 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_3072_mul_d_136(t2, sd, r1);
        sp_3072_sub_136(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 136);
        for (i=0; i<134; i++) {
            r[i+1] += r[i] >> 23;
            r[i] &= 0x7fffff;
        }
        sp_3072_cond_add_136(r, r, sd, 0 - (r[135] < 0));
    }

    sp_3072_rshift_136(r, r, 10);

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
static int sp_3072_mod_136(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_3072_div_136(a, m, NULL, r);
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
static int sp_3072_mod_exp_136(sp_digit* r, sp_digit* a, sp_digit* e, int bits,
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

    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 136 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        XMEMSET(td, 0, sizeof(*td) * 3 * 136 * 2);

        norm = t[0] = td;
        t[1] = &td[136 * 2];
        t[2] = &td[2 * 136 * 2];

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_136(norm, m);

        if (reduceA)
            err = sp_3072_mod_136(t[1], a, m);
        else
            XMEMCPY(t[1], a, sizeof(sp_digit) * 136);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_136(t[1], t[1], norm);
        err = sp_3072_mod_136(t[1], t[1], m);
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_3072_mont_mul_136(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(*t[2]) * 136 * 2);
            sp_3072_mont_sqr_136(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(*t[2]) * 136 * 2);
        }

        sp_3072_mont_reduce_136(t[0], m, mp);
        n = sp_3072_cmp_136(t[0], m);
        sp_3072_cond_sub_136(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(*r) * 136 * 2);

    }

    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#elif defined(WOLFSSL_SP_CACHE_RESISTANT)
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[3][272];
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
    td = (sp_digit*)XMALLOC(sizeof(*td) * 3 * 136 * 2, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        t[0] = td;
        t[1] = &td[136 * 2];
        t[2] = &td[2 * 136 * 2];
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_136(norm, m);

        if (reduceA) {
            err = sp_3072_mod_136(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_136(t[1], t[1], norm);
                err = sp_3072_mod_136(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_136(t[1], a, norm);
            err = sp_3072_mod_136(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        i = bits / 23;
        c = bits % 23;
        n = e[i--] << (23 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = e[i--];
                c = 23;
            }

            y = (n >> 22) & 1;
            n <<= 1;

            sp_3072_mont_mul_136(t[y^1], t[0], t[1], m, mp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                 ((size_t)t[1] & addr_mask[y])), sizeof(t[2]));
            sp_3072_mont_sqr_136(t[2], t[2], m, mp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                           ((size_t)t[1] & addr_mask[y])), t[2], sizeof(t[2]));
        }

        sp_3072_mont_reduce_136(t[0], m, mp);
        n = sp_3072_cmp_136(t[0], m);
        sp_3072_cond_sub_136(t[0], t[0], m, (n < 0) - 1);
        XMEMCPY(r, t[0], sizeof(t[0]));
    }

#ifdef WOLFSSL_SMALL_STACK
    if (td != NULL)
        XFREE(td, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit t[32][272];
#else
    sp_digit* t[32];
    sp_digit* td;
#endif
    sp_digit* norm;
    sp_digit rt[272];
    sp_digit mp = 1;
    sp_digit n;
    int i;
    int c, y;
    int err = MP_OKAY;

#ifdef WOLFSSL_SMALL_STACK
    td = (sp_digit*)XMALLOC(sizeof(sp_digit) * 32 * 272, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);
    if (td == NULL)
        err = MEMORY_E;

    if (err == MP_OKAY) {
        for (i=0; i<32; i++)
            t[i] = td + i * 272;
        norm = t[0];
    }
#else
    norm = t[0];
#endif

    if (err == MP_OKAY) {
        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_136(norm, m);

        if (reduceA) {
            err = sp_3072_mod_136(t[1], a, m);
            if (err == MP_OKAY) {
                sp_3072_mul_136(t[1], t[1], norm);
                err = sp_3072_mod_136(t[1], t[1], m);
            }
        }
        else {
            sp_3072_mul_136(t[1], a, norm);
            err = sp_3072_mod_136(t[1], t[1], m);
        }
    }

    if (err == MP_OKAY) {
        sp_3072_mont_sqr_136(t[ 2], t[ 1], m, mp);
        sp_3072_mont_mul_136(t[ 3], t[ 2], t[ 1], m, mp);
        sp_3072_mont_sqr_136(t[ 4], t[ 2], m, mp);
        sp_3072_mont_mul_136(t[ 5], t[ 3], t[ 2], m, mp);
        sp_3072_mont_sqr_136(t[ 6], t[ 3], m, mp);
        sp_3072_mont_mul_136(t[ 7], t[ 4], t[ 3], m, mp);
        sp_3072_mont_sqr_136(t[ 8], t[ 4], m, mp);
        sp_3072_mont_mul_136(t[ 9], t[ 5], t[ 4], m, mp);
        sp_3072_mont_sqr_136(t[10], t[ 5], m, mp);
        sp_3072_mont_mul_136(t[11], t[ 6], t[ 5], m, mp);
        sp_3072_mont_sqr_136(t[12], t[ 6], m, mp);
        sp_3072_mont_mul_136(t[13], t[ 7], t[ 6], m, mp);
        sp_3072_mont_sqr_136(t[14], t[ 7], m, mp);
        sp_3072_mont_mul_136(t[15], t[ 8], t[ 7], m, mp);
        sp_3072_mont_sqr_136(t[16], t[ 8], m, mp);
        sp_3072_mont_mul_136(t[17], t[ 9], t[ 8], m, mp);
        sp_3072_mont_sqr_136(t[18], t[ 9], m, mp);
        sp_3072_mont_mul_136(t[19], t[10], t[ 9], m, mp);
        sp_3072_mont_sqr_136(t[20], t[10], m, mp);
        sp_3072_mont_mul_136(t[21], t[11], t[10], m, mp);
        sp_3072_mont_sqr_136(t[22], t[11], m, mp);
        sp_3072_mont_mul_136(t[23], t[12], t[11], m, mp);
        sp_3072_mont_sqr_136(t[24], t[12], m, mp);
        sp_3072_mont_mul_136(t[25], t[13], t[12], m, mp);
        sp_3072_mont_sqr_136(t[26], t[13], m, mp);
        sp_3072_mont_mul_136(t[27], t[14], t[13], m, mp);
        sp_3072_mont_sqr_136(t[28], t[14], m, mp);
        sp_3072_mont_mul_136(t[29], t[15], t[14], m, mp);
        sp_3072_mont_sqr_136(t[30], t[15], m, mp);
        sp_3072_mont_mul_136(t[31], t[16], t[15], m, mp);

        bits = ((bits + 4) / 5) * 5;
        i = ((bits + 22) / 23) - 1;
        c = bits % 23;
        if (c == 0)
            c = 23;
        if (i < 136)
            n = e[i--] << (32 - c);
        else {
            n = 0;
            i--;
        }
        if (c < 5) {
            n |= e[i--] << (9 - c);
            c += 23;
        }
        y = n >> 27;
        n <<= 5;
        c -= 5;
        XMEMCPY(rt, t[y], sizeof(rt));
        for (; i>=0 || c>=5; ) {
            if (c < 5) {
                n |= e[i--] << (9 - c);
                c += 23;
            }
            y = (n >> 27) & 0x1f;
            n <<= 5;
            c -= 5;

            sp_3072_mont_sqr_136(rt, rt, m, mp);
            sp_3072_mont_sqr_136(rt, rt, m, mp);
            sp_3072_mont_sqr_136(rt, rt, m, mp);
            sp_3072_mont_sqr_136(rt, rt, m, mp);
            sp_3072_mont_sqr_136(rt, rt, m, mp);

            sp_3072_mont_mul_136(rt, rt, t[y], m, mp);
        }

        sp_3072_mont_reduce_136(rt, m, mp);
        n = sp_3072_cmp_136(rt, m);
        sp_3072_cond_sub_136(rt, rt, m, (n < 0) - 1);
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
static void sp_3072_mask_68(sp_digit* r, sp_digit* a, sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<68; i++)
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
    r[64] = a[64] & m;
    r[65] = a[65] & m;
    r[66] = a[66] & m;
    r[67] = a[67] & m;
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
    if (err == MP_OKAY && (mp_count_bits(em) > 23 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 136 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 136 * 2;
        m = r + 136 * 2;
        norm = r;

        sp_3072_from_bin(a, 136, in, inLen);
#if DIGIT_BIT >= 23
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
        sp_3072_from_mp(m, 136, mm);

        sp_3072_mont_setup(m, &mp);
        sp_3072_mont_norm_136(norm, m);
    }
    if (err == MP_OKAY) {
        sp_3072_mul_136(a, a, norm);
        err = sp_3072_mod_136(a, a, m);
    }
    if (err == MP_OKAY) {
        for (i=22; i>=0; i--)
            if (e[0] >> i)
                break;

        XMEMCPY(r, a, sizeof(sp_digit) * 136 * 2);
        for (i--; i>=0; i--) {
            sp_3072_mont_sqr_136(r, r, m, mp);

            if (((e[0] >> i) & 1) == 1)
                sp_3072_mont_mul_136(r, r, a, m, mp);
        }
        sp_3072_mont_reduce_136(r, m, mp);
        mp = sp_3072_cmp_136(r, m);
        sp_3072_cond_sub_136(r, r, m, (mp < 0) - 1);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (d != NULL)
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return err;
#else
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_digit ad[272], md[136], rd[272];
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
    if (err == MP_OKAY && (mp_count_bits(em) > 23 || inLen > 384 ||
                                                     mp_count_bits(mm) != 3072))
        err = MP_READ_E;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 136 * 5, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        a = d;
        r = a + 136 * 2;
        m = r + 136 * 2;
    }
#else
    a = ad;
    m = md;
    r = rd;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 136, in, inLen);
#if DIGIT_BIT >= 23
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
        sp_3072_from_mp(m, 136, mm);

        if (e[0] == 0x3) {
            if (err == MP_OKAY) {
                sp_3072_sqr_136(r, a);
                err = sp_3072_mod_136(r, r, m);
            }
            if (err == MP_OKAY) {
                sp_3072_mul_136(r, a, r);
                err = sp_3072_mod_136(r, r, m);
            }
        }
        else {
            sp_digit* norm = r;
            int i;
            sp_digit mp;

            sp_3072_mont_setup(m, &mp);
            sp_3072_mont_norm_136(norm, m);

            if (err == MP_OKAY) {
                sp_3072_mul_136(a, a, norm);
                err = sp_3072_mod_136(a, a, m);
            }

            if (err == MP_OKAY) {
                for (i=22; i>=0; i--)
                    if (e[0] >> i)
                        break;

                XMEMCPY(r, a, sizeof(sp_digit) * 272);
                for (i--; i>=0; i--) {
                    sp_3072_mont_sqr_136(r, r, m, mp);

                    if (((e[0] >> i) & 1) == 1)
                        sp_3072_mont_mul_136(r, r, a, m, mp);
                }
                sp_3072_mont_reduce_136(r, m, mp);
                mp = sp_3072_cmp_136(r, m);
                sp_3072_cond_sub_136(r, r, m, (mp < 0) - 1);
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
        d = (sp_digit*)XMALLOC(sizeof(sp_digit) * 136 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = d + 136;
        m = a + 136;
        r = a;

        sp_3072_from_bin(a, 136, in, inLen);
        sp_3072_from_mp(d, 136, dm);
        sp_3072_from_mp(m, 136, mm);
        err = sp_3072_mod_exp_136(r, a, d, 3072, m, 0);
    }
    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (d != NULL) {
        XMEMSET(d, 0, sizeof(sp_digit) * 136);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[272], d[136], m[136];
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
        sp_3072_from_bin(a, 136, in, inLen);
        sp_3072_from_mp(d, 136, dm);
        sp_3072_from_mp(m, 136, mm);
        err = sp_3072_mod_exp_136(r, a, d, 3072, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    XMEMSET(d, 0, sizeof(sp_digit) * 136);

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
        t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 68 * 11, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (t == NULL)
            err = MEMORY_E;
    }
    if (err == MP_OKAY) {
        a = t;
        p = a + 136 * 2;
        q = p + 68;
        qi = dq = dp = q + 68;
        tmpa = qi + 68;
        tmpb = tmpa + 136;

        tmp = t;
        r = tmp + 136;

        sp_3072_from_bin(a, 136, in, inLen);
        sp_3072_from_mp(p, 68, pm);
        sp_3072_from_mp(q, 68, qm);
        sp_3072_from_mp(dp, 68, dpm);
        err = sp_3072_mod_exp_68(tmpa, a, dp, 1536, p, 1);
    }
    if (err == MP_OKAY) {
        sp_3072_from_mp(dq, 68, dqm);
        err = sp_3072_mod_exp_68(tmpb, a, dq, 1536, q, 1);
    }
    if (err == MP_OKAY) {
        sp_3072_sub_68(tmpa, tmpa, tmpb);
        sp_3072_mask_68(tmp, p, tmpa[67] >> 31);
        sp_3072_add_68(tmpa, tmpa, tmp);

        sp_3072_from_mp(qi, 68, qim);
        sp_3072_mul_68(tmpa, tmpa, qi);
        err = sp_3072_mod_68(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_3072_mul_68(tmpa, q, tmpa);
        sp_3072_add_136(r, tmpb, tmpa);
        sp_3072_norm_136(r);

        sp_3072_to_bin(r, out);
        *outLen = 384;
    }

    if (t != NULL) {
        XMEMSET(t, 0, sizeof(sp_digit) * 68 * 11);
        XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return err;
#else
    sp_digit a[136 * 2];
    sp_digit p[68], q[68], dp[68], dq[68], qi[68];
    sp_digit tmp[136], tmpa[136], tmpb[136];
    sp_digit* r = a;
    int err = MP_OKAY;

    (void)dm;
    (void)mm;

    if (*outLen < 384)
        err = MP_TO_E;
    if (err == MP_OKAY && (inLen > 384 || mp_count_bits(mm) != 3072))
        err = MP_READ_E;

    if (err == MP_OKAY) {
        sp_3072_from_bin(a, 136, in, inLen);
        sp_3072_from_mp(p, 68, pm);
        sp_3072_from_mp(q, 68, qm);
        sp_3072_from_mp(dp, 68, dpm);
        sp_3072_from_mp(dq, 68, dqm);
        sp_3072_from_mp(qi, 68, qim);

        err = sp_3072_mod_exp_68(tmpa, a, dp, 1536, p, 1);
    }
    if (err == MP_OKAY)
        err = sp_3072_mod_exp_68(tmpb, a, dq, 1536, q, 1);

    if (err == MP_OKAY) {
        sp_3072_sub_68(tmpa, tmpa, tmpb);
        sp_3072_mask_68(tmp, p, tmpa[67] >> 31);
        sp_3072_add_68(tmpa, tmpa, tmp);
        sp_3072_mul_68(tmpa, tmpa, qi);
        err = sp_3072_mod_68(tmpa, tmpa, p);
    }

    if (err == MP_OKAY) {
        sp_3072_mul_68(tmpa, tmpa, q);
        sp_3072_add_136(r, tmpb, tmpa);
        sp_3072_norm_136(r);

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
#if DIGIT_BIT == 23
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 136);
        r->used = 136;
        mp_clamp(r);
#elif DIGIT_BIT < 23
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 136; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 23) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 23 - s;
        }
        r->used = (3072 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 136; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 23 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 23 - s;
            }
            else
                s += 23;
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 136 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 136 * 2;
        m = e + 136;
        r = b;

        sp_3072_from_mp(b, 136, base);
        sp_3072_from_mp(e, 136, exp);
        sp_3072_from_mp(m, 136, mod);

        err = sp_3072_mod_exp_136(r, b, e, mp_count_bits(exp), m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_3072_to_mp(r, res);
    }

    if (d != NULL) {
        XMEMSET(e, 0, sizeof(sp_digit) * 136);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[272], ed[136], md[136];
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 136 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 136 * 2;
        m = e + 136;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 136, base);
        sp_3072_from_mp(e, 136, exp);
        sp_3072_from_mp(m, 136, mod);

        err = sp_3072_mod_exp_136(r, b, e, expBits, m, 0);
    }

    if (err == MP_OKAY) {
        err = sp_3072_to_mp(r, res);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 136);

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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 136 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 136 * 2;
        m = e + 136;
        r = b;

        sp_3072_from_mp(b, 136, base);
        sp_3072_from_bin(e, 136, exp, expLen);
        sp_3072_from_mp(m, 136, mod);

        err = sp_3072_mod_exp_136(r, b, e, expLen * 8, m, 0);
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
        XMEMSET(e, 0, sizeof(sp_digit) * 136);
        XFREE(d, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return err;
#else
#ifndef WOLFSSL_SMALL_STACK
    sp_digit bd[272], ed[136], md[136];
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
        d = (sp_digit*)XMALLOC(sizeof(*d) * 136 * 4, NULL,
                               DYNAMIC_TYPE_TMP_BUFFER);
        if (d == NULL)
            err = MEMORY_E;
    }

    if (err == MP_OKAY) {
        b = d;
        e = b + 136 * 2;
        m = e + 136;
        r = b;
    }
#else
    r = b = bd;
    e = ed;
    m = md;
#endif

    if (err == MP_OKAY) {
        sp_3072_from_mp(b, 136, base);
        sp_3072_from_bin(e, 136, exp, expLen);
        sp_3072_from_mp(m, 136, mod);

        err = sp_3072_mod_exp_136(r, b, e, expLen * 8, m, 0);
    }

    if (err == MP_OKAY) {
        sp_3072_to_bin(r, out);
        *outLen = 384;
        for (i=0; i<384 && out[i] == 0; i++) {
        }
        *outLen -= i;
        XMEMMOVE(out, out + i, *outLen);
    }

    XMEMSET(e, 0, sizeof(sp_digit) * 136);

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
    sp_digit x[2 * 10];
    sp_digit y[2 * 10];
    sp_digit z[2 * 10];
    int infinity;
} sp_point;

/* The modulus (prime) of the curve P256. */
static sp_digit p256_mod[10] = {
    0x3ffffff,0x3ffffff,0x3ffffff,0x003ffff,0x0000000,0x0000000,0x0000000,
    0x0000400,0x3ff0000,0x03fffff
};
#ifndef WOLFSSL_SP_SMALL
/* The Montogmery normalizer for modulus of the curve P256. */
static sp_digit p256_norm_mod[10] = {
    0x0000001,0x0000000,0x0000000,0x3fc0000,0x3ffffff,0x3ffffff,0x3ffffff,
    0x3fffbff,0x000ffff,0x0000000
};
#endif /* WOLFSSL_SP_SMALL */
/* The Montogmery multiplier for modulus of the curve P256. */
static sp_digit p256_mp_mod = 0x000001;
#if defined(WOLFSSL_VALIDATE_ECC_KEYGEN) || defined(HAVE_ECC_SIGN) || \
                                            defined(HAVE_ECC_VERIFY)
/* The order of the curve P256. */
static sp_digit p256_order[10] = {
    0x0632551,0x272b0bf,0x1e84f3b,0x2b69c5e,0x3bce6fa,0x3ffffff,0x3ffffff,
    0x00003ff,0x3ff0000,0x03fffff
};
#endif
/* The order of the curve P256 minus 2. */
static sp_digit p256_order2[10] = {
    0x063254f,0x272b0bf,0x1e84f3b,0x2b69c5e,0x3bce6fa,0x3ffffff,0x3ffffff,
    0x00003ff,0x3ff0000,0x03fffff
};
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery normalizer for order of the curve P256. */
static sp_digit p256_norm_order[10] = {
    0x39cdaaf,0x18d4f40,0x217b0c4,0x14963a1,0x0431905,0x0000000,0x0000000,
    0x3fffc00,0x000ffff,0x0000000
};
#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
/* The Montogmery multiplier for order of the curve P256. */
static sp_digit p256_mp_order = 0x200bc4f;
#endif
/* The base point of curve P256. */
static sp_point p256_base = {
    /* X ordinate */
    {
        0x098c296,0x04e5176,0x33a0f4a,0x204b7ac,0x277037d,0x0e9103c,0x3ce6e56,
        0x1091fe2,0x1f2e12c,0x01ac5f4
    },
    /* Y ordinate */
    {
        0x3bf51f5,0x1901a0d,0x1ececbb,0x15dacc5,0x22bce33,0x303e785,0x27eb4a7,
        0x1fe6e3b,0x2e2fe1a,0x013f8d0
    },
    /* Z ordinate */
    {
        0x0000001,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,0x0000000,
        0x0000000,0x0000000,0x0000000
    },
    /* infinity */
    0
};
#if defined(HAVE_ECC_CHECK_KEY) || defined(HAVE_COMP_KEY)
static sp_digit p256_b[10] = {
    0x3d2604b,0x38f0f89,0x30f63bc,0x2c3314e,0x0651d06,0x1a621af,0x2bbd557,
    0x24f9ecf,0x1d8aa3a,0x016b18d
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
static int sp_256_mod_mul_norm_10(sp_digit* r, sp_digit* a, sp_digit* m)
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
        a32[0] = a[0];
        a32[0] |= a[1] << 26;
        a32[0] &= 0xffffffff;
        a32[1] = (sp_digit)(a[1] >> 6);
        a32[1] |= a[2] << 20;
        a32[1] &= 0xffffffff;
        a32[2] = (sp_digit)(a[2] >> 12);
        a32[2] |= a[3] << 14;
        a32[2] &= 0xffffffff;
        a32[3] = (sp_digit)(a[3] >> 18);
        a32[3] |= a[4] << 8;
        a32[3] &= 0xffffffff;
        a32[4] = (sp_digit)(a[4] >> 24);
        a32[4] |= a[5] << 2;
        a32[4] |= a[6] << 28;
        a32[4] &= 0xffffffff;
        a32[5] = (sp_digit)(a[6] >> 4);
        a32[5] |= a[7] << 22;
        a32[5] &= 0xffffffff;
        a32[6] = (sp_digit)(a[7] >> 10);
        a32[6] |= a[8] << 16;
        a32[6] &= 0xffffffff;
        a32[7] = (sp_digit)(a[8] >> 16);
        a32[7] |= a[9] << 10;
        a32[7] &= 0xffffffff;

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

        r[0] = (sp_digit)(t[0]) & 0x3ffffff;
        r[1] = (sp_digit)(t[0] >> 26);
        r[1] |= t[1] << 6;
        r[1] &= 0x3ffffff;
        r[2] = (sp_digit)(t[1] >> 20);
        r[2] |= t[2] << 12;
        r[2] &= 0x3ffffff;
        r[3] = (sp_digit)(t[2] >> 14);
        r[3] |= t[3] << 18;
        r[3] &= 0x3ffffff;
        r[4] = (sp_digit)(t[3] >> 8);
        r[4] |= t[4] << 24;
        r[4] &= 0x3ffffff;
        r[5] = (sp_digit)(t[4] >> 2) & 0x3ffffff;
        r[6] = (sp_digit)(t[4] >> 28);
        r[6] |= t[5] << 4;
        r[6] &= 0x3ffffff;
        r[7] = (sp_digit)(t[5] >> 22);
        r[7] |= t[6] << 10;
        r[7] &= 0x3ffffff;
        r[8] = (sp_digit)(t[6] >> 16);
        r[8] |= t[7] << 16;
        r[8] &= 0x3ffffff;
        r[9] = (sp_digit)(t[7] >> 10);
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
#if DIGIT_BIT == 26
    int j;

    XMEMCPY(r, a->dp, sizeof(sp_digit) * a->used);

    for (j = a->used; j < max; j++)
        r[j] = 0;
#elif DIGIT_BIT > 26
    int i, j = 0, s = 0;

    r[0] = 0;
    for (i = 0; i < a->used && j < max; i++) {
        r[j] |= a->dp[i] << s;
        r[j] &= 0x3ffffff;
        s = 26 - s;
        if (j + 1 >= max)
            break;
        r[++j] = a->dp[i] >> s;
        while (s + 26 <= DIGIT_BIT) {
            s += 26;
            r[j] &= 0x3ffffff;
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
        if (s + DIGIT_BIT >= 26) {
            r[j] &= 0x3ffffff;
            if (j + 1 >= max)
                break;
            s = 26 - s;
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
static void sp_256_point_from_ecc_point_10(sp_point* p, ecc_point* pm)
{
    XMEMSET(p->x, 0, sizeof(p->x));
    XMEMSET(p->y, 0, sizeof(p->y));
    XMEMSET(p->z, 0, sizeof(p->z));
    sp_256_from_mp(p->x, 10, pm->x);
    sp_256_from_mp(p->y, 10, pm->y);
    sp_256_from_mp(p->z, 10, pm->z);
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
#if DIGIT_BIT == 26
        XMEMCPY(r->dp, a, sizeof(sp_digit) * 10);
        r->used = 10;
        mp_clamp(r);
#elif DIGIT_BIT < 26
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 10; i++) {
            r->dp[j] |= a[i] << s;
            r->dp[j] &= (1l << DIGIT_BIT) - 1;
            s = DIGIT_BIT - s;
            r->dp[++j] = a[i] >> s;
            while (s + DIGIT_BIT <= 26) {
                s += DIGIT_BIT;
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
                r->dp[++j] = a[i] >> s;
            }
            s = 26 - s;
        }
        r->used = (256 + DIGIT_BIT - 1) / DIGIT_BIT;
        mp_clamp(r);
#else
        int i, j = 0, s = 0;

        r->dp[0] = 0;
        for (i = 0; i < 10; i++) {
            r->dp[j] |= ((mp_digit)a[i]) << s;
            if (s + 26 >= DIGIT_BIT) {
    #if DIGIT_BIT < 32
                r->dp[j] &= (1l << DIGIT_BIT) - 1;
    #endif
                s = DIGIT_BIT - s;
                r->dp[++j] = a[i] >> s;
                s = 26 - s;
            }
            else
                s += 26;
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
static int sp_256_point_to_ecc_point_10(sp_point* p, ecc_point* pm)
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
static sp_digit sp_256_cmp_10(const sp_digit* a, const sp_digit* b)
{
    sp_digit r = 0;
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=9; i>=0; i--)
        r |= (a[i] - b[i]) & (0 - !r);
#else
    r |= (a[ 9] - b[ 9]) & (0 - !r);
    r |= (a[ 8] - b[ 8]) & (0 - !r);
    r |= (a[ 7] - b[ 7]) & (0 - !r);
    r |= (a[ 6] - b[ 6]) & (0 - !r);
    r |= (a[ 5] - b[ 5]) & (0 - !r);
    r |= (a[ 4] - b[ 4]) & (0 - !r);
    r |= (a[ 3] - b[ 3]) & (0 - !r);
    r |= (a[ 2] - b[ 2]) & (0 - !r);
    r |= (a[ 1] - b[ 1]) & (0 - !r);
    r |= (a[ 0] - b[ 0]) & (0 - !r);
#endif /* WOLFSSL_SP_SMALL */

    return r;
}

/* Normalize the values in each word to 26.
 *
 * a  Array of sp_digit to normalize.
 */
static void sp_256_norm_10(sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    for (i = 0; i < 9; i++) {
        a[i+1] += a[i] >> 26;
        a[i] &= 0x3ffffff;
    }
#else
    a[1] += a[0] >> 26; a[0] &= 0x3ffffff;
    a[2] += a[1] >> 26; a[1] &= 0x3ffffff;
    a[3] += a[2] >> 26; a[2] &= 0x3ffffff;
    a[4] += a[3] >> 26; a[3] &= 0x3ffffff;
    a[5] += a[4] >> 26; a[4] &= 0x3ffffff;
    a[6] += a[5] >> 26; a[5] &= 0x3ffffff;
    a[7] += a[6] >> 26; a[6] &= 0x3ffffff;
    a[8] += a[7] >> 26; a[7] &= 0x3ffffff;
    a[9] += a[8] >> 26; a[8] &= 0x3ffffff;
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
static void sp_256_cond_sub_10(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 10; i++)
        r[i] = a[i] - (b[i] & m);
#else
    r[ 0] = a[ 0] - (b[ 0] & m);
    r[ 1] = a[ 1] - (b[ 1] & m);
    r[ 2] = a[ 2] - (b[ 2] & m);
    r[ 3] = a[ 3] - (b[ 3] & m);
    r[ 4] = a[ 4] - (b[ 4] & m);
    r[ 5] = a[ 5] - (b[ 5] & m);
    r[ 6] = a[ 6] - (b[ 6] & m);
    r[ 7] = a[ 7] - (b[ 7] & m);
    r[ 8] = a[ 8] - (b[ 8] & m);
    r[ 9] = a[ 9] - (b[ 9] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Mul a by scalar b and add into r. (r += a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A scalar.
 */
SP_NOINLINE static void sp_256_mul_add_10(sp_digit* r, const sp_digit* a,
        const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 10; i++) {
        t += (tb * a[i]) + r[i];
        r[i] = t & 0x3ffffff;
        t >>= 26;
    }
    r[10] += t;
#else
    int64_t tb = b;
    int64_t t[10];

    t[ 0] = tb * a[ 0];
    t[ 1] = tb * a[ 1];
    t[ 2] = tb * a[ 2];
    t[ 3] = tb * a[ 3];
    t[ 4] = tb * a[ 4];
    t[ 5] = tb * a[ 5];
    t[ 6] = tb * a[ 6];
    t[ 7] = tb * a[ 7];
    t[ 8] = tb * a[ 8];
    t[ 9] = tb * a[ 9];
    r[ 0] +=                 (t[ 0] & 0x3ffffff);
    r[ 1] += (t[ 0] >> 26) + (t[ 1] & 0x3ffffff);
    r[ 2] += (t[ 1] >> 26) + (t[ 2] & 0x3ffffff);
    r[ 3] += (t[ 2] >> 26) + (t[ 3] & 0x3ffffff);
    r[ 4] += (t[ 3] >> 26) + (t[ 4] & 0x3ffffff);
    r[ 5] += (t[ 4] >> 26) + (t[ 5] & 0x3ffffff);
    r[ 6] += (t[ 5] >> 26) + (t[ 6] & 0x3ffffff);
    r[ 7] += (t[ 6] >> 26) + (t[ 7] & 0x3ffffff);
    r[ 8] += (t[ 7] >> 26) + (t[ 8] & 0x3ffffff);
    r[ 9] += (t[ 8] >> 26) + (t[ 9] & 0x3ffffff);
    r[10] +=  t[ 9] >> 26;
#endif /* WOLFSSL_SP_SMALL */
}

/* Shift the result in the high 256 bits down to the bottom.
 *
 * r  A single precision number.
 * a  A single precision number.
 */
static void sp_256_mont_shift_10(sp_digit* r, const sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;
    sp_digit n, s;

    s = a[10];
    n = a[9] >> 22;
    for (i = 0; i < 9; i++) {
        n += (s & 0x3ffffff) << 4;
        r[i] = n & 0x3ffffff;
        n >>= 26;
        s = a[11 + i] + (s >> 26);
    }
    n += s << 4;
    r[9] = n;
#else
    sp_digit n, s;

    s = a[10]; n = a[9] >> 22;
    n += (s & 0x3ffffff) << 4; r[ 0] = n & 0x3ffffff;
    n >>= 26; s = a[11] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 1] = n & 0x3ffffff;
    n >>= 26; s = a[12] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 2] = n & 0x3ffffff;
    n >>= 26; s = a[13] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 3] = n & 0x3ffffff;
    n >>= 26; s = a[14] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 4] = n & 0x3ffffff;
    n >>= 26; s = a[15] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 5] = n & 0x3ffffff;
    n >>= 26; s = a[16] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 6] = n & 0x3ffffff;
    n >>= 26; s = a[17] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 7] = n & 0x3ffffff;
    n >>= 26; s = a[18] + (s >> 26);
    n += (s & 0x3ffffff) << 4; r[ 8] = n & 0x3ffffff;
    n >>= 26; s = a[19] + (s >> 26);
    n += s << 4;              r[ 9] = n;
#endif /* WOLFSSL_SP_SMALL */
    XMEMSET(&r[10], 0, sizeof(*r) * 10);
}

/* Reduce the number back to 256 bits using Montgomery reduction.
 *
 * a   A single precision number to reduce in place.
 * m   The single precision number representing the modulus.
 * mp  The digit representing the negative inverse of m mod 2^n.
 */
static void sp_256_mont_reduce_10(sp_digit* a, sp_digit* m, sp_digit mp)
{
    int i;
    sp_digit mu;

    if (mp != 1) {
        for (i=0; i<9; i++) {
            mu = (a[i] * mp) & 0x3ffffff;
            sp_256_mul_add_10(a+i, m, mu);
            a[i+1] += a[i] >> 26;
        }
        mu = (a[i] * mp) & 0x3fffffl;
        sp_256_mul_add_10(a+i, m, mu);
        a[i+1] += a[i] >> 26;
        a[i] &= 0x3ffffff;
    }
    else {
        for (i=0; i<9; i++) {
            mu = a[i] & 0x3ffffff;
            sp_256_mul_add_10(a+i, p256_mod, mu);
            a[i+1] += a[i] >> 26;
        }
        mu = a[i] & 0x3fffffl;
        sp_256_mul_add_10(a+i, p256_mod, mu);
        a[i+1] += a[i] >> 26;
        a[i] &= 0x3ffffff;
    }

    sp_256_mont_shift_10(a, a);
    sp_256_cond_sub_10(a, a, m, 0 - ((a[9] >> 22) > 0));
    sp_256_norm_10(a);
}

#ifdef WOLFSSL_SP_SMALL
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_256_mul_10(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[9]) * b[9];
    r[19] = (sp_digit)(c >> 26);
    c = (c & 0x3ffffff) << 26;
    for (k = 17; k >= 0; k--) {
        for (i = 9; i >= 0; i--) {
            j = k - i;
            if (j >= 10)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * b[j];
        }
        r[k + 2] += c >> 52;
        r[k + 1] = (c >> 26) & 0x3ffffff;
        c = (c & 0x3ffffff) << 26;
    }
    r[0] = (sp_digit)(c >> 26);
}

#else
/* Multiply a and b into r. (r = a * b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static void sp_256_mul_10(sp_digit* r, const sp_digit* a,
    const sp_digit* b)
{
    int64_t t0   = ((int64_t)a[ 0]) * b[ 0];
    int64_t t1   = ((int64_t)a[ 0]) * b[ 1]
                 + ((int64_t)a[ 1]) * b[ 0];
    int64_t t2   = ((int64_t)a[ 0]) * b[ 2]
                 + ((int64_t)a[ 1]) * b[ 1]
                 + ((int64_t)a[ 2]) * b[ 0];
    int64_t t3   = ((int64_t)a[ 0]) * b[ 3]
                 + ((int64_t)a[ 1]) * b[ 2]
                 + ((int64_t)a[ 2]) * b[ 1]
                 + ((int64_t)a[ 3]) * b[ 0];
    int64_t t4   = ((int64_t)a[ 0]) * b[ 4]
                 + ((int64_t)a[ 1]) * b[ 3]
                 + ((int64_t)a[ 2]) * b[ 2]
                 + ((int64_t)a[ 3]) * b[ 1]
                 + ((int64_t)a[ 4]) * b[ 0];
    int64_t t5   = ((int64_t)a[ 0]) * b[ 5]
                 + ((int64_t)a[ 1]) * b[ 4]
                 + ((int64_t)a[ 2]) * b[ 3]
                 + ((int64_t)a[ 3]) * b[ 2]
                 + ((int64_t)a[ 4]) * b[ 1]
                 + ((int64_t)a[ 5]) * b[ 0];
    int64_t t6   = ((int64_t)a[ 0]) * b[ 6]
                 + ((int64_t)a[ 1]) * b[ 5]
                 + ((int64_t)a[ 2]) * b[ 4]
                 + ((int64_t)a[ 3]) * b[ 3]
                 + ((int64_t)a[ 4]) * b[ 2]
                 + ((int64_t)a[ 5]) * b[ 1]
                 + ((int64_t)a[ 6]) * b[ 0];
    int64_t t7   = ((int64_t)a[ 0]) * b[ 7]
                 + ((int64_t)a[ 1]) * b[ 6]
                 + ((int64_t)a[ 2]) * b[ 5]
                 + ((int64_t)a[ 3]) * b[ 4]
                 + ((int64_t)a[ 4]) * b[ 3]
                 + ((int64_t)a[ 5]) * b[ 2]
                 + ((int64_t)a[ 6]) * b[ 1]
                 + ((int64_t)a[ 7]) * b[ 0];
    int64_t t8   = ((int64_t)a[ 0]) * b[ 8]
                 + ((int64_t)a[ 1]) * b[ 7]
                 + ((int64_t)a[ 2]) * b[ 6]
                 + ((int64_t)a[ 3]) * b[ 5]
                 + ((int64_t)a[ 4]) * b[ 4]
                 + ((int64_t)a[ 5]) * b[ 3]
                 + ((int64_t)a[ 6]) * b[ 2]
                 + ((int64_t)a[ 7]) * b[ 1]
                 + ((int64_t)a[ 8]) * b[ 0];
    int64_t t9   = ((int64_t)a[ 0]) * b[ 9]
                 + ((int64_t)a[ 1]) * b[ 8]
                 + ((int64_t)a[ 2]) * b[ 7]
                 + ((int64_t)a[ 3]) * b[ 6]
                 + ((int64_t)a[ 4]) * b[ 5]
                 + ((int64_t)a[ 5]) * b[ 4]
                 + ((int64_t)a[ 6]) * b[ 3]
                 + ((int64_t)a[ 7]) * b[ 2]
                 + ((int64_t)a[ 8]) * b[ 1]
                 + ((int64_t)a[ 9]) * b[ 0];
    int64_t t10  = ((int64_t)a[ 1]) * b[ 9]
                 + ((int64_t)a[ 2]) * b[ 8]
                 + ((int64_t)a[ 3]) * b[ 7]
                 + ((int64_t)a[ 4]) * b[ 6]
                 + ((int64_t)a[ 5]) * b[ 5]
                 + ((int64_t)a[ 6]) * b[ 4]
                 + ((int64_t)a[ 7]) * b[ 3]
                 + ((int64_t)a[ 8]) * b[ 2]
                 + ((int64_t)a[ 9]) * b[ 1];
    int64_t t11  = ((int64_t)a[ 2]) * b[ 9]
                 + ((int64_t)a[ 3]) * b[ 8]
                 + ((int64_t)a[ 4]) * b[ 7]
                 + ((int64_t)a[ 5]) * b[ 6]
                 + ((int64_t)a[ 6]) * b[ 5]
                 + ((int64_t)a[ 7]) * b[ 4]
                 + ((int64_t)a[ 8]) * b[ 3]
                 + ((int64_t)a[ 9]) * b[ 2];
    int64_t t12  = ((int64_t)a[ 3]) * b[ 9]
                 + ((int64_t)a[ 4]) * b[ 8]
                 + ((int64_t)a[ 5]) * b[ 7]
                 + ((int64_t)a[ 6]) * b[ 6]
                 + ((int64_t)a[ 7]) * b[ 5]
                 + ((int64_t)a[ 8]) * b[ 4]
                 + ((int64_t)a[ 9]) * b[ 3];
    int64_t t13  = ((int64_t)a[ 4]) * b[ 9]
                 + ((int64_t)a[ 5]) * b[ 8]
                 + ((int64_t)a[ 6]) * b[ 7]
                 + ((int64_t)a[ 7]) * b[ 6]
                 + ((int64_t)a[ 8]) * b[ 5]
                 + ((int64_t)a[ 9]) * b[ 4];
    int64_t t14  = ((int64_t)a[ 5]) * b[ 9]
                 + ((int64_t)a[ 6]) * b[ 8]
                 + ((int64_t)a[ 7]) * b[ 7]
                 + ((int64_t)a[ 8]) * b[ 6]
                 + ((int64_t)a[ 9]) * b[ 5];
    int64_t t15  = ((int64_t)a[ 6]) * b[ 9]
                 + ((int64_t)a[ 7]) * b[ 8]
                 + ((int64_t)a[ 8]) * b[ 7]
                 + ((int64_t)a[ 9]) * b[ 6];
    int64_t t16  = ((int64_t)a[ 7]) * b[ 9]
                 + ((int64_t)a[ 8]) * b[ 8]
                 + ((int64_t)a[ 9]) * b[ 7];
    int64_t t17  = ((int64_t)a[ 8]) * b[ 9]
                 + ((int64_t)a[ 9]) * b[ 8];
    int64_t t18  = ((int64_t)a[ 9]) * b[ 9];

    t1   += t0  >> 26; r[ 0] = t0  & 0x3ffffff;
    t2   += t1  >> 26; r[ 1] = t1  & 0x3ffffff;
    t3   += t2  >> 26; r[ 2] = t2  & 0x3ffffff;
    t4   += t3  >> 26; r[ 3] = t3  & 0x3ffffff;
    t5   += t4  >> 26; r[ 4] = t4  & 0x3ffffff;
    t6   += t5  >> 26; r[ 5] = t5  & 0x3ffffff;
    t7   += t6  >> 26; r[ 6] = t6  & 0x3ffffff;
    t8   += t7  >> 26; r[ 7] = t7  & 0x3ffffff;
    t9   += t8  >> 26; r[ 8] = t8  & 0x3ffffff;
    t10  += t9  >> 26; r[ 9] = t9  & 0x3ffffff;
    t11  += t10 >> 26; r[10] = t10 & 0x3ffffff;
    t12  += t11 >> 26; r[11] = t11 & 0x3ffffff;
    t13  += t12 >> 26; r[12] = t12 & 0x3ffffff;
    t14  += t13 >> 26; r[13] = t13 & 0x3ffffff;
    t15  += t14 >> 26; r[14] = t14 & 0x3ffffff;
    t16  += t15 >> 26; r[15] = t15 & 0x3ffffff;
    t17  += t16 >> 26; r[16] = t16 & 0x3ffffff;
    t18  += t17 >> 26; r[17] = t17 & 0x3ffffff;
    r[19] = (sp_digit)(t18 >> 26);
                       r[18] = t18 & 0x3ffffff;
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
static void sp_256_mont_mul_10(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m, sp_digit mp)
{
    sp_256_mul_10(r, a, b);
    sp_256_mont_reduce_10(r, m, mp);
}

#ifdef WOLFSSL_SP_SMALL
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_sqr_10(sp_digit* r, const sp_digit* a)
{
    int i, j, k;
    int64_t c;

    c = ((int64_t)a[9]) * a[9];
    r[19] = (sp_digit)(c >> 26);
    c = (c & 0x3ffffff) << 26;
    for (k = 17; k >= 0; k--) {
        for (i = 9; i >= 0; i--) {
            j = k - i;
            if (j >= 10 || i <= j)
                break;
            if (j < 0)
                continue;

            c += ((int64_t)a[i]) * a[j] * 2;
        }
        if (i == j)
           c += ((int64_t)a[i]) * a[i];

        r[k + 2] += c >> 52;
        r[k + 1] = (c >> 26) & 0x3ffffff;
        c = (c & 0x3ffffff) << 26;
    }
    r[0] = (sp_digit)(c >> 26);
}

#else
/* Square a and put result in r. (r = a * a)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_sqr_10(sp_digit* r, const sp_digit* a)
{
    int64_t t0   =  ((int64_t)a[ 0]) * a[ 0];
    int64_t t1   = (((int64_t)a[ 0]) * a[ 1]) * 2;
    int64_t t2   = (((int64_t)a[ 0]) * a[ 2]) * 2
                 +  ((int64_t)a[ 1]) * a[ 1];
    int64_t t3   = (((int64_t)a[ 0]) * a[ 3]
                 +  ((int64_t)a[ 1]) * a[ 2]) * 2;
    int64_t t4   = (((int64_t)a[ 0]) * a[ 4]
                 +  ((int64_t)a[ 1]) * a[ 3]) * 2
                 +  ((int64_t)a[ 2]) * a[ 2];
    int64_t t5   = (((int64_t)a[ 0]) * a[ 5]
                 +  ((int64_t)a[ 1]) * a[ 4]
                 +  ((int64_t)a[ 2]) * a[ 3]) * 2;
    int64_t t6   = (((int64_t)a[ 0]) * a[ 6]
                 +  ((int64_t)a[ 1]) * a[ 5]
                 +  ((int64_t)a[ 2]) * a[ 4]) * 2
                 +  ((int64_t)a[ 3]) * a[ 3];
    int64_t t7   = (((int64_t)a[ 0]) * a[ 7]
                 +  ((int64_t)a[ 1]) * a[ 6]
                 +  ((int64_t)a[ 2]) * a[ 5]
                 +  ((int64_t)a[ 3]) * a[ 4]) * 2;
    int64_t t8   = (((int64_t)a[ 0]) * a[ 8]
                 +  ((int64_t)a[ 1]) * a[ 7]
                 +  ((int64_t)a[ 2]) * a[ 6]
                 +  ((int64_t)a[ 3]) * a[ 5]) * 2
                 +  ((int64_t)a[ 4]) * a[ 4];
    int64_t t9   = (((int64_t)a[ 0]) * a[ 9]
                 +  ((int64_t)a[ 1]) * a[ 8]
                 +  ((int64_t)a[ 2]) * a[ 7]
                 +  ((int64_t)a[ 3]) * a[ 6]
                 +  ((int64_t)a[ 4]) * a[ 5]) * 2;
    int64_t t10  = (((int64_t)a[ 1]) * a[ 9]
                 +  ((int64_t)a[ 2]) * a[ 8]
                 +  ((int64_t)a[ 3]) * a[ 7]
                 +  ((int64_t)a[ 4]) * a[ 6]) * 2
                 +  ((int64_t)a[ 5]) * a[ 5];
    int64_t t11  = (((int64_t)a[ 2]) * a[ 9]
                 +  ((int64_t)a[ 3]) * a[ 8]
                 +  ((int64_t)a[ 4]) * a[ 7]
                 +  ((int64_t)a[ 5]) * a[ 6]) * 2;
    int64_t t12  = (((int64_t)a[ 3]) * a[ 9]
                 +  ((int64_t)a[ 4]) * a[ 8]
                 +  ((int64_t)a[ 5]) * a[ 7]) * 2
                 +  ((int64_t)a[ 6]) * a[ 6];
    int64_t t13  = (((int64_t)a[ 4]) * a[ 9]
                 +  ((int64_t)a[ 5]) * a[ 8]
                 +  ((int64_t)a[ 6]) * a[ 7]) * 2;
    int64_t t14  = (((int64_t)a[ 5]) * a[ 9]
                 +  ((int64_t)a[ 6]) * a[ 8]) * 2
                 +  ((int64_t)a[ 7]) * a[ 7];
    int64_t t15  = (((int64_t)a[ 6]) * a[ 9]
                 +  ((int64_t)a[ 7]) * a[ 8]) * 2;
    int64_t t16  = (((int64_t)a[ 7]) * a[ 9]) * 2
                 +  ((int64_t)a[ 8]) * a[ 8];
    int64_t t17  = (((int64_t)a[ 8]) * a[ 9]) * 2;
    int64_t t18  =  ((int64_t)a[ 9]) * a[ 9];

    t1   += t0  >> 26; r[ 0] = t0  & 0x3ffffff;
    t2   += t1  >> 26; r[ 1] = t1  & 0x3ffffff;
    t3   += t2  >> 26; r[ 2] = t2  & 0x3ffffff;
    t4   += t3  >> 26; r[ 3] = t3  & 0x3ffffff;
    t5   += t4  >> 26; r[ 4] = t4  & 0x3ffffff;
    t6   += t5  >> 26; r[ 5] = t5  & 0x3ffffff;
    t7   += t6  >> 26; r[ 6] = t6  & 0x3ffffff;
    t8   += t7  >> 26; r[ 7] = t7  & 0x3ffffff;
    t9   += t8  >> 26; r[ 8] = t8  & 0x3ffffff;
    t10  += t9  >> 26; r[ 9] = t9  & 0x3ffffff;
    t11  += t10 >> 26; r[10] = t10 & 0x3ffffff;
    t12  += t11 >> 26; r[11] = t11 & 0x3ffffff;
    t13  += t12 >> 26; r[12] = t12 & 0x3ffffff;
    t14  += t13 >> 26; r[13] = t13 & 0x3ffffff;
    t15  += t14 >> 26; r[14] = t14 & 0x3ffffff;
    t16  += t15 >> 26; r[15] = t15 & 0x3ffffff;
    t17  += t16 >> 26; r[16] = t16 & 0x3ffffff;
    t18  += t17 >> 26; r[17] = t17 & 0x3ffffff;
    r[19] = (sp_digit)(t18 >> 26);
                       r[18] = t18 & 0x3ffffff;
}

#endif /* WOLFSSL_SP_SMALL */
/* Square the Montgomery form number. (r = a * a mod m)
 *
 * r   Result of squaring.
 * a   Number to square in Montogmery form.
 * m   Modulus (prime).
 * mp  Montogmery mulitplier.
 */
static void sp_256_mont_sqr_10(sp_digit* r, sp_digit* a, sp_digit* m,
        sp_digit mp)
{
    sp_256_sqr_10(r, a);
    sp_256_mont_reduce_10(r, m, mp);
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
static void sp_256_mont_sqr_n_10(sp_digit* r, sp_digit* a, int n,
        sp_digit* m, sp_digit mp)
{
    sp_256_mont_sqr_10(r, a, m, mp);
    for (; n > 1; n--)
        sp_256_mont_sqr_10(r, r, m, mp);
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
static void sp_256_mont_inv_10(sp_digit* r, sp_digit* a, sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 10);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_10(t, t, p256_mod, p256_mp_mod);
        if (p256_mod_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_10(t, t, a, p256_mod, p256_mp_mod);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 10);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 10;
    sp_digit* t3 = td + 4 * 10;

    /* t = a^2 */
    sp_256_mont_sqr_10(t, a, p256_mod, p256_mp_mod);
    /* t = a^3 = t * a */
    sp_256_mont_mul_10(t, t, a, p256_mod, p256_mp_mod);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_10(t2, t, 2, p256_mod, p256_mp_mod);
    /* t3= a^d = t2 * a */
    sp_256_mont_mul_10(t3, t2, a, p256_mod, p256_mp_mod);
    /* t = a^f = t2 * t */
    sp_256_mont_mul_10(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^f0 = t ^ 2 ^ 4 */
    sp_256_mont_sqr_n_10(t2, t, 4, p256_mod, p256_mp_mod);
    /* t3= a^fd = t2 * t3 */
    sp_256_mont_mul_10(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ff = t2 * t */
    sp_256_mont_mul_10(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_10(t2, t, 8, p256_mod, p256_mp_mod);
    /* t3= a^fffd = t2 * t3 */
    sp_256_mont_mul_10(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_10(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_10(t2, t, 16, p256_mod, p256_mp_mod);
    /* t3= a^fffffffd = t2 * t3 */
    sp_256_mont_mul_10(t3, t2, t3, p256_mod, p256_mp_mod);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_10(t, t2, t, p256_mod, p256_mp_mod);
    /* t = a^ffffffff00000000 = t ^ 2 ^ 32  */
    sp_256_mont_sqr_n_10(t2, t, 32, p256_mod, p256_mp_mod);
    /* t2= a^ffffffffffffffff = t2 * t */
    sp_256_mont_mul_10(t, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001 = t2 * a */
    sp_256_mont_mul_10(t2, t2, a, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff000000010000000000000000000000000000000000000000
     *   = t2 ^ 2 ^ 160 */
    sp_256_mont_sqr_n_10(t2, t2, 160, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff
     *   = t2 * t */
    sp_256_mont_mul_10(t2, t2, t, p256_mod, p256_mp_mod);
    /* t2= a^ffffffff00000001000000000000000000000000ffffffffffffffff00000000
     *   = t2 ^ 2 ^ 32 */
    sp_256_mont_sqr_n_10(t2, t2, 32, p256_mod, p256_mp_mod);
    /* r = a^ffffffff00000001000000000000000000000000fffffffffffffffffffffffd
     *   = t2 * t3 */
    sp_256_mont_mul_10(r, t2, t3, p256_mod, p256_mp_mod);
#endif /* WOLFSSL_SP_SMALL */
}

/* Map the Montgomery form projective co-ordinate point to an affine point.
 *
 * r  Resulting affine co-ordinate point.
 * p  Montgomery form projective co-ordinate point.
 * t  Temporary ordinate data.
 */
static void sp_256_map_10(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*10;
    int32_t n;

    sp_256_mont_inv_10(t1, p->z, t + 2*10);

    sp_256_mont_sqr_10(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_10(t1, t2, t1, p256_mod, p256_mp_mod);

    /* x /= z^2 */
    sp_256_mont_mul_10(r->x, p->x, t2, p256_mod, p256_mp_mod);
    XMEMSET(r->x + 10, 0, sizeof(r->x) / 2);
    sp_256_mont_reduce_10(r->x, p256_mod, p256_mp_mod);
    /* Reduce x to less than modulus */
    n = sp_256_cmp_10(r->x, p256_mod);
    sp_256_cond_sub_10(r->x, r->x, p256_mod, 0 - (n >= 0));
    sp_256_norm_10(r->x);

    /* y /= z^3 */
    sp_256_mont_mul_10(r->y, p->y, t1, p256_mod, p256_mp_mod);
    XMEMSET(r->y + 10, 0, sizeof(r->y) / 2);
    sp_256_mont_reduce_10(r->y, p256_mod, p256_mp_mod);
    /* Reduce y to less than modulus */
    n = sp_256_cmp_10(r->y, p256_mod);
    sp_256_cond_sub_10(r->y, r->y, p256_mod, 0 - (n >= 0));
    sp_256_norm_10(r->y);

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
SP_NOINLINE static int sp_256_add_10(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 10; i++)
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
SP_NOINLINE static int sp_256_add_10(sp_digit* r, const sp_digit* a,
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
    r[ 9] = a[ 9] + b[ 9];

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
static void sp_256_mont_add_10(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    sp_256_add_10(r, a, b);
    sp_256_norm_10(r);
    sp_256_cond_sub_10(r, r, m, 0 - ((r[9] >> 22) > 0));
    sp_256_norm_10(r);
}

/* Double a Montgomery form number (r = a + a % m).
 *
 * r   Result of doubling.
 * a   Number to double in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_dbl_10(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_add_10(r, a, a);
    sp_256_norm_10(r);
    sp_256_cond_sub_10(r, r, m, 0 - ((r[9] >> 22) > 0));
    sp_256_norm_10(r);
}

/* Triple a Montgomery form number (r = a + a + a % m).
 *
 * r   Result of Tripling.
 * a   Number to triple in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_tpl_10(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_add_10(r, a, a);
    sp_256_norm_10(r);
    sp_256_cond_sub_10(r, r, m, 0 - ((r[9] >> 22) > 0));
    sp_256_norm_10(r);
    sp_256_add_10(r, r, a);
    sp_256_norm_10(r);
    sp_256_cond_sub_10(r, r, m, 0 - ((r[9] >> 22) > 0));
    sp_256_norm_10(r);
}

#ifdef WOLFSSL_SP_SMALL
/* Sub b from a into r. (r = a - b)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 * b  A single precision integer.
 */
SP_NOINLINE static int sp_256_sub_10(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    int i;

    for (i = 0; i < 10; i++)
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
SP_NOINLINE static int sp_256_sub_10(sp_digit* r, const sp_digit* a,
        const sp_digit* b)
{
    r[ 0] = a[ 0] - b[ 0];
    r[ 1] = a[ 1] - b[ 1];
    r[ 2] = a[ 2] - b[ 2];
    r[ 3] = a[ 3] - b[ 3];
    r[ 4] = a[ 4] - b[ 4];
    r[ 5] = a[ 5] - b[ 5];
    r[ 6] = a[ 6] - b[ 6];
    r[ 7] = a[ 7] - b[ 7];
    r[ 8] = a[ 8] - b[ 8];
    r[ 9] = a[ 9] - b[ 9];

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
static void sp_256_cond_add_10(sp_digit* r, const sp_digit* a,
        const sp_digit* b, const sp_digit m)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i = 0; i < 10; i++)
        r[i] = a[i] + (b[i] & m);
#else
    r[ 0] = a[ 0] + (b[ 0] & m);
    r[ 1] = a[ 1] + (b[ 1] & m);
    r[ 2] = a[ 2] + (b[ 2] & m);
    r[ 3] = a[ 3] + (b[ 3] & m);
    r[ 4] = a[ 4] + (b[ 4] & m);
    r[ 5] = a[ 5] + (b[ 5] & m);
    r[ 6] = a[ 6] + (b[ 6] & m);
    r[ 7] = a[ 7] + (b[ 7] & m);
    r[ 8] = a[ 8] + (b[ 8] & m);
    r[ 9] = a[ 9] + (b[ 9] & m);
#endif /* WOLFSSL_SP_SMALL */
}

/* Subtract two Montgomery form numbers (r = a - b % m).
 *
 * r   Result of subtration.
 * a   Number to subtract from in Montogmery form.
 * b   Number to subtract with in Montogmery form.
 * m   Modulus (prime).
 */
static void sp_256_mont_sub_10(sp_digit* r, sp_digit* a, sp_digit* b,
        sp_digit* m)
{
    sp_256_sub_10(r, a, b);
    sp_256_cond_add_10(r, r, m, r[9] >> 22);
    sp_256_norm_10(r);
}

/* Shift number left one bit.
 * Bottom bit is lost.
 *
 * r  Result of shift.
 * a  Number to shift.
 */
SP_NOINLINE static void sp_256_rshift1_10(sp_digit* r, sp_digit* a)
{
#ifdef WOLFSSL_SP_SMALL
    int i;

    for (i=0; i<9; i++)
        r[i] = ((a[i] >> 1) | (a[i + 1] << 25)) & 0x3ffffff;
#else
    r[0] = ((a[0] >> 1) | (a[1] << 25)) & 0x3ffffff;
    r[1] = ((a[1] >> 1) | (a[2] << 25)) & 0x3ffffff;
    r[2] = ((a[2] >> 1) | (a[3] << 25)) & 0x3ffffff;
    r[3] = ((a[3] >> 1) | (a[4] << 25)) & 0x3ffffff;
    r[4] = ((a[4] >> 1) | (a[5] << 25)) & 0x3ffffff;
    r[5] = ((a[5] >> 1) | (a[6] << 25)) & 0x3ffffff;
    r[6] = ((a[6] >> 1) | (a[7] << 25)) & 0x3ffffff;
    r[7] = ((a[7] >> 1) | (a[8] << 25)) & 0x3ffffff;
    r[8] = ((a[8] >> 1) | (a[9] << 25)) & 0x3ffffff;
#endif
    r[9] = a[9] >> 1;
}

/* Divide the number by 2 mod the modulus (prime). (r = a / 2 % m)
 *
 * r  Result of division by 2.
 * a  Number to divide.
 * m  Modulus (prime).
 */
static void sp_256_div2_10(sp_digit* r, sp_digit* a, sp_digit* m)
{
    sp_256_cond_add_10(r, a, m, 0 - (a[0] & 1));
    sp_256_norm_10(r);
    sp_256_rshift1_10(r, r);
}

/* Double the Montgomery form projective point p.
 *
 * r  Result of doubling point.
 * p  Point to double.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_dbl_10(sp_point* r, sp_point* p, sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*10;
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
        for (i=0; i<10; i++)
            r->x[i] = p->x[i];
        for (i=0; i<10; i++)
            r->y[i] = p->y[i];
        for (i=0; i<10; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* T1 = Z * Z */
    sp_256_mont_sqr_10(t1, z, p256_mod, p256_mp_mod);
    /* Z = Y * Z */
    sp_256_mont_mul_10(z, y, z, p256_mod, p256_mp_mod);
    /* Z = 2Z */
    sp_256_mont_dbl_10(z, z, p256_mod);
    /* T2 = X - T1 */
    sp_256_mont_sub_10(t2, x, t1, p256_mod);
    /* T1 = X + T1 */
    sp_256_mont_add_10(t1, x, t1, p256_mod);
    /* T2 = T1 * T2 */
    sp_256_mont_mul_10(t2, t1, t2, p256_mod, p256_mp_mod);
    /* T1 = 3T2 */
    sp_256_mont_tpl_10(t1, t2, p256_mod);
    /* Y = 2Y */
    sp_256_mont_dbl_10(y, y, p256_mod);
    /* Y = Y * Y */
    sp_256_mont_sqr_10(y, y, p256_mod, p256_mp_mod);
    /* T2 = Y * Y */
    sp_256_mont_sqr_10(t2, y, p256_mod, p256_mp_mod);
    /* T2 = T2/2 */
    sp_256_div2_10(t2, t2, p256_mod);
    /* Y = Y * X */
    sp_256_mont_mul_10(y, y, x, p256_mod, p256_mp_mod);
    /* X = T1 * T1 */
    sp_256_mont_mul_10(x, t1, t1, p256_mod, p256_mp_mod);
    /* X = X - Y */
    sp_256_mont_sub_10(x, x, y, p256_mod);
    /* X = X - Y */
    sp_256_mont_sub_10(x, x, y, p256_mod);
    /* Y = Y - X */
    sp_256_mont_sub_10(y, y, x, p256_mod);
    /* Y = Y * T1 */
    sp_256_mont_mul_10(y, y, t1, p256_mod, p256_mp_mod);
    /* Y = Y - T2 */
    sp_256_mont_sub_10(y, y, t2, p256_mod);

}

/* Compare two numbers to determine if they are equal.
 * Constant time implementation.
 *
 * a  First number to compare.
 * b  Second number to compare.
 * returns 1 when equal and 0 otherwise.
 */
static int sp_256_cmp_equal_10(const sp_digit* a, const sp_digit* b)
{
    return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) | (a[3] ^ b[3]) |
            (a[4] ^ b[4]) | (a[5] ^ b[5]) | (a[6] ^ b[6]) | (a[7] ^ b[7]) |
            (a[8] ^ b[8]) | (a[9] ^ b[9])) == 0;
}

/* Add two Montgomery form projective points.
 *
 * r  Result of addition.
 * p  Frist point to add.
 * q  Second point to add.
 * t  Temporary ordinate data.
 */
static void sp_256_proj_point_add_10(sp_point* r, sp_point* p, sp_point* q,
        sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*10;
    sp_digit* t3 = t + 4*10;
    sp_digit* t4 = t + 6*10;
    sp_digit* t5 = t + 8*10;
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
    sp_256_sub_10(t1, p256_mod, q->y);
    sp_256_norm_10(t1);
    if (sp_256_cmp_equal_10(p->x, q->x) & sp_256_cmp_equal_10(p->z, q->z) &
        (sp_256_cmp_equal_10(p->y, q->y) | sp_256_cmp_equal_10(p->y, t1))) {
        sp_256_proj_point_dbl_10(r, p, t);
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
        for (i=0; i<10; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<10; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<10; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U1 = X1*Z2^2 */
        sp_256_mont_sqr_10(t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t3, t1, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t1, t1, x, p256_mod, p256_mp_mod);
        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_10(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S1 = Y1*Z2^3 */
        sp_256_mont_mul_10(t3, t3, y, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_10(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - U1 */
        sp_256_mont_sub_10(t2, t2, t1, p256_mod);
        /* R = S2 - S1 */
        sp_256_mont_sub_10(t4, t4, t3, p256_mod);
        /* Z3 = H*Z1*Z2 */
        sp_256_mont_mul_10(z, z, q->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*U1*H^2 */
        sp_256_mont_sqr_10(x, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_10(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(y, t1, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_10(x, x, t5, p256_mod);
        sp_256_mont_dbl_10(t1, y, p256_mod);
        sp_256_mont_sub_10(x, x, t1, p256_mod);
        /* Y3 = R*(U1*H^2 - X3) - S1*H^3 */
        sp_256_mont_sub_10(y, y, x, p256_mod);
        sp_256_mont_mul_10(y, y, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t5, t5, t3, p256_mod, p256_mp_mod);
        sp_256_mont_sub_10(y, y, t5, p256_mod);
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
static int sp_256_ecc_mulmod_10(sp_point* r, sp_point* g, sp_digit* k,
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
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 10 * 5, heap,
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
        err = sp_256_mod_mul_norm_10(t[1]->x, g->x, p256_mod);
    }
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t[1]->y, g->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t[1]->z, g->z, p256_mod);

    if (err == MP_OKAY) {
        i = 9;
        c = 22;
        n = k[i--] << (26 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = k[i--];
                c = 26;
            }

            y = (n >> 25) & 1;
            n <<= 1;

            sp_256_proj_point_add_10(t[y^1], t[0], t[1], tmp);

            XMEMCPY(t[2], (void*)(((size_t)t[0] & addr_mask[y^1]) +
                                  ((size_t)t[1] & addr_mask[y])),
                    sizeof(sp_point));
            sp_256_proj_point_dbl_10(t[2], t[2], tmp);
            XMEMCPY((void*)(((size_t)t[0] & addr_mask[y^1]) +
                            ((size_t)t[1] & addr_mask[y])), t[2],
                    sizeof(sp_point));
        }

        if (map)
            sp_256_map_10(r, t[0], tmp);
        else
            XMEMCPY(r, t[0], sizeof(sp_point));
    }

    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 10 * 5);
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
static int sp_256_ecc_mulmod_10(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td[3];
    sp_digit tmpd[2 * 10 * 5];
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
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 10 * 5, heap,
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
        err = sp_256_mod_mul_norm_10(t[1].x, g->x, p256_mod);
    }
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t[1].y, g->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t[1].z, g->z, p256_mod);

    if (err == MP_OKAY) {
        i = 9;
        c = 22;
        n = k[i--] << (26 - c);
        for (; ; c--) {
            if (c == 0) {
                if (i == -1)
                    break;

                n = k[i--];
                c = 26;
            }

            y = (n >> 25) & 1;
            n <<= 1;

            sp_256_proj_point_add_10(&t[y^1], &t[0], &t[1], tmp);

            XMEMCPY(&t[2], (void*)(((size_t)&t[0] & addr_mask[y^1]) +
                                 ((size_t)&t[1] & addr_mask[y])), sizeof(t[2]));
            sp_256_proj_point_dbl_10(&t[2], &t[2], tmp);
            XMEMCPY((void*)(((size_t)&t[0] & addr_mask[y^1]) +
                           ((size_t)&t[1] & addr_mask[y])), &t[2], sizeof(t[2]));
        }

        if (map)
            sp_256_map_10(r, &t[0], tmp);
        else
            XMEMCPY(r, &t[0], sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 10 * 5);
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
    sp_digit x[10];
    sp_digit y[10];
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
static int sp_256_ecc_mulmod_fast_10(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point td[16];
    sp_point rtd;
    sp_digit tmpd[2 * 10 * 5];
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
    tmp = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 10 * 5, heap,
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
        sp_256_mod_mul_norm_10(t[1].x, g->x, p256_mod);
        sp_256_mod_mul_norm_10(t[1].y, g->y, p256_mod);
        sp_256_mod_mul_norm_10(t[1].z, g->z, p256_mod);
        t[1].infinity = 0;
        sp_256_proj_point_dbl_10(&t[ 2], &t[ 1], tmp);
        t[ 2].infinity = 0;
        sp_256_proj_point_add_10(&t[ 3], &t[ 2], &t[ 1], tmp);
        t[ 3].infinity = 0;
        sp_256_proj_point_dbl_10(&t[ 4], &t[ 2], tmp);
        t[ 4].infinity = 0;
        sp_256_proj_point_add_10(&t[ 5], &t[ 3], &t[ 2], tmp);
        t[ 5].infinity = 0;
        sp_256_proj_point_dbl_10(&t[ 6], &t[ 3], tmp);
        t[ 6].infinity = 0;
        sp_256_proj_point_add_10(&t[ 7], &t[ 4], &t[ 3], tmp);
        t[ 7].infinity = 0;
        sp_256_proj_point_dbl_10(&t[ 8], &t[ 4], tmp);
        t[ 8].infinity = 0;
        sp_256_proj_point_add_10(&t[ 9], &t[ 5], &t[ 4], tmp);
        t[ 9].infinity = 0;
        sp_256_proj_point_dbl_10(&t[10], &t[ 5], tmp);
        t[10].infinity = 0;
        sp_256_proj_point_add_10(&t[11], &t[ 6], &t[ 5], tmp);
        t[11].infinity = 0;
        sp_256_proj_point_dbl_10(&t[12], &t[ 6], tmp);
        t[12].infinity = 0;
        sp_256_proj_point_add_10(&t[13], &t[ 7], &t[ 6], tmp);
        t[13].infinity = 0;
        sp_256_proj_point_dbl_10(&t[14], &t[ 7], tmp);
        t[14].infinity = 0;
        sp_256_proj_point_add_10(&t[15], &t[ 8], &t[ 7], tmp);
        t[15].infinity = 0;

        i = 8;
        n = k[i+1] << 6;
        c = 18;
        y = n >> 24;
        XMEMCPY(rt, &t[y], sizeof(sp_point));
        n <<= 8;
        for (; i>=0 || c>=4; ) {
            if (c < 4) {
                n |= k[i--] << (6 - c);
                c += 26;
            }
            y = (n >> 28) & 0xf;
            n <<= 4;
            c -= 4;

            sp_256_proj_point_dbl_10(rt, rt, tmp);
            sp_256_proj_point_dbl_10(rt, rt, tmp);
            sp_256_proj_point_dbl_10(rt, rt, tmp);
            sp_256_proj_point_dbl_10(rt, rt, tmp);

            sp_256_proj_point_add_10(rt, rt, &t[y], tmp);
        }

        if (map)
            sp_256_map_10(r, rt, tmp);
        else
            XMEMCPY(r, rt, sizeof(sp_point));
    }

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (tmp != NULL) {
        XMEMSET(tmp, 0, sizeof(sp_digit) * 2 * 10 * 5);
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
static void sp_256_proj_point_dbl_n_10(sp_point* r, sp_point* p, int n,
        sp_digit* t)
{
    sp_point *rp[2];
    sp_point tp;
    sp_digit* w = t;
    sp_digit* a = t + 2*10;
    sp_digit* b = t + 4*10;
    sp_digit* t1 = t + 6*10;
    sp_digit* t2 = t + 8*10;
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
        for (i=0; i<10; i++)
            r->x[i] = p->x[i];
        for (i=0; i<10; i++)
            r->y[i] = p->y[i];
        for (i=0; i<10; i++)
            r->z[i] = p->z[i];
        r->infinity = p->infinity;
    }

    /* Y = 2*Y */
    sp_256_mont_dbl_10(y, y, p256_mod);
    /* W = Z^4 */
    sp_256_mont_sqr_10(w, z, p256_mod, p256_mp_mod);
    sp_256_mont_sqr_10(w, w, p256_mod, p256_mp_mod);
    while (n--) {
        /* A = 3*(X^2 - W) */
        sp_256_mont_sqr_10(t1, x, p256_mod, p256_mp_mod);
        sp_256_mont_sub_10(t1, t1, w, p256_mod);
        sp_256_mont_tpl_10(a, t1, p256_mod);
        /* B = X*Y^2 */
        sp_256_mont_sqr_10(t2, y, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(b, t2, x, p256_mod, p256_mp_mod);
        /* X = A^2 - 2B */
        sp_256_mont_sqr_10(x, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_10(t1, b, p256_mod);
        sp_256_mont_sub_10(x, x, t1, p256_mod);
        /* Z = Z*Y */
        sp_256_mont_mul_10(z, z, y, p256_mod, p256_mp_mod);
        /* t2 = Y^4 */
        sp_256_mont_sqr_10(t2, t2, p256_mod, p256_mp_mod);
        if (n) {
            /* W = W*Y^4 */
            sp_256_mont_mul_10(w, w, t2, p256_mod, p256_mp_mod);
        }
        /* y = 2*A*(B - X) - Y^4 */
        sp_256_mont_sub_10(y, b, x, p256_mod);
        sp_256_mont_mul_10(y, y, a, p256_mod, p256_mp_mod);
        sp_256_mont_dbl_10(y, y, p256_mod);
        sp_256_mont_sub_10(y, y, t2, p256_mod);
    }
    /* Y = Y/2 */
    sp_256_div2_10(y, y, p256_mod);
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
static void sp_256_proj_point_add_qz1_10(sp_point* r, sp_point* p,
        sp_point* q, sp_digit* t)
{
    sp_point *ap[2];
    sp_point *rp[2];
    sp_point tp;
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2*10;
    sp_digit* t3 = t + 4*10;
    sp_digit* t4 = t + 6*10;
    sp_digit* t5 = t + 8*10;
    sp_digit* x;
    sp_digit* y;
    sp_digit* z;
    int i;

    /* Check double */
    sp_256_sub_10(t1, p256_mod, q->y);
    sp_256_norm_10(t1);
    if (sp_256_cmp_equal_10(p->x, q->x) & sp_256_cmp_equal_10(p->z, q->z) &
        (sp_256_cmp_equal_10(p->y, q->y) | sp_256_cmp_equal_10(p->y, t1))) {
        sp_256_proj_point_dbl_10(r, p, t);
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
        for (i=0; i<10; i++)
            r->x[i] = ap[p->infinity]->x[i];
        for (i=0; i<10; i++)
            r->y[i] = ap[p->infinity]->y[i];
        for (i=0; i<10; i++)
            r->z[i] = ap[p->infinity]->z[i];
        r->infinity = ap[p->infinity]->infinity;

        /* U2 = X2*Z1^2 */
        sp_256_mont_sqr_10(t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t4, t2, z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t2, t2, q->x, p256_mod, p256_mp_mod);
        /* S2 = Y2*Z1^3 */
        sp_256_mont_mul_10(t4, t4, q->y, p256_mod, p256_mp_mod);
        /* H = U2 - X1 */
        sp_256_mont_sub_10(t2, t2, x, p256_mod);
        /* R = S2 - Y1 */
        sp_256_mont_sub_10(t4, t4, y, p256_mod);
        /* Z3 = H*Z1 */
        sp_256_mont_mul_10(z, z, t2, p256_mod, p256_mp_mod);
        /* X3 = R^2 - H^3 - 2*X1*H^2 */
        sp_256_mont_sqr_10(t1, t4, p256_mod, p256_mp_mod);
        sp_256_mont_sqr_10(t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t3, x, t5, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t5, t5, t2, p256_mod, p256_mp_mod);
        sp_256_mont_sub_10(x, t1, t5, p256_mod);
        sp_256_mont_dbl_10(t1, t3, p256_mod);
        sp_256_mont_sub_10(x, x, t1, p256_mod);
        /* Y3 = R*(X1*H^2 - X3) - Y1*H^3 */
        sp_256_mont_sub_10(t3, t3, x, p256_mod);
        sp_256_mont_mul_10(t3, t3, t4, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(t5, t5, y, p256_mod, p256_mp_mod);
        sp_256_mont_sub_10(y, t3, t5, p256_mod);
    }
}

#ifdef FP_ECC
/* Convert the projective point to affine.
 * Ordinates are in Montgomery form.
 *
 * a  Point to convert.
 * t  Temprorary data.
 */
static void sp_256_proj_to_affine_10(sp_point* a, sp_digit* t)
{
    sp_digit* t1 = t;
    sp_digit* t2 = t + 2 * 10;
    sp_digit* tmp = t + 4 * 10;

    sp_256_mont_inv_10(t1, a->z, tmp);

    sp_256_mont_sqr_10(t2, t1, p256_mod, p256_mp_mod);
    sp_256_mont_mul_10(t1, t2, t1, p256_mod, p256_mp_mod);

    sp_256_mont_mul_10(a->x, a->x, t2, p256_mod, p256_mp_mod);
    sp_256_mont_mul_10(a->y, a->y, t1, p256_mod, p256_mp_mod);
    XMEMCPY(a->z, p256_norm_mod, sizeof(p256_norm_mod));
}

/* Generate the pre-computed table of points for the base point.
 *
 * a      The base point.
 * table  Place to store generated point data.
 * tmp    Temprorary data.
 * heap  Heap to use for allocation.
 */
static int sp_256_gen_stripe_table_10(sp_point* a,
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
        err = sp_256_mod_mul_norm_10(t->x, a->x, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t->y, a->y, p256_mod);
    if (err == MP_OKAY)
        err = sp_256_mod_mul_norm_10(t->z, a->z, p256_mod);
    if (err == MP_OKAY) {
        t->infinity = 0;
        sp_256_proj_to_affine_10(t, tmp);

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
            sp_256_proj_point_dbl_n_10(t, t, 32, tmp);
            sp_256_proj_to_affine_10(t, tmp);
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
                sp_256_proj_point_add_qz1_10(t, s1, s2, tmp);
                sp_256_proj_to_affine_10(t, tmp);
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
static int sp_256_ecc_mulmod_stripe_10(sp_point* r, sp_point* g,
        sp_table_entry* table, sp_digit* k, int map, void* heap)
{
#if !defined(WOLFSSL_SP_SMALL) && !defined(WOLFSSL_SMALL_STACK)
    sp_point rtd;
    sp_point pd;
    sp_digit td[2 * 10 * 5];
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
    t = (sp_digit*)XMALLOC(sizeof(sp_digit) * 2 * 10 * 5, heap,
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
            y |= ((k[x / 26] >> (x % 26)) & 1) << j;
        XMEMCPY(rt->x, table[y].x, sizeof(table[y].x));
        XMEMCPY(rt->y, table[y].y, sizeof(table[y].y));
        rt->infinity = table[y].infinity;
        for (i=30; i>=0; i--) {
            y = 0;
            for (j=0,x=i; j<8; j++,x+=32)
                y |= ((k[x / 26] >> (x % 26)) & 1) << j;

            sp_256_proj_point_dbl_10(rt, rt, t);
            XMEMCPY(p->x, table[y].x, sizeof(table[y].x));
            XMEMCPY(p->y, table[y].y, sizeof(table[y].y));
            p->infinity = table[y].infinity;
            sp_256_proj_point_add_qz1_10(rt, rt, p, t);
        }

        if (map)
            sp_256_map_10(r, rt, t);
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
    sp_digit x[10];
    sp_digit y[10];
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

        if (sp_256_cmp_equal_10(g->x, sp_cache[i].x) & 
                           sp_256_cmp_equal_10(g->y, sp_cache[i].y)) {
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
static int sp_256_ecc_mulmod_10(sp_point* r, sp_point* g, sp_digit* k,
        int map, void* heap)
{
#ifndef FP_ECC
    return sp_256_ecc_mulmod_fast_10(r, g, k, map, heap);
#else
    sp_digit tmp[2 * 10 * 5];
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
            sp_256_gen_stripe_table_10(g, cache->table, tmp, heap);

#ifndef HAVE_THREAD_LS
        wc_UnLockMutex(&sp_cache_lock);
#endif /* HAVE_THREAD_LS */

        if (cache->cnt < 2) {
            err = sp_256_ecc_mulmod_fast_10(r, g, k, map, heap);
        }
        else {
            err = sp_256_ecc_mulmod_stripe_10(r, g, cache->table, k,
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
    sp_digit kd[10];
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
        k = XMALLOC(sizeof(sp_digit) * 10, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 10, km);
        sp_256_point_from_ecc_point_10(point, gm);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_10(point, point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_10(point, point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_10(point, r);

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
static int sp_256_ecc_mulmod_base_10(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    /* No pre-computed values. */
    return sp_256_ecc_mulmod_10(r, &p256_base, k, map, heap);
}

#else
static sp_table_entry p256_table[256] = {
    /* 0 */
    { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      1 },
    /* 1 */
    { { 0x0a9143c,0x1cc3506,0x360179e,0x3f17fb6,0x075ba95,0x1d88944,
        0x3b732b7,0x15719e7,0x376a537,0x0062417 },
      { 0x295560a,0x094d5f3,0x245cddf,0x392e867,0x18b4ab8,0x3487cc9,
        0x288688d,0x176174b,0x3182588,0x0215c7f },
      0 },
    /* 2 */
    { { 0x147519a,0x2218090,0x32f0202,0x2b09acd,0x0d0981e,0x1e17af2,
        0x14a7caa,0x163a6a7,0x10ddbdf,0x03654f1 },
      { 0x1590f8f,0x0d8733f,0x09179d6,0x1ad139b,0x372e962,0x0bad933,
        0x1961102,0x223cdff,0x37e9eb2,0x0218fae },
      0 },
    /* 3 */
    { { 0x0db6485,0x1ad88d7,0x2f97785,0x288bc28,0x3808f0e,0x3df8c02,
        0x28d9544,0x20280f9,0x055b5ff,0x00001d8 },
      { 0x38d2010,0x13ae6e0,0x308a763,0x2ecc90d,0x254014f,0x10a9981,
        0x247d398,0x0fb8383,0x3613437,0x020c21d },
      0 },
    /* 4 */
    { { 0x2a0d2bb,0x08bf145,0x34994f9,0x1b06988,0x30d5cc1,0x1f18b22,
        0x01cf3a5,0x199fe49,0x161fd1b,0x00bd79a },
      { 0x1a01797,0x171c2fd,0x21925c1,0x1358255,0x23d20b4,0x1c7f6d4,
        0x111b370,0x03dec12,0x1168d6f,0x03d923e },
      0 },
    /* 5 */
    { { 0x137bbbc,0x19a11f8,0x0bec9e5,0x27a29a8,0x3e43446,0x275cd18,
        0x0427617,0x00056c7,0x285133d,0x016af80 },
      { 0x04c7dab,0x2a0df30,0x0c0792a,0x1310c98,0x3573d9f,0x239b30d,
        0x1315627,0x1ce0c32,0x25b6b6f,0x0252edc },
      0 },
    /* 6 */
    { { 0x20f141c,0x26d23dc,0x3c74bbf,0x334b7d6,0x06199b3,0x0441171,
        0x3f61294,0x313bf70,0x3cb2f7d,0x03375ae },
      { 0x2f436fd,0x19c02fa,0x26becca,0x1b6e64c,0x26f647f,0x053c948,
        0x0fa7920,0x397d830,0x2bd4bda,0x028d86f },
      0 },
    /* 7 */
    { { 0x17c13c7,0x2895616,0x03e128a,0x17d42df,0x1c38d63,0x0f02747,
        0x039aecf,0x0a4b01c,0x209c4b5,0x02e84b2 },
      { 0x1f91dfd,0x023e916,0x07fb9e4,0x19b3ba8,0x13af43b,0x35e02ca,
        0x0eb0899,0x3bd2c7b,0x19d701f,0x014faee },
      0 },
    /* 8 */
    { { 0x0e63d34,0x1fb8c6c,0x0fab4fe,0x1caa795,0x0f46005,0x179ed69,
        0x093334d,0x120c701,0x39206d5,0x021627e },
      { 0x183553a,0x03d7319,0x09e5aa7,0x12b8959,0x2087909,0x0011194,
        0x1045071,0x0713f32,0x16d0254,0x03aec1a },
      0 },
    /* 9 */
    { { 0x01647c5,0x1b2856b,0x1799461,0x11f133d,0x0b8127d,0x1937eeb,
        0x266aa37,0x1f68f71,0x0cbd1b2,0x03aca08 },
      { 0x287e008,0x1be361a,0x38f3940,0x276488d,0x2d87dfa,0x0333b2c,
        0x2d2e428,0x368755b,0x09b55a7,0x007ca0a },
      0 },
    /* 10 */
    { { 0x389da99,0x2a8300e,0x0022abb,0x27ae0a1,0x0a6f2d7,0x207017a,
        0x047862b,0x1358c9e,0x35905e5,0x00cde92 },
      { 0x1f7794a,0x1d40348,0x3f613c6,0x2ddf5b5,0x0207005,0x133f5ba,
        0x1a37810,0x3ef5829,0x0d5f4c2,0x0035978 },
      0 },
    /* 11 */
    { { 0x1275d38,0x026efad,0x2358d9d,0x1142f82,0x14268a7,0x1cfac99,
        0x362ff49,0x288cbc1,0x24252f4,0x0308f68 },
      { 0x394520c,0x06e13c2,0x178e5da,0x18ec16f,0x1096667,0x134a7a8,
        0x0dcb869,0x33fc4e9,0x38cc790,0x006778e },
      0 },
    /* 12 */
    { { 0x2c5fe04,0x29c5b09,0x1bdb183,0x02ceee8,0x03b28de,0x132dc4b,
        0x32c586a,0x32ff5d0,0x3d491fc,0x038d372 },
      { 0x2a58403,0x2351aea,0x3a53b40,0x21a0ba5,0x39a6974,0x1aaaa2b,
        0x3901273,0x03dfe78,0x3447b4e,0x039d907 },
      0 },
    /* 13 */
    { { 0x364ba59,0x14e5077,0x02fc7d7,0x3b02c09,0x1d33f10,0x0560616,
        0x06dfc6a,0x15efd3c,0x357052a,0x01284b7 },
      { 0x039dbd0,0x18ce3e5,0x3e1fbfa,0x352f794,0x0d3c24b,0x07c6cc5,
        0x1e4ffa2,0x3a91bf5,0x293bb5b,0x01abd6a },
      0 },
    /* 14 */
    { { 0x0c91999,0x02da644,0x0491da1,0x100a960,0x00a24b4,0x2330824,
        0x0094b4b,0x1004cf8,0x35a66a4,0x017f8d1 },
      { 0x13e7b4b,0x232af7e,0x391ab0f,0x069f08f,0x3292b50,0x3479898,
        0x2889aec,0x2a4590b,0x308ecfe,0x02d5138 },
      0 },
    /* 15 */
    { { 0x2ddfdce,0x231ba45,0x39e6647,0x19be245,0x12c3291,0x35399f8,
        0x0d6e764,0x3082d3a,0x2bda6b0,0x0382dac },
      { 0x37efb57,0x04b7cae,0x00070d3,0x379e431,0x01aac0d,0x1e6f251,
        0x0336ad6,0x0ddd3e4,0x3de25a6,0x01c7008 },
      0 },
    /* 16 */
    { { 0x3e20925,0x230912f,0x286762a,0x30e3f73,0x391c19a,0x34e1c18,
        0x16a5d5d,0x093d96a,0x3d421d3,0x0187561 },
      { 0x37173ea,0x19ce8a8,0x0b65e87,0x0214dde,0x2238480,0x16ead0f,
        0x38441e0,0x3bef843,0x2124621,0x03e847f },
      0 },
    /* 17 */
    { { 0x0b19ffd,0x247cacb,0x3c231c8,0x16ec648,0x201ba8d,0x2b172a3,
        0x103d678,0x2fb72db,0x04c1f13,0x0161bac },
      { 0x3e8ed09,0x171b949,0x2de20c3,0x0f06067,0x21e81a3,0x1b194be,
        0x0fd6c05,0x13c449e,0x0087086,0x006756b },
      0 },
    /* 18 */
    { { 0x09a4e1f,0x27d604c,0x00741e9,0x06fa49c,0x0ab7de7,0x3f4a348,
        0x25ef0be,0x158fc9a,0x33f7f9c,0x039f001 },
      { 0x2f59f76,0x3598e83,0x30501f6,0x15083f2,0x0669b3b,0x29980b5,
        0x0c1f7a7,0x0f02b02,0x0fec65b,0x0382141 },
      0 },
    /* 19 */
    { { 0x031b3ca,0x23da368,0x2d66f09,0x27b9b69,0x06d1cab,0x13c91ba,
        0x3d81fa9,0x25ad16f,0x0825b09,0x01e3c06 },
      { 0x225787f,0x3bf790e,0x2c9bb7e,0x0347732,0x28016f8,0x0d6ff0d,
        0x2a4877b,0x1d1e833,0x3b87e94,0x010e9dc },
      0 },
    /* 20 */
    { { 0x2b533d5,0x1ddcd34,0x1dc0625,0x3da86f7,0x3673b8a,0x1e7b0a4,
        0x3e7c9aa,0x19ac55d,0x251c3b2,0x02edb79 },
      { 0x25259b3,0x24c0ead,0x3480e7e,0x34f40e9,0x3d6a0af,0x2cf3f09,
        0x2c83d19,0x2e66f16,0x19a5d18,0x0182d18 },
      0 },
    /* 21 */
    { { 0x2e5aa1c,0x28e3846,0x3658bd6,0x0ad279c,0x1b8b765,0x397e1fb,
        0x130014e,0x3ff342c,0x3b2aeeb,0x02743c9 },
      { 0x2730a55,0x0918c5e,0x083aca9,0x0bf76ef,0x19c955b,0x300669c,
        0x01dfe0a,0x312341f,0x26d356e,0x0091295 },
      0 },
    /* 22 */
    { { 0x2cf1f96,0x00e52ba,0x271c6db,0x2a40930,0x19f2122,0x0b2f4ee,
        0x26ac1b8,0x3bda498,0x0873581,0x0117963 },
      { 0x38f9dbc,0x3d1e768,0x2040d3f,0x11ba222,0x3a8aaf1,0x1b82fb5,
        0x1adfb24,0x2de9251,0x21cc1e4,0x0301038 },
      0 },
    /* 23 */
    { { 0x38117b6,0x2bc001b,0x1433847,0x3fdce8d,0x3651969,0x3651d7a,
        0x2b35761,0x1bb1d20,0x097682c,0x00737d7 },
      { 0x1f04839,0x1dd6d04,0x16987db,0x3d12378,0x17dbeac,0x1c2cc86,
        0x121dd1b,0x3fcf6ca,0x1f8a92d,0x00119d5 },
      0 },
    /* 24 */
    { { 0x0e8ffcd,0x2b174af,0x1a82cc8,0x22cbf98,0x30d53c4,0x080b5b1,
        0x3161727,0x297cfdb,0x2113b83,0x0011b97 },
      { 0x0007f01,0x23fd936,0x3183e7b,0x0496bd0,0x07fb1ef,0x178680f,
        0x1c5ea63,0x0016c11,0x2c3303d,0x01b8041 },
      0 },
    /* 25 */
    { { 0x0dd73b1,0x1cd6122,0x10d948c,0x23e657b,0x3767070,0x15a8aad,
        0x385ea8c,0x33c7ce0,0x0ede901,0x0110965 },
      { 0x2d4b65b,0x2a8b244,0x0c37f8f,0x0ee5b24,0x394c234,0x3a5e347,
        0x26e4a15,0x39a3b4c,0x2514c2e,0x029e5be },
      0 },
    /* 26 */
    { { 0x23addd7,0x3ed8120,0x13b3359,0x20f959a,0x09e2a61,0x32fcf20,
        0x05b78e3,0x19ba7e2,0x1a9c697,0x0392b4b },
      { 0x2048a61,0x3dfd0a3,0x19a0357,0x233024b,0x3082d19,0x00fb63b,
        0x3a1af4c,0x1450ff0,0x046c37b,0x0317a50 },
      0 },
    /* 27 */
    { { 0x3e75f9e,0x294e30a,0x3a78476,0x3a32c48,0x36fd1a9,0x0427012,
        0x1e4df0b,0x11d1f61,0x1afdb46,0x018ca0f },
      { 0x2f2df15,0x0a33dee,0x27f4ce7,0x1542b66,0x3e592c4,0x20d2f30,
        0x3226ade,0x2a4e3ea,0x1ab1981,0x01a2f46 },
      0 },
    /* 28 */
    { { 0x087d659,0x3ab5446,0x305ac08,0x3d2cd64,0x33374d5,0x3f9d3f8,
        0x186981c,0x37f5a5a,0x2f53c6f,0x01254a4 },
      { 0x2cec896,0x1e32786,0x04844a8,0x043b16d,0x3d964b2,0x1935829,
        0x16f7e26,0x1a0dd9a,0x30d2603,0x003b1d4 },
      0 },
    /* 29 */
    { { 0x12687bb,0x04e816b,0x21fa2da,0x1abccb8,0x3a1f83b,0x375181e,
        0x0f5ef51,0x0fc2ce4,0x3a66486,0x003d881 },
      { 0x3138233,0x1f8eec3,0x2718bd6,0x1b09caa,0x2dd66b9,0x1bb222b,
        0x1004072,0x1b73e3b,0x07208ed,0x03fc36c },
      0 },
    /* 30 */
    { { 0x095d553,0x3e84053,0x0a8a749,0x3f575a0,0x3a44052,0x3ced59b,
        0x3b4317f,0x03a8c60,0x13c8874,0x00c4ed4 },
      { 0x0d11549,0x0b8ab02,0x221cb40,0x02ed37b,0x2071ee1,0x1fc8c83,
        0x3987dd4,0x27e049a,0x0f986f1,0x00b4eaf },
      0 },
    /* 31 */
    { { 0x15581a2,0x2214060,0x11af4c2,0x1598c88,0x19a0a6d,0x32acba6,
        0x3a7a0f0,0x2337c66,0x210ded9,0x0300dbe },
      { 0x1fbd009,0x3822eb0,0x181629a,0x2401b45,0x30b68b1,0x2e78363,
        0x2b32779,0x006530b,0x2c4b6d4,0x029aca8 },
      0 },
    /* 32 */
    { { 0x13549cf,0x0f943db,0x265ed43,0x1bfeb35,0x06f3369,0x3847f2d,
        0x1bfdacc,0x26181a5,0x252af7c,0x02043b8 },
      { 0x159bb2c,0x143f85c,0x357b654,0x2f9d62c,0x2f7dfbe,0x1a7fa9c,
        0x057e74d,0x05d14ac,0x17a9273,0x035215c },
      0 },
    /* 33 */
    { { 0x0cb5a98,0x106a2bc,0x10bf117,0x24c7cc4,0x3d3da8f,0x2ce0ab7,
        0x14e2cba,0x1813866,0x1a72f9a,0x01a9811 },
      { 0x2b2411d,0x3034fe8,0x16e0170,0x0f9443a,0x0be0eb8,0x2196cf3,
        0x0c9f738,0x15e40ef,0x0faf9e1,0x034f917 },
      0 },
    /* 34 */
    { { 0x03f7669,0x3da6efa,0x3d6bce1,0x209ca1d,0x109f8ae,0x09109e3,
        0x08ae543,0x3067255,0x1dee3c2,0x0081dd5 },
      { 0x3ef1945,0x358765b,0x28c387b,0x3bec4b4,0x218813c,0x0b7d92a,
        0x3cd1d67,0x2c0367e,0x2e57154,0x0123717 },
      0 },
    /* 35 */
    { { 0x3e5a199,0x1e42ffd,0x0bb7123,0x33e6273,0x1e0efb8,0x294671e,
        0x3a2bfe0,0x3d11709,0x2eddff6,0x03cbec2 },
      { 0x0b5025f,0x0255d7c,0x1f2241c,0x35d03ea,0x0550543,0x202fef4,
        0x23c8ad3,0x354963e,0x015db28,0x0284fa4 },
      0 },
    /* 36 */
    { { 0x2b65cbc,0x1e8d428,0x0226f9f,0x1c8a919,0x10b04b9,0x08fc1e8,
        0x1ce241e,0x149bc99,0x2b01497,0x00afc35 },
      { 0x3216fb7,0x1374fd2,0x226ad3d,0x19fef76,0x0f7d7b8,0x1c21417,
        0x37b83f6,0x3a27eba,0x25a162f,0x010aa52 },
      0 },
    /* 37 */
    { { 0x2adf191,0x1ab42fa,0x28d7584,0x2409689,0x20f8a48,0x253707d,
        0x2030504,0x378f7a1,0x169c65e,0x00b0b76 },
      { 0x3849c17,0x085c764,0x10dd6d0,0x2e87689,0x1460488,0x30e9521,
        0x10c7063,0x1b6f120,0x21f42c5,0x03d0dfe },
      0 },
    /* 38 */
    { { 0x20f7dab,0x035c512,0x29ac6aa,0x24c5ddb,0x20f0497,0x17ce5e1,
        0x00a050f,0x1eaa14b,0x3335470,0x02abd16 },
      { 0x18d364a,0x0df0cf0,0x316585e,0x018f925,0x0d40b9b,0x17b1511,
        0x1716811,0x1caf3d0,0x10df4f2,0x0337d8c },
      0 },
    /* 39 */
    { { 0x2a8b7ef,0x0f188e3,0x2287747,0x06216f0,0x008e935,0x2f6a38d,
        0x1567722,0x0bfc906,0x0bada9e,0x03c3402 },
      { 0x014d3b1,0x099c749,0x2a76291,0x216c067,0x3b37549,0x14ef2f6,
        0x21b96d4,0x1ee2d71,0x2f5ca88,0x016f570 },
      0 },
    /* 40 */
    { { 0x09a3154,0x3d1a7bd,0x2e9aef0,0x255b8ac,0x03e85a5,0x2a492a7,
        0x2aec1ea,0x11c6516,0x3c8a09e,0x02a84b7 },
      { 0x1f69f1d,0x09c89d3,0x1e7326f,0x0b28bfd,0x0e0e4c8,0x1ea7751,
        0x18ce73b,0x2a406e7,0x273e48c,0x01b00db },
      0 },
    /* 41 */
    { { 0x36e3138,0x2b84a83,0x345a5cf,0x00096b4,0x16966ef,0x159caf1,
        0x13c64b4,0x2f89226,0x25896af,0x00a4bfd },
      { 0x2213402,0x1435117,0x09fed52,0x09d0e4b,0x0f6580e,0x2871cba,
        0x3b397fd,0x1c9d825,0x090311b,0x0191383 },
      0 },
    /* 42 */
    { { 0x07153f0,0x1087869,0x18c9e1e,0x1e64810,0x2b86c3b,0x0175d9c,
        0x3dce877,0x269de4e,0x393cab7,0x03c96b9 },
      { 0x1869d0c,0x06528db,0x02641f3,0x209261b,0x29d55c8,0x25ba517,
        0x3b5ea30,0x028f927,0x25313db,0x00e6e39 },
      0 },
    /* 43 */
    { { 0x2fd2e59,0x150802d,0x098f377,0x19a4957,0x135e2c0,0x38a95ce,
        0x1ab21a0,0x36c1b67,0x32f0f19,0x00e448b },
      { 0x3cad53c,0x3387800,0x17e3cfb,0x03f9970,0x3225b2c,0x2a84e1d,
        0x3af1d29,0x3fe35ca,0x2f8ce80,0x0237a02 },
      0 },
    /* 44 */
    { { 0x07bbb76,0x3aa3648,0x2758afb,0x1f085e0,0x1921c7e,0x3010dac,
        0x22b74b1,0x230137e,0x1062e36,0x021c652 },
      { 0x3993df5,0x24a2ee8,0x126ab5f,0x2d7cecf,0x0639d75,0x16d5414,
        0x1aa78a8,0x3f78404,0x26a5b74,0x03f0c57 },
      0 },
    /* 45 */
    { { 0x0d6ecfa,0x3f506ba,0x3f86561,0x3d86bb1,0x15f8c44,0x2491d07,
        0x052a7b4,0x2422261,0x3adee38,0x039b529 },
      { 0x193c75d,0x14bb451,0x1162605,0x293749c,0x370a70d,0x2e8b1f6,
        0x2ede937,0x2b95f4a,0x39a9be2,0x00d77eb },
      0 },
    /* 46 */
    { { 0x2736636,0x15bf36a,0x2b7e6b9,0x25eb8b2,0x209f51d,0x3cd2659,
        0x10bf410,0x034afec,0x3d71c83,0x0076971 },
      { 0x0ce6825,0x07920cf,0x3c3b5c4,0x23fe55c,0x015ad11,0x08c0dae,
        0x0552c7f,0x2e75a8a,0x0fddbf4,0x01c1df0 },
      0 },
    /* 47 */
    { { 0x2b9661c,0x0ffe351,0x3d71bf6,0x1ac34b3,0x3a1dfd3,0x211fe3d,
        0x33e140a,0x3f9100d,0x32ee50e,0x014ea18 },
      { 0x16d8051,0x1bfda1a,0x068a097,0x2571d3d,0x1daec0c,0x39389af,
        0x194dc35,0x3f3058a,0x36d34e1,0x000a329 },
      0 },
    /* 48 */
    { { 0x09877ee,0x351f73f,0x0002d11,0x0420074,0x2c8b362,0x130982d,
        0x02c1175,0x3c11b40,0x0d86962,0x001305f },
      { 0x0daddf5,0x2f4252c,0x15c06d9,0x1d49339,0x1bea235,0x0b680ed,
        0x3356e67,0x1d1d198,0x1e9fed9,0x03dee93 },
      0 },
    /* 49 */
    { { 0x3e1263f,0x2fe8d3a,0x3ce6d0d,0x0d5c6b9,0x3557637,0x0a9bd48,
        0x0405538,0x0710749,0x2005213,0x038c7e5 },
      { 0x26b6ec6,0x2e485ba,0x3c44d1b,0x0b9cf0b,0x037a1d1,0x27428a5,
        0x0e7eac8,0x351ef04,0x259ce34,0x02a8e98 },
      0 },
    /* 50 */
    { { 0x2f3dcd3,0x3e77d4d,0x3360fbc,0x1434afd,0x36ceded,0x3d413d6,
        0x1710fad,0x36bb924,0x1627e79,0x008e637 },
      { 0x109569e,0x1c168db,0x3769cf4,0x2ed4527,0x0ea0619,0x17d80d3,
        0x1c03773,0x18843fe,0x1b21c04,0x015c5fd },
      0 },
    /* 51 */
    { { 0x1dd895e,0x08a7248,0x04519fe,0x001030a,0x18e5185,0x358dfb3,
        0x13d2391,0x0a37be8,0x0560e3c,0x019828b },
      { 0x27fcbd0,0x2a22bb5,0x30969cc,0x1e03aa7,0x1c84724,0x0ba4ad3,
        0x32f4817,0x0914cca,0x14c4f52,0x01893b9 },
      0 },
    /* 52 */
    { { 0x097eccc,0x1273936,0x00aa095,0x364fe62,0x04d49d1,0x10e9f08,
        0x3c24230,0x3ef01c8,0x2fb92bd,0x013ce4a },
      { 0x1e44fd9,0x27e3e9f,0x2156696,0x3915ecc,0x0b66cfb,0x1a3af0f,
        0x2fa8033,0x0e6736c,0x177ccdb,0x0228f9e },
      0 },
    /* 53 */
    { { 0x2c4b125,0x06207c1,0x0a8cdde,0x003db8f,0x1ae34e3,0x31e84fa,
        0x2999de5,0x11013bd,0x02370c2,0x00e2234 },
      { 0x0f91081,0x200d591,0x1504762,0x1857c05,0x23d9fcf,0x0cb34db,
        0x27edc86,0x08cd860,0x2471810,0x029798b },
      0 },
    /* 54 */
    { { 0x3acd6c8,0x097b8cb,0x3c661a8,0x15152f2,0x1699c63,0x237e64c,
        0x23edf79,0x16b7033,0x0e6466a,0x00b11da },
      { 0x0a64bc9,0x1bfe324,0x1f5cb34,0x08391de,0x0630a60,0x3017a21,
        0x09d064b,0x14a8365,0x041f9e6,0x01ed799 },
      0 },
    /* 55 */
    { { 0x128444a,0x2508b07,0x2a39216,0x362f84d,0x2e996c5,0x2c31ff3,
        0x07afe5f,0x1d1288e,0x3cb0c8d,0x02e2bdc },
      { 0x38b86fd,0x3a0ea8c,0x1cff5fd,0x1629629,0x3fee3f1,0x02b250c,
        0x2e8f6f2,0x0225727,0x15f7f3f,0x0280d8e },
      0 },
    /* 56 */
    { { 0x10f7770,0x0f1aee8,0x0e248c7,0x20684a8,0x3a6f16d,0x06f0ae7,
        0x0df6825,0x2d4cc40,0x301875f,0x012f8da },
      { 0x3b56dbb,0x1821ba7,0x24f8922,0x22c1f9e,0x0306fef,0x1b54bc8,
        0x2ccc056,0x00303ba,0x2871bdc,0x0232f26 },
      0 },
    /* 57 */
    { { 0x0dac4ab,0x0625730,0x3112e13,0x101c4bf,0x3a874a4,0x2873b95,
        0x32ae7c6,0x0d7e18c,0x13e0c08,0x01139d5 },
      { 0x334002d,0x00fffdd,0x025c6d5,0x22c2cd1,0x19d35cb,0x3a1ce2d,
        0x3702760,0x3f06257,0x03a5eb8,0x011c29a },
      0 },
    /* 58 */
    { { 0x0513482,0x1d87724,0x276a81b,0x0a807a4,0x3028720,0x339cc20,
        0x2441ee0,0x31bbf36,0x290c63d,0x0059041 },
      { 0x106a2ed,0x0d2819b,0x100bf50,0x114626c,0x1dd4d77,0x2e08632,
        0x14ae72a,0x2ed3f64,0x1fd7abc,0x035cd1e },
      0 },
    /* 59 */
    { { 0x2d4c6e5,0x3bec596,0x104d7ed,0x23d6c1b,0x0262cf0,0x15d72c5,
        0x2d5bb18,0x199ac4b,0x1e30771,0x020591a },
      { 0x21e291e,0x2e75e55,0x1661d7a,0x08b0778,0x3eb9daf,0x0d78144,
        0x1827eb1,0x0fe73d2,0x123f0dd,0x0028db7 },
      0 },
    /* 60 */
    { { 0x1d5533c,0x34cb1d0,0x228f098,0x27a1a11,0x17c5f5a,0x0d26f44,
        0x2228ade,0x2c460e6,0x3d6fdba,0x038cc77 },
      { 0x3cc6ed8,0x02ada1a,0x260e510,0x2f7bde8,0x37160c3,0x33a1435,
        0x23d9a7b,0x0ce2641,0x02a492e,0x034ed1e },
      0 },
    /* 61 */
    { { 0x3821f90,0x26dba3c,0x3aada14,0x3b59bad,0x292edd9,0x2804c45,
        0x3669531,0x296f42e,0x35a4c86,0x01ca049 },
      { 0x3ff47e5,0x2163df4,0x2441503,0x2f18405,0x15e1616,0x37f66ec,
        0x30f11a7,0x141658a,0x27ece14,0x00b018b },
      0 },
    /* 62 */
    { { 0x159ac2e,0x3e65bc0,0x2713a76,0x0db2f6c,0x3281e77,0x2391811,
        0x16d2880,0x1fbc4ab,0x1f92c4e,0x00a0a8d },
      { 0x0ce5cd2,0x152c7b0,0x02299c3,0x3244de7,0x2cf99ef,0x3a0b047,
        0x2caf383,0x0aaf664,0x113554d,0x031c735 },
      0 },
    /* 63 */
    { { 0x1b578f4,0x177a702,0x3a7a488,0x1638ebf,0x31884e2,0x2460bc7,
        0x36b1b75,0x3ce8e3d,0x340cf47,0x03143d9 },
      { 0x34b68ea,0x12b7ccd,0x1fe2a9c,0x08da659,0x0a406f3,0x1694c14,
        0x06a2228,0x16370be,0x3a72129,0x02e7b2c },
      0 },
    /* 64 */
    { { 0x0f8b16a,0x21043bd,0x266a56f,0x3fb11ec,0x197241a,0x36721f0,
        0x006b8e6,0x2ac6c29,0x202cd42,0x0200fcf },
      { 0x0dbec69,0x0c26a01,0x105f7f0,0x3dceeeb,0x3a83b85,0x363865f,
        0x097273a,0x2b70718,0x00e5067,0x03025d1 },
      0 },
    /* 65 */
    { { 0x379ab34,0x295bcb0,0x38d1846,0x22e1077,0x3a8ee06,0x1db1a3b,
        0x3144591,0x07cc080,0x2d5915f,0x03c6bcc },
      { 0x175bd50,0x0dd4c57,0x27bc99c,0x2ebdcbd,0x3837cff,0x235dc8f,
        0x13a4184,0x0722c18,0x130e2d4,0x008f43c },
      0 },
    /* 66 */
    { { 0x01500d9,0x2adbb7d,0x2da8857,0x397f2fa,0x10d890a,0x25c9654,
        0x3e86488,0x3eb754b,0x1d6c0a3,0x02c0a23 },
      { 0x10bcb08,0x083cc19,0x2e16853,0x04da575,0x271af63,0x2626a9d,
        0x3520a7b,0x32348c7,0x24ff408,0x03ff4dc },
      0 },
    /* 67 */
    { { 0x058e6cb,0x1a3992d,0x1d28539,0x080c5e9,0x2992dad,0x2a9d7d5,
        0x14ae0b7,0x09b7ce0,0x34ad78c,0x03d5643 },
      { 0x30ba55a,0x092f4f3,0x0bae0fc,0x12831de,0x20fc472,0x20ed9d2,
        0x29864f6,0x1288073,0x254f6f7,0x00635b6 },
      0 },
    /* 68 */
    { { 0x1be5a2b,0x0f88975,0x33c6ed9,0x20d64d3,0x06fe799,0x0989bff,
        0x1409262,0x085a90c,0x0d97990,0x0142eed },
      { 0x17ec63e,0x06471b9,0x0db2378,0x1006077,0x265422c,0x08db83d,
        0x28099b0,0x1270d06,0x11801fe,0x00ac400 },
      0 },
    /* 69 */
    { { 0x3391593,0x22d7166,0x30fcfc6,0x2896609,0x3c385f5,0x066b72e,
        0x04f3aad,0x2b831c5,0x19983fb,0x0375562 },
      { 0x0b82ff4,0x222e39d,0x34c993b,0x101c79c,0x2d2e03c,0x0f00c8a,
        0x3a9eaf4,0x1810669,0x151149d,0x039b931 },
      0 },
    /* 70 */
    { { 0x29af288,0x1956ec7,0x293155f,0x193deb6,0x1647e1a,0x2ca0839,
        0x297e4bc,0x15bfd0d,0x1b107ed,0x0147803 },
      { 0x31c327e,0x05a6e1d,0x02ad43d,0x02d2a5b,0x129cdb2,0x37ad1de,
        0x3d51f53,0x245df01,0x2414982,0x0388bd0 },
      0 },
    /* 71 */
    { { 0x35f1abb,0x17a3d18,0x0874cd4,0x2d5a14e,0x17edc0c,0x16a00d3,
        0x072c1fb,0x1232725,0x33d52dc,0x03dc24d },
      { 0x0af30d6,0x259aeea,0x369c401,0x12bc4de,0x295bf5f,0x0d8711f,
        0x26162a9,0x16c44e5,0x288e727,0x02f54b4 },
      0 },
    /* 72 */
    { { 0x05fa877,0x1571ea7,0x3d48ab1,0x1c9f4e8,0x017dad6,0x0f46276,
        0x343f9e7,0x1de990f,0x0e4c8aa,0x028343e },
      { 0x094f92d,0x3abf633,0x1b3a0bb,0x2f83137,0x0d818c8,0x20bae85,
        0x0c65f8b,0x1a8008b,0x0c7946d,0x0295b1e },
      0 },
    /* 73 */
    { { 0x1d09529,0x08e46c3,0x1fcf296,0x298f6b7,0x1803e0e,0x2d6fd20,
        0x37351f5,0x0d9e8b1,0x1f8731a,0x0362fbf },
      { 0x00157f4,0x06750bf,0x2650ab9,0x35ffb23,0x2f51cae,0x0b522c2,
        0x39cb400,0x191e337,0x0a5ce9f,0x021529a },
      0 },
    /* 74 */
    { { 0x3506ea5,0x17d9ed8,0x0d66dc3,0x22693f8,0x19286c4,0x3a57353,
        0x101d3bf,0x1aa54fc,0x20b9884,0x0172b3a },
      { 0x0eac44d,0x37d8327,0x1c3aa90,0x3d0d534,0x23db29a,0x3576eaf,
        0x1d3de8a,0x3bea423,0x11235e4,0x039260b },
      0 },
    /* 75 */
    { { 0x34cd55e,0x01288b0,0x1132231,0x2cc9a03,0x358695b,0x3e87650,
        0x345afa1,0x01267ec,0x3f616b2,0x02011ad },
      { 0x0e7d098,0x0d6078e,0x0b70b53,0x237d1bc,0x0d7f61e,0x132de31,
        0x1ea9ea4,0x2bd54c3,0x27b9082,0x03ac5f2 },
      0 },
    /* 76 */
    { { 0x2a145b9,0x06d661d,0x31ec175,0x03f06f1,0x3a5cf6b,0x249c56e,
        0x2035653,0x384c74f,0x0bafab5,0x0025ec0 },
      { 0x25f69e1,0x1b23a55,0x1199aa6,0x16ad6f9,0x077e8f7,0x293f661,
        0x33ba11d,0x3327980,0x07bafdb,0x03e571d },
      0 },
    /* 77 */
    { { 0x2bae45e,0x3c074ef,0x2955558,0x3c312f1,0x2a8ebe9,0x2f193f1,
        0x3705b1d,0x360deba,0x01e566e,0x00d4498 },
      { 0x21161cd,0x1bc787e,0x2f87933,0x3553197,0x1328ab8,0x093c879,
        0x17eee27,0x2adad1d,0x1236068,0x003be5c },
      0 },
    /* 78 */
    { { 0x0ca4226,0x2633dd5,0x2c8e025,0x0e3e190,0x05eede1,0x1a385e4,
        0x163f744,0x2f25522,0x1333b4f,0x03f05b6 },
      { 0x3c800ca,0x1becc79,0x2daabe9,0x0c499e2,0x1138063,0x3fcfa2d,
        0x2244976,0x1e85cf5,0x2f1b95d,0x0053292 },
      0 },
    /* 79 */
    { { 0x12f81d5,0x1dc6eaf,0x11967a4,0x1a407df,0x31a5f9d,0x2b67241,
        0x18bef7c,0x08c7762,0x063f59c,0x01015ec },
      { 0x1c05c0a,0x360bfa2,0x1f85bff,0x1bc7703,0x3e4911c,0x0d685b6,
        0x2fccaea,0x02c4cef,0x164f133,0x0070ed7 },
      0 },
    /* 80 */
    { { 0x0ec21fe,0x052ffa0,0x3e825fe,0x1ab0956,0x3f6ce11,0x3d29759,
        0x3c5a072,0x18ebe62,0x148db7e,0x03eb49c },
      { 0x1ab05b3,0x02dab0a,0x1ae690c,0x0f13894,0x137a9a8,0x0aab79f,
        0x3dc875c,0x06a1029,0x1e39f0e,0x01dce1f },
      0 },
    /* 81 */
    { { 0x16c0dd7,0x3b31269,0x2c741e9,0x3611821,0x2a5cffc,0x1416bb3,
        0x3a1408f,0x311fa3d,0x1c0bef0,0x02cdee1 },
      { 0x00e6a8f,0x1adb933,0x0f23359,0x2fdace2,0x2fd6d4b,0x0e73bd3,
        0x2453fac,0x0a356ae,0x2c8f9f6,0x02704d6 },
      0 },
    /* 82 */
    { { 0x0e35743,0x28c80a1,0x0def32a,0x2c6168f,0x1320d6a,0x37c6606,
        0x21b1761,0x2147ee0,0x21fc433,0x015c84d },
      { 0x1fc9168,0x36cda9c,0x003c1f0,0x1cd7971,0x15f98ba,0x1ef363d,
        0x0ca87e3,0x046f7d9,0x3c9e6bb,0x0372eb0 },
      0 },
    /* 83 */
    { { 0x118cbe2,0x3665a11,0x304ef01,0x062727a,0x3d242fc,0x11ffbaf,
        0x3663c7e,0x1a189c9,0x09e2d62,0x02e3072 },
      { 0x0e1d569,0x162f772,0x0cd051a,0x322df62,0x3563809,0x047cc7a,
        0x027fd9f,0x08b509b,0x3da2f94,0x01748ee },
      0 },
    /* 84 */
    { { 0x1c8f8be,0x31ca525,0x22bf0a1,0x200efcd,0x02961c4,0x3d8f52b,
        0x018403d,0x3a40279,0x1cb91ec,0x030427e },
      { 0x0945705,0x0257416,0x05c0c2d,0x25b77ae,0x3b9083d,0x2901126,
        0x292b8d7,0x07b8611,0x04f2eee,0x026f0cd },
      0 },
    /* 85 */
    { { 0x2913074,0x2b8d590,0x02b10d5,0x09d2295,0x255491b,0x0c41cca,
        0x1ca665b,0x133051a,0x1525f1a,0x00a5647 },
      { 0x04f983f,0x3d6daee,0x04e1e76,0x1067d7e,0x1be7eef,0x02ea862,
        0x00d4968,0x0ccb048,0x11f18ef,0x018dd95 },
      0 },
    /* 86 */
    { { 0x22976cc,0x17c5395,0x2c38bda,0x3983bc4,0x222bca3,0x332a614,
        0x3a30646,0x261eaef,0x1c808e2,0x02f6de7 },
      { 0x306a772,0x32d7272,0x2dcefd2,0x2abf94d,0x038f475,0x30ad76e,
        0x23e0227,0x3052b0a,0x001add3,0x023ba18 },
      0 },
    /* 87 */
    { { 0x0ade873,0x25a6069,0x248ccbe,0x13713ee,0x17ee9aa,0x28152e9,
        0x2e28995,0x2a92cb3,0x17a6f77,0x024b947 },
      { 0x190a34d,0x2ebea1c,0x1ed1948,0x16fdaf4,0x0d698f7,0x32bc451,
        0x0ee6e30,0x2aaab40,0x06f0a56,0x01460be },
      0 },
    /* 88 */
    { { 0x24cc99c,0x1884b1e,0x1ca1fba,0x1a0f9b6,0x2ff609b,0x2b26316,
        0x3b27cb5,0x29bc976,0x35d4073,0x024772a },
      { 0x3575a70,0x1b30f57,0x07fa01b,0x0e5be36,0x20cb361,0x26605cd,
        0x1d4e8c8,0x13cac59,0x2db9797,0x005e833 },
      0 },
    /* 89 */
    { { 0x36c8d3a,0x1878a81,0x124b388,0x0e4843e,0x1701aad,0x0ea0d76,
        0x10eae41,0x37d0653,0x36c7f4c,0x00ba338 },
      { 0x37a862b,0x1cf6ac0,0x08fa912,0x2dd8393,0x101ba9b,0x0eebcb7,
        0x2453883,0x1a3cfe5,0x2cb34f6,0x03d3331 },
      0 },
    /* 90 */
    { { 0x1f79687,0x3d4973c,0x281544e,0x2564bbe,0x17c5954,0x171e34a,
        0x231741a,0x3cf2784,0x0889a0d,0x02b036d },
      { 0x301747f,0x3f1c477,0x1f1386b,0x163bc5f,0x1592b93,0x332daed,
        0x080e4f5,0x1d28b96,0x26194c9,0x0256992 },
      0 },
    /* 91 */
    { { 0x15a4c93,0x07bf6b0,0x114172c,0x1ce0961,0x140269b,0x1b2c2eb,
        0x0dfb1c1,0x019ddaa,0x0ba2921,0x008c795 },
      { 0x2e6d2dc,0x37e45e2,0x2918a70,0x0fce444,0x34d6aa6,0x396dc88,
        0x27726b5,0x0c787d8,0x032d8a7,0x02ac2f8 },
      0 },
    /* 92 */
    { { 0x1131f2d,0x2b43a63,0x3101097,0x38cec13,0x0637f09,0x17a69d2,
        0x086196d,0x299e46b,0x0802cf6,0x03c6f32 },
      { 0x0daacb4,0x1a4503a,0x100925c,0x15583d9,0x23c4e40,0x1de4de9,
        0x1cc8fc4,0x2c9c564,0x0695aeb,0x02145a5 },
      0 },
    /* 93 */
    { { 0x1dcf593,0x17050fc,0x3e3bde3,0x0a6c062,0x178202b,0x2f7674f,
        0x0dadc29,0x15763a7,0x1d2daad,0x023d9f6 },
      { 0x081ea5f,0x045959d,0x190c841,0x3a78d31,0x0e7d2dd,0x1414fea,
        0x1d43f40,0x22d77ff,0x2b9c072,0x03e115c },
      0 },
    /* 94 */
    { { 0x3af71c9,0x29e9c65,0x25655e1,0x111e9cd,0x3a14494,0x3875418,
        0x34ae070,0x0b06686,0x310616b,0x03b7b89 },
      { 0x1734121,0x00d3d44,0x29f0b2f,0x1552897,0x31cac6e,0x1030bb3,
        0x0148f3a,0x35fd237,0x29b44eb,0x027f49f },
      0 },
    /* 95 */
    { { 0x2e2cb16,0x1d962bd,0x19b63cc,0x0b3f964,0x3e3eb7d,0x1a35560,
        0x0c58161,0x3ce1d6a,0x3b6958f,0x029030b },
      { 0x2dcc158,0x3b1583f,0x30568c9,0x31957c8,0x27ad804,0x28c1f84,
        0x3967049,0x37b3f64,0x3b87dc6,0x0266f26 },
      0 },
    /* 96 */
    { { 0x27dafc6,0x2548764,0x0d1984a,0x1a57027,0x252c1fb,0x24d9b77,
        0x1581a0f,0x1f99276,0x10ba16d,0x026af88 },
      { 0x0915220,0x2be1292,0x16c6480,0x1a93760,0x2fa7317,0x1a07296,
        0x1539871,0x112c31f,0x25787f3,0x01e2070 },
      0 },
    /* 97 */
    { { 0x0bcf3ff,0x266d478,0x34f6933,0x31449fd,0x00d02cb,0x340765a,
        0x3465a2d,0x225023e,0x319a30e,0x00579b8 },
      { 0x20e05f4,0x35b834f,0x0404646,0x3710d62,0x3fad7bd,0x13e1434,
        0x21c7d1c,0x1cb3af9,0x2cf1911,0x003957e },
      0 },
    /* 98 */
    { { 0x0787564,0x36601be,0x1ce67e9,0x084c7a1,0x21a3317,0x2067a35,
        0x0158cab,0x195ddac,0x1766fe9,0x035cf42 },
      { 0x2b7206e,0x20d0947,0x3b42424,0x03f1862,0x0a51929,0x38c2948,
        0x0bb8595,0x2942d77,0x3748f15,0x0249428 },
      0 },
    /* 99 */
    { { 0x2577410,0x3c23e2f,0x28c6caf,0x00d41de,0x0fd408a,0x30298e9,
        0x363289e,0x2302fc7,0x082c1cc,0x01dd050 },
      { 0x30991cd,0x103e9ba,0x029605a,0x19927f7,0x0c1ca08,0x0c93f50,
        0x28a3c7b,0x082e4e9,0x34d12eb,0x0232c13 },
      0 },
    /* 100 */
    { { 0x106171c,0x0b4155a,0x0c3fb1c,0x336c090,0x19073e9,0x2241a10,
        0x0e6b4fd,0x0ed476e,0x1ef4712,0x039390a },
      { 0x0ec36f4,0x3754f0e,0x2a270b8,0x007fd2d,0x0f9d2dc,0x1e6a692,
        0x066e078,0x1954974,0x2ff3c6e,0x00def28 },
      0 },
    /* 101 */
    { { 0x3562470,0x0b8f1f7,0x0ac94cd,0x28b0259,0x244f272,0x031e4ef,
        0x2d5df98,0x2c8a9f1,0x2dc3002,0x016644f },
      { 0x350592a,0x0e6a0d5,0x1e027a1,0x2039e0f,0x399e01d,0x2817593,
        0x0c0375e,0x3889b3e,0x24ab013,0x010de1b },
      0 },
    /* 102 */
    { { 0x256b5a6,0x0ac3b67,0x28f9ff3,0x29b67f1,0x30750d9,0x25e11a9,
        0x15e8455,0x279ebb0,0x298b7e7,0x0218e32 },
      { 0x2fc24b2,0x2b82582,0x28f22f5,0x2bd36b3,0x305398e,0x3b2e9e3,
        0x365dd0a,0x29bc0ed,0x36a7b3a,0x007b374 },
      0 },
    /* 103 */
    { { 0x05ff2f3,0x2b3589b,0x29785d3,0x300a1ce,0x0a2d516,0x0844355,
        0x14c9fad,0x3ccb6b6,0x385d459,0x0361743 },
      { 0x0b11da3,0x002e344,0x18c49f7,0x0c29e0c,0x1d2c22c,0x08237b3,
        0x2988f49,0x0f18955,0x1c3b4ed,0x02813c6 },
      0 },
    /* 104 */
    { { 0x17f93bd,0x249323b,0x11f6087,0x174e4bd,0x3cb64ac,0x086dc6b,
        0x2e330a8,0x142c1f2,0x2ea5c09,0x024acbb },
      { 0x1b6e235,0x3132521,0x00f085a,0x2a4a4db,0x1ab2ca4,0x0142224,
        0x3aa6b3e,0x09db203,0x2215834,0x007b9e0 },
      0 },
    /* 105 */
    { { 0x23e79f7,0x28b8039,0x1906a60,0x2cbce67,0x1f590e7,0x181f027,
        0x21054a6,0x3854240,0x2d857a6,0x03cfcb3 },
      { 0x10d9b55,0x1443cfc,0x2648200,0x2b36190,0x09d2fcf,0x22f439f,
        0x231aa7e,0x3884395,0x0543da3,0x003d5a9 },
      0 },
    /* 106 */
    { { 0x043e0df,0x06ffe84,0x3e6d5b2,0x3327001,0x26c74b6,0x12a145e,
        0x256ec0d,0x3898c69,0x3411969,0x02f63c5 },
      { 0x2b7494a,0x2eee1af,0x38388a9,0x1bd17ce,0x21567d4,0x13969e6,
        0x3a12a7a,0x3e8277d,0x03530cc,0x00b4687 },
      0 },
    /* 107 */
    { { 0x06508da,0x38e04d4,0x15a7192,0x312875e,0x3336180,0x2a6512c,
        0x1b59497,0x2e91b37,0x25eb91f,0x02841e9 },
      { 0x394d639,0x0747143,0x37d7e6d,0x1d62962,0x08b4af3,0x34df287,
        0x3c5584b,0x26bc869,0x20af87a,0x0060f5d },
      0 },
    /* 108 */
    { { 0x1de59a4,0x1a5c443,0x2f8729d,0x01c3a2f,0x0f1ad8d,0x3cbaf9e,
        0x1b49634,0x35d508a,0x39dc269,0x0075105 },
      { 0x390d30e,0x37033e0,0x110cb32,0x14c37a0,0x20a3b27,0x2f00ce6,
        0x2f1dc52,0x34988c6,0x0c29606,0x01dc7e7 },
      0 },
    /* 109 */
    { { 0x1040739,0x24f9de1,0x2939999,0x2e6009a,0x244539d,0x17e3f09,
        0x00f6f2f,0x1c63b3d,0x2310362,0x019109e },
      { 0x1428aa8,0x3cb61e1,0x09a84f4,0x0ffafed,0x07b7adc,0x08f406b,
        0x1b2c6df,0x035b480,0x3496ae9,0x012766d },
      0 },
    /* 110 */
    { { 0x35d1099,0x2362f10,0x1a08cc7,0x13a3a34,0x12adbcd,0x32da290,
        0x02e2a02,0x151140b,0x01b3f60,0x0240df6 },
      { 0x34c7b61,0x2eb09c1,0x172e7cd,0x2ad5eff,0x2fe2031,0x25b54d4,
        0x0cec965,0x18e7187,0x26a7cc0,0x00230f7 },
      0 },
    /* 111 */
    { { 0x2d552ab,0x374083d,0x01f120f,0x2601736,0x156baff,0x04d44a4,
        0x3b7c3e9,0x1acbc1b,0x0424579,0x031a425 },
      { 0x1231bd1,0x0eba710,0x020517b,0x21d7316,0x21eac6e,0x275a848,
        0x0837abf,0x0eb0082,0x302cafe,0x00fe8f6 },
      0 },
    /* 112 */
    { { 0x1058880,0x28f9941,0x03f2d75,0x3bd90e5,0x17da365,0x2ac9249,
        0x07861cf,0x023fd05,0x1b0fdb8,0x031712f },
      { 0x272b56b,0x04f8d2c,0x043a735,0x25446e4,0x1c8327e,0x221125a,
        0x0ce37df,0x2dad7f6,0x39446c2,0x00b55b6 },
      0 },
    /* 113 */
    { { 0x346ac6b,0x05e0bff,0x2425246,0x0981e8b,0x1d19f79,0x2692378,
        0x3ea3c40,0x2e90beb,0x19de503,0x003d5af },
      { 0x05cda49,0x353b44d,0x299d137,0x3f205bc,0x2821158,0x3ad0d00,
        0x06a54aa,0x2d7c79f,0x39d1173,0x01000ee },
      0 },
    /* 114 */
    { { 0x0803387,0x3a06268,0x14043b8,0x3d4e72f,0x1ece115,0x0a1dfc8,
        0x17208dd,0x0be790a,0x122a07f,0x014dd95 },
      { 0x0a4182d,0x202886a,0x1f79a49,0x1e8c867,0x0a2bbd0,0x28668b5,
        0x0d0a2e1,0x115259d,0x3586c5d,0x01e815b },
      0 },
    /* 115 */
    { { 0x18a2a47,0x2c95627,0x2773646,0x1230f7c,0x15b5829,0x2fc354e,
        0x2c000ea,0x099d547,0x2f17a1a,0x01df520 },
      { 0x3853948,0x06f6561,0x3feeb8a,0x2f5b3ef,0x3a6f817,0x01a0791,
        0x2ec0578,0x2c392ad,0x12b2b38,0x0104540 },
      0 },
    /* 116 */
    { { 0x1e28ced,0x0fc3d1b,0x2c473c7,0x1826c4f,0x21d5da7,0x39718e4,
        0x38ce9e6,0x0251986,0x172fbea,0x0337c11 },
      { 0x053c3b0,0x0f162db,0x043c1cb,0x04111ee,0x297fe3c,0x32e5e03,
        0x2b8ae12,0x0c427ec,0x1da9738,0x03b9c0f },
      0 },
    /* 117 */
    { { 0x357e43a,0x054503f,0x11b8345,0x34ec6e0,0x2d44660,0x3d0ae61,
        0x3b5dff8,0x33884ac,0x09da162,0x00a82b6 },
      { 0x3c277ba,0x129a51a,0x027664e,0x1530507,0x0c788c9,0x2afd89d,
        0x1aa64cc,0x1196450,0x367ac2b,0x0358b42 },
      0 },
    /* 118 */
    { { 0x0054ac4,0x1761ecb,0x378839c,0x167c9f7,0x2570058,0x0604a35,
        0x37cbf3b,0x0909bb7,0x3f2991c,0x02ce688 },
      { 0x0b16ae5,0x212857c,0x351b952,0x2c684db,0x30c6a05,0x09c01e0,
        0x23c137f,0x1331475,0x092c067,0x0013b40 },
      0 },
    /* 119 */
    { { 0x2e90393,0x0617466,0x24e61f4,0x0a528f5,0x03047b4,0x2153f05,
        0x0001a69,0x30e1eb8,0x3c10177,0x0282a47 },
      { 0x22c831e,0x28fc06b,0x3e16ff0,0x208adc9,0x0bb76ae,0x28c1d6d,
        0x12c8a15,0x031063c,0x1889ed2,0x002133e },
      0 },
    /* 120 */
    { { 0x0a6becf,0x14277bf,0x3328d98,0x201f7fe,0x12fceae,0x1de3a2e,
        0x0a15c44,0x3ddf976,0x1b273ab,0x0355e55 },
      { 0x1b5d4f1,0x369e78c,0x3a1c210,0x12cf3e9,0x3aa52f0,0x309f082,
        0x112089d,0x107c753,0x24202d1,0x023853a },
      0 },
    /* 121 */
    { { 0x2897042,0x140d17c,0x2c4aeed,0x07d0d00,0x18d0533,0x22f7ec8,
        0x19c194c,0x3456323,0x2372aa4,0x0165f86 },
      { 0x30bd68c,0x1fb06b3,0x0945032,0x372ac09,0x06d4be0,0x27f8fa1,
        0x1c8d7ac,0x137a96e,0x236199b,0x0328fc0 },
      0 },
    /* 122 */
    { { 0x170bd20,0x2842d58,0x1de7592,0x3c5b4fd,0x20ea897,0x12cab78,
        0x363ff14,0x01f928c,0x17e309c,0x02f79ff },
      { 0x0f5432c,0x2edb4ae,0x044b516,0x32f810d,0x2210dc1,0x23e56d6,
        0x301e6ff,0x34660f6,0x10e0a7d,0x02d88eb },
      0 },
    /* 123 */
    { { 0x0c7b65b,0x2f59d58,0x2289a75,0x2408e92,0x1ab8c55,0x1ec99e5,
        0x220fd0d,0x04defe0,0x24658ec,0x035aa8b },
      { 0x138bb85,0x2f002d4,0x295c10a,0x08760ce,0x28c31d1,0x1c0a8cb,
        0x0ff00b1,0x144eac9,0x2e02dcc,0x0044598 },
      0 },
    /* 124 */
    { { 0x3b42b87,0x050057b,0x0dff781,0x1c06db1,0x1bd9f5d,0x1f5f04a,
        0x2cccd7a,0x143e19b,0x1cb94b7,0x036cfb8 },
      { 0x34837cf,0x3cf6c3c,0x0d4fb26,0x22ee55e,0x1e7eed1,0x315995f,
        0x2cdf937,0x1a96574,0x0425220,0x0221a99 },
      0 },
    /* 125 */
    { { 0x1b569ea,0x0d33ed9,0x19c13c2,0x107dc84,0x2200111,0x0569867,
        0x2dc85da,0x05ef22e,0x0eb018a,0x029c33d },
      { 0x04a6a65,0x3e5eba3,0x378f224,0x09c04d0,0x036e5cf,0x3df8258,
        0x3a609e4,0x1eddef8,0x2abd174,0x02a91dc },
      0 },
    /* 126 */
    { { 0x2a60cc0,0x1d84c5e,0x115f676,0x1840da0,0x2c79163,0x2f06ed6,
        0x198bb4b,0x3e5d37b,0x1dc30fa,0x018469b },
      { 0x15ee47a,0x1e32f30,0x16a530e,0x2093836,0x02e8962,0x3767b62,
        0x335adf3,0x27220db,0x2f81642,0x0173ffe },
      0 },
    /* 127 */
    { { 0x37a99cd,0x1533fe6,0x05a1c0d,0x27610f1,0x17bf3b9,0x0b1ce78,
        0x0a908f6,0x265300e,0x3237dc1,0x01b969a },
      { 0x3a5db77,0x2d15382,0x0d63ef8,0x1feb3d8,0x0b7b880,0x19820de,
        0x11c0c67,0x2af3396,0x38d242d,0x0120688 },
      0 },
    /* 128 */
    { { 0x1d0b34a,0x05ef00d,0x00a7e34,0x1ae0c9f,0x1440b38,0x300d8b4,
        0x37262da,0x3e50e3e,0x14ce0cd,0x00b1044 },
      { 0x195a0b1,0x173bc6b,0x03622ba,0x2a19f55,0x1c09b37,0x07921b2,
        0x16cdd20,0x24a5c9b,0x2bf42ff,0x00811de },
      0 },
    /* 129 */
    { { 0x0d65dbf,0x145cf06,0x1ad82f7,0x038ce7b,0x077bf94,0x33c4007,
        0x22d26bd,0x25ad9c0,0x09ac773,0x02b1990 },
      { 0x2261cc3,0x2ecdbf1,0x3e908b0,0x3246439,0x0213f7b,0x1179b04,
        0x01cebaa,0x0be1595,0x175cc12,0x033a39a },
      0 },
    /* 130 */
    { { 0x00a67d2,0x086d06f,0x248a0f1,0x0291134,0x362d476,0x166d1cd,
        0x044f1d6,0x2d2a038,0x365250b,0x0023f78 },
      { 0x08bf287,0x3b0f6a1,0x1d6eace,0x20b4cda,0x2c2a621,0x0912520,
        0x02dfdc9,0x1b35cd6,0x3d2565d,0x00bdf8b },
      0 },
    /* 131 */
    { { 0x3770fa7,0x2e4b6f0,0x03f9ae4,0x170de41,0x1095e8d,0x1dd845c,
        0x334e9d1,0x00ab953,0x12e9077,0x03196fa },
      { 0x2fd0a40,0x228c0fd,0x384b275,0x38ef339,0x3e7d822,0x3e5d9ef,
        0x24f5854,0x0ece9eb,0x247d119,0x012ffe3 },
      0 },
    /* 132 */
    { { 0x0ff1480,0x07487c0,0x1b16cd4,0x1f41d53,0x22ab8fb,0x2f83cfa,
        0x01d2efb,0x259f6b2,0x2e65772,0x00f9392 },
      { 0x05303e6,0x23cdb4f,0x23977e1,0x12e4898,0x03bd999,0x0c930f0,
        0x170e261,0x180a27b,0x2fd58ec,0x014e22b },
      0 },
    /* 133 */
    { { 0x25d7713,0x0c5fad7,0x09daad1,0x3b9d779,0x109b985,0x1d3ec98,
        0x35bc4fc,0x2f838cb,0x0d14f75,0x0173e42 },
      { 0x2657b12,0x10d4423,0x19e6760,0x296e5bb,0x2bfd421,0x25c3330,
        0x29f51f8,0x0338838,0x24060f0,0x029a62e },
      0 },
    /* 134 */
    { { 0x3748fec,0x2c5a1bb,0x2cf973d,0x289fa74,0x3e6e755,0x38997bf,
        0x0b6544c,0x2b6358c,0x38a7aeb,0x02c50bb },
      { 0x3d5770a,0x06be7c5,0x012fad3,0x19cb2cd,0x266af3b,0x3ccd677,
        0x160d1bd,0x141d5af,0x2965851,0x034625a },
      0 },
    /* 135 */
    { { 0x3c41c08,0x255eacc,0x22e1ec5,0x2b151a3,0x087de94,0x311cbdb,
        0x016b73a,0x368e462,0x20b7981,0x0099ec3 },
      { 0x262b988,0x1539763,0x21e76e5,0x15445b4,0x1d8ddc7,0x34a9be6,
        0x10faf03,0x24e4d18,0x07aa111,0x02d538a },
      0 },
    /* 136 */
    { { 0x38a876b,0x048ad45,0x04b40a0,0x3fc2144,0x251ff96,0x13ca7dd,
        0x0b31ab1,0x3539814,0x28b5f87,0x0212aec },
      { 0x270790a,0x350e7e0,0x346bd5e,0x276178f,0x22d6cb5,0x3078884,
        0x355c1b6,0x15901d7,0x3671765,0x03950db },
      0 },
    /* 137 */
    { { 0x286e8d5,0x2409788,0x13be53f,0x2d21911,0x0353c95,0x10238e8,
        0x32f5bde,0x3a67b60,0x28b5b9c,0x001013d },
      { 0x381e8e5,0x0cef7a9,0x2f5bcad,0x06058f0,0x33cdf50,0x04672a8,
        0x1769600,0x31c055d,0x3df0ac1,0x00e9098 },
      0 },
    /* 138 */
    { { 0x2eb596d,0x197b326,0x12b4c29,0x39c08f2,0x101ea03,0x3804e58,
        0x04b4b62,0x28d9d1c,0x13f905e,0x0032a3f },
      { 0x11b2b61,0x08e9095,0x0d06925,0x270e43f,0x21eb7a8,0x0e4a98f,
        0x31d2be0,0x030cf9f,0x2644ddb,0x025b728 },
      0 },
    /* 139 */
    { { 0x07510af,0x2ed0e8e,0x2a01203,0x2a2a68d,0x0846fea,0x3e540de,
        0x3a57702,0x1677348,0x2123aad,0x010d8f8 },
      { 0x0246a47,0x0e871d0,0x124dca4,0x34b9577,0x2b362b8,0x363ebe5,
        0x3086045,0x26313e6,0x15cd8bb,0x0210384 },
      0 },
    /* 140 */
    { { 0x023e8a7,0x0817884,0x3a0bf12,0x3376371,0x3c808a8,0x18e9777,
        0x12a2721,0x35b538a,0x2bd30de,0x017835a },
      { 0x0fc0f64,0x1c8709f,0x2d8807a,0x0743957,0x242eec0,0x347e76c,
        0x27bef91,0x289689a,0x0f42945,0x01f7a92 },
      0 },
    /* 141 */
    { { 0x1060a81,0x3dbc739,0x1615abd,0x1cbe3e5,0x3e79f9c,0x1ab09a2,
        0x136c540,0x05b473f,0x2beebfd,0x02af0a8 },
      { 0x3e2eac7,0x19be474,0x04668ac,0x18f4b74,0x36f10ba,0x0a0b4c6,
        0x10e3770,0x3bf059e,0x3946c7e,0x013a8d4 },
      0 },
    /* 142 */
    { { 0x266309d,0x28be354,0x1a3eed8,0x3020651,0x10a51c6,0x1e31770,
        0x0af45a5,0x3ff0f3b,0x2891c94,0x00e9db9 },
      { 0x17b0d0f,0x33a291f,0x0a5f9aa,0x25a3d61,0x2963ace,0x39a5fef,
        0x230c724,0x1919146,0x10a465e,0x02084a8 },
      0 },
    /* 143 */
    { { 0x3ab8caa,0x31870f3,0x2390ef7,0x2103850,0x218eb8e,0x3a5ccf2,
        0x1dff677,0x2c59334,0x371599c,0x02a9f2a },
      { 0x0837bd1,0x3249cef,0x35d702f,0x3430dab,0x1c06407,0x108f692,
        0x221292f,0x05f0c5d,0x073fe06,0x01038e0 },
      0 },
    /* 144 */
    { { 0x3bf9b7c,0x2020929,0x30d0f4f,0x080fef8,0x3365d23,0x1f3e738,
        0x3e53209,0x1549afe,0x300b305,0x038d811 },
      { 0x0c6c2c7,0x2e6445b,0x3ee64dc,0x022e932,0x0726837,0x0deb67b,
        0x1ed4346,0x3857f73,0x277a3de,0x01950b5 },
      0 },
    /* 145 */
    { { 0x36c377a,0x0adb41e,0x08be3f3,0x11e40d1,0x36cb038,0x036a2bd,
        0x3dd3a82,0x1bc875b,0x2ee09bb,0x02994d2 },
      { 0x035facf,0x05e0344,0x07e630a,0x0ce772d,0x335e55a,0x111fce4,
        0x250fe1c,0x3bc89ba,0x32fdc9a,0x03cf2d9 },
      0 },
    /* 146 */
    { { 0x355fd83,0x1c67f8e,0x1d10eb3,0x1b21d77,0x0e0d7a4,0x173a9e1,
        0x2c9fa90,0x1c39cce,0x22eaae8,0x01f2bea },
      { 0x153b338,0x0534107,0x26c69b8,0x283be1f,0x3e0acc0,0x059cac3,
        0x13d1081,0x148bbee,0x3c1b9bd,0x002aac4 },
      0 },
    /* 147 */
    { { 0x2681297,0x3389e34,0x146addc,0x2c6d425,0x2cb350e,0x1986abc,
        0x0431737,0x04ba4b7,0x2028470,0x012e469 },
      { 0x2f8ddcf,0x3c4255c,0x1af4dcf,0x07a6a44,0x208ebf6,0x0dc90c3,
        0x34360ac,0x072ad23,0x0537232,0x01254d3 },
      0 },
    /* 148 */
    { { 0x07b7e9d,0x3df5c7c,0x116f83d,0x28c4f35,0x3a478ef,0x3011fb8,
        0x2f264b6,0x317b9e3,0x04fd65a,0x032bd1b },
      { 0x2aa8266,0x3431de4,0x04bba04,0x19a44da,0x0edf454,0x392c5ac,
        0x265168a,0x1dc3d5b,0x25704c6,0x00533a7 },
      0 },
    /* 149 */
    { { 0x25e8f91,0x1178fa5,0x2492994,0x2eb2c3c,0x0d3aca1,0x0322828,
        0x1cc70f9,0x269c74c,0x0a53e4c,0x006edc2 },
      { 0x18bdd7a,0x2a79a55,0x26b1d5c,0x0200628,0x0734a05,0x3273c7b,
        0x13aa714,0x0040ac2,0x2f2da30,0x03e7449 },
      0 },
    /* 150 */
    { { 0x3f9563e,0x2f29eab,0x14a0749,0x3fad264,0x1dd077a,0x3d7c59c,
        0x3a0311b,0x331a789,0x0b9729e,0x0201ebf },
      { 0x1b08b77,0x2a4cdf2,0x3e387f8,0x21510f1,0x286c3a7,0x1dbf62e,
        0x3afa594,0x3363217,0x0d16568,0x01d46b7 },
      0 },
    /* 151 */
    { { 0x0715c0d,0x28e2d04,0x17f78ae,0x1c63dda,0x1d113ea,0x0fefc1b,
        0x1eab149,0x1d0fd99,0x0682537,0x00a7b11 },
      { 0x10bebbc,0x11c672d,0x14223d9,0x2ff9141,0x1399ee5,0x34b7b6c,
        0x0d5b3a8,0x01df643,0x0e392a4,0x03fe4dc },
      0 },
    /* 152 */
    { { 0x2b75b65,0x0b5a6f1,0x11c559a,0x3549999,0x24188f8,0x37a75f4,
        0x29f33e3,0x34068a2,0x38ba2a9,0x025dd91 },
      { 0x29af2c7,0x0988b64,0x0923885,0x1b539a4,0x1334f5d,0x226947a,
        0x2cc7e5a,0x20beb39,0x13fac2f,0x01d298c },
      0 },
    /* 153 */
    { { 0x35f079c,0x137f76d,0x2fbbb2f,0x254638d,0x185b07c,0x1f34db7,
        0x2cfcf0e,0x218f46d,0x2150ff4,0x02add6f },
      { 0x33fc9b7,0x0d9f005,0x0fd081b,0x0834965,0x2b90a74,0x102448d,
        0x3dbf03c,0x167d857,0x02e0b44,0x013afab },
      0 },
    /* 154 */
    { { 0x09f2c53,0x317f9d7,0x1411eb6,0x0463aba,0x0d25220,0x256b176,
        0x087633f,0x2bff322,0x07b2c1b,0x037e662 },
      { 0x10aaecb,0x23bb4a1,0x2272bb7,0x06c075a,0x09d4918,0x0736f2b,
        0x0dd511b,0x101625e,0x0a7779f,0x009ec10 },
      0 },
    /* 155 */
    { { 0x33b2eb2,0x0176dfd,0x2118904,0x022386c,0x2e0df85,0x2588c9f,
        0x1b71525,0x28fd540,0x137e4cf,0x02ce4f7 },
      { 0x3d75165,0x0c39ecf,0x3554a12,0x30af34c,0x2d66344,0x3ded408,
        0x36f1be0,0x0d065b0,0x012d046,0x0025623 },
      0 },
    /* 156 */
    { { 0x2601c3b,0x1824fc0,0x335fe08,0x3e33d70,0x0fb0252,0x252bfca,
        0x1cf2808,0x1922e55,0x1a9db9f,0x020721e },
      { 0x2f56c51,0x39a1f31,0x218c040,0x1a4fc5d,0x3fed471,0x0164d4e,
        0x388a419,0x06f1113,0x0f55fc1,0x03e8352 },
      0 },
    /* 157 */
    { { 0x1608e4d,0x3872778,0x022cbc6,0x044d60a,0x3010dda,0x15fb0b5,
        0x37ddc11,0x19f5bda,0x156b6a3,0x023a838 },
      { 0x383b3b4,0x1380bc8,0x353ca35,0x250fc07,0x169966b,0x3780f29,
        0x36632b2,0x2d6b13f,0x124fa00,0x00fd6ae },
      0 },
    /* 158 */
    { { 0x1739efb,0x2ec3656,0x2c0d337,0x3d39faf,0x1c751b0,0x04699f4,
        0x252dd64,0x095b8b6,0x0872b74,0x022f1da },
      { 0x2d3d253,0x38edca0,0x379fa5b,0x287d635,0x3a9f679,0x059d9ee,
        0x0ac168e,0x3cd3e87,0x19060fc,0x02ce1bc },
      0 },
    /* 159 */
    { { 0x3edcfc2,0x0f04d4b,0x2f0d31f,0x1898be2,0x25396bf,0x15ca230,
        0x02b4eae,0x2713668,0x0f71b06,0x0132d18 },
      { 0x38095ea,0x1ed34d6,0x3603ae6,0x165bf01,0x192bbf8,0x1852859,
        0x075f66b,0x1488f85,0x10895ef,0x014b035 },
      0 },
    /* 160 */
    { { 0x1339848,0x3084385,0x0c8d231,0x3a1c1de,0x0e87a28,0x255b85c,
        0x1de6616,0x2702e74,0x1382bb0,0x012b0f2 },
      { 0x198987d,0x381545a,0x34d619b,0x312b827,0x18b2376,0x28fe4cf,
        0x20b7651,0x017d077,0x0c7e397,0x00e0365 },
      0 },
    /* 161 */
    { { 0x1542e75,0x0d56aa0,0x39b701a,0x287b806,0x396c724,0x0935c21,
        0x3a29776,0x0debdac,0x171de26,0x00b38f8 },
      { 0x1d5bc1a,0x3fad27d,0x22b5cfe,0x1f89ddf,0x0a65560,0x144dd5b,
        0x2aac2f9,0x139353f,0x0520b62,0x00b9b36 },
      0 },
    /* 162 */
    { { 0x031c31d,0x16552e3,0x1a0c368,0x0016fc8,0x168533d,0x171e7b2,
        0x17626e7,0x275502f,0x14742c6,0x03285dd },
      { 0x2d2dbb2,0x3b6bffd,0x1d18cc6,0x2f45d2a,0x0fd0d8c,0x2915e3a,
        0x1e8793a,0x0b39a1d,0x3139cab,0x02a5da9 },
      0 },
    /* 163 */
    { { 0x3fb353d,0x147c6e4,0x3a720a6,0x22d5ff3,0x1d75cab,0x06c54a0,
        0x08cfa73,0x12666aa,0x3170a1f,0x021c829 },
      { 0x13e1b90,0x3a34dda,0x1fc38c3,0x02c5bdb,0x2d345dc,0x14aa1d0,
        0x28d00ab,0x224f23a,0x329c769,0x025c67b },
      0 },
    /* 164 */
    { { 0x0e35909,0x3bb6356,0x0116820,0x370cf77,0x29366d8,0x3881409,
        0x3999d06,0x013075f,0x176e157,0x02941ca },
      { 0x0e70b2e,0x28dfab1,0x2a8a002,0x15da242,0x084dcf6,0x116ca97,
        0x31bf186,0x1dc9735,0x09df7b7,0x0264e27 },
      0 },
    /* 165 */
    { { 0x2da7a4b,0x3023c9e,0x1366238,0x00ff4e2,0x03abe9d,0x19bd44b,
        0x272e897,0x20b91ad,0x2aa202c,0x02a2201 },
      { 0x380184e,0x08112b4,0x0b85660,0x31049aa,0x3a8cb78,0x36113c5,
        0x1670c0a,0x373f9e7,0x3fb4738,0x00010ef },
      0 },
    /* 166 */
    { { 0x2d5192e,0x26d770d,0x32af8d5,0x34d1642,0x1acf885,0x05805e0,
        0x166d0a1,0x1219a0d,0x301ba6c,0x014bcfb },
      { 0x2dcb64d,0x19cca83,0x379f398,0x08e01a0,0x10a482c,0x0103cc2,
        0x0be5fa7,0x1f9d45b,0x1899ef2,0x00ca5af },
      0 },
    /* 167 */
    { { 0x14d81d7,0x2aea251,0x1b3c476,0x3bd47ae,0x29eade7,0x0715e61,
        0x1a21cd8,0x1c7a586,0x2bfaee5,0x00ee43f },
      { 0x096f7cb,0x0c08f95,0x1bc4939,0x361fed4,0x255be41,0x26fad73,
        0x31dd489,0x02c600f,0x29d9f81,0x01ba201 },
      0 },
    /* 168 */
    { { 0x03ea1db,0x1eac46d,0x1292ce3,0x2a54967,0x20a7ff1,0x3e13c61,
        0x1b02218,0x2b44e14,0x3eadefa,0x029c88a },
      { 0x30a9144,0x31e3b0a,0x19c5a2a,0x147cbe9,0x05a0240,0x051f38e,
        0x11eca56,0x31a4247,0x123bc2a,0x02fa535 },
      0 },
    /* 169 */
    { { 0x3226ce7,0x1251782,0x0b7072f,0x11e59fa,0x2b8afd7,0x169b18f,
        0x2a46f18,0x31d9bb7,0x2fe9be8,0x01de0b7 },
      { 0x1b38626,0x34aa90f,0x3ad1760,0x21ddbd9,0x3460ae7,0x1126736,
        0x1b86fc5,0x0b92cd0,0x167a289,0x000e0e1 },
      0 },
    /* 170 */
    { { 0x1ec1a0f,0x36bbf5e,0x1c972d8,0x3f73ace,0x13bbcd6,0x23d86a5,
        0x175ffc5,0x2d083d5,0x2c4adf7,0x036f661 },
      { 0x1f39eb7,0x2a20505,0x176c81a,0x3d6e636,0x16ee2fc,0x3cbdc5f,
        0x25475dc,0x2ef4151,0x3c46860,0x0238934 },
      0 },
    /* 171 */
    { { 0x2587390,0x3639526,0x0588749,0x13c32fb,0x212bb19,0x09660f1,
        0x207da4b,0x2bf211b,0x1c4407b,0x01506a6 },
      { 0x24c8842,0x105a498,0x05ffdb2,0x0ab61b0,0x26044c1,0x3dff3d8,
        0x1d14b44,0x0d74716,0x049f57d,0x030024b },
      0 },
    /* 172 */
    { { 0x32e61ef,0x31d70f7,0x35cad3c,0x320b86c,0x07e8841,0x027ca7d,
        0x2d30d19,0x2513718,0x2347286,0x01d7901 },
      { 0x3c237d0,0x107f16e,0x01c9e7d,0x3c3b13c,0x0c9537b,0x20af54d,
        0x051a162,0x2161a47,0x258c784,0x016df2d },
      0 },
    /* 173 */
    { { 0x228ead1,0x29c2122,0x07f6964,0x023f4ed,0x1802dc5,0x19f96ce,
        0x24bfd17,0x25e866b,0x2ba8df0,0x01eb84f },
      { 0x2dd384e,0x05bbe3a,0x3f06fd2,0x366dacb,0x30361a2,0x2f36d7c,
        0x0b98784,0x38ff481,0x074e2a8,0x01e1f60 },
      0 },
    /* 174 */
    { { 0x17fbb1c,0x0975add,0x1debc5e,0x2cb2880,0x3e47bdd,0x3488cff,
        0x15e9a36,0x2121129,0x0199ef2,0x017088a },
      { 0x0315250,0x352a162,0x17c1773,0x0ae09c2,0x321b21a,0x3bd74cf,
        0x3c4ea1d,0x3cac2ad,0x3abbaf0,0x039174d },
      0 },
    /* 175 */
    { { 0x0511c8a,0x3c78d0a,0x2cd3d2d,0x322f729,0x3ebb229,0x09f0e69,
        0x0a71a76,0x2e74d5e,0x12284df,0x03b5ef0 },
      { 0x3dea561,0x0a9b7e4,0x0ed1cf2,0x237523c,0x05443f1,0x2eb48fa,
        0x3861405,0x1b49f62,0x0c945ca,0x02ab25f },
      0 },
    /* 176 */
    { { 0x16bd00a,0x13a9d28,0x3cc1eb5,0x2b7d702,0x2d839e9,0x3e6ff01,
        0x2bb7f11,0x3713824,0x3b31163,0x00c63e5 },
      { 0x30d7138,0x0316fb0,0x0220ecc,0x08eaf0c,0x244e8df,0x0088d81,
        0x37972fb,0x3fd34ae,0x2a19a84,0x03e907e },
      0 },
    /* 177 */
    { { 0x2642269,0x0b65d29,0x03bd440,0x33a6ede,0x3c81814,0x2507982,
        0x0d38e47,0x3a788e6,0x32c1d26,0x00e2eda },
      { 0x2577f87,0x392895a,0x3e1cc64,0x14f7047,0x08b52d2,0x08a01ca,
        0x336abf6,0x00697fc,0x105ce76,0x0253742 },
      0 },
    /* 178 */
    { { 0x293f92a,0x33df737,0x3315156,0x32e26d7,0x0a01333,0x26579d4,
        0x004df9c,0x0aba409,0x067d25c,0x02481de },
      { 0x3f39d44,0x1c78042,0x13d7e24,0x0825aed,0x35f2c90,0x3270f63,
        0x04b7b35,0x3ad4531,0x28bd29b,0x0207a10 },
      0 },
    /* 179 */
    { { 0x077199f,0x270aeb1,0x0dd96dd,0x3b9ad7b,0x28cb8ee,0x3903f43,
        0x37db3fe,0x292c62b,0x362dbbf,0x006e52a },
      { 0x247f143,0x0362cf3,0x216344f,0x3f18fd1,0x351e623,0x31664e0,
        0x0f270fc,0x243bbc6,0x2280555,0x001a8e3 },
      0 },
    /* 180 */
    { { 0x3355b49,0x2c04e6c,0x399b2e5,0x182d3af,0x020e265,0x09a7cf7,
        0x0ffa6bd,0x353e302,0x02083d9,0x029ecdb },
      { 0x33e8830,0x0570e86,0x1c0b64d,0x386a27e,0x0d5fcea,0x0b45a4c,
        0x2ee4a2e,0x0a8833f,0x2b4a282,0x02f9531 },
      0 },
    /* 181 */
    { { 0x191167c,0x36cf7e3,0x225ed6c,0x1e79e99,0x0517c3f,0x11ab1fd,
        0x05648f3,0x08aedc4,0x1abeae0,0x02fcc29 },
      { 0x3828a68,0x1e16fa4,0x30368e7,0x0c9fcfb,0x25161c3,0x24851ac,
        0x1b5feb5,0x344eb84,0x0de2732,0x0347208 },
      0 },
    /* 182 */
    { { 0x038b363,0x384d1e4,0x2519043,0x151ac17,0x158c11f,0x009b2b4,
        0x257abe6,0x2368d3f,0x3ed68a1,0x02df45e },
      { 0x29c2559,0x2962478,0x3d8444c,0x1d96fff,0x04f7a03,0x1391a52,
        0x0de4af7,0x3319126,0x15e6412,0x00e65ff },
      0 },
    /* 183 */
    { { 0x3d61507,0x1d1a0a2,0x0d2af20,0x354d299,0x329e132,0x2a28578,
        0x2ddfb08,0x04fa3ff,0x1293c6c,0x003bae2 },
      { 0x3e259f8,0x1a68fa9,0x3e67e9b,0x39b44f9,0x1ce1db7,0x347e9a1,
        0x3318f6a,0x2dbbc9d,0x2f8c922,0x008a245 },
      0 },
    /* 184 */
    { { 0x212ab5b,0x2b896c2,0x0136959,0x07e55ef,0x0cc1117,0x05b8ac3,
        0x18429ed,0x025fa01,0x11d6e93,0x03b016b },
      { 0x03f3708,0x2e96fab,0x1d77157,0x0d4c2d6,0x131baf9,0x0608d39,
        0x3552371,0x06cdd1e,0x1567ff1,0x01f4c50 },
      0 },
    /* 185 */
    { { 0x2dfefab,0x270173d,0x37077bd,0x1a372cd,0x1be2f22,0x28e2ee5,
        0x3ead973,0x35e8f94,0x2fc9bc1,0x03a7399 },
      { 0x36a02a1,0x2855d9b,0x00ed75a,0x37d8398,0x138c087,0x233706e,
        0x147f346,0x01947e2,0x3017228,0x0365942 },
      0 },
    /* 186 */
    { { 0x2057e60,0x2d31296,0x25e4504,0x2fa37bc,0x1cbccc3,0x1f0732f,
        0x3532081,0x2de8a98,0x19a804e,0x005359a },
      { 0x31f411a,0x2a10576,0x369c2c8,0x02fe035,0x109fbaf,0x30bddeb,
        0x1eef901,0x1662ad3,0x0410d43,0x01bd31a },
      0 },
    /* 187 */
    { { 0x2c24a96,0x1b7d3a5,0x19a3872,0x217f2f6,0x2534dbc,0x2cab8c2,
        0x066ef28,0x26aecf1,0x0fd6118,0x01310d4 },
      { 0x055b8da,0x1fdc5be,0x38a1296,0x25118f0,0x341a423,0x2ba4cd0,
        0x3e1413e,0x062d70d,0x2425a31,0x029c9b4 },
      0 },
    /* 188 */
    { { 0x08c1086,0x1acfba5,0x22e1dae,0x0f72f4e,0x3f1de50,0x0f408bc,
        0x35ed3f0,0x3ce48fc,0x282cc6c,0x004d8e7 },
      { 0x1afaa86,0x24e3ef3,0x22589ac,0x3ec9952,0x1f45bc5,0x14144ca,
        0x23b26e4,0x0d68c65,0x1e1c1a3,0x032a4d9 },
      0 },
    /* 189 */
    { { 0x03b2d20,0x16b1d53,0x241b361,0x05e4138,0x1742a54,0x32741c7,
        0x0521c4c,0x1ca96c2,0x034970b,0x02738a7 },
      { 0x13e0ad6,0x207dcdb,0x034c8cc,0x27bcbe1,0x18060da,0x33a18b6,
        0x2d1d1a6,0x2be60d7,0x3d7ab42,0x012312a },
      0 },
    /* 190 */
    { { 0x0c7485a,0x06c3310,0x0dbfd22,0x2ef949d,0x0ead455,0x098f4ba,
        0x3c76989,0x0cf2d24,0x032f67b,0x01e005f },
      { 0x30cb5ee,0x0d5da64,0x0ed2b9d,0x2503102,0x1c0d14e,0x1cbc693,
        0x37bf552,0x07013e2,0x054de5c,0x014f341 },
      0 },
    /* 191 */
    { { 0x128ccac,0x1617e97,0x346ebcd,0x158016d,0x25f823e,0x34048ea,
        0x39f0a1c,0x3ea3df1,0x1c1d3d7,0x03ba919 },
      { 0x151803b,0x01967c1,0x2f70781,0x27df39a,0x06c0b59,0x24a239c,
        0x15a7702,0x2464d06,0x2a47ae6,0x006db90 },
      0 },
    /* 192 */
    { { 0x27d04c3,0x024df3d,0x38112e8,0x38a27ba,0x01e312b,0x0965358,
        0x35d8879,0x2f4f55a,0x214187f,0x0008936 },
      { 0x05fe36f,0x2ee18c3,0x1f5f87a,0x1813bd4,0x0580f3c,0x0ed0a7b,
        0x0fb1bfb,0x3fcce59,0x2f042bf,0x01820e3 },
      0 },
    /* 193 */
    { { 0x20bbe99,0x32cbc9f,0x39ee432,0x3cc12a8,0x37bda44,0x3ea4e40,
        0x097c7a9,0x0590d7d,0x2022d33,0x018dbac },
      { 0x3ae00aa,0x3439864,0x2d2ffcf,0x3f8c6b9,0x0875a00,0x3e4e407,
        0x3658a29,0x22eb3d0,0x2b63921,0x022113b },
      0 },
    /* 194 */
    { { 0x33bae58,0x05c749a,0x1f3e114,0x1c45f8e,0x27db3df,0x06a3ab6,
        0x37bc7f8,0x1e27b34,0x3dc51fb,0x009eea0 },
      { 0x3f54de5,0x3d0e7fe,0x1a71a7d,0x02ed7f8,0x0727703,0x2ca5e92,
        0x2e8e35d,0x292ad0b,0x13487f3,0x02b6d8b },
      0 },
    /* 195 */
    { { 0x175df2a,0x05a28a8,0x32e99b1,0x13d8630,0x2082aa0,0x11ac245,
        0x24f2e71,0x322cb27,0x17675e7,0x02e643f },
      { 0x1f37313,0x2765ad3,0x0789082,0x1e742d0,0x11c2055,0x2021dc4,
        0x09ae4a7,0x346359b,0x2f94d10,0x0205c1f },
      0 },
    /* 196 */
    { { 0x3d6ff96,0x1f2ac80,0x336097d,0x3f03610,0x35b851b,0x010b6d2,
        0x0823c4d,0x2a9709a,0x2ead5a8,0x00de4b6 },
      { 0x01afa0b,0x0621965,0x3671528,0x1050b60,0x3f3e9e7,0x2f93829,
        0x0825275,0x006e85f,0x35e94b0,0x016af58 },
      0 },
    /* 197 */
    { { 0x2c4927c,0x3ea1382,0x0f23727,0x0d69f23,0x3e38860,0x2b72837,
        0x3cd5ea4,0x2d84292,0x321846a,0x016656f },
      { 0x29dfa33,0x3e182e0,0x018be90,0x2ba563f,0x2caafe2,0x218c0d9,
        0x3baf447,0x1047a6c,0x0a2d483,0x01130cb },
      0 },
    /* 198 */
    { { 0x00ed80c,0x2a5fc79,0x0a82a74,0x2c4c74b,0x15f938c,0x30b5ab6,
        0x32124b7,0x295314f,0x2fb8082,0x007c858 },
      { 0x20b173e,0x19f315c,0x12f97e4,0x198217c,0x040e8a6,0x3275977,
        0x2bc20e4,0x01f2633,0x02bc3e9,0x023c750 },
      0 },
    /* 199 */
    { { 0x3c4058a,0x24be73e,0x16704f5,0x2d8a4bd,0x3b15e14,0x3076315,
        0x1cfe37b,0x36fe715,0x343926e,0x02c6603 },
      { 0x2c76b09,0x0cf824c,0x3f7898c,0x274cec1,0x11df527,0x18eed18,
        0x08ead48,0x23915bc,0x19b3744,0x00a0a2b },
      0 },
    /* 200 */
    { { 0x0cf4ac5,0x1c8b131,0x0afb696,0x0ff7799,0x2f5ac1a,0x022420c,
        0x11baa2e,0x2ce4015,0x1275a14,0x0125cfc },
      { 0x22eac5d,0x360cd4c,0x3568e59,0x3d42f66,0x35e07ee,0x09620e4,
        0x36720fa,0x22b1eac,0x2d0db16,0x01b6b23 },
      0 },
    /* 201 */
    { { 0x1a835ef,0x1516bbb,0x2d51f7b,0x3487443,0x14aa113,0x0dd06c2,
        0x1a65e01,0x379300d,0x35920b9,0x012c8fb },
      { 0x04c7341,0x2eda00f,0x3c37e82,0x1b4fd62,0x0d45770,0x1478fba,
        0x127863a,0x26939cd,0x134ddf4,0x01375c5 },
      0 },
    /* 202 */
    { { 0x1476cd9,0x1119ca5,0x325bbf9,0x0bf8c69,0x0648d07,0x312d9f8,
        0x01c8b8f,0x136ec51,0x0002f4a,0x03f4c5c },
      { 0x195d0e1,0x10ffd22,0x29aa1cb,0x3443bdc,0x276e695,0x05e6260,
        0x15f9764,0x3cd9783,0x18c9569,0x0053eb1 },
      0 },
    /* 203 */
    { { 0x312ae18,0x280197c,0x3fc9ad9,0x303f324,0x251958d,0x29f4a11,
        0x2142408,0x3694366,0x25136ab,0x03b5f1d },
      { 0x1d4abbc,0x1c3c689,0x13ea462,0x3cfc684,0x39b5dd8,0x2d4654b,
        0x09b0755,0x27d4f18,0x3f74d2e,0x03fbf2d },
      0 },
    /* 204 */
    { { 0x2119185,0x2525eae,0x1ba4bd0,0x0c2ab11,0x1d54e8c,0x294845e,
        0x2479dea,0x3602d24,0x17e87e0,0x0060069 },
      { 0x0afffb0,0x34fe37f,0x1240073,0x02eb895,0x06cf33c,0x2d7f7ef,
        0x1d763b5,0x04191e0,0x11e1ead,0x027e3f0 },
      0 },
    /* 205 */
    { { 0x269544c,0x0e85c57,0x3813158,0x19fc12d,0x20eaf85,0x1e2930c,
        0x22a8fd2,0x1a6a478,0x09d3d3a,0x02a74e0 },
      { 0x1a2da3b,0x30b0b16,0x0847936,0x3d86257,0x138ccbc,0x0f5421a,
        0x25244e6,0x23bdd79,0x1aee117,0x00c01ae },
      0 },
    /* 206 */
    { { 0x1eead28,0x07cac32,0x1fbc0bb,0x17627d3,0x17eef63,0x0b3a24e,
        0x0757fdb,0x3dd841d,0x3d745f8,0x002ae17 },
      { 0x25b4549,0x29f24cf,0x2f21ecd,0x1725e48,0x04be2bb,0x10ee010,
        0x1a1274b,0x10b0898,0x27511e9,0x02c48b5 },
      0 },
    /* 207 */
    { { 0x2a5ae7a,0x181ef99,0x0be33be,0x3e9dab7,0x101e703,0x3adb971,
        0x1043014,0x2ebb2be,0x1c1097d,0x027d667 },
      { 0x3f250ed,0x16dc603,0x20dc6d7,0x1d0d268,0x38eb915,0x02c89e8,
        0x1605a41,0x12de109,0x0e08a29,0x01f554a },
      0 },
    /* 208 */
    { { 0x0c26def,0x163d988,0x2d1ef0f,0x3a960ac,0x1025585,0x0738e20,
        0x27d79b0,0x05cc3ef,0x201303f,0x00a333a },
      { 0x1644ba5,0x2af345e,0x30b8d1d,0x3a01bff,0x31fc643,0x1acf85e,
        0x0a76fc6,0x04efe98,0x348a1d0,0x03062eb },
      0 },
    /* 209 */
    { { 0x1c4216d,0x18e3217,0x02ac34e,0x19c8185,0x200c010,0x17d4192,
        0x13a1719,0x165af51,0x09db7a9,0x0277be0 },
      { 0x3ab8d2c,0x2190b99,0x22b641e,0x0cd88de,0x3b42404,0x1310862,
        0x106a6d6,0x23395f5,0x0b06880,0x000d5fe },
      0 },
    /* 210 */
    { { 0x0d2cc88,0x36f9913,0x339d8e9,0x237c2e3,0x0cc61c2,0x34c2832,
        0x309874c,0x2621d28,0x2dd1b48,0x0392806 },
      { 0x17cd8f9,0x07bab3d,0x0c482ed,0x0faf565,0x31b767d,0x2f4bde1,
        0x295c717,0x330c29c,0x179ce10,0x0119b5f },
      0 },
    /* 211 */
    { { 0x1ada2c7,0x0c624a7,0x227d47d,0x30e3e6a,0x14fa0a6,0x0829678,
        0x24fd288,0x2b46a43,0x122451e,0x0319ca9 },
      { 0x186b655,0x01f3217,0x0af1306,0x0efe6b5,0x2f0235d,0x1c45ca9,
        0x2086805,0x1d44e66,0x0faf2a6,0x0178f59 },
      0 },
    /* 212 */
    { { 0x33b4416,0x10431e6,0x2d99aa6,0x217aac9,0x0cd8fcf,0x2d95a9d,
        0x3ff74ad,0x10bf17a,0x295eb8e,0x01b229e },
      { 0x02a63bd,0x182e9ec,0x004710c,0x00e2e3c,0x06b2f23,0x04b642c,
        0x2c37383,0x32a4631,0x022ad82,0x00d22b9 },
      0 },
    /* 213 */
    { { 0x0cda2fb,0x1d198d7,0x26d27f4,0x286381c,0x022acca,0x24ac7c8,
        0x2df7824,0x0b4ba16,0x1e0d9ef,0x03041d3 },
      { 0x29a65b3,0x0f3912b,0x151bfcf,0x2b0175c,0x0fd71e4,0x39aa5e2,
        0x311f50c,0x13ff351,0x3dbc9e5,0x03eeb7e },
      0 },
    /* 214 */
    { { 0x0a99363,0x0fc7348,0x2775171,0x23db3c8,0x2b91565,0x134d66c,
        0x0175cd2,0x1bf365a,0x2b48371,0x02dfe5d },
      { 0x16dbf74,0x2389357,0x2f36575,0x3f5c70e,0x38d23ba,0x090f7f8,
        0x3477600,0x3201523,0x32ecafc,0x03d3506 },
      0 },
    /* 215 */
    { { 0x1abd48d,0x073ca3f,0x38a451f,0x0d8cb01,0x1ce81be,0x05c51ba,
        0x0e29741,0x03c41ab,0x0eae016,0x0060209 },
      { 0x2e58358,0x1da62d9,0x2358038,0x14b39b2,0x1635687,0x39079b1,
        0x380e345,0x1b49608,0x23983cf,0x019f97d },
      0 },
    /* 216 */
    { { 0x34899ef,0x332e373,0x04c0f89,0x3c27aed,0x1949015,0x09663b2,
        0x2f9276b,0x07f1951,0x09a04c1,0x027fbde },
      { 0x3d2a071,0x19fb3d4,0x1b096d3,0x1fe9146,0x3b10e1a,0x0478bbb,
        0x2b3fb06,0x1388329,0x181a99c,0x02f2030 },
      0 },
    /* 217 */
    { { 0x1eb82e6,0x14dbe39,0x3920972,0x31fd5b2,0x21a484f,0x02d7697,
        0x0e21715,0x37c431e,0x2629f8c,0x01249c3 },
      { 0x26b50ad,0x26deefa,0x0ffc1a3,0x30688e2,0x39a0284,0x041c65e,
        0x03eb178,0x0bdfd50,0x2f96137,0x034bb94 },
      0 },
    /* 218 */
    { { 0x0e0362a,0x334a162,0x194dd37,0x29e3e97,0x2442fa8,0x10d2949,
        0x3836e5a,0x2dccebf,0x0bee5ab,0x037ed1e },
      { 0x33eede6,0x3c739d9,0x2f04a91,0x350ad6c,0x3a5390a,0x14c368b,
        0x26f7bf5,0x11ce979,0x0b408df,0x0366850 },
      0 },
    /* 219 */
    { { 0x28ea498,0x0886d5b,0x2e090e0,0x0a4d58f,0x2623478,0x0d74ab7,
        0x2b83913,0x12c6b81,0x18d623f,0x01d8301 },
      { 0x198aa79,0x26d6330,0x3a7f0b8,0x34bc1ea,0x2f74890,0x378955a,
        0x204110f,0x0102538,0x02d8f19,0x01c5066 },
      0 },
    /* 220 */
    { { 0x14b0f45,0x2838cd3,0x14e16f0,0x0e0e4aa,0x2d9280b,0x0f18757,
        0x3324c6b,0x1391ceb,0x1ce89d5,0x00ebe74 },
      { 0x0930371,0x3de6048,0x3097fd8,0x1308705,0x3eda266,0x3108c26,
        0x1545dcd,0x1f7583a,0x1c37395,0x02c7e05 },
      0 },
    /* 221 */
    { { 0x1fec44a,0x2a9e3a2,0x0caf84f,0x11cf2a9,0x0c8c2ae,0x06da989,
        0x1c807dc,0x3c149a4,0x1141543,0x02906bb },
      { 0x15ffe04,0x0d4e65f,0x2e20424,0x37d896d,0x18bacb2,0x1e05ddd,
        0x1660be8,0x183be17,0x1dd86fb,0x035ba70 },
      0 },
    /* 222 */
    { { 0x2853264,0x0ba5fb1,0x0a0b3aa,0x2df88c1,0x2771533,0x23aba6f,
        0x112bb7b,0x3e3086e,0x210ae9b,0x027271b },
      { 0x030b74c,0x0269678,0x1e90a23,0x135a98c,0x24ed749,0x126de7c,
        0x344b23a,0x186da27,0x19640fa,0x0159af5 },
      0 },
    /* 223 */
    { { 0x18061f3,0x3004630,0x3c70066,0x34df20f,0x1190b25,0x1c9cc91,
        0x1fc8e02,0x0d17bc1,0x390f525,0x033cb1c },
      { 0x0eb30cf,0x2f3ad04,0x303aa09,0x2e835dd,0x1cfd2eb,0x143fc95,
        0x02c43a1,0x025e7a1,0x3558aa2,0x000bd45 },
      0 },
    /* 224 */
    { { 0x1db7d07,0x3bde52b,0x1500396,0x1089115,0x20b4fc7,0x1e2a8f3,
        0x3f8eacc,0x365f7eb,0x1a5e8d4,0x0053a6b },
      { 0x37079e2,0x120284b,0x000edaa,0x33792c2,0x145baa3,0x20e055f,
        0x365e2d7,0x26ba005,0x3ab8e9d,0x0282b53 },
      0 },
    /* 225 */
    { { 0x2653618,0x2dd8852,0x2a5f0bf,0x0f0c7aa,0x2187281,0x1252757,
        0x13e7374,0x3b47855,0x0b86e56,0x02f354c },
      { 0x2e9c47b,0x2fa14cc,0x19ab169,0x3fad401,0x0dc2776,0x24afeed,
        0x3a97611,0x0d07736,0x3cf6979,0x02424a0 },
      0 },
    /* 226 */
    { { 0x2e81a13,0x000c91d,0x123967b,0x265885c,0x29bee1a,0x0cb8675,
        0x2d361bd,0x1526823,0x3c9ace1,0x00d7bad },
      { 0x24e5bdc,0x02b969f,0x2c6e128,0x34edb3b,0x12dcd2c,0x3899af0,
        0x24224c6,0x3a1914b,0x0f4448a,0x026a2cb },
      0 },
    /* 227 */
    { { 0x1d03b59,0x1c6fc82,0x32abf64,0x28ed96b,0x1c90e62,0x2f57bb2,
        0x3ff168e,0x04de7fd,0x0f4d449,0x01af6d8 },
      { 0x255bc30,0x2bfaf22,0x3fe0dad,0x0584025,0x1c79ead,0x3078ef7,
        0x2197414,0x022a50b,0x0fd94ba,0x0007b0f },
      0 },
    /* 228 */
    { { 0x09485c2,0x09dfaf7,0x10c7ba6,0x1e48bec,0x248cc9a,0x028a362,
        0x21d60f7,0x193d93d,0x1c04754,0x0346b2c },
      { 0x2f36612,0x240ac49,0x0d8bd26,0x13b8186,0x259c3a4,0x020d5fb,
        0x38a8133,0x09b0937,0x39d4056,0x01f7341 },
      0 },
    /* 229 */
    { { 0x05a4b48,0x1f534fc,0x07725ce,0x148dc8c,0x2adcd29,0x04aa456,
        0x0f79718,0x066e346,0x189377d,0x002fd4d },
      { 0x068ea73,0x336569b,0x184d35e,0x32a08e9,0x3c7f3bb,0x11ce9c8,
        0x3674c6f,0x21bf27e,0x0d9e166,0x034a2f9 },
      0 },
    /* 230 */
    { { 0x0fa8e4b,0x2e6418e,0x18fc5d2,0x1ba24ff,0x0559f18,0x0dbedbf,
        0x2de2aa4,0x22338e9,0x3aa510f,0x035d801 },
      { 0x23a4988,0x02aad94,0x02732d1,0x111d374,0x0b455cf,0x0d01c9e,
        0x067082a,0x2ec05fd,0x368b303,0x03cad4b },
      0 },
    /* 231 */
    { { 0x035b4ca,0x1fabea6,0x1cbc0d5,0x3f2ed9a,0x02d2232,0x1990c66,
        0x2eb680c,0x3b4ea3b,0x18ecc5a,0x03636fa },
      { 0x1a02709,0x26f8ff1,0x1fa8cba,0x397d6e8,0x230be68,0x043aa14,
        0x3d43cdf,0x25c17fa,0x3a3ee55,0x0380564 },
      0 },
    /* 232 */
    { { 0x275a0a6,0x16bd43a,0x0033d3e,0x2b15e16,0x2512226,0x005d901,
        0x26d50fd,0x3bc19bf,0x3b1aeb8,0x02bfb01 },
      { 0x0bb0a31,0x26559e0,0x1aae7fb,0x330dcc2,0x16f1af3,0x06afce2,
        0x13a15a0,0x2ff7645,0x3546e2d,0x029c6e4 },
      0 },
    /* 233 */
    { { 0x0f593d2,0x384b806,0x122bbf8,0x0a281e0,0x1d1a904,0x2e93cab,
        0x0505db0,0x08f6454,0x05c6285,0x014e880 },
      { 0x3f2b935,0x22d8e79,0x161a07c,0x16b060a,0x02bff97,0x146328b,
        0x3ceea77,0x238f61a,0x19b3d58,0x02fd1f4 },
      0 },
    /* 234 */
    { { 0x17665d5,0x259e9f7,0x0de5672,0x15cbcbd,0x34e3030,0x035240f,
        0x0005ae8,0x286d851,0x07f39c9,0x000070b },
      { 0x1efc6d6,0x2a0051a,0x2724143,0x2a9ef1e,0x0c810bd,0x1e05429,
        0x25670ba,0x2e66d7d,0x0e786ff,0x03f6b7e },
      0 },
    /* 235 */
    { { 0x3c00785,0x232e23f,0x2b67fd3,0x244ed23,0x077fa75,0x3cda3ef,
        0x14d055b,0x0f25011,0x24d5aa4,0x00ea0e3 },
      { 0x297bb9a,0x198ca4f,0x14d9561,0x18d1076,0x39eb933,0x2b6caa0,
        0x1591a60,0x0768d45,0x257873e,0x00f36e0 },
      0 },
    /* 236 */
    { { 0x1e77eab,0x0502a5f,0x0109137,0x0350592,0x3f7e1c5,0x3ac7437,
        0x2dcad2c,0x1fee9d8,0x089f1f5,0x0169833 },
      { 0x0d45673,0x0d8e090,0x065580b,0x065644f,0x11b82be,0x3592dd0,
        0x3284b8d,0x23f0015,0x16fdbfd,0x0248bfd },
      0 },
    /* 237 */
    { { 0x1a129a1,0x1977bb2,0x0e041b2,0x15f30a1,0x0a5b1ce,0x3afef8f,
        0x380c46c,0x3358810,0x27df6c5,0x01ca466 },
      { 0x3b90f9a,0x3d14ea3,0x031b298,0x02e2390,0x2d719c0,0x25bc615,
        0x2c0e777,0x0226b8c,0x3803624,0x0179e45 },
      0 },
    /* 238 */
    { { 0x363cdfb,0x1bb155f,0x24fd5c1,0x1c7c72b,0x28e6a35,0x18165f2,
        0x226bea5,0x0beaff3,0x371e24c,0x0138294 },
      { 0x1765357,0x29034e9,0x22b4276,0x11035ce,0x23c89af,0x074468c,
        0x3370ae4,0x013bae3,0x018d566,0x03d7fde },
      0 },
    /* 239 */
    { { 0x209df21,0x0f8ff86,0x0e47fbf,0x23b99ba,0x126d5d2,0x2722405,
        0x16bd0a2,0x1799082,0x0e9533f,0x039077c },
      { 0x3ba9e3f,0x3f6902c,0x1895305,0x3ac9813,0x3f2340c,0x3c0d9f1,
        0x26e1927,0x0557c21,0x16eac4f,0x023b75f },
      0 },
    /* 240 */
    { { 0x3fc8ff3,0x0770382,0x342fc9a,0x0afa4db,0x314efd8,0x328e07b,
        0x016f7cc,0x3ba599c,0x1caed8a,0x0050cb0 },
      { 0x0b23c26,0x2120a5c,0x3273ec6,0x1cc1cd6,0x2a64fe8,0x2bbc3d6,
        0x09f6e5e,0x34b1b8e,0x00b5ac8,0x032bbd2 },
      0 },
    /* 241 */
    { { 0x1315922,0x1725e1d,0x0ca5524,0x1c4c18f,0x3d82951,0x193bcb2,
        0x0e60d0b,0x388dbcf,0x37e8efa,0x0342e85 },
      { 0x1b3af60,0x26ba3ec,0x220e53a,0x394f4b6,0x01a796a,0x3e7bbca,
        0x163605d,0x2b85807,0x17c1c54,0x03cc725 },
      0 },
    /* 242 */
    { { 0x1cc4597,0x1635492,0x2028c0f,0x2c2eb82,0x2dc5015,0x0d2a052,
        0x05fc557,0x1f0ebbf,0x0cb96e1,0x0004d01 },
      { 0x1a824bf,0x3896172,0x2ed7b29,0x178007a,0x0d59318,0x07bda2b,
        0x2ee6826,0x0f9b235,0x04b9193,0x01bcddf },
      0 },
    /* 243 */
    { { 0x0333fd2,0x0eeb46a,0x15b89f9,0x00968aa,0x2a89302,0x2bdd6b3,
        0x1e5037e,0x2541884,0x24ed2d0,0x01b6e8f },
      { 0x04399cd,0x3be6334,0x3adea48,0x1bb9adc,0x31811c6,0x05fb2bc,
        0x360752c,0x3d29dcb,0x3423bec,0x03c4f3c },
      0 },
    /* 244 */
    { { 0x119e2eb,0x2e7b02a,0x0f68cee,0x257d8b0,0x183a9a1,0x2ae88a6,
        0x3a3bb67,0x2eb4f3e,0x1a9274b,0x0320fea },
      { 0x2fa1ce0,0x346c2d8,0x2fbf0d7,0x3d4d063,0x0e58b60,0x09c1bc1,
        0x28ef9e5,0x09a0efe,0x0f45d70,0x02d275c },
      0 },
    /* 245 */
    { { 0x2d5513b,0x31d443e,0x1e2d914,0x3b2c5d4,0x105f32e,0x27ee756,
        0x050418d,0x3c73db6,0x1bb0c30,0x01673eb },
      { 0x1cb7fd6,0x1eb08d5,0x26a3e16,0x2e20810,0x0249367,0x029e219,
        0x2ec58c9,0x12d9fab,0x362354a,0x016eafc },
      0 },
    /* 246 */
    { { 0x2424865,0x260747b,0x177f37c,0x1e3cb95,0x08b0028,0x2783016,
        0x2970f1b,0x323c1c0,0x2a79026,0x0186231 },
      { 0x0f244da,0x26866f4,0x087306f,0x173ec20,0x31ecced,0x3c84d8d,
        0x070f9b9,0x2e764d5,0x075df50,0x0264ff9 },
      0 },
    /* 247 */
    { { 0x32c3609,0x0c737e6,0x14ea68e,0x300b11b,0x184eb19,0x29dd440,
        0x09ec1a9,0x185adeb,0x0664c80,0x0207dd9 },
      { 0x1fbe978,0x30a969d,0x33561d7,0x34fc60e,0x36743fe,0x00774af,
        0x0d1f045,0x018360e,0x12a5fe9,0x01592a0 },
      0 },
    /* 248 */
    { { 0x2817d1d,0x2993d3e,0x2e0f7a5,0x112faa0,0x255f968,0x355fe6a,
        0x3f5a0fc,0x075b2d7,0x3cf00e5,0x0089afc },
      { 0x32833cf,0x06a7e4b,0x09a8d6d,0x1693d3e,0x320a0a3,0x3cfdfdd,
        0x136c498,0x1e0d845,0x347ff25,0x01a1de7 },
      0 },
    /* 249 */
    { { 0x3043d08,0x030705c,0x20fa79b,0x1d07f00,0x0a54467,0x29b49b4,
        0x367e289,0x0b82f4d,0x0d1eb09,0x025ef2c },
      { 0x32ed3c3,0x1baaa3c,0x3c482ab,0x146ca06,0x3c8a4f1,0x3e85e3c,
        0x1bf4f3b,0x1195534,0x3e80a78,0x02a1cbf },
      0 },
    /* 250 */
    { { 0x32b2086,0x2de4d68,0x3486b1a,0x03a0583,0x2e1eb71,0x2dab9af,
        0x10cd913,0x28daa6f,0x3fcb732,0x000a04a },
      { 0x3605318,0x3f5f2b3,0x2d1da63,0x143f7f5,0x1646e5d,0x040b586,
        0x1683982,0x25abe87,0x0c9fe53,0x001ce47 },
      0 },
    /* 251 */
    { { 0x380d02b,0x055fc22,0x3f7fc50,0x3458a1d,0x26b8333,0x23550ab,
        0x0a1af87,0x0a821eb,0x2dc7e6d,0x00d574a },
      { 0x07386e1,0x3ccd68a,0x3275b41,0x253e390,0x2fd272a,0x1e6627a,
        0x2ca2cde,0x0e9e4a1,0x1e37c2a,0x00f70ac },
      0 },
    /* 252 */
    { { 0x0581352,0x2748701,0x02bed68,0x094dd9e,0x30a00c8,0x3fb5c07,
        0x3bd5909,0x211ac80,0x1103ccd,0x0311e1a },
      { 0x0c768ed,0x29dc209,0x36575db,0x009a107,0x272feea,0x2b33383,
        0x313ed56,0x134c9cc,0x168d5bb,0x033310a },
      0 },
    /* 253 */
    { { 0x17620b9,0x143784f,0x256a94e,0x229664a,0x1d89a5c,0x1d521f2,
        0x0076406,0x1c73f70,0x342aa48,0x03851fa },
      { 0x0f3ae46,0x2ad3bab,0x0fbe274,0x3ed40d4,0x2fd4936,0x232103a,
        0x2afe474,0x25b8f7c,0x047080e,0x008e6b0 },
      0 },
    /* 254 */
    { { 0x3fee8d4,0x347cd4a,0x0fec481,0x33fe9ec,0x0ce80b5,0x33a6bcf,
        0x1c4c9e2,0x3967441,0x1a3f5f7,0x03157e8 },
      { 0x257c227,0x1bc53a0,0x200b318,0x0fcd0af,0x2c5b165,0x2a413ec,
        0x2fc998a,0x2da6426,0x19cd4f4,0x0025336 },
      0 },
    /* 255 */
    { { 0x303beba,0x2072135,0x32918a9,0x140cb3a,0x08631d1,0x0ef527b,
        0x05f2c9e,0x2b4ce91,0x0b642ab,0x02e428c },
      { 0x0a5abf9,0x15013ed,0x3603b46,0x30dd76d,0x3004750,0x28d7627,
        0x1a42ccc,0x093ddbe,0x39a1b79,0x00067e2 },
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
static int sp_256_ecc_mulmod_base_10(sp_point* r, sp_digit* k,
        int map, void* heap)
{
    return sp_256_ecc_mulmod_stripe_10(r, &p256_base, p256_table,
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
    sp_digit kd[10];
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
        k = XMALLOC(sizeof(sp_digit) * 10, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(k, 10, km);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_10(point, k, map, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_10(point, k, map, heap);
    }
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_10(point, r);

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
static int sp_256_iszero_10(const sp_digit* a)
{
    return (a[0] | a[1] | a[2] | a[3] | a[4] | a[5] | a[6] | a[7] |
            a[8] | a[9]) == 0;
}

#endif /* WOLFSSL_VALIDATE_ECC_KEYGEN || HAVE_ECC_SIGN */
/* Add 1 to a. (a = a + 1)
 *
 * r  A single precision integer.
 * a  A single precision integer.
 */
SP_NOINLINE static void sp_256_add_one_10(sp_digit* a)
{
    a[0]++;
    sp_256_norm_10(a);
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
        if (s >= 18) {
            r[j] &= 0x3ffffff;
            s = 26 - s;
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
static int sp_256_ecc_gen_k_10(WC_RNG* rng, sp_digit* k)
{
    int err;
    byte buf[32];

    do {
        err = wc_RNG_GenerateBlock(rng, buf, sizeof(buf));
        if (err == 0) {
            sp_256_from_bin(k, 10, buf, sizeof(buf));
            if (sp_256_cmp_10(k, p256_order2) < 0) {
                sp_256_add_one_10(k);
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
    sp_digit kd[10];
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
        k = XMALLOC(sizeof(sp_digit) * 10, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY)
        err = sp_256_ecc_gen_k_10(rng, k);
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_10(point, k, 1, NULL);
        else
#endif
            err = sp_256_ecc_mulmod_base_10(point, k, 1, NULL);
    }

#ifdef WOLFSSL_VALIDATE_ECC_KEYGEN
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            err = sp_256_ecc_mulmod_avx2_10(infinity, point, p256_order, 1,
                                                                          NULL);
        }
        else
#endif
            err = sp_256_ecc_mulmod_10(infinity, point, p256_order, 1, NULL);
    }
    if (err == MP_OKAY) {
        if (!sp_256_iszero_10(point->x) || !sp_256_iszero_10(point->y))
            err = ECC_INF_E;
    }
#endif

    if (err == MP_OKAY)
        err = sp_256_to_mp(k, priv);
    if (err == MP_OKAY)
        err = sp_256_point_to_ecc_point_10(point, pub);

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

    for (i=0; i<9; i++) {
        r[i+1] += r[i] >> 26;
        r[i] &= 0x3ffffff;
    }
    j = 256 / 8 - 1;
    a[j] = 0;
    for (i=0; i<10 && j>=0; i++) {
        b = 0;
        a[j--] |= r[i] << s; b += 8 - s;
        if (j < 0)
            break;
        while (b < 26) {
            a[j--] = r[i] >> b; b += 8;
            if (j < 0)
                break;
        }
        s = 8 - (b - 26);
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
    sp_digit kd[10];
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
        k = XMALLOC(sizeof(sp_digit) * 10, heap, DYNAMIC_TYPE_ECC);
        if (k == NULL)
            err = MEMORY_E;
    }
#else
    k = kd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(k, 10, priv);
        sp_256_point_from_ecc_point_10(point, pub);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_10(point, point, k, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_10(point, point, k, 1, heap);
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
SP_NOINLINE static void sp_256_mul_d_10(sp_digit* r, const sp_digit* a,
    const sp_digit b)
{
#ifdef WOLFSSL_SP_SMALL
    int64_t tb = b;
    int64_t t = 0;
    int i;

    for (i = 0; i < 10; i++) {
        t += tb * a[i];
        r[i] = t & 0x3ffffff;
        t >>= 26;
    }
    r[10] = (sp_digit)t;
#else
    int64_t tb = b;
    int64_t t[10];

    t[ 0] = tb * a[ 0];
    t[ 1] = tb * a[ 1];
    t[ 2] = tb * a[ 2];
    t[ 3] = tb * a[ 3];
    t[ 4] = tb * a[ 4];
    t[ 5] = tb * a[ 5];
    t[ 6] = tb * a[ 6];
    t[ 7] = tb * a[ 7];
    t[ 8] = tb * a[ 8];
    t[ 9] = tb * a[ 9];
    r[ 0] =                           (t[ 0] & 0x3ffffff);
    r[ 1] = (sp_digit)(t[ 0] >> 26) + (t[ 1] & 0x3ffffff);
    r[ 2] = (sp_digit)(t[ 1] >> 26) + (t[ 2] & 0x3ffffff);
    r[ 3] = (sp_digit)(t[ 2] >> 26) + (t[ 3] & 0x3ffffff);
    r[ 4] = (sp_digit)(t[ 3] >> 26) + (t[ 4] & 0x3ffffff);
    r[ 5] = (sp_digit)(t[ 4] >> 26) + (t[ 5] & 0x3ffffff);
    r[ 6] = (sp_digit)(t[ 5] >> 26) + (t[ 6] & 0x3ffffff);
    r[ 7] = (sp_digit)(t[ 6] >> 26) + (t[ 7] & 0x3ffffff);
    r[ 8] = (sp_digit)(t[ 7] >> 26) + (t[ 8] & 0x3ffffff);
    r[ 9] = (sp_digit)(t[ 8] >> 26) + (t[ 9] & 0x3ffffff);
    r[10] = (sp_digit)(t[ 9] >> 26);
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
static int sp_256_div_10(sp_digit* a, sp_digit* d, sp_digit* m,
        sp_digit* r)
{
    int i;
    int64_t d1;
    sp_digit div, r1;
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* td;
#else
    sp_digit t1d[20], t2d[10 + 1];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    td = XMALLOC(sizeof(sp_digit) * (3 * 10 + 1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (td != NULL) {
        t1 = td;
        t2 = td + 2 * 10;
    }
    else
        err = MEMORY_E;
#else
    t1 = t1d;
    t2 = t2d;
#endif

    (void)m;

    if (err == MP_OKAY) {
        div = d[9];
        XMEMCPY(t1, a, sizeof(*t1) * 2 * 10);
        for (i=9; i>=0; i--) {
            t1[10 + i] += t1[10 + i - 1] >> 26;
            t1[10 + i - 1] &= 0x3ffffff;
            d1 = t1[10 + i];
            d1 <<= 26;
            d1 += t1[10 + i - 1];
            r1 = (sp_digit)(d1 / div);

            sp_256_mul_d_10(t2, d, r1);
            sp_256_sub_10(&t1[i], &t1[i], t2);
            t1[10 + i] -= t2[10];
            t1[10 + i] += t1[10 + i - 1] >> 26;
            t1[10 + i - 1] &= 0x3ffffff;
            r1 = (((-t1[10 + i]) << 26) - t1[10 + i - 1]) / div;
            r1++;
            sp_256_mul_d_10(t2, d, r1);
            sp_256_add_10(&t1[i], &t1[i], t2);
            t1[10 + i] += t1[10 + i - 1] >> 26;
            t1[10 + i - 1] &= 0x3ffffff;
        }
        t1[10 - 1] += t1[10 - 2] >> 26;
        t1[10 - 2] &= 0x3ffffff;
        d1 = t1[10 - 1];
        r1 = (sp_digit)(d1 / div);

        sp_256_mul_d_10(t2, d, r1);
        sp_256_sub_10(t1, t1, t2);
        XMEMCPY(r, t1, sizeof(*r) * 2 * 10);
        for (i=0; i<8; i++) {
            r[i+1] += r[i] >> 26;
            r[i] &= 0x3ffffff;
        }
        sp_256_cond_add_10(r, r, d, 0 - (r[9] < 0));
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
static int sp_256_mod_10(sp_digit* r, sp_digit* a, sp_digit* m)
{
    return sp_256_div_10(a, m, NULL, r);
}

#endif
#if defined(HAVE_ECC_SIGN) || defined(HAVE_ECC_VERIFY)
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
static void sp_256_mont_mul_order_10(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_10(r, a, b);
    sp_256_mont_reduce_10(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_10(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_10(r, a);
    sp_256_mont_reduce_10(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_10(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_10(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_10(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_10(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 10);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_10(t, t);
        if (p256_order_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_10(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 10);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 10;
    sp_digit* t3 = td + 4 * 10;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_10(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_10(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_10(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_10(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_10(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_10(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_10(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_10(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_10(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_10(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_10(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_10(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_10(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_10(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_10(t2, t2, 4);
    sp_256_mont_mul_order_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_10(t2, t2, 4);
    sp_256_mont_mul_order_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_10(t2, t2, 4);
    sp_256_mont_mul_order_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_10(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_10(r, t2, t3);
#endif /* WOLFSSL_SP_SMALL */
}

#ifdef HAVE_INTEL_AVX2
/* Multiply two number mod the order of P256 curve. (r = a * b mod order)
 *
 * r  Result of the multiplication.
 * a  First operand of the multiplication.
 * b  Second operand of the multiplication.
 */
static void sp_256_mont_mul_order_avx2_10(sp_digit* r, sp_digit* a, sp_digit* b)
{
    sp_256_mul_avx2_10(r, a, b);
    sp_256_mont_reduce_avx2_10(r, p256_order, p256_mp_order);
}

/* Square number mod the order of P256 curve. (r = a * a mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_order_avx2_10(sp_digit* r, sp_digit* a)
{
    sp_256_sqr_avx2_10(r, a);
    sp_256_mont_reduce_avx2_10(r, p256_order, p256_mp_order);
}

#ifndef WOLFSSL_SP_SMALL
/* Square number mod the order of P256 curve a number of times.
 * (r = a ^ n mod order)
 *
 * r  Result of the squaring.
 * a  Number to square.
 */
static void sp_256_mont_sqr_n_order_avx2_10(sp_digit* r, sp_digit* a, int n)
{
    int i;

    sp_256_mont_sqr_order_avx2_10(r, a);
    for (i=1; i<n; i++)
        sp_256_mont_sqr_order_avx2_10(r, r);
}
#endif /* !WOLFSSL_SP_SMALL */

/* Invert the number, in Montgomery form, modulo the order of the P256 curve.
 * (r = 1 / a mod order)
 *
 * r   Inverse result.
 * a   Number to invert.
 * td  Temporary data.
 */
static void sp_256_mont_inv_order_avx2_10(sp_digit* r, sp_digit* a,
        sp_digit* td)
{
#ifdef WOLFSSL_SP_SMALL
    sp_digit* t = td;
    int i;

    XMEMCPY(t, a, sizeof(sp_digit) * 10);
    for (i=254; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_10(t, t);
        if (p256_order_2[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_10(t, t, a);
    }
    XMEMCPY(r, t, sizeof(sp_digit) * 10);
#else
    sp_digit* t = td;
    sp_digit* t2 = td + 2 * 10;
    sp_digit* t3 = td + 4 * 10;
    int i;

    /* t = a^2 */
    sp_256_mont_sqr_order_avx2_10(t, a);
    /* t = a^3 = t * a */
    sp_256_mont_mul_order_avx2_10(t, t, a);
    /* t2= a^c = t ^ 2 ^ 2 */
    sp_256_mont_sqr_n_order_avx2_10(t2, t, 2);
    /* t3= a^f = t2 * t */
    sp_256_mont_mul_order_avx2_10(t3, t2, t);
    /* t2= a^f0 = t3 ^ 2 ^ 4 */
    sp_256_mont_sqr_n_order_avx2_10(t2, t3, 4);
    /* t = a^ff = t2 * t3 */
    sp_256_mont_mul_order_avx2_10(t, t2, t3);
    /* t3= a^ff00 = t ^ 2 ^ 8 */
    sp_256_mont_sqr_n_order_avx2_10(t2, t, 8);
    /* t = a^ffff = t2 * t */
    sp_256_mont_mul_order_avx2_10(t, t2, t);
    /* t2= a^ffff0000 = t ^ 2 ^ 16 */
    sp_256_mont_sqr_n_order_avx2_10(t2, t, 16);
    /* t = a^ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_10(t, t2, t);
    /* t2= a^ffffffff0000000000000000 = t ^ 2 ^ 64  */
    sp_256_mont_sqr_n_order_avx2_10(t2, t, 64);
    /* t2= a^ffffffff00000000ffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_10(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffff00000000 = t2 ^ 2 ^ 32  */
    sp_256_mont_sqr_n_order_avx2_10(t2, t2, 32);
    /* t2= a^ffffffff00000000ffffffffffffffff = t2 * t */
    sp_256_mont_mul_order_avx2_10(t2, t2, t);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6 */
    for (i=127; i>=112; i--) {
        sp_256_mont_sqr_order_avx2_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6f */
    sp_256_mont_sqr_n_order_avx2_10(t2, t2, 4);
    sp_256_mont_mul_order_avx2_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84 */
    for (i=107; i>=64; i--) {
        sp_256_mont_sqr_order_avx2_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f */
    sp_256_mont_sqr_n_order_avx2_10(t2, t2, 4);
    sp_256_mont_mul_order_avx2_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2 */
    for (i=59; i>=32; i--) {
        sp_256_mont_sqr_order_avx2_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2f */
    sp_256_mont_sqr_n_order_avx2_10(t2, t2, 4);
    sp_256_mont_mul_order_avx2_10(t2, t2, t3);
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254 */
    for (i=27; i>=0; i--) {
        sp_256_mont_sqr_order_avx2_10(t2, t2);
        if (p256_order_low[i / 32] & ((sp_digit)1 << (i % 32)))
            sp_256_mont_mul_order_avx2_10(t2, t2, a);
    }
    /* t2= a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632540 */
    sp_256_mont_sqr_n_order_avx2_10(t2, t2, 4);
    /* r = a^ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc63254f */
    sp_256_mont_mul_order_avx2_10(r, t2, t3);
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
    sp_digit ed[2*10];
    sp_digit xd[2*10];
    sp_digit kd[2*10];
    sp_digit rd[2*10];
    sp_digit td[3 * 2*10];
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
        d = XMALLOC(sizeof(sp_digit) * 7 * 2 * 10, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            e = d + 0 * 10;
            x = d + 2 * 10;
            k = d + 4 * 10;
            r = d + 6 * 10;
            tmp = d + 8 * 10;
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

        sp_256_from_bin(e, 10, hash, hashLen);
        sp_256_from_mp(x, 10, priv);
    }

    for (i = SP_ECC_MAX_SIG_GEN; err == MP_OKAY && i > 0; i--) {
        /* New random point. */
        err = sp_256_ecc_gen_k_10(rng, k);
        if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                err = sp_256_ecc_mulmod_base_avx2_10(point, k, 1, heap);
            else
#endif
                err = sp_256_ecc_mulmod_base_10(point, k, 1, NULL);
        }

        if (err == MP_OKAY) {
            /* r = point->x mod order */
            XMEMCPY(r, point->x, sizeof(sp_digit) * 10);
            sp_256_norm_10(r);
            c = sp_256_cmp_10(r, p256_order);
            sp_256_cond_sub_10(r, r, p256_order, 0 - (c >= 0));
            sp_256_norm_10(r);

            /* Conv k to Montgomery form (mod order) */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_10(k, k, p256_norm_order);
            else
#endif
                sp_256_mul_10(k, k, p256_norm_order);
            err = sp_256_mod_10(k, k, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_10(k);
            /* kInv = 1/k mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_inv_order_avx2_10(kInv, k, tmp);
            else
#endif
                sp_256_mont_inv_order_10(kInv, k, tmp);
            sp_256_norm_10(kInv);

            /* s = r * x + e */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mul_avx2_10(x, x, r);
            else
#endif
                sp_256_mul_10(x, x, r);
            err = sp_256_mod_10(x, x, p256_order);
        }
        if (err == MP_OKAY) {
            sp_256_norm_10(x);
            carry = sp_256_add_10(s, e, x);
            sp_256_cond_sub_10(s, s, p256_order, 0 - carry);
            sp_256_norm_10(s);
            c = sp_256_cmp_10(s, p256_order);
            sp_256_cond_sub_10(s, s, p256_order, 0 - (c >= 0));
            sp_256_norm_10(s);

            /* s = s * k^-1 mod order */
#ifdef HAVE_INTEL_AVX2
            if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
                sp_256_mont_mul_order_avx2_10(s, s, kInv);
            else
#endif
                sp_256_mont_mul_order_10(s, s, kInv);
            sp_256_norm_10(s);

            /* Check that signature is usable. */
            if (!sp_256_iszero_10(s))
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
        XMEMSET(d, 0, sizeof(sp_digit) * 8 * 10);
        XFREE(d, heap, DYNAMIC_TYPE_ECC);
    }
#else
    XMEMSET(e, 0, sizeof(sp_digit) * 2 * 10);
    XMEMSET(x, 0, sizeof(sp_digit) * 2 * 10);
    XMEMSET(k, 0, sizeof(sp_digit) * 2 * 10);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 10);
    XMEMSET(r, 0, sizeof(sp_digit) * 2 * 10);
    XMEMSET(tmp, 0, sizeof(sp_digit) * 3 * 2*10);
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
    sp_digit u1d[2*10];
    sp_digit u2d[2*10];
    sp_digit sd[2*10];
    sp_digit tmpd[2*10 * 5];
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
        d = XMALLOC(sizeof(sp_digit) * 16 * 10, heap, DYNAMIC_TYPE_ECC);
        if (d != NULL) {
            u1  = d + 0 * 10;
            u2  = d + 2 * 10;
            s   = d + 4 * 10;
            tmp = d + 6 * 10;
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

        sp_256_from_bin(u1, 10, hash, hashLen);
        sp_256_from_mp(u2, 10, r);
        sp_256_from_mp(s, 10, sm);
        sp_256_from_mp(p2->x, 10, pX);
        sp_256_from_mp(p2->y, 10, pY);
        sp_256_from_mp(p2->z, 10, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_mul_avx2_10(s, s, p256_norm_order);
        else
#endif
            sp_256_mul_10(s, s, p256_norm_order);
        err = sp_256_mod_10(s, s, p256_order);
    }
    if (err == MP_OKAY) {
        sp_256_norm_10(s);
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_inv_order_avx2_10(s, s, tmp);
            sp_256_mont_mul_order_avx2_10(u1, u1, s);
            sp_256_mont_mul_order_avx2_10(u2, u2, s);
        }
        else
#endif
        {
            sp_256_mont_inv_order_10(s, s, tmp);
            sp_256_mont_mul_order_10(u1, u1, s);
            sp_256_mont_mul_order_10(u2, u2, s);
        }

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_10(p1, u1, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_10(p1, u1, 0, heap);
    }
    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_10(p2, p2, u2, 0, heap);
        else
#endif
            err = sp_256_ecc_mulmod_10(p2, p2, u2, 0, heap);
    }

    if (err == MP_OKAY) {
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_10(p1, p1, p2, tmp);
        else
#endif
            sp_256_proj_point_add_10(p1, p1, p2, tmp);

        /* (r + n*order).z'.z' mod prime == (u1.G + u2.Q)->x' */
        /* Reload r and convert to Montgomery form. */
        sp_256_from_mp(u2, 10, r);
        err = sp_256_mod_mul_norm_10(u2, u2, p256_mod);
    }

    if (err == MP_OKAY) {
        /* u1 = r.z'.z' mod prime */
        sp_256_mont_sqr_10(p1->z, p1->z, p256_mod, p256_mp_mod);
        sp_256_mont_mul_10(u1, u2, p1->z, p256_mod, p256_mp_mod);
        *res = sp_256_cmp_10(p1->x, u1) == 0;
        if (*res == 0) {
            /* Reload r and add order. */
            sp_256_from_mp(u2, 10, r);
            carry = sp_256_add_10(u2, u2, p256_order);
            /* Carry means result is greater than mod and is not valid. */
            if (!carry) {
                sp_256_norm_10(u2);

                /* Compare with mod and if greater or equal then not valid. */
                c = sp_256_cmp_10(u2, p256_mod);
                if (c < 0) {
                    /* Convert to Montogomery form */
                    err = sp_256_mod_mul_norm_10(u2, u2, p256_mod);
                    if (err == MP_OKAY) {
                        /* u1 = (r + 1*order).z'.z' mod prime */
                        sp_256_mont_mul_10(u1, u2, p1->z, p256_mod,
                                                                  p256_mp_mod);
                        *res = sp_256_cmp_10(p1->x, u2) == 0;
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
static int sp_256_ecc_is_point_10(sp_point* point, void* heap)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d = NULL;
#else
    sp_digit t1d[2*10];
    sp_digit t2d[2*10];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 10 * 4, heap, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 10;
        t2 = d + 2 * 10;
    }
    else
        err = MEMORY_E;
#else
    (void)heap;

    t1 = t1d;
    t2 = t2d;
#endif

    if (err == MP_OKAY) {
        sp_256_sqr_10(t1, point->y);
        sp_256_mod_10(t1, t1, p256_mod);
        sp_256_sqr_10(t2, point->x);
        sp_256_mod_10(t2, t2, p256_mod);
        sp_256_mul_10(t2, t2, point->x);
        sp_256_mod_10(t2, t2, p256_mod);
	sp_256_sub_10(t2, p256_mod, t2);
        sp_256_mont_add_10(t1, t1, t2, p256_mod);

        sp_256_mont_add_10(t1, t1, point->x, p256_mod);
        sp_256_mont_add_10(t1, t1, point->x, p256_mod);
        sp_256_mont_add_10(t1, t1, point->x, p256_mod);

        if (sp_256_cmp_10(t1, p256_b) != 0)
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
        sp_256_from_mp(pub->x, 10, pX);
        sp_256_from_mp(pub->y, 10, pY);
        sp_256_from_bin(pub->z, 10, one, sizeof(one));

        err = sp_256_ecc_is_point_10(pub, NULL);
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
    sp_digit privd[10];
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
        priv = XMALLOC(sizeof(sp_digit) * 10, heap, DYNAMIC_TYPE_ECC);
        if (priv == NULL)
            err = MEMORY_E;
    }
#else
    priv = privd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(pub->x, 10, pX);
        sp_256_from_mp(pub->y, 10, pY);
        sp_256_from_bin(pub->z, 10, one, sizeof(one));
        sp_256_from_mp(priv, 10, privm);

        /* Check point at infinitiy. */
        if (sp_256_iszero_10(pub->x) &&
            sp_256_iszero_10(pub->y))
            err = ECC_INF_E;
    }

    if (err == MP_OKAY) {
        /* Check range of X and Y */
        if (sp_256_cmp_10(pub->x, p256_mod) >= 0 ||
            sp_256_cmp_10(pub->y, p256_mod) >= 0)
            err = ECC_OUT_OF_RANGE_E;
    }

    if (err == MP_OKAY) {
        /* Check point is on curve */
        err = sp_256_ecc_is_point_10(pub, heap);
    }

    if (err == MP_OKAY) {
        /* Point * order = infinity */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_avx2_10(p, pub, p256_order, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_10(p, pub, p256_order, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is infinity */
        if (!sp_256_iszero_10(p->x) ||
            !sp_256_iszero_10(p->y)) {
            err = ECC_INF_E;
        }
    }

    if (err == MP_OKAY) {
        /* Base * private = point */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            err = sp_256_ecc_mulmod_base_avx2_10(p, priv, 1, heap);
        else
#endif
            err = sp_256_ecc_mulmod_base_10(p, priv, 1, heap);
    }
    if (err == MP_OKAY) {
        /* Check result is public key */
        if (sp_256_cmp_10(p->x, pub->x) != 0 ||
            sp_256_cmp_10(p->y, pub->y) != 0) {
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
    sp_digit tmpd[2 * 10 * 5];
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
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 10 * 5, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 10, pX);
        sp_256_from_mp(p->y, 10, pY);
        sp_256_from_mp(p->z, 10, pZ);
        sp_256_from_mp(q->x, 10, qX);
        sp_256_from_mp(q->y, 10, qY);
        sp_256_from_mp(q->z, 10, qZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_add_avx2_10(p, p, q, tmp);
        else
#endif
            sp_256_proj_point_add_10(p, p, q, tmp);
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
    sp_digit tmpd[2 * 10 * 2];
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
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 10 * 2, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 10, pX);
        sp_256_from_mp(p->y, 10, pY);
        sp_256_from_mp(p->z, 10, pZ);

#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags))
            sp_256_proj_point_dbl_avx2_10(p, p, tmp);
        else
#endif
            sp_256_proj_point_dbl_10(p, p, tmp);
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
    sp_digit tmpd[2 * 10 * 4];
    sp_point pd;
#endif
    sp_digit* tmp;
    sp_point* p;
    int err;

    err = sp_ecc_point_new(NULL, pd, p);
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    if (err == MP_OKAY) {
        tmp = XMALLOC(sizeof(sp_digit) * 2 * 10 * 4, NULL, DYNAMIC_TYPE_ECC);
        if (tmp == NULL)
            err = MEMORY_E;
    }
#else
    tmp = tmpd;
#endif
    if (err == MP_OKAY) {
        sp_256_from_mp(p->x, 10, pX);
        sp_256_from_mp(p->y, 10, pY);
        sp_256_from_mp(p->z, 10, pZ);

        sp_256_map_10(p, p, tmp);
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
static int sp_256_mont_sqrt_10(sp_digit* y)
{
#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    sp_digit* d;
#else
    sp_digit t1d[2 * 10];
    sp_digit t2d[2 * 10];
#endif
    sp_digit* t1;
    sp_digit* t2;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 10, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        t1 = d + 0 * 10;
        t2 = d + 2 * 10;
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
            sp_256_mont_sqr_avx2_10(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_avx2_10(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_avx2_10(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_avx2_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_avx2_10(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_avx2_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_avx2_10(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_avx2_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_avx2_10(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_avx2_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_avx2_10(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_avx2_10(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_avx2_10(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_avx2_10(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_avx2_10(y, t1, 94, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            /* t2 = y ^ 0x2 */
            sp_256_mont_sqr_10(t2, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0x3 */
            sp_256_mont_mul_10(t1, t2, y, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xc */
            sp_256_mont_sqr_n_10(t2, t1, 2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xf */
            sp_256_mont_mul_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xf0 */
            sp_256_mont_sqr_n_10(t2, t1, 4, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xff */
            sp_256_mont_mul_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xff00 */
            sp_256_mont_sqr_n_10(t2, t1, 8, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffff */
            sp_256_mont_mul_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t2 = y ^ 0xffff0000 */
            sp_256_mont_sqr_n_10(t2, t1, 16, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff */
            sp_256_mont_mul_10(t1, t1, t2, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000000 */
            sp_256_mont_sqr_n_10(t1, t1, 32, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001 */
            sp_256_mont_mul_10(t1, t1, y, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000000 */
            sp_256_mont_sqr_n_10(t1, t1, 96, p256_mod, p256_mp_mod);
            /* t1 = y ^ 0xffffffff00000001000000000000000000000001 */
            sp_256_mont_mul_10(t1, t1, y, p256_mod, p256_mp_mod);
            sp_256_mont_sqr_n_10(y, t1, 94, p256_mod, p256_mp_mod);
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
    sp_digit xd[2 * 10];
    sp_digit yd[2 * 10];
#endif
    sp_digit* x;
    sp_digit* y;
    int err = MP_OKAY;
#ifdef HAVE_INTEL_AVX2
    word32 cpuid_flags = cpuid_get_flags();
#endif

#if defined(WOLFSSL_SP_SMALL) || defined(WOLFSSL_SMALL_STACK)
    d = XMALLOC(sizeof(sp_digit) * 4 * 10, NULL, DYNAMIC_TYPE_ECC);
    if (d != NULL) {
        x = d + 0 * 10;
        y = d + 2 * 10;
    }
    else
        err = MEMORY_E;
#else
    x = xd;
    y = yd;
#endif

    if (err == MP_OKAY) {
        sp_256_from_mp(x, 10, xm);

        err = sp_256_mod_mul_norm_10(x, x, p256_mod);
    }

    if (err == MP_OKAY) {
        /* y = x^3 */
#ifdef HAVE_INTEL_AVX2
        if (IS_INTEL_BMI2(cpuid_flags) && IS_INTEL_ADX(cpuid_flags)) {
            sp_256_mont_sqr_avx2_10(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_avx2_10(y, y, x, p256_mod, p256_mp_mod);
        }
        else
#endif
        {
            sp_256_mont_sqr_10(y, x, p256_mod, p256_mp_mod);
            sp_256_mont_mul_10(y, y, x, p256_mod, p256_mp_mod);
        }
        /* y = x^3 - 3x */
        sp_256_mont_sub_10(y, y, x, p256_mod);
        sp_256_mont_sub_10(y, y, x, p256_mod);
        sp_256_mont_sub_10(y, y, x, p256_mod);
        /* y = x^3 - 3x + b */
        err = sp_256_mod_mul_norm_10(x, p256_b, p256_mod);
    }
    if (err == MP_OKAY) {
        sp_256_mont_add_10(y, y, x, p256_mod);
        /* y = sqrt(x^3 - 3x + b) */
        err = sp_256_mont_sqrt_10(y);
    }
    if (err == MP_OKAY) {
        XMEMSET(y + 10, 0, 10 * sizeof(sp_digit));
        sp_256_mont_reduce_10(y, p256_mod, p256_mp_mod);
        if (((y[0] ^ odd) & 1) != 0)
            sp_256_mont_sub_10(y, p256_mod, y, p256_mod);

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
#endif /* SP_WORD_SIZE == 32 */
#endif /* !WOLFSSL_SP_ASM */
#endif /* WOLFSSL_HAVE_SP_ECC */
#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_ECC */
