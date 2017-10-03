/* visibility.h
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


/* Visibility control macros */

#ifndef CTAO_CRYPT_VISIBILITY_H
#define CTAO_CRYPT_VISIBILITY_H

/* fips compatibility @wc_fips */
#ifndef HAVE_FIPS
    #include <wolfssl/wolfcrypt/visibility.h>
    #define CYASSL_API   WOLFSSL_API
	#define CYASSL_LOCAL WOLFSSL_LOCAL
#else
/* CYASSL_API is used for the public API symbols.
        It either imports or exports (or does nothing for static builds)

   CYASSL_LOCAL is used for non-API symbols (private).
*/

#if defined(BUILDING_WOLFSSL)
    #if defined(HAVE_VISIBILITY) && HAVE_VISIBILITY
        #define CYASSL_API   __attribute__ ((visibility("default")))
        #define CYASSL_LOCAL __attribute__ ((visibility("hidden")))
    #elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
        #define CYASSL_API   __global
        #define CYASSL_LOCAL __hidden
    #elif defined(_MSC_VER)
        #ifdef CYASSL_DLL
            #define CYASSL_API extern __declspec(dllexport)
        #else
            #define CYASSL_API
        #endif
        #define CYASSL_LOCAL
    #else
        #define CYASSL_API
        #define CYASSL_LOCAL
    #endif /* HAVE_VISIBILITY */
#else /* BUILDING_WOLFSSL */
    #if defined(_MSC_VER)
        #ifdef CYASSL_DLL
            #define CYASSL_API extern __declspec(dllimport)
        #else
            #define CYASSL_API
        #endif
        #define CYASSL_LOCAL
    #else
        #define CYASSL_API
        #define CYASSL_LOCAL
    #endif
#endif /* BUILDING_WOLFSSL */
#endif /* HAVE_FIPS */
#endif /* CTAO_CRYPT_VISIBILITY_H */

