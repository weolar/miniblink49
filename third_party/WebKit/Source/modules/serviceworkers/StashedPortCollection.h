// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StashedPortCollection_h
#define StashedPortCollection_h

#include "core/dom/ContextLifecycleObserver.h"
#include "core/events/EventTarget.h"
#include "modules/ModulesExport.h"
#include "modules/serviceworkers/StashedMessagePort.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {
class ScriptState;

class MODULES_EXPORT StashedPortCollection final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<StashedPortCollection>
    , public ContextLifecycleObserver {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(StashedPortCollection);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(StashedPortCollection);
    WTF_MAKE_NONCOPYABLE(StashedPortCollection);
public:
    static StashedPortCollection* create(ExecutionContext*);
    ~StashedPortCollection() override;

    void addPorts(const StashedMessagePortArray&);

    // StashedPortCollection.idl
    StashedMessagePort* add(ScriptState*, const String& name, MessagePort*);

    // EventTarget overrides.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit StashedPortCollection(ExecutionContext*);

    StashedMessagePortArray m_ports;
};

} // namespace blink

#endif // StashedPortCollection_h
