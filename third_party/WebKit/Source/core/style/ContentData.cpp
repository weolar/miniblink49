/*
 * Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/style/ContentData.h"

#include "core/layout/LayoutCounter.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutImageResource.h"
#include "core/layout/LayoutImageResourceStyleImage.h"
#include "core/layout/LayoutQuote.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/style/ComputedStyle.h"

namespace blink {

PassOwnPtr<ContentData> ContentData::create(PassRefPtr<StyleImage> image)
{
    return adoptPtr(new ImageContentData(image));
}

PassOwnPtr<ContentData> ContentData::create(const String& text)
{
    return adoptPtr(new TextContentData(text));
}

PassOwnPtr<ContentData> ContentData::create(PassOwnPtr<CounterContent> counter)
{
    return adoptPtr(new CounterContentData(counter));
}

PassOwnPtr<ContentData> ContentData::create(QuoteType quote)
{
    return adoptPtr(new QuoteContentData(quote));
}

PassOwnPtr<ContentData> ContentData::clone() const
{
    OwnPtr<ContentData> result = cloneInternal();

    ContentData* lastNewData = result.get();
    for (const ContentData* contentData = next(); contentData; contentData = contentData->next()) {
        OwnPtr<ContentData> newData = contentData->cloneInternal();
        lastNewData->setNext(newData.release());
        lastNewData = lastNewData->next();
    }

    return result.release();
}

LayoutObject* ImageContentData::createLayoutObject(Document& doc, ComputedStyle& pseudoStyle) const
{
    LayoutImage* image = LayoutImage::createAnonymous(&doc);
    image->setPseudoStyle(&pseudoStyle);
    if (m_image)
        image->setImageResource(LayoutImageResourceStyleImage::create(m_image.get()));
    else
        image->setImageResource(LayoutImageResource::create());
    return image;
}

LayoutObject* TextContentData::createLayoutObject(Document& doc, ComputedStyle& pseudoStyle) const
{
    LayoutObject* layoutObject = new LayoutTextFragment(&doc, m_text.impl());
    layoutObject->setPseudoStyle(&pseudoStyle);
    return layoutObject;
}

LayoutObject* CounterContentData::createLayoutObject(Document& doc, ComputedStyle& pseudoStyle) const
{
    LayoutObject* layoutObject = new LayoutCounter(&doc, *m_counter);
    layoutObject->setPseudoStyle(&pseudoStyle);
    return layoutObject;
}

LayoutObject* QuoteContentData::createLayoutObject(Document& doc, ComputedStyle& pseudoStyle) const
{
    LayoutObject* layoutObject = new LayoutQuote(&doc, m_quote);
    layoutObject->setPseudoStyle(&pseudoStyle);
    return layoutObject;
}

} // namespace blink
