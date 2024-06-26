// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fpdfsdk/cpdfsdk_annot.h"
#include "fpdfsdk/cpdfsdk_annotiterator.h"
#include "fpdfsdk/cpdfsdk_formfillenvironment.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"
#include "fpdfsdk/formfiller/cffl_interactiveformfiller.h"
#include "fpdfsdk/pwl/cpwl_combo_box.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"
#include "public/fpdf_fwlevent.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

class CPWLComboBoxEditEmbedderTest : public EmbedderTest {
 protected:
  void SetUp() override {
    EmbedderTest::SetUp();
    CreateAndInitializeFormComboboxPDF();
  }

  void TearDown() override {
    UnloadPage(GetPage());
    EmbedderTest::TearDown();
  }

  void CreateAndInitializeFormComboboxPDF() {
    EXPECT_TRUE(OpenDocument("combobox_form.pdf"));
    m_page = LoadPage(0);
    ASSERT_TRUE(m_page);

    m_pFormFillEnv =
        CPDFSDKFormFillEnvironmentFromFPDFFormHandle(form_handle());
    CPDFSDK_AnnotIterator iter(m_pFormFillEnv->GetPageView(0),
                               CPDF_Annot::Subtype::WIDGET);

    // User editable combobox.
    m_pAnnotEditable = iter.GetFirstAnnot();
    ASSERT_TRUE(m_pAnnotEditable);
    ASSERT_EQ(CPDF_Annot::Subtype::WIDGET, m_pAnnotEditable->GetAnnotSubtype());

    // Normal combobox with pre-selected value.
    m_pAnnotNormal = iter.GetNextAnnot(m_pAnnotEditable);
    ASSERT_TRUE(m_pAnnotNormal);
    ASSERT_EQ(CPDF_Annot::Subtype::WIDGET, m_pAnnotNormal->GetAnnotSubtype());

    // Read-only combobox.
    CPDFSDK_Annot* pAnnotReadOnly = iter.GetNextAnnot(m_pAnnotNormal);
    CPDFSDK_Annot* pLastAnnot = iter.GetLastAnnot();
    ASSERT_EQ(pAnnotReadOnly, pLastAnnot);
  }

  void FormFillerAndWindowSetup(CPDFSDK_Annot* pAnnotCombobox) {
    CFFL_InteractiveFormFiller* pInteractiveFormFiller =
        m_pFormFillEnv->GetInteractiveFormFiller();
    {
      CPDFSDK_Annot::ObservedPtr pObserved(pAnnotCombobox);
      EXPECT_TRUE(pInteractiveFormFiller->OnSetFocus(&pObserved, 0));
    }

    m_pFormFiller =
        pInteractiveFormFiller->GetFormFiller(pAnnotCombobox, false);
    ASSERT_TRUE(m_pFormFiller);

    CPWL_Wnd* pWindow =
        m_pFormFiller->GetPDFWindow(m_pFormFillEnv->GetPageView(0), false);
    ASSERT_TRUE(pWindow);
    m_pComboBox = static_cast<CPWL_ComboBox*>(pWindow);
  }

  void TypeTextIntoTextField(int num_chars) {
    // Type text starting with 'A' to as many chars as specified by |num_chars|.
    for (int i = 0; i < num_chars; ++i) {
      EXPECT_TRUE(GetCFFLFormFiller()->OnChar(GetCPDFSDKAnnotUserEditable(),
                                              i + 'A', 0));
    }
  }

  FPDF_PAGE GetPage() const { return m_page; }
  CPWL_ComboBox* GetCPWLComboBox() const { return m_pComboBox; }
  CFFL_FormFiller* GetCFFLFormFiller() const { return m_pFormFiller; }
  CPDFSDK_Annot* GetCPDFSDKAnnotNormal() const { return m_pAnnotNormal; }
  CPDFSDK_Annot* GetCPDFSDKAnnotUserEditable() const {
    return m_pAnnotEditable;
  }
  CPDFSDK_FormFillEnvironment* GetCPDFSDKFormFillEnv() const {
    return m_pFormFillEnv;
  }

 private:
  FPDF_PAGE m_page;
  CPWL_ComboBox* m_pComboBox;
  CFFL_FormFiller* m_pFormFiller;
  CPDFSDK_Annot* m_pAnnotNormal;
  CPDFSDK_Annot* m_pAnnotEditable;
  CPDFSDK_FormFillEnvironment* m_pFormFillEnv;
};

TEST_F(CPWLComboBoxEditEmbedderTest, GetSelectedTextEmptyAndBasicNormal) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotNormal());

  // Automatically pre-filled with "Banana".
  EXPECT_FALSE(GetCPWLComboBox()->GetText().IsEmpty());
  EXPECT_STREQ(L"Banana", GetCPWLComboBox()->GetText().c_str());

  // Check that selection is intially empty, then select entire word.
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());
  GetCPWLComboBox()->SetSelectText();
  EXPECT_STREQ(L"Banana", GetCPWLComboBox()->GetSelectedText().c_str());

  // Select other options.
  GetCPWLComboBox()->SetSelect(0);
  EXPECT_STREQ(L"Apple", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->SetSelect(2);
  EXPECT_STREQ(L"Cherry", GetCPWLComboBox()->GetSelectedText().c_str());

  // Verify that combobox text cannot be edited.
  EXPECT_FALSE(GetCFFLFormFiller()->OnChar(GetCPDFSDKAnnotNormal(), 'a', 0));
}

TEST_F(CPWLComboBoxEditEmbedderTest, GetSelectedTextFragmentsNormal) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotNormal());
  EXPECT_STREQ(L"Banana", GetCPWLComboBox()->GetText().c_str());

  GetCPWLComboBox()->SetEditSelection(0, 0);
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());

  GetCPWLComboBox()->SetEditSelection(0, 1);
  EXPECT_STREQ(L"B", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(0, -1);
  EXPECT_STREQ(L"Banana", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(-8, -1);
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());

  GetCPWLComboBox()->SetEditSelection(4, 1);
  EXPECT_STREQ(L"ana", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(1, 4);
  EXPECT_STREQ(L"ana", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(5, 6);
  EXPECT_STREQ(L"a", GetCPWLComboBox()->GetSelectedText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, GetSelectedTextEmptyAndBasicEditable) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  EXPECT_TRUE(GetCPWLComboBox()->GetText().IsEmpty());

  // Check selection is intially empty, then select a provided option.
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());
  GetCPWLComboBox()->SetSelect(0);
  GetCPWLComboBox()->SetSelectText();
  EXPECT_STREQ(L"Foo", GetCPWLComboBox()->GetSelectedText().c_str());

  // Select another option and then select last char of that option.
  GetCPWLComboBox()->SetSelect(1);
  EXPECT_STREQ(L"Bar", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->SetEditSelection(2, 3);
  EXPECT_STREQ(L"r", GetCPWLComboBox()->GetSelectedText().c_str());

  // Type into editable combobox text field and select new text.
  EXPECT_TRUE(
      GetCFFLFormFiller()->OnChar(GetCPDFSDKAnnotUserEditable(), 'a', 0));
  EXPECT_TRUE(
      GetCFFLFormFiller()->OnChar(GetCPDFSDKAnnotUserEditable(), 'b', 0));
  EXPECT_TRUE(
      GetCFFLFormFiller()->OnChar(GetCPDFSDKAnnotUserEditable(), 'c', 0));

  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());
  GetCPWLComboBox()->SetEditSelection(0, 5);
  EXPECT_STREQ(L"Baabc", GetCPWLComboBox()->GetSelectedText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, GetSelectedTextFragmentsEditable) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->SetEditSelection(0, 0);
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());

  GetCPWLComboBox()->SetEditSelection(0, 1);
  EXPECT_STREQ(L"A", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(0, -1);
  EXPECT_STREQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqr",
               GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(-8, -1);
  EXPECT_TRUE(GetCPWLComboBox()->GetSelectedText().IsEmpty());

  GetCPWLComboBox()->SetEditSelection(23, 12);
  EXPECT_STREQ(L"MNOPQRSTUVW", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(12, 23);
  EXPECT_STREQ(L"MNOPQRSTUVW", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(49, 50);
  EXPECT_STREQ(L"r", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->SetEditSelection(49, 55);
  EXPECT_STREQ(L"r", GetCPWLComboBox()->GetSelectedText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, DeleteEntireTextSelection) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->SetEditSelection(0, -1);
  EXPECT_STREQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqr",
               GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->ReplaceSelection(L"");
  EXPECT_TRUE(GetCPWLComboBox()->GetText().IsEmpty());
}

TEST_F(CPWLComboBoxEditEmbedderTest, DeleteTextSelectionMiddle) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->SetEditSelection(12, 23);
  EXPECT_STREQ(L"MNOPQRSTUVW", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->ReplaceSelection(L"");
  EXPECT_STREQ(L"ABCDEFGHIJKLXYZ[\\]^_`abcdefghijklmnopqr",
               GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, DeleteTextSelectionLeft) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->SetEditSelection(0, 5);
  EXPECT_STREQ(L"ABCDE", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->ReplaceSelection(L"");
  EXPECT_STREQ(L"FGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqr",
               GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, DeleteTextSelectionRight) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->SetEditSelection(45, 50);
  EXPECT_STREQ(L"nopqr", GetCPWLComboBox()->GetSelectedText().c_str());

  GetCPWLComboBox()->ReplaceSelection(L"");
  EXPECT_STREQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm",
               GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, DeleteEmptyTextSelection) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(50);

  GetCPWLComboBox()->ReplaceSelection(L"");
  EXPECT_STREQ(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqr",
               GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest, InsertTextInEmptyEditableComboBox) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"Hello", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextInPopulatedEditableComboBoxLeft) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  // Move cursor to beginning of user-editable combobox text field.
  EXPECT_TRUE(GetCFFLFormFiller()->OnKeyDown(GetCPDFSDKAnnotUserEditable(),
                                             FWL_VKEY_Home, 0));

  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"HelloABCDEFGHIJ", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextInPopulatedEditableComboBoxMiddle) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  // Move cursor to middle of user-editable combobox text field.
  for (int i = 0; i < 5; ++i) {
    EXPECT_TRUE(GetCFFLFormFiller()->OnKeyDown(GetCPDFSDKAnnotUserEditable(),
                                               FWL_VKEY_Left, 0));
  }

  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"ABCDEHelloFGHIJ", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextInPopulatedEditableComboBoxRight) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"ABCDEFGHIJHello", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextAndReplaceSelectionInPopulatedEditableComboBoxWhole) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  GetCPWLComboBox()->SetEditSelection(0, -1);
  EXPECT_STREQ(L"ABCDEFGHIJ", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"Hello", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextAndReplaceSelectionInPopulatedEditableComboBoxLeft) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  GetCPWLComboBox()->SetEditSelection(0, 5);
  EXPECT_STREQ(L"ABCDE", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"HelloFGHIJ", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextAndReplaceSelectionInPopulatedEditableComboBoxMiddle) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  GetCPWLComboBox()->SetEditSelection(2, 7);
  EXPECT_STREQ(L"CDEFG", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"ABHelloHIJ", GetCPWLComboBox()->GetText().c_str());
}

TEST_F(CPWLComboBoxEditEmbedderTest,
       InsertTextAndReplaceSelectionInPopulatedEditableComboBoxRight) {
  FormFillerAndWindowSetup(GetCPDFSDKAnnotUserEditable());
  TypeTextIntoTextField(10);

  GetCPWLComboBox()->SetEditSelection(5, 10);
  EXPECT_STREQ(L"FGHIJ", GetCPWLComboBox()->GetSelectedText().c_str());
  GetCPWLComboBox()->ReplaceSelection(L"Hello");
  EXPECT_STREQ(L"ABCDEHello", GetCPWLComboBox()->GetText().c_str());
}
