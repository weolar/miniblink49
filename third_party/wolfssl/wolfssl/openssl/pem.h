/* pem.h
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

/* pem.h for openssl */

/*!
    \file wolfssl/openssl/pem.h
*/


#ifndef WOLFSSL_PEM_H_
#define WOLFSSL_PEM_H_

#include <wolfssl/openssl/evp.h>
#include <wolfssl/openssl/bio.h>
#include <wolfssl/openssl/rsa.h>
#include <wolfssl/openssl/dsa.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* RSA */
WOLFSSL_API
int wolfSSL_PEM_write_bio_RSAPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_RSA* rsa,
                                        const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        pem_password_cb* cb, void* arg);
WOLFSSL_API
WOLFSSL_RSA* wolfSSL_PEM_read_bio_RSAPrivateKey(WOLFSSL_BIO* bio,
                                                  WOLFSSL_RSA**,
                                                  pem_password_cb* cb,
                                                  void* arg);
WOLFSSL_API
int wolfSSL_PEM_write_mem_RSAPrivateKey(RSA* rsa, const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_RSAPrivateKey(FILE *fp, WOLFSSL_RSA *rsa,
                                    const EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    pem_password_cb *cb, void *u);
WOLFSSL_API
WOLFSSL_RSA *wolfSSL_PEM_read_RSAPublicKey(FILE *fp, WOLFSSL_RSA **x,
                                           pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_RSAPublicKey(FILE *fp, WOLFSSL_RSA *x);

WOLFSSL_API
int wolfSSL_PEM_write_RSA_PUBKEY(FILE *fp, WOLFSSL_RSA *x);
#endif /* NO_FILESYSTEM */

/* DSA */
WOLFSSL_API
int wolfSSL_PEM_write_bio_DSAPrivateKey(WOLFSSL_BIO* bio,
                                        WOLFSSL_DSA* dsa,
                                        const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        pem_password_cb* cb, void* arg);
WOLFSSL_API
int wolfSSL_PEM_write_mem_DSAPrivateKey(WOLFSSL_DSA* dsa,
                                        const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_DSAPrivateKey(FILE *fp, WOLFSSL_DSA *dsa,
                                    const EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_DSA_PUBKEY(FILE *fp, WOLFSSL_DSA *x);
#endif /* NO_FILESYSTEM */

/* ECC */
WOLFSSL_API
int wolfSSL_PEM_write_bio_ECPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EC_KEY* ec,
                                       const EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       pem_password_cb* cb, void* arg);
WOLFSSL_API
int wolfSSL_PEM_write_mem_ECPrivateKey(WOLFSSL_EC_KEY* key,
                                       const EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       unsigned char **pem, int *plen);
#if !defined(NO_FILESYSTEM)
WOLFSSL_API
int wolfSSL_PEM_write_ECPrivateKey(FILE *fp, WOLFSSL_EC_KEY *key,
                                   const EVP_CIPHER *enc,
                                   unsigned char *kstr, int klen,
                                   pem_password_cb *cb, void *u);
WOLFSSL_API
int wolfSSL_PEM_write_EC_PUBKEY(FILE *fp, WOLFSSL_EC_KEY *key);
#endif /* NO_FILESYSTEM */

/* EVP_KEY */
WOLFSSL_API
WOLFSSL_EVP_PKEY* wolfSSL_PEM_read_bio_PrivateKey(WOLFSSL_BIO* bio,
                                                  WOLFSSL_EVP_PKEY**,
                                                  pem_password_cb* cb,
                                                  void* arg);
WOLFSSL_API
int wolfSSL_PEM_write_bio_PrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EVP_PKEY* key,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        pem_password_cb* cb, void* arg);

WOLFSSL_API
int wolfSSL_EVP_PKEY_type(int type);

WOLFSSL_API
int wolfSSL_EVP_PKEY_base_id(const EVP_PKEY *pkey);

#if !defined(NO_FILESYSTEM)
WOLFSSL_API
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PUBKEY(FILE *fp, EVP_PKEY **x,
										  pem_password_cb *cb, void *u);
WOLFSSL_API
WOLFSSL_X509 *wolfSSL_PEM_read_X509(FILE *fp, WOLFSSL_X509 **x,
                                          pem_password_cb *cb, void *u);
WOLFSSL_API
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PrivateKey(FILE *fp, WOLFSSL_EVP_PKEY **x,
                                          pem_password_cb *cb, void *u);
#endif /* NO_FILESYSTEM */

#define PEM_read_X509               wolfSSL_PEM_read_X509
#define PEM_read_PrivateKey         wolfSSL_PEM_read_PrivateKey
#define PEM_write_bio_PrivateKey    wolfSSL_PEM_write_bio_PrivateKey
/* RSA */
#define PEM_write_bio_RSAPrivateKey wolfSSL_PEM_write_bio_RSAPrivateKey
#define PEM_read_bio_RSAPrivateKey  wolfSSL_PEM_read_bio_RSAPrivateKey
#define PEM_write_RSAPrivateKey     wolfSSL_PEM_write_RSAPrivateKey
#define PEM_write_RSA_PUBKEY        wolfSSL_PEM_write_RSA_PUBKEY
#define PEM_write_RSAPublicKey      wolfSSL_PEM_write_RSAPublicKey
#define PEM_read_RSAPublicKey       wolfSSL_PEM_read_RSAPublicKey
/* DSA */
#define PEM_write_bio_DSAPrivateKey wolfSSL_PEM_write_bio_DSAPrivateKey
#define PEM_write_DSAPrivateKey     wolfSSL_PEM_write_DSAPrivateKey
#define PEM_write_DSA_PUBKEY        wolfSSL_PEM_write_DSA_PUBKEY
/* ECC */
#define PEM_write_bio_ECPrivateKey wolfSSL_PEM_write_bio_ECPrivateKey
#define PEM_write_EC_PUBKEY        wolfSSL_PEM_write_EC_PUBKEY
#define PEM_write_ECPrivateKey     wolfSSL_PEM_write_ECPrivateKey
/* EVP_KEY */
#define PEM_read_bio_PrivateKey wolfSSL_PEM_read_bio_PrivateKey
#define PEM_read_PUBKEY         wolfSSL_PEM_read_PUBKEY
#define EVP_PKEY_type           wolfSSL_EVP_PKEY_type

#ifdef __cplusplus
    }  /* extern "C" */ 
#endif

#endif /* WOLFSSL_PEM_H_ */

