/*
 * Copyright (C) 2004 Apple Computer, Inc.  All rights reserved.
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

#ifndef markup_h
#define markup_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/dom/ParserContentPolicy.h"
#include "core/dom/Position.h"
#include "core/editing/EphemeralRange.h"
#include "core/editing/HTMLInterchange.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class ContainerNode;
class Document;
class DocumentFragment;
class Element;
class ExceptionState;
class KURL;
class Node;
class Range;
class StylePropertySet;

enum EChildrenOnly { IncludeNode, ChildrenOnly };
enum EAbsoluteURLs { DoNotResolveURLs, ResolveAllURLs, ResolveNonLocalURLs };
enum class ConvertBlocksToInlines { NotConvert, Convert };

PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromText(const EphemeralRange& context, const String& text);
// TODO(yosin) We should get rid of |createFragmentFromText()| with |Range| for
// Oilpan.
PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromText(Range* context, const String& text);
PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromMarkup(Document&, const String& markup, const String& baseURL, ParserContentPolicy = AllowScriptingContent);
PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentFromMarkupWithContext(Document&, const String& markup, unsigned fragmentStart, unsigned fragmentEnd, const String& baseURL, ParserContentPolicy);
PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentForInnerOuterHTML(const String&, Element*, ParserContentPolicy, const char* method, ExceptionState&);
PassRefPtrWillBeRawPtr<DocumentFragment> createFragmentForTransformToFragment(const String&, const String& sourceMIMEType, Document& outputDoc);
PassRefPtrWillBeRawPtr<DocumentFragment> createContextualFragment(const String&, Element*, ParserContentPolicy, ExceptionState&);

bool isPlainTextMarkup(Node*);

// These methods are used by HTMLElement & ShadowRoot to replace the
// children with respected fragment/text.
void replaceChildrenWithFragment(ContainerNode*, PassRefPtrWillBeRawPtr<DocumentFragment>, ExceptionState&);
void replaceChildrenWithText(ContainerNode*, const String&, ExceptionState&);

CORE_EXPORT String createMarkup(const Node*, EChildrenOnly = IncludeNode, EAbsoluteURLs = DoNotResolveURLs);

CORE_EXPORT String createMarkup(const Position& start, const Position& end, EAnnotateForInterchange = DoNotAnnotateForInterchange, ConvertBlocksToInlines = ConvertBlocksToInlines::NotConvert, EAbsoluteURLs = DoNotResolveURLs, Node* constrainingAncestor = nullptr);
CORE_EXPORT String createMarkup(const PositionInComposedTree& start, const PositionInComposedTree& end, EAnnotateForInterchange = DoNotAnnotateForInterchange, ConvertBlocksToInlines = ConvertBlocksToInlines::NotConvert, EAbsoluteURLs = DoNotResolveURLs, Node* constrainingAncestor = nullptr);

String urlToMarkup(const KURL&, const String& title);
void mergeWithNextTextNode(Text*, ExceptionState&);

bool propertyMissingOrEqualToNone(StylePropertySet*, CSSPropertyID);

}

#endif // markup_h
