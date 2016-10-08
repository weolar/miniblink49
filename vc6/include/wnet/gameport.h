/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    gameport.h

Abstract:

    This module contains the common public declarations for the game port
    enumerator.


Environment:

    kernel mode only

Notes:


Revision History:


--*/


#ifndef __GAMEPORT_H
#define __GAMEPORT_H

#define FILE_DEVICE_GAMEENUM         FILE_DEVICE_BUS_EXTENDER

// ***************************************************************************
// IOCTL interface to the bus (fdo)
//
// Clients use this to tell the enumerator what gaming devices on legacy ports
// exist.  (like for instance a control panel)
// ***************************************************************************

//
// Define an Interface Guid to access the game port enumerator
//

#undef FAR
#define FAR
#undef PHYSICAL_ADDRESS
#define PHYSICAL_ADDRESS LARGE_INTEGER

DEFINE_GUID (GUID_GAMEENUM_BUS_ENUMERATOR, 0xcae56030, 0x684a, 0x11d0, 0xd6, 0xf6, 0x00, 0xa0, 0xc9, 0x0f, 0x57, 0xda);
//  cae56030-684a-11d0-b6f6-00a0c90f57da

#define GAMEENUM_IOCTL(_index_) \
    CTL_CODE (FILE_DEVICE_GAMEENUM, _index_, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define GAMEENUM_INTERNAL_IOCTL(_index_) \
    CTL_CODE (FILE_DEVICE_GAMEENUM, _index_, METHOD_NEITHER, FILE_ANY_ACCESS)

#define IOCTL_GAMEENUM_EXPOSE_HARDWARE  GAMEENUM_IOCTL (0x0)
#define IOCTL_GAMEENUM_REMOVE_HARDWARE  GAMEENUM_IOCTL (0x1)
#define IOCTL_GAMEENUM_PORT_DESC        GAMEENUM_IOCTL (0x2)

//
//      Private data storage area for OEM devices. Values preserved if supplied to
// IOCTL_GAMEENUM_EXPOSE_HARDWARE and GAMEENUM_INTERNAL_IOCTL_EXPOSE_SIBLING,
//      and set to zero otherwise on initial mini-driver invocation (DriverEntry).
//

#define SIZE_GAMEENUM_OEM_DATA                  8
typedef ULONG   GAMEENUM_OEM_DATA[SIZE_GAMEENUM_OEM_DATA];

#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning(disable:4200)

typedef struct _GAMEENUM_EXPOSE_HARDWARE
{
    //
    // sizeof (struct _GAMEENUM_HARDWARE)
    //
    IN ULONG Size;

    //
    // The handle of the port found in the port desc
    //
    IN PVOID PortHandle;

    //
    // A handle to the exposed PDO
    //
    OUT PVOID HardwareHandle;

    //
    // For legacy joysticks only
    //
    IN USHORT NumberJoysticks;

    //
    // legacy joysticks only (joysticks * axis <= 4).
    //
    IN USHORT NumberAxis;

    //
    // Unique ID
    //
    IN USHORT UnitID;

    //
    // Number of buttons present on the device
    //
    IN USHORT NumberButtons;

    //
    // Bit flags controlling the behavior of the device
    //
    USHORT Flags;

    //
    // Reserved for future use
    //
    USHORT Reserved[5];

    //
    // Specific OEM Data
    //
    IN GAMEENUM_OEM_DATA OemData;

    //
    // An array of (zero terminated wide character strings). The array itself
    //  also null terminated (ie, MULTI_SZ)
    //
    IN  WCHAR                                   HardwareIDs[];

} GAMEENUM_EXPOSE_HARDWARE, *PGAMEENUM_EXPOSE_HARDWARE;


//
// Bit values defined for the Flags field
//     GAMEENUM_FLAG_NOCOMPATID the default compatibility hardware ID should 
//     not be exposed for this device.
//     GAMEENUM_FLAG_COMPATIDCTRL if this is zero GAMEENUM_FLAG_NOCOMPATID is 
//     ignored
//     GAMEENUM_FLAG_RESERVED reserved bits, should be set to zero
//
#define GAMEENUM_FLAG_NOCOMPATID    0x0001
#define GAMEENUM_FLAG_COMPATIDCTRL  0x0002
#define GAMEENUM_FLAG_RESERVED      0xFFFC


#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

typedef struct _GAMEENUM_REMOVE_HARDWARE
{
    //
    // sizeof (struct _REMOVE_HARDWARE)
    //
    IN ULONG Size;

    //
    // Same value as HardwareHandle in GAMEENUM_EXPOSE_HARDWARE
    //
    IN PVOID HardwareHandle;

} GAMEENUM_REMOVE_HARDWARE, *PGAMEENUM_REMOVE_HARDWARE;

typedef struct _GAMEENUM_PORT_DESC
{
    IN  ULONG               Size;       // sizeof (struct _PORT_DESC)
    OUT PVOID               PortHandle;
    OUT PHYSICAL_ADDRESS    PortAddress;
        ULONG               Reserved [5];
} GAMEENUM_PORT_DESC, *PGAMEENUM_PORT_DESC;

// **************************************************************************
// Internal IOCTL interface for (pdo)
// The HID to legacy game port minidriver uses this interface to
// find the address of the device.
// **************************************************************************

#define IOCTL_GAMEENUM_PORT_PARAMETERS          GAMEENUM_INTERNAL_IOCTL (0x100)
#define IOCTL_GAMEENUM_EXPOSE_SIBLING           GAMEENUM_INTERNAL_IOCTL (0x101)
#define IOCTL_GAMEENUM_REMOVE_SELF              GAMEENUM_INTERNAL_IOCTL (0x102)
#define IOCTL_GAMEENUM_ACQUIRE_ACCESSORS        GAMEENUM_INTERNAL_IOCTL (0x103)

// Of which IO_STACK_LOCATION->Parameters.Others.Argument1 is set to
// a pointer to struct _GAMEENUM_GAME_PARAMETERS

typedef
UCHAR
(*PGAMEENUM_READPORT) (
    PVOID  GameContext
    );

typedef
VOID
(*PGAMEENUM_WRITEPORT) (
    PVOID  GameContext,
    UCHAR   Value
    );

#define GAMEENUM_BUTTON_1   0x01
#define GAMEENUM_BUTTON_2   0x02
#define GAMEENUM_BUTTON_3   0x04
#define GAMEENUM_BUTTON_4   0x08

#define GAMEENUM_AXIS_X     0x10
#define GAMEENUM_AXIS_Y     0x20
#define GAMEENUM_AXIS_R     0x40
#define GAMEENUM_AXIS_Z     0x80

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif

typedef
NTSTATUS
(*PGAMEENUM_READPORT_DIGITAL) (
    IN     PVOID   Context,
    IN     UCHAR   ButtonAxisMask,
    IN     BOOLEAN Approximate,
    IN OUT ULONG   AxisState[4],
       OUT UCHAR   ButtonState[4]
    );
/*++
Routine Description.

    Will read from the gameport digitally.

Arguments:

    Context        -    value passed in GAME_PORT_PARAMETERS.GameContext

    ButtonAxisMask -    Mask indicating which axis  and buttons are expected to
                        have valid data

    Approximate    -    OK to approximate (if polling times out, etc)

    AxisState      -    IN  = Last valid axis state
                        OUT = Current Axis state

                        Index       Maps to Axis
                        0           X
                        1           Y
                        2           R
                        3           Z

    ButtonState    -    OUT =  Current button state.

--*/

typedef
NTSTATUS
(*PGAMEENUM_ACQUIRE_PORT) (
    PVOID  GameContext
    );

typedef
VOID
(*PGAMEENUM_RELEASE_PORT) (
    PVOID  GameContext
    );

typedef enum _GAMEENUM_PORTION
{
        GameenumFirstHalf,
        GameenumSecondHalf,
        GameenumWhole
} GAMEENUM_PORTION;

typedef struct _GAMEENUM_PORT_PARAMETERS
{
    //
    // sizeof (GAMEENUM_GET_PORT_PARAMETERS)
    //
    IN  ULONG Size;

    //
    // read the game port (analog)
    //
    OUT PGAMEENUM_READPORT ReadAccessor;

    //
    // write the game port (analog)
    //
    OUT PGAMEENUM_WRITEPORT WriteAccessor;

    //
    // token to read/write this game port
    //
    OUT PVOID GameContext;

    //
    // Which joystick is it?
    //
    OUT GAMEENUM_PORTION Portion;

    //
    // legacy joysticks only
    //
    OUT USHORT NumberAxis;

    //
    // unique id
    //
    IN USHORT UnitID;

    //
    // OEM specific data
    //
    IN GAMEENUM_OEM_DATA OemData;

    //
    // Number of buttons
    //
    OUT USHORT NumberButtons;

    //
    // Reserved for future use
    //
    USHORT Reserved2;

    //
    // Read the game port (digital)
    //
    OUT PGAMEENUM_READPORT_DIGITAL  ReadAccessorDigital;

    //
    // Function to call before reading/writing to the port
    //
    OUT PGAMEENUM_ACQUIRE_PORT AcquirePort;

    //
    // Function to call when done reading/writing to the port
    //
    OUT PGAMEENUM_RELEASE_PORT ReleasePort;

    //
    // Context to pass to AcquirePort and ReleasePort
    //
    OUT PVOID PortContext;

    ULONG Reserved[3];

} GAMEENUM_PORT_PARAMETERS, *PGAMEENUM_PORT_PARAMETERS;

typedef struct _GAMEENUM_EXPOSE_SIBLING
{
    //
    // sizeof (struct _GAMEENUM_EXPOSE_SIBLING)
    //
    IN ULONG Size;

    //
    // A handle to the exposed PDO
    //
    OUT PVOID HardwareHandle;

    //
    // OEM specific data
    //
    IN GAMEENUM_OEM_DATA OemData;

    //
    // The id of this device object
    //
    IN USHORT UnitID;

    USHORT Reserved[3];

    //
    // An array of (zero terminated wide character strings). The array itself
    //  also null terminated (ie, MULTI_SZ),
    //
    IN PWCHAR HardwareIDs OPTIONAL;

} GAMEENUM_EXPOSE_SIBLING, *PGAMEENUM_EXPOSE_SIBLING;

//
// This struct is sent down to the PDO/lower filters of gameenum via
// the internal IOCTL  IOCTL_GAMEENUM_ACQUIRE_ACCESSORS.  If this IOCTL is
// handled, GameContext, ReadAccessor, and WriteAccessor must be filled in.
// ReadAccessorDigital is optional
//
typedef struct _GAMEENUM_ACQUIRE_ACCESSORS
{
    //
    // sizeof (struct _GAMEENUM_ACQUIRE_ACCESSORS)
    //
    IN ULONG                        Size;

    //
    // token to read/write this game port
    //
    OUT PVOID                       GameContext;

    //
    // read the game port (analog)
    //
    OUT PGAMEENUM_READPORT          ReadAccessor;

    //
    // write the game port (analog)
    //
    OUT PGAMEENUM_WRITEPORT         WriteAccessor;

    //
    // Read the game port (digital)
    //
    OUT PGAMEENUM_READPORT_DIGITAL  ReadAccessorDigital;

    //
    // Function to call before reading/writing to the port
    //
    OUT PGAMEENUM_ACQUIRE_PORT AcquirePort;

    //
    // Function to call when done reading/writing to the port
    //
    OUT PGAMEENUM_RELEASE_PORT ReleasePort;

    //
    // Context to pass to AcquirePort and ReleasePort
    //
    OUT PVOID PortContext;

    OUT ULONG                       Reserved[3];

} GAMEENUM_ACQUIRE_ACCESSORS, *PGAMEENUM_ACQUIRE_ACCESSORS;

#endif



