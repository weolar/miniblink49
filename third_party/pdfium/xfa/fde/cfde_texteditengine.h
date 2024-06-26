// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_TEXTEDITENGINE_H_
#define XFA_FDE_CFDE_TEXTEDITENGINE_H_

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/fx_dib.h"
#include "xfa/fgas/layout/cfx_txtbreak.h"

class CFGAS_GEFont;

struct FDE_TEXTEDITPIECE {
  FDE_TEXTEDITPIECE();
  FDE_TEXTEDITPIECE(const FDE_TEXTEDITPIECE& that);
  ~FDE_TEXTEDITPIECE();

  CFX_RectF rtPiece;
  int32_t nStart = 0;
  int32_t nCount = 0;
  int32_t nBidiLevel = 0;
  uint32_t dwCharStyles = 0;
};

inline FDE_TEXTEDITPIECE::FDE_TEXTEDITPIECE() = default;
inline FDE_TEXTEDITPIECE::FDE_TEXTEDITPIECE(const FDE_TEXTEDITPIECE& that) =
    default;
inline FDE_TEXTEDITPIECE::~FDE_TEXTEDITPIECE() = default;

class CFDE_TextEditEngine : public CFX_TxtBreak::Engine {
 public:
  class Iterator {
   public:
    explicit Iterator(const CFDE_TextEditEngine* engine);
    ~Iterator();

    void Next(bool bPrev);
    wchar_t GetChar() const;
    void SetAt(size_t nIndex);
    size_t FindNextBreakPos(bool bPrev);
    bool IsEOF(bool bPrev) const;

   private:
    UnownedPtr<const CFDE_TextEditEngine> engine_;
    int32_t current_position_;
  };

  class Operation {
   public:
    virtual ~Operation() = default;
    virtual void Redo() const = 0;
    virtual void Undo() const = 0;
  };

  struct TextChange {
    WideString text;
    WideString previous_text;
    size_t selection_start;
    size_t selection_end;
    bool cancelled;
  };

  class Delegate {
   public:
    virtual ~Delegate() = default;
    virtual void NotifyTextFull() = 0;
    virtual void OnCaretChanged() = 0;
    virtual void OnTextWillChange(TextChange* change) = 0;
    virtual void OnTextChanged() = 0;
    virtual void OnSelChanged() = 0;
    virtual bool OnValidate(const WideString& wsText) = 0;
    virtual void SetScrollOffset(float fScrollOffset) = 0;
  };

  enum class RecordOperation { kInsertRecord, kSkipRecord, kSkipNotify };

  CFDE_TextEditEngine();
  ~CFDE_TextEditEngine() override;

  // CFX_TxtBreak::Engine:
  wchar_t GetChar(size_t idx) const override;
  size_t GetWidthOfChar(size_t idx) override;

  void SetDelegate(Delegate* delegate) { delegate_ = delegate; }
  void Clear();

  void Insert(size_t idx,
              const WideString& text,
              RecordOperation add_operation = RecordOperation::kInsertRecord);
  WideString Delete(
      size_t start_idx,
      size_t length,
      RecordOperation add_operation = RecordOperation::kInsertRecord);
  WideString GetText() const;
  size_t GetLength() const;

  // Non-const so we can force a layout.
  CFX_RectF GetContentsBoundingBox();
  void SetAvailableWidth(size_t width);

  void SetFont(RetainPtr<CFGAS_GEFont> font);
  RetainPtr<CFGAS_GEFont> GetFont() const;
  void SetFontSize(float size);
  float GetFontSize() const { return font_size_; }
  void SetFontColor(FX_ARGB color) { font_color_ = color; }
  FX_ARGB GetFontColor() const { return font_color_; }
  float GetFontAscent() const;

  void SetAlignment(uint32_t alignment);
  float GetLineSpace() const { return line_spacing_; }
  void SetLineSpace(float space) { line_spacing_ = space; }
  void SetAliasChar(wchar_t alias) { password_alias_ = alias; }
  void SetHasCharacterLimit(bool limit);
  void SetCharacterLimit(size_t limit);
  void SetCombText(bool enable);
  void SetTabWidth(float width);
  void SetVisibleLineCount(size_t lines);

  void EnableValidation(bool val) { validation_enabled_ = val; }
  void EnablePasswordMode(bool val) { password_mode_ = val; }
  void EnableMultiLine(bool val);
  void EnableLineWrap(bool val);
  void LimitHorizontalScroll(bool val);
  void LimitVerticalScroll(bool val);

  bool CanUndo() const;
  bool CanRedo() const;
  bool Redo();
  bool Undo();
  void ClearOperationRecords();

  // This is not const it can trigger a |Layout|.
  size_t GetIndexBefore(size_t pos);
  size_t GetIndexLeft(size_t pos) const;
  size_t GetIndexRight(size_t pos) const;
  size_t GetIndexUp(size_t pos) const;
  size_t GetIndexDown(size_t pos) const;
  size_t GetIndexAtStartOfLine(size_t pos) const;
  size_t GetIndexAtEndOfLine(size_t pos) const;

  void SelectAll();
  void SetSelection(size_t start_idx, size_t count);
  void ClearSelection();
  bool HasSelection() const { return has_selection_; }
  // Returns <start_idx, count> of the selection.
  std::pair<size_t, size_t> GetSelection() const {
    return {selection_.start_idx, selection_.count};
  }
  WideString GetSelectedText() const;
  WideString DeleteSelectedText(
      RecordOperation add_operation = RecordOperation::kInsertRecord);
  void ReplaceSelectedText(const WideString& str);

  void Layout();

  // Non-const so we can force a Layout() if needed.
  size_t GetIndexForPoint(const CFX_PointF& point);
  // <start_idx, count>
  std::pair<size_t, size_t> BoundsForWordAt(size_t idx) const;

  // Returns <bidi level, character rect>
  std::pair<int32_t, CFX_RectF> GetCharacterInfo(int32_t start_idx);
  std::vector<CFX_RectF> GetCharacterRectsInRange(int32_t start_idx,
                                                  int32_t count);

  const std::vector<FDE_TEXTEDITPIECE>& GetTextPieces() {
    // Force a layout if needed.
    Layout();
    return text_piece_info_;
  }

  std::vector<FXTEXT_CHARPOS> GetDisplayPos(const FDE_TEXTEDITPIECE& info);

  void SetMaxEditOperationsForTesting(size_t max);

 private:
  void SetCombTextWidth();
  void AdjustGap(size_t idx, size_t length);
  void RebuildPieces();
  size_t CountCharsExceedingSize(const WideString& str, size_t num_to_check);
  void AddOperationRecord(std::unique_ptr<Operation> op);

  bool IsAlignedRight() const {
    return !!(character_alignment_ & CFX_TxtLineAlignment_Left);
  }

  bool IsAlignedCenter() const {
    return !!(character_alignment_ & CFX_TxtLineAlignment_Center);
  }
  std::vector<CFX_RectF> GetCharRects(const FDE_TEXTEDITPIECE& piece);

  struct Selection {
    size_t start_idx;
    size_t count;
  };

  CFX_RectF contents_bounding_box_;
  UnownedPtr<Delegate> delegate_;
  std::vector<FDE_TEXTEDITPIECE> text_piece_info_;
  std::vector<size_t> char_widths_;
  CFX_TxtBreak text_break_;
  RetainPtr<CFGAS_GEFont> font_;
  FX_ARGB font_color_;
  float font_size_;
  float line_spacing_;
  std::vector<WideString::CharType> content_;
  size_t text_length_;
  size_t gap_position_;
  size_t gap_size_;
  size_t available_width_;
  size_t character_limit_;
  size_t visible_line_count_;
  // Ring buffer of edit operations
  std::vector<std::unique_ptr<Operation>> operation_buffer_;
  // Next edit operation to undo.
  size_t next_operation_index_to_undo_;
  // Next index to insert an edit operation into.
  size_t next_operation_index_to_insert_;
  size_t max_edit_operations_;
  uint32_t character_alignment_;
  bool has_character_limit_;
  bool is_comb_text_;
  bool is_dirty_;
  bool validation_enabled_;
  bool is_multiline_;
  bool is_linewrap_enabled_;
  bool limit_horizontal_area_;
  bool limit_vertical_area_;
  bool password_mode_;
  wchar_t password_alias_;
  bool has_selection_;
  Selection selection_;
};

#endif  // XFA_FDE_CFDE_TEXTEDITENGINE_H_
