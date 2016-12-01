
;
;  Pointer size in bytes
;

SizeofPointer equ 00004H

;
;  Process State Enumerated Type Values
;

ProcessInMemory equ 00000H
ProcessOutOfMemory equ 00001H
ProcessInTransition equ 00002H

;
;  Thread State Enumerated Type Values
;

Initialized equ 00000H
Ready equ 00001H
Running equ 00002H
Standby equ 00003H
Terminated equ 00004H
Waiting equ 00005H

;
;  Wait Reason and Wait Type Enumerated Type Values
;

WrExecutive equ 00000H
WrMutex equ 0001DH
WrDispatchInt equ 0001FH
WrQuantumEnd equ 0001EH
WrEventPair equ 0000EH
WaitAny equ 00001H
WaitAll equ 00000H

;
;  Apc State Structure Offset Definitions
;

AsApcListHead equ 00000H
AsProcess equ 00010H
AsKernelApcInProgress equ 00014H
AsKernelApcPending equ 00015H
AsUserApcPending equ 00016H

;
;  Bug Check Code Definitions
;

APC_INDEX_MISMATCH equ 00001H
ATTEMPTED_SWITCH_FROM_DPC equ 000B8H
DATA_BUS_ERROR equ 0002EH
DATA_COHERENCY_EXCEPTION equ 00055H
HAL1_INITIALIZATION_FAILED equ 00061H
INSTRUCTION_BUS_ERROR equ 0002FH
INSTRUCTION_COHERENCY_EXCEPTION equ 00056H
INTERRUPT_EXCEPTION_NOT_HANDLED equ 0003DH
INTERRUPT_UNWIND_ATTEMPTED equ 0003CH
INVALID_AFFINITY_SET equ 00003H
INVALID_DATA_ACCESS_TRAP equ 00004H
IRQL_GT_ZERO_AT_SYSTEM_SERVICE equ 0004AH
IRQL_NOT_LESS_OR_EQUAL equ 0000AH
KMODE_EXCEPTION_NOT_HANDLED equ 0001EH
NMI_HARDWARE_FAILURE equ 00080H
NO_USER_MODE_CONTEXT equ 0000EH
PAGE_FAULT_WITH_INTERRUPTS_OFF equ 00049H
PANIC_STACK_SWITCH equ 0002BH
SPIN_LOCK_INIT_FAILURE equ 00081H
SYSTEM_EXIT_OWNED_MUTEX equ 00039H
SYSTEM_SERVICE_EXCEPTION equ 0003BH
SYSTEM_UNWIND_PREVIOUS_USER equ 0003AH
TRAP_CAUSE_UNKNOWN equ 00012H
UNEXPECTED_KERNEL_MODE_TRAP equ 0007FH
HARDWARE_INTERRUPT_STORM equ 000F2H
RECURSIVE_MACHINE_CHECK equ 000FBH
RECURSIVE_NMI equ 00111H

;
;  Breakpoint type definitions
;

DBG_STATUS_CONTROL_C equ 00001H
STATUS_ASSERTION_FAILURE equ 0C0000420H

;
;  Client Id Structure Offset Definitions
;

CidUniqueProcess equ 00000H
CidUniqueThread equ 00004H

;
;  Critical Section Structure Offset Definitions
;

CsDebugInfo equ 00000H
CsLockCount equ 00004H
CsRecursionCount equ 00008H
CsOwningThread equ 0000CH
CsLockSemaphore equ 00010H
CsSpinCount equ 00014H

;
;  Critical Section Debug Information Structure Offset Definitions
;

CsType equ 00000H
CsCreatorBackTraceIndex equ 00002H
CsCriticalSection equ 00004H
CsProcessLocksList equ 00008H
CsEntryCount equ 00010H
CsContentionCount equ 00014H

;
;  Exception Record Offset, Flag, and Enumerated Type Definitions
;

EXCEPTION_NONCONTINUABLE equ 00001H
EXCEPTION_UNWINDING equ 00002H
EXCEPTION_EXIT_UNWIND equ 00004H
EXCEPTION_STACK_INVALID equ 00008H
EXCEPTION_NESTED_CALL equ 00010H
EXCEPTION_TARGET_UNWIND equ 00020H
EXCEPTION_COLLIDED_UNWIND equ 00040H
EXCEPTION_UNWIND equ 00066H
EXCEPTION_EXECUTE_HANDLER equ 00001H
EXCEPTION_CONTINUE_SEARCH equ 00000H
EXCEPTION_CONTINUE_EXECUTION equ 0FFFFFFFFH
EXCEPTION_CHAIN_END equ 0FFFFFFFFH
FIXED_NTVDMSTATE_LINEAR equ 00714H

ExceptionContinueExecution equ 00000H
ExceptionContinueSearch equ 00001H
ExceptionNestedException equ 00002H
ExceptionCollidedUnwind equ 00003H

ErExceptionCode equ 00000H
ErExceptionFlags equ 00004H
ErExceptionRecord equ 00008H
ErExceptionAddress equ 0000CH
ErNumberParameters equ 00010H
ErExceptionInformation equ 00014H
ExceptionRecordLength equ 00050H

;
;  Fast Mutex Structure Offset Definitions
;

FmCount equ 00000H
FmOwner equ 00004H
FmContention equ 00008H
FmGate equ 0000CH
FmOldIrql equ 0001CH

;
;  Interrupt Priority Request Level Definitions
;

PASSIVE_LEVEL equ 00000H
APC_LEVEL equ 00001H
DISPATCH_LEVEL equ 00002H
CLOCK1_LEVEL equ 0001CH
CLOCK2_LEVEL equ 0001CH
IPI_LEVEL equ 0001DH
POWER_LEVEL equ 0001EH
PROFILE_LEVEL equ 0001BH
HIGH_LEVEL equ 0001FH
ifdef NT_UP
SYNCH_LEVEL equ 00002H
else
SYNCH_LEVEL equ 0001BH
endif

;
;  Large Integer Structure Offset Definitions
;

LiLowPart equ 00000H
LiHighPart equ 00004H

;
;  List Entry Structure Offset Definitions
;

LsFlink equ 00000H
LsBlink equ 00004H

;
;  String Structure Offset Definitions
;

StrLength equ 00000H
StrMaximumLength equ 00002H
StrBuffer equ 00004H

;
;  System Time Structure Offset Definitions
;

StLowTime equ 00000H
StHigh1Time equ 00004H
StHigh2Time equ 00008H

;
;  User Shared Data Structure Offset Definitions
;

UsTickCountMultiplier equ 00004H
UsInterruptTime equ 00008H
UsSystemTime equ 00014H
UsTimeZoneBias equ 00020H
UsImageNumberLow equ 0002CH
UsImageNumberHigh equ 0002EH
UsNtSystemRoot equ 00030H
UsMaxStackTraceDepth equ 00238H
UsCryptoExponent equ 0023CH
UsTimeZoneId equ 00240H
UsLargePageMinimum equ 00244H
UsReserved2 equ 00248H
UsNtProductType equ 00264H
UsProductTypeIsValid equ 00268H
UsNtMajorVersion equ 0026CH
UsNtMinorVersion equ 00270H
UsProcessorFeatures equ 00274H
UsReserved1 equ 002B4H
UsReserved3 equ 002B8H
UsTimeSlip equ 002BCH
UsAlternativeArchitecture equ 002C0H
UsSystemExpirationDate equ 002C8H
UsSuiteMask equ 002D0H
UsKdDebuggerEnabled equ 002D4H
UsActiveConsoleId equ 002D8H
UsDismountCount equ 002DCH
UsComPlusPackage equ 002E0H
UsLastSystemRITEventTickCount equ 002E4H
UsNumberOfPhysicalPages equ 002E8H
UsSafeBootMode equ 002ECH
UsTraceLogging equ 002F0H
UsTestRetInstruction equ 002F8H
UsSystemCall equ 00300H
UsSystemCallReturn equ 00304H
UsSystemCallPad equ 00308H
UsTickCount equ 00320H
UsTickCountQuad equ 00320H
UsWow64SharedInformation equ 00334H

;
;  Time Structure Offset Definitions
;

TmLowTime equ 00000H
TmHighTime equ 00004H

;
;  Thread Switch Counter Offset Definitions
;

TwFindAny equ 00000H
TwFindIdeal equ 00004H
TwFindLast equ 00008H
TwIdleAny equ 0000CH
TwIdleCurrent equ 00010H
TwIdleIdeal equ 00014H
TwIdleLast equ 00018H
TwPreemptAny equ 0001CH
TwPreemptCurrent equ 00020H
TwPreemptLast equ 00024H
TwSwitchToIdle equ 00028H

;
;  Status Code Definitions
;

STATUS_ACCESS_VIOLATION equ 0C0000005H
STATUS_ARRAY_BOUNDS_EXCEEDED equ 0C000008CH
STATUS_BAD_COMPRESSION_BUFFER equ 0C0000242H
STATUS_BREAKPOINT equ 080000003H
STATUS_CALLBACK_POP_STACK equ 0C0000423H
STATUS_DATATYPE_MISALIGNMENT equ 080000002H
STATUS_FLOAT_DENORMAL_OPERAND equ 0C000008DH
STATUS_FLOAT_DIVIDE_BY_ZERO equ 0C000008EH
STATUS_FLOAT_INEXACT_RESULT equ 0C000008FH
STATUS_FLOAT_INVALID_OPERATION equ 0C0000090H
STATUS_FLOAT_OVERFLOW equ 0C0000091H
STATUS_FLOAT_STACK_CHECK equ 0C0000092H
STATUS_FLOAT_UNDERFLOW equ 0C0000093H
STATUS_FLOAT_MULTIPLE_FAULTS equ 0C00002B4H
STATUS_FLOAT_MULTIPLE_TRAPS equ 0C00002B5H
STATUS_GUARD_PAGE_VIOLATION equ 080000001H
STATUS_ILLEGAL_FLOAT_CONTEXT equ 0C000014AH
STATUS_ILLEGAL_INSTRUCTION equ 0C000001DH
STATUS_INSTRUCTION_MISALIGNMENT equ 0C00000AAH
STATUS_INVALID_HANDLE equ 0C0000008H
STATUS_INVALID_LOCK_SEQUENCE equ 0C000001EH
STATUS_INVALID_OWNER equ 0C000005AH
STATUS_INVALID_PARAMETER equ 0C000000DH
STATUS_INVALID_PARAMETER_1 equ 0C00000EFH
STATUS_INVALID_SYSTEM_SERVICE equ 0C000001CH
STATUS_INTEGER_DIVIDE_BY_ZERO equ 0C0000094H
STATUS_INTEGER_OVERFLOW equ 0C0000095H
STATUS_IN_PAGE_ERROR equ 0C0000006H
STATUS_KERNEL_APC equ 00100H
STATUS_LONGJUMP equ 080000026H
STATUS_NO_CALLBACK_ACTIVE equ 0C0000258H
STATUS_NO_EVENT_PAIR equ 0C000014EH
STATUS_PRIVILEGED_INSTRUCTION equ 0C0000096H
STATUS_SINGLE_STEP equ 080000004H
STATUS_STACK_BUFFER_OVERRUN equ 0C0000409H
STATUS_STACK_OVERFLOW equ 0C00000FDH
STATUS_SUCCESS equ 00000H
STATUS_THREAD_IS_TERMINATING equ 0C000004BH
STATUS_TIMEOUT equ 00102H
STATUS_UNWIND equ 0C0000027H
STATUS_UNWIND_CONSOLIDATE equ 080000029H
STATUS_USER_APC equ 000C0H
STATUS_WAKE_SYSTEM_DEBUGGER equ 080000007H

;
;  APC Object Structure Offset Definitions
;

ApType equ 00000H
ApSize equ 00002H
ApThread equ 00008H
ApApcListEntry equ 0000CH
ApKernelRoutine equ 00014H
ApRundownRoutine equ 00018H
ApNormalRoutine equ 0001CH
ApNormalContext equ 00020H
ApSystemArgument1 equ 00024H
ApSystemArgument2 equ 00028H
ApApcStateIndex equ 0002CH
ApApcMode equ 0002DH
ApInserted equ 0002EH
ApcObjectLength equ 00030H

;
;  DPC object Structure Offset Definitions
;

DpType equ 00000H
DpNumber equ 00002H
DpImportance equ 00001H
DpDpcListEntry equ 00004H
DpDeferredRoutine equ 0000CH
DpDeferredContext equ 00010H
DpSystemArgument1 equ 00014H
DpSystemArgument2 equ 00018H
DpDpcData equ 0001CH
DpcObjectLength equ 00020H

;
;  Device Queue Object Structure Offset Definitions
;

DvType equ 00000H
DvSize equ 00002H
DvDeviceListHead equ 00004H
DvSpinLock equ 0000CH
DvBusy equ 00010H
DeviceQueueObjectLength equ 00014H

;
;  Device Queue Entry Structure Offset Definitions
;

DeDeviceListEntry equ 00000H
DeSortKey equ 00008H
DeInserted equ 0000CH
DeviceQueueEntryLength equ 00010H

;
;  Event Object Structure Offset Definitions
;

EvType equ 00000H
EvSize equ 00002H
EvSignalState equ 00004H
EvWaitListHead equ 00008H
EventObjectLength equ 00010H

;
;  Event Pair Object Structure Offset Definitions
;

EpType equ 00000H
EpSize equ 00002H
EpEventLow equ 00004H
EpEventHigh equ 00014H

;
;  Interrupt Object Structure Offset Definitions
;

InLevelSensitive equ 00000H
InLatched equ 00001H

InType equ 00000H
InSize equ 00002H
InInterruptListEntry equ 00004H
InServiceRoutine equ 0000CH
InServiceContext equ 00010H
InSpinLock equ 00014H
InTickCount equ 00018H
InActualLock equ 0001CH
InDispatchAddress equ 00020H
InVector equ 00024H
InIrql equ 00028H
InSynchronizeIrql equ 00029H
InFloatingSave equ 0002AH
InConnected equ 0002BH
InNumber equ 0002CH
InShareVector equ 0002DH
InMode equ 00030H
InServiceCount equ 00034H
InDispatchCount equ 00038H
InDispatchCode equ 0003CH
InterruptObjectLength equ 001E4H

NORMAL_DISPATCH_LENGTH equ 001A8H
DISPATCH_LENGTH equ 001A8H

;
;  Process Object Structure Offset Definitions
;

PrType equ 00000H
PrSize equ 00002H
PrSignalState equ 00004H
PrProfileListHead equ 00010H
PrDirectoryTableBase equ 00018H
PrLdtDescriptor equ 00020H
PrIopmOffset equ 00030H
PrInt21Descriptor equ 00028H
PrIopl equ 00032H
PrVdmTrapcHandler equ 0004CH
PrVdmObjects equ 00144H
PrFlags equ 00240H
PrActiveProcessors equ 00034H
PrKernelTime equ 00038H
PrUserTime equ 0003CH
PrReadyListHead equ 00040H
PrSwapListEntry equ 00048H
PrThreadListHead equ 00050H
PrProcessLock equ 00058H
PrAffinity equ 0005CH
PrProcessFlags equ 00060H
PrBasePriority equ 00064H
PrQuantumReset equ 00065H
PrState equ 00066H
PrStackCount equ 0006CH
KernelProcessObjectLength equ 00078H
ExecutiveProcessObjectLength equ 00278H

;
;  Profile Object Structure Offset Definitions
;

PfType equ 00000H
PfSize equ 00002H
PfProfileListEntry equ 00004H
PfProcess equ 0000CH
PfRangeBase equ 00010H
PfRangeLimit equ 00014H
PfBucketShift equ 00018H
PfBuffer equ 0001CH
PfSegment equ 00020H
PfAffinity equ 00024H
PfSource equ 00028H
PfStarted equ 0002AH
ProfileObjectLength equ 0002CH

;
;  Queue Object Structure Offset Definitions
;

QuType equ 00000H
QuSize equ 00002H
QuSignalState equ 00004H
QuEntryListHead equ 00010H
QuCurrentCount equ 00018H
QuMaximumCount equ 0001CH
QuThreadListHead equ 00020H
QueueObjectLength equ 00028H

;
;  Thread Object Structure Offset Definitions
;

EeKernelEventPair equ 00000H
EtCid equ 001E4H

ThType equ 00000H
ThNpxIrql equ 00001H
ThSize equ 00002H
ThDebugActive equ 00003H
ThSignalState equ 00004H
ThMutantListHead equ 00010H
ThInitialStack equ 00018H
ThStackLimit equ 0001CH
ThKernelStack equ 00020H
ThThreadLock equ 00024H
ThApcState equ 00028H
ThApcQueueable equ 0003FH
ThNextProcessor equ 00040H
ThDeferredProcessor equ 00041H
ThAdjustReason equ 00042H
ThAdjustIncrement equ 00043H
ThApcQueueLock equ 00044H
ThContextSwitches equ 00048H
ThState equ 0004CH
ThNpxState equ 0004DH
ThWaitIrql equ 0004EH
ThWaitMode equ 0004FH
ThWaitStatus equ 00050H
ThWaitBlockList equ 00054H
ThGateObject equ 00054H
ThAlertable equ 00058H
ThWaitNext equ 00059H
ThWaitReason equ 0005AH
ThPriority equ 0005BH
ThEnableStackSwap equ 0005CH
ThSwapBusy equ 0005DH
ThAlerted equ 0005EH
ThWaitListEntry equ 00060H
ThSwapListEntry equ 00060H
ThQueue equ 00068H
ThWaitTime equ 0006CH
ThCombinedApcDisable equ 00070H
ThKernelApcDisable equ 00070H
ThSpecialApcDisable equ 00072H
ThTeb equ 00074H
ThTimer equ 00078H
ThThreadFlags equ 000A0H
ThWaitBlock equ 000A8H
ThSystemAffinityActive equ 000BFH
ThPreviousMode equ 000D7H
ThResourceIndex equ 000EFH
ThLargeStack equ 00107H
ThQueueListEntry equ 00108H
ThTrapFrame equ 00110H
ThCallbackStack equ 00114H
ThServiceTable equ 00118H
ThApcStateIndex equ 0011CH
ThIdealProcessor equ 0011DH
ThPreempted equ 0011EH
ThProcessReadyQueue equ 0011FH
ThKernelStackResident equ 00120H
ThBasePriority equ 00121H
ThPriorityDecrement equ 00122H
ThSaturation equ 00123H
ThUserAffinity equ 00124H
ThProcess equ 00128H
ThAffinity equ 0012CH
ThApcStatePointer equ 00130H
ThSavedApcState equ 00138H
ThFreezeCount equ 0014FH
ThSuspendCount equ 00150H
ThUserIdealProcessor equ 00151H
ThCalloutActive equ 00152H
ThIopl equ 00153H
ThWin32Thread equ 00154H
ThStackBase equ 00158H
ThSuspendApc equ 0015CH
ThQuantum equ 0015DH
ThQuantumReset equ 0015FH
ThKernelTime equ 00160H
ThTlsArray equ 00180H
ThLegoData equ 00184H
ThPowerState equ 0018BH
ThUserTime equ 0018CH
ThSuspendSemaphore equ 00190H
ThSListFaultCount equ 001A4H
ThThreadListEntry equ 001A8H
ThSListFaultAddress equ 001B0H

KI_SLIST_FAULT_COUNT_MAXIMUM equ 00400H
KTHREAD_AUTO_ALIGNMENT_BIT equ 00000H

KernelThreadObjectLength equ 001B8H
ExecutiveThreadObjectLength equ 00250H

NPX_STATE_NOT_LOADED equ 0000AH
NPX_STATE_LOADED equ 00000H

;
;  Timer object Structure Offset Definitions
;

TiType equ 00000H
TiSize equ 00002H
TiInserted equ 00003H
TiSignalState equ 00004H
TiDueTime equ 00010H
TiTimerListEntry equ 00018H
TiDpc equ 00020H
TiPeriod equ 00024H
TimerObjectLength equ 00028H


;
;  Timer Table Entry Structure Offset Definitions
;

TtEntry equ 00000H
TtTime equ 00008H
TIMER_ENTRY_SIZE equ 00010H
TIMER_TABLE_SIZE equ 00200H

;
;  Wait Block Structure Offset Definitions
;

WbWaitListEntry equ 00000H
WbThread equ 00008H
WbObject equ 0000CH
WbNextWaitBlock equ 00010H
WbWaitKey equ 00014H
WbWaitType equ 00016H

;
;  Fiber Structure Offset Definitions
;

FbFiberData equ 00000H
FbExceptionList equ 00004H
FbStackBase equ 00008H
FbStackLimit equ 0000CH
FbDeallocationStack equ 00010H
FbFiberContext equ 00014H
FbWx86Tib equ 002E0H
FbActivationContextStackPointer equ 002E4H
FbFlsData equ 002E8H
FbGuaranteedStackBytes equ 002ECH

;
;  Process Environment Block Structure Offset Definitions
;

PeKernelCallbackTable equ 0002CH
ProcessEnvironmentBlockLength equ 00230H

;
;  System Service Descriptor Table Structure Definitions
;

NUMBER_SERVICE_TABLES equ 00002H
SERVICE_NUMBER_MASK equ 00FFFH
SERVICE_TABLE_SHIFT equ 00008H
SERVICE_TABLE_MASK equ 00010H
SERVICE_TABLE_TEST equ 00010H

SdBase equ 00000H
SdCount equ 00004H
SdLimit equ 00008H
SdNumber equ 0000CH
SdLength equ 00010H

;
;  Thread Environment Block Structure Offset Definitions
;

TeExceptionList equ 00000H
TeStackBase equ 00004H
TeStackLimit equ 00008H
TeFiberData equ 00010H
TeSelf equ 00018H
TeEnvironmentPointer equ 0001CH
TeClientId equ 00020H
TeActiveRpcHandle equ 00028H
TeThreadLocalStoragePointer equ 0002CH
TeCountOfOwnedCriticalSections equ 00038H
TePeb equ 00030H
TeCsrClientThread equ 0003CH
TeWOW32Reserved equ 000C0H
TeSoftFpcr equ 000C8H
TeExceptionCode equ 001A4H
TeActivationContextStackPointer equ 001A8H
TeGdiClientPID equ 006C0H
TeGdiClientTID equ 006C4H
TeGdiThreadLocalInfo equ 006C8H
TeglDispatchTable equ 007C4H
TeglReserved1 equ 00B68H
TeglReserved2 equ 00BDCH
TeglSectionInfo equ 00BE0H
TeglSection equ 00BE4H
TeglTable equ 00BE8H
TeglCurrentRC equ 00BECH
TeglContext equ 00BF0H
TeDeallocationStack equ 00E0CH
TeTlsSlots equ 00E10H
TeTlsExpansionSlots equ 00F94H
TeLastErrorValue equ 00034H
TeVdm equ 00F18H
TeInstrumentation equ 00F2CH
TeGdiBatchCount equ 00F70H
TeGuaranteedStackBytes equ 00F78H
TeSafeThunkCall equ 00FB8H
TeFlsData equ 00FB4H
ThreadEnvironmentBlockLength equ 00FBCH
CmThreadEnvironmentBlockOffset equ 01000H
TLS_MINIMUM_AVAILABLE equ 00040H
TLS_EXPANSION_SLOTS equ 00400H

;
;  Lock Queue Structure Offset Definitions
;

LOCK_QUEUE_WAIT equ 00001H
LOCK_QUEUE_OWNER equ 00002H
LOCK_QUEUE_HEADER_SIZE equ 00008H

LockQueueDispatcherLock equ 00000H

LqNext equ 00000H
LqLock equ 00004H

LqhNext equ 00000H
LqhLock equ 00004H
LqhOldIrql equ 00008H

;
;  Performance Definitions
;

PERF_CONTEXTSWAP_OFFSET equ 00004H
PERF_CONTEXTSWAP_FLAG equ 00004H
PERF_INTERRUPT_OFFSET equ 00004H
PERF_INTERRUPT_FLAG equ 04000H
PERF_PROFILE_OFFSET equ 00004H
PERF_PROFILE_FLAG equ 00002H

;
;  Apc Record Structure Offset Definitions
;

ArNormalRoutine equ 00000H
ArNormalContext equ 00004H
ArSystemArgument1 equ 00008H
ArSystemArgument2 equ 0000CH
ApcRecordLength equ 00010H


;
;  Processor Control Registers Structure Offset Definitions
;

KI_BEGIN_KERNEL_RESERVED equ 0FFDF0000H
ifdef NT_UP
    P0PCRADDRESS equ 0FFDFF000H
    PCR equ ds:[0FFDFF000H]
else
    PCR equ fs:
endif

PcExceptionList equ 00000H
PcInitialStack equ 00004H
PcPerfGlobalGroupMask equ 00008H
PcTssCopy equ 0000CH
PcContextSwitches equ 00010H
PcSetMemberCopy equ 00014H
PcSelfPcr equ 0001CH
PcPrcb equ 00020H
PcTeb equ 00018H
PcIrql equ 00024H
PcIRR equ 00028H
PcIrrActive equ 0002CH
PcIDR equ 00030H
PcIdt equ 00038H
PcGdt equ 0003CH
PcTss equ 00040H
PcNumber equ 00051H
PcVdmAlert equ 00054H
PcSetMember equ 00048H
PcStallScaleFactor equ 0004CH
PcHal equ 00094H
PcPrcbData equ 00120H
ProcessorControlRegisterLength equ 00FE0H
TebPeb equ 00030H
PebBeingDebugged equ 00002H
PebKernelCallbackTable equ 0002CH

;
;  Defines for user shared data
;

USER_SHARED_DATA equ 0FFDF0000H
MM_SHARED_USER_DATA_VA equ 07FFE0000H
USERDATA equ ds:[0FFDF0000H]
UsTickCountMultiplier equ 00004H
UsInterruptTime equ 00008H
UsSystemTime equ 00014H
UsProcessorFeatures equ 00274H
UsSystemCall equ 00300H
UsSystemCallReturn equ 00304H

PF_XMMI_INSTRUCTIONS_AVAILABLE equ 00006H

;
;  Tss Structure Offset Definitions
;

TssEsp0 equ 00004H
TssCR3 equ 0001CH
TssEip equ 00020H
TssEFlags equ 00024H
TssEax equ 00028H
TssEbx equ 00034H
TssEcx equ 0002CH
TssEdx equ 00030H
TssEsp equ 00038H
TssEbp equ 0003CH
TssEsi equ 00040H
TssEdi equ 00044H
TssEs equ 00048H
TssCs equ 0004CH
TssSs equ 00050H
TssDs equ 00054H
TssFs equ 00058H
TssGs equ 0005CH
TssLDT equ 00060H
TssIoMapBase equ 00066H
TssIoMaps equ 00068H
TssLength equ 020ACH

;
;  Gdt Descriptor Offset Definitions
;

KGDT_R3_DATA equ 00020H
KGDT_R3_CODE equ 00018H
KGDT_R0_CODE equ 00008H
KGDT_R0_DATA equ 00010H
KGDT_R0_PCR equ 00030H
KGDT_STACK16 equ 000F8H
KGDT_CODE16 equ 000F0H
KGDT_TSS equ 00028H
KGDT_R3_TEB equ 00038H
KGDT_DF_TSS equ 00050H
KGDT_NMI_TSS equ 00058H
KGDT_LDT equ 00048H

;
;  GdtEntry Offset Definitions
;

KgdtBaseLow equ 00002H
KgdtBaseMid equ 00004H
KgdtBaseHi equ 00007H
KgdtLimitHi equ 00006H
KgdtLimitLow equ 00000H


;
;  Processor Block Structure Offset Definitions
;

PbCurrentThread equ 00004H
PbNextThread equ 00008H
PbIdleThread equ 0000CH
PbNumber equ 00010H
PbSetMember equ 00014H
PbCpuID equ 00019H
PbCpuType equ 00018H
PbCpuStep equ 0001AH
PbProcessorState equ 0001CH
PbHalReserved equ 0037CH
PbLockQueue equ 00418H
PbNpxThread equ 00520H
PbInterruptCount equ 00524H
PbKernelTime equ 00528H
PbUserTime equ 0052CH
PbDpcTime equ 00530H
PbDebugDpcTime equ 00534H
PbInterruptTime equ 00538H
PbAdjustDpcThreshold equ 0053CH
PbPageColor equ 00540H
PbSkipTick equ 00544H
PbNodeShiftedColor equ 00548H
PbParentNode equ 0054CH
PbMultiThreadProcessorSet equ 00550H
PbMultiThreadSetMaster equ 00554H
PbSecondaryColorMask equ 00558H
PbAlignmentFixupCount equ 00578H
PbDcacheFlushCount equ 00580H
PbExceptionDispatchCount equ 00584H
PbFirstLevelTbFills equ 00588H
PbFloatingEmulationCount equ 0058CH
PbIcacheFlushCount equ 00590H
PbSecondLevelTbFills equ 00594H
PbSystemCalls equ 00598H
PbPPLookasideList equ 005E0H
PbPPNPagedLookasideList equ 00660H
PbPPPagedLookasideList equ 00760H
PbPacketBarrier equ 00860H
PbReverseStall equ 00864H
PbIpiFrame equ 00868H
PbCurrentPacket equ 008A0H
PbTargetSet equ 008ACH
PbWorkerRoutine equ 008B0H
PbIpiFrozen equ 008B4H
PbRequestSummary equ 008E0H
PbSignalDone equ 008E4H
PbDpcListHead equ 00920H
PbDpcLock equ 00928H
PbDpcQueueDepth equ 0092CH
PbDpcCount equ 00930H
PbDpcStack equ 00948H
PbMaximumDpcQueueDepth equ 0094CH
PbDpcRequestRate equ 00950H
PbMinimumDpcRate equ 00954H
PbDpcInterruptRequested equ 00958H
PbDpcThreadRequested equ 00959H
PbDpcRoutineActive equ 0095AH
PbDpcThreadActive equ 0095BH
PbPrcbLock equ 0095CH
PbDpcLastCount equ 00960H
PbTimerHand equ 00964H
PbTimerRequest equ 00968H
PbQuantumEnd equ 00981H
PbIdleSchedule equ 00983H
PbReadySummary equ 009E8H
PbDispatcherReadyListHead equ 009F0H
PbDeferredReadyListHead equ 00AF0H
PbNpxSaveArea equ 00B90H
PbChainedInterruptList equ 00B20H
PbPowerState equ 00DA0H
ProcessorBlockLength equ 00EC0H

;
;  Processor Power State Offset Definitions
;

PpIdleFunction equ 00000H

;
;  Immediate Interprocessor Command Definitions
;

IPI_APC equ 00001H
IPI_DPC equ 00002H
IPI_FREEZE equ 00004H
IPI_PACKET_READY equ 00008H
IPI_SYNCH_REQUEST equ 00010H

;
;  Thread Environment Block Structure Offset Definitions
;

TbExceptionList equ 00000H
TbStackBase equ 00004H
TbStackLimit equ 00008H
TbEnvironmentPointer equ 0001CH
TbVersion equ 00010H
TbFiberData equ 00010H
TbArbitraryUserPointer equ 00014H
TbClientId equ 00020H
TbThreadLocalStoragePointer equ 0002CH
TbCountOfOwnedCriticalSections equ 00038H
TbSystemReserved1 equ 000CCH
TbVdm equ 00F18H
TbCsrClientThread equ 0003CH
TbGdiThreadLocalInfo equ 006C8H
TbglDispatchTable equ 007C4H
TbglSectionInfo equ 00BE0H
TbglSection equ 00BE4H
TbglTable equ 00BE8H
TbglCurrentRC equ 00BECH
TbglContext equ 00BF0H
TbWOW32Reserved equ 000C0H
TbExceptionCode equ 001A4H
TbDeallocationStack equ 00E0CH
TbGdiBatchCount equ 00F70H

;
;  Time Fields (TIME_FIELDS) Structure Offset Definitions
;

TfSecond equ 0000AH
TfMinute equ 00008H
TfHour equ 00006H
TfWeekday equ 0000EH
TfDay equ 00004H
TfMonth equ 00002H
TfYear equ 00000H
TfMilliseconds equ 0000CH


;
;  constants for system irql and IDT vector conversion
;

MAXIMUM_IDTVECTOR equ 000FFH
MAXIMUM_PRIMARY_VECTOR equ 000FFH
PRIMARY_VECTOR_BASE equ 00030H
RPL_MASK equ 00003H
MODE_MASK equ 00001H

;
;  Flags in the CR0 register
;

CR0_PG equ 080000000H
CR0_ET equ 00010H
CR0_TS equ 00008H
CR0_EM equ 00004H
CR0_MP equ 00002H
CR0_PE equ 00001H
CR0_CD equ 040000000H
CR0_NW equ 020000000H
CR0_AM equ 040000H
CR0_WP equ 010000H
CR0_NE equ 00020H

;
;  Flags in the CR4 register
;

CR4_VME equ 00001H
CR4_PVI equ 00002H
CR4_TSD equ 00004H
CR4_DE equ 00008H
CR4_PSE equ 00010H
CR4_PAE equ 00020H
CR4_MCE equ 00040H
CR4_PGE equ 00080H
CR4_FXSR equ 00200H
CR4_XMMEXCPT equ 00400H

;
;  Miscellaneous Definitions
;

MAXIMUM_PROCESSORS equ 00020H
INITIAL_STALL_COUNT equ 00064H
IRQL_NOT_GREATER_OR_EQUAL equ 00009H
IRQL_NOT_LESS_OR_EQUAL equ 0000AH
MUTEX_ALREADY_OWNED equ 000BFH
THREAD_NOT_MUTEX_OWNER equ 00011H
SPIN_LOCK_ALREADY_OWNED equ 0000FH
SPIN_LOCK_NOT_OWNED equ 00010H
BASE_PRIORITY_THRESHOLD equ 00008H
EVENT_PAIR_INCREMENT equ 00001H
LOW_REALTIME_PRIORITY equ 00010H
BlackHole equ 0FFFFA000H
KERNEL_LARGE_STACK_COMMIT equ 03000H
KERNEL_STACK_SIZE equ 03000H
DOUBLE_FAULT_STACK_SIZE equ 03000H
EFLAG_SELECT equ 0C000H
BREAKPOINT_BREAK equ 00000H
IPI_FREEZE equ 00004H
CLOCK_QUANTUM_DECREMENT equ 00003H
READY_SKIP_QUANTUM equ 00002H
THREAD_QUANTUM equ 00006H
WAIT_QUANTUM_DECREMENT equ 00001H
ROUND_TRIP_DECREMENT_COUNT equ 00010H
KI_EXCEPTION_ACCESS_VIOLATION equ 010000004H

;
;  Trap Frame Offset Definitions and Length
;

TsExceptionList equ 0004CH
TsPreviousPreviousMode equ 00048H
TsSegGs equ 00030H
TsSegFs equ 00050H
TsSegEs equ 00034H
TsSegDs equ 00038H
TsEdi equ 00054H
TsEsi equ 00058H
TsEbp equ 00060H
TsEbx equ 0005CH
TsEdx equ 0003CH
TsEcx equ 00040H
TsEax equ 00044H
TsErrCode equ 00064H
TsEip equ 00068H
TsSegCs equ 0006CH
TsEflags equ 00070H
TsHardwareEsp equ 00074H
TsHardwareSegSs equ 00078H
TsTempSegCs equ 00010H
TsTempEsp equ 00014H
TsDbgEbp equ 00000H
TsDbgEip equ 00004H
TsDbgArgMark equ 00008H
TsDbgArgPointer equ 0000CH
TsDr0 equ 00018H
TsDr1 equ 0001CH
TsDr2 equ 00020H
TsDr3 equ 00024H
TsDr6 equ 00028H
TsDr7 equ 0002CH
TsV86Es equ 0007CH
TsV86Ds equ 00080H
TsV86Fs equ 00084H
TsV86Gs equ 00088H
KTRAP_FRAME_LENGTH equ 0008CH
KTRAP_FRAME_ALIGN equ 00004H
FRAME_EDITED equ 0FFF8H
EFLAGS_ALIGN_CHECK equ 040000H
EFLAGS_V86_MASK equ 020000H
EFLAGS_INTERRUPT_MASK equ 00200H
EFLAGS_TF equ 00100H
EFLAGS_VIF equ 080000H
EFLAGS_VIP equ 0100000H
EFLAGS_USER_SANITIZE equ 03F4DD7H

;
;  Context Frame Offset and Flag Definitions
;

CONTEXT_FULL equ 010007H
CONTEXT_DEBUG_REGISTERS equ 010010H
CONTEXT_CONTROL equ 010001H
CONTEXT_FLOATING_POINT equ 010008H
CONTEXT_INTEGER equ 010002H
CONTEXT_SEGMENTS equ 010004H

CsContextFlags equ 00000H
CsDr0 equ 00004H
CsDr1 equ 00008H
CsDr2 equ 0000CH
CsDr3 equ 00010H
CsDr6 equ 00014H
CsDr7 equ 00018H
CsFloatSave equ 0001CH
CsSegGs equ 0008CH
CsSegFs equ 00090H
CsSegEs equ 00094H
CsSegDs equ 00098H
CsEdi equ 0009CH
CsEsi equ 000A0H
CsEbx equ 000A4H
CsEdx equ 000A8H
CsEcx equ 000ACH
CsEax equ 000B0H
CsEbp equ 000B4H
CsEip equ 000B8H
CsSegCs equ 000BCH
CsEflags equ 000C0H
CsEsp equ 000C4H
CsSegSs equ 000C8H
CsExtendedRegisters equ 000CCH
ContextFrameLength equ 002D0H
CONTEXT_ALIGNED_SIZE equ 002CCH

DR6_LEGAL equ 0E00FH
DR7_LEGAL equ 0FFFF0155H
DR7_ACTIVE equ 00055H
DR7_RESERVED_MASK equ 0DC00H

ErrHandler equ 00004H
ErrNext equ 00000H
ErrLength equ 00008H

;
;  Floating save area field offset definitions
;

FpControlWord equ 00000H
FpStatusWord equ 00004H
FpTagWord equ 00008H
FpErrorOffset equ 0000CH
FpErrorSelector equ 00010H
FpDataOffset equ 00014H
FpDataSelector equ 00018H
FpRegisterArea equ 0001CH
FpCtxtCr0NpxState equ 0006CH

;
;  FX Floating save area field offset definitions
;

FxControlWord equ 00000H
FxStatusWord equ 00002H
FxTagWord equ 00004H
FxErrorOpcode equ 00006H
FxErrorOffset equ 00008H
FxErrorSelector equ 0000CH
FxDataOffset equ 00010H
FxDataSelector equ 00014H
FxMXCsr equ 00018H
FxFpRegisterArea equ 00020H
FpNpxSavedCpu equ 00208H
FpCr0NpxState equ 0020CH

NPX_FRAME_LENGTH equ 00210H

;
;  Processor State Frame Offset Definitions

;

PsContextFrame equ 00000H
PsSpecialRegisters equ 002CCH
SrCr0 equ 00000H
SrCr2 equ 00004H
SrCr3 equ 00008H
SrCr4 equ 0000CH
SrKernelDr0 equ 00010H
SrKernelDr1 equ 00014H
SrKernelDr2 equ 00018H
SrKernelDr3 equ 0001CH
SrKernelDr6 equ 00020H
SrKernelDr7 equ 00024H
SrGdtr equ 0002AH
SrIdtr equ 00032H
SrTr equ 00038H
SrLdtr equ 0003AH
ProcessorStateLength equ 00320H

;
;  EPROCESS
;

EpDebugPort equ 000CCH
EpVdmObjects equ 00144H

;
;  Machine type definitions (Temporarily)
;

MACHINE_TYPE_ISA equ 00000H
MACHINE_TYPE_EISA equ 00001H
MACHINE_TYPE_MCA equ 00002H

;
;  KeFeatureBits defines
;

KF_V86_VIS equ 00001H
KF_RDTSC equ 00002H
KF_CR4 equ 00004H
KF_GLOBAL_PAGE equ 00010H
KF_LARGE_PAGE equ 00020H
KF_CMPXCHG8B equ 00080H
KF_FAST_SYSCALL equ 01000H

;
;  LoaderParameterBlock offsets relative to base
;

LpbLoadOrderListHead equ 00000H
LpbMemoryDescriptorListHead equ 00008H
LpbKernelStack equ 00018H
LpbPrcb equ 0001CH
LpbProcess equ 00020H
LpbThread equ 00024H
LpbI386 equ 0005CH
LpbRegistryLength equ 00028H
LpbRegistryBase equ 0002CH
LpbConfigurationRoot equ 00030H
LpbArcBootDeviceName equ 00034H
LpbArcHalDeviceName equ 00038H
LpbExtension equ 00058H

;
;  LoaderParameterExtension offsets relative to base
;

LpeHalpIRQLToTPR equ 00040H
LpeHalpVectorToIRQL equ 00044H
PAGE_SIZE equ 01000H

;
;  VDM equates.
;

VDM_INDEX_Invalid equ 00000H
VDM_INDEX_0F equ 00001H
VDM_INDEX_ESPrefix equ 00002H
VDM_INDEX_CSPrefix equ 00003H
VDM_INDEX_SSPrefix equ 00004H
VDM_INDEX_DSPrefix equ 00005H
VDM_INDEX_FSPrefix equ 00006H
VDM_INDEX_GSPrefix equ 00007H
VDM_INDEX_OPER32Prefix equ 00008H
VDM_INDEX_ADDR32Prefix equ 00009H
VDM_INDEX_INSB equ 0000AH
VDM_INDEX_INSW equ 0000BH
VDM_INDEX_OUTSB equ 0000CH
VDM_INDEX_OUTSW equ 0000DH
VDM_INDEX_PUSHF equ 0000EH
VDM_INDEX_POPF equ 0000FH
VDM_INDEX_INTnn equ 00010H
VDM_INDEX_INTO equ 00011H
VDM_INDEX_IRET equ 00012H
VDM_INDEX_NPX equ 00013H
VDM_INDEX_INBimm equ 00014H
VDM_INDEX_INWimm equ 00015H
VDM_INDEX_OUTBimm equ 00016H
VDM_INDEX_OUTWimm equ 00017H
VDM_INDEX_INB equ 00018H
VDM_INDEX_INW equ 00019H
VDM_INDEX_OUTB equ 0001AH
VDM_INDEX_OUTW equ 0001BH
VDM_INDEX_LOCKPrefix equ 0001CH
VDM_INDEX_REPNEPrefix equ 0001DH
VDM_INDEX_REPPrefix equ 0001EH
VDM_INDEX_CLI equ 0001FH
VDM_INDEX_STI equ 00020H
VDM_INDEX_HLT equ 00021H
MAX_VDM_INDEX equ 00022H

;
;  VDM feature bits.
;

V86_VIRTUAL_INT_EXTENSIONS equ 00001H
PM_VIRTUAL_INT_EXTENSIONS equ 00002H

;
;  Selector types.
;

SEL_TYPE_NP equ 00040H

;
;  Usermode callout frame definitions
;

CuInStk equ 00000H
CuTrFr equ 00004H
CuCbStk equ 00008H
CuEdi equ 0000CH
CuEsi equ 00010H
CuEbx equ 00014H
CuEbp equ 00018H
CuRet equ 0001CH
CuOutBf equ 00020H
CuOutLn equ 00024H

;
;  VDM_PROCESS_OBJECTS
;

VpVdmTib equ 00098H

;
;  Wow64 turbo dispatch system call types
;

ServiceNoTurbo equ 00000H

;
;  No arguments system call
;

Service0Arg equ 00001H
Service0ArgReloadState equ 00002H

;
;  One argument system call types
;

Service1ArgSp equ 00003H
Service1ArgNSp equ 00004H

;
;  Two arguments system call types
;

Service2ArgNSpNSp equ 00005H
Service2ArgNSpNSpReloadState equ 00006H
Service2ArgSpNSp equ 00007H
Service2ArgSpSp equ 00008H
Service2ArgNSpSp equ 00009H

;
;  Three arguments system call types
;

Service3ArgNSpNSpNSp equ 0000AH
Service3ArgSpSpSp equ 0000BH
Service3ArgSpNSpNSp equ 0000CH
Service3ArgSpNSpNSpReloadState equ 0000DH
Service3ArgSpSpNSp equ 0000EH
Service3ArgSpSpSp equ 0000BH
Service3ArgSpNSpSp equ 00010H

;
;  Four arguments system call types
;

Service4ArgNSpNSpNSpNSp equ 00011H
Service4ArgSpSpNSpNSp equ 00012H
Service4ArgSpSpNSpNSpReloadState equ 00013H
Service4ArgSpNSpNSpNSp equ 00014H
Service4ArgSpNSpNSpNSpReloadState equ 00015H
Service4ArgNSpSpNSpNSp equ 00016H
Service4ArgSpSpSpNSp equ 00017H

;
;  Special system calls
;

ServiceCpupTdQuerySystemTime equ 00018H
ServiceCpupTdGetCurrentProcessorNumber equ 00019H
ServiceCpupTdReadWriteFile equ 0001AH
ServiceCpupTdDeviceIoControlFile equ 0001BH
ServiceCpupTdRemoveIoCompletion equ 0001CH
ServiceCpupTdWaitForMultipleObjects equ 0001DH
ServiceCpupTdWaitForMultipleObjects32 equ 0001EH
Wow64ServiceTypesCount equ 0001FH

