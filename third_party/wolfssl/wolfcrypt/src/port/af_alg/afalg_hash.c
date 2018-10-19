/* afalg_hash.c
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

#if defined(WOLFSSL_AFALG_HASH)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>

static const char WC_TYPE_HASH[] = "hash";

#if !defined(NO_SHA256)
#include <wolfssl/wolfcrypt/sha256.h>

static const char WC_NAME_SHA256[] = "sha256";


/* create AF_ALG sockets for SHA256 operation */
int wc_InitSha256_ex(wc_Sha256* sha, void* heap, int devId)
{
    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)devId; /* no async for now */
    XMEMSET(sha, 0, sizeof(wc_Sha256));
    sha->heap = heap;

    sha->len  = 0;
    sha->used = 0;
    sha->msg  = NULL;
    sha->alFd = -1;
    sha->rdFd = -1;

    sha->alFd = wc_Afalg_Socket();
    if (sha->alFd < 0) {
        return WC_AFALG_SOCK_E;
    }

    sha->rdFd = wc_Afalg_CreateRead(sha->alFd, WC_TYPE_HASH, WC_NAME_SHA256);
    if (sha->rdFd < 0) {
        close(sha->alFd);
        return WC_AFALG_SOCK_E;
    }

    return 0;
}


int wc_Sha256Update(wc_Sha256* sha, const byte* in, word32 sz)
{
    if (sha == NULL || (sz > 0 && in == NULL)) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    /* keep full message to hash at end instead of incremental updates */
    if (sha->len < sha->used + sz) {
        if (sha->msg == NULL) {
            sha->msg = (byte*)XMALLOC(sha->used + sz, sha->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
        } else {
            byte* pt = (byte*)XREALLOC(sha->msg, sha->used + sz, sha->heap,
                    DYNAMIC_TYPE_TMP_BUFFER);
            if (pt == NULL) {
                return MEMORY_E;
	    }
            sha->msg = pt;
        }
        if (sha->msg == NULL) {
            return MEMORY_E;
        }
        sha->len = sha->used + sz;
    }
    XMEMCPY(sha->msg + sha->used, in, sz);
    sha->used += sz;
#else
    int ret;

    if ((ret = (int)send(sha->rdFd, in, sz, MSG_MORE)) < 0) {
        return ret;
    }
#endif
    return 0;
}


int wc_Sha256Final(wc_Sha256* sha, byte* hash)
{
    int ret;

    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_AFALG_HASH_KEEP
    /* keep full message to hash at end instead of incremental updates */
    if ((ret = (int)send(sha->rdFd, sha->msg, sha->used, 0)) < 0) {
        return ret;
    }
    XFREE(sha->msg, sha->heap, DYNAMIC_TYPE_TMP_BUFFER);
    sha->msg = NULL;
#else
    if ((ret = (int)send(sha->rdFd, NULL, 0, 0)) < 0) {
        return ret;
    }
#endif

    if ((ret = (int)read(sha->rdFd, hash, WC_SHA256_DIGEST_SIZE)) !=
            WC_SHA256_DIGEST_SIZE) {
        return ret;
    }

    wc_Sha256Free(sha);
    return wc_InitSha256_ex(sha, sha->heap, 0);
}


int wc_Sha256GetHash(wc_Sha256* sha, byte* hash)
{
    int ret;

    if (sha == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

    (void)ret;
#ifdef WOLFSSL_AFALG_HASH_KEEP
    if ((ret = (int)send(sha->rdFd, sha->msg, sha->used, 0)) < 0) {
        return ret;
    }

    if ((ret = (int)read(sha->rdFd, hash, WC_SHA256_DIGEST_SIZE)) !=
            WC_SHA256_DIGEST_SIZE) {
        return ret;
    }
    return 0;
#else
    (void)sha;
    (void)hash;

    WOLFSSL_MSG("Compile with WOLFSSL_AFALG_HASH_KEEP for this feature");
    return NOT_COMPILED_IN;
#endif
}

int wc_Sha256Copy(wc_Sha256* src, wc_Sha256* dst)
{
    if (src == NULL || dst == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMCPY(dst, src, sizeof(wc_Sha256));

#ifdef WOLFSSL_AFALG_HASH_KEEP
    dst->msg = (byte*)XMALLOC(src->len, dst->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (dst->msg == NULL) {
        return MEMORY_E;
    }
    XMEMCPY(dst->msg, src->msg, src->len);
#endif

    dst->rdFd = accept(src->rdFd, NULL, 0);
    dst->alFd = accept(src->alFd, NULL, 0);

    if (dst->rdFd == -1 || dst->alFd == -1) {
        wc_Sha256Free(dst);
        return -1;
    }

    return 0;
}

#endif /* !NO_SHA256 */




#endif /* WOLFSSL_AFALG */
