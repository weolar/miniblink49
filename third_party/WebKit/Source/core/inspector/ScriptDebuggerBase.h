// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScriptDebuggerBase_h
#define ScriptDebuggerBase_h

#include "core/CoreExport.h"
#include "core/inspector/V8Debugger.h"
#include "platform/heap/Handle.h"

namespace blink {

class CORE_EXPORT ScriptDebuggerBase : public V8Debugger::Client {
    WTF_MAKE_NONCOPYABLE(ScriptDebuggerBase);
public:
    ScriptDebuggerBase(v8::Isolate*, PassOwnPtrWillBeRawPtr<V8Debugger>);
    ~ScriptDebuggerBase() override;
    v8::Local<v8::Object> compileDebuggerScript() override;
    V8Debugger* debugger() const { return m_debugger.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    v8::Isolate* m_isolate;
    OwnPtrWillBeMember<V8Debugger> m_debugger;
};

} // namespace blink


#endif // !defined(ScriptDebuggerBase_h)
