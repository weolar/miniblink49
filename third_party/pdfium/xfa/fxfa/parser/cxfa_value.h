// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_VALUE_H_
#define XFA_FXFA_PARSER_CXFA_VALUE_H_

#include "core/fxcrt/fx_string.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_text.h"

class CXFA_Arc;
class CXFA_ExData;
class CXFA_Image;
class CXFA_Line;
class CXFA_Rectangle;

class CXFA_Value final : public CXFA_Node {
 public:
  CXFA_Value(CXFA_Document* doc, XFA_PacketType packet);
  ~CXFA_Value() override;

  XFA_Element GetChildValueClassID() const;
  WideString GetChildValueContent() const;
  CXFA_Arc* GetArcIfExists() const;
  CXFA_Line* GetLineIfExists() const;
  CXFA_Rectangle* GetRectangleIfExists() const;
  CXFA_Text* GetTextIfExists() const;
  CXFA_ExData* GetExDataIfExists() const;
  CXFA_Image* GetImageIfExists() const;
};

#endif  // XFA_FXFA_PARSER_CXFA_VALUE_H_
