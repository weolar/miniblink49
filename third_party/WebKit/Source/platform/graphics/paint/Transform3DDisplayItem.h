// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Transform3DDisplayItem_h
#define Transform3DDisplayItem_h

#include "platform/graphics/paint/DisplayItem.h"
#include "platform/transforms/TransformationMatrix.h"
#include "wtf/Assertions.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PLATFORM_EXPORT BeginTransform3DDisplayItem : public PairedBeginDisplayItem {
public:
    BeginTransform3DDisplayItem(const DisplayItemClientWrapper& client, Type type, const TransformationMatrix& transform)
        : PairedBeginDisplayItem(client, type)
        , m_transform(transform)
    {
        ASSERT(DisplayItem::isTransform3DType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

    const TransformationMatrix& transform() const { return m_transform; }

private:
    const TransformationMatrix m_transform;
};

class PLATFORM_EXPORT EndTransform3DDisplayItem : public PairedEndDisplayItem {
public:
    EndTransform3DDisplayItem(const DisplayItemClientWrapper& client, Type type)
        : PairedEndDisplayItem(client, type)
    {
        ASSERT(DisplayItem::isEndTransform3DType(type));
    }

    void replay(GraphicsContext&) override;
    void appendToWebDisplayItemList(WebDisplayItemList*) const override;

private:
#if ENABLE(ASSERT)
    bool isEndAndPairedWith(DisplayItem::Type otherType) const final
    {
        return DisplayItem::transform3DTypeToEndTransform3DType(otherType) == type();
    }
#endif
};

} // namespace blink

#endif // Transform3DDisplayItem_h
