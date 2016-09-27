/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller <mueller@kde.org>
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This class provides all functionality needed for loading images, style sheets and html
    pages from the web. It has a memory cache for these objects.
*/

#ifndef XSLStyleSheetResource_h
#define XSLStyleSheetResource_h

#include "core/fetch/ResourcePtr.h"
#include "core/fetch/StyleSheetResource.h"

namespace blink {

class FetchRequest;
class ResourceFetcher;

class XSLStyleSheetResource final : public StyleSheetResource {
public:
    static ResourcePtr<XSLStyleSheetResource> fetch(FetchRequest&, ResourceFetcher*);

    const String& sheet() const { return m_sheet; }

    void didAddClient(ResourceClient*) override;

protected:
    class XSLStyleSheetResourceFactory : public ResourceFactory {
    public:
        XSLStyleSheetResourceFactory()
            : ResourceFactory(Resource::XSLStyleSheet) { }

        Resource* create(const ResourceRequest& request, const String& charset) const override
        {
            return new XSLStyleSheetResource(request, charset);
        }
    };
    XSLStyleSheetResource(const ResourceRequest&, const String& charset);

    void checkNotify() override;

    String m_sheet;
};

DEFINE_RESOURCE_TYPE_CASTS(XSLStyleSheet);

} // namespace blink

#endif
