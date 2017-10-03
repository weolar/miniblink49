/* ed25519.c
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


 /* Based On Daniel J Bernstein's ed25519 Public Domain ref10 work. */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

/* in case user set HAVE_ED25519 there */
#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_ED25519

#include <wolfssl/wolfcrypt/ed25519.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/hash.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef FREESCALE_LTC_ECC
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif

/* generate an ed25519 key pair.
 * returns 0 on success
 */
int wc_ed25519_make_key(WC_RNG* rng, int keySz, ed25519_key* key)
{
    byte  az[ED25519_PRV_KEY_SIZE];
    int   ret;
#if !defined(FREESCALE_LTC_ECC)
    ge_p3 A;
#endif

    if (rng == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* ed25519 has 32 byte key sizes */
    if (keySz != ED25519_KEY_SIZE)
        return BAD_FUNC_ARG;

    ret  = wc_RNG_GenerateBlock(rng, key->k, ED25519_KEY_SIZE);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Hash(key->k, ED25519_KEY_SIZE, az);
    if (ret != 0) {
        ForceZero(key->k, ED25519_KEY_SIZE);
        return ret;
    }

    /* apply clamp */
    az[0]  &= 248;
    az[31] &= 63; /* same than az[31] &= 127 because of az[31] |= 64 */
    az[31] |= 64;

#ifdef FREESCALE_LTC_ECC
    ltc_pkha_ecc_point_t publicKey = {0};
    publicKey.X = key->pointX;
    publicKey.Y = key->pointY;
    LTC_PKHA_Ed25519_PointMul(LTC_PKHA_Ed25519_BasePoint(), az, ED25519_KEY_SIZE, &publicKey, kLTC_Ed25519 /* result on Ed25519 */);
    LTC_PKHA_Ed25519_Compress(&publicKey, key->p);
#else
    ge_scalarmult_base(&A, az);
    ge_p3_tobytes(key->p, &A);
#endif
    /* put public key after private key, on the same buffer */
    XMEMMOVE(key->k + ED25519_KEY_SIZE, key->p, ED25519_PUB_KEY_SIZE);

    return ret;
}


#ifdef HAVE_ED25519_SIGN
/*
    in     contains the message to sign
    inlen  is the length of the message to sign
    out    is the buffer to write the signature
    outLen [in/out] input size of out buf
                     output gets set as the final length of out
    key    is the ed25519 key to use when signing
    return 0 on success
 */
int wc_ed25519_sign_msg(const byte* in, word32 inlen, byte* out,
                        word32 *outLen, ed25519_key* key)
{
#ifdef FREESCALE_LTC_ECC
    byte   tempBuf[ED25519_PRV_KEY_SIZE];
#else
    ge_p3  R;
#endif
    byte   nonce[SHA512_DIGEST_SIZE];
    byte   hram[SHA512_DIGEST_SIZE];
    byte   az[ED25519_PRV_KEY_SIZE];
    Sha512 sha;
    int    ret;

    /* sanity check on arguments */
    if (in == NULL || out == NULL || outLen == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* check and set up out length */
    if (*outLen < ED25519_SIG_SIZE) {
        *outLen = ED25519_SIG_SIZE;
        return BUFFER_E;
    }
    *outLen = ED25519_SIG_SIZE;

    /* step 1: create nonce to use where nonce is r in
       r = H(h_b, ... ,h_2b-1,M) */
    ret = wc_Sha512Hash(key->k, ED25519_KEY_SIZE, az);
    if (ret != 0)
        return ret;

    /* apply clamp */
    az[0]  &= 248;
    az[31] &= 63; /* same than az[31] &= 127 because of az[31] |= 64 */
    az[31] |= 64;

    ret = wc_InitSha512(&sha);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, az + ED25519_KEY_SIZE, ED25519_KEY_SIZE);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, in, inlen);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Final(&sha, nonce);
    if (ret != 0)
        return ret;

#ifdef FREESCALE_LTC_ECC
    ltc_pkha_ecc_point_t ltcPoint = {0};
    ltcPoint.X = &tempBuf[0];
    ltcPoint.Y = &tempBuf[32];
    LTC_PKHA_sc_reduce(nonce);
    LTC_PKHA_Ed25519_PointMul(LTC_PKHA_Ed25519_BasePoint(), nonce, ED25519_KEY_SIZE, &ltcPoint, kLTC_Ed25519 /* result on Ed25519 */);
    LTC_PKHA_Ed25519_Compress(&ltcPoint, out);
#else
    sc_reduce(nonce);

    /* step 2: computing R = rB where rB is the scalar multiplication of
       r and B */
    ge_scalarmult_base(&R,nonce);
    ge_p3_tobytes(out,&R);
#endif

    /* step 3: hash R + public key + message getting H(R,A,M) then
       creating S = (r + H(R,A,M)a) mod l */
    ret = wc_InitSha512(&sha);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, out, ED25519_SIG_SIZE/2);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, key->p, ED25519_PUB_KEY_SIZE);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, in, inlen);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Final(&sha, hram);
    if (ret != 0)
        return ret;

#ifdef FREESCALE_LTC_ECC
    LTC_PKHA_sc_reduce(hram);
    LTC_PKHA_sc_muladd(out + (ED25519_SIG_SIZE/2), hram, az, nonce);
#else
    sc_reduce(hram);
    sc_muladd(out + (ED25519_SIG_SIZE/2), hram, az, nonce);
#endif

    return ret;
}

#endif /* HAVE_ED25519_SIGN */

#ifdef HAVE_ED25519_VERIFY

/*
   sig     is array of bytes containing the signature
   siglen  is the length of sig byte array
   msg     the array of bytes containing the message
   msglen  length of msg array
   res     will be 1 on successful verify and 0 on unsuccessful
   return  0 and res of 1 on success
*/
int wc_ed25519_verify_msg(const byte* sig, word32 siglen, const byte* msg,
                          word32 msglen, int* res, ed25519_key* key)
{
    byte   rcheck[ED25519_KEY_SIZE];
    byte   h[SHA512_DIGEST_SIZE];
#ifndef FREESCALE_LTC_ECC
    ge_p3  A;
    ge_p2  R;
#endif
    int    ret;
    Sha512 sha;

    /* sanity check on arguments */
    if (sig == NULL || msg == NULL || res == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* set verification failed by default */
    *res = 0;

    /* check on basics needed to verify signature */
    if (siglen < ED25519_SIG_SIZE || (sig[ED25519_SIG_SIZE-1] & 224))
        return BAD_FUNC_ARG;

    /* uncompress A (public key), test if valid, and negate it */
#ifndef FREESCALE_LTC_ECC
    if (ge_frombytes_negate_vartime(&A, key->p) != 0)
        return BAD_FUNC_ARG;
#endif

    /* find H(R,A,M) and store it as h */
    ret  = wc_InitSha512(&sha);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, sig,    ED25519_SIG_SIZE/2);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, key->p, ED25519_PUB_KEY_SIZE);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Update(&sha, msg,    msglen);
    if (ret != 0)
        return ret;
    ret = wc_Sha512Final(&sha,  h);
    if (ret != 0)
        return ret;

#ifdef FREESCALE_LTC_ECC
    LTC_PKHA_sc_reduce(h);
    LTC_PKHA_SignatureForVerify(rcheck, h, sig + (ED25519_SIG_SIZE/2), key);
#else
    sc_reduce(h);

    /*
       Uses a fast single-signature verification SB = R + H(R,A,M)A becomes
       SB - H(R,A,M)A saving decompression of R
    */
    ret = ge_double_scalarmult_vartime(&R, h, &A, sig + (ED25519_SIG_SIZE/2));
    if (ret != 0)
        return ret;

    ge_tobytes(rcheck, &R);
#endif /* FREESCALE_LTC_ECC */

    /* comparison of R created to R in sig */
    ret = ConstantCompare(rcheck, sig, ED25519_SIG_SIZE/2);
    if (ret != 0)
        return SIG_VERIFY_E;

    /* set the verification status */
    *res = 1;

    return ret;
}

#endif /* HAVE_ED25519_VERIFY */


/* initialize information and memory for key */
int wc_ed25519_init(ed25519_key* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(key, 0, sizeof(ed25519_key));

    return 0;
}


/* clear memory of key */
void wc_ed25519_free(ed25519_key* key)
{
    if (key == NULL)
        return;

    ForceZero(key, sizeof(ed25519_key));
}


#ifdef HAVE_ED25519_KEY_EXPORT

/*
    outLen should contain the size of out buffer when input. outLen is than set
    to the final output length.
    returns 0 on success
 */
int wc_ed25519_export_public(ed25519_key* key, byte* out, word32* outLen)
{
    /* sanity check on arguments */
    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    if (*outLen < ED25519_PUB_KEY_SIZE) {
        *outLen = ED25519_PUB_KEY_SIZE;
        return BUFFER_E;
    }

    *outLen = ED25519_PUB_KEY_SIZE;
    XMEMCPY(out, key->p, ED25519_PUB_KEY_SIZE);

    return 0;
}

#endif /* HAVE_ED25519_KEY_EXPORT */


#ifdef HAVE_ED25519_KEY_IMPORT
/*
    Imports a compressed/uncompressed public key.
    in    the byte array containing the public key
    inLen the length of the byte array being passed in
    key   ed25519 key struct to put the public key in
 */
int wc_ed25519_import_public(const byte* in, word32 inLen, ed25519_key* key)
{
    int    ret;

    /* sanity check on arguments */
    if (in == NULL || key == NULL)
        return BAD_FUNC_ARG;

    if (inLen < ED25519_PUB_KEY_SIZE)
        return BAD_FUNC_ARG;

    /* compressed prefix according to draft
       http://www.ietf.org/id/draft-koch-eddsa-for-openpgp-02.txt */
    if (in[0] == 0x40 && inLen > ED25519_PUB_KEY_SIZE) {
        /* key is stored in compressed format so just copy in */
        XMEMCPY(key->p, (in + 1), ED25519_PUB_KEY_SIZE);
#ifdef FREESCALE_LTC_ECC
        /* recover X coordinate */
        ltc_pkha_ecc_point_t pubKey;
        pubKey.X = key->pointX;
        pubKey.Y = key->pointY;
        LTC_PKHA_Ed25519_PointDecompress(key->p, ED25519_PUB_KEY_SIZE, &pubKey);
#endif
        return 0;
    }

    /* importing uncompressed public key */
    if (in[0] == 0x04 && inLen > 2*ED25519_PUB_KEY_SIZE) {
#ifdef FREESCALE_LTC_ECC
        /* reverse bytes for little endian byte order */
        for (int i = 0; i < ED25519_KEY_SIZE; i++)
        {
            key->pointX[i] = *(in + ED25519_KEY_SIZE - i);
            key->pointY[i] = *(in + 2*ED25519_KEY_SIZE - i);
        }
        XMEMCPY(key->p, key->pointY, ED25519_KEY_SIZE);
        ret = 0;
#else
        /* pass in (x,y) and store compressed key */
        ret = ge_compress_key(key->p, in+1,
                              in+1+ED25519_PUB_KEY_SIZE, ED25519_PUB_KEY_SIZE);
#endif /* FREESCALE_LTC_ECC */
        return ret;
    }

    /* if not specified compressed or uncompressed check key size
       if key size is equal to compressed key size copy in key */
    if (inLen == ED25519_PUB_KEY_SIZE) {
        XMEMCPY(key->p, in, ED25519_PUB_KEY_SIZE);
#ifdef FREESCALE_LTC_ECC
        /* recover X coordinate */
        ltc_pkha_ecc_point_t pubKey;
        pubKey.X = key->pointX;
        pubKey.Y = key->pointY;
        LTC_PKHA_Ed25519_PointDecompress(key->p, ED25519_PUB_KEY_SIZE, &pubKey);
#endif
        return 0;
    }

    /* bad public key format */
    return BAD_FUNC_ARG;
}


/*
    For importing a private key.
 */
int wc_ed25519_import_private_only(const byte* priv, word32 privSz,
                                                               ed25519_key* key)
{
    /* sanity check on arguments */
    if (priv == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* key size check */
    if (privSz < ED25519_KEY_SIZE)
        return BAD_FUNC_ARG;

    XMEMCPY(key->k, priv, ED25519_KEY_SIZE);

    return 0;
}

/*
    For importing a private key and its associated public key.
 */
int wc_ed25519_import_private_key(const byte* priv, word32 privSz,
                                const byte* pub, word32 pubSz, ed25519_key* key)
{
    int    ret;

    /* sanity check on arguments */
    if (priv == NULL || pub == NULL || key == NULL)
        return BAD_FUNC_ARG;

    /* key size check */
    if (privSz < ED25519_KEY_SIZE || pubSz < ED25519_PUB_KEY_SIZE)
        return BAD_FUNC_ARG;

    /* import public key */
    ret = wc_ed25519_import_public(pub, pubSz, key);
    if (ret != 0)
        return ret;

    /* make the private key (priv + pub) */
    XMEMCPY(key->k, priv, ED25519_KEY_SIZE);
    XMEMCPY(key->k + ED25519_KEY_SIZE, key->p, ED25519_PUB_KEY_SIZE);

    return ret;
}

#endif /* HAVE_ED25519_KEY_IMPORT */


#ifdef HAVE_ED25519_KEY_EXPORT

/*
 export private key only (secret part so 32 bytes)
 outLen should contain the size of out buffer when input. outLen is than set
 to the final output length.
 returns 0 on success
 */
int wc_ed25519_export_private_only(ed25519_key* key, byte* out, word32* outLen)
{
    /* sanity checks on arguments */
    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    if (*outLen < ED25519_KEY_SIZE) {
        *outLen = ED25519_KEY_SIZE;
        return BUFFER_E;
    }

    *outLen = ED25519_KEY_SIZE;
    XMEMCPY(out, key->k, ED25519_KEY_SIZE);

    return 0;
}

/*
 export private key, including public part
 outLen should contain the size of out buffer when input. outLen is than set
 to the final output length.
 returns 0 on success
 */
int wc_ed25519_export_private(ed25519_key* key, byte* out, word32* outLen)
{
    /* sanity checks on arguments */
    if (key == NULL || out == NULL || outLen == NULL)
        return BAD_FUNC_ARG;

    if (*outLen < ED25519_PRV_KEY_SIZE) {
        *outLen = ED25519_PRV_KEY_SIZE;
        return BUFFER_E;
    }

    *outLen = ED25519_PRV_KEY_SIZE;
    XMEMCPY(out, key->k, ED25519_PRV_KEY_SIZE);

    return 0;
}

/* export full private key and public key
   return 0 on success
 */
int wc_ed25519_export_key(ed25519_key* key,
                          byte* priv, word32 *privSz,
                          byte* pub, word32 *pubSz)
{
    int ret;

    /* export 'full' private part */
    ret = wc_ed25519_export_private(key, priv, privSz);
    if (ret != 0)
        return ret;

    /* export public part */
    ret = wc_ed25519_export_public(key, pub, pubSz);

    return ret;
}

#endif /* HAVE_ED25519_KEY_EXPORT */

/* check the private and public keys match */
int wc_ed25519_check_key(ed25519_key* key)
{
    /* TODO: Perform check of private and public key */
    (void)key;

    return 0;
}

/* returns the private key size (secret only) in bytes */
int wc_ed25519_size(ed25519_key* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    return ED25519_KEY_SIZE;
}

/* returns the private key size (secret + public) in bytes */
int wc_ed25519_priv_size(ed25519_key* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    return ED25519_PRV_KEY_SIZE;
}

/* returns the compressed key size in bytes (public key) */
int wc_ed25519_pub_size(ed25519_key* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    return ED25519_PUB_KEY_SIZE;
}

/* returns the size of signature in bytes */
int wc_ed25519_sig_size(ed25519_key* key)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    return ED25519_SIG_SIZE;
}

#endif /* HAVE_ED25519 */

