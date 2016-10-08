/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    WinSCard

Abstract:

    This header file provides the definitions and symbols necessary for an
    Application or Smart Card Service Provider to access the Smartcard
    Subsystem.

Environment:

    Win32

Notes:

--*/

#ifndef _WINSCARD_H_
#define _WINSCARD_H_

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <wtypes.h>
#include <winioctl.h>
#include "winsmcrd.h"
#ifndef SCARD_S_SUCCESS
#include "SCardErr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LPCBYTE_DEFINED
#define _LPCBYTE_DEFINED
typedef const BYTE *LPCBYTE;
#endif
#ifndef _LPCVOID_DEFINED
#define _LPCVOID_DEFINED
typedef const VOID *LPCVOID;
#endif

#ifndef WINSCARDAPI
#define WINSCARDAPI
#endif
#ifndef WINSCARDDATA
#define WINSCARDDATA __declspec(dllimport)
#endif

/* In clr:pure we cannot mark data export with dllimport.
 * We should add small functions which returns the value of
 * the global.
 */
#if !defined(_M_CEE_PURE)
WINSCARDDATA extern const SCARD_IO_REQUEST
    g_rgSCardT0Pci,
    g_rgSCardT1Pci,
    g_rgSCardRawPci;
#define SCARD_PCI_T0  (&g_rgSCardT0Pci)
#define SCARD_PCI_T1  (&g_rgSCardT1Pci)
#define SCARD_PCI_RAW (&g_rgSCardRawPci)
#endif


//
////////////////////////////////////////////////////////////////////////////////
//
//  Service Manager Access Services
//
//      The following services are used to manage user and terminal contexts for
//      Smart Cards.
//

typedef ULONG_PTR SCARDCONTEXT;
typedef SCARDCONTEXT *PSCARDCONTEXT, *LPSCARDCONTEXT;

typedef ULONG_PTR SCARDHANDLE;
typedef SCARDHANDLE *PSCARDHANDLE, *LPSCARDHANDLE;

#define SCARD_AUTOALLOCATE (DWORD)(-1)

#define SCARD_SCOPE_USER     0  // The context is a user context, and any
                                // database operations are performed within the
                                // domain of the user.
#define SCARD_SCOPE_TERMINAL 1  // The context is that of the current terminal,
                                // and any database operations are performed
                                // within the domain of that terminal.  (The
                                // calling application must have appropriate
                                // access permissions for any database actions.)
#define SCARD_SCOPE_SYSTEM    2 // The context is the system context, and any
                                // database operations are performed within the
                                // domain of the system.  (The calling
                                // application must have appropriate access
                                // permissions for any database actions.)

extern WINSCARDAPI LONG WINAPI
SCardEstablishContext(
    IN  DWORD dwScope,
    IN  LPCVOID pvReserved1,
    IN  LPCVOID pvReserved2,
    OUT LPSCARDCONTEXT phContext);

extern WINSCARDAPI LONG WINAPI
SCardReleaseContext(
    IN      SCARDCONTEXT hContext);

extern WINSCARDAPI LONG WINAPI
SCardIsValidContext(
    IN      SCARDCONTEXT hContext);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Smart Card Database Management Services
//
//      The following services provide for managing the Smart Card Database.
//

#define SCARD_ALL_READERS       TEXT("SCard$AllReaders\000")
#define SCARD_DEFAULT_READERS   TEXT("SCard$DefaultReaders\000")
#define SCARD_LOCAL_READERS     TEXT("SCard$LocalReaders\000")
#define SCARD_SYSTEM_READERS    TEXT("SCard$SystemReaders\000")

#define SCARD_PROVIDER_PRIMARY  1   // Primary Provider Id
#define SCARD_PROVIDER_CSP      2   // Crypto Service Provider Id


//
// Database Reader routines
//

extern WINSCARDAPI LONG WINAPI
SCardListReaderGroupsA(
    IN      SCARDCONTEXT hContext,
    OUT     LPSTR mszGroups,
    IN OUT  LPDWORD pcchGroups);
extern WINSCARDAPI LONG WINAPI
SCardListReaderGroupsW(
    IN      SCARDCONTEXT hContext,
    OUT     LPWSTR mszGroups,
    IN OUT  LPDWORD pcchGroups);
#ifdef UNICODE
#define SCardListReaderGroups  SCardListReaderGroupsW
#else
#define SCardListReaderGroups  SCardListReaderGroupsA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardListReadersA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszGroups,
    OUT     LPSTR mszReaders,
    IN OUT  LPDWORD pcchReaders);
extern WINSCARDAPI LONG WINAPI
SCardListReadersW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszGroups,
    OUT     LPWSTR mszReaders,
    IN OUT  LPDWORD pcchReaders);
#ifdef UNICODE
#define SCardListReaders  SCardListReadersW
#else
#define SCardListReaders  SCardListReadersA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardListCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPSTR mszCards,
    IN OUT  LPDWORD pcchCards);
extern WINSCARDAPI LONG WINAPI
SCardListCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCBYTE pbAtr,
    IN      LPCGUID rgquidInterfaces,
    IN      DWORD cguidInterfaceCount,
    OUT     LPWSTR mszCards,
    IN OUT  LPDWORD pcchCards);
#ifdef UNICODE
#define SCardListCards  SCardListCardsW
#else
#define SCardListCards  SCardListCardsA
#endif // !UNICODE
//
// NOTE:    The routine SCardListCards name differs from the PC/SC definition.
//          It should be:
//
//              extern WINSCARDAPI LONG WINAPI
//              SCardListCardTypes(
//                  IN      SCARDCONTEXT hContext,
//                  IN      LPCBYTE pbAtr,
//                  IN      LPCGUID rgquidInterfaces,
//                  IN      DWORD cguidInterfaceCount,
//                  OUT     LPTSTR mszCards,
//                  IN OUT  LPDWORD pcchCards);
//
//          Here's a work-around MACRO:
#define SCardListCardTypes SCardListCards

extern WINSCARDAPI LONG WINAPI
SCardListInterfacesA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces);
extern WINSCARDAPI LONG WINAPI
SCardListInterfacesW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidInterfaces,
    IN OUT  LPDWORD pcguidInterfaces);
#ifdef UNICODE
#define SCardListInterfaces  SCardListInterfacesW
#else
#define SCardListInterfaces  SCardListInterfacesA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardGetProviderIdA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szCard,
    OUT     LPGUID pguidProviderId);
extern WINSCARDAPI LONG WINAPI
SCardGetProviderIdW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szCard,
    OUT     LPGUID pguidProviderId);
#ifdef UNICODE
#define SCardGetProviderId  SCardGetProviderIdW
#else
#define SCardGetProviderId  SCardGetProviderIdA
#endif // !UNICODE
//
// NOTE:    The routine SCardGetProviderId in this implementation uses GUIDs.
//          The PC/SC definition uses BYTEs.
//

extern WINSCARDAPI LONG WINAPI
SCardGetCardTypeProviderNameA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName,
    IN DWORD dwProviderId,
    OUT LPSTR szProvider,
    IN OUT LPDWORD pcchProvider);
extern WINSCARDAPI LONG WINAPI
SCardGetCardTypeProviderNameW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName,
    IN DWORD dwProviderId,
    OUT LPWSTR szProvider,
    IN OUT LPDWORD pcchProvider);
#ifdef UNICODE
#define SCardGetCardTypeProviderName  SCardGetCardTypeProviderNameW
#else
#define SCardGetCardTypeProviderName  SCardGetCardTypeProviderNameA
#endif // !UNICODE
//
// NOTE:    This routine is an extension to the PC/SC definitions.
//


//
// Database Writer routines
//

extern WINSCARDAPI LONG WINAPI
SCardIntroduceReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
SCardIntroduceReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define SCardIntroduceReaderGroup  SCardIntroduceReaderGroupW
#else
#define SCardIntroduceReaderGroup  SCardIntroduceReaderGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardForgetReaderGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
SCardForgetReaderGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define SCardForgetReaderGroup  SCardForgetReaderGroupW
#else
#define SCardForgetReaderGroup  SCardForgetReaderGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardIntroduceReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szDeviceName);
extern WINSCARDAPI LONG WINAPI
SCardIntroduceReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szDeviceName);
#ifdef UNICODE
#define SCardIntroduceReader  SCardIntroduceReaderW
#else
#define SCardIntroduceReader  SCardIntroduceReaderA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardForgetReaderA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName);
extern WINSCARDAPI LONG WINAPI
SCardForgetReaderW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName);
#ifdef UNICODE
#define SCardForgetReader  SCardForgetReaderW
#else
#define SCardForgetReader  SCardForgetReaderA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardAddReaderToGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
SCardAddReaderToGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define SCardAddReaderToGroup  SCardAddReaderToGroupW
#else
#define SCardAddReaderToGroup  SCardAddReaderToGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardRemoveReaderFromGroupA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szReaderName,
    IN LPCSTR szGroupName);
extern WINSCARDAPI LONG WINAPI
SCardRemoveReaderFromGroupW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szReaderName,
    IN LPCWSTR szGroupName);
#ifdef UNICODE
#define SCardRemoveReaderFromGroup  SCardRemoveReaderFromGroupW
#else
#define SCardRemoveReaderFromGroup  SCardRemoveReaderFromGroupA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardIntroduceCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName,
    IN LPCGUID pguidPrimaryProvider,
    IN LPCGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen);
extern WINSCARDAPI LONG WINAPI
SCardIntroduceCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName,
    IN LPCGUID pguidPrimaryProvider,
    IN LPCGUID rgguidInterfaces,
    IN DWORD dwInterfaceCount,
    IN LPCBYTE pbAtr,
    IN LPCBYTE pbAtrMask,
    IN DWORD cbAtrLen);
#ifdef UNICODE
#define SCardIntroduceCardType  SCardIntroduceCardTypeW
#else
#define SCardIntroduceCardType  SCardIntroduceCardTypeA
#endif // !UNICODE
//
// NOTE:    The routine SCardIntroduceCardType's parameters' order differs from
//          the PC/SC definition.  It should be:
//
//              extern WINSCARDAPI LONG WINAPI
//              SCardIntroduceCardType(
//                  IN SCARDCONTEXT hContext,
//                  IN LPCTSTR szCardName,
//                  IN LPCBYTE pbAtr,
//                  IN LPCBYTE pbAtrMask,
//                  IN DWORD cbAtrLen,
//                  IN LPCGUID pguidPrimaryProvider,
//                  IN LPCGUID rgguidInterfaces,
//                  IN DWORD dwInterfaceCount);
//
//          Here's a work-around MACRO:
#define PCSCardIntroduceCardType(hContext, szCardName, pbAtr, pbAtrMask, cbAtrLen, pguidPrimaryProvider, rgguidInterfaces, dwInterfaceCount) \
          SCardIntroduceCardType(hContext, szCardName, pguidPrimaryProvider, rgguidInterfaces, dwInterfaceCount, pbAtr, pbAtrMask, cbAtrLen)

extern WINSCARDAPI LONG WINAPI
SCardSetCardTypeProviderNameA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName,
    IN DWORD dwProviderId,
    IN LPCSTR szProvider);
extern WINSCARDAPI LONG WINAPI
SCardSetCardTypeProviderNameW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName,
    IN DWORD dwProviderId,
    IN LPCWSTR szProvider);
#ifdef UNICODE
#define SCardSetCardTypeProviderName  SCardSetCardTypeProviderNameW
#else
#define SCardSetCardTypeProviderName  SCardSetCardTypeProviderNameA
#endif // !UNICODE
//
// NOTE:    This routine is an extention to the PC/SC specifications.
//

extern WINSCARDAPI LONG WINAPI
SCardForgetCardTypeA(
    IN SCARDCONTEXT hContext,
    IN LPCSTR szCardName);
extern WINSCARDAPI LONG WINAPI
SCardForgetCardTypeW(
    IN SCARDCONTEXT hContext,
    IN LPCWSTR szCardName);
#ifdef UNICODE
#define SCardForgetCardType  SCardForgetCardTypeW
#else
#define SCardForgetCardType  SCardForgetCardTypeA
#endif // !UNICODE


//
////////////////////////////////////////////////////////////////////////////////
//
//  Service Manager Support Routines
//
//      The following services are supplied to simplify the use of the Service
//      Manager API.
//

extern WINSCARDAPI LONG WINAPI
SCardFreeMemory(
    IN SCARDCONTEXT hContext,
    IN LPCVOID pvMem);

extern WINSCARDAPI HANDLE WINAPI
SCardAccessStartedEvent(void);

extern WINSCARDAPI void WINAPI
SCardReleaseStartedEvent(void);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Reader Services
//
//      The following services supply means for tracking cards within readers.
//

typedef struct {
    LPCSTR      szReader;       // reader name
    LPVOID      pvUserData;     // user defined data
    DWORD       dwCurrentState; // current state of reader at time of call
    DWORD       dwEventState;   // state of reader after state change
    DWORD       cbAtr;          // Number of bytes in the returned ATR.
    BYTE        rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
} SCARD_READERSTATEA, *PSCARD_READERSTATEA, *LPSCARD_READERSTATEA;
typedef struct {
    LPCWSTR     szReader;       // reader name
    LPVOID      pvUserData;     // user defined data
    DWORD       dwCurrentState; // current state of reader at time of call
    DWORD       dwEventState;   // state of reader after state change
    DWORD       cbAtr;          // Number of bytes in the returned ATR.
    BYTE        rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
} SCARD_READERSTATEW, *PSCARD_READERSTATEW, *LPSCARD_READERSTATEW;
#ifdef UNICODE
typedef SCARD_READERSTATEW SCARD_READERSTATE;
typedef PSCARD_READERSTATEW PSCARD_READERSTATE;
typedef LPSCARD_READERSTATEW LPSCARD_READERSTATE;
#else
typedef SCARD_READERSTATEA SCARD_READERSTATE;
typedef PSCARD_READERSTATEA PSCARD_READERSTATE;
typedef LPSCARD_READERSTATEA LPSCARD_READERSTATE;
#endif // UNICODE

// Backwards compatibility macros
#define SCARD_READERSTATE_A SCARD_READERSTATEA
#define SCARD_READERSTATE_W SCARD_READERSTATEW
#define PSCARD_READERSTATE_A PSCARD_READERSTATEA
#define PSCARD_READERSTATE_W PSCARD_READERSTATEW
#define LPSCARD_READERSTATE_A LPSCARD_READERSTATEA
#define LPSCARD_READERSTATE_W LPSCARD_READERSTATEW

#define SCARD_STATE_UNAWARE     0x00000000  // The application is unaware of the
                                            // current state, and would like to
                                            // know.  The use of this value
                                            // results in an immediate return
                                            // from state transition monitoring
                                            // services.  This is represented by
                                            // all bits set to zero.
#define SCARD_STATE_IGNORE      0x00000001  // The application requested that
                                            // this reader be ignored.  No other
                                            // bits will be set.
#define SCARD_STATE_CHANGED     0x00000002  // This implies that there is a
                                            // difference between the state
                                            // believed by the application, and
                                            // the state known by the Service
                                            // Manager.  When this bit is set,
                                            // the application may assume a
                                            // significant state change has
                                            // occurred on this reader.
#define SCARD_STATE_UNKNOWN     0x00000004  // This implies that the given
                                            // reader name is not recognized by
                                            // the Service Manager.  If this bit
                                            // is set, then SCARD_STATE_CHANGED
                                            // and SCARD_STATE_IGNORE will also
                                            // be set.
#define SCARD_STATE_UNAVAILABLE 0x00000008  // This implies that the actual
                                            // state of this reader is not
                                            // available.  If this bit is set,
                                            // then all the following bits are
                                            // clear.
#define SCARD_STATE_EMPTY       0x00000010  // This implies that there is not
                                            // card in the reader.  If this bit
                                            // is set, all the following bits
                                            // will be clear.
#define SCARD_STATE_PRESENT     0x00000020  // This implies that there is a card
                                            // in the reader.
#define SCARD_STATE_ATRMATCH    0x00000040  // This implies that there is a card
                                            // in the reader with an ATR
                                            // matching one of the target cards.
                                            // If this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.  This bit is only returned
                                            // on the SCardLocateCard() service.
#define SCARD_STATE_EXCLUSIVE   0x00000080  // This implies that the card in the
                                            // reader is allocated for exclusive
                                            // use by another application.  If
                                            // this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.
#define SCARD_STATE_INUSE       0x00000100  // This implies that the card in the
                                            // reader is in use by one or more
                                            // other applications, but may be
                                            // connected to in shared mode.  If
                                            // this bit is set,
                                            // SCARD_STATE_PRESENT will also be
                                            // set.
#define SCARD_STATE_MUTE        0x00000200  // This implies that the card in the
                                            // reader is unresponsive or not
                                            // supported by the reader or
                                            // software.
#define SCARD_STATE_UNPOWERED   0x00000400  // This implies that the card in the
                                            // reader has not been powered up.

extern WINSCARDAPI LONG WINAPI
SCardLocateCardsA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR mszCards,
    IN OUT  LPSCARD_READERSTATEA rgReaderStates,
    IN      DWORD cReaders);
extern WINSCARDAPI LONG WINAPI
SCardLocateCardsW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR mszCards,
    IN OUT  LPSCARD_READERSTATEW rgReaderStates,
    IN      DWORD cReaders);
#ifdef UNICODE
#define SCardLocateCards  SCardLocateCardsW
#else
#define SCardLocateCards  SCardLocateCardsA
#endif // !UNICODE

typedef struct _SCARD_ATRMASK {
    DWORD       cbAtr;          // Number of bytes in the ATR and the mask.
    BYTE        rgbAtr[36];     // Atr of card (extra alignment bytes)
    BYTE        rgbMask[36];    // Mask for the Atr (extra alignment bytes)
} SCARD_ATRMASK, *PSCARD_ATRMASK, *LPSCARD_ATRMASK;


extern WINSCARDAPI LONG WINAPI
SCardLocateCardsByATRA(
    IN      SCARDCONTEXT hContext,
    IN      LPSCARD_ATRMASK rgAtrMasks,
    IN      DWORD cAtrs,
    IN OUT  LPSCARD_READERSTATEA rgReaderStates,
    IN      DWORD cReaders);
extern WINSCARDAPI LONG WINAPI
SCardLocateCardsByATRW(
    IN      SCARDCONTEXT hContext,
    IN      LPSCARD_ATRMASK rgAtrMasks,
    IN      DWORD cAtrs,
    IN OUT  LPSCARD_READERSTATEW rgReaderStates,
    IN      DWORD cReaders);
#ifdef UNICODE
#define SCardLocateCardsByATR  SCardLocateCardsByATRW
#else
#define SCardLocateCardsByATR  SCardLocateCardsByATRA
#endif // !UNICODE


extern WINSCARDAPI LONG WINAPI
SCardGetStatusChangeA(
    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATEA rgReaderStates,
    IN      DWORD cReaders);
extern WINSCARDAPI LONG WINAPI
SCardGetStatusChangeW(
    IN      SCARDCONTEXT hContext,
    IN      DWORD dwTimeout,
    IN OUT  LPSCARD_READERSTATEW rgReaderStates,
    IN      DWORD cReaders);
#ifdef UNICODE
#define SCardGetStatusChange  SCardGetStatusChangeW
#else
#define SCardGetStatusChange  SCardGetStatusChangeA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardCancel(
    IN      SCARDCONTEXT hContext);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Card/Reader Communication Services
//
//      The following services provide means for communication with the card.
//

#define SCARD_SHARE_EXCLUSIVE 1 // This application is not willing to share this
                                // card with other applications.
#define SCARD_SHARE_SHARED    2 // This application is willing to share this
                                // card with other applications.
#define SCARD_SHARE_DIRECT    3 // This application demands direct control of
                                // the reader, so it is not available to other
                                // applications.

#define SCARD_LEAVE_CARD      0 // Don't do anything special on close
#define SCARD_RESET_CARD      1 // Reset the card on close
#define SCARD_UNPOWER_CARD    2 // Power down the card on close
#define SCARD_EJECT_CARD      3 // Eject the card on close

extern WINSCARDAPI LONG WINAPI
SCardConnectA(
    IN      SCARDCONTEXT hContext,
    IN      LPCSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol);
extern WINSCARDAPI LONG WINAPI
SCardConnectW(
    IN      SCARDCONTEXT hContext,
    IN      LPCWSTR szReader,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    OUT     LPSCARDHANDLE phCard,
    OUT     LPDWORD pdwActiveProtocol);
#ifdef UNICODE
#define SCardConnect  SCardConnectW
#else
#define SCardConnect  SCardConnectA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardReconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwShareMode,
    IN      DWORD dwPreferredProtocols,
    IN      DWORD dwInitialization,
    OUT     LPDWORD pdwActiveProtocol);

extern WINSCARDAPI LONG WINAPI
SCardDisconnect(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition);

extern WINSCARDAPI LONG WINAPI
SCardBeginTransaction(
    IN      SCARDHANDLE hCard);

extern WINSCARDAPI LONG WINAPI
SCardEndTransaction(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwDisposition);

extern WINSCARDAPI LONG WINAPI
SCardCancelTransaction(
    IN      SCARDHANDLE hCard);
//
// NOTE:    This call corresponds to the PC/SC SCARDCOMM::Cancel routine,
//          terminating a blocked SCardBeginTransaction service.
//


extern WINSCARDAPI LONG WINAPI
SCardState(
    IN SCARDHANDLE hCard,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    IN OUT LPDWORD pcbAtrLen);
//
// NOTE:    SCardState is an obsolete routine.  PC/SC has replaced it with
//          SCardStatus.
//

extern WINSCARDAPI LONG WINAPI
SCardStatusA(
    IN SCARDHANDLE hCard,
    OUT LPSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    IN OUT LPDWORD pcbAtrLen);
extern WINSCARDAPI LONG WINAPI
SCardStatusW(
    IN SCARDHANDLE hCard,
    OUT LPWSTR szReaderName,
    IN OUT LPDWORD pcchReaderLen,
    OUT LPDWORD pdwState,
    OUT LPDWORD pdwProtocol,
    OUT LPBYTE pbAtr,
    IN OUT LPDWORD pcbAtrLen);
#ifdef UNICODE
#define SCardStatus  SCardStatusW
#else
#define SCardStatus  SCardStatusA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardTransmit(
    IN SCARDHANDLE hCard,
    IN LPCSCARD_IO_REQUEST pioSendPci,
    IN LPCBYTE pbSendBuffer,
    IN DWORD cbSendLength,
    IN OUT LPSCARD_IO_REQUEST pioRecvPci,
    OUT LPBYTE pbRecvBuffer,
    IN OUT LPDWORD pcbRecvLength);


//
////////////////////////////////////////////////////////////////////////////////
//
//  Reader Control Routines
//
//      The following services provide for direct, low-level manipulation of the
//      reader by the calling application allowing it control over the
//      attributes of the communications with the card.
//

extern WINSCARDAPI LONG WINAPI
SCardControl(
    IN      SCARDHANDLE hCard,
    IN      DWORD dwControlCode,
    IN      LPCVOID lpInBuffer,
    IN      DWORD nInBufferSize,
    OUT     LPVOID lpOutBuffer,
    IN      DWORD nOutBufferSize,
    OUT     LPDWORD lpBytesReturned);

extern WINSCARDAPI LONG WINAPI
SCardGetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    OUT LPBYTE pbAttr,
    IN OUT LPDWORD pcbAttrLen);
//
// NOTE:    The routine SCardGetAttrib's name differs from the PC/SC definition.
//          It should be:
//
//              extern WINSCARDAPI LONG WINAPI
//              SCardGetReaderCapabilities(
//                  IN SCARDHANDLE hCard,
//                  IN DWORD dwTag,
//                  OUT LPBYTE pbAttr,
//                  IN OUT LPDWORD pcbAttrLen);
//
//          Here's a work-around MACRO:
#define SCardGetReaderCapabilities SCardGetAttrib

extern WINSCARDAPI LONG WINAPI
SCardSetAttrib(
    IN SCARDHANDLE hCard,
    IN DWORD dwAttrId,
    IN LPCBYTE pbAttr,
    IN DWORD cbAttrLen);
//
// NOTE:    The routine SCardSetAttrib's name differs from the PC/SC definition.
//          It should be:
//
//              extern WINSCARDAPI LONG WINAPI
//              SCardSetReaderCapabilities(
//                  IN SCARDHANDLE hCard,
//                  IN DWORD dwTag,
//                  OUT LPBYTE pbAttr,
//                  IN OUT LPDWORD pcbAttrLen);
//
//          Here's a work-around MACRO:
#define SCardSetReaderCapabilities SCardSetAttrib


//
////////////////////////////////////////////////////////////////////////////////
//
//  Smart Card Dialog definitions
//
//      The following section contains structures and  exported function
//      declarations for the Smart Card Common Dialog dialog.
//

// Defined constants
// Flags
#define SC_DLG_MINIMAL_UI       0x01
#define SC_DLG_NO_UI            0x02
#define SC_DLG_FORCE_UI         0x04

#define SCERR_NOCARDNAME        0x4000
#define SCERR_NOGUIDS           0x8000

typedef SCARDHANDLE (WINAPI *LPOCNCONNPROCA) (IN SCARDCONTEXT, IN LPSTR, IN LPSTR, IN PVOID);
typedef SCARDHANDLE (WINAPI *LPOCNCONNPROCW) (IN SCARDCONTEXT, IN LPWSTR, IN LPWSTR, IN PVOID);
#ifdef UNICODE
#define LPOCNCONNPROC  LPOCNCONNPROCW
#else
#define LPOCNCONNPROC  LPOCNCONNPROCA
#endif // !UNICODE
typedef BOOL (WINAPI *LPOCNCHKPROC) (IN SCARDCONTEXT, IN SCARDHANDLE, IN PVOID);
typedef void (WINAPI *LPOCNDSCPROC) (IN SCARDCONTEXT, IN SCARDHANDLE, IN PVOID);


//
// OPENCARD_SEARCH_CRITERIA: In order to specify a user-extended search,
// lpfnCheck must not be NULL.  Moreover, the connection to be made to the
// card before performing the callback must be indicated by either providing
// lpfnConnect and lpfnDisconnect OR by setting dwShareMode.
// If both the connection callbacks and dwShareMode are non-NULL, the callbacks
// will be used.
//

typedef struct {
    DWORD           dwStructSize;
    LPSTR           lpstrGroupNames;        // OPTIONAL reader groups to include in
    DWORD           nMaxGroupNames;         //          search.  NULL defaults to
                                            //          SCard$DefaultReaders
    LPCGUID         rgguidInterfaces;       // OPTIONAL requested interfaces
    DWORD           cguidInterfaces;        //          supported by card's SSP
    LPSTR           lpstrCardNames;         // OPTIONAL requested card names; all cards w/
    DWORD           nMaxCardNames;          //          matching ATRs will be accepted
    LPOCNCHKPROC    lpfnCheck;              // OPTIONAL if NULL no user check will be performed.
    LPOCNCONNPROCA  lpfnConnect;            // OPTIONAL if lpfnConnect is provided,
    LPOCNDSCPROC    lpfnDisconnect;         //          lpfnDisconnect must also be set.
    LPVOID          pvUserData;             // OPTIONAL parameter to callbacks
    DWORD           dwShareMode;            // OPTIONAL must be set if lpfnCheck is not null
    DWORD           dwPreferredProtocols;   // OPTIONAL
} OPENCARD_SEARCH_CRITERIAA, *POPENCARD_SEARCH_CRITERIAA, *LPOPENCARD_SEARCH_CRITERIAA;
typedef struct {
    DWORD           dwStructSize;
    LPWSTR          lpstrGroupNames;        // OPTIONAL reader groups to include in
    DWORD           nMaxGroupNames;         //          search.  NULL defaults to
                                            //          SCard$DefaultReaders
    LPCGUID         rgguidInterfaces;       // OPTIONAL requested interfaces
    DWORD           cguidInterfaces;        //          supported by card's SSP
    LPWSTR          lpstrCardNames;         // OPTIONAL requested card names; all cards w/
    DWORD           nMaxCardNames;          //          matching ATRs will be accepted
    LPOCNCHKPROC    lpfnCheck;              // OPTIONAL if NULL no user check will be performed.
    LPOCNCONNPROCW  lpfnConnect;            // OPTIONAL if lpfnConnect is provided,
    LPOCNDSCPROC    lpfnDisconnect;         //          lpfnDisconnect must also be set.
    LPVOID          pvUserData;             // OPTIONAL parameter to callbacks
    DWORD           dwShareMode;            // OPTIONAL must be set if lpfnCheck is not null
    DWORD           dwPreferredProtocols;   // OPTIONAL
} OPENCARD_SEARCH_CRITERIAW, *POPENCARD_SEARCH_CRITERIAW, *LPOPENCARD_SEARCH_CRITERIAW;
#ifdef UNICODE
typedef OPENCARD_SEARCH_CRITERIAW OPENCARD_SEARCH_CRITERIA;
typedef POPENCARD_SEARCH_CRITERIAW POPENCARD_SEARCH_CRITERIA;
typedef LPOPENCARD_SEARCH_CRITERIAW LPOPENCARD_SEARCH_CRITERIA;
#else
typedef OPENCARD_SEARCH_CRITERIAA OPENCARD_SEARCH_CRITERIA;
typedef POPENCARD_SEARCH_CRITERIAA POPENCARD_SEARCH_CRITERIA;
typedef LPOPENCARD_SEARCH_CRITERIAA LPOPENCARD_SEARCH_CRITERIA;
#endif // UNICODE


//
// OPENCARDNAME_EX: used by SCardUIDlgSelectCard; replaces obsolete OPENCARDNAME
//

typedef struct {
    DWORD           dwStructSize;           // REQUIRED
    SCARDCONTEXT    hSCardContext;          // REQUIRED
    HWND            hwndOwner;              // OPTIONAL
    DWORD           dwFlags;                // OPTIONAL -- default is SC_DLG_MINIMAL_UI
    LPCSTR          lpstrTitle;             // OPTIONAL
    LPCSTR          lpstrSearchDesc;        // OPTIONAL (eg. "Please insert your <brandname> smart card.")
    HICON           hIcon;                  // OPTIONAL 32x32 icon for your brand insignia
    POPENCARD_SEARCH_CRITERIAA pOpenCardSearchCriteria; // OPTIONAL
    LPOCNCONNPROCA  lpfnConnect;            // OPTIONAL - performed on successful selection
    LPVOID          pvUserData;             // OPTIONAL parameter to lpfnConnect
    DWORD           dwShareMode;            // OPTIONAL - if lpfnConnect is NULL, dwShareMode and
    DWORD           dwPreferredProtocols;   // OPTIONAL dwPreferredProtocols will be used to
                                            //          connect to the selected card
    LPSTR           lpstrRdr;               // REQUIRED [IN|OUT] Name of selected reader
    DWORD           nMaxRdr;                // REQUIRED [IN|OUT]
    LPSTR           lpstrCard;              // REQUIRED [IN|OUT] Name of selected card
    DWORD           nMaxCard;               // REQUIRED [IN|OUT]
    DWORD           dwActiveProtocol;       // [OUT] set only if dwShareMode not NULL
    SCARDHANDLE     hCardHandle;            // [OUT] set if a card connection was indicated
} OPENCARDNAME_EXA, *POPENCARDNAME_EXA, *LPOPENCARDNAME_EXA;
typedef struct {
    DWORD           dwStructSize;           // REQUIRED
    SCARDCONTEXT    hSCardContext;          // REQUIRED
    HWND            hwndOwner;              // OPTIONAL
    DWORD           dwFlags;                // OPTIONAL -- default is SC_DLG_MINIMAL_UI
    LPCWSTR         lpstrTitle;             // OPTIONAL
    LPCWSTR         lpstrSearchDesc;        // OPTIONAL (eg. "Please insert your <brandname> smart card.")
    HICON           hIcon;                  // OPTIONAL 32x32 icon for your brand insignia
    POPENCARD_SEARCH_CRITERIAW pOpenCardSearchCriteria; // OPTIONAL
    LPOCNCONNPROCW  lpfnConnect;            // OPTIONAL - performed on successful selection
    LPVOID          pvUserData;             // OPTIONAL parameter to lpfnConnect
    DWORD           dwShareMode;            // OPTIONAL - if lpfnConnect is NULL, dwShareMode and
    DWORD           dwPreferredProtocols;   // OPTIONAL dwPreferredProtocols will be used to
                                            //          connect to the selected card
    LPWSTR          lpstrRdr;               // REQUIRED [IN|OUT] Name of selected reader
    DWORD           nMaxRdr;                // REQUIRED [IN|OUT]
    LPWSTR          lpstrCard;              // REQUIRED [IN|OUT] Name of selected card
    DWORD           nMaxCard;               // REQUIRED [IN|OUT]
    DWORD           dwActiveProtocol;       // [OUT] set only if dwShareMode not NULL
    SCARDHANDLE     hCardHandle;            // [OUT] set if a card connection was indicated
} OPENCARDNAME_EXW, *POPENCARDNAME_EXW, *LPOPENCARDNAME_EXW;
#ifdef UNICODE
typedef OPENCARDNAME_EXW OPENCARDNAME_EX;
typedef POPENCARDNAME_EXW POPENCARDNAME_EX;
typedef LPOPENCARDNAME_EXW LPOPENCARDNAME_EX;
#else
typedef OPENCARDNAME_EXA OPENCARDNAME_EX;
typedef POPENCARDNAME_EXA POPENCARDNAME_EX;
typedef LPOPENCARDNAME_EXA LPOPENCARDNAME_EX;
#endif // UNICODE

#define OPENCARDNAMEA_EX OPENCARDNAME_EXA
#define OPENCARDNAMEW_EX OPENCARDNAME_EXW
#define POPENCARDNAMEA_EX POPENCARDNAME_EXA
#define POPENCARDNAMEW_EX POPENCARDNAME_EXW
#define LPOPENCARDNAMEA_EX LPOPENCARDNAME_EXA
#define LPOPENCARDNAMEW_EX LPOPENCARDNAME_EXW


//
// SCardUIDlgSelectCard replaces GetOpenCardName
//

extern WINSCARDAPI LONG WINAPI
SCardUIDlgSelectCardA(
    LPOPENCARDNAMEA_EX);
extern WINSCARDAPI LONG WINAPI
SCardUIDlgSelectCardW(
    LPOPENCARDNAMEW_EX);
#ifdef UNICODE
#define SCardUIDlgSelectCard  SCardUIDlgSelectCardW
#else
#define SCardUIDlgSelectCard  SCardUIDlgSelectCardA
#endif // !UNICODE


//
// "Smart Card Common Dialog" definitions for backwards compatibility
//  with the Smart Card Base Services SDK version 1.0
//

typedef struct {
    DWORD           dwStructSize;
    HWND            hwndOwner;
    SCARDCONTEXT    hSCardContext;
    LPSTR           lpstrGroupNames;
    DWORD           nMaxGroupNames;
    LPSTR           lpstrCardNames;
    DWORD           nMaxCardNames;
    LPCGUID         rgguidInterfaces;
    DWORD           cguidInterfaces;
    LPSTR           lpstrRdr;
    DWORD           nMaxRdr;
    LPSTR           lpstrCard;
    DWORD           nMaxCard;
    LPCSTR          lpstrTitle;
    DWORD           dwFlags;
    LPVOID          pvUserData;
    DWORD           dwShareMode;
    DWORD           dwPreferredProtocols;
    DWORD           dwActiveProtocol;
    LPOCNCONNPROCA  lpfnConnect;
    LPOCNCHKPROC    lpfnCheck;
    LPOCNDSCPROC    lpfnDisconnect;
    SCARDHANDLE     hCardHandle;
} OPENCARDNAMEA, *POPENCARDNAMEA, *LPOPENCARDNAMEA;
typedef struct {
    DWORD           dwStructSize;
    HWND            hwndOwner;
    SCARDCONTEXT    hSCardContext;
    LPWSTR          lpstrGroupNames;
    DWORD           nMaxGroupNames;
    LPWSTR          lpstrCardNames;
    DWORD           nMaxCardNames;
    LPCGUID         rgguidInterfaces;
    DWORD           cguidInterfaces;
    LPWSTR          lpstrRdr;
    DWORD           nMaxRdr;
    LPWSTR          lpstrCard;
    DWORD           nMaxCard;
    LPCWSTR         lpstrTitle;
    DWORD           dwFlags;
    LPVOID          pvUserData;
    DWORD           dwShareMode;
    DWORD           dwPreferredProtocols;
    DWORD           dwActiveProtocol;
    LPOCNCONNPROCW  lpfnConnect;
    LPOCNCHKPROC    lpfnCheck;
    LPOCNDSCPROC    lpfnDisconnect;
    SCARDHANDLE     hCardHandle;
} OPENCARDNAMEW, *POPENCARDNAMEW, *LPOPENCARDNAMEW;
#ifdef UNICODE
typedef OPENCARDNAMEW OPENCARDNAME;
typedef POPENCARDNAMEW POPENCARDNAME;
typedef LPOPENCARDNAMEW LPOPENCARDNAME;
#else
typedef OPENCARDNAMEA OPENCARDNAME;
typedef POPENCARDNAMEA POPENCARDNAME;
typedef LPOPENCARDNAMEA LPOPENCARDNAME;
#endif // UNICODE

// Backwards compatibility macros
#define OPENCARDNAME_A OPENCARDNAMEA
#define OPENCARDNAME_W OPENCARDNAMEW
#define POPENCARDNAME_A POPENCARDNAMEA
#define POPENCARDNAME_W POPENCARDNAMEW
#define LPOPENCARDNAME_A LPOPENCARDNAMEA
#define LPOPENCARDNAME_W LPOPENCARDNAMEW

extern WINSCARDAPI LONG WINAPI
GetOpenCardNameA(
    LPOPENCARDNAMEA);
extern WINSCARDAPI LONG WINAPI
GetOpenCardNameW(
    LPOPENCARDNAMEW);
#ifdef UNICODE
#define GetOpenCardName  GetOpenCardNameW
#else
#define GetOpenCardName  GetOpenCardNameA
#endif // !UNICODE

extern WINSCARDAPI LONG WINAPI
SCardDlgExtendedError (void);

#ifdef __cplusplus
}
#endif
#endif // _WINSCARD_H_


