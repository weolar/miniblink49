// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FrameOwner_h
#define FrameOwner_h

#include "core/CoreExport.h"
#include "core/dom/SandboxFlags.h"
#include "platform/heap/Handle.h"

namespace blink {

// Oilpan: all FrameOwner instances are GCed objects. FrameOwner additionally
// derives from GarbageCollectedMixin so that Member<FrameOwner> references can
// be kept (e.g., Frame::m_owner.)
class CORE_EXPORT FrameOwner : public WillBeGarbageCollectedMixin {
public:
    virtual ~FrameOwner() { }
    DEFINE_INLINE_VIRTUAL_TRACE() { }

    virtual bool isLocal() const = 0;

    virtual SandboxFlags sandboxFlags() const = 0;
    virtual void dispatchLoad() = 0;

    // On load failure, a frame can ask its owner to render fallback content
    // which replaces the frame contents.
    virtual void renderFallbackContent() = 0;
};

} // namespace blink

#endif // FrameOwner_h
