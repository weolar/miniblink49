// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_texteditengine.h"

#include <algorithm>
#include <limits>

#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fde/cfde_wordbreak_data.h"
#include "xfa/fgas/font/cfgas_gefont.h"

namespace {

constexpr size_t kMaxEditOperations = 128;
constexpr size_t kGapSize = 128;
constexpr size_t kPageWidthMax = 0xffff;

class InsertOperation final : public CFDE_TextEditEngine::Operation {
 public:
  InsertOperation(CFDE_TextEditEngine* engine,
                  size_t start_idx,
                  const WideString& added_text)
      : engine_(engine), start_idx_(start_idx), added_text_(added_text) {}

  ~InsertOperation() override {}

  void Redo() const override {
    engine_->Insert(start_idx_, added_text_,
                    CFDE_TextEditEngine::RecordOperation::kSkipRecord);
  }

  void Undo() const override {
    engine_->Delete(start_idx_, added_text_.GetLength(),
                    CFDE_TextEditEngine::RecordOperation::kSkipRecord);
  }

 private:
  UnownedPtr<CFDE_TextEditEngine> engine_;
  size_t start_idx_;
  WideString added_text_;
};

class DeleteOperation final : public CFDE_TextEditEngine::Operation {
 public:
  DeleteOperation(CFDE_TextEditEngine* engine,
                  size_t start_idx,
                  const WideString& removed_text)
      : engine_(engine), start_idx_(start_idx), removed_text_(removed_text) {}

  ~DeleteOperation() override {}

  void Redo() const override {
    engine_->Delete(start_idx_, removed_text_.GetLength(),
                    CFDE_TextEditEngine::RecordOperation::kSkipRecord);
  }

  void Undo() const override {
    engine_->Insert(start_idx_, removed_text_,
                    CFDE_TextEditEngine::RecordOperation::kSkipRecord);
  }

 private:
  UnownedPtr<CFDE_TextEditEngine> engine_;
  size_t start_idx_;
  WideString removed_text_;
};

class ReplaceOperation final : public CFDE_TextEditEngine::Operation {
 public:
  ReplaceOperation(CFDE_TextEditEngine* engine,
                   size_t start_idx,
                   const WideString& removed_text,
                   const WideString& added_text)
      : insert_op_(engine, start_idx, added_text),
        delete_op_(engine, start_idx, removed_text) {}

  ~ReplaceOperation() override {}

  void Redo() const override {
    delete_op_.Redo();
    insert_op_.Redo();
  }

  void Undo() const override {
    insert_op_.Undo();
    delete_op_.Undo();
  }

 private:
  InsertOperation insert_op_;
  DeleteOperation delete_op_;
};

bool CheckStateChangeForWordBreak(WordBreakProperty from,
                                  WordBreakProperty to) {
  ASSERT(static_cast<int>(from) < 13);

  return !!(gs_FX_WordBreak_Table[static_cast<int>(from)] &
            static_cast<uint16_t>(1 << static_cast<int>(to)));
}

WordBreakProperty GetWordBreakProperty(wchar_t wcCodePoint) {
  uint8_t dwProperty = gs_FX_WordBreak_CodePointProperties[wcCodePoint >> 1];
  return static_cast<WordBreakProperty>((wcCodePoint & 1) ? (dwProperty & 0x0F)
                                                          : (dwProperty >> 4));
}

int GetBreakFlagsFor(WordBreakProperty current, WordBreakProperty next) {
  if (current == WordBreakProperty::kMidLetter) {
    if (next == WordBreakProperty::kALetter)
      return 1;
  } else if (current == WordBreakProperty::kMidNum) {
    if (next == WordBreakProperty::kNumeric)
      return 2;
  } else if (current == WordBreakProperty::kMidNumLet) {
    if (next == WordBreakProperty::kALetter)
      return 1;
    if (next == WordBreakProperty::kNumeric)
      return 2;
  }
  return 0;
}

bool BreakFlagsChanged(int flags, WordBreakProperty previous) {
  return (flags != 1 || previous != WordBreakProperty::kALetter) &&
         (flags != 2 || previous != WordBreakProperty::kNumeric);
}

}  // namespace

CFDE_TextEditEngine::CFDE_TextEditEngine()
    : font_color_(0xff000000),
      font_size_(10.0f),
      line_spacing_(10.0f),
      text_length_(0),
      gap_position_(0),
      gap_size_(kGapSize),
      available_width_(kPageWidthMax),
      character_limit_(std::numeric_limits<size_t>::max()),
      visible_line_count_(1),
      next_operation_index_to_undo_(kMaxEditOperations - 1),
      next_operation_index_to_insert_(0),
      max_edit_operations_(kMaxEditOperations),
      character_alignment_(CFX_TxtLineAlignment_Left),
      has_character_limit_(false),
      is_comb_text_(false),
      is_dirty_(false),
      validation_enabled_(false),
      is_multiline_(false),
      is_linewrap_enabled_(false),
      limit_horizontal_area_(false),
      limit_vertical_area_(false),
      password_mode_(false),
      password_alias_(L'*'),
      has_selection_(false),
      selection_({0, 0}) {
  content_.resize(gap_size_);
  operation_buffer_.resize(max_edit_operations_);

  text_break_.SetFontSize(font_size_);
  text_break_.SetLineBreakTolerance(2.0f);
  text_break_.SetTabWidth(36);
}

CFDE_TextEditEngine::~CFDE_TextEditEngine() {}

void CFDE_TextEditEngine::Clear() {
  text_length_ = 0;
  gap_position_ = 0;
  gap_size_ = kGapSize;

  content_.clear();
  content_.resize(gap_size_);

  ClearSelection();
  ClearOperationRecords();
}

void CFDE_TextEditEngine::SetMaxEditOperationsForTesting(size_t max) {
  max_edit_operations_ = max;
  operation_buffer_.resize(max);

  ClearOperationRecords();
}

void CFDE_TextEditEngine::AdjustGap(size_t idx, size_t length) {
  static const size_t char_size = sizeof(WideString::CharType);

  // Move the gap, if necessary.
  if (idx < gap_position_) {
    memmove(content_.data() + idx + gap_size_, content_.data() + idx,
            (gap_position_ - idx) * char_size);
    gap_position_ = idx;
  } else if (idx > gap_position_) {
    memmove(content_.data() + gap_position_,
            content_.data() + gap_position_ + gap_size_,
            (idx - gap_position_) * char_size);
    gap_position_ = idx;
  }

  // If the gap is too small, make it bigger.
  if (length >= gap_size_) {
    size_t new_gap_size = length + kGapSize;
    content_.resize(text_length_ + new_gap_size);

    memmove(content_.data() + gap_position_ + new_gap_size,
            content_.data() + gap_position_ + gap_size_,
            (text_length_ - gap_position_) * char_size);

    gap_size_ = new_gap_size;
  }
}

size_t CFDE_TextEditEngine::CountCharsExceedingSize(const WideString& text,
                                                    size_t num_to_check) {
  if (!limit_horizontal_area_ && !limit_vertical_area_)
    return 0;

  auto text_out = pdfium::MakeUnique<CFDE_TextOut>();
  text_out->SetLineSpace(line_spacing_);
  text_out->SetFont(font_);
  text_out->SetFontSize(font_size_);

  FDE_TextStyle style;
  style.single_line_ = !is_multiline_;

  CFX_RectF text_rect;
  if (is_linewrap_enabled_) {
    style.line_wrap_ = true;
    text_rect.width = available_width_;
  } else {
    text_rect.width = kPageWidthMax;
  }
  text_out->SetStyles(style);

  size_t length = text.GetLength();
  WideStringView temp = text.AsStringView();

  float vertical_height = line_spacing_ * visible_line_count_;
  size_t chars_exceeding_size = 0;
  // TODO(dsinclair): Can this get changed to a binary search?
  for (size_t i = 0; i < num_to_check; i++) {
    // This does a lot of string copying ....
    // TODO(dsinclair): make CalcLogicSize take a WideStringC instead.
    text_out->CalcLogicSize(WideString(temp), &text_rect);

    if (limit_horizontal_area_ && text_rect.width <= available_width_)
      break;
    if (limit_vertical_area_ && text_rect.height <= vertical_height)
      break;

    --length;
    temp = temp.Mid(0, length);
    ++chars_exceeding_size;
  }

  return chars_exceeding_size;
}

void CFDE_TextEditEngine::Insert(size_t idx,
                                 const WideString& request_text,
                                 RecordOperation add_operation) {
  WideString text = request_text;
  if (text.GetLength() == 0)
    return;
  if (idx > text_length_)
    idx = text_length_;

  TextChange change;
  change.selection_start = idx;
  change.selection_end = idx;
  change.text = text;
  change.previous_text = GetText();
  change.cancelled = false;

  if (delegate_ && (add_operation != RecordOperation::kSkipRecord &&
                    add_operation != RecordOperation::kSkipNotify)) {
    delegate_->OnTextWillChange(&change);
    if (change.cancelled)
      return;

    text = change.text;
    idx = change.selection_start;

    // JS extended the selection, so delete it before we insert.
    if (change.selection_end != change.selection_start)
      DeleteSelectedText(RecordOperation::kSkipRecord);
  }

  size_t length = text.GetLength();
  if (length == 0)
    return;

  // If we're going to be too big we insert what we can and notify the
  // delegate we've filled the text after the insert is done.
  bool exceeded_limit = false;

  // Currently we allow inserting a number of characters over the text limit if
  // we're skipping notify. This means we're setting the formatted text into the
  // engine. Otherwise, if you enter 123456789 for an SSN into a field
  // with a 9 character limit and we reformat to 123-45-6789 we'll truncate
  // the 89 when inserting into the text edit. See https://crbug.com/pdfium/1089
  if (has_character_limit_ && add_operation != RecordOperation::kSkipNotify &&
      text_length_ + length > character_limit_) {
    exceeded_limit = true;
    length = character_limit_ - text_length_;
  }

  AdjustGap(idx, length);

  if (validation_enabled_ || limit_horizontal_area_ || limit_vertical_area_) {
    WideString str;
    if (gap_position_ > 0)
      str += WideStringView(content_.data(), gap_position_);

    str += text;

    if (text_length_ - gap_position_ > 0) {
      str += WideStringView(content_.data() + gap_position_ + gap_size_,
                            text_length_ - gap_position_);
    }

    if (validation_enabled_ && delegate_ && !delegate_->OnValidate(str)) {
      // TODO(dsinclair): Notify delegate of validation failure?
      return;
    }

    // Check if we've limited the horizontal/vertical area, and if so determine
    // how many of our characters would be outside the area.
    size_t chars_exceeding = CountCharsExceedingSize(str, length);
    if (chars_exceeding > 0) {
      // If none of the characters will fit, notify and exit.
      if (chars_exceeding == length) {
        if (delegate_)
          delegate_->NotifyTextFull();
        return;
      }

      // Some, but not all, chars will fit, insert them and then notify
      // we're full.
      exceeded_limit = true;
      length -= chars_exceeding;
    }
  }

  if (add_operation == RecordOperation::kInsertRecord) {
    AddOperationRecord(
        pdfium::MakeUnique<InsertOperation>(this, gap_position_, text));
  }

  WideString previous_text;
  if (delegate_)
    previous_text = GetText();

  // Copy the new text into the gap.
  static const size_t char_size = sizeof(WideString::CharType);
  memcpy(content_.data() + gap_position_, text.c_str(), length * char_size);
  gap_position_ += length;
  gap_size_ -= length;
  text_length_ += length;

  is_dirty_ = true;

  // Inserting text resets the selection.
  ClearSelection();

  if (delegate_) {
    if (exceeded_limit)
      delegate_->NotifyTextFull();

    delegate_->OnTextChanged();
  }
}

void CFDE_TextEditEngine::AddOperationRecord(std::unique_ptr<Operation> op) {
  size_t last_insert_position = next_operation_index_to_insert_ == 0
                                    ? max_edit_operations_ - 1
                                    : next_operation_index_to_insert_ - 1;

  // If our undo record is not the last thing we inserted then we need to
  // remove all the undo records between our insert position and the undo marker
  // and make that our new insert position.
  if (next_operation_index_to_undo_ != last_insert_position) {
    if (next_operation_index_to_undo_ > last_insert_position) {
      // Our Undo position is ahead of us, which means we need to clear out the
      // head of the queue.
      while (last_insert_position != 0) {
        operation_buffer_[last_insert_position].reset();
        --last_insert_position;
      }
      operation_buffer_[0].reset();

      // Moving this will let us then clear out the end, setting the undo
      // position to before the insert position.
      last_insert_position = max_edit_operations_ - 1;
    }

    // Clear out the vector from undo position to our set insert position.
    while (next_operation_index_to_undo_ != last_insert_position) {
      operation_buffer_[last_insert_position].reset();
      --last_insert_position;
    }
  }

  // We're now pointing at the next thing we want to Undo, so insert at the
  // next position in the queue.
  ++last_insert_position;
  if (last_insert_position >= max_edit_operations_)
    last_insert_position = 0;

  operation_buffer_[last_insert_position] = std::move(op);
  next_operation_index_to_insert_ =
      (last_insert_position + 1) % max_edit_operations_;
  next_operation_index_to_undo_ = last_insert_position;
}

void CFDE_TextEditEngine::ClearOperationRecords() {
  for (auto& record : operation_buffer_)
    record.reset();

  next_operation_index_to_undo_ = max_edit_operations_ - 1;
  next_operation_index_to_insert_ = 0;
}

size_t CFDE_TextEditEngine::GetIndexBefore(size_t pos) {
  int32_t bidi_level;
  CFX_RectF rect;
  // Possible |Layout| triggered by |GetCharacterInfo|.
  std::tie(bidi_level, rect) = GetCharacterInfo(pos);
  return FX_IsOdd(bidi_level) ? GetIndexRight(pos) : GetIndexLeft(pos);
}

size_t CFDE_TextEditEngine::GetIndexLeft(size_t pos) const {
  if (pos == 0)
    return 0;
  --pos;

  while (pos != 0) {
    // We want to be on the location just before the \r or \n
    wchar_t ch = GetChar(pos - 1);
    if (ch != '\r' && ch != '\n')
      break;

    --pos;
  }
  return pos;
}

size_t CFDE_TextEditEngine::GetIndexRight(size_t pos) const {
  if (pos >= text_length_)
    return text_length_;
  ++pos;

  wchar_t ch = GetChar(pos);
  // We want to be on the location after the \r\n.
  while (pos < text_length_ && (ch == '\r' || ch == '\n')) {
    ++pos;
    ch = GetChar(pos);
  }

  return pos;
}

size_t CFDE_TextEditEngine::GetIndexUp(size_t pos) const {
  size_t line_start = GetIndexAtStartOfLine(pos);
  if (line_start == 0)
    return pos;

  // Determine how far along the line we were.
  size_t dist = pos - line_start;

  // Move to the end of the preceding line.
  wchar_t ch;
  do {
    --line_start;
    ch = GetChar(line_start);
  } while (line_start != 0 && (ch == '\r' || ch == '\n'));

  if (line_start == 0)
    return dist;

  // Get the start of the line prior to the current line.
  size_t prior_start = GetIndexAtStartOfLine(line_start);

  // Prior line is shorter then next line, and we're past the end of that line
  // return the end of line.
  if (prior_start + dist > line_start)
    return GetIndexAtEndOfLine(line_start);

  return prior_start + dist;
}

size_t CFDE_TextEditEngine::GetIndexDown(size_t pos) const {
  size_t line_end = GetIndexAtEndOfLine(pos);
  if (line_end == text_length_)
    return pos;

  wchar_t ch;
  do {
    ++line_end;
    ch = GetChar(line_end);
  } while (line_end < text_length_ && (ch == '\r' || ch == '\n'));

  if (line_end == text_length_)
    return line_end;

  // Determine how far along the line we are.
  size_t dist = pos - GetIndexAtStartOfLine(pos);

  // Check if next line is shorter then current line. If so, return end
  // of next line.
  size_t next_line_end = GetIndexAtEndOfLine(line_end);
  if (line_end + dist > next_line_end)
    return next_line_end;

  return line_end + dist;
}

size_t CFDE_TextEditEngine::GetIndexAtStartOfLine(size_t pos) const {
  if (pos == 0)
    return 0;

  wchar_t ch = GetChar(pos);
  // What to do.
  if (ch == '\r' || ch == '\n')
    return pos;

  do {
    // We want to be on the location just after the \r\n
    ch = GetChar(pos - 1);
    if (ch == '\r' || ch == '\n')
      break;

    --pos;
  } while (pos > 0);

  return pos;
}

size_t CFDE_TextEditEngine::GetIndexAtEndOfLine(size_t pos) const {
  if (pos >= text_length_)
    return text_length_;

  wchar_t ch = GetChar(pos);
  // Not quite sure which way to go here?
  if (ch == '\r' || ch == '\n')
    return pos;

  // We want to be on the location of the first \r or \n.
  do {
    ++pos;
    ch = GetChar(pos);
  } while (pos < text_length_ && (ch != '\r' && ch != '\n'));

  return pos;
}

void CFDE_TextEditEngine::LimitHorizontalScroll(bool val) {
  ClearOperationRecords();
  limit_horizontal_area_ = val;
}

void CFDE_TextEditEngine::LimitVerticalScroll(bool val) {
  ClearOperationRecords();
  limit_vertical_area_ = val;
}

bool CFDE_TextEditEngine::CanUndo() const {
  return operation_buffer_[next_operation_index_to_undo_] != nullptr &&
         next_operation_index_to_undo_ != next_operation_index_to_insert_;
}

bool CFDE_TextEditEngine::CanRedo() const {
  size_t idx = (next_operation_index_to_undo_ + 1) % max_edit_operations_;
  return idx != next_operation_index_to_insert_ &&
         operation_buffer_[idx] != nullptr;
}

bool CFDE_TextEditEngine::Redo() {
  if (!CanRedo())
    return false;

  next_operation_index_to_undo_ =
      (next_operation_index_to_undo_ + 1) % max_edit_operations_;
  operation_buffer_[next_operation_index_to_undo_]->Redo();
  return true;
}

bool CFDE_TextEditEngine::Undo() {
  if (!CanUndo())
    return false;

  operation_buffer_[next_operation_index_to_undo_]->Undo();
  next_operation_index_to_undo_ = next_operation_index_to_undo_ == 0
                                      ? max_edit_operations_ - 1
                                      : next_operation_index_to_undo_ - 1;
  return true;
}

void CFDE_TextEditEngine::Layout() {
  if (!is_dirty_)
    return;

  is_dirty_ = false;
  RebuildPieces();
}

CFX_RectF CFDE_TextEditEngine::GetContentsBoundingBox() {
  // Layout if necessary.
  Layout();
  return contents_bounding_box_;
}

void CFDE_TextEditEngine::SetAvailableWidth(size_t width) {
  if (width == available_width_)
    return;

  ClearOperationRecords();

  available_width_ = width;
  if (is_linewrap_enabled_)
    text_break_.SetLineWidth(width);
  if (is_comb_text_)
    SetCombTextWidth();

  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetHasCharacterLimit(bool limit) {
  if (has_character_limit_ == limit)
    return;

  has_character_limit_ = limit;
  if (is_comb_text_)
    SetCombTextWidth();

  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetCharacterLimit(size_t limit) {
  if (character_limit_ == limit)
    return;

  ClearOperationRecords();

  character_limit_ = limit;
  if (is_comb_text_)
    SetCombTextWidth();

  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetFont(RetainPtr<CFGAS_GEFont> font) {
  if (font_ == font)
    return;

  font_ = font;
  text_break_.SetFont(font_);
  is_dirty_ = true;
}

RetainPtr<CFGAS_GEFont> CFDE_TextEditEngine::GetFont() const {
  return font_;
}

void CFDE_TextEditEngine::SetFontSize(float size) {
  if (font_size_ == size)
    return;

  font_size_ = size;
  text_break_.SetFontSize(font_size_);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetTabWidth(float width) {
  int32_t old_tab_width = text_break_.GetTabWidth();
  text_break_.SetTabWidth(width);
  if (old_tab_width == text_break_.GetTabWidth())
    return;

  is_dirty_ = true;
}

float CFDE_TextEditEngine::GetFontAscent() const {
  return (static_cast<float>(font_->GetAscent()) * font_size_) / 1000;
}

void CFDE_TextEditEngine::SetAlignment(uint32_t alignment) {
  if (alignment == character_alignment_)
    return;

  character_alignment_ = alignment;
  text_break_.SetAlignment(alignment);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetVisibleLineCount(size_t count) {
  if (visible_line_count_ == count)
    return;

  visible_line_count_ = std::max(static_cast<size_t>(1), count);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::EnableMultiLine(bool val) {
  if (is_multiline_ == val)
    return;

  is_multiline_ = true;

  uint32_t style = text_break_.GetLayoutStyles();
  if (is_multiline_)
    style &= ~FX_LAYOUTSTYLE_SingleLine;
  else
    style |= FX_LAYOUTSTYLE_SingleLine;
  text_break_.SetLayoutStyles(style);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::EnableLineWrap(bool val) {
  if (is_linewrap_enabled_ == val)
    return;

  is_linewrap_enabled_ = val;
  text_break_.SetLineWidth(is_linewrap_enabled_ ? available_width_
                                                : kPageWidthMax);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetCombText(bool enable) {
  if (is_comb_text_ == enable)
    return;

  is_comb_text_ = enable;

  uint32_t style = text_break_.GetLayoutStyles();
  if (enable) {
    style |= FX_LAYOUTSTYLE_CombText;
    SetCombTextWidth();
  } else {
    style &= ~FX_LAYOUTSTYLE_CombText;
  }
  text_break_.SetLayoutStyles(style);
  is_dirty_ = true;
}

void CFDE_TextEditEngine::SetCombTextWidth() {
  size_t width = available_width_;
  if (has_character_limit_)
    width /= character_limit_;

  text_break_.SetCombWidth(width);
}

void CFDE_TextEditEngine::SelectAll() {
  if (text_length_ == 0)
    return;

  has_selection_ = true;
  selection_.start_idx = 0;
  selection_.count = text_length_;
}

void CFDE_TextEditEngine::ClearSelection() {
  has_selection_ = false;
  selection_.start_idx = 0;
  selection_.count = 0;
}

void CFDE_TextEditEngine::SetSelection(size_t start_idx, size_t count) {
  if (count == 0) {
    ClearSelection();
    return;
  }

  if (start_idx > text_length_)
    return;
  if (start_idx + count > text_length_)
    count = text_length_ - start_idx;

  has_selection_ = true;
  selection_.start_idx = start_idx;
  selection_.count = count;
}

WideString CFDE_TextEditEngine::GetSelectedText() const {
  if (!has_selection_)
    return WideString();

  WideString text;
  if (selection_.start_idx < gap_position_) {
    // Fully on left of gap.
    if (selection_.start_idx + selection_.count < gap_position_) {
      text += WideStringView(content_.data() + selection_.start_idx,
                             selection_.count);
      return text;
    }

    // Pre-gap text
    text += WideStringView(content_.data() + selection_.start_idx,
                           gap_position_ - selection_.start_idx);

    if (selection_.count - (gap_position_ - selection_.start_idx) > 0) {
      // Post-gap text
      text += WideStringView(
          content_.data() + gap_position_ + gap_size_,
          selection_.count - (gap_position_ - selection_.start_idx));
    }

    return text;
  }

  // Fully right of gap
  text += WideStringView(content_.data() + gap_size_ + selection_.start_idx,
                         selection_.count);
  return text;
}

WideString CFDE_TextEditEngine::DeleteSelectedText(
    RecordOperation add_operation) {
  if (!has_selection_)
    return WideString();

  return Delete(selection_.start_idx, selection_.count, add_operation);
}

WideString CFDE_TextEditEngine::Delete(size_t start_idx,
                                       size_t length,
                                       RecordOperation add_operation) {
  if (start_idx >= text_length_)
    return WideString();

  TextChange change;
  change.text.clear();
  change.cancelled = false;
  if (delegate_ && (add_operation != RecordOperation::kSkipRecord &&
                    add_operation != RecordOperation::kSkipNotify)) {
    change.previous_text = GetText();
    change.selection_start = start_idx;
    change.selection_end = start_idx + length;

    delegate_->OnTextWillChange(&change);
    if (change.cancelled)
      return WideString();

    start_idx = change.selection_start;
    length = change.selection_end - change.selection_start;
  }

  length = std::min(length, text_length_ - start_idx);
  AdjustGap(start_idx + length, 0);

  WideString ret;
  ret += WideStringView(content_.data() + start_idx, length);

  if (add_operation == RecordOperation::kInsertRecord) {
    AddOperationRecord(
        pdfium::MakeUnique<DeleteOperation>(this, start_idx, ret));
  }

  WideString previous_text = GetText();

  gap_position_ = start_idx;
  gap_size_ += length;

  text_length_ -= length;
  is_dirty_ = true;
  ClearSelection();

  // The JS requested the insertion of text instead of just a deletion.
  if (!change.text.IsEmpty())
    Insert(start_idx, change.text, RecordOperation::kSkipRecord);

  if (delegate_)
    delegate_->OnTextChanged();

  return ret;
}

void CFDE_TextEditEngine::ReplaceSelectedText(const WideString& requested_rep) {
  WideString rep = requested_rep;

  if (delegate_) {
    TextChange change;
    change.selection_start = selection_.start_idx;
    change.selection_end = selection_.start_idx + selection_.count;
    change.text = rep;
    change.previous_text = GetText();
    change.cancelled = false;

    delegate_->OnTextWillChange(&change);
    if (change.cancelled)
      return;

    rep = change.text;
    selection_.start_idx = change.selection_start;
    selection_.count = change.selection_end - change.selection_start;
  }

  size_t start_idx = selection_.start_idx;
  WideString txt = DeleteSelectedText(RecordOperation::kSkipRecord);
  Insert(gap_position_, rep, RecordOperation::kSkipRecord);

  AddOperationRecord(
      pdfium::MakeUnique<ReplaceOperation>(this, start_idx, txt, rep));
}

WideString CFDE_TextEditEngine::GetText() const {
  WideString str;
  if (gap_position_ > 0)
    str += WideStringView(content_.data(), gap_position_);
  if (text_length_ - gap_position_ > 0) {
    str += WideStringView(content_.data() + gap_position_ + gap_size_,
                          text_length_ - gap_position_);
  }
  return str;
}

size_t CFDE_TextEditEngine::GetLength() const {
  return text_length_;
}

wchar_t CFDE_TextEditEngine::GetChar(size_t idx) const {
  if (idx >= text_length_)
    return L'\0';
  if (password_mode_)
    return password_alias_;

  return idx < gap_position_
             ? content_[idx]
             : content_[gap_position_ + gap_size_ + (idx - gap_position_)];
}

size_t CFDE_TextEditEngine::GetWidthOfChar(size_t idx) {
  // Recalculate the widths if necessary.
  Layout();
  return idx < char_widths_.size() ? char_widths_[idx] : 0;
}

size_t CFDE_TextEditEngine::GetIndexForPoint(const CFX_PointF& point) {
  // Recalculate the widths if necessary.
  Layout();

  auto start_it = text_piece_info_.begin();
  for (; start_it < text_piece_info_.end(); ++start_it) {
    if (start_it->rtPiece.top <= point.y &&
        point.y < start_it->rtPiece.bottom())
      break;
  }
  // We didn't find the point before getting to the end of the text, return
  // end of text.
  if (start_it == text_piece_info_.end())
    return text_length_;

  auto end_it = start_it;
  for (; end_it < text_piece_info_.end(); ++end_it) {
    // We've moved past where the point should be and didn't find anything.
    // Return the start of the current piece as the location.
    if (end_it->rtPiece.bottom() <= point.y || point.y < end_it->rtPiece.top)
      break;
  }
  // Make sure the end iterator is pointing to our text pieces.
  if (end_it == text_piece_info_.end())
    --end_it;

  size_t start_it_idx = start_it->nStart;
  for (; start_it <= end_it; ++start_it) {
    bool piece_contains_point_vertically =
        (point.y >= start_it->rtPiece.top &&
         point.y < start_it->rtPiece.bottom());
    if (!piece_contains_point_vertically)
      continue;

    std::vector<CFX_RectF> rects = GetCharRects(*start_it);
    for (size_t i = 0; i < rects.size(); ++i) {
      bool character_contains_point_horizontally =
          (point.x >= rects[i].left && point.x < rects[i].right());
      if (!character_contains_point_horizontally)
        continue;

      // When clicking on the left half of a character, the cursor should be
      // moved before it. If the click was on the right half of that character,
      // move the cursor after it.
      bool closer_to_left =
          (point.x - rects[i].left < rects[i].right() - point.x);
      int caret_pos = (closer_to_left ? i : i + 1);
      size_t pos = start_it->nStart + caret_pos;
      if (pos >= text_length_)
        return text_length_;

      wchar_t wch = GetChar(pos);
      if (wch == L'\n' || wch == L'\r') {
        if (wch == L'\n' && pos > 0 && GetChar(pos - 1) == L'\r')
          --pos;
        return pos;
      }

      // TODO(dsinclair): Old code had a before flag set based on bidi?
      return pos;
    }

    // Point is not within the horizontal range of any characters, it's
    // afterwards. Return the position after the the last character.
    // The last line has nCount equal to the number of characters + 1 (sentinel
    // character maybe?). Restrict to the text_length_ to account for that.
    size_t pos = std::min(
        static_cast<size_t>(start_it->nStart + start_it->nCount), text_length_);

    // If the line is not the last one and it was broken right after a breaking
    // whitespace (space or line break), the cursor should not be placed after
    // the whitespace, but before it. If the cursor is moved after the
    // whitespace, it goes to the beginning of the next line.
    bool is_last_line = (std::next(start_it) == text_piece_info_.end());
    if (!is_last_line && pos > 0) {
      wchar_t previous_char = GetChar(pos - 1);
      if (previous_char == L' ' || previous_char == L'\n' ||
          previous_char == L'\r') {
        --pos;
      }
    }

    return pos;
  }

  if (start_it == text_piece_info_.end())
    return start_it_idx;
  if (start_it == end_it)
    return start_it->nStart;

  // We didn't find the point before going over all of the pieces, we want to
  // return the start of the piece after the point.
  return end_it->nStart;
}

std::vector<CFX_RectF> CFDE_TextEditEngine::GetCharRects(
    const FDE_TEXTEDITPIECE& piece) {
  if (piece.nCount < 1)
    return std::vector<CFX_RectF>();

  CFX_TxtBreak::Run tr;
  tr.pEdtEngine = this;
  tr.iStart = piece.nStart;
  tr.iLength = piece.nCount;
  tr.pFont = font_;
  tr.fFontSize = font_size_;
  tr.dwStyles = text_break_.GetLayoutStyles();
  tr.dwCharStyles = piece.dwCharStyles;
  tr.pRect = &piece.rtPiece;
  return text_break_.GetCharRects(&tr, false);
}

std::vector<FXTEXT_CHARPOS> CFDE_TextEditEngine::GetDisplayPos(
    const FDE_TEXTEDITPIECE& piece) {
  if (piece.nCount < 1)
    return std::vector<FXTEXT_CHARPOS>();

  CFX_TxtBreak::Run tr;
  tr.pEdtEngine = this;
  tr.iStart = piece.nStart;
  tr.iLength = piece.nCount;
  tr.pFont = font_;
  tr.fFontSize = font_size_;
  tr.dwStyles = text_break_.GetLayoutStyles();
  tr.dwCharStyles = piece.dwCharStyles;
  tr.pRect = &piece.rtPiece;

  std::vector<FXTEXT_CHARPOS> data(text_break_.GetDisplayPos(&tr, nullptr));
  text_break_.GetDisplayPos(&tr, data.data());
  return data;
}

void CFDE_TextEditEngine::RebuildPieces() {
  text_break_.EndBreak(CFX_BreakType::Paragraph);
  text_break_.ClearBreakPieces();

  char_widths_.clear();
  text_piece_info_.clear();

  // Must have a font set in order to break the text.
  if (text_length_ == 0 || !font_)
    return;

  bool initialized_bounding_box = false;
  contents_bounding_box_ = CFX_RectF();
  size_t current_piece_start = 0;
  float current_line_start = 0;

  auto iter = pdfium::MakeUnique<CFDE_TextEditEngine::Iterator>(this);
  while (!iter->IsEOF(false)) {
    iter->Next(false);

    CFX_BreakType break_status = text_break_.AppendChar(
        password_mode_ ? password_alias_ : iter->GetChar());
    if (iter->IsEOF(false) && CFX_BreakTypeNoneOrPiece(break_status))
      break_status = text_break_.EndBreak(CFX_BreakType::Paragraph);

    if (CFX_BreakTypeNoneOrPiece(break_status))
      continue;
    int32_t piece_count = text_break_.CountBreakPieces();
    for (int32_t i = 0; i < piece_count; ++i) {
      const CFX_BreakPiece* piece = text_break_.GetBreakPieceUnstable(i);

      FDE_TEXTEDITPIECE txtEdtPiece;
      memset(&txtEdtPiece, 0, sizeof(FDE_TEXTEDITPIECE));

      txtEdtPiece.nBidiLevel = piece->m_iBidiLevel;
      txtEdtPiece.nCount = piece->GetLength();
      txtEdtPiece.nStart = current_piece_start;
      txtEdtPiece.dwCharStyles = piece->m_dwCharStyles;
      if (FX_IsOdd(piece->m_iBidiLevel))
        txtEdtPiece.dwCharStyles |= FX_TXTCHARSTYLE_OddBidiLevel;

      txtEdtPiece.rtPiece.left = piece->m_iStartPos / 20000.0f;
      txtEdtPiece.rtPiece.top = current_line_start;
      txtEdtPiece.rtPiece.width = piece->m_iWidth / 20000.0f;
      txtEdtPiece.rtPiece.height = line_spacing_;
      text_piece_info_.push_back(txtEdtPiece);

      if (initialized_bounding_box) {
        contents_bounding_box_.Union(txtEdtPiece.rtPiece);
      } else {
        contents_bounding_box_ = txtEdtPiece.rtPiece;
        initialized_bounding_box = true;
      }

      current_piece_start += txtEdtPiece.nCount;
      for (int32_t k = 0; k < txtEdtPiece.nCount; ++k)
        char_widths_.push_back(piece->GetChar(k)->m_iCharWidth);
    }

    current_line_start += line_spacing_;
    text_break_.ClearBreakPieces();
  }

  float delta = 0.0;
  bool bounds_smaller = contents_bounding_box_.width < available_width_;
  if (IsAlignedRight() && bounds_smaller) {
    delta = available_width_ - contents_bounding_box_.width;
  } else if (IsAlignedCenter() && bounds_smaller) {
    delta = (available_width_ - contents_bounding_box_.width) / 2.0f;
  }

  if (delta != 0.0) {
    float offset = delta - contents_bounding_box_.left;
    for (auto& info : text_piece_info_)
      info.rtPiece.Offset(offset, 0.0f);
    contents_bounding_box_.Offset(offset, 0.0f);
  }

  // Shrink the last piece down to the font_size.
  contents_bounding_box_.height -= line_spacing_ - font_size_;
  text_piece_info_.back().rtPiece.height = font_size_;
}

std::pair<int32_t, CFX_RectF> CFDE_TextEditEngine::GetCharacterInfo(
    int32_t start_idx) {
  ASSERT(start_idx >= 0);
  ASSERT(static_cast<size_t>(start_idx) <= text_length_);

  // Make sure the current available data is fresh.
  Layout();

  auto it = text_piece_info_.begin();
  for (; it != text_piece_info_.end(); ++it) {
    if (it->nStart <= start_idx && start_idx < it->nStart + it->nCount)
      break;
  }
  if (it == text_piece_info_.end()) {
    NOTREACHED();
    return {0, CFX_RectF()};
  }

  return {it->nBidiLevel, GetCharRects(*it)[start_idx - it->nStart]};
}

std::vector<CFX_RectF> CFDE_TextEditEngine::GetCharacterRectsInRange(
    int32_t start_idx,
    int32_t count) {
  // Make sure the current available data is fresh.
  Layout();

  auto it = text_piece_info_.begin();
  for (; it != text_piece_info_.end(); ++it) {
    if (it->nStart <= start_idx && start_idx < it->nStart + it->nCount)
      break;
  }
  if (it == text_piece_info_.end())
    return std::vector<CFX_RectF>();

  int32_t end_idx = start_idx + count - 1;
  std::vector<CFX_RectF> rects;
  while (it != text_piece_info_.end()) {
    // If we end inside the current piece, extract what we need and we're done.
    if (it->nStart <= end_idx && end_idx < it->nStart + it->nCount) {
      std::vector<CFX_RectF> arr = GetCharRects(*it);
      CFX_RectF piece = arr[0];
      piece.Union(arr[end_idx - it->nStart]);
      rects.push_back(piece);
      break;
    }
    rects.push_back(it->rtPiece);
    ++it;
  }

  return rects;
}

std::pair<size_t, size_t> CFDE_TextEditEngine::BoundsForWordAt(
    size_t idx) const {
  if (idx > text_length_)
    return {0, 0};

  CFDE_TextEditEngine::Iterator iter(this);
  iter.SetAt(idx);

  size_t start_idx = iter.FindNextBreakPos(true);
  size_t end_idx = iter.FindNextBreakPos(false);
  return {start_idx, end_idx - start_idx + 1};
}

CFDE_TextEditEngine::Iterator::Iterator(const CFDE_TextEditEngine* engine)
    : engine_(engine), current_position_(-1) {}

CFDE_TextEditEngine::Iterator::~Iterator() {}

void CFDE_TextEditEngine::Iterator::Next(bool bPrev) {
  if (bPrev && current_position_ == -1)
    return;
  if (!bPrev && current_position_ > -1 &&
      static_cast<size_t>(current_position_) == engine_->GetLength()) {
    return;
  }

  if (bPrev)
    --current_position_;
  else
    ++current_position_;
}

wchar_t CFDE_TextEditEngine::Iterator::GetChar() const {
  return engine_->GetChar(current_position_);
}

void CFDE_TextEditEngine::Iterator::SetAt(size_t nIndex) {
  if (static_cast<size_t>(nIndex) >= engine_->GetLength())
    current_position_ = engine_->GetLength();
  else
    current_position_ = nIndex;
}

bool CFDE_TextEditEngine::Iterator::IsEOF(bool bPrev) const {
  return bPrev ? current_position_ == -1
               : current_position_ > -1 &&
                     static_cast<size_t>(current_position_) ==
                         engine_->GetLength();
}

size_t CFDE_TextEditEngine::Iterator::FindNextBreakPos(bool bPrev) {
  if (IsEOF(bPrev))
    return current_position_ > -1 ? current_position_ : 0;

  WordBreakProperty ePreType = WordBreakProperty::kNone;
  if (!IsEOF(!bPrev)) {
    Next(!bPrev);
    ePreType = GetWordBreakProperty(GetChar());
    Next(bPrev);
  }

  WordBreakProperty eCurType = GetWordBreakProperty(GetChar());
  bool bFirst = true;
  while (!IsEOF(bPrev)) {
    Next(bPrev);

    WordBreakProperty eNextType = GetWordBreakProperty(GetChar());
    bool wBreak = CheckStateChangeForWordBreak(eCurType, eNextType);
    if (wBreak) {
      if (IsEOF(bPrev)) {
        Next(!bPrev);
        break;
      }
      if (bFirst) {
        int32_t nFlags = GetBreakFlagsFor(eCurType, eNextType);
        if (nFlags > 0) {
          if (BreakFlagsChanged(nFlags, ePreType)) {
            Next(!bPrev);
            break;
          }
          Next(bPrev);
          wBreak = false;
        }
      }
      if (wBreak) {
        int32_t nFlags = GetBreakFlagsFor(eNextType, eCurType);
        if (nFlags <= 0) {
          Next(!bPrev);
          break;
        }

        Next(bPrev);
        eNextType = GetWordBreakProperty(GetChar());
        if (BreakFlagsChanged(nFlags, eNextType)) {
          Next(!bPrev);
          Next(!bPrev);
          break;
        }
      }
    }
    eCurType = eNextType;
    bFirst = false;
  }
  return current_position_ > -1 ? current_position_ : 0;
}
