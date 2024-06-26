// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_simple_parser.h"

#include "core/fpdfapi/parser/fpdf_parser_utility.h"

CPDF_SimpleParser::CPDF_SimpleParser(pdfium::span<const uint8_t> input)
    : data_(input) {}

CPDF_SimpleParser::~CPDF_SimpleParser() = default;

ByteStringView CPDF_SimpleParser::GetWord() {
  uint8_t ch;

  // Skip whitespace and comment lines.
  while (1) {
    if (data_.size() <= cur_pos_)
      return ByteStringView();

    ch = data_[cur_pos_++];
    while (PDFCharIsWhitespace(ch)) {
      if (data_.size() <= cur_pos_)
        return ByteStringView();
      ch = data_[cur_pos_++];
    }

    if (ch != '%')
      break;

    while (1) {
      if (data_.size() <= cur_pos_)
        return ByteStringView();

      ch = data_[cur_pos_++];
      if (PDFCharIsLineEnding(ch))
        break;
    }
  }

  uint8_t dwSize = 0;
  uint32_t start_pos = cur_pos_ - 1;
  if (PDFCharIsDelimiter(ch)) {
    // Find names
    if (ch == '/') {
      while (1) {
        if (data_.size() <= cur_pos_)
          break;

        ch = data_[cur_pos_++];
        if (!PDFCharIsOther(ch) && !PDFCharIsNumeric(ch)) {
          cur_pos_--;
          dwSize = cur_pos_ - start_pos;
          break;
        }
      }
      return data_.subspan(start_pos, dwSize);
    }

    dwSize = 1;
    if (ch == '<') {
      if (data_.size() <= cur_pos_)
        return data_.subspan(start_pos, dwSize);

      ch = data_[cur_pos_++];
      if (ch == '<') {
        dwSize = 2;
      } else {
        while (cur_pos_ < data_.size() && data_[cur_pos_] != '>')
          cur_pos_++;

        if (cur_pos_ < data_.size())
          cur_pos_++;

        dwSize = cur_pos_ - start_pos;
      }
    } else if (ch == '>') {
      if (data_.size() <= cur_pos_)
        return data_.subspan(start_pos, dwSize);

      ch = data_[cur_pos_++];
      if (ch == '>')
        dwSize = 2;
      else
        cur_pos_--;
    } else if (ch == '(') {
      int level = 1;
      while (cur_pos_ < data_.size()) {
        if (data_[cur_pos_] == ')') {
          level--;
          if (level == 0)
            break;
        }

        if (data_[cur_pos_] == '\\') {
          if (data_.size() <= cur_pos_)
            break;

          cur_pos_++;
        } else if (data_[cur_pos_] == '(') {
          level++;
        }
        if (data_.size() <= cur_pos_)
          break;

        cur_pos_++;
      }
      if (cur_pos_ < data_.size())
        cur_pos_++;

      dwSize = cur_pos_ - start_pos;
    }
    return data_.subspan(start_pos, dwSize);
  }

  dwSize = 1;
  while (cur_pos_ < data_.size()) {
    ch = data_[cur_pos_++];

    if (PDFCharIsDelimiter(ch) || PDFCharIsWhitespace(ch)) {
      cur_pos_--;
      break;
    }
    dwSize++;
  }
  return data_.subspan(start_pos, dwSize);
}
