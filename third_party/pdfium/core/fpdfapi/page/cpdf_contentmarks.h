// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_CONTENTMARKS_H_
#define CORE_FPDFAPI_PAGE_CPDF_CONTENTMARKS_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_contentmarkitem.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"

class CPDF_Dictionary;

class CPDF_ContentMarks {
 public:
  CPDF_ContentMarks();
  ~CPDF_ContentMarks();

  std::unique_ptr<CPDF_ContentMarks> Clone();
  int GetMarkedContentID() const;
  size_t CountItems() const;
  bool ContainsItem(const CPDF_ContentMarkItem* pItem) const;

  // The returned pointer is never null.
  CPDF_ContentMarkItem* GetItem(size_t index);
  const CPDF_ContentMarkItem* GetItem(size_t index) const;

  void AddMark(ByteString name);
  void AddMarkWithDirectDict(ByteString name, CPDF_Dictionary* pDict);
  void AddMarkWithPropertiesHolder(const ByteString& name,
                                   CPDF_Dictionary* pHolder,
                                   const ByteString& property_name);
  bool RemoveMark(CPDF_ContentMarkItem* pMarkItem);
  void DeleteLastMark();
  size_t FindFirstDifference(const CPDF_ContentMarks* other) const;

 private:
  class MarkData final : public Retainable {
   public:
    MarkData();
    MarkData(const MarkData& src);
    ~MarkData() override;

    size_t CountItems() const;
    bool ContainsItem(const CPDF_ContentMarkItem* pItem) const;
    CPDF_ContentMarkItem* GetItem(size_t index);
    const CPDF_ContentMarkItem* GetItem(size_t index) const;

    int GetMarkedContentID() const;
    void AddMark(ByteString name);
    void AddMarkWithDirectDict(ByteString name, CPDF_Dictionary* pDict);
    void AddMarkWithPropertiesHolder(const ByteString& name,
                                     CPDF_Dictionary* pHolder,
                                     const ByteString& property_name);
    bool RemoveMark(CPDF_ContentMarkItem* pMarkItem);
    void DeleteLastMark();

   private:
    std::vector<RetainPtr<CPDF_ContentMarkItem>> m_Marks;
  };

  void EnsureMarkDataExists();

  RetainPtr<MarkData> m_pMarkData;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_CONTENTMARKS_H_
