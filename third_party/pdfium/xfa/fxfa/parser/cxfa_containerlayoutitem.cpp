// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_containerlayoutitem.h"

#include "fxjs/xfa/cjx_object.h"
#include "xfa/fxfa/parser/cxfa_layoutpagemgr.h"
#include "xfa/fxfa/parser/cxfa_layoutprocessor.h"
#include "xfa/fxfa/parser/cxfa_measurement.h"
#include "xfa/fxfa/parser/cxfa_medium.h"
#include "xfa/fxfa/parser/cxfa_node.h"

CXFA_ContainerLayoutItem::CXFA_ContainerLayoutItem(CXFA_Node* pNode)
    : CXFA_LayoutItem(pNode, kContainerItem) {}

CXFA_ContainerLayoutItem::~CXFA_ContainerLayoutItem() = default;

CXFA_LayoutProcessor* CXFA_ContainerLayoutItem::GetLayout() const {
  return m_pFormNode->GetDocument()->GetLayoutProcessor();
}

int32_t CXFA_ContainerLayoutItem::GetPageIndex() const {
  return m_pFormNode->GetDocument()
      ->GetLayoutProcessor()
      ->GetLayoutPageMgr()
      ->GetPageIndex(this);
}

CFX_SizeF CXFA_ContainerLayoutItem::GetPageSize() const {
  CFX_SizeF size;
  CXFA_Medium* pMedium =
      m_pFormNode->GetFirstChildByClass<CXFA_Medium>(XFA_Element::Medium);
  if (!pMedium)
    return size;

  size = CFX_SizeF(pMedium->JSObject()
                       ->GetMeasure(XFA_Attribute::Short)
                       .ToUnit(XFA_Unit::Pt),
                   pMedium->JSObject()
                       ->GetMeasure(XFA_Attribute::Long)
                       .ToUnit(XFA_Unit::Pt));
  if (pMedium->JSObject()->GetEnum(XFA_Attribute::Orientation) ==
      XFA_AttributeValue::Landscape) {
    size = CFX_SizeF(size.height, size.width);
  }
  return size;
}

CXFA_Node* CXFA_ContainerLayoutItem::GetMasterPage() const {
  return m_pFormNode.Get();
}
