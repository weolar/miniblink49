/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef StyleResourceLoader_h
#define StyleResourceLoader_h

#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class ElementStyleResources;
class Document;
class ComputedStyle;
class RenderStyle;
class ShapeValue;
class StyleImage;
class StylePendingImage;

// Manages loading of resources, requested by the stylesheets.
// Expects the same lifetime as StyleResolver, because
// it expects Document to never change.
class StyleResourceLoader final {
    WTF_MAKE_NONCOPYABLE(StyleResourceLoader);
    DISALLOW_ALLOCATION();
public:
    explicit StyleResourceLoader(Document*);

    void loadPendingResources(ComputedStyle*, ElementStyleResources&);

    DECLARE_TRACE();

private:
    void loadPendingSVGDocuments(ComputedStyle*, ElementStyleResources&);

    PassRefPtr<StyleImage> loadPendingImage(StylePendingImage*, float deviceScaleFactor);
    void loadPendingImages(ComputedStyle*, ElementStyleResources&);
    void loadPendingShapeImage(ComputedStyle*, ShapeValue*, float deviceScaleFactor);

    RawPtrWillBeMember<Document> m_document;
};

} // namespace blink

#endif // StyleResourceLoader_h
