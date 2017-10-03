/* logging.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

/* submitted by eof */

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>


#ifdef __cplusplus
    extern "C" {
#endif
    WOLFSSL_API int  wolfSSL_Debugging_ON(void);
    WOLFSSL_API void wolfSSL_Debugging_OFF(void);
#ifdef __cplusplus
    }
#endif

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
static wolfSSL_Mutex debug_mutex; /* mutex for access to debug structure */

/* accessing any node from the queue should be wrapped in a lock of
 * debug_mutex */
static void* wc_error_heap;
struct wc_error_queue {
    void*  heap; /* the heap hint used with nodes creation */
    struct wc_error_queue* next;
    struct wc_error_queue* prev;
    char   error[WOLFSSL_MAX_ERROR_SZ];
    char   file[WOLFSSL_MAX_ERROR_SZ];
    int    value;
    int    line;
};
volatile struct wc_error_queue* wc_errors;
static struct wc_error_queue* wc_last_node;
/* pointer to last node in queue to make insertion O(1) */
#endif



#if defined(DEBUG_WOLFSSL)

/* Set these to default values initially. */
static wolfSSL_Logging_cb log_function = NULL;
static int loggingEnabled = 0;

#endif /* DEBUG_WOLFSSL */


int wolfSSL_SetLoggingCb(wolfSSL_Logging_cb f)
{
#ifdef DEBUG_WOLFSSL
    int res = 0;

    if (f)
        log_function = f;
    else
        res = BAD_FUNC_ARG;

    return res;
#else
    (void)f;
    return NOT_COMPILED_IN;
#endif
}


int wolfSSL_Debugging_ON(void)
{
#ifdef DEBUG_WOLFSSL
    loggingEnabled = 1;
    return 0;
#else
    return NOT_COMPILED_IN;
#endif
}


void wolfSSL_Debugging_OFF(void)
{
#ifdef DEBUG_WOLFSSL
    loggingEnabled = 0;
#endif
}


#ifdef DEBUG_WOLFSSL

#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #if MQX_USE_IO_OLD
        #include <fio.h>
    #else
        #include <nio.h>
    #endif
#elif defined(WOLFSSL_SGX)
    /* Declare sprintf for ocall */
    int sprintf(char* buf, const char *fmt, ...);
#elif defined(MICRIUM)
    #include <bsp_ser.h>
#else
    #include <stdio.h>   /* for default printf stuff */
#endif

#if defined(THREADX) && !defined(THREADX_NO_DC_PRINTF)
    int dc_log_printf(char*, ...);
#endif

static void wolfssl_log(const int logLevel, const char *const logMessage)
{
    if (log_function)
        log_function(logLevel, logMessage);
    else {
        if (loggingEnabled) {
#if defined(THREADX) && !defined(THREADX_NO_DC_PRINTF)
            dc_log_printf("%s\n", logMessage);
#elif defined(MICRIUM)
            BSP_Ser_Printf("%s\r\n", logMessage);
#elif defined(WOLFSSL_MDK_ARM)
            fflush(stdout) ;
            printf("%s\n", logMessage);
            fflush(stdout) ;
#elif defined(WOLFSSL_LOG_PRINTF)
            printf("%s\n", logMessage);
#elif defined(WOLFSSL_UTASKER)
            fnDebugMsg((char*)logMessage);
            fnDebugMsg("\r\n");
#elif defined(MQX_USE_IO_OLD)
            fprintf(_mqxio_stderr, "%s\n", logMessage);
#else
            fprintf(stderr, "%s\n", logMessage);
#endif
        }
    }
}


void WOLFSSL_MSG(const char* msg)
{
    if (loggingEnabled)
        wolfssl_log(INFO_LOG , msg);
}


void WOLFSSL_BUFFER(const byte* buffer, word32 length)
{
    #define LINE_LEN 16

    if (loggingEnabled) {
        word32 i;
        char line[80];

        if (!buffer) {
            wolfssl_log(INFO_LOG, "\tNULL");

            return;
        }

        sprintf(line, "\t");

        for (i = 0; i < LINE_LEN; i++) {
            if (i < length)
                sprintf(line + 1 + i * 3,"%02x ", buffer[i]);
            else
                sprintf(line + 1 + i * 3, "   ");
        }

        sprintf(line + 1 + LINE_LEN * 3, "| ");

        for (i = 0; i < LINE_LEN; i++)
            if (i < length)
                sprintf(line + 3 + LINE_LEN * 3 + i,
                     "%c", 31 < buffer[i] && buffer[i] < 127 ? buffer[i] : '.');

        wolfssl_log(INFO_LOG, line);

        if (length > LINE_LEN)
            WOLFSSL_BUFFER(buffer + LINE_LEN, length - LINE_LEN);
    }
}


void WOLFSSL_ENTER(const char* msg)
{
    if (loggingEnabled) {
        char buffer[80];
        sprintf(buffer, "wolfSSL Entering %s", msg);
        wolfssl_log(ENTER_LOG , buffer);
    }
}


void WOLFSSL_LEAVE(const char* msg, int ret)
{
    if (loggingEnabled) {
        char buffer[80];
        sprintf(buffer, "wolfSSL Leaving %s, return %d", msg, ret);
        wolfssl_log(LEAVE_LOG , buffer);
    }
}
#endif  /* DEBUG_WOLFSSL */

/*
 * When using OPENSSL_EXTRA or DEBUG_WOLFSSL_VERBOSE macro then WOLFSSL_ERROR is
 * mapped to new funtion WOLFSSL_ERROR_LINE which gets the line # and function
 * name where WOLFSSL_ERROR is called at.
 */
#if (defined(DEBUG_WOLFSSL) || defined(WOLFSSL_NGINX)) || defined(WOLFSSL_HAPROXY)
    #if (defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE))
void WOLFSSL_ERROR_LINE(int error, const char* func, unsigned int line,
            const char* file, void* usrCtx)
    #else
void WOLFSSL_ERROR(int error)
    #endif
{
    #if defined(DEBUG_WOLFSSL) && !defined(WOLFSSL_NGINX)
    if (loggingEnabled && error != WC_PENDING_E)
    #endif
    {
        char buffer[80];
        #if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
            (void)usrCtx; /* a user ctx for future flexibility */
            (void)func;

            if (wc_LockMutex(&debug_mutex) != 0) {
                WOLFSSL_MSG("Lock debug mutex failed");
                sprintf(buffer, "wolfSSL error occurred, error = %d", error);
            }
            else {
                if (error < 0) error = error - (2*error); /*get absolute value*/
                sprintf(buffer, "wolfSSL error occurred, error = %d line:%d file:%s",
                    error, line, file);
                if (wc_AddErrorNode(error, line, buffer, (char*)file) != 0) {
                    WOLFSSL_MSG("Error creating logging node");
                    /* with void function there is no return here, continue on
                     * to unlock mutex and log what buffer was created. */
                }

                wc_UnLockMutex(&debug_mutex);
            }
        #else
            sprintf(buffer, "wolfSSL error occurred, error = %d", error);
        #endif
        #ifdef DEBUG_WOLFSSL
        wolfssl_log(ERROR_LOG , buffer);
        #endif
    }
}

#endif  /* DEBUG_WOLFSSL || WOLFSSL_NGINX || WOLFSSL_HAPROXY */

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
/* Internal function that is called by wolfCrypt_Init() */
int wc_LoggingInit(void)
{
    if (wc_InitMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Bad Init Mutex");
        return BAD_MUTEX_E;
    }
    wc_errors          = NULL;
    wc_last_node       = NULL;

    return 0;
}


/* internal function that is called by wolfCrypt_Cleanup */
int wc_LoggingCleanup(void)
{
    /* clear logging entries */
    wc_ClearErrorNodes();

    /* free mutex */
    if (wc_FreeMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Bad Mutex free");
        return BAD_MUTEX_E;
    }
    return 0;
}


#if defined(DEBUG_WOLFSSL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_MYSQL_COMPATIBLE)
/* peek at an error node
 *
 * idx : if -1 then the most recent node is looked at, otherwise search
 *         through queue for node at the given index
 * file  : pointer to internal file string
 * reason : pointer to internal error reason
 * line  : line number that error happened at
 *
 * Returns a negative value in error case, on success returns the nodes error
 * value which is positve (absolute value)
 */
int wc_PeekErrorNode(int idx, const char **file, const char **reason,
        int *line)
{
    struct wc_error_queue* err;

    if (wc_LockMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }

    if (idx < 0) {
        err = wc_last_node;
        if (err == NULL) {
            WOLFSSL_MSG("No Errors in queue");
            wc_UnLockMutex(&debug_mutex);
            return BAD_STATE_E;
        }
    }
    else {
        int i;

        err = (struct wc_error_queue*)wc_errors;
        for (i = 0; i < idx; i++) {
            if (err == NULL) {
                WOLFSSL_MSG("Error node not found. Bad index?");
                wc_UnLockMutex(&debug_mutex);
                return BAD_FUNC_ARG;
            }
            err = err->next;
        }
    }

    if (file != NULL) {
        *file = err->file;
    }

    if (reason != NULL) {
        *reason = err->error;
    }

    if (line != NULL) {
        *line = err->line;
    }

    wc_UnLockMutex(&debug_mutex);

    return err->value;
}


/* create new error node and add it to the queue
 * buffers are assumed to be of size WOLFSSL_MAX_ERROR_SZ for this internal
 * function. debug_mutex should be locked before a call to this function. */
int wc_AddErrorNode(int error, int line, char* buf, char* file)
{

    struct wc_error_queue* err;

    err = (struct wc_error_queue*)XMALLOC(
            sizeof(struct wc_error_queue), wc_error_heap, DYNAMIC_TYPE_LOG);
    if (err == NULL) {
        WOLFSSL_MSG("Unable to create error node for log");
        return MEMORY_E;
    }
    else {
        int sz;

        XMEMSET(err, 0, sizeof(struct wc_error_queue));
        err->heap = wc_error_heap;
        sz = (int)XSTRLEN(buf);
        if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
            sz = WOLFSSL_MAX_ERROR_SZ - 1;
        }
        if (sz > 0) {
            XMEMCPY(err->error, buf, sz);
        }

        sz = (int)XSTRLEN(file);
        if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
            sz = WOLFSSL_MAX_ERROR_SZ - 1;
        }
        if (sz > 0) {
            XMEMCPY(err->file, file, sz);
        }

        err->value = error;
        err->line  = line;

        /* make sure is terminated */
        err->error[WOLFSSL_MAX_ERROR_SZ - 1] = '\0';
        err->file[WOLFSSL_MAX_ERROR_SZ - 1]  = '\0';


        /* since is queue place new node at last of the list */
        if (wc_last_node == NULL) {
            /* case of first node added to queue */
            if (wc_errors != NULL) {
                /* check for unexpected case before over writing wc_errors */
                WOLFSSL_MSG("ERROR in adding new node to logging queue!!\n");
            }
            else {
                wc_errors    = err;
                wc_last_node = err;
            }
        }
        else {
            wc_last_node->next = err;
            err->prev = wc_last_node;
            wc_last_node = err;
        }
    }

    return 0;
}

/* Removes the error node at the specified index.
 * idx : if -1 then the most recent node is looked at, otherwise search
 *         through queue for node at the given index
 */
void wc_RemoveErrorNode(int idx)
{
    struct wc_error_queue* current;

    if (wc_LockMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return;
    }

    if (idx == -1)
        current = wc_last_node;
    else {
        current = (struct wc_error_queue*)wc_errors;
        for (; current != NULL && idx > 0; idx--)
             current = current->next;
    }
    if (current != NULL) {
        if (current->prev != NULL)
            current->prev->next = current->next;
        if (wc_last_node == current)
            wc_last_node = current->prev;
        if (wc_errors == current)
            wc_errors = current->next;
        XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
    }

    wc_UnLockMutex(&debug_mutex);
}

#endif /* DEBUG_WOLFSSL || WOLFSSL_NGINX */

/* Clears out the list of error nodes.
 */
void wc_ClearErrorNodes(void)
{
    if (wc_LockMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return;
    }

    /* free all nodes from error queue */
    {
        struct wc_error_queue* current;
        struct wc_error_queue* next;

        current = (struct wc_error_queue*)wc_errors;
        while (current != NULL) {
            next = current->next;
            XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
            current = next;
        }
    }

    wc_errors    = NULL;
    wc_last_node = NULL;
    wc_UnLockMutex(&debug_mutex);
}

int wc_SetLoggingHeap(void* h)
{
    if (wc_LockMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }
    wc_error_heap = h;
    wc_UnLockMutex(&debug_mutex);
    return 0;
}

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
/* empties out the error queue into the file */
void wc_ERR_print_errors_fp(FILE* fp)
{
    WOLFSSL_ENTER("wc_ERR_print_errors_fp");

    if (wc_LockMutex(&debug_mutex) != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
    }
    else {
        /* free all nodes from error queue and print them to file */
        {
            struct wc_error_queue* current;
            struct wc_error_queue* next;

            current = (struct wc_error_queue*)wc_errors;
            while (current != NULL) {
                next = current->next;
                fprintf(fp, "%s\n", current->error);
                XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
                current = next;
            }

            /* set global pointers to match having been freed */
            wc_errors    = NULL;
            wc_last_node = NULL;
        }

        wc_UnLockMutex(&debug_mutex);
    }
}
#endif /* !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) */

#endif /* defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE) */

