// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RefCountedScriptWrappable_h
#define RefCountedScriptWrappable_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class RefCountedScriptWrappable : public RefCounted<RefCountedScriptWrappable>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtr<RefCountedScriptWrappable> create(const String&);
    virtual ~RefCountedScriptWrappable();

    const String& toString() const { return m_string; }

protected:
    RefCountedScriptWrappable(const String&);

private:
    String m_string;
};

} // namespace blink

#endif // RefCountedScriptWrappable_h
