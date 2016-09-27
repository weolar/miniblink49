// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UnionTypesTest_h
#define UnionTypesTest_h

#include "bindings/core/v8/UnionTypesCore.h"
#include "wtf/text/WTFString.h"

namespace blink {

class UnionTypesTest final : public GarbageCollectedFinalized<UnionTypesTest>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static UnionTypesTest* create()
    {
        return new UnionTypesTest();
    }
    virtual ~UnionTypesTest() { }

    void doubleOrStringOrStringArrayAttribute(DoubleOrStringOrStringArray&);
    void setDoubleOrStringOrStringArrayAttribute(const DoubleOrStringOrStringArray&);

    String doubleOrStringArg(DoubleOrString&);
    String doubleOrInternalEnumArg(DoubleOrInternalEnum&);
    String doubleOrStringArrayArg(HeapVector<DoubleOrString>&);
    String doubleOrStringSequenceArg(HeapVector<DoubleOrString>&);

    String nodeListOrElementArg(NodeListOrElement&);
    String nodeListOrElementOrNullArg(NodeListOrElement&);

    String doubleOrStringOrStringArrayArg(const DoubleOrStringOrStringArray&);
    String doubleOrStringOrStringSequenceArg(const DoubleOrStringOrStringSequence&);

    DEFINE_INLINE_TRACE() { }

private:
    UnionTypesTest()
        : m_attributeType(SpecificTypeNone)
    {
    }

    enum AttributeSpecificType {
        SpecificTypeNone,
        SpecificTypeDouble,
        SpecificTypeString,
        SpecificTypeStringArray,
    };
    AttributeSpecificType m_attributeType;
    double m_attributeDouble;
    String m_attributeString;
    Vector<String> m_attributeStringArray;
};

} // namespace blink

#endif // UnionTypesTest_h
