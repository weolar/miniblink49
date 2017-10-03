/* curve25519.c
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


 /* Based On Daniel J Bernstein's curve25519 Public Domain ref10 work. */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_CURVE25519

#include <wolfssl/wolfcrypt/curve25519.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#if defined(FREESCALE_LTC_ECC)
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif

const curve25519_set_type curve25519_sets[] = {
    {
        32,
        "CURVE25519",
    }
};

int wc_curve25519_make_key(WC_RNG* rng, int keysize, curve25519_key* key)
{
#ifdef FREESCALE_LTC_ECC
    const ECPoint* basepoint = wc_curve25519_GetBasePoint();
#else
    unsigned char basepoint[CURVE25519_KEYSIZE] = {9};
#endif
    int  ret;

    if (key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    /* currently only a key size of 32 bytes is used */
    if (keysize != CURVE25519_KEYSIZE)
        return ECC_BAD_ARG_E;

    /* random number for private key */
    ret = wc_RNG_GenerateBlock(rng, key->k.point, keysize);
    if (ret != 0)
        return ret;

    /* Clamp the private key */
    key->k.point[0] &= 248;
    key->k.point[CURVE25519_KEYSIZE-1] &= 63; /* same &=127 because |=64 after */
    key->k.point[CURVE25519_KEYSIZE-1] |= 64;

    /* compute public key */
    #ifdef FREESCALE_LTC_ECC
        ret = wc_curve25519(&key->p, key->k.point, basepoint, kLTC_Weierstrass); /* input basepoint on Weierstrass curve */
    #else
        ret = curve25519(key->p.point, key->k.point, basepoint);
    #endif
    if (ret != 0) {
        ForceZero(key->k.point, keysize);
        ForceZero(key->p.point, keysize);
        return ret;
    }

    return ret;
}

#ifdef HAVE_CURVE25519_SHARED_SECRET

int wc_curve25519_shared_secret(curve25519_key* private_key,
                                curve25519_key* public_key,
                                byte* out, word32* outlen)
{
    return wc_curve25519_shared_secret_ex(private_key, public_key,
                                          out, outlen, EC25519_BIG_ENDIAN);
}

int wc_curve25519_shared_secret_ex(curve25519_key* private_key,
                                   curve25519_key* public_key,
                                   byte* out, word32* outlen, int endian)
{
    #ifdef FREESCALE_LTC_ECC
        ECPoint o = {{0}};
    #else
        unsigned char o[CURVE25519_KEYSIZE];
    #endif
    int ret = 0;

    /* sanity check */
    if (private_key == NULL || public_key == NULL ||
        out == NULL || outlen == NULL || *outlen < CURVE25519_KEYSIZE)
        return BAD_FUNC_ARG;

    /* avoid implementation fingerprinting */
    if (public_key->p.point[CURVE25519_KEYSIZE-1] > 0x7F)
        return ECC_BAD_ARG_E;

    #ifdef FREESCALE_LTC_ECC
        ret = wc_curve25519(&o, private_key->k.point, &public_key->p, kLTC_Curve25519 /* input point P on Curve25519 */);
    #else
        ret = curve25519(o, private_key->k.point, public_key->p.point);
    #endif
    if (ret != 0) {
        #ifdef FREESCALE_LTC_ECC
            ForceZero(o.point, CURVE25519_KEYSIZE);
            ForceZero(o.pointY, CURVE25519_KEYSIZE);
        #else
            ForceZero(o, CURVE25519_KEYSIZE);
        #endif
        return ret;
    }

    if (endian == EC25519_BIG_ENDIAN) {
        int i;
        /* put shared secret key in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++)
            #ifdef FREESCALE_LTC_ECC
                out[i] = o.point[CURVE25519_KEYSIZE - i -1];
            #else
                out[i] = o[CURVE25519_KEYSIZE - i -1];
            #endif
    }
    else /* put shared secret key in Little Endian format */
        #ifdef FREESCALE_LTC_ECC
            XMEMCPY(out, o.point, CURVE25519_KEYSIZE);
        #else
            XMEMCPY(out, o, CURVE25519_KEYSIZE);
        #endif

    *outlen = CURVE25519_KEYSIZE;

    #ifdef FREESCALE_LTC_ECC
        ForceZero(o.point, CURVE25519_KEYSIZE);
        ForceZero(o.pointY, CURVE25519_KEYSIZE);
    #else
        ForceZero(o, CURVE25519_KEYSIZE);
    #endif

    return ret;
}

#endif /* HAVE_CURVE25519_SHARED_SECRET */

#ifdef HAVE_CURVE25519_KEY_EXPORT

/* export curve25519 public key (Big endian)
 * return 0 on success */
int wc_curve25519_export_public(curve25519_key* key, byte* out, word32* outLen)
{
    return wc_curve25519_export_public_ex(key, out, outLen, EC25519_BIG_ENDIAN);
}

/* export curve25519 public key (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_public_ex(curve25519_key* key, byte* out,
                                   word32* outLen, int endian)
{
    word32 keySz;

    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    /* check size of outgoing key */
    keySz  = wc_curve25519_size(key);

    /* check and set outgoing key size */
    if (*outLen < keySz) {
        *outLen = keySz;
        return ECC_BAD_ARG_E;
    }
    *outLen = keySz;

    if (endian == EC25519_BIG_ENDIAN) {
        int i;

        /* read keys in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++)
            out[i] = key->p.point[CURVE25519_KEYSIZE - i - 1];
    }
    else
        XMEMCPY(out, key->p.point, keySz);

    return 0;
}

#endif /* HAVE_CURVE25519_KEY_EXPORT */

#ifdef HAVE_CURVE25519_KEY_IMPORT

/* import curve25519 public key (Big endian)
 *  return 0 on success */
int wc_curve25519_import_public(const byte* in, word32 inLen,
                                curve25519_key* key)
{
    return wc_curve25519_import_public_ex(in, inLen, key, EC25519_BIG_ENDIAN);
}

/* import curve25519 public key (Big or Little endian)
 * return 0 on success */
int wc_curve25519_import_public_ex(const byte* in, word32 inLen,
                                curve25519_key* key, int endian)
{
    word32 keySz;

    /* sanity check */
    if (key == NULL || in == NULL)
        return BAD_FUNC_ARG;

    /* check size of incoming keys */
    keySz = wc_curve25519_size(key);
    if (inLen != keySz)
       return ECC_BAD_ARG_E;

    if (endian == EC25519_BIG_ENDIAN) {
        int i;

        /* read keys in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++)
            key->p.point[i] = in[CURVE25519_KEYSIZE - i - 1];
    }
    else
        XMEMCPY(key->p.point, in, inLen);

    key->dp = &curve25519_sets[0];

    /* LTC needs also Y coordinate - let's compute it */
    #ifdef FREESCALE_LTC_ECC
        ltc_pkha_ecc_point_t ltcPoint;
        ltcPoint.X = &key->p.point[0];
        ltcPoint.Y = &key->p.pointY[0];
        LTC_PKHA_Curve25519ComputeY(&ltcPoint);
    #endif

    return 0;
}

#endif /* HAVE_CURVE25519_KEY_IMPORT */


#ifdef HAVE_CURVE25519_KEY_EXPORT

/* export curve25519 private key only raw (Big endian)
 * outLen is in/out size
 * return 0 on success */
int wc_curve25519_export_private_raw(curve25519_key* key, byte* out,
                                     word32* outLen)
{
    return wc_curve25519_export_private_raw_ex(key, out, outLen,
                                               EC25519_BIG_ENDIAN);
}

/* export curve25519 private key only raw (Big or Little endian)
 * outLen is in/out size
 * return 0 on success */
int wc_curve25519_export_private_raw_ex(curve25519_key* key, byte* out,
                                        word32* outLen, int endian)
{
    word32 keySz;

    /* sanity check */
    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    /* check size of outgoing buffer */
    keySz = wc_curve25519_size(key);
    if (*outLen < keySz) {
        *outLen = keySz;
        return ECC_BAD_ARG_E;
    }
    *outLen = keySz;

    if (endian == EC25519_BIG_ENDIAN) {
        int i;

        /* put the key in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++)
            out[i] = key->k.point[CURVE25519_KEYSIZE - i - 1];
    }
    else
        XMEMCPY(out, key->k.point, keySz);

    return 0;
}

/* curve25519 key pair export (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_key_raw(curve25519_key* key,
                                 byte* priv, word32 *privSz,
                                 byte* pub, word32 *pubSz)
{
    return wc_curve25519_export_key_raw_ex(key, priv, privSz,
                                           pub, pubSz, EC25519_BIG_ENDIAN);
}

/* curve25519 key pair export (Big or Little endian)
 * return 0 on success */
int wc_curve25519_export_key_raw_ex(curve25519_key* key,
                                    byte* priv, word32 *privSz,
                                    byte* pub, word32 *pubSz,
                                    int endian)
{
    int ret;

    /* export private part */
    ret = wc_curve25519_export_private_raw_ex(key, priv, privSz, endian);
    if (ret != 0)
        return ret;

    /* export public part */
    return wc_curve25519_export_public_ex(key, pub, pubSz, endian);
}

#endif /* HAVE_CURVE25519_KEY_EXPORT */

#ifdef HAVE_CURVE25519_KEY_IMPORT

/* curve25519 private key import (Big endian)
 * Public key to match private key needs to be imported too
 * return 0 on success */
int wc_curve25519_import_private_raw(const byte* priv, word32 privSz,
                                     const byte* pub, word32 pubSz,
                                     curve25519_key* key)
{
    return wc_curve25519_import_private_raw_ex(priv, privSz, pub, pubSz,
                                               key, EC25519_BIG_ENDIAN);
}

/* curve25519 private key import (Big or Little endian)
 * Public key to match private key needs to be imported too
 * return 0 on success */
int wc_curve25519_import_private_raw_ex(const byte* priv, word32 privSz,
                                        const byte* pub, word32 pubSz,
                                        curve25519_key* key, int endian)
{
    int ret;

    /* import private part */
    ret = wc_curve25519_import_private_ex(priv, privSz, key, endian);
    if (ret != 0)
        return ret;

    /* import public part */
    return wc_curve25519_import_public_ex(pub, pubSz, key, endian);
}

/* curve25519 private key import only. (Big endian)
 * return 0 on success */
int wc_curve25519_import_private(const byte* priv, word32 privSz,
                                 curve25519_key* key)
{
    return wc_curve25519_import_private_ex(priv, privSz,
                                           key, EC25519_BIG_ENDIAN);
}

/* curve25519 private key import only. (Big or Little endian)
 * return 0 on success */
int wc_curve25519_import_private_ex(const byte* priv, word32 privSz,
                                    curve25519_key* key, int endian)
{
    /* sanity check */
    if (key == NULL || priv == NULL)
        return BAD_FUNC_ARG;

    /* check size of incoming keys */
    if ((int)privSz != wc_curve25519_size(key))
        return ECC_BAD_ARG_E;

    if (endian == EC25519_BIG_ENDIAN) {
        int i;

        /* read the key in Big Endian format */
        for (i = 0; i < CURVE25519_KEYSIZE; i++)
            key->k.point[i] = priv[CURVE25519_KEYSIZE - i - 1];
    }
    else
        XMEMCPY(key->k.point, priv, privSz);

    key->dp = &curve25519_sets[0];

    /* Clamp the key */
    key->k.point[0] &= 248;
    key->k.point[privSz-1] &= 63; /* same &=127 because |=64 after */
    key->k.point[privSz-1] |= 64;

    return 0;
}

#endif /* HAVE_CURVE25519_KEY_IMPORT */


int wc_curve25519_init(curve25519_key* key)
{
    if (key == NULL)
       return BAD_FUNC_ARG;

    /* currently the format for curve25519 */
    key->dp = &curve25519_sets[0];

    XMEMSET(key->k.point, 0, key->dp->size);
    XMEMSET(key->p.point, 0, key->dp->size);
    #ifdef FREESCALE_LTC_ECC
        XMEMSET(key->k.pointY, 0, key->dp->size);
        XMEMSET(key->p.pointY, 0, key->dp->size);
    #endif
    return 0;
}


/* Clean the memory of a key */
void wc_curve25519_free(curve25519_key* key)
{
   if (key == NULL)
       return;

   key->dp = NULL;
   ForceZero(key->p.point, sizeof(key->p.point));
   ForceZero(key->k.point, sizeof(key->k.point));
   #ifdef FREESCALE_LTC_ECC
       ForceZero(key->p.point, sizeof(key->p.pointY));
       ForceZero(key->k.point, sizeof(key->k.pointY));
   #endif
}


/* get key size */
int wc_curve25519_size(curve25519_key* key)
{
    if (key == NULL)
        return 0;

    return key->dp->size;
}

#endif /*HAVE_CURVE25519*/

