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

#ifndef Storage_h
#define Storage_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/frame/DOMWindowProperty.h"
#include "modules/storage/StorageArea.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefPtr.h"

namespace blink {

class ExceptionState;
class LocalFrame;

class Storage final : public GarbageCollectedFinalized<Storage>, public ScriptWrappable, public DOMWindowProperty {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Storage);
public:
    static Storage* create(LocalFrame*, StorageArea*);
    virtual ~Storage();

    unsigned length(ExceptionState& ec) const { return m_storageArea->length(ec, m_frame); }
    String key(unsigned index, ExceptionState& ec) const { return m_storageArea->key(index, ec, m_frame); }
    String getItem(const String& key, ExceptionState& ec) const { return m_storageArea->getItem(key, ec, m_frame); }
    void setItem(const String& key, const String& value, ExceptionState& ec) { m_storageArea->setItem(key, value, ec, m_frame); }
    void removeItem(const String& key, ExceptionState& ec) { m_storageArea->removeItem(key, ec, m_frame); }
    void clear(ExceptionState& ec) { m_storageArea->clear(ec, m_frame); }
    bool contains(const String& key, ExceptionState& ec) const { return m_storageArea->contains(key, ec, m_frame); }

    StorageArea* area() const { return m_storageArea.get(); }

    String anonymousIndexedGetter(unsigned, ExceptionState&);
    String anonymousNamedGetter(const AtomicString&, ExceptionState&);
    bool anonymousNamedSetter(const AtomicString& name, const AtomicString& value, ExceptionState&);
    bool anonymousIndexedSetter(unsigned, const AtomicString&, ExceptionState&);
    DeleteResult anonymousNamedDeleter(const AtomicString&, ExceptionState&);
    DeleteResult anonymousIndexedDeleter(unsigned, ExceptionState&);
    void namedPropertyEnumerator(Vector<String>&, ExceptionState&);
    bool namedPropertyQuery(const AtomicString&, ExceptionState&);

    DECLARE_VIRTUAL_TRACE();

private:
    Storage(LocalFrame*, StorageArea*);

    Member<StorageArea> m_storageArea;
};

} // namespace blink

#endif // Storage_h
