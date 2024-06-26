// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_INTERACTIVEFORM_H_
#define CORE_FPDFDOC_CPDF_INTERACTIVEFORM_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfdoc/cpdf_defaultappearance.h"
#include "core/fpdfdoc/cpdf_formfield.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CFieldTree;
class CFDF_Document;
class CPDF_Document;
class CPDF_Dictionary;
class CPDF_Font;
class CPDF_FormControl;
class CPDF_Object;
class CPDF_Page;
class IPDF_FormNotify;

CPDF_Font* AddNativeInteractiveFormFont(CPDF_Dictionary*& pFormDict,
                                        CPDF_Document* pDocument,
                                        ByteString* csNameTag);

class CPDF_InteractiveForm {
 public:
  explicit CPDF_InteractiveForm(CPDF_Document* pDocument);
  ~CPDF_InteractiveForm();

  static void SetUpdateAP(bool bUpdateAP);
  static bool IsUpdateAPEnabled();
  static CPDF_Font* AddStandardFont(CPDF_Document* pDocument,
                                    ByteString csFontName);
  static ByteString GetNativeFont(uint8_t iCharSet, void* pLogFont);
  static uint8_t GetNativeCharSet();
  static CPDF_Font* AddNativeFont(uint8_t iCharSet, CPDF_Document* pDocument);
  static CPDF_Font* AddNativeFont(CPDF_Document* pDocument);

  size_t CountFields(const WideString& csFieldName) const;
  CPDF_FormField* GetField(uint32_t index, const WideString& csFieldName) const;
  CPDF_FormField* GetFieldByDict(CPDF_Dictionary* pFieldDict) const;

  CPDF_FormControl* GetControlAtPoint(CPDF_Page* pPage,
                                      const CFX_PointF& point,
                                      int* z_order) const;
  CPDF_FormControl* GetControlByDict(const CPDF_Dictionary* pWidgetDict) const;

  bool NeedConstructAP() const;
  int CountFieldsInCalculationOrder();
  CPDF_FormField* GetFieldInCalculationOrder(int index);
  int FindFieldInCalculationOrder(const CPDF_FormField* pField);

  CPDF_Font* GetFormFont(ByteString csNameTag) const;
  CPDF_DefaultAppearance GetDefaultAppearance() const;
  int GetFormAlignment() const;

  bool CheckRequiredFields(const std::vector<CPDF_FormField*>* fields,
                           bool bIncludeOrExclude) const;

  std::unique_ptr<CFDF_Document> ExportToFDF(const WideString& pdf_path,
                                             bool bSimpleFileSpec) const;

  std::unique_ptr<CFDF_Document> ExportToFDF(
      const WideString& pdf_path,
      const std::vector<CPDF_FormField*>& fields,
      bool bIncludeOrExclude,
      bool bSimpleFileSpec) const;

  void ResetForm(NotificationOption notify);

  // TODO(tsepez): Use a span.
  void ResetForm(const std::vector<CPDF_FormField*>& fields,
                 bool bIncludeOrExclude,
                 NotificationOption notify);

  void SetFormNotify(IPDF_FormNotify* pNotify);
  bool HasXFAForm() const;
  void FixPageFields(CPDF_Page* pPage);

  IPDF_FormNotify* GetFormNotify() const { return m_pFormNotify.Get(); }
  CPDF_Document* GetDocument() const { return m_pDocument.Get(); }
  CPDF_Dictionary* GetFormDict() const { return m_pFormDict.Get(); }

  const std::vector<UnownedPtr<CPDF_FormControl>>& GetControlsForField(
      const CPDF_FormField* pField);

 private:
  void LoadField(CPDF_Dictionary* pFieldDict, int nLevel);
  void AddTerminalField(CPDF_Dictionary* pFieldDict);
  CPDF_FormControl* AddControl(CPDF_FormField* pField,
                               CPDF_Dictionary* pWidgetDict);

  static bool s_bUpdateAP;

  ByteString m_bsEncoding;
  UnownedPtr<CPDF_Document> const m_pDocument;
  UnownedPtr<CPDF_Dictionary> m_pFormDict;
  std::unique_ptr<CFieldTree> m_pFieldTree;
  std::map<const CPDF_Dictionary*, std::unique_ptr<CPDF_FormControl>>
      m_ControlMap;
  // Points into |m_ControlMap|.
  std::map<const CPDF_FormField*, std::vector<UnownedPtr<CPDF_FormControl>>>
      m_ControlLists;
  UnownedPtr<IPDF_FormNotify> m_pFormNotify;
};

#endif  // CORE_FPDFDOC_CPDF_INTERACTIVEFORM_H_
