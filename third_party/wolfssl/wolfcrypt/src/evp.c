/* evp.c
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
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

#if !defined(WOLFSSL_EVP_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning evp.c does not need to be compiled seperatly from ssl.c
    #endif
#else

static unsigned int cipherType(const WOLFSSL_EVP_CIPHER *cipher);


/* Getter function for cipher key length
 *
 * c  WOLFSSL_EVP_CIPHER structure to get key length from
 *
 * NOTE: OpenSSL_add_all_ciphers() should be called first before using this
 *       function
 *
 * Returns size of key in bytes
 */
int wolfSSL_EVP_Cipher_key_length(const WOLFSSL_EVP_CIPHER* c)
{
    WOLFSSL_ENTER("wolfSSL_EVP_Cipher_key_length");

    if (c == NULL) {
        return 0;
    }

    switch (cipherType(c)) {
    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
      case AES_128_CBC_TYPE: return 16;
      case AES_192_CBC_TYPE: return 24;
      case AES_256_CBC_TYPE: return 32;
  #endif
  #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
      case AES_128_CTR_TYPE: return 16;
      case AES_192_CTR_TYPE: return 24;
      case AES_256_CTR_TYPE: return 32;
  #endif
  #if !defined(NO_AES) && defined(HAVE_AES_ECB)
      case AES_128_ECB_TYPE: return 16;
      case AES_192_ECB_TYPE: return 24;
      case AES_256_ECB_TYPE: return 32;
  #endif
  #ifndef NO_DES3
      case DES_CBC_TYPE:      return 8;
      case DES_EDE3_CBC_TYPE: return 24;
      case DES_ECB_TYPE:      return 8;
      case DES_EDE3_ECB_TYPE: return 24;
  #endif
      default:
          return 0;
      }
}


WOLFSSL_API int  wolfSSL_EVP_EncryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 1);
}

WOLFSSL_API int  wolfSSL_EVP_EncryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        WOLFSSL_ENGINE *impl,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    (void) impl;
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 1);
}

WOLFSSL_API int  wolfSSL_EVP_DecryptInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    WOLFSSL_ENTER("wolfSSL_EVP_CipherInit");
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 0);
}

WOLFSSL_API int  wolfSSL_EVP_DecryptInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                        const WOLFSSL_EVP_CIPHER* type,
                                        WOLFSSL_ENGINE *impl,
                                        const unsigned char* key,
                                        const unsigned char* iv)
{
    (void) impl;
    WOLFSSL_ENTER("wolfSSL_EVP_DecryptInit");
    return wolfSSL_EVP_CipherInit(ctx, type, (byte*)key, (byte*)iv, 0);
}


WOLFSSL_API WOLFSSL_EVP_CIPHER_CTX *wolfSSL_EVP_CIPHER_CTX_new(void)
{
	WOLFSSL_EVP_CIPHER_CTX *ctx = (WOLFSSL_EVP_CIPHER_CTX*)XMALLOC(sizeof *ctx,
                                                 NULL, DYNAMIC_TYPE_TMP_BUFFER);
	if (ctx) {
      WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_new");
		  wolfSSL_EVP_CIPHER_CTX_init(ctx);
  }
	return ctx;
}

WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_free(WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    if (ctx) {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_free");
		    wolfSSL_EVP_CIPHER_CTX_cleanup(ctx);
		    XFREE(ctx, NULL, DYNAMIC_TYPE_TMP_BUFFER);
		}
}

WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_CTX_mode(const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
  if (ctx == NULL) return 0;
  return ctx->flags & WOLFSSL_EVP_CIPH_MODE;
}

WOLFSSL_API int  wolfSSL_EVP_EncryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_EncryptFinal");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else
        return WOLFSSL_FAILURE;
}


WOLFSSL_API int  wolfSSL_EVP_CipherInit_ex(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                    const WOLFSSL_EVP_CIPHER* type,
                                    WOLFSSL_ENGINE *impl,
                                    const unsigned char* key,
                                    const unsigned char* iv,
                                    int enc)
{
    (void)impl;
    return wolfSSL_EVP_CipherInit(ctx, type, key, iv, enc);
}

WOLFSSL_API int  wolfSSL_EVP_EncryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && ctx->enc) {
        WOLFSSL_ENTER("wolfSSL_EVP_EncryptFinal_ex");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
    else
        return WOLFSSL_FAILURE;
}

WOLFSSL_API int  wolfSSL_EVP_DecryptFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
  if (ctx && ctx->enc)
      return WOLFSSL_FAILURE;
  else {
      WOLFSSL_ENTER("wolfSSL_EVP_DecryptFinal");
      return wolfSSL_EVP_CipherFinal(ctx, out, outl);
  }
}

WOLFSSL_API int  wolfSSL_EVP_DecryptFinal_ex(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    if (ctx && ctx->enc)
        return WOLFSSL_FAILURE;
    else {
        WOLFSSL_ENTER("wolfSSL_EVP_CipherFinal_ex");
        return wolfSSL_EVP_CipherFinal(ctx, out, outl);
    }
}


WOLFSSL_API int wolfSSL_EVP_DigestInit_ex(WOLFSSL_EVP_MD_CTX* ctx,
                                     const WOLFSSL_EVP_MD* type,
                                     WOLFSSL_ENGINE *impl)
{
    (void) impl;
    WOLFSSL_ENTER("wolfSSL_EVP_DigestInit_ex");
    return wolfSSL_EVP_DigestInit(ctx, type);
}

#ifdef DEBUG_WOLFSSL_EVP
#define PRINT_BUF(b, sz) { int _i; for(_i=0; _i<(sz); _i++) { \
  printf("%02x(%c),", (b)[_i], (b)[_i]); if ((_i+1)%8==0)printf("\n");}}
#else
#define PRINT_BUF(b, sz)
#endif

static int fillBuff(WOLFSSL_EVP_CIPHER_CTX *ctx, const unsigned char *in, int sz)
{
    int fill;

    if (sz > 0) {
        if ((sz+ctx->bufUsed) > ctx->block_size) {
            fill = ctx->block_size - ctx->bufUsed;
        } else {
            fill = sz;
        }
        XMEMCPY(&(ctx->buf[ctx->bufUsed]), in, fill);
        ctx->bufUsed += fill;
        return fill;
    } else return 0;
}

static int evpCipherBlock(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out,
                                   const unsigned char *in, int inl)
{
    int ret = 0;

    switch (ctx->cipherType) {
    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
        case AES_128_CBC_TYPE:
        case AES_192_CBC_TYPE:
        case AES_256_CBC_TYPE:
            if (ctx->enc)
                ret = wc_AesCbcEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesCbcDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
        case AES_128_CTR_TYPE:
        case AES_192_CTR_TYPE:
        case AES_256_CTR_TYPE:
            ret = wc_AesCtrEncrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #if !defined(NO_AES) && defined(HAVE_AES_ECB)
        case AES_128_ECB_TYPE:
        case AES_192_ECB_TYPE:
        case AES_256_ECB_TYPE:
            if (ctx->enc)
                ret = wc_AesEcbEncrypt(&ctx->cipher.aes, out, in, inl);
            else
                ret = wc_AesEcbDecrypt(&ctx->cipher.aes, out, in, inl);
            break;
    #endif
    #ifndef NO_DES3
        case DES_CBC_TYPE:
            if (ctx->enc)
                ret = wc_Des_CbcEncrypt(&ctx->cipher.des, out, in, inl);
            else
                ret = wc_Des_CbcDecrypt(&ctx->cipher.des, out, in, inl);
            break;
        case DES_EDE3_CBC_TYPE:
            if (ctx->enc)
                ret = wc_Des3_CbcEncrypt(&ctx->cipher.des3, out, in, inl);
            else
                ret = wc_Des3_CbcDecrypt(&ctx->cipher.des3, out, in, inl);
            break;
        #if defined(WOLFSSL_DES_ECB)
        case DES_ECB_TYPE:
            ret = wc_Des_EcbEncrypt(&ctx->cipher.des, out, in, inl);
            break;
        case DES_EDE3_ECB_TYPE:
            ret = wc_Des3_EcbEncrypt(&ctx->cipher.des3, out, in, inl);
            break;
        #endif
    #endif
    #ifndef NO_RC4
        case ARC4_TYPE:
            wc_Arc4Process(&ctx->cipher.arc4, out, in, inl);
        break;
    #endif
        default:
            return WOLFSSL_FAILURE;
    }

    if (ret != 0)
        return WOLFSSL_FAILURE; /* failure */

    (void)in;
    (void)inl;
    (void)out;

    return WOLFSSL_SUCCESS; /* success */
}

WOLFSSL_API int wolfSSL_EVP_CipherUpdate(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl,
                                   const unsigned char *in, int inl)
{
    int blocks;
    int fill;

    if ((ctx == NULL) || (inl < 0) ||
        (outl == NULL)|| (out == NULL) || (in == NULL)) return BAD_FUNC_ARG;
    WOLFSSL_ENTER("wolfSSL_EVP_CipherUpdate");

    *outl = 0;
    if (inl == 0) return WOLFSSL_SUCCESS;

    if (ctx->bufUsed > 0) { /* concatinate them if there is anything */
        fill = fillBuff(ctx, in, inl);
        inl -= fill;
        in  += fill;
    }
    if ((ctx->enc == 0)&& (ctx->lastUsed == 1)) {
        PRINT_BUF(ctx->lastBlock, ctx->block_size);
        XMEMCPY(out, ctx->lastBlock, ctx->block_size);
        *outl+= ctx->block_size;
        out  += ctx->block_size;
    }
    if (ctx->bufUsed == ctx->block_size) {
        /* the buff is full, flash out */
        PRINT_BUF(ctx->buf, ctx->block_size);
        if (evpCipherBlock(ctx, out, ctx->buf, ctx->block_size) == 0)
            return WOLFSSL_FAILURE;
        PRINT_BUF(out, ctx->block_size);
        if (ctx->enc == 0) {
            ctx->lastUsed = 1;
            XMEMCPY(ctx->lastBlock, out, ctx->block_size);
        } else {
            *outl+= ctx->block_size;
            out  += ctx->block_size;
        }
        ctx->bufUsed = 0;
    }

    blocks = inl / ctx->block_size;
    if (blocks > 0) {
        /* process blocks */
        if (evpCipherBlock(ctx, out, in, blocks * ctx->block_size) == 0)
            return WOLFSSL_FAILURE;
        PRINT_BUF(in, ctx->block_size*blocks);
        PRINT_BUF(out,ctx->block_size*blocks);
        inl  -= ctx->block_size * blocks;
        in   += ctx->block_size * blocks;
        if (ctx->enc == 0) {
            if ((ctx->flags & WOLFSSL_EVP_CIPH_NO_PADDING) ||
                    (ctx->block_size == 1)) {
                ctx->lastUsed = 0;
                XMEMCPY(ctx->lastBlock, &out[ctx->block_size * blocks], ctx->block_size);
                *outl+= ctx->block_size * blocks;
            } else {
                ctx->lastUsed = 1;
                XMEMCPY(ctx->lastBlock, &out[ctx->block_size * (blocks-1)], ctx->block_size);
                *outl+= ctx->block_size * (blocks-1);
            }
        } else {
            *outl+= ctx->block_size * blocks;
        }
    }
    if (inl > 0) {
        /* put fraction into buff */
        fillBuff(ctx, in, inl);
        /* no increase of outl */
    }

    (void)out; /* silence warning in case not read */

    return WOLFSSL_SUCCESS;
}

static void padBlock(WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    int i;
    for (i = ctx->bufUsed; i < ctx->block_size; i++)
        ctx->buf[i] = (byte)(ctx->block_size - ctx->bufUsed);
}

static int checkPad(WOLFSSL_EVP_CIPHER_CTX *ctx, unsigned char *buff)
{
    int i;
    int n;
    n = buff[ctx->block_size-1];
    if (n > ctx->block_size) return -1;
    for (i = 0; i < n; i++) {
        if (buff[ctx->block_size-i-1] != n)
            return -1;
    }
    return ctx->block_size - n;
}

WOLFSSL_API int  wolfSSL_EVP_CipherFinal(WOLFSSL_EVP_CIPHER_CTX *ctx,
                                   unsigned char *out, int *outl)
{
    int fl;
    if (ctx == NULL || out == NULL) return BAD_FUNC_ARG;
    WOLFSSL_ENTER("wolfSSL_EVP_CipherFinal");
    if (ctx->flags & WOLFSSL_EVP_CIPH_NO_PADDING) {
        if (ctx->bufUsed != 0) return WOLFSSL_FAILURE;
        *outl = 0;
        return WOLFSSL_SUCCESS;
    }
    if (ctx->enc) {
        if (ctx->block_size == 1) {
            *outl = 0;
            return WOLFSSL_SUCCESS;
        }
        if ((ctx->bufUsed >= 0) && (ctx->block_size != 1)) {
            padBlock(ctx);
            PRINT_BUF(ctx->buf, ctx->block_size);
            if (evpCipherBlock(ctx, out, ctx->buf, ctx->block_size) == 0)
                return WOLFSSL_FAILURE;

            PRINT_BUF(out, ctx->block_size);
            *outl = ctx->block_size;
        }
    } else {
        if (ctx->block_size == 1) {
            *outl = 0;
            return WOLFSSL_SUCCESS;
        }
        if (ctx->lastUsed) {
            PRINT_BUF(ctx->lastBlock, ctx->block_size);
            if ((fl = checkPad(ctx, ctx->lastBlock)) >= 0) {
                XMEMCPY(out, ctx->lastBlock, fl);
                *outl = fl;
            } else return 0;
        }
    }
    return WOLFSSL_SUCCESS;
}

WOLFSSL_API int wolfSSL_EVP_CIPHER_CTX_block_size(const WOLFSSL_EVP_CIPHER_CTX *ctx)
{
    if (ctx == NULL) return BAD_FUNC_ARG;
    switch (ctx->cipherType) {
#if !defined(NO_AES) || !defined(NO_DES3)
#if !defined(NO_AES) && defined(HAVE_AES_CBC)
    case AES_128_CBC_TYPE:
    case AES_192_CBC_TYPE:
    case AES_256_CBC_TYPE:
#endif
#if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
    case AES_128_CTR_TYPE:
    case AES_192_CTR_TYPE:
    case AES_256_CTR_TYPE:
#endif
#if !defined(NO_AES)
    case AES_128_ECB_TYPE:
    case AES_192_ECB_TYPE:
    case AES_256_ECB_TYPE:
#endif
#ifndef NO_DES3
    case DES_CBC_TYPE:
    case DES_ECB_TYPE:
    case DES_EDE3_CBC_TYPE:
    case DES_EDE3_ECB_TYPE:
#endif
        return ctx->block_size;
#endif /* !NO_AES || !NO_DES3 */
    default:
        return 0;
    }
}

static unsigned int cipherType(const WOLFSSL_EVP_CIPHER *cipher)
{
    if (cipher == NULL) return 0; /* dummy for #ifdef */
  #ifndef NO_DES3
      else if (XSTRNCMP(cipher, EVP_DES_CBC, EVP_DES_SIZE) == 0)
          return DES_CBC_TYPE;
      else if (XSTRNCMP(cipher, EVP_DES_EDE3_CBC, EVP_DES_EDE3_SIZE) == 0)
          return DES_EDE3_CBC_TYPE;
  #if !defined(NO_DES3)
      else if (XSTRNCMP(cipher, EVP_DES_ECB, EVP_DES_SIZE) == 0)
          return DES_ECB_TYPE;
      else if (XSTRNCMP(cipher, EVP_DES_EDE3_ECB, EVP_DES_EDE3_SIZE) == 0)
          return DES_EDE3_ECB_TYPE;
  #endif /* NO_DES3 && HAVE_AES_ECB */
  #endif

  #if !defined(NO_AES) && defined(HAVE_AES_CBC)
      #ifdef WOLFSSL_AES_128
      else if (XSTRNCMP(cipher, EVP_AES_128_CBC, EVP_AES_SIZE) == 0)
          return AES_128_CBC_TYPE;
      #endif
      #ifdef WOLFSSL_AES_192
      else if (XSTRNCMP(cipher, EVP_AES_192_CBC, EVP_AES_SIZE) == 0)
          return AES_192_CBC_TYPE;
      #endif
      #ifdef WOLFSSL_AES_256
      else if (XSTRNCMP(cipher, EVP_AES_256_CBC, EVP_AES_SIZE) == 0)
          return AES_256_CBC_TYPE;
      #endif
  #endif /* !NO_AES && HAVE_AES_CBC */
  #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
      #ifdef WOLFSSL_AES_128
      else if (XSTRNCMP(cipher, EVP_AES_128_CTR, EVP_AES_SIZE) == 0)
          return AES_128_CTR_TYPE;
      #endif
      #ifdef WOLFSSL_AES_192
      else if (XSTRNCMP(cipher, EVP_AES_192_CTR, EVP_AES_SIZE) == 0)
          return AES_192_CTR_TYPE;
      #endif
      #ifdef WOLFSSL_AES_256
      else if (XSTRNCMP(cipher, EVP_AES_256_CTR, EVP_AES_SIZE) == 0)
          return AES_256_CTR_TYPE;
      #endif
  #endif /* !NO_AES && HAVE_AES_CBC */
  #if !defined(NO_AES) && defined(HAVE_AES_ECB)
      #ifdef WOLFSSL_AES_128
      else if (XSTRNCMP(cipher, EVP_AES_128_ECB, EVP_AES_SIZE) == 0)
          return AES_128_ECB_TYPE;
      #endif
      #ifdef WOLFSSL_AES_192
      else if (XSTRNCMP(cipher, EVP_AES_192_ECB, EVP_AES_SIZE) == 0)
          return AES_192_ECB_TYPE;
      #endif
      #ifdef WOLFSSL_AES_256
      else if (XSTRNCMP(cipher, EVP_AES_256_ECB, EVP_AES_SIZE) == 0)
          return AES_256_ECB_TYPE;
      #endif
  #endif /* !NO_AES && HAVE_AES_CBC */
      else return 0;
}

WOLFSSL_API int wolfSSL_EVP_CIPHER_block_size(const WOLFSSL_EVP_CIPHER *cipher)
{
  if (cipher == NULL) return BAD_FUNC_ARG;
  switch (cipherType(cipher)) {
  #if !defined(NO_AES) && defined(HAVE_AES_CBC)
      case AES_128_CBC_TYPE:
      case AES_192_CBC_TYPE:
      case AES_256_CBC_TYPE:
                             return AES_BLOCK_SIZE;
  #endif
  #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
      case AES_128_CTR_TYPE:
      case AES_192_CTR_TYPE:
      case AES_256_CTR_TYPE:
                             return AES_BLOCK_SIZE;
  #endif
  #if !defined(NO_AES) && defined(HAVE_AES_ECB)
      case AES_128_ECB_TYPE:
      case AES_192_ECB_TYPE:
      case AES_256_ECB_TYPE:
                             return AES_BLOCK_SIZE;
  #endif
  #ifndef NO_DES3
      case DES_CBC_TYPE: return 8;
      case DES_EDE3_CBC_TYPE: return 8;
      case DES_ECB_TYPE: return 8;
      case DES_EDE3_ECB_TYPE: return 8;
  #endif
      default:
          return 0;
      }
}

unsigned long WOLFSSL_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher)
{
    switch (cipherType(cipher)) {
    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
        case AES_128_CBC_TYPE:
        case AES_192_CBC_TYPE:
        case AES_256_CBC_TYPE:
            return WOLFSSL_EVP_CIPH_CBC_MODE;
    #endif
    #if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER)
        case AES_128_CTR_TYPE:
        case AES_192_CTR_TYPE:
        case AES_256_CTR_TYPE:
            return WOLFSSL_EVP_CIPH_CTR_MODE;
    #endif
    #if !defined(NO_AES)
        case AES_128_ECB_TYPE:
        case AES_192_ECB_TYPE:
        case AES_256_ECB_TYPE:
            return WOLFSSL_EVP_CIPH_ECB_MODE;
    #endif
    #ifndef NO_DES3
        case DES_CBC_TYPE:
        case DES_EDE3_CBC_TYPE:
            return WOLFSSL_EVP_CIPH_CBC_MODE;
        case DES_ECB_TYPE:
        case DES_EDE3_ECB_TYPE:
            return WOLFSSL_EVP_CIPH_ECB_MODE;
    #endif
    #ifndef NO_RC4
        case ARC4_TYPE:
            return EVP_CIPH_STREAM_CIPHER;
    #endif
        default:
            return 0;
        }
}

WOLFSSL_API unsigned long WOLFSSL_EVP_CIPHER_mode(const WOLFSSL_EVP_CIPHER *cipher)
{
  if (cipher == NULL) return 0;
  return WOLFSSL_CIPHER_mode(cipher);
}

WOLFSSL_API void wolfSSL_EVP_CIPHER_CTX_set_flags(WOLFSSL_EVP_CIPHER_CTX *ctx, int flags)
{
    if (ctx != NULL) {
        ctx->flags = flags;
    }
}

WOLFSSL_API unsigned long wolfSSL_EVP_CIPHER_flags(const WOLFSSL_EVP_CIPHER *cipher)
{
  if (cipher == NULL) return 0;
  return WOLFSSL_CIPHER_mode(cipher);
}

WOLFSSL_API int  wolfSSL_EVP_CIPHER_CTX_set_padding(WOLFSSL_EVP_CIPHER_CTX *ctx, int padding)
{
  if (ctx == NULL) return BAD_FUNC_ARG;
  if (padding) {
      ctx->flags &= ~WOLFSSL_EVP_CIPH_NO_PADDING;
  }
  else {
      ctx->flags |=  WOLFSSL_EVP_CIPH_NO_PADDING;
  }
  return 1;
}

WOLFSSL_API int wolfSSL_EVP_add_digest(const WOLFSSL_EVP_MD *digest)
{
    (void)digest;
    /* nothing to do */
    return 0;
}


/* Frees the WOLFSSL_EVP_PKEY_CTX passed in.
 *
 * return WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_free(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_CTX_free");
    XFREE(ctx, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    return WOLFSSL_SUCCESS;
}


/* Creates a new WOLFSSL_EVP_PKEY_CTX structure.
 *
 * pkey  key structure to use with new WOLFSSL_EVP_PEKY_CTX
 * e     engine to use. It should be NULL at this time.
 *
 * return the new structure on success and NULL if failed.
 */
WOLFSSL_API WOLFSSL_EVP_PKEY_CTX *wolfSSL_EVP_PKEY_CTX_new(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_ENGINE *e)
{
    WOLFSSL_EVP_PKEY_CTX* ctx;

    if (pkey == NULL) return 0;
    if (e != NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_CTX_new");

    ctx = (WOLFSSL_EVP_PKEY_CTX*)XMALLOC(sizeof(WOLFSSL_EVP_PKEY_CTX), NULL,
            DYNAMIC_TYPE_PUBLIC_KEY);
    if (ctx == NULL) return NULL;
    XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_PKEY_CTX));
    ctx->pkey = pkey;
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    ctx->padding = RSA_PKCS1_PADDING;
#endif

    return ctx;
}


/* Sets the type of RSA padding to use.
 *
 * ctx     structure to set padding in.
 * padding RSA padding type
 *
 * returns WOLFSSL_SUCCESS on success.
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_CTX_set_rsa_padding(WOLFSSL_EVP_PKEY_CTX *ctx, int padding)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_CTX_set_rsa_padding");
    ctx->padding = padding;
    return WOLFSSL_SUCCESS;
}


/* Uses the WOLFSSL_EVP_PKEY_CTX to decrypt a buffer.
 *
 * ctx    structure to decrypt with
 * out    buffer to hold the results
 * outlen initially holds size of out buffer and gets set to decrypt result size
 * in     buffer decrypt
 * inlen  length of in buffer
 *
 * returns WOLFSSL_SUCCESS on success.
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_decrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outlen,
                     const unsigned char *in, size_t inlen)
{
    int len;

    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_decrypt");

    (void)out;
    (void)outlen;
    (void)in;
    (void)inlen;
    (void)len;

    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        len = wolfSSL_RSA_private_decrypt((int)inlen, (unsigned char*)in, out,
              ctx->pkey->rsa, ctx->padding);
        if (len < 0) break;
        else {
            *outlen = len;
            return WOLFSSL_SUCCESS;
        }
#endif /* NO_RSA */

    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Initialize a WOLFSSL_EVP_PKEY_CTX structure for decryption
 *
 * ctx    WOLFSSL_EVP_PKEY_CTX structure to use with decryption
 *
 * Returns WOLFSSL_FAILURE on failure and WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_decrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_PKEY_decrypt_init");
    switch (ctx->pkey->type) {
    case EVP_PKEY_RSA:
        ctx->op = EVP_PKEY_OP_DECRYPT;
        return WOLFSSL_SUCCESS;
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Use a WOLFSSL_EVP_PKEY_CTX structure to encrypt data
 *
 * ctx    WOLFSSL_EVP_PKEY_CTX structure to use with encryption
 * out    buffer to hold encrypted data
 * outlen length of out buffer
 * in     data to be encrypted
 * inlen  length of in buffer
 *
 * Returns WOLFSSL_FAILURE on failure and WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_encrypt(WOLFSSL_EVP_PKEY_CTX *ctx,
                     unsigned char *out, size_t *outlen,
                     const unsigned char *in, size_t inlen)
{
    int len;
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_PKEY_encrypt");
    if (ctx->op != EVP_PKEY_OP_ENCRYPT) return WOLFSSL_FAILURE;

    (void)out;
    (void)outlen;
    (void)in;
    (void)inlen;
    (void)len;
    switch (ctx->pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        len = wolfSSL_RSA_public_encrypt((int)inlen, (unsigned char *)in, out,
                  ctx->pkey->rsa, ctx->padding);
        if (len < 0)
            break;
        else {
            *outlen = len;
            return WOLFSSL_SUCCESS;
        }
#endif /* NO_RSA */

    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Initialize a WOLFSSL_EVP_PKEY_CTX structure to encrypt data
 *
 * ctx    WOLFSSL_EVP_PKEY_CTX structure to use with encryption
 *
 * Returns WOLFSSL_FAILURE on failure and WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_encrypt_init(WOLFSSL_EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_PKEY_encrypt_init");

    switch (ctx->pkey->type) {
    case EVP_PKEY_RSA:
        ctx->op = EVP_PKEY_OP_ENCRYPT;
        return WOLFSSL_SUCCESS;
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Get the size in bits for WOLFSSL_EVP_PKEY key
 *
 * pkey WOLFSSL_EVP_PKEY structure to get key size of
 *
 * returns the size in bits of key on success
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_bits(const WOLFSSL_EVP_PKEY *pkey)
{
    int bytes;

    if (pkey == NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_bits");
    if ((bytes = wolfSSL_EVP_PKEY_size((WOLFSSL_EVP_PKEY*)pkey)) ==0) return 0;
    return bytes*8;
}


/* Get the size in bytes for WOLFSSL_EVP_PKEY key
 *
 * pkey WOLFSSL_EVP_PKEY structure to get key size of
 *
 * returns the size of a key on success which is the maximum size of a
 *         signature
 */
WOLFSSL_API int wolfSSL_EVP_PKEY_size(WOLFSSL_EVP_PKEY *pkey)
{
    if (pkey == NULL) return 0;
    WOLFSSL_ENTER("EVP_PKEY_size");

    switch (pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA:
        return (int)wolfSSL_RSA_size((const WOLFSSL_RSA*)(pkey->rsa));
#endif /* NO_RSA */

#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        if (pkey->ecc == NULL || pkey->ecc->internal == NULL) {
            WOLFSSL_MSG("No ECC key has been set");
            break;
        }
        return wc_ecc_size((ecc_key*)(pkey->ecc->internal));
#endif /* HAVE_ECC */

    default:
        break;
    }
    return 0;
}


/* Initialize structure for signing
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to initialize
 * type is the type of message digest to use
 *
 * returns WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_SignInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_SignInit");
    return wolfSSL_EVP_DigestInit(ctx,type);
}


/* Update structure with data for signing
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to update
 * data buffer holding data to update with for sign
 * len  length of data buffer
 *
 * returns WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_SignUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len)
{
    if (ctx == NULL) return 0;
    WOLFSSL_ENTER("EVP_SignUpdate(");
    return wolfSSL_EVP_DigestUpdate(ctx, data, len);
}

/* macro gaurd because currently only used with RSA */
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
/* Helper function for getting the NID value from md
 *
 * returns the NID value associated with md on success */
static int md2nid(int md)
{
    const char * d;
    d = (const char *)wolfSSL_EVP_get_md((const unsigned char)md);
    if (XSTRNCMP(d, "SHA", 3) == 0) {
        if (XSTRLEN(d) > 3) {
            if (XSTRNCMP(d, "SHA256", 6) == 0) {
                return NID_sha256;
            }
            if (XSTRNCMP(d, "SHA384", 6) == 0) {
                return NID_sha384;
            }
            if (XSTRNCMP(d, "SHA512", 6) == 0) {
                return NID_sha512;
            }
            WOLFSSL_MSG("Unknown SHA type");
            return 0;
        }
        else {
            return NID_sha1;
        }
    }
    if (XSTRNCMP(d, "MD5", 3) == 0)
        return NID_md5;
    return 0;
}
#endif /* NO_RSA */

/* Finalize structure for signing
 *
 * ctx    WOLFSSL_EVP_MD_CTX structure to finalize
 * sigret buffer to hold resulting signature
 * siglen length of sigret buffer
 * pkey   key to sign with
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
WOLFSSL_API int wolfSSL_EVP_SignFinal(WOLFSSL_EVP_MD_CTX *ctx, unsigned char *sigret,
                  unsigned int *siglen, WOLFSSL_EVP_PKEY *pkey)
{
    unsigned int mdsize;
    unsigned char md[WC_MAX_DIGEST_SIZE];
    int ret;
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_SignFinal");

    ret = wolfSSL_EVP_DigestFinal(ctx, md, &mdsize);
    if (ret <= 0) return ret;

    (void)sigret;
    (void)siglen;

    switch (pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA: {
        int nid = md2nid(ctx->macType);
        if (nid < 0) break;
        return wolfSSL_RSA_sign(nid, md, mdsize, sigret,
                                siglen, pkey->rsa);
    }
#endif /* NO_RSA */

    case EVP_PKEY_DSA:
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}


/* Initialize structure for verifying signature
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to initialize
 * type is the type of message digest to use
 *
 * returns WOLFSSL_SUCCESS on success
 */
WOLFSSL_API int wolfSSL_EVP_VerifyInit(WOLFSSL_EVP_MD_CTX *ctx, const WOLFSSL_EVP_MD *type)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyInit");
    return wolfSSL_EVP_DigestInit(ctx,type);
}


/* Update structure for verifying signature
 *
 * ctx  WOLFSSL_EVP_MD_CTX structure to update
 * data buffer holding data to update with for verify
 * len  length of data buffer
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
WOLFSSL_API int wolfSSL_EVP_VerifyUpdate(WOLFSSL_EVP_MD_CTX *ctx, const void *data, size_t len)
{
    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyUpdate");
    return wolfSSL_EVP_DigestUpdate(ctx, data, len);
}


/* Finalize structure for verifying signature
 *
 * ctx    WOLFSSL_EVP_MD_CTX structure to finalize
 * sig    buffer holding signature
 * siglen length of sig buffer
 * pkey   key to verify with
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
WOLFSSL_API int wolfSSL_EVP_VerifyFinal(WOLFSSL_EVP_MD_CTX *ctx,
        unsigned char*sig, unsigned int siglen, WOLFSSL_EVP_PKEY *pkey)
{
    int ret;
    unsigned char md[WC_MAX_DIGEST_SIZE];
    unsigned int mdsize;

    if (ctx == NULL) return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("EVP_VerifyFinal");
    ret = wolfSSL_EVP_DigestFinal(ctx, md, &mdsize);
    if (ret <= 0) return ret;

    (void)sig;
    (void)siglen;

    switch (pkey->type) {
#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    case EVP_PKEY_RSA: {
        int nid = md2nid(ctx->macType);
        if (nid < 0) break;
        return wolfSSL_RSA_verify(nid, md, mdsize, sig,
                (unsigned int)siglen, pkey->rsa);
    }
#endif /* NO_RSA */

    case EVP_PKEY_DSA:
    case EVP_PKEY_EC:
        WOLFSSL_MSG("not implemented");
        FALL_THROUGH;
    default:
        break;
    }
    return WOLFSSL_FAILURE;
}

WOLFSSL_API int wolfSSL_EVP_add_cipher(const WOLFSSL_EVP_CIPHER *cipher)
{
    (void)cipher;
    /* nothing to do */
    return 0;
}


WOLFSSL_EVP_PKEY* wolfSSL_EVP_PKEY_new_mac_key(int type, ENGINE* e,
                                          const unsigned char* key, int keylen)
{
    WOLFSSL_EVP_PKEY* pkey;

    (void)e;

    if (type != EVP_PKEY_HMAC || (key == NULL && keylen != 0))
        return NULL;

    pkey = wolfSSL_PKEY_new();
    if (pkey != NULL) {
        pkey->pkey.ptr = (char*)XMALLOC(keylen, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        if (pkey->pkey.ptr == NULL && keylen > 0) {
            wolfSSL_EVP_PKEY_free(pkey);
            pkey = NULL;
        }
        else {
            XMEMCPY(pkey->pkey.ptr, key, keylen);
            pkey->pkey_sz = keylen;
            pkey->type = pkey->save_type = type;
        }
    }

    return pkey;
}


const unsigned char* wolfSSL_EVP_PKEY_get0_hmac(const WOLFSSL_EVP_PKEY* pkey,
                                                size_t* len)
{
    if (pkey == NULL || len == NULL)
        return NULL;

    *len = (size_t)pkey->pkey_sz;

    return (const unsigned char*)pkey->pkey.ptr;
}


int wolfSSL_EVP_DigestSignInit(WOLFSSL_EVP_MD_CTX *ctx,
                               WOLFSSL_EVP_PKEY_CTX **pctx,
                               const WOLFSSL_EVP_MD *type,
                               WOLFSSL_ENGINE *e,
                               WOLFSSL_EVP_PKEY *pkey)
{
    int hashType;
    const unsigned char* key;
    size_t keySz;

    /* Unused parameters */
    (void)pctx;
    (void)e;

    WOLFSSL_ENTER("EVP_DigestSignInit");

    if (ctx == NULL || type == NULL || pkey == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_ASYNC_CRYPT
    /* compile-time validation of ASYNC_CTX_SIZE */
    typedef char async_test[WC_ASYNC_DEV_SIZE >= sizeof(WC_ASYNC_DEV) ?
                                                                    1 : -1];
    (void)sizeof(async_test);
#endif

    if (XSTRNCMP(type, "SHA256", 6) == 0) {
         hashType = WC_SHA256;
    }
#ifdef WOLFSSL_SHA224
    else if (XSTRNCMP(type, "SHA224", 6) == 0) {
         hashType = WC_SHA224;
    }
#endif
#ifdef WOLFSSL_SHA384
    else if (XSTRNCMP(type, "SHA384", 6) == 0) {
         hashType = WC_SHA384;
    }
#endif
#ifdef WOLFSSL_SHA512
    else if (XSTRNCMP(type, "SHA512", 6) == 0) {
         hashType = WC_SHA512;
    }
#endif
#ifndef NO_MD5
    else if (XSTRNCMP(type, "MD5", 3) == 0) {
        hashType = WC_MD5;
    }
#endif
#ifndef NO_SHA
    /* has to be last since would pick or 224, 256, 384, or 512 too */
    else if (XSTRNCMP(type, "SHA", 3) == 0) {
         hashType = WC_SHA;
    }
#endif /* NO_SHA */
    else
         return BAD_FUNC_ARG;

    key = wolfSSL_EVP_PKEY_get0_hmac(pkey, &keySz);

    if (wc_HmacInit(&ctx->hash.hmac, NULL, INVALID_DEVID) != 0)
        return WOLFSSL_FAILURE;

    if (wc_HmacSetKey(&ctx->hash.hmac, hashType, key, (word32)keySz) != 0)
        return WOLFSSL_FAILURE;

    ctx->macType = NID_hmac & 0xFF;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_EVP_DigestSignUpdate(WOLFSSL_EVP_MD_CTX *ctx,
                                 const void *d, unsigned int cnt)
{
    WOLFSSL_ENTER("EVP_DigestSignFinal");

    if (ctx->macType != (NID_hmac & 0xFF))
        return WOLFSSL_FAILURE;

    if (wc_HmacUpdate(&ctx->hash.hmac, (const byte *)d, cnt) != 0)
        return WOLFSSL_FAILURE;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_EVP_DigestSignFinal(WOLFSSL_EVP_MD_CTX *ctx,
                                unsigned char *sig, size_t *siglen)
{
    unsigned char digest[WC_MAX_DIGEST_SIZE];
    Hmac hmacCopy;
    int hashLen, ret;

    WOLFSSL_ENTER("EVP_DigestSignFinal");

    if (ctx == NULL || siglen == NULL)
        return WOLFSSL_FAILURE;

    if (ctx->macType != (NID_hmac & 0xFF))
        return WOLFSSL_FAILURE;

    switch (ctx->hash.hmac.macType) {
    #ifndef NO_MD5
        case WC_MD5:
            hashLen = WC_MD5_DIGEST_SIZE;
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case WC_SHA:
            hashLen = WC_SHA_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            hashLen = WC_SHA224_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case WC_SHA256:
            hashLen = WC_SHA256_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            hashLen = WC_SHA384_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA384 */
        case WC_SHA512:
            hashLen = WC_SHA512_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            hashLen = BLAKE2B_OUTBYTES;
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            return 0;
    }

    if (sig == NULL) {
        *siglen = hashLen;
        return WOLFSSL_SUCCESS;
    }

    if ((int)(*siglen) > hashLen)
        *siglen = hashLen;

    XMEMCPY(&hmacCopy, &ctx->hash.hmac, sizeof(hmacCopy));
    ret = wc_HmacFinal(&hmacCopy, digest) == 0;
    if (ret == 1)
        XMEMCPY(sig, digest, *siglen);

    ForceZero(&hmacCopy, sizeof(hmacCopy));
    ForceZero(digest, sizeof(digest));
    return ret;
}
#endif /* WOLFSSL_EVP_INCLUDED */

#if defined(OPENSSL_EXTRA) && !defined(NO_PWDBASED) && !defined(NO_SHA)
WOLFSSL_API int wolfSSL_PKCS5_PBKDF2_HMAC_SHA1(const char *pass, int passlen,
                                               const unsigned char *salt,
                                               int saltlen, int iter,
                                               int keylen, unsigned char *out)
{
    const char *nostring = "";
    int ret = 0;

    if (pass == NULL) {
        passlen = 0;
        pass = nostring;
    } else if (passlen == -1) {
        passlen = (int)XSTRLEN(pass);
    }

    ret = wc_PBKDF2((byte*)out, (byte*)pass, passlen, (byte*)salt, saltlen,
                    iter, keylen, WC_SHA);
    if (ret == 0)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FAILURE;
}
#endif /* OPENSSL_EXTRA && !NO_PWDBASED !NO_SHA*/
