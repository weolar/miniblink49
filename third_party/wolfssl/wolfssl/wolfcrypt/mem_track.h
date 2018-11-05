/* mem_track.h
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


/* The memory tracker overrides the wolfSSL memory callback system and uses a
 * static to track the total, peak and currently allocated bytes.
 *
 * If you are already using the memory callbacks then enabling this will
 * override the memory callbacks and prevent your memory callbacks from
 * working. This assumes malloc() and free() are available. Feel free to
 * customize this for your needs.

 * The enable this feature define the following:
 * #define USE_WOLFSSL_MEMORY
 * #define WOLFSSL_TRACK_MEMORY
 *
 * On startup call:
 * InitMemoryTracker();
 *
 * When ready to dump the memory report call:
 * ShowMemoryTracker();
 *
 * Report example:
 * total   Allocs =       228
 * total   Bytes  =     93442
 * peak    Bytes  =      8840
 * current Bytes  =         0
 *
 *
 * You can also:
 * #define WOLFSSL_DEBUG_MEMORY
 *
 * To print every alloc/free along with the function and line number.
 * Example output:
 * Alloc: 0x7fa14a500010 -> 120 at wc_InitRng:496
 * Free: 0x7fa14a500010 -> 120 at wc_FreeRng:606
 */


#ifndef WOLFSSL_MEM_TRACK_H
#define WOLFSSL_MEM_TRACK_H

#if defined(USE_WOLFSSL_MEMORY) && !defined(WOLFSSL_STATIC_MEMORY)

    #include "wolfssl/wolfcrypt/logging.h"

    typedef struct memoryStats {
        size_t totalAllocs;     /* number of allocations */
        size_t totalDeallocs;   /* number of deallocations */
        size_t totalBytes;      /* total number of bytes allocated */
        size_t peakBytes;       /* concurrent max bytes */
        size_t currentBytes;    /* total current bytes in use */
    } memoryStats;

    typedef struct memHint {
        size_t thisSize;      /* size of this memory */
        void*  thisMemory;    /* actual memory for user */
    } memHint;

    typedef struct memoryTrack {
        union {
            memHint hint;
            byte    alignit[16];   /* make sure we have strong alignment */
        } u;
    } memoryTrack;

    #if defined(WOLFSSL_TRACK_MEMORY)
        #define DO_MEM_STATS
        static memoryStats ourMemStats;
    #endif

    /* if defined to not using inline then declare function prototypes */
    #ifdef NO_INLINE
        #define STATIC
		#ifdef WOLFSSL_DEBUG_MEMORY
			WOLFSSL_LOCAL void* TrackMalloc(size_t sz, const char* func, unsigned int line);
			WOLFSSL_LOCAL void TrackFree(void* ptr, const char* func, unsigned int line);
			WOLFSSL_LOCAL void* TrackRealloc(void* ptr, size_t sz, const char* func, unsigned int line);
		#else
			WOLFSSL_LOCAL void* TrackMalloc(size_t sz);
			WOLFSSL_LOCAL void TrackFree(void* ptr);
			WOLFSSL_LOCAL void* TrackRealloc(void* ptr, size_t sz);
		#endif
        WOLFSSL_LOCAL int InitMemoryTracker(void);
        WOLFSSL_LOCAL void ShowMemoryTracker(void);
    #else
        #define STATIC static
    #endif

#ifdef WOLFSSL_DEBUG_MEMORY
    STATIC INLINE void* TrackMalloc(size_t sz, const char* func, unsigned int line)
#else
    STATIC INLINE void* TrackMalloc(size_t sz)
#endif
    {
        memoryTrack* mt;

        if (sz == 0)
            return NULL;

        mt = (memoryTrack*)malloc(sizeof(memoryTrack) + sz);
        if (mt == NULL)
            return NULL;

        mt->u.hint.thisSize   = sz;
        mt->u.hint.thisMemory = (byte*)mt + sizeof(memoryTrack);

#ifdef WOLFSSL_DEBUG_MEMORY
        printf("Alloc: %p -> %u at %s:%d\n", mt->u.hint.thisMemory, (word32)sz, func, line);
#endif

#ifdef DO_MEM_STATS
        ourMemStats.totalAllocs++;
        ourMemStats.totalBytes   += sz;
        ourMemStats.currentBytes += sz;
        if (ourMemStats.currentBytes > ourMemStats.peakBytes)
            ourMemStats.peakBytes = ourMemStats.currentBytes;
#endif

        return mt->u.hint.thisMemory;
    }


#ifdef WOLFSSL_DEBUG_MEMORY
    STATIC INLINE void TrackFree(void* ptr, const char* func, unsigned int line)
#else
    STATIC INLINE void TrackFree(void* ptr)
#endif
    {
        memoryTrack* mt;

        if (ptr == NULL) {
            return;
        }

        mt = (memoryTrack*)ptr;
        --mt;   /* same as minus sizeof(memoryTrack), removes header */

#ifdef DO_MEM_STATS
        ourMemStats.currentBytes -= mt->u.hint.thisSize;
        ourMemStats.totalDeallocs++;
#endif

#ifdef WOLFSSL_DEBUG_MEMORY
        printf("Free: %p -> %u at %s:%d\n", ptr, (word32)mt->u.hint.thisSize, func, line);
#endif

        free(mt);
    }


#ifdef WOLFSSL_DEBUG_MEMORY
    STATIC INLINE void* TrackRealloc(void* ptr, size_t sz, const char* func, unsigned int line)
#else
    STATIC INLINE void* TrackRealloc(void* ptr, size_t sz)
#endif
    {
    #ifdef WOLFSSL_DEBUG_MEMORY
        void* ret = TrackMalloc(sz, func, line);
    #else
        void* ret = TrackMalloc(sz);
    #endif

        if (ptr) {
            /* if realloc is bigger, don't overread old ptr */
            memoryTrack* mt = (memoryTrack*)ptr;
            --mt;  /* same as minus sizeof(memoryTrack), removes header */

            if (mt->u.hint.thisSize < sz)
                sz = mt->u.hint.thisSize;
        }

        if (ret && ptr)
            XMEMCPY(ret, ptr, sz);

        if (ret) {
        #ifdef WOLFSSL_DEBUG_MEMORY
            TrackFree(ptr, func, line);
        #else
            TrackFree(ptr);
        #endif
        }

        return ret;
    }

#ifdef WOLFSSL_TRACK_MEMORY
    STATIC INLINE int InitMemoryTracker(void)
    {
        int ret = wolfSSL_SetAllocators(TrackMalloc, TrackFree, TrackRealloc);
        if (ret < 0) {
            printf("wolfSSL SetAllocators failed for track memory\n");
            return ret;
        }

    #ifdef DO_MEM_STATS
        ourMemStats.totalAllocs  = 0;
        ourMemStats.totalDeallocs = 0;
        ourMemStats.totalBytes   = 0;
        ourMemStats.peakBytes    = 0;
        ourMemStats.currentBytes = 0;
    #endif

        return ret;
    }

    STATIC INLINE void ShowMemoryTracker(void)
    {
    #ifdef DO_MEM_STATS
        printf("total   Allocs   = %9lu\n",
                                       (unsigned long)ourMemStats.totalAllocs);
        printf("total   Deallocs = %9lu\n",
                                       (unsigned long)ourMemStats.totalDeallocs);
        printf("total   Bytes    = %9lu\n",
                                       (unsigned long)ourMemStats.totalBytes);
        printf("peak    Bytes    = %9lu\n",
                                       (unsigned long)ourMemStats.peakBytes);
        printf("current Bytes    = %9lu\n",
                                       (unsigned long)ourMemStats.currentBytes);
    #endif
    }
#endif

#endif /* USE_WOLFSSL_MEMORY */

#endif /* WOLFSSL_MEM_TRACK_H */

