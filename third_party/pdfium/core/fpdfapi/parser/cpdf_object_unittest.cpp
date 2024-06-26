// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_null.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

void TestArrayAccessors(const CPDF_Array* arr,
                        size_t index,
                        const char* str_val,
                        const char* const_str_val,
                        int int_val,
                        float float_val,
                        CPDF_Array* arr_val,
                        CPDF_Dictionary* dict_val,
                        CPDF_Stream* stream_val) {
  EXPECT_STREQ(str_val, arr->GetStringAt(index).c_str());
  EXPECT_EQ(int_val, arr->GetIntegerAt(index));
  EXPECT_EQ(float_val, arr->GetNumberAt(index));
  EXPECT_EQ(float_val, arr->GetFloatAt(index));
  EXPECT_EQ(arr_val, arr->GetArrayAt(index));
  EXPECT_EQ(dict_val, arr->GetDictAt(index));
  EXPECT_EQ(stream_val, arr->GetStreamAt(index));
}

}  // namespace

class PDFObjectsTest : public testing::Test {
 public:
  void SetUp() override {
    // Initialize different kinds of objects.
    // Boolean objects.
    CPDF_Boolean* boolean_false_obj = new CPDF_Boolean(false);
    CPDF_Boolean* boolean_true_obj = new CPDF_Boolean(true);
    // Number objects.
    CPDF_Number* number_int_obj = new CPDF_Number(1245);
    CPDF_Number* number_float_obj = new CPDF_Number(9.00345f);
    // String objects.
    CPDF_String* str_reg_obj = new CPDF_String(nullptr, L"A simple test");
    CPDF_String* str_spec_obj = new CPDF_String(nullptr, L"\t\n");
    // Name object.
    CPDF_Name* name_obj = new CPDF_Name(nullptr, "space");
    // Array object.
    m_ArrayObj = new CPDF_Array;
    m_ArrayObj->InsertNewAt<CPDF_Number>(0, 8902);
    m_ArrayObj->InsertNewAt<CPDF_Name>(1, "address");
    // Dictionary object.
    m_DictObj = new CPDF_Dictionary();
    m_DictObj->SetNewFor<CPDF_Boolean>("bool", false);
    m_DictObj->SetNewFor<CPDF_Number>("num", 0.23f);
    // Stream object.
    const char content[] = "abcdefghijklmnopqrstuvwxyz";
    size_t buf_len = FX_ArraySize(content);
    std::unique_ptr<uint8_t, FxFreeDeleter> buf(FX_Alloc(uint8_t, buf_len));
    memcpy(buf.get(), content, buf_len);
    auto pNewDict = pdfium::MakeUnique<CPDF_Dictionary>();
    m_StreamDictObj = pNewDict.get();
    m_StreamDictObj->SetNewFor<CPDF_String>("key1", L" test dict");
    m_StreamDictObj->SetNewFor<CPDF_Number>("key2", -1);
    CPDF_Stream* stream_obj =
        new CPDF_Stream(std::move(buf), buf_len, std::move(pNewDict));
    // Null Object.
    CPDF_Null* null_obj = new CPDF_Null;
    // All direct objects.
    CPDF_Object* objs[] = {boolean_false_obj, boolean_true_obj, number_int_obj,
                           number_float_obj,  str_reg_obj,      str_spec_obj,
                           name_obj,          m_ArrayObj.Get(), m_DictObj.Get(),
                           stream_obj,        null_obj};
    m_DirectObjTypes = {
        CPDF_Object::kBoolean, CPDF_Object::kBoolean, CPDF_Object::kNumber,
        CPDF_Object::kNumber,  CPDF_Object::kString,  CPDF_Object::kString,
        CPDF_Object::kName,    CPDF_Object::kArray,   CPDF_Object::kDictionary,
        CPDF_Object::kStream,  CPDF_Object::kNullobj};
    for (size_t i = 0; i < FX_ArraySize(objs); ++i)
      m_DirectObjs.emplace_back(objs[i]);

    // Indirect references to indirect objects.
    m_ObjHolder = pdfium::MakeUnique<CPDF_IndirectObjectHolder>();
    m_IndirectObjs = {m_ObjHolder->AddIndirectObject(boolean_true_obj->Clone()),
                      m_ObjHolder->AddIndirectObject(number_int_obj->Clone()),
                      m_ObjHolder->AddIndirectObject(str_spec_obj->Clone()),
                      m_ObjHolder->AddIndirectObject(name_obj->Clone()),
                      m_ObjHolder->AddIndirectObject(m_ArrayObj->Clone()),
                      m_ObjHolder->AddIndirectObject(m_DictObj->Clone()),
                      m_ObjHolder->AddIndirectObject(stream_obj->Clone())};
    for (CPDF_Object* pObj : m_IndirectObjs) {
      m_RefObjs.emplace_back(
          new CPDF_Reference(m_ObjHolder.get(), pObj->GetObjNum()));
    }
  }

  bool Equal(const CPDF_Object* obj1, const CPDF_Object* obj2) {
    if (obj1 == obj2)
      return true;
    if (!obj1 || !obj2 || obj1->GetType() != obj2->GetType())
      return false;
    switch (obj1->GetType()) {
      case CPDF_Object::kBoolean:
        return obj1->GetInteger() == obj2->GetInteger();
      case CPDF_Object::kNumber:
        return obj1->AsNumber()->IsInteger() == obj2->AsNumber()->IsInteger() &&
               obj1->GetInteger() == obj2->GetInteger();
      case CPDF_Object::kString:
      case CPDF_Object::kName:
        return obj1->GetString() == obj2->GetString();
      case CPDF_Object::kArray: {
        const CPDF_Array* array1 = obj1->AsArray();
        const CPDF_Array* array2 = obj2->AsArray();
        if (array1->size() != array2->size())
          return false;
        for (size_t i = 0; i < array1->size(); ++i) {
          if (!Equal(array1->GetObjectAt(i), array2->GetObjectAt(i)))
            return false;
        }
        return true;
      }
      case CPDF_Object::kDictionary: {
        const CPDF_Dictionary* dict1 = obj1->AsDictionary();
        const CPDF_Dictionary* dict2 = obj2->AsDictionary();
        if (dict1->size() != dict2->size())
          return false;
        CPDF_DictionaryLocker locker1(dict1);
        for (const auto& item : locker1) {
          if (!Equal(item.second.get(), dict2->GetObjectFor(item.first)))
            return false;
        }
        return true;
      }
      case CPDF_Object::kNullobj:
        return true;
      case CPDF_Object::kStream: {
        const CPDF_Stream* stream1 = obj1->AsStream();
        const CPDF_Stream* stream2 = obj2->AsStream();
        if (!stream1->GetDict() && !stream2->GetDict())
          return true;
        // Compare dictionaries.
        if (!Equal(stream1->GetDict(), stream2->GetDict()))
          return false;

        auto streamAcc1 = pdfium::MakeRetain<CPDF_StreamAcc>(stream1);
        streamAcc1->LoadAllDataRaw();
        auto streamAcc2 = pdfium::MakeRetain<CPDF_StreamAcc>(stream2);
        streamAcc2->LoadAllDataRaw();

        // Compare sizes.
        if (streamAcc1->GetSize() != streamAcc2->GetSize())
          return false;

        return memcmp(streamAcc1->GetData(), streamAcc2->GetData(),
                      streamAcc2->GetSize()) == 0;
      }
      case CPDF_Object::kReference:
        return obj1->AsReference()->GetRefObjNum() ==
               obj2->AsReference()->GetRefObjNum();
    }
    return false;
  }

 protected:
  // m_ObjHolder needs to be declared first and destructed last since it also
  // refers to some objects in m_DirectObjs.
  std::unique_ptr<CPDF_IndirectObjectHolder> m_ObjHolder;
  std::vector<std::unique_ptr<CPDF_Object>> m_DirectObjs;
  std::vector<int> m_DirectObjTypes;
  std::vector<std::unique_ptr<CPDF_Object>> m_RefObjs;
  UnownedPtr<CPDF_Dictionary> m_DictObj;
  UnownedPtr<CPDF_Dictionary> m_StreamDictObj;
  UnownedPtr<CPDF_Array> m_ArrayObj;
  std::vector<CPDF_Object*> m_IndirectObjs;
};

TEST_F(PDFObjectsTest, GetString) {
  const char* const direct_obj_results[] = {
      "false", "true", "1245", "9.00345", "A simple test", "\t\n", "space",
      "",      "",     "",     ""};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_STREQ(direct_obj_results[i], m_DirectObjs[i]->GetString().c_str());

  // Check indirect references.
  const char* const indirect_obj_results[] = {"true", "1245", "\t\n", "space",
                                              "",     "",     ""};
  for (size_t i = 0; i < m_RefObjs.size(); ++i) {
    EXPECT_STREQ(indirect_obj_results[i], m_RefObjs[i]->GetString().c_str());
  }
}

TEST_F(PDFObjectsTest, GetUnicodeText) {
  const wchar_t* const direct_obj_results[] = {
      L"",     L"",      L"", L"", L"A simple test",
      L"\t\n", L"space", L"", L"", L"abcdefghijklmnopqrstuvwxyz",
      L""};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    EXPECT_STREQ(direct_obj_results[i],
                 m_DirectObjs[i]->GetUnicodeText().c_str());
  }

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_STREQ(L"", it->GetUnicodeText().c_str());
}

TEST_F(PDFObjectsTest, GetNumber) {
  const float direct_obj_results[] = {0, 0, 1245, 9.00345f, 0, 0,
                                      0, 0, 0,    0,        0};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetNumber());

  // Check indirect references.
  const float indirect_obj_results[] = {0, 1245, 0, 0, 0, 0, 0};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(indirect_obj_results[i], m_RefObjs[i]->GetNumber());
}

TEST_F(PDFObjectsTest, GetInteger) {
  const int direct_obj_results[] = {0, 1, 1245, 9, 0, 0, 0, 0, 0, 0, 0};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetInteger());

  // Check indirect references.
  const int indirect_obj_results[] = {1, 1245, 0, 0, 0, 0, 0};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(indirect_obj_results[i], m_RefObjs[i]->GetInteger());
}

TEST_F(PDFObjectsTest, GetDict) {
  const CPDF_Dictionary* const direct_obj_results[] = {
      nullptr, nullptr, nullptr, nullptr,         nullptr,
      nullptr, nullptr, nullptr, m_DictObj.Get(), m_StreamDictObj.Get(),
      nullptr};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->GetDict());

  // Check indirect references.
  const CPDF_Dictionary* const indirect_obj_results[] = {nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         nullptr,
                                                         m_DictObj.Get(),
                                                         m_StreamDictObj.Get()};
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_TRUE(Equal(indirect_obj_results[i], m_RefObjs[i]->GetDict()));
}

TEST_F(PDFObjectsTest, GetArray) {
  const CPDF_Array* const direct_obj_results[] = {
      nullptr, nullptr,          nullptr, nullptr, nullptr, nullptr,
      nullptr, m_ArrayObj.Get(), nullptr, nullptr, nullptr};
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(direct_obj_results[i], m_DirectObjs[i]->AsArray());

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_EQ(nullptr, it->AsArray());
}

TEST_F(PDFObjectsTest, Clone) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    std::unique_ptr<CPDF_Object> obj = m_DirectObjs[i]->Clone();
    EXPECT_TRUE(Equal(m_DirectObjs[i].get(), obj.get()));
  }

  // Check indirect references.
  for (const auto& it : m_RefObjs) {
    std::unique_ptr<CPDF_Object> obj = it->Clone();
    EXPECT_TRUE(Equal(it.get(), obj.get()));
  }
}

TEST_F(PDFObjectsTest, GetType) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(m_DirectObjTypes[i], m_DirectObjs[i]->GetType());

  // Check indirect references.
  for (const auto& it : m_RefObjs)
    EXPECT_EQ(CPDF_Object::kReference, it->GetType());
}

TEST_F(PDFObjectsTest, GetDirect) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i)
    EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->GetDirect());

  // Check indirect references.
  for (size_t i = 0; i < m_RefObjs.size(); ++i)
    EXPECT_EQ(m_IndirectObjs[i], m_RefObjs[i]->GetDirect());
}

TEST_F(PDFObjectsTest, SetString) {
  // Check for direct objects.
  const char* const set_values[] = {"true",    "fake", "3.125f", "097",
                                    "changed", "",     "NewName"};
  const char* const expected[] = {"true",    "false", "3.125",  "97",
                                  "changed", "",      "NewName"};
  for (size_t i = 0; i < FX_ArraySize(set_values); ++i) {
    m_DirectObjs[i]->SetString(set_values[i]);
    EXPECT_STREQ(expected[i], m_DirectObjs[i]->GetString().c_str());
  }
}

TEST_F(PDFObjectsTest, IsTypeAndAsType) {
  // Check for direct objects.
  for (size_t i = 0; i < m_DirectObjs.size(); ++i) {
    if (m_DirectObjTypes[i] == CPDF_Object::kArray) {
      EXPECT_TRUE(m_DirectObjs[i]->IsArray());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsArray());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsArray());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsArray());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kBoolean) {
      EXPECT_TRUE(m_DirectObjs[i]->IsBoolean());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsBoolean());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsBoolean());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsBoolean());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kName) {
      EXPECT_TRUE(m_DirectObjs[i]->IsName());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsName());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsName());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsName());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kNumber) {
      EXPECT_TRUE(m_DirectObjs[i]->IsNumber());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsNumber());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsNumber());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsNumber());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kString) {
      EXPECT_TRUE(m_DirectObjs[i]->IsString());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsString());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsString());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsString());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kDictionary) {
      EXPECT_TRUE(m_DirectObjs[i]->IsDictionary());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsDictionary());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsDictionary());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsDictionary());
    }

    if (m_DirectObjTypes[i] == CPDF_Object::kStream) {
      EXPECT_TRUE(m_DirectObjs[i]->IsStream());
      EXPECT_EQ(m_DirectObjs[i].get(), m_DirectObjs[i]->AsStream());
    } else {
      EXPECT_FALSE(m_DirectObjs[i]->IsStream());
      EXPECT_EQ(nullptr, m_DirectObjs[i]->AsStream());
    }

    EXPECT_FALSE(m_DirectObjs[i]->IsReference());
    EXPECT_EQ(nullptr, m_DirectObjs[i]->AsReference());
  }
  // Check indirect references.
  for (size_t i = 0; i < m_RefObjs.size(); ++i) {
    EXPECT_TRUE(m_RefObjs[i]->IsReference());
    EXPECT_EQ(m_RefObjs[i].get(), m_RefObjs[i]->AsReference());
  }
}

TEST_F(PDFObjectsTest, MakeReferenceGeneric) {
  auto original_obj = pdfium::MakeUnique<CPDF_Null>();
  original_obj->SetObjNum(42);
  ASSERT_FALSE(original_obj->IsInline());

  auto ref_obj = original_obj->MakeReference(m_ObjHolder.get());

  ASSERT_TRUE(ref_obj->IsReference());
  EXPECT_EQ(original_obj->GetObjNum(),
            ToReference(ref_obj.get())->GetRefObjNum());
}

TEST(PDFArrayTest, GetMatrix) {
  float elems[][6] = {{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
                      {1, 2, 3, 4, 5, 6},
                      {2.3f, 4.05f, 3, -2, -3, 0.0f},
                      {0.05f, 0.1f, 0.56f, 0.67f, 1.34f, 99.9f}};
  for (size_t i = 0; i < FX_ArraySize(elems); ++i) {
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    CFX_Matrix matrix(elems[i][0], elems[i][1], elems[i][2], elems[i][3],
                      elems[i][4], elems[i][5]);
    for (size_t j = 0; j < 6; ++j)
      arr->AddNew<CPDF_Number>(elems[i][j]);
    CFX_Matrix arr_matrix = arr->GetMatrix();
    EXPECT_EQ(matrix.a, arr_matrix.a);
    EXPECT_EQ(matrix.b, arr_matrix.b);
    EXPECT_EQ(matrix.c, arr_matrix.c);
    EXPECT_EQ(matrix.d, arr_matrix.d);
    EXPECT_EQ(matrix.e, arr_matrix.e);
    EXPECT_EQ(matrix.f, arr_matrix.f);
  }
}

TEST(PDFArrayTest, GetRect) {
  float elems[][4] = {{0.0f, 0.0f, 0.0f, 0.0f},
                      {1, 2, 5, 6},
                      {2.3f, 4.05f, -3, 0.0f},
                      {0.05f, 0.1f, 1.34f, 99.9f}};
  for (size_t i = 0; i < FX_ArraySize(elems); ++i) {
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    CFX_FloatRect rect(elems[i]);
    for (size_t j = 0; j < 4; ++j)
      arr->AddNew<CPDF_Number>(elems[i][j]);
    CFX_FloatRect arr_rect = arr->GetRect();
    EXPECT_EQ(rect.left, arr_rect.left);
    EXPECT_EQ(rect.right, arr_rect.right);
    EXPECT_EQ(rect.bottom, arr_rect.bottom);
    EXPECT_EQ(rect.top, arr_rect.top);
  }
}

TEST(PDFArrayTest, GetTypeAt) {
  {
    // Boolean array.
    const bool vals[] = {true, false, false, true, true};
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < FX_ArraySize(vals); ++i)
      arr->InsertNewAt<CPDF_Boolean>(i, vals[i]);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(arr.get(), i,                // Array and index.
                         vals[i] ? "true" : "false",  // String value.
                         nullptr,                     // Const string value.
                         vals[i] ? 1 : 0,             // Integer value.
                         0,                           // Float value.
                         nullptr,                     // Array value.
                         nullptr,                     // Dictionary value.
                         nullptr);                    // Stream value.
    }
  }
  {
    // Integer array.
    const int vals[] = {10, 0, -345, 2089345456, -1000000000, 567, 93658767};
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < FX_ArraySize(vals); ++i)
      arr->InsertNewAt<CPDF_Number>(i, vals[i]);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      char buf[33];
      TestArrayAccessors(arr.get(), i,                  // Array and index.
                         FXSYS_itoa(vals[i], buf, 10),  // String value.
                         nullptr,                       // Const string value.
                         vals[i],                       // Integer value.
                         vals[i],                       // Float value.
                         nullptr,                       // Array value.
                         nullptr,                       // Dictionary value.
                         nullptr);                      // Stream value.
    }
  }
  {
    // Float array.
    const float vals[] = {0.0f,    0,     10,    10.0f,   0.0345f,
                          897.34f, -2.5f, -1.0f, -345.0f, -0.0f};
    const char* const expected_str[] = {
        "0", "0", "10", "10", "0.0345", "897.34", "-2.5", "-1", "-345", "0"};
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < FX_ArraySize(vals); ++i)
      arr->InsertNewAt<CPDF_Number>(i, vals[i]);
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(arr.get(), i,     // Array and index.
                         expected_str[i],  // String value.
                         nullptr,          // Const string value.
                         vals[i],          // Integer value.
                         vals[i],          // Float value.
                         nullptr,          // Array value.
                         nullptr,          // Dictionary value.
                         nullptr);         // Stream value.
    }
  }
  {
    // String and name array
    const char* const vals[] = {"this", "adsde$%^", "\r\t",           "\"012",
                                ".",    "EYREW",    "It is a joke :)"};
    auto string_array = pdfium::MakeUnique<CPDF_Array>();
    auto name_array = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      string_array->InsertNewAt<CPDF_String>(i, vals[i], false);
      name_array->InsertNewAt<CPDF_Name>(i, vals[i]);
    }
    for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
      TestArrayAccessors(string_array.get(), i,  // Array and index.
                         vals[i],                // String value.
                         vals[i],                // Const string value.
                         0,                      // Integer value.
                         0,                      // Float value.
                         nullptr,                // Array value.
                         nullptr,                // Dictionary value.
                         nullptr);               // Stream value.
      TestArrayAccessors(name_array.get(), i,    // Array and index.
                         vals[i],                // String value.
                         vals[i],                // Const string value.
                         0,                      // Integer value.
                         0,                      // Float value.
                         nullptr,                // Array value.
                         nullptr,                // Dictionary value.
                         nullptr);               // Stream value.
    }
  }
  {
    // Null element array.
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < 3; ++i)
      arr->InsertNewAt<CPDF_Null>(i);
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         nullptr,       // Array value.
                         nullptr,       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Array of array.
    CPDF_Array* vals[3];
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = arr->AddNew<CPDF_Array>();
      for (size_t j = 0; j < 3; ++j) {
        int value = j + 100;
        vals[i]->InsertNewAt<CPDF_Number>(i, value);
      }
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         vals[i],       // Array value.
                         nullptr,       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Dictionary array.
    CPDF_Dictionary* vals[3];
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = arr->AddNew<CPDF_Dictionary>();
      for (size_t j = 0; j < 3; ++j) {
        std::string key("key");
        char buf[33];
        key.append(FXSYS_itoa(j, buf, 10));
        int value = j + 200;
        vals[i]->SetNewFor<CPDF_Number>(key.c_str(), value);
      }
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,  // Array and index.
                         "",            // String value.
                         nullptr,       // Const string value.
                         0,             // Integer value.
                         0,             // Float value.
                         nullptr,       // Array value.
                         vals[i],       // Dictionary value.
                         nullptr);      // Stream value.
    }
  }
  {
    // Stream array.
    CPDF_Dictionary* vals[3];
    CPDF_Stream* stream_vals[3];
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    for (size_t i = 0; i < 3; ++i) {
      vals[i] = new CPDF_Dictionary();
      for (size_t j = 0; j < 3; ++j) {
        std::string key("key");
        char buf[33];
        key.append(FXSYS_itoa(j, buf, 10));
        int value = j + 200;
        vals[i]->SetNewFor<CPDF_Number>(key.c_str(), value);
      }
      uint8_t content[] = "content: this is a stream";
      size_t data_size = FX_ArraySize(content);
      std::unique_ptr<uint8_t, FxFreeDeleter> data(
          FX_Alloc(uint8_t, data_size));
      memcpy(data.get(), content, data_size);
      stream_vals[i] = arr->AddNew<CPDF_Stream>(std::move(data), data_size,
                                                pdfium::WrapUnique(vals[i]));
    }
    for (size_t i = 0; i < 3; ++i) {
      TestArrayAccessors(arr.get(), i,     // Array and index.
                         "",               // String value.
                         nullptr,          // Const string value.
                         0,                // Integer value.
                         0,                // Float value.
                         nullptr,          // Array value.
                         vals[i],          // Dictionary value.
                         stream_vals[i]);  // Stream value.
    }
  }
  {
    // Mixed array.
    auto arr = pdfium::MakeUnique<CPDF_Array>();
    arr->InsertNewAt<CPDF_Boolean>(0, true);
    arr->InsertNewAt<CPDF_Boolean>(1, false);
    arr->InsertNewAt<CPDF_Number>(2, 0);
    arr->InsertNewAt<CPDF_Number>(3, -1234);
    arr->InsertNewAt<CPDF_Number>(4, 2345.0f);
    arr->InsertNewAt<CPDF_Number>(5, 0.05f);
    arr->InsertNewAt<CPDF_String>(6, "", false);
    arr->InsertNewAt<CPDF_String>(7, "It is a test!", false);
    arr->InsertNewAt<CPDF_Name>(8, "NAME");
    arr->InsertNewAt<CPDF_Name>(9, "test");
    arr->InsertNewAt<CPDF_Null>(10);

    CPDF_Array* arr_val = arr->InsertNewAt<CPDF_Array>(11);
    arr_val->AddNew<CPDF_Number>(1);
    arr_val->AddNew<CPDF_Number>(2);

    CPDF_Dictionary* dict_val = arr->InsertNewAt<CPDF_Dictionary>(12);
    dict_val->SetNewFor<CPDF_String>("key1", "Linda", false);
    dict_val->SetNewFor<CPDF_String>("key2", "Zoe", false);

    CPDF_Dictionary* stream_dict = new CPDF_Dictionary();
    stream_dict->SetNewFor<CPDF_String>("key1", "John", false);
    stream_dict->SetNewFor<CPDF_String>("key2", "King", false);
    uint8_t data[] = "A stream for test";
    // The data buffer will be owned by stream object, so it needs to be
    // dynamically allocated.
    size_t buf_size = sizeof(data);
    std::unique_ptr<uint8_t, FxFreeDeleter> buf(FX_Alloc(uint8_t, buf_size));
    memcpy(buf.get(), data, buf_size);
    CPDF_Stream* stream_val = arr->InsertNewAt<CPDF_Stream>(
        13, std::move(buf), buf_size, pdfium::WrapUnique(stream_dict));
    const char* const expected_str[] = {
        "true",          "false", "0",    "-1234", "2345", "0.05", "",
        "It is a test!", "NAME",  "test", "",      "",     "",     ""};
    const int expected_int[] = {1, 0, 0, -1234, 2345, 0, 0,
                                0, 0, 0, 0,     0,    0, 0};
    const float expected_float[] = {0, 0, 0, -1234, 2345, 0.05f, 0,
                                    0, 0, 0, 0,     0,    0,     0};
    for (size_t i = 0; i < arr->size(); ++i) {
      EXPECT_STREQ(expected_str[i], arr->GetStringAt(i).c_str());
      EXPECT_EQ(expected_int[i], arr->GetIntegerAt(i));
      EXPECT_EQ(expected_float[i], arr->GetNumberAt(i));
      EXPECT_EQ(expected_float[i], arr->GetFloatAt(i));
      if (i == 11)
        EXPECT_EQ(arr_val, arr->GetArrayAt(i));
      else
        EXPECT_EQ(nullptr, arr->GetArrayAt(i));
      if (i == 13) {
        EXPECT_EQ(stream_dict, arr->GetDictAt(i));
        EXPECT_EQ(stream_val, arr->GetStreamAt(i));
      } else {
        EXPECT_EQ(nullptr, arr->GetStreamAt(i));
        if (i == 12)
          EXPECT_EQ(dict_val, arr->GetDictAt(i));
        else
          EXPECT_EQ(nullptr, arr->GetDictAt(i));
      }
    }
  }
}

TEST(PDFArrayTest, AddNumber) {
  float vals[] = {1.0f,         -1.0f, 0,    0.456734f,
                  12345.54321f, 0.5f,  1000, 0.000045f};
  auto arr = pdfium::MakeUnique<CPDF_Array>();
  for (size_t i = 0; i < FX_ArraySize(vals); ++i)
    arr->AddNew<CPDF_Number>(vals[i]);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::kNumber, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(vals[i], arr->GetObjectAt(i)->GetNumber());
  }
}

TEST(PDFArrayTest, AddInteger) {
  int vals[] = {0, 1, 934435456, 876, 10000, -1, -24354656, -100};
  auto arr = pdfium::MakeUnique<CPDF_Array>();
  for (size_t i = 0; i < FX_ArraySize(vals); ++i)
    arr->AddNew<CPDF_Number>(vals[i]);
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::kNumber, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(vals[i], arr->GetObjectAt(i)->GetNumber());
  }
}

TEST(PDFArrayTest, AddStringAndName) {
  static constexpr const char* vals[] = {
      "",        "a", "ehjhRIOYTTFdfcdnv",  "122323",
      "$#%^&**", " ", "This is a test.\r\n"};
  auto string_array = pdfium::MakeUnique<CPDF_Array>();
  auto name_array = pdfium::MakeUnique<CPDF_Array>();
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    string_array->AddNew<CPDF_String>(vals[i], false);
    name_array->AddNew<CPDF_Name>(vals[i]);
  }
  for (size_t i = 0; i < FX_ArraySize(vals); ++i) {
    EXPECT_EQ(CPDF_Object::kString, string_array->GetObjectAt(i)->GetType());
    EXPECT_STREQ(vals[i], string_array->GetObjectAt(i)->GetString().c_str());
    EXPECT_EQ(CPDF_Object::kName, name_array->GetObjectAt(i)->GetType());
    EXPECT_STREQ(vals[i], name_array->GetObjectAt(i)->GetString().c_str());
  }
}

TEST(PDFArrayTest, AddReferenceAndGetObjectAt) {
  auto holder = pdfium::MakeUnique<CPDF_IndirectObjectHolder>();
  CPDF_Boolean* boolean_obj = new CPDF_Boolean(true);
  CPDF_Number* int_obj = new CPDF_Number(-1234);
  CPDF_Number* float_obj = new CPDF_Number(2345.089f);
  CPDF_String* str_obj =
      new CPDF_String(nullptr, "Adsfdsf 343434 %&&*\n", false);
  CPDF_Name* name_obj = new CPDF_Name(nullptr, "Title:");
  CPDF_Null* null_obj = new CPDF_Null();
  CPDF_Object* indirect_objs[] = {boolean_obj, int_obj,  float_obj,
                                  str_obj,     name_obj, null_obj};
  unsigned int obj_nums[] = {2, 4, 7, 2345, 799887, 1};
  auto arr = pdfium::MakeUnique<CPDF_Array>();
  auto arr1 = pdfium::MakeUnique<CPDF_Array>();
  // Create two arrays of references by different AddReference() APIs.
  for (size_t i = 0; i < FX_ArraySize(indirect_objs); ++i) {
    holder->ReplaceIndirectObjectIfHigherGeneration(
        obj_nums[i], pdfium::WrapUnique<CPDF_Object>(indirect_objs[i]));
    arr->AddNew<CPDF_Reference>(holder.get(), obj_nums[i]);
    arr1->AddNew<CPDF_Reference>(holder.get(), indirect_objs[i]->GetObjNum());
  }
  // Check indirect objects.
  for (size_t i = 0; i < FX_ArraySize(obj_nums); ++i)
    EXPECT_EQ(indirect_objs[i], holder->GetOrParseIndirectObject(obj_nums[i]));
  // Check arrays.
  EXPECT_EQ(arr->size(), arr1->size());
  for (size_t i = 0; i < arr->size(); ++i) {
    EXPECT_EQ(CPDF_Object::kReference, arr->GetObjectAt(i)->GetType());
    EXPECT_EQ(indirect_objs[i], arr->GetObjectAt(i)->GetDirect());
    EXPECT_EQ(indirect_objs[i], arr->GetDirectObjectAt(i));
    EXPECT_EQ(CPDF_Object::kReference, arr1->GetObjectAt(i)->GetType());
    EXPECT_EQ(indirect_objs[i], arr1->GetObjectAt(i)->GetDirect());
    EXPECT_EQ(indirect_objs[i], arr1->GetDirectObjectAt(i));
  }
}

TEST(PDFArrayTest, CloneDirectObject) {
  CPDF_IndirectObjectHolder objects_holder;
  auto array = pdfium::MakeUnique<CPDF_Array>();
  array->AddNew<CPDF_Reference>(&objects_holder, 1234);
  ASSERT_EQ(1U, array->size());
  CPDF_Object* obj = array->GetObjectAt(0);
  ASSERT_TRUE(obj);
  EXPECT_TRUE(obj->IsReference());

  std::unique_ptr<CPDF_Object> cloned_array_object = array->CloneDirectObject();
  ASSERT_TRUE(cloned_array_object);
  ASSERT_TRUE(cloned_array_object->IsArray());

  std::unique_ptr<CPDF_Array> cloned_array =
      ToArray(std::move(cloned_array_object));
  ASSERT_EQ(0U, cloned_array->size());
  CPDF_Object* cloned_obj = cloned_array->GetObjectAt(0);
  EXPECT_FALSE(cloned_obj);
}

TEST(PDFArrayTest, ConvertIndirect) {
  CPDF_IndirectObjectHolder objects_holder;
  auto array = pdfium::MakeUnique<CPDF_Array>();
  CPDF_Object* pObj = array->AddNew<CPDF_Number>(42);
  array->ConvertToIndirectObjectAt(0, &objects_holder);
  CPDF_Object* pRef = array->GetObjectAt(0);
  CPDF_Object* pNum = array->GetDirectObjectAt(0);
  EXPECT_TRUE(pRef->IsReference());
  EXPECT_TRUE(pNum->IsNumber());
  EXPECT_NE(pObj, pRef);
  EXPECT_EQ(pObj, pNum);
  EXPECT_EQ(42, array->GetIntegerAt(0));
}

TEST(PDFStreamTest, SetData) {
  std::vector<uint8_t> data(100);
  auto stream = pdfium::MakeUnique<CPDF_Stream>();
  stream->InitStream(data, pdfium::MakeUnique<CPDF_Dictionary>());
  EXPECT_EQ(static_cast<int>(data.size()),
            stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));

  stream->GetDict()->SetNewFor<CPDF_String>(pdfium::stream::kFilter,
                                            L"SomeFilter");
  stream->GetDict()->SetNewFor<CPDF_String>(pdfium::stream::kDecodeParms,
                                            L"SomeParams");

  std::vector<uint8_t> new_data(data.size() * 2);
  stream->SetData(new_data);

  // The "Length" field should be updated for new data size.
  EXPECT_EQ(static_cast<int>(new_data.size()),
            stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));

  // The "Filter" and "DecodeParms" fields should not be changed.
  EXPECT_EQ(stream->GetDict()->GetUnicodeTextFor(pdfium::stream::kFilter),
            L"SomeFilter");
  EXPECT_EQ(stream->GetDict()->GetUnicodeTextFor(pdfium::stream::kDecodeParms),
            L"SomeParams");
}

TEST(PDFStreamTest, SetDataAndRemoveFilter) {
  std::vector<uint8_t> data(100);
  auto stream = pdfium::MakeUnique<CPDF_Stream>();
  stream->InitStream(data, pdfium::MakeUnique<CPDF_Dictionary>());
  EXPECT_EQ(static_cast<int>(data.size()),
            stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));

  stream->GetDict()->SetNewFor<CPDF_String>(pdfium::stream::kFilter,
                                            L"SomeFilter");
  stream->GetDict()->SetNewFor<CPDF_String>(pdfium::stream::kDecodeParms,
                                            L"SomeParams");

  std::vector<uint8_t> new_data(data.size() * 2);
  stream->SetDataAndRemoveFilter(new_data);
  // The "Length" field should be updated for new data size.
  EXPECT_EQ(static_cast<int>(new_data.size()),
            stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));

  // The "Filter" and "DecodeParms" should be removed.
  EXPECT_FALSE(stream->GetDict()->KeyExist(pdfium::stream::kFilter));
  EXPECT_FALSE(stream->GetDict()->KeyExist(pdfium::stream::kDecodeParms));
}

TEST(PDFStreamTest, LengthInDictionaryOnCreate) {
  static constexpr uint32_t kBufSize = 100;
  // The length field should be created on stream create.
  {
    std::unique_ptr<uint8_t, FxFreeDeleter> data;
    data.reset(FX_Alloc(uint8_t, kBufSize));
    auto stream = pdfium::MakeUnique<CPDF_Stream>(
        std::move(data), kBufSize, pdfium::MakeUnique<CPDF_Dictionary>());
    EXPECT_EQ(static_cast<int>(kBufSize),
              stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));
  }
  // The length field should be corrected on stream create.
  {
    std::unique_ptr<uint8_t, FxFreeDeleter> data;
    data.reset(FX_Alloc(uint8_t, kBufSize));
    auto dict = pdfium::MakeUnique<CPDF_Dictionary>();
    dict->SetNewFor<CPDF_Number>(pdfium::stream::kLength, 30000);
    auto stream = pdfium::MakeUnique<CPDF_Stream>(std::move(data), kBufSize,
                                                  std::move(dict));
    EXPECT_EQ(static_cast<int>(kBufSize),
              stream->GetDict()->GetIntegerFor(pdfium::stream::kLength));
  }
}

TEST(PDFDictionaryTest, CloneDirectObject) {
  CPDF_IndirectObjectHolder objects_holder;
  auto dict = pdfium::MakeUnique<CPDF_Dictionary>();
  dict->SetNewFor<CPDF_Reference>("foo", &objects_holder, 1234);
  ASSERT_EQ(1U, dict->size());
  CPDF_Object* obj = dict->GetObjectFor("foo");
  ASSERT_TRUE(obj);
  EXPECT_TRUE(obj->IsReference());

  std::unique_ptr<CPDF_Object> cloned_dict_object = dict->CloneDirectObject();
  ASSERT_TRUE(cloned_dict_object);
  ASSERT_TRUE(cloned_dict_object->IsDictionary());

  std::unique_ptr<CPDF_Dictionary> cloned_dict =
      ToDictionary(std::move(cloned_dict_object));
  ASSERT_EQ(0U, cloned_dict->size());
  CPDF_Object* cloned_obj = cloned_dict->GetObjectFor("foo");
  EXPECT_FALSE(cloned_obj);
}

TEST(PDFObjectTest, CloneCheckLoop) {
  {
    // Create a dictionary/array pair with a reference loop. It takes
    // some work to do this nowadays, in particular we need the
    // anti-pattern pdfium::WrapUnique(arr.get()).
    auto arr_obj = pdfium::MakeUnique<CPDF_Array>();
    CPDF_Dictionary* dict_obj = arr_obj->InsertNewAt<CPDF_Dictionary>(0);
    dict_obj->SetFor("arr", pdfium::WrapUnique(arr_obj.get()));
    // Clone this object to see whether stack overflow will be triggered.
    std::unique_ptr<CPDF_Array> cloned_array = ToArray(arr_obj->Clone());
    // Cloned object should be the same as the original.
    ASSERT_TRUE(cloned_array);
    EXPECT_EQ(1u, cloned_array->size());
    CPDF_Object* cloned_dict = cloned_array->GetObjectAt(0);
    ASSERT_TRUE(cloned_dict);
    ASSERT_TRUE(cloned_dict->IsDictionary());
    // Recursively referenced object is not cloned.
    EXPECT_EQ(nullptr, cloned_dict->AsDictionary()->GetObjectFor("arr"));
  }
  {
    // Create a dictionary/stream pair with a reference loop. It takes
    // some work to do this nowadays, in particular we need the
    // anti-pattern pdfium::WrapUnique(dict.get()).
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    CPDF_Stream* stream_obj = dict_obj->SetNewFor<CPDF_Stream>(
        "stream", nullptr, 0, pdfium::WrapUnique(dict_obj.get()));
    // Clone this object to see whether stack overflow will be triggered.
    std::unique_ptr<CPDF_Stream> cloned_stream = ToStream(stream_obj->Clone());
    // Cloned object should be the same as the original.
    ASSERT_TRUE(cloned_stream);
    CPDF_Object* cloned_dict = cloned_stream->GetDict();
    ASSERT_TRUE(cloned_dict);
    ASSERT_TRUE(cloned_dict->IsDictionary());
    // Recursively referenced object is not cloned.
    EXPECT_EQ(nullptr, cloned_dict->AsDictionary()->GetObjectFor("stream"));
  }
  {
    CPDF_IndirectObjectHolder objects_holder;
    // Create an object with a reference loop.
    CPDF_Dictionary* dict_obj = objects_holder.NewIndirect<CPDF_Dictionary>();
    std::unique_ptr<CPDF_Array> arr_obj = pdfium::MakeUnique<CPDF_Array>();
    arr_obj->InsertNewAt<CPDF_Reference>(0, &objects_holder,
                                         dict_obj->GetObjNum());
    CPDF_Object* elem0 = arr_obj->GetObjectAt(0);
    dict_obj->SetFor("arr", std::move(arr_obj));
    EXPECT_EQ(1u, dict_obj->GetObjNum());
    ASSERT_TRUE(elem0);
    ASSERT_TRUE(elem0->IsReference());
    EXPECT_EQ(1u, elem0->AsReference()->GetRefObjNum());
    EXPECT_EQ(dict_obj, elem0->AsReference()->GetDirect());

    // Clone this object to see whether stack overflow will be triggered.
    std::unique_ptr<CPDF_Dictionary> cloned_dict =
        ToDictionary(dict_obj->CloneDirectObject());
    // Cloned object should be the same as the original.
    ASSERT_TRUE(cloned_dict);
    CPDF_Object* cloned_arr = cloned_dict->GetObjectFor("arr");
    ASSERT_TRUE(cloned_arr);
    ASSERT_TRUE(cloned_arr->IsArray());
    EXPECT_EQ(0U, cloned_arr->AsArray()->size());
    // Recursively referenced object is not cloned.
    EXPECT_EQ(nullptr, cloned_arr->AsArray()->GetObjectAt(0));
  }
}

TEST(PDFDictionaryTest, ConvertIndirect) {
  CPDF_IndirectObjectHolder objects_holder;
  auto dict = pdfium::MakeUnique<CPDF_Dictionary>();
  CPDF_Object* pObj = dict->SetNewFor<CPDF_Number>("clams", 42);
  dict->ConvertToIndirectObjectFor("clams", &objects_holder);
  CPDF_Object* pRef = dict->GetObjectFor("clams");
  CPDF_Object* pNum = dict->GetDirectObjectFor("clams");
  EXPECT_TRUE(pRef->IsReference());
  EXPECT_TRUE(pNum->IsNumber());
  EXPECT_NE(pObj, pRef);
  EXPECT_EQ(pObj, pNum);
  EXPECT_EQ(42, dict->GetIntegerFor("clams"));
}

TEST(PDFDictionaryTest, ExtractObjectOnRemove) {
  auto dict = pdfium::MakeUnique<CPDF_Dictionary>();
  CPDF_Object* pObj = dict->SetNewFor<CPDF_Number>("child", 42);
  auto extracted_object = dict->RemoveFor("child");
  EXPECT_EQ(pObj, extracted_object.get());

  extracted_object = dict->RemoveFor("non_exists_object");
  EXPECT_FALSE(extracted_object);
}

TEST(PDFRefernceTest, MakeReferenceToReference) {
  auto obj_holder = pdfium::MakeUnique<CPDF_IndirectObjectHolder>();
  auto original_ref = pdfium::MakeUnique<CPDF_Reference>(obj_holder.get(), 42);
  original_ref->SetObjNum(1952);
  ASSERT_FALSE(original_ref->IsInline());

  auto ref_obj = original_ref->MakeReference(obj_holder.get());

  ASSERT_TRUE(ref_obj->IsReference());
  // We do not allow reference to reference.
  // New reference should have same RefObjNum.
  EXPECT_EQ(original_ref->GetRefObjNum(),
            ToReference(ref_obj.get())->GetRefObjNum());
}
