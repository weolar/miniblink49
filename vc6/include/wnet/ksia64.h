#include "kxia64.h"
#include "regia64.h"

//
// Pointer size in bytes
//

#define SizeofPointer 0x8

//
// Process State Enumerated Type Values
//

#define ProcessInMemory 0x0
#define ProcessOutOfMemory 0x1
#define ProcessInTransition 0x2

//
// Thread State Enumerated Type Values
//

#define Initialized 0x0
#define Ready 0x1
#define Running 0x2
#define Standby 0x3
#define Terminated 0x4
#define Waiting 0x5

//
// Wait Reason and Wait Type Enumerated Type Values
//

#define WrExecutive 0x0
#define WrMutex 0x1d
#define WrDispatchInt 0x1f
#define WrQuantumEnd 0x1e
#define WrEventPair 0xe
#define WaitAny 0x1
#define WaitAll 0x0

//
// Apc State Structure Offset Definitions
//

#define AsApcListHead 0x0
#define AsProcess 0x20
#define AsKernelApcInProgress 0x28
#define AsKernelApcPending 0x29
#define AsUserApcPending 0x2a

//
// Bug Check Code Definitions
//

#define APC_INDEX_MISMATCH 0x1
#define ATTEMPTED_SWITCH_FROM_DPC 0xb8
#define DATA_BUS_ERROR 0x2e
#define DATA_COHERENCY_EXCEPTION 0x55
#define HAL1_INITIALIZATION_FAILED 0x61
#define INSTRUCTION_BUS_ERROR 0x2f
#define INSTRUCTION_COHERENCY_EXCEPTION 0x56
#define INTERRUPT_EXCEPTION_NOT_HANDLED 0x3d
#define INTERRUPT_UNWIND_ATTEMPTED 0x3c
#define INVALID_AFFINITY_SET 0x3
#define INVALID_DATA_ACCESS_TRAP 0x4
#define IRQL_GT_ZERO_AT_SYSTEM_SERVICE 0x4a
#define IRQL_NOT_LESS_OR_EQUAL 0xa
#define KMODE_EXCEPTION_NOT_HANDLED 0x1e
#define NMI_HARDWARE_FAILURE 0x80
#define NO_USER_MODE_CONTEXT 0xe
#define PAGE_FAULT_WITH_INTERRUPTS_OFF 0x49
#define PANIC_STACK_SWITCH 0x2b
#define SPIN_LOCK_INIT_FAILURE 0x81
#define SYSTEM_EXIT_OWNED_MUTEX 0x39
#define SYSTEM_SERVICE_EXCEPTION 0x3b
#define SYSTEM_UNWIND_PREVIOUS_USER 0x3a
#define TRAP_CAUSE_UNKNOWN 0x12
#define UNEXPECTED_KERNEL_MODE_TRAP 0x7f
#define HARDWARE_INTERRUPT_STORM 0xf2
#define RECURSIVE_MACHINE_CHECK 0xfb
#define RECURSIVE_NMI 0x111

//
// Breakpoint type definitions
//

#define DBG_STATUS_CONTROL_C 0x1
#define STATUS_ASSERTION_FAILURE 0xc0000420

//
// Client Id Structure Offset Definitions
//

#define CidUniqueProcess 0x0
#define CidUniqueThread 0x8

//
// Critical Section Structure Offset Definitions
//

#define CsDebugInfo 0x0
#define CsLockCount 0x8
#define CsRecursionCount 0xc
#define CsOwningThread 0x10
#define CsLockSemaphore 0x18
#define CsSpinCount 0x20

//
// Critical Section Debug Information Structure Offset Definitions
//

#define CsType 0x0
#define CsCreatorBackTraceIndex 0x2
#define CsCriticalSection 0x8
#define CsProcessLocksList 0x10
#define CsEntryCount 0x20
#define CsContentionCount 0x24

//
// Exception Record Offset, Flag, and Enumerated Type Definitions
//

#define EXCEPTION_NONCONTINUABLE 0x1
#define EXCEPTION_UNWINDING 0x2
#define EXCEPTION_EXIT_UNWIND 0x4
#define EXCEPTION_STACK_INVALID 0x8
#define EXCEPTION_NESTED_CALL 0x10
#define EXCEPTION_TARGET_UNWIND 0x20
#define EXCEPTION_COLLIDED_UNWIND 0x40
#define EXCEPTION_UNWIND 0x66
#define EXCEPTION_EXECUTE_HANDLER 0x1
#define EXCEPTION_CONTINUE_SEARCH 0x0
#define EXCEPTION_CONTINUE_EXECUTION 0xffffffff

#define ExceptionContinueExecution 0x0
#define ExceptionContinueSearch 0x1
#define ExceptionNestedException 0x2
#define ExceptionCollidedUnwind 0x3

#define ErExceptionCode 0x0
#define ErExceptionFlags 0x4
#define ErExceptionRecord 0x8
#define ErExceptionAddress 0x10
#define ErNumberParameters 0x18
#define ErExceptionInformation 0x20
#define ExceptionRecordLength 0xa0

//
// Fast Mutex Structure Offset Definitions
//

#define FmCount 0x0
#define FmOwner 0x8
#define FmContention 0x10
#define FmGate 0x18
#define FmOldIrql 0x30

//
// Interrupt Priority Request Level Definitions
//

#define PASSIVE_LEVEL 0x0
#define APC_LEVEL 0x1
#define DISPATCH_LEVEL 0x2
#define IPI_LEVEL 0xe
#define POWER_LEVEL 0xf
#define PROFILE_LEVEL 0xf
#define HIGH_LEVEL 0xf
#ifdef NT_UP
#define SYNCH_LEVEL 0x2
#else
#define SYNCH_LEVEL 0xc
#endif

//
// Large Integer Structure Offset Definitions
//

#define LiLowPart 0x0
#define LiHighPart 0x4

//
// List Entry Structure Offset Definitions
//

#define LsFlink 0x0
#define LsBlink 0x8

//
// String Structure Offset Definitions
//

#define StrLength 0x0
#define StrMaximumLength 0x2
#define StrBuffer 0x8

//
// User Shared Data Structure Offset Definitions
//

#define UsTickCountMultiplier 0x4
#define UsInterruptTime 0x8
#define UsSystemTime 0x14
#define UsTimeZoneBias 0x20
#define UsImageNumberLow 0x2c
#define UsImageNumberHigh 0x2e
#define UsNtSystemRoot 0x30
#define UsMaxStackTraceDepth 0x238
#define UsCryptoExponent 0x23c
#define UsTimeZoneId 0x240
#define UsLargePageMinimum 0x244
#define UsReserved2 0x248
#define UsNtProductType 0x264
#define UsProductTypeIsValid 0x268
#define UsNtMajorVersion 0x26c
#define UsNtMinorVersion 0x270
#define UsProcessorFeatures 0x274
#define UsReserved1 0x2b4
#define UsReserved3 0x2b8
#define UsTimeSlip 0x2bc
#define UsAlternativeArchitecture 0x2c0
#define UsSystemExpirationDate 0x2c8
#define UsSuiteMask 0x2d0
#define UsKdDebuggerEnabled 0x2d4
#define UsActiveConsoleId 0x2d8
#define UsDismountCount 0x2dc
#define UsComPlusPackage 0x2e0
#define UsLastSystemRITEventTickCount 0x2e4
#define UsNumberOfPhysicalPages 0x2e8
#define UsSafeBootMode 0x2ec
#define UsTraceLogging 0x2f0
#define UsTestRetInstruction 0x2f8
#define UsSystemCall 0x300
#define UsSystemCallReturn 0x304
#define UsSystemCallPad 0x308
#define UsTickCount 0x320
#define UsTickCountQuad 0x320
#define UsWow64SharedInformation 0x334

//
// Time Structure Offset Definitions
//

#define TmLowTime 0x0
#define TmHighTime 0x4

//
// Thread Switch Counter Offset Definitions
//

#define TwFindAny 0x0
#define TwFindIdeal 0x4
#define TwFindLast 0x8
#define TwIdleAny 0xc
#define TwIdleCurrent 0x10
#define TwIdleIdeal 0x14
#define TwIdleLast 0x18
#define TwPreemptAny 0x1c
#define TwPreemptCurrent 0x20
#define TwPreemptLast 0x24
#define TwSwitchToIdle 0x28

//
// Status Code Definitions
//

#define STATUS_IA64_INVALID_STACK 0xc0000028
#define STATUS_ACCESS_VIOLATION 0xc0000005
#define STATUS_ARRAY_BOUNDS_EXCEEDED 0xc000008c
#define STATUS_BAD_COMPRESSION_BUFFER 0xc0000242
#define STATUS_BREAKPOINT 0x80000003
#define STATUS_CALLBACK_POP_STACK 0xc0000423
#define STATUS_DATATYPE_MISALIGNMENT 0x80000002
#define STATUS_FLOAT_DENORMAL_OPERAND 0xc000008d
#define STATUS_FLOAT_DIVIDE_BY_ZERO 0xc000008e
#define STATUS_FLOAT_INEXACT_RESULT 0xc000008f
#define STATUS_FLOAT_INVALID_OPERATION 0xc0000090
#define STATUS_FLOAT_OVERFLOW 0xc0000091
#define STATUS_FLOAT_STACK_CHECK 0xc0000092
#define STATUS_FLOAT_UNDERFLOW 0xc0000093
#define STATUS_FLOAT_MULTIPLE_FAULTS 0xc00002b4
#define STATUS_FLOAT_MULTIPLE_TRAPS 0xc00002b5
#define STATUS_GUARD_PAGE_VIOLATION 0x80000001
#define STATUS_ILLEGAL_FLOAT_CONTEXT 0xc000014a
#define STATUS_ILLEGAL_INSTRUCTION 0xc000001d
#define STATUS_INSTRUCTION_MISALIGNMENT 0xc00000aa
#define STATUS_INVALID_HANDLE 0xc0000008
#define STATUS_INVALID_LOCK_SEQUENCE 0xc000001e
#define STATUS_INVALID_OWNER 0xc000005a
#define STATUS_INVALID_PARAMETER 0xc000000d
#define STATUS_INVALID_PARAMETER_1 0xc00000ef
#define STATUS_INVALID_SYSTEM_SERVICE 0xc000001c
#define STATUS_INTEGER_DIVIDE_BY_ZERO 0xc0000094
#define STATUS_INTEGER_OVERFLOW 0xc0000095
#define STATUS_IN_PAGE_ERROR 0xc0000006
#define STATUS_KERNEL_APC 0x100
#define STATUS_LONGJUMP 0x80000026
#define STATUS_NO_CALLBACK_ACTIVE 0xc0000258
#define STATUS_NO_EVENT_PAIR 0xc000014e
#define STATUS_PRIVILEGED_INSTRUCTION 0xc0000096
#define STATUS_SINGLE_STEP 0x80000004
#define STATUS_STACK_BUFFER_OVERRUN 0xc0000409
#define STATUS_STACK_OVERFLOW 0xc00000fd
#define STATUS_SUCCESS 0x0
#define STATUS_THREAD_IS_TERMINATING 0xc000004b
#define STATUS_TIMEOUT 0x102
#define STATUS_UNWIND 0xc0000027
#define STATUS_UNWIND_CONSOLIDATE 0x80000029
#define STATUS_USER_APC 0xc0
#define STATUS_WAKE_SYSTEM_DEBUGGER 0x80000007

//
// APC Object Structure Offset Definitions
//

#define ApType 0x0
#define ApSize 0x2
#define ApThread 0x8
#define ApApcListEntry 0x10
#define ApKernelRoutine 0x20
#define ApRundownRoutine 0x28
#define ApNormalRoutine 0x30
#define ApNormalContext 0x38
#define ApSystemArgument1 0x40
#define ApSystemArgument2 0x48
#define ApApcStateIndex 0x50
#define ApApcMode 0x51
#define ApInserted 0x52
#define ApcObjectLength 0x58

//
// DPC object Structure Offset Definitions
//

#define DpType 0x0
#define DpNumber 0x2
#define DpImportance 0x1
#define DpDpcListEntry 0x8
#define DpDeferredRoutine 0x18
#define DpDeferredContext 0x20
#define DpSystemArgument1 0x28
#define DpSystemArgument2 0x30
#define DpDpcData 0x38
#define DpcObjectLength 0x40

//
// Device Queue Object Structure Offset Definitions
//

#define DvType 0x0
#define DvSize 0x2
#define DvDeviceListHead 0x8
#define DvSpinLock 0x18
#define DvBusy 0x20
#define DeviceQueueObjectLength 0x28

//
// Device Queue Entry Structure Offset Definitions
//

#define DeDeviceListEntry 0x0
#define DeSortKey 0x10
#define DeInserted 0x14
#define DeviceQueueEntryLength 0x18

//
// Event Object Structure Offset Definitions
//

#define EvType 0x0
#define EvSize 0x2
#define EvSignalState 0x4
#define EvWaitListHead 0x8
#define EventObjectLength 0x18

//
// Event Pair Object Structure Offset Definitions
//

#define EpType 0x0
#define EpSize 0x2
#define EpEventLow 0x8
#define EpEventHigh 0x20
#define SET_LOW_WAIT_HIGH 0xffffffd0
#define SET_HIGH_WAIT_LOW 0xffffffe8
#define SET_EVENT_PAIR_MASK 0x18

//
// Interrupt Object Structure Offset Definitions
//

#define InLevelSensitive 0x0
#define InLatched 0x1

#define InType 0x0
#define InSize 0x2
#define InInterruptListEntry 0x8
#define InServiceRoutine 0x18
#define InServiceContext 0x20
#define InSpinLock 0x28
#define InTickCount 0x30
#define InActualLock 0x38
#define InDispatchAddress 0x40
#define InVector 0x48
#define InIrql 0x4c
#define InSynchronizeIrql 0x4d
#define InFloatingSave 0x4e
#define InConnected 0x4f
#define InNumber 0x50
#define InShareVector 0x51
#define InMode 0x54
#define InServiceCount 0x58
#define InDispatchCount 0x5c
#define InDispatchCode 0x60
#define InterruptObjectLength 0x70

//
// Process Object Structure Offset Definitions
//

#define PrType 0x0
#define PrSize 0x2
#define PrSignalState 0x4
#define PrProfileListHead 0x18
#define PrDirectoryTableBase 0x28
#define PrProcessRegion 0x38
#define PrSessionMapInfo 0x48
#define PrSessionParentBase 0x50
#define PrActiveProcessors 0x58
#define PrKernelTime 0x60
#define PrUserTime 0x64
#define PrReadyListHead 0x68
#define PrSwapListEntry 0x78
#define PrThreadListHead 0x88
#define PrProcessLock 0x98
#define PrAffinity 0xa0
#define PrProcessFlags 0xa8
#define PrBasePriority 0xac
#define PrQuantumReset 0xad
#define PrState 0xae
#define PrStackCount 0xc0
#define KernelProcessObjectLength 0xd8
#define ExecutiveProcessObjectLength 0x400

//
// Profile Object Structure Offset Definitions
//

#define PfType 0x0
#define PfSize 0x2
#define PfProfileListEntry 0x8
#define PfProcess 0x18
#define PfRangeBase 0x20
#define PfRangeLimit 0x28
#define PfBucketShift 0x30
#define PfBuffer 0x38
#define PfSegment 0x40
#define PfAffinity 0x48
#define PfSource 0x50
#define PfStarted 0x52
#define ProfileObjectLength 0x58

//
// Queue Object Structure Offset Definitions
//

#define QuType 0x0
#define QuSize 0x2
#define QuSignalState 0x4
#define QuEntryListHead 0x18
#define QuCurrentCount 0x28
#define QuMaximumCount 0x2c
#define QuThreadListHead 0x30
#define QueueObjectLength 0x40

//
// Thread Object Structure Offset Definitions
//

#define EeKernelEventPair 0x0
#define EtCid 0x398

#define ThType 0x0
#define ThNpxIrql 0x1
#define ThSize 0x2
#define ThDebugActive 0x3
#define ThSignalState 0x4
#define ThMutantListHead 0x18
#define ThInitialStack 0x28
#define ThStackLimit 0x30
#define ThKernelStack 0x38
#define ThThreadLock 0x60
#define ThInitialBStore 0x40
#define ThBStoreLimit 0x48
#define ThNumber 0x50
#define ThKernelBStore 0x58
#define ThApcState 0x68
#define ThApcQueueable 0x93
#define ThNextProcessor 0x94
#define ThDeferredProcessor 0x95
#define ThAdjustReason 0x96
#define ThAdjustIncrement 0x97
#define ThApcQueueLock 0x98
#define ThContextSwitches 0xa0
#define ThState 0xa4
#define ThNpxState 0xa5
#define ThWaitIrql 0xa6
#define ThWaitMode 0xa7
#define ThWaitStatus 0xa8
#define ThWaitBlockList 0xb0
#define ThGateObject 0xb0
#define ThAlertable 0xb8
#define ThWaitNext 0xb9
#define ThWaitReason 0xba
#define ThPriority 0xbb
#define ThEnableStackSwap 0xbc
#define ThSwapBusy 0xbd
#define ThAlerted 0xbe
#define ThWaitListEntry 0xc0
#define ThSwapListEntry 0xc0
#define ThQueue 0xd0
#define ThWaitTime 0xd8
#define ThCombinedApcDisable 0xdc
#define ThKernelApcDisable 0xdc
#define ThSpecialApcDisable 0xde
#define ThTeb 0xe0
#define ThTimer 0xe8
#define ThThreadFlags 0x124
#define ThWaitBlock 0x128
#define ThSystemAffinityActive 0x153
#define ThPreviousMode 0x183
#define ThResourceIndex 0x1b3
#define ThLargeStack 0x1e3
#define ThQueueListEntry 0x1e8
#define ThTrapFrame 0x1f8
#define ThCallbackStack 0x200
#define ThServiceTable 0x208
#define ThApcStateIndex 0x210
#define ThIdealProcessor 0x211
#define ThPreempted 0x212
#define ThProcessReadyQueue 0x213
#define ThKernelStackResident 0x214
#define ThBasePriority 0x215
#define ThPriorityDecrement 0x216
#define ThSaturation 0x217
#define ThUserAffinity 0x218
#define ThProcess 0x220
#define ThAffinity 0x228
#define ThApcStatePointer 0x230
#define ThSavedApcState 0x240
#define ThFreezeCount 0x26b
#define ThSuspendCount 0x26c
#define ThUserIdealProcessor 0x26d
#define ThCalloutActive 0x26e
#define ThCallbackBStore 0x270
#define ThWin32Thread 0x278
#define ThStackBase 0x280
#define ThSuspendApc 0x288
#define ThQuantum 0x289
#define ThQuantumReset 0x28b
#define ThKernelTime 0x28c
#define ThTlsArray 0x2c8
#define ThLegoData 0x2d0
#define ThPowerState 0x2db
#define ThUserTime 0x2dc
#define ThSuspendSemaphore 0x2e0
#define ThSListFaultCount 0x2fc
#define ThThreadListEntry 0x300
#define ThSListFaultAddress 0x310

#define KI_SLIST_FAULT_COUNT_MAXIMUM 0x400
#define KTHREAD_AUTO_ALIGNMENT_BIT 0x0

#define KernelThreadObjectLength 0x348
#define ExecutiveThreadObjectLength 0x450


//
// Timer object Structure Offset Definitions
//

#define TiType 0x0
#define TiSize 0x2
#define TiInserted 0x3
#define TiSignalState 0x4
#define TiDueTime 0x18
#define TiTimerListEntry 0x20
#define TiDpc 0x30
#define TiPeriod 0x38
#define TimerObjectLength 0x40


//
// Timer Table Entry Structure Offset Definitions
//

#define TtEntry 0x0
#define TtTime 0x10
#define TIMER_ENTRY_SIZE 0x18
#define TIMER_TABLE_SIZE 0x200

//
// Wait Block Structure Offset Definitions
//

#define WbWaitListEntry 0x0
#define WbThread 0x10
#define WbObject 0x18
#define WbNextWaitBlock 0x20
#define WbWaitKey 0x28
#define WbWaitType 0x2a

//
// Fiber Structure Offset Definitions
//

#define FbFiberData 0x0
#define FbExceptionList 0x8
#define FbStackBase 0x10
#define FbStackLimit 0x18
#define FbDeallocationStack 0x20
#define FbFiberContext 0x30
#define FbWx86Tib 0xaa0
#define FbDeallocationBStore 0xaa8
#define FbBStoreLimit 0xab0
#define FbActivationContextStackPointer 0xab8
#define FbFlsData 0xac0
#define FbGuaranteedStackBytes 0xac8

//
// Process Environment Block Structure Offset Definitions
//

#define PeKernelCallbackTable 0x58
#define ProcessEnvironmentBlockLength 0x358

//
// System Service Descriptor Table Structure Definitions
//

#define NUMBER_SERVICE_TABLES 0x2
#define SERVICE_NUMBER_MASK 0xfff
#define SERVICE_TABLE_SHIFT 0x7
#define SERVICE_TABLE_MASK 0x20
#define SERVICE_TABLE_TEST 0x20

#define SdBase 0x0
#define SdCount 0x8
#define SdLimit 0x10
#define SdTableBaseGpOffset 0x14
#define SdNumber 0x18
#define SdLength 0x20

//
// Thread Environment Block Structure Offset Definitions
//

#define TeCmTeb 0x0
#define TeStackBase 0x8
#define TeStackLimit 0x10
#define TeFiberData 0x20
#define TeSelf 0x30
#define TeEnvironmentPointer 0x38
#define TeClientId 0x40
#define TeActiveRpcHandle 0x50
#define TeThreadLocalStoragePointer 0x58
#define TeCountOfOwnedCriticalSections 0x6c
#define TePeb 0x60
#define TeCsrClientThread 0x70
#define TeWOW32Reserved 0x100
#define TeSoftFpcr 0x10c
#define TeExceptionCode 0x2c0
#define TeActivationContextStackPointer 0x2c8
#define TeGdiClientPID 0x7f0
#define TeGdiClientTID 0x7f4
#define TeGdiThreadLocalInfo 0x7f8
#define TeglDispatchTable 0x9f0
#define TeglReserved1 0x1138
#define TeglReserved2 0x1220
#define TeglSectionInfo 0x1228
#define TeglSection 0x1230
#define TeglTable 0x1238
#define TeglCurrentRC 0x1240
#define TeglContext 0x1248
#define TeDeallocationStack 0x1478
#define TeTlsSlots 0x1480
#define TeTlsExpansionSlots 0x1780
#define TeLastErrorValue 0x68
#define TeVdm 0x1690
#define TeInstrumentation 0x16b8
#define TeGdiBatchCount 0x1740
#define TeGuaranteedStackBytes 0x1748
#define TeSafeThunkCall 0x17d0
#define TeExceptionList 0x0
#define TeDeallocationBStore 0x1788
#define TeBStoreLimit 0x1790
#define TeFlsData 0x17c8
#define ThreadEnvironmentBlockLength 0x17d8
#define CmThreadEnvironmentBlockOffset 0x2000
#define TLS_MINIMUM_AVAILABLE 0x40
#define TLS_EXPANSION_SLOTS 0x400

//
// Lock Queue Structure Offset Definitions
//

#define LOCK_QUEUE_WAIT 0x1
#define LOCK_QUEUE_OWNER 0x2
#define LOCK_QUEUE_HEADER_SIZE 0x10

#define LockQueueDispatcherLock 0x0

#define LqNext 0x0
#define LqLock 0x8

#define LqhNext 0x0
#define LqhLock 0x8
#define LqhOldIrql 0x10

//
// Performance Definitions
//

#define PERF_CONTEXTSWAP_OFFSET 0x4
#define PERF_CONTEXTSWAP_FLAG 0x4
#define PERF_INTERRUPT_OFFSET 0x4
#define PERF_INTERRUPT_FLAG 0x4000
#define PERF_PROFILE_OFFSET 0x4
#define PERF_PROFILE_FLAG 0x2

//
// EPROCESS offset used for debugging.
//

#define PrWow64Process 0x2c8

//
// Processor OS_MCA HandOff Structure Offset Definitions
//

#define SalHandOffPalProcEntryPoint 0x0
#define SalHandOffSalProcEntryPoint 0x8
#define SalHandOffSalGlobalPointer 0x10
#define SalHandOffRendezVousResult 0x18
#define SalHandOffSalReturnAddress 0x20
#define SalHandOffMinStateSavePtr 0x28

//
// Processor OS_INIT HandOff Structure Offset Definitions
//

#define OsHandOffResult 0x0
#define OsHandOffSalGlobalPointer 0x8
#define OsHandOffMinStateSavePtr 0x10
#define OsHandOffSalReturnAddress 0x18
#define OsHandOffNewContextFlag 0x20

//
// SAL Event Resouces Structure Offset Definitions
//

#define SerSalToOsHandOff 0x0
#define SerOsToSalHandOff 0x30
#define SerStateDumpPhysical 0x60
#define SerBackStore 0x68
#define SerBackStoreLimit 0x70
#define SerStack 0x78
#define SerStackLimit 0x80
#define SerPTOM 0x88
#define SerStackFrame 0x90
#define SerEventPool 0xd0
#define SerEventPoolSize 0xd8
#define SER_EVENT_STACK_FRAME_ENTRIES 0x8

//
// PAL mini-save area Structure Offset Definitions
//

#define PmsIntNats 0x0
#define PmsIntGp 0x8
#define PmsIntT0 0x10
#define PmsIntT1 0x18
#define PmsIntS0 0x20
#define PmsIntS1 0x28
#define PmsIntS2 0x30
#define PmsIntS3 0x38
#define PmsIntV0 0x40
#define PmsIntT2 0x48
#define PmsIntT3 0x50
#define PmsIntT4 0x58
#define PmsIntSp 0x60
#define PmsIntTeb 0x68
#define PmsIntT5 0x70
#define PmsIntT6 0x78

#define PmsB0R16 0x80

#define PmsIntT7 0x100
#define PmsIntT8 0x108
#define PmsIntT9 0x110
#define PmsIntT10 0x118
#define PmsIntT11 0x120
#define PmsIntT12 0x128
#define PmsIntT13 0x130
#define PmsIntT14 0x138
#define PmsIntT15 0x140
#define PmsIntT16 0x148
#define PmsIntT17 0x150
#define PmsIntT18 0x158
#define PmsIntT19 0x160
#define PmsIntT20 0x168
#define PmsIntT21 0x170
#define PmsIntT22 0x178

#define PmsPreds 0x180
#define PmsBrRp 0x188
#define PmsRsRSC 0x190

#define PmsStIIP 0x198
#define PmsStIPSR 0x1a0
#define PmsStIFS 0x1a8

#define PmsXIP 0x1b0
#define PmsXPSR 0x1b8
#define PmsXFS 0x1c0

#define PalMiniSaveLength 0x1d0

//
// Processor Control Registers Structure Offset Definitions
//

#define PCR_MINOR_VERSION 0x1
#define PCR_MAJOR_VERSION 0x1
#define PcMinorVersion 0x0
#define PcMajorVersion 0x4
#define PcInterruptRoutine 0x300
#define PcFirstLevelDcacheSize 0x8
#define PcFirstLevelDcacheFillSize 0xc
#define PcFirstLevelIcacheSize 0x10
#define PcFirstLevelIcacheFillSize 0x14
#define PcSecondLevelDcacheSize 0x18
#define PcSecondLevelDcacheFillSize 0x1c
#define PcSecondLevelIcacheSize 0x20
#define PcSecondLevelIcacheFillSize 0x24
#define PcPrcb 0xb18
#define PcDcacheAlignment 0x28
#define PcDcacheFillSize 0x2c
#define PcIcacheAlignment 0x30
#define PcIcacheFillSize 0x34
#define PcProcessorId 0x38
#define PcProfileInterval 0x3c
#define PcProfileCount 0x40
#define PcStallExecutionCount 0x44
#define PcStallScaleFactor 0x48
#define PcNumber 0xb28
#define PcKernelDebugActive 0xb2a
#define PcCurrentIrql 0xb2b
#define PcSoftwareInterruptPending 0xb2c
#define PcApcInterrupt 0xb2c
#define PcDispatchInterrupt 0xb2d
#define PcIrqlMask 0x280
#define PcIrqlTable 0x2c0
#define PcSetMember 0xb08
#define PcCurrentThread 0xb20
#define PcNotMember 0xb10
#define PcSystemReserved 0x50
#define PcHalReserved 0x80
#define PcIvr 0xb58
#define PcInitialStack 0xb60
#define PcInitialBStore 0xb68
#define PcStackLimit 0xb70
#define PcBStoreLimit 0xb78
#define PcPanicStack 0xb80
#define PcSavedIIM 0xb88
#define PcSavedIFA 0xb90
#define PcFpbLock 0xb98
#define PcForwardProgressBuffer 0xba0
#define PcEOITable 0xb30
#define PcInOsMca 0xb38
#define PcInOsInit 0xb39
#define PcInOsCmc 0xb3a
#define PcInOsCpe 0xb3b
#define PcOsMcaResourcePtr 0xb40
#define PcHighFpOwner 0xb50
#define PcInterruptionCount 0x4c
#define PcPteUbase 0xc28
#define PcPteKbase 0xc30
#define PcPteSbase 0xc38
#define PcPdeUbase 0xc40
#define PcPdeKbase 0xc48
#define PcPdeSbase 0xc50
#define PcPdeUtbase 0xc58
#define PcPdeKtbase 0xc60
#define PcPdeStbase 0xc68
#define MAX_NUMBER_OF_IHISTORY_RECORDS 0x80
#define ProcessorControlRegisterLength 0xe30
#define UsTickCountMultiplier 0x4
#define UsInterruptTime 0x8
#define UsSystemTime 0x14

//
// Processor Block Structure Offset Definitions
//

#define PRCB_MINOR_VERSION 0x1
#define PRCB_MAJOR_VERSION 0x1
#define PbMinorVersion 0x0
#define PbMajorVersion 0x2
#define PbCurrentThread 0x8
#define PbNextThread 0x10
#define PbIdleThread 0x18
#define PbNumber 0x20
#define PbBuildType 0x22
#define PbSetMember 0x28
#define PbRestartBlock 0x30
#define PbPcrPage 0x38
#define PbProcessorModel 0x50
#define PbProcessorRevision 0x54
#define PbProcessorFamily 0x58
#define PbProcessorSerialNumber 0x60
#define PbProcessorFeatureBits 0x68
#define PbProcessorVendorString 0x70
#define PbSystemReserved 0x80
#define PbHalReserved 0xc0
#define PbWakeIdle 0x21
#define PbDpcTime 0x1100
#define PbInterruptTime 0x1104
#define PbKernelTime 0x1108
#define PbUserTime 0x110c
#define PbInterruptCount 0x1110
#define PbDispatchInterruptCount 0x1114
#define PbIpiFrozen 0x1158
#define PbNestingLevel 0x1120
#define PbProcessorState 0x140
#define PbCcFastReadNoWait 0x1180
#define PbCcFastReadWait 0x1184
#define PbCcFastReadNotPossible 0x1188
#define PbCcCopyReadNoWait 0x118c
#define PbCcCopyReadWait 0x1190
#define PbCcCopyReadNoWaitMiss 0x1194
#define PbAlignmentFixupCount 0x1198
#define PbContextSwitches 0x119c
#define PbDcacheFlushCount 0x11a0
#define PbExceptionDispatchCount 0x11a4
#define PbFirstLevelTbFills 0x11a8
#define PbFloatingEmulationCount 0x11ac
#define PbIcacheFlushCount 0x11b0
#define PbSecondLevelTbFills 0x11b4
#define PbSystemCalls 0x11b8
#define PbLockQueue 0x12f0
#define PbPacketBarrier 0x1a00
#define PbCurrentPacket 0x1a80
#define PbTargetSet 0x1a98
#define PbWorkerRoutine 0x1aa0
#define PbBranchHistory 0x1ab0
#define PbRequestSummary 0x1b00
#define PbSignalDone 0x1b08
#define PbDpcListHead 0x1b80
#define PbDpcLock 0x1b90
#define PbDpcQueueDepth 0x1b98
#define PbDpcCount 0x1b9c
#define PbMaximumDpcQueueDepth 0x1bc8
#define PbDpcRequestRate 0x1bcc
#define PbMinimumDpcRate 0x1bd0
#define PbDpcLastCount 0x1bd8
#define PbDpcInterruptRequested 0x1bd4
#define PbDpcThreadRequested 0x1bd5
#define PbDpcRoutineActive 0x1bd6
#define PbDpcThreadActive 0x1bd7
#define PbTimerHand 0x1be0
#define PbThreadDpcEnable 0x1be8
#define PbQuantumEnd 0x1be9
#define PbDpcSetEventRequest 0x1bec
#define PbAdjustDpcThreshold 0x1bf0
#define PbStartCount 0x1bf8
#define PbIdleSchedule 0x1bea
#define PbDeferredReadyListHead 0x1c98
#define PbPrcbLock 0x1c90
#define PbReadySummary 0x1ca0
#define PbDispatcherReadyListHead 0x1ca8
#define PbSkipTick 0x1f00
#define PbPowerState 0x1f08
#define ProcessorBlockLength 0x21d0

//
// Processor Power State Offset Definitions
//

#define PpIdleFunction 0x0

//
// Immediate Interprocessor Command Definitions
//

#define IPI_APC 0x1
#define IPI_DPC 0x2
#define IPI_FREEZE 0x4
#define IPI_PACKET_READY 0x8

//
// Interprocessor Interrupt Count Structure Offset Definitions
//

#define IcFreeze 0x0
#define IcPacket 0x4
#define IcDPC 0x8
#define IcAPC 0xc
#define IcFlushSingleTb 0x10
#define IcFlushMultipleTb 0x14
#define IcFlushEntireTb 0x18
#define IcGenericCall 0x1c
#define IcChangeColor 0x20
#define IcSweepDcache 0x24
#define IcSweepIcache 0x28
#define IcSweepIcacheRange 0x2c
#define IcFlushIoBuffers 0x30
#define IcGratuitousDPC 0x34

//
// Context Frame Offset and Flag Definitions
//

#define CONTEXT_FULL 0x8002f
#define CONTEXT_CONTROL 0x80001
#define CONTEXT_INTEGER 0x80008
#define CONTEXT_LOWER_FLOATING_POINT 0x80002
#define CONTEXT_HIGHER_FLOATING_POINT 0x80004
#define CONTEXT_FLOATING_POINT 0x80006
#define CONTEXT_DEBUG 0x80010
#define CONTEXT_IA32_CONTROL 0x80020

#define CxContextFlags 0x0

#define CxDbI0 0x10
#define CxDbI1 0x18
#define CxDbI2 0x20
#define CxDbI3 0x28
#define CxDbI4 0x30
#define CxDbI5 0x38
#define CxDbI6 0x40
#define CxDbI7 0x48

#define CxDbD0 0x50
#define CxDbD1 0x58
#define CxDbD2 0x60
#define CxDbD3 0x68
#define CxDbD4 0x70
#define CxDbD5 0x78
#define CxDbD6 0x80
#define CxDbD7 0x88

#define CxFltS0 0x90
#define CxFltS1 0xa0
#define CxFltS2 0xb0
#define CxFltS3 0xc0

#define CxFltT0 0xd0
#define CxFltT1 0xe0
#define CxFltT2 0xf0
#define CxFltT3 0x100
#define CxFltT4 0x110
#define CxFltT5 0x120
#define CxFltT6 0x130
#define CxFltT7 0x140
#define CxFltT8 0x150
#define CxFltT9 0x160

#define CxFltS4 0x170
#define CxFltS5 0x180
#define CxFltS6 0x190
#define CxFltS7 0x1a0
#define CxFltS8 0x1b0
#define CxFltS9 0x1c0
#define CxFltS10 0x1d0
#define CxFltS11 0x1e0
#define CxFltS12 0x1f0
#define CxFltS13 0x200
#define CxFltS14 0x210
#define CxFltS15 0x220
#define CxFltS16 0x230
#define CxFltS17 0x240
#define CxFltS18 0x250
#define CxFltS19 0x260

#define CxFltF32 0x270
#define CxFltF33 0x280
#define CxFltF34 0x290
#define CxFltF35 0x2a0
#define CxFltF36 0x2b0
#define CxFltF37 0x2c0
#define CxFltF38 0x2d0
#define CxFltF39 0x2e0

#define CxFltF40 0x2f0
#define CxFltF41 0x300
#define CxFltF42 0x310
#define CxFltF43 0x320
#define CxFltF44 0x330
#define CxFltF45 0x340
#define CxFltF46 0x350
#define CxFltF47 0x360
#define CxFltF48 0x370
#define CxFltF49 0x380

#define CxFltF50 0x390
#define CxFltF51 0x3a0
#define CxFltF52 0x3b0
#define CxFltF53 0x3c0
#define CxFltF54 0x3d0
#define CxFltF55 0x3e0
#define CxFltF56 0x3f0
#define CxFltF57 0x400
#define CxFltF58 0x410
#define CxFltF59 0x420

#define CxFltF60 0x430
#define CxFltF61 0x440
#define CxFltF62 0x450
#define CxFltF63 0x460
#define CxFltF64 0x470
#define CxFltF65 0x480
#define CxFltF66 0x490
#define CxFltF67 0x4a0
#define CxFltF68 0x4b0
#define CxFltF69 0x4c0

#define CxFltF70 0x4d0
#define CxFltF71 0x4e0
#define CxFltF72 0x4f0
#define CxFltF73 0x500
#define CxFltF74 0x510
#define CxFltF75 0x520
#define CxFltF76 0x530
#define CxFltF77 0x540
#define CxFltF78 0x550
#define CxFltF79 0x560

#define CxFltF80 0x570
#define CxFltF81 0x580
#define CxFltF82 0x590
#define CxFltF83 0x5a0
#define CxFltF84 0x5b0
#define CxFltF85 0x5c0
#define CxFltF86 0x5d0
#define CxFltF87 0x5e0
#define CxFltF88 0x5f0
#define CxFltF89 0x600

#define CxFltF90 0x610
#define CxFltF91 0x620
#define CxFltF92 0x630
#define CxFltF93 0x640
#define CxFltF94 0x650
#define CxFltF95 0x660
#define CxFltF96 0x670
#define CxFltF97 0x680
#define CxFltF98 0x690
#define CxFltF99 0x6a0

#define CxFltF100 0x6b0
#define CxFltF101 0x6c0
#define CxFltF102 0x6d0
#define CxFltF103 0x6e0
#define CxFltF104 0x6f0
#define CxFltF105 0x700
#define CxFltF106 0x710
#define CxFltF107 0x720
#define CxFltF108 0x730
#define CxFltF109 0x740

#define CxFltF110 0x750
#define CxFltF111 0x760
#define CxFltF112 0x770
#define CxFltF113 0x780
#define CxFltF114 0x790
#define CxFltF115 0x7a0
#define CxFltF116 0x7b0
#define CxFltF117 0x7c0
#define CxFltF118 0x7d0
#define CxFltF119 0x7e0

#define CxFltF120 0x7f0
#define CxFltF121 0x800
#define CxFltF122 0x810
#define CxFltF123 0x820
#define CxFltF124 0x830
#define CxFltF125 0x840
#define CxFltF126 0x850
#define CxFltF127 0x860

#define CxStFPSR 0x870

#define CxIntGp 0x878
#define CxIntT0 0x880
#define CxIntT1 0x888
#define CxIntS0 0x890
#define CxIntS1 0x898
#define CxIntS2 0x8a0
#define CxIntS3 0x8a8
#define CxIntV0 0x8b0
#define CxIntT2 0x8b8
#define CxIntT3 0x8c0
#define CxIntT4 0x8c8
#define CxIntSp 0x8d0
#define CxIntTeb 0x8d8
#define CxIntT5 0x8e0
#define CxIntT6 0x8e8
#define CxIntT7 0x8f0
#define CxIntT8 0x8f8
#define CxIntT9 0x900

#define CxIntT10 0x908
#define CxIntT11 0x910
#define CxIntT12 0x918
#define CxIntT13 0x920
#define CxIntT14 0x928
#define CxIntT15 0x930
#define CxIntT16 0x938
#define CxIntT17 0x940
#define CxIntT18 0x948
#define CxIntT19 0x950
#define CxIntT20 0x958
#define CxIntT21 0x960
#define CxIntT22 0x968

#define CxIntNats 0x970
#define CxPreds 0x978

#define CxBrRp 0x980
#define CxBrS0 0x988
#define CxBrS1 0x990
#define CxBrS2 0x998
#define CxBrS3 0x9a0
#define CxBrS4 0x9a8
#define CxBrT0 0x9b0
#define CxBrT1 0x9b8

#define CxApUNAT 0x9c0
#define CxApLC 0x9c8
#define CxApEC 0x9d0
#define CxApCCV 0x9d8
#define CxApDCR 0x9e0
#define CxRsPFS 0x9e8
#define CxRsBSP 0x9f0
#define CxRsBSPSTORE 0x9f8
#define CxRsRSC 0xa00
#define CxRsRNAT 0xa08
#define CxStIPSR 0xa10
#define CxStIIP 0xa18
#define CxStIFS 0xa20

#define CxStFCR 0xa28
#define CxEflag 0xa30
#define CxSegCSD 0xa38
#define CxSegSSD 0xa40
#define CxCflag 0xa48
#define CxStFSR 0xa50
#define CxStFIR 0xa58
#define CxStFDR 0xa60

#define ContextFrameLength 0xa70


//
// Dispatcher Context Structure Offset Definitions
//

#define DcControlPc 0x10
#define DcFunctionEntry 0x20
#define DcEstablisherFrame 0x0
#define DcContextRecord 0x28

//
// Debug Register Offset Definitions and Length
//

#define TsAr21 0x0
#define TsAr24 0x8
#define TsAr26 0x18
#define TsAr27 0x20
#define TsAr28 0x28
#define TsAr29 0x30
#define TsAr30 0x38

//
// Higher FP Volatile Offset Definitions and Length
//

#define HiFltF32 0x0
#define HiFltF33 0x10
#define HiFltF34 0x20
#define HiFltF35 0x30
#define HiFltF36 0x40
#define HiFltF37 0x50
#define HiFltF38 0x60
#define HiFltF39 0x70

#define HiFltF40 0x80
#define HiFltF41 0x90
#define HiFltF42 0xa0
#define HiFltF43 0xb0
#define HiFltF44 0xc0
#define HiFltF45 0xd0
#define HiFltF46 0xe0
#define HiFltF47 0xf0
#define HiFltF48 0x100
#define HiFltF49 0x110

#define HiFltF50 0x120
#define HiFltF51 0x130
#define HiFltF52 0x140
#define HiFltF53 0x150
#define HiFltF54 0x160
#define HiFltF55 0x170
#define HiFltF56 0x180
#define HiFltF57 0x190
#define HiFltF58 0x1a0
#define HiFltF59 0x1b0

#define HiFltF60 0x1c0
#define HiFltF61 0x1d0
#define HiFltF62 0x1e0
#define HiFltF63 0x1f0
#define HiFltF64 0x200
#define HiFltF65 0x210
#define HiFltF66 0x220
#define HiFltF67 0x230
#define HiFltF68 0x240
#define HiFltF69 0x250

#define HiFltF70 0x260
#define HiFltF71 0x270
#define HiFltF72 0x280
#define HiFltF73 0x290
#define HiFltF74 0x2a0
#define HiFltF75 0x2b0
#define HiFltF76 0x2c0
#define HiFltF77 0x2d0
#define HiFltF78 0x2e0
#define HiFltF79 0x2f0

#define HiFltF80 0x300
#define HiFltF81 0x310
#define HiFltF82 0x320
#define HiFltF83 0x330
#define HiFltF84 0x340
#define HiFltF85 0x350
#define HiFltF86 0x360
#define HiFltF87 0x370
#define HiFltF88 0x380
#define HiFltF89 0x390

#define HiFltF90 0x3a0
#define HiFltF91 0x3b0
#define HiFltF92 0x3c0
#define HiFltF93 0x3d0
#define HiFltF94 0x3e0
#define HiFltF95 0x3f0
#define HiFltF96 0x400
#define HiFltF97 0x410
#define HiFltF98 0x420
#define HiFltF99 0x430

#define HiFltF100 0x440
#define HiFltF101 0x450
#define HiFltF102 0x460
#define HiFltF103 0x470
#define HiFltF104 0x480
#define HiFltF105 0x490
#define HiFltF106 0x4a0
#define HiFltF107 0x4b0
#define HiFltF108 0x4c0
#define HiFltF109 0x4d0

#define HiFltF110 0x4e0
#define HiFltF111 0x4f0
#define HiFltF112 0x500
#define HiFltF113 0x510
#define HiFltF114 0x520
#define HiFltF115 0x530
#define HiFltF116 0x540
#define HiFltF117 0x550
#define HiFltF118 0x560
#define HiFltF119 0x570

#define HiFltF120 0x580
#define HiFltF121 0x590
#define HiFltF122 0x5a0
#define HiFltF123 0x5b0
#define HiFltF124 0x5c0
#define HiFltF125 0x5d0
#define HiFltF126 0x5e0
#define HiFltF127 0x5f0


//
// Debug Register Offset Definitions and Length
//

#define DrDbI0 0x0
#define DrDbI1 0x8
#define DrDbI2 0x10
#define DrDbI3 0x18
#define DrDbI4 0x20
#define DrDbI5 0x28
#define DrDbI6 0x30
#define DrDbI7 0x38

#define DrDbD0 0x40
#define DrDbD1 0x48
#define DrDbD2 0x50
#define DrDbD3 0x58
#define DrDbD4 0x60
#define DrDbD5 0x68
#define DrDbD6 0x70
#define DrDbD7 0x78

#define TsAppRegisters 0x0
#define TsPerfRegisters 0x40
#define TsHigherFPVolatile 0x80
#define TsDebugRegisters 0x680
#define ThreadStateSaveAreaLength 0x700

//
// Exception Frame Offset Definitions and Length
//

#define ExFltS0 0x60
#define ExFltS1 0x70
#define ExFltS2 0x80
#define ExFltS3 0x90
#define ExFltS4 0xa0
#define ExFltS5 0xb0
#define ExFltS6 0xc0
#define ExFltS7 0xd0
#define ExFltS8 0xe0
#define ExFltS9 0xf0
#define ExFltS10 0x100
#define ExFltS11 0x110
#define ExFltS12 0x120
#define ExFltS13 0x130
#define ExFltS14 0x140
#define ExFltS15 0x150
#define ExFltS16 0x160
#define ExFltS17 0x170
#define ExFltS18 0x180
#define ExFltS19 0x190

#define ExIntS0 0x18
#define ExIntS1 0x20
#define ExIntS2 0x28
#define ExIntS3 0x30
#define ExIntNats 0x10

#define ExBrS0 0x38
#define ExBrS1 0x40
#define ExBrS2 0x48
#define ExBrS3 0x50
#define ExBrS4 0x58

#define ExApEC 0x0
#define ExApLC 0x8

#define ExceptionFrameLength 0x1a0

//
// Switch Frame Offset Definitions and Length
//

#define SwExFrame 0x30
#define SwPreds 0x0
#define SwRp 0x8
#define SwPFS 0x10
#define SwFPSR 0x18
#define SwBsp 0x20
#define SwRnat 0x28

#define SwitchFrameLength 0x1d0

//
// Plabel structure offset definitions
//

#define PlEntryPoint 0x0
#define PlGlobalPointer 0x8

//
// Jump Offset Definitions and Length
//

#define JbRegistration 0x18
#define JbTryLevel 0x1c
#define JbCookie 0x20
#define JbUnwindFunc 0x24
#define JbUnwindData 0x28

#define JbFPSR 0x180
#define JbFltS0 0x40
#define JbFltS1 0x50
#define JbFltS2 0x60
#define JbFltS3 0x70
#define JbFltS4 0x80
#define JbFltS5 0x90
#define JbFltS6 0xa0
#define JbFltS7 0xb0
#define JbFltS8 0xc0
#define JbFltS9 0xd0

#define JbFltS10 0xe0
#define JbFltS11 0xf0
#define JbFltS12 0x100
#define JbFltS13 0x110
#define JbFltS14 0x120
#define JbFltS15 0x130
#define JbFltS16 0x140
#define JbFltS17 0x150
#define JbFltS18 0x160
#define JbFltS19 0x170

#define JbStIIP 0x188
#define JbBrS0 0x190
#define JbBrS1 0x198
#define JbBrS2 0x1a0
#define JbBrS3 0x1a8
#define JbBrS4 0x1b0

#define JbRsBSP 0x1d8
#define JbRsPFS 0x1e0
#define JbApUNAT 0x1e8
#define JbApLC 0x1f0

#define JbIntS0 0x1b8
#define JbIntS1 0x1c0
#define JbIntS2 0x1c8
#define JbIntS3 0x1d0
#define JbIntSp 0x1f8
#define JbIntNats 0x200
#define JbPreds 0x208

#define JumpBufferLength 0x210

//
// Trap Frame Offset Definitions and Length
//

#define TrFltT0 0x50
#define TrFltT1 0x60
#define TrFltT2 0x70
#define TrFltT3 0x80
#define TrFltT4 0x90
#define TrFltT5 0xa0
#define TrFltT6 0xb0
#define TrFltT7 0xc0
#define TrFltT8 0xd0
#define TrFltT9 0xe0

#define TrIntGp 0xf0
#define TrIntT0 0xf8
#define TrIntT1 0x100

#define TrApUNAT 0x108
#define TrApCCV 0x110
#define TrSegCSD 0x118
#define TrPreds 0x120

#define TrIntV0 0x128
#define TrIntT2 0x130
#define TrIntT3 0x138
#define TrIntT4 0x140
#define TrIntSp 0x148
#define TrIntTeb 0x150
#define TrIntT5 0x158
#define TrIntT6 0x160
#define TrIntT7 0x168
#define TrIntT8 0x170
#define TrIntT9 0x178

#define TrIntT10 0x180
#define TrIntT11 0x188
#define TrIntT12 0x190
#define TrIntT13 0x198
#define TrIntT14 0x1a0
#define TrIntT15 0x1a8
#define TrIntT16 0x1b0
#define TrIntT17 0x1b8
#define TrIntT18 0x1c0
#define TrIntT19 0x1c8
#define TrIntT20 0x1d0
#define TrIntT21 0x1d8
#define TrIntT22 0x1e0

#define TrIntNats 0x1e8

#define TrBrRp 0x1f0
#define TrBrT0 0x1f8
#define TrBrT1 0x200

#define TrRsPFS 0x228
#define TrRsBSP 0x210
#define TrRsRSC 0x208
#define TrRsRNAT 0x220
#define TrRsBSPSTORE 0x218

#define TrStIPSR 0x230
#define TrStISR 0x250
#define TrStIFA 0x258
#define TrStIIP 0x238
#define TrStIIPA 0x260
#define TrStIFS 0x240
#define TrStIIM 0x268
#define TrStIHA 0x270
#define TrStFPSR 0x248

#define TrOldIrql 0x278
#define TrPreviousMode 0x27c
#define TrTrapFrame 0x280
#define TrNewBSP 0x328
#define TrEOFMarker 0x330
#define TrExceptionRecord 0x288

#define TrapFrameLength 0x340
#define TrapFrameArguments 0x40
#define KTRAP_FRAME_EOF 0xe0f0e0f0e0f0e000
#define MODIFIED_FRAME 0x20

//
// Usermode callout kernel frame definitions
//

#define CuBrRp 0x0
#define CuRsPFS 0x8
#define CuPreds 0x10
#define CuApUNAT 0x18
#define CuApLC 0x20
#define CuIntS0 0x38
#define CuIntS1 0x40
#define CuIntS2 0x48
#define CuIntS3 0x50
#define CuBrS0 0x58
#define CuBrS1 0x60
#define CuBrS2 0x68
#define CuBrS3 0x70
#define CuBrS4 0x78
#define CuRsRNAT 0x28
#define CuIntNats 0x30
#define CuFltS0 0x80
#define CuFltS1 0x90
#define CuFltS2 0xa0
#define CuFltS3 0xb0
#define CuFltS4 0xc0
#define CuFltS5 0xd0
#define CuFltS6 0xe0
#define CuFltS7 0xf0
#define CuFltS8 0x100
#define CuFltS9 0x110
#define CuFltS10 0x120
#define CuFltS11 0x130
#define CuFltS12 0x140
#define CuFltS13 0x150
#define CuFltS14 0x160
#define CuFltS15 0x170
#define CuFltS16 0x180
#define CuFltS17 0x190
#define CuFltS18 0x1a0
#define CuFltS19 0x1b0
#define CuA0 0x1c0
#define CuA1 0x1c8
#define CuCbStk 0x1d0
#define CuInStack 0x1d8
#define CuCbBStore 0x1e0
#define CuInBStore 0x1e8
#define CuTrFrame 0x1f0
#define CuTrStIIP 0x1f8
#define CuFrameLength 0x200

//
// Usermode callout user frame definitions
//

#define CkBuffer 0x0
#define CkLength 0x8
#define CkApiNumber 0xc
#define CkIntSp 0x10
#define CkRsPFS 0x18
#define CkBrRp 0x20

//
// Translation Register Info Definitions
//

#define TrInfoValid 0x0
#define TrInfoPageSize 0x4
#define TrInfoVirtualAddress 0x8
#define TrInfoLength 0x28

//
// Loader Parameter Block Offset Definitions
//

#define LpbLoadOrderListHead 0x0
#define LpbMemoryDescriptorListHead 0x10
#define LpbKernelStack 0x30
#define LpbPrcb 0x38
#define LpbProcess 0x40
#define LpbThread 0x48
#define LpbAcpiRsdt 0x108
#define LpbKernelPhysicalBase 0xc0
#define LpbKernelVirtualBase 0xc8
#define LpbInterruptStack 0xd0
#define LpbPanicStack 0xd8
#define LpbPcrPage 0xe0
#define LpbPdrPage 0xe8
#define LpbPcrPage2 0xf0
#define LpbMachineType 0xb8
#define LpbItrInfo 0x188
#define LpbDtrInfo 0x2c8

//
// Address Space Layout Definitions
//

#define UREGION_INDEX 0x0
#define KSEG0_BASE 0xe000000080000000
#define KSEG2_BASE 0xe0000000a0000000
#define KADDRESS_BASE 0xe000000000000000
#define UADDRESS_BASE 0x0
#define SADDRESS_BASE 0x2000000000000000
#define VIRTUAL_IO_BASE 0xe0000000f0000000
#define SYSTEM_BASE 0xe0000000c3000000
#define KSEG3_BASE 0x8000000000000000
#define KSEG3_LIMIT 0x8004000000000000

//
// Page Table and Directory Entry Definitions
//

#define PAGE_SIZE 0x2000
#define PAGE_SHIFT 0xd
#define PDI_SHIFT 0x17
#define PTI_SHIFT 0xd
#define PTE_SHIFT 0x3
#define VHPT_PDE_BITS 0x28
#define LARGE_PAGE_SIZE 0x1000000
#define LARGE_PAGE_SHIFT 0x18

//
// Breakpoint Definitions
//

#define USER_BREAKPOINT 0x80002
#define KERNEL_BREAKPOINT 0x80001
#define BREAKPOINT_BREAKIN 0x80019
#define UNKNOWN_ERROR_BREAK 0x0
#define INTEGER_DIVIDE_BY_ZERO_BREAK 0x1
#define INTEGER_OVERFLOW_BREAK 0x2
#define RANGE_CHECK_BREAK 0x3
#define NULL_POINTER_DEFERENCE_BREAK 0x4
#define MISALIGNED_DATA_BREAK 0x5
#define DECIMAL_OVERFLOW_BREAK 0x6
#define DECIMAL_DIVIDE_BY_ZERO_BREAK 0x7
#define PACKED_DECIMAL_ERROR_BREAK 0x8
#define INVALID_ASCII_DIGIT_BREAK 0x9
#define INVALID_DECIMAL_DIGIT_BREAK 0xa
#define PARAGRAPH_STACK_OVERFLOW_BREAK 0xb
#define BREAKPOINT_PRINT 0x80014
#define BREAKPOINT_PROMPT 0x80015
#define BREAKPOINT_STOP 0x80016
#define BREAKPOINT_LOAD_SYMBOLS 0x80017
#define BREAKPOINT_UNLOAD_SYMBOLS 0x80018
#define BREAKPOINT_COMMAND_STRING 0x8001a

//
// IA64 Specific Definitions
//

#define BREAK_APP_BASE 0x40000
#define BREAK_DEBUG_BASE 0x80000
#define BREAK_SYSCALL_BASE 0x180000
#define BREAK_SYSCALL 0x180000
#define BREAK_FASTSYS_BASE 0x1c0000
#define BREAK_SET_LOW_WAIT_HIGH 0x1c0020
#define BREAK_SET_HIGH_WAIT_LOW 0x1c0010
#define SYSCALL_FRAME 0x0
#define INTERRUPT_FRAME 0x1
#define EXCEPTION_FRAME 0x2
#define CONTEXT_FRAME 0xa


//
// Miscellaneous Definitions
//

#define Executive 0x0
#define KernelMode 0x0
#define UserMode 0x1
#define FALSE 0x0
#define TRUE 0x1
#define KiPcr 0xe0000000ffff0000
#define KiPcr2 0xe0000000fffe0000
#define BASE_PRIORITY_THRESHOLD 0x8
#define EVENT_PAIR_INCREMENT 0x1
#define LOW_REALTIME_PRIORITY 0x10
#define KERNEL_STACK_SIZE 0x8000
#define KERNEL_BSTORE_SIZE 0x8000
#define KERNEL_LARGE_STACK_COMMIT 0x8000
#define KERNEL_LARGE_BSTORE_COMMIT 0x8000
#define MI_USER_PROBE_ADDRESS 0x000006fbffff0000
#define MM_EPC_VA 0xe0000000ffa00000
#define THREAD_QUANTUM 0x6
#define CLOCK_QUANTUM_DECREMENT 0x3
#define WAIT_QUANTUM_DECREMENT 0x1
#define READY_SKIP_QUANTUM 0x2
#define ROUND_TRIP_DECREMENT_COUNT 0x10

//
// kernel special register frame offset definitions
//

#define KpsSpecialRegisters 0xa70
#define KsKernelDbI0 0x0
#define KsKernelDbI1 0x8
#define KsKernelDbI2 0x10
#define KsKernelDbI3 0x18
#define KsKernelDbI4 0x20
#define KsKernelDbI5 0x28
#define KsKernelDbI6 0x30
#define KsKernelDbI7 0x38
#define KsKernelDbD0 0x40
#define KsKernelDbD1 0x48
#define KsKernelDbD2 0x50
#define KsKernelDbD3 0x58
#define KsKernelDbD4 0x60
#define KsKernelDbD5 0x68
#define KsKernelDbD6 0x70
#define KsKernelDbD7 0x78
#define KsKernelPfC0 0x80
#define KsKernelPfC1 0x88
#define KsKernelPfC2 0x90
#define KsKernelPfC3 0x98
#define KsKernelPfC4 0xa0
#define KsKernelPfC5 0xa8
#define KsKernelPfC6 0xb0
#define KsKernelPfC7 0xb8
#define KsKernelPfD0 0xc0
#define KsKernelPfD1 0xc8
#define KsKernelPfD2 0xd0
#define KsKernelPfD3 0xd8
#define KsKernelPfD4 0xe0
#define KsKernelPfD5 0xe8
#define KsKernelPfD6 0xf0
#define KsKernelPfD7 0xf8
#define KsIntH16 0x100
#define KsIntH17 0x108
#define KsIntH18 0x110
#define KsIntH19 0x118
#define KsIntH20 0x120
#define KsIntH21 0x128
#define KsIntH22 0x130
#define KsIntH23 0x138
#define KsIntH24 0x140
#define KsIntH25 0x148
#define KsIntH26 0x150
#define KsIntH27 0x158
#define KsIntH28 0x160
#define KsIntH29 0x168
#define KsIntH30 0x170
#define KsIntH31 0x178
#define KsApCPUID0 0x180
#define KsApCPUID1 0x188
#define KsApCPUID2 0x190
#define KsApCPUID3 0x198
#define KsApCPUID4 0x1a0
#define KsApCPUID5 0x1a8
#define KsApCPUID6 0x1b0
#define KsApCPUID7 0x1b8
#define KsApKR0 0x1c0
#define KsApKR1 0x1c8
#define KsApKR2 0x1d0
#define KsApKR3 0x1d8
#define KsApKR4 0x1e0
#define KsApKR5 0x1e8
#define KsApKR6 0x1f0
#define KsApKR7 0x1f8
#define KsApITC 0x200
#define KsApITM 0x208
#define KsApIVA 0x210
#define KsApPTA 0x218
#define KsApGPTA 0x220
#define KsStISR 0x228
#define KsStIFA 0x230
#define KsStITIR 0x238
#define KsStIIPA 0x240
#define KsStIIM 0x248
#define KsStIHA 0x250
#define KsSaLID 0x258
#define KsSaIVR 0x260
#define KsSaTPR 0x268
#define KsSaEOI 0x270
#define KsSaIRR0 0x278
#define KsSaIRR1 0x280
#define KsSaIRR2 0x288
#define KsSaIRR3 0x290
#define KsSaITV 0x298
#define KsSaPMV 0x2a0
#define KsSaCMCV 0x2a8
#define KsSaLRR0 0x2b0
#define KsSaLRR1 0x2b8
#define KsRr0 0x2c0
#define KsRr1 0x2c8
#define KsRr2 0x2d0
#define KsRr3 0x2d8
#define KsRr4 0x2e0
#define KsRr5 0x2e8
#define KsRr6 0x2f0
#define KsRr7 0x2f8
#define KsPkr0 0x300
#define KsPkr1 0x308
#define KsPkr2 0x310
#define KsPkr3 0x318
#define KsPkr4 0x320
#define KsPkr5 0x328
#define KsPkr6 0x330
#define KsPkr7 0x338
#define KsPkr8 0x340
#define KsPkr9 0x348
#define KsPkr10 0x350
#define KsPkr11 0x358
#define KsPkr12 0x360
#define KsPkr13 0x368
#define KsPkr14 0x370
#define KsPkr15 0x378
#define KsTrI0 0x380
#define KsTrI1 0x388
#define KsTrI2 0x390
#define KsTrI3 0x398
#define KsTrI4 0x3a0
#define KsTrI5 0x3a8
#define KsTrI6 0x3b0
#define KsTrI7 0x3b8
#define KsTrD0 0x3c0
#define KsTrD1 0x3c8
#define KsTrD2 0x3d0
#define KsTrD3 0x3d8
#define KsTrD4 0x3e0
#define KsTrD5 0x3e8
#define KsTrD6 0x3f0
#define KsTrD7 0x3f8
#define KsSrMSR0 0x400
#define KsSrMSR1 0x408
#define KsSrMSR2 0x410
#define KsSrMSR3 0x418
#define KsSrMSR4 0x420
#define KsSrMSR5 0x428
#define KsSrMSR6 0x430
#define KsSrMSR7 0x438
#define KpsTrEntry 0xeb0
#define KtrIndex 0x0
#define KtrGR 0x8
#define KtrITIR 0x10
#define KtrIFA 0x18
#define PHYSICAL_ADDRESS_BITS 0x32

