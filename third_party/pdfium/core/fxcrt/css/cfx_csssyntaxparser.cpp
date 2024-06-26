// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_csssyntaxparser.h"

#include <algorithm>

#include "core/fxcrt/css/cfx_cssdata.h"
#include "core/fxcrt/css/cfx_cssdeclaration.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/compiler_specific.h"
#include "third_party/base/logging.h"

namespace {

bool IsSelectorStart(wchar_t wch) {
  return wch == '.' || wch == '#' || wch == '*' ||
         (isascii(wch) && isalpha(wch));
}

}  // namespace

CFX_CSSSyntaxParser::CFX_CSSSyntaxParser(const wchar_t* pBuffer,
                                         int32_t iBufferSize)
    : CFX_CSSSyntaxParser(pBuffer, iBufferSize, 32, false) {}

CFX_CSSSyntaxParser::CFX_CSSSyntaxParser(const wchar_t* pBuffer,
                                         int32_t iBufferSize,
                                         int32_t iTextDatSize,
                                         bool bOnlyDeclaration)
    : m_iTextDataLen(0),
      m_dwCheck(0xFFFFFFFF),
      m_eStatus(CFX_CSSSyntaxStatus::None) {
  ASSERT(pBuffer);
  ASSERT(iBufferSize > 0);
  ASSERT(iTextDatSize > 0);

  m_eMode = bOnlyDeclaration ? CFX_CSSSyntaxMode::PropertyName
                             : CFX_CSSSyntaxMode::RuleSet;
  m_TextData.InitWithSize(iTextDatSize);
  m_TextPlane.AttachBuffer(pBuffer, iBufferSize);
}

CFX_CSSSyntaxParser::~CFX_CSSSyntaxParser() {}

CFX_CSSSyntaxStatus CFX_CSSSyntaxParser::DoSyntaxParse() {
  while (m_eStatus >= CFX_CSSSyntaxStatus::None) {
    if (m_TextPlane.IsEOF()) {
      if (m_eMode == CFX_CSSSyntaxMode::PropertyValue &&
          m_TextData.GetLength() > 0) {
        SaveTextData();
        m_eStatus = CFX_CSSSyntaxStatus::PropertyValue;
        return m_eStatus;
      }
      m_eStatus = CFX_CSSSyntaxStatus::EOS;
      return m_eStatus;
    }
    wchar_t wch;
    while (!m_TextPlane.IsEOF()) {
      wch = m_TextPlane.GetChar();
      switch (m_eMode) {
        case CFX_CSSSyntaxMode::RuleSet:
          switch (wch) {
            case '}':
              m_TextPlane.MoveNext();
              if (RestoreMode())
                return CFX_CSSSyntaxStatus::DeclClose;

              m_eStatus = CFX_CSSSyntaxStatus::Error;
              return m_eStatus;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                m_ModeStack.push(m_eMode);
                SwitchMode(CFX_CSSSyntaxMode::Comment);
                break;
              }
              FALLTHROUGH;
            default:
              if (wch <= ' ') {
                m_TextPlane.MoveNext();
              } else if (IsSelectorStart(wch)) {
                SwitchMode(CFX_CSSSyntaxMode::Selector);
                return CFX_CSSSyntaxStatus::StyleRule;
              } else {
                m_eStatus = CFX_CSSSyntaxStatus::Error;
                return m_eStatus;
              }
              break;
          }
          break;
        case CFX_CSSSyntaxMode::Selector:
          switch (wch) {
            case ',':
              m_TextPlane.MoveNext();
              SwitchMode(CFX_CSSSyntaxMode::Selector);
              if (m_iTextDataLen > 0)
                return CFX_CSSSyntaxStatus::Selector;
              break;
            case '{':
              if (m_TextData.GetLength() > 0) {
                SaveTextData();
                return CFX_CSSSyntaxStatus::Selector;
              }
              m_TextPlane.MoveNext();
              m_ModeStack.push(CFX_CSSSyntaxMode::RuleSet);
              SwitchMode(CFX_CSSSyntaxMode::PropertyName);
              return CFX_CSSSyntaxStatus::DeclOpen;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0)
                  return CFX_CSSSyntaxStatus::Selector;
                break;
              }
              FALLTHROUGH;
            default:
              AppendChar(wch);
              break;
          }
          break;
        case CFX_CSSSyntaxMode::PropertyName:
          switch (wch) {
            case ':':
              m_TextPlane.MoveNext();
              SwitchMode(CFX_CSSSyntaxMode::PropertyValue);
              return CFX_CSSSyntaxStatus::PropertyName;
            case '}':
              m_TextPlane.MoveNext();
              if (RestoreMode())
                return CFX_CSSSyntaxStatus::DeclClose;

              m_eStatus = CFX_CSSSyntaxStatus::Error;
              return m_eStatus;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0)
                  return CFX_CSSSyntaxStatus::PropertyName;
                break;
              }
              FALLTHROUGH;
            default:
              AppendChar(wch);
              break;
          }
          break;
        case CFX_CSSSyntaxMode::PropertyValue:
          switch (wch) {
            case ';':
              m_TextPlane.MoveNext();
              FALLTHROUGH;
            case '}':
              SwitchMode(CFX_CSSSyntaxMode::PropertyName);
              return CFX_CSSSyntaxStatus::PropertyValue;
            case '/':
              if (m_TextPlane.GetNextChar() == '*') {
                if (SwitchToComment() > 0)
                  return CFX_CSSSyntaxStatus::PropertyValue;
                break;
              }
              FALLTHROUGH;
            default:
              AppendChar(wch);
              break;
          }
          break;
        case CFX_CSSSyntaxMode::Comment:
          if (wch == '/' && m_TextData.GetLength() > 0 &&
              m_TextData.GetBuffer()[m_TextData.GetLength() - 1] == '*') {
            RestoreMode();
          } else {
            m_TextData.AppendChar(wch);
          }
          m_TextPlane.MoveNext();
          break;
        case CFX_CSSSyntaxMode::UnknownRule:
          if (wch == ';')
            SwitchMode(CFX_CSSSyntaxMode::RuleSet);
          m_TextPlane.MoveNext();
          break;
        default:
          NOTREACHED();
          break;
      }
    }
  }
  return m_eStatus;
}

bool CFX_CSSSyntaxParser::IsImportEnabled() const {
  if ((m_dwCheck & CFX_CSSSYNTAXCHECK_AllowImport) == 0)
    return false;
  if (m_ModeStack.size() > 1)
    return false;
  return true;
}

bool CFX_CSSSyntaxParser::AppendChar(wchar_t wch) {
  m_TextPlane.MoveNext();
  if (m_TextData.GetLength() > 0 || wch > ' ') {
    m_TextData.AppendChar(wch);
    return true;
  }
  return false;
}

int32_t CFX_CSSSyntaxParser::SaveTextData() {
  m_iTextDataLen = m_TextData.TrimEnd();
  m_TextData.Clear();
  return m_iTextDataLen;
}

void CFX_CSSSyntaxParser::SwitchMode(CFX_CSSSyntaxMode eMode) {
  m_eMode = eMode;
  SaveTextData();
}

int32_t CFX_CSSSyntaxParser::SwitchToComment() {
  int32_t iLength = m_TextData.GetLength();
  m_ModeStack.push(m_eMode);
  SwitchMode(CFX_CSSSyntaxMode::Comment);
  return iLength;
}

bool CFX_CSSSyntaxParser::RestoreMode() {
  if (m_ModeStack.empty())
    return false;

  SwitchMode(m_ModeStack.top());
  m_ModeStack.pop();
  return true;
}

WideStringView CFX_CSSSyntaxParser::GetCurrentString() const {
  return WideStringView(m_TextData.GetBuffer(), m_iTextDataLen);
}
