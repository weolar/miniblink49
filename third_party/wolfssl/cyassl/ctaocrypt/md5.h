/* md5.h
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


#ifndef NO_MD5

#ifndef CTAO_CRYPT_MD5_H
#define CTAO_CRYPT_MD5_H

#include <wolfssl/wolfcrypt/md5.h>

#ifndef HAVE_FIPS
    #define InitMd5   wc_InitMd5
    #define Md5Update wc_Md5Update
    #define Md5Final  wc_Md5Final
    #define Md5Hash   wc_Md5Hash
#else
    /* redfined name so that hmac is calling same function names with fips */
    #define wc_InitMd5   InitMd5
    #define wc_Md5Update Md5Update
    #define wc_Md5Final  Md5Final
    #define wc_Md5Hash   Md5Hash
#endif

#endif /* CTAO_CRYPT_MD5_H */
#endif /* NO_MD5 */

