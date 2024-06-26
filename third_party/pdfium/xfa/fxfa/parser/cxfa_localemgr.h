// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_PARSER_CXFA_LOCALEMGR_H_
#define XFA_FXFA_PARSER_CXFA_LOCALEMGR_H_

#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxcrt/widestring.h"

class CXFA_Node;
class LocaleIface;

class CXFA_LocaleMgr {
 public:
  CXFA_LocaleMgr(CXFA_Node* pLocaleSet, WideString wsDeflcid);
  ~CXFA_LocaleMgr();

  LocaleIface* GetDefLocale();
  LocaleIface* GetLocaleByName(const WideString& wsLocaleName);

  void SetDefLocale(LocaleIface* pLocale);
  WideString GetConfigLocaleName(CXFA_Node* pConfig);

 private:
  std::unique_ptr<LocaleIface> GetLocale(uint16_t lcid);

  std::vector<std::unique_ptr<LocaleIface>> m_LocaleArray;
  std::vector<std::unique_ptr<LocaleIface>> m_XMLLocaleArray;

  // Owned by m_LocaleArray or m_XMLLocaleArray.
  UnownedPtr<LocaleIface> m_pDefLocale;

  WideString m_wsConfigLocale;
  uint16_t m_dwDeflcid;
  bool m_hasSetLocaleName = false;
};

#endif  // XFA_FXFA_PARSER_CXFA_LOCALEMGR_H_
