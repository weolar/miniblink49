// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_PACKET_H_
#define FXJS_XFA_CJX_PACKET_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Packet;

class CJX_Packet final : public CJX_Node {
 public:
  explicit CJX_Packet(CXFA_Packet* packet);
  ~CJX_Packet() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(getAttribute);
  JSE_METHOD(removeAttribute);
  JSE_METHOD(setAttribute);

  JSE_PROP(content);

 private:
  using Type__ = CJX_Packet;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Packet;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_PACKET_H_
