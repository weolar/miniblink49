// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_DOCUMENT_DATAMERGER_IMP_H_
#define XFA_FXFA_PARSER_XFA_DOCUMENT_DATAMERGER_IMP_H_

#include <vector>

#include "xfa/fxfa/fxfa_basic.h"

class CXFA_Document;
class CXFA_Node;

bool XFA_DataMerge_NeedGenerateForm(CXFA_Node* pTemplateChild,
                                    bool bUseInstanceManager);
CXFA_Node* XFA_NodeMerge_CloneOrMergeContainer(
    CXFA_Document* pDocument,
    CXFA_Node* pFormParent,
    CXFA_Node* pTemplateNode,
    bool bRecursive,
    std::vector<CXFA_Node*>* pSubformArray);
CXFA_Node* XFA_DataMerge_FindDataScope(CXFA_Node* pParentFormNode);
CXFA_Node* XFA_DataMerge_FindFormDOMInstance(CXFA_Document* pDocument,
                                             XFA_Element eType,
                                             uint32_t dwNameHash,
                                             CXFA_Node* pFormParent);

#endif  // XFA_FXFA_PARSER_XFA_DOCUMENT_DATAMERGER_IMP_H_
