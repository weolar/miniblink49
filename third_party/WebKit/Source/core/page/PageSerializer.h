/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
 * contributors may be used to endorse or promo te products derived from
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

#ifndef PageSerializer_h
#define PageSerializer_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/KURLHash.h"
#include "wtf/HashMap.h"
#include "wtf/ListHashSet.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class Attribute;
class FontResource;
class ImageResource;
class CSSRule;
class CSSStyleSheet;
class CSSValue;
class Document;
class LocalFrame;
class Page;
class LayoutObject;
class Resource;
class SharedBuffer;
class StylePropertySet;

struct SerializedResource;

// This class is used to serialize a page contents back to text (typically HTML).
// It serializes all the page frames and retrieves resources such as images and CSS stylesheets.
class CORE_EXPORT PageSerializer final {
    STACK_ALLOCATED();
public:
    class Delegate {
    public:
        virtual ~Delegate() { }
        virtual bool shouldIgnoreAttribute(const Attribute&) = 0;
    };

    PageSerializer(Vector<SerializedResource>*, PassOwnPtr<Delegate>);

    // Initiates the serialization of the frame's page. All serialized content and retrieved
    // resources are added to the Vector passed to the constructor. The first resource in that
    // vector is the top frame serialized content.
    void serialize(Page*);

    void registerRewriteURL(const String& from, const String& to);
    void setRewriteURLFolder(const String&);

    KURL urlForBlankFrame(LocalFrame*);

    Delegate* delegate();

private:
    void serializeFrame(LocalFrame*);

    // Serializes the stylesheet back to text and adds it to the resources if URL is not-empty.
    // It also adds any resources included in that stylesheet (including any imported stylesheets and their own resources).
    void serializeCSSStyleSheet(CSSStyleSheet&, const KURL&);

    // Serializes the css rule (including any imported stylesheets), adding referenced resources.
    void serializeCSSRule(CSSRule*);

    bool shouldAddURL(const KURL&);

    void addToResources(Resource *, PassRefPtr<SharedBuffer>, const KURL&);
    void addImageToResources(ImageResource*, LayoutObject*, const KURL&);
    void addFontToResources(FontResource*);

    void retrieveResourcesForProperties(const StylePropertySet*, Document&);
    void retrieveResourcesForCSSValue(CSSValue*, Document&);

    Vector<SerializedResource>* m_resources;
    ListHashSet<KURL> m_resourceURLs;

    using BlankFrameURLMap = WillBeHeapHashMap<RawPtrWillBeMember<LocalFrame>, KURL>;
    BlankFrameURLMap m_blankFrameURLs;
    HashMap<String, String> m_rewriteURLs;
    String m_rewriteFolder;
    unsigned m_blankFrameCounter;

    OwnPtr<Delegate> m_delegate;
};

} // namespace blink

#endif // PageSerializer_h
