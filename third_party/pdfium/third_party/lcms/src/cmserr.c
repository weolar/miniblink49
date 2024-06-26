//---------------------------------------------------------------------------------
//
//  Little Color Management System
//  Copyright (c) 1998-2017 Marti Maria Saguer
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software
// is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//---------------------------------------------------------------------------------

#include "lcms2_internal.h"

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"

// This function is here to help applications to prevent mixing lcms versions on header and shared objects.
int CMSEXPORT cmsGetEncodedCMMversion(void)
{
       return LCMS_VERSION;
}

// I am so tired about incompatibilities on those functions that here are some replacements
// that hopefully would be fully portable.

// compare two strings ignoring case
int CMSEXPORT cmsstrcasecmp(const char* s1, const char* s2)
{
    register const unsigned char *us1 = (const unsigned char *)s1,
                                 *us2 = (const unsigned char *)s2;

    while (toupper(*us1) == toupper(*us2++))
        if (*us1++ == '\0')
            return 0;

    return (toupper(*us1) - toupper(*--us2));
}

// long int because C99 specifies ftell in such way (7.19.9.2)
long int CMSEXPORT cmsfilelength(FILE* f)
{
    long int p , n;

    p = ftell(f); // register current file position
    if (p == -1L) 
        return -1L;

    if (fseek(f, 0, SEEK_END) != 0) {
        return -1L;
    }

    n = ftell(f);
    fseek(f, p, SEEK_SET); // file position restored

    return n;
}

cmsBool  _cmsRegisterMemHandlerPlugin(cmsContext ContextID, cmsPluginBase* Plugin)
{
    return TRUE;
}

// Generic allocate
void* CMSEXPORT _cmsMalloc(cmsContext ContextID, cmsUInt32Number size)
{
    return FXMEM_DefaultAlloc(size);
}

// Generic allocate & zero
void* CMSEXPORT _cmsMallocZero(cmsContext ContextID, cmsUInt32Number size)
{
    void* p = FXMEM_DefaultAlloc(size);
    if (p) memset(p, 0, size);
    return p;
}

// Generic calloc
void* CMSEXPORT _cmsCalloc(cmsContext ContextID, cmsUInt32Number num, cmsUInt32Number size)
{
    cmsUInt32Number total = num * size;
    if (total == 0 || total / size != num || total >= 512 * 1024 * 1024)
        return NULL;

    return _cmsMallocZero(ContextID, num * size);
}

// Generic reallocate
void* CMSEXPORT _cmsRealloc(cmsContext ContextID, void* Ptr, cmsUInt32Number size)
{
    return FXMEM_DefaultRealloc(Ptr, size);
}

// Generic free memory
void CMSEXPORT _cmsFree(cmsContext ContextID, void* Ptr)
{
    if (Ptr != NULL) FXMEM_DefaultFree(Ptr);
}

// Generic block duplication
void* CMSEXPORT _cmsDupMem(cmsContext ContextID, const void* Org, cmsUInt32Number size)
{
    void* p = FXMEM_DefaultAlloc(size);
    memmove(p, Org, size);
    return p;
}

_cmsMemPluginChunkType _cmsMemPluginChunk = {_cmsMalloc, _cmsMallocZero, _cmsFree,
                                             _cmsRealloc, _cmsCalloc,    _cmsDupMem
                                            };

void _cmsAllocMemPluginChunk(struct _cmsContext_struct* ctx, const struct _cmsContext_struct* src)
{
    _cmsAssert(ctx != NULL);

    if (src != NULL) {

        // Duplicate
        ctx ->chunks[MemPlugin] = _cmsSubAllocDup(ctx ->MemPool, src ->chunks[MemPlugin], sizeof(_cmsMemPluginChunkType));
    }
    else {

        // To reset it, we use the default allocators, which cannot be overridden
        ctx ->chunks[MemPlugin] = &ctx ->DefaultMemoryManager;
    }
}

void _cmsInstallAllocFunctions(cmsPluginMemHandler* Plugin, _cmsMemPluginChunkType* ptr)
{
    if (Plugin == NULL) {

        memcpy(ptr, &_cmsMemPluginChunk, sizeof(_cmsMemPluginChunk));
    }
    else {

        ptr ->MallocPtr  = Plugin -> MallocPtr;
        ptr ->FreePtr    = Plugin -> FreePtr;
        ptr ->ReallocPtr = Plugin -> ReallocPtr;

        // Make sure we revert to defaults
        ptr ->MallocZeroPtr= _cmsMallocZero;
        ptr ->CallocPtr    = _cmsCalloc;
        ptr ->DupPtr       = _cmsDupMem;

        if (Plugin ->MallocZeroPtr != NULL) ptr ->MallocZeroPtr = Plugin -> MallocZeroPtr;
        if (Plugin ->CallocPtr != NULL)     ptr ->CallocPtr     = Plugin -> CallocPtr;
        if (Plugin ->DupPtr != NULL)        ptr ->DupPtr        = Plugin -> DupPtr;

    }
}

// ********************************************************************************************

// Sub allocation takes care of many pointers of small size. The memory allocated in
// this way have be freed at once. Next function allocates a single chunk for linked list
// I prefer this method over realloc due to the big inpact on xput realloc may have if
// memory is being swapped to disk. This approach is safer (although that may not be true on all platforms)
static
_cmsSubAllocator_chunk* _cmsCreateSubAllocChunk(cmsContext ContextID, cmsUInt32Number Initial)
{
    _cmsSubAllocator_chunk* chunk;

    // 20K by default
    if (Initial == 0)
        Initial = 20*1024;

    // Create the container
    chunk = (_cmsSubAllocator_chunk*) _cmsMallocZero(ContextID, sizeof(_cmsSubAllocator_chunk));
    if (chunk == NULL) return NULL;

    // Initialize values
    chunk ->Block     = (cmsUInt8Number*) _cmsMalloc(ContextID, Initial);
    if (chunk ->Block == NULL) {

        // Something went wrong
        _cmsFree(ContextID, chunk);
        return NULL;
    }

    chunk ->BlockSize = Initial;
    chunk ->Used      = 0;
    chunk ->next      = NULL;

    return chunk;
}

// The suballocated is nothing but a pointer to the first element in the list. We also keep
// the thread ID in this structure.
_cmsSubAllocator* _cmsCreateSubAlloc(cmsContext ContextID, cmsUInt32Number Initial)
{
    _cmsSubAllocator* sub;

    // Create the container
    sub = (_cmsSubAllocator*) _cmsMallocZero(ContextID, sizeof(_cmsSubAllocator));
    if (sub == NULL) return NULL;

    sub ->ContextID = ContextID;

    sub ->h = _cmsCreateSubAllocChunk(ContextID, Initial);
    if (sub ->h == NULL) {
        _cmsFree(ContextID, sub);
        return NULL;
    }

    return sub;
}


// Get rid of whole linked list
void _cmsSubAllocDestroy(_cmsSubAllocator* sub)
{
    _cmsSubAllocator_chunk *chunk, *n;

    for (chunk = sub ->h; chunk != NULL; chunk = n) {

        n = chunk->next;
        if (chunk->Block != NULL) _cmsFree(sub ->ContextID, chunk->Block);
        _cmsFree(sub ->ContextID, chunk);
    }

    // Free the header
    _cmsFree(sub ->ContextID, sub);
}


// Get a pointer to small memory block.
void*  _cmsSubAlloc(_cmsSubAllocator* sub, cmsUInt32Number size)
{
    cmsUInt32Number Free = sub -> h ->BlockSize - sub -> h -> Used;
    cmsUInt8Number* ptr;

    size = _cmsALIGNMEM(size);

    // Check for memory. If there is no room, allocate a new chunk of double memory size.
    if (size > Free) {

        _cmsSubAllocator_chunk* chunk;
        cmsUInt32Number newSize;

        newSize = sub -> h ->BlockSize * 2;
        if (newSize < size) newSize = size;

        chunk = _cmsCreateSubAllocChunk(sub -> ContextID, newSize);
        if (chunk == NULL) return NULL;

        // Link list
        chunk ->next = sub ->h;
        sub ->h    = chunk;

    }

    ptr =  sub -> h ->Block + sub -> h ->Used;
    sub -> h -> Used += size;

    return (void*) ptr;
}

// Duplicate in pool
void* _cmsSubAllocDup(_cmsSubAllocator* s, const void *ptr, cmsUInt32Number size)
{
    void *NewPtr;
    
    // Dup of null pointer is also NULL
    if (ptr == NULL)
        return NULL;

    NewPtr = _cmsSubAlloc(s, size);

    if (ptr != NULL && NewPtr != NULL) {
        memcpy(NewPtr, ptr, size);
    }

    return NewPtr;
}



// Error logging ******************************************************************

// There is no error handling at all. When a function fails, it returns proper value.
// For example, all create functions does return NULL on failure. Other return FALSE
// It may be interesting, for the developer, to know why the function is failing.
// for that reason, lcms2 does offer a logging function. This function does recive
// a ENGLISH string with some clues on what is going wrong. You can show this
// info to the end user, or just create some sort of log.
// The logging function should NOT terminate the program, as this obviously can leave
// resources. It is the programmer's responsibility to check each function return code
// to make sure it didn't fail.

// Error messages are limited to MAX_ERROR_MESSAGE_LEN

#define MAX_ERROR_MESSAGE_LEN   1024

// ---------------------------------------------------------------------------------------------------------

// This is our default log error
static void DefaultLogErrorHandlerFunction(cmsContext ContextID, cmsUInt32Number ErrorCode, const char *Text);

// Context0 storage, which is global
_cmsLogErrorChunkType _cmsLogErrorChunk = { DefaultLogErrorHandlerFunction };

// Allocates and inits error logger container for a given context. If src is NULL, only initializes the value
// to the default. Otherwise, it duplicates the value. The interface is standard across all context clients
void _cmsAllocLogErrorChunk(struct _cmsContext_struct* ctx, 
                            const struct _cmsContext_struct* src)
{    
    static _cmsLogErrorChunkType LogErrorChunk = { DefaultLogErrorHandlerFunction };
    void* from;
     
     if (src != NULL) {
        from = src ->chunks[Logger];       
    }
    else {
       from = &LogErrorChunk;
    }
    
    ctx ->chunks[Logger] = _cmsSubAllocDup(ctx ->MemPool, from, sizeof(_cmsLogErrorChunkType));   
}

// The default error logger does nothing.
static
void DefaultLogErrorHandlerFunction(cmsContext ContextID, cmsUInt32Number ErrorCode, const char *Text)
{
    // fprintf(stderr, "[lcms]: %s\n", Text);
    // fflush(stderr);

     cmsUNUSED_PARAMETER(ContextID);
     cmsUNUSED_PARAMETER(ErrorCode);
     cmsUNUSED_PARAMETER(Text);
}

// Change log error, context based
void CMSEXPORT cmsSetLogErrorHandlerTHR(cmsContext ContextID, cmsLogErrorHandlerFunction Fn)
{
    _cmsLogErrorChunkType* lhg = (_cmsLogErrorChunkType*) _cmsContextGetClientChunk(ContextID, Logger);

    if (lhg != NULL) {

        if (Fn == NULL)
            lhg -> LogErrorHandler = DefaultLogErrorHandlerFunction;
        else
            lhg -> LogErrorHandler = Fn;
    }
}

// Change log error, legacy
void CMSEXPORT cmsSetLogErrorHandler(cmsLogErrorHandlerFunction Fn)
{
    cmsSetLogErrorHandlerTHR(NULL, Fn);    
}

// Log an error
// ErrorText is a text holding an english description of error.
void CMSEXPORT cmsSignalError(cmsContext ContextID, cmsUInt32Number ErrorCode, const char *ErrorText, ...)
{
    va_list args;
    char Buffer[MAX_ERROR_MESSAGE_LEN];
    _cmsLogErrorChunkType* lhg;


    va_start(args, ErrorText);
    vsnprintf(Buffer, MAX_ERROR_MESSAGE_LEN-1, ErrorText, args);
    va_end(args);

    // Check for the context, if specified go there. If not, go for the global
    lhg = (_cmsLogErrorChunkType*) _cmsContextGetClientChunk(ContextID, Logger);
    if (lhg ->LogErrorHandler) {
        lhg ->LogErrorHandler(ContextID, ErrorCode, Buffer);
    }   
}

// Utility function to print signatures
void _cmsTagSignature2String(char String[5], cmsTagSignature sig)
{
    cmsUInt32Number be;

    // Convert to big endian
    be = _cmsAdjustEndianess32((cmsUInt32Number) sig);

    // Move chars
    memmove(String, &be, 4);

    // Make sure of terminator
    String[4] = 0;
}

//--------------------------------------------------------------------------------------------------


static
void* defMtxCreate(cmsContext id)
{
    _cmsMutex* ptr_mutex = (_cmsMutex*) _cmsMalloc(id, sizeof(_cmsMutex));
    _cmsInitMutexPrimitive(ptr_mutex);
    return (void*) ptr_mutex;   
}

static
void defMtxDestroy(cmsContext id, void* mtx)
{
    _cmsDestroyMutexPrimitive((_cmsMutex *) mtx); 
    _cmsFree(id, mtx);
}

static
cmsBool defMtxLock(cmsContext id, void* mtx)
{
    cmsUNUSED_PARAMETER(id);
    return _cmsLockPrimitive((_cmsMutex *) mtx) == 0;     
}

static
void defMtxUnlock(cmsContext id, void* mtx)
{
    cmsUNUSED_PARAMETER(id);
    _cmsUnlockPrimitive((_cmsMutex *) mtx); 
}



// Pointers to memory manager functions in Context0
_cmsMutexPluginChunkType _cmsMutexPluginChunk = { defMtxCreate, defMtxDestroy, defMtxLock, defMtxUnlock };

// Allocate and init mutex container.
void _cmsAllocMutexPluginChunk(struct _cmsContext_struct* ctx, 
                                        const struct _cmsContext_struct* src)
{
    static _cmsMutexPluginChunkType MutexChunk = {defMtxCreate, defMtxDestroy, defMtxLock, defMtxUnlock };
    void* from;
     
     if (src != NULL) {
        from = src ->chunks[MutexPlugin];       
    }
    else {
       from = &MutexChunk;
    }
    
    ctx ->chunks[MutexPlugin] = _cmsSubAllocDup(ctx ->MemPool, from, sizeof(_cmsMutexPluginChunkType));   
}

// Register new ways to transform
cmsBool  _cmsRegisterMutexPlugin(cmsContext ContextID, cmsPluginBase* Data)
{
    cmsPluginMutex* Plugin = (cmsPluginMutex*) Data;
    _cmsMutexPluginChunkType* ctx = ( _cmsMutexPluginChunkType*) _cmsContextGetClientChunk(ContextID, MutexPlugin);

    if (Data == NULL) {

        // No lock routines
        ctx->CreateMutexPtr = NULL; 
        ctx->DestroyMutexPtr = NULL; 
        ctx->LockMutexPtr = NULL;
        ctx ->UnlockMutexPtr = NULL;
        return TRUE;
    }

    // Factory callback is required
    if (Plugin ->CreateMutexPtr == NULL || Plugin ->DestroyMutexPtr == NULL || 
        Plugin ->LockMutexPtr == NULL || Plugin ->UnlockMutexPtr == NULL) return FALSE;


    ctx->CreateMutexPtr  = Plugin->CreateMutexPtr;
    ctx->DestroyMutexPtr = Plugin ->DestroyMutexPtr;
    ctx ->LockMutexPtr   = Plugin ->LockMutexPtr;
    ctx ->UnlockMutexPtr = Plugin ->UnlockMutexPtr;

    // All is ok
    return TRUE;
}

// Generic Mutex fns
void* CMSEXPORT _cmsCreateMutex(cmsContext ContextID)
{
    _cmsMutexPluginChunkType* ptr = (_cmsMutexPluginChunkType*) _cmsContextGetClientChunk(ContextID, MutexPlugin);

    if (ptr ->CreateMutexPtr == NULL) return NULL;

    return ptr ->CreateMutexPtr(ContextID);
}

void CMSEXPORT _cmsDestroyMutex(cmsContext ContextID, void* mtx)
{
    _cmsMutexPluginChunkType* ptr = (_cmsMutexPluginChunkType*) _cmsContextGetClientChunk(ContextID, MutexPlugin);

    if (ptr ->DestroyMutexPtr != NULL) {

        ptr ->DestroyMutexPtr(ContextID, mtx);
    }
}

cmsBool CMSEXPORT _cmsLockMutex(cmsContext ContextID, void* mtx)
{
    _cmsMutexPluginChunkType* ptr = (_cmsMutexPluginChunkType*) _cmsContextGetClientChunk(ContextID, MutexPlugin);

    if (ptr ->LockMutexPtr == NULL) return TRUE;

    return ptr ->LockMutexPtr(ContextID, mtx);
}

void CMSEXPORT _cmsUnlockMutex(cmsContext ContextID, void* mtx)
{
    _cmsMutexPluginChunkType* ptr = (_cmsMutexPluginChunkType*) _cmsContextGetClientChunk(ContextID, MutexPlugin);

    if (ptr ->UnlockMutexPtr != NULL) {

        ptr ->UnlockMutexPtr(ContextID, mtx);
    }
}
