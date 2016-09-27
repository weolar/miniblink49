// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaKeySystemAccess_h
#define MediaKeySystemAccess_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/encryptedmedia/MediaKeySystemConfiguration.h"
#include "public/platform/WebContentDecryptionModuleAccess.h"
#include "wtf/Forward.h"

namespace blink {

class MediaKeySystemAccess final : public GarbageCollectedFinalized<MediaKeySystemAccess>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();

public:
    MediaKeySystemAccess(const String& keySystem, PassOwnPtr<WebContentDecryptionModuleAccess>);
    virtual ~MediaKeySystemAccess();

    const String& keySystem() const { return m_keySystem; }
    void getConfiguration(MediaKeySystemConfiguration& result);
    ScriptPromise createMediaKeys(ScriptState*);

    DECLARE_TRACE();

private:
    const String m_keySystem;
    OwnPtr<WebContentDecryptionModuleAccess> m_access;
};

} // namespace blink

#endif // MediaKeySystemAccess_h
