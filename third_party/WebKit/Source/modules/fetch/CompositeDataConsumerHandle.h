// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CompositeDataConsumerHandle_h
#define CompositeDataConsumerHandle_h

#include "modules/ModulesExport.h"
#include "public/platform/WebDataConsumerHandle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

// This is a utility class to construct a composite data consumer handle. It
// owns a web data consumer handle and delegates methods. A user can update
// the handle by using |update| method.
class MODULES_EXPORT CompositeDataConsumerHandle final : public WebDataConsumerHandle {
    WTF_MAKE_NONCOPYABLE(CompositeDataConsumerHandle);
public:
    // |handle| must not be null and must not be locked.
    static PassOwnPtr<CompositeDataConsumerHandle> create(PassOwnPtr<WebDataConsumerHandle> handle)
    {
        ASSERT(handle);
        return adoptPtr(new CompositeDataConsumerHandle(handle));
    }
    ~CompositeDataConsumerHandle() override;

    // This function should be called on the thread on which this object
    // was created. |handle| must not be null and must not be locked.
    void update(PassOwnPtr<WebDataConsumerHandle> /* handle */);

private:
    class Context;
    class ReaderImpl;
    Reader* obtainReaderInternal(Client*) override;

    const char* debugName() const override { return "CompositeDataConsumerHandle"; }

    explicit CompositeDataConsumerHandle(PassOwnPtr<WebDataConsumerHandle>);

    RefPtr<Context> m_context;
};

} // namespace blink

#endif // CompositeDataConsumerHandle_h
