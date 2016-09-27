// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PrivateScriptTest_h
#define PrivateScriptTest_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;

class PrivateScriptTest : public GarbageCollectedFinalized<PrivateScriptTest>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PrivateScriptTest* create(Document* document)
    {
        return new PrivateScriptTest(document);
    }

    int addIntegerImplementedInCPPForPrivateScriptOnly(int value1, int value2);
    String stringAttributeImplementedInCPPForPrivateScriptOnly();
    void setStringAttributeImplementedInCPPForPrivateScriptOnly(String);

    DEFINE_INLINE_TRACE() { }

private:
    explicit PrivateScriptTest(Document*);

    String m_stringAttributeImplementedInCPPForPrivateSriptOnly;
};

} // namespace blink

#endif // PrivateScriptTest_h
