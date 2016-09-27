// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeofencingRegion_h
#define GeofencingRegion_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class MODULES_EXPORT GeofencingRegion : public GarbageCollectedFinalized<GeofencingRegion>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(GeofencingRegion);
public:
    virtual ~GeofencingRegion() { }

    String id() const { return m_id; }

    DEFINE_INLINE_VIRTUAL_TRACE() { }

    // For SpecialWrapFor
    virtual bool isCircularGeofencingRegion() const { return false; }

protected:
    explicit GeofencingRegion(const String& id) : m_id(id) { }

private:
    String m_id;
};

} // namespace blink

#endif // GeofencingRegion_h
