/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/indexeddb/IDBAny.h"

#include "core/dom/DOMStringList.h"
#include "modules/indexeddb/IDBCursorWithValue.h"
#include "modules/indexeddb/IDBDatabase.h"
#include "modules/indexeddb/IDBIndex.h"
#include "modules/indexeddb/IDBObjectStore.h"

namespace blink {

IDBAny* IDBAny::createUndefined()
{
    return new IDBAny(UndefinedType);
}

IDBAny* IDBAny::createNull()
{
    return new IDBAny(NullType);
}

IDBAny::IDBAny(Type type)
    : m_type(type)
{
    ASSERT(type == UndefinedType || type == NullType);
}

IDBAny::~IDBAny()
{
}

void IDBAny::contextWillBeDestroyed()
{
    if (m_idbCursor)
        m_idbCursor->contextWillBeDestroyed();
}

DOMStringList* IDBAny::domStringList() const
{
    ASSERT(m_type == DOMStringListType);
    return m_domStringList.get();
}

IDBCursor* IDBAny::idbCursor() const
{
    ASSERT(m_type == IDBCursorType);
    ASSERT_WITH_SECURITY_IMPLICATION(m_idbCursor->isKeyCursor());
    return m_idbCursor.get();
}

IDBCursorWithValue* IDBAny::idbCursorWithValue() const
{
    ASSERT(m_type == IDBCursorWithValueType);
    ASSERT_WITH_SECURITY_IMPLICATION(m_idbCursor->isCursorWithValue());
    return toIDBCursorWithValue(m_idbCursor.get());
}

IDBDatabase* IDBAny::idbDatabase() const
{
    ASSERT(m_type == IDBDatabaseType);
    return m_idbDatabase.get();
}

IDBIndex* IDBAny::idbIndex() const
{
    ASSERT(m_type == IDBIndexType);
    return m_idbIndex.get();
}

IDBObjectStore* IDBAny::idbObjectStore() const
{
    ASSERT(m_type == IDBObjectStoreType);
    return m_idbObjectStore.get();
}

const IDBKey* IDBAny::key() const
{
    // If type is IDBValueType then instead use value()->primaryKey().
    ASSERT(m_type == KeyType);
    return m_idbKey.get();
}

IDBValue* IDBAny::value() const
{
    ASSERT(m_type == IDBValueType);
    return m_idbValue.get();
}

const Vector<RefPtr<IDBValue>>* IDBAny::values() const
{
    ASSERT(m_type == IDBValueArrayType);
    return &m_idbValues;
}

int64_t IDBAny::integer() const
{
    ASSERT(m_type == IntegerType);
    return m_integer;
}

IDBAny::IDBAny(PassRefPtrWillBeRawPtr<DOMStringList> value)
    : m_type(DOMStringListType)
    , m_domStringList(value)
{
}

IDBAny::IDBAny(IDBCursor* value)
    : m_type(value->isCursorWithValue() ? IDBCursorWithValueType : IDBCursorType)
    , m_idbCursor(value)
{
}

IDBAny::IDBAny(IDBDatabase* value)
    : m_type(IDBDatabaseType)
    , m_idbDatabase(value)
{
}

IDBAny::IDBAny(IDBIndex* value)
    : m_type(IDBIndexType)
    , m_idbIndex(value)
{
}

IDBAny::IDBAny(IDBObjectStore* value)
    : m_type(IDBObjectStoreType)
    , m_idbObjectStore(value)
{
}

IDBAny::IDBAny(const Vector<RefPtr<IDBValue>>& values)
    : m_type(IDBValueArrayType)
    , m_idbValues(values)
{
}

IDBAny::IDBAny(PassRefPtr<IDBValue> value)
    : m_type(IDBValueType)
    , m_idbValue(value)
{
}

IDBAny::IDBAny(IDBKey* key)
    : m_type(KeyType)
    , m_idbKey(key)
{
}

IDBAny::IDBAny(int64_t value)
    : m_type(IntegerType)
    , m_integer(value)
{
}

DEFINE_TRACE(IDBAny)
{
    visitor->trace(m_domStringList);
    visitor->trace(m_idbCursor);
    visitor->trace(m_idbDatabase);
    visitor->trace(m_idbIndex);
    visitor->trace(m_idbObjectStore);
    visitor->trace(m_idbKey);
}

} // namespace blink
