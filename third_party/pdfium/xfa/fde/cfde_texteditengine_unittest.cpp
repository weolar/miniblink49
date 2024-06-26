// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fde/cfde_texteditengine.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"

class CFDE_TextEditEngineTest : public testing::Test {
 public:
  class Delegate final : public CFDE_TextEditEngine::Delegate {
   public:
    void Reset() {
      text_is_full = false;
      fail_validation = false;
    }

    void NotifyTextFull() override { text_is_full = true; }

    void OnCaretChanged() override {}
    void OnTextWillChange(CFDE_TextEditEngine::TextChange* change) override {}
    void OnTextChanged() override {}
    void OnSelChanged() override {}
    bool OnValidate(const WideString& wsText) override {
      return !fail_validation;
    }
    void SetScrollOffset(float fScrollOffset) override {}

    bool fail_validation = false;
    bool text_is_full = false;
  };

  CFDE_TextEditEngineTest() {}
  ~CFDE_TextEditEngineTest() override {}

  void SetUp() override {
    font_ =
        CFGAS_GEFont::LoadFont(L"Arial Black", 0, 0, GetGlobalFontManager());
    ASSERT_TRUE(font_.Get() != nullptr);

    engine_ = pdfium::MakeUnique<CFDE_TextEditEngine>();
    engine_->SetFont(font_);
    engine_->SetFontSize(12.0f);
  }

  void TearDown() override { engine_.reset(); }

  CFDE_TextEditEngine* engine() const { return engine_.get(); }

 private:
  RetainPtr<CFGAS_GEFont> font_;
  std::unique_ptr<CFDE_TextEditEngine> engine_;
};

TEST_F(CFDE_TextEditEngineTest, Insert) {
  EXPECT_STREQ(L"", engine()->GetText().c_str());

  engine()->Insert(0, L"");
  EXPECT_STREQ(L"", engine()->GetText().c_str());
  EXPECT_EQ(0U, engine()->GetLength());

  engine()->Insert(0, L"Hello");
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());
  EXPECT_EQ(5U, engine()->GetLength());

  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  EXPECT_EQ(11U, engine()->GetLength());

  engine()->Insert(5, L" New");
  EXPECT_STREQ(L"Hello New World", engine()->GetText().c_str());

  engine()->Insert(100, L" Cat");
  EXPECT_STREQ(L"Hello New World Cat", engine()->GetText().c_str());

  engine()->Clear();

  engine()->SetHasCharacterLimit(true);
  engine()->SetCharacterLimit(5);
  engine()->Insert(0, L"Hello");

  // No delegate
  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());

  engine()->SetCharacterLimit(8);
  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello Wo", engine()->GetText().c_str());

  engine()->Clear();

  // With Delegate
  auto delegate = pdfium::MakeUnique<CFDE_TextEditEngineTest::Delegate>();
  engine()->SetDelegate(delegate.get());

  engine()->SetCharacterLimit(5);
  engine()->Insert(0, L"Hello");

  // Insert when full.
  engine()->Insert(5, L" World");
  EXPECT_TRUE(delegate->text_is_full);
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());
  delegate->Reset();

  engine()->SetCharacterLimit(8);
  engine()->Insert(5, L" World");
  EXPECT_TRUE(delegate->text_is_full);
  EXPECT_STREQ(L"Hello Wo", engine()->GetText().c_str());
  delegate->Reset();
  engine()->SetHasCharacterLimit(false);

  engine()->Clear();
  engine()->Insert(0, L"Hello");

  // Insert Invalid text
  delegate->fail_validation = true;
  engine()->EnableValidation(true);
  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());

  delegate->fail_validation = false;
  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  engine()->EnableValidation(false);

  engine()->Clear();

  engine()->Insert(0, L"Hello\nWorld");
  EXPECT_FALSE(delegate->text_is_full);
  EXPECT_STREQ(L"Hello\nWorld", engine()->GetText().c_str());
  delegate->Reset();
  engine()->Clear();

  // Insert with limited area and over-fill
  engine()->LimitHorizontalScroll(true);
  engine()->SetAvailableWidth(60.0f);  // Fits 'Hello Wo'.
  engine()->Insert(0, L"Hello");
  EXPECT_FALSE(delegate->text_is_full);
  engine()->Insert(5, L" World");
  EXPECT_TRUE(delegate->text_is_full);
  EXPECT_STREQ(L"Hello Wo", engine()->GetText().c_str());
  engine()->LimitHorizontalScroll(false);

  delegate->Reset();
  engine()->Clear();

  engine()->SetLineSpace(12.0f);
  engine()->LimitVerticalScroll(true);
  // Default is one line of text.
  engine()->Insert(0, L"Hello");
  EXPECT_FALSE(delegate->text_is_full);
  engine()->Insert(5, L" Wo\nrld");
  EXPECT_TRUE(delegate->text_is_full);
  EXPECT_STREQ(L"Hello Wo\n", engine()->GetText().c_str());
  engine()->LimitVerticalScroll(false);

  engine()->SetDelegate(nullptr);
}

TEST_F(CFDE_TextEditEngineTest, Delete) {
  EXPECT_STREQ(L"", engine()->Delete(0, 50).c_str());
  EXPECT_STREQ(L"", engine()->GetText().c_str());

  engine()->Insert(0, L"Hello World");
  EXPECT_STREQ(L" World", engine()->Delete(5, 6).c_str());
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello World");
  EXPECT_STREQ(L" ", engine()->Delete(5, 1).c_str());
  EXPECT_STREQ(L"HelloWorld", engine()->GetText().c_str());

  EXPECT_STREQ(L"elloWorld", engine()->Delete(1, 50).c_str());
  EXPECT_STREQ(L"H", engine()->GetText().c_str());
}

TEST_F(CFDE_TextEditEngineTest, Clear) {
  EXPECT_STREQ(L"", engine()->GetText().c_str());

  engine()->Clear();
  EXPECT_STREQ(L"", engine()->GetText().c_str());

  engine()->Insert(0, L"Hello World");
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());

  engine()->Clear();
  EXPECT_STREQ(L"", engine()->GetText().c_str());
  EXPECT_EQ(0U, engine()->GetLength());
}

TEST_F(CFDE_TextEditEngineTest, GetChar) {
  // Out of bounds.
  EXPECT_EQ(L'\0', engine()->GetChar(0));

  engine()->Insert(0, L"Hello World");
  EXPECT_EQ(L'H', engine()->GetChar(0));
  EXPECT_EQ(L'd', engine()->GetChar(engine()->GetLength() - 1));
  EXPECT_EQ(L' ', engine()->GetChar(5));

  engine()->Insert(5, L" A");
  EXPECT_STREQ(L"Hello A World", engine()->GetText().c_str());
  EXPECT_EQ(L'W', engine()->GetChar(8));

  engine()->EnablePasswordMode(true);
  EXPECT_EQ(L'*', engine()->GetChar(8));

  engine()->SetAliasChar(L'+');
  EXPECT_EQ(L'+', engine()->GetChar(8));
}

TEST_F(CFDE_TextEditEngineTest, GetWidthOfChar) {
  // Out of Bounds.
  EXPECT_EQ(0U, engine()->GetWidthOfChar(0));

  engine()->Insert(0, L"Hello World");
  EXPECT_EQ(199920U, engine()->GetWidthOfChar(0));
  EXPECT_EQ(159840U, engine()->GetWidthOfChar(1));

  engine()->Insert(0, L"\t");
  EXPECT_EQ(0U, engine()->GetWidthOfChar(0));
}

TEST_F(CFDE_TextEditEngineTest, GetDisplayPos) {
  EXPECT_EQ(0U, engine()->GetDisplayPos(FDE_TEXTEDITPIECE()).size());
}

TEST_F(CFDE_TextEditEngineTest, Selection) {
  EXPECT_FALSE(engine()->HasSelection());
  engine()->SelectAll();
  EXPECT_FALSE(engine()->HasSelection());

  engine()->Insert(0, L"Hello World");
  EXPECT_STREQ(L"", engine()->DeleteSelectedText().c_str());

  EXPECT_FALSE(engine()->HasSelection());
  engine()->SelectAll();
  EXPECT_TRUE(engine()->HasSelection());
  EXPECT_STREQ(L"Hello World", engine()->GetSelectedText().c_str());

  engine()->ClearSelection();
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"", engine()->GetSelectedText().c_str());

  engine()->SelectAll();
  size_t start_idx;
  size_t count;
  std::tie(start_idx, count) = engine()->GetSelection();
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(11U, count);

  // Selection before gap.
  EXPECT_STREQ(L"Hello World", engine()->GetSelectedText().c_str());
  EXPECT_TRUE(engine()->HasSelection());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());

  engine()->Insert(5, L" A");
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"", engine()->GetSelectedText().c_str());

  // Selection over the gap.
  engine()->SelectAll();
  EXPECT_TRUE(engine()->HasSelection());
  EXPECT_STREQ(L"Hello A World", engine()->GetSelectedText().c_str());
  engine()->Clear();

  engine()->Insert(0, L"Hello World");
  engine()->SelectAll();

  EXPECT_STREQ(L"Hello World", engine()->DeleteSelectedText().c_str());
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"", engine()->GetText().c_str());

  engine()->Insert(0, L"Hello World");
  engine()->SetSelection(5, 5);
  EXPECT_STREQ(L" Worl", engine()->DeleteSelectedText().c_str());
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"Hellod", engine()->GetText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello World");
  engine()->SelectAll();
  engine()->ReplaceSelectedText(L"Goodbye Everybody");
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"Goodbye Everybody", engine()->GetText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello World");
  engine()->SetSelection(1, 4);
  engine()->ReplaceSelectedText(L"i,");
  EXPECT_FALSE(engine()->HasSelection());
  EXPECT_STREQ(L"Hi, World", engine()->GetText().c_str());

  // Selection fully after gap.
  engine()->Clear();
  engine()->Insert(0, L"Hello");
  engine()->Insert(0, L"A ");
  engine()->SetSelection(3, 6);
  EXPECT_STREQ(L"ello", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello World");
  engine()->ClearSelection();
  engine()->DeleteSelectedText();
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
}

TEST_F(CFDE_TextEditEngineTest, UndoRedo) {
  EXPECT_FALSE(engine()->CanUndo());
  EXPECT_FALSE(engine()->CanRedo());
  EXPECT_FALSE(engine()->Undo());
  EXPECT_FALSE(engine()->Redo());

  engine()->Insert(0, L"Hello");
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_FALSE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"", engine()->GetText().c_str());
  EXPECT_FALSE(engine()->CanUndo());
  EXPECT_TRUE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_FALSE(engine()->CanRedo());

  engine()->Clear();
  EXPECT_FALSE(engine()->CanUndo());
  EXPECT_FALSE(engine()->CanRedo());

  engine()->Insert(0, L"Hello World");
  engine()->SelectAll();
  engine()->DeleteSelectedText();
  EXPECT_STREQ(L"", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_STREQ(L"", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_FALSE(engine()->CanRedo());

  engine()->Insert(0, L"Hello World");
  engine()->SelectAll();
  engine()->ReplaceSelectedText(L"Goodbye Friend");
  EXPECT_STREQ(L"Goodbye Friend", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_STREQ(L"Goodbye Friend", engine()->GetText().c_str());

  engine()->Clear();
  engine()->SetMaxEditOperationsForTesting(3);
  engine()->Insert(0, L"First ");
  engine()->Insert(engine()->GetLength(), L"Second ");
  engine()->Insert(engine()->GetLength(), L"Third");

  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"First Second ", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->CanUndo());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_FALSE(
      engine()->CanUndo());  // Can't undo First; undo buffer too small.
  EXPECT_STREQ(L"First ", engine()->GetText().c_str());

  EXPECT_TRUE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_TRUE(engine()->CanRedo());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_FALSE(engine()->CanRedo());
  EXPECT_STREQ(L"First Second Third", engine()->GetText().c_str());

  engine()->Clear();

  engine()->SetMaxEditOperationsForTesting(4);

  // Go beyond the max operations limit.
  engine()->Insert(0, L"H");
  engine()->Insert(1, L"e");
  engine()->Insert(2, L"l");
  engine()->Insert(3, L"l");
  engine()->Insert(4, L"o");
  engine()->Insert(5, L" World");
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());

  // Do A, undo. Do B, undo. Redo should cause B.
  engine()->Delete(4, 3);
  EXPECT_STREQ(L"Hellorld", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  engine()->Delete(5, 6);
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->Redo());
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());

  // Undo down to the limit.
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello World", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hello", engine()->GetText().c_str());
  EXPECT_TRUE(engine()->Undo());
  EXPECT_STREQ(L"Hell", engine()->GetText().c_str());
  EXPECT_FALSE(engine()->Undo());
  EXPECT_STREQ(L"Hell", engine()->GetText().c_str());
}

TEST_F(CFDE_TextEditEngineTest, GetIndexForPoint) {
  engine()->SetFontSize(10.0f);
  engine()->Insert(0, L"Hello World");
  EXPECT_EQ(0U, engine()->GetIndexForPoint({0.0f, 0.0f}));
  EXPECT_EQ(11U, engine()->GetIndexForPoint({999999.0f, 9999999.0f}));
  EXPECT_EQ(11U, engine()->GetIndexForPoint({999999.0f, 0.0f}));
  EXPECT_EQ(1U, engine()->GetIndexForPoint({5.0f, 5.0f}));
  EXPECT_EQ(1U, engine()->GetIndexForPoint({10.0f, 5.0f}));
}

TEST_F(CFDE_TextEditEngineTest, GetIndexForPointLineWrap) {
  engine()->SetFontSize(10.0f);
  engine()->Insert(0,
                   L"A text long enough to span multiple lines and test "
                   L"getting indexes on multi-line edits.");
  EXPECT_EQ(0U, engine()->GetIndexForPoint({0.0f, 0.0f}));
  EXPECT_EQ(87U, engine()->GetIndexForPoint({999999.0f, 9999999.0f}));
  EXPECT_EQ(11U, engine()->GetIndexForPoint({999999.0f, 0.0f}));
  EXPECT_EQ(12U, engine()->GetIndexForPoint({1.0f, 10.0f}));
  EXPECT_EQ(1U, engine()->GetIndexForPoint({5.0f, 5.0f}));
  EXPECT_EQ(2U, engine()->GetIndexForPoint({10.0f, 5.0f}));
}

TEST_F(CFDE_TextEditEngineTest, GetIndexForPointSpaceAtEnd) {
  engine()->SetFontSize(10.0f);
  engine()->Insert(0, L"Hello World ");
  EXPECT_EQ(0U, engine()->GetIndexForPoint({0.0f, 0.0f}));
  EXPECT_EQ(12U, engine()->GetIndexForPoint({999999.0f, 9999999.0f}));
  EXPECT_EQ(12U, engine()->GetIndexForPoint({999999.0f, 0.0f}));
}

TEST_F(CFDE_TextEditEngineTest, GetIndexForPointLineBreaks) {
  engine()->SetFontSize(10.0f);
  engine()->Insert(0, L"Hello\nWorld");
  EXPECT_EQ(0U, engine()->GetIndexForPoint({0.0f, 0.0f}));
  EXPECT_EQ(5U, engine()->GetIndexForPoint({999999.0f, 0.0f}));
  EXPECT_EQ(6U, engine()->GetIndexForPoint({0.0f, 10.0f}));
  EXPECT_EQ(11U, engine()->GetIndexForPoint({999999.0f, 9999999.0f}));
}

TEST_F(CFDE_TextEditEngineTest, BoundsForWordAt) {
  size_t start_idx;
  size_t count;

  std::tie(start_idx, count) = engine()->BoundsForWordAt(100);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(0U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(0);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(5U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"Hello", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello World");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(100);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(0U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(0);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(5U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"Hello", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(1);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(5U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"Hello", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(4);
  EXPECT_EQ(0U, start_idx);
  EXPECT_EQ(5U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"Hello", engine()->GetSelectedText().c_str());

  // Select the space
  std::tie(start_idx, count) = engine()->BoundsForWordAt(5);
  EXPECT_EQ(5U, start_idx);
  EXPECT_EQ(1U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L" ", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(6);
  EXPECT_EQ(6U, start_idx);
  EXPECT_EQ(5U, count);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"123 456 789");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(5);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"456", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"123def789");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(5);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"123def789", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"abc456ghi");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(5);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"abc456ghi", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"hello, world");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(0);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"hello", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"hello, world");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(5);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L",", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"np-complete");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(6);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"complete", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"(123) 456-7890");
  std::tie(start_idx, count) = engine()->BoundsForWordAt(0);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"(", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(1);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"123", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(7);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"456", engine()->GetSelectedText().c_str());

  std::tie(start_idx, count) = engine()->BoundsForWordAt(11);
  engine()->SetSelection(start_idx, count);
  EXPECT_STREQ(L"7890", engine()->GetSelectedText().c_str());

  // Tests from:
  // http://unicode.org/Public/UNIDATA/auxiliary/WordBreakTest.html#samples
  struct bounds {
    size_t start;
    size_t end;
  };
  struct {
    const wchar_t* str;
    std::vector<const wchar_t*> results;
  } tests[] = {
      // {L"\r\na\n\u0308", {L"\r\n", L"a", L"\n", L"\u0308"}},
      // {L"a\u0308", {L"a\u0308"}},
      // {L" \u200d\u0646", {L" \u200d", L"\u0646"}},
      // {L"\u0646\u200d ", {L"\u0646\u200d", L" "}},
      {L"AAA", {L"AAA"}},
      {L"A:A", {L"A:A"}},
      {L"A::A", {L"A", L":", L":", L"A"}},
      // {L"\u05d0'", {L"\u05d0'"}},
      // {L"\u05d0\"\u05d0", {L"\u05d0\"\u05d0"}},
      {L"A00A", {L"A00A"}},
      {L"0,0", {L"0,0"}},
      {L"0,,0", {L"0", L",", L",", L"0"}},
      {L"\u3031\u3031", {L"\u3031\u3031"}},
      {L"A_0_\u3031_", {L"A_0_\u3031_"}},
      {L"A__A", {L"A__A"}},
      // {L"\u200d\u2640", {L"\u200d\u2640"}},
      // {L"a\u0308\u200b\u0308b", {L"a\u0308\u200b\u0308b"}},
  };

  for (auto t : tests) {
    engine()->Clear();
    engine()->Insert(0, t.str);

    size_t idx = 0;
    for (const auto* res : t.results) {
      std::tie(start_idx, count) = engine()->BoundsForWordAt(idx);
      engine()->SetSelection(start_idx, count);
      EXPECT_STREQ(res, engine()->GetSelectedText().c_str())
          << "Input: '" << t.str << "'";
      idx += count;
    }
  }
}

TEST_F(CFDE_TextEditEngineTest, CursorMovement) {
  engine()->Clear();
  engine()->Insert(0, L"Hello");

  EXPECT_EQ(0U, engine()->GetIndexLeft(0));
  EXPECT_EQ(5U, engine()->GetIndexRight(5));
  EXPECT_EQ(2U, engine()->GetIndexUp(2));
  EXPECT_EQ(2U, engine()->GetIndexDown(2));
  EXPECT_EQ(1U, engine()->GetIndexLeft(2));
  EXPECT_EQ(1U, engine()->GetIndexBefore(2));
  EXPECT_EQ(3U, engine()->GetIndexRight(2));
  EXPECT_EQ(0U, engine()->GetIndexAtStartOfLine(2));
  EXPECT_EQ(5U, engine()->GetIndexAtEndOfLine(2));

  engine()->Clear();
  engine()->Insert(0, L"The book is \"مدخل إلى C++\"");
  EXPECT_EQ(2U, engine()->GetIndexBefore(3));    // Before is to left.
  EXPECT_EQ(16U, engine()->GetIndexBefore(15));  // Before is to right.
  EXPECT_EQ(22U, engine()->GetIndexBefore(23));  // Before is to left.

  engine()->Clear();
  engine()->Insert(0, L"Hello\r\nWorld\r\nTest");
  // Move to end of Hello from start of World.
  engine()->SetSelection(engine()->GetIndexBefore(7U), 7);
  EXPECT_STREQ(L"\r\nWorld", engine()->GetSelectedText().c_str());

  // Second letter in Hello from second letter in World.
  engine()->SetSelection(engine()->GetIndexUp(8U), 2);
  EXPECT_STREQ(L"el", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Test.
  engine()->SetSelection(engine()->GetIndexUp(15U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Hello.
  engine()->SetSelection(engine()->GetIndexDown(1U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in Test from second letter in World.
  engine()->SetSelection(engine()->GetIndexDown(8U), 2);
  EXPECT_STREQ(L"es", engine()->GetSelectedText().c_str());

  size_t start_idx = engine()->GetIndexAtStartOfLine(8U);
  size_t end_idx = engine()->GetIndexAtEndOfLine(8U);
  engine()->SetSelection(start_idx, end_idx - start_idx);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  // Move past \r\n to before W.
  engine()->SetSelection(engine()->GetIndexRight(5U), 5);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Short\nAnd a very long line");
  engine()->SetSelection(engine()->GetIndexUp(14U), 11);
  EXPECT_STREQ(L"\nAnd a very", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"A Very long line\nShort");
  EXPECT_EQ(engine()->GetLength(), engine()->GetIndexDown(8U));

  engine()->Clear();
  engine()->Insert(0, L"Hello\rWorld\rTest");
  // Move to end of Hello from start of World.
  engine()->SetSelection(engine()->GetIndexBefore(6U), 6);
  EXPECT_STREQ(L"\rWorld", engine()->GetSelectedText().c_str());

  // Second letter in Hello from second letter in World.
  engine()->SetSelection(engine()->GetIndexUp(7U), 2);
  EXPECT_STREQ(L"el", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Test.
  engine()->SetSelection(engine()->GetIndexUp(13U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Hello.
  engine()->SetSelection(engine()->GetIndexDown(1U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in Test from second letter in World.
  engine()->SetSelection(engine()->GetIndexDown(7U), 2);
  EXPECT_STREQ(L"es", engine()->GetSelectedText().c_str());

  start_idx = engine()->GetIndexAtStartOfLine(7U);
  end_idx = engine()->GetIndexAtEndOfLine(7U);
  engine()->SetSelection(start_idx, end_idx - start_idx);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  // Move past \r to before W.
  engine()->SetSelection(engine()->GetIndexRight(5U), 5);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  engine()->Clear();
  engine()->Insert(0, L"Hello\nWorld\nTest");
  // Move to end of Hello from start of World.
  engine()->SetSelection(engine()->GetIndexBefore(6U), 6);
  EXPECT_STREQ(L"\nWorld", engine()->GetSelectedText().c_str());

  // Second letter in Hello from second letter in World.
  engine()->SetSelection(engine()->GetIndexUp(7U), 2);
  EXPECT_STREQ(L"el", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Test.
  engine()->SetSelection(engine()->GetIndexUp(13U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in World from second letter in Hello.
  engine()->SetSelection(engine()->GetIndexDown(1U), 2);
  EXPECT_STREQ(L"or", engine()->GetSelectedText().c_str());

  // Second letter in Test from second letter in World.
  engine()->SetSelection(engine()->GetIndexDown(7U), 2);
  EXPECT_STREQ(L"es", engine()->GetSelectedText().c_str());

  start_idx = engine()->GetIndexAtStartOfLine(7U);
  end_idx = engine()->GetIndexAtEndOfLine(7U);
  engine()->SetSelection(start_idx, end_idx - start_idx);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());

  // Move past \r to before W.
  engine()->SetSelection(engine()->GetIndexRight(5U), 5);
  EXPECT_STREQ(L"World", engine()->GetSelectedText().c_str());
}
