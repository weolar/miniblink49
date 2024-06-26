// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/cfx_widetextbuf.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace fxcrt {

TEST(WideTextBuf, EmptyBuf) {
  CFX_WideTextBuf wtb;
  EXPECT_EQ(nullptr, wtb.GetBuffer());
  EXPECT_TRUE(wtb.AsStringView().IsEmpty());
  EXPECT_TRUE(wtb.MakeString().IsEmpty());
}

TEST(WideTextBuf, OperatorLtLt) {
  CFX_WideTextBuf wtb;
  wtb << 42 << 3.14 << "clams" << L"\u208c\u208e";
  EXPECT_TRUE(wtb.MakeString() == L"423.14clams\u208c\u208e");
}

TEST(WideTextBuf, Deletion) {
  CFX_WideTextBuf wtb;
  wtb << L"ABCDEFG";
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII("ABCDEFG"));

  wtb.Delete(1, 3);
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII("AEFG"));

  wtb.Delete(1, 0);
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII("AEFG"));

  wtb.Delete(0, 2);
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII("FG"));

  wtb.Delete(0, 2);
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII(""));

  wtb.Delete(0, 0);
  EXPECT_TRUE(wtb.AsStringView().EqualsASCII(""));
}

}  // namespace fxcrt
