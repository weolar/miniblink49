//
// DelayImp.h
//
//  define structures and prototypes necessary for delay loading of imports
//
#if !defined(_delayimp_h)
#define _delayimp_h

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef DELAYLOAD_VERSION
#ifdef _WIN64
#define DELAYLOAD_VERSION 0x200
#else
#define DELAYLOAD_VERSION 0x100
#endif
#endif

#if defined(__cplusplus)
#define ExternC extern "C"
#else
#define ExternC
#endif

typedef IMAGE_THUNK_DATA *          PImgThunkData;
typedef const IMAGE_THUNK_DATA *    PCImgThunkData;
typedef DWORD                       RVA;

typedef struct ImgDelayDescrV2 {
    DWORD           grAttrs;        // attributes
    RVA             rvaDLLName;     // RVA to dll name
    RVA             rvaHmod;        // RVA of module handle
    RVA             rvaIAT;         // RVA of the IAT
    RVA             rvaINT;         // RVA of the INT
    RVA             rvaBoundIAT;    // RVA of the optional bound IAT
    RVA             rvaUnloadIAT;   // RVA of optional copy of original IAT
    DWORD           dwTimeStamp;    // 0 if not bound,
                                    // O.W. date/time stamp of DLL bound to (Old BIND)
    } ImgDelayDescrV2, * PImgDelayDescrV2;

typedef struct ImgDelayDescrV1 {
    DWORD           grAttrs;        // attributes
    LPCSTR          szName;         // pointer to dll name
    HMODULE *       phmod;          // address of module handle
    PImgThunkData   pIAT;           // address of the IAT
    PCImgThunkData  pINT;           // address of the INT
    PCImgThunkData  pBoundIAT;      // address of the optional bound IAT
    PCImgThunkData  pUnloadIAT;     // address of optional copy of original IAT
    DWORD           dwTimeStamp;    // 0 if not bound,
                                    // O.W. date/time stamp of DLL bound to (Old BIND)
    } ImgDelayDescrV1, * PImgDelayDescrV1;

#if DELAYLOAD_VERSION >= 0x0200
typedef ImgDelayDescrV2  ImgDelayDescr;
typedef PImgDelayDescrV2 PImgDelayDescr;
#else
typedef ImgDelayDescrV1  ImgDelayDescr;
typedef PImgDelayDescrV1 PImgDelayDescr;
#endif

typedef const ImgDelayDescr *   PCImgDelayDescr;

enum DLAttr {                   // Delay Load Attributes
    dlattrRva = 0x1,                // RVAs are used instead of pointers
    };

//
// Delay load import hook notifications
//
enum {
    dliStartProcessing,             // used to bypass or note helper only
    dliNotePreLoadLibrary,          // called just before LoadLibrary, can
                                    //  override w/ new HMODULE return val
    dliNotePreGetProcAddress,       // called just before GetProcAddress, can
                                    //  override w/ new FARPROC return value
    dliFailLoadLib,                 // failed to load library, fix it by
                                    //  returning a valid HMODULE
    dliFailGetProc,                 // failed to get proc address, fix it by
                                    //  returning a valid FARPROC
    dliNoteEndProcessing,           // called after all processing is done, no
                                    //  no bypass possible at this point except
                                    //  by longjmp()/throw()/RaiseException.
    };

typedef struct DelayLoadProc {
    BOOL                fImportByName;
    union {
        LPCSTR          szProcName;
        DWORD           dwOrdinal;
        };
    } DelayLoadProc;

typedef struct DelayLoadInfo {
    DWORD               cb;         // size of structure
    PCImgDelayDescr     pidd;       // raw form of data (everything is there)
    FARPROC *           ppfn;       // points to address of function to load
    LPCSTR              szDll;      // name of dll
    DelayLoadProc       dlp;        // name or ordinal of procedure
    HMODULE             hmodCur;    // the hInstance of the library we have loaded
    FARPROC             pfnCur;     // the actual function that will be called
    DWORD               dwLastError;// error received (if an error notification)
    } DelayLoadInfo, * PDelayLoadInfo;

typedef FARPROC (WINAPI *PfnDliHook)(
    unsigned        dliNotify,
    PDelayLoadInfo  pdli
    );

// utility function for calculating the index of the current import
// for all the tables (INT, BIAT, UIAT, and IAT).
__inline unsigned
IndexFromPImgThunkData(PCImgThunkData pitdCur, PCImgThunkData pitdBase) {
    return (unsigned)(pitdCur - pitdBase);
    }

// C++ template utility function for converting RVAs to pointers
//
#if defined(_WIN64) && defined(_M_IA64)
#pragma section(".base", long, read, write)
ExternC
__declspec(allocate(".base"))
extern
IMAGE_DOS_HEADER __ImageBase;
#else
ExternC
extern
IMAGE_DOS_HEADER __ImageBase;
#endif

#if defined(__cplusplus)
template <class X>
X * PFromRva(RVA rva, const X *) {
    return (X*)(PBYTE(&__ImageBase) + rva);
    }
#else
__inline
void *
WINAPI
PFromRva(RVA rva, void *unused) {
    return (PVOID)(((PBYTE)&__ImageBase) + rva);
    }
#endif

//
// Unload support
//

// routine definition; takes a pointer to a name to unload
//
#if DELAYLOAD_VERSION >= 0x0200
ExternC
BOOL WINAPI
__FUnloadDelayLoadedDLL2(LPCSTR szDll);
#else
ExternC
BOOL WINAPI
__FUnloadDelayLoadedDLL(LPCSTR szDll);
#endif

// structure definitions for the list of unload records
typedef struct UnloadInfo * PUnloadInfo;
typedef struct UnloadInfo {
    PUnloadInfo     puiNext;
    PCImgDelayDescr pidd;
    } UnloadInfo;

// the default delay load helper places the unloadinfo records in the list
// headed by the following pointer.
ExternC
extern
PUnloadInfo __puiHead;

//
// Exception information
//
#define FACILITY_VISUALCPP  ((LONG)0x6d)
#define VcppException(sev,err)  ((sev) | (FACILITY_VISUALCPP<<16) | err)

// utility function for calculating the count of imports given the base
// of the IAT.  NB: this only works on a valid IAT!
__inline unsigned
CountOfImports(PCImgThunkData pitdBase) {
    unsigned        cRet = 0;
    PCImgThunkData  pitd = pitdBase;
    while (pitd->u1.Function) {
        pitd++;
        cRet++;
        }
    return cRet;
    }

//
// Hook pointers
//

// The "notify hook" gets called for every call to the
// delay load helper.  This allows a user to hook every call and
// skip the delay load helper entirely.
//
// dliNotify == {
//  dliStartProcessing |
//  dliPreLoadLibrary  |
//  dliPreGetProc |
//  dliNoteEndProcessing}
//  on this call.
//
ExternC
extern
PfnDliHook   __pfnDliNotifyHook;

ExternC
extern
PfnDliHook   __pfnDliNotifyHook2;

// This is the failure hook, dliNotify = {dliFailLoadLib|dliFailGetProc}
ExternC
extern
PfnDliHook   __pfnDliFailureHook;

ExternC
extern
PfnDliHook   __pfnDliFailureHook2;

#if DELAYLOAD_VERSION >= 0x0200
#define __pfnDliFailureHook __pfnDliFailureHook2
#define __pfnDliNotifyHook  __pfnDliNotifyHook2
#endif

#endif

