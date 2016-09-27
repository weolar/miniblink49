/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef IDBKey_h
#define IDBKey_h

#include "modules/ModulesExport.h"
#include "platform/SharedBuffer.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class MODULES_EXPORT IDBKey : public GarbageCollectedFinalized<IDBKey> {
public:
    typedef HeapVector<Member<IDBKey>> KeyArray;

    static IDBKey* createInvalid()
    {
        return new IDBKey();
    }

    static IDBKey* createNumber(double number)
    {
        return new IDBKey(NumberType, number);
    }

    static IDBKey* createBinary(PassRefPtr<SharedBuffer> binary)
    {
        return new IDBKey(binary);
    }

    static IDBKey* createString(const String& string)
    {
        return new IDBKey(string);
    }

    static IDBKey* createDate(double date)
    {
        return new IDBKey(DateType, date);
    }

    static IDBKey* createMultiEntryArray(const KeyArray& array)
    {
        KeyArray result;

        for (size_t i = 0; i < array.size(); i++) {
            if (!array[i]->isValid())
                continue;

            bool skip = false;
            for (size_t j = 0; j < result.size(); j++) {
                if (array[i]->isEqual(result[j].get())) {
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                result.append(array[i]);
            }
        }
        IDBKey* idbKey = new IDBKey(result);
        ASSERT(idbKey->isValid());
        return idbKey;
    }

    static IDBKey* createArray(const KeyArray& array)
    {
        return new IDBKey(array);
    }

    ~IDBKey();
    DECLARE_TRACE();

    // In order of the least to the highest precedent in terms of sort order.
    enum Type {
        InvalidType = 0,
        ArrayType,
        BinaryType,
        StringType,
        DateType,
        NumberType,
        MinType
    };

    Type type() const { return m_type; }
    bool isValid() const;

    const KeyArray& array() const
    {
        ASSERT(m_type == ArrayType);
        return m_array;
    }

    PassRefPtr<SharedBuffer> binary() const
    {
        ASSERT(m_type == BinaryType);
        return m_binary;
    }

    const String& string() const
    {
        ASSERT(m_type == StringType);
        return m_string;
    }

    double date() const
    {
        ASSERT(m_type == DateType);
        return m_number;
    }

    double number() const
    {
        ASSERT(m_type == NumberType);
        return m_number;
    }

    int compare(const IDBKey* other) const;
    bool isLessThan(const IDBKey* other) const;
    bool isEqual(const IDBKey* other) const;

private:
    IDBKey() : m_type(InvalidType) { }
    IDBKey(Type type, double number) : m_type(type), m_number(number) { }
    explicit IDBKey(const String& value) : m_type(StringType), m_string(value) { }
    explicit IDBKey(PassRefPtr<SharedBuffer> value) : m_type(BinaryType), m_binary(value) { }
    explicit IDBKey(const KeyArray& keyArray) : m_type(ArrayType), m_array(keyArray) { }

    const Type m_type;
    const KeyArray m_array;
    RefPtr<SharedBuffer> m_binary;
    const String m_string;
    const double m_number = 0;
};

} // namespace blink

#endif // IDBKey_h
