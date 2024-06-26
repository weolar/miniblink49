// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_RESOLVENODE_RS_H_
#define XFA_FXFA_PARSER_XFA_RESOLVENODE_RS_H_

#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/parser/xfa_basic_data.h"

class CXFA_Object;

enum XFA_ResolveNode_RSType {
  XFA_ResolveNode_RSType_Nodes,
  XFA_ResolveNode_RSType_Attribute,
  XFA_ResolveNode_RSType_CreateNodeOne,
  XFA_ResolveNode_RSType_CreateNodeAll,
  XFA_ResolveNode_RSType_CreateNodeMidAll,
  XFA_ResolveNode_RSType_ExistNodes,
};

struct XFA_RESOLVENODE_RS {
  XFA_RESOLVENODE_RS();
  ~XFA_RESOLVENODE_RS();

  XFA_ResolveNode_RSType dwFlags = XFA_ResolveNode_RSType_Nodes;
  XFA_SCRIPTATTRIBUTEINFO script_attribute;
  std::vector<UnownedPtr<CXFA_Object>> objects;
};

inline XFA_RESOLVENODE_RS::XFA_RESOLVENODE_RS() = default;

inline XFA_RESOLVENODE_RS::~XFA_RESOLVENODE_RS() = default;

#endif  // XFA_FXFA_PARSER_XFA_RESOLVENODE_RS_H_
