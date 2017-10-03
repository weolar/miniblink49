/* opensslv.h compatibility */

#ifndef WOLFSSL_OPENSSLV_H_
#define WOLFSSL_OPENSSLV_H_


/* api version compatibility */
#if defined(HAVE_STUNNEL) || defined(HAVE_LIGHTY) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
     /* version number can be increased for Lighty after compatibility for ECDH
        is added */
     #define OPENSSL_VERSION_NUMBER 0x10001000L
#else
     #define OPENSSL_VERSION_NUMBER 0x0090810fL
#endif

#define OPENSSL_VERSION_TEXT             LIBWOLFSSL_VERSION_STRING


#endif /* header */

