// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef URLSearchParams_h
#define URLSearchParams_h

#include "bindings/core/v8/Iterable.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/USVStringOrURLSearchParams.h"
#include "platform/heap/Handle.h"
//#include "platform/network/EncodedFormData.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"
#include <utility>

namespace blink {

class ExceptionState;
class DOMURL;

typedef USVStringOrURLSearchParams URLSearchParamsInit;

class CORE_EXPORT URLSearchParams final
    : public GarbageCollectedFinalized<URLSearchParams>,
    public ScriptWrappable,
    public PairIterable<String, String>{
    DEFINE_WRAPPERTYPEINFO();

public:
    static URLSearchParams* create(const URLSearchParamsInit&);

    static URLSearchParams* create(const String& queryString,
        DOMURL* urlObject = nullptr)
    {
        return new URLSearchParams(queryString, urlObject);
    }

    ~URLSearchParams();

    // URLSearchParams interface methods
    String toString() const;
    void append(const String& name, const String& value);
    void deleteAllWithName(const String&);
    String get(const String&) const;
    Vector<String> getAll(const String&) const;
    bool has(const String&) const;
    void set(const String& name, const String& value);
    void setInput(const String&);

    // Internal helpers
    //PassRefPtr<EncodedFormData> toEncodedFormData() const;
    const Vector<std::pair<String, String>>& params() const { return m_params; }

#ifdef ASSERT
    DOMURL* urlObject() const;
#endif

    DECLARE_TRACE();

private:
    //FRIEND_TEST_ALL_PREFIXES(URLSearchParamsTest, EncodedFormData);

    explicit URLSearchParams(const String&, DOMURL* = nullptr);
    explicit URLSearchParams(URLSearchParams*);

    void runUpdateSteps();
    IterationSource* startIteration(ScriptState*, ExceptionState&) override;
    void encodeAsFormData(Vector<char>&) const;

    Vector<std::pair<String, String>> m_params;

    WeakMember<DOMURL> m_urlObject;
};

}  // namespace blink

#endif  // URLSearchParams_h
