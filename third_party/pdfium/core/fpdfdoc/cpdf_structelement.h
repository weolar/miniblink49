// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFDOC_CPDF_STRUCTELEMENT_H_
#define CORE_FPDFDOC_CPDF_STRUCTELEMENT_H_

#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Dictionary;
class CPDF_Object;
class CPDF_StructElement;
class CPDF_StructTree;

class CPDF_StructKid {
 public:
  CPDF_StructKid();
  CPDF_StructKid(const CPDF_StructKid& that);
  ~CPDF_StructKid();

  enum { Invalid, Element, PageContent, StreamContent, Object } m_Type;

  RetainPtr<CPDF_StructElement> m_pElement;      // For Element.
  UnownedPtr<const CPDF_Dictionary> m_pDict;     // For Element.
  uint32_t m_PageObjNum;  // For PageContent, StreamContent, Object.
  uint32_t m_RefObjNum;   // For StreamContent, Object.
  uint32_t m_ContentId;   // For PageContent, StreamContent.
};

class CPDF_StructElement final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  ByteString GetType() const { return m_Type; }
  ByteString GetTitle() const { return m_Title; }
  const CPDF_Dictionary* GetDict() const { return m_pDict.Get(); }

  size_t CountKids() const;
  CPDF_StructElement* GetKidIfElement(size_t index) const;
  std::vector<CPDF_StructKid>* GetKids() { return &m_Kids; }

 private:
  CPDF_StructElement(CPDF_StructTree* pTree,
                     CPDF_StructElement* pParent,
                     const CPDF_Dictionary* pDict);
  ~CPDF_StructElement() override;

  void LoadKids(const CPDF_Dictionary* pDict);
  void LoadKid(uint32_t PageObjNum,
               const CPDF_Object* pObj,
               CPDF_StructKid* pKid);

  UnownedPtr<CPDF_StructTree> const m_pTree;
  UnownedPtr<CPDF_StructElement> const m_pParent;
  UnownedPtr<const CPDF_Dictionary> const m_pDict;
  ByteString m_Type;
  ByteString m_Title;
  std::vector<CPDF_StructKid> m_Kids;
};

#endif  // CORE_FPDFDOC_CPDF_STRUCTELEMENT_H_
