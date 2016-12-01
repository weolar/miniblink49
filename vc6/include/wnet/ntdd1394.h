/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntdd1394.h

Abstract:

    Definitions for the 1394 api

Author:

    George Chrysanthakopoulos (georgioc) 4/26/99

Environment:

    Kernel mode only

Revision History:


--*/

#ifndef _NTDD1394_H_
#define _NTDD1394_H_

#if (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// registry definitions
//

#define BUS1394_VIRTUAL_DEVICE_LIST_KEY     L"Virtual Device List"
#define BUS1394_LOCAL_HOST_INSTANCE_KEY     L"LOCAL HOST EUI64"


//
// Various definitions
//

#define IOCTL_IEEE1394_API_REQUEST                  CTL_CODE( \
                                                FILE_DEVICE_UNKNOWN, \
                                                0x100, \
                                                METHOD_BUFFERED, \
                                                FILE_ANY_ACCESS \
                                                )

//
// IEEE 1394 Sbp2 Request packet.  It is how other
// device drivers communicate with the 1sbp2 trasnport.
//

typedef struct _IEEE1394_VDEV_PNP_REQUEST{

    ULONG fulFlags;
    ULONG Reserved;
    ULARGE_INTEGER InstanceId;
    UCHAR DeviceId;

} IEEE1394_VDEV_PNP_REQUEST,*PIEEE1394_VDEV_PNP_REQUEST;


typedef struct _IEEE1394_API_REQUEST {

    //
    // Holds the zero based Function number that corresponds to the request
    // that device drivers are asking the sbp2 port driver to carry out.
    //

    ULONG RequestNumber;

    //
    // Holds Flags that may be unique to this particular operation
    //

    ULONG Flags;

    //
    // Holds the structures used in performing the various 1394 APIs
    //

    union {

        IEEE1394_VDEV_PNP_REQUEST AddVirtualDevice;
        IEEE1394_VDEV_PNP_REQUEST RemoveVirtualDevice;

    } u;

} IEEE1394_API_REQUEST, *PIEEE1394_API_REQUEST;

//
// Request Number
//

#define IEEE1394_API_ADD_VIRTUAL_DEVICE             0x00000001
#define IEEE1394_API_REMOVE_VIRTUAL_DEVICE          0x00000002

//
// flags for the add/remove requests
//

#define IEEE1394_REQUEST_FLAG_UNICODE       0x00000001
#define IEEE1394_REQUEST_FLAG_PERSISTENT    0x00000002
#define IEEE1394_REQUEST_FLAG_USE_LOCAL_HOST_EUI        0x00000004

//
// definitions for the access/ownership 1394 scheme
//

#ifdef __cplusplus
}
#endif

#endif      // _NTDD1394_H_

