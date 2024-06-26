// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cfx_globaldata.h"

#include <utility>

#include "core/fdrm/fx_crypt.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

constexpr size_t kMinGlobalDataBytes = 12;
constexpr size_t kMaxGlobalDataBytes = 4 * 1024 - 8;
constexpr uint16_t kMagic = ('X' << 8) | 'F';
constexpr uint16_t kMaxVersion = 2;

const uint8_t kRC4KEY[] = {
    0x19, 0xa8, 0xe8, 0x01, 0xf6, 0xa8, 0xb6, 0x4d, 0x82, 0x04, 0x45, 0x6d,
    0xb4, 0xcf, 0xd7, 0x77, 0x67, 0xf9, 0x75, 0x9f, 0xf0, 0xe0, 0x1e, 0x51,
    0xee, 0x46, 0xfd, 0x0b, 0xc9, 0x93, 0x25, 0x55, 0x4a, 0xee, 0xe0, 0x16,
    0xd0, 0xdf, 0x8c, 0xfa, 0x2a, 0xa9, 0x49, 0xfd, 0x97, 0x1c, 0x0e, 0x22,
    0x13, 0x28, 0x7c, 0xaf, 0xc4, 0xfc, 0x9c, 0x12, 0x65, 0x8c, 0x4e, 0x5b,
    0x04, 0x75, 0x89, 0xc9, 0xb1, 0xed, 0x50, 0xca, 0x96, 0x6f, 0x1a, 0x7a,
    0xfe, 0x58, 0x5d, 0xec, 0x19, 0x4a, 0xf6, 0x35, 0x6a, 0x97, 0x14, 0x00,
    0x0e, 0xd0, 0x6b, 0xbb, 0xd5, 0x75, 0x55, 0x8b, 0x6e, 0x6b, 0x19, 0xa0,
    0xf8, 0x77, 0xd5, 0xa3};

CFX_GlobalData* g_pInstance = nullptr;

// Returns true if non-empty, setting sPropName
bool TrimPropName(ByteString* sPropName) {
  sPropName->Trim();
  return sPropName->GetLength() != 0;
}

void MakeNameTypeString(const ByteString& name,
                        CFX_Value::DataType eType,
                        CFX_BinaryBuf* result) {
  uint32_t dwNameLen = (uint32_t)name.GetLength();
  result->AppendBlock(&dwNameLen, sizeof(uint32_t));
  result->AppendString(name);

  uint16_t wType = static_cast<uint16_t>(eType);
  result->AppendBlock(&wType, sizeof(uint16_t));
}

bool MakeByteString(const ByteString& name,
                    const CFX_KeyValue& pData,
                    CFX_BinaryBuf* result) {
  switch (pData.nType) {
    case CFX_Value::DataType::NUMBER: {
      MakeNameTypeString(name, pData.nType, result);
      double dData = pData.dData;
      result->AppendBlock(&dData, sizeof(double));
      return true;
    }
    case CFX_Value::DataType::BOOLEAN: {
      MakeNameTypeString(name, pData.nType, result);
      uint16_t wData = static_cast<uint16_t>(pData.bData);
      result->AppendBlock(&wData, sizeof(uint16_t));
      return true;
    }
    case CFX_Value::DataType::STRING: {
      MakeNameTypeString(name, pData.nType, result);
      uint32_t dwDataLen = (uint32_t)pData.sData.GetLength();
      result->AppendBlock(&dwDataLen, sizeof(uint32_t));
      result->AppendString(pData.sData);
      return true;
    }
    case CFX_Value::DataType::NULLOBJ: {
      MakeNameTypeString(name, pData.nType, result);
      return true;
    }
    // Arrays don't get persisted per JS spec page 484.
    case CFX_Value::DataType::OBJECT:
    default:
      break;
  }
  return false;
}

}  // namespace

// static
CFX_GlobalData* CFX_GlobalData::GetRetainedInstance(Delegate* pDelegate) {
  if (!g_pInstance) {
    g_pInstance = new CFX_GlobalData(pDelegate);
  }
  ++g_pInstance->m_RefCount;
  return g_pInstance;
}

bool CFX_GlobalData::Release() {
  if (--m_RefCount)
    return false;

  delete g_pInstance;
  g_pInstance = nullptr;
  return true;
}

CFX_GlobalData::CFX_GlobalData(Delegate* pDelegate) : m_pDelegate(pDelegate) {
  LoadGlobalPersistentVariables();
}

CFX_GlobalData::~CFX_GlobalData() {
  SaveGlobalPersisitentVariables();
}

CFX_GlobalData::iterator CFX_GlobalData::FindGlobalVariable(
    const ByteString& propname) {
  for (auto it = m_arrayGlobalData.begin(); it != m_arrayGlobalData.end();
       ++it) {
    if ((*it)->data.sKey == propname)
      return it;
  }
  return m_arrayGlobalData.end();
}

CFX_GlobalData::const_iterator CFX_GlobalData::FindGlobalVariable(
    const ByteString& propname) const {
  for (auto it = m_arrayGlobalData.begin(); it != m_arrayGlobalData.end();
       ++it) {
    if ((*it)->data.sKey == propname)
      return it;
  }
  return m_arrayGlobalData.end();
}

CFX_GlobalData::Element* CFX_GlobalData::GetGlobalVariable(
    const ByteString& propname) {
  auto iter = FindGlobalVariable(propname);
  return iter != m_arrayGlobalData.end() ? iter->get() : nullptr;
}

void CFX_GlobalData::SetGlobalVariableNumber(ByteString sPropName,
                                             double dData) {
  if (!TrimPropName(&sPropName))
    return;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (pData) {
    pData->data.nType = CFX_Value::DataType::NUMBER;
    pData->data.dData = dData;
    return;
  }
  auto pNewData = pdfium::MakeUnique<CFX_GlobalData::Element>();
  pNewData->data.sKey = std::move(sPropName);
  pNewData->data.nType = CFX_Value::DataType::NUMBER;
  pNewData->data.dData = dData;
  m_arrayGlobalData.push_back(std::move(pNewData));
}

void CFX_GlobalData::SetGlobalVariableBoolean(ByteString sPropName,
                                              bool bData) {
  if (!TrimPropName(&sPropName))
    return;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (pData) {
    pData->data.nType = CFX_Value::DataType::BOOLEAN;
    pData->data.bData = bData;
    return;
  }
  auto pNewData = pdfium::MakeUnique<CFX_GlobalData::Element>();
  pNewData->data.sKey = std::move(sPropName);
  pNewData->data.nType = CFX_Value::DataType::BOOLEAN;
  pNewData->data.bData = bData;
  m_arrayGlobalData.push_back(std::move(pNewData));
}

void CFX_GlobalData::SetGlobalVariableString(ByteString sPropName,
                                             const ByteString& sData) {
  if (!TrimPropName(&sPropName))
    return;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (pData) {
    pData->data.nType = CFX_Value::DataType::STRING;
    pData->data.sData = sData;
    return;
  }
  auto pNewData = pdfium::MakeUnique<CFX_GlobalData::Element>();
  pNewData->data.sKey = std::move(sPropName);
  pNewData->data.nType = CFX_Value::DataType::STRING;
  pNewData->data.sData = sData;
  m_arrayGlobalData.push_back(std::move(pNewData));
}

void CFX_GlobalData::SetGlobalVariableObject(ByteString sPropName,
                                             CFX_GlobalArray array) {
  if (!TrimPropName(&sPropName))
    return;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (pData) {
    pData->data.nType = CFX_Value::DataType::OBJECT;
    pData->data.objData = std::move(array);
    return;
  }
  auto pNewData = pdfium::MakeUnique<CFX_GlobalData::Element>();
  pNewData->data.sKey = std::move(sPropName);
  pNewData->data.nType = CFX_Value::DataType::OBJECT;
  pNewData->data.objData = std::move(array);
  m_arrayGlobalData.push_back(std::move(pNewData));
}

void CFX_GlobalData::SetGlobalVariableNull(ByteString sPropName) {
  if (!TrimPropName(&sPropName))
    return;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (pData) {
    pData->data.nType = CFX_Value::DataType::NULLOBJ;
    return;
  }
  auto pNewData = pdfium::MakeUnique<CFX_GlobalData::Element>();
  pNewData->data.sKey = std::move(sPropName);
  pNewData->data.nType = CFX_Value::DataType::NULLOBJ;
  m_arrayGlobalData.push_back(std::move(pNewData));
}

bool CFX_GlobalData::SetGlobalVariablePersistent(ByteString sPropName,
                                                 bool bPersistent) {
  if (!TrimPropName(&sPropName))
    return false;

  CFX_GlobalData::Element* pData = GetGlobalVariable(sPropName);
  if (!pData)
    return false;

  pData->bPersistent = bPersistent;
  return true;
}

bool CFX_GlobalData::DeleteGlobalVariable(ByteString sPropName) {
  if (!TrimPropName(&sPropName))
    return false;

  auto iter = FindGlobalVariable(sPropName);
  if (iter == m_arrayGlobalData.end())
    return false;

  m_arrayGlobalData.erase(iter);
  return true;
}

int32_t CFX_GlobalData::GetSize() const {
  return pdfium::CollectionSize<int32_t>(m_arrayGlobalData);
}

CFX_GlobalData::Element* CFX_GlobalData::GetAt(int index) {
  if (index < 0 || index >= GetSize())
    return nullptr;
  return m_arrayGlobalData[index].get();
}

bool CFX_GlobalData::LoadGlobalPersistentVariables() {
  if (!m_pDelegate)
    return false;

  bool ret;
  {
    // Span can't outlive call to BufferDone().
    Optional<pdfium::span<uint8_t>> buffer = m_pDelegate->LoadBuffer();
    if (!buffer.has_value() || buffer.value().empty())
      return false;

    ret = LoadGlobalPersistentVariablesFromBuffer(buffer.value());
  }
  m_pDelegate->BufferDone();
  return ret;
}

bool CFX_GlobalData::LoadGlobalPersistentVariablesFromBuffer(
    pdfium::span<uint8_t> buffer) {
  if (buffer.size() < kMinGlobalDataBytes)
    return false;

  CRYPT_ArcFourCryptBlock(buffer.data(), buffer.size(), kRC4KEY,
                          sizeof(kRC4KEY));

  uint8_t* p = buffer.data();
  uint16_t wType = *((uint16_t*)p);
  p += sizeof(uint16_t);
  if (wType != kMagic)
    return false;

  uint16_t wVersion = *((uint16_t*)p);
  p += sizeof(uint16_t);
  if (wVersion > kMaxVersion)
    return false;

  uint32_t dwCount = *((uint32_t*)p);
  p += sizeof(uint32_t);

  uint32_t dwSize = *((uint32_t*)p);
  p += sizeof(uint32_t);

  if (dwSize != buffer.size() - sizeof(uint16_t) * 2 - sizeof(uint32_t) * 2)
    return false;

  for (int32_t i = 0, sz = dwCount; i < sz; i++) {
    if (p > buffer.end())
      break;

    uint32_t dwNameLen = *((uint32_t*)p);
    p += sizeof(uint32_t);
    if (p + dwNameLen > buffer.end())
      break;

    ByteString sEntry = ByteString(p, dwNameLen);
    p += sizeof(char) * dwNameLen;

    CFX_Value::DataType wDataType =
        static_cast<CFX_Value::DataType>(*((uint16_t*)p));
    p += sizeof(uint16_t);

    switch (wDataType) {
      case CFX_Value::DataType::NUMBER: {
        double dData = 0;
        switch (wVersion) {
          case 1: {
            uint32_t dwData = *((uint32_t*)p);
            p += sizeof(uint32_t);
            dData = dwData;
          } break;
          case 2: {
            dData = *((double*)p);
            p += sizeof(double);
          } break;
        }
        SetGlobalVariableNumber(sEntry, dData);
        SetGlobalVariablePersistent(sEntry, true);
      } break;
      case CFX_Value::DataType::BOOLEAN: {
        uint16_t wData = *((uint16_t*)p);
        p += sizeof(uint16_t);
        SetGlobalVariableBoolean(sEntry, (bool)(wData == 1));
        SetGlobalVariablePersistent(sEntry, true);
      } break;
      case CFX_Value::DataType::STRING: {
        uint32_t dwLength = *((uint32_t*)p);
        p += sizeof(uint32_t);
        if (p + dwLength > buffer.end())
          break;

        SetGlobalVariableString(sEntry, ByteString(p, dwLength));
        SetGlobalVariablePersistent(sEntry, true);
        p += sizeof(char) * dwLength;
      } break;
      case CFX_Value::DataType::NULLOBJ: {
        SetGlobalVariableNull(sEntry);
        SetGlobalVariablePersistent(sEntry, true);
      } break;
      case CFX_Value::DataType::OBJECT:
      default:
        // Arrays aren't allowed in these buffers, nor are unrecoginzed tags.
        return false;
    }
  }
  return true;
}

bool CFX_GlobalData::SaveGlobalPersisitentVariables() {
  if (!m_pDelegate)
    return false;

  uint32_t nCount = 0;
  CFX_BinaryBuf sData;
  for (const auto& pElement : m_arrayGlobalData) {
    if (!pElement->bPersistent)
      continue;

    CFX_BinaryBuf sElement;
    if (!MakeByteString(pElement->data.sKey, pElement->data, &sElement))
      continue;

    if (sData.GetSize() + sElement.GetSize() > kMaxGlobalDataBytes)
      break;

    sData.AppendBlock(sElement.GetBuffer(), sElement.GetSize());
    nCount++;
  }

  CFX_BinaryBuf sFile;
  uint16_t wType = kMagic;
  uint16_t wVersion = 2;
  sFile.AppendBlock(&wType, sizeof(uint16_t));
  sFile.AppendBlock(&wVersion, sizeof(uint16_t));
  sFile.AppendBlock(&nCount, sizeof(uint32_t));

  uint32_t dwSize = sData.GetSize();
  sFile.AppendBlock(&dwSize, sizeof(uint32_t));
  sFile.AppendBlock(sData.GetBuffer(), sData.GetSize());

  CRYPT_ArcFourCryptBlock(sFile.GetBuffer(), sFile.GetSize(), kRC4KEY,
                          sizeof(kRC4KEY));

  return m_pDelegate->StoreBuffer({sFile.GetBuffer(), sFile.GetSize()});
}

CFX_GlobalData::Element::Element() = default;

CFX_GlobalData::Element::~Element() = default;
