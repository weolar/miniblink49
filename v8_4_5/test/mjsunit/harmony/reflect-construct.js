// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --harmony-reflect


(function testReflectConstructArity() {
  assertEquals(2, Reflect.construct.length);
})();


(function testReflectConstructNonConstructor() {
  assertThrows(function() {
    new Reflect.construct(function(){}, []);
  }, TypeError);
})();


(function testReflectConstructBasic() {
  function Constructor() { "use strict"; }
  assertInstanceof(Reflect.construct(Constructor, []), Constructor);
})();


(function testReflectConstructBasicSloppy() {
  function Constructor() {}
  assertInstanceof(Reflect.construct(Constructor, []), Constructor);
})();


(function testReflectConstructReturnSomethingElseStrict() {
  var R = {};
  function Constructor() { "use strict"; return R; }
  assertSame(R, Reflect.construct(Constructor, []));
})();


(function testReflectConstructReturnSomethingElseSloppy() {
  var R = {};
  function Constructor() { return R; }
  assertSame(R, Reflect.construct(Constructor, []));
})();


(function testReflectConstructNewTargetStrict() {
  "use strict";
  function Constructor() { this[9] = 1; }
  var O = Reflect.construct(Constructor, [], Array);
  assertEquals(1, O[9]);
  // Ordinary object with Array.prototype --- no exotic Array magic
  assertFalse(Array.isArray(O));
  assertEquals(0, O.length);
  assertSame(Array.prototype, Object.getPrototypeOf(O));
})();


(function testReflectConstructNewTargetSloppy() {
  function Constructor() { this[9] = 1; }
  var O = Reflect.construct(Constructor, [], Array);
  assertEquals(1, O[9]);
  // Ordinary object with Array.prototype --- no exotic Array magic
  assertFalse(Array.isArray(O));
  assertEquals(0, O.length);
  assertSame(Array.prototype, Object.getPrototypeOf(O));
})();


(function testReflectConstructNewTargetStrict2() {
  "use strict";
  function Constructor() { this[9] = 1; }
  Constructor.prototype.add = function(x) {
    this[this.length] = x; return this;
  }
  var O = Reflect.construct(Array, [1, 2, 3], Constructor);
  // Exotic Array object with Constructor.prototype
  assertTrue(Array.isArray(O));
  assertSame(Constructor.prototype, Object.getPrototypeOf(O));
  assertFalse(O instanceof Array);
  assertEquals(3, O.length);
  assertEquals(undefined, O[9]);
  assertSame(O, O.add(4));
  assertEquals(4, O.length);
  assertEquals(4, O[3]);
})();


(function testReflectConstructNewTargetSloppy2() {
  function Constructor() { this[9] = 1; }
  Constructor.prototype.add = function(x) {
    this[this.length] = x; return this;
  }
  var O = Reflect.construct(Array, [1, 2, 3], Constructor);
  // Exotic Array object with Constructor.prototype
  assertTrue(Array.isArray(O));
  assertSame(Constructor.prototype, Object.getPrototypeOf(O));
  assertFalse(O instanceof Array);
  assertEquals(3, O.length);
  assertEquals(undefined, O[9]);
  assertSame(O, O.add(4));
  assertEquals(4, O.length);
  assertEquals(4, O[3]);
})();


(function testReflectConstructNewTargetStrict3() {
  "use strict";
  function A() {}
  function B() {}
  var O = Reflect.construct(A, [], B);
  // TODO(caitp): bug: newTarget prototype is not used if it is not
  // explicitly set.
  //assertSame(B.prototype, Object.getPrototypeOf(O));
})();


(function testReflectConstructNewTargetSloppy3() {
  function A() {}
  function B() {}
  var O = Reflect.construct(A, [], B);
  // TODO(caitp): bug: newTarget prototype is not used if it is not
  // explicitly set.
  //assertSame(B.prototype, Object.getPrototypeOf(O));
})();


(function testAppliedArgumentsLength() {
  function lengthStrict() { 'use strict'; this.a = arguments.length; }
  function lengthSloppy() { this.a = arguments.length; }

  assertEquals(0, Reflect.construct(lengthStrict, []).a);
  assertEquals(0, Reflect.construct(lengthSloppy, []).a);
  assertEquals(0, Reflect.construct(lengthStrict, {}).a);
  assertEquals(0, Reflect.construct(lengthSloppy, {}).a);

  for (var i = 0; i < 256; ++i) {
    assertEquals(i, Reflect.construct(lengthStrict, new Array(i)).a);
    assertEquals(i, Reflect.construct(lengthSloppy, new Array(i)).a);
    assertEquals(i, Reflect.construct(lengthStrict, { length: i }).a);
    assertEquals(i, Reflect.construct(lengthSloppy, { length: i }).a);
  }
})();


(function testAppliedArgumentsLengthThrows() {
  function noopStrict() { 'use strict'; }
  function noopSloppy() { }
  function MyError() {}

  var argsList = {};
  Object.defineProperty(argsList, "length", {
    get: function() { throw new MyError(); }
  });

  assertThrows(function() {
    Reflect.construct(noopStrict, argsList);
  }, MyError);

  assertThrows(function() {
    Reflect.construct(noopSloppy, argsList);
  }, MyError);
})();


(function testAppliedArgumentsElementThrows() {
  function noopStrict() { 'use strict'; }
  function noopSloppy() { }
  function MyError() {}

  var argsList = { length: 1 };
  Object.defineProperty(argsList, "0", {
    get: function() { throw new MyError(); }
  });

  assertThrows(function() {
    Reflect.construct(noopStrict, argsList);
  }, MyError);

  assertThrows(function() {
    Reflect.construct(noopSloppy, argsList);
  }, MyError);
})();


(function testAppliedNonFunctionStrict() {
  'use strict';
  assertThrows(function() { Reflect.construct(void 0, []); }, TypeError);
  assertThrows(function() { Reflect.construct(null, []); }, TypeError);
  assertThrows(function() { Reflect.construct(123, []); }, TypeError);
  assertThrows(function() { Reflect.construct("str", []); }, TypeError);
  assertThrows(function() { Reflect.construct(Symbol("x"), []); }, TypeError);
  assertThrows(function() { Reflect.construct(/123/, []); }, TypeError);
  assertThrows(function() { Reflect.construct(NaN, []); }, TypeError);
  assertThrows(function() { Reflect.construct({}, []); }, TypeError);
  assertThrows(function() { Reflect.construct([], []); }, TypeError);
})();


(function testAppliedNonFunctionSloppy() {
  assertThrows(function() { Reflect.construct(void 0, []); }, TypeError);
  assertThrows(function() { Reflect.construct(null, []); }, TypeError);
  assertThrows(function() { Reflect.construct(123, []); }, TypeError);
  assertThrows(function() { Reflect.construct("str", []); }, TypeError);
  assertThrows(function() { Reflect.construct(Symbol("x"), []); }, TypeError);
  assertThrows(function() { Reflect.construct(/123/, []); }, TypeError);
  assertThrows(function() { Reflect.construct(NaN, []); }, TypeError);
  assertThrows(function() { Reflect.construct({}, []); }, TypeError);
  assertThrows(function() { Reflect.construct([], []); }, TypeError);
})();


(function testAppliedArgumentsNonList() {
  function noopStrict() { 'use strict'; }
  function noopSloppy() {}
  assertThrows(function() { Reflect.construct(noopStrict, null); }, TypeError);
  assertThrows(function() { Reflect.construct(noopSloppy, null); }, TypeError);
  assertThrows(function() { Reflect.construct(noopStrict, 1); }, TypeError);
  assertThrows(function() { Reflect.construct(noopSloppy, 1); }, TypeError);
  assertThrows(function() { Reflect.construct(noopStrict, "BAD"); }, TypeError);
  assertThrows(function() { Reflect.construct(noopSloppy, "BAD"); }, TypeError);
  assertThrows(function() { Reflect.construct(noopStrict, true); }, TypeError);
  assertThrows(function() { Reflect.construct(noopSloppy, true); }, TypeError);
  var sym = Symbol("x");
  assertThrows(function() { Reflect.construct(noopStrict, sym); }, TypeError);
  assertThrows(function() { Reflect.construct(noopSloppy, sym); }, TypeError);
})();


(function testAppliedArgumentValue() {
  function firstStrict(a) { 'use strict'; this.a = a; }
  function firstSloppy(a) { this.a = a; }
  function lastStrict(a) {
    'use strict'; this.a = arguments[arguments.length - 1]; }
  function lastSloppy(a) { this.a = arguments[arguments.length - 1]; }
  function sumStrict() {
    'use strict';
    var sum = arguments[0];
    for (var i = 1; i < arguments.length; ++i) {
      sum += arguments[i];
    }
    this.a = sum;
  }
  function sumSloppy() {
    var sum = arguments[0];
    for (var i = 1; i < arguments.length; ++i) {
      sum += arguments[i];
    }
    this.a = sum;
  }

  assertEquals("OK!", Reflect.construct(firstStrict, ["OK!"]).a);
  assertEquals("OK!", Reflect.construct(firstSloppy, ["OK!"]).a);
  assertEquals("OK!", Reflect.construct(firstStrict,
                                        { 0: "OK!", length: 1 }).a);
  assertEquals("OK!", Reflect.construct(firstSloppy,
                                        { 0: "OK!", length: 1 }).a);
  assertEquals("OK!", Reflect.construct(lastStrict,
                                        [0, 1, 2, 3, 4, 5, 6, 7, 8, "OK!"]).a);
  assertEquals("OK!", Reflect.construct(lastSloppy,
                                        [0, 1, 2, 3, 4, 5, 6, 7, 8, "OK!"]).a);
  assertEquals("OK!", Reflect.construct(lastStrict,
                                        { 9: "OK!", length: 10 }).a);
  assertEquals("OK!", Reflect.construct(lastSloppy,
                                        { 9: "OK!", length: 10 }).a);
  assertEquals("TEST", Reflect.construct(sumStrict,
                                         ["T", "E", "S", "T"]).a);
  assertEquals("TEST!!", Reflect.construct(sumStrict,
                                           ["T", "E", "S", "T", "!", "!"]).a);
  assertEquals(10, Reflect.construct(sumStrict,
                                     { 0: 1, 1: 2, 2: 3, 3: 4, length: 4 }).a);
  assertEquals("TEST", Reflect.construct(sumSloppy,
                                         ["T", "E", "S", "T"]).a);
  assertEquals("TEST!!", Reflect.construct(sumSloppy,
                                           ["T", "E", "S", "T", "!", "!"]).a);
  assertEquals(10, Reflect.construct(sumSloppy,
                                     { 0: 1, 1: 2, 2: 3, 3: 4, length: 4 }).a);
})();
