/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef SVGStringListTearOff_h
#define SVGStringListTearOff_h

#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGStringList.h"
#include "core/svg/properties/SVGPropertyTearOff.h"

namespace blink {

class SVGStringListTearOff : public SVGPropertyTearOff<SVGStringList>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGStringListTearOff> create(PassRefPtrWillBeRawPtr<SVGStringList> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName = QualifiedName::null())
    {
        return adoptRefWillBeNoop(new SVGStringListTearOff(target, contextElement, propertyIsAnimVal, attributeName));
    }

    // SVGStringList DOM interface:

    // WebIDL requires "unsigned long" type instead of size_t.
    unsigned long length()
    {
        return target()->length();
    }

    void clear(ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return;
        }

        target()->clear();
        commitChange();
    }

    String initialize(const String& item, ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return String();
        }

        target()->initialize(item);
        commitChange();

        return item;
    }

    String getItem(unsigned long index, ExceptionState& exceptionState)
    {
        return target()->getItem(index, exceptionState);
    }

    String insertItemBefore(const String& item, unsigned long index, ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return String();
        }

        target()->insertItemBefore(item, index);
        commitChange();

        return item;
    }

    String replaceItem(const String& item, unsigned long index, ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return String();
        }

        target()->replaceItem(item, index, exceptionState);
        commitChange();

        return item;
    }

    bool anonymousIndexedSetter(unsigned index, const String& item, ExceptionState& exceptionState)
    {
        replaceItem(item, index, exceptionState);
        return true;
    }

    String removeItem(unsigned long index, ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return String();
        }

        String removedItem = target()->removeItem(index, exceptionState);
        commitChange();

        return removedItem;
    }

    String appendItem(const String& item, ExceptionState& exceptionState)
    {
        if (isImmutable()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The object is read-only.");
            return String();
        }

        target()->appendItem(item);
        commitChange();

        return item;
    }

protected:
    SVGStringListTearOff(PassRefPtrWillBeRawPtr<SVGStringList>, SVGElement*, PropertyIsAnimValType, const QualifiedName&);
};

} // namespace blink

#endif // SVGStringListTearOff_h
