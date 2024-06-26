// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_INDIRECT_OBJECT_HOLDER_H_
#define CORE_FPDFAPI_PARSER_CPDF_INDIRECT_OBJECT_HOLDER_H_

#include <map>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"
#include "third_party/base/ptr_util.h"

class CPDF_IndirectObjectHolder {
 public:
  using const_iterator =
      std::map<uint32_t, std::unique_ptr<CPDF_Object>>::const_iterator;

  CPDF_IndirectObjectHolder();
  virtual ~CPDF_IndirectObjectHolder();

  CPDF_Object* GetIndirectObject(uint32_t objnum) const;
  virtual CPDF_Object* GetOrParseIndirectObject(uint32_t objnum);
  void DeleteIndirectObject(uint32_t objnum);

  // Creates and adds a new object owned by the indirect object holder,
  // and returns an unowned pointer to it.  We have a special case to
  // handle objects that can intern strings from our ByteStringPool.
  template <typename T, typename... Args>
  typename std::enable_if<!CanInternStrings<T>::value, T*>::type NewIndirect(
      Args&&... args) {
    return static_cast<T*>(
        AddIndirectObject(pdfium::MakeUnique<T>(std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, T*>::type NewIndirect(
      Args&&... args) {
    return static_cast<T*>(AddIndirectObject(
        pdfium::MakeUnique<T>(m_pByteStringPool, std::forward<Args>(args)...)));
  }

  // Creates and adds a new object not owned by the indirect object holder,
  // but which can intern strings from it.
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, std::unique_ptr<T>>::type
  New(Args&&... args) {
    return pdfium::MakeUnique<T>(m_pByteStringPool,
                                 std::forward<Args>(args)...);
  }

  // Takes ownership of |pObj|, returns unowned pointer to it.
  CPDF_Object* AddIndirectObject(std::unique_ptr<CPDF_Object> pObj);

  // Always takes ownership of |pObj|, return true if higher generation number.
  bool ReplaceIndirectObjectIfHigherGeneration(
      uint32_t objnum,
      std::unique_ptr<CPDF_Object> pObj);

  // Takes ownership of |pObj|, persist it for life of the indirect object
  // holder (typically so that unowned pointers to it remain valid). No-op
  // if |pObj| is NULL.
  void AddOrphan(std::unique_ptr<CPDF_Object> pObj);

  uint32_t GetLastObjNum() const { return m_LastObjNum; }
  void SetLastObjNum(uint32_t objnum) { m_LastObjNum = objnum; }

  WeakPtr<ByteStringPool> GetByteStringPool() const {
    return m_pByteStringPool;
  }

  const_iterator begin() const { return m_IndirectObjs.begin(); }
  const_iterator end() const { return m_IndirectObjs.end(); }

 protected:
  virtual std::unique_ptr<CPDF_Object> ParseIndirectObject(uint32_t objnum);

 private:
  uint32_t m_LastObjNum;
  std::map<uint32_t, std::unique_ptr<CPDF_Object>> m_IndirectObjs;
  std::vector<std::unique_ptr<CPDF_Object>> m_OrphanObjs;
  WeakPtr<ByteStringPool> m_pByteStringPool;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_INDIRECT_OBJECT_HOLDER_H_
