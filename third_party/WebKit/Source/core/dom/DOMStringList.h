/*
 * Copyright (C) 2010 Google Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DOMStringList_h
#define DOMStringList_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;

// FIXME: Some consumers of this class may benefit from lazily fetching items rather
//        than creating the list statically as is currently the only option.
class CORE_EXPORT DOMStringList final : public RefCountedWillBeGarbageCollectedFinalized<DOMStringList>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    // We would like to remove DOMStringList from the platform if possible.
    // Track the source of each instance so we can measure the use of methods
    // not present on Arrays and determine the feasibility of removal and
    // what path it should take. http://crbug.com/460726
    enum Source { IndexedDB, Location };

    static PassRefPtrWillBeRawPtr<DOMStringList> create(Source source)
    {
        return adoptRefWillBeNoop(new DOMStringList(source));
    }

    bool isEmpty() const { return m_strings.isEmpty(); }
    void clear() { m_strings.clear(); }
    void append(const String& string) { m_strings.append(string); }
    void sort();

    // Implements the IDL.
    size_t length() const { return m_strings.size(); }
    String anonymousIndexedGetter(unsigned index) const;

    String item(ExecutionContext*, unsigned index) const;
    bool contains(ExecutionContext*, const String&) const;

    operator const Vector<String>&() const { return m_strings; }

    DEFINE_INLINE_TRACE() { }

private:
    explicit DOMStringList(Source source) : m_source(source) { }

    Vector<String> m_strings;
    Source m_source;
};

} // namespace blink

#endif // DOMStringList_h
