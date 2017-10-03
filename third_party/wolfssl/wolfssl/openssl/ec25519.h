/* ec25519.h */

#ifndef WOLFSSL_EC25519_H_
#define WOLFSSL_EC25519_H_

#ifdef __cplusplus
extern "C" {
#endif

WOLFSSL_API
int wolfSSL_EC25519_generate_key(unsigned char *priv, unsigned int *privSz,
                                 unsigned char *pub, unsigned int *pubSz);

WOLFSSL_API
int wolfSSL_EC25519_shared_key(unsigned char *shared, unsigned int *sharedSz,
                               const unsigned char *priv, unsigned int privSz,
                               const unsigned char *pub, unsigned int pubSz);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* header */
