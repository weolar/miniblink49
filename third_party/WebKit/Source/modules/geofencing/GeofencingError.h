// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeofencingError_h
#define GeofencingError_h

#include "platform/heap/Handle.h"
#include "public/platform/WebGeofencingError.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class DOMException;
class ScriptPromiseResolver;

class GeofencingError {
    WTF_MAKE_NONCOPYABLE(GeofencingError);
public:
    // For CallbackPromiseAdapter.
    typedef WebGeofencingError WebType;
    static DOMException* take(ScriptPromiseResolver*, PassOwnPtr<WebType> webError);

private:
    GeofencingError() = delete;
};

} // namespace blink

#endif // GeofencingError_h
