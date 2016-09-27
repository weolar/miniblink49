/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Pasteboard_h
#define Pasteboard_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebClipboard.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DataObject;
class Image;
class KURL;

class CORE_EXPORT Pasteboard {
    WTF_MAKE_NONCOPYABLE(Pasteboard); WTF_MAKE_FAST_ALLOCATED(Pasteboard);
public:
    enum SmartReplaceOption {
        CanSmartReplace,
        CannotSmartReplace
    };

    static Pasteboard* generalPasteboard();
    void writePlainText(const String&, SmartReplaceOption);
    void writeImage(Image*, const KURL&, const String& title);
    void writeDataObject(DataObject*);
    bool canSmartReplace();
    bool isHTMLAvailable();
    String plainText();

    // If no data is read, an empty string will be returned and all out parameters will be cleared.
    // If applicable, the page URL will be assigned to the KURL parameter.
    // fragmentStart and fragmentEnd are indexes into the returned markup that indicate
    // the start and end of the returned markup. If there is no additional context,
    // fragmentStart will be zero and fragmentEnd will be the same as the length of the markup.
    String readHTML(KURL&, unsigned& fragmentStart, unsigned& fragmentEnd);

    void writeHTML(const String& markup, const KURL& documentURL, const String& plainText, bool canSmartCopyOrDelete);

    bool isSelectionMode() const;
    void setSelectionMode(bool);

    WebClipboard::Buffer buffer() const { return m_buffer; }

private:
    Pasteboard();

    WebClipboard::Buffer m_buffer;
};

} // namespace blink

#endif // Pasteboard_h
