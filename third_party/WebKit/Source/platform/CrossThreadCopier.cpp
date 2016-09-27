/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "config.h"

#include "platform/CrossThreadCopier.h"

#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

CrossThreadCopierBase<false, false, false, KURL>::Type CrossThreadCopierBase<false, false, false, KURL>::copy(const KURL& url)
{
    return url.copy();
}

CrossThreadCopierBase<false, false, false, String>::Type CrossThreadCopierBase<false, false, false, String>::copy(const String& str)
{
    return str.isolatedCopy();
}

CrossThreadCopierBase<false, false, false, ResourceError>::Type CrossThreadCopierBase<false, false, false, ResourceError>::copy(const ResourceError& error)
{
    return error.copy();
}

CrossThreadCopierBase<false, false, false, ResourceRequest>::Type CrossThreadCopierBase<false, false, false, ResourceRequest>::copy(const ResourceRequest& request)
{
    return request.copyData();
}

CrossThreadCopierBase<false, false, false, ResourceResponse>::Type CrossThreadCopierBase<false, false, false, ResourceResponse>::copy(const ResourceResponse& response)
{
    return response.copyData();
}

// Test CrossThreadCopier using static_assert.

// Verify that ThreadSafeRefCounted objects get handled correctly.
class CopierThreadSafeRefCountedTest : public ThreadSafeRefCounted<CopierThreadSafeRefCountedTest> {
};

static_assert((WTF::IsSameType<
    PassRefPtr<CopierThreadSafeRefCountedTest>,
    CrossThreadCopier<PassRefPtr<CopierThreadSafeRefCountedTest>>::Type
    >::value),
    "PassRefPtr test");
static_assert((WTF::IsSameType<
    PassRefPtr<CopierThreadSafeRefCountedTest>,
    CrossThreadCopier<RefPtr<CopierThreadSafeRefCountedTest>>::Type
    >::value),
    "RefPtr test");
static_assert((WTF::IsSameType<
    PassRefPtr<CopierThreadSafeRefCountedTest>,
    CrossThreadCopier<CopierThreadSafeRefCountedTest*>::Type
    >::value),
    "RawPointer test");


// Add a generic specialization which will let's us verify that no other template matches.
template<typename T> struct CrossThreadCopierBase<false, false, false, T> {
    typedef int Type;
};

// Verify that RefCounted objects only match our generic template which exposes Type as int.
class CopierRefCountedTest : public RefCounted<CopierRefCountedTest> {
};

static_assert((WTF::IsSameType<
    int,
    CrossThreadCopier<PassRefPtr<CopierRefCountedTest>>::Type
    >::value),
    "PassRefPtr<RefCountedTest> test");

static_assert((WTF::IsSameType<
    int,
    CrossThreadCopier<RefPtr<CopierRefCountedTest>>::Type
    >::value),
    "RefPtr<RefCounted> test");

static_assert((WTF::IsSameType<
    int,
    CrossThreadCopier<CopierRefCountedTest*>::Type
    >::value),
    "Raw pointer RefCounted test");

// Verify that PassOwnPtr gets passed through.
static_assert((WTF::IsSameType<
    PassOwnPtr<float>,
    CrossThreadCopier<PassOwnPtr<float>>::Type
    >::value),
    "PassOwnPtr test");

// Verify that PassOwnPtr does not get passed through.
static_assert((WTF::IsSameType<
    int,
    CrossThreadCopier<OwnPtr<float>>::Type
    >::value),
    "OwnPtr test");

} // namespace blink
