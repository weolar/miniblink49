/* cryptodev.c
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This framework provides a central place for crypto hardware integration
   using the devId scheme. If not supported return `NOT_COMPILED_IN`. */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLF_CRYPTO_DEV

#include <wolfssl/wolfcrypt/cryptodev.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>


/* TODO: Consider linked list with mutex */
#ifndef MAX_CRYPTO_DEVICES
#define MAX_CRYPTO_DEVICES 8
#endif

typedef struct CryptoDev {
    int devId;
    CryptoDevCallbackFunc cb;
    void* ctx;
} CryptoDev;
static CryptoDev gCryptoDev[MAX_CRYPTO_DEVICES];

static CryptoDev* wc_CryptoDev_FindDevice(int devId)
{
    int i;
    for (i=0; i<MAX_CRYPTO_DEVICES; i++) {
        if (gCryptoDev[i].devId == devId)
            return &gCryptoDev[i];
    }
    return NULL;
}

void wc_CryptoDev_Init(void)
{
    int i;
    for (i=0; i<MAX_CRYPTO_DEVICES; i++)
        gCryptoDev[i].devId = INVALID_DEVID;
}

int wc_CryptoDev_RegisterDevice(int devId, CryptoDevCallbackFunc cb, void* ctx)
{
    /* find existing or new */
    CryptoDev* dev = wc_CryptoDev_FindDevice(devId);
    if (dev == NULL)
        dev = wc_CryptoDev_FindDevice(INVALID_DEVID);

    if (dev == NULL)
        return BUFFER_E; /* out of devices */

    dev->devId = devId;
    dev->cb = cb;
    dev->ctx = ctx;

    return 0;
}

void wc_CryptoDev_UnRegisterDevice(int devId)
{
    CryptoDev* dev = wc_CryptoDev_FindDevice(devId);
    if (dev) {
        XMEMSET(dev, 0, sizeof(*dev));
        dev->devId = INVALID_DEVID;
    }
}

#ifndef NO_RSA
int wc_CryptoDev_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_RSA;
            cryptoInfo.pk.rsa.in = in;
            cryptoInfo.pk.rsa.inLen = inLen;
            cryptoInfo.pk.rsa.out = out;
            cryptoInfo.pk.rsa.outLen = outLen;
            cryptoInfo.pk.rsa.type = type;
            cryptoInfo.pk.rsa.key = key;
            cryptoInfo.pk.rsa.rng = rng;

            ret = dev->cb(key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}

#ifdef WOLFSSL_KEY_GEN
int wc_CryptoDev_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_RSA_KEYGEN;
            cryptoInfo.pk.rsakg.key = key;
            cryptoInfo.pk.rsakg.size = size;
            cryptoInfo.pk.rsakg.e = e;
            cryptoInfo.pk.rsakg.rng = rng;

            ret = dev->cb(key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}
#endif
#endif /* !NO_RSA */

#ifdef HAVE_ECC
int wc_CryptoDev_MakeEccKey(WC_RNG* rng, int keySize, ecc_key* key, int curveId)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_EC_KEYGEN;
            cryptoInfo.pk.eckg.rng = rng;
            cryptoInfo.pk.eckg.size = keySize;
            cryptoInfo.pk.eckg.key = key;
            cryptoInfo.pk.eckg.curveId = curveId;

            ret = dev->cb(key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}

int wc_CryptoDev_Ecdh(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(private_key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_ECDH;
            cryptoInfo.pk.ecdh.private_key = private_key;
            cryptoInfo.pk.ecdh.public_key = public_key;
            cryptoInfo.pk.ecdh.out = out;
            cryptoInfo.pk.ecdh.outlen = outlen;

            ret = dev->cb(private_key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}

int wc_CryptoDev_EccSign(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_ECDSA_SIGN;
            cryptoInfo.pk.eccsign.in = in;
            cryptoInfo.pk.eccsign.inlen = inlen;
            cryptoInfo.pk.eccsign.out = out;
            cryptoInfo.pk.eccsign.outlen = outlen;
            cryptoInfo.pk.eccsign.rng = rng;
            cryptoInfo.pk.eccsign.key = key;

            ret = dev->cb(key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}

int wc_CryptoDev_EccVerify(const byte* sig, word32 siglen,
    const byte* hash, word32 hashlen, int* res, ecc_key* key)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(key->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_PK;
            cryptoInfo.pk.type = WC_PK_TYPE_ECDSA_VERIFY;
            cryptoInfo.pk.eccverify.sig = sig;
            cryptoInfo.pk.eccverify.siglen = siglen;
            cryptoInfo.pk.eccverify.hash = hash;
            cryptoInfo.pk.eccverify.hashlen = hashlen;
            cryptoInfo.pk.eccverify.res = res;
            cryptoInfo.pk.eccverify.key = key;

            ret = dev->cb(key->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}
#endif /* HAVE_ECC */

#if !defined(NO_AES) && defined(HAVE_AESGCM)
int wc_CryptoDev_AesGcmEncrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* iv, word32 ivSz,
                               byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(aes->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
            cryptoInfo.cipher.type = WC_CIPHER_AES_GCM;
            cryptoInfo.cipher.enc = 1;
            cryptoInfo.cipher.aesgcm_enc.aes       = aes;
            cryptoInfo.cipher.aesgcm_enc.out       = out;
            cryptoInfo.cipher.aesgcm_enc.in        = in;
            cryptoInfo.cipher.aesgcm_enc.sz        = sz;
            cryptoInfo.cipher.aesgcm_enc.iv        = iv;
            cryptoInfo.cipher.aesgcm_enc.ivSz      = ivSz;
            cryptoInfo.cipher.aesgcm_enc.authTag   = authTag;
            cryptoInfo.cipher.aesgcm_enc.authTagSz = authTagSz;
            cryptoInfo.cipher.aesgcm_enc.authIn    = authIn;
            cryptoInfo.cipher.aesgcm_enc.authInSz  = authInSz;

            ret = dev->cb(aes->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}

int wc_CryptoDev_AesGcmDecrypt(Aes* aes, byte* out,
                               const byte* in, word32 sz,
                               const byte* iv, word32 ivSz,
                               const byte* authTag, word32 authTagSz,
                               const byte* authIn, word32 authInSz)
{
    int ret = NOT_COMPILED_IN;
    CryptoDev* dev;

    /* locate registered callback */
    dev = wc_CryptoDev_FindDevice(aes->devId);
    if (dev) {
        if (dev->cb) {
            wc_CryptoInfo cryptoInfo;
            XMEMSET(&cryptoInfo, 0, sizeof(cryptoInfo));
            cryptoInfo.algo_type = WC_ALGO_TYPE_CIPHER;
            cryptoInfo.cipher.type = WC_CIPHER_AES_GCM;
            cryptoInfo.cipher.enc = 0;
            cryptoInfo.cipher.aesgcm_dec.aes       = aes;
            cryptoInfo.cipher.aesgcm_dec.out       = out;
            cryptoInfo.cipher.aesgcm_dec.in        = in;
            cryptoInfo.cipher.aesgcm_dec.sz        = sz;
            cryptoInfo.cipher.aesgcm_dec.iv        = iv;
            cryptoInfo.cipher.aesgcm_dec.ivSz      = ivSz;
            cryptoInfo.cipher.aesgcm_dec.authTag   = authTag;
            cryptoInfo.cipher.aesgcm_dec.authTagSz = authTagSz;
            cryptoInfo.cipher.aesgcm_dec.authIn    = authIn;
            cryptoInfo.cipher.aesgcm_dec.authInSz  = authInSz;

            ret = dev->cb(aes->devId, &cryptoInfo, dev->ctx);
        }
    }

    return ret;
}
#endif /* !NO_AES && HAVE_AESGCM */

/* call to support callback for entire buffer hash */
int wc_CryptoDev_Sha256Hash(const byte* data, word32 len, byte* hash)
{
    (void)data;
    (void)len;
    (void)hash;
    return NOT_COMPILED_IN;
}

#endif /* WOLF_CRYPTO_DEV */
