// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchManager_h
#define FetchManager_h

#include "bindings/core/v8/ScriptPromise.h"
#include "wtf/HashSet.h"
#include "wtf/OwnPtr.h"

namespace blink {

class ExecutionContext;
class FetchRequestData;
class ScriptState;

class FetchManager final : public NoBaseWillBeGarbageCollectedFinalized<FetchManager> {
public:
    static PassOwnPtrWillBeRawPtr<FetchManager> create(ExecutionContext* executionContext)
    {
        return adoptPtrWillBeNoop(new FetchManager(executionContext));
    }
    ~FetchManager();
    ScriptPromise fetch(ScriptState*, FetchRequestData*);
    void stop();
    bool isStopped() const { return m_isStopped; }

    DECLARE_TRACE();

private:
    class Loader;

    explicit FetchManager(ExecutionContext*);
    // Removes loader from |m_loaders|.
    void onLoaderFinished(Loader*);

    RawPtrWillBeMember<ExecutionContext> m_executionContext;
    WillBeHeapHashSet<OwnPtrWillBeMember<Loader>> m_loaders;
    bool m_isStopped;
};

} // namespace blink

#endif // FetchManager_h
