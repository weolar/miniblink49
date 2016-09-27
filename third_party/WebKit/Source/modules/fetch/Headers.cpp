// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/Headers.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8IteratorResultValue.h"
#include "core/dom/Iterator.h"
#include "core/fetch/FetchUtils.h"
#include "wtf/NotFound.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

class HeadersIterationSource final : public PairIterable<String, String>::IterationSource {
public:
    explicit HeadersIterationSource(FetchHeaderList* headers) : m_headers(headers), m_current(0) { }

    bool next(ScriptState* scriptState, String& key, String& value, ExceptionState& exception) override
    {
        // FIXME: This simply advances an index and returns the next value if
        // any, so if the iterated object is mutated values may be skipped.
        if (m_current >= m_headers->size())
            return false;

        const FetchHeaderList::Header& header = m_headers->entry(m_current++);
        key = header.first;
        value = header.second;
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_headers);
        PairIterable<String, String>::IterationSource::trace(visitor);
    }

private:
    const Member<FetchHeaderList> m_headers;
    size_t m_current;
};

} // namespace

Headers* Headers::create()
{
    return new Headers;
}

Headers* Headers::create(ExceptionState&)
{
    return create();
}

Headers* Headers::create(const Headers* init, ExceptionState& exceptionState)
{
    // "The Headers(|init|) constructor, when invoked, must run these steps:"
    // "1. Let |headers| be a new Headers object."
    Headers* headers = create();
    // "2. If |init| is given, fill headers with |init|. Rethrow any exception."
    headers->fillWith(init, exceptionState);
    // "3. Return |headers|."
    return headers;
}

Headers* Headers::create(const Vector<Vector<String>>& init, ExceptionState& exceptionState)
{
    // The same steps as above.
    Headers* headers = create();
    headers->fillWith(init, exceptionState);
    return headers;
}

Headers* Headers::create(const Dictionary& init, ExceptionState& exceptionState)
{
    // "The Headers(|init|) constructor, when invoked, must run these steps:"
    // "1. Let |headers| be a new Headers object."
    Headers* headers = create();
    // "2. If |init| is given, fill headers with |init|. Rethrow any exception."
    headers->fillWith(init, exceptionState);
    // "3. Return |headers|."
    return headers;
}

Headers* Headers::create(FetchHeaderList* headerList)
{
    return new Headers(headerList);
}

Headers* Headers::clone() const
{
    FetchHeaderList* headerList = m_headerList->clone();
    Headers* headers = create(headerList);
    headers->m_guard = m_guard;
    return headers;
}

void Headers::append(const String& name, const String& value, ExceptionState& exceptionState)
{
    // "To append a name/value (|name|/|value|) pair to a Headers object
    // (|headers|), run these steps:"
    // "1. If |name| is not a name or |value| is not a value, throw a
    //     TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return;
    }
    if (!FetchHeaderList::isValidHeaderValue(value)) {
        exceptionState.throwTypeError("Invalid value");
        return;
    }
    // "2. If guard is |request|, throw a TypeError."
    if (m_guard == ImmutableGuard) {
        exceptionState.throwTypeError("Headers are immutable");
        return;
    }
    // "3. Otherwise, if guard is |request| and |name| is a forbidden header
    //     name, return."
    if (m_guard == RequestGuard && FetchUtils::isForbiddenHeaderName(name))
        return;
    // "4. Otherwise, if guard is |request-no-CORS| and |name|/|value| is not a
    //     simple header, return."
    if (m_guard == RequestNoCORSGuard && !FetchUtils::isSimpleHeader(AtomicString(name), AtomicString(value)))
        return;
    // "5. Otherwise, if guard is |response| and |name| is a forbidden response
    //     header name, return."
    if (m_guard == ResponseGuard && FetchUtils::isForbiddenResponseHeaderName(name))
        return;
    // "6. Append |name|/|value| to header list."
    m_headerList->append(name, value);
}

void Headers::remove(const String& name, ExceptionState& exceptionState)
{
    // "The delete(|name|) method, when invoked, must run these steps:"
    // "1. If name is not a name, throw a TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return;
    }
    // "2. If guard is |immutable|, throw a TypeError."
    if (m_guard == ImmutableGuard) {
        exceptionState.throwTypeError("Headers are immutable");
        return;
    }
    // "3. Otherwise, if guard is |request| and |name| is a forbidden header
    //     name, return."
    if (m_guard == RequestGuard && FetchUtils::isForbiddenHeaderName(name))
        return;
    // "4. Otherwise, if guard is |request-no-CORS| and |name|/`invalid` is not
    //     a simple header, return."
    if (m_guard == RequestNoCORSGuard && !FetchUtils::isSimpleHeader(AtomicString(name), "invalid"))
        return;
    // "5. Otherwise, if guard is |response| and |name| is a forbidden response
    //     header name, return."
    if (m_guard == ResponseGuard && FetchUtils::isForbiddenResponseHeaderName(name))
        return;
    // "6. Delete |name| from header list."
    m_headerList->remove(name);
}

String Headers::get(const String& name, ExceptionState& exceptionState)
{
    // "The get(|name|) method, when invoked, must run these steps:"
    // "1. If |name| is not a name, throw a TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return String();
    }
    // "2. Return the value of the first header in header list whose name is
    //     |name|, and null otherwise."
    String result;
    m_headerList->get(name, result);
    return result;
}

Vector<String> Headers::getAll(const String& name, ExceptionState& exceptionState)
{
    // "The getAll(|name|) method, when invoked, must run these steps:"
    // "1. If |name| is not a name, throw a TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return Vector<String>();
    }
    // "2. Return the values of all headers in header list whose name is |name|,
    //     in list order, and the empty sequence otherwise."
    Vector<String> result;
    m_headerList->getAll(name, result);
    return result;
}

bool Headers::has(const String& name, ExceptionState& exceptionState)
{
    // "The has(|name|) method, when invoked, must run these steps:"
    // "1. If |name| is not a name, throw a TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return false;
    }
    // "2. Return true if there is a header in header list whose name is |name|,
    //     and false otherwise."
    return m_headerList->has(name);
}

void Headers::set(const String& name, const String& value, ExceptionState& exceptionState)
{
    // "The set(|name|, |value|) method, when invoked, must run these steps:"
    // "1. If |name| is not a name or |value| is not a value, throw a
    //     TypeError."
    if (!FetchHeaderList::isValidHeaderName(name)) {
        exceptionState.throwTypeError("Invalid name");
        return;
    }
    if (!FetchHeaderList::isValidHeaderValue(value)) {
        exceptionState.throwTypeError("Invalid value");
        return;
    }
    // "2. If guard is |immutable|, throw a TypeError."
    if (m_guard == ImmutableGuard) {
        exceptionState.throwTypeError("Headers are immutable");
        return;
    }
    // "3. Otherwise, if guard is |request| and |name| is a forbidden header
    //     name, return."
    if (m_guard == RequestGuard && FetchUtils::isForbiddenHeaderName(name))
        return;
    // "4. Otherwise, if guard is |request-no-CORS| and |name|/|value| is not a
    //     simple header, return."
    if (m_guard == RequestNoCORSGuard && !FetchUtils::isSimpleHeader(AtomicString(name), AtomicString(value)))
        return;
    // "5. Otherwise, if guard is |response| and |name| is a forbidden response
    //     header name, return."
    if (m_guard == ResponseGuard && FetchUtils::isForbiddenResponseHeaderName(name))
        return;
    // "6. Set |name|/|value| in header list."
    m_headerList->set(name, value);
}

void Headers::fillWith(const Headers* object, ExceptionState& exceptionState)
{
    ASSERT(m_headerList->size() == 0);
    // "To fill a Headers object (|this|) with a given object (|object|), run
    // these steps:"
    // "1. If |object| is a Headers object, copy its header list as
    //     |headerListCopy| and then for each |header| in |headerListCopy|,
    //     retaining order, append header's |name|/|header|'s value to
    //     |headers|. Rethrow any exception."
    for (size_t i = 0; i < object->m_headerList->list().size(); ++i) {
        append(object->m_headerList->list()[i]->first, object->m_headerList->list()[i]->second, exceptionState);
        if (exceptionState.hadException())
            return;
    }
}

void Headers::fillWith(const Vector<Vector<String>>& object, ExceptionState& exceptionState)
{
    ASSERT(!m_headerList->size());
    // "2. Otherwise, if |object| is a sequence, then for each |header| in
    //     |object|, run these substeps:
    //    1. If |header| does not contain exactly two items, throw a
    //       TypeError.
    //    2. Append |header|'s first item/|header|'s second item to
    //       |headers|. Rethrow any exception."
    for (size_t i = 0; i < object.size(); ++i) {
        if (object[i].size() != 2) {
            exceptionState.throwTypeError("Invalid value");
            return;
        }
        append(object[i][0], object[i][1], exceptionState);
        if (exceptionState.hadException())
            return;
    }
}

void Headers::fillWith(const Dictionary& object, ExceptionState& exceptionState)
{
    ASSERT(!m_headerList->size());
    Vector<String> keys;
    object.getPropertyNames(keys);
    if (!keys.size())
        return;

    // "3. Otherwise, if |object| is an open-ended dictionary, then for each
    //    |header| in object, run these substeps:
    //    1. Set |header|'s key to |header|'s key, converted to ByteString.
    //       Rethrow any exception.
    //    2. Append |header|'s key/|header|'s value to |headers|. Rethrow any
    //       exception."
    // FIXME: Support OpenEndedDictionary<ByteString>.
    for (size_t i = 0; i < keys.size(); ++i) {
        String value;
        if (!DictionaryHelper::get(object, keys[i], value)) {
            exceptionState.throwTypeError("Invalid value");
            return;
        }
        append(keys[i], value, exceptionState);
        if (exceptionState.hadException())
            return;
    }
}

Headers::Headers()
    : m_headerList(FetchHeaderList::create())
    , m_guard(NoneGuard)
{
}

Headers::Headers(FetchHeaderList* headerList)
    : m_headerList(headerList)
    , m_guard(NoneGuard)
{
}

DEFINE_TRACE(Headers)
{
    visitor->trace(m_headerList);
}

PairIterable<String, String>::IterationSource* Headers::startIteration(ScriptState*, ExceptionState&)
{
    return new HeadersIterationSource(m_headerList);
}

} // namespace blink
