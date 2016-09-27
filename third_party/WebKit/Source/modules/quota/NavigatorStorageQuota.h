/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NavigatorStorageQuota_h
#define NavigatorStorageQuota_h

#include "core/frame/DOMWindowProperty.h"
#include "modules/quota/DeprecatedStorageQuota.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class LocalFrame;
class Navigator;
class StorageQuota;

class NavigatorStorageQuota final : public GarbageCollectedFinalized<NavigatorStorageQuota>, public HeapSupplement<Navigator>, public DOMWindowProperty {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorStorageQuota);
public:
    static NavigatorStorageQuota& from(Navigator&);
    virtual ~NavigatorStorageQuota();

    static StorageQuota* storageQuota(Navigator&);
    static DeprecatedStorageQuota* webkitTemporaryStorage(Navigator&);
    static DeprecatedStorageQuota* webkitPersistentStorage(Navigator&);

    StorageQuota* storageQuota() const;
    DeprecatedStorageQuota* webkitTemporaryStorage() const;
    DeprecatedStorageQuota* webkitPersistentStorage() const;

    DECLARE_TRACE();

private:
    explicit NavigatorStorageQuota(LocalFrame*);
    static const char* supplementName();

    mutable Member<StorageQuota> m_storageQuota;
    mutable Member<DeprecatedStorageQuota> m_temporaryStorage;
    mutable Member<DeprecatedStorageQuota> m_persistentStorage;
};

} // namespace blink

#endif // NavigatorStorageQuota_h
