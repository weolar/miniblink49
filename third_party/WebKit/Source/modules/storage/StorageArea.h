/*
 * Copyright (C) 2009 Google Inc. All Rights Reserved.
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

#ifndef StorageArea_h
#define StorageArea_h

#include "core/frame/LocalFrameLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class LocalFrame;
class KURL;
class SecurityOrigin;
class Storage;
class WebStorageArea;
class WebStorageNamespace;

enum StorageType {
    LocalStorage,
    SessionStorage
};

class MODULES_EXPORT StorageArea final : public GarbageCollectedFinalized<StorageArea>, public LocalFrameLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(StorageArea);
public:
    static StorageArea* create(PassOwnPtr<WebStorageArea>, StorageType);

    virtual ~StorageArea();

    // The HTML5 DOM Storage API
    unsigned length(ExceptionState&, LocalFrame* sourceFrame);
    String key(unsigned index, ExceptionState&, LocalFrame* sourceFrame);
    String getItem(const String& key, ExceptionState&, LocalFrame* sourceFrame);
    void setItem(const String& key, const String& value, ExceptionState&, LocalFrame* sourceFrame);
    void removeItem(const String& key, ExceptionState&, LocalFrame* sourceFrame);
    void clear(ExceptionState&, LocalFrame* sourceFrame);
    bool contains(const String& key, ExceptionState&, LocalFrame* sourceFrame);

    bool canAccessStorage(LocalFrame*);
    size_t memoryBytesUsedByCache();

    static void dispatchLocalStorageEvent(const String& key, const String& oldValue, const String& newValue,
        SecurityOrigin*, const KURL& pageURL, WebStorageArea* sourceAreaInstance, bool originatedInProcess);
    static void dispatchSessionStorageEvent(const String& key, const String& oldValue, const String& newValue,
        SecurityOrigin*, const KURL& pageURL, const WebStorageNamespace&,
        WebStorageArea* sourceAreaInstance, bool originatedInProcess);

    DECLARE_TRACE();

private:
    StorageArea(PassOwnPtr<WebStorageArea>, StorageType);

    static bool isEventSource(Storage*, WebStorageArea* sourceAreaInstance);

    OwnPtr<WebStorageArea> m_storageArea;
    StorageType m_storageType;
    bool m_canAccessStorageCachedResult;
};

} // namespace blink

#endif // StorageArea_h
