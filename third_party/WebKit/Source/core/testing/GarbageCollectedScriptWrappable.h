// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GarbageCollectedScriptWrappable_h
#define GarbageCollectedScriptWrappable_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Heap.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class GarbageCollectedScriptWrappable : public GarbageCollectedFinalized<GarbageCollectedScriptWrappable>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    GarbageCollectedScriptWrappable(const String&);
    virtual ~GarbageCollectedScriptWrappable();

    const String& toString() const { return m_string; }
    DEFINE_INLINE_VIRTUAL_TRACE() { }

private:
    String m_string;
};

} // namespace blink

#endif // GarbageCollectedScriptWrappable_h
