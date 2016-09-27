// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CallbackStack_h
#define CallbackStack_h

#include "platform/heap/ThreadState.h"

namespace blink {

// The CallbackStack contains all the visitor callbacks used to trace and mark
// objects. A specific CallbackStack instance contains at most bufferSize elements.
// If more space is needed a new CallbackStack instance is created and chained
// together with the former instance. I.e. a logical CallbackStack can be made of
// multiple chained CallbackStack object instances.
class CallbackStack {
public:
    class Item {
    public:
        Item() { }
        Item(void* object, VisitorCallback callback)
            : m_object(object)
            , m_callback(callback)
        {
        }
        void* object() { return m_object; }
        VisitorCallback callback() { return m_callback; }
        void call(Visitor* visitor) { m_callback(visitor, m_object); }

    private:
        void* m_object;
        VisitorCallback m_callback;
    };

    CallbackStack();
    ~CallbackStack();

    void clear();

    Item* allocateEntry();
    Item* pop();

    bool isEmpty() const;

    void invokeEphemeronCallbacks(Visitor*);

#if ENABLE(ASSERT)
    bool hasCallbackForObject(const void*);
#endif

    static const size_t blockSize = 8192;

private:
    class Block {
    public:
        explicit Block(Block* next)
            : m_limit(&(m_buffer[blockSize]))
            , m_current(&(m_buffer[0]))
            , m_next(next)
        {
            clearUnused();
        }

        ~Block()
        {
            clearUnused();
        }

        void clear();

        Block* next() const { return m_next; }
        void setNext(Block* next) { m_next = next; }

        bool isEmptyBlock() const
        {
            return m_current == &(m_buffer[0]);
        }

        size_t size() const
        {
            return blockSize - (m_limit - m_current);
        }

        Item* allocateEntry()
        {
            if (LIKELY(m_current < m_limit))
                return m_current++;
            return 0;
        }

        Item* pop()
        {
            if (UNLIKELY(isEmptyBlock()))
                return 0;
            return --m_current;
        }

        void invokeEphemeronCallbacks(Visitor*);
#if ENABLE(ASSERT)
        bool hasCallbackForObject(const void*);
#endif

    private:
        void clearUnused();

        Item m_buffer[blockSize];
        Item* m_limit;
        Item* m_current;
        Block* m_next;
    };

    Item* popSlow();
    Item* allocateEntrySlow();
    void invokeOldestCallbacks(Block*, Block*, Visitor*);
    bool hasJustOneBlock() const;
    void swap(CallbackStack* other);

    Block* m_first;
    Block* m_last;
};

ALWAYS_INLINE CallbackStack::Item* CallbackStack::allocateEntry()
{
    Item* item = m_first->allocateEntry();
    if (LIKELY(!!item))
        return item;

    return allocateEntrySlow();
}

ALWAYS_INLINE CallbackStack::Item* CallbackStack::pop()
{
    Item* item = m_first->pop();
    if (LIKELY(!!item))
        return item;

    return popSlow();
}

} // namespace blink

#endif // CallbackStack_h
