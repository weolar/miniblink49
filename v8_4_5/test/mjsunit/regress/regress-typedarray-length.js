// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

var a = new Int32Array(100);
a.__proto__ = null;

function get(a) {
  return a.length;
}

assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(undefined, get(a));

get = function(a) {
  return a.byteLength;
}

assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(undefined, get(a));

get = function(a) {
  return a.byteOffset;
}

assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
assertEquals(undefined, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(undefined, get(a));

(function() {
  "use strict";

  class MyTypedArray extends Int32Array {
    get length() {
      return "length";
    }
  }

  a = new MyTypedArray();

  get = function(a) {
    return a.length;
  }

  assertEquals("length", get(a));
  assertEquals("length", get(a));
  assertEquals("length", get(a));
  %OptimizeFunctionOnNextCall(get);
  assertEquals("length", get(a));

  a.__proto__ = null;

  get = function(a) {
    return a.length;
  }

  assertEquals(undefined, get(a));
  assertEquals(undefined, get(a));
  assertEquals(undefined, get(a));
  %OptimizeFunctionOnNextCall(get);
  assertEquals(undefined, get(a));
})();

// Ensure we cannot delete length, byteOffset, byteLength.
assertTrue(Int32Array.prototype.hasOwnProperty("length"));
assertTrue(Int32Array.prototype.hasOwnProperty("byteOffset"));
assertTrue(Int32Array.prototype.hasOwnProperty("byteLength"));
assertFalse(delete Int32Array.prototype.length);
assertFalse(delete Int32Array.prototype.byteOffset);
assertFalse(delete Int32Array.prototype.byteLength);

a = new Int32Array(100);

get = function(a) {
  return a.length;
}

assertEquals(100, get(a));
assertEquals(100, get(a));
assertEquals(100, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(100, get(a));

get = function(a) {
  return a.byteLength;
}

assertEquals(400, get(a));
assertEquals(400, get(a));
assertEquals(400, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(400, get(a));

get = function(a) {
  return a.byteOffset;
}

assertEquals(0, get(a));
assertEquals(0, get(a));
assertEquals(0, get(a));
%OptimizeFunctionOnNextCall(get);
assertEquals(0, get(a));
