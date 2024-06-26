// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/xml/cfx_xmlparser.h"

#include <algorithm>
#include <cwctype>
#include <iterator>
#include <stack>
#include <utility>

#include "core/fxcrt/cfx_seekablestreamproxy.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/xml/cfx_xmlchardata.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "core/fxcrt/xml/cfx_xmlinstruction.h"
#include "core/fxcrt/xml/cfx_xmlnode.h"
#include "core/fxcrt/xml/cfx_xmltext.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr size_t kCurrentTextReserve = 128;
constexpr uint32_t kMaxCharRange = 0x10ffff;

bool IsXMLWhiteSpace(wchar_t ch) {
  return ch == L' ' || ch == 0x0A || ch == 0x0D || ch == 0x09;
}

struct FX_XMLNAMECHAR {
  uint16_t wStart;
  uint16_t wEnd;
  bool bStartChar;
};

const FX_XMLNAMECHAR g_XMLNameChars[] = {
    {L'-', L'.', false},    {L'0', L'9', false},     {L':', L':', false},
    {L'A', L'Z', true},     {L'_', L'_', true},      {L'a', L'z', true},
    {0xB7, 0xB7, false},    {0xC0, 0xD6, true},      {0xD8, 0xF6, true},
    {0xF8, 0x02FF, true},   {0x0300, 0x036F, false}, {0x0370, 0x037D, true},
    {0x037F, 0x1FFF, true}, {0x200C, 0x200D, true},  {0x203F, 0x2040, false},
    {0x2070, 0x218F, true}, {0x2C00, 0x2FEF, true},  {0x3001, 0xD7FF, true},
    {0xF900, 0xFDCF, true}, {0xFDF0, 0xFFFD, true},
};

}  // namespace

// static
bool CFX_XMLParser::IsXMLNameChar(wchar_t ch, bool bFirstChar) {
  auto* it = std::lower_bound(
      std::begin(g_XMLNameChars), std::end(g_XMLNameChars), ch,
      [](const FX_XMLNAMECHAR& arg, wchar_t ch) { return arg.wEnd < ch; });
  return it != std::end(g_XMLNameChars) && ch >= it->wStart &&
         (!bFirstChar || it->bStartChar);
}

CFX_XMLParser::CFX_XMLParser(const RetainPtr<IFX_SeekableReadStream>& pStream) {
  ASSERT(pStream);

  auto proxy = pdfium::MakeRetain<CFX_SeekableStreamProxy>(pStream);
  uint16_t wCodePage = proxy->GetCodePage();
  if (wCodePage != FX_CODEPAGE_UTF16LE && wCodePage != FX_CODEPAGE_UTF16BE &&
      wCodePage != FX_CODEPAGE_UTF8) {
    proxy->SetCodePage(FX_CODEPAGE_UTF8);
  }
  stream_ = proxy;

  xml_plane_size_ = std::min(
      xml_plane_size_, pdfium::base::checked_cast<size_t>(stream_->GetSize()));

  current_text_.reserve(kCurrentTextReserve);
}

CFX_XMLParser::~CFX_XMLParser() = default;

std::unique_ptr<CFX_XMLDocument> CFX_XMLParser::Parse() {
  auto doc = pdfium::MakeUnique<CFX_XMLDocument>();
  current_node_ = doc->GetRoot();

  return DoSyntaxParse(doc.get()) ? std::move(doc) : nullptr;
}

bool CFX_XMLParser::DoSyntaxParse(CFX_XMLDocument* doc) {
  FX_FILESIZE current_buffer_idx = 0;
  FX_FILESIZE buffer_size = 0;

  FX_SAFE_SIZE_T alloc_size_safe = xml_plane_size_;
  alloc_size_safe += 1;  // For NUL.
  if (!alloc_size_safe.IsValid() || alloc_size_safe.ValueOrDie() <= 0 ||
      xml_plane_size_ <= 0)
    return false;

  std::vector<wchar_t> buffer;
  buffer.resize(pdfium::base::ValueOrDieForType<size_t>(alloc_size_safe));

  std::stack<wchar_t> character_to_skip_too_stack;
  std::stack<FX_XMLNODETYPE> node_type_stack;
  WideString current_attribute_name;
  FDE_XmlSyntaxState current_parser_state = FDE_XmlSyntaxState::Text;
  int32_t iCount = 0;
  wchar_t current_quote_character = 0;
  wchar_t current_character_to_skip_to = 0;

  while (true) {
    if (current_buffer_idx >= buffer_size) {
      if (stream_->IsEOF())
        return true;

      size_t buffer_chars = stream_->ReadBlock(buffer.data(), xml_plane_size_);
      if (buffer_chars == 0)
        return true;

      current_buffer_idx = 0;
      buffer_size = buffer_chars;
    }

    while (current_buffer_idx < buffer_size) {
      wchar_t ch = buffer[current_buffer_idx];
      switch (current_parser_state) {
        case FDE_XmlSyntaxState::Text:
          if (ch == L'<') {
            if (!current_text_.empty()) {
              current_node_->AppendChild(
                  doc->CreateNode<CFX_XMLText>(GetTextData()));
            } else {
              current_buffer_idx++;
              current_parser_state = FDE_XmlSyntaxState::Node;
            }
          } else {
            // Fail if there is text outside of the root element, ignore
            // whitespace/null.
            if (node_type_stack.empty() && ch && !FXSYS_iswspace(ch))
              return false;
            ProcessTextChar(ch);
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::Node:
          if (ch == L'!') {
            current_buffer_idx++;
            current_parser_state = FDE_XmlSyntaxState::SkipCommentOrDecl;
          } else if (ch == L'/') {
            current_buffer_idx++;
            current_parser_state = FDE_XmlSyntaxState::CloseElement;
          } else if (ch == L'?') {
            node_type_stack.push(FX_XMLNODE_Instruction);
            current_buffer_idx++;
            current_parser_state = FDE_XmlSyntaxState::Target;
          } else {
            node_type_stack.push(FX_XMLNODE_Element);
            current_parser_state = FDE_XmlSyntaxState::Tag;
          }
          break;
        case FDE_XmlSyntaxState::Target:
          if (!IsXMLNameChar(ch, current_text_.empty())) {
            if (current_text_.empty())
              return false;

            current_parser_state = FDE_XmlSyntaxState::TargetData;

            WideString target_name = GetTextData();
            if (target_name.EqualsASCII("originalXFAVersion") ||
                target_name.EqualsASCII("acrobat")) {
              auto* node = doc->CreateNode<CFX_XMLInstruction>(target_name);
              current_node_->AppendChild(node);
              current_node_ = node;
            }
          } else {
            current_text_.push_back(ch);
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::Tag:
          if (!IsXMLNameChar(ch, current_text_.empty())) {
            if (current_text_.empty())
              return false;

            current_parser_state = FDE_XmlSyntaxState::AttriName;

            auto* child = doc->CreateNode<CFX_XMLElement>(GetTextData());
            current_node_->AppendChild(child);
            current_node_ = child;
          } else {
            current_text_.push_back(ch);
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::AttriName:
          if (current_text_.empty() && IsXMLWhiteSpace(ch)) {
            current_buffer_idx++;
            break;
          }
          if (!IsXMLNameChar(ch, current_text_.empty())) {
            if (current_text_.empty()) {
              if (node_type_stack.top() == FX_XMLNODE_Element) {
                if (ch == L'>' || ch == L'/') {
                  current_parser_state = FDE_XmlSyntaxState::BreakElement;
                  break;
                }
              } else if (node_type_stack.top() == FX_XMLNODE_Instruction) {
                if (ch == L'?') {
                  current_parser_state = FDE_XmlSyntaxState::CloseInstruction;
                  current_buffer_idx++;
                } else {
                  current_parser_state = FDE_XmlSyntaxState::TargetData;
                }
                break;
              }
              return false;
            } else {
              if (node_type_stack.top() == FX_XMLNODE_Instruction) {
                if (ch != '=' && !IsXMLWhiteSpace(ch)) {
                  current_parser_state = FDE_XmlSyntaxState::TargetData;
                  break;
                }
              }
              current_parser_state = FDE_XmlSyntaxState::AttriEqualSign;
              current_attribute_name = GetTextData();
            }
          } else {
            current_text_.push_back(ch);
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::AttriEqualSign:
          if (IsXMLWhiteSpace(ch)) {
            current_buffer_idx++;
            break;
          }
          if (ch != L'=') {
            if (node_type_stack.top() == FX_XMLNODE_Instruction) {
              current_parser_state = FDE_XmlSyntaxState::TargetData;
              break;
            }
            return false;
          } else {
            current_parser_state = FDE_XmlSyntaxState::AttriQuotation;
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::AttriQuotation:
          if (IsXMLWhiteSpace(ch)) {
            current_buffer_idx++;
            break;
          }
          if (ch != L'\"' && ch != L'\'') {
            return false;
          }

          current_quote_character = ch;
          current_parser_state = FDE_XmlSyntaxState::AttriValue;
          current_buffer_idx++;
          break;
        case FDE_XmlSyntaxState::AttriValue:
          if (ch == current_quote_character) {
            if (entity_start_ > -1)
              return false;

            current_quote_character = 0;
            current_buffer_idx++;
            current_parser_state = FDE_XmlSyntaxState::AttriName;

            CFX_XMLElement* elem = ToXMLElement(current_node_);
            if (elem)
              elem->SetAttribute(current_attribute_name, GetTextData());

            current_attribute_name.clear();
          } else {
            ProcessTextChar(ch);
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::CloseInstruction:
          if (ch != L'>') {
            current_text_.push_back(ch);
            current_parser_state = FDE_XmlSyntaxState::TargetData;
          } else if (!current_text_.empty()) {
            ProcessTargetData();
          } else {
            current_buffer_idx++;
            if (node_type_stack.empty())
              return false;

            node_type_stack.pop();
            current_parser_state = FDE_XmlSyntaxState::Text;

            if (current_node_ &&
                current_node_->GetType() == FX_XMLNODE_Instruction)
              current_node_ = current_node_->GetParent();
          }
          break;
        case FDE_XmlSyntaxState::BreakElement:
          if (ch == L'>') {
            current_parser_state = FDE_XmlSyntaxState::Text;
          } else if (ch == L'/') {
            current_parser_state = FDE_XmlSyntaxState::CloseElement;
          } else {
            return false;
          }
          current_buffer_idx++;
          break;
        case FDE_XmlSyntaxState::CloseElement:
          if (!IsXMLNameChar(ch, current_text_.empty())) {
            if (ch == L'>') {
              if (node_type_stack.empty())
                return false;

              node_type_stack.pop();
              current_parser_state = FDE_XmlSyntaxState::Text;

              CFX_XMLElement* element = ToXMLElement(current_node_);
              if (!element)
                return false;

              WideString element_name = GetTextData();
              if (element_name.GetLength() > 0 &&
                  element_name != element->GetName()) {
                return false;
              }

              current_node_ = current_node_->GetParent();
              iCount++;
            } else if (!IsXMLWhiteSpace(ch)) {
              return false;
            }
          } else {
            current_text_.push_back(ch);
          }
          current_buffer_idx++;
          break;
        case FDE_XmlSyntaxState::SkipCommentOrDecl:
          if (FXSYS_wcsnicmp(buffer.data() + current_buffer_idx, L"--", 2) ==
              0) {
            current_buffer_idx += 2;
            current_parser_state = FDE_XmlSyntaxState::SkipComment;
          } else if (FXSYS_wcsnicmp(buffer.data() + current_buffer_idx,
                                    L"[CDATA[", 7) == 0) {
            current_buffer_idx += 7;
            current_parser_state = FDE_XmlSyntaxState::SkipCData;
          } else {
            current_parser_state = FDE_XmlSyntaxState::SkipDeclNode;
            current_character_to_skip_to = L'>';
            character_to_skip_too_stack.push(L'>');
          }
          break;
        case FDE_XmlSyntaxState::SkipCData: {
          if (FXSYS_wcsnicmp(buffer.data() + current_buffer_idx, L"]]>", 3) ==
              0) {
            current_buffer_idx += 3;
            current_parser_state = FDE_XmlSyntaxState::Text;

            current_node_->AppendChild(
                doc->CreateNode<CFX_XMLCharData>(GetTextData()));
          } else {
            current_text_.push_back(ch);
            current_buffer_idx++;
          }
          break;
        }
        case FDE_XmlSyntaxState::SkipDeclNode:
          if (current_character_to_skip_to == L'\'' ||
              current_character_to_skip_to == L'\"') {
            current_buffer_idx++;
            if (ch != current_character_to_skip_to)
              break;

            character_to_skip_too_stack.pop();
            if (character_to_skip_too_stack.empty())
              current_parser_state = FDE_XmlSyntaxState::Text;
            else
              current_character_to_skip_to = character_to_skip_too_stack.top();
          } else {
            switch (ch) {
              case L'<':
                current_character_to_skip_to = L'>';
                character_to_skip_too_stack.push(L'>');
                break;
              case L'[':
                current_character_to_skip_to = L']';
                character_to_skip_too_stack.push(L']');
                break;
              case L'(':
                current_character_to_skip_to = L')';
                character_to_skip_too_stack.push(L')');
                break;
              case L'\'':
                current_character_to_skip_to = L'\'';
                character_to_skip_too_stack.push(L'\'');
                break;
              case L'\"':
                current_character_to_skip_to = L'\"';
                character_to_skip_too_stack.push(L'\"');
                break;
              default:
                if (ch == current_character_to_skip_to) {
                  character_to_skip_too_stack.pop();
                  if (character_to_skip_too_stack.empty()) {
                    current_parser_state = FDE_XmlSyntaxState::Text;
                  } else {
                    current_character_to_skip_to =
                        character_to_skip_too_stack.top();
                  }
                }
                break;
            }
            current_buffer_idx++;
          }
          break;
        case FDE_XmlSyntaxState::SkipComment:
          if (FXSYS_wcsnicmp(buffer.data() + current_buffer_idx, L"-->", 3) ==
              0) {
            current_buffer_idx += 2;
            current_parser_state = FDE_XmlSyntaxState::Text;
          }

          current_buffer_idx++;
          break;
        case FDE_XmlSyntaxState::TargetData:
          if (IsXMLWhiteSpace(ch)) {
            if (current_text_.empty()) {
              current_buffer_idx++;
              break;
            }
            if (current_quote_character == 0) {
              current_buffer_idx++;
              ProcessTargetData();
              break;
            }
          }
          if (ch == '?') {
            current_parser_state = FDE_XmlSyntaxState::CloseInstruction;
            current_buffer_idx++;
          } else if (ch == '\"') {
            if (current_quote_character == 0) {
              current_quote_character = ch;
              current_buffer_idx++;
            } else if (ch == current_quote_character) {
              current_quote_character = 0;
              current_buffer_idx++;
              ProcessTargetData();
            } else {
              return false;
            }
          } else {
            current_text_.push_back(ch);
            current_buffer_idx++;
          }
          break;
        default:
          break;
      }
    }
  }

  current_node_->AppendChild(doc->CreateNode<CFX_XMLText>(GetTextData()));
  return true;
}

void CFX_XMLParser::ProcessTextChar(wchar_t character) {
  current_text_.push_back(character);

  if (entity_start_ > -1 && character == L';') {
    // Copy the entity out into a string and remove from the vector. When we
    // copy the entity we don't want to copy out the & or the ; so we start
    // shifted by one and want to copy 2 less characters in total.
    WideString csEntity(current_text_.data() + entity_start_ + 1,
                        current_text_.size() - entity_start_ - 2);
    current_text_.erase(current_text_.begin() + entity_start_,
                        current_text_.end());

    int32_t iLen = csEntity.GetLength();
    if (iLen > 0) {
      if (csEntity[0] == L'#') {
        uint32_t ch = 0;
        if (iLen > 1 && csEntity[1] == L'x') {
          for (int32_t i = 2; i < iLen; i++) {
            if (!FXSYS_IsHexDigit(csEntity[i]))
              break;
            ch = (ch << 4) + FXSYS_HexCharToInt(csEntity[i]);
          }
        } else {
          for (int32_t i = 1; i < iLen; i++) {
            if (!FXSYS_IsDecimalDigit(csEntity[i]))
              break;
            ch = ch * 10 + FXSYS_DecimalCharToInt(csEntity[i]);
          }
        }
        if (ch > kMaxCharRange)
          ch = ' ';

        character = static_cast<wchar_t>(ch);
        if (character != 0)
          current_text_.push_back(character);
      } else {
        if (csEntity.Compare(L"amp") == 0) {
          current_text_.push_back(L'&');
        } else if (csEntity.Compare(L"lt") == 0) {
          current_text_.push_back(L'<');
        } else if (csEntity.Compare(L"gt") == 0) {
          current_text_.push_back(L'>');
        } else if (csEntity.Compare(L"apos") == 0) {
          current_text_.push_back(L'\'');
        } else if (csEntity.Compare(L"quot") == 0) {
          current_text_.push_back(L'"');
        }
      }
    }

    entity_start_ = -1;
  } else if (entity_start_ < 0 && character == L'&') {
    entity_start_ = current_text_.size() - 1;
  }
}

void CFX_XMLParser::ProcessTargetData() {
  WideString target_data = GetTextData();
  if (target_data.IsEmpty())
    return;

  CFX_XMLInstruction* instruction = ToXMLInstruction(current_node_);
  if (instruction)
    instruction->AppendData(target_data);
}

WideString CFX_XMLParser::GetTextData() {
  WideString ret(current_text_.data(), current_text_.size());
  entity_start_ = -1;
  current_text_.clear();
  current_text_.reserve(kCurrentTextReserve);
  return ret;
}
