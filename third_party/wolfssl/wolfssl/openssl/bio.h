/* bio.h
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

/* bio.h for openssl */


#ifndef WOLFSSL_BIO_H_
#define WOLFSSL_BIO_H_

#include <wolfssl/openssl/ssl.h>


#ifdef __cplusplus
    extern "C" {
#endif

#define BIO_FLAG_BASE64_NO_NL WOLFSSL_BIO_FLAG_BASE64_NO_NL
#define BIO_FLAG_READ         WOLFSSL_BIO_FLAG_READ
#define BIO_FLAG_WRITE        WOLFSSL_BIO_FLAG_WRITE
#define BIO_FLAG_IO_SPECIAL   WOLFSSL_BIO_FLAG_IO_SPECIAL
#define BIO_FLAG_RETRY        WOLFSSL_BIO_FLAG_RETRY

#define BIO_find_type wolfSSL_BIO_find_type
#define BIO_next      wolfSSL_BIO_next
#define BIO_gets      wolfSSL_BIO_gets


#define BIO_TYPE_FILE WOLFSSL_BIO_FILE
#define BIO_TYPE_BIO  WOLFSSL_BIO_BIO
#define BIO_TYPE_MEM  WOLFSSL_BIO_MEMORY
#define BIO_TYPE_BASE64 WOLFSSL_BIO_BASE64


#ifdef __cplusplus
    }  /* extern "C" */ 
#endif


#endif /* WOLFSSL_BIO_H_ */

