/* dh.h
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

/* dh.h for openSSL */


#ifndef WOLFSSL_DH_H_
#define WOLFSSL_DH_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/openssl/bn.h>

#ifdef __cplusplus
    extern "C" {
#endif

struct WOLFSSL_DH {
    WOLFSSL_BIGNUM* p;
    WOLFSSL_BIGNUM* g;
    WOLFSSL_BIGNUM* q;
    WOLFSSL_BIGNUM* pub_key;      /* openssh deference g^x */
    WOLFSSL_BIGNUM* priv_key;     /* openssh deference x   */
    void*          internal;     /* our DH */
    char           inSet;        /* internal set from external ? */
    char           exSet;        /* external set from internal ? */
    /*added for lighttpd openssl compatibility, go back and add a getter in
     * lighttpd src code.
     */
     int length;
};


WOLFSSL_API WOLFSSL_DH* wolfSSL_DH_new(void);
WOLFSSL_API void       wolfSSL_DH_free(WOLFSSL_DH*);

WOLFSSL_API int wolfSSL_DH_size(WOLFSSL_DH*);
WOLFSSL_API int wolfSSL_DH_generate_key(WOLFSSL_DH*);
WOLFSSL_API int wolfSSL_DH_compute_key(unsigned char* key, WOLFSSL_BIGNUM* pub,
                                     WOLFSSL_DH*);

typedef WOLFSSL_DH DH;

#define DH_new  wolfSSL_DH_new
#define DH_free wolfSSL_DH_free

#define DH_size         wolfSSL_DH_size
#define DH_generate_key wolfSSL_DH_generate_key
#define DH_compute_key  wolfSSL_DH_compute_key
#define get_rfc3526_prime_1536 wolfSSL_DH_1536_prime


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#if defined(OPENSSL_ALL) || defined(HAVE_STUNNEL)
#define DH_generate_parameters    wolfSSL_DH_generate_parameters
#define DH_generate_parameters_ex wolfSSL_DH_generate_parameters_ex
#endif /* OPENSSL_ALL || HAVE_STUNNEL */

#endif /* WOLFSSL_DH_H_ */
