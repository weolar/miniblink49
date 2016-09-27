// Copyright (c) 2010 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <map>
#include <vector>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_string16.h"
#include "include/internal/cef_string.h"
#include "include/internal/cef_string_list.h"
#include "include/internal/cef_string_map.h"
#include "include/internal/cef_string_multimap.h"
#include "testing/gtest/include/gtest/gtest.h"

// Test UTF8 strings.
TEST(StringTest, UTF8) {
  CefStringUTF8 str1("Test String");
  EXPECT_EQ(str1.length(), (size_t)11);
  EXPECT_FALSE(str1.empty());
  EXPECT_TRUE(str1.IsOwner());

  // Test equality.
  CefStringUTF8 str2("Test String");
  EXPECT_EQ(str1, str2);
  EXPECT_LE(str1, str2);
  EXPECT_GE(str1, str2);

  str2 = "Test Test";
  EXPECT_LT(str1, str2);
  EXPECT_GT(str2, str1);

  // When strings are the same but of unequal length, the longer string is
  // greater.
  str2 = "Test";
  EXPECT_LT(str2, str1);
  EXPECT_GT(str1, str2);

  // Test conversions.
  str2 = str1.ToString();
  EXPECT_EQ(str1, str2);
  str2 = str1.ToWString();
  EXPECT_EQ(str1, str2);

  // Test userfree assignment.
  cef_string_userfree_utf8_t uf = str2.DetachToUserFree();
  EXPECT_TRUE(uf != NULL);
  EXPECT_TRUE(str2.empty());
  str2.AttachToUserFree(uf);
  EXPECT_FALSE(str2.empty());
  EXPECT_EQ(str1, str2);
}

// Test UTF16 strings.
TEST(StringTest, UTF16) {
  CefStringUTF16 str1("Test String");
  EXPECT_EQ(str1.length(), (size_t)11);
  EXPECT_FALSE(str1.empty());
  EXPECT_TRUE(str1.IsOwner());

  // Test equality.
  CefStringUTF16 str2("Test String");
  EXPECT_EQ(str1, str2);
  EXPECT_LE(str1, str2);
  EXPECT_GE(str1, str2);

  str2 = "Test Test";
  EXPECT_LT(str1, str2);
  EXPECT_GT(str2, str1);

  // When strings are the same but of unequal length, the longer string is
  // greater.
  str2 = "Test";
  EXPECT_LT(str2, str1);
  EXPECT_GT(str1, str2);

  // Test conversions.
  str2 = str1.ToString();
  EXPECT_EQ(str1, str2);
  str2 = str1.ToWString();
  EXPECT_EQ(str1, str2);

  // Test userfree assignment.
  cef_string_userfree_utf16_t uf = str2.DetachToUserFree();
  EXPECT_TRUE(uf != NULL);
  EXPECT_TRUE(str2.empty());
  str2.AttachToUserFree(uf);
  EXPECT_FALSE(str2.empty());
  EXPECT_EQ(str1, str2);
}

// Test wide strings.
TEST(StringTest, Wide) {
  CefStringWide str1("Test String");
  EXPECT_EQ(str1.length(), (size_t)11);
  EXPECT_FALSE(str1.empty());
  EXPECT_TRUE(str1.IsOwner());

  // Test equality.
  CefStringWide str2("Test String");
  EXPECT_EQ(str1, str2);
  EXPECT_LE(str1, str2);
  EXPECT_GE(str1, str2);

  str2 = "Test Test";
  EXPECT_LT(str1, str2);
  EXPECT_GT(str2, str1);

  // When strings are the same but of unequal length, the longer string is
  // greater.
  str2 = "Test";
  EXPECT_LT(str2, str1);
  EXPECT_GT(str1, str2);

  // Test conversions.
  str2 = str1.ToString();
  EXPECT_EQ(str1, str2);
  str2 = str1.ToWString();
  EXPECT_EQ(str1, str2);

  // Test userfree assignment.
  cef_string_userfree_wide_t uf = str2.DetachToUserFree();
  EXPECT_TRUE(uf != NULL);
  EXPECT_TRUE(str2.empty());
  str2.AttachToUserFree(uf);
  EXPECT_FALSE(str2.empty());
  EXPECT_EQ(str1, str2);
}

// Test base::string16 convertion to/from CefString types.
TEST(StringTest, string16) {
  CefStringUTF8 str8("Test String 1"), str8b;
  CefStringUTF16 str16("Test String 2"), str16b;
  CefStringWide strwide("Test String 3"), strwideb;
  base::string16 base_str;

  base_str = str8;
  str8b = base_str;
  EXPECT_EQ(str8, base_str);
  EXPECT_EQ(str8, str8b);

  base_str = str16;
  str16b = base_str;
  EXPECT_EQ(str16, base_str);
  EXPECT_EQ(str16, str16b);

  base_str = strwide;
  strwideb = base_str;
  EXPECT_EQ(strwide, base_str);
  EXPECT_EQ(strwide, strwideb);
}

// Test string lists.
TEST(StringTest, List) {
  typedef std::vector<CefString> ListType;
  ListType list;
  list.push_back("String 1");
  list.push_back("String 2");
  list.push_back("String 3");

  EXPECT_EQ(list[0], "String 1");
  EXPECT_EQ(list[1], "String 2");
  EXPECT_EQ(list[2], "String 3");

  cef_string_list_t listPtr = cef_string_list_alloc();
  EXPECT_TRUE(listPtr != NULL);
  ListType::const_iterator it = list.begin();
  for (; it != list.end(); ++it)
    cef_string_list_append(listPtr, it->GetStruct());

  CefString str;
  int ret;

  EXPECT_EQ(cef_string_list_size(listPtr), 3);

  ret = cef_string_list_value(listPtr, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 1");
  ret = cef_string_list_value(listPtr, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 2");
  ret = cef_string_list_value(listPtr, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 3");

  cef_string_list_t listPtr2 = cef_string_list_copy(listPtr);
  cef_string_list_clear(listPtr);
  EXPECT_EQ(cef_string_list_size(listPtr), 0);
  cef_string_list_free(listPtr);

  EXPECT_EQ(cef_string_list_size(listPtr2), 3);

  ret = cef_string_list_value(listPtr2, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 1");
  ret = cef_string_list_value(listPtr2, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 2");
  ret = cef_string_list_value(listPtr2, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 3");

  cef_string_list_free(listPtr2);
}

// Test string maps.
TEST(StringTest, Map) {
  typedef std::map<CefString, CefString> MapType;
  MapType map;
  map.insert(std::make_pair("Key 1", "String 1"));
  map.insert(std::make_pair("Key 2", "String 2"));
  map.insert(std::make_pair("Key 3", "String 3"));

  MapType::const_iterator it;

  it = map.find("Key 2");
  EXPECT_TRUE(it != map.end());
  EXPECT_EQ(it->first, "Key 2");
  EXPECT_EQ(it->second, "String 2");

  it = map.find(L"Key 2");
  EXPECT_TRUE(it != map.end());
  EXPECT_EQ(it->first, L"Key 2");
  EXPECT_EQ(it->second, L"String 2");

  EXPECT_EQ(map["Key 1"], "String 1");
  EXPECT_EQ(map["Key 2"], "String 2");
  EXPECT_EQ(map["Key 3"], "String 3");

  cef_string_map_t mapPtr = cef_string_map_alloc();

  it = map.begin();
  for (; it != map.end(); ++it) {
    cef_string_map_append(mapPtr, it->first.GetStruct(),
        it->second.GetStruct());
  }

  CefString str;
  int ret;

  EXPECT_EQ(cef_string_map_size(mapPtr), 3);

  ret = cef_string_map_key(mapPtr, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 1");
  ret = cef_string_map_value(mapPtr, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 1");

  ret = cef_string_map_key(mapPtr, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 2");
  ret = cef_string_map_value(mapPtr, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 2");

  ret = cef_string_map_key(mapPtr, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 3");
  ret = cef_string_map_value(mapPtr, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 3");

  CefString key;
  key.FromASCII("Key 2");
  ret = cef_string_map_find(mapPtr, key.GetStruct(), str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 2");

  cef_string_map_clear(mapPtr);
  EXPECT_EQ(cef_string_map_size(mapPtr), 0);

  cef_string_map_free(mapPtr);
}

// Test string maps.
TEST(StringTest, Multimap) {
  typedef std::multimap<CefString, CefString> MapType;
  MapType map;
  map.insert(std::make_pair("Key 1", "String 1"));
  map.insert(std::make_pair("Key 2", "String 2"));
  map.insert(std::make_pair("Key 2", "String 2.1"));
  map.insert(std::make_pair("Key 3", "String 3"));

  MapType::const_iterator it;

  it = map.find("Key 2");
  EXPECT_TRUE(it != map.end());
  EXPECT_EQ(it->first, "Key 2");
  EXPECT_EQ(it->second, "String 2");

  std::pair<MapType::const_iterator, MapType::const_iterator>
      range_it = map.equal_range("Key 2");
  EXPECT_TRUE(range_it.first != range_it.second);
  MapType::const_iterator same_key_it = range_it.first;
  // Either of "String 2" or "String 2.1" is fine since
  // std::multimap provides no guarantee wrt the order of
  // values with the same key.
  EXPECT_EQ(same_key_it->second.ToString().find("String 2"), (size_t)0);
  EXPECT_EQ((++same_key_it)->second.ToString().find("String 2"), (size_t)0);
  EXPECT_EQ(map.count("Key 2"), (size_t)2);

  EXPECT_EQ(map.find("Key 1")->second, "String 1");
  EXPECT_EQ(map.find("Key 3")->second, "String 3");

  cef_string_multimap_t mapPtr = cef_string_multimap_alloc();

  it = map.begin();
  for (; it != map.end(); ++it) {
    cef_string_multimap_append(mapPtr, it->first.GetStruct(),
        it->second.GetStruct());
  }

  CefString str;
  int ret;

  EXPECT_EQ(cef_string_multimap_size(mapPtr), 4);

  ret = cef_string_multimap_key(mapPtr, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 1");
  ret = cef_string_multimap_value(mapPtr, 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 1");

  ret = cef_string_multimap_key(mapPtr, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 2");
  ret = cef_string_multimap_value(mapPtr, 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str.ToString().find("String 2"), (size_t)0);

  ret = cef_string_multimap_key(mapPtr, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 2");
  ret = cef_string_multimap_value(mapPtr, 2, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str.ToString().find("String 2"), (size_t)0);

  ret = cef_string_multimap_key(mapPtr, 3, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "Key 3");
  ret = cef_string_multimap_value(mapPtr, 3, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str, "String 3");

  CefString key;
  key.FromASCII("Key 2");
  ret = cef_string_multimap_find_count(mapPtr, key.GetStruct());
  EXPECT_EQ(ret, 2);

  ret = cef_string_multimap_enumerate(mapPtr,
                    key.GetStruct(), 0, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str.ToString().find("String 2"), (size_t)0);

  ret = cef_string_multimap_enumerate(mapPtr,
                    key.GetStruct(), 1, str.GetWritableStruct());
  EXPECT_TRUE(ret);
  EXPECT_EQ(str.ToString().find("String 2"), (size_t)0);

  cef_string_multimap_clear(mapPtr);
  EXPECT_EQ(cef_string_multimap_size(mapPtr), 0);

  cef_string_multimap_free(mapPtr);
}
