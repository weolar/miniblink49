/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ddvdeo.h

Abstract:

    This is the include file that defines all constants and types for
    accessing the Video device.

Author:

    Andre Vachon (andreva) 21-Jan-1992

Revision History:

--*/

#ifndef _NTDDVDEO_
#define _NTDDVDEO_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <tvout.h>

//
// VideoIoControlFile InputBuffer/OutputBuffer record structures for
// this device.
//

//
// Name used to create the miniport logical device names
//

#define VIDEO_DEVICE_NAME "DISPLAY%d"
#define WVIDEO_DEVICE_NAME L"DISPLAY%d"

//
// Display output interfaces
//

// DEFINE_GUID(GUID_DISPLAY_OUTPUT_INTERFACE_STANDARD,  0x96304D9F, 0x54b5, 0x11d1, 0x8b, 0x0f, 0x00, 0xa0, 0xc9, 0x06, 0x8f, 0xf3);

//
// Display adapter device interface
// 5b45201d-f2f2-4f3b-85bb-30ff1f953599
//

DEFINE_GUID(GUID_DEVINTERFACE_DISPLAY_ADAPTER, 0x5b45201d, 0xf2f2, 0x4f3b, 0x85, 0xbb, 0x30, 0xff, 0x1f, 0x95, 0x35, 0x99);

//
// Obsolete device interface class GUID names.
// (use of above GUID_DEVINTERFACE_* names is recommended).
//

#define GUID_DISPLAY_ADAPTER_INTERFACE  GUID_DEVINTERFACE_DISPLAY_ADAPTER


//
// The first set of IOCTLs are handle by the port driver and never seen
// by the miniport.
//

#define IOCTL_VIDEO_ENABLE_VDM \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x00, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_DISABLE_VDM \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_REGISTER_VDM \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_OUTPUT_DEVICE_POWER_STATE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x03, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_OUTPUT_DEVICE_POWER_STATE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x04, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MONITOR_DEVICE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x05, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENUM_MONITOR_PDO \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x06, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_INIT_WIN32K_CALLBACKS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x07, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_HANDLE_VIDEOPARAMETERS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x08, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_IS_VGA_DEVICE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x09, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_USE_DEVICE_IN_SESSION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x0a, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_PREPARE_FOR_EARECOVERY \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x0b, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// All these IOCTL's must be both handled by the port and miniport since
// they require processing by both parties.
//
#define IOCTL_VIDEO_SAVE_HARDWARE_STATE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x80, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_RESTORE_HARDWARE_STATE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x81, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// All these IOCTL's are public and must/can be handled by the miniport
// driver
//

#define IOCTL_VIDEO_QUERY_AVAIL_MODES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x100, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x101, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_CURRENT_MODE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x102, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_CURRENT_MODE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x103, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_RESET_DEVICE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x104, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_LOAD_AND_SET_FONT \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x105, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_PALETTE_REGISTERS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x106, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_COLOR_REGISTERS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x107, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENABLE_CURSOR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x108, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_DISABLE_CURSOR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x109, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_CURSOR_ATTR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10a, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_CURSOR_ATTR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_CURSOR_POSITION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10c, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_CURSOR_POSITION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10d, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_ENABLE_POINTER \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_DISABLE_POINTER \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x10f, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_POINTER_ATTR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x110, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_POINTER_ATTR \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x111, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_SET_POINTER_POSITION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x112, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_POINTER_POSITION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x113, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_POINTER_CAPABILITIES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x114, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_BANK_SELECT_CODE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x115, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_MAP_VIDEO_MEMORY \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x116, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_UNMAP_VIDEO_MEMORY \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x117, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x118, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x119, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_QUERY_COLOR_CAPABILITIES \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11a, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// New IOCTLs defined for product 1.0A
//

#define IOCTL_VIDEO_SET_POWER_MANAGEMENT \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11b, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_GET_POWER_MANAGEMENT \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11c, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SHARE_VIDEO_MEMORY \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11d, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VIDEO_UNSHARE_VIDEO_MEMORY \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11e, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_COLOR_LUT_DATA \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x11f, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_GET_CHILD_STATE \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x120, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_VALIDATE_CHILD_STATE_CONFIGURATION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x121, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_CHILD_STATE_CONFIGURATION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x122, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SWITCH_DUALVIEW \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x123, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_BANK_POSITION \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x124, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x125, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x126, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS \
    CTL_CODE(FILE_DEVICE_VIDEO, 0x127, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// All these IOCTL's are handled by the Far East Full Screen Video driver
//

#define IOCTL_FSVIDEO_COPY_FRAME_BUFFER \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x200, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FSVIDEO_WRITE_TO_FRAME_BUFFER \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x201, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FSVIDEO_REVERSE_MOUSE_POINTER \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x202, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FSVIDEO_SET_CURRENT_MODE \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x203, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FSVIDEO_SET_SCREEN_INFORMATION \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x204, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_FSVIDEO_SET_CURSOR_POSITION \
    CTL_CODE(FILE_DEVICE_FULLSCREEN_VIDEO, 0x205, METHOD_BUFFERED, FILE_ANY_ACCESS)



//
// Many of the video ICOTLs are modal. When ever the palette is set, or the
// cursor is set or queried, it is done for the current mode.
//
// Modal specifies that the operation is only valid within a mode. Once a
// set mode operation is performed, the state associated to the modal IOCTL
// has been destroyed or reinitialized.
// Non-modal IOCTLs have their state preserved across set-mode operations.
//
// Optional IOCTLs are IOCTLs the miniport can optionally support. If the
// miniport does not support the IOCTL, it should return the appropriate
// error status.
// Required IOCTLs must be implemented in a miniport in order for the system
// to system properly.
//
// IOCTL_VIDEO_ENABLE_VDM                       Non-Modal    Private(1)
// IOCTL_VIDEO_DISABLE_VDM                      Non-Modal    Private(1)
// IOCTL_VIDEO_REGISTER_VDM                     Non-Modal    Private(1)
//
// IOCTL_VIDEO_SAVE_HARDWARE_STATE              Non-Modal    Required(2)
// IOCTL_VIDEO_RESTORE_HARDWARE_STATE           Non-Modal    Required(2)
//
// IOCTL_VIDEO_QUERY_AVAIL_MODES                Non-Modal    Required
// IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES            Non-Modal    Required
// IOCTL_VIDEO_QUERY_CURRENT_MODE               Modal        Required
// IOCTL_VIDEO_SET_CURRENT_MODE                 Non-Modal    Required
// IOCTL_VIDEO_RESET_DEVICE                     Non-Modal    Required
//
// IOCTL_VIDEO_LOAD_AND_SET_FONT                Modal        Required(2)
//
// IOCTL_VIDEO_SET_PALETTE_REGISTERS            Modal        Required(2)
// IOCTL_VIDEO_SET_COLOR_REGISTERS              Modal        Required(3)
//
// IOCTL_VIDEO_ENABLE_CURSOR                    Modal        Required(2)
// IOCTL_VIDEO_DISABLE_CURSOR                   Modal        Required(2)
// IOCTL_VIDEO_SET_CURSOR_ATTR                  Modal        Required(2)
// IOCTL_VIDEO_QUERY_CURSOR_ATTR                Modal        Required(2)
// IOCTL_VIDEO_SET_CURSOR_POSITION              Modal        Required(2)
// IOCTL_VIDEO_QUERY_CURSOR_POSITION            Modal        Required(2)
//
// IOCTL_VIDEO_ENABLE_POINTER                   Modal        Optional
// IOCTL_VIDEO_DISABLE_POINTER                  Modal        Optional
// IOCTL_VIDEO_SET_POINTER_ATTR                 Modal        Optional
// IOCTL_VIDEO_QUERY_POINTER_ATTR               Modal        Optional
// IOCTL_VIDEO_SET_POINTER_POSITION             Modal        Optional
// IOCTL_VIDEO_QUERY_POINTER_POSITION           Modal        Optional
// IOCTL_VIDEO_QUERY_POINTER_CAPABILITIES       Non-Modal    Optional
//
// IOCTL_VIDEO_GET_BANK_SELECT_CODE             Modal        Required(2)
//
// IOCTL_VIDEO_MAP_VIDEO_MEMORY                 Special(4)   Required
// IOCTL_VIDEO_UNMAP_VIDEO_MEMORY               Non-Modal    Required
// IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES       Non-Modal    Optional
// IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES        Non-Modal    Optional
//
// IOCTL_VIDEO_QUERY_COLOR_CAPABILITIES         Non-Modal    Optional
//
// IOCTL_VIDEO_SET_POWER_MANAGEMENT             Non-Modal    Optional
// IOCTL_VIDEO_GET_POWER_MANAGEMENT             Non-Modal    Optional
//
// IOCTL_VIDEO_SET_COLOR_LUT_DATA               Modal        Optional

//
// (1) Private means the IOCTL is completely implemeted within the port driver
//     and the miniport does not need to support it.
//
// (2) These Required functions are for "Vga Compatible" miniports. They are
//     Optional for other, non vga-compatible (i.e frame buffers) drivers.
//     VGA compatible means here that the miniport implements all the VGA
//     functionality and that the VgaCompatible flag for the miniport in the
//     registry parameters is turned on.
//
// (3) This IOCTL is required if the device has a color lookup table (also
//     commonly called palette) the PALETTE IOCTL is used for VGA while the
//     COLOR IOCTL is the more general IOCTL that is called by the display
//     driver or application to set the colors in the devices internal
//     lookup table
//
// (4) This IOCTL is both modal and non-modal. It should map all of video
//     memory in the caller's address space. A set mode MUST NOT cause the
//     video memory to change location - in this sense it is non-modal.
//     However, this IOCTL returns the location size of the frame buffer within
//     video memory, and the frame buffer size and location may vary from mode
//     to mode - so that information is modal.
//


//
// Any IOCTL that returns information should return in the status block the
// size of the data returned.
// If the output buffer was too small, an error should be returned.
//
//
//
//



//
// IOCTL_VIDEO_ENABLE_VDM
// IOCTL_VIDEO_DISABLE_VDM
// IOCTL_VIDEO_REGISTER_VDM
//
// These IOCTLs are used to enable or disable a VDM's access to the video
// hardware. This call will cause the real video frame buffer to be mapped
// into the VDM's address space and get the video validator connected to the
// V86 emulator for direct video register access.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_VDM {
    HANDLE ProcessHandle;
} VIDEO_VDM, *PVIDEO_VDM;

//
//ProcessHandle - Handle to the process for which this request must be
//    performed. This is required because the console calls the miniport on
//    the behalf of the VDM process; we are not performing this request in
//    the context of the current caller.
//


typedef struct _VIDEO_REGISTER_VDM {
    ULONG MinimumStateSize;
} VIDEO_REGISTER_VDM, *PVIDEO_REGISTER_VDM;

//
//MinimumStateSize - Output value determining the minimum size required to
//    store the video hardware state when performing SAVE_HARDWARE_SATE or
//    RESTORE_HARDWARE_STATE Ioctls.
//


//
// IOCTL_VIDEO_ENUM_MONITOR_PDO
//
// Descriptor of monitor devices
//

typedef struct tagVIDEO_MONITOR_DEVICE {
    ULONG   flag;       // The monitor is ON/OFF
    HANDLE  pdo;        // Monitor device handle
    ULONG   HwID;
} VIDEO_MONITOR_DEVICE, *PVIDEO_MONITOR_DEVICE;


//
// IOCTL_VIDEO_INIT_WIN32K_CALLBACKS
//
// List of function pointers used to make callbacks to win32k
//

typedef enum _VIDEO_WIN32K_CALLBACKS_PARAMS_TYPE {
    VideoPowerNotifyCallout = 1,
    VideoDisplaySwitchCallout,
    VideoEnumChildPdoNotifyCallout,
    VideoFindAdapterCallout,
    VideoWakeupCallout,
    VideoChangeDisplaySettingsCallout
} VIDEO_WIN32K_CALLBACKS_PARAMS_TYPE;


typedef struct _VIDEO_WIN32K_CALLBACKS_PARAMS {
    VIDEO_WIN32K_CALLBACKS_PARAMS_TYPE CalloutType;
    PVOID   PhysDisp;
    ULONG_PTR Param;
    LONG    Status;
} VIDEO_WIN32K_CALLBACKS_PARAMS, *PVIDEO_WIN32K_CALLBACKS_PARAMS;


typedef
VOID
(*PVIDEO_WIN32K_CALLOUT) (
    IN PVOID Params
    );


typedef struct _VIDEO_WIN32K_CALLBACKS {
    PVOID                 PhysDisp;
    PVIDEO_WIN32K_CALLOUT Callout;
    ULONG                 bACPI;
    HANDLE                pPhysDeviceObject;
    ULONG                 DualviewFlags;
} VIDEO_WIN32K_CALLBACKS, *PVIDEO_WIN32K_CALLBACKS;


//
// IOCTL_VIDEO_USE_DEVICE_IN_SESSION
//
// Parameters to request new enabled/disabled state for a device
//

typedef struct _VIDEO_DEVICE_SESSION_STATUS {
    ULONG   bEnable;	// Is device being enabled or disabled
    ULONG   bSuccess;   // Was request validated
} VIDEO_DEVICE_SESSION_STATUS, *PVIDEO_DEVICE_SESSION_STATUS;


//
// defines for Dualview Flags
//

#define VIDEO_DUALVIEW_REMOVABLE           0x00000001
#define VIDEO_DUALVIEW_PRIMARY             0x80000000
#define VIDEO_DUALVIEW_SECONDARY           0x40000000


//
// defines for child state flag
//

#define VIDEO_CHILD_ACTIVE                 0x00000001
#define VIDEO_CHILD_DETACHED               0x00000002
#define VIDEO_CHILD_NOPRUNE_FREQ           0x80000000
#define VIDEO_CHILD_NOPRUNE_RESOLUTION     0x40000000


//
// Second set of structures
//

//
// These IOCTLs are used by the VDM and the console to communicate state
// changes between the VDM and the kernel video driver.
//
// IOCTL_VIDEO_SAVE_HARDWARE_STATE -
// IOCTL_VIDEO_RESTORE_HARDWARE_STATE -
//
//
// This structure is at the start of the block used when saving or restoring
// the state of the video hardware using ConsoleHardwareState().
// the ULONG are offset to the location of the rest of the data. That data
// is stored within the same memory block pointed to by the
// VIDEO_HARDWARE_STATE structure, right after this header.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_HARDWARE_STATE_HEADER {
    ULONG Length;
    UCHAR PortValue[0x30];
    ULONG AttribIndexDataState;
    ULONG BasicSequencerOffset;
    ULONG BasicCrtContOffset;
    ULONG BasicGraphContOffset;
    ULONG BasicAttribContOffset;
    ULONG BasicDacOffset;
    ULONG BasicLatchesOffset;
    ULONG ExtendedSequencerOffset;
    ULONG ExtendedCrtContOffset;
    ULONG ExtendedGraphContOffset;
    ULONG ExtendedAttribContOffset;
    ULONG ExtendedDacOffset;
    ULONG ExtendedValidatorStateOffset;
    ULONG ExtendedMiscDataOffset;
    ULONG PlaneLength;
    ULONG Plane1Offset;
    ULONG Plane2Offset;
    ULONG Plane3Offset;
    ULONG Plane4Offset;
    ULONG VGAStateFlags;
    ULONG DIBOffset;
    ULONG DIBBitsPerPixel;
    ULONG DIBXResolution;
    ULONG DIBYResolution;
    ULONG DIBXlatOffset;
    ULONG DIBXlatLength;
    ULONG VesaInfoOffset;
    PVOID FrameBufferData;

} VIDEO_HARDWARE_STATE_HEADER, *PVIDEO_HARDWARE_STATE_HEADER;

//
// defines for VGAStateFlags
//

#define VIDEO_STATE_NON_STANDARD_VGA       0x00000001
#define VIDEO_STATE_UNEMULATED_VGA_STATE   0x00000002
#define VIDEO_STATE_PACKED_CHAIN4_MODE     0x00000004

typedef struct _VIDEO_HARDWARE_STATE {
    PVIDEO_HARDWARE_STATE_HEADER StateHeader;
    ULONG StateLength;
} VIDEO_HARDWARE_STATE, *PVIDEO_HARDWARE_STATE;

//
//Length - Length of the basic structure. Used for versioning purposes. The
//    length field should be initialized to be equal to
//    sizeof(VIDEO_HARDWARE_STATE_HEADER).
//
//PortValue - Array of entries containing the data values for port 3B0 through
//    3DF.
//
//AttribIndexDataState - State of the attribute index register.
//
//BasicSequencerOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    sequencer register set of the VGA.
//
//BasicCrtContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    CRT register set of the VGA.
//
//BasicGraphContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    graphics controller register set of the VGA.
//
//BasicAttribContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    attribute controller register set of the VGA.
//
//BasicDacOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    DAC registers of the VGA.
//
//BasicLatchesOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the register values for the basic
//    latches of the VGA.
//
//ExtendedSequencerOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the registers values for the extended
//    sequencer register set of the VGA.
//
//ExtendedCrtContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the registers values for the extended
//    CRT register set of the VGA.
//
//ExtendedGraphContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the registers values for the extended
//    graphics controller register set of the VGA.
//
//ExtendedAttribContOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the registers values for the extended
//    attribute controller register set of the VGA.
//
//ExtendedDacOffset - Offset, in bytes, from the beginning of the structure,
//    to an array of fields containing the registers values for the extended
//    DAC registers of the VGA.
//
//ExtendedValidatorStateOffset - Offset, in bytes, from the beginning of the
//    structure, to an area reserved for the miniport to put the unemulated
//    save state that the miniport uses to perform instruction validation for
//    DOS apps.
//
//ExtendedMiscDataOffset - Offset, in bytes, from the beginning of the structure,
//    to an area reserved for the use of the miniport.
//
//PlaneLength - Length of each of the following plane (if present)
//
//Plane1Offset - Offset, in bytes, from the beginning of the structure, to an
//    array of fields containing the data of the first plane of video memory.
//
//Plane2Offset - Offset, in bytes, from the beginning of the structure, to an
//    array of fields containing the data of the second plane of video memory.
//
//Plane3Offset - Offset, in bytes, from the beginning of the structure, to an
//    array of fields containing the data of the third plane of video memory.
//
//Plane4Offset - Offset, in bytes, from the beginning of the structure, to an
//    array of fields containing the data of the fourth plane of video memory.
//
//VGAStateFlags - Flags used for the interpretation of the VGA state.
//    VIDEO_STATE_NON_STANDARD_VGA is set when the set of registers the VGA
//        returns is not the basic set (all super vga's are not standard).
//        The VDM should not emulate the saved state unless a specific VDD
//        has been written for the device.
//    VIDEO_STATE_UNEMULATED_VGA_STATE specified the miniport has stored
//        informaiton in the ExtendedValidatorState field and the miniport
//        should treat this as a frozen state, whatever the registers say.
//    VIDEO_STATE_PACKED_CHAIN4_MODE indicates that in mode 13 (320x200x256).
//        the data is stored in a packed pixel format in the plane, as
//        opposed to the standard VGA format where the data is interleaved
//        at every four bytes, and on every 16K boundary, offset by one
//        extra byte.
//
//DIBOffset - Offset to the location of the DIB in the allocated data
//        structure. If NULL, no translation is available.
//
//DIBBitsPerPixel - Format of the DIB.
//
//DIBXResolution - Width of the DIB in pixels.
//
//DIBYResolution - Height of the DIB in pixels.
//
//DIBXlatOffset - Offset to the location of the translation vector
//    from DIB pixel values to 32-bit RGB (1 byte red, 1 byte green, 1 byte
//    blue, 1 byte empty). Maximum length 256. If NULL, the standard
//    VGA palette stored in this structure should be used.
//
//DIBXlatLength - Length of the RGB translation vector at DIBXlatOffset.
//
// For each of the offset fields, if an offset value is NULL, then there is
// no data for that offset.
// The length of a data area is:
//   1) the specific length given to it : plane length (planes) or XResolution *
//        Yresolution * BitsPerPel (DIB)
//   2) otherwise, the length = next_non-null_offset_value -
//                                   current_offset_value
//

//
//StateHeader - Pointer to the VIDEO_HARDWARE_STATE_HEADER structure.
//
//StateLength - Size of the VIDEO_HARDWARE_STATE_HEADER structure.
//

//
// IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES - Returns number of different modes
//                                     available on the controller.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_NUM_MODES {
    ULONG NumModes;
    ULONG ModeInformationLength;
} VIDEO_NUM_MODES, *PVIDEO_NUM_MODES;

//
//NumModes - Returns the number of modes supported by the kernel driver.
//
//ModeInformationLength - Length of the VIDEO_MODE_INFORMATION structure
//    for the IOCTL_VIDEO QUERY_AVAILABLE_MODES IOCTL.


//
// IOCTL_VIDEO_SET_CURRENT_MODE - Is used to set the mode of the controller.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_MODE {
    ULONG RequestedMode;
} VIDEO_MODE, *PVIDEO_MODE;

#define VIDEO_MODE_NO_ZERO_MEMORY 0x80000000 // High order bit of the mode
                                             // determines if the set mode
                                             // should (0) or should not (1)
                                             // cause the video memory to be
                                             // zeroed out simultaneously to
                                             // the set mode operation.

#define VIDEO_MODE_MAP_MEM_LINEAR 0x40000000 // Miniports which support this
                                             // flag will set a linear mode
                                             // if possible when this flag
                                             // is set.  Note: Some miniports
                                             // may return a linear mode even
                                             // if this flag is not set.


//
//RequestedMode - Indicates in which mode the adapter should be initialized.
//


//
// IOCTL_VIDEO_RESET_DEVICE - Is used to reset the mode of the adapter when GDI
//                            gives up control of the device to allow a VDM to
//                            access the hardware. x86 only.
//                            The default mode should be whatever is the
//                            default mode when the machine is booted
//
// No information is needed fo this function.
//



//
// IOCTL_VIDEO_QUERY_AVAIL_MODES - Returns information about each available
//                                 mode on the controller.
//
// IOCTL_VIDEO_QUERY_CURRENT_MODE - Returns the information for the current
//                                  controller mode.
//
// Information used by this function is passed using the following structure:
//
// NOTE This structure is matched exactly with the DISP_MODE structure
// in winddi.h - every change to this structure MUST be made to the
// structure in winddi.h.
//

typedef struct _VIDEO_MODE_INFORMATION {
    ULONG Length;
    ULONG ModeIndex;
    ULONG VisScreenWidth;
    ULONG VisScreenHeight;
    ULONG ScreenStride;
    ULONG NumberOfPlanes;
    ULONG BitsPerPlane;
    ULONG Frequency;
    ULONG XMillimeter;
    ULONG YMillimeter;
    ULONG NumberRedBits;
    ULONG NumberGreenBits;
    ULONG NumberBlueBits;
    ULONG RedMask;
    ULONG GreenMask;
    ULONG BlueMask;
    ULONG AttributeFlags;
    ULONG VideoMemoryBitmapWidth;
    ULONG VideoMemoryBitmapHeight;
    ULONG DriverSpecificAttributeFlags;
} VIDEO_MODE_INFORMATION, *PVIDEO_MODE_INFORMATION;

//
// Bit definitions for Attribute Flags
//

#define VIDEO_MODE_COLOR            0x0001  // 0 = Mono-compatible, 1 = Color
#define VIDEO_MODE_GRAPHICS         0x0002  // 0 = Text mode, 1 = Graphics
#define VIDEO_MODE_PALETTE_DRIVEN   0x0004  // 0 = Colors are direct
                                            // 1 = Colors are index to a palette
#define VIDEO_MODE_MANAGED_PALETTE  0x0008  // 0 = Palette is fixed (must be
                                            //     queried from miniport
                                            // 1 = Palette is settable.
#define VIDEO_MODE_INTERLACED       0x0010  // 1 = Mode is interlaced
                                            // 0 = non-interlaced
#define VIDEO_MODE_NO_OFF_SCREEN    0x0020  // 1 = Offscreen memory CAN NOT be
                                            //     used to store information.
                                            // 0 = Offscreen memory is available
#define VIDEO_MODE_NO_64_BIT_ACCESS 0x0040  // 1 = 64 bit memory writes to frame
                                            //     buffer are not handled properly.
                                            // 0 = 64 bit memory writes to frame
                                            //     buffer are handled properly.
#define VIDEO_MODE_BANKED           0x0080  // 0 = undefined
                                            // 1 = this is a banked mode
#define VIDEO_MODE_LINEAR           0x0100  // 0 = undefined
                                            // 1 = this is a linear mode

//
//Length - Length of the structure in bytes. Also used to do verisioning.
//
//ModeIndex - Number used to set this mode when calling the miniport driver.
//
//VisScreenWidth - Number of visible horizontal pixels on a scan line
//
//VisScreenHeight - Number of visible lines (or scan lines)
//
//ScreenStride - Delta, in *BYTES*, between the start of two scan lines.
//
//    NOTE: the width and height are in pixels, but the stride is in bytes !!!
//
//NumberOfPlanes - Number of separate planes combined by the device.
//
//BitsPerPlane - Number of bits per pixel on a plane.
//
//Frequency - Screen Frequency, in Hertz.
//
//XMillimeter - Size of the horizontal active region on the output device,
//    in millimeters.
//
//YMillimeter - Size of the vertical active region on the output device,
//    in millimeters.
//
//NumberRedBits - Number of bits in the red DAC.
//
//NumberGreenBits - Number of bits in the green DAC.
//
//NumberBlueBits - Number of bits in the blue DAC.
//
//RedMask - Red color Mask for device with direct color modes. Bits turned
//    on indicate the bit is of color Red.
//
//GreenMask - Green color Mask for device with direct color modes. Bits
//    turned on indicate the bit is of color Green.
//
//BlueMask - Blue color Mask for device with direct color modes. Bits
//    turned on indicate the bit is of color Blue.
//
//AttributeFlags. Flags indicating certain behavior for the device.
//
//VideoMemoryBitmapWidth - Width of the video memory bitmap.
//    VisScreenWidth <= VideoMemoryBitmapWidth <= ScreenStride
//
//VideoMemoryBitmapHeight - Height of the video memory bitmap.
//   VisScreenHeight <= VideoMemoryBitmapHeight = VideoRamLength / ScreenStride
//
//DriverSpecificAttributeFlags - Flags indicating certain behavior for the
//   device that are private to the miniport\display driver.
//


//
// IOCTL_VIDEO_LOAD_AND_SET_FONT - Is used to load a user-defined font.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_LOAD_FONT_INFORMATION {
    USHORT WidthInPixels;
    USHORT HeightInPixels;
    ULONG FontSize;
    UCHAR Font[1];
} VIDEO_LOAD_FONT_INFORMATION, *PVIDEO_LOAD_FONT_INFORMATION;

//
//WidthInPixels - Width of the characters in the font, in pixels.
//
//HeigthInPixels - Heigth of the characters in the font, in pixels.
//
//FontSize - Size of the font buffer being passed in, in bytes.
//
//Font - Start of the font buffer.
//


//
// IOCTL_VIDEO_SET_PALETTE_REGISTERS - Takes buffer containing
//                                     VIDEO_PALETTE_DATA where Colors[]
//                                     specifies the array containing the
//                                     color values for the palette registers.
//
// Information used by this function is passed using the following structure:
//
// NOTE: This should only be used by the VGA type drivers
//

typedef struct _VIDEO_PALETTE_DATA {
    USHORT NumEntries;
    USHORT FirstEntry;
    USHORT Colors[1];
} VIDEO_PALETTE_DATA, *PVIDEO_PALETTE_DATA;

//
//NumEntries - Number of entries in the array of color values.
//
//FirstEntry - Location in the device palette to which the first entry in the
//    list of colors should be copied to. The other entries in the color list
//    should be copied sequentially, from this starting point into the device's
//    palette.
//
//Colors - Array of color entries to copy into the device's color palette.
//

//
// IOCTL_VIDEO_SET_COLOR_REGISTERS - Takes buffer containing VIDEO_CLUT.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_CLUTDATA {
    UCHAR Red;
    UCHAR Green;
    UCHAR Blue;
    UCHAR Unused;
} VIDEO_CLUTDATA, *PVIDEO_CLUTDATA;

//
//Red - Bits to be put in the Red portion of the color registers.
//
//Green - Bits to be put in the Green portion of the color registers.
//
//Blue - Bits to be put in the Blue portion of the color registers.
//

typedef struct {
    USHORT   NumEntries;
    USHORT   FirstEntry;
    union {
        VIDEO_CLUTDATA RgbArray;
        ULONG RgbLong;
    } LookupTable[1];
} VIDEO_CLUT, *PVIDEO_CLUT;

//
//NumEntries - Number of entries in the LookupTable of color values.
//
//FirstEntry - Location in the device palette to which the first entry in the
//    LookupTable of colors should be copied to. The other entries in the
//    LookupTable should be copied sequentially, from this starting point into
//    the device's palette.
//
//LookupTable - Array of color entries to copy into the device's color
//    registers/palette. The color entries can be accessed as a genric 32 bit
//    value or as Red/Green/Blue/Unused fields.
//

//
// NOTE: Cursor vs. Pointer:
//    A cursor is a rectangular set of pixels which are used to indicate the
//    location of input coming from the keyboard.
//
//    A pointer is the set of pixels that are used to paint the shape
//    associated with the mouse.
//

//
// IOCTL_VIDEO_QUERY_CURSOR_POSITION - Returns the location of the cursor on
//                                     the screen.
//
// IOCTL_VIDEO_SET_CURSOR_POSITION - Is used to set the location of the
//                                   cursor on the screen.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_CURSOR_POSITION {
    SHORT Column;
    SHORT Row;
} VIDEO_CURSOR_POSITION, *PVIDEO_CURSOR_POSITION;

//
//Column - Column on which the cursor is located from the top left, in pixels.
//
//Row - Row on which the cusor is located from the top left, in pixels.
//


//
// IOCTL_VIDEO_QUERY_CURSOR_ATTR - Returns all attributes of the cursor.
//
// IOCTL_VIDEO_SET_CURSOR_ATTR - Is used to set the attributes of the cursor.
//
// Information used by this function is passed using the following structure:
//

//
// For the VGA:
// TopScanLine will be stored in the height when an IOCTL is made
// BottomScanLine will be stored in the width when an IOCTL is made
//

typedef struct _VIDEO_CURSOR_ATTRIBUTES {
    USHORT Width;
    USHORT Height;
    SHORT Column;
    SHORT Row;
    UCHAR Rate;
    UCHAR Enable;
} VIDEO_CURSOR_ATTRIBUTES, *PVIDEO_CURSOR_ATTRIBUTES;

//
//Width - Width of the cursor, in pixels.
//
//Height - Height of the cursor, in scans.
//
//Column - Column on which the cursor is located from the top left, in pixels.
//
//Row - Row on which the cusor is located from the top left, in pixels.
//
//Rate - Rate at which the cursor whould flash.
//
//Enable - Non-zero to display cursor, 0 not to display.
//

//
// IOCTL_VIDEO_QUERY_POINTER_POSITION - Returns the location of the pointer
//                                      on the screen
//
// IOCTL_VIDEO_SET_POINTER_POSITION - Is used to set the location of the
//                                    pointer on the screen.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_POINTER_POSITION {
    SHORT Column;
    SHORT Row;
} VIDEO_POINTER_POSITION, *PVIDEO_POINTER_POSITION;

//
//Column - Column on which the cursor is located from the top left, in pixels.
//
//Row - Row on which the cusor is located from the top left, in pixels.
//


//
// IOCTL_VIDEO_QUERY_POINTER_ATTR - Returns all attributes of the pointer.
//
// IOCTL_VIDEO_SET_POINTER_ATTR - Is used to set the attributes of the
//                                pointer.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_POINTER_ATTRIBUTES {
    ULONG Flags;
    ULONG Width;
    ULONG Height;
    ULONG WidthInBytes;
    ULONG Enable;
    SHORT Column;
    SHORT Row;
    UCHAR Pixels[1];
} VIDEO_POINTER_ATTRIBUTES, *PVIDEO_POINTER_ATTRIBUTES;

//
//Flags - color or mono pointer, same as for query pointer capabilities.
//
//Width - Width of the pointer, in pixels.
//
//Height - Height of the pointer, in scans.
//
//WidthInBytes - Width of the pointer, in bytes.
//
//Enable - Non-zero to display pointer, 0 not to display.
//
//Column - Column on which the cursor is located from the top left, in pixels.
//
//Row - Row on which the cusor is located from the top left, in pixels.
//
//Pixels - Start of pointer data, in device-compatible DIB format.
//    (Mask data is always in 1-bpp DIB format.)
//


//
// IOCTL_VIDEO_QUERY_POINTER_CAPABILITIES - Returns capabilities of miniport
//                                          hardware cursor
//

typedef struct _VIDEO_POINTER_CAPABILITIES {
    ULONG Flags;
    ULONG MaxWidth;
    ULONG MaxHeight;
    ULONG HWPtrBitmapStart;
    ULONG HWPtrBitmapEnd;
} VIDEO_POINTER_CAPABILITIES, *PVIDEO_POINTER_CAPABILITIES;

//
// Flag bit definitions
//

#define VIDEO_MODE_ASYNC_POINTER  0x01 // 1 if the cursor can be updated
                                       // asynchronously to drawing operations.
#define VIDEO_MODE_MONO_POINTER   0x02 // 1 if a monochrome hardware pointer
                                       // is supported.
#define VIDEO_MODE_COLOR_POINTER  0x04 // 1 if a color hardware pointer is
                                       // supported.
#define VIDEO_MODE_ANIMATE_START  0x08 // The pointer being passed down has
#define VIDEO_MODE_ANIMATE_UPDATE 0x10 // the same hotspot as the previous
                                       // pointer

//
//MaxWidth - Widest pointer bitmap the miniport should be requested to load
//    for either monochrome or color pointer.
//
//MaxHeight - widest pointer bitmap the miniport should be requested to load
//    for either monochrome color pointer handled.
//
//HWPtrBitmapStart = first offset in bitmap of memory used to store hardware
//    pointer bitmap, in CPU-addressable units (-1 if not applicable). For
//    planar modes (like VGA mode 12h), this is a planar offset; for linear
//    modes (like VGA mode 13h), this is a linear offset. The CPU-addressable
//    translation in HC planar mode is assumed to be linearaddress/4,
//    because there are four planes at each address.
//
//HWPtrBitmapEnd = last offset in bitmap of memory used to store hardware
//    pointer bitmap (-1 if not applicable).
//
// Note: Miniport has options to reject any call to set a pointer.
//


//
// IOCTL_VIDEO_GET_BANK_SELECT_CODE - Called by the Windows display driver
//                                    to get a block of executable code used
//                                    to perform bank-switching in high
//                                    resolution SVGA drivers.
//
// Gets information needed to implement banking control for a selected mode.
//
// Information used by this function is passed using the following structures:
//

//
// The input from the caller in the input buffer is a VIDEO_MODE structure, as
// described under IOCTL_VIDEO_SET_CURRENT_MODE.
//
// RequestedMode - mode index for which banking information is desired.
//

//
// Returned in output buffer.
//

typedef struct _VIDEO_BANK_SELECT {
    ULONG Length;
    ULONG Size;
    ULONG BankingFlags;
    ULONG BankingType;
    ULONG PlanarHCBankingType;
    ULONG BitmapWidthInBytes;
    ULONG BitmapSize;
    ULONG Granularity;
    ULONG PlanarHCGranularity;
    ULONG CodeOffset;
    ULONG PlanarHCBankCodeOffset;
    ULONG PlanarHCEnableCodeOffset;
    ULONG PlanarHCDisableCodeOffset;
} VIDEO_BANK_SELECT, *PVIDEO_BANK_SELECT;

//
// Stored in the BankType and PlanarHCBankintType fields
//

typedef enum _VIDEO_BANK_TYPE {
    VideoNotBanked = 0,
    VideoBanked1RW,
    VideoBanked1R1W,
    VideoBanked2RW,
    NumVideoBankTypes
} VIDEO_BANK_TYPE, *PVIDEO_BANK_TYPE;

//
// Defines for BankingFlags.
//

#define PLANAR_HC               0x00000001

//
//Note: planar high-color ("planar HC") mode is a special 8-bpp-and-up
//    CPU addressing mode in which four bytes can be accessed at
//    once by using the VGA's planar hardware.  This mode is enabled
//    by turning off the Chain4 bit (bit 3 in Sequence Controller
//    register 4), so it is also known as non-Chain4 mode.  Planar HC
//    mode can greatly accelerate operations such as solid fills,
//    some pattern fills, and some blits.
//
//Note: the term "CPU-addressable bytes" means offsets measured
//    in bytes as accessed by the CPU.  In 16-color modes, this
//    merely means "measured in bytes" rather than "measured in
//    pixels," where each byte contains 8 pixels, as usual.
//    In normal high-color modes, "CPU-addressable bytes"
//    is exactly what you'd expect; it's the number of pixels in 256
//    color modes, pixels*2 in 16-bpp modes, and so on.  However, in
//    planar HC modes, there are four display memory bytes at every CPU-
//    addressable byte, because four planes are at each address, so
//    in 256 color modes the number of CPU-addressable bytes is
//    pixels/4, in 16-bpp modes CPU-addressable bytes = pixels/2, and
//    so on.  Basically, "CPU-addressable bytes" just means the
//    offsets the CPU needs to address banks properly in the
//    specified mode.
//
//Note: the start address must be set to 0 (displayed pixels must
//    start at offset 0 in display memory), and the banking windows
//    must fit within the 64K area starting at A000:0; no 128K
//    mappings, please, because there may be a monochrome adapter
//    in the system.
//
//Length - Length of the basic structure. Used for versioning by checking the
//    Length of the struct is at least as large as the value given by sizeof().
//
//Size - number of bytes required to hold all banking information for
//    this mode, including the VIDEO_BANK_SELECT structure and all
//    bank-switch code.  This is the size of the buffer that
//    VgaGetBankSelectCode requires in order properly to return info.
//
//BankingFlags - indicate the type of banking supported in this mode.
//    PLANAR_HC - if set, indicates that planar high-color (HC) mode
//          (non-Chain4 8-, 15-, 16-, 24-, and 32-bpp) is supported.
//          If this bit is set, the following fields must be filled in:
//              PlanarHCGranularity, pPlanarHCBankCode,
//              pPlanarHCEnableCode, pPlanarHCDisableCode.
//          This bit is ignored by the 16-color driver, as are the
//          associated fields.
//
//BankingType - These are the banking types supported by the adapter
//    when it is ina standard mode.
//
//    VideoNotBanked - this mode does not support or require banking.
//    VideoBanked1RW - this mode supports a single RW (readable and
//        writable) banking window.  The window is assumed to be
//        64K in size.
//    VideoBanked1R1W - this mode supports a single window, but the
//        window can be mapped to different areas of display memory
//        for reads and for writes.  The window is assumed to be
//        64K in size.
//    VideoBanked2RW - this mode supports two independently mappable
//        banking windows, each RW.  Each window is assumed to be
//        32K in size.  The second window is assumed
//        to start immediately after the end of the first, at
//        A000:8000.
//
//PlanarHCBankingType - These are the banking types supported by the
//    adapter when it is in a PLANAR HC mode.
//
//    See BankingType for defintions of each banking type.
//
//
//BitmapWidthInBytes - distance from start of one scan line to start
//    of next, counted in CPU-addressable bytes (not pixels).  The
//    CPU-addressable distance from one scan line to the next is
//    assumed to be BitmapWidthInBytes/4 in planar HC modes, because
//    there are four planes at each address.
//
//BitmapSize - size of display memory in CPU-addressable bytes (for
//    example, 256K on a 1 Mb SVGA in 16-color mode, because there
//    are four bytes at each address).  The CPU-addressable bitmap
//    size is assumed to be BitmapSize/4 in planar HC modes, because
//    there are four planes at each address.
//
//Granularity - granularity with which display memory may be mapped
//    into a banking window.  (That is, resolution with which the
//    display memory address mapped to the start of a window may be
//    set; anywhere from 1K to 64K, depending on the adapter.  If
//    Granularity < window size (either 64K or 32K), then adjacent
//    banks can overlap, and broken rasters can always be avoided.
//    If Granularity == window size, then banks are disjoint, and
//    display memory is basically segmented into banks.)  Granularity
//    is measured in CPU-addressable bytes.
//
//PlanarHCGranularity - granularity with which display memory may be
//    mapped into a banking window in planar HC mode.
//    PlanarHCGranularity is measured in CPU-addressable bytes, and
//    is typically but not always Granularity/4.  Ignored in
//    16-color modes.
//
//CodeOffset - base of the code section in the structure.
//
//PlanarHCBankCodeOffset - offset from Code of executable code
//    that performs planar HC mode bank switching.  Ignored in
//    16-color modes.
//
//PlanarHCEnableCodeOffset - offset from Code of executable code
//    that enables planar HC mode.  Ignored in 16-color modes.
//
//PlanarHCDisableCodeOffset - offset from Code of executable code
//    that disables planar HC mode.  Ignored in 16-color modes.
//
//Specification for bank switch code at Code:
//    Executes requested bank mappings.
//
//    Input:
//      EAX = bank number to which to map window #0
//      EDX = bank number to which to map window #1
//      interpreted according to BankingType as follows:
//        VideoBanked1RW - the single window is mapped to bank EAX,
//            EBX is ignored.
//        VideoBanked1RW - the read window is mapped to bank EAX,
//            the write window is mapped to bank EBX
//        VideoBanked1R1W - the window at A000:0 is mapped to bank EAX,
//            the window at A800:0 is mapped to bank EBX
//
//    Output: none
//
// Note: the definition of "bank n" is the bank that starts at
//    display memory offset Granularity*n.  In other words,
//    banks are assumed to start every Granularity CPU-addressable
//    bytes, and are numbered from 0 to number of banks-1.
//
//Specification for planar HC executable code:
//    ***To be filled in when we get to planar HC modes***
//


//
// IOCTL_VIDEO_MAP_VIDEO_MEMORY - Maps the frame buffer into the callers
//                                address space.
// IOCTL_VIDEO_UNMAP_VIDEO_MEMORY - Unmaps the frame buffer from the callers
//                                  address space.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_MEMORY {
    PVOID RequestedVirtualAddress;
} VIDEO_MEMORY, *PVIDEO_MEMORY;

//
//RequestedVirtualAddress - For MAP: Requested virtual address for the video
//    memory. This value is optional. If zero is specified, the operating
//    system will choose an appropriate location.  For UNMAP: Virtual Address
//    of the base of video memory. The size is implicit since it can not
//    change (you can not add video memory dynamically!).
//

// IOCTL_VIDEO_SHARE_VIDEO_MEMORY - Maps the frame buffer to another process'
//                                  address space.  This IOCTL is initally
//                                  defined to support DCI.
// IOCTL_VIDEO_UNSHARE_VIDEO_MEMORY - Unmaps a previously shared buffer.
//
// Note: for the MAP_VIDEO_MEMORY_IOCTL, the process handle is passed in
// the VirtualAddress filed, while for this IOCTL the handle is explicit.
//

typedef struct _VIDEO_SHARE_MEMORY {
    HANDLE ProcessHandle;
    ULONG ViewOffset;
    ULONG ViewSize;
    PVOID RequestedVirtualAddress;
} VIDEO_SHARE_MEMORY, *PVIDEO_SHARE_MEMORY;

typedef struct _VIDEO_SHARE_MEMORY_INFORMATION {
    ULONG SharedViewOffset;
    ULONG SharedViewSize;
    PVOID VirtualAddress;
} VIDEO_SHARE_MEMORY_INFORMATION, *PVIDEO_SHARE_MEMORY_INFORMATION;


//
// IOCTL_VIDEO_MAP_VIDEO_MEMORY - Returns the virtual address and size of
//                                the frame buffer and video memory in the
//                                caller's address space.
//                                This IOCTL must be called after a call
//                                to the MAP IOCTL has been made.
//

typedef struct _VIDEO_MEMORY_INFORMATION {
    PVOID VideoRamBase;
    ULONG VideoRamLength;
    PVOID FrameBufferBase;
    ULONG FrameBufferLength;
} VIDEO_MEMORY_INFORMATION, *PVIDEO_MEMORY_INFORMATION;

//
//VideoRamBase - Virtual address of the Video RAM in the callers address space
//    (only valid if the memory is mapped.
//
//VideoRamLength - Linear length of the Video RAM in the caller's virtual
//    address space (memory accessible through a bank switch mechanism is not
//    described by this value).
//    This value must be equal to VideoMemoryBitmapHeight * ScreenStride
//
//FrameBufferBase - Virtual address of the Frame Buffer in the caller's
//    address space. The Frame buffer is the actively displayed part of Video
//    Ram.
//
//FrameBufferLength - Linear length of the Frame Buffer in the caller's
//    virtual address space (memory accessible through a bank switch mechanism
//    is not described by this value).
//    This value must be equal to VisScreenWidth * ScreenStride
//


//
// IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES - Returns the access range used to
//                                          program the hardware directly.
//                                          An array of these is returned if
//                                          multiple ranges exist.
//
// IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES - Frees up the access ranges that were
//                                         allocated by the QUERY_ACCESS_RANGES
//                                         call.
//
// Information used by this function is passed using the following structure:
//

typedef struct _VIDEO_PUBLIC_ACCESS_RANGES {
    ULONG InIoSpace;
    ULONG MappedInIoSpace;
    PVOID VirtualAddress;
} VIDEO_PUBLIC_ACCESS_RANGES, *PVIDEO_PUBLIC_ACCESS_RANGES;

//
//InIoSpace - Indicates if the hardware registers or ports are in IO space
//    or in memory space.
//
//MappedInIoSpace - Indicates if under the current platform the registers or
//    ports are mapped in IO Space or memory space.
//
//VirtualAddress - Location of the registers or IO ports as mapped under the
//    current architecture.
//


//
// IOCTL_VIDEO_QUERY_COLOR_CAPABILITIES - Returns the color information
//                                        found in the monitors VDDPs
//                                        description file.
//
// NOTE: This structure must be filled out completely. A subset of the
//         values can not be returned.
//

typedef struct _VIDEO_COLOR_CAPABILITIES {
    ULONG Length;
    ULONG AttributeFlags;
    LONG  RedPhosphoreDecay;
    LONG  GreenPhosphoreDecay;
    LONG  BluePhosphoreDecay;
    LONG  WhiteChromaticity_x;
    LONG  WhiteChromaticity_y;
    LONG  WhiteChromaticity_Y;
    LONG  RedChromaticity_x;
    LONG  RedChromaticity_y;
    LONG  GreenChromaticity_x;
    LONG  GreenChromaticity_y;
    LONG  BlueChromaticity_x;
    LONG  BlueChromaticity_y;
    LONG  WhiteGamma;
    LONG  RedGamma;
    LONG  GreenGamma;
    LONG  BlueGamma;
} VIDEO_COLOR_CAPABILITIES, *PVIDEO_COLOR_CAPABILITIES;

//
// Flag Bit definitions
//

#define VIDEO_DEVICE_COLOR          0x1   // Is this device support color (1)
                                          // or monochrome only
#define VIDEO_OPTIONAL_GAMMET_TABLE 0x2   // Indicates that a gammet table can
                                          // be queried/set for the device
                                          // use other IOCTLs for that purpose.
//
//Length - Length of the basic structure. Used for versioning by checking the
//    Length of the struct is at least as large as the value given by sizeof().
//
//AttributesFlag - List of falgs determining some of the properties of the
//    device.
//
//See the VDDP documentation for the details on the various fields
//
//RedPhosphoreDecay
//GreenPhosphoreDecay
//BluePhosphoreDecay -
//
//WhiteChromaticity_x
//WhiteChromaticity_y
//WhiteChromaticity_Y -
//
//RedChromaticity_x
//RedChromaticity_y
//GreenChromaticity_x
//GreenChromaticity_y
//BlueChromaticity_x
//BlueChromaticity_y -
//
//WhiteGamma -
//
//RedGamma
//GreenGamma
//BlueGamma -
//
//All values returned in this structure are integers.
//The values returned must be floating point values * 10,000; i.e:
//a gamma of 2.34 would be returned as 23400.
//



//
// IOCTL_VIDEO_SET_POWER_MANAGEMENT - Tells the device to change the power
//                                    consumption level of the device to the
//                                    new state.
// IOCTL_VIDEO_GET_POWER_MANAGEMENT - Return the current power consumption
//                                    level of the device.
//
// Private IOCTLs intercepted by the video port:
//
// IOCTL_VIDEO_SET_OUTPUT_DEVICE_POWER_STATE - Sets the power state on the
//                                             output device
//
// IOCTL_VIDEO_GET_OUTPUT_DEVICE_POWER_STATE - Returns if it is possible to set
//                                             this partcular power state on the
//                                             output device (monitor, TV).
//
// NOTE:
// This IOCTL is based on the VESA DPMS proposal.
// Changes to the DPMS standard will be refelcted in this IOCTL.
//

typedef enum _VIDEO_POWER_STATE {
    VideoPowerUnspecified = 0,
    VideoPowerOn = 1,
    VideoPowerStandBy,
    VideoPowerSuspend,
    VideoPowerOff,
    VideoPowerHibernate,
    VideoPowerShutdown,
    VideoPowerMaximum
} VIDEO_POWER_STATE, *PVIDEO_POWER_STATE;


typedef struct _VIDEO_POWER_MANAGEMENT {
    ULONG Length;
    ULONG DPMSVersion;
    ULONG PowerState;
} VIDEO_POWER_MANAGEMENT, *PVIDEO_POWER_MANAGEMENT;

//
//Length - Length of the structure in bytes. Also used to do verisioning.
//
//DPMSVersion - Version of the DPMS standard supported by the device.
//              Only used in the "GET" IOCTL.
//
//PowerState - One of the power states listed in VIDEO_POWER_STATE.
//

//
// Note:
// Once the power has been turned off to the device, all other IOCTLs made
// to the miniport will be intercepted by the port driver and will return
// failiure, until the power on the device has been turned back on.
//


//
// IOCTL_VIDEO_SET_COLOR_LUT_DATA - Confugure color look up table on video adaptor.
//

typedef struct _VIDEO_COLOR_LUT_DATA {
    ULONG Length;
    ULONG LutDataFormat;
    UCHAR LutData[1];
} VIDEO_COLOR_LUT_DATA, *PVIDEO_COLOR_LUT_DATA;

//
// Length - Length of the structure in bytes.
//
// LutDataFormat values - indicate data format in ColorLutTable.
//
// LutDataTable - color lut table data.
//

#define VIDEO_COLOR_LUT_DATA_FORMAT_RGB256WORDS     0x00000001

typedef struct _VIDEO_LUT_RGB256WORDS {
    USHORT Red[256];
    USHORT Green[256];
    USHORT Blue[256];
} VIDEO_LUT_RGB256WORDS, *PVIDEO_LUT_RGB256WORDS;

#define VIDEO_COLOR_LUT_DATA_FORMAT_PRIVATEFORMAT   0x80000000

//
// VIDEO_COLOR_LUT_DATA_FORMAT_RGB256WORDS -
//      Lut data has 3 array of 256 WORDs. 1st 256 WORDs array for Red, next
//     for Blue, then Green. And its value have to be packed in the most
//     significant bits of the WORDs (0 to 0xFF00 for 8 bit). This allows
//     for 8, 12 and 16 bit RAMDAC independance. Thus Driver can shifts them
//     right by 8, 4 or 0 places for 8, 12 and 16 bits RAMDAC.
//
// VIDEO_COLOR_LUT_DATA_FORMAT_PRIVATEFORMAT -
//      Driver defined format. This value should be OR-ed with other driver
//     internal identify index in 0 - 30 bits. Callee should know the detail
//     format.
//

//
// BANK_POSITION
//

typedef struct _BANK_POSITION
{
    ULONG ReadBankPosition;
    ULONG WriteBankPosition;
} BANK_POSITION, *PBANK_POSITION;

//
// IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS - Queries via _BCL the available backlight
//                                          levels.
// IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS   - Queries the current AC/DC backlight levels
//                                          and indicates the current power state per
//                                          ucDisplayPolicy.
// IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS     - Sets via _BCM the AC/DC brightness of the
//                                          backlight for the power states indicated
//                                          in ucDisplayPolicy.
//

typedef struct _DISPLAY_BRIGHTNESS {
    UCHAR ucDisplayPolicy;
    UCHAR ucACBrightness;
    UCHAR ucDCBrightness;
} DISPLAY_BRIGHTNESS, *PDISPLAY_BRIGHTNESS;

#define DISPLAYPOLICY_AC                0x00000001
#define DISPLAYPOLICY_DC                0x00000002
#define DISPLAYPOLICY_BOTH              (DISPLAYPOLICY_AC | DISPLAYPOLICY_DC)



//+----------------------------------------------------------------------------
//
//  Far East fullscreen support
//
//-----------------------------------------------------------------------------


#ifndef _WINCON_

typedef struct _COORD {
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;

typedef struct _CHAR_INFO {
    union {
        WCHAR UnicodeChar;
        CHAR   AsciiChar;
    } Char;
    USHORT Attributes;
} CHAR_INFO, *PCHAR_INFO;

//
// Attributes flags:
//

#define FOREGROUND_BLUE      0x0001 // text color contains blue.
#define FOREGROUND_GREEN     0x0002 // text color contains green.
#define FOREGROUND_RED       0x0004 // text color contains red.
#define FOREGROUND_INTENSITY 0x0008 // text color is intensified.
#define BACKGROUND_BLUE      0x0010 // background color contains blue.
#define BACKGROUND_GREEN     0x0020 // background color contains green.
#define BACKGROUND_RED       0x0040 // background color contains red.
#define BACKGROUND_INTENSITY 0x0080 // background color is intensified.
#define COMMON_LVB_LEADING_BYTE    0x0100 // Leading Byte of DBCS
#define COMMON_LVB_TRAILING_BYTE   0x0200 // Trailing Byte of DBCS
#define COMMON_LVB_GRID_HORIZONTAL 0x0400 // DBCS: Grid attribute: top horizontal.
#define COMMON_LVB_GRID_LVERTICAL  0x0800 // DBCS: Grid attribute: left vertical.
#define COMMON_LVB_GRID_RVERTICAL  0x1000 // DBCS: Grid attribute: right vertical.
#define COMMON_LVB_REVERSE_VIDEO   0x4000 // DBCS: Reverse fore/back ground attribute.
#define COMMON_LVB_UNDERSCORE      0x8000 // DBCS: Underscore.

#define COMMON_LVB_SBCSDBCS        0x0300 // SBCS or DBCS flag.



//
// Share of conapi.h
//
#define CHAR_TYPE_SBCS     0   // Displayed SBCS character
#define CHAR_TYPE_LEADING  2   // Displayed leading byte of DBCS
#define CHAR_TYPE_TRAILING 3   // Displayed trailing byte of DBCS


//
// Share of foncache.h
//
#define BITMAP_BITS_BYTE_ALIGN   8 // BYTE align is 8 bit
#define BITMAP_BITS_WORD_ALIGN  16 // WORD align is 16 bit
#define BITMAP_ARRAY_BYTE  3       // BYTE array is 8 bit  (shift count = 3)

#define BITMAP_PLANES      1
#define BITMAP_BITS_PIXEL  1


#define BYTE_ALIGN  sizeof(UCHAR)
#define WORD_ALIGN  sizeof(USHORT)


#endif // _WINCON_


typedef struct _FSCNTL_SCREEN_INFO {
    COORD Position;
    COORD ScreenSize;
    ULONG nNumberOfChars;
} FSCNTL_SCREEN_INFO, *PFSCNTL_SCREEN_INFO;


typedef struct _FONT_IMAGE_INFO {
    COORD  FontSize;
    PUCHAR ImageBits;                                 // WORD aligned.
} FONT_IMAGE_INFO, *PFONT_IMAGE_INFO;


typedef struct _CHAR_IMAGE_INFO {
    CHAR_INFO       CharInfo;
    FONT_IMAGE_INFO FontImageInfo;
} CHAR_IMAGE_INFO, *PCHAR_IMAGE_INFO;

//
// Share of consrv.h
//
#define SCREEN_BUFFER_POINTER(X,Y,XSIZE,CELLSIZE) (((XSIZE * (Y)) + (X)) * (ULONG)CELLSIZE)

typedef struct _VGA_CHAR {
    CHAR Char;
    CHAR Attributes;
} VGA_CHAR, *PVGA_CHAR;


//
// Define the Full Screen Video device name strings.
//

#define DD_FULLSCREEN_VIDEO_DEVICE_NAME L"\\Device\\FSVideo"


//
// IOCTL_FSVIDEO_COPY_FRAME_BUFFER - Copy in the frame buffer.
//
typedef struct _FSVIDEO_COPY_FRAME_BUFFER {
    FSCNTL_SCREEN_INFO SrcScreen;
    FSCNTL_SCREEN_INFO DestScreen;
} FSVIDEO_COPY_FRAME_BUFFER, *PFSVIDEO_COPY_FRAME_BUFFER;

//
// IOCTL_FSVIDEO_WRITE_TO_FRAME_BUFFER - Write to the frame buffer.
//
typedef struct _FSVIDEO_WRITE_TO_FRAME_BUFFER {
    PCHAR_IMAGE_INFO   SrcBuffer;
    FSCNTL_SCREEN_INFO DestScreen;
} FSVIDEO_WRITE_TO_FRAME_BUFFER, *PFSVIDEO_WRITE_TO_FRAME_BUFFER;

//
// IOCTL_FSVIDEO_REVERSE_MOUSE_POINTER - Reverse to the frame buffer for mouse pointer.
//
// dwType as follows:
//    CHAR_TYPE_SBCS     0   // Displayed SBCS character
//    CHAR_TYPE_LEADING  2   // Displayed leading byte of DBCS
//    CHAR_TYPE_TRAILING 3   // Displayed trailing byte of DBCS
//
typedef struct _FSVIDEO_REVERSE_MOUSE_POINTER {
    FSCNTL_SCREEN_INFO Screen;
    ULONG dwType;
} FSVIDEO_REVERSE_MOUSE_POINTER, *PFSVIDEO_REVERSE_MOUSE_POINTER;

//
// IOCTL_FSVIDEO_SET_CURRENT_MODE - Set the information for the current
//                                  video mode.
//
// Information used by this function is passed using the following structure:
//
typedef struct _FSVIDEO_MODE_INFORMATION {
    VIDEO_MODE_INFORMATION VideoMode;
    VIDEO_MEMORY_INFORMATION VideoMemory;
} FSVIDEO_MODE_INFORMATION, *PFSVIDEO_MODE_INFORMATION;

//
// IOCTL_FSVIDEO_SET_SCREEN_INFORMATION - Set the information for current console screen
//
typedef struct _FSVIDEO_SCREEN_INFORMATION {
    COORD ScreenSize;
    COORD FontSize;
} FSVIDEO_SCREEN_INFORMATION, *PFSVIDEO_SCREEN_INFORMATION;


//
// IOCTL_FSVIDEO_SET_CURSOR_POSITION - Set the information for cursor position
//
// dwType as follows:
//    CHAR_TYPE_SBCS     0   // Displayed SBCS character
//    CHAR_TYPE_LEADING  2   // Displayed leading byte of DBCS
//    CHAR_TYPE_TRAILING 3   // Displayed trailing byte of DBCS
//
typedef struct _FSVIDEO_CURSOR_POSITION {
    VIDEO_CURSOR_POSITION Coord;
    ULONG dwType;
} FSVIDEO_CURSOR_POSITION, *PFSVIDEO_CURSOR_POSITION;

#ifdef __cplusplus
}
#endif

#endif  // _NTDDVDEO_

