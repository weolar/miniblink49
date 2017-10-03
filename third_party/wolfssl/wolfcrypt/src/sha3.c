/* sha3.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_XILINX_CRYPT)

#include <wolfssl/wolfcrypt/sha3.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

/* fips wrapper calls, user can call direct */
#ifdef HAVE_FIPS

    int wc_InitSha3_224(Sha3* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha3_224_fips(sha);
    }
    int wc_Sha3_224_Update(Sha3* sha, const byte* data, word32 len)
    {
        if (sha == NULL ||  (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }
        return Sha3_224_Update_fips(sha, data, len);
    }
    int wc_Sha3_224_Final(Sha3* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }
        return Sha3_224_Final_fips(sha, out);
    }
    void wc_Sha3_224_Free(Sha3* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

    int wc_InitSha3_256(Sha3* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha3_256_fips(sha);
    }
    int wc_Sha3_256_Update(Sha3* sha, const byte* data, word32 len)
    {
        if (sha == NULL ||  (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }
        return Sha3_256_Update_fips(sha, data, len);
    }
    int wc_Sha3_256_Final(Sha3* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }
        return Sha3_256_Final_fips(sha, out);
    }
    void wc_Sha3_256_Free(Sha3* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

    int wc_InitSha3_384(Sha3* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha3_384_fips(sha);
    }
    int wc_Sha3_384_Update(Sha3* sha, const byte* data, word32 len)
    {
        if (sha == NULL ||  (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }
        return Sha3_384_Update_fips(sha, data, len);
    }
    int wc_Sha3_384_Final(Sha3* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }
        return Sha3_384_Final_fips(sha, out);
    }
    void wc_Sha3_384_Free(Sha3* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

    int wc_InitSha3_512(Sha3* sha, void* heap, int devId)
    {
        (void)heap;
        (void)devId;
        if (sha == NULL) {
            return BAD_FUNC_ARG;
        }
        return InitSha3_512_fips(sha);
    }
    int wc_Sha3_512_Update(Sha3* sha, const byte* data, word32 len)
    {
        if (sha == NULL ||  (data == NULL && len > 0)) {
            return BAD_FUNC_ARG;
        }
        return Sha3_512_Update_fips(sha, data, len);
    }
    int wc_Sha3_512_Final(Sha3* sha, byte* out)
    {
        if (sha == NULL || out == NULL) {
            return BAD_FUNC_ARG;
        }
        return Sha3_512_Final_fips(sha, out);
    }
    void wc_Sha3_512_Free(Sha3* sha)
    {
        (void)sha;
        /* Not supported in FIPS */
    }

#else /* else build without fips */


#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#ifdef WOLFSSL_SHA3_SMALL
/* Rotate a 64-bit value left.
 *
 * a  Number to rotate left.
 * r  Number od bits to rotate left.
 * returns the rotated number.
 */
#define ROTL64(a, n)    (((a)<<(n))|((a)>>(64-(n))))

/* An array of values to XOR for block operation. */
static const word64 hash_keccak_r[24] =
{
    0x0000000000000001UL, 0x0000000000008082UL,
    0x800000000000808aUL, 0x8000000080008000UL,
    0x000000000000808bUL, 0x0000000080000001UL,
    0x8000000080008081UL, 0x8000000000008009UL,
    0x000000000000008aUL, 0x0000000000000088UL,
    0x0000000080008009UL, 0x000000008000000aUL,
    0x000000008000808bUL, 0x800000000000008bUL,
    0x8000000000008089UL, 0x8000000000008003UL,
    0x8000000000008002UL, 0x8000000000000080UL,
    0x000000000000800aUL, 0x800000008000000aUL,
    0x8000000080008081UL, 0x8000000000008080UL,
    0x0000000080000001UL, 0x8000000080008008UL
};

/* Indeces used in swap and rotate operation. */
#define K_I_0   10
#define K_I_1    7
#define K_I_2   11
#define K_I_3   17
#define K_I_4   18
#define K_I_5    3
#define K_I_6    5
#define K_I_7   16
#define K_I_8    8
#define K_I_9   21
#define K_I_10  24
#define K_I_11   4
#define K_I_12  15
#define K_I_13  23
#define K_I_14  19
#define K_I_15  13
#define K_I_16  12
#define K_I_17   2
#define K_I_18  20
#define K_I_19  14
#define K_I_20  22
#define K_I_21   9
#define K_I_22   6
#define K_I_23   1

/* Number of bits to rotate in swap and rotate operation. */
#define K_R_0    1
#define K_R_1    3
#define K_R_2    6
#define K_R_3   10
#define K_R_4   15
#define K_R_5   21
#define K_R_6   28
#define K_R_7   36
#define K_R_8   45
#define K_R_9   55
#define K_R_10   2
#define K_R_11  14
#define K_R_12  27
#define K_R_13  41
#define K_R_14  56
#define K_R_15   8
#define K_R_16  25
#define K_R_17  43
#define K_R_18  62
#define K_R_19  18
#define K_R_20  39
#define K_R_21  61
#define K_R_22  20
#define K_R_23  44

/* Swap and rotate left operation.
 *
 * s   The state.
 * t1  Temporary value.
 * t2  Second temporary value.
 * i   The index of the loop.
 */
#define SWAP_ROTL(s, t1, t2, i)                                         \
do                                                                      \
{                                                                       \
    t2 = s[K_I_##i]; s[K_I_##i] = ROTL64(t1, K_R_##i);                  \
}                                                                       \
while (0)

/* Mix the XOR of the column's values into each number by column.
 *
 * s  The state.
 * b  Temporary array of XORed column values.
 * x  The index of the column.
 * t  Temporary variable.
 */
#define COL_MIX(s, b, x, t)                                             \
do                                                                      \
{                                                                       \
    for (x = 0; x < 5; x++)                                             \
        b[x] = s[x + 0] ^ s[x + 5] ^ s[x + 10] ^ s[x + 15] ^ s[x + 20]; \
    for (x = 0; x < 5; x++)                                             \
    {                                                                   \
        t = b[(x + 4) % 5] ^ ROTL64(b[(x + 1) % 5], 1);                 \
        s[x +  0] ^= t;                                                 \
        s[x +  5] ^= t;                                                 \
        s[x + 10] ^= t;                                                 \
        s[x + 15] ^= t;                                                 \
        s[x + 20] ^= t;                                                 \
    }                                                                   \
}                                                                       \
while (0)

#ifdef SHA3_BY_SPEC
/* Mix the row values.
 * BMI1 has ANDN instruction ((~a) & b) - Haswell and above.
 *
 * s   The state.
 * b   Temporary array of XORed row values.
 * y   The index of the row to work on.
 * x   The index of the column.
 * t0  Temporary variable.
 * t1  Temporary variable.
 */
#define ROW_MIX(s, b, y, x, t0, t1)                                     \
do                                                                      \
{                                                                       \
    for (y = 0; y < 5; y++)                                             \
    {                                                                   \
        for (x = 0; x < 5; x++)                                         \
            b[x] = s[y * 5 + x];                                        \
        for (x = 0; x < 5; x++)                                         \
           s[y * 5 + x] = b[x] ^ (~b[(x + 1) % 5] & b[(x + 2) % 5]);    \
    }                                                                   \
}                                                                       \
while (0)
#else
/* Mix the row values.
 * a ^ (~b & c) == a ^ (c & (b ^ c)) == (a ^ b) ^ (b | c)
 *
 * s   The state.
 * b   Temporary array of XORed row values.
 * y   The index of the row to work on.
 * x   The index of the column.
 * t0  Temporary variable.
 * t1  Temporary variable.
 */
#define ROW_MIX(s, b, y, x, t12, t34)                                   \
do                                                                      \
{                                                                       \
    for (y = 0; y < 5; y++)                                             \
    {                                                                   \
        for (x = 0; x < 5; x++)                                         \
            b[x] = s[y * 5 + x];                                        \
        t12 = (b[1] ^ b[2]); t34 = (b[3] ^ b[4]);                       \
        s[y * 5 + 0] = b[0] ^ (b[2] &  t12);                            \
        s[y * 5 + 1] =  t12 ^ (b[2] | b[3]);                            \
        s[y * 5 + 2] = b[2] ^ (b[4] &  t34);                            \
        s[y * 5 + 3] =  t34 ^ (b[4] | b[0]);                            \
        s[y * 5 + 4] = b[4] ^ (b[1] & (b[0] ^ b[1]));                   \
    }                                                                   \
}                                                                       \
while (0)
#endif

/* The block operation performed on the state.
 *
 * s  The state.
 */
static void BlockSha3(word64 *s)
{
    byte i, x, y;
    word64 t0, t1;
    word64 b[5];

    for (i = 0; i < 24; i++)
    {
        COL_MIX(s, b, x, t0);

        t0 = s[1];
        SWAP_ROTL(s, t0, t1,  0);
        SWAP_ROTL(s, t1, t0,  1);
        SWAP_ROTL(s, t0, t1,  2);
        SWAP_ROTL(s, t1, t0,  3);
        SWAP_ROTL(s, t0, t1,  4);
        SWAP_ROTL(s, t1, t0,  5);
        SWAP_ROTL(s, t0, t1,  6);
        SWAP_ROTL(s, t1, t0,  7);
        SWAP_ROTL(s, t0, t1,  8);
        SWAP_ROTL(s, t1, t0,  9);
        SWAP_ROTL(s, t0, t1, 10);
        SWAP_ROTL(s, t1, t0, 11);
        SWAP_ROTL(s, t0, t1, 12);
        SWAP_ROTL(s, t1, t0, 13);
        SWAP_ROTL(s, t0, t1, 14);
        SWAP_ROTL(s, t1, t0, 15);
        SWAP_ROTL(s, t0, t1, 16);
        SWAP_ROTL(s, t1, t0, 17);
        SWAP_ROTL(s, t0, t1, 18);
        SWAP_ROTL(s, t1, t0, 19);
        SWAP_ROTL(s, t0, t1, 20);
        SWAP_ROTL(s, t1, t0, 21);
        SWAP_ROTL(s, t0, t1, 22);
        SWAP_ROTL(s, t1, t0, 23);

        ROW_MIX(s, b, y, x, t0, t1);

        s[0] ^= hash_keccak_r[i];
    }
}
#else
#include "sha3_long.i"
#endif

/* Convert the array of bytes, in little-endian order, to a 64-bit integer.
 *
 * a  Array of bytes.
 * returns a 64-bit integer.
 */
static word64 Load64BitBigEndian(const byte* a)
{
#ifdef BIG_ENDIAN_ORDER
    word64 n = 0;
    int i;

    for (i = 0; i < 8; i++)
        n |= (word64)a[i] << (8 * i);

    return n;
#else
    return *(word64*)a;
#endif
}

/* Initialize the state for a SHA3-224 hash operation.
 *
 * sha3   Sha3 object holding state.
 * returns 0 on success.
 */
static int InitSha3(Sha3* sha3)
{
    int i;

    for (i = 0; i < 25; i++)
        sha3->s[i] = 0;
    sha3->i = 0;

    return 0;
}

/* Update the SHA-3 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * p     Number of 64-bit numbers in a block of data to process.
 * returns 0 on success.
 */
static int Sha3Update(Sha3* sha3, const byte* data, word32 len, byte p)
{
    byte i;
    byte l;
    byte *t;

    if (sha3->i > 0)
    {
        l = p * 8 - sha3->i;
        if (l > len)
            l = len;

        t = &sha3->t[sha3->i];
        for (i = 0; i < l; i++)
            t[i] = data[i];
        data += i;
        len -= i; 
        sha3->i += i;

        if (sha3->i == p * 8)
        {   
            for (i = 0; i < p; i++)
                sha3->s[i] ^= Load64BitBigEndian(sha3->t + 8 * i);
            BlockSha3(sha3->s);
            sha3->i = 0;
        }
    }
    while (len >= p * 8)
    {
        for (i = 0; i < p; i++)
            sha3->s[i] ^= Load64BitBigEndian(data + 8 * i);
        BlockSha3(sha3->s);
        len -= p * 8;
        data += p * 8;
    }
    for (i = 0; i < len; i++)
        sha3->t[i] = data[i];
    sha3->i += i;

    return 0;
}

/* Calculate the SHA-3 hash based on all the message data seen.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int Sha3Final(Sha3* sha3, byte* hash, byte p, byte l)
{
    byte i;
    byte *s8 = (byte *)sha3->s;

    sha3->t[p * 8 - 1]  = 0x00;
    sha3->t[  sha3->i]  = 0x06;
    sha3->t[p * 8 - 1] |= 0x80;
    for (i=sha3->i + 1; i < p * 8 - 1; i++)
        sha3->t[i] = 0;
    for (i = 0; i < p; i++)
        sha3->s[i] ^= Load64BitBigEndian(sha3->t + 8 * i);
    BlockSha3(sha3->s);
#if defined(BIG_ENDIAN_ORDER)
    ByteReverseWords64(sha3->s, sha3->s, ((l+7)/8)*8);
#endif
    for (i = 0; i < l; i++)
        hash[i] = s8[i];

    return 0;
}

/* Initialize the state for a SHA-3 hash operation.
 *
 * sha3   Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
static int wc_InitSha3(Sha3* sha3, void* heap, int devId)
{
    int ret = 0;

    if (sha3 == NULL)
        return BAD_FUNC_ARG;

    sha3->heap = heap;
    ret = InitSha3(sha3);
    if (ret != 0)
        return ret;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    ret = wolfAsync_DevCtxInit(&sha3->asyncDev,
                        WOLFSSL_ASYNC_MARKER_SHA3, sha3->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}

/* Update the SHA-3 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * p     Number of 64-bit numbers in a block of data to process.
 * returns 0 on success.
 */
static int wc_Sha3Update(Sha3* sha3, const byte* data, word32 len, byte p)
{
    int ret = 0;

    if (sha3 == NULL || (data == NULL && len > 0)) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA3) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha3(&sha3->asyncDev, NULL, data, len);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    Sha3Update(sha3, data, len, p);

    return ret;
}

/* Calculate the SHA-3 hash based on all the message data seen.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int wc_Sha3Final(Sha3* sha3, byte* hash, byte p, byte len)
{
    int ret;

    if (sha3 == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3->asyncDev.marker == WOLFSSL_ASYNC_MARKER_SHA3) {
    #if defined(HAVE_INTEL_QA)
        return IntelQaSymSha3(&sha3->asyncDev, hash, NULL,
                              SHA3_DIGEST_SIZE);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = Sha3Final(sha3, hash, p, len);
    if (ret != 0)
        return ret;

    return InitSha3(sha3);  /* reset state */
}

/* Dispose of any dynamically allocated data from the SHA3-384 operation.
 * (Required for async ops.)
 *
 * sha3  Sha3 object holding state.
 * returns 0 on success.
 */
static void wc_Sha3Free(Sha3* sha3)
{
    (void)sha3;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_SHA3)
    if (sha3 == NULL)
        return;

    wolfAsync_DevCtxFree(&sha3->asyncDev, WOLFSSL_ASYNC_MARKER_SHA3);
#endif /* WOLFSSL_ASYNC_CRYPT */
}
#endif /* HAVE_FIPS */

/* Copy the state of the SHA3 operation.
 *
 * src  Sha3 object holding state top copy.
 * dst  Sha3 object to copy into.
 * returns 0 on success.
 */
static int wc_Sha3Copy(Sha3* src, Sha3* dst)
{
    int ret = 0;

    if (src == NULL || dst == NULL)
        return BAD_FUNC_ARG;

    XMEMCPY(dst, src, sizeof(Sha3));

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfAsync_DevCopy(&src->asyncDev, &dst->asyncDev);
#endif

    return ret;
}

/* Calculate the SHA3-224 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * p     Number of 64-bit numbers in a block of data to process.
 * len   Number of bytes in output.
 * returns 0 on success.
 */
static int wc_Sha3GetHash(Sha3* sha3, byte* hash, byte p, byte len)
{
    int ret;
    Sha3 tmpSha3;

    if (sha3 == NULL || hash == NULL)
        return BAD_FUNC_ARG;

    ret = wc_Sha3Copy(sha3, &tmpSha3);
    if (ret == 0) {
        ret = wc_Sha3Final(&tmpSha3, hash, p, len);
    }
    return ret;
}


/* Initialize the state for a SHA3-224 hash operation.
 *
 * sha3   Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
WOLFSSL_API int wc_InitSha3_224(Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-224 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_224_Update(Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, SHA3_224_COUNT);
}

/* Calculate the SHA3-224 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_224_Final(Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, SHA3_224_COUNT, SHA3_224_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-224 operation.
 * (Required for async ops.)
 *
 * sha3  Sha3 object holding state.
 * returns 0 on success.
 */
WOLFSSL_API void wc_Sha3_224_Free(Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-224 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 28 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_224_GetHash(Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, SHA3_224_COUNT, SHA3_224_DIGEST_SIZE);
}

/* Copy the state of the SHA3-224 operation.
 *
 * src  Sha3 object holding state top copy.
 * dst  Sha3 object to copy into.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_224_Copy(Sha3* src, Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-256 hash operation.
 *
 * sha3   Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
WOLFSSL_API int wc_InitSha3_256(Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-256 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_256_Update(Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, SHA3_256_COUNT);
}

/* Calculate the SHA3-256 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 32 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_256_Final(Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, SHA3_256_COUNT, SHA3_256_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-256 operation.
 * (Required for async ops.)
 *
 * sha3  Sha3 object holding state.
 * returns 0 on success.
 */
WOLFSSL_API void wc_Sha3_256_Free(Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-256 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 32 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_256_GetHash(Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, SHA3_256_COUNT, SHA3_256_DIGEST_SIZE);
}

/* Copy the state of the SHA3-256 operation.
 *
 * src  Sha3 object holding state top copy.
 * dst  Sha3 object to copy into.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_256_Copy(Sha3* src, Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-384 hash operation.
 *
 * sha3   Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
WOLFSSL_API int wc_InitSha3_384(Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-384 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_384_Update(Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, SHA3_384_COUNT);
}

/* Calculate the SHA3-384 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 48 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_384_Final(Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, SHA3_384_COUNT, SHA3_384_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-384 operation.
 * (Required for async ops.)
 *
 * sha3  Sha3 object holding state.
 * returns 0 on success.
 */
WOLFSSL_API void wc_Sha3_384_Free(Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-384 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 48 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_384_GetHash(Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, SHA3_384_COUNT, SHA3_384_DIGEST_SIZE);
}

/* Copy the state of the SHA3-384 operation.
 *
 * src  Sha3 object holding state top copy.
 * dst  Sha3 object to copy into.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_384_Copy(Sha3* src, Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}


/* Initialize the state for a SHA3-512 hash operation.
 *
 * sha3   Sha3 object holding state.
 * heap   Heap reference for dynamic memory allocation. (Used in async ops.)
 * devId  Device identifier for asynchronous operation.
 * returns 0 on success.
 */
WOLFSSL_API int wc_InitSha3_512(Sha3* sha3, void* heap, int devId)
{
    return wc_InitSha3(sha3, heap, devId);
}

/* Update the SHA3-512 hash state with message data.
 *
 * sha3  Sha3 object holding state.
 * data  Message data to be hashed.
 * len   Length of the message data.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_512_Update(Sha3* sha3, const byte* data, word32 len)
{
    return wc_Sha3Update(sha3, data, len, SHA3_512_COUNT);
}

/* Calculate the SHA3-512 hash based on all the message data seen.
 * The state is initialized ready for a new message to hash.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_512_Final(Sha3* sha3, byte* hash)
{
    return wc_Sha3Final(sha3, hash, SHA3_512_COUNT, SHA3_512_DIGEST_SIZE);
}

/* Dispose of any dynamically allocated data from the SHA3-512 operation.
 * (Required for async ops.)
 *
 * sha3  Sha3 object holding state.
 * returns 0 on success.
 */
WOLFSSL_API void wc_Sha3_512_Free(Sha3* sha3)
{
    wc_Sha3Free(sha3);
}

/* Calculate the SHA3-512 hash based on all the message data so far.
 * More message data can be added, after this operation, using the current
 * state.
 *
 * sha3  Sha3 object holding state.
 * hash  Buffer to hold the hash result. Must be at least 64 bytes.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_512_GetHash(Sha3* sha3, byte* hash)
{
    return wc_Sha3GetHash(sha3, hash, SHA3_512_COUNT, SHA3_512_DIGEST_SIZE);
}

/* Copy the state of the SHA3-512 operation.
 *
 * src  Sha3 object holding state top copy.
 * dst  Sha3 object to copy into.
 * returns 0 on success.
 */
WOLFSSL_API int wc_Sha3_512_Copy(Sha3* src, Sha3* dst)
{
    return wc_Sha3Copy(src, dst);
}

#endif /* WOLFSSL_SHA3 */
