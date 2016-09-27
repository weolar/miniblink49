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

#include "config.h"
#include "core/css/resolver/StyleResourceLoader.h"

#include "core/CSSPropertyNames.h"
#include "core/css/CSSCursorImageValue.h"
#include "core/css/CSSImageValue.h"
#include "core/css/CSSSVGDocumentValue.h"
#include "core/css/resolver/ElementStyleResources.h"
#include "core/dom/Document.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/style/ContentData.h"
#include "core/style/FillLayer.h"
#include "core/style/ComputedStyle.h"
#include "core/style/StyleFetchedImage.h"
#include "core/style/StyleFetchedImageSet.h"
#include "core/style/StyleGeneratedImage.h"
#include "core/style/StylePendingImage.h"
#include "core/layout/svg/ReferenceFilterBuilder.h"

namespace blink {

StyleResourceLoader::StyleResourceLoader(Document* document)
    : m_document(document)
{
}

DEFINE_TRACE(StyleResourceLoader)
{
    visitor->trace(m_document);
}

void StyleResourceLoader::loadPendingSVGDocuments(ComputedStyle* computedStyle, ElementStyleResources& elementStyleResources)
{
    if (!computedStyle->hasFilter() || elementStyleResources.pendingSVGDocuments().isEmpty())
        return;

    FilterOperations::FilterOperationVector& filterOperations = computedStyle->mutableFilter().operations();
    for (unsigned i = 0; i < filterOperations.size(); ++i) {
        RefPtrWillBeRawPtr<FilterOperation> filterOperation = filterOperations.at(i);
        if (filterOperation->type() == FilterOperation::REFERENCE) {
            ReferenceFilterOperation* referenceFilter = toReferenceFilterOperation(filterOperation.get());

            CSSSVGDocumentValue* value = elementStyleResources.pendingSVGDocuments().get(referenceFilter);
            if (!value)
                continue;
            DocumentResource* resource = value->load(m_document);
            if (!resource)
                continue;

            // Stash the DocumentResource on the reference filter.
            ReferenceFilterBuilder::setDocumentResourceReference(referenceFilter, adoptPtr(new DocumentResourceReference(resource)));
        }
    }

    elementStyleResources.clearPendingSVGDocuments();
}

static PassRefPtr<StyleImage> doLoadPendingImage(Document* document, StylePendingImage* pendingImage, float deviceScaleFactor, const ResourceLoaderOptions& options)
{
    if (CSSImageValue* imageValue = pendingImage->cssImageValue())
        return imageValue->cachedImage(document, options);

    if (CSSImageGeneratorValue* imageGeneratorValue
        = pendingImage->cssImageGeneratorValue()) {
        imageGeneratorValue->loadSubimages(document);
        return StyleGeneratedImage::create(imageGeneratorValue);
    }

    if (CSSCursorImageValue* cursorImageValue
        = pendingImage->cssCursorImageValue())
        return cursorImageValue->cachedImage(document, deviceScaleFactor);

    if (CSSImageSetValue* imageSetValue = pendingImage->cssImageSetValue())
        return imageSetValue->cachedImageSet(document, deviceScaleFactor, options);

    return nullptr;
}

PassRefPtr<StyleImage> StyleResourceLoader::loadPendingImage(StylePendingImage* pendingImage, float deviceScaleFactor)
{
    return doLoadPendingImage(m_document, pendingImage, deviceScaleFactor, ResourceFetcher::defaultResourceOptions());
}

void StyleResourceLoader::loadPendingShapeImage(ComputedStyle* computedStyle, ShapeValue* shapeValue, float deviceScaleFactor)
{
    if (!shapeValue)
        return;

    StyleImage* image = shapeValue->image();
    if (!image || !image->isPendingImage())
        return;

    ResourceLoaderOptions options = ResourceFetcher::defaultResourceOptions();
    options.allowCredentials = DoNotAllowStoredCredentials;
    options.credentialsRequested  = ClientDidNotRequestCredentials;
    options.corsEnabled = IsCORSEnabled;

    shapeValue->setImage(doLoadPendingImage(m_document, toStylePendingImage(image), deviceScaleFactor, options));
}

void StyleResourceLoader::loadPendingImages(ComputedStyle* style, ElementStyleResources& elementStyleResources)
{
    if (elementStyleResources.pendingImageProperties().isEmpty())
        return;

    PendingImagePropertyMap::const_iterator::Keys end = elementStyleResources.pendingImageProperties().end().keys();
    for (PendingImagePropertyMap::const_iterator::Keys it = elementStyleResources.pendingImageProperties().begin().keys(); it != end; ++it) {
        CSSPropertyID currentProperty = *it;

        switch (currentProperty) {
        case CSSPropertyBackgroundImage: {
            for (FillLayer* backgroundLayer = &style->accessBackgroundLayers(); backgroundLayer; backgroundLayer = backgroundLayer->next()) {
                if (backgroundLayer->image() && backgroundLayer->image()->isPendingImage())
                    backgroundLayer->setImage(loadPendingImage(toStylePendingImage(backgroundLayer->image()), elementStyleResources.deviceScaleFactor()));
            }
            break;
        }
        case CSSPropertyContent: {
            for (ContentData* contentData = const_cast<ContentData*>(style->contentData()); contentData; contentData = contentData->next()) {
                if (contentData->isImage()) {
                    StyleImage* image = toImageContentData(contentData)->image();
                    if (image->isPendingImage()) {
                        RefPtr<StyleImage> loadedImage = loadPendingImage(toStylePendingImage(image), elementStyleResources.deviceScaleFactor());
                        if (loadedImage)
                            toImageContentData(contentData)->setImage(loadedImage.release());
                    }
                }
            }
            break;
        }
        case CSSPropertyCursor: {
            if (CursorList* cursorList = style->cursors()) {
                for (size_t i = 0; i < cursorList->size(); ++i) {
                    CursorData& currentCursor = cursorList->at(i);
                    if (StyleImage* image = currentCursor.image()) {
                        if (image->isPendingImage())
                            currentCursor.setImage(loadPendingImage(toStylePendingImage(image), elementStyleResources.deviceScaleFactor()));
                    }
                }
            }
            break;
        }
        case CSSPropertyListStyleImage: {
            if (style->listStyleImage() && style->listStyleImage()->isPendingImage())
                style->setListStyleImage(loadPendingImage(toStylePendingImage(style->listStyleImage()), elementStyleResources.deviceScaleFactor()));
            break;
        }
        case CSSPropertyBorderImageSource: {
            if (style->borderImageSource() && style->borderImageSource()->isPendingImage())
                style->setBorderImageSource(loadPendingImage(toStylePendingImage(style->borderImageSource()), elementStyleResources.deviceScaleFactor()));
            break;
        }
        case CSSPropertyWebkitBoxReflect: {
            if (StyleReflection* reflection = style->boxReflect()) {
                const NinePieceImage& maskImage = reflection->mask();
                if (maskImage.image() && maskImage.image()->isPendingImage()) {
                    RefPtr<StyleImage> loadedImage = loadPendingImage(toStylePendingImage(maskImage.image()), elementStyleResources.deviceScaleFactor());
                    reflection->setMask(NinePieceImage(loadedImage.release(), maskImage.imageSlices(), maskImage.fill(), maskImage.borderSlices(), maskImage.outset(), maskImage.horizontalRule(), maskImage.verticalRule()));
                }
            }
            break;
        }
        case CSSPropertyWebkitMaskBoxImageSource: {
            if (style->maskBoxImageSource() && style->maskBoxImageSource()->isPendingImage())
                style->setMaskBoxImageSource(loadPendingImage(toStylePendingImage(style->maskBoxImageSource()), elementStyleResources.deviceScaleFactor()));
            break;
        }
        case CSSPropertyWebkitMaskImage: {
            for (FillLayer* maskLayer = &style->accessMaskLayers(); maskLayer; maskLayer = maskLayer->next()) {
                if (maskLayer->image() && maskLayer->image()->isPendingImage())
                    maskLayer->setImage(loadPendingImage(toStylePendingImage(maskLayer->image()), elementStyleResources.deviceScaleFactor()));
            }
            break;
        }
        case CSSPropertyShapeOutside:
            loadPendingShapeImage(style, style->shapeOutside(), elementStyleResources.deviceScaleFactor());
            break;
        default:
            ASSERT_NOT_REACHED();
        }
    }

    elementStyleResources.clearPendingImageProperties();
}

void StyleResourceLoader::loadPendingResources(ComputedStyle* computedStyle, ElementStyleResources& elementStyleResources)
{
    // Start loading images referenced by this style.
    loadPendingImages(computedStyle, elementStyleResources);

    // Start loading the SVG Documents referenced by this style.
    loadPendingSVGDocuments(computedStyle, elementStyleResources);
}

}
