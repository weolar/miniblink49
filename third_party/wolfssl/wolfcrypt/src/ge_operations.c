/* ge_operations.c
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


 /* Based On Daniel J Bernstein's ed25519 Public Domain ref10 work. */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ED25519
#ifndef ED25519_SMALL /* run when not defined to use small memory math */

#include <wolfssl/wolfcrypt/ge_operations.h>
#include <wolfssl/wolfcrypt/ed25519.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

static void ge_p2_0(ge_p2 *);
#ifndef CURVED25519_X64
static void ge_precomp_0(ge_precomp *);
#endif
static void ge_p3_to_p2(ge_p2 *,const ge_p3 *);
static void ge_p3_to_cached(ge_cached *,const ge_p3 *);
static void ge_p1p1_to_p2(ge_p2 *,const ge_p1p1 *);
static void ge_p1p1_to_p3(ge_p3 *,const ge_p1p1 *);
static void ge_p2_dbl(ge_p1p1 *,const ge_p2 *);
static void ge_p3_dbl(ge_p1p1 *,const ge_p3 *);

static void ge_madd(ge_p1p1 *,const ge_p3 *,const ge_precomp *);
static void ge_msub(ge_p1p1 *,const ge_p3 *,const ge_precomp *);
static void ge_add(ge_p1p1 *,const ge_p3 *,const ge_cached *);
static void ge_sub(ge_p1p1 *,const ge_p3 *,const ge_cached *);

/*
ge means group element.

Here the group is the set of pairs (x,y) of field elements (see ge_operations.h)
satisfying -x^2 + y^2 = 1 + d x^2y^2
where d = -121665/121666.

Representations:
  ge_p2 (projective): (X:Y:Z) satisfying x=X/Z, y=Y/Z
  ge_p3 (extended): (X:Y:Z:T) satisfying x=X/Z, y=Y/Z, XY=ZT
  ge_p1p1 (completed): ((X:Z),(Y:T)) satisfying x=X/Z, y=Y/T
  ge_precomp (Duif): (y+x,y-x,2dxy)
*/

#if !defined(HAVE___UINT128_T) || defined(NO_CURVED25519_128BIT)
#define MASK_21     0x1fffff
#define ORDER_0     0x15d3ed
#define ORDER_1     0x18d2e7
#define ORDER_2     0x160498
#define ORDER_3     0xf39ac
#define ORDER_4     0x1dea2f
#define ORDER_5     0xa6f7c

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/
void sc_reduce(byte* s)
{
    int64_t t[24];
    int64_t carry;

    t[ 0] = MASK_21 & (load_3(s +  0) >> 0);
    t[ 1] = MASK_21 & (load_4(s +  2) >> 5);
    t[ 2] = MASK_21 & (load_3(s +  5) >> 2);
    t[ 3] = MASK_21 & (load_4(s +  7) >> 7);
    t[ 4] = MASK_21 & (load_4(s + 10) >> 4);
    t[ 5] = MASK_21 & (load_3(s + 13) >> 1);
    t[ 6] = MASK_21 & (load_4(s + 15) >> 6);
    t[ 7] = MASK_21 & (load_3(s + 18) >> 3);
    t[ 8] = MASK_21 & (load_3(s + 21) >> 0);
    t[ 9] = MASK_21 & (load_4(s + 23) >> 5);
    t[10] = MASK_21 & (load_3(s + 26) >> 2);
    t[11] = MASK_21 & (load_4(s + 28) >> 7);
    t[12] = MASK_21 & (load_4(s + 31) >> 4);
    t[13] = MASK_21 & (load_3(s + 34) >> 1);
    t[14] = MASK_21 & (load_4(s + 36) >> 6);
    t[15] = MASK_21 & (load_3(s + 39) >> 3);
    t[16] = MASK_21 & (load_3(s + 42) >> 0);
    t[17] = MASK_21 & (load_4(s + 44) >> 5);
    t[18] = MASK_21 & (load_3(s + 47) >> 2);
    t[19] = MASK_21 & (load_4(s + 49) >> 7);
    t[20] = MASK_21 & (load_4(s + 52) >> 4);
    t[21] = MASK_21 & (load_3(s + 55) >> 1);
    t[22] = MASK_21 & (load_4(s + 57) >> 6);
    t[23] =           (load_4(s + 60) >> 3);

    t[11] -= t[23] * ORDER_0;
    t[12] -= t[23] * ORDER_1;
    t[13] -= t[23] * ORDER_2;
    t[14] -= t[23] * ORDER_3;
    t[15] -= t[23] * ORDER_4;
    t[16] -= t[23] * ORDER_5;

    t[10] -= t[22] * ORDER_0;
    t[11] -= t[22] * ORDER_1;
    t[12] -= t[22] * ORDER_2;
    t[13] -= t[22] * ORDER_3;
    t[14] -= t[22] * ORDER_4;
    t[15] -= t[22] * ORDER_5;

    t[ 9] -= t[21] * ORDER_0;
    t[10] -= t[21] * ORDER_1;
    t[11] -= t[21] * ORDER_2;
    t[12] -= t[21] * ORDER_3;
    t[13] -= t[21] * ORDER_4;
    t[14] -= t[21] * ORDER_5;

    t[ 8] -= t[20] * ORDER_0;
    t[ 9] -= t[20] * ORDER_1;
    t[10] -= t[20] * ORDER_2;
    t[11] -= t[20] * ORDER_3;
    t[12] -= t[20] * ORDER_4;
    t[13] -= t[20] * ORDER_5;

    t[ 7] -= t[19] * ORDER_0;
    t[ 8] -= t[19] * ORDER_1;
    t[ 9] -= t[19] * ORDER_2;
    t[10] -= t[19] * ORDER_3;
    t[11] -= t[19] * ORDER_4;
    t[12] -= t[19] * ORDER_5;

    t[ 6] -= t[18] * ORDER_0;
    t[ 7] -= t[18] * ORDER_1;
    t[ 8] -= t[18] * ORDER_2;
    t[ 9] -= t[18] * ORDER_3;
    t[10] -= t[18] * ORDER_4;
    t[11] -= t[18] * ORDER_5;

    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;

    t[ 5] -= t[17] * ORDER_0;
    t[ 6] -= t[17] * ORDER_1;
    t[ 7] -= t[17] * ORDER_2;
    t[ 8] -= t[17] * ORDER_3;
    t[ 9] -= t[17] * ORDER_4;
    t[10] -= t[17] * ORDER_5;

    t[ 4] -= t[16] * ORDER_0;
    t[ 5] -= t[16] * ORDER_1;
    t[ 6] -= t[16] * ORDER_2;
    t[ 7] -= t[16] * ORDER_3;
    t[ 8] -= t[16] * ORDER_4;
    t[ 9] -= t[16] * ORDER_5;

    t[ 3] -= t[15] * ORDER_0;
    t[ 4] -= t[15] * ORDER_1;
    t[ 5] -= t[15] * ORDER_2;
    t[ 6] -= t[15] * ORDER_3;
    t[ 7] -= t[15] * ORDER_4;
    t[ 8] -= t[15] * ORDER_5;

    t[ 2] -= t[14] * ORDER_0;
    t[ 3] -= t[14] * ORDER_1;
    t[ 4] -= t[14] * ORDER_2;
    t[ 5] -= t[14] * ORDER_3;
    t[ 6] -= t[14] * ORDER_4;
    t[ 7] -= t[14] * ORDER_5;

    t[ 1] -= t[13] * ORDER_0;
    t[ 2] -= t[13] * ORDER_1;
    t[ 3] -= t[13] * ORDER_2;
    t[ 4] -= t[13] * ORDER_3;
    t[ 5] -= t[13] * ORDER_4;
    t[ 6] -= t[13] * ORDER_5;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;
    t[12]  = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)((t[ 0] >> 16) | (t[ 1] <<  5));
    s[ 3] = (byte)(t[ 1] >>  3);
    s[ 4] = (byte)(t[ 1] >> 11);
    s[ 5] = (byte)((t[ 1] >> 19) | (t[ 2] <<  2));
    s[ 6] = (byte)(t[ 2] >>  6);
    s[ 7] = (byte)((t[ 2] >> 14) | (t[ 3] <<  7));
    s[ 8] = (byte)(t[ 3] >>  1);
    s[ 9] = (byte)(t[ 3] >>  9);
    s[10] = (byte)((t[ 3] >> 17) | (t[ 4] <<  4));
    s[11] = (byte)(t[ 4] >>  4);
    s[12] = (byte)(t[ 4] >> 12);
    s[13] = (byte)((t[ 4] >> 20) | (t[ 5] <<  1));
    s[14] = (byte)(t[ 5] >>  7);
    s[15] = (byte)((t[ 5] >> 15) | (t[ 6] <<  6));
    s[16] = (byte)(t[ 6] >>  2);
    s[17] = (byte)(t[ 6] >> 10);
    s[18] = (byte)((t[ 6] >> 18) | (t[ 7] <<  3));
    s[19] = (byte)(t[ 7] >>  5);
    s[20] = (byte)(t[ 7] >> 13);
    s[21] = (byte)(t[ 8] >>  0);
    s[22] = (byte)(t[ 8] >>  8);
    s[23] = (byte)((t[ 8] >> 16) | (t[ 9] <<  5));
    s[24] = (byte)(t[ 9] >>  3);
    s[25] = (byte)(t[ 9] >> 11);
    s[26] = (byte)((t[ 9] >> 19) | (t[10] <<  2));
    s[27] = (byte)(t[10] >>  6);
    s[28] = (byte)((t[10] >> 14) | (t[11] <<  7));
    s[29] = (byte)(t[11] >>  1);
    s[30] = (byte)(t[11] >>  9);
    s[31] = (byte)(t[11] >> 17);
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/
void sc_muladd(byte* s, const byte* a, const byte* b, const byte* c)
{
    uint32_t ad[12], bd[12], cd[12];
    int64_t t[24];
    int64_t carry;

    ad[ 0] = MASK_21 & (load_3(a +  0) >> 0);
    ad[ 1] = MASK_21 & (load_4(a +  2) >> 5);
    ad[ 2] = MASK_21 & (load_3(a +  5) >> 2);
    ad[ 3] = MASK_21 & (load_4(a +  7) >> 7);
    ad[ 4] = MASK_21 & (load_4(a + 10) >> 4);
    ad[ 5] = MASK_21 & (load_3(a + 13) >> 1);
    ad[ 6] = MASK_21 & (load_4(a + 15) >> 6);
    ad[ 7] = MASK_21 & (load_3(a + 18) >> 3);
    ad[ 8] = MASK_21 & (load_3(a + 21) >> 0);
    ad[ 9] = MASK_21 & (load_4(a + 23) >> 5);
    ad[10] = MASK_21 & (load_3(a + 26) >> 2);
    ad[11] = (uint32_t)(load_4(a + 28) >> 7);
    bd[ 0] = MASK_21 & (load_3(b +  0) >> 0);
    bd[ 1] = MASK_21 & (load_4(b +  2) >> 5);
    bd[ 2] = MASK_21 & (load_3(b +  5) >> 2);
    bd[ 3] = MASK_21 & (load_4(b +  7) >> 7);
    bd[ 4] = MASK_21 & (load_4(b + 10) >> 4);
    bd[ 5] = MASK_21 & (load_3(b + 13) >> 1);
    bd[ 6] = MASK_21 & (load_4(b + 15) >> 6);
    bd[ 7] = MASK_21 & (load_3(b + 18) >> 3);
    bd[ 8] = MASK_21 & (load_3(b + 21) >> 0);
    bd[ 9] = MASK_21 & (load_4(b + 23) >> 5);
    bd[10] = MASK_21 & (load_3(b + 26) >> 2);
    bd[11] = (uint32_t)(load_4(b + 28) >> 7);
    cd[ 0] = MASK_21 & (load_3(c +  0) >> 0);
    cd[ 1] = MASK_21 & (load_4(c +  2) >> 5);
    cd[ 2] = MASK_21 & (load_3(c +  5) >> 2);
    cd[ 3] = MASK_21 & (load_4(c +  7) >> 7);
    cd[ 4] = MASK_21 & (load_4(c + 10) >> 4);
    cd[ 5] = MASK_21 & (load_3(c + 13) >> 1);
    cd[ 6] = MASK_21 & (load_4(c + 15) >> 6);
    cd[ 7] = MASK_21 & (load_3(c + 18) >> 3);
    cd[ 8] = MASK_21 & (load_3(c + 21) >> 0);
    cd[ 9] = MASK_21 & (load_4(c + 23) >> 5);
    cd[10] = MASK_21 & (load_3(c + 26) >> 2);
    cd[11] = (uint32_t)(load_4(c + 28) >> 7);

    t[ 0] = cd[ 0] + (int64_t)ad[ 0] * bd[ 0];
    t[ 1] = cd[ 1] + (int64_t)ad[ 0] * bd[ 1] + (int64_t)ad[ 1] * bd[ 0];
    t[ 2] = cd[ 2] + (int64_t)ad[ 0] * bd[ 2] + (int64_t)ad[ 1] * bd[ 1] +
                     (int64_t)ad[ 2] * bd[ 0];
    t[ 3] = cd[ 3] + (int64_t)ad[ 0] * bd[ 3] + (int64_t)ad[ 1] * bd[ 2] +
                     (int64_t)ad[ 2] * bd[ 1] + (int64_t)ad[ 3] * bd[ 0];
    t[ 4] = cd[ 4] + (int64_t)ad[ 0] * bd[ 4] + (int64_t)ad[ 1] * bd[ 3] +
                     (int64_t)ad[ 2] * bd[ 2] + (int64_t)ad[ 3] * bd[ 1] +
                     (int64_t)ad[ 4] * bd[ 0];
    t[ 5] = cd[ 5] + (int64_t)ad[ 0] * bd[ 5] + (int64_t)ad[ 1] * bd[ 4] +
                     (int64_t)ad[ 2] * bd[ 3] + (int64_t)ad[ 3] * bd[ 2] +
                     (int64_t)ad[ 4] * bd[ 1] + (int64_t)ad[ 5] * bd[ 0];
    t[ 6] = cd[ 6] + (int64_t)ad[ 0] * bd[ 6] + (int64_t)ad[ 1] * bd[ 5] +
                     (int64_t)ad[ 2] * bd[ 4] + (int64_t)ad[ 3] * bd[ 3] +
                     (int64_t)ad[ 4] * bd[ 2] + (int64_t)ad[ 5] * bd[ 1] +
                     (int64_t)ad[ 6] * bd[ 0];
    t[ 7] = cd[ 7] + (int64_t)ad[ 0] * bd[ 7] + (int64_t)ad[ 1] * bd[ 6] +
                     (int64_t)ad[ 2] * bd[ 5] + (int64_t)ad[ 3] * bd[ 4] +
                     (int64_t)ad[ 4] * bd[ 3] + (int64_t)ad[ 5] * bd[ 2] +
                     (int64_t)ad[ 6] * bd[ 1] + (int64_t)ad[ 7] * bd[ 0];
    t[ 8] = cd[ 8] + (int64_t)ad[ 0] * bd[ 8] + (int64_t)ad[ 1] * bd[ 7] +
                     (int64_t)ad[ 2] * bd[ 6] + (int64_t)ad[ 3] * bd[ 5] +
                     (int64_t)ad[ 4] * bd[ 4] + (int64_t)ad[ 5] * bd[ 3] +
                     (int64_t)ad[ 6] * bd[ 2] + (int64_t)ad[ 7] * bd[ 1] +
                     (int64_t)ad[ 8] * bd[ 0];
    t[ 9] = cd[ 9] + (int64_t)ad[ 0] * bd[ 9] + (int64_t)ad[ 1] * bd[ 8] +
                     (int64_t)ad[ 2] * bd[ 7] + (int64_t)ad[ 3] * bd[ 6] +
                     (int64_t)ad[ 4] * bd[ 5] + (int64_t)ad[ 5] * bd[ 4] +
                     (int64_t)ad[ 6] * bd[ 3] + (int64_t)ad[ 7] * bd[ 2] +
                     (int64_t)ad[ 8] * bd[ 1] + (int64_t)ad[ 9] * bd[ 0];
    t[10] = cd[10] + (int64_t)ad[ 0] * bd[10] + (int64_t)ad[ 1] * bd[ 9] +
                     (int64_t)ad[ 2] * bd[ 8] + (int64_t)ad[ 3] * bd[ 7] +
                     (int64_t)ad[ 4] * bd[ 6] + (int64_t)ad[ 5] * bd[ 5] +
                     (int64_t)ad[ 6] * bd[ 4] + (int64_t)ad[ 7] * bd[ 3] +
                     (int64_t)ad[ 8] * bd[ 2] + (int64_t)ad[ 9] * bd[ 1] +
                     (int64_t)ad[10] * bd[ 0];
    t[11] = cd[11] + (int64_t)ad[ 0] * bd[11] + (int64_t)ad[ 1] * bd[10] +
                     (int64_t)ad[ 2] * bd[ 9] + (int64_t)ad[ 3] * bd[ 8] +
                     (int64_t)ad[ 4] * bd[ 7] + (int64_t)ad[ 5] * bd[ 6] +
                     (int64_t)ad[ 6] * bd[ 5] + (int64_t)ad[ 7] * bd[ 4] +
                     (int64_t)ad[ 8] * bd[ 3] + (int64_t)ad[ 9] * bd[ 2] +
                     (int64_t)ad[10] * bd[ 1] + (int64_t)ad[11] * bd[ 0];
    t[12] =          (int64_t)ad[ 1] * bd[11] + (int64_t)ad[ 2] * bd[10] +
                     (int64_t)ad[ 3] * bd[ 9] + (int64_t)ad[ 4] * bd[ 8] +
                     (int64_t)ad[ 5] * bd[ 7] + (int64_t)ad[ 6] * bd[ 6] +
                     (int64_t)ad[ 7] * bd[ 5] + (int64_t)ad[ 8] * bd[ 4] +
                     (int64_t)ad[ 9] * bd[ 3] + (int64_t)ad[10] * bd[ 2] +
                     (int64_t)ad[11] * bd[ 1];
    t[13] =          (int64_t)ad[ 2] * bd[11] + (int64_t)ad[ 3] * bd[10] +
                     (int64_t)ad[ 4] * bd[ 9] + (int64_t)ad[ 5] * bd[ 8] +
                     (int64_t)ad[ 6] * bd[ 7] + (int64_t)ad[ 7] * bd[ 6] +
                     (int64_t)ad[ 8] * bd[ 5] + (int64_t)ad[ 9] * bd[ 4] +
                     (int64_t)ad[10] * bd[ 3] + (int64_t)ad[11] * bd[ 2];
    t[14] =          (int64_t)ad[ 3] * bd[11] + (int64_t)ad[ 4] * bd[10] +
                     (int64_t)ad[ 5] * bd[ 9] + (int64_t)ad[ 6] * bd[ 8] +
                     (int64_t)ad[ 7] * bd[ 7] + (int64_t)ad[ 8] * bd[ 6] +
                     (int64_t)ad[ 9] * bd[ 5] + (int64_t)ad[10] * bd[ 4] +
                     (int64_t)ad[11] * bd[ 3];
    t[15] =          (int64_t)ad[ 4] * bd[11] + (int64_t)ad[ 5] * bd[10] +
                     (int64_t)ad[ 6] * bd[ 9] + (int64_t)ad[ 7] * bd[ 8] +
                     (int64_t)ad[ 8] * bd[ 7] + (int64_t)ad[ 9] * bd[ 6] +
                     (int64_t)ad[10] * bd[ 5] + (int64_t)ad[11] * bd[ 4];
    t[16] =          (int64_t)ad[ 5] * bd[11] + (int64_t)ad[ 6] * bd[10] +
                     (int64_t)ad[ 7] * bd[ 9] + (int64_t)ad[ 8] * bd[ 8] +
                     (int64_t)ad[ 9] * bd[ 7] + (int64_t)ad[10] * bd[ 6] +
                     (int64_t)ad[11] * bd[ 5];
    t[17] =          (int64_t)ad[ 6] * bd[11] + (int64_t)ad[ 7] * bd[10] +
                     (int64_t)ad[ 8] * bd[ 9] + (int64_t)ad[ 9] * bd[ 8] +
                     (int64_t)ad[10] * bd[ 7] + (int64_t)ad[11] * bd[ 6];
    t[18] =          (int64_t)ad[ 7] * bd[11] + (int64_t)ad[ 8] * bd[10] +
                     (int64_t)ad[ 9] * bd[ 9] + (int64_t)ad[10] * bd[ 8] +
                     (int64_t)ad[11] * bd[ 7];
    t[19] =          (int64_t)ad[ 8] * bd[11] + (int64_t)ad[ 9] * bd[10] +
                     (int64_t)ad[10] * bd[ 9] + (int64_t)ad[11] * bd[ 8];
    t[20] =          (int64_t)ad[ 9] * bd[11] + (int64_t)ad[10] * bd[10] +
                     (int64_t)ad[11] * bd[ 9];
    t[21] =          (int64_t)ad[10] * bd[11] + (int64_t)ad[11] * bd[10];
    t[22] =          (int64_t)ad[11] * bd[11];
    t[23] = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[18] >> 21; t[19] += carry; t[18] &= MASK_21;
    carry = t[20] >> 21; t[21] += carry; t[20] &= MASK_21;
    carry = t[22] >> 21; t[23] += carry; t[22] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;
    carry = t[17] >> 21; t[18] += carry; t[17] &= MASK_21;
    carry = t[19] >> 21; t[20] += carry; t[19] &= MASK_21;
    carry = t[21] >> 21; t[22] += carry; t[21] &= MASK_21;

    t[11] -= t[23] * ORDER_0;
    t[12] -= t[23] * ORDER_1;
    t[13] -= t[23] * ORDER_2;
    t[14] -= t[23] * ORDER_3;
    t[15] -= t[23] * ORDER_4;
    t[16] -= t[23] * ORDER_5;

    t[10] -= t[22] * ORDER_0;
    t[11] -= t[22] * ORDER_1;
    t[12] -= t[22] * ORDER_2;
    t[13] -= t[22] * ORDER_3;
    t[14] -= t[22] * ORDER_4;
    t[15] -= t[22] * ORDER_5;

    t[ 9] -= t[21] * ORDER_0;
    t[10] -= t[21] * ORDER_1;
    t[11] -= t[21] * ORDER_2;
    t[12] -= t[21] * ORDER_3;
    t[13] -= t[21] * ORDER_4;
    t[14] -= t[21] * ORDER_5;

    t[ 8] -= t[20] * ORDER_0;
    t[ 9] -= t[20] * ORDER_1;
    t[10] -= t[20] * ORDER_2;
    t[11] -= t[20] * ORDER_3;
    t[12] -= t[20] * ORDER_4;
    t[13] -= t[20] * ORDER_5;

    t[ 7] -= t[19] * ORDER_0;
    t[ 8] -= t[19] * ORDER_1;
    t[ 9] -= t[19] * ORDER_2;
    t[10] -= t[19] * ORDER_3;
    t[11] -= t[19] * ORDER_4;
    t[12] -= t[19] * ORDER_5;

    t[ 6] -= t[18] * ORDER_0;
    t[ 7] -= t[18] * ORDER_1;
    t[ 8] -= t[18] * ORDER_2;
    t[ 9] -= t[18] * ORDER_3;
    t[10] -= t[18] * ORDER_4;
    t[11] -= t[18] * ORDER_5;

    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[12] >> 21; t[13] += carry; t[12] &= MASK_21;
    carry = t[14] >> 21; t[15] += carry; t[14] &= MASK_21;
    carry = t[16] >> 21; t[17] += carry; t[16] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;
    carry = t[13] >> 21; t[14] += carry; t[13] &= MASK_21;
    carry = t[15] >> 21; t[16] += carry; t[15] &= MASK_21;

    t[ 5] -= t[17] * ORDER_0;
    t[ 6] -= t[17] * ORDER_1;
    t[ 7] -= t[17] * ORDER_2;
    t[ 8] -= t[17] * ORDER_3;
    t[ 9] -= t[17] * ORDER_4;
    t[10] -= t[17] * ORDER_5;

    t[ 4] -= t[16] * ORDER_0;
    t[ 5] -= t[16] * ORDER_1;
    t[ 6] -= t[16] * ORDER_2;
    t[ 7] -= t[16] * ORDER_3;
    t[ 8] -= t[16] * ORDER_4;
    t[ 9] -= t[16] * ORDER_5;

    t[ 3] -= t[15] * ORDER_0;
    t[ 4] -= t[15] * ORDER_1;
    t[ 5] -= t[15] * ORDER_2;
    t[ 6] -= t[15] * ORDER_3;
    t[ 7] -= t[15] * ORDER_4;
    t[ 8] -= t[15] * ORDER_5;

    t[ 2] -= t[14] * ORDER_0;
    t[ 3] -= t[14] * ORDER_1;
    t[ 4] -= t[14] * ORDER_2;
    t[ 5] -= t[14] * ORDER_3;
    t[ 6] -= t[14] * ORDER_4;
    t[ 7] -= t[14] * ORDER_5;

    t[ 1] -= t[13] * ORDER_0;
    t[ 2] -= t[13] * ORDER_1;
    t[ 3] -= t[13] * ORDER_2;
    t[ 4] -= t[13] * ORDER_3;
    t[ 5] -= t[13] * ORDER_4;
    t[ 6] -= t[13] * ORDER_5;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;
    t[12]  = 0;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;
    carry = t[11] >> 21; t[12] += carry; t[11] &= MASK_21;

    t[ 0] -= t[12] * ORDER_0;
    t[ 1] -= t[12] * ORDER_1;
    t[ 2] -= t[12] * ORDER_2;
    t[ 3] -= t[12] * ORDER_3;
    t[ 4] -= t[12] * ORDER_4;
    t[ 5] -= t[12] * ORDER_5;

    carry = t[ 0] >> 21; t[ 1] += carry; t[ 0] &= MASK_21;
    carry = t[ 1] >> 21; t[ 2] += carry; t[ 1] &= MASK_21;
    carry = t[ 2] >> 21; t[ 3] += carry; t[ 2] &= MASK_21;
    carry = t[ 3] >> 21; t[ 4] += carry; t[ 3] &= MASK_21;
    carry = t[ 4] >> 21; t[ 5] += carry; t[ 4] &= MASK_21;
    carry = t[ 5] >> 21; t[ 6] += carry; t[ 5] &= MASK_21;
    carry = t[ 6] >> 21; t[ 7] += carry; t[ 6] &= MASK_21;
    carry = t[ 7] >> 21; t[ 8] += carry; t[ 7] &= MASK_21;
    carry = t[ 8] >> 21; t[ 9] += carry; t[ 8] &= MASK_21;
    carry = t[ 9] >> 21; t[10] += carry; t[ 9] &= MASK_21;
    carry = t[10] >> 21; t[11] += carry; t[10] &= MASK_21;

    s[ 0] = (byte)(t[ 0] >>  0);
    s[ 1] = (byte)(t[ 0] >>  8);
    s[ 2] = (byte)((t[ 0] >> 16) | (t[ 1] <<  5));
    s[ 3] = (byte)(t[ 1] >>  3);
    s[ 4] = (byte)(t[ 1] >> 11);
    s[ 5] = (byte)((t[ 1] >> 19) | (t[ 2] <<  2));
    s[ 6] = (byte)(t[ 2] >>  6);
    s[ 7] = (byte)((t[ 2] >> 14) | (t[ 3] <<  7));
    s[ 8] = (byte)(t[ 3] >>  1);
    s[ 9] = (byte)(t[ 3] >>  9);
    s[10] = (byte)((t[ 3] >> 17) | (t[ 4] <<  4));
    s[11] = (byte)(t[ 4] >>  4);
    s[12] = (byte)(t[ 4] >> 12);
    s[13] = (byte)((t[ 4] >> 20) | (t[ 5] <<  1));
    s[14] = (byte)(t[ 5] >>  7);
    s[15] = (byte)((t[ 5] >> 15) | (t[ 6] <<  6));
    s[16] = (byte)(t[ 6] >>  2);
    s[17] = (byte)(t[ 6] >> 10);
    s[18] = (byte)((t[ 6] >> 18) | (t[ 7] <<  3));
    s[19] = (byte)(t[ 7] >>  5);
    s[20] = (byte)(t[ 7] >> 13);
    s[21] = (byte)(t[ 8] >>  0);
    s[22] = (byte)(t[ 8] >>  8);
    s[23] = (byte)((t[ 8] >> 16) | (t[ 9] <<  5));
    s[24] = (byte)(t[ 9] >>  3);
    s[25] = (byte)(t[ 9] >> 11);
    s[26] = (byte)((t[ 9] >> 19) | (t[10] <<  2));
    s[27] = (byte)(t[10] >>  6);
    s[28] = (byte)((t[10] >> 14) | (t[11] <<  7));
    s[29] = (byte)(t[11] >>  1);
    s[30] = (byte)(t[11] >>  9);
    s[31] = (byte)(t[11] >> 17);
}
#else
static uint64_t load_6(const byte* a)
{
    uint64_t n;
    n = ((uint64_t)a[0] <<  0) |
        ((uint64_t)a[1] <<  8) |
        ((uint64_t)a[2] << 16) |
        ((uint64_t)a[3] << 24) |
        ((uint64_t)a[4] << 32) |
        ((uint64_t)a[5] << 40);
    return n;
}

static uint64_t load_7(const byte* a)
{
    uint64_t n;
    n = ((uint64_t)a[0] <<  0) |
        ((uint64_t)a[1] <<  8) |
        ((uint64_t)a[2] << 16) |
        ((uint64_t)a[3] << 24) |
        ((uint64_t)a[4] << 32) |
        ((uint64_t)a[5] << 40) |
        ((uint64_t)a[6] << 48);
    return n;
}

#define MASK_42     0x3ffffffffffl
#define ORDER_0     0x31a5cf5d3edl
#define ORDER_1     0x1e735960498l
#define ORDER_2     0x14def9dea2fl

/*
Input:
  s[0]+256*s[1]+...+256^63*s[63] = s

Output:
  s[0]+256*s[1]+...+256^31*s[31] = s mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
  Overwrites s in place.
*/
void sc_reduce(byte* s)
{
    __int128_t t[12];
    __int128_t carry;

    t[ 0] = MASK_42 & (load_6(s +  0) >> 0);
    t[ 1] = MASK_42 & (load_6(s +  5) >> 2);
    t[ 2] = MASK_42 & (load_6(s + 10) >> 4);
    t[ 3] = MASK_42 & (load_6(s + 15) >> 6);
    t[ 4] = MASK_42 & (load_6(s + 21) >> 0);
    t[ 5] = MASK_42 & (load_6(s + 26) >> 2);
    t[ 6] = MASK_42 & (load_6(s + 31) >> 4);
    t[ 7] = MASK_42 & (load_6(s + 36) >> 6);
    t[ 8] = MASK_42 & (load_6(s + 42) >> 0);
    t[ 9] = MASK_42 & (load_6(s + 47) >> 2);
    t[10] = MASK_42 & (load_6(s + 52) >> 4);
    t[11] =           (load_7(s + 57) >> 6);

    t[ 5] -= t[11] * ORDER_0;
    t[ 6] -= t[11] * ORDER_1;
    t[ 7] -= t[11] * ORDER_2;

    t[ 4] -= t[10] * ORDER_0;
    t[ 5] -= t[10] * ORDER_1;
    t[ 6] -= t[10] * ORDER_2;

    t[ 3] -= t[ 9] * ORDER_0;
    t[ 4] -= t[ 9] * ORDER_1;
    t[ 5] -= t[ 9] * ORDER_2;

    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;

    t[ 2] -= t[ 8] * ORDER_0;
    t[ 3] -= t[ 8] * ORDER_1;
    t[ 4] -= t[ 8] * ORDER_2;

    t[ 1] -= t[ 7] * ORDER_0;
    t[ 2] -= t[ 7] * ORDER_1;
    t[ 3] -= t[ 7] * ORDER_2;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;
    t[ 6]  = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;

    s[ 0] = (t[ 0] >>  0);
    s[ 1] = (t[ 0] >>  8);
    s[ 2] = (t[ 0] >> 16);
    s[ 3] = (t[ 0] >> 24);
    s[ 4] = (t[ 0] >> 32);
    s[ 5] = (t[ 0] >> 40) | (t[ 1] <<  2);
    s[ 6] = (t[ 1] >>  6);
    s[ 7] = (t[ 1] >> 14);
    s[ 8] = (t[ 1] >> 22);
    s[ 9] = (t[ 1] >> 30);
    s[10] = (t[ 1] >> 38) | (t[ 2] <<  4);
    s[11] = (t[ 2] >>  4);
    s[12] = (t[ 2] >> 12);
    s[13] = (t[ 2] >> 20);
    s[14] = (t[ 2] >> 28);
    s[15] = (t[ 2] >> 36) | (t[ 3] <<  6);
    s[16] = (t[ 3] >>  2);
    s[17] = (t[ 3] >> 10);
    s[18] = (t[ 3] >> 18);
    s[19] = (t[ 3] >> 26);
    s[20] = (t[ 3] >> 34);
    s[21] = (t[ 4] >>  0);
    s[22] = (t[ 4] >>  8);
    s[23] = (t[ 4] >> 16);
    s[24] = (t[ 4] >> 24);
    s[25] = (t[ 4] >> 32);
    s[26] = (t[ 4] >> 40) | (t[ 5] <<  2);
    s[27] = (t[ 5] >>  6);
    s[28] = (t[ 5] >> 14);
    s[29] = (t[ 5] >> 22);
    s[30] = (t[ 5] >> 30);
    s[31] = (t[ 5] >> 38);
}

/*
Input:
  a[0]+256*a[1]+...+256^31*a[31] = a
  b[0]+256*b[1]+...+256^31*b[31] = b
  c[0]+256*c[1]+...+256^31*c[31] = c

Output:
  s[0]+256*s[1]+...+256^31*s[31] = (ab+c) mod l
  where l = 2^252 + 27742317777372353535851937790883648493.
*/
void sc_muladd(byte* s, const byte* a, const byte* b, const byte* c)
{
    uint64_t ad[6], bd[6], cd[6];
    __int128_t t[12];
    __int128_t carry;

    ad[ 0] = MASK_42 & (load_6(a +  0) >> 0);
    ad[ 1] = MASK_42 & (load_6(a +  5) >> 2);
    ad[ 2] = MASK_42 & (load_6(a + 10) >> 4);
    ad[ 3] = MASK_42 & (load_6(a + 15) >> 6);
    ad[ 4] = MASK_42 & (load_6(a + 21) >> 0);
    ad[ 5] =           (load_6(a + 26) >> 2);
    bd[ 0] = MASK_42 & (load_6(b +  0) >> 0);
    bd[ 1] = MASK_42 & (load_6(b +  5) >> 2);
    bd[ 2] = MASK_42 & (load_6(b + 10) >> 4);
    bd[ 3] = MASK_42 & (load_6(b + 15) >> 6);
    bd[ 4] = MASK_42 & (load_6(b + 21) >> 0);
    bd[ 5] =           (load_6(b + 26) >> 2);
    cd[ 0] = MASK_42 & (load_6(c +  0) >> 0);
    cd[ 1] = MASK_42 & (load_6(c +  5) >> 2);
    cd[ 2] = MASK_42 & (load_6(c + 10) >> 4);
    cd[ 3] = MASK_42 & (load_6(c + 15) >> 6);
    cd[ 4] = MASK_42 & (load_6(c + 21) >> 0);
    cd[ 5] =           (load_6(c + 26) >> 2);

    t[ 0] = cd[ 0] + (__int128_t)ad[ 0] * bd[ 0];
    t[ 1] = cd[ 1] + (__int128_t)ad[ 0] * bd[ 1] + (__int128_t)ad[ 1] * bd[ 0];
    t[ 2] = cd[ 2] + (__int128_t)ad[ 0] * bd[ 2] + (__int128_t)ad[ 1] * bd[ 1] +
                     (__int128_t)ad[ 2] * bd[ 0];
    t[ 3] = cd[ 3] + (__int128_t)ad[ 0] * bd[ 3] + (__int128_t)ad[ 1] * bd[ 2] +
                     (__int128_t)ad[ 2] * bd[ 1] + (__int128_t)ad[ 3] * bd[ 0];
    t[ 4] = cd[ 4] + (__int128_t)ad[ 0] * bd[ 4] + (__int128_t)ad[ 1] * bd[ 3] +
                     (__int128_t)ad[ 2] * bd[ 2] + (__int128_t)ad[ 3] * bd[ 1] +
                     (__int128_t)ad[ 4] * bd[ 0];
    t[ 5] = cd[ 5] + (__int128_t)ad[ 0] * bd[ 5] + (__int128_t)ad[ 1] * bd[ 4] +
                     (__int128_t)ad[ 2] * bd[ 3] + (__int128_t)ad[ 3] * bd[ 2] +
                     (__int128_t)ad[ 4] * bd[ 1] + (__int128_t)ad[ 5] * bd[ 0];
    t[ 6] =          (__int128_t)ad[ 1] * bd[ 5] + (__int128_t)ad[ 2] * bd[ 4] +
                     (__int128_t)ad[ 3] * bd[ 3] + (__int128_t)ad[ 4] * bd[ 2] +
                     (__int128_t)ad[ 5] * bd[ 1];
    t[ 7] =          (__int128_t)ad[ 2] * bd[ 5] + (__int128_t)ad[ 3] * bd[ 4] +
                     (__int128_t)ad[ 4] * bd[ 3] + (__int128_t)ad[ 5] * bd[ 2];
    t[ 8] =          (__int128_t)ad[ 3] * bd[ 5] + (__int128_t)ad[ 4] * bd[ 4] +
                     (__int128_t)ad[ 5] * bd[ 3];
    t[ 9] =          (__int128_t)ad[ 4] * bd[ 5] + (__int128_t)ad[ 5] * bd[ 4];
    t[10] =          (__int128_t)ad[ 5] * bd[ 5];
    t[11] = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;
    carry = t[ 8] >> 42; t[ 9] += carry; t[ 8] &= MASK_42;
    carry = t[10] >> 42; t[11] += carry; t[10] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 9] >> 42; t[10] += carry; t[ 9] &= MASK_42;

    t[ 5] -= t[11] * ORDER_0;
    t[ 6] -= t[11] * ORDER_1;
    t[ 7] -= t[11] * ORDER_2;

    t[ 4] -= t[10] * ORDER_0;
    t[ 5] -= t[10] * ORDER_1;
    t[ 6] -= t[10] * ORDER_2;

    t[ 3] -= t[ 9] * ORDER_0;
    t[ 4] -= t[ 9] * ORDER_1;
    t[ 5] -= t[ 9] * ORDER_2;

    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;
    carry = t[ 7] >> 42; t[ 8] += carry; t[ 7] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 6] >> 42; t[ 7] += carry; t[ 6] &= MASK_42;

    t[ 2] -= t[ 8] * ORDER_0;
    t[ 3] -= t[ 8] * ORDER_1;
    t[ 4] -= t[ 8] * ORDER_2;

    t[ 1] -= t[ 7] * ORDER_0;
    t[ 2] -= t[ 7] * ORDER_1;
    t[ 3] -= t[ 7] * ORDER_2;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;
    t[ 6]  = 0;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;
    carry = t[ 5] >> 42; t[ 6] += carry; t[ 5] &= MASK_42;

    t[ 0] -= t[ 6] * ORDER_0;
    t[ 1] -= t[ 6] * ORDER_1;
    t[ 2] -= t[ 6] * ORDER_2;

    carry = t[ 0] >> 42; t[ 1] += carry; t[ 0] &= MASK_42;
    carry = t[ 1] >> 42; t[ 2] += carry; t[ 1] &= MASK_42;
    carry = t[ 2] >> 42; t[ 3] += carry; t[ 2] &= MASK_42;
    carry = t[ 3] >> 42; t[ 4] += carry; t[ 3] &= MASK_42;
    carry = t[ 4] >> 42; t[ 5] += carry; t[ 4] &= MASK_42;

    s[ 0] = (t[ 0] >>  0);
    s[ 1] = (t[ 0] >>  8);
    s[ 2] = (t[ 0] >> 16);
    s[ 3] = (t[ 0] >> 24);
    s[ 4] = (t[ 0] >> 32);
    s[ 5] = (t[ 0] >> 40) | (t[ 1] <<  2);
    s[ 6] = (t[ 1] >>  6);
    s[ 7] = (t[ 1] >> 14);
    s[ 8] = (t[ 1] >> 22);
    s[ 9] = (t[ 1] >> 30);
    s[10] = (t[ 1] >> 38) | (t[ 2] <<  4);
    s[11] = (t[ 2] >>  4);
    s[12] = (t[ 2] >> 12);
    s[13] = (t[ 2] >> 20);
    s[14] = (t[ 2] >> 28);
    s[15] = (t[ 2] >> 36) | (t[ 3] <<  6);
    s[16] = (t[ 3] >>  2);
    s[17] = (t[ 3] >> 10);
    s[18] = (t[ 3] >> 18);
    s[19] = (t[ 3] >> 26);
    s[20] = (t[ 3] >> 34);
    s[21] = (t[ 4] >>  0);
    s[22] = (t[ 4] >>  8);
    s[23] = (t[ 4] >> 16);
    s[24] = (t[ 4] >> 24);
    s[25] = (t[ 4] >> 32);
    s[26] = (t[ 4] >> 40) | (t[ 5] <<  2);
    s[27] = (t[ 5] >>  6);
    s[28] = (t[ 5] >> 14);
    s[29] = (t[ 5] >> 22);
    s[30] = (t[ 5] >> 30);
    s[31] = (t[ 5] >> 38);
}
#endif /* !HAVE___UINT128_T || NO_CURVED25519_128BIT */

int ge_compress_key(byte* out, const byte* xIn, const byte* yIn, word32 keySz)
{
    ge     x,y,z;
    ge_p3  g;
    byte   bArray[ED25519_KEY_SIZE];
    word32 i;

    fe_0(x);
    fe_0(y);
    fe_1(z);
    fe_frombytes(x, xIn);
    fe_frombytes(y, yIn);

    fe_copy(g.X, x);
    fe_copy(g.Y, y);
    fe_copy(g.Z, z);

    ge_p3_tobytes(bArray, &g);

    for (i = 0; i < keySz; i++) {
        out[keySz - 1 - i] = bArray[i];
    }

    return 0;
}


/*
r = p + q
*/
static INLINE void ge_add(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
{
#ifndef CURVED25519_X64
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->YplusX);
    fe_mul(r->Y,r->Y,q->YminusX);
    fe_mul(r->T,q->T2d,p->T);
    fe_mul(r->X,p->Z,q->Z);
    fe_add(t0,r->X,r->X);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_add(r->Z,t0,r->T);
    fe_sub(r->T,t0,r->T);
#else
    fe_ge_add(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->Z, q->T2d,
              q->YplusX, q->YminusX);
#endif
}


#ifndef CURVED25519_X64
/* ge_scalar mult base */
static unsigned char equal(signed char b,signed char c)
{
  unsigned char ub = b;
  unsigned char uc = c;
  unsigned char x = ub ^ uc; /* 0: yes; 1..255: no */
  uint32_t y = x; /* 0: yes; 1..255: no */
  y -= 1; /* 4294967295: yes; 0..254: no */
  y >>= 31; /* 1: yes; 0: no */
  return (unsigned char)y;
}


static unsigned char negative(signed char b)
{
  return ((unsigned char)b) >> 7;
}


static INLINE void cmov(ge_precomp *t,const ge_precomp *u,unsigned char b,
                        unsigned char n)
{
  b = equal(b,n);
  fe_cmov(t->yplusx,u->yplusx,b);
  fe_cmov(t->yminusx,u->yminusx,b);
  fe_cmov(t->xy2d,u->xy2d,b);
}
#endif

#ifdef CURVED25519_X64
static const ge_precomp base[64][8] = {
{
    {
        { 0x2fbc93c6f58c3b85, 0xcf932dc6fb8c0e19, 0x270b4898643d42c2, 0x07cf9d3a33d4ba65 },
        { 0x9d103905d740913e, 0xfd399f05d140beb3, 0xa5c18434688f8a09, 0x44fd2f9298f81267 },
        { 0xdbbd15674b6fbb59, 0x41e13f00eea2a5ea, 0xcdd49d1cc957c6fa, 0x4f0ebe1faf16ecca }
    },
    {
        { 0x9224e7fc933c71d7, 0x9f469d967a0ff5b5, 0x5aa69a65e1d60702, 0x590c063fa87d2e2e },
        { 0x8a99a56042b4d5a8, 0x8f2b810c4e60acf6, 0xe09e236bb16e37aa, 0x6bb595a669c92555 },
        { 0x6e347eaadad36802, 0xbaf3599383ee4805, 0x3bcabe10e6076826, 0x49314f0a165ed1b8 }
    },
    {
        { 0xaf25b0a84cee9730, 0x025a8430e8864b8a, 0xc11b50029f016732, 0x7a164e1b9a80f8f4 },
        { 0x56611fe8a4fcd265, 0x3bd353fde5c1ba7d, 0x8131f31a214bd6bd, 0x2ab91587555bda62 },
        { 0x9bf211f4f1674834, 0xb84e6b17f62df895, 0xd7de6f075b722a4e, 0x549a04b963bb2a21 }
    },
    {
        { 0x287351b98efc099f, 0x6765c6f47dfd2538, 0xca348d3dfb0a9265, 0x680e910321e58727 },
        { 0x95fe050a056818bf, 0x327e89715660faa9, 0xc3e8e3cd06a05073, 0x27933f4c7445a49a },
        { 0xbf1e45ece51426b0, 0xe32bc63d6dba0f94, 0xe42974d58cf852c0, 0x44f079b1b0e64c18 }
    },
    {
        { 0xa212bc4408a5bb33, 0x8d5048c3c75eed02, 0xdd1beb0c5abfec44, 0x2945ccf146e206eb },
        { 0x7f9182c3a447d6ba, 0xd50014d14b2729b7, 0xe33cf11cb864a087, 0x154a7e73eb1b55f3 },
        { 0xc832a179e7d003b3, 0x5f729d0a00124d7e, 0x62c1d4a10e6d8ff3, 0x68b8ac5938b27a98 }
    },
    {
        { 0x3a0ceeeb77157131, 0x9b27158900c8af88, 0x8065b668da59a736, 0x51e57bb6a2cc38bd },
        { 0x499806b67b7d8ca4, 0x575be28427d22739, 0xbb085ce7204553b9, 0x38b64c41ae417884 },
        { 0x8f9dad91689de3a4, 0x175f2428f8fb9137, 0x050ab5329fcfb988, 0x7865dfa21354c09f }
    },
    {
        { 0x6b1a5cd0944ea3bf, 0x7470353ab39dc0d2, 0x71b2528228542e49, 0x461bea69283c927e },
        { 0xba6f2c9aaa3221b1, 0x6ca021533bba23a7, 0x9dea764f92192c3a, 0x1d6edd5d2e5317e0 },
        { 0x217a8aacab0fda36, 0xa528c6543d3549c8, 0x37d05b8b13ab7568, 0x233cef623a2cbc37 }
    },
    {
        { 0x59b7596604dd3e8f, 0x6cb30377e288702c, 0xb1339c665ed9c323, 0x0915e76061bce52f },
        { 0xe2a75dedf39234d9, 0x963d7680e1b558f9, 0x2c2741ac6e3c23fb, 0x3a9024a1320e01c3 },
        { 0xdf7de835a834a37e, 0x8be19cda689857ea, 0x2c1185367167b326, 0x589eb3d9dbefd5c2 }
    },
},
{
    {
        { 0x322d04a52d9021f6, 0xb9c19f3375c6bf9c, 0x587a3a4342d20b09, 0x143b1cf8aa64fe61 },
        { 0x7ec851ca553e2df3, 0xa71284cba64878b3, 0xe6b5e4193288d1e7, 0x4cf210ec5a9a8883 },
        { 0x9f867c7d968acaab, 0x5f54258e27092729, 0xd0a7d34bea180975, 0x21b546a3374126e1 }
    },
    {
        { 0x490a7a45d185218f, 0x9a15377846049335, 0x0060ea09cc31e1f6, 0x7e041577f86ee965 },
        { 0xa94ff858a2888343, 0xce0ed4565313ed3c, 0xf55c3dcfb5bf34fa, 0x0a653ca5c9eab371 },
        { 0x66b2a496ce5b67f3, 0xff5492d8bd569796, 0x503cec294a592cd0, 0x566943650813acb2 }
    },
    {
        { 0x5672f9eb1dabb69d, 0xba70b535afe853fc, 0x47ac0f752796d66d, 0x32a5351794117275 },
        { 0xb818db0c26620798, 0x5d5c31d9606e354a, 0x0982fa4f00a8cdc7, 0x17e12bcd4653e2d4 },
        { 0xd3a644a6df648437, 0x703b6559880fbfdd, 0xcb852540ad3a1aa5, 0x0900b3f78e4c6468 }
    },
    {
        { 0xed280fbec816ad31, 0x52d9595bd8e6efe3, 0x0fe71772f6c623f5, 0x4314030b051e293c },
        { 0x0a851b9f679d651b, 0xe108cb61033342f2, 0xd601f57fe88b30a3, 0x371f3acaed2dd714 },
        { 0xd560005efbf0bcad, 0x8eb70f2ed1870c5e, 0x201f9033d084e6a0, 0x4c3a5ae1ce7b6670 }
    },
    {
        { 0xbaf875e4c93da0dd, 0xb93282a771b9294d, 0x80d63fb7f4c6c460, 0x6de9c73dea66c181 },
        { 0x4138a434dcb8fa95, 0x870cf67d6c96840b, 0xde388574297be82c, 0x7c814db27262a55a },
        { 0x478904d5a04df8f2, 0xfafbae4ab10142d3, 0xf6c8ac63555d0998, 0x5aac4a412f90b104 }
    },
    {
        { 0x603a0d0abd7f5134, 0x8089c932e1d3ae46, 0xdf2591398798bd63, 0x1c145cd274ba0235 },
        { 0xc64f326b3ac92908, 0x5551b282e663e1e0, 0x476b35f54a1a4b83, 0x1b9da3fe189f68c2 },
        { 0x32e8386475f3d743, 0x365b8baf6ae5d9ef, 0x825238b6385b681e, 0x234929c1167d65e1 }
    },
    {
        { 0x48145cc21d099fcf, 0x4535c192cc28d7e5, 0x80e7c1e548247e01, 0x4a5f28743b2973ee },
        { 0x984decaba077ade8, 0x383f77ad19eb389d, 0xc7ec6b7e2954d794, 0x59c77b3aeb7c3a7a },
        { 0xd3add725225ccf62, 0x911a3381b2152c5d, 0xd8b39fad5b08f87d, 0x6f05606b4799fe3b }
    },
    {
        { 0x5b433149f91b6483, 0xadb5dc655a2cbf62, 0x87fa8412632827b3, 0x60895e91ab49f8d8 },
        { 0x9ffe9e92177ba962, 0x98aee71d0de5cae1, 0x3ff4ae942d831044, 0x714de12e58533ac8 },
        { 0xe9ecf2ed0cf86c18, 0xb46d06120735dfd4, 0xbc9da09804b96be7, 0x73e2e62fd96dc26b }
    },
},
{
    {
        { 0x2eccdd0e632f9c1d, 0x51d0b69676893115, 0x52dfb76ba8637a58, 0x6dd37d49a00eef39 },
        { 0xed5b635449aa515e, 0xa865c49f0bc6823a, 0x850c1fe95b42d1c4, 0x30d76d6f03d315b9 },
        { 0x6c4444172106e4c7, 0xfb53d680928d7f69, 0xb4739ea4694d3f26, 0x10c697112e864bb0 }
    },
    {
        { 0x0ca62aa08358c805, 0x6a3d4ae37a204247, 0x7464d3a63b11eddc, 0x03bf9baf550806ef },
        { 0x6493c4277dbe5fde, 0x265d4fad19ad7ea2, 0x0e00dfc846304590, 0x25e61cabed66fe09 },
        { 0x3f13e128cc586604, 0x6f5873ecb459747e, 0xa0b63dedcc1268f5, 0x566d78634586e22c }
    },
    {
        { 0xa1054285c65a2fd0, 0x6c64112af31667c3, 0x680ae240731aee58, 0x14fba5f34793b22a },
        { 0x1637a49f9cc10834, 0xbc8e56d5a89bc451, 0x1cb5ec0f7f7fd2db, 0x33975bca5ecc35d9 },
        { 0x3cd746166985f7d4, 0x593e5e84c9c80057, 0x2fc3f2b67b61131e, 0x14829cea83fc526c }
    },
    {
        { 0x21e70b2f4e71ecb8, 0xe656ddb940a477e3, 0xbf6556cece1d4f80, 0x05fc3bc4535d7b7e },
        { 0xff437b8497dd95c2, 0x6c744e30aa4eb5a7, 0x9e0c5d613c85e88b, 0x2fd9c71e5f758173 },
        { 0x24b8b3ae52afdedd, 0x3495638ced3b30cf, 0x33a4bc83a9be8195, 0x373767475c651f04 }
    },
    {
        { 0x634095cb14246590, 0xef12144016c15535, 0x9e38140c8910bc60, 0x6bf5905730907c8c },
        { 0x2fba99fd40d1add9, 0xb307166f96f4d027, 0x4363f05215f03bae, 0x1fbea56c3b18f999 },
        { 0x0fa778f1e1415b8a, 0x06409ff7bac3a77e, 0x6f52d7b89aa29a50, 0x02521cf67a635a56 }
    },
    {
        { 0xb1146720772f5ee4, 0xe8f894b196079ace, 0x4af8224d00ac824a, 0x001753d9f7cd6cc4 },
        { 0x513fee0b0a9d5294, 0x8f98e75c0fdf5a66, 0xd4618688bfe107ce, 0x3fa00a7e71382ced },
        { 0x3c69232d963ddb34, 0x1dde87dab4973858, 0xaad7d1f9a091f285, 0x12b5fe2fa048edb6 }
    },
    {
        { 0xdf2b7c26ad6f1e92, 0x4b66d323504b8913, 0x8c409dc0751c8bc3, 0x6f7e93c20796c7b8 },
        { 0x71f0fbc496fce34d, 0x73b9826badf35bed, 0xd2047261ff28c561, 0x749b76f96fb1206f },
        { 0x1f5af604aea6ae05, 0xc12351f1bee49c99, 0x61a808b5eeff6b66, 0x0fcec10f01e02151 }
    },
    {
        { 0x3df2d29dc4244e45, 0x2b020e7493d8de0a, 0x6cc8067e820c214d, 0x413779166feab90a },
        { 0x644d58a649fe1e44, 0x21fcaea231ad777e, 0x02441c5a887fd0d2, 0x4901aa7183c511f3 },
        { 0x08b1b7548c1af8f0, 0xce0f7a7c246299b4, 0xf760b0f91e06d939, 0x41bb887b726d1213 }
    },
},
{
    {
        { 0x97d980e0aa39f7d2, 0x35d0384252c6b51c, 0x7d43f49307cd55aa, 0x56bd36cfb78ac362 },
        { 0x9267806c567c49d8, 0x066d04ccca791e6a, 0xa69f5645e3cc394b, 0x5c95b686a0788cd2 },
        { 0x2ac519c10d14a954, 0xeaf474b494b5fa90, 0xe6af8382a9f87a5a, 0x0dea6db1879be094 }
    },
    {
        { 0x15baeb74d6a8797a, 0x7ef55cf1fac41732, 0x29001f5a3c8b05c5, 0x0ad7cc8752eaccfb },
        { 0xaa66bf547344e5ab, 0xda1258888f1b4309, 0x5e87d2b3fd564b2f, 0x5b2c78885483b1dd },
        { 0x52151362793408cf, 0xeb0f170319963d94, 0xa833b2fa883d9466, 0x093a7fa775003c78 }
    },
    {
        { 0xb8e9604460a91286, 0x7f3fd8047778d3de, 0x67d01e31bf8a5e2d, 0x7b038a06c27b653e },
        { 0xe5107de63a16d7be, 0xa377ffdc9af332cf, 0x70d5bf18440b677f, 0x6a252b19a4a31403 },
        { 0x9ed919d5d36990f3, 0x5213aebbdb4eb9f2, 0xc708ea054cb99135, 0x58ded57f72260e56 }
    },
    {
        { 0xda6d53265b0fd48b, 0x8960823193bfa988, 0xd78ac93261d57e28, 0x79f2942d3a5c8143 },
        { 0x78e79dade9413d77, 0xf257f9d59729e67d, 0x59db910ee37aa7e6, 0x6aa11b5bbb9e039c },
        { 0x97da2f25b6c88de9, 0x251ba7eaacf20169, 0x09b44f87ef4eb4e4, 0x7d90ab1bbc6a7da5 }
    },
    {
        { 0x1a07a3f496b3c397, 0x11ceaa188f4e2532, 0x7d9498d5a7751bf0, 0x19ed161f508dd8a0 },
        { 0x9acca683a7016bfe, 0x90505f4df2c50b6d, 0x6b610d5fcce435aa, 0x19a10d446198ff96 },
        { 0x560a2cd687dce6ca, 0x7f3568c48664cf4d, 0x8741e95222803a38, 0x483bdab1595653fc }
    },
    {
        { 0xd6cf4d0ab4da80f6, 0x82483e45f8307fe0, 0x05005269ae6f9da4, 0x1c7052909cf7877a },
        { 0xfa780f148734fa49, 0x106f0b70360534e0, 0x2210776fe3e307bd, 0x3286c109dde6a0fe },
        { 0x32ee7de2874e98d4, 0x14c362e9b97e0c60, 0x5781dcde6a60a38a, 0x217dd5eaaa7aa840 }
    },
    {
        { 0x8bdf1fb9be8c0ec8, 0x00bae7f8e30a0282, 0x4963991dad6c4f6c, 0x07058a6e5df6f60a },
        { 0x9db7c4d0248e1eb0, 0xe07697e14d74bf52, 0x1e6a9b173c562354, 0x7fa7c21f795a4965 },
        { 0xe9eb02c4db31f67f, 0xed25fd8910bcfb2b, 0x46c8131f5c5cddb4, 0x33b21c13a0cb9bce }
    },
    {
        { 0x9aafb9b05ee38c5b, 0xbf9d2d4e071a13c7, 0x8eee6e6de933290a, 0x1c3bab17ae109717 },
        { 0x360692f8087d8e31, 0xf4dcc637d27163f7, 0x25a4e62065ea5963, 0x659bf72e5ac160d9 },
        { 0x1c9ab216c7cab7b0, 0x7d65d37407bbc3cc, 0x52744750504a58d5, 0x09f2606b131a2990 }
    },
},
{
    {
        { 0x7e234c597c6691ae, 0x64889d3d0a85b4c8, 0xdae2c90c354afae7, 0x0a871e070c6a9e1d },
        { 0x40e87d44744346be, 0x1d48dad415b52b25, 0x7c3a8a18a13b603e, 0x4eb728c12fcdbdf7 },
        { 0x3301b5994bbc8989, 0x736bae3a5bdd4260, 0x0d61ade219d59e3c, 0x3ee7300f2685d464 }
    },
    {
        { 0x43fa7947841e7518, 0xe5c6fa59639c46d7, 0xa1065e1de3052b74, 0x7d47c6a2cfb89030 },
        { 0xf5d255e49e7dd6b7, 0x8016115c610b1eac, 0x3c99975d92e187ca, 0x13815762979125c2 },
        { 0x3fdad0148ef0d6e0, 0x9d3e749a91546f3c, 0x71ec621026bb8157, 0x148cf58d34c9ec80 }
    },
    {
        { 0xe2572f7d9ae4756d, 0x56c345bb88f3487f, 0x9fd10b6d6960a88d, 0x278febad4eaea1b9 },
        { 0x46a492f67934f027, 0x469984bef6840aa9, 0x5ca1bc2a89611854, 0x3ff2fa1ebd5dbbd4 },
        { 0xb1aa681f8c933966, 0x8c21949c20290c98, 0x39115291219d3c52, 0x4104dd02fe9c677b }
    },
    {
        { 0x81214e06db096ab8, 0x21a8b6c90ce44f35, 0x6524c12a409e2af5, 0x0165b5a48efca481 },
        { 0x72b2bf5e1124422a, 0xa1fa0c3398a33ab5, 0x94cb6101fa52b666, 0x2c863b00afaf53d5 },
        { 0xf190a474a0846a76, 0x12eff984cd2f7cc0, 0x695e290658aa2b8f, 0x591b67d9bffec8b8 }
    },
    {
        { 0x99b9b3719f18b55d, 0xe465e5faa18c641e, 0x61081136c29f05ed, 0x489b4f867030128b },
        { 0x312f0d1c80b49bfa, 0x5979515eabf3ec8a, 0x727033c09ef01c88, 0x3de02ec7ca8f7bcb },
        { 0xd232102d3aeb92ef, 0xe16253b46116a861, 0x3d7eabe7190baa24, 0x49f5fbba496cbebf }
    },
    {
        { 0x155d628c1e9c572e, 0x8a4d86acc5884741, 0x91a352f6515763eb, 0x06a1a6c28867515b },
        { 0x30949a108a5bcfd4, 0xdc40dd70bc6473eb, 0x92c294c1307c0d1c, 0x5604a86dcbfa6e74 },
        { 0x7288d1d47c1764b6, 0x72541140e0418b51, 0x9f031a6018acf6d1, 0x20989e89fe2742c6 }
    },
    {
        { 0x1674278b85eaec2e, 0x5621dc077acb2bdf, 0x640a4c1661cbf45a, 0x730b9950f70595d3 },
        { 0x499777fd3a2dcc7f, 0x32857c2ca54fd892, 0xa279d864d207e3a0, 0x0403ed1d0ca67e29 },
        { 0xc94b2d35874ec552, 0xc5e6c8cf98246f8d, 0xf7cb46fa16c035ce, 0x5bd7454308303dcc }
    },
    {
        { 0x85c4932115e7792a, 0xc64c89a2bdcdddc9, 0x9d1e3da8ada3d762, 0x5bb7db123067f82c },
        { 0x7f9ad19528b24cc2, 0x7f6b54656335c181, 0x66b8b66e4fc07236, 0x133a78007380ad83 },
        { 0x0961f467c6ca62be, 0x04ec21d6211952ee, 0x182360779bd54770, 0x740dca6d58f0e0d2 }
    },
},
{
    {
        { 0x3906c72aed261ae5, 0x9ab68fd988e100f7, 0xf5e9059af3360197, 0x0e53dc78bf2b6d47 },
        { 0x50b70bf5d3f0af0b, 0x4feaf48ae32e71f7, 0x60e84ed3a55bbd34, 0x00ed489b3f50d1ed },
        { 0xb90829bf7971877a, 0x5e4444636d17e631, 0x4d05c52e18276893, 0x27632d9a5a4a4af5 }
    },
    {
        { 0xa98285d187eaffdb, 0xa5b4fbbbd8d0a864, 0xb658f27f022663f7, 0x3bbc2b22d99ce282 },
        { 0xd11ff05154b260ce, 0xd86dc38e72f95270, 0x601fcd0d267cc138, 0x2b67916429e90ccd },
        { 0xb917c952583c0a58, 0x653ff9b80fe4c6f3, 0x9b0da7d7bcdf3c0c, 0x43a0eeb6ab54d60e }
    },
    {
        { 0x3ac6322357875fe8, 0xd9d4f4ecf5fbcb8f, 0x8dee8493382bb620, 0x50c5eaa14c799fdc },
        { 0x396966a46d4a5487, 0xf811a18aac2bb3ba, 0x66e4685b5628b26b, 0x70a477029d929b92 },
        { 0xdd0edc8bd6f2fb3c, 0x54c63aa79cc7b7a0, 0xae0b032b2c8d9f1a, 0x6f9ce107602967fb }
    },
    {
        { 0x139693063520e0b5, 0x437fcf7c88ea03fe, 0xf7d4c40bd3c959bc, 0x699154d1f893ded9 },
        { 0xad1054b1cde1c22a, 0xc4a8e90248eb32df, 0x5f3e7b33accdc0ea, 0x72364713fc79963e },
        { 0x315d5c75b4b27526, 0xcccb842d0236daa5, 0x22f0c8a3345fee8e, 0x73975a617d39dbed }
    },
    {
        { 0x6f37f392f4433e46, 0x0e19b9a11f566b18, 0x220fb78a1fd1d662, 0x362a4258a381c94d },
        { 0xe4024df96375da10, 0x78d3251a1830c870, 0x902b1948658cd91c, 0x7e18b10b29b7438a },
        { 0x9071d9132b6beb2f, 0x0f26e9ad28418247, 0xeab91ec9bdec925d, 0x4be65bc8f48af2de }
    },
    {
        { 0x1d50fba257c26234, 0x7bd4823adeb0678b, 0xc2b0dc6ea6538af5, 0x5665eec6351da73e },
        { 0x78487feba36e7028, 0x5f3f13001dd8ce34, 0x934fb12d4b30c489, 0x056c244d397f0a2b },
        { 0xdb3ee00943bfb210, 0x4972018720800ac2, 0x26ab5d6173bd8667, 0x20b209c2ab204938 }
    },
    {
        { 0x1fcca94516bd3289, 0x448d65aa41420428, 0x59c3b7b216a55d62, 0x49992cc64e612cd8 },
        { 0x549e342ac07fb34b, 0x02d8220821373d93, 0xbc262d70acd1f567, 0x7a92c9fdfbcac784 },
        { 0x65bd1bea70f801de, 0x1befb7c0fe49e28a, 0xa86306cdb1b2ae4a, 0x3b7ac0cd265c2a09 }
    },
    {
        { 0xf0d54e4f22ed39a7, 0xa2aae91e5608150a, 0xf421b2e9eddae875, 0x31bc531d6b7de992 },
        { 0x822bee438c01bcec, 0x530cb525c0fbc73b, 0x48519034c1953fe9, 0x265cc261e09a0f5b },
        { 0xdf3d134da980f971, 0x7a4fb8d1221a22a7, 0x3df7d42035aad6d8, 0x2a14edcc6a1a125e }
    },
},
{
    {
        { 0x231a8c570478433c, 0xb7b5270ec281439d, 0xdbaa99eae3d9079f, 0x2c03f5256c2b03d9 },
        { 0xdf48ee0752cfce4e, 0xc3fffaf306ec08b7, 0x05710b2ab95459c4, 0x161d25fa963ea38d },
        { 0x790f18757b53a47d, 0x307b0130cf0c5879, 0x31903d77257ef7f9, 0x699468bdbd96bbaf }
    },
    {
        { 0xd8dd3de66aa91948, 0x485064c22fc0d2cc, 0x9b48246634fdea2f, 0x293e1c4e6c4a2e3a },
        { 0xbd1f2f46f4dafecf, 0x7cef0114a47fd6f7, 0xd31ffdda4a47b37f, 0x525219a473905785 },
        { 0x376e134b925112e1, 0x703778b5dca15da0, 0xb04589af461c3111, 0x5b605c447f032823 }
    },
    {
        { 0x3be9fec6f0e7f04c, 0x866a579e75e34962, 0x5542ef161e1de61a, 0x2f12fef4cc5abdd5 },
        { 0xb965805920c47c89, 0xe7f0100c923b8fcc, 0x0001256502e2ef77, 0x24a76dcea8aeb3ee },
        { 0x0a4522b2dfc0c740, 0x10d06e7f40c9a407, 0xc6cf144178cff668, 0x5e607b2518a43790 }
    },
    {
        { 0xa02c431ca596cf14, 0xe3c42d40aed3e400, 0xd24526802e0f26db, 0x201f33139e457068 },
        { 0x58b31d8f6cdf1818, 0x35cfa74fc36258a2, 0xe1b3ff4f66e61d6e, 0x5067acab6ccdd5f7 },
        { 0xfd527f6b08039d51, 0x18b14964017c0006, 0xd5220eb02e25a4a8, 0x397cba8862460375 }
    },
    {
        { 0x7815c3fbc81379e7, 0xa6619420dde12af1, 0xffa9c0f885a8fdd5, 0x771b4022c1e1c252 },
        { 0x30c13093f05959b2, 0xe23aa18de9a97976, 0x222fd491721d5e26, 0x2339d320766e6c3a },
        { 0xd87dd986513a2fa7, 0xf5ac9b71f9d4cf08, 0xd06bc31b1ea283b3, 0x331a189219971a76 }
    },
    {
        { 0x26512f3a9d7572af, 0x5bcbe28868074a9e, 0x84edc1c11180f7c4, 0x1ac9619ff649a67b },
        { 0xf5166f45fb4f80c6, 0x9c36c7de61c775cf, 0xe3d4e81b9041d91c, 0x31167c6b83bdfe21 },
        { 0xf22b3842524b1068, 0x5068343bee9ce987, 0xfc9d71844a6250c8, 0x612436341f08b111 }
    },
    {
        { 0x8b6349e31a2d2638, 0x9ddfb7009bd3fd35, 0x7f8bf1b8a3a06ba4, 0x1522aa3178d90445 },
        { 0xd99d41db874e898d, 0x09fea5f16c07dc20, 0x793d2c67d00f9bbc, 0x46ebe2309e5eff40 },
        { 0x2c382f5369614938, 0xdafe409ab72d6d10, 0xe8c83391b646f227, 0x45fe70f50524306c }
    },
    {
        { 0x62f24920c8951491, 0x05f007c83f630ca2, 0x6fbb45d2f5c9d4b8, 0x16619f6db57a2245 },
        { 0xda4875a6960c0b8c, 0x5b68d076ef0e2f20, 0x07fb51cf3d0b8fd4, 0x428d1623a0e392d4 },
        { 0x084f4a4401a308fd, 0xa82219c376a5caac, 0xdeb8de4643d1bc7d, 0x1d81592d60bd38c6 }
    },
},
{
    {
        { 0x3a4a369a2f89c8a1, 0x63137a1d7c8de80d, 0xbcac008a78eda015, 0x2cb8b3a5b483b03f },
        { 0xd833d7beec2a4c38, 0x2c9162830acc20ed, 0xe93a47aa92df7581, 0x702d67a3333c4a81 },
        { 0x36e417cbcb1b90a1, 0x33b3ddaa7f11794e, 0x3f510808885bc607, 0x24141dc0e6a8020d }
    },
    {
        { 0x91925dccbd83157d, 0x3ca1205322cc8094, 0x28e57f183f90d6e4, 0x1a4714cede2e767b },
        { 0x59f73c773fefee9d, 0xb3f1ef89c1cf989d, 0xe35dfb42e02e545f, 0x5766120b47a1b47c },
        { 0xdb20ba0fb8b6b7ff, 0xb732c3b677511fa1, 0xa92b51c099f02d89, 0x4f3875ad489ca5f1 }
    },
    {
        { 0x79ed13f6ee73eec0, 0xa5c6526d69110bb1, 0xe48928c38603860c, 0x722a1446fd7059f5 },
        { 0xc7fc762f4932ab22, 0x7ac0edf72f4c3c1b, 0x5f6b55aa9aa895e8, 0x3680274dad0a0081 },
        { 0xd0959fe9a8cf8819, 0xd0a995508475a99c, 0x6eac173320b09cc5, 0x628ecf04331b1095 }
    },
    {
        { 0x9b41acf85c74ccf1, 0xb673318108265251, 0x99c92aed11adb147, 0x7a47d70d34ecb40f },
        { 0x98bcb118a9d0ddbc, 0xee449e3408b4802b, 0x87089226b8a6b104, 0x685f349a45c7915d },
        { 0x60a0c4cbcc43a4f5, 0x775c66ca3677bea9, 0xa17aa1752ff8f5ed, 0x11ded9020e01fdc0 }
    },
    {
        { 0x471f95b03bea93b7, 0x0552d7d43313abd3, 0xbd9370e2e17e3f7b, 0x7b120f1db20e5bec },
        { 0x890e7809caefe704, 0x8728296de30e8c6c, 0x4c5cd2a392aeb1c9, 0x194263d15771531f },
        { 0x17d2fb3d86502d7a, 0xb564d84450a69352, 0x7da962c8a60ed75d, 0x00d0f85b318736aa }
    },
    {
        { 0xa6753c1efd7621c1, 0x69c0b4a7445671f5, 0x971f527405b23c11, 0x387bc74851a8c7cd },
        { 0x978b142e777c84fd, 0xf402644705a8c062, 0xa67ad51be7e612c7, 0x2f7b459698dd6a33 },
        { 0x81894b4d4a52a9a8, 0xadd93e12f6b8832f, 0x184d8548b61bd638, 0x3f1c62dbd6c9f6cd }
    },
    {
        { 0x3fad3e40148f693d, 0x052656e194eb9a72, 0x2f4dcbfd184f4e2f, 0x406f8db1c482e18b },
        { 0x2e8f1f0091910c1f, 0xa4df4fe0bff2e12c, 0x60c6560aee927438, 0x6338283facefc8fa },
        { 0x9e630d2c7f191ee4, 0x4fbf8301bc3ff670, 0x787d8e4e7afb73c4, 0x50d83d5be8f58fa5 }
    },
    {
        { 0xc0accf90b4d3b66d, 0xa7059de561732e60, 0x033d1f7870c6b0ba, 0x584161cd26d946e4 },
        { 0x85683916c11a1897, 0x2d69a4efe506d008, 0x39af1378f664bd01, 0x65942131361517c6 },
        { 0xbbf2b1a072d27ca2, 0xbf393c59fbdec704, 0xe98dbbcee262b81e, 0x02eebd0b3029b589 }
    },
},
{
    {
        { 0x8765b69f7b85c5e8, 0x6ff0678bd168bab2, 0x3a70e77c1d330f9b, 0x3a5f6d51b0af8e7c },
        { 0x61368756a60dac5f, 0x17e02f6aebabdc57, 0x7f193f2d4cce0f7d, 0x20234a7789ecdcf0 },
        { 0x76d20db67178b252, 0x071c34f9d51ed160, 0xf62a4a20b3e41170, 0x7cd682353cffe366 }
    },
    {
        { 0xa665cd6068acf4f3, 0x42d92d183cd7e3d3, 0x5759389d336025d9, 0x3ef0253b2b2cd8ff },
        { 0x0be1a45bd887fab6, 0x2a846a32ba403b6e, 0xd9921012e96e6000, 0x2838c8863bdc0943 },
        { 0xd16bb0cf4a465030, 0xfa496b4115c577ab, 0x82cfae8af4ab419d, 0x21dcb8a606a82812 }
    },
    {
        { 0x9a8d00fabe7731ba, 0x8203607e629e1889, 0xb2cc023743f3d97f, 0x5d840dbf6c6f678b },
        { 0x5c6004468c9d9fc8, 0x2540096ed42aa3cb, 0x125b4d4c12ee2f9c, 0x0bc3d08194a31dab },
        { 0x706e380d309fe18b, 0x6eb02da6b9e165c7, 0x57bbba997dae20ab, 0x3a4276232ac196dd }
    },
    {
        { 0x3bf8c172db447ecb, 0x5fcfc41fc6282dbd, 0x80acffc075aa15fe, 0x0770c9e824e1a9f9 },
        { 0x4b42432c8a7084fa, 0x898a19e3dfb9e545, 0xbe9f00219c58e45d, 0x1ff177cea16debd1 },
        { 0xcf61d99a45b5b5fd, 0x860984e91b3a7924, 0xe7300919303e3e89, 0x39f264fd41500b1e }
    },
    {
        { 0xd19b4aabfe097be1, 0xa46dfce1dfe01929, 0xc3c908942ca6f1ff, 0x65c621272c35f14e },
        { 0xa7ad3417dbe7e29c, 0xbd94376a2b9c139c, 0xa0e91b8e93597ba9, 0x1712d73468889840 },
        { 0xe72b89f8ce3193dd, 0x4d103356a125c0bb, 0x0419a93d2e1cfe83, 0x22f9800ab19ce272 }
    },
    {
        { 0x42029fdd9a6efdac, 0xb912cebe34a54941, 0x640f64b987bdf37b, 0x4171a4d38598cab4 },
        { 0x605a368a3e9ef8cb, 0xe3e9c022a5504715, 0x553d48b05f24248f, 0x13f416cd647626e5 },
        { 0xfa2758aa99c94c8c, 0x23006f6fb000b807, 0xfbd291ddadda5392, 0x508214fa574bd1ab }
    },
    {
        { 0x461a15bb53d003d6, 0xb2102888bcf3c965, 0x27c576756c683a5a, 0x3a7758a4c86cb447 },
        { 0xc20269153ed6fe4b, 0xa65a6739511d77c4, 0xcbde26462c14af94, 0x22f960ec6faba74b },
        { 0x548111f693ae5076, 0x1dae21df1dfd54a6, 0x12248c90f3115e65, 0x5d9fd15f8de7f494 }
    },
    {
        { 0x3f244d2aeed7521e, 0x8e3a9028432e9615, 0xe164ba772e9c16d4, 0x3bc187fa47eb98d8 },
        { 0x031408d36d63727f, 0x6a379aefd7c7b533, 0xa9e18fc5ccaee24b, 0x332f35914f8fbed3 },
        { 0x6d470115ea86c20c, 0x998ab7cb6c46d125, 0xd77832b53a660188, 0x450d81ce906fba03 }
    },
},
{
    {
        { 0x23264d66b2cae0b5, 0x7dbaed33ebca6576, 0x030ebed6f0d24ac8, 0x2a887f78f7635510 },
        { 0xf8ae4d2ad8453902, 0x7018058ee8db2d1d, 0xaab3995fc7d2c11e, 0x53b16d2324ccca79 },
        { 0x2a23b9e75c012d4f, 0x0c974651cae1f2ea, 0x2fb63273675d70ca, 0x0ba7250b864403f5 }
    },
    {
        { 0xdd63589386f86d9c, 0x61699176e13a85a4, 0x2e5111954eaa7d57, 0x32c21b57fb60bdfb },
        { 0xbb0d18fd029c6421, 0xbc2d142189298f02, 0x8347f8e68b250e96, 0x7b9f2fe8032d71c9 },
        { 0xd87823cd319e0780, 0xefc4cfc1897775c5, 0x4854fb129a0ab3f7, 0x12c49d417238c371 }
    },
    {
        { 0x09b3a01783799542, 0x626dd08faad5ee3f, 0xba00bceeeb70149f, 0x1421b246a0a444c9 },
        { 0x0950b533ffe83769, 0x21861c1d8e1d6bd1, 0xf022d8381302e510, 0x2509200c6391cab4 },
        { 0x4aa43a8e8c24a7c7, 0x04c1f540d8f05ef5, 0xadba5e0c0b3eb9dc, 0x2ab5504448a49ce3 }
    },
    {
        { 0xdc07ac631c5d3afa, 0x58615171f9df8c6c, 0x72a079d89d73e2b0, 0x7301f4ceb4eae15d },
        { 0x2ed227266f0f5dec, 0x9824ee415ed50824, 0x807bec7c9468d415, 0x7093bae1b521e23f },
        { 0x6409e759d6722c41, 0xa674e1cf72bf729b, 0xbc0a24eb3c21e569, 0x390167d24ebacb23 }
    },
    {
        { 0xd7bb054ba2f2120b, 0xe2b9ceaeb10589b7, 0x3fe8bac8f3c0edbe, 0x4cbd40767112cb69 },
        { 0x27f58e3bba353f1c, 0x4c47764dbf6a4361, 0xafbbc4e56e562650, 0x07db2ee6aae1a45d },
        { 0x0b603cc029c58176, 0x5988e3825cb15d61, 0x2bb61413dcf0ad8d, 0x7b8eec6c74183287 }
    },
    {
        { 0x32fee570fc386b73, 0xda8b0141da3a8cc7, 0x975ffd0ac8968359, 0x6ee809a1b132a855 },
        { 0xe4ca40782cd27cb0, 0xdaf9c323fbe967bd, 0xb29bd34a8ad41e9e, 0x72810497626ede4d },
        { 0x9444bb31fcfd863a, 0x2fe3690a3e4e48c5, 0xdc29c867d088fa25, 0x13bd1e38d173292e }
    },
    {
        { 0x223fb5cf1dfac521, 0x325c25316f554450, 0x030b98d7659177ac, 0x1ed018b64f88a4bd },
        { 0xd32b4cd8696149b5, 0xe55937d781d8aab7, 0x0bcb2127ae122b94, 0x41e86fcfb14099b0 },
        { 0x3630dfa1b802a6b0, 0x880f874742ad3bd5, 0x0af90d6ceec5a4d4, 0x746a247a37cdc5d9 }
    },
    {
        { 0x6eccd85278d941ed, 0x2254ae83d22f7843, 0xc522d02e7bbfcdb7, 0x681e3351bff0e4e2 },
        { 0xd531b8bd2b7b9af6, 0x5005093537fc5b51, 0x232fcf25c593546d, 0x20a365142bb40f49 },
        { 0x8b64b59d83034f45, 0x2f8b71f21fa20efb, 0x69249495ba6550e4, 0x539ef98e45d5472b }
    },
},
{
    {
        { 0xd074d8961cae743f, 0xf86d18f5ee1c63ed, 0x97bdc55be7f4ed29, 0x4cbad279663ab108 },
        { 0x6e7bb6a1a6205275, 0xaa4f21d7413c8e83, 0x6f56d155e88f5cb2, 0x2de25d4ba6345be1 },
        { 0x80d19024a0d71fcd, 0xc525c20afb288af8, 0xb1a3974b5f3a6419, 0x7d7fbcefe2007233 }
    },
    {
        { 0xcd7c5dc5f3c29094, 0xc781a29a2a9105ab, 0x80c61d36421c3058, 0x4f9cd196dcd8d4d7 },
        { 0xfaef1e6a266b2801, 0x866c68c4d5739f16, 0xf68a2fbc1b03762c, 0x5975435e87b75a8d },
        { 0x199297d86a7b3768, 0xd0d058241ad17a63, 0xba029cad5c1c0c17, 0x7ccdd084387a0307 }
    },
    {
        { 0x9b0c84186760cc93, 0xcdae007a1ab32a99, 0xa88dec86620bda18, 0x3593ca848190ca44 },
        { 0xdca6422c6d260417, 0xae153d50948240bd, 0xa9c0c1b4fb68c677, 0x428bd0ed61d0cf53 },
        { 0x9213189a5e849aa7, 0xd4d8c33565d8facd, 0x8c52545b53fdbbd1, 0x27398308da2d63e6 }
    },
    {
        { 0xb9a10e4c0a702453, 0x0fa25866d57d1bde, 0xffb9d9b5cd27daf7, 0x572c2945492c33fd },
        { 0x42c38d28435ed413, 0xbd50f3603278ccc9, 0xbb07ab1a79da03ef, 0x269597aebe8c3355 },
        { 0xc77fc745d6cd30be, 0xe4dfe8d3e3baaefb, 0xa22c8830aa5dda0c, 0x7f985498c05bca80 }
    },
    {
        { 0xd35615520fbf6363, 0x08045a45cf4dfba6, 0xeec24fbc873fa0c2, 0x30f2653cd69b12e7 },
        { 0x3849ce889f0be117, 0x8005ad1b7b54a288, 0x3da3c39f23fc921c, 0x76c2ec470a31f304 },
        { 0x8a08c938aac10c85, 0x46179b60db276bcb, 0xa920c01e0e6fac70, 0x2f1273f1596473da }
    },
    {
        { 0x30488bd755a70bc0, 0x06d6b5a4f1d442e7, 0xead1a69ebc596162, 0x38ac1997edc5f784 },
        { 0x4739fc7c8ae01e11, 0xfd5274904a6aab9f, 0x41d98a8287728f2e, 0x5d9e572ad85b69f2 },
        { 0x0666b517a751b13b, 0x747d06867e9b858c, 0xacacc011454dde49, 0x22dfcd9cbfe9e69c }
    },
    {
        { 0x56ec59b4103be0a1, 0x2ee3baecd259f969, 0x797cb29413f5cd32, 0x0fe9877824cde472 },
        { 0x8ddbd2e0c30d0cd9, 0xad8e665facbb4333, 0x8f6b258c322a961f, 0x6b2916c05448c1c7 },
        { 0x7edb34d10aba913b, 0x4ea3cd822e6dac0e, 0x66083dff6578f815, 0x4c303f307ff00a17 }
    },
    {
        { 0x29fc03580dd94500, 0xecd27aa46fbbec93, 0x130a155fc2e2a7f8, 0x416b151ab706a1d5 },
        { 0xd30a3bd617b28c85, 0xc5d377b739773bea, 0xc6c6e78c1e6a5cbf, 0x0d61b8f78b2ab7c4 },
        { 0x56a8d7efe9c136b0, 0xbd07e5cd58e44b20, 0xafe62fda1b57e0ab, 0x191a2af74277e8d2 }
    },
},
{
    {
        { 0x09d4b60b2fe09a14, 0xc384f0afdbb1747e, 0x58e2ea8978b5fd6e, 0x519ef577b5e09b0a },
        { 0xd550095bab6f4985, 0x04f4cd5b4fbfaf1a, 0x9d8e2ed12a0c7540, 0x2bc24e04b2212286 },
        { 0x1863d7d91124cca9, 0x7ac08145b88a708e, 0x2bcd7309857031f5, 0x62337a6e8ab8fae5 }
    },
    {
        { 0xd1ab324e1b3a1273, 0x18947cf181055340, 0x3b5d9567a98c196e, 0x7fa00425802e1e68 },
        { 0x4bcef17f06ffca16, 0xde06e1db692ae16a, 0x0753702d614f42b0, 0x5f6041b45b9212d0 },
        { 0x7d531574028c2705, 0x80317d69db0d75fe, 0x30fface8ef8c8ddd, 0x7e9de97bb6c3e998 }
    },
    {
        { 0xf004be62a24d40dd, 0xba0659910452d41f, 0x81c45ee162a44234, 0x4cb829d8a22266ef },
        { 0x1558967b9e6585a3, 0x97c99ce098e98b92, 0x10af149b6eb3adad, 0x42181fe8f4d38cfa },
        { 0x1dbcaa8407b86681, 0x081f001e8b26753b, 0x3cd7ce6a84048e81, 0x78af11633f25f22c }
    },
    {
        { 0x3241c00e7d65318c, 0xe6bee5dcd0e86de7, 0x118b2dc2fbc08c26, 0x680d04a7fc603dc3 },
        { 0x8416ebd40b50babc, 0x1508722628208bee, 0xa3148fafb9c1c36d, 0x0d07daacd32d7d5d },
        { 0xf9c2414a695aa3eb, 0xdaa42c4c05a68f21, 0x7c6c23987f93963e, 0x210e8cd30c3954e3 }
    },
    {
        { 0x2b50f16137fe6c26, 0xe102bcd856e404d8, 0x12b0f1414c561f6b, 0x51b17bc8d028ec91 },
        { 0xac4201f210a71c06, 0x6a65e0aef3bfb021, 0xbc42c35c393632f7, 0x56ea8db1865f0742 },
        { 0xfff5fb4bcf535119, 0xf4989d79df1108a0, 0xbdfcea659a3ba325, 0x18a11f1174d1a6f2 }
    },
    {
        { 0xfbd63cdad27a5f2c, 0xf00fc4bc8aa106d7, 0x53fb5c1a8e64a430, 0x04eaabe50c1a2e85 },
        { 0x407375ab3f6bba29, 0x9ec3b6d8991e482e, 0x99c80e82e55f92e9, 0x307c13b6fb0c0ae1 },
        { 0x24751021cb8ab5e7, 0xfc2344495c5010eb, 0x5f1e717b4e5610a1, 0x44da5f18c2710cd5 }
    },
    {
        { 0x9156fe6b89d8eacc, 0xe6b79451e23126a1, 0xbd7463d93944eb4e, 0x726373f6767203ae },
        { 0x033cc55ff1b82eb5, 0xb15ae36d411cae52, 0xba40b6198ffbacd3, 0x768edce1532e861f },
        { 0xe305ca72eb7ef68a, 0x662cf31f70eadb23, 0x18f026fdb4c45b68, 0x513b5384b5d2ecbd }
    },
    {
        { 0x5e2702878af34ceb, 0x900b0409b946d6ae, 0x6512ebf7dabd8512, 0x61d9b76988258f81 },
        { 0x46d46280c729989e, 0x4b93fbd05368a5dd, 0x63df3f81d1765a89, 0x34cebd64b9a0a223 },
        { 0xa6c5a71349b7d94b, 0xa3f3d15823eb9446, 0x0416fbd277484834, 0x69d45e6f2c70812f }
    },
},
{
    {
        { 0x9fe62b434f460efb, 0xded303d4a63607d6, 0xf052210eb7a0da24, 0x237e7dbe00545b93 },
        { 0xce16f74bc53c1431, 0x2b9725ce2072edde, 0xb8b9c36fb5b23ee7, 0x7e2e0e450b5cc908 },
        { 0x013575ed6701b430, 0x231094e69f0bfd10, 0x75320f1583e47f22, 0x71afa699b11155e3 }
    },
    {
        { 0xea423c1c473b50d6, 0x51e87a1f3b38ef10, 0x9b84bf5fb2c9be95, 0x00731fbc78f89a1c },
        { 0x65ce6f9b3953b61d, 0xc65839eaafa141e6, 0x0f435ffda9f759fe, 0x021142e9c2b1c28e },
        { 0xe430c71848f81880, 0xbf960c225ecec119, 0xb6dae0836bba15e3, 0x4c4d6f3347e15808 }
    },
    {
        { 0x2f0cddfc988f1970, 0x6b916227b0b9f51b, 0x6ec7b6c4779176be, 0x38bf9500a88f9fa8 },
        { 0x18f7eccfc17d1fc9, 0x6c75f5a651403c14, 0xdbde712bf7ee0cdf, 0x193fddaaa7e47a22 },
        { 0x1fd2c93c37e8876f, 0xa2f61e5a18d1462c, 0x5080f58239241276, 0x6a6fb99ebf0d4969 }
    },
    {
        { 0xeeb122b5b6e423c6, 0x939d7010f286ff8e, 0x90a92a831dcf5d8c, 0x136fda9f42c5eb10 },
        { 0x6a46c1bb560855eb, 0x2416bb38f893f09d, 0xd71d11378f71acc1, 0x75f76914a31896ea },
        { 0xf94cdfb1a305bdd1, 0x0f364b9d9ff82c08, 0x2a87d8a5c3bb588a, 0x022183510be8dcba }
    },
    {
        { 0x9d5a710143307a7f, 0xb063de9ec47da45f, 0x22bbfe52be927ad3, 0x1387c441fd40426c },
        { 0x4af766385ead2d14, 0xa08ed880ca7c5830, 0x0d13a6e610211e3d, 0x6a071ce17b806c03 },
        { 0xb5d3c3d187978af8, 0x722b5a3d7f0e4413, 0x0d7b4848bb477ca0, 0x3171b26aaf1edc92 }
    },
    {
        { 0xa60db7d8b28a47d1, 0xa6bf14d61770a4f1, 0xd4a1f89353ddbd58, 0x6c514a63344243e9 },
        { 0xa92f319097564ca8, 0xff7bb84c2275e119, 0x4f55fe37a4875150, 0x221fd4873cf0835a },
        { 0x2322204f3a156341, 0xfb73e0e9ba0a032d, 0xfce0dd4c410f030e, 0x48daa596fb924aaa }
    },
    {
        { 0x14f61d5dc84c9793, 0x9941f9e3ef418206, 0xcdf5b88f346277ac, 0x58c837fa0e8a79a9 },
        { 0x6eca8e665ca59cc7, 0xa847254b2e38aca0, 0x31afc708d21e17ce, 0x676dd6fccad84af7 },
        { 0x0cf9688596fc9058, 0x1ddcbbf37b56a01b, 0xdcc2e77d4935d66a, 0x1c4f73f2c6a57f0a }
    },
    {
        { 0xb36e706efc7c3484, 0x73dfc9b4c3c1cf61, 0xeb1d79c9781cc7e5, 0x70459adb7daf675c },
        { 0x0e7a4fbd305fa0bb, 0x829d4ce054c663ad, 0xf421c3832fe33848, 0x795ac80d1bf64c42 },
        { 0x1b91db4991b42bb3, 0x572696234b02dcca, 0x9fdf9ee51f8c78dc, 0x5fe162848ce21fd3 }
    },
},
{
    {
        { 0x315c29c795115389, 0xd7e0e507862f74ce, 0x0c4a762185927432, 0x72de6c984a25a1e4 },
        { 0xe2790aae4d077c41, 0x8b938270db7469a3, 0x6eb632dc8abd16a2, 0x720814ecaa064b72 },
        { 0xae9ab553bf6aa310, 0x050a50a9806d6e1b, 0x92bb7403adff5139, 0x0394d27645be618b }
    },
    {
        { 0xf5396425b23545a4, 0x15a7a27e98fbb296, 0xab6c52bc636fdd86, 0x79d995a8419334ee },
        { 0x4d572251857eedf4, 0xe3724edde19e93c5, 0x8a71420e0b797035, 0x3b3c833687abe743 },
        { 0xcd8a8ea61195dd75, 0xa504d8a81dd9a82f, 0x540dca81a35879b6, 0x60dd16a379c86a8a }
    },
    {
        { 0x3501d6f8153e47b8, 0xb7a9675414a2f60c, 0x112ee8b6455d9523, 0x4e62a3c18112ea8a },
        { 0x35a2c8487381e559, 0x596ffea6d78082cb, 0xcb9771ebdba7b653, 0x5a08b5019b4da685 },
        { 0xc8d4ac04516ab786, 0x595af3215295b23d, 0xd6edd234db0230c1, 0x0929efe8825b41cc }
    },
    {
        { 0x8b3172b7ad56651d, 0x01581b7a3fabd717, 0x2dc94df6424df6e4, 0x30376e5d2c29284f },
        { 0x5f0601d1cbd0f2d3, 0x736e412f6132bb7f, 0x83604432238dde87, 0x1e3a5272f5c0753c },
        { 0xd2918da78159a59c, 0x6bdc1cd93f0713f3, 0x565f7a934acd6590, 0x53daacec4cb4c128 }
    },
    {
        { 0x99852bc3852cfdb0, 0x2cc12e9559d6ed0b, 0x70f9e2bf9b5ac27b, 0x4f3b8c117959ae99 },
        { 0x4ca73bd79cc8a7d6, 0x4d4a738f47e9a9b2, 0xf4cbf12942f5fe00, 0x01a13ff9bdbf0752 },
        { 0x55b6c9c82ff26412, 0x1ac4a8c91fb667a8, 0xd527bfcfeb778bf2, 0x303337da7012a3be }
    },
    {
        { 0x976d3ccbfad2fdd1, 0xcb88839737a640a8, 0x2ff00c1d6734cb25, 0x269ff4dc789c2d2b },
        { 0x955422228c1c9d7c, 0x01fac1371a9b340f, 0x7e8d9177925b48d7, 0x53f8ad5661b3e31b },
        { 0x0c003fbdc08d678d, 0x4d982fa37ead2b17, 0xc07e6bcdb2e582f1, 0x296c7291df412a44 }
    },
    {
        { 0xdfb23205dab8b59e, 0x465aeaa0c8092250, 0xd133c1189a725d18, 0x2327370261f117d1 },
        { 0x7903de2b33daf397, 0xd0ff0619c9a624b3, 0x8a1d252b555b3e18, 0x2b6d581c52e0b7c0 },
        { 0x3d0543d3623e7986, 0x679414c2c278a354, 0xae43f0cc726196f6, 0x7836c41f8245eaba }
    },
    {
        { 0xca651e848011937c, 0xc6b0c46e6ef41a28, 0xb7021ba75f3f8d52, 0x119dff99ead7b9fd },
        { 0xe7a254db49e95a81, 0x5192d5d008b0ad73, 0x4d20e5b1d00afc07, 0x5d55f8012cf25f38 },
        { 0x43eadfcbf4b31d4d, 0xc6503f7411148892, 0xfeee68c5060d3b17, 0x329293b3dd4a0ac8 }
    },
},
{
    {
        { 0x2879852d5d7cb208, 0xb8dedd70687df2e7, 0xdc0bffab21687891, 0x2b44c043677daa35 },
        { 0x4e59214fe194961a, 0x49be7dc70d71cd4f, 0x9300cfd23b50f22d, 0x4789d446fc917232 },
        { 0x1a1c87ab074eb78e, 0xfac6d18e99daf467, 0x3eacbbcd484f9067, 0x60c52eef2bb9a4e4 }
    },
    {
        { 0x702bc5c27cae6d11, 0x44c7699b54a48cab, 0xefbc4056ba492eb2, 0x70d77248d9b6676d },
        { 0x0b5d89bc3bfd8bf1, 0xb06b9237c9f3551a, 0x0e4c16b0d53028f5, 0x10bc9c312ccfcaab },
        { 0xaa8ae84b3ec2a05b, 0x98699ef4ed1781e0, 0x794513e4708e85d1, 0x63755bd3a976f413 }
    },
    {
        { 0x3dc7101897f1acb7, 0x5dda7d5ec165bbd8, 0x508e5b9c0fa1020f, 0x2763751737c52a56 },
        { 0xb55fa03e2ad10853, 0x356f75909ee63569, 0x9ff9f1fdbe69b890, 0x0d8cc1c48bc16f84 },
        { 0x029402d36eb419a9, 0xf0b44e7e77b460a5, 0xcfa86230d43c4956, 0x70c2dd8a7ad166e7 }
    },
    {
        { 0x91d4967db8ed7e13, 0x74252f0ad776817a, 0xe40982e00d852564, 0x32b8613816a53ce5 },
        { 0x656194509f6fec0e, 0xee2e7ea946c6518d, 0x9733c1f367e09b5c, 0x2e0fac6363948495 },
        { 0x79e7f7bee448cd64, 0x6ac83a67087886d0, 0xf89fd4d9a0e4db2e, 0x4179215c735a4f41 }
    },
    {
        { 0xe4ae33b9286bcd34, 0xb7ef7eb6559dd6dc, 0x278b141fb3d38e1f, 0x31fa85662241c286 },
        { 0x8c7094e7d7dced2a, 0x97fb8ac347d39c70, 0xe13be033a906d902, 0x700344a30cd99d76 },
        { 0xaf826c422e3622f4, 0xc12029879833502d, 0x9bc1b7e12b389123, 0x24bb2312a9952489 }
    },
    {
        { 0x41f80c2af5f85c6b, 0x687284c304fa6794, 0x8945df99a3ba1bad, 0x0d1d2af9ffeb5d16 },
        { 0xb1a8ed1732de67c3, 0x3cb49418461b4948, 0x8ebd434376cfbcd2, 0x0fee3e871e188008 },
        { 0xa9da8aa132621edf, 0x30b822a159226579, 0x4004197ba79ac193, 0x16acd79718531d76 }
    },
    {
        { 0xc959c6c57887b6ad, 0x94e19ead5f90feba, 0x16e24e62a342f504, 0x164ed34b18161700 },
        { 0x72df72af2d9b1d3d, 0x63462a36a432245a, 0x3ecea07916b39637, 0x123e0ef6b9302309 },
        { 0x487ed94c192fe69a, 0x61ae2cea3a911513, 0x877bf6d3b9a4de27, 0x78da0fc61073f3eb }
    },
    {
        { 0xa29f80f1680c3a94, 0x71f77e151ae9e7e6, 0x1100f15848017973, 0x054aa4b316b38ddd },
        { 0x5bf15d28e52bc66a, 0x2c47e31870f01a8e, 0x2419afbc06c28bdd, 0x2d25deeb256b173a },
        { 0xdfc8468d19267cb8, 0x0b28789c66e54daf, 0x2aeb1d2a666eec17, 0x134610a6ab7da760 }
    },
},
{
    {
        { 0xd91430e0dc028c3c, 0x0eb955a85217c771, 0x4b09e1ed2c99a1fa, 0x42881af2bd6a743c },
        { 0xcaf55ec27c59b23f, 0x99aeed3e154d04f2, 0x68441d72e14141f4, 0x140345133932a0a2 },
        { 0x7bfec69aab5cad3d, 0xc23e8cd34cb2cfad, 0x685dd14bfb37d6a2, 0x0ad6d64415677a18 }
    },
    {
        { 0x7914892847927e9f, 0x33dad6ef370aa877, 0x1f8f24fa11122703, 0x5265ac2f2adf9592 },
        { 0x781a439e417becb5, 0x4ac5938cd10e0266, 0x5da385110692ac24, 0x11b065a2ade31233 },
        { 0x405fdd309afcb346, 0xd9723d4428e63f54, 0x94c01df05f65aaae, 0x43e4dc3ae14c0809 }
    },
    {
        { 0xea6f7ac3adc2c6a3, 0xd0e928f6e9717c94, 0xe2d379ead645eaf5, 0x46dd8785c51ffbbe },
        { 0xbc12c7f1a938a517, 0x473028ab3180b2e1, 0x3f78571efbcd254a, 0x74e534426ff6f90f },
        { 0x709801be375c8898, 0x4b06dab5e3fd8348, 0x75880ced27230714, 0x2b09468fdd2f4c42 }
    },
    {
        { 0x5b97946582ffa02a, 0xda096a51fea8f549, 0xa06351375f77af9b, 0x1bcfde61201d1e76 },
        { 0x97c749eeb701cb96, 0x83f438d4b6a369c3, 0x62962b8b9a402cd9, 0x6976c7509888df7b },
        { 0x4a4a5490246a59a2, 0xd63ebddee87fdd90, 0xd9437c670d2371fa, 0x69e87308d30f8ed6 }
    },
    {
        { 0x0f80bf028bc80303, 0x6aae16b37a18cefb, 0xdd47ea47d72cd6a3, 0x61943588f4ed39aa },
        { 0x435a8bb15656beb0, 0xf8fac9ba4f4d5bca, 0xb9b278c41548c075, 0x3eb0ef76e892b622 },
        { 0xd26e5c3e91039f85, 0xc0e9e77df6f33aa9, 0xe8968c5570066a93, 0x3c34d1881faaaddd }
    },
    {
        { 0xbd5b0b8f2fffe0d9, 0x6aa254103ed24fb9, 0x2ac7d7bcb26821c4, 0x605b394b60dca36a },
        { 0x3f9d2b5ea09f9ec0, 0x1dab3b6fb623a890, 0xa09ba3ea72d926c4, 0x374193513fd8b36d },
        { 0xb4e856e45a9d1ed2, 0xefe848766c97a9a2, 0xb104cf641e5eee7d, 0x2f50b81c88a71c8f }
    },
    {
        { 0x2b552ca0a7da522a, 0x3230b336449b0250, 0xf2c4c5bca4b99fb9, 0x7b2c674958074a22 },
        { 0x31723c61fc6811bb, 0x9cb450486211800f, 0x768933d347995753, 0x3491a53502752fcd },
        { 0xd55165883ed28cdf, 0x12d84fd2d362de39, 0x0a874ad3e3378e4f, 0x000d2b1f7c763e74 }
    },
    {
        { 0x9624778c3e94a8ab, 0x0ad6f3cee9a78bec, 0x948ac7810d743c4f, 0x76627935aaecfccc },
        { 0x3d420811d06d4a67, 0xbefc048590e0ffe3, 0xf870c6b7bd487bde, 0x6e2a7316319afa28 },
        { 0x56a8ac24d6d59a9f, 0xc8db753e3096f006, 0x477f41e68f4c5299, 0x588d851cf6c86114 }
    },
},
{
    {
        { 0xcd2a65e777d1f515, 0x548991878faa60f1, 0xb1b73bbcdabc06e5, 0x654878cba97cc9fb },
        { 0x51138ec78df6b0fe, 0x5397da89e575f51b, 0x09207a1d717af1b9, 0x2102fdba2b20d650 },
        { 0x969ee405055ce6a1, 0x36bca7681251ad29, 0x3a1af517aa7da415, 0x0ad725db29ecb2ba }
    },
    {
        { 0xfec7bc0c9b056f85, 0x537d5268e7f5ffd7, 0x77afc6624312aefa, 0x4f675f5302399fd9 },
        { 0xdc4267b1834e2457, 0xb67544b570ce1bc5, 0x1af07a0bf7d15ed7, 0x4aefcffb71a03650 },
        { 0xc32d36360415171e, 0xcd2bef118998483b, 0x870a6eadd0945110, 0x0bccbb72a2a86561 }
    },
    {
        { 0x186d5e4c50fe1296, 0xe0397b82fee89f7e, 0x3bc7f6c5507031b0, 0x6678fd69108f37c2 },
        { 0x185e962feab1a9c8, 0x86e7e63565147dcd, 0xb092e031bb5b6df2, 0x4024f0ab59d6b73e },
        { 0x1586fa31636863c2, 0x07f68c48572d33f2, 0x4f73cc9f789eaefc, 0x2d42e2108ead4701 }
    },
    {
        { 0x21717b0d0f537593, 0x914e690b131e064c, 0x1bb687ae752ae09f, 0x420bf3a79b423c6e },
        { 0x97f5131594dfd29b, 0x6155985d313f4c6a, 0xeba13f0708455010, 0x676b2608b8d2d322 },
        { 0x8138ba651c5b2b47, 0x8671b6ec311b1b80, 0x7bff0cb1bc3135b0, 0x745d2ffa9c0cf1e0 }
    },
    {
        { 0x6036df5721d34e6a, 0xb1db8827997bb3d0, 0xd3c209c3c8756afa, 0x06e15be54c1dc839 },
        { 0xbf525a1e2bc9c8bd, 0xea5b260826479d81, 0xd511c70edf0155db, 0x1ae23ceb960cf5d0 },
        { 0x5b725d871932994a, 0x32351cb5ceb1dab0, 0x7dc41549dab7ca05, 0x58ded861278ec1f7 }
    },
    {
        { 0x2dfb5ba8b6c2c9a8, 0x48eeef8ef52c598c, 0x33809107f12d1573, 0x08ba696b531d5bd8 },
        { 0xd8173793f266c55c, 0xc8c976c5cc454e49, 0x5ce382f8bc26c3a8, 0x2ff39de85485f6f9 },
        { 0x77ed3eeec3efc57a, 0x04e05517d4ff4811, 0xea3d7a3ff1a671cb, 0x120633b4947cfe54 }
    },
    {
        { 0x82bd31474912100a, 0xde237b6d7e6fbe06, 0xe11e761911ea79c6, 0x07433be3cb393bde },
        { 0x0b94987891610042, 0x4ee7b13cecebfae8, 0x70be739594f0a4c0, 0x35d30a99b4d59185 },
        { 0xff7944c05ce997f4, 0x575d3de4b05c51a3, 0x583381fd5a76847c, 0x2d873ede7af6da9f }
    },
    {
        { 0xaa6202e14e5df981, 0xa20d59175015e1f5, 0x18a275d3bae21d6c, 0x0543618a01600253 },
        { 0x157a316443373409, 0xfab8b7eef4aa81d9, 0xb093fee6f5a64806, 0x2e773654707fa7b6 },
        { 0x0deabdf4974c23c1, 0xaa6f0a259dce4693, 0x04202cb8a29aba2c, 0x4b1443362d07960d }
    },
},
{
    {
        { 0x299b1c3f57c5715e, 0x96cb929e6b686d90, 0x3004806447235ab3, 0x2c435c24a44d9fe1 },
        { 0x47b837f753242cec, 0x256dc48cc04212f2, 0xe222fbfbe1d928c5, 0x48ea295bad8a2c07 },
        { 0x0607c97c80f8833f, 0x0e851578ca25ec5b, 0x54f7450b161ebb6f, 0x7bcb4792a0def80e }
    },
    {
        { 0x1cecd0a0045224c2, 0x757f1b1b69e53952, 0x775b7a925289f681, 0x1b6cc62016736148 },
        { 0x8487e3d02bc73659, 0x4baf8445059979df, 0xd17c975adcad6fbf, 0x57369f0bdefc96b6 },
        { 0xf1a9990175638698, 0x353dd1beeeaa60d3, 0x849471334c9ba488, 0x63fa6e6843ade311 }
    },
    {
        { 0x2195becdd24b5eb7, 0x5e41f18cc0cd44f9, 0xdf28074441ca9ede, 0x07073b98f35b7d67 },
        { 0xd15c20536597c168, 0x9f73740098d28789, 0x18aee7f13257ba1f, 0x3418bfda07346f14 },
        { 0xd03c676c4ce530d4, 0x0b64c0473b5df9f4, 0x065cef8b19b3a31e, 0x3084d661533102c9 }
    },
    {
        { 0x9a6ce876760321fd, 0x7fe2b5109eb63ad8, 0x00e7d4ae8ac80592, 0x73d86b7abb6f723a },
        { 0xe1f6b79ebf8469ad, 0x15801004e2663135, 0x9a498330af74181b, 0x3ba2504f049b673c },
        { 0x0b52b5606dba5ab6, 0xa9134f0fbbb1edab, 0x30a9520d9b04a635, 0x6813b8f37973e5db }
    },
    {
        { 0xf194ca56f3157e29, 0x136d35705ef528a5, 0xdd4cef778b0599bc, 0x7d5472af24f833ed },
        { 0x9854b054334127c1, 0x105d047882fbff25, 0xdb49f7f944186f4f, 0x1768e838bed0b900 },
        { 0xd0ef874daf33da47, 0x00d3be5db6e339f9, 0x3f2a8a2f9c9ceece, 0x5d1aeb792352435a }
    },
    {
        { 0x12c7bfaeb61ba775, 0xb84e621fe263bffd, 0x0b47a5c35c840dcf, 0x7e83be0bccaf8634 },
        { 0xf59e6bb319cd63ca, 0x670c159221d06839, 0xb06d565b2150cab6, 0x20fb199d104f12a3 },
        { 0x61943dee6d99c120, 0x86101f2e460b9fe0, 0x6bb2f1518ee8598d, 0x76b76289fcc475cc }
    },
    {
        { 0x4245f1a1522ec0b3, 0x558785b22a75656d, 0x1d485a2548a1b3c0, 0x60959eccd58fe09f },
        { 0x791b4cc1756286fa, 0xdbced317d74a157c, 0x7e732421ea72bde6, 0x01fe18491131c8e9 },
        { 0x3ebfeb7ba8ed7a09, 0x49fdc2bbe502789c, 0x44ebce5d3c119428, 0x35e1eb55be947f4a }
    },
    {
        { 0x14fd6dfa726ccc74, 0x3b084cfe2f53b965, 0xf33ae4f552a2c8b4, 0x59aab07a0d40166a },
        { 0xdbdae701c5738dd3, 0xf9c6f635b26f1bee, 0x61e96a8042f15ef4, 0x3aa1d11faf60a4d8 },
        { 0x77bcec4c925eac25, 0x1848718460137738, 0x5b374337fea9f451, 0x1865e78ec8e6aa46 }
    },
},
{
    {
        { 0x967c54e91c529ccb, 0x30f6269264c635fb, 0x2747aff478121965, 0x17038418eaf66f5c },
        { 0xccc4b7c7b66e1f7a, 0x44157e25f50c2f7e, 0x3ef06dfc713eaf1c, 0x582f446752da63f7 },
        { 0xc6317bd320324ce4, 0xa81042e8a4488bc4, 0xb21ef18b4e5a1364, 0x0c2a1c4bcda28dc9 }
    },
    {
        { 0xedc4814869bd6945, 0x0d6d907dbe1c8d22, 0xc63bd212d55cc5ab, 0x5a6a9b30a314dc83 },
        { 0xd24dc7d06f1f0447, 0xb2269e3edb87c059, 0xd15b0272fbb2d28f, 0x7c558bd1c6f64877 },
        { 0xd0ec1524d396463d, 0x12bb628ac35a24f0, 0xa50c3a791cbc5fa4, 0x0404a5ca0afbafc3 }
    },
    {
        { 0x62bc9e1b2a416fd1, 0xb5c6f728e350598b, 0x04343fd83d5d6967, 0x39527516e7f8ee98 },
        { 0x8c1f40070aa743d6, 0xccbad0cb5b265ee8, 0x574b046b668fd2de, 0x46395bfdcadd9633 },
        { 0x117fdb2d1a5d9a9c, 0x9c7745bcd1005c2a, 0xefd4bef154d56fea, 0x76579a29e822d016 }
    },
    {
        { 0x333cb51352b434f2, 0xd832284993de80e1, 0xb5512887750d35ce, 0x02c514bb2a2777c1 },
        { 0x45b68e7e49c02a17, 0x23cd51a2bca9a37f, 0x3ed65f11ec224c1b, 0x43a384dc9e05bdb1 },
        { 0x684bd5da8bf1b645, 0xfb8bd37ef6b54b53, 0x313916d7a9b0d253, 0x1160920961548059 }
    },
    {
        { 0x7a385616369b4dcd, 0x75c02ca7655c3563, 0x7dc21bf9d4f18021, 0x2f637d7491e6e042 },
        { 0xb44d166929dacfaa, 0xda529f4c8413598f, 0xe9ef63ca453d5559, 0x351e125bc5698e0b },
        { 0xd4b49b461af67bbe, 0xd603037ac8ab8961, 0x71dee19ff9a699fb, 0x7f182d06e7ce2a9a }
    },
    {
        { 0x09454b728e217522, 0xaa58e8f4d484b8d8, 0xd358254d7f46903c, 0x44acc043241c5217 },
        { 0x7a7c8e64ab0168ec, 0xcb5a4a5515edc543, 0x095519d347cd0eda, 0x67d4ac8c343e93b0 },
        { 0x1c7d6bbb4f7a5777, 0x8b35fed4918313e1, 0x4adca1c6c96b4684, 0x556d1c8312ad71bd }
    },
    {
        { 0x81f06756b11be821, 0x0faff82310a3f3dd, 0xf8b2d0556a99465d, 0x097abe38cc8c7f05 },
        { 0x17ef40e30c8d3982, 0x31f7073e15a3fa34, 0x4f21f3cb0773646e, 0x746c6c6d1d824eff },
        { 0x0c49c9877ea52da4, 0x4c4369559bdc1d43, 0x022c3809f7ccebd2, 0x577e14a34bee84bd }
    },
    {
        { 0x94fecebebd4dd72b, 0xf46a4fda060f2211, 0x124a5977c0c8d1ff, 0x705304b8fb009295 },
        { 0xf0e268ac61a73b0a, 0xf2fafa103791a5f5, 0xc1e13e826b6d00e9, 0x60fa7ee96fd78f42 },
        { 0xb63d1d354d296ec6, 0xf3c3053e5fad31d8, 0x670b958cb4bd42ec, 0x21398e0ca16353fd }
    },
},
{
    {
        { 0x86c5fc16861b7e9a, 0xf6a330476a27c451, 0x01667267a1e93597, 0x05ffb9cd6082dfeb },
        { 0x216ab2ca8da7d2ef, 0x366ad9dd99f42827, 0xae64b9004fdd3c75, 0x403a395b53909e62 },
        { 0xa617fa9ff53f6139, 0x60f2b5e513e66cb6, 0xd7a8beefb3448aa4, 0x7a2932856f5ea192 }
    },
    {
        { 0xb89c444879639302, 0x4ae4f19350c67f2c, 0xf0b35da8c81af9c6, 0x39d0003546871017 },
        { 0x0b39d761b02de888, 0x5f550e7ed2414e1f, 0xa6bfa45822e1a940, 0x050a2f7dfd447b99 },
        { 0x437c3b33a650db77, 0x6bafe81dbac52bb2, 0xfe99402d2db7d318, 0x2b5b7eec372ba6ce }
    },
    {
        { 0xa694404d613ac8f4, 0x500c3c2bfa97e72c, 0x874104d21fcec210, 0x1b205fb38604a8ee },
        { 0xb3bc4bbd83f50eef, 0x508f0c998c927866, 0x43e76587c8b7e66e, 0x0f7655a3a47f98d9 },
        { 0x55ecad37d24b133c, 0x441e147d6038c90b, 0x656683a1d62c6fee, 0x0157d5dc87e0ecae }
    },
    {
        { 0x95265514d71eb524, 0xe603d8815df14593, 0x147cdf410d4de6b7, 0x5293b1730437c850 },
        { 0xf2a7af510354c13d, 0xd7a0b145aa372b60, 0x2869b96a05a3d470, 0x6528e42d82460173 },
        { 0x23d0e0814bccf226, 0x92c745cd8196fb93, 0x8b61796c59541e5b, 0x40a44df0c021f978 }
    },
    {
        { 0x86c96e514bc5d095, 0xf20d4098fca6804a, 0x27363d89c826ea5d, 0x39ca36565719cacf },
        { 0xdaa869894f20ea6a, 0xea14a3d14c620618, 0x6001fccb090bf8be, 0x35f4e822947e9cf0 },
        { 0x97506f2f6f87b75c, 0xc624aea0034ae070, 0x1ec856e3aad34dd6, 0x055b0be0e440e58f }
    },
    {
        { 0x4d12a04b6ea33da2, 0x57cf4c15e36126dd, 0x90ec9675ee44d967, 0x64ca348d2a985aac },
        { 0x6469a17d89735d12, 0xdb6f27d5e662b9f1, 0x9fcba3286a395681, 0x363b8004d269af25 },
        { 0x99588e19e4c4912d, 0xefcc3b4e1ca5ce6b, 0x4522ea60fa5b98d5, 0x7064bbab1de4a819 }
    },
    {
        { 0xa290c06142542129, 0xf2e2c2aebe8d5b90, 0xcf2458db76abfe1b, 0x02157ade83d626bf },
        { 0xb919e1515a770641, 0xa9a2e2c74e7f8039, 0x7527250b3df23109, 0x756a7330ac27b78b },
        { 0x3e46972a1b9a038b, 0x2e4ee66a7ee03fb4, 0x81a248776edbb4ca, 0x1a944ee88ecd0563 }
    },
    {
        { 0xbb40a859182362d6, 0xb99f55778a4d1abb, 0x8d18b427758559f6, 0x26c20fe74d26235a },
        { 0xd5a91d1151039372, 0x2ed377b799ca26de, 0xa17202acfd366b6b, 0x0730291bd6901995 },
        { 0x648d1d9fe9cc22f5, 0x66bc561928dd577c, 0x47d3ed21652439d1, 0x49d271acedaf8b49 }
    },
},
{
    {
        { 0x2798aaf9b4b75601, 0x5eac72135c8dad72, 0xd2ceaa6161b7a023, 0x1bbfb284e98f7d4e },
        { 0x89f5058a382b33f3, 0x5ae2ba0bad48c0b4, 0x8f93b503a53db36e, 0x5aa3ed9d95a232e6 },
        { 0x656777e9c7d96561, 0xcb2b125472c78036, 0x65053299d9506eee, 0x4a07e14e5e8957cc }
    },
    {
        { 0x240b58cdc477a49b, 0xfd38dade6447f017, 0x19928d32a7c86aad, 0x50af7aed84afa081 },
        { 0x4ee412cb980df999, 0xa315d76f3c6ec771, 0xbba5edde925c77fd, 0x3f0bac391d313402 },
        { 0x6e4fde0115f65be5, 0x29982621216109b2, 0x780205810badd6d9, 0x1921a316baebd006 }
    },
    {
        { 0xd75aad9ad9f3c18b, 0x566a0eef60b1c19c, 0x3e9a0bac255c0ed9, 0x7b049deca062c7f5 },
        { 0x89422f7edfb870fc, 0x2c296beb4f76b3bd, 0x0738f1d436c24df7, 0x6458df41e273aeb0 },
        { 0xdccbe37a35444483, 0x758879330fedbe93, 0x786004c312c5dd87, 0x6093dccbc2950e64 }
    },
    {
        { 0x6bdeeebe6084034b, 0x3199c2b6780fb854, 0x973376abb62d0695, 0x6e3180c98b647d90 },
        { 0x1ff39a8585e0706d, 0x36d0a5d8b3e73933, 0x43b9f2e1718f453b, 0x57d1ea084827a97c },
        { 0xee7ab6e7a128b071, 0xa4c1596d93a88baa, 0xf7b4de82b2216130, 0x363e999ddd97bd18 }
    },
    {
        { 0x2f1848dce24baec6, 0x769b7255babcaf60, 0x90cb3c6e3cefe931, 0x231f979bc6f9b355 },
        { 0x96a843c135ee1fc4, 0x976eb35508e4c8cf, 0xb42f6801b58cd330, 0x48ee9b78693a052b },
        { 0x5c31de4bcc2af3c6, 0xb04bb030fe208d1f, 0xb78d7009c14fb466, 0x079bfa9b08792413 }
    },
    {
        { 0xf3c9ed80a2d54245, 0x0aa08b7877f63952, 0xd76dac63d1085475, 0x1ef4fb159470636b },
        { 0xe3903a51da300df4, 0x843964233da95ab0, 0xed3cf12d0b356480, 0x038c77f684817194 },
        { 0x854e5ee65b167bec, 0x59590a4296d0cdc2, 0x72b2df3498102199, 0x575ee92a4a0bff56 }
    },
    {
        { 0x5d46bc450aa4d801, 0xc3af1227a533b9d8, 0x389e3b262b8906c2, 0x200a1e7e382f581b },
        { 0xd4c080908a182fcf, 0x30e170c299489dbd, 0x05babd5752f733de, 0x43d4e7112cd3fd00 },
        { 0x518db967eaf93ac5, 0x71bc989b056652c0, 0xfe2b85d9567197f5, 0x050eca52651e4e38 }
    },
    {
        { 0x97ac397660e668ea, 0x9b19bbfe153ab497, 0x4cb179b534eca79f, 0x6151c09fa131ae57 },
        { 0xc3431ade453f0c9c, 0xe9f5045eff703b9b, 0xfcd97ac9ed847b3d, 0x4b0ee6c21c58f4c6 },
        { 0x3af55c0dfdf05d96, 0xdd262ee02ab4ee7a, 0x11b2bb8712171709, 0x1fef24fa800f030b }
    },
},
{
    {
        { 0xff91a66a90166220, 0xf22552ae5bf1e009, 0x7dff85d87f90df7c, 0x4f620ffe0c736fb9 },
        { 0xb496123a6b6c6609, 0xa750fe8580ab5938, 0xf471bf39b7c27a5f, 0x507903ce77ac193c },
        { 0x62f90d65dfde3e34, 0xcf28c592b9fa5fad, 0x99c86ef9c6164510, 0x25d448044a256c84 }
    },
    {
        { 0x2c7c4415c9022b55, 0x56a0d241812eb1fe, 0xf02ea1c9d7b65e0d, 0x4180512fd5323b26 },
        { 0xbd68230ec7e9b16f, 0x0eb1b9c1c1c5795d, 0x7943c8c495b6b1ff, 0x2f9faf620bbacf5e },
        { 0xa4ff3e698a48a5db, 0xba6a3806bd95403b, 0x9f7ce1af47d5b65d, 0x15e087e55939d2fb }
    },
    {
        { 0x8894186efb963f38, 0x48a00e80dc639bd5, 0xa4e8092be96c1c99, 0x5a097d54ca573661 },
        { 0x12207543745c1496, 0xdaff3cfdda38610c, 0xe4e797272c71c34f, 0x39c07b1934bdede9 },
        { 0x2d45892b17c9e755, 0xd033fd7289308df8, 0x6c2fe9d9525b8bd9, 0x2edbecf1c11cc079 }
    },
    {
        { 0xee0f0fddd087a25f, 0x9c7531555c3e34ee, 0x660c572e8fab3ab5, 0x0854fc44544cd3b2 },
        { 0x1616a4e3c715a0d2, 0x53623cb0f8341d4d, 0x96ef5329c7e899cb, 0x3d4e8dbba668baa6 },
        { 0x61eba0c555edad19, 0x24b533fef0a83de6, 0x3b77042883baa5f8, 0x678f82b898a47e8d }
    },
    {
        { 0x1e09d94057775696, 0xeed1265c3cd951db, 0xfa9dac2b20bce16f, 0x0f7f76e0e8d089f4 },
        { 0xb1491d0bd6900c54, 0x3539722c9d132636, 0x4db928920b362bc9, 0x4d7cd1fea68b69df },
        { 0x36d9ebc5d485b00c, 0xa2596492e4adb365, 0xc1659480c2119ccd, 0x45306349186e0d5f }
    },
    {
        { 0x96a414ec2b072491, 0x1bb2218127a7b65b, 0x6d2849596e8a4af0, 0x65f3b08ccd27765f },
        { 0x94ddd0c1a6cdff1d, 0x55f6f115e84213ae, 0x6c935f85992fcf6a, 0x067ee0f54a37f16f },
        { 0xecb29fff199801f7, 0x9d361d1fa2a0f72f, 0x25f11d2375fd2f49, 0x124cefe80fe10fe2 }
    },
    {
        { 0x1518e85b31b16489, 0x8faadcb7db710bfb, 0x39b0bdf4a14ae239, 0x05f4cbea503d20c1 },
        { 0x4c126cf9d18df255, 0xc1d471e9147a63b6, 0x2c6d3c73f3c93b5f, 0x6be3a6a2e3ff86a2 },
        { 0xce040e9ec04145bc, 0xc71ff4e208f6834c, 0xbd546e8dab8847a3, 0x64666aa0a4d2aba5 }
    },
    {
        { 0xb0c53bf73337e94c, 0x7cb5697e11e14f15, 0x4b84abac1930c750, 0x28dd4abfe0640468 },
        { 0x6841435a7c06d912, 0xca123c21bb3f830b, 0xd4b37b27b1cbe278, 0x1d753b84c76f5046 },
        { 0x7dc0b64c44cb9f44, 0x18a3e1ace3925dbf, 0x7a3034862d0457c4, 0x4c498bf78a0c892e }
    },
},
{
    {
        { 0x22d2aff530976b86, 0x8d90b806c2d24604, 0xdca1896c4de5bae5, 0x28005fe6c8340c17 },
        { 0x37d653fb1aa73196, 0x0f9495303fd76418, 0xad200b09fb3a17b2, 0x544d49292fc8613e },
        { 0x6aefba9f34528688, 0x5c1bff9425107da1, 0xf75bbbcd66d94b36, 0x72e472930f316dfa }
    },
    {
        { 0x07f3f635d32a7627, 0x7aaa4d865f6566f0, 0x3c85e79728d04450, 0x1fee7f000fe06438 },
        { 0x2695208c9781084f, 0xb1502a0b23450ee1, 0xfd9daea603efde02, 0x5a9d2e8c2733a34c },
        { 0x765305da03dbf7e5, 0xa4daf2491434cdbd, 0x7b4ad5cdd24a88ec, 0x00f94051ee040543 }
    },
    {
        { 0xd7ef93bb07af9753, 0x583ed0cf3db766a7, 0xce6998bf6e0b1ec5, 0x47b7ffd25dd40452 },
        { 0x8d356b23c3d330b2, 0xf21c8b9bb0471b06, 0xb36c316c6e42b83c, 0x07d79c7e8beab10d },
        { 0x87fbfb9cbc08dd12, 0x8a066b3ae1eec29b, 0x0d57242bdb1fc1bf, 0x1c3520a35ea64bb6 }
    },
    {
        { 0xcda86f40216bc059, 0x1fbb231d12bcd87e, 0xb4956a9e17c70990, 0x38750c3b66d12e55 },
        { 0x80d253a6bccba34a, 0x3e61c3a13838219b, 0x90c3b6019882e396, 0x1c3d05775d0ee66f },
        { 0x692ef1409422e51a, 0xcbc0c73c2b5df671, 0x21014fe7744ce029, 0x0621e2c7d330487c }
    },
    {
        { 0xb7ae1796b0dbf0f3, 0x54dfafb9e17ce196, 0x25923071e9aaa3b4, 0x5d8e589ca1002e9d },
        { 0xaf9860cc8259838d, 0x90ea48c1c69f9adc, 0x6526483765581e30, 0x0007d6097bd3a5bc },
        { 0xc0bf1d950842a94b, 0xb2d3c363588f2e3e, 0x0a961438bb51e2ef, 0x1583d7783c1cbf86 }
    },
    {
        { 0x90034704cc9d28c7, 0x1d1b679ef72cc58f, 0x16e12b5fbe5b8726, 0x4958064e83c5580a },
        { 0xeceea2ef5da27ae1, 0x597c3a1455670174, 0xc9a62a126609167a, 0x252a5f2e81ed8f70 },
        { 0x0d2894265066e80d, 0xfcc3f785307c8c6b, 0x1b53da780c1112fd, 0x079c170bd843b388 }
    },
    {
        { 0xcdd6cd50c0d5d056, 0x9af7686dbb03573b, 0x3ca6723ff3c3ef48, 0x6768c0d7317b8acc },
        { 0x0506ece464fa6fff, 0xbee3431e6205e523, 0x3579422451b8ea42, 0x6dec05e34ac9fb00 },
        { 0x94b625e5f155c1b3, 0x417bf3a7997b7b91, 0xc22cbddc6d6b2600, 0x51445e14ddcd52f4 }
    },
    {
        { 0x893147ab2bbea455, 0x8c53a24f92079129, 0x4b49f948be30f7a7, 0x12e990086e4fd43d },
        { 0x57502b4b3b144951, 0x8e67ff6b444bbcb3, 0xb8bd6927166385db, 0x13186f31e39295c8 },
        { 0xf10c96b37fdfbb2e, 0x9f9a935e121ceaf9, 0xdf1136c43a5b983f, 0x77b2e3f05d3e99af }
    },
},
{
    {
        { 0x9532f48fcc5cd29b, 0x2ba851bea3ce3671, 0x32dacaa051122941, 0x478d99d9350004f2 },
        { 0xfd0d75879cf12657, 0xe82fef94e53a0e29, 0xcc34a7f05bbb4be7, 0x0b251172a50c38a2 },
        { 0x1d5ad94890bb02c0, 0x50e208b10ec25115, 0xa26a22894ef21702, 0x4dc923343b524805 }
    },
    {
        { 0x3ad3e3ebf36c4975, 0xd75d25a537862125, 0xe873943da025a516, 0x6bbc7cb4c411c847 },
        { 0xe3828c400f8086b6, 0x3f77e6f7979f0dc8, 0x7ef6de304df42cb4, 0x5265797cb6abd784 },
        { 0x3c6f9cd1d4a50d56, 0xb6244077c6feab7e, 0x6ff9bf483580972e, 0x00375883b332acfb }
    },
    {
        { 0xc98bec856c75c99c, 0xe44184c000e33cf4, 0x0a676b9bba907634, 0x669e2cb571f379d7 },
        { 0x0001b2cd28cb0940, 0x63fb51a06f1c24c9, 0xb5ad8691dcd5ca31, 0x67238dbd8c450660 },
        { 0xcb116b73a49bd308, 0x025aad6b2392729e, 0xb4793efa3f55d9b1, 0x72a1056140678bb9 }
    },
    {
        { 0x0d8d2909e2e505b6, 0x98ca78abc0291230, 0x77ef5569a9b12327, 0x7c77897b81439b47 },
        { 0xa2b6812b1cc9249d, 0x62866eee21211f58, 0x2cb5c5b85df10ece, 0x03a6b259e263ae00 },
        { 0xf1c1b5e2de331cb5, 0x5a9f5d8e15fca420, 0x9fa438f17bd932b1, 0x2a381bf01c6146e7 }
    },
    {
        { 0xf7c0be32b534166f, 0x27e6ca6419cf70d4, 0x934df7d7a957a759, 0x5701461dabdec2aa },
        { 0xac9b9879cfc811c1, 0x8b7d29813756e567, 0x50da4e607c70edfc, 0x5dbca62f884400b6 },
        { 0x2c6747402c915c25, 0x1bdcd1a80b0d340a, 0x5e5601bd07b43f5f, 0x2555b4e05539a242 }
    },
    {
        { 0x78409b1d87e463d4, 0xad4da95acdfb639d, 0xec28773755259b9c, 0x69c806e9c31230ab },
        { 0x6fc09f5266ddd216, 0xdce560a7c8e37048, 0xec65939da2df62fd, 0x7a869ae7e52ed192 },
        { 0x7b48f57414bb3f22, 0x68c7cee4aedccc88, 0xed2f936179ed80be, 0x25d70b885f77bc4b }
    },
    {
        { 0x98459d29bb1ae4d4, 0x56b9c4c739f954ec, 0x832743f6c29b4b3e, 0x21ea8e2798b6878a },
        { 0x4151c3d9762bf4de, 0x083f435f2745d82b, 0x29775a2e0d23ddd5, 0x138e3a6269a5db24 },
        { 0x87bef4b46a5a7b9c, 0xd2299d1b5fc1d062, 0x82409818dd321648, 0x5c5abeb1e5a2e03d }
    },
    {
        { 0x02cde6de1306a233, 0x7b5a52a2116f8ec7, 0xe1c681f4c1163b5b, 0x241d350660d32643 },
        { 0x14722af4b73c2ddb, 0xbc470c5f5a05060d, 0x00943eac2581b02e, 0x0e434b3b1f499c8f },
        { 0x6be4404d0ebc52c7, 0xae46233bb1a791f5, 0x2aec170ed25db42b, 0x1d8dfd966645d694 }
    },
},
{
    {
        { 0xd598639c12ddb0a4, 0xa5d19f30c024866b, 0xd17c2f0358fce460, 0x07a195152e095e8a },
        { 0x296fa9c59c2ec4de, 0xbc8b61bf4f84f3cb, 0x1c7706d917a8f908, 0x63b795fc7ad3255d },
        { 0xa8368f02389e5fc8, 0x90433b02cf8de43b, 0xafa1fd5dc5412643, 0x3e8fe83d032f0137 }
    },
    {
        { 0x08704c8de8efd13c, 0xdfc51a8e33e03731, 0xa59d5da51260cde3, 0x22d60899a6258c86 },
        { 0x2f8b15b90570a294, 0x94f2427067084549, 0xde1c5ae161bbfd84, 0x75ba3b797fac4007 },
        { 0x6239dbc070cdd196, 0x60fe8a8b6c7d8a9a, 0xb38847bceb401260, 0x0904d07b87779e5e }
    },
    {
        { 0xf4322d6648f940b9, 0x06952f0cbd2d0c39, 0x167697ada081f931, 0x6240aacebaf72a6c },
        { 0xb4ce1fd4ddba919c, 0xcf31db3ec74c8daa, 0x2c63cc63ad86cc51, 0x43e2143fbc1dde07 },
        { 0xf834749c5ba295a0, 0xd6947c5bca37d25a, 0x66f13ba7e7c9316a, 0x56bdaf238db40cac }
    },
    {
        { 0x1310d36cc19d3bb2, 0x062a6bb7622386b9, 0x7c9b8591d7a14f5c, 0x03aa31507e1e5754 },
        { 0x362ab9e3f53533eb, 0x338568d56eb93d40, 0x9e0e14521d5a5572, 0x1d24a86d83741318 },
        { 0xf4ec7648ffd4ce1f, 0xe045eaf054ac8c1c, 0x88d225821d09357c, 0x43b261dc9aeb4859 }
    },
    {
        { 0x19513d8b6c951364, 0x94fe7126000bf47b, 0x028d10ddd54f9567, 0x02b4d5e242940964 },
        { 0xe55b1e1988bb79bb, 0xa09ed07dc17a359d, 0xb02c2ee2603dea33, 0x326055cf5b276bc2 },
        { 0xb4a155cb28d18df2, 0xeacc4646186ce508, 0xc49cf4936c824389, 0x27a6c809ae5d3410 }
    },
    {
        { 0xcd2c270ac43d6954, 0xdd4a3e576a66cab2, 0x79fa592469d7036c, 0x221503603d8c2599 },
        { 0x8ba6ebcd1f0db188, 0x37d3d73a675a5be8, 0xf22edfa315f5585a, 0x2cb67174ff60a17e },
        { 0x59eecdf9390be1d0, 0xa9422044728ce3f1, 0x82891c667a94f0f4, 0x7b1df4b73890f436 }
    },
    {
        { 0x5f2e221807f8f58c, 0xe3555c9fd49409d4, 0xb2aaa88d1fb6a630, 0x68698245d352e03d },
        { 0xe492f2e0b3b2a224, 0x7c6c9e062b551160, 0x15eb8fe20d7f7b0e, 0x61fcef2658fc5992 },
        { 0xdbb15d852a18187a, 0xf3e4aad386ddacd7, 0x44bae2810ff6c482, 0x46cf4c473daf01cf }
    },
    {
        { 0x213c6ea7f1498140, 0x7c1e7ef8392b4854, 0x2488c38c5629ceba, 0x1065aae50d8cc5bb },
        { 0x426525ed9ec4e5f9, 0x0e5eda0116903303, 0x72b1a7f2cbe5cadc, 0x29387bcd14eb5f40 },
        { 0x1c2c4525df200d57, 0x5c3b2dd6bfca674a, 0x0a07e7b1e1834030, 0x69a198e64f1ce716 }
    },
},
{
    {
        { 0x7b26e56b9e2d4734, 0xc4c7132b81c61675, 0xef5c9525ec9cde7f, 0x39c80b16e71743ad },
        { 0x7afcd613efa9d697, 0x0cc45aa41c067959, 0xa56fe104c1fada96, 0x3a73b70472e40365 },
        { 0x0f196e0d1b826c68, 0xf71ff0e24960e3db, 0x6113167023b7436c, 0x0cf0ea5877da7282 }
    },
    {
        { 0xe332ced43ba6945a, 0xde0b1361e881c05d, 0x1ad40f095e67ed3b, 0x5da8acdab8c63d5d },
        { 0x196c80a4ddd4ccbd, 0x22e6f55d95f2dd9d, 0xc75e33c740d6c71b, 0x7bb51279cb3c042f },
        { 0xc4b6664a3a70159f, 0x76194f0f0a904e14, 0xa5614c39a4096c13, 0x6cd0ff50979feced }
    },
    {
        { 0x7fecfabdb04ba18e, 0xd0fc7bfc3bddbcf7, 0xa41d486e057a131c, 0x641a4391f2223a61 },
        { 0xc0e067e78f4428ac, 0x14835ab0a61135e3, 0xf21d14f338062935, 0x6390a4c8df04849c },
        { 0xc5c6b95aa606a8db, 0x914b7f9eb06825f1, 0x2a731f6b44fc9eff, 0x30ddf38562705cfc }
    },
    {
        { 0x33bef2bd68bcd52c, 0xc649dbb069482ef2, 0xb5b6ee0c41cb1aee, 0x5c294d270212a7e5 },
        { 0x4e3dcbdad1bff7f9, 0xc9118e8220645717, 0xbacccebc0f189d56, 0x1b4822e9d4467668 },
        { 0xab360a7f25563781, 0x2512228a480f7958, 0xc75d05276114b4e3, 0x222d9625d976fe2a }
    },
    {
        { 0x0f94be7e0a344f85, 0xeb2faa8c87f22c38, 0x9ce1e75e4ee16f0f, 0x43e64e5418a08dea },
        { 0x1c717f85b372ace1, 0x81930e694638bf18, 0x239cad056bc08b58, 0x0b34271c87f8fff4 },
        { 0x8155e2521a35ce63, 0xbe100d4df912028e, 0xbff80bf8a57ddcec, 0x57342dc96d6bc6e4 }
    },
    {
        { 0xf3c3bcb71e707bf6, 0x351d9b8c7291a762, 0x00502e6edad69a33, 0x522f521f1ec8807f },
        { 0xefeef065c8ce5998, 0xbf029510b5cbeaa2, 0x8c64a10620b7c458, 0x35134fb231c24855 },
        { 0x272c1f46f9a3902b, 0xc91ba3b799657bcc, 0xae614b304f8a1c0e, 0x7afcaad70b99017b }
    },
    {
        { 0xa88141ecef842b6b, 0x55e7b14797abe6c5, 0x8c748f9703784ffe, 0x5b50a1f7afcd00b7 },
        { 0xc25ded54a4b8be41, 0x902d13e11bb0e2dd, 0x41f43233cde82ab2, 0x1085faa5c3aae7cb },
        { 0x9b840f66f1361315, 0x18462242701003e9, 0x65ed45fae4a25080, 0x0a2862393fda7320 }
    },
    {
        { 0x960e737b6ecb9d17, 0xfaf24948d67ceae1, 0x37e7a9b4d55e1b89, 0x5cb7173cb46c59eb },
        { 0x46ab13c8347cbc9d, 0x3849e8d499c12383, 0x4cea314087d64ac9, 0x1f354134b1a29ee7 },
        { 0x4a89e68b82b7abf0, 0xf41cd9279ba6b7b9, 0x16e6c210e18d876f, 0x7cacdb0f7f1b09c6 }
    },
},
{
    {
        { 0xe1014434dcc5caed, 0x47ed5d963c84fb33, 0x70019576ed86a0e7, 0x25b2697bd267f9e4 },
        { 0x9062b2e0d91a78bc, 0x47c9889cc8509667, 0x9df54a66405070b8, 0x7369e6a92493a1bf },
        { 0x9d673ffb13986864, 0x3ca5fbd9415dc7b8, 0xe04ecc3bdf273b5e, 0x1420683db54e4cd2 }
    },
    {
        { 0x34eebb6fc1cc5ad0, 0x6a1b0ce99646ac8b, 0xd3b0da49a66bde53, 0x31e83b4161d081c1 },
        { 0xb478bd1e249dd197, 0x620c35005e58c102, 0xfb02d32fccbaac5c, 0x60b63bebf508a72d },
        { 0x97e8c7129e062b4f, 0x49e48f4f29320ad8, 0x5bece14b6f18683f, 0x55cf1eb62d550317 }
    },
    {
        { 0x3076b5e37df58c52, 0xd73ab9dde799cc36, 0xbd831ce34913ee20, 0x1a56fbaa62ba0133 },
        { 0x5879101065c23d58, 0x8b9d086d5094819c, 0xe2402fa912c55fa7, 0x669a6564570891d4 },
        { 0x943e6b505c9dc9ec, 0x302557bba77c371a, 0x9873ae5641347651, 0x13c4836799c58a5c }
    },
    {
        { 0xc4dcfb6a5d8bd080, 0xdeebc4ec571a4842, 0xd4b2e883b8e55365, 0x50bdc87dc8e5b827 },
        { 0x423a5d465ab3e1b9, 0xfc13c187c7f13f61, 0x19f83664ecb5b9b6, 0x66f80c93a637b607 },
        { 0x606d37836edfe111, 0x32353e15f011abd9, 0x64b03ac325b73b96, 0x1dd56444725fd5ae }
    },
    {
        { 0xc297e60008bac89a, 0x7d4cea11eae1c3e0, 0xf3e38be19fe7977c, 0x3a3a450f63a305cd },
        { 0x8fa47ff83362127d, 0xbc9f6ac471cd7c15, 0x6e71454349220c8b, 0x0e645912219f732e },
        { 0x078f2f31d8394627, 0x389d3183de94a510, 0xd1e36c6d17996f80, 0x318c8d9393a9a87b }
    },
    {
        { 0x5d669e29ab1dd398, 0xfc921658342d9e3b, 0x55851dfdf35973cd, 0x509a41c325950af6 },
        { 0xf2745d032afffe19, 0x0c9f3c497f24db66, 0xbc98d3e3ba8598ef, 0x224c7c679a1d5314 },
        { 0xbdc06edca6f925e9, 0x793ef3f4641b1f33, 0x82ec12809d833e89, 0x05bff02328a11389 }
    },
    {
        { 0x6881a0dd0dc512e4, 0x4fe70dc844a5fafe, 0x1f748e6b8f4a5240, 0x576277cdee01a3ea },
        { 0x3632137023cae00b, 0x544acf0ad1accf59, 0x96741049d21a1c88, 0x780b8cc3fa2a44a7 },
        { 0x1ef38abc234f305f, 0x9a577fbd1405de08, 0x5e82a51434e62a0d, 0x5ff418726271b7a1 }
    },
    {
        { 0xe5db47e813b69540, 0xf35d2a3b432610e1, 0xac1f26e938781276, 0x29d4db8ca0a0cb69 },
        { 0x398e080c1789db9d, 0xa7602025f3e778f5, 0xfa98894c06bd035d, 0x106a03dc25a966be },
        { 0xd9ad0aaf333353d0, 0x38669da5acd309e5, 0x3c57658ac888f7f0, 0x4ab38a51052cbefa }
    },
},
{
    {
        { 0xf68fe2e8809de054, 0xe3bc096a9c82bad1, 0x076353d40aadbf45, 0x7b9b1fb5dea1959e },
        { 0xdfdacbee4324c0e9, 0x054442883f955bb7, 0xdef7aaa8ea31609f, 0x68aee70642287cff },
        { 0xf01cc8f17471cc0c, 0x95242e37579082bb, 0x27776093d3e46b5f, 0x2d13d55a28bd85fb }
    },
    {
        { 0xbf019cce7aee7a52, 0xa8ded2b6e454ead3, 0x3c619f0b87a8bb19, 0x3619b5d7560916d8 },
        { 0xfac5d2065b35b8da, 0xa8da8a9a85624bb7, 0xccd2ca913d21cd0f, 0x6b8341ee8bf90d58 },
        { 0x3579f26b0282c4b2, 0x64d592f24fafefae, 0xb7cded7b28c8c7c0, 0x6a927b6b7173a8d7 }
    },
    {
        { 0x8d7040863ece88eb, 0xf0e307a980eec08c, 0xac2250610d788fda, 0x056d92a43a0d478d },
        { 0x1f6db24f986e4656, 0x1021c02ed1e9105b, 0xf8ff3fff2cc0a375, 0x1d2a6bf8c6c82592 },
        { 0x1b05a196fc3da5a1, 0x77d7a8c243b59ed0, 0x06da3d6297d17918, 0x66fbb494f12353f7 }
    },
    {
        { 0xd6d70996f12309d6, 0xdbfb2385e9c3d539, 0x46d602b0f7552411, 0x270a0b0557843e0c },
        { 0x751a50b9d85c0fb8, 0xd1afdc258bcf097b, 0x2f16a6a38309a969, 0x14ddff9ee5b00659 },
        { 0x61ff0640a7862bcc, 0x81cac09a5f11abfe, 0x9047830455d12abb, 0x19a4bde1945ae873 }
    },
    {
        { 0x40c709dec076c49f, 0x657bfaf27f3e53f6, 0x40662331eca042c4, 0x14b375487eb4df04 },
        { 0x9b9f26f520a6200a, 0x64804443cf13eaf8, 0x8a63673f8631edd3, 0x72bbbce11ed39dc1 },
        { 0xae853c94ab66dc47, 0xeb62343edf762d6e, 0xf08e0e186fb2f7d1, 0x4f0b1c02700ab37a }
    },
    {
        { 0x79fd21ccc1b2e23f, 0x4ae7c281453df52a, 0xc8172ec9d151486b, 0x68abe9443e0a7534 },
        { 0xe1706787d81951fa, 0xa10a2c8eb290c77b, 0xe7382fa03ed66773, 0x0a4d84710bcc4b54 },
        { 0xda12c6c407831dcb, 0x0da230d74d5c510d, 0x4ab1531e6bd404e1, 0x4106b166bcf440ef }
    },
    {
        { 0xa485ccd539e4ecf2, 0x5aa3f3ad0555bab5, 0x145e3439937df82d, 0x1238b51e1214283f },
        { 0x02e57a421cd23668, 0x4ad9fb5d0eaef6fd, 0x954e6727b1244480, 0x7f792f9d2699f331 },
        { 0x0b886b925fd4d924, 0x60906f7a3626a80d, 0xecd367b4b98abd12, 0x2876beb1def344cf }
    },
    {
        { 0xd594b3333a8a85f8, 0x4ea37689e78d7d58, 0x73bf9f455e8e351f, 0x5507d7d2bc41ebb4 },
        { 0xdc84e93563144691, 0x632fe8a0d61f23f4, 0x4caa800612a9a8d5, 0x48f9dbfa0e9918d3 },
        { 0x1ceb2903299572fc, 0x7c8ccaa29502d0ee, 0x91bfa43411cce67b, 0x5784481964a831e7 }
    },
},
{
    {
        { 0xd6cfd1ef5fddc09c, 0xe82b3efdf7575dce, 0x25d56b5d201634c2, 0x3041c6bb04ed2b9b },
        { 0xda7c2b256768d593, 0x98c1c0574422ca13, 0xf1a80bd5ca0ace1d, 0x29cdd1adc088a690 },
        { 0x0ff2f2f9d956e148, 0xade797759f356b2e, 0x1a4698bb5f6c025c, 0x104bbd6814049a7b }
    },
    {
        { 0xa95d9a5fd67ff163, 0xe92be69d4cc75681, 0xb7f8024cde20f257, 0x204f2a20fb072df5 },
        { 0x51f0fd3168f1ed67, 0x2c811dcdd86f3bc2, 0x44dc5c4304d2f2de, 0x5be8cc57092a7149 },
        { 0xc8143b3d30ebb079, 0x7589155abd652e30, 0x653c3c318f6d5c31, 0x2570fb17c279161f }
    },
    {
        { 0x192ea9550bb8245a, 0xc8e6fba88f9050d1, 0x7986ea2d88a4c935, 0x241c5f91de018668 },
        { 0x3efa367f2cb61575, 0xf5f96f761cd6026c, 0xe8c7142a65b52562, 0x3dcb65ea53030acd },
        { 0x28d8172940de6caa, 0x8fbf2cf022d9733a, 0x16d7fcdd235b01d1, 0x08420edd5fcdf0e5 }
    },
    {
        { 0x0358c34e04f410ce, 0xb6135b5a276e0685, 0x5d9670c7ebb91521, 0x04d654f321db889c },
        { 0xcdff20ab8362fa4a, 0x57e118d4e21a3e6e, 0xe3179617fc39e62b, 0x0d9a53efbc1769fd },
        { 0x5e7dc116ddbdb5d5, 0x2954deb68da5dd2d, 0x1cb608173334a292, 0x4a7a4f2618991ad7 }
    },
    {
        { 0x24c3b291af372a4b, 0x93da8270718147f2, 0xdd84856486899ef2, 0x4a96314223e0ee33 },
        { 0xf4a718025fb15f95, 0x3df65f346b5c1b8f, 0xcdfcf08500e01112, 0x11b50c4cddd31848 },
        { 0xa6e8274408a4ffd6, 0x738e177e9c1576d9, 0x773348b63d02b3f2, 0x4f4bce4dce6bcc51 }
    },
    {
        { 0x30e2616ec49d0b6f, 0xe456718fcaec2317, 0x48eb409bf26b4fa6, 0x3042cee561595f37 },
        { 0xa71fce5ae2242584, 0x26ea725692f58a9e, 0xd21a09d71cea3cf4, 0x73fcdd14b71c01e6 },
        { 0x427e7079449bac41, 0x855ae36dbce2310a, 0x4cae76215f841a7c, 0x389e740c9a9ce1d6 }
    },
    {
        { 0xc9bd78f6570eac28, 0xe55b0b3227919ce1, 0x65fc3eaba19b91ed, 0x25c425e5d6263690 },
        { 0x64fcb3ae34dcb9ce, 0x97500323e348d0ad, 0x45b3f07d62c6381b, 0x61545379465a6788 },
        { 0x3f3e06a6f1d7de6e, 0x3ef976278e062308, 0x8c14f6264e8a6c77, 0x6539a08915484759 }
    },
    {
        { 0xddc4dbd414bb4a19, 0x19b2bc3c98424f8e, 0x48a89fd736ca7169, 0x0f65320ef019bd90 },
        { 0xe9d21f74c3d2f773, 0xc150544125c46845, 0x624e5ce8f9b99e33, 0x11c5e4aac5cd186c },
        { 0xd486d1b1cafde0c6, 0x4f3fe6e3163b5181, 0x59a8af0dfaf2939a, 0x4cabc7bdec33072a }
    },
},
{
    {
        { 0xf7c0a19c1a54a044, 0x4a1c5e2477bd9fbb, 0xa6e3ca115af22972, 0x1819bb953f2e9e0d },
        { 0x16faa8fb532f7428, 0xdbd42ea046a4e272, 0x5337653b8b9ea480, 0x4065947223973f03 },
        { 0x498fbb795e042e84, 0x7d0dd89a7698b714, 0x8bfb0ba427fe6295, 0x36ba82e721200524 }
    },
    {
        { 0xc8d69d0a57274ed5, 0x45ba803260804b17, 0xdf3cda102255dfac, 0x77d221232709b339 },
        { 0xd60ecbb74245ec41, 0xfd9be89e34348716, 0xc9240afee42284de, 0x4472f648d0531db4 },
        { 0x498a6d7064ad94d8, 0xa5b5c8fd9af62263, 0x8ca8ed0545c141f4, 0x2c63bec3662d358c }
    },
    {
        { 0x9a518b3a8586f8bf, 0x9ee71af6cbb196f0, 0xaa0625e6a2385cf2, 0x1deb2176ddd7c8d1 },
        { 0x7fe60d8bea787955, 0xb9dc117eb5f401b7, 0x91c7c09a19355cce, 0x22692ef59442bedf },
        { 0x8563d19a2066cf6c, 0x401bfd8c4dcc7cd7, 0xd976a6becd0d8f62, 0x67cfd773a278b05e }
    },
    {
        { 0x2d5fa9855a4e586a, 0x65f8f7a449beab7e, 0xaa074dddf21d33d3, 0x185cba721bcb9dee },
        { 0x8dec31faef3ee475, 0x99dbff8a9e22fd92, 0x512d11594e26cab1, 0x0cde561eec4310b9 },
        { 0x93869da3f4e3cb41, 0xbf0392f540f7977e, 0x026204fcd0463b83, 0x3ec91a769eec6eed }
    },
    {
        { 0x0fad2fb7b0a3402f, 0x46615ecbfb69f4a8, 0xf745bcc8c5f8eaa6, 0x7a5fa8794a94e896 },
        { 0x1e9df75bf78166ad, 0x4dfda838eb0cd7af, 0xba002ed8c1eaf988, 0x13fedb3e11f33cfc },
        { 0x52958faa13cd67a1, 0x965ee0818bdbb517, 0x16e58daa2e8845b3, 0x357d397d5499da8f }
    },
    {
        { 0x481dacb4194bfbf8, 0x4d77e3f1bae58299, 0x1ef4612e7d1372a0, 0x3a8d867e70ff69e1 },
        { 0x1ebfa05fb0bace6c, 0xc934620c1caf9a1e, 0xcc771cc41d82b61a, 0x2d94a16aa5f74fec },
        { 0x6f58cd5d55aff958, 0xba3eaa5c75567721, 0x75c123999165227d, 0x69be1343c2f2b35e }
    },
    {
        { 0x82bbbdac684b8de3, 0xa2f4c7d03fca0718, 0x337f92fbe096aaa8, 0x200d4d8c63587376 },
        { 0x0e091d5ee197c92a, 0x4f51019f2945119f, 0x143679b9f034e99c, 0x7d88112e4d24c696 },
        { 0x208aed4b4893b32b, 0x3efbf23ebe59b964, 0xd762deb0dba5e507, 0x69607bd681bd9d94 }
    },
    {
        { 0x3b7f3bd49323a902, 0x7c21b5566b2c6e53, 0xe5ba8ff53a7852a7, 0x28bc77a5838ece00 },
        { 0xf6be021068de1ce1, 0xe8d518e70edcbc1f, 0xe3effdd01b5505a5, 0x35f63353d3ec3fd0 },
        { 0x63ba78a8e25d8036, 0x63651e0094333490, 0x48d82f20288ce532, 0x3a31abfa36b57524 }
    },
},
{
    {
        { 0xc08f788f3f78d289, 0xfe30a72ca1404d9f, 0xf2778bfccf65cc9d, 0x7ee498165acb2021 },
        { 0x239e9624089c0a2e, 0xc748c4c03afe4738, 0x17dbed2a764fa12a, 0x639b93f0321c8582 },
        { 0x7bd508e39111a1c3, 0x2b2b90d480907489, 0xe7d2aec2ae72fd19, 0x0edf493c85b602a6 }
    },
    {
        { 0x6767c4d284764113, 0xa090403ff7f5f835, 0x1c8fcffacae6bede, 0x04c00c54d1dfa369 },
        { 0xaecc8158599b5a68, 0xea574f0febade20e, 0x4fe41d7422b67f07, 0x403b92e3019d4fb4 },
        { 0x4dc22f818b465cf8, 0x71a0f35a1480eff8, 0xaee8bfad04c7d657, 0x355bb12ab26176f4 }
    },
    {
        { 0xa301dac75a8c7318, 0xed90039db3ceaa11, 0x6f077cbf3bae3f2d, 0x7518eaf8e052ad8e },
        { 0xa71e64cc7493bbf4, 0xe5bd84d9eca3b0c3, 0x0a6bc50cfa05e785, 0x0f9b8132182ec312 },
        { 0xa48859c41b7f6c32, 0x0f2d60bcf4383298, 0x1815a929c9b1d1d9, 0x47c3871bbb1755c4 }
    },
    {
        { 0xfbe65d50c85066b0, 0x62ecc4b0b3a299b0, 0xe53754ea441ae8e0, 0x08fea02ce8d48d5f },
        { 0x5144539771ec4f48, 0xf805b17dc98c5d6e, 0xf762c11a47c3c66b, 0x00b89b85764699dc },
        { 0x824ddd7668deead0, 0xc86445204b685d23, 0xb514cfcd5d89d665, 0x473829a74f75d537 }
    },
    {
        { 0x23d9533aad3902c9, 0x64c2ddceef03588f, 0x15257390cfe12fb4, 0x6c668b4d44e4d390 },
        { 0x82d2da754679c418, 0xe63bd7d8b2618df0, 0x355eef24ac47eb0a, 0x2078684c4833c6b4 },
        { 0x3b48cf217a78820c, 0xf76a0ab281273e97, 0xa96c65a78c8eed7b, 0x7411a6054f8a433f }
    },
    {
        { 0x579ae53d18b175b4, 0x68713159f392a102, 0x8455ecba1eef35f5, 0x1ec9a872458c398f },
        { 0x4d659d32b99dc86d, 0x044cdc75603af115, 0xb34c712cdcc2e488, 0x7c136574fb8134ff },
        { 0xb8e6a4d400a2509b, 0x9b81d7020bc882b4, 0x57e7cc9bf1957561, 0x3add88a5c7cd6460 }
    },
    {
        { 0x85c298d459393046, 0x8f7e35985ff659ec, 0x1d2ca22af2f66e3a, 0x61ba1131a406a720 },
        { 0xab895770b635dcf2, 0x02dfef6cf66c1fbc, 0x85530268beb6d187, 0x249929fccc879e74 },
        { 0xa3d0a0f116959029, 0x023b6b6cba7ebd89, 0x7bf15a3e26783307, 0x5620310cbbd8ece7 }
    },
    {
        { 0x6646b5f477e285d6, 0x40e8ff676c8f6193, 0xa6ec7311abb594dd, 0x7ec846f3658cec4d },
        { 0x528993434934d643, 0xb9dbf806a51222f5, 0x8f6d878fc3f41c22, 0x37676a2a4d9d9730 },
        { 0x9b5e8f3f1da22ec7, 0x130f1d776c01cd13, 0x214c8fcfa2989fb8, 0x6daaf723399b9dd5 }
    },
},
{
    {
        { 0x81aebbdd2cd13070, 0x962e4325f85a0e9e, 0xde9391aacadffecb, 0x53177fda52c230e6 },
        { 0x591e4a5610628564, 0x2a4bb87ca8b4df34, 0xde2a2572e7a38e43, 0x3cbdabd9fee5046e },
        { 0xa7bc970650b9de79, 0x3d12a7fbc301b59b, 0x02652e68d36ae38c, 0x79d739835a6199dc }
    },
    {
        { 0x21c9d9920d591737, 0x9bea41d2e9b46cd6, 0xe20e84200d89bfca, 0x79d99f946eae5ff8 },
        { 0xd9354df64131c1bd, 0x758094a186ec5822, 0x4464ee12e459f3c2, 0x6c11fce4cb133282 },
        { 0xf17b483568673205, 0x387deae83caad96c, 0x61b471fd56ffe386, 0x31741195b745a599 }
    },
    {
        { 0x17f8ba683b02a047, 0x50212096feefb6c8, 0x70139be21556cbe2, 0x203e44a11d98915b },
        { 0xe8d10190b77a360b, 0x99b983209995e702, 0xbd4fdff8fa0247aa, 0x2772e344e0d36a87 },
        { 0xd6863eba37b9e39f, 0x105bc169723b5a23, 0x104f6459a65c0762, 0x567951295b4d38d4 }
    },
    {
        { 0x07242eb30d4b497f, 0x1ef96306b9bccc87, 0x37950934d8116f45, 0x05468d6201405b04 },
        { 0x535fd60613037524, 0xe210adf6b0fbc26a, 0xac8d0a9b23e990ae, 0x47204d08d72fdbf9 },
        { 0x00f565a9f93267de, 0xcecfd78dc0d58e8a, 0xa215e2dcf318e28e, 0x4599ee919b633352 }
    },
    {
        { 0xac746d6b861ae579, 0x31ab0650f6aea9dc, 0x241d661140256d4c, 0x2f485e853d21a5de },
        { 0xd3c220ca70e0e76b, 0xb12bea58ea9f3094, 0x294ddec8c3271282, 0x0c3539e1a1d1d028 },
        { 0x329744839c0833f3, 0x6fe6257fd2abc484, 0x5327d1814b358817, 0x65712585893fe9bc }
    },
    {
        { 0x81c29f1bd708ee3f, 0xddcb5a05ae6407d0, 0x97aec1d7d2a3eba7, 0x1590521a91d50831 },
        { 0x9c102fb732a61161, 0xe48e10dd34d520a8, 0x365c63546f9a9176, 0x32f6fe4c046f6006 },
        { 0x40a3a11ec7910acc, 0x9013dff8f16d27ae, 0x1a9720d8abb195d4, 0x1bb9fe452ea98463 }
    },
    {
        { 0xcf5e6c95cc36747c, 0x294201536b0bc30d, 0x453ac67cee797af0, 0x5eae6ab32a8bb3c9 },
        { 0xe9d1d950b3d54f9e, 0x2d5f9cbee00d33c1, 0x51c2c656a04fc6ac, 0x65c091ee3c1cbcc9 },
        { 0x7083661114f118ea, 0x2b37b87b94349cad, 0x7273f51cb4e99f40, 0x78a2a95823d75698 }
    },
    {
        { 0xb4f23c425ef83207, 0xabf894d3c9a934b5, 0xd0708c1339fd87f7, 0x1876789117166130 },
        { 0xa2b072e95c8c2ace, 0x69cffc96651e9c4b, 0x44328ef842e7b42b, 0x5dd996c122aadeb3 },
        { 0x925b5ef0670c507c, 0x819bc842b93c33bf, 0x10792e9a70dd003f, 0x59ad4b7a6e28dc74 }
    },
},
{
    {
        { 0x583b04bfacad8ea2, 0x29b743e8148be884, 0x2b1e583b0810c5db, 0x2b5449e58eb3bbaa },
        { 0x5f3a7562eb3dbe47, 0xf7ea38548ebda0b8, 0x00c3e53145747299, 0x1304e9e71627d551 },
        { 0x789814d26adc9cfe, 0x3c1bab3f8b48dd0b, 0xda0fe1fff979c60a, 0x4468de2d7c2dd693 }
    },
    {
        { 0x4b9ad8c6f86307ce, 0x21113531435d0c28, 0xd4a866c5657a772c, 0x5da6427e63247352 },
        { 0x51bb355e9419469e, 0x33e6dc4c23ddc754, 0x93a5b6d6447f9962, 0x6cce7c6ffb44bd63 },
        { 0x1a94c688deac22ca, 0xb9066ef7bbae1ff8, 0x88ad8c388d59580f, 0x58f29abfe79f2ca8 }
    },
    {
        { 0x4b5a64bf710ecdf6, 0xb14ce538462c293c, 0x3643d056d50b3ab9, 0x6af93724185b4870 },
        { 0xe90ecfab8de73e68, 0x54036f9f377e76a5, 0xf0495b0bbe015982, 0x577629c4a7f41e36 },
        { 0x3220024509c6a888, 0xd2e036134b558973, 0x83e236233c33289f, 0x701f25bb0caec18f }
    },
    {
        { 0x9d18f6d97cbec113, 0x844a06e674bfdbe4, 0x20f5b522ac4e60d6, 0x720a5bc050955e51 },
        { 0xc3a8b0f8e4616ced, 0xf700660e9e25a87d, 0x61e3061ff4bca59c, 0x2e0c92bfbdc40be9 },
        { 0x0c3f09439b805a35, 0xe84e8b376242abfc, 0x691417f35c229346, 0x0e9b9cbb144ef0ec }
    },
    {
        { 0x8dee9bd55db1beee, 0xc9c3ab370a723fb9, 0x44a8f1bf1c68d791, 0x366d44191cfd3cde },
        { 0xfbbad48ffb5720ad, 0xee81916bdbf90d0e, 0xd4813152635543bf, 0x221104eb3f337bd8 },
        { 0x9e3c1743f2bc8c14, 0x2eda26fcb5856c3b, 0xccb82f0e68a7fb97, 0x4167a4e6bc593244 }
    },
    {
        { 0xc2be2665f8ce8fee, 0xe967ff14e880d62c, 0xf12e6e7e2f364eee, 0x34b33370cb7ed2f6 },
        { 0x643b9d2876f62700, 0x5d1d9d400e7668eb, 0x1b4b430321fc0684, 0x7938bb7e2255246a },
        { 0xcdc591ee8681d6cc, 0xce02109ced85a753, 0xed7485c158808883, 0x1176fc6e2dfe65e4 }
    },
    {
        { 0xdb90e28949770eb8, 0x98fbcc2aacf440a3, 0x21354ffeded7879b, 0x1f6a3e54f26906b6 },
        { 0xb4af6cd05b9c619b, 0x2ddfc9f4b2a58480, 0x3d4fa502ebe94dc4, 0x08fc3a4c677d5f34 },
        { 0x60a4c199d30734ea, 0x40c085b631165cd6, 0xe2333e23f7598295, 0x4f2fad0116b900d1 }
    },
    {
        { 0x962cd91db73bb638, 0xe60577aafc129c08, 0x6f619b39f3b61689, 0x3451995f2944ee81 },
        { 0x44beb24194ae4e54, 0x5f541c511857ef6c, 0xa61e6b2d368d0498, 0x445484a4972ef7ab },
        { 0x9152fcd09fea7d7c, 0x4a816c94b0935cf6, 0x258e9aaa47285c40, 0x10b89ca6042893b7 }
    },
},
{
    {
        { 0xd67cded679d34aa0, 0xcc0b9ec0cc4db39f, 0xa535a456e35d190f, 0x2e05d9eaf61f6fef },
        { 0x9b2a426e3b646025, 0x32127190385ce4cf, 0xa25cffc2dd6dea45, 0x06409010bea8de75 },
        { 0xc447901ad61beb59, 0x661f19bce5dc880a, 0x24685482b7ca6827, 0x293c778cefe07f26 }
    },
    {
        { 0x16c795d6a11ff200, 0xcb70d0e2b15815c9, 0x89f293209b5395b5, 0x50b8c2d031e47b4f },
        { 0x86809e7007069096, 0xaad75b15e4e50189, 0x07f35715a21a0147, 0x0487f3f112815d5e },
        { 0x48350c08068a4962, 0x6ffdd05351092c9a, 0x17af4f4aaf6fc8dd, 0x4b0553b53cdba58b }
    },
    {
        { 0xbf05211b27c152d4, 0x5ec26849bd1af639, 0x5e0b2caa8e6fab98, 0x054c8bdd50bd0840 },
        { 0x9c65fcbe1b32ff79, 0xeb75ea9f03b50f9b, 0xfced2a6c6c07e606, 0x35106cd551717908 },
        { 0x38a0b12f1dcf073d, 0x4b60a8a3b7f6a276, 0xfed5ac25d3404f9a, 0x72e82d5e5505c229 }
    },
    {
        { 0x00d9cdfd69771d02, 0x410276cd6cfbf17e, 0x4c45306c1cb12ec7, 0x2857bf1627500861 },
        { 0x6b0b697ff0d844c8, 0xbb12f85cd979cb49, 0xd2a541c6c1da0f1f, 0x7b7c242958ce7211 },
        { 0x9f21903f0101689e, 0xd779dfd3bf861005, 0xa122ee5f3deb0f1b, 0x510df84b485a00d4 }
    },
    {
        { 0x24b3c887c70ac15e, 0xb0f3a557fb81b732, 0x9b2cde2fe578cc1b, 0x4cf7ed0703b54f8e },
        { 0xa54133bb9277a1fa, 0x74ec3b6263991237, 0x1a3c54dc35d2f15a, 0x2d347144e482ba3a },
        { 0x6bd47c6598fbee0f, 0x9e4733e2ab55be2d, 0x1093f624127610c5, 0x4e05e26ad0a1eaa4 }
    },
    {
        { 0x1833c773e18fe6c0, 0xe3c4711ad3c87265, 0x3bfd3c4f0116b283, 0x1955875eb4cd4db8 },
        { 0xda9b6b624b531f20, 0x429a760e77509abb, 0xdbe9f522e823cb80, 0x618f1856880c8f82 },
        { 0x6da6de8f0e399799, 0x7ad61aa440fda178, 0xb32cd8105e3563dd, 0x15f6beae2ae340ae }
    },
    {
        { 0xba9a0f7b9245e215, 0xf368612dd98c0dbb, 0x2e84e4cbf220b020, 0x6ba92fe962d90eda },
        { 0x862bcb0c31ec3a62, 0x810e2b451138f3c2, 0x788ec4b839dac2a4, 0x28f76867ae2a9281 },
        { 0x3e4df9655884e2aa, 0xbd62fbdbdbd465a5, 0xd7596caa0de9e524, 0x6e8042ccb2b1b3d7 }
    },
    {
        { 0x1530653616521f7e, 0x660d06b896203dba, 0x2d3989bc545f0879, 0x4b5303af78ebd7b0 },
        { 0xf10d3c29ce28ca6e, 0xbad34540fcb6093d, 0xe7426ed7a2ea2d3f, 0x08af9d4e4ff298b9 },
        { 0x72f8a6c3bebcbde8, 0x4f0fca4adc3a8e89, 0x6fa9d4e8c7bfdf7a, 0x0dcf2d679b624eb7 }
    },
},
{
    {
        { 0x753941be5a45f06e, 0xd07caeed6d9c5f65, 0x11776b9c72ff51b6, 0x17d2d1d9ef0d4da9 },
        { 0x3d5947499718289c, 0x12ebf8c524533f26, 0x0262bfcb14c3ef15, 0x20b878d577b7518e },
        { 0x27f2af18073f3e6a, 0xfd3fe519d7521069, 0x22e3b72c3ca60022, 0x72214f63cc65c6a7 }
    },
    {
        { 0x1d9db7b9f43b29c9, 0xd605824a4f518f75, 0xf2c072bd312f9dc4, 0x1f24ac855a1545b0 },
        { 0xb4e37f405307a693, 0xaba714d72f336795, 0xd6fbd0a773761099, 0x5fdf48c58171cbc9 },
        { 0x24d608328e9505aa, 0x4748c1d10c1420ee, 0xc7ffe45c06fb25a2, 0x00ba739e2ae395e6 }
    },
    {
        { 0xae4426f5ea88bb26, 0x360679d984973bfb, 0x5c9f030c26694e50, 0x72297de7d518d226 },
        { 0x592e98de5c8790d6, 0xe5bfb7d345c2a2df, 0x115a3b60f9b49922, 0x03283a3e67ad78f3 },
        { 0x48241dc7be0cb939, 0x32f19b4d8b633080, 0xd3dfc90d02289308, 0x05e1296846271945 }
    },
    {
        { 0xadbfbbc8242c4550, 0xbcc80cecd03081d9, 0x843566a6f5c8df92, 0x78cf25d38258ce4c },
        { 0xba82eeb32d9c495a, 0xceefc8fcf12bb97c, 0xb02dabae93b5d1e0, 0x39c00c9c13698d9b },
        { 0x15ae6b8e31489d68, 0xaa851cab9c2bf087, 0xc9a75a97f04efa05, 0x006b52076b3ff832 }
    },
    {
        { 0xf5cb7e16b9ce082d, 0x3407f14c417abc29, 0xd4b36bce2bf4a7ab, 0x7de2e9561a9f75ce },
        { 0x29e0cfe19d95781c, 0xb681df18966310e2, 0x57df39d370516b39, 0x4d57e3443bc76122 },
        { 0xde70d4f4b6a55ecb, 0x4801527f5d85db99, 0xdbc9c440d3ee9a81, 0x6b2a90af1a6029ed }
    },
    {
        { 0x77ebf3245bb2d80a, 0xd8301b472fb9079b, 0xc647e6f24cee7333, 0x465812c8276c2109 },
        { 0x6923f4fc9ae61e97, 0x5735281de03f5fd1, 0xa764ae43e6edd12d, 0x5fd8f4e9d12d3e4a },
        { 0x4d43beb22a1062d9, 0x7065fb753831dc16, 0x180d4a7bde2968d7, 0x05b32c2b1cb16790 }
    },
    {
        { 0xf7fca42c7ad58195, 0x3214286e4333f3cc, 0xb6c29d0d340b979d, 0x31771a48567307e1 },
        { 0xc8c05eccd24da8fd, 0xa1cf1aac05dfef83, 0xdbbeeff27df9cd61, 0x3b5556a37b471e99 },
        { 0x32b0c524e14dd482, 0xedb351541a2ba4b6, 0xa3d16048282b5af3, 0x4fc079d27a7336eb }
    },
    {
        { 0xdc348b440c86c50d, 0x1337cbc9cc94e651, 0x6422f74d643e3cb9, 0x241170c2bae3cd08 },
        { 0x51c938b089bf2f7f, 0x2497bd6502dfe9a7, 0xffffc09c7880e453, 0x124567cecaf98e92 },
        { 0x3ff9ab860ac473b4, 0xf0911dee0113e435, 0x4ae75060ebc6c4af, 0x3f8612966c87000d }
    },
},
{
    {
        { 0x529fdffe638c7bf3, 0xdf2b9e60388b4995, 0xe027b34f1bad0249, 0x7bc92fc9b9fa74ed },
        { 0x0c9c5303f7957be4, 0xa3c31a20e085c145, 0xb0721d71d0850050, 0x0aba390eab0bf2da },
        { 0x9f97ef2e801ad9f9, 0x83697d5479afda3a, 0xe906b3ffbd596b50, 0x02672b37dd3fb8e0 }
    },
    {
        { 0xee9ba729398ca7f5, 0xeb9ca6257a4849db, 0x29eb29ce7ec544e1, 0x232ca21ef736e2c8 },
        { 0x48b2ca8b260885e4, 0xa4286bec82b34c1c, 0x937e1a2617f58f74, 0x741d1fcbab2ca2a5 },
        { 0xbf61423d253fcb17, 0x08803ceafa39eb14, 0xf18602df9851c7af, 0x0400f3a049e3414b }
    },
    {
        { 0x2efba412a06e7b06, 0x146785452c8d2560, 0xdf9713ebd67a91c7, 0x32830ac7157eadf3 },
        { 0xabce0476ba61c55b, 0x36a3d6d7c4d39716, 0x6eb259d5e8d82d09, 0x0c9176e984d756fb },
        { 0x0e782a7ab73769e8, 0x04a05d7875b18e2c, 0x29525226ebcceae1, 0x0d794f8383eba820 }
    },
    {
        { 0x7be44ce7a7a2e1ac, 0x411fd93efad1b8b7, 0x1734a1d70d5f7c9b, 0x0d6592233127db16 },
        { 0xff35f5cb9e1516f4, 0xee805bcf648aae45, 0xf0d73c2bb93a9ef3, 0x097b0bf22092a6c2 },
        { 0xc48bab1521a9d733, 0xa6c2eaead61abb25, 0x625c6c1cc6cb4305, 0x7fc90fea93eb3a67 }
    },
    {
        { 0xc527deb59c7cb23d, 0x955391695328404e, 0xd64392817ccf2c7a, 0x6ce97dabf7d8fa11 },
        { 0x0408f1fe1f5c5926, 0x1a8f2f5e3b258bf4, 0x40a951a2fdc71669, 0x6598ee93c98b577e },
        { 0x25b5a8e50ef7c48f, 0xeb6034116f2ce532, 0xc5e75173e53de537, 0x73119fa08c12bb03 }
    },
    {
        { 0x7845b94d21f4774d, 0xbf62f16c7897b727, 0x671857c03c56522b, 0x3cd6a85295621212 },
        { 0xed30129453f1a4cb, 0xbce621c9c8f53787, 0xfacb2b1338bee7b9, 0x3025798a9ea8428c },
        { 0x3fecde923aeca999, 0xbdaa5b0062e8c12f, 0x67b99dfc96988ade, 0x3f52c02852661036 }
    },
    {
        { 0x9258bf99eec416c6, 0xac8a5017a9d2f671, 0x629549ab16dea4ab, 0x05d0e85c99091569 },
        { 0xffeaa48e2a1351c6, 0x28624754fa7f53d7, 0x0b5ba9e57582ddf1, 0x60c0104ba696ac59 },
        { 0x051de020de9cbe97, 0xfa07fc56b50bcf74, 0x378cec9f0f11df65, 0x36853c69ab96de4d }
    },
    {
        { 0x4433c0b0fac5e7be, 0x724bae854c08dcbe, 0xf1f24cc446978f9b, 0x4a0aff6d62825fc8 },
        { 0x36d9b8de78f39b2d, 0x7f42ed71a847b9ec, 0x241cd1d679bd3fde, 0x6a704fec92fbce6b },
        { 0xe917fb9e61095301, 0xc102df9402a092f8, 0xbf09e2f5fa66190b, 0x681109bee0dcfe37 }
    },
},
{
    {
        { 0x9c18fcfa36048d13, 0x29159db373899ddd, 0xdc9f350b9f92d0aa, 0x26f57eee878a19d4 },
        { 0x559a0cc9782a0dde, 0x551dcdb2ea718385, 0x7f62865b31ef238c, 0x504aa7767973613d },
        { 0x0cab2cd55687efb1, 0x5180d162247af17b, 0x85c15a344f5a2467, 0x4041943d9dba3069 }
    },
    {
        { 0x4b217743a26caadd, 0x47a6b424648ab7ce, 0xcb1d4f7a03fbc9e3, 0x12d931429800d019 },
        { 0xc3c0eeba43ebcc96, 0x8d749c9c26ea9caf, 0xd9fa95ee1c77ccc6, 0x1420a1d97684340f },
        { 0x00c67799d337594f, 0x5e3c5140b23aa47b, 0x44182854e35ff395, 0x1b4f92314359a012 }
    },
    {
        { 0x33cf3030a49866b1, 0x251f73d2215f4859, 0xab82aa4051def4f6, 0x5ff191d56f9a23f6 },
        { 0x3e5c109d89150951, 0x39cefa912de9696a, 0x20eae43f975f3020, 0x239b572a7f132dae },
        { 0x819ed433ac2d9068, 0x2883ab795fc98523, 0xef4572805593eb3d, 0x020c526a758f36cb }
    },
    {
        { 0xe931ef59f042cc89, 0x2c589c9d8e124bb6, 0xadc8e18aaec75997, 0x452cfe0a5602c50c },
        { 0x779834f89ed8dbbc, 0xc8f2aaf9dc7ca46c, 0xa9524cdca3e1b074, 0x02aacc4615313877 },
        { 0x86a0f7a0647877df, 0xbbc464270e607c9f, 0xab17ea25f1fb11c9, 0x4cfb7d7b304b877b }
    },
    {
        { 0xe28699c29789ef12, 0x2b6ecd71df57190d, 0xc343c857ecc970d0, 0x5b1d4cbc434d3ac5 },
        { 0x72b43d6cb89b75fe, 0x54c694d99c6adc80, 0xb8c3aa373ee34c9f, 0x14b4622b39075364 },
        { 0xb6fb2615cc0a9f26, 0x3a4f0e2bb88dcce5, 0x1301498b3369a705, 0x2f98f71258592dd1 }
    },
    {
        { 0x2e12ae444f54a701, 0xfcfe3ef0a9cbd7de, 0xcebf890d75835de0, 0x1d8062e9e7614554 },
        { 0x0c94a74cb50f9e56, 0x5b1ff4a98e8e1320, 0x9a2acc2182300f67, 0x3a6ae249d806aaf9 },
        { 0x657ada85a9907c5a, 0x1a0ea8b591b90f62, 0x8d0e1dfbdf34b4e9, 0x298b8ce8aef25ff3 }
    },
    {
        { 0x837a72ea0a2165de, 0x3fab07b40bcf79f6, 0x521636c77738ae70, 0x6ba6271803a7d7dc },
        { 0x2a927953eff70cb2, 0x4b89c92a79157076, 0x9418457a30a7cf6a, 0x34b8a8404d5ce485 },
        { 0xc26eecb583693335, 0xd5a813df63b5fefd, 0xa293aa9aa4b22573, 0x71d62bdd465e1c6a }
    },
    {
        { 0xcd2db5dab1f75ef5, 0xd77f95cf16b065f5, 0x14571fea3f49f085, 0x1c333621262b2b3d },
        { 0x6533cc28d378df80, 0xf6db43790a0fa4b4, 0xe3645ff9f701da5a, 0x74d5f317f3172ba4 },
        { 0xa86fe55467d9ca81, 0x398b7c752b298c37, 0xda6d0892e3ac623b, 0x4aebcc4547e9d98c }
    },
},
{
    {
        { 0x0de9b204a059a445, 0xe15cb4aa4b17ad0f, 0xe1bbec521f79c557, 0x2633f1b9d071081b },
        { 0x53175a7205d21a77, 0xb0c04422d3b934d4, 0xadd9f24bdd5deadc, 0x074f46e69f10ff8c },
        { 0xc1fb4177018b9910, 0xa6ea20dc6c0fe140, 0xd661f3e74354c6ff, 0x5ecb72e6f1a3407a }
    },
    {
        { 0xfeeae106e8e86997, 0x9863337f98d09383, 0x9470480eaa06ebef, 0x038b6898d4c5c2d0 },
        { 0xa515a31b2259fb4e, 0x0960f3972bcac52f, 0xedb52fec8d3454cb, 0x382e2720c476c019 },
        { 0xf391c51d8ace50a6, 0x3142d0b9ae2d2948, 0xdb4d5a1a7f24ca80, 0x21aeba8b59250ea8 }
    },
    {
        { 0x53853600f0087f23, 0x4c461879da7d5784, 0x6af303deb41f6860, 0x0a3c16c5c27c18ed },
        { 0x24f13b34cf405530, 0x3c44ea4a43088af7, 0x5dd5c5170006a482, 0x118eb8f8890b086d },
        { 0x17e49c17cc947f3d, 0xccc6eda6aac1d27b, 0xdf6092ceb0f08e56, 0x4909b3e22c67c36b }
    },
    {
        { 0x59a16676706ff64e, 0x10b953dd0d86a53d, 0x5848e1e6ce5c0b96, 0x2d8b78e712780c68 },
        { 0x9c9c85ea63fe2e89, 0xbe1baf910e9412ec, 0x8f7baa8a86fbfe7b, 0x0fb17f9fef968b6c },
        { 0x79d5c62eafc3902b, 0x773a215289e80728, 0xc38ae640e10120b9, 0x09ae23717b2b1a6d }
    },
    {
        { 0x10ab8fa1ad32b1d0, 0xe9aced1be2778b24, 0xa8856bc0373de90f, 0x66f35ddddda53996 },
        { 0xbb6a192a4e4d083c, 0x34ace0630029e192, 0x98245a59aafabaeb, 0x6d9c8a9ada97faac },
        { 0xd27d9afb24997323, 0x1bb7e07ef6f01d2e, 0x2ba7472df52ecc7f, 0x03019b4f646f9dc8 }
    },
    {
        { 0xaf09b214e6b3dc6b, 0x3f7573b5ad7d2f65, 0xd019d988100a23b0, 0x392b63a58b5c35f7 },
        { 0x04a186b5565345cd, 0xeee76610bcc4116a, 0x689c73b478fb2a45, 0x387dcbff65697512 },
        { 0x4093addc9c07c205, 0xc565be15f532c37e, 0x63dbecfd1583402a, 0x61722b4aef2e032e }
    },
    {
        { 0xd6b07a5581cb0e3c, 0x290ff006d9444969, 0x08680b6a16dcda1f, 0x5568d2b75a06de59 },
        { 0x0012aafeecbd47af, 0x55a266fb1cd46309, 0xf203eb680967c72c, 0x39633944ca3c1429 },
        { 0x8d0cb88c1b37cfe1, 0x05b6a5a3053818f3, 0xf2e9bc04b787d959, 0x6beba1249add7f64 }
    },
    {
        { 0x5c3cecb943f5a53b, 0x9cc9a61d06c08df2, 0xcfba639a85895447, 0x5a845ae80df09fd5 },
        { 0x1d06005ca5b1b143, 0x6d4c6bb87fd1cda2, 0x6ef5967653fcffe7, 0x097c29e8c1ce1ea5 },
        { 0x4ce97dbe5deb94ca, 0x38d0a4388c709c48, 0xc43eced4a169d097, 0x0a1249fff7e587c3 }
    },
},
{
    {
        { 0x0b408d9e7354b610, 0x806b32535ba85b6e, 0xdbe63a034a58a207, 0x173bd9ddc9a1df2c },
        { 0x12f0071b276d01c9, 0xe7b8bac586c48c70, 0x5308129b71d6fba9, 0x5d88fbf95a3db792 },
        { 0x2b500f1efe5872df, 0x58d6582ed43918c1, 0xe6ed278ec9673ae0, 0x06e1cd13b19ea319 }
    },
    {
        { 0x472baf629e5b0353, 0x3baa0b90278d0447, 0x0c785f469643bf27, 0x7f3a6a1a8d837b13 },
        { 0x40d0ad516f166f23, 0x118e32931fab6abe, 0x3fe35e14a04d088e, 0x3080603526e16266 },
        { 0xf7e644395d3d800b, 0x95a8d555c901edf6, 0x68cd7830592c6339, 0x30d0fded2e51307e }
    },
    {
        { 0x9cb4971e68b84750, 0xa09572296664bbcf, 0x5c8de72672fa412b, 0x4615084351c589d9 },
        { 0xe0594d1af21233b3, 0x1bdbe78ef0cc4d9c, 0x6965187f8f499a77, 0x0a9214202c099868 },
        { 0xbc9019c0aeb9a02e, 0x55c7110d16034cae, 0x0e6df501659932ec, 0x3bca0d2895ca5dfe }
    },
    {
        { 0x9c688eb69ecc01bf, 0xf0bc83ada644896f, 0xca2d955f5f7a9fe2, 0x4ea8b4038df28241 },
        { 0x40f031bc3c5d62a4, 0x19fc8b3ecff07a60, 0x98183da2130fb545, 0x5631deddae8f13cd },
        { 0x2aed460af1cad202, 0x46305305a48cee83, 0x9121774549f11a5f, 0x24ce0930542ca463 }
    },
    {
        { 0x3fcfa155fdf30b85, 0xd2f7168e36372ea4, 0xb2e064de6492f844, 0x549928a7324f4280 },
        { 0x1fe890f5fd06c106, 0xb5c468355d8810f2, 0x827808fe6e8caf3e, 0x41d4e3c28a06d74b },
        { 0xf26e32a763ee1a2e, 0xae91e4b7d25ffdea, 0xbc3bd33bd17f4d69, 0x491b66dec0dcff6a }
    },
    {
        { 0x75f04a8ed0da64a1, 0xed222caf67e2284b, 0x8234a3791f7b7ba4, 0x4cf6b8b0b7018b67 },
        { 0x98f5b13dc7ea32a7, 0xe3d5f8cc7e16db98, 0xac0abf52cbf8d947, 0x08f338d0c85ee4ac },
        { 0xc383a821991a73bd, 0xab27bc01df320c7a, 0xc13d331b84777063, 0x530d4a82eb078a99 }
    },
    {
        { 0x6d6973456c9abf9e, 0x257fb2fc4900a880, 0x2bacf412c8cfb850, 0x0db3e7e00cbfbd5b },
        { 0x004c3630e1f94825, 0x7e2d78268cab535a, 0xc7482323cc84ff8b, 0x65ea753f101770b9 },
        { 0x3d66fc3ee2096363, 0x81d62c7f61b5cb6b, 0x0fbe044213443b1a, 0x02a4ec1921e1a1db }
    },
    {
        { 0xf5c86162f1cf795f, 0x118c861926ee57f2, 0x172124851c063578, 0x36d12b5dec067fcf },
        { 0x5ce6259a3b24b8a2, 0xb8577acc45afa0b8, 0xcccbe6e88ba07037, 0x3d143c51127809bf },
        { 0x126d279179154557, 0xd5e48f5cfc783a0a, 0x36bdb6e8df179bac, 0x2ef517885ba82859 }
    },
},
{
    {
        { 0x1ea436837c6da1e9, 0xf9c189af1fb9bdbe, 0x303001fcce5dd155, 0x28a7c99ebc57be52 },
        { 0x88bd438cd11e0d4a, 0x30cb610d43ccf308, 0xe09a0e3791937bcc, 0x4559135b25b1720c },
        { 0xb8fd9399e8d19e9d, 0x908191cb962423ff, 0xb2b948d747c742a3, 0x37f33226d7fb44c4 }
    },
    {
        { 0x33912553c821b11d, 0x66ed42c241e301df, 0x066fcc11104222fd, 0x307a3b41c192168f },
        { 0x0dae8767b55f6e08, 0x4a43b3b35b203a02, 0xe3725a6e80af8c79, 0x0f7a7fd1705fa7a3 },
        { 0x8eeb5d076eb55ce0, 0x2fc536bfaa0d925a, 0xbe81830fdcb6c6e8, 0x556c7045827baf52 }
    },
    {
        { 0xb94b90022bf44406, 0xabd4237eff90b534, 0x7600a960faf86d3a, 0x2f45abdac2322ee3 },
        { 0x8e2b517302e9d8b7, 0xe3e52269248714e8, 0xbd4fbd774ca960b5, 0x6f4b4199c5ecada9 },
        { 0x61af4912c8ef8a6a, 0xe58fa4fe43fb6e5e, 0xb5afcc5d6fd427cf, 0x6a5393281e1e11eb }
    },
    {
        { 0x0fff04fe149443cf, 0x53cac6d9865cddd7, 0x31385b03531ed1b7, 0x5846a27cacd1039d },
        { 0xf3da5139a5d1ee89, 0x8145457cff936988, 0x3f622fed00e188c4, 0x0f513815db8b5a3d },
        { 0x4ff5cdac1eb08717, 0x67e8b29590f2e9bc, 0x44093b5e237afa99, 0x0d414bed8708b8b2 }
    },
    {
        { 0x81886a92294ac9e8, 0x23162b45d55547be, 0x94cfbc4403715983, 0x50eb8fdb134bc401 },
        { 0xcfb68265fd0e75f6, 0xe45b3e28bb90e707, 0x7242a8de9ff92c7a, 0x685b3201933202dd },
        { 0xc0b73ec6d6b330cd, 0x84e44807132faff1, 0x732b7352c4a5dee1, 0x5d7c7cf1aa7cd2d2 }
    },
    {
        { 0x33d1013e9b73a562, 0x925cef5748ec26e1, 0xa7fce614dd468058, 0x78b0fad41e9aa438 },
        { 0xaf3b46bf7a4aafa2, 0xb78705ec4d40d411, 0x114f0c6aca7c15e3, 0x3f364faaa9489d4d },
        { 0xbf56a431ed05b488, 0xa533e66c9c495c7e, 0xe8652baf87f3651a, 0x0241800059d66c33 }
    },
    {
        { 0x28350c7dcf38ea01, 0x7c6cdbc0b2917ab6, 0xace7cfbe857082f7, 0x4d2845aba2d9a1e0 },
        { 0xceb077fea37a5be4, 0xdb642f02e5a5eeb7, 0xc2e6d0c5471270b8, 0x4771b65538e4529c },
        { 0xbb537fe0447070de, 0xcba744436dd557df, 0xd3b5a3473600dbcb, 0x4aeabbe6f9ffd7f8 }
    },
    {
        { 0x6a2134bcc4a9c8f2, 0xfbf8fd1c8ace2e37, 0x000ae3049911a0ba, 0x046e3a616bc89b9e },
        { 0x4630119e40d8f78c, 0xa01a9bc53c710e11, 0x486d2b258910dd79, 0x1e6c47b3db0324e5 },
        { 0x14e65442f03906be, 0x4a019d54e362be2a, 0x68ccdfec8dc230c7, 0x7cfb7e3faf6b861c }
    },
},
{
    {
        { 0x96eebffb305b2f51, 0xd3f938ad889596b8, 0xf0f52dc746d5dd25, 0x57968290bb3a0095 },
        { 0x4637974e8c58aedc, 0xb9ef22fbabf041a4, 0xe185d956e980718a, 0x2f1b78fab143a8a6 },
        { 0xf71ab8430a20e101, 0xf393658d24f0ec47, 0xcf7509a86ee2eed1, 0x7dc43e35dc2aa3e1 }
    },
    {
        { 0x5a782a5c273e9718, 0x3576c6995e4efd94, 0x0f2ed8051f237d3e, 0x044fb81d82d50a99 },
        { 0x85966665887dd9c3, 0xc90f9b314bb05355, 0xc6e08df8ef2079b1, 0x7ef72016758cc12f },
        { 0xc1df18c5a907e3d9, 0x57b3371dce4c6359, 0xca704534b201bb49, 0x7f79823f9c30dd2e }
    },
    {
        { 0x6a9c1ff068f587ba, 0x0827894e0050c8de, 0x3cbf99557ded5be7, 0x64a9b0431c06d6f0 },
        { 0x8334d239a3b513e8, 0xc13670d4b91fa8d8, 0x12b54136f590bd33, 0x0a4e0373d784d9b4 },
        { 0x2eb3d6a15b7d2919, 0xb0b4f6a0d53a8235, 0x7156ce4389a45d47, 0x071a7d0ace18346c }
    },
    {
        { 0xcc0c355220e14431, 0x0d65950709b15141, 0x9af5621b209d5f36, 0x7c69bcf7617755d3 },
        { 0xd3072daac887ba0b, 0x01262905bfa562ee, 0xcf543002c0ef768b, 0x2c3bcc7146ea7e9c },
        { 0x07f0d7eb04e8295f, 0x10db18252f50f37d, 0xe951a9a3171798d7, 0x6f5a9a7322aca51d }
    },
    {
        { 0xe729d4eba3d944be, 0x8d9e09408078af9e, 0x4525567a47869c03, 0x02ab9680ee8d3b24 },
        { 0x8ba1000c2f41c6c5, 0xc49f79c10cfefb9b, 0x4efa47703cc51c9f, 0x494e21a2e147afca },
        { 0xefa48a85dde50d9a, 0x219a224e0fb9a249, 0xfa091f1dd91ef6d9, 0x6b5d76cbea46bb34 }
    },
    {
        { 0xe0f941171e782522, 0xf1e6ae74036936d3, 0x408b3ea2d0fcc746, 0x16fb869c03dd313e },
        { 0x8857556cec0cd994, 0x6472dc6f5cd01dba, 0xaf0169148f42b477, 0x0ae333f685277354 },
        { 0x288e199733b60962, 0x24fc72b4d8abe133, 0x4811f7ed0991d03e, 0x3f81e38b8f70d075 }
    },
    {
        { 0x0adb7f355f17c824, 0x74b923c3d74299a4, 0xd57c3e8bcbf8eaf7, 0x0ad3e2d34cdedc3d },
        { 0x7f910fcc7ed9affe, 0x545cb8a12465874b, 0xa8397ed24b0c4704, 0x50510fc104f50993 },
        { 0x6f0c0fc5336e249d, 0x745ede19c331cfd9, 0xf2d6fd0009eefe1c, 0x127c158bf0fa1ebe }
    },
    {
        { 0xdea28fc4ae51b974, 0x1d9973d3744dfe96, 0x6240680b873848a8, 0x4ed82479d167df95 },
        { 0xf6197c422e9879a2, 0xa44addd452ca3647, 0x9b413fc14b4eaccb, 0x354ef87d07ef4f68 },
        { 0xfee3b52260c5d975, 0x50352efceb41b0b8, 0x8808ac30a9f6653c, 0x302d92d20539236d }
    },
},
{
    {
        { 0x957b8b8b0df53c30, 0x2a1c770a8e60f098, 0xbbc7a670345796de, 0x22a48f9a90c99bc9 },
        { 0x4c59023fcb3efb7c, 0x6c2fcb99c63c2a94, 0xba4190e2c3c7e084, 0x0e545daea51874d9 },
        { 0x6b7dc0dc8d3fac58, 0x5497cd6ce6e42bfd, 0x542f7d1bf400d305, 0x4159f47f048d9136 }
    },
    {
        { 0x748515a8bbd24839, 0x77128347afb02b55, 0x50ba2ac649a2a17f, 0x060525513ad730f1 },
        { 0x20ad660839e31e32, 0xf81e1bd58405be50, 0xf8064056f4dabc69, 0x14d23dd4ce71b975 },
        { 0xf2398e098aa27f82, 0x6d7982bb89a1b024, 0xfa694084214dd24c, 0x71ab966fa32301c3 }
    },
    {
        { 0xb1088a0702809955, 0x43b273ea0b43c391, 0xca9b67aefe0686ed, 0x605eecbf8335f4ed },
        { 0x2dcbd8e34ded02fc, 0x1151f3ec596f22aa, 0xbca255434e0328da, 0x35768fbe92411b22 },
        { 0x83200a656c340431, 0x9fcd71678ee59c2f, 0x75d4613f71300f8a, 0x7a912faf60f542f9 }
    },
    {
        { 0x253f4f8dfa2d5597, 0x25e49c405477130c, 0x00c052e5996b1102, 0x33cb966e33bb6c4a },
        { 0xb204585e5edc1a43, 0x9f0e16ee5897c73c, 0x5b82c0ae4e70483c, 0x624a170e2bddf9be },
        { 0x597028047f116909, 0x828ac41c1e564467, 0x70417dbde6217387, 0x721627aefbac4384 }
    },
    {
        { 0xfd3097bc410b2f22, 0xf1a05da7b5cfa844, 0x61289a1def57ca74, 0x245ea199bb821902 },
        { 0x97d03bc38736add5, 0x2f1422afc532b130, 0x3aa68a057101bbc4, 0x4c946cf7e74f9fa7 },
        { 0xaedca66978d477f8, 0x1898ba3c29117fe1, 0xcf73f983720cbd58, 0x67da12e6b8b56351 }
    },
    {
        { 0x2b7ef3d38ec8308c, 0x828fd7ec71eb94ab, 0x807c3b36c5062abd, 0x0cb64cb831a94141 },
        { 0x7067e187b4bd6e07, 0x6e8f0203c7d1fe74, 0x93c6aa2f38c85a30, 0x76297d1f3d75a78a },
        { 0x3030fc33534c6378, 0xb9635c5ce541e861, 0x15d9a9bed9b2c728, 0x49233ea3f3775dcb }
    },
    {
        { 0x7b3985fe1c9f249b, 0x4fd6b2d5a1233293, 0xceb345941adf4d62, 0x6987ff6f542de50c },
        { 0x629398fa8dbffc3a, 0xe12fe52dd54db455, 0xf3be11dfdaf25295, 0x628b140dce5e7b51 },
        { 0x47e241428f83753c, 0x6317bebc866af997, 0xdabb5b433d1a9829, 0x074d8d245287fb2d }
    },
    {
        { 0x481875c6c0e31488, 0x219429b2e22034b4, 0x7223c98a31283b65, 0x3420d60b342277f9 },
        { 0x8337d9cd440bfc31, 0x729d2ca1af318fd7, 0xa040a4a4772c2070, 0x46002ef03a7349be },
        { 0xfaa23adeaffe65f7, 0x78261ed45be0764c, 0x441c0a1e2f164403, 0x5aea8e567a87d395 }
    },
},
{
    {
        { 0x2dbc6fb6e4e0f177, 0x04e1bf29a4bd6a93, 0x5e1966d4787af6e8, 0x0edc5f5eb426d060 },
        { 0x7813c1a2bca4283d, 0xed62f091a1863dd9, 0xaec7bcb8c268fa86, 0x10e5d3b76f1cae4c },
        { 0x5453bfd653da8e67, 0xe9dc1eec24a9f641, 0xbf87263b03578a23, 0x45b46c51361cba72 }
    },
    {
        { 0xce9d4ddd8a7fe3e4, 0xab13645676620e30, 0x4b594f7bb30e9958, 0x5c1c0aef321229df },
        { 0xa9402abf314f7fa1, 0xe257f1dc8e8cf450, 0x1dbbd54b23a8be84, 0x2177bfa36dcb713b },
        { 0x37081bbcfa79db8f, 0x6048811ec25f59b3, 0x087a76659c832487, 0x4ae619387d8ab5bb }
    },
    {
        { 0x61117e44985bfb83, 0xfce0462a71963136, 0x83ac3448d425904b, 0x75685abe5ba43d64 },
        { 0x8ddbf6aa5344a32e, 0x7d88eab4b41b4078, 0x5eb0eb974a130d60, 0x1a00d91b17bf3e03 },
        { 0x6e960933eb61f2b2, 0x543d0fa8c9ff4952, 0xdf7275107af66569, 0x135529b623b0e6aa }
    },
    {
        { 0xf5c716bce22e83fe, 0xb42beb19e80985c1, 0xec9da63714254aae, 0x5972ea051590a613 },
        { 0x18f0dbd7add1d518, 0x979f7888cfc11f11, 0x8732e1f07114759b, 0x79b5b81a65ca3a01 },
        { 0x0fd4ac20dc8f7811, 0x9a9ad294ac4d4fa8, 0xc01b2d64b3360434, 0x4f7e9c95905f3bdb }
    },
    {
        { 0x71c8443d355299fe, 0x8bcd3b1cdbebead7, 0x8092499ef1a49466, 0x1942eec4a144adc8 },
        { 0x62674bbc5781302e, 0xd8520f3989addc0f, 0x8c2999ae53fbd9c6, 0x31993ad92e638e4c },
        { 0x7dac5319ae234992, 0x2c1b3d910cea3e92, 0x553ce494253c1122, 0x2a0a65314ef9ca75 }
    },
    {
        { 0xcf361acd3c1c793a, 0x2f9ebcac5a35bc3b, 0x60e860e9a8cda6ab, 0x055dc39b6dea1a13 },
        { 0x2db7937ff7f927c2, 0xdb741f0617d0a635, 0x5982f3a21155af76, 0x4cf6e218647c2ded },
        { 0xb119227cc28d5bb6, 0x07e24ebc774dffab, 0xa83c78cee4a32c89, 0x121a307710aa24b6 }
    },
    {
        { 0xd659713ec77483c9, 0x88bfe077b82b96af, 0x289e28231097bcd3, 0x527bb94a6ced3a9b },
        { 0xe4db5d5e9f034a97, 0xe153fc093034bc2d, 0x460546919551d3b1, 0x333fc76c7a40e52d },
        { 0x563d992a995b482e, 0x3405d07c6e383801, 0x485035de2f64d8e5, 0x6b89069b20a7a9f7 }
    },
    {
        { 0x4082fa8cb5c7db77, 0x068686f8c734c155, 0x29e6c8d9f6e7a57e, 0x0473d308a7639bcf },
        { 0x812aa0416270220d, 0x995a89faf9245b4e, 0xffadc4ce5072ef05, 0x23bc2103aa73eb73 },
        { 0xcaee792603589e05, 0x2b4b421246dcc492, 0x02a1ef74e601a94f, 0x102f73bfde04341a }
    },
},
{
    {
        { 0x358ecba293a36247, 0xaf8f9862b268fd65, 0x412f7e9968a01c89, 0x5786f312cd754524 },
        { 0xb5a2d50c7ec20d3e, 0xc64bdd6ea0c97263, 0x56e89052c1ff734d, 0x4929c6f72b2ffaba },
        { 0x337788ffca14032c, 0xf3921028447f1ee3, 0x8b14071f231bccad, 0x4c817b4bf2344783 }
    },
    {
        { 0x413ba057a40b4484, 0xba4c2e1a4f5f6a43, 0x614ba0a5aee1d61c, 0x78a1531a8b05dc53 },
        { 0x0ff853852871b96e, 0xe13e9fab60c3f1bb, 0xeefd595325344402, 0x0a37c37075b7744b },
        { 0x6cbdf1703ad0562b, 0x8ecf4830c92521a3, 0xdaebd303fd8424e7, 0x72ad82a42e5ec56f }
    },
    {
        { 0xc368939167024bc3, 0x8e69d16d49502fda, 0xfcf2ec3ce45f4b29, 0x065f669ea3b4cbc4 },
        { 0x3f9e8e35bafb65f6, 0x39d69ec8f27293a1, 0x6cb8cd958cf6a3d0, 0x1734778173adae6d },
        { 0x8a00aec75532db4d, 0xb869a4e443e31bb1, 0x4a0f8552d3a7f515, 0x19adeb7c303d7c08 }
    },
    {
        { 0x9d05ba7d43c31794, 0x2470c8ff93322526, 0x8323dec816197438, 0x2852709881569b53 },
        { 0xc720cb6153ead9a3, 0x55b2c97f512b636e, 0xb1e35b5fd40290b1, 0x2fd9ccf13b530ee2 },
        { 0x07bd475b47f796b8, 0xd2c7b013542c8f54, 0x2dbd23f43b24f87e, 0x6551afd77b0901d6 }
    },
    {
        { 0x68a24ce3a1d5c9ac, 0xbb77a33d10ff6461, 0x0f86ce4425d3166e, 0x56507c0950b9623b },
        { 0x4546baaf54aac27f, 0xf6f66fecb2a45a28, 0x582d1b5b562bcfe8, 0x44b123f3920f785f },
        { 0x1206f0b7d1713e63, 0x353fe3d915bafc74, 0x194ceb970ad9d94d, 0x62fadd7cf9d03ad3 }
    },
    {
        { 0x3cd7bc61e7ce4594, 0xcd6b35a9b7dd267e, 0xa080abc84366ef27, 0x6ec7c46f59c79711 },
        { 0xc6b5967b5598a074, 0x5efe91ce8e493e25, 0xd4b72c4549280888, 0x20ef1149a26740c2 },
        { 0x2f07ad636f09a8a2, 0x8697e6ce24205e7d, 0xc0aefc05ee35a139, 0x15e80958b5f9d897 }
    },
    {
        { 0x4dd1ed355bb061c4, 0x42dc0cef941c0700, 0x61305dc1fd86340e, 0x56b2cc930e55a443 },
        { 0x25a5ef7d0c3e235b, 0x6c39c17fbe134ee7, 0xc774e1342dc5c327, 0x021354b892021f39 },
        { 0x1df79da6a6bfc5a2, 0x02f3a2749fde4369, 0xb323d9f2cda390a7, 0x7be0847b8774d363 }
    },
    {
        { 0x1466f5af5307fa11, 0x817fcc7ded6c0af2, 0x0a6de44ec3a4a3fb, 0x74071475bc927d0b },
        { 0x8c99cc5a8b3f55c3, 0x0611d7253fded2a0, 0xed2995ff36b70a36, 0x1f699a54d78a2619 },
        { 0xe77292f373e7ea8a, 0x296537d2cb045a31, 0x1bd0653ed3274fde, 0x2f9a2c4476bd2966 }
    },
},
{
    {
        { 0xa2b4dae0b5511c9a, 0x7ac860292bffff06, 0x981f375df5504234, 0x3f6bd725da4ea12d },
        { 0xeb18b9ab7f5745c6, 0x023a8aee5787c690, 0xb72712da2df7afa9, 0x36597d25ea5c013d },
        { 0x734d8d7b106058ac, 0xd940579e6fc6905f, 0x6466f8f99202932d, 0x7b7ecc19da60d6d0 }
    },
    {
        { 0x6dae4a51a77cfa9b, 0x82263654e7a38650, 0x09bbffcd8f2d82db, 0x03bedc661bf5caba },
        { 0x78c2373c695c690d, 0xdd252e660642906e, 0x951d44444ae12bd2, 0x4235ad7601743956 },
        { 0x6258cb0d078975f5, 0x492942549189f298, 0xa0cab423e2e36ee4, 0x0e7ce2b0cdf066a1 }
    },
    {
        { 0xfea6fedfd94b70f9, 0xf130c051c1fcba2d, 0x4882d47e7f2fab89, 0x615256138aeceeb5 },
        { 0xc494643ac48c85a3, 0xfd361df43c6139ad, 0x09db17dd3ae94d48, 0x666e0a5d8fb4674a },
        { 0x2abbf64e4870cb0d, 0xcd65bcf0aa458b6b, 0x9abe4eba75e8985d, 0x7f0bc810d514dee4 }
    },
    {
        { 0x83ac9dad737213a0, 0x9ff6f8ba2ef72e98, 0x311e2edd43ec6957, 0x1d3a907ddec5ab75 },
        { 0xb9006ba426f4136f, 0x8d67369e57e03035, 0xcbc8dfd94f463c28, 0x0d1f8dbcf8eedbf5 },
        { 0xba1693313ed081dc, 0x29329fad851b3480, 0x0128013c030321cb, 0x00011b44a31bfde3 }
    },
    {
        { 0x16561f696a0aa75c, 0xc1bf725c5852bd6a, 0x11a8dd7f9a7966ad, 0x63d988a2d2851026 },
        { 0x3fdfa06c3fc66c0c, 0x5d40e38e4dd60dd2, 0x7ae38b38268e4d71, 0x3ac48d916e8357e1 },
        { 0x00120753afbd232e, 0xe92bceb8fdd8f683, 0xf81669b384e72b91, 0x33fad52b2368a066 }
    },
    {
        { 0x8d2cc8d0c422cfe8, 0x072b4f7b05a13acb, 0xa3feb6e6ecf6a56f, 0x3cc355ccb90a71e2 },
        { 0x540649c6c5e41e16, 0x0af86430333f7735, 0xb2acfcd2f305e746, 0x16c0f429a256dca7 },
        { 0xe9b69443903e9131, 0xb8a494cb7a5637ce, 0xc87cd1a4baba9244, 0x631eaf426bae7568 }
    },
    {
        { 0x47d975b9a3700de8, 0x7280c5fbe2f80552, 0x53658f2732e45de1, 0x431f2c7f665f80b5 },
        { 0xb3e90410da66fe9f, 0x85dd4b526c16e5a6, 0xbc3d97611ef9bf83, 0x5599648b1ea919b5 },
        { 0xd6026344858f7b19, 0x14ab352fa1ea514a, 0x8900441a2090a9d7, 0x7b04715f91253b26 }
    },
    {
        { 0xb376c280c4e6bac6, 0x970ed3dd6d1d9b0b, 0xb09a9558450bf944, 0x48d0acfa57cde223 },
        { 0x83edbd28acf6ae43, 0x86357c8b7d5c7ab4, 0xc0404769b7eb2c44, 0x59b37bf5c2f6583f },
        { 0xb60f26e47dabe671, 0xf1d1a197622f3a37, 0x4208ce7ee9960394, 0x16234191336d3bdb }
    },
},
{
    {
        { 0x852dd1fd3d578bbe, 0x2b65ce72c3286108, 0x658c07f4eace2273, 0x0933f804ec38ab40 },
        { 0xf19aeac733a63aef, 0x2c7fba5d4442454e, 0x5da87aa04795e441, 0x413051e1a4e0b0f5 },
        { 0xa7ab69798d496476, 0x8121aadefcb5abc8, 0xa5dc12ef7b539472, 0x07fd47065e45351a }
    },
    {
        { 0x304211559ae8e7c3, 0xf281b229944882a5, 0x8a13ac2e378250e4, 0x014afa0954ba48f4 },
        { 0xc8583c3d258d2bcd, 0x17029a4daf60b73f, 0xfa0fc9d6416a3781, 0x1c1e5fba38b3fb23 },
        { 0xcb3197001bb3666c, 0x330060524bffecb9, 0x293711991a88233c, 0x291884363d4ed364 }
    },
    {
        { 0xfb9d37c3bc1ab6eb, 0x02be14534d57a240, 0xf4d73415f8a5e1f6, 0x5964f4300ccc8188 },
        { 0x033c6805dc4babfa, 0x2c15bf5e5596ecc1, 0x1bc70624b59b1d3b, 0x3ede9850a19f0ec5 },
        { 0xe44a23152d096800, 0x5c08c55970866996, 0xdf2db60a46affb6e, 0x579155c1f856fd89 }
    },
    {
        { 0xb5f16b630817e7a6, 0x808c69233c351026, 0x324a983b54cef201, 0x53c092084a485345 },
        { 0x96324edd12e0c9ef, 0x468b878df2420297, 0x199a3776a4f573be, 0x1e7fbcf18e91e92a },
        { 0xd2d41481f1cbafbf, 0x231d2db6716174e5, 0x0b7d7656e2a55c98, 0x3e955cd82aa495f6 }
    },
    {
        { 0xab39f3ef61bb3a3f, 0x8eb400652eb9193e, 0xb5de6ecc38c11f74, 0x654d7e9626f3c49f },
        { 0xe48f535e3ed15433, 0xd075692a0d7270a3, 0x40fbd21daade6387, 0x14264887cf4495f5 },
        { 0xe564cfdd5c7d2ceb, 0x82eeafded737ccb9, 0x6107db62d1f9b0ab, 0x0b6baac3b4358dbb }
    },
    {
        { 0x204abad63700a93b, 0xbe0023d3da779373, 0xd85f0346633ab709, 0x00496dc490820412 },
        { 0x7ae62bcb8622fe98, 0x47762256ceb891af, 0x1a5a92bcf2e406b4, 0x7d29401784e41501 },
        { 0x1c74b88dc27e6360, 0x074854268d14850c, 0xa145fb7b3e0dcb30, 0x10843f1b43803b23 }
    },
    {
        { 0xd56f672de324689b, 0xd1da8aedb394a981, 0xdd7b58fe9168cfed, 0x7ce246cd4d56c1e8 },
        { 0xc5f90455376276dd, 0xce59158dd7645cd9, 0x92f65d511d366b39, 0x11574b6e526996c4 },
        { 0xb8f4308e7f80be53, 0x5f3cb8cb34a9d397, 0x18a961bd33cc2b2c, 0x710045fb3a9af671 }
    },
    {
        { 0xa03fc862059d699e, 0x2370cfa19a619e69, 0xc4fe3b122f823deb, 0x1d1b056fa7f0844e },
        { 0x73f93d36101b95eb, 0xfaef33794f6f4486, 0x5651735f8f15e562, 0x7fa3f19058b40da1 },
        { 0x1bc64631e56bf61f, 0xd379ab106e5382a3, 0x4d58c57e0540168d, 0x566256628442d8e4 }
    },
},
{
    {
        { 0xdd499cd61ff38640, 0x29cd9bc3063625a0, 0x51e2d8023dd73dc3, 0x4a25707a203b9231 },
        { 0xb9e499def6267ff6, 0x7772ca7b742c0843, 0x23a0153fe9a4f2b1, 0x2cdfdfecd5d05006 },
        { 0x2ab7668a53f6ed6a, 0x304242581dd170a1, 0x4000144c3ae20161, 0x5721896d248e49fc }
    },
    {
        { 0x285d5091a1d0da4e, 0x4baa6fa7b5fe3e08, 0x63e5177ce19393b3, 0x03c935afc4b030fd },
        { 0x0b6e5517fd181bae, 0x9022629f2bb963b4, 0x5509bce932064625, 0x578edd74f63c13da },
        { 0x997276c6492b0c3d, 0x47ccc2c4dfe205fc, 0xdcd29b84dd623a3c, 0x3ec2ab590288c7a2 }
    },
    {
        { 0xa7213a09ae32d1cb, 0x0f2b87df40f5c2d5, 0x0baea4c6e81eab29, 0x0e1bf66c6adbac5e },
        { 0xa1a0d27be4d87bb9, 0xa98b4deb61391aed, 0x99a0ddd073cb9b83, 0x2dd5c25a200fcace },
        { 0xe2abd5e9792c887e, 0x1a020018cb926d5d, 0xbfba69cdbaae5f1e, 0x730548b35ae88f5f }
    },
    {
        { 0x805b094ba1d6e334, 0xbf3ef17709353f19, 0x423f06cb0622702b, 0x585a2277d87845dd },
        { 0xc43551a3cba8b8ee, 0x65a26f1db2115f16, 0x760f4f52ab8c3850, 0x3043443b411db8ca },
        { 0xa18a5f8233d48962, 0x6698c4b5ec78257f, 0xa78e6fa5373e41ff, 0x7656278950ef981f }
    },
    {
        { 0xe17073a3ea86cf9d, 0x3a8cfbb707155fdc, 0x4853e7fc31838a8e, 0x28bbf484b613f616 },
        { 0x38c3cf59d51fc8c0, 0x9bedd2fd0506b6f2, 0x26bf109fab570e8f, 0x3f4160a8c1b846a6 },
        { 0xf2612f5c6f136c7c, 0xafead107f6dd11be, 0x527e9ad213de6f33, 0x1e79cb358188f75d }
    },
    {
        { 0x77e953d8f5e08181, 0x84a50c44299dded9, 0xdc6c2d0c864525e5, 0x478ab52d39d1f2f4 },
        { 0x013436c3eef7e3f1, 0x828b6a7ffe9e10f8, 0x7ff908e5bcf9defc, 0x65d7951b3a3b3831 },
        { 0x66a6a4d39252d159, 0xe5dde1bc871ac807, 0xb82c6b40a6c1c96f, 0x16d87a411a212214 }
    },
    {
        { 0xfba4d5e2d54e0583, 0xe21fafd72ebd99fa, 0x497ac2736ee9778f, 0x1f990b577a5a6dde },
        { 0xb3bd7e5a42066215, 0x879be3cd0c5a24c1, 0x57c05db1d6f994b7, 0x28f87c8165f38ca6 },
        { 0xa3344ead1be8f7d6, 0x7d1e50ebacea798f, 0x77c6569e520de052, 0x45882fe1534d6d3e }
    },
    {
        { 0xd8ac9929943c6fe4, 0xb5f9f161a38392a2, 0x2699db13bec89af3, 0x7dcf843ce405f074 },
        { 0x6669345d757983d6, 0x62b6ed1117aa11a6, 0x7ddd1857985e128f, 0x688fe5b8f626f6dd },
        { 0x6c90d6484a4732c0, 0xd52143fdca563299, 0xb3be28c3915dc6e1, 0x6739687e7327191b }
    },
},
{
    {
        { 0x8ce5aad0c9cb971f, 0x1156aaa99fd54a29, 0x41f7247015af9b78, 0x1fe8cca8420f49aa },
        { 0x9f65c5ea200814cf, 0x840536e169a31740, 0x8b0ed13925c8b4ad, 0x0080dbafe936361d },
        { 0x72a1848f3c0cc82a, 0x38c560c2877c9e54, 0x5004e228ce554140, 0x042418a103429d71 }
    },
    {
        { 0x58e84c6f20816247, 0x8db2b2b6e36fd793, 0x977182561d484d85, 0x0822024f8632abd7 },
        { 0x899dea51abf3ff5f, 0x9b93a8672fc2d8ba, 0x2c38cb97be6ebd5c, 0x114d578497263b5d },
        { 0xb301bb7c6b1beca3, 0x55393f6dc6eb1375, 0x910d281097b6e4eb, 0x1ad4548d9d479ea3 }
    },
    {
        { 0xa06fe66d0fe9fed3, 0xa8733a401c587909, 0x30d14d800df98953, 0x41ce5876c7b30258 },
        { 0xcd5a7da0389a48fd, 0xb38fa4aa9a78371e, 0xc6d9761b2cdb8e6c, 0x35cf51dbc97e1443 },
        { 0x59ac3bc5d670c022, 0xeae67c109b119406, 0x9798bdf0b3782fda, 0x651e3201fd074092 }
    },
    {
        { 0xa57ba4a01efcae9e, 0x769f4beedc308a94, 0xd1f10eeb3603cb2e, 0x4099ce5e7e441278 },
        { 0xd63d8483ef30c5cf, 0x4cd4b4962361cc0c, 0xee90e500a48426ac, 0x0af51d7d18c14eeb },
        { 0x1ac98e4f8a5121e9, 0x7dae9544dbfa2fe0, 0x8320aa0dd6430df9, 0x667282652c4a2fb5 }
    },
    {
        { 0xada8b6e02946db23, 0x1c0ce51a7b253ab7, 0x8448c85a66dd485b, 0x7f1fc025d0675adf },
        { 0x874621f4d86bc9ab, 0xb54c7bbe56fe6fea, 0x077a24257fadc22c, 0x1ab53be419b90d39 },
        { 0xd8ee1b18319ea6aa, 0x004d88083a21f0da, 0x3bd6aa1d883a4f4b, 0x4db9a3a6dfd9fd14 }
    },
    {
        { 0xd95b00bbcbb77c68, 0xddbc846a91f17849, 0x7cf700aebe28d9b3, 0x5ce1285c85d31f3e },
        { 0x8ce7b23bb99c0755, 0x35c5d6edc4f50f7a, 0x7e1e2ed2ed9b50c3, 0x36305f16e8934da1 },
        { 0x31b6972d98b0bde8, 0x7d920706aca6de5b, 0xe67310f8908a659f, 0x50fac2a6efdf0235 }
    },
    {
        { 0x295b1c86f6f449bc, 0x51b2e84a1f0ab4dd, 0xc001cb30aa8e551d, 0x6a28d35944f43662 },
        { 0xf3d3a9f35b880f5a, 0xedec050cdb03e7c2, 0xa896981ff9f0b1a2, 0x49a4ae2bac5e34a4 },
        { 0x28bb12ee04a740e0, 0x14313bbd9bce8174, 0x72f5b5e4e8c10c40, 0x7cbfb19936adcd5b }
    },
    {
        { 0x8e793a7acc36e6e0, 0xf9fab7a37d586eed, 0x3a4f9692bae1f4e4, 0x1c14b03eff5f447e },
        { 0xa311ddc26b89792d, 0x1b30b4c6da512664, 0x0ca77b4ccf150859, 0x1de443df1b009408 },
        { 0x19647bd114a85291, 0x57b76cb21034d3af, 0x6329db440f9d6dfa, 0x5ef43e586a571493 }
    },
},
{
    {
        { 0xa66dcc9dc80c1ac0, 0x97a05cf41b38a436, 0xa7ebf3be95dbd7c6, 0x7da0b8f68d7e7dab },
        { 0xef782014385675a6, 0xa2649f30aafda9e8, 0x4cd1eb505cdfa8cb, 0x46115aba1d4dc0b3 },
        { 0xd40f1953c3b5da76, 0x1dac6f7321119e9b, 0x03cc6021feb25960, 0x5a5f887e83674b4b }
    },
    {
        { 0x9e9628d3a0a643b9, 0xb5c3cb00e6c32064, 0x9b5302897c2dec32, 0x43e37ae2d5d1c70c },
        { 0x8f6301cf70a13d11, 0xcfceb815350dd0c4, 0xf70297d4a4bca47e, 0x3669b656e44d1434 },
        { 0x387e3f06eda6e133, 0x67301d5199a13ac0, 0xbd5ad8f836263811, 0x6a21e6cd4fd5e9be }
    },
    {
        { 0xef4129126699b2e3, 0x71d30847708d1301, 0x325432d01182b0bd, 0x45371b07001e8b36 },
        { 0xf1c6170a3046e65f, 0x58712a2a00d23524, 0x69dbbd3c8c82b755, 0x586bf9f1a195ff57 },
        { 0xa6db088d5ef8790b, 0x5278f0dc610937e5, 0xac0349d261a16eb8, 0x0eafb03790e52179 }
    },
    {
        { 0x5140805e0f75ae1d, 0xec02fbe32662cc30, 0x2cebdf1eea92396d, 0x44ae3344c5435bb3 },
        { 0x960555c13748042f, 0x219a41e6820baa11, 0x1c81f73873486d0c, 0x309acc675a02c661 },
        { 0x9cf289b9bba543ee, 0xf3760e9d5ac97142, 0x1d82e5c64f9360aa, 0x62d5221b7f94678f }
    },
    {
        { 0x7585d4263af77a3c, 0xdfae7b11fee9144d, 0xa506708059f7193d, 0x14f29a5383922037 },
        { 0x524c299c18d0936d, 0xc86bb56c8a0c1a0c, 0xa375052edb4a8631, 0x5c0efde4bc754562 },
        { 0xdf717edc25b2d7f5, 0x21f970db99b53040, 0xda9234b7c3ed4c62, 0x5e72365c7bee093e }
    },
    {
        { 0x7d9339062f08b33e, 0x5b9659e5df9f32be, 0xacff3dad1f9ebdfd, 0x70b20555cb7349b7 },
        { 0x575bfc074571217f, 0x3779675d0694d95b, 0x9a0a37bbf4191e33, 0x77f1104c47b4eabc },
        { 0xbe5113c555112c4c, 0x6688423a9a881fcd, 0x446677855e503b47, 0x0e34398f4a06404a }
    },
    {
        { 0x18930b093e4b1928, 0x7de3e10e73f3f640, 0xf43217da73395d6f, 0x6f8aded6ca379c3e },
        { 0xb67d22d93ecebde8, 0x09b3e84127822f07, 0x743fa61fb05b6d8d, 0x5e5405368a362372 },
        { 0xe340123dfdb7b29a, 0x487b97e1a21ab291, 0xf9967d02fde6949e, 0x780de72ec8d3de97 }
    },
    {
        { 0x671feaf300f42772, 0x8f72eb2a2a8c41aa, 0x29a17fd797373292, 0x1defc6ad32b587a6 },
        { 0x0ae28545089ae7bc, 0x388ddecf1c7f4d06, 0x38ac15510a4811b8, 0x0eb28bf671928ce4 },
        { 0xaf5bbe1aef5195a7, 0x148c1277917b15ed, 0x2991f7fb7ae5da2e, 0x467d201bf8dd2867 }
    },
},
{
    {
        { 0x745f9d56296bc318, 0x993580d4d8152e65, 0xb0e5b13f5839e9ce, 0x51fc2b28d43921c0 },
        { 0x7906ee72f7bd2e6b, 0x05d270d6109abf4e, 0x8d5cfe45b941a8a4, 0x44c218671c974287 },
        { 0x1b8fd11795e2a98c, 0x1c4e5ee12b6b6291, 0x5b30e7107424b572, 0x6e6b9de84c4f4ac6 }
    },
    {
        { 0x6b7c5f10f80cb088, 0x736b54dc56e42151, 0xc2b620a5c6ef99c4, 0x5f4c802cc3a06f42 },
        { 0xdff25fce4b1de151, 0xd841c0c7e11c4025, 0x2554b3c854749c87, 0x2d292459908e0df9 },
        { 0x9b65c8f17d0752da, 0x881ce338c77ee800, 0xc3b514f05b62f9e3, 0x66ed5dd5bec10d48 }
    },
    {
        { 0xf0adf3c9cbca047d, 0x81c3b2cbf4552f6b, 0xcfda112d44735f93, 0x1f23a0c77e20048c },
        { 0x7d38a1c20bb2089d, 0x808334e196ccd412, 0xc4a70b8c6c97d313, 0x2eacf8bc03007f20 },
        { 0xf235467be5bc1570, 0x03d2d9020dbab38c, 0x27529aa2fcf9e09e, 0x0840bef29d34bc50 }
    },
    {
        { 0xcd54e06b7f37e4eb, 0x8cc15f87f5e96cca, 0xb8248bb0d3597dce, 0x246affa06074400c },
        { 0x796dfb35dc10b287, 0x27176bcd5c7ff29d, 0x7f3d43e8c7b24905, 0x0304f5a191c54276 },
        { 0x37d88e68fbe45321, 0x86097548c0d75032, 0x4e9b13ef894a0d35, 0x25a83cac5753d325 }
    },
    {
        { 0x9f0f66293952b6e2, 0x33db5e0e0934267b, 0xff45252bd609fedc, 0x06be10f5c506e0c9 },
        { 0x10222f48eed8165e, 0x623fc1234b8bcf3a, 0x1e145c09c221e8f0, 0x7ccfa59fca782630 },
        { 0x1a9615a9b62a345f, 0x22050c564a52fecc, 0xa7a2788528bc0dfe, 0x5e82770a1a1ee71d }
    },
    {
        { 0xe802e80a42339c74, 0x34175166a7fffae5, 0x34865d1f1c408cae, 0x2cca982c605bc5ee },
        { 0x35425183ad896a5c, 0xe8673afbe78d52f6, 0x2c66f25f92a35f64, 0x09d04f3b3b86b102 },
        { 0xfd2d5d35197dbe6e, 0x207c2eea8be4ffa3, 0x2613d8db325ae918, 0x7a325d1727741d3e }
    },
    {
        { 0xecd27d017e2a076a, 0xd788689f1636495e, 0x52a61af0919233e5, 0x2a479df17bb1ae64 },
        { 0xd036b9bbd16dfde2, 0xa2055757c497a829, 0x8e6cc966a7f12667, 0x4d3b1a791239c180 },
        { 0x9e5eee8e33db2710, 0x189854ded6c43ca5, 0xa41c22c592718138, 0x27ad5538a43a5e9b }
    },
    {
        { 0xcb5a7d638e47077c, 0x8db7536120a1c059, 0x549e1e4d8bedfdcc, 0x080153b7503b179d },
        { 0x2746dd4b15350d61, 0xd03fcbc8ee9521b7, 0xe86e365a138672ca, 0x510e987f7e7d89e2 },
        { 0xdda69d930a3ed3e3, 0x3d386ef1cd60a722, 0xc817ad58bdaa4ee6, 0x23be8d554fe7372a }
    },
},
{
    {
        { 0xbc1ef4bd567ae7a9, 0x3f624cb2d64498bd, 0xe41064d22c1f4ec8, 0x2ef9c5a5ba384001 },
        { 0x95fe919a74ef4fad, 0x3a827becf6a308a2, 0x964e01d309a47b01, 0x71c43c4f5ba3c797 },
        { 0xb6fd6df6fa9e74cd, 0xf18278bce4af267a, 0x8255b3d0f1ef990e, 0x5a758ca390c5f293 }
    },
    {
        { 0x8ce0918b1d61dc94, 0x8ded36469a813066, 0xd4e6a829afe8aad3, 0x0a738027f639d43f },
        { 0xa2b72710d9462495, 0x3aa8c6d2d57d5003, 0xe3d400bfa0b487ca, 0x2dbae244b3eb72ec },
        { 0x980f4a2f57ffe1cc, 0x00670d0de1839843, 0x105c3f4a49fb15fd, 0x2698ca635126a69c }
    },
    {
        { 0x2e3d702f5e3dd90e, 0x9e3f0918e4d25386, 0x5e773ef6024da96a, 0x3c004b0c4afa3332 },
        { 0xe765318832b0ba78, 0x381831f7925cff8b, 0x08a81b91a0291fcc, 0x1fb43dcc49caeb07 },
        { 0x9aa946ac06f4b82b, 0x1ca284a5a806c4f3, 0x3ed3265fc6cd4787, 0x6b43fd01cd1fd217 }
    },
    {
        { 0xb5c742583e760ef3, 0x75dc52b9ee0ab990, 0xbf1427c2072b923f, 0x73420b2d6ff0d9f0 },
        { 0xc7a75d4b4697c544, 0x15fdf848df0fffbf, 0x2868b9ebaa46785a, 0x5a68d7105b52f714 },
        { 0xaf2cf6cb9e851e06, 0x8f593913c62238c4, 0xda8ab89699fbf373, 0x3db5632fea34bc9e }
    },
    {
        { 0x2e4990b1829825d5, 0xedeaeb873e9a8991, 0xeef03d394c704af8, 0x59197ea495df2b0e },
        { 0xf46eee2bf75dd9d8, 0x0d17b1f6396759a5, 0x1bf2d131499e7273, 0x04321adf49d75f13 },
        { 0x04e16019e4e55aae, 0xe77b437a7e2f92e9, 0xc7ce2dc16f159aa4, 0x45eafdc1f4d70cc0 }
    },
    {
        { 0xb60e4624cfccb1ed, 0x59dbc292bd5c0395, 0x31a09d1ddc0481c9, 0x3f73ceea5d56d940 },
        { 0x698401858045d72b, 0x4c22faa2cf2f0651, 0x941a36656b222dc6, 0x5a5eebc80362dade },
        { 0xb7a7bfd10a4e8dc6, 0xbe57007e44c9b339, 0x60c1207f1557aefa, 0x26058891266218db }
    },
    {
        { 0x4c818e3cc676e542, 0x5e422c9303ceccad, 0xec07cccab4129f08, 0x0dedfa10b24443b8 },
        { 0x59f704a68360ff04, 0xc3d93fde7661e6f4, 0x831b2a7312873551, 0x54ad0c2e4e615d57 },
        { 0xee3b67d5b82b522a, 0x36f163469fa5c1eb, 0xa5b4d2f26ec19fd3, 0x62ecb2baa77a9408 }
    },
    {
        { 0x92072836afb62874, 0x5fcd5e8579e104a5, 0x5aad01adc630a14a, 0x61913d5075663f98 },
        { 0xe5ed795261152b3d, 0x4962357d0eddd7d1, 0x7482c8d0b96b4c71, 0x2e59f919a966d8be },
        { 0x0dc62d361a3231da, 0xfa47583294200270, 0x02d801513f9594ce, 0x3ddbc2a131c05d5c }
    },
},
{
    {
        { 0xfb735ac2004a35d1, 0x31de0f433a6607c3, 0x7b8591bfc528d599, 0x55be9a25f5bb050c },
        { 0x3f50a50a4ffb81ef, 0xb1e035093bf420bf, 0x9baa8e1cc6aa2cd0, 0x32239861fa237a40 },
        { 0x0d005acd33db3dbf, 0x0111b37c80ac35e2, 0x4892d66c6f88ebeb, 0x770eadb16508fbcd }
    },
    {
        { 0xf1d3b681a05071b9, 0x2207659a3592ff3a, 0x5f0169297881e40e, 0x16bedd0e86ba374e },
        { 0x8451f9e05e4e89dd, 0xc06302ffbc793937, 0x5d22749556a6495c, 0x09a6755ca05603fb },
        { 0x5ecccc4f2c2737b5, 0x43b79e0c2dccb703, 0x33e008bc4ec43df3, 0x06c1b840f07566c0 }
    },
    {
        { 0x69ee9e7f9b02805c, 0xcbff828a547d1640, 0x3d93a869b2430968, 0x46b7b8cd3fe26972 },
        { 0x7688a5c6a388f877, 0x02a96c14deb2b6ac, 0x64c9f3431b8c2af8, 0x3628435554a1eed6 },
        { 0xe9812086fe7eebe0, 0x4cba6be72f515437, 0x1d04168b516efae9, 0x5ea1391043982cb9 }
    },
    {
        { 0x6f2b3be4d5d3b002, 0xafec33d96a09c880, 0x035f73a4a8bcc4cc, 0x22c5b9284662198b },
        { 0x49125c9cf4702ee1, 0x4520b71f8b25b32d, 0x33193026501fef7e, 0x656d8997c8d2eb2b },
        { 0xcb58c8fe433d8939, 0x89a0cb2e6a8d7e50, 0x79ca955309fbbe5a, 0x0c626616cd7fc106 }
    },
    {
        { 0x8fdfc379fbf454b1, 0x45a5a970f1a4b771, 0xac921ef7bad35915, 0x42d088dca81c2192 },
        { 0x1ffeb80a4879b61f, 0x6396726e4ada21ed, 0x33c7b093368025ba, 0x471aa0c6f3c31788 },
        { 0x8fda0f37a0165199, 0x0adadb77c8a0e343, 0x20fbfdfcc875e820, 0x1cf2bea80c2206e7 }
    },
    {
        { 0x982d6e1a02c0412f, 0x90fa4c83db58e8fe, 0x01c2f5bcdcb18bc0, 0x686e0c90216abc66 },
        { 0xc2ddf1deb36202ac, 0x92a5fe09d2e27aa5, 0x7d1648f6fc09f1d3, 0x74c2cc0513bc4959 },
        { 0x1fadbadba54395a7, 0xb41a02a0ae0da66a, 0xbf19f598bba37c07, 0x6a12b8acde48430d }
    },
    {
        { 0x793bdd801aaeeb5f, 0x00a2a0aac1518871, 0xe8a373a31f2136b4, 0x48aab888fc91ef19 },
        { 0xf8daea1f39d495d9, 0x592c190e525f1dfc, 0xdb8cbd04c9991d1b, 0x11f7fda3d88f0cb7 },
        { 0x041f7e925830f40e, 0x002d6ca979661c06, 0x86dc9ff92b046a2e, 0x760360928b0493d1 }
    },
    {
        { 0xb43108e5695a0b05, 0x6cb00ee8ad37a38b, 0x5edad6eea3537381, 0x3f2602d4b6dc3224 },
        { 0x21bb41c6120cf9c6, 0xeab2aa12decda59b, 0xc1a72d020aa48b34, 0x215d4d27e87d3b68 },
        { 0xc8b247b65bcaf19c, 0x49779dc3b1b2c652, 0x89a180bbd5ece2e2, 0x13f098a3cec8e039 }
    },
},
{
    {
        { 0xf3aa57a22796bb14, 0x883abab79b07da21, 0xe54be21831a0391c, 0x5ee7fb38d83205f9 },
        { 0x9adc0ff9ce5ec54b, 0x039c2a6b8c2f130d, 0x028007c7f0f89515, 0x78968314ac04b36b },
        { 0x538dfdcb41446a8e, 0xa5acfda9434937f9, 0x46af908d263c8c78, 0x61d0633c9bca0d09 }
    },
    {
        { 0xada328bcf8fc73df, 0xee84695da6f037fc, 0x637fb4db38c2a909, 0x5b23ac2df8067bdc },
        { 0x63744935ffdb2566, 0xc5bd6b89780b68bb, 0x6f1b3280553eec03, 0x6e965fd847aed7f5 },
        { 0x9ad2b953ee80527b, 0xe88f19aafade6d8d, 0x0e711704150e82cf, 0x79b9bbb9dd95dedc }
    },
    {
        { 0xd1997dae8e9f7374, 0xa032a2f8cfbb0816, 0xcd6cba126d445f0a, 0x1ba811460accb834 },
        { 0xebb355406a3126c2, 0xd26383a868c8c393, 0x6c0c6429e5b97a82, 0x5065f158c9fd2147 },
        { 0x708169fb0c429954, 0xe14600acd76ecf67, 0x2eaab98a70e645ba, 0x3981f39e58a4faf2 }
    },
    {
        { 0xc845dfa56de66fde, 0xe152a5002c40483a, 0xe9d2e163c7b4f632, 0x30f4452edcbc1b65 },
        { 0x18fb8a7559230a93, 0x1d168f6960e6f45d, 0x3a85a94514a93cb5, 0x38dc083705acd0fd },
        { 0x856d2782c5759740, 0xfa134569f99cbecc, 0x8844fc73c0ea4e71, 0x632d9a1a593f2469 }
    },
    {
        { 0xbf09fd11ed0c84a7, 0x63f071810d9f693a, 0x21908c2d57cf8779, 0x3a5a7df28af64ba2 },
        { 0xf6bb6b15b807cba6, 0x1823c7dfbc54f0d7, 0xbb1d97036e29670b, 0x0b24f48847ed4a57 },
        { 0xdcdad4be511beac7, 0xa4538075ed26ccf2, 0xe19cff9f005f9a65, 0x34fcf74475481f63 }
    },
    {
        { 0xa5bb1dab78cfaa98, 0x5ceda267190b72f2, 0x9309c9110a92608e, 0x0119a3042fb374b0 },
        { 0xc197e04c789767ca, 0xb8714dcb38d9467d, 0x55de888283f95fa8, 0x3d3bdc164dfa63f7 },
        { 0x67a2d89ce8c2177d, 0x669da5f66895d0c1, 0xf56598e5b282a2b0, 0x56c088f1ede20a73 }
    },
    {
        { 0x581b5fac24f38f02, 0xa90be9febae30cbd, 0x9a2169028acf92f0, 0x038b7ea48359038f },
        { 0x336d3d1110a86e17, 0xd7f388320b75b2fa, 0xf915337625072988, 0x09674c6b99108b87 },
        { 0x9f4ef82199316ff8, 0x2f49d282eaa78d4f, 0x0971a5ab5aef3174, 0x6e5e31025969eb65 }
    },
    {
        { 0x3304fb0e63066222, 0xfb35068987acba3f, 0xbd1924778c1061a3, 0x3058ad43d1838620 },
        { 0xb16c62f587e593fb, 0x4999eddeca5d3e71, 0xb491c1e014cc3e6d, 0x08f5114789a8dba8 },
        { 0x323c0ffde57663d0, 0x05c3df38a22ea610, 0xbdc78abdac994f9a, 0x26549fa4efe3dc99 }
    },
},
{
    {
        { 0x04dbbc17f75396b9, 0x69e6a2d7d2f86746, 0xc6409d99f53eabc6, 0x606175f6332e25d2 },
        { 0x738b38d787ce8f89, 0xb62658e24179a88d, 0x30738c9cf151316d, 0x49128c7f727275c9 },
        { 0x4021370ef540e7dd, 0x0910d6f5a1f1d0a5, 0x4634aacd5b06b807, 0x6a39e6356944f235 }
    },
    {
        { 0x1da1965774049e9d, 0xfbcd6ea198fe352b, 0xb1cbcd50cc5236a6, 0x1f5ec83d3f9846e2 },
        { 0x96cd5640df90f3e7, 0x6c3a760edbfa25ea, 0x24f3ef0959e33cc4, 0x42889e7e530d2e58 },
        { 0x8efb23c3328ccb75, 0xaf42a207dd876ee9, 0x20fbdadc5dfae796, 0x241e246b06bf9f51 }
    },
    {
        { 0x7eaafc9a6280bbb8, 0x22a70f12f403d809, 0x31ce40bb1bfc8d20, 0x2bc65635e8bd53ee },
        { 0x29e68e57ad6e98f6, 0x4c9260c80b462065, 0x3f00862ea51ebb4b, 0x5bc2c77fb38d9097 },
        { 0xe8d5dc9fa96bad93, 0xe58fb17dde1947dc, 0x681532ea65185fa3, 0x1fdd6c3b034a7830 }
    },
    {
        { 0x9c13a6a52dd8f7a9, 0x2dbb1f8c3efdcabf, 0x961e32405e08f7b5, 0x48c8a121bbe6c9e5 },
        { 0x0a64e28c55dc18fe, 0xe3df9e993399ebdd, 0x79ac432370e2e652, 0x35ff7fc33ae4cc0e },
        { 0xfc415a7c59646445, 0xd224b2d7c128b615, 0x6035c9c905fbb912, 0x42d7a91274429fab }
    },
    {
        { 0xa9a48947933da5bc, 0x4a58920ec2e979ec, 0x96d8800013e5ac4c, 0x453692d74b48b147 },
        { 0x4e6213e3eaf72ed3, 0x6794981a43acd4e7, 0xff547cde6eb508cb, 0x6fed19dd10fcb532 },
        { 0xdd775d99a8559c6f, 0xf42a2140df003e24, 0x5223e229da928a66, 0x063f46ba6d38f22c }
    },
    {
        { 0x39843cb737346921, 0xa747fb0738c89447, 0xcb8d8031a245307e, 0x67810f8e6d82f068 },
        { 0xd2d242895f536694, 0xca33a2c542939b2c, 0x986fada6c7ddb95c, 0x5a152c042f712d5d },
        { 0x3eeb8fbcd2287db4, 0x72c7d3a301a03e93, 0x5473e88cbd98265a, 0x7324aa515921b403 }
    },
    {
        { 0xad23f6dae82354cb, 0x6962502ab6571a6d, 0x9b651636e38e37d1, 0x5cac5005d1a3312f },
        { 0x857942f46c3cbe8e, 0xa1d364b14730c046, 0x1c8ed914d23c41bf, 0x0838e161eef6d5d2 },
        { 0x8cc154cce9e39904, 0x5b3a040b84de6846, 0xc4d8a61cb1be5d6e, 0x40fb897bd8861f02 }
    },
    {
        { 0xe57ed8475ab10761, 0x71435e206fd13746, 0x342f824ecd025632, 0x4b16281ea8791e7b },
        { 0x84c5aa9062de37a1, 0x421da5000d1d96e1, 0x788286306a9242d9, 0x3c5e464a690d10da },
        { 0xd1c101d50b813381, 0xdee60f1176ee6828, 0x0cb68893383f6409, 0x6183c565f6ff484a }
    },
},
{
    {
        { 0xdb468549af3f666e, 0xd77fcf04f14a0ea5, 0x3df23ff7a4ba0c47, 0x3a10dfe132ce3c85 },
        { 0x741d5a461e6bf9d6, 0x2305b3fc7777a581, 0xd45574a26474d3d9, 0x1926e1dc6401e0ff },
        { 0xe07f4e8aea17cea0, 0x2fd515463a1fc1fd, 0x175322fd31f2c0f1, 0x1fa1d01d861e5d15 }
    },
    {
        { 0x38dcac00d1df94ab, 0x2e712bddd1080de9, 0x7f13e93efdd5e262, 0x73fced18ee9a01e5 },
        { 0xcc8055947d599832, 0x1e4656da37f15520, 0x99f6f7744e059320, 0x773563bc6a75cf33 },
        { 0x06b1e90863139cb3, 0xa493da67c5a03ecd, 0x8d77cec8ad638932, 0x1f426b701b864f44 }
    },
    {
        { 0xf17e35c891a12552, 0xb76b8153575e9c76, 0xfa83406f0d9b723e, 0x0b76bb1b3fa7e438 },
        { 0xefc9264c41911c01, 0xf1a3b7b817a22c25, 0x5875da6bf30f1447, 0x4e1af5271d31b090 },
        { 0x08b8c1f97f92939b, 0xbe6771cbd444ab6e, 0x22e5646399bb8017, 0x7b6dd61eb772a955 }
    },
    {
        { 0x5730abf9ab01d2c7, 0x16fb76dc40143b18, 0x866cbe65a0cbb281, 0x53fa9b659bff6afe },
        { 0xb7adc1e850f33d92, 0x7998fa4f608cd5cf, 0xad962dbd8dfc5bdb, 0x703e9bceaf1d2f4f },
        { 0x6c14c8e994885455, 0x843a5d6665aed4e5, 0x181bb73ebcd65af1, 0x398d93e5c4c61f50 }
    },
    {
        { 0xc3877c60d2e7e3f2, 0x3b34aaa030828bb1, 0x283e26e7739ef138, 0x699c9c9002c30577 },
        { 0x1c4bd16733e248f3, 0xbd9e128715bf0a5f, 0xd43f8cf0a10b0376, 0x53b09b5ddf191b13 },
        { 0xf306a7235946f1cc, 0x921718b5cce5d97d, 0x28cdd24781b4e975, 0x51caf30c6fcdd907 }
    },
    {
        { 0x737af99a18ac54c7, 0x903378dcc51cb30f, 0x2b89bc334ce10cc7, 0x12ae29c189f8e99a },
        { 0xa60ba7427674e00a, 0x630e8570a17a7bf3, 0x3758563dcf3324cc, 0x5504aa292383fdaa },
        { 0xa99ec0cb1f0d01cf, 0x0dd1efcc3a34f7ae, 0x55ca7521d09c4e22, 0x5fd14fe958eba5ea }
    },
    {
        { 0x3c42fe5ebf93cb8e, 0xbedfa85136d4565f, 0xe0f0859e884220e8, 0x7dd73f960725d128 },
        { 0xb5dc2ddf2845ab2c, 0x069491b10a7fe993, 0x4daaf3d64002e346, 0x093ff26e586474d1 },
        { 0xb10d24fe68059829, 0x75730672dbaf23e5, 0x1367253ab457ac29, 0x2f59bcbc86b470a4 }
    },
    {
        { 0x7041d560b691c301, 0x85201b3fadd7e71e, 0x16c2e16311335585, 0x2aa55e3d010828b1 },
        { 0x83847d429917135f, 0xad1b911f567d03d7, 0x7e7748d9be77aad1, 0x5458b42e2e51af4a },
        { 0xed5192e60c07444f, 0x42c54e2d74421d10, 0x352b4c82fdb5c864, 0x13e9004a8a768664 }
    },
},
{
    {
        { 0x1e6284c5806b467c, 0xc5f6997be75d607b, 0x8b67d958b378d262, 0x3d88d66a81cd8b70 },
        { 0xcbb5b5556c032bff, 0xdf7191b729297a3a, 0xc1ff7326aded81bb, 0x71ade8bb68be03f5 },
        { 0x8b767a93204ed789, 0x762fcacb9fa0ae2a, 0x771febcc6dce4887, 0x343062158ff05fb3 }
    },
    {
        { 0xfce219072a7b31b4, 0x4d7adc75aa578016, 0x0ec276a687479324, 0x6d6d9d5d1fda4beb },
        { 0xe05da1a7e1f5bf49, 0x26457d6dd4736092, 0x77dcb07773cc32f6, 0x0a5d94969cdd5fcd },
        { 0x22b1a58ae9b08183, 0xfd95d071c15c388b, 0xa9812376850a0517, 0x33384cbabb7f335e }
    },
    {
        { 0x33bc627a26218b8d, 0xea80b21fc7a80c61, 0x9458b12b173e9ee6, 0x076247be0e2f3059 },
        { 0x3c6fa2680ca2c7b5, 0x1b5082046fb64fda, 0xeb53349c5431d6de, 0x5278b38f6b879c89 },
        { 0x52e105f61416375a, 0xec97af3685abeba4, 0x26e6b50623a67c36, 0x5cf0e856f3d4fb01 }
    },
    {
        { 0xbeaece313db342a8, 0xcba3635b842db7ee, 0xe88c6620817f13ef, 0x1b9438aa4e76d5c6 },
        { 0xf6c968731ae8cab4, 0x5e20741ecb4f92c5, 0x2da53be58ccdbc3e, 0x2dddfea269970df7 },
        { 0x8a50777e166f031a, 0x067b39f10fb7a328, 0x1925c9a6010fbd76, 0x6df9b575cc740905 }
    },
    {
        { 0xecdfc35b48cade41, 0x6a88471fb2328270, 0x740a4a2440a01b6a, 0x471e5796003b5f29 },
        { 0x42c1192927f6bdcf, 0x8f91917a403d61ca, 0xdc1c5a668b9e1f61, 0x1596047804ec0f8d },
        { 0xda96bbb3aced37ac, 0x7a2423b5e9208cea, 0x24cc5c3038aebae2, 0x50c356afdc5dae2f }
    },
    {
        { 0xcfed9cdf1b31b964, 0xf486a9858ca51af3, 0x14897265ea8c1f84, 0x784a53dd932acc00 },
        { 0x09dcbf4341c30318, 0xeeba061183181dce, 0xc179c0cedc1e29a1, 0x1dbf7b89073f35b0 },
        { 0x2d99f9df14fc4920, 0x76ccb60cc4499fe5, 0xa4132cbbe5cf0003, 0x3f93d82354f000ea }
    },
    {
        { 0xeaac12d179e14978, 0xff923ff3bbebff5e, 0x4af663e40663ce27, 0x0fd381a811a5f5ff },
        { 0x8183e7689e04ce85, 0x678fb71e04465341, 0xad92058f6688edac, 0x5da350d3532b099a },
        { 0xf256aceca436df54, 0x108b6168ae69d6e8, 0x20d986cb6b5d036c, 0x655957b9fee2af50 }
    },
    {
        { 0xbdc1409bd002d0ac, 0x66660245b5ccd9a6, 0x82317dc4fade85ec, 0x02fe934b6ad7df0d },
        { 0xaea8b07fa902030f, 0xf88c766af463d143, 0x15b083663c787a60, 0x08eab1148267a4a8 },
        { 0xef5cf100cfb7ea74, 0x22897633a1cb42ac, 0xd4ce0c54cef285e2, 0x30408c048a146a55 }
    },
},
{
    {
        { 0xbb2e00c9193b877f, 0xece3a890e0dc506b, 0xecf3b7c036de649f, 0x5f46040898de9e1a },
        { 0x739d8845832fcedb, 0xfa38d6c9ae6bf863, 0x32bc0dcab74ffef7, 0x73937e8814bce45e },
        { 0xb9037116297bf48d, 0xa9d13b22d4f06834, 0xe19715574696bdc6, 0x2cf8a4e891d5e835 }
    },
    {
        { 0x2cb5487e17d06ba2, 0x24d2381c3950196b, 0xd7659c8185978a30, 0x7a6f7f2891d6a4f6 },
        { 0x6d93fd8707110f67, 0xdd4c09d37c38b549, 0x7cb16a4cc2736a86, 0x2049bd6e58252a09 },
        { 0x7d09fd8d6a9aef49, 0xf0ee60be5b3db90b, 0x4c21b52c519ebfd4, 0x6011aadfc545941d }
    },
    {
        { 0x63ded0c802cbf890, 0xfbd098ca0dff6aaa, 0x624d0afdb9b6ed99, 0x69ce18b779340b1e },
        { 0x5f67926dcf95f83c, 0x7c7e856171289071, 0xd6a1e7f3998f7a5b, 0x6fc5cc1b0b62f9e0 },
        { 0xd1ef5528b29879cb, 0xdd1aae3cd47e9092, 0x127e0442189f2352, 0x15596b3ae57101f1 }
    },
    {
        { 0x09ff31167e5124ca, 0x0be4158bd9c745df, 0x292b7d227ef556e5, 0x3aa4e241afb6d138 },
        { 0x462739d23f9179a2, 0xff83123197d6ddcf, 0x1307deb553f2148a, 0x0d2237687b5f4dda },
        { 0x2cc138bf2a3305f5, 0x48583f8fa2e926c3, 0x083ab1a25549d2eb, 0x32fcaa6e4687a36c }
    },
    {
        { 0x3207a4732787ccdf, 0x17e31908f213e3f8, 0xd5b2ecd7f60d964e, 0x746f6336c2600be9 },
        { 0x7bc56e8dc57d9af5, 0x3e0bd2ed9df0bdf2, 0xaac014de22efe4a3, 0x4627e9cefebd6a5c },
        { 0x3f4af345ab6c971c, 0xe288eb729943731f, 0x33596a8a0344186d, 0x7b4917007ed66293 }
    },
    {
        { 0x54341b28dd53a2dd, 0xaa17905bdf42fc3f, 0x0ff592d94dd2f8f4, 0x1d03620fe08cd37d },
        { 0x2d85fb5cab84b064, 0x497810d289f3bc14, 0x476adc447b15ce0c, 0x122ba376f844fd7b },
        { 0xc20232cda2b4e554, 0x9ed0fd42115d187f, 0x2eabb4be7dd479d9, 0x02c70bf52b68ec4c }
    },
    {
        { 0xace532bf458d72e1, 0x5be768e07cb73cb5, 0x56cf7d94ee8bbde7, 0x6b0697e3feb43a03 },
        { 0xa287ec4b5d0b2fbb, 0x415c5790074882ca, 0xe044a61ec1d0815c, 0x26334f0a409ef5e0 },
        { 0xb6c8f04adf62a3c0, 0x3ef000ef076da45d, 0x9c9cb95849f0d2a9, 0x1cc37f43441b2fae }
    },
    {
        { 0xd76656f1c9ceaeb9, 0x1c5b15f818e5656a, 0x26e72832844c2334, 0x3a346f772f196838 },
        { 0x508f565a5cc7324f, 0xd061c4c0e506a922, 0xfb18abdb5c45ac19, 0x6c6809c10380314a },
        { 0xd2d55112e2da6ac8, 0xe9bd0331b1e851ed, 0x960746dd8ec67262, 0x05911b9f6ef7c5d0 }
    },
},
{
    {
        { 0x01c18980c5fe9f94, 0xcd656769716fd5c8, 0x816045c3d195a086, 0x6e2b7f3266cc7982 },
        { 0xe9dcd756b637ff2d, 0xec4c348fc987f0c4, 0xced59285f3fbc7b7, 0x3305354793e1ea87 },
        { 0xcc802468f7c3568f, 0x9de9ba8219974cb3, 0xabb7229cb5b81360, 0x44e2017a6fbeba62 }
    },
    {
        { 0x87f82cf3b6ca6ecd, 0x580f893e18f4a0c2, 0x058930072604e557, 0x6cab6ac256d19c1d },
        { 0xc4c2a74354dab774, 0x8e5d4c3c4eaf031a, 0xb76c23d242838f17, 0x749a098f68dce4ea },
        { 0xdcdfe0a02cc1de60, 0x032665ff51c5575b, 0x2c0c32f1073abeeb, 0x6a882014cd7b8606 }
    },
    {
        { 0xd111d17caf4feb6e, 0x050bba42b33aa4a3, 0x17514c3ceeb46c30, 0x54bedb8b1bc27d75 },
        { 0xa52a92fea4747fb5, 0xdc12a4491fa5ab89, 0xd82da94bb847a4ce, 0x4d77edce9512cc4e },
        { 0x77c8e14577e2189c, 0xa3e46f6aff99c445, 0x3144dfc86d335343, 0x3a96559e7c4216a9 }
    },
    {
        { 0x4493896880baaa52, 0x4c98afc4f285940e, 0xef4aa79ba45448b6, 0x5278c510a57aae7f },
        { 0x12550d37f42ad2ee, 0x8b78e00498a1fbf5, 0x5d53078233894cb2, 0x02c84e4e3e498d0c },
        { 0xa54dd074294c0b94, 0xf55d46b8df18ffb6, 0xf06fecc58dae8366, 0x588657668190d165 }
    },
    {
        { 0xbf5834f03de25cc3, 0xb887c8aed6815496, 0x5105221a9481e892, 0x6760ed19f7723f93 },
        { 0xd47712311aef7117, 0x50343101229e92c7, 0x7a95e1849d159b97, 0x2449959b8b5d29c9 },
        { 0x669ba3b7ac35e160, 0x2eccf73fba842056, 0x1aec1f17c0804f07, 0x0d96bc031856f4e7 }
    },
    {
        { 0xb1d534b0cc7505e1, 0x32cd003416c35288, 0xcb36a5800762c29d, 0x5bfe69b9237a0bf8 },
        { 0x3318be7775c52d82, 0x4cb764b554d0aab9, 0xabcf3d27cc773d91, 0x3bf4d1848123288a },
        { 0x183eab7e78a151ab, 0xbbe990c999093763, 0xff717d6e4ac7e335, 0x4c5cddb325f39f88 }
    },
    {
        { 0x57750967e7a9f902, 0x2c37fdfc4f5b467e, 0xb261663a3177ba46, 0x3a375e78dc2d532b },
        { 0xc0f6b74d6190a6eb, 0x20ea81a42db8f4e4, 0xa8bd6f7d97315760, 0x33b1d60262ac7c21 },
        { 0x8141e72f2d4dddea, 0xe6eafe9862c607c8, 0x23c28458573cafd0, 0x46b9476f4ff97346 }
    },
    {
        { 0x1215505c0d58359f, 0x2a2013c7fc28c46b, 0x24a0a1af89ea664e, 0x4400b638a1130e1f },
        { 0x0c1ffea44f901e5c, 0x2b0b6fb72184b782, 0xe587ff910114db88, 0x37130f364785a142 },
        { 0x3a01b76496ed19c3, 0x31e00ab0ed327230, 0x520a885783ca15b1, 0x06aab9875accbec7 }
    },
},
{
    {
        { 0x5349acf3512eeaef, 0x20c141d31cc1cb49, 0x24180c07a99a688d, 0x555ef9d1c64b2d17 },
        { 0xc1339983f5df0ebb, 0xc0f3758f512c4cac, 0x2cf1130a0bb398e1, 0x6b3cecf9aa270c62 },
        { 0x36a770ba3b73bd08, 0x624aef08a3afbf0c, 0x5737ff98b40946f2, 0x675f4de13381749d }
    },
    {
        { 0xa12ff6d93bdab31d, 0x0725d80f9d652dfe, 0x019c4ff39abe9487, 0x60f450b882cd3c43 },
        { 0x0e2c52036b1782fc, 0x64816c816cad83b4, 0xd0dcbdd96964073e, 0x13d99df70164c520 },
        { 0x014b5ec321e5c0ca, 0x4fcb69c9d719bfa2, 0x4e5f1c18750023a0, 0x1c06de9e55edac80 }
    },
    {
        { 0xffd52b40ff6d69aa, 0x34530b18dc4049bb, 0x5e4a5c2fa34d9897, 0x78096f8e7d32ba2d },
        { 0x990f7ad6a33ec4e2, 0x6608f938be2ee08e, 0x9ca143c563284515, 0x4cf38a1fec2db60d },
        { 0xa0aaaa650dfa5ce7, 0xf9c49e2a48b5478c, 0x4f09cc7d7003725b, 0x373cad3a26091abe }
    },
    {
        { 0xf1bea8fb89ddbbad, 0x3bcb2cbc61aeaecb, 0x8f58a7bb1f9b8d9d, 0x21547eda5112a686 },
        { 0xb294634d82c9f57c, 0x1fcbfde124934536, 0x9e9c4db3418cdb5a, 0x0040f3d9454419fc },
        { 0xdefde939fd5986d3, 0xf4272c89510a380c, 0xb72ba407bb3119b9, 0x63550a334a254df4 }
    },
    {
        { 0x9bba584572547b49, 0xf305c6fae2c408e0, 0x60e8fa69c734f18d, 0x39a92bafaa7d767a },
        { 0x6507d6edb569cf37, 0x178429b00ca52ee1, 0xea7c0090eb6bd65d, 0x3eea62c7daf78f51 },
        { 0x9d24c713e693274e, 0x5f63857768dbd375, 0x70525560eb8ab39a, 0x68436a0665c9c4cd }
    },
    {
        { 0x1e56d317e820107c, 0xc5266844840ae965, 0xc1e0a1c6320ffc7a, 0x5373669c91611472 },
        { 0xbc0235e8202f3f27, 0xc75c00e264f975b0, 0x91a4e9d5a38c2416, 0x17b6e7f68ab789f9 },
        { 0x5d2814ab9a0e5257, 0x908f2084c9cab3fc, 0xafcaf5885b2d1eca, 0x1cb4b5a678f87d11 }
    },
    {
        { 0x6b74aa62a2a007e7, 0xf311e0b0f071c7b1, 0x5707e438000be223, 0x2dc0fd2d82ef6eac },
        { 0xb664c06b394afc6c, 0x0c88de2498da5fb1, 0x4f8d03164bcad834, 0x330bca78de7434a2 },
        { 0x982eff841119744e, 0xf9695e962b074724, 0xc58ac14fbfc953fb, 0x3c31be1b369f1cf5 }
    },
    {
        { 0xc168bc93f9cb4272, 0xaeb8711fc7cedb98, 0x7f0e52aa34ac8d7a, 0x41cec1097e7d55bb },
        { 0xb0f4864d08948aee, 0x07dc19ee91ba1c6f, 0x7975cdaea6aca158, 0x330b61134262d4bb },
        { 0xf79619d7a26d808a, 0xbb1fd49e1d9e156d, 0x73d7c36cdba1df27, 0x26b44cd91f28777d }
    },
},
{
    {
        { 0xaf44842db0285f37, 0x8753189047efc8df, 0x9574e091f820979a, 0x0e378d6069615579 },
        { 0x300a9035393aa6d8, 0x2b501131a12bb1cd, 0x7b1ff677f093c222, 0x4309c1f8cab82bad },
        { 0xd9fa917183075a55, 0x4bdb5ad26b009fdc, 0x7829ad2cd63def0e, 0x078fc54975fd3877 }
    },
    {
        { 0xe2004b5bb833a98a, 0x44775dec2d4c3330, 0x3aa244067eace913, 0x272630e3d58e00a9 },
        { 0x87dfbd1428878f2d, 0x134636dd1e9421a1, 0x4f17c951257341a3, 0x5df98d4bad296cb8 },
        { 0xf3678fd0ecc90b54, 0xf001459b12043599, 0x26725fbc3758b89b, 0x4325e4aa73a719ae }
    },
    {
        { 0xed24629acf69f59d, 0x2a4a1ccedd5abbf4, 0x3535ca1f56b2d67b, 0x5d8c68d043b1b42d },
        { 0x657dc6ef433c3493, 0x65375e9f80dbf8c3, 0x47fd2d465b372dae, 0x4966ab79796e7947 },
        { 0xee332d4de3b42b0a, 0xd84e5a2b16a4601c, 0x78243877078ba3e4, 0x77ed1eb4184ee437 }
    },
    {
        { 0x185d43f89e92ed1a, 0xb04a1eeafe4719c6, 0x499fbe88a6f03f4f, 0x5d8b0d2f3c859bdd },
        { 0xbfd4e13f201839a0, 0xaeefffe23e3df161, 0xb65b04f06b5d1fe3, 0x52e085fb2b62fbc0 },
        { 0x124079eaa54cf2ba, 0xd72465eb001b26e7, 0x6843bcfdc97af7fd, 0x0524b42b55eacd02 }
    },
    {
        { 0xbc18dcad9b829eac, 0x23ae7d28b5f579d0, 0xc346122a69384233, 0x1a6110b2e7d4ac89 },
        { 0xfd0d5dbee45447b0, 0x6cec351a092005ee, 0x99a47844567579cb, 0x59d242a216e7fa45 },
        { 0x4f833f6ae66997ac, 0x6849762a361839a4, 0x6985dec1970ab525, 0x53045e89dcb1f546 }
    },
    {
        { 0x84da3cde8d45fe12, 0xbd42c218e444e2d2, 0xa85196781f7e3598, 0x7642c93f5616e2b2 },
        { 0xcb8bb346d75353db, 0xfcfcb24bae511e22, 0xcba48d40d50ae6ef, 0x26e3bae5f4f7cb5d },
        { 0x2323daa74595f8e4, 0xde688c8b857abeb4, 0x3fc48e961c59326e, 0x0b2e73ca15c9b8ba }
    },
    {
        { 0x0e3fbfaf79c03a55, 0x3077af054cbb5acf, 0xd5c55245db3de39f, 0x015e68c1476a4af7 },
        { 0xd6bb4428c17f5026, 0x9eb27223fb5a9ca7, 0xe37ba5031919c644, 0x21ce380db59a6602 },
        { 0xc1d5285220066a38, 0x95603e523570aef3, 0x832659a7226b8a4d, 0x5dd689091f8eedc9 }
    },
    {
        { 0x1d022591a5313084, 0xca2d4aaed6270872, 0x86a12b852f0bfd20, 0x56e6c439ad7da748 },
        { 0xcbac84debfd3c856, 0x1624c348b35ff244, 0xb7f88dca5d9cad07, 0x3b0e574da2c2ebe8 },
        { 0xc704ff4942bdbae6, 0x5e21ade2b2de1f79, 0xe95db3f35652fad8, 0x0822b5378f08ebc1 }
    },
},
{
    {
        { 0xe1b7f29362730383, 0x4b5279ffebca8a2c, 0xdafc778abfd41314, 0x7deb10149c72610f },
        { 0x51f048478f387475, 0xb25dbcf49cbecb3c, 0x9aab1244d99f2055, 0x2c709e6c1c10a5d6 },
        { 0xcb62af6a8766ee7a, 0x66cbec045553cd0e, 0x588001380f0be4b5, 0x08e68e9ff62ce2ea }
    },
    {
        { 0x2f2d09d50ab8f2f9, 0xacb9218dc55923df, 0x4a8f342673766cb9, 0x4cb13bd738f719f5 },
        { 0x34ad500a4bc130ad, 0x8d38db493d0bd49c, 0xa25c3d98500a89be, 0x2f1f3f87eeba3b09 },
        { 0xf7848c75e515b64a, 0xa59501badb4a9038, 0xc20d313f3f751b50, 0x19a1e353c0ae2ee8 }
    },
    {
        { 0xb42172cdd596bdbd, 0x93e0454398eefc40, 0x9fb15347b44109b5, 0x736bd3990266ae34 },
        { 0x7d1c7560bafa05c3, 0xb3e1a0a0c6e55e61, 0xe3529718c0d66473, 0x41546b11c20c3486 },
        { 0x85532d509334b3b4, 0x46fd114b60816573, 0xcc5f5f30425c8375, 0x412295a2b87fab5c }
    },
    {
        { 0x2e655261e293eac6, 0x845a92032133acdb, 0x460975cb7900996b, 0x0760bb8d195add80 },
        { 0x19c99b88f57ed6e9, 0x5393cb266df8c825, 0x5cee3213b30ad273, 0x14e153ebb52d2e34 },
        { 0x413e1a17cde6818a, 0x57156da9ed69a084, 0x2cbf268f46caccb1, 0x6b34be9bc33ac5f2 }
    },
    {
        { 0x11fc69656571f2d3, 0xc6c9e845530e737a, 0xe33ae7a2d4fe5035, 0x01b9c7b62e6dd30b },
        { 0xf3df2f643a78c0b2, 0x4c3e971ef22e027c, 0xec7d1c5e49c1b5a3, 0x2012c18f0922dd2d },
        { 0x880b55e55ac89d29, 0x1483241f45a0a763, 0x3d36efdfc2e76c1f, 0x08af5b784e4bade8 }
    },
    {
        { 0xe27314d289cc2c4b, 0x4be4bd11a287178d, 0x18d528d6fa3364ce, 0x6423c1d5afd9826e },
        { 0x283499dc881f2533, 0x9d0525da779323b6, 0x897addfb673441f4, 0x32b79d71163a168d },
        { 0xcc85f8d9edfcb36a, 0x22bcc28f3746e5f9, 0xe49de338f9e5d3cd, 0x480a5efbc13e2dcc }
    },
    {
        { 0xb6614ce442ce221f, 0x6e199dcc4c053928, 0x663fb4a4dc1cbe03, 0x24b31d47691c8e06 },
        { 0x0b51e70b01622071, 0x06b505cf8b1dafc5, 0x2c6bb061ef5aabcd, 0x47aa27600cb7bf31 },
        { 0x2a541eedc015f8c3, 0x11a4fe7e7c693f7c, 0xf0af66134ea278d6, 0x545b585d14dda094 }
    },
    {
        { 0x6204e4d0e3b321e1, 0x3baa637a28ff1e95, 0x0b0ccffd5b99bd9e, 0x4d22dc3e64c8d071 },
        { 0x67bf275ea0d43a0f, 0xade68e34089beebe, 0x4289134cd479e72e, 0x0f62f9c332ba5454 },
        { 0xfcb46589d63b5f39, 0x5cae6a3f57cbcf61, 0xfebac2d2953afa05, 0x1c0fa01a36371436 }
    },
},
{
    {
        { 0xc11ee5e854c53fae, 0x6a0b06c12b4f3ff4, 0x33540f80e0b67a72, 0x15f18fc3cd07e3ef },
        { 0xe7547449bc7cd692, 0x0f9abeaae6f73ddf, 0x4af01ca700837e29, 0x63ab1b5d3f1bc183 },
        { 0x32750763b028f48c, 0x06020740556a065f, 0xd53bd812c3495b58, 0x08706c9b865f508d }
    },
    {
        { 0xcc991b4138b41246, 0x243b9c526f9ac26b, 0xb9ef494db7cbabbd, 0x5fba433dd082ed00 },
        { 0xf37ca2ab3d343dff, 0x1a8c6a2d80abc617, 0x8e49e035d4ccffca, 0x48b46beebaa1d1b9 },
        { 0x9c49e355c9941ad0, 0xb9734ade74498f84, 0x41c3fed066663e5c, 0x0ecfedf8e8e710b3 }
    },
    {
        { 0x744f7463e9403762, 0xf79a8dee8dfcc9c9, 0x163a649655e4cde3, 0x3b61788db284f435 },
        { 0x76430f9f9cd470d9, 0xb62acc9ba42f6008, 0x1898297c59adad5e, 0x7789dd2db78c5080 },
        { 0xb22228190d6ef6b2, 0xa94a66b246ce4bfa, 0x46c1a77a4f0b6cc7, 0x4236ccffeb7338cf }
    },
    {
        { 0x3bd82dbfda777df6, 0x71b177cc0b98369e, 0x1d0e8463850c3699, 0x5a71945b48e2d1f1 },
        { 0x8497404d0d55e274, 0x6c6663d9c4ad2b53, 0xec2fb0d9ada95734, 0x2617e120cdb8f73c },
        { 0x6f203dd5405b4b42, 0x327ec60410b24509, 0x9c347230ac2a8846, 0x77de29fc11ffeb6a }
    },
    {
        { 0x835e138fecced2ca, 0x8c9eaf13ea963b9a, 0xc95fbfc0b2160ea6, 0x575e66f3ad877892 },
        { 0xb0ac57c983b778a8, 0x53cdcca9d7fe912c, 0x61c2b854ff1f59dc, 0x3a1a2cf0f0de7dac },
        { 0x99803a27c88fcb3a, 0x345a6789275ec0b0, 0x459789d0ff6c2be5, 0x62f882651e70a8b2 }
    },
    {
        { 0x6d822986698a19e0, 0xdc9821e174d78a71, 0x41a85f31f6cb1f47, 0x352721c2bcda9c51 },
        { 0x085ae2c759ff1be4, 0x149145c93b0e40b7, 0xc467e7fa7ff27379, 0x4eeecf0ad5c73a95 },
        { 0x48329952213fc985, 0x1087cf0d368a1746, 0x8e5261b166c15aa5, 0x2d5b2d842ed24c21 }
    },
    {
        { 0x5eb7d13d196ac533, 0x377234ecdb80be2b, 0xe144cffc7cf5ae24, 0x5226bcf9c441acec },
        { 0x02cfebd9ebd3ded1, 0xd45b217739021974, 0x7576f813fe30a1b7, 0x5691b6f9a34ef6c2 },
        { 0x79ee6c7223e5b547, 0x6f5f50768330d679, 0xed73e1e96d8adce9, 0x27c3da1e1d8ccc03 }
    },
    {
        { 0x28302e71630ef9f6, 0xc2d4a2032b64cee0, 0x090820304b6292be, 0x5fca747aa82adf18 },
        { 0x7eb9efb23fe24c74, 0x3e50f49f1651be01, 0x3ea732dc21858dea, 0x17377bd75bb810f9 },
        { 0x232a03c35c258ea5, 0x86f23a2c6bcb0cf1, 0x3dad8d0d2e442166, 0x04a8933cab76862b }
    },
},
{
    {
        { 0x69082b0e8c936a50, 0xf9c9a035c1dac5b6, 0x6fb73e54c4dfb634, 0x4005419b1d2bc140 },
        { 0xd2c604b622943dff, 0xbc8cbece44cfb3a0, 0x5d254ff397808678, 0x0fa3614f3b1ca6bf },
        { 0xa003febdb9be82f0, 0x2089c1af3a44ac90, 0xf8499f911954fa8e, 0x1fba218aef40ab42 }
    },
    {
        { 0x4f3e57043e7b0194, 0xa81d3eee08daaf7f, 0xc839c6ab99dcdef1, 0x6c535d13ff7761d5 },
        { 0xab549448fac8f53e, 0x81f6e89a7ba63741, 0x74fd6c7d6c2b5e01, 0x392e3acaa8c86e42 },
        { 0x4cbd34e93e8a35af, 0x2e0781445887e816, 0x19319c76f29ab0ab, 0x25e17fe4d50ac13b }
    },
    {
        { 0x915f7ff576f121a7, 0xc34a32272fcd87e3, 0xccba2fde4d1be526, 0x6bba828f8969899b },
        { 0x0a289bd71e04f676, 0x208e1c52d6420f95, 0x5186d8b034691fab, 0x255751442a9fb351 },
        { 0xe2d1bc6690fe3901, 0x4cb54a18a0997ad5, 0x971d6914af8460d4, 0x559d504f7f6b7be4 }
    },
    {
        { 0x9c4891e7f6d266fd, 0x0744a19b0307781b, 0x88388f1d6061e23b, 0x123ea6a3354bd50e },
        { 0xa7738378b3eb54d5, 0x1d69d366a5553c7c, 0x0a26cf62f92800ba, 0x01ab12d5807e3217 },
        { 0x118d189041e32d96, 0xb9ede3c2d8315848, 0x1eab4271d83245d9, 0x4a3961e2c918a154 }
    },
    {
        { 0x0327d644f3233f1e, 0x499a260e34fcf016, 0x83b5a716f2dab979, 0x68aceead9bd4111f },
        { 0x71dc3be0f8e6bba0, 0xd6cef8347effe30a, 0xa992425fe13a476a, 0x2cd6bce3fb1db763 },
        { 0x38b4c90ef3d7c210, 0x308e6e24b7ad040c, 0x3860d9f1b7e73e23, 0x595760d5b508f597 }
    },
    {
        { 0x882acbebfd022790, 0x89af3305c4115760, 0x65f492e37d3473f4, 0x2cb2c5df54515a2b },
        { 0x6129bfe104aa6397, 0x8f960008a4a7fccb, 0x3f8bc0897d909458, 0x709fa43edcb291a9 },
        { 0xeb0a5d8c63fd2aca, 0xd22bc1662e694eff, 0x2723f36ef8cbb03a, 0x70f029ecf0c8131f }
    },
    {
        { 0x2a6aafaa5e10b0b9, 0x78f0a370ef041aa9, 0x773efb77aa3ad61f, 0x44eca5a2a74bd9e1 },
        { 0x461307b32eed3e33, 0xae042f33a45581e7, 0xc94449d3195f0366, 0x0b7d5d8a6c314858 },
        { 0x25d448327b95d543, 0x70d38300a3340f1d, 0xde1c531c60e1c52b, 0x272224512c7de9e4 }
    },
    {
        { 0xbf7bbb8a42a975fc, 0x8c5c397796ada358, 0xe27fc76fcdedaa48, 0x19735fd7f6bc20a6 },
        { 0x1abc92af49c5342e, 0xffeed811b2e6fad0, 0xefa28c8dfcc84e29, 0x11b5df18a44cc543 },
        { 0xe3ab90d042c84266, 0xeb848e0f7f19547e, 0x2503a1d065a497b9, 0x0fef911191df895f }
    },
},
{
    {
        { 0x6ab5dcb85b1c16b7, 0x94c0fce83c7b27a5, 0xa4b11c1a735517be, 0x499238d0ba0eafaa },
        { 0xb1507ca1ab1c6eb9, 0xbd448f3e16b687b3, 0x3455fb7f2c7a91ab, 0x7579229e2f2adec1 },
        { 0xecf46e527aba8b57, 0x15a08c478bd1647b, 0x7af1c6a65f706fef, 0x6345fa78f03a30d5 }
    },
    {
        { 0x93d3cbe9bdd8f0a4, 0xdb152c1bfd177302, 0x7dbddc6d7f17a875, 0x3e1a71cc8f426efe },
        { 0xdf02f95f1015e7a1, 0x790ec41da9b40263, 0x4d3a0ea133ea1107, 0x54f70be7e33af8c9 },
        { 0xc83ca3e390babd62, 0x80ede3670291c833, 0xc88038ccd37900c4, 0x2c5fc0231ec31fa1 }
    },
    {
        { 0xc422e4d102456e65, 0x87414ac1cad47b91, 0x1592e2bba2b6ffdd, 0x75d9d2bff5c2100f },
        { 0xfeba911717038b4f, 0xe5123721c9deef81, 0x1c97e4e75d0d8834, 0x68afae7a23dc3bc6 },
        { 0x5bd9b4763626e81c, 0x89966936bca02edd, 0x0a41193d61f077b3, 0x3097a24200ce5471 }
    },
    {
        { 0xa162e7246695c486, 0x131d633435a89607, 0x30521561a0d12a37, 0x56704bada6afb363 },
        { 0x57427734c7f8b84c, 0xf141a13e01b270e9, 0x02d1adfeb4e564a6, 0x4bb23d92ce83bd48 },
        { 0xaf6c4aa752f912b9, 0x5e665f6cd86770c8, 0x4c35ac83a3c8cd58, 0x2b7a29c010a58a7e }
    },
    {
        { 0x33810a23bf00086e, 0xafce925ee736ff7c, 0x3d60e670e24922d4, 0x11ce9e714f96061b },
        { 0xc4007f77d0c1cec3, 0x8d1020b6bac492f8, 0x32ec29d57e69daaf, 0x599408759d95fce0 },
        { 0x219ef713d815bac1, 0xf141465d485be25c, 0x6d5447cc4e513c51, 0x174926be5ef44393 }
    },
    {
        { 0x3ef5d41593ea022e, 0x5cbcc1a20ed0eed6, 0x8fd24ecf07382c8c, 0x6fa42ead06d8e1ad },
        { 0xb5deb2f9fc5bd5bb, 0x92daa72ae1d810e1, 0xafc4cfdcb72a1c59, 0x497d78813fc22a24 },
        { 0xe276824a1f73371f, 0x7f7cf01c4f5b6736, 0x7e201fe304fa46e7, 0x785a36a357808c96 }
    },
    {
        { 0x070442985d517bc3, 0x6acd56c7ae653678, 0x00a27983985a7763, 0x5167effae512662b },
        { 0x825fbdfd63014d2b, 0xc852369c6ca7578b, 0x5b2fcd285c0b5df0, 0x12ab214c58048c8f },
        { 0xbd4ea9e10f53c4b6, 0x1673dc5f8ac91a14, 0xa8f81a4e2acc1aba, 0x33a92a7924332a25 }
    },
    {
        { 0x7ba95ba0218f2ada, 0xcff42287330fb9ca, 0xdada496d56c6d907, 0x5380c296f4beee54 },
        { 0x9dd1f49927996c02, 0x0cb3b058e04d1752, 0x1f7e88967fd02c3e, 0x2f964268cb8b3eb1 },
        { 0x9d4f270466898d0a, 0x3d0987990aff3f7a, 0xd09ef36267daba45, 0x7761455e7b1c669c }
    },
},
};
#elif defined(CURVED25519_128BIT)
static const ge_precomp base[32][8] = {
{
    {
        { 0x493c6f58c3b85, 0x0df7181c325f7, 0x0f50b0b3e4cb7, 0x5329385a44c32, 0x07cf9d3a33d4b },
        { 0x03905d740913e, 0x0ba2817d673a2, 0x23e2827f4e67c, 0x133d2e0c21a34, 0x44fd2f9298f81 },
        { 0x11205877aaa68, 0x479955893d579, 0x50d66309b67a0, 0x2d42d0dbee5ee, 0x6f117b689f0c6 },
    },
    {
        { 0x4e7fc933c71d7, 0x2cf41feb6b244, 0x7581c0a7d1a76, 0x7172d534d32f0, 0x590c063fa87d2 },
        { 0x1a56042b4d5a8, 0x189cc159ed153, 0x5b8deaa3cae04, 0x2aaf04f11b5d8, 0x6bb595a669c92 },
        { 0x2a8b3a59b7a5f, 0x3abb359ef087f, 0x4f5a8c4db05af, 0x5b9a807d04205, 0x701af5b13ea50 },
    },
    {
        { 0x5b0a84cee9730, 0x61d10c97155e4, 0x4059cc8096a10, 0x47a608da8014f, 0x7a164e1b9a80f },
        { 0x11fe8a4fcd265, 0x7bcb8374faacc, 0x52f5af4ef4d4f, 0x5314098f98d10, 0x2ab91587555bd },
        { 0x6933f0dd0d889, 0x44386bb4c4295, 0x3cb6d3162508c, 0x26368b872a2c6, 0x5a2826af12b9b },
    },
    {
        { 0x351b98efc099f, 0x68fbfa4a7050e, 0x42a49959d971b, 0x393e51a469efd, 0x680e910321e58 },
        { 0x6050a056818bf, 0x62acc1f5532bf, 0x28141ccc9fa25, 0x24d61f471e683, 0x27933f4c7445a },
        { 0x3fbe9c476ff09, 0x0af6b982e4b42, 0x0ad1251ba78e5, 0x715aeedee7c88, 0x7f9d0cbf63553 },
    },
    {
        { 0x2bc4408a5bb33, 0x078ebdda05442, 0x2ffb112354123, 0x375ee8df5862d, 0x2945ccf146e20 },
        { 0x182c3a447d6ba, 0x22964e536eff2, 0x192821f540053, 0x2f9f19e788e5c, 0x154a7e73eb1b5 },
        { 0x3dbf1812a8285, 0x0fa17ba3f9797, 0x6f69cb49c3820, 0x34d5a0db3858d, 0x43aabe696b3bb },
    },
    {
        { 0x4eeeb77157131, 0x1201915f10741, 0x1669cda6c9c56, 0x45ec032db346d, 0x51e57bb6a2cc3 },
        { 0x006b67b7d8ca4, 0x084fa44e72933, 0x1154ee55d6f8a, 0x4425d842e7390, 0x38b64c41ae417 },
        { 0x4326702ea4b71, 0x06834376030b5, 0x0ef0512f9c380, 0x0f1a9f2512584, 0x10b8e91a9f0d6 },
    },
    {
        { 0x25cd0944ea3bf, 0x75673b81a4d63, 0x150b925d1c0d4, 0x13f38d9294114, 0x461bea69283c9 },
        { 0x72c9aaa3221b1, 0x267774474f74d, 0x064b0e9b28085, 0x3f04ef53b27c9, 0x1d6edd5d2e531 },
        { 0x36dc801b8b3a2, 0x0e0a7d4935e30, 0x1deb7cecc0d7d, 0x053a94e20dd2c, 0x7a9fbb1c6a0f9 },
    },
    {
        { 0x7596604dd3e8f, 0x6fc510e058b36, 0x3670c8db2cc0d, 0x297d899ce332f, 0x0915e76061bce },
        { 0x75dedf39234d9, 0x01c36ab1f3c54, 0x0f08fee58f5da, 0x0e19613a0d637, 0x3a9024a1320e0 },
        { 0x1f5d9c9a2911a, 0x7117994fafcf8, 0x2d8a8cae28dc5, 0x74ab1b2090c87, 0x26907c5c2ecc4 },
    },
},
{
    {
        { 0x4dd0e632f9c1d, 0x2ced12622a5d9, 0x18de9614742da, 0x79ca96fdbb5d4, 0x6dd37d49a00ee },
        { 0x3635449aa515e, 0x3e178d0475dab, 0x50b4712a19712, 0x2dcc2860ff4ad, 0x30d76d6f03d31 },
        { 0x444172106e4c7, 0x01251afed2d88, 0x534fc9bed4f5a, 0x5d85a39cf5234, 0x10c697112e864 },
    },
    {
        { 0x62aa08358c805, 0x46f440848e194, 0x447b771a8f52b, 0x377ba3269d31d, 0x03bf9baf55080 },
        { 0x3c4277dbe5fde, 0x5a335afd44c92, 0x0c1164099753e, 0x70487006fe423, 0x25e61cabed66f },
        { 0x3e128cc586604, 0x5968b2e8fc7e2, 0x049a3d5bd61cf, 0x116505b1ef6e6, 0x566d78634586e },
    },
    {
        { 0x54285c65a2fd0, 0x55e62ccf87420, 0x46bb961b19044, 0x1153405712039, 0x14fba5f34793b },
        { 0x7a49f9cc10834, 0x2b513788a22c6, 0x5ff4b6ef2395b, 0x2ec8e5af607bf, 0x33975bca5ecc3 },
        { 0x746166985f7d4, 0x09939000ae79a, 0x5844c7964f97a, 0x13617e1f95b3d, 0x14829cea83fc5 },
    },
    {
        { 0x70b2f4e71ecb8, 0x728148efc643c, 0x0753e03995b76, 0x5bf5fb2ab6767, 0x05fc3bc4535d7 },
        { 0x37b8497dd95c2, 0x61549d6b4ffe8, 0x217a22db1d138, 0x0b9cf062eb09e, 0x2fd9c71e5f758 },
        { 0x0b3ae52afdedd, 0x19da76619e497, 0x6fa0654d2558e, 0x78219d25e41d4, 0x373767475c651 },
    },
    {
        { 0x095cb14246590, 0x002d82aa6ac68, 0x442f183bc4851, 0x6464f1c0a0644, 0x6bf5905730907 },
        { 0x299fd40d1add9, 0x5f2de9a04e5f7, 0x7c0eebacc1c59, 0x4cca1b1f8290a, 0x1fbea56c3b18f },
        { 0x778f1e1415b8a, 0x6f75874efc1f4, 0x28a694019027f, 0x52b37a96bdc4d, 0x02521cf67a635 },
    },
    {
        { 0x46720772f5ee4, 0x632c0f359d622, 0x2b2092ba3e252, 0x662257c112680, 0x001753d9f7cd6 },
        { 0x7ee0b0a9d5294, 0x381fbeb4cca27, 0x7841f3a3e639d, 0x676ea30c3445f, 0x3fa00a7e71382 },
        { 0x1232d963ddb34, 0x35692e70b078d, 0x247ca14777a1f, 0x6db556be8fcd0, 0x12b5fe2fa048e },
    },
    {
        { 0x37c26ad6f1e92, 0x46a0971227be5, 0x4722f0d2d9b4c, 0x3dc46204ee03a, 0x6f7e93c20796c },
        { 0x0fbc496fce34d, 0x575be6b7dae3e, 0x4a31585cee609, 0x037e9023930ff, 0x749b76f96fb12 },
        { 0x2f604aea6ae05, 0x637dc939323eb, 0x3fdad9b048d47, 0x0a8b0d4045af7, 0x0fcec10f01e02 },
    },
    {
        { 0x2d29dc4244e45, 0x6927b1bc147be, 0x0308534ac0839, 0x4853664033f41, 0x413779166feab },
        { 0x558a649fe1e44, 0x44635aeefcc89, 0x1ff434887f2ba, 0x0f981220e2d44, 0x4901aa7183c51 },
        { 0x1b7548c1af8f0, 0x7848c53368116, 0x01b64e7383de9, 0x109fbb0587c8f, 0x41bb887b726d1 },
    },
},
{
    {
        { 0x34c597c6691ae, 0x7a150b6990fc4, 0x52beb9d922274, 0x70eed7164861a, 0x0a871e070c6a9 },
        { 0x07d44744346be, 0x282b6a564a81d, 0x4ed80f875236b, 0x6fbbe1d450c50, 0x4eb728c12fcdb },
        { 0x1b5994bbc8989, 0x74b7ba84c0660, 0x75678f1cdaeb8, 0x23206b0d6f10c, 0x3ee7300f2685d },
    },
    {
        { 0x27947841e7518, 0x32c7388dae87f, 0x414add3971be9, 0x01850832f0ef1, 0x7d47c6a2cfb89 },
        { 0x255e49e7dd6b7, 0x38c2163d59eba, 0x3861f2a005845, 0x2e11e4ccbaec9, 0x1381576297912 },
        { 0x2d0148ef0d6e0, 0x3522a8de787fb, 0x2ee055e74f9d2, 0x64038f6310813, 0x148cf58d34c9e },
    },
    {
        { 0x72f7d9ae4756d, 0x7711e690ffc4a, 0x582a2355b0d16, 0x0dccfe885b6b4, 0x278febad4eaea },
        { 0x492f67934f027, 0x7ded0815528d4, 0x58461511a6612, 0x5ea2e50de1544, 0x3ff2fa1ebd5db },
        { 0x2681f8c933966, 0x3840521931635, 0x674f14a308652, 0x3bd9c88a94890, 0x4104dd02fe9c6 },
    },
    {
        { 0x14e06db096ab8, 0x1219c89e6b024, 0x278abd486a2db, 0x240b292609520, 0x0165b5a48efca },
        { 0x2bf5e1124422a, 0x673146756ae56, 0x14ad99a87e830, 0x1eaca65b080fd, 0x2c863b00afaf5 },
        { 0x0a474a0846a76, 0x099a5ef981e32, 0x2a8ae3c4bbfe6, 0x45c34af14832c, 0x591b67d9bffec },
    },
    {
        { 0x1b3719f18b55d, 0x754318c83d337, 0x27c17b7919797, 0x145b084089b61, 0x489b4f8670301 },
        { 0x70d1c80b49bfa, 0x3d57e7d914625, 0x3c0722165e545, 0x5e5b93819e04f, 0x3de02ec7ca8f7 },
        { 0x2102d3aeb92ef, 0x68c22d50c3a46, 0x42ea89385894e, 0x75f9ebf55f38c, 0x49f5fbba496cb },
    },
    {
        { 0x5628c1e9c572e, 0x598b108e822ab, 0x55d8fae29361a, 0x0adc8d1a97b28, 0x06a1a6c288675 },
        { 0x49a108a5bcfd4, 0x6178c8e7d6612, 0x1f03473710375, 0x73a49614a6098, 0x5604a86dcbfa6 },
        { 0x0d1d47c1764b6, 0x01c08316a2e51, 0x2b3db45c95045, 0x1634f818d300c, 0x20989e89fe274 },
    },
    {
        { 0x4278b85eaec2e, 0x0ef59657be2ce, 0x72fd169588770, 0x2e9b205260b30, 0x730b9950f7059 },
        { 0x777fd3a2dcc7f, 0x594a9fb124932, 0x01f8e80ca15f0, 0x714d13cec3269, 0x0403ed1d0ca67 },
        { 0x32d35874ec552, 0x1f3048df1b929, 0x300d73b179b23, 0x6e67be5a37d0b, 0x5bd7454308303 },
    },
    {
        { 0x4932115e7792a, 0x457b9bbb930b8, 0x68f5d8b193226, 0x4164e8f1ed456, 0x5bb7db123067f },
        { 0x2d19528b24cc2, 0x4ac66b8302ff3, 0x701c8d9fdad51, 0x6c1b35c5b3727, 0x133a78007380a },
        { 0x1f467c6ca62be, 0x2c4232a5dc12c, 0x7551dc013b087, 0x0690c11b03bcd, 0x740dca6d58f0e },
    },
},
{
    {
        { 0x28c570478433c, 0x1d8502873a463, 0x7641e7eded49c, 0x1ecedd54cf571, 0x2c03f5256c2b0 },
        { 0x0ee0752cfce4e, 0x660dd8116fbe9, 0x55167130fffeb, 0x1c682b885955c, 0x161d25fa963ea },
        { 0x718757b53a47d, 0x619e18b0f2f21, 0x5fbdfe4c1ec04, 0x5d798c81ebb92, 0x699468bdbd96b },
    },
    {
        { 0x53de66aa91948, 0x045f81a599b1b, 0x3f7a8bd214193, 0x71d4da412331a, 0x293e1c4e6c4a2 },
        { 0x72f46f4dafecf, 0x2948ffadef7a3, 0x11ecdfdf3bc04, 0x3c2e98ffeed25, 0x525219a473905 },
        { 0x6134b925112e1, 0x6bb942bb406ed, 0x070c445c0dde2, 0x411d822c4d7a3, 0x5b605c447f032 },
    },
    {
        { 0x1fec6f0e7f04c, 0x3cebc692c477d, 0x077986a19a95e, 0x6eaaaa1778b0f, 0x2f12fef4cc5ab },
        { 0x5805920c47c89, 0x1924771f9972c, 0x38bbddf9fc040, 0x1f7000092b281, 0x24a76dcea8aeb },
        { 0x522b2dfc0c740, 0x7e8193480e148, 0x33fd9a04341b9, 0x3c863678a20bc, 0x5e607b2518a43 },
    },
    {
        { 0x4431ca596cf14, 0x015da7c801405, 0x03c9b6f8f10b5, 0x0346922934017, 0x201f33139e457 },
        { 0x31d8f6cdf1818, 0x1f86c4b144b16, 0x39875b8d73e9d, 0x2fbf0d9ffa7b3, 0x5067acab6ccdd },
        { 0x27f6b08039d51, 0x4802f8000dfaa, 0x09692a062c525, 0x1baea91075817, 0x397cba8862460 },
    },
    {
        { 0x5c3fbc81379e7, 0x41bbc255e2f02, 0x6a3f756998650, 0x1297fd4e07c42, 0x771b4022c1e1c },
        { 0x13093f05959b2, 0x1bd352f2ec618, 0x075789b88ea86, 0x61d1117ea48b9, 0x2339d320766e6 },
        { 0x5d986513a2fa7, 0x63f3a99e11b0f, 0x28a0ecfd6b26d, 0x53b6835e18d8f, 0x331a189219971 },
    },
    {
        { 0x12f3a9d7572af, 0x10d00e953c4ca, 0x603df116f2f8a, 0x33dc276e0e088, 0x1ac9619ff649a },
        { 0x66f45fb4f80c6, 0x3cc38eeb9fea2, 0x107647270db1f, 0x710f1ea740dc8, 0x31167c6b83bdf },
        { 0x33842524b1068, 0x77dd39d30fe45, 0x189432141a0d0, 0x088fe4eb8c225, 0x612436341f08b },
    },
    {
        { 0x349e31a2d2638, 0x0137a7fa6b16c, 0x681ae92777edc, 0x222bfc5f8dc51, 0x1522aa3178d90 },
        { 0x541db874e898d, 0x62d80fb841b33, 0x03e6ef027fa97, 0x7a03c9e9633e8, 0x46ebe2309e5ef },
        { 0x02f5369614938, 0x356e5ada20587, 0x11bc89f6bf902, 0x036746419c8db, 0x45fe70f505243 },
    },
    {
        { 0x24920c8951491, 0x107ec61944c5e, 0x72752e017c01f, 0x122b7dda2e97a, 0x16619f6db57a2 },
        { 0x075a6960c0b8c, 0x6dde1c5e41b49, 0x42e3f516da341, 0x16a03fda8e79e, 0x428d1623a0e39 },
        { 0x74a4401a308fd, 0x06ed4b9558109, 0x746f1f6a08867, 0x4636f5c6f2321, 0x1d81592d60bd3 },
    },
},
{
    {
        { 0x5b69f7b85c5e8, 0x17a2d175650ec, 0x4cc3e6dbfc19e, 0x73e1d3873be0e, 0x3a5f6d51b0af8 },
        { 0x68756a60dac5f, 0x55d757b8aec26, 0x3383df45f80bd, 0x6783f8c9f96a6, 0x20234a7789ecd },
        { 0x20db67178b252, 0x73aa3da2c0eda, 0x79045c01c70d3, 0x1b37b15251059, 0x7cd682353cffe },
    },
    {
        { 0x5cd6068acf4f3, 0x3079afc7a74cc, 0x58097650b64b4, 0x47fabac9c4e99, 0x3ef0253b2b2cd },
        { 0x1a45bd887fab6, 0x65748076dc17c, 0x5b98000aa11a8, 0x4a1ecc9080974, 0x2838c8863bdc0 },
        { 0x3b0cf4a465030, 0x022b8aef57a2d, 0x2ad0677e925ad, 0x4094167d7457a, 0x21dcb8a606a82 },
    },
    {
        { 0x500fabe7731ba, 0x7cc53c3113351, 0x7cf65fe080d81, 0x3c5d966011ba1, 0x5d840dbf6c6f6 },
        { 0x004468c9d9fc8, 0x5da8554796b8c, 0x3b8be70950025, 0x6d5892da6a609, 0x0bc3d08194a31 },
        { 0x6380d309fe18b, 0x4d73c2cb8ee0d, 0x6b882adbac0b6, 0x36eabdddd4cbe, 0x3a4276232ac19 },
    },
    {
        { 0x0c172db447ecb, 0x3f8c505b7a77f, 0x6a857f97f3f10, 0x4fcc0567fe03a, 0x0770c9e824e1a },
        { 0x2432c8a7084fa, 0x47bf73ca8a968, 0x1639176262867, 0x5e8df4f8010ce, 0x1ff177cea16de },
        { 0x1d99a45b5b5fd, 0x523674f2499ec, 0x0f8fa26182613, 0x58f7398048c98, 0x39f264fd41500 },
    },
    {
        { 0x34aabfe097be1, 0x43bfc03253a33, 0x29bc7fe91b7f3, 0x0a761e4844a16, 0x65c621272c35f },
        { 0x53417dbe7e29c, 0x54573827394f5, 0x565eea6f650dd, 0x42050748dc749, 0x1712d73468889 },
        { 0x389f8ce3193dd, 0x2d424b8177ce5, 0x073fa0d3440cd, 0x139020cd49e97, 0x22f9800ab19ce },
    },
    {
        { 0x29fdd9a6efdac, 0x7c694a9282840, 0x6f7cdeee44b3a, 0x55a3207b25cc3, 0x4171a4d38598c },
        { 0x2368a3e9ef8cb, 0x454aa08e2ac0b, 0x490923f8fa700, 0x372aa9ea4582f, 0x13f416cd64762 },
        { 0x758aa99c94c8c, 0x5f6001700ff44, 0x7694e488c01bd, 0x0d5fde948eed6, 0x508214fa574bd },
    },
    {
        { 0x215bb53d003d6, 0x1179e792ca8c3, 0x1a0e96ac840a2, 0x22393e2bb3ab6, 0x3a7758a4c86cb },
        { 0x269153ed6fe4b, 0x72a23aef89840, 0x052be5299699c, 0x3a5e5ef132316, 0x22f960ec6faba },
        { 0x111f693ae5076, 0x3e3bfaa94ca90, 0x445799476b887, 0x24a0912464879, 0x5d9fd15f8de7f },
    },
    {
        { 0x44d2aeed7521e, 0x50865d2c2a7e4, 0x2705b5238ea40, 0x46c70b25d3b97, 0x3bc187fa47eb9 },
        { 0x408d36d63727f, 0x5faf8f6a66062, 0x2bb892da8de6b, 0x769d4f0c7e2e6, 0x332f35914f8fb },
        { 0x70115ea86c20c, 0x16d88da24ada8, 0x1980622662adf, 0x501ebbc195a9d, 0x450d81ce906fb },
    },
},
{
    {
        { 0x4d8961cae743f, 0x6bdc38c7dba0e, 0x7d3b4a7e1b463, 0x0844bdee2adf3, 0x4cbad279663ab },
        { 0x3b6a1a6205275, 0x2e82791d06dcf, 0x23d72caa93c87, 0x5f0b7ab68aaf4, 0x2de25d4ba6345 },
        { 0x19024a0d71fcd, 0x15f65115f101a, 0x4e99067149708, 0x119d8d1cba5af, 0x7d7fbcefe2007 },
    },
    {
        { 0x45dc5f3c29094, 0x3455220b579af, 0x070c1631e068a, 0x26bc0630e9b21, 0x4f9cd196dcd8d },
        { 0x71e6a266b2801, 0x09aae73e2df5d, 0x40dd8b219b1a3, 0x546fb4517de0d, 0x5975435e87b75 },
        { 0x297d86a7b3768, 0x4835a2f4c6332, 0x070305f434160, 0x183dd014e56ae, 0x7ccdd084387a0 },
    },
    {
        { 0x484186760cc93, 0x7435665533361, 0x02f686336b801, 0x5225446f64331, 0x3593ca848190c },
        { 0x6422c6d260417, 0x212904817bb94, 0x5a319deb854f5, 0x7a9d4e060da7d, 0x428bd0ed61d0c },
        { 0x3189a5e849aa7, 0x6acbb1f59b242, 0x7f6ef4753630c, 0x1f346292a2da9, 0x27398308da2d6 },
    },
    {
        { 0x10e4c0a702453, 0x4daafa37bd734, 0x49f6bdc3e8961, 0x1feffdcecdae6, 0x572c2945492c3 },
        { 0x38d28435ed413, 0x4064f19992858, 0x7680fbef543cd, 0x1aadd83d58d3c, 0x269597aebe8c3 },
        { 0x7c745d6cd30be, 0x27c7755df78ef, 0x1776833937fa3, 0x5405116441855, 0x7f985498c05bc },
    },
    {
        { 0x615520fbf6363, 0x0b9e9bf74da6a, 0x4fe8308201169, 0x173f76127de43, 0x30f2653cd69b1 },
        { 0x1ce889f0be117, 0x36f6a94510709, 0x7f248720016b4, 0x1821ed1e1cf91, 0x76c2ec470a31f },
        { 0x0c938aac10c85, 0x41b64ed797141, 0x1beb1c1185e6d, 0x1ed5490600f07, 0x2f1273f159647 },
    },
    {
        { 0x08bd755a70bc0, 0x49e3a885ce609, 0x16585881b5ad6, 0x3c27568d34f5e, 0x38ac1997edc5f },
        { 0x1fc7c8ae01e11, 0x2094d5573e8e7, 0x5ca3cbbf549d2, 0x4f920ecc54143, 0x5d9e572ad85b6 },
        { 0x6b517a751b13b, 0x0cfd370b180cc, 0x5377925d1f41a, 0x34e56566008a2, 0x22dfcd9cbfe9e },
    },
    {
        { 0x459b4103be0a1, 0x59a4b3f2d2add, 0x7d734c8bb8eeb, 0x2393cbe594a09, 0x0fe9877824cde },
        { 0x3d2e0c30d0cd9, 0x3f597686671bb, 0x0aa587eb63999, 0x0e3c7b592c619, 0x6b2916c05448c },
        { 0x334d10aba913b, 0x045cdb581cfdb, 0x5e3e0553a8f36, 0x50bb3041effb2, 0x4c303f307ff00 },
    },
    {
        { 0x403580dd94500, 0x48df77d92653f, 0x38a9fe3b349ea, 0x0ea89850aafe1, 0x416b151ab706a },
        { 0x23bd617b28c85, 0x6e72ee77d5a61, 0x1a972ff174dde, 0x3e2636373c60f, 0x0d61b8f78b2ab },
        { 0x0d7efe9c136b0, 0x1ab1c89640ad5, 0x55f82aef41f97, 0x46957f317ed0d, 0x191a2af74277e },
    },
},
{
    {
        { 0x62b434f460efb, 0x294c6c0fad3fc, 0x68368937b4c0f, 0x5c9f82910875b, 0x237e7dbe00545 },
        { 0x6f74bc53c1431, 0x1c40e5dbbd9c2, 0x6c8fb9cae5c97, 0x4845c5ce1b7da, 0x7e2e0e450b5cc },
        { 0x575ed6701b430, 0x4d3e17fa20026, 0x791fc888c4253, 0x2f1ba99078ac1, 0x71afa699b1115 },
    },
    {
        { 0x23c1c473b50d6, 0x3e7671de21d48, 0x326fa5547a1e8, 0x50e4dc25fafd9, 0x00731fbc78f89 },
        { 0x66f9b3953b61d, 0x555f4283cccb9, 0x7dd67fb1960e7, 0x14707a1affed4, 0x021142e9c2b1c },
        { 0x0c71848f81880, 0x44bd9d8233c86, 0x6e8578efe5830, 0x4045b6d7041b5, 0x4c4d6f3347e15 },
    },
    {
        { 0x4ddfc988f1970, 0x4f6173ea365e1, 0x645daf9ae4588, 0x7d43763db623b, 0x38bf9500a88f9 },
        { 0x7eccfc17d1fc9, 0x4ca280782831e, 0x7b8337db1d7d6, 0x5116def3895fb, 0x193fddaaa7e47 },
        { 0x2c93c37e8876f, 0x3431a28c583fa, 0x49049da8bd879, 0x4b4a8407ac11c, 0x6a6fb99ebf0d4 },
    },
    {
        { 0x122b5b6e423c6, 0x21e50dff1ddd6, 0x73d76324e75c0, 0x588485495418e, 0x136fda9f42c5e },
        { 0x6c1bb560855eb, 0x71f127e13ad48, 0x5c6b304905aec, 0x3756b8e889bc7, 0x75f76914a3189 },
        { 0x4dfb1a305bdd1, 0x3b3ff05811f29, 0x6ed62283cd92e, 0x65d1543ec52e1, 0x022183510be8d },
    },
    {
        { 0x2710143307a7f, 0x3d88fb48bf3ab, 0x249eb4ec18f7a, 0x136115dff295f, 0x1387c441fd404 },
        { 0x766385ead2d14, 0x0194f8b06095e, 0x08478f6823b62, 0x6018689d37308, 0x6a071ce17b806 },
        { 0x3c3d187978af8, 0x7afe1c88276ba, 0x51df281c8ad68, 0x64906bda4245d, 0x3171b26aaf1ed },
    },
    {
        { 0x5b7d8b28a47d1, 0x2c2ee149e34c1, 0x776f5629afc53, 0x1f4ea50fc49a9, 0x6c514a6334424 },
        { 0x7319097564ca8, 0x1844ebc233525, 0x21d4543fdeee1, 0x1ad27aaff1bd2, 0x221fd4873cf08 },
        { 0x2204f3a156341, 0x537414065a464, 0x43c0c3bedcf83, 0x5557e706ea620, 0x48daa596fb924 },
    },
    {
        { 0x61d5dc84c9793, 0x47de83040c29e, 0x189deb26507e7, 0x4d4e6fadc479a, 0x58c837fa0e8a7 },
        { 0x28e665ca59cc7, 0x165c715940dd9, 0x0785f3aa11c95, 0x57b98d7e38469, 0x676dd6fccad84 },
        { 0x1688596fc9058, 0x66f6ad403619f, 0x4d759a87772ef, 0x7856e6173bea4, 0x1c4f73f2c6a57 },
    },
    {
        { 0x6706efc7c3484, 0x6987839ec366d, 0x0731f95cf7f26, 0x3ae758ebce4bc, 0x70459adb7daf6 },
        { 0x24fbd305fa0bb, 0x40a98cc75a1cf, 0x78ce1220a7533, 0x6217a10e1c197, 0x795ac80d1bf64 },
        { 0x1db4991b42bb3, 0x469605b994372, 0x631e3715c9a58, 0x7e9cfefcf728f, 0x5fe162848ce21 },
    },
},
{
    {
        { 0x1852d5d7cb208, 0x60d0fbe5ce50f, 0x5a1e246e37b75, 0x51aee05ffd590, 0x2b44c043677da },
        { 0x1214fe194961a, 0x0e1ae39a9e9cb, 0x543c8b526f9f7, 0x119498067e91d, 0x4789d446fc917 },
        { 0x487ab074eb78e, 0x1d33b5e8ce343, 0x13e419feb1b46, 0x2721f565de6a4, 0x60c52eef2bb9a },
    },
    {
        { 0x3c5c27cae6d11, 0x36a9491956e05, 0x124bac9131da6, 0x3b6f7de202b5d, 0x70d77248d9b66 },
        { 0x589bc3bfd8bf1, 0x6f93e6aa3416b, 0x4c0a3d6c1ae48, 0x55587260b586a, 0x10bc9c312ccfc },
        { 0x2e84b3ec2a05b, 0x69da2f03c1551, 0x23a174661a67b, 0x209bca289f238, 0x63755bd3a976f },
    },
    {
        { 0x7101897f1acb7, 0x3d82cb77b07b8, 0x684083d7769f5, 0x52b28472dce07, 0x2763751737c52 },
        { 0x7a03e2ad10853, 0x213dcc6ad36ab, 0x1a6e240d5bdd6, 0x7c24ffcf8fedf, 0x0d8cc1c48bc16 },
        { 0x402d36eb419a9, 0x7cef68c14a052, 0x0f1255bc2d139, 0x373e7d431186a, 0x70c2dd8a7ad16 },
    },
    {
        { 0x4967db8ed7e13, 0x15aeed02f523a, 0x6149591d094bc, 0x672f204c17006, 0x32b8613816a53 },
        { 0x194509f6fec0e, 0x528d8ca31acac, 0x7826d73b8b9fa, 0x24acb99e0f9b3, 0x2e0fac6363948 },
        { 0x7f7bee448cd64, 0x4e10f10da0f3c, 0x3936cb9ab20e9, 0x7a0fc4fea6cd0, 0x4179215c735a4 },
    },
    {
        { 0x633b9286bcd34, 0x6cab3badb9c95, 0x74e387edfbdfa, 0x14313c58a0fd9, 0x31fa85662241c },
        { 0x094e7d7dced2a, 0x068fa738e118e, 0x41b640a5fee2b, 0x6bb709df019d4, 0x700344a30cd99 },
        { 0x26c422e3622f4, 0x0f3066a05b5f0, 0x4e2448f0480a6, 0x244cde0dbf095, 0x24bb2312a9952 },
    },
    {
        { 0x00c2af5f85c6b, 0x0609f4cf2883f, 0x6e86eb5a1ca13, 0x68b44a2efccd1, 0x0d1d2af9ffeb5 },
        { 0x0ed1732de67c3, 0x308c369291635, 0x33ef348f2d250, 0x004475ea1a1bb, 0x0fee3e871e188 },
        { 0x28aa132621edf, 0x42b244caf353b, 0x66b064cc2e08a, 0x6bb20020cbdd3, 0x16acd79718531 },
    },
    {
        { 0x1c6c57887b6ad, 0x5abf21fd7592b, 0x50bd41253867a, 0x3800b71273151, 0x164ed34b18161 },
        { 0x772af2d9b1d3d, 0x6d486448b4e5b, 0x2ce58dd8d18a8, 0x1849f67503c8b, 0x123e0ef6b9302 },
        { 0x6d94c192fe69a, 0x5475222a2690f, 0x693789d86b8b3, 0x1f5c3bdfb69dc, 0x78da0fc61073f },
    },
    {
        { 0x780f1680c3a94, 0x2a35d3cfcd453, 0x005e5cdc7ddf8, 0x6ee888078ac24, 0x054aa4b316b38 },
        { 0x15d28e52bc66a, 0x30e1e0351cb7e, 0x30a2f74b11f8c, 0x39d120cd7de03, 0x2d25deeb256b1 },
        { 0x0468d19267cb8, 0x38cdca9b5fbf9, 0x1bbb05c2ca1e2, 0x3b015758e9533, 0x134610a6ab7da },
    },
},
{
    {
        { 0x265e777d1f515, 0x0f1f54c1e39a5, 0x2f01b95522646, 0x4fdd8db9dde6d, 0x654878cba97cc },
        { 0x38ec78df6b0fe, 0x13caebea36a22, 0x5ebc6e54e5f6a, 0x32804903d0eb8, 0x2102fdba2b20d },
        { 0x6e405055ce6a1, 0x5024a35a532d3, 0x1f69054daf29d, 0x15d1d0d7a8bd5, 0x0ad725db29ecb },
    },
    {
        { 0x7bc0c9b056f85, 0x51cfebffaffd8, 0x44abbe94df549, 0x7ecbbd7e33121, 0x4f675f5302399 },
        { 0x267b1834e2457, 0x6ae19c378bb88, 0x7457b5ed9d512, 0x3280d783d05fb, 0x4aefcffb71a03 },
        { 0x536360415171e, 0x2313309077865, 0x251444334afbc, 0x2b0c3853756e8, 0x0bccbb72a2a86 },
    },
    {
        { 0x55e4c50fe1296, 0x05fdd13efc30d, 0x1c0c6c380e5ee, 0x3e11de3fb62a8, 0x6678fd69108f3 },
        { 0x6962feab1a9c8, 0x6aca28fb9a30b, 0x56db7ca1b9f98, 0x39f58497018dd, 0x4024f0ab59d6b },
        { 0x6fa31636863c2, 0x10ae5a67e42b0, 0x27abbf01fda31, 0x380a7b9e64fbc, 0x2d42e2108ead4 },
    },
    {
        { 0x17b0d0f537593, 0x16263c0c9842e, 0x4ab827e4539a4, 0x6370ddb43d73a, 0x420bf3a79b423 },
        { 0x5131594dfd29b, 0x3a627e98d52fe, 0x1154041855661, 0x19175d09f8384, 0x676b2608b8d2d },
        { 0x0ba651c5b2b47, 0x5862363701027, 0x0c4d6c219c6db, 0x0f03dff8658de, 0x745d2ffa9c0cf },
    },
    {
        { 0x6df5721d34e6a, 0x4f32f767a0c06, 0x1d5abeac76e20, 0x41ce9e104e1e4, 0x06e15be54c1dc },
        { 0x25a1e2bc9c8bd, 0x104c8f3b037ea, 0x405576fa96c98, 0x2e86a88e3876f, 0x1ae23ceb960cf },
        { 0x25d871932994a, 0x6b9d63b560b6e, 0x2df2814c8d472, 0x0fbbee20aa4ed, 0x58ded861278ec },
    },
    {
        { 0x35ba8b6c2c9a8, 0x1dea58b3185bf, 0x4b455cd23bbbe, 0x5ec19c04883f8, 0x08ba696b531d5 },
        { 0x73793f266c55c, 0x0b988a9c93b02, 0x09b0ea32325db, 0x37cae71c17c5e, 0x2ff39de85485f },
        { 0x53eeec3efc57a, 0x2fa9fe9022efd, 0x699c72c138154, 0x72a751ebd1ff8, 0x120633b4947cf },
    },
    {
        { 0x531474912100a, 0x5afcdf7c0d057, 0x7a9e71b788ded, 0x5ef708f3b0c88, 0x07433be3cb393 },
        { 0x4987891610042, 0x79d9d7f5d0172, 0x3c293013b9ec4, 0x0c2b85f39caca, 0x35d30a99b4d59 },
        { 0x144c05ce997f4, 0x4960b8a347fef, 0x1da11f15d74f7, 0x54fac19c0fead, 0x2d873ede7af6d },
    },
    {
        { 0x202e14e5df981, 0x2ea02bc3eb54c, 0x38875b2883564, 0x1298c513ae9dd, 0x0543618a01600 },
        { 0x2316443373409, 0x5de95503b22af, 0x699201beae2df, 0x3db5849ff737a, 0x2e773654707fa },
        { 0x2bdf4974c23c1, 0x4b3b9c8d261bd, 0x26ae8b2a9bc28, 0x3068210165c51, 0x4b1443362d079 },
    },
},
{
    {
        { 0x454e91c529ccb, 0x24c98c6bf72cf, 0x0486594c3d89a, 0x7ae13a3d7fa3c, 0x17038418eaf66 },
        { 0x4b7c7b66e1f7a, 0x4bea185efd998, 0x4fabc711055f8, 0x1fb9f7836fe38, 0x582f446752da6 },
        { 0x17bd320324ce4, 0x51489117898c6, 0x1684d92a0410b, 0x6e4d90f78c5a7, 0x0c2a1c4bcda28 },
    },
    {
        { 0x4814869bd6945, 0x7b7c391a45db8, 0x57316ac35b641, 0x641e31de9096a, 0x5a6a9b30a314d },
        { 0x5c7d06f1f0447, 0x7db70f80b3a49, 0x6cb4a3ec89a78, 0x43be8ad81397d, 0x7c558bd1c6f64 },
        { 0x41524d396463d, 0x1586b449e1a1d, 0x2f17e904aed8a, 0x7e1d2861d3c8e, 0x0404a5ca0afba },
    },
    {
        { 0x49e1b2a416fd1, 0x51c6a0b316c57, 0x575a59ed71bdc, 0x74c021a1fec1e, 0x39527516e7f8e },
        { 0x740070aa743d6, 0x16b64cbdd1183, 0x23f4b7b32eb43, 0x319aba58235b3, 0x46395bfdcadd9 },
        { 0x7db2d1a5d9a9c, 0x79a200b85422f, 0x355bfaa71dd16, 0x00b77ea5f78aa, 0x76579a29e822d },
    },
    {
        { 0x4b51352b434f2, 0x1327bd01c2667, 0x434d73b60c8a1, 0x3e0daa89443ba, 0x02c514bb2a277 },
        { 0x68e7e49c02a17, 0x45795346fe8b6, 0x089306c8f3546, 0x6d89f6b2f88f6, 0x43a384dc9e05b },
        { 0x3d5da8bf1b645, 0x7ded6a96a6d09, 0x6c3494fee2f4d, 0x02c989c8b6bd4, 0x1160920961548 },
    },
    {
        { 0x05616369b4dcd, 0x4ecab86ac6f47, 0x3c60085d700b2, 0x0213ee10dfcea, 0x2f637d7491e6e },
        { 0x5166929dacfaa, 0x190826b31f689, 0x4f55567694a7d, 0x705f4f7b1e522, 0x351e125bc5698 },
        { 0x49b461af67bbe, 0x75915712c3a96, 0x69a67ef580c0d, 0x54d38ef70cffc, 0x7f182d06e7ce2 },
    },
    {
        { 0x54b728e217522, 0x69a90971b0128, 0x51a40f2a963a3, 0x10be9ac12a6bf, 0x44acc043241c5 },
        { 0x48e64ab0168ec, 0x2a2bdb8a86f4f, 0x7343b6b2d6929, 0x1d804aa8ce9a3, 0x67d4ac8c343e9 },
        { 0x56bbb4f7a5777, 0x29230627c238f, 0x5ad1a122cd7fb, 0x0dea56e50e364, 0x556d1c8312ad7 },
    },
    {
        { 0x06756b11be821, 0x462147e7bb03e, 0x26519743ebfe0, 0x782fc59682ab5, 0x097abe38cc8c7 },
        { 0x740e30c8d3982, 0x7c2b47f4682fd, 0x5cd91b8c7dc1c, 0x77fa790f9e583, 0x746c6c6d1d824 },
        { 0x1c9877ea52da4, 0x2b37b83a86189, 0x733af49310da5, 0x25e81161c04fb, 0x577e14a34bee8 },
    },
    {
        { 0x6cebebd4dd72b, 0x340c1e442329f, 0x32347ffd1a93f, 0x14a89252cbbe0, 0x705304b8fb009 },
        { 0x268ac61a73b0a, 0x206f234bebe1c, 0x5b403a7cbebe8, 0x7a160f09f4135, 0x60fa7ee96fd78 },
        { 0x51d354d296ec6, 0x7cbf5a63b16c7, 0x2f50bb3cf0c14, 0x1feb385cac65a, 0x21398e0ca1635 },
    },
},
{
    {
        { 0x0aaf9b4b75601, 0x26b91b5ae44f3, 0x6de808d7ab1c8, 0x6a769675530b0, 0x1bbfb284e98f7 },
        { 0x5058a382b33f3, 0x175a91816913e, 0x4f6cdb96b8ae8, 0x17347c9da81d2, 0x5aa3ed9d95a23 },
        { 0x777e9c7d96561, 0x28e58f006ccac, 0x541bbbb2cac49, 0x3e63282994cec, 0x4a07e14e5e895 },
    },
    {
        { 0x358cdc477a49b, 0x3cc88fe02e481, 0x721aab7f4e36b, 0x0408cc9469953, 0x50af7aed84afa },
        { 0x412cb980df999, 0x5e78dd8ee29dc, 0x171dff68c575d, 0x2015dd2f6ef49, 0x3f0bac391d313 },
        { 0x7de0115f65be5, 0x4242c21364dc9, 0x6b75b64a66098, 0x0033c0102c085, 0x1921a316baebd },
    },
    {
        { 0x2ad9ad9f3c18b, 0x5ec1638339aeb, 0x5703b6559a83b, 0x3fa9f4d05d612, 0x7b049deca062c },
        { 0x22f7edfb870fc, 0x569eed677b128, 0x30937dcb0a5af, 0x758039c78ea1b, 0x6458df41e273a },
        { 0x3e37a35444483, 0x661fdb7d27b99, 0x317761dd621e4, 0x7323c30026189, 0x6093dccbc2950 },
    },
    {
        { 0x6eebe6084034b, 0x6cf01f70a8d7b, 0x0b41a54c6670a, 0x6c84b99bb55db, 0x6e3180c98b647 },
        { 0x39a8585e0706d, 0x3167ce72663fe, 0x63d14ecdb4297, 0x4be21dcf970b8, 0x57d1ea084827a },
        { 0x2b6e7a128b071, 0x5b27511755dcf, 0x08584c2930565, 0x68c7bda6f4159, 0x363e999ddd97b },
    },
    {
        { 0x048dce24baec6, 0x2b75795ec05e3, 0x3bfa4c5da6dc9, 0x1aac8659e371e, 0x231f979bc6f9b },
        { 0x043c135ee1fc4, 0x2a11c9919f2d5, 0x6334cc25dbacd, 0x295da17b400da, 0x48ee9b78693a0 },
        { 0x1de4bcc2af3c6, 0x61fc411a3eb86, 0x53ed19ac12ec0, 0x209dbc6b804e0, 0x079bfa9b08792 },
    },
    {
        { 0x1ed80a2d54245, 0x70efec72a5e79, 0x42151d42a822d, 0x1b5ebb6d631e8, 0x1ef4fb1594706 },
        { 0x03a51da300df4, 0x467b52b561c72, 0x4d5920210e590, 0x0ca769e789685, 0x038c77f684817 },
        { 0x65ee65b167bec, 0x052da19b850a9, 0x0408665656429, 0x7ab39596f9a4c, 0x575ee92a4a0bf },
    },
    {
        { 0x6bc450aa4d801, 0x4f4a6773b0ba8, 0x6241b0b0ebc48, 0x40d9c4f1d9315, 0x200a1e7e382f5 },
        { 0x080908a182fcf, 0x0532913b7ba98, 0x3dccf78c385c3, 0x68002dd5eaba9, 0x43d4e7112cd3f },
        { 0x5b967eaf93ac5, 0x360acca580a31, 0x1c65fd5c6f262, 0x71c7f15c2ecab, 0x050eca52651e4 },
    },
    {
        { 0x4397660e668ea, 0x7c2a75692f2f5, 0x3b29e7e6c66ef, 0x72ba658bcda9a, 0x6151c09fa131a },
        { 0x31ade453f0c9c, 0x3dfee07737868, 0x611ecf7a7d411, 0x2637e6cbd64f6, 0x4b0ee6c21c58f },
        { 0x55c0dfdf05d96, 0x405569dcf475e, 0x05c5c277498bb, 0x18588d95dc389, 0x1fef24fa800f0 },
    },
},
{
    {
        { 0x2aff530976b86, 0x0d85a48c0845a, 0x796eb963642e0, 0x60bee50c4b626, 0x28005fe6c8340 },
        { 0x653fb1aa73196, 0x607faec8306fa, 0x4e85ec83e5254, 0x09f56900584fd, 0x544d49292fc86 },
        { 0x7ba9f34528688, 0x284a20fb42d5d, 0x3652cd9706ffe, 0x6fd7baddde6b3, 0x72e472930f316 },
    },
    {
        { 0x3f635d32a7627, 0x0cbecacde00fe, 0x3411141eaa936, 0x21c1e42f3cb94, 0x1fee7f000fe06 },
        { 0x5208c9781084f, 0x16468a1dc24d2, 0x7bf780ac540a8, 0x1a67eced75301, 0x5a9d2e8c2733a },
        { 0x305da03dbf7e5, 0x1228699b7aeca, 0x12a23b2936bc9, 0x2a1bda56ae6e9, 0x00f94051ee040 },
    },
    {
        { 0x793bb07af9753, 0x1e7b6ecd4fafd, 0x02c7b1560fb43, 0x2296734cc5fb7, 0x47b7ffd25dd40 },
        { 0x56b23c3d330b2, 0x37608e360d1a6, 0x10ae0f3c8722e, 0x086d9b618b637, 0x07d79c7e8beab },
        { 0x3fb9cbc08dd12, 0x75c3dd85370ff, 0x47f06fe2819ac, 0x5db06ab9215ed, 0x1c3520a35ea64 },
    },
    {
        { 0x06f40216bc059, 0x3a2579b0fd9b5, 0x71c26407eec8c, 0x72ada4ab54f0b, 0x38750c3b66d12 },
        { 0x253a6bccba34a, 0x427070433701a, 0x20b8e58f9870e, 0x337c861db00cc, 0x1c3d05775d0ee },
        { 0x6f1409422e51a, 0x7856bbece2d25, 0x13380a72f031c, 0x43e1080a7f3ba, 0x0621e2c7d3304 },
    },
    {
        { 0x61796b0dbf0f3, 0x73c2f9c32d6f5, 0x6aa8ed1537ebe, 0x74e92c91838f4, 0x5d8e589ca1002 },
        { 0x060cc8259838d, 0x038d3f35b95f3, 0x56078c243a923, 0x2de3293241bb2, 0x0007d6097bd3a },
        { 0x71d950842a94b, 0x46b11e5c7d817, 0x5478bbecb4f0d, 0x7c3054b0a1c5d, 0x1583d7783c1cb },
    },
    {
        { 0x34704cc9d28c7, 0x3dee598b1f200, 0x16e1c98746d9e, 0x4050b7095afdf, 0x4958064e83c55 },
        { 0x6a2ef5da27ae1, 0x28aace02e9d9d, 0x02459e965f0e8, 0x7b864d3150933, 0x252a5f2e81ed8 },
        { 0x094265066e80d, 0x0a60f918d61a5, 0x0444bf7f30fde, 0x1c40da9ed3c06, 0x079c170bd843b },
    },
    {
        { 0x6cd50c0d5d056, 0x5b7606ae779ba, 0x70fbd226bdda1, 0x5661e53391ff9, 0x6768c0d7317b8 },
        { 0x6ece464fa6fff, 0x3cc40bca460a0, 0x6e3a90afb8d0c, 0x5801abca11228, 0x6dec05e34ac9f },
        { 0x625e5f155c1b3, 0x4f32f6f723296, 0x5ac980105efce, 0x17a61165eee36, 0x51445e14ddcd5 },
    },
    {
        { 0x147ab2bbea455, 0x1f240f2253126, 0x0c3de9e314e89, 0x21ea5a4fca45f, 0x12e990086e4fd },
        { 0x02b4b3b144951, 0x5688977966aea, 0x18e176e399ffd, 0x2e45c5eb4938b, 0x13186f31e3929 },
        { 0x496b37fdfbb2e, 0x3c2439d5f3e21, 0x16e60fe7e6a4d, 0x4d7ef889b621d, 0x77b2e3f05d3e9 },
    },
},
{
    {
        { 0x0639c12ddb0a4, 0x6180490cd7ab3, 0x3f3918297467c, 0x74568be1781ac, 0x07a195152e095 },
        { 0x7a9c59c2ec4de, 0x7e9f09e79652d, 0x6a3e422f22d86, 0x2ae8e3b836c8b, 0x63b795fc7ad32 },
        { 0x68f02389e5fc8, 0x059f1bc877506, 0x504990e410cec, 0x09bd7d0feaee2, 0x3e8fe83d032f0 },
    },
    {
        { 0x04c8de8efd13c, 0x1c67c06e6210e, 0x183378f7f146a, 0x64352ceaed289, 0x22d60899a6258 },
        { 0x315b90570a294, 0x60ce108a925f1, 0x6eff61253c909, 0x003ef0e2d70b0, 0x75ba3b797fac4 },
        { 0x1dbc070cdd196, 0x16d8fb1534c47, 0x500498183fa2a, 0x72f59c423de75, 0x0904d07b87779 },
    },
    {
        { 0x22d6648f940b9, 0x197a5a1873e86, 0x207e4c41a54bc, 0x5360b3b4bd6d0, 0x6240aacebaf72 },
        { 0x61fd4ddba919c, 0x7d8e991b55699, 0x61b31473cc76c, 0x7039631e631d6, 0x43e2143fbc1dd },
        { 0x4749c5ba295a0, 0x37946fa4b5f06, 0x724c5ab5a51f1, 0x65633789dd3f3, 0x56bdaf238db40 },
    },
    {
        { 0x0d36cc19d3bb2, 0x6ec4470d72262, 0x6853d7018a9ae, 0x3aa3e4dc2c8eb, 0x03aa31507e1e5 },
        { 0x2b9e3f53533eb, 0x2add727a806c5, 0x56955c8ce15a3, 0x18c4f070a290e, 0x1d24a86d83741 },
        { 0x47648ffd4ce1f, 0x60a9591839e9d, 0x424d5f38117ab, 0x42cc46912c10e, 0x43b261dc9aeb4 },
    },
    {
        { 0x13d8b6c951364, 0x4c0017e8f632a, 0x53e559e53f9c4, 0x4b20146886eea, 0x02b4d5e242940 },
        { 0x31e1988bb79bb, 0x7b82f46b3bcab, 0x0f7a8ce827b41, 0x5e15816177130, 0x326055cf5b276 },
        { 0x155cb28d18df2, 0x0c30d9ca11694, 0x2090e27ab3119, 0x208624e7a49b6, 0x27a6c809ae5d3 },
    },
    {
        { 0x4270ac43d6954, 0x2ed4cd95659a5, 0x75c0db37528f9, 0x2ccbcfd2c9234, 0x221503603d8c2 },
        { 0x6ebcd1f0db188, 0x74ceb4b7d1174, 0x7d56168df4f5c, 0x0bf79176fd18a, 0x2cb67174ff60a },
        { 0x6cdf9390be1d0, 0x08e519c7e2b3d, 0x253c3d2a50881, 0x21b41448e333d, 0x7b1df4b73890f },
    },
    {
        { 0x6221807f8f58c, 0x3fa92813a8be5, 0x6da98c38d5572, 0x01ed95554468f, 0x68698245d352e },
        { 0x2f2e0b3b2a224, 0x0c56aa22c1c92, 0x5fdec39f1b278, 0x4c90af5c7f106, 0x61fcef2658fc5 },
        { 0x15d852a18187a, 0x270dbb59afb76, 0x7db120bcf92ab, 0x0e7a25d714087, 0x46cf4c473daf0 },
    },
    {
        { 0x46ea7f1498140, 0x70725690a8427, 0x0a73ae9f079fb, 0x2dd924461c62b, 0x1065aae50d8cc },
        { 0x525ed9ec4e5f9, 0x022d20660684c, 0x7972b70397b68, 0x7a03958d3f965, 0x29387bcd14eb5 },
        { 0x44525df200d57, 0x2d7f94ce94385, 0x60d00c170ecb7, 0x38b0503f3d8f0, 0x69a198e64f1ce },
    },
},
{
    {
        { 0x14434dcc5caed, 0x2c7909f667c20, 0x61a839d1fb576, 0x4f23800cabb76, 0x25b2697bd267f },
        { 0x2b2e0d91a78bc, 0x3990a12ccf20c, 0x141c2e11f2622, 0x0dfcefaa53320, 0x7369e6a92493a },
        { 0x73ffb13986864, 0x3282bb8f713ac, 0x49ced78f297ef, 0x6697027661def, 0x1420683db54e4 },
    },
    {
        { 0x6bb6fc1cc5ad0, 0x532c8d591669d, 0x1af794da86c33, 0x0e0e9d86d24d3, 0x31e83b4161d08 },
        { 0x0bd1e249dd197, 0x00bcb1820568f, 0x2eab1718830d4, 0x396fd816997e6, 0x60b63bebf508a },
        { 0x0c7129e062b4f, 0x1e526415b12fd, 0x461a0fd27923d, 0x18badf670a5b7, 0x55cf1eb62d550 },
    },
    {
        { 0x6b5e37df58c52, 0x3bcf33986c60e, 0x44fb8835ceae7, 0x099dec18e71a4, 0x1a56fbaa62ba0 },
        { 0x1101065c23d58, 0x5aa1290338b0f, 0x3157e9e2e7421, 0x0ea712017d489, 0x669a656457089 },
        { 0x66b505c9dc9ec, 0x774ef86e35287, 0x4d1d944c0955e, 0x52e4c39d72b20, 0x13c4836799c58 },
    },
    {
        { 0x4fb6a5d8bd080, 0x58ae34908589b, 0x3954d977baf13, 0x413ea597441dc, 0x50bdc87dc8e5b },
        { 0x25d465ab3e1b9, 0x0f8fe27ec2847, 0x2d6e6dbf04f06, 0x3038cfc1b3276, 0x66f80c93a637b },
        { 0x537836edfe111, 0x2be02357b2c0d, 0x6dcee58c8d4f8, 0x2d732581d6192, 0x1dd56444725fd },
    },
    {
        { 0x7e60008bac89a, 0x23d5c387c1852, 0x79e5df1f533a8, 0x2e6f9f1c5f0cf, 0x3a3a450f63a30 },
        { 0x47ff83362127d, 0x08e39af82b1f4, 0x488322ef27dab, 0x1973738a2a1a4, 0x0e645912219f7 },
        { 0x72f31d8394627, 0x07bd294a200f1, 0x665be00e274c6, 0x43de8f1b6368b, 0x318c8d9393a9a },
    },
    {
        { 0x69e29ab1dd398, 0x30685b3c76bac, 0x565cf37f24859, 0x57b2ac28efef9, 0x509a41c325950 },
        { 0x45d032afffe19, 0x12fe49b6cde4e, 0x21663bc327cf1, 0x18a5e4c69f1dd, 0x224c7c679a1d5 },
        { 0x06edca6f925e9, 0x68c8363e677b8, 0x60cfa25e4fbcf, 0x1c4c17609404e, 0x05bff02328a11 },
    },
    {
        { 0x1a0dd0dc512e4, 0x10894bf5fcd10, 0x52949013f9c37, 0x1f50fba4735c7, 0x576277cdee01a },
        { 0x2137023cae00b, 0x15a3599eb26c6, 0x0687221512b3c, 0x253cb3a0824e9, 0x780b8cc3fa2a4 },
        { 0x38abc234f305f, 0x7a280bbc103de, 0x398a836695dfe, 0x3d0af41528a1a, 0x5ff418726271b },
    },
    {
        { 0x347e813b69540, 0x76864c21c3cbb, 0x1e049dbcd74a8, 0x5b4d60f93749c, 0x29d4db8ca0a0c },
        { 0x6080c1789db9d, 0x4be7cef1ea731, 0x2f40d769d8080, 0x35f7d4c44a603, 0x106a03dc25a96 },
        { 0x50aaf333353d0, 0x4b59a613cbb35, 0x223dfc0e19a76, 0x77d1e2bb2c564, 0x4ab38a51052cb },
    },
},
{
    {
        { 0x7d1ef5fddc09c, 0x7beeaebb9dad9, 0x058d30ba0acfb, 0x5cd92eab5ae90, 0x3041c6bb04ed2 },
        { 0x42b256768d593, 0x2e88459427b4f, 0x02b3876630701, 0x34878d405eae5, 0x29cdd1adc088a },
        { 0x2f2f9d956e148, 0x6b3e6ad65c1fe, 0x5b00972b79e5d, 0x53d8d234c5daf, 0x104bbd6814049 },
    },
    {
        { 0x59a5fd67ff163, 0x3a998ead0352b, 0x083c95fa4af9a, 0x6fadbfc01266f, 0x204f2a20fb072 },
        { 0x0fd3168f1ed67, 0x1bb0de7784a3e, 0x34bcb78b20477, 0x0a4a26e2e2182, 0x5be8cc57092a7 },
        { 0x43b3d30ebb079, 0x357aca5c61902, 0x5b570c5d62455, 0x30fb29e1e18c7, 0x2570fb17c2791 },
    },
    {
        { 0x6a9550bb8245a, 0x511f20a1a2325, 0x29324d7239bee, 0x3343cc37516c4, 0x241c5f91de018 },
        { 0x2367f2cb61575, 0x6c39ac04d87df, 0x6d4958bd7e5bd, 0x566f4638a1532, 0x3dcb65ea53030 },
        { 0x0172940de6caa, 0x6045b2e67451b, 0x56c07463efcb3, 0x0728b6bfe6e91, 0x08420edd5fcdf },
    },
    {
        { 0x0c34e04f410ce, 0x344edc0d0a06b, 0x6e45486d84d6d, 0x44e2ecb3863f5, 0x04d654f321db8 },
        { 0x720ab8362fa4a, 0x29c4347cdd9bf, 0x0e798ad5f8463, 0x4fef18bcb0bfe, 0x0d9a53efbc176 },
        { 0x5c116ddbdb5d5, 0x6d1b4bba5abcf, 0x4d28a48a5537a, 0x56b8e5b040b99, 0x4a7a4f2618991 },
    },
    {
        { 0x3b291af372a4b, 0x60e3028fe4498, 0x2267bca4f6a09, 0x719eec242b243, 0x4a96314223e0e },
        { 0x718025fb15f95, 0x68d6b8371fe94, 0x3804448f7d97c, 0x42466fe784280, 0x11b50c4cddd31 },
        { 0x0274408a4ffd6, 0x7d382aedb34dd, 0x40acfc9ce385d, 0x628bb99a45b1e, 0x4f4bce4dce6bc },
    },
    {
        { 0x2616ec49d0b6f, 0x1f95d8462e61c, 0x1ad3e9b9159c6, 0x79ba475a04df9, 0x3042cee561595 },
        { 0x7ce5ae2242584, 0x2d25eb153d4e3, 0x3a8f3d09ba9c9, 0x0f3690d04eb8e, 0x73fcdd14b71c0 },
        { 0x67079449bac41, 0x5b79c4621484f, 0x61069f2156b8d, 0x0eb26573b10af, 0x389e740c9a9ce },
    },
    {
        { 0x578f6570eac28, 0x644f2339c3937, 0x66e47b7956c2c, 0x34832fe1f55d0, 0x25c425e5d6263 },
        { 0x4b3ae34dcb9ce, 0x47c691a15ac9f, 0x318e06e5d400c, 0x3c422d9f83eb1, 0x61545379465a6 },
        { 0x606a6f1d7de6e, 0x4f1c0c46107e7, 0x229b1dcfbe5d8, 0x3acc60a7b1327, 0x6539a08915484 },
    },
    {
        { 0x4dbd414bb4a19, 0x7930849f1dbb8, 0x329c5a466caf0, 0x6c824544feb9b, 0x0f65320ef019b },
        { 0x21f74c3d2f773, 0x024b88d08bd3a, 0x6e678cf054151, 0x43631272e747c, 0x11c5e4aac5cd1 },
        { 0x6d1b1cafde0c6, 0x462c76a303a90, 0x3ca4e693cff9b, 0x3952cd45786fd, 0x4cabc7bdec330 },
    },
},
{
    {
        { 0x7788f3f78d289, 0x5942809b3f811, 0x5973277f8c29c, 0x010f93bc5fe67, 0x7ee498165acb2 },
        { 0x69624089c0a2e, 0x0075fc8e70473, 0x13e84ab1d2313, 0x2c10bedf6953b, 0x639b93f0321c8 },
        { 0x508e39111a1c3, 0x290120e912f7a, 0x1cbf464acae43, 0x15373e9576157, 0x0edf493c85b60 },
    },
    {
        { 0x7c4d284764113, 0x7fefebf06acec, 0x39afb7a824100, 0x1b48e47e7fd65, 0x04c00c54d1dfa },
        { 0x48158599b5a68, 0x1fd75bc41d5d9, 0x2d9fc1fa95d3c, 0x7da27f20eba11, 0x403b92e3019d4 },
        { 0x22f818b465cf8, 0x342901dff09b8, 0x31f595dc683cd, 0x37a57745fd682, 0x355bb12ab2617 },
    },
    {
        { 0x1dac75a8c7318, 0x3b679d5423460, 0x6b8fcb7b6400e, 0x6c73783be5f9d, 0x7518eaf8e052a },
        { 0x664cc7493bbf4, 0x33d94761874e3, 0x0179e1796f613, 0x1890535e2867d, 0x0f9b8132182ec },
        { 0x059c41b7f6c32, 0x79e8706531491, 0x6c747643cb582, 0x2e20c0ad494e4, 0x47c3871bbb175 },
    },
    {
        { 0x65d50c85066b0, 0x6167453361f7c, 0x06ba3818bb312, 0x6aff29baa7522, 0x08fea02ce8d48 },
        { 0x4539771ec4f48, 0x7b9318badca28, 0x70f19afe016c5, 0x4ee7bb1608d23, 0x00b89b8576469 },
        { 0x5dd7668deead0, 0x4096d0ba47049, 0x6275997219114, 0x29bda8a67e6ae, 0x473829a74f75d },
    },
    {
        { 0x1533aad3902c9, 0x1dde06b11e47b, 0x784bed1930b77, 0x1c80a92b9c867, 0x6c668b4d44e4d },
        { 0x2da754679c418, 0x3164c31be105a, 0x11fac2b98ef5f, 0x35a1aaf779256, 0x2078684c4833c },
        { 0x0cf217a78820c, 0x65024e7d2e769, 0x23bb5efdda82a, 0x19fd4b632d3c6, 0x7411a6054f8a4 },
    },
    {
        { 0x2e53d18b175b4, 0x33e7254204af3, 0x3bcd7d5a1c4c5, 0x4c7c22af65d0f, 0x1ec9a872458c3 },
        { 0x59d32b99dc86d, 0x6ac075e22a9ac, 0x30b9220113371, 0x27fd9a638966e, 0x7c136574fb813 },
        { 0x6a4d400a2509b, 0x041791056971c, 0x655d5866e075c, 0x2302bf3e64df8, 0x3add88a5c7cd6 },
    },
    {
        { 0x298d459393046, 0x30bfecb3d90b8, 0x3d9b8ea3df8d6, 0x3900e96511579, 0x61ba1131a406a },
        { 0x15770b635dcf2, 0x59ecd83f79571, 0x2db461c0b7fbd, 0x73a42a981345f, 0x249929fccc879 },
        { 0x0a0f116959029, 0x5974fd7b1347a, 0x1e0cc1c08edad, 0x673bdf8ad1f13, 0x5620310cbbd8e },
    },
    {
        { 0x6b5f477e285d6, 0x4ed91ec326cc8, 0x6d6537503a3fd, 0x626d3763988d5, 0x7ec846f3658ce },
        { 0x193434934d643, 0x0d4a2445eaa51, 0x7d0708ae76fe0, 0x39847b6c3c7e1, 0x37676a2a4d9d9 },
        { 0x68f3f1da22ec7, 0x6ed8039a2736b, 0x2627ee04c3c75, 0x6ea90a647e7d1, 0x6daaf723399b9 },
    },
},
{
    {
        { 0x304bfacad8ea2, 0x502917d108b07, 0x043176ca6dd0f, 0x5d5158f2c1d84, 0x2b5449e58eb3b },
        { 0x27562eb3dbe47, 0x291d7b4170be7, 0x5d1ca67dfa8e1, 0x2a88061f298a2, 0x1304e9e71627d },
        { 0x014d26adc9cfe, 0x7f1691ba16f13, 0x5e71828f06eac, 0x349ed07f0fffc, 0x4468de2d7c2dd },
    },
    {
        { 0x2d8c6f86307ce, 0x6286ba1850973, 0x5e9dcb08444d4, 0x1a96a543362b2, 0x5da6427e63247 },
        { 0x3355e9419469e, 0x1847bb8ea8a37, 0x1fe6588cf9b71, 0x6b1c9d2db6b22, 0x6cce7c6ffb44b },
        { 0x4c688deac22ca, 0x6f775c3ff0352, 0x565603ee419bb, 0x6544456c61c46, 0x58f29abfe79f2 },
    },
    {
        { 0x264bf710ecdf6, 0x708c58527896b, 0x42ceae6c53394, 0x4381b21e82b6a, 0x6af93724185b4 },
        { 0x6cfab8de73e68, 0x3e6efced4bd21, 0x0056609500dbe, 0x71b7824ad85df, 0x577629c4a7f41 },
        { 0x0024509c6a888, 0x2696ab12e6644, 0x0cca27f4b80d8, 0x0c7c1f11b119e, 0x701f25bb0caec },
    },
    {
        { 0x0f6d97cbec113, 0x4ce97fb7c93a3, 0x139835a11281b, 0x728907ada9156, 0x720a5bc050955 },
        { 0x0b0f8e4616ced, 0x1d3c4b50fb875, 0x2f29673dc0198, 0x5f4b0f1830ffa, 0x2e0c92bfbdc40 },
        { 0x709439b805a35, 0x6ec48557f8187, 0x08a4d1ba13a2c, 0x076348a0bf9ae, 0x0e9b9cbb144ef },
    },
    {
        { 0x69bd55db1beee, 0x6e14e47f731bd, 0x1a35e47270eac, 0x66f225478df8e, 0x366d44191cfd3 },
        { 0x2d48ffb5720ad, 0x57b7f21a1df77, 0x5550effba0645, 0x5ec6a4098a931, 0x221104eb3f337 },
        { 0x41743f2bc8c14, 0x796b0ad8773c7, 0x29fee5cbb689b, 0x122665c178734, 0x4167a4e6bc593 },
    },
    {
        { 0x62665f8ce8fee, 0x29d101ac59857, 0x4d93bbba59ffc, 0x17b7897373f17, 0x34b33370cb7ed },
        { 0x39d2876f62700, 0x001cecd1d6c87, 0x7f01a11747675, 0x2350da5a18190, 0x7938bb7e22552 },
        { 0x591ee8681d6cc, 0x39db0b4ea79b8, 0x202220f380842, 0x2f276ba42e0ac, 0x1176fc6e2dfe6 },
    },
    {
        { 0x0e28949770eb8, 0x5559e88147b72, 0x35e1e6e63ef30, 0x35b109aa7ff6f, 0x1f6a3e54f2690 },
        { 0x76cd05b9c619b, 0x69654b0901695, 0x7a53710b77f27, 0x79a1ea7d28175, 0x08fc3a4c677d5 },
        { 0x4c199d30734ea, 0x6c622cb9acc14, 0x5660a55030216, 0x068f1199f11fb, 0x4f2fad0116b90 },
    },
    {
        { 0x4d91db73bb638, 0x55f82538112c5, 0x6d85a279815de, 0x740b7b0cd9cf9, 0x3451995f2944e },
        { 0x6b24194ae4e54, 0x2230afded8897, 0x23412617d5071, 0x3d5d30f35969b, 0x445484a4972ef },
        { 0x2fcd09fea7d7c, 0x296126b9ed22a, 0x4a171012a05b2, 0x1db92c74d5523, 0x10b89ca604289 },
    },
},
{
    {
        { 0x141be5a45f06e, 0x5adb38becaea7, 0x3fd46db41f2bb, 0x6d488bbb5ce39, 0x17d2d1d9ef0d4 },
        { 0x147499718289c, 0x0a48a67e4c7ab, 0x30fbc544bafe3, 0x0c701315fe58a, 0x20b878d577b75 },
        { 0x2af18073f3e6a, 0x33aea420d24fe, 0x298008bf4ff94, 0x3539171db961e, 0x72214f63cc65c },
    },
    {
        { 0x5b7b9f43b29c9, 0x149ea31eea3b3, 0x4be7713581609, 0x2d87960395e98, 0x1f24ac855a154 },
        { 0x37f405307a693, 0x2e5e66cf2b69c, 0x5d84266ae9c53, 0x5e4eb7de853b9, 0x5fdf48c58171c },
        { 0x608328e9505aa, 0x22182841dc49a, 0x3ec96891d2307, 0x2f363fff22e03, 0x00ba739e2ae39 },
    },
    {
        { 0x426f5ea88bb26, 0x33092e77f75c8, 0x1a53940d819e7, 0x1132e4f818613, 0x72297de7d518d },
        { 0x698de5c8790d6, 0x268b8545beb25, 0x6d2648b96fedf, 0x47988ad1db07c, 0x03283a3e67ad7 },
        { 0x41dc7be0cb939, 0x1b16c66100904, 0x0a24c20cbc66d, 0x4a2e9efe48681, 0x05e1296846271 },
    },
    {
        { 0x7bbc8242c4550, 0x59a06103b35b7, 0x7237e4af32033, 0x726421ab3537a, 0x78cf25d38258c },
        { 0x2eeb32d9c495a, 0x79e25772f9750, 0x6d747833bbf23, 0x6cdd816d5d749, 0x39c00c9c13698 },
        { 0x66b8e31489d68, 0x573857e10e2b5, 0x13be816aa1472, 0x41964d3ad4bf8, 0x006b52076b3ff },
    },
    {
        { 0x37e16b9ce082d, 0x1882f57853eb9, 0x7d29eacd01fc5, 0x2e76a59b5e715, 0x7de2e9561a9f7 },
        { 0x0cfe19d95781c, 0x312cc621c453c, 0x145ace6da077c, 0x0912bef9ce9b8, 0x4d57e3443bc76 },
        { 0x0d4f4b6a55ecb, 0x7ebb0bb733bce, 0x7ba6a05200549, 0x4f6ede4e22069, 0x6b2a90af1a602 },
    },
    {
        { 0x3f3245bb2d80a, 0x0e5f720f36efd, 0x3b9cccf60c06d, 0x084e323f37926, 0x465812c8276c2 },
        { 0x3f4fc9ae61e97, 0x3bc07ebfa2d24, 0x3b744b55cd4a0, 0x72553b25721f3, 0x5fd8f4e9d12d3 },
        { 0x3beb22a1062d9, 0x6a7063b82c9a8, 0x0a5a35dc197ed, 0x3c80c06a53def, 0x05b32c2b1cb16 },
    },
    {
        { 0x4a42c7ad58195, 0x5c8667e799eff, 0x02e5e74c850a1, 0x3f0db614e869a, 0x31771a4856730 },
        { 0x05eccd24da8fd, 0x580bbfdf07918, 0x7e73586873c6a, 0x74ceddf77f93e, 0x3b5556a37b471 },
        { 0x0c524e14dd482, 0x283457496c656, 0x0ad6bcfb6cd45, 0x375d1e8b02414, 0x4fc079d27a733 },
    },
    {
        { 0x48b440c86c50d, 0x139929cca3b86, 0x0f8f2e44cdf2f, 0x68432117ba6b2, 0x241170c2bae3c },
        { 0x138b089bf2f7f, 0x4a05bfd34ea39, 0x203914c925ef5, 0x7497fffe04e3c, 0x124567cecaf98 },
        { 0x1ab860ac473b4, 0x5c0227c86a7ff, 0x71b12bfc24477, 0x006a573a83075, 0x3f8612966c870 },
    },
},
{
    {
        { 0x0fcfa36048d13, 0x66e7133bbb383, 0x64b42a8a45676, 0x4ea6e4f9a85cf, 0x26f57eee878a1 },
        { 0x20cc9782a0dde, 0x65d4e3070aab3, 0x7bc8e31547736, 0x09ebfb1432d98, 0x504aa77679736 },
        { 0x32cd55687efb1, 0x4448f5e2f6195, 0x568919d460345, 0x034c2e0ad1a27, 0x4041943d9dba3 },
    },
    {
        { 0x17743a26caadd, 0x48c9156f9c964, 0x7ef278d1e9ad0, 0x00ce58ea7bd01, 0x12d931429800d },
        { 0x0eeba43ebcc96, 0x384dd5395f878, 0x1df331a35d272, 0x207ecfd4af70e, 0x1420a1d976843 },
        { 0x67799d337594f, 0x01647548f6018, 0x57fce5578f145, 0x009220c142a71, 0x1b4f92314359a },
    },
    {
        { 0x73030a49866b1, 0x2442be90b2679, 0x77bd3d8947dcf, 0x1fb55c1552028, 0x5ff191d56f9a2 },
        { 0x4109d89150951, 0x225bd2d2d47cb, 0x57cc080e73bea, 0x6d71075721fcb, 0x239b572a7f132 },
        { 0x6d433ac2d9068, 0x72bf930a47033, 0x64facf4a20ead, 0x365f7a2b9402a, 0x020c526a758f3 },
    },
    {
        { 0x1ef59f042cc89, 0x3b1c24976dd26, 0x31d665cb16272, 0x28656e470c557, 0x452cfe0a5602c },
        { 0x034f89ed8dbbc, 0x73b8f948d8ef3, 0x786c1d323caab, 0x43bd4a9266e51, 0x02aacc4615313 },
        { 0x0f7a0647877df, 0x4e1cc0f93f0d4, 0x7ec4726ef1190, 0x3bdd58bf512f8, 0x4cfb7d7b304b8 },
    },
    {
        { 0x699c29789ef12, 0x63beae321bc50, 0x325c340adbb35, 0x562e1a1e42bf6, 0x5b1d4cbc434d3 },
        { 0x43d6cb89b75fe, 0x3338d5b900e56, 0x38d327d531a53, 0x1b25c61d51b9f, 0x14b4622b39075 },
        { 0x32615cc0a9f26, 0x57711b99cb6df, 0x5a69c14e93c38, 0x6e88980a4c599, 0x2f98f71258592 },
    },
    {
        { 0x2ae444f54a701, 0x615397afbc5c2, 0x60d7783f3f8fb, 0x2aa675fc486ba, 0x1d8062e9e7614 },
        { 0x4a74cb50f9e56, 0x531d1c2640192, 0x0c03d9d6c7fd2, 0x57ccd156610c1, 0x3a6ae249d806a },
        { 0x2da85a9907c5a, 0x6b23721ec4caf, 0x4d2d3a4683aa2, 0x7f9c6870efdef, 0x298b8ce8aef25 },
    },
    {
        { 0x272ea0a2165de, 0x68179ef3ed06f, 0x4e2b9c0feac1e, 0x3ee290b1b63bb, 0x6ba6271803a7d },
        { 0x27953eff70cb2, 0x54f22ae0ec552, 0x29f3da92e2724, 0x242ca0c22bd18, 0x34b8a8404d5ce },
        { 0x6ecb583693335, 0x3ec76bfdfb84d, 0x2c895cf56a04f, 0x6355149d54d52, 0x71d62bdd465e1 },
    },
    {
        { 0x5b5dab1f75ef5, 0x1e2d60cbeb9a5, 0x527c2175dfe57, 0x59e8a2b8ff51f, 0x1c333621262b2 },
        { 0x3cc28d378df80, 0x72141f4968ca6, 0x407696bdb6d0d, 0x5d271b22ffcfb, 0x74d5f317f3172 },
        { 0x7e55467d9ca81, 0x6a5653186f50d, 0x6b188ece62df1, 0x4c66d36844971, 0x4aebcc4547e9d },
    },
},
{
    {
        { 0x08d9e7354b610, 0x26b750b6dc168, 0x162881e01acc9, 0x7966df31d01a5, 0x173bd9ddc9a1d },
        { 0x0071b276d01c9, 0x0b0d8918e025e, 0x75beea79ee2eb, 0x3c92984094db8, 0x5d88fbf95a3db },
        { 0x00f1efe5872df, 0x5da872318256a, 0x59ceb81635960, 0x18cf37693c764, 0x06e1cd13b19ea },
    },
    {
        { 0x3af629e5b0353, 0x204f1a088e8e5, 0x10efc9ceea82e, 0x589863c2fa34b, 0x7f3a6a1a8d837 },
        { 0x0ad516f166f23, 0x263f56d57c81a, 0x13422384638ca, 0x1331ff1af0a50, 0x3080603526e16 },
        { 0x644395d3d800b, 0x2b9203dbedefc, 0x4b18ce656a355, 0x03f3466bc182c, 0x30d0fded2e513 },
    },
    {
        { 0x4971e68b84750, 0x52ccc9779f396, 0x3e904ae8255c8, 0x4ecae46f39339, 0x4615084351c58 },
        { 0x14d1af21233b3, 0x1de1989b39c0b, 0x52669dc6f6f9e, 0x43434b28c3fc7, 0x0a9214202c099 },
        { 0x019c0aeb9a02e, 0x1a2c06995d792, 0x664cbb1571c44, 0x6ff0736fa80b2, 0x3bca0d2895ca5 },
    },
    {
        { 0x08eb69ecc01bf, 0x5b4c8912df38d, 0x5ea7f8bc2f20e, 0x120e516caafaf, 0x4ea8b4038df28 },
        { 0x031bc3c5d62a4, 0x7d9fe0f4c081e, 0x43ed51467f22c, 0x1e6cc0c1ed109, 0x5631deddae8f1 },
        { 0x5460af1cad202, 0x0b4919dd0655d, 0x7c4697d18c14c, 0x231c890bba2a4, 0x24ce0930542ca },
    },
    {
        { 0x7a155fdf30b85, 0x1c6c6e5d487f9, 0x24be1134bdc5a, 0x1405970326f32, 0x549928a7324f4 },
        { 0x090f5fd06c106, 0x6abb1021e43fd, 0x232bcfad711a0, 0x3a5c13c047f37, 0x41d4e3c28a06d },
        { 0x632a763ee1a2e, 0x6fa4bffbd5e4d, 0x5fd35a6ba4792, 0x7b55e1de99de8, 0x491b66dec0dcf },
    },
    {
        { 0x04a8ed0da64a1, 0x5ecfc45096ebe, 0x5edee93b488b2, 0x5b3c11a51bc8f, 0x4cf6b8b0b7018 },
        { 0x5b13dc7ea32a7, 0x18fc2db73131e, 0x7e3651f8f57e3, 0x25656055fa965, 0x08f338d0c85ee },
        { 0x3a821991a73bd, 0x03be6418f5870, 0x1ddc18eac9ef0, 0x54ce09e998dc2, 0x530d4a82eb078 },
    },
    {
        { 0x173456c9abf9e, 0x7892015100dad, 0x33ee14095fecb, 0x6ad95d67a0964, 0x0db3e7e00cbfb },
        { 0x43630e1f94825, 0x4d1956a6b4009, 0x213fe2df8b5e0, 0x05ce3a41191e6, 0x65ea753f10177 },
        { 0x6fc3ee2096363, 0x7ec36b96d67ac, 0x510ec6a0758b1, 0x0ed87df022109, 0x02a4ec1921e1a },
    },
    {
        { 0x06162f1cf795f, 0x324ddcafe5eb9, 0x018d5e0463218, 0x7e78b9092428e, 0x36d12b5dec067 },
        { 0x6259a3b24b8a2, 0x188b5f4170b9c, 0x681c0dee15deb, 0x4dfe665f37445, 0x3d143c5112780 },
        { 0x5279179154557, 0x39f8f0741424d, 0x45e6eb357923d, 0x42c9b5edb746f, 0x2ef517885ba82 },
    },
},
{
    {
        { 0x6bffb305b2f51, 0x5b112b2d712dd, 0x35774974fe4e2, 0x04af87a96e3a3, 0x57968290bb3a0 },
        { 0x7974e8c58aedc, 0x7757e083488c6, 0x601c62ae7bc8b, 0x45370c2ecab74, 0x2f1b78fab143a },
        { 0x2b8430a20e101, 0x1a49e1d88fee3, 0x38bbb47ce4d96, 0x1f0e7ba84d437, 0x7dc43e35dc2aa },
    },
    {
        { 0x02a5c273e9718, 0x32bc9dfb28b4f, 0x48df4f8d5db1a, 0x54c87976c028f, 0x044fb81d82d50 },
        { 0x66665887dd9c3, 0x629760a6ab0b2, 0x481e6c7243e6c, 0x097e37046fc77, 0x7ef72016758cc },
        { 0x718c5a907e3d9, 0x3b9c98c6b383b, 0x006ed255eccdc, 0x6976538229a59, 0x7f79823f9c30d },
    },
    {
        { 0x41ff068f587ba, 0x1c00a191bcd53, 0x7b56f9c209e25, 0x3781e5fccaabe, 0x64a9b0431c06d },
        { 0x4d239a3b513e8, 0x29723f51b1066, 0x642f4cf04d9c3, 0x4da095aa09b7a, 0x0a4e0373d784d },
        { 0x3d6a15b7d2919, 0x41aa75046a5d6, 0x691751ec2d3da, 0x23638ab6721c4, 0x071a7d0ace183 },
    },
    {
        { 0x4355220e14431, 0x0e1362a283981, 0x2757cd8359654, 0x2e9cd7ab10d90, 0x7c69bcf761775 },
        { 0x72daac887ba0b, 0x0b7f4ac5dda60, 0x3bdda2c0498a4, 0x74e67aa180160, 0x2c3bcc7146ea7 },
        { 0x0d7eb04e8295f, 0x4a5ea1e6fa0fe, 0x45e635c436c60, 0x28ef4a8d4d18b, 0x6f5a9a7322aca },
    },
    {
        { 0x1d4eba3d944be, 0x0100f15f3dce5, 0x61a700e367825, 0x5922292ab3d23, 0x02ab9680ee8d3 },
        { 0x1000c2f41c6c5, 0x0219fdf737174, 0x314727f127de7, 0x7e5277d23b81e, 0x494e21a2e147a },
        { 0x48a85dde50d9a, 0x1c1f734493df4, 0x47bdb64866889, 0x59a7d048f8eec, 0x6b5d76cbea46b },
    },
    {
        { 0x141171e782522, 0x6806d26da7c1f, 0x3f31d1bc79ab9, 0x09f20459f5168, 0x16fb869c03dd3 },
        { 0x7556cec0cd994, 0x5eb9a03b7510a, 0x50ad1dd91cb71, 0x1aa5780b48a47, 0x0ae333f685277 },
        { 0x6199733b60962, 0x69b157c266511, 0x64740f893f1ca, 0x03aa408fbf684, 0x3f81e38b8f70d },
    },
    {
        { 0x37f355f17c824, 0x07ae85334815b, 0x7e3abddd2e48f, 0x61eeabe1f45e5, 0x0ad3e2d34cded },
        { 0x10fcc7ed9affe, 0x4248cb0e96ff2, 0x4311c115172e2, 0x4c9d41cbf6925, 0x50510fc104f50 },
        { 0x40fc5336e249d, 0x3386639fb2de1, 0x7bbf871d17b78, 0x75f796b7e8004, 0x127c158bf0fa1 },
    },
    {
        { 0x28fc4ae51b974, 0x26e89bfd2dbd4, 0x4e122a07665cf, 0x7cab1203405c3, 0x4ed82479d167d },
        { 0x17c422e9879a2, 0x28a5946c8fec3, 0x53ab32e912b77, 0x7b44da09fe0a5, 0x354ef87d07ef4 },
        { 0x3b52260c5d975, 0x79d6836171fdc, 0x7d994f140d4bb, 0x1b6c404561854, 0x302d92d205392 },
    },
},
{
    {
        { 0x46fb6e4e0f177, 0x53497ad5265b7, 0x1ebdba01386fc, 0x0302f0cb36a3c, 0x0edc5f5eb426d },
        { 0x3c1a2bca4283d, 0x23430c7bb2f02, 0x1a3ea1bb58bc2, 0x7265763de5c61, 0x10e5d3b76f1ca },
        { 0x3bfd653da8e67, 0x584953ec82a8a, 0x55e288fa7707b, 0x5395fc3931d81, 0x45b46c51361cb },
    },
    {
        { 0x54ddd8a7fe3e4, 0x2cecc41c619d3, 0x43a6562ac4d91, 0x4efa5aca7bdd9, 0x5c1c0aef32122 },
        { 0x02abf314f7fa1, 0x391d19e8a1528, 0x6a2fa13895fc7, 0x09d8eddeaa591, 0x2177bfa36dcb7 },
        { 0x01bbcfa79db8f, 0x3d84beb3666e1, 0x20c921d812204, 0x2dd843d3b32ce, 0x4ae619387d8ab },
    },
    {
        { 0x17e44985bfb83, 0x54e32c626cc22, 0x096412ff38118, 0x6b241d61a246a, 0x75685abe5ba43 },
        { 0x3f6aa5344a32e, 0x69683680f11bb, 0x04c3581f623aa, 0x701af5875cba5, 0x1a00d91b17bf3 },
        { 0x60933eb61f2b2, 0x5193fe92a4dd2, 0x3d995a550f43e, 0x3556fb93a883d, 0x135529b623b0e },
    },
    {
        { 0x716bce22e83fe, 0x33d0130b83eb8, 0x0952abad0afac, 0x309f64ed31b8a, 0x5972ea051590a },
        { 0x0dbd7add1d518, 0x119f823e2231e, 0x451d66e5e7de2, 0x500c39970f838, 0x79b5b81a65ca3 },
        { 0x4ac20dc8f7811, 0x29589a9f501fa, 0x4d810d26a6b4a, 0x5ede00d96b259, 0x4f7e9c95905f3 },
    },
    {
        { 0x0443d355299fe, 0x39b7d7d5aee39, 0x692519a2f34ec, 0x6e4404924cf78, 0x1942eec4a144a },
        { 0x74bbc5781302e, 0x73135bb81ec4c, 0x7ef671b61483c, 0x7264614ccd729, 0x31993ad92e638 },
        { 0x45319ae234992, 0x2219d47d24fb5, 0x4f04488b06cf6, 0x53aaa9e724a12, 0x2a0a65314ef9c },
    },
    {
        { 0x61acd3c1c793a, 0x58b46b78779e6, 0x3369aacbe7af2, 0x509b0743074d4, 0x055dc39b6dea1 },
        { 0x7937ff7f927c2, 0x0c2fa14c6a5b6, 0x556bddb6dd07c, 0x6f6acc179d108, 0x4cf6e218647c2 },
        { 0x1227cc28d5bb6, 0x78ee9bff57623, 0x28cb2241f893a, 0x25b541e3c6772, 0x121a307710aa2 },
    },
    {
        { 0x1713ec77483c9, 0x6f70572d5facb, 0x25ef34e22ff81, 0x54d944f141188, 0x527bb94a6ced3 },
        { 0x35d5e9f034a97, 0x126069785bc9b, 0x5474ec7854ff0, 0x296a302a348ca, 0x333fc76c7a40e },
        { 0x5992a995b482e, 0x78dc707002ac7, 0x5936394d01741, 0x4fba4281aef17, 0x6b89069b20a7a },
    },
    {
        { 0x2fa8cb5c7db77, 0x718e6982aa810, 0x39e95f81a1a1b, 0x5e794f3646cfb, 0x0473d308a7639 },
        { 0x2a0416270220d, 0x75f248b69d025, 0x1cbbc16656a27, 0x5b9ffd6e26728, 0x23bc2103aa73e },
        { 0x6792603589e05, 0x248db9892595d, 0x006a53cad2d08, 0x20d0150f7ba73, 0x102f73bfde043 },
    },
},
{
    {
        { 0x4dae0b5511c9a, 0x5257fffe0d456, 0x54108d1eb2180, 0x096cc0f9baefa, 0x3f6bd725da4ea },
        { 0x0b9ab7f5745c6, 0x5caf0f8d21d63, 0x7debea408ea2b, 0x09edb93896d16, 0x36597d25ea5c0 },
        { 0x58d7b106058ac, 0x3cdf8d20bee69, 0x00a4cb765015e, 0x36832337c7cc9, 0x7b7ecc19da60d },
    },
    {
        { 0x64a51a77cfa9b, 0x29cf470ca0db5, 0x4b60b6e0898d9, 0x55d04ddffe6c7, 0x03bedc661bf5c },
        { 0x2373c695c690d, 0x4c0c8520dcf18, 0x384af4b7494b9, 0x4ab4a8ea22225, 0x4235ad7601743 },
        { 0x0cb0d078975f5, 0x292313e530c4b, 0x38dbb9124a509, 0x350d0655a11f1, 0x0e7ce2b0cdf06 },
    },
    {
        { 0x6fedfd94b70f9, 0x2383f9745bfd4, 0x4beae27c4c301, 0x75aa4416a3f3f, 0x615256138aece },
        { 0x4643ac48c85a3, 0x6878c2735b892, 0x3a53523f4d877, 0x3a504ed8bee9d, 0x666e0a5d8fb46 },
        { 0x3f64e4870cb0d, 0x61548b16d6557, 0x7a261773596f3, 0x7724d5f275d3a, 0x7f0bc810d514d },
    },
    {
        { 0x49dad737213a0, 0x745dee5d31075, 0x7b1a55e7fdbe2, 0x5ba988f176ea1, 0x1d3a907ddec5a },
        { 0x06ba426f4136f, 0x3cafc0606b720, 0x518f0a2359cda, 0x5fae5e46feca7, 0x0d1f8dbcf8eed },
        { 0x693313ed081dc, 0x5b0a366901742, 0x40c872ca4ca7e, 0x6f18094009e01, 0x00011b44a31bf },
    },
    {
        { 0x61f696a0aa75c, 0x38b0a57ad42ca, 0x1e59ab706fdc9, 0x01308d46ebfcd, 0x63d988a2d2851 },
        { 0x7a06c3fc66c0c, 0x1c9bac1ba47fb, 0x23935c575038e, 0x3f0bd71c59c13, 0x3ac48d916e835 },
        { 0x20753afbd232e, 0x71fbb1ed06002, 0x39cae47a4af3a, 0x0337c0b34d9c2, 0x33fad52b2368a },
    },
    {
        { 0x4c8d0c422cfe8, 0x760b4275971a5, 0x3da95bc1cad3d, 0x0f151ff5b7376, 0x3cc355ccb90a7 },
        { 0x649c6c5e41e16, 0x60667eee6aa80, 0x4179d182be190, 0x653d9567e6979, 0x16c0f429a256d },
        { 0x69443903e9131, 0x16f4ac6f9dd36, 0x2ea4912e29253, 0x2b4643e68d25d, 0x631eaf426bae7 },
    },
    {
        { 0x175b9a3700de8, 0x77c5f00aa48fb, 0x3917785ca0317, 0x05aa9b2c79399, 0x431f2c7f665f8 },
        { 0x10410da66fe9f, 0x24d82dcb4d67d, 0x3e6fe0e17752d, 0x4dade1ecbb08f, 0x5599648b1ea91 },
        { 0x26344858f7b19, 0x5f43d4a295ac0, 0x242a75c52acd4, 0x5934480220d10, 0x7b04715f91253 },
    },
    {
        { 0x6c280c4e6bac6, 0x3ada3b361766e, 0x42fe5125c3b4f, 0x111d84d4aac22, 0x48d0acfa57cde },
        { 0x5bd28acf6ae43, 0x16fab8f56907d, 0x7acb11218d5f2, 0x41fe02023b4db, 0x59b37bf5c2f65 },
        { 0x726e47dabe671, 0x2ec45e746f6c1, 0x6580e53c74686, 0x5eda104673f74, 0x16234191336d3 },
    },
},
{
    {
        { 0x19cd61ff38640, 0x060c6c4b41ba9, 0x75cf70ca7366f, 0x118a8f16c011e, 0x4a25707a203b9 },
        { 0x499def6267ff6, 0x76e858108773c, 0x693cac5ddcb29, 0x00311d00a9ff4, 0x2cdfdfecd5d05 },
        { 0x7668a53f6ed6a, 0x303ba2e142556, 0x3880584c10909, 0x4fe20000a261d, 0x5721896d248e4 },
    },
    {
        { 0x55091a1d0da4e, 0x4f6bfc7c1050b, 0x64e4ecd2ea9be, 0x07eb1f28bbe70, 0x03c935afc4b03 },
        { 0x65517fd181bae, 0x3e5772c76816d, 0x019189640898a, 0x1ed2a84de7499, 0x578edd74f63c1 },
        { 0x276c6492b0c3d, 0x09bfc40bf932e, 0x588e8f11f330b, 0x3d16e694dc26e, 0x3ec2ab590288c },
    },
    {
        { 0x13a09ae32d1cb, 0x3e81eb85ab4e4, 0x07aaca43cae1f, 0x62f05d7526374, 0x0e1bf66c6adba },
        { 0x0d27be4d87bb9, 0x56c27235db434, 0x72e6e0ea62d37, 0x5674cd06ee839, 0x2dd5c25a200fc },
        { 0x3d5e9792c887e, 0x319724dabbc55, 0x2b97c78680800, 0x7afdfdd34e6dd, 0x730548b35ae88 },
    },
    {
        { 0x3094ba1d6e334, 0x6e126a7e3300b, 0x089c0aefcfbc5, 0x2eea11f836583, 0x585a2277d8784 },
        { 0x551a3cba8b8ee, 0x3b6422be2d886, 0x630e1419689bc, 0x4653b07a7a955, 0x3043443b411db },
        { 0x25f8233d48962, 0x6bd8f04aff431, 0x4f907fd9a6312, 0x40fd3c737d29b, 0x7656278950ef9 },
    },
    {
        { 0x073a3ea86cf9d, 0x6e0e2abfb9c2e, 0x60e2a38ea33ee, 0x30b2429f3fe18, 0x28bbf484b613f },
        { 0x3cf59d51fc8c0, 0x7a0a0d6de4718, 0x55c3a3e6fb74b, 0x353135f884fd5, 0x3f4160a8c1b84 },
        { 0x12f5c6f136c7c, 0x0fedba237de4c, 0x779bccebfab44, 0x3aea93f4d6909, 0x1e79cb358188f },
    },
    {
        { 0x153d8f5e08181, 0x08533bbdb2efd, 0x1149796129431, 0x17a6e36168643, 0x478ab52d39d1f },
        { 0x436c3eef7e3f1, 0x7ffd3c21f0026, 0x3e77bf20a2da9, 0x418bffc8472de, 0x65d7951b3a3b3 },
        { 0x6a4d39252d159, 0x790e35900ecd4, 0x30725bf977786, 0x10a5c1635a053, 0x16d87a411a212 },
    },
    {
        { 0x4d5e2d54e0583, 0x2e5d7b33f5f74, 0x3a5de3f887ebf, 0x6ef24bd6139b7, 0x1f990b577a5a6 },
        { 0x57e5a42066215, 0x1a18b44983677, 0x3e652de1e6f8f, 0x6532be02ed8eb, 0x28f87c8165f38 },
        { 0x44ead1be8f7d6, 0x5759d4f31f466, 0x0378149f47943, 0x69f3be32b4f29, 0x45882fe1534d6 },
    },
    {
        { 0x49929943c6fe4, 0x4347072545b15, 0x3226bced7e7c5, 0x03a134ced89df, 0x7dcf843ce405f },
        { 0x1345d757983d6, 0x222f54234cccd, 0x1784a3d8adbb4, 0x36ebeee8c2bcc, 0x688fe5b8f626f },
        { 0x0d6484a4732c0, 0x7b94ac6532d92, 0x5771b8754850f, 0x48dd9df1461c8, 0x6739687e73271 },
    },
},
{
    {
        { 0x5cc9dc80c1ac0, 0x683671486d4cd, 0x76f5f1a5e8173, 0x6d5d3f5f9df4a, 0x7da0b8f68d7e7 },
        { 0x02014385675a6, 0x6155fb53d1def, 0x37ea32e89927c, 0x059a668f5a82e, 0x46115aba1d4dc },
        { 0x71953c3b5da76, 0x6642233d37a81, 0x2c9658076b1bd, 0x5a581e63010ff, 0x5a5f887e83674 },
    },
    {
        { 0x628d3a0a643b9, 0x01cd8640c93d2, 0x0b7b0cad70f2c, 0x3864da98144be, 0x43e37ae2d5d1c },
        { 0x301cf70a13d11, 0x2a6a1ba1891ec, 0x2f291fb3f3ae0, 0x21a7b814bea52, 0x3669b656e44d1 },
        { 0x63f06eda6e133, 0x233342758070f, 0x098e0459cc075, 0x4df5ead6c7c1b, 0x6a21e6cd4fd5e },
    },
    {
        { 0x129126699b2e3, 0x0ee11a2603de8, 0x60ac2f5c74c21, 0x59b192a196808, 0x45371b07001e8 },
        { 0x6170a3046e65f, 0x5401a46a49e38, 0x20add5561c4a8, 0x7abb4edde9e46, 0x586bf9f1a195f },
        { 0x3088d5ef8790b, 0x38c2126fcb4db, 0x685bae149e3c3, 0x0bcd601a4e930, 0x0eafb03790e52 },
    },
    {
        { 0x0805e0f75ae1d, 0x464cc59860a28, 0x248e5b7b00bef, 0x5d99675ef8f75, 0x44ae3344c5435 },
        { 0x555c13748042f, 0x4d041754232c0, 0x521b430866907, 0x3308e40fb9c39, 0x309acc675a02c },
        { 0x289b9bba543ee, 0x3ab592e28539e, 0x64d82abcdd83a, 0x3c78ec172e327, 0x62d5221b7f946 },
    },
    {
        { 0x5d4263af77a3c, 0x23fdd2289aeb0, 0x7dc64f77eb9ec, 0x01bd28338402c, 0x14f29a5383922 },
        { 0x4299c18d0936d, 0x5914183418a49, 0x52a18c721aed5, 0x2b151ba82976d, 0x5c0efde4bc754 },
        { 0x17edc25b2d7f5, 0x37336a6081bee, 0x7b5318887e5c3, 0x49f6d491a5be1, 0x5e72365c7bee0 },
    },
    {
        { 0x339062f08b33e, 0x4bbf3e657cfb2, 0x67af7f56e5967, 0x4dbd67f9ed68f, 0x70b20555cb734 },
        { 0x3fc074571217f, 0x3a0d29b2b6aeb, 0x06478ccdde59d, 0x55e4d051bddfa, 0x77f1104c47b4e },
        { 0x113c555112c4c, 0x7535103f9b7ca, 0x140ed1d9a2108, 0x02522333bc2af, 0x0e34398f4a064 },
    },
    {
        { 0x30b093e4b1928, 0x1ce7e7ec80312, 0x4e575bdf78f84, 0x61f7a190bed39, 0x6f8aded6ca379 },
        { 0x522d93ecebde8, 0x024f045e0f6cf, 0x16db63426cfa1, 0x1b93a1fd30fd8, 0x5e5405368a362 },
        { 0x0123dfdb7b29a, 0x4344356523c68, 0x79a527921ee5f, 0x74bfccb3e817e, 0x780de72ec8d3d },
    },
    {
        { 0x7eaf300f42772, 0x5455188354ce3, 0x4dcca4a3dcbac, 0x3d314d0bfebcb, 0x1defc6ad32b58 },
        { 0x28545089ae7bc, 0x1e38fe9a0c15c, 0x12046e0e2377b, 0x6721c560aa885, 0x0eb28bf671928 },
        { 0x3be1aef5195a7, 0x6f22f62bdb5eb, 0x39768b8523049, 0x43394c8fbfdbd, 0x467d201bf8dd2 },
    },
},
{
    {
        { 0x6f4bd567ae7a9, 0x65ac89317b783, 0x07d3b20fd8932, 0x000f208326916, 0x2ef9c5a5ba384 },
        { 0x6919a74ef4fad, 0x59ed4611452bf, 0x691ec04ea09ef, 0x3cbcb2700e984, 0x71c43c4f5ba3c },
        { 0x56df6fa9e74cd, 0x79c95e4cf56df, 0x7be643bc609e2, 0x149c12ad9e878, 0x5a758ca390c5f },
    },
    {
        { 0x0918b1d61dc94, 0x0d350260cd19c, 0x7a2ab4e37b4d9, 0x21fea735414d7, 0x0a738027f639d },
        { 0x72710d9462495, 0x25aafaa007456, 0x2d21f28eaa31b, 0x17671ea005fd0, 0x2dbae244b3eb7 },
        { 0x74a2f57ffe1cc, 0x1bc3073087301, 0x7ec57f4019c34, 0x34e082e1fa524, 0x2698ca635126a },
    },
    {
        { 0x5702f5e3dd90e, 0x31c9a4a70c5c7, 0x136a5aa78fc24, 0x1992f3b9f7b01, 0x3c004b0c4afa3 },
        { 0x5318832b0ba78, 0x6f24b9ff17cec, 0x0a47f30e060c7, 0x58384540dc8d0, 0x1fb43dcc49cae },
        { 0x146ac06f4b82b, 0x4b500d89e7355, 0x3351e1c728a12, 0x10b9f69932fe3, 0x6b43fd01cd1fd },
    },
    {
        { 0x742583e760ef3, 0x73dc1573216b8, 0x4ae48fdd7714a, 0x4f85f8a13e103, 0x73420b2d6ff0d },
        { 0x75d4b4697c544, 0x11be1fff7f8f4, 0x119e16857f7e1, 0x38a14345cf5d5, 0x5a68d7105b52f },
        { 0x4f6cb9e851e06, 0x278c4471895e5, 0x7efcdce3d64e4, 0x64f6d455c4b4c, 0x3db5632fea34b },
    },
    {
        { 0x190b1829825d5, 0x0e7d3513225c9, 0x1c12be3b7abae, 0x58777781e9ca6, 0x59197ea495df2 },
        { 0x6ee2bf75dd9d8, 0x6c72ceb34be8d, 0x679c9cc345ec7, 0x7898df96898a4, 0x04321adf49d75 },
        { 0x16019e4e55aae, 0x74fc5f25d209c, 0x4566a939ded0d, 0x66063e716e0b7, 0x45eafdc1f4d70 },
    },
    {
        { 0x64624cfccb1ed, 0x257ab8072b6c1, 0x0120725676f0a, 0x4a018d04e8eee, 0x3f73ceea5d56d },
        { 0x401858045d72b, 0x459e5e0ca2d30, 0x488b719308bea, 0x56f4a0d1b32b5, 0x5a5eebc80362d },
        { 0x7bfd10a4e8dc6, 0x7c899366736f4, 0x55ebbeaf95c01, 0x46db060903f8a, 0x2605889126621 },
    },
    {
        { 0x18e3cc676e542, 0x26079d995a990, 0x04a7c217908b2, 0x1dc7603e6655a, 0x0dedfa10b2444 },
        { 0x704a68360ff04, 0x3cecc3cde8b3e, 0x21cd5470f64ff, 0x6abc18d953989, 0x54ad0c2e4e615 },
        { 0x367d5b82b522a, 0x0d3f4b83d7dc7, 0x3067f4cdbc58d, 0x20452da697937, 0x62ecb2baa77a9 },
    },
    {
        { 0x72836afb62874, 0x0af3c2094b240, 0x0c285297f357a, 0x7cc2d5680d6e3, 0x61913d5075663 },
        { 0x5795261152b3d, 0x7a1dbbafa3cbd, 0x5ad31c52588d5, 0x45f3a4164685c, 0x2e59f919a966d },
        { 0x62d361a3231da, 0x65284004e01b8, 0x656533be91d60, 0x6ae016c00a89f, 0x3ddbc2a131c05 },
    },
},
{
    {
        { 0x257a22796bb14, 0x6f360fb443e75, 0x680e47220eaea, 0x2fcf2a5f10c18, 0x5ee7fb38d8320 },
        { 0x40ff9ce5ec54b, 0x57185e261b35b, 0x3e254540e70a9, 0x1b5814003e3f8, 0x78968314ac04b },
        { 0x5fdcb41446a8e, 0x5286926ff2a71, 0x0f231e296b3f6, 0x684a357c84693, 0x61d0633c9bca0 },
    },
    {
        { 0x328bcf8fc73df, 0x3b4de06ff95b4, 0x30aa427ba11a5, 0x5ee31bfda6d9c, 0x5b23ac2df8067 },
        { 0x44935ffdb2566, 0x12f016d176c6e, 0x4fbb00f16f5ae, 0x3fab78d99402a, 0x6e965fd847aed },
        { 0x2b953ee80527b, 0x55f5bcdb1b35a, 0x43a0b3fa23c66, 0x76e07388b820a, 0x79b9bbb9dd95d },
    },
    {
        { 0x17dae8e9f7374, 0x719f76102da33, 0x5117c2a80ca8b, 0x41a66b65d0936, 0x1ba811460accb },
        { 0x355406a3126c2, 0x50d1918727d76, 0x6e5ea0b498e0e, 0x0a3b6063214f2, 0x5065f158c9fd2 },
        { 0x169fb0c429954, 0x59aedd9ecee10, 0x39916eb851802, 0x57917555cc538, 0x3981f39e58a4f },
    },
    {
        { 0x5dfa56de66fde, 0x0058809075908, 0x6d3d8cb854a94, 0x5b2f4e970b1e3, 0x30f4452edcbc1 },
        { 0x38a7559230a93, 0x52c1cde8ba31f, 0x2a4f2d4745a3d, 0x07e9d42d4a28a, 0x38dc083705acd },
        { 0x52782c5759740, 0x53f3397d990ad, 0x3a939c7e84d15, 0x234c4227e39e0, 0x632d9a1a593f2 },
    },
    {
        { 0x1fd11ed0c84a7, 0x021b3ed2757e1, 0x73e1de58fc1c6, 0x5d110c84616ab, 0x3a5a7df28af64 },
        { 0x36b15b807cba6, 0x3f78a9e1afed7, 0x0a59c2c608f1f, 0x52bdd8ecb81b7, 0x0b24f48847ed4 },
        { 0x2d4be511beac7, 0x6bda4d99e5b9b, 0x17e6996914e01, 0x7b1f0ce7fcf80, 0x34fcf74475481 },
    },
    {
        { 0x31dab78cfaa98, 0x4e3216e5e54b7, 0x249823973b689, 0x2584984e48885, 0x0119a3042fb37 },
        { 0x7e04c789767ca, 0x1671b28cfb832, 0x7e57ea2e1c537, 0x1fbaaef444141, 0x3d3bdc164dfa6 },
        { 0x2d89ce8c2177d, 0x6cd12ba182cf4, 0x20a8ac19a7697, 0x539fab2cc72d9, 0x56c088f1ede20 },
    },
    {
        { 0x35fac24f38f02, 0x7d75c6197ab03, 0x33e4bc2a42fa7, 0x1c7cd10b48145, 0x038b7ea483590 },
        { 0x53d1110a86e17, 0x6416eb65f466d, 0x41ca6235fce20, 0x5c3fc8a99bb12, 0x09674c6b99108 },
        { 0x6f82199316ff8, 0x05d54f1a9f3e9, 0x3bcc5d0bd274a, 0x5b284b8d2d5ad, 0x6e5e31025969e },
    },
    {
        { 0x4fb0e63066222, 0x130f59747e660, 0x041868fecd41a, 0x3105e8c923bc6, 0x3058ad43d1838 },
        { 0x462f587e593fb, 0x3d94ba7ce362d, 0x330f9b52667b7, 0x5d45a48e0f00a, 0x08f5114789a8d },
        { 0x40ffde57663d0, 0x71445d4c20647, 0x2653e68170f7c, 0x64cdee3c55ed6, 0x26549fa4efe3d },
    },
},
{
    {
        { 0x68549af3f666e, 0x09e2941d4bb68, 0x2e8311f5dff3c, 0x6429ef91ffbd2, 0x3a10dfe132ce3 },
        { 0x55a461e6bf9d6, 0x78eeef4b02e83, 0x1d34f648c16cf, 0x07fea2aba5132, 0x1926e1dc6401e },
        { 0x74e8aea17cea0, 0x0c743f83fbc0f, 0x7cb03c4bf5455, 0x68a8ba9917e98, 0x1fa1d01d861e5 },
    },
    {
        { 0x4ac00d1df94ab, 0x3ba2101bd271b, 0x7578988b9c4af, 0x0f2bf89f49f7e, 0x73fced18ee9a0 },
        { 0x055947d599832, 0x346fe2aa41990, 0x0164c8079195b, 0x799ccfb7bba27, 0x773563bc6a75c },
        { 0x1e90863139cb3, 0x4f8b407d9a0d6, 0x58e24ca924f69, 0x7a246bbe76456, 0x1f426b701b864 },
    },
    {
        { 0x635c891a12552, 0x26aebd38ede2f, 0x66dc8faddae05, 0x21c7d41a03786, 0x0b76bb1b3fa7e },
        { 0x1264c41911c01, 0x702f44584bdf9, 0x43c511fc68ede, 0x0482c3aed35f9, 0x4e1af5271d31b },
        { 0x0c1f97f92939b, 0x17a88956dc117, 0x6ee005ef99dc7, 0x4aa9172b231cc, 0x7b6dd61eb772a },
    },
    {
        { 0x0abf9ab01d2c7, 0x3880287630ae6, 0x32eca045beddb, 0x57f43365f32d0, 0x53fa9b659bff6 },
        { 0x5c1e850f33d92, 0x1ec119ab9f6f5, 0x7f16f6de663e9, 0x7a7d6cb16dec6, 0x703e9bceaf1d2 },
        { 0x4c8e994885455, 0x4ccb5da9cad82, 0x3596bc610e975, 0x7a80c0ddb9f5e, 0x398d93e5c4c61 },
    },
    {
        { 0x77c60d2e7e3f2, 0x4061051763870, 0x67bc4e0ecd2aa, 0x2bb941f1373b9, 0x699c9c9002c30 },
        { 0x3d16733e248f3, 0x0e2b7e14be389, 0x42c0ddaf6784a, 0x589ea1fc67850, 0x53b09b5ddf191 },
        { 0x6a7235946f1cc, 0x6b99cbb2fbe60, 0x6d3a5d6485c62, 0x4839466e923c0, 0x51caf30c6fcdd },
    },
    {
        { 0x2f99a18ac54c7, 0x398a39661ee6f, 0x384331e40cde3, 0x4cd15c4de19a6, 0x12ae29c189f8e },
        { 0x3a7427674e00a, 0x6142f4f7e74c1, 0x4cc93318c3a15, 0x6d51bac2b1ee7, 0x5504aa292383f },
        { 0x6c0cb1f0d01cf, 0x187469ef5d533, 0x27138883747bf, 0x2f52ae53a90e8, 0x5fd14fe958eba },
    },
    {
        { 0x2fe5ebf93cb8e, 0x226da8acbe788, 0x10883a2fb7ea1, 0x094707842cf44, 0x7dd73f960725d },
        { 0x42ddf2845ab2c, 0x6214ffd3276bb, 0x00b8d181a5246, 0x268a6d579eb20, 0x093ff26e58647 },
        { 0x524fe68059829, 0x65b75e47cb621, 0x15eb0a5d5cc19, 0x05209b3929d5a, 0x2f59bcbc86b47 },
    },
    {
        { 0x1d560b691c301, 0x7f5bafce3ce08, 0x4cd561614806c, 0x4588b6170b188, 0x2aa55e3d01082 },
        { 0x47d429917135f, 0x3eacfa07af070, 0x1deab46b46e44, 0x7a53f3ba46cdf, 0x5458b42e2e51a },
        { 0x192e60c07444f, 0x5ae8843a21daa, 0x6d721910b1538, 0x3321a95a6417e, 0x13e9004a8a768 },
    },
},
{
    {
        { 0x600c9193b877f, 0x21c1b8a0d7765, 0x379927fb38ea2, 0x70d7679dbe01b, 0x5f46040898de9 },
        { 0x58845832fcedb, 0x135cd7f0c6e73, 0x53ffbdfe8e35b, 0x22f195e06e55b, 0x73937e8814bce },
        { 0x37116297bf48d, 0x45a9e0d069720, 0x25af71aa744ec, 0x41af0cb8aaba3, 0x2cf8a4e891d5e },
    },
    {
        { 0x5487e17d06ba2, 0x3872a032d6596, 0x65e28c09348e0, 0x27b6bb2ce40c2, 0x7a6f7f2891d6a },
        { 0x3fd8707110f67, 0x26f8716a92db2, 0x1cdaa1b753027, 0x504be58b52661, 0x2049bd6e58252 },
        { 0x1fd8d6a9aef49, 0x7cb67b7216fa1, 0x67aff53c3b982, 0x20ea610da9628, 0x6011aadfc5459 },
    },
    {
        { 0x6d0c802cbf890, 0x141bfed554c7b, 0x6dbb667ef4263, 0x58f3126857edc, 0x69ce18b779340 },
        { 0x7926dcf95f83c, 0x42e25120e2bec, 0x63de96df1fa15, 0x4f06b50f3f9cc, 0x6fc5cc1b0b62f },
        { 0x75528b29879cb, 0x79a8fd2125a3d, 0x27c8d4b746ab8, 0x0f8893f02210c, 0x15596b3ae5710 },
    },
    {
        { 0x731167e5124ca, 0x17b38e8bbe13f, 0x3d55b942f9056, 0x09c1495be913f, 0x3aa4e241afb6d },
        { 0x739d23f9179a2, 0x632fadbb9e8c4, 0x7c8522bfe0c48, 0x6ed0983ef5aa9, 0x0d2237687b5f4 },
        { 0x138bf2a3305f5, 0x1f45d24d86598, 0x5274bad2160fe, 0x1b6041d58d12a, 0x32fcaa6e4687a },
    },
    {
        { 0x7a4732787ccdf, 0x11e427c7f0640, 0x03659385f8c64, 0x5f4ead9766bfb, 0x746f6336c2600 },
        { 0x56e8dc57d9af5, 0x5b3be17be4f78, 0x3bf928cf82f4b, 0x52e55600a6f11, 0x4627e9cefebd6 },
        { 0x2f345ab6c971c, 0x653286e63e7e9, 0x51061b78a23ad, 0x14999acb54501, 0x7b4917007ed66 },
    },
    {
        { 0x41b28dd53a2dd, 0x37be85f87ea86, 0x74be3d2a85e41, 0x1be87fac96ca6, 0x1d03620fe08cd },
        { 0x5fb5cab84b064, 0x2513e778285b0, 0x457383125e043, 0x6bda3b56e223d, 0x122ba376f844f },
        { 0x232cda2b4e554, 0x0422ba30ff840, 0x751e7667b43f5, 0x6261755da5f3e, 0x02c70bf52b68e },
    },
    {
        { 0x532bf458d72e1, 0x40f96e796b59c, 0x22ef79d6f9da3, 0x501ab67beca77, 0x6b0697e3feb43 },
        { 0x7ec4b5d0b2fbb, 0x200e910595450, 0x742057105715e, 0x2f07022530f60, 0x26334f0a409ef },
        { 0x0f04adf62a3c0, 0x5e0edb48bb6d9, 0x7c34aa4fbc003, 0x7d74e4e5cac24, 0x1cc37f43441b2 },
    },
    {
        { 0x656f1c9ceaeb9, 0x7031cacad5aec, 0x1308cd0716c57, 0x41c1373941942, 0x3a346f772f196 },
        { 0x7565a5cc7324f, 0x01ca0d5244a11, 0x116b067418713, 0x0a57d8c55edae, 0x6c6809c103803 },
        { 0x55112e2da6ac8, 0x6363d0a3dba5a, 0x319c98ba6f40c, 0x2e84b03a36ec7, 0x05911b9f6ef7c },
    },
},
{
    {
        { 0x1acf3512eeaef, 0x2639839692a69, 0x669a234830507, 0x68b920c0603d4, 0x555ef9d1c64b2 },
        { 0x39983f5df0ebb, 0x1ea2589959826, 0x6ce638703cdd6, 0x6311678898505, 0x6b3cecf9aa270 },
        { 0x770ba3b73bd08, 0x11475f7e186d4, 0x0251bc9892bbc, 0x24eab9bffcc5a, 0x675f4de133817 },
    },
    {
        { 0x7f6d93bdab31d, 0x1f3aca5bfd425, 0x2fa521c1c9760, 0x62180ce27f9cd, 0x60f450b882cd3 },
        { 0x452036b1782fc, 0x02d95b07681c5, 0x5901cf99205b2, 0x290686e5eecb4, 0x13d99df70164c },
        { 0x35ec321e5c0ca, 0x13ae337f44029, 0x4008e813f2da7, 0x640272f8e0c3a, 0x1c06de9e55eda },
    },
    {
        { 0x52b40ff6d69aa, 0x31b8809377ffa, 0x536625cd14c2c, 0x516af252e17d1, 0x78096f8e7d32b },
        { 0x77ad6a33ec4e2, 0x717c5dc11d321, 0x4a114559823e4, 0x306ce50a1e2b1, 0x4cf38a1fec2db },
        { 0x2aa650dfa5ce7, 0x54916a8f19415, 0x00dc96fe71278, 0x55f2784e63eb8, 0x373cad3a26091 },
    },
    {
        { 0x6a8fb89ddbbad, 0x78c35d5d97e37, 0x66e3674ef2cb2, 0x34347ac53dd8f, 0x21547eda5112a },
        { 0x4634d82c9f57c, 0x4249268a6d652, 0x6336d687f2ff7, 0x4fe4f4e26d9a0, 0x0040f3d945441 },
        { 0x5e939fd5986d3, 0x12a2147019bdf, 0x4c466e7d09cb2, 0x6fa5b95d203dd, 0x63550a334a254 },
    },
    {
        { 0x2584572547b49, 0x75c58811c1377, 0x4d3c637cc171b, 0x33d30747d34e3, 0x39a92bafaa7d7 },
        { 0x7d6edb569cf37, 0x60194a5dc2ca0, 0x5af59745e10a6, 0x7a8f53e004875, 0x3eea62c7daf78 },
        { 0x4c713e693274e, 0x6ed1b7a6eb3a4, 0x62ace697d8e15, 0x266b8292ab075, 0x68436a0665c9c },
    },
    {
        { 0x6d317e820107c, 0x090815d2ca3ca, 0x03ff1eb1499a1, 0x23960f050e319, 0x5373669c91611 },
        { 0x235e8202f3f27, 0x44c9f2eb61780, 0x630905b1d7003, 0x4fcc8d274ead1, 0x17b6e7f68ab78 },
        { 0x014ab9a0e5257, 0x09939567f8ba5, 0x4b47b2a423c82, 0x688d7e57ac42d, 0x1cb4b5a678f87 },
    },
    {
        { 0x4aa62a2a007e7, 0x61e0e38f62d6e, 0x02f888fcc4782, 0x7562b83f21c00, 0x2dc0fd2d82ef6 },
        { 0x4c06b394afc6c, 0x4931b4bf636cc, 0x72b60d0322378, 0x25127c6818b25, 0x330bca78de743 },
        { 0x6ff841119744e, 0x2c560e8e49305, 0x7254fefe5a57a, 0x67ae2c560a7df, 0x3c31be1b369f1 },
    },
    {
        { 0x0bc93f9cb4272, 0x3f8f9db73182d, 0x2b235eabae1c4, 0x2ddbf8729551a, 0x41cec1097e7d5 },
        { 0x4864d08948aee, 0x5d237438df61e, 0x2b285601f7067, 0x25dbcbae6d753, 0x330b61134262d },
        { 0x619d7a26d808a, 0x3c3b3c2adbef2, 0x6877c9eec7f52, 0x3beb9ebe1b66d, 0x26b44cd91f287 },
    },
},
{
    {
        { 0x7f29362730383, 0x7fd7951459c36, 0x7504c512d49e7, 0x087ed7e3bc55f, 0x7deb10149c726 },
        { 0x048478f387475, 0x69397d9678a3e, 0x67c8156c976f3, 0x2eb4d5589226c, 0x2c709e6c1c10a },
        { 0x2af6a8766ee7a, 0x08aaa79a1d96c, 0x42f92d59b2fb0, 0x1752c40009c07, 0x08e68e9ff62ce },
    },
    {
        { 0x509d50ab8f2f9, 0x1b8ab247be5e5, 0x5d9b2e6b2e486, 0x4faa5479a1339, 0x4cb13bd738f71 },
        { 0x5500a4bc130ad, 0x127a17a938695, 0x02a26fa34e36d, 0x584d12e1ecc28, 0x2f1f3f87eeba3 },
        { 0x48c75e515b64a, 0x75b6952071ef0, 0x5d46d42965406, 0x7746106989f9f, 0x19a1e353c0ae2 },
    },
    {
        { 0x172cdd596bdbd, 0x0731ddf881684, 0x10426d64f8115, 0x71a4fd8a9a3da, 0x736bd3990266a },
        { 0x47560bafa05c3, 0x418dcabcc2fa3, 0x35991cecf8682, 0x24371a94b8c60, 0x41546b11c20c3 },
        { 0x32d509334b3b4, 0x16c102cae70aa, 0x1720dd51bf445, 0x5ae662faf9821, 0x412295a2b87fa },
    },
    {
        { 0x55261e293eac6, 0x06426759b65cc, 0x40265ae116a48, 0x6c02304bae5bc, 0x0760bb8d195ad },
        { 0x19b88f57ed6e9, 0x4cdbf1904a339, 0x42b49cd4e4f2c, 0x71a2e771909d9, 0x14e153ebb52d2 },
        { 0x61a17cde6818a, 0x53dad34108827, 0x32b32c55c55b6, 0x2f9165f9347a3, 0x6b34be9bc33ac },
    },
    {
        { 0x469656571f2d3, 0x0aa61ce6f423f, 0x3f940d71b27a1, 0x185f19d73d16a, 0x01b9c7b62e6dd },
        { 0x72f643a78c0b2, 0x3de45c04f9e7b, 0x706d68d30fa5c, 0x696f63e8e2f24, 0x2012c18f0922d },
        { 0x355e55ac89d29, 0x3e8b414ec7101, 0x39db07c520c90, 0x6f41e9b77efe1, 0x08af5b784e4ba },
    },
    {
        { 0x314d289cc2c4b, 0x23450e2f1bc4e, 0x0cd93392f92f4, 0x1370c6a946b7d, 0x6423c1d5afd98 },
        { 0x499dc881f2533, 0x34ef26476c506, 0x4d107d2741497, 0x346c4bd6efdb3, 0x32b79d71163a1 },
        { 0x5f8d9edfcb36a, 0x1e6e8dcbf3990, 0x7974f348af30a, 0x6e6724ef19c7c, 0x480a5efbc13e2 },
    },
    {
        { 0x14ce442ce221f, 0x18980a72516cc, 0x072f80db86677, 0x703331fda526e, 0x24b31d47691c8 },
        { 0x1e70b01622071, 0x1f163b5f8a16a, 0x56aaf341ad417, 0x7989635d830f7, 0x47aa27600cb7b },
        { 0x41eedc015f8c3, 0x7cf8d27ef854a, 0x289e3584693f9, 0x04a7857b309a7, 0x545b585d14dda },
    },
    {
        { 0x4e4d0e3b321e1, 0x7451fe3d2ac40, 0x666f678eea98d, 0x038858667fead, 0x4d22dc3e64c8d },
        { 0x7275ea0d43a0f, 0x681137dd7ccf7, 0x1e79cbab79a38, 0x22a214489a66a, 0x0f62f9c332ba5 },
        { 0x46589d63b5f39, 0x7eaf979ec3f96, 0x4ebe81572b9a8, 0x21b7f5d61694a, 0x1c0fa01a36371 },
    },
},
{
    {
        { 0x02b0e8c936a50, 0x6b83b58b6cd21, 0x37ed8d3e72680, 0x0a037db9f2a62, 0x4005419b1d2bc },
        { 0x604b622943dff, 0x1c899f6741a58, 0x60219e2f232fb, 0x35fae92a7f9cb, 0x0fa3614f3b1ca },
        { 0x3febdb9be82f0, 0x5e74895921400, 0x553ea38822706, 0x5a17c24cfc88c, 0x1fba218aef40a },
    },
    {
        { 0x657043e7b0194, 0x5c11b55efe9e7, 0x7737bc6a074fb, 0x0eae41ce355cc, 0x6c535d13ff776 },
        { 0x49448fac8f53e, 0x34f74c6e8356a, 0x0ad780607dba2, 0x7213a7eb63eb6, 0x392e3acaa8c86 },
        { 0x534e93e8a35af, 0x08b10fd02c997, 0x26ac2acb81e05, 0x09d8c98ce3b79, 0x25e17fe4d50ac },
    },
    {
        { 0x77ff576f121a7, 0x4e5f9b0fc722b, 0x46f949b0d28c8, 0x4cde65d17ef26, 0x6bba828f89698 },
        { 0x09bd71e04f676, 0x25ac841f2a145, 0x1a47eac823871, 0x1a8a8c36c581a, 0x255751442a9fb },
        { 0x1bc6690fe3901, 0x314132f5abc5a, 0x611835132d528, 0x5f24b8eb48a57, 0x559d504f7f6b7 },
    },
    {
        { 0x091e7f6d266fd, 0x36060ef037389, 0x18788ec1d1286, 0x287441c478eb0, 0x123ea6a3354bd },
        { 0x38378b3eb54d5, 0x4d4aaa78f94ee, 0x4a002e875a74d, 0x10b851367b17c, 0x01ab12d5807e3 },
        { 0x5189041e32d96, 0x05b062b090231, 0x0c91766e7b78f, 0x0aa0f55a138ec, 0x4a3961e2c918a },
    },
    {
        { 0x7d644f3233f1e, 0x1c69f9e02c064, 0x36ae5e5266898, 0x08fc1dad38b79, 0x68aceead9bd41 },
        { 0x43be0f8e6bba0, 0x68fdffc614e3b, 0x4e91dab5b3be0, 0x3b1d4c9212ff0, 0x2cd6bce3fb1db },
        { 0x4c90ef3d7c210, 0x496f5a0818716, 0x79cf88cc239b8, 0x2cb9c306cf8db, 0x595760d5b508f },
    },
    {
        { 0x2cbebfd022790, 0x0b8822aec1105, 0x4d1cfd226bccc, 0x515b2fa4971be, 0x2cb2c5df54515 },
        { 0x1bfe104aa6397, 0x11494ff996c25, 0x64251623e5800, 0x0d49fc5e044be, 0x709fa43edcb29 },
        { 0x25d8c63fd2aca, 0x4c5cd29dffd61, 0x32ec0eb48af05, 0x18f9391f9b77c, 0x70f029ecf0c81 },
    },
    {
        { 0x2afaa5e10b0b9, 0x61de08355254d, 0x0eb587de3c28d, 0x4f0bb9f7dbbd5, 0x44eca5a2a74bd },
        { 0x307b32eed3e33, 0x6748ab03ce8c2, 0x57c0d9ab810bc, 0x42c64a224e98c, 0x0b7d5d8a6c314 },
        { 0x448327b95d543, 0x0146681e3a4ba, 0x38714adc34e0c, 0x4f26f0e298e30, 0x272224512c7de },
    },
    {
        { 0x3bb8a42a975fc, 0x6f2d5b46b17ef, 0x7b6a9223170e5, 0x053713fe3b7e6, 0x19735fd7f6bc2 },
        { 0x492af49c5342e, 0x2365cdf5a0357, 0x32138a7ffbb60, 0x2a1f7d14646fe, 0x11b5df18a44cc },
        { 0x390d042c84266, 0x1efe32a8fdc75, 0x6925ee7ae1238, 0x4af9281d0e832, 0x0fef911191df8 },
    },
},
};
#else
/* base[i][j] = (j+1)*256^i*B */
static const ge_precomp base[32][8] = {
{
 {
  { 25967493,-14356035,29566456,3660896,-12694345,4014787,27544626,-11754271,-6079156,2047605 },
  { -12545711,934262,-2722910,3049990,-727428,9406986,12720692,5043384,19500929,-15469378 },
  { -8738181,4489570,9688441,-14785194,10184609,-12363380,29287919,11864899,-24514362,-4438546 },
 },
 {
  { -12815894,-12976347,-21581243,11784320,-25355658,-2750717,-11717903,-3814571,-358445,-10211303 },
  { -21703237,6903825,27185491,6451973,-29577724,-9554005,-15616551,11189268,-26829678,-5319081 },
  { 26966642,11152617,32442495,15396054,14353839,-12752335,-3128826,-9541118,-15472047,-4166697 },
 },
 {
  { 15636291,-9688557,24204773,-7912398,616977,-16685262,27787600,-14772189,28944400,-1550024 },
  { 16568933,4717097,-11556148,-1102322,15682896,-11807043,16354577,-11775962,7689662,11199574 },
  { 30464156,-5976125,-11779434,-15670865,23220365,15915852,7512774,10017326,-17749093,-9920357 },
 },
 {
  { -17036878,13921892,10945806,-6033431,27105052,-16084379,-28926210,15006023,3284568,-6276540 },
  { 23599295,-8306047,-11193664,-7687416,13236774,10506355,7464579,9656445,13059162,10374397 },
  { 7798556,16710257,3033922,2874086,28997861,2835604,32406664,-3839045,-641708,-101325 },
 },
 {
  { 10861363,11473154,27284546,1981175,-30064349,12577861,32867885,14515107,-15438304,10819380 },
  { 4708026,6336745,20377586,9066809,-11272109,6594696,-25653668,12483688,-12668491,5581306 },
  { 19563160,16186464,-29386857,4097519,10237984,-4348115,28542350,13850243,-23678021,-15815942 },
 },
 {
  { -15371964,-12862754,32573250,4720197,-26436522,5875511,-19188627,-15224819,-9818940,-12085777 },
  { -8549212,109983,15149363,2178705,22900618,4543417,3044240,-15689887,1762328,14866737 },
  { -18199695,-15951423,-10473290,1707278,-17185920,3916101,-28236412,3959421,27914454,4383652 },
 },
 {
  { 5153746,9909285,1723747,-2777874,30523605,5516873,19480852,5230134,-23952439,-15175766 },
  { -30269007,-3463509,7665486,10083793,28475525,1649722,20654025,16520125,30598449,7715701 },
  { 28881845,14381568,9657904,3680757,-20181635,7843316,-31400660,1370708,29794553,-1409300 },
 },
 {
  { 14499471,-2729599,-33191113,-4254652,28494862,14271267,30290735,10876454,-33154098,2381726 },
  { -7195431,-2655363,-14730155,462251,-27724326,3941372,-6236617,3696005,-32300832,15351955 },
  { 27431194,8222322,16448760,-3907995,-18707002,11938355,-32961401,-2970515,29551813,10109425 },
 },
},
{
 {
  { -13657040,-13155431,-31283750,11777098,21447386,6519384,-2378284,-1627556,10092783,-4764171 },
  { 27939166,14210322,4677035,16277044,-22964462,-12398139,-32508754,12005538,-17810127,12803510 },
  { 17228999,-15661624,-1233527,300140,-1224870,-11714777,30364213,-9038194,18016357,4397660 },
 },
 {
  { -10958843,-7690207,4776341,-14954238,27850028,-15602212,-26619106,14544525,-17477504,982639 },
  { 29253598,15796703,-2863982,-9908884,10057023,3163536,7332899,-4120128,-21047696,9934963 },
  { 5793303,16271923,-24131614,-10116404,29188560,1206517,-14747930,4559895,-30123922,-10897950 },
 },
 {
  { -27643952,-11493006,16282657,-11036493,28414021,-15012264,24191034,4541697,-13338309,5500568 },
  { 12650548,-1497113,9052871,11355358,-17680037,-8400164,-17430592,12264343,10874051,13524335 },
  { 25556948,-3045990,714651,2510400,23394682,-10415330,33119038,5080568,-22528059,5376628 },
 },
 {
  { -26088264,-4011052,-17013699,-3537628,-6726793,1920897,-22321305,-9447443,4535768,1569007 },
  { -2255422,14606630,-21692440,-8039818,28430649,8775819,-30494562,3044290,31848280,12543772 },
  { -22028579,2943893,-31857513,6777306,13784462,-4292203,-27377195,-2062731,7718482,14474653 },
 },
 {
  { 2385315,2454213,-22631320,46603,-4437935,-15680415,656965,-7236665,24316168,-5253567 },
  { 13741529,10911568,-33233417,-8603737,-20177830,-1033297,33040651,-13424532,-20729456,8321686 },
  { 21060490,-2212744,15712757,-4336099,1639040,10656336,23845965,-11874838,-9984458,608372 },
 },
 {
  { -13672732,-15087586,-10889693,-7557059,-6036909,11305547,1123968,-6780577,27229399,23887 },
  { -23244140,-294205,-11744728,14712571,-29465699,-2029617,12797024,-6440308,-1633405,16678954 },
  { -29500620,4770662,-16054387,14001338,7830047,9564805,-1508144,-4795045,-17169265,4904953 },
 },
 {
  { 24059557,14617003,19037157,-15039908,19766093,-14906429,5169211,16191880,2128236,-4326833 },
  { -16981152,4124966,-8540610,-10653797,30336522,-14105247,-29806336,916033,-6882542,-2986532 },
  { -22630907,12419372,-7134229,-7473371,-16478904,16739175,285431,2763829,15736322,4143876 },
 },
 {
  { 2379352,11839345,-4110402,-5988665,11274298,794957,212801,-14594663,23527084,-16458268 },
  { 33431127,-11130478,-17838966,-15626900,8909499,8376530,-32625340,4087881,-15188911,-14416214 },
  { 1767683,7197987,-13205226,-2022635,-13091350,448826,5799055,4357868,-4774191,-16323038 },
 },
},
{
 {
  { 6721966,13833823,-23523388,-1551314,26354293,-11863321,23365147,-3949732,7390890,2759800 },
  { 4409041,2052381,23373853,10530217,7676779,-12885954,21302353,-4264057,1244380,-12919645 },
  { -4421239,7169619,4982368,-2957590,30256825,-2777540,14086413,9208236,15886429,16489664 },
 },
 {
  { 1996075,10375649,14346367,13311202,-6874135,-16438411,-13693198,398369,-30606455,-712933 },
  { -25307465,9795880,-2777414,14878809,-33531835,14780363,13348553,12076947,-30836462,5113182 },
  { -17770784,11797796,31950843,13929123,-25888302,12288344,-30341101,-7336386,13847711,5387222 },
 },
 {
  { -18582163,-3416217,17824843,-2340966,22744343,-10442611,8763061,3617786,-19600662,10370991 },
  { 20246567,-14369378,22358229,-543712,18507283,-10413996,14554437,-8746092,32232924,16763880 },
  { 9648505,10094563,26416693,14745928,-30374318,-6472621,11094161,15689506,3140038,-16510092 },
 },
 {
  { -16160072,5472695,31895588,4744994,8823515,10365685,-27224800,9448613,-28774454,366295 },
  { 19153450,11523972,-11096490,-6503142,-24647631,5420647,28344573,8041113,719605,11671788 },
  { 8678025,2694440,-6808014,2517372,4964326,11152271,-15432916,-15266516,27000813,-10195553 },
 },
 {
  { -15157904,7134312,8639287,-2814877,-7235688,10421742,564065,5336097,6750977,-14521026 },
  { 11836410,-3979488,26297894,16080799,23455045,15735944,1695823,-8819122,8169720,16220347 },
  { -18115838,8653647,17578566,-6092619,-8025777,-16012763,-11144307,-2627664,-5990708,-14166033 },
 },
 {
  { -23308498,-10968312,15213228,-10081214,-30853605,-11050004,27884329,2847284,2655861,1738395 },
  { -27537433,-14253021,-25336301,-8002780,-9370762,8129821,21651608,-3239336,-19087449,-11005278 },
  { 1533110,3437855,23735889,459276,29970501,11335377,26030092,5821408,10478196,8544890 },
 },
 {
  { 32173121,-16129311,24896207,3921497,22579056,-3410854,19270449,12217473,17789017,-3395995 },
  { -30552961,-2228401,-15578829,-10147201,13243889,517024,15479401,-3853233,30460520,1052596 },
  { -11614875,13323618,32618793,8175907,-15230173,12596687,27491595,-4612359,3179268,-9478891 },
 },
 {
  { 31947069,-14366651,-4640583,-15339921,-15125977,-6039709,-14756777,-16411740,19072640,-9511060 },
  { 11685058,11822410,3158003,-13952594,33402194,-4165066,5977896,-5215017,473099,5040608 },
  { -20290863,8198642,-27410132,11602123,1290375,-2799760,28326862,1721092,-19558642,-3131606 },
 },
},
{
 {
  { 7881532,10687937,7578723,7738378,-18951012,-2553952,21820786,8076149,-27868496,11538389 },
  { -19935666,3899861,18283497,-6801568,-15728660,-11249211,8754525,7446702,-5676054,5797016 },
  { -11295600,-3793569,-15782110,-7964573,12708869,-8456199,2014099,-9050574,-2369172,-5877341 },
 },
 {
  { -22472376,-11568741,-27682020,1146375,18956691,16640559,1192730,-3714199,15123619,10811505 },
  { 14352098,-3419715,-18942044,10822655,32750596,4699007,-70363,15776356,-28886779,-11974553 },
  { -28241164,-8072475,-4978962,-5315317,29416931,1847569,-20654173,-16484855,4714547,-9600655 },
 },
 {
  { 15200332,8368572,19679101,15970074,-31872674,1959451,24611599,-4543832,-11745876,12340220 },
  { 12876937,-10480056,33134381,6590940,-6307776,14872440,9613953,8241152,15370987,9608631 },
  { -4143277,-12014408,8446281,-391603,4407738,13629032,-7724868,15866074,-28210621,-8814099 },
 },
 {
  { 26660628,-15677655,8393734,358047,-7401291,992988,-23904233,858697,20571223,8420556 },
  { 14620715,13067227,-15447274,8264467,14106269,15080814,33531827,12516406,-21574435,-12476749 },
  { 236881,10476226,57258,-14677024,6472998,2466984,17258519,7256740,8791136,15069930 },
 },
 {
  { 1276410,-9371918,22949635,-16322807,-23493039,-5702186,14711875,4874229,-30663140,-2331391 },
  { 5855666,4990204,-13711848,7294284,-7804282,1924647,-1423175,-7912378,-33069337,9234253 },
  { 20590503,-9018988,31529744,-7352666,-2706834,10650548,31559055,-11609587,18979186,13396066 },
 },
 {
  { 24474287,4968103,22267082,4407354,24063882,-8325180,-18816887,13594782,33514650,7021958 },
  { -11566906,-6565505,-21365085,15928892,-26158305,4315421,-25948728,-3916677,-21480480,12868082 },
  { -28635013,13504661,19988037,-2132761,21078225,6443208,-21446107,2244500,-12455797,-8089383 },
 },
 {
  { -30595528,13793479,-5852820,319136,-25723172,-6263899,33086546,8957937,-15233648,5540521 },
  { -11630176,-11503902,-8119500,-7643073,2620056,1022908,-23710744,-1568984,-16128528,-14962807 },
  { 23152971,775386,27395463,14006635,-9701118,4649512,1689819,892185,-11513277,-15205948 },
 },
 {
  { 9770129,9586738,26496094,4324120,1556511,-3550024,27453819,4763127,-19179614,5867134 },
  { -32765025,1927590,31726409,-4753295,23962434,-16019500,27846559,5931263,-29749703,-16108455 },
  { 27461885,-2977536,22380810,1815854,-23033753,-3031938,7283490,-15148073,-19526700,7734629 },
 },
},
{
 {
  { -8010264,-9590817,-11120403,6196038,29344158,-13430885,7585295,-3176626,18549497,15302069 },
  { -32658337,-6171222,-7672793,-11051681,6258878,13504381,10458790,-6418461,-8872242,8424746 },
  { 24687205,8613276,-30667046,-3233545,1863892,-1830544,19206234,7134917,-11284482,-828919 },
 },
 {
  { 11334899,-9218022,8025293,12707519,17523892,-10476071,10243738,-14685461,-5066034,16498837 },
  { 8911542,6887158,-9584260,-6958590,11145641,-9543680,17303925,-14124238,6536641,10543906 },
  { -28946384,15479763,-17466835,568876,-1497683,11223454,-2669190,-16625574,-27235709,8876771 },
 },
 {
  { -25742899,-12566864,-15649966,-846607,-33026686,-796288,-33481822,15824474,-604426,-9039817 },
  { 10330056,70051,7957388,-9002667,9764902,15609756,27698697,-4890037,1657394,3084098 },
  { 10477963,-7470260,12119566,-13250805,29016247,-5365589,31280319,14396151,-30233575,15272409 },
 },
 {
  { -12288309,3169463,28813183,16658753,25116432,-5630466,-25173957,-12636138,-25014757,1950504 },
  { -26180358,9489187,11053416,-14746161,-31053720,5825630,-8384306,-8767532,15341279,8373727 },
  { 28685821,7759505,-14378516,-12002860,-31971820,4079242,298136,-10232602,-2878207,15190420 },
 },
 {
  { -32932876,13806336,-14337485,-15794431,-24004620,10940928,8669718,2742393,-26033313,-6875003 },
  { -1580388,-11729417,-25979658,-11445023,-17411874,-10912854,9291594,-16247779,-12154742,6048605 },
  { -30305315,14843444,1539301,11864366,20201677,1900163,13934231,5128323,11213262,9168384 },
 },
 {
  { -26280513,11007847,19408960,-940758,-18592965,-4328580,-5088060,-11105150,20470157,-16398701 },
  { -23136053,9282192,14855179,-15390078,-7362815,-14408560,-22783952,14461608,14042978,5230683 },
  { 29969567,-2741594,-16711867,-8552442,9175486,-2468974,21556951,3506042,-5933891,-12449708 },
 },
 {
  { -3144746,8744661,19704003,4581278,-20430686,6830683,-21284170,8971513,-28539189,15326563 },
  { -19464629,10110288,-17262528,-3503892,-23500387,1355669,-15523050,15300988,-20514118,9168260 },
  { -5353335,4488613,-23803248,16314347,7780487,-15638939,-28948358,9601605,33087103,-9011387 },
 },
 {
  { -19443170,-15512900,-20797467,-12445323,-29824447,10229461,-27444329,-15000531,-5996870,15664672 },
  { 23294591,-16632613,-22650781,-8470978,27844204,11461195,13099750,-2460356,18151676,13417686 },
  { -24722913,-4176517,-31150679,5988919,-26858785,6685065,1661597,-12551441,15271676,-15452665 },
 },
},
{
 {
  { 11433042,-13228665,8239631,-5279517,-1985436,-725718,-18698764,2167544,-6921301,-13440182 },
  { -31436171,15575146,30436815,12192228,-22463353,9395379,-9917708,-8638997,12215110,12028277 },
  { 14098400,6555944,23007258,5757252,-15427832,-12950502,30123440,4617780,-16900089,-655628 },
 },
 {
  { -4026201,-15240835,11893168,13718664,-14809462,1847385,-15819999,10154009,23973261,-12684474 },
  { -26531820,-3695990,-1908898,2534301,-31870557,-16550355,18341390,-11419951,32013174,-10103539 },
  { -25479301,10876443,-11771086,-14625140,-12369567,1838104,21911214,6354752,4425632,-837822 },
 },
 {
  { -10433389,-14612966,22229858,-3091047,-13191166,776729,-17415375,-12020462,4725005,14044970 },
  { 19268650,-7304421,1555349,8692754,-21474059,-9910664,6347390,-1411784,-19522291,-16109756 },
  { -24864089,12986008,-10898878,-5558584,-11312371,-148526,19541418,8180106,9282262,10282508 },
 },
 {
  { -26205082,4428547,-8661196,-13194263,4098402,-14165257,15522535,8372215,5542595,-10702683 },
  { -10562541,14895633,26814552,-16673850,-17480754,-2489360,-2781891,6993761,-18093885,10114655 },
  { -20107055,-929418,31422704,10427861,-7110749,6150669,-29091755,-11529146,25953725,-106158 },
 },
 {
  { -4234397,-8039292,-9119125,3046000,2101609,-12607294,19390020,6094296,-3315279,12831125 },
  { -15998678,7578152,5310217,14408357,-33548620,-224739,31575954,6326196,7381791,-2421839 },
  { -20902779,3296811,24736065,-16328389,18374254,7318640,6295303,8082724,-15362489,12339664 },
 },
 {
  { 27724736,2291157,6088201,-14184798,1792727,5857634,13848414,15768922,25091167,14856294 },
  { -18866652,8331043,24373479,8541013,-701998,-9269457,12927300,-12695493,-22182473,-9012899 },
  { -11423429,-5421590,11632845,3405020,30536730,-11674039,-27260765,13866390,30146206,9142070 },
 },
 {
  { 3924129,-15307516,-13817122,-10054960,12291820,-668366,-27702774,9326384,-8237858,4171294 },
  { -15921940,16037937,6713787,16606682,-21612135,2790944,26396185,3731949,345228,-5462949 },
  { -21327538,13448259,25284571,1143661,20614966,-8849387,2031539,-12391231,-16253183,-13582083 },
 },
 {
  { 31016211,-16722429,26371392,-14451233,-5027349,14854137,17477601,3842657,28012650,-16405420 },
  { -5075835,9368966,-8562079,-4600902,-15249953,6970560,-9189873,16292057,-8867157,3507940 },
  { 29439664,3537914,23333589,6997794,-17555561,-11018068,-15209202,-15051267,-9164929,6580396 },
 },
},
{
 {
  { -12185861,-7679788,16438269,10826160,-8696817,-6235611,17860444,-9273846,-2095802,9304567 },
  { 20714564,-4336911,29088195,7406487,11426967,-5095705,14792667,-14608617,5289421,-477127 },
  { -16665533,-10650790,-6160345,-13305760,9192020,-1802462,17271490,12349094,26939669,-3752294 },
 },
 {
  { -12889898,9373458,31595848,16374215,21471720,13221525,-27283495,-12348559,-3698806,117887 },
  { 22263325,-6560050,3984570,-11174646,-15114008,-566785,28311253,5358056,-23319780,541964 },
  { 16259219,3261970,2309254,-15534474,-16885711,-4581916,24134070,-16705829,-13337066,-13552195 },
 },
 {
  { 9378160,-13140186,-22845982,-12745264,28198281,-7244098,-2399684,-717351,690426,14876244 },
  { 24977353,-314384,-8223969,-13465086,28432343,-1176353,-13068804,-12297348,-22380984,6618999 },
  { -1538174,11685646,12944378,13682314,-24389511,-14413193,8044829,-13817328,32239829,-5652762 },
 },
 {
  { -18603066,4762990,-926250,8885304,-28412480,-3187315,9781647,-10350059,32779359,5095274 },
  { -33008130,-5214506,-32264887,-3685216,9460461,-9327423,-24601656,14506724,21639561,-2630236 },
  { -16400943,-13112215,25239338,15531969,3987758,-4499318,-1289502,-6863535,17874574,558605 },
 },
 {
  { -13600129,10240081,9171883,16131053,-20869254,9599700,33499487,5080151,2085892,5119761 },
  { -22205145,-2519528,-16381601,414691,-25019550,2170430,30634760,-8363614,-31999993,-5759884 },
  { -6845704,15791202,8550074,-1312654,29928809,-12092256,27534430,-7192145,-22351378,12961482 },
 },
 {
  { -24492060,-9570771,10368194,11582341,-23397293,-2245287,16533930,8206996,-30194652,-5159638 },
  { -11121496,-3382234,2307366,6362031,-135455,8868177,-16835630,7031275,7589640,8945490 },
  { -32152748,8917967,6661220,-11677616,-1192060,-15793393,7251489,-11182180,24099109,-14456170 },
 },
 {
  { 5019558,-7907470,4244127,-14714356,-26933272,6453165,-19118182,-13289025,-6231896,-10280736 },
  { 10853594,10721687,26480089,5861829,-22995819,1972175,-1866647,-10557898,-3363451,-6441124 },
  { -17002408,5906790,221599,-6563147,7828208,-13248918,24362661,-2008168,-13866408,7421392 },
 },
 {
  { 8139927,-6546497,32257646,-5890546,30375719,1886181,-21175108,15441252,28826358,-4123029 },
  { 6267086,9695052,7709135,-16603597,-32869068,-1886135,14795160,-7840124,13746021,-1742048 },
  { 28584902,7787108,-6732942,-15050729,22846041,-7571236,-3181936,-363524,4771362,-8419958 },
 },
},
{
 {
  { 24949256,6376279,-27466481,-8174608,-18646154,-9930606,33543569,-12141695,3569627,11342593 },
  { 26514989,4740088,27912651,3697550,19331575,-11472339,6809886,4608608,7325975,-14801071 },
  { -11618399,-14554430,-24321212,7655128,-1369274,5214312,-27400540,10258390,-17646694,-8186692 },
 },
 {
  { 11431204,15823007,26570245,14329124,18029990,4796082,-31446179,15580664,9280358,-3973687 },
  { -160783,-10326257,-22855316,-4304997,-20861367,-13621002,-32810901,-11181622,-15545091,4387441 },
  { -20799378,12194512,3937617,-5805892,-27154820,9340370,-24513992,8548137,20617071,-7482001 },
 },
 {
  { -938825,-3930586,-8714311,16124718,24603125,-6225393,-13775352,-11875822,24345683,10325460 },
  { -19855277,-1568885,-22202708,8714034,14007766,6928528,16318175,-1010689,4766743,3552007 },
  { -21751364,-16730916,1351763,-803421,-4009670,3950935,3217514,14481909,10988822,-3994762 },
 },
 {
  { 15564307,-14311570,3101243,5684148,30446780,-8051356,12677127,-6505343,-8295852,13296005 },
  { -9442290,6624296,-30298964,-11913677,-4670981,-2057379,31521204,9614054,-30000824,12074674 },
  { 4771191,-135239,14290749,-13089852,27992298,14998318,-1413936,-1556716,29832613,-16391035 },
 },
 {
  { 7064884,-7541174,-19161962,-5067537,-18891269,-2912736,25825242,5293297,-27122660,13101590 },
  { -2298563,2439670,-7466610,1719965,-27267541,-16328445,32512469,-5317593,-30356070,-4190957 },
  { -30006540,10162316,-33180176,3981723,-16482138,-13070044,14413974,9515896,19568978,9628812 },
 },
 {
  { 33053803,199357,15894591,1583059,27380243,-4580435,-17838894,-6106839,-6291786,3437740 },
  { -18978877,3884493,19469877,12726490,15913552,13614290,-22961733,70104,7463304,4176122 },
  { -27124001,10659917,11482427,-16070381,12771467,-6635117,-32719404,-5322751,24216882,5944158 },
 },
 {
  { 8894125,7450974,-2664149,-9765752,-28080517,-12389115,19345746,14680796,11632993,5847885 },
  { 26942781,-2315317,9129564,-4906607,26024105,11769399,-11518837,6367194,-9727230,4782140 },
  { 19916461,-4828410,-22910704,-11414391,25606324,-5972441,33253853,8220911,6358847,-1873857 },
 },
 {
  { 801428,-2081702,16569428,11065167,29875704,96627,7908388,-4480480,-13538503,1387155 },
  { 19646058,5720633,-11416706,12814209,11607948,12749789,14147075,15156355,-21866831,11835260 },
  { 19299512,1155910,28703737,14890794,2925026,7269399,26121523,15467869,-26560550,5052483 },
 },
},
{
 {
  { -3017432,10058206,1980837,3964243,22160966,12322533,-6431123,-12618185,12228557,-7003677 },
  { 32944382,14922211,-22844894,5188528,21913450,-8719943,4001465,13238564,-6114803,8653815 },
  { 22865569,-4652735,27603668,-12545395,14348958,8234005,24808405,5719875,28483275,2841751 },
 },
 {
  { -16420968,-1113305,-327719,-12107856,21886282,-15552774,-1887966,-315658,19932058,-12739203 },
  { -11656086,10087521,-8864888,-5536143,-19278573,-3055912,3999228,13239134,-4777469,-13910208 },
  { 1382174,-11694719,17266790,9194690,-13324356,9720081,20403944,11284705,-14013818,3093230 },
 },
 {
  { 16650921,-11037932,-1064178,1570629,-8329746,7352753,-302424,16271225,-24049421,-6691850 },
  { -21911077,-5927941,-4611316,-5560156,-31744103,-10785293,24123614,15193618,-21652117,-16739389 },
  { -9935934,-4289447,-25279823,4372842,2087473,10399484,31870908,14690798,17361620,11864968 },
 },
 {
  { -11307610,6210372,13206574,5806320,-29017692,-13967200,-12331205,-7486601,-25578460,-16240689 },
  { 14668462,-12270235,26039039,15305210,25515617,4542480,10453892,6577524,9145645,-6443880 },
  { 5974874,3053895,-9433049,-10385191,-31865124,3225009,-7972642,3936128,-5652273,-3050304 },
 },
 {
  { 30625386,-4729400,-25555961,-12792866,-20484575,7695099,17097188,-16303496,-27999779,1803632 },
  { -3553091,9865099,-5228566,4272701,-5673832,-16689700,14911344,12196514,-21405489,7047412 },
  { 20093277,9920966,-11138194,-5343857,13161587,12044805,-32856851,4124601,-32343828,-10257566 },
 },
 {
  { -20788824,14084654,-13531713,7842147,19119038,-13822605,4752377,-8714640,-21679658,2288038 },
  { -26819236,-3283715,29965059,3039786,-14473765,2540457,29457502,14625692,-24819617,12570232 },
  { -1063558,-11551823,16920318,12494842,1278292,-5869109,-21159943,-3498680,-11974704,4724943 },
 },
 {
  { 17960970,-11775534,-4140968,-9702530,-8876562,-1410617,-12907383,-8659932,-29576300,1903856 },
  { 23134274,-14279132,-10681997,-1611936,20684485,15770816,-12989750,3190296,26955097,14109738 },
  { 15308788,5320727,-30113809,-14318877,22902008,7767164,29425325,-11277562,31960942,11934971 },
 },
 {
  { -27395711,8435796,4109644,12222639,-24627868,14818669,20638173,4875028,10491392,1379718 },
  { -13159415,9197841,3875503,-8936108,-1383712,-5879801,33518459,16176658,21432314,12180697 },
  { -11787308,11500838,13787581,-13832590,-22430679,10140205,1465425,12689540,-10301319,-13872883 },
 },
},
{
 {
  { 5414091,-15386041,-21007664,9643570,12834970,1186149,-2622916,-1342231,26128231,6032912 },
  { -26337395,-13766162,32496025,-13653919,17847801,-12669156,3604025,8316894,-25875034,-10437358 },
  { 3296484,6223048,24680646,-12246460,-23052020,5903205,-8862297,-4639164,12376617,3188849 },
 },
 {
  { 29190488,-14659046,27549113,-1183516,3520066,-10697301,32049515,-7309113,-16109234,-9852307 },
  { -14744486,-9309156,735818,-598978,-20407687,-5057904,25246078,-15795669,18640741,-960977 },
  { -6928835,-16430795,10361374,5642961,4910474,12345252,-31638386,-494430,10530747,1053335 },
 },
 {
  { -29265967,-14186805,-13538216,-12117373,-19457059,-10655384,-31462369,-2948985,24018831,15026644 },
  { -22592535,-3145277,-2289276,5953843,-13440189,9425631,25310643,13003497,-2314791,-15145616 },
  { -27419985,-603321,-8043984,-1669117,-26092265,13987819,-27297622,187899,-23166419,-2531735 },
 },
 {
  { -21744398,-13810475,1844840,5021428,-10434399,-15911473,9716667,16266922,-5070217,726099 },
  { 29370922,-6053998,7334071,-15342259,9385287,2247707,-13661962,-4839461,30007388,-15823341 },
  { -936379,16086691,23751945,-543318,-1167538,-5189036,9137109,730663,9835848,4555336 },
 },
 {
  { -23376435,1410446,-22253753,-12899614,30867635,15826977,17693930,544696,-11985298,12422646 },
  { 31117226,-12215734,-13502838,6561947,-9876867,-12757670,-5118685,-4096706,29120153,13924425 },
  { -17400879,-14233209,19675799,-2734756,-11006962,-5858820,-9383939,-11317700,7240931,-237388 },
 },
 {
  { -31361739,-11346780,-15007447,-5856218,-22453340,-12152771,1222336,4389483,3293637,-15551743 },
  { -16684801,-14444245,11038544,11054958,-13801175,-3338533,-24319580,7733547,12796905,-6335822 },
  { -8759414,-10817836,-25418864,10783769,-30615557,-9746811,-28253339,3647836,3222231,-11160462 },
 },
 {
  { 18606113,1693100,-25448386,-15170272,4112353,10045021,23603893,-2048234,-7550776,2484985 },
  { 9255317,-3131197,-12156162,-1004256,13098013,-9214866,16377220,-2102812,-19802075,-3034702 },
  { -22729289,7496160,-5742199,11329249,19991973,-3347502,-31718148,9936966,-30097688,-10618797 },
 },
 {
  { 21878590,-5001297,4338336,13643897,-3036865,13160960,19708896,5415497,-7360503,-4109293 },
  { 27736861,10103576,12500508,8502413,-3413016,-9633558,10436918,-1550276,-23659143,-8132100 },
  { 19492550,-12104365,-29681976,-852630,-3208171,12403437,30066266,8367329,13243957,8709688 },
 },
},
{
 {
  { 12015105,2801261,28198131,10151021,24818120,-4743133,-11194191,-5645734,5150968,7274186 },
  { 2831366,-12492146,1478975,6122054,23825128,-12733586,31097299,6083058,31021603,-9793610 },
  { -2529932,-2229646,445613,10720828,-13849527,-11505937,-23507731,16354465,15067285,-14147707 },
 },
 {
  { 7840942,14037873,-33364863,15934016,-728213,-3642706,21403988,1057586,-19379462,-12403220 },
  { 915865,-16469274,15608285,-8789130,-24357026,6060030,-17371319,8410997,-7220461,16527025 },
  { 32922597,-556987,20336074,-16184568,10903705,-5384487,16957574,52992,23834301,6588044 },
 },
 {
  { 32752030,11232950,3381995,-8714866,22652988,-10744103,17159699,16689107,-20314580,-1305992 },
  { -4689649,9166776,-25710296,-10847306,11576752,12733943,7924251,-2752281,1976123,-7249027 },
  { 21251222,16309901,-2983015,-6783122,30810597,12967303,156041,-3371252,12331345,-8237197 },
 },
 {
  { 8651614,-4477032,-16085636,-4996994,13002507,2950805,29054427,-5106970,10008136,-4667901 },
  { 31486080,15114593,-14261250,12951354,14369431,-7387845,16347321,-13662089,8684155,-10532952 },
  { 19443825,11385320,24468943,-9659068,-23919258,2187569,-26263207,-6086921,31316348,14219878 },
 },
 {
  { -28594490,1193785,32245219,11392485,31092169,15722801,27146014,6992409,29126555,9207390 },
  { 32382935,1110093,18477781,11028262,-27411763,-7548111,-4980517,10843782,-7957600,-14435730 },
  { 2814918,7836403,27519878,-7868156,-20894015,-11553689,-21494559,8550130,28346258,1994730 },
 },
 {
  { -19578299,8085545,-14000519,-3948622,2785838,-16231307,-19516951,7174894,22628102,8115180 },
  { -30405132,955511,-11133838,-15078069,-32447087,-13278079,-25651578,3317160,-9943017,930272 },
  { -15303681,-6833769,28856490,1357446,23421993,1057177,24091212,-1388970,-22765376,-10650715 },
 },
 {
  { -22751231,-5303997,-12907607,-12768866,-15811511,-7797053,-14839018,-16554220,-1867018,8398970 },
  { -31969310,2106403,-4736360,1362501,12813763,16200670,22981545,-6291273,18009408,-15772772 },
  { -17220923,-9545221,-27784654,14166835,29815394,7444469,29551787,-3727419,19288549,1325865 },
 },
 {
  { 15100157,-15835752,-23923978,-1005098,-26450192,15509408,12376730,-3479146,33166107,-8042750 },
  { 20909231,13023121,-9209752,16251778,-5778415,-8094914,12412151,10018715,2213263,-13878373 },
  { 32529814,-11074689,30361439,-16689753,-9135940,1513226,22922121,6382134,-5766928,8371348 },
 },
},
{
 {
  { 9923462,11271500,12616794,3544722,-29998368,-1721626,12891687,-8193132,-26442943,10486144 },
  { -22597207,-7012665,8587003,-8257861,4084309,-12970062,361726,2610596,-23921530,-11455195 },
  { 5408411,-1136691,-4969122,10561668,24145918,14240566,31319731,-4235541,19985175,-3436086 },
 },
 {
  { -13994457,16616821,14549246,3341099,32155958,13648976,-17577068,8849297,65030,8370684 },
  { -8320926,-12049626,31204563,5839400,-20627288,-1057277,-19442942,6922164,12743482,-9800518 },
  { -2361371,12678785,28815050,4759974,-23893047,4884717,23783145,11038569,18800704,255233 },
 },
 {
  { -5269658,-1773886,13957886,7990715,23132995,728773,13393847,9066957,19258688,-14753793 },
  { -2936654,-10827535,-10432089,14516793,-3640786,4372541,-31934921,2209390,-1524053,2055794 },
  { 580882,16705327,5468415,-2683018,-30926419,-14696000,-7203346,-8994389,-30021019,7394435 },
 },
 {
  { 23838809,1822728,-15738443,15242727,8318092,-3733104,-21672180,-3492205,-4821741,14799921 },
  { 13345610,9759151,3371034,-16137791,16353039,8577942,31129804,13496856,-9056018,7402518 },
  { 2286874,-4435931,-20042458,-2008336,-13696227,5038122,11006906,-15760352,8205061,1607563 },
 },
 {
  { 14414086,-8002132,3331830,-3208217,22249151,-5594188,18364661,-2906958,30019587,-9029278 },
  { -27688051,1585953,-10775053,931069,-29120221,-11002319,-14410829,12029093,9944378,8024 },
  { 4368715,-3709630,29874200,-15022983,-20230386,-11410704,-16114594,-999085,-8142388,5640030 },
 },
 {
  { 10299610,13746483,11661824,16234854,7630238,5998374,9809887,-16694564,15219798,-14327783 },
  { 27425505,-5719081,3055006,10660664,23458024,595578,-15398605,-1173195,-18342183,9742717 },
  { 6744077,2427284,26042789,2720740,-847906,1118974,32324614,7406442,12420155,1994844 },
 },
 {
  { 14012521,-5024720,-18384453,-9578469,-26485342,-3936439,-13033478,-10909803,24319929,-6446333 },
  { 16412690,-4507367,10772641,15929391,-17068788,-4658621,10555945,-10484049,-30102368,-4739048 },
  { 22397382,-7767684,-9293161,-12792868,17166287,-9755136,-27333065,6199366,21880021,-12250760 },
 },
 {
  { -4283307,5368523,-31117018,8163389,-30323063,3209128,16557151,8890729,8840445,4957760 },
  { -15447727,709327,-6919446,-10870178,-29777922,6522332,-21720181,12130072,-14796503,5005757 },
  { -2114751,-14308128,23019042,15765735,-25269683,6002752,10183197,-13239326,-16395286,-2176112 },
 },
},
{
 {
  { -19025756,1632005,13466291,-7995100,-23640451,16573537,-32013908,-3057104,22208662,2000468 },
  { 3065073,-1412761,-25598674,-361432,-17683065,-5703415,-8164212,11248527,-3691214,-7414184 },
  { 10379208,-6045554,8877319,1473647,-29291284,-12507580,16690915,2553332,-3132688,16400289 },
 },
 {
  { 15716668,1254266,-18472690,7446274,-8448918,6344164,-22097271,-7285580,26894937,9132066 },
  { 24158887,12938817,11085297,-8177598,-28063478,-4457083,-30576463,64452,-6817084,-2692882 },
  { 13488534,7794716,22236231,5989356,25426474,-12578208,2350710,-3418511,-4688006,2364226 },
 },
 {
  { 16335052,9132434,25640582,6678888,1725628,8517937,-11807024,-11697457,15445875,-7798101 },
  { 29004207,-7867081,28661402,-640412,-12794003,-7943086,31863255,-4135540,-278050,-15759279 },
  { -6122061,-14866665,-28614905,14569919,-10857999,-3591829,10343412,-6976290,-29828287,-10815811 },
 },
 {
  { 27081650,3463984,14099042,-4517604,1616303,-6205604,29542636,15372179,17293797,960709 },
  { 20263915,11434237,-5765435,11236810,13505955,-10857102,-16111345,6493122,-19384511,7639714 },
  { -2830798,-14839232,25403038,-8215196,-8317012,-16173699,18006287,-16043750,29994677,-15808121 },
 },
 {
  { 9769828,5202651,-24157398,-13631392,-28051003,-11561624,-24613141,-13860782,-31184575,709464 },
  { 12286395,13076066,-21775189,-1176622,-25003198,4057652,-32018128,-8890874,16102007,13205847 },
  { 13733362,5599946,10557076,3195751,-5557991,8536970,-25540170,8525972,10151379,10394400 },
 },
 {
  { 4024660,-16137551,22436262,12276534,-9099015,-2686099,19698229,11743039,-33302334,8934414 },
  { -15879800,-4525240,-8580747,-2934061,14634845,-698278,-9449077,3137094,-11536886,11721158 },
  { 17555939,-5013938,8268606,2331751,-22738815,9761013,9319229,8835153,-9205489,-1280045 },
 },
 {
  { -461409,-7830014,20614118,16688288,-7514766,-4807119,22300304,505429,6108462,-6183415 },
  { -5070281,12367917,-30663534,3234473,32617080,-8422642,29880583,-13483331,-26898490,-7867459 },
  { -31975283,5726539,26934134,10237677,-3173717,-605053,24199304,3795095,7592688,-14992079 },
 },
 {
  { 21594432,-14964228,17466408,-4077222,32537084,2739898,6407723,12018833,-28256052,4298412 },
  { -20650503,-11961496,-27236275,570498,3767144,-1717540,13891942,-1569194,13717174,10805743 },
  { -14676630,-15644296,15287174,11927123,24177847,-8175568,-796431,14860609,-26938930,-5863836 },
 },
},
{
 {
  { 12962541,5311799,-10060768,11658280,18855286,-7954201,13286263,-12808704,-4381056,9882022 },
  { 18512079,11319350,-20123124,15090309,18818594,5271736,-22727904,3666879,-23967430,-3299429 },
  { -6789020,-3146043,16192429,13241070,15898607,-14206114,-10084880,-6661110,-2403099,5276065 },
 },
 {
  { 30169808,-5317648,26306206,-11750859,27814964,7069267,7152851,3684982,1449224,13082861 },
  { 10342826,3098505,2119311,193222,25702612,12233820,23697382,15056736,-21016438,-8202000 },
  { -33150110,3261608,22745853,7948688,19370557,-15177665,-26171976,6482814,-10300080,-11060101 },
 },
 {
  { 32869458,-5408545,25609743,15678670,-10687769,-15471071,26112421,2521008,-22664288,6904815 },
  { 29506923,4457497,3377935,-9796444,-30510046,12935080,1561737,3841096,-29003639,-6657642 },
  { 10340844,-6630377,-18656632,-2278430,12621151,-13339055,30878497,-11824370,-25584551,5181966 },
 },
 {
  { 25940115,-12658025,17324188,-10307374,-8671468,15029094,24396252,-16450922,-2322852,-12388574 },
  { -21765684,9916823,-1300409,4079498,-1028346,11909559,1782390,12641087,20603771,-6561742 },
  { -18882287,-11673380,24849422,11501709,13161720,-4768874,1925523,11914390,4662781,7820689 },
 },
 {
  { 12241050,-425982,8132691,9393934,32846760,-1599620,29749456,12172924,16136752,15264020 },
  { -10349955,-14680563,-8211979,2330220,-17662549,-14545780,10658213,6671822,19012087,3772772 },
  { 3753511,-3421066,10617074,2028709,14841030,-6721664,28718732,-15762884,20527771,12988982 },
 },
 {
  { -14822485,-5797269,-3707987,12689773,-898983,-10914866,-24183046,-10564943,3299665,-12424953 },
  { -16777703,-15253301,-9642417,4978983,3308785,8755439,6943197,6461331,-25583147,8991218 },
  { -17226263,1816362,-1673288,-6086439,31783888,-8175991,-32948145,7417950,-30242287,1507265 },
 },
 {
  { 29692663,6829891,-10498800,4334896,20945975,-11906496,-28887608,8209391,14606362,-10647073 },
  { -3481570,8707081,32188102,5672294,22096700,1711240,-33020695,9761487,4170404,-2085325 },
  { -11587470,14855945,-4127778,-1531857,-26649089,15084046,22186522,16002000,-14276837,-8400798 },
 },
 {
  { -4811456,13761029,-31703877,-2483919,-3312471,7869047,-7113572,-9620092,13240845,10965870 },
  { -7742563,-8256762,-14768334,-13656260,-23232383,12387166,4498947,14147411,29514390,4302863 },
  { -13413405,-12407859,20757302,-13801832,14785143,8976368,-5061276,-2144373,17846988,-13971927 },
 },
},
{
 {
  { -2244452,-754728,-4597030,-1066309,-6247172,1455299,-21647728,-9214789,-5222701,12650267 },
  { -9906797,-16070310,21134160,12198166,-27064575,708126,387813,13770293,-19134326,10958663 },
  { 22470984,12369526,23446014,-5441109,-21520802,-9698723,-11772496,-11574455,-25083830,4271862 },
 },
 {
  { -25169565,-10053642,-19909332,15361595,-5984358,2159192,75375,-4278529,-32526221,8469673 },
  { 15854970,4148314,-8893890,7259002,11666551,13824734,-30531198,2697372,24154791,-9460943 },
  { 15446137,-15806644,29759747,14019369,30811221,-9610191,-31582008,12840104,24913809,9815020 },
 },
 {
  { -4709286,-5614269,-31841498,-12288893,-14443537,10799414,-9103676,13438769,18735128,9466238 },
  { 11933045,9281483,5081055,-5183824,-2628162,-4905629,-7727821,-10896103,-22728655,16199064 },
  { 14576810,379472,-26786533,-8317236,-29426508,-10812974,-102766,1876699,30801119,2164795 },
 },
 {
  { 15995086,3199873,13672555,13712240,-19378835,-4647646,-13081610,-15496269,-13492807,1268052 },
  { -10290614,-3659039,-3286592,10948818,23037027,3794475,-3470338,-12600221,-17055369,3565904 },
  { 29210088,-9419337,-5919792,-4952785,10834811,-13327726,-16512102,-10820713,-27162222,-14030531 },
 },
 {
  { -13161890,15508588,16663704,-8156150,-28349942,9019123,-29183421,-3769423,2244111,-14001979 },
  { -5152875,-3800936,-9306475,-6071583,16243069,14684434,-25673088,-16180800,13491506,4641841 },
  { 10813417,643330,-19188515,-728916,30292062,-16600078,27548447,-7721242,14476989,-12767431 },
 },
 {
  { 10292079,9984945,6481436,8279905,-7251514,7032743,27282937,-1644259,-27912810,12651324 },
  { -31185513,-813383,22271204,11835308,10201545,15351028,17099662,3988035,21721536,-3148940 },
  { 10202177,-6545839,-31373232,-9574638,-32150642,-8119683,-12906320,3852694,13216206,14842320 },
 },
 {
  { -15815640,-10601066,-6538952,-7258995,-6984659,-6581778,-31500847,13765824,-27434397,9900184 },
  { 14465505,-13833331,-32133984,-14738873,-27443187,12990492,33046193,15796406,-7051866,-8040114 },
  { 30924417,-8279620,6359016,-12816335,16508377,9071735,-25488601,15413635,9524356,-7018878 },
 },
 {
  { 12274201,-13175547,32627641,-1785326,6736625,13267305,5237659,-5109483,15663516,4035784 },
  { -2951309,8903985,17349946,601635,-16432815,-4612556,-13732739,-15889334,-22258478,4659091 },
  { -16916263,-4952973,-30393711,-15158821,20774812,15897498,5736189,15026997,-2178256,-13455585 },
 },
},
{
 {
  { -8858980,-2219056,28571666,-10155518,-474467,-10105698,-3801496,278095,23440562,-290208 },
  { 10226241,-5928702,15139956,120818,-14867693,5218603,32937275,11551483,-16571960,-7442864 },
  { 17932739,-12437276,-24039557,10749060,11316803,7535897,22503767,5561594,-3646624,3898661 },
 },
 {
  { 7749907,-969567,-16339731,-16464,-25018111,15122143,-1573531,7152530,21831162,1245233 },
  { 26958459,-14658026,4314586,8346991,-5677764,11960072,-32589295,-620035,-30402091,-16716212 },
  { -12165896,9166947,33491384,13673479,29787085,13096535,6280834,14587357,-22338025,13987525 },
 },
 {
  { -24349909,7778775,21116000,15572597,-4833266,-5357778,-4300898,-5124639,-7469781,-2858068 },
  { 9681908,-6737123,-31951644,13591838,-6883821,386950,31622781,6439245,-14581012,4091397 },
  { -8426427,1470727,-28109679,-1596990,3978627,-5123623,-19622683,12092163,29077877,-14741988 },
 },
 {
  { 5269168,-6859726,-13230211,-8020715,25932563,1763552,-5606110,-5505881,-20017847,2357889 },
  { 32264008,-15407652,-5387735,-1160093,-2091322,-3946900,23104804,-12869908,5727338,189038 },
  { 14609123,-8954470,-6000566,-16622781,-14577387,-7743898,-26745169,10942115,-25888931,-14884697 },
 },
 {
  { 20513500,5557931,-15604613,7829531,26413943,-2019404,-21378968,7471781,13913677,-5137875 },
  { -25574376,11967826,29233242,12948236,-6754465,4713227,-8940970,14059180,12878652,8511905 },
  { -25656801,3393631,-2955415,-7075526,-2250709,9366908,-30223418,6812974,5568676,-3127656 },
 },
 {
  { 11630004,12144454,2116339,13606037,27378885,15676917,-17408753,-13504373,-14395196,8070818 },
  { 27117696,-10007378,-31282771,-5570088,1127282,12772488,-29845906,10483306,-11552749,-1028714 },
  { 10637467,-5688064,5674781,1072708,-26343588,-6982302,-1683975,9177853,-27493162,15431203 },
 },
 {
  { 20525145,10892566,-12742472,12779443,-29493034,16150075,-28240519,14943142,-15056790,-7935931 },
  { -30024462,5626926,-551567,-9981087,753598,11981191,25244767,-3239766,-3356550,9594024 },
  { -23752644,2636870,-5163910,-10103818,585134,7877383,11345683,-6492290,13352335,-10977084 },
 },
 {
  { -1931799,-5407458,3304649,-12884869,17015806,-4877091,-29783850,-7752482,-13215537,-319204 },
  { 20239939,6607058,6203985,3483793,-18386976,-779229,-20723742,15077870,-22750759,14523817 },
  { 27406042,-6041657,27423596,-4497394,4996214,10002360,-28842031,-4545494,-30172742,-4805667 },
 },
},
{
 {
  { 11374242,12660715,17861383,-12540833,10935568,1099227,-13886076,-9091740,-27727044,11358504 },
  { -12730809,10311867,1510375,10778093,-2119455,-9145702,32676003,11149336,-26123651,4985768 },
  { -19096303,341147,-6197485,-239033,15756973,-8796662,-983043,13794114,-19414307,-15621255 },
 },
 {
  { 6490081,11940286,25495923,-7726360,8668373,-8751316,3367603,6970005,-1691065,-9004790 },
  { 1656497,13457317,15370807,6364910,13605745,8362338,-19174622,-5475723,-16796596,-5031438 },
  { -22273315,-13524424,-64685,-4334223,-18605636,-10921968,-20571065,-7007978,-99853,-10237333 },
 },
 {
  { 17747465,10039260,19368299,-4050591,-20630635,-16041286,31992683,-15857976,-29260363,-5511971 },
  { 31932027,-4986141,-19612382,16366580,22023614,88450,11371999,-3744247,4882242,-10626905 },
  { 29796507,37186,19818052,10115756,-11829032,3352736,18551198,3272828,-5190932,-4162409 },
 },
 {
  { 12501286,4044383,-8612957,-13392385,-32430052,5136599,-19230378,-3529697,330070,-3659409 },
  { 6384877,2899513,17807477,7663917,-2358888,12363165,25366522,-8573892,-271295,12071499 },
  { -8365515,-4042521,25133448,-4517355,-6211027,2265927,-32769618,1936675,-5159697,3829363 },
 },
 {
  { 28425966,-5835433,-577090,-4697198,-14217555,6870930,7921550,-6567787,26333140,14267664 },
  { -11067219,11871231,27385719,-10559544,-4585914,-11189312,10004786,-8709488,-21761224,8930324 },
  { -21197785,-16396035,25654216,-1725397,12282012,11008919,1541940,4757911,-26491501,-16408940 },
 },
 {
  { 13537262,-7759490,-20604840,10961927,-5922820,-13218065,-13156584,6217254,-15943699,13814990 },
  { -17422573,15157790,18705543,29619,24409717,-260476,27361681,9257833,-1956526,-1776914 },
  { -25045300,-10191966,15366585,15166509,-13105086,8423556,-29171540,12361135,-18685978,4578290 },
 },
 {
  { 24579768,3711570,1342322,-11180126,-27005135,14124956,-22544529,14074919,21964432,8235257 },
  { -6528613,-2411497,9442966,-5925588,12025640,-1487420,-2981514,-1669206,13006806,2355433 },
  { -16304899,-13605259,-6632427,-5142349,16974359,-10911083,27202044,1719366,1141648,-12796236 },
 },
 {
  { -12863944,-13219986,-8318266,-11018091,-6810145,-4843894,13475066,-3133972,32674895,13715045 },
  { 11423335,-5468059,32344216,8962751,24989809,9241752,-13265253,16086212,-28740881,-15642093 },
  { -1409668,12530728,-6368726,10847387,19531186,-14132160,-11709148,7791794,-27245943,4383347 },
 },
},
{
 {
  { -28970898,5271447,-1266009,-9736989,-12455236,16732599,-4862407,-4906449,27193557,6245191 },
  { -15193956,5362278,-1783893,2695834,4960227,12840725,23061898,3260492,22510453,8577507 },
  { -12632451,11257346,-32692994,13548177,-721004,10879011,31168030,13952092,-29571492,-3635906 },
 },
 {
  { 3877321,-9572739,32416692,5405324,-11004407,-13656635,3759769,11935320,5611860,8164018 },
  { -16275802,14667797,15906460,12155291,-22111149,-9039718,32003002,-8832289,5773085,-8422109 },
  { -23788118,-8254300,1950875,8937633,18686727,16459170,-905725,12376320,31632953,190926 },
 },
 {
  { -24593607,-16138885,-8423991,13378746,14162407,6901328,-8288749,4508564,-25341555,-3627528 },
  { 8884438,-5884009,6023974,10104341,-6881569,-4941533,18722941,-14786005,-1672488,827625 },
  { -32720583,-16289296,-32503547,7101210,13354605,2659080,-1800575,-14108036,-24878478,1541286 },
 },
 {
  { 2901347,-1117687,3880376,-10059388,-17620940,-3612781,-21802117,-3567481,20456845,-1885033 },
  { 27019610,12299467,-13658288,-1603234,-12861660,-4861471,-19540150,-5016058,29439641,15138866 },
  { 21536104,-6626420,-32447818,-10690208,-22408077,5175814,-5420040,-16361163,7779328,109896 },
 },
 {
  { 30279744,14648750,-8044871,6425558,13639621,-743509,28698390,12180118,23177719,-554075 },
  { 26572847,3405927,-31701700,12890905,-19265668,5335866,-6493768,2378492,4439158,-13279347 },
  { -22716706,3489070,-9225266,-332753,18875722,-1140095,14819434,-12731527,-17717757,-5461437 },
 },
 {
  { -5056483,16566551,15953661,3767752,-10436499,15627060,-820954,2177225,8550082,-15114165 },
  { -18473302,16596775,-381660,15663611,22860960,15585581,-27844109,-3582739,-23260460,-8428588 },
  { -32480551,15707275,-8205912,-5652081,29464558,2713815,-22725137,15860482,-21902570,1494193 },
 },
 {
  { -19562091,-14087393,-25583872,-9299552,13127842,759709,21923482,16529112,8742704,12967017 },
  { -28464899,1553205,32536856,-10473729,-24691605,-406174,-8914625,-2933896,-29903758,15553883 },
  { 21877909,3230008,9881174,10539357,-4797115,2841332,11543572,14513274,19375923,-12647961 },
 },
 {
  { 8832269,-14495485,13253511,5137575,5037871,4078777,24880818,-6222716,2862653,9455043 },
  { 29306751,5123106,20245049,-14149889,9592566,8447059,-2077124,-2990080,15511449,4789663 },
  { -20679756,7004547,8824831,-9434977,-4045704,-3750736,-5754762,108893,23513200,16652362 },
 },
},
{
 {
  { -33256173,4144782,-4476029,-6579123,10770039,-7155542,-6650416,-12936300,-18319198,10212860 },
  { 2756081,8598110,7383731,-6859892,22312759,-1105012,21179801,2600940,-9988298,-12506466 },
  { -24645692,13317462,-30449259,-15653928,21365574,-10869657,11344424,864440,-2499677,-16710063 },
 },
 {
  { -26432803,6148329,-17184412,-14474154,18782929,-275997,-22561534,211300,2719757,4940997 },
  { -1323882,3911313,-6948744,14759765,-30027150,7851207,21690126,8518463,26699843,5276295 },
  { -13149873,-6429067,9396249,365013,24703301,-10488939,1321586,149635,-15452774,7159369 },
 },
 {
  { 9987780,-3404759,17507962,9505530,9731535,-2165514,22356009,8312176,22477218,-8403385 },
  { 18155857,-16504990,19744716,9006923,15154154,-10538976,24256460,-4864995,-22548173,9334109 },
  { 2986088,-4911893,10776628,-3473844,10620590,-7083203,-21413845,14253545,-22587149,536906 },
 },
 {
  { 4377756,8115836,24567078,15495314,11625074,13064599,7390551,10589625,10838060,-15420424 },
  { -19342404,867880,9277171,-3218459,-14431572,-1986443,19295826,-15796950,6378260,699185 },
  { 7895026,4057113,-7081772,-13077756,-17886831,-323126,-716039,15693155,-5045064,-13373962 },
 },
 {
  { -7737563,-5869402,-14566319,-7406919,11385654,13201616,31730678,-10962840,-3918636,-9669325 },
  { 10188286,-15770834,-7336361,13427543,22223443,14896287,30743455,7116568,-21786507,5427593 },
  { 696102,13206899,27047647,-10632082,15285305,-9853179,10798490,-4578720,19236243,12477404 },
 },
 {
  { -11229439,11243796,-17054270,-8040865,-788228,-8167967,-3897669,11180504,-23169516,7733644 },
  { 17800790,-14036179,-27000429,-11766671,23887827,3149671,23466177,-10538171,10322027,15313801 },
  { 26246234,11968874,32263343,-5468728,6830755,-13323031,-15794704,-101982,-24449242,10890804 },
 },
 {
  { -31365647,10271363,-12660625,-6267268,16690207,-13062544,-14982212,16484931,25180797,-5334884 },
  { -586574,10376444,-32586414,-11286356,19801893,10997610,2276632,9482883,316878,13820577 },
  { -9882808,-4510367,-2115506,16457136,-11100081,11674996,30756178,-7515054,30696930,-3712849 },
 },
 {
  { 32988917,-9603412,12499366,7910787,-10617257,-11931514,-7342816,-9985397,-32349517,7392473 },
  { -8855661,15927861,9866406,-3649411,-2396914,-16655781,-30409476,-9134995,25112947,-2926644 },
  { -2504044,-436966,25621774,-5678772,15085042,-5479877,-24884878,-13526194,5537438,-13914319 },
 },
},
{
 {
  { -11225584,2320285,-9584280,10149187,-33444663,5808648,-14876251,-1729667,31234590,6090599 },
  { -9633316,116426,26083934,2897444,-6364437,-2688086,609721,15878753,-6970405,-9034768 },
  { -27757857,247744,-15194774,-9002551,23288161,-10011936,-23869595,6503646,20650474,1804084 },
 },
 {
  { -27589786,15456424,8972517,8469608,15640622,4439847,3121995,-10329713,27842616,-202328 },
  { -15306973,2839644,22530074,10026331,4602058,5048462,28248656,5031932,-11375082,12714369 },
  { 20807691,-7270825,29286141,11421711,-27876523,-13868230,-21227475,1035546,-19733229,12796920 },
 },
 {
  { 12076899,-14301286,-8785001,-11848922,-25012791,16400684,-17591495,-12899438,3480665,-15182815 },
  { -32361549,5457597,28548107,7833186,7303070,-11953545,-24363064,-15921875,-33374054,2771025 },
  { -21389266,421932,26597266,6860826,22486084,-6737172,-17137485,-4210226,-24552282,15673397 },
 },
 {
  { -20184622,2338216,19788685,-9620956,-4001265,-8740893,-20271184,4733254,3727144,-12934448 },
  { 6120119,814863,-11794402,-622716,6812205,-15747771,2019594,7975683,31123697,-10958981 },
  { 30069250,-11435332,30434654,2958439,18399564,-976289,12296869,9204260,-16432438,9648165 },
 },
 {
  { 32705432,-1550977,30705658,7451065,-11805606,9631813,3305266,5248604,-26008332,-11377501 },
  { 17219865,2375039,-31570947,-5575615,-19459679,9219903,294711,15298639,2662509,-16297073 },
  { -1172927,-7558695,-4366770,-4287744,-21346413,-8434326,32087529,-1222777,32247248,-14389861 },
 },
 {
  { 14312628,1221556,17395390,-8700143,-4945741,-8684635,-28197744,-9637817,-16027623,-13378845 },
  { -1428825,-9678990,-9235681,6549687,-7383069,-468664,23046502,9803137,17597934,2346211 },
  { 18510800,15337574,26171504,981392,-22241552,7827556,-23491134,-11323352,3059833,-11782870 },
 },
 {
  { 10141598,6082907,17829293,-1947643,9830092,13613136,-25556636,-5544586,-33502212,3592096 },
  { 33114168,-15889352,-26525686,-13343397,33076705,8716171,1151462,1521897,-982665,-6837803 },
  { -32939165,-4255815,23947181,-324178,-33072974,-12305637,-16637686,3891704,26353178,693168 },
 },
 {
  { 30374239,1595580,-16884039,13186931,4600344,406904,9585294,-400668,31375464,14369965 },
  { -14370654,-7772529,1510301,6434173,-18784789,-6262728,32732230,-13108839,17901441,16011505 },
  { 18171223,-11934626,-12500402,15197122,-11038147,-15230035,-19172240,-16046376,8764035,12309598 },
 },
},
{
 {
  { 5975908,-5243188,-19459362,-9681747,-11541277,14015782,-23665757,1228319,17544096,-10593782 },
  { 5811932,-1715293,3442887,-2269310,-18367348,-8359541,-18044043,-15410127,-5565381,12348900 },
  { -31399660,11407555,25755363,6891399,-3256938,14872274,-24849353,8141295,-10632534,-585479 },
 },
 {
  { -12675304,694026,-5076145,13300344,14015258,-14451394,-9698672,-11329050,30944593,1130208 },
  { 8247766,-6710942,-26562381,-7709309,-14401939,-14648910,4652152,2488540,23550156,-271232 },
  { 17294316,-3788438,7026748,15626851,22990044,113481,2267737,-5908146,-408818,-137719 },
 },
 {
  { 16091085,-16253926,18599252,7340678,2137637,-1221657,-3364161,14550936,3260525,-7166271 },
  { -4910104,-13332887,18550887,10864893,-16459325,-7291596,-23028869,-13204905,-12748722,2701326 },
  { -8574695,16099415,4629974,-16340524,-20786213,-6005432,-10018363,9276971,11329923,1862132 },
 },
 {
  { 14763076,-15903608,-30918270,3689867,3511892,10313526,-21951088,12219231,-9037963,-940300 },
  { 8894987,-3446094,6150753,3013931,301220,15693451,-31981216,-2909717,-15438168,11595570 },
  { 15214962,3537601,-26238722,-14058872,4418657,-15230761,13947276,10730794,-13489462,-4363670 },
 },
 {
  { -2538306,7682793,32759013,263109,-29984731,-7955452,-22332124,-10188635,977108,699994 },
  { -12466472,4195084,-9211532,550904,-15565337,12917920,19118110,-439841,-30534533,-14337913 },
  { 31788461,-14507657,4799989,7372237,8808585,-14747943,9408237,-10051775,12493932,-5409317 },
 },
 {
  { -25680606,5260744,-19235809,-6284470,-3695942,16566087,27218280,2607121,29375955,6024730 },
  { 842132,-2794693,-4763381,-8722815,26332018,-12405641,11831880,6985184,-9940361,2854096 },
  { -4847262,-7969331,2516242,-5847713,9695691,-7221186,16512645,960770,12121869,16648078 },
 },
 {
  { -15218652,14667096,-13336229,2013717,30598287,-464137,-31504922,-7882064,20237806,2838411 },
  { -19288047,4453152,15298546,-16178388,22115043,-15972604,12544294,-13470457,1068881,-12499905 },
  { -9558883,-16518835,33238498,13506958,30505848,-1114596,-8486907,-2630053,12521378,4845654 },
 },
 {
  { -28198521,10744108,-2958380,10199664,7759311,-13088600,3409348,-873400,-6482306,-12885870 },
  { -23561822,6230156,-20382013,10655314,-24040585,-11621172,10477734,-1240216,-3113227,13974498 },
  { 12966261,15550616,-32038948,-1615346,21025980,-629444,5642325,7188737,18895762,12629579 },
 },
},
{
 {
  { 14741879,-14946887,22177208,-11721237,1279741,8058600,11758140,789443,32195181,3895677 },
  { 10758205,15755439,-4509950,9243698,-4879422,6879879,-2204575,-3566119,-8982069,4429647 },
  { -2453894,15725973,-20436342,-10410672,-5803908,-11040220,-7135870,-11642895,18047436,-15281743 },
 },
 {
  { -25173001,-11307165,29759956,11776784,-22262383,-15820455,10993114,-12850837,-17620701,-9408468 },
  { 21987233,700364,-24505048,14972008,-7774265,-5718395,32155026,2581431,-29958985,8773375 },
  { -25568350,454463,-13211935,16126715,25240068,8594567,20656846,12017935,-7874389,-13920155 },
 },
 {
  { 6028182,6263078,-31011806,-11301710,-818919,2461772,-31841174,-5468042,-1721788,-2776725 },
  { -12278994,16624277,987579,-5922598,32908203,1248608,7719845,-4166698,28408820,6816612 },
  { -10358094,-8237829,19549651,-12169222,22082623,16147817,20613181,13982702,-10339570,5067943 },
 },
 {
  { -30505967,-3821767,12074681,13582412,-19877972,2443951,-19719286,12746132,5331210,-10105944 },
  { 30528811,3601899,-1957090,4619785,-27361822,-15436388,24180793,-12570394,27679908,-1648928 },
  { 9402404,-13957065,32834043,10838634,-26580150,-13237195,26653274,-8685565,22611444,-12715406 },
 },
 {
  { 22190590,1118029,22736441,15130463,-30460692,-5991321,19189625,-4648942,4854859,6622139 },
  { -8310738,-2953450,-8262579,-3388049,-10401731,-271929,13424426,-3567227,26404409,13001963 },
  { -31241838,-15415700,-2994250,8939346,11562230,-12840670,-26064365,-11621720,-15405155,11020693 },
 },
 {
  { 1866042,-7949489,-7898649,-10301010,12483315,13477547,3175636,-12424163,28761762,1406734 },
  { -448555,-1777666,13018551,3194501,-9580420,-11161737,24760585,-4347088,25577411,-13378680 },
  { -24290378,4759345,-690653,-1852816,2066747,10693769,-29595790,9884936,-9368926,4745410 },
 },
 {
  { -9141284,6049714,-19531061,-4341411,-31260798,9944276,-15462008,-11311852,10931924,-11931931 },
  { -16561513,14112680,-8012645,4817318,-8040464,-11414606,-22853429,10856641,-20470770,13434654 },
  { 22759489,-10073434,-16766264,-1871422,13637442,-10168091,1765144,-12654326,28445307,-5364710 },
 },
 {
  { 29875063,12493613,2795536,-3786330,1710620,15181182,-10195717,-8788675,9074234,1167180 },
  { -26205683,11014233,-9842651,-2635485,-26908120,7532294,-18716888,-9535498,3843903,9367684 },
  { -10969595,-6403711,9591134,9582310,11349256,108879,16235123,8601684,-139197,4242895 },
 },
},
{
 {
  { 22092954,-13191123,-2042793,-11968512,32186753,-11517388,-6574341,2470660,-27417366,16625501 },
  { -11057722,3042016,13770083,-9257922,584236,-544855,-7770857,2602725,-27351616,14247413 },
  { 6314175,-10264892,-32772502,15957557,-10157730,168750,-8618807,14290061,27108877,-1180880 },
 },
 {
  { -8586597,-7170966,13241782,10960156,-32991015,-13794596,33547976,-11058889,-27148451,981874 },
  { 22833440,9293594,-32649448,-13618667,-9136966,14756819,-22928859,-13970780,-10479804,-16197962 },
  { -7768587,3326786,-28111797,10783824,19178761,14905060,22680049,13906969,-15933690,3797899 },
 },
 {
  { 21721356,-4212746,-12206123,9310182,-3882239,-13653110,23740224,-2709232,20491983,-8042152 },
  { 9209270,-15135055,-13256557,-6167798,-731016,15289673,25947805,15286587,30997318,-6703063 },
  { 7392032,16618386,23946583,-8039892,-13265164,-1533858,-14197445,-2321576,17649998,-250080 },
 },
 {
  { -9301088,-14193827,30609526,-3049543,-25175069,-1283752,-15241566,-9525724,-2233253,7662146 },
  { -17558673,1763594,-33114336,15908610,-30040870,-12174295,7335080,-8472199,-3174674,3440183 },
  { -19889700,-5977008,-24111293,-9688870,10799743,-16571957,40450,-4431835,4862400,1133 },
 },
 {
  { -32856209,-7873957,-5422389,14860950,-16319031,7956142,7258061,311861,-30594991,-7379421 },
  { -3773428,-1565936,28985340,7499440,24445838,9325937,29727763,16527196,18278453,15405622 },
  { -4381906,8508652,-19898366,-3674424,-5984453,15149970,-13313598,843523,-21875062,13626197 },
 },
 {
  { 2281448,-13487055,-10915418,-2609910,1879358,16164207,-10783882,3953792,13340839,15928663 },
  { 31727126,-7179855,-18437503,-8283652,2875793,-16390330,-25269894,-7014826,-23452306,5964753 },
  { 4100420,-5959452,-17179337,6017714,-18705837,12227141,-26684835,11344144,2538215,-7570755 },
 },
 {
  { -9433605,6123113,11159803,-2156608,30016280,14966241,-20474983,1485421,-629256,-15958862 },
  { -26804558,4260919,11851389,9658551,-32017107,16367492,-20205425,-13191288,11659922,-11115118 },
  { 26180396,10015009,-30844224,-8581293,5418197,9480663,2231568,-10170080,33100372,-1306171 },
 },
 {
  { 15121113,-5201871,-10389905,15427821,-27509937,-15992507,21670947,4486675,-5931810,-14466380 },
  { 16166486,-9483733,-11104130,6023908,-31926798,-1364923,2340060,-16254968,-10735770,-10039824 },
  { 28042865,-3557089,-12126526,12259706,-3717498,-6945899,6766453,-8689599,18036436,5803270 },
 },
},
{
 {
  { -817581,6763912,11803561,1585585,10958447,-2671165,23855391,4598332,-6159431,-14117438 },
  { -31031306,-14256194,17332029,-2383520,31312682,-5967183,696309,50292,-20095739,11763584 },
  { -594563,-2514283,-32234153,12643980,12650761,14811489,665117,-12613632,-19773211,-10713562 },
 },
 {
  { 30464590,-11262872,-4127476,-12734478,19835327,-7105613,-24396175,2075773,-17020157,992471 },
  { 18357185,-6994433,7766382,16342475,-29324918,411174,14578841,8080033,-11574335,-10601610 },
  { 19598397,10334610,12555054,2555664,18821899,-10339780,21873263,16014234,26224780,16452269 },
 },
 {
  { -30223925,5145196,5944548,16385966,3976735,2009897,-11377804,-7618186,-20533829,3698650 },
  { 14187449,3448569,-10636236,-10810935,-22663880,-3433596,7268410,-10890444,27394301,12015369 },
  { 19695761,16087646,28032085,12999827,6817792,11427614,20244189,-1312777,-13259127,-3402461 },
 },
 {
  { 30860103,12735208,-1888245,-4699734,-16974906,2256940,-8166013,12298312,-8550524,-10393462 },
  { -5719826,-11245325,-1910649,15569035,26642876,-7587760,-5789354,-15118654,-4976164,12651793 },
  { -2848395,9953421,11531313,-5282879,26895123,-12697089,-13118820,-16517902,9768698,-2533218 },
 },
 {
  { -24719459,1894651,-287698,-4704085,15348719,-8156530,32767513,12765450,4940095,10678226 },
  { 18860224,15980149,-18987240,-1562570,-26233012,-11071856,-7843882,13944024,-24372348,16582019 },
  { -15504260,4970268,-29893044,4175593,-20993212,-2199756,-11704054,15444560,-11003761,7989037 },
 },
 {
  { 31490452,5568061,-2412803,2182383,-32336847,4531686,-32078269,6200206,-19686113,-14800171 },
  { -17308668,-15879940,-31522777,-2831,-32887382,16375549,8680158,-16371713,28550068,-6857132 },
  { -28126887,-5688091,16837845,-1820458,-6850681,12700016,-30039981,4364038,1155602,5988841 },
 },
 {
  { 21890435,-13272907,-12624011,12154349,-7831873,15300496,23148983,-4470481,24618407,8283181 },
  { -33136107,-10512751,9975416,6841041,-31559793,16356536,3070187,-7025928,1466169,10740210 },
  { -1509399,-15488185,-13503385,-10655916,32799044,909394,-13938903,-5779719,-32164649,-15327040 },
 },
 {
  { 3960823,-14267803,-28026090,-15918051,-19404858,13146868,15567327,951507,-3260321,-573935 },
  { 24740841,5052253,-30094131,8961361,25877428,6165135,-24368180,14397372,-7380369,-6144105 },
  { -28888365,3510803,-28103278,-1158478,-11238128,-10631454,-15441463,-14453128,-1625486,-6494814 },
 },
},
{
 {
  { 793299,-9230478,8836302,-6235707,-27360908,-2369593,33152843,-4885251,-9906200,-621852 },
  { 5666233,525582,20782575,-8038419,-24538499,14657740,16099374,1468826,-6171428,-15186581 },
  { -4859255,-3779343,-2917758,-6748019,7778750,11688288,-30404353,-9871238,-1558923,-9863646 },
 },
 {
  { 10896332,-7719704,824275,472601,-19460308,3009587,25248958,14783338,-30581476,-15757844 },
  { 10566929,12612572,-31944212,11118703,-12633376,12362879,21752402,8822496,24003793,14264025 },
  { 27713862,-7355973,-11008240,9227530,27050101,2504721,23886875,-13117525,13958495,-5732453 },
 },
 {
  { -23481610,4867226,-27247128,3900521,29838369,-8212291,-31889399,-10041781,7340521,-15410068 },
  { 4646514,-8011124,-22766023,-11532654,23184553,8566613,31366726,-1381061,-15066784,-10375192 },
  { -17270517,12723032,-16993061,14878794,21619651,-6197576,27584817,3093888,-8843694,3849921 },
 },
 {
  { -9064912,2103172,25561640,-15125738,-5239824,9582958,32477045,-9017955,5002294,-15550259 },
  { -12057553,-11177906,21115585,-13365155,8808712,-12030708,16489530,13378448,-25845716,12741426 },
  { -5946367,10645103,-30911586,15390284,-3286982,-7118677,24306472,15852464,28834118,-7646072 },
 },
 {
  { -17335748,-9107057,-24531279,9434953,-8472084,-583362,-13090771,455841,20461858,5491305 },
  { 13669248,-16095482,-12481974,-10203039,-14569770,-11893198,-24995986,11293807,-28588204,-9421832 },
  { 28497928,6272777,-33022994,14470570,8906179,-1225630,18504674,-14165166,29867745,-8795943 },
 },
 {
  { -16207023,13517196,-27799630,-13697798,24009064,-6373891,-6367600,-13175392,22853429,-4012011 },
  { 24191378,16712145,-13931797,15217831,14542237,1646131,18603514,-11037887,12876623,-2112447 },
  { 17902668,4518229,-411702,-2829247,26878217,5258055,-12860753,608397,16031844,3723494 },
 },
 {
  { -28632773,12763728,-20446446,7577504,33001348,-13017745,17558842,-7872890,23896954,-4314245 },
  { -20005381,-12011952,31520464,605201,2543521,5991821,-2945064,7229064,-9919646,-8826859 },
  { 28816045,298879,-28165016,-15920938,19000928,-1665890,-12680833,-2949325,-18051778,-2082915 },
 },
 {
  { 16000882,-344896,3493092,-11447198,-29504595,-13159789,12577740,16041268,-19715240,7847707 },
  { 10151868,10572098,27312476,7922682,14825339,4723128,-32855931,-6519018,-10020567,3852848 },
  { -11430470,15697596,-21121557,-4420647,5386314,15063598,16514493,-15932110,29330899,-15076224 },
 },
},
{
 {
  { -25499735,-4378794,-15222908,-6901211,16615731,2051784,3303702,15490,-27548796,12314391 },
  { 15683520,-6003043,18109120,-9980648,15337968,-5997823,-16717435,15921866,16103996,-3731215 },
  { -23169824,-10781249,13588192,-1628807,-3798557,-1074929,-19273607,5402699,-29815713,-9841101 },
 },
 {
  { 23190676,2384583,-32714340,3462154,-29903655,-1529132,-11266856,8911517,-25205859,2739713 },
  { 21374101,-3554250,-33524649,9874411,15377179,11831242,-33529904,6134907,4931255,11987849 },
  { -7732,-2978858,-16223486,7277597,105524,-322051,-31480539,13861388,-30076310,10117930 },
 },
 {
  { -29501170,-10744872,-26163768,13051539,-25625564,5089643,-6325503,6704079,12890019,15728940 },
  { -21972360,-11771379,-951059,-4418840,14704840,2695116,903376,-10428139,12885167,8311031 },
  { -17516482,5352194,10384213,-13811658,7506451,13453191,26423267,4384730,1888765,-5435404 },
 },
 {
  { -25817338,-3107312,-13494599,-3182506,30896459,-13921729,-32251644,-12707869,-19464434,-3340243 },
  { -23607977,-2665774,-526091,4651136,5765089,4618330,6092245,14845197,17151279,-9854116 },
  { -24830458,-12733720,-15165978,10367250,-29530908,-265356,22825805,-7087279,-16866484,16176525 },
 },
 {
  { -23583256,6564961,20063689,3798228,-4740178,7359225,2006182,-10363426,-28746253,-10197509 },
  { -10626600,-4486402,-13320562,-5125317,3432136,-6393229,23632037,-1940610,32808310,1099883 },
  { 15030977,5768825,-27451236,-2887299,-6427378,-15361371,-15277896,-6809350,2051441,-15225865 },
 },
 {
  { -3362323,-7239372,7517890,9824992,23555850,295369,5148398,-14154188,-22686354,16633660 },
  { 4577086,-16752288,13249841,-15304328,19958763,-14537274,18559670,-10759549,8402478,-9864273 },
  { -28406330,-1051581,-26790155,-907698,-17212414,-11030789,9453451,-14980072,17983010,9967138 },
 },
 {
  { -25762494,6524722,26585488,9969270,24709298,1220360,-1677990,7806337,17507396,3651560 },
  { -10420457,-4118111,14584639,15971087,-15768321,8861010,26556809,-5574557,-18553322,-11357135 },
  { 2839101,14284142,4029895,3472686,14402957,12689363,-26642121,8459447,-5605463,-7621941 },
 },
 {
  { -4839289,-3535444,9744961,2871048,25113978,3187018,-25110813,-849066,17258084,-7977739 },
  { 18164541,-10595176,-17154882,-1542417,19237078,-9745295,23357533,-15217008,26908270,12150756 },
  { -30264870,-7647865,5112249,-7036672,-1499807,-6974257,43168,-5537701,-32302074,16215819 },
 },
},
{
 {
  { -6898905,9824394,-12304779,-4401089,-31397141,-6276835,32574489,12532905,-7503072,-8675347 },
  { -27343522,-16515468,-27151524,-10722951,946346,16291093,254968,7168080,21676107,-1943028 },
  { 21260961,-8424752,-16831886,-11920822,-23677961,3968121,-3651949,-6215466,-3556191,-7913075 },
 },
 {
  { 16544754,13250366,-16804428,15546242,-4583003,12757258,-2462308,-8680336,-18907032,-9662799 },
  { -2415239,-15577728,18312303,4964443,-15272530,-12653564,26820651,16690659,25459437,-4564609 },
  { -25144690,11425020,28423002,-11020557,-6144921,-15826224,9142795,-2391602,-6432418,-1644817 },
 },
 {
  { -23104652,6253476,16964147,-3768872,-25113972,-12296437,-27457225,-16344658,6335692,7249989 },
  { -30333227,13979675,7503222,-12368314,-11956721,-4621693,-30272269,2682242,25993170,-12478523 },
  { 4364628,5930691,32304656,-10044554,-8054781,15091131,22857016,-10598955,31820368,15075278 },
 },
 {
  { 31879134,-8918693,17258761,90626,-8041836,-4917709,24162788,-9650886,-17970238,12833045 },
  { 19073683,14851414,-24403169,-11860168,7625278,11091125,-19619190,2074449,-9413939,14905377 },
  { 24483667,-11935567,-2518866,-11547418,-1553130,15355506,-25282080,9253129,27628530,-7555480 },
 },
 {
  { 17597607,8340603,19355617,552187,26198470,-3176583,4593324,-9157582,-14110875,15297016 },
  { 510886,14337390,-31785257,16638632,6328095,2713355,-20217417,-11864220,8683221,2921426 },
  { 18606791,11874196,27155355,-5281482,-24031742,6265446,-25178240,-1278924,4674690,13890525 },
 },
 {
  { 13609624,13069022,-27372361,-13055908,24360586,9592974,14977157,9835105,4389687,288396 },
  { 9922506,-519394,13613107,5883594,-18758345,-434263,-12304062,8317628,23388070,16052080 },
  { 12720016,11937594,-31970060,-5028689,26900120,8561328,-20155687,-11632979,-14754271,-10812892 },
 },
 {
  { 15961858,14150409,26716931,-665832,-22794328,13603569,11829573,7467844,-28822128,929275 },
  { 11038231,-11582396,-27310482,-7316562,-10498527,-16307831,-23479533,-9371869,-21393143,2465074 },
  { 20017163,-4323226,27915242,1529148,12396362,15675764,13817261,-9658066,2463391,-4622140 },
 },
 {
  { -16358878,-12663911,-12065183,4996454,-1256422,1073572,9583558,12851107,4003896,12673717 },
  { -1731589,-15155870,-3262930,16143082,19294135,13385325,14741514,-9103726,7903886,2348101 },
  { 24536016,-16515207,12715592,-3862155,1511293,10047386,-3842346,-7129159,-28377538,10048127 },
 },
},
{
 {
  { -12622226,-6204820,30718825,2591312,-10617028,12192840,18873298,-7297090,-32297756,15221632 },
  { -26478122,-11103864,11546244,-1852483,9180880,7656409,-21343950,2095755,29769758,6593415 },
  { -31994208,-2907461,4176912,3264766,12538965,-868111,26312345,-6118678,30958054,8292160 },
 },
 {
  { 31429822,-13959116,29173532,15632448,12174511,-2760094,32808831,3977186,26143136,-3148876 },
  { 22648901,1402143,-22799984,13746059,7936347,365344,-8668633,-1674433,-3758243,-2304625 },
  { -15491917,8012313,-2514730,-12702462,-23965846,-10254029,-1612713,-1535569,-16664475,8194478 },
 },
 {
  { 27338066,-7507420,-7414224,10140405,-19026427,-6589889,27277191,8855376,28572286,3005164 },
  { 26287124,4821776,25476601,-4145903,-3764513,-15788984,-18008582,1182479,-26094821,-13079595 },
  { -7171154,3178080,23970071,6201893,-17195577,-4489192,-21876275,-13982627,32208683,-1198248 },
 },
 {
  { -16657702,2817643,-10286362,14811298,6024667,13349505,-27315504,-10497842,-27672585,-11539858 },
  { 15941029,-9405932,-21367050,8062055,31876073,-238629,-15278393,-1444429,15397331,-4130193 },
  { 8934485,-13485467,-23286397,-13423241,-32446090,14047986,31170398,-1441021,-27505566,15087184 },
 },
 {
  { -18357243,-2156491,24524913,-16677868,15520427,-6360776,-15502406,11461896,16788528,-5868942 },
  { -1947386,16013773,21750665,3714552,-17401782,-16055433,-3770287,-10323320,31322514,-11615635 },
  { 21426655,-5650218,-13648287,-5347537,-28812189,-4920970,-18275391,-14621414,13040862,-12112948 },
 },
 {
  { 11293895,12478086,-27136401,15083750,-29307421,14748872,14555558,-13417103,1613711,4896935 },
  { -25894883,15323294,-8489791,-8057900,25967126,-13425460,2825960,-4897045,-23971776,-11267415 },
  { -15924766,-5229880,-17443532,6410664,3622847,10243618,20615400,12405433,-23753030,-8436416 },
 },
 {
  { -7091295,12556208,-20191352,9025187,-17072479,4333801,4378436,2432030,23097949,-566018 },
  { 4565804,-16025654,20084412,-7842817,1724999,189254,24767264,10103221,-18512313,2424778 },
  { 366633,-11976806,8173090,-6890119,30788634,5745705,-7168678,1344109,-3642553,12412659 },
 },
 {
  { -24001791,7690286,14929416,-168257,-32210835,-13412986,24162697,-15326504,-3141501,11179385 },
  { 18289522,-14724954,8056945,16430056,-21729724,7842514,-6001441,-1486897,-18684645,-11443503 },
  { 476239,6601091,-6152790,-9723375,17503545,-4863900,27672959,13403813,11052904,5219329 },
 },
},
{
 {
  { 20678546,-8375738,-32671898,8849123,-5009758,14574752,31186971,-3973730,9014762,-8579056 },
  { -13644050,-10350239,-15962508,5075808,-1514661,-11534600,-33102500,9160280,8473550,-3256838 },
  { 24900749,14435722,17209120,-15292541,-22592275,9878983,-7689309,-16335821,-24568481,11788948 },
 },
 {
  { -3118155,-11395194,-13802089,14797441,9652448,-6845904,-20037437,10410733,-24568470,-1458691 },
  { -15659161,16736706,-22467150,10215878,-9097177,7563911,11871841,-12505194,-18513325,8464118 },
  { -23400612,8348507,-14585951,-861714,-3950205,-6373419,14325289,8628612,33313881,-8370517 },
 },
 {
  { -20186973,-4967935,22367356,5271547,-1097117,-4788838,-24805667,-10236854,-8940735,-5818269 },
  { -6948785,-1795212,-32625683,-16021179,32635414,-7374245,15989197,-12838188,28358192,-4253904 },
  { -23561781,-2799059,-32351682,-1661963,-9147719,10429267,-16637684,4072016,-5351664,5596589 },
 },
 {
  { -28236598,-3390048,12312896,6213178,3117142,16078565,29266239,2557221,1768301,15373193 },
  { -7243358,-3246960,-4593467,-7553353,-127927,-912245,-1090902,-4504991,-24660491,3442910 },
  { -30210571,5124043,14181784,8197961,18964734,-11939093,22597931,7176455,-18585478,13365930 },
 },
 {
  { -7877390,-1499958,8324673,4690079,6261860,890446,24538107,-8570186,-9689599,-3031667 },
  { 25008904,-10771599,-4305031,-9638010,16265036,15721635,683793,-11823784,15723479,-15163481 },
  { -9660625,12374379,-27006999,-7026148,-7724114,-12314514,11879682,5400171,519526,-1235876 },
 },
 {
  { 22258397,-16332233,-7869817,14613016,-22520255,-2950923,-20353881,7315967,16648397,7605640 },
  { -8081308,-8464597,-8223311,9719710,19259459,-15348212,23994942,-5281555,-9468848,4763278 },
  { -21699244,9220969,-15730624,1084137,-25476107,-2852390,31088447,-7764523,-11356529,728112 },
 },
 {
  { 26047220,-11751471,-6900323,-16521798,24092068,9158119,-4273545,-12555558,-29365436,-5498272 },
  { 17510331,-322857,5854289,8403524,17133918,-3112612,-28111007,12327945,10750447,10014012 },
  { -10312768,3936952,9156313,-8897683,16498692,-994647,-27481051,-666732,3424691,7540221 },
 },
 {
  { 30322361,-6964110,11361005,-4143317,7433304,4989748,-7071422,-16317219,-9244265,15258046 },
  { 13054562,-2779497,19155474,469045,-12482797,4566042,5631406,2711395,1062915,-5136345 },
  { -19240248,-11254599,-29509029,-7499965,-5835763,13005411,-6066489,12194497,32960380,1459310 },
 },
},
{
 {
  { 19852034,7027924,23669353,10020366,8586503,-6657907,394197,-6101885,18638003,-11174937 },
  { 31395534,15098109,26581030,8030562,-16527914,-5007134,9012486,-7584354,-6643087,-5442636 },
  { -9192165,-2347377,-1997099,4529534,25766844,607986,-13222,9677543,-32294889,-6456008 },
 },
 {
  { -2444496,-149937,29348902,8186665,1873760,12489863,-30934579,-7839692,-7852844,-8138429 },
  { -15236356,-15433509,7766470,746860,26346930,-10221762,-27333451,10754588,-9431476,5203576 },
  { 31834314,14135496,-770007,5159118,20917671,-16768096,-7467973,-7337524,31809243,7347066 },
 },
 {
  { -9606723,-11874240,20414459,13033986,13716524,-11691881,19797970,-12211255,15192876,-2087490 },
  { -12663563,-2181719,1168162,-3804809,26747877,-14138091,10609330,12694420,33473243,-13382104 },
  { 33184999,11180355,15832085,-11385430,-1633671,225884,15089336,-11023903,-6135662,14480053 },
 },
 {
  { 31308717,-5619998,31030840,-1897099,15674547,-6582883,5496208,13685227,27595050,8737275 },
  { -20318852,-15150239,10933843,-16178022,8335352,-7546022,-31008351,-12610604,26498114,66511 },
  { 22644454,-8761729,-16671776,4884562,-3105614,-13559366,30540766,-4286747,-13327787,-7515095 },
 },
 {
  { -28017847,9834845,18617207,-2681312,-3401956,-13307506,8205540,13585437,-17127465,15115439 },
  { 23711543,-672915,31206561,-8362711,6164647,-9709987,-33535882,-1426096,8236921,16492939 },
  { -23910559,-13515526,-26299483,-4503841,25005590,-7687270,19574902,10071562,6708380,-6222424 },
 },
 {
  { 2101391,-4930054,19702731,2367575,-15427167,1047675,5301017,9328700,29955601,-11678310 },
  { 3096359,9271816,-21620864,-15521844,-14847996,-7592937,-25892142,-12635595,-9917575,6216608 },
  { -32615849,338663,-25195611,2510422,-29213566,-13820213,24822830,-6146567,-26767480,7525079 },
 },
 {
  { -23066649,-13985623,16133487,-7896178,-3389565,778788,-910336,-2782495,-19386633,11994101 },
  { 21691500,-13624626,-641331,-14367021,3285881,-3483596,-25064666,9718258,-7477437,13381418 },
  { 18445390,-4202236,14979846,11622458,-1727110,-3582980,23111648,-6375247,28535282,15779576 },
 },
 {
  { 30098053,3089662,-9234387,16662135,-21306940,11308411,-14068454,12021730,9955285,-16303356 },
  { 9734894,-14576830,-7473633,-9138735,2060392,11313496,-18426029,9924399,20194861,13380996 },
  { -26378102,-7965207,-22167821,15789297,-18055342,-6168792,-1984914,15707771,26342023,10146099 },
 },
},
{
 {
  { -26016874,-219943,21339191,-41388,19745256,-2878700,-29637280,2227040,21612326,-545728 },
  { -13077387,1184228,23562814,-5970442,-20351244,-6348714,25764461,12243797,-20856566,11649658 },
  { -10031494,11262626,27384172,2271902,26947504,-15997771,39944,6114064,33514190,2333242 },
 },
 {
  { -21433588,-12421821,8119782,7219913,-21830522,-9016134,-6679750,-12670638,24350578,-13450001 },
  { -4116307,-11271533,-23886186,4843615,-30088339,690623,-31536088,-10406836,8317860,12352766 },
  { 18200138,-14475911,-33087759,-2696619,-23702521,-9102511,-23552096,-2287550,20712163,6719373 },
 },
 {
  { 26656208,6075253,-7858556,1886072,-28344043,4262326,11117530,-3763210,26224235,-3297458 },
  { -17168938,-14854097,-3395676,-16369877,-19954045,14050420,21728352,9493610,18620611,-16428628 },
  { -13323321,13325349,11432106,5964811,18609221,6062965,-5269471,-9725556,-30701573,-16479657 },
 },
 {
  { -23860538,-11233159,26961357,1640861,-32413112,-16737940,12248509,-5240639,13735342,1934062 },
  { 25089769,6742589,17081145,-13406266,21909293,-16067981,-15136294,-3765346,-21277997,5473616 },
  { 31883677,-7961101,1083432,-11572403,22828471,13290673,-7125085,12469656,29111212,-5451014 },
 },
 {
  { 24244947,-15050407,-26262976,2791540,-14997599,16666678,24367466,6388839,-10295587,452383 },
  { -25640782,-3417841,5217916,16224624,19987036,-4082269,-24236251,-5915248,15766062,8407814 },
  { -20406999,13990231,15495425,16395525,5377168,15166495,-8917023,-4388953,-8067909,2276718 },
 },
 {
  { 30157918,12924066,-17712050,9245753,19895028,3368142,-23827587,5096219,22740376,-7303417 },
  { 2041139,-14256350,7783687,13876377,-25946985,-13352459,24051124,13742383,-15637599,13295222 },
  { 33338237,-8505733,12532113,7977527,9106186,-1715251,-17720195,-4612972,-4451357,-14669444 },
 },
 {
  { -20045281,5454097,-14346548,6447146,28862071,1883651,-2469266,-4141880,7770569,9620597 },
  { 23208068,7979712,33071466,8149229,1758231,-10834995,30945528,-1694323,-33502340,-14767970 },
  { 1439958,-16270480,-1079989,-793782,4625402,10647766,-5043801,1220118,30494170,-11440799 },
 },
 {
  { -5037580,-13028295,-2970559,-3061767,15640974,-6701666,-26739026,926050,-1684339,-13333647 },
  { 13908495,-3549272,30919928,-6273825,-21521863,7989039,9021034,9078865,3353509,4033511 },
  { -29663431,-15113610,32259991,-344482,24295849,-12912123,23161163,8839127,27485041,7356032 },
 },
},
{
 {
  { 9661027,705443,11980065,-5370154,-1628543,14661173,-6346142,2625015,28431036,-16771834 },
  { -23839233,-8311415,-25945511,7480958,-17681669,-8354183,-22545972,14150565,15970762,4099461 },
  { 29262576,16756590,26350592,-8793563,8529671,-11208050,13617293,-9937143,11465739,8317062 },
 },
 {
  { -25493081,-6962928,32500200,-9419051,-23038724,-2302222,14898637,3848455,20969334,-5157516 },
  { -20384450,-14347713,-18336405,13884722,-33039454,2842114,-21610826,-3649888,11177095,14989547 },
  { -24496721,-11716016,16959896,2278463,12066309,10137771,13515641,2581286,-28487508,9930240 },
 },
 {
  { -17751622,-2097826,16544300,-13009300,-15914807,-14949081,18345767,-13403753,16291481,-5314038 },
  { -33229194,2553288,32678213,9875984,8534129,6889387,-9676774,6957617,4368891,9788741 },
  { 16660756,7281060,-10830758,12911820,20108584,-8101676,-21722536,-8613148,16250552,-11111103 },
 },
 {
  { -19765507,2390526,-16551031,14161980,1905286,6414907,4689584,10604807,-30190403,4782747 },
  { -1354539,14736941,-7367442,-13292886,7710542,-14155590,-9981571,4383045,22546403,437323 },
  { 31665577,-12180464,-16186830,1491339,-18368625,3294682,27343084,2786261,-30633590,-14097016 },
 },
 {
  { -14467279,-683715,-33374107,7448552,19294360,14334329,-19690631,2355319,-19284671,-6114373 },
  { 15121312,-15796162,6377020,-6031361,-10798111,-12957845,18952177,15496498,-29380133,11754228 },
  { -2637277,-13483075,8488727,-14303896,12728761,-1622493,7141596,11724556,22761615,-10134141 },
 },
 {
  { 16918416,11729663,-18083579,3022987,-31015732,-13339659,-28741185,-12227393,32851222,11717399 },
  { 11166634,7338049,-6722523,4531520,-29468672,-7302055,31474879,3483633,-1193175,-4030831 },
  { -185635,9921305,31456609,-13536438,-12013818,13348923,33142652,6546660,-19985279,-3948376 },
 },
 {
  { -32460596,11266712,-11197107,-7899103,31703694,3855903,-8537131,-12833048,-30772034,-15486313 },
  { -18006477,12709068,3991746,-6479188,-21491523,-10550425,-31135347,-16049879,10928917,3011958 },
  { -6957757,-15594337,31696059,334240,29576716,14796075,-30831056,-12805180,18008031,10258577 },
 },
 {
  { -22448644,15655569,7018479,-4410003,-30314266,-1201591,-1853465,1367120,25127874,6671743 },
  { 29701166,-14373934,-10878120,9279288,-17568,13127210,21382910,11042292,25838796,4642684 },
  { -20430234,14955537,-24126347,8124619,-5369288,-5990470,30468147,-13900640,18423289,4177476 },
 },
},
} ;
#endif


static void ge_select(ge_precomp *t,int pos,signed char b)
{
#ifndef CURVED25519_X64
  ge_precomp minust;
  unsigned char bnegative = negative(b);
  unsigned char babs = b - (((-bnegative) & b) << 1);

  ge_precomp_0(t);
  cmov(t,&base[pos][0],babs,1);
  cmov(t,&base[pos][1],babs,2);
  cmov(t,&base[pos][2],babs,3);
  cmov(t,&base[pos][3],babs,4);
  cmov(t,&base[pos][4],babs,5);
  cmov(t,&base[pos][5],babs,6);
  cmov(t,&base[pos][6],babs,7);
  cmov(t,&base[pos][7],babs,8);
  fe_cswap(t->yminusx, t->yplusx, bnegative);
  fe_neg(minust.xy2d,t->xy2d);
  fe_cmov(t->xy2d,minust.xy2d,bnegative);
#else
  fe_cmov_table((fe*)t, (fe*)base[pos], b);
#endif
}


/*
h = a * B
where a = a[0]+256*a[1]+...+256^31 a[31]
B is the Ed25519 base point (x,4/5) with x positive.

Preconditions:
  a[31] <= 127
*/
void ge_scalarmult_base(ge_p3 *h,const unsigned char *a)
{
  signed char e[64];
  signed char carry;
  ge_p1p1 r;
#ifndef CURVED25519_X64
  ge_p2 s;
#endif
  ge_precomp t;
  int i;

  for (i = 0;i < 32;++i) {
    e[2 * i + 0] = (a[i] >> 0) & 15;
    e[2 * i + 1] = (a[i] >> 4) & 15;
  }
  /* each e[i] is between 0 and 15 */
  /* e[63] is between 0 and 7 */

  carry = 0;
  for (i = 0;i < 63;++i) {
    e[i] += carry;
    carry = e[i] + 8;
    carry >>= 4;
    e[i] -= carry << 4;
  }
  e[63] += carry;
  /* each e[i] is between -8 and 8 */

#ifndef CURVED25519_X64
  ge_select(&t,0,e[1]);
  fe_sub(h->X, t.yplusx, t.yminusx);
  fe_add(h->Y, t.yplusx, t.yminusx);
  fe_0(h->Z);
  h->Z[0] = 4;
  fe_mul(h->T,h->X,h->Y);
  fe_add(h->X, h->X, h->X);
  fe_add(h->Y, h->Y, h->Y);

  for (i = 3;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }

  ge_p3_dbl(&r,h);  ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p2(&s,&r);
  ge_p2_dbl(&r,&s); ge_p1p1_to_p3(h,&r);

  for (i = 0;i < 64;i += 2) {
    ge_select(&t,i / 2,e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }
#else
  ge_select(&t, 0, e[0]);
  fe_sub(h->X, t.yplusx, t.yminusx);
  fe_add(h->Y, t.yplusx, t.yminusx);
  fe_0(h->Z);
  h->Z[0] = 2;
  fe_copy(h->T, t.xy2d);
  for (i = 1; i < 64; i++) {
    ge_select(&t, i, e[i]);
    ge_madd(&r,h,&t); ge_p1p1_to_p3(h,&r);
  }
#endif
}


/* ge double scalar mult */
static void slide(signed char *r,const unsigned char *a)
{
  int i;
  int b;
  int k;

  for (i = 0;i < 256;++i)
    r[i] = 1 & (a[i >> 3] >> (i & 7));

  for (i = 0;i < 256;++i)
    if (r[i]) {
      for (b = 1;b <= 6 && i + b < 256;++b) {
        if (r[i + b]) {
          if (r[i] + (r[i + b] << b) <= 15) {
            r[i] += r[i + b] << b; r[i + b] = 0;
          } else if (r[i] - (r[i + b] << b) >= -15) {
            r[i] -= r[i + b] << b;
            for (k = i + b;k < 256;++k) {
              if (!r[k]) {
                r[k] = 1;
                break;
              }
              r[k] = 0;
            }
          } else
            break;
        }
      }
    }
}

#ifdef CURVED25519_X64
static const ge_precomp Bi[8] = {
    {
        { 0x2fbc93c6f58c3b85, 0xcf932dc6fb8c0e19, 0x270b4898643d42c2, 0x7cf9d3a33d4ba65,  },
        { 0x9d103905d740913e, 0xfd399f05d140beb3, 0xa5c18434688f8a09, 0x44fd2f9298f81267,  },
        { 0xabc91205877aaa68, 0x26d9e823ccaac49e, 0x5a1b7dcbdd43598c, 0x6f117b689f0c65a8,  },
    },
    {
        { 0xaf25b0a84cee9730, 0x25a8430e8864b8a, 0xc11b50029f016732, 0x7a164e1b9a80f8f4,  },
        { 0x56611fe8a4fcd265, 0x3bd353fde5c1ba7d, 0x8131f31a214bd6bd, 0x2ab91587555bda62,  },
        { 0x14ae933f0dd0d889, 0x589423221c35da62, 0xd170e5458cf2db4c, 0x5a2826af12b9b4c6,  },
    },
    {
        { 0xa212bc4408a5bb33, 0x8d5048c3c75eed02, 0xdd1beb0c5abfec44, 0x2945ccf146e206eb,  },
        { 0x7f9182c3a447d6ba, 0xd50014d14b2729b7, 0xe33cf11cb864a087, 0x154a7e73eb1b55f3,  },
        { 0xbcbbdbf1812a8285, 0x270e0807d0bdd1fc, 0xb41b670b1bbda72d, 0x43aabe696b3bb69a,  },
    },
    {
        { 0x6b1a5cd0944ea3bf, 0x7470353ab39dc0d2, 0x71b2528228542e49, 0x461bea69283c927e,  },
        { 0xba6f2c9aaa3221b1, 0x6ca021533bba23a7, 0x9dea764f92192c3a, 0x1d6edd5d2e5317e0,  },
        { 0xf1836dc801b8b3a2, 0xb3035f47053ea49a, 0x529c41ba5877adf3, 0x7a9fbb1c6a0f90a7,  },
    },
    {
        { 0x9b2e678aa6a8632f, 0xa6509e6f51bc46c5, 0xceb233c9c686f5b5, 0x34b9ed338add7f59,  },
        { 0xf36e217e039d8064, 0x98a081b6f520419b, 0x96cbc608e75eb044, 0x49c05a51fadc9c8f,  },
        { 0x6b4e8bf9045af1b, 0xe2ff83e8a719d22f, 0xaaf6fc2993d4cf16, 0x73c172021b008b06,  },
    },
    {
        { 0x2fbf00848a802ade, 0xe5d9fecf02302e27, 0x113e847117703406, 0x4275aae2546d8faf,  },
        { 0x315f5b0249864348, 0x3ed6b36977088381, 0xa3a075556a8deb95, 0x18ab598029d5c77f,  },
        { 0xd82b2cc5fd6089e9, 0x31eb4a13282e4a4, 0x44311199b51a8622, 0x3dc65522b53df948,  },
    },
    {
        { 0xbf70c222a2007f6d, 0xbf84b39ab5bcdedb, 0x537a0e12fb07ba07, 0x234fd7eec346f241,  },
        { 0x506f013b327fbf93, 0xaefcebc99b776f6b, 0x9d12b232aaad5968, 0x267882d176024a7,  },
        { 0x5360a119732ea378, 0x2437e6b1df8dd471, 0xa2ef37f891a7e533, 0x497ba6fdaa097863,  },
    },
    {
        { 0x24cecc0313cfeaa0, 0x8648c28d189c246d, 0x2dbdbdfac1f2d4d0, 0x61e22917f12de72b,  },
        { 0x40bcd86468ccf0b, 0xd3829ba42a9910d6, 0x7508300807b25192, 0x43b5cd4218d05ebf,  },
        { 0x5d9a762f9bd0b516, 0xeb38af4e373fdeee, 0x32e5a7d93d64270, 0x511d61210ae4d842,  },
    },
};
#elif defined(CURVED25519_128BIT)
static const ge_precomp Bi[8] = {
    {
        { 0x493c6f58c3b85, 0x0df7181c325f7, 0x0f50b0b3e4cb7, 0x5329385a44c32, 0x07cf9d3a33d4b },
        { 0x03905d740913e, 0x0ba2817d673a2, 0x23e2827f4e67c, 0x133d2e0c21a34, 0x44fd2f9298f81 },
        { 0x11205877aaa68, 0x479955893d579, 0x50d66309b67a0, 0x2d42d0dbee5ee, 0x6f117b689f0c6 },
    },
    {
        { 0x5b0a84cee9730, 0x61d10c97155e4, 0x4059cc8096a10, 0x47a608da8014f, 0x7a164e1b9a80f },
        { 0x11fe8a4fcd265, 0x7bcb8374faacc, 0x52f5af4ef4d4f, 0x5314098f98d10, 0x2ab91587555bd },
        { 0x6933f0dd0d889, 0x44386bb4c4295, 0x3cb6d3162508c, 0x26368b872a2c6, 0x5a2826af12b9b },
    },
    {
        { 0x2bc4408a5bb33, 0x078ebdda05442, 0x2ffb112354123, 0x375ee8df5862d, 0x2945ccf146e20 },
        { 0x182c3a447d6ba, 0x22964e536eff2, 0x192821f540053, 0x2f9f19e788e5c, 0x154a7e73eb1b5 },
        { 0x3dbf1812a8285, 0x0fa17ba3f9797, 0x6f69cb49c3820, 0x34d5a0db3858d, 0x43aabe696b3bb },
    },
    {
        { 0x25cd0944ea3bf, 0x75673b81a4d63, 0x150b925d1c0d4, 0x13f38d9294114, 0x461bea69283c9 },
        { 0x72c9aaa3221b1, 0x267774474f74d, 0x064b0e9b28085, 0x3f04ef53b27c9, 0x1d6edd5d2e531 },
        { 0x36dc801b8b3a2, 0x0e0a7d4935e30, 0x1deb7cecc0d7d, 0x053a94e20dd2c, 0x7a9fbb1c6a0f9 },
    },
    {
        { 0x6678aa6a8632f, 0x5ea3788d8b365, 0x21bd6d6994279, 0x7ace75919e4e3, 0x34b9ed338add7 },
        { 0x6217e039d8064, 0x6dea408337e6d, 0x57ac112628206, 0x647cb65e30473, 0x49c05a51fadc9 },
        { 0x4e8bf9045af1b, 0x514e33a45e0d6, 0x7533c5b8bfe0f, 0x583557b7e14c9, 0x73c172021b008 },
    },
    {
        { 0x700848a802ade, 0x1e04605c4e5f7, 0x5c0d01b9767fb, 0x7d7889f42388b, 0x4275aae2546d8 },
        { 0x75b0249864348, 0x52ee11070262b, 0x237ae54fb5acd, 0x3bfd1d03aaab5, 0x18ab598029d5c },
        { 0x32cc5fd6089e9, 0x426505c949b05, 0x46a18880c7ad2, 0x4a4221888ccda, 0x3dc65522b53df },
    },
    {
        { 0x0c222a2007f6d, 0x356b79bdb77ee, 0x41ee81efe12ce, 0x120a9bd07097d, 0x234fd7eec346f },
        { 0x7013b327fbf93, 0x1336eeded6a0d, 0x2b565a2bbf3af, 0x253ce89591955, 0x0267882d17602 },
        { 0x0a119732ea378, 0x63bf1ba8e2a6c, 0x69f94cc90df9a, 0x431d1779bfc48, 0x497ba6fdaa097 },
    },
    {
        { 0x6cc0313cfeaa0, 0x1a313848da499, 0x7cb534219230a, 0x39596dedefd60, 0x61e22917f12de },
        { 0x3cd86468ccf0b, 0x48553221ac081, 0x6c9464b4e0a6e, 0x75fba84180403, 0x43b5cd4218d05 },
        { 0x2762f9bd0b516, 0x1c6e7fbddcbb3, 0x75909c3ace2bd, 0x42101972d3ec9, 0x511d61210ae4d },
    },
};
#else
static const ge_precomp Bi[8] = {
 {
  { 25967493,-14356035,29566456,3660896,-12694345,4014787,27544626,-11754271,-6079156,2047605 },
  { -12545711,934262,-2722910,3049990,-727428,9406986,12720692,5043384,19500929,-15469378 },
  { -8738181,4489570,9688441,-14785194,10184609,-12363380,29287919,11864899,-24514362,-4438546 },
 },
 {
  { 15636291,-9688557,24204773,-7912398,616977,-16685262,27787600,-14772189,28944400,-1550024 },
  { 16568933,4717097,-11556148,-1102322,15682896,-11807043,16354577,-11775962,7689662,11199574 },
  { 30464156,-5976125,-11779434,-15670865,23220365,15915852,7512774,10017326,-17749093,-9920357 },
 },
 {
  { 10861363,11473154,27284546,1981175,-30064349,12577861,32867885,14515107,-15438304,10819380 },
  { 4708026,6336745,20377586,9066809,-11272109,6594696,-25653668,12483688,-12668491,5581306 },
  { 19563160,16186464,-29386857,4097519,10237984,-4348115,28542350,13850243,-23678021,-15815942 },
 },
 {
  { 5153746,9909285,1723747,-2777874,30523605,5516873,19480852,5230134,-23952439,-15175766 },
  { -30269007,-3463509,7665486,10083793,28475525,1649722,20654025,16520125,30598449,7715701 },
  { 28881845,14381568,9657904,3680757,-20181635,7843316,-31400660,1370708,29794553,-1409300 },
 },
 {
  { -22518993,-6692182,14201702,-8745502,-23510406,8844726,18474211,-1361450,-13062696,13821877 },
  { -6455177,-7839871,3374702,-4740862,-27098617,-10571707,31655028,-7212327,18853322,-14220951 },
  { 4566830,-12963868,-28974889,-12240689,-7602672,-2830569,-8514358,-10431137,2207753,-3209784 },
 },
 {
  { -25154831,-4185821,29681144,7868801,-6854661,-9423865,-12437364,-663000,-31111463,-16132436 },
  { 25576264,-2703214,7349804,-11814844,16472782,9300885,3844789,15725684,171356,6466918 },
  { 23103977,13316479,9739013,-16149481,817875,-15038942,8965339,-14088058,-30714912,16193877 },
 },
 {
  { -33521811,3180713,-2394130,14003687,-16903474,-16270840,17238398,4729455,-18074513,9256800 },
  { -25182317,-4174131,32336398,5036987,-21236817,11360617,22616405,9761698,-19827198,630305 },
  { -13720693,2639453,-24237460,-7406481,9494427,-5774029,-6554551,-15960994,-2449256,-14291300 },
 },
 {
  { -3151181,-5046075,9282714,6866145,-31907062,-863023,-18940575,15033784,25105118,-7894876 },
  { -24326370,15950226,-31801215,-14592823,-11662737,-5090925,1573892,-2625887,2198790,-15804619 },
  { -3099351,10324967,-2241613,7453183,-5446979,-2735503,-13812022,-16236442,-32461234,-12290683 },
 },
} ;
#endif


/*
r = a * A + b * B
where a = a[0]+256*a[1]+...+256^31 a[31].
and b = b[0]+256*b[1]+...+256^31 b[31].
B is the Ed25519 base point (x,4/5) with x positive.
*/
int ge_double_scalarmult_vartime(ge_p2 *r, const unsigned char *a,
                                 const ge_p3 *A, const unsigned char *b)
{
  signed char aslide[256];
  signed char bslide[256];
  ge_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
  ge_p1p1 t;
  ge_p3 u;
  ge_p3 A2;
  int i;

  slide(aslide,a);
  slide(bslide,b);

  ge_p3_to_cached(&Ai[0],A);
  ge_p3_dbl(&t,A); ge_p1p1_to_p3(&A2,&t);
  ge_add(&t,&A2,&Ai[0]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[1],&u);
  ge_add(&t,&A2,&Ai[1]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[2],&u);
  ge_add(&t,&A2,&Ai[2]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[3],&u);
  ge_add(&t,&A2,&Ai[3]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[4],&u);
  ge_add(&t,&A2,&Ai[4]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[5],&u);
  ge_add(&t,&A2,&Ai[5]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[6],&u);
  ge_add(&t,&A2,&Ai[6]); ge_p1p1_to_p3(&u,&t); ge_p3_to_cached(&Ai[7],&u);

  ge_p2_0(r);

  for (i = 255;i >= 0;--i) {
    if (aslide[i] || bslide[i]) break;
  }

  for (;i >= 0;--i) {
    ge_p2_dbl(&t,r);

    if (aslide[i] > 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_add(&t,&u,&Ai[aslide[i]/2]);
    } else if (aslide[i] < 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_sub(&t,&u,&Ai[(-aslide[i])/2]);
    }

    if (bslide[i] > 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_madd(&t,&u,&Bi[bslide[i]/2]);
    } else if (bslide[i] < 0) {
      ge_p1p1_to_p3(&u,&t);
      ge_msub(&t,&u,&Bi[(-bslide[i])/2]);
    }

    ge_p1p1_to_p2(r,&t);
  }

  return 0;
}

#ifdef CURVED25519_X64
static const ge d = {
    0x75eb4dca135978a3, 0x700a4d4141d8ab, 0x8cc740797779e898, 0x52036cee2b6ffe73,
    };
#elif defined(CURVED25519_128BIT)
static const ge d = {
    0x34dca135978a3, 0x1a8283b156ebd, 0x5e7a26001c029, 0x739c663a03cbb,
    0x52036cee2b6ff
};
#else
static const ge d = {
-10913610,13857413,-15372611,6949391,114729,
-8787816,-6275908,-3247719,-18696448,-12055116
} ;
#endif


#ifdef CURVED25519_X64
static const ge sqrtm1 = {
    0xc4ee1b274a0ea0b0, 0x2f431806ad2fe478, 0x2b4d00993dfbd7a7, 0x2b8324804fc1df0b,
    };
#elif defined(CURVED25519_128BIT)
static const ge sqrtm1 = {
    0x61b274a0ea0b0, 0x0d5a5fc8f189d, 0x7ef5e9cbd0c60, 0x78595a6804c9e,
    0x2b8324804fc1d
};
#else
static const ge sqrtm1 = {
-32595792,-7943725,9377950,3500415,12389472,
-272473,-25146209,-2005654,326686,11406482
} ;
#endif


int ge_frombytes_negate_vartime(ge_p3 *h,const unsigned char *s)
{
  ge u;
  ge v;
  ge v3;
  ge vxx;
  ge check;

  fe_frombytes(h->Y,s);
  fe_1(h->Z);
  fe_sq(u,h->Y);
  fe_mul(v,u,d);
  fe_sub(u,u,h->Z);       /* u = y^2-1 */
  fe_add(v,v,h->Z);       /* v = dy^2+1 */


  fe_sq(v3,v);
  fe_mul(v3,v3,v);        /* v3 = v^3 */
  fe_sq(h->X,v3);
  fe_mul(h->X,h->X,v);
  fe_mul(h->X,h->X,u);    /* x = uv^7 */

  fe_pow22523(h->X,h->X); /* x = (uv^7)^((q-5)/8) */
  fe_mul(h->X,h->X,v3);
  fe_mul(h->X,h->X,u);    /* x = uv^3(uv^7)^((q-5)/8) */

  fe_sq(vxx,h->X);
  fe_mul(vxx,vxx,v);
  fe_sub(check,vxx,u);    /* vx^2-u */
  if (fe_isnonzero(check)) {
    fe_add(check,vxx,u);  /* vx^2+u */
    if (fe_isnonzero(check)) return -1;
    fe_mul(h->X,h->X,sqrtm1);
  }

  if (fe_isnegative(h->X) == (s[31] >> 7))
    fe_neg(h->X,h->X);

  fe_mul(h->T,h->X,h->Y);
  return 0;
}


/* ge madd */
/*
r = p + q
*/

static INLINE void ge_madd(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
{
#ifndef CURVED25519_X64
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->yplusx);
    fe_mul(r->Y,r->Y,q->yminusx);
    fe_mul(r->T,q->xy2d,p->T);
    fe_add(t0,p->Z,p->Z);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_add(r->Z,t0,r->T);
    fe_sub(r->T,t0,r->T);
#else
    fe_ge_madd(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->xy2d,
              q->yplusx, q->yminusx);
#endif
}


/* ge msub */

/*
r = p - q
*/

static INLINE void ge_msub(ge_p1p1 *r,const ge_p3 *p,const ge_precomp *q)
{
#ifndef CURVED25519_X64
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->yminusx);
    fe_mul(r->Y,r->Y,q->yplusx);
    fe_mul(r->T,q->xy2d,p->T);
    fe_add(t0,p->Z,p->Z);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_sub(r->Z,t0,r->T);
    fe_add(r->T,t0,r->T);
#else
    fe_ge_msub(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->xy2d,
              q->yplusx, q->yminusx);
#endif
}


/* ge p1p1 to p2 */
/*
r = p
*/

static void ge_p1p1_to_p2(ge_p2 *r,const ge_p1p1 *p)
{
#ifndef CURVED25519_X64
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
#else
  fe_ge_to_p2(r->X, r->Y, r->Z, p->X, p->Y, p->Z, p->T);
#endif
}


/* ge p1p1 to p3 */

/*
r = p
*/

static INLINE void ge_p1p1_to_p3(ge_p3 *r,const ge_p1p1 *p)
{
#ifndef CURVED25519_X64
  fe_mul(r->X,p->X,p->T);
  fe_mul(r->Y,p->Y,p->Z);
  fe_mul(r->Z,p->Z,p->T);
  fe_mul(r->T,p->X,p->Y);
#else
  fe_ge_to_p3(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T);
#endif
}


/* ge p2 0 */

static void ge_p2_0(ge_p2 *h)
{
  fe_0(h->X);
  fe_1(h->Y);
  fe_1(h->Z);
}


/* ge p2 dbl */

/*
r = 2 * p
*/

static INLINE void ge_p2_dbl(ge_p1p1 *r,const ge_p2 *p)
{
#ifndef CURVED25519_X64
    ge t0;
    fe_sq(r->X,p->X);
    fe_sq(r->Z,p->Y);
    fe_sq2(r->T,p->Z);
    fe_add(r->Y,p->X,p->Y);
    fe_sq(t0,r->Y);
    fe_add(r->Y,r->Z,r->X);
    fe_sub(r->Z,r->Z,r->X);
    fe_sub(r->X,t0,r->Y);
    fe_sub(r->T,r->T,r->Z);
#else
    fe_ge_dbl(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z);
#endif
}


/* ge p3 dble */

/*
r = 2 * p
*/

static void ge_p3_dbl(ge_p1p1 *r,const ge_p3 *p)
{
  ge_p2 q;
  ge_p3_to_p2(&q,p);
  ge_p2_dbl(r,&q);
}


/* ge p3 to cached */

/*
r = p
*/

#ifdef CURVED25519_X64
static const ge d2 = {
    0xebd69b9426b2f159, 0xe0149a8283b156, 0x198e80f2eef3d130, 0x2406d9dc56dffce7,
    };
#elif defined(CURVED25519_128BIT)
static const ge d2 = {
    0x69b9426b2f159, 0x35050762add7a, 0x3cf44c0038052, 0x6738cc7407977,
    0x2406d9dc56dff
};
#else
static const ge d2 = {
-21827239,-5839606,-30745221,13898782,229458,
15978800,-12551817,-6495438,29715968,9444199
} ;
#endif


static INLINE void ge_p3_to_cached(ge_cached *r,const ge_p3 *p)
{
  fe_add(r->YplusX,p->Y,p->X);
  fe_sub(r->YminusX,p->Y,p->X);
  fe_copy(r->Z,p->Z);
  fe_mul(r->T2d,p->T,d2);
}


/* ge p3 to p2 */
/*
r = p
*/

static void ge_p3_to_p2(ge_p2 *r,const ge_p3 *p)
{
  fe_copy(r->X,p->X);
  fe_copy(r->Y,p->Y);
  fe_copy(r->Z,p->Z);
}


/* ge p3 tobytes */
void ge_p3_tobytes(unsigned char *s,const ge_p3 *h)
{
  ge recip;
  ge x;
  ge y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= fe_isnegative(x) << 7;
}


#ifndef CURVED25519_X64
/* ge_precomp_0 */
static void ge_precomp_0(ge_precomp *h)
{
  fe_1(h->yplusx);
  fe_1(h->yminusx);
  fe_0(h->xy2d);
}
#endif


/* ge_sub */
/*
r = p - q
*/

static INLINE void ge_sub(ge_p1p1 *r,const ge_p3 *p,const ge_cached *q)
{
#ifndef CURVED25519_X64
    ge t0;
    fe_add(r->X,p->Y,p->X);
    fe_sub(r->Y,p->Y,p->X);
    fe_mul(r->Z,r->X,q->YminusX);
    fe_mul(r->Y,r->Y,q->YplusX);
    fe_mul(r->T,q->T2d,p->T);
    fe_mul(r->X,p->Z,q->Z);
    fe_add(t0,r->X,r->X);
    fe_sub(r->X,r->Z,r->Y);
    fe_add(r->Y,r->Z,r->Y);
    fe_sub(r->Z,t0,r->T);
    fe_add(r->T,t0,r->T);
#else
    fe_ge_sub(r->X, r->Y, r->Z, r->T, p->X, p->Y, p->Z, p->T, q->Z, q->T2d,
              q->YplusX, q->YminusX);
#endif
}


/* ge tobytes */
void ge_tobytes(unsigned char *s,const ge_p2 *h)
{
  ge recip;
  ge x;
  ge y;

  fe_invert(recip,h->Z);
  fe_mul(x,h->X,recip);
  fe_mul(y,h->Y,recip);
  fe_tobytes(s,y);
  s[31] ^= fe_isnegative(x) << 7;
}

#endif /* !ED25519_SMALL */
#endif /* HAVE_ED25519 */
