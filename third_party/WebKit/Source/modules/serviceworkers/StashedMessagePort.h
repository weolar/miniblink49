// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StashedMessagePort_h
#define StashedMessagePort_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/MessagePort.h"
#include "modules/ModulesExport.h"

namespace blink {
class ExecutionContext;
class StashedMessagePort;

using StashedMessagePortArray = HeapVector<Member<StashedMessagePort>, 1>;

// Represents a message port that has been stashed. Overrides messageAvailable
// to dispatch messages as a global event instead of as events on this port.
class MODULES_EXPORT StashedMessagePort final : public MessagePort {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(StashedMessagePort);
public:
    static StashedMessagePort* create(ExecutionContext&, PassOwnPtr<WebMessagePortChannel>, const String& name);
    ~StashedMessagePort() override;

    static StashedMessagePortArray* toStashedMessagePortArray(ExecutionContext*, const WebMessagePortChannelArray&, const WebVector<WebString>& channelKeys);

    // StashedMessagePort.idl
    String name() const;

    DEFINE_INLINE_VIRTUAL_TRACE() { MessagePort::trace(visitor); }

private:
    StashedMessagePort(ExecutionContext&, PassOwnPtr<WebMessagePortChannel>, const String& name);

    void messageAvailable() override;
    void dispatchMessages();

    String m_name;
    WeakPtrFactory<StashedMessagePort> m_weakFactory;
};

} // namespace blink

#endif // StashedMessagePort_h
