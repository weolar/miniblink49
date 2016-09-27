// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

define(["gtest"], function(gtest) {
  gtest.expectTrue(true, "true is true");
  gtest.expectFalse(false, "false is false");
  gtest.expectTrue(this, "this is " + this);

  this.result = "PASS";
});
