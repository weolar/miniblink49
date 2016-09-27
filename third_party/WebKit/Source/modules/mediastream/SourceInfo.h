/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SourceInfo_h
#define SourceInfo_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "public/platform/WebSourceInfo.h"
#include "wtf/Vector.h"

namespace blink {

class SourceInfo : public GarbageCollectedFinalized<SourceInfo>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SourceInfo* create(const WebSourceInfo&);

    String id() const;
    String kind() const;
    String label() const;
    String facing() const;

    DEFINE_INLINE_TRACE() { }

private:
    explicit SourceInfo(const WebSourceInfo&);

    WebSourceInfo m_webSourceInfo;
};

typedef HeapVector<Member<SourceInfo>> SourceInfoVector;

} // namespace blink

#endif // SourceInfo_h
