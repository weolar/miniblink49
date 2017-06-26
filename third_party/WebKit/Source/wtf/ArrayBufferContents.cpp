/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "wtf/ArrayBufferContents.h"

#include "wtf/Assertions.h"
#include "wtf/PartitionAlloc.h"
#include "wtf/Partitions.h"
#include <string.h>

namespace WTF {

AdjustAmountOfExternalAllocatedMemoryFunction ArrayBufferContents::s_adjustAmountOfExternalAllocatedMemoryFunction;

ArrayBufferContents::ArrayBufferContents()
    : m_holder(adoptRef(new DataHolder())) { }

ArrayBufferContents::ArrayBufferContents(unsigned numElements, unsigned elementByteSize, SharingType isShared, ArrayBufferContents::InitializationPolicy policy)
    : m_holder(adoptRef(new DataHolder()))
{
    // Do not allow 32-bit overflow of the total size.
    unsigned totalSize = numElements * elementByteSize;
    if (numElements) {
        if (totalSize / numElements != elementByteSize) {
            return;
        }
    }

    m_holder->allocateNew(totalSize, isShared, policy);
}

ArrayBufferContents::ArrayBufferContents(
    void* data, unsigned sizeInBytes, SharingType isShared)
    : m_holder(adoptRef(new DataHolder()))
{
    if (data) {
        m_holder->adopt(data, sizeInBytes, isShared);
    } else {
        ASSERT(!sizeInBytes);
        sizeInBytes = 0;
        // Allow null data if size is 0 bytes, make sure data is valid pointer.
        // (PartitionAlloc guarantees valid pointer for size 0)
        m_holder->allocateNew(sizeInBytes, isShared, ZeroInitialize);
    }
}

ArrayBufferContents::~ArrayBufferContents()
{
}

void ArrayBufferContents::neuter()
{
    m_holder.clear();
}

void ArrayBufferContents::transfer(ArrayBufferContents& other)
{
    ASSERT(!isShared());
    ASSERT(!other.m_holder->data());
    other.m_holder = m_holder;
    neuter();
}

void ArrayBufferContents::shareWith(ArrayBufferContents& other)
{
    ASSERT(isShared());
    ASSERT(!other.m_holder->data());
    other.m_holder = m_holder;
}

void ArrayBufferContents::copyTo(ArrayBufferContents& other)
{
    ASSERT(!m_holder->isShared() && !other.m_holder->isShared());
    m_holder->copyMemoryTo(*other.m_holder);
}

struct MemoryHead {
    size_t magicNum;
    size_t size;

    static const size_t kMagicNum0 = 0x1122dd44;
    static const size_t kMagicNum1 = 0x11227788;

    static MemoryHead* getPointerHead(void* pointer) { return ((MemoryHead*)pointer) - 1; }
    static size_t getPointerMemSize(void* pointer) { return getPointerHead(pointer)->size; }
    static void* getHeadToMemBegin(MemoryHead* head) { return head + 1; }
};

void ArrayBufferContents::allocateMemory(size_t size, InitializationPolicy policy, void*& data)
{
    if (s_adjustAmountOfExternalAllocatedMemoryFunction)
        s_adjustAmountOfExternalAllocatedMemoryFunction(static_cast<int>(size));
#if 1
    MemoryHead* head = (MemoryHead*)partitionAllocGenericFlags(WTF::Partitions::bufferPartition(), PartitionAllocReturnNull, size + sizeof(MemoryHead));
    head->magicNum = MemoryHead::kMagicNum0;
    head->size = size;
    data = MemoryHead::getHeadToMemBegin(head);
#else
    data = partitionAllocGenericFlags(WTF::Partitions::bufferPartition(), PartitionAllocReturnNull, size);
#endif
    if (policy == ZeroInitialize && data)
        memset(data, '\0', size);
}

void ArrayBufferContents::freeMemory(void* data, size_t size)
{
#if 1
    if (!data || 0 == size)
        return;
    MemoryHead* head = MemoryHead::getPointerHead(data);
    if (head->magicNum != MemoryHead::kMagicNum0)
        DebugBreak();
    partitionFreeGeneric(WTF::Partitions::bufferPartition(), head);
#else
    partitionFreeGeneric(WTF::Partitions::bufferPartition(), data);
#endif
    if (s_adjustAmountOfExternalAllocatedMemoryFunction)
        s_adjustAmountOfExternalAllocatedMemoryFunction(-static_cast<int>(size));
}

ArrayBufferContents::DataHolder::DataHolder()
    : m_data(nullptr)
    , m_sizeInBytes(0)
    , m_isShared(NotShared) { }

ArrayBufferContents::DataHolder::~DataHolder()
{
    ArrayBufferContents::freeMemory(m_data, m_sizeInBytes);

    m_data = nullptr;
    m_sizeInBytes = 0;
    m_isShared = NotShared;
}

void ArrayBufferContents::DataHolder::allocateNew(unsigned sizeInBytes, SharingType isShared, InitializationPolicy policy)
{
    ASSERT(!m_data);
    void* data = nullptr;
    allocateMemory(sizeInBytes, policy, data);
    m_data = data;
    m_sizeInBytes = data ? sizeInBytes : 0;
    m_isShared = isShared;
}

void ArrayBufferContents::DataHolder::adopt(void* data, unsigned sizeInBytes, SharingType isShared)
{
    ASSERT(!m_data);
    m_data = data;
    m_sizeInBytes = sizeInBytes;
    m_isShared = isShared;
}

void ArrayBufferContents::DataHolder::copyMemoryTo(DataHolder& other)
{
    ASSERT(!other.m_sizeInBytes);
    ArrayBufferContents::freeMemory(other.m_data, other.m_sizeInBytes);
    ArrayBufferContents::allocateMemory(m_sizeInBytes, DontInitialize, other.m_data);
    if (!other.m_data)
        return;
    memcpy(other.m_data, m_data, m_sizeInBytes);
    other.m_sizeInBytes = m_sizeInBytes;
}

} // namespace WTF
