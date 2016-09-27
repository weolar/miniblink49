// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/test/cef_translator_test.h"
#include "testing/gtest/include/gtest/gtest.h"

// Test getting/setting primitive types.
TEST(TranslatorTest, Primitive) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  obj->GetVoid();  // Does nothing, but shouldn't crash.
  EXPECT_EQ(TEST_BOOL_VAL, obj->GetBool());
  EXPECT_EQ(TEST_INT_VAL, obj->GetInt());
  EXPECT_EQ(TEST_DOUBLE_VAL, obj->GetDouble());
  EXPECT_EQ(TEST_LONG_VAL, obj->GetLong());
  EXPECT_EQ(TEST_SIZET_VAL, obj->GetSizet());
  EXPECT_TRUE(obj->SetVoid());  // Does nothing, but shouldn't crash.
  EXPECT_TRUE(obj->SetBool(TEST_BOOL_VAL));
  EXPECT_TRUE(obj->SetInt(TEST_INT_VAL));
  EXPECT_TRUE(obj->SetDouble(TEST_DOUBLE_VAL));
  EXPECT_TRUE(obj->SetLong(TEST_LONG_VAL));
  EXPECT_TRUE(obj->SetSizet(TEST_SIZET_VAL));

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting primitive list types.
TEST(TranslatorTest, PrimitiveList) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  std::vector<int> list;
  list.push_back(TEST_INT_VAL);
  list.push_back(TEST_INT_VAL2);
  EXPECT_TRUE(obj->SetIntList(list));

  list.clear();
  EXPECT_TRUE(obj->GetIntListByRef(list));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(TEST_INT_VAL, list[0]);
  EXPECT_EQ(TEST_INT_VAL2, list[1]);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string types.
TEST(TranslatorTest, String) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  EXPECT_STREQ(TEST_STRING_VAL, obj->GetString().ToString().c_str());
  EXPECT_TRUE(obj->SetString(TEST_STRING_VAL));

  CefString str;
  obj->GetStringByRef(str);
  EXPECT_STREQ(TEST_STRING_VAL, str.ToString().c_str());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string list types.
TEST(TranslatorTest, StringList) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  std::vector<CefString> list;
  list.push_back(TEST_STRING_VAL);
  list.push_back(TEST_STRING_VAL2);
  list.push_back(TEST_STRING_VAL3);
  EXPECT_TRUE(obj->SetStringList(list));

  list.clear();
  EXPECT_TRUE(obj->GetStringListByRef(list));
  EXPECT_EQ(3U, list.size());
  EXPECT_STREQ(TEST_STRING_VAL, list[0].ToString().c_str());
  EXPECT_STREQ(TEST_STRING_VAL2, list[1].ToString().c_str());
  EXPECT_STREQ(TEST_STRING_VAL3, list[2].ToString().c_str());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string map types.
TEST(TranslatorTest, StringMap) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  CefTranslatorTest::StringMap map;
  map.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
  map.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
  map.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
  EXPECT_TRUE(obj->SetStringMap(map));

  map.clear();
  EXPECT_TRUE(obj->GetStringMapByRef(map));
  EXPECT_EQ(3U, map.size());

  CefTranslatorTest::StringMap::const_iterator it;

  it = map.find(TEST_STRING_KEY);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL);
  it = map.find(TEST_STRING_KEY2);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL2);
  it = map.find(TEST_STRING_KEY3);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL3);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting string multimap types.
TEST(TranslatorTest, StringMultimap) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  CefTranslatorTest::StringMultimap map;
  map.insert(std::make_pair(TEST_STRING_KEY, TEST_STRING_VAL));
  map.insert(std::make_pair(TEST_STRING_KEY2, TEST_STRING_VAL2));
  map.insert(std::make_pair(TEST_STRING_KEY3, TEST_STRING_VAL3));
  EXPECT_TRUE(obj->SetStringMultimap(map));

  map.clear();
  EXPECT_TRUE(obj->GetStringMultimapByRef(map));
  EXPECT_EQ(3U, map.size());

  CefTranslatorTest::StringMultimap::const_iterator it;

  it = map.find(TEST_STRING_KEY);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL);
  it = map.find(TEST_STRING_KEY2);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL2);
  it = map.find(TEST_STRING_KEY3);
  EXPECT_TRUE(it != map.end() && it->second == TEST_STRING_VAL3);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting struct types.
TEST(TranslatorTest, Struct) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();

  CefPoint point(TEST_X_VAL, TEST_Y_VAL);
  EXPECT_EQ(point, obj->GetPoint());
  EXPECT_TRUE(obj->SetPoint(point));

  CefPoint point2;
  obj->GetPointByRef(point2);
  EXPECT_EQ(point, point2);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting struct list types.
TEST(TranslatorTest, StructList) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  std::vector<CefPoint> list;
  list.push_back(CefPoint(TEST_X_VAL, TEST_Y_VAL));
  list.push_back(CefPoint(TEST_X_VAL2, TEST_Y_VAL2));
  EXPECT_TRUE(obj->SetPointList(list));

  list.clear();
  EXPECT_TRUE(obj->GetPointListByRef(list));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(CefPoint(TEST_X_VAL, TEST_Y_VAL), list[0]);
  EXPECT_EQ(CefPoint(TEST_X_VAL2, TEST_Y_VAL2), list[1]);

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
}

// Test getting/setting library-side object types.
TEST(TranslatorTest, Object) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kTestVal = 12;
  CefRefPtr<CefTranslatorTestObject> test_obj =
      CefTranslatorTestObject::Create(kTestVal);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  int retval = obj->SetObject(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  
  const int kTestVal2 = 30;
  CefRefPtr<CefTranslatorTestObject> test_obj2 = obj->GetObject(kTestVal2);
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());
  int retval2 = obj->SetObject(test_obj2);
  EXPECT_EQ(kTestVal2, retval2);
  EXPECT_EQ(kTestVal2, test_obj2->GetValue());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
  EXPECT_TRUE(test_obj2->HasOneRef());
}

// Test getting/setting inherited library-side object types.
TEST(TranslatorTest, ObjectInherit) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kTestVal = 12;
  const int kTestVal2 = 40;
  CefRefPtr<CefTranslatorTestObjectChild> test_obj =
      CefTranslatorTestObjectChild::Create(kTestVal, kTestVal2);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetObject(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildObject(test_obj));
  EXPECT_EQ(kTestVal, obj->SetChildObjectAndReturnParent(test_obj)->GetValue());
  
  const int kTestVal3 = 100;
  CefRefPtr<CefTranslatorTestObjectChildChild> test_obj2 =
      CefTranslatorTestObjectChildChild::Create(kTestVal, kTestVal2, kTestVal3);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());
  int retval2 = obj->SetObject(test_obj2);
  EXPECT_EQ(kTestVal, retval2);
  EXPECT_EQ(kTestVal, test_obj2->GetValue());
  EXPECT_EQ(kTestVal2, test_obj2->GetOtherValue());
  EXPECT_EQ(kTestVal3, test_obj2->GetOtherOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildObject(test_obj2));
  EXPECT_EQ(kTestVal,
            obj->SetChildObjectAndReturnParent(test_obj2)->GetValue());

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
  EXPECT_TRUE(test_obj2->HasOneRef());
}

// Test getting/setting library-side object list types.
TEST(TranslatorTest, ObjectList) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kVal1 = 34;
  const int kVal2 = 10;

  CefRefPtr<CefTranslatorTestObject> val1 =
      CefTranslatorTestObject::Create(kVal1);
  CefRefPtr<CefTranslatorTestObject> val2 =
      CefTranslatorTestObjectChild::Create(kVal2, 0);

  std::vector<CefRefPtr<CefTranslatorTestObject> > list;
  list.push_back(val1);
  list.push_back(val2);
  EXPECT_TRUE(obj->SetObjectList(list, kVal1, kVal2));

  list.clear();
  EXPECT_TRUE(obj->GetObjectListByRef(list, kVal1, kVal2));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(kVal1, list[0]->GetValue());
  EXPECT_EQ(kVal2, list[1]->GetValue());

  list.clear();

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(val1->HasOneRef());
  EXPECT_TRUE(val2->HasOneRef());
}

namespace {

class TranslatorTestHandler : public CefTranslatorTestHandler {
 public:
  explicit TranslatorTestHandler(const int val)
    : val_(val) {
  }

  virtual int GetValue() override {
    return val_;
  }

 private:
  const int val_;

  IMPLEMENT_REFCOUNTING(TranslatorTestHandler);
  DISALLOW_COPY_AND_ASSIGN(TranslatorTestHandler);
};

class TranslatorTestHandlerChild : public CefTranslatorTestHandlerChild {
 public:
  TranslatorTestHandlerChild(const int val,
                             const int other_val)
    : val_(val),
      other_val_(other_val) {
  }

  virtual int GetValue() override {
    return val_;
  }

  virtual int GetOtherValue() override {
    return other_val_;
  }

 private:
  const int val_;
  const int other_val_;

  IMPLEMENT_REFCOUNTING(TranslatorTestHandlerChild);
  DISALLOW_COPY_AND_ASSIGN(TranslatorTestHandlerChild);
};

}  // namespace

// Test getting/setting client-side handler types.
TEST(TranslatorTest, Handler) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kTestVal = 12;

  CefRefPtr<TranslatorTestHandler> test_obj =
      new TranslatorTestHandler(kTestVal);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal, obj->SetHandler(test_obj.get()));
  CefRefPtr<CefTranslatorTestHandler> handler =
      obj->SetHandlerAndReturn(test_obj.get());
  EXPECT_EQ(test_obj.get(), handler.get());
  EXPECT_EQ(kTestVal, handler->GetValue());
  handler = NULL;

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
}

// Test getting/setting inherited client-side handler types.
TEST(TranslatorTest, HandlerInherit) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kTestVal = 12;
  const int kTestVal2 = 86;

  CefRefPtr<TranslatorTestHandlerChild> test_obj =
      new TranslatorTestHandlerChild(kTestVal, kTestVal2);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());
  int retval = obj->SetHandler(test_obj);
  EXPECT_EQ(kTestVal, retval);
  EXPECT_EQ(kTestVal, test_obj->GetValue());
  EXPECT_EQ(kTestVal2, test_obj->GetOtherValue());

  EXPECT_EQ(kTestVal, obj->SetChildHandler(test_obj));
  CefRefPtr<CefTranslatorTestHandler> handler =
      obj->SetChildHandlerAndReturnParent(test_obj);
  EXPECT_EQ(kTestVal, handler->GetValue());
  EXPECT_EQ(test_obj.get(), handler.get());
  handler = NULL;

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(test_obj->HasOneRef());
}

// Test getting/setting client-side object list types.
TEST(TranslatorTest, HandlerList) {
  CefRefPtr<CefTranslatorTest> obj = CefTranslatorTest::Create();
  
  const int kVal1 = 34;
  const int kVal2 = 10;

  CefRefPtr<CefTranslatorTestHandler> val1 = new TranslatorTestHandler(kVal1);
  CefRefPtr<CefTranslatorTestHandler> val2 =
      new TranslatorTestHandlerChild(kVal2, 0);

  std::vector<CefRefPtr<CefTranslatorTestHandler> > list;
  list.push_back(val1);
  list.push_back(val2);
  EXPECT_TRUE(obj->SetHandlerList(list, kVal1, kVal2));

  list.clear();
  EXPECT_TRUE(obj->GetHandlerListByRef(list, val1, val2));
  EXPECT_EQ(2U, list.size());
  EXPECT_EQ(kVal1, list[0]->GetValue());
  EXPECT_EQ(val1.get(), list[0].get());
  EXPECT_EQ(kVal2, list[1]->GetValue());
  EXPECT_EQ(val2.get(), list[1].get());

  list.clear();

  // Only one reference to the object should exist.
  EXPECT_TRUE(obj->HasOneRef());
  EXPECT_TRUE(val1->HasOneRef());
  EXPECT_TRUE(val2->HasOneRef());
}
