/* sp_int.c
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

/* Implementation by Sean Parkinson. */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#ifdef WOLFSSL_SP_MATH

#include <wolfssl/wolfcrypt/sp_int.h>

/* Initialize the big number to be zero.
 *
 * a  SP integer.
 * returns MP_OKAY always.
 */
int sp_init(sp_int* a)
{
    a->used = 0;
    a->size = SP_INT_DIGITS;

    return MP_OKAY;
}

/* Initialize up to six big numbers to be zero.
 *
 * a  SP integer.
 * b  SP integer.
 * c  SP integer.
 * d  SP integer.
 * e  SP integer.
 * f  SP integer.
 * returns MP_OKAY always.
 */
int sp_init_multi(sp_int* a, sp_int* b, sp_int* c, sp_int* d, sp_int* e,
                  sp_int* f)
{
    if (a != NULL) {
        a->used = 0;
        a->size = SP_INT_DIGITS;
    }
    if (b != NULL) {
        b->used = 0;
        b->size = SP_INT_DIGITS;
    }
    if (c != NULL) {
        c->used = 0;
        c->size = SP_INT_DIGITS;
    }
    if (d != NULL) {
        d->used = 0;
        d->size = SP_INT_DIGITS;
    }
    if (e != NULL) {
        e->used = 0;
        e->size = SP_INT_DIGITS;
    }
    if (f != NULL) {
        f->used = 0;
        f->size = SP_INT_DIGITS;
    }

    return MP_OKAY;
}

/* Clear the data from the big number and set to zero.
 *
 * a  SP integer.
 */
void sp_clear(sp_int* a)
{
    int i;

    for (i=0; i<a->used; i++)
        a->dp[i] = 0;
    a->used = 0;
}

/* Calculate the number of 8-bit values required to represent the big number.
 *
 * a  SP integer.
 * returns the count.
 */
int sp_unsigned_bin_size(sp_int* a)
{
    int size = sp_count_bits(a);
    return (size + 7) / 8;
}

/* Convert a number as an array of bytes in big-endian format to a big number.
 *
 * a     SP integer.
 * in    Array of bytes.
 * inSz  Number of data bytes in array.
 * returns MP_OKAY always.
 */
int sp_read_unsigned_bin(sp_int* a, const byte* in, word32 inSz)
{
    int i, j = 0, s = 0;

    a->dp[0] = 0;
    for (i = inSz-1; i >= 0; i--) {
        a->dp[j] |= ((sp_int_digit)in[i]) << s;
        if (s == DIGIT_BIT - 8) {
            a->dp[++j] = 0;
            s = 0;
        }
        else if (s > DIGIT_BIT - 8) {
            s = DIGIT_BIT - s;
            if (j + 1 >= a->size)
                break;
            a->dp[++j] = in[i] >> s;
            s = 8 - s;
        }
        else
            s += 8;
    }

    a->used = j + 1;
    if (a->dp[j] == 0)
        a->used--;

    for (j++; j < a->size; j++)
        a->dp[j] = 0;

    return MP_OKAY;
}

/* Convert a number as string in big-endian format to a big number.
 * Only supports base-16 (hexadecimal).
 * Negative values not supported.
 *
 * a      SP integer.
 * in     NUL terminated string.
 * radix  Number of values in a digit.
 * returns BAD_FUNC_ARG when radix not supported or value is negative, MP_VAL
 * when a character is not valid and MP_OKAY otherwise.
 */
int sp_read_radix(sp_int* a, const char* in, int radix)
{
    int     i, j, k;
    char    ch;

    if (radix != 16)
        return BAD_FUNC_ARG;

    if (*in == '-') {
        return BAD_FUNC_ARG;
    }

    j = 0;
    k = 0;
    a->dp[0] = 0;
    for (i = (int)(XSTRLEN(in) - 1); i >= 0; i--) {
        ch = in[i];
        if (ch >= '0' && ch <= '9')
            ch -= '0';
        else if (ch >= 'A' && ch <= 'F')
            ch -= 'A' - 10;
        else if (ch >= 'a' && ch <= 'f')
            ch -= 'a' - 10;
        else
            return MP_VAL;

        a->dp[k] |= ((sp_int_digit)ch) << j;
        j += 4;
        if (j == DIGIT_BIT && k < SP_INT_DIGITS)
            a->dp[++k] = 0;
        j &= DIGIT_BIT - 1;
    }

    a->used = k + 1;
    if (a->dp[k] == 0)
        a->used--;

    for (k++; k < a->size; k++)
        a->dp[k] = 0;

    return MP_OKAY;
}

/* Compare two big numbers.
 *
 * a  SP integer.
 * b  SP integer.
 * returns MP_GT if a is greater than b, MP_LT if a is less than b and MP_EQ
 * when a equals b.
 */
int sp_cmp(sp_int* a, sp_int* b)
{
    int i;

    if (a->used > b->used)
        return MP_GT;
    else if (a->used < b->used)
        return MP_LT;

    for (i = a->used - 1; i >= 0; i--) {
        if (a->dp[i] > b->dp[i])
            return MP_GT;
        else if (a->dp[i] < b->dp[i])
            return MP_LT;
    }
    return MP_EQ;
}

/* Count the number of bits in the big number.
 *
 * a  SP integer.
 * returns the number of bits.
 */
int sp_count_bits(sp_int* a)
{
    int r = 0;
    sp_int_digit d;

    r = a->used - 1;
    while (r >= 0 && a->dp[r] == 0)
        r--;
    if (r < 0)
        r = 0;
    else {
        d = a->dp[r];
        r *= DIGIT_BIT;
        while (d != 0) {
            r++;
            d >>= 1;
        }
    }

    return r;
}

/* Determine if the most significant byte of the encoded big number as the top
 * bit set.
 *
 * a  SP integer.
 * returns 1 when the top bit is set and 0 otherwise.
 */
int sp_leading_bit(sp_int* a)
{
    int bit = 0;
    sp_int_digit d;

    if (a->used > 0) {
        d = a->dp[a->used - 1];
        while (d > (sp_int_digit)0xff)
            d >>= 8;
        bit = (int)(d >> 7);
    }

    return bit;
}

/* Convert the big number to an array of bytes in big-endian format.
 * The array must be large enough for encoded number - use mp_unsigned_bin_size
 * to calculate the number of bytes required.
 *
 * a  SP integer.
 * returns MP_OKAY always.
 */
int sp_to_unsigned_bin(sp_int* a, byte* out)
{
    int i, j, b;

    j = sp_unsigned_bin_size(a) - 1;
    for (i=0; j>=0; i++) {
        for (b = 0; b < SP_WORD_SIZE; b += 8) {
            out[j--] = a->dp[i] >> b;
            if (j < 0)
                break;
        }
    }

    return MP_OKAY;
}

/* Ensure the data in the big number is zeroed.
 *
 * a  SP integer.
 */
void sp_forcezero(sp_int* a)
{
    ForceZero(a->dp, a->used * sizeof(sp_int_digit));
    a->used = 0;
}

/* Copy value of big number a into b.
 *
 * a  SP integer.
 * b  SP integer.
 * returns MP_OKAY always.
 */
int sp_copy(sp_int* a, sp_int* b)
{
    if (a != b) {
        XMEMCPY(b->dp, a->dp, a->used * sizeof(sp_int_digit));
        b->used = a->used;
    }
    return MP_OKAY;
}

/* Set the big number to be the value of the digit.
 *
 * a  SP integer.
 * d  Digit to be set.
 * returns MP_OKAY always.
 */
int sp_set(sp_int* a, sp_int_digit d)
{
    a->dp[0] = d;
    a->used = 1;
    return MP_OKAY;
}

/* Checks whether the value of the big number is zero.
 *
 * a  SP integer.
 * returns 1 when value is zero and 0 otherwise.
 */
int sp_iszero(sp_int* a)
{
    return a->used == 0;
}

/* Recalculate the number of digits used.
 *
 * a  SP integer.
 */
void sp_clamp(sp_int* a)
{
    int i;

    for (i = a->used - 1; i >= 0 && a->dp[i] == 0; i--) {
    }
    a->used = i + 1;
}

/* Grow big number to be able to hold l digits.
 * This function does nothing as the number of digits is fixed.
 *
 * a  SP integer.
 * l  Number of digits.
 * retuns MP_MEM if the number of digits requested is more than available and
 * MP_OKAY otherwise.
 */
int sp_grow(sp_int* a, int l)
{
    if (l > a->size)
        return MP_MEM;
    (void)a;
    (void)l;
    return MP_OKAY;
}

/* Sub a one digit number from the big number.
 *
 * a  SP integer.
 * d  Digit to subtract.
 * r  SP integer - result.
 * returns MP_OKAY always.
 */
int sp_sub_d(sp_int* a, sp_int_digit d, sp_int* r)
{
    int i = 0;

    r->used = a->used;
    r->dp[0] = a->dp[0] - d;
    if (r->dp[i] > a->dp[i]) {
        for (; i < a->used; i++) {
            r->dp[i] = a->dp[i] - 1;
            if (r->dp[i] != (sp_int_digit)-1)
               break;
        }
    }
    for (; i < a->used; i++)
        r->dp[i] = a->dp[i];

    return MP_OKAY;
}

/* Compare a one digit number with a big number.
 *
 * a  SP integer.
 * d  Digit to compare with.
 * returns MP_GT if a is greater than d, MP_LT if a is less than d and MP_EQ
 * when a equals d.
 */
int sp_cmp_d(sp_int *a, sp_int_digit d)
{
    /* special case for zero*/
    if (a->used == 0) {
        if (d == 0)
            return MP_EQ;
        else
            return MP_LT;
    }
    else if (a->used > 1)
        return MP_GT;

    /* compare the only digit of a to d */
    if (a->dp[0] > d)
        return MP_GT;
    else if (a->dp[0] < d)
        return MP_LT;
    return MP_EQ;
}

/* Left shift the number by number of bits.
 * Bits may be larger than the word size.
 *
 * a  SP integer.
 * n  Number of bits to shift.
 * returns MP_OKAY always.
 */
static int sp_lshb(sp_int* a, int n)
{
    int i;

    if (n >= SP_WORD_SIZE) {
        sp_lshd(a, n / SP_WORD_SIZE);
        n %= SP_WORD_SIZE;
    }

    if (n == 0)
        return MP_OKAY;

    a->dp[a->used] = 0;
    for (i = a->used - 1; i >= 0; i--) {
        a->dp[i+1] |= a->dp[i] >> (SP_WORD_SIZE - n);
        a->dp[i] = a->dp[i] << n;
    }
    if (a->dp[a->used] != 0)
        a->used++;

    return MP_OKAY;
}

/* Subtract two large numbers into result: r = a - b
 * a must be greater than b.
 *
 * a  SP integer.
 * b  SP integer.
 * r  SP integer.
 * returns MP_OKAY always.
 */
static int sp_sub(sp_int* a, sp_int* b, sp_int* r)
{
    int i;
    sp_int_digit c = 0;
    sp_int_digit t;

    for (i = 0; i < a->used && i < b->used; i++) {
        t = a->dp[i] - b->dp[i] - c;
        if (c == 0)
            c = t > a->dp[i];
        else
            c = t >= a->dp[i];
        r->dp[i] = t;
    }
    for (; i < a->used; i++) {
        r->dp[i] = a->dp[i] - c;
        c = r->dp[i] == (sp_int_digit)-1;
    }
    r->used = i;
    sp_clamp(r);

    return MP_OKAY;
}

/* Calculate the r = a mod m.
 *
 * a  SP integer.
 * m  SP integer.
 * r  SP integer.
 * returns MP_OKAY always.
 */
int sp_mod(sp_int* a, sp_int* m, sp_int* r)
{
    sp_int t;
    int mBits = sp_count_bits(m);
    int rBits;

    if (a != r)
        sp_copy(a, r);
    sp_init(&t);

    rBits = sp_count_bits(r);
    while (rBits > mBits) {
        sp_copy(m, &t);
        sp_lshb(&t, rBits - mBits);

        if (sp_cmp(&t, r) == MP_GT) {
            sp_copy(m, &t);
            sp_lshb(&t, rBits - mBits - 1);
        }
        sp_sub(r, &t, r);

        rBits = sp_count_bits(r);
    }
    if (sp_cmp(r, m) != MP_LT)
        sp_sub(r, m, r);

    return MP_OKAY;
}

#if defined(USE_FAST_MATH) || !defined(NO_BIG_INT)
/* Clear all data in the big number and sets value to zero.
 *
 * a  SP integer.
 */
void sp_zero(sp_int* a)
{
    XMEMSET(a->dp, 0, a->size);
    a->used = 0;
}

/* Add a one digit number to the big number.
 *
 * a  SP integer.
 * d  Digit to add.
 * r  SP integer - result.
 * returns MP_OKAY always.
 */
int sp_add_d(sp_int* a, sp_int_digit d, sp_int* r)
{
    int i = 0;

    r->used = a->used;
    r->dp[0] = a->dp[0] + d;
    if (r->dp[i] < a->dp[i]) {
        for (; i < a->used; i++) {
            r->dp[i] = a->dp[i] + 1;
            if (r->dp[i] != 0)
               break;
        }

        if (i == a->used) {
            r->used++;
            r->dp[i] = 1;
        }
    }
    for (; i < a->used; i++)
        r->dp[i] = a->dp[i];

    return MP_OKAY;
}

/* Left shift the big number by a number of digits.
 * WIll chop off digits overflowing maximum size.
 *
 * a  SP integer.
 * s  Number of digits to shift.
 * returns MP_OKAY always.
 */
int sp_lshd(sp_int* a, int s)
{
    if (a->used + s > a->size)
        a->used = a->size - s;

    XMEMMOVE(a->dp + s, a->dp, a->used * SP_INT_DIGITS);
    a->used += s;
    XMEMSET(a->dp, 0, s * sizeof(sp_int_digit));

    return MP_OKAY;
}
#endif

#ifndef NO_PWDBASED
/* Add two large numbers into result: r = a + b
 *
 * a  SP integer.
 * b  SP integer.
 * r  SP integer.
 * returns MP_OKAY always.
 */
int sp_add(sp_int* a, sp_int* b, sp_int* r)
{
    int i;
    sp_digit c = 0;
    sp_digit t;

    for (i = 0; i < a->used && i < b->used; i++) {
        t = a->dp[i] + b->dp[i] + c;
        if (c == 0)
            c = t < a->dp[i];
        else
            c = t <= a->dp[i];
        r->dp[i] = t;
    }
    for (; i < a->used; i++) {
        r->dp[i] = a->dp[i] + c;
        c = r->dp[i] == 0;
    }
    for (; i < b->used; i++) {
        r->dp[i] = b->dp[i] + c;
        c = r->dp[i] == 0;
    }
    r->dp[i] = c;
    r->used = (int)(i + c);

    return MP_OKAY;
}
#endif

#if !defined(USE_FAST_MATH)
/* Returns the run time settings.
 *
 * returns the settings value.
 */
word32 CheckRunTimeSettings(void)
{
    return CTC_SETTINGS;
}
#endif

#endif

