/* afalg_aes.c
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#if !defined(NO_AES) && defined(WOLFSSL_AFALG)

#include <wolfssl/wolfcrypt/aes.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>

#include <sys/uio.h> /* for readv */

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

static const char WC_TYPE_SYMKEY[] = "skcipher";
static const char WC_NAME_AESCBC[] = "cbc(aes)";

static int wc_AesSetup(Aes* aes, const char* type, const char* name, int ivSz, int aadSz)
{
    aes->rdFd = wc_Afalg_CreateRead(aes->alFd, type, name);
    if (aes->rdFd < 0) {
        WOLFSSL_MSG("Unable to accept and get AF_ALG read socket");
        aes->rdFd = WC_SOCK_NOTSET;
        return aes->rdFd;
    }

    if (setsockopt(aes->alFd, SOL_ALG, ALG_SET_KEY, (byte*)aes->key, aes->keylen) != 0) {
        WOLFSSL_MSG("Unable to set AF_ALG key");
        aes->rdFd = WC_SOCK_NOTSET;
        return WC_AFALG_SOCK_E;
    }
    ForceZero((byte*)aes->key, sizeof(aes->key));

    /* set up CMSG headers */
    XMEMSET((byte*)&(aes->msg), 0, sizeof(struct msghdr));

    aes->msg.msg_control = (byte*)(aes->key); /* use existing key buffer for
                                               * control buffer */
    aes->msg.msg_controllen = CMSG_SPACE(4);
    if (aadSz > 0) {
        aes->msg.msg_controllen += CMSG_SPACE(4);
    }
    if (ivSz > 0) {
        aes->msg.msg_controllen += CMSG_SPACE((sizeof(struct af_alg_iv) + ivSz));
    }

    if (wc_Afalg_SetOp(CMSG_FIRSTHDR(&(aes->msg)), aes->dir) < 0) {
        WOLFSSL_MSG("Error with setting AF_ALG operation");
        aes->rdFd = WC_SOCK_NOTSET;
        return -1;
    }

    return 0;
}


int wc_AesSetKey(Aes* aes, const byte* userKey, word32 keylen,
    const byte* iv, int dir)
{
#if defined(AES_MAX_KEY_SIZE)
    const word32 max_key_len = (AES_MAX_KEY_SIZE / 8);
#endif

    if (aes == NULL ||
            !((keylen == 16) || (keylen == 24) || (keylen == 32))) {
        return BAD_FUNC_ARG;
    }

#if defined(AES_MAX_KEY_SIZE)
    /* Check key length */
    if (keylen > max_key_len) {
        return BAD_FUNC_ARG;
    }
#endif
    aes->keylen = keylen;
    aes->rounds = keylen/4 + 6;

#ifdef WOLFSSL_AES_COUNTER
    aes->left = 0;
#endif

    aes->rdFd = WC_SOCK_NOTSET;
    aes->alFd = wc_Afalg_Socket();
    if (aes->alFd < 0) {
         WOLFSSL_MSG("Unable to open an AF_ALG socket");
         return WC_AFALG_SOCK_E;
    }

    /* save key until type is known i.e. CBC, ECB, ... */
    XMEMCPY((byte*)(aes->key), userKey, keylen);
    aes->dir = dir;

    return wc_AesSetIV(aes, iv);
}


/* AES-CBC */
#ifdef HAVE_AES_CBC
    int wc_AesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        struct cmsghdr* cmsg;
        struct iovec    iov;
        int ret;

        if (aes == NULL || out == NULL || in == NULL) {
            return BAD_FUNC_ARG;
        }

        if (aes->rdFd == WC_SOCK_NOTSET) {
                if ((ret = wc_AesSetup(aes, WC_TYPE_SYMKEY, WC_NAME_AESCBC,
                                AES_IV_SIZE, 0)) != 0) {
                WOLFSSL_MSG("Error with first time setup of AF_ALG socket");
                return ret;
            }
        }

        sz = sz - (sz % AES_BLOCK_SIZE);
        if ((sz / AES_BLOCK_SIZE) > 0) {
            /* update IV */
            cmsg = CMSG_FIRSTHDR(&(aes->msg));
            ret = wc_Afalg_SetIv(CMSG_NXTHDR(&(aes->msg), cmsg),
                    (byte*)(aes->reg), AES_IV_SIZE);
            if (ret < 0) {
                WOLFSSL_MSG("Error setting IV");
                return ret;
            }

            /* set data to be encrypted */
            iov.iov_base = (byte*)in;
            iov.iov_len  = sz;

            aes->msg.msg_iov    = &iov;
            aes->msg.msg_iovlen = 1; /* # of iov structures */

            ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
            if (ret < 0) {
                return ret;
            }
            ret = (int)read(aes->rdFd, out, sz);
            if (ret < 0) {
                return ret;
            }

            /* set IV for next CBC call */
            XMEMCPY(aes->reg, out + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        }

        return 0;
    }

    #ifdef HAVE_AES_DECRYPT
    int wc_AesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
    {
        struct cmsghdr* cmsg;
        struct iovec    iov;
        int ret;

        if (aes == NULL || out == NULL || in == NULL
                                       || sz % AES_BLOCK_SIZE != 0) {
            return BAD_FUNC_ARG;
        }

        if (aes->rdFd == WC_SOCK_NOTSET) {
            if ((ret = wc_AesSetup(aes, WC_TYPE_SYMKEY, WC_NAME_AESCBC,
                                AES_IV_SIZE, 0)) != 0) {
                return ret;
            }
        }

        if ((sz / AES_BLOCK_SIZE) > 0) {
            /* update IV */
            cmsg = CMSG_FIRSTHDR(&(aes->msg));
            ret = wc_Afalg_SetIv(CMSG_NXTHDR(&(aes->msg), cmsg),
                    (byte*)(aes->reg), AES_IV_SIZE);
            if (ret != 0) {
                return ret;
            }

            /* set data to be decrypted */
            iov.iov_base = (byte*)in;
            iov.iov_len  = sz;

            aes->msg.msg_iov    = &iov;
            aes->msg.msg_iovlen = 1; /* # of iov structures */

            /* set IV for next CBC call */
            XMEMCPY(aes->reg, in + sz - AES_BLOCK_SIZE, AES_BLOCK_SIZE);

            ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
            if (ret < 0) {
                return ret;
            }
            ret = (int)read(aes->rdFd, out, sz);
            if (ret < 0) {
                return ret;
            }

        }

        return 0;
    }
    #endif

#endif /* HAVE_AES_CBC */


/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT) || defined(HAVE_AES_ECB)

static const char WC_NAME_AESECB[] = "ecb(aes)";

/* common code between ECB encrypt and decrypt
 * returns 0 on success */
static int wc_Afalg_AesDirect(Aes* aes, byte* out, const byte* in, word32 sz)
{
        struct iovec    iov;
        int ret;

     if (aes == NULL || out == NULL || in == NULL) {
         return BAD_FUNC_ARG;
     }

        if (aes->rdFd == WC_SOCK_NOTSET) {
                if ((ret = wc_AesSetup(aes, WC_TYPE_SYMKEY, WC_NAME_AESECB,
                                0, 0)) != 0) {
                WOLFSSL_MSG("Error with first time setup of AF_ALG socket");
                return ret;
            }
        }

            /* set data to be encrypted */
            iov.iov_base = (byte*)in;
            iov.iov_len  = sz;

            aes->msg.msg_iov    = &iov;
            aes->msg.msg_iovlen = 1; /* # of iov structures */

            ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
            if (ret < 0) {
                return ret;
            }
            ret = (int)read(aes->rdFd, out, sz);
            if (ret < 0) {
                return ret;
            }

        return 0;
}
#endif


#if defined(WOLFSSL_AES_DIRECT)
void wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in)
{
    if (wc_Afalg_AesDirect(aes, out, in, AES_BLOCK_SIZE) != 0) {
        WOLFSSL_MSG("Error with AES encrypt direct call");
    }
}


void wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in)
{
    if (wc_Afalg_AesDirect(aes, out, in, AES_BLOCK_SIZE) != 0) {
        WOLFSSL_MSG("Error with AES decrypt direct call");
    }
}


int wc_AesSetKeyDirect(Aes* aes, const byte* userKey, word32 keylen,
                    const byte* iv, int dir)
{
    return wc_AesSetKey(aes, userKey, keylen, iv, dir);
}
#endif


/* AES-CTR */
#if defined(WOLFSSL_AES_COUNTER)
        static const char WC_NAME_AESCTR[] = "ctr(aes)";

        /* Increment AES counter */
        static WC_INLINE void IncrementAesCounter(byte* inOutCtr)
        {
            /* in network byte order so start at end and work back */
            int i;
            for (i = AES_BLOCK_SIZE - 1; i >= 0; i--) {
                if (++inOutCtr[i])  /* we're done unless we overflow */
                    return;
            }
        }

        int wc_AesCtrEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
        {
            struct cmsghdr* cmsg;
            struct iovec    iov[2];
            int   ret;
            byte* tmp;

            if (aes == NULL || out == NULL || in == NULL) {
                return BAD_FUNC_ARG;
            }

            /* consume any unused bytes left in aes->tmp */
            tmp = (byte*)aes->tmp + AES_BLOCK_SIZE - aes->left;
            while (aes->left && sz) {
               *(out++) = *(in++) ^ *(tmp++);
               aes->left--;
               sz--;
            }

            if (aes->rdFd == WC_SOCK_NOTSET) {
                if ((ret = wc_AesSetup(aes, WC_TYPE_SYMKEY, WC_NAME_AESCTR,
                                    AES_IV_SIZE, 0)) != 0) {
                    WOLFSSL_MSG("Error with first time setup of AF_ALG socket");
                    return ret;
                }
            }

            if (sz > 0) {
                aes->left = sz % AES_BLOCK_SIZE;

                /* clear previously leftover data */
                tmp = (byte*)aes->tmp;
                XMEMSET(tmp, 0, AES_BLOCK_SIZE);

                /* update IV */
                cmsg = CMSG_FIRSTHDR(&(aes->msg));
                ret = wc_Afalg_SetIv(CMSG_NXTHDR(&(aes->msg), cmsg),
                        (byte*)(aes->reg), AES_IV_SIZE);
                if (ret < 0) {
                    WOLFSSL_MSG("Error setting IV");
                    return ret;
                }

                /* set data to be encrypted */
                iov[0].iov_base = (byte*)in;
                iov[0].iov_len  = sz - aes->left;

                iov[1].iov_base = tmp;
                if (aes->left > 0) {
                    XMEMCPY(tmp, in + sz - aes->left, aes->left);
                    iov[1].iov_len  = AES_BLOCK_SIZE;
                }
                else {
                    iov[1].iov_len  = 0;
                }

                aes->msg.msg_iov    = iov;
                aes->msg.msg_iovlen = 2; /* # of iov structures */

                ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
                if (ret < 0) {
                    return ret;
                }


                /* set buffers to hold result and left over stream */
                iov[0].iov_base = (byte*)out;
                iov[0].iov_len  = sz - aes->left;

                iov[1].iov_base = tmp;
                if (aes->left > 0) {
                    iov[1].iov_len  = AES_BLOCK_SIZE;
                }
                else {
                    iov[1].iov_len  = 0;
                }

                ret = (int)readv(aes->rdFd, iov, 2);
                if (ret < 0) {
                    return ret;
                }

                if (aes->left > 0) {
                    XMEMCPY(out + sz - aes->left, tmp, aes->left);
                    aes->left = AES_BLOCK_SIZE - aes->left;
                }
            }

            /* adjust counter after call to hardware */
            while (sz >= AES_BLOCK_SIZE) {
                IncrementAesCounter((byte*)aes->reg);
                sz  -= AES_BLOCK_SIZE;
            }

            if (aes->left > 0) {
                IncrementAesCounter((byte*)aes->reg);
            }

            return 0;
        }
#endif /* WOLFSSL_AES_COUNTER */


#ifdef HAVE_AESGCM

static const char WC_TYPE_AEAD[]   = "aead";
static const char WC_NAME_AESGCM[] = "gcm(aes)";

#ifndef WC_SYSTEM_AESGCM_IV
/* size of IV allowed on system for AES-GCM */
#define WC_SYSTEM_AESGCM_IV 12
#endif

#ifndef WOLFSSL_MAX_AUTH_TAG_SZ
/* size of tag is restricted by system for AES-GCM
 * check 'cat /proc/crypto' to see restricted size */
#define WOLFSSL_MAX_AUTH_TAG_SZ 16
#endif

int wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len)
{
#if defined(AES_MAX_KEY_SIZE)
    const word32 max_key_len = (AES_MAX_KEY_SIZE / 8);
#endif

    if (aes == NULL ||
            !((len == 16) || (len == 24) || (len == 32))) {
        return BAD_FUNC_ARG;
    }

#if defined(AES_MAX_KEY_SIZE)
    /* Check key length */
    if (len > max_key_len) {
        return BAD_FUNC_ARG;
    }
#endif
    aes->keylen = len;
    aes->rounds = len/4 + 6;

    aes->rdFd = WC_SOCK_NOTSET;
    aes->alFd = wc_Afalg_Socket();
    if (aes->alFd < 0) {
         WOLFSSL_MSG("Unable to open an AF_ALG socket");
         return WC_AFALG_SOCK_E;
    }

    /* save key until direction is known i.e. encrypt or decrypt */
    XMEMCPY((byte*)(aes->key), key, len);

    return 0;
}



int wc_AesGcmEncrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                   const byte* iv, word32 ivSz,
                   byte* authTag, word32 authTagSz,
                   const byte* authIn, word32 authInSz)
{
    struct cmsghdr* cmsg;
    struct iovec    iov[3];
    int ret;
    struct msghdr* msg;
    byte scratch[16];

    /* argument checks */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != WC_SYSTEM_AESGCM_IV || authTagSz > WOLFSSL_MAX_AUTH_TAG_SZ) {
        WOLFSSL_MSG("IV/AAD size not supported on system");
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    if (aes->rdFd == WC_SOCK_NOTSET) {
        aes->dir = AES_ENCRYPTION;
        if ((ret = wc_AesSetup(aes, WC_TYPE_AEAD, WC_NAME_AESGCM, ivSz,
                        authInSz)) != 0) {
            WOLFSSL_MSG("Error with first time setup of AF_ALG socket");
            return ret;
        }

        /* note that if the ivSz was to change, the msg_controllen would need
           reset */

        /* set auth tag
         * @TODO case where tag size changes between calls? */
        ret = setsockopt(aes->alFd, SOL_ALG, ALG_SET_AEAD_AUTHSIZE, NULL,
                authTagSz);
        if (ret != 0) {
        perror("set tag");
            WOLFSSL_MSG("Unable to set AF_ALG tag size ");
            return WC_AFALG_SOCK_E;
        }
    }

    msg = &(aes->msg);
    cmsg = CMSG_FIRSTHDR(msg);
    cmsg = CMSG_NXTHDR(msg, cmsg);

    /* set IV and AAD size */
    ret = wc_Afalg_SetIv(cmsg, (byte*)iv, ivSz);
    if (ret < 0) {
        WOLFSSL_MSG("Error setting IV");
        return ret;

    }

    if (authInSz > 0) {
        cmsg = CMSG_NXTHDR(msg, cmsg);
        ret = wc_Afalg_SetAad(cmsg, authInSz);
        if (ret < 0) {
            WOLFSSL_MSG("Unable to set AAD size");
            return ret;
        }
    }

    /* set data to be encrypted*/
    iov[0].iov_base = (byte*)authIn;
    iov[0].iov_len  = authInSz;

    iov[1].iov_base = (byte*)in;
    iov[1].iov_len  = sz;

    aes->msg.msg_iov    = iov;
    aes->msg.msg_iovlen = 2; /* # of iov structures */

    ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
    if (ret < 0) {
        perror("sendmsg error");
        return ret;
    }

    /* first 16 bytes was all 0's */
    iov[0].iov_base = scratch;
    iov[0].iov_len  = authInSz;

    iov[1].iov_base = out;
    iov[1].iov_len  = sz;

    iov[2].iov_base = authTag;
    iov[2].iov_len  = authTagSz;

    ret = (int)readv(aes->rdFd, iov, 3);
    if (ret < 0) {
        perror("read error");
        return ret;
    }

    return 0;
}

#if defined(HAVE_AES_DECRYPT) || defined(HAVE_AESGCM_DECRYPT)
int wc_AesGcmDecrypt(Aes* aes, byte* out, const byte* in, word32 sz,
                     const byte* iv, word32 ivSz,
                     const byte* authTag, word32 authTagSz,
                     const byte* authIn, word32 authInSz)
{
    struct cmsghdr* cmsg;
    struct iovec    iov[3];
    int ret;
    struct msghdr* msg;
    byte scratch[16];

    /* argument checks */
    if (aes == NULL || authTagSz > AES_BLOCK_SIZE) {
        return BAD_FUNC_ARG;
    }

    if (ivSz != WC_SYSTEM_AESGCM_IV || authTagSz > WOLFSSL_MAX_AUTH_TAG_SZ) {
        WOLFSSL_MSG("IV/AAD size not supported on system");
        return BAD_FUNC_ARG;
    }

    if (authTagSz < WOLFSSL_MIN_AUTH_TAG_SZ) {
        WOLFSSL_MSG("GcmEncrypt authTagSz too small error");
        return BAD_FUNC_ARG;
    }

    if (aes->rdFd == WC_SOCK_NOTSET) {
        aes->dir = AES_DECRYPTION;
        if ((ret = wc_AesSetup(aes, WC_TYPE_AEAD, WC_NAME_AESGCM, ivSz,
                        authInSz)) != 0) {
            WOLFSSL_MSG("Error with first time setup of AF_ALG socket");
            return ret;
        }

        /* set auth tag
         * @TODO case where tag size changes between calls? */
        ret = setsockopt(aes->alFd, SOL_ALG, ALG_SET_AEAD_AUTHSIZE, NULL,
                authTagSz);
        if (ret != 0) {
            WOLFSSL_MSG("Unable to set AF_ALG tag size ");
            return WC_AFALG_SOCK_E;
        }
    }


    /* set IV and AAD size */
    msg = &(aes->msg);
    cmsg = CMSG_FIRSTHDR(msg);
    cmsg = CMSG_NXTHDR(msg, cmsg);
    ret = wc_Afalg_SetIv(cmsg, (byte*)iv, ivSz);
    if (ret < 0) {
        return ret;
    }

    if (authInSz > 0) {
        cmsg = CMSG_NXTHDR(msg, cmsg);
        ret = wc_Afalg_SetAad(cmsg, authInSz);
        if (ret < 0) {
            return ret;
        }
    }

    /* set data to be decrypted*/
    iov[0].iov_base = (byte*)authIn;
    iov[0].iov_len  = authInSz;

    iov[1].iov_base = (byte*)in;
    iov[1].iov_len  = sz;

    iov[2].iov_base = (byte*)authTag;
    iov[2].iov_len  = authTagSz;

    aes->msg.msg_iov    = iov;
    aes->msg.msg_iovlen = 3; /* # of iov structures */

    ret = (int)sendmsg(aes->rdFd, &(aes->msg), 0);
    if (ret < 0) {
        return ret;
    }

    iov[0].iov_base = scratch;
    iov[0].iov_len  = authInSz;

    iov[1].iov_base = out;
    iov[1].iov_len  = sz;

    ret = (int)readv(aes->rdFd, iov, 2);
    if (ret < 0) {
        return AES_GCM_AUTH_E;
    }

    return 0;
}
#endif /* HAVE_AES_DECRYPT || HAVE_AESGCM_DECRYPT */
#endif /* HAVE_AESGCM */


#ifdef HAVE_AES_ECB
int wc_AesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_Afalg_AesDirect(aes, out, in, sz);
}


int wc_AesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz)
{
    return wc_Afalg_AesDirect(aes, out, in, sz);
}
#endif /* HAVE_AES_ECB */
#endif /* !NO_AES && WOLFSSL_AFALG */

