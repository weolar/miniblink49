// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_XFA_BASIC_DATA_H_
#define XFA_FXFA_PARSER_XFA_BASIC_DATA_H_

#include <stddef.h>

#include "core/fxcrt/widestring.h"
#include "fxjs/xfa/cjx_object.h"
#include "third_party/base/optional.h"
#include "xfa/fxfa/fxfa_basic.h"

struct XFA_PACKETINFO {
  const wchar_t* name;
  XFA_PacketType packet_type;
  const wchar_t* uri;
  uint32_t flags;
};

struct XFA_ATTRIBUTEINFO {
  XFA_Attribute attribute;
  XFA_ScriptType eValueType;
};

struct XFA_SCRIPTATTRIBUTEINFO {
  XFA_Attribute attribute;
  XFA_ScriptType eValueType;
  XFA_ATTRIBUTE_CALLBACK callback = nullptr;
};

XFA_PACKETINFO XFA_GetPacketByIndex(XFA_PacketType ePacket);
Optional<XFA_PACKETINFO> XFA_GetPacketByName(WideStringView wsName);

ByteStringView XFA_ElementToName(XFA_Element elem);
XFA_Element XFA_GetElementByName(WideStringView name);

ByteStringView XFA_AttributeToName(XFA_Attribute attr);
Optional<XFA_ATTRIBUTEINFO> XFA_GetAttributeByName(WideStringView name);

ByteStringView XFA_AttributeValueToName(XFA_AttributeValue item);
Optional<XFA_AttributeValue> XFA_GetAttributeValueByName(WideStringView name);

Optional<XFA_SCRIPTATTRIBUTEINFO> XFA_GetScriptAttributeByName(
    XFA_Element eElement,
    WideStringView wsAttributeName);

#endif  // XFA_FXFA_PARSER_XFA_BASIC_DATA_H_
