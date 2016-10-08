/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntdd8042.h

Abstract:

    This is the include file that defines all constants and types for hooking
    i8042 devices.

Author:

    Doron J. Holan (doronh) 17-Dec-1997

Revision History:

--*/

#ifndef _NTDD8042_
#define _NTDD8042_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Internal IOCTLs used to find who is in the chain of notification
//
#define IOCTL_INTERNAL_I8042_HOOK_KEYBOARD  CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0FF0, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_I8042_HOOK_MOUSE     CTL_CODE(FILE_DEVICE_MOUSE, 0x0FF0, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// Internal IOCTLs used to write data to either device
//
// The IOCTL will be completed when the write has been completed.  If the write
// time out, the Cancel Routine of the Irp will be ignored and the Irp will still
// be completed successfully with a status of STATUS_IO_TIMEOUT instead of
// STATUS_SUCCESS
//
#define IOCTL_INTERNAL_I8042_KEYBOARD_WRITE_BUFFER   CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0FF1, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_I8042_MOUSE_WRITE_BUFFER      CTL_CODE(FILE_DEVICE_MOUSE, 0x0FF1, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// Can only be sent down the keyboard stack
//
#define IOCTL_INTERNAL_I8042_CONTROLLER_WRITE_BUFFER CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0FF2, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// The keyboard / mouse will send this down the stack so that the upper device
// filter has a device object to synch against.  
//
#define IOCTL_INTERNAL_I8042_KEYBOARD_START_INFORMATION   CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0FF3, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_INTERNAL_I8042_MOUSE_START_INFORMATION      CTL_CODE(FILE_DEVICE_MOUSE, 0x0FF3, METHOD_NEITHER, FILE_ANY_ACCESS)

//
// Valid bits for the PowerCapabilities REG_DWORD that can be put in the devnode
// indicating the presence of their respecitve power keys on the device
//
#define I8042_POWER_SYS_BUTTON           0x0001
#define I8042_SLEEP_SYS_BUTTON           0x0002
#define I8042_WAKE_SYS_BUTTON            0x0004
#define I8042_SYS_BUTTONS  (I8042_POWER_SYS_BUTTON | \
                            I8042_SLEEP_SYS_BUTTON | \
                            I8042_WAKE_SYS_BUTTON) 

//
// Synchronous reads and writes during kb initialization
//
#if 0
typedef enum _I8042_PORT_TYPE {
    PortTypeData = 0,
    PortTypeCommand
} I8042_PORT_TYPE;
#endif

typedef
NTSTATUS
(*PI8042_SYNCH_READ_PORT) (
    IN PVOID    Context,
    PUCHAR      Value,
    BOOLEAN     WaitForACK
    );

/*++
Routine Description:

    This routine sends a command or data byte to the keyboard
    in polling mode.  It waits for acknowledgment and resends
    the command/data if WaitForACK is true.

Arguments:

    Context - Function specific context

    PortType - If PortTypeCommand, send the byte to the command register,
        otherwise send it to the data register.

    WaitForACK - If true, wait for an ACK back from the hardware.

    AckDeviceType - Indicates which device we expect to get the ACK back
        from.

    Value - The byte to send to the hardware.

Return Value:

    STATUS_IO_TIMEOUT - The hardware was not ready for input or did not
    respond.

    STATUS_SUCCESS - The byte was successfully sent to the hardware.
  --*/
typedef
NTSTATUS
(*PI8042_SYNCH_WRITE_PORT) (
    IN PVOID    Context,
    UCHAR       Value,
    BOOLEAN     WaitForACK
    );

//
// These functions (PI8042_ISR_WRITE_PORT, PI8042_QUEUE_PACKET) are only valid
// when called with the context of the ISR hook
//
typedef
VOID
(*PI8042_ISR_WRITE_PORT) (
    IN PVOID        Context,
    IN UCHAR        Value
    );

//
// Call this function when an input packet has been fully formed and is ready to
// be queued to the class driver above of us in the stack
//
typedef
VOID
(*PI8042_QUEUE_PACKET) (
    IN PVOID        Context
    );

//
// Current state of writing to a device
//
typedef enum _TRANSMIT_STATE {
    Idle = 0,
    SendingBytes
} TRANSMIT_STATE;

//
// Current state of writing to the device.  If State != Idle, then a write is in
// progress
//
typedef struct _OUTPUT_PACKET {
    PUCHAR         Bytes;
    ULONG          CurrentByte;
    ULONG          ByteCount;
    TRANSMIT_STATE State;
} OUTPUT_PACKET, *POUTPUT_PACKET;

typedef enum _MOUSE_STATE {
    MouseIdle,              // expecting byte 1
    XMovement,              // expecting byte 2
    YMovement,              // expecting byte 3
    ZMovement,              // expecting byte 4 (if a wheel mouse)
    MouseExpectingACK,      // expecting ACK from Enable Mouse cmd
    MouseResetting          // reset substate
} MOUSE_STATE, *PMOUSE_STATE;

/*--
Normal reset process

Step                                    Transition into
Send Reset to mouse                     ExpectingReset (ie, 0xAA)
Got reset                               ExpectingResetId
Got reset id, send get device id        ExpectingGetDeviceIdACK
Got get dev id ack,                     ExpectingGetDeviceIdValue
Got dev id                              ExpectingSetResolutionACK
Got ACK for set res                     ExpectingSetResolutionValueAck
Got ACK for value                       ExpectingSetScaling1to1ACK
Got ACK for set scaling                 ExpectingSetScaling1to1ACK2
Got ACK for set scaling                 ExpectingSetScaling1to1ACK3

                                        EnableWheelDetection (via registry) ==
                                        0 send set sampling rate,
                                            ExpectingSetSamplingRateDefaultACK
                                        1 StartPnPIdDetection
                                        2 EnableWheel
StartPnPIdDetection, send series        ExpectingPnpIdByte1 after setting complete
 of set sampling rate commands
ExpectingPnpIdByte7                     compare pnp id to list, if valid,
                                        EnableWheel, else set def sampling rate
EnableWheel, send series of             send get device id after done,
 set sampling rate commands              ExpectingSetSamplingRateDefaultACK
got set sampling ack                    send sampling value
got sampling value ack                  mouse idle
 ++*/
typedef enum _MOUSE_RESET_SUBSTATE {
    ExpectingReset = 0,
    ExpectingResetId,                           /*  1 */
    ExpectingGetDeviceIdACK,                    /*  2 */
    ExpectingGetDeviceIdValue,                  /*  3 */

    ExpectingSetResolutionDefaultACK,           /*  4 */
    ExpectingSetResolutionDefaultValueACK,      /*  5 */

    ExpectingSetResolutionACK,                  /*  6 */
    ExpectingSetResolutionValueACK,             /*  7 */
    ExpectingSetScaling1to1ACK,                 /*  8 */
    ExpectingSetScaling1to1ACK2,                /*  9 */
    ExpectingSetScaling1to1ACK3,                /* 10 */
    ExpectingReadMouseStatusACK,                /* 11 */
    ExpectingReadMouseStatusByte1,              /* 12 */
    ExpectingReadMouseStatusByte2,              /* 13 */
    ExpectingReadMouseStatusByte3,              /* 14 */

    StartPnPIdDetection,                        /* 15 */

    ExpectingLoopSetSamplingRateACK,            /* 16 */
    ExpectingLoopSetSamplingRateValueACK,       /* 17 */

    ExpectingPnpIdByte1,                        /* 18 */
    ExpectingPnpIdByte2,                        /* 19 */
    ExpectingPnpIdByte3,                        /* 20 */
    ExpectingPnpIdByte4,                        /* 21 */
    ExpectingPnpIdByte5,                        /* 22 */
    ExpectingPnpIdByte6,                        /* 23 */
    ExpectingPnpIdByte7,                        /* 24 */

    EnableWheel,                                /* 25 */
    Enable5Buttons,                             /* 26 */

    ExpectingGetDeviceId2ACK,                   /* 27 */
    ExpectingGetDeviceId2Value,                 /* 28 */

    ExpectingSetSamplingRateACK,                /* 29 */
    ExpectingSetSamplingRateValueACK,           /* 30 */

    ExpectingEnableACK,                         /* 31 */

    ExpectingFinalResolutionACK,                /* 32 */
    ExpectingFinalResolutionValueACK,           /* 33 */

    ExpectingGetDeviceIdDetectACK,              /* 34 */
    ExpectingGetDeviceIdDetectValue,            /* 35 */

    CustomHookStateMinimum = 100,
    CustomHookStateMaximum = 999,

    I8042ReservedMinimum = 1000

} MOUSE_RESET_SUBSTATE, *PMOUSE_RESET_SUBSTATE;

/*--
IsrContext      -- user provided context
CurrentInput    -- current packet to being assembled
StatusByte      -- byte reported by the command port
Byte            -- byte reported by the mouse (ie, the data port)
MouseState      -- current state of the i8042prt ISR
ResetSubState   -- current reset sub state, only valid when
                   MouseState == MouseResetting (otherwise null)
ContinueProcessing -- if TRUE, the i8042prt ISR will continue executing after
                      calling the hook
  ++*/
typedef
BOOLEAN
(*PI8042_MOUSE_ISR) (
    PVOID                   IsrContext,
    PMOUSE_INPUT_DATA       CurrentInput,
    POUTPUT_PACKET          CurrentOutput,
    UCHAR                   StatusByte,
    PUCHAR                  Byte,
    PBOOLEAN                ContinueProcessing,
    PMOUSE_STATE            MouseState,
    PMOUSE_RESET_SUBSTATE   ResetSubState
);

typedef struct _INTERNAL_I8042_HOOK_MOUSE {

    //
    // Context variable for IsrRoutine, CancelRoutine
    //
    OUT PVOID Context;

    //
    // Routine to call when a byte is received via the interrupt
    //
    OUT PI8042_MOUSE_ISR IsrRoutine;

    //
    // Write function, will automatically write to the command port saying the
    // next byte is directed towards the auxilliary device.
    //
    // NB:  May only called within the context of the IsrRoutine provided above
    //      The mouse's response to the write will passed to the IsrRoutine when
    //      it is received (ie, if 0xF4 (enable) was written using this function,
    //      an 0xFA (ACK) will be passed to IsrRoutine if the enable was
    //      successful)
    //
    IN PI8042_ISR_WRITE_PORT IsrWritePort;

    //
    // Queue the current packet (ie the one passed into the isr callback hook)
    // to be reported to the class driver
    //
    IN PI8042_QUEUE_PACKET QueueMousePacket;

    //
    // Context for IsrWritePort, QueueMousePacket
    //
    IN PVOID CallContext;

} INTERNAL_I8042_HOOK_MOUSE, *PINTERNAL_I8042_HOOK_MOUSE;

//
// Define the keyboard scan code input states.
//
typedef enum _KEYBOARD_SCAN_STATE {
    Normal,
    GotE0,
    GotE1
} KEYBOARD_SCAN_STATE, *PKEYBOARD_SCAN_STATE;

typedef
NTSTATUS
(*PI8042_KEYBOARD_INITIALIZATION_ROUTINE) (
    IN PVOID                           InitializationContext,
    IN PVOID                           SynchFuncContext,
    IN PI8042_SYNCH_READ_PORT          ReadPort,
    IN PI8042_SYNCH_WRITE_PORT         WritePort,
    OUT PBOOLEAN                       TurnTranslationOn
    );

typedef
BOOLEAN
(*PI8042_KEYBOARD_ISR) (
    PVOID                   IsrContext,
    PKEYBOARD_INPUT_DATA    CurrentInput,
    POUTPUT_PACKET          CurrentOutput,
    UCHAR                   StatusByte,
    PUCHAR                  Byte,
    PBOOLEAN                ContinueProcessing,
    PKEYBOARD_SCAN_STATE    ScanState
    );

typedef struct _INTERNAL_I8042_HOOK_KEYBOARD {

    //
    // Context variable for all callback routines
    //
    OUT PVOID Context;

    //
    // Routine to call after the mouse is reset
    //
    OUT PI8042_KEYBOARD_INITIALIZATION_ROUTINE InitializationRoutine;

    //
    // Routine to call when a byte is received via the interrupt
    //
    OUT PI8042_KEYBOARD_ISR IsrRoutine;

    //
    // Write function
    //
    IN PI8042_ISR_WRITE_PORT IsrWritePort;

    //
    // Queue the current packet (ie the one passed into the isr callback hook)
    // to be reported to the class driver
    //
    IN PI8042_QUEUE_PACKET QueueKeyboardPacket;

    //
    // Context for IsrWritePort, QueueKeyboardPacket
    //
    IN PVOID CallContext;

} INTERNAL_I8042_HOOK_KEYBOARD, *PINTERNAL_I8042_HOOK_KEYBOARD;

typedef struct _INTERNAL_I8042_START_INFORMATION {
    //
    // Size of this structure
    //
    ULONG Size;

    //
    // Interrupt object for the device.  Should only be used for calls to 
    // KeSynchronizeExecution
    //
    PKINTERRUPT InterruptObject;
    
    //
    // Future use
    //
    ULONG Reserved[8];

} INTERNAL_I8042_START_INFORMATION, *PINTERNAL_I8042_START_INFORMATION;

#ifdef __cplusplus
}
#endif

#endif // _NTDD8042_



