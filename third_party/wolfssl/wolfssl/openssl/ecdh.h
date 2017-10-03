/* ecdh.h for openssl */

#ifndef WOLFSSL_ECDH_H_
#define WOLFSSL_ECDH_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/openssl/bn.h>

#ifdef __cplusplus
extern "C" {
#endif


WOLFSSL_API int wolfSSL_ECDH_compute_key(void *out, size_t outlen,
                                         const WOLFSSL_EC_POINT *pub_key,
                                         WOLFSSL_EC_KEY *ecdh,
                                         void *(*KDF) (const void *in,
                                                       size_t inlen,
                                                       void *out,
                                                       size_t *outlen));

#define ECDH_compute_key wolfSSL_ECDH_compute_key

#ifdef __cplusplus
}  /* extern C */
#endif

#endif /* header */
