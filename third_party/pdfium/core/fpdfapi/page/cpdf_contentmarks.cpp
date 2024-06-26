// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_contentmarks.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "third_party/base/ptr_util.h"

CPDF_ContentMarks::CPDF_ContentMarks() {}

CPDF_ContentMarks::~CPDF_ContentMarks() {}

std::unique_ptr<CPDF_ContentMarks> CPDF_ContentMarks::Clone() {
  auto result = pdfium::MakeUnique<CPDF_ContentMarks>();
  if (m_pMarkData)
    result->m_pMarkData = pdfium::MakeRetain<MarkData>(*m_pMarkData);
  return result;
}

size_t CPDF_ContentMarks::CountItems() const {
  return m_pMarkData ? m_pMarkData->CountItems() : 0;
}

bool CPDF_ContentMarks::ContainsItem(const CPDF_ContentMarkItem* pItem) const {
  return m_pMarkData && m_pMarkData->ContainsItem(pItem);
}

CPDF_ContentMarkItem* CPDF_ContentMarks::GetItem(size_t index) {
  return const_cast<CPDF_ContentMarkItem*>(
      static_cast<const CPDF_ContentMarks*>(this)->GetItem(index));
}

const CPDF_ContentMarkItem* CPDF_ContentMarks::GetItem(size_t index) const {
  ASSERT(index < CountItems());
  return m_pMarkData->GetItem(index);
}

int CPDF_ContentMarks::GetMarkedContentID() const {
  return m_pMarkData ? m_pMarkData->GetMarkedContentID() : -1;
}

void CPDF_ContentMarks::AddMark(ByteString name) {
  EnsureMarkDataExists();
  m_pMarkData->AddMark(std::move(name));
}

void CPDF_ContentMarks::AddMarkWithDirectDict(ByteString name,
                                              CPDF_Dictionary* pDict) {
  EnsureMarkDataExists();
  m_pMarkData->AddMarkWithDirectDict(std::move(name), pDict);
}

void CPDF_ContentMarks::AddMarkWithPropertiesHolder(
    const ByteString& name,
    CPDF_Dictionary* pDict,
    const ByteString& property_name) {
  EnsureMarkDataExists();
  m_pMarkData->AddMarkWithPropertiesHolder(name, pDict, property_name);
}

bool CPDF_ContentMarks::RemoveMark(CPDF_ContentMarkItem* pMarkItem) {
  return m_pMarkData && m_pMarkData->RemoveMark(pMarkItem);
}

void CPDF_ContentMarks::EnsureMarkDataExists() {
  if (!m_pMarkData)
    m_pMarkData = pdfium::MakeRetain<MarkData>();
}

void CPDF_ContentMarks::DeleteLastMark() {
  if (!m_pMarkData)
    return;

  m_pMarkData->DeleteLastMark();
  if (CountItems() == 0)
    m_pMarkData.Reset();
}

size_t CPDF_ContentMarks::FindFirstDifference(
    const CPDF_ContentMarks* other) const {
  if (m_pMarkData == other->m_pMarkData)
    return CountItems();

  size_t min_len = std::min(CountItems(), other->CountItems());

  for (size_t i = 0; i < min_len; ++i) {
    if (GetItem(i) != other->GetItem(i))
      return i;
  }
  return min_len;
}

CPDF_ContentMarks::MarkData::MarkData() {}

CPDF_ContentMarks::MarkData::MarkData(const MarkData& src)
    : m_Marks(src.m_Marks) {}

CPDF_ContentMarks::MarkData::~MarkData() {}

size_t CPDF_ContentMarks::MarkData::CountItems() const {
  return m_Marks.size();
}

bool CPDF_ContentMarks::MarkData::ContainsItem(
    const CPDF_ContentMarkItem* pItem) const {
  for (const auto pMark : m_Marks) {
    if (pMark.Get() == pItem)
      return true;
  }
  return false;
}

CPDF_ContentMarkItem* CPDF_ContentMarks::MarkData::GetItem(size_t index) {
  return m_Marks[index].Get();
}

const CPDF_ContentMarkItem* CPDF_ContentMarks::MarkData::GetItem(
    size_t index) const {
  return m_Marks[index].Get();
}

int CPDF_ContentMarks::MarkData::GetMarkedContentID() const {
  for (const auto pMark : m_Marks) {
    const CPDF_Dictionary* pDict = pMark->GetParam();
    if (pDict && pDict->KeyExist("MCID"))
      return pDict->GetIntegerFor("MCID");
  }
  return -1;
}

void CPDF_ContentMarks::MarkData::AddMark(ByteString name) {
  auto pItem = pdfium::MakeRetain<CPDF_ContentMarkItem>(std::move(name));
  m_Marks.push_back(pItem);
}

void CPDF_ContentMarks::MarkData::AddMarkWithDirectDict(
    ByteString name,
    CPDF_Dictionary* pDict) {
  auto pItem = pdfium::MakeRetain<CPDF_ContentMarkItem>(std::move(name));
  pItem->SetDirectDict(ToDictionary(pDict->Clone()));
  m_Marks.push_back(pItem);
}

void CPDF_ContentMarks::MarkData::AddMarkWithPropertiesHolder(
    const ByteString& name,
    CPDF_Dictionary* pDict,
    const ByteString& property_name) {
  auto pItem = pdfium::MakeRetain<CPDF_ContentMarkItem>(std::move(name));
  pItem->SetPropertiesHolder(pDict, property_name);
  m_Marks.push_back(pItem);
}

bool CPDF_ContentMarks::MarkData::RemoveMark(CPDF_ContentMarkItem* pMarkItem) {
  for (auto it = m_Marks.begin(); it != m_Marks.end(); ++it) {
    if (it->Get() == pMarkItem) {
      m_Marks.erase(it);
      return true;
    }
  }
  return false;
}

void CPDF_ContentMarks::MarkData::DeleteLastMark() {
  if (!m_Marks.empty())
    m_Marks.pop_back();
}
