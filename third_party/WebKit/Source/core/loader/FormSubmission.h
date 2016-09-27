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

#ifndef FormSubmission_h
#define FormSubmission_h

#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/Referrer.h"

namespace blink {

class Event;
class FormData;
struct FrameLoadRequest;
class HTMLFormElement;

class FormSubmission : public RefCountedWillBeGarbageCollectedFinalized<FormSubmission> {
public:
    enum Method { GetMethod, PostMethod, DialogMethod };

    class Attributes {
        WTF_MAKE_NONCOPYABLE(Attributes);
    public:
        Attributes()
            : m_method(GetMethod)
            , m_isMultiPartForm(false)
            , m_encodingType("application/x-www-form-urlencoded", AtomicString::ConstructFromLiteral)
        {
        }

        Method method() const { return m_method; }
        static Method parseMethodType(const String&);
        void updateMethodType(const String&);
        static String methodString(Method);

        const String& action() const { return m_action; }
        void parseAction(const String&);

        const AtomicString& target() const { return m_target; }
        void setTarget(const AtomicString& target) { m_target = target; }

        const AtomicString& encodingType() const { return m_encodingType; }
        static AtomicString parseEncodingType(const String&);
        void updateEncodingType(const String&);
        bool isMultiPartForm() const { return m_isMultiPartForm; }

        const String& acceptCharset() const { return m_acceptCharset; }
        void setAcceptCharset(const String& value) { m_acceptCharset = value; }

        void copyFrom(const Attributes&);

    private:
        Method m_method;
        bool m_isMultiPartForm;

        String m_action;
        AtomicString m_target;
        AtomicString m_encodingType;
        String m_acceptCharset;
    };

    static PassRefPtrWillBeRawPtr<FormSubmission> create(HTMLFormElement*, const Attributes&, PassRefPtrWillBeRawPtr<Event>);
    DECLARE_TRACE();

    void populateFrameLoadRequest(FrameLoadRequest&);

    KURL requestURL() const;

    Method method() const { return m_method; }
    const KURL& action() const { return m_action; }
    const AtomicString& target() const { return m_target; }
    void clearTarget() { m_target = nullAtom; }
    HTMLFormElement* form() const { return m_form.get(); }
    FormData* data() const { return m_formData.get(); }
    Event* event() const { return m_event.get(); }

    const String& result() const { return m_result; }

private:
    FormSubmission(Method, const KURL& action, const AtomicString& target, const AtomicString& contentType, HTMLFormElement*, PassRefPtr<FormData>, const String& boundary, PassRefPtrWillBeRawPtr<Event>);
    // FormSubmission for DialogMethod
    FormSubmission(const String& result);

    // FIXME: Hold an instance of Attributes instead of individual members.
    Method m_method;
    KURL m_action;
    AtomicString m_target;
    AtomicString m_contentType;
    RefPtrWillBeMember<HTMLFormElement> m_form;
    RefPtr<FormData> m_formData;
    String m_boundary;
    RefPtrWillBeMember<Event> m_event;
    String m_result;
};

}

#endif // FormSubmission_h
