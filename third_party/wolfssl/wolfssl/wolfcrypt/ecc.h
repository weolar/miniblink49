/* ecc.h
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


#ifndef WOLF_CRYPT_ECC_H
#define WOLF_CRYPT_ECC_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_ECC

#include <wolfssl/wolfcrypt/integer.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef HAVE_X963_KDF
    #include <wolfssl/wolfcrypt/hash.h>
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
    #ifdef WOLFSSL_CERT_GEN
        #include <wolfssl/wolfcrypt/asn.h>
    #endif
#endif

#ifdef WOLFSSL_ATECC508A
    #include <wolfssl/wolfcrypt/port/atmel/atmel.h>
#endif /* WOLFSSL_ATECC508A */


#ifdef __cplusplus
    extern "C" {
#endif


/* Enable curve B parameter if needed */
#if defined(HAVE_COMP_KEY) || defined(ECC_CACHE_CURVE)
    #ifndef USE_ECC_B_PARAM /* Allow someone to force enable */
        #define USE_ECC_B_PARAM
    #endif
#endif


/* Use this as the key->idx if a custom ecc_set is used for key->dp */
#define ECC_CUSTOM_IDX    (-1)


/* Determine max ECC bits based on enabled curves */
#if defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)
    #define MAX_ECC_BITS    521
#elif defined(HAVE_ECC512)
    #define MAX_ECC_BITS    512
#elif defined(HAVE_ECC384)
    #define MAX_ECC_BITS    384
#elif defined(HAVE_ECC320)
    #define MAX_ECC_BITS    320
#elif defined(HAVE_ECC239)
    #define MAX_ECC_BITS    239
#elif defined(HAVE_ECC224)
    #define MAX_ECC_BITS    224
#elif !defined(NO_ECC256)
    #define MAX_ECC_BITS    256
#elif defined(HAVE_ECC192)
    #define MAX_ECC_BITS    192
#elif defined(HAVE_ECC160)
    #define MAX_ECC_BITS    160
#elif defined(HAVE_ECC128)
    #define MAX_ECC_BITS    128
#elif defined(HAVE_ECC112)
    #define MAX_ECC_BITS    112
#endif

/* calculate max ECC bytes */
#if ((MAX_ECC_BITS * 2) % 8) == 0
    #define MAX_ECC_BYTES     (MAX_ECC_BITS / 8)
#else
    /* add byte if not aligned */
    #define MAX_ECC_BYTES     ((MAX_ECC_BITS / 8) + 1)
#endif


enum {
    ECC_PUBLICKEY       = 1,
    ECC_PRIVATEKEY      = 2,
    ECC_PRIVATEKEY_ONLY = 3,
    ECC_MAXNAME     = 16,   /* MAX CURVE NAME LENGTH */
    SIG_HEADER_SZ   =  6,   /* ECC signature header size */
    ECC_BUFSIZE     = 256,  /* for exported keys temp buffer */
    ECC_MINSIZE     = 20,   /* MIN Private Key size */
    ECC_MAXSIZE     = 66,   /* MAX Private Key size */
    ECC_MAXSIZE_GEN = 74,   /* MAX Buffer size required when generating ECC keys*/
    ECC_MAX_PAD_SZ  = 4,    /* ECC maximum padding size */
    ECC_MAX_OID_LEN = 16,
    ECC_MAX_SIG_SIZE= ((MAX_ECC_BYTES * 2) + ECC_MAX_PAD_SZ + SIG_HEADER_SZ)
};

/* Curve Types */
typedef enum ecc_curve_id {
    ECC_CURVE_INVALID = -1,
    ECC_CURVE_DEF = 0, /* NIST or SECP */

    /* NIST Prime Curves */
    ECC_SECP192R1,
    ECC_PRIME192V2,
    ECC_PRIME192V3,
    ECC_PRIME239V1,
    ECC_PRIME239V2,
    ECC_PRIME239V3,
    ECC_SECP256R1,

    /* SECP Curves */
    ECC_SECP112R1,
    ECC_SECP112R2,
    ECC_SECP128R1,
    ECC_SECP128R2,
    ECC_SECP160R1,
    ECC_SECP160R2,
    ECC_SECP224R1,
    ECC_SECP384R1,
    ECC_SECP521R1,

    /* Koblitz */
    ECC_SECP160K1,
    ECC_SECP192K1,
    ECC_SECP224K1,
    ECC_SECP256K1,

    /* Brainpool Curves */
    ECC_BRAINPOOLP160R1,
    ECC_BRAINPOOLP192R1,
    ECC_BRAINPOOLP224R1,
    ECC_BRAINPOOLP256R1,
    ECC_BRAINPOOLP320R1,
    ECC_BRAINPOOLP384R1,
    ECC_BRAINPOOLP512R1,

    /* Twisted Edwards Curves */
#ifdef HAVE_CURVE25519
    ECC_X25519,
#endif
#ifdef HAVE_X448
    ECC_X448,
#endif

#ifdef WOLFSSL_CUSTOM_CURVES
    ECC_CURVE_CUSTOM,
#endif
} ecc_curve_id;

#ifdef HAVE_OID_ENCODING
typedef word16 ecc_oid_t;
#else
typedef byte   ecc_oid_t;
    /* OID encoded with ASN scheme:
        first element = (oid[0] * 40) + oid[1]
        if any element > 127 then MSB 0x80 indicates additional byte */
#endif

/* ECC set type defined a GF(p) curve */
typedef struct ecc_set_type {
    int size;             /* The size of the curve in octets */
    int id;               /* id of this curve */
    const char* name;     /* name of this curve */
    const char* prime;    /* prime that defines the field, curve is in (hex) */
    const char* Af;       /* fields A param (hex) */
    const char* Bf;       /* fields B param (hex) */
    const char* order;    /* order of the curve (hex) */
    const char* Gx;       /* x coordinate of the base point on curve (hex) */
    const char* Gy;       /* y coordinate of the base point on curve (hex) */
    const ecc_oid_t* oid;
    word32      oidSz;
    word32      oidSum;    /* sum of encoded OID bytes */
    int         cofactor;
} ecc_set_type;


#ifdef ALT_ECC_SIZE

/* Note on ALT_ECC_SIZE:
 * The fast math code uses an array of a fixed size to store the big integers.
 * By default, the array is big enough for RSA keys. There is a size,
 * FP_MAX_BITS which can be used to make the array smaller when one wants ECC
 * but not RSA. Some people want fast math sized for both RSA and ECC, where
 * ECC won't use as much as RSA. The flag ALT_ECC_SIZE switches in an alternate
 * ecc_point structure that uses an alternate fp_int that has a shorter array
 * of fp_digits.
 *
 * Now, without ALT_ECC_SIZE, the ecc_point has three single item arrays of
 * mp_ints for the components of the point. With ALT_ECC_SIZE, the components
 * of the point are pointers that are set to each of a three item array of
 * alt_fp_ints. While an mp_int will have 4096 bits of digit inside the
 * structure, the alt_fp_int will only have 528 bits. A size value was added
 * in the ALT case, as well, and is set by mp_init() and alt_fp_init(). The
 * functions fp_zero() and fp_copy() use the size parameter. An int needs to
 * be initialized before using it instead of just fp_zeroing it, the init will
 * call zero. FP_MAX_BITS_ECC defaults to 528, but can be set to change the
 * number of bits used in the alternate FP_INT.
 *
 * Do not enable ALT_ECC_SIZE and disable fast math in the configuration.
 */

#ifndef USE_FAST_MATH
    #error USE_FAST_MATH must be defined to use ALT_ECC_SIZE
#endif

/* determine max bits required for ECC math */
#ifndef FP_MAX_BITS_ECC
    /* check alignment */
    #if ((MAX_ECC_BITS * 2) % DIGIT_BIT) == 0
        /* max bits is double */
        #define FP_MAX_BITS_ECC     (MAX_ECC_BITS * 2)
    #else
        /* max bits is doubled, plus one digit of fudge */
        #define FP_MAX_BITS_ECC     ((MAX_ECC_BITS * 2) + DIGIT_BIT)
    #endif
#else
    /* verify alignment */
    #if FP_MAX_BITS_ECC % CHAR_BIT
       #error FP_MAX_BITS_ECC must be a multiple of CHAR_BIT
    #endif
#endif

/* determine buffer size */
#define FP_SIZE_ECC    (FP_MAX_BITS_ECC/DIGIT_BIT)


/* This needs to match the size of the fp_int struct, except the
 * fp_digit array will be shorter. */
typedef struct alt_fp_int {
    int used, sign, size;
    fp_digit dp[FP_SIZE_ECC];
} alt_fp_int;
#endif /* ALT_ECC_SIZE */


/* A point on an ECC curve, stored in Jacbobian format such that (x,y,z) =>
   (x/z^2, y/z^3, 1) when interpreted as affine */
typedef struct {
#ifndef ALT_ECC_SIZE
    mp_int x[1];        /* The x coordinate */
    mp_int y[1];        /* The y coordinate */
    mp_int z[1];        /* The z coordinate */
#else
    mp_int* x;        /* The x coordinate */
    mp_int* y;        /* The y coordinate */
    mp_int* z;        /* The z coordinate */
    alt_fp_int xyz[3];
#endif
} ecc_point;

/* ECC Flags */
enum {
    WC_ECC_FLAG_NONE = 0x00,
#ifdef HAVE_ECC_CDH
    WC_ECC_FLAG_COFACTOR = 0x01,
#endif
};

/* An ECC Key */
struct ecc_key {
    int type;           /* Public or Private */
    int idx;            /* Index into the ecc_sets[] for the parameters of
                           this curve if -1, this key is using user supplied
                           curve in dp */
    int    state;
    word32 flags;
    const ecc_set_type* dp;     /* domain parameters, either points to NIST
                                   curves (idx >= 0) or user supplied */
    void* heap;         /* heap hint */
#ifdef WOLFSSL_ATECC508A
    int  slot;        /* Key Slot Number (-1 unknown) */
    byte pubkey[PUB_KEY_SIZE];
#else
    ecc_point pubkey;   /* public key */
    mp_int    k;        /* private key */
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    mp_int* r;          /* sign/verify temps */
    mp_int* s;
    WC_ASYNC_DEV asyncDev;
    #ifdef WOLFSSL_CERT_GEN
        CertSignCtx certSignCtx; /* context info for cert sign (MakeSignature) */
    #endif
#endif /* WOLFSSL_ASYNC_CRYPT */
};

#ifndef WC_ECCKEY_TYPE_DEFINED
    typedef struct ecc_key ecc_key;
    #define WC_ECCKEY_TYPE_DEFINED
#endif


/* ECC predefined curve sets  */
extern const ecc_set_type ecc_sets[];

WOLFSSL_API
const char* wc_ecc_get_name(int curve_id);

#ifndef WOLFSSL_ATECC508A

#ifdef WOLFSSL_PUBLIC_ECC_ADD_DBL
    #define ECC_API    WOLFSSL_API
#else
    #define ECC_API    WOLFSSL_LOCAL
#endif

ECC_API int ecc_map(ecc_point*, mp_int*, mp_digit);
ECC_API int ecc_projective_add_point(ecc_point* P, ecc_point* Q, ecc_point* R,
                                     mp_int* a, mp_int* modulus, mp_digit mp);
ECC_API int ecc_projective_dbl_point(ecc_point* P, ecc_point* R, mp_int* a,
                                     mp_int* modulus, mp_digit mp);

#endif

WOLFSSL_API
int wc_ecc_make_key(WC_RNG* rng, int keysize, ecc_key* key);
WOLFSSL_API
int wc_ecc_make_key_ex(WC_RNG* rng, int keysize, ecc_key* key,
    int curve_id);
WOLFSSL_API
int wc_ecc_make_pub(ecc_key* key, ecc_point* pubOut);
WOLFSSL_API
int wc_ecc_check_key(ecc_key* key);
WOLFSSL_API
int wc_ecc_is_point(ecc_point* ecp, mp_int* a, mp_int* b, mp_int* prime);

#ifdef HAVE_ECC_DHE
WOLFSSL_API
int wc_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key, byte* out,
                      word32* outlen);
WOLFSSL_LOCAL
int wc_ecc_shared_secret_gen(ecc_key* private_key, ecc_point* point,
                             byte* out, word32 *outlen);
WOLFSSL_API
int wc_ecc_shared_secret_ex(ecc_key* private_key, ecc_point* point,
                             byte* out, word32 *outlen);
#define wc_ecc_shared_secret_ssh wc_ecc_shared_secret_ex /* For backwards compat */
#endif /* HAVE_ECC_DHE */

#ifdef HAVE_ECC_SIGN
WOLFSSL_API
int wc_ecc_sign_hash(const byte* in, word32 inlen, byte* out, word32 *outlen,
                     WC_RNG* rng, ecc_key* key);
WOLFSSL_API
int wc_ecc_sign_hash_ex(const byte* in, word32 inlen, WC_RNG* rng,
                        ecc_key* key, mp_int *r, mp_int *s);
#endif /* HAVE_ECC_SIGN */

#ifdef HAVE_ECC_VERIFY
WOLFSSL_API
int wc_ecc_verify_hash(const byte* sig, word32 siglen, const byte* hash,
                    word32 hashlen, int* stat, ecc_key* key);
WOLFSSL_API
int wc_ecc_verify_hash_ex(mp_int *r, mp_int *s, const byte* hash,
                          word32 hashlen, int* stat, ecc_key* key);
#endif /* HAVE_ECC_VERIFY */

WOLFSSL_API
int wc_ecc_init(ecc_key* key);
WOLFSSL_API
int wc_ecc_init_ex(ecc_key* key, void* heap, int devId);
WOLFSSL_API
void wc_ecc_free(ecc_key* key);
WOLFSSL_API
int wc_ecc_set_flags(ecc_key* key, word32 flags);
WOLFSSL_API
void wc_ecc_fp_free(void);

WOLFSSL_API
int wc_ecc_set_curve(ecc_key* key, int keysize, int curve_id);

WOLFSSL_API
int wc_ecc_is_valid_idx(int n);
WOLFSSL_API
int wc_ecc_get_curve_idx(int curve_id);
WOLFSSL_API
int wc_ecc_get_curve_id(int curve_idx);
#define wc_ecc_get_curve_name_from_id wc_ecc_get_name
WOLFSSL_API
int wc_ecc_get_curve_size_from_id(int curve_id);

WOLFSSL_API
int wc_ecc_get_curve_idx_from_name(const char* curveName);
WOLFSSL_API
int wc_ecc_get_curve_size_from_name(const char* curveName);
WOLFSSL_API
int wc_ecc_get_curve_id_from_name(const char* curveName);
WOLFSSL_API
int wc_ecc_get_curve_id_from_params(int fieldSize,
        const byte* prime, word32 primeSz, const byte* Af, word32 AfSz,
        const byte* Bf, word32 BfSz, const byte* order, word32 orderSz,
        const byte* Gx, word32 GxSz, const byte* Gy, word32 GySz, int cofactor);

#ifndef WOLFSSL_ATECC508A

WOLFSSL_API
ecc_point* wc_ecc_new_point(void);
WOLFSSL_API
ecc_point* wc_ecc_new_point_h(void* h);
WOLFSSL_API
void wc_ecc_del_point(ecc_point* p);
WOLFSSL_API
void wc_ecc_del_point_h(ecc_point* p, void* h);
WOLFSSL_API
int wc_ecc_copy_point(ecc_point* p, ecc_point *r);
WOLFSSL_API
int wc_ecc_cmp_point(ecc_point* a, ecc_point *b);
WOLFSSL_API
int wc_ecc_point_is_at_infinity(ecc_point *p);
WOLFSSL_API
int wc_ecc_mulmod(mp_int* k, ecc_point *G, ecc_point *R,
                  mp_int* a, mp_int* modulus, int map);
WOLFSSL_LOCAL
int wc_ecc_mulmod_ex(mp_int* k, ecc_point *G, ecc_point *R,
                  mp_int* a, mp_int* modulus, int map, void* heap);
#endif /* !WOLFSSL_ATECC508A */


#ifdef HAVE_ECC_KEY_EXPORT
/* ASN key helpers */
WOLFSSL_API
int wc_ecc_export_x963(ecc_key*, byte* out, word32* outLen);
WOLFSSL_API
int wc_ecc_export_x963_ex(ecc_key*, byte* out, word32* outLen, int compressed);
    /* extended functionality with compressed option */
#endif /* HAVE_ECC_KEY_EXPORT */

#ifdef HAVE_ECC_KEY_IMPORT
WOLFSSL_API
int wc_ecc_import_x963(const byte* in, word32 inLen, ecc_key* key);
WOLFSSL_API
int wc_ecc_import_x963_ex(const byte* in, word32 inLen, ecc_key* key,
                          int curve_id);
WOLFSSL_API
int wc_ecc_import_private_key(const byte* priv, word32 privSz, const byte* pub,
                           word32 pubSz, ecc_key* key);
WOLFSSL_API
int wc_ecc_import_private_key_ex(const byte* priv, word32 privSz,
                const byte* pub, word32 pubSz, ecc_key* key, int curve_id);
WOLFSSL_API
int wc_ecc_rs_to_sig(const char* r, const char* s, byte* out, word32* outlen);
WOLFSSL_API
int wc_ecc_sig_to_rs(const byte* sig, word32 sigLen, byte* r, word32* rLen,
                   byte* s, word32* sLen);
WOLFSSL_API
int wc_ecc_import_raw(ecc_key* key, const char* qx, const char* qy,
                   const char* d, const char* curveName);
WOLFSSL_API
int wc_ecc_import_raw_ex(ecc_key* key, const char* qx, const char* qy,
                   const char* d, int curve_id);
#endif /* HAVE_ECC_KEY_IMPORT */

#ifdef HAVE_ECC_KEY_EXPORT
WOLFSSL_API
int wc_ecc_export_private_only(ecc_key* key, byte* out, word32* outLen);
WOLFSSL_API
int wc_ecc_export_public_raw(ecc_key* key, byte* qx, word32* qxLen,
                             byte* qy, word32* qyLen);
WOLFSSL_API
int wc_ecc_export_private_raw(ecc_key* key, byte* qx, word32* qxLen,
                            byte* qy, word32* qyLen, byte* d, word32* dLen);
#endif /* HAVE_ECC_KEY_EXPORT */

#ifdef HAVE_ECC_KEY_EXPORT

WOLFSSL_API
int wc_ecc_export_point_der(const int curve_idx, ecc_point* point,
                            byte* out, word32* outLen);
#endif /* HAVE_ECC_KEY_EXPORT */


#ifdef HAVE_ECC_KEY_IMPORT
WOLFSSL_API
int wc_ecc_import_point_der(byte* in, word32 inLen, const int curve_idx,
                            ecc_point* point);
#endif /* HAVE_ECC_KEY_IMPORT */

/* size helper */
WOLFSSL_API
int wc_ecc_size(ecc_key* key);
WOLFSSL_API
int wc_ecc_sig_size(ecc_key* key);

WOLFSSL_API
int wc_ecc_get_oid(word32 oidSum, const byte** oid, word32* oidSz);

#ifdef WOLFSSL_CUSTOM_CURVES
    WOLFSSL_API
    int wc_ecc_set_custom_curve(ecc_key* key, const ecc_set_type* dp);
#endif

#ifdef HAVE_ECC_ENCRYPT
/* ecc encrypt */

enum ecEncAlgo {
    ecAES_128_CBC = 1,  /* default */
    ecAES_256_CBC = 2
};

enum ecKdfAlgo {
    ecHKDF_SHA256 = 1,  /* default */
    ecHKDF_SHA1   = 2
};

enum ecMacAlgo {
    ecHMAC_SHA256 = 1,  /* default */
    ecHMAC_SHA1   = 2
};

enum {
    KEY_SIZE_128     = 16,
    KEY_SIZE_256     = 32,
    IV_SIZE_64       =  8,
    IV_SIZE_128      = 16,
    EXCHANGE_SALT_SZ = 16,
    EXCHANGE_INFO_SZ = 23
};

enum ecFlags {
    REQ_RESP_CLIENT = 1,
    REQ_RESP_SERVER = 2
};


typedef struct ecEncCtx ecEncCtx;

WOLFSSL_API
ecEncCtx* wc_ecc_ctx_new(int flags, WC_RNG* rng);
WOLFSSL_API
ecEncCtx* wc_ecc_ctx_new_ex(int flags, WC_RNG* rng, void* heap);
WOLFSSL_API
void wc_ecc_ctx_free(ecEncCtx*);
WOLFSSL_API
int wc_ecc_ctx_reset(ecEncCtx*, WC_RNG*);  /* reset for use again w/o alloc/free */

WOLFSSL_API
const byte* wc_ecc_ctx_get_own_salt(ecEncCtx*);
WOLFSSL_API
int wc_ecc_ctx_set_peer_salt(ecEncCtx*, const byte* salt);
WOLFSSL_API
int wc_ecc_ctx_set_info(ecEncCtx*, const byte* info, int sz);

WOLFSSL_API
int wc_ecc_encrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx);
WOLFSSL_API
int wc_ecc_decrypt(ecc_key* privKey, ecc_key* pubKey, const byte* msg,
                word32 msgSz, byte* out, word32* outSz, ecEncCtx* ctx);

#endif /* HAVE_ECC_ENCRYPT */

#ifdef HAVE_X963_KDF
WOLFSSL_API int wc_X963_KDF(enum wc_HashType type, const byte* secret,
                word32 secretSz, const byte* sinfo, word32 sinfoSz,
                byte* out, word32 outSz);
#endif

#ifdef ECC_CACHE_CURVE
WOLFSSL_API int wc_ecc_curve_cache_init(void);
WOLFSSL_API void wc_ecc_curve_cache_free(void);
#endif


#ifdef __cplusplus
    }    /* extern "C" */
#endif

#endif /* HAVE_ECC */
#endif /* WOLF_CRYPT_ECC_H */
