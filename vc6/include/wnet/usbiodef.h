/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

        USBIODEF.H

Abstract:

        Common header file for all USB IOCTLs defined for
        the core stack.  We define them in this single header file
        so that we can maintain backward compatibilty with older
        versions of the stack.


        We divide the IOCTLS supported by the stack as follows:

        kernel IOCTLS:


        user IOCTLS:

            IOCTLS Handled by HCD (PORT) FDO
            IOCTLS Handled by HUB FDO
            IOCTLS Handled by USB (DEVICE) PDO

Environment:

    kernel & user mode

Revision History:


--*/

#ifndef   __USBIODEF_H__
#define   __USBIODEF_H__

/*
    kernel mode IOCTL index values

    The following codes are valid only if passed as in
    the icControlCode parameter for
    IRP_MJ_INTERNAL_DEVICE_CONTROL

*/
#define USB_SUBMIT_URB              0
#define USB_RESET_PORT              1
#define USB_GET_ROOTHUB_PDO         3
#define USB_GET_PORT_STATUS         4
#define USB_ENABLE_PORT             5
#define USB_GET_HUB_COUNT           6
#define USB_CYCLE_PORT              7
#define USB_GET_HUB_NAME            8
#define USB_IDLE_NOTIFICATION       9
#define USB_GET_BUS_INFO            264
#define USB_GET_CONTROLLER_NAME     265
#define USB_GET_BUSGUID_INFO        266
#define USB_GET_PARENT_HUB_INFO     267
#define USB_GET_DEVICE_HANDLE       268


/*
    user mode IOCTL index values

    The following codes are valid only if passed as in
    the icControlCode parameter for
    IRP_MJ_DEVICE_CONTROL
    hence, they are callable by user mode applications
*/
#define HCD_GET_STATS_1                     255
#define HCD_DIAGNOSTIC_MODE_ON              256
#define HCD_DIAGNOSTIC_MODE_OFF             257
#define HCD_GET_ROOT_HUB_NAME               258
#define HCD_GET_DRIVERKEY_NAME              265
#define HCD_GET_STATS_2                     266
#define HCD_DISABLE_PORT                    268
#define HCD_ENABLE_PORT                     269
#define HCD_USER_REQUEST                    270

#define USB_GET_NODE_INFORMATION                    258
#define USB_GET_NODE_CONNECTION_INFORMATION         259
#define USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION     260
#define USB_GET_NODE_CONNECTION_NAME                261
#define USB_DIAG_IGNORE_HUBS_ON                     262
#define USB_DIAG_IGNORE_HUBS_OFF                    263
#define USB_GET_NODE_CONNECTION_DRIVERKEY_NAME      264
#define USB_GET_HUB_CAPABILITIES                    271
#define USB_GET_NODE_CONNECTION_ATTRIBUTES          272
#define	USB_HUB_CYCLE_PORT							273
#define USB_GET_NODE_CONNECTION_INFORMATION_EX      274

/*
USB specific GUIDs
*/


/* f18a0e88-c30c-11d0-8815-00a0c906bed8 */
DEFINE_GUID(GUID_DEVINTERFACE_USB_HUB,    0xf18a0e88, 0xc30c, 0x11d0, 0x88, 0x15, 0x00, \
             0xa0, 0xc9, 0x06, 0xbe, 0xd8);

/* A5DCBF10-6530-11D2-901F-00C04FB951ED */
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, \
             0xC0, 0x4F, 0xB9, 0x51, 0xED);

/* 3ABF6F2D-71C4-462a-8A92-1E6861E6AF27 */
DEFINE_GUID(GUID_DEVINTERFACE_USB_HOST_CONTROLLER, 0x3abf6f2d, 0x71c4, 0x462a, 0x8a, 0x92, 0x1e, \
             0x68, 0x61, 0xe6, 0xaf, 0x27);
             
/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
DEFINE_GUID(GUID_USB_WMI_STD_DATA, 0x4E623B20L, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00,\
             0xA0, 0xC9, 0x59, 0xBB, 0xD2);

/* 4E623B20-CB14-11D1-B331-00A0C959BBD2 */
DEFINE_GUID(GUID_USB_WMI_STD_NOTIFICATION, 0x4E623B20L, 0xCB14, 0x11D1, 0xB3, 0x31, 0x00,\
             0xA0, 0xC9, 0x59, 0xBB, 0xD2);        


/*
Obsolete device interface class GUID names.
(use of above GUID_DEVINTERFACE_* names is recommended).
--*/

#define GUID_CLASS_USBHUB               GUID_DEVINTERFACE_USB_HUB
#define GUID_CLASS_USB_DEVICE           GUID_DEVINTERFACE_USB_DEVICE
#define GUID_CLASS_USB_HOST_CONTROLLER  GUID_DEVINTERFACE_USB_HOST_CONTROLLER

#define FILE_DEVICE_USB         FILE_DEVICE_UNKNOWN

/*
    common macro used by IOCTL header files
*/
#define USB_CTL(id)  CTL_CODE(FILE_DEVICE_USB,  \
                                      (id), \
                                      METHOD_BUFFERED,  \
                                      FILE_ANY_ACCESS)

#define USB_KERNEL_CTL(id)  CTL_CODE(FILE_DEVICE_USB,  \
                                      (id), \
                                      METHOD_NEITHER,  \
                                      FILE_ANY_ACCESS)                                      

#endif //__USBIODEF_H__

