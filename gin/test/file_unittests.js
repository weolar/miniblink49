// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

define([
    "gin/test/expect",
    "file"
  ], function(expect, file) {

  function isString(x) {
    return toString.call(x) === '[object String]'
  }

  var rootDir = file.getSourceRootDirectory();
  expect(isString(rootDir)).toBeTruthy();

  var noArgsNull = file.getFilesInDirectory();
  expect(noArgsNull).toBeNull();

  var files = file.getFilesInDirectory(rootDir);
  expect(Array.isArray(files)).toBeTruthy();

  var nsdNull = file.getFilesInDirectory(rootDir + "/no_such_dir");
  expect(nsdNull).toBeNull();

  var owners = file.readFileToString(rootDir + "/OWNERS");
  expect(isString(owners)).toBeTruthy();
  expect(owners.length).toBeGreaterThan(0);

  noArgsNull = file.readFileToString();
  expect(noArgsNull).toBeNull();

  var nsfNull = file.readFileToString(rootDir + "/no_such_file");
  expect(nsfNull).toBeNull();

  this.result = "PASS";
});
