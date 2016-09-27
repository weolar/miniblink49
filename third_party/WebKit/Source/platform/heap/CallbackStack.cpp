// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/CallbackStack.h"

#include "platform/heap/Heap.h"

namespace blink {

void CallbackStack::Block::clear()
{
    m_current = &m_buffer[0];
    m_next = 0;
    clearUnused();
}

void CallbackStack::Block::invokeEphemeronCallbacks(Visitor* visitor)
{
    // This loop can tolerate entries being added by the callbacks after
    // iteration starts.
    for (unsigned i = 0; m_buffer + i < m_current; i++) {
        Item& item = m_buffer[i];
        item.call(visitor);
    }
}

#if ENABLE(ASSERT)
bool CallbackStack::Block::hasCallbackForObject(const void* object)
{
    for (unsigned i = 0; m_buffer + i < m_current; i++) {
        Item* item = &m_buffer[i];
        if (item->object() == object)
            return true;
    }
    return false;
}
#endif

void CallbackStack::Block::clearUnused()
{
#if ENABLE(ASSERT)
    for (size_t i = 0; i < blockSize; i++)
        m_buffer[i] = Item(0, 0);
#endif
}

CallbackStack::CallbackStack() : m_first(new Block(0)), m_last(m_first)
{
}

CallbackStack::~CallbackStack()
{
    clear();
    delete m_first;
    m_first = 0;
    m_last = 0;
}

void CallbackStack::clear()
{
    Block* next;
    for (Block* current = m_first->next(); current; current = next) {
        next = current->next();
        delete current;
    }
    m_first->clear();
    m_last = m_first;
}

bool CallbackStack::isEmpty() const
{
    return hasJustOneBlock() && m_first->isEmptyBlock();
}

CallbackStack::Item* CallbackStack::allocateEntrySlow()
{
    ASSERT(!m_first->allocateEntry());
    m_first = new Block(m_first);
    return m_first->allocateEntry();
}

CallbackStack::Item* CallbackStack::popSlow()
{
    ASSERT(m_first->isEmptyBlock());

    for (;;) {
        if (hasJustOneBlock()) {
#if ENABLE(ASSERT)
            m_first->clear();
#endif
            return 0;
        }
        Block* next = m_first->next();
        delete m_first;
        m_first = next;
        if (Item* item = m_first->pop())
            return item;
    }
}

void CallbackStack::invokeEphemeronCallbacks(Visitor* visitor)
{
    // The first block is the only one where new ephemerons are added, so we
    // call the callbacks on that last, to catch any new ephemerons discovered
    // in the callbacks.
    // However, if enough ephemerons were added, we may have a new block that
    // has been prepended to the chain. This will be very rare, but we can
    // handle the situation by starting again and calling all the callbacks
    // on the prepended blocks.
    Block* from = 0;
    Block* upto = 0;
    while (from != m_first) {
        upto = from;
        from = m_first;
        invokeOldestCallbacks(from, upto, visitor);
    }
}

void CallbackStack::invokeOldestCallbacks(Block* from, Block* upto, Visitor* visitor)
{
    if (from == upto)
        return;
    ASSERT(from);
    // Recurse first (blockSize at a time) so we get to the newly added entries last.
    invokeOldestCallbacks(from->next(), upto, visitor);
    from->invokeEphemeronCallbacks(visitor);
}

bool CallbackStack::hasJustOneBlock() const
{
    return !m_first->next();
}

void CallbackStack::swap(CallbackStack* other)
{
    Block* tmp = m_first;
    m_first = other->m_first;
    other->m_first = tmp;
    tmp = m_last;
    m_last = other->m_last;
    other->m_last = tmp;
}

#if ENABLE(ASSERT)
bool CallbackStack::hasCallbackForObject(const void* object)
{
    for (Block* current = m_first; current; current = current->next()) {
        if (current->hasCallbackForObject(object))
            return true;
    }
    return false;
}
#endif

}
