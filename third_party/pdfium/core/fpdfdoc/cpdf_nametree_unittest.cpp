// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfdoc/cpdf_nametree.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

void AddNameKeyValue(CPDF_Array* pNames, const char* key, const int value) {
  pNames->AddNew<CPDF_String>(key, false);
  pNames->AddNew<CPDF_Number>(value);
}

void CheckNameKeyValue(CPDF_Array* pNames,
                       const int index,
                       const char* key,
                       const int value) {
  EXPECT_STREQ(key, pNames->GetStringAt(index * 2).c_str());
  EXPECT_EQ(value, pNames->GetIntegerAt(index * 2 + 1));
}

void AddLimitsArray(CPDF_Dictionary* pNode,
                    const char* least,
                    const char* greatest) {
  CPDF_Array* pLimits = pNode->SetNewFor<CPDF_Array>("Limits");
  pLimits->AddNew<CPDF_String>(least, false);
  pLimits->AddNew<CPDF_String>(greatest, false);
}

void CheckLimitsArray(CPDF_Dictionary* pNode,
                      const char* least,
                      const char* greatest) {
  CPDF_Array* pLimits = pNode->GetArrayFor("Limits");
  ASSERT_TRUE(pLimits);
  EXPECT_STREQ(least, pLimits->GetStringAt(0).c_str());
  EXPECT_STREQ(greatest, pLimits->GetStringAt(1).c_str());
}

void FillNameTreeDict(CPDF_Dictionary* pRootDict) {
  CPDF_Array* pKids = pRootDict->SetNewFor<CPDF_Array>("Kids");
  CPDF_Dictionary* pKid1 = pKids->AddNew<CPDF_Dictionary>();

  // Make the lower and upper limit out of order on purpose.
  AddLimitsArray(pKid1, "9.txt", "1.txt");
  pKids = pKid1->SetNewFor<CPDF_Array>("Kids");
  CPDF_Dictionary* pKid2 = pKids->AddNew<CPDF_Dictionary>();
  CPDF_Dictionary* pKid3 = pKids->AddNew<CPDF_Dictionary>();

  AddLimitsArray(pKid2, "1.txt", "5.txt");
  pKids = pKid2->SetNewFor<CPDF_Array>("Kids");
  CPDF_Dictionary* pKid4 = pKids->AddNew<CPDF_Dictionary>();
  CPDF_Dictionary* pKid5 = pKids->AddNew<CPDF_Dictionary>();

  AddLimitsArray(pKid3, "9.txt", "9.txt");
  CPDF_Array* pNames = pKid3->SetNewFor<CPDF_Array>("Names");
  AddNameKeyValue(pNames, "9.txt", 999);

  // Make the lower and upper limit out of order on purpose.
  AddLimitsArray(pKid4, "2.txt", "1.txt");
  pNames = pKid4->SetNewFor<CPDF_Array>("Names");
  AddNameKeyValue(pNames, "1.txt", 111);
  AddNameKeyValue(pNames, "2.txt", 222);

  AddLimitsArray(pKid5, "3.txt", "5.txt");
  pNames = pKid5->SetNewFor<CPDF_Array>("Names");
  AddNameKeyValue(pNames, "3.txt", 333);
  AddNameKeyValue(pNames, "5.txt", 555);
}

}  // namespace

TEST(cpdf_nametree, GetUnicodeNameWithBOM) {
  // Set up the root dictionary with a Names array.
  auto pRootDict = pdfium::MakeUnique<CPDF_Dictionary>();
  CPDF_Array* pNames = pRootDict->SetNewFor<CPDF_Array>("Names");

  // Add the key "1" (with BOM) and value 100 into the array.
  std::ostringstream buf;
  constexpr char kData[] = "\xFE\xFF\x00\x31";
  for (size_t i = 0; i < sizeof(kData); ++i)
    buf.put(kData[i]);
  pNames->AddNew<CPDF_String>(ByteString(buf), true);
  pNames->AddNew<CPDF_Number>(100);

  // Check that the key is as expected.
  CPDF_NameTree nameTree(pRootDict.get());
  WideString storedName;
  nameTree.LookupValueAndName(0, &storedName);
  EXPECT_STREQ(L"1", storedName.c_str());

  // Check that the correct value object can be obtained by looking up "1".
  WideString matchName = L"1";
  CPDF_Object* pObj = nameTree.LookupValue(matchName);
  ASSERT_TRUE(pObj->IsNumber());
  EXPECT_EQ(100, pObj->AsNumber()->GetInteger());
}

TEST(cpdf_nametree, AddIntoNames) {
  // Set up a name tree with a single Names array.
  auto pRootDict = pdfium::MakeUnique<CPDF_Dictionary>();
  CPDF_Array* pNames = pRootDict->SetNewFor<CPDF_Array>("Names");
  AddNameKeyValue(pNames, "2.txt", 222);
  AddNameKeyValue(pNames, "7.txt", 777);

  CPDF_NameTree nameTree(pRootDict.get());
  pNames = nameTree.GetRoot()->GetArrayFor("Names");

  // Insert a name that already exists in the names array.
  EXPECT_FALSE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(111), L"2.txt"));

  // Insert in the beginning of the names array.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(111), L"1.txt"));

  // Insert in the middle of the names array.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(555), L"5.txt"));

  // Insert at the end of the names array.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(999), L"9.txt"));

  // Check that the names array has the expected key-value pairs.
  CheckNameKeyValue(pNames, 0, "1.txt", 111);
  CheckNameKeyValue(pNames, 1, "2.txt", 222);
  CheckNameKeyValue(pNames, 2, "5.txt", 555);
  CheckNameKeyValue(pNames, 3, "7.txt", 777);
  CheckNameKeyValue(pNames, 4, "9.txt", 999);
}

TEST(cpdf_nametree, AddIntoKids) {
  // Set up a name tree with five nodes of three levels.
  auto pRootDict = pdfium::MakeUnique<CPDF_Dictionary>();
  FillNameTreeDict(pRootDict.get());
  CPDF_NameTree nameTree(pRootDict.get());

  // Check that adding an existing name would fail.
  EXPECT_FALSE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(444), L"9.txt"));

  // Add a name within the limits of a leaf node.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(444), L"4.txt"));
  ASSERT_TRUE(nameTree.LookupValue(L"4.txt"));
  EXPECT_EQ(444, nameTree.LookupValue(L"4.txt")->GetInteger());

  // Add a name that requires changing the limits of two bottom levels.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(666), L"6.txt"));
  ASSERT_TRUE(nameTree.LookupValue(L"6.txt"));
  EXPECT_EQ(666, nameTree.LookupValue(L"6.txt")->GetInteger());

  // Add a name that requires changing the limits of two top levels.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(99), L"99.txt"));
  ASSERT_TRUE(nameTree.LookupValue(L"99.txt"));
  EXPECT_EQ(99, nameTree.LookupValue(L"99.txt")->GetInteger());

  // Add a name that requires changing the lower limit of all levels.
  EXPECT_TRUE(
      nameTree.AddValueAndName(pdfium::MakeUnique<CPDF_Number>(-5), L"0.txt"));
  ASSERT_TRUE(nameTree.LookupValue(L"0.txt"));
  EXPECT_EQ(-5, nameTree.LookupValue(L"0.txt")->GetInteger());

  // Check that the node on the first level has the expected limits.
  CPDF_Dictionary* pKid1 =
      nameTree.GetRoot()->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid1);
  CheckLimitsArray(pKid1, "0.txt", "99.txt");

  // Check that the nodes on the second level has the expected limits and names.
  CPDF_Dictionary* pKid2 = pKid1->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid2);
  CheckLimitsArray(pKid2, "0.txt", "6.txt");

  CPDF_Dictionary* pKid3 = pKid1->GetArrayFor("Kids")->GetDictAt(1);
  ASSERT_TRUE(pKid3);
  CheckLimitsArray(pKid3, "9.txt", "99.txt");
  CPDF_Array* pNames = pKid3->GetArrayFor("Names");
  ASSERT_TRUE(pNames);
  CheckNameKeyValue(pNames, 0, "9.txt", 999);
  CheckNameKeyValue(pNames, 1, "99.txt", 99);

  // Check that the nodes on the third level has the expected limits and names.
  CPDF_Dictionary* pKid4 = pKid2->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid4);
  CheckLimitsArray(pKid4, "0.txt", "2.txt");
  pNames = pKid4->GetArrayFor("Names");
  ASSERT_TRUE(pNames);
  CheckNameKeyValue(pNames, 0, "0.txt", -5);
  CheckNameKeyValue(pNames, 1, "1.txt", 111);
  CheckNameKeyValue(pNames, 2, "2.txt", 222);

  CPDF_Dictionary* pKid5 = pKid2->GetArrayFor("Kids")->GetDictAt(1);
  ASSERT_TRUE(pKid5);
  CheckLimitsArray(pKid5, "3.txt", "6.txt");
  pNames = pKid5->GetArrayFor("Names");
  ASSERT_TRUE(pNames);
  CheckNameKeyValue(pNames, 0, "3.txt", 333);
  CheckNameKeyValue(pNames, 1, "4.txt", 444);
  CheckNameKeyValue(pNames, 2, "5.txt", 555);
  CheckNameKeyValue(pNames, 3, "6.txt", 666);
}

TEST(cpdf_nametree, DeleteFromKids) {
  // Set up a name tree with five nodes of three levels.
  auto pRootDict = pdfium::MakeUnique<CPDF_Dictionary>();
  FillNameTreeDict(pRootDict.get());
  CPDF_NameTree nameTree(pRootDict.get());

  // Retrieve the kid dictionaries.
  CPDF_Dictionary* pKid1 =
      nameTree.GetRoot()->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid1);
  CPDF_Dictionary* pKid2 = pKid1->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid2);
  CPDF_Dictionary* pKid3 = pKid1->GetArrayFor("Kids")->GetDictAt(1);
  ASSERT_TRUE(pKid3);
  CPDF_Dictionary* pKid4 = pKid2->GetArrayFor("Kids")->GetDictAt(0);
  ASSERT_TRUE(pKid4);
  CPDF_Dictionary* pKid5 = pKid2->GetArrayFor("Kids")->GetDictAt(1);
  ASSERT_TRUE(pKid5);

  // Check that deleting an out-of-bound index would fail.
  EXPECT_FALSE(nameTree.DeleteValueAndName(5));

  // Delete the name "9.txt", and check that its node gets deleted and its
  // parent node's limits get updated.
  WideString csName;
  ASSERT_TRUE(nameTree.LookupValue(L"9.txt"));
  EXPECT_EQ(999, nameTree.LookupValue(L"9.txt")->GetInteger());
  EXPECT_TRUE(nameTree.LookupValueAndName(4, &csName));
  EXPECT_STREQ(L"9.txt", csName.c_str());
  EXPECT_EQ(2u, pKid1->GetArrayFor("Kids")->size());
  EXPECT_TRUE(nameTree.DeleteValueAndName(4));
  EXPECT_EQ(1u, pKid1->GetArrayFor("Kids")->size());
  CheckLimitsArray(pKid1, "1.txt", "5.txt");

  // Delete the name "2.txt", and check that its node does not get deleted, its
  // node's limits get updated, and no other limits get updated.
  ASSERT_TRUE(nameTree.LookupValue(L"2.txt"));
  EXPECT_EQ(222, nameTree.LookupValue(L"2.txt")->GetInteger());
  EXPECT_TRUE(nameTree.LookupValueAndName(1, &csName));
  EXPECT_STREQ(L"2.txt", csName.c_str());
  EXPECT_EQ(4u, pKid4->GetArrayFor("Names")->size());
  EXPECT_TRUE(nameTree.DeleteValueAndName(1));
  EXPECT_EQ(2u, pKid4->GetArrayFor("Names")->size());
  CheckLimitsArray(pKid4, "1.txt", "1.txt");
  CheckLimitsArray(pKid2, "1.txt", "5.txt");
  CheckLimitsArray(pKid1, "1.txt", "5.txt");

  // Delete the name "1.txt", and check that its node gets deleted, and its
  // parent's and gradparent's limits get updated.
  ASSERT_TRUE(nameTree.LookupValue(L"1.txt"));
  EXPECT_EQ(111, nameTree.LookupValue(L"1.txt")->GetInteger());
  EXPECT_TRUE(nameTree.LookupValueAndName(0, &csName));
  EXPECT_STREQ(L"1.txt", csName.c_str());
  EXPECT_EQ(2u, pKid2->GetArrayFor("Kids")->size());
  EXPECT_TRUE(nameTree.DeleteValueAndName(0));
  EXPECT_EQ(1u, pKid2->GetArrayFor("Kids")->size());
  CheckLimitsArray(pKid2, "3.txt", "5.txt");
  CheckLimitsArray(pKid1, "3.txt", "5.txt");

  // Delete the name "3.txt", and check that its node does not get deleted, and
  // its node's, its parent's, and its grandparent's limits get updated.
  ASSERT_TRUE(nameTree.LookupValue(L"3.txt"));
  EXPECT_EQ(333, nameTree.LookupValue(L"3.txt")->GetInteger());
  EXPECT_TRUE(nameTree.LookupValueAndName(0, &csName));
  EXPECT_STREQ(L"3.txt", csName.c_str());
  EXPECT_EQ(4u, pKid5->GetArrayFor("Names")->size());
  EXPECT_TRUE(nameTree.DeleteValueAndName(0));
  EXPECT_EQ(2u, pKid5->GetArrayFor("Names")->size());
  CheckLimitsArray(pKid5, "5.txt", "5.txt");
  CheckLimitsArray(pKid2, "5.txt", "5.txt");
  CheckLimitsArray(pKid1, "5.txt", "5.txt");

  // Delete the name "5.txt", and check that all nodes in the tree get deleted
  // since they are now all empty.
  ASSERT_TRUE(nameTree.LookupValue(L"5.txt"));
  EXPECT_EQ(555, nameTree.LookupValue(L"5.txt")->GetInteger());
  EXPECT_TRUE(nameTree.LookupValueAndName(0, &csName));
  EXPECT_STREQ(L"5.txt", csName.c_str());
  EXPECT_EQ(1u, nameTree.GetRoot()->GetArrayFor("Kids")->size());
  EXPECT_TRUE(nameTree.DeleteValueAndName(0));
  EXPECT_EQ(0u, nameTree.GetRoot()->GetArrayFor("Kids")->size());

  // Check that the tree is now empty.
  EXPECT_EQ(0u, nameTree.GetCount());
  EXPECT_FALSE(nameTree.LookupValueAndName(0, &csName));
  EXPECT_FALSE(nameTree.DeleteValueAndName(0));
}
