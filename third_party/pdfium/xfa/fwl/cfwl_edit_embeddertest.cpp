// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/widestring.h"
#include "public/fpdf_formfill.h"
#include "public/fpdf_fwlevent.h"
#include "testing/embedder_test.h"
#include "testing/embedder_test_timer_handling_delegate.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/xfa_js_embedder_test.h"

class CFWLEditEmbedderTest : public XFAJSEmbedderTest {
 protected:
  void SetUp() override {
    EmbedderTest::SetUp();
    SetDelegate(&delegate_);
  }

  void TearDown() override {
    UnloadPage(page());
    EmbedderTest::TearDown();
  }

  void CreateAndInitializeFormPDF(const char* filename) {
    EXPECT_TRUE(OpenDocument(filename));
    page_ = LoadPage(0);
    ASSERT_TRUE(page_);
  }

  FPDF_PAGE page() const { return page_; }
  EmbedderTestTimerHandlingDelegate delegate() const { return delegate_; }

 private:
  FPDF_PAGE page_;
  EmbedderTestTimerHandlingDelegate delegate_;
};

TEST_F(CFWLEditEmbedderTest, Trivial) {
  CreateAndInitializeFormPDF("xfa/email_recommended.pdf");
  ASSERT_EQ(0u, delegate().GetAlerts().size());
}

TEST_F(CFWLEditEmbedderTest, LeftClickMouseSelection) {
  CreateAndInitializeFormPDF("xfa/email_recommended.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);
  for (size_t i = 0; i < 10; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);

  // Mouse selection
  FORM_OnLButtonDown(form_handle(), page(), 0, 128, 58);
  FORM_OnLButtonDown(form_handle(), page(), FWL_EVENTFLAG_ShiftKey, 152, 58);

  // 12 == (2 * strlen(defgh)) + 2 (for \0\0)
  EXPECT_EQ(12UL, FORM_GetSelectedText(form_handle(), page(), nullptr, 0));

  unsigned short buf[128];
  unsigned long len = FORM_GetSelectedText(form_handle(), page(), &buf, 128);
  EXPECT_STREQ(L"defgh", WideString::FromUTF16LE(buf, len).c_str());
}

TEST_F(CFWLEditEmbedderTest, DragMouseSelection) {
  CreateAndInitializeFormPDF("xfa/email_recommended.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);
  for (size_t i = 0; i < 10; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);

  // Mouse selection
  FORM_OnLButtonDown(form_handle(), page(), 0, 128, 58);
  FORM_OnMouseMove(form_handle(), page(), FWL_EVENTFLAG_ShiftKey, 152, 58);

  // 12 == (2 * strlen(defgh)) + 2 (for \0\0)
  EXPECT_EQ(12UL, FORM_GetSelectedText(form_handle(), page(), nullptr, 0));

  unsigned short buf[128];
  unsigned long len = FORM_GetSelectedText(form_handle(), page(), &buf, 128);
  EXPECT_STREQ(L"defgh", WideString::FromUTF16LE(buf, len).c_str());

  // TODO(hnakashima): This is incorrect. Visually 'abcdefgh' are selected.
  const char kDraggedMD5[] = "69c13fe53b5fc422ebeab56d101a4658";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kDraggedMD5);
  }
}

TEST_F(CFWLEditEmbedderTest, SimpleFill) {
  CreateAndInitializeFormPDF("xfa/email_recommended.pdf");
  const char kBlankMD5[] = "eea5c72701270ac4a7edcc4df66d812a";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kBlankMD5);
  }

  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);
  for (size_t i = 0; i < 10; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);

  const char kFilledMD5[] = "e73263fcea46c18d874b3d5a79f53805";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMD5);
  }
}

TEST_F(CFWLEditEmbedderTest, FillWithNewLineWithoutMultiline) {
  CreateAndInitializeFormPDF("xfa/email_recommended.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);
  for (size_t i = 0; i < 5; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);
  FORM_OnChar(form_handle(), page(), '\r', 0);
  for (size_t i = 5; i < 10; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);

  const char kFilledMD5[] = "e73263fcea46c18d874b3d5a79f53805";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMD5);
  }
}

// Disabled due to flakiness.
TEST_F(CFWLEditEmbedderTest, DISABLED_FillWithNewLineWithMultiline) {
  CreateAndInitializeFormPDF("xfa/xfa_multiline_textfield.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);

  for (size_t i = 0; i < 5; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);
  FORM_OnChar(form_handle(), page(), '\r', 0);
  for (size_t i = 5; i < 10; ++i)
    FORM_OnChar(form_handle(), page(), 'a' + i, 0);

  // Should look like:
  // abcde
  // fghij|
  {
#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
    const char kFilledMultilineMD5[] = "fc1f4d5fdb2c5755005fc525b0a60ec9";
#else
    const char kFilledMultilineMD5[] = "a5654e027d8b1667c20f3b86d1918003";
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMultilineMD5);
  }

  for (size_t i = 0; i < 4; ++i)
    FORM_OnKeyDown(form_handle(), page(), FWL_VKEY_Left, 0);

  // Should look like:
  // abcde
  // f|ghij

  // Two backspaces is a workaround because left arrow does not behave well
  // in the first character of a line. It skips back to the previous line.
  for (size_t i = 0; i < 2; ++i)
    FORM_OnChar(form_handle(), page(), '\b', 0);

  // Should look like:
  // abcde|ghij
  {
#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
    const char kMultilineBackspaceMD5[] = "8bb62a8100ff1e1cc113d4033e0d824e";
#else
    const char kMultilineBackspaceMD5[] = "a2f1dcab92bb1fb7c2f9ccc70100c989";
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kMultilineBackspaceMD5);
  }
}

TEST_F(CFWLEditEmbedderTest, DateTimePickerTest) {
  CreateAndInitializeFormPDF("xfa/xfa_date_time_edit.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);

  const char kFilledMD5[] = "1036b8837a9dba75c6bd8f9347ae2eb2";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMD5);
  }
}

TEST_F(CFWLEditEmbedderTest, ImageEditTest) {
  CreateAndInitializeFormPDF("xfa/xfa_image_edit.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);

  const char kFilledMD5[] = "1940568c9ba33bac5d0b1ee9558c76b3";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMD5);
  }
}

TEST_F(CFWLEditEmbedderTest, ComboBoxTest) {
  CreateAndInitializeFormPDF("xfa/xfa_combobox.pdf");
  FORM_OnLButtonDown(form_handle(), page(), 0, 115, 58);

  const char kFilledMD5[] = "dad642ae8a5afce2591ffbcabbfc58dd";
  {
    ScopedFPDFBitmap page_bitmap =
        RenderPageWithFlags(page(), form_handle(), FPDF_ANNOT);
    CompareBitmap(page_bitmap.get(), 612, 792, kFilledMD5);
  }
}
