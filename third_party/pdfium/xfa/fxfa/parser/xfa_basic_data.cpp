// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/xfa_basic_data.h"

#include <utility>

// #include "fxjs/xfa/cjx_arc.h"
// #include "fxjs/xfa/cjx_area.h"
// #include "fxjs/xfa/cjx_assist.h"
// #include "fxjs/xfa/cjx_barcode.h"
// #include "fxjs/xfa/cjx_bind.h"
// #include "fxjs/xfa/cjx_binditems.h"
// #include "fxjs/xfa/cjx_bookend.h"
// #include "fxjs/xfa/cjx_boolean.h"
// #include "fxjs/xfa/cjx_border.h"
// #include "fxjs/xfa/cjx_break.h"
// #include "fxjs/xfa/cjx_breakafter.h"
// #include "fxjs/xfa/cjx_breakbefore.h"
// #include "fxjs/xfa/cjx_button.h"
// #include "fxjs/xfa/cjx_calculate.h"
// #include "fxjs/xfa/cjx_caption.h"
// #include "fxjs/xfa/cjx_certificate.h"
// #include "fxjs/xfa/cjx_certificates.h"
// #include "fxjs/xfa/cjx_checkbutton.h"
// #include "fxjs/xfa/cjx_choicelist.h"
// #include "fxjs/xfa/cjx_color.h"
// #include "fxjs/xfa/cjx_comb.h"
// #include "fxjs/xfa/cjx_command.h"
// #include "fxjs/xfa/cjx_connect.h"
// #include "fxjs/xfa/cjx_connectstring.h"
// #include "fxjs/xfa/cjx_contentarea.h"
// #include "fxjs/xfa/cjx_corner.h"
// #include "fxjs/xfa/cjx_datavalue.h"
// #include "fxjs/xfa/cjx_datawindow.h"
// #include "fxjs/xfa/cjx_date.h"
// #include "fxjs/xfa/cjx_datetime.h"
// #include "fxjs/xfa/cjx_datetimeedit.h"
// #include "fxjs/xfa/cjx_decimal.h"
// #include "fxjs/xfa/cjx_defaultui.h"
// #include "fxjs/xfa/cjx_delete.h"
// #include "fxjs/xfa/cjx_delta.h"
// #include "fxjs/xfa/cjx_desc.h"
// #include "fxjs/xfa/cjx_digestmethod.h"
// #include "fxjs/xfa/cjx_digestmethods.h"
// #include "fxjs/xfa/cjx_draw.h"
// #include "fxjs/xfa/cjx_edge.h"
// #include "fxjs/xfa/cjx_encoding.h"
// #include "fxjs/xfa/cjx_encodings.h"
// #include "fxjs/xfa/cjx_encrypt.h"
// #include "fxjs/xfa/cjx_event.h"
// #include "fxjs/xfa/cjx_eventpseudomodel.h"
// #include "fxjs/xfa/cjx_exclgroup.h"
// #include "fxjs/xfa/cjx_exdata.h"
// #include "fxjs/xfa/cjx_execute.h"
// #include "fxjs/xfa/cjx_exobject.h"
// #include "fxjs/xfa/cjx_extras.h"
// #include "fxjs/xfa/cjx_field.h"
// #include "fxjs/xfa/cjx_fill.h"
// #include "fxjs/xfa/cjx_filter.h"
// #include "fxjs/xfa/cjx_float.h"
// #include "fxjs/xfa/cjx_font.h"
// #include "fxjs/xfa/cjx_format.h"
// #include "fxjs/xfa/cjx_handler.h"
// #include "fxjs/xfa/cjx_hostpseudomodel.h"
// #include "fxjs/xfa/cjx_image.h"
// #include "fxjs/xfa/cjx_imageedit.h"
// #include "fxjs/xfa/cjx_insert.h"
// #include "fxjs/xfa/cjx_instancemanager.h"
// #include "fxjs/xfa/cjx_integer.h"
// #include "fxjs/xfa/cjx_issuers.h"
// #include "fxjs/xfa/cjx_items.h"
// #include "fxjs/xfa/cjx_keep.h"
// #include "fxjs/xfa/cjx_keyusage.h"
// #include "fxjs/xfa/cjx_layoutpseudomodel.h"
// #include "fxjs/xfa/cjx_line.h"
// #include "fxjs/xfa/cjx_linear.h"
// #include "fxjs/xfa/cjx_logpseudomodel.h"
// #include "fxjs/xfa/cjx_manifest.h"
// #include "fxjs/xfa/cjx_map.h"
// #include "fxjs/xfa/cjx_margin.h"
// #include "fxjs/xfa/cjx_mdp.h"
// #include "fxjs/xfa/cjx_medium.h"
// #include "fxjs/xfa/cjx_message.h"
// #include "fxjs/xfa/cjx_node.h"
// #include "fxjs/xfa/cjx_numericedit.h"
// #include "fxjs/xfa/cjx_occur.h"
// #include "fxjs/xfa/cjx_oid.h"
// #include "fxjs/xfa/cjx_oids.h"
// #include "fxjs/xfa/cjx_operation.h"
// #include "fxjs/xfa/cjx_overflow.h"
// #include "fxjs/xfa/cjx_packet.h"
// #include "fxjs/xfa/cjx_pagearea.h"
// #include "fxjs/xfa/cjx_pageset.h"
// #include "fxjs/xfa/cjx_para.h"
// #include "fxjs/xfa/cjx_password.h"
// #include "fxjs/xfa/cjx_passwordedit.h"
// #include "fxjs/xfa/cjx_pattern.h"
// #include "fxjs/xfa/cjx_picture.h"
// #include "fxjs/xfa/cjx_query.h"
// #include "fxjs/xfa/cjx_radial.h"
// #include "fxjs/xfa/cjx_reason.h"
// #include "fxjs/xfa/cjx_reasons.h"
// #include "fxjs/xfa/cjx_recordset.h"
// #include "fxjs/xfa/cjx_rectangle.h"
// #include "fxjs/xfa/cjx_ref.h"
// #include "fxjs/xfa/cjx_rootelement.h"
// #include "fxjs/xfa/cjx_script.h"
// #include "fxjs/xfa/cjx_select.h"
// #include "fxjs/xfa/cjx_setproperty.h"
// #include "fxjs/xfa/cjx_signature.h"
// #include "fxjs/xfa/cjx_signatureproperties.h"
// #include "fxjs/xfa/cjx_signaturepseudomodel.h"
// #include "fxjs/xfa/cjx_signdata.h"
// #include "fxjs/xfa/cjx_signing.h"
// #include "fxjs/xfa/cjx_soapaction.h"
// #include "fxjs/xfa/cjx_soapaddress.h"
// #include "fxjs/xfa/cjx_solid.h"
// #include "fxjs/xfa/cjx_source.h"
// #include "fxjs/xfa/cjx_sourceset.h"
// #include "fxjs/xfa/cjx_speak.h"
// #include "fxjs/xfa/cjx_stipple.h"
// #include "fxjs/xfa/cjx_subform.h"
// #include "fxjs/xfa/cjx_subformset.h"
// #include "fxjs/xfa/cjx_subjectdn.h"
// #include "fxjs/xfa/cjx_subjectdns.h"
// #include "fxjs/xfa/cjx_submit.h"
// #include "fxjs/xfa/cjx_text.h"
// #include "fxjs/xfa/cjx_textedit.h"
// #include "fxjs/xfa/cjx_time.h"
// #include "fxjs/xfa/cjx_timestamp.h"
// #include "fxjs/xfa/cjx_tooltip.h"
// #include "fxjs/xfa/cjx_traversal.h"
// #include "fxjs/xfa/cjx_traverse.h"
// #include "fxjs/xfa/cjx_tree.h"
// #include "fxjs/xfa/cjx_treelist.h"
// #include "fxjs/xfa/cjx_ui.h"
// #include "fxjs/xfa/cjx_update.h"
// #include "fxjs/xfa/cjx_uri.h"
// #include "fxjs/xfa/cjx_user.h"
// #include "fxjs/xfa/cjx_validate.h"
// #include "fxjs/xfa/cjx_value.h"
// #include "fxjs/xfa/cjx_variables.h"
// #include "fxjs/xfa/cjx_wsdladdress.h"
// #include "fxjs/xfa/cjx_wsdlconnection.h"
// #include "fxjs/xfa/cjx_xfa.h"
// #include "fxjs/xfa/cjx_xmlconnection.h"
// #include "fxjs/xfa/cjx_xsdconnection.h"
#include "xfa/fxfa/fxfa_basic.h"

namespace {

struct PacketRecord {
  uint32_t hash;
  const wchar_t* name;
  XFA_PacketType packet_type;
  const wchar_t* uri;
  uint32_t flags;
};

const PacketRecord g_PacketTable[] = {
#undef PCKT____
#define PCKT____(a, b, c, d, e, f) \
  {a, L##b, XFA_PacketType::c, d,  \
   XFA_XDPPACKET_FLAGS_##e | XFA_XDPPACKET_FLAGS_##f},
#include "xfa/fxfa/parser/packets.inc"
#undef PCKT____
};

struct ElementRecord {
  uint32_t hash;  // Hashed as wide string.
  XFA_Element element;
  XFA_Element parent;
  const char* name;
};

const ElementRecord g_ElementTable[] = {
#undef ELEM____
#define ELEM____(a, b, c, d) {a, XFA_Element::c, XFA_Element::d, b},
#include "xfa/fxfa/parser/elements.inc"
#undef ELEM____
};

struct AttributeRecord {
  uint32_t hash;  // Hashed as wide string.
  XFA_Attribute attribute;
  XFA_ScriptType script_type;
  const char* name;
};

const AttributeRecord g_AttributeTable[] = {
#undef ATTR____
#define ATTR____(a, b, c, d) {a, XFA_Attribute::c, XFA_ScriptType::d, b},
#include "xfa/fxfa/parser/attributes.inc"
#undef ATTR____
};

struct AttributeValueRecord {
  uint32_t uHash;  // |pName| hashed as WideString.
  XFA_AttributeValue eName;
  const char* pName;
};

const AttributeValueRecord g_AttributeValueTable[] = {
#undef VALUE____
#define VALUE____(a, b, c) {a, XFA_AttributeValue::c, b},
#include "xfa/fxfa/parser/attribute_values.inc"
#undef VALUE____
};

struct ElementAttributeRecord {
  XFA_Element element;
  XFA_Attribute attribute;
  XFA_ATTRIBUTE_CALLBACK callback;
};

const ElementAttributeRecord g_ElementAttributeTable[] = {
#undef ELEM_ATTR____
#define ELEM_ATTR____(a, b, c) {XFA_Element::a, XFA_Attribute::b, c##_static},
#include "xfa/fxfa/parser/element_attributes.inc"
#undef ELEM_ATTR____
};

}  // namespace

XFA_PACKETINFO XFA_GetPacketByIndex(XFA_PacketType ePacket) {
  const PacketRecord* pRecord = &g_PacketTable[static_cast<uint8_t>(ePacket)];
  return {pRecord->name, pRecord->packet_type, pRecord->uri, pRecord->flags};
}

Optional<XFA_PACKETINFO> XFA_GetPacketByName(WideStringView wsName) {
  uint32_t hash = FX_HashCode_GetW(wsName, false);
  auto* elem = std::lower_bound(
      std::begin(g_PacketTable), std::end(g_PacketTable), hash,
      [](const PacketRecord& a, uint32_t hash) { return a.hash < hash; });
  if (elem != std::end(g_PacketTable) && elem->name == wsName)
    return XFA_GetPacketByIndex(elem->packet_type);
  return {};
}

ByteStringView XFA_ElementToName(XFA_Element elem) {
  return g_ElementTable[static_cast<size_t>(elem)].name;
}

XFA_Element XFA_GetElementByName(WideStringView name) {
  uint32_t hash = FX_HashCode_GetW(name, false);
  auto* elem = std::lower_bound(
      std::begin(g_ElementTable), std::end(g_ElementTable), hash,
      [](const ElementRecord& a, uint32_t hash) { return a.hash < hash; });
  if (elem != std::end(g_ElementTable) && name.EqualsASCII(elem->name))
    return elem->element;
  return XFA_Element::Unknown;
}

ByteStringView XFA_AttributeToName(XFA_Attribute attr) {
  return g_AttributeTable[static_cast<size_t>(attr)].name;
}

Optional<XFA_ATTRIBUTEINFO> XFA_GetAttributeByName(WideStringView name) {
  uint32_t hash = FX_HashCode_GetW(name, false);
  auto* elem = std::lower_bound(
      std::begin(g_AttributeTable), std::end(g_AttributeTable), hash,
      [](const AttributeRecord& a, uint32_t hash) { return a.hash < hash; });
  if (elem != std::end(g_AttributeTable) && name.EqualsASCII(elem->name)) {
    XFA_ATTRIBUTEINFO result;
    result.attribute = elem->attribute;
    result.eValueType = elem->script_type;
    return result;
  }
  return {};
}

ByteStringView XFA_AttributeValueToName(XFA_AttributeValue item) {
  return g_AttributeValueTable[static_cast<int32_t>(item)].pName;
}

Optional<XFA_AttributeValue> XFA_GetAttributeValueByName(WideStringView name) {
  auto* it = std::lower_bound(std::begin(g_AttributeValueTable),
                              std::end(g_AttributeValueTable),
                              FX_HashCode_GetW(name, false),
                              [](const AttributeValueRecord& arg,
                                 uint32_t hash) { return arg.uHash < hash; });
  if (it != std::end(g_AttributeValueTable) && name.EqualsASCII(it->pName))
    return it->eName;

  return {};
}

Optional<XFA_SCRIPTATTRIBUTEINFO> XFA_GetScriptAttributeByName(
    XFA_Element element,
    WideStringView attribute_name) {
  Optional<XFA_ATTRIBUTEINFO> attr = XFA_GetAttributeByName(attribute_name);
  if (!attr.has_value())
    return {};

  while (element != XFA_Element::Unknown) {
    auto compound_key = std::make_pair(element, attr.value().attribute);
    auto* it = std::lower_bound(
        std::begin(g_ElementAttributeTable), std::end(g_ElementAttributeTable),
        compound_key,
        [](const ElementAttributeRecord& arg,
           const std::pair<XFA_Element, XFA_Attribute>& key) {
          return std::make_pair(arg.element, arg.attribute) < key;
        });
    if (it != std::end(g_ElementAttributeTable) &&
        compound_key == std::make_pair(it->element, it->attribute)) {
      XFA_SCRIPTATTRIBUTEINFO result;
      result.attribute = attr.value().attribute;
      result.eValueType = attr.value().eValueType;
      result.callback = it->callback;
      return result;
    }
    element = g_ElementTable[static_cast<size_t>(element)].parent;
  }
  return {};
}
