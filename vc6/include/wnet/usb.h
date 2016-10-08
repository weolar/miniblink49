/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

        USB.H

Abstract:

   structures and APIs for USB drivers.

Environment:

    Kernel & user mode

Revision History:

    09-29-95 : created

--*/

#ifndef   __USB_H__
#define   __USB_H__

/*
    This file is equivalent to USBDI.H with extensions supported by 
    usbport.sys for eUSB.        
    
    This file replaces usbdi.h and is compatible with older versions
    of the USB stack.
*/

#ifdef __USBDI_H__
error
#endif

#ifdef OSR21_COMPAT
#pragma message("WARNING: OSR21_COMPAT SWITCH NOT SUPPORTED")
#endif

#ifndef _NTDDK_
#ifndef _WDMDDK_
typedef PVOID PIRP;
typedef PVOID PMDL;
#endif
#endif

#define USBDI_VERSION    0x00000500

#include "usb200.h"
#ifdef _WDMDDK_
#endif


/*
    Microsoft Extended Port Attribute Flags
*/

#define USB_PORTATTR_NO_CONNECTOR       0x00000001
#define USB_PORTATTR_SHARED_USB2        0x00000002
#define USB_PORTATTR_MINI_CONNECTOR     0x00000004
#define USB_PORTATTR_OEM_CONNECTOR      0x00000008

/* dynamic attributes */
#define USB_PORTATTR_OWNED_BY_CC        0x01000000
#define USB_PORTATTR_NO_OVERCURRENT_UI  0x02000000


/* define USB controller flavors:
    These are all known HW implementations that require special 
    hacks.
*/
    
typedef enum _USB_CONTROLLER_FLAVOR {

    USB_HcGeneric   = 0,
    
    OHCI_Generic    = 100,
    OHCI_Hydra,
    OHCI_NEC,
    
    UHCI_Generic    = 200,
    UHCI_Piix4,
    UHCI_Piix3,
    UHCI_Ich2_1,
    UHCI_Ich2_2,
    UHCI_Ich1,
    
    UHCI_VIA        = 250,

    EHCI_Generic    = 1000,
    EHCI_NEC        = 2000,
    EHCI_Lucent     = 3000
    
} USB_CONTROLLER_FLAVOR;


//
// USB defined structures and constants
// (see chapter 9 of USB specification)
//

#define USB_DEFAULT_DEVICE_ADDRESS     0
#define USB_DEFAULT_ENDPOINT_ADDRESS   0

//
// max packet size (bytes) for default endpoint
// until SET_ADDRESS command is received.
//

#define USB_DEFAULT_MAX_PACKET         64

//
// USBD interface structures and constants
//


#define URB_FROM_IRP(Irp) ((IoGetCurrentIrpStackLocation(Irp))->Parameters.Others.Argument1)

//
//  URB request codes
//
                                                    
#define URB_FUNCTION_SELECT_CONFIGURATION            0x0000
#define URB_FUNCTION_SELECT_INTERFACE                0x0001
#define URB_FUNCTION_ABORT_PIPE                      0x0002
#define URB_FUNCTION_TAKE_FRAME_LENGTH_CONTROL       0x0003
#define URB_FUNCTION_RELEASE_FRAME_LENGTH_CONTROL    0x0004
#define URB_FUNCTION_GET_FRAME_LENGTH                0x0005
#define URB_FUNCTION_SET_FRAME_LENGTH                0x0006
#define URB_FUNCTION_GET_CURRENT_FRAME_NUMBER        0x0007
#define URB_FUNCTION_CONTROL_TRANSFER                0x0008
#define URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER      0x0009
#define URB_FUNCTION_ISOCH_TRANSFER                  0x000A
#define URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE      0x000B
#define URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE        0x000C
#define URB_FUNCTION_SET_FEATURE_TO_DEVICE           0x000D
#define URB_FUNCTION_SET_FEATURE_TO_INTERFACE        0x000E
#define URB_FUNCTION_SET_FEATURE_TO_ENDPOINT         0x000F
#define URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE         0x0010
#define URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE      0x0011
#define URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT       0x0012
#define URB_FUNCTION_GET_STATUS_FROM_DEVICE          0x0013
#define URB_FUNCTION_GET_STATUS_FROM_INTERFACE       0x0014
#define URB_FUNCTION_GET_STATUS_FROM_ENDPOINT        0x0015
#define URB_FUNCTION_RESERVED_0X0016                 0x0016
#define URB_FUNCTION_VENDOR_DEVICE                   0x0017
#define URB_FUNCTION_VENDOR_INTERFACE                0x0018
#define URB_FUNCTION_VENDOR_ENDPOINT                 0x0019
#define URB_FUNCTION_CLASS_DEVICE                    0x001A
#define URB_FUNCTION_CLASS_INTERFACE                 0x001B
#define URB_FUNCTION_CLASS_ENDPOINT                  0x001C
#define URB_FUNCTION_RESERVE_0X001D                  0x001D
// previously URB_FUNCTION_RESET_PIPE
#define URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL 0x001E
#define URB_FUNCTION_CLASS_OTHER                     0x001F
#define URB_FUNCTION_VENDOR_OTHER                    0x0020
#define URB_FUNCTION_GET_STATUS_FROM_OTHER           0x0021
#define URB_FUNCTION_CLEAR_FEATURE_TO_OTHER          0x0022
#define URB_FUNCTION_SET_FEATURE_TO_OTHER            0x0023
#define URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT    0x0024
#define URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT      0x0025
#define URB_FUNCTION_GET_CONFIGURATION               0x0026
#define URB_FUNCTION_GET_INTERFACE                   0x0027
#define URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE   0x0028
#define URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE     0x0029
// Rserveve 0x002B-0x002F
#define URB_FUNCTION_GET_MS_FEATURE_DESCRIPTOR       0x002A
#define URB_FUNCTION_RESERVE_0X002B                  0x002B
#define URB_FUNCTION_RESERVE_0X002C                  0x002C
#define URB_FUNCTION_RESERVE_0X002D                  0x002D
#define URB_FUNCTION_RESERVE_0X002E                  0x002E
#define URB_FUNCTION_RESERVE_0X002F                  0x002F
// USB 2.0 calls start at 0x0030         
#define URB_FUNCTION_SYNC_RESET_PIPE                 0x0030
#define URB_FUNCTION_SYNC_CLEAR_STALL                0x0031

// for backward drivers
#define URB_FUNCTION_RESET_PIPE     \
    URB_FUNCTION_SYNC_RESET_PIPE_AND_CLEAR_STALL

/* Control Pipe Function Groupings

   These functions correspond to the standard commands 
   on the default pipe, direction is implied
  

URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE     
URB_FUNCTION_GET_DESCRIPTOR_FROM_ENDPOINT   
URB_FUNCTION_GET_DESCRIPTOR_FROM_INTERFACE  
                                                           
URB_FUNCTION_SET_DESCRIPTOR_TO_DEVICE       
URB_FUNCTION_SET_DESCRIPTOR_TO_ENDPOINT     
URB_FUNCTION_SET_DESCRIPTOR_TO_INTERFACE    

URB_FUNCTION_SET_FEATURE_TO_DEVICE          
URB_FUNCTION_SET_FEATURE_TO_INTERFACE       
URB_FUNCTION_SET_FEATURE_TO_ENDPOINT        
URB_FUNCTION_SET_FEATURE_TO_OTHER           

URB_FUNCTION_CLEAR_FEATURE_TO_DEVICE        
URB_FUNCTION_CLEAR_FEATURE_TO_INTERFACE     
URB_FUNCTION_CLEAR_FEATURE_TO_ENDPOINT      
URB_FUNCTION_CLEAR_FEATURE_TO_OTHER         

URB_FUNCTION_GET_STATUS_FROM_DEVICE         
URB_FUNCTION_GET_STATUS_FROM_INTERFACE      
URB_FUNCTION_GET_STATUS_FROM_ENDPOINT       
URB_FUNCTION_GET_STATUS_FROM_OTHER          

URB_FUNCTION_VENDOR_DEVICE                   
URB_FUNCTION_VENDOR_INTERFACE                
URB_FUNCTION_VENDOR_ENDPOINT                 
URB_FUNCTION_VENDOR_OTHER                    

URB_FUNCTION_CLASS_DEVICE                    
URB_FUNCTION_CLASS_INTERFACE                 
URB_FUNCTION_CLASS_ENDPOINT                  
URB_FUNCTION_CLASS_OTHER                     

*/

//
// Values for URB TransferFlags Field
//

/*
    Set if data moves device->host
*/
#define USBD_TRANSFER_DIRECTION               0x00000001
/*
    This bit if not set indicates that a short packet, and hence,
    a short transfer is an error condition
*/
#define USBD_SHORT_TRANSFER_OK                0x00000002
/*
    Subit the iso transfer on the next frame
*/
#define USBD_START_ISO_TRANSFER_ASAP          0x00000004
#define USBD_DEFAULT_PIPE_TRANSFER            0x00000008


#define USBD_TRANSFER_DIRECTION_FLAG(flags)  ((flags) & USBD_TRANSFER_DIRECTION)

#define USBD_TRANSFER_DIRECTION_OUT           0   
#define USBD_TRANSFER_DIRECTION_IN            1

#define VALID_TRANSFER_FLAGS_MASK             (USBD_SHORT_TRANSFER_OK | \
                                               USBD_TRANSFER_DIRECTION | \
                                               USBD_START_ISO_TRANSFER_ASAP | \
                                               USBD_DEFAULT_PIPE_TRANSFER)
                                               
#define USBD_ISO_START_FRAME_RANGE            1024

typedef LONG USBD_STATUS;

//
// USBD status codes
//
//  Status values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+---------------------------+-------------------------------+
//  | S |               Status Code                                 |
//  +---+---------------------------+-------------------------------+
//
//  where
//
//      S - is the state code
//
//          00 - completed with success
//          01 - request is pending
//          11, 10 - completed with error
//
//
//      Code - is the status code
//

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define USBD_SUCCESS(Status) ((USBD_STATUS)(Status) >= 0)

//
// Generic test for pending status value.
//

#define USBD_PENDING(Status) ((ULONG)(Status) >> 30 == 1)

//
// Generic test for error on any status value.
//

#define USBD_ERROR(Status) ((USBD_STATUS)(Status) < 0)

//
// Macro to check the status code only
//
//
//define USBD_STATUS(Status) ((ULONG)(Status) & 0x0FFFFFFFL)

// the high order bits (0xC) will always be set on an error

#define USBD_STATUS_SUCCESS                  ((USBD_STATUS)0x00000000L)
#define USBD_STATUS_PENDING                  ((USBD_STATUS)0x40000000L)
// 
//#define USBD_STATUS_ERROR                    ((USBD_STATUS)0xC0000000L)


// The following are defined for backward compatibility with the usb 1.0 stack


//
// HC (Hardware) status codes range 0x00000001 - 0x000000FF
//

#define USBD_STATUS_CRC                      ((USBD_STATUS)0xC0000001L)
#define USBD_STATUS_BTSTUFF                  ((USBD_STATUS)0xC0000002L)
#define USBD_STATUS_DATA_TOGGLE_MISMATCH     ((USBD_STATUS)0xC0000003L)
#define USBD_STATUS_STALL_PID                ((USBD_STATUS)0xC0000004L)
#define USBD_STATUS_DEV_NOT_RESPONDING       ((USBD_STATUS)0xC0000005L)
#define USBD_STATUS_PID_CHECK_FAILURE        ((USBD_STATUS)0xC0000006L)
#define USBD_STATUS_UNEXPECTED_PID           ((USBD_STATUS)0xC0000007L)
#define USBD_STATUS_DATA_OVERRUN             ((USBD_STATUS)0xC0000008L)
#define USBD_STATUS_DATA_UNDERRUN            ((USBD_STATUS)0xC0000009L)
#define USBD_STATUS_RESERVED1                ((USBD_STATUS)0xC000000AL)
#define USBD_STATUS_RESERVED2                ((USBD_STATUS)0xC000000BL)
#define USBD_STATUS_BUFFER_OVERRUN           ((USBD_STATUS)0xC000000CL)
#define USBD_STATUS_BUFFER_UNDERRUN          ((USBD_STATUS)0xC000000DL)
#define USBD_STATUS_NOT_ACCESSED             ((USBD_STATUS)0xC000000FL)
#define USBD_STATUS_FIFO                     ((USBD_STATUS)0xC0000010L)

#define USBD_STATUS_XACT_ERROR               ((USBD_STATUS)0xC0000011L)
#define USBD_STATUS_BABBLE_DETECTED          ((USBD_STATUS)0xC0000012L)
#define USBD_STATUS_DATA_BUFFER_ERROR        ((USBD_STATUS)0xC0000013L)

//
// returned by HCD if a transfer is submitted to an endpoint that is 
// stalled
//
#define USBD_STATUS_ENDPOINT_HALTED          ((USBD_STATUS)0xC0000030L)

//
// Software status codes
//
#define USBD_STATUS_INVALID_URB_FUNCTION     ((USBD_STATUS)0x80000200L)
#define USBD_STATUS_INVALID_PARAMETER        ((USBD_STATUS)0x80000300L)

//
// returned if client driver attempts to close an endpoint/interface
// or configuration with outstanding transfers.
//
#define USBD_STATUS_ERROR_BUSY               ((USBD_STATUS)0x80000400L)
//
// returned by USBD if it cannot complete a URB request, typically this 
// will be returned in the URB status field when the Irp is completed
// with a more specific NT error code in the irp.status field.
//
//#define USBD_STATUS_REQUEST_FAILED           ((USBD_STATUS)0x80000500L)

#define USBD_STATUS_INVALID_PIPE_HANDLE      ((USBD_STATUS)0x80000600L)

// returned when there is not enough bandwidth avialable
// to open a requested endpoint
#define USBD_STATUS_NO_BANDWIDTH             ((USBD_STATUS)0x80000700L)
//
// generic HC error
// 
#define USBD_STATUS_INTERNAL_HC_ERROR        ((USBD_STATUS)0x80000800L)
//
// returned when a short packet terminates the transfer
// ie USBD_SHORT_TRANSFER_OK bit not set
// 
#define USBD_STATUS_ERROR_SHORT_TRANSFER     ((USBD_STATUS)0x80000900L)
// 
// returned if the requested start frame is not within
// USBD_ISO_START_FRAME_RANGE of the current USB frame, 
// note that the stall bit is set
// 
#define USBD_STATUS_BAD_START_FRAME          ((USBD_STATUS)0xC0000A00L)
//
// returned by HCD if all packets in an iso transfer complete with an error 
//
#define USBD_STATUS_ISOCH_REQUEST_FAILED     ((USBD_STATUS)0xC0000B00L)
//
// returned by USBD if the frame length control for a given 
// HC is already taken by anothe driver
//
#define USBD_STATUS_FRAME_CONTROL_OWNED      ((USBD_STATUS)0xC0000C00L)
//
// returned by USBD if the caller does not own frame length control and
// attempts to release or modify the HC frame length
//
#define USBD_STATUS_FRAME_CONTROL_NOT_OWNED  ((USBD_STATUS)0xC0000D00L)

//
// additonal software error codes added for usb 2.0
//

//
// returned for APIS not supported/implemented
//
#define USBD_STATUS_NOT_SUPPORTED            ((USBD_STATUS)0xC0000E00L)

#define USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR \
                                             ((USBD_STATUS)0xC0000F00L)

#define USBD_STATUS_INSUFFICIENT_RESOURCES   ((USBD_STATUS)0xC0001000L)                                          

#define USBD_STATUS_SET_CONFIG_FAILED        ((USBD_STATUS)0xC0002000L)

#define USBD_STATUS_BUFFER_TOO_SMALL         ((USBD_STATUS)0xC0003000L)

#define USBD_STATUS_INTERFACE_NOT_FOUND      ((USBD_STATUS)0xC0004000L)

#define USBD_STATUS_INAVLID_PIPE_FLAGS       ((USBD_STATUS)0xC0005000L)

#define USBD_STATUS_TIMEOUT                  ((USBD_STATUS)0xC0006000L)

#define USBD_STATUS_DEVICE_GONE              ((USBD_STATUS)0xC0007000L)

#define USBD_STATUS_STATUS_NOT_MAPPED        ((USBD_STATUS)0xC0008000L)

//
// set when a transfers is completed due to an AbortPipe request from
// the client driver
//
//
#define USBD_STATUS_CANCELED                 ((USBD_STATUS)0xC0010000L)

// 
// extended isochronous error codes, these errors appear in the 
// packet status field of an isochronous transfer
//


// for some reason the controller did not access the TD asocated with this 
// packet
#define USBD_STATUS_ISO_NOT_ACCESSED_BY_HW   ((USBD_STATUS)0xC0020000L)   
// controller reported an error in the TD 
// since TD errors are controoler specific they are reorted 
// generically with this error code
#define USBD_STATUS_ISO_TD_ERROR             ((USBD_STATUS)0xC0030000L)   
// the packet was submitted in time by the client but 
// failed to reach the miniport in time
#define USBD_STATUS_ISO_NA_LATE_USBPORT      ((USBD_STATUS)0xC0040000L) 
// the packet was not sent because the client submitted it too late 
// to transmit
#define USBD_STATUS_ISO_NOT_ACCESSED_LATE    ((USBD_STATUS)0xC0050000L)


typedef PVOID USBD_PIPE_HANDLE;
typedef PVOID USBD_CONFIGURATION_HANDLE;
typedef PVOID USBD_INTERFACE_HANDLE;

//
// Value used to indicate the default max transfer size
//

/* 
    MAX TRANSFER SIZE 
    
    Specified during select_configuration or 
    selec_interface.  This is the largest 
    transfer a client driver will do to an
    endpoint.

    This value may be from 0x00000001 to 
    0xFFFFFFFF (1 to 4GB)
    
*/
// 
#define USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE  0xFFFFFFFF


// 
// structure returned from USBD_GetVersion function
//

typedef struct _USBD_VERSION_INFORMATION {
    ULONG USBDI_Version;          //BCD usb interface version number
    ULONG Supported_USB_Version;  //BCD USB spec version number
} USBD_VERSION_INFORMATION, *PUSBD_VERSION_INFORMATION;

typedef enum _USBD_PIPE_TYPE {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
} USBD_PIPE_TYPE;

#define USBD_PIPE_DIRECTION_IN(pipeInformation) ((pipeInformation)->EndpointAddress & \
                                                  USB_ENDPOINT_DIRECTION_MASK) 

typedef struct _USBD_DEVICE_INFORMATION {
    ULONG OffsetNext;
    PVOID UsbdDeviceHandle;
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;
} USBD_DEVICE_INFORMATION, *PUSBD_DEVICE_INFORMATION;

//
//      URB request structures
//

//
// USBD pipe information structure, this structure
// is returned for each pipe opened thru an
// SELECT_CONFIGURATION or SELECT_INTERFACE request.
//

typedef struct _USBD_PIPE_INFORMATION {
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    USHORT MaximumPacketSize;  // Maximum packet size for this pipe
    UCHAR EndpointAddress;     // 8 bit USB endpoint address (includes direction)
                               // taken from endpoint descriptor
    UCHAR Interval;            // Polling interval in ms if interrupt pipe 
    
    USBD_PIPE_TYPE PipeType;   // PipeType identifies type of transfer valid for this pipe
    USBD_PIPE_HANDLE PipeHandle;
    
    //
    // INPUT
    // These fields are filled in by the client driver
    //
    ULONG MaximumTransferSize; // Maximum size for a single request
                               // in bytes.
    ULONG PipeFlags;
} USBD_PIPE_INFORMATION, *PUSBD_PIPE_INFORMATION;

//
// values for PipeFlags field in USBD_PIPE_INFORMATION field
//

// override the enpoint max_packet size
// with the value in pipe_information 
// field
#define USBD_PF_CHANGE_MAX_PACKET             0x00000001
// optimize for short packets
// 'bulk optimization #1'
#define USBD_PF_SHORT_PACKET_OPT              0x00000002 
// optimize transfers for use 
// with 'real time threads
#define USBD_PF_ENABLE_RT_THREAD_ACCESS       0x00000004 
// causes the driver to allocate map
// map more transfers in the queue.
#define USBD_PF_MAP_ADD_TRANSFERS             0x00000008 


#define USBD_PF_VALID_MASK    (USBD_PF_CHANGE_MAX_PACKET | \
                               USBD_PF_SHORT_PACKET_OPT | \
                               USBD_PF_ENABLE_RT_THREAD_ACCESS | \
                               USBD_PF_MAP_ADD_TRANSFERS)
//
// USBD interface information structure, this structure
// is returned for each interface opened thru an
// SELECT_CONFIGURATION or SELECT_INTERFACE request.
//

typedef struct _USBD_INTERFACE_INFORMATION {
    USHORT Length;       // Length of this structure, including
                         // all pipe information structures that
                         // follow.
    //
    // INPUT
    //
    // Interface number and Alternate setting this
    // structure is associated with
    //
    UCHAR InterfaceNumber;
    UCHAR AlternateSetting;
    
    //
    // OUTPUT
    // These fields are filled in by USBD
    //
    UCHAR Class;
    UCHAR SubClass;
    UCHAR Protocol;
    UCHAR Reserved;
    
    USBD_INTERFACE_HANDLE InterfaceHandle;
    ULONG NumberOfPipes; 

    //
    // INPUT/OUPUT
    // see PIPE_INFORMATION
    USBD_PIPE_INFORMATION Pipes[1];
} USBD_INTERFACE_INFORMATION, *PUSBD_INTERFACE_INFORMATION;

//
// work space provided for HCDs
//

struct _URB_HCD_AREA {
    PVOID Reserved8[8];
};

struct _URB_HEADER {
    //
    // Fields filled in by client driver
    //
    USHORT Length;
    USHORT Function;
    USBD_STATUS Status;
    //
    // Fields used only by USBD
    //
    PVOID UsbdDeviceHandle; // device handle assigned to this device
                            // by USBD
    ULONG UsbdFlags;        // flags field reserved for USBD use.
};

struct _URB_SELECT_INTERFACE {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;

    // client must input AlternateSetting & Interface Number
    // class driver returns interface and handle
    // for new alternate setting
    USBD_INTERFACE_INFORMATION Interface;
};

struct _URB_SELECT_CONFIGURATION {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    // NULL indicates to set the device
    // to the 'unconfigured' state
    // ie set to configuration 0
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor;
    USBD_CONFIGURATION_HANDLE ConfigurationHandle;
    USBD_INTERFACE_INFORMATION Interface;
};

//
// This structure used for ABORT_PIPE & RESET_PIPE
//

struct _URB_PIPE_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    ULONG Reserved;
};

//
// This structure used for
// TAKE_FRAME_LENGTH_CONTROL &
//        RELEASE_FRAME_LENGTH_CONTROL
//

struct _URB_FRAME_LENGTH_CONTROL {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
};

struct _URB_GET_FRAME_LENGTH {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    ULONG FrameLength;
    ULONG FrameNumber;
};

struct _URB_SET_FRAME_LENGTH {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    LONG FrameLengthDelta;
};

struct _URB_GET_CURRENT_FRAME_NUMBER {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    ULONG FrameNumber;
};

//
// Structures for specific control transfers
// on the default pipe.
//

// GET_DESCRIPTOR
// SET_DESCRIPTOR

struct _URB_CONTROL_DESCRIPTOR_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;               // fields for HCD use
    USHORT Reserved1;
    UCHAR Index;
    UCHAR DescriptorType;
    USHORT LanguageId;
    USHORT Reserved2;
};

// GET_STATUS

struct _URB_CONTROL_GET_STATUS_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[4];
    USHORT Index;                       // zero, interface or endpoint
    USHORT Reserved2;
};

// SET_FEATURE
// CLEAR_FEATURE

struct _URB_CONTROL_FEATURE_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved2;
    ULONG Reserved3;
    PVOID Reserved4;
    PMDL Reserved5;
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    USHORT Reserved0;
    USHORT FeatureSelector;
    USHORT Index;                       // zero, interface or endpoint
    USHORT Reserved1;
};

// VENDOR & CLASS

struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR RequestTypeReservedBits;
    UCHAR Request;
    USHORT Value;
    USHORT Index;
    USHORT Reserved1;
};


struct _URB_CONTROL_GET_INTERFACE_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[4];    
    USHORT Interface;
    USHORT Reserved2;
};


struct _URB_CONTROL_GET_CONFIGURATION_REQUEST {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR Reserved1[8];    
};

// Microsoft OS Descriptor APIs

#define OS_STRING_DESCRIPTOR_INDEX                  0xEE

#define MS_GENRE_DESCRIPTOR_INDEX                   0x0001
#define MS_POWER_DESCRIPTOR_INDEX                   0x0002

#define MS_OS_STRING_SIGNATURE                      L"MSFT100"

typedef struct _OS_STRING {
    UCHAR bLength;
    UCHAR bDescriptorType;
    WCHAR MicrosoftString[7];
    UCHAR bVendorCode;
    UCHAR bPad;
} OS_STRING, *POS_STRING;


struct _URB_OS_FEATURE_DESCRIPTOR_REQUEST {
    struct _URB_HEADER Hdr;  // function code indicates get or set.
    PVOID Reserved;
    ULONG Reserved0;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR   Recipient:5;                // Recipient {Device,Interface,Endpoint}
    UCHAR   Reserved1:3;
    UCHAR   Reserved2;
    UCHAR   InterfaceNumber;            // wValue - high byte
    UCHAR   MS_PageIndex;               // wValue - low byte
    USHORT  MS_FeatureDescriptorIndex;  // wIndex field
    USHORT  Reserved3;
};

//
// request format for a control transfer on
// the non-default pipe.
//

struct _URB_CONTROL_TRANSFER {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
    UCHAR SetupPacket[8];
};


struct _URB_BULK_OR_INTERRUPT_TRANSFER {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;                // note: the direction bit will be set by USBD
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use
};


//
// ISO Transfer request
//
// TransferBufferMDL must point to a single virtually 
// contiguous buffer.
//
// StartFrame - the frame to send/receive the first packet of 
// the request. 
//
// NumberOfPackets - number of packets to send in this request
//
//
// IsoPacket Array
//
//      Input:  Offset - offset of the packet from the beginig
//                 of the client buffer.
//      Output: Length -  is set to the actual length of the packet
//                (For IN transfers). 
//      Status: error that occurred during transmission or 
//              reception of the packet.
//      

typedef struct _USBD_ISO_PACKET_DESCRIPTOR {
    ULONG Offset;       // INPUT Offset of the packet from the begining of the
                        // buffer.

    ULONG Length;       // OUTPUT length of data received (for in).
                        // OUTPUT 0 for OUT.
    USBD_STATUS Status; // status code for this packet.     
} USBD_ISO_PACKET_DESCRIPTOR, *PUSBD_ISO_PACKET_DESCRIPTOR;

struct _URB_ISOCH_TRANSFER {
    //
    // This block is the same as CommonTransfer
    //
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    struct _URB *UrbLink;               // *optional* link to next urb request
                                        // if this is a chain of commands
    struct _URB_HCD_AREA hca;           // fields for HCD use

    //
    // this block contains transfer fields
    // specific to isochronous transfers
    //

    // 32 bit frame number to begin this transfer on, must be within 1000
    // frames of the current USB frame or an error is returned.

    // START_ISO_TRANSFER_ASAP flag in transferFlags:
    // If this flag is set and no transfers have been submitted
    // for the pipe then the transfer will begin on the next frame
    // and StartFrame will be updated with the frame number the transfer
    // was started on.
    // If this flag is set and the pipe has active transfers then 
    // the transfer will be queued to begin on the frame after the 
    // last transfer queued is completed.
    //
    ULONG StartFrame;
    // number of packets that make up this request
    ULONG NumberOfPackets;
    // number of packets that completed with errors
    ULONG ErrorCount;
    USBD_ISO_PACKET_DESCRIPTOR IsoPacket[1]; 
};

#if 0
//
// new for USB 2.0
// client is responsible for initailizing all fields 
// of the setup packet

// option URB timeout, if nonzero the request will be 
// timed out after the speified number of ms and completed
// with USBD_STATUS_TIMEOUT.

struct _URB_TIMEOUT {
    ULONG TimeoutValue;   // timeout in ms, 0 = no timeout
#ifdef WIN64
    ULONG Pad1;
#endif
} URB_TIMEOUT, *PURB_TIMEOUT;

struct _URB_RAW_CONTROL_TRANSFER {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    ULONG TransferFlags;
    ULONG TransferBufferLength;
    PVOID TransferBuffer;
    PMDL TransferBufferMDL;             // *optional*
    PVOID Reservedxxx;

    PVOID HcdTransferContext;           // usbport context
    URB_TIMEOUT UrbTimeout;   
    PVOID Reservedxxx[5];
    struct _URB_HCD_AREA hcaXXX;           

    USB_DEFAULT_PIPE_SETUP_PACKET SetupPacket;
};

//optption flags
//
// option_1
// 'safe abort' stall abort if data is data is bieing received for 
// the endpoint
XXX_OPTION_1
// option_2
// enable 'throttleing' on this endpoint bulk_opt_2
XXX_OPTION_2
// option_3
// enable short packet/small transfer optimization bulk_opt_1
XXX_OPTION_3
// option_4
// enable 'irpless' interface for this endpoint 


struct _URB_SET_PIPE_PARAMETERS {
    struct _URB_HEADER Hdr;                 
    USBD_PIPE_HANDLE PipeHandle;
    
    // This is the maximum thruput of the device
    // under optimal conditions 
    // ie if a device can ony supply or receive 64 bytes/ms 
    // then set this value to 64.
    // this is value is used by the operating system to 
    // optimize the scheduling of transfers.
    // It will default to the maximum for the endpoint type
    ULONG  DeviceMaxThruput;                

    // Max Packet size for the endpoint this value defaults to
    // the endpoint supplied value and may be subsequently changed 
    // by the driver. 
    // This parameter may be used to dymically adjust the packet 
    // size of an endpoint without re-configuring the device or
    // interface.
    USHORT MaximumPacketSize;
};

struct _URB_GET_PIPE_PARAMETERS {
    struct _URB_HEADER Hdr;                 // function code indicates get or set.
    USBD_PIPE_HANDLE PipeHandle;
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
};
#endif



typedef struct _URB {
    union {
        struct _URB_HEADER                           
            UrbHeader;
        struct _URB_SELECT_INTERFACE                 
            UrbSelectInterface;
        struct _URB_SELECT_CONFIGURATION             
            UrbSelectConfiguration;
        struct _URB_PIPE_REQUEST                     
            UrbPipeRequest;
        struct _URB_FRAME_LENGTH_CONTROL             
            UrbFrameLengthControl;
        struct _URB_GET_FRAME_LENGTH                 
            UrbGetFrameLength;
        struct _URB_SET_FRAME_LENGTH                 
            UrbSetFrameLength;
        struct _URB_GET_CURRENT_FRAME_NUMBER         
            UrbGetCurrentFrameNumber;
        struct _URB_CONTROL_TRANSFER                 
            UrbControlTransfer;
        struct _URB_BULK_OR_INTERRUPT_TRANSFER       
            UrbBulkOrInterruptTransfer;
        struct _URB_ISOCH_TRANSFER                   
            UrbIsochronousTransfer;

        // for standard control transfers on the default pipe
        struct _URB_CONTROL_DESCRIPTOR_REQUEST       
            UrbControlDescriptorRequest;
        struct _URB_CONTROL_GET_STATUS_REQUEST       
            UrbControlGetStatusRequest;
        struct _URB_CONTROL_FEATURE_REQUEST          
            UrbControlFeatureRequest;
        struct _URB_CONTROL_VENDOR_OR_CLASS_REQUEST  
            UrbControlVendorClassRequest;
        struct _URB_CONTROL_GET_INTERFACE_REQUEST    
            UrbControlGetInterfaceRequest;
        struct _URB_CONTROL_GET_CONFIGURATION_REQUEST 
            UrbControlGetConfigurationRequest;
        struct _URB_OS_FEATURE_DESCRIPTOR_REQUEST     
            UrbOSFeatureDescriptorRequest;
    };
} URB, *PURB;


#endif /*  __USB_H__ */

