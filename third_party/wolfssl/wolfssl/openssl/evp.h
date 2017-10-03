/* evp.h
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



/*  evp.h defines mini evp openssl compatibility layer
 *
 */


#ifndef WOLFSSL_EVP_H_
#define WOLFSSL_EVP_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_PREFIX
#include "prefix_evp.h"
#endif

#ifndef NO_MD5
    #include <wolfssl/openssl/md5.h>
#endif
#include <wolfssl/openssl/sha.h>
#include <wolfssl/openssl/ripemd.h>
#include <wolfssl/openssl/rsa.h>
#include <wolfssl/openssl/dsa.h>
#include <wolfssl/openssl/ec.h>

#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/arc4.h>
#include <wolfssl/wolfcrypt/hmac.h>
#ifdef HAVE_IDEA
    #include <wolfssl/wolfcrypt/idea.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

typedef char WOLFSSL_EVP_CIPHER;
typedef char WOLFSSL_EVP_MD;

#ifndef NO_MD5
    WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_md5(void);
#endif
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha1(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha224(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha256(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha384(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512(void);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_ripemd160(void);

WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ctr(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ctr(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ctr(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_ecb(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc4(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_idea_cbc(void);
WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_enc_null(void);


typedef union {
    #ifndef NO_MD5
        WOLFSSL_MD5_CTX    md5;
    #endif
    WOLFSSL_SHA_CTX    sha;
    #ifdef WOLFSSL_SHA224
        WOLFSSL_SHA224_CTX sha224;
    #endif
    WOLFSSL_SHA256_CTX sha256;
    #ifdef WOLFSSL_SHA384
        WOLFSSL_SHA384_CTX sha384;
    #endif
    #ifdef WOLFSSL_SHA512
        WOLFSSL_SHA512_CTX sha512;
    #endif
    #ifdef WOLFSSL_RIPEMD
        WOLFSSL_RIPEMD_CTX ripemd;
    #endif
} WOLFSSL_Hasher;


typedef struct WOLFSSL_EVP_MD_CTX {
    union {
        WOLFSSL_Hasher digest;
        Hmac hmac;
    } hash;
    unsigned char macType;
} WOLFSSL_EVP_MD_CTX;


typedef union {
#ifndef NO_AES
    Aes  aes;
#endif
#ifndef NO_DES3
    Des  des;
    Des3 des3;
#endif
    Arc4 arc4;
#ifdef HAVE_IDEA
    Idea idea;
#endif
} WOLFSSL_Cipher;


enum {
    AES_128_CBC_TYPE  = 1,
    AES_192_CBC_TYPE  = 2,
    AES_256_CBC_TYPE  = 3,
    AES_128_CTR_TYPE  = 4,
    AES_192_CTR_TYPE  = 5,
    AES_256_CTR_TYPE  = 6,
    AES_128_ECB_TYPE  = 7,
    AES_192_ECB_TYPE  = 8,
    AES_256_ECB_TYPE  = 9,
    DES_CBC_TYPE      = 10,
    DES_ECB_TYPE      = 11,
    DES_EDE3_CBC_TYPE = 12,
    DES_EDE3_ECB_TYPE = 13,
    ARC4_TYPE         = 14,
    NULL_CIPHER_TYPE  = 15,
    EVP_PKEY_RSA      = 16,
    EVP_PKEY_DSA      = 17,
    EVP_PKEY_EC       = 18,
    IDEA_CBC_TYPE     = 19,
    NID_sha1          = 64,
    NID_md2           = 3,
    NID_md5           =  4,
    NID_hmac          = 855,
    EVP_PKEY_HMAC     = NID_hmac
};

enum {
    NID_aes_128_cbc = 419,
    NID_aes_192_cbc = 423,
    NID_aes_256_cbc = 427,
    NID_aes_128_ctr = 904,
    NID_aes_192_ctr = 905,
    NID_aes_256_ctr = 906,
    NID_aes_128_ecb = 418,
    NID_aes_192_ecb = 422,
    NID_aes_256_ecb = 426,
    NID_des_cbc     =  31,
    NID_des_ecb     =  29,
    NID_des_ede3_cbc=  44,
    NID_des_ede3_ecb=  33,
    NID_idea_cbc    =  34,
};

#define WOLFSSL_EVP_BUF_SIZE 16
typedef struct WOLFSSL_EVP_CIPHER_CTX {
    int            keyLen;         /* user may set for variable */
    int            block_size;
    unsigned long  flags;
    unsigned char  enc;            /* if encrypt side, then true */
    unsigned char  cipherType;
#ifndef NO_AES
    /* working iv pointer into cipher */
    ALIGN16 unsigned char  iv[AES_BLOCK_SIZE];
#elif !defined(NO_DES3)
    /* working iv pointer into cipher */
    ALIGN16 unsigned char  iv[DES_BLOCK_SIZE];
#endif
    WOLFSSL_Cipher  cipher;
    ALIGN16 byte buf[WOLFSSL_EVP_BUF_SIZE];
    int  bufUsed;
    ALIGN16 byte lastBlock[WOLFSSL_EVP_BUF_SIZE];
    int  lastUsed;
} WOLFSSL_EVP_CIPHER_CTX;


#ifndef WOLFSSL_EVP_PKEY_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_EVP_PKEY     WOLFSSL_EVP_PKEY;
#define WOLFSSL_EVP_PKEY_TYPE_DEFINED
#endif

struct WOLFSSL_EVP_PKEY {
    int type;         /* openssh dereference */
    int save_type;    /* openssh dereference */
    int pkey_sz;
    union {
        char* ptr; /* der format of key / or raw for NTRU */
    } pkey;
    #ifdef HAVE_ECC
        int pkey_curve;
    #endif
};

typedef int WOLFSSL_ENGINE  ;
typedef WOLFSSL_ENGINE ENGINE;
typedef int WOLFSSL_EVP_PKEY_CTX;
typedef WOLFSSL_EVP_PKEY_CTX EVP_PKEY_CTX;

WOLFSSL_API void wolfSSL_EVP_init(void);
WOLFSSL_API int  wolfSSL_EVP_MD_size(const WOLFSSL_EVP_MD* md);
WOLFSSL_API int wolfSSL_EVP_MD_type(const WOLFSSL_EVP_MD *md);

WOLFSSL_API WOLFSSL_EVP_MD_CTX *wolfSSL_EVP_MD_CTX_new (void);
WOLFSSL_API void                wolfSSL_EVP_MD_CTX_free(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API void wolfSSL_EVP_MD_CTX_init(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_MD_CTX_cleanup(WOLFSSL_EVP_MD_CTX* ctx);
WOLFSSL_API const WOLFSSL_EVP_MD *wolfSSL_EVP_MD_CTX_md(const WOLFSSL_EVP_MD_CTX *ctx);
WOLFSSL_API const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbyname(const char *name);
WOLFSSL_API const WOLFSSL_EVP_MD     *wolfSSL_EVP_get_digestbyname(const char *name);

WOLFSSL_API int wolfSSL_EVP_DigestInit(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type);
WOLFSSL_API int wolfSSL_EVP_DigestInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl);
WOLFSSL_API int wolfSSL_EVP_DigestUpdate(WOLFSSL_EVP_MD_CTX* ctx, const void* data,
                                       unsigned long sz);
WOLFSSL_API int wolfSSL_EVP_DigestFinal(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                                      unsigned int* s);
WOLFSSL_API int wolfSSL_EVP_DigestFinal_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                            unsigned char* md, unsigned int* s);

WOLFSSL_API int wolfSSL_EVP_DigestSignInit(WOLFSSL_EVP_MD_CTX *ctx,
                                           WOLFSSL_EVP_PKEY_CTX **pctx,
                                           const WOLFSSL_EVP_MD *type,
                                           WOLFSSL_ENGINE *e,
                                           WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int wolfSSL_EVP_DigestSignUpdate(WOLFSSL_EVP_MD_CTX *ctx,
                                             const void *d, unsigned int cnt);
WOLFSSL_API int wolfSSL_EVP_DigestSignFinal(WOLFSSL_EVP_MD_CTX *ctx,
                                            unsigned char *sig, size_t *siglen);

#ifndef NO_MD5
WOLFSSL_API int wolfSSL_EVP_BytesToKey(const WOLFSSL_EVP_CIPHER*,
                              const WOLFSSL_EVP_MD*, const unsigned char*,
                              const unsigned char*, int, int, unsigned char*,
                              unsigned char*);
#endif

WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_init(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_cleanup(WOLFSSL_EVP_CIPHER_CTX* ctx);

WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_iv_length(const WOLFSSL_EVP_CIPHER_CTX*);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_iv_length(const WOLFSSL_EVP_CIPHER*);


WOLFSSL_API int  wolfSSL_EVP_CipherInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    unsigned char* key, unsigned char* iv,
                                    int enc);
WOLFSSL_API int  wolfSSL_EVP_CipherInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    unsigned char* key, unsigned char* iv,
                                    int enc);
WOLFSSL_API int  wolfSSL_EVP_EncryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    unsigned char* key, unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_EncryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    unsigned char* key, unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_DecryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    unsigned char* key, unsigned char* iv);
WOLFSSL_API int  wolfSSL_EVP_DecryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    unsigned char* key, unsigned char* iv);
WOLFSSL_API int wolfSSL_EVP_CipherUpdate(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl);
WOLFSSL_API int  wolfSSL_EVP_CipherFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_CipherFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl, int enc);
WOLFSSL_API int  wolfSSL_EVP_EncryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_EncryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_DecryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);
WOLFSSL_API int  wolfSSL_EVP_DecryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl);

WOLFSSL_API WOLFSSL_EVP_CIPHER_CTX *wolfSSL_EVP_CIPHER_CTX_new(void);
WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_free(WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                                     int keylen);
WOLFSSL_API int  wolfSSL_EVP_Cipher(WOLFSSL_EVP_CIPHER_CTX* ctx,
                          unsigned char* dst, unsigned char* src,
                          unsigned int len);

WOLFSSL_API const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_get_cipherbynid(int);
WOLFSSL_API const WOLFSSL_EVP_MD* wolfSSL_EVP_get_digestbynid(int);

WOLFSSL_API WOLFSSL_RSA* wolfSSL_EVP_PKEY_get1_RSA(WOLFSSL_EVP_PKEY*);
WOLFSSL_API WOLFSSL_DSA* wolfSSL_EVP_PKEY_get1_DSA(WOLFSSL_EVP_PKEY*);
WOLFSSL_API WOLFSSL_EC_KEY *wolfSSL_EVP_PKEY_get1_EC_KEY(WOLFSSL_EVP_PKEY *key);

WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_mac_key(int type, ENGINE* e,
                                          const unsigned char* key, int keylen);
WOLFSSL_API const unsigned char* wolfSSL_EVP_PKEY_get0_hmac(const WOLFSSL_EVP_PKEY* pkey,
        size_t* len);

/* these next ones don't need real OpenSSL type, for OpenSSH compat only */
WOLFSSL_API void* wolfSSL_EVP_X_STATE(const WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int   wolfSSL_EVP_X_STATE_LEN(const WOLFSSL_EVP_CIPHER_CTX* ctx);

WOLFSSL_API void  wolfSSL_3des_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                                unsigned char* iv, int len);
WOLFSSL_API void  wolfSSL_aes_ctr_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                                unsigned char* iv, int len);

WOLFSSL_API int  wolfSSL_StoreExternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx);
WOLFSSL_API int  wolfSSL_SetInternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx);

WOLFSSL_API int wolfSSL_EVP_CIPHER_CTX_block_size(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int wolfSSL_EVP_CIPHER_block_size(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long WOLFSSL_EVP_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long WOLFSSL_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_flags(const WOLFSSL_EVP_CIPHER *cipher);
WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_set_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags);
WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_CTX_mode(const WOLFSSL_EVP_CIPHER_CTX *ctx);
WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_padding(WOLFSSL_EVP_CIPHER_CTX *c, int pad);
WOLFSSL_API int  wolfSSL_EVP_add_digest(const WOLFSSL_EVP_MD *digest);
WOLFSSL_API int  wolfSSL_EVP_add_cipher(const WOLFSSL_EVP_CIPHER *cipher);

#define EVP_CIPH_STREAM_CIPHER WOLFSSL_EVP_CIPH_STREAM_CIPHER
#define EVP_CIPH_ECB_MODE WOLFSSL_EVP_CIPH_ECB_MODE
#define EVP_CIPH_CBC_MODE WOLFSSL_EVP_CIPH_CBC_MODE
#define EVP_CIPH_CFB_MODE WOLFSSL_EVP_CIPH_CFB_MODE
#define EVP_CIPH_OFB_MODE WOLFSSL_EVP_CIPH_OFB_MODE
#define EVP_CIPH_CTR_MODE WOLFSSL_EVP_CIPH_CTR_MODE
#define EVP_CIPH_GCM_MODE WOLFSSL_EVP_CIPH_GCM_MODE
#define EVP_CIPH_CCM_MODE WOLFSSL_EVP_CIPH_CCM_MODE

#define WOLFSSL_EVP_CIPH_MODE           0x0007
#define WOLFSSL_EVP_CIPH_STREAM_CIPHER      0x0
#define WOLFSSL_EVP_CIPH_ECB_MODE           0x1
#define WOLFSSL_EVP_CIPH_CBC_MODE           0x2
#define WOLFSSL_EVP_CIPH_CFB_MODE           0x3
#define WOLFSSL_EVP_CIPH_OFB_MODE           0x4
#define WOLFSSL_EVP_CIPH_CTR_MODE           0x5
#define WOLFSSL_EVP_CIPH_GCM_MODE           0x6
#define WOLFSSL_EVP_CIPH_CCM_MODE           0x7
#define WOLFSSL_EVP_CIPH_NO_PADDING       0x100

/* end OpenSSH compat */

typedef WOLFSSL_EVP_MD         EVP_MD;
typedef WOLFSSL_EVP_CIPHER     EVP_CIPHER;
typedef WOLFSSL_EVP_MD_CTX     EVP_MD_CTX;
typedef WOLFSSL_EVP_CIPHER_CTX EVP_CIPHER_CTX;

#ifndef NO_MD5
    #define EVP_md5       wolfSSL_EVP_md5
#endif
#define EVP_sha1      wolfSSL_EVP_sha1
#define EVP_sha224    wolfSSL_EVP_sha224
#define EVP_sha256    wolfSSL_EVP_sha256
#define EVP_sha384    wolfSSL_EVP_sha384
#define EVP_sha512    wolfSSL_EVP_sha512
#define EVP_ripemd160 wolfSSL_EVP_ripemd160

#define EVP_aes_128_cbc  wolfSSL_EVP_aes_128_cbc
#define EVP_aes_192_cbc  wolfSSL_EVP_aes_192_cbc
#define EVP_aes_256_cbc  wolfSSL_EVP_aes_256_cbc
#define EVP_aes_128_ecb  wolfSSL_EVP_aes_128_ecb
#define EVP_aes_192_ecb  wolfSSL_EVP_aes_192_ecb
#define EVP_aes_256_ecb  wolfSSL_EVP_aes_256_ecb
#define EVP_aes_128_ctr  wolfSSL_EVP_aes_128_ctr
#define EVP_aes_192_ctr  wolfSSL_EVP_aes_192_ctr
#define EVP_aes_256_ctr  wolfSSL_EVP_aes_256_ctr
#define EVP_des_cbc      wolfSSL_EVP_des_cbc
#define EVP_des_ecb      wolfSSL_EVP_des_ecb
#define EVP_des_ede3_cbc wolfSSL_EVP_des_ede3_cbc
#define EVP_des_ede3_ecb wolfSSL_EVP_des_ede3_ecb
#define EVP_rc4          wolfSSL_EVP_rc4
#define EVP_idea_cbc     wolfSSL_EVP_idea_cbc
#define EVP_enc_null     wolfSSL_EVP_enc_null

#define EVP_MD_size        wolfSSL_EVP_MD_size
#define EVP_MD_CTX_new     wolfSSL_EVP_MD_CTX_new
#define EVP_MD_CTX_create  wolfSSL_EVP_MD_CTX_new
#define EVP_MD_CTX_free    wolfSSL_EVP_MD_CTX_free
#define EVP_MD_CTX_destroy wolfSSL_EVP_MD_CTX_free
#define EVP_MD_CTX_init    wolfSSL_EVP_MD_CTX_init
#define EVP_MD_CTX_cleanup wolfSSL_EVP_MD_CTX_cleanup
#define EVP_MD_CTX_md      wolfSSL_EVP_MD_CTX_md
#define EVP_MD_CTX_type    wolfSSL_EVP_MD_type
#define EVP_MD_type        wolfSSL_EVP_MD_type

#define EVP_DigestInit     wolfSSL_EVP_DigestInit
#define EVP_DigestInit_ex  wolfSSL_EVP_DigestInit_ex
#define EVP_DigestUpdate   wolfSSL_EVP_DigestUpdate
#define EVP_DigestFinal    wolfSSL_EVP_DigestFinal
#define EVP_DigestFinal_ex wolfSSL_EVP_DigestFinal_ex
#define EVP_DigestSignInit   wolfSSL_EVP_DigestSignInit
#define EVP_DigestSignUpdate wolfSSL_EVP_DigestSignUpdate
#define EVP_DigestSignFinal  wolfSSL_EVP_DigestSignFinal
#define EVP_BytesToKey     wolfSSL_EVP_BytesToKey

#define EVP_get_cipherbyname wolfSSL_EVP_get_cipherbyname
#define EVP_get_digestbyname wolfSSL_EVP_get_digestbyname

#define EVP_CIPHER_CTX_init           wolfSSL_EVP_CIPHER_CTX_init
#define EVP_CIPHER_CTX_cleanup        wolfSSL_EVP_CIPHER_CTX_cleanup
#define EVP_CIPHER_CTX_iv_length      wolfSSL_EVP_CIPHER_CTX_iv_length
#define EVP_CIPHER_CTX_key_length     wolfSSL_EVP_CIPHER_CTX_key_length
#define EVP_CIPHER_CTX_set_key_length wolfSSL_EVP_CIPHER_CTX_set_key_length
#define EVP_CIPHER_CTX_mode           wolfSSL_EVP_CIPHER_CTX_mode

#define EVP_CIPHER_iv_length          wolfSSL_EVP_CIPHER_iv_length

#define EVP_CipherInit                wolfSSL_EVP_CipherInit
#define EVP_CipherInit_ex             wolfSSL_EVP_CipherInit_ex
#define EVP_EncryptInit               wolfSSL_EVP_EncryptInit
#define EVP_EncryptInit_ex            wolfSSL_EVP_EncryptInit_ex
#define EVP_DecryptInit               wolfSSL_EVP_DecryptInit
#define EVP_DecryptInit_ex            wolfSSL_EVP_DecryptInit_ex

#define EVP_Cipher                    wolfSSL_EVP_Cipher
#define EVP_CipherUpdate              wolfSSL_EVP_CipherUpdate
#define EVP_EncryptUpdate             wolfSSL_EVP_CipherUpdate
#define EVP_DecryptUpdate             wolfSSL_EVP_CipherUpdate
#define EVP_CipherFinal               wolfSSL_EVP_CipherFinal
#define EVP_CipherFinal_ex            wolfSSL_EVP_CipherFinal
#define EVP_EncryptFinal              wolfSSL_EVP_CipherFinal
#define EVP_EncryptFinal_ex           wolfSSL_EVP_CipherFinal
#define EVP_DecryptFinal              wolfSSL_EVP_CipherFinal
#define EVP_DecryptFinal_ex           wolfSSL_EVP_CipherFinal

#define EVP_CIPHER_CTX_free           wolfSSL_EVP_CIPHER_CTX_free
#define EVP_CIPHER_CTX_new            wolfSSL_EVP_CIPHER_CTX_new

#define EVP_get_cipherbynid           wolfSSL_EVP_get_cipherbynid
#define EVP_get_digestbynid           wolfSSL_EVP_get_digestbynid
#define EVP_get_cipherbyname          wolfSSL_EVP_get_cipherbyname
#define EVP_get_digestbyname          wolfSSL_EVP_get_digestbyname

#define EVP_PKEY_get1_RSA   wolfSSL_EVP_PKEY_get1_RSA
#define EVP_PKEY_get1_DSA   wolfSSL_EVP_PKEY_get1_DSA
#define EVP_PKEY_get1_EC_KEY wolfSSL_EVP_PKEY_get1_EC_KEY
#define EVP_PKEY_get0_hmac  wolfSSL_EVP_PKEY_get0_hmac
#define EVP_PKEY_new_mac_key       wolfSSL_EVP_PKEY_new_mac_key

#define EVP_CIPHER_CTX_block_size  wolfSSL_EVP_CIPHER_CTX_block_size
#define EVP_CIPHER_block_size      wolfSSL_EVP_CIPHER_block_size
#define EVP_CIPHER_flags           wolfSSL_EVP_CIPHER_flags
#define EVP_CIPHER_CTX_set_flags   wolfSSL_EVP_CIPHER_CTX_set_flags
#define EVP_CIPHER_CTX_set_padding wolfSSL_EVP_CIPHER_CTX_set_padding
#define EVP_CIPHER_CTX_flags       wolfSSL_EVP_CIPHER_CTX_flags
#define EVP_add_digest             wolfSSL_EVP_add_digest
#define EVP_add_cipher             wolfSSL_EVP_add_cipher

#ifndef EVP_MAX_MD_SIZE
    #define EVP_MAX_MD_SIZE   64     /* sha512 */
#endif

#ifndef EVP_MAX_BLOCK_LENGTH
    #define EVP_MAX_BLOCK_LENGTH   32  /* 2 * blocklen(AES)? */
    /* They define this as 32. Using the same value here. */
#endif

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* WOLFSSL_EVP_H_ */
