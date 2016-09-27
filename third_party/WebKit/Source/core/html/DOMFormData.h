/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef DOMFormData_h
#define DOMFormData_h

#include "bindings/core/v8/Iterable.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/CoreExport.h"
#include "core/html/FormDataList.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace WTF {
class TextEncoding;
}

namespace blink {

class Blob;
class HTMLFormElement;

// Typedef from FormData.idl:
typedef FileOrUSVString FormDataEntryValue;

class CORE_EXPORT DOMFormData final : public FormDataList, public ScriptWrappable, public PairIterable<String, FormDataEntryValue> {
    DEFINE_WRAPPERTYPEINFO();

public:
    static DOMFormData* create(HTMLFormElement* form = 0)
    {
        return new DOMFormData(form);
    }

    static DOMFormData* create(const WTF::TextEncoding& encoding)
    {
        return new DOMFormData(encoding);
    }

    // FormData interface.
    void append(const String& name, const String& value);
    void append(ExecutionContext*, const String& name, Blob*, const String& filename = String());
    void get(const String& name, FormDataEntryValue& result);
    Vector<FormDataEntryValue> getAll(const String& name);
    void set(const String& name, const String& value);
    void set(const String& name, Blob*, const String& filename = String());

private:
    explicit DOMFormData(const WTF::TextEncoding&);
    explicit DOMFormData(HTMLFormElement*);

    IterationSource* startIteration(ScriptState*, ExceptionState&) override;
};

} // namespace blink

#endif // DOMFormData_h
