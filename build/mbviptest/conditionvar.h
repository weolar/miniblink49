
#ifndef ConditionVarXp_h
#define ConditionVarXp_h

#ifdef _WIN64
#define InterlockedBitTestAndSetPointer(ptr,val) InterlockedBitTestAndSet64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedAddPointer(ptr,val) InterlockedAdd64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedAndPointer(ptr,val) InterlockedAnd64((PLONGLONG)ptr,(LONGLONG)val)
#define InterlockedOrPointer(ptr,val) InterlockedOr64((PLONGLONG)ptr,(LONGLONG)val)
#else
#define InterlockedBitTestAndSetPointer(ptr,val) InterlockedBitTestAndSet((PLONG)ptr,(LONG)val)
#define InterlockedAddPointer(ptr,val) InterlockedAdd((PLONG)ptr,(LONG)val)
#define InterlockedAndPointer(ptr,val) InterlockedAnd((PLONG)ptr,(LONG)val)
#define InterlockedOrPointer(ptr,val) InterlockedOr((PLONG)ptr,(LONG)val)
#endif

#define XP_SRWLOCK_OWNED_BIT   0
#define XP_SRWLOCK_CONTENDED_BIT   1
#define XP_SRWLOCK_SHARED_BIT  2
#define XP_SRWLOCK_CONTENTION_LOCK_BIT 3
#define XP_SRWLOCK_OWNED   (1 << XP_SRWLOCK_OWNED_BIT)
#define XP_SRWLOCK_CONTENDED   (1 << XP_SRWLOCK_CONTENDED_BIT)
#define XP_SRWLOCK_SHARED  (1 << XP_SRWLOCK_SHARED_BIT)
#define XP_SRWLOCK_CONTENTION_LOCK (1 << XP_SRWLOCK_CONTENTION_LOCK_BIT)
#define XP_SRWLOCK_MASK    (XP_SRWLOCK_OWNED | XP_SRWLOCK_CONTENDED | \
                             XP_SRWLOCK_SHARED | XP_SRWLOCK_CONTENTION_LOCK)
#define XP_SRWLOCK_BITS    4

typedef struct _XP_SRWLOCK_SHARED_WAKE {
    LONG Wake;
    volatile struct _XP_SRWLOCK_SHARED_WAKE *Next;
} volatile XP_SRWLOCK_SHARED_WAKE, *PXP_SRWLOCK_SHARED_WAKE;

typedef struct _XP_SRWLOCK_WAITBLOCK {
    /* SharedCount is the number of shared acquirers. */
    LONG SharedCount;

    /* Last points to the last wait block in the chain. The value
    is only valid when read from the first wait block. */
    volatile struct _XP_SRWLOCK_WAITBLOCK *Last;

    /* Next points to the next wait block in the chain. */
    volatile struct _XP_SRWLOCK_WAITBLOCK *Next;

    union {
        /* Wake is only valid for exclusive wait blocks */
        LONG Wake;
        /* The wake chain is only valid for shared wait blocks */
        struct {
            PXP_SRWLOCK_SHARED_WAKE SharedWakeChain;
            PXP_SRWLOCK_SHARED_WAKE LastSharedWake;
        };
    };

    BOOLEAN Exclusive;
} volatile XP_SRWLOCK_WAITBLOCK, *PXP_SRWLOCK_WAITBLOCK;

inline void CV_ASSERT(bool b)
{
    if (!b)
        DebugBreak();
}

class SlimReadWriteLock {
private:
    static void releaseWaitBlockLockExclusiveImpl(PRTL_SRWLOCK SRWLock, PXP_SRWLOCK_WAITBLOCK FirstWaitBlock)
    {
        PXP_SRWLOCK_WAITBLOCK Next;
        LONG_PTR NewValue;

        /* NOTE: We're currently in an exclusive lock in contended mode. */
        Next = FirstWaitBlock->Next;
        if (Next != NULL) {
            /* There's more blocks chained, we need to update the pointers
            in the next wait block and update the wait block pointer. */
            NewValue = (LONG_PTR)Next | XP_SRWLOCK_OWNED | XP_SRWLOCK_CONTENDED;
            if (!FirstWaitBlock->Exclusive) {
                /* The next wait block has to be an exclusive lock! */
                CV_ASSERT(!!(Next->Exclusive));

                /* Save the shared count */
                Next->SharedCount = FirstWaitBlock->SharedCount;

                NewValue |= XP_SRWLOCK_SHARED;
            }

            Next->Last = FirstWaitBlock->Last;
        } else {
            /* Convert the lock to a simple lock. */
            if (FirstWaitBlock->Exclusive)
                NewValue = XP_SRWLOCK_OWNED;
            else {
                CV_ASSERT(FirstWaitBlock->SharedCount > 0);

                NewValue = ((LONG_PTR)FirstWaitBlock->SharedCount << XP_SRWLOCK_BITS) |
                    XP_SRWLOCK_SHARED | XP_SRWLOCK_OWNED;
            }
        }

        (void)InterlockedExchangePointer(&SRWLock->Ptr, (PVOID)NewValue);

        if (FirstWaitBlock->Exclusive) {
            (void)_InterlockedOr(&FirstWaitBlock->Wake, TRUE);
        } else {
            PXP_SRWLOCK_SHARED_WAKE WakeChain, NextWake;

            /* If we were the first one to acquire the shared
            lock, we now need to wake all others... */
            WakeChain = FirstWaitBlock->SharedWakeChain;
            do {
                NextWake = WakeChain->Next;

                (void)_InterlockedOr((PLONG)&WakeChain->Wake, TRUE);

                WakeChain = NextWake;
            } while (WakeChain != NULL);
        }
    }

    static VOID NTAPI releaseWaitBlockLockLastSharedImpl(PRTL_SRWLOCK SRWLock, PXP_SRWLOCK_WAITBLOCK FirstWaitBlock)
    {
        PXP_SRWLOCK_WAITBLOCK Next;
        LONG_PTR NewValue;

        /* NOTE: We're currently in a shared lock in contended mode. */

        /* The next acquirer to be unwaited *must* be an exclusive lock! */
        CV_ASSERT(!!(FirstWaitBlock->Exclusive));

        Next = FirstWaitBlock->Next;
        if (Next != NULL) {
            /* There's more blocks chained, we need to update the pointers
            in the next wait block and update the wait block pointer. */
            NewValue = (LONG_PTR)Next | XP_SRWLOCK_OWNED | XP_SRWLOCK_CONTENDED;

            Next->Last = FirstWaitBlock->Last;
        } else {
            /* Convert the lock to a simple exclusive lock. */
            NewValue = XP_SRWLOCK_OWNED;
        }

        (void)InterlockedExchangePointer(&SRWLock->Ptr, (PVOID)NewValue);

        (void)_InterlockedOr(&FirstWaitBlock->Wake, TRUE);
    }

    static void releaseWaitBlockLockImpl(PRTL_SRWLOCK SRWLock)
    {
        _InterlockedAnd((volatile long *)&SRWLock->Ptr, ~XP_SRWLOCK_CONTENTION_LOCK);
    }

    static PXP_SRWLOCK_WAITBLOCK acquireWaitBlockLockImpl(IN OUT PRTL_SRWLOCK SRWLock)
    {
        LONG_PTR PrevValue;
        PXP_SRWLOCK_WAITBLOCK WaitBlock;

        while (1) {
            PrevValue = _InterlockedOr((volatile long *)&SRWLock->Ptr, XP_SRWLOCK_CONTENTION_LOCK);

            if (!(PrevValue & XP_SRWLOCK_CONTENTION_LOCK))
                break;

            YieldProcessor();
        }

        if (!(PrevValue & XP_SRWLOCK_CONTENDED) || (PrevValue & ~XP_SRWLOCK_MASK) == 0) {
            /* Too bad, looks like the wait block was removed in the
            meanwhile, unlock again */
            releaseWaitBlockLockImpl(SRWLock);
            return NULL;
        }

        WaitBlock = (PXP_SRWLOCK_WAITBLOCK)(PrevValue & ~XP_SRWLOCK_MASK);

        return WaitBlock;
    }

    static void acquireSRWLockExclusiveWaitImpl(PRTL_SRWLOCK SRWLock, PXP_SRWLOCK_WAITBLOCK WaitBlock)
    {
        LONG_PTR CurrentValue;

        while (1) {
            CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;
            if (!(CurrentValue & XP_SRWLOCK_SHARED)) {
                if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                    if (WaitBlock->Wake != 0) {
                        /* Our wait block became the first one
                        in the chain, we own the lock now! */
                        break;
                    }
                } else {
                    /* The last wait block was removed and/or we're
                    finally a simple exclusive lock. This means we
                    don't need to wait anymore, we acquired the lock! */
                    break;
                }
            }

            YieldProcessor();
        }
    }

    static void acquireSRWLockSharedWaitImpl(PRTL_SRWLOCK SRWLock, PXP_SRWLOCK_WAITBLOCK FirstWait, PXP_SRWLOCK_SHARED_WAKE WakeChain)
    {
        if (FirstWait != NULL) {
            while (WakeChain->Wake == 0) {
                YieldProcessor();
            }
        } else {
            LONG_PTR CurrentValue;

            while (1) {
                CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;
                if (CurrentValue & XP_SRWLOCK_SHARED) {
                    /* The XP_SRWLOCK_OWNED bit always needs to be set when
                    XP_SRWLOCK_SHARED is set! */
                    CV_ASSERT(CurrentValue & XP_SRWLOCK_OWNED);

                    if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                        if (WakeChain->Wake != 0) {
                            /* Our wait block became the first one
                            in the chain, we own the lock now! */
                            break;
                        }
                    } else {
                        /* The last wait block was removed and/or we're
                        finally a simple shared lock. This means we
                        don't need to wait anymore, we acquired the lock! */
                        break;
                    }
                }

                YieldProcessor();
            }
        }
    }

public:
    static void acquireSRWLockShared(IN OUT PRTL_SRWLOCK SRWLock)
    {
        __declspec(align(16)) XP_SRWLOCK_WAITBLOCK StackWaitBlock;
        XP_SRWLOCK_SHARED_WAKE SharedWake;
        LONG_PTR CurrentValue, NewValue;
        PXP_SRWLOCK_WAITBLOCK First, Shared, FirstWait;

        while (1) {
            CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;

            if (CurrentValue & XP_SRWLOCK_SHARED) {
                /* NOTE: It is possible that the XP_SRWLOCK_OWNED bit is set! */

                if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                    /* There's other waiters already, lock the wait blocks and
                    increment the shared count */
                    First = acquireWaitBlockLockImpl(SRWLock);
                    if (First != NULL) {
                        FirstWait = NULL;

                        if (First->Exclusive) {
                            /* We need to setup a new wait block! Although
                            we're currently in a shared lock and we're acquiring
                            a shared lock, there are exclusive locks queued. We need
                            to wait until those are released. */
                            Shared = First->Last;

                            if (Shared->Exclusive) {
                                StackWaitBlock.Exclusive = FALSE;
                                StackWaitBlock.SharedCount = 1;
                                StackWaitBlock.Next = NULL;
                                StackWaitBlock.Last = &StackWaitBlock;
                                StackWaitBlock.SharedWakeChain = &SharedWake;

                                Shared->Next = &StackWaitBlock;
                                First->Last = &StackWaitBlock;

                                Shared = &StackWaitBlock;
                                FirstWait = &StackWaitBlock;
                            } else {
                                Shared->LastSharedWake->Next = &SharedWake;
                                Shared->SharedCount++;
                            }
                        } else {
                            Shared = First;
                            Shared->LastSharedWake->Next = &SharedWake;
                            Shared->SharedCount++;
                        }

                        SharedWake.Next = NULL;
                        SharedWake.Wake = 0;

                        Shared->LastSharedWake = &SharedWake;

                        releaseWaitBlockLockImpl(SRWLock);

                        acquireSRWLockSharedWaitImpl(SRWLock,
                            FirstWait,
                            &SharedWake);

                        /* Successfully incremented the shared count, we acquired the lock */
                        break;
                    }
                } else {
                    /* This is a fastest path, just increment the number of
                    current shared locks */

                    /* Since the XP_SRWLOCK_SHARED bit is set, the XP_SRWLOCK_OWNED bit also has
                    to be set! */

                    CV_ASSERT(CurrentValue & XP_SRWLOCK_OWNED);

                    NewValue = (CurrentValue >> XP_SRWLOCK_BITS) + 1;
                    NewValue = (NewValue << XP_SRWLOCK_BITS) | (CurrentValue & XP_SRWLOCK_MASK);

                    if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                        (PVOID)NewValue,
                        (PVOID)CurrentValue) == CurrentValue) {
                        /* Successfully incremented the shared count, we acquired the lock */
                        break;
                    }
                }
            } else {
                if (CurrentValue & XP_SRWLOCK_OWNED) {
                    /* The resource is currently acquired exclusively */
                    if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                        SharedWake.Next = NULL;
                        SharedWake.Wake = 0;

                        /* There's other waiters already, lock the wait blocks and
                        increment the shared count. If the last block in the chain
                        is an exclusive lock, add another block. */

                        StackWaitBlock.Exclusive = FALSE;
                        StackWaitBlock.SharedCount = 0;
                        StackWaitBlock.Next = NULL;
                        StackWaitBlock.Last = &StackWaitBlock;
                        StackWaitBlock.SharedWakeChain = &SharedWake;

                        First = acquireWaitBlockLockImpl(SRWLock);
                        if (First != NULL) {
                            Shared = First->Last;
                            if (Shared->Exclusive) {
                                Shared->Next = &StackWaitBlock;
                                First->Last = &StackWaitBlock;

                                Shared = &StackWaitBlock;
                                FirstWait = &StackWaitBlock;
                            } else {
                                FirstWait = NULL;
                                Shared->LastSharedWake->Next = &SharedWake;
                            }

                            Shared->SharedCount++;
                            Shared->LastSharedWake = &SharedWake;

                            releaseWaitBlockLockImpl(SRWLock);

                            acquireSRWLockSharedWaitImpl(SRWLock,
                                FirstWait,
                                &SharedWake);

                            /* Successfully incremented the shared count, we acquired the lock */
                            break;
                        }
                    } else {
                        SharedWake.Next = NULL;
                        SharedWake.Wake = 0;

                        /* We need to setup the first wait block. Currently an exclusive lock is
                        held, change the lock to contended mode. */
                        StackWaitBlock.Exclusive = FALSE;
                        StackWaitBlock.SharedCount = 1;
                        StackWaitBlock.Next = NULL;
                        StackWaitBlock.Last = &StackWaitBlock;
                        StackWaitBlock.SharedWakeChain = &SharedWake;
                        StackWaitBlock.LastSharedWake = &SharedWake;

                        NewValue = (ULONG_PTR)&StackWaitBlock | XP_SRWLOCK_OWNED | XP_SRWLOCK_CONTENDED;
                        if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                            (PVOID)NewValue,
                            (PVOID)CurrentValue) == CurrentValue) {
                            acquireSRWLockSharedWaitImpl(SRWLock,
                                &StackWaitBlock,
                                &SharedWake);

                            /* Successfully set the shared count, we acquired the lock */
                            break;
                        }
                    }
                } else {
                    /* This is a fast path, we can simply try to set the shared count to 1 */
                    NewValue = (1 << XP_SRWLOCK_BITS) | XP_SRWLOCK_SHARED | XP_SRWLOCK_OWNED;

                    /* The XP_SRWLOCK_CONTENDED bit should never be set if neither the
                    XP_SRWLOCK_SHARED nor the XP_SRWLOCK_OWNED bit is set */
                    CV_ASSERT(!(CurrentValue & XP_SRWLOCK_CONTENDED));

                    if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                        (PVOID)NewValue,
                        (PVOID)CurrentValue) == CurrentValue) {
                        /* Successfully set the shared count, we acquired the lock */
                        break;
                    }
                }
            }

            YieldProcessor();
        }
    }

    static void releaseSRWLockShared(PRTL_SRWLOCK SRWLock)
    {
        LONG_PTR CurrentValue, NewValue;
        PXP_SRWLOCK_WAITBLOCK WaitBlock;
        BOOLEAN LastShared;

        while (1) {
            CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;

            if (CurrentValue & XP_SRWLOCK_SHARED) {
                if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                    /* There's a wait block, we need to wake a pending
                    exclusive acquirer if this is the last shared release */
                    WaitBlock = acquireWaitBlockLockImpl(SRWLock);
                    if (WaitBlock != NULL) {
                        LastShared = (--WaitBlock->SharedCount == 0);

                        if (LastShared)
                            releaseWaitBlockLockLastSharedImpl(SRWLock, WaitBlock);
                        else
                            releaseWaitBlockLockImpl(SRWLock);

                        /* We released the lock */
                        break;
                    }
                } else {
                    /* This is a fast path, we can simply decrement the shared
                    count and store the pointer */
                    NewValue = CurrentValue >> XP_SRWLOCK_BITS;

                    if (--NewValue != 0) {
                        NewValue = (NewValue << XP_SRWLOCK_BITS) | XP_SRWLOCK_SHARED | XP_SRWLOCK_OWNED;
                    }

                    if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                        (PVOID)NewValue,
                        (PVOID)CurrentValue) == CurrentValue) {
                        /* Successfully released the lock */
                        break;
                    }
                }
            } else {
                /* The XP_SRWLOCK_SHARED bit has to be present now,
                even in the contended case! */
                //RtlRaiseStatus(STATUS_RESOURCE_NOT_OWNED);
                DebugBreak();
            }

            YieldProcessor();
        }
    }

    static void acquireSRWLockExclusive(IN OUT PRTL_SRWLOCK SRWLock)
    {
        __declspec(align(16)) XP_SRWLOCK_WAITBLOCK StackWaitBlock;
        PXP_SRWLOCK_WAITBLOCK First, Last;

        if (InterlockedBitTestAndSetPointer(&SRWLock->Ptr,
            XP_SRWLOCK_OWNED_BIT)) {
            LONG_PTR CurrentValue, NewValue;

            while (1) {
                CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;

                if (CurrentValue & XP_SRWLOCK_SHARED) {
                    /* A shared lock is being held right now. We need to add a wait block! */

                    if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                        goto AddWaitBlock;
                    } else {
                        /* There are no wait blocks so far, we need to add ourselves as the first
                        wait block. We need to keep the shared count! */
                        StackWaitBlock.Exclusive = TRUE;
                        StackWaitBlock.SharedCount = (LONG)(CurrentValue >> XP_SRWLOCK_BITS);
                        StackWaitBlock.Next = NULL;
                        StackWaitBlock.Last = &StackWaitBlock;
                        StackWaitBlock.Wake = 0;

                        NewValue = (ULONG_PTR)&StackWaitBlock | XP_SRWLOCK_SHARED | XP_SRWLOCK_CONTENDED | XP_SRWLOCK_OWNED;

                        if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                            (PVOID)NewValue,
                            (PVOID)CurrentValue) == CurrentValue) {
                            acquireSRWLockExclusiveWaitImpl(SRWLock,
                                &StackWaitBlock);

                            /* Successfully acquired the exclusive lock */
                            break;
                        }
                    }
                } else {
                    if (CurrentValue & XP_SRWLOCK_OWNED) {
                        /* An exclusive lock is being held right now. We need to add a wait block! */

                        if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                        AddWaitBlock:
                            StackWaitBlock.Exclusive = TRUE;
                            StackWaitBlock.SharedCount = 0;
                            StackWaitBlock.Next = NULL;
                            StackWaitBlock.Last = &StackWaitBlock;
                            StackWaitBlock.Wake = 0;

                            First = acquireWaitBlockLockImpl(SRWLock);
                            if (First != NULL) {
                                Last = First->Last;
                                Last->Next = &StackWaitBlock;
                                First->Last = &StackWaitBlock;

                                releaseWaitBlockLockImpl(SRWLock);

                                acquireSRWLockExclusiveWaitImpl(SRWLock,
                                    &StackWaitBlock);

                                /* Successfully acquired the exclusive lock */
                                break;
                            }
                        } else {
                            /* There are no wait blocks so far, we need to add ourselves as the first
                            wait block. We need to keep the shared count! */
                            StackWaitBlock.Exclusive = TRUE;
                            StackWaitBlock.SharedCount = 0;
                            StackWaitBlock.Next = NULL;
                            StackWaitBlock.Last = &StackWaitBlock;
                            StackWaitBlock.Wake = 0;

                            NewValue = (ULONG_PTR)&StackWaitBlock | XP_SRWLOCK_OWNED | XP_SRWLOCK_CONTENDED;
                            if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                                (PVOID)NewValue,
                                (PVOID)CurrentValue) == CurrentValue) {
                                acquireSRWLockExclusiveWaitImpl(SRWLock,
                                    &StackWaitBlock);

                                /* Successfully acquired the exclusive lock */
                                break;
                            }
                        }
                    } else {
                        if (!InterlockedBitTestAndSetPointer(&SRWLock->Ptr,
                            XP_SRWLOCK_OWNED_BIT)) {
                            /* We managed to get hold of a simple exclusive lock! */
                            break;
                        }
                    }
                }

                YieldProcessor();
            }
        }
    }

    static void releaseSRWLockExclusive(PRTL_SRWLOCK SRWLock)
    {
        LONG_PTR CurrentValue, NewValue;
        PXP_SRWLOCK_WAITBLOCK WaitBlock;

        while (1) {
            CurrentValue = *(volatile LONG_PTR *)&SRWLock->Ptr;

            if (!(CurrentValue & XP_SRWLOCK_OWNED)) {
                //RtlRaiseStatus(STATUS_RESOURCE_NOT_OWNED);
                DebugBreak();
            }

            if (!(CurrentValue & XP_SRWLOCK_SHARED)) {
                if (CurrentValue & XP_SRWLOCK_CONTENDED) {
                    /* There's a wait block, we need to wake the next pending
                    acquirer (exclusive or shared) */
                    WaitBlock = acquireWaitBlockLockImpl(SRWLock);
                    if (WaitBlock != NULL) {
                        releaseWaitBlockLockExclusiveImpl(SRWLock, WaitBlock);

                        /* We released the lock */
                        break;
                    }
                } else {
                    /* This is a fast path, we can simply clear the XP_SRWLOCK_OWNED
                    bit. All other bits should be 0 now because this is a simple
                    exclusive lock and no one is waiting. */

                    CV_ASSERT(!(CurrentValue & ~XP_SRWLOCK_OWNED));

                    NewValue = 0;
                    if ((LONG_PTR)InterlockedCompareExchangePointer(&SRWLock->Ptr,
                        (PVOID)NewValue,
                        (PVOID)CurrentValue) == CurrentValue) {
                        /* We released the lock */
                        break;
                    }
                }
            } else {
                /* The XP_SRWLOCK_SHARED bit must not be present now,
                not even in the contended case! */
                //RtlRaiseStatus(STATUS_RESOURCE_NOT_OWNED);
                DebugBreak();
            }

            YieldProcessor();
        }
    }
};

#define kCondVarLockedFlag ((ULONG_PTR)2)

class CondVarXp {
private:
    typedef struct _CondVarWaitEntry {
        int evt;
        struct _CondVarWaitEntry* nextPtr;
        struct _CondVarWaitEntry* lastPtr;
    } CondVarWaitEntry;

public:
    static PVOID lockConditionVar(PCONDITION_VARIABLE condVar)
    {
        PVOID ptr = condVar->Ptr;
        if (ptr == (PVOID)kCondVarLockedFlag)
            DebugBreak();

        ptr = (PVOID)((ULONG_PTR)ptr | kCondVarLockedFlag);
        while (true) {
            _InterlockedCompareExchange((long volatile *)&condVar->Ptr, (long)ptr, (long)((ULONG_PTR)condVar->Ptr & 0xfffffff0));
            if (condVar->Ptr == ptr)
                break;
            ::Sleep(50);
        }
        return ptr;
    }

    static void unlockCriticalSectionOrSRWLock(PCRITICAL_SECTION criticalSection, PSRWLOCK SRWLock, ULONG SRWFlags)
    {
        if (criticalSection == NULL) {
            if (0 == (RTL_CONDITION_VARIABLE_LOCKMODE_SHARED & SRWFlags)) {
                ::ReleaseSRWLockExclusive(SRWLock);
            } else {
                ::ReleaseSRWLockShared(SRWLock);
            }
        } else {
            ::LeaveCriticalSection(criticalSection);
        }
    }

    static void lockCriticalSectionOrSRWLock(PCRITICAL_SECTION criticalSection, PSRWLOCK SRWLock, ULONG SRWFlags)
    {
        if (criticalSection == NULL) {
            if (0 == (RTL_CONDITION_VARIABLE_LOCKMODE_SHARED & SRWFlags)) {
                ::AcquireSRWLockExclusive(SRWLock);
            } else {
                ::AcquireSRWLockShared(SRWLock);
            }
        } else {
            ::EnterCriticalSection(criticalSection);
        }
    }

    static BOOL waitCondVar(CondVarWaitEntry* entry, DWORD timeout)
    {
        const int waitOneTimeInMs = 50;
        DWORD count = 0;
        while (true) {
            count *= waitOneTimeInMs;
            if (0 != entry->evt)
                break;
            if (0xffffffff != timeout && count > timeout)
                return FALSE;
            ::Sleep(waitOneTimeInMs);
        }

        return TRUE;
    }

    static BOOL internalSleep(PCONDITION_VARIABLE condVar, PCRITICAL_SECTION criticalSection, PSRWLOCK SRWLock, ULONG SRWFlags, DWORD timeout)
    {
        CondVarWaitEntry* head = nullptr;
        PVOID ptr = lockConditionVar(condVar);

        ptr = (PVOID)((ULONG_PTR)ptr & 0xfffffff0);

        __declspec(align(16)) CondVarWaitEntry entry = { 0, nullptr, nullptr };
        if ((ULONG_PTR)(&entry) & 0xf)
            DebugBreak();

        if (ptr) {
            head = (CondVarWaitEntry*)ptr;
            head->lastPtr = &entry;
            entry.nextPtr = head;
        }

        _InterlockedExchange((long volatile *)&condVar->Ptr, (long)&entry);
        unlockCriticalSectionOrSRWLock(criticalSection, SRWLock, SRWFlags);

        if (!waitCondVar(&entry, timeout))
            return FALSE;

        lockCriticalSectionOrSRWLock(criticalSection, SRWLock, SRWFlags);

        ptr = lockConditionVar(condVar);
        ptr = (PVOID)((ULONG_PTR)ptr & 0xfffffff0);
        if (!ptr)
            DebugBreak();

        do {
            head = (CondVarWaitEntry*)ptr;
            if (head == &entry) {
                if (head->lastPtr)
                    head->lastPtr->nextPtr = head->nextPtr;
                if (head->nextPtr)
                    head->nextPtr->lastPtr = head->lastPtr;
                break;
            }
            head = head->nextPtr;
        } while (head);

        _InterlockedExchange((long volatile *)&condVar->Ptr, (long)ptr);

        return TRUE;
    }

    static void wakeAllConditionVariable(PCONDITION_VARIABLE condVar)
    {
        if (!condVar->Ptr)
            return;

        CondVarWaitEntry* head = nullptr;
        PVOID ptr = lockConditionVar(condVar);

        ptr = (PVOID)((ULONG_PTR)ptr & 0xfffffff0);
        if (!ptr)
            DebugBreak();

        do {
            head = (CondVarWaitEntry*)ptr;
            _InterlockedExchange((long volatile *)&head->evt, 1);
            head = head->nextPtr;
        } while (head);

        _InterlockedExchange((long volatile *)&condVar->Ptr, (long)ptr);
    }

    static void wakeConditionVariable(PCONDITION_VARIABLE condVar)
    {
        if (!condVar->Ptr)
            return;

        CondVarWaitEntry* head = nullptr;
        PVOID ptr = lockConditionVar(condVar);

        ptr = (PVOID)((ULONG_PTR)ptr & 0xfffffff0);
        if (!ptr)
            DebugBreak();

        head = (CondVarWaitEntry*)ptr;
        _InterlockedExchange((long volatile *)&head->evt, 1);

        _InterlockedExchange((long volatile *)&condVar->Ptr, (long)ptr);
    }
};

inline BOOL SleepConditionVariableCSXp(PCONDITION_VARIABLE condVar, PCRITICAL_SECTION criticalSection, DWORD timeout)
{
    return CondVarXp::internalSleep(condVar, criticalSection, nullptr, 0, timeout);
}

inline BOOL SleepConditionVariableSRWXp(PCONDITION_VARIABLE condVar, PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags)
{
    return CondVarXp::internalSleep(condVar, nullptr, SRWLock, Flags, dwMilliseconds);
}

void WakeAllConditionVariableXp(PCONDITION_VARIABLE condVar)
{
    CondVarXp::wakeAllConditionVariable(condVar);
}

inline void WakeConditionVariableXp(PCONDITION_VARIABLE condVar)
{
    CondVarXp::wakeConditionVariable(condVar);
}

inline void InitializeConditionVariableXp(PCONDITION_VARIABLE condVar)
{
    condVar->Ptr = NULL;
}

inline void AcquireSRWLockSharedXp(PRTL_SRWLOCK SRWLock)
{
    SlimReadWriteLock::acquireSRWLockShared(SRWLock);
}

inline void InitializeSRWLockXp(PRTL_SRWLOCK SRWLock)
{
    SRWLock->Ptr = NULL;
}

inline void ReleaseSRWLockExclusiveXp(PRTL_SRWLOCK SRWLock)
{
    SlimReadWriteLock::releaseSRWLockExclusive(SRWLock);
}

inline void AcquireSRWLockExclusiveXp(PRTL_SRWLOCK SRWLock)
{
    SlimReadWriteLock::acquireSRWLockExclusive(SRWLock);
}

inline void ReleaseSRWLockSharedXp(PRTL_SRWLOCK SRWLock)
{
    SlimReadWriteLock::releaseSRWLockShared(SRWLock);
}

#endif // ConditionVarXp_h