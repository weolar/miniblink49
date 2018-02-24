/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ksproxy.h

Abstract:

    Interface definitions for WDM-CSA proxy filters.

--*/

#ifndef __KSPROXY__
#define __KSPROXY__

#ifdef __cplusplus
extern "C" {
#endif

#undef KSDDKAPI
#ifdef _KSDDK_
#define KSDDKAPI
#else // !_KSDDK_
#define KSDDKAPI DECLSPEC_IMPORT
#endif // _KSDDK_

#define STATIC_IID_IKsObject\
    0x423c13a2L, 0x2070, 0x11d0, 0x9e, 0xf7, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1

#define STATIC_IID_IKsPinEx\
    0x7bb38260L, 0xd19c, 0x11d2, 0xb3, 0x8a, 0x00, 0xa0, 0xc9, 0x5e, 0xc2, 0x2e

#define STATIC_IID_IKsPin\
    0xb61178d1L, 0xa2d9, 0x11cf, 0x9e, 0x53, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1
    
#define STATIC_IID_IKsPinPipe\
    0xe539cd90L, 0xa8b4, 0x11d1, 0x81, 0x89, 0x00, 0xa0, 0xc9, 0x06, 0x28, 0x02

#define STATIC_IID_IKsDataTypeHandler\
    0x5ffbaa02L, 0x49a3, 0x11d0, 0x9f, 0x36, 0x00, 0xaa, 0x00, 0xa2, 0x16, 0xa1

#define STATIC_IID_IKsDataTypeCompletion\
    0x827D1A0EL, 0x0F73, 0x11D2, 0xB2, 0x7A, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_IID_IKsInterfaceHandler\
    0xD3ABC7E0L, 0x9A61, 0x11D0, 0xA4, 0x0D, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_IID_IKsClockPropertySet\
    0x5C5CBD84L, 0xE755, 0x11D0, 0xAC, 0x18, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_IID_IKsAllocator\
    0x8da64899L, 0xc0d9, 0x11d0, 0x84, 0x13, 0x00, 0x00, 0xf8, 0x22, 0xfe, 0x8a
    
#define STATIC_IID_IKsAllocatorEx\
    0x091bb63aL, 0x603f, 0x11d1, 0xb0, 0x67, 0x00, 0xa0, 0xc9, 0x06, 0x28, 0x02
    

#ifndef STATIC_IID_IKsPropertySet
#define STATIC_IID_IKsPropertySet\
    0x31EFAC30L, 0x515C, 0x11d0, 0xA9, 0xAA, 0x00, 0xAA, 0x00, 0x61, 0xBE, 0x93
#endif // STATIC_IID_IKsPropertySet

#define STATIC_IID_IKsTopology\
    0x28F54683L, 0x06FD, 0x11D2, 0xB2, 0x7A, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#ifndef STATIC_IID_IKsControl
#define STATIC_IID_IKsControl\
    0x28F54685L, 0x06FD, 0x11D2, 0xB2, 0x7A, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96
#endif // STATIC_IID_IKsControl

#define STATIC_IID_IKsAggregateControl\
    0x7F40EAC0L, 0x3947, 0x11D2, 0x87, 0x4E, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#define STATIC_CLSID_Proxy \
    0x17CCA71BL, 0xECD7, 0x11D0, 0xB9, 0x08, 0x00, 0xA0, 0xC9, 0x22, 0x31, 0x96

#ifdef _KS_

#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsObject);

DEFINE_GUIDEX(IID_IKsPin);

DEFINE_GUIDEX(IID_IKsPinEx);

DEFINE_GUIDEX(IID_IKsPinPipe);

DEFINE_GUIDEX(IID_IKsDataTypeHandler);

DEFINE_GUIDEX(IID_IKsDataTypeCompletion);

DEFINE_GUIDEX(IID_IKsInterfaceHandler);

DEFINE_GUIDEX(IID_IKsClockPropertySet);

DEFINE_GUIDEX(IID_IKsAllocator);

DEFINE_GUIDEX(IID_IKsAllocatorEx);

#define IID_IKsQualityForwarder KSCATEGORY_QUALITY
#endif // !defined(__cplusplus) || _MSC_VER < 1100

#define STATIC_IID_IKsQualityForwarder STATIC_KSCATEGORY_QUALITY

typedef enum {
    KsAllocatorMode_User,
    KsAllocatorMode_Kernel
} KSALLOCATORMODE;


typedef enum {
    FramingProp_Uninitialized,
    FramingProp_None,
    FramingProp_Old,
    FramingProp_Ex
} FRAMING_PROP;

typedef FRAMING_PROP *PFRAMING_PROP;


typedef enum {
    Framing_Cache_Update,     // request to bypass cache when read/write
    Framing_Cache_ReadLast,
    Framing_Cache_ReadOrig,
    Framing_Cache_Write
} FRAMING_CACHE_OPS;


typedef struct {
    LONGLONG     MinTotalNominator;  
    LONGLONG     MaxTotalNominator;
    LONGLONG     TotalDenominator;
} OPTIMAL_WEIGHT_TOTALS;

// forward declaration
typedef struct IPin IPin;
typedef struct IKsPin IKsPin;
typedef struct IKsAllocator IKsAllocator;
typedef struct IKsAllocatorEx IKsAllocatorEx;

                      
//
// allocators strategy is defined by graph manager
//
#define AllocatorStrategy_DontCare                      0

//
// what to optimize
//
#define AllocatorStrategy_MinimizeNumberOfFrames        0x00000001
#define AllocatorStrategy_MinimizeFrameSize             0x00000002
#define AllocatorStrategy_MinimizeNumberOfAllocators    0x00000004
#define AllocatorStrategy_MaximizeSpeed                 0x00000008 

//
// factors (flags) defining the Pipes properties
//
#define PipeFactor_None                   0
#define PipeFactor_UserModeUpstream       0x00000001
#define PipeFactor_UserModeDownstream     0x00000002
#define PipeFactor_MemoryTypes            0x00000004
#define PipeFactor_Flags                  0x00000008
#define PipeFactor_PhysicalRanges         0x00000010
#define PipeFactor_OptimalRanges          0x00000020
#define PipeFactor_FixedCompression       0x00000040
#define PipeFactor_UnknownCompression     0x00000080

#define PipeFactor_Buffers                0x00000100
#define PipeFactor_Align                  0x00000200

#define PipeFactor_PhysicalEnd            0x00000400
#define PipeFactor_LogicalEnd             0x00000800



typedef enum {
    PipeState_DontCare,
    PipeState_RangeNotFixed,
    PipeState_RangeFixed,
    PipeState_CompressionUnknown,
    PipeState_Finalized
} PIPE_STATE;


//
// pipe dimensions relative to BeginPin.
//

typedef struct _PIPE_DIMENSIONS {
    KS_COMPRESSION    AllocatorPin;
    KS_COMPRESSION    MaxExpansionPin;
    KS_COMPRESSION    EndPin;
} PIPE_DIMENSIONS, *PPIPE_DIMENSIONS;


typedef enum {
    Pipe_Allocator_None,
    Pipe_Allocator_FirstPin,
    Pipe_Allocator_LastPin,
    Pipe_Allocator_MiddlePin
} PIPE_ALLOCATOR_PLACE;

typedef PIPE_ALLOCATOR_PLACE  *PPIPE_ALLOCATOR_PLACE;


typedef enum {
    KS_MemoryTypeDontCare = 0,
    KS_MemoryTypeKernelPaged,
    KS_MemoryTypeKernelNonPaged,
    KS_MemoryTypeDeviceHostMapped,
    KS_MemoryTypeDeviceSpecific,
    KS_MemoryTypeUser,
    KS_MemoryTypeAnyHost
} KS_LogicalMemoryType;

typedef KS_LogicalMemoryType  *PKS_LogicalMemoryType;



typedef struct _PIPE_TERMINATION {
    ULONG                       Flags;
    ULONG                       OutsideFactors;
    ULONG                       Weigth;            // outside weight
    KS_FRAMING_RANGE            PhysicalRange;
    KS_FRAMING_RANGE_WEIGHTED   OptimalRange;      
    KS_COMPRESSION              Compression;       // relative to the connected pin on a neighboring filter.
} PIPE_TERMINATION;


//
// extended allocator properties 
//
typedef struct _ALLOCATOR_PROPERTIES_EX
    {
    long cBuffers;
    long cbBuffer;
    long cbAlign;
    long cbPrefix;
// new part
    GUID                       MemoryType;         
    GUID                       BusType;            // one of the buses this pipe is using
    PIPE_STATE                 State;                        
    PIPE_TERMINATION           Input;                        
    PIPE_TERMINATION           Output;                       
    ULONG                      Strategy;
    ULONG                      Flags;              
    ULONG                      Weight;
    KS_LogicalMemoryType       LogicalMemoryType;
    PIPE_ALLOCATOR_PLACE       AllocatorPlace;
    PIPE_DIMENSIONS            Dimensions;
    KS_FRAMING_RANGE           PhysicalRange;      // on allocator pin
    IKsAllocatorEx*            PrevSegment;        // doubly-linked list of KS allocators
    ULONG                      CountNextSegments;  // possible multiple dependent pipes
    IKsAllocatorEx**           NextSegments;
    ULONG                      InsideFactors;      // existing factors (different from "don't care")
    ULONG                      NumberPins;                   
} ALLOCATOR_PROPERTIES_EX;

typedef ALLOCATOR_PROPERTIES_EX *PALLOCATOR_PROPERTIES_EX;


#ifdef __STREAMS__

struct DECLSPEC_UUID("5C5CBD84-E755-11D0-AC18-00A0C9223196") IKsClockPropertySet;
#undef INTERFACE
#define INTERFACE IKsClockPropertySet
DECLARE_INTERFACE_(IKsClockPropertySet, IUnknown)
{
    STDMETHOD(KsGetTime)(
        THIS_
        LONGLONG* Time
    ) = 0;
    STDMETHOD(KsSetTime)(
        THIS_
        LONGLONG Time
    ) = 0;
    STDMETHOD(KsGetPhysicalTime)(
        THIS_
        LONGLONG* Time
    ) = 0;
    STDMETHOD(KsSetPhysicalTime)(
        THIS_
        LONGLONG Time
    ) = 0;
    STDMETHOD(KsGetCorrelatedTime)(
        THIS_
        KSCORRELATED_TIME* CorrelatedTime
    ) = 0;
    STDMETHOD(KsSetCorrelatedTime)(
        THIS_
        KSCORRELATED_TIME* CorrelatedTime
    ) = 0;
    STDMETHOD(KsGetCorrelatedPhysicalTime)(
        THIS_
        KSCORRELATED_TIME* CorrelatedTime
    ) = 0;
    STDMETHOD(KsSetCorrelatedPhysicalTime)(
        THIS_
        KSCORRELATED_TIME* CorrelatedTime
    ) = 0;
    STDMETHOD(KsGetResolution)(
        THIS_
        KSRESOLUTION* Resolution
    ) = 0;
    STDMETHOD(KsGetState)(
        THIS_
        KSSTATE* State
    ) = 0;
};


interface DECLSPEC_UUID("8da64899-c0d9-11d0-8413-0000f822fe8a") IKsAllocator;
#undef INTERFACE
#define INTERFACE IKsAllocator
DECLARE_INTERFACE_(IKsAllocator, IUnknown)
{
    STDMETHOD_(HANDLE, KsGetAllocatorHandle)(
        THIS
    ) = 0;
    STDMETHOD_(KSALLOCATORMODE, KsGetAllocatorMode)(
        THIS
    ) = 0;
    STDMETHOD(KsGetAllocatorStatus)(
        THIS_
        PKSSTREAMALLOCATOR_STATUS AllocatorStatus
    ) = 0;
    STDMETHOD_(VOID, KsSetAllocatorMode)(
        THIS_
        KSALLOCATORMODE Mode
    ) = 0;
};

interface DECLSPEC_UUID("091bb63a-603f-11d1-b067-00a0c9062802") IKsAllocatorEx;
#undef INTERFACE
#define INTERFACE IKsAllocatorEx
DECLARE_INTERFACE_(IKsAllocatorEx, IKsAllocator)
{
    STDMETHOD_(PALLOCATOR_PROPERTIES_EX, KsGetProperties)(
        THIS
    ) = 0;
    STDMETHOD_(VOID, KsSetProperties)(
        THIS_
        PALLOCATOR_PROPERTIES_EX 
    ) = 0;
    STDMETHOD_(VOID, KsSetAllocatorHandle)(
        THIS_
        HANDLE AllocatorHandle
    ) = 0;
    STDMETHOD_(HANDLE, KsCreateAllocatorAndGetHandle)(
        THIS_
        IKsPin*   KsPin
    ) = 0;
};  

typedef enum {
    KsPeekOperation_PeekOnly,
    KsPeekOperation_AddRef
} KSPEEKOPERATION;

typedef struct _KSSTREAM_SEGMENT *PKSSTREAM_SEGMENT;

interface DECLSPEC_UUID("b61178d1-a2d9-11cf-9e53-00aa00a216a1") IKsPin;

#undef INTERFACE
#define INTERFACE IKsPin
DECLARE_INTERFACE_(IKsPin, IUnknown)
{
    STDMETHOD(KsQueryMediums)(
        THIS_
        PKSMULTIPLE_ITEM* MediumList
    ) = 0;
    STDMETHOD(KsQueryInterfaces)(
        THIS_
        PKSMULTIPLE_ITEM* InterfaceList
    ) = 0;
    STDMETHOD(KsCreateSinkPinHandle)(
        THIS_
        KSPIN_INTERFACE& Interface,
        KSPIN_MEDIUM& Medium
    ) = 0;
    STDMETHOD(KsGetCurrentCommunication)(
        THIS_
        KSPIN_COMMUNICATION *Communication,
        KSPIN_INTERFACE *Interface,
        KSPIN_MEDIUM *Medium
    ) = 0;
    STDMETHOD(KsPropagateAcquire)(
        THIS
    ) = 0;
    STDMETHOD(KsDeliver)(
        THIS_
        IMediaSample* Sample,
        ULONG Flags
    ) = 0;
    STDMETHOD(KsMediaSamplesCompleted)(
        THIS_
        PKSSTREAM_SEGMENT StreamSegment
    ) = 0;
    STDMETHOD_(IMemAllocator *, KsPeekAllocator)(
        THIS_
        KSPEEKOPERATION Operation
    ) = 0;
    STDMETHOD(KsReceiveAllocator)(
        THIS_
        IMemAllocator *MemAllocator
    ) = 0;
    STDMETHOD(KsRenegotiateAllocator)(
        THIS
    ) = 0;
    STDMETHOD_(LONG, KsIncrementPendingIoCount)(
        THIS
    ) = 0;
    STDMETHOD_(LONG, KsDecrementPendingIoCount)(
        THIS
    ) = 0;
    STDMETHOD(KsQualityNotify)(
        THIS_
        ULONG Proportion,
        REFERENCE_TIME TimeDelta
    ) = 0;
};

interface DECLSPEC_UUID("7bb38260-d19c-11d2-b38a-00a0c95ec22e") IKsPinEx;
#undef INTERFACE
#define INTERFACE IKsPinEx
DECLARE_INTERFACE_(IKsPinEx, IKsPin)
{
    STDMETHOD_(VOID, KsNotifyError)(
        THIS_
        IMediaSample* Sample,
        HRESULT hr
    ) = 0;        
};
                         
interface DECLSPEC_UUID("e539cd90-a8b4-11d1-8189-00a0c9062802") IKsPinPipe;
#undef INTERFACE
#define INTERFACE IKsPinPipe
DECLARE_INTERFACE_(IKsPinPipe, IUnknown)
{
    STDMETHOD(KsGetPinFramingCache)(
        THIS_
        PKSALLOCATOR_FRAMING_EX *FramingEx,
        PFRAMING_PROP FramingProp,
        FRAMING_CACHE_OPS Option
    ) = 0;
    STDMETHOD(KsSetPinFramingCache)(
        THIS_
        PKSALLOCATOR_FRAMING_EX FramingEx,
        PFRAMING_PROP FramingProp,
        FRAMING_CACHE_OPS Option
    ) = 0;
    STDMETHOD_(IPin*, KsGetConnectedPin)(
        THIS
    ) = 0;
    STDMETHOD_(IKsAllocatorEx*, KsGetPipe)(
        THIS_
        KSPEEKOPERATION Operation
    ) = 0;
    STDMETHOD(KsSetPipe)(
        THIS_
        IKsAllocatorEx *KsAllocator
    ) = 0;
    STDMETHOD_(ULONG, KsGetPipeAllocatorFlag)(
        THIS
    ) = 0;
    STDMETHOD(KsSetPipeAllocatorFlag)(
        THIS_
        ULONG Flag
    ) = 0;
    STDMETHOD_(GUID, KsGetPinBusCache)(
        THIS
    ) = 0;
    STDMETHOD(KsSetPinBusCache)(
        THIS_
        GUID Bus
    ) = 0;
//
// very useful methods for tracing.
//
    STDMETHOD_(PWCHAR, KsGetPinName)(
        THIS
    ) = 0;
    STDMETHOD_(PWCHAR, KsGetFilterName)(
        THIS
    ) = 0;
};


interface DECLSPEC_UUID("CD5EBE6B-8B6E-11D1-8AE0-00A0C9223196") IKsPinFactory;
#undef INTERFACE
#define INTERFACE IKsPinFactory
DECLARE_INTERFACE_(IKsPinFactory, IUnknown)
{
    STDMETHOD(KsPinFactory)(
        THIS_
        ULONG* PinFactory
    ) = 0;
};

typedef enum {
    KsIoOperation_Write,
    KsIoOperation_Read
} KSIOOPERATION;

interface DECLSPEC_UUID("5ffbaa02-49a3-11d0-9f36-00aa00a216a1") IKsDataTypeHandler;
#undef INTERFACE
#define INTERFACE IKsDataTypeHandler
DECLARE_INTERFACE_(IKsDataTypeHandler, IUnknown)
{
    STDMETHOD(KsCompleteIoOperation)(
        THIS_
        IMediaSample *Sample,
        PVOID StreamHeader,
        KSIOOPERATION IoOperation,
        BOOL Cancelled
    ) = 0;
    STDMETHOD(KsIsMediaTypeInRanges)(
        THIS_
        PVOID DataRanges
        ) = 0;
    STDMETHOD(KsPrepareIoOperation)(
        THIS_
        IMediaSample *Sample,
        PVOID StreamHeader,
        KSIOOPERATION IoOperation
    ) = 0;
    STDMETHOD(KsQueryExtendedSize)(
        THIS_
        ULONG* ExtendedSize
    ) = 0;
    STDMETHOD(KsSetMediaType)(
        THIS_
        const AM_MEDIA_TYPE* AmMediaType
    ) = 0;
};

interface DECLSPEC_UUID("827D1A0E-0F73-11D2-B27A-00A0C9223196") IKsDataTypeCompletion;
#undef INTERFACE
#define INTERFACE IKsDataTypeCompletion
DECLARE_INTERFACE_(IKsDataTypeCompletion, IUnknown)
{
    STDMETHOD(KsCompleteMediaType)(
        THIS_
        HANDLE FilterHandle,
        ULONG PinFactoryId,
        AM_MEDIA_TYPE* AmMediaType
    ) = 0;
};

interface DECLSPEC_UUID("D3ABC7E0-9A61-11d0-A40D-00A0C9223196") IKsInterfaceHandler;
#undef INTERFACE
#define INTERFACE IKsInterfaceHandler
DECLARE_INTERFACE_(IKsInterfaceHandler, IUnknown)
{
    STDMETHOD(KsSetPin)(
        THIS_
        IKsPin *KsPin
    ) = 0;
    STDMETHOD(KsProcessMediaSamples)(
        THIS_
        IKsDataTypeHandler *KsDataTypeHandler,
        IMediaSample** SampleList,
        PLONG SampleCount,
        KSIOOPERATION IoOperation,
        PKSSTREAM_SEGMENT *StreamSegment
    ) = 0;
    STDMETHOD(KsCompleteIo)(
        THIS_
        PKSSTREAM_SEGMENT StreamSegment
    ) = 0;
};


//
// This structure definition is the common header required by the proxy to 
// dispatch the stream segment to the interface handler.  Interface handlers 
// will create extended structures to include other information such as 
// media samples, extended header size and so on.
//

typedef struct _KSSTREAM_SEGMENT {
    IKsInterfaceHandler     *KsInterfaceHandler;
    IKsDataTypeHandler      *KsDataTypeHandler;
    KSIOOPERATION           IoOperation;
    HANDLE                  CompletionEvent;
    
} KSSTREAM_SEGMENT;

interface DECLSPEC_UUID("423c13a2-2070-11d0-9ef7-00aa00a216a1") IKsObject;
#undef INTERFACE
#define INTERFACE IKsObject
DECLARE_INTERFACE_(IKsObject, IUnknown)
{
    STDMETHOD_(HANDLE, KsGetObjectHandle)(
        THIS
    ) = 0;
};

interface DECLSPEC_UUID("97ebaacb-95bd-11d0-a3ea-00a0c9223196") IKsQualityForwarder;
#undef INTERFACE
#define INTERFACE IKsQualityForwarder
DECLARE_INTERFACE_(IKsQualityForwarder, IKsObject)
{
    STDMETHOD_(VOID, KsFlushClient)(
        THIS_
        IKsPin* Pin
    ) = 0;
};

interface DECLSPEC_UUID("412bd695-f84b-46c1-ac73-54196dbc8fa7") IKsNotifyEvent;
#undef INTERFACE
#define INTERFACE IKsNotifyEvent
DECLARE_INTERFACE_(IKsNotifyEvent, IUnknown)
{
    STDMETHOD(KsNotifyEvent)(
        THIS_
        ULONG Event,
        ULONG_PTR lParam1,
        ULONG_PTR lParam2
    ) = 0;
};

KSDDKAPI
HRESULT
WINAPI
KsResolveRequiredAttributes(
    PKSDATARANGE DataRange,
    PKSMULTIPLE_ITEM Attributes OPTIONAL
    );

KSDDKAPI
HRESULT
WINAPI
KsOpenDefaultDevice(
    REFGUID Category,
    ACCESS_MASK Access,
    PHANDLE DeviceHandle
    );
KSDDKAPI
HRESULT
WINAPI
KsSynchronousDeviceControl(
    HANDLE      Handle,
    ULONG       IoControl,
    PVOID       InBuffer,
    ULONG       InLength,
    PVOID       OutBuffer,
    ULONG       OutLength,
    PULONG      BytesReturned
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMultiplePinFactoryItems(
    HANDLE  FilterHandle,
    ULONG   PinFactoryId,
    ULONG   PropertyId,
    PVOID*  Items
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMediaTypeCount(
    HANDLE      FilterHandle,
    ULONG       PinFactoryId,
    ULONG*      MediaTypeCount
    );
KSDDKAPI
HRESULT
WINAPI
KsGetMediaType(
    int         Position,
    AM_MEDIA_TYPE* AmMediaType,
    HANDLE      FilterHandle,
    ULONG       PinFactoryId
    );

#endif // __STREAMS__

#ifndef _IKsPropertySet_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsPropertySet);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsPropertySet_

#ifndef _IKsControl_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsControl);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsControl_

#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsAggregateControl);
#endif // !defined(__cplusplus) || _MSC_VER < 1100

#ifndef _IKsTopology_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUIDEX(IID_IKsTopology);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsTopology_

DEFINE_GUIDSTRUCT("17CCA71B-ECD7-11D0-B908-00A0C9223196", CLSID_Proxy);
#define CLSID_Proxy DEFINE_GUIDNAMED(CLSID_Proxy)

#else // !_KS_

#ifndef _IKsPropertySet_
#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUID(IID_IKsPropertySet, STATIC_IID_IKsPropertySet);
#endif // !defined(__cplusplus) || _MSC_VER < 1100
#endif // _IKsPropertySet_

#if !defined(__cplusplus) || _MSC_VER < 1100
DEFINE_GUID(CLSID_Proxy, STATIC_CLSID_Proxy);
#else  // defined(__cplusplus) && _MSC_VER >= 1100
DECLSPEC_UUID("17CCA71B-ECD7-11D0-B908-00A0C9223196") CLSID_Proxy;
#endif  // defined(__cplusplus) && _MSC_VER >= 1100

#endif // !_KS_

#ifndef _IKsPropertySet_
#define _IKsPropertySet_

#define KSPROPERTY_SUPPORT_GET 1
#define KSPROPERTY_SUPPORT_SET 2

#ifdef DECLARE_INTERFACE_

interface DECLSPEC_UUID("31EFAC30-515C-11d0-A9AA-00aa0061be93") IKsPropertySet;
#undef INTERFACE
#define INTERFACE IKsPropertySet
DECLARE_INTERFACE_(IKsPropertySet, IUnknown)
{
    STDMETHOD(Set)(
        THIS_
        IN REFGUID PropSet,
        IN ULONG Id,
        IN LPVOID InstanceData,
        IN ULONG InstanceLength,
        IN LPVOID PropertyData,
        IN ULONG DataLength
    ) = 0;

    STDMETHOD(Get)(
        THIS_
        IN REFGUID PropSet,
        IN ULONG Id,
        IN LPVOID InstanceData,
        IN ULONG InstanceLength,
        OUT LPVOID PropertyData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
    ) = 0;

    STDMETHOD(QuerySupported)(
        THIS_
        IN REFGUID PropSet,
        IN ULONG Id,
        OUT ULONG* TypeSupport
    ) = 0;
};

#endif // DECLARE_INTERFACE_

#endif // _IKsPropertySet_

#ifndef _IKsControl_
#define _IKsControl_

#ifdef DECLARE_INTERFACE_

interface DECLSPEC_UUID("28F54685-06FD-11D2-B27A-00A0C9223196") IKsControl;
#undef INTERFACE
#define INTERFACE IKsControl
DECLARE_INTERFACE_(IKsControl, IUnknown)
{
    STDMETHOD(KsProperty)(
        THIS_
        IN PKSPROPERTY Property,
        IN ULONG PropertyLength,
        IN OUT LPVOID PropertyData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
    ) = 0;
    STDMETHOD(KsMethod)(
        THIS_
        IN PKSMETHOD Method,
        IN ULONG MethodLength,
        IN OUT LPVOID MethodData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
    ) = 0;
    STDMETHOD(KsEvent)(
        THIS_
        IN PKSEVENT Event OPTIONAL,
        IN ULONG EventLength,
        IN OUT LPVOID EventData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
    ) = 0;
};

#endif // DECLARE_INTERFACE_

#endif // _IKsControl_


#ifdef DECLARE_INTERFACE_

interface DECLSPEC_UUID("7F40EAC0-3947-11D2-874E-00A0C9223196") IKsAggregateControl;
#undef INTERFACE
#define INTERFACE IKsAggregateControl
DECLARE_INTERFACE_(IKsAggregateControl, IUnknown)
{
    STDMETHOD(KsAddAggregate)(
        THIS_
        IN REFGUID AggregateClass
    ) = 0;
    STDMETHOD(KsRemoveAggregate)(
        THIS_
        IN REFGUID AggregateClass
    ) = 0;
};

#endif // DECLARE_INTERFACE_

#ifndef _IKsTopology_
#define _IKsTopology_

#ifdef DECLARE_INTERFACE_

interface DECLSPEC_UUID("28F54683-06FD-11D2-B27A-00A0C9223196") IKsTopology;
#undef INTERFACE
#define INTERFACE IKsTopology
DECLARE_INTERFACE_(IKsTopology, IUnknown)
{
    STDMETHOD(CreateNodeInstance)(
        THIS_
        IN ULONG NodeId,
        IN ULONG Flags,
        IN ACCESS_MASK DesiredAccess,
        IN IUnknown* UnkOuter OPTIONAL,
        IN REFGUID InterfaceId,
        OUT LPVOID* Interface
    ) = 0;
};

#endif // DECLARE_INTERFACE_

#endif // _IKsTopology_

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __KSPROXY__

