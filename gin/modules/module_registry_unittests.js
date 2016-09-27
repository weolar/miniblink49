// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

define("module0", function() {
  return {
    "foo": "bar",
  }
});

define("module2", [
    "gtest",
    "module0",
    "module1"
  ], function(gtest, module0, module1) {
  gtest.expectEqual(module0.foo, "bar",
      "module0.foo is " + module0.foo);
  gtest.expectFalse(module0.bar,
      "module0.bar is " + module0.bar);
  gtest.expectEqual(module1.baz, "qux",
      "module1.baz is " + module1.baz);
  gtest.expectFalse(module1.qux,
      "module1.qux is " + module1.qux);

  this.result = "PASS";
});

define("module1", {
  "baz": "qux",
});
