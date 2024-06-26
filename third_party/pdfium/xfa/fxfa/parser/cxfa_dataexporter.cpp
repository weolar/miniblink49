// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_dataexporter.h"

#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "xfa/fxfa/parser/cxfa_document.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/xfa_utils.h"

CXFA_DataExporter::CXFA_DataExporter() = default;

CXFA_DataExporter::~CXFA_DataExporter() = default;

bool CXFA_DataExporter::Export(const RetainPtr<IFX_SeekableStream>& pStream,
                               CXFA_Node* pNode) {
  ASSERT(pStream);

  if (!pStream)
    return false;

  if (pNode->IsModelNode()) {
    switch (pNode->GetPacketType()) {
      case XFA_PacketType::Xdp: {
        pStream->WriteString(
            "<xdp:xdp xmlns:xdp=\"http://ns.adobe.com/xdp/\">");
        for (CXFA_Node* pChild = pNode->GetFirstChild(); pChild;
             pChild = pChild->GetNextSibling()) {
          Export(pStream, pChild);
        }
        pStream->WriteString("</xdp:xdp\n>");
        break;
      }
      case XFA_PacketType::Datasets: {
        CFX_XMLElement* pElement = ToXMLElement(pNode->GetXMLMappingNode());
        if (!pElement)
          return false;

        CXFA_Node* pDataNode = pNode->GetFirstChild();
        ASSERT(pDataNode);
        XFA_DataExporter_DealWithDataGroupNode(pDataNode);
        pElement->Save(pStream);
        break;
      }
      case XFA_PacketType::Form:
        XFA_DataExporter_RegenerateFormFile(pNode, pStream, false);
        break;
      case XFA_PacketType::Template:
      default: {
        CFX_XMLElement* pElement = ToXMLElement(pNode->GetXMLMappingNode());
        if (!pElement)
          return false;

        pElement->Save(pStream);
        break;
      }
    }
    return true;
  }

  CXFA_Node* pDataNode = pNode->GetParent();
  CXFA_Node* pExportNode = pNode;
  for (CXFA_Node* pChildNode = pDataNode->GetFirstChild(); pChildNode;
       pChildNode = pChildNode->GetNextSibling()) {
    if (pChildNode != pNode) {
      pExportNode = pDataNode;
      break;
    }
  }
  CFX_XMLElement* pElement = ToXMLElement(pExportNode->GetXMLMappingNode());
  if (!pElement)
    return false;

  XFA_DataExporter_DealWithDataGroupNode(pExportNode);
  pElement->SetAttribute(L"xmlns:xfa",
                         L"http://www.xfa.org/schema/xfa-data/1.0/");
  pElement->Save(pStream);
  pElement->RemoveAttribute(L"xmlns:xfa");
  return true;
}
