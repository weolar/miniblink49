/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Google Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include "modules/mediastream/MediaConstraintsImpl.h"

#include "bindings/core/v8/ArrayValue.h"
#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"
#include "wtf/text/StringHash.h"

namespace blink {

namespace MediaConstraintsImpl {

static bool parse(const Dictionary& constraintsDictionary, WebVector<WebMediaConstraint>& optional, WebVector<WebMediaConstraint>& mandatory)
{
    if (constraintsDictionary.isUndefinedOrNull())
        return true;

    Vector<String> names;
    constraintsDictionary.getPropertyNames(names);

    String mandatoryName("mandatory");
    String optionalName("optional");

    for (Vector<String>::iterator it = names.begin(); it != names.end(); ++it) {
        if (*it != mandatoryName && *it != optionalName)
            return false;
    }

    Vector<WebMediaConstraint> mandatoryConstraintsVector;
    if (names.contains(mandatoryName)) {
        Dictionary mandatoryConstraintsDictionary;
        bool ok = constraintsDictionary.get(mandatoryName, mandatoryConstraintsDictionary);
        if (!ok || mandatoryConstraintsDictionary.isUndefinedOrNull())
            return false;

        HashMap<String, String> mandatoryConstraintsHashMap;
        ok = mandatoryConstraintsDictionary.getOwnPropertiesAsStringHashMap(mandatoryConstraintsHashMap);
        if (!ok)
            return false;

        HashMap<String, String>::const_iterator iter = mandatoryConstraintsHashMap.begin();
        for (; iter != mandatoryConstraintsHashMap.end(); ++iter)
            mandatoryConstraintsVector.append(WebMediaConstraint(iter->key, iter->value));
    }

    Vector<WebMediaConstraint> optionalConstraintsVector;
    if (names.contains(optionalName)) {
        ArrayValue optionalConstraints;
        bool ok = DictionaryHelper::get(constraintsDictionary, optionalName, optionalConstraints);
        if (!ok || optionalConstraints.isUndefinedOrNull())
            return false;

        size_t numberOfConstraints;
        ok = optionalConstraints.length(numberOfConstraints);
        if (!ok)
            return false;

        for (size_t i = 0; i < numberOfConstraints; ++i) {
            Dictionary constraint;
            ok = optionalConstraints.get(i, constraint);
            if (!ok || constraint.isUndefinedOrNull())
                return false;
            Vector<String> localNames;
            constraint.getPropertyNames(localNames);
            if (localNames.size() != 1)
                return false;
            String key = localNames[0];
            String value;
            ok = DictionaryHelper::get(constraint, key, value);
            if (!ok)
                return false;
            optionalConstraintsVector.append(WebMediaConstraint(key, value));
        }
    }

    optional.assign(optionalConstraintsVector);
    mandatory.assign(mandatoryConstraintsVector);
    return true;
}


WebMediaConstraints create(const Dictionary& constraintsDictionary, ExceptionState& exceptionState)
{
    WebVector<WebMediaConstraint> optional;
    WebVector<WebMediaConstraint> mandatory;
    if (!parse(constraintsDictionary, optional, mandatory)) {
        exceptionState.throwTypeError("Malformed constraints object.");
        return WebMediaConstraints();
    }

    WebMediaConstraints constraints;
    constraints.initialize(optional, mandatory);
    return constraints;
}

WebMediaConstraints create()
{
    WebMediaConstraints constraints;
    constraints.initialize();
    return constraints;
}

} // namespace MediaConstraintsImpl
} // namespace blink
