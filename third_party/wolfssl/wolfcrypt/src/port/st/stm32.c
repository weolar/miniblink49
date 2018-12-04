/* stm32.c
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

/* Generic STM32 Hashing Function */
/* Supports CubeMX HAL or Standard Peripheral Library */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#include <wolfssl/wolfcrypt/port/st/stm32.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef STM32_HASH

/* User can override STM32_HASH_CLOCK_ENABLE and STM32_HASH_CLOCK_DISABLE */
#ifndef STM32_HASH_CLOCK_ENABLE
    static INLINE void wc_Stm32_Hash_Clock_Enable(STM32_HASH_Context* stmCtx)
    {
    #ifdef WOLFSSL_STM32_CUBEMX
        __HAL_RCC_HASH_CLK_ENABLE();
    #else
        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);
    #endif
        (void)stmCtx;
    }
    #define STM32_HASH_CLOCK_ENABLE(ctx) wc_Stm32_Hash_Clock_Enable(ctx)
#endif

#ifndef STM32_HASH_CLOCK_DISABLE
    static INLINE void wc_Stm32_Hash_Clock_Disable(STM32_HASH_Context* stmCtx)
    {
    #ifdef WOLFSSL_STM32_CUBEMX
        __HAL_RCC_HASH_CLK_DISABLE();
    #else
        RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, DISABLE);
    #endif
        (void)stmCtx;
    }
    #define STM32_HASH_CLOCK_DISABLE(ctx) wc_Stm32_Hash_Clock_Disable(ctx)
#endif

/* STM32 Port Internal Functions */
static INLINE void wc_Stm32_Hash_SaveContext(STM32_HASH_Context* ctx)
{
    int i;

    /* save context registers */
    ctx->HASH_IMR = HASH->IMR;
    ctx->HASH_STR = HASH->STR;
    ctx->HASH_CR  = HASH->CR;
    for (i=0; i<HASH_CR_SIZE; i++) {
        ctx->HASH_CSR[i] = HASH->CSR[i];
    }
}

static INLINE int wc_Stm32_Hash_RestoreContext(STM32_HASH_Context* ctx)
{
    int i;

    if (ctx->HASH_CR != 0) {
        /* restore context registers */
        HASH->IMR = ctx->HASH_IMR;
        HASH->STR = ctx->HASH_STR;
        HASH->CR = ctx->HASH_CR;

        /* Initialize the hash processor */
        HASH->CR |= HASH_CR_INIT;

        /* continue restoring context registers */
        for (i=0; i<HASH_CR_SIZE; i++) {
            HASH->CSR[i] = ctx->HASH_CSR[i];
        }
        return 1;
    }
    return 0;
}

static INLINE void wc_Stm32_Hash_GetDigest(byte* hash, int digestSize)
{
    word32 digest[HASH_MAX_DIGEST/sizeof(word32)];

    /* get digest result */
    digest[0] = HASH->HR[0];
    digest[1] = HASH->HR[1];
    digest[2] = HASH->HR[2];
    digest[3] = HASH->HR[3];
    if (digestSize >= 20) {
        digest[4] = HASH->HR[4];
    #ifdef HASH_DIGEST
        if (digestSize >= 28) {
            digest[5] = HASH_DIGEST->HR[5];
            digest[6] = HASH_DIGEST->HR[6];
            if (digestSize == 32)
                digest[7] = HASH_DIGEST->HR[7];
        }
    #endif
    }

    ByteReverseWords(digest, digest, digestSize);

    XMEMCPY(hash, digest, digestSize);
}


/* STM32 Port Exposed Functions */
static INLINE int wc_Stm32_Hash_WaitDone(void)
{
    /* wait until hash hardware is not busy */
    int timeout = 0;
    while ((HASH->SR & HASH_SR_BUSY) && ++timeout < STM32_HASH_TIMEOUT) {

    }
    /* verify timeout did not occur */
    if (timeout >= STM32_HASH_TIMEOUT) {
        return WC_TIMEOUT_E;
    }
    return 0;
}


void wc_Stm32_Hash_Init(STM32_HASH_Context* stmCtx)
{
    /* clear context */
    XMEMSET(stmCtx, 0, sizeof(STM32_HASH_Context));
}

int wc_Stm32_Hash_Update(STM32_HASH_Context* stmCtx, word32 algo,
    const byte* data, int len)
{
    int ret = 0;
    byte* local = (byte*)stmCtx->buffer;
    int wroteToFifo = 0;

    /* check that internal buffLen is valid */
    if (stmCtx->buffLen >= STM32_HASH_REG_SIZE) {
        return BUFFER_E;
    }

    /* turn on hash clock */
    STM32_HASH_CLOCK_ENABLE(stmCtx);

    /* restore hash context or init as new hash */
    if (wc_Stm32_Hash_RestoreContext(stmCtx) == 0) {
        /* reset the control register */
        HASH->CR &= ~(HASH_CR_ALGO | HASH_CR_DATATYPE | HASH_CR_MODE);

        /* configure algorithm, mode and data type */
        HASH->CR |= (algo | HASH_ALGOMODE_HASH | HASH_DATATYPE_8B);

        /* reset HASH processor */
        HASH->CR |= HASH_CR_INIT;
    }

    /* write 4-bytes at a time into FIFO */
    while (len) {
        word32 add = min(len, STM32_HASH_REG_SIZE - stmCtx->buffLen);
        XMEMCPY(&local[stmCtx->buffLen], data, add);

        stmCtx->buffLen += add;
        data            += add;
        len             -= add;

        if (stmCtx->buffLen == STM32_HASH_REG_SIZE) {
            wroteToFifo = 1;
            HASH->DIN = *(word32*)stmCtx->buffer;

            stmCtx->loLen += STM32_HASH_REG_SIZE;
            stmCtx->buffLen = 0;
        }
    }

    if (wroteToFifo) {
        /* save hash state for next operation */
        wc_Stm32_Hash_SaveContext(stmCtx);
    }

    /* turn off hash clock */
    STM32_HASH_CLOCK_DISABLE(stmCtx);

    return ret;
}

int wc_Stm32_Hash_Final(STM32_HASH_Context* stmCtx, word32 algo,
    byte* hash, int digestSize)
{
    int ret = 0;
    word32 nbvalidbitsdata = 0;

    /* turn on hash clock */
    STM32_HASH_CLOCK_ENABLE(stmCtx);

    /* restore hash state */
    wc_Stm32_Hash_RestoreContext(stmCtx);

    /* finish reading any trailing bytes into FIFO */
    if (stmCtx->buffLen > 0) {
        HASH->DIN = *(word32*)stmCtx->buffer;
        stmCtx->loLen += stmCtx->buffLen;
    }

    /* calculate number of valid bits in last word */
    nbvalidbitsdata = 8 * (stmCtx->loLen % STM32_HASH_REG_SIZE);
    HASH->STR &= ~HASH_STR_NBW;
    HASH->STR |= nbvalidbitsdata;

    /* start hash processor */
    HASH->STR |= HASH_STR_DCAL;

    /* wait for hash done */
    ret = wc_Stm32_Hash_WaitDone();
    if (ret == 0) {
        /* read message digest */
        wc_Stm32_Hash_GetDigest(hash, digestSize);
    }

    /* turn off hash clock */
    STM32_HASH_CLOCK_DISABLE(stmCtx);

    return ret;
}

#endif /* STM32_HASH */
