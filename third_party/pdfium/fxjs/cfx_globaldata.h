// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CFX_GLOBALDATA_H_
#define FXJS_CFX_GLOBALDATA_H_

#include <memory>
#include <vector>

#include "core/fxcrt/cfx_binarybuf.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fxjs/cfx_keyvalue.h"
#include "third_party/base/optional.h"
#include "third_party/base/span.h"

class CPDFSDK_FormFillEnvironment;

class CFX_GlobalData {
 public:
  class Delegate {
   public:
    virtual ~Delegate() {}

    virtual bool StoreBuffer(pdfium::span<const uint8_t> pBuffer) = 0;
    virtual Optional<pdfium::span<uint8_t>> LoadBuffer() = 0;
    virtual void BufferDone() = 0;
  };

  class Element {
   public:
    Element();
    ~Element();

    CFX_KeyValue data;
    bool bPersistent;
  };

  static CFX_GlobalData* GetRetainedInstance(Delegate* pDelegate);
  bool Release();

  void SetGlobalVariableNumber(ByteString propname, double dData);
  void SetGlobalVariableBoolean(ByteString propname, bool bData);
  void SetGlobalVariableString(ByteString propname, const ByteString& sData);
  void SetGlobalVariableObject(ByteString propname, CFX_GlobalArray array);
  void SetGlobalVariableNull(ByteString propname);
  bool SetGlobalVariablePersistent(ByteString propname, bool bPersistent);
  bool DeleteGlobalVariable(ByteString propname);

  int32_t GetSize() const;
  Element* GetAt(int index);

  // Exposed for testing.
  Element* GetGlobalVariable(const ByteString& sPropname);

 private:
  using iterator = std::vector<std::unique_ptr<Element>>::iterator;
  using const_iterator = std::vector<std::unique_ptr<Element>>::const_iterator;

  explicit CFX_GlobalData(Delegate* pDelegate);
  ~CFX_GlobalData();

  bool LoadGlobalPersistentVariables();
  bool LoadGlobalPersistentVariablesFromBuffer(pdfium::span<uint8_t> buffer);
  bool SaveGlobalPersisitentVariables();

  iterator FindGlobalVariable(const ByteString& sPropname);
  const_iterator FindGlobalVariable(const ByteString& sPropname) const;

  void LoadFileBuffer(const wchar_t* sFilePath,
                      uint8_t*& pBuffer,
                      int32_t& nLength);
  void WriteFileBuffer(const wchar_t* sFilePath,
                       const char* pBuffer,
                       int32_t nLength);

  size_t m_RefCount = 0;
  UnownedPtr<Delegate> const m_pDelegate;
  std::vector<std::unique_ptr<Element>> m_arrayGlobalData;
};

#endif  // FXJS_CFX_GLOBALDATA_H_
