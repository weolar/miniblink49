/* sp_int.h
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


#ifndef WOLF_CRYPT_SP_INT_H
#define WOLF_CRYPT_SP_INT_H

#include <stdint.h>
#include <limits.h>

#ifdef WOLFSSL_SP_X86_64_ASM
    #define SP_WORD_SIZE 64

    #define HAVE_INTEL_AVX1
    #define HAVE_INTEL_AVX2
#elif defined(WOLFSSL_SP_ARM64_ASM)
    #define SP_WORD_SIZE 64
#elif defined(WOLFSSL_SP_ARM32_ASM)
    #define SP_WORD_SIZE 32
#endif

#ifndef SP_WORD_SIZE
    #if defined(NO_64BIT) || !defined(HAVE___UINT128_T)
        #define SP_WORD_SIZE 32
    #else
        #define SP_WORD_SIZE 64
    #endif
#endif

#ifndef WOLFSSL_SP_ASM
  #if SP_WORD_SIZE == 32
    typedef int32_t sp_digit;
    typedef uint32_t sp_int_digit;
  #elif SP_WORD_SIZE == 64
    typedef int64_t sp_digit;
    typedef uint64_t sp_int_digit;
    typedef unsigned long uint128_t __attribute__ ((mode(TI)));
    typedef long int128_t __attribute__ ((mode(TI)));
  #else
    #error Word size not defined
  #endif
#else
  #if SP_WORD_SIZE == 32
    typedef uint32_t sp_digit;
    typedef uint32_t sp_int_digit;
  #elif SP_WORD_SIZE == 64
    typedef uint64_t sp_digit;
    typedef uint64_t sp_int_digit;
    typedef unsigned long uint128_t __attribute__ ((mode(TI)));
    typedef long int128_t __attribute__ ((mode(TI)));
  #else
    #error Word size not defined
  #endif
#endif

#ifdef WOLFSSL_SP_MATH
#include <wolfssl/wolfcrypt/random.h>

#ifndef MIN
   #define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
   #define MAX(x,y) ((x)>(y)?(x):(y))
#endif

#ifdef WOLFSSL_PUBLIC_MP
    #define MP_API   WOLFSSL_API
#else
    #define MP_API   WOLFSSL_LOCAL
#endif

#if !defined(WOLFSSL_HAVE_SP_RSA) && !defined(WOLFSSL_HAVE_SP_DH)
    #if !defined(NO_PWDBASED) && defined(WOLFSSL_SHA512)
        #define SP_INT_DIGITS        ((512 + SP_WORD_SIZE) / SP_WORD_SIZE)
    #else
        #define SP_INT_DIGITS        ((256 + SP_WORD_SIZE) / SP_WORD_SIZE)
    #endif
#elif !defined(WOLFSSL_SP_NO_3072)
    #define SP_INT_DIGITS        ((2048 + SP_WORD_SIZE) / SP_WORD_SIZE)
#else
    #define SP_INT_DIGITS        ((3072 + SP_WORD_SIZE) / SP_WORD_SIZE)
#endif

#define sp_isodd(a) (a->used != 0 && (a->dp[0] & 1))

typedef struct sp_int {
    sp_int_digit dp[SP_INT_DIGITS];
    int size;
    int used;
} sp_int;


MP_API int sp_init(sp_int* a);
MP_API int sp_init_multi(sp_int* a, sp_int* b, sp_int* c, sp_int* d,
                         sp_int* e, sp_int* f);
MP_API void sp_clear(sp_int* a);
MP_API int sp_unsigned_bin_size(sp_int* a);
MP_API int sp_read_unsigned_bin(sp_int* a, const byte* in, word32 inSz);
MP_API int sp_read_radix(sp_int* a, const char* in, int radix);
MP_API int sp_cmp(sp_int* a, sp_int* b);
MP_API int sp_count_bits(sp_int* a);
MP_API int sp_leading_bit(sp_int* a);
MP_API int sp_to_unsigned_bin(sp_int* a, byte* in);
MP_API void sp_forcezero(sp_int* a);
MP_API int sp_copy(sp_int* a, sp_int* b);
MP_API int sp_set(sp_int* a, sp_int_digit d);
MP_API int sp_iszero(sp_int* a);
MP_API void sp_clamp(sp_int* a);
MP_API int sp_grow(sp_int* a, int l);
MP_API int sp_sub_d(sp_int* a, sp_int_digit d, sp_int* r);
MP_API int sp_cmp_d(sp_int* a, sp_int_digit d);
MP_API int sp_mod(sp_int* a, sp_int* m, sp_int* r);
MP_API void sp_zero(sp_int* a);
MP_API int sp_add_d(sp_int* a, sp_int_digit d, sp_int* r);
MP_API int sp_lshd(sp_int* a, int s);
MP_API int sp_add(sp_int* a, sp_int* b, sp_int* r);

typedef sp_int mp_int;
typedef sp_digit mp_digit;

#define MP_OKAY    0
#define MP_NO      0
#define MP_YES     1

#define MP_RADIX_HEX     16

#define MP_GT    1
#define MP_EQ    0
#define MP_LT    -1

#define MP_MEM   -2
#define MP_VAL   -3

#define DIGIT_BIT  SP_WORD_SIZE

#define CheckFastMathSettings() 1

#define mp_free(a)

#define mp_init                 sp_init
#define mp_init_multi           sp_init_multi
#define mp_clear                sp_clear
#define mp_read_unsigned_bin    sp_read_unsigned_bin
#define mp_unsigned_bin_size    sp_unsigned_bin_size
#define mp_read_radix           sp_read_radix
#define mp_cmp                  sp_cmp
#define mp_count_bits           sp_count_bits
#define mp_leading_bit          sp_leading_bit
#define mp_to_unsigned_bin      sp_to_unsigned_bin
#define mp_forcezero            sp_forcezero
#define mp_copy                 sp_copy
#define mp_set                  sp_set
#define mp_iszero               sp_iszero
#define mp_clamp                sp_clamp
#define mp_grow                 sp_grow
#define mp_sub_d                sp_sub_d
#define mp_cmp_d                sp_cmp_d
#define mp_mod                  sp_mod
#define mp_zero                 sp_zero
#define mp_add_d                sp_add_d
#define mp_lshd                 sp_lshd
#define mp_add                  sp_add
#define mp_isodd                sp_isodd

#define MP_INT_DEFINED

#include <wolfssl/wolfcrypt/wolfmath.h>
#endif

#endif /* WOLF_CRYPT_SP_H */

