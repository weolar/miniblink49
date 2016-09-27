/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WorkerNavigator_h
#define WorkerNavigator_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/frame/NavigatorCPU.h"
#include "core/frame/NavigatorID.h"
#include "core/frame/NavigatorOnLine.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class WorkerNavigator final : public GarbageCollectedFinalized<WorkerNavigator>, public ScriptWrappable, public NavigatorCPU, public NavigatorID, public NavigatorOnLine, public HeapSupplementable<WorkerNavigator> {
    DEFINE_WRAPPERTYPEINFO();
    USING_GARBAGE_COLLECTED_MIXIN(WorkerNavigator);
public:
    static WorkerNavigator* create(const String& userAgent)
    {
        return new WorkerNavigator(userAgent);
    }
    virtual ~WorkerNavigator();

    virtual String userAgent() const override;

    DECLARE_TRACE();

private:
    explicit WorkerNavigator(const String&);

    String m_userAgent;
};

} // namespace blink

#endif // WorkerNavigator_h
