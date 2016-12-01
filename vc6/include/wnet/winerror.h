/************************************************************************
*                                                                       *
*   winerror.h --  error code definitions for the Win32 API functions   *
*                                                                       *
*   Copyright (c) Microsoft Corp.  All rights reserved.                 *
*                                                                       *
************************************************************************/

#ifndef _WINERROR_
#define _WINERROR_

#if defined (_MSC_VER) && (_MSC_VER >= 1020) && !defined(__midl)
#pragma once
#endif

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_WINDOWSUPDATE           36
#define FACILITY_WINDOWS_CE              24
#define FACILITY_WINDOWS                 8
#define FACILITY_URT                     19
#define FACILITY_UMI                     22
#define FACILITY_SXS                     23
#define FACILITY_STORAGE                 3
#define FACILITY_STATE_MANAGEMENT        34
#define FACILITY_SSPI                    9
#define FACILITY_SCARD                   16
#define FACILITY_SETUPAPI                15
#define FACILITY_SECURITY                9
#define FACILITY_RPC                     1
#define FACILITY_WIN32                   7
#define FACILITY_CONTROL                 10
#define FACILITY_NULL                    0
#define FACILITY_METADIRECTORY           35
#define FACILITY_MSMQ                    14
#define FACILITY_MEDIASERVER             13
#define FACILITY_INTERNET                12
#define FACILITY_ITF                     4
#define FACILITY_HTTP                    25
#define FACILITY_DPLAY                   21
#define FACILITY_DISPATCH                2
#define FACILITY_DIRECTORYSERVICE        37
#define FACILITY_CONFIGURATION           33
#define FACILITY_COMPLUS                 17
#define FACILITY_CERT                    11
#define FACILITY_BACKGROUNDCOPY          32
#define FACILITY_ACS                     20
#define FACILITY_AAF                     18


//
// Define the severity codes
//


//
// MessageId: ERROR_SUCCESS
//
// MessageText:
//
//  The operation completed successfully.
//
#define ERROR_SUCCESS                    0L

#define NO_ERROR 0L                                                 // dderror
#define SEC_E_OK                         ((HRESULT)0x00000000L)

//
// MessageId: ERROR_INVALID_FUNCTION
//
// MessageText:
//
//  Incorrect function.
//
#define ERROR_INVALID_FUNCTION           1L    // dderror

//
// MessageId: ERROR_FILE_NOT_FOUND
//
// MessageText:
//
//  The system cannot find the file specified.
//
#define ERROR_FILE_NOT_FOUND             2L

//
// MessageId: ERROR_PATH_NOT_FOUND
//
// MessageText:
//
//  The system cannot find the path specified.
//
#define ERROR_PATH_NOT_FOUND             3L

//
// MessageId: ERROR_TOO_MANY_OPEN_FILES
//
// MessageText:
//
//  The system cannot open the file.
//
#define ERROR_TOO_MANY_OPEN_FILES        4L

//
// MessageId: ERROR_ACCESS_DENIED
//
// MessageText:
//
//  Access is denied.
//
#define ERROR_ACCESS_DENIED              5L

//
// MessageId: ERROR_INVALID_HANDLE
//
// MessageText:
//
//  The handle is invalid.
//
#define ERROR_INVALID_HANDLE             6L

//
// MessageId: ERROR_ARENA_TRASHED
//
// MessageText:
//
//  The storage control blocks were destroyed.
//
#define ERROR_ARENA_TRASHED              7L

//
// MessageId: ERROR_NOT_ENOUGH_MEMORY
//
// MessageText:
//
//  Not enough storage is available to process this command.
//
#define ERROR_NOT_ENOUGH_MEMORY          8L    // dderror

//
// MessageId: ERROR_INVALID_BLOCK
//
// MessageText:
//
//  The storage control block address is invalid.
//
#define ERROR_INVALID_BLOCK              9L

//
// MessageId: ERROR_BAD_ENVIRONMENT
//
// MessageText:
//
//  The environment is incorrect.
//
#define ERROR_BAD_ENVIRONMENT            10L

//
// MessageId: ERROR_BAD_FORMAT
//
// MessageText:
//
//  An attempt was made to load a program with an incorrect format.
//
#define ERROR_BAD_FORMAT                 11L

//
// MessageId: ERROR_INVALID_ACCESS
//
// MessageText:
//
//  The access code is invalid.
//
#define ERROR_INVALID_ACCESS             12L

//
// MessageId: ERROR_INVALID_DATA
//
// MessageText:
//
//  The data is invalid.
//
#define ERROR_INVALID_DATA               13L

//
// MessageId: ERROR_OUTOFMEMORY
//
// MessageText:
//
//  Not enough storage is available to complete this operation.
//
#define ERROR_OUTOFMEMORY                14L

//
// MessageId: ERROR_INVALID_DRIVE
//
// MessageText:
//
//  The system cannot find the drive specified.
//
#define ERROR_INVALID_DRIVE              15L

//
// MessageId: ERROR_CURRENT_DIRECTORY
//
// MessageText:
//
//  The directory cannot be removed.
//
#define ERROR_CURRENT_DIRECTORY          16L

//
// MessageId: ERROR_NOT_SAME_DEVICE
//
// MessageText:
//
//  The system cannot move the file to a different disk drive.
//
#define ERROR_NOT_SAME_DEVICE            17L

//
// MessageId: ERROR_NO_MORE_FILES
//
// MessageText:
//
//  There are no more files.
//
#define ERROR_NO_MORE_FILES              18L

//
// MessageId: ERROR_WRITE_PROTECT
//
// MessageText:
//
//  The media is write protected.
//
#define ERROR_WRITE_PROTECT              19L

//
// MessageId: ERROR_BAD_UNIT
//
// MessageText:
//
//  The system cannot find the device specified.
//
#define ERROR_BAD_UNIT                   20L

//
// MessageId: ERROR_NOT_READY
//
// MessageText:
//
//  The device is not ready.
//
#define ERROR_NOT_READY                  21L

//
// MessageId: ERROR_BAD_COMMAND
//
// MessageText:
//
//  The device does not recognize the command.
//
#define ERROR_BAD_COMMAND                22L

//
// MessageId: ERROR_CRC
//
// MessageText:
//
//  Data error (cyclic redundancy check).
//
#define ERROR_CRC                        23L

//
// MessageId: ERROR_BAD_LENGTH
//
// MessageText:
//
//  The program issued a command but the command length is incorrect.
//
#define ERROR_BAD_LENGTH                 24L

//
// MessageId: ERROR_SEEK
//
// MessageText:
//
//  The drive cannot locate a specific area or track on the disk.
//
#define ERROR_SEEK                       25L

//
// MessageId: ERROR_NOT_DOS_DISK
//
// MessageText:
//
//  The specified disk or diskette cannot be accessed.
//
#define ERROR_NOT_DOS_DISK               26L

//
// MessageId: ERROR_SECTOR_NOT_FOUND
//
// MessageText:
//
//  The drive cannot find the sector requested.
//
#define ERROR_SECTOR_NOT_FOUND           27L

//
// MessageId: ERROR_OUT_OF_PAPER
//
// MessageText:
//
//  The printer is out of paper.
//
#define ERROR_OUT_OF_PAPER               28L

//
// MessageId: ERROR_WRITE_FAULT
//
// MessageText:
//
//  The system cannot write to the specified device.
//
#define ERROR_WRITE_FAULT                29L

//
// MessageId: ERROR_READ_FAULT
//
// MessageText:
//
//  The system cannot read from the specified device.
//
#define ERROR_READ_FAULT                 30L

//
// MessageId: ERROR_GEN_FAILURE
//
// MessageText:
//
//  A device attached to the system is not functioning.
//
#define ERROR_GEN_FAILURE                31L

//
// MessageId: ERROR_SHARING_VIOLATION
//
// MessageText:
//
//  The process cannot access the file because it is being used by another process.
//
#define ERROR_SHARING_VIOLATION          32L

//
// MessageId: ERROR_LOCK_VIOLATION
//
// MessageText:
//
//  The process cannot access the file because another process has locked a portion of the file.
//
#define ERROR_LOCK_VIOLATION             33L

//
// MessageId: ERROR_WRONG_DISK
//
// MessageText:
//
//  The wrong diskette is in the drive.
//  Insert %2 (Volume Serial Number: %3) into drive %1.
//
#define ERROR_WRONG_DISK                 34L

//
// MessageId: ERROR_SHARING_BUFFER_EXCEEDED
//
// MessageText:
//
//  Too many files opened for sharing.
//
#define ERROR_SHARING_BUFFER_EXCEEDED    36L

//
// MessageId: ERROR_HANDLE_EOF
//
// MessageText:
//
//  Reached the end of the file.
//
#define ERROR_HANDLE_EOF                 38L

//
// MessageId: ERROR_HANDLE_DISK_FULL
//
// MessageText:
//
//  The disk is full.
//
#define ERROR_HANDLE_DISK_FULL           39L

//
// MessageId: ERROR_NOT_SUPPORTED
//
// MessageText:
//
//  The request is not supported.
//
#define ERROR_NOT_SUPPORTED              50L

//
// MessageId: ERROR_REM_NOT_LIST
//
// MessageText:
//
//  Windows cannot find the network path. Verify that the network path is correct and the destination computer is not busy or turned off. If Windows still cannot find the network path, contact your network administrator.
//
#define ERROR_REM_NOT_LIST               51L

//
// MessageId: ERROR_DUP_NAME
//
// MessageText:
//
//  You were not connected because a duplicate name exists on the network. Go to System in Control Panel to change the computer name and try again.
//
#define ERROR_DUP_NAME                   52L

//
// MessageId: ERROR_BAD_NETPATH
//
// MessageText:
//
//  The network path was not found.
//
#define ERROR_BAD_NETPATH                53L

//
// MessageId: ERROR_NETWORK_BUSY
//
// MessageText:
//
//  The network is busy.
//
#define ERROR_NETWORK_BUSY               54L

//
// MessageId: ERROR_DEV_NOT_EXIST
//
// MessageText:
//
//  The specified network resource or device is no longer available.
//
#define ERROR_DEV_NOT_EXIST              55L    // dderror

//
// MessageId: ERROR_TOO_MANY_CMDS
//
// MessageText:
//
//  The network BIOS command limit has been reached.
//
#define ERROR_TOO_MANY_CMDS              56L

//
// MessageId: ERROR_ADAP_HDW_ERR
//
// MessageText:
//
//  A network adapter hardware error occurred.
//
#define ERROR_ADAP_HDW_ERR               57L

//
// MessageId: ERROR_BAD_NET_RESP
//
// MessageText:
//
//  The specified server cannot perform the requested operation.
//
#define ERROR_BAD_NET_RESP               58L

//
// MessageId: ERROR_UNEXP_NET_ERR
//
// MessageText:
//
//  An unexpected network error occurred.
//
#define ERROR_UNEXP_NET_ERR              59L

//
// MessageId: ERROR_BAD_REM_ADAP
//
// MessageText:
//
//  The remote adapter is not compatible.
//
#define ERROR_BAD_REM_ADAP               60L

//
// MessageId: ERROR_PRINTQ_FULL
//
// MessageText:
//
//  The printer queue is full.
//
#define ERROR_PRINTQ_FULL                61L

//
// MessageId: ERROR_NO_SPOOL_SPACE
//
// MessageText:
//
//  Space to store the file waiting to be printed is not available on the server.
//
#define ERROR_NO_SPOOL_SPACE             62L

//
// MessageId: ERROR_PRINT_CANCELLED
//
// MessageText:
//
//  Your file waiting to be printed was deleted.
//
#define ERROR_PRINT_CANCELLED            63L

//
// MessageId: ERROR_NETNAME_DELETED
//
// MessageText:
//
//  The specified network name is no longer available.
//
#define ERROR_NETNAME_DELETED            64L

//
// MessageId: ERROR_NETWORK_ACCESS_DENIED
//
// MessageText:
//
//  Network access is denied.
//
#define ERROR_NETWORK_ACCESS_DENIED      65L

//
// MessageId: ERROR_BAD_DEV_TYPE
//
// MessageText:
//
//  The network resource type is not correct.
//
#define ERROR_BAD_DEV_TYPE               66L

//
// MessageId: ERROR_BAD_NET_NAME
//
// MessageText:
//
//  The network name cannot be found.
//
#define ERROR_BAD_NET_NAME               67L

//
// MessageId: ERROR_TOO_MANY_NAMES
//
// MessageText:
//
//  The name limit for the local computer network adapter card was exceeded.
//
#define ERROR_TOO_MANY_NAMES             68L

//
// MessageId: ERROR_TOO_MANY_SESS
//
// MessageText:
//
//  The network BIOS session limit was exceeded.
//
#define ERROR_TOO_MANY_SESS              69L

//
// MessageId: ERROR_SHARING_PAUSED
//
// MessageText:
//
//  The remote server has been paused or is in the process of being started.
//
#define ERROR_SHARING_PAUSED             70L

//
// MessageId: ERROR_REQ_NOT_ACCEP
//
// MessageText:
//
//  No more connections can be made to this remote computer at this time because there are already as many connections as the computer can accept.
//
#define ERROR_REQ_NOT_ACCEP              71L

//
// MessageId: ERROR_REDIR_PAUSED
//
// MessageText:
//
//  The specified printer or disk device has been paused.
//
#define ERROR_REDIR_PAUSED               72L

//
// MessageId: ERROR_FILE_EXISTS
//
// MessageText:
//
//  The file exists.
//
#define ERROR_FILE_EXISTS                80L

//
// MessageId: ERROR_CANNOT_MAKE
//
// MessageText:
//
//  The directory or file cannot be created.
//
#define ERROR_CANNOT_MAKE                82L

//
// MessageId: ERROR_FAIL_I24
//
// MessageText:
//
//  Fail on INT 24.
//
#define ERROR_FAIL_I24                   83L

//
// MessageId: ERROR_OUT_OF_STRUCTURES
//
// MessageText:
//
//  Storage to process this request is not available.
//
#define ERROR_OUT_OF_STRUCTURES          84L

//
// MessageId: ERROR_ALREADY_ASSIGNED
//
// MessageText:
//
//  The local device name is already in use.
//
#define ERROR_ALREADY_ASSIGNED           85L

//
// MessageId: ERROR_INVALID_PASSWORD
//
// MessageText:
//
//  The specified network password is not correct.
//
#define ERROR_INVALID_PASSWORD           86L

//
// MessageId: ERROR_INVALID_PARAMETER
//
// MessageText:
//
//  The parameter is incorrect.
//
#define ERROR_INVALID_PARAMETER          87L    // dderror

//
// MessageId: ERROR_NET_WRITE_FAULT
//
// MessageText:
//
//  A write fault occurred on the network.
//
#define ERROR_NET_WRITE_FAULT            88L

//
// MessageId: ERROR_NO_PROC_SLOTS
//
// MessageText:
//
//  The system cannot start another process at this time.
//
#define ERROR_NO_PROC_SLOTS              89L

//
// MessageId: ERROR_TOO_MANY_SEMAPHORES
//
// MessageText:
//
//  Cannot create another system semaphore.
//
#define ERROR_TOO_MANY_SEMAPHORES        100L

//
// MessageId: ERROR_EXCL_SEM_ALREADY_OWNED
//
// MessageText:
//
//  The exclusive semaphore is owned by another process.
//
#define ERROR_EXCL_SEM_ALREADY_OWNED     101L

//
// MessageId: ERROR_SEM_IS_SET
//
// MessageText:
//
//  The semaphore is set and cannot be closed.
//
#define ERROR_SEM_IS_SET                 102L

//
// MessageId: ERROR_TOO_MANY_SEM_REQUESTS
//
// MessageText:
//
//  The semaphore cannot be set again.
//
#define ERROR_TOO_MANY_SEM_REQUESTS      103L

//
// MessageId: ERROR_INVALID_AT_INTERRUPT_TIME
//
// MessageText:
//
//  Cannot request exclusive semaphores at interrupt time.
//
#define ERROR_INVALID_AT_INTERRUPT_TIME  104L

//
// MessageId: ERROR_SEM_OWNER_DIED
//
// MessageText:
//
//  The previous ownership of this semaphore has ended.
//
#define ERROR_SEM_OWNER_DIED             105L

//
// MessageId: ERROR_SEM_USER_LIMIT
//
// MessageText:
//
//  Insert the diskette for drive %1.
//
#define ERROR_SEM_USER_LIMIT             106L

//
// MessageId: ERROR_DISK_CHANGE
//
// MessageText:
//
//  The program stopped because an alternate diskette was not inserted.
//
#define ERROR_DISK_CHANGE                107L

//
// MessageId: ERROR_DRIVE_LOCKED
//
// MessageText:
//
//  The disk is in use or locked by another process.
//
#define ERROR_DRIVE_LOCKED               108L

//
// MessageId: ERROR_BROKEN_PIPE
//
// MessageText:
//
//  The pipe has been ended.
//
#define ERROR_BROKEN_PIPE                109L

//
// MessageId: ERROR_OPEN_FAILED
//
// MessageText:
//
//  The system cannot open the device or file specified.
//
#define ERROR_OPEN_FAILED                110L

//
// MessageId: ERROR_BUFFER_OVERFLOW
//
// MessageText:
//
//  The file name is too long.
//
#define ERROR_BUFFER_OVERFLOW            111L

//
// MessageId: ERROR_DISK_FULL
//
// MessageText:
//
//  There is not enough space on the disk.
//
#define ERROR_DISK_FULL                  112L

//
// MessageId: ERROR_NO_MORE_SEARCH_HANDLES
//
// MessageText:
//
//  No more internal file identifiers available.
//
#define ERROR_NO_MORE_SEARCH_HANDLES     113L

//
// MessageId: ERROR_INVALID_TARGET_HANDLE
//
// MessageText:
//
//  The target internal file identifier is incorrect.
//
#define ERROR_INVALID_TARGET_HANDLE      114L

//
// MessageId: ERROR_INVALID_CATEGORY
//
// MessageText:
//
//  The IOCTL call made by the application program is not correct.
//
#define ERROR_INVALID_CATEGORY           117L

//
// MessageId: ERROR_INVALID_VERIFY_SWITCH
//
// MessageText:
//
//  The verify-on-write switch parameter value is not correct.
//
#define ERROR_INVALID_VERIFY_SWITCH      118L

//
// MessageId: ERROR_BAD_DRIVER_LEVEL
//
// MessageText:
//
//  The system does not support the command requested.
//
#define ERROR_BAD_DRIVER_LEVEL           119L

//
// MessageId: ERROR_CALL_NOT_IMPLEMENTED
//
// MessageText:
//
//  This function is not supported on this system.
//
#define ERROR_CALL_NOT_IMPLEMENTED       120L

//
// MessageId: ERROR_SEM_TIMEOUT
//
// MessageText:
//
//  The semaphore timeout period has expired.
//
#define ERROR_SEM_TIMEOUT                121L

//
// MessageId: ERROR_INSUFFICIENT_BUFFER
//
// MessageText:
//
//  The data area passed to a system call is too small.
//
#define ERROR_INSUFFICIENT_BUFFER        122L    // dderror

//
// MessageId: ERROR_INVALID_NAME
//
// MessageText:
//
//  The filename, directory name, or volume label syntax is incorrect.
//
#define ERROR_INVALID_NAME               123L    // dderror

//
// MessageId: ERROR_INVALID_LEVEL
//
// MessageText:
//
//  The system call level is not correct.
//
#define ERROR_INVALID_LEVEL              124L

//
// MessageId: ERROR_NO_VOLUME_LABEL
//
// MessageText:
//
//  The disk has no volume label.
//
#define ERROR_NO_VOLUME_LABEL            125L

//
// MessageId: ERROR_MOD_NOT_FOUND
//
// MessageText:
//
//  The specified module could not be found.
//
#define ERROR_MOD_NOT_FOUND              126L

//
// MessageId: ERROR_PROC_NOT_FOUND
//
// MessageText:
//
//  The specified procedure could not be found.
//
#define ERROR_PROC_NOT_FOUND             127L

//
// MessageId: ERROR_WAIT_NO_CHILDREN
//
// MessageText:
//
//  There are no child processes to wait for.
//
#define ERROR_WAIT_NO_CHILDREN           128L

//
// MessageId: ERROR_CHILD_NOT_COMPLETE
//
// MessageText:
//
//  The %1 application cannot be run in Win32 mode.
//
#define ERROR_CHILD_NOT_COMPLETE         129L

//
// MessageId: ERROR_DIRECT_ACCESS_HANDLE
//
// MessageText:
//
//  Attempt to use a file handle to an open disk partition for an operation other than raw disk I/O.
//
#define ERROR_DIRECT_ACCESS_HANDLE       130L

//
// MessageId: ERROR_NEGATIVE_SEEK
//
// MessageText:
//
//  An attempt was made to move the file pointer before the beginning of the file.
//
#define ERROR_NEGATIVE_SEEK              131L

//
// MessageId: ERROR_SEEK_ON_DEVICE
//
// MessageText:
//
//  The file pointer cannot be set on the specified device or file.
//
#define ERROR_SEEK_ON_DEVICE             132L

//
// MessageId: ERROR_IS_JOIN_TARGET
//
// MessageText:
//
//  A JOIN or SUBST command cannot be used for a drive that contains previously joined drives.
//
#define ERROR_IS_JOIN_TARGET             133L

//
// MessageId: ERROR_IS_JOINED
//
// MessageText:
//
//  An attempt was made to use a JOIN or SUBST command on a drive that has already been joined.
//
#define ERROR_IS_JOINED                  134L

//
// MessageId: ERROR_IS_SUBSTED
//
// MessageText:
//
//  An attempt was made to use a JOIN or SUBST command on a drive that has already been substituted.
//
#define ERROR_IS_SUBSTED                 135L

//
// MessageId: ERROR_NOT_JOINED
//
// MessageText:
//
//  The system tried to delete the JOIN of a drive that is not joined.
//
#define ERROR_NOT_JOINED                 136L

//
// MessageId: ERROR_NOT_SUBSTED
//
// MessageText:
//
//  The system tried to delete the substitution of a drive that is not substituted.
//
#define ERROR_NOT_SUBSTED                137L

//
// MessageId: ERROR_JOIN_TO_JOIN
//
// MessageText:
//
//  The system tried to join a drive to a directory on a joined drive.
//
#define ERROR_JOIN_TO_JOIN               138L

//
// MessageId: ERROR_SUBST_TO_SUBST
//
// MessageText:
//
//  The system tried to substitute a drive to a directory on a substituted drive.
//
#define ERROR_SUBST_TO_SUBST             139L

//
// MessageId: ERROR_JOIN_TO_SUBST
//
// MessageText:
//
//  The system tried to join a drive to a directory on a substituted drive.
//
#define ERROR_JOIN_TO_SUBST              140L

//
// MessageId: ERROR_SUBST_TO_JOIN
//
// MessageText:
//
//  The system tried to SUBST a drive to a directory on a joined drive.
//
#define ERROR_SUBST_TO_JOIN              141L

//
// MessageId: ERROR_BUSY_DRIVE
//
// MessageText:
//
//  The system cannot perform a JOIN or SUBST at this time.
//
#define ERROR_BUSY_DRIVE                 142L

//
// MessageId: ERROR_SAME_DRIVE
//
// MessageText:
//
//  The system cannot join or substitute a drive to or for a directory on the same drive.
//
#define ERROR_SAME_DRIVE                 143L

//
// MessageId: ERROR_DIR_NOT_ROOT
//
// MessageText:
//
//  The directory is not a subdirectory of the root directory.
//
#define ERROR_DIR_NOT_ROOT               144L

//
// MessageId: ERROR_DIR_NOT_EMPTY
//
// MessageText:
//
//  The directory is not empty.
//
#define ERROR_DIR_NOT_EMPTY              145L

//
// MessageId: ERROR_IS_SUBST_PATH
//
// MessageText:
//
//  The path specified is being used in a substitute.
//
#define ERROR_IS_SUBST_PATH              146L

//
// MessageId: ERROR_IS_JOIN_PATH
//
// MessageText:
//
//  Not enough resources are available to process this command.
//
#define ERROR_IS_JOIN_PATH               147L

//
// MessageId: ERROR_PATH_BUSY
//
// MessageText:
//
//  The path specified cannot be used at this time.
//
#define ERROR_PATH_BUSY                  148L

//
// MessageId: ERROR_IS_SUBST_TARGET
//
// MessageText:
//
//  An attempt was made to join or substitute a drive for which a directory on the drive is the target of a previous substitute.
//
#define ERROR_IS_SUBST_TARGET            149L

//
// MessageId: ERROR_SYSTEM_TRACE
//
// MessageText:
//
//  System trace information was not specified in your CONFIG.SYS file, or tracing is disallowed.
//
#define ERROR_SYSTEM_TRACE               150L

//
// MessageId: ERROR_INVALID_EVENT_COUNT
//
// MessageText:
//
//  The number of specified semaphore events for DosMuxSemWait is not correct.
//
#define ERROR_INVALID_EVENT_COUNT        151L

//
// MessageId: ERROR_TOO_MANY_MUXWAITERS
//
// MessageText:
//
//  DosMuxSemWait did not execute; too many semaphores are already set.
//
#define ERROR_TOO_MANY_MUXWAITERS        152L

//
// MessageId: ERROR_INVALID_LIST_FORMAT
//
// MessageText:
//
//  The DosMuxSemWait list is not correct.
//
#define ERROR_INVALID_LIST_FORMAT        153L

//
// MessageId: ERROR_LABEL_TOO_LONG
//
// MessageText:
//
//  The volume label you entered exceeds the label character limit of the target file system.
//
#define ERROR_LABEL_TOO_LONG             154L

//
// MessageId: ERROR_TOO_MANY_TCBS
//
// MessageText:
//
//  Cannot create another thread.
//
#define ERROR_TOO_MANY_TCBS              155L

//
// MessageId: ERROR_SIGNAL_REFUSED
//
// MessageText:
//
//  The recipient process has refused the signal.
//
#define ERROR_SIGNAL_REFUSED             156L

//
// MessageId: ERROR_DISCARDED
//
// MessageText:
//
//  The segment is already discarded and cannot be locked.
//
#define ERROR_DISCARDED                  157L

//
// MessageId: ERROR_NOT_LOCKED
//
// MessageText:
//
//  The segment is already unlocked.
//
#define ERROR_NOT_LOCKED                 158L

//
// MessageId: ERROR_BAD_THREADID_ADDR
//
// MessageText:
//
//  The address for the thread ID is not correct.
//
#define ERROR_BAD_THREADID_ADDR          159L

//
// MessageId: ERROR_BAD_ARGUMENTS
//
// MessageText:
//
//  One or more arguments are not correct.
//
#define ERROR_BAD_ARGUMENTS              160L

//
// MessageId: ERROR_BAD_PATHNAME
//
// MessageText:
//
//  The specified path is invalid.
//
#define ERROR_BAD_PATHNAME               161L

//
// MessageId: ERROR_SIGNAL_PENDING
//
// MessageText:
//
//  A signal is already pending.
//
#define ERROR_SIGNAL_PENDING             162L

//
// MessageId: ERROR_MAX_THRDS_REACHED
//
// MessageText:
//
//  No more threads can be created in the system.
//
#define ERROR_MAX_THRDS_REACHED          164L

//
// MessageId: ERROR_LOCK_FAILED
//
// MessageText:
//
//  Unable to lock a region of a file.
//
#define ERROR_LOCK_FAILED                167L

//
// MessageId: ERROR_BUSY
//
// MessageText:
//
//  The requested resource is in use.
//
#define ERROR_BUSY                       170L    // dderror

//
// MessageId: ERROR_CANCEL_VIOLATION
//
// MessageText:
//
//  A lock request was not outstanding for the supplied cancel region.
//
#define ERROR_CANCEL_VIOLATION           173L

//
// MessageId: ERROR_ATOMIC_LOCKS_NOT_SUPPORTED
//
// MessageText:
//
//  The file system does not support atomic changes to the lock type.
//
#define ERROR_ATOMIC_LOCKS_NOT_SUPPORTED 174L

//
// MessageId: ERROR_INVALID_SEGMENT_NUMBER
//
// MessageText:
//
//  The system detected a segment number that was not correct.
//
#define ERROR_INVALID_SEGMENT_NUMBER     180L

//
// MessageId: ERROR_INVALID_ORDINAL
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_ORDINAL            182L

//
// MessageId: ERROR_ALREADY_EXISTS
//
// MessageText:
//
//  Cannot create a file when that file already exists.
//
#define ERROR_ALREADY_EXISTS             183L

//
// MessageId: ERROR_INVALID_FLAG_NUMBER
//
// MessageText:
//
//  The flag passed is not correct.
//
#define ERROR_INVALID_FLAG_NUMBER        186L

//
// MessageId: ERROR_SEM_NOT_FOUND
//
// MessageText:
//
//  The specified system semaphore name was not found.
//
#define ERROR_SEM_NOT_FOUND              187L

//
// MessageId: ERROR_INVALID_STARTING_CODESEG
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_STARTING_CODESEG   188L

//
// MessageId: ERROR_INVALID_STACKSEG
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_STACKSEG           189L

//
// MessageId: ERROR_INVALID_MODULETYPE
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_MODULETYPE         190L

//
// MessageId: ERROR_INVALID_EXE_SIGNATURE
//
// MessageText:
//
//  Cannot run %1 in Win32 mode.
//
#define ERROR_INVALID_EXE_SIGNATURE      191L

//
// MessageId: ERROR_EXE_MARKED_INVALID
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_EXE_MARKED_INVALID         192L

//
// MessageId: ERROR_BAD_EXE_FORMAT
//
// MessageText:
//
//  %1 is not a valid Win32 application.
//
#define ERROR_BAD_EXE_FORMAT             193L

//
// MessageId: ERROR_ITERATED_DATA_EXCEEDS_64k
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_ITERATED_DATA_EXCEEDS_64k  194L

//
// MessageId: ERROR_INVALID_MINALLOCSIZE
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_MINALLOCSIZE       195L

//
// MessageId: ERROR_DYNLINK_FROM_INVALID_RING
//
// MessageText:
//
//  The operating system cannot run this application program.
//
#define ERROR_DYNLINK_FROM_INVALID_RING  196L

//
// MessageId: ERROR_IOPL_NOT_ENABLED
//
// MessageText:
//
//  The operating system is not presently configured to run this application.
//
#define ERROR_IOPL_NOT_ENABLED           197L

//
// MessageId: ERROR_INVALID_SEGDPL
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INVALID_SEGDPL             198L

//
// MessageId: ERROR_AUTODATASEG_EXCEEDS_64k
//
// MessageText:
//
//  The operating system cannot run this application program.
//
#define ERROR_AUTODATASEG_EXCEEDS_64k    199L

//
// MessageId: ERROR_RING2SEG_MUST_BE_MOVABLE
//
// MessageText:
//
//  The code segment cannot be greater than or equal to 64K.
//
#define ERROR_RING2SEG_MUST_BE_MOVABLE   200L

//
// MessageId: ERROR_RELOC_CHAIN_XEEDS_SEGLIM
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM   201L

//
// MessageId: ERROR_INFLOOP_IN_RELOC_CHAIN
//
// MessageText:
//
//  The operating system cannot run %1.
//
#define ERROR_INFLOOP_IN_RELOC_CHAIN     202L

//
// MessageId: ERROR_ENVVAR_NOT_FOUND
//
// MessageText:
//
//  The system could not find the environment option that was entered.
//
#define ERROR_ENVVAR_NOT_FOUND           203L

//
// MessageId: ERROR_NO_SIGNAL_SENT
//
// MessageText:
//
//  No process in the command subtree has a signal handler.
//
#define ERROR_NO_SIGNAL_SENT             205L

//
// MessageId: ERROR_FILENAME_EXCED_RANGE
//
// MessageText:
//
//  The filename or extension is too long.
//
#define ERROR_FILENAME_EXCED_RANGE       206L

//
// MessageId: ERROR_RING2_STACK_IN_USE
//
// MessageText:
//
//  The ring 2 stack is in use.
//
#define ERROR_RING2_STACK_IN_USE         207L

//
// MessageId: ERROR_META_EXPANSION_TOO_LONG
//
// MessageText:
//
//  The global filename characters, * or ?, are entered incorrectly or too many global filename characters are specified.
//
#define ERROR_META_EXPANSION_TOO_LONG    208L

//
// MessageId: ERROR_INVALID_SIGNAL_NUMBER
//
// MessageText:
//
//  The signal being posted is not correct.
//
#define ERROR_INVALID_SIGNAL_NUMBER      209L

//
// MessageId: ERROR_THREAD_1_INACTIVE
//
// MessageText:
//
//  The signal handler cannot be set.
//
#define ERROR_THREAD_1_INACTIVE          210L

//
// MessageId: ERROR_LOCKED
//
// MessageText:
//
//  The segment is locked and cannot be reallocated.
//
#define ERROR_LOCKED                     212L

//
// MessageId: ERROR_TOO_MANY_MODULES
//
// MessageText:
//
//  Too many dynamic-link modules are attached to this program or dynamic-link module.
//
#define ERROR_TOO_MANY_MODULES           214L

//
// MessageId: ERROR_NESTING_NOT_ALLOWED
//
// MessageText:
//
//  Cannot nest calls to LoadModule.
//
#define ERROR_NESTING_NOT_ALLOWED        215L

//
// MessageId: ERROR_EXE_MACHINE_TYPE_MISMATCH
//
// MessageText:
//
//  The image file %1 is valid, but is for a machine type other than the current machine.
//
#define ERROR_EXE_MACHINE_TYPE_MISMATCH  216L

//
// MessageId: ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY
//
// MessageText:
//
//  The image file %1 is signed, unable to modify.
//
#define ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY 217L

//
// MessageId: ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY
//
// MessageText:
//
//  The image file %1 is strong signed, unable to modify.
//
#define ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY 218L

//
// MessageId: ERROR_BAD_PIPE
//
// MessageText:
//
//  The pipe state is invalid.
//
#define ERROR_BAD_PIPE                   230L

//
// MessageId: ERROR_PIPE_BUSY
//
// MessageText:
//
//  All pipe instances are busy.
//
#define ERROR_PIPE_BUSY                  231L

//
// MessageId: ERROR_NO_DATA
//
// MessageText:
//
//  The pipe is being closed.
//
#define ERROR_NO_DATA                    232L

//
// MessageId: ERROR_PIPE_NOT_CONNECTED
//
// MessageText:
//
//  No process is on the other end of the pipe.
//
#define ERROR_PIPE_NOT_CONNECTED         233L

//
// MessageId: ERROR_MORE_DATA
//
// MessageText:
//
//  More data is available.
//
#define ERROR_MORE_DATA                  234L    // dderror

//
// MessageId: ERROR_VC_DISCONNECTED
//
// MessageText:
//
//  The session was canceled.
//
#define ERROR_VC_DISCONNECTED            240L

//
// MessageId: ERROR_INVALID_EA_NAME
//
// MessageText:
//
//  The specified extended attribute name was invalid.
//
#define ERROR_INVALID_EA_NAME            254L

//
// MessageId: ERROR_EA_LIST_INCONSISTENT
//
// MessageText:
//
//  The extended attributes are inconsistent.
//
#define ERROR_EA_LIST_INCONSISTENT       255L

//
// MessageId: WAIT_TIMEOUT
//
// MessageText:
//
//  The wait operation timed out.
//
#define WAIT_TIMEOUT                     258L    // dderror

//
// MessageId: ERROR_NO_MORE_ITEMS
//
// MessageText:
//
//  No more data is available.
//
#define ERROR_NO_MORE_ITEMS              259L

//
// MessageId: ERROR_CANNOT_COPY
//
// MessageText:
//
//  The copy functions cannot be used.
//
#define ERROR_CANNOT_COPY                266L

//
// MessageId: ERROR_DIRECTORY
//
// MessageText:
//
//  The directory name is invalid.
//
#define ERROR_DIRECTORY                  267L

//
// MessageId: ERROR_EAS_DIDNT_FIT
//
// MessageText:
//
//  The extended attributes did not fit in the buffer.
//
#define ERROR_EAS_DIDNT_FIT              275L

//
// MessageId: ERROR_EA_FILE_CORRUPT
//
// MessageText:
//
//  The extended attribute file on the mounted file system is corrupt.
//
#define ERROR_EA_FILE_CORRUPT            276L

//
// MessageId: ERROR_EA_TABLE_FULL
//
// MessageText:
//
//  The extended attribute table file is full.
//
#define ERROR_EA_TABLE_FULL              277L

//
// MessageId: ERROR_INVALID_EA_HANDLE
//
// MessageText:
//
//  The specified extended attribute handle is invalid.
//
#define ERROR_INVALID_EA_HANDLE          278L

//
// MessageId: ERROR_EAS_NOT_SUPPORTED
//
// MessageText:
//
//  The mounted file system does not support extended attributes.
//
#define ERROR_EAS_NOT_SUPPORTED          282L

//
// MessageId: ERROR_NOT_OWNER
//
// MessageText:
//
//  Attempt to release mutex not owned by caller.
//
#define ERROR_NOT_OWNER                  288L

//
// MessageId: ERROR_TOO_MANY_POSTS
//
// MessageText:
//
//  Too many posts were made to a semaphore.
//
#define ERROR_TOO_MANY_POSTS             298L

//
// MessageId: ERROR_PARTIAL_COPY
//
// MessageText:
//
//  Only part of a ReadProcessMemory or WriteProcessMemory request was completed.
//
#define ERROR_PARTIAL_COPY               299L

//
// MessageId: ERROR_OPLOCK_NOT_GRANTED
//
// MessageText:
//
//  The oplock request is denied.
//
#define ERROR_OPLOCK_NOT_GRANTED         300L

//
// MessageId: ERROR_INVALID_OPLOCK_PROTOCOL
//
// MessageText:
//
//  An invalid oplock acknowledgment was received by the system.
//
#define ERROR_INVALID_OPLOCK_PROTOCOL    301L

//
// MessageId: ERROR_DISK_TOO_FRAGMENTED
//
// MessageText:
//
//  The volume is too fragmented to complete this operation.
//
#define ERROR_DISK_TOO_FRAGMENTED        302L

//
// MessageId: ERROR_DELETE_PENDING
//
// MessageText:
//
//  The file cannot be opened because it is in the process of being deleted.
//
#define ERROR_DELETE_PENDING             303L

//
// MessageId: ERROR_MR_MID_NOT_FOUND
//
// MessageText:
//
//  The system cannot find message text for message number 0x%1 in the message file for %2.
//
#define ERROR_MR_MID_NOT_FOUND           317L

//
// MessageId: ERROR_SCOPE_NOT_FOUND
//
// MessageText:
//
//  The scope specified was not found.
//
#define ERROR_SCOPE_NOT_FOUND            318L

//
// MessageId: ERROR_INVALID_ADDRESS
//
// MessageText:
//
//  Attempt to access invalid address.
//
#define ERROR_INVALID_ADDRESS            487L

//
// MessageId: ERROR_ARITHMETIC_OVERFLOW
//
// MessageText:
//
//  Arithmetic result exceeded 32 bits.
//
#define ERROR_ARITHMETIC_OVERFLOW        534L

//
// MessageId: ERROR_PIPE_CONNECTED
//
// MessageText:
//
//  There is a process on other end of the pipe.
//
#define ERROR_PIPE_CONNECTED             535L

//
// MessageId: ERROR_PIPE_LISTENING
//
// MessageText:
//
//  Waiting for a process to open the other end of the pipe.
//
#define ERROR_PIPE_LISTENING             536L

//
// MessageId: ERROR_EA_ACCESS_DENIED
//
// MessageText:
//
//  Access to the extended attribute was denied.
//
#define ERROR_EA_ACCESS_DENIED           994L

//
// MessageId: ERROR_OPERATION_ABORTED
//
// MessageText:
//
//  The I/O operation has been aborted because of either a thread exit or an application request.
//
#define ERROR_OPERATION_ABORTED          995L

//
// MessageId: ERROR_IO_INCOMPLETE
//
// MessageText:
//
//  Overlapped I/O event is not in a signaled state.
//
#define ERROR_IO_INCOMPLETE              996L

//
// MessageId: ERROR_IO_PENDING
//
// MessageText:
//
//  Overlapped I/O operation is in progress.
//
#define ERROR_IO_PENDING                 997L    // dderror

//
// MessageId: ERROR_NOACCESS
//
// MessageText:
//
//  Invalid access to memory location.
//
#define ERROR_NOACCESS                   998L

//
// MessageId: ERROR_SWAPERROR
//
// MessageText:
//
//  Error performing inpage operation.
//
#define ERROR_SWAPERROR                  999L

//
// MessageId: ERROR_STACK_OVERFLOW
//
// MessageText:
//
//  Recursion too deep; the stack overflowed.
//
#define ERROR_STACK_OVERFLOW             1001L

//
// MessageId: ERROR_INVALID_MESSAGE
//
// MessageText:
//
//  The window cannot act on the sent message.
//
#define ERROR_INVALID_MESSAGE            1002L

//
// MessageId: ERROR_CAN_NOT_COMPLETE
//
// MessageText:
//
//  Cannot complete this function.
//
#define ERROR_CAN_NOT_COMPLETE           1003L

//
// MessageId: ERROR_INVALID_FLAGS
//
// MessageText:
//
//  Invalid flags.
//
#define ERROR_INVALID_FLAGS              1004L

//
// MessageId: ERROR_UNRECOGNIZED_VOLUME
//
// MessageText:
//
//  The volume does not contain a recognized file system.
//  Please make sure that all required file system drivers are loaded and that the volume is not corrupted.
//
#define ERROR_UNRECOGNIZED_VOLUME        1005L

//
// MessageId: ERROR_FILE_INVALID
//
// MessageText:
//
//  The volume for a file has been externally altered so that the opened file is no longer valid.
//
#define ERROR_FILE_INVALID               1006L

//
// MessageId: ERROR_FULLSCREEN_MODE
//
// MessageText:
//
//  The requested operation cannot be performed in full-screen mode.
//
#define ERROR_FULLSCREEN_MODE            1007L

//
// MessageId: ERROR_NO_TOKEN
//
// MessageText:
//
//  An attempt was made to reference a token that does not exist.
//
#define ERROR_NO_TOKEN                   1008L

//
// MessageId: ERROR_BADDB
//
// MessageText:
//
//  The configuration registry database is corrupt.
//
#define ERROR_BADDB                      1009L

//
// MessageId: ERROR_BADKEY
//
// MessageText:
//
//  The configuration registry key is invalid.
//
#define ERROR_BADKEY                     1010L

//
// MessageId: ERROR_CANTOPEN
//
// MessageText:
//
//  The configuration registry key could not be opened.
//
#define ERROR_CANTOPEN                   1011L

//
// MessageId: ERROR_CANTREAD
//
// MessageText:
//
//  The configuration registry key could not be read.
//
#define ERROR_CANTREAD                   1012L

//
// MessageId: ERROR_CANTWRITE
//
// MessageText:
//
//  The configuration registry key could not be written.
//
#define ERROR_CANTWRITE                  1013L

//
// MessageId: ERROR_REGISTRY_RECOVERED
//
// MessageText:
//
//  One of the files in the registry database had to be recovered by use of a log or alternate copy. The recovery was successful.
//
#define ERROR_REGISTRY_RECOVERED         1014L

//
// MessageId: ERROR_REGISTRY_CORRUPT
//
// MessageText:
//
//  The registry is corrupted. The structure of one of the files containing registry data is corrupted, or the system's memory image of the file is corrupted, or the file could not be recovered because the alternate copy or log was absent or corrupted.
//
#define ERROR_REGISTRY_CORRUPT           1015L

//
// MessageId: ERROR_REGISTRY_IO_FAILED
//
// MessageText:
//
//  An I/O operation initiated by the registry failed unrecoverably. The registry could not read in, or write out, or flush, one of the files that contain the system's image of the registry.
//
#define ERROR_REGISTRY_IO_FAILED         1016L

//
// MessageId: ERROR_NOT_REGISTRY_FILE
//
// MessageText:
//
//  The system has attempted to load or restore a file into the registry, but the specified file is not in a registry file format.
//
#define ERROR_NOT_REGISTRY_FILE          1017L

//
// MessageId: ERROR_KEY_DELETED
//
// MessageText:
//
//  Illegal operation attempted on a registry key that has been marked for deletion.
//
#define ERROR_KEY_DELETED                1018L

//
// MessageId: ERROR_NO_LOG_SPACE
//
// MessageText:
//
//  System could not allocate the required space in a registry log.
//
#define ERROR_NO_LOG_SPACE               1019L

//
// MessageId: ERROR_KEY_HAS_CHILDREN
//
// MessageText:
//
//  Cannot create a symbolic link in a registry key that already has subkeys or values.
//
#define ERROR_KEY_HAS_CHILDREN           1020L

//
// MessageId: ERROR_CHILD_MUST_BE_VOLATILE
//
// MessageText:
//
//  Cannot create a stable subkey under a volatile parent key.
//
#define ERROR_CHILD_MUST_BE_VOLATILE     1021L

//
// MessageId: ERROR_NOTIFY_ENUM_DIR
//
// MessageText:
//
//  A notify change request is being completed and the information is not being returned in the caller's buffer. The caller now needs to enumerate the files to find the changes.
//
#define ERROR_NOTIFY_ENUM_DIR            1022L

//
// MessageId: ERROR_DEPENDENT_SERVICES_RUNNING
//
// MessageText:
//
//  A stop control has been sent to a service that other running services are dependent on.
//
#define ERROR_DEPENDENT_SERVICES_RUNNING 1051L

//
// MessageId: ERROR_INVALID_SERVICE_CONTROL
//
// MessageText:
//
//  The requested control is not valid for this service.
//
#define ERROR_INVALID_SERVICE_CONTROL    1052L

//
// MessageId: ERROR_SERVICE_REQUEST_TIMEOUT
//
// MessageText:
//
//  The service did not respond to the start or control request in a timely fashion.
//
#define ERROR_SERVICE_REQUEST_TIMEOUT    1053L

//
// MessageId: ERROR_SERVICE_NO_THREAD
//
// MessageText:
//
//  A thread could not be created for the service.
//
#define ERROR_SERVICE_NO_THREAD          1054L

//
// MessageId: ERROR_SERVICE_DATABASE_LOCKED
//
// MessageText:
//
//  The service database is locked.
//
#define ERROR_SERVICE_DATABASE_LOCKED    1055L

//
// MessageId: ERROR_SERVICE_ALREADY_RUNNING
//
// MessageText:
//
//  An instance of the service is already running.
//
#define ERROR_SERVICE_ALREADY_RUNNING    1056L

//
// MessageId: ERROR_INVALID_SERVICE_ACCOUNT
//
// MessageText:
//
//  The account name is invalid or does not exist, or the password is invalid for the account name specified.
//
#define ERROR_INVALID_SERVICE_ACCOUNT    1057L

//
// MessageId: ERROR_SERVICE_DISABLED
//
// MessageText:
//
//  The service cannot be started, either because it is disabled or because it has no enabled devices associated with it.
//
#define ERROR_SERVICE_DISABLED           1058L

//
// MessageId: ERROR_CIRCULAR_DEPENDENCY
//
// MessageText:
//
//  Circular service dependency was specified.
//
#define ERROR_CIRCULAR_DEPENDENCY        1059L

//
// MessageId: ERROR_SERVICE_DOES_NOT_EXIST
//
// MessageText:
//
//  The specified service does not exist as an installed service.
//
#define ERROR_SERVICE_DOES_NOT_EXIST     1060L

//
// MessageId: ERROR_SERVICE_CANNOT_ACCEPT_CTRL
//
// MessageText:
//
//  The service cannot accept control messages at this time.
//
#define ERROR_SERVICE_CANNOT_ACCEPT_CTRL 1061L

//
// MessageId: ERROR_SERVICE_NOT_ACTIVE
//
// MessageText:
//
//  The service has not been started.
//
#define ERROR_SERVICE_NOT_ACTIVE         1062L

//
// MessageId: ERROR_FAILED_SERVICE_CONTROLLER_CONNECT
//
// MessageText:
//
//  The service process could not connect to the service controller.
//
#define ERROR_FAILED_SERVICE_CONTROLLER_CONNECT 1063L

//
// MessageId: ERROR_EXCEPTION_IN_SERVICE
//
// MessageText:
//
//  An exception occurred in the service when handling the control request.
//
#define ERROR_EXCEPTION_IN_SERVICE       1064L

//
// MessageId: ERROR_DATABASE_DOES_NOT_EXIST
//
// MessageText:
//
//  The database specified does not exist.
//
#define ERROR_DATABASE_DOES_NOT_EXIST    1065L

//
// MessageId: ERROR_SERVICE_SPECIFIC_ERROR
//
// MessageText:
//
//  The service has returned a service-specific error code.
//
#define ERROR_SERVICE_SPECIFIC_ERROR     1066L

//
// MessageId: ERROR_PROCESS_ABORTED
//
// MessageText:
//
//  The process terminated unexpectedly.
//
#define ERROR_PROCESS_ABORTED            1067L

//
// MessageId: ERROR_SERVICE_DEPENDENCY_FAIL
//
// MessageText:
//
//  The dependency service or group failed to start.
//
#define ERROR_SERVICE_DEPENDENCY_FAIL    1068L

//
// MessageId: ERROR_SERVICE_LOGON_FAILED
//
// MessageText:
//
//  The service did not start due to a logon failure.
//
#define ERROR_SERVICE_LOGON_FAILED       1069L

//
// MessageId: ERROR_SERVICE_START_HANG
//
// MessageText:
//
//  After starting, the service hung in a start-pending state.
//
#define ERROR_SERVICE_START_HANG         1070L

//
// MessageId: ERROR_INVALID_SERVICE_LOCK
//
// MessageText:
//
//  The specified service database lock is invalid.
//
#define ERROR_INVALID_SERVICE_LOCK       1071L

//
// MessageId: ERROR_SERVICE_MARKED_FOR_DELETE
//
// MessageText:
//
//  The specified service has been marked for deletion.
//
#define ERROR_SERVICE_MARKED_FOR_DELETE  1072L

//
// MessageId: ERROR_SERVICE_EXISTS
//
// MessageText:
//
//  The specified service already exists.
//
#define ERROR_SERVICE_EXISTS             1073L

//
// MessageId: ERROR_ALREADY_RUNNING_LKG
//
// MessageText:
//
//  The system is currently running with the last-known-good configuration.
//
#define ERROR_ALREADY_RUNNING_LKG        1074L

//
// MessageId: ERROR_SERVICE_DEPENDENCY_DELETED
//
// MessageText:
//
//  The dependency service does not exist or has been marked for deletion.
//
#define ERROR_SERVICE_DEPENDENCY_DELETED 1075L

//
// MessageId: ERROR_BOOT_ALREADY_ACCEPTED
//
// MessageText:
//
//  The current boot has already been accepted for use as the last-known-good control set.
//
#define ERROR_BOOT_ALREADY_ACCEPTED      1076L

//
// MessageId: ERROR_SERVICE_NEVER_STARTED
//
// MessageText:
//
//  No attempts to start the service have been made since the last boot.
//
#define ERROR_SERVICE_NEVER_STARTED      1077L

//
// MessageId: ERROR_DUPLICATE_SERVICE_NAME
//
// MessageText:
//
//  The name is already in use as either a service name or a service display name.
//
#define ERROR_DUPLICATE_SERVICE_NAME     1078L

//
// MessageId: ERROR_DIFFERENT_SERVICE_ACCOUNT
//
// MessageText:
//
//  The account specified for this service is different from the account specified for other services running in the same process.
//
#define ERROR_DIFFERENT_SERVICE_ACCOUNT  1079L

//
// MessageId: ERROR_CANNOT_DETECT_DRIVER_FAILURE
//
// MessageText:
//
//  Failure actions can only be set for Win32 services, not for drivers.
//
#define ERROR_CANNOT_DETECT_DRIVER_FAILURE 1080L

//
// MessageId: ERROR_CANNOT_DETECT_PROCESS_ABORT
//
// MessageText:
//
//  This service runs in the same process as the service control manager.
//  Therefore, the service control manager cannot take action if this service's process terminates unexpectedly.
//
#define ERROR_CANNOT_DETECT_PROCESS_ABORT 1081L

//
// MessageId: ERROR_NO_RECOVERY_PROGRAM
//
// MessageText:
//
//  No recovery program has been configured for this service.
//
#define ERROR_NO_RECOVERY_PROGRAM        1082L

//
// MessageId: ERROR_SERVICE_NOT_IN_EXE
//
// MessageText:
//
//  The executable program that this service is configured to run in does not implement the service.
//
#define ERROR_SERVICE_NOT_IN_EXE         1083L

//
// MessageId: ERROR_NOT_SAFEBOOT_SERVICE
//
// MessageText:
//
//  This service cannot be started in Safe Mode
//
#define ERROR_NOT_SAFEBOOT_SERVICE       1084L

//
// MessageId: ERROR_END_OF_MEDIA
//
// MessageText:
//
//  The physical end of the tape has been reached.
//
#define ERROR_END_OF_MEDIA               1100L

//
// MessageId: ERROR_FILEMARK_DETECTED
//
// MessageText:
//
//  A tape access reached a filemark.
//
#define ERROR_FILEMARK_DETECTED          1101L

//
// MessageId: ERROR_BEGINNING_OF_MEDIA
//
// MessageText:
//
//  The beginning of the tape or a partition was encountered.
//
#define ERROR_BEGINNING_OF_MEDIA         1102L

//
// MessageId: ERROR_SETMARK_DETECTED
//
// MessageText:
//
//  A tape access reached the end of a set of files.
//
#define ERROR_SETMARK_DETECTED           1103L

//
// MessageId: ERROR_NO_DATA_DETECTED
//
// MessageText:
//
//  No more data is on the tape.
//
#define ERROR_NO_DATA_DETECTED           1104L

//
// MessageId: ERROR_PARTITION_FAILURE
//
// MessageText:
//
//  Tape could not be partitioned.
//
#define ERROR_PARTITION_FAILURE          1105L

//
// MessageId: ERROR_INVALID_BLOCK_LENGTH
//
// MessageText:
//
//  When accessing a new tape of a multivolume partition, the current block size is incorrect.
//
#define ERROR_INVALID_BLOCK_LENGTH       1106L

//
// MessageId: ERROR_DEVICE_NOT_PARTITIONED
//
// MessageText:
//
//  Tape partition information could not be found when loading a tape.
//
#define ERROR_DEVICE_NOT_PARTITIONED     1107L

//
// MessageId: ERROR_UNABLE_TO_LOCK_MEDIA
//
// MessageText:
//
//  Unable to lock the media eject mechanism.
//
#define ERROR_UNABLE_TO_LOCK_MEDIA       1108L

//
// MessageId: ERROR_UNABLE_TO_UNLOAD_MEDIA
//
// MessageText:
//
//  Unable to unload the media.
//
#define ERROR_UNABLE_TO_UNLOAD_MEDIA     1109L

//
// MessageId: ERROR_MEDIA_CHANGED
//
// MessageText:
//
//  The media in the drive may have changed.
//
#define ERROR_MEDIA_CHANGED              1110L

//
// MessageId: ERROR_BUS_RESET
//
// MessageText:
//
//  The I/O bus was reset.
//
#define ERROR_BUS_RESET                  1111L

//
// MessageId: ERROR_NO_MEDIA_IN_DRIVE
//
// MessageText:
//
//  No media in drive.
//
#define ERROR_NO_MEDIA_IN_DRIVE          1112L

//
// MessageId: ERROR_NO_UNICODE_TRANSLATION
//
// MessageText:
//
//  No mapping for the Unicode character exists in the target multi-byte code page.
//
#define ERROR_NO_UNICODE_TRANSLATION     1113L

//
// MessageId: ERROR_DLL_INIT_FAILED
//
// MessageText:
//
//  A dynamic link library (DLL) initialization routine failed.
//
#define ERROR_DLL_INIT_FAILED            1114L

//
// MessageId: ERROR_SHUTDOWN_IN_PROGRESS
//
// MessageText:
//
//  A system shutdown is in progress.
//
#define ERROR_SHUTDOWN_IN_PROGRESS       1115L

//
// MessageId: ERROR_NO_SHUTDOWN_IN_PROGRESS
//
// MessageText:
//
//  Unable to abort the system shutdown because no shutdown was in progress.
//
#define ERROR_NO_SHUTDOWN_IN_PROGRESS    1116L

//
// MessageId: ERROR_IO_DEVICE
//
// MessageText:
//
//  The request could not be performed because of an I/O device error.
//
#define ERROR_IO_DEVICE                  1117L

//
// MessageId: ERROR_SERIAL_NO_DEVICE
//
// MessageText:
//
//  No serial device was successfully initialized. The serial driver will unload.
//
#define ERROR_SERIAL_NO_DEVICE           1118L

//
// MessageId: ERROR_IRQ_BUSY
//
// MessageText:
//
//  Unable to open a device that was sharing an interrupt request (IRQ) with other devices. At least one other device that uses that IRQ was already opened.
//
#define ERROR_IRQ_BUSY                   1119L

//
// MessageId: ERROR_MORE_WRITES
//
// MessageText:
//
//  A serial I/O operation was completed by another write to the serial port.
//  (The IOCTL_SERIAL_XOFF_COUNTER reached zero.)
//
#define ERROR_MORE_WRITES                1120L

//
// MessageId: ERROR_COUNTER_TIMEOUT
//
// MessageText:
//
//  A serial I/O operation completed because the timeout period expired.
//  (The IOCTL_SERIAL_XOFF_COUNTER did not reach zero.)
//
#define ERROR_COUNTER_TIMEOUT            1121L

//
// MessageId: ERROR_FLOPPY_ID_MARK_NOT_FOUND
//
// MessageText:
//
//  No ID address mark was found on the floppy disk.
//
#define ERROR_FLOPPY_ID_MARK_NOT_FOUND   1122L

//
// MessageId: ERROR_FLOPPY_WRONG_CYLINDER
//
// MessageText:
//
//  Mismatch between the floppy disk sector ID field and the floppy disk controller track address.
//
#define ERROR_FLOPPY_WRONG_CYLINDER      1123L

//
// MessageId: ERROR_FLOPPY_UNKNOWN_ERROR
//
// MessageText:
//
//  The floppy disk controller reported an error that is not recognized by the floppy disk driver.
//
#define ERROR_FLOPPY_UNKNOWN_ERROR       1124L

//
// MessageId: ERROR_FLOPPY_BAD_REGISTERS
//
// MessageText:
//
//  The floppy disk controller returned inconsistent results in its registers.
//
#define ERROR_FLOPPY_BAD_REGISTERS       1125L

//
// MessageId: ERROR_DISK_RECALIBRATE_FAILED
//
// MessageText:
//
//  While accessing the hard disk, a recalibrate operation failed, even after retries.
//
#define ERROR_DISK_RECALIBRATE_FAILED    1126L

//
// MessageId: ERROR_DISK_OPERATION_FAILED
//
// MessageText:
//
//  While accessing the hard disk, a disk operation failed even after retries.
//
#define ERROR_DISK_OPERATION_FAILED      1127L

//
// MessageId: ERROR_DISK_RESET_FAILED
//
// MessageText:
//
//  While accessing the hard disk, a disk controller reset was needed, but even that failed.
//
#define ERROR_DISK_RESET_FAILED          1128L

//
// MessageId: ERROR_EOM_OVERFLOW
//
// MessageText:
//
//  Physical end of tape encountered.
//
#define ERROR_EOM_OVERFLOW               1129L

//
// MessageId: ERROR_NOT_ENOUGH_SERVER_MEMORY
//
// MessageText:
//
//  Not enough server storage is available to process this command.
//
#define ERROR_NOT_ENOUGH_SERVER_MEMORY   1130L

//
// MessageId: ERROR_POSSIBLE_DEADLOCK
//
// MessageText:
//
//  A potential deadlock condition has been detected.
//
#define ERROR_POSSIBLE_DEADLOCK          1131L

//
// MessageId: ERROR_MAPPED_ALIGNMENT
//
// MessageText:
//
//  The base address or the file offset specified does not have the proper alignment.
//
#define ERROR_MAPPED_ALIGNMENT           1132L

//
// MessageId: ERROR_SET_POWER_STATE_VETOED
//
// MessageText:
//
//  An attempt to change the system power state was vetoed by another application or driver.
//
#define ERROR_SET_POWER_STATE_VETOED     1140L

//
// MessageId: ERROR_SET_POWER_STATE_FAILED
//
// MessageText:
//
//  The system BIOS failed an attempt to change the system power state.
//
#define ERROR_SET_POWER_STATE_FAILED     1141L

//
// MessageId: ERROR_TOO_MANY_LINKS
//
// MessageText:
//
//  An attempt was made to create more links on a file than the file system supports.
//
#define ERROR_TOO_MANY_LINKS             1142L

//
// MessageId: ERROR_OLD_WIN_VERSION
//
// MessageText:
//
//  The specified program requires a newer version of Windows.
//
#define ERROR_OLD_WIN_VERSION            1150L

//
// MessageId: ERROR_APP_WRONG_OS
//
// MessageText:
//
//  The specified program is not a Windows or MS-DOS program.
//
#define ERROR_APP_WRONG_OS               1151L

//
// MessageId: ERROR_SINGLE_INSTANCE_APP
//
// MessageText:
//
//  Cannot start more than one instance of the specified program.
//
#define ERROR_SINGLE_INSTANCE_APP        1152L

//
// MessageId: ERROR_RMODE_APP
//
// MessageText:
//
//  The specified program was written for an earlier version of Windows.
//
#define ERROR_RMODE_APP                  1153L

//
// MessageId: ERROR_INVALID_DLL
//
// MessageText:
//
//  One of the library files needed to run this application is damaged.
//
#define ERROR_INVALID_DLL                1154L

//
// MessageId: ERROR_NO_ASSOCIATION
//
// MessageText:
//
//  No application is associated with the specified file for this operation.
//
#define ERROR_NO_ASSOCIATION             1155L

//
// MessageId: ERROR_DDE_FAIL
//
// MessageText:
//
//  An error occurred in sending the command to the application.
//
#define ERROR_DDE_FAIL                   1156L

//
// MessageId: ERROR_DLL_NOT_FOUND
//
// MessageText:
//
//  One of the library files needed to run this application cannot be found.
//
#define ERROR_DLL_NOT_FOUND              1157L

//
// MessageId: ERROR_NO_MORE_USER_HANDLES
//
// MessageText:
//
//  The current process has used all of its system allowance of handles for Window Manager objects.
//
#define ERROR_NO_MORE_USER_HANDLES       1158L

//
// MessageId: ERROR_MESSAGE_SYNC_ONLY
//
// MessageText:
//
//  The message can be used only with synchronous operations.
//
#define ERROR_MESSAGE_SYNC_ONLY          1159L

//
// MessageId: ERROR_SOURCE_ELEMENT_EMPTY
//
// MessageText:
//
//  The indicated source element has no media.
//
#define ERROR_SOURCE_ELEMENT_EMPTY       1160L

//
// MessageId: ERROR_DESTINATION_ELEMENT_FULL
//
// MessageText:
//
//  The indicated destination element already contains media.
//
#define ERROR_DESTINATION_ELEMENT_FULL   1161L

//
// MessageId: ERROR_ILLEGAL_ELEMENT_ADDRESS
//
// MessageText:
//
//  The indicated element does not exist.
//
#define ERROR_ILLEGAL_ELEMENT_ADDRESS    1162L

//
// MessageId: ERROR_MAGAZINE_NOT_PRESENT
//
// MessageText:
//
//  The indicated element is part of a magazine that is not present.
//
#define ERROR_MAGAZINE_NOT_PRESENT       1163L

//
// MessageId: ERROR_DEVICE_REINITIALIZATION_NEEDED
//
// MessageText:
//
//  The indicated device requires reinitialization due to hardware errors.
//
#define ERROR_DEVICE_REINITIALIZATION_NEEDED 1164L    // dderror

//
// MessageId: ERROR_DEVICE_REQUIRES_CLEANING
//
// MessageText:
//
//  The device has indicated that cleaning is required before further operations are attempted.
//
#define ERROR_DEVICE_REQUIRES_CLEANING   1165L

//
// MessageId: ERROR_DEVICE_DOOR_OPEN
//
// MessageText:
//
//  The device has indicated that its door is open.
//
#define ERROR_DEVICE_DOOR_OPEN           1166L

//
// MessageId: ERROR_DEVICE_NOT_CONNECTED
//
// MessageText:
//
//  The device is not connected.
//
#define ERROR_DEVICE_NOT_CONNECTED       1167L

//
// MessageId: ERROR_NOT_FOUND
//
// MessageText:
//
//  Element not found.
//
#define ERROR_NOT_FOUND                  1168L

//
// MessageId: ERROR_NO_MATCH
//
// MessageText:
//
//  There was no match for the specified key in the index.
//
#define ERROR_NO_MATCH                   1169L

//
// MessageId: ERROR_SET_NOT_FOUND
//
// MessageText:
//
//  The property set specified does not exist on the object.
//
#define ERROR_SET_NOT_FOUND              1170L

//
// MessageId: ERROR_POINT_NOT_FOUND
//
// MessageText:
//
//  The point passed to GetMouseMovePoints is not in the buffer.
//
#define ERROR_POINT_NOT_FOUND            1171L

//
// MessageId: ERROR_NO_TRACKING_SERVICE
//
// MessageText:
//
//  The tracking (workstation) service is not running.
//
#define ERROR_NO_TRACKING_SERVICE        1172L

//
// MessageId: ERROR_NO_VOLUME_ID
//
// MessageText:
//
//  The Volume ID could not be found.
//
#define ERROR_NO_VOLUME_ID               1173L

//
// MessageId: ERROR_UNABLE_TO_REMOVE_REPLACED
//
// MessageText:
//
//  Unable to remove the file to be replaced.
//
#define ERROR_UNABLE_TO_REMOVE_REPLACED  1175L

//
// MessageId: ERROR_UNABLE_TO_MOVE_REPLACEMENT
//
// MessageText:
//
//  Unable to move the replacement file to the file to be replaced. The file to be replaced has retained its original name.
//
#define ERROR_UNABLE_TO_MOVE_REPLACEMENT 1176L

//
// MessageId: ERROR_UNABLE_TO_MOVE_REPLACEMENT_2
//
// MessageText:
//
//  Unable to move the replacement file to the file to be replaced. The file to be replaced has been renamed using the backup name.
//
#define ERROR_UNABLE_TO_MOVE_REPLACEMENT_2 1177L

//
// MessageId: ERROR_JOURNAL_DELETE_IN_PROGRESS
//
// MessageText:
//
//  The volume change journal is being deleted.
//
#define ERROR_JOURNAL_DELETE_IN_PROGRESS 1178L

//
// MessageId: ERROR_JOURNAL_NOT_ACTIVE
//
// MessageText:
//
//  The volume change journal is not active.
//
#define ERROR_JOURNAL_NOT_ACTIVE         1179L

//
// MessageId: ERROR_POTENTIAL_FILE_FOUND
//
// MessageText:
//
//  A file was found, but it may not be the correct file.
//
#define ERROR_POTENTIAL_FILE_FOUND       1180L

//
// MessageId: ERROR_JOURNAL_ENTRY_DELETED
//
// MessageText:
//
//  The journal entry has been deleted from the journal.
//
#define ERROR_JOURNAL_ENTRY_DELETED      1181L

//
// MessageId: ERROR_BAD_DEVICE
//
// MessageText:
//
//  The specified device name is invalid.
//
#define ERROR_BAD_DEVICE                 1200L

//
// MessageId: ERROR_CONNECTION_UNAVAIL
//
// MessageText:
//
//  The device is not currently connected but it is a remembered connection.
//
#define ERROR_CONNECTION_UNAVAIL         1201L

//
// MessageId: ERROR_DEVICE_ALREADY_REMEMBERED
//
// MessageText:
//
//  The local device name has a remembered connection to another network resource.
//
#define ERROR_DEVICE_ALREADY_REMEMBERED  1202L

//
// MessageId: ERROR_NO_NET_OR_BAD_PATH
//
// MessageText:
//
//  No network provider accepted the given network path.
//
#define ERROR_NO_NET_OR_BAD_PATH         1203L

//
// MessageId: ERROR_BAD_PROVIDER
//
// MessageText:
//
//  The specified network provider name is invalid.
//
#define ERROR_BAD_PROVIDER               1204L

//
// MessageId: ERROR_CANNOT_OPEN_PROFILE
//
// MessageText:
//
//  Unable to open the network connection profile.
//
#define ERROR_CANNOT_OPEN_PROFILE        1205L

//
// MessageId: ERROR_BAD_PROFILE
//
// MessageText:
//
//  The network connection profile is corrupted.
//
#define ERROR_BAD_PROFILE                1206L

//
// MessageId: ERROR_NOT_CONTAINER
//
// MessageText:
//
//  Cannot enumerate a noncontainer.
//
#define ERROR_NOT_CONTAINER              1207L

//
// MessageId: ERROR_EXTENDED_ERROR
//
// MessageText:
//
//  An extended error has occurred.
//
#define ERROR_EXTENDED_ERROR             1208L

//
// MessageId: ERROR_INVALID_GROUPNAME
//
// MessageText:
//
//  The format of the specified group name is invalid.
//
#define ERROR_INVALID_GROUPNAME          1209L

//
// MessageId: ERROR_INVALID_COMPUTERNAME
//
// MessageText:
//
//  The format of the specified computer name is invalid.
//
#define ERROR_INVALID_COMPUTERNAME       1210L

//
// MessageId: ERROR_INVALID_EVENTNAME
//
// MessageText:
//
//  The format of the specified event name is invalid.
//
#define ERROR_INVALID_EVENTNAME          1211L

//
// MessageId: ERROR_INVALID_DOMAINNAME
//
// MessageText:
//
//  The format of the specified domain name is invalid.
//
#define ERROR_INVALID_DOMAINNAME         1212L

//
// MessageId: ERROR_INVALID_SERVICENAME
//
// MessageText:
//
//  The format of the specified service name is invalid.
//
#define ERROR_INVALID_SERVICENAME        1213L

//
// MessageId: ERROR_INVALID_NETNAME
//
// MessageText:
//
//  The format of the specified network name is invalid.
//
#define ERROR_INVALID_NETNAME            1214L

//
// MessageId: ERROR_INVALID_SHARENAME
//
// MessageText:
//
//  The format of the specified share name is invalid.
//
#define ERROR_INVALID_SHARENAME          1215L

//
// MessageId: ERROR_INVALID_PASSWORDNAME
//
// MessageText:
//
//  The format of the specified password is invalid.
//
#define ERROR_INVALID_PASSWORDNAME       1216L

//
// MessageId: ERROR_INVALID_MESSAGENAME
//
// MessageText:
//
//  The format of the specified message name is invalid.
//
#define ERROR_INVALID_MESSAGENAME        1217L

//
// MessageId: ERROR_INVALID_MESSAGEDEST
//
// MessageText:
//
//  The format of the specified message destination is invalid.
//
#define ERROR_INVALID_MESSAGEDEST        1218L

//
// MessageId: ERROR_SESSION_CREDENTIAL_CONFLICT
//
// MessageText:
//
//  Multiple connections to a server or shared resource by the same user, using more than one user name, are not allowed. Disconnect all previous connections to the server or shared resource and try again.
//
#define ERROR_SESSION_CREDENTIAL_CONFLICT 1219L

//
// MessageId: ERROR_REMOTE_SESSION_LIMIT_EXCEEDED
//
// MessageText:
//
//  An attempt was made to establish a session to a network server, but there are already too many sessions established to that server.
//
#define ERROR_REMOTE_SESSION_LIMIT_EXCEEDED 1220L

//
// MessageId: ERROR_DUP_DOMAINNAME
//
// MessageText:
//
//  The workgroup or domain name is already in use by another computer on the network.
//
#define ERROR_DUP_DOMAINNAME             1221L

//
// MessageId: ERROR_NO_NETWORK
//
// MessageText:
//
//  The network is not present or not started.
//
#define ERROR_NO_NETWORK                 1222L

//
// MessageId: ERROR_CANCELLED
//
// MessageText:
//
//  The operation was canceled by the user.
//
#define ERROR_CANCELLED                  1223L

//
// MessageId: ERROR_USER_MAPPED_FILE
//
// MessageText:
//
//  The requested operation cannot be performed on a file with a user-mapped section open.
//
#define ERROR_USER_MAPPED_FILE           1224L

//
// MessageId: ERROR_CONNECTION_REFUSED
//
// MessageText:
//
//  The remote system refused the network connection.
//
#define ERROR_CONNECTION_REFUSED         1225L

//
// MessageId: ERROR_GRACEFUL_DISCONNECT
//
// MessageText:
//
//  The network connection was gracefully closed.
//
#define ERROR_GRACEFUL_DISCONNECT        1226L

//
// MessageId: ERROR_ADDRESS_ALREADY_ASSOCIATED
//
// MessageText:
//
//  The network transport endpoint already has an address associated with it.
//
#define ERROR_ADDRESS_ALREADY_ASSOCIATED 1227L

//
// MessageId: ERROR_ADDRESS_NOT_ASSOCIATED
//
// MessageText:
//
//  An address has not yet been associated with the network endpoint.
//
#define ERROR_ADDRESS_NOT_ASSOCIATED     1228L

//
// MessageId: ERROR_CONNECTION_INVALID
//
// MessageText:
//
//  An operation was attempted on a nonexistent network connection.
//
#define ERROR_CONNECTION_INVALID         1229L

//
// MessageId: ERROR_CONNECTION_ACTIVE
//
// MessageText:
//
//  An invalid operation was attempted on an active network connection.
//
#define ERROR_CONNECTION_ACTIVE          1230L

//
// MessageId: ERROR_NETWORK_UNREACHABLE
//
// MessageText:
//
//  The network location cannot be reached. For information about network troubleshooting, see Windows Help.
//
#define ERROR_NETWORK_UNREACHABLE        1231L

//
// MessageId: ERROR_HOST_UNREACHABLE
//
// MessageText:
//
//  The network location cannot be reached. For information about network troubleshooting, see Windows Help.
//
#define ERROR_HOST_UNREACHABLE           1232L

//
// MessageId: ERROR_PROTOCOL_UNREACHABLE
//
// MessageText:
//
//  The network location cannot be reached. For information about network troubleshooting, see Windows Help.
//
#define ERROR_PROTOCOL_UNREACHABLE       1233L

//
// MessageId: ERROR_PORT_UNREACHABLE
//
// MessageText:
//
//  No service is operating at the destination network endpoint on the remote system.
//
#define ERROR_PORT_UNREACHABLE           1234L

//
// MessageId: ERROR_REQUEST_ABORTED
//
// MessageText:
//
//  The request was aborted.
//
#define ERROR_REQUEST_ABORTED            1235L

//
// MessageId: ERROR_CONNECTION_ABORTED
//
// MessageText:
//
//  The network connection was aborted by the local system.
//
#define ERROR_CONNECTION_ABORTED         1236L

//
// MessageId: ERROR_RETRY
//
// MessageText:
//
//  The operation could not be completed. A retry should be performed.
//
#define ERROR_RETRY                      1237L

//
// MessageId: ERROR_CONNECTION_COUNT_LIMIT
//
// MessageText:
//
//  A connection to the server could not be made because the limit on the number of concurrent connections for this account has been reached.
//
#define ERROR_CONNECTION_COUNT_LIMIT     1238L

//
// MessageId: ERROR_LOGIN_TIME_RESTRICTION
//
// MessageText:
//
//  Attempting to log in during an unauthorized time of day for this account.
//
#define ERROR_LOGIN_TIME_RESTRICTION     1239L

//
// MessageId: ERROR_LOGIN_WKSTA_RESTRICTION
//
// MessageText:
//
//  The account is not authorized to log in from this station.
//
#define ERROR_LOGIN_WKSTA_RESTRICTION    1240L

//
// MessageId: ERROR_INCORRECT_ADDRESS
//
// MessageText:
//
//  The network address could not be used for the operation requested.
//
#define ERROR_INCORRECT_ADDRESS          1241L

//
// MessageId: ERROR_ALREADY_REGISTERED
//
// MessageText:
//
//  The service is already registered.
//
#define ERROR_ALREADY_REGISTERED         1242L

//
// MessageId: ERROR_SERVICE_NOT_FOUND
//
// MessageText:
//
//  The specified service does not exist.
//
#define ERROR_SERVICE_NOT_FOUND          1243L

//
// MessageId: ERROR_NOT_AUTHENTICATED
//
// MessageText:
//
//  The operation being requested was not performed because the user has not been authenticated.
//
#define ERROR_NOT_AUTHENTICATED          1244L

//
// MessageId: ERROR_NOT_LOGGED_ON
//
// MessageText:
//
//  The operation being requested was not performed because the user has not logged on to the network.
//  The specified service does not exist.
//
#define ERROR_NOT_LOGGED_ON              1245L

//
// MessageId: ERROR_CONTINUE
//
// MessageText:
//
//  Continue with work in progress.
//
#define ERROR_CONTINUE                   1246L    // dderror

//
// MessageId: ERROR_ALREADY_INITIALIZED
//
// MessageText:
//
//  An attempt was made to perform an initialization operation when initialization has already been completed.
//
#define ERROR_ALREADY_INITIALIZED        1247L

//
// MessageId: ERROR_NO_MORE_DEVICES
//
// MessageText:
//
//  No more local devices.
//
#define ERROR_NO_MORE_DEVICES            1248L    // dderror

//
// MessageId: ERROR_NO_SUCH_SITE
//
// MessageText:
//
//  The specified site does not exist.
//
#define ERROR_NO_SUCH_SITE               1249L

//
// MessageId: ERROR_DOMAIN_CONTROLLER_EXISTS
//
// MessageText:
//
//  A domain controller with the specified name already exists.
//
#define ERROR_DOMAIN_CONTROLLER_EXISTS   1250L

//
// MessageId: ERROR_ONLY_IF_CONNECTED
//
// MessageText:
//
//  This operation is supported only when you are connected to the server.
//
#define ERROR_ONLY_IF_CONNECTED          1251L

//
// MessageId: ERROR_OVERRIDE_NOCHANGES
//
// MessageText:
//
//  The group policy framework should call the extension even if there are no changes.
//
#define ERROR_OVERRIDE_NOCHANGES         1252L

//
// MessageId: ERROR_BAD_USER_PROFILE
//
// MessageText:
//
//  The specified user does not have a valid profile.
//
#define ERROR_BAD_USER_PROFILE           1253L

//
// MessageId: ERROR_NOT_SUPPORTED_ON_SBS
//
// MessageText:
//
//  This operation is not supported on a computer running Windows Server 2003 for Small Business Server
//
#define ERROR_NOT_SUPPORTED_ON_SBS       1254L

//
// MessageId: ERROR_SERVER_SHUTDOWN_IN_PROGRESS
//
// MessageText:
//
//  The server machine is shutting down.
//
#define ERROR_SERVER_SHUTDOWN_IN_PROGRESS 1255L

//
// MessageId: ERROR_HOST_DOWN
//
// MessageText:
//
//  The remote system is not available. For information about network troubleshooting, see Windows Help.
//
#define ERROR_HOST_DOWN                  1256L

//
// MessageId: ERROR_NON_ACCOUNT_SID
//
// MessageText:
//
//  The security identifier provided is not from an account domain.
//
#define ERROR_NON_ACCOUNT_SID            1257L

//
// MessageId: ERROR_NON_DOMAIN_SID
//
// MessageText:
//
//  The security identifier provided does not have a domain component.
//
#define ERROR_NON_DOMAIN_SID             1258L

//
// MessageId: ERROR_APPHELP_BLOCK
//
// MessageText:
//
//  AppHelp dialog canceled thus preventing the application from starting.
//
#define ERROR_APPHELP_BLOCK              1259L

//
// MessageId: ERROR_ACCESS_DISABLED_BY_POLICY
//
// MessageText:
//
//  Windows cannot open this program because it has been prevented by a software restriction policy. For more information, open Event Viewer or contact your system administrator.
//
#define ERROR_ACCESS_DISABLED_BY_POLICY  1260L

//
// MessageId: ERROR_REG_NAT_CONSUMPTION
//
// MessageText:
//
//  A program attempt to use an invalid register value.  Normally caused by an uninitialized register. This error is Itanium specific.
//
#define ERROR_REG_NAT_CONSUMPTION        1261L

//
// MessageId: ERROR_CSCSHARE_OFFLINE
//
// MessageText:
//
//  The share is currently offline or does not exist.
//
#define ERROR_CSCSHARE_OFFLINE           1262L

//
// MessageId: ERROR_PKINIT_FAILURE
//
// MessageText:
//
//  The kerberos protocol encountered an error while validating the
//  KDC certificate during smartcard logon.  There is more information in the
//  system event log.
//
#define ERROR_PKINIT_FAILURE             1263L

//
// MessageId: ERROR_SMARTCARD_SUBSYSTEM_FAILURE
//
// MessageText:
//
//  The kerberos protocol encountered an error while attempting to utilize
//  the smartcard subsystem.
//
#define ERROR_SMARTCARD_SUBSYSTEM_FAILURE 1264L

//
// MessageId: ERROR_DOWNGRADE_DETECTED
//
// MessageText:
//
//  The system detected a possible attempt to compromise security. Please ensure that you can contact the server that authenticated you.
//
#define ERROR_DOWNGRADE_DETECTED         1265L

//
// Do not use ID's 1266 - 1270 as the symbolicNames have been moved to SEC_E_*
//
//
// MessageId: ERROR_MACHINE_LOCKED
//
// MessageText:
//
//  The machine is locked and can not be shut down without the force option.
//
#define ERROR_MACHINE_LOCKED             1271L

//
// MessageId: ERROR_CALLBACK_SUPPLIED_INVALID_DATA
//
// MessageText:
//
//  An application-defined callback gave invalid data when called.
//
#define ERROR_CALLBACK_SUPPLIED_INVALID_DATA 1273L

//
// MessageId: ERROR_SYNC_FOREGROUND_REFRESH_REQUIRED
//
// MessageText:
//
//  The group policy framework should call the extension in the synchronous foreground policy refresh.
//
#define ERROR_SYNC_FOREGROUND_REFRESH_REQUIRED 1274L

//
// MessageId: ERROR_DRIVER_BLOCKED
//
// MessageText:
//
//  This driver has been blocked from loading
//
#define ERROR_DRIVER_BLOCKED             1275L

//
// MessageId: ERROR_INVALID_IMPORT_OF_NON_DLL
//
// MessageText:
//
//  A dynamic link library (DLL) referenced a module that was neither a DLL nor the process's executable image.
//
#define ERROR_INVALID_IMPORT_OF_NON_DLL  1276L

//
// MessageId: ERROR_ACCESS_DISABLED_WEBBLADE
//
// MessageText:
//
//  Windows cannot open this program since it has been disabled.
//
#define ERROR_ACCESS_DISABLED_WEBBLADE   1277L

//
// MessageId: ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER
//
// MessageText:
//
//  Windows cannot open this program because the license enforcement system has been tampered with or become corrupted.
//
#define ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER 1278L

//
// MessageId: ERROR_RECOVERY_FAILURE
//
// MessageText:
//
//  A transaction recover failed.
//
#define ERROR_RECOVERY_FAILURE           1279L

//
// MessageId: ERROR_ALREADY_FIBER
//
// MessageText:
//
//  The current thread has already been converted to a fiber.
//
#define ERROR_ALREADY_FIBER              1280L

//
// MessageId: ERROR_ALREADY_THREAD
//
// MessageText:
//
//  The current thread has already been converted from a fiber.
//
#define ERROR_ALREADY_THREAD             1281L

//
// MessageId: ERROR_STACK_BUFFER_OVERRUN
//
// MessageText:
//
//  The system detected an overrun of a stack-based buffer in this application.  This
//  overrun could potentially allow a malicious user to gain control of this application.
//
#define ERROR_STACK_BUFFER_OVERRUN       1282L

//
// MessageId: ERROR_PARAMETER_QUOTA_EXCEEDED
//
// MessageText:
//
//  Data present in one of the parameters is more than the function can operate on.
//
#define ERROR_PARAMETER_QUOTA_EXCEEDED   1283L

//
// MessageId: ERROR_DEBUGGER_INACTIVE
//
// MessageText:
//
//  An attempt to do an operation on a debug object failed because the object is in the process of being deleted.
//
#define ERROR_DEBUGGER_INACTIVE          1284L

//
// MessageId: ERROR_DELAY_LOAD_FAILED
//
// MessageText:
//
//  An attempt to delay-load a .dll or get a function address in a delay-loaded .dll failed.
//
#define ERROR_DELAY_LOAD_FAILED          1285L

//
// MessageId: ERROR_VDM_DISALLOWED
//
// MessageText:
//
//  %1 is a 16-bit application. You do not have permissions to execute 16-bit applications. Check your permissions with your system administrator.
//
#define ERROR_VDM_DISALLOWED             1286L

//
// MessageId: ERROR_UNIDENTIFIED_ERROR
//
// MessageText:
//
//  Insufficient information exists to identify the cause of failure.
//
#define ERROR_UNIDENTIFIED_ERROR         1287L


///////////////////////////
//
// Add new status codes before this point unless there is a component specific section below.
//
///////////////////////////


///////////////////////////
//                       //
// Security Status Codes //
//                       //
///////////////////////////


//
// MessageId: ERROR_NOT_ALL_ASSIGNED
//
// MessageText:
//
//  Not all privileges referenced are assigned to the caller.
//
#define ERROR_NOT_ALL_ASSIGNED           1300L

//
// MessageId: ERROR_SOME_NOT_MAPPED
//
// MessageText:
//
//  Some mapping between account names and security IDs was not done.
//
#define ERROR_SOME_NOT_MAPPED            1301L

//
// MessageId: ERROR_NO_QUOTAS_FOR_ACCOUNT
//
// MessageText:
//
//  No system quota limits are specifically set for this account.
//
#define ERROR_NO_QUOTAS_FOR_ACCOUNT      1302L

//
// MessageId: ERROR_LOCAL_USER_SESSION_KEY
//
// MessageText:
//
//  No encryption key is available. A well-known encryption key was returned.
//
#define ERROR_LOCAL_USER_SESSION_KEY     1303L

//
// MessageId: ERROR_NULL_LM_PASSWORD
//
// MessageText:
//
//  The password is too complex to be converted to a LAN Manager password. The LAN Manager password returned is a NULL string.
//
#define ERROR_NULL_LM_PASSWORD           1304L

//
// MessageId: ERROR_UNKNOWN_REVISION
//
// MessageText:
//
//  The revision level is unknown.
//
#define ERROR_UNKNOWN_REVISION           1305L

//
// MessageId: ERROR_REVISION_MISMATCH
//
// MessageText:
//
//  Indicates two revision levels are incompatible.
//
#define ERROR_REVISION_MISMATCH          1306L

//
// MessageId: ERROR_INVALID_OWNER
//
// MessageText:
//
//  This security ID may not be assigned as the owner of this object.
//
#define ERROR_INVALID_OWNER              1307L

//
// MessageId: ERROR_INVALID_PRIMARY_GROUP
//
// MessageText:
//
//  This security ID may not be assigned as the primary group of an object.
//
#define ERROR_INVALID_PRIMARY_GROUP      1308L

//
// MessageId: ERROR_NO_IMPERSONATION_TOKEN
//
// MessageText:
//
//  An attempt has been made to operate on an impersonation token by a thread that is not currently impersonating a client.
//
#define ERROR_NO_IMPERSONATION_TOKEN     1309L

//
// MessageId: ERROR_CANT_DISABLE_MANDATORY
//
// MessageText:
//
//  The group may not be disabled.
//
#define ERROR_CANT_DISABLE_MANDATORY     1310L

//
// MessageId: ERROR_NO_LOGON_SERVERS
//
// MessageText:
//
//  There are currently no logon servers available to service the logon request.
//
#define ERROR_NO_LOGON_SERVERS           1311L

//
// MessageId: ERROR_NO_SUCH_LOGON_SESSION
//
// MessageText:
//
//  A specified logon session does not exist. It may already have been terminated.
//
#define ERROR_NO_SUCH_LOGON_SESSION      1312L

//
// MessageId: ERROR_NO_SUCH_PRIVILEGE
//
// MessageText:
//
//  A specified privilege does not exist.
//
#define ERROR_NO_SUCH_PRIVILEGE          1313L

//
// MessageId: ERROR_PRIVILEGE_NOT_HELD
//
// MessageText:
//
//  A required privilege is not held by the client.
//
#define ERROR_PRIVILEGE_NOT_HELD         1314L

//
// MessageId: ERROR_INVALID_ACCOUNT_NAME
//
// MessageText:
//
//  The name provided is not a properly formed account name.
//
#define ERROR_INVALID_ACCOUNT_NAME       1315L

//
// MessageId: ERROR_USER_EXISTS
//
// MessageText:
//
//  The specified user already exists.
//
#define ERROR_USER_EXISTS                1316L

//
// MessageId: ERROR_NO_SUCH_USER
//
// MessageText:
//
//  The specified user does not exist.
//
#define ERROR_NO_SUCH_USER               1317L

//
// MessageId: ERROR_GROUP_EXISTS
//
// MessageText:
//
//  The specified group already exists.
//
#define ERROR_GROUP_EXISTS               1318L

//
// MessageId: ERROR_NO_SUCH_GROUP
//
// MessageText:
//
//  The specified group does not exist.
//
#define ERROR_NO_SUCH_GROUP              1319L

//
// MessageId: ERROR_MEMBER_IN_GROUP
//
// MessageText:
//
//  Either the specified user account is already a member of the specified group, or the specified group cannot be deleted because it contains a member.
//
#define ERROR_MEMBER_IN_GROUP            1320L

//
// MessageId: ERROR_MEMBER_NOT_IN_GROUP
//
// MessageText:
//
//  The specified user account is not a member of the specified group account.
//
#define ERROR_MEMBER_NOT_IN_GROUP        1321L

//
// MessageId: ERROR_LAST_ADMIN
//
// MessageText:
//
//  The last remaining administration account cannot be disabled or deleted.
//
#define ERROR_LAST_ADMIN                 1322L

//
// MessageId: ERROR_WRONG_PASSWORD
//
// MessageText:
//
//  Unable to update the password. The value provided as the current password is incorrect.
//
#define ERROR_WRONG_PASSWORD             1323L

//
// MessageId: ERROR_ILL_FORMED_PASSWORD
//
// MessageText:
//
//  Unable to update the password. The value provided for the new password contains values that are not allowed in passwords.
//
#define ERROR_ILL_FORMED_PASSWORD        1324L

//
// MessageId: ERROR_PASSWORD_RESTRICTION
//
// MessageText:
//
//  Unable to update the password. The value provided for the new password does not meet the length, complexity, or history requirement of the domain.
//
#define ERROR_PASSWORD_RESTRICTION       1325L

//
// MessageId: ERROR_LOGON_FAILURE
//
// MessageText:
//
//  Logon failure: unknown user name or bad password.
//
#define ERROR_LOGON_FAILURE              1326L

//
// MessageId: ERROR_ACCOUNT_RESTRICTION
//
// MessageText:
//
//  Logon failure: user account restriction.  Possible reasons are blank passwords not allowed, logon hour restrictions, or a policy restriction has been enforced.
//
#define ERROR_ACCOUNT_RESTRICTION        1327L

//
// MessageId: ERROR_INVALID_LOGON_HOURS
//
// MessageText:
//
//  Logon failure: account logon time restriction violation.
//
#define ERROR_INVALID_LOGON_HOURS        1328L

//
// MessageId: ERROR_INVALID_WORKSTATION
//
// MessageText:
//
//  Logon failure: user not allowed to log on to this computer.
//
#define ERROR_INVALID_WORKSTATION        1329L

//
// MessageId: ERROR_PASSWORD_EXPIRED
//
// MessageText:
//
//  Logon failure: the specified account password has expired.
//
#define ERROR_PASSWORD_EXPIRED           1330L

//
// MessageId: ERROR_ACCOUNT_DISABLED
//
// MessageText:
//
//  Logon failure: account currently disabled.
//
#define ERROR_ACCOUNT_DISABLED           1331L

//
// MessageId: ERROR_NONE_MAPPED
//
// MessageText:
//
//  No mapping between account names and security IDs was done.
//
#define ERROR_NONE_MAPPED                1332L

//
// MessageId: ERROR_TOO_MANY_LUIDS_REQUESTED
//
// MessageText:
//
//  Too many local user identifiers (LUIDs) were requested at one time.
//
#define ERROR_TOO_MANY_LUIDS_REQUESTED   1333L

//
// MessageId: ERROR_LUIDS_EXHAUSTED
//
// MessageText:
//
//  No more local user identifiers (LUIDs) are available.
//
#define ERROR_LUIDS_EXHAUSTED            1334L

//
// MessageId: ERROR_INVALID_SUB_AUTHORITY
//
// MessageText:
//
//  The subauthority part of a security ID is invalid for this particular use.
//
#define ERROR_INVALID_SUB_AUTHORITY      1335L

//
// MessageId: ERROR_INVALID_ACL
//
// MessageText:
//
//  The access control list (ACL) structure is invalid.
//
#define ERROR_INVALID_ACL                1336L

//
// MessageId: ERROR_INVALID_SID
//
// MessageText:
//
//  The security ID structure is invalid.
//
#define ERROR_INVALID_SID                1337L

//
// MessageId: ERROR_INVALID_SECURITY_DESCR
//
// MessageText:
//
//  The security descriptor structure is invalid.
//
#define ERROR_INVALID_SECURITY_DESCR     1338L

//
// MessageId: ERROR_BAD_INHERITANCE_ACL
//
// MessageText:
//
//  The inherited access control list (ACL) or access control entry (ACE) could not be built.
//
#define ERROR_BAD_INHERITANCE_ACL        1340L

//
// MessageId: ERROR_SERVER_DISABLED
//
// MessageText:
//
//  The server is currently disabled.
//
#define ERROR_SERVER_DISABLED            1341L

//
// MessageId: ERROR_SERVER_NOT_DISABLED
//
// MessageText:
//
//  The server is currently enabled.
//
#define ERROR_SERVER_NOT_DISABLED        1342L

//
// MessageId: ERROR_INVALID_ID_AUTHORITY
//
// MessageText:
//
//  The value provided was an invalid value for an identifier authority.
//
#define ERROR_INVALID_ID_AUTHORITY       1343L

//
// MessageId: ERROR_ALLOTTED_SPACE_EXCEEDED
//
// MessageText:
//
//  No more memory is available for security information updates.
//
#define ERROR_ALLOTTED_SPACE_EXCEEDED    1344L

//
// MessageId: ERROR_INVALID_GROUP_ATTRIBUTES
//
// MessageText:
//
//  The specified attributes are invalid, or incompatible with the attributes for the group as a whole.
//
#define ERROR_INVALID_GROUP_ATTRIBUTES   1345L

//
// MessageId: ERROR_BAD_IMPERSONATION_LEVEL
//
// MessageText:
//
//  Either a required impersonation level was not provided, or the provided impersonation level is invalid.
//
#define ERROR_BAD_IMPERSONATION_LEVEL    1346L

//
// MessageId: ERROR_CANT_OPEN_ANONYMOUS
//
// MessageText:
//
//  Cannot open an anonymous level security token.
//
#define ERROR_CANT_OPEN_ANONYMOUS        1347L

//
// MessageId: ERROR_BAD_VALIDATION_CLASS
//
// MessageText:
//
//  The validation information class requested was invalid.
//
#define ERROR_BAD_VALIDATION_CLASS       1348L

//
// MessageId: ERROR_BAD_TOKEN_TYPE
//
// MessageText:
//
//  The type of the token is inappropriate for its attempted use.
//
#define ERROR_BAD_TOKEN_TYPE             1349L

//
// MessageId: ERROR_NO_SECURITY_ON_OBJECT
//
// MessageText:
//
//  Unable to perform a security operation on an object that has no associated security.
//
#define ERROR_NO_SECURITY_ON_OBJECT      1350L

//
// MessageId: ERROR_CANT_ACCESS_DOMAIN_INFO
//
// MessageText:
//
//  Configuration information could not be read from the domain controller, either because the machine is unavailable, or access has been denied.
//
#define ERROR_CANT_ACCESS_DOMAIN_INFO    1351L

//
// MessageId: ERROR_INVALID_SERVER_STATE
//
// MessageText:
//
//  The security account manager (SAM) or local security authority (LSA) server was in the wrong state to perform the security operation.
//
#define ERROR_INVALID_SERVER_STATE       1352L

//
// MessageId: ERROR_INVALID_DOMAIN_STATE
//
// MessageText:
//
//  The domain was in the wrong state to perform the security operation.
//
#define ERROR_INVALID_DOMAIN_STATE       1353L

//
// MessageId: ERROR_INVALID_DOMAIN_ROLE
//
// MessageText:
//
//  This operation is only allowed for the Primary Domain Controller of the domain.
//
#define ERROR_INVALID_DOMAIN_ROLE        1354L

//
// MessageId: ERROR_NO_SUCH_DOMAIN
//
// MessageText:
//
//  The specified domain either does not exist or could not be contacted.
//
#define ERROR_NO_SUCH_DOMAIN             1355L

//
// MessageId: ERROR_DOMAIN_EXISTS
//
// MessageText:
//
//  The specified domain already exists.
//
#define ERROR_DOMAIN_EXISTS              1356L

//
// MessageId: ERROR_DOMAIN_LIMIT_EXCEEDED
//
// MessageText:
//
//  An attempt was made to exceed the limit on the number of domains per server.
//
#define ERROR_DOMAIN_LIMIT_EXCEEDED      1357L

//
// MessageId: ERROR_INTERNAL_DB_CORRUPTION
//
// MessageText:
//
//  Unable to complete the requested operation because of either a catastrophic media failure or a data structure corruption on the disk.
//
#define ERROR_INTERNAL_DB_CORRUPTION     1358L

//
// MessageId: ERROR_INTERNAL_ERROR
//
// MessageText:
//
//  An internal error occurred.
//
#define ERROR_INTERNAL_ERROR             1359L

//
// MessageId: ERROR_GENERIC_NOT_MAPPED
//
// MessageText:
//
//  Generic access types were contained in an access mask which should already be mapped to nongeneric types.
//
#define ERROR_GENERIC_NOT_MAPPED         1360L

//
// MessageId: ERROR_BAD_DESCRIPTOR_FORMAT
//
// MessageText:
//
//  A security descriptor is not in the right format (absolute or self-relative).
//
#define ERROR_BAD_DESCRIPTOR_FORMAT      1361L

//
// MessageId: ERROR_NOT_LOGON_PROCESS
//
// MessageText:
//
//  The requested action is restricted for use by logon processes only. The calling process has not registered as a logon process.
//
#define ERROR_NOT_LOGON_PROCESS          1362L

//
// MessageId: ERROR_LOGON_SESSION_EXISTS
//
// MessageText:
//
//  Cannot start a new logon session with an ID that is already in use.
//
#define ERROR_LOGON_SESSION_EXISTS       1363L

//
// MessageId: ERROR_NO_SUCH_PACKAGE
//
// MessageText:
//
//  A specified authentication package is unknown.
//
#define ERROR_NO_SUCH_PACKAGE            1364L

//
// MessageId: ERROR_BAD_LOGON_SESSION_STATE
//
// MessageText:
//
//  The logon session is not in a state that is consistent with the requested operation.
//
#define ERROR_BAD_LOGON_SESSION_STATE    1365L

//
// MessageId: ERROR_LOGON_SESSION_COLLISION
//
// MessageText:
//
//  The logon session ID is already in use.
//
#define ERROR_LOGON_SESSION_COLLISION    1366L

//
// MessageId: ERROR_INVALID_LOGON_TYPE
//
// MessageText:
//
//  A logon request contained an invalid logon type value.
//
#define ERROR_INVALID_LOGON_TYPE         1367L

//
// MessageId: ERROR_CANNOT_IMPERSONATE
//
// MessageText:
//
//  Unable to impersonate using a named pipe until data has been read from that pipe.
//
#define ERROR_CANNOT_IMPERSONATE         1368L

//
// MessageId: ERROR_RXACT_INVALID_STATE
//
// MessageText:
//
//  The transaction state of a registry subtree is incompatible with the requested operation.
//
#define ERROR_RXACT_INVALID_STATE        1369L

//
// MessageId: ERROR_RXACT_COMMIT_FAILURE
//
// MessageText:
//
//  An internal security database corruption has been encountered.
//
#define ERROR_RXACT_COMMIT_FAILURE       1370L

//
// MessageId: ERROR_SPECIAL_ACCOUNT
//
// MessageText:
//
//  Cannot perform this operation on built-in accounts.
//
#define ERROR_SPECIAL_ACCOUNT            1371L

//
// MessageId: ERROR_SPECIAL_GROUP
//
// MessageText:
//
//  Cannot perform this operation on this built-in special group.
//
#define ERROR_SPECIAL_GROUP              1372L

//
// MessageId: ERROR_SPECIAL_USER
//
// MessageText:
//
//  Cannot perform this operation on this built-in special user.
//
#define ERROR_SPECIAL_USER               1373L

//
// MessageId: ERROR_MEMBERS_PRIMARY_GROUP
//
// MessageText:
//
//  The user cannot be removed from a group because the group is currently the user's primary group.
//
#define ERROR_MEMBERS_PRIMARY_GROUP      1374L

//
// MessageId: ERROR_TOKEN_ALREADY_IN_USE
//
// MessageText:
//
//  The token is already in use as a primary token.
//
#define ERROR_TOKEN_ALREADY_IN_USE       1375L

//
// MessageId: ERROR_NO_SUCH_ALIAS
//
// MessageText:
//
//  The specified local group does not exist.
//
#define ERROR_NO_SUCH_ALIAS              1376L

//
// MessageId: ERROR_MEMBER_NOT_IN_ALIAS
//
// MessageText:
//
//  The specified account name is not a member of the local group.
//
#define ERROR_MEMBER_NOT_IN_ALIAS        1377L

//
// MessageId: ERROR_MEMBER_IN_ALIAS
//
// MessageText:
//
//  The specified account name is already a member of the local group.
//
#define ERROR_MEMBER_IN_ALIAS            1378L

//
// MessageId: ERROR_ALIAS_EXISTS
//
// MessageText:
//
//  The specified local group already exists.
//
#define ERROR_ALIAS_EXISTS               1379L

//
// MessageId: ERROR_LOGON_NOT_GRANTED
//
// MessageText:
//
//  Logon failure: the user has not been granted the requested logon type at this computer.
//
#define ERROR_LOGON_NOT_GRANTED          1380L

//
// MessageId: ERROR_TOO_MANY_SECRETS
//
// MessageText:
//
//  The maximum number of secrets that may be stored in a single system has been exceeded.
//
#define ERROR_TOO_MANY_SECRETS           1381L

//
// MessageId: ERROR_SECRET_TOO_LONG
//
// MessageText:
//
//  The length of a secret exceeds the maximum length allowed.
//
#define ERROR_SECRET_TOO_LONG            1382L

//
// MessageId: ERROR_INTERNAL_DB_ERROR
//
// MessageText:
//
//  The local security authority database contains an internal inconsistency.
//
#define ERROR_INTERNAL_DB_ERROR          1383L

//
// MessageId: ERROR_TOO_MANY_CONTEXT_IDS
//
// MessageText:
//
//  During a logon attempt, the user's security context accumulated too many security IDs.
//
#define ERROR_TOO_MANY_CONTEXT_IDS       1384L

//
// MessageId: ERROR_LOGON_TYPE_NOT_GRANTED
//
// MessageText:
//
//  Logon failure: the user has not been granted the requested logon type at this computer.
//
#define ERROR_LOGON_TYPE_NOT_GRANTED     1385L

//
// MessageId: ERROR_NT_CROSS_ENCRYPTION_REQUIRED
//
// MessageText:
//
//  A cross-encrypted password is necessary to change a user password.
//
#define ERROR_NT_CROSS_ENCRYPTION_REQUIRED 1386L

//
// MessageId: ERROR_NO_SUCH_MEMBER
//
// MessageText:
//
//  A member could not be added to or removed from the local group because the member does not exist.
//
#define ERROR_NO_SUCH_MEMBER             1387L

//
// MessageId: ERROR_INVALID_MEMBER
//
// MessageText:
//
//  A new member could not be added to a local group because the member has the wrong account type.
//
#define ERROR_INVALID_MEMBER             1388L

//
// MessageId: ERROR_TOO_MANY_SIDS
//
// MessageText:
//
//  Too many security IDs have been specified.
//
#define ERROR_TOO_MANY_SIDS              1389L

//
// MessageId: ERROR_LM_CROSS_ENCRYPTION_REQUIRED
//
// MessageText:
//
//  A cross-encrypted password is necessary to change this user password.
//
#define ERROR_LM_CROSS_ENCRYPTION_REQUIRED 1390L

//
// MessageId: ERROR_NO_INHERITANCE
//
// MessageText:
//
//  Indicates an ACL contains no inheritable components.
//
#define ERROR_NO_INHERITANCE             1391L

//
// MessageId: ERROR_FILE_CORRUPT
//
// MessageText:
//
//  The file or directory is corrupted and unreadable.
//
#define ERROR_FILE_CORRUPT               1392L

//
// MessageId: ERROR_DISK_CORRUPT
//
// MessageText:
//
//  The disk structure is corrupted and unreadable.
//
#define ERROR_DISK_CORRUPT               1393L

//
// MessageId: ERROR_NO_USER_SESSION_KEY
//
// MessageText:
//
//  There is no user session key for the specified logon session.
//
#define ERROR_NO_USER_SESSION_KEY        1394L

//
// MessageId: ERROR_LICENSE_QUOTA_EXCEEDED
//
// MessageText:
//
//  The service being accessed is licensed for a particular number of connections.
//  No more connections can be made to the service at this time because there are already as many connections as the service can accept.
//
#define ERROR_LICENSE_QUOTA_EXCEEDED     1395L

//
// MessageId: ERROR_WRONG_TARGET_NAME
//
// MessageText:
//
//  Logon Failure: The target account name is incorrect.
//
#define ERROR_WRONG_TARGET_NAME          1396L

//
// MessageId: ERROR_MUTUAL_AUTH_FAILED
//
// MessageText:
//
//  Mutual Authentication failed. The server's password is out of date at the domain controller.
//
#define ERROR_MUTUAL_AUTH_FAILED         1397L

//
// MessageId: ERROR_TIME_SKEW
//
// MessageText:
//
//  There is a time and/or date difference between the client and server.
//
#define ERROR_TIME_SKEW                  1398L

//
// MessageId: ERROR_CURRENT_DOMAIN_NOT_ALLOWED
//
// MessageText:
//
//  This operation can not be performed on the current domain.
//
#define ERROR_CURRENT_DOMAIN_NOT_ALLOWED 1399L

// End of security error codes



///////////////////////////
//                       //
// WinUser Error Codes   //
//                       //
///////////////////////////


//
// MessageId: ERROR_INVALID_WINDOW_HANDLE
//
// MessageText:
//
//  Invalid window handle.
//
#define ERROR_INVALID_WINDOW_HANDLE      1400L

//
// MessageId: ERROR_INVALID_MENU_HANDLE
//
// MessageText:
//
//  Invalid menu handle.
//
#define ERROR_INVALID_MENU_HANDLE        1401L

//
// MessageId: ERROR_INVALID_CURSOR_HANDLE
//
// MessageText:
//
//  Invalid cursor handle.
//
#define ERROR_INVALID_CURSOR_HANDLE      1402L

//
// MessageId: ERROR_INVALID_ACCEL_HANDLE
//
// MessageText:
//
//  Invalid accelerator table handle.
//
#define ERROR_INVALID_ACCEL_HANDLE       1403L

//
// MessageId: ERROR_INVALID_HOOK_HANDLE
//
// MessageText:
//
//  Invalid hook handle.
//
#define ERROR_INVALID_HOOK_HANDLE        1404L

//
// MessageId: ERROR_INVALID_DWP_HANDLE
//
// MessageText:
//
//  Invalid handle to a multiple-window position structure.
//
#define ERROR_INVALID_DWP_HANDLE         1405L

//
// MessageId: ERROR_TLW_WITH_WSCHILD
//
// MessageText:
//
//  Cannot create a top-level child window.
//
#define ERROR_TLW_WITH_WSCHILD           1406L

//
// MessageId: ERROR_CANNOT_FIND_WND_CLASS
//
// MessageText:
//
//  Cannot find window class.
//
#define ERROR_CANNOT_FIND_WND_CLASS      1407L

//
// MessageId: ERROR_WINDOW_OF_OTHER_THREAD
//
// MessageText:
//
//  Invalid window; it belongs to other thread.
//
#define ERROR_WINDOW_OF_OTHER_THREAD     1408L

//
// MessageId: ERROR_HOTKEY_ALREADY_REGISTERED
//
// MessageText:
//
//  Hot key is already registered.
//
#define ERROR_HOTKEY_ALREADY_REGISTERED  1409L

//
// MessageId: ERROR_CLASS_ALREADY_EXISTS
//
// MessageText:
//
//  Class already exists.
//
#define ERROR_CLASS_ALREADY_EXISTS       1410L

//
// MessageId: ERROR_CLASS_DOES_NOT_EXIST
//
// MessageText:
//
//  Class does not exist.
//
#define ERROR_CLASS_DOES_NOT_EXIST       1411L

//
// MessageId: ERROR_CLASS_HAS_WINDOWS
//
// MessageText:
//
//  Class still has open windows.
//
#define ERROR_CLASS_HAS_WINDOWS          1412L

//
// MessageId: ERROR_INVALID_INDEX
//
// MessageText:
//
//  Invalid index.
//
#define ERROR_INVALID_INDEX              1413L

//
// MessageId: ERROR_INVALID_ICON_HANDLE
//
// MessageText:
//
//  Invalid icon handle.
//
#define ERROR_INVALID_ICON_HANDLE        1414L

//
// MessageId: ERROR_PRIVATE_DIALOG_INDEX
//
// MessageText:
//
//  Using private DIALOG window words.
//
#define ERROR_PRIVATE_DIALOG_INDEX       1415L

//
// MessageId: ERROR_LISTBOX_ID_NOT_FOUND
//
// MessageText:
//
//  The list box identifier was not found.
//
#define ERROR_LISTBOX_ID_NOT_FOUND       1416L

//
// MessageId: ERROR_NO_WILDCARD_CHARACTERS
//
// MessageText:
//
//  No wildcards were found.
//
#define ERROR_NO_WILDCARD_CHARACTERS     1417L

//
// MessageId: ERROR_CLIPBOARD_NOT_OPEN
//
// MessageText:
//
//  Thread does not have a clipboard open.
//
#define ERROR_CLIPBOARD_NOT_OPEN         1418L

//
// MessageId: ERROR_HOTKEY_NOT_REGISTERED
//
// MessageText:
//
//  Hot key is not registered.
//
#define ERROR_HOTKEY_NOT_REGISTERED      1419L

//
// MessageId: ERROR_WINDOW_NOT_DIALOG
//
// MessageText:
//
//  The window is not a valid dialog window.
//
#define ERROR_WINDOW_NOT_DIALOG          1420L

//
// MessageId: ERROR_CONTROL_ID_NOT_FOUND
//
// MessageText:
//
//  Control ID not found.
//
#define ERROR_CONTROL_ID_NOT_FOUND       1421L

//
// MessageId: ERROR_INVALID_COMBOBOX_MESSAGE
//
// MessageText:
//
//  Invalid message for a combo box because it does not have an edit control.
//
#define ERROR_INVALID_COMBOBOX_MESSAGE   1422L

//
// MessageId: ERROR_WINDOW_NOT_COMBOBOX
//
// MessageText:
//
//  The window is not a combo box.
//
#define ERROR_WINDOW_NOT_COMBOBOX        1423L

//
// MessageId: ERROR_INVALID_EDIT_HEIGHT
//
// MessageText:
//
//  Height must be less than 256.
//
#define ERROR_INVALID_EDIT_HEIGHT        1424L

//
// MessageId: ERROR_DC_NOT_FOUND
//
// MessageText:
//
//  Invalid device context (DC) handle.
//
#define ERROR_DC_NOT_FOUND               1425L

//
// MessageId: ERROR_INVALID_HOOK_FILTER
//
// MessageText:
//
//  Invalid hook procedure type.
//
#define ERROR_INVALID_HOOK_FILTER        1426L

//
// MessageId: ERROR_INVALID_FILTER_PROC
//
// MessageText:
//
//  Invalid hook procedure.
//
#define ERROR_INVALID_FILTER_PROC        1427L

//
// MessageId: ERROR_HOOK_NEEDS_HMOD
//
// MessageText:
//
//  Cannot set nonlocal hook without a module handle.
//
#define ERROR_HOOK_NEEDS_HMOD            1428L

//
// MessageId: ERROR_GLOBAL_ONLY_HOOK
//
// MessageText:
//
//  This hook procedure can only be set globally.
//
#define ERROR_GLOBAL_ONLY_HOOK           1429L

//
// MessageId: ERROR_JOURNAL_HOOK_SET
//
// MessageText:
//
//  The journal hook procedure is already installed.
//
#define ERROR_JOURNAL_HOOK_SET           1430L

//
// MessageId: ERROR_HOOK_NOT_INSTALLED
//
// MessageText:
//
//  The hook procedure is not installed.
//
#define ERROR_HOOK_NOT_INSTALLED         1431L

//
// MessageId: ERROR_INVALID_LB_MESSAGE
//
// MessageText:
//
//  Invalid message for single-selection list box.
//
#define ERROR_INVALID_LB_MESSAGE         1432L

//
// MessageId: ERROR_SETCOUNT_ON_BAD_LB
//
// MessageText:
//
//  LB_SETCOUNT sent to non-lazy list box.
//
#define ERROR_SETCOUNT_ON_BAD_LB         1433L

//
// MessageId: ERROR_LB_WITHOUT_TABSTOPS
//
// MessageText:
//
//  This list box does not support tab stops.
//
#define ERROR_LB_WITHOUT_TABSTOPS        1434L

//
// MessageId: ERROR_DESTROY_OBJECT_OF_OTHER_THREAD
//
// MessageText:
//
//  Cannot destroy object created by another thread.
//
#define ERROR_DESTROY_OBJECT_OF_OTHER_THREAD 1435L

//
// MessageId: ERROR_CHILD_WINDOW_MENU
//
// MessageText:
//
//  Child windows cannot have menus.
//
#define ERROR_CHILD_WINDOW_MENU          1436L

//
// MessageId: ERROR_NO_SYSTEM_MENU
//
// MessageText:
//
//  The window does not have a system menu.
//
#define ERROR_NO_SYSTEM_MENU             1437L

//
// MessageId: ERROR_INVALID_MSGBOX_STYLE
//
// MessageText:
//
//  Invalid message box style.
//
#define ERROR_INVALID_MSGBOX_STYLE       1438L

//
// MessageId: ERROR_INVALID_SPI_VALUE
//
// MessageText:
//
//  Invalid system-wide (SPI_*) parameter.
//
#define ERROR_INVALID_SPI_VALUE          1439L

//
// MessageId: ERROR_SCREEN_ALREADY_LOCKED
//
// MessageText:
//
//  Screen already locked.
//
#define ERROR_SCREEN_ALREADY_LOCKED      1440L

//
// MessageId: ERROR_HWNDS_HAVE_DIFF_PARENT
//
// MessageText:
//
//  All handles to windows in a multiple-window position structure must have the same parent.
//
#define ERROR_HWNDS_HAVE_DIFF_PARENT     1441L

//
// MessageId: ERROR_NOT_CHILD_WINDOW
//
// MessageText:
//
//  The window is not a child window.
//
#define ERROR_NOT_CHILD_WINDOW           1442L

//
// MessageId: ERROR_INVALID_GW_COMMAND
//
// MessageText:
//
//  Invalid GW_* command.
//
#define ERROR_INVALID_GW_COMMAND         1443L

//
// MessageId: ERROR_INVALID_THREAD_ID
//
// MessageText:
//
//  Invalid thread identifier.
//
#define ERROR_INVALID_THREAD_ID          1444L

//
// MessageId: ERROR_NON_MDICHILD_WINDOW
//
// MessageText:
//
//  Cannot process a message from a window that is not a multiple document interface (MDI) window.
//
#define ERROR_NON_MDICHILD_WINDOW        1445L

//
// MessageId: ERROR_POPUP_ALREADY_ACTIVE
//
// MessageText:
//
//  Popup menu already active.
//
#define ERROR_POPUP_ALREADY_ACTIVE       1446L

//
// MessageId: ERROR_NO_SCROLLBARS
//
// MessageText:
//
//  The window does not have scroll bars.
//
#define ERROR_NO_SCROLLBARS              1447L

//
// MessageId: ERROR_INVALID_SCROLLBAR_RANGE
//
// MessageText:
//
//  Scroll bar range cannot be greater than MAXLONG.
//
#define ERROR_INVALID_SCROLLBAR_RANGE    1448L

//
// MessageId: ERROR_INVALID_SHOWWIN_COMMAND
//
// MessageText:
//
//  Cannot show or remove the window in the way specified.
//
#define ERROR_INVALID_SHOWWIN_COMMAND    1449L

//
// MessageId: ERROR_NO_SYSTEM_RESOURCES
//
// MessageText:
//
//  Insufficient system resources exist to complete the requested service.
//
#define ERROR_NO_SYSTEM_RESOURCES        1450L

//
// MessageId: ERROR_NONPAGED_SYSTEM_RESOURCES
//
// MessageText:
//
//  Insufficient system resources exist to complete the requested service.
//
#define ERROR_NONPAGED_SYSTEM_RESOURCES  1451L

//
// MessageId: ERROR_PAGED_SYSTEM_RESOURCES
//
// MessageText:
//
//  Insufficient system resources exist to complete the requested service.
//
#define ERROR_PAGED_SYSTEM_RESOURCES     1452L

//
// MessageId: ERROR_WORKING_SET_QUOTA
//
// MessageText:
//
//  Insufficient quota to complete the requested service.
//
#define ERROR_WORKING_SET_QUOTA          1453L

//
// MessageId: ERROR_PAGEFILE_QUOTA
//
// MessageText:
//
//  Insufficient quota to complete the requested service.
//
#define ERROR_PAGEFILE_QUOTA             1454L

//
// MessageId: ERROR_COMMITMENT_LIMIT
//
// MessageText:
//
//  The paging file is too small for this operation to complete.
//
#define ERROR_COMMITMENT_LIMIT           1455L

//
// MessageId: ERROR_MENU_ITEM_NOT_FOUND
//
// MessageText:
//
//  A menu item was not found.
//
#define ERROR_MENU_ITEM_NOT_FOUND        1456L

//
// MessageId: ERROR_INVALID_KEYBOARD_HANDLE
//
// MessageText:
//
//  Invalid keyboard layout handle.
//
#define ERROR_INVALID_KEYBOARD_HANDLE    1457L

//
// MessageId: ERROR_HOOK_TYPE_NOT_ALLOWED
//
// MessageText:
//
//  Hook type not allowed.
//
#define ERROR_HOOK_TYPE_NOT_ALLOWED      1458L

//
// MessageId: ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION
//
// MessageText:
//
//  This operation requires an interactive window station.
//
#define ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION 1459L

//
// MessageId: ERROR_TIMEOUT
//
// MessageText:
//
//  This operation returned because the timeout period expired.
//
#define ERROR_TIMEOUT                    1460L

//
// MessageId: ERROR_INVALID_MONITOR_HANDLE
//
// MessageText:
//
//  Invalid monitor handle.
//
#define ERROR_INVALID_MONITOR_HANDLE     1461L

//
// MessageId: ERROR_INCORRECT_SIZE
//
// MessageText:
//
//  Incorrect size argument.
//
#define ERROR_INCORRECT_SIZE             1462L

// End of WinUser error codes



///////////////////////////
//                       //
// Eventlog Status Codes //
//                       //
///////////////////////////


//
// MessageId: ERROR_EVENTLOG_FILE_CORRUPT
//
// MessageText:
//
//  The event log file is corrupted.
//
#define ERROR_EVENTLOG_FILE_CORRUPT      1500L

//
// MessageId: ERROR_EVENTLOG_CANT_START
//
// MessageText:
//
//  No event log file could be opened, so the event logging service did not start.
//
#define ERROR_EVENTLOG_CANT_START        1501L

//
// MessageId: ERROR_LOG_FILE_FULL
//
// MessageText:
//
//  The event log file is full.
//
#define ERROR_LOG_FILE_FULL              1502L

//
// MessageId: ERROR_EVENTLOG_FILE_CHANGED
//
// MessageText:
//
//  The event log file has changed between read operations.
//
#define ERROR_EVENTLOG_FILE_CHANGED      1503L

// End of eventlog error codes



///////////////////////////
//                       //
// MSI Error Codes       //
//                       //
///////////////////////////


//
// MessageId: ERROR_INSTALL_SERVICE_FAILURE
//
// MessageText:
//
//  The Windows Installer Service could not be accessed. This can occur if you are running Windows in safe mode, or if the Windows Installer is not correctly installed. Contact your support personnel for assistance.
//
#define ERROR_INSTALL_SERVICE_FAILURE    1601L

//
// MessageId: ERROR_INSTALL_USEREXIT
//
// MessageText:
//
//  User cancelled installation.
//
#define ERROR_INSTALL_USEREXIT           1602L

//
// MessageId: ERROR_INSTALL_FAILURE
//
// MessageText:
//
//  Fatal error during installation.
//
#define ERROR_INSTALL_FAILURE            1603L

//
// MessageId: ERROR_INSTALL_SUSPEND
//
// MessageText:
//
//  Installation suspended, incomplete.
//
#define ERROR_INSTALL_SUSPEND            1604L

//
// MessageId: ERROR_UNKNOWN_PRODUCT
//
// MessageText:
//
//  This action is only valid for products that are currently installed.
//
#define ERROR_UNKNOWN_PRODUCT            1605L

//
// MessageId: ERROR_UNKNOWN_FEATURE
//
// MessageText:
//
//  Feature ID not registered.
//
#define ERROR_UNKNOWN_FEATURE            1606L

//
// MessageId: ERROR_UNKNOWN_COMPONENT
//
// MessageText:
//
//  Component ID not registered.
//
#define ERROR_UNKNOWN_COMPONENT          1607L

//
// MessageId: ERROR_UNKNOWN_PROPERTY
//
// MessageText:
//
//  Unknown property.
//
#define ERROR_UNKNOWN_PROPERTY           1608L

//
// MessageId: ERROR_INVALID_HANDLE_STATE
//
// MessageText:
//
//  Handle is in an invalid state.
//
#define ERROR_INVALID_HANDLE_STATE       1609L

//
// MessageId: ERROR_BAD_CONFIGURATION
//
// MessageText:
//
//  The configuration data for this product is corrupt.  Contact your support personnel.
//
#define ERROR_BAD_CONFIGURATION          1610L

//
// MessageId: ERROR_INDEX_ABSENT
//
// MessageText:
//
//  Component qualifier not present.
//
#define ERROR_INDEX_ABSENT               1611L

//
// MessageId: ERROR_INSTALL_SOURCE_ABSENT
//
// MessageText:
//
//  The installation source for this product is not available.  Verify that the source exists and that you can access it.
//
#define ERROR_INSTALL_SOURCE_ABSENT      1612L

//
// MessageId: ERROR_INSTALL_PACKAGE_VERSION
//
// MessageText:
//
//  This installation package cannot be installed by the Windows Installer service.  You must install a Windows service pack that contains a newer version of the Windows Installer service.
//
#define ERROR_INSTALL_PACKAGE_VERSION    1613L

//
// MessageId: ERROR_PRODUCT_UNINSTALLED
//
// MessageText:
//
//  Product is uninstalled.
//
#define ERROR_PRODUCT_UNINSTALLED        1614L

//
// MessageId: ERROR_BAD_QUERY_SYNTAX
//
// MessageText:
//
//  SQL query syntax invalid or unsupported.
//
#define ERROR_BAD_QUERY_SYNTAX           1615L

//
// MessageId: ERROR_INVALID_FIELD
//
// MessageText:
//
//  Record field does not exist.
//
#define ERROR_INVALID_FIELD              1616L

//
// MessageId: ERROR_DEVICE_REMOVED
//
// MessageText:
//
//  The device has been removed.
//
#define ERROR_DEVICE_REMOVED             1617L

//
// MessageId: ERROR_INSTALL_ALREADY_RUNNING
//
// MessageText:
//
//  Another installation is already in progress.  Complete that installation before proceeding with this install.
//
#define ERROR_INSTALL_ALREADY_RUNNING    1618L

//
// MessageId: ERROR_INSTALL_PACKAGE_OPEN_FAILED
//
// MessageText:
//
//  This installation package could not be opened.  Verify that the package exists and that you can access it, or contact the application vendor to verify that this is a valid Windows Installer package.
//
#define ERROR_INSTALL_PACKAGE_OPEN_FAILED 1619L

//
// MessageId: ERROR_INSTALL_PACKAGE_INVALID
//
// MessageText:
//
//  This installation package could not be opened.  Contact the application vendor to verify that this is a valid Windows Installer package.
//
#define ERROR_INSTALL_PACKAGE_INVALID    1620L

//
// MessageId: ERROR_INSTALL_UI_FAILURE
//
// MessageText:
//
//  There was an error starting the Windows Installer service user interface.  Contact your support personnel.
//
#define ERROR_INSTALL_UI_FAILURE         1621L

//
// MessageId: ERROR_INSTALL_LOG_FAILURE
//
// MessageText:
//
//  Error opening installation log file. Verify that the specified log file location exists and that you can write to it.
//
#define ERROR_INSTALL_LOG_FAILURE        1622L

//
// MessageId: ERROR_INSTALL_LANGUAGE_UNSUPPORTED
//
// MessageText:
//
//  The language of this installation package is not supported by your system.
//
#define ERROR_INSTALL_LANGUAGE_UNSUPPORTED 1623L

//
// MessageId: ERROR_INSTALL_TRANSFORM_FAILURE
//
// MessageText:
//
//  Error applying transforms.  Verify that the specified transform paths are valid.
//
#define ERROR_INSTALL_TRANSFORM_FAILURE  1624L

//
// MessageId: ERROR_INSTALL_PACKAGE_REJECTED
//
// MessageText:
//
//  This installation is forbidden by system policy.  Contact your system administrator.
//
#define ERROR_INSTALL_PACKAGE_REJECTED   1625L

//
// MessageId: ERROR_FUNCTION_NOT_CALLED
//
// MessageText:
//
//  Function could not be executed.
//
#define ERROR_FUNCTION_NOT_CALLED        1626L

//
// MessageId: ERROR_FUNCTION_FAILED
//
// MessageText:
//
//  Function failed during execution.
//
#define ERROR_FUNCTION_FAILED            1627L

//
// MessageId: ERROR_INVALID_TABLE
//
// MessageText:
//
//  Invalid or unknown table specified.
//
#define ERROR_INVALID_TABLE              1628L

//
// MessageId: ERROR_DATATYPE_MISMATCH
//
// MessageText:
//
//  Data supplied is of wrong type.
//
#define ERROR_DATATYPE_MISMATCH          1629L

//
// MessageId: ERROR_UNSUPPORTED_TYPE
//
// MessageText:
//
//  Data of this type is not supported.
//
#define ERROR_UNSUPPORTED_TYPE           1630L

//
// MessageId: ERROR_CREATE_FAILED
//
// MessageText:
//
//  The Windows Installer service failed to start.  Contact your support personnel.
//
#define ERROR_CREATE_FAILED              1631L

//
// MessageId: ERROR_INSTALL_TEMP_UNWRITABLE
//
// MessageText:
//
//  The Temp folder is on a drive that is full or is inaccessible. Free up space on the drive or verify that you have write permission on the Temp folder.
//
#define ERROR_INSTALL_TEMP_UNWRITABLE    1632L

//
// MessageId: ERROR_INSTALL_PLATFORM_UNSUPPORTED
//
// MessageText:
//
//  This installation package is not supported by this processor type. Contact your product vendor.
//
#define ERROR_INSTALL_PLATFORM_UNSUPPORTED 1633L

//
// MessageId: ERROR_INSTALL_NOTUSED
//
// MessageText:
//
//  Component not used on this computer.
//
#define ERROR_INSTALL_NOTUSED            1634L

//
// MessageId: ERROR_PATCH_PACKAGE_OPEN_FAILED
//
// MessageText:
//
//  This patch package could not be opened.  Verify that the patch package exists and that you can access it, or contact the application vendor to verify that this is a valid Windows Installer patch package.
//
#define ERROR_PATCH_PACKAGE_OPEN_FAILED  1635L

//
// MessageId: ERROR_PATCH_PACKAGE_INVALID
//
// MessageText:
//
//  This patch package could not be opened.  Contact the application vendor to verify that this is a valid Windows Installer patch package.
//
#define ERROR_PATCH_PACKAGE_INVALID      1636L

//
// MessageId: ERROR_PATCH_PACKAGE_UNSUPPORTED
//
// MessageText:
//
//  This patch package cannot be processed by the Windows Installer service.  You must install a Windows service pack that contains a newer version of the Windows Installer service.
//
#define ERROR_PATCH_PACKAGE_UNSUPPORTED  1637L

//
// MessageId: ERROR_PRODUCT_VERSION
//
// MessageText:
//
//  Another version of this product is already installed.  Installation of this version cannot continue.  To configure or remove the existing version of this product, use Add/Remove Programs on the Control Panel.
//
#define ERROR_PRODUCT_VERSION            1638L

//
// MessageId: ERROR_INVALID_COMMAND_LINE
//
// MessageText:
//
//  Invalid command line argument.  Consult the Windows Installer SDK for detailed command line help.
//
#define ERROR_INVALID_COMMAND_LINE       1639L

//
// MessageId: ERROR_INSTALL_REMOTE_DISALLOWED
//
// MessageText:
//
//  Only administrators have permission to add, remove, or configure server software during a Terminal services remote session. If you want to install or configure software on the server, contact your network administrator.
//
#define ERROR_INSTALL_REMOTE_DISALLOWED  1640L

//
// MessageId: ERROR_SUCCESS_REBOOT_INITIATED
//
// MessageText:
//
//  The requested operation completed successfully.  The system will be restarted so the changes can take effect.
//
#define ERROR_SUCCESS_REBOOT_INITIATED   1641L

//
// MessageId: ERROR_PATCH_TARGET_NOT_FOUND
//
// MessageText:
//
//  The upgrade patch cannot be installed by the Windows Installer service because the program to be upgraded may be missing, or the upgrade patch may update a different version of the program. Verify that the program to be upgraded exists on your computer an
//  d that you have the correct upgrade patch.
//
#define ERROR_PATCH_TARGET_NOT_FOUND     1642L

//
// MessageId: ERROR_PATCH_PACKAGE_REJECTED
//
// MessageText:
//
//  The patch package is not permitted by software restriction policy.
//
#define ERROR_PATCH_PACKAGE_REJECTED     1643L

//
// MessageId: ERROR_INSTALL_TRANSFORM_REJECTED
//
// MessageText:
//
//  One or more customizations are not permitted by software restriction policy.
//
#define ERROR_INSTALL_TRANSFORM_REJECTED 1644L

//
// MessageId: ERROR_INSTALL_REMOTE_PROHIBITED
//
// MessageText:
//
//  The Windows Installer does not permit installation from a Remote Desktop Connection.
//
#define ERROR_INSTALL_REMOTE_PROHIBITED  1645L

// End of MSI error codes



///////////////////////////
//                       //
//   RPC Status Codes    //
//                       //
///////////////////////////


//
// MessageId: RPC_S_INVALID_STRING_BINDING
//
// MessageText:
//
//  The string binding is invalid.
//
#define RPC_S_INVALID_STRING_BINDING     1700L

//
// MessageId: RPC_S_WRONG_KIND_OF_BINDING
//
// MessageText:
//
//  The binding handle is not the correct type.
//
#define RPC_S_WRONG_KIND_OF_BINDING      1701L

//
// MessageId: RPC_S_INVALID_BINDING
//
// MessageText:
//
//  The binding handle is invalid.
//
#define RPC_S_INVALID_BINDING            1702L

//
// MessageId: RPC_S_PROTSEQ_NOT_SUPPORTED
//
// MessageText:
//
//  The RPC protocol sequence is not supported.
//
#define RPC_S_PROTSEQ_NOT_SUPPORTED      1703L

//
// MessageId: RPC_S_INVALID_RPC_PROTSEQ
//
// MessageText:
//
//  The RPC protocol sequence is invalid.
//
#define RPC_S_INVALID_RPC_PROTSEQ        1704L

//
// MessageId: RPC_S_INVALID_STRING_UUID
//
// MessageText:
//
//  The string universal unique identifier (UUID) is invalid.
//
#define RPC_S_INVALID_STRING_UUID        1705L

//
// MessageId: RPC_S_INVALID_ENDPOINT_FORMAT
//
// MessageText:
//
//  The endpoint format is invalid.
//
#define RPC_S_INVALID_ENDPOINT_FORMAT    1706L

//
// MessageId: RPC_S_INVALID_NET_ADDR
//
// MessageText:
//
//  The network address is invalid.
//
#define RPC_S_INVALID_NET_ADDR           1707L

//
// MessageId: RPC_S_NO_ENDPOINT_FOUND
//
// MessageText:
//
//  No endpoint was found.
//
#define RPC_S_NO_ENDPOINT_FOUND          1708L

//
// MessageId: RPC_S_INVALID_TIMEOUT
//
// MessageText:
//
//  The timeout value is invalid.
//
#define RPC_S_INVALID_TIMEOUT            1709L

//
// MessageId: RPC_S_OBJECT_NOT_FOUND
//
// MessageText:
//
//  The object universal unique identifier (UUID) was not found.
//
#define RPC_S_OBJECT_NOT_FOUND           1710L

//
// MessageId: RPC_S_ALREADY_REGISTERED
//
// MessageText:
//
//  The object universal unique identifier (UUID) has already been registered.
//
#define RPC_S_ALREADY_REGISTERED         1711L

//
// MessageId: RPC_S_TYPE_ALREADY_REGISTERED
//
// MessageText:
//
//  The type universal unique identifier (UUID) has already been registered.
//
#define RPC_S_TYPE_ALREADY_REGISTERED    1712L

//
// MessageId: RPC_S_ALREADY_LISTENING
//
// MessageText:
//
//  The RPC server is already listening.
//
#define RPC_S_ALREADY_LISTENING          1713L

//
// MessageId: RPC_S_NO_PROTSEQS_REGISTERED
//
// MessageText:
//
//  No protocol sequences have been registered.
//
#define RPC_S_NO_PROTSEQS_REGISTERED     1714L

//
// MessageId: RPC_S_NOT_LISTENING
//
// MessageText:
//
//  The RPC server is not listening.
//
#define RPC_S_NOT_LISTENING              1715L

//
// MessageId: RPC_S_UNKNOWN_MGR_TYPE
//
// MessageText:
//
//  The manager type is unknown.
//
#define RPC_S_UNKNOWN_MGR_TYPE           1716L

//
// MessageId: RPC_S_UNKNOWN_IF
//
// MessageText:
//
//  The interface is unknown.
//
#define RPC_S_UNKNOWN_IF                 1717L

//
// MessageId: RPC_S_NO_BINDINGS
//
// MessageText:
//
//  There are no bindings.
//
#define RPC_S_NO_BINDINGS                1718L

//
// MessageId: RPC_S_NO_PROTSEQS
//
// MessageText:
//
//  There are no protocol sequences.
//
#define RPC_S_NO_PROTSEQS                1719L

//
// MessageId: RPC_S_CANT_CREATE_ENDPOINT
//
// MessageText:
//
//  The endpoint cannot be created.
//
#define RPC_S_CANT_CREATE_ENDPOINT       1720L

//
// MessageId: RPC_S_OUT_OF_RESOURCES
//
// MessageText:
//
//  Not enough resources are available to complete this operation.
//
#define RPC_S_OUT_OF_RESOURCES           1721L

//
// MessageId: RPC_S_SERVER_UNAVAILABLE
//
// MessageText:
//
//  The RPC server is unavailable.
//
#define RPC_S_SERVER_UNAVAILABLE         1722L

//
// MessageId: RPC_S_SERVER_TOO_BUSY
//
// MessageText:
//
//  The RPC server is too busy to complete this operation.
//
#define RPC_S_SERVER_TOO_BUSY            1723L

//
// MessageId: RPC_S_INVALID_NETWORK_OPTIONS
//
// MessageText:
//
//  The network options are invalid.
//
#define RPC_S_INVALID_NETWORK_OPTIONS    1724L

//
// MessageId: RPC_S_NO_CALL_ACTIVE
//
// MessageText:
//
//  There are no remote procedure calls active on this thread.
//
#define RPC_S_NO_CALL_ACTIVE             1725L

//
// MessageId: RPC_S_CALL_FAILED
//
// MessageText:
//
//  The remote procedure call failed.
//
#define RPC_S_CALL_FAILED                1726L

//
// MessageId: RPC_S_CALL_FAILED_DNE
//
// MessageText:
//
//  The remote procedure call failed and did not execute.
//
#define RPC_S_CALL_FAILED_DNE            1727L

//
// MessageId: RPC_S_PROTOCOL_ERROR
//
// MessageText:
//
//  A remote procedure call (RPC) protocol error occurred.
//
#define RPC_S_PROTOCOL_ERROR             1728L

//
// MessageId: RPC_S_UNSUPPORTED_TRANS_SYN
//
// MessageText:
//
//  The transfer syntax is not supported by the RPC server.
//
#define RPC_S_UNSUPPORTED_TRANS_SYN      1730L

//
// MessageId: RPC_S_UNSUPPORTED_TYPE
//
// MessageText:
//
//  The universal unique identifier (UUID) type is not supported.
//
#define RPC_S_UNSUPPORTED_TYPE           1732L

//
// MessageId: RPC_S_INVALID_TAG
//
// MessageText:
//
//  The tag is invalid.
//
#define RPC_S_INVALID_TAG                1733L

//
// MessageId: RPC_S_INVALID_BOUND
//
// MessageText:
//
//  The array bounds are invalid.
//
#define RPC_S_INVALID_BOUND              1734L

//
// MessageId: RPC_S_NO_ENTRY_NAME
//
// MessageText:
//
//  The binding does not contain an entry name.
//
#define RPC_S_NO_ENTRY_NAME              1735L

//
// MessageId: RPC_S_INVALID_NAME_SYNTAX
//
// MessageText:
//
//  The name syntax is invalid.
//
#define RPC_S_INVALID_NAME_SYNTAX        1736L

//
// MessageId: RPC_S_UNSUPPORTED_NAME_SYNTAX
//
// MessageText:
//
//  The name syntax is not supported.
//
#define RPC_S_UNSUPPORTED_NAME_SYNTAX    1737L

//
// MessageId: RPC_S_UUID_NO_ADDRESS
//
// MessageText:
//
//  No network address is available to use to construct a universal unique identifier (UUID).
//
#define RPC_S_UUID_NO_ADDRESS            1739L

//
// MessageId: RPC_S_DUPLICATE_ENDPOINT
//
// MessageText:
//
//  The endpoint is a duplicate.
//
#define RPC_S_DUPLICATE_ENDPOINT         1740L

//
// MessageId: RPC_S_UNKNOWN_AUTHN_TYPE
//
// MessageText:
//
//  The authentication type is unknown.
//
#define RPC_S_UNKNOWN_AUTHN_TYPE         1741L

//
// MessageId: RPC_S_MAX_CALLS_TOO_SMALL
//
// MessageText:
//
//  The maximum number of calls is too small.
//
#define RPC_S_MAX_CALLS_TOO_SMALL        1742L

//
// MessageId: RPC_S_STRING_TOO_LONG
//
// MessageText:
//
//  The string is too long.
//
#define RPC_S_STRING_TOO_LONG            1743L

//
// MessageId: RPC_S_PROTSEQ_NOT_FOUND
//
// MessageText:
//
//  The RPC protocol sequence was not found.
//
#define RPC_S_PROTSEQ_NOT_FOUND          1744L

//
// MessageId: RPC_S_PROCNUM_OUT_OF_RANGE
//
// MessageText:
//
//  The procedure number is out of range.
//
#define RPC_S_PROCNUM_OUT_OF_RANGE       1745L

//
// MessageId: RPC_S_BINDING_HAS_NO_AUTH
//
// MessageText:
//
//  The binding does not contain any authentication information.
//
#define RPC_S_BINDING_HAS_NO_AUTH        1746L

//
// MessageId: RPC_S_UNKNOWN_AUTHN_SERVICE
//
// MessageText:
//
//  The authentication service is unknown.
//
#define RPC_S_UNKNOWN_AUTHN_SERVICE      1747L

//
// MessageId: RPC_S_UNKNOWN_AUTHN_LEVEL
//
// MessageText:
//
//  The authentication level is unknown.
//
#define RPC_S_UNKNOWN_AUTHN_LEVEL        1748L

//
// MessageId: RPC_S_INVALID_AUTH_IDENTITY
//
// MessageText:
//
//  The security context is invalid.
//
#define RPC_S_INVALID_AUTH_IDENTITY      1749L

//
// MessageId: RPC_S_UNKNOWN_AUTHZ_SERVICE
//
// MessageText:
//
//  The authorization service is unknown.
//
#define RPC_S_UNKNOWN_AUTHZ_SERVICE      1750L

//
// MessageId: EPT_S_INVALID_ENTRY
//
// MessageText:
//
//  The entry is invalid.
//
#define EPT_S_INVALID_ENTRY              1751L

//
// MessageId: EPT_S_CANT_PERFORM_OP
//
// MessageText:
//
//  The server endpoint cannot perform the operation.
//
#define EPT_S_CANT_PERFORM_OP            1752L

//
// MessageId: EPT_S_NOT_REGISTERED
//
// MessageText:
//
//  There are no more endpoints available from the endpoint mapper.
//
#define EPT_S_NOT_REGISTERED             1753L

//
// MessageId: RPC_S_NOTHING_TO_EXPORT
//
// MessageText:
//
//  No interfaces have been exported.
//
#define RPC_S_NOTHING_TO_EXPORT          1754L

//
// MessageId: RPC_S_INCOMPLETE_NAME
//
// MessageText:
//
//  The entry name is incomplete.
//
#define RPC_S_INCOMPLETE_NAME            1755L

//
// MessageId: RPC_S_INVALID_VERS_OPTION
//
// MessageText:
//
//  The version option is invalid.
//
#define RPC_S_INVALID_VERS_OPTION        1756L

//
// MessageId: RPC_S_NO_MORE_MEMBERS
//
// MessageText:
//
//  There are no more members.
//
#define RPC_S_NO_MORE_MEMBERS            1757L

//
// MessageId: RPC_S_NOT_ALL_OBJS_UNEXPORTED
//
// MessageText:
//
//  There is nothing to unexport.
//
#define RPC_S_NOT_ALL_OBJS_UNEXPORTED    1758L

//
// MessageId: RPC_S_INTERFACE_NOT_FOUND
//
// MessageText:
//
//  The interface was not found.
//
#define RPC_S_INTERFACE_NOT_FOUND        1759L

//
// MessageId: RPC_S_ENTRY_ALREADY_EXISTS
//
// MessageText:
//
//  The entry already exists.
//
#define RPC_S_ENTRY_ALREADY_EXISTS       1760L

//
// MessageId: RPC_S_ENTRY_NOT_FOUND
//
// MessageText:
//
//  The entry is not found.
//
#define RPC_S_ENTRY_NOT_FOUND            1761L

//
// MessageId: RPC_S_NAME_SERVICE_UNAVAILABLE
//
// MessageText:
//
//  The name service is unavailable.
//
#define RPC_S_NAME_SERVICE_UNAVAILABLE   1762L

//
// MessageId: RPC_S_INVALID_NAF_ID
//
// MessageText:
//
//  The network address family is invalid.
//
#define RPC_S_INVALID_NAF_ID             1763L

//
// MessageId: RPC_S_CANNOT_SUPPORT
//
// MessageText:
//
//  The requested operation is not supported.
//
#define RPC_S_CANNOT_SUPPORT             1764L

//
// MessageId: RPC_S_NO_CONTEXT_AVAILABLE
//
// MessageText:
//
//  No security context is available to allow impersonation.
//
#define RPC_S_NO_CONTEXT_AVAILABLE       1765L

//
// MessageId: RPC_S_INTERNAL_ERROR
//
// MessageText:
//
//  An internal error occurred in a remote procedure call (RPC).
//
#define RPC_S_INTERNAL_ERROR             1766L

//
// MessageId: RPC_S_ZERO_DIVIDE
//
// MessageText:
//
//  The RPC server attempted an integer division by zero.
//
#define RPC_S_ZERO_DIVIDE                1767L

//
// MessageId: RPC_S_ADDRESS_ERROR
//
// MessageText:
//
//  An addressing error occurred in the RPC server.
//
#define RPC_S_ADDRESS_ERROR              1768L

//
// MessageId: RPC_S_FP_DIV_ZERO
//
// MessageText:
//
//  A floating-point operation at the RPC server caused a division by zero.
//
#define RPC_S_FP_DIV_ZERO                1769L

//
// MessageId: RPC_S_FP_UNDERFLOW
//
// MessageText:
//
//  A floating-point underflow occurred at the RPC server.
//
#define RPC_S_FP_UNDERFLOW               1770L

//
// MessageId: RPC_S_FP_OVERFLOW
//
// MessageText:
//
//  A floating-point overflow occurred at the RPC server.
//
#define RPC_S_FP_OVERFLOW                1771L

//
// MessageId: RPC_X_NO_MORE_ENTRIES
//
// MessageText:
//
//  The list of RPC servers available for the binding of auto handles has been exhausted.
//
#define RPC_X_NO_MORE_ENTRIES            1772L

//
// MessageId: RPC_X_SS_CHAR_TRANS_OPEN_FAIL
//
// MessageText:
//
//  Unable to open the character translation table file.
//
#define RPC_X_SS_CHAR_TRANS_OPEN_FAIL    1773L

//
// MessageId: RPC_X_SS_CHAR_TRANS_SHORT_FILE
//
// MessageText:
//
//  The file containing the character translation table has fewer than 512 bytes.
//
#define RPC_X_SS_CHAR_TRANS_SHORT_FILE   1774L

//
// MessageId: RPC_X_SS_IN_NULL_CONTEXT
//
// MessageText:
//
//  A null context handle was passed from the client to the host during a remote procedure call.
//
#define RPC_X_SS_IN_NULL_CONTEXT         1775L

//
// MessageId: RPC_X_SS_CONTEXT_DAMAGED
//
// MessageText:
//
//  The context handle changed during a remote procedure call.
//
#define RPC_X_SS_CONTEXT_DAMAGED         1777L

//
// MessageId: RPC_X_SS_HANDLES_MISMATCH
//
// MessageText:
//
//  The binding handles passed to a remote procedure call do not match.
//
#define RPC_X_SS_HANDLES_MISMATCH        1778L

//
// MessageId: RPC_X_SS_CANNOT_GET_CALL_HANDLE
//
// MessageText:
//
//  The stub is unable to get the remote procedure call handle.
//
#define RPC_X_SS_CANNOT_GET_CALL_HANDLE  1779L

//
// MessageId: RPC_X_NULL_REF_POINTER
//
// MessageText:
//
//  A null reference pointer was passed to the stub.
//
#define RPC_X_NULL_REF_POINTER           1780L

//
// MessageId: RPC_X_ENUM_VALUE_OUT_OF_RANGE
//
// MessageText:
//
//  The enumeration value is out of range.
//
#define RPC_X_ENUM_VALUE_OUT_OF_RANGE    1781L

//
// MessageId: RPC_X_BYTE_COUNT_TOO_SMALL
//
// MessageText:
//
//  The byte count is too small.
//
#define RPC_X_BYTE_COUNT_TOO_SMALL       1782L

//
// MessageId: RPC_X_BAD_STUB_DATA
//
// MessageText:
//
//  The stub received bad data.
//
#define RPC_X_BAD_STUB_DATA              1783L

//
// MessageId: ERROR_INVALID_USER_BUFFER
//
// MessageText:
//
//  The supplied user buffer is not valid for the requested operation.
//
#define ERROR_INVALID_USER_BUFFER        1784L

//
// MessageId: ERROR_UNRECOGNIZED_MEDIA
//
// MessageText:
//
//  The disk media is not recognized. It may not be formatted.
//
#define ERROR_UNRECOGNIZED_MEDIA         1785L

//
// MessageId: ERROR_NO_TRUST_LSA_SECRET
//
// MessageText:
//
//  The workstation does not have a trust secret.
//
#define ERROR_NO_TRUST_LSA_SECRET        1786L

//
// MessageId: ERROR_NO_TRUST_SAM_ACCOUNT
//
// MessageText:
//
//  The security database on the server does not have a computer account for this workstation trust relationship.
//
#define ERROR_NO_TRUST_SAM_ACCOUNT       1787L

//
// MessageId: ERROR_TRUSTED_DOMAIN_FAILURE
//
// MessageText:
//
//  The trust relationship between the primary domain and the trusted domain failed.
//
#define ERROR_TRUSTED_DOMAIN_FAILURE     1788L

//
// MessageId: ERROR_TRUSTED_RELATIONSHIP_FAILURE
//
// MessageText:
//
//  The trust relationship between this workstation and the primary domain failed.
//
#define ERROR_TRUSTED_RELATIONSHIP_FAILURE 1789L

//
// MessageId: ERROR_TRUST_FAILURE
//
// MessageText:
//
//  The network logon failed.
//
#define ERROR_TRUST_FAILURE              1790L

//
// MessageId: RPC_S_CALL_IN_PROGRESS
//
// MessageText:
//
//  A remote procedure call is already in progress for this thread.
//
#define RPC_S_CALL_IN_PROGRESS           1791L

//
// MessageId: ERROR_NETLOGON_NOT_STARTED
//
// MessageText:
//
//  An attempt was made to logon, but the network logon service was not started.
//
#define ERROR_NETLOGON_NOT_STARTED       1792L

//
// MessageId: ERROR_ACCOUNT_EXPIRED
//
// MessageText:
//
//  The user's account has expired.
//
#define ERROR_ACCOUNT_EXPIRED            1793L

//
// MessageId: ERROR_REDIRECTOR_HAS_OPEN_HANDLES
//
// MessageText:
//
//  The redirector is in use and cannot be unloaded.
//
#define ERROR_REDIRECTOR_HAS_OPEN_HANDLES 1794L

//
// MessageId: ERROR_PRINTER_DRIVER_ALREADY_INSTALLED
//
// MessageText:
//
//  The specified printer driver is already installed.
//
#define ERROR_PRINTER_DRIVER_ALREADY_INSTALLED 1795L

//
// MessageId: ERROR_UNKNOWN_PORT
//
// MessageText:
//
//  The specified port is unknown.
//
#define ERROR_UNKNOWN_PORT               1796L

//
// MessageId: ERROR_UNKNOWN_PRINTER_DRIVER
//
// MessageText:
//
//  The printer driver is unknown.
//
#define ERROR_UNKNOWN_PRINTER_DRIVER     1797L

//
// MessageId: ERROR_UNKNOWN_PRINTPROCESSOR
//
// MessageText:
//
//  The print processor is unknown.
//
#define ERROR_UNKNOWN_PRINTPROCESSOR     1798L

//
// MessageId: ERROR_INVALID_SEPARATOR_FILE
//
// MessageText:
//
//  The specified separator file is invalid.
//
#define ERROR_INVALID_SEPARATOR_FILE     1799L

//
// MessageId: ERROR_INVALID_PRIORITY
//
// MessageText:
//
//  The specified priority is invalid.
//
#define ERROR_INVALID_PRIORITY           1800L

//
// MessageId: ERROR_INVALID_PRINTER_NAME
//
// MessageText:
//
//  The printer name is invalid.
//
#define ERROR_INVALID_PRINTER_NAME       1801L

//
// MessageId: ERROR_PRINTER_ALREADY_EXISTS
//
// MessageText:
//
//  The printer already exists.
//
#define ERROR_PRINTER_ALREADY_EXISTS     1802L

//
// MessageId: ERROR_INVALID_PRINTER_COMMAND
//
// MessageText:
//
//  The printer command is invalid.
//
#define ERROR_INVALID_PRINTER_COMMAND    1803L

//
// MessageId: ERROR_INVALID_DATATYPE
//
// MessageText:
//
//  The specified datatype is invalid.
//
#define ERROR_INVALID_DATATYPE           1804L

//
// MessageId: ERROR_INVALID_ENVIRONMENT
//
// MessageText:
//
//  The environment specified is invalid.
//
#define ERROR_INVALID_ENVIRONMENT        1805L

//
// MessageId: RPC_S_NO_MORE_BINDINGS
//
// MessageText:
//
//  There are no more bindings.
//
#define RPC_S_NO_MORE_BINDINGS           1806L

//
// MessageId: ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT
//
// MessageText:
//
//  The account used is an interdomain trust account. Use your global user account or local user account to access this server.
//
#define ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT 1807L

//
// MessageId: ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT
//
// MessageText:
//
//  The account used is a computer account. Use your global user account or local user account to access this server.
//
#define ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT 1808L

//
// MessageId: ERROR_NOLOGON_SERVER_TRUST_ACCOUNT
//
// MessageText:
//
//  The account used is a server trust account. Use your global user account or local user account to access this server.
//
#define ERROR_NOLOGON_SERVER_TRUST_ACCOUNT 1809L

//
// MessageId: ERROR_DOMAIN_TRUST_INCONSISTENT
//
// MessageText:
//
//  The name or security ID (SID) of the domain specified is inconsistent with the trust information for that domain.
//
#define ERROR_DOMAIN_TRUST_INCONSISTENT  1810L

//
// MessageId: ERROR_SERVER_HAS_OPEN_HANDLES
//
// MessageText:
//
//  The server is in use and cannot be unloaded.
//
#define ERROR_SERVER_HAS_OPEN_HANDLES    1811L

//
// MessageId: ERROR_RESOURCE_DATA_NOT_FOUND
//
// MessageText:
//
//  The specified image file did not contain a resource section.
//
#define ERROR_RESOURCE_DATA_NOT_FOUND    1812L

//
// MessageId: ERROR_RESOURCE_TYPE_NOT_FOUND
//
// MessageText:
//
//  The specified resource type cannot be found in the image file.
//
#define ERROR_RESOURCE_TYPE_NOT_FOUND    1813L

//
// MessageId: ERROR_RESOURCE_NAME_NOT_FOUND
//
// MessageText:
//
//  The specified resource name cannot be found in the image file.
//
#define ERROR_RESOURCE_NAME_NOT_FOUND    1814L

//
// MessageId: ERROR_RESOURCE_LANG_NOT_FOUND
//
// MessageText:
//
//  The specified resource language ID cannot be found in the image file.
//
#define ERROR_RESOURCE_LANG_NOT_FOUND    1815L

//
// MessageId: ERROR_NOT_ENOUGH_QUOTA
//
// MessageText:
//
//  Not enough quota is available to process this command.
//
#define ERROR_NOT_ENOUGH_QUOTA           1816L

//
// MessageId: RPC_S_NO_INTERFACES
//
// MessageText:
//
//  No interfaces have been registered.
//
#define RPC_S_NO_INTERFACES              1817L

//
// MessageId: RPC_S_CALL_CANCELLED
//
// MessageText:
//
//  The remote procedure call was cancelled.
//
#define RPC_S_CALL_CANCELLED             1818L

//
// MessageId: RPC_S_BINDING_INCOMPLETE
//
// MessageText:
//
//  The binding handle does not contain all required information.
//
#define RPC_S_BINDING_INCOMPLETE         1819L

//
// MessageId: RPC_S_COMM_FAILURE
//
// MessageText:
//
//  A communications failure occurred during a remote procedure call.
//
#define RPC_S_COMM_FAILURE               1820L

//
// MessageId: RPC_S_UNSUPPORTED_AUTHN_LEVEL
//
// MessageText:
//
//  The requested authentication level is not supported.
//
#define RPC_S_UNSUPPORTED_AUTHN_LEVEL    1821L

//
// MessageId: RPC_S_NO_PRINC_NAME
//
// MessageText:
//
//  No principal name registered.
//
#define RPC_S_NO_PRINC_NAME              1822L

//
// MessageId: RPC_S_NOT_RPC_ERROR
//
// MessageText:
//
//  The error specified is not a valid Windows RPC error code.
//
#define RPC_S_NOT_RPC_ERROR              1823L

//
// MessageId: RPC_S_UUID_LOCAL_ONLY
//
// MessageText:
//
//  A UUID that is valid only on this computer has been allocated.
//
#define RPC_S_UUID_LOCAL_ONLY            1824L

//
// MessageId: RPC_S_SEC_PKG_ERROR
//
// MessageText:
//
//  A security package specific error occurred.
//
#define RPC_S_SEC_PKG_ERROR              1825L

//
// MessageId: RPC_S_NOT_CANCELLED
//
// MessageText:
//
//  Thread is not canceled.
//
#define RPC_S_NOT_CANCELLED              1826L

//
// MessageId: RPC_X_INVALID_ES_ACTION
//
// MessageText:
//
//  Invalid operation on the encoding/decoding handle.
//
#define RPC_X_INVALID_ES_ACTION          1827L

//
// MessageId: RPC_X_WRONG_ES_VERSION
//
// MessageText:
//
//  Incompatible version of the serializing package.
//
#define RPC_X_WRONG_ES_VERSION           1828L

//
// MessageId: RPC_X_WRONG_STUB_VERSION
//
// MessageText:
//
//  Incompatible version of the RPC stub.
//
#define RPC_X_WRONG_STUB_VERSION         1829L

//
// MessageId: RPC_X_INVALID_PIPE_OBJECT
//
// MessageText:
//
//  The RPC pipe object is invalid or corrupted.
//
#define RPC_X_INVALID_PIPE_OBJECT        1830L

//
// MessageId: RPC_X_WRONG_PIPE_ORDER
//
// MessageText:
//
//  An invalid operation was attempted on an RPC pipe object.
//
#define RPC_X_WRONG_PIPE_ORDER           1831L

//
// MessageId: RPC_X_WRONG_PIPE_VERSION
//
// MessageText:
//
//  Unsupported RPC pipe version.
//
#define RPC_X_WRONG_PIPE_VERSION         1832L

//
// MessageId: RPC_S_GROUP_MEMBER_NOT_FOUND
//
// MessageText:
//
//  The group member was not found.
//
#define RPC_S_GROUP_MEMBER_NOT_FOUND     1898L

//
// MessageId: EPT_S_CANT_CREATE
//
// MessageText:
//
//  The endpoint mapper database entry could not be created.
//
#define EPT_S_CANT_CREATE                1899L

//
// MessageId: RPC_S_INVALID_OBJECT
//
// MessageText:
//
//  The object universal unique identifier (UUID) is the nil UUID.
//
#define RPC_S_INVALID_OBJECT             1900L

//
// MessageId: ERROR_INVALID_TIME
//
// MessageText:
//
//  The specified time is invalid.
//
#define ERROR_INVALID_TIME               1901L

//
// MessageId: ERROR_INVALID_FORM_NAME
//
// MessageText:
//
//  The specified form name is invalid.
//
#define ERROR_INVALID_FORM_NAME          1902L

//
// MessageId: ERROR_INVALID_FORM_SIZE
//
// MessageText:
//
//  The specified form size is invalid.
//
#define ERROR_INVALID_FORM_SIZE          1903L

//
// MessageId: ERROR_ALREADY_WAITING
//
// MessageText:
//
//  The specified printer handle is already being waited on
//
#define ERROR_ALREADY_WAITING            1904L

//
// MessageId: ERROR_PRINTER_DELETED
//
// MessageText:
//
//  The specified printer has been deleted.
//
#define ERROR_PRINTER_DELETED            1905L

//
// MessageId: ERROR_INVALID_PRINTER_STATE
//
// MessageText:
//
//  The state of the printer is invalid.
//
#define ERROR_INVALID_PRINTER_STATE      1906L

//
// MessageId: ERROR_PASSWORD_MUST_CHANGE
//
// MessageText:
//
//  The user's password must be changed before logging on the first time.
//
#define ERROR_PASSWORD_MUST_CHANGE       1907L

//
// MessageId: ERROR_DOMAIN_CONTROLLER_NOT_FOUND
//
// MessageText:
//
//  Could not find the domain controller for this domain.
//
#define ERROR_DOMAIN_CONTROLLER_NOT_FOUND 1908L

//
// MessageId: ERROR_ACCOUNT_LOCKED_OUT
//
// MessageText:
//
//  The referenced account is currently locked out and may not be logged on to.
//
#define ERROR_ACCOUNT_LOCKED_OUT         1909L

//
// MessageId: OR_INVALID_OXID
//
// MessageText:
//
//  The object exporter specified was not found.
//
#define OR_INVALID_OXID                  1910L

//
// MessageId: OR_INVALID_OID
//
// MessageText:
//
//  The object specified was not found.
//
#define OR_INVALID_OID                   1911L

//
// MessageId: OR_INVALID_SET
//
// MessageText:
//
//  The object resolver set specified was not found.
//
#define OR_INVALID_SET                   1912L

//
// MessageId: RPC_S_SEND_INCOMPLETE
//
// MessageText:
//
//  Some data remains to be sent in the request buffer.
//
#define RPC_S_SEND_INCOMPLETE            1913L

//
// MessageId: RPC_S_INVALID_ASYNC_HANDLE
//
// MessageText:
//
//  Invalid asynchronous remote procedure call handle.
//
#define RPC_S_INVALID_ASYNC_HANDLE       1914L

//
// MessageId: RPC_S_INVALID_ASYNC_CALL
//
// MessageText:
//
//  Invalid asynchronous RPC call handle for this operation.
//
#define RPC_S_INVALID_ASYNC_CALL         1915L

//
// MessageId: RPC_X_PIPE_CLOSED
//
// MessageText:
//
//  The RPC pipe object has already been closed.
//
#define RPC_X_PIPE_CLOSED                1916L

//
// MessageId: RPC_X_PIPE_DISCIPLINE_ERROR
//
// MessageText:
//
//  The RPC call completed before all pipes were processed.
//
#define RPC_X_PIPE_DISCIPLINE_ERROR      1917L

//
// MessageId: RPC_X_PIPE_EMPTY
//
// MessageText:
//
//  No more data is available from the RPC pipe.
//
#define RPC_X_PIPE_EMPTY                 1918L

//
// MessageId: ERROR_NO_SITENAME
//
// MessageText:
//
//  No site name is available for this machine.
//
#define ERROR_NO_SITENAME                1919L

//
// MessageId: ERROR_CANT_ACCESS_FILE
//
// MessageText:
//
//  The file can not be accessed by the system.
//
#define ERROR_CANT_ACCESS_FILE           1920L

//
// MessageId: ERROR_CANT_RESOLVE_FILENAME
//
// MessageText:
//
//  The name of the file cannot be resolved by the system.
//
#define ERROR_CANT_RESOLVE_FILENAME      1921L

//
// MessageId: RPC_S_ENTRY_TYPE_MISMATCH
//
// MessageText:
//
//  The entry is not of the expected type.
//
#define RPC_S_ENTRY_TYPE_MISMATCH        1922L

//
// MessageId: RPC_S_NOT_ALL_OBJS_EXPORTED
//
// MessageText:
//
//  Not all object UUIDs could be exported to the specified entry.
//
#define RPC_S_NOT_ALL_OBJS_EXPORTED      1923L

//
// MessageId: RPC_S_INTERFACE_NOT_EXPORTED
//
// MessageText:
//
//  Interface could not be exported to the specified entry.
//
#define RPC_S_INTERFACE_NOT_EXPORTED     1924L

//
// MessageId: RPC_S_PROFILE_NOT_ADDED
//
// MessageText:
//
//  The specified profile entry could not be added.
//
#define RPC_S_PROFILE_NOT_ADDED          1925L

//
// MessageId: RPC_S_PRF_ELT_NOT_ADDED
//
// MessageText:
//
//  The specified profile element could not be added.
//
#define RPC_S_PRF_ELT_NOT_ADDED          1926L

//
// MessageId: RPC_S_PRF_ELT_NOT_REMOVED
//
// MessageText:
//
//  The specified profile element could not be removed.
//
#define RPC_S_PRF_ELT_NOT_REMOVED        1927L

//
// MessageId: RPC_S_GRP_ELT_NOT_ADDED
//
// MessageText:
//
//  The group element could not be added.
//
#define RPC_S_GRP_ELT_NOT_ADDED          1928L

//
// MessageId: RPC_S_GRP_ELT_NOT_REMOVED
//
// MessageText:
//
//  The group element could not be removed.
//
#define RPC_S_GRP_ELT_NOT_REMOVED        1929L

//
// MessageId: ERROR_KM_DRIVER_BLOCKED
//
// MessageText:
//
//  The printer driver is not compatible with a policy enabled on your computer that blocks NT 4.0 drivers.
//
#define ERROR_KM_DRIVER_BLOCKED          1930L

//
// MessageId: ERROR_CONTEXT_EXPIRED
//
// MessageText:
//
//  The context has expired and can no longer be used.
//
#define ERROR_CONTEXT_EXPIRED            1931L

//
// MessageId: ERROR_PER_USER_TRUST_QUOTA_EXCEEDED
//
// MessageText:
//
//  The current user's delegated trust creation quota has been exceeded.
//
#define ERROR_PER_USER_TRUST_QUOTA_EXCEEDED 1932L

//
// MessageId: ERROR_ALL_USER_TRUST_QUOTA_EXCEEDED
//
// MessageText:
//
//  The total delegated trust creation quota has been exceeded.
//
#define ERROR_ALL_USER_TRUST_QUOTA_EXCEEDED 1933L

//
// MessageId: ERROR_USER_DELETE_TRUST_QUOTA_EXCEEDED
//
// MessageText:
//
//  The current user's delegated trust deletion quota has been exceeded.
//
#define ERROR_USER_DELETE_TRUST_QUOTA_EXCEEDED 1934L

//
// MessageId: ERROR_AUTHENTICATION_FIREWALL_FAILED
//
// MessageText:
//
//  Logon Failure: The machine you are logging onto is protected by an authentication firewall.  The specified account is not allowed to authenticate to the machine.
//
#define ERROR_AUTHENTICATION_FIREWALL_FAILED 1935L

//
// MessageId: ERROR_REMOTE_PRINT_CONNECTIONS_BLOCKED
//
// MessageText:
//
//  Remote connections to the Print Spooler are blocked by a policy set on your machine.
//
#define ERROR_REMOTE_PRINT_CONNECTIONS_BLOCKED 1936L




///////////////////////////
//                       //
//   OpenGL Error Code   //
//                       //
///////////////////////////


//
// MessageId: ERROR_INVALID_PIXEL_FORMAT
//
// MessageText:
//
//  The pixel format is invalid.
//
#define ERROR_INVALID_PIXEL_FORMAT       2000L

//
// MessageId: ERROR_BAD_DRIVER
//
// MessageText:
//
//  The specified driver is invalid.
//
#define ERROR_BAD_DRIVER                 2001L

//
// MessageId: ERROR_INVALID_WINDOW_STYLE
//
// MessageText:
//
//  The window style or class attribute is invalid for this operation.
//
#define ERROR_INVALID_WINDOW_STYLE       2002L

//
// MessageId: ERROR_METAFILE_NOT_SUPPORTED
//
// MessageText:
//
//  The requested metafile operation is not supported.
//
#define ERROR_METAFILE_NOT_SUPPORTED     2003L

//
// MessageId: ERROR_TRANSFORM_NOT_SUPPORTED
//
// MessageText:
//
//  The requested transformation operation is not supported.
//
#define ERROR_TRANSFORM_NOT_SUPPORTED    2004L

//
// MessageId: ERROR_CLIPPING_NOT_SUPPORTED
//
// MessageText:
//
//  The requested clipping operation is not supported.
//
#define ERROR_CLIPPING_NOT_SUPPORTED     2005L

// End of OpenGL error codes



///////////////////////////////////////////
//                                       //
//   Image Color Management Error Code   //
//                                       //
///////////////////////////////////////////


//
// MessageId: ERROR_INVALID_CMM
//
// MessageText:
//
//  The specified color management module is invalid.
//
#define ERROR_INVALID_CMM                2010L

//
// MessageId: ERROR_INVALID_PROFILE
//
// MessageText:
//
//  The specified color profile is invalid.
//
#define ERROR_INVALID_PROFILE            2011L

//
// MessageId: ERROR_TAG_NOT_FOUND
//
// MessageText:
//
//  The specified tag was not found.
//
#define ERROR_TAG_NOT_FOUND              2012L

//
// MessageId: ERROR_TAG_NOT_PRESENT
//
// MessageText:
//
//  A required tag is not present.
//
#define ERROR_TAG_NOT_PRESENT            2013L

//
// MessageId: ERROR_DUPLICATE_TAG
//
// MessageText:
//
//  The specified tag is already present.
//
#define ERROR_DUPLICATE_TAG              2014L

//
// MessageId: ERROR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE
//
// MessageText:
//
//  The specified color profile is not associated with any device.
//
#define ERROR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE 2015L

//
// MessageId: ERROR_PROFILE_NOT_FOUND
//
// MessageText:
//
//  The specified color profile was not found.
//
#define ERROR_PROFILE_NOT_FOUND          2016L

//
// MessageId: ERROR_INVALID_COLORSPACE
//
// MessageText:
//
//  The specified color space is invalid.
//
#define ERROR_INVALID_COLORSPACE         2017L

//
// MessageId: ERROR_ICM_NOT_ENABLED
//
// MessageText:
//
//  Image Color Management is not enabled.
//
#define ERROR_ICM_NOT_ENABLED            2018L

//
// MessageId: ERROR_DELETING_ICM_XFORM
//
// MessageText:
//
//  There was an error while deleting the color transform.
//
#define ERROR_DELETING_ICM_XFORM         2019L

//
// MessageId: ERROR_INVALID_TRANSFORM
//
// MessageText:
//
//  The specified color transform is invalid.
//
#define ERROR_INVALID_TRANSFORM          2020L

//
// MessageId: ERROR_COLORSPACE_MISMATCH
//
// MessageText:
//
//  The specified transform does not match the bitmap's color space.
//
#define ERROR_COLORSPACE_MISMATCH        2021L

//
// MessageId: ERROR_INVALID_COLORINDEX
//
// MessageText:
//
//  The specified named color index is not present in the profile.
//
#define ERROR_INVALID_COLORINDEX         2022L




///////////////////////////
//                       //
// Winnet32 Status Codes //
//                       //
// The range 2100 through 2999 is reserved for network status codes.
// See lmerr.h for a complete listing
///////////////////////////


//
// MessageId: ERROR_CONNECTED_OTHER_PASSWORD
//
// MessageText:
//
//  The network connection was made successfully, but the user had to be prompted for a password other than the one originally specified.
//
#define ERROR_CONNECTED_OTHER_PASSWORD   2108L

//
// MessageId: ERROR_CONNECTED_OTHER_PASSWORD_DEFAULT
//
// MessageText:
//
//  The network connection was made successfully using default credentials.
//
#define ERROR_CONNECTED_OTHER_PASSWORD_DEFAULT 2109L

//
// MessageId: ERROR_BAD_USERNAME
//
// MessageText:
//
//  The specified username is invalid.
//
#define ERROR_BAD_USERNAME               2202L

//
// MessageId: ERROR_NOT_CONNECTED
//
// MessageText:
//
//  This network connection does not exist.
//
#define ERROR_NOT_CONNECTED              2250L

//
// MessageId: ERROR_OPEN_FILES
//
// MessageText:
//
//  This network connection has files open or requests pending.
//
#define ERROR_OPEN_FILES                 2401L

//
// MessageId: ERROR_ACTIVE_CONNECTIONS
//
// MessageText:
//
//  Active connections still exist.
//
#define ERROR_ACTIVE_CONNECTIONS         2402L

//
// MessageId: ERROR_DEVICE_IN_USE
//
// MessageText:
//
//  The device is in use by an active process and cannot be disconnected.
//
#define ERROR_DEVICE_IN_USE              2404L


////////////////////////////////////
//                                //
//     Win32 Spooler Error Codes  //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_UNKNOWN_PRINT_MONITOR
//
// MessageText:
//
//  The specified print monitor is unknown.
//
#define ERROR_UNKNOWN_PRINT_MONITOR      3000L

//
// MessageId: ERROR_PRINTER_DRIVER_IN_USE
//
// MessageText:
//
//  The specified printer driver is currently in use.
//
#define ERROR_PRINTER_DRIVER_IN_USE      3001L

//
// MessageId: ERROR_SPOOL_FILE_NOT_FOUND
//
// MessageText:
//
//  The spool file was not found.
//
#define ERROR_SPOOL_FILE_NOT_FOUND       3002L

//
// MessageId: ERROR_SPL_NO_STARTDOC
//
// MessageText:
//
//  A StartDocPrinter call was not issued.
//
#define ERROR_SPL_NO_STARTDOC            3003L

//
// MessageId: ERROR_SPL_NO_ADDJOB
//
// MessageText:
//
//  An AddJob call was not issued.
//
#define ERROR_SPL_NO_ADDJOB              3004L

//
// MessageId: ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED
//
// MessageText:
//
//  The specified print processor has already been installed.
//
#define ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED 3005L

//
// MessageId: ERROR_PRINT_MONITOR_ALREADY_INSTALLED
//
// MessageText:
//
//  The specified print monitor has already been installed.
//
#define ERROR_PRINT_MONITOR_ALREADY_INSTALLED 3006L

//
// MessageId: ERROR_INVALID_PRINT_MONITOR
//
// MessageText:
//
//  The specified print monitor does not have the required functions.
//
#define ERROR_INVALID_PRINT_MONITOR      3007L

//
// MessageId: ERROR_PRINT_MONITOR_IN_USE
//
// MessageText:
//
//  The specified print monitor is currently in use.
//
#define ERROR_PRINT_MONITOR_IN_USE       3008L

//
// MessageId: ERROR_PRINTER_HAS_JOBS_QUEUED
//
// MessageText:
//
//  The requested operation is not allowed when there are jobs queued to the printer.
//
#define ERROR_PRINTER_HAS_JOBS_QUEUED    3009L

//
// MessageId: ERROR_SUCCESS_REBOOT_REQUIRED
//
// MessageText:
//
//  The requested operation is successful. Changes will not be effective until the system is rebooted.
//
#define ERROR_SUCCESS_REBOOT_REQUIRED    3010L

//
// MessageId: ERROR_SUCCESS_RESTART_REQUIRED
//
// MessageText:
//
//  The requested operation is successful. Changes will not be effective until the service is restarted.
//
#define ERROR_SUCCESS_RESTART_REQUIRED   3011L

//
// MessageId: ERROR_PRINTER_NOT_FOUND
//
// MessageText:
//
//  No printers were found.
//
#define ERROR_PRINTER_NOT_FOUND          3012L

//
// MessageId: ERROR_PRINTER_DRIVER_WARNED
//
// MessageText:
//
//  The printer driver is known to be unreliable.
//
#define ERROR_PRINTER_DRIVER_WARNED      3013L

//
// MessageId: ERROR_PRINTER_DRIVER_BLOCKED
//
// MessageText:
//
//  The printer driver is known to harm the system.
//
#define ERROR_PRINTER_DRIVER_BLOCKED     3014L

////////////////////////////////////
//                                //
//     Wins Error Codes           //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_WINS_INTERNAL
//
// MessageText:
//
//  WINS encountered an error while processing the command.
//
#define ERROR_WINS_INTERNAL              4000L

//
// MessageId: ERROR_CAN_NOT_DEL_LOCAL_WINS
//
// MessageText:
//
//  The local WINS can not be deleted.
//
#define ERROR_CAN_NOT_DEL_LOCAL_WINS     4001L

//
// MessageId: ERROR_STATIC_INIT
//
// MessageText:
//
//  The importation from the file failed.
//
#define ERROR_STATIC_INIT                4002L

//
// MessageId: ERROR_INC_BACKUP
//
// MessageText:
//
//  The backup failed. Was a full backup done before?
//
#define ERROR_INC_BACKUP                 4003L

//
// MessageId: ERROR_FULL_BACKUP
//
// MessageText:
//
//  The backup failed. Check the directory to which you are backing the database.
//
#define ERROR_FULL_BACKUP                4004L

//
// MessageId: ERROR_REC_NON_EXISTENT
//
// MessageText:
//
//  The name does not exist in the WINS database.
//
#define ERROR_REC_NON_EXISTENT           4005L

//
// MessageId: ERROR_RPL_NOT_ALLOWED
//
// MessageText:
//
//  Replication with a nonconfigured partner is not allowed.
//
#define ERROR_RPL_NOT_ALLOWED            4006L

////////////////////////////////////
//                                //
//     DHCP Error Codes           //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_DHCP_ADDRESS_CONFLICT
//
// MessageText:
//
//  The DHCP client has obtained an IP address that is already in use on the network. The local interface will be disabled until the DHCP client can obtain a new address.
//
#define ERROR_DHCP_ADDRESS_CONFLICT      4100L

////////////////////////////////////
//                                //
//     WMI Error Codes            //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_WMI_GUID_NOT_FOUND
//
// MessageText:
//
//  The GUID passed was not recognized as valid by a WMI data provider.
//
#define ERROR_WMI_GUID_NOT_FOUND         4200L

//
// MessageId: ERROR_WMI_INSTANCE_NOT_FOUND
//
// MessageText:
//
//  The instance name passed was not recognized as valid by a WMI data provider.
//
#define ERROR_WMI_INSTANCE_NOT_FOUND     4201L

//
// MessageId: ERROR_WMI_ITEMID_NOT_FOUND
//
// MessageText:
//
//  The data item ID passed was not recognized as valid by a WMI data provider.
//
#define ERROR_WMI_ITEMID_NOT_FOUND       4202L

//
// MessageId: ERROR_WMI_TRY_AGAIN
//
// MessageText:
//
//  The WMI request could not be completed and should be retried.
//
#define ERROR_WMI_TRY_AGAIN              4203L

//
// MessageId: ERROR_WMI_DP_NOT_FOUND
//
// MessageText:
//
//  The WMI data provider could not be located.
//
#define ERROR_WMI_DP_NOT_FOUND           4204L

//
// MessageId: ERROR_WMI_UNRESOLVED_INSTANCE_REF
//
// MessageText:
//
//  The WMI data provider references an instance set that has not been registered.
//
#define ERROR_WMI_UNRESOLVED_INSTANCE_REF 4205L

//
// MessageId: ERROR_WMI_ALREADY_ENABLED
//
// MessageText:
//
//  The WMI data block or event notification has already been enabled.
//
#define ERROR_WMI_ALREADY_ENABLED        4206L

//
// MessageId: ERROR_WMI_GUID_DISCONNECTED
//
// MessageText:
//
//  The WMI data block is no longer available.
//
#define ERROR_WMI_GUID_DISCONNECTED      4207L

//
// MessageId: ERROR_WMI_SERVER_UNAVAILABLE
//
// MessageText:
//
//  The WMI data service is not available.
//
#define ERROR_WMI_SERVER_UNAVAILABLE     4208L

//
// MessageId: ERROR_WMI_DP_FAILED
//
// MessageText:
//
//  The WMI data provider failed to carry out the request.
//
#define ERROR_WMI_DP_FAILED              4209L

//
// MessageId: ERROR_WMI_INVALID_MOF
//
// MessageText:
//
//  The WMI MOF information is not valid.
//
#define ERROR_WMI_INVALID_MOF            4210L

//
// MessageId: ERROR_WMI_INVALID_REGINFO
//
// MessageText:
//
//  The WMI registration information is not valid.
//
#define ERROR_WMI_INVALID_REGINFO        4211L

//
// MessageId: ERROR_WMI_ALREADY_DISABLED
//
// MessageText:
//
//  The WMI data block or event notification has already been disabled.
//
#define ERROR_WMI_ALREADY_DISABLED       4212L

//
// MessageId: ERROR_WMI_READ_ONLY
//
// MessageText:
//
//  The WMI data item or data block is read only.
//
#define ERROR_WMI_READ_ONLY              4213L

//
// MessageId: ERROR_WMI_SET_FAILURE
//
// MessageText:
//
//  The WMI data item or data block could not be changed.
//
#define ERROR_WMI_SET_FAILURE            4214L

//////////////////////////////////////////
//                                      //
// NT Media Services (RSM) Error Codes  //
//                                      //
//////////////////////////////////////////
//
// MessageId: ERROR_INVALID_MEDIA
//
// MessageText:
//
//  The media identifier does not represent a valid medium.
//
#define ERROR_INVALID_MEDIA              4300L

//
// MessageId: ERROR_INVALID_LIBRARY
//
// MessageText:
//
//  The library identifier does not represent a valid library.
//
#define ERROR_INVALID_LIBRARY            4301L

//
// MessageId: ERROR_INVALID_MEDIA_POOL
//
// MessageText:
//
//  The media pool identifier does not represent a valid media pool.
//
#define ERROR_INVALID_MEDIA_POOL         4302L

//
// MessageId: ERROR_DRIVE_MEDIA_MISMATCH
//
// MessageText:
//
//  The drive and medium are not compatible or exist in different libraries.
//
#define ERROR_DRIVE_MEDIA_MISMATCH       4303L

//
// MessageId: ERROR_MEDIA_OFFLINE
//
// MessageText:
//
//  The medium currently exists in an offline library and must be online to perform this operation.
//
#define ERROR_MEDIA_OFFLINE              4304L

//
// MessageId: ERROR_LIBRARY_OFFLINE
//
// MessageText:
//
//  The operation cannot be performed on an offline library.
//
#define ERROR_LIBRARY_OFFLINE            4305L

//
// MessageId: ERROR_EMPTY
//
// MessageText:
//
//  The library, drive, or media pool is empty.
//
#define ERROR_EMPTY                      4306L

//
// MessageId: ERROR_NOT_EMPTY
//
// MessageText:
//
//  The library, drive, or media pool must be empty to perform this operation.
//
#define ERROR_NOT_EMPTY                  4307L

//
// MessageId: ERROR_MEDIA_UNAVAILABLE
//
// MessageText:
//
//  No media is currently available in this media pool or library.
//
#define ERROR_MEDIA_UNAVAILABLE          4308L

//
// MessageId: ERROR_RESOURCE_DISABLED
//
// MessageText:
//
//  A resource required for this operation is disabled.
//
#define ERROR_RESOURCE_DISABLED          4309L

//
// MessageId: ERROR_INVALID_CLEANER
//
// MessageText:
//
//  The media identifier does not represent a valid cleaner.
//
#define ERROR_INVALID_CLEANER            4310L

//
// MessageId: ERROR_UNABLE_TO_CLEAN
//
// MessageText:
//
//  The drive cannot be cleaned or does not support cleaning.
//
#define ERROR_UNABLE_TO_CLEAN            4311L

//
// MessageId: ERROR_OBJECT_NOT_FOUND
//
// MessageText:
//
//  The object identifier does not represent a valid object.
//
#define ERROR_OBJECT_NOT_FOUND           4312L

//
// MessageId: ERROR_DATABASE_FAILURE
//
// MessageText:
//
//  Unable to read from or write to the database.
//
#define ERROR_DATABASE_FAILURE           4313L

//
// MessageId: ERROR_DATABASE_FULL
//
// MessageText:
//
//  The database is full.
//
#define ERROR_DATABASE_FULL              4314L

//
// MessageId: ERROR_MEDIA_INCOMPATIBLE
//
// MessageText:
//
//  The medium is not compatible with the device or media pool.
//
#define ERROR_MEDIA_INCOMPATIBLE         4315L

//
// MessageId: ERROR_RESOURCE_NOT_PRESENT
//
// MessageText:
//
//  The resource required for this operation does not exist.
//
#define ERROR_RESOURCE_NOT_PRESENT       4316L

//
// MessageId: ERROR_INVALID_OPERATION
//
// MessageText:
//
//  The operation identifier is not valid.
//
#define ERROR_INVALID_OPERATION          4317L

//
// MessageId: ERROR_MEDIA_NOT_AVAILABLE
//
// MessageText:
//
//  The media is not mounted or ready for use.
//
#define ERROR_MEDIA_NOT_AVAILABLE        4318L

//
// MessageId: ERROR_DEVICE_NOT_AVAILABLE
//
// MessageText:
//
//  The device is not ready for use.
//
#define ERROR_DEVICE_NOT_AVAILABLE       4319L

//
// MessageId: ERROR_REQUEST_REFUSED
//
// MessageText:
//
//  The operator or administrator has refused the request.
//
#define ERROR_REQUEST_REFUSED            4320L

//
// MessageId: ERROR_INVALID_DRIVE_OBJECT
//
// MessageText:
//
//  The drive identifier does not represent a valid drive.
//
#define ERROR_INVALID_DRIVE_OBJECT       4321L

//
// MessageId: ERROR_LIBRARY_FULL
//
// MessageText:
//
//  Library is full.  No slot is available for use.
//
#define ERROR_LIBRARY_FULL               4322L

//
// MessageId: ERROR_MEDIUM_NOT_ACCESSIBLE
//
// MessageText:
//
//  The transport cannot access the medium.
//
#define ERROR_MEDIUM_NOT_ACCESSIBLE      4323L

//
// MessageId: ERROR_UNABLE_TO_LOAD_MEDIUM
//
// MessageText:
//
//  Unable to load the medium into the drive.
//
#define ERROR_UNABLE_TO_LOAD_MEDIUM      4324L

//
// MessageId: ERROR_UNABLE_TO_INVENTORY_DRIVE
//
// MessageText:
//
//  Unable to retrieve the drive status.
//
#define ERROR_UNABLE_TO_INVENTORY_DRIVE  4325L

//
// MessageId: ERROR_UNABLE_TO_INVENTORY_SLOT
//
// MessageText:
//
//  Unable to retrieve the slot status.
//
#define ERROR_UNABLE_TO_INVENTORY_SLOT   4326L

//
// MessageId: ERROR_UNABLE_TO_INVENTORY_TRANSPORT
//
// MessageText:
//
//  Unable to retrieve status about the transport.
//
#define ERROR_UNABLE_TO_INVENTORY_TRANSPORT 4327L

//
// MessageId: ERROR_TRANSPORT_FULL
//
// MessageText:
//
//  Cannot use the transport because it is already in use.
//
#define ERROR_TRANSPORT_FULL             4328L

//
// MessageId: ERROR_CONTROLLING_IEPORT
//
// MessageText:
//
//  Unable to open or close the inject/eject port.
//
#define ERROR_CONTROLLING_IEPORT         4329L

//
// MessageId: ERROR_UNABLE_TO_EJECT_MOUNTED_MEDIA
//
// MessageText:
//
//  Unable to eject the medium because it is in a drive.
//
#define ERROR_UNABLE_TO_EJECT_MOUNTED_MEDIA 4330L

//
// MessageId: ERROR_CLEANER_SLOT_SET
//
// MessageText:
//
//  A cleaner slot is already reserved.
//
#define ERROR_CLEANER_SLOT_SET           4331L

//
// MessageId: ERROR_CLEANER_SLOT_NOT_SET
//
// MessageText:
//
//  A cleaner slot is not reserved.
//
#define ERROR_CLEANER_SLOT_NOT_SET       4332L

//
// MessageId: ERROR_CLEANER_CARTRIDGE_SPENT
//
// MessageText:
//
//  The cleaner cartridge has performed the maximum number of drive cleanings.
//
#define ERROR_CLEANER_CARTRIDGE_SPENT    4333L

//
// MessageId: ERROR_UNEXPECTED_OMID
//
// MessageText:
//
//  Unexpected on-medium identifier.
//
#define ERROR_UNEXPECTED_OMID            4334L

//
// MessageId: ERROR_CANT_DELETE_LAST_ITEM
//
// MessageText:
//
//  The last remaining item in this group or resource cannot be deleted.
//
#define ERROR_CANT_DELETE_LAST_ITEM      4335L

//
// MessageId: ERROR_MESSAGE_EXCEEDS_MAX_SIZE
//
// MessageText:
//
//  The message provided exceeds the maximum size allowed for this parameter.
//
#define ERROR_MESSAGE_EXCEEDS_MAX_SIZE   4336L

//
// MessageId: ERROR_VOLUME_CONTAINS_SYS_FILES
//
// MessageText:
//
//  The volume contains system or paging files.
//
#define ERROR_VOLUME_CONTAINS_SYS_FILES  4337L

//
// MessageId: ERROR_INDIGENOUS_TYPE
//
// MessageText:
//
//  The media type cannot be removed from this library since at least one drive in the library reports it can support this media type.
//
#define ERROR_INDIGENOUS_TYPE            4338L

//
// MessageId: ERROR_NO_SUPPORTING_DRIVES
//
// MessageText:
//
//  This offline media cannot be mounted on this system since no enabled drives are present which can be used.
//
#define ERROR_NO_SUPPORTING_DRIVES       4339L

//
// MessageId: ERROR_CLEANER_CARTRIDGE_INSTALLED
//
// MessageText:
//
//  A cleaner cartridge is present in the tape library.
//
#define ERROR_CLEANER_CARTRIDGE_INSTALLED 4340L

//
// MessageId: ERROR_IEPORT_FULL
//
// MessageText:
//
//  Cannot use the ieport because it is not empty.
//
#define ERROR_IEPORT_FULL                4341L

////////////////////////////////////////////
//                                        //
// NT Remote Storage Service Error Codes  //
//                                        //
////////////////////////////////////////////
//
// MessageId: ERROR_FILE_OFFLINE
//
// MessageText:
//
//  The remote storage service was not able to recall the file.
//
#define ERROR_FILE_OFFLINE               4350L

//
// MessageId: ERROR_REMOTE_STORAGE_NOT_ACTIVE
//
// MessageText:
//
//  The remote storage service is not operational at this time.
//
#define ERROR_REMOTE_STORAGE_NOT_ACTIVE  4351L

//
// MessageId: ERROR_REMOTE_STORAGE_MEDIA_ERROR
//
// MessageText:
//
//  The remote storage service encountered a media error.
//
#define ERROR_REMOTE_STORAGE_MEDIA_ERROR 4352L

////////////////////////////////////////////
//                                        //
// NT Reparse Points Error Codes          //
//                                        //
////////////////////////////////////////////
//
// MessageId: ERROR_NOT_A_REPARSE_POINT
//
// MessageText:
//
//  The file or directory is not a reparse point.
//
#define ERROR_NOT_A_REPARSE_POINT        4390L

//
// MessageId: ERROR_REPARSE_ATTRIBUTE_CONFLICT
//
// MessageText:
//
//  The reparse point attribute cannot be set because it conflicts with an existing attribute.
//
#define ERROR_REPARSE_ATTRIBUTE_CONFLICT 4391L

//
// MessageId: ERROR_INVALID_REPARSE_DATA
//
// MessageText:
//
//  The data present in the reparse point buffer is invalid.
//
#define ERROR_INVALID_REPARSE_DATA       4392L

//
// MessageId: ERROR_REPARSE_TAG_INVALID
//
// MessageText:
//
//  The tag present in the reparse point buffer is invalid.
//
#define ERROR_REPARSE_TAG_INVALID        4393L

//
// MessageId: ERROR_REPARSE_TAG_MISMATCH
//
// MessageText:
//
//  There is a mismatch between the tag specified in the request and the tag present in the reparse point.
//  
//
#define ERROR_REPARSE_TAG_MISMATCH       4394L

////////////////////////////////////////////
//                                        //
// NT Single Instance Store Error Codes   //
//                                        //
////////////////////////////////////////////
//
// MessageId: ERROR_VOLUME_NOT_SIS_ENABLED
//
// MessageText:
//
//  Single Instance Storage is not available on this volume.
//
#define ERROR_VOLUME_NOT_SIS_ENABLED     4500L

////////////////////////////////////
//                                //
//     Cluster Error Codes        //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_DEPENDENT_RESOURCE_EXISTS
//
// MessageText:
//
//  The cluster resource cannot be moved to another group because other resources are dependent on it.
//
#define ERROR_DEPENDENT_RESOURCE_EXISTS  5001L

//
// MessageId: ERROR_DEPENDENCY_NOT_FOUND
//
// MessageText:
//
//  The cluster resource dependency cannot be found.
//
#define ERROR_DEPENDENCY_NOT_FOUND       5002L

//
// MessageId: ERROR_DEPENDENCY_ALREADY_EXISTS
//
// MessageText:
//
//  The cluster resource cannot be made dependent on the specified resource because it is already dependent.
//
#define ERROR_DEPENDENCY_ALREADY_EXISTS  5003L

//
// MessageId: ERROR_RESOURCE_NOT_ONLINE
//
// MessageText:
//
//  The cluster resource is not online.
//
#define ERROR_RESOURCE_NOT_ONLINE        5004L

//
// MessageId: ERROR_HOST_NODE_NOT_AVAILABLE
//
// MessageText:
//
//  A cluster node is not available for this operation.
//
#define ERROR_HOST_NODE_NOT_AVAILABLE    5005L

//
// MessageId: ERROR_RESOURCE_NOT_AVAILABLE
//
// MessageText:
//
//  The cluster resource is not available.
//
#define ERROR_RESOURCE_NOT_AVAILABLE     5006L

//
// MessageId: ERROR_RESOURCE_NOT_FOUND
//
// MessageText:
//
//  The cluster resource could not be found.
//
#define ERROR_RESOURCE_NOT_FOUND         5007L

//
// MessageId: ERROR_SHUTDOWN_CLUSTER
//
// MessageText:
//
//  The cluster is being shut down.
//
#define ERROR_SHUTDOWN_CLUSTER           5008L

//
// MessageId: ERROR_CANT_EVICT_ACTIVE_NODE
//
// MessageText:
//
//  A cluster node cannot be evicted from the cluster unless the node is down or it is the last node.
//
#define ERROR_CANT_EVICT_ACTIVE_NODE     5009L

//
// MessageId: ERROR_OBJECT_ALREADY_EXISTS
//
// MessageText:
//
//  The object already exists.
//
#define ERROR_OBJECT_ALREADY_EXISTS      5010L

//
// MessageId: ERROR_OBJECT_IN_LIST
//
// MessageText:
//
//  The object is already in the list.
//
#define ERROR_OBJECT_IN_LIST             5011L

//
// MessageId: ERROR_GROUP_NOT_AVAILABLE
//
// MessageText:
//
//  The cluster group is not available for any new requests.
//
#define ERROR_GROUP_NOT_AVAILABLE        5012L

//
// MessageId: ERROR_GROUP_NOT_FOUND
//
// MessageText:
//
//  The cluster group could not be found.
//
#define ERROR_GROUP_NOT_FOUND            5013L

//
// MessageId: ERROR_GROUP_NOT_ONLINE
//
// MessageText:
//
//  The operation could not be completed because the cluster group is not online.
//
#define ERROR_GROUP_NOT_ONLINE           5014L

//
// MessageId: ERROR_HOST_NODE_NOT_RESOURCE_OWNER
//
// MessageText:
//
//  The cluster node is not the owner of the resource.
//
#define ERROR_HOST_NODE_NOT_RESOURCE_OWNER 5015L

//
// MessageId: ERROR_HOST_NODE_NOT_GROUP_OWNER
//
// MessageText:
//
//  The cluster node is not the owner of the group.
//
#define ERROR_HOST_NODE_NOT_GROUP_OWNER  5016L

//
// MessageId: ERROR_RESMON_CREATE_FAILED
//
// MessageText:
//
//  The cluster resource could not be created in the specified resource monitor.
//
#define ERROR_RESMON_CREATE_FAILED       5017L

//
// MessageId: ERROR_RESMON_ONLINE_FAILED
//
// MessageText:
//
//  The cluster resource could not be brought online by the resource monitor.
//
#define ERROR_RESMON_ONLINE_FAILED       5018L

//
// MessageId: ERROR_RESOURCE_ONLINE
//
// MessageText:
//
//  The operation could not be completed because the cluster resource is online.
//
#define ERROR_RESOURCE_ONLINE            5019L

//
// MessageId: ERROR_QUORUM_RESOURCE
//
// MessageText:
//
//  The cluster resource could not be deleted or brought offline because it is the quorum resource.
//
#define ERROR_QUORUM_RESOURCE            5020L

//
// MessageId: ERROR_NOT_QUORUM_CAPABLE
//
// MessageText:
//
//  The cluster could not make the specified resource a quorum resource because it is not capable of being a quorum resource.
//
#define ERROR_NOT_QUORUM_CAPABLE         5021L

//
// MessageId: ERROR_CLUSTER_SHUTTING_DOWN
//
// MessageText:
//
//  The cluster software is shutting down.
//
#define ERROR_CLUSTER_SHUTTING_DOWN      5022L

//
// MessageId: ERROR_INVALID_STATE
//
// MessageText:
//
//  The group or resource is not in the correct state to perform the requested operation.
//
#define ERROR_INVALID_STATE              5023L

//
// MessageId: ERROR_RESOURCE_PROPERTIES_STORED
//
// MessageText:
//
//  The properties were stored but not all changes will take effect until the next time the resource is brought online.
//
#define ERROR_RESOURCE_PROPERTIES_STORED 5024L

//
// MessageId: ERROR_NOT_QUORUM_CLASS
//
// MessageText:
//
//  The cluster could not make the specified resource a quorum resource because it does not belong to a shared storage class.
//
#define ERROR_NOT_QUORUM_CLASS           5025L

//
// MessageId: ERROR_CORE_RESOURCE
//
// MessageText:
//
//  The cluster resource could not be deleted since it is a core resource.
//
#define ERROR_CORE_RESOURCE              5026L

//
// MessageId: ERROR_QUORUM_RESOURCE_ONLINE_FAILED
//
// MessageText:
//
//  The quorum resource failed to come online.
//
#define ERROR_QUORUM_RESOURCE_ONLINE_FAILED 5027L

//
// MessageId: ERROR_QUORUMLOG_OPEN_FAILED
//
// MessageText:
//
//  The quorum log could not be created or mounted successfully.
//
#define ERROR_QUORUMLOG_OPEN_FAILED      5028L

//
// MessageId: ERROR_CLUSTERLOG_CORRUPT
//
// MessageText:
//
//  The cluster log is corrupt.
//
#define ERROR_CLUSTERLOG_CORRUPT         5029L

//
// MessageId: ERROR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE
//
// MessageText:
//
//  The record could not be written to the cluster log since it exceeds the maximum size.
//
#define ERROR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE 5030L

//
// MessageId: ERROR_CLUSTERLOG_EXCEEDS_MAXSIZE
//
// MessageText:
//
//  The cluster log exceeds its maximum size.
//
#define ERROR_CLUSTERLOG_EXCEEDS_MAXSIZE 5031L

//
// MessageId: ERROR_CLUSTERLOG_CHKPOINT_NOT_FOUND
//
// MessageText:
//
//  No checkpoint record was found in the cluster log.
//
#define ERROR_CLUSTERLOG_CHKPOINT_NOT_FOUND 5032L

//
// MessageId: ERROR_CLUSTERLOG_NOT_ENOUGH_SPACE
//
// MessageText:
//
//  The minimum required disk space needed for logging is not available.
//
#define ERROR_CLUSTERLOG_NOT_ENOUGH_SPACE 5033L

//
// MessageId: ERROR_QUORUM_OWNER_ALIVE
//
// MessageText:
//
//  The cluster node failed to take control of the quorum resource because the resource is owned by another active node.
//
#define ERROR_QUORUM_OWNER_ALIVE         5034L

//
// MessageId: ERROR_NETWORK_NOT_AVAILABLE
//
// MessageText:
//
//  A cluster network is not available for this operation.
//
#define ERROR_NETWORK_NOT_AVAILABLE      5035L

//
// MessageId: ERROR_NODE_NOT_AVAILABLE
//
// MessageText:
//
//  A cluster node is not available for this operation.
//
#define ERROR_NODE_NOT_AVAILABLE         5036L

//
// MessageId: ERROR_ALL_NODES_NOT_AVAILABLE
//
// MessageText:
//
//  All cluster nodes must be running to perform this operation.
//
#define ERROR_ALL_NODES_NOT_AVAILABLE    5037L

//
// MessageId: ERROR_RESOURCE_FAILED
//
// MessageText:
//
//  A cluster resource failed.
//
#define ERROR_RESOURCE_FAILED            5038L

//
// MessageId: ERROR_CLUSTER_INVALID_NODE
//
// MessageText:
//
//  The cluster node is not valid.
//
#define ERROR_CLUSTER_INVALID_NODE       5039L

//
// MessageId: ERROR_CLUSTER_NODE_EXISTS
//
// MessageText:
//
//  The cluster node already exists.
//
#define ERROR_CLUSTER_NODE_EXISTS        5040L

//
// MessageId: ERROR_CLUSTER_JOIN_IN_PROGRESS
//
// MessageText:
//
//  A node is in the process of joining the cluster.
//
#define ERROR_CLUSTER_JOIN_IN_PROGRESS   5041L

//
// MessageId: ERROR_CLUSTER_NODE_NOT_FOUND
//
// MessageText:
//
//  The cluster node was not found.
//
#define ERROR_CLUSTER_NODE_NOT_FOUND     5042L

//
// MessageId: ERROR_CLUSTER_LOCAL_NODE_NOT_FOUND
//
// MessageText:
//
//  The cluster local node information was not found.
//
#define ERROR_CLUSTER_LOCAL_NODE_NOT_FOUND 5043L

//
// MessageId: ERROR_CLUSTER_NETWORK_EXISTS
//
// MessageText:
//
//  The cluster network already exists.
//
#define ERROR_CLUSTER_NETWORK_EXISTS     5044L

//
// MessageId: ERROR_CLUSTER_NETWORK_NOT_FOUND
//
// MessageText:
//
//  The cluster network was not found.
//
#define ERROR_CLUSTER_NETWORK_NOT_FOUND  5045L

//
// MessageId: ERROR_CLUSTER_NETINTERFACE_EXISTS
//
// MessageText:
//
//  The cluster network interface already exists.
//
#define ERROR_CLUSTER_NETINTERFACE_EXISTS 5046L

//
// MessageId: ERROR_CLUSTER_NETINTERFACE_NOT_FOUND
//
// MessageText:
//
//  The cluster network interface was not found.
//
#define ERROR_CLUSTER_NETINTERFACE_NOT_FOUND 5047L

//
// MessageId: ERROR_CLUSTER_INVALID_REQUEST
//
// MessageText:
//
//  The cluster request is not valid for this object.
//
#define ERROR_CLUSTER_INVALID_REQUEST    5048L

//
// MessageId: ERROR_CLUSTER_INVALID_NETWORK_PROVIDER
//
// MessageText:
//
//  The cluster network provider is not valid.
//
#define ERROR_CLUSTER_INVALID_NETWORK_PROVIDER 5049L

//
// MessageId: ERROR_CLUSTER_NODE_DOWN
//
// MessageText:
//
//  The cluster node is down.
//
#define ERROR_CLUSTER_NODE_DOWN          5050L

//
// MessageId: ERROR_CLUSTER_NODE_UNREACHABLE
//
// MessageText:
//
//  The cluster node is not reachable.
//
#define ERROR_CLUSTER_NODE_UNREACHABLE   5051L

//
// MessageId: ERROR_CLUSTER_NODE_NOT_MEMBER
//
// MessageText:
//
//  The cluster node is not a member of the cluster.
//
#define ERROR_CLUSTER_NODE_NOT_MEMBER    5052L

//
// MessageId: ERROR_CLUSTER_JOIN_NOT_IN_PROGRESS
//
// MessageText:
//
//  A cluster join operation is not in progress.
//
#define ERROR_CLUSTER_JOIN_NOT_IN_PROGRESS 5053L

//
// MessageId: ERROR_CLUSTER_INVALID_NETWORK
//
// MessageText:
//
//  The cluster network is not valid.
//
#define ERROR_CLUSTER_INVALID_NETWORK    5054L

//
// MessageId: ERROR_CLUSTER_NODE_UP
//
// MessageText:
//
//  The cluster node is up.
//
#define ERROR_CLUSTER_NODE_UP            5056L

//
// MessageId: ERROR_CLUSTER_IPADDR_IN_USE
//
// MessageText:
//
//  The cluster IP address is already in use.
//
#define ERROR_CLUSTER_IPADDR_IN_USE      5057L

//
// MessageId: ERROR_CLUSTER_NODE_NOT_PAUSED
//
// MessageText:
//
//  The cluster node is not paused.
//
#define ERROR_CLUSTER_NODE_NOT_PAUSED    5058L

//
// MessageId: ERROR_CLUSTER_NO_SECURITY_CONTEXT
//
// MessageText:
//
//  No cluster security context is available.
//
#define ERROR_CLUSTER_NO_SECURITY_CONTEXT 5059L

//
// MessageId: ERROR_CLUSTER_NETWORK_NOT_INTERNAL
//
// MessageText:
//
//  The cluster network is not configured for internal cluster communication.
//
#define ERROR_CLUSTER_NETWORK_NOT_INTERNAL 5060L

//
// MessageId: ERROR_CLUSTER_NODE_ALREADY_UP
//
// MessageText:
//
//  The cluster node is already up.
//
#define ERROR_CLUSTER_NODE_ALREADY_UP    5061L

//
// MessageId: ERROR_CLUSTER_NODE_ALREADY_DOWN
//
// MessageText:
//
//  The cluster node is already down.
//
#define ERROR_CLUSTER_NODE_ALREADY_DOWN  5062L

//
// MessageId: ERROR_CLUSTER_NETWORK_ALREADY_ONLINE
//
// MessageText:
//
//  The cluster network is already online.
//
#define ERROR_CLUSTER_NETWORK_ALREADY_ONLINE 5063L

//
// MessageId: ERROR_CLUSTER_NETWORK_ALREADY_OFFLINE
//
// MessageText:
//
//  The cluster network is already offline.
//
#define ERROR_CLUSTER_NETWORK_ALREADY_OFFLINE 5064L

//
// MessageId: ERROR_CLUSTER_NODE_ALREADY_MEMBER
//
// MessageText:
//
//  The cluster node is already a member of the cluster.
//
#define ERROR_CLUSTER_NODE_ALREADY_MEMBER 5065L

//
// MessageId: ERROR_CLUSTER_LAST_INTERNAL_NETWORK
//
// MessageText:
//
//  The cluster network is the only one configured for internal cluster communication between two or more active cluster nodes. The internal communication capability cannot be removed from the network.
//
#define ERROR_CLUSTER_LAST_INTERNAL_NETWORK 5066L

//
// MessageId: ERROR_CLUSTER_NETWORK_HAS_DEPENDENTS
//
// MessageText:
//
//  One or more cluster resources depend on the network to provide service to clients. The client access capability cannot be removed from the network.
//
#define ERROR_CLUSTER_NETWORK_HAS_DEPENDENTS 5067L

//
// MessageId: ERROR_INVALID_OPERATION_ON_QUORUM
//
// MessageText:
//
//  This operation cannot be performed on the cluster resource as it the quorum resource. You may not bring the quorum resource offline or modify its possible owners list.
//
#define ERROR_INVALID_OPERATION_ON_QUORUM 5068L

//
// MessageId: ERROR_DEPENDENCY_NOT_ALLOWED
//
// MessageText:
//
//  The cluster quorum resource is not allowed to have any dependencies.
//
#define ERROR_DEPENDENCY_NOT_ALLOWED     5069L

//
// MessageId: ERROR_CLUSTER_NODE_PAUSED
//
// MessageText:
//
//  The cluster node is paused.
//
#define ERROR_CLUSTER_NODE_PAUSED        5070L

//
// MessageId: ERROR_NODE_CANT_HOST_RESOURCE
//
// MessageText:
//
//  The cluster resource cannot be brought online. The owner node cannot run this resource.
//
#define ERROR_NODE_CANT_HOST_RESOURCE    5071L

//
// MessageId: ERROR_CLUSTER_NODE_NOT_READY
//
// MessageText:
//
//  The cluster node is not ready to perform the requested operation.
//
#define ERROR_CLUSTER_NODE_NOT_READY     5072L

//
// MessageId: ERROR_CLUSTER_NODE_SHUTTING_DOWN
//
// MessageText:
//
//  The cluster node is shutting down.
//
#define ERROR_CLUSTER_NODE_SHUTTING_DOWN 5073L

//
// MessageId: ERROR_CLUSTER_JOIN_ABORTED
//
// MessageText:
//
//  The cluster join operation was aborted.
//
#define ERROR_CLUSTER_JOIN_ABORTED       5074L

//
// MessageId: ERROR_CLUSTER_INCOMPATIBLE_VERSIONS
//
// MessageText:
//
//  The cluster join operation failed due to incompatible software versions between the joining node and its sponsor.
//
#define ERROR_CLUSTER_INCOMPATIBLE_VERSIONS 5075L

//
// MessageId: ERROR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED
//
// MessageText:
//
//  This resource cannot be created because the cluster has reached the limit on the number of resources it can monitor.
//
#define ERROR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED 5076L

//
// MessageId: ERROR_CLUSTER_SYSTEM_CONFIG_CHANGED
//
// MessageText:
//
//  The system configuration changed during the cluster join or form operation. The join or form operation was aborted.
//
#define ERROR_CLUSTER_SYSTEM_CONFIG_CHANGED 5077L

//
// MessageId: ERROR_CLUSTER_RESOURCE_TYPE_NOT_FOUND
//
// MessageText:
//
//  The specified resource type was not found.
//
#define ERROR_CLUSTER_RESOURCE_TYPE_NOT_FOUND 5078L

//
// MessageId: ERROR_CLUSTER_RESTYPE_NOT_SUPPORTED
//
// MessageText:
//
//  The specified node does not support a resource of this type.  This may be due to version inconsistencies or due to the absence of the resource DLL on this node.
//
#define ERROR_CLUSTER_RESTYPE_NOT_SUPPORTED 5079L

//
// MessageId: ERROR_CLUSTER_RESNAME_NOT_FOUND
//
// MessageText:
//
//  The specified resource name is not supported by this resource DLL. This may be due to a bad (or changed) name supplied to the resource DLL.
//
#define ERROR_CLUSTER_RESNAME_NOT_FOUND  5080L

//
// MessageId: ERROR_CLUSTER_NO_RPC_PACKAGES_REGISTERED
//
// MessageText:
//
//  No authentication package could be registered with the RPC server.
//
#define ERROR_CLUSTER_NO_RPC_PACKAGES_REGISTERED 5081L

//
// MessageId: ERROR_CLUSTER_OWNER_NOT_IN_PREFLIST
//
// MessageText:
//
//  You cannot bring the group online because the owner of the group is not in the preferred list for the group. To change the owner node for the group, move the group.
//
#define ERROR_CLUSTER_OWNER_NOT_IN_PREFLIST 5082L

//
// MessageId: ERROR_CLUSTER_DATABASE_SEQMISMATCH
//
// MessageText:
//
//  The join operation failed because the cluster database sequence number has changed or is incompatible with the locker node. This may happen during a join operation if the cluster database was changing during the join.
//
#define ERROR_CLUSTER_DATABASE_SEQMISMATCH 5083L

//
// MessageId: ERROR_RESMON_INVALID_STATE
//
// MessageText:
//
//  The resource monitor will not allow the fail operation to be performed while the resource is in its current state. This may happen if the resource is in a pending state.
//
#define ERROR_RESMON_INVALID_STATE       5084L

//
// MessageId: ERROR_CLUSTER_GUM_NOT_LOCKER
//
// MessageText:
//
//  A non locker code got a request to reserve the lock for making global updates.
//
#define ERROR_CLUSTER_GUM_NOT_LOCKER     5085L

//
// MessageId: ERROR_QUORUM_DISK_NOT_FOUND
//
// MessageText:
//
//  The quorum disk could not be located by the cluster service.
//
#define ERROR_QUORUM_DISK_NOT_FOUND      5086L

//
// MessageId: ERROR_DATABASE_BACKUP_CORRUPT
//
// MessageText:
//
//  The backed up cluster database is possibly corrupt.
//
#define ERROR_DATABASE_BACKUP_CORRUPT    5087L

//
// MessageId: ERROR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT
//
// MessageText:
//
//  A DFS root already exists in this cluster node.
//
#define ERROR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT 5088L

//
// MessageId: ERROR_RESOURCE_PROPERTY_UNCHANGEABLE
//
// MessageText:
//
//  An attempt to modify a resource property failed because it conflicts with another existing property.
//
#define ERROR_RESOURCE_PROPERTY_UNCHANGEABLE 5089L

/*
 Codes from 4300 through 5889 overlap with codes in ds\published\inc\apperr2.w.
 Do not add any more error codes in that range.
*/
//
// MessageId: ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE
//
// MessageText:
//
//  An operation was attempted that is incompatible with the current membership state of the node.
//
#define ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE 5890L

//
// MessageId: ERROR_CLUSTER_QUORUMLOG_NOT_FOUND
//
// MessageText:
//
//  The quorum resource does not contain the quorum log.
//
#define ERROR_CLUSTER_QUORUMLOG_NOT_FOUND 5891L

//
// MessageId: ERROR_CLUSTER_MEMBERSHIP_HALT
//
// MessageText:
//
//  The membership engine requested shutdown of the cluster service on this node.
//
#define ERROR_CLUSTER_MEMBERSHIP_HALT    5892L

//
// MessageId: ERROR_CLUSTER_INSTANCE_ID_MISMATCH
//
// MessageText:
//
//  The join operation failed because the cluster instance ID of the joining node does not match the cluster instance ID of the sponsor node.
//
#define ERROR_CLUSTER_INSTANCE_ID_MISMATCH 5893L

//
// MessageId: ERROR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP
//
// MessageText:
//
//  A matching network for the specified IP address could not be found. Please also specify a subnet mask and a cluster network.
//
#define ERROR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP 5894L

//
// MessageId: ERROR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH
//
// MessageText:
//
//  The actual data type of the property did not match the expected data type of the property.
//
#define ERROR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH 5895L

//
// MessageId: ERROR_CLUSTER_EVICT_WITHOUT_CLEANUP
//
// MessageText:
//
//  The cluster node was evicted from the cluster successfully, but the node was not cleaned up.  Extended status information explaining why the node was not cleaned up is available.
//
#define ERROR_CLUSTER_EVICT_WITHOUT_CLEANUP 5896L

//
// MessageId: ERROR_CLUSTER_PARAMETER_MISMATCH
//
// MessageText:
//
//  Two or more parameter values specified for a resource's properties are in conflict.
//
#define ERROR_CLUSTER_PARAMETER_MISMATCH 5897L

//
// MessageId: ERROR_NODE_CANNOT_BE_CLUSTERED
//
// MessageText:
//
//  This computer cannot be made a member of a cluster.
//
#define ERROR_NODE_CANNOT_BE_CLUSTERED   5898L

//
// MessageId: ERROR_CLUSTER_WRONG_OS_VERSION
//
// MessageText:
//
//  This computer cannot be made a member of a cluster because it does not have the correct version of Windows installed.
//
#define ERROR_CLUSTER_WRONG_OS_VERSION   5899L

//
// MessageId: ERROR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME
//
// MessageText:
//
//  A cluster cannot be created with the specified cluster name because that cluster name is already in use. Specify a different name for the cluster.
//
#define ERROR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME 5900L

//
// MessageId: ERROR_CLUSCFG_ALREADY_COMMITTED
//
// MessageText:
//
//  The cluster configuration action has already been committed.
//
#define ERROR_CLUSCFG_ALREADY_COMMITTED  5901L

//
// MessageId: ERROR_CLUSCFG_ROLLBACK_FAILED
//
// MessageText:
//
//  The cluster configuration action could not be rolled back.
//
#define ERROR_CLUSCFG_ROLLBACK_FAILED    5902L

//
// MessageId: ERROR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT
//
// MessageText:
//
//  The drive letter assigned to a system disk on one node conflicted with the drive letter assigned to a disk on another node.
//
#define ERROR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT 5903L

//
// MessageId: ERROR_CLUSTER_OLD_VERSION
//
// MessageText:
//
//  One or more nodes in the cluster are running a version of Windows that does not support this operation.
//
#define ERROR_CLUSTER_OLD_VERSION        5904L

//
// MessageId: ERROR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME
//
// MessageText:
//
//  The name of the corresponding computer account doesn't match the Network Name for this resource.
//
#define ERROR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME 5905L

////////////////////////////////////
//                                //
//     EFS Error Codes            //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_ENCRYPTION_FAILED
//
// MessageText:
//
//  The specified file could not be encrypted.
//
#define ERROR_ENCRYPTION_FAILED          6000L

//
// MessageId: ERROR_DECRYPTION_FAILED
//
// MessageText:
//
//  The specified file could not be decrypted.
//
#define ERROR_DECRYPTION_FAILED          6001L

//
// MessageId: ERROR_FILE_ENCRYPTED
//
// MessageText:
//
//  The specified file is encrypted and the user does not have the ability to decrypt it.
//
#define ERROR_FILE_ENCRYPTED             6002L

//
// MessageId: ERROR_NO_RECOVERY_POLICY
//
// MessageText:
//
//  There is no valid encryption recovery policy configured for this system.
//
#define ERROR_NO_RECOVERY_POLICY         6003L

//
// MessageId: ERROR_NO_EFS
//
// MessageText:
//
//  The required encryption driver is not loaded for this system.
//
#define ERROR_NO_EFS                     6004L

//
// MessageId: ERROR_WRONG_EFS
//
// MessageText:
//
//  The file was encrypted with a different encryption driver than is currently loaded.
//
#define ERROR_WRONG_EFS                  6005L

//
// MessageId: ERROR_NO_USER_KEYS
//
// MessageText:
//
//  There are no EFS keys defined for the user.
//
#define ERROR_NO_USER_KEYS               6006L

//
// MessageId: ERROR_FILE_NOT_ENCRYPTED
//
// MessageText:
//
//  The specified file is not encrypted.
//
#define ERROR_FILE_NOT_ENCRYPTED         6007L

//
// MessageId: ERROR_NOT_EXPORT_FORMAT
//
// MessageText:
//
//  The specified file is not in the defined EFS export format.
//
#define ERROR_NOT_EXPORT_FORMAT          6008L

//
// MessageId: ERROR_FILE_READ_ONLY
//
// MessageText:
//
//  The specified file is read only.
//
#define ERROR_FILE_READ_ONLY             6009L

//
// MessageId: ERROR_DIR_EFS_DISALLOWED
//
// MessageText:
//
//  The directory has been disabled for encryption.
//
#define ERROR_DIR_EFS_DISALLOWED         6010L

//
// MessageId: ERROR_EFS_SERVER_NOT_TRUSTED
//
// MessageText:
//
//  The server is not trusted for remote encryption operation.
//
#define ERROR_EFS_SERVER_NOT_TRUSTED     6011L

//
// MessageId: ERROR_BAD_RECOVERY_POLICY
//
// MessageText:
//
//  Recovery policy configured for this system contains invalid recovery certificate.
//
#define ERROR_BAD_RECOVERY_POLICY        6012L

//
// MessageId: ERROR_EFS_ALG_BLOB_TOO_BIG
//
// MessageText:
//
//  The encryption algorithm used on the source file needs a bigger key buffer than the one on the destination file.
//
#define ERROR_EFS_ALG_BLOB_TOO_BIG       6013L

//
// MessageId: ERROR_VOLUME_NOT_SUPPORT_EFS
//
// MessageText:
//
//  The disk partition does not support file encryption.
//
#define ERROR_VOLUME_NOT_SUPPORT_EFS     6014L

//
// MessageId: ERROR_EFS_DISABLED
//
// MessageText:
//
//  This machine is disabled for file encryption.
//
#define ERROR_EFS_DISABLED               6015L

//
// MessageId: ERROR_EFS_VERSION_NOT_SUPPORT
//
// MessageText:
//
//  A newer system is required to decrypt this encrypted file.
//
#define ERROR_EFS_VERSION_NOT_SUPPORT    6016L

// This message number is for historical purposes and cannot be changed or re-used.
//
// MessageId: ERROR_NO_BROWSER_SERVERS_FOUND
//
// MessageText:
//
//  The list of servers for this workgroup is not currently available
//
#define ERROR_NO_BROWSER_SERVERS_FOUND   6118L

//////////////////////////////////////////////////////////////////
//                                                              //
// Task Scheduler Error Codes that NET START must understand    //
//                                                              //
//////////////////////////////////////////////////////////////////
//
// MessageId: SCHED_E_SERVICE_NOT_LOCALSYSTEM
//
// MessageText:
//
//  The Task Scheduler service must be configured to run in the System account to function properly.  Individual tasks may be configured to run in other accounts.
//
#define SCHED_E_SERVICE_NOT_LOCALSYSTEM  6200L

////////////////////////////////////
//                                //
// Terminal Server Error Codes    //
//                                //
////////////////////////////////////
//
// MessageId: ERROR_CTX_WINSTATION_NAME_INVALID
//
// MessageText:
//
//  The specified session name is invalid.
//
#define ERROR_CTX_WINSTATION_NAME_INVALID 7001L

//
// MessageId: ERROR_CTX_INVALID_PD
//
// MessageText:
//
//  The specified protocol driver is invalid.
//
#define ERROR_CTX_INVALID_PD             7002L

//
// MessageId: ERROR_CTX_PD_NOT_FOUND
//
// MessageText:
//
//  The specified protocol driver was not found in the system path.
//
#define ERROR_CTX_PD_NOT_FOUND           7003L

//
// MessageId: ERROR_CTX_WD_NOT_FOUND
//
// MessageText:
//
//  The specified terminal connection driver was not found in the system path.
//
#define ERROR_CTX_WD_NOT_FOUND           7004L

//
// MessageId: ERROR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY
//
// MessageText:
//
//  A registry key for event logging could not be created for this session.
//
#define ERROR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY 7005L

//
// MessageId: ERROR_CTX_SERVICE_NAME_COLLISION
//
// MessageText:
//
//  A service with the same name already exists on the system.
//
#define ERROR_CTX_SERVICE_NAME_COLLISION 7006L

//
// MessageId: ERROR_CTX_CLOSE_PENDING
//
// MessageText:
//
//  A close operation is pending on the session.
//
#define ERROR_CTX_CLOSE_PENDING          7007L

//
// MessageId: ERROR_CTX_NO_OUTBUF
//
// MessageText:
//
//  There are no free output buffers available.
//
#define ERROR_CTX_NO_OUTBUF              7008L

//
// MessageId: ERROR_CTX_MODEM_INF_NOT_FOUND
//
// MessageText:
//
//  The MODEM.INF file was not found.
//
#define ERROR_CTX_MODEM_INF_NOT_FOUND    7009L

//
// MessageId: ERROR_CTX_INVALID_MODEMNAME
//
// MessageText:
//
//  The modem name was not found in MODEM.INF.
//
#define ERROR_CTX_INVALID_MODEMNAME      7010L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_ERROR
//
// MessageText:
//
//  The modem did not accept the command sent to it. Verify that the configured modem name matches the attached modem.
//
#define ERROR_CTX_MODEM_RESPONSE_ERROR   7011L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_TIMEOUT
//
// MessageText:
//
//  The modem did not respond to the command sent to it. Verify that the modem is properly cabled and powered on.
//
#define ERROR_CTX_MODEM_RESPONSE_TIMEOUT 7012L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_NO_CARRIER
//
// MessageText:
//
//  Carrier detect has failed or carrier has been dropped due to disconnect.
//
#define ERROR_CTX_MODEM_RESPONSE_NO_CARRIER 7013L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_NO_DIALTONE
//
// MessageText:
//
//  Dial tone not detected within the required time. Verify that the phone cable is properly attached and functional.
//
#define ERROR_CTX_MODEM_RESPONSE_NO_DIALTONE 7014L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_BUSY
//
// MessageText:
//
//  Busy signal detected at remote site on callback.
//
#define ERROR_CTX_MODEM_RESPONSE_BUSY    7015L

//
// MessageId: ERROR_CTX_MODEM_RESPONSE_VOICE
//
// MessageText:
//
//  Voice detected at remote site on callback.
//
#define ERROR_CTX_MODEM_RESPONSE_VOICE   7016L

//
// MessageId: ERROR_CTX_TD_ERROR
//
// MessageText:
//
//  Transport driver error
//
#define ERROR_CTX_TD_ERROR               7017L

//
// MessageId: ERROR_CTX_WINSTATION_NOT_FOUND
//
// MessageText:
//
//  The specified session cannot be found.
//
#define ERROR_CTX_WINSTATION_NOT_FOUND   7022L

//
// MessageId: ERROR_CTX_WINSTATION_ALREADY_EXISTS
//
// MessageText:
//
//  The specified session name is already in use.
//
#define ERROR_CTX_WINSTATION_ALREADY_EXISTS 7023L

//
// MessageId: ERROR_CTX_WINSTATION_BUSY
//
// MessageText:
//
//  The requested operation cannot be completed because the terminal connection is currently busy processing a connect, disconnect, reset, or delete operation.
//
#define ERROR_CTX_WINSTATION_BUSY        7024L

//
// MessageId: ERROR_CTX_BAD_VIDEO_MODE
//
// MessageText:
//
//  An attempt has been made to connect to a session whose video mode is not supported by the current client.
//
#define ERROR_CTX_BAD_VIDEO_MODE         7025L

//
// MessageId: ERROR_CTX_GRAPHICS_INVALID
//
// MessageText:
//
//  The application attempted to enable DOS graphics mode.
//  DOS graphics mode is not supported.
//
#define ERROR_CTX_GRAPHICS_INVALID       7035L

//
// MessageId: ERROR_CTX_LOGON_DISABLED
//
// MessageText:
//
//  Your interactive logon privilege has been disabled.
//  Please contact your administrator.
//
#define ERROR_CTX_LOGON_DISABLED         7037L

//
// MessageId: ERROR_CTX_NOT_CONSOLE
//
// MessageText:
//
//  The requested operation can be performed only on the system console.
//  This is most often the result of a driver or system DLL requiring direct console access.
//
#define ERROR_CTX_NOT_CONSOLE            7038L

//
// MessageId: ERROR_CTX_CLIENT_QUERY_TIMEOUT
//
// MessageText:
//
//  The client failed to respond to the server connect message.
//
#define ERROR_CTX_CLIENT_QUERY_TIMEOUT   7040L

//
// MessageId: ERROR_CTX_CONSOLE_DISCONNECT
//
// MessageText:
//
//  Disconnecting the console session is not supported.
//
#define ERROR_CTX_CONSOLE_DISCONNECT     7041L

//
// MessageId: ERROR_CTX_CONSOLE_CONNECT
//
// MessageText:
//
//  Reconnecting a disconnected session to the console is not supported.
//
#define ERROR_CTX_CONSOLE_CONNECT        7042L

//
// MessageId: ERROR_CTX_SHADOW_DENIED
//
// MessageText:
//
//  The request to control another session remotely was denied.
//
#define ERROR_CTX_SHADOW_DENIED          7044L

//
// MessageId: ERROR_CTX_WINSTATION_ACCESS_DENIED
//
// MessageText:
//
//  The requested session access is denied.
//
#define ERROR_CTX_WINSTATION_ACCESS_DENIED 7045L

//
// MessageId: ERROR_CTX_INVALID_WD
//
// MessageText:
//
//  The specified terminal connection driver is invalid.
//
#define ERROR_CTX_INVALID_WD             7049L

//
// MessageId: ERROR_CTX_SHADOW_INVALID
//
// MessageText:
//
//  The requested session cannot be controlled remotely.
//  This may be because the session is disconnected or does not currently have a user logged on.
//
#define ERROR_CTX_SHADOW_INVALID         7050L

//
// MessageId: ERROR_CTX_SHADOW_DISABLED
//
// MessageText:
//
//  The requested session is not configured to allow remote control.
//
#define ERROR_CTX_SHADOW_DISABLED        7051L

//
// MessageId: ERROR_CTX_CLIENT_LICENSE_IN_USE
//
// MessageText:
//
//  Your request to connect to this Terminal Server has been rejected. Your Terminal Server client license number is currently being used by another user.
//  Please call your system administrator to obtain a unique license number.
//
#define ERROR_CTX_CLIENT_LICENSE_IN_USE  7052L

//
// MessageId: ERROR_CTX_CLIENT_LICENSE_NOT_SET
//
// MessageText:
//
//  Your request to connect to this Terminal Server has been rejected. Your Terminal Server client license number has not been entered for this copy of the Terminal Server client.
//  Please contact your system administrator.
//
#define ERROR_CTX_CLIENT_LICENSE_NOT_SET 7053L

//
// MessageId: ERROR_CTX_LICENSE_NOT_AVAILABLE
//
// MessageText:
//
//  The system has reached its licensed logon limit.
//  Please try again later.
//
#define ERROR_CTX_LICENSE_NOT_AVAILABLE  7054L

//
// MessageId: ERROR_CTX_LICENSE_CLIENT_INVALID
//
// MessageText:
//
//  The client you are using is not licensed to use this system.  Your logon request is denied.
//
#define ERROR_CTX_LICENSE_CLIENT_INVALID 7055L

//
// MessageId: ERROR_CTX_LICENSE_EXPIRED
//
// MessageText:
//
//  The system license has expired.  Your logon request is denied.
//
#define ERROR_CTX_LICENSE_EXPIRED        7056L

//
// MessageId: ERROR_CTX_SHADOW_NOT_RUNNING
//
// MessageText:
//
//  Remote control could not be terminated because the specified session is not currently being remotely controlled.
//
#define ERROR_CTX_SHADOW_NOT_RUNNING     7057L

//
// MessageId: ERROR_CTX_SHADOW_ENDED_BY_MODE_CHANGE
//
// MessageText:
//
//  The remote control of the console was terminated because the display mode was changed. Changing the display mode in a remote control session is not supported.
//
#define ERROR_CTX_SHADOW_ENDED_BY_MODE_CHANGE 7058L

//
// MessageId: ERROR_ACTIVATION_COUNT_EXCEEDED
//
// MessageText:
//
//  Activation has already been reset the maximum number of times for this installation. Your activation timer will not be cleared.
//
#define ERROR_ACTIVATION_COUNT_EXCEEDED  7059L

///////////////////////////////////////////////////
//                                                /
//             Traffic Control Error Codes        /
//                                                /
//                  7500 to  7999                 /
//                                                /
//         defined in: tcerror.h                  /
///////////////////////////////////////////////////
///////////////////////////////////////////////////
//                                                /
//             Active Directory Error Codes       /
//                                                /
//                  8000 to  8999                 /
///////////////////////////////////////////////////
// *****************
// FACILITY_FILE_REPLICATION_SERVICE
// *****************
//
// MessageId: FRS_ERR_INVALID_API_SEQUENCE
//
// MessageText:
//
//  The file replication service API was called incorrectly.
//
#define FRS_ERR_INVALID_API_SEQUENCE     8001L

//
// MessageId: FRS_ERR_STARTING_SERVICE
//
// MessageText:
//
//  The file replication service cannot be started.
//
#define FRS_ERR_STARTING_SERVICE         8002L

//
// MessageId: FRS_ERR_STOPPING_SERVICE
//
// MessageText:
//
//  The file replication service cannot be stopped.
//
#define FRS_ERR_STOPPING_SERVICE         8003L

//
// MessageId: FRS_ERR_INTERNAL_API
//
// MessageText:
//
//  The file replication service API terminated the request.
//  The event log may have more information.
//
#define FRS_ERR_INTERNAL_API             8004L

//
// MessageId: FRS_ERR_INTERNAL
//
// MessageText:
//
//  The file replication service terminated the request.
//  The event log may have more information.
//
#define FRS_ERR_INTERNAL                 8005L

//
// MessageId: FRS_ERR_SERVICE_COMM
//
// MessageText:
//
//  The file replication service cannot be contacted.
//  The event log may have more information.
//
#define FRS_ERR_SERVICE_COMM             8006L

//
// MessageId: FRS_ERR_INSUFFICIENT_PRIV
//
// MessageText:
//
//  The file replication service cannot satisfy the request because the user has insufficient privileges.
//  The event log may have more information.
//
#define FRS_ERR_INSUFFICIENT_PRIV        8007L

//
// MessageId: FRS_ERR_AUTHENTICATION
//
// MessageText:
//
//  The file replication service cannot satisfy the request because authenticated RPC is not available.
//  The event log may have more information.
//
#define FRS_ERR_AUTHENTICATION           8008L

//
// MessageId: FRS_ERR_PARENT_INSUFFICIENT_PRIV
//
// MessageText:
//
//  The file replication service cannot satisfy the request because the user has insufficient privileges on the domain controller.
//  The event log may have more information.
//
#define FRS_ERR_PARENT_INSUFFICIENT_PRIV 8009L

//
// MessageId: FRS_ERR_PARENT_AUTHENTICATION
//
// MessageText:
//
//  The file replication service cannot satisfy the request because authenticated RPC is not available on the domain controller.
//  The event log may have more information.
//
#define FRS_ERR_PARENT_AUTHENTICATION    8010L

//
// MessageId: FRS_ERR_CHILD_TO_PARENT_COMM
//
// MessageText:
//
//  The file replication service cannot communicate with the file replication service on the domain controller.
//  The event log may have more information.
//
#define FRS_ERR_CHILD_TO_PARENT_COMM     8011L

//
// MessageId: FRS_ERR_PARENT_TO_CHILD_COMM
//
// MessageText:
//
//  The file replication service on the domain controller cannot communicate with the file replication service on this computer.
//  The event log may have more information.
//
#define FRS_ERR_PARENT_TO_CHILD_COMM     8012L

//
// MessageId: FRS_ERR_SYSVOL_POPULATE
//
// MessageText:
//
//  The file replication service cannot populate the system volume because of an internal error.
//  The event log may have more information.
//
#define FRS_ERR_SYSVOL_POPULATE          8013L

//
// MessageId: FRS_ERR_SYSVOL_POPULATE_TIMEOUT
//
// MessageText:
//
//  The file replication service cannot populate the system volume because of an internal timeout.
//  The event log may have more information.
//
#define FRS_ERR_SYSVOL_POPULATE_TIMEOUT  8014L

//
// MessageId: FRS_ERR_SYSVOL_IS_BUSY
//
// MessageText:
//
//  The file replication service cannot process the request. The system volume is busy with a previous request.
//
#define FRS_ERR_SYSVOL_IS_BUSY           8015L

//
// MessageId: FRS_ERR_SYSVOL_DEMOTE
//
// MessageText:
//
//  The file replication service cannot stop replicating the system volume because of an internal error.
//  The event log may have more information.
//
#define FRS_ERR_SYSVOL_DEMOTE            8016L

//
// MessageId: FRS_ERR_INVALID_SERVICE_PARAMETER
//
// MessageText:
//
//  The file replication service detected an invalid parameter.
//
#define FRS_ERR_INVALID_SERVICE_PARAMETER 8017L

// *****************
// FACILITY DIRECTORY SERVICE
// *****************
#define DS_S_SUCCESS NO_ERROR
//
// MessageId: ERROR_DS_NOT_INSTALLED
//
// MessageText:
//
//  An error occurred while installing the directory service. For more information, see the event log.
//
#define ERROR_DS_NOT_INSTALLED           8200L

//
// MessageId: ERROR_DS_MEMBERSHIP_EVALUATED_LOCALLY
//
// MessageText:
//
//  The directory service evaluated group memberships locally.
//
#define ERROR_DS_MEMBERSHIP_EVALUATED_LOCALLY 8201L

//
// MessageId: ERROR_DS_NO_ATTRIBUTE_OR_VALUE
//
// MessageText:
//
//  The specified directory service attribute or value does not exist.
//
#define ERROR_DS_NO_ATTRIBUTE_OR_VALUE   8202L

//
// MessageId: ERROR_DS_INVALID_ATTRIBUTE_SYNTAX
//
// MessageText:
//
//  The attribute syntax specified to the directory service is invalid.
//
#define ERROR_DS_INVALID_ATTRIBUTE_SYNTAX 8203L

//
// MessageId: ERROR_DS_ATTRIBUTE_TYPE_UNDEFINED
//
// MessageText:
//
//  The attribute type specified to the directory service is not defined.
//
#define ERROR_DS_ATTRIBUTE_TYPE_UNDEFINED 8204L

//
// MessageId: ERROR_DS_ATTRIBUTE_OR_VALUE_EXISTS
//
// MessageText:
//
//  The specified directory service attribute or value already exists.
//
#define ERROR_DS_ATTRIBUTE_OR_VALUE_EXISTS 8205L

//
// MessageId: ERROR_DS_BUSY
//
// MessageText:
//
//  The directory service is busy.
//
#define ERROR_DS_BUSY                    8206L

//
// MessageId: ERROR_DS_UNAVAILABLE
//
// MessageText:
//
//  The directory service is unavailable.
//
#define ERROR_DS_UNAVAILABLE             8207L

//
// MessageId: ERROR_DS_NO_RIDS_ALLOCATED
//
// MessageText:
//
//  The directory service was unable to allocate a relative identifier.
//
#define ERROR_DS_NO_RIDS_ALLOCATED       8208L

//
// MessageId: ERROR_DS_NO_MORE_RIDS
//
// MessageText:
//
//  The directory service has exhausted the pool of relative identifiers.
//
#define ERROR_DS_NO_MORE_RIDS            8209L

//
// MessageId: ERROR_DS_INCORRECT_ROLE_OWNER
//
// MessageText:
//
//  The requested operation could not be performed because the directory service is not the master for that type of operation.
//
#define ERROR_DS_INCORRECT_ROLE_OWNER    8210L

//
// MessageId: ERROR_DS_RIDMGR_INIT_ERROR
//
// MessageText:
//
//  The directory service was unable to initialize the subsystem that allocates relative identifiers.
//
#define ERROR_DS_RIDMGR_INIT_ERROR       8211L

//
// MessageId: ERROR_DS_OBJ_CLASS_VIOLATION
//
// MessageText:
//
//  The requested operation did not satisfy one or more constraints associated with the class of the object.
//
#define ERROR_DS_OBJ_CLASS_VIOLATION     8212L

//
// MessageId: ERROR_DS_CANT_ON_NON_LEAF
//
// MessageText:
//
//  The directory service can perform the requested operation only on a leaf object.
//
#define ERROR_DS_CANT_ON_NON_LEAF        8213L

//
// MessageId: ERROR_DS_CANT_ON_RDN
//
// MessageText:
//
//  The directory service cannot perform the requested operation on the RDN attribute of an object.
//
#define ERROR_DS_CANT_ON_RDN             8214L

//
// MessageId: ERROR_DS_CANT_MOD_OBJ_CLASS
//
// MessageText:
//
//  The directory service detected an attempt to modify the object class of an object.
//
#define ERROR_DS_CANT_MOD_OBJ_CLASS      8215L

//
// MessageId: ERROR_DS_CROSS_DOM_MOVE_ERROR
//
// MessageText:
//
//  The requested cross-domain move operation could not be performed.
//
#define ERROR_DS_CROSS_DOM_MOVE_ERROR    8216L

//
// MessageId: ERROR_DS_GC_NOT_AVAILABLE
//
// MessageText:
//
//  Unable to contact the global catalog server.
//
#define ERROR_DS_GC_NOT_AVAILABLE        8217L

//
// MessageId: ERROR_SHARED_POLICY
//
// MessageText:
//
//  The policy object is shared and can only be modified at the root.
//
#define ERROR_SHARED_POLICY              8218L

//
// MessageId: ERROR_POLICY_OBJECT_NOT_FOUND
//
// MessageText:
//
//  The policy object does not exist.
//
#define ERROR_POLICY_OBJECT_NOT_FOUND    8219L

//
// MessageId: ERROR_POLICY_ONLY_IN_DS
//
// MessageText:
//
//  The requested policy information is only in the directory service.
//
#define ERROR_POLICY_ONLY_IN_DS          8220L

//
// MessageId: ERROR_PROMOTION_ACTIVE
//
// MessageText:
//
//  A domain controller promotion is currently active.
//
#define ERROR_PROMOTION_ACTIVE           8221L

//
// MessageId: ERROR_NO_PROMOTION_ACTIVE
//
// MessageText:
//
//  A domain controller promotion is not currently active
//
#define ERROR_NO_PROMOTION_ACTIVE        8222L

// 8223 unused
//
// MessageId: ERROR_DS_OPERATIONS_ERROR
//
// MessageText:
//
//  An operations error occurred.
//
#define ERROR_DS_OPERATIONS_ERROR        8224L

//
// MessageId: ERROR_DS_PROTOCOL_ERROR
//
// MessageText:
//
//  A protocol error occurred.
//
#define ERROR_DS_PROTOCOL_ERROR          8225L

//
// MessageId: ERROR_DS_TIMELIMIT_EXCEEDED
//
// MessageText:
//
//  The time limit for this request was exceeded.
//
#define ERROR_DS_TIMELIMIT_EXCEEDED      8226L

//
// MessageId: ERROR_DS_SIZELIMIT_EXCEEDED
//
// MessageText:
//
//  The size limit for this request was exceeded.
//
#define ERROR_DS_SIZELIMIT_EXCEEDED      8227L

//
// MessageId: ERROR_DS_ADMIN_LIMIT_EXCEEDED
//
// MessageText:
//
//  The administrative limit for this request was exceeded.
//
#define ERROR_DS_ADMIN_LIMIT_EXCEEDED    8228L

//
// MessageId: ERROR_DS_COMPARE_FALSE
//
// MessageText:
//
//  The compare response was false.
//
#define ERROR_DS_COMPARE_FALSE           8229L

//
// MessageId: ERROR_DS_COMPARE_TRUE
//
// MessageText:
//
//  The compare response was true.
//
#define ERROR_DS_COMPARE_TRUE            8230L

//
// MessageId: ERROR_DS_AUTH_METHOD_NOT_SUPPORTED
//
// MessageText:
//
//  The requested authentication method is not supported by the server.
//
#define ERROR_DS_AUTH_METHOD_NOT_SUPPORTED 8231L

//
// MessageId: ERROR_DS_STRONG_AUTH_REQUIRED
//
// MessageText:
//
//  A more secure authentication method is required for this server.
//
#define ERROR_DS_STRONG_AUTH_REQUIRED    8232L

//
// MessageId: ERROR_DS_INAPPROPRIATE_AUTH
//
// MessageText:
//
//  Inappropriate authentication.
//
#define ERROR_DS_INAPPROPRIATE_AUTH      8233L

//
// MessageId: ERROR_DS_AUTH_UNKNOWN
//
// MessageText:
//
//  The authentication mechanism is unknown.
//
#define ERROR_DS_AUTH_UNKNOWN            8234L

//
// MessageId: ERROR_DS_REFERRAL
//
// MessageText:
//
//  A referral was returned from the server.
//
#define ERROR_DS_REFERRAL                8235L

//
// MessageId: ERROR_DS_UNAVAILABLE_CRIT_EXTENSION
//
// MessageText:
//
//  The server does not support the requested critical extension.
//
#define ERROR_DS_UNAVAILABLE_CRIT_EXTENSION 8236L

//
// MessageId: ERROR_DS_CONFIDENTIALITY_REQUIRED
//
// MessageText:
//
//  This request requires a secure connection.
//
#define ERROR_DS_CONFIDENTIALITY_REQUIRED 8237L

//
// MessageId: ERROR_DS_INAPPROPRIATE_MATCHING
//
// MessageText:
//
//  Inappropriate matching.
//
#define ERROR_DS_INAPPROPRIATE_MATCHING  8238L

//
// MessageId: ERROR_DS_CONSTRAINT_VIOLATION
//
// MessageText:
//
//  A constraint violation occurred.
//
#define ERROR_DS_CONSTRAINT_VIOLATION    8239L

//
// MessageId: ERROR_DS_NO_SUCH_OBJECT
//
// MessageText:
//
//  There is no such object on the server.
//
#define ERROR_DS_NO_SUCH_OBJECT          8240L

//
// MessageId: ERROR_DS_ALIAS_PROBLEM
//
// MessageText:
//
//  There is an alias problem.
//
#define ERROR_DS_ALIAS_PROBLEM           8241L

//
// MessageId: ERROR_DS_INVALID_DN_SYNTAX
//
// MessageText:
//
//  An invalid dn syntax has been specified.
//
#define ERROR_DS_INVALID_DN_SYNTAX       8242L

//
// MessageId: ERROR_DS_IS_LEAF
//
// MessageText:
//
//  The object is a leaf object.
//
#define ERROR_DS_IS_LEAF                 8243L

//
// MessageId: ERROR_DS_ALIAS_DEREF_PROBLEM
//
// MessageText:
//
//  There is an alias dereferencing problem.
//
#define ERROR_DS_ALIAS_DEREF_PROBLEM     8244L

//
// MessageId: ERROR_DS_UNWILLING_TO_PERFORM
//
// MessageText:
//
//  The server is unwilling to process the request.
//
#define ERROR_DS_UNWILLING_TO_PERFORM    8245L

//
// MessageId: ERROR_DS_LOOP_DETECT
//
// MessageText:
//
//  A loop has been detected.
//
#define ERROR_DS_LOOP_DETECT             8246L

//
// MessageId: ERROR_DS_NAMING_VIOLATION
//
// MessageText:
//
//  There is a naming violation.
//
#define ERROR_DS_NAMING_VIOLATION        8247L

//
// MessageId: ERROR_DS_OBJECT_RESULTS_TOO_LARGE
//
// MessageText:
//
//  The result set is too large.
//
#define ERROR_DS_OBJECT_RESULTS_TOO_LARGE 8248L

//
// MessageId: ERROR_DS_AFFECTS_MULTIPLE_DSAS
//
// MessageText:
//
//  The operation affects multiple DSAs
//
#define ERROR_DS_AFFECTS_MULTIPLE_DSAS   8249L

//
// MessageId: ERROR_DS_SERVER_DOWN
//
// MessageText:
//
//  The server is not operational.
//
#define ERROR_DS_SERVER_DOWN             8250L

//
// MessageId: ERROR_DS_LOCAL_ERROR
//
// MessageText:
//
//  A local error has occurred.
//
#define ERROR_DS_LOCAL_ERROR             8251L

//
// MessageId: ERROR_DS_ENCODING_ERROR
//
// MessageText:
//
//  An encoding error has occurred.
//
#define ERROR_DS_ENCODING_ERROR          8252L

//
// MessageId: ERROR_DS_DECODING_ERROR
//
// MessageText:
//
//  A decoding error has occurred.
//
#define ERROR_DS_DECODING_ERROR          8253L

//
// MessageId: ERROR_DS_FILTER_UNKNOWN
//
// MessageText:
//
//  The search filter cannot be recognized.
//
#define ERROR_DS_FILTER_UNKNOWN          8254L

//
// MessageId: ERROR_DS_PARAM_ERROR
//
// MessageText:
//
//  One or more parameters are illegal.
//
#define ERROR_DS_PARAM_ERROR             8255L

//
// MessageId: ERROR_DS_NOT_SUPPORTED
//
// MessageText:
//
//  The specified method is not supported.
//
#define ERROR_DS_NOT_SUPPORTED           8256L

//
// MessageId: ERROR_DS_NO_RESULTS_RETURNED
//
// MessageText:
//
//  No results were returned.
//
#define ERROR_DS_NO_RESULTS_RETURNED     8257L

//
// MessageId: ERROR_DS_CONTROL_NOT_FOUND
//
// MessageText:
//
//  The specified control is not supported by the server.
//
#define ERROR_DS_CONTROL_NOT_FOUND       8258L

//
// MessageId: ERROR_DS_CLIENT_LOOP
//
// MessageText:
//
//  A referral loop was detected by the client.
//
#define ERROR_DS_CLIENT_LOOP             8259L

//
// MessageId: ERROR_DS_REFERRAL_LIMIT_EXCEEDED
//
// MessageText:
//
//  The preset referral limit was exceeded.
//
#define ERROR_DS_REFERRAL_LIMIT_EXCEEDED 8260L

//
// MessageId: ERROR_DS_SORT_CONTROL_MISSING
//
// MessageText:
//
//  The search requires a SORT control.
//
#define ERROR_DS_SORT_CONTROL_MISSING    8261L

//
// MessageId: ERROR_DS_OFFSET_RANGE_ERROR
//
// MessageText:
//
//  The search results exceed the offset range specified.
//
#define ERROR_DS_OFFSET_RANGE_ERROR      8262L

//
// MessageId: ERROR_DS_ROOT_MUST_BE_NC
//
// MessageText:
//
//  The root object must be the head of a naming context. The root object cannot have an instantiated parent.
//
#define ERROR_DS_ROOT_MUST_BE_NC         8301L

//
// MessageId: ERROR_DS_ADD_REPLICA_INHIBITED
//
// MessageText:
//
//  The add replica operation cannot be performed. The naming context must be writeable in order to create the replica.
//
#define ERROR_DS_ADD_REPLICA_INHIBITED   8302L

//
// MessageId: ERROR_DS_ATT_NOT_DEF_IN_SCHEMA
//
// MessageText:
//
//  A reference to an attribute that is not defined in the schema occurred.
//
#define ERROR_DS_ATT_NOT_DEF_IN_SCHEMA   8303L

//
// MessageId: ERROR_DS_MAX_OBJ_SIZE_EXCEEDED
//
// MessageText:
//
//  The maximum size of an object has been exceeded.
//
#define ERROR_DS_MAX_OBJ_SIZE_EXCEEDED   8304L

//
// MessageId: ERROR_DS_OBJ_STRING_NAME_EXISTS
//
// MessageText:
//
//  An attempt was made to add an object to the directory with a name that is already in use.
//
#define ERROR_DS_OBJ_STRING_NAME_EXISTS  8305L

//
// MessageId: ERROR_DS_NO_RDN_DEFINED_IN_SCHEMA
//
// MessageText:
//
//  An attempt was made to add an object of a class that does not have an RDN defined in the schema.
//
#define ERROR_DS_NO_RDN_DEFINED_IN_SCHEMA 8306L

//
// MessageId: ERROR_DS_RDN_DOESNT_MATCH_SCHEMA
//
// MessageText:
//
//  An attempt was made to add an object using an RDN that is not the RDN defined in the schema.
//
#define ERROR_DS_RDN_DOESNT_MATCH_SCHEMA 8307L

//
// MessageId: ERROR_DS_NO_REQUESTED_ATTS_FOUND
//
// MessageText:
//
//  None of the requested attributes were found on the objects.
//
#define ERROR_DS_NO_REQUESTED_ATTS_FOUND 8308L

//
// MessageId: ERROR_DS_USER_BUFFER_TO_SMALL
//
// MessageText:
//
//  The user buffer is too small.
//
#define ERROR_DS_USER_BUFFER_TO_SMALL    8309L

//
// MessageId: ERROR_DS_ATT_IS_NOT_ON_OBJ
//
// MessageText:
//
//  The attribute specified in the operation is not present on the object.
//
#define ERROR_DS_ATT_IS_NOT_ON_OBJ       8310L

//
// MessageId: ERROR_DS_ILLEGAL_MOD_OPERATION
//
// MessageText:
//
//  Illegal modify operation. Some aspect of the modification is not permitted.
//
#define ERROR_DS_ILLEGAL_MOD_OPERATION   8311L

//
// MessageId: ERROR_DS_OBJ_TOO_LARGE
//
// MessageText:
//
//  The specified object is too large.
//
#define ERROR_DS_OBJ_TOO_LARGE           8312L

//
// MessageId: ERROR_DS_BAD_INSTANCE_TYPE
//
// MessageText:
//
//  The specified instance type is not valid.
//
#define ERROR_DS_BAD_INSTANCE_TYPE       8313L

//
// MessageId: ERROR_DS_MASTERDSA_REQUIRED
//
// MessageText:
//
//  The operation must be performed at a master DSA.
//
#define ERROR_DS_MASTERDSA_REQUIRED      8314L

//
// MessageId: ERROR_DS_OBJECT_CLASS_REQUIRED
//
// MessageText:
//
//  The object class attribute must be specified.
//
#define ERROR_DS_OBJECT_CLASS_REQUIRED   8315L

//
// MessageId: ERROR_DS_MISSING_REQUIRED_ATT
//
// MessageText:
//
//  A required attribute is missing.
//
#define ERROR_DS_MISSING_REQUIRED_ATT    8316L

//
// MessageId: ERROR_DS_ATT_NOT_DEF_FOR_CLASS
//
// MessageText:
//
//  An attempt was made to modify an object to include an attribute that is not legal for its class.
//
#define ERROR_DS_ATT_NOT_DEF_FOR_CLASS   8317L

//
// MessageId: ERROR_DS_ATT_ALREADY_EXISTS
//
// MessageText:
//
//  The specified attribute is already present on the object.
//
#define ERROR_DS_ATT_ALREADY_EXISTS      8318L

// 8319 unused
//
// MessageId: ERROR_DS_CANT_ADD_ATT_VALUES
//
// MessageText:
//
//  The specified attribute is not present, or has no values.
//
#define ERROR_DS_CANT_ADD_ATT_VALUES     8320L

//
// MessageId: ERROR_DS_SINGLE_VALUE_CONSTRAINT
//
// MessageText:
//
//  Multiple values were specified for an attribute that can have only one value.
//
#define ERROR_DS_SINGLE_VALUE_CONSTRAINT 8321L

//
// MessageId: ERROR_DS_RANGE_CONSTRAINT
//
// MessageText:
//
//  A value for the attribute was not in the acceptable range of values.
//
#define ERROR_DS_RANGE_CONSTRAINT        8322L

//
// MessageId: ERROR_DS_ATT_VAL_ALREADY_EXISTS
//
// MessageText:
//
//  The specified value already exists.
//
#define ERROR_DS_ATT_VAL_ALREADY_EXISTS  8323L

//
// MessageId: ERROR_DS_CANT_REM_MISSING_ATT
//
// MessageText:
//
//  The attribute cannot be removed because it is not present on the object.
//
#define ERROR_DS_CANT_REM_MISSING_ATT    8324L

//
// MessageId: ERROR_DS_CANT_REM_MISSING_ATT_VAL
//
// MessageText:
//
//  The attribute value cannot be removed because it is not present on the object.
//
#define ERROR_DS_CANT_REM_MISSING_ATT_VAL 8325L

//
// MessageId: ERROR_DS_ROOT_CANT_BE_SUBREF
//
// MessageText:
//
//  The specified root object cannot be a subref.
//
#define ERROR_DS_ROOT_CANT_BE_SUBREF     8326L

//
// MessageId: ERROR_DS_NO_CHAINING
//
// MessageText:
//
//  Chaining is not permitted.
//
#define ERROR_DS_NO_CHAINING             8327L

//
// MessageId: ERROR_DS_NO_CHAINED_EVAL
//
// MessageText:
//
//  Chained evaluation is not permitted.
//
#define ERROR_DS_NO_CHAINED_EVAL         8328L

//
// MessageId: ERROR_DS_NO_PARENT_OBJECT
//
// MessageText:
//
//  The operation could not be performed because the object's parent is either uninstantiated or deleted.
//
#define ERROR_DS_NO_PARENT_OBJECT        8329L

//
// MessageId: ERROR_DS_PARENT_IS_AN_ALIAS
//
// MessageText:
//
//  Having a parent that is an alias is not permitted. Aliases are leaf objects.
//
#define ERROR_DS_PARENT_IS_AN_ALIAS      8330L

//
// MessageId: ERROR_DS_CANT_MIX_MASTER_AND_REPS
//
// MessageText:
//
//  The object and parent must be of the same type, either both masters or both replicas.
//
#define ERROR_DS_CANT_MIX_MASTER_AND_REPS 8331L

//
// MessageId: ERROR_DS_CHILDREN_EXIST
//
// MessageText:
//
//  The operation cannot be performed because child objects exist. This operation can only be performed on a leaf object.
//
#define ERROR_DS_CHILDREN_EXIST          8332L

//
// MessageId: ERROR_DS_OBJ_NOT_FOUND
//
// MessageText:
//
//  Directory object not found.
//
#define ERROR_DS_OBJ_NOT_FOUND           8333L

//
// MessageId: ERROR_DS_ALIASED_OBJ_MISSING
//
// MessageText:
//
//  The aliased object is missing.
//
#define ERROR_DS_ALIASED_OBJ_MISSING     8334L

//
// MessageId: ERROR_DS_BAD_NAME_SYNTAX
//
// MessageText:
//
//  The object name has bad syntax.
//
#define ERROR_DS_BAD_NAME_SYNTAX         8335L

//
// MessageId: ERROR_DS_ALIAS_POINTS_TO_ALIAS
//
// MessageText:
//
//  It is not permitted for an alias to refer to another alias.
//
#define ERROR_DS_ALIAS_POINTS_TO_ALIAS   8336L

//
// MessageId: ERROR_DS_CANT_DEREF_ALIAS
//
// MessageText:
//
//  The alias cannot be dereferenced.
//
#define ERROR_DS_CANT_DEREF_ALIAS        8337L

//
// MessageId: ERROR_DS_OUT_OF_SCOPE
//
// MessageText:
//
//  The operation is out of scope.
//
#define ERROR_DS_OUT_OF_SCOPE            8338L

//
// MessageId: ERROR_DS_OBJECT_BEING_REMOVED
//
// MessageText:
//
//  The operation cannot continue because the object is in the process of being removed.
//
#define ERROR_DS_OBJECT_BEING_REMOVED    8339L

//
// MessageId: ERROR_DS_CANT_DELETE_DSA_OBJ
//
// MessageText:
//
//  The DSA object cannot be deleted.
//
#define ERROR_DS_CANT_DELETE_DSA_OBJ     8340L

//
// MessageId: ERROR_DS_GENERIC_ERROR
//
// MessageText:
//
//  A directory service error has occurred.
//
#define ERROR_DS_GENERIC_ERROR           8341L

//
// MessageId: ERROR_DS_DSA_MUST_BE_INT_MASTER
//
// MessageText:
//
//  The operation can only be performed on an internal master DSA object.
//
#define ERROR_DS_DSA_MUST_BE_INT_MASTER  8342L

//
// MessageId: ERROR_DS_CLASS_NOT_DSA
//
// MessageText:
//
//  The object must be of class DSA.
//
#define ERROR_DS_CLASS_NOT_DSA           8343L

//
// MessageId: ERROR_DS_INSUFF_ACCESS_RIGHTS
//
// MessageText:
//
//  Insufficient access rights to perform the operation.
//
#define ERROR_DS_INSUFF_ACCESS_RIGHTS    8344L

//
// MessageId: ERROR_DS_ILLEGAL_SUPERIOR
//
// MessageText:
//
//  The object cannot be added because the parent is not on the list of possible superiors.
//
#define ERROR_DS_ILLEGAL_SUPERIOR        8345L

//
// MessageId: ERROR_DS_ATTRIBUTE_OWNED_BY_SAM
//
// MessageText:
//
//  Access to the attribute is not permitted because the attribute is owned by the Security Accounts Manager (SAM).
//
#define ERROR_DS_ATTRIBUTE_OWNED_BY_SAM  8346L

//
// MessageId: ERROR_DS_NAME_TOO_MANY_PARTS
//
// MessageText:
//
//  The name has too many parts.
//
#define ERROR_DS_NAME_TOO_MANY_PARTS     8347L

//
// MessageId: ERROR_DS_NAME_TOO_LONG
//
// MessageText:
//
//  The name is too long.
//
#define ERROR_DS_NAME_TOO_LONG           8348L

//
// MessageId: ERROR_DS_NAME_VALUE_TOO_LONG
//
// MessageText:
//
//  The name value is too long.
//
#define ERROR_DS_NAME_VALUE_TOO_LONG     8349L

//
// MessageId: ERROR_DS_NAME_UNPARSEABLE
//
// MessageText:
//
//  The directory service encountered an error parsing a name.
//
#define ERROR_DS_NAME_UNPARSEABLE        8350L

//
// MessageId: ERROR_DS_NAME_TYPE_UNKNOWN
//
// MessageText:
//
//  The directory service cannot get the attribute type for a name.
//
#define ERROR_DS_NAME_TYPE_UNKNOWN       8351L

//
// MessageId: ERROR_DS_NOT_AN_OBJECT
//
// MessageText:
//
//  The name does not identify an object; the name identifies a phantom.
//
#define ERROR_DS_NOT_AN_OBJECT           8352L

//
// MessageId: ERROR_DS_SEC_DESC_TOO_SHORT
//
// MessageText:
//
//  The security descriptor is too short.
//
#define ERROR_DS_SEC_DESC_TOO_SHORT      8353L

//
// MessageId: ERROR_DS_SEC_DESC_INVALID
//
// MessageText:
//
//  The security descriptor is invalid.
//
#define ERROR_DS_SEC_DESC_INVALID        8354L

//
// MessageId: ERROR_DS_NO_DELETED_NAME
//
// MessageText:
//
//  Failed to create name for deleted object.
//
#define ERROR_DS_NO_DELETED_NAME         8355L

//
// MessageId: ERROR_DS_SUBREF_MUST_HAVE_PARENT
//
// MessageText:
//
//  The parent of a new subref must exist.
//
#define ERROR_DS_SUBREF_MUST_HAVE_PARENT 8356L

//
// MessageId: ERROR_DS_NCNAME_MUST_BE_NC
//
// MessageText:
//
//  The object must be a naming context.
//
#define ERROR_DS_NCNAME_MUST_BE_NC       8357L

//
// MessageId: ERROR_DS_CANT_ADD_SYSTEM_ONLY
//
// MessageText:
//
//  It is not permitted to add an attribute which is owned by the system.
//
#define ERROR_DS_CANT_ADD_SYSTEM_ONLY    8358L

//
// MessageId: ERROR_DS_CLASS_MUST_BE_CONCRETE
//
// MessageText:
//
//  The class of the object must be structural; you cannot instantiate an abstract class.
//
#define ERROR_DS_CLASS_MUST_BE_CONCRETE  8359L

//
// MessageId: ERROR_DS_INVALID_DMD
//
// MessageText:
//
//  The schema object could not be found.
//
#define ERROR_DS_INVALID_DMD             8360L

//
// MessageId: ERROR_DS_OBJ_GUID_EXISTS
//
// MessageText:
//
//  A local object with this GUID (dead or alive) already exists.
//
#define ERROR_DS_OBJ_GUID_EXISTS         8361L

//
// MessageId: ERROR_DS_NOT_ON_BACKLINK
//
// MessageText:
//
//  The operation cannot be performed on a back link.
//
#define ERROR_DS_NOT_ON_BACKLINK         8362L

//
// MessageId: ERROR_DS_NO_CROSSREF_FOR_NC
//
// MessageText:
//
//  The cross reference for the specified naming context could not be found.
//
#define ERROR_DS_NO_CROSSREF_FOR_NC      8363L

//
// MessageId: ERROR_DS_SHUTTING_DOWN
//
// MessageText:
//
//  The operation could not be performed because the directory service is shutting down.
//
#define ERROR_DS_SHUTTING_DOWN           8364L

//
// MessageId: ERROR_DS_UNKNOWN_OPERATION
//
// MessageText:
//
//  The directory service request is invalid.
//
#define ERROR_DS_UNKNOWN_OPERATION       8365L

//
// MessageId: ERROR_DS_INVALID_ROLE_OWNER
//
// MessageText:
//
//  The role owner attribute could not be read.
//
#define ERROR_DS_INVALID_ROLE_OWNER      8366L

//
// MessageId: ERROR_DS_COULDNT_CONTACT_FSMO
//
// MessageText:
//
//  The requested FSMO operation failed. The current FSMO holder could not be contacted.
//
#define ERROR_DS_COULDNT_CONTACT_FSMO    8367L

//
// MessageId: ERROR_DS_CROSS_NC_DN_RENAME
//
// MessageText:
//
//  Modification of a DN across a naming context is not permitted.
//
#define ERROR_DS_CROSS_NC_DN_RENAME      8368L

//
// MessageId: ERROR_DS_CANT_MOD_SYSTEM_ONLY
//
// MessageText:
//
//  The attribute cannot be modified because it is owned by the system.
//
#define ERROR_DS_CANT_MOD_SYSTEM_ONLY    8369L

//
// MessageId: ERROR_DS_REPLICATOR_ONLY
//
// MessageText:
//
//  Only the replicator can perform this function.
//
#define ERROR_DS_REPLICATOR_ONLY         8370L

//
// MessageId: ERROR_DS_OBJ_CLASS_NOT_DEFINED
//
// MessageText:
//
//  The specified class is not defined.
//
#define ERROR_DS_OBJ_CLASS_NOT_DEFINED   8371L

//
// MessageId: ERROR_DS_OBJ_CLASS_NOT_SUBCLASS
//
// MessageText:
//
//  The specified class is not a subclass.
//
#define ERROR_DS_OBJ_CLASS_NOT_SUBCLASS  8372L

//
// MessageId: ERROR_DS_NAME_REFERENCE_INVALID
//
// MessageText:
//
//  The name reference is invalid.
//
#define ERROR_DS_NAME_REFERENCE_INVALID  8373L

//
// MessageId: ERROR_DS_CROSS_REF_EXISTS
//
// MessageText:
//
//  A cross reference already exists.
//
#define ERROR_DS_CROSS_REF_EXISTS        8374L

//
// MessageId: ERROR_DS_CANT_DEL_MASTER_CROSSREF
//
// MessageText:
//
//  It is not permitted to delete a master cross reference.
//
#define ERROR_DS_CANT_DEL_MASTER_CROSSREF 8375L

//
// MessageId: ERROR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD
//
// MessageText:
//
//  Subtree notifications are only supported on NC heads.
//
#define ERROR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD 8376L

//
// MessageId: ERROR_DS_NOTIFY_FILTER_TOO_COMPLEX
//
// MessageText:
//
//  Notification filter is too complex.
//
#define ERROR_DS_NOTIFY_FILTER_TOO_COMPLEX 8377L

//
// MessageId: ERROR_DS_DUP_RDN
//
// MessageText:
//
//  Schema update failed: duplicate RDN.
//
#define ERROR_DS_DUP_RDN                 8378L

//
// MessageId: ERROR_DS_DUP_OID
//
// MessageText:
//
//  Schema update failed: duplicate OID.
//
#define ERROR_DS_DUP_OID                 8379L

//
// MessageId: ERROR_DS_DUP_MAPI_ID
//
// MessageText:
//
//  Schema update failed: duplicate MAPI identifier.
//
#define ERROR_DS_DUP_MAPI_ID             8380L

//
// MessageId: ERROR_DS_DUP_SCHEMA_ID_GUID
//
// MessageText:
//
//  Schema update failed: duplicate schema-id GUID.
//
#define ERROR_DS_DUP_SCHEMA_ID_GUID      8381L

//
// MessageId: ERROR_DS_DUP_LDAP_DISPLAY_NAME
//
// MessageText:
//
//  Schema update failed: duplicate LDAP display name.
//
#define ERROR_DS_DUP_LDAP_DISPLAY_NAME   8382L

//
// MessageId: ERROR_DS_SEMANTIC_ATT_TEST
//
// MessageText:
//
//  Schema update failed: range-lower less than range upper.
//
#define ERROR_DS_SEMANTIC_ATT_TEST       8383L

//
// MessageId: ERROR_DS_SYNTAX_MISMATCH
//
// MessageText:
//
//  Schema update failed: syntax mismatch.
//
#define ERROR_DS_SYNTAX_MISMATCH         8384L

//
// MessageId: ERROR_DS_EXISTS_IN_MUST_HAVE
//
// MessageText:
//
//  Schema deletion failed: attribute is used in must-contain.
//
#define ERROR_DS_EXISTS_IN_MUST_HAVE     8385L

//
// MessageId: ERROR_DS_EXISTS_IN_MAY_HAVE
//
// MessageText:
//
//  Schema deletion failed: attribute is used in may-contain.
//
#define ERROR_DS_EXISTS_IN_MAY_HAVE      8386L

//
// MessageId: ERROR_DS_NONEXISTENT_MAY_HAVE
//
// MessageText:
//
//  Schema update failed: attribute in may-contain does not exist.
//
#define ERROR_DS_NONEXISTENT_MAY_HAVE    8387L

//
// MessageId: ERROR_DS_NONEXISTENT_MUST_HAVE
//
// MessageText:
//
//  Schema update failed: attribute in must-contain does not exist.
//
#define ERROR_DS_NONEXISTENT_MUST_HAVE   8388L

//
// MessageId: ERROR_DS_AUX_CLS_TEST_FAIL
//
// MessageText:
//
//  Schema update failed: class in aux-class list does not exist or is not an auxiliary class.
//
#define ERROR_DS_AUX_CLS_TEST_FAIL       8389L

//
// MessageId: ERROR_DS_NONEXISTENT_POSS_SUP
//
// MessageText:
//
//  Schema update failed: class in poss-superiors does not exist.
//
#define ERROR_DS_NONEXISTENT_POSS_SUP    8390L

//
// MessageId: ERROR_DS_SUB_CLS_TEST_FAIL
//
// MessageText:
//
//  Schema update failed: class in subclassof list does not exist or does not satisfy hierarchy rules.
//
#define ERROR_DS_SUB_CLS_TEST_FAIL       8391L

//
// MessageId: ERROR_DS_BAD_RDN_ATT_ID_SYNTAX
//
// MessageText:
//
//  Schema update failed: Rdn-Att-Id has wrong syntax.
//
#define ERROR_DS_BAD_RDN_ATT_ID_SYNTAX   8392L

//
// MessageId: ERROR_DS_EXISTS_IN_AUX_CLS
//
// MessageText:
//
//  Schema deletion failed: class is used as auxiliary class.
//
#define ERROR_DS_EXISTS_IN_AUX_CLS       8393L

//
// MessageId: ERROR_DS_EXISTS_IN_SUB_CLS
//
// MessageText:
//
//  Schema deletion failed: class is used as sub class.
//
#define ERROR_DS_EXISTS_IN_SUB_CLS       8394L

//
// MessageId: ERROR_DS_EXISTS_IN_POSS_SUP
//
// MessageText:
//
//  Schema deletion failed: class is used as poss superior.
//
#define ERROR_DS_EXISTS_IN_POSS_SUP      8395L

//
// MessageId: ERROR_DS_RECALCSCHEMA_FAILED
//
// MessageText:
//
//  Schema update failed in recalculating validation cache.
//
#define ERROR_DS_RECALCSCHEMA_FAILED     8396L

//
// MessageId: ERROR_DS_TREE_DELETE_NOT_FINISHED
//
// MessageText:
//
//  The tree deletion is not finished.  The request must be made again to continue deleting the tree.
//
#define ERROR_DS_TREE_DELETE_NOT_FINISHED 8397L

//
// MessageId: ERROR_DS_CANT_DELETE
//
// MessageText:
//
//  The requested delete operation could not be performed.
//
#define ERROR_DS_CANT_DELETE             8398L

//
// MessageId: ERROR_DS_ATT_SCHEMA_REQ_ID
//
// MessageText:
//
//  Cannot read the governs class identifier for the schema record.
//
#define ERROR_DS_ATT_SCHEMA_REQ_ID       8399L

//
// MessageId: ERROR_DS_BAD_ATT_SCHEMA_SYNTAX
//
// MessageText:
//
//  The attribute schema has bad syntax.
//
#define ERROR_DS_BAD_ATT_SCHEMA_SYNTAX   8400L

//
// MessageId: ERROR_DS_CANT_CACHE_ATT
//
// MessageText:
//
//  The attribute could not be cached.
//
#define ERROR_DS_CANT_CACHE_ATT          8401L

//
// MessageId: ERROR_DS_CANT_CACHE_CLASS
//
// MessageText:
//
//  The class could not be cached.
//
#define ERROR_DS_CANT_CACHE_CLASS        8402L

//
// MessageId: ERROR_DS_CANT_REMOVE_ATT_CACHE
//
// MessageText:
//
//  The attribute could not be removed from the cache.
//
#define ERROR_DS_CANT_REMOVE_ATT_CACHE   8403L

//
// MessageId: ERROR_DS_CANT_REMOVE_CLASS_CACHE
//
// MessageText:
//
//  The class could not be removed from the cache.
//
#define ERROR_DS_CANT_REMOVE_CLASS_CACHE 8404L

//
// MessageId: ERROR_DS_CANT_RETRIEVE_DN
//
// MessageText:
//
//  The distinguished name attribute could not be read.
//
#define ERROR_DS_CANT_RETRIEVE_DN        8405L

//
// MessageId: ERROR_DS_MISSING_SUPREF
//
// MessageText:
//
//  No superior reference has been configured for the directory service. The directory service is therefore unable to issue referrals to objects outside this forest.
//
#define ERROR_DS_MISSING_SUPREF          8406L

//
// MessageId: ERROR_DS_CANT_RETRIEVE_INSTANCE
//
// MessageText:
//
//  The instance type attribute could not be retrieved.
//
#define ERROR_DS_CANT_RETRIEVE_INSTANCE  8407L

//
// MessageId: ERROR_DS_CODE_INCONSISTENCY
//
// MessageText:
//
//  An internal error has occurred.
//
#define ERROR_DS_CODE_INCONSISTENCY      8408L

//
// MessageId: ERROR_DS_DATABASE_ERROR
//
// MessageText:
//
//  A database error has occurred.
//
#define ERROR_DS_DATABASE_ERROR          8409L

//
// MessageId: ERROR_DS_GOVERNSID_MISSING
//
// MessageText:
//
//  The attribute GOVERNSID is missing.
//
#define ERROR_DS_GOVERNSID_MISSING       8410L

//
// MessageId: ERROR_DS_MISSING_EXPECTED_ATT
//
// MessageText:
//
//  An expected attribute is missing.
//
#define ERROR_DS_MISSING_EXPECTED_ATT    8411L

//
// MessageId: ERROR_DS_NCNAME_MISSING_CR_REF
//
// MessageText:
//
//  The specified naming context is missing a cross reference.
//
#define ERROR_DS_NCNAME_MISSING_CR_REF   8412L

//
// MessageId: ERROR_DS_SECURITY_CHECKING_ERROR
//
// MessageText:
//
//  A security checking error has occurred.
//
#define ERROR_DS_SECURITY_CHECKING_ERROR 8413L

//
// MessageId: ERROR_DS_SCHEMA_NOT_LOADED
//
// MessageText:
//
//  The schema is not loaded.
//
#define ERROR_DS_SCHEMA_NOT_LOADED       8414L

//
// MessageId: ERROR_DS_SCHEMA_ALLOC_FAILED
//
// MessageText:
//
//  Schema allocation failed. Please check if the machine is running low on memory.
//
#define ERROR_DS_SCHEMA_ALLOC_FAILED     8415L

//
// MessageId: ERROR_DS_ATT_SCHEMA_REQ_SYNTAX
//
// MessageText:
//
//  Failed to obtain the required syntax for the attribute schema.
//
#define ERROR_DS_ATT_SCHEMA_REQ_SYNTAX   8416L

//
// MessageId: ERROR_DS_GCVERIFY_ERROR
//
// MessageText:
//
//  The global catalog verification failed. The global catalog is not available or does not support the operation. Some part of the directory is currently not available.
//
#define ERROR_DS_GCVERIFY_ERROR          8417L

//
// MessageId: ERROR_DS_DRA_SCHEMA_MISMATCH
//
// MessageText:
//
//  The replication operation failed because of a schema mismatch between the servers involved.
//
#define ERROR_DS_DRA_SCHEMA_MISMATCH     8418L

//
// MessageId: ERROR_DS_CANT_FIND_DSA_OBJ
//
// MessageText:
//
//  The DSA object could not be found.
//
#define ERROR_DS_CANT_FIND_DSA_OBJ       8419L

//
// MessageId: ERROR_DS_CANT_FIND_EXPECTED_NC
//
// MessageText:
//
//  The naming context could not be found.
//
#define ERROR_DS_CANT_FIND_EXPECTED_NC   8420L

//
// MessageId: ERROR_DS_CANT_FIND_NC_IN_CACHE
//
// MessageText:
//
//  The naming context could not be found in the cache.
//
#define ERROR_DS_CANT_FIND_NC_IN_CACHE   8421L

//
// MessageId: ERROR_DS_CANT_RETRIEVE_CHILD
//
// MessageText:
//
//  The child object could not be retrieved.
//
#define ERROR_DS_CANT_RETRIEVE_CHILD     8422L

//
// MessageId: ERROR_DS_SECURITY_ILLEGAL_MODIFY
//
// MessageText:
//
//  The modification was not permitted for security reasons.
//
#define ERROR_DS_SECURITY_ILLEGAL_MODIFY 8423L

//
// MessageId: ERROR_DS_CANT_REPLACE_HIDDEN_REC
//
// MessageText:
//
//  The operation cannot replace the hidden record.
//
#define ERROR_DS_CANT_REPLACE_HIDDEN_REC 8424L

//
// MessageId: ERROR_DS_BAD_HIERARCHY_FILE
//
// MessageText:
//
//  The hierarchy file is invalid.
//
#define ERROR_DS_BAD_HIERARCHY_FILE      8425L

//
// MessageId: ERROR_DS_BUILD_HIERARCHY_TABLE_FAILED
//
// MessageText:
//
//  The attempt to build the hierarchy table failed.
//
#define ERROR_DS_BUILD_HIERARCHY_TABLE_FAILED 8426L

//
// MessageId: ERROR_DS_CONFIG_PARAM_MISSING
//
// MessageText:
//
//  The directory configuration parameter is missing from the registry.
//
#define ERROR_DS_CONFIG_PARAM_MISSING    8427L

//
// MessageId: ERROR_DS_COUNTING_AB_INDICES_FAILED
//
// MessageText:
//
//  The attempt to count the address book indices failed.
//
#define ERROR_DS_COUNTING_AB_INDICES_FAILED 8428L

//
// MessageId: ERROR_DS_HIERARCHY_TABLE_MALLOC_FAILED
//
// MessageText:
//
//  The allocation of the hierarchy table failed.
//
#define ERROR_DS_HIERARCHY_TABLE_MALLOC_FAILED 8429L

//
// MessageId: ERROR_DS_INTERNAL_FAILURE
//
// MessageText:
//
//  The directory service encountered an internal failure.
//
#define ERROR_DS_INTERNAL_FAILURE        8430L

//
// MessageId: ERROR_DS_UNKNOWN_ERROR
//
// MessageText:
//
//  The directory service encountered an unknown failure.
//
#define ERROR_DS_UNKNOWN_ERROR           8431L

//
// MessageId: ERROR_DS_ROOT_REQUIRES_CLASS_TOP
//
// MessageText:
//
//  A root object requires a class of 'top'.
//
#define ERROR_DS_ROOT_REQUIRES_CLASS_TOP 8432L

//
// MessageId: ERROR_DS_REFUSING_FSMO_ROLES
//
// MessageText:
//
//  This directory server is shutting down, and cannot take ownership of new floating single-master operation roles.
//
#define ERROR_DS_REFUSING_FSMO_ROLES     8433L

//
// MessageId: ERROR_DS_MISSING_FSMO_SETTINGS
//
// MessageText:
//
//  The directory service is missing mandatory configuration information, and is unable to determine the ownership of floating single-master operation roles.
//
#define ERROR_DS_MISSING_FSMO_SETTINGS   8434L

//
// MessageId: ERROR_DS_UNABLE_TO_SURRENDER_ROLES
//
// MessageText:
//
//  The directory service was unable to transfer ownership of one or more floating single-master operation roles to other servers.
//
#define ERROR_DS_UNABLE_TO_SURRENDER_ROLES 8435L

//
// MessageId: ERROR_DS_DRA_GENERIC
//
// MessageText:
//
//  The replication operation failed.
//
#define ERROR_DS_DRA_GENERIC             8436L

//
// MessageId: ERROR_DS_DRA_INVALID_PARAMETER
//
// MessageText:
//
//  An invalid parameter was specified for this replication operation.
//
#define ERROR_DS_DRA_INVALID_PARAMETER   8437L

//
// MessageId: ERROR_DS_DRA_BUSY
//
// MessageText:
//
//  The directory service is too busy to complete the replication operation at this time.
//
#define ERROR_DS_DRA_BUSY                8438L

//
// MessageId: ERROR_DS_DRA_BAD_DN
//
// MessageText:
//
//  The distinguished name specified for this replication operation is invalid.
//
#define ERROR_DS_DRA_BAD_DN              8439L

//
// MessageId: ERROR_DS_DRA_BAD_NC
//
// MessageText:
//
//  The naming context specified for this replication operation is invalid.
//
#define ERROR_DS_DRA_BAD_NC              8440L

//
// MessageId: ERROR_DS_DRA_DN_EXISTS
//
// MessageText:
//
//  The distinguished name specified for this replication operation already exists.
//
#define ERROR_DS_DRA_DN_EXISTS           8441L

//
// MessageId: ERROR_DS_DRA_INTERNAL_ERROR
//
// MessageText:
//
//  The replication system encountered an internal error.
//
#define ERROR_DS_DRA_INTERNAL_ERROR      8442L

//
// MessageId: ERROR_DS_DRA_INCONSISTENT_DIT
//
// MessageText:
//
//  The replication operation encountered a database inconsistency.
//
#define ERROR_DS_DRA_INCONSISTENT_DIT    8443L

//
// MessageId: ERROR_DS_DRA_CONNECTION_FAILED
//
// MessageText:
//
//  The server specified for this replication operation could not be contacted.
//
#define ERROR_DS_DRA_CONNECTION_FAILED   8444L

//
// MessageId: ERROR_DS_DRA_BAD_INSTANCE_TYPE
//
// MessageText:
//
//  The replication operation encountered an object with an invalid instance type.
//
#define ERROR_DS_DRA_BAD_INSTANCE_TYPE   8445L

//
// MessageId: ERROR_DS_DRA_OUT_OF_MEM
//
// MessageText:
//
//  The replication operation failed to allocate memory.
//
#define ERROR_DS_DRA_OUT_OF_MEM          8446L

//
// MessageId: ERROR_DS_DRA_MAIL_PROBLEM
//
// MessageText:
//
//  The replication operation encountered an error with the mail system.
//
#define ERROR_DS_DRA_MAIL_PROBLEM        8447L

//
// MessageId: ERROR_DS_DRA_REF_ALREADY_EXISTS
//
// MessageText:
//
//  The replication reference information for the target server already exists.
//
#define ERROR_DS_DRA_REF_ALREADY_EXISTS  8448L

//
// MessageId: ERROR_DS_DRA_REF_NOT_FOUND
//
// MessageText:
//
//  The replication reference information for the target server does not exist.
//
#define ERROR_DS_DRA_REF_NOT_FOUND       8449L

//
// MessageId: ERROR_DS_DRA_OBJ_IS_REP_SOURCE
//
// MessageText:
//
//  The naming context cannot be removed because it is replicated to another server.
//
#define ERROR_DS_DRA_OBJ_IS_REP_SOURCE   8450L

//
// MessageId: ERROR_DS_DRA_DB_ERROR
//
// MessageText:
//
//  The replication operation encountered a database error.
//
#define ERROR_DS_DRA_DB_ERROR            8451L

//
// MessageId: ERROR_DS_DRA_NO_REPLICA
//
// MessageText:
//
//  The naming context is in the process of being removed or is not replicated from the specified server.
//
#define ERROR_DS_DRA_NO_REPLICA          8452L

//
// MessageId: ERROR_DS_DRA_ACCESS_DENIED
//
// MessageText:
//
//  Replication access was denied.
//
#define ERROR_DS_DRA_ACCESS_DENIED       8453L

//
// MessageId: ERROR_DS_DRA_NOT_SUPPORTED
//
// MessageText:
//
//  The requested operation is not supported by this version of the directory service.
//
#define ERROR_DS_DRA_NOT_SUPPORTED       8454L

//
// MessageId: ERROR_DS_DRA_RPC_CANCELLED
//
// MessageText:
//
//  The replication remote procedure call was cancelled.
//
#define ERROR_DS_DRA_RPC_CANCELLED       8455L

//
// MessageId: ERROR_DS_DRA_SOURCE_DISABLED
//
// MessageText:
//
//  The source server is currently rejecting replication requests.
//
#define ERROR_DS_DRA_SOURCE_DISABLED     8456L

//
// MessageId: ERROR_DS_DRA_SINK_DISABLED
//
// MessageText:
//
//  The destination server is currently rejecting replication requests.
//
#define ERROR_DS_DRA_SINK_DISABLED       8457L

//
// MessageId: ERROR_DS_DRA_NAME_COLLISION
//
// MessageText:
//
//  The replication operation failed due to a collision of object names.
//
#define ERROR_DS_DRA_NAME_COLLISION      8458L

//
// MessageId: ERROR_DS_DRA_SOURCE_REINSTALLED
//
// MessageText:
//
//  The replication source has been reinstalled.
//
#define ERROR_DS_DRA_SOURCE_REINSTALLED  8459L

//
// MessageId: ERROR_DS_DRA_MISSING_PARENT
//
// MessageText:
//
//  The replication operation failed because a required parent object is missing.
//
#define ERROR_DS_DRA_MISSING_PARENT      8460L

//
// MessageId: ERROR_DS_DRA_PREEMPTED
//
// MessageText:
//
//  The replication operation was preempted.
//
#define ERROR_DS_DRA_PREEMPTED           8461L

//
// MessageId: ERROR_DS_DRA_ABANDON_SYNC
//
// MessageText:
//
//  The replication synchronization attempt was abandoned because of a lack of updates.
//
#define ERROR_DS_DRA_ABANDON_SYNC        8462L

//
// MessageId: ERROR_DS_DRA_SHUTDOWN
//
// MessageText:
//
//  The replication operation was terminated because the system is shutting down.
//
#define ERROR_DS_DRA_SHUTDOWN            8463L

//
// MessageId: ERROR_DS_DRA_INCOMPATIBLE_PARTIAL_SET
//
// MessageText:
//
//  Synchronization attempt failed because the destination DC is currently waiting to synchronize new partial attributes from source. This condition is normal if a recent schema change modified the partial attribute set. The destination partial attribute set is not a subset of source partial attribute set.
//
#define ERROR_DS_DRA_INCOMPATIBLE_PARTIAL_SET 8464L

//
// MessageId: ERROR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA
//
// MessageText:
//
//  The replication synchronization attempt failed because a master replica attempted to sync from a partial replica.
//
#define ERROR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA 8465L

//
// MessageId: ERROR_DS_DRA_EXTN_CONNECTION_FAILED
//
// MessageText:
//
//  The server specified for this replication operation was contacted, but that server was unable to contact an additional server needed to complete the operation.
//
#define ERROR_DS_DRA_EXTN_CONNECTION_FAILED 8466L

//
// MessageId: ERROR_DS_INSTALL_SCHEMA_MISMATCH
//
// MessageText:
//
//  The version of the Active Directory schema of the source forest is not compatible with the version of Active Directory on this computer.
//
#define ERROR_DS_INSTALL_SCHEMA_MISMATCH 8467L

//
// MessageId: ERROR_DS_DUP_LINK_ID
//
// MessageText:
//
//  Schema update failed: An attribute with the same link identifier already exists.
//
#define ERROR_DS_DUP_LINK_ID             8468L

//
// MessageId: ERROR_DS_NAME_ERROR_RESOLVING
//
// MessageText:
//
//  Name translation: Generic processing error.
//
#define ERROR_DS_NAME_ERROR_RESOLVING    8469L

//
// MessageId: ERROR_DS_NAME_ERROR_NOT_FOUND
//
// MessageText:
//
//  Name translation: Could not find the name or insufficient right to see name.
//
#define ERROR_DS_NAME_ERROR_NOT_FOUND    8470L

//
// MessageId: ERROR_DS_NAME_ERROR_NOT_UNIQUE
//
// MessageText:
//
//  Name translation: Input name mapped to more than one output name.
//
#define ERROR_DS_NAME_ERROR_NOT_UNIQUE   8471L

//
// MessageId: ERROR_DS_NAME_ERROR_NO_MAPPING
//
// MessageText:
//
//  Name translation: Input name found, but not the associated output format.
//
#define ERROR_DS_NAME_ERROR_NO_MAPPING   8472L

//
// MessageId: ERROR_DS_NAME_ERROR_DOMAIN_ONLY
//
// MessageText:
//
//  Name translation: Unable to resolve completely, only the domain was found.
//
#define ERROR_DS_NAME_ERROR_DOMAIN_ONLY  8473L

//
// MessageId: ERROR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING
//
// MessageText:
//
//  Name translation: Unable to perform purely syntactical mapping at the client without going out to the wire.
//
#define ERROR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING 8474L

//
// MessageId: ERROR_DS_CONSTRUCTED_ATT_MOD
//
// MessageText:
//
//  Modification of a constructed attribute is not allowed.
//
#define ERROR_DS_CONSTRUCTED_ATT_MOD     8475L

//
// MessageId: ERROR_DS_WRONG_OM_OBJ_CLASS
//
// MessageText:
//
//  The OM-Object-Class specified is incorrect for an attribute with the specified syntax.
//
#define ERROR_DS_WRONG_OM_OBJ_CLASS      8476L

//
// MessageId: ERROR_DS_DRA_REPL_PENDING
//
// MessageText:
//
//  The replication request has been posted; waiting for reply.
//
#define ERROR_DS_DRA_REPL_PENDING        8477L

//
// MessageId: ERROR_DS_DS_REQUIRED
//
// MessageText:
//
//  The requested operation requires a directory service, and none was available.
//
#define ERROR_DS_DS_REQUIRED             8478L

//
// MessageId: ERROR_DS_INVALID_LDAP_DISPLAY_NAME
//
// MessageText:
//
//  The LDAP display name of the class or attribute contains non-ASCII characters.
//
#define ERROR_DS_INVALID_LDAP_DISPLAY_NAME 8479L

//
// MessageId: ERROR_DS_NON_BASE_SEARCH
//
// MessageText:
//
//  The requested search operation is only supported for base searches.
//
#define ERROR_DS_NON_BASE_SEARCH         8480L

//
// MessageId: ERROR_DS_CANT_RETRIEVE_ATTS
//
// MessageText:
//
//  The search failed to retrieve attributes from the database.
//
#define ERROR_DS_CANT_RETRIEVE_ATTS      8481L

//
// MessageId: ERROR_DS_BACKLINK_WITHOUT_LINK
//
// MessageText:
//
//  The schema update operation tried to add a backward link attribute that has no corresponding forward link.
//
#define ERROR_DS_BACKLINK_WITHOUT_LINK   8482L

//
// MessageId: ERROR_DS_EPOCH_MISMATCH
//
// MessageText:
//
//  Source and destination of a cross-domain move do not agree on the object's epoch number.  Either source or destination does not have the latest version of the object.
//
#define ERROR_DS_EPOCH_MISMATCH          8483L

//
// MessageId: ERROR_DS_SRC_NAME_MISMATCH
//
// MessageText:
//
//  Source and destination of a cross-domain move do not agree on the object's current name.  Either source or destination does not have the latest version of the object.
//
#define ERROR_DS_SRC_NAME_MISMATCH       8484L

//
// MessageId: ERROR_DS_SRC_AND_DST_NC_IDENTICAL
//
// MessageText:
//
//  Source and destination for the cross-domain move operation are identical.  Caller should use local move operation instead of cross-domain move operation.
//
#define ERROR_DS_SRC_AND_DST_NC_IDENTICAL 8485L

//
// MessageId: ERROR_DS_DST_NC_MISMATCH
//
// MessageText:
//
//  Source and destination for a cross-domain move are not in agreement on the naming contexts in the forest.  Either source or destination does not have the latest version of the Partitions container.
//
#define ERROR_DS_DST_NC_MISMATCH         8486L

//
// MessageId: ERROR_DS_NOT_AUTHORITIVE_FOR_DST_NC
//
// MessageText:
//
//  Destination of a cross-domain move is not authoritative for the destination naming context.
//
#define ERROR_DS_NOT_AUTHORITIVE_FOR_DST_NC 8487L

//
// MessageId: ERROR_DS_SRC_GUID_MISMATCH
//
// MessageText:
//
//  Source and destination of a cross-domain move do not agree on the identity of the source object.  Either source or destination does not have the latest version of the source object.
//
#define ERROR_DS_SRC_GUID_MISMATCH       8488L

//
// MessageId: ERROR_DS_CANT_MOVE_DELETED_OBJECT
//
// MessageText:
//
//  Object being moved across-domains is already known to be deleted by the destination server.  The source server does not have the latest version of the source object.
//
#define ERROR_DS_CANT_MOVE_DELETED_OBJECT 8489L

//
// MessageId: ERROR_DS_PDC_OPERATION_IN_PROGRESS
//
// MessageText:
//
//  Another operation which requires exclusive access to the PDC FSMO is already in progress.
//
#define ERROR_DS_PDC_OPERATION_IN_PROGRESS 8490L

//
// MessageId: ERROR_DS_CROSS_DOMAIN_CLEANUP_REQD
//
// MessageText:
//
//  A cross-domain move operation failed such that two versions of the moved object exist - one each in the source and destination domains.  The destination object needs to be removed to restore the system to a consistent state.
//
#define ERROR_DS_CROSS_DOMAIN_CLEANUP_REQD 8491L

//
// MessageId: ERROR_DS_ILLEGAL_XDOM_MOVE_OPERATION
//
// MessageText:
//
//  This object may not be moved across domain boundaries either because cross-domain moves for this class are disallowed, or the object has some special characteristics, e.g.: trust account or restricted RID, which prevent its move.
//
#define ERROR_DS_ILLEGAL_XDOM_MOVE_OPERATION 8492L

//
// MessageId: ERROR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS
//
// MessageText:
//
//  Can't move objects with memberships across domain boundaries as once moved, this would violate the membership conditions of the account group.  Remove the object from any account group memberships and retry.
//
#define ERROR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS 8493L

//
// MessageId: ERROR_DS_NC_MUST_HAVE_NC_PARENT
//
// MessageText:
//
//  A naming context head must be the immediate child of another naming context head, not of an interior node.
//
#define ERROR_DS_NC_MUST_HAVE_NC_PARENT  8494L

//
// MessageId: ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE
//
// MessageText:
//
//  The directory cannot validate the proposed naming context name because it does not hold a replica of the naming context above the proposed naming context.  Please ensure that the domain naming master role is held by a server that is configured as a global catalog server, and that the server is up to date with its replication partners. (Applies only to Windows 2000 Domain Naming masters)
//
#define ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE 8495L

//
// MessageId: ERROR_DS_DST_DOMAIN_NOT_NATIVE
//
// MessageText:
//
//  Destination domain must be in native mode.
//
#define ERROR_DS_DST_DOMAIN_NOT_NATIVE   8496L

//
// MessageId: ERROR_DS_MISSING_INFRASTRUCTURE_CONTAINER
//
// MessageText:
//
//  The operation can not be performed because the server does not have an infrastructure container in the domain of interest.
//
#define ERROR_DS_MISSING_INFRASTRUCTURE_CONTAINER 8497L

//
// MessageId: ERROR_DS_CANT_MOVE_ACCOUNT_GROUP
//
// MessageText:
//
//  Cross-domain move of non-empty account groups is not allowed.
//
#define ERROR_DS_CANT_MOVE_ACCOUNT_GROUP 8498L

//
// MessageId: ERROR_DS_CANT_MOVE_RESOURCE_GROUP
//
// MessageText:
//
//  Cross-domain move of non-empty resource groups is not allowed.
//
#define ERROR_DS_CANT_MOVE_RESOURCE_GROUP 8499L

//
// MessageId: ERROR_DS_INVALID_SEARCH_FLAG
//
// MessageText:
//
//  The search flags for the attribute are invalid. The ANR bit is valid only on attributes of Unicode or Teletex strings.
//
#define ERROR_DS_INVALID_SEARCH_FLAG     8500L

//
// MessageId: ERROR_DS_NO_TREE_DELETE_ABOVE_NC
//
// MessageText:
//
//  Tree deletions starting at an object which has an NC head as a descendant are not allowed.
//
#define ERROR_DS_NO_TREE_DELETE_ABOVE_NC 8501L

//
// MessageId: ERROR_DS_COULDNT_LOCK_TREE_FOR_DELETE
//
// MessageText:
//
//  The directory service failed to lock a tree in preparation for a tree deletion because the tree was in use.
//
#define ERROR_DS_COULDNT_LOCK_TREE_FOR_DELETE 8502L

//
// MessageId: ERROR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE
//
// MessageText:
//
//  The directory service failed to identify the list of objects to delete while attempting a tree deletion.
//
#define ERROR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE 8503L

//
// MessageId: ERROR_DS_SAM_INIT_FAILURE
//
// MessageText:
//
//  Security Accounts Manager initialization failed because of the following error: %1.
//  Error Status: 0x%2. Click OK to shut down the system and reboot into Directory Services Restore Mode. Check the event log for detailed information.
//
#define ERROR_DS_SAM_INIT_FAILURE        8504L

//
// MessageId: ERROR_DS_SENSITIVE_GROUP_VIOLATION
//
// MessageText:
//
//  Only an administrator can modify the membership list of an administrative group.
//
#define ERROR_DS_SENSITIVE_GROUP_VIOLATION 8505L

//
// MessageId: ERROR_DS_CANT_MOD_PRIMARYGROUPID
//
// MessageText:
//
//  Cannot change the primary group ID of a domain controller account.
//
#define ERROR_DS_CANT_MOD_PRIMARYGROUPID 8506L

//
// MessageId: ERROR_DS_ILLEGAL_BASE_SCHEMA_MOD
//
// MessageText:
//
//  An attempt is made to modify the base schema.
//
#define ERROR_DS_ILLEGAL_BASE_SCHEMA_MOD 8507L

//
// MessageId: ERROR_DS_NONSAFE_SCHEMA_CHANGE
//
// MessageText:
//
//  Adding a new mandatory attribute to an existing class, deleting a mandatory attribute from an existing class, or adding an optional attribute to the special class Top that is not a backlink attribute (directly or through inheritance, for example, by adding or deleting an auxiliary class) is not allowed.
//
#define ERROR_DS_NONSAFE_SCHEMA_CHANGE   8508L

//
// MessageId: ERROR_DS_SCHEMA_UPDATE_DISALLOWED
//
// MessageText:
//
//  Schema update is not allowed on this DC because the DC is not the schema FSMO Role Owner.
//
#define ERROR_DS_SCHEMA_UPDATE_DISALLOWED 8509L

//
// MessageId: ERROR_DS_CANT_CREATE_UNDER_SCHEMA
//
// MessageText:
//
//  An object of this class cannot be created under the schema container. You can only create attribute-schema and class-schema objects under the schema container.
//
#define ERROR_DS_CANT_CREATE_UNDER_SCHEMA 8510L

//
// MessageId: ERROR_DS_INSTALL_NO_SRC_SCH_VERSION
//
// MessageText:
//
//  The replica/child install failed to get the objectVersion attribute on the schema container on the source DC. Either the attribute is missing on the schema container or the credentials supplied do not have permission to read it.
//
#define ERROR_DS_INSTALL_NO_SRC_SCH_VERSION 8511L

//
// MessageId: ERROR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE
//
// MessageText:
//
//  The replica/child install failed to read the objectVersion attribute in the SCHEMA section of the file schema.ini in the system32 directory.
//
#define ERROR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE 8512L

//
// MessageId: ERROR_DS_INVALID_GROUP_TYPE
//
// MessageText:
//
//  The specified group type is invalid.
//
#define ERROR_DS_INVALID_GROUP_TYPE      8513L

//
// MessageId: ERROR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN
//
// MessageText:
//
//  You cannot nest global groups in a mixed domain if the group is security-enabled.
//
#define ERROR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN 8514L

//
// MessageId: ERROR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN
//
// MessageText:
//
//  You cannot nest local groups in a mixed domain if the group is security-enabled.
//
#define ERROR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN 8515L

//
// MessageId: ERROR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER
//
// MessageText:
//
//  A global group cannot have a local group as a member.
//
#define ERROR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER 8516L

//
// MessageId: ERROR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER
//
// MessageText:
//
//  A global group cannot have a universal group as a member.
//
#define ERROR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER 8517L

//
// MessageId: ERROR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER
//
// MessageText:
//
//  A universal group cannot have a local group as a member.
//
#define ERROR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER 8518L

//
// MessageId: ERROR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER
//
// MessageText:
//
//  A global group cannot have a cross-domain member.
//
#define ERROR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER 8519L

//
// MessageId: ERROR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER
//
// MessageText:
//
//  A local group cannot have another cross domain local group as a member.
//
#define ERROR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER 8520L

//
// MessageId: ERROR_DS_HAVE_PRIMARY_MEMBERS
//
// MessageText:
//
//  A group with primary members cannot change to a security-disabled group.
//
#define ERROR_DS_HAVE_PRIMARY_MEMBERS    8521L

//
// MessageId: ERROR_DS_STRING_SD_CONVERSION_FAILED
//
// MessageText:
//
//  The schema cache load failed to convert the string default SD on a class-schema object.
//
#define ERROR_DS_STRING_SD_CONVERSION_FAILED 8522L

//
// MessageId: ERROR_DS_NAMING_MASTER_GC
//
// MessageText:
//
//  Only DSAs configured to be Global Catalog servers should be allowed to hold the Domain Naming Master FSMO role. (Applies only to Windows 2000 servers)
//
#define ERROR_DS_NAMING_MASTER_GC        8523L

//
// MessageId: ERROR_DS_DNS_LOOKUP_FAILURE
//
// MessageText:
//
//  The DSA operation is unable to proceed because of a DNS lookup failure.
//
#define ERROR_DS_DNS_LOOKUP_FAILURE      8524L

//
// MessageId: ERROR_DS_COULDNT_UPDATE_SPNS
//
// MessageText:
//
//  While processing a change to the DNS Host Name for an object, the Service Principal Name values could not be kept in sync.
//
#define ERROR_DS_COULDNT_UPDATE_SPNS     8525L

//
// MessageId: ERROR_DS_CANT_RETRIEVE_SD
//
// MessageText:
//
//  The Security Descriptor attribute could not be read.
//
#define ERROR_DS_CANT_RETRIEVE_SD        8526L

//
// MessageId: ERROR_DS_KEY_NOT_UNIQUE
//
// MessageText:
//
//  The object requested was not found, but an object with that key was found.
//
#define ERROR_DS_KEY_NOT_UNIQUE          8527L

//
// MessageId: ERROR_DS_WRONG_LINKED_ATT_SYNTAX
//
// MessageText:
//
//  The syntax of the linked attribute being added is incorrect. Forward links can only have syntax 2.5.5.1, 2.5.5.7, and 2.5.5.14, and backlinks can only have syntax 2.5.5.1
//
#define ERROR_DS_WRONG_LINKED_ATT_SYNTAX 8528L

//
// MessageId: ERROR_DS_SAM_NEED_BOOTKEY_PASSWORD
//
// MessageText:
//
//  Security Account Manager needs to get the boot password.
//
#define ERROR_DS_SAM_NEED_BOOTKEY_PASSWORD 8529L

//
// MessageId: ERROR_DS_SAM_NEED_BOOTKEY_FLOPPY
//
// MessageText:
//
//  Security Account Manager needs to get the boot key from floppy disk.
//
#define ERROR_DS_SAM_NEED_BOOTKEY_FLOPPY 8530L

//
// MessageId: ERROR_DS_CANT_START
//
// MessageText:
//
//  Directory Service cannot start.
//
#define ERROR_DS_CANT_START              8531L

//
// MessageId: ERROR_DS_INIT_FAILURE
//
// MessageText:
//
//  Directory Services could not start.
//
#define ERROR_DS_INIT_FAILURE            8532L

//
// MessageId: ERROR_DS_NO_PKT_PRIVACY_ON_CONNECTION
//
// MessageText:
//
//  The connection between client and server requires packet privacy or better.
//
#define ERROR_DS_NO_PKT_PRIVACY_ON_CONNECTION 8533L

//
// MessageId: ERROR_DS_SOURCE_DOMAIN_IN_FOREST
//
// MessageText:
//
//  The source domain may not be in the same forest as destination.
//
#define ERROR_DS_SOURCE_DOMAIN_IN_FOREST 8534L

//
// MessageId: ERROR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST
//
// MessageText:
//
//  The destination domain must be in the forest.
//
#define ERROR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST 8535L

//
// MessageId: ERROR_DS_DESTINATION_AUDITING_NOT_ENABLED
//
// MessageText:
//
//  The operation requires that destination domain auditing be enabled.
//
#define ERROR_DS_DESTINATION_AUDITING_NOT_ENABLED 8536L

//
// MessageId: ERROR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN
//
// MessageText:
//
//  The operation couldn't locate a DC for the source domain.
//
#define ERROR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN 8537L

//
// MessageId: ERROR_DS_SRC_OBJ_NOT_GROUP_OR_USER
//
// MessageText:
//
//  The source object must be a group or user.
//
#define ERROR_DS_SRC_OBJ_NOT_GROUP_OR_USER 8538L

//
// MessageId: ERROR_DS_SRC_SID_EXISTS_IN_FOREST
//
// MessageText:
//
//  The source object's SID already exists in destination forest.
//
#define ERROR_DS_SRC_SID_EXISTS_IN_FOREST 8539L

//
// MessageId: ERROR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH
//
// MessageText:
//
//  The source and destination object must be of the same type.
//
#define ERROR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH 8540L

//
// MessageId: ERROR_SAM_INIT_FAILURE
//
// MessageText:
//
//  Security Accounts Manager initialization failed because of the following error: %1.
//  Error Status: 0x%2. Click OK to shut down the system and reboot into Safe Mode. Check the event log for detailed information.
//
#define ERROR_SAM_INIT_FAILURE           8541L

//
// MessageId: ERROR_DS_DRA_SCHEMA_INFO_SHIP
//
// MessageText:
//
//  Schema information could not be included in the replication request.
//
#define ERROR_DS_DRA_SCHEMA_INFO_SHIP    8542L

//
// MessageId: ERROR_DS_DRA_SCHEMA_CONFLICT
//
// MessageText:
//
//  The replication operation could not be completed due to a schema incompatibility.
//
#define ERROR_DS_DRA_SCHEMA_CONFLICT     8543L

//
// MessageId: ERROR_DS_DRA_EARLIER_SCHEMA_CONFLICT
//
// MessageText:
//
//  The replication operation could not be completed due to a previous schema incompatibility.
//
#define ERROR_DS_DRA_EARLIER_SCHEMA_CONFLICT 8544L

//
// MessageId: ERROR_DS_DRA_OBJ_NC_MISMATCH
//
// MessageText:
//
//  The replication update could not be applied because either the source or the destination has not yet received information regarding a recent cross-domain move operation.
//
#define ERROR_DS_DRA_OBJ_NC_MISMATCH     8545L

//
// MessageId: ERROR_DS_NC_STILL_HAS_DSAS
//
// MessageText:
//
//  The requested domain could not be deleted because there exist domain controllers that still host this domain.
//
#define ERROR_DS_NC_STILL_HAS_DSAS       8546L

//
// MessageId: ERROR_DS_GC_REQUIRED
//
// MessageText:
//
//  The requested operation can be performed only on a global catalog server.
//
#define ERROR_DS_GC_REQUIRED             8547L

//
// MessageId: ERROR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY
//
// MessageText:
//
//  A local group can only be a member of other local groups in the same domain.
//
#define ERROR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY 8548L

//
// MessageId: ERROR_DS_NO_FPO_IN_UNIVERSAL_GROUPS
//
// MessageText:
//
//  Foreign security principals cannot be members of universal groups.
//
#define ERROR_DS_NO_FPO_IN_UNIVERSAL_GROUPS 8549L

//
// MessageId: ERROR_DS_CANT_ADD_TO_GC
//
// MessageText:
//
//  The attribute is not allowed to be replicated to the GC because of security reasons.
//
#define ERROR_DS_CANT_ADD_TO_GC          8550L

//
// MessageId: ERROR_DS_NO_CHECKPOINT_WITH_PDC
//
// MessageText:
//
//  The checkpoint with the PDC could not be taken because there too many modifications being processed currently.
//
#define ERROR_DS_NO_CHECKPOINT_WITH_PDC  8551L

//
// MessageId: ERROR_DS_SOURCE_AUDITING_NOT_ENABLED
//
// MessageText:
//
//  The operation requires that source domain auditing be enabled.
//
#define ERROR_DS_SOURCE_AUDITING_NOT_ENABLED 8552L

//
// MessageId: ERROR_DS_CANT_CREATE_IN_NONDOMAIN_NC
//
// MessageText:
//
//  Security principal objects can only be created inside domain naming contexts.
//
#define ERROR_DS_CANT_CREATE_IN_NONDOMAIN_NC 8553L

//
// MessageId: ERROR_DS_INVALID_NAME_FOR_SPN
//
// MessageText:
//
//  A Service Principal Name (SPN) could not be constructed because the provided hostname is not in the necessary format.
//
#define ERROR_DS_INVALID_NAME_FOR_SPN    8554L

//
// MessageId: ERROR_DS_FILTER_USES_CONTRUCTED_ATTRS
//
// MessageText:
//
//  A Filter was passed that uses constructed attributes.
//
#define ERROR_DS_FILTER_USES_CONTRUCTED_ATTRS 8555L

//
// MessageId: ERROR_DS_UNICODEPWD_NOT_IN_QUOTES
//
// MessageText:
//
//  The unicodePwd attribute value must be enclosed in double quotes.
//
#define ERROR_DS_UNICODEPWD_NOT_IN_QUOTES 8556L

//
// MessageId: ERROR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED
//
// MessageText:
//
//  Your computer could not be joined to the domain. You have exceeded the maximum number of computer accounts you are allowed to create in this domain. Contact your system administrator to have this limit reset or increased.
//
#define ERROR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED 8557L

//
// MessageId: ERROR_DS_MUST_BE_RUN_ON_DST_DC
//
// MessageText:
//
//  For security reasons, the operation must be run on the destination DC.
//
#define ERROR_DS_MUST_BE_RUN_ON_DST_DC   8558L

//
// MessageId: ERROR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER
//
// MessageText:
//
//  For security reasons, the source DC must be NT4SP4 or greater.
//
#define ERROR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER 8559L

//
// MessageId: ERROR_DS_CANT_TREE_DELETE_CRITICAL_OBJ
//
// MessageText:
//
//  Critical Directory Service System objects cannot be deleted during tree delete operations.  The tree delete may have been partially performed.
//
#define ERROR_DS_CANT_TREE_DELETE_CRITICAL_OBJ 8560L

//
// MessageId: ERROR_DS_INIT_FAILURE_CONSOLE
//
// MessageText:
//
//  Directory Services could not start because of the following error: %1.
//  Error Status: 0x%2. Please click OK to shutdown the system. You can use the recovery console to diagnose the system further.
//
#define ERROR_DS_INIT_FAILURE_CONSOLE    8561L

//
// MessageId: ERROR_DS_SAM_INIT_FAILURE_CONSOLE
//
// MessageText:
//
//  Security Accounts Manager initialization failed because of the following error: %1.
//  Error Status: 0x%2. Please click OK to shutdown the system. You can use the recovery console to diagnose the system further.
//
#define ERROR_DS_SAM_INIT_FAILURE_CONSOLE 8562L

//
// MessageId: ERROR_DS_FOREST_VERSION_TOO_HIGH
//
// MessageText:
//
//  The version of the operating system installed is incompatible with the current forest functional level. You must upgrade to a new version of the operating system before this server can become a domain controller in this forest.
//
#define ERROR_DS_FOREST_VERSION_TOO_HIGH 8563L

//
// MessageId: ERROR_DS_DOMAIN_VERSION_TOO_HIGH
//
// MessageText:
//
//  The version of the operating system installed is incompatible with the current domain functional level. You must upgrade to a new version of the operating system before this server can become a domain controller in this domain.
//
#define ERROR_DS_DOMAIN_VERSION_TOO_HIGH 8564L

//
// MessageId: ERROR_DS_FOREST_VERSION_TOO_LOW
//
// MessageText:
//
//  The version of the operating system installed on this server no longer supports the current forest functional level. You must raise the forest functional level before this server can become a domain controller in this forest.
//
#define ERROR_DS_FOREST_VERSION_TOO_LOW  8565L

//
// MessageId: ERROR_DS_DOMAIN_VERSION_TOO_LOW
//
// MessageText:
//
//  The version of the operating system installed on this server no longer supports the current domain functional level. You must raise the domain functional level before this server can become a domain controller in this domain.
//
#define ERROR_DS_DOMAIN_VERSION_TOO_LOW  8566L

//
// MessageId: ERROR_DS_INCOMPATIBLE_VERSION
//
// MessageText:
//
//  The version of the operating system installed on this server is incompatible with the functional level of the domain or forest.
//
#define ERROR_DS_INCOMPATIBLE_VERSION    8567L

//
// MessageId: ERROR_DS_LOW_DSA_VERSION
//
// MessageText:
//
//  The functional level of the domain (or forest) cannot be raised to the requested value, because there exist one or more domain controllers in the domain (or forest) that are at a lower incompatible functional level.
//
#define ERROR_DS_LOW_DSA_VERSION         8568L

//
// MessageId: ERROR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN
//
// MessageText:
//
//  The forest functional level cannot be raised to the requested value since one or more domains are still in mixed domain mode. All domains in the forest must be in native mode, for you to raise the forest functional level.
//
#define ERROR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN 8569L

//
// MessageId: ERROR_DS_NOT_SUPPORTED_SORT_ORDER
//
// MessageText:
//
//  The sort order requested is not supported.
//
#define ERROR_DS_NOT_SUPPORTED_SORT_ORDER 8570L

//
// MessageId: ERROR_DS_NAME_NOT_UNIQUE
//
// MessageText:
//
//  The requested name already exists as a unique identifier.
//
#define ERROR_DS_NAME_NOT_UNIQUE         8571L

//
// MessageId: ERROR_DS_MACHINE_ACCOUNT_CREATED_PRENT4
//
// MessageText:
//
//  The machine account was created pre-NT4.  The account needs to be recreated.
//
#define ERROR_DS_MACHINE_ACCOUNT_CREATED_PRENT4 8572L

//
// MessageId: ERROR_DS_OUT_OF_VERSION_STORE
//
// MessageText:
//
//  The database is out of version store.
//
#define ERROR_DS_OUT_OF_VERSION_STORE    8573L

//
// MessageId: ERROR_DS_INCOMPATIBLE_CONTROLS_USED
//
// MessageText:
//
//  Unable to continue operation because multiple conflicting controls were used.
//
#define ERROR_DS_INCOMPATIBLE_CONTROLS_USED 8574L

//
// MessageId: ERROR_DS_NO_REF_DOMAIN
//
// MessageText:
//
//  Unable to find a valid security descriptor reference domain for this partition.
//
#define ERROR_DS_NO_REF_DOMAIN           8575L

//
// MessageId: ERROR_DS_RESERVED_LINK_ID
//
// MessageText:
//
//  Schema update failed: The link identifier is reserved.
//
#define ERROR_DS_RESERVED_LINK_ID        8576L

//
// MessageId: ERROR_DS_LINK_ID_NOT_AVAILABLE
//
// MessageText:
//
//  Schema update failed: There are no link identifiers available.
//
#define ERROR_DS_LINK_ID_NOT_AVAILABLE   8577L

//
// MessageId: ERROR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER
//
// MessageText:
//
//  An account group can not have a universal group as a member.
//
#define ERROR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER 8578L

//
// MessageId: ERROR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE
//
// MessageText:
//
//  Rename or move operations on naming context heads or read-only objects are not allowed.
//
#define ERROR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE 8579L

//
// MessageId: ERROR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC
//
// MessageText:
//
//  Move operations on objects in the schema naming context are not allowed.
//
#define ERROR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC 8580L

//
// MessageId: ERROR_DS_MODIFYDN_DISALLOWED_BY_FLAG
//
// MessageText:
//
//  A system flag has been set on the object and does not allow the object to be moved or renamed.
//
#define ERROR_DS_MODIFYDN_DISALLOWED_BY_FLAG 8581L

//
// MessageId: ERROR_DS_MODIFYDN_WRONG_GRANDPARENT
//
// MessageText:
//
//  This object is not allowed to change its grandparent container. Moves are not forbidden on this object, but are restricted to sibling containers.
//
#define ERROR_DS_MODIFYDN_WRONG_GRANDPARENT 8582L

//
// MessageId: ERROR_DS_NAME_ERROR_TRUST_REFERRAL
//
// MessageText:
//
//  Unable to resolve completely, a referral to another forest is generated.
//
#define ERROR_DS_NAME_ERROR_TRUST_REFERRAL 8583L

//
// MessageId: ERROR_NOT_SUPPORTED_ON_STANDARD_SERVER
//
// MessageText:
//
//  The requested action is not supported on standard server.
//
#define ERROR_NOT_SUPPORTED_ON_STANDARD_SERVER 8584L

//
// MessageId: ERROR_DS_CANT_ACCESS_REMOTE_PART_OF_AD
//
// MessageText:
//
//  Could not access a partition of the Active Directory located on a remote server.  Make sure at least one server is running for the partition in question.
//
#define ERROR_DS_CANT_ACCESS_REMOTE_PART_OF_AD 8585L

//
// MessageId: ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2
//
// MessageText:
//
//  The directory cannot validate the proposed naming context (or partition) name because it does not hold a replica nor can it contact a replica of the naming context above the proposed naming context.  Please ensure that the parent naming context is properly registered in DNS, and at least one replica of this naming context is reachable by the Domain Naming master.
//
#define ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2 8586L

//
// MessageId: ERROR_DS_THREAD_LIMIT_EXCEEDED
//
// MessageText:
//
//  The thread limit for this request was exceeded.
//
#define ERROR_DS_THREAD_LIMIT_EXCEEDED   8587L

//
// MessageId: ERROR_DS_NOT_CLOSEST
//
// MessageText:
//
//  The Global catalog server is not in the closest site.
//
#define ERROR_DS_NOT_CLOSEST             8588L

//
// MessageId: ERROR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF
//
// MessageText:
//
//  The DS cannot derive a service principal name (SPN) with which to mutually authenticate the target server because the corresponding server object in the local DS database has no serverReference attribute.
//
#define ERROR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF 8589L

//
// MessageId: ERROR_DS_SINGLE_USER_MODE_FAILED
//
// MessageText:
//
//  The Directory Service failed to enter single user mode.
//
#define ERROR_DS_SINGLE_USER_MODE_FAILED 8590L

//
// MessageId: ERROR_DS_NTDSCRIPT_SYNTAX_ERROR
//
// MessageText:
//
//  The Directory Service cannot parse the script because of a syntax error.
//
#define ERROR_DS_NTDSCRIPT_SYNTAX_ERROR  8591L

//
// MessageId: ERROR_DS_NTDSCRIPT_PROCESS_ERROR
//
// MessageText:
//
//  The Directory Service cannot process the script because of an error.
//
#define ERROR_DS_NTDSCRIPT_PROCESS_ERROR 8592L

//
// MessageId: ERROR_DS_DIFFERENT_REPL_EPOCHS
//
// MessageText:
//
//  The directory service cannot perform the requested operation because the servers
//  involved are of different replication epochs (which is usually related to a
//  domain rename that is in progress).
//
#define ERROR_DS_DIFFERENT_REPL_EPOCHS   8593L

//
// MessageId: ERROR_DS_DRS_EXTENSIONS_CHANGED
//
// MessageText:
//
//  The directory service binding must be renegotiated due to a change in the server
//  extensions information.
//
#define ERROR_DS_DRS_EXTENSIONS_CHANGED  8594L

//
// MessageId: ERROR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR
//
// MessageText:
//
//  Operation not allowed on a disabled cross ref.
//
#define ERROR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR 8595L

//
// MessageId: ERROR_DS_NO_MSDS_INTID
//
// MessageText:
//
//  Schema update failed: No values for msDS-IntId are available.
//
#define ERROR_DS_NO_MSDS_INTID           8596L

//
// MessageId: ERROR_DS_DUP_MSDS_INTID
//
// MessageText:
//
//  Schema update failed: Duplicate msDS-INtId. Retry the operation.
//
#define ERROR_DS_DUP_MSDS_INTID          8597L

//
// MessageId: ERROR_DS_EXISTS_IN_RDNATTID
//
// MessageText:
//
//  Schema deletion failed: attribute is used in rDNAttID.
//
#define ERROR_DS_EXISTS_IN_RDNATTID      8598L

//
// MessageId: ERROR_DS_AUTHORIZATION_FAILED
//
// MessageText:
//
//  The directory service failed to authorize the request.
//
#define ERROR_DS_AUTHORIZATION_FAILED    8599L

//
// MessageId: ERROR_DS_INVALID_SCRIPT
//
// MessageText:
//
//  The Directory Service cannot process the script because it is invalid.
//
#define ERROR_DS_INVALID_SCRIPT          8600L

//
// MessageId: ERROR_DS_REMOTE_CROSSREF_OP_FAILED
//
// MessageText:
//
//  The remote create cross reference operation failed on the Domain Naming Master FSMO.  The operation's error is in the extended data.
//
#define ERROR_DS_REMOTE_CROSSREF_OP_FAILED 8601L

//
// MessageId: ERROR_DS_CROSS_REF_BUSY
//
// MessageText:
//
//  A cross reference is in use locally with the same name.
//
#define ERROR_DS_CROSS_REF_BUSY          8602L

//
// MessageId: ERROR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN
//
// MessageText:
//
//  The DS cannot derive a service principal name (SPN) with which to mutually authenticate the target server because the server's domain has been deleted from the forest.
//
#define ERROR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN 8603L

//
// MessageId: ERROR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC
//
// MessageText:
//
//  Writeable NCs prevent this DC from demoting.
//
#define ERROR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC 8604L

//
// MessageId: ERROR_DS_DUPLICATE_ID_FOUND
//
// MessageText:
//
//  The requested object has a non-unique identifier and cannot be retrieved.
//
#define ERROR_DS_DUPLICATE_ID_FOUND      8605L

//
// MessageId: ERROR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT
//
// MessageText:
//
//  Insufficient attributes were given to create an object.  This object may not exist because it may have been deleted and already garbage collected.
//
#define ERROR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT 8606L

//
// MessageId: ERROR_DS_GROUP_CONVERSION_ERROR
//
// MessageText:
//
//  The group cannot be converted due to attribute restrictions on the requested group type.
//
#define ERROR_DS_GROUP_CONVERSION_ERROR  8607L

//
// MessageId: ERROR_DS_CANT_MOVE_APP_BASIC_GROUP
//
// MessageText:
//
//  Cross-domain move of non-empty basic application groups is not allowed.
//
#define ERROR_DS_CANT_MOVE_APP_BASIC_GROUP 8608L

//
// MessageId: ERROR_DS_CANT_MOVE_APP_QUERY_GROUP
//
// MessageText:
//
//  Cross-domain move of non-empty query based application groups is not allowed.
//
#define ERROR_DS_CANT_MOVE_APP_QUERY_GROUP 8609L

//
// MessageId: ERROR_DS_ROLE_NOT_VERIFIED
//
// MessageText:
//
//  The FSMO role ownership could not be verified because its directory partition has not replicated successfully with atleast one replication partner.
//
#define ERROR_DS_ROLE_NOT_VERIFIED       8610L

//
// MessageId: ERROR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL
//
// MessageText:
//
//  The target container for a redirection of a well known object container cannot already be a special container.
//
#define ERROR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL 8611L

//
// MessageId: ERROR_DS_DOMAIN_RENAME_IN_PROGRESS
//
// MessageText:
//
//  The Directory Service cannot perform the requested operation because a domain rename operation is in progress.
//
#define ERROR_DS_DOMAIN_RENAME_IN_PROGRESS 8612L

//
// MessageId: ERROR_DS_EXISTING_AD_CHILD_NC
//
// MessageText:
//
//  The Active Directory detected an Active Directory child partition below the
//  requested new partition name.  The Active Directory's partition heiarchy must
//  be created in a top down method.
//
#define ERROR_DS_EXISTING_AD_CHILD_NC    8613L

//
// MessageId: ERROR_DS_REPL_LIFETIME_EXCEEDED
//
// MessageText:
//
//  The Active Directory cannot replicate with this server because the time since the last replication with this server has exceeded the tombstone lifetime.
//
#define ERROR_DS_REPL_LIFETIME_EXCEEDED  8614L

//
// MessageId: ERROR_DS_DISALLOWED_IN_SYSTEM_CONTAINER
//
// MessageText:
//
//  The requested operation is not allowed on an object under the system container.
//
#define ERROR_DS_DISALLOWED_IN_SYSTEM_CONTAINER 8615L

//
// MessageId: ERROR_DS_LDAP_SEND_QUEUE_FULL
//
// MessageText:
//
//  The LDAP servers network send queue has filled up because the client is not
//  processing the results of it's requests fast enough.  No more requests will
//  be processed until the client catches up.  If the client does not catch up
//  then it will be disconnected.
//
#define ERROR_DS_LDAP_SEND_QUEUE_FULL    8616L

//
// MessageId: ERROR_DS_DRA_OUT_SCHEDULE_WINDOW
//
// MessageText:
//
//  The scheduled replication did not take place because the system was too busy to execute the request within the schedule window.  The replication queue is overloaded. Consider reducing the number of partners or decreasing the scheduled replication frequency.
//
#define ERROR_DS_DRA_OUT_SCHEDULE_WINDOW 8617L

///////////////////////////////////////////////////
//                                                /
//     End of Active Directory Error Codes        /
//                                                /
//                  8000 to  8999                 /
///////////////////////////////////////////////////


///////////////////////////////////////////////////
//                                               //
//                  DNS Error Codes              //
//                                               //
//                   9000 to 9999                //
///////////////////////////////////////////////////

// =============================
// Facility DNS Error Messages
// =============================

//
//  DNS response codes.
//

#define DNS_ERROR_RESPONSE_CODES_BASE 9000

#define DNS_ERROR_RCODE_NO_ERROR NO_ERROR

#define DNS_ERROR_MASK 0x00002328 // 9000 or DNS_ERROR_RESPONSE_CODES_BASE

// DNS_ERROR_RCODE_FORMAT_ERROR          0x00002329
//
// MessageId: DNS_ERROR_RCODE_FORMAT_ERROR
//
// MessageText:
//
//  DNS server unable to interpret format.
//
#define DNS_ERROR_RCODE_FORMAT_ERROR     9001L

// DNS_ERROR_RCODE_SERVER_FAILURE        0x0000232a
//
// MessageId: DNS_ERROR_RCODE_SERVER_FAILURE
//
// MessageText:
//
//  DNS server failure.
//
#define DNS_ERROR_RCODE_SERVER_FAILURE   9002L

// DNS_ERROR_RCODE_NAME_ERROR            0x0000232b
//
// MessageId: DNS_ERROR_RCODE_NAME_ERROR
//
// MessageText:
//
//  DNS name does not exist.
//
#define DNS_ERROR_RCODE_NAME_ERROR       9003L

// DNS_ERROR_RCODE_NOT_IMPLEMENTED       0x0000232c
//
// MessageId: DNS_ERROR_RCODE_NOT_IMPLEMENTED
//
// MessageText:
//
//  DNS request not supported by name server.
//
#define DNS_ERROR_RCODE_NOT_IMPLEMENTED  9004L

// DNS_ERROR_RCODE_REFUSED               0x0000232d
//
// MessageId: DNS_ERROR_RCODE_REFUSED
//
// MessageText:
//
//  DNS operation refused.
//
#define DNS_ERROR_RCODE_REFUSED          9005L

// DNS_ERROR_RCODE_YXDOMAIN              0x0000232e
//
// MessageId: DNS_ERROR_RCODE_YXDOMAIN
//
// MessageText:
//
//  DNS name that ought not exist, does exist.
//
#define DNS_ERROR_RCODE_YXDOMAIN         9006L

// DNS_ERROR_RCODE_YXRRSET               0x0000232f
//
// MessageId: DNS_ERROR_RCODE_YXRRSET
//
// MessageText:
//
//  DNS RR set that ought not exist, does exist.
//
#define DNS_ERROR_RCODE_YXRRSET          9007L

// DNS_ERROR_RCODE_NXRRSET               0x00002330
//
// MessageId: DNS_ERROR_RCODE_NXRRSET
//
// MessageText:
//
//  DNS RR set that ought to exist, does not exist.
//
#define DNS_ERROR_RCODE_NXRRSET          9008L

// DNS_ERROR_RCODE_NOTAUTH               0x00002331
//
// MessageId: DNS_ERROR_RCODE_NOTAUTH
//
// MessageText:
//
//  DNS server not authoritative for zone.
//
#define DNS_ERROR_RCODE_NOTAUTH          9009L

// DNS_ERROR_RCODE_NOTZONE               0x00002332
//
// MessageId: DNS_ERROR_RCODE_NOTZONE
//
// MessageText:
//
//  DNS name in update or prereq is not in zone.
//
#define DNS_ERROR_RCODE_NOTZONE          9010L

// DNS_ERROR_RCODE_BADSIG                0x00002338
//
// MessageId: DNS_ERROR_RCODE_BADSIG
//
// MessageText:
//
//  DNS signature failed to verify.
//
#define DNS_ERROR_RCODE_BADSIG           9016L

// DNS_ERROR_RCODE_BADKEY                0x00002339
//
// MessageId: DNS_ERROR_RCODE_BADKEY
//
// MessageText:
//
//  DNS bad key.
//
#define DNS_ERROR_RCODE_BADKEY           9017L

// DNS_ERROR_RCODE_BADTIME               0x0000233a
//
// MessageId: DNS_ERROR_RCODE_BADTIME
//
// MessageText:
//
//  DNS signature validity expired.
//
#define DNS_ERROR_RCODE_BADTIME          9018L

#define DNS_ERROR_RCODE_LAST DNS_ERROR_RCODE_BADTIME


//
//  Packet format
//

#define DNS_ERROR_PACKET_FMT_BASE 9500

// DNS_INFO_NO_RECORDS                   0x0000251d
//
// MessageId: DNS_INFO_NO_RECORDS
//
// MessageText:
//
//  No records found for given DNS query.
//
#define DNS_INFO_NO_RECORDS              9501L

// DNS_ERROR_BAD_PACKET                  0x0000251e
//
// MessageId: DNS_ERROR_BAD_PACKET
//
// MessageText:
//
//  Bad DNS packet.
//
#define DNS_ERROR_BAD_PACKET             9502L

// DNS_ERROR_NO_PACKET                   0x0000251f
//
// MessageId: DNS_ERROR_NO_PACKET
//
// MessageText:
//
//  No DNS packet.
//
#define DNS_ERROR_NO_PACKET              9503L

// DNS_ERROR_RCODE                       0x00002520
//
// MessageId: DNS_ERROR_RCODE
//
// MessageText:
//
//  DNS error, check rcode.
//
#define DNS_ERROR_RCODE                  9504L

// DNS_ERROR_UNSECURE_PACKET             0x00002521
//
// MessageId: DNS_ERROR_UNSECURE_PACKET
//
// MessageText:
//
//  Unsecured DNS packet.
//
#define DNS_ERROR_UNSECURE_PACKET        9505L

#define DNS_STATUS_PACKET_UNSECURE DNS_ERROR_UNSECURE_PACKET


//
//  General API errors
//

#define DNS_ERROR_NO_MEMORY            ERROR_OUTOFMEMORY
#define DNS_ERROR_INVALID_NAME         ERROR_INVALID_NAME
#define DNS_ERROR_INVALID_DATA         ERROR_INVALID_DATA

#define DNS_ERROR_GENERAL_API_BASE 9550

// DNS_ERROR_INVALID_TYPE                0x0000254f
//
// MessageId: DNS_ERROR_INVALID_TYPE
//
// MessageText:
//
//  Invalid DNS type.
//
#define DNS_ERROR_INVALID_TYPE           9551L

// DNS_ERROR_INVALID_IP_ADDRESS          0x00002550
//
// MessageId: DNS_ERROR_INVALID_IP_ADDRESS
//
// MessageText:
//
//  Invalid IP address.
//
#define DNS_ERROR_INVALID_IP_ADDRESS     9552L

// DNS_ERROR_INVALID_PROPERTY            0x00002551
//
// MessageId: DNS_ERROR_INVALID_PROPERTY
//
// MessageText:
//
//  Invalid property.
//
#define DNS_ERROR_INVALID_PROPERTY       9553L

// DNS_ERROR_TRY_AGAIN_LATER             0x00002552
//
// MessageId: DNS_ERROR_TRY_AGAIN_LATER
//
// MessageText:
//
//  Try DNS operation again later.
//
#define DNS_ERROR_TRY_AGAIN_LATER        9554L

// DNS_ERROR_NOT_UNIQUE                  0x00002553
//
// MessageId: DNS_ERROR_NOT_UNIQUE
//
// MessageText:
//
//  Record for given name and type is not unique.
//
#define DNS_ERROR_NOT_UNIQUE             9555L

// DNS_ERROR_NON_RFC_NAME                0x00002554
//
// MessageId: DNS_ERROR_NON_RFC_NAME
//
// MessageText:
//
//  DNS name does not comply with RFC specifications.
//
#define DNS_ERROR_NON_RFC_NAME           9556L

// DNS_STATUS_FQDN                       0x00002555
//
// MessageId: DNS_STATUS_FQDN
//
// MessageText:
//
//  DNS name is a fully-qualified DNS name.
//
#define DNS_STATUS_FQDN                  9557L

// DNS_STATUS_DOTTED_NAME                0x00002556
//
// MessageId: DNS_STATUS_DOTTED_NAME
//
// MessageText:
//
//  DNS name is dotted (multi-label).
//
#define DNS_STATUS_DOTTED_NAME           9558L

// DNS_STATUS_SINGLE_PART_NAME           0x00002557
//
// MessageId: DNS_STATUS_SINGLE_PART_NAME
//
// MessageText:
//
//  DNS name is a single-part name.
//
#define DNS_STATUS_SINGLE_PART_NAME      9559L

// DNS_ERROR_INVALID_NAME_CHAR           0x00002558
//
// MessageId: DNS_ERROR_INVALID_NAME_CHAR
//
// MessageText:
//
//  DNS name contains an invalid character.
//
#define DNS_ERROR_INVALID_NAME_CHAR      9560L

// DNS_ERROR_NUMERIC_NAME                0x00002559
//
// MessageId: DNS_ERROR_NUMERIC_NAME
//
// MessageText:
//
//  DNS name is entirely numeric.
//
#define DNS_ERROR_NUMERIC_NAME           9561L

// DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER  0x0000255A
//
// MessageId: DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER
//
// MessageText:
//
//  The operation requested is not permitted on a DNS root server.
//
#define DNS_ERROR_NOT_ALLOWED_ON_ROOT_SERVER 9562L

// DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION  0x0000255B
//
// MessageId: DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION
//
// MessageText:
//
//  The record could not be created because this part of the DNS namespace has
//  been delegated to another server.
//
#define DNS_ERROR_NOT_ALLOWED_UNDER_DELEGATION 9563L

// DNS_ERROR_CANNOT_FIND_ROOT_HINTS  0x0000255C
//
// MessageId: DNS_ERROR_CANNOT_FIND_ROOT_HINTS
//
// MessageText:
//
//  The DNS server could not find a set of root hints.
//
#define DNS_ERROR_CANNOT_FIND_ROOT_HINTS 9564L

// DNS_ERROR_INCONSISTENT_ROOT_HINTS  0x0000255D
//
// MessageId: DNS_ERROR_INCONSISTENT_ROOT_HINTS
//
// MessageText:
//
//  The DNS server found root hints but they were not consistent across
//  all adapters.
//
#define DNS_ERROR_INCONSISTENT_ROOT_HINTS 9565L


//
//  Zone errors
//

#define DNS_ERROR_ZONE_BASE 9600

// DNS_ERROR_ZONE_DOES_NOT_EXIST         0x00002581
//
// MessageId: DNS_ERROR_ZONE_DOES_NOT_EXIST
//
// MessageText:
//
//  DNS zone does not exist.
//
#define DNS_ERROR_ZONE_DOES_NOT_EXIST    9601L

// DNS_ERROR_NO_ZONE_INFO                0x00002582
//
// MessageId: DNS_ERROR_NO_ZONE_INFO
//
// MessageText:
//
//  DNS zone information not available.
//
#define DNS_ERROR_NO_ZONE_INFO           9602L

// DNS_ERROR_INVALID_ZONE_OPERATION      0x00002583
//
// MessageId: DNS_ERROR_INVALID_ZONE_OPERATION
//
// MessageText:
//
//  Invalid operation for DNS zone.
//
#define DNS_ERROR_INVALID_ZONE_OPERATION 9603L

// DNS_ERROR_ZONE_CONFIGURATION_ERROR    0x00002584
//
// MessageId: DNS_ERROR_ZONE_CONFIGURATION_ERROR
//
// MessageText:
//
//  Invalid DNS zone configuration.
//
#define DNS_ERROR_ZONE_CONFIGURATION_ERROR 9604L

// DNS_ERROR_ZONE_HAS_NO_SOA_RECORD      0x00002585
//
// MessageId: DNS_ERROR_ZONE_HAS_NO_SOA_RECORD
//
// MessageText:
//
//  DNS zone has no start of authority (SOA) record.
//
#define DNS_ERROR_ZONE_HAS_NO_SOA_RECORD 9605L

// DNS_ERROR_ZONE_HAS_NO_NS_RECORDS      0x00002586
//
// MessageId: DNS_ERROR_ZONE_HAS_NO_NS_RECORDS
//
// MessageText:
//
//  DNS zone has no Name Server (NS) record.
//
#define DNS_ERROR_ZONE_HAS_NO_NS_RECORDS 9606L

// DNS_ERROR_ZONE_LOCKED                 0x00002587
//
// MessageId: DNS_ERROR_ZONE_LOCKED
//
// MessageText:
//
//  DNS zone is locked.
//
#define DNS_ERROR_ZONE_LOCKED            9607L

// DNS_ERROR_ZONE_CREATION_FAILED        0x00002588
//
// MessageId: DNS_ERROR_ZONE_CREATION_FAILED
//
// MessageText:
//
//  DNS zone creation failed.
//
#define DNS_ERROR_ZONE_CREATION_FAILED   9608L

// DNS_ERROR_ZONE_ALREADY_EXISTS         0x00002589
//
// MessageId: DNS_ERROR_ZONE_ALREADY_EXISTS
//
// MessageText:
//
//  DNS zone already exists.
//
#define DNS_ERROR_ZONE_ALREADY_EXISTS    9609L

// DNS_ERROR_AUTOZONE_ALREADY_EXISTS     0x0000258a
//
// MessageId: DNS_ERROR_AUTOZONE_ALREADY_EXISTS
//
// MessageText:
//
//  DNS automatic zone already exists.
//
#define DNS_ERROR_AUTOZONE_ALREADY_EXISTS 9610L

// DNS_ERROR_INVALID_ZONE_TYPE           0x0000258b
//
// MessageId: DNS_ERROR_INVALID_ZONE_TYPE
//
// MessageText:
//
//  Invalid DNS zone type.
//
#define DNS_ERROR_INVALID_ZONE_TYPE      9611L

// DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP 0x0000258c
//
// MessageId: DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP
//
// MessageText:
//
//  Secondary DNS zone requires master IP address.
//
#define DNS_ERROR_SECONDARY_REQUIRES_MASTER_IP 9612L

// DNS_ERROR_ZONE_NOT_SECONDARY          0x0000258d
//
// MessageId: DNS_ERROR_ZONE_NOT_SECONDARY
//
// MessageText:
//
//  DNS zone not secondary.
//
#define DNS_ERROR_ZONE_NOT_SECONDARY     9613L

// DNS_ERROR_NEED_SECONDARY_ADDRESSES    0x0000258e
//
// MessageId: DNS_ERROR_NEED_SECONDARY_ADDRESSES
//
// MessageText:
//
//  Need secondary IP address.
//
#define DNS_ERROR_NEED_SECONDARY_ADDRESSES 9614L

// DNS_ERROR_WINS_INIT_FAILED            0x0000258f
//
// MessageId: DNS_ERROR_WINS_INIT_FAILED
//
// MessageText:
//
//  WINS initialization failed.
//
#define DNS_ERROR_WINS_INIT_FAILED       9615L

// DNS_ERROR_NEED_WINS_SERVERS           0x00002590
//
// MessageId: DNS_ERROR_NEED_WINS_SERVERS
//
// MessageText:
//
//  Need WINS servers.
//
#define DNS_ERROR_NEED_WINS_SERVERS      9616L

// DNS_ERROR_NBSTAT_INIT_FAILED          0x00002591
//
// MessageId: DNS_ERROR_NBSTAT_INIT_FAILED
//
// MessageText:
//
//  NBTSTAT initialization call failed.
//
#define DNS_ERROR_NBSTAT_INIT_FAILED     9617L

// DNS_ERROR_SOA_DELETE_INVALID          0x00002592
//
// MessageId: DNS_ERROR_SOA_DELETE_INVALID
//
// MessageText:
//
//  Invalid delete of start of authority (SOA)
//
#define DNS_ERROR_SOA_DELETE_INVALID     9618L

// DNS_ERROR_FORWARDER_ALREADY_EXISTS    0x00002593
//
// MessageId: DNS_ERROR_FORWARDER_ALREADY_EXISTS
//
// MessageText:
//
//  A conditional forwarding zone already exists for that name.
//
#define DNS_ERROR_FORWARDER_ALREADY_EXISTS 9619L

// DNS_ERROR_ZONE_REQUIRES_MASTER_IP     0x00002594
//
// MessageId: DNS_ERROR_ZONE_REQUIRES_MASTER_IP
//
// MessageText:
//
//  This zone must be configured with one or more master DNS server IP addresses.
//
#define DNS_ERROR_ZONE_REQUIRES_MASTER_IP 9620L

// DNS_ERROR_ZONE_IS_SHUTDOWN            0x00002595
//
// MessageId: DNS_ERROR_ZONE_IS_SHUTDOWN
//
// MessageText:
//
//  The operation cannot be performed because this zone is shutdown.
//
#define DNS_ERROR_ZONE_IS_SHUTDOWN       9621L


//
//  Datafile errors
//

#define DNS_ERROR_DATAFILE_BASE 9650

// DNS                                   0x000025b3
//
// MessageId: DNS_ERROR_PRIMARY_REQUIRES_DATAFILE
//
// MessageText:
//
//  Primary DNS zone requires datafile.
//
#define DNS_ERROR_PRIMARY_REQUIRES_DATAFILE 9651L

// DNS                                   0x000025b4
//
// MessageId: DNS_ERROR_INVALID_DATAFILE_NAME
//
// MessageText:
//
//  Invalid datafile name for DNS zone.
//
#define DNS_ERROR_INVALID_DATAFILE_NAME  9652L

// DNS                                   0x000025b5
//
// MessageId: DNS_ERROR_DATAFILE_OPEN_FAILURE
//
// MessageText:
//
//  Failed to open datafile for DNS zone.
//
#define DNS_ERROR_DATAFILE_OPEN_FAILURE  9653L

// DNS                                   0x000025b6
//
// MessageId: DNS_ERROR_FILE_WRITEBACK_FAILED
//
// MessageText:
//
//  Failed to write datafile for DNS zone.
//
#define DNS_ERROR_FILE_WRITEBACK_FAILED  9654L

// DNS                                   0x000025b7
//
// MessageId: DNS_ERROR_DATAFILE_PARSING
//
// MessageText:
//
//  Failure while reading datafile for DNS zone.
//
#define DNS_ERROR_DATAFILE_PARSING       9655L


//
//  Database errors
//

#define DNS_ERROR_DATABASE_BASE 9700

// DNS_ERROR_RECORD_DOES_NOT_EXIST       0x000025e5
//
// MessageId: DNS_ERROR_RECORD_DOES_NOT_EXIST
//
// MessageText:
//
//  DNS record does not exist.
//
#define DNS_ERROR_RECORD_DOES_NOT_EXIST  9701L

// DNS_ERROR_RECORD_FORMAT               0x000025e6
//
// MessageId: DNS_ERROR_RECORD_FORMAT
//
// MessageText:
//
//  DNS record format error.
//
#define DNS_ERROR_RECORD_FORMAT          9702L

// DNS_ERROR_NODE_CREATION_FAILED        0x000025e7
//
// MessageId: DNS_ERROR_NODE_CREATION_FAILED
//
// MessageText:
//
//  Node creation failure in DNS.
//
#define DNS_ERROR_NODE_CREATION_FAILED   9703L

// DNS_ERROR_UNKNOWN_RECORD_TYPE         0x000025e8
//
// MessageId: DNS_ERROR_UNKNOWN_RECORD_TYPE
//
// MessageText:
//
//  Unknown DNS record type.
//
#define DNS_ERROR_UNKNOWN_RECORD_TYPE    9704L

// DNS_ERROR_RECORD_TIMED_OUT            0x000025e9
//
// MessageId: DNS_ERROR_RECORD_TIMED_OUT
//
// MessageText:
//
//  DNS record timed out.
//
#define DNS_ERROR_RECORD_TIMED_OUT       9705L

// DNS_ERROR_NAME_NOT_IN_ZONE            0x000025ea
//
// MessageId: DNS_ERROR_NAME_NOT_IN_ZONE
//
// MessageText:
//
//  Name not in DNS zone.
//
#define DNS_ERROR_NAME_NOT_IN_ZONE       9706L

// DNS_ERROR_CNAME_LOOP                  0x000025eb
//
// MessageId: DNS_ERROR_CNAME_LOOP
//
// MessageText:
//
//  CNAME loop detected.
//
#define DNS_ERROR_CNAME_LOOP             9707L

// DNS_ERROR_NODE_IS_CNAME               0x000025ec
//
// MessageId: DNS_ERROR_NODE_IS_CNAME
//
// MessageText:
//
//  Node is a CNAME DNS record.
//
#define DNS_ERROR_NODE_IS_CNAME          9708L

// DNS_ERROR_CNAME_COLLISION             0x000025ed
//
// MessageId: DNS_ERROR_CNAME_COLLISION
//
// MessageText:
//
//  A CNAME record already exists for given name.
//
#define DNS_ERROR_CNAME_COLLISION        9709L

// DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT    0x000025ee
//
// MessageId: DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT
//
// MessageText:
//
//  Record only at DNS zone root.
//
#define DNS_ERROR_RECORD_ONLY_AT_ZONE_ROOT 9710L

// DNS_ERROR_RECORD_ALREADY_EXISTS       0x000025ef
//
// MessageId: DNS_ERROR_RECORD_ALREADY_EXISTS
//
// MessageText:
//
//  DNS record already exists.
//
#define DNS_ERROR_RECORD_ALREADY_EXISTS  9711L

// DNS_ERROR_SECONDARY_DATA              0x000025f0
//
// MessageId: DNS_ERROR_SECONDARY_DATA
//
// MessageText:
//
//  Secondary DNS zone data error.
//
#define DNS_ERROR_SECONDARY_DATA         9712L

// DNS_ERROR_NO_CREATE_CACHE_DATA        0x000025f1
//
// MessageId: DNS_ERROR_NO_CREATE_CACHE_DATA
//
// MessageText:
//
//  Could not create DNS cache data.
//
#define DNS_ERROR_NO_CREATE_CACHE_DATA   9713L

// DNS_ERROR_NAME_DOES_NOT_EXIST         0x000025f2
//
// MessageId: DNS_ERROR_NAME_DOES_NOT_EXIST
//
// MessageText:
//
//  DNS name does not exist.
//
#define DNS_ERROR_NAME_DOES_NOT_EXIST    9714L

// DNS_WARNING_PTR_CREATE_FAILED         0x000025f3
//
// MessageId: DNS_WARNING_PTR_CREATE_FAILED
//
// MessageText:
//
//  Could not create pointer (PTR) record.
//
#define DNS_WARNING_PTR_CREATE_FAILED    9715L

// DNS_WARNING_DOMAIN_UNDELETED          0x000025f4
//
// MessageId: DNS_WARNING_DOMAIN_UNDELETED
//
// MessageText:
//
//  DNS domain was undeleted.
//
#define DNS_WARNING_DOMAIN_UNDELETED     9716L

// DNS_ERROR_DS_UNAVAILABLE              0x000025f5
//
// MessageId: DNS_ERROR_DS_UNAVAILABLE
//
// MessageText:
//
//  The directory service is unavailable.
//
#define DNS_ERROR_DS_UNAVAILABLE         9717L

// DNS_ERROR_DS_ZONE_ALREADY_EXISTS      0x000025f6
//
// MessageId: DNS_ERROR_DS_ZONE_ALREADY_EXISTS
//
// MessageText:
//
//  DNS zone already exists in the directory service.
//
#define DNS_ERROR_DS_ZONE_ALREADY_EXISTS 9718L

// DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE      0x000025f7
//
// MessageId: DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE
//
// MessageText:
//
//  DNS server not creating or reading the boot file for the directory service integrated DNS zone.
//
#define DNS_ERROR_NO_BOOTFILE_IF_DS_ZONE 9719L


//
//  Operation errors
//

#define DNS_ERROR_OPERATION_BASE 9750

// DNS_INFO_AXFR_COMPLETE                0x00002617
//
// MessageId: DNS_INFO_AXFR_COMPLETE
//
// MessageText:
//
//  DNS AXFR (zone transfer) complete.
//
#define DNS_INFO_AXFR_COMPLETE           9751L

// DNS_ERROR_AXFR                        0x00002618
//
// MessageId: DNS_ERROR_AXFR
//
// MessageText:
//
//  DNS zone transfer failed.
//
#define DNS_ERROR_AXFR                   9752L

// DNS_INFO_ADDED_LOCAL_WINS             0x00002619
//
// MessageId: DNS_INFO_ADDED_LOCAL_WINS
//
// MessageText:
//
//  Added local WINS server.
//
#define DNS_INFO_ADDED_LOCAL_WINS        9753L


//
//  Secure update
//

#define DNS_ERROR_SECURE_BASE 9800

// DNS_STATUS_CONTINUE_NEEDED            0x00002649
//
// MessageId: DNS_STATUS_CONTINUE_NEEDED
//
// MessageText:
//
//  Secure update call needs to continue update request.
//
#define DNS_STATUS_CONTINUE_NEEDED       9801L


//
//  Setup errors
//

#define DNS_ERROR_SETUP_BASE 9850

// DNS_ERROR_NO_TCPIP                    0x0000267b
//
// MessageId: DNS_ERROR_NO_TCPIP
//
// MessageText:
//
//  TCP/IP network protocol not installed.
//
#define DNS_ERROR_NO_TCPIP               9851L

// DNS_ERROR_NO_DNS_SERVERS              0x0000267c
//
// MessageId: DNS_ERROR_NO_DNS_SERVERS
//
// MessageText:
//
//  No DNS servers configured for local system.
//
#define DNS_ERROR_NO_DNS_SERVERS         9852L


//
//  Directory partition (DP) errors
//

#define DNS_ERROR_DP_BASE 9900

// DNS_ERROR_DP_DOES_NOT_EXIST           0x000026ad
//
// MessageId: DNS_ERROR_DP_DOES_NOT_EXIST
//
// MessageText:
//
//  The specified directory partition does not exist.
//
#define DNS_ERROR_DP_DOES_NOT_EXIST      9901L

// DNS_ERROR_DP_ALREADY_EXISTS           0x000026ae
//
// MessageId: DNS_ERROR_DP_ALREADY_EXISTS
//
// MessageText:
//
//  The specified directory partition already exists.
//
#define DNS_ERROR_DP_ALREADY_EXISTS      9902L

// DNS_ERROR_DP_NOT_ENLISTED             0x000026af
//
// MessageId: DNS_ERROR_DP_NOT_ENLISTED
//
// MessageText:
//
//  This DNS server is not enlisted in the specified directory partition.
//
#define DNS_ERROR_DP_NOT_ENLISTED        9903L

// DNS_ERROR_DP_ALREADY_ENLISTED         0x000026b0
//
// MessageId: DNS_ERROR_DP_ALREADY_ENLISTED
//
// MessageText:
//
//  This DNS server is already enlisted in the specified directory partition.
//
#define DNS_ERROR_DP_ALREADY_ENLISTED    9904L

// DNS_ERROR_DP_NOT_AVAILABLE            0x000026b1
//
// MessageId: DNS_ERROR_DP_NOT_AVAILABLE
//
// MessageText:
//
//  The directory partition is not available at this time. Please wait
//  a few minutes and try again.
//
#define DNS_ERROR_DP_NOT_AVAILABLE       9905L

// DNS_ERROR_DP_FSMO_ERROR               0x000026b2
//
// MessageId: DNS_ERROR_DP_FSMO_ERROR
//
// MessageText:
//
//  The application directory partition operation failed. The domain controller
//  holding the domain naming master role is down or unable to service the
//  request or is not running Windows Server 2003.
//
#define DNS_ERROR_DP_FSMO_ERROR          9906L

///////////////////////////////////////////////////
//                                               //
//             End of DNS Error Codes            //
//                                               //
//                  9000 to 9999                 //
///////////////////////////////////////////////////


///////////////////////////////////////////////////
//                                               //
//               WinSock Error Codes             //
//                                               //
//                 10000 to 11999                //
///////////////////////////////////////////////////

//
// WinSock error codes are also defined in WinSock.h
// and WinSock2.h, hence the IFDEF
//
#ifndef WSABASEERR
#define WSABASEERR 10000
//
// MessageId: WSAEINTR
//
// MessageText:
//
//  A blocking operation was interrupted by a call to WSACancelBlockingCall.
//
#define WSAEINTR                         10004L

//
// MessageId: WSAEBADF
//
// MessageText:
//
//  The file handle supplied is not valid.
//
#define WSAEBADF                         10009L

//
// MessageId: WSAEACCES
//
// MessageText:
//
//  An attempt was made to access a socket in a way forbidden by its access permissions.
//
#define WSAEACCES                        10013L

//
// MessageId: WSAEFAULT
//
// MessageText:
//
//  The system detected an invalid pointer address in attempting to use a pointer argument in a call.
//
#define WSAEFAULT                        10014L

//
// MessageId: WSAEINVAL
//
// MessageText:
//
//  An invalid argument was supplied.
//
#define WSAEINVAL                        10022L

//
// MessageId: WSAEMFILE
//
// MessageText:
//
//  Too many open sockets.
//
#define WSAEMFILE                        10024L

//
// MessageId: WSAEWOULDBLOCK
//
// MessageText:
//
//  A non-blocking socket operation could not be completed immediately.
//
#define WSAEWOULDBLOCK                   10035L

//
// MessageId: WSAEINPROGRESS
//
// MessageText:
//
//  A blocking operation is currently executing.
//
#define WSAEINPROGRESS                   10036L

//
// MessageId: WSAEALREADY
//
// MessageText:
//
//  An operation was attempted on a non-blocking socket that already had an operation in progress.
//
#define WSAEALREADY                      10037L

//
// MessageId: WSAENOTSOCK
//
// MessageText:
//
//  An operation was attempted on something that is not a socket.
//
#define WSAENOTSOCK                      10038L

//
// MessageId: WSAEDESTADDRREQ
//
// MessageText:
//
//  A required address was omitted from an operation on a socket.
//
#define WSAEDESTADDRREQ                  10039L

//
// MessageId: WSAEMSGSIZE
//
// MessageText:
//
//  A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself.
//
#define WSAEMSGSIZE                      10040L

//
// MessageId: WSAEPROTOTYPE
//
// MessageText:
//
//  A protocol was specified in the socket function call that does not support the semantics of the socket type requested.
//
#define WSAEPROTOTYPE                    10041L

//
// MessageId: WSAENOPROTOOPT
//
// MessageText:
//
//  An unknown, invalid, or unsupported option or level was specified in a getsockopt or setsockopt call.
//
#define WSAENOPROTOOPT                   10042L

//
// MessageId: WSAEPROTONOSUPPORT
//
// MessageText:
//
//  The requested protocol has not been configured into the system, or no implementation for it exists.
//
#define WSAEPROTONOSUPPORT               10043L

//
// MessageId: WSAESOCKTNOSUPPORT
//
// MessageText:
//
//  The support for the specified socket type does not exist in this address family.
//
#define WSAESOCKTNOSUPPORT               10044L

//
// MessageId: WSAEOPNOTSUPP
//
// MessageText:
//
//  The attempted operation is not supported for the type of object referenced.
//
#define WSAEOPNOTSUPP                    10045L

//
// MessageId: WSAEPFNOSUPPORT
//
// MessageText:
//
//  The protocol family has not been configured into the system or no implementation for it exists.
//
#define WSAEPFNOSUPPORT                  10046L

//
// MessageId: WSAEAFNOSUPPORT
//
// MessageText:
//
//  An address incompatible with the requested protocol was used.
//
#define WSAEAFNOSUPPORT                  10047L

//
// MessageId: WSAEADDRINUSE
//
// MessageText:
//
//  Only one usage of each socket address (protocol/network address/port) is normally permitted.
//
#define WSAEADDRINUSE                    10048L

//
// MessageId: WSAEADDRNOTAVAIL
//
// MessageText:
//
//  The requested address is not valid in its context.
//
#define WSAEADDRNOTAVAIL                 10049L

//
// MessageId: WSAENETDOWN
//
// MessageText:
//
//  A socket operation encountered a dead network.
//
#define WSAENETDOWN                      10050L

//
// MessageId: WSAENETUNREACH
//
// MessageText:
//
//  A socket operation was attempted to an unreachable network.
//
#define WSAENETUNREACH                   10051L

//
// MessageId: WSAENETRESET
//
// MessageText:
//
//  The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress.
//
#define WSAENETRESET                     10052L

//
// MessageId: WSAECONNABORTED
//
// MessageText:
//
//  An established connection was aborted by the software in your host machine.
//
#define WSAECONNABORTED                  10053L

//
// MessageId: WSAECONNRESET
//
// MessageText:
//
//  An existing connection was forcibly closed by the remote host.
//
#define WSAECONNRESET                    10054L

//
// MessageId: WSAENOBUFS
//
// MessageText:
//
//  An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full.
//
#define WSAENOBUFS                       10055L

//
// MessageId: WSAEISCONN
//
// MessageText:
//
//  A connect request was made on an already connected socket.
//
#define WSAEISCONN                       10056L

//
// MessageId: WSAENOTCONN
//
// MessageText:
//
//  A request to send or receive data was disallowed because the socket is not connected and (when sending on a datagram socket using a sendto call) no address was supplied.
//
#define WSAENOTCONN                      10057L

//
// MessageId: WSAESHUTDOWN
//
// MessageText:
//
//  A request to send or receive data was disallowed because the socket had already been shut down in that direction with a previous shutdown call.
//
#define WSAESHUTDOWN                     10058L

//
// MessageId: WSAETOOMANYREFS
//
// MessageText:
//
//  Too many references to some kernel object.
//
#define WSAETOOMANYREFS                  10059L

//
// MessageId: WSAETIMEDOUT
//
// MessageText:
//
//  A connection attempt failed because the connected party did not properly respond after a period of time, or established connection failed because connected host has failed to respond.
//
#define WSAETIMEDOUT                     10060L

//
// MessageId: WSAECONNREFUSED
//
// MessageText:
//
//  No connection could be made because the target machine actively refused it.
//
#define WSAECONNREFUSED                  10061L

//
// MessageId: WSAELOOP
//
// MessageText:
//
//  Cannot translate name.
//
#define WSAELOOP                         10062L

//
// MessageId: WSAENAMETOOLONG
//
// MessageText:
//
//  Name component or name was too long.
//
#define WSAENAMETOOLONG                  10063L

//
// MessageId: WSAEHOSTDOWN
//
// MessageText:
//
//  A socket operation failed because the destination host was down.
//
#define WSAEHOSTDOWN                     10064L

//
// MessageId: WSAEHOSTUNREACH
//
// MessageText:
//
//  A socket operation was attempted to an unreachable host.
//
#define WSAEHOSTUNREACH                  10065L

//
// MessageId: WSAENOTEMPTY
//
// MessageText:
//
//  Cannot remove a directory that is not empty.
//
#define WSAENOTEMPTY                     10066L

//
// MessageId: WSAEPROCLIM
//
// MessageText:
//
//  A Windows Sockets implementation may have a limit on the number of applications that may use it simultaneously.
//
#define WSAEPROCLIM                      10067L

//
// MessageId: WSAEUSERS
//
// MessageText:
//
//  Ran out of quota.
//
#define WSAEUSERS                        10068L

//
// MessageId: WSAEDQUOT
//
// MessageText:
//
//  Ran out of disk quota.
//
#define WSAEDQUOT                        10069L

//
// MessageId: WSAESTALE
//
// MessageText:
//
//  File handle reference is no longer available.
//
#define WSAESTALE                        10070L

//
// MessageId: WSAEREMOTE
//
// MessageText:
//
//  Item is not available locally.
//
#define WSAEREMOTE                       10071L

//
// MessageId: WSASYSNOTREADY
//
// MessageText:
//
//  WSAStartup cannot function at this time because the underlying system it uses to provide network services is currently unavailable.
//
#define WSASYSNOTREADY                   10091L

//
// MessageId: WSAVERNOTSUPPORTED
//
// MessageText:
//
//  The Windows Sockets version requested is not supported.
//
#define WSAVERNOTSUPPORTED               10092L

//
// MessageId: WSANOTINITIALISED
//
// MessageText:
//
//  Either the application has not called WSAStartup, or WSAStartup failed.
//
#define WSANOTINITIALISED                10093L

//
// MessageId: WSAEDISCON
//
// MessageText:
//
//  Returned by WSARecv or WSARecvFrom to indicate the remote party has initiated a graceful shutdown sequence.
//
#define WSAEDISCON                       10101L

//
// MessageId: WSAENOMORE
//
// MessageText:
//
//  No more results can be returned by WSALookupServiceNext.
//
#define WSAENOMORE                       10102L

//
// MessageId: WSAECANCELLED
//
// MessageText:
//
//  A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
//
#define WSAECANCELLED                    10103L

//
// MessageId: WSAEINVALIDPROCTABLE
//
// MessageText:
//
//  The procedure call table is invalid.
//
#define WSAEINVALIDPROCTABLE             10104L

//
// MessageId: WSAEINVALIDPROVIDER
//
// MessageText:
//
//  The requested service provider is invalid.
//
#define WSAEINVALIDPROVIDER              10105L

//
// MessageId: WSAEPROVIDERFAILEDINIT
//
// MessageText:
//
//  The requested service provider could not be loaded or initialized.
//
#define WSAEPROVIDERFAILEDINIT           10106L

//
// MessageId: WSASYSCALLFAILURE
//
// MessageText:
//
//  A system call that should never fail has failed.
//
#define WSASYSCALLFAILURE                10107L

//
// MessageId: WSASERVICE_NOT_FOUND
//
// MessageText:
//
//  No such service is known. The service cannot be found in the specified name space.
//
#define WSASERVICE_NOT_FOUND             10108L

//
// MessageId: WSATYPE_NOT_FOUND
//
// MessageText:
//
//  The specified class was not found.
//
#define WSATYPE_NOT_FOUND                10109L

//
// MessageId: WSA_E_NO_MORE
//
// MessageText:
//
//  No more results can be returned by WSALookupServiceNext.
//
#define WSA_E_NO_MORE                    10110L

//
// MessageId: WSA_E_CANCELLED
//
// MessageText:
//
//  A call to WSALookupServiceEnd was made while this call was still processing. The call has been canceled.
//
#define WSA_E_CANCELLED                  10111L

//
// MessageId: WSAEREFUSED
//
// MessageText:
//
//  A database query failed because it was actively refused.
//
#define WSAEREFUSED                      10112L

//
// MessageId: WSAHOST_NOT_FOUND
//
// MessageText:
//
//  No such host is known.
//
#define WSAHOST_NOT_FOUND                11001L

//
// MessageId: WSATRY_AGAIN
//
// MessageText:
//
//  This is usually a temporary error during hostname resolution and means that the local server did not receive a response from an authoritative server.
//
#define WSATRY_AGAIN                     11002L

//
// MessageId: WSANO_RECOVERY
//
// MessageText:
//
//  A non-recoverable error occurred during a database lookup.
//
#define WSANO_RECOVERY                   11003L

//
// MessageId: WSANO_DATA
//
// MessageText:
//
//  The requested name is valid, but no data of the requested type was found.
//
#define WSANO_DATA                       11004L

//
// MessageId: WSA_QOS_RECEIVERS
//
// MessageText:
//
//  At least one reserve has arrived.
//
#define WSA_QOS_RECEIVERS                11005L

//
// MessageId: WSA_QOS_SENDERS
//
// MessageText:
//
//  At least one path has arrived.
//
#define WSA_QOS_SENDERS                  11006L

//
// MessageId: WSA_QOS_NO_SENDERS
//
// MessageText:
//
//  There are no senders.
//
#define WSA_QOS_NO_SENDERS               11007L

//
// MessageId: WSA_QOS_NO_RECEIVERS
//
// MessageText:
//
//  There are no receivers.
//
#define WSA_QOS_NO_RECEIVERS             11008L

//
// MessageId: WSA_QOS_REQUEST_CONFIRMED
//
// MessageText:
//
//  Reserve has been confirmed.
//
#define WSA_QOS_REQUEST_CONFIRMED        11009L

//
// MessageId: WSA_QOS_ADMISSION_FAILURE
//
// MessageText:
//
//  Error due to lack of resources.
//
#define WSA_QOS_ADMISSION_FAILURE        11010L

//
// MessageId: WSA_QOS_POLICY_FAILURE
//
// MessageText:
//
//  Rejected for administrative reasons - bad credentials.
//
#define WSA_QOS_POLICY_FAILURE           11011L

//
// MessageId: WSA_QOS_BAD_STYLE
//
// MessageText:
//
//  Unknown or conflicting style.
//
#define WSA_QOS_BAD_STYLE                11012L

//
// MessageId: WSA_QOS_BAD_OBJECT
//
// MessageText:
//
//  Problem with some part of the filterspec or providerspecific buffer in general.
//
#define WSA_QOS_BAD_OBJECT               11013L

//
// MessageId: WSA_QOS_TRAFFIC_CTRL_ERROR
//
// MessageText:
//
//  Problem with some part of the flowspec.
//
#define WSA_QOS_TRAFFIC_CTRL_ERROR       11014L

//
// MessageId: WSA_QOS_GENERIC_ERROR
//
// MessageText:
//
//  General QOS error.
//
#define WSA_QOS_GENERIC_ERROR            11015L

//
// MessageId: WSA_QOS_ESERVICETYPE
//
// MessageText:
//
//  An invalid or unrecognized service type was found in the flowspec.
//
#define WSA_QOS_ESERVICETYPE             11016L

//
// MessageId: WSA_QOS_EFLOWSPEC
//
// MessageText:
//
//  An invalid or inconsistent flowspec was found in the QOS structure.
//
#define WSA_QOS_EFLOWSPEC                11017L

//
// MessageId: WSA_QOS_EPROVSPECBUF
//
// MessageText:
//
//  Invalid QOS provider-specific buffer.
//
#define WSA_QOS_EPROVSPECBUF             11018L

//
// MessageId: WSA_QOS_EFILTERSTYLE
//
// MessageText:
//
//  An invalid QOS filter style was used.
//
#define WSA_QOS_EFILTERSTYLE             11019L

//
// MessageId: WSA_QOS_EFILTERTYPE
//
// MessageText:
//
//  An invalid QOS filter type was used.
//
#define WSA_QOS_EFILTERTYPE              11020L

//
// MessageId: WSA_QOS_EFILTERCOUNT
//
// MessageText:
//
//  An incorrect number of QOS FILTERSPECs were specified in the FLOWDESCRIPTOR.
//
#define WSA_QOS_EFILTERCOUNT             11021L

//
// MessageId: WSA_QOS_EOBJLENGTH
//
// MessageText:
//
//  An object with an invalid ObjectLength field was specified in the QOS provider-specific buffer.
//
#define WSA_QOS_EOBJLENGTH               11022L

//
// MessageId: WSA_QOS_EFLOWCOUNT
//
// MessageText:
//
//  An incorrect number of flow descriptors was specified in the QOS structure.
//
#define WSA_QOS_EFLOWCOUNT               11023L

//
// MessageId: WSA_QOS_EUNKOWNPSOBJ
//
// MessageText:
//
//  An unrecognized object was found in the QOS provider-specific buffer.
//
#define WSA_QOS_EUNKOWNPSOBJ             11024L

//
// MessageId: WSA_QOS_EPOLICYOBJ
//
// MessageText:
//
//  An invalid policy object was found in the QOS provider-specific buffer.
//
#define WSA_QOS_EPOLICYOBJ               11025L

//
// MessageId: WSA_QOS_EFLOWDESC
//
// MessageText:
//
//  An invalid QOS flow descriptor was found in the flow descriptor list.
//
#define WSA_QOS_EFLOWDESC                11026L

//
// MessageId: WSA_QOS_EPSFLOWSPEC
//
// MessageText:
//
//  An invalid or inconsistent flowspec was found in the QOS provider specific buffer.
//
#define WSA_QOS_EPSFLOWSPEC              11027L

//
// MessageId: WSA_QOS_EPSFILTERSPEC
//
// MessageText:
//
//  An invalid FILTERSPEC was found in the QOS provider-specific buffer.
//
#define WSA_QOS_EPSFILTERSPEC            11028L

//
// MessageId: WSA_QOS_ESDMODEOBJ
//
// MessageText:
//
//  An invalid shape discard mode object was found in the QOS provider specific buffer.
//
#define WSA_QOS_ESDMODEOBJ               11029L

//
// MessageId: WSA_QOS_ESHAPERATEOBJ
//
// MessageText:
//
//  An invalid shaping rate object was found in the QOS provider-specific buffer.
//
#define WSA_QOS_ESHAPERATEOBJ            11030L

//
// MessageId: WSA_QOS_RESERVED_PETYPE
//
// MessageText:
//
//  A reserved policy element was found in the QOS provider-specific buffer.
//
#define WSA_QOS_RESERVED_PETYPE          11031L

#endif // defined(WSABASEERR)

///////////////////////////////////////////////////
//                                               //
//           End of WinSock Error Codes          //
//                                               //
//                 10000 to 11999                //
///////////////////////////////////////////////////



///////////////////////////////////////////////////
//                                               //
//             Side By Side Error Codes          //
//                                               //
//                 14000 to 14999                //
///////////////////////////////////////////////////

//
// MessageId: ERROR_SXS_SECTION_NOT_FOUND
//
// MessageText:
//
//  The requested section was not present in the activation context.
//
#define ERROR_SXS_SECTION_NOT_FOUND      14000L

//
// MessageId: ERROR_SXS_CANT_GEN_ACTCTX
//
// MessageText:
//
//  This application has failed to start because the application configuration is incorrect. Reinstalling the application may fix this problem.
//
#define ERROR_SXS_CANT_GEN_ACTCTX        14001L

//
// MessageId: ERROR_SXS_INVALID_ACTCTXDATA_FORMAT
//
// MessageText:
//
//  The application binding data format is invalid.
//
#define ERROR_SXS_INVALID_ACTCTXDATA_FORMAT 14002L

//
// MessageId: ERROR_SXS_ASSEMBLY_NOT_FOUND
//
// MessageText:
//
//  The referenced assembly is not installed on your system.
//
#define ERROR_SXS_ASSEMBLY_NOT_FOUND     14003L

//
// MessageId: ERROR_SXS_MANIFEST_FORMAT_ERROR
//
// MessageText:
//
//  The manifest file does not begin with the required tag and format information.
//
#define ERROR_SXS_MANIFEST_FORMAT_ERROR  14004L

//
// MessageId: ERROR_SXS_MANIFEST_PARSE_ERROR
//
// MessageText:
//
//  The manifest file contains one or more syntax errors.
//
#define ERROR_SXS_MANIFEST_PARSE_ERROR   14005L

//
// MessageId: ERROR_SXS_ACTIVATION_CONTEXT_DISABLED
//
// MessageText:
//
//  The application attempted to activate a disabled activation context.
//
#define ERROR_SXS_ACTIVATION_CONTEXT_DISABLED 14006L

//
// MessageId: ERROR_SXS_KEY_NOT_FOUND
//
// MessageText:
//
//  The requested lookup key was not found in any active activation context.
//
#define ERROR_SXS_KEY_NOT_FOUND          14007L

//
// MessageId: ERROR_SXS_VERSION_CONFLICT
//
// MessageText:
//
//  A component version required by the application conflicts with another component version already active.
//
#define ERROR_SXS_VERSION_CONFLICT       14008L

//
// MessageId: ERROR_SXS_WRONG_SECTION_TYPE
//
// MessageText:
//
//  The type requested activation context section does not match the query API used.
//
#define ERROR_SXS_WRONG_SECTION_TYPE     14009L

//
// MessageId: ERROR_SXS_THREAD_QUERIES_DISABLED
//
// MessageText:
//
//  Lack of system resources has required isolated activation to be disabled for the current thread of execution.
//
#define ERROR_SXS_THREAD_QUERIES_DISABLED 14010L

//
// MessageId: ERROR_SXS_PROCESS_DEFAULT_ALREADY_SET
//
// MessageText:
//
//  An attempt to set the process default activation context failed because the process default activation context was already set.
//
#define ERROR_SXS_PROCESS_DEFAULT_ALREADY_SET 14011L

//
// MessageId: ERROR_SXS_UNKNOWN_ENCODING_GROUP
//
// MessageText:
//
//  The encoding group identifier specified is not recognized.
//
#define ERROR_SXS_UNKNOWN_ENCODING_GROUP 14012L

//
// MessageId: ERROR_SXS_UNKNOWN_ENCODING
//
// MessageText:
//
//  The encoding requested is not recognized.
//
#define ERROR_SXS_UNKNOWN_ENCODING       14013L

//
// MessageId: ERROR_SXS_INVALID_XML_NAMESPACE_URI
//
// MessageText:
//
//  The manifest contains a reference to an invalid URI.
//
#define ERROR_SXS_INVALID_XML_NAMESPACE_URI 14014L

//
// MessageId: ERROR_SXS_ROOT_MANIFEST_DEPENDENCY_NOT_INSTALLED
//
// MessageText:
//
//  The application manifest contains a reference to a dependent assembly which is not installed
//
#define ERROR_SXS_ROOT_MANIFEST_DEPENDENCY_NOT_INSTALLED 14015L

//
// MessageId: ERROR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED
//
// MessageText:
//
//  The manifest for an assembly used by the application has a reference to a dependent assembly which is not installed
//
#define ERROR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED 14016L

//
// MessageId: ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE
//
// MessageText:
//
//  The manifest contains an attribute for the assembly identity which is not valid.
//
#define ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE 14017L

//
// MessageId: ERROR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE
//
// MessageText:
//
//  The manifest is missing the required default namespace specification on the assembly element.
//
#define ERROR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE 14018L

//
// MessageId: ERROR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE
//
// MessageText:
//
//  The manifest has a default namespace specified on the assembly element but its value is not "urn:schemas-microsoft-com:asm.v1".
//
#define ERROR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE 14019L

//
// MessageId: ERROR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT
//
// MessageText:
//
//  The private manifest probed has crossed reparse-point-associated path
//
#define ERROR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT 14020L

//
// MessageId: ERROR_SXS_DUPLICATE_DLL_NAME
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have files by the same name.
//
#define ERROR_SXS_DUPLICATE_DLL_NAME     14021L

//
// MessageId: ERROR_SXS_DUPLICATE_WINDOWCLASS_NAME
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have window classes with the same name.
//
#define ERROR_SXS_DUPLICATE_WINDOWCLASS_NAME 14022L

//
// MessageId: ERROR_SXS_DUPLICATE_CLSID
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have the same COM server CLSIDs.
//
#define ERROR_SXS_DUPLICATE_CLSID        14023L

//
// MessageId: ERROR_SXS_DUPLICATE_IID
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have proxies for the same COM interface IIDs.
//
#define ERROR_SXS_DUPLICATE_IID          14024L

//
// MessageId: ERROR_SXS_DUPLICATE_TLBID
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have the same COM type library TLBIDs.
//
#define ERROR_SXS_DUPLICATE_TLBID        14025L

//
// MessageId: ERROR_SXS_DUPLICATE_PROGID
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest have the same COM ProgIDs.
//
#define ERROR_SXS_DUPLICATE_PROGID       14026L

//
// MessageId: ERROR_SXS_DUPLICATE_ASSEMBLY_NAME
//
// MessageText:
//
//  Two or more components referenced directly or indirectly by the application manifest are different versions of the same component which is not permitted.
//
#define ERROR_SXS_DUPLICATE_ASSEMBLY_NAME 14027L

//
// MessageId: ERROR_SXS_FILE_HASH_MISMATCH
//
// MessageText:
//
//  A component's file does not match the verification information present in the
//  component manifest.
//
#define ERROR_SXS_FILE_HASH_MISMATCH     14028L

//
// MessageId: ERROR_SXS_POLICY_PARSE_ERROR
//
// MessageText:
//
//  The policy manifest contains one or more syntax errors.
//
#define ERROR_SXS_POLICY_PARSE_ERROR     14029L

//
// MessageId: ERROR_SXS_XML_E_MISSINGQUOTE
//
// MessageText:
//
//  Manifest Parse Error : A string literal was expected, but no opening quote character was found.
//
#define ERROR_SXS_XML_E_MISSINGQUOTE     14030L

//
// MessageId: ERROR_SXS_XML_E_COMMENTSYNTAX
//
// MessageText:
//
//  Manifest Parse Error : Incorrect syntax was used in a comment.
//
#define ERROR_SXS_XML_E_COMMENTSYNTAX    14031L

//
// MessageId: ERROR_SXS_XML_E_BADSTARTNAMECHAR
//
// MessageText:
//
//  Manifest Parse Error : A name was started with an invalid character.
//
#define ERROR_SXS_XML_E_BADSTARTNAMECHAR 14032L

//
// MessageId: ERROR_SXS_XML_E_BADNAMECHAR
//
// MessageText:
//
//  Manifest Parse Error : A name contained an invalid character.
//
#define ERROR_SXS_XML_E_BADNAMECHAR      14033L

//
// MessageId: ERROR_SXS_XML_E_BADCHARINSTRING
//
// MessageText:
//
//  Manifest Parse Error : A string literal contained an invalid character.
//
#define ERROR_SXS_XML_E_BADCHARINSTRING  14034L

//
// MessageId: ERROR_SXS_XML_E_XMLDECLSYNTAX
//
// MessageText:
//
//  Manifest Parse Error : Invalid syntax for an xml declaration.
//
#define ERROR_SXS_XML_E_XMLDECLSYNTAX    14035L

//
// MessageId: ERROR_SXS_XML_E_BADCHARDATA
//
// MessageText:
//
//  Manifest Parse Error : An Invalid character was found in text content.
//
#define ERROR_SXS_XML_E_BADCHARDATA      14036L

//
// MessageId: ERROR_SXS_XML_E_MISSINGWHITESPACE
//
// MessageText:
//
//  Manifest Parse Error : Required white space was missing.
//
#define ERROR_SXS_XML_E_MISSINGWHITESPACE 14037L

//
// MessageId: ERROR_SXS_XML_E_EXPECTINGTAGEND
//
// MessageText:
//
//  Manifest Parse Error : The character '>' was expected.
//
#define ERROR_SXS_XML_E_EXPECTINGTAGEND  14038L

//
// MessageId: ERROR_SXS_XML_E_MISSINGSEMICOLON
//
// MessageText:
//
//  Manifest Parse Error : A semi colon character was expected.
//
#define ERROR_SXS_XML_E_MISSINGSEMICOLON 14039L

//
// MessageId: ERROR_SXS_XML_E_UNBALANCEDPAREN
//
// MessageText:
//
//  Manifest Parse Error : Unbalanced parentheses.
//
#define ERROR_SXS_XML_E_UNBALANCEDPAREN  14040L

//
// MessageId: ERROR_SXS_XML_E_INTERNALERROR
//
// MessageText:
//
//  Manifest Parse Error : Internal error.
//
#define ERROR_SXS_XML_E_INTERNALERROR    14041L

//
// MessageId: ERROR_SXS_XML_E_UNEXPECTED_WHITESPACE
//
// MessageText:
//
//  Manifest Parse Error : Whitespace is not allowed at this location.
//
#define ERROR_SXS_XML_E_UNEXPECTED_WHITESPACE 14042L

//
// MessageId: ERROR_SXS_XML_E_INCOMPLETE_ENCODING
//
// MessageText:
//
//  Manifest Parse Error : End of file reached in invalid state for current encoding.
//
#define ERROR_SXS_XML_E_INCOMPLETE_ENCODING 14043L

//
// MessageId: ERROR_SXS_XML_E_MISSING_PAREN
//
// MessageText:
//
//  Manifest Parse Error : Missing parenthesis.
//
#define ERROR_SXS_XML_E_MISSING_PAREN    14044L

//
// MessageId: ERROR_SXS_XML_E_EXPECTINGCLOSEQUOTE
//
// MessageText:
//
//  Manifest Parse Error : A single or double closing quote character (\' or \") is missing.
//
#define ERROR_SXS_XML_E_EXPECTINGCLOSEQUOTE 14045L

//
// MessageId: ERROR_SXS_XML_E_MULTIPLE_COLONS
//
// MessageText:
//
//  Manifest Parse Error : Multiple colons are not allowed in a name.
//
#define ERROR_SXS_XML_E_MULTIPLE_COLONS  14046L

//
// MessageId: ERROR_SXS_XML_E_INVALID_DECIMAL
//
// MessageText:
//
//  Manifest Parse Error : Invalid character for decimal digit.
//
#define ERROR_SXS_XML_E_INVALID_DECIMAL  14047L

//
// MessageId: ERROR_SXS_XML_E_INVALID_HEXIDECIMAL
//
// MessageText:
//
//  Manifest Parse Error : Invalid character for hexidecimal digit.
//
#define ERROR_SXS_XML_E_INVALID_HEXIDECIMAL 14048L

//
// MessageId: ERROR_SXS_XML_E_INVALID_UNICODE
//
// MessageText:
//
//  Manifest Parse Error : Invalid unicode character value for this platform.
//
#define ERROR_SXS_XML_E_INVALID_UNICODE  14049L

//
// MessageId: ERROR_SXS_XML_E_WHITESPACEORQUESTIONMARK
//
// MessageText:
//
//  Manifest Parse Error : Expecting whitespace or '?'.
//
#define ERROR_SXS_XML_E_WHITESPACEORQUESTIONMARK 14050L

//
// MessageId: ERROR_SXS_XML_E_UNEXPECTEDENDTAG
//
// MessageText:
//
//  Manifest Parse Error : End tag was not expected at this location.
//
#define ERROR_SXS_XML_E_UNEXPECTEDENDTAG 14051L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDTAG
//
// MessageText:
//
//  Manifest Parse Error : The following tags were not closed: %1.
//
#define ERROR_SXS_XML_E_UNCLOSEDTAG      14052L

//
// MessageId: ERROR_SXS_XML_E_DUPLICATEATTRIBUTE
//
// MessageText:
//
//  Manifest Parse Error : Duplicate attribute.
//
#define ERROR_SXS_XML_E_DUPLICATEATTRIBUTE 14053L

//
// MessageId: ERROR_SXS_XML_E_MULTIPLEROOTS
//
// MessageText:
//
//  Manifest Parse Error : Only one top level element is allowed in an XML document.
//
#define ERROR_SXS_XML_E_MULTIPLEROOTS    14054L

//
// MessageId: ERROR_SXS_XML_E_INVALIDATROOTLEVEL
//
// MessageText:
//
//  Manifest Parse Error : Invalid at the top level of the document.
//
#define ERROR_SXS_XML_E_INVALIDATROOTLEVEL 14055L

//
// MessageId: ERROR_SXS_XML_E_BADXMLDECL
//
// MessageText:
//
//  Manifest Parse Error : Invalid xml declaration.
//
#define ERROR_SXS_XML_E_BADXMLDECL       14056L

//
// MessageId: ERROR_SXS_XML_E_MISSINGROOT
//
// MessageText:
//
//  Manifest Parse Error : XML document must have a top level element.
//
#define ERROR_SXS_XML_E_MISSINGROOT      14057L

//
// MessageId: ERROR_SXS_XML_E_UNEXPECTEDEOF
//
// MessageText:
//
//  Manifest Parse Error : Unexpected end of file.
//
#define ERROR_SXS_XML_E_UNEXPECTEDEOF    14058L

//
// MessageId: ERROR_SXS_XML_E_BADPEREFINSUBSET
//
// MessageText:
//
//  Manifest Parse Error : Parameter entities cannot be used inside markup declarations in an internal subset.
//
#define ERROR_SXS_XML_E_BADPEREFINSUBSET 14059L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDSTARTTAG
//
// MessageText:
//
//  Manifest Parse Error : Element was not closed.
//
#define ERROR_SXS_XML_E_UNCLOSEDSTARTTAG 14060L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDENDTAG
//
// MessageText:
//
//  Manifest Parse Error : End element was missing the character '>'.
//
#define ERROR_SXS_XML_E_UNCLOSEDENDTAG   14061L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDSTRING
//
// MessageText:
//
//  Manifest Parse Error : A string literal was not closed.
//
#define ERROR_SXS_XML_E_UNCLOSEDSTRING   14062L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDCOMMENT
//
// MessageText:
//
//  Manifest Parse Error : A comment was not closed.
//
#define ERROR_SXS_XML_E_UNCLOSEDCOMMENT  14063L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDDECL
//
// MessageText:
//
//  Manifest Parse Error : A declaration was not closed.
//
#define ERROR_SXS_XML_E_UNCLOSEDDECL     14064L

//
// MessageId: ERROR_SXS_XML_E_UNCLOSEDCDATA
//
// MessageText:
//
//  Manifest Parse Error : A CDATA section was not closed.
//
#define ERROR_SXS_XML_E_UNCLOSEDCDATA    14065L

//
// MessageId: ERROR_SXS_XML_E_RESERVEDNAMESPACE
//
// MessageText:
//
//  Manifest Parse Error : The namespace prefix is not allowed to start with the reserved string "xml".
//
#define ERROR_SXS_XML_E_RESERVEDNAMESPACE 14066L

//
// MessageId: ERROR_SXS_XML_E_INVALIDENCODING
//
// MessageText:
//
//  Manifest Parse Error : System does not support the specified encoding.
//
#define ERROR_SXS_XML_E_INVALIDENCODING  14067L

//
// MessageId: ERROR_SXS_XML_E_INVALIDSWITCH
//
// MessageText:
//
//  Manifest Parse Error : Switch from current encoding to specified encoding not supported.
//
#define ERROR_SXS_XML_E_INVALIDSWITCH    14068L

//
// MessageId: ERROR_SXS_XML_E_BADXMLCASE
//
// MessageText:
//
//  Manifest Parse Error : The name 'xml' is reserved and must be lower case.
//
#define ERROR_SXS_XML_E_BADXMLCASE       14069L

//
// MessageId: ERROR_SXS_XML_E_INVALID_STANDALONE
//
// MessageText:
//
//  Manifest Parse Error : The standalone attribute must have the value 'yes' or 'no'.
//
#define ERROR_SXS_XML_E_INVALID_STANDALONE 14070L

//
// MessageId: ERROR_SXS_XML_E_UNEXPECTED_STANDALONE
//
// MessageText:
//
//  Manifest Parse Error : The standalone attribute cannot be used in external entities.
//
#define ERROR_SXS_XML_E_UNEXPECTED_STANDALONE 14071L

//
// MessageId: ERROR_SXS_XML_E_INVALID_VERSION
//
// MessageText:
//
//  Manifest Parse Error : Invalid version number.
//
#define ERROR_SXS_XML_E_INVALID_VERSION  14072L

//
// MessageId: ERROR_SXS_XML_E_MISSINGEQUALS
//
// MessageText:
//
//  Manifest Parse Error : Missing equals sign between attribute and attribute value.
//
#define ERROR_SXS_XML_E_MISSINGEQUALS    14073L

//
// MessageId: ERROR_SXS_PROTECTION_RECOVERY_FAILED
//
// MessageText:
//
//  Assembly Protection Error : Unable to recover the specified assembly.
//
#define ERROR_SXS_PROTECTION_RECOVERY_FAILED 14074L

//
// MessageId: ERROR_SXS_PROTECTION_PUBLIC_KEY_TOO_SHORT
//
// MessageText:
//
//  Assembly Protection Error : The public key for an assembly was too short to be allowed.
//
#define ERROR_SXS_PROTECTION_PUBLIC_KEY_TOO_SHORT 14075L

//
// MessageId: ERROR_SXS_PROTECTION_CATALOG_NOT_VALID
//
// MessageText:
//
//  Assembly Protection Error : The catalog for an assembly is not valid, or does not match the assembly's manifest.
//
#define ERROR_SXS_PROTECTION_CATALOG_NOT_VALID 14076L

//
// MessageId: ERROR_SXS_UNTRANSLATABLE_HRESULT
//
// MessageText:
//
//  An HRESULT could not be translated to a corresponding Win32 error code.
//
#define ERROR_SXS_UNTRANSLATABLE_HRESULT 14077L

//
// MessageId: ERROR_SXS_PROTECTION_CATALOG_FILE_MISSING
//
// MessageText:
//
//  Assembly Protection Error : The catalog for an assembly is missing.
//
#define ERROR_SXS_PROTECTION_CATALOG_FILE_MISSING 14078L

//
// MessageId: ERROR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE
//
// MessageText:
//
//  The supplied assembly identity is missing one or more attributes which must be present in this context.
//
#define ERROR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE 14079L

//
// MessageId: ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME
//
// MessageText:
//
//  The supplied assembly identity has one or more attribute names that contain characters not permitted in XML names.
//
#define ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME 14080L


///////////////////////////////////////////////////
//                                               //
//           End of Side By Side Error Codes     //
//                                               //
//                 14000 to 14999                //
///////////////////////////////////////////////////



///////////////////////////////////////////////////
//                                               //
//           Start of IPSec Error codes          //
//                                               //
//                 13000 to 13999                //
///////////////////////////////////////////////////


//
// MessageId: ERROR_IPSEC_QM_POLICY_EXISTS
//
// MessageText:
//
//  The specified quick mode policy already exists.
//
#define ERROR_IPSEC_QM_POLICY_EXISTS     13000L

//
// MessageId: ERROR_IPSEC_QM_POLICY_NOT_FOUND
//
// MessageText:
//
//  The specified quick mode policy was not found.
//
#define ERROR_IPSEC_QM_POLICY_NOT_FOUND  13001L

//
// MessageId: ERROR_IPSEC_QM_POLICY_IN_USE
//
// MessageText:
//
//  The specified quick mode policy is being used.
//
#define ERROR_IPSEC_QM_POLICY_IN_USE     13002L

//
// MessageId: ERROR_IPSEC_MM_POLICY_EXISTS
//
// MessageText:
//
//  The specified main mode policy already exists.
//
#define ERROR_IPSEC_MM_POLICY_EXISTS     13003L

//
// MessageId: ERROR_IPSEC_MM_POLICY_NOT_FOUND
//
// MessageText:
//
//  The specified main mode policy was not found
//
#define ERROR_IPSEC_MM_POLICY_NOT_FOUND  13004L

//
// MessageId: ERROR_IPSEC_MM_POLICY_IN_USE
//
// MessageText:
//
//  The specified main mode policy is being used.
//
#define ERROR_IPSEC_MM_POLICY_IN_USE     13005L

//
// MessageId: ERROR_IPSEC_MM_FILTER_EXISTS
//
// MessageText:
//
//  The specified main mode filter already exists.
//
#define ERROR_IPSEC_MM_FILTER_EXISTS     13006L

//
// MessageId: ERROR_IPSEC_MM_FILTER_NOT_FOUND
//
// MessageText:
//
//  The specified main mode filter was not found.
//
#define ERROR_IPSEC_MM_FILTER_NOT_FOUND  13007L

//
// MessageId: ERROR_IPSEC_TRANSPORT_FILTER_EXISTS
//
// MessageText:
//
//  The specified transport mode filter already exists.
//
#define ERROR_IPSEC_TRANSPORT_FILTER_EXISTS 13008L

//
// MessageId: ERROR_IPSEC_TRANSPORT_FILTER_NOT_FOUND
//
// MessageText:
//
//  The specified transport mode filter does not exist.
//
#define ERROR_IPSEC_TRANSPORT_FILTER_NOT_FOUND 13009L

//
// MessageId: ERROR_IPSEC_MM_AUTH_EXISTS
//
// MessageText:
//
//  The specified main mode authentication list exists.
//
#define ERROR_IPSEC_MM_AUTH_EXISTS       13010L

//
// MessageId: ERROR_IPSEC_MM_AUTH_NOT_FOUND
//
// MessageText:
//
//  The specified main mode authentication list was not found.
//
#define ERROR_IPSEC_MM_AUTH_NOT_FOUND    13011L

//
// MessageId: ERROR_IPSEC_MM_AUTH_IN_USE
//
// MessageText:
//
//  The specified quick mode policy is being used.
//
#define ERROR_IPSEC_MM_AUTH_IN_USE       13012L

//
// MessageId: ERROR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND
//
// MessageText:
//
//  The specified main mode policy was not found.
//
#define ERROR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND 13013L

//
// MessageId: ERROR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND
//
// MessageText:
//
//  The specified quick mode policy was not found
//
#define ERROR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND 13014L

//
// MessageId: ERROR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND
//
// MessageText:
//
//  The manifest file contains one or more syntax errors.
//
#define ERROR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND 13015L

//
// MessageId: ERROR_IPSEC_TUNNEL_FILTER_EXISTS
//
// MessageText:
//
//  The application attempted to activate a disabled activation context.
//
#define ERROR_IPSEC_TUNNEL_FILTER_EXISTS 13016L

//
// MessageId: ERROR_IPSEC_TUNNEL_FILTER_NOT_FOUND
//
// MessageText:
//
//  The requested lookup key was not found in any active activation context.
//
#define ERROR_IPSEC_TUNNEL_FILTER_NOT_FOUND 13017L

//
// MessageId: ERROR_IPSEC_MM_FILTER_PENDING_DELETION
//
// MessageText:
//
//  The Main Mode filter is pending deletion.
//
#define ERROR_IPSEC_MM_FILTER_PENDING_DELETION 13018L

//
// MessageId: ERROR_IPSEC_TRANSPORT_FILTER_PENDING_DELETION
//
// MessageText:
//
//  The transport filter is pending deletion.
//
#define ERROR_IPSEC_TRANSPORT_FILTER_PENDING_DELETION 13019L

//
// MessageId: ERROR_IPSEC_TUNNEL_FILTER_PENDING_DELETION
//
// MessageText:
//
//  The tunnel filter is pending deletion.
//
#define ERROR_IPSEC_TUNNEL_FILTER_PENDING_DELETION 13020L

//
// MessageId: ERROR_IPSEC_MM_POLICY_PENDING_DELETION
//
// MessageText:
//
//  The Main Mode policy is pending deletion.
//
#define ERROR_IPSEC_MM_POLICY_PENDING_DELETION 13021L

//
// MessageId: ERROR_IPSEC_MM_AUTH_PENDING_DELETION
//
// MessageText:
//
//  The Main Mode authentication bundle is pending deletion.
//
#define ERROR_IPSEC_MM_AUTH_PENDING_DELETION 13022L

//
// MessageId: ERROR_IPSEC_QM_POLICY_PENDING_DELETION
//
// MessageText:
//
//  The Quick Mode policy is pending deletion.
//
#define ERROR_IPSEC_QM_POLICY_PENDING_DELETION 13023L

//
// MessageId: WARNING_IPSEC_MM_POLICY_PRUNED
//
// MessageText:
//
//  The Main Mode policy was successfully added, but some of the requested offers are not supported.
//
#define WARNING_IPSEC_MM_POLICY_PRUNED   13024L

//
// MessageId: WARNING_IPSEC_QM_POLICY_PRUNED
//
// MessageText:
//
//  The Quick Mode policy was successfully added, but some of the requested offers are not supported.
//
#define WARNING_IPSEC_QM_POLICY_PRUNED   13025L

//
// MessageId: ERROR_IPSEC_IKE_NEG_STATUS_BEGIN
//
// MessageText:
//
//  ERROR_IPSEC_IKE_NEG_STATUS_BEGIN
//
#define ERROR_IPSEC_IKE_NEG_STATUS_BEGIN 13800L

//
// MessageId: ERROR_IPSEC_IKE_AUTH_FAIL
//
// MessageText:
//
//  IKE authentication credentials are unacceptable
//
#define ERROR_IPSEC_IKE_AUTH_FAIL        13801L

//
// MessageId: ERROR_IPSEC_IKE_ATTRIB_FAIL
//
// MessageText:
//
//  IKE security attributes are unacceptable
//
#define ERROR_IPSEC_IKE_ATTRIB_FAIL      13802L

//
// MessageId: ERROR_IPSEC_IKE_NEGOTIATION_PENDING
//
// MessageText:
//
//  IKE Negotiation in progress
//
#define ERROR_IPSEC_IKE_NEGOTIATION_PENDING 13803L

//
// MessageId: ERROR_IPSEC_IKE_GENERAL_PROCESSING_ERROR
//
// MessageText:
//
//  General processing error
//
#define ERROR_IPSEC_IKE_GENERAL_PROCESSING_ERROR 13804L

//
// MessageId: ERROR_IPSEC_IKE_TIMED_OUT
//
// MessageText:
//
//  Negotiation timed out
//
#define ERROR_IPSEC_IKE_TIMED_OUT        13805L

//
// MessageId: ERROR_IPSEC_IKE_NO_CERT
//
// MessageText:
//
//  IKE failed to find valid machine certificate
//
#define ERROR_IPSEC_IKE_NO_CERT          13806L

//
// MessageId: ERROR_IPSEC_IKE_SA_DELETED
//
// MessageText:
//
//  IKE SA deleted by peer before establishment completed
//
#define ERROR_IPSEC_IKE_SA_DELETED       13807L

//
// MessageId: ERROR_IPSEC_IKE_SA_REAPED
//
// MessageText:
//
//  IKE SA deleted before establishment completed
//
#define ERROR_IPSEC_IKE_SA_REAPED        13808L

//
// MessageId: ERROR_IPSEC_IKE_MM_ACQUIRE_DROP
//
// MessageText:
//
//  Negotiation request sat in Queue too long
//
#define ERROR_IPSEC_IKE_MM_ACQUIRE_DROP  13809L

//
// MessageId: ERROR_IPSEC_IKE_QM_ACQUIRE_DROP
//
// MessageText:
//
//  Negotiation request sat in Queue too long
//
#define ERROR_IPSEC_IKE_QM_ACQUIRE_DROP  13810L

//
// MessageId: ERROR_IPSEC_IKE_QUEUE_DROP_MM
//
// MessageText:
//
//  Negotiation request sat in Queue too long
//
#define ERROR_IPSEC_IKE_QUEUE_DROP_MM    13811L

//
// MessageId: ERROR_IPSEC_IKE_QUEUE_DROP_NO_MM
//
// MessageText:
//
//  Negotiation request sat in Queue too long
//
#define ERROR_IPSEC_IKE_QUEUE_DROP_NO_MM 13812L

//
// MessageId: ERROR_IPSEC_IKE_DROP_NO_RESPONSE
//
// MessageText:
//
//  No response from peer
//
#define ERROR_IPSEC_IKE_DROP_NO_RESPONSE 13813L

//
// MessageId: ERROR_IPSEC_IKE_MM_DELAY_DROP
//
// MessageText:
//
//  Negotiation took too long
//
#define ERROR_IPSEC_IKE_MM_DELAY_DROP    13814L

//
// MessageId: ERROR_IPSEC_IKE_QM_DELAY_DROP
//
// MessageText:
//
//  Negotiation took too long
//
#define ERROR_IPSEC_IKE_QM_DELAY_DROP    13815L

//
// MessageId: ERROR_IPSEC_IKE_ERROR
//
// MessageText:
//
//  Unknown error occurred
//
#define ERROR_IPSEC_IKE_ERROR            13816L

//
// MessageId: ERROR_IPSEC_IKE_CRL_FAILED
//
// MessageText:
//
//  Certificate Revocation Check failed
//
#define ERROR_IPSEC_IKE_CRL_FAILED       13817L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_KEY_USAGE
//
// MessageText:
//
//  Invalid certificate key usage
//
#define ERROR_IPSEC_IKE_INVALID_KEY_USAGE 13818L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_CERT_TYPE
//
// MessageText:
//
//  Invalid certificate type
//
#define ERROR_IPSEC_IKE_INVALID_CERT_TYPE 13819L

//
// MessageId: ERROR_IPSEC_IKE_NO_PRIVATE_KEY
//
// MessageText:
//
//  No private key associated with machine certificate
//
#define ERROR_IPSEC_IKE_NO_PRIVATE_KEY   13820L

//
// MessageId: ERROR_IPSEC_IKE_DH_FAIL
//
// MessageText:
//
//  Failure in Diffie-Helman computation
//
#define ERROR_IPSEC_IKE_DH_FAIL          13822L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_HEADER
//
// MessageText:
//
//  Invalid header
//
#define ERROR_IPSEC_IKE_INVALID_HEADER   13824L

//
// MessageId: ERROR_IPSEC_IKE_NO_POLICY
//
// MessageText:
//
//  No policy configured
//
#define ERROR_IPSEC_IKE_NO_POLICY        13825L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_SIGNATURE
//
// MessageText:
//
//  Failed to verify signature
//
#define ERROR_IPSEC_IKE_INVALID_SIGNATURE 13826L

//
// MessageId: ERROR_IPSEC_IKE_KERBEROS_ERROR
//
// MessageText:
//
//  Failed to authenticate using kerberos
//
#define ERROR_IPSEC_IKE_KERBEROS_ERROR   13827L

//
// MessageId: ERROR_IPSEC_IKE_NO_PUBLIC_KEY
//
// MessageText:
//
//  Peer's certificate did not have a public key
//
#define ERROR_IPSEC_IKE_NO_PUBLIC_KEY    13828L

// These must stay as a unit.
//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR
//
// MessageText:
//
//  Error processing error payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR      13829L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_SA
//
// MessageText:
//
//  Error processing SA payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_SA   13830L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_PROP
//
// MessageText:
//
//  Error processing Proposal payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_PROP 13831L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_TRANS
//
// MessageText:
//
//  Error processing Transform payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_TRANS 13832L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_KE
//
// MessageText:
//
//  Error processing KE payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_KE   13833L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_ID
//
// MessageText:
//
//  Error processing ID payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_ID   13834L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_CERT
//
// MessageText:
//
//  Error processing Cert payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_CERT 13835L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_CERT_REQ
//
// MessageText:
//
//  Error processing Certificate Request payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_CERT_REQ 13836L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_HASH
//
// MessageText:
//
//  Error processing Hash payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_HASH 13837L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_SIG
//
// MessageText:
//
//  Error processing Signature payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_SIG  13838L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_NONCE
//
// MessageText:
//
//  Error processing Nonce payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_NONCE 13839L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_NOTIFY
//
// MessageText:
//
//  Error processing Notify payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_NOTIFY 13840L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_DELETE
//
// MessageText:
//
//  Error processing Delete Payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_DELETE 13841L

//
// MessageId: ERROR_IPSEC_IKE_PROCESS_ERR_VENDOR
//
// MessageText:
//
//  Error processing VendorId payload
//
#define ERROR_IPSEC_IKE_PROCESS_ERR_VENDOR 13842L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_PAYLOAD
//
// MessageText:
//
//  Invalid payload received
//
#define ERROR_IPSEC_IKE_INVALID_PAYLOAD  13843L

//
// MessageId: ERROR_IPSEC_IKE_LOAD_SOFT_SA
//
// MessageText:
//
//  Soft SA loaded
//
#define ERROR_IPSEC_IKE_LOAD_SOFT_SA     13844L

//
// MessageId: ERROR_IPSEC_IKE_SOFT_SA_TORN_DOWN
//
// MessageText:
//
//  Soft SA torn down
//
#define ERROR_IPSEC_IKE_SOFT_SA_TORN_DOWN 13845L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_COOKIE
//
// MessageText:
//
//  Invalid cookie received.
//
#define ERROR_IPSEC_IKE_INVALID_COOKIE   13846L

//
// MessageId: ERROR_IPSEC_IKE_NO_PEER_CERT
//
// MessageText:
//
//  Peer failed to send valid machine certificate
//
#define ERROR_IPSEC_IKE_NO_PEER_CERT     13847L

//
// MessageId: ERROR_IPSEC_IKE_PEER_CRL_FAILED
//
// MessageText:
//
//  Certification Revocation check of peer's certificate failed
//
#define ERROR_IPSEC_IKE_PEER_CRL_FAILED  13848L

//
// MessageId: ERROR_IPSEC_IKE_POLICY_CHANGE
//
// MessageText:
//
//  New policy invalidated SAs formed with old policy
//
#define ERROR_IPSEC_IKE_POLICY_CHANGE    13849L

//
// MessageId: ERROR_IPSEC_IKE_NO_MM_POLICY
//
// MessageText:
//
//  There is no available Main Mode IKE policy.
//
#define ERROR_IPSEC_IKE_NO_MM_POLICY     13850L

//
// MessageId: ERROR_IPSEC_IKE_NOTCBPRIV
//
// MessageText:
//
//  Failed to enabled TCB privilege.
//
#define ERROR_IPSEC_IKE_NOTCBPRIV        13851L

//
// MessageId: ERROR_IPSEC_IKE_SECLOADFAIL
//
// MessageText:
//
//  Failed to load SECURITY.DLL.
//
#define ERROR_IPSEC_IKE_SECLOADFAIL      13852L

//
// MessageId: ERROR_IPSEC_IKE_FAILSSPINIT
//
// MessageText:
//
//  Failed to obtain security function table dispatch address from SSPI.
//
#define ERROR_IPSEC_IKE_FAILSSPINIT      13853L

//
// MessageId: ERROR_IPSEC_IKE_FAILQUERYSSP
//
// MessageText:
//
//  Failed to query Kerberos package to obtain max token size.
//
#define ERROR_IPSEC_IKE_FAILQUERYSSP     13854L

//
// MessageId: ERROR_IPSEC_IKE_SRVACQFAIL
//
// MessageText:
//
//  Failed to obtain Kerberos server credentials for ISAKMP/ERROR_IPSEC_IKE service.  Kerberos authentication will not function.  The most likely reason for this is lack of domain membership.  This is normal if your computer is a member of a workgroup.
//
#define ERROR_IPSEC_IKE_SRVACQFAIL       13855L

//
// MessageId: ERROR_IPSEC_IKE_SRVQUERYCRED
//
// MessageText:
//
//  Failed to determine SSPI principal name for ISAKMP/ERROR_IPSEC_IKE service (QueryCredentialsAttributes).
//
#define ERROR_IPSEC_IKE_SRVQUERYCRED     13856L

//
// MessageId: ERROR_IPSEC_IKE_GETSPIFAIL
//
// MessageText:
//
//  Failed to obtain new SPI for the inbound SA from Ipsec driver.  The most common cause for this is that the driver does not have the correct filter.  Check your policy to verify the filters.
//
#define ERROR_IPSEC_IKE_GETSPIFAIL       13857L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_FILTER
//
// MessageText:
//
//  Given filter is invalid
//
#define ERROR_IPSEC_IKE_INVALID_FILTER   13858L

//
// MessageId: ERROR_IPSEC_IKE_OUT_OF_MEMORY
//
// MessageText:
//
//  Memory allocation failed.
//
#define ERROR_IPSEC_IKE_OUT_OF_MEMORY    13859L

//
// MessageId: ERROR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED
//
// MessageText:
//
//  Failed to add Security Association to IPSec Driver.  The most common cause for this is if the IKE negotiation took too long to complete.  If the problem persists, reduce the load on the faulting machine.
//
#define ERROR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED 13860L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_POLICY
//
// MessageText:
//
//  Invalid policy
//
#define ERROR_IPSEC_IKE_INVALID_POLICY   13861L

//
// MessageId: ERROR_IPSEC_IKE_UNKNOWN_DOI
//
// MessageText:
//
//  Invalid DOI
//
#define ERROR_IPSEC_IKE_UNKNOWN_DOI      13862L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_SITUATION
//
// MessageText:
//
//  Invalid situation
//
#define ERROR_IPSEC_IKE_INVALID_SITUATION 13863L

//
// MessageId: ERROR_IPSEC_IKE_DH_FAILURE
//
// MessageText:
//
//  Diffie-Hellman failure
//
#define ERROR_IPSEC_IKE_DH_FAILURE       13864L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_GROUP
//
// MessageText:
//
//  Invalid Diffie-Hellman group
//
#define ERROR_IPSEC_IKE_INVALID_GROUP    13865L

//
// MessageId: ERROR_IPSEC_IKE_ENCRYPT
//
// MessageText:
//
//  Error encrypting payload
//
#define ERROR_IPSEC_IKE_ENCRYPT          13866L

//
// MessageId: ERROR_IPSEC_IKE_DECRYPT
//
// MessageText:
//
//  Error decrypting payload
//
#define ERROR_IPSEC_IKE_DECRYPT          13867L

//
// MessageId: ERROR_IPSEC_IKE_POLICY_MATCH
//
// MessageText:
//
//  Policy match error
//
#define ERROR_IPSEC_IKE_POLICY_MATCH     13868L

//
// MessageId: ERROR_IPSEC_IKE_UNSUPPORTED_ID
//
// MessageText:
//
//  Unsupported ID
//
#define ERROR_IPSEC_IKE_UNSUPPORTED_ID   13869L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_HASH
//
// MessageText:
//
//  Hash verification failed
//
#define ERROR_IPSEC_IKE_INVALID_HASH     13870L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_HASH_ALG
//
// MessageText:
//
//  Invalid hash algorithm
//
#define ERROR_IPSEC_IKE_INVALID_HASH_ALG 13871L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_HASH_SIZE
//
// MessageText:
//
//  Invalid hash size
//
#define ERROR_IPSEC_IKE_INVALID_HASH_SIZE 13872L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_ENCRYPT_ALG
//
// MessageText:
//
//  Invalid encryption algorithm
//
#define ERROR_IPSEC_IKE_INVALID_ENCRYPT_ALG 13873L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_AUTH_ALG
//
// MessageText:
//
//  Invalid authentication algorithm
//
#define ERROR_IPSEC_IKE_INVALID_AUTH_ALG 13874L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_SIG
//
// MessageText:
//
//  Invalid certificate signature
//
#define ERROR_IPSEC_IKE_INVALID_SIG      13875L

//
// MessageId: ERROR_IPSEC_IKE_LOAD_FAILED
//
// MessageText:
//
//  Load failed
//
#define ERROR_IPSEC_IKE_LOAD_FAILED      13876L

//
// MessageId: ERROR_IPSEC_IKE_RPC_DELETE
//
// MessageText:
//
//  Deleted via RPC call
//
#define ERROR_IPSEC_IKE_RPC_DELETE       13877L

//
// MessageId: ERROR_IPSEC_IKE_BENIGN_REINIT
//
// MessageText:
//
//  Temporary state created to perform reinit. This is not a real failure.
//
#define ERROR_IPSEC_IKE_BENIGN_REINIT    13878L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY
//
// MessageText:
//
//  The lifetime value received in the Responder Lifetime Notify is below the Windows 2000 configured minimum value.  Please fix the policy on the peer machine.
//
#define ERROR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY 13879L

//
// MessageId: ERROR_IPSEC_IKE_INVALID_CERT_KEYLEN
//
// MessageText:
//
//  Key length in certificate is too small for configured security requirements.
//
#define ERROR_IPSEC_IKE_INVALID_CERT_KEYLEN 13881L

//
// MessageId: ERROR_IPSEC_IKE_MM_LIMIT
//
// MessageText:
//
//  Max number of established MM SAs to peer exceeded.
//
#define ERROR_IPSEC_IKE_MM_LIMIT         13882L

//
// MessageId: ERROR_IPSEC_IKE_NEGOTIATION_DISABLED
//
// MessageText:
//
//  IKE received a policy that disables negotiation.
//
#define ERROR_IPSEC_IKE_NEGOTIATION_DISABLED 13883L

//
// MessageId: ERROR_IPSEC_IKE_NEG_STATUS_END
//
// MessageText:
//
//  ERROR_IPSEC_IKE_NEG_STATUS_END
//
#define ERROR_IPSEC_IKE_NEG_STATUS_END   13884L

////////////////////////////////////
//                                //
//     COM Error Codes            //
//                                //
////////////////////////////////////

//
// The return value of COM functions and methods is an HRESULT.
// This is not a handle to anything, but is merely a 32-bit value
// with several fields encoded in the value.  The parts of an
// HRESULT are shown below.
//
// Many of the macros and functions below were orginally defined to
// operate on SCODEs.  SCODEs are no longer used.  The macros are
// still present for compatibility and easy porting of Win16 code.
// Newly written code should use the HRESULT macros and functions.
//

//
//  HRESULTs are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|R|C|N|r|    Facility         |               Code            |
//  +-+-+-+-+-+---------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      R - reserved portion of the facility code, corresponds to NT's
//              second severity bit.
//
//      C - reserved portion of the facility code, corresponds to NT's
//              C field.
//
//      N - reserved portion of the facility code. Used to indicate a
//              mapped NT status value.
//
//      r - reserved portion of the facility code. Reserved for internal
//              use. Used to indicate HRESULT values that are not status
//              values, but are instead message ids for display strings.
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//

//
// Severity values
//

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1


//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define SUCCEEDED(hr) ((HRESULT)(hr) >= 0)

//
// and the inverse
//

#define FAILED(hr) ((HRESULT)(hr) < 0)


//
// Generic test for error on any status value.
//

#define IS_ERROR(Status) ((unsigned long)(Status) >> 31 == SEVERITY_ERROR)

//
// Return the code
//

#define HRESULT_CODE(hr)    ((hr) & 0xFFFF)
#define SCODE_CODE(sc)      ((sc) & 0xFFFF)

//
//  Return the facility
//

#define HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1fff)
#define SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)

//
//  Return the severity
//

#define HRESULT_SEVERITY(hr)  (((hr) >> 31) & 0x1)
#define SCODE_SEVERITY(sc)    (((sc) >> 31) & 0x1)

//
// Create an HRESULT value from component pieces
//

#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define MAKE_SCODE(sev,fac,code) \
    ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )


//
// Map a WIN32 error value into a HRESULT
// Note: This assumes that WIN32 errors fall in the range -32k to 32k.
//
// Define bits here so macros are guaranteed to work

#define FACILITY_NT_BIT                 0x10000000

// __HRESULT_FROM_WIN32 will always be a macro.
// The goal will be to enable INLINE_HRESULT_FROM_WIN32 all the time,
// but there's too much code to change to do that at this time.

#define __HRESULT_FROM_WIN32(x) ((HRESULT)(x) <= 0 ? ((HRESULT)(x)) : ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000)))

#ifdef INLINE_HRESULT_FROM_WIN32
#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif
#ifndef __midl
__inline HRESULT HRESULT_FROM_WIN32(long x) { return x <= 0 ? (HRESULT)x : (HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000);}
#else
#define HRESULT_FROM_WIN32(x) __HRESULT_FROM_WIN32(x)
#endif
#else
#define HRESULT_FROM_WIN32(x) __HRESULT_FROM_WIN32(x)
#endif

//
// Map an NT status value into a HRESULT
//

#define HRESULT_FROM_NT(x)      ((HRESULT) ((x) | FACILITY_NT_BIT))


// ****** OBSOLETE functions

// HRESULT functions
// As noted above, these functions are obsolete and should not be used.


// Extract the SCODE from a HRESULT

#define GetScode(hr) ((SCODE) (hr))

// Convert an SCODE into an HRESULT.

#define ResultFromScode(sc) ((HRESULT) (sc))


// PropagateResult is a noop
#define PropagateResult(hrPrevious, scBase) ((HRESULT) scBase)


// ****** End of OBSOLETE functions.


// ---------------------- HRESULT value definitions -----------------
//
// HRESULT definitions
//

#ifdef RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) _sc
#else // RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#endif // RC_INVOKED

#define NOERROR             0

//
// Error definitions follow
//

//
// Codes 0x4000-0x40ff are reserved for OLE
//
//
// Error codes
//
//
// MessageId: E_UNEXPECTED
//
// MessageText:
//
//  Catastrophic failure
//
#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)

#if defined(_WIN32) && !defined(_MAC)
//
// MessageId: E_NOTIMPL
//
// MessageText:
//
//  Not implemented
//
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)

//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
//  Ran out of memory
//
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)

//
// MessageId: E_INVALIDARG
//
// MessageText:
//
//  One or more arguments are invalid
//
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)

//
// MessageId: E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)

//
// MessageId: E_HANDLE
//
// MessageText:
//
//  Invalid handle
//
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)

//
// MessageId: E_ABORT
//
// MessageText:
//
//  Operation aborted
//
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)

//
// MessageId: E_FAIL
//
// MessageText:
//
//  Unspecified error
//
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)

//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
//  General access denied error
//
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)

#else
//
// MessageId: E_NOTIMPL
//
// MessageText:
//
//  Not implemented
//
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80000001L)

//
// MessageId: E_OUTOFMEMORY
//
// MessageText:
//
//  Ran out of memory
//
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x80000002L)

//
// MessageId: E_INVALIDARG
//
// MessageText:
//
//  One or more arguments are invalid
//
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80000003L)

//
// MessageId: E_NOINTERFACE
//
// MessageText:
//
//  No such interface supported
//
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80000004L)

//
// MessageId: E_POINTER
//
// MessageText:
//
//  Invalid pointer
//
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80000005L)

//
// MessageId: E_HANDLE
//
// MessageText:
//
//  Invalid handle
//
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80000006L)

//
// MessageId: E_ABORT
//
// MessageText:
//
//  Operation aborted
//
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80000007L)

//
// MessageId: E_FAIL
//
// MessageText:
//
//  Unspecified error
//
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80000008L)

//
// MessageId: E_ACCESSDENIED
//
// MessageText:
//
//  General access denied error
//
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80000009L)

#endif //WIN32
//
// MessageId: E_PENDING
//
// MessageText:
//
//  The data necessary to complete this operation is not yet available.
//
#define E_PENDING                        _HRESULT_TYPEDEF_(0x8000000AL)

//
// MessageId: CO_E_INIT_TLS
//
// MessageText:
//
//  Thread local storage failure
//
#define CO_E_INIT_TLS                    _HRESULT_TYPEDEF_(0x80004006L)

//
// MessageId: CO_E_INIT_SHARED_ALLOCATOR
//
// MessageText:
//
//  Get shared memory allocator failure
//
#define CO_E_INIT_SHARED_ALLOCATOR       _HRESULT_TYPEDEF_(0x80004007L)

//
// MessageId: CO_E_INIT_MEMORY_ALLOCATOR
//
// MessageText:
//
//  Get memory allocator failure
//
#define CO_E_INIT_MEMORY_ALLOCATOR       _HRESULT_TYPEDEF_(0x80004008L)

//
// MessageId: CO_E_INIT_CLASS_CACHE
//
// MessageText:
//
//  Unable to initialize class cache
//
#define CO_E_INIT_CLASS_CACHE            _HRESULT_TYPEDEF_(0x80004009L)

//
// MessageId: CO_E_INIT_RPC_CHANNEL
//
// MessageText:
//
//  Unable to initialize RPC services
//
#define CO_E_INIT_RPC_CHANNEL            _HRESULT_TYPEDEF_(0x8000400AL)

//
// MessageId: CO_E_INIT_TLS_SET_CHANNEL_CONTROL
//
// MessageText:
//
//  Cannot set thread local storage channel control
//
#define CO_E_INIT_TLS_SET_CHANNEL_CONTROL _HRESULT_TYPEDEF_(0x8000400BL)

//
// MessageId: CO_E_INIT_TLS_CHANNEL_CONTROL
//
// MessageText:
//
//  Could not allocate thread local storage channel control
//
#define CO_E_INIT_TLS_CHANNEL_CONTROL    _HRESULT_TYPEDEF_(0x8000400CL)

//
// MessageId: CO_E_INIT_UNACCEPTED_USER_ALLOCATOR
//
// MessageText:
//
//  The user supplied memory allocator is unacceptable
//
#define CO_E_INIT_UNACCEPTED_USER_ALLOCATOR _HRESULT_TYPEDEF_(0x8000400DL)

//
// MessageId: CO_E_INIT_SCM_MUTEX_EXISTS
//
// MessageText:
//
//  The OLE service mutex already exists
//
#define CO_E_INIT_SCM_MUTEX_EXISTS       _HRESULT_TYPEDEF_(0x8000400EL)

//
// MessageId: CO_E_INIT_SCM_FILE_MAPPING_EXISTS
//
// MessageText:
//
//  The OLE service file mapping already exists
//
#define CO_E_INIT_SCM_FILE_MAPPING_EXISTS _HRESULT_TYPEDEF_(0x8000400FL)

//
// MessageId: CO_E_INIT_SCM_MAP_VIEW_OF_FILE
//
// MessageText:
//
//  Unable to map view of file for OLE service
//
#define CO_E_INIT_SCM_MAP_VIEW_OF_FILE   _HRESULT_TYPEDEF_(0x80004010L)

//
// MessageId: CO_E_INIT_SCM_EXEC_FAILURE
//
// MessageText:
//
//  Failure attempting to launch OLE service
//
#define CO_E_INIT_SCM_EXEC_FAILURE       _HRESULT_TYPEDEF_(0x80004011L)

//
// MessageId: CO_E_INIT_ONLY_SINGLE_THREADED
//
// MessageText:
//
//  There was an attempt to call CoInitialize a second time while single threaded
//
#define CO_E_INIT_ONLY_SINGLE_THREADED   _HRESULT_TYPEDEF_(0x80004012L)

//
// MessageId: CO_E_CANT_REMOTE
//
// MessageText:
//
//  A Remote activation was necessary but was not allowed
//
#define CO_E_CANT_REMOTE                 _HRESULT_TYPEDEF_(0x80004013L)

//
// MessageId: CO_E_BAD_SERVER_NAME
//
// MessageText:
//
//  A Remote activation was necessary but the server name provided was invalid
//
#define CO_E_BAD_SERVER_NAME             _HRESULT_TYPEDEF_(0x80004014L)

//
// MessageId: CO_E_WRONG_SERVER_IDENTITY
//
// MessageText:
//
//  The class is configured to run as a security id different from the caller
//
#define CO_E_WRONG_SERVER_IDENTITY       _HRESULT_TYPEDEF_(0x80004015L)

//
// MessageId: CO_E_OLE1DDE_DISABLED
//
// MessageText:
//
//  Use of Ole1 services requiring DDE windows is disabled
//
#define CO_E_OLE1DDE_DISABLED            _HRESULT_TYPEDEF_(0x80004016L)

//
// MessageId: CO_E_RUNAS_SYNTAX
//
// MessageText:
//
//  A RunAs specification must be <domain name>\<user name> or simply <user name>
//
#define CO_E_RUNAS_SYNTAX                _HRESULT_TYPEDEF_(0x80004017L)

//
// MessageId: CO_E_CREATEPROCESS_FAILURE
//
// MessageText:
//
//  The server process could not be started.  The pathname may be incorrect.
//
#define CO_E_CREATEPROCESS_FAILURE       _HRESULT_TYPEDEF_(0x80004018L)

//
// MessageId: CO_E_RUNAS_CREATEPROCESS_FAILURE
//
// MessageText:
//
//  The server process could not be started as the configured identity.  The pathname may be incorrect or unavailable.
//
#define CO_E_RUNAS_CREATEPROCESS_FAILURE _HRESULT_TYPEDEF_(0x80004019L)

//
// MessageId: CO_E_RUNAS_LOGON_FAILURE
//
// MessageText:
//
//  The server process could not be started because the configured identity is incorrect.  Check the username and password.
//
#define CO_E_RUNAS_LOGON_FAILURE         _HRESULT_TYPEDEF_(0x8000401AL)

//
// MessageId: CO_E_LAUNCH_PERMSSION_DENIED
//
// MessageText:
//
//  The client is not allowed to launch this server.
//
#define CO_E_LAUNCH_PERMSSION_DENIED     _HRESULT_TYPEDEF_(0x8000401BL)

//
// MessageId: CO_E_START_SERVICE_FAILURE
//
// MessageText:
//
//  The service providing this server could not be started.
//
#define CO_E_START_SERVICE_FAILURE       _HRESULT_TYPEDEF_(0x8000401CL)

//
// MessageId: CO_E_REMOTE_COMMUNICATION_FAILURE
//
// MessageText:
//
//  This computer was unable to communicate with the computer providing the server.
//
#define CO_E_REMOTE_COMMUNICATION_FAILURE _HRESULT_TYPEDEF_(0x8000401DL)

//
// MessageId: CO_E_SERVER_START_TIMEOUT
//
// MessageText:
//
//  The server did not respond after being launched.
//
#define CO_E_SERVER_START_TIMEOUT        _HRESULT_TYPEDEF_(0x8000401EL)

//
// MessageId: CO_E_CLSREG_INCONSISTENT
//
// MessageText:
//
//  The registration information for this server is inconsistent or incomplete.
//
#define CO_E_CLSREG_INCONSISTENT         _HRESULT_TYPEDEF_(0x8000401FL)

//
// MessageId: CO_E_IIDREG_INCONSISTENT
//
// MessageText:
//
//  The registration information for this interface is inconsistent or incomplete.
//
#define CO_E_IIDREG_INCONSISTENT         _HRESULT_TYPEDEF_(0x80004020L)

//
// MessageId: CO_E_NOT_SUPPORTED
//
// MessageText:
//
//  The operation attempted is not supported.
//
#define CO_E_NOT_SUPPORTED               _HRESULT_TYPEDEF_(0x80004021L)

//
// MessageId: CO_E_RELOAD_DLL
//
// MessageText:
//
//  A dll must be loaded.
//
#define CO_E_RELOAD_DLL                  _HRESULT_TYPEDEF_(0x80004022L)

//
// MessageId: CO_E_MSI_ERROR
//
// MessageText:
//
//  A Microsoft Software Installer error was encountered.
//
#define CO_E_MSI_ERROR                   _HRESULT_TYPEDEF_(0x80004023L)

//
// MessageId: CO_E_ATTEMPT_TO_CREATE_OUTSIDE_CLIENT_CONTEXT
//
// MessageText:
//
//  The specified activation could not occur in the client context as specified.
//
#define CO_E_ATTEMPT_TO_CREATE_OUTSIDE_CLIENT_CONTEXT _HRESULT_TYPEDEF_(0x80004024L)

//
// MessageId: CO_E_SERVER_PAUSED
//
// MessageText:
//
//  Activations on the server are paused.
//
#define CO_E_SERVER_PAUSED               _HRESULT_TYPEDEF_(0x80004025L)

//
// MessageId: CO_E_SERVER_NOT_PAUSED
//
// MessageText:
//
//  Activations on the server are not paused.
//
#define CO_E_SERVER_NOT_PAUSED           _HRESULT_TYPEDEF_(0x80004026L)

//
// MessageId: CO_E_CLASS_DISABLED
//
// MessageText:
//
//  The component or application containing the component has been disabled.
//
#define CO_E_CLASS_DISABLED              _HRESULT_TYPEDEF_(0x80004027L)

//
// MessageId: CO_E_CLRNOTAVAILABLE
//
// MessageText:
//
//  The common language runtime is not available
//
#define CO_E_CLRNOTAVAILABLE             _HRESULT_TYPEDEF_(0x80004028L)

//
// MessageId: CO_E_ASYNC_WORK_REJECTED
//
// MessageText:
//
//  The thread-pool rejected the submitted asynchronous work.
//
#define CO_E_ASYNC_WORK_REJECTED         _HRESULT_TYPEDEF_(0x80004029L)

//
// MessageId: CO_E_SERVER_INIT_TIMEOUT
//
// MessageText:
//
//  The server started, but did not finish initializing in a timely fashion.
//
#define CO_E_SERVER_INIT_TIMEOUT         _HRESULT_TYPEDEF_(0x8000402AL)

//
// MessageId: CO_E_NO_SECCTX_IN_ACTIVATE
//
// MessageText:
//
//  Unable to complete the call since there is no COM+ security context inside IObjectControl.Activate.
//
#define CO_E_NO_SECCTX_IN_ACTIVATE       _HRESULT_TYPEDEF_(0x8000402BL)

//
// MessageId: CO_E_TRACKER_CONFIG
//
// MessageText:
//
//  The provided tracker configuration is invalid
//
#define CO_E_TRACKER_CONFIG              _HRESULT_TYPEDEF_(0x80004030L)

//
// MessageId: CO_E_THREADPOOL_CONFIG
//
// MessageText:
//
//  The provided thread pool configuration is invalid
//
#define CO_E_THREADPOOL_CONFIG           _HRESULT_TYPEDEF_(0x80004031L)

//
// MessageId: CO_E_SXS_CONFIG
//
// MessageText:
//
//  The provided side-by-side configuration is invalid
//
#define CO_E_SXS_CONFIG                  _HRESULT_TYPEDEF_(0x80004032L)

//
// MessageId: CO_E_MALFORMED_SPN
//
// MessageText:
//
//  The server principal name (SPN) obtained during security negotiation is malformed.
//
#define CO_E_MALFORMED_SPN               _HRESULT_TYPEDEF_(0x80004033L)


//
// Success codes
//
#define S_OK                                   ((HRESULT)0x00000000L)
#define S_FALSE                                ((HRESULT)0x00000001L)

// ******************
// FACILITY_ITF
// ******************

//
// Codes 0x0-0x01ff are reserved for the OLE group of
// interfaces.
//


//
// Generic OLE errors that may be returned by many inerfaces
//

#define OLE_E_FIRST ((HRESULT)0x80040000L)
#define OLE_E_LAST  ((HRESULT)0x800400FFL)
#define OLE_S_FIRST ((HRESULT)0x00040000L)
#define OLE_S_LAST  ((HRESULT)0x000400FFL)

//
// Old OLE errors
//
//
// MessageId: OLE_E_OLEVERB
//
// MessageText:
//
//  Invalid OLEVERB structure
//
#define OLE_E_OLEVERB                    _HRESULT_TYPEDEF_(0x80040000L)

//
// MessageId: OLE_E_ADVF
//
// MessageText:
//
//  Invalid advise flags
//
#define OLE_E_ADVF                       _HRESULT_TYPEDEF_(0x80040001L)

//
// MessageId: OLE_E_ENUM_NOMORE
//
// MessageText:
//
//  Can't enumerate any more, because the associated data is missing
//
#define OLE_E_ENUM_NOMORE                _HRESULT_TYPEDEF_(0x80040002L)

//
// MessageId: OLE_E_ADVISENOTSUPPORTED
//
// MessageText:
//
//  This implementation doesn't take advises
//
#define OLE_E_ADVISENOTSUPPORTED         _HRESULT_TYPEDEF_(0x80040003L)

//
// MessageId: OLE_E_NOCONNECTION
//
// MessageText:
//
//  There is no connection for this connection ID
//
#define OLE_E_NOCONNECTION               _HRESULT_TYPEDEF_(0x80040004L)

//
// MessageId: OLE_E_NOTRUNNING
//
// MessageText:
//
//  Need to run the object to perform this operation
//
#define OLE_E_NOTRUNNING                 _HRESULT_TYPEDEF_(0x80040005L)

//
// MessageId: OLE_E_NOCACHE
//
// MessageText:
//
//  There is no cache to operate on
//
#define OLE_E_NOCACHE                    _HRESULT_TYPEDEF_(0x80040006L)

//
// MessageId: OLE_E_BLANK
//
// MessageText:
//
//  Uninitialized object
//
#define OLE_E_BLANK                      _HRESULT_TYPEDEF_(0x80040007L)

//
// MessageId: OLE_E_CLASSDIFF
//
// MessageText:
//
//  Linked object's source class has changed
//
#define OLE_E_CLASSDIFF                  _HRESULT_TYPEDEF_(0x80040008L)

//
// MessageId: OLE_E_CANT_GETMONIKER
//
// MessageText:
//
//  Not able to get the moniker of the object
//
#define OLE_E_CANT_GETMONIKER            _HRESULT_TYPEDEF_(0x80040009L)

//
// MessageId: OLE_E_CANT_BINDTOSOURCE
//
// MessageText:
//
//  Not able to bind to the source
//
#define OLE_E_CANT_BINDTOSOURCE          _HRESULT_TYPEDEF_(0x8004000AL)

//
// MessageId: OLE_E_STATIC
//
// MessageText:
//
//  Object is static; operation not allowed
//
#define OLE_E_STATIC                     _HRESULT_TYPEDEF_(0x8004000BL)

//
// MessageId: OLE_E_PROMPTSAVECANCELLED
//
// MessageText:
//
//  User canceled out of save dialog
//
#define OLE_E_PROMPTSAVECANCELLED        _HRESULT_TYPEDEF_(0x8004000CL)

//
// MessageId: OLE_E_INVALIDRECT
//
// MessageText:
//
//  Invalid rectangle
//
#define OLE_E_INVALIDRECT                _HRESULT_TYPEDEF_(0x8004000DL)

//
// MessageId: OLE_E_WRONGCOMPOBJ
//
// MessageText:
//
//  compobj.dll is too old for the ole2.dll initialized
//
#define OLE_E_WRONGCOMPOBJ               _HRESULT_TYPEDEF_(0x8004000EL)

//
// MessageId: OLE_E_INVALIDHWND
//
// MessageText:
//
//  Invalid window handle
//
#define OLE_E_INVALIDHWND                _HRESULT_TYPEDEF_(0x8004000FL)

//
// MessageId: OLE_E_NOT_INPLACEACTIVE
//
// MessageText:
//
//  Object is not in any of the inplace active states
//
#define OLE_E_NOT_INPLACEACTIVE          _HRESULT_TYPEDEF_(0x80040010L)

//
// MessageId: OLE_E_CANTCONVERT
//
// MessageText:
//
//  Not able to convert object
//
#define OLE_E_CANTCONVERT                _HRESULT_TYPEDEF_(0x80040011L)

//
// MessageId: OLE_E_NOSTORAGE
//
// MessageText:
//
//  Not able to perform the operation because object is not given storage yet
//
#define OLE_E_NOSTORAGE                  _HRESULT_TYPEDEF_(0x80040012L)

//
// MessageId: DV_E_FORMATETC
//
// MessageText:
//
//  Invalid FORMATETC structure
//
#define DV_E_FORMATETC                   _HRESULT_TYPEDEF_(0x80040064L)

//
// MessageId: DV_E_DVTARGETDEVICE
//
// MessageText:
//
//  Invalid DVTARGETDEVICE structure
//
#define DV_E_DVTARGETDEVICE              _HRESULT_TYPEDEF_(0x80040065L)

//
// MessageId: DV_E_STGMEDIUM
//
// MessageText:
//
//  Invalid STDGMEDIUM structure
//
#define DV_E_STGMEDIUM                   _HRESULT_TYPEDEF_(0x80040066L)

//
// MessageId: DV_E_STATDATA
//
// MessageText:
//
//  Invalid STATDATA structure
//
#define DV_E_STATDATA                    _HRESULT_TYPEDEF_(0x80040067L)

//
// MessageId: DV_E_LINDEX
//
// MessageText:
//
//  Invalid lindex
//
#define DV_E_LINDEX                      _HRESULT_TYPEDEF_(0x80040068L)

//
// MessageId: DV_E_TYMED
//
// MessageText:
//
//  Invalid tymed
//
#define DV_E_TYMED                       _HRESULT_TYPEDEF_(0x80040069L)

//
// MessageId: DV_E_CLIPFORMAT
//
// MessageText:
//
//  Invalid clipboard format
//
#define DV_E_CLIPFORMAT                  _HRESULT_TYPEDEF_(0x8004006AL)

//
// MessageId: DV_E_DVASPECT
//
// MessageText:
//
//  Invalid aspect(s)
//
#define DV_E_DVASPECT                    _HRESULT_TYPEDEF_(0x8004006BL)

//
// MessageId: DV_E_DVTARGETDEVICE_SIZE
//
// MessageText:
//
//  tdSize parameter of the DVTARGETDEVICE structure is invalid
//
#define DV_E_DVTARGETDEVICE_SIZE         _HRESULT_TYPEDEF_(0x8004006CL)

//
// MessageId: DV_E_NOIVIEWOBJECT
//
// MessageText:
//
//  Object doesn't support IViewObject interface
//
#define DV_E_NOIVIEWOBJECT               _HRESULT_TYPEDEF_(0x8004006DL)

#define DRAGDROP_E_FIRST 0x80040100L
#define DRAGDROP_E_LAST  0x8004010FL
#define DRAGDROP_S_FIRST 0x00040100L
#define DRAGDROP_S_LAST  0x0004010FL
//
// MessageId: DRAGDROP_E_NOTREGISTERED
//
// MessageText:
//
//  Trying to revoke a drop target that has not been registered
//
#define DRAGDROP_E_NOTREGISTERED         _HRESULT_TYPEDEF_(0x80040100L)

//
// MessageId: DRAGDROP_E_ALREADYREGISTERED
//
// MessageText:
//
//  This window has already been registered as a drop target
//
#define DRAGDROP_E_ALREADYREGISTERED     _HRESULT_TYPEDEF_(0x80040101L)

//
// MessageId: DRAGDROP_E_INVALIDHWND
//
// MessageText:
//
//  Invalid window handle
//
#define DRAGDROP_E_INVALIDHWND           _HRESULT_TYPEDEF_(0x80040102L)

#define CLASSFACTORY_E_FIRST  0x80040110L
#define CLASSFACTORY_E_LAST   0x8004011FL
#define CLASSFACTORY_S_FIRST  0x00040110L
#define CLASSFACTORY_S_LAST   0x0004011FL
//
// MessageId: CLASS_E_NOAGGREGATION
//
// MessageText:
//
//  Class does not support aggregation (or class object is remote)
//
#define CLASS_E_NOAGGREGATION            _HRESULT_TYPEDEF_(0x80040110L)

//
// MessageId: CLASS_E_CLASSNOTAVAILABLE
//
// MessageText:
//
//  ClassFactory cannot supply requested class
//
#define CLASS_E_CLASSNOTAVAILABLE        _HRESULT_TYPEDEF_(0x80040111L)

//
// MessageId: CLASS_E_NOTLICENSED
//
// MessageText:
//
//  Class is not licensed for use
//
#define CLASS_E_NOTLICENSED              _HRESULT_TYPEDEF_(0x80040112L)

#define MARSHAL_E_FIRST  0x80040120L
#define MARSHAL_E_LAST   0x8004012FL
#define MARSHAL_S_FIRST  0x00040120L
#define MARSHAL_S_LAST   0x0004012FL
#define DATA_E_FIRST     0x80040130L
#define DATA_E_LAST      0x8004013FL
#define DATA_S_FIRST     0x00040130L
#define DATA_S_LAST      0x0004013FL
#define VIEW_E_FIRST     0x80040140L
#define VIEW_E_LAST      0x8004014FL
#define VIEW_S_FIRST     0x00040140L
#define VIEW_S_LAST      0x0004014FL
//
// MessageId: VIEW_E_DRAW
//
// MessageText:
//
//  Error drawing view
//
#define VIEW_E_DRAW                      _HRESULT_TYPEDEF_(0x80040140L)

#define REGDB_E_FIRST     0x80040150L
#define REGDB_E_LAST      0x8004015FL
#define REGDB_S_FIRST     0x00040150L
#define REGDB_S_LAST      0x0004015FL
//
// MessageId: REGDB_E_READREGDB
//
// MessageText:
//
//  Could not read key from registry
//
#define REGDB_E_READREGDB                _HRESULT_TYPEDEF_(0x80040150L)

//
// MessageId: REGDB_E_WRITEREGDB
//
// MessageText:
//
//  Could not write key to registry
//
#define REGDB_E_WRITEREGDB               _HRESULT_TYPEDEF_(0x80040151L)

//
// MessageId: REGDB_E_KEYMISSING
//
// MessageText:
//
//  Could not find the key in the registry
//
#define REGDB_E_KEYMISSING               _HRESULT_TYPEDEF_(0x80040152L)

//
// MessageId: REGDB_E_INVALIDVALUE
//
// MessageText:
//
//  Invalid value for registry
//
#define REGDB_E_INVALIDVALUE             _HRESULT_TYPEDEF_(0x80040153L)

//
// MessageId: REGDB_E_CLASSNOTREG
//
// MessageText:
//
//  Class not registered
//
#define REGDB_E_CLASSNOTREG              _HRESULT_TYPEDEF_(0x80040154L)

//
// MessageId: REGDB_E_IIDNOTREG
//
// MessageText:
//
//  Interface not registered
//
#define REGDB_E_IIDNOTREG                _HRESULT_TYPEDEF_(0x80040155L)

//
// MessageId: REGDB_E_BADTHREADINGMODEL
//
// MessageText:
//
//  Threading model entry is not valid
//
#define REGDB_E_BADTHREADINGMODEL        _HRESULT_TYPEDEF_(0x80040156L)

#define CAT_E_FIRST     0x80040160L
#define CAT_E_LAST      0x80040161L
//
// MessageId: CAT_E_CATIDNOEXIST
//
// MessageText:
//
//  CATID does not exist
//
#define CAT_E_CATIDNOEXIST               _HRESULT_TYPEDEF_(0x80040160L)

//
// MessageId: CAT_E_NODESCRIPTION
//
// MessageText:
//
//  Description not found
//
#define CAT_E_NODESCRIPTION              _HRESULT_TYPEDEF_(0x80040161L)

////////////////////////////////////
//                                //
//     Class Store Error Codes    //
//                                //
////////////////////////////////////
#define CS_E_FIRST     0x80040164L
#define CS_E_LAST      0x8004016FL
//
// MessageId: CS_E_PACKAGE_NOTFOUND
//
// MessageText:
//
//  No package in the software installation data in the Active Directory meets this criteria.
//
#define CS_E_PACKAGE_NOTFOUND            _HRESULT_TYPEDEF_(0x80040164L)

//
// MessageId: CS_E_NOT_DELETABLE
//
// MessageText:
//
//  Deleting this will break the referential integrity of the software installation data in the Active Directory.
//
#define CS_E_NOT_DELETABLE               _HRESULT_TYPEDEF_(0x80040165L)

//
// MessageId: CS_E_CLASS_NOTFOUND
//
// MessageText:
//
//  The CLSID was not found in the software installation data in the Active Directory.
//
#define CS_E_CLASS_NOTFOUND              _HRESULT_TYPEDEF_(0x80040166L)

//
// MessageId: CS_E_INVALID_VERSION
//
// MessageText:
//
//  The software installation data in the Active Directory is corrupt.
//
#define CS_E_INVALID_VERSION             _HRESULT_TYPEDEF_(0x80040167L)

//
// MessageId: CS_E_NO_CLASSSTORE
//
// MessageText:
//
//  There is no software installation data in the Active Directory.
//
#define CS_E_NO_CLASSSTORE               _HRESULT_TYPEDEF_(0x80040168L)

//
// MessageId: CS_E_OBJECT_NOTFOUND
//
// MessageText:
//
//  There is no software installation data object in the Active Directory.
//
#define CS_E_OBJECT_NOTFOUND             _HRESULT_TYPEDEF_(0x80040169L)

//
// MessageId: CS_E_OBJECT_ALREADY_EXISTS
//
// MessageText:
//
//  The software installation data object in the Active Directory already exists.
//
#define CS_E_OBJECT_ALREADY_EXISTS       _HRESULT_TYPEDEF_(0x8004016AL)

//
// MessageId: CS_E_INVALID_PATH
//
// MessageText:
//
//  The path to the software installation data in the Active Directory is not correct.
//
#define CS_E_INVALID_PATH                _HRESULT_TYPEDEF_(0x8004016BL)

//
// MessageId: CS_E_NETWORK_ERROR
//
// MessageText:
//
//  A network error interrupted the operation.
//
#define CS_E_NETWORK_ERROR               _HRESULT_TYPEDEF_(0x8004016CL)

//
// MessageId: CS_E_ADMIN_LIMIT_EXCEEDED
//
// MessageText:
//
//  The size of this object exceeds the maximum size set by the Administrator.
//
#define CS_E_ADMIN_LIMIT_EXCEEDED        _HRESULT_TYPEDEF_(0x8004016DL)

//
// MessageId: CS_E_SCHEMA_MISMATCH
//
// MessageText:
//
//  The schema for the software installation data in the Active Directory does not match the required schema.
//
#define CS_E_SCHEMA_MISMATCH             _HRESULT_TYPEDEF_(0x8004016EL)

//
// MessageId: CS_E_INTERNAL_ERROR
//
// MessageText:
//
//  An error occurred in the software installation data in the Active Directory.
//
#define CS_E_INTERNAL_ERROR              _HRESULT_TYPEDEF_(0x8004016FL)

#define CACHE_E_FIRST     0x80040170L
#define CACHE_E_LAST      0x8004017FL
#define CACHE_S_FIRST     0x00040170L
#define CACHE_S_LAST      0x0004017FL
//
// MessageId: CACHE_E_NOCACHE_UPDATED
//
// MessageText:
//
//  Cache not updated
//
#define CACHE_E_NOCACHE_UPDATED          _HRESULT_TYPEDEF_(0x80040170L)

#define OLEOBJ_E_FIRST     0x80040180L
#define OLEOBJ_E_LAST      0x8004018FL
#define OLEOBJ_S_FIRST     0x00040180L
#define OLEOBJ_S_LAST      0x0004018FL
//
// MessageId: OLEOBJ_E_NOVERBS
//
// MessageText:
//
//  No verbs for OLE object
//
#define OLEOBJ_E_NOVERBS                 _HRESULT_TYPEDEF_(0x80040180L)

//
// MessageId: OLEOBJ_E_INVALIDVERB
//
// MessageText:
//
//  Invalid verb for OLE object
//
#define OLEOBJ_E_INVALIDVERB             _HRESULT_TYPEDEF_(0x80040181L)

#define CLIENTSITE_E_FIRST     0x80040190L
#define CLIENTSITE_E_LAST      0x8004019FL
#define CLIENTSITE_S_FIRST     0x00040190L
#define CLIENTSITE_S_LAST      0x0004019FL
//
// MessageId: INPLACE_E_NOTUNDOABLE
//
// MessageText:
//
//  Undo is not available
//
#define INPLACE_E_NOTUNDOABLE            _HRESULT_TYPEDEF_(0x800401A0L)

//
// MessageId: INPLACE_E_NOTOOLSPACE
//
// MessageText:
//
//  Space for tools is not available
//
#define INPLACE_E_NOTOOLSPACE            _HRESULT_TYPEDEF_(0x800401A1L)

#define INPLACE_E_FIRST     0x800401A0L
#define INPLACE_E_LAST      0x800401AFL
#define INPLACE_S_FIRST     0x000401A0L
#define INPLACE_S_LAST      0x000401AFL
#define ENUM_E_FIRST        0x800401B0L
#define ENUM_E_LAST         0x800401BFL
#define ENUM_S_FIRST        0x000401B0L
#define ENUM_S_LAST         0x000401BFL
#define CONVERT10_E_FIRST        0x800401C0L
#define CONVERT10_E_LAST         0x800401CFL
#define CONVERT10_S_FIRST        0x000401C0L
#define CONVERT10_S_LAST         0x000401CFL
//
// MessageId: CONVERT10_E_OLESTREAM_GET
//
// MessageText:
//
//  OLESTREAM Get method failed
//
#define CONVERT10_E_OLESTREAM_GET        _HRESULT_TYPEDEF_(0x800401C0L)

//
// MessageId: CONVERT10_E_OLESTREAM_PUT
//
// MessageText:
//
//  OLESTREAM Put method failed
//
#define CONVERT10_E_OLESTREAM_PUT        _HRESULT_TYPEDEF_(0x800401C1L)

//
// MessageId: CONVERT10_E_OLESTREAM_FMT
//
// MessageText:
//
//  Contents of the OLESTREAM not in correct format
//
#define CONVERT10_E_OLESTREAM_FMT        _HRESULT_TYPEDEF_(0x800401C2L)

//
// MessageId: CONVERT10_E_OLESTREAM_BITMAP_TO_DIB
//
// MessageText:
//
//  There was an error in a Windows GDI call while converting the bitmap to a DIB
//
#define CONVERT10_E_OLESTREAM_BITMAP_TO_DIB _HRESULT_TYPEDEF_(0x800401C3L)

//
// MessageId: CONVERT10_E_STG_FMT
//
// MessageText:
//
//  Contents of the IStorage not in correct format
//
#define CONVERT10_E_STG_FMT              _HRESULT_TYPEDEF_(0x800401C4L)

//
// MessageId: CONVERT10_E_STG_NO_STD_STREAM
//
// MessageText:
//
//  Contents of IStorage is missing one of the standard streams
//
#define CONVERT10_E_STG_NO_STD_STREAM    _HRESULT_TYPEDEF_(0x800401C5L)

//
// MessageId: CONVERT10_E_STG_DIB_TO_BITMAP
//
// MessageText:
//
//  There was an error in a Windows GDI call while converting the DIB to a bitmap.
//  
//
#define CONVERT10_E_STG_DIB_TO_BITMAP    _HRESULT_TYPEDEF_(0x800401C6L)

#define CLIPBRD_E_FIRST        0x800401D0L
#define CLIPBRD_E_LAST         0x800401DFL
#define CLIPBRD_S_FIRST        0x000401D0L
#define CLIPBRD_S_LAST         0x000401DFL
//
// MessageId: CLIPBRD_E_CANT_OPEN
//
// MessageText:
//
//  OpenClipboard Failed
//
#define CLIPBRD_E_CANT_OPEN              _HRESULT_TYPEDEF_(0x800401D0L)

//
// MessageId: CLIPBRD_E_CANT_EMPTY
//
// MessageText:
//
//  EmptyClipboard Failed
//
#define CLIPBRD_E_CANT_EMPTY             _HRESULT_TYPEDEF_(0x800401D1L)

//
// MessageId: CLIPBRD_E_CANT_SET
//
// MessageText:
//
//  SetClipboard Failed
//
#define CLIPBRD_E_CANT_SET               _HRESULT_TYPEDEF_(0x800401D2L)

//
// MessageId: CLIPBRD_E_BAD_DATA
//
// MessageText:
//
//  Data on clipboard is invalid
//
#define CLIPBRD_E_BAD_DATA               _HRESULT_TYPEDEF_(0x800401D3L)

//
// MessageId: CLIPBRD_E_CANT_CLOSE
//
// MessageText:
//
//  CloseClipboard Failed
//
#define CLIPBRD_E_CANT_CLOSE             _HRESULT_TYPEDEF_(0x800401D4L)

#define MK_E_FIRST        0x800401E0L
#define MK_E_LAST         0x800401EFL
#define MK_S_FIRST        0x000401E0L
#define MK_S_LAST         0x000401EFL
//
// MessageId: MK_E_CONNECTMANUALLY
//
// MessageText:
//
//  Moniker needs to be connected manually
//
#define MK_E_CONNECTMANUALLY             _HRESULT_TYPEDEF_(0x800401E0L)

//
// MessageId: MK_E_EXCEEDEDDEADLINE
//
// MessageText:
//
//  Operation exceeded deadline
//
#define MK_E_EXCEEDEDDEADLINE            _HRESULT_TYPEDEF_(0x800401E1L)

//
// MessageId: MK_E_NEEDGENERIC
//
// MessageText:
//
//  Moniker needs to be generic
//
#define MK_E_NEEDGENERIC                 _HRESULT_TYPEDEF_(0x800401E2L)

//
// MessageId: MK_E_UNAVAILABLE
//
// MessageText:
//
//  Operation unavailable
//
#define MK_E_UNAVAILABLE                 _HRESULT_TYPEDEF_(0x800401E3L)

//
// MessageId: MK_E_SYNTAX
//
// MessageText:
//
//  Invalid syntax
//
#define MK_E_SYNTAX                      _HRESULT_TYPEDEF_(0x800401E4L)

//
// MessageId: MK_E_NOOBJECT
//
// MessageText:
//
//  No object for moniker
//
#define MK_E_NOOBJECT                    _HRESULT_TYPEDEF_(0x800401E5L)

//
// MessageId: MK_E_INVALIDEXTENSION
//
// MessageText:
//
//  Bad extension for file
//
#define MK_E_INVALIDEXTENSION            _HRESULT_TYPEDEF_(0x800401E6L)

//
// MessageId: MK_E_INTERMEDIATEINTERFACENOTSUPPORTED
//
// MessageText:
//
//  Intermediate operation failed
//
#define MK_E_INTERMEDIATEINTERFACENOTSUPPORTED _HRESULT_TYPEDEF_(0x800401E7L)

//
// MessageId: MK_E_NOTBINDABLE
//
// MessageText:
//
//  Moniker is not bindable
//
#define MK_E_NOTBINDABLE                 _HRESULT_TYPEDEF_(0x800401E8L)

//
// MessageId: MK_E_NOTBOUND
//
// MessageText:
//
//  Moniker is not bound
//
#define MK_E_NOTBOUND                    _HRESULT_TYPEDEF_(0x800401E9L)

//
// MessageId: MK_E_CANTOPENFILE
//
// MessageText:
//
//  Moniker cannot open file
//
#define MK_E_CANTOPENFILE                _HRESULT_TYPEDEF_(0x800401EAL)

//
// MessageId: MK_E_MUSTBOTHERUSER
//
// MessageText:
//
//  User input required for operation to succeed
//
#define MK_E_MUSTBOTHERUSER              _HRESULT_TYPEDEF_(0x800401EBL)

//
// MessageId: MK_E_NOINVERSE
//
// MessageText:
//
//  Moniker class has no inverse
//
#define MK_E_NOINVERSE                   _HRESULT_TYPEDEF_(0x800401ECL)

//
// MessageId: MK_E_NOSTORAGE
//
// MessageText:
//
//  Moniker does not refer to storage
//
#define MK_E_NOSTORAGE                   _HRESULT_TYPEDEF_(0x800401EDL)

//
// MessageId: MK_E_NOPREFIX
//
// MessageText:
//
//  No common prefix
//
#define MK_E_NOPREFIX                    _HRESULT_TYPEDEF_(0x800401EEL)

//
// MessageId: MK_E_ENUMERATION_FAILED
//
// MessageText:
//
//  Moniker could not be enumerated
//
#define MK_E_ENUMERATION_FAILED          _HRESULT_TYPEDEF_(0x800401EFL)

#define CO_E_FIRST        0x800401F0L
#define CO_E_LAST         0x800401FFL
#define CO_S_FIRST        0x000401F0L
#define CO_S_LAST         0x000401FFL
//
// MessageId: CO_E_NOTINITIALIZED
//
// MessageText:
//
//  CoInitialize has not been called.
//
#define CO_E_NOTINITIALIZED              _HRESULT_TYPEDEF_(0x800401F0L)

//
// MessageId: CO_E_ALREADYINITIALIZED
//
// MessageText:
//
//  CoInitialize has already been called.
//
#define CO_E_ALREADYINITIALIZED          _HRESULT_TYPEDEF_(0x800401F1L)

//
// MessageId: CO_E_CANTDETERMINECLASS
//
// MessageText:
//
//  Class of object cannot be determined
//
#define CO_E_CANTDETERMINECLASS          _HRESULT_TYPEDEF_(0x800401F2L)

//
// MessageId: CO_E_CLASSSTRING
//
// MessageText:
//
//  Invalid class string
//
#define CO_E_CLASSSTRING                 _HRESULT_TYPEDEF_(0x800401F3L)

//
// MessageId: CO_E_IIDSTRING
//
// MessageText:
//
//  Invalid interface string
//
#define CO_E_IIDSTRING                   _HRESULT_TYPEDEF_(0x800401F4L)

//
// MessageId: CO_E_APPNOTFOUND
//
// MessageText:
//
//  Application not found
//
#define CO_E_APPNOTFOUND                 _HRESULT_TYPEDEF_(0x800401F5L)

//
// MessageId: CO_E_APPSINGLEUSE
//
// MessageText:
//
//  Application cannot be run more than once
//
#define CO_E_APPSINGLEUSE                _HRESULT_TYPEDEF_(0x800401F6L)

//
// MessageId: CO_E_ERRORINAPP
//
// MessageText:
//
//  Some error in application program
//
#define CO_E_ERRORINAPP                  _HRESULT_TYPEDEF_(0x800401F7L)

//
// MessageId: CO_E_DLLNOTFOUND
//
// MessageText:
//
//  DLL for class not found
//
#define CO_E_DLLNOTFOUND                 _HRESULT_TYPEDEF_(0x800401F8L)

//
// MessageId: CO_E_ERRORINDLL
//
// MessageText:
//
//  Error in the DLL
//
#define CO_E_ERRORINDLL                  _HRESULT_TYPEDEF_(0x800401F9L)

//
// MessageId: CO_E_WRONGOSFORAPP
//
// MessageText:
//
//  Wrong OS or OS version for application
//
#define CO_E_WRONGOSFORAPP               _HRESULT_TYPEDEF_(0x800401FAL)

//
// MessageId: CO_E_OBJNOTREG
//
// MessageText:
//
//  Object is not registered
//
#define CO_E_OBJNOTREG                   _HRESULT_TYPEDEF_(0x800401FBL)

//
// MessageId: CO_E_OBJISREG
//
// MessageText:
//
//  Object is already registered
//
#define CO_E_OBJISREG                    _HRESULT_TYPEDEF_(0x800401FCL)

//
// MessageId: CO_E_OBJNOTCONNECTED
//
// MessageText:
//
//  Object is not connected to server
//
#define CO_E_OBJNOTCONNECTED             _HRESULT_TYPEDEF_(0x800401FDL)

//
// MessageId: CO_E_APPDIDNTREG
//
// MessageText:
//
//  Application was launched but it didn't register a class factory
//
#define CO_E_APPDIDNTREG                 _HRESULT_TYPEDEF_(0x800401FEL)

//
// MessageId: CO_E_RELEASED
//
// MessageText:
//
//  Object has been released
//
#define CO_E_RELEASED                    _HRESULT_TYPEDEF_(0x800401FFL)

#define EVENT_E_FIRST        0x80040200L
#define EVENT_E_LAST         0x8004021FL
#define EVENT_S_FIRST        0x00040200L
#define EVENT_S_LAST         0x0004021FL
//
// MessageId: EVENT_S_SOME_SUBSCRIBERS_FAILED
//
// MessageText:
//
//  An event was able to invoke some but not all of the subscribers
//
#define EVENT_S_SOME_SUBSCRIBERS_FAILED  _HRESULT_TYPEDEF_(0x00040200L)

//
// MessageId: EVENT_E_ALL_SUBSCRIBERS_FAILED
//
// MessageText:
//
//  An event was unable to invoke any of the subscribers
//
#define EVENT_E_ALL_SUBSCRIBERS_FAILED   _HRESULT_TYPEDEF_(0x80040201L)

//
// MessageId: EVENT_S_NOSUBSCRIBERS
//
// MessageText:
//
//  An event was delivered but there were no subscribers
//
#define EVENT_S_NOSUBSCRIBERS            _HRESULT_TYPEDEF_(0x00040202L)

//
// MessageId: EVENT_E_QUERYSYNTAX
//
// MessageText:
//
//  A syntax error occurred trying to evaluate a query string
//
#define EVENT_E_QUERYSYNTAX              _HRESULT_TYPEDEF_(0x80040203L)

//
// MessageId: EVENT_E_QUERYFIELD
//
// MessageText:
//
//  An invalid field name was used in a query string
//
#define EVENT_E_QUERYFIELD               _HRESULT_TYPEDEF_(0x80040204L)

//
// MessageId: EVENT_E_INTERNALEXCEPTION
//
// MessageText:
//
//  An unexpected exception was raised
//
#define EVENT_E_INTERNALEXCEPTION        _HRESULT_TYPEDEF_(0x80040205L)

//
// MessageId: EVENT_E_INTERNALERROR
//
// MessageText:
//
//  An unexpected internal error was detected
//
#define EVENT_E_INTERNALERROR            _HRESULT_TYPEDEF_(0x80040206L)

//
// MessageId: EVENT_E_INVALID_PER_USER_SID
//
// MessageText:
//
//  The owner SID on a per-user subscription doesn't exist
//
#define EVENT_E_INVALID_PER_USER_SID     _HRESULT_TYPEDEF_(0x80040207L)

//
// MessageId: EVENT_E_USER_EXCEPTION
//
// MessageText:
//
//  A user-supplied component or subscriber raised an exception
//
#define EVENT_E_USER_EXCEPTION           _HRESULT_TYPEDEF_(0x80040208L)

//
// MessageId: EVENT_E_TOO_MANY_METHODS
//
// MessageText:
//
//  An interface has too many methods to fire events from
//
#define EVENT_E_TOO_MANY_METHODS         _HRESULT_TYPEDEF_(0x80040209L)

//
// MessageId: EVENT_E_MISSING_EVENTCLASS
//
// MessageText:
//
//  A subscription cannot be stored unless its event class already exists
//
#define EVENT_E_MISSING_EVENTCLASS       _HRESULT_TYPEDEF_(0x8004020AL)

//
// MessageId: EVENT_E_NOT_ALL_REMOVED
//
// MessageText:
//
//  Not all the objects requested could be removed
//
#define EVENT_E_NOT_ALL_REMOVED          _HRESULT_TYPEDEF_(0x8004020BL)

//
// MessageId: EVENT_E_COMPLUS_NOT_INSTALLED
//
// MessageText:
//
//  COM+ is required for this operation, but is not installed
//
#define EVENT_E_COMPLUS_NOT_INSTALLED    _HRESULT_TYPEDEF_(0x8004020CL)

//
// MessageId: EVENT_E_CANT_MODIFY_OR_DELETE_UNCONFIGURED_OBJECT
//
// MessageText:
//
//  Cannot modify or delete an object that was not added using the COM+ Admin SDK
//
#define EVENT_E_CANT_MODIFY_OR_DELETE_UNCONFIGURED_OBJECT _HRESULT_TYPEDEF_(0x8004020DL)

//
// MessageId: EVENT_E_CANT_MODIFY_OR_DELETE_CONFIGURED_OBJECT
//
// MessageText:
//
//  Cannot modify or delete an object that was added using the COM+ Admin SDK
//
#define EVENT_E_CANT_MODIFY_OR_DELETE_CONFIGURED_OBJECT _HRESULT_TYPEDEF_(0x8004020EL)

//
// MessageId: EVENT_E_INVALID_EVENT_CLASS_PARTITION
//
// MessageText:
//
//  The event class for this subscription is in an invalid partition
//
#define EVENT_E_INVALID_EVENT_CLASS_PARTITION _HRESULT_TYPEDEF_(0x8004020FL)

//
// MessageId: EVENT_E_PER_USER_SID_NOT_LOGGED_ON
//
// MessageText:
//
//  The owner of the PerUser subscription is not logged on to the system specified
//
#define EVENT_E_PER_USER_SID_NOT_LOGGED_ON _HRESULT_TYPEDEF_(0x80040210L)

#define XACT_E_FIRST   0x8004D000
#define XACT_E_LAST    0x8004D029
#define XACT_S_FIRST   0x0004D000
#define XACT_S_LAST    0x0004D010
//
// MessageId: XACT_E_ALREADYOTHERSINGLEPHASE
//
// MessageText:
//
//  Another single phase resource manager has already been enlisted in this transaction.
//
#define XACT_E_ALREADYOTHERSINGLEPHASE   _HRESULT_TYPEDEF_(0x8004D000L)

//
// MessageId: XACT_E_CANTRETAIN
//
// MessageText:
//
//  A retaining commit or abort is not supported
//
#define XACT_E_CANTRETAIN                _HRESULT_TYPEDEF_(0x8004D001L)

//
// MessageId: XACT_E_COMMITFAILED
//
// MessageText:
//
//  The transaction failed to commit for an unknown reason. The transaction was aborted.
//
#define XACT_E_COMMITFAILED              _HRESULT_TYPEDEF_(0x8004D002L)

//
// MessageId: XACT_E_COMMITPREVENTED
//
// MessageText:
//
//  Cannot call commit on this transaction object because the calling application did not initiate the transaction.
//
#define XACT_E_COMMITPREVENTED           _HRESULT_TYPEDEF_(0x8004D003L)

//
// MessageId: XACT_E_HEURISTICABORT
//
// MessageText:
//
//  Instead of committing, the resource heuristically aborted.
//
#define XACT_E_HEURISTICABORT            _HRESULT_TYPEDEF_(0x8004D004L)

//
// MessageId: XACT_E_HEURISTICCOMMIT
//
// MessageText:
//
//  Instead of aborting, the resource heuristically committed.
//
#define XACT_E_HEURISTICCOMMIT           _HRESULT_TYPEDEF_(0x8004D005L)

//
// MessageId: XACT_E_HEURISTICDAMAGE
//
// MessageText:
//
//  Some of the states of the resource were committed while others were aborted, likely because of heuristic decisions.
//
#define XACT_E_HEURISTICDAMAGE           _HRESULT_TYPEDEF_(0x8004D006L)

//
// MessageId: XACT_E_HEURISTICDANGER
//
// MessageText:
//
//  Some of the states of the resource may have been committed while others may have been aborted, likely because of heuristic decisions.
//
#define XACT_E_HEURISTICDANGER           _HRESULT_TYPEDEF_(0x8004D007L)

//
// MessageId: XACT_E_ISOLATIONLEVEL
//
// MessageText:
//
//  The requested isolation level is not valid or supported.
//
#define XACT_E_ISOLATIONLEVEL            _HRESULT_TYPEDEF_(0x8004D008L)

//
// MessageId: XACT_E_NOASYNC
//
// MessageText:
//
//  The transaction manager doesn't support an asynchronous operation for this method.
//
#define XACT_E_NOASYNC                   _HRESULT_TYPEDEF_(0x8004D009L)

//
// MessageId: XACT_E_NOENLIST
//
// MessageText:
//
//  Unable to enlist in the transaction.
//
#define XACT_E_NOENLIST                  _HRESULT_TYPEDEF_(0x8004D00AL)

//
// MessageId: XACT_E_NOISORETAIN
//
// MessageText:
//
//  The requested semantics of retention of isolation across retaining commit and abort boundaries cannot be supported by this transaction implementation, or isoFlags was not equal to zero.
//
#define XACT_E_NOISORETAIN               _HRESULT_TYPEDEF_(0x8004D00BL)

//
// MessageId: XACT_E_NORESOURCE
//
// MessageText:
//
//  There is no resource presently associated with this enlistment
//
#define XACT_E_NORESOURCE                _HRESULT_TYPEDEF_(0x8004D00CL)

//
// MessageId: XACT_E_NOTCURRENT
//
// MessageText:
//
//  The transaction failed to commit due to the failure of optimistic concurrency control in at least one of the resource managers.
//
#define XACT_E_NOTCURRENT                _HRESULT_TYPEDEF_(0x8004D00DL)

//
// MessageId: XACT_E_NOTRANSACTION
//
// MessageText:
//
//  The transaction has already been implicitly or explicitly committed or aborted
//
#define XACT_E_NOTRANSACTION             _HRESULT_TYPEDEF_(0x8004D00EL)

//
// MessageId: XACT_E_NOTSUPPORTED
//
// MessageText:
//
//  An invalid combination of flags was specified
//
#define XACT_E_NOTSUPPORTED              _HRESULT_TYPEDEF_(0x8004D00FL)

//
// MessageId: XACT_E_UNKNOWNRMGRID
//
// MessageText:
//
//  The resource manager id is not associated with this transaction or the transaction manager.
//
#define XACT_E_UNKNOWNRMGRID             _HRESULT_TYPEDEF_(0x8004D010L)

//
// MessageId: XACT_E_WRONGSTATE
//
// MessageText:
//
//  This method was called in the wrong state
//
#define XACT_E_WRONGSTATE                _HRESULT_TYPEDEF_(0x8004D011L)

//
// MessageId: XACT_E_WRONGUOW
//
// MessageText:
//
//  The indicated unit of work does not match the unit of work expected by the resource manager.
//
#define XACT_E_WRONGUOW                  _HRESULT_TYPEDEF_(0x8004D012L)

//
// MessageId: XACT_E_XTIONEXISTS
//
// MessageText:
//
//  An enlistment in a transaction already exists.
//
#define XACT_E_XTIONEXISTS               _HRESULT_TYPEDEF_(0x8004D013L)

//
// MessageId: XACT_E_NOIMPORTOBJECT
//
// MessageText:
//
//  An import object for the transaction could not be found.
//
#define XACT_E_NOIMPORTOBJECT            _HRESULT_TYPEDEF_(0x8004D014L)

//
// MessageId: XACT_E_INVALIDCOOKIE
//
// MessageText:
//
//  The transaction cookie is invalid.
//
#define XACT_E_INVALIDCOOKIE             _HRESULT_TYPEDEF_(0x8004D015L)

//
// MessageId: XACT_E_INDOUBT
//
// MessageText:
//
//  The transaction status is in doubt. A communication failure occurred, or a transaction manager or resource manager has failed
//
#define XACT_E_INDOUBT                   _HRESULT_TYPEDEF_(0x8004D016L)

//
// MessageId: XACT_E_NOTIMEOUT
//
// MessageText:
//
//  A time-out was specified, but time-outs are not supported.
//
#define XACT_E_NOTIMEOUT                 _HRESULT_TYPEDEF_(0x8004D017L)

//
// MessageId: XACT_E_ALREADYINPROGRESS
//
// MessageText:
//
//  The requested operation is already in progress for the transaction.
//
#define XACT_E_ALREADYINPROGRESS         _HRESULT_TYPEDEF_(0x8004D018L)

//
// MessageId: XACT_E_ABORTED
//
// MessageText:
//
//  The transaction has already been aborted.
//
#define XACT_E_ABORTED                   _HRESULT_TYPEDEF_(0x8004D019L)

//
// MessageId: XACT_E_LOGFULL
//
// MessageText:
//
//  The Transaction Manager returned a log full error.
//
#define XACT_E_LOGFULL                   _HRESULT_TYPEDEF_(0x8004D01AL)

//
// MessageId: XACT_E_TMNOTAVAILABLE
//
// MessageText:
//
//  The Transaction Manager is not available.
//
#define XACT_E_TMNOTAVAILABLE            _HRESULT_TYPEDEF_(0x8004D01BL)

//
// MessageId: XACT_E_CONNECTION_DOWN
//
// MessageText:
//
//  A connection with the transaction manager was lost.
//
#define XACT_E_CONNECTION_DOWN           _HRESULT_TYPEDEF_(0x8004D01CL)

//
// MessageId: XACT_E_CONNECTION_DENIED
//
// MessageText:
//
//  A request to establish a connection with the transaction manager was denied.
//
#define XACT_E_CONNECTION_DENIED         _HRESULT_TYPEDEF_(0x8004D01DL)

//
// MessageId: XACT_E_REENLISTTIMEOUT
//
// MessageText:
//
//  Resource manager reenlistment to determine transaction status timed out.
//
#define XACT_E_REENLISTTIMEOUT           _HRESULT_TYPEDEF_(0x8004D01EL)

//
// MessageId: XACT_E_TIP_CONNECT_FAILED
//
// MessageText:
//
//  This transaction manager failed to establish a connection with another TIP transaction manager.
//
#define XACT_E_TIP_CONNECT_FAILED        _HRESULT_TYPEDEF_(0x8004D01FL)

//
// MessageId: XACT_E_TIP_PROTOCOL_ERROR
//
// MessageText:
//
//  This transaction manager encountered a protocol error with another TIP transaction manager.
//
#define XACT_E_TIP_PROTOCOL_ERROR        _HRESULT_TYPEDEF_(0x8004D020L)

//
// MessageId: XACT_E_TIP_PULL_FAILED
//
// MessageText:
//
//  This transaction manager could not propagate a transaction from another TIP transaction manager.
//
#define XACT_E_TIP_PULL_FAILED           _HRESULT_TYPEDEF_(0x8004D021L)

//
// MessageId: XACT_E_DEST_TMNOTAVAILABLE
//
// MessageText:
//
//  The Transaction Manager on the destination machine is not available.
//
#define XACT_E_DEST_TMNOTAVAILABLE       _HRESULT_TYPEDEF_(0x8004D022L)

//
// MessageId: XACT_E_TIP_DISABLED
//
// MessageText:
//
//  The Transaction Manager has disabled its support for TIP.
//
#define XACT_E_TIP_DISABLED              _HRESULT_TYPEDEF_(0x8004D023L)

//
// MessageId: XACT_E_NETWORK_TX_DISABLED
//
// MessageText:
//
//  The transaction manager has disabled its support for remote/network transactions.
//
#define XACT_E_NETWORK_TX_DISABLED       _HRESULT_TYPEDEF_(0x8004D024L)

//
// MessageId: XACT_E_PARTNER_NETWORK_TX_DISABLED
//
// MessageText:
//
//  The partner transaction manager has disabled its support for remote/network transactions.
//
#define XACT_E_PARTNER_NETWORK_TX_DISABLED _HRESULT_TYPEDEF_(0x8004D025L)

//
// MessageId: XACT_E_XA_TX_DISABLED
//
// MessageText:
//
//  The transaction manager has disabled its support for XA transactions.
//
#define XACT_E_XA_TX_DISABLED            _HRESULT_TYPEDEF_(0x8004D026L)

//
// MessageId: XACT_E_UNABLE_TO_READ_DTC_CONFIG
//
// MessageText:
//
//  MSDTC was unable to read its configuration information.
//
#define XACT_E_UNABLE_TO_READ_DTC_CONFIG _HRESULT_TYPEDEF_(0x8004D027L)

//
// MessageId: XACT_E_UNABLE_TO_LOAD_DTC_PROXY
//
// MessageText:
//
//  MSDTC was unable to load the dtc proxy dll.
//
#define XACT_E_UNABLE_TO_LOAD_DTC_PROXY  _HRESULT_TYPEDEF_(0x8004D028L)

//
// MessageId: XACT_E_ABORTING
//
// MessageText:
//
//  The local transaction has aborted.
//
#define XACT_E_ABORTING                  _HRESULT_TYPEDEF_(0x8004D029L)

//
// TXF & CRM errors start 4d080.
//
// MessageId: XACT_E_CLERKNOTFOUND
//
// MessageText:
//
//  XACT_E_CLERKNOTFOUND
//
#define XACT_E_CLERKNOTFOUND             _HRESULT_TYPEDEF_(0x8004D080L)

//
// MessageId: XACT_E_CLERKEXISTS
//
// MessageText:
//
//  XACT_E_CLERKEXISTS
//
#define XACT_E_CLERKEXISTS               _HRESULT_TYPEDEF_(0x8004D081L)

//
// MessageId: XACT_E_RECOVERYINPROGRESS
//
// MessageText:
//
//  XACT_E_RECOVERYINPROGRESS
//
#define XACT_E_RECOVERYINPROGRESS        _HRESULT_TYPEDEF_(0x8004D082L)

//
// MessageId: XACT_E_TRANSACTIONCLOSED
//
// MessageText:
//
//  XACT_E_TRANSACTIONCLOSED
//
#define XACT_E_TRANSACTIONCLOSED         _HRESULT_TYPEDEF_(0x8004D083L)

//
// MessageId: XACT_E_INVALIDLSN
//
// MessageText:
//
//  XACT_E_INVALIDLSN
//
#define XACT_E_INVALIDLSN                _HRESULT_TYPEDEF_(0x8004D084L)

//
// MessageId: XACT_E_REPLAYREQUEST
//
// MessageText:
//
//  XACT_E_REPLAYREQUEST
//
#define XACT_E_REPLAYREQUEST             _HRESULT_TYPEDEF_(0x8004D085L)

//
// OleTx Success codes.
//
//
// MessageId: XACT_S_ASYNC
//
// MessageText:
//
//  An asynchronous operation was specified. The operation has begun, but its outcome is not known yet.
//
#define XACT_S_ASYNC                     _HRESULT_TYPEDEF_(0x0004D000L)

//
// MessageId: XACT_S_DEFECT
//
// MessageText:
//
//  XACT_S_DEFECT
//
#define XACT_S_DEFECT                    _HRESULT_TYPEDEF_(0x0004D001L)

//
// MessageId: XACT_S_READONLY
//
// MessageText:
//
//  The method call succeeded because the transaction was read-only.
//
#define XACT_S_READONLY                  _HRESULT_TYPEDEF_(0x0004D002L)

//
// MessageId: XACT_S_SOMENORETAIN
//
// MessageText:
//
//  The transaction was successfully aborted. However, this is a coordinated transaction, and some number of enlisted resources were aborted outright because they could not support abort-retaining semantics
//
#define XACT_S_SOMENORETAIN              _HRESULT_TYPEDEF_(0x0004D003L)

//
// MessageId: XACT_S_OKINFORM
//
// MessageText:
//
//  No changes were made during this call, but the sink wants another chance to look if any other sinks make further changes.
//
#define XACT_S_OKINFORM                  _HRESULT_TYPEDEF_(0x0004D004L)

//
// MessageId: XACT_S_MADECHANGESCONTENT
//
// MessageText:
//
//  The sink is content and wishes the transaction to proceed. Changes were made to one or more resources during this call.
//
#define XACT_S_MADECHANGESCONTENT        _HRESULT_TYPEDEF_(0x0004D005L)

//
// MessageId: XACT_S_MADECHANGESINFORM
//
// MessageText:
//
//  The sink is for the moment and wishes the transaction to proceed, but if other changes are made following this return by other event sinks then this sink wants another chance to look
//
#define XACT_S_MADECHANGESINFORM         _HRESULT_TYPEDEF_(0x0004D006L)

//
// MessageId: XACT_S_ALLNORETAIN
//
// MessageText:
//
//  The transaction was successfully aborted. However, the abort was non-retaining.
//
#define XACT_S_ALLNORETAIN               _HRESULT_TYPEDEF_(0x0004D007L)

//
// MessageId: XACT_S_ABORTING
//
// MessageText:
//
//  An abort operation was already in progress.
//
#define XACT_S_ABORTING                  _HRESULT_TYPEDEF_(0x0004D008L)

//
// MessageId: XACT_S_SINGLEPHASE
//
// MessageText:
//
//  The resource manager has performed a single-phase commit of the transaction.
//
#define XACT_S_SINGLEPHASE               _HRESULT_TYPEDEF_(0x0004D009L)

//
// MessageId: XACT_S_LOCALLY_OK
//
// MessageText:
//
//  The local transaction has not aborted.
//
#define XACT_S_LOCALLY_OK                _HRESULT_TYPEDEF_(0x0004D00AL)

//
// MessageId: XACT_S_LASTRESOURCEMANAGER
//
// MessageText:
//
//  The resource manager has requested to be the coordinator (last resource manager) for the transaction.
//
#define XACT_S_LASTRESOURCEMANAGER       _HRESULT_TYPEDEF_(0x0004D010L)

#define CONTEXT_E_FIRST        0x8004E000L
#define CONTEXT_E_LAST         0x8004E02FL
#define CONTEXT_S_FIRST        0x0004E000L
#define CONTEXT_S_LAST         0x0004E02FL
//
// MessageId: CONTEXT_E_ABORTED
//
// MessageText:
//
//  The root transaction wanted to commit, but transaction aborted
//
#define CONTEXT_E_ABORTED                _HRESULT_TYPEDEF_(0x8004E002L)

//
// MessageId: CONTEXT_E_ABORTING
//
// MessageText:
//
//  You made a method call on a COM+ component that has a transaction that has already aborted or in the process of aborting.
//
#define CONTEXT_E_ABORTING               _HRESULT_TYPEDEF_(0x8004E003L)

//
// MessageId: CONTEXT_E_NOCONTEXT
//
// MessageText:
//
//  There is no MTS object context
//
#define CONTEXT_E_NOCONTEXT              _HRESULT_TYPEDEF_(0x8004E004L)

//
// MessageId: CONTEXT_E_WOULD_DEADLOCK
//
// MessageText:
//
//  The component is configured to use synchronization and this method call would cause a deadlock to occur.
//
#define CONTEXT_E_WOULD_DEADLOCK         _HRESULT_TYPEDEF_(0x8004E005L)

//
// MessageId: CONTEXT_E_SYNCH_TIMEOUT
//
// MessageText:
//
//  The component is configured to use synchronization and a thread has timed out waiting to enter the context.
//
#define CONTEXT_E_SYNCH_TIMEOUT          _HRESULT_TYPEDEF_(0x8004E006L)

//
// MessageId: CONTEXT_E_OLDREF
//
// MessageText:
//
//  You made a method call on a COM+ component that has a transaction that has already committed or aborted.
//
#define CONTEXT_E_OLDREF                 _HRESULT_TYPEDEF_(0x8004E007L)

//
// MessageId: CONTEXT_E_ROLENOTFOUND
//
// MessageText:
//
//  The specified role was not configured for the application
//
#define CONTEXT_E_ROLENOTFOUND           _HRESULT_TYPEDEF_(0x8004E00CL)

//
// MessageId: CONTEXT_E_TMNOTAVAILABLE
//
// MessageText:
//
//  COM+ was unable to talk to the Microsoft Distributed Transaction Coordinator
//
#define CONTEXT_E_TMNOTAVAILABLE         _HRESULT_TYPEDEF_(0x8004E00FL)

//
// MessageId: CO_E_ACTIVATIONFAILED
//
// MessageText:
//
//  An unexpected error occurred during COM+ Activation.
//
#define CO_E_ACTIVATIONFAILED            _HRESULT_TYPEDEF_(0x8004E021L)

//
// MessageId: CO_E_ACTIVATIONFAILED_EVENTLOGGED
//
// MessageText:
//
//  COM+ Activation failed. Check the event log for more information
//
#define CO_E_ACTIVATIONFAILED_EVENTLOGGED _HRESULT_TYPEDEF_(0x8004E022L)

//
// MessageId: CO_E_ACTIVATIONFAILED_CATALOGERROR
//
// MessageText:
//
//  COM+ Activation failed due to a catalog or configuration error.
//
#define CO_E_ACTIVATIONFAILED_CATALOGERROR _HRESULT_TYPEDEF_(0x8004E023L)

//
// MessageId: CO_E_ACTIVATIONFAILED_TIMEOUT
//
// MessageText:
//
//  COM+ activation failed because the activation could not be completed in the specified amount of time.
//
#define CO_E_ACTIVATIONFAILED_TIMEOUT    _HRESULT_TYPEDEF_(0x8004E024L)

//
// MessageId: CO_E_INITIALIZATIONFAILED
//
// MessageText:
//
//  COM+ Activation failed because an initialization function failed.  Check the event log for more information.
//
#define CO_E_INITIALIZATIONFAILED        _HRESULT_TYPEDEF_(0x8004E025L)

//
// MessageId: CONTEXT_E_NOJIT
//
// MessageText:
//
//  The requested operation requires that JIT be in the current context and it is not
//
#define CONTEXT_E_NOJIT                  _HRESULT_TYPEDEF_(0x8004E026L)

//
// MessageId: CONTEXT_E_NOTRANSACTION
//
// MessageText:
//
//  The requested operation requires that the current context have a Transaction, and it does not
//
#define CONTEXT_E_NOTRANSACTION          _HRESULT_TYPEDEF_(0x8004E027L)

//
// MessageId: CO_E_THREADINGMODEL_CHANGED
//
// MessageText:
//
//  The components threading model has changed after install into a COM+ Application.  Please re-install component.
//
#define CO_E_THREADINGMODEL_CHANGED      _HRESULT_TYPEDEF_(0x8004E028L)

//
// MessageId: CO_E_NOIISINTRINSICS
//
// MessageText:
//
//  IIS intrinsics not available.  Start your work with IIS.
//
#define CO_E_NOIISINTRINSICS             _HRESULT_TYPEDEF_(0x8004E029L)

//
// MessageId: CO_E_NOCOOKIES
//
// MessageText:
//
//  An attempt to write a cookie failed.
//
#define CO_E_NOCOOKIES                   _HRESULT_TYPEDEF_(0x8004E02AL)

//
// MessageId: CO_E_DBERROR
//
// MessageText:
//
//  An attempt to use a database generated a database specific error.
//
#define CO_E_DBERROR                     _HRESULT_TYPEDEF_(0x8004E02BL)

//
// MessageId: CO_E_NOTPOOLED
//
// MessageText:
//
//  The COM+ component you created must use object pooling to work.
//
#define CO_E_NOTPOOLED                   _HRESULT_TYPEDEF_(0x8004E02CL)

//
// MessageId: CO_E_NOTCONSTRUCTED
//
// MessageText:
//
//  The COM+ component you created must use object construction to work correctly.
//
#define CO_E_NOTCONSTRUCTED              _HRESULT_TYPEDEF_(0x8004E02DL)

//
// MessageId: CO_E_NOSYNCHRONIZATION
//
// MessageText:
//
//  The COM+ component requires synchronization, and it is not configured for it.
//
#define CO_E_NOSYNCHRONIZATION           _HRESULT_TYPEDEF_(0x8004E02EL)

//
// MessageId: CO_E_ISOLEVELMISMATCH
//
// MessageText:
//
//  The TxIsolation Level property for the COM+ component being created is stronger than the TxIsolationLevel for the "root" component for the transaction.  The creation failed.
//
#define CO_E_ISOLEVELMISMATCH            _HRESULT_TYPEDEF_(0x8004E02FL)

//
// Old OLE Success Codes
//
//
// MessageId: OLE_S_USEREG
//
// MessageText:
//
//  Use the registry database to provide the requested information
//
#define OLE_S_USEREG                     _HRESULT_TYPEDEF_(0x00040000L)

//
// MessageId: OLE_S_STATIC
//
// MessageText:
//
//  Success, but static
//
#define OLE_S_STATIC                     _HRESULT_TYPEDEF_(0x00040001L)

//
// MessageId: OLE_S_MAC_CLIPFORMAT
//
// MessageText:
//
//  Macintosh clipboard format
//
#define OLE_S_MAC_CLIPFORMAT             _HRESULT_TYPEDEF_(0x00040002L)

//
// MessageId: DRAGDROP_S_DROP
//
// MessageText:
//
//  Successful drop took place
//
#define DRAGDROP_S_DROP                  _HRESULT_TYPEDEF_(0x00040100L)

//
// MessageId: DRAGDROP_S_CANCEL
//
// MessageText:
//
//  Drag-drop operation canceled
//
#define DRAGDROP_S_CANCEL                _HRESULT_TYPEDEF_(0x00040101L)

//
// MessageId: DRAGDROP_S_USEDEFAULTCURSORS
//
// MessageText:
//
//  Use the default cursor
//
#define DRAGDROP_S_USEDEFAULTCURSORS     _HRESULT_TYPEDEF_(0x00040102L)

//
// MessageId: DATA_S_SAMEFORMATETC
//
// MessageText:
//
//  Data has same FORMATETC
//
#define DATA_S_SAMEFORMATETC             _HRESULT_TYPEDEF_(0x00040130L)

//
// MessageId: VIEW_S_ALREADY_FROZEN
//
// MessageText:
//
//  View is already frozen
//
#define VIEW_S_ALREADY_FROZEN            _HRESULT_TYPEDEF_(0x00040140L)

//
// MessageId: CACHE_S_FORMATETC_NOTSUPPORTED
//
// MessageText:
//
//  FORMATETC not supported
//
#define CACHE_S_FORMATETC_NOTSUPPORTED   _HRESULT_TYPEDEF_(0x00040170L)

//
// MessageId: CACHE_S_SAMECACHE
//
// MessageText:
//
//  Same cache
//
#define CACHE_S_SAMECACHE                _HRESULT_TYPEDEF_(0x00040171L)

//
// MessageId: CACHE_S_SOMECACHES_NOTUPDATED
//
// MessageText:
//
//  Some cache(s) not updated
//
#define CACHE_S_SOMECACHES_NOTUPDATED    _HRESULT_TYPEDEF_(0x00040172L)

//
// MessageId: OLEOBJ_S_INVALIDVERB
//
// MessageText:
//
//  Invalid verb for OLE object
//
#define OLEOBJ_S_INVALIDVERB             _HRESULT_TYPEDEF_(0x00040180L)

//
// MessageId: OLEOBJ_S_CANNOT_DOVERB_NOW
//
// MessageText:
//
//  Verb number is valid but verb cannot be done now
//
#define OLEOBJ_S_CANNOT_DOVERB_NOW       _HRESULT_TYPEDEF_(0x00040181L)

//
// MessageId: OLEOBJ_S_INVALIDHWND
//
// MessageText:
//
//  Invalid window handle passed
//
#define OLEOBJ_S_INVALIDHWND             _HRESULT_TYPEDEF_(0x00040182L)

//
// MessageId: INPLACE_S_TRUNCATED
//
// MessageText:
//
//  Message is too long; some of it had to be truncated before displaying
//
#define INPLACE_S_TRUNCATED              _HRESULT_TYPEDEF_(0x000401A0L)

//
// MessageId: CONVERT10_S_NO_PRESENTATION
//
// MessageText:
//
//  Unable to convert OLESTREAM to IStorage
//
#define CONVERT10_S_NO_PRESENTATION      _HRESULT_TYPEDEF_(0x000401C0L)

//
// MessageId: MK_S_REDUCED_TO_SELF
//
// MessageText:
//
//  Moniker reduced to itself
//
#define MK_S_REDUCED_TO_SELF             _HRESULT_TYPEDEF_(0x000401E2L)

//
// MessageId: MK_S_ME
//
// MessageText:
//
//  Common prefix is this moniker
//
#define MK_S_ME                          _HRESULT_TYPEDEF_(0x000401E4L)

//
// MessageId: MK_S_HIM
//
// MessageText:
//
//  Common prefix is input moniker
//
#define MK_S_HIM                         _HRESULT_TYPEDEF_(0x000401E5L)

//
// MessageId: MK_S_US
//
// MessageText:
//
//  Common prefix is both monikers
//
#define MK_S_US                          _HRESULT_TYPEDEF_(0x000401E6L)

//
// MessageId: MK_S_MONIKERALREADYREGISTERED
//
// MessageText:
//
//  Moniker is already registered in running object table
//
#define MK_S_MONIKERALREADYREGISTERED    _HRESULT_TYPEDEF_(0x000401E7L)

//
// Task Scheduler errors
//
//
// MessageId: SCHED_S_TASK_READY
//
// MessageText:
//
//  The task is ready to run at its next scheduled time.
//
#define SCHED_S_TASK_READY               _HRESULT_TYPEDEF_(0x00041300L)

//
// MessageId: SCHED_S_TASK_RUNNING
//
// MessageText:
//
//  The task is currently running.
//
#define SCHED_S_TASK_RUNNING             _HRESULT_TYPEDEF_(0x00041301L)

//
// MessageId: SCHED_S_TASK_DISABLED
//
// MessageText:
//
//  The task will not run at the scheduled times because it has been disabled.
//
#define SCHED_S_TASK_DISABLED            _HRESULT_TYPEDEF_(0x00041302L)

//
// MessageId: SCHED_S_TASK_HAS_NOT_RUN
//
// MessageText:
//
//  The task has not yet run.
//
#define SCHED_S_TASK_HAS_NOT_RUN         _HRESULT_TYPEDEF_(0x00041303L)

//
// MessageId: SCHED_S_TASK_NO_MORE_RUNS
//
// MessageText:
//
//  There are no more runs scheduled for this task.
//
#define SCHED_S_TASK_NO_MORE_RUNS        _HRESULT_TYPEDEF_(0x00041304L)

//
// MessageId: SCHED_S_TASK_NOT_SCHEDULED
//
// MessageText:
//
//  One or more of the properties that are needed to run this task on a schedule have not been set.
//
#define SCHED_S_TASK_NOT_SCHEDULED       _HRESULT_TYPEDEF_(0x00041305L)

//
// MessageId: SCHED_S_TASK_TERMINATED
//
// MessageText:
//
//  The last run of the task was terminated by the user.
//
#define SCHED_S_TASK_TERMINATED          _HRESULT_TYPEDEF_(0x00041306L)

//
// MessageId: SCHED_S_TASK_NO_VALID_TRIGGERS
//
// MessageText:
//
//  Either the task has no triggers or the existing triggers are disabled or not set.
//
#define SCHED_S_TASK_NO_VALID_TRIGGERS   _HRESULT_TYPEDEF_(0x00041307L)

//
// MessageId: SCHED_S_EVENT_TRIGGER
//
// MessageText:
//
//  Event triggers don't have set run times.
//
#define SCHED_S_EVENT_TRIGGER            _HRESULT_TYPEDEF_(0x00041308L)

//
// MessageId: SCHED_E_TRIGGER_NOT_FOUND
//
// MessageText:
//
//  Trigger not found.
//
#define SCHED_E_TRIGGER_NOT_FOUND        _HRESULT_TYPEDEF_(0x80041309L)

//
// MessageId: SCHED_E_TASK_NOT_READY
//
// MessageText:
//
//  One or more of the properties that are needed to run this task have not been set.
//
#define SCHED_E_TASK_NOT_READY           _HRESULT_TYPEDEF_(0x8004130AL)

//
// MessageId: SCHED_E_TASK_NOT_RUNNING
//
// MessageText:
//
//  There is no running instance of the task to terminate.
//
#define SCHED_E_TASK_NOT_RUNNING         _HRESULT_TYPEDEF_(0x8004130BL)

//
// MessageId: SCHED_E_SERVICE_NOT_INSTALLED
//
// MessageText:
//
//  The Task Scheduler Service is not installed on this computer.
//
#define SCHED_E_SERVICE_NOT_INSTALLED    _HRESULT_TYPEDEF_(0x8004130CL)

//
// MessageId: SCHED_E_CANNOT_OPEN_TASK
//
// MessageText:
//
//  The task object could not be opened.
//
#define SCHED_E_CANNOT_OPEN_TASK         _HRESULT_TYPEDEF_(0x8004130DL)

//
// MessageId: SCHED_E_INVALID_TASK
//
// MessageText:
//
//  The object is either an invalid task object or is not a task object.
//
#define SCHED_E_INVALID_TASK             _HRESULT_TYPEDEF_(0x8004130EL)

//
// MessageId: SCHED_E_ACCOUNT_INFORMATION_NOT_SET
//
// MessageText:
//
//  No account information could be found in the Task Scheduler security database for the task indicated.
//
#define SCHED_E_ACCOUNT_INFORMATION_NOT_SET _HRESULT_TYPEDEF_(0x8004130FL)

//
// MessageId: SCHED_E_ACCOUNT_NAME_NOT_FOUND
//
// MessageText:
//
//  Unable to establish existence of the account specified.
//
#define SCHED_E_ACCOUNT_NAME_NOT_FOUND   _HRESULT_TYPEDEF_(0x80041310L)

//
// MessageId: SCHED_E_ACCOUNT_DBASE_CORRUPT
//
// MessageText:
//
//  Corruption was detected in the Task Scheduler security database; the database has been reset.
//
#define SCHED_E_ACCOUNT_DBASE_CORRUPT    _HRESULT_TYPEDEF_(0x80041311L)

//
// MessageId: SCHED_E_NO_SECURITY_SERVICES
//
// MessageText:
//
//  Task Scheduler security services are available only on Windows NT.
//
#define SCHED_E_NO_SECURITY_SERVICES     _HRESULT_TYPEDEF_(0x80041312L)

//
// MessageId: SCHED_E_UNKNOWN_OBJECT_VERSION
//
// MessageText:
//
//  The task object version is either unsupported or invalid.
//
#define SCHED_E_UNKNOWN_OBJECT_VERSION   _HRESULT_TYPEDEF_(0x80041313L)

//
// MessageId: SCHED_E_UNSUPPORTED_ACCOUNT_OPTION
//
// MessageText:
//
//  The task has been configured with an unsupported combination of account settings and run time options.
//
#define SCHED_E_UNSUPPORTED_ACCOUNT_OPTION _HRESULT_TYPEDEF_(0x80041314L)

//
// MessageId: SCHED_E_SERVICE_NOT_RUNNING
//
// MessageText:
//
//  The Task Scheduler Service is not running.
//
#define SCHED_E_SERVICE_NOT_RUNNING      _HRESULT_TYPEDEF_(0x80041315L)

// ******************
// FACILITY_WINDOWS
// ******************
//
// Codes 0x0-0x01ff are reserved for the OLE group of
// interfaces.
//
//
// MessageId: CO_E_CLASS_CREATE_FAILED
//
// MessageText:
//
//  Attempt to create a class object failed
//
#define CO_E_CLASS_CREATE_FAILED         _HRESULT_TYPEDEF_(0x80080001L)

//
// MessageId: CO_E_SCM_ERROR
//
// MessageText:
//
//  OLE service could not bind object
//
#define CO_E_SCM_ERROR                   _HRESULT_TYPEDEF_(0x80080002L)

//
// MessageId: CO_E_SCM_RPC_FAILURE
//
// MessageText:
//
//  RPC communication failed with OLE service
//
#define CO_E_SCM_RPC_FAILURE             _HRESULT_TYPEDEF_(0x80080003L)

//
// MessageId: CO_E_BAD_PATH
//
// MessageText:
//
//  Bad path to object
//
#define CO_E_BAD_PATH                    _HRESULT_TYPEDEF_(0x80080004L)

//
// MessageId: CO_E_SERVER_EXEC_FAILURE
//
// MessageText:
//
//  Server execution failed
//
#define CO_E_SERVER_EXEC_FAILURE         _HRESULT_TYPEDEF_(0x80080005L)

//
// MessageId: CO_E_OBJSRV_RPC_FAILURE
//
// MessageText:
//
//  OLE service could not communicate with the object server
//
#define CO_E_OBJSRV_RPC_FAILURE          _HRESULT_TYPEDEF_(0x80080006L)

//
// MessageId: MK_E_NO_NORMALIZED
//
// MessageText:
//
//  Moniker path could not be normalized
//
#define MK_E_NO_NORMALIZED               _HRESULT_TYPEDEF_(0x80080007L)

//
// MessageId: CO_E_SERVER_STOPPING
//
// MessageText:
//
//  Object server is stopping when OLE service contacts it
//
#define CO_E_SERVER_STOPPING             _HRESULT_TYPEDEF_(0x80080008L)

//
// MessageId: MEM_E_INVALID_ROOT
//
// MessageText:
//
//  An invalid root block pointer was specified
//
#define MEM_E_INVALID_ROOT               _HRESULT_TYPEDEF_(0x80080009L)

//
// MessageId: MEM_E_INVALID_LINK
//
// MessageText:
//
//  An allocation chain contained an invalid link pointer
//
#define MEM_E_INVALID_LINK               _HRESULT_TYPEDEF_(0x80080010L)

//
// MessageId: MEM_E_INVALID_SIZE
//
// MessageText:
//
//  The requested allocation size was too large
//
#define MEM_E_INVALID_SIZE               _HRESULT_TYPEDEF_(0x80080011L)

//
// MessageId: CO_S_NOTALLINTERFACES
//
// MessageText:
//
//  Not all the requested interfaces were available
//
#define CO_S_NOTALLINTERFACES            _HRESULT_TYPEDEF_(0x00080012L)

//
// MessageId: CO_S_MACHINENAMENOTFOUND
//
// MessageText:
//
//  The specified machine name was not found in the cache.
//
#define CO_S_MACHINENAMENOTFOUND         _HRESULT_TYPEDEF_(0x00080013L)

// ******************
// FACILITY_DISPATCH
// ******************
//
// MessageId: DISP_E_UNKNOWNINTERFACE
//
// MessageText:
//
//  Unknown interface.
//
#define DISP_E_UNKNOWNINTERFACE          _HRESULT_TYPEDEF_(0x80020001L)

//
// MessageId: DISP_E_MEMBERNOTFOUND
//
// MessageText:
//
//  Member not found.
//
#define DISP_E_MEMBERNOTFOUND            _HRESULT_TYPEDEF_(0x80020003L)

//
// MessageId: DISP_E_PARAMNOTFOUND
//
// MessageText:
//
//  Parameter not found.
//
#define DISP_E_PARAMNOTFOUND             _HRESULT_TYPEDEF_(0x80020004L)

//
// MessageId: DISP_E_TYPEMISMATCH
//
// MessageText:
//
//  Type mismatch.
//
#define DISP_E_TYPEMISMATCH              _HRESULT_TYPEDEF_(0x80020005L)

//
// MessageId: DISP_E_UNKNOWNNAME
//
// MessageText:
//
//  Unknown name.
//
#define DISP_E_UNKNOWNNAME               _HRESULT_TYPEDEF_(0x80020006L)

//
// MessageId: DISP_E_NONAMEDARGS
//
// MessageText:
//
//  No named arguments.
//
#define DISP_E_NONAMEDARGS               _HRESULT_TYPEDEF_(0x80020007L)

//
// MessageId: DISP_E_BADVARTYPE
//
// MessageText:
//
//  Bad variable type.
//
#define DISP_E_BADVARTYPE                _HRESULT_TYPEDEF_(0x80020008L)

//
// MessageId: DISP_E_EXCEPTION
//
// MessageText:
//
//  Exception occurred.
//
#define DISP_E_EXCEPTION                 _HRESULT_TYPEDEF_(0x80020009L)

//
// MessageId: DISP_E_OVERFLOW
//
// MessageText:
//
//  Out of present range.
//
#define DISP_E_OVERFLOW                  _HRESULT_TYPEDEF_(0x8002000AL)

//
// MessageId: DISP_E_BADINDEX
//
// MessageText:
//
//  Invalid index.
//
#define DISP_E_BADINDEX                  _HRESULT_TYPEDEF_(0x8002000BL)

//
// MessageId: DISP_E_UNKNOWNLCID
//
// MessageText:
//
//  Unknown language.
//
#define DISP_E_UNKNOWNLCID               _HRESULT_TYPEDEF_(0x8002000CL)

//
// MessageId: DISP_E_ARRAYISLOCKED
//
// MessageText:
//
//  Memory is locked.
//
#define DISP_E_ARRAYISLOCKED             _HRESULT_TYPEDEF_(0x8002000DL)

//
// MessageId: DISP_E_BADPARAMCOUNT
//
// MessageText:
//
//  Invalid number of parameters.
//
#define DISP_E_BADPARAMCOUNT             _HRESULT_TYPEDEF_(0x8002000EL)

//
// MessageId: DISP_E_PARAMNOTOPTIONAL
//
// MessageText:
//
//  Parameter not optional.
//
#define DISP_E_PARAMNOTOPTIONAL          _HRESULT_TYPEDEF_(0x8002000FL)

//
// MessageId: DISP_E_BADCALLEE
//
// MessageText:
//
//  Invalid callee.
//
#define DISP_E_BADCALLEE                 _HRESULT_TYPEDEF_(0x80020010L)

//
// MessageId: DISP_E_NOTACOLLECTION
//
// MessageText:
//
//  Does not support a collection.
//
#define DISP_E_NOTACOLLECTION            _HRESULT_TYPEDEF_(0x80020011L)

//
// MessageId: DISP_E_DIVBYZERO
//
// MessageText:
//
//  Division by zero.
//
#define DISP_E_DIVBYZERO                 _HRESULT_TYPEDEF_(0x80020012L)

//
// MessageId: DISP_E_BUFFERTOOSMALL
//
// MessageText:
//
//  Buffer too small
//
#define DISP_E_BUFFERTOOSMALL            _HRESULT_TYPEDEF_(0x80020013L)

//
// MessageId: TYPE_E_BUFFERTOOSMALL
//
// MessageText:
//
//  Buffer too small.
//
#define TYPE_E_BUFFERTOOSMALL            _HRESULT_TYPEDEF_(0x80028016L)

//
// MessageId: TYPE_E_FIELDNOTFOUND
//
// MessageText:
//
//  Field name not defined in the record.
//
#define TYPE_E_FIELDNOTFOUND             _HRESULT_TYPEDEF_(0x80028017L)

//
// MessageId: TYPE_E_INVDATAREAD
//
// MessageText:
//
//  Old format or invalid type library.
//
#define TYPE_E_INVDATAREAD               _HRESULT_TYPEDEF_(0x80028018L)

//
// MessageId: TYPE_E_UNSUPFORMAT
//
// MessageText:
//
//  Old format or invalid type library.
//
#define TYPE_E_UNSUPFORMAT               _HRESULT_TYPEDEF_(0x80028019L)

//
// MessageId: TYPE_E_REGISTRYACCESS
//
// MessageText:
//
//  Error accessing the OLE registry.
//
#define TYPE_E_REGISTRYACCESS            _HRESULT_TYPEDEF_(0x8002801CL)

//
// MessageId: TYPE_E_LIBNOTREGISTERED
//
// MessageText:
//
//  Library not registered.
//
#define TYPE_E_LIBNOTREGISTERED          _HRESULT_TYPEDEF_(0x8002801DL)

//
// MessageId: TYPE_E_UNDEFINEDTYPE
//
// MessageText:
//
//  Bound to unknown type.
//
#define TYPE_E_UNDEFINEDTYPE             _HRESULT_TYPEDEF_(0x80028027L)

//
// MessageId: TYPE_E_QUALIFIEDNAMEDISALLOWED
//
// MessageText:
//
//  Qualified name disallowed.
//
#define TYPE_E_QUALIFIEDNAMEDISALLOWED   _HRESULT_TYPEDEF_(0x80028028L)

//
// MessageId: TYPE_E_INVALIDSTATE
//
// MessageText:
//
//  Invalid forward reference, or reference to uncompiled type.
//
#define TYPE_E_INVALIDSTATE              _HRESULT_TYPEDEF_(0x80028029L)

//
// MessageId: TYPE_E_WRONGTYPEKIND
//
// MessageText:
//
//  Type mismatch.
//
#define TYPE_E_WRONGTYPEKIND             _HRESULT_TYPEDEF_(0x8002802AL)

//
// MessageId: TYPE_E_ELEMENTNOTFOUND
//
// MessageText:
//
//  Element not found.
//
#define TYPE_E_ELEMENTNOTFOUND           _HRESULT_TYPEDEF_(0x8002802BL)

//
// MessageId: TYPE_E_AMBIGUOUSNAME
//
// MessageText:
//
//  Ambiguous name.
//
#define TYPE_E_AMBIGUOUSNAME             _HRESULT_TYPEDEF_(0x8002802CL)

//
// MessageId: TYPE_E_NAMECONFLICT
//
// MessageText:
//
//  Name already exists in the library.
//
#define TYPE_E_NAMECONFLICT              _HRESULT_TYPEDEF_(0x8002802DL)

//
// MessageId: TYPE_E_UNKNOWNLCID
//
// MessageText:
//
//  Unknown LCID.
//
#define TYPE_E_UNKNOWNLCID               _HRESULT_TYPEDEF_(0x8002802EL)

//
// MessageId: TYPE_E_DLLFUNCTIONNOTFOUND
//
// MessageText:
//
//  Function not defined in specified DLL.
//
#define TYPE_E_DLLFUNCTIONNOTFOUND       _HRESULT_TYPEDEF_(0x8002802FL)

//
// MessageId: TYPE_E_BADMODULEKIND
//
// MessageText:
//
//  Wrong module kind for the operation.
//
#define TYPE_E_BADMODULEKIND             _HRESULT_TYPEDEF_(0x800288BDL)

//
// MessageId: TYPE_E_SIZETOOBIG
//
// MessageText:
//
//  Size may not exceed 64K.
//
#define TYPE_E_SIZETOOBIG                _HRESULT_TYPEDEF_(0x800288C5L)

//
// MessageId: TYPE_E_DUPLICATEID
//
// MessageText:
//
//  Duplicate ID in inheritance hierarchy.
//
#define TYPE_E_DUPLICATEID               _HRESULT_TYPEDEF_(0x800288C6L)

//
// MessageId: TYPE_E_INVALIDID
//
// MessageText:
//
//  Incorrect inheritance depth in standard OLE hmember.
//
#define TYPE_E_INVALIDID                 _HRESULT_TYPEDEF_(0x800288CFL)

//
// MessageId: TYPE_E_TYPEMISMATCH
//
// MessageText:
//
//  Type mismatch.
//
#define TYPE_E_TYPEMISMATCH              _HRESULT_TYPEDEF_(0x80028CA0L)

//
// MessageId: TYPE_E_OUTOFBOUNDS
//
// MessageText:
//
//  Invalid number of arguments.
//
#define TYPE_E_OUTOFBOUNDS               _HRESULT_TYPEDEF_(0x80028CA1L)

//
// MessageId: TYPE_E_IOERROR
//
// MessageText:
//
//  I/O Error.
//
#define TYPE_E_IOERROR                   _HRESULT_TYPEDEF_(0x80028CA2L)

//
// MessageId: TYPE_E_CANTCREATETMPFILE
//
// MessageText:
//
//  Error creating unique tmp file.
//
#define TYPE_E_CANTCREATETMPFILE         _HRESULT_TYPEDEF_(0x80028CA3L)

//
// MessageId: TYPE_E_CANTLOADLIBRARY
//
// MessageText:
//
//  Error loading type library/DLL.
//
#define TYPE_E_CANTLOADLIBRARY           _HRESULT_TYPEDEF_(0x80029C4AL)

//
// MessageId: TYPE_E_INCONSISTENTPROPFUNCS
//
// MessageText:
//
//  Inconsistent property functions.
//
#define TYPE_E_INCONSISTENTPROPFUNCS     _HRESULT_TYPEDEF_(0x80029C83L)

//
// MessageId: TYPE_E_CIRCULARTYPE
//
// MessageText:
//
//  Circular dependency between types/modules.
//
#define TYPE_E_CIRCULARTYPE              _HRESULT_TYPEDEF_(0x80029C84L)

// ******************
// FACILITY_STORAGE
// ******************
//
// MessageId: STG_E_INVALIDFUNCTION
//
// MessageText:
//
//  Unable to perform requested operation.
//
#define STG_E_INVALIDFUNCTION            _HRESULT_TYPEDEF_(0x80030001L)

//
// MessageId: STG_E_FILENOTFOUND
//
// MessageText:
//
//  %1 could not be found.
//
#define STG_E_FILENOTFOUND               _HRESULT_TYPEDEF_(0x80030002L)

//
// MessageId: STG_E_PATHNOTFOUND
//
// MessageText:
//
//  The path %1 could not be found.
//
#define STG_E_PATHNOTFOUND               _HRESULT_TYPEDEF_(0x80030003L)

//
// MessageId: STG_E_TOOMANYOPENFILES
//
// MessageText:
//
//  There are insufficient resources to open another file.
//
#define STG_E_TOOMANYOPENFILES           _HRESULT_TYPEDEF_(0x80030004L)

//
// MessageId: STG_E_ACCESSDENIED
//
// MessageText:
//
//  Access Denied.
//
#define STG_E_ACCESSDENIED               _HRESULT_TYPEDEF_(0x80030005L)

//
// MessageId: STG_E_INVALIDHANDLE
//
// MessageText:
//
//  Attempted an operation on an invalid object.
//
#define STG_E_INVALIDHANDLE              _HRESULT_TYPEDEF_(0x80030006L)

//
// MessageId: STG_E_INSUFFICIENTMEMORY
//
// MessageText:
//
//  There is insufficient memory available to complete operation.
//
#define STG_E_INSUFFICIENTMEMORY         _HRESULT_TYPEDEF_(0x80030008L)

//
// MessageId: STG_E_INVALIDPOINTER
//
// MessageText:
//
//  Invalid pointer error.
//
#define STG_E_INVALIDPOINTER             _HRESULT_TYPEDEF_(0x80030009L)

//
// MessageId: STG_E_NOMOREFILES
//
// MessageText:
//
//  There are no more entries to return.
//
#define STG_E_NOMOREFILES                _HRESULT_TYPEDEF_(0x80030012L)

//
// MessageId: STG_E_DISKISWRITEPROTECTED
//
// MessageText:
//
//  Disk is write-protected.
//
#define STG_E_DISKISWRITEPROTECTED       _HRESULT_TYPEDEF_(0x80030013L)

//
// MessageId: STG_E_SEEKERROR
//
// MessageText:
//
//  An error occurred during a seek operation.
//
#define STG_E_SEEKERROR                  _HRESULT_TYPEDEF_(0x80030019L)

//
// MessageId: STG_E_WRITEFAULT
//
// MessageText:
//
//  A disk error occurred during a write operation.
//
#define STG_E_WRITEFAULT                 _HRESULT_TYPEDEF_(0x8003001DL)

//
// MessageId: STG_E_READFAULT
//
// MessageText:
//
//  A disk error occurred during a read operation.
//
#define STG_E_READFAULT                  _HRESULT_TYPEDEF_(0x8003001EL)

//
// MessageId: STG_E_SHAREVIOLATION
//
// MessageText:
//
//  A share violation has occurred.
//
#define STG_E_SHAREVIOLATION             _HRESULT_TYPEDEF_(0x80030020L)

//
// MessageId: STG_E_LOCKVIOLATION
//
// MessageText:
//
//  A lock violation has occurred.
//
#define STG_E_LOCKVIOLATION              _HRESULT_TYPEDEF_(0x80030021L)

//
// MessageId: STG_E_FILEALREADYEXISTS
//
// MessageText:
//
//  %1 already exists.
//
#define STG_E_FILEALREADYEXISTS          _HRESULT_TYPEDEF_(0x80030050L)

//
// MessageId: STG_E_INVALIDPARAMETER
//
// MessageText:
//
//  Invalid parameter error.
//
#define STG_E_INVALIDPARAMETER           _HRESULT_TYPEDEF_(0x80030057L)

//
// MessageId: STG_E_MEDIUMFULL
//
// MessageText:
//
//  There is insufficient disk space to complete operation.
//
#define STG_E_MEDIUMFULL                 _HRESULT_TYPEDEF_(0x80030070L)

//
// MessageId: STG_E_PROPSETMISMATCHED
//
// MessageText:
//
//  Illegal write of non-simple property to simple property set.
//
#define STG_E_PROPSETMISMATCHED          _HRESULT_TYPEDEF_(0x800300F0L)

//
// MessageId: STG_E_ABNORMALAPIEXIT
//
// MessageText:
//
//  An API call exited abnormally.
//
#define STG_E_ABNORMALAPIEXIT            _HRESULT_TYPEDEF_(0x800300FAL)

//
// MessageId: STG_E_INVALIDHEADER
//
// MessageText:
//
//  The file %1 is not a valid compound file.
//
#define STG_E_INVALIDHEADER              _HRESULT_TYPEDEF_(0x800300FBL)

//
// MessageId: STG_E_INVALIDNAME
//
// MessageText:
//
//  The name %1 is not valid.
//
#define STG_E_INVALIDNAME                _HRESULT_TYPEDEF_(0x800300FCL)

//
// MessageId: STG_E_UNKNOWN
//
// MessageText:
//
//  An unexpected error occurred.
//
#define STG_E_UNKNOWN                    _HRESULT_TYPEDEF_(0x800300FDL)

//
// MessageId: STG_E_UNIMPLEMENTEDFUNCTION
//
// MessageText:
//
//  That function is not implemented.
//
#define STG_E_UNIMPLEMENTEDFUNCTION      _HRESULT_TYPEDEF_(0x800300FEL)

//
// MessageId: STG_E_INVALIDFLAG
//
// MessageText:
//
//  Invalid flag error.
//
#define STG_E_INVALIDFLAG                _HRESULT_TYPEDEF_(0x800300FFL)

//
// MessageId: STG_E_INUSE
//
// MessageText:
//
//  Attempted to use an object that is busy.
//
#define STG_E_INUSE                      _HRESULT_TYPEDEF_(0x80030100L)

//
// MessageId: STG_E_NOTCURRENT
//
// MessageText:
//
//  The storage has been changed since the last commit.
//
#define STG_E_NOTCURRENT                 _HRESULT_TYPEDEF_(0x80030101L)

//
// MessageId: STG_E_REVERTED
//
// MessageText:
//
//  Attempted to use an object that has ceased to exist.
//
#define STG_E_REVERTED                   _HRESULT_TYPEDEF_(0x80030102L)

//
// MessageId: STG_E_CANTSAVE
//
// MessageText:
//
//  Can't save.
//
#define STG_E_CANTSAVE                   _HRESULT_TYPEDEF_(0x80030103L)

//
// MessageId: STG_E_OLDFORMAT
//
// MessageText:
//
//  The compound file %1 was produced with an incompatible version of storage.
//
#define STG_E_OLDFORMAT                  _HRESULT_TYPEDEF_(0x80030104L)

//
// MessageId: STG_E_OLDDLL
//
// MessageText:
//
//  The compound file %1 was produced with a newer version of storage.
//
#define STG_E_OLDDLL                     _HRESULT_TYPEDEF_(0x80030105L)

//
// MessageId: STG_E_SHAREREQUIRED
//
// MessageText:
//
//  Share.exe or equivalent is required for operation.
//
#define STG_E_SHAREREQUIRED              _HRESULT_TYPEDEF_(0x80030106L)

//
// MessageId: STG_E_NOTFILEBASEDSTORAGE
//
// MessageText:
//
//  Illegal operation called on non-file based storage.
//
#define STG_E_NOTFILEBASEDSTORAGE        _HRESULT_TYPEDEF_(0x80030107L)

//
// MessageId: STG_E_EXTANTMARSHALLINGS
//
// MessageText:
//
//  Illegal operation called on object with extant marshallings.
//
#define STG_E_EXTANTMARSHALLINGS         _HRESULT_TYPEDEF_(0x80030108L)

//
// MessageId: STG_E_DOCFILECORRUPT
//
// MessageText:
//
//  The docfile has been corrupted.
//
#define STG_E_DOCFILECORRUPT             _HRESULT_TYPEDEF_(0x80030109L)

//
// MessageId: STG_E_BADBASEADDRESS
//
// MessageText:
//
//  OLE32.DLL has been loaded at the wrong address.
//
#define STG_E_BADBASEADDRESS             _HRESULT_TYPEDEF_(0x80030110L)

//
// MessageId: STG_E_DOCFILETOOLARGE
//
// MessageText:
//
//  The compound file is too large for the current implementation
//
#define STG_E_DOCFILETOOLARGE            _HRESULT_TYPEDEF_(0x80030111L)

//
// MessageId: STG_E_NOTSIMPLEFORMAT
//
// MessageText:
//
//  The compound file was not created with the STGM_SIMPLE flag
//
#define STG_E_NOTSIMPLEFORMAT            _HRESULT_TYPEDEF_(0x80030112L)

//
// MessageId: STG_E_INCOMPLETE
//
// MessageText:
//
//  The file download was aborted abnormally.  The file is incomplete.
//
#define STG_E_INCOMPLETE                 _HRESULT_TYPEDEF_(0x80030201L)

//
// MessageId: STG_E_TERMINATED
//
// MessageText:
//
//  The file download has been terminated.
//
#define STG_E_TERMINATED                 _HRESULT_TYPEDEF_(0x80030202L)

//
// MessageId: STG_S_CONVERTED
//
// MessageText:
//
//  The underlying file was converted to compound file format.
//
#define STG_S_CONVERTED                  _HRESULT_TYPEDEF_(0x00030200L)

//
// MessageId: STG_S_BLOCK
//
// MessageText:
//
//  The storage operation should block until more data is available.
//
#define STG_S_BLOCK                      _HRESULT_TYPEDEF_(0x00030201L)

//
// MessageId: STG_S_RETRYNOW
//
// MessageText:
//
//  The storage operation should retry immediately.
//
#define STG_S_RETRYNOW                   _HRESULT_TYPEDEF_(0x00030202L)

//
// MessageId: STG_S_MONITORING
//
// MessageText:
//
//  The notified event sink will not influence the storage operation.
//
#define STG_S_MONITORING                 _HRESULT_TYPEDEF_(0x00030203L)

//
// MessageId: STG_S_MULTIPLEOPENS
//
// MessageText:
//
//  Multiple opens prevent consolidated. (commit succeeded).
//
#define STG_S_MULTIPLEOPENS              _HRESULT_TYPEDEF_(0x00030204L)

//
// MessageId: STG_S_CONSOLIDATIONFAILED
//
// MessageText:
//
//  Consolidation of the storage file failed. (commit succeeded).
//
#define STG_S_CONSOLIDATIONFAILED        _HRESULT_TYPEDEF_(0x00030205L)

//
// MessageId: STG_S_CANNOTCONSOLIDATE
//
// MessageText:
//
//  Consolidation of the storage file is inappropriate. (commit succeeded).
//
#define STG_S_CANNOTCONSOLIDATE          _HRESULT_TYPEDEF_(0x00030206L)

/*++

 MessageId's 0x0305 - 0x031f (inclusive) are reserved for **STORAGE**
 copy protection errors.

--*/
//
// MessageId: STG_E_STATUS_COPY_PROTECTION_FAILURE
//
// MessageText:
//
//  Generic Copy Protection Error.
//
#define STG_E_STATUS_COPY_PROTECTION_FAILURE _HRESULT_TYPEDEF_(0x80030305L)

//
// MessageId: STG_E_CSS_AUTHENTICATION_FAILURE
//
// MessageText:
//
//  Copy Protection Error - DVD CSS Authentication failed.
//
#define STG_E_CSS_AUTHENTICATION_FAILURE _HRESULT_TYPEDEF_(0x80030306L)

//
// MessageId: STG_E_CSS_KEY_NOT_PRESENT
//
// MessageText:
//
//  Copy Protection Error - The given sector does not have a valid CSS key.
//
#define STG_E_CSS_KEY_NOT_PRESENT        _HRESULT_TYPEDEF_(0x80030307L)

//
// MessageId: STG_E_CSS_KEY_NOT_ESTABLISHED
//
// MessageText:
//
//  Copy Protection Error - DVD session key not established.
//
#define STG_E_CSS_KEY_NOT_ESTABLISHED    _HRESULT_TYPEDEF_(0x80030308L)

//
// MessageId: STG_E_CSS_SCRAMBLED_SECTOR
//
// MessageText:
//
//  Copy Protection Error - The read failed because the sector is encrypted.
//
#define STG_E_CSS_SCRAMBLED_SECTOR       _HRESULT_TYPEDEF_(0x80030309L)

//
// MessageId: STG_E_CSS_REGION_MISMATCH
//
// MessageText:
//
//  Copy Protection Error - The current DVD's region does not correspond to the region setting of the drive.
//
#define STG_E_CSS_REGION_MISMATCH        _HRESULT_TYPEDEF_(0x8003030AL)

//
// MessageId: STG_E_RESETS_EXHAUSTED
//
// MessageText:
//
//  Copy Protection Error - The drive's region setting may be permanent or the number of user resets has been exhausted.
//
#define STG_E_RESETS_EXHAUSTED           _HRESULT_TYPEDEF_(0x8003030BL)

/*++

 MessageId's 0x0305 - 0x031f (inclusive) are reserved for **STORAGE**
 copy protection errors.

--*/
// ******************
// FACILITY_RPC
// ******************
//
// Codes 0x0-0x11 are propagated from 16 bit OLE.
//
//
// MessageId: RPC_E_CALL_REJECTED
//
// MessageText:
//
//  Call was rejected by callee.
//
#define RPC_E_CALL_REJECTED              _HRESULT_TYPEDEF_(0x80010001L)

//
// MessageId: RPC_E_CALL_CANCELED
//
// MessageText:
//
//  Call was canceled by the message filter.
//
#define RPC_E_CALL_CANCELED              _HRESULT_TYPEDEF_(0x80010002L)

//
// MessageId: RPC_E_CANTPOST_INSENDCALL
//
// MessageText:
//
//  The caller is dispatching an intertask SendMessage call and cannot call out via PostMessage.
//
#define RPC_E_CANTPOST_INSENDCALL        _HRESULT_TYPEDEF_(0x80010003L)

//
// MessageId: RPC_E_CANTCALLOUT_INASYNCCALL
//
// MessageText:
//
//  The caller is dispatching an asynchronous call and cannot make an outgoing call on behalf of this call.
//
#define RPC_E_CANTCALLOUT_INASYNCCALL    _HRESULT_TYPEDEF_(0x80010004L)

//
// MessageId: RPC_E_CANTCALLOUT_INEXTERNALCALL
//
// MessageText:
//
//  It is illegal to call out while inside message filter.
//
#define RPC_E_CANTCALLOUT_INEXTERNALCALL _HRESULT_TYPEDEF_(0x80010005L)

//
// MessageId: RPC_E_CONNECTION_TERMINATED
//
// MessageText:
//
//  The connection terminated or is in a bogus state and cannot be used any more. Other connections are still valid.
//
#define RPC_E_CONNECTION_TERMINATED      _HRESULT_TYPEDEF_(0x80010006L)

//
// MessageId: RPC_E_SERVER_DIED
//
// MessageText:
//
//  The callee (server [not server application]) is not available and disappeared; all connections are invalid. The call may have executed.
//
#define RPC_E_SERVER_DIED                _HRESULT_TYPEDEF_(0x80010007L)

//
// MessageId: RPC_E_CLIENT_DIED
//
// MessageText:
//
//  The caller (client) disappeared while the callee (server) was processing a call.
//
#define RPC_E_CLIENT_DIED                _HRESULT_TYPEDEF_(0x80010008L)

//
// MessageId: RPC_E_INVALID_DATAPACKET
//
// MessageText:
//
//  The data packet with the marshalled parameter data is incorrect.
//
#define RPC_E_INVALID_DATAPACKET         _HRESULT_TYPEDEF_(0x80010009L)

//
// MessageId: RPC_E_CANTTRANSMIT_CALL
//
// MessageText:
//
//  The call was not transmitted properly; the message queue was full and was not emptied after yielding.
//
#define RPC_E_CANTTRANSMIT_CALL          _HRESULT_TYPEDEF_(0x8001000AL)

//
// MessageId: RPC_E_CLIENT_CANTMARSHAL_DATA
//
// MessageText:
//
//  The client (caller) cannot marshall the parameter data - low memory, etc.
//
#define RPC_E_CLIENT_CANTMARSHAL_DATA    _HRESULT_TYPEDEF_(0x8001000BL)

//
// MessageId: RPC_E_CLIENT_CANTUNMARSHAL_DATA
//
// MessageText:
//
//  The client (caller) cannot unmarshall the return data - low memory, etc.
//
#define RPC_E_CLIENT_CANTUNMARSHAL_DATA  _HRESULT_TYPEDEF_(0x8001000CL)

//
// MessageId: RPC_E_SERVER_CANTMARSHAL_DATA
//
// MessageText:
//
//  The server (callee) cannot marshall the return data - low memory, etc.
//
#define RPC_E_SERVER_CANTMARSHAL_DATA    _HRESULT_TYPEDEF_(0x8001000DL)

//
// MessageId: RPC_E_SERVER_CANTUNMARSHAL_DATA
//
// MessageText:
//
//  The server (callee) cannot unmarshall the parameter data - low memory, etc.
//
#define RPC_E_SERVER_CANTUNMARSHAL_DATA  _HRESULT_TYPEDEF_(0x8001000EL)

//
// MessageId: RPC_E_INVALID_DATA
//
// MessageText:
//
//  Received data is invalid; could be server or client data.
//
#define RPC_E_INVALID_DATA               _HRESULT_TYPEDEF_(0x8001000FL)

//
// MessageId: RPC_E_INVALID_PARAMETER
//
// MessageText:
//
//  A particular parameter is invalid and cannot be (un)marshalled.
//
#define RPC_E_INVALID_PARAMETER          _HRESULT_TYPEDEF_(0x80010010L)

//
// MessageId: RPC_E_CANTCALLOUT_AGAIN
//
// MessageText:
//
//  There is no second outgoing call on same channel in DDE conversation.
//
#define RPC_E_CANTCALLOUT_AGAIN          _HRESULT_TYPEDEF_(0x80010011L)

//
// MessageId: RPC_E_SERVER_DIED_DNE
//
// MessageText:
//
//  The callee (server [not server application]) is not available and disappeared; all connections are invalid. The call did not execute.
//
#define RPC_E_SERVER_DIED_DNE            _HRESULT_TYPEDEF_(0x80010012L)

//
// MessageId: RPC_E_SYS_CALL_FAILED
//
// MessageText:
//
//  System call failed.
//
#define RPC_E_SYS_CALL_FAILED            _HRESULT_TYPEDEF_(0x80010100L)

//
// MessageId: RPC_E_OUT_OF_RESOURCES
//
// MessageText:
//
//  Could not allocate some required resource (memory, events, ...)
//
#define RPC_E_OUT_OF_RESOURCES           _HRESULT_TYPEDEF_(0x80010101L)

//
// MessageId: RPC_E_ATTEMPTED_MULTITHREAD
//
// MessageText:
//
//  Attempted to make calls on more than one thread in single threaded mode.
//
#define RPC_E_ATTEMPTED_MULTITHREAD      _HRESULT_TYPEDEF_(0x80010102L)

//
// MessageId: RPC_E_NOT_REGISTERED
//
// MessageText:
//
//  The requested interface is not registered on the server object.
//
#define RPC_E_NOT_REGISTERED             _HRESULT_TYPEDEF_(0x80010103L)

//
// MessageId: RPC_E_FAULT
//
// MessageText:
//
//  RPC could not call the server or could not return the results of calling the server.
//
#define RPC_E_FAULT                      _HRESULT_TYPEDEF_(0x80010104L)

//
// MessageId: RPC_E_SERVERFAULT
//
// MessageText:
//
//  The server threw an exception.
//
#define RPC_E_SERVERFAULT                _HRESULT_TYPEDEF_(0x80010105L)

//
// MessageId: RPC_E_CHANGED_MODE
//
// MessageText:
//
//  Cannot change thread mode after it is set.
//
#define RPC_E_CHANGED_MODE               _HRESULT_TYPEDEF_(0x80010106L)

//
// MessageId: RPC_E_INVALIDMETHOD
//
// MessageText:
//
//  The method called does not exist on the server.
//
#define RPC_E_INVALIDMETHOD              _HRESULT_TYPEDEF_(0x80010107L)

//
// MessageId: RPC_E_DISCONNECTED
//
// MessageText:
//
//  The object invoked has disconnected from its clients.
//
#define RPC_E_DISCONNECTED               _HRESULT_TYPEDEF_(0x80010108L)

//
// MessageId: RPC_E_RETRY
//
// MessageText:
//
//  The object invoked chose not to process the call now.  Try again later.
//
#define RPC_E_RETRY                      _HRESULT_TYPEDEF_(0x80010109L)

//
// MessageId: RPC_E_SERVERCALL_RETRYLATER
//
// MessageText:
//
//  The message filter indicated that the application is busy.
//
#define RPC_E_SERVERCALL_RETRYLATER      _HRESULT_TYPEDEF_(0x8001010AL)

//
// MessageId: RPC_E_SERVERCALL_REJECTED
//
// MessageText:
//
//  The message filter rejected the call.
//
#define RPC_E_SERVERCALL_REJECTED        _HRESULT_TYPEDEF_(0x8001010BL)

//
// MessageId: RPC_E_INVALID_CALLDATA
//
// MessageText:
//
//  A call control interfaces was called with invalid data.
//
#define RPC_E_INVALID_CALLDATA           _HRESULT_TYPEDEF_(0x8001010CL)

//
// MessageId: RPC_E_CANTCALLOUT_ININPUTSYNCCALL
//
// MessageText:
//
//  An outgoing call cannot be made since the application is dispatching an input-synchronous call.
//
#define RPC_E_CANTCALLOUT_ININPUTSYNCCALL _HRESULT_TYPEDEF_(0x8001010DL)

//
// MessageId: RPC_E_WRONG_THREAD
//
// MessageText:
//
//  The application called an interface that was marshalled for a different thread.
//
#define RPC_E_WRONG_THREAD               _HRESULT_TYPEDEF_(0x8001010EL)

//
// MessageId: RPC_E_THREAD_NOT_INIT
//
// MessageText:
//
//  CoInitialize has not been called on the current thread.
//
#define RPC_E_THREAD_NOT_INIT            _HRESULT_TYPEDEF_(0x8001010FL)

//
// MessageId: RPC_E_VERSION_MISMATCH
//
// MessageText:
//
//  The version of OLE on the client and server machines does not match.
//
#define RPC_E_VERSION_MISMATCH           _HRESULT_TYPEDEF_(0x80010110L)

//
// MessageId: RPC_E_INVALID_HEADER
//
// MessageText:
//
//  OLE received a packet with an invalid header.
//
#define RPC_E_INVALID_HEADER             _HRESULT_TYPEDEF_(0x80010111L)

//
// MessageId: RPC_E_INVALID_EXTENSION
//
// MessageText:
//
//  OLE received a packet with an invalid extension.
//
#define RPC_E_INVALID_EXTENSION          _HRESULT_TYPEDEF_(0x80010112L)

//
// MessageId: RPC_E_INVALID_IPID
//
// MessageText:
//
//  The requested object or interface does not exist.
//
#define RPC_E_INVALID_IPID               _HRESULT_TYPEDEF_(0x80010113L)

//
// MessageId: RPC_E_INVALID_OBJECT
//
// MessageText:
//
//  The requested object does not exist.
//
#define RPC_E_INVALID_OBJECT             _HRESULT_TYPEDEF_(0x80010114L)

//
// MessageId: RPC_S_CALLPENDING
//
// MessageText:
//
//  OLE has sent a request and is waiting for a reply.
//
#define RPC_S_CALLPENDING                _HRESULT_TYPEDEF_(0x80010115L)

//
// MessageId: RPC_S_WAITONTIMER
//
// MessageText:
//
//  OLE is waiting before retrying a request.
//
#define RPC_S_WAITONTIMER                _HRESULT_TYPEDEF_(0x80010116L)

//
// MessageId: RPC_E_CALL_COMPLETE
//
// MessageText:
//
//  Call context cannot be accessed after call completed.
//
#define RPC_E_CALL_COMPLETE              _HRESULT_TYPEDEF_(0x80010117L)

//
// MessageId: RPC_E_UNSECURE_CALL
//
// MessageText:
//
//  Impersonate on unsecure calls is not supported.
//
#define RPC_E_UNSECURE_CALL              _HRESULT_TYPEDEF_(0x80010118L)

//
// MessageId: RPC_E_TOO_LATE
//
// MessageText:
//
//  Security must be initialized before any interfaces are marshalled or unmarshalled. It cannot be changed once initialized.
//
#define RPC_E_TOO_LATE                   _HRESULT_TYPEDEF_(0x80010119L)

//
// MessageId: RPC_E_NO_GOOD_SECURITY_PACKAGES
//
// MessageText:
//
//  No security packages are installed on this machine or the user is not logged on or there are no compatible security packages between the client and server.
//
#define RPC_E_NO_GOOD_SECURITY_PACKAGES  _HRESULT_TYPEDEF_(0x8001011AL)

//
// MessageId: RPC_E_ACCESS_DENIED
//
// MessageText:
//
//  Access is denied.
//
#define RPC_E_ACCESS_DENIED              _HRESULT_TYPEDEF_(0x8001011BL)

//
// MessageId: RPC_E_REMOTE_DISABLED
//
// MessageText:
//
//  Remote calls are not allowed for this process.
//
#define RPC_E_REMOTE_DISABLED            _HRESULT_TYPEDEF_(0x8001011CL)

//
// MessageId: RPC_E_INVALID_OBJREF
//
// MessageText:
//
//  The marshaled interface data packet (OBJREF) has an invalid or unknown format.
//
#define RPC_E_INVALID_OBJREF             _HRESULT_TYPEDEF_(0x8001011DL)

//
// MessageId: RPC_E_NO_CONTEXT
//
// MessageText:
//
//  No context is associated with this call. This happens for some custom marshalled calls and on the client side of the call.
//
#define RPC_E_NO_CONTEXT                 _HRESULT_TYPEDEF_(0x8001011EL)

//
// MessageId: RPC_E_TIMEOUT
//
// MessageText:
//
//  This operation returned because the timeout period expired.
//
#define RPC_E_TIMEOUT                    _HRESULT_TYPEDEF_(0x8001011FL)

//
// MessageId: RPC_E_NO_SYNC
//
// MessageText:
//
//  There are no synchronize objects to wait on.
//
#define RPC_E_NO_SYNC                    _HRESULT_TYPEDEF_(0x80010120L)

//
// MessageId: RPC_E_FULLSIC_REQUIRED
//
// MessageText:
//
//  Full subject issuer chain SSL principal name expected from the server.
//
#define RPC_E_FULLSIC_REQUIRED           _HRESULT_TYPEDEF_(0x80010121L)

//
// MessageId: RPC_E_INVALID_STD_NAME
//
// MessageText:
//
//  Principal name is not a valid MSSTD name.
//
#define RPC_E_INVALID_STD_NAME           _HRESULT_TYPEDEF_(0x80010122L)

//
// MessageId: CO_E_FAILEDTOIMPERSONATE
//
// MessageText:
//
//  Unable to impersonate DCOM client
//
#define CO_E_FAILEDTOIMPERSONATE         _HRESULT_TYPEDEF_(0x80010123L)

//
// MessageId: CO_E_FAILEDTOGETSECCTX
//
// MessageText:
//
//  Unable to obtain server's security context
//
#define CO_E_FAILEDTOGETSECCTX           _HRESULT_TYPEDEF_(0x80010124L)

//
// MessageId: CO_E_FAILEDTOOPENTHREADTOKEN
//
// MessageText:
//
//  Unable to open the access token of the current thread
//
#define CO_E_FAILEDTOOPENTHREADTOKEN     _HRESULT_TYPEDEF_(0x80010125L)

//
// MessageId: CO_E_FAILEDTOGETTOKENINFO
//
// MessageText:
//
//  Unable to obtain user info from an access token
//
#define CO_E_FAILEDTOGETTOKENINFO        _HRESULT_TYPEDEF_(0x80010126L)

//
// MessageId: CO_E_TRUSTEEDOESNTMATCHCLIENT
//
// MessageText:
//
//  The client who called IAccessControl::IsAccessPermitted was not the trustee provided to the method
//
#define CO_E_TRUSTEEDOESNTMATCHCLIENT    _HRESULT_TYPEDEF_(0x80010127L)

//
// MessageId: CO_E_FAILEDTOQUERYCLIENTBLANKET
//
// MessageText:
//
//  Unable to obtain the client's security blanket
//
#define CO_E_FAILEDTOQUERYCLIENTBLANKET  _HRESULT_TYPEDEF_(0x80010128L)

//
// MessageId: CO_E_FAILEDTOSETDACL
//
// MessageText:
//
//  Unable to set a discretionary ACL into a security descriptor
//
#define CO_E_FAILEDTOSETDACL             _HRESULT_TYPEDEF_(0x80010129L)

//
// MessageId: CO_E_ACCESSCHECKFAILED
//
// MessageText:
//
//  The system function, AccessCheck, returned false
//
#define CO_E_ACCESSCHECKFAILED           _HRESULT_TYPEDEF_(0x8001012AL)

//
// MessageId: CO_E_NETACCESSAPIFAILED
//
// MessageText:
//
//  Either NetAccessDel or NetAccessAdd returned an error code.
//
#define CO_E_NETACCESSAPIFAILED          _HRESULT_TYPEDEF_(0x8001012BL)

//
// MessageId: CO_E_WRONGTRUSTEENAMESYNTAX
//
// MessageText:
//
//  One of the trustee strings provided by the user did not conform to the <Domain>\<Name> syntax and it was not the "*" string
//
#define CO_E_WRONGTRUSTEENAMESYNTAX      _HRESULT_TYPEDEF_(0x8001012CL)

//
// MessageId: CO_E_INVALIDSID
//
// MessageText:
//
//  One of the security identifiers provided by the user was invalid
//
#define CO_E_INVALIDSID                  _HRESULT_TYPEDEF_(0x8001012DL)

//
// MessageId: CO_E_CONVERSIONFAILED
//
// MessageText:
//
//  Unable to convert a wide character trustee string to a multibyte trustee string
//
#define CO_E_CONVERSIONFAILED            _HRESULT_TYPEDEF_(0x8001012EL)

//
// MessageId: CO_E_NOMATCHINGSIDFOUND
//
// MessageText:
//
//  Unable to find a security identifier that corresponds to a trustee string provided by the user
//
#define CO_E_NOMATCHINGSIDFOUND          _HRESULT_TYPEDEF_(0x8001012FL)

//
// MessageId: CO_E_LOOKUPACCSIDFAILED
//
// MessageText:
//
//  The system function, LookupAccountSID, failed
//
#define CO_E_LOOKUPACCSIDFAILED          _HRESULT_TYPEDEF_(0x80010130L)

//
// MessageId: CO_E_NOMATCHINGNAMEFOUND
//
// MessageText:
//
//  Unable to find a trustee name that corresponds to a security identifier provided by the user
//
#define CO_E_NOMATCHINGNAMEFOUND         _HRESULT_TYPEDEF_(0x80010131L)

//
// MessageId: CO_E_LOOKUPACCNAMEFAILED
//
// MessageText:
//
//  The system function, LookupAccountName, failed
//
#define CO_E_LOOKUPACCNAMEFAILED         _HRESULT_TYPEDEF_(0x80010132L)

//
// MessageId: CO_E_SETSERLHNDLFAILED
//
// MessageText:
//
//  Unable to set or reset a serialization handle
//
#define CO_E_SETSERLHNDLFAILED           _HRESULT_TYPEDEF_(0x80010133L)

//
// MessageId: CO_E_FAILEDTOGETWINDIR
//
// MessageText:
//
//  Unable to obtain the Windows directory
//
#define CO_E_FAILEDTOGETWINDIR           _HRESULT_TYPEDEF_(0x80010134L)

//
// MessageId: CO_E_PATHTOOLONG
//
// MessageText:
//
//  Path too long
//
#define CO_E_PATHTOOLONG                 _HRESULT_TYPEDEF_(0x80010135L)

//
// MessageId: CO_E_FAILEDTOGENUUID
//
// MessageText:
//
//  Unable to generate a uuid.
//
#define CO_E_FAILEDTOGENUUID             _HRESULT_TYPEDEF_(0x80010136L)

//
// MessageId: CO_E_FAILEDTOCREATEFILE
//
// MessageText:
//
//  Unable to create file
//
#define CO_E_FAILEDTOCREATEFILE          _HRESULT_TYPEDEF_(0x80010137L)

//
// MessageId: CO_E_FAILEDTOCLOSEHANDLE
//
// MessageText:
//
//  Unable to close a serialization handle or a file handle.
//
#define CO_E_FAILEDTOCLOSEHANDLE         _HRESULT_TYPEDEF_(0x80010138L)

//
// MessageId: CO_E_EXCEEDSYSACLLIMIT
//
// MessageText:
//
//  The number of ACEs in an ACL exceeds the system limit.
//
#define CO_E_EXCEEDSYSACLLIMIT           _HRESULT_TYPEDEF_(0x80010139L)

//
// MessageId: CO_E_ACESINWRONGORDER
//
// MessageText:
//
//  Not all the DENY_ACCESS ACEs are arranged in front of the GRANT_ACCESS ACEs in the stream.
//
#define CO_E_ACESINWRONGORDER            _HRESULT_TYPEDEF_(0x8001013AL)

//
// MessageId: CO_E_INCOMPATIBLESTREAMVERSION
//
// MessageText:
//
//  The version of ACL format in the stream is not supported by this implementation of IAccessControl
//
#define CO_E_INCOMPATIBLESTREAMVERSION   _HRESULT_TYPEDEF_(0x8001013BL)

//
// MessageId: CO_E_FAILEDTOOPENPROCESSTOKEN
//
// MessageText:
//
//  Unable to open the access token of the server process
//
#define CO_E_FAILEDTOOPENPROCESSTOKEN    _HRESULT_TYPEDEF_(0x8001013CL)

//
// MessageId: CO_E_DECODEFAILED
//
// MessageText:
//
//  Unable to decode the ACL in the stream provided by the user
//
#define CO_E_DECODEFAILED                _HRESULT_TYPEDEF_(0x8001013DL)

//
// MessageId: CO_E_ACNOTINITIALIZED
//
// MessageText:
//
//  The COM IAccessControl object is not initialized
//
#define CO_E_ACNOTINITIALIZED            _HRESULT_TYPEDEF_(0x8001013FL)

//
// MessageId: CO_E_CANCEL_DISABLED
//
// MessageText:
//
//  Call Cancellation is disabled
//
#define CO_E_CANCEL_DISABLED             _HRESULT_TYPEDEF_(0x80010140L)

//
// MessageId: RPC_E_UNEXPECTED
//
// MessageText:
//
//  An internal error occurred.
//
#define RPC_E_UNEXPECTED                 _HRESULT_TYPEDEF_(0x8001FFFFL)



//////////////////////////////////////
//                                  //
// Additional Security Status Codes //
//                                  //
// Facility=Security                //
//                                  //
//////////////////////////////////////


//
// MessageId: ERROR_AUDITING_DISABLED
//
// MessageText:
//
//  The specified event is currently not being audited.
//
#define ERROR_AUDITING_DISABLED          _HRESULT_TYPEDEF_(0xC0090001L)

//
// MessageId: ERROR_ALL_SIDS_FILTERED
//
// MessageText:
//
//  The SID filtering operation removed all SIDs.
//
#define ERROR_ALL_SIDS_FILTERED          _HRESULT_TYPEDEF_(0xC0090002L)



/////////////////////////////////////////////
//                                         //
// end of Additional Security Status Codes //
//                                         //
/////////////////////////////////////////////



 /////////////////
 //
 //  FACILITY_SSPI
 //
 /////////////////

//
// MessageId: NTE_BAD_UID
//
// MessageText:
//
//  Bad UID.
//
#define NTE_BAD_UID                      _HRESULT_TYPEDEF_(0x80090001L)

//
// MessageId: NTE_BAD_HASH
//
// MessageText:
//
//  Bad Hash.
//
#define NTE_BAD_HASH                     _HRESULT_TYPEDEF_(0x80090002L)

//
// MessageId: NTE_BAD_KEY
//
// MessageText:
//
//  Bad Key.
//
#define NTE_BAD_KEY                      _HRESULT_TYPEDEF_(0x80090003L)

//
// MessageId: NTE_BAD_LEN
//
// MessageText:
//
//  Bad Length.
//
#define NTE_BAD_LEN                      _HRESULT_TYPEDEF_(0x80090004L)

//
// MessageId: NTE_BAD_DATA
//
// MessageText:
//
//  Bad Data.
//
#define NTE_BAD_DATA                     _HRESULT_TYPEDEF_(0x80090005L)

//
// MessageId: NTE_BAD_SIGNATURE
//
// MessageText:
//
//  Invalid Signature.
//
#define NTE_BAD_SIGNATURE                _HRESULT_TYPEDEF_(0x80090006L)

//
// MessageId: NTE_BAD_VER
//
// MessageText:
//
//  Bad Version of provider.
//
#define NTE_BAD_VER                      _HRESULT_TYPEDEF_(0x80090007L)

//
// MessageId: NTE_BAD_ALGID
//
// MessageText:
//
//  Invalid algorithm specified.
//
#define NTE_BAD_ALGID                    _HRESULT_TYPEDEF_(0x80090008L)

//
// MessageId: NTE_BAD_FLAGS
//
// MessageText:
//
//  Invalid flags specified.
//
#define NTE_BAD_FLAGS                    _HRESULT_TYPEDEF_(0x80090009L)

//
// MessageId: NTE_BAD_TYPE
//
// MessageText:
//
//  Invalid type specified.
//
#define NTE_BAD_TYPE                     _HRESULT_TYPEDEF_(0x8009000AL)

//
// MessageId: NTE_BAD_KEY_STATE
//
// MessageText:
//
//  Key not valid for use in specified state.
//
#define NTE_BAD_KEY_STATE                _HRESULT_TYPEDEF_(0x8009000BL)

//
// MessageId: NTE_BAD_HASH_STATE
//
// MessageText:
//
//  Hash not valid for use in specified state.
//
#define NTE_BAD_HASH_STATE               _HRESULT_TYPEDEF_(0x8009000CL)

//
// MessageId: NTE_NO_KEY
//
// MessageText:
//
//  Key does not exist.
//
#define NTE_NO_KEY                       _HRESULT_TYPEDEF_(0x8009000DL)

//
// MessageId: NTE_NO_MEMORY
//
// MessageText:
//
//  Insufficient memory available for the operation.
//
#define NTE_NO_MEMORY                    _HRESULT_TYPEDEF_(0x8009000EL)

//
// MessageId: NTE_EXISTS
//
// MessageText:
//
//  Object already exists.
//
#define NTE_EXISTS                       _HRESULT_TYPEDEF_(0x8009000FL)

//
// MessageId: NTE_PERM
//
// MessageText:
//
//  Access denied.
//
#define NTE_PERM                         _HRESULT_TYPEDEF_(0x80090010L)

//
// MessageId: NTE_NOT_FOUND
//
// MessageText:
//
//  Object was not found.
//
#define NTE_NOT_FOUND                    _HRESULT_TYPEDEF_(0x80090011L)

//
// MessageId: NTE_DOUBLE_ENCRYPT
//
// MessageText:
//
//  Data already encrypted.
//
#define NTE_DOUBLE_ENCRYPT               _HRESULT_TYPEDEF_(0x80090012L)

//
// MessageId: NTE_BAD_PROVIDER
//
// MessageText:
//
//  Invalid provider specified.
//
#define NTE_BAD_PROVIDER                 _HRESULT_TYPEDEF_(0x80090013L)

//
// MessageId: NTE_BAD_PROV_TYPE
//
// MessageText:
//
//  Invalid provider type specified.
//
#define NTE_BAD_PROV_TYPE                _HRESULT_TYPEDEF_(0x80090014L)

//
// MessageId: NTE_BAD_PUBLIC_KEY
//
// MessageText:
//
//  Provider's public key is invalid.
//
#define NTE_BAD_PUBLIC_KEY               _HRESULT_TYPEDEF_(0x80090015L)

//
// MessageId: NTE_BAD_KEYSET
//
// MessageText:
//
//  Keyset does not exist
//
#define NTE_BAD_KEYSET                   _HRESULT_TYPEDEF_(0x80090016L)

//
// MessageId: NTE_PROV_TYPE_NOT_DEF
//
// MessageText:
//
//  Provider type not defined.
//
#define NTE_PROV_TYPE_NOT_DEF            _HRESULT_TYPEDEF_(0x80090017L)

//
// MessageId: NTE_PROV_TYPE_ENTRY_BAD
//
// MessageText:
//
//  Provider type as registered is invalid.
//
#define NTE_PROV_TYPE_ENTRY_BAD          _HRESULT_TYPEDEF_(0x80090018L)

//
// MessageId: NTE_KEYSET_NOT_DEF
//
// MessageText:
//
//  The keyset is not defined.
//
#define NTE_KEYSET_NOT_DEF               _HRESULT_TYPEDEF_(0x80090019L)

//
// MessageId: NTE_KEYSET_ENTRY_BAD
//
// MessageText:
//
//  Keyset as registered is invalid.
//
#define NTE_KEYSET_ENTRY_BAD             _HRESULT_TYPEDEF_(0x8009001AL)

//
// MessageId: NTE_PROV_TYPE_NO_MATCH
//
// MessageText:
//
//  Provider type does not match registered value.
//
#define NTE_PROV_TYPE_NO_MATCH           _HRESULT_TYPEDEF_(0x8009001BL)

//
// MessageId: NTE_SIGNATURE_FILE_BAD
//
// MessageText:
//
//  The digital signature file is corrupt.
//
#define NTE_SIGNATURE_FILE_BAD           _HRESULT_TYPEDEF_(0x8009001CL)

//
// MessageId: NTE_PROVIDER_DLL_FAIL
//
// MessageText:
//
//  Provider DLL failed to initialize correctly.
//
#define NTE_PROVIDER_DLL_FAIL            _HRESULT_TYPEDEF_(0x8009001DL)

//
// MessageId: NTE_PROV_DLL_NOT_FOUND
//
// MessageText:
//
//  Provider DLL could not be found.
//
#define NTE_PROV_DLL_NOT_FOUND           _HRESULT_TYPEDEF_(0x8009001EL)

//
// MessageId: NTE_BAD_KEYSET_PARAM
//
// MessageText:
//
//  The Keyset parameter is invalid.
//
#define NTE_BAD_KEYSET_PARAM             _HRESULT_TYPEDEF_(0x8009001FL)

//
// MessageId: NTE_FAIL
//
// MessageText:
//
//  An internal error occurred.
//
#define NTE_FAIL                         _HRESULT_TYPEDEF_(0x80090020L)

//
// MessageId: NTE_SYS_ERR
//
// MessageText:
//
//  A base error occurred.
//
#define NTE_SYS_ERR                      _HRESULT_TYPEDEF_(0x80090021L)

//
// MessageId: NTE_SILENT_CONTEXT
//
// MessageText:
//
//  Provider could not perform the action since the context was acquired as silent.
//
#define NTE_SILENT_CONTEXT               _HRESULT_TYPEDEF_(0x80090022L)

//
// MessageId: NTE_TOKEN_KEYSET_STORAGE_FULL
//
// MessageText:
//
//  The security token does not have storage space available for an additional container.
//
#define NTE_TOKEN_KEYSET_STORAGE_FULL    _HRESULT_TYPEDEF_(0x80090023L)

//
// MessageId: NTE_TEMPORARY_PROFILE
//
// MessageText:
//
//  The profile for the user is a temporary profile.
//
#define NTE_TEMPORARY_PROFILE            _HRESULT_TYPEDEF_(0x80090024L)

//
// MessageId: NTE_FIXEDPARAMETER
//
// MessageText:
//
//  The key parameters could not be set because the CSP uses fixed parameters.
//
#define NTE_FIXEDPARAMETER               _HRESULT_TYPEDEF_(0x80090025L)

//
// MessageId: SEC_E_INSUFFICIENT_MEMORY
//
// MessageText:
//
//  Not enough memory is available to complete this request
//
#define SEC_E_INSUFFICIENT_MEMORY        _HRESULT_TYPEDEF_(0x80090300L)

//
// MessageId: SEC_E_INVALID_HANDLE
//
// MessageText:
//
//  The handle specified is invalid
//
#define SEC_E_INVALID_HANDLE             _HRESULT_TYPEDEF_(0x80090301L)

//
// MessageId: SEC_E_UNSUPPORTED_FUNCTION
//
// MessageText:
//
//  The function requested is not supported
//
#define SEC_E_UNSUPPORTED_FUNCTION       _HRESULT_TYPEDEF_(0x80090302L)

//
// MessageId: SEC_E_TARGET_UNKNOWN
//
// MessageText:
//
//  The specified target is unknown or unreachable
//
#define SEC_E_TARGET_UNKNOWN             _HRESULT_TYPEDEF_(0x80090303L)

//
// MessageId: SEC_E_INTERNAL_ERROR
//
// MessageText:
//
//  The Local Security Authority cannot be contacted
//
#define SEC_E_INTERNAL_ERROR             _HRESULT_TYPEDEF_(0x80090304L)

//
// MessageId: SEC_E_SECPKG_NOT_FOUND
//
// MessageText:
//
//  The requested security package does not exist
//
#define SEC_E_SECPKG_NOT_FOUND           _HRESULT_TYPEDEF_(0x80090305L)

//
// MessageId: SEC_E_NOT_OWNER
//
// MessageText:
//
//  The caller is not the owner of the desired credentials
//
#define SEC_E_NOT_OWNER                  _HRESULT_TYPEDEF_(0x80090306L)

//
// MessageId: SEC_E_CANNOT_INSTALL
//
// MessageText:
//
//  The security package failed to initialize, and cannot be installed
//
#define SEC_E_CANNOT_INSTALL             _HRESULT_TYPEDEF_(0x80090307L)

//
// MessageId: SEC_E_INVALID_TOKEN
//
// MessageText:
//
//  The token supplied to the function is invalid
//
#define SEC_E_INVALID_TOKEN              _HRESULT_TYPEDEF_(0x80090308L)

//
// MessageId: SEC_E_CANNOT_PACK
//
// MessageText:
//
//  The security package is not able to marshall the logon buffer, so the logon attempt has failed
//
#define SEC_E_CANNOT_PACK                _HRESULT_TYPEDEF_(0x80090309L)

//
// MessageId: SEC_E_QOP_NOT_SUPPORTED
//
// MessageText:
//
//  The per-message Quality of Protection is not supported by the security package
//
#define SEC_E_QOP_NOT_SUPPORTED          _HRESULT_TYPEDEF_(0x8009030AL)

//
// MessageId: SEC_E_NO_IMPERSONATION
//
// MessageText:
//
//  The security context does not allow impersonation of the client
//
#define SEC_E_NO_IMPERSONATION           _HRESULT_TYPEDEF_(0x8009030BL)

//
// MessageId: SEC_E_LOGON_DENIED
//
// MessageText:
//
//  The logon attempt failed
//
#define SEC_E_LOGON_DENIED               _HRESULT_TYPEDEF_(0x8009030CL)

//
// MessageId: SEC_E_UNKNOWN_CREDENTIALS
//
// MessageText:
//
//  The credentials supplied to the package were not recognized
//
#define SEC_E_UNKNOWN_CREDENTIALS        _HRESULT_TYPEDEF_(0x8009030DL)

//
// MessageId: SEC_E_NO_CREDENTIALS
//
// MessageText:
//
//  No credentials are available in the security package
//
#define SEC_E_NO_CREDENTIALS             _HRESULT_TYPEDEF_(0x8009030EL)

//
// MessageId: SEC_E_MESSAGE_ALTERED
//
// MessageText:
//
//  The message or signature supplied for verification has been altered
//
#define SEC_E_MESSAGE_ALTERED            _HRESULT_TYPEDEF_(0x8009030FL)

//
// MessageId: SEC_E_OUT_OF_SEQUENCE
//
// MessageText:
//
//  The message supplied for verification is out of sequence
//
#define SEC_E_OUT_OF_SEQUENCE            _HRESULT_TYPEDEF_(0x80090310L)

//
// MessageId: SEC_E_NO_AUTHENTICATING_AUTHORITY
//
// MessageText:
//
//  No authority could be contacted for authentication.
//
#define SEC_E_NO_AUTHENTICATING_AUTHORITY _HRESULT_TYPEDEF_(0x80090311L)

//
// MessageId: SEC_I_CONTINUE_NEEDED
//
// MessageText:
//
//  The function completed successfully, but must be called again to complete the context
//
#define SEC_I_CONTINUE_NEEDED            _HRESULT_TYPEDEF_(0x00090312L)

//
// MessageId: SEC_I_COMPLETE_NEEDED
//
// MessageText:
//
//  The function completed successfully, but CompleteToken must be called
//
#define SEC_I_COMPLETE_NEEDED            _HRESULT_TYPEDEF_(0x00090313L)

//
// MessageId: SEC_I_COMPLETE_AND_CONTINUE
//
// MessageText:
//
//  The function completed successfully, but both CompleteToken and this function must be called to complete the context
//
#define SEC_I_COMPLETE_AND_CONTINUE      _HRESULT_TYPEDEF_(0x00090314L)

//
// MessageId: SEC_I_LOCAL_LOGON
//
// MessageText:
//
//  The logon was completed, but no network authority was available. The logon was made using locally known information
//
#define SEC_I_LOCAL_LOGON                _HRESULT_TYPEDEF_(0x00090315L)

//
// MessageId: SEC_E_BAD_PKGID
//
// MessageText:
//
//  The requested security package does not exist
//
#define SEC_E_BAD_PKGID                  _HRESULT_TYPEDEF_(0x80090316L)

//
// MessageId: SEC_E_CONTEXT_EXPIRED
//
// MessageText:
//
//  The context has expired and can no longer be used.
//
#define SEC_E_CONTEXT_EXPIRED            _HRESULT_TYPEDEF_(0x80090317L)

//
// MessageId: SEC_I_CONTEXT_EXPIRED
//
// MessageText:
//
//  The context has expired and can no longer be used.
//
#define SEC_I_CONTEXT_EXPIRED            _HRESULT_TYPEDEF_(0x00090317L)

//
// MessageId: SEC_E_INCOMPLETE_MESSAGE
//
// MessageText:
//
//  The supplied message is incomplete.  The signature was not verified.
//
#define SEC_E_INCOMPLETE_MESSAGE         _HRESULT_TYPEDEF_(0x80090318L)

//
// MessageId: SEC_E_INCOMPLETE_CREDENTIALS
//
// MessageText:
//
//  The credentials supplied were not complete, and could not be verified. The context could not be initialized.
//
#define SEC_E_INCOMPLETE_CREDENTIALS     _HRESULT_TYPEDEF_(0x80090320L)

//
// MessageId: SEC_E_BUFFER_TOO_SMALL
//
// MessageText:
//
//  The buffers supplied to a function was too small.
//
#define SEC_E_BUFFER_TOO_SMALL           _HRESULT_TYPEDEF_(0x80090321L)

//
// MessageId: SEC_I_INCOMPLETE_CREDENTIALS
//
// MessageText:
//
//  The credentials supplied were not complete, and could not be verified. Additional information can be returned from the context.
//
#define SEC_I_INCOMPLETE_CREDENTIALS     _HRESULT_TYPEDEF_(0x00090320L)

//
// MessageId: SEC_I_RENEGOTIATE
//
// MessageText:
//
//  The context data must be renegotiated with the peer.
//
#define SEC_I_RENEGOTIATE                _HRESULT_TYPEDEF_(0x00090321L)

//
// MessageId: SEC_E_WRONG_PRINCIPAL
//
// MessageText:
//
//  The target principal name is incorrect.
//
#define SEC_E_WRONG_PRINCIPAL            _HRESULT_TYPEDEF_(0x80090322L)

//
// MessageId: SEC_I_NO_LSA_CONTEXT
//
// MessageText:
//
//  There is no LSA mode context associated with this context.
//
#define SEC_I_NO_LSA_CONTEXT             _HRESULT_TYPEDEF_(0x00090323L)

//
// MessageId: SEC_E_TIME_SKEW
//
// MessageText:
//
//  The clocks on the client and server machines are skewed.
//
#define SEC_E_TIME_SKEW                  _HRESULT_TYPEDEF_(0x80090324L)

//
// MessageId: SEC_E_UNTRUSTED_ROOT
//
// MessageText:
//
//  The certificate chain was issued by an authority that is not trusted.
//
#define SEC_E_UNTRUSTED_ROOT             _HRESULT_TYPEDEF_(0x80090325L)

//
// MessageId: SEC_E_ILLEGAL_MESSAGE
//
// MessageText:
//
//  The message received was unexpected or badly formatted.
//
#define SEC_E_ILLEGAL_MESSAGE            _HRESULT_TYPEDEF_(0x80090326L)

//
// MessageId: SEC_E_CERT_UNKNOWN
//
// MessageText:
//
//  An unknown error occurred while processing the certificate.
//
#define SEC_E_CERT_UNKNOWN               _HRESULT_TYPEDEF_(0x80090327L)

//
// MessageId: SEC_E_CERT_EXPIRED
//
// MessageText:
//
//  The received certificate has expired.
//
#define SEC_E_CERT_EXPIRED               _HRESULT_TYPEDEF_(0x80090328L)

//
// MessageId: SEC_E_ENCRYPT_FAILURE
//
// MessageText:
//
//  The specified data could not be encrypted.
//
#define SEC_E_ENCRYPT_FAILURE            _HRESULT_TYPEDEF_(0x80090329L)

//
// MessageId: SEC_E_DECRYPT_FAILURE
//
// MessageText:
//
//  The specified data could not be decrypted.
//  
//
#define SEC_E_DECRYPT_FAILURE            _HRESULT_TYPEDEF_(0x80090330L)

//
// MessageId: SEC_E_ALGORITHM_MISMATCH
//
// MessageText:
//
//  The client and server cannot communicate, because they do not possess a common algorithm.
//
#define SEC_E_ALGORITHM_MISMATCH         _HRESULT_TYPEDEF_(0x80090331L)

//
// MessageId: SEC_E_SECURITY_QOS_FAILED
//
// MessageText:
//
//  The security context could not be established due to a failure in the requested quality of service (e.g. mutual authentication or delegation).
//
#define SEC_E_SECURITY_QOS_FAILED        _HRESULT_TYPEDEF_(0x80090332L)

//
// MessageId: SEC_E_UNFINISHED_CONTEXT_DELETED
//
// MessageText:
//
//  A security context was deleted before the context was completed.  This is considered a logon failure.
//
#define SEC_E_UNFINISHED_CONTEXT_DELETED _HRESULT_TYPEDEF_(0x80090333L)

//
// MessageId: SEC_E_NO_TGT_REPLY
//
// MessageText:
//
//  The client is trying to negotiate a context and the server requires user-to-user but didn't send a TGT reply.
//
#define SEC_E_NO_TGT_REPLY               _HRESULT_TYPEDEF_(0x80090334L)

//
// MessageId: SEC_E_NO_IP_ADDRESSES
//
// MessageText:
//
//  Unable to accomplish the requested task because the local machine does not have any IP addresses.
//
#define SEC_E_NO_IP_ADDRESSES            _HRESULT_TYPEDEF_(0x80090335L)

//
// MessageId: SEC_E_WRONG_CREDENTIAL_HANDLE
//
// MessageText:
//
//  The supplied credential handle does not match the credential associated with the security context.
//
#define SEC_E_WRONG_CREDENTIAL_HANDLE    _HRESULT_TYPEDEF_(0x80090336L)

//
// MessageId: SEC_E_CRYPTO_SYSTEM_INVALID
//
// MessageText:
//
//  The crypto system or checksum function is invalid because a required function is unavailable.
//
#define SEC_E_CRYPTO_SYSTEM_INVALID      _HRESULT_TYPEDEF_(0x80090337L)

//
// MessageId: SEC_E_MAX_REFERRALS_EXCEEDED
//
// MessageText:
//
//  The number of maximum ticket referrals has been exceeded.
//
#define SEC_E_MAX_REFERRALS_EXCEEDED     _HRESULT_TYPEDEF_(0x80090338L)

//
// MessageId: SEC_E_MUST_BE_KDC
//
// MessageText:
//
//  The local machine must be a Kerberos KDC (domain controller) and it is not.
//
#define SEC_E_MUST_BE_KDC                _HRESULT_TYPEDEF_(0x80090339L)

//
// MessageId: SEC_E_STRONG_CRYPTO_NOT_SUPPORTED
//
// MessageText:
//
//  The other end of the security negotiation is requires strong crypto but it is not supported on the local machine.
//
#define SEC_E_STRONG_CRYPTO_NOT_SUPPORTED _HRESULT_TYPEDEF_(0x8009033AL)

//
// MessageId: SEC_E_TOO_MANY_PRINCIPALS
//
// MessageText:
//
//  The KDC reply contained more than one principal name.
//
#define SEC_E_TOO_MANY_PRINCIPALS        _HRESULT_TYPEDEF_(0x8009033BL)

//
// MessageId: SEC_E_NO_PA_DATA
//
// MessageText:
//
//  Expected to find PA data for a hint of what etype to use, but it was not found.
//
#define SEC_E_NO_PA_DATA                 _HRESULT_TYPEDEF_(0x8009033CL)

//
// MessageId: SEC_E_PKINIT_NAME_MISMATCH
//
// MessageText:
//
//  The client certificate does not contain a valid UPN, or does not match the client name
//  in the logon request.  Please contact your administrator.
//
#define SEC_E_PKINIT_NAME_MISMATCH       _HRESULT_TYPEDEF_(0x8009033DL)

//
// MessageId: SEC_E_SMARTCARD_LOGON_REQUIRED
//
// MessageText:
//
//  Smartcard logon is required and was not used.
//
#define SEC_E_SMARTCARD_LOGON_REQUIRED   _HRESULT_TYPEDEF_(0x8009033EL)

//
// MessageId: SEC_E_SHUTDOWN_IN_PROGRESS
//
// MessageText:
//
//  A system shutdown is in progress.
//
#define SEC_E_SHUTDOWN_IN_PROGRESS       _HRESULT_TYPEDEF_(0x8009033FL)

//
// MessageId: SEC_E_KDC_INVALID_REQUEST
//
// MessageText:
//
//  An invalid request was sent to the KDC.
//
#define SEC_E_KDC_INVALID_REQUEST        _HRESULT_TYPEDEF_(0x80090340L)

//
// MessageId: SEC_E_KDC_UNABLE_TO_REFER
//
// MessageText:
//
//  The KDC was unable to generate a referral for the service requested.
//
#define SEC_E_KDC_UNABLE_TO_REFER        _HRESULT_TYPEDEF_(0x80090341L)

//
// MessageId: SEC_E_KDC_UNKNOWN_ETYPE
//
// MessageText:
//
//  The encryption type requested is not supported by the KDC.
//
#define SEC_E_KDC_UNKNOWN_ETYPE          _HRESULT_TYPEDEF_(0x80090342L)

//
// MessageId: SEC_E_UNSUPPORTED_PREAUTH
//
// MessageText:
//
//  An unsupported preauthentication mechanism was presented to the kerberos package.
//
#define SEC_E_UNSUPPORTED_PREAUTH        _HRESULT_TYPEDEF_(0x80090343L)

//
// MessageId: SEC_E_DELEGATION_REQUIRED
//
// MessageText:
//
//  The requested operation cannot be completed.  The computer must be trusted for delegation and the current user account must be configured to allow delegation.
//
#define SEC_E_DELEGATION_REQUIRED        _HRESULT_TYPEDEF_(0x80090345L)

//
// MessageId: SEC_E_BAD_BINDINGS
//
// MessageText:
//
//  Client's supplied SSPI channel bindings were incorrect.
//
#define SEC_E_BAD_BINDINGS               _HRESULT_TYPEDEF_(0x80090346L)

//
// MessageId: SEC_E_MULTIPLE_ACCOUNTS
//
// MessageText:
//
//  The received certificate was mapped to multiple accounts.
//
#define SEC_E_MULTIPLE_ACCOUNTS          _HRESULT_TYPEDEF_(0x80090347L)

//
// MessageId: SEC_E_NO_KERB_KEY
//
// MessageText:
//
//  SEC_E_NO_KERB_KEY
//
#define SEC_E_NO_KERB_KEY                _HRESULT_TYPEDEF_(0x80090348L)

//
// MessageId: SEC_E_CERT_WRONG_USAGE
//
// MessageText:
//
//  The certificate is not valid for the requested usage.
//
#define SEC_E_CERT_WRONG_USAGE           _HRESULT_TYPEDEF_(0x80090349L)

//
// MessageId: SEC_E_DOWNGRADE_DETECTED
//
// MessageText:
//
//  The system detected a possible attempt to compromise security.  Please ensure that you can contact the server that authenticated you.
//
#define SEC_E_DOWNGRADE_DETECTED         _HRESULT_TYPEDEF_(0x80090350L)

//
// MessageId: SEC_E_SMARTCARD_CERT_REVOKED
//
// MessageText:
//
//  The smartcard certificate used for authentication has been revoked.
//  Please contact your system administrator.  There may be additional information in the
//  event log.
//
#define SEC_E_SMARTCARD_CERT_REVOKED     _HRESULT_TYPEDEF_(0x80090351L)

//
// MessageId: SEC_E_ISSUING_CA_UNTRUSTED
//
// MessageText:
//
//  An untrusted certificate authority was detected While processing the
//  smartcard certificate used for authentication.  Please contact your system
//  administrator.
//
#define SEC_E_ISSUING_CA_UNTRUSTED       _HRESULT_TYPEDEF_(0x80090352L)

//
// MessageId: SEC_E_REVOCATION_OFFLINE_C
//
// MessageText:
//
//  The revocation status of the smartcard certificate used for
//  authentication could not be determined. Please contact your system administrator.
//
#define SEC_E_REVOCATION_OFFLINE_C       _HRESULT_TYPEDEF_(0x80090353L)

//
// MessageId: SEC_E_PKINIT_CLIENT_FAILURE
//
// MessageText:
//
//  The smartcard certificate used for authentication was not trusted.  Please
//  contact your system administrator.
//
#define SEC_E_PKINIT_CLIENT_FAILURE      _HRESULT_TYPEDEF_(0x80090354L)

//
// MessageId: SEC_E_SMARTCARD_CERT_EXPIRED
//
// MessageText:
//
//  The smartcard certificate used for authentication has expired.  Please
//  contact your system administrator.
//
#define SEC_E_SMARTCARD_CERT_EXPIRED     _HRESULT_TYPEDEF_(0x80090355L)

//
// MessageId: SEC_E_NO_S4U_PROT_SUPPORT
//
// MessageText:
//
//  The Kerberos subsystem encountered an error.  A service for user protocol request was made
//  against a domain controller which does not support service for user.
//
#define SEC_E_NO_S4U_PROT_SUPPORT        _HRESULT_TYPEDEF_(0x80090356L)

//
// MessageId: SEC_E_CROSSREALM_DELEGATION_FAILURE
//
// MessageText:
//
//  An attempt was made by this server to make a Kerberos constrained delegation request for a target
//  outside of the server's realm.  This is not supported, and indicates a misconfiguration on this
//  server's allowed to delegate to list.  Please contact your administrator.
//
#define SEC_E_CROSSREALM_DELEGATION_FAILURE _HRESULT_TYPEDEF_(0x80090357L)

//
// MessageId: SEC_E_REVOCATION_OFFLINE_KDC
//
// MessageText:
//
//  The revocation status of the domain controller certificate used for smartcard
//  authentication could not be determined.  There is additional information in the system event
//  log. Please contact your system administrator.
//
#define SEC_E_REVOCATION_OFFLINE_KDC     _HRESULT_TYPEDEF_(0x80090358L)

//
// MessageId: SEC_E_ISSUING_CA_UNTRUSTED_KDC
//
// MessageText:
//
//  An untrusted certificate authority was detected while processing the
//  domain controller certificate used for authentication.  There is additional information in
//  the system event log.  Please contact your system administrator.
//
#define SEC_E_ISSUING_CA_UNTRUSTED_KDC   _HRESULT_TYPEDEF_(0x80090359L)

//
// MessageId: SEC_E_KDC_CERT_EXPIRED
//
// MessageText:
//
//  The domain controller certificate used for smartcard logon has expired.
//  Please contact your system administrator with the contents of your system event log.
//
#define SEC_E_KDC_CERT_EXPIRED           _HRESULT_TYPEDEF_(0x8009035AL)

//
// MessageId: SEC_E_KDC_CERT_REVOKED
//
// MessageText:
//
//  The domain controller certificate used for smartcard logon has been revoked.
//  Please contact your system administrator with the contents of your system event log.
//
#define SEC_E_KDC_CERT_REVOKED           _HRESULT_TYPEDEF_(0x8009035BL)

//
// Provided for backwards compatibility
//

#define SEC_E_NO_SPM SEC_E_INTERNAL_ERROR
#define SEC_E_NOT_SUPPORTED SEC_E_UNSUPPORTED_FUNCTION

//
// MessageId: CRYPT_E_MSG_ERROR
//
// MessageText:
//
//  An error occurred while performing an operation on a cryptographic message.
//
#define CRYPT_E_MSG_ERROR                _HRESULT_TYPEDEF_(0x80091001L)

//
// MessageId: CRYPT_E_UNKNOWN_ALGO
//
// MessageText:
//
//  Unknown cryptographic algorithm.
//
#define CRYPT_E_UNKNOWN_ALGO             _HRESULT_TYPEDEF_(0x80091002L)

//
// MessageId: CRYPT_E_OID_FORMAT
//
// MessageText:
//
//  The object identifier is poorly formatted.
//
#define CRYPT_E_OID_FORMAT               _HRESULT_TYPEDEF_(0x80091003L)

//
// MessageId: CRYPT_E_INVALID_MSG_TYPE
//
// MessageText:
//
//  Invalid cryptographic message type.
//
#define CRYPT_E_INVALID_MSG_TYPE         _HRESULT_TYPEDEF_(0x80091004L)

//
// MessageId: CRYPT_E_UNEXPECTED_ENCODING
//
// MessageText:
//
//  Unexpected cryptographic message encoding.
//
#define CRYPT_E_UNEXPECTED_ENCODING      _HRESULT_TYPEDEF_(0x80091005L)

//
// MessageId: CRYPT_E_AUTH_ATTR_MISSING
//
// MessageText:
//
//  The cryptographic message does not contain an expected authenticated attribute.
//
#define CRYPT_E_AUTH_ATTR_MISSING        _HRESULT_TYPEDEF_(0x80091006L)

//
// MessageId: CRYPT_E_HASH_VALUE
//
// MessageText:
//
//  The hash value is not correct.
//
#define CRYPT_E_HASH_VALUE               _HRESULT_TYPEDEF_(0x80091007L)

//
// MessageId: CRYPT_E_INVALID_INDEX
//
// MessageText:
//
//  The index value is not valid.
//
#define CRYPT_E_INVALID_INDEX            _HRESULT_TYPEDEF_(0x80091008L)

//
// MessageId: CRYPT_E_ALREADY_DECRYPTED
//
// MessageText:
//
//  The content of the cryptographic message has already been decrypted.
//
#define CRYPT_E_ALREADY_DECRYPTED        _HRESULT_TYPEDEF_(0x80091009L)

//
// MessageId: CRYPT_E_NOT_DECRYPTED
//
// MessageText:
//
//  The content of the cryptographic message has not been decrypted yet.
//
#define CRYPT_E_NOT_DECRYPTED            _HRESULT_TYPEDEF_(0x8009100AL)

//
// MessageId: CRYPT_E_RECIPIENT_NOT_FOUND
//
// MessageText:
//
//  The enveloped-data message does not contain the specified recipient.
//
#define CRYPT_E_RECIPIENT_NOT_FOUND      _HRESULT_TYPEDEF_(0x8009100BL)

//
// MessageId: CRYPT_E_CONTROL_TYPE
//
// MessageText:
//
//  Invalid control type.
//
#define CRYPT_E_CONTROL_TYPE             _HRESULT_TYPEDEF_(0x8009100CL)

//
// MessageId: CRYPT_E_ISSUER_SERIALNUMBER
//
// MessageText:
//
//  Invalid issuer and/or serial number.
//
#define CRYPT_E_ISSUER_SERIALNUMBER      _HRESULT_TYPEDEF_(0x8009100DL)

//
// MessageId: CRYPT_E_SIGNER_NOT_FOUND
//
// MessageText:
//
//  Cannot find the original signer.
//
#define CRYPT_E_SIGNER_NOT_FOUND         _HRESULT_TYPEDEF_(0x8009100EL)

//
// MessageId: CRYPT_E_ATTRIBUTES_MISSING
//
// MessageText:
//
//  The cryptographic message does not contain all of the requested attributes.
//
#define CRYPT_E_ATTRIBUTES_MISSING       _HRESULT_TYPEDEF_(0x8009100FL)

//
// MessageId: CRYPT_E_STREAM_MSG_NOT_READY
//
// MessageText:
//
//  The streamed cryptographic message is not ready to return data.
//
#define CRYPT_E_STREAM_MSG_NOT_READY     _HRESULT_TYPEDEF_(0x80091010L)

//
// MessageId: CRYPT_E_STREAM_INSUFFICIENT_DATA
//
// MessageText:
//
//  The streamed cryptographic message requires more data to complete the decode operation.
//
#define CRYPT_E_STREAM_INSUFFICIENT_DATA _HRESULT_TYPEDEF_(0x80091011L)

//
// MessageId: CRYPT_I_NEW_PROTECTION_REQUIRED
//
// MessageText:
//
//  The protected data needs to be re-protected.
//
#define CRYPT_I_NEW_PROTECTION_REQUIRED  _HRESULT_TYPEDEF_(0x00091012L)

//
// MessageId: CRYPT_E_BAD_LEN
//
// MessageText:
//
//  The length specified for the output data was insufficient.
//
#define CRYPT_E_BAD_LEN                  _HRESULT_TYPEDEF_(0x80092001L)

//
// MessageId: CRYPT_E_BAD_ENCODE
//
// MessageText:
//
//  An error occurred during encode or decode operation.
//
#define CRYPT_E_BAD_ENCODE               _HRESULT_TYPEDEF_(0x80092002L)

//
// MessageId: CRYPT_E_FILE_ERROR
//
// MessageText:
//
//  An error occurred while reading or writing to a file.
//
#define CRYPT_E_FILE_ERROR               _HRESULT_TYPEDEF_(0x80092003L)

//
// MessageId: CRYPT_E_NOT_FOUND
//
// MessageText:
//
//  Cannot find object or property.
//
#define CRYPT_E_NOT_FOUND                _HRESULT_TYPEDEF_(0x80092004L)

//
// MessageId: CRYPT_E_EXISTS
//
// MessageText:
//
//  The object or property already exists.
//
#define CRYPT_E_EXISTS                   _HRESULT_TYPEDEF_(0x80092005L)

//
// MessageId: CRYPT_E_NO_PROVIDER
//
// MessageText:
//
//  No provider was specified for the store or object.
//
#define CRYPT_E_NO_PROVIDER              _HRESULT_TYPEDEF_(0x80092006L)

//
// MessageId: CRYPT_E_SELF_SIGNED
//
// MessageText:
//
//  The specified certificate is self signed.
//
#define CRYPT_E_SELF_SIGNED              _HRESULT_TYPEDEF_(0x80092007L)

//
// MessageId: CRYPT_E_DELETED_PREV
//
// MessageText:
//
//  The previous certificate or CRL context was deleted.
//
#define CRYPT_E_DELETED_PREV             _HRESULT_TYPEDEF_(0x80092008L)

//
// MessageId: CRYPT_E_NO_MATCH
//
// MessageText:
//
//  Cannot find the requested object.
//
#define CRYPT_E_NO_MATCH                 _HRESULT_TYPEDEF_(0x80092009L)

//
// MessageId: CRYPT_E_UNEXPECTED_MSG_TYPE
//
// MessageText:
//
//  The certificate does not have a property that references a private key.
//
#define CRYPT_E_UNEXPECTED_MSG_TYPE      _HRESULT_TYPEDEF_(0x8009200AL)

//
// MessageId: CRYPT_E_NO_KEY_PROPERTY
//
// MessageText:
//
//  Cannot find the certificate and private key for decryption.
//
#define CRYPT_E_NO_KEY_PROPERTY          _HRESULT_TYPEDEF_(0x8009200BL)

//
// MessageId: CRYPT_E_NO_DECRYPT_CERT
//
// MessageText:
//
//  Cannot find the certificate and private key to use for decryption.
//
#define CRYPT_E_NO_DECRYPT_CERT          _HRESULT_TYPEDEF_(0x8009200CL)

//
// MessageId: CRYPT_E_BAD_MSG
//
// MessageText:
//
//  Not a cryptographic message or the cryptographic message is not formatted correctly.
//
#define CRYPT_E_BAD_MSG                  _HRESULT_TYPEDEF_(0x8009200DL)

//
// MessageId: CRYPT_E_NO_SIGNER
//
// MessageText:
//
//  The signed cryptographic message does not have a signer for the specified signer index.
//
#define CRYPT_E_NO_SIGNER                _HRESULT_TYPEDEF_(0x8009200EL)

//
// MessageId: CRYPT_E_PENDING_CLOSE
//
// MessageText:
//
//  Final closure is pending until additional frees or closes.
//
#define CRYPT_E_PENDING_CLOSE            _HRESULT_TYPEDEF_(0x8009200FL)

//
// MessageId: CRYPT_E_REVOKED
//
// MessageText:
//
//  The certificate is revoked.
//
#define CRYPT_E_REVOKED                  _HRESULT_TYPEDEF_(0x80092010L)

//
// MessageId: CRYPT_E_NO_REVOCATION_DLL
//
// MessageText:
//
//  No Dll or exported function was found to verify revocation.
//
#define CRYPT_E_NO_REVOCATION_DLL        _HRESULT_TYPEDEF_(0x80092011L)

//
// MessageId: CRYPT_E_NO_REVOCATION_CHECK
//
// MessageText:
//
//  The revocation function was unable to check revocation for the certificate.
//
#define CRYPT_E_NO_REVOCATION_CHECK      _HRESULT_TYPEDEF_(0x80092012L)

//
// MessageId: CRYPT_E_REVOCATION_OFFLINE
//
// MessageText:
//
//  The revocation function was unable to check revocation because the revocation server was offline.
//
#define CRYPT_E_REVOCATION_OFFLINE       _HRESULT_TYPEDEF_(0x80092013L)

//
// MessageId: CRYPT_E_NOT_IN_REVOCATION_DATABASE
//
// MessageText:
//
//  The certificate is not in the revocation server's database.
//
#define CRYPT_E_NOT_IN_REVOCATION_DATABASE _HRESULT_TYPEDEF_(0x80092014L)

//
// MessageId: CRYPT_E_INVALID_NUMERIC_STRING
//
// MessageText:
//
//  The string contains a non-numeric character.
//
#define CRYPT_E_INVALID_NUMERIC_STRING   _HRESULT_TYPEDEF_(0x80092020L)

//
// MessageId: CRYPT_E_INVALID_PRINTABLE_STRING
//
// MessageText:
//
//  The string contains a non-printable character.
//
#define CRYPT_E_INVALID_PRINTABLE_STRING _HRESULT_TYPEDEF_(0x80092021L)

//
// MessageId: CRYPT_E_INVALID_IA5_STRING
//
// MessageText:
//
//  The string contains a character not in the 7 bit ASCII character set.
//
#define CRYPT_E_INVALID_IA5_STRING       _HRESULT_TYPEDEF_(0x80092022L)

//
// MessageId: CRYPT_E_INVALID_X500_STRING
//
// MessageText:
//
//  The string contains an invalid X500 name attribute key, oid, value or delimiter.
//
#define CRYPT_E_INVALID_X500_STRING      _HRESULT_TYPEDEF_(0x80092023L)

//
// MessageId: CRYPT_E_NOT_CHAR_STRING
//
// MessageText:
//
//  The dwValueType for the CERT_NAME_VALUE is not one of the character strings.  Most likely it is either a CERT_RDN_ENCODED_BLOB or CERT_TDN_OCTED_STRING.
//
#define CRYPT_E_NOT_CHAR_STRING          _HRESULT_TYPEDEF_(0x80092024L)

//
// MessageId: CRYPT_E_FILERESIZED
//
// MessageText:
//
//  The Put operation can not continue.  The file needs to be resized.  However, there is already a signature present.  A complete signing operation must be done.
//
#define CRYPT_E_FILERESIZED              _HRESULT_TYPEDEF_(0x80092025L)

//
// MessageId: CRYPT_E_SECURITY_SETTINGS
//
// MessageText:
//
//  The cryptographic operation failed due to a local security option setting.
//
#define CRYPT_E_SECURITY_SETTINGS        _HRESULT_TYPEDEF_(0x80092026L)

//
// MessageId: CRYPT_E_NO_VERIFY_USAGE_DLL
//
// MessageText:
//
//  No DLL or exported function was found to verify subject usage.
//
#define CRYPT_E_NO_VERIFY_USAGE_DLL      _HRESULT_TYPEDEF_(0x80092027L)

//
// MessageId: CRYPT_E_NO_VERIFY_USAGE_CHECK
//
// MessageText:
//
//  The called function was unable to do a usage check on the subject.
//
#define CRYPT_E_NO_VERIFY_USAGE_CHECK    _HRESULT_TYPEDEF_(0x80092028L)

//
// MessageId: CRYPT_E_VERIFY_USAGE_OFFLINE
//
// MessageText:
//
//  Since the server was offline, the called function was unable to complete the usage check.
//
#define CRYPT_E_VERIFY_USAGE_OFFLINE     _HRESULT_TYPEDEF_(0x80092029L)

//
// MessageId: CRYPT_E_NOT_IN_CTL
//
// MessageText:
//
//  The subject was not found in a Certificate Trust List (CTL).
//
#define CRYPT_E_NOT_IN_CTL               _HRESULT_TYPEDEF_(0x8009202AL)

//
// MessageId: CRYPT_E_NO_TRUSTED_SIGNER
//
// MessageText:
//
//  None of the signers of the cryptographic message or certificate trust list is trusted.
//
#define CRYPT_E_NO_TRUSTED_SIGNER        _HRESULT_TYPEDEF_(0x8009202BL)

//
// MessageId: CRYPT_E_MISSING_PUBKEY_PARA
//
// MessageText:
//
//  The public key's algorithm parameters are missing.
//
#define CRYPT_E_MISSING_PUBKEY_PARA      _HRESULT_TYPEDEF_(0x8009202CL)

//
// MessageId: CRYPT_E_OSS_ERROR
//
// MessageText:
//
//  OSS Certificate encode/decode error code base
//  
//  See asn1code.h for a definition of the OSS runtime errors. The OSS
//  error values are offset by CRYPT_E_OSS_ERROR.
//
#define CRYPT_E_OSS_ERROR                _HRESULT_TYPEDEF_(0x80093000L)

//
// MessageId: OSS_MORE_BUF
//
// MessageText:
//
//  OSS ASN.1 Error: Output Buffer is too small.
//
#define OSS_MORE_BUF                     _HRESULT_TYPEDEF_(0x80093001L)

//
// MessageId: OSS_NEGATIVE_UINTEGER
//
// MessageText:
//
//  OSS ASN.1 Error: Signed integer is encoded as a unsigned integer.
//
#define OSS_NEGATIVE_UINTEGER            _HRESULT_TYPEDEF_(0x80093002L)

//
// MessageId: OSS_PDU_RANGE
//
// MessageText:
//
//  OSS ASN.1 Error: Unknown ASN.1 data type.
//
#define OSS_PDU_RANGE                    _HRESULT_TYPEDEF_(0x80093003L)

//
// MessageId: OSS_MORE_INPUT
//
// MessageText:
//
//  OSS ASN.1 Error: Output buffer is too small, the decoded data has been truncated.
//
#define OSS_MORE_INPUT                   _HRESULT_TYPEDEF_(0x80093004L)

//
// MessageId: OSS_DATA_ERROR
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_DATA_ERROR                   _HRESULT_TYPEDEF_(0x80093005L)

//
// MessageId: OSS_BAD_ARG
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid argument.
//
#define OSS_BAD_ARG                      _HRESULT_TYPEDEF_(0x80093006L)

//
// MessageId: OSS_BAD_VERSION
//
// MessageText:
//
//  OSS ASN.1 Error: Encode/Decode version mismatch.
//
#define OSS_BAD_VERSION                  _HRESULT_TYPEDEF_(0x80093007L)

//
// MessageId: OSS_OUT_MEMORY
//
// MessageText:
//
//  OSS ASN.1 Error: Out of memory.
//
#define OSS_OUT_MEMORY                   _HRESULT_TYPEDEF_(0x80093008L)

//
// MessageId: OSS_PDU_MISMATCH
//
// MessageText:
//
//  OSS ASN.1 Error: Encode/Decode Error.
//
#define OSS_PDU_MISMATCH                 _HRESULT_TYPEDEF_(0x80093009L)

//
// MessageId: OSS_LIMITED
//
// MessageText:
//
//  OSS ASN.1 Error: Internal Error.
//
#define OSS_LIMITED                      _HRESULT_TYPEDEF_(0x8009300AL)

//
// MessageId: OSS_BAD_PTR
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_BAD_PTR                      _HRESULT_TYPEDEF_(0x8009300BL)

//
// MessageId: OSS_BAD_TIME
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_BAD_TIME                     _HRESULT_TYPEDEF_(0x8009300CL)

//
// MessageId: OSS_INDEFINITE_NOT_SUPPORTED
//
// MessageText:
//
//  OSS ASN.1 Error: Unsupported BER indefinite-length encoding.
//
#define OSS_INDEFINITE_NOT_SUPPORTED     _HRESULT_TYPEDEF_(0x8009300DL)

//
// MessageId: OSS_MEM_ERROR
//
// MessageText:
//
//  OSS ASN.1 Error: Access violation.
//
#define OSS_MEM_ERROR                    _HRESULT_TYPEDEF_(0x8009300EL)

//
// MessageId: OSS_BAD_TABLE
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_BAD_TABLE                    _HRESULT_TYPEDEF_(0x8009300FL)

//
// MessageId: OSS_TOO_LONG
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_TOO_LONG                     _HRESULT_TYPEDEF_(0x80093010L)

//
// MessageId: OSS_CONSTRAINT_VIOLATED
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_CONSTRAINT_VIOLATED          _HRESULT_TYPEDEF_(0x80093011L)

//
// MessageId: OSS_FATAL_ERROR
//
// MessageText:
//
//  OSS ASN.1 Error: Internal Error.
//
#define OSS_FATAL_ERROR                  _HRESULT_TYPEDEF_(0x80093012L)

//
// MessageId: OSS_ACCESS_SERIALIZATION_ERROR
//
// MessageText:
//
//  OSS ASN.1 Error: Multi-threading conflict.
//
#define OSS_ACCESS_SERIALIZATION_ERROR   _HRESULT_TYPEDEF_(0x80093013L)

//
// MessageId: OSS_NULL_TBL
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_NULL_TBL                     _HRESULT_TYPEDEF_(0x80093014L)

//
// MessageId: OSS_NULL_FCN
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_NULL_FCN                     _HRESULT_TYPEDEF_(0x80093015L)

//
// MessageId: OSS_BAD_ENCRULES
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_BAD_ENCRULES                 _HRESULT_TYPEDEF_(0x80093016L)

//
// MessageId: OSS_UNAVAIL_ENCRULES
//
// MessageText:
//
//  OSS ASN.1 Error: Encode/Decode function not implemented.
//
#define OSS_UNAVAIL_ENCRULES             _HRESULT_TYPEDEF_(0x80093017L)

//
// MessageId: OSS_CANT_OPEN_TRACE_WINDOW
//
// MessageText:
//
//  OSS ASN.1 Error: Trace file error.
//
#define OSS_CANT_OPEN_TRACE_WINDOW       _HRESULT_TYPEDEF_(0x80093018L)

//
// MessageId: OSS_UNIMPLEMENTED
//
// MessageText:
//
//  OSS ASN.1 Error: Function not implemented.
//
#define OSS_UNIMPLEMENTED                _HRESULT_TYPEDEF_(0x80093019L)

//
// MessageId: OSS_OID_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_OID_DLL_NOT_LINKED           _HRESULT_TYPEDEF_(0x8009301AL)

//
// MessageId: OSS_CANT_OPEN_TRACE_FILE
//
// MessageText:
//
//  OSS ASN.1 Error: Trace file error.
//
#define OSS_CANT_OPEN_TRACE_FILE         _HRESULT_TYPEDEF_(0x8009301BL)

//
// MessageId: OSS_TRACE_FILE_ALREADY_OPEN
//
// MessageText:
//
//  OSS ASN.1 Error: Trace file error.
//
#define OSS_TRACE_FILE_ALREADY_OPEN      _HRESULT_TYPEDEF_(0x8009301CL)

//
// MessageId: OSS_TABLE_MISMATCH
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_TABLE_MISMATCH               _HRESULT_TYPEDEF_(0x8009301DL)

//
// MessageId: OSS_TYPE_NOT_SUPPORTED
//
// MessageText:
//
//  OSS ASN.1 Error: Invalid data.
//
#define OSS_TYPE_NOT_SUPPORTED           _HRESULT_TYPEDEF_(0x8009301EL)

//
// MessageId: OSS_REAL_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_REAL_DLL_NOT_LINKED          _HRESULT_TYPEDEF_(0x8009301FL)

//
// MessageId: OSS_REAL_CODE_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_REAL_CODE_NOT_LINKED         _HRESULT_TYPEDEF_(0x80093020L)

//
// MessageId: OSS_OUT_OF_RANGE
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_OUT_OF_RANGE                 _HRESULT_TYPEDEF_(0x80093021L)

//
// MessageId: OSS_COPIER_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_COPIER_DLL_NOT_LINKED        _HRESULT_TYPEDEF_(0x80093022L)

//
// MessageId: OSS_CONSTRAINT_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_CONSTRAINT_DLL_NOT_LINKED    _HRESULT_TYPEDEF_(0x80093023L)

//
// MessageId: OSS_COMPARATOR_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_COMPARATOR_DLL_NOT_LINKED    _HRESULT_TYPEDEF_(0x80093024L)

//
// MessageId: OSS_COMPARATOR_CODE_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_COMPARATOR_CODE_NOT_LINKED   _HRESULT_TYPEDEF_(0x80093025L)

//
// MessageId: OSS_MEM_MGR_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_MEM_MGR_DLL_NOT_LINKED       _HRESULT_TYPEDEF_(0x80093026L)

//
// MessageId: OSS_PDV_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_PDV_DLL_NOT_LINKED           _HRESULT_TYPEDEF_(0x80093027L)

//
// MessageId: OSS_PDV_CODE_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_PDV_CODE_NOT_LINKED          _HRESULT_TYPEDEF_(0x80093028L)

//
// MessageId: OSS_API_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_API_DLL_NOT_LINKED           _HRESULT_TYPEDEF_(0x80093029L)

//
// MessageId: OSS_BERDER_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_BERDER_DLL_NOT_LINKED        _HRESULT_TYPEDEF_(0x8009302AL)

//
// MessageId: OSS_PER_DLL_NOT_LINKED
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_PER_DLL_NOT_LINKED           _HRESULT_TYPEDEF_(0x8009302BL)

//
// MessageId: OSS_OPEN_TYPE_ERROR
//
// MessageText:
//
//  OSS ASN.1 Error: Program link error.
//
#define OSS_OPEN_TYPE_ERROR              _HRESULT_TYPEDEF_(0x8009302CL)

//
// MessageId: OSS_MUTEX_NOT_CREATED
//
// MessageText:
//
//  OSS ASN.1 Error: System resource error.
//
#define OSS_MUTEX_NOT_CREATED            _HRESULT_TYPEDEF_(0x8009302DL)

//
// MessageId: OSS_CANT_CLOSE_TRACE_FILE
//
// MessageText:
//
//  OSS ASN.1 Error: Trace file error.
//
#define OSS_CANT_CLOSE_TRACE_FILE        _HRESULT_TYPEDEF_(0x8009302EL)

//
// MessageId: CRYPT_E_ASN1_ERROR
//
// MessageText:
//
//  ASN1 Certificate encode/decode error code base.
//  
//  The ASN1 error values are offset by CRYPT_E_ASN1_ERROR.
//
#define CRYPT_E_ASN1_ERROR               _HRESULT_TYPEDEF_(0x80093100L)

//
// MessageId: CRYPT_E_ASN1_INTERNAL
//
// MessageText:
//
//  ASN1 internal encode or decode error.
//
#define CRYPT_E_ASN1_INTERNAL            _HRESULT_TYPEDEF_(0x80093101L)

//
// MessageId: CRYPT_E_ASN1_EOD
//
// MessageText:
//
//  ASN1 unexpected end of data.
//
#define CRYPT_E_ASN1_EOD                 _HRESULT_TYPEDEF_(0x80093102L)

//
// MessageId: CRYPT_E_ASN1_CORRUPT
//
// MessageText:
//
//  ASN1 corrupted data.
//
#define CRYPT_E_ASN1_CORRUPT             _HRESULT_TYPEDEF_(0x80093103L)

//
// MessageId: CRYPT_E_ASN1_LARGE
//
// MessageText:
//
//  ASN1 value too large.
//
#define CRYPT_E_ASN1_LARGE               _HRESULT_TYPEDEF_(0x80093104L)

//
// MessageId: CRYPT_E_ASN1_CONSTRAINT
//
// MessageText:
//
//  ASN1 constraint violated.
//
#define CRYPT_E_ASN1_CONSTRAINT          _HRESULT_TYPEDEF_(0x80093105L)

//
// MessageId: CRYPT_E_ASN1_MEMORY
//
// MessageText:
//
//  ASN1 out of memory.
//
#define CRYPT_E_ASN1_MEMORY              _HRESULT_TYPEDEF_(0x80093106L)

//
// MessageId: CRYPT_E_ASN1_OVERFLOW
//
// MessageText:
//
//  ASN1 buffer overflow.
//
#define CRYPT_E_ASN1_OVERFLOW            _HRESULT_TYPEDEF_(0x80093107L)

//
// MessageId: CRYPT_E_ASN1_BADPDU
//
// MessageText:
//
//  ASN1 function not supported for this PDU.
//
#define CRYPT_E_ASN1_BADPDU              _HRESULT_TYPEDEF_(0x80093108L)

//
// MessageId: CRYPT_E_ASN1_BADARGS
//
// MessageText:
//
//  ASN1 bad arguments to function call.
//
#define CRYPT_E_ASN1_BADARGS             _HRESULT_TYPEDEF_(0x80093109L)

//
// MessageId: CRYPT_E_ASN1_BADREAL
//
// MessageText:
//
//  ASN1 bad real value.
//
#define CRYPT_E_ASN1_BADREAL             _HRESULT_TYPEDEF_(0x8009310AL)

//
// MessageId: CRYPT_E_ASN1_BADTAG
//
// MessageText:
//
//  ASN1 bad tag value met.
//
#define CRYPT_E_ASN1_BADTAG              _HRESULT_TYPEDEF_(0x8009310BL)

//
// MessageId: CRYPT_E_ASN1_CHOICE
//
// MessageText:
//
//  ASN1 bad choice value.
//
#define CRYPT_E_ASN1_CHOICE              _HRESULT_TYPEDEF_(0x8009310CL)

//
// MessageId: CRYPT_E_ASN1_RULE
//
// MessageText:
//
//  ASN1 bad encoding rule.
//
#define CRYPT_E_ASN1_RULE                _HRESULT_TYPEDEF_(0x8009310DL)

//
// MessageId: CRYPT_E_ASN1_UTF8
//
// MessageText:
//
//  ASN1 bad unicode (UTF8).
//
#define CRYPT_E_ASN1_UTF8                _HRESULT_TYPEDEF_(0x8009310EL)

//
// MessageId: CRYPT_E_ASN1_PDU_TYPE
//
// MessageText:
//
//  ASN1 bad PDU type.
//
#define CRYPT_E_ASN1_PDU_TYPE            _HRESULT_TYPEDEF_(0x80093133L)

//
// MessageId: CRYPT_E_ASN1_NYI
//
// MessageText:
//
//  ASN1 not yet implemented.
//
#define CRYPT_E_ASN1_NYI                 _HRESULT_TYPEDEF_(0x80093134L)

//
// MessageId: CRYPT_E_ASN1_EXTENDED
//
// MessageText:
//
//  ASN1 skipped unknown extension(s).
//
#define CRYPT_E_ASN1_EXTENDED            _HRESULT_TYPEDEF_(0x80093201L)

//
// MessageId: CRYPT_E_ASN1_NOEOD
//
// MessageText:
//
//  ASN1 end of data expected
//
#define CRYPT_E_ASN1_NOEOD               _HRESULT_TYPEDEF_(0x80093202L)

//
// MessageId: CERTSRV_E_BAD_REQUESTSUBJECT
//
// MessageText:
//
//  The request subject name is invalid or too long.
//
#define CERTSRV_E_BAD_REQUESTSUBJECT     _HRESULT_TYPEDEF_(0x80094001L)

//
// MessageId: CERTSRV_E_NO_REQUEST
//
// MessageText:
//
//  The request does not exist.
//
#define CERTSRV_E_NO_REQUEST             _HRESULT_TYPEDEF_(0x80094002L)

//
// MessageId: CERTSRV_E_BAD_REQUESTSTATUS
//
// MessageText:
//
//  The request's current status does not allow this operation.
//
#define CERTSRV_E_BAD_REQUESTSTATUS      _HRESULT_TYPEDEF_(0x80094003L)

//
// MessageId: CERTSRV_E_PROPERTY_EMPTY
//
// MessageText:
//
//  The requested property value is empty.
//
#define CERTSRV_E_PROPERTY_EMPTY         _HRESULT_TYPEDEF_(0x80094004L)

//
// MessageId: CERTSRV_E_INVALID_CA_CERTIFICATE
//
// MessageText:
//
//  The certification authority's certificate contains invalid data.
//
#define CERTSRV_E_INVALID_CA_CERTIFICATE _HRESULT_TYPEDEF_(0x80094005L)

//
// MessageId: CERTSRV_E_SERVER_SUSPENDED
//
// MessageText:
//
//  Certificate service has been suspended for a database restore operation.
//
#define CERTSRV_E_SERVER_SUSPENDED       _HRESULT_TYPEDEF_(0x80094006L)

//
// MessageId: CERTSRV_E_ENCODING_LENGTH
//
// MessageText:
//
//  The certificate contains an encoded length that is potentially incompatible with older enrollment software.
//
#define CERTSRV_E_ENCODING_LENGTH        _HRESULT_TYPEDEF_(0x80094007L)

//
// MessageId: CERTSRV_E_ROLECONFLICT
//
// MessageText:
//
//  The operation is denied. The user has multiple roles assigned and the certification authority is configured to enforce role separation.
//
#define CERTSRV_E_ROLECONFLICT           _HRESULT_TYPEDEF_(0x80094008L)

//
// MessageId: CERTSRV_E_RESTRICTEDOFFICER
//
// MessageText:
//
//  The operation is denied. It can only be performed by a certificate manager that is allowed to manage certificates for the current requester.
//
#define CERTSRV_E_RESTRICTEDOFFICER      _HRESULT_TYPEDEF_(0x80094009L)

//
// MessageId: CERTSRV_E_KEY_ARCHIVAL_NOT_CONFIGURED
//
// MessageText:
//
//  Cannot archive private key.  The certification authority is not configured for key archival.
//
#define CERTSRV_E_KEY_ARCHIVAL_NOT_CONFIGURED _HRESULT_TYPEDEF_(0x8009400AL)

//
// MessageId: CERTSRV_E_NO_VALID_KRA
//
// MessageText:
//
//  Cannot archive private key.  The certification authority could not verify one or more key recovery certificates.
//
#define CERTSRV_E_NO_VALID_KRA           _HRESULT_TYPEDEF_(0x8009400BL)

//
// MessageId: CERTSRV_E_BAD_REQUEST_KEY_ARCHIVAL
//
// MessageText:
//
//  The request is incorrectly formatted.  The encrypted private key must be in an unauthenticated attribute in an outermost signature.
//
#define CERTSRV_E_BAD_REQUEST_KEY_ARCHIVAL _HRESULT_TYPEDEF_(0x8009400CL)

//
// MessageId: CERTSRV_E_NO_CAADMIN_DEFINED
//
// MessageText:
//
//  At least one security principal must have the permission to manage this CA.
//
#define CERTSRV_E_NO_CAADMIN_DEFINED     _HRESULT_TYPEDEF_(0x8009400DL)

//
// MessageId: CERTSRV_E_BAD_RENEWAL_CERT_ATTRIBUTE
//
// MessageText:
//
//  The request contains an invalid renewal certificate attribute.
//
#define CERTSRV_E_BAD_RENEWAL_CERT_ATTRIBUTE _HRESULT_TYPEDEF_(0x8009400EL)

//
// MessageId: CERTSRV_E_NO_DB_SESSIONS
//
// MessageText:
//
//  An attempt was made to open a Certification Authority database session, but there are already too many active sessions.  The server may need to be configured to allow additional sessions.
//
#define CERTSRV_E_NO_DB_SESSIONS         _HRESULT_TYPEDEF_(0x8009400FL)

//
// MessageId: CERTSRV_E_ALIGNMENT_FAULT
//
// MessageText:
//
//  A memory reference caused a data alignment fault.
//
#define CERTSRV_E_ALIGNMENT_FAULT        _HRESULT_TYPEDEF_(0x80094010L)

//
// MessageId: CERTSRV_E_ENROLL_DENIED
//
// MessageText:
//
//  The permissions on this certification authority do not allow the current user to enroll for certificates.
//
#define CERTSRV_E_ENROLL_DENIED          _HRESULT_TYPEDEF_(0x80094011L)

//
// MessageId: CERTSRV_E_TEMPLATE_DENIED
//
// MessageText:
//
//  The permissions on the certificate template do not allow the current user to enroll for this type of certificate.
//
#define CERTSRV_E_TEMPLATE_DENIED        _HRESULT_TYPEDEF_(0x80094012L)

//
// MessageId: CERTSRV_E_DOWNLEVEL_DC_SSL_OR_UPGRADE
//
// MessageText:
//
//  The contacted domain controller cannot support signed LDAP traffic.  Update the domain controller or configure Certificate Services to use SSL for Active Directory access.
//
#define CERTSRV_E_DOWNLEVEL_DC_SSL_OR_UPGRADE _HRESULT_TYPEDEF_(0x80094013L)

//
// MessageId: CERTSRV_E_UNSUPPORTED_CERT_TYPE
//
// MessageText:
//
//  The requested certificate template is not supported by this CA.
//
#define CERTSRV_E_UNSUPPORTED_CERT_TYPE  _HRESULT_TYPEDEF_(0x80094800L)

//
// MessageId: CERTSRV_E_NO_CERT_TYPE
//
// MessageText:
//
//  The request contains no certificate template information.
//
#define CERTSRV_E_NO_CERT_TYPE           _HRESULT_TYPEDEF_(0x80094801L)

//
// MessageId: CERTSRV_E_TEMPLATE_CONFLICT
//
// MessageText:
//
//  The request contains conflicting template information.
//
#define CERTSRV_E_TEMPLATE_CONFLICT      _HRESULT_TYPEDEF_(0x80094802L)

//
// MessageId: CERTSRV_E_SUBJECT_ALT_NAME_REQUIRED
//
// MessageText:
//
//  The request is missing a required Subject Alternate name extension.
//
#define CERTSRV_E_SUBJECT_ALT_NAME_REQUIRED _HRESULT_TYPEDEF_(0x80094803L)

//
// MessageId: CERTSRV_E_ARCHIVED_KEY_REQUIRED
//
// MessageText:
//
//  The request is missing a required private key for archival by the server.
//
#define CERTSRV_E_ARCHIVED_KEY_REQUIRED  _HRESULT_TYPEDEF_(0x80094804L)

//
// MessageId: CERTSRV_E_SMIME_REQUIRED
//
// MessageText:
//
//  The request is missing a required SMIME capabilities extension.
//
#define CERTSRV_E_SMIME_REQUIRED         _HRESULT_TYPEDEF_(0x80094805L)

//
// MessageId: CERTSRV_E_BAD_RENEWAL_SUBJECT
//
// MessageText:
//
//  The request was made on behalf of a subject other than the caller.  The certificate template must be configured to require at least one signature to authorize the request.
//
#define CERTSRV_E_BAD_RENEWAL_SUBJECT    _HRESULT_TYPEDEF_(0x80094806L)

//
// MessageId: CERTSRV_E_BAD_TEMPLATE_VERSION
//
// MessageText:
//
//  The request template version is newer than the supported template version.
//
#define CERTSRV_E_BAD_TEMPLATE_VERSION   _HRESULT_TYPEDEF_(0x80094807L)

//
// MessageId: CERTSRV_E_TEMPLATE_POLICY_REQUIRED
//
// MessageText:
//
//  The template is missing a required signature policy attribute.
//
#define CERTSRV_E_TEMPLATE_POLICY_REQUIRED _HRESULT_TYPEDEF_(0x80094808L)

//
// MessageId: CERTSRV_E_SIGNATURE_POLICY_REQUIRED
//
// MessageText:
//
//  The request is missing required signature policy information.
//
#define CERTSRV_E_SIGNATURE_POLICY_REQUIRED _HRESULT_TYPEDEF_(0x80094809L)

//
// MessageId: CERTSRV_E_SIGNATURE_COUNT
//
// MessageText:
//
//  The request is missing one or more required signatures.
//
#define CERTSRV_E_SIGNATURE_COUNT        _HRESULT_TYPEDEF_(0x8009480AL)

//
// MessageId: CERTSRV_E_SIGNATURE_REJECTED
//
// MessageText:
//
//  One or more signatures did not include the required application or issuance policies.  The request is missing one or more required valid signatures.
//
#define CERTSRV_E_SIGNATURE_REJECTED     _HRESULT_TYPEDEF_(0x8009480BL)

//
// MessageId: CERTSRV_E_ISSUANCE_POLICY_REQUIRED
//
// MessageText:
//
//  The request is missing one or more required signature issuance policies.
//
#define CERTSRV_E_ISSUANCE_POLICY_REQUIRED _HRESULT_TYPEDEF_(0x8009480CL)

//
// MessageId: CERTSRV_E_SUBJECT_UPN_REQUIRED
//
// MessageText:
//
//  The UPN is unavailable and cannot be added to the Subject Alternate name.
//
#define CERTSRV_E_SUBJECT_UPN_REQUIRED   _HRESULT_TYPEDEF_(0x8009480DL)

//
// MessageId: CERTSRV_E_SUBJECT_DIRECTORY_GUID_REQUIRED
//
// MessageText:
//
//  The Active Directory GUID is unavailable and cannot be added to the Subject Alternate name.
//
#define CERTSRV_E_SUBJECT_DIRECTORY_GUID_REQUIRED _HRESULT_TYPEDEF_(0x8009480EL)

//
// MessageId: CERTSRV_E_SUBJECT_DNS_REQUIRED
//
// MessageText:
//
//  The DNS name is unavailable and cannot be added to the Subject Alternate name.
//
#define CERTSRV_E_SUBJECT_DNS_REQUIRED   _HRESULT_TYPEDEF_(0x8009480FL)

//
// MessageId: CERTSRV_E_ARCHIVED_KEY_UNEXPECTED
//
// MessageText:
//
//  The request includes a private key for archival by the server, but key archival is not enabled for the specified certificate template.
//
#define CERTSRV_E_ARCHIVED_KEY_UNEXPECTED _HRESULT_TYPEDEF_(0x80094810L)

//
// MessageId: CERTSRV_E_KEY_LENGTH
//
// MessageText:
//
//  The public key does not meet the minimum size required by the specified certificate template.
//
#define CERTSRV_E_KEY_LENGTH             _HRESULT_TYPEDEF_(0x80094811L)

//
// MessageId: CERTSRV_E_SUBJECT_EMAIL_REQUIRED
//
// MessageText:
//
//  The EMail name is unavailable and cannot be added to the Subject or Subject Alternate name.
//
#define CERTSRV_E_SUBJECT_EMAIL_REQUIRED _HRESULT_TYPEDEF_(0x80094812L)

//
// MessageId: CERTSRV_E_UNKNOWN_CERT_TYPE
//
// MessageText:
//
//  One or more certificate templates to be enabled on this certification authority could not be found.
//
#define CERTSRV_E_UNKNOWN_CERT_TYPE      _HRESULT_TYPEDEF_(0x80094813L)

//
// MessageId: CERTSRV_E_CERT_TYPE_OVERLAP
//
// MessageText:
//
//  The certificate template renewal period is longer than the certificate validity period.  The template should be reconfigured or the CA certificate renewed.
//
#define CERTSRV_E_CERT_TYPE_OVERLAP      _HRESULT_TYPEDEF_(0x80094814L)

//
// The range 0x5000-0x51ff is reserved for XENROLL errors.
//
//
// MessageId: XENROLL_E_KEY_NOT_EXPORTABLE
//
// MessageText:
//
//  The key is not exportable.
//
#define XENROLL_E_KEY_NOT_EXPORTABLE     _HRESULT_TYPEDEF_(0x80095000L)

//
// MessageId: XENROLL_E_CANNOT_ADD_ROOT_CERT
//
// MessageText:
//
//  You cannot add the root CA certificate into your local store.
//
#define XENROLL_E_CANNOT_ADD_ROOT_CERT   _HRESULT_TYPEDEF_(0x80095001L)

//
// MessageId: XENROLL_E_RESPONSE_KA_HASH_NOT_FOUND
//
// MessageText:
//
//  The key archival hash attribute was not found in the response.
//
#define XENROLL_E_RESPONSE_KA_HASH_NOT_FOUND _HRESULT_TYPEDEF_(0x80095002L)

//
// MessageId: XENROLL_E_RESPONSE_UNEXPECTED_KA_HASH
//
// MessageText:
//
//  An unexpected key archival hash attribute was found in the response.
//
#define XENROLL_E_RESPONSE_UNEXPECTED_KA_HASH _HRESULT_TYPEDEF_(0x80095003L)

//
// MessageId: XENROLL_E_RESPONSE_KA_HASH_MISMATCH
//
// MessageText:
//
//  There is a key archival hash mismatch between the request and the response.
//
#define XENROLL_E_RESPONSE_KA_HASH_MISMATCH _HRESULT_TYPEDEF_(0x80095004L)

//
// MessageId: XENROLL_E_KEYSPEC_SMIME_MISMATCH
//
// MessageText:
//
//  Signing certificate cannot include SMIME extension.
//
#define XENROLL_E_KEYSPEC_SMIME_MISMATCH _HRESULT_TYPEDEF_(0x80095005L)

//
// MessageId: TRUST_E_SYSTEM_ERROR
//
// MessageText:
//
//  A system-level error occurred while verifying trust.
//
#define TRUST_E_SYSTEM_ERROR             _HRESULT_TYPEDEF_(0x80096001L)

//
// MessageId: TRUST_E_NO_SIGNER_CERT
//
// MessageText:
//
//  The certificate for the signer of the message is invalid or not found.
//
#define TRUST_E_NO_SIGNER_CERT           _HRESULT_TYPEDEF_(0x80096002L)

//
// MessageId: TRUST_E_COUNTER_SIGNER
//
// MessageText:
//
//  One of the counter signatures was invalid.
//
#define TRUST_E_COUNTER_SIGNER           _HRESULT_TYPEDEF_(0x80096003L)

//
// MessageId: TRUST_E_CERT_SIGNATURE
//
// MessageText:
//
//  The signature of the certificate can not be verified.
//
#define TRUST_E_CERT_SIGNATURE           _HRESULT_TYPEDEF_(0x80096004L)

//
// MessageId: TRUST_E_TIME_STAMP
//
// MessageText:
//
//  The timestamp signature and/or certificate could not be verified or is malformed.
//
#define TRUST_E_TIME_STAMP               _HRESULT_TYPEDEF_(0x80096005L)

//
// MessageId: TRUST_E_BAD_DIGEST
//
// MessageText:
//
//  The digital signature of the object did not verify.
//
#define TRUST_E_BAD_DIGEST               _HRESULT_TYPEDEF_(0x80096010L)

//
// MessageId: TRUST_E_BASIC_CONSTRAINTS
//
// MessageText:
//
//  A certificate's basic constraint extension has not been observed.
//
#define TRUST_E_BASIC_CONSTRAINTS        _HRESULT_TYPEDEF_(0x80096019L)

//
// MessageId: TRUST_E_FINANCIAL_CRITERIA
//
// MessageText:
//
//  The certificate does not meet or contain the Authenticode(tm) financial extensions.
//
#define TRUST_E_FINANCIAL_CRITERIA       _HRESULT_TYPEDEF_(0x8009601EL)

//
//  Error codes for mssipotf.dll
//  Most of the error codes can only occur when an error occurs
//    during font file signing
//
//
//
// MessageId: MSSIPOTF_E_OUTOFMEMRANGE
//
// MessageText:
//
//  Tried to reference a part of the file outside the proper range.
//
#define MSSIPOTF_E_OUTOFMEMRANGE         _HRESULT_TYPEDEF_(0x80097001L)

//
// MessageId: MSSIPOTF_E_CANTGETOBJECT
//
// MessageText:
//
//  Could not retrieve an object from the file.
//
#define MSSIPOTF_E_CANTGETOBJECT         _HRESULT_TYPEDEF_(0x80097002L)

//
// MessageId: MSSIPOTF_E_NOHEADTABLE
//
// MessageText:
//
//  Could not find the head table in the file.
//
#define MSSIPOTF_E_NOHEADTABLE           _HRESULT_TYPEDEF_(0x80097003L)

//
// MessageId: MSSIPOTF_E_BAD_MAGICNUMBER
//
// MessageText:
//
//  The magic number in the head table is incorrect.
//
#define MSSIPOTF_E_BAD_MAGICNUMBER       _HRESULT_TYPEDEF_(0x80097004L)

//
// MessageId: MSSIPOTF_E_BAD_OFFSET_TABLE
//
// MessageText:
//
//  The offset table has incorrect values.
//
#define MSSIPOTF_E_BAD_OFFSET_TABLE      _HRESULT_TYPEDEF_(0x80097005L)

//
// MessageId: MSSIPOTF_E_TABLE_TAGORDER
//
// MessageText:
//
//  Duplicate table tags or tags out of alphabetical order.
//
#define MSSIPOTF_E_TABLE_TAGORDER        _HRESULT_TYPEDEF_(0x80097006L)

//
// MessageId: MSSIPOTF_E_TABLE_LONGWORD
//
// MessageText:
//
//  A table does not start on a long word boundary.
//
#define MSSIPOTF_E_TABLE_LONGWORD        _HRESULT_TYPEDEF_(0x80097007L)

//
// MessageId: MSSIPOTF_E_BAD_FIRST_TABLE_PLACEMENT
//
// MessageText:
//
//  First table does not appear after header information.
//
#define MSSIPOTF_E_BAD_FIRST_TABLE_PLACEMENT _HRESULT_TYPEDEF_(0x80097008L)

//
// MessageId: MSSIPOTF_E_TABLES_OVERLAP
//
// MessageText:
//
//  Two or more tables overlap.
//
#define MSSIPOTF_E_TABLES_OVERLAP        _HRESULT_TYPEDEF_(0x80097009L)

//
// MessageId: MSSIPOTF_E_TABLE_PADBYTES
//
// MessageText:
//
//  Too many pad bytes between tables or pad bytes are not 0.
//
#define MSSIPOTF_E_TABLE_PADBYTES        _HRESULT_TYPEDEF_(0x8009700AL)

//
// MessageId: MSSIPOTF_E_FILETOOSMALL
//
// MessageText:
//
//  File is too small to contain the last table.
//
#define MSSIPOTF_E_FILETOOSMALL          _HRESULT_TYPEDEF_(0x8009700BL)

//
// MessageId: MSSIPOTF_E_TABLE_CHECKSUM
//
// MessageText:
//
//  A table checksum is incorrect.
//
#define MSSIPOTF_E_TABLE_CHECKSUM        _HRESULT_TYPEDEF_(0x8009700CL)

//
// MessageId: MSSIPOTF_E_FILE_CHECKSUM
//
// MessageText:
//
//  The file checksum is incorrect.
//
#define MSSIPOTF_E_FILE_CHECKSUM         _HRESULT_TYPEDEF_(0x8009700DL)

//
// MessageId: MSSIPOTF_E_FAILED_POLICY
//
// MessageText:
//
//  The signature does not have the correct attributes for the policy.
//
#define MSSIPOTF_E_FAILED_POLICY         _HRESULT_TYPEDEF_(0x80097010L)

//
// MessageId: MSSIPOTF_E_FAILED_HINTS_CHECK
//
// MessageText:
//
//  The file did not pass the hints check.
//
#define MSSIPOTF_E_FAILED_HINTS_CHECK    _HRESULT_TYPEDEF_(0x80097011L)

//
// MessageId: MSSIPOTF_E_NOT_OPENTYPE
//
// MessageText:
//
//  The file is not an OpenType file.
//
#define MSSIPOTF_E_NOT_OPENTYPE          _HRESULT_TYPEDEF_(0x80097012L)

//
// MessageId: MSSIPOTF_E_FILE
//
// MessageText:
//
//  Failed on a file operation (open, map, read, write).
//
#define MSSIPOTF_E_FILE                  _HRESULT_TYPEDEF_(0x80097013L)

//
// MessageId: MSSIPOTF_E_CRYPT
//
// MessageText:
//
//  A call to a CryptoAPI function failed.
//
#define MSSIPOTF_E_CRYPT                 _HRESULT_TYPEDEF_(0x80097014L)

//
// MessageId: MSSIPOTF_E_BADVERSION
//
// MessageText:
//
//  There is a bad version number in the file.
//
#define MSSIPOTF_E_BADVERSION            _HRESULT_TYPEDEF_(0x80097015L)

//
// MessageId: MSSIPOTF_E_DSIG_STRUCTURE
//
// MessageText:
//
//  The structure of the DSIG table is incorrect.
//
#define MSSIPOTF_E_DSIG_STRUCTURE        _HRESULT_TYPEDEF_(0x80097016L)

//
// MessageId: MSSIPOTF_E_PCONST_CHECK
//
// MessageText:
//
//  A check failed in a partially constant table.
//
#define MSSIPOTF_E_PCONST_CHECK          _HRESULT_TYPEDEF_(0x80097017L)

//
// MessageId: MSSIPOTF_E_STRUCTURE
//
// MessageText:
//
//  Some kind of structural error.
//
#define MSSIPOTF_E_STRUCTURE             _HRESULT_TYPEDEF_(0x80097018L)

#define NTE_OP_OK 0

//
// Note that additional FACILITY_SSPI errors are in issperr.h
//
// ******************
// FACILITY_CERT
// ******************
//
// MessageId: TRUST_E_PROVIDER_UNKNOWN
//
// MessageText:
//
//  Unknown trust provider.
//
#define TRUST_E_PROVIDER_UNKNOWN         _HRESULT_TYPEDEF_(0x800B0001L)

//
// MessageId: TRUST_E_ACTION_UNKNOWN
//
// MessageText:
//
//  The trust verification action specified is not supported by the specified trust provider.
//
#define TRUST_E_ACTION_UNKNOWN           _HRESULT_TYPEDEF_(0x800B0002L)

//
// MessageId: TRUST_E_SUBJECT_FORM_UNKNOWN
//
// MessageText:
//
//  The form specified for the subject is not one supported or known by the specified trust provider.
//
#define TRUST_E_SUBJECT_FORM_UNKNOWN     _HRESULT_TYPEDEF_(0x800B0003L)

//
// MessageId: TRUST_E_SUBJECT_NOT_TRUSTED
//
// MessageText:
//
//  The subject is not trusted for the specified action.
//
#define TRUST_E_SUBJECT_NOT_TRUSTED      _HRESULT_TYPEDEF_(0x800B0004L)

//
// MessageId: DIGSIG_E_ENCODE
//
// MessageText:
//
//  Error due to problem in ASN.1 encoding process.
//
#define DIGSIG_E_ENCODE                  _HRESULT_TYPEDEF_(0x800B0005L)

//
// MessageId: DIGSIG_E_DECODE
//
// MessageText:
//
//  Error due to problem in ASN.1 decoding process.
//
#define DIGSIG_E_DECODE                  _HRESULT_TYPEDEF_(0x800B0006L)

//
// MessageId: DIGSIG_E_EXTENSIBILITY
//
// MessageText:
//
//  Reading / writing Extensions where Attributes are appropriate, and visa versa.
//
#define DIGSIG_E_EXTENSIBILITY           _HRESULT_TYPEDEF_(0x800B0007L)

//
// MessageId: DIGSIG_E_CRYPTO
//
// MessageText:
//
//  Unspecified cryptographic failure.
//
#define DIGSIG_E_CRYPTO                  _HRESULT_TYPEDEF_(0x800B0008L)

//
// MessageId: PERSIST_E_SIZEDEFINITE
//
// MessageText:
//
//  The size of the data could not be determined.
//
#define PERSIST_E_SIZEDEFINITE           _HRESULT_TYPEDEF_(0x800B0009L)

//
// MessageId: PERSIST_E_SIZEINDEFINITE
//
// MessageText:
//
//  The size of the indefinite-sized data could not be determined.
//
#define PERSIST_E_SIZEINDEFINITE         _HRESULT_TYPEDEF_(0x800B000AL)

//
// MessageId: PERSIST_E_NOTSELFSIZING
//
// MessageText:
//
//  This object does not read and write self-sizing data.
//
#define PERSIST_E_NOTSELFSIZING          _HRESULT_TYPEDEF_(0x800B000BL)

//
// MessageId: TRUST_E_NOSIGNATURE
//
// MessageText:
//
//  No signature was present in the subject.
//
#define TRUST_E_NOSIGNATURE              _HRESULT_TYPEDEF_(0x800B0100L)

//
// MessageId: CERT_E_EXPIRED
//
// MessageText:
//
//  A required certificate is not within its validity period when verifying against the current system clock or the timestamp in the signed file.
//
#define CERT_E_EXPIRED                   _HRESULT_TYPEDEF_(0x800B0101L)

//
// MessageId: CERT_E_VALIDITYPERIODNESTING
//
// MessageText:
//
//  The validity periods of the certification chain do not nest correctly.
//
#define CERT_E_VALIDITYPERIODNESTING     _HRESULT_TYPEDEF_(0x800B0102L)

//
// MessageId: CERT_E_ROLE
//
// MessageText:
//
//  A certificate that can only be used as an end-entity is being used as a CA or visa versa.
//
#define CERT_E_ROLE                      _HRESULT_TYPEDEF_(0x800B0103L)

//
// MessageId: CERT_E_PATHLENCONST
//
// MessageText:
//
//  A path length constraint in the certification chain has been violated.
//
#define CERT_E_PATHLENCONST              _HRESULT_TYPEDEF_(0x800B0104L)

//
// MessageId: CERT_E_CRITICAL
//
// MessageText:
//
//  A certificate contains an unknown extension that is marked 'critical'.
//
#define CERT_E_CRITICAL                  _HRESULT_TYPEDEF_(0x800B0105L)

//
// MessageId: CERT_E_PURPOSE
//
// MessageText:
//
//  A certificate being used for a purpose other than the ones specified by its CA.
//
#define CERT_E_PURPOSE                   _HRESULT_TYPEDEF_(0x800B0106L)

//
// MessageId: CERT_E_ISSUERCHAINING
//
// MessageText:
//
//  A parent of a given certificate in fact did not issue that child certificate.
//
#define CERT_E_ISSUERCHAINING            _HRESULT_TYPEDEF_(0x800B0107L)

//
// MessageId: CERT_E_MALFORMED
//
// MessageText:
//
//  A certificate is missing or has an empty value for an important field, such as a subject or issuer name.
//
#define CERT_E_MALFORMED                 _HRESULT_TYPEDEF_(0x800B0108L)

//
// MessageId: CERT_E_UNTRUSTEDROOT
//
// MessageText:
//
//  A certificate chain processed, but terminated in a root certificate which is not trusted by the trust provider.
//
#define CERT_E_UNTRUSTEDROOT             _HRESULT_TYPEDEF_(0x800B0109L)

//
// MessageId: CERT_E_CHAINING
//
// MessageText:
//
//  A certificate chain could not be built to a trusted root authority.
//
#define CERT_E_CHAINING                  _HRESULT_TYPEDEF_(0x800B010AL)

//
// MessageId: TRUST_E_FAIL
//
// MessageText:
//
//  Generic trust failure.
//
#define TRUST_E_FAIL                     _HRESULT_TYPEDEF_(0x800B010BL)

//
// MessageId: CERT_E_REVOKED
//
// MessageText:
//
//  A certificate was explicitly revoked by its issuer.
//
#define CERT_E_REVOKED                   _HRESULT_TYPEDEF_(0x800B010CL)

//
// MessageId: CERT_E_UNTRUSTEDTESTROOT
//
// MessageText:
//
//  The certification path terminates with the test root which is not trusted with the current policy settings.
//
#define CERT_E_UNTRUSTEDTESTROOT         _HRESULT_TYPEDEF_(0x800B010DL)

//
// MessageId: CERT_E_REVOCATION_FAILURE
//
// MessageText:
//
//  The revocation process could not continue - the certificate(s) could not be checked.
//
#define CERT_E_REVOCATION_FAILURE        _HRESULT_TYPEDEF_(0x800B010EL)

//
// MessageId: CERT_E_CN_NO_MATCH
//
// MessageText:
//
//  The certificate's CN name does not match the passed value.
//
#define CERT_E_CN_NO_MATCH               _HRESULT_TYPEDEF_(0x800B010FL)

//
// MessageId: CERT_E_WRONG_USAGE
//
// MessageText:
//
//  The certificate is not valid for the requested usage.
//
#define CERT_E_WRONG_USAGE               _HRESULT_TYPEDEF_(0x800B0110L)

//
// MessageId: TRUST_E_EXPLICIT_DISTRUST
//
// MessageText:
//
//  The certificate was explicitly marked as untrusted by the user.
//
#define TRUST_E_EXPLICIT_DISTRUST        _HRESULT_TYPEDEF_(0x800B0111L)

//
// MessageId: CERT_E_UNTRUSTEDCA
//
// MessageText:
//
//  A certification chain processed correctly, but one of the CA certificates is not trusted by the policy provider.
//
#define CERT_E_UNTRUSTEDCA               _HRESULT_TYPEDEF_(0x800B0112L)

//
// MessageId: CERT_E_INVALID_POLICY
//
// MessageText:
//
//  The certificate has invalid policy.
//
#define CERT_E_INVALID_POLICY            _HRESULT_TYPEDEF_(0x800B0113L)

//
// MessageId: CERT_E_INVALID_NAME
//
// MessageText:
//
//  The certificate has an invalid name. The name is not included in the permitted list or is explicitly excluded.
//
#define CERT_E_INVALID_NAME              _HRESULT_TYPEDEF_(0x800B0114L)

// *****************
// FACILITY_SETUPAPI
// *****************
//
// Since these error codes aren't in the standard Win32 range (i.e., 0-64K), define a
// macro to map either Win32 or SetupAPI error codes into an HRESULT.
//
#define HRESULT_FROM_SETUPAPI(x) ((((x) & (APPLICATION_ERROR_MASK|ERROR_SEVERITY_ERROR)) == (APPLICATION_ERROR_MASK|ERROR_SEVERITY_ERROR)) \
                                 ? ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_SETUPAPI << 16) | 0x80000000))                               \
                                 : HRESULT_FROM_WIN32(x))
//
// MessageId: SPAPI_E_EXPECTED_SECTION_NAME
//
// MessageText:
//
//  A non-empty line was encountered in the INF before the start of a section.
//
#define SPAPI_E_EXPECTED_SECTION_NAME    _HRESULT_TYPEDEF_(0x800F0000L)

//
// MessageId: SPAPI_E_BAD_SECTION_NAME_LINE
//
// MessageText:
//
//  A section name marker in the INF is not complete, or does not exist on a line by itself.
//
#define SPAPI_E_BAD_SECTION_NAME_LINE    _HRESULT_TYPEDEF_(0x800F0001L)

//
// MessageId: SPAPI_E_SECTION_NAME_TOO_LONG
//
// MessageText:
//
//  An INF section was encountered whose name exceeds the maximum section name length.
//
#define SPAPI_E_SECTION_NAME_TOO_LONG    _HRESULT_TYPEDEF_(0x800F0002L)

//
// MessageId: SPAPI_E_GENERAL_SYNTAX
//
// MessageText:
//
//  The syntax of the INF is invalid.
//
#define SPAPI_E_GENERAL_SYNTAX           _HRESULT_TYPEDEF_(0x800F0003L)

//
// MessageId: SPAPI_E_WRONG_INF_STYLE
//
// MessageText:
//
//  The style of the INF is different than what was requested.
//
#define SPAPI_E_WRONG_INF_STYLE          _HRESULT_TYPEDEF_(0x800F0100L)

//
// MessageId: SPAPI_E_SECTION_NOT_FOUND
//
// MessageText:
//
//  The required section was not found in the INF.
//
#define SPAPI_E_SECTION_NOT_FOUND        _HRESULT_TYPEDEF_(0x800F0101L)

//
// MessageId: SPAPI_E_LINE_NOT_FOUND
//
// MessageText:
//
//  The required line was not found in the INF.
//
#define SPAPI_E_LINE_NOT_FOUND           _HRESULT_TYPEDEF_(0x800F0102L)

//
// MessageId: SPAPI_E_NO_BACKUP
//
// MessageText:
//
//  The files affected by the installation of this file queue have not been backed up for uninstall.
//
#define SPAPI_E_NO_BACKUP                _HRESULT_TYPEDEF_(0x800F0103L)

//
// MessageId: SPAPI_E_NO_ASSOCIATED_CLASS
//
// MessageText:
//
//  The INF or the device information set or element does not have an associated install class.
//
#define SPAPI_E_NO_ASSOCIATED_CLASS      _HRESULT_TYPEDEF_(0x800F0200L)

//
// MessageId: SPAPI_E_CLASS_MISMATCH
//
// MessageText:
//
//  The INF or the device information set or element does not match the specified install class.
//
#define SPAPI_E_CLASS_MISMATCH           _HRESULT_TYPEDEF_(0x800F0201L)

//
// MessageId: SPAPI_E_DUPLICATE_FOUND
//
// MessageText:
//
//  An existing device was found that is a duplicate of the device being manually installed.
//
#define SPAPI_E_DUPLICATE_FOUND          _HRESULT_TYPEDEF_(0x800F0202L)

//
// MessageId: SPAPI_E_NO_DRIVER_SELECTED
//
// MessageText:
//
//  There is no driver selected for the device information set or element.
//
#define SPAPI_E_NO_DRIVER_SELECTED       _HRESULT_TYPEDEF_(0x800F0203L)

//
// MessageId: SPAPI_E_KEY_DOES_NOT_EXIST
//
// MessageText:
//
//  The requested device registry key does not exist.
//
#define SPAPI_E_KEY_DOES_NOT_EXIST       _HRESULT_TYPEDEF_(0x800F0204L)

//
// MessageId: SPAPI_E_INVALID_DEVINST_NAME
//
// MessageText:
//
//  The device instance name is invalid.
//
#define SPAPI_E_INVALID_DEVINST_NAME     _HRESULT_TYPEDEF_(0x800F0205L)

//
// MessageId: SPAPI_E_INVALID_CLASS
//
// MessageText:
//
//  The install class is not present or is invalid.
//
#define SPAPI_E_INVALID_CLASS            _HRESULT_TYPEDEF_(0x800F0206L)

//
// MessageId: SPAPI_E_DEVINST_ALREADY_EXISTS
//
// MessageText:
//
//  The device instance cannot be created because it already exists.
//
#define SPAPI_E_DEVINST_ALREADY_EXISTS   _HRESULT_TYPEDEF_(0x800F0207L)

//
// MessageId: SPAPI_E_DEVINFO_NOT_REGISTERED
//
// MessageText:
//
//  The operation cannot be performed on a device information element that has not been registered.
//
#define SPAPI_E_DEVINFO_NOT_REGISTERED   _HRESULT_TYPEDEF_(0x800F0208L)

//
// MessageId: SPAPI_E_INVALID_REG_PROPERTY
//
// MessageText:
//
//  The device property code is invalid.
//
#define SPAPI_E_INVALID_REG_PROPERTY     _HRESULT_TYPEDEF_(0x800F0209L)

//
// MessageId: SPAPI_E_NO_INF
//
// MessageText:
//
//  The INF from which a driver list is to be built does not exist.
//
#define SPAPI_E_NO_INF                   _HRESULT_TYPEDEF_(0x800F020AL)

//
// MessageId: SPAPI_E_NO_SUCH_DEVINST
//
// MessageText:
//
//  The device instance does not exist in the hardware tree.
//
#define SPAPI_E_NO_SUCH_DEVINST          _HRESULT_TYPEDEF_(0x800F020BL)

//
// MessageId: SPAPI_E_CANT_LOAD_CLASS_ICON
//
// MessageText:
//
//  The icon representing this install class cannot be loaded.
//
#define SPAPI_E_CANT_LOAD_CLASS_ICON     _HRESULT_TYPEDEF_(0x800F020CL)

//
// MessageId: SPAPI_E_INVALID_CLASS_INSTALLER
//
// MessageText:
//
//  The class installer registry entry is invalid.
//
#define SPAPI_E_INVALID_CLASS_INSTALLER  _HRESULT_TYPEDEF_(0x800F020DL)

//
// MessageId: SPAPI_E_DI_DO_DEFAULT
//
// MessageText:
//
//  The class installer has indicated that the default action should be performed for this installation request.
//
#define SPAPI_E_DI_DO_DEFAULT            _HRESULT_TYPEDEF_(0x800F020EL)

//
// MessageId: SPAPI_E_DI_NOFILECOPY
//
// MessageText:
//
//  The operation does not require any files to be copied.
//
#define SPAPI_E_DI_NOFILECOPY            _HRESULT_TYPEDEF_(0x800F020FL)

//
// MessageId: SPAPI_E_INVALID_HWPROFILE
//
// MessageText:
//
//  The specified hardware profile does not exist.
//
#define SPAPI_E_INVALID_HWPROFILE        _HRESULT_TYPEDEF_(0x800F0210L)

//
// MessageId: SPAPI_E_NO_DEVICE_SELECTED
//
// MessageText:
//
//  There is no device information element currently selected for this device information set.
//
#define SPAPI_E_NO_DEVICE_SELECTED       _HRESULT_TYPEDEF_(0x800F0211L)

//
// MessageId: SPAPI_E_DEVINFO_LIST_LOCKED
//
// MessageText:
//
//  The operation cannot be performed because the device information set is locked.
//
#define SPAPI_E_DEVINFO_LIST_LOCKED      _HRESULT_TYPEDEF_(0x800F0212L)

//
// MessageId: SPAPI_E_DEVINFO_DATA_LOCKED
//
// MessageText:
//
//  The operation cannot be performed because the device information element is locked.
//
#define SPAPI_E_DEVINFO_DATA_LOCKED      _HRESULT_TYPEDEF_(0x800F0213L)

//
// MessageId: SPAPI_E_DI_BAD_PATH
//
// MessageText:
//
//  The specified path does not contain any applicable device INFs.
//
#define SPAPI_E_DI_BAD_PATH              _HRESULT_TYPEDEF_(0x800F0214L)

//
// MessageId: SPAPI_E_NO_CLASSINSTALL_PARAMS
//
// MessageText:
//
//  No class installer parameters have been set for the device information set or element.
//
#define SPAPI_E_NO_CLASSINSTALL_PARAMS   _HRESULT_TYPEDEF_(0x800F0215L)

//
// MessageId: SPAPI_E_FILEQUEUE_LOCKED
//
// MessageText:
//
//  The operation cannot be performed because the file queue is locked.
//
#define SPAPI_E_FILEQUEUE_LOCKED         _HRESULT_TYPEDEF_(0x800F0216L)

//
// MessageId: SPAPI_E_BAD_SERVICE_INSTALLSECT
//
// MessageText:
//
//  A service installation section in this INF is invalid.
//
#define SPAPI_E_BAD_SERVICE_INSTALLSECT  _HRESULT_TYPEDEF_(0x800F0217L)

//
// MessageId: SPAPI_E_NO_CLASS_DRIVER_LIST
//
// MessageText:
//
//  There is no class driver list for the device information element.
//
#define SPAPI_E_NO_CLASS_DRIVER_LIST     _HRESULT_TYPEDEF_(0x800F0218L)

//
// MessageId: SPAPI_E_NO_ASSOCIATED_SERVICE
//
// MessageText:
//
//  The installation failed because a function driver was not specified for this device instance.
//
#define SPAPI_E_NO_ASSOCIATED_SERVICE    _HRESULT_TYPEDEF_(0x800F0219L)

//
// MessageId: SPAPI_E_NO_DEFAULT_DEVICE_INTERFACE
//
// MessageText:
//
//  There is presently no default device interface designated for this interface class.
//
#define SPAPI_E_NO_DEFAULT_DEVICE_INTERFACE _HRESULT_TYPEDEF_(0x800F021AL)

//
// MessageId: SPAPI_E_DEVICE_INTERFACE_ACTIVE
//
// MessageText:
//
//  The operation cannot be performed because the device interface is currently active.
//
#define SPAPI_E_DEVICE_INTERFACE_ACTIVE  _HRESULT_TYPEDEF_(0x800F021BL)

//
// MessageId: SPAPI_E_DEVICE_INTERFACE_REMOVED
//
// MessageText:
//
//  The operation cannot be performed because the device interface has been removed from the system.
//
#define SPAPI_E_DEVICE_INTERFACE_REMOVED _HRESULT_TYPEDEF_(0x800F021CL)

//
// MessageId: SPAPI_E_BAD_INTERFACE_INSTALLSECT
//
// MessageText:
//
//  An interface installation section in this INF is invalid.
//
#define SPAPI_E_BAD_INTERFACE_INSTALLSECT _HRESULT_TYPEDEF_(0x800F021DL)

//
// MessageId: SPAPI_E_NO_SUCH_INTERFACE_CLASS
//
// MessageText:
//
//  This interface class does not exist in the system.
//
#define SPAPI_E_NO_SUCH_INTERFACE_CLASS  _HRESULT_TYPEDEF_(0x800F021EL)

//
// MessageId: SPAPI_E_INVALID_REFERENCE_STRING
//
// MessageText:
//
//  The reference string supplied for this interface device is invalid.
//
#define SPAPI_E_INVALID_REFERENCE_STRING _HRESULT_TYPEDEF_(0x800F021FL)

//
// MessageId: SPAPI_E_INVALID_MACHINENAME
//
// MessageText:
//
//  The specified machine name does not conform to UNC naming conventions.
//
#define SPAPI_E_INVALID_MACHINENAME      _HRESULT_TYPEDEF_(0x800F0220L)

//
// MessageId: SPAPI_E_REMOTE_COMM_FAILURE
//
// MessageText:
//
//  A general remote communication error occurred.
//
#define SPAPI_E_REMOTE_COMM_FAILURE      _HRESULT_TYPEDEF_(0x800F0221L)

//
// MessageId: SPAPI_E_MACHINE_UNAVAILABLE
//
// MessageText:
//
//  The machine selected for remote communication is not available at this time.
//
#define SPAPI_E_MACHINE_UNAVAILABLE      _HRESULT_TYPEDEF_(0x800F0222L)

//
// MessageId: SPAPI_E_NO_CONFIGMGR_SERVICES
//
// MessageText:
//
//  The Plug and Play service is not available on the remote machine.
//
#define SPAPI_E_NO_CONFIGMGR_SERVICES    _HRESULT_TYPEDEF_(0x800F0223L)

//
// MessageId: SPAPI_E_INVALID_PROPPAGE_PROVIDER
//
// MessageText:
//
//  The property page provider registry entry is invalid.
//
#define SPAPI_E_INVALID_PROPPAGE_PROVIDER _HRESULT_TYPEDEF_(0x800F0224L)

//
// MessageId: SPAPI_E_NO_SUCH_DEVICE_INTERFACE
//
// MessageText:
//
//  The requested device interface is not present in the system.
//
#define SPAPI_E_NO_SUCH_DEVICE_INTERFACE _HRESULT_TYPEDEF_(0x800F0225L)

//
// MessageId: SPAPI_E_DI_POSTPROCESSING_REQUIRED
//
// MessageText:
//
//  The device's co-installer has additional work to perform after installation is complete.
//
#define SPAPI_E_DI_POSTPROCESSING_REQUIRED _HRESULT_TYPEDEF_(0x800F0226L)

//
// MessageId: SPAPI_E_INVALID_COINSTALLER
//
// MessageText:
//
//  The device's co-installer is invalid.
//
#define SPAPI_E_INVALID_COINSTALLER      _HRESULT_TYPEDEF_(0x800F0227L)

//
// MessageId: SPAPI_E_NO_COMPAT_DRIVERS
//
// MessageText:
//
//  There are no compatible drivers for this device.
//
#define SPAPI_E_NO_COMPAT_DRIVERS        _HRESULT_TYPEDEF_(0x800F0228L)

//
// MessageId: SPAPI_E_NO_DEVICE_ICON
//
// MessageText:
//
//  There is no icon that represents this device or device type.
//
#define SPAPI_E_NO_DEVICE_ICON           _HRESULT_TYPEDEF_(0x800F0229L)

//
// MessageId: SPAPI_E_INVALID_INF_LOGCONFIG
//
// MessageText:
//
//  A logical configuration specified in this INF is invalid.
//
#define SPAPI_E_INVALID_INF_LOGCONFIG    _HRESULT_TYPEDEF_(0x800F022AL)

//
// MessageId: SPAPI_E_DI_DONT_INSTALL
//
// MessageText:
//
//  The class installer has denied the request to install or upgrade this device.
//
#define SPAPI_E_DI_DONT_INSTALL          _HRESULT_TYPEDEF_(0x800F022BL)

//
// MessageId: SPAPI_E_INVALID_FILTER_DRIVER
//
// MessageText:
//
//  One of the filter drivers installed for this device is invalid.
//
#define SPAPI_E_INVALID_FILTER_DRIVER    _HRESULT_TYPEDEF_(0x800F022CL)

//
// MessageId: SPAPI_E_NON_WINDOWS_NT_DRIVER
//
// MessageText:
//
//  The driver selected for this device does not support Windows XP.
//
#define SPAPI_E_NON_WINDOWS_NT_DRIVER    _HRESULT_TYPEDEF_(0x800F022DL)

//
// MessageId: SPAPI_E_NON_WINDOWS_DRIVER
//
// MessageText:
//
//  The driver selected for this device does not support Windows.
//
#define SPAPI_E_NON_WINDOWS_DRIVER       _HRESULT_TYPEDEF_(0x800F022EL)

//
// MessageId: SPAPI_E_NO_CATALOG_FOR_OEM_INF
//
// MessageText:
//
//  The third-party INF does not contain digital signature information.
//
#define SPAPI_E_NO_CATALOG_FOR_OEM_INF   _HRESULT_TYPEDEF_(0x800F022FL)

//
// MessageId: SPAPI_E_DEVINSTALL_QUEUE_NONNATIVE
//
// MessageText:
//
//  An invalid attempt was made to use a device installation file queue for verification of digital signatures relative to other platforms.
//
#define SPAPI_E_DEVINSTALL_QUEUE_NONNATIVE _HRESULT_TYPEDEF_(0x800F0230L)

//
// MessageId: SPAPI_E_NOT_DISABLEABLE
//
// MessageText:
//
//  The device cannot be disabled.
//
#define SPAPI_E_NOT_DISABLEABLE          _HRESULT_TYPEDEF_(0x800F0231L)

//
// MessageId: SPAPI_E_CANT_REMOVE_DEVINST
//
// MessageText:
//
//  The device could not be dynamically removed.
//
#define SPAPI_E_CANT_REMOVE_DEVINST      _HRESULT_TYPEDEF_(0x800F0232L)

//
// MessageId: SPAPI_E_INVALID_TARGET
//
// MessageText:
//
//  Cannot copy to specified target.
//
#define SPAPI_E_INVALID_TARGET           _HRESULT_TYPEDEF_(0x800F0233L)

//
// MessageId: SPAPI_E_DRIVER_NONNATIVE
//
// MessageText:
//
//  Driver is not intended for this platform.
//
#define SPAPI_E_DRIVER_NONNATIVE         _HRESULT_TYPEDEF_(0x800F0234L)

//
// MessageId: SPAPI_E_IN_WOW64
//
// MessageText:
//
//  Operation not allowed in WOW64.
//
#define SPAPI_E_IN_WOW64                 _HRESULT_TYPEDEF_(0x800F0235L)

//
// MessageId: SPAPI_E_SET_SYSTEM_RESTORE_POINT
//
// MessageText:
//
//  The operation involving unsigned file copying was rolled back, so that a system restore point could be set.
//
#define SPAPI_E_SET_SYSTEM_RESTORE_POINT _HRESULT_TYPEDEF_(0x800F0236L)

//
// MessageId: SPAPI_E_INCORRECTLY_COPIED_INF
//
// MessageText:
//
//  An INF was copied into the Windows INF directory in an improper manner.
//
#define SPAPI_E_INCORRECTLY_COPIED_INF   _HRESULT_TYPEDEF_(0x800F0237L)

//
// MessageId: SPAPI_E_SCE_DISABLED
//
// MessageText:
//
//  The Security Configuration Editor (SCE) APIs have been disabled on this Embedded product.
//
#define SPAPI_E_SCE_DISABLED             _HRESULT_TYPEDEF_(0x800F0238L)

//
// MessageId: SPAPI_E_UNKNOWN_EXCEPTION
//
// MessageText:
//
//  An unknown exception was encountered.
//
#define SPAPI_E_UNKNOWN_EXCEPTION        _HRESULT_TYPEDEF_(0x800F0239L)

//
// MessageId: SPAPI_E_PNP_REGISTRY_ERROR
//
// MessageText:
//
//  A problem was encountered when accessing the Plug and Play registry database.
//
#define SPAPI_E_PNP_REGISTRY_ERROR       _HRESULT_TYPEDEF_(0x800F023AL)

//
// MessageId: SPAPI_E_REMOTE_REQUEST_UNSUPPORTED
//
// MessageText:
//
//  The requested operation is not supported for a remote machine.
//
#define SPAPI_E_REMOTE_REQUEST_UNSUPPORTED _HRESULT_TYPEDEF_(0x800F023BL)

//
// MessageId: SPAPI_E_NOT_AN_INSTALLED_OEM_INF
//
// MessageText:
//
//  The specified file is not an installed OEM INF.
//
#define SPAPI_E_NOT_AN_INSTALLED_OEM_INF _HRESULT_TYPEDEF_(0x800F023CL)

//
// MessageId: SPAPI_E_INF_IN_USE_BY_DEVICES
//
// MessageText:
//
//  One or more devices are presently installed using the specified INF.
//
#define SPAPI_E_INF_IN_USE_BY_DEVICES    _HRESULT_TYPEDEF_(0x800F023DL)

//
// MessageId: SPAPI_E_DI_FUNCTION_OBSOLETE
//
// MessageText:
//
//  The requested device install operation is obsolete.
//
#define SPAPI_E_DI_FUNCTION_OBSOLETE     _HRESULT_TYPEDEF_(0x800F023EL)

//
// MessageId: SPAPI_E_NO_AUTHENTICODE_CATALOG
//
// MessageText:
//
//  A file could not be verified because it does not have an associated catalog signed via Authenticode(tm).
//
#define SPAPI_E_NO_AUTHENTICODE_CATALOG  _HRESULT_TYPEDEF_(0x800F023FL)

//
// MessageId: SPAPI_E_AUTHENTICODE_DISALLOWED
//
// MessageText:
//
//  Authenticode(tm) signature verification is not supported for the specified INF.
//
#define SPAPI_E_AUTHENTICODE_DISALLOWED  _HRESULT_TYPEDEF_(0x800F0240L)

//
// MessageId: SPAPI_E_AUTHENTICODE_TRUSTED_PUBLISHER
//
// MessageText:
//
//  The INF was signed with an Authenticode(tm) catalog from a trusted publisher.
//
#define SPAPI_E_AUTHENTICODE_TRUSTED_PUBLISHER _HRESULT_TYPEDEF_(0x800F0241L)

//
// MessageId: SPAPI_E_AUTHENTICODE_TRUST_NOT_ESTABLISHED
//
// MessageText:
//
//  The publisher of an Authenticode(tm) signed catalog has not yet been established as trusted.
//
#define SPAPI_E_AUTHENTICODE_TRUST_NOT_ESTABLISHED _HRESULT_TYPEDEF_(0x800F0242L)

//
// MessageId: SPAPI_E_AUTHENTICODE_PUBLISHER_NOT_TRUSTED
//
// MessageText:
//
//  The publisher of an Authenticode(tm) signed catalog was not established as trusted.
//
#define SPAPI_E_AUTHENTICODE_PUBLISHER_NOT_TRUSTED _HRESULT_TYPEDEF_(0x800F0243L)

//
// MessageId: SPAPI_E_SIGNATURE_OSATTRIBUTE_MISMATCH
//
// MessageText:
//
//  The software was tested for compliance with Windows Logo requirements on a different version of Windows, and may not be compatible with this version.
//
#define SPAPI_E_SIGNATURE_OSATTRIBUTE_MISMATCH _HRESULT_TYPEDEF_(0x800F0244L)

//
// MessageId: SPAPI_E_ONLY_VALIDATE_VIA_AUTHENTICODE
//
// MessageText:
//
//  The file may only be validated by a catalog signed via Authenticode(tm).
//
#define SPAPI_E_ONLY_VALIDATE_VIA_AUTHENTICODE _HRESULT_TYPEDEF_(0x800F0245L)

//
// MessageId: SPAPI_E_UNRECOVERABLE_STACK_OVERFLOW
//
// MessageText:
//
//  An unrecoverable stack overflow was encountered.
//
#define SPAPI_E_UNRECOVERABLE_STACK_OVERFLOW _HRESULT_TYPEDEF_(0x800F0300L)

//
// MessageId: SPAPI_E_ERROR_NOT_INSTALLED
//
// MessageText:
//
//  No installed components were detected.
//
#define SPAPI_E_ERROR_NOT_INSTALLED      _HRESULT_TYPEDEF_(0x800F1000L)

// *****************
// FACILITY_SCARD
// *****************
//
// =============================
// Facility SCARD Error Messages
// =============================
//
#define SCARD_S_SUCCESS NO_ERROR
//
// MessageId: SCARD_F_INTERNAL_ERROR
//
// MessageText:
//
//  An internal consistency check failed.
//
#define SCARD_F_INTERNAL_ERROR           _HRESULT_TYPEDEF_(0x80100001L)

//
// MessageId: SCARD_E_CANCELLED
//
// MessageText:
//
//  The action was cancelled by an SCardCancel request.
//
#define SCARD_E_CANCELLED                _HRESULT_TYPEDEF_(0x80100002L)

//
// MessageId: SCARD_E_INVALID_HANDLE
//
// MessageText:
//
//  The supplied handle was invalid.
//
#define SCARD_E_INVALID_HANDLE           _HRESULT_TYPEDEF_(0x80100003L)

//
// MessageId: SCARD_E_INVALID_PARAMETER
//
// MessageText:
//
//  One or more of the supplied parameters could not be properly interpreted.
//
#define SCARD_E_INVALID_PARAMETER        _HRESULT_TYPEDEF_(0x80100004L)

//
// MessageId: SCARD_E_INVALID_TARGET
//
// MessageText:
//
//  Registry startup information is missing or invalid.
//
#define SCARD_E_INVALID_TARGET           _HRESULT_TYPEDEF_(0x80100005L)

//
// MessageId: SCARD_E_NO_MEMORY
//
// MessageText:
//
//  Not enough memory available to complete this command.
//
#define SCARD_E_NO_MEMORY                _HRESULT_TYPEDEF_(0x80100006L)

//
// MessageId: SCARD_F_WAITED_TOO_LONG
//
// MessageText:
//
//  An internal consistency timer has expired.
//
#define SCARD_F_WAITED_TOO_LONG          _HRESULT_TYPEDEF_(0x80100007L)

//
// MessageId: SCARD_E_INSUFFICIENT_BUFFER
//
// MessageText:
//
//  The data buffer to receive returned data is too small for the returned data.
//
#define SCARD_E_INSUFFICIENT_BUFFER      _HRESULT_TYPEDEF_(0x80100008L)

//
// MessageId: SCARD_E_UNKNOWN_READER
//
// MessageText:
//
//  The specified reader name is not recognized.
//
#define SCARD_E_UNKNOWN_READER           _HRESULT_TYPEDEF_(0x80100009L)

//
// MessageId: SCARD_E_TIMEOUT
//
// MessageText:
//
//  The user-specified timeout value has expired.
//
#define SCARD_E_TIMEOUT                  _HRESULT_TYPEDEF_(0x8010000AL)

//
// MessageId: SCARD_E_SHARING_VIOLATION
//
// MessageText:
//
//  The smart card cannot be accessed because of other connections outstanding.
//
#define SCARD_E_SHARING_VIOLATION        _HRESULT_TYPEDEF_(0x8010000BL)

//
// MessageId: SCARD_E_NO_SMARTCARD
//
// MessageText:
//
//  The operation requires a Smart Card, but no Smart Card is currently in the device.
//
#define SCARD_E_NO_SMARTCARD             _HRESULT_TYPEDEF_(0x8010000CL)

//
// MessageId: SCARD_E_UNKNOWN_CARD
//
// MessageText:
//
//  The specified smart card name is not recognized.
//
#define SCARD_E_UNKNOWN_CARD             _HRESULT_TYPEDEF_(0x8010000DL)

//
// MessageId: SCARD_E_CANT_DISPOSE
//
// MessageText:
//
//  The system could not dispose of the media in the requested manner.
//
#define SCARD_E_CANT_DISPOSE             _HRESULT_TYPEDEF_(0x8010000EL)

//
// MessageId: SCARD_E_PROTO_MISMATCH
//
// MessageText:
//
//  The requested protocols are incompatible with the protocol currently in use with the smart card.
//
#define SCARD_E_PROTO_MISMATCH           _HRESULT_TYPEDEF_(0x8010000FL)

//
// MessageId: SCARD_E_NOT_READY
//
// MessageText:
//
//  The reader or smart card is not ready to accept commands.
//
#define SCARD_E_NOT_READY                _HRESULT_TYPEDEF_(0x80100010L)

//
// MessageId: SCARD_E_INVALID_VALUE
//
// MessageText:
//
//  One or more of the supplied parameters values could not be properly interpreted.
//
#define SCARD_E_INVALID_VALUE            _HRESULT_TYPEDEF_(0x80100011L)

//
// MessageId: SCARD_E_SYSTEM_CANCELLED
//
// MessageText:
//
//  The action was cancelled by the system, presumably to log off or shut down.
//
#define SCARD_E_SYSTEM_CANCELLED         _HRESULT_TYPEDEF_(0x80100012L)

//
// MessageId: SCARD_F_COMM_ERROR
//
// MessageText:
//
//  An internal communications error has been detected.
//
#define SCARD_F_COMM_ERROR               _HRESULT_TYPEDEF_(0x80100013L)

//
// MessageId: SCARD_F_UNKNOWN_ERROR
//
// MessageText:
//
//  An internal error has been detected, but the source is unknown.
//
#define SCARD_F_UNKNOWN_ERROR            _HRESULT_TYPEDEF_(0x80100014L)

//
// MessageId: SCARD_E_INVALID_ATR
//
// MessageText:
//
//  An ATR obtained from the registry is not a valid ATR string.
//
#define SCARD_E_INVALID_ATR              _HRESULT_TYPEDEF_(0x80100015L)

//
// MessageId: SCARD_E_NOT_TRANSACTED
//
// MessageText:
//
//  An attempt was made to end a non-existent transaction.
//
#define SCARD_E_NOT_TRANSACTED           _HRESULT_TYPEDEF_(0x80100016L)

//
// MessageId: SCARD_E_READER_UNAVAILABLE
//
// MessageText:
//
//  The specified reader is not currently available for use.
//
#define SCARD_E_READER_UNAVAILABLE       _HRESULT_TYPEDEF_(0x80100017L)

//
// MessageId: SCARD_P_SHUTDOWN
//
// MessageText:
//
//  The operation has been aborted to allow the server application to exit.
//
#define SCARD_P_SHUTDOWN                 _HRESULT_TYPEDEF_(0x80100018L)

//
// MessageId: SCARD_E_PCI_TOO_SMALL
//
// MessageText:
//
//  The PCI Receive buffer was too small.
//
#define SCARD_E_PCI_TOO_SMALL            _HRESULT_TYPEDEF_(0x80100019L)

//
// MessageId: SCARD_E_READER_UNSUPPORTED
//
// MessageText:
//
//  The reader driver does not meet minimal requirements for support.
//
#define SCARD_E_READER_UNSUPPORTED       _HRESULT_TYPEDEF_(0x8010001AL)

//
// MessageId: SCARD_E_DUPLICATE_READER
//
// MessageText:
//
//  The reader driver did not produce a unique reader name.
//
#define SCARD_E_DUPLICATE_READER         _HRESULT_TYPEDEF_(0x8010001BL)

//
// MessageId: SCARD_E_CARD_UNSUPPORTED
//
// MessageText:
//
//  The smart card does not meet minimal requirements for support.
//
#define SCARD_E_CARD_UNSUPPORTED         _HRESULT_TYPEDEF_(0x8010001CL)

//
// MessageId: SCARD_E_NO_SERVICE
//
// MessageText:
//
//  The Smart card resource manager is not running.
//
#define SCARD_E_NO_SERVICE               _HRESULT_TYPEDEF_(0x8010001DL)

//
// MessageId: SCARD_E_SERVICE_STOPPED
//
// MessageText:
//
//  The Smart card resource manager has shut down.
//
#define SCARD_E_SERVICE_STOPPED          _HRESULT_TYPEDEF_(0x8010001EL)

//
// MessageId: SCARD_E_UNEXPECTED
//
// MessageText:
//
//  An unexpected card error has occurred.
//
#define SCARD_E_UNEXPECTED               _HRESULT_TYPEDEF_(0x8010001FL)

//
// MessageId: SCARD_E_ICC_INSTALLATION
//
// MessageText:
//
//  No Primary Provider can be found for the smart card.
//
#define SCARD_E_ICC_INSTALLATION         _HRESULT_TYPEDEF_(0x80100020L)

//
// MessageId: SCARD_E_ICC_CREATEORDER
//
// MessageText:
//
//  The requested order of object creation is not supported.
//
#define SCARD_E_ICC_CREATEORDER          _HRESULT_TYPEDEF_(0x80100021L)

//
// MessageId: SCARD_E_UNSUPPORTED_FEATURE
//
// MessageText:
//
//  This smart card does not support the requested feature.
//
#define SCARD_E_UNSUPPORTED_FEATURE      _HRESULT_TYPEDEF_(0x80100022L)

//
// MessageId: SCARD_E_DIR_NOT_FOUND
//
// MessageText:
//
//  The identified directory does not exist in the smart card.
//
#define SCARD_E_DIR_NOT_FOUND            _HRESULT_TYPEDEF_(0x80100023L)

//
// MessageId: SCARD_E_FILE_NOT_FOUND
//
// MessageText:
//
//  The identified file does not exist in the smart card.
//
#define SCARD_E_FILE_NOT_FOUND           _HRESULT_TYPEDEF_(0x80100024L)

//
// MessageId: SCARD_E_NO_DIR
//
// MessageText:
//
//  The supplied path does not represent a smart card directory.
//
#define SCARD_E_NO_DIR                   _HRESULT_TYPEDEF_(0x80100025L)

//
// MessageId: SCARD_E_NO_FILE
//
// MessageText:
//
//  The supplied path does not represent a smart card file.
//
#define SCARD_E_NO_FILE                  _HRESULT_TYPEDEF_(0x80100026L)

//
// MessageId: SCARD_E_NO_ACCESS
//
// MessageText:
//
//  Access is denied to this file.
//
#define SCARD_E_NO_ACCESS                _HRESULT_TYPEDEF_(0x80100027L)

//
// MessageId: SCARD_E_WRITE_TOO_MANY
//
// MessageText:
//
//  The smartcard does not have enough memory to store the information.
//
#define SCARD_E_WRITE_TOO_MANY           _HRESULT_TYPEDEF_(0x80100028L)

//
// MessageId: SCARD_E_BAD_SEEK
//
// MessageText:
//
//  There was an error trying to set the smart card file object pointer.
//
#define SCARD_E_BAD_SEEK                 _HRESULT_TYPEDEF_(0x80100029L)

//
// MessageId: SCARD_E_INVALID_CHV
//
// MessageText:
//
//  The supplied PIN is incorrect.
//
#define SCARD_E_INVALID_CHV              _HRESULT_TYPEDEF_(0x8010002AL)

//
// MessageId: SCARD_E_UNKNOWN_RES_MNG
//
// MessageText:
//
//  An unrecognized error code was returned from a layered component.
//
#define SCARD_E_UNKNOWN_RES_MNG          _HRESULT_TYPEDEF_(0x8010002BL)

//
// MessageId: SCARD_E_NO_SUCH_CERTIFICATE
//
// MessageText:
//
//  The requested certificate does not exist.
//
#define SCARD_E_NO_SUCH_CERTIFICATE      _HRESULT_TYPEDEF_(0x8010002CL)

//
// MessageId: SCARD_E_CERTIFICATE_UNAVAILABLE
//
// MessageText:
//
//  The requested certificate could not be obtained.
//
#define SCARD_E_CERTIFICATE_UNAVAILABLE  _HRESULT_TYPEDEF_(0x8010002DL)

//
// MessageId: SCARD_E_NO_READERS_AVAILABLE
//
// MessageText:
//
//  Cannot find a smart card reader.
//
#define SCARD_E_NO_READERS_AVAILABLE     _HRESULT_TYPEDEF_(0x8010002EL)

//
// MessageId: SCARD_E_COMM_DATA_LOST
//
// MessageText:
//
//  A communications error with the smart card has been detected.  Retry the operation.
//
#define SCARD_E_COMM_DATA_LOST           _HRESULT_TYPEDEF_(0x8010002FL)

//
// MessageId: SCARD_E_NO_KEY_CONTAINER
//
// MessageText:
//
//  The requested key container does not exist on the smart card.
//
#define SCARD_E_NO_KEY_CONTAINER         _HRESULT_TYPEDEF_(0x80100030L)

//
// MessageId: SCARD_E_SERVER_TOO_BUSY
//
// MessageText:
//
//  The Smart card resource manager is too busy to complete this operation.
//
#define SCARD_E_SERVER_TOO_BUSY          _HRESULT_TYPEDEF_(0x80100031L)

//
// These are warning codes.
//
//
// MessageId: SCARD_W_UNSUPPORTED_CARD
//
// MessageText:
//
//  The reader cannot communicate with the smart card, due to ATR configuration conflicts.
//
#define SCARD_W_UNSUPPORTED_CARD         _HRESULT_TYPEDEF_(0x80100065L)

//
// MessageId: SCARD_W_UNRESPONSIVE_CARD
//
// MessageText:
//
//  The smart card is not responding to a reset.
//
#define SCARD_W_UNRESPONSIVE_CARD        _HRESULT_TYPEDEF_(0x80100066L)

//
// MessageId: SCARD_W_UNPOWERED_CARD
//
// MessageText:
//
//  Power has been removed from the smart card, so that further communication is not possible.
//
#define SCARD_W_UNPOWERED_CARD           _HRESULT_TYPEDEF_(0x80100067L)

//
// MessageId: SCARD_W_RESET_CARD
//
// MessageText:
//
//  The smart card has been reset, so any shared state information is invalid.
//
#define SCARD_W_RESET_CARD               _HRESULT_TYPEDEF_(0x80100068L)

//
// MessageId: SCARD_W_REMOVED_CARD
//
// MessageText:
//
//  The smart card has been removed, so that further communication is not possible.
//
#define SCARD_W_REMOVED_CARD             _HRESULT_TYPEDEF_(0x80100069L)

//
// MessageId: SCARD_W_SECURITY_VIOLATION
//
// MessageText:
//
//  Access was denied because of a security violation.
//
#define SCARD_W_SECURITY_VIOLATION       _HRESULT_TYPEDEF_(0x8010006AL)

//
// MessageId: SCARD_W_WRONG_CHV
//
// MessageText:
//
//  The card cannot be accessed because the wrong PIN was presented.
//
#define SCARD_W_WRONG_CHV                _HRESULT_TYPEDEF_(0x8010006BL)

//
// MessageId: SCARD_W_CHV_BLOCKED
//
// MessageText:
//
//  The card cannot be accessed because the maximum number of PIN entry attempts has been reached.
//
#define SCARD_W_CHV_BLOCKED              _HRESULT_TYPEDEF_(0x8010006CL)

//
// MessageId: SCARD_W_EOF
//
// MessageText:
//
//  The end of the smart card file has been reached.
//
#define SCARD_W_EOF                      _HRESULT_TYPEDEF_(0x8010006DL)

//
// MessageId: SCARD_W_CANCELLED_BY_USER
//
// MessageText:
//
//  The action was cancelled by the user.
//
#define SCARD_W_CANCELLED_BY_USER        _HRESULT_TYPEDEF_(0x8010006EL)

//
// MessageId: SCARD_W_CARD_NOT_AUTHENTICATED
//
// MessageText:
//
//  No PIN was presented to the smart card.
//
#define SCARD_W_CARD_NOT_AUTHENTICATED   _HRESULT_TYPEDEF_(0x8010006FL)

// *****************
// FACILITY_COMPLUS
// *****************
//
// ===============================
// Facility COMPLUS Error Messages
// ===============================
//
//
// The following are the subranges  within the COMPLUS facility
// 0x400 - 0x4ff               COMADMIN_E_CAT
// 0x600 - 0x6ff               COMQC errors
// 0x700 - 0x7ff               MSDTC errors
// 0x800 - 0x8ff               Other COMADMIN errors
//
// COMPLUS Admin errors
//
//
// MessageId: COMADMIN_E_OBJECTERRORS
//
// MessageText:
//
//  Errors occurred accessing one or more objects - the ErrorInfo collection may have more detail
//
#define COMADMIN_E_OBJECTERRORS          _HRESULT_TYPEDEF_(0x80110401L)

//
// MessageId: COMADMIN_E_OBJECTINVALID
//
// MessageText:
//
//  One or more of the object's properties are missing or invalid
//
#define COMADMIN_E_OBJECTINVALID         _HRESULT_TYPEDEF_(0x80110402L)

//
// MessageId: COMADMIN_E_KEYMISSING
//
// MessageText:
//
//  The object was not found in the catalog
//
#define COMADMIN_E_KEYMISSING            _HRESULT_TYPEDEF_(0x80110403L)

//
// MessageId: COMADMIN_E_ALREADYINSTALLED
//
// MessageText:
//
//  The object is already registered
//
#define COMADMIN_E_ALREADYINSTALLED      _HRESULT_TYPEDEF_(0x80110404L)

//
// MessageId: COMADMIN_E_APP_FILE_WRITEFAIL
//
// MessageText:
//
//  Error occurred writing to the application file
//
#define COMADMIN_E_APP_FILE_WRITEFAIL    _HRESULT_TYPEDEF_(0x80110407L)

//
// MessageId: COMADMIN_E_APP_FILE_READFAIL
//
// MessageText:
//
//  Error occurred reading the application file
//
#define COMADMIN_E_APP_FILE_READFAIL     _HRESULT_TYPEDEF_(0x80110408L)

//
// MessageId: COMADMIN_E_APP_FILE_VERSION
//
// MessageText:
//
//  Invalid version number in application file
//
#define COMADMIN_E_APP_FILE_VERSION      _HRESULT_TYPEDEF_(0x80110409L)

//
// MessageId: COMADMIN_E_BADPATH
//
// MessageText:
//
//  The file path is invalid
//
#define COMADMIN_E_BADPATH               _HRESULT_TYPEDEF_(0x8011040AL)

//
// MessageId: COMADMIN_E_APPLICATIONEXISTS
//
// MessageText:
//
//  The application is already installed
//
#define COMADMIN_E_APPLICATIONEXISTS     _HRESULT_TYPEDEF_(0x8011040BL)

//
// MessageId: COMADMIN_E_ROLEEXISTS
//
// MessageText:
//
//  The role already exists
//
#define COMADMIN_E_ROLEEXISTS            _HRESULT_TYPEDEF_(0x8011040CL)

//
// MessageId: COMADMIN_E_CANTCOPYFILE
//
// MessageText:
//
//  An error occurred copying the file
//
#define COMADMIN_E_CANTCOPYFILE          _HRESULT_TYPEDEF_(0x8011040DL)

//
// MessageId: COMADMIN_E_NOUSER
//
// MessageText:
//
//  One or more users are not valid
//
#define COMADMIN_E_NOUSER                _HRESULT_TYPEDEF_(0x8011040FL)

//
// MessageId: COMADMIN_E_INVALIDUSERIDS
//
// MessageText:
//
//  One or more users in the application file are not valid
//
#define COMADMIN_E_INVALIDUSERIDS        _HRESULT_TYPEDEF_(0x80110410L)

//
// MessageId: COMADMIN_E_NOREGISTRYCLSID
//
// MessageText:
//
//  The component's CLSID is missing or corrupt
//
#define COMADMIN_E_NOREGISTRYCLSID       _HRESULT_TYPEDEF_(0x80110411L)

//
// MessageId: COMADMIN_E_BADREGISTRYPROGID
//
// MessageText:
//
//  The component's progID is missing or corrupt
//
#define COMADMIN_E_BADREGISTRYPROGID     _HRESULT_TYPEDEF_(0x80110412L)

//
// MessageId: COMADMIN_E_AUTHENTICATIONLEVEL
//
// MessageText:
//
//  Unable to set required authentication level for update request
//
#define COMADMIN_E_AUTHENTICATIONLEVEL   _HRESULT_TYPEDEF_(0x80110413L)

//
// MessageId: COMADMIN_E_USERPASSWDNOTVALID
//
// MessageText:
//
//  The identity or password set on the application is not valid
//
#define COMADMIN_E_USERPASSWDNOTVALID    _HRESULT_TYPEDEF_(0x80110414L)

//
// MessageId: COMADMIN_E_CLSIDORIIDMISMATCH
//
// MessageText:
//
//  Application file CLSIDs or IIDs do not match corresponding DLLs
//
#define COMADMIN_E_CLSIDORIIDMISMATCH    _HRESULT_TYPEDEF_(0x80110418L)

//
// MessageId: COMADMIN_E_REMOTEINTERFACE
//
// MessageText:
//
//  Interface information is either missing or changed
//
#define COMADMIN_E_REMOTEINTERFACE       _HRESULT_TYPEDEF_(0x80110419L)

//
// MessageId: COMADMIN_E_DLLREGISTERSERVER
//
// MessageText:
//
//  DllRegisterServer failed on component install
//
#define COMADMIN_E_DLLREGISTERSERVER     _HRESULT_TYPEDEF_(0x8011041AL)

//
// MessageId: COMADMIN_E_NOSERVERSHARE
//
// MessageText:
//
//  No server file share available
//
#define COMADMIN_E_NOSERVERSHARE         _HRESULT_TYPEDEF_(0x8011041BL)

//
// MessageId: COMADMIN_E_DLLLOADFAILED
//
// MessageText:
//
//  DLL could not be loaded
//
#define COMADMIN_E_DLLLOADFAILED         _HRESULT_TYPEDEF_(0x8011041DL)

//
// MessageId: COMADMIN_E_BADREGISTRYLIBID
//
// MessageText:
//
//  The registered TypeLib ID is not valid
//
#define COMADMIN_E_BADREGISTRYLIBID      _HRESULT_TYPEDEF_(0x8011041EL)

//
// MessageId: COMADMIN_E_APPDIRNOTFOUND
//
// MessageText:
//
//  Application install directory not found
//
#define COMADMIN_E_APPDIRNOTFOUND        _HRESULT_TYPEDEF_(0x8011041FL)

//
// MessageId: COMADMIN_E_REGISTRARFAILED
//
// MessageText:
//
//  Errors occurred while in the component registrar
//
#define COMADMIN_E_REGISTRARFAILED       _HRESULT_TYPEDEF_(0x80110423L)

//
// MessageId: COMADMIN_E_COMPFILE_DOESNOTEXIST
//
// MessageText:
//
//  The file does not exist
//
#define COMADMIN_E_COMPFILE_DOESNOTEXIST _HRESULT_TYPEDEF_(0x80110424L)

//
// MessageId: COMADMIN_E_COMPFILE_LOADDLLFAIL
//
// MessageText:
//
//  The DLL could not be loaded
//
#define COMADMIN_E_COMPFILE_LOADDLLFAIL  _HRESULT_TYPEDEF_(0x80110425L)

//
// MessageId: COMADMIN_E_COMPFILE_GETCLASSOBJ
//
// MessageText:
//
//  GetClassObject failed in the DLL
//
#define COMADMIN_E_COMPFILE_GETCLASSOBJ  _HRESULT_TYPEDEF_(0x80110426L)

//
// MessageId: COMADMIN_E_COMPFILE_CLASSNOTAVAIL
//
// MessageText:
//
//  The DLL does not support the components listed in the TypeLib
//
#define COMADMIN_E_COMPFILE_CLASSNOTAVAIL _HRESULT_TYPEDEF_(0x80110427L)

//
// MessageId: COMADMIN_E_COMPFILE_BADTLB
//
// MessageText:
//
//  The TypeLib could not be loaded
//
#define COMADMIN_E_COMPFILE_BADTLB       _HRESULT_TYPEDEF_(0x80110428L)

//
// MessageId: COMADMIN_E_COMPFILE_NOTINSTALLABLE
//
// MessageText:
//
//  The file does not contain components or component information
//
#define COMADMIN_E_COMPFILE_NOTINSTALLABLE _HRESULT_TYPEDEF_(0x80110429L)

//
// MessageId: COMADMIN_E_NOTCHANGEABLE
//
// MessageText:
//
//  Changes to this object and its sub-objects have been disabled
//
#define COMADMIN_E_NOTCHANGEABLE         _HRESULT_TYPEDEF_(0x8011042AL)

//
// MessageId: COMADMIN_E_NOTDELETEABLE
//
// MessageText:
//
//  The delete function has been disabled for this object
//
#define COMADMIN_E_NOTDELETEABLE         _HRESULT_TYPEDEF_(0x8011042BL)

//
// MessageId: COMADMIN_E_SESSION
//
// MessageText:
//
//  The server catalog version is not supported
//
#define COMADMIN_E_SESSION               _HRESULT_TYPEDEF_(0x8011042CL)

//
// MessageId: COMADMIN_E_COMP_MOVE_LOCKED
//
// MessageText:
//
//  The component move was disallowed, because the source or destination application is either a system application or currently locked against changes
//
#define COMADMIN_E_COMP_MOVE_LOCKED      _HRESULT_TYPEDEF_(0x8011042DL)

//
// MessageId: COMADMIN_E_COMP_MOVE_BAD_DEST
//
// MessageText:
//
//  The component move failed because the destination application no longer exists
//
#define COMADMIN_E_COMP_MOVE_BAD_DEST    _HRESULT_TYPEDEF_(0x8011042EL)

//
// MessageId: COMADMIN_E_REGISTERTLB
//
// MessageText:
//
//  The system was unable to register the TypeLib
//
#define COMADMIN_E_REGISTERTLB           _HRESULT_TYPEDEF_(0x80110430L)

//
// MessageId: COMADMIN_E_SYSTEMAPP
//
// MessageText:
//
//  This operation can not be performed on the system application
//
#define COMADMIN_E_SYSTEMAPP             _HRESULT_TYPEDEF_(0x80110433L)

//
// MessageId: COMADMIN_E_COMPFILE_NOREGISTRAR
//
// MessageText:
//
//  The component registrar referenced in this file is not available
//
#define COMADMIN_E_COMPFILE_NOREGISTRAR  _HRESULT_TYPEDEF_(0x80110434L)

//
// MessageId: COMADMIN_E_COREQCOMPINSTALLED
//
// MessageText:
//
//  A component in the same DLL is already installed
//
#define COMADMIN_E_COREQCOMPINSTALLED    _HRESULT_TYPEDEF_(0x80110435L)

//
// MessageId: COMADMIN_E_SERVICENOTINSTALLED
//
// MessageText:
//
//  The service is not installed
//
#define COMADMIN_E_SERVICENOTINSTALLED   _HRESULT_TYPEDEF_(0x80110436L)

//
// MessageId: COMADMIN_E_PROPERTYSAVEFAILED
//
// MessageText:
//
//  One or more property settings are either invalid or in conflict with each other
//
#define COMADMIN_E_PROPERTYSAVEFAILED    _HRESULT_TYPEDEF_(0x80110437L)

//
// MessageId: COMADMIN_E_OBJECTEXISTS
//
// MessageText:
//
//  The object you are attempting to add or rename already exists
//
#define COMADMIN_E_OBJECTEXISTS          _HRESULT_TYPEDEF_(0x80110438L)

//
// MessageId: COMADMIN_E_COMPONENTEXISTS
//
// MessageText:
//
//  The component already exists
//
#define COMADMIN_E_COMPONENTEXISTS       _HRESULT_TYPEDEF_(0x80110439L)

//
// MessageId: COMADMIN_E_REGFILE_CORRUPT
//
// MessageText:
//
//  The registration file is corrupt
//
#define COMADMIN_E_REGFILE_CORRUPT       _HRESULT_TYPEDEF_(0x8011043BL)

//
// MessageId: COMADMIN_E_PROPERTY_OVERFLOW
//
// MessageText:
//
//  The property value is too large
//
#define COMADMIN_E_PROPERTY_OVERFLOW     _HRESULT_TYPEDEF_(0x8011043CL)

//
// MessageId: COMADMIN_E_NOTINREGISTRY
//
// MessageText:
//
//  Object was not found in registry
//
#define COMADMIN_E_NOTINREGISTRY         _HRESULT_TYPEDEF_(0x8011043EL)

//
// MessageId: COMADMIN_E_OBJECTNOTPOOLABLE
//
// MessageText:
//
//  This object is not poolable
//
#define COMADMIN_E_OBJECTNOTPOOLABLE     _HRESULT_TYPEDEF_(0x8011043FL)

//
// MessageId: COMADMIN_E_APPLID_MATCHES_CLSID
//
// MessageText:
//
//  A CLSID with the same GUID as the new application ID is already installed on this machine
//
#define COMADMIN_E_APPLID_MATCHES_CLSID  _HRESULT_TYPEDEF_(0x80110446L)

//
// MessageId: COMADMIN_E_ROLE_DOES_NOT_EXIST
//
// MessageText:
//
//  A role assigned to a component, interface, or method did not exist in the application
//
#define COMADMIN_E_ROLE_DOES_NOT_EXIST   _HRESULT_TYPEDEF_(0x80110447L)

//
// MessageId: COMADMIN_E_START_APP_NEEDS_COMPONENTS
//
// MessageText:
//
//  You must have components in an application in order to start the application
//
#define COMADMIN_E_START_APP_NEEDS_COMPONENTS _HRESULT_TYPEDEF_(0x80110448L)

//
// MessageId: COMADMIN_E_REQUIRES_DIFFERENT_PLATFORM
//
// MessageText:
//
//  This operation is not enabled on this platform
//
#define COMADMIN_E_REQUIRES_DIFFERENT_PLATFORM _HRESULT_TYPEDEF_(0x80110449L)

//
// MessageId: COMADMIN_E_CAN_NOT_EXPORT_APP_PROXY
//
// MessageText:
//
//  Application Proxy is not exportable
//
#define COMADMIN_E_CAN_NOT_EXPORT_APP_PROXY _HRESULT_TYPEDEF_(0x8011044AL)

//
// MessageId: COMADMIN_E_CAN_NOT_START_APP
//
// MessageText:
//
//  Failed to start application because it is either a library application or an application proxy
//
#define COMADMIN_E_CAN_NOT_START_APP     _HRESULT_TYPEDEF_(0x8011044BL)

//
// MessageId: COMADMIN_E_CAN_NOT_EXPORT_SYS_APP
//
// MessageText:
//
//  System application is not exportable
//
#define COMADMIN_E_CAN_NOT_EXPORT_SYS_APP _HRESULT_TYPEDEF_(0x8011044CL)

//
// MessageId: COMADMIN_E_CANT_SUBSCRIBE_TO_COMPONENT
//
// MessageText:
//
//  Can not subscribe to this component (the component may have been imported)
//
#define COMADMIN_E_CANT_SUBSCRIBE_TO_COMPONENT _HRESULT_TYPEDEF_(0x8011044DL)

//
// MessageId: COMADMIN_E_EVENTCLASS_CANT_BE_SUBSCRIBER
//
// MessageText:
//
//  An event class cannot also be a subscriber component
//
#define COMADMIN_E_EVENTCLASS_CANT_BE_SUBSCRIBER _HRESULT_TYPEDEF_(0x8011044EL)

//
// MessageId: COMADMIN_E_LIB_APP_PROXY_INCOMPATIBLE
//
// MessageText:
//
//  Library applications and application proxies are incompatible
//
#define COMADMIN_E_LIB_APP_PROXY_INCOMPATIBLE _HRESULT_TYPEDEF_(0x8011044FL)

//
// MessageId: COMADMIN_E_BASE_PARTITION_ONLY
//
// MessageText:
//
//  This function is valid for the base partition only
//
#define COMADMIN_E_BASE_PARTITION_ONLY   _HRESULT_TYPEDEF_(0x80110450L)

//
// MessageId: COMADMIN_E_START_APP_DISABLED
//
// MessageText:
//
//  You cannot start an application that has been disabled
//
#define COMADMIN_E_START_APP_DISABLED    _HRESULT_TYPEDEF_(0x80110451L)

//
// MessageId: COMADMIN_E_CAT_DUPLICATE_PARTITION_NAME
//
// MessageText:
//
//  The specified partition name is already in use on this computer
//
#define COMADMIN_E_CAT_DUPLICATE_PARTITION_NAME _HRESULT_TYPEDEF_(0x80110457L)

//
// MessageId: COMADMIN_E_CAT_INVALID_PARTITION_NAME
//
// MessageText:
//
//  The specified partition name is invalid. Check that the name contains at least one visible character
//
#define COMADMIN_E_CAT_INVALID_PARTITION_NAME _HRESULT_TYPEDEF_(0x80110458L)

//
// MessageId: COMADMIN_E_CAT_PARTITION_IN_USE
//
// MessageText:
//
//  The partition cannot be deleted because it is the default partition for one or more users
//
#define COMADMIN_E_CAT_PARTITION_IN_USE  _HRESULT_TYPEDEF_(0x80110459L)

//
// MessageId: COMADMIN_E_FILE_PARTITION_DUPLICATE_FILES
//
// MessageText:
//
//  The partition cannot be exported, because one or more components in the partition have the same file name
//
#define COMADMIN_E_FILE_PARTITION_DUPLICATE_FILES _HRESULT_TYPEDEF_(0x8011045AL)

//
// MessageId: COMADMIN_E_CAT_IMPORTED_COMPONENTS_NOT_ALLOWED
//
// MessageText:
//
//  Applications that contain one or more imported components cannot be installed into a non-base partition
//
#define COMADMIN_E_CAT_IMPORTED_COMPONENTS_NOT_ALLOWED _HRESULT_TYPEDEF_(0x8011045BL)

//
// MessageId: COMADMIN_E_AMBIGUOUS_APPLICATION_NAME
//
// MessageText:
//
//  The application name is not unique and cannot be resolved to an application id
//
#define COMADMIN_E_AMBIGUOUS_APPLICATION_NAME _HRESULT_TYPEDEF_(0x8011045CL)

//
// MessageId: COMADMIN_E_AMBIGUOUS_PARTITION_NAME
//
// MessageText:
//
//  The partition name is not unique and cannot be resolved to a partition id
//
#define COMADMIN_E_AMBIGUOUS_PARTITION_NAME _HRESULT_TYPEDEF_(0x8011045DL)

//
// MessageId: COMADMIN_E_REGDB_NOTINITIALIZED
//
// MessageText:
//
//  The COM+ registry database has not been initialized
//
#define COMADMIN_E_REGDB_NOTINITIALIZED  _HRESULT_TYPEDEF_(0x80110472L)

//
// MessageId: COMADMIN_E_REGDB_NOTOPEN
//
// MessageText:
//
//  The COM+ registry database is not open
//
#define COMADMIN_E_REGDB_NOTOPEN         _HRESULT_TYPEDEF_(0x80110473L)

//
// MessageId: COMADMIN_E_REGDB_SYSTEMERR
//
// MessageText:
//
//  The COM+ registry database detected a system error
//
#define COMADMIN_E_REGDB_SYSTEMERR       _HRESULT_TYPEDEF_(0x80110474L)

//
// MessageId: COMADMIN_E_REGDB_ALREADYRUNNING
//
// MessageText:
//
//  The COM+ registry database is already running
//
#define COMADMIN_E_REGDB_ALREADYRUNNING  _HRESULT_TYPEDEF_(0x80110475L)

//
// MessageId: COMADMIN_E_MIG_VERSIONNOTSUPPORTED
//
// MessageText:
//
//  This version of the COM+ registry database cannot be migrated
//
#define COMADMIN_E_MIG_VERSIONNOTSUPPORTED _HRESULT_TYPEDEF_(0x80110480L)

//
// MessageId: COMADMIN_E_MIG_SCHEMANOTFOUND
//
// MessageText:
//
//  The schema version to be migrated could not be found in the COM+ registry database
//
#define COMADMIN_E_MIG_SCHEMANOTFOUND    _HRESULT_TYPEDEF_(0x80110481L)

//
// MessageId: COMADMIN_E_CAT_BITNESSMISMATCH
//
// MessageText:
//
//  There was a type mismatch between binaries
//
#define COMADMIN_E_CAT_BITNESSMISMATCH   _HRESULT_TYPEDEF_(0x80110482L)

//
// MessageId: COMADMIN_E_CAT_UNACCEPTABLEBITNESS
//
// MessageText:
//
//  A binary of unknown or invalid type was provided
//
#define COMADMIN_E_CAT_UNACCEPTABLEBITNESS _HRESULT_TYPEDEF_(0x80110483L)

//
// MessageId: COMADMIN_E_CAT_WRONGAPPBITNESS
//
// MessageText:
//
//  There was a type mismatch between a binary and an application
//
#define COMADMIN_E_CAT_WRONGAPPBITNESS   _HRESULT_TYPEDEF_(0x80110484L)

//
// MessageId: COMADMIN_E_CAT_PAUSE_RESUME_NOT_SUPPORTED
//
// MessageText:
//
//  The application cannot be paused or resumed
//
#define COMADMIN_E_CAT_PAUSE_RESUME_NOT_SUPPORTED _HRESULT_TYPEDEF_(0x80110485L)

//
// MessageId: COMADMIN_E_CAT_SERVERFAULT
//
// MessageText:
//
//  The COM+ Catalog Server threw an exception during execution
//
#define COMADMIN_E_CAT_SERVERFAULT       _HRESULT_TYPEDEF_(0x80110486L)

//
// COMPLUS Queued component errors
//
//
// MessageId: COMQC_E_APPLICATION_NOT_QUEUED
//
// MessageText:
//
//  Only COM+ Applications marked "queued" can be invoked using the "queue" moniker
//
#define COMQC_E_APPLICATION_NOT_QUEUED   _HRESULT_TYPEDEF_(0x80110600L)

//
// MessageId: COMQC_E_NO_QUEUEABLE_INTERFACES
//
// MessageText:
//
//  At least one interface must be marked "queued" in order to create a queued component instance with the "queue" moniker
//
#define COMQC_E_NO_QUEUEABLE_INTERFACES  _HRESULT_TYPEDEF_(0x80110601L)

//
// MessageId: COMQC_E_QUEUING_SERVICE_NOT_AVAILABLE
//
// MessageText:
//
//  MSMQ is required for the requested operation and is not installed
//
#define COMQC_E_QUEUING_SERVICE_NOT_AVAILABLE _HRESULT_TYPEDEF_(0x80110602L)

//
// MessageId: COMQC_E_NO_IPERSISTSTREAM
//
// MessageText:
//
//  Unable to marshal an interface that does not support IPersistStream
//
#define COMQC_E_NO_IPERSISTSTREAM        _HRESULT_TYPEDEF_(0x80110603L)

//
// MessageId: COMQC_E_BAD_MESSAGE
//
// MessageText:
//
//  The message is improperly formatted or was damaged in transit
//
#define COMQC_E_BAD_MESSAGE              _HRESULT_TYPEDEF_(0x80110604L)

//
// MessageId: COMQC_E_UNAUTHENTICATED
//
// MessageText:
//
//  An unauthenticated message was received by an application that accepts only authenticated messages
//
#define COMQC_E_UNAUTHENTICATED          _HRESULT_TYPEDEF_(0x80110605L)

//
// MessageId: COMQC_E_UNTRUSTED_ENQUEUER
//
// MessageText:
//
//  The message was requeued or moved by a user not in the "QC Trusted User" role
//
#define COMQC_E_UNTRUSTED_ENQUEUER       _HRESULT_TYPEDEF_(0x80110606L)

//
// The range 0x700-0x7ff is reserved for MSDTC errors.
//
//
// MessageId: MSDTC_E_DUPLICATE_RESOURCE
//
// MessageText:
//
//  Cannot create a duplicate resource of type Distributed Transaction Coordinator
//
#define MSDTC_E_DUPLICATE_RESOURCE       _HRESULT_TYPEDEF_(0x80110701L)

//
// More COMADMIN errors from 0x8**
//
//
// MessageId: COMADMIN_E_OBJECT_PARENT_MISSING
//
// MessageText:
//
//  One of the objects being inserted or updated does not belong to a valid parent collection
//
#define COMADMIN_E_OBJECT_PARENT_MISSING _HRESULT_TYPEDEF_(0x80110808L)

//
// MessageId: COMADMIN_E_OBJECT_DOES_NOT_EXIST
//
// MessageText:
//
//  One of the specified objects cannot be found
//
#define COMADMIN_E_OBJECT_DOES_NOT_EXIST _HRESULT_TYPEDEF_(0x80110809L)

//
// MessageId: COMADMIN_E_APP_NOT_RUNNING
//
// MessageText:
//
//  The specified application is not currently running
//
#define COMADMIN_E_APP_NOT_RUNNING       _HRESULT_TYPEDEF_(0x8011080AL)

//
// MessageId: COMADMIN_E_INVALID_PARTITION
//
// MessageText:
//
//  The partition(s) specified are not valid.
//
#define COMADMIN_E_INVALID_PARTITION     _HRESULT_TYPEDEF_(0x8011080BL)

//
// MessageId: COMADMIN_E_SVCAPP_NOT_POOLABLE_OR_RECYCLABLE
//
// MessageText:
//
//  COM+ applications that run as NT service may not be pooled or recycled
//
#define COMADMIN_E_SVCAPP_NOT_POOLABLE_OR_RECYCLABLE _HRESULT_TYPEDEF_(0x8011080DL)

//
// MessageId: COMADMIN_E_USER_IN_SET
//
// MessageText:
//
//  One or more users are already assigned to a local partition set.
//
#define COMADMIN_E_USER_IN_SET           _HRESULT_TYPEDEF_(0x8011080EL)

//
// MessageId: COMADMIN_E_CANTRECYCLELIBRARYAPPS
//
// MessageText:
//
//  Library applications may not be recycled.
//
#define COMADMIN_E_CANTRECYCLELIBRARYAPPS _HRESULT_TYPEDEF_(0x8011080FL)

//
// MessageId: COMADMIN_E_CANTRECYCLESERVICEAPPS
//
// MessageText:
//
//  Applications running as NT services may not be recycled.
//
#define COMADMIN_E_CANTRECYCLESERVICEAPPS _HRESULT_TYPEDEF_(0x80110811L)

//
// MessageId: COMADMIN_E_PROCESSALREADYRECYCLED
//
// MessageText:
//
//  The process has already been recycled.
//
#define COMADMIN_E_PROCESSALREADYRECYCLED _HRESULT_TYPEDEF_(0x80110812L)

//
// MessageId: COMADMIN_E_PAUSEDPROCESSMAYNOTBERECYCLED
//
// MessageText:
//
//  A paused process may not be recycled.
//
#define COMADMIN_E_PAUSEDPROCESSMAYNOTBERECYCLED _HRESULT_TYPEDEF_(0x80110813L)

//
// MessageId: COMADMIN_E_CANTMAKEINPROCSERVICE
//
// MessageText:
//
//  Library applications may not be NT services.
//
#define COMADMIN_E_CANTMAKEINPROCSERVICE _HRESULT_TYPEDEF_(0x80110814L)

//
// MessageId: COMADMIN_E_PROGIDINUSEBYCLSID
//
// MessageText:
//
//  The ProgID provided to the copy operation is invalid. The ProgID is in use by another registered CLSID.
//
#define COMADMIN_E_PROGIDINUSEBYCLSID    _HRESULT_TYPEDEF_(0x80110815L)

//
// MessageId: COMADMIN_E_DEFAULT_PARTITION_NOT_IN_SET
//
// MessageText:
//
//  The partition specified as default is not a member of the partition set.
//
#define COMADMIN_E_DEFAULT_PARTITION_NOT_IN_SET _HRESULT_TYPEDEF_(0x80110816L)

//
// MessageId: COMADMIN_E_RECYCLEDPROCESSMAYNOTBEPAUSED
//
// MessageText:
//
//  A recycled process may not be paused.
//
#define COMADMIN_E_RECYCLEDPROCESSMAYNOTBEPAUSED _HRESULT_TYPEDEF_(0x80110817L)

//
// MessageId: COMADMIN_E_PARTITION_ACCESSDENIED
//
// MessageText:
//
//  Access to the specified partition is denied.
//
#define COMADMIN_E_PARTITION_ACCESSDENIED _HRESULT_TYPEDEF_(0x80110818L)

//
// MessageId: COMADMIN_E_PARTITION_MSI_ONLY
//
// MessageText:
//
//  Only Application Files (*.MSI files) can be installed into partitions.
//
#define COMADMIN_E_PARTITION_MSI_ONLY    _HRESULT_TYPEDEF_(0x80110819L)

//
// MessageId: COMADMIN_E_LEGACYCOMPS_NOT_ALLOWED_IN_1_0_FORMAT
//
// MessageText:
//
//  Applications containing one or more legacy components may not be exported to 1.0 format.
//
#define COMADMIN_E_LEGACYCOMPS_NOT_ALLOWED_IN_1_0_FORMAT _HRESULT_TYPEDEF_(0x8011081AL)

//
// MessageId: COMADMIN_E_LEGACYCOMPS_NOT_ALLOWED_IN_NONBASE_PARTITIONS
//
// MessageText:
//
//  Legacy components may not exist in non-base partitions.
//
#define COMADMIN_E_LEGACYCOMPS_NOT_ALLOWED_IN_NONBASE_PARTITIONS _HRESULT_TYPEDEF_(0x8011081BL)

//
// MessageId: COMADMIN_E_COMP_MOVE_SOURCE
//
// MessageText:
//
//  A component cannot be moved (or copied) from the System Application, an application proxy or a non-changeable application
//
#define COMADMIN_E_COMP_MOVE_SOURCE      _HRESULT_TYPEDEF_(0x8011081CL)

//
// MessageId: COMADMIN_E_COMP_MOVE_DEST
//
// MessageText:
//
//  A component cannot be moved (or copied) to the System Application, an application proxy or a non-changeable application
//
#define COMADMIN_E_COMP_MOVE_DEST        _HRESULT_TYPEDEF_(0x8011081DL)

//
// MessageId: COMADMIN_E_COMP_MOVE_PRIVATE
//
// MessageText:
//
//  A private component cannot be moved (or copied) to a library application or to the base partition
//
#define COMADMIN_E_COMP_MOVE_PRIVATE     _HRESULT_TYPEDEF_(0x8011081EL)

//
// MessageId: COMADMIN_E_BASEPARTITION_REQUIRED_IN_SET
//
// MessageText:
//
//  The Base Application Partition exists in all partition sets and cannot be removed.
//
#define COMADMIN_E_BASEPARTITION_REQUIRED_IN_SET _HRESULT_TYPEDEF_(0x8011081FL)

//
// MessageId: COMADMIN_E_CANNOT_ALIAS_EVENTCLASS
//
// MessageText:
//
//  Alas, Event Class components cannot be aliased.
//
#define COMADMIN_E_CANNOT_ALIAS_EVENTCLASS _HRESULT_TYPEDEF_(0x80110820L)

//
// MessageId: COMADMIN_E_PRIVATE_ACCESSDENIED
//
// MessageText:
//
//  Access is denied because the component is private.
//
#define COMADMIN_E_PRIVATE_ACCESSDENIED  _HRESULT_TYPEDEF_(0x80110821L)

//
// MessageId: COMADMIN_E_SAFERINVALID
//
// MessageText:
//
//  The specified SAFER level is invalid.
//
#define COMADMIN_E_SAFERINVALID          _HRESULT_TYPEDEF_(0x80110822L)

//
// MessageId: COMADMIN_E_REGISTRY_ACCESSDENIED
//
// MessageText:
//
//  The specified user cannot write to the system registry
//
#define COMADMIN_E_REGISTRY_ACCESSDENIED _HRESULT_TYPEDEF_(0x80110823L)

//
// MessageId: COMADMIN_E_PARTITIONS_DISABLED
//
// MessageText:
//
//  COM+ partitions are currently disabled.
//
#define COMADMIN_E_PARTITIONS_DISABLED   _HRESULT_TYPEDEF_(0x80110824L)

#endif//_WINERROR_

