// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LOCALEVALUE_H_
#define XFA_FXFA_PARSER_CXFA_LOCALEVALUE_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fxfa/parser/cxfa_node.h"

class LocaleIface;
class CFX_DateTime;
class CXFA_LocaleMgr;

#define XFA_VT_NULL 0
#define XFA_VT_BOOLEAN 1
#define XFA_VT_INTEGER 2
#define XFA_VT_DECIMAL 4
#define XFA_VT_FLOAT 8
#define XFA_VT_TEXT 16
#define XFA_VT_DATE 32
#define XFA_VT_TIME 64
#define XFA_VT_DATETIME 128

class CXFA_LocaleValue {
 public:
  CXFA_LocaleValue();
  CXFA_LocaleValue(uint32_t dwType, CXFA_LocaleMgr* pLocaleMgr);
  CXFA_LocaleValue(uint32_t dwType,
                   const WideString& wsValue,
                   CXFA_LocaleMgr* pLocaleMgr);
  CXFA_LocaleValue(uint32_t dwType,
                   const WideString& wsValue,
                   const WideString& wsFormat,
                   LocaleIface* pLocale,
                   CXFA_LocaleMgr* pLocaleMgr);
  CXFA_LocaleValue(const CXFA_LocaleValue& that);
  ~CXFA_LocaleValue();

  CXFA_LocaleValue& operator=(const CXFA_LocaleValue& that);

  bool ValidateValue(const WideString& wsValue,
                     const WideString& wsPattern,
                     LocaleIface* pLocale,
                     WideString* pMatchFormat);

  bool FormatPatterns(WideString& wsResult,
                      const WideString& wsFormat,
                      LocaleIface* pLocale,
                      XFA_VALUEPICTURE eValueType) const;

  void GetNumericFormat(WideString& wsFormat, int32_t nIntLen, int32_t nDecLen);
  bool ValidateNumericTemp(const WideString& wsNumeric,
                           const WideString& wsFormat,
                           LocaleIface* pLocale);

  bool IsValid() const { return m_bValid; }
  const WideString& GetValue() const { return m_wsValue; }
  uint32_t GetType() const { return m_dwType; }
  double GetDoubleNum() const;
  bool SetDate(const CFX_DateTime& d);
  CFX_DateTime GetDate() const;
  CFX_DateTime GetTime() const;

 private:
  bool FormatSinglePattern(WideString& wsResult,
                           const WideString& wsFormat,
                           LocaleIface* pLocale,
                           XFA_VALUEPICTURE eValueType) const;
  bool ValidateCanonicalValue(const WideString& wsValue, uint32_t dwVType);
  bool ValidateCanonicalDate(const WideString& wsDate, CFX_DateTime* unDate);
  bool ValidateCanonicalTime(const WideString& wsTime);

  bool SetTime(const CFX_DateTime& t);
  bool SetDateTime(const CFX_DateTime& dt);

  bool ParsePatternValue(const WideString& wsValue,
                         const WideString& wsPattern,
                         LocaleIface* pLocale);

  UnownedPtr<CXFA_LocaleMgr> m_pLocaleMgr;
  WideString m_wsValue;
  uint32_t m_dwType = XFA_VT_NULL;
  bool m_bValid = true;
};

#endif  // XFA_FXFA_PARSER_CXFA_LOCALEVALUE_H_
